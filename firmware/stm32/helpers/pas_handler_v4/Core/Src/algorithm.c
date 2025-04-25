/*
 * algorithm.c
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

#include "algorithm.h"

#include <math.h>

#include "adc.h"
#include "commands.h"
#include "gpio.h"
#include "helpers.h"
#include "logger.h"
#include "main.h"
#include "motor.h"
#include "sounds.h"
#include "stm32f4xx_hal.h"
#include "pid.h"
#include "speedometer.h"

float currentDutyCycle = 0.0f;

EMAFilter throttleFilter;
EMAFilter pasFilter;
const float throttleFilterAlpha = 0.0009f;
const float pasFilterAlpha = 0.01f;

static uint32_t lastPasPulseTime = 0;
static uint32_t pasPulses = 0;

float omegaPedals = 0.0f;
float targetOmegaWheel = 0.0f;
float targetVelocityWheelKmh = 0.0f;

float rawDutyCycle = 0.0f;

float dutyCycleOffset = ALGORITHM_DUTY_OFFSET_DEFAULT;
float dutyCycleFactor = ALGORITHM_DUTY_FACTOR_DEFAULT;

void algorithm_setDutyOffset(float offset) { dutyCycleOffset = offset; }

void algorithm_setDutyFactor(float factor) { dutyCycleFactor = factor; }

float algorithm_getDutyOffset() { return dutyCycleOffset; }

float algorithm_getDutyFactor() { return dutyCycleFactor; }

float limitDutyCycleIncrease(float targetDutyCycle) {
    if (targetDutyCycle - motor_getDutyCycle() > 3.0f)
        return motor_getDutyCycle() + 3.0f;
    return targetDutyCycle;
}

uint32_t getDutyCycleFromVWheelKmh(float x) {
    // f(x) = 0,0001227995637 x⁵ − 0,0061246087742 x⁴ + 0,1124919694247 x³ −
    // 0,8730248662225 x² + 3,9743492255457 x + 1,6789618357226
    if (x > 27.0f) return 0;

    float duty =
        ((((0.0001228f * x - 0.0061246f) * x + 0.1124920f) * x - 0.8730249f) *
             x +
         3.9743492f) *
            x +
        1.6789618f;

    duty = duty + dutyCycleOffset + (duty * duty * duty * dutyCycleFactor);

    if (duty < 0.0f) duty = 0.0f;
    if (duty > 100.0f) duty = 100.0f;

    return (uint32_t)duty;
}

void algorithm_handlePasPulse() {
    pasPulses++;

    uint32_t now = HAL_GetTick();

    uint32_t delta = now - lastPasPulseTime;
    if (delta < 5) return;  // debounce

    lastPasPulseTime = now;

    if (pasPulses <= 2)
        return;  // require at least 3 pulses if bike was stationary

    float deltaSeconds = (float)delta / 1000.0f;

    omegaPedals = ALGORITHM_PAS_MAGNET_ANGLE / deltaSeconds;
    targetOmegaWheel = omegaPedals * ALGORITHM_PEDAL_GEAR_RATIO;
    targetVelocityWheelKmh =
        targetOmegaWheel * R_WHEEL * 3.6f;  // [km/h]

    rawDutyCycle = getDutyCycleFromVWheelKmh(targetVelocityWheelKmh);
}

void algorithm_updateSoundEffect() {
    float rangeSize = 100.0f / 4.0f;
    float D_start = 0.0f;

    int rangeIndex = (currentDutyCycle - D_start) / rangeSize;
    if (rangeIndex > 3) rangeIndex = 3;

    float D_local = currentDutyCycle - (D_start + rangeIndex * rangeSize);

    int PSC_max = 209 + (rangeIndex * 30);
    int PSC_min = 111 + (rangeIndex * 10);

    TIM1->PSC = PSC_max - ((PSC_max - PSC_min) / rangeSize) * D_local;
}

void algorithm_init() {
    emaFilter_init(&throttleFilter, 0.0f, throttleFilterAlpha, 1);
    emaFilter_init(&pasFilter, 0.0f, pasFilterAlpha, 1);
}

void algorithm_pasProc() {
    uint32_t now = HAL_GetTick();

    if (now - lastPasPulseTime > ALGORITHM_PAS_INACTIVE_TIME_MS) {
        currentDutyCycle = 0.0f;
        motor_setDutyCycle(currentDutyCycle);
        emaFilter_reset(&pasFilter);
        pasPulses = 0;
        return;
    }

    currentDutyCycle = emaFilter_update(&pasFilter, rawDutyCycle);
    currentDutyCycle = limitDutyCycleIncrease(currentDutyCycle);
}


// TODO rename this garbage 

float last_v_kolo = 0.0f;
uint32_t lastTick = 0;
uint32_t lastNewPasProcTick = 0;
float filteredSpeedDrop = 0.0f;

//                         Kp    Ki    Kd    I  LE Max
PID motorWheelSpeedPID = { 1.0f, 0.5f, 0.0f, 0, 0, 100 };
PID adjustmentPID = {1.0, 0.2, 0.05, 0, 0, 2.0};   // PID agresji :))

#define PEDAL_SYNC_THRESHOLD 0.15f
#define FILTER_ALPHA 0.1f   

void algorithm_newPasProc() {
    uint32_t now = HAL_GetTick();

    if (now - lastPasPulseTime > ALGORITHM_PAS_INACTIVE_TIME_MS) {
        currentDutyCycle = 0.0f;
        motor_setDutyCycle(currentDutyCycle);
        pasPulses = 0;
        return;
    }

    uint32_t dt = now - lastTick;

    if (now - lastNewPasProcTick < 10) return; // proc every 10m
    lastNewPasProcTick = now;

    float v_kolo = speedometer_getWheelVelocityKmh();           // v kola bezposrednio mierzone
    float v_pedaly = targetVelocityWheelKmh;                    // v kola obliczone z PAS
    float v_silnik = speedometer_getMotorWheelVelocityKmh();    // v kola obliczone z silnika

    float delta_v_kolo = v_kolo - last_v_kolo;                 
    float speed_drop = delta_v_kolo / dt;                       // przyspieszenie 

    // Filtracja (EMA)
    filteredSpeedDrop = FILTER_ALPHA * speed_drop + (1 - FILTER_ALPHA) * filteredSpeedDrop;

    // TODO shouldnt it be just < threshold?
    if (fabsf(v_pedaly - v_kolo) < PEDAL_SYNC_THRESHOLD * v_kolo) {
        // nie powinno byc spadku predkosci
        float aggression = PID_Calculate(&adjustmentPID, 0.0f, filteredSpeedDrop);
        aggression = constrain(aggression, 0.0f, 2.0f);

        // adjust motor PID params
        float Kp_dyn = constrain(1.0f + aggression, 1.0f, 3.0f);
        float Ki_dyn = constrain(0.5f - aggression * 0.3f, 0.1f, 0.5f);

        PID_SetTunings(&motorWheelSpeedPID, Kp_dyn, Ki_dyn, motorWheelSpeedPID.Kd);

        // Now we can calculate PWM, motor speed 105% of wheel speed
        float target_speed = v_kolo * 1.05f;
        float pwm = PID_Calculate(&motorWheelSpeedPID, target_speed, v_silnik);
        pwm = constrain(pwm, 0.0f, 100.0f);

        last_v_kolo = v_kolo;
        lastTick = now;

        currentDutyCycle = pwm;
    } else { // pedals moving too slow
        motorWheelSpeedPID.integral = 0;
        motorWheelSpeedPID.last_error = 0;
        currentDutyCycle = 0.0f;
    }    
}

void algorithm_throttleProc() {
    if (adc_throttleVoltage < ALGORITHM_MIN_THROTTLE_VOLTAGE) {
        currentDutyCycle = 0.0f;
        motor_setDutyCycle(currentDutyCycle);
        motor_updateDutyCycle();
        TIM1->PSC = 168 - 1;
        emaFilter_reset(&throttleFilter);
        return;
    }

    float normalizedVoltage = (adc_throttleVoltage - ALGORITHM_MIN_THROTTLE_VOLTAGE) /
                              (2.55f - ALGORITHM_MIN_THROTTLE_VOLTAGE);
    normalizedVoltage = clamp(normalizedVoltage, 0.0f, 1.0f);

    rawDutyCycle = 100.0f * powf(normalizedVoltage, 2.5f); 

    currentDutyCycle = emaFilter_update(&throttleFilter, rawDutyCycle);
    currentDutyCycle = limitDutyCycleIncrease(currentDutyCycle);
}

void algorithm_proc() {
    if (throttleEnabled)
        algorithm_throttleProc();
    else if (pasEnabled)
        // algorithm_pasProc();
        algorithm_newPasProc();
    else
        currentDutyCycle = 0.0f;

    algorithm_updateSoundEffect();

    motor_setDutyCycle(currentDutyCycle);
    motor_updateDutyCycle();  // actually write duty cycle
}

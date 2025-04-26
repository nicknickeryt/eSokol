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
#include "pid.h"
#include "sounds.h"
#include "speedometer.h"
#include "stm32f4xx_hal.h"

bool algorithmEnabled = false;

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

float targetSpeedPercentage =
    ALGORITHM_TARGET_SPEED_PERCENTAGE_DEFAULT;  // 1.05 = 105%

void algorithm_setTargetSpeedPercentage(float value) {
    targetSpeedPercentage = value;
}

float algorithm_getTargetSpeedPercentage() { return targetSpeedPercentage; }

float limitDutyCycleIncrease(float targetDutyCycle) {
    if (targetDutyCycle - motor_getDutyCycle() > 5.0f)
        return motor_getDutyCycle() + 5.0f;
    return targetDutyCycle;
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
    targetVelocityWheelKmh = targetOmegaWheel * R_WHEEL * 3.6f;  // [km/h]
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

// TODO rename this garbage

float last_v_kolo = 0.0f;
uint32_t lastTick = 0;
uint32_t lastNewPasProcTick = 0;
float filteredSpeedDrop = 0.0f;

PID motorWheelSpeedPID = {0.4f, 0.07f, 0.04f,
                          0,    0,     100};  // TODO PID agresjI >:)

#define ALGORITHM_PEDAL_SYNC_THRESHOLD 0.3f  // is it ok? idk

void algorithm_newPasProc() {
    uint32_t now = HAL_GetTick();

    if (now - lastPasPulseTime > ALGORITHM_PAS_INACTIVE_TIME_MS) {
        currentDutyCycle = 0.0f;
        motor_setDutyCycle(currentDutyCycle);
        pasPulses = 0;
        PID_Reset(&motorWheelSpeedPID);
        return;
    }

    if (now - lastNewPasProcTick < 10) return;  // proc every 10ms
    lastNewPasProcTick = now;

    float v_kolo =
        speedometer_getWheelVelocityKmh();    // v kola bezposrednio mierzone
    float v_pedaly = targetVelocityWheelKmh;  // v kola obliczone z PAS
    float v_silnik =
        speedometer_getMotorWheelVelocityKmh();  // v kola obliczone z silnika

    if (fabsf(v_pedaly - v_kolo) < ALGORITHM_PEDAL_SYNC_THRESHOLD * v_kolo) {
        float target_speed =
            v_kolo * targetSpeedPercentage;  // motor speed 105% of wheel speed
        float rawDutyCycle =
            PID_Calculate(&motorWheelSpeedPID, target_speed, v_silnik);
        rawDutyCycle = constrain(rawDutyCycle, 0.0f, 30.0f);

        last_v_kolo = v_kolo;
        lastTick = now;

        currentDutyCycle = emaFilter_update(&pasFilter, rawDutyCycle);
        currentDutyCycle = limitDutyCycleIncrease(currentDutyCycle);
    } else {
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

    float normalizedVoltage =
        (adc_throttleVoltage - ALGORITHM_MIN_THROTTLE_VOLTAGE) /
        (2.55f - ALGORITHM_MIN_THROTTLE_VOLTAGE);
    normalizedVoltage = clamp(normalizedVoltage, 0.0f, 1.0f);

    rawDutyCycle = 100.0f * powf(normalizedVoltage, 2.5f);

    currentDutyCycle = emaFilter_update(&throttleFilter, rawDutyCycle);
    currentDutyCycle = limitDutyCycleIncrease(currentDutyCycle);
}

void algorithm_proc() {
    if (!algorithmEnabled) {
        motor_setDutyCycle(0.0f);
        motor_updateDutyCycle();
        algorithm_updateSoundEffect();
        return;
    }

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

void algorithm_toggleEnabled() { algorithmEnabled = !algorithmEnabled; }

bool algorithm_isEnabled() { return algorithmEnabled; }
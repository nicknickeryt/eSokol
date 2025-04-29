/*
 * algorithm.c
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

#include "algorithm.h"

#include <math.h>
#include <stdlib.h>

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

bool algorithmEnabled = true;

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

float targetSpeedPercentage = ALGORITHM_TARGET_SPEED_PERCENTAGE_DEFAULT;

// FEEDFORWARD
float lastFeedforwardDuty = 0.0f;
float feedforwardDuty = 0.0f;
const float feedforwardAlpha = 0.1f;

uint32_t getDutyCycleFromVWheelKmh(float x) {
    // f(x) = 0,0001227995637 x⁵ − 0,0061246087742 x⁴ + 0,1124919694247 x³ -
    // 0,8730248662225 x² + 3,9743492255457 x + 1,6789618357226
    if (x > 27.0f) return 0;

    float duty =
        ((((0.0001228f * x - 0.0061246f) * x + 0.1124920f) * x - 0.8730249f) *
             x +
         3.9743492f) *
            x +
        1.6789618f;

    // duty = duty + (duty * duty * 0.001f);

    if (duty < 0.0f) duty = 0.0f;
    if (duty > 100.0f) duty = 100.0f;

    return (uint32_t)duty;
}

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

char* pidTestStatusMessage;

void algorithm_init() {
    emaFilter_init(&throttleFilter, 0.0f, throttleFilterAlpha, 1);
    emaFilter_init(&pasFilter, 0.0f, pasFilterAlpha, 1);
    pidTestStatusMessage = (char*)malloc(50 * sizeof(char));
}

// TODO rename this garbage
uint32_t lastTick = 0;
uint32_t lastNewPasProcTick = 0;

uint32_t lastPidSendTick = 0;

PID motorWheelSpeedPID = {0.5f, 0.05f, 0.5f, 0, 0, 100};

void algorithm_newPasProc() {
    uint32_t now = HAL_GetTick();

    if (now - lastNewPasProcTick < 10) return;  // proc every 10ms

    lastNewPasProcTick = now;

    if (now - lastPasPulseTime > ALGORITHM_PAS_INACTIVE_TIME_MS) {
        currentDutyCycle = 0.0f;
        pasPulses = 0;
        PID_Reset(&motorWheelSpeedPID);
        emaFilter_reset(&pasFilter);
        return;
    }

    float v_pedaly = targetVelocityWheelKmh;
    float v_silnik = speedometer_getMotorWheelVelocityKmh();

    float target_speed = v_pedaly * targetSpeedPercentage;

    // calculate always!!
    float pidDuty = PID_Calculate(&motorWheelSpeedPID, target_speed, v_silnik);
    rawDutyCycle = pidDuty;  
    currentDutyCycle = pidDuty;
    rawDutyCycle = constrain(rawDutyCycle, 0.0f, 100.0f);

    if (now - lastPidSendTick >= 100) {
        int target = (int)(target_speed * 100.0f);    // np. 800
        int current = (int)(v_silnik * 100.0f);  // np. 362

        // Rozbij targetSpeed na 4 cyfry
        char t1 = (target / 1000) + '0';
        char t2 = ((target / 100) % 10) + '0';
        char t3 = ((target / 10) % 10) + '0';
        char t4 = (target % 10) + '0';

        // Rozbij currentSpeed na 4 cyfry
        char c1 = (current / 1000) + '0';
        char c2 = ((current / 100) % 10) + '0';
        char c3 = ((current / 10) % 10) + '0';
        char c4 = (current % 10) + '0';

        // Złóż wynikowy string
        sprintf(pidTestStatusMessage, "eskl_pid%c%c%c%c%c%c%c%c\r\n", t1, t2,
                t3, t4, c1, c2, c3, c4);

        logger_sendChar(pidTestStatusMessage);

        lastPidSendTick = now;
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

PID testPid = {0.5f, 0.05f, 0.5f, 0, 0, 100};
// PID testPid = {0.5f, 0.02f, 1.0f, 0, 0, 100}; // tez ok
// PID testPid = {0.5f, 0.02f, 0.0f, 0, 0, 100}; // wydaje mi sie ze fajne parametry

uint32_t testPidLastSwitchTick = 0;
uint32_t lastTestPidTick = 0;
bool testPidTarget8 = true;

void algorithm_pidTestProc() {
    uint32_t now = HAL_GetTick();

    // Obliczaj PID co 10 ms
    if (now - lastTestPidTick < 10) return;
    lastTestPidTick = now;

    // if (now - testPidLastSwitchTick >= 20000) {
    //     testPidLastSwitchTick = now;
    //     testPidTarget8 = !testPidTarget8;
    // }

    float currentSpeed = speedometer_getMotorWheelVelocityKmh();
    float targetSpeed = testPidTarget8 ? 7.0f : 0.0f;

    float pidDuty = PID_Calculate(&testPid, targetSpeed, currentSpeed);
    pidDuty = constrain(pidDuty, 0.0f, 100.0f);

    rawDutyCycle = pidDuty;
    currentDutyCycle = pidDuty;

    if (now - lastPidSendTick >= 100) {
        int target = (int)(targetSpeed * 100.0f);    // np. 800
        int current = (int)(currentSpeed * 100.0f);  // np. 362

        // Rozbij targetSpeed na 4 cyfry
        char t1 = (target / 1000) + '0';
        char t2 = ((target / 100) % 10) + '0';
        char t3 = ((target / 10) % 10) + '0';
        char t4 = (target % 10) + '0';

        // Rozbij currentSpeed na 4 cyfry
        char c1 = (current / 1000) + '0';
        char c2 = ((current / 100) % 10) + '0';
        char c3 = ((current / 10) % 10) + '0';
        char c4 = (current % 10) + '0';

        // Złóż wynikowy string
        sprintf(pidTestStatusMessage, "eskl_pid%c%c%c%c%c%c%c%c\r\n", t1, t2,
                t3, t4, c1, c2, c3, c4);
        logger_sendChar(pidTestStatusMessage);
        lastPidSendTick = now;
    }
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
        currentDutyCycle = 0.0f;
        // algorithm_pasProc();
        // algorithm_newPasProc();
        // algorithm_pidTestProc();
    else
        currentDutyCycle = 0.0f;

    algorithm_updateSoundEffect();

    motor_setDutyCycle(currentDutyCycle);
    motor_updateDutyCycle();  // actually write duty cycle
}

void algorithm_toggleEnabled() { algorithmEnabled = !algorithmEnabled; }

bool algorithm_isEnabled() { return algorithmEnabled; }
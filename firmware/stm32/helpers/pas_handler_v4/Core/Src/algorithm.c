/*
 * algorithm.c
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

#include "algorithm.h"
#include "adc.h"
#include "commands.h"
#include "gpio.h"
#include "helpers.h"
#include "logger.h"
#include "main.h"
#include "pas.h"
#include "sounds.h"

#include "stm32f4xx_hal.h"

#include <math.h>

float algorithm_eq_component = ALGORITHM_EQ_COMPONENT_DEFAULT;

float pasTimeS = 0.0;

float currentDutyCycle = 0.0f;
const float throttle_alpha = 0.001f;

void algorithm_proc() {
    if (!throttleEnabled) return;

    if (adc_throttleVoltage < MINIMUM_THROTTLE_VOLTAGE) {
        targetDutyCycle = 0.0f;
        currentDutyCycle = 0.0f;
        TIM1->PSC = 168 - 1;
    } else {
        float normalizedVoltage = (adc_throttleVoltage - MINIMUM_THROTTLE_VOLTAGE) / (2.55f - MINIMUM_THROTTLE_VOLTAGE);
        if (normalizedVoltage > 1.0f) normalizedVoltage = 1.0f;
        if (normalizedVoltage < 0.0f) normalizedVoltage = 0.0f;

        float rawDuty = 100.0f * powf(normalizedVoltage, 2.5f);  // 0-100%

        currentDutyCycle += throttle_alpha * (rawDuty - currentDutyCycle); // EMA

        if(currentDutyCycle - targetDutyCycle > 3.0f) currentDutyCycle = targetDutyCycle + 3.0f;

        targetDutyCycle = currentDutyCycle;

        // Sound effect
        float rangeSize = (2.55f - 0.9f) / 4.0f;
        float V_start = 0.9f;

        int rangeIndex = (adc_throttleVoltage - V_start) / rangeSize;
        float V_local = adc_throttleVoltage - (V_start + rangeIndex * rangeSize);

        int PSC_max = 209 + (rangeIndex * 30);
        int PSC_min = 111 + (rangeIndex * 10);

        TIM1->PSC = PSC_max - ((PSC_max - PSC_min) / rangeSize) * V_local;
    }

    updateDutyCycle();  // actually write duty cycle
}

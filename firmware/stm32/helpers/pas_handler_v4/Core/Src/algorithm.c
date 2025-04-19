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
#include "pas.h"
#include "sounds.h"
#include "stm32f4xx_hal.h"

float algorithm_eq_component = ALGORITHM_EQ_COMPONENT_DEFAULT;

float pasTimeS = 0.0;

float currentDutyCycle = 0.0f;
const float throttle_alpha = 0.0009f;

EMAFilter throttleFilter;

void algorithm_updateSoundEffect() {
    float rangeSize = 100.0f / 4.0f; 
    float D_start = 0.0f;

    int rangeIndex = (targetDutyCycle - D_start) / rangeSize;
    if (rangeIndex > 3) rangeIndex = 3;

    float D_local = targetDutyCycle - (D_start + rangeIndex * rangeSize);

    int PSC_max = 209 + (rangeIndex * 30);
    int PSC_min = 111 + (rangeIndex * 10);

    TIM1->PSC = PSC_max - ((PSC_max - PSC_min) / rangeSize) * D_local;
}

void algorithm_init() {
    emaFilter_init(&throttleFilter, 0.0f, throttle_alpha, 1);
}

void algorithm_throttleProc() {
    if (adc_throttleVoltage < MINIMUM_THROTTLE_VOLTAGE) {
        targetDutyCycle = 0.0f;
        currentDutyCycle = 0.0f;
        TIM1->PSC = 168 - 1;
        emaFilter_reset(&throttleFilter);
        return;
    }

    float normalizedVoltage = (adc_throttleVoltage - MINIMUM_THROTTLE_VOLTAGE) / (2.55f - MINIMUM_THROTTLE_VOLTAGE);
    normalizedVoltage = clamp(normalizedVoltage, 0.0f, 1.0f);

    float rawDuty = 100.0f * powf(normalizedVoltage, 2.5f);  // 0-100%

    currentDutyCycle = emaFilter_update(&throttleFilter, rawDuty);

    // Maksymalny przyrost
    if (currentDutyCycle - targetDutyCycle > 3.0f)
        currentDutyCycle = targetDutyCycle + 3.0f;

    targetDutyCycle = currentDutyCycle;
}
  
void algorithm_proc() {
    if (throttleEnabled) algorithm_throttleProc();
    // else if (pasEnabled) algorithm_pasProc();
    else targetDutyCycle = 0.0f;
    
    algorithm_updateSoundEffect();

    updateDutyCycle();  // actually write duty cycle
}

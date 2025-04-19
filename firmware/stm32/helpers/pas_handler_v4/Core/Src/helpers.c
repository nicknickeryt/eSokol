/*
 * helpers.c
 *
 * Contains UART and various other helpers
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

#include "helpers.h"

#include <stdbool.h>

#include "main.h"
#include "pas.h"

void pwm_toggle(TIM_HandleTypeDef *htim, bool state) {
    if (state) {
        resetDutyCycle();
        HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1);
    } else {
        resetDutyCycle();
        HAL_TIM_PWM_Stop(htim, TIM_CHANNEL_1);
    }
}

void emaFilter_init(EMAFilter *filter, float initialValue, float alpha,
                    uint32_t intervalMs) {
    filter->value = initialValue;
    filter->alpha = alpha;
    filter->lastUpdateMs = HAL_GetTick();
    filter->intervalMs = intervalMs;
}

float emaFilter_update(EMAFilter *filter, float target) {
    uint32_t now = HAL_GetTick();
    if (now - filter->lastUpdateMs >= filter->intervalMs) {
        filter->value += filter->alpha * (target - filter->value);
        filter->lastUpdateMs = now;
    }
    return filter->value;
}

void emaFilter_reset(EMAFilter *filter) { filter->value = 0.0f; }

float clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}
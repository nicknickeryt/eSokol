/*
 * helpers.h
 *
 * Contains UART and various other helpers
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

#ifndef __HELPERS_H
#define __HELPERS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "stm32f4xx_hal.h"

#define TIM_THROTTLE_LEDS &htim1
#define TIM_SOUND &htim3
#define PWM_ENABLED 1
#define PWM_DISABLED 0

// π rounded value // TODO: do we really need so many decimal places?
#define PI 3.141592

extern bool bike_bluetoothConnected;

extern uint16_t adc_batteryVoltage;

typedef struct {
    float value;
    float alpha;
    uint32_t lastUpdateMs;  // czas ostatniej aktualizacji
    uint32_t intervalMs;    // co ile ms liczyć EMA
} EMAFilter;

void emaFilter_init(EMAFilter* filter, float initialValue, float alpha,
                    uint32_t intervalMs);
float emaFilter_update(EMAFilter* filter, float target);
void emaFilter_reset(EMAFilter* filter);

void pwm_toggle(TIM_HandleTypeDef* htim, bool state);

float clamp(float value, float min, float max);

#endif /* __HELPERS_H */

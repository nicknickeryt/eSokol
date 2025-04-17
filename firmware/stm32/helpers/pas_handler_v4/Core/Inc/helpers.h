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

void bike_init(void);
void bike_proc(void);
void bike_handleGpioInterrupt(uint16_t GPIO_Pin);


void pwm_toggle(TIM_HandleTypeDef* htim, bool state);

#endif /* __HELPERS_H */

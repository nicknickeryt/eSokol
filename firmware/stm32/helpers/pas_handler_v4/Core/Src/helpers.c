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

void pwm_toggle(TIM_HandleTypeDef* htim, bool state) {
    if (state) {
        resetDutyCycle();
        HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1);
    } else {
        resetDutyCycle();
        HAL_TIM_PWM_Stop(htim, TIM_CHANNEL_1);
    }
}
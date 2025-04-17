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

#include "adc.h"
#include "ambientlight.h"
#include "animations.h"
#include "blinkers.h"
#include "commands.h"
#include "gpio.h"
#include "logger.h"
#include "main.h"
#include "pas.h"
#include "sounds.h"
#include "uart.h"
#include "speedometer.h"

bool bluetoothConnected = 0;

void bike_init() {
    HAL_UARTEx_ReceiveToIdle_IT(&huart1, rxBuffer, 16);

    lastPasResetTick = HAL_GetTick();

    TIM1->CCR1 = targetDutyCycle;  // OUT_PWM
    TIM1->CCR4 = 500;              // FRONT_COLD_PWM

    HAL_TIM_PWM_Start(TIM_THROTTLE_LEDS, TIM_CHANNEL_1);  // OUT_PWM
    HAL_TIM_PWM_Start(TIM_THROTTLE_LEDS, TIM_CHANNEL_4);  // FRONT_COLD_PWM
    HAL_TIM_PWM_Start(TIM_SOUND, TIM_CHANNEL_1);          // SOUND

    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)rawAdcValues, 4);

    animation_play(ANIM_STARTUP_PHASE1);

    if (gpio_read(BT_STATE_GPIO_Port, BT_STATE_Pin)) bluetoothConnected = true;

    initBlinkers();
    initStatusMessage();
    initVelocityBuffer();

    sound_play(SOUND_RUDOLF);

    TIM1->PSC = 168 - 1;
}

void togglePWM(TIM_HandleTypeDef* htim, bool state) {
    if (state) {
        resetDutyCycle();
        HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1);
    } else {
        resetDutyCycle();
        HAL_TIM_PWM_Stop(htim, TIM_CHANNEL_1);
    }
}
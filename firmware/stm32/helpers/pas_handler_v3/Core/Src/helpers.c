/*
 * helpers.c
 *
 * Contains UART and various other helpers
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */
#include <stdbool.h>

#include "main.h"
#include "helpers.h"
#include "uart.h"
#include "sounds.h"
#include "animations.h"
#include "gpio.h"

bool bluetoothConnected = 0;

void bikeInit()
{
	writePin(THR_SPORT_GPIO_Port, THR_SPORT_Pin, 1);
	lastPasResetTick = HAL_GetTick();

	TIM1->CCR1 = targetDutyCycle;
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

	playAnim(ANIM_STARTUP_PHASE1);
	writePin(GPIOB, GPIO_PIN_3, 0); // Throttle default disabled

	if (readPin(BT_STATE_GPIO_Port, BT_STATE_Pin))
		bluetoothConnected = true;

	playTone(2);
}

void togglePWM(TIM_HandleTypeDef *htim, bool state)
{
	if (state)
	{
		resetDutyCycle();
		HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1);
	}
	else
	{
		resetDutyCycle();
		HAL_TIM_PWM_Stop(htim, TIM_CHANNEL_1);
	}
}

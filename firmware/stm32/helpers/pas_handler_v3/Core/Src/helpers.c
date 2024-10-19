/*
 * helpers.c
 *
 * Contains UART and various other helpers
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

#include "helpers.h"
#include "main.h"
#include "uart.h"
#include <stdbool.h>

uint8_t pasCounter = 0;	  // counts rising edges of pas signal
int lastPasResetTick = 0; // last pas counter reset (at 12 counters)

float omegaPedals = 0.0;
float omegaWheel = 0.0;
float vWheel = 0.0;

float pasActive = 0;

float targetDutyCycle = 0.0; // PWM duty cycle

// TODO proper .h/macro!!!
float previousDutyCycle = 0.0; // Przechowuje poprzednią wartość duty cycle

// startupAnim helpers
bool shouldStartAnim = 0;
bool isAnimatingStartup = 0;

bool shouldConnectedAnim = 0;
bool shouldDisconnectedAnim = 0;
bool connected = 0;

uint32_t blink1STime = 0.0;			  // blink every 1s, TODO: magicDiodCompositio-like blink
uint32_t blinkDisconnectedTime = 0.0; // blink every 1s if disconnected

void bikeInit()
{
	lastPasResetTick = HAL_GetTick();

	TIM1->CCR1 = targetDutyCycle;
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

	startupAnim();
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0); // Throttle default disabled
}

void logDebugDegrees(float timeS)
{
	send_string("[DEBUG]: 30 stopni!\r\nczas: ");

	send_float(timeS);
	send_string("sekund\r\n");
}

void logDebugVWheel()
{
	send_string("vWheel: ");
	send_float(vWheel);
	send_string(" [m/s] ");
	send_float(vWheel * 3.6);
	send_string(" [km/h] \r\n");
}

void logDebugDutyCycle()
{
	send_string("dutyCycle: ");
	send_float(targetDutyCycle);
	send_string(" [%] \r\n");
	send_string("targetDutyCycle: ");
	send_float(targetDutyCycle);
	send_string(" [%] \r\n");
}

void logDebugInactive()
{
	send_string("[DEBUG]: ---- INACTIVE ----\r\n");
}

void updateDutyCycle(void)
{

	TIM1->CCR1 = (targetDutyCycle / 100.0) * DUTY_PWM_MAX_CCR1;
}

void resetPas(bool inactive)
{
	lastPasResetTick = HAL_GetTick();

	if (inactive)
	{
		pasActive = 0;
		targetDutyCycle = 0.0;
		previousDutyCycle = 0.0;
		vWheel = 0.0; // Reset vWheel only if the bike is inactive!
		TIM1->CCR1 = 0;
	}
	else
	{
		pasActive = 1;
		pasCounter = 0;
	}
}

void startupAnim()
{
	for (uint8_t i = 0; i < 20; i++)
	{
		HAL_GPIO_WritePin(FRONT_COLD_GPIO_Port, FRONT_COLD_Pin, 1);
		HAL_GPIO_WritePin(REAR_LED_GPIO_Port, REAR_LED_Pin, 0);
		HAL_Delay(15);
		HAL_GPIO_WritePin(FRONT_COLD_GPIO_Port, FRONT_COLD_Pin, 0);
		HAL_GPIO_WritePin(REAR_LED_GPIO_Port, REAR_LED_Pin, 1);
		HAL_Delay(15);
	}
	HAL_Delay(200);

	for (uint8_t i = 0; i < 4; i++)
	{
		HAL_GPIO_WritePin(FRONT_COLD_GPIO_Port, FRONT_COLD_Pin, 1);
		HAL_GPIO_WritePin(REAR_LED_GPIO_Port, REAR_LED_Pin, 0);
		HAL_Delay(75);
		HAL_GPIO_WritePin(FRONT_COLD_GPIO_Port, FRONT_COLD_Pin, 0);
		HAL_GPIO_WritePin(REAR_LED_GPIO_Port, REAR_LED_Pin, 1);
		HAL_Delay(75);
	}

	HAL_Delay(200);

	for (uint8_t i = 0; i < 2; i++)
	{
		HAL_GPIO_WritePin(FRONT_COLD_GPIO_Port, FRONT_COLD_Pin, 1);
		HAL_GPIO_WritePin(REAR_LED_GPIO_Port, REAR_LED_Pin, 0);
		HAL_Delay(250);
		HAL_GPIO_WritePin(FRONT_COLD_GPIO_Port, FRONT_COLD_Pin, 0);
		HAL_GPIO_WritePin(REAR_LED_GPIO_Port, REAR_LED_Pin, 1);
		HAL_Delay(250);
	}
	HAL_GPIO_WritePin(FRONT_COLD_GPIO_Port, FRONT_COLD_Pin, 1);
	HAL_GPIO_WritePin(REAR_LED_GPIO_Port, REAR_LED_Pin, 1);
}

void handleStartupAnim()
{
	if (shouldStartAnim && !isAnimatingStartup)
	{
		shouldStartAnim = 0;
		isAnimatingStartup = 1; // Rozpocznij animację
		startupAnim();
		isAnimatingStartup = 0; // Ustaw flagę po zakończeniu
	}
}

void connectedAnim()
{
	for (uint8_t i = 0; i < 4; i++)
	{
		HAL_GPIO_WritePin(FRONT_COLD_GPIO_Port, FRONT_COLD_Pin, 1);
		HAL_GPIO_WritePin(REAR_LED_GPIO_Port, REAR_LED_Pin, 0);
		HAL_Delay(75);
		HAL_GPIO_WritePin(FRONT_COLD_GPIO_Port, FRONT_COLD_Pin, 0);
		HAL_GPIO_WritePin(REAR_LED_GPIO_Port, REAR_LED_Pin, 1);
		HAL_Delay(75);
	}
}

void disconnectedAnim()
{
	for (uint8_t i = 0; i < 10; i++)
	{
		HAL_GPIO_WritePin(FRONT_COLD_GPIO_Port, FRONT_COLD_Pin, 1);
		HAL_GPIO_WritePin(REAR_LED_GPIO_Port, REAR_LED_Pin, 0);
		HAL_Delay(15);
		HAL_GPIO_WritePin(FRONT_COLD_GPIO_Port, FRONT_COLD_Pin, 0);
		HAL_GPIO_WritePin(REAR_LED_GPIO_Port, REAR_LED_Pin, 1);
		HAL_Delay(15);
	}
}

void handleConnectionStatusAnim()
{
	if ((connected && !shouldDisconnectedAnim) || isAnimatingStartup)
		return;
	if (shouldConnectedAnim)
	{
		shouldConnectedAnim = 0;
		connected = 1;
		connectedAnim();
		HAL_GPIO_WritePin(FRONT_COLD_GPIO_Port, FRONT_COLD_Pin, 0);
		HAL_GPIO_WritePin(REAR_LED_GPIO_Port, REAR_LED_Pin, 0);
	}
	else if (shouldDisconnectedAnim)
	{
		shouldDisconnectedAnim = 0;
		connected = 0;
		disconnectedAnim();
		HAL_GPIO_WritePin(FRONT_COLD_GPIO_Port, FRONT_COLD_Pin, 0);
		HAL_GPIO_WritePin(REAR_LED_GPIO_Port, REAR_LED_Pin, 0);
	}

	else if (HAL_GetTick() - blinkDisconnectedTime > 1000)
	{
		HAL_GPIO_TogglePin(FRONT_COLD_GPIO_Port, FRONT_COLD_Pin);
		HAL_GPIO_TogglePin(REAR_LED_GPIO_Port, REAR_LED_Pin);
		// HAL_GPIO_WritePin(REAR_LED_GPIO_Port, REAR_LED_Pin, !HAL_GPIO_ReadPin(FRONT_COLD_GPIO_Port, FRONT_COLD_Pin));
		blinkDisconnectedTime = HAL_GetTick();
	}
}

void blink1S()
{
	if (HAL_GetTick() - blink1STime > 1000)
	{
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		blink1STime = HAL_GetTick();
	}
}

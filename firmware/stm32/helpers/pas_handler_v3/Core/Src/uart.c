/*
 * uart.c
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

#include <stdbool.h>
#include <stdlib.h>

#include "uart.h"
#include "helpers.h"
#include "main.h"
#include "sounds.h"
#include "animations.h"
#include "gpio.h"
#include "commands.h"

uint8_t rxBuffer[16];

char *currentMessage = "eskl_initCompl\r\n";

bool shouldSendStatus = false;

void handleRxInterrupt(UART_HandleTypeDef *huart, uint16_t size)
{
	if (huart != &huart1 || size == 0)
	{
		HAL_UARTEx_ReceiveToIdle_IT(huart, rxBuffer, sizeof(rxBuffer));
		return;
	}

	for (uint32_t i = 0; i < sizeof(commands) / sizeof(Command); i++)
	{
		if (strncmp((char *)rxBuffer, commands[i].command, strlen(commands[i].command)) == 0)
		{
			commands[i].action();
			break;
		}
	}
	HAL_UARTEx_ReceiveToIdle_IT(huart, rxBuffer, sizeof(rxBuffer));
}

void processSendStatus()
{
	if (shouldSendStatus)
	{
		shouldSendStatus = false;
		sendStatus();
	}
}

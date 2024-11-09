/*
 * uart.h
 *
 * Contains UART helperss
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

#ifndef __UART_H
#define __UART_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "stm32f4xx_hal.h"

extern bool shouldSendStatus;

extern uint8_t rxBuffer[16];

void handleRxInterrupt(UART_HandleTypeDef *huart, uint16_t size);

void processSendStatus(void);
void sendStatus1s(void);

#endif /* __UART_H */
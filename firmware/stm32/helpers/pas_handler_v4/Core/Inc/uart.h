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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "stm32f4xx_hal.h"

extern bool shouldSendStatus;

extern uint8_t rxBuffer[16];

void uart_handleRxInterrupt(UART_HandleTypeDef* huart, uint16_t size);

void status_proc();
void sendStatus1s();

#endif /* __UART_H */
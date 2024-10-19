/*
 * helpers.h
 *
 * Contains UART helperss
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_H
#define __UART_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

extern uint8_t rxBuffer[16];

void send_string(char *s);
void send_uint8(uint8_t num);
void send_int(int num);
void send_float(float num);

void handleRxInterrupt(UART_HandleTypeDef *huart, uint16_t size);

#endif
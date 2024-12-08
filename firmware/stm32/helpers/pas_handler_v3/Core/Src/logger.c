/*
 * logger.c
 *
 * Contains logging utilities
 *
 *  Created on: Oct 27, 2024
 *      Author: nick
 */

#include <stdio.h>
#include <string.h>

#include "main.h"
#include "helpers.h"

#include "stm32f4xx_hal.h"

void send_string(char* s) {
  HAL_UART_Transmit_IT(&huart1, (uint8_t*)s, strlen(s));
}

void send_uint8(uint8_t num) {
  char buffer[4];
  sprintf(buffer, "%u", num);
  send_string(buffer);
}

void send_int(int num) {
  char buffer[12];
  sprintf(buffer, "%d", num);
  send_string(buffer);
}

void send_float(float num) {
  send_string(float_to_char(num));
}
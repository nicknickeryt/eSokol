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

#include "helpers.h"
#include "main.h"
#include "stm32f4xx_hal.h"

char* float_to_char(float f) {
    static char buffer[20];

    int whole_part = (int)f;
    int fractional_part = (int)((f - whole_part) * 100);

    if (fractional_part < 0) fractional_part = -fractional_part;

    sprintf(buffer, "%d.%02d", whole_part, fractional_part);

    return buffer;
}

void send_string(char* s) {
    HAL_UART_Transmit_IT(&huart1, (uint8_t*)s, strlen(s));
}

void send_uint8(uint8_t num) {
    char buffer[6];
    sprintf(buffer, "%u\n", num);
    send_string(buffer);
}

void send_uint32(uint32_t num) {
    char string[32];
    HAL_UART_Transmit(&huart1, (uint8_t*)string, sprintf(string, "%lu\n", num),
                      HAL_MAX_DELAY);
}

void send_int(int num) {
    char buffer[12];
    sprintf(buffer, "%d", num);
    send_string(buffer);
}

void send_float(float num) { send_string(float_to_char(num)); }
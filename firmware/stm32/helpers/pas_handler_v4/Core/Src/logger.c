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

char* logger_floatToChar(float f) {
    static char buffer[20];

    int whole_part = (int)f;
    int fractional_part = (int)((f - whole_part) * 100);

    if (fractional_part < 0) fractional_part = -fractional_part;

    sprintf(buffer, "%d.%02d", whole_part, fractional_part);

    return buffer;
}

void logger_sendChar(char* s) {
    HAL_UART_Transmit_IT(&huart1, (uint8_t*)s, strlen(s));
}

void logger_sendUint8(uint8_t num) {
    char buffer[6];
    sprintf(buffer, "%u\n", num);
    logger_sendChar(buffer);
}

void logger_sendUint32(uint32_t num) {
    char string[32];
    HAL_UART_Transmit(&huart1, (uint8_t*)string, sprintf(string, "%lu\n", num),
                      HAL_MAX_DELAY);
}

void logger_sendInt(int num) {
    char buffer[12];
    sprintf(buffer, "%d", num);
    logger_sendChar(buffer);
}

void logger_sendFloat(float num) { logger_sendChar(logger_floatToChar(num)); }
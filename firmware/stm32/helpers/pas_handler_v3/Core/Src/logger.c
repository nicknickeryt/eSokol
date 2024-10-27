/*
 * logger.c
 *
 * Contains logging utilities
 *
 *  Created on: Oct 27, 2024
 *      Author: nick
 */

#include <string.h>
#include <stdio.h>

#include "main.h"

#include "stm32f4xx_hal.h"

void send_string(char *s)
{
    HAL_UART_Transmit_IT(&huart1, (uint8_t *)s, strlen(s));
}

void send_uint8(uint8_t num)
{
    char buffer[4];
    sprintf(buffer, "%u", num);
    send_string(buffer);
}

void send_int(int num)
{
    char buffer[12];
    sprintf(buffer, "%d", num);
    send_string(buffer);
}

void send_float(float num)
{
    int whole_part = (int)num;
    int fractional_part = (int)((num - whole_part) * 100);

    if (fractional_part < 0)
    {
        fractional_part = -fractional_part;
    }

    char buffer[20];
    sprintf(buffer, "%d.%02d", whole_part, fractional_part);
    send_string(buffer);
}

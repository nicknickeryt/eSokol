/*
 * gpio.c
 *
 * GPIO helpers to keep up with logic
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

#include <stdint.h>

#include "main.h"
#include "helpers.h"
#include "commands.h"

#include "stm32f4xx_hal.h"

void writePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, bool newState)
{
    switch (GPIO_Pin)
    {
    case FRONT_WARM_Pin:
        if (GPIOx == FRONT_WARM_GPIO_Port)
            frontWarmEnabled = newState;
        break;
    case REAR_LED_Pin:
        if (GPIOx == REAR_LED_GPIO_Port)
            rearEnabled = newState;
        break;
    case THR_DIS_Pin:
        if (GPIOx == THR_DIS_GPIO_Port)
            throttleEnabled = newState;
        break;
    case THR_SPORT_Pin:
        if (GPIOx == THR_SPORT_GPIO_Port)
            sportModeDisabled = newState;
        break;
    case BULBS_Pin:
        if (GPIOx == BULBS_GPIO_Port)
            bulbsEnabled = newState;  
        break;
    default:
        break;
    }
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, newState);
}

bool readPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    return HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
}

void togglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    bool newState = !readPin(GPIOx, GPIO_Pin);
    writePin(GPIOx, GPIO_Pin, newState);
}
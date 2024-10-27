/*
 * gpio.h
 *
 * GPIO helpers to keep up with logic
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

#ifndef __GPIO_H
#define __GPIO_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "stm32f4xx_hal.h"

void togglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void writePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, bool newState);
bool readPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

#endif /* __GPIO_H */
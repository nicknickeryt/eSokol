/*
 * animations.c
 *
 * Contains light animation helpers
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

#include "animations.h"

#include <stdint.h>

#include "ambientlight.h"
#include "commands.h"
#include "gpio.h"
#include "helpers.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "uart.h"

uint32_t blink1STime = 0;  // blink every 1s
uint32_t blinkDisconnectedTime = 0;  // blink every 1s if disconnected

AnimState currentAnimState = ANIM_IDLE;
uint32_t animStartTime = 0;
uint8_t animStep = 0;

void animation_play(AnimState animState) {
    currentAnimState = animState;
    animStartTime = HAL_GetTick();
    animStep = 0;
}

void anim_proc() {
    uint32_t now = HAL_GetTick();

    switch (currentAnimState) {
        case ANIM_IDLE:
            if (!bike_bluetoothConnected) {
                currentAnimState = ANIM_BLINK_DISCONNECTED;
                animStartTime = now;
            }
            break;

        case ANIM_STARTUP_PHASE1:
            if (now - animStartTime >= 30) {
                toggleFrontColdNoSound();
                togglePin(REAR_LED_GPIO_Port, REAR_LED_Pin);
                animStep++;
                animStartTime = now;

                if (animStep >= 20) {
                    animStep = 0;
                    currentAnimState = ANIM_STARTUP_PHASE2;
                }
            }
            break;

        case ANIM_STARTUP_PHASE2:
            if (now - animStartTime >= 75) {
                toggleFrontColdNoSound();
                togglePin(REAR_LED_GPIO_Port, REAR_LED_Pin);
                animStep++;
                animStartTime = now;

                if (animStep >= 4) {
                    animStep = 0;
                    currentAnimState = ANIM_STARTUP_PHASE3;
                }
            }
            break;

        case ANIM_STARTUP_PHASE3:
            if (now - animStartTime >= 250) {
                toggleFrontColdNoSound();
                togglePin(REAR_LED_GPIO_Port, REAR_LED_Pin);
                animStep++;
                animStartTime = now;

                if (animStep >= 2) {
                    currentAnimState = ANIM_IDLE;
                    enableFrontColdNoSound();
                    gpio_write(REAR_LED_GPIO_Port, REAR_LED_Pin, 1);
                    shouldSendStatus = true;
                }
            }
            break;

        case ANIM_CONNECTED:
            if (now - animStartTime >= 75) {
                toggleFrontColdNoSound();
                togglePin(REAR_LED_GPIO_Port, REAR_LED_Pin);
                animStep++;
                animStartTime = now;

                if (animStep >= 5) {
                    currentAnimState = ANIM_IDLE;
                    disableFrontColdNoSound();
                    gpio_write(REAR_LED_GPIO_Port, REAR_LED_Pin, 0);
                    shouldSendStatus = true;
                    initAmbientLight();  // this is important!
                }
            }
            break;

        case ANIM_DISCONNECTED:
            if (now - animStartTime >= 30) {
                toggleFrontColdNoSound();
                togglePin(REAR_LED_GPIO_Port, REAR_LED_Pin);
                animStep++;
                animStartTime = now;

                if (animStep >= 10) {
                    currentAnimState = ANIM_IDLE;
                    disableFrontColdNoSound();
                    gpio_write(REAR_LED_GPIO_Port, REAR_LED_Pin, 0);
                }
            }
            break;

        case ANIM_BLINK_DISCONNECTED:
            if (now - animStartTime >= 1000) {
                toggleFrontColdNoSound();
                togglePin(REAR_LED_GPIO_Port, REAR_LED_Pin);
                animStartTime = now;
            }
            break;
    }
}

void anim_blinkProc() {
    if (HAL_GetTick() - blink1STime > 1000) {
        togglePin(GPIOC, GPIO_PIN_13);
        blink1STime = HAL_GetTick();
    }
}
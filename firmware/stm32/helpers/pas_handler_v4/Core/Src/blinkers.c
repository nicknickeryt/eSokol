/*
 * blinkers.c
 *
 *  Created on: Dec 3, 2024
 *      Author: nick
 */

#include "commands.h"
#include "gpio.h"
#include "main.h"
#include "sounds.h"

bool blinkerLeftBlink = 0;
bool blinkerRightBlink = 0;
bool bothBlinkersBlink = 0;

bool blinkerLeftPinState = 1;
bool blinkerRightPinState = 1;

uint32_t lastToneTime = 0;
uint32_t lastSwitchCheckTime = 0;
bool toggleTone = false;

void blinkers_init() {
    gpio_write(BLINKER_LEFT_GPIO_Port, BLINKER_LEFT_Pin, 1);
    gpio_write(BLINKER_RIGHT_GPIO_Port, BLINKER_RIGHT_Pin, 1);
    blinkerLeftPinState = 1;
    blinkerRightPinState = 1;
}

void blinkers_reset() {
    blinkerRightBlink = 0;
    blinkerLeftBlink = 0;
    blinkers_init();
}

void toggleLeftBlinker() {
    blinkerLeftBlink = !blinkerLeftBlink;
    blinkers_init();
    if (blinkerLeftBlink) blinkerRightBlink = 0;
}

void toggleRightBlinker() {
    blinkerRightBlink = !blinkerRightBlink;
    blinkers_init();
    if (blinkerRightBlink) blinkerLeftBlink = 0;
}

void blinkers_enableLeft() {
    blinkers_init();
    blinkerLeftBlink = 1;
    blinkerRightBlink = 0;
}

void blinkers_enableRight() {
    blinkers_init();
    blinkerRightBlink = 1;
    blinkerLeftBlink = 0;
}

void toggleBothBlinkers() {
    blinkers_init();
    bothBlinkersBlink = !bothBlinkersBlink;
}

void blinkLeft(bool state) {
    if (blinkerLeftBlink || bothBlinkersBlink) {
        gpio_write(BLINKER_LEFT_GPIO_Port, BLINKER_LEFT_Pin, state);
        blinkerLeftPinState = state;
    }
}

void blinkRight(bool state) {
    if (blinkerRightBlink || bothBlinkersBlink) {
        gpio_write(BLINKER_RIGHT_GPIO_Port, BLINKER_RIGHT_Pin, state);
        blinkerRightPinState = state;
    }
}

void blinkBlinkers(bool state) {
    blinkLeft(state);
    blinkRight(state);
}

void processBlinkerSwitch() {
    uint32_t currentTime = HAL_GetTick();
    if (currentTime - lastSwitchCheckTime >= 1000) {
        lastSwitchCheckTime = currentTime;
        if (!gpio_read(BLINKER_LEFT_IN_GPIO_Port, BLINKER_LEFT_IN_Pin))
            blinkerLeftBlink = 1;
        if (!gpio_read(BLINKER_RIGHT_IN_GPIO_Port, BLINKER_RIGHT_IN_Pin))
            blinkerRightBlink = 1;
    }
}

void blinkers_proc() {
    processBlinkerSwitch();

    if (!blinkerLeftBlink && !blinkerRightBlink && !bothBlinkersBlink) {
        if (!blinkerLeftPinState || !blinkerRightPinState) blinkers_init();
        return;
    }

    uint32_t currentTime = HAL_GetTick();
    if (currentTime - lastToneTime >= 500) {
        lastToneTime = currentTime;

        blinkBlinkers(toggleTone);
        sound_play(toggleTone ? SOUND_CLICK_OFF : SOUND_CLICK_ON);

        toggleTone = !toggleTone;
        sendStatus();
    }
}
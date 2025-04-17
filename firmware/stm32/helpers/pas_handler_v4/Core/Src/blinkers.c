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

void initBlinkers() {
    writePin(BLINKER_LEFT_GPIO_Port, BLINKER_LEFT_Pin, 1);
    writePin(BLINKER_RIGHT_GPIO_Port, BLINKER_RIGHT_Pin, 1);
    blinkerLeftPinState = 1;
    blinkerRightPinState = 1;
}

void blinkers_reset() {
    blinkerRightBlink = 0;
    blinkerLeftBlink = 0;
    initBlinkers();
}

void toggleLeftBlinker() {
    blinkerLeftBlink = !blinkerLeftBlink;
    initBlinkers();
    if (blinkerLeftBlink) blinkerRightBlink = 0;
}

void toggleRightBlinker() {
    blinkerRightBlink = !blinkerRightBlink;
    initBlinkers();
    if (blinkerRightBlink) blinkerLeftBlink = 0;
}

void blinkers_enableLeft() {
    initBlinkers();
    blinkerLeftBlink = 1;
    blinkerRightBlink = 0;
}

void blinkers_enableRight() {
    initBlinkers();
    blinkerRightBlink = 1;
    blinkerLeftBlink = 0;
}

void toggleBothBlinkers() {
    initBlinkers();
    bothBlinkersBlink = !bothBlinkersBlink;
}

void blinkLeft(bool state) {
    if (blinkerLeftBlink || bothBlinkersBlink) {
        writePin(BLINKER_LEFT_GPIO_Port, BLINKER_LEFT_Pin, state);
        blinkerLeftPinState = state;
    }
}

void blinkRight(bool state) {
    if (blinkerRightBlink || bothBlinkersBlink) {
        writePin(BLINKER_RIGHT_GPIO_Port, BLINKER_RIGHT_Pin, state);
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
        if (!blinkerLeftPinState || !blinkerRightPinState) initBlinkers();
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
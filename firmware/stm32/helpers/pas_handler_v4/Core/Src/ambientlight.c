/*
 * ambientlight.h
 *
 *  Created on: Dec 8, 2024
 *      Author: nick
 */

#include <stdint.h>

#include "adc.h"
#include "ambientlight.h"
#include "commands.h"

uint32_t lastDimTick = 0;
bool previousAmbientLightState = 0; // 0 - day, 1 - night
bool currentAmbientLightState = 0;



void enableDefaultLights() {
    enableFrontColdNoSound();
    enableRearLEDNoSound();
}

void disableDefaultLights() {
    disableFrontColdNoSound();
    disableRearLEDNoSound();
}

void processAmbientLightState() {
    currentAmbientLightState = ldrVoltage < 0.7f ? 1 : 0;
}

void setAmbientLight() {
    switch (currentAmbientLightState)
        {
        case 0:
            disableDefaultLights();
            break;
        case 1:
            enableDefaultLights();
            break;
        }
}

void initAmbientLight() {
    processAmbientLightState();
    setAmbientLight();
}

void processAmbientLight() {
    if(!ambientLightEnabled) return;

    processAmbientLightState();

    if(currentAmbientLightState != previousAmbientLightState) {
        previousAmbientLightState = currentAmbientLightState;
        setAmbientLight();
    }
}
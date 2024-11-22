/*
 * helpers.h
 *
 * Contains UART and various other helpers
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

#ifndef __HELPERS_H
#define __HELPERS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "stm32f4xx_hal.h"

// #define DEBUG_ENABLED            // enabled algorithm & friends debug mode

#define TIM_THROTTLE_LEDS &htim1
#define TIM_SOUND &htim3
#define PWM_ENABLED 1
#define PWM_DISABLED 0

extern uint8_t pasCounter;
extern int lastPasResetTick;
extern const float pedalGearRatio;
extern const float motorGearRatio;

extern const float pi;
extern const float pasMagnetAngle;

extern const float rWheel;

extern float omegaPedals;
extern float omegaWheel;
extern float vWheel;

extern float pasActive;
extern float targetDutyCycle;
extern float previousDutyCycle;

extern float timeS;

extern bool bluetoothConnected;

void bikeInit(void);

void logDebugDegrees(float);
void logDebugVWheel(void);
void logDebugDutyCycle(void);
void logDebugInactive(void);

void togglePWM(TIM_HandleTypeDef *htim, bool state);
void resetDutyCycle(void);
void updateDutyCycle(void);
void resetPas(bool inactive);

void processDummyVelocityData(void);

#endif /* __HELPERS_H */

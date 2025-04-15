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

#define TIM_THROTTLE_LEDS &htim1
#define TIM_SOUND &htim3
#define PWM_ENABLED 1
#define PWM_DISABLED 0

// π rounded value // TODO: do we really need so many decimal places?
#define PI 3.141592

extern uint8_t pasCounter;
extern int lastPasResetTick;

extern float omegaPedals;
extern float targetOmegaWheel;
extern float targetVelocityWheel;

extern float pasActive;
extern float targetDutyCycle;
extern float previousDutyCycle;

extern float pasTimeS;

extern bool bluetoothConnected;

extern uint16_t batteryVoltage;

void bikeInit(void);

void logDebugDegrees(float);
void logDebugVWheel(void);
void logDebugDutyCycle(void);
void logDebugInactive(void);

void togglePWM(TIM_HandleTypeDef* htim, bool state);
void resetDutyCycle(void);
void updateDutyCycle(void);
void resetPas(bool inactive);

extern float currentRealBikeVelocity;

float calculateRealBikeVelocity(uint32_t hallCurrTick);
void setRealBikeVelocity(float velocity);

void processRealVelocity(void);

#endif /* __HELPERS_H */

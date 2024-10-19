/*
 * helpers.h
 *
 * Contains UART and various other helpers
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HELPERS_H
#define __HELPERS_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "stm32f4xx_hal.h"

#define MIN_DUTY_CYCLE 16.0
#define MAX_DUTY_CYCLE 66.0
#define WARN_DUTY_CYCLE 80.0

#define PEDAL_GEAR_RATIO 2.875  // this is ratio to calculate target speed, 46:16 (pedals gear, wheel gear)
#define MOTOR_GEAR_RATIO 0.5625 // this is ratio to calculate target speed, 9:16 (motor gear, wheel gear)

#define PI 3.141592
#define PAS_MAGNET_ANGLE (2.0 * PI) / 12.0 // pas angle after which we calculate omega. 2pi/12 is 30° (we have 12 magnets)

#define R_WHEEL 0.25 // wheel radius [m]

#define DUTY_SMOOTH_FACTOR_UP 0.4   // duty cycle EMA smoothing factor (acceleration)
#define DUTY_SMOOTH_FACTOR_DOWN 0.8 // duty cycle EMA smoothing factor (slowing down)
#define DUTY_PWM_MAX_CCR1 1000.0


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

extern bool shouldStartAnim;
extern bool isAnimatingStartup;
extern bool shouldConnectedAnim;
extern bool shouldDisconnectedAnim;

void bikeInit(void);

void logDebugDegrees(float);
void logDebugVWheel(void);
void logDebugDutyCycle(void);
void logDebugInactive(void);

void updateDutyCycle(void);
void resetPas(bool inactive);

void startupAnim(void);
void handleStartupAnim(void);
void handleConnectionStatusAnim(void);
void blink1S(void);

#endif /* __HELPERS_H */

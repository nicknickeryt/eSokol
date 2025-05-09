/*
 * algorithm.h
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

#ifndef __ALGORITHM_H_
#define __ALGORITHM_H_

#include <stdbool.h>
#include <stdint.h>

#include "pid.h"

#define ALGORITHM_TARGET_SPEED_PERCENTAGE_DEFAULT 1.05f ///// TODOOOOO

#define ALGORITHM_TARGET_SPEED_PERCENTAGE_MIN 0.5f
#define ALGORITHM_TARGET_SPEED_PERCENTAGE_MAX 1.5f

#define ALGORITHM_PAS_INACTIVE_TIME_MS 750

#define ALGORITHM_MIN_THROTTLE_VOLTAGE 1.1                  // throttle voltage at idle is ~ 1.04V

#define ALGORITHM_PEDAL_GEAR_RATIO 2.875                    // this is ratio to calculate target speed, 46:16 (pedals gear, wheel gear)
#define MOTOR_GEAR_RATIO 0.8125                             // this is ratio to calculate target speed, 13:16 (motor gear, wheel gear)
#define ALGORITHM_PAS_MAGNET_ANGLE (2.0 * PI) / 12.0        // pas angle after which we calculate omega. 2pi/12 is 30° (we have 12 magnets)

#define R_WHEEL 0.25
#define CIRCUMFERENCE_WHEEL_M (2.0 * PI * R_WHEEL)

#define ALGORITHM_PEDAL_SYNC_THRESHOLD 20.0f  // is it ok? idk

extern PID motorWheelSpeedPID;

void algorithm_handlePasPulse();

void algorithm_init();
void algorithm_proc();

void algorithm_setTargetSpeedPercentage(float value);
float algorithm_getTargetSpeedPercentage();

void algorithm_toggleEnabled();
bool algorithm_isEnabled();

#endif /* __ALGORITHM_H_ */

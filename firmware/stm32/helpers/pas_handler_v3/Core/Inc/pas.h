/*
 * pas.h
 *
 * Contains pedal assist sensor helpers
 *
 *  Created on: Oct 27, 2024
 *      Author: nick
 */

#ifndef __PAS_H_
#define __PAS_H_

#include <stdbool.h>

#define MIN_DUTY_CYCLE 16.0
#define MAX_DUTY_CYCLE 66.0
#define WARN_DUTY_CYCLE 80.0

// this is ratio to calculate target speed, 46:16 (pedals gear, wheel gear)
#define PEDAL_GEAR_RATIO 2.875
// this is ratio to calculate target speed, 9:16 (motor gear, wheel gear)
#define MOTOR_GEAR_RATIO 0.5625

// pas angle after which we calculate omega. 2pi/12 is 30° (we have 12 magnets)
#define PAS_MAGNET_ANGLE (2.0 * PI) / 12.0
// wheel radius [m]
#define R_WHEEL 0.25

// duty cycle EMA smoothing factor (acceleration)
#define DUTY_SMOOTH_FACTOR_UP 0.4
// duty cycle EMA smoothing factor (slowing down)
#define DUTY_SMOOTH_FACTOR_DOWN 0.8
#define DUTY_PWM_MAX_CCR1 1000.0

void resetDutyCycle();

void updateDutyCycle();

void resetPas(bool inactive);

#endif /* __PAS_H_ */
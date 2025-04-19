/*
 * motor.h
 *
 * Contains motor control helpers
 *
 *  Created on: Apr 19, 2025
 *      Author: nick
 */

#ifndef __MOTOR_H
#define __MOTOR_H

#include <stdbool.h>
#include <stdint.h>

#include "helpers.h"

#define MIN_DUTY_CYCLE 0.0f
#define MAX_DUTY_CYCLE 100.0f

#define DUTY_PWM_MAX_CCR1 1000.0

void motor_setDutyCycle(float dutyCycle);
float motor_getDutyCycle();

void motor_updateDutyCycle();
void motor_resetDutyCycle();

#endif /* __MOTOR_H */
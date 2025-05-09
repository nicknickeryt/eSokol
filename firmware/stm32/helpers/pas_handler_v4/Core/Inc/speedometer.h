/*
 * sounds.h
 *
 * Contains sounds helpers
 *
 *  Created on: Oct 26, 2024
 *      Author: nick
 */
#ifndef __SPEEDOMETER_H
#define __SPEEDOMETER_H

#include <stdint.h> 

void speedometer_init();

float speedometer_calculateWheelVelocity(uint32_t hallCurrTick);
float speedometer_calculateMotorWheelVelocity(uint32_t hallCurrTick);

void speedometer_setWheelVelocity(float velocity);
void speedometer_setMotorWheelVelocity(float velocity);

float speedometer_getWheelVelocityKmh();
float speedometer_getMotorWheelVelocityKmh();

void speedometer_proc();

#endif /* __SPEEDOMETER_H */
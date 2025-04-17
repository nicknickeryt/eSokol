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

void speedometer_init(void);

float speedometer_calculateVelocity(uint32_t hallCurrTick);
void speedometer_setVelocity(float velocity);
float speedometer_getVelocity(void);

void speedometer_proc(void);

#endif /* __SPEEDOMETER_H */
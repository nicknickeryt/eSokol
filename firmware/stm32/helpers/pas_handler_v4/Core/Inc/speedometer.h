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

extern char* velocityBuffer;

extern uint32_t hallLastTick;
extern uint32_t hallLastSendTick;
extern float currentRealBikeVelocity;

void initVelocityBuffer(void);

float speedometer_calculateVelocity(uint32_t hallCurrTick);
void speedometer_setVelocity(float velocity);

void speedometer_proc(void);

#endif /* __SPEEDOMETER_H */
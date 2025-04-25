#include "speedometer.h"
#include "helpers.h"
#include "algorithm.h"
#include "logger.h"

#include <stdlib.h>

char* wheelVelocityBuffer = 0;

uint32_t wheelHallLastTick = 0;
uint32_t wheelHallLastSendTick = 0;
float wheelCurrentVelocityKmh = 0;

uint32_t motorWheelHallLastTick = 0;
float motorWheelCurrentVelocityKmh = 0;

void speedometer_init() { wheelVelocityBuffer = (char*)malloc(18 * sizeof(char)); }

float speedometer_calculateWheelVelocity(uint32_t hallCurrTick) {
    float omega = (2.0f * PI) / ((hallCurrTick - wheelHallLastTick) / 1000.0f);
    wheelHallLastTick = hallCurrTick;
    return omega * R_WHEEL * 3.6;  // velocity in km/h
}

float speedometer_calculateMotorWheelVelocity(uint32_t hallCurrTick) {
    float omega = (2.0f * PI) / ((hallCurrTick - motorWheelHallLastTick) / 1000.0f);
    omega /= 2; // we've got 2 magnets!
    motorWheelHallLastTick = hallCurrTick;
    return omega * MOTOR_GEAR_RATIO * R_WHEEL * 3.6;  // velocity in km/h
}

void speedometer_setWheelVelocity(float velocity) { wheelCurrentVelocityKmh = velocity; }
void speedometer_setMotorWheelVelocity(float velocity) { motorWheelCurrentVelocityKmh = velocity; }

float speedometer_getWheelVelocityKmh() { return wheelCurrentVelocityKmh;}
float speedometer_getMotorWheelVelocityKmh() { return motorWheelCurrentVelocityKmh;}

void speedometer_proc() {
    if (!(HAL_GetTick() - wheelHallLastSendTick > 300)) return;
    if (HAL_GetTick() - wheelHallLastTick > 2500) wheelCurrentVelocityKmh = 0.0f;

    sprintf(wheelVelocityBuffer, "eskl_evel%s\r\n",
            logger_floatToChar(wheelCurrentVelocityKmh));
    wheelHallLastSendTick = HAL_GetTick();

    logger_sendChar(wheelVelocityBuffer);
}

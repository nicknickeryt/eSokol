#include "speedometer.h"
#include "helpers.h"
#include "algorithm.h"
#include "logger.h"

#include <stdlib.h>

char* velocityBuffer = 0;

uint32_t hallLastTick = 0;
uint32_t hallLastSendTick = 0;
float currentRealBikeVelocity = 0;

void speedometer_init() { velocityBuffer = (char*)malloc(18 * sizeof(char)); }

float speedometer_calculateVelocity(uint32_t hallCurrTick) {
    float omega = (2.0f * PI) / ((hallCurrTick - hallLastTick) / 1000.0f);
    hallLastTick = hallCurrTick;
    return omega * R_WHEEL * 3.6;  // velocity in km/h
}

void speedometer_setVelocity(float velocity) { currentRealBikeVelocity = velocity; }

float speedometer_getVelocity() { return currentRealBikeVelocity;}

void speedometer_proc() {
    if (!(HAL_GetTick() - hallLastSendTick > 300)) return;
    if (HAL_GetTick() - hallLastTick > 2500) currentRealBikeVelocity = 0.0f;

    sprintf(velocityBuffer, "eskl_evel%s\r\n",
            logger_floatToChar(currentRealBikeVelocity));
    hallLastSendTick = HAL_GetTick();

    logger_sendChar(velocityBuffer);
}

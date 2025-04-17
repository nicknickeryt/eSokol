#include "odometer.h"

static uint32_t odometer_pulseCount = 0;

void odometer_init(void) {
    odometer_pulseCount = 0;
}

void odometer_addPulse(void) {
    odometer_pulseCount++;
}

float odometer_getDistanceMeters(void) {
    return odometer_pulseCount * CIRCUMFERENCE_WHEEL_M;
}

void odometer_reset(void) {
    odometer_pulseCount = 0;
}

#ifndef __ODOMETER_H
#define __ODOMETER_H

#include "pas.h"
#include <stdint.h>

#define ODOMETER_DISTANCE_SAVE_METERS 500

void odometer_init();

void odometer_pulseInterrupt();

float odometer_getDistanceMeters();

void odometer_reset();

void odometer_proc();

#endif // __ODOMETER_H

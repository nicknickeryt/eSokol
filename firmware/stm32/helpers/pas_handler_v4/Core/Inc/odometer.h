#ifndef __ODOMETER_H
#define __ODOMETER_H

#include "pas.h"
#include <stdint.h>

void odometer_init(void);

void odometer_addPulse(void);

float odometer_getDistanceMeters(void);

void odometer_reset(void);

#endif // __ODOMETER_H

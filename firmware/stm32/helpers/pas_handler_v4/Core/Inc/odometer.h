#ifndef __ODOMETER_H
#define __ODOMETER_H

#include <stdint.h>

#include "pas.h"

#define ODOMETER_DISTANCE_SAVE_METERS 500

#define ODOMETER_FLASH_SLOT_COUNT 30
#define ODOMETER_FLASH_SECTOR_ADDR 0x0000
#define ODOMETER_FLASH_SLOT_SIZE 4

void odometer_init();

void odometer_pulseInterrupt();

float odometer_getDistanceMeters();

void odometer_reset();

void odometer_proc();

#endif  // __ODOMETER_H

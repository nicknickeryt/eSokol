#include "odometer.h"
#include "algorithm.h"
#include "main.h"
#include "spif.h"

#include "logger.h"

static uint32_t odometer_pulseCount = 0;
static uint32_t odometer_distanceMeters = 0;

static uint32_t odometer_lastSavedDistance = 0;

static uint32_t odometer_currentFlashSlot = 0;

SPIF_HandleTypeDef spif;

void odometer_writeToFlash() {
    odometer_currentFlashSlot++; // fist increase flash slot, then write!

    uint8_t writeBuffer[4];

    writeBuffer[0] = (uint8_t)(odometer_distanceMeters & 0xFF);
    writeBuffer[1] = (uint8_t)((odometer_distanceMeters >> 8) & 0xFF);
    writeBuffer[2] = (uint8_t)((odometer_distanceMeters >> 16) & 0xFF);
    writeBuffer[3] = (uint8_t)((odometer_distanceMeters >> 24) & 0xFF);

    uint32_t offset =
        ODOMETER_FLASH_SECTOR_ADDR + odometer_currentFlashSlot * ODOMETER_FLASH_SLOT_SIZE;

    SPIF_WritePage(&spif, 0, writeBuffer, 4, offset);


    if (odometer_currentFlashSlot >= ODOMETER_FLASH_SLOT_COUNT) {
        SPIF_EraseSector(&spif, 0);
        odometer_currentFlashSlot = 0;

        odometer_writeToFlash();
    }
}

void odometer_init() {
    SPIF_Init(&spif, &hspi1, SPI1_CS_GPIO_Port, SPI1_CS_Pin);

    uint8_t readBuffer[4];
    uint32_t maxDistance = 0;
    uint32_t currentDistance;

    for (uint32_t i = 0; i < ODOMETER_FLASH_SLOT_COUNT; i++) {
        SPIF_ReadPage(&spif, 0, readBuffer, 4,
                      ODOMETER_FLASH_SECTOR_ADDR + i * ODOMETER_FLASH_SLOT_SIZE);

        currentDistance = (uint32_t)readBuffer[0];
        currentDistance |= (uint32_t)readBuffer[1] << 8;
        currentDistance |= (uint32_t)readBuffer[2] << 16;
        currentDistance |= (uint32_t)readBuffer[3] << 24;

        if (currentDistance != 0xFFFFFFFF && currentDistance >= maxDistance) {
            maxDistance = currentDistance;
            odometer_currentFlashSlot = i;
        }
    }

    odometer_distanceMeters = maxDistance;
    odometer_lastSavedDistance = maxDistance;
}

void odometer_pulseInterrupt() {
    odometer_distanceMeters += CIRCUMFERENCE_WHEEL_M;
}

float odometer_getDistanceMeters() { return odometer_distanceMeters; }

void odometer_reset() {
    odometer_pulseCount = 0;
    odometer_distanceMeters = 0;
    odometer_lastSavedDistance = 0;
    odometer_currentFlashSlot = 0;

    SPIF_EraseSector(&spif, 0);

    uint8_t zeroBuffer[4] = {0, 0, 0, 0};
    SPIF_WritePage(&spif, 0, zeroBuffer, 4, ODOMETER_FLASH_SECTOR_ADDR);
}

void odometer_proc() {
    if (odometer_distanceMeters - odometer_lastSavedDistance >=
        ODOMETER_DISTANCE_SAVE_METERS) {
        odometer_writeToFlash();
        odometer_lastSavedDistance = odometer_distanceMeters;
    }
}
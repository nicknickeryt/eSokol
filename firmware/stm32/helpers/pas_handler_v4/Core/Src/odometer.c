#include "odometer.h"
#include "spif.h"
#include "main.h"

static uint32_t odometer_pulseCount = 0;
static uint32_t odometer_distanceMeters = 0;

static uint32_t odometer_lastSavedDistance = 0;

SPIF_HandleTypeDef spif;

void odometer_writeToFlash() {
    uint8_t writeBuffer[4];

    writeBuffer[0] = (uint8_t)(odometer_distanceMeters & 0xFF);
    writeBuffer[1] = (uint8_t)((odometer_distanceMeters >> 8) & 0xFF);
    writeBuffer[2] = (uint8_t)((odometer_distanceMeters >> 16) & 0xFF);
    writeBuffer[3] = (uint8_t)((odometer_distanceMeters >> 24) & 0xFF);

    SPIF_EraseSector(&spif, 0);

    SPIF_WritePage(&spif, 0, writeBuffer, 4, 0);
}

void odometer_init() {
    SPIF_Init(&spif, &hspi1, SPI1_CS_GPIO_Port, SPI1_CS_Pin);

    uint8_t readBuffer[4];  
    if (SPIF_ReadPage(&spif, 0, readBuffer, 4, 0)) {
        uint32_t tempDistance = 0;
        tempDistance  = (uint32_t)readBuffer[0];
        tempDistance |= (uint32_t)readBuffer[1] << 8;
        tempDistance |= (uint32_t)readBuffer[2] << 16;
        tempDistance |= (uint32_t)readBuffer[3] << 24;

        odometer_distanceMeters = tempDistance;
        odometer_lastSavedDistance = tempDistance;
    }
}

void odometer_pulseInterrupt() {
    odometer_distanceMeters += CIRCUMFERENCE_WHEEL_M;
}

float odometer_getDistanceMeters() {
    return odometer_distanceMeters;
}

void odometer_reset() {
    odometer_pulseCount = 0;
}

void odometer_proc() {
    if (odometer_distanceMeters - odometer_lastSavedDistance >= ODOMETER_DISTANCE_SAVE_METERS){
        odometer_writeToFlash();
        odometer_lastSavedDistance = odometer_distanceMeters;
    }
}
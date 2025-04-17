/*
 * adc.c
 *
 *  Created on: Dec 8, 2024
 *      Author: nick
 */

#include "adc.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "logger.h"
#include "main.h"

bool adc_convCmplt = 0;
bool adc_initialMeasurementCmplt = 0;

uint16_t adc_batteryVoltage = 0;
uint16_t adc_batteryCurrent = 0;
float adc_ldrVoltage = 0;
float adc_throttleVoltage = 0;

uint16_t adc_rawBatteryValues[ADC_AVG_SAMPLES] = {0};
uint16_t adc_rawBatteryCurrentValues[ADC_AVG_SAMPLES] = {0};
uint16_t adc_rawLdrValues[ADC_AVG_SAMPLES] = {0};
uint16_t adc_rawThrottleValues[ADC_AVG_SAMPLES] = {0};

uint32_t adc_lastSendTick = 0;
uint8_t adc_currentIdx = 0;
uint16_t adc_rawValues[4] = {0};

void adc_startMeasurement() { adc_convCmplt = 1; }

void adc_batteryVoltageProc() {
    uint16_t adc_measurementsSum = 0;
    for (uint8_t i = 0; i < ADC_AVG_SAMPLES; i++)
        adc_measurementsSum += adc_rawBatteryValues[i];

    adc_batteryVoltage =
        adc_initialMeasurementCmplt
            ? (100.0f *
               (((adc_measurementsSum / ADC_AVG_SAMPLES) * 3.3f) / 4096.0f)) +
                  23.0f
            : (100.0f *
               ((adc_rawValues[ADC_BATTERY_CHANNEL] * 3.3f) / 4096.0f)) +
                  23.0f;
}

void adc_batteryCurrentProc() {
    uint16_t adc_measurementsSum = 0;
    for (uint8_t i = 0; i < ADC_AVG_SAMPLES; i++)
        adc_measurementsSum += adc_rawBatteryCurrentValues[i];

    adc_batteryCurrent =
        adc_initialMeasurementCmplt
            ? (1000.0f *
               (((adc_measurementsSum / ADC_AVG_SAMPLES) * 3.3f) / 4096.0f))
            : (1000.0f *
               ((adc_rawValues[ADC_BATTERY_CURRENT_CHANNEL] * 3.3f) / 4096.0f));
}

void adc_ldrVoltageProc() {
    uint16_t adc_measurementsSum = 0;
    for (uint8_t i = 0; i < ADC_AVG_SAMPLES; i++)
        adc_measurementsSum += adc_rawLdrValues[i];

    adc_ldrVoltage =
        adc_initialMeasurementCmplt
            ? (((adc_measurementsSum / ADC_AVG_SAMPLES) * 3.3f) / 4096.0f)
            : ((adc_rawValues[ADC_LDR_CHANNEL] * 3.3f) / 4096.0f);
}

void adc_throttleVoltageProc() {
    uint16_t adc_measurementsSum = 0;
    for (uint8_t i = 0; i < ADC_AVG_SAMPLES; i++)
        adc_measurementsSum += adc_rawThrottleValues[i];

    adc_throttleVoltage =
        adc_initialMeasurementCmplt
            ? (((adc_measurementsSum / ADC_AVG_SAMPLES) * 3.3f) / 4096.0f)
            : ((adc_rawValues[ADC_THROTTLE_AIN_CHANNEL] * 3.3f) / 4096.0f);
}

void adc_proc() {
    if (!(HAL_GetTick() - adc_lastSendTick > 10) || !adc_convCmplt) return;
    adc_lastSendTick = HAL_GetTick();

    adc_rawBatteryValues[adc_currentIdx] = adc_rawValues[ADC_BATTERY_CHANNEL];
    adc_rawLdrValues[adc_currentIdx] = adc_rawValues[ADC_LDR_CHANNEL];
    adc_rawThrottleValues[adc_currentIdx] =
        adc_rawValues[ADC_THROTTLE_AIN_CHANNEL];
    adc_rawBatteryCurrentValues[adc_currentIdx] =
        adc_rawValues[ADC_BATTERY_CURRENT_CHANNEL];
    adc_currentIdx = (adc_currentIdx + 1) % ADC_AVG_SAMPLES;

    if (adc_currentIdx == ADC_AVG_SAMPLES - 1) adc_initialMeasurementCmplt = 1;

    adc_batteryVoltageProc();
    adc_ldrVoltageProc();
    adc_throttleVoltageProc();
    adc_batteryCurrentProc();

    adc_convCmplt = 0;
}
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

static float adc_toVoltage(uint16_t adcValue) {
    return (adcValue * 3.3f) / 4096.0f;
}

static void adc_storeRawValues(void) {
    adc_rawBatteryValues[adc_currentIdx]          = adc_rawValues[ADC_BATTERY_CHANNEL];
    adc_rawLdrValues[adc_currentIdx]              = adc_rawValues[ADC_LDR_CHANNEL];
    adc_rawThrottleValues[adc_currentIdx]         = adc_rawValues[ADC_THROTTLE_AIN_CHANNEL];
    adc_rawBatteryCurrentValues[adc_currentIdx]   = adc_rawValues[ADC_BATTERY_CURRENT_CHANNEL];
}

static uint16_t adc_average(const uint16_t* values) {
    uint32_t sum = 0;
    for (uint8_t i = 0; i < ADC_AVG_SAMPLES; i++) sum += values[i];
    return sum / ADC_AVG_SAMPLES;
}

void adc_batteryVoltageProc() {
    float voltage = adc_toVoltage(adc_initialMeasurementCmplt
                                      ? adc_average(adc_rawBatteryValues)
                                      : adc_rawValues[ADC_BATTERY_CHANNEL]);
    adc_batteryVoltage = 100.0f * voltage + 23.0f;
}

void adc_batteryCurrentProc() {
    float voltage =
        adc_toVoltage(adc_initialMeasurementCmplt
                          ? adc_average(adc_rawBatteryCurrentValues)
                          : adc_rawValues[ADC_BATTERY_CURRENT_CHANNEL]);
    adc_batteryCurrent = 1000.0f * voltage;
}

void adc_ldrVoltageProc() {
    adc_ldrVoltage = adc_toVoltage(adc_initialMeasurementCmplt
                                       ? adc_average(adc_rawLdrValues)
                                       : adc_rawValues[ADC_LDR_CHANNEL]);
}

void adc_throttleVoltageProc() {
    adc_throttleVoltage = adc_toVoltage(
        adc_initialMeasurementCmplt ? adc_average(adc_rawThrottleValues)
                                    : adc_rawValues[ADC_THROTTLE_AIN_CHANNEL]);
}

void adc_proc() {
    if (!(HAL_GetTick() - adc_lastSendTick > ADC_SEND_PERIOD_MS) || !adc_convCmplt) return;

    adc_lastSendTick = HAL_GetTick();

    adc_storeRawValues();

    adc_currentIdx = (adc_currentIdx + 1) % ADC_AVG_SAMPLES;
    if (adc_currentIdx == ADC_AVG_SAMPLES - 1) adc_initialMeasurementCmplt = 1;

    adc_batteryVoltageProc();
    adc_ldrVoltageProc();
    adc_throttleVoltageProc();
    adc_batteryCurrentProc();

    adc_convCmplt = 0;
}
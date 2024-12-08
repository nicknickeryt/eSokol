/*
 * adc.c
 *
 *  Created on: Dec 8, 2024
 *      Author: nick
 */

#include <stdint.h>
#include <stdbool.h>

#include "adc.h"
#include "main.h"
#include "logger.h"

uint16_t batteryVoltage = 0;
float ldrVoltage = 0;

uint32_t adcLastSendTick = 0;

uint16_t rawAdcValues[2] = {0};

bool adcConvCompleted = 0;

bool initialAdcMeasurementCompleted = 0;

uint8_t adcIndex = 0;
uint16_t rawBatteryValues[ADC_AVG_SAMPLES] = {0};
uint16_t rawLdrValues[ADC_AVG_SAMPLES] = {0};

void startAdcMeasurement() {
    adcConvCompleted = 1;
}

void processBatteryVoltage() {
  uint16_t adcSum = 0;
  for (uint8_t i = 0; i < ADC_AVG_SAMPLES; i++)
     adcSum += rawBatteryValues[i];
   
  batteryVoltage = initialAdcMeasurementCompleted ? (100.0f * (((adcSum/ADC_AVG_SAMPLES) * 3.3f) / 4096.0f)) + 23.0f : (100.0f * ((rawAdcValues[ADC_BATTERY_CHANNEL] * 3.3f) / 4096.0f)) + 23.0f;
}

void processLdrVoltage() {
  uint16_t adcSum = 0;
  for (uint8_t i = 0; i < ADC_AVG_SAMPLES; i++)
     adcSum += rawLdrValues[i];
   
  ldrVoltage = initialAdcMeasurementCompleted ? (((adcSum/ADC_AVG_SAMPLES) * 3.3f) / 4096.0f) : ((rawAdcValues[ADC_LDR_CHANNEL] * 3.3f) / 4096.0f);
}


void processAdcMeasurement() {
  if (!(HAL_GetTick() - adcLastSendTick > 1000) || !adcConvCompleted)
    return;
  adcLastSendTick = HAL_GetTick();

  rawBatteryValues[adcIndex] = rawAdcValues[ADC_BATTERY_CHANNEL];
  rawLdrValues[adcIndex] = rawAdcValues[ADC_LDR_CHANNEL];
  adcIndex = (adcIndex + 1) % ADC_AVG_SAMPLES;

  if(adcIndex == ADC_AVG_SAMPLES - 1) initialAdcMeasurementCompleted = 1;

  processBatteryVoltage();
  processLdrVoltage();

  adcConvCompleted = 0;
}
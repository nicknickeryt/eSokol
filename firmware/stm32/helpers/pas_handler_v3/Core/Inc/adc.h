/*
 * adc.h
 *
 *  Created on: Dec 8, 2024
 *      Author: nick
 */

#ifndef __ADC_H_
#define __ADC_H

#include <stdint.h>

#define ADC_AVG_SAMPLES 20

#define ADC_BATTERY_CHANNEL 0
#define ADC_LDR_CHANNEL 1

extern uint16_t batteryVoltage;
extern float ldrVoltage;

extern uint16_t rawAdcValues[2];

void startAdcMeasurement(void);
void processAdcMeasurement(void);

#endif /* __ADC_H_ */
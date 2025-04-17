/*
 * adc.h
 *
 *  Created on: Dec 8, 2024
 *      Author: nick
 */

#ifndef __ADC_H_
#define __ADC_H

#include <stdint.h>

#define ADC_AVG_SAMPLES 10

#define ADC_BATTERY_CHANNEL 0
#define ADC_LDR_CHANNEL 1
#define ADC_THROTTLE_AIN_CHANNEL 2
#define ADC_BATTERY_CURRENT_CHANNEL 3

extern uint16_t batteryVoltage;
extern uint16_t batteryCurrent;
extern float ldrVoltage;
extern float throttleVoltage;

extern uint16_t rawAdcValues[4];

void adc_initMeasurement(void);
void adc_proc(void);

#endif /* __ADC_H_ */
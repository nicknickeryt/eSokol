/*
 * algorithm.h
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

#ifndef __ALGORITHM_H_
#define __ALGORITHM_H_

#include <stdbool.h>
#include <stdint.h>

// #define DEBUG_ENABLED            // enabled algorithm & friends debug mode

#define ALGORITHM_EQ_COMPONENT_DEFAULT -1.0
#define ALGORITHM_EQ_FACTOR 1.05

#define ALGORITHM_EQ_COMPONENT_MIN -3.0f
#define ALGORITHM_EQ_COMPONENT_MAX 3.0f

#define PAS_INACTIVE_TIME_S 0.5     // time in seconds after which targetDutyCycle is set to 0

// this should be a value between -9.9 and 9.9
extern float algorithm_eq_component;

void calculateDutyCycle(float x);
bool runAlgorithm();

#endif /* __ALGORITHM_H_ */

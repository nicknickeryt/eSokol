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

#define ALGORITHM_EQ_COMPONENT_DEFAULT -9.0
#define ALGORITHM_EQ_FACTOR 1.5

#define ALGORITHM_EQ_COMPONENT_MIN -9.0f
#define ALGORITHM_EQ_COMPONENT_MAX 3.0f

#define PAS_INACTIVE_TIME_S 0.5     // time in seconds after which targetDutyCycle is set to 0

#define MINIMUM_THROTTLE_VOLTAGE 1.1 // voltage below which duty cycle is 0, throttle voltage at idle is ~ 1.04V

#define ALGORITM_DISABLE // temp!! disable algorithm for now...

// this should be a value between -9.9 and 9.9
extern float algorithm_eq_component;

bool algorithm_proc();

#endif /* __ALGORITHM_H_ */

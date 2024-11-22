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

#define ALGORITHM_EQ_COMPONENT -1.0
#define ALGORITHM_EQ_FACTOR 1.05

void calculateDutyCycle(float x);
bool runAlgorithm();

#endif /* __ALGORITHM_H_ */

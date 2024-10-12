/*
 * algorithm.h
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

#ifndef INC_ALGORITHM_H_
#define INC_ALGORITHM_H_

#include <stdint.h>
#include <stdbool.h>

#define ALGORITHM_EQ_COMPONENT 	3.3
#define ALGORITHM_EQ_FACTOR		1.1

void calculateDutyCycle(float x);
bool runAlgorithm(float timeS);

#endif /* INC_ALGORITHM_H_ */

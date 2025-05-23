/*
 * algorithm.c
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

#include "algorithm.h"
#include "helpers.h"
#include "stm32f4xx_hal.h"

// (-0.00000044 * (x*x*x*x*x)) - (0.000049 * (x*x*x*x)) + (0.00164 * (x*x*x)) +( 0.0169 * (x*x)) + (1.1815 * x) + 18.912;
void calculateDutyCycle(float x) {
	float rawDutyCycle = ALGORITHM_EQ_FACTOR * ((-0.00000044 * (x * x * x * x * x))
			- (0.000049 * (x * x * x * x)) + (0.00164 * (x * x * x))
			+ (0.0169 * (x * x)) + (1.1815 * x) + 18.912 + ALGORITHM_EQ_COMPONENT); // DUTY_EQ_COMPONENT to spin a bit FASTER than pedals
	if (rawDutyCycle <= MIN_DUTY_CYCLE) {
		targetDutyCycle = 0;
	} else if (rawDutyCycle > WARN_DUTY_CYCLE) {
		targetDutyCycle = 0;
	} else if (rawDutyCycle > MAX_DUTY_CYCLE && rawDutyCycle < WARN_DUTY_CYCLE) {
		targetDutyCycle = MAX_DUTY_CYCLE;
	} else {
		if (rawDutyCycle > previousDutyCycle) {								// speeding up
			targetDutyCycle = (DUTY_SMOOTH_FACTOR_UP * rawDutyCycle)
					+ ((1 - DUTY_SMOOTH_FACTOR_UP) * previousDutyCycle);
		} else {															// slowing down
			targetDutyCycle = (DUTY_SMOOTH_FACTOR_DOWN * rawDutyCycle)
					+ ((1 - DUTY_SMOOTH_FACTOR_DOWN) * previousDutyCycle);
		}
	}
	previousDutyCycle = targetDutyCycle;
}

bool runAlgorithm(float timeS) {

	if(!checkSwitches()) return false;

	if (pasCounter >= 1) {

//	 	If bike is stationary, wait for 5 PAS counts before updating
		if (vWheel == 0.0 && pasCounter < 5) {
			if (pasCounter < 4)
				lastPasResetTick = HAL_GetTick();
			return false;
		}

		logDebugDegrees(timeS);  // 30 stopni!

		omegaPedals = PAS_MAGNET_ANGLE / timeS;
		omegaWheel = omegaPedals * PEDAL_GEAR_RATIO;

		vWheel = omegaWheel * R_WHEEL;
		logDebugVWheel();

		// Duty cycle calc
		calculateDutyCycle(vWheel * 3.6);
		updateDutyCycle();
		logDebugDutyCycle();

		resetPas(0);
	}
	if (timeS > 0.75 && pasActive) {
		logDebugInactive();
		resetPas(1);
	}
	return true;
}

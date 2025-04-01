/*
 * algorithm.c
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

#include "algorithm.h"

#include "adc.h"
#include "commands.h"
#include "gpio.h"
#include "helpers.h"
#include "logger.h"
#include "main.h"
#include "pas.h"
#include "sounds.h"
#include "stm32f4xx_hal.h"

float algorithm_eq_component = ALGORITHM_EQ_COMPONENT_DEFAULT;

float pasTimeS = 0.0;


// (-0.00000044 * (x*x*x*x*x)) - (0.000049 * (x*x*x*x)) + (0.00164 * (x*x*x)) +(
// 0.0169 * (x*x)) + (1.1815 * x) + 18.912;
void calculateDutyCycle(float x) {
    // ALGORITHM_EQ_FACTOR *
    // ((-0.00000044 * (x * x * x * x * x)) - (0.000049 * (x * x * x * x)) +
    //  (0.00164 * (x * x * x)) + (0.0169 * (x * x)) + (1.1815 * x) + 18.912 +
    //  algorithm_eq_component);  // algorithm_eq_component can now be changed
    //                            // via command

    float rawDutyCycle =
     ALGORITHM_EQ_FACTOR * 
      (2.857 * x + -5.0);

    if (rawDutyCycle <= MIN_DUTY_CYCLE)
        targetDutyCycle = 0;
    else if (rawDutyCycle > MAX_DUTY_CYCLE)
        targetDutyCycle = MAX_DUTY_CYCLE;
    else {
        if (rawDutyCycle > previousDutyCycle)
            targetDutyCycle = (DUTY_SMOOTH_FACTOR_UP * rawDutyCycle) +
                              ((1 - DUTY_SMOOTH_FACTOR_UP) * previousDutyCycle);
        else
            targetDutyCycle =
                (DUTY_SMOOTH_FACTOR_DOWN * rawDutyCycle) +
                ((1 - DUTY_SMOOTH_FACTOR_DOWN) * previousDutyCycle);
    }

    previousDutyCycle = targetDutyCycle;
}

bool runAlgorithm() {
    // If throttle is enabled run throttle algorithm
    if (throttleEnabled) {
        if (throttleVoltage < 0.9) {
            targetDutyCycle = 0;
            TIM1->PSC = 168 - 1;
        } else {
            targetDutyCycle = 58.824 * throttleVoltage - 49.0;

            // Definiujemy zakresy
            float rangeSize = (2.55 - 0.9) / 4.0;  // 4 równe zakresy
            float V_start = 0.9;

            // Obliczamy, w którym zakresie jesteśmy
            int rangeIndex = (throttleVoltage - V_start) / rangeSize;
            float V_local =
                throttleVoltage -
                (V_start +
                 rangeIndex * rangeSize);  // Lokalny poziom napięcia w zakresie

            // Definiujemy zakres PSC dla danego zakresu
            int PSC_max =
                209 + (rangeIndex * 30);  // Każdy okres piły ma wyższy PSC o 30
            int PSC_min =
                111 + (rangeIndex * 10);  // Dolna granica przesunięta o 10

            // Liniowe skalowanie PSC w zakresie
            TIM1->PSC = PSC_max - ((PSC_max - PSC_min) / rangeSize) * V_local;
        }
        updateDutyCycle();
        return true;
    }

    pasTimeS = ((float)HAL_GetTick() - (float)lastPasResetTick) / 1000.0;

    if (pasCounter >= 1) {
        // If bike is stationary, wait for 5 PAS counts before updating
        if (targetVelocityWheel == 0.0 && pasCounter < 5) {
            if (pasCounter < 4) lastPasResetTick = HAL_GetTick();
            return false;
        }

        omegaPedals = PAS_MAGNET_ANGLE / pasTimeS;
        targetOmegaWheel = omegaPedals * PEDAL_GEAR_RATIO;

        targetVelocityWheel = targetOmegaWheel * R_WHEEL;

        calculateDutyCycle(targetVelocityWheel * 3.6);
        updateDutyCycle();
        
// TODO fix debug by moving to main -> logDebugProc or sth
#ifdef DEBUG_ENABLED
        logDebugDegrees(pasTimeS);
        logDebugVWheel();
        logDebugDutyCycle();
#endif

        resetPas(0);
    }
    if (pasTimeS > PAS_INACTIVE_TIME_S && pasActive) {
#ifdef DEBUG_ENABLED
        logDebugInactive();
#endif
        resetPas(1);
        TIM1->PSC = 168 - 1;
    }
    return true;
}

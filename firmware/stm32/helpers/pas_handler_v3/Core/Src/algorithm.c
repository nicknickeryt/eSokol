/*
 * algorithm.c
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

#include "algorithm.h"
#include "gpio.h"
#include "helpers.h"
#include "logger.h"
#include "pas.h"
#include "sounds.h"
#include "main.h"
#include "commands.h"

#include "stm32f4xx_hal.h"

float algorithm_eq_component = ALGORITHM_EQ_COMPONENT_DEFAULT;

float pasTimeS = 0.0;

bool algorithmStartTonePlayed = false;

void playVVVFSound(float presc) {
  if(soundEnabled) __HAL_TIM_SET_PRESCALER(&htim3, presc);
}

// (-0.00000044 * (x*x*x*x*x)) - (0.000049 * (x*x*x*x)) + (0.00164 * (x*x*x)) +(
// 0.0169 * (x*x)) + (1.1815 * x) + 18.912;
void calculateDutyCycle(float x) {
  float rawDutyCycle =
      ALGORITHM_EQ_FACTOR *
      ((-0.00000044 * (x * x * x * x * x)) - (0.000049 * (x * x * x * x)) +
       (0.00164 * (x * x * x)) + (0.0169 * (x * x)) + (1.1815 * x) + 18.912 +
       algorithm_eq_component); // algorithm_eq_component can now be changed via
                                // command
  if (rawDutyCycle <= MIN_DUTY_CYCLE)
    targetDutyCycle = 0;
  else if (rawDutyCycle > WARN_DUTY_CYCLE)
    targetDutyCycle = 0;
  else if (rawDutyCycle > MAX_DUTY_CYCLE && rawDutyCycle < WARN_DUTY_CYCLE)
    targetDutyCycle = MAX_DUTY_CYCLE;
  else {
    if (rawDutyCycle > previousDutyCycle)
      targetDutyCycle = (DUTY_SMOOTH_FACTOR_UP * rawDutyCycle) +
                        ((1 - DUTY_SMOOTH_FACTOR_UP) * previousDutyCycle);
    else
      targetDutyCycle = (DUTY_SMOOTH_FACTOR_DOWN * rawDutyCycle) +
                        ((1 - DUTY_SMOOTH_FACTOR_DOWN) * previousDutyCycle);
  }

  if(rawDutyCycle > 28.0f && !isSoundPlaying())
    playVVVFSound(300 - 3*targetDutyCycle);

  else if (targetDutyCycle > 0.0f && previousDutyCycle == 0.0f && !algorithmStartTonePlayed && currentRealBikeVelocity < 10.0f) {
    playTone(SOUND_ALGORITHM_START);
    algorithmStartTonePlayed = true;
  }

  previousDutyCycle = targetDutyCycle;
}

bool runAlgorithm() {
  pasTimeS = ((float)HAL_GetTick() - (float)lastPasResetTick) / 1000.0;

  // If THR_DIS is set to high, then throttle is enabled, we don't need to run
  // algorithm.
  if (readPin(GPIOB, GPIO_PIN_3))
    return false;

  if (pasCounter >= 1) {
    // If bike is stationary, wait for 5 PAS counts before updating
    if (targetVelocityWheel == 0.0 && pasCounter < 5) {
      if (pasCounter < 4)
        lastPasResetTick = HAL_GetTick();
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
    algorithmStartTonePlayed = false;

    if(!isSoundPlaying()) __HAL_TIM_SET_PRESCALER(&htim3, 0);
  }
  return true;
}

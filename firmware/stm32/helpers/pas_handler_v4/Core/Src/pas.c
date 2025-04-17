/*
 * pas.h
 *
 * Contains pedal assist sensor helpers
 *
 *  Created on: Oct 27, 2024
 *      Author: nick
 */

#include "pas.h"
#include "algorithm.h"
#include "logger.h"
#include "uart.h"

#include "stm32f4xx_hal.h"

uint8_t pas_counter = 0;    // counts rising edges of pas signal
int pas_lastResetTick = 0;  // last pas counter reset (at 12 counters)

float omegaPedals = 0.0;
float targetOmegaWheel = 0.0;
float targetVelocityWheel = 0.0;

float pasActive = 0;

float targetDutyCycle = 0.0f;  // PWM duty cycle

// TODO proper .h/macro!!!
float previousDutyCycle = 0.0f;  // Przechowuje poprzednią wartość duty cycle

void resetDutyCycle() {
  TIM1->CCR1 = 0;
}

void updateDutyCycle() {
  TIM1->CCR1 = (targetDutyCycle / 100.0) * DUTY_PWM_MAX_CCR1;
}

void resetPas(bool inactive) {
  pas_lastResetTick = HAL_GetTick();

  if (inactive) {
    pasActive = 0;
    targetDutyCycle = 0.0;
    previousDutyCycle = 0.0;
    targetVelocityWheel = 0.0;  // Reset vWheel only if the bike is inactive!
    TIM1->CCR1 = 0;
  } else {
    pasActive = 1;
    pas_counter = 0;
  }
}

void logDebugDegrees(float timeS) {
  logger_sendChar("[DEBUG]: 30 stopni!\r\nczas: ");

  logger_sendFloat(timeS);
  logger_sendChar("sekund\r\n");
}

void logDebugVWheel() {
  logger_sendChar("vWheel: ");
  logger_sendFloat(targetVelocityWheel);
  logger_sendChar(" [m/s] ");
  logger_sendFloat(targetVelocityWheel * 3.6);
  logger_sendChar(" [km/h] \r\n");
}

void logDebugDutyCycle() {
  logger_sendChar("dutyCycle: ");
  logger_sendFloat(targetDutyCycle);
  logger_sendChar(" [%] \r\n");
  logger_sendChar("targetDutyCycle: ");
  logger_sendFloat(targetDutyCycle);
  logger_sendChar(" [%] \r\n");
}

void logDebugInactive() {
  logger_sendChar("[DEBUG]: ---- INACTIVE ----\r\n");
}

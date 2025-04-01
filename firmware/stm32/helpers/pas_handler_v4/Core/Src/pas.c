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

uint8_t pasCounter = 0;    // counts rising edges of pas signal
int lastPasResetTick = 0;  // last pas counter reset (at 12 counters)

float omegaPedals = 0.0;
float targetOmegaWheel = 0.0;
float targetVelocityWheel = 0.0;

float pasActive = 0;

float targetDutyCycle = 0.0;  // PWM duty cycle

// TODO proper .h/macro!!!
float previousDutyCycle = 0.0;  // Przechowuje poprzednią wartość duty cycle

void resetDutyCycle() {
  TIM1->CCR1 = 0;
}

void updateDutyCycle() {
  TIM1->CCR1 = (targetDutyCycle / 100.0) * DUTY_PWM_MAX_CCR1;
}

void resetPas(bool inactive) {
  lastPasResetTick = HAL_GetTick();

  if (inactive) {
    pasActive = 0;
    targetDutyCycle = 0.0;
    previousDutyCycle = 0.0;
    targetVelocityWheel = 0.0;  // Reset vWheel only if the bike is inactive!
    TIM1->CCR1 = 0;
  } else {
    pasActive = 1;
    pasCounter = 0;
  }
}

void logDebugDegrees(float timeS) {
  send_string("[DEBUG]: 30 stopni!\r\nczas: ");

  send_float(timeS);
  send_string("sekund\r\n");
}

void logDebugVWheel() {
  send_string("vWheel: ");
  send_float(targetVelocityWheel);
  send_string(" [m/s] ");
  send_float(targetVelocityWheel * 3.6);
  send_string(" [km/h] \r\n");
}

void logDebugDutyCycle() {
  send_string("dutyCycle: ");
  send_float(targetDutyCycle);
  send_string(" [%] \r\n");
  send_string("targetDutyCycle: ");
  send_float(targetDutyCycle);
  send_string(" [%] \r\n");
}

void logDebugInactive() {
  send_string("[DEBUG]: ---- INACTIVE ----\r\n");
}

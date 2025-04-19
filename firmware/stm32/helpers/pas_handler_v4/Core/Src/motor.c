#include "motor.h"

#include "algorithm.h"
#include "logger.h"
#include "stm32f4xx_hal.h"
#include "uart.h"

float targetDutyCycle = 0.0f;  // PWM duty cycle

void motor_resetDutyCycle() { TIM1->CCR1 = 0; }

void motor_setDutyCycle(float dutyCycle) {
    if (dutyCycle < MIN_DUTY_CYCLE) dutyCycle = MIN_DUTY_CYCLE;
    if (dutyCycle > MAX_DUTY_CYCLE) dutyCycle = MAX_DUTY_CYCLE;

    targetDutyCycle = dutyCycle;
}

float motor_getDutyCycle() { return targetDutyCycle; }

void motor_updateDutyCycle() {
    TIM1->CCR1 = (targetDutyCycle / 100.0) * DUTY_PWM_MAX_CCR1;
}
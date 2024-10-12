/*
 * helpers.c
 *
 * Contains UART and various other helpers
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

#include "helpers.h"
#include "main.h"

uint8_t pasCounter = 0;         			// counts rising edges of pas signal
int lastPasResetTick = 0;       	// last pas counter reset (at 12 counters)

float omegaPedals = 0.0;
float omegaWheel = 0.0;
float vWheel = 0.0;

float pasActive = 0;

float targetDutyCycle = 0.0;				// PWM duty cycle

// TODO proper .h/macro!!!
float previousDutyCycle = 0.0;  // Przechowuje poprzednią wartość duty cycle

void init() {
	lastPasResetTick = HAL_GetTick();

	TIM1->CCR1 = targetDutyCycle;
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

	startupAnim();
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0);
}

void send_string(char *s) {
	HAL_UART_Transmit(&huart1, (uint8_t*) s, strlen(s), 1000);
}

void send_uint8(uint8_t num) {
	char buffer[4];
	sprintf(buffer, "%u", num);
	send_string(buffer);
}

void send_int(int num) {
	char buffer[12];
	sprintf(buffer, "%d", num);
	send_string(buffer);
}

void send_float(float num) {
	int whole_part = (int) num; // Część całkowita
	int fractional_part = (int) ((num - whole_part) * 100); // Część ułamkowa do 2 miejsc po przecinku

	if (fractional_part < 0) {
		fractional_part = -fractional_part; // Dla ujemnych wartości
	}

	char buffer[20];
	sprintf(buffer, "%d.%02d", whole_part, fractional_part); // Formatowanie na część całkowitą i ułamkową
	send_string(buffer); // Wywołanie send_string
}

void logDebugDegrees(float timeS) {
	send_string("[DEBUG]: 30 stopni!\r\nczas: ");

	send_float(timeS);
	send_string("sekund\r\n");
}

void logDebugVWheel() {
	send_string("vWheel: ");
	send_float(vWheel);
	send_string(" [m/s] ");
	send_float(vWheel * 3.6);
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

void updateDutyCycle(void) {

	TIM1->CCR1 = (targetDutyCycle / 100.0) * DUTY_PWM_MAX_CCR1;
}

void resetPas(bool inactive) {
	lastPasResetTick = HAL_GetTick();

	if (inactive) {
		pasActive = 0;
		targetDutyCycle = 0.0;
		previousDutyCycle = 0.0;
		vWheel = 0.0;  // Reset vWheel only if the bike is inactive!
		TIM1->CCR1 = 0;
	} else {
		pasActive = 1;
		pasCounter = 0;
	}
}

void startupAnim() {
	for (uint8_t i = 0; i < 20; i++) {
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, 1);
		HAL_Delay(15);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, 0);
		HAL_Delay(15);
	}
	HAL_Delay(200);

	for (uint8_t i = 0; i < 4; i++) {
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, 1);
		HAL_Delay(75);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, 0);
		HAL_Delay(75);
	}

	HAL_Delay(200);

	for (uint8_t i = 0; i < 1; i++) {
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, 1);
		HAL_Delay(200);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, 0);
		HAL_Delay(200);
	}
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, 1);
}

bool checkSwitches() {
	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == 1) {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 1);
		return 0;
	} else {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0);
		return 1;
	}
}

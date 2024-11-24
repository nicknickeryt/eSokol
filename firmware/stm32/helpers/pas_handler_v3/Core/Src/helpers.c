/*
 * helpers.c
 *
 * Contains UART and various other helpers
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */
#include <stdbool.h>
#include <stdlib.h>

#include "animations.h"
#include "commands.h"
#include "gpio.h"
#include "helpers.h"
#include "logger.h"
#include "main.h"
#include "pas.h"
#include "sounds.h"
#include "uart.h"

bool bluetoothConnected = 0;

char* velocityBuffer;

void initVelocityBuffer() {
  velocityBuffer = (char*)malloc(18 * sizeof(char));
}

void bikeInit() {
  HAL_UARTEx_ReceiveToIdle_IT(&huart1, rxBuffer, 16);

  writePin(THR_SPORT_GPIO_Port, THR_SPORT_Pin, 1);
  lastPasResetTick = HAL_GetTick();

  TIM1->CCR1 = targetDutyCycle;
  TIM1->CCR4 = 500;

  HAL_TIM_PWM_Start(TIM_THROTTLE_LEDS, TIM_CHANNEL_1);  // OUT_PWM
  HAL_TIM_PWM_Start(TIM_THROTTLE_LEDS, TIM_CHANNEL_4);  // FRONT_COLD_PWM
  HAL_TIM_PWM_Start(TIM_SOUND, TIM_CHANNEL_1);          // SOUND

  playAnim(ANIM_STARTUP_PHASE1);
  writePin(GPIOB, GPIO_PIN_3, 0);  // Throttle default disabled

  if (readPin(BT_STATE_GPIO_Port, BT_STATE_Pin))
    bluetoothConnected = true;

  initStatusMessage();
  initVelocityBuffer();
  playTone(2);
}

void togglePWM(TIM_HandleTypeDef* htim, bool state) {
  if (state) {
    resetDutyCycle();
    HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1);
  } else {
    resetDutyCycle();
    HAL_TIM_PWM_Stop(htim, TIM_CHANNEL_1);
  }
}

char* float_to_char(float f) {
  static char buffer[20];

  int whole_part = (int)f;
  int fractional_part = (int)((f - whole_part) * 100);

  if (fractional_part < 0)
    fractional_part = -fractional_part;

  sprintf(buffer, "%d.%02d", whole_part, fractional_part);

  return buffer;
}

uint32_t hallLastTick = 0;
uint32_t hallLastSendTick = 0;
float currentRealBikeVelocity = 0;

float calculateRealBikeVelocity(uint32_t hallCurrTick) {
	float omega = (2.0f * PI) / ( (hallCurrTick - hallLastTick) / 1000.0f );
	hallLastTick = hallCurrTick;
	return omega * R_WHEEL * 3.6; // velocity in km/h
}

void setRealBikeVelocity(float velocity) {
	currentRealBikeVelocity = velocity;
}

void processRealVelocity() {
  if (!(HAL_GetTick() - hallLastSendTick > 300)) return;
  if (HAL_GetTick() - hallLastTick > 2500)
    currentRealBikeVelocity = 0.0f;

  sprintf(velocityBuffer, "eskl_evel%s\r\n", float_to_char(currentRealBikeVelocity));
  hallLastSendTick = HAL_GetTick();

  send_string(velocityBuffer);
}

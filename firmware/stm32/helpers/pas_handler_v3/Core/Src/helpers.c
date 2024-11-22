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

static uint32_t seed = 1;
uint32_t timeDummyVelocity = 0;

uint32_t generateRandomNum(void) {
  seed = (seed * 1103515245 + 12345) & 0x7fffffff;
  return seed % 31;
}

void processDummyVelocityData() {
  if (!(HAL_GetTick() - timeDummyVelocity > 5000))
    return;

  timeDummyVelocity = HAL_GetTick();
  uint8_t velocity = generateRandomNum();

  sprintf(velocityBuffer, "eskl_evel%d\r\n", velocity);

  send_string(velocityBuffer);
}

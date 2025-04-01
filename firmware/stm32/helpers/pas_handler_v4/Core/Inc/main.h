/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DEBUG_LED_Pin GPIO_PIN_13
#define DEBUG_LED_GPIO_Port GPIOC
#define BLINKER_RIGHT_IN_Pin GPIO_PIN_14
#define BLINKER_RIGHT_IN_GPIO_Port GPIOC
#define BLINKER_RIGHT_IN_EXTI_IRQn EXTI15_10_IRQn
#define BLINKER_LEFT_IN_Pin GPIO_PIN_15
#define BLINKER_LEFT_IN_GPIO_Port GPIOC
#define BLINKER_LEFT_IN_EXTI_IRQn EXTI15_10_IRQn
#define AMBIENTLIGHT_Pin GPIO_PIN_0
#define AMBIENTLIGHT_GPIO_Port GPIOA
#define BATT_SENSE_Pin GPIO_PIN_1
#define BATT_SENSE_GPIO_Port GPIOA
#define FRONT_WARM_Pin GPIO_PIN_2
#define FRONT_WARM_GPIO_Port GPIOA
#define THROTTLE_AIN_Pin GPIO_PIN_3
#define THROTTLE_AIN_GPIO_Port GPIOA
#define BLINKER_RIGHT_Pin GPIO_PIN_6
#define BLINKER_RIGHT_GPIO_Port GPIOA
#define BLINKER_LEFT_Pin GPIO_PIN_7
#define BLINKER_LEFT_GPIO_Port GPIOA
#define BATT_CURR_TODO_ENABLEME_Pin GPIO_PIN_1
#define BATT_CURR_TODO_ENABLEME_GPIO_Port GPIOB
#define OUT_PWM_Pin GPIO_PIN_8
#define OUT_PWM_GPIO_Port GPIOA
#define FRONT_COLD_PWM_Pin GPIO_PIN_11
#define FRONT_COLD_PWM_GPIO_Port GPIOA
#define REAR_LED_Pin GPIO_PIN_12
#define REAR_LED_GPIO_Port GPIOA
#define BULBS_Pin GPIO_PIN_15
#define BULBS_GPIO_Port GPIOA
#define SPEAKER_PWM_Pin GPIO_PIN_4
#define SPEAKER_PWM_GPIO_Port GPIOB
#define PAS_SIGNAL_Pin GPIO_PIN_5
#define PAS_SIGNAL_GPIO_Port GPIOB
#define PAS_SIGNAL_EXTI_IRQn EXTI9_5_IRQn
#define BT_STATE_Pin GPIO_PIN_8
#define BT_STATE_GPIO_Port GPIOB
#define BT_STATE_EXTI_IRQn EXTI9_5_IRQn
#define HALL_SPEED_Pin GPIO_PIN_9
#define HALL_SPEED_GPIO_Port GPIOB
#define HALL_SPEED_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* Global variables ----------------------------------------------------------*/
extern ADC_HandleTypeDef hadc1;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;

extern UART_HandleTypeDef huart1;

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

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
#define BLINK_Pin GPIO_PIN_13
#define BLINK_GPIO_Port GPIOC
#define FRONT_COLD_Pin GPIO_PIN_15
#define FRONT_COLD_GPIO_Port GPIOC
#define REAR_LED_Pin GPIO_PIN_13
#define REAR_LED_GPIO_Port GPIOB
#define THR_SWITCH_Pin GPIO_PIN_14
#define THR_SWITCH_GPIO_Port GPIOB
#define OUT_PWM_Pin GPIO_PIN_8
#define OUT_PWM_GPIO_Port GPIOA
#define THR_DIS_Pin GPIO_PIN_3
#define THR_DIS_GPIO_Port GPIOB
#define PAS_SIGNAL_Pin GPIO_PIN_5
#define PAS_SIGNAL_GPIO_Port GPIOB
#define PAS_SIGNAL_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */


/* Global variables ----------------------------------------------------------*/
extern TIM_HandleTypeDef htim1;
extern UART_HandleTypeDef huart1;

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

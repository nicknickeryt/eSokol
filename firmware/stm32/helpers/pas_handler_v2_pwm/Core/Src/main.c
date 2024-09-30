/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define LOG_VERBOSE
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


// DEBUG UART HELPERS
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

// GLOBAL VARS

uint8_t pasCounter = 0;         // counts rising edges of pas signal
int lastPasResetTick = 0;       // last pas counter reset (at 12 counters)
const float pedalGearRatio = 2.875;   // this is ratio to calculate target speed, 46:16 (pedals gear, wheel gear)
const float motorGearRatio = 0.5625;  // this is ratio to calculate target speed, 9:16 (motor gear, wheel gear)

const float pi = 3.141592;
const float pasMagnetAngle = (2.0  * pi) / 12.0; // pas angle after which we calculate omega. 2pi/12 is 30° (we have 12 magnets)

const float rWheel = 0.25; 			// wheel radius [m]

float omegaPedals = 0.0;
float omegaWheel = 0.0;
float vWheel = 0.0;
float lastVWheel = 0.0;

float pasActive = 0;

// duty cycle
float targetDutyCycle = 0.0;

// (-0.00000044 * (x*x*x*x*x)) - (0.000049 * (x*x*x*x)) + (0.00164 * (x*x*x)) +( 0.0169 * (x*x)) + (1.1815 * x) + 18.912

const float minDutyCycle = 16.0;
const float maxDutyCycle = 66.0;
const float warnDutyCycle = 80.0;

// x => speedKmh
void calculateDutyCycle(float x) {
	float dutyCycle = (-0.00000044 * (x*x*x*x*x)) - (0.000049 * (x*x*x*x)) + (0.00164 * (x*x*x)) +( 0.0169 * (x*x)) + (1.1815 * x) + 18.912;
	if(dutyCycle <= minDutyCycle) {
		targetDutyCycle = 0;
	}
	else if(dutyCycle > warnDutyCycle) {
		targetDutyCycle = 0;
	}
	else if(dutyCycle > maxDutyCycle && dutyCycle < warnDutyCycle) {
		targetDutyCycle = maxDutyCycle;
	}
	else targetDutyCycle = dutyCycle;
}

void updateDutyCycle(void) {
	TIM1->CCR1 = targetDutyCycle;
}


void resetPas(int inactive) {
	pasCounter = 0;
	lastPasResetTick = HAL_GetTick();

	if(inactive) {
		lastPasResetTick = HAL_GetTick();
		pasActive = 0;

		targetDutyCycle = 0.0;

		TIM1->CCR1 = 0;
	}
	else pasActive = 1;
}



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == GPIO_PIN_5) {

		pasCounter++;

		// lub od razu liczymy omege, ale pamietajmy o tym, by byly co najmniej 2 cykle
		// od jakichs 5 sekund, jak w ciagu 5 sekund nie ma interrupta, resetujemy wszystki i czekamy na 2 w counterze
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

  // inicjalizacja milis
  lastPasResetTick = HAL_GetTick();

  // inicjalizacja PWM
  TIM1->CCR1 = targetDutyCycle;
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {

		// aktualnie jest to z dokladnoscia 360°/12 = 30° ze wzgledu na 12 magnesów.
		int currTick = HAL_GetTick();
		float timeS = ((float) currTick - (float) lastPasResetTick) / 1000.0;

		if (pasCounter >= 1) {
			send_string("[DEBUG]: 30 stopni!\r\nczas: ");


#ifdef LOG_VERBOSE
			send_string("\r\n tick: ");
			send_int(currTick);
			send_string("\r\n lastTick: ");
			send_int(lastPasResetTick);
			send_string("\r\n diffTick: ");
			send_int(currTick - lastPasResetTick);
			send_string("\r\n");
#endif


			send_float(timeS);
			send_string("sekund\r\n");

			omegaPedals = pasMagnetAngle / timeS;
			omegaWheel = omegaPedals * pedalGearRatio;

#ifdef LOG_VERBOSE
			send_string("omegaPedals: ");
			send_float(omegaPedals);
			send_string("\r\n");

			send_string("omegaWheel: ");
			send_float(omegaWheel);
			send_string("\r\n");
#endif

			vWheel = omegaWheel * rWheel;

			send_string("vWheel: ");
			send_float(vWheel);
			send_string(" [m/s] ");
			send_float(vWheel * 3.6);
			send_string(" [km/h] \r\n");


			// Duty cycle calc
			calculateDutyCycle(vWheel * 3.6);
			updateDutyCycle();

		send_string("dutyCycle: ");
			send_float(targetDutyCycle);
			send_string(" [%] \r\n");
			send_string("targetDutyCycle: ");
			send_float(targetDutyCycle);
			send_string(" [%] \r\n");

			resetPas(0);
		}
		if(timeS > 0.75 && pasActive) {
			send_string("[DEBUG]: ---- INACTIVE ----\r\n");

			resetPas(1);
		}


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 72-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 100-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : PAS_SIGNAL_Pin */
  GPIO_InitStruct.Pin = PAS_SIGNAL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(PAS_SIGNAL_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

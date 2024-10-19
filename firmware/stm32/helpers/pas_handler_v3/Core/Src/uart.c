#include "uart.h"
#include "helpers.h"
#include "main.h"
#include <stdbool.h>

uint8_t rxBuffer[16]; // 14 znaków + 2 znaki ([CR] [LF])

void send_string(char *s)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)s, strlen(s), 1000);
}

void send_uint8(uint8_t num)
{
	char buffer[4];
	sprintf(buffer, "%u", num);
	send_string(buffer);
}

void send_int(int num)
{
	char buffer[12];
	sprintf(buffer, "%d", num);
	send_string(buffer);
}

void send_float(float num)
{
	int whole_part = (int)num;
	int fractional_part = (int)((num - whole_part) * 100);

	if (fractional_part < 0)
	{
		fractional_part = -fractional_part;
	}

	char buffer[20];
	sprintf(buffer, "%d.%02d", whole_part, fractional_part);
	send_string(buffer);
}

typedef struct
{
	const char *command;
	void (*action)(void);
} Command;

void animStart() { shouldStartAnim = 1; }
void toggleFrontC() { HAL_GPIO_TogglePin(FRONT_COLD_GPIO_Port, FRONT_COLD_Pin); }
void toggleRearLED() { HAL_GPIO_TogglePin(REAR_LED_GPIO_Port, REAR_LED_Pin); }
void toggleHandle() { HAL_GPIO_TogglePin(THR_DIS_GPIO_Port, THR_DIS_Pin); }

Command commands[] = {
	{"eskl_animStart\r\n", animStart},
	{"eskl_frontCTog\r\n", toggleFrontC},
	{"eskl_rearLETog\r\n", toggleRearLED},
	{"eskl_handleTog\r\n", toggleHandle}};

void handleRxInterrupt(UART_HandleTypeDef *huart, uint16_t size)
{
	if (huart != &huart1 || size == 0)
	{
		HAL_UARTEx_ReceiveToIdle_IT(huart, rxBuffer, sizeof(rxBuffer));
		return;
	}

	static uint8_t Data[26]; // Zwiększ rozmiar na dłuższy ciąg (14 + 2 + 10 dla "rxBuffer ")

	for (uint32_t i = 0; i < sizeof(commands) / sizeof(Command); i++)
	{
		if (strncmp((char *)rxBuffer, commands[i].command, strlen(commands[i].command)) == 0)
		{
			commands[i].action(); // Call the corresponding action
			break;
		}
	}

	// Weird HC-05 connection status strings are actually useful
	if (strstr((char *)rxBuffer, "+DISC") != NULL) {
		shouldDisconnectedAnim = 1;
	}
	else if (strstr((char *)rxBuffer, "+CONNEC") != NULL) {
		shouldConnectedAnim = 1;
	}

	snprintf((char *)Data, sizeof(Data), "Received: %s", rxBuffer);

	HAL_UART_Transmit_IT(huart, Data, strlen((char *)Data));

	HAL_UARTEx_ReceiveToIdle_IT(huart, rxBuffer, sizeof(rxBuffer));
}
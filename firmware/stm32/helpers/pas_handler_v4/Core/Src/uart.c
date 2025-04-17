/*
 * uart.c
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

#include "uart.h"

#include <stdbool.h>
#include <stdlib.h>

#include "animations.h"
#include "commands.h"
#include "gpio.h"
#include "helpers.h"
#include "logger.h"
#include "main.h"
#include "sounds.h"

uint8_t rxBuffer[16];

char* currentMessage = "eskl_initCompl\r\n";

bool shouldSendStatus = false;

uint32_t shouldSendStatus1sTime = 0;
bool shouldSendStatus1s = false;

void uart_handleRxInterrupt(UART_HandleTypeDef* huart, uint16_t size) {
    if (huart != &huart1 || size == 0) {
        HAL_UARTEx_ReceiveToIdle_IT(huart, rxBuffer, sizeof(rxBuffer));
        return;
    }

    for (uint32_t i = 0; i < sizeof(commands) / sizeof(Command); i++) {
        if (strncmp((char*)rxBuffer, commands[i].command,
                    strlen(commands[i].command)) == 0) {
            if (commands[i].action == NULL) {
                send_string("ERROR: NULL action pointer!\r\n");
                return;
            }
            commands[i].action();
            break;
        }
    }

    // FIXME well that is currently done terribly wrong
    for (uint32_t i = 0; i < sizeof(variableCommands) / sizeof(VariableCommand);
         i++) {
        if (strncmp((char*)rxBuffer, variableCommands[i].command,
                    strlen(variableCommands[i].command)) == 0) {
            uint16_t arg = (rxBuffer[10] - '0') * 100 +
                           (rxBuffer[11] - '0') * 10 + (rxBuffer[12] - '0');
            variableCommands[i].action(arg);
            break;
        }
    }

    HAL_UARTEx_ReceiveToIdle_IT(huart, rxBuffer, sizeof(rxBuffer));
}

void status_proc() {
    sendStatus1s();
    if (shouldSendStatus || shouldSendStatus1s) {
        shouldSendStatus = false, shouldSendStatus1s = false;
        sendStatus();
    }
}

void sendStatus1s() {
    shouldSendStatus1s = HAL_GetTick() - shouldSendStatus1sTime > 100;
    if (shouldSendStatus1s) shouldSendStatus1sTime = HAL_GetTick();
}
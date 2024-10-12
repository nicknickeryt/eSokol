#pragma once

#include "stm32f4xx.h"

#define DS_GPIO_Port GPIOB
#define DS_Pin GPIO_PIN_4


// Inicjalizacja 1-wire
// Start licznika używanego do opóźnień
// return - HAL_OK/HAL_ERROR zależnie od stanu licznika
HAL_StatusTypeDef wire_init(TIM_HandleTypeDef *htim);

// Przesłanie sekwencji reset przez 1-wire
// return - HAL_OK/HAL_ERROR zależnie od odp. czujnika
HAL_StatusTypeDef wire_reset(void);

// Odczytanie danych przez 1-wire
// Czyta 8 bitów i połącz w bajt
// return - odczytany bajt
uint8_t wire_read(void);

// Wyślij dane przez 1-wire
// byte - bajt do wysłania
void wire_write(uint8_t byte);

// Policz sumę kontrolną
// data - blok danych
// len - długość bloku danych
uint8_t wire_crc(const uint8_t* data, int len);

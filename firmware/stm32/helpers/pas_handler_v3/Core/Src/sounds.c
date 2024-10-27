/*
 * sounds.c
 *
 * Contains sounds helpers
 *
 *  Created on: Oct 26, 2024
 *      Author: nick
 */

#include "main.h"
#include "sounds.h"
#include "helpers.h"

#include "stm32f4xx_hal.h"

uint32_t sound_mario[] = {
    659, 0, 659, 0, 659, 0, 523, 659, 784, 392, 0};
uint32_t sound_mario_dur[] = {
    150, 25, 300, 25, 150, 150, 150, 300, 600, 600, 300};

uint32_t sound_connected[] = {
    523, 440, 349, 659};
uint32_t sound_connected_dur[] = {
    250, 125, 125, 250};

uint32_t sound_disconnected[] = {
    440, 349, 330};
uint32_t sound_disconnected_dur[] = {
    125, 125, 250};

uint32_t sound_on[] = {
    523, 659};
uint32_t sound_on_dur[] = {
    125, 62};

uint32_t sound_off[] = {
    659, 587};
uint32_t sound_off_dur[] = {
    125, 62};


uint32_t presForFrequency(uint32_t frequency)
{
    return frequency == 0 ? 0 : (TIM3_FREQ / (1000 * frequency));
}

void noTone()
{
    __HAL_TIM_SET_PRESCALER(&htim3, 0);
}

// Deklaracja zmiennych globalnych dla sekwencji dźwięków
uint32_t *currentToneSequence = NULL;
uint32_t *currentDurationSequence = NULL;
uint32_t sequenceSize = 0;
uint32_t currentIndex = 0;
uint32_t toneStartTime = 0;
bool isToneActive = 0;

void startToneSequence(uint32_t *tone, uint32_t *duration, uint32_t size)
{
    currentToneSequence = tone;
    currentDurationSequence = duration;
    sequenceSize = size;
    currentIndex = 0;
    isToneActive = 0;
}

void processTone()
{
    if (currentToneSequence == NULL || currentIndex >= sequenceSize) {
        return; // Brak sekwencji lub zakończenie odtwarzania
    }

    uint32_t now = HAL_GetTick();

    // Sprawdzamy, czy czas trwania tonu upłynął
    if (isToneActive && (now - toneStartTime >= currentDurationSequence[currentIndex])) {
        noTone();  // Zatrzymujemy ton
        isToneActive = 0; // Przechodzimy do pauzy
        currentIndex++;
    }

    // Rozpoczynamy nowy ton
    if (!isToneActive && currentIndex < sequenceSize) {
        int frequency = currentToneSequence[currentIndex];
        if (frequency > 0) {
            int prescaler = presForFrequency(frequency);
            __HAL_TIM_SET_PRESCALER(&htim3, prescaler);
            toneStartTime = HAL_GetTick(); // Ustawienie nowego czasu startu
            isToneActive = 1;
        } else {
            // Jeśli częstotliwość to 0, pauzujemy bez dźwięku
            toneStartTime = HAL_GetTick();
            isToneActive = 1;
        }
    }
}


void playTone(uint8_t number)
{
    switch (number)
    {
    case 1:
        startToneSequence(sound_mario, sound_mario_dur, (sizeof(sound_mario) / sizeof(sound_mario[0])));
        break;
    case 2:
        startToneSequence(sound_connected, sound_connected_dur, (sizeof(sound_connected) / sizeof(sound_connected[0])));
        break;
    case 3:
        startToneSequence(sound_disconnected, sound_disconnected_dur, (sizeof(sound_disconnected) / sizeof(sound_disconnected[0])));
        break;
    case 4:
        startToneSequence(sound_on, sound_on_dur, (sizeof(sound_on) / sizeof(sound_on[0])));
        break;
    case 5:
        startToneSequence(sound_off, sound_off_dur, (sizeof(sound_off_dur) / sizeof(sound_off_dur[0])));
        break;
    default:
        break;
    }
}

void playToggleSound(bool state) {
    
	uint8_t tone = state == true ? SOUND_ON : SOUND_OFF;
    playTone(tone);

}

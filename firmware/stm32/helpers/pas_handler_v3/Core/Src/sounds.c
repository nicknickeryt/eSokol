/*
 * sounds.c
 *
 * Contains sounds helpers
 *
 *  Created on: Oct 26, 2024
 *      Author: nick
 */

#include "sounds.h"
#include "commands.h"
#include "helpers.h"
#include "main.h"

#include "stm32f4xx_hal.h"

uint32_t sound_mario[] = {659, 0, 659, 0, 659, 0, 523, 659, 784, 392, 0};
uint32_t sound_mario_dur[] = {150, 25,  300, 25,  150, 150,
                              150, 300, 600, 600, 300};

uint32_t sound_windows_xp_startup[] = {
    440, 494, 523, 440, 392, 440, 494, 523, 0, 392, 440, 494, 0, 440, 392, 349
};
uint32_t sound_windows_xp_startup_dur[] = {
    200, 200, 300, 150, 150, 200, 200, 300, 100, 200, 200, 300, 100, 200, 200, 400
};

uint32_t sound_connected[] = {523, 440, 349, 659};
uint32_t sound_connected_dur[] = {250, 125, 125, 250};

uint32_t sound_disconnected[] = {440, 349, 330};
uint32_t sound_disconnected_dur[] = {125, 125, 250};

uint32_t sound_err[] = {370, 0, 370};
uint32_t sound_err_dur[] = {62, 25, 125};

uint32_t sound_on[] = {523, 659};
uint32_t sound_on_dur[] = {125, 62};

uint32_t sound_off[] = {659, 587};
uint32_t sound_off_dur[] = {125, 62};

uint32_t sound_click_on[] = {1000, 800, 0, 600}; 
uint32_t sound_click_on_dur[] = {5, 3, 2, 5}; 

uint32_t sound_click_off[] = {800, 600, 0, 400}; 
uint32_t sound_click_off_dur[] = {5, 3, 2, 5};

// uint32_t sound_algorithm_start[] = {1000}; 
// uint32_t sound_algorithm_start_dur[] = {500};

// 110-147-197-223-234-262-294-312-350-400Hz
// or (E♭)-F-G-A-B-C-D-E♭-F-G
// uint32_t sound_algorithm_start[] = {175, 196, 223, 233, 262, 294, 311, 350, 400};
// uint32_t sound_algorithm_start_dur[] = {500, 200, 300, 350, 300, 250, 200, 150, 1000};

uint32_t sound_algorithm_start[] = {311, 349, 392, 440, 494, 523, 587, 622, 698, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784, 0, 784};
uint32_t sound_algorithm_start_dur[] = {20*8, 20*8, 20*7, 20*5, 20*5, 20*4, 20*4, 20*6, 20*7, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 18, 18, 18, 18, 18, 18, 18, 18, 18, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 16, 16, 16, 16, 16, 16, 16, 16, 16, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 14, 14, 14, 14, 14, 14, 14, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};

uint32_t presForFrequency(uint32_t frequency) {
  return frequency == 0 ? 0 : (TIM3_FREQ / (TIM3_MAX_CNT * frequency));
}

void noTone() {
  __HAL_TIM_SET_PRESCALER(TIM_SOUND, 0);
}

uint32_t* currentToneSequence = NULL;
uint32_t* currentDurationSequence = NULL;
uint32_t sequenceSize = 0;
uint32_t currentIndex = 0;
uint32_t toneStartTime = 0;
bool isToneActive = 0;

void startToneSequence(uint32_t* tone, uint32_t* duration, uint32_t size) {
  currentToneSequence = tone;
  currentDurationSequence = duration;
  sequenceSize = size;
  currentIndex = 0;
  isToneActive = 0;
}

void processTone() {
  if (currentToneSequence == NULL || currentIndex >= sequenceSize)
    return;

  uint32_t now = HAL_GetTick();

  if (isToneActive && (now - toneStartTime >= currentDurationSequence[currentIndex])) {
    noTone();         
    isToneActive = 0; 
    currentIndex++;
  }

  if (!isToneActive && currentIndex < sequenceSize) {
    int frequency = currentToneSequence[currentIndex];
    if (frequency > 0) {
      int prescaler = presForFrequency(frequency);
      __HAL_TIM_SET_PRESCALER(TIM_SOUND, prescaler);
      toneStartTime = HAL_GetTick();  
      isToneActive = 1;
    } else {
      toneStartTime = HAL_GetTick();
      isToneActive = 1;
    }
  }
}

void playTone(uint8_t number) {
  if (!soundEnabled)
    return;
  switch (number) {
    case SOUND_MARIO:
      startToneSequence(sound_mario, sound_mario_dur,
                        (sizeof(sound_mario) / sizeof(sound_mario[0])));
      break;
    case SOUND_CONNECTED:
      startToneSequence(sound_connected, sound_connected_dur,
                        (sizeof(sound_connected) / sizeof(sound_connected[0])));
      break;
    case SOUND_DISCONNECTED:
      startToneSequence(
          sound_disconnected, sound_disconnected_dur,
          (sizeof(sound_disconnected) / sizeof(sound_disconnected[0])));
      break;
    case SOUND_ON:
      startToneSequence(sound_on, sound_on_dur,
                        (sizeof(sound_on) / sizeof(sound_on[0])));
      break;
    case SOUND_OFF:
      startToneSequence(sound_off, sound_off_dur,
                        (sizeof(sound_off_dur) / sizeof(sound_off_dur[0])));
      break;
    case SOUND_ERR:
      startToneSequence(sound_err, sound_err_dur,
                        (sizeof(sound_err_dur) / sizeof(sound_err_dur[0])));
      break;
    case SOUND_CLICK_ON:
      startToneSequence(sound_click_on, sound_click_on_dur,
                        (sizeof(sound_click_on_dur) / sizeof(sound_click_on_dur[0])));
      break;
    case SOUND_CLICK_OFF:
      startToneSequence(sound_click_off, sound_click_off_dur,
                        (sizeof(sound_click_off_dur) / sizeof(sound_click_off_dur[0])));
      break;
    case SOUND_ALGORITHM_START:
      startToneSequence(sound_algorithm_start, sound_algorithm_start_dur,
                        (sizeof(sound_algorithm_start_dur) / sizeof(sound_algorithm_start_dur[0])));
      break;
    case 10:
      startToneSequence(sound_windows_xp_startup, sound_windows_xp_startup_dur,
                        (sizeof(sound_windows_xp_startup_dur) / sizeof(sound_windows_xp_startup_dur[0])));
      break;
    default:
      break;
  }
}

void playToggleSound(bool state) {
  uint8_t tone = state == true ? SOUND_ON : SOUND_OFF;
  playTone(tone);
}

bool isSoundPlaying() {
  return isToneActive;
}
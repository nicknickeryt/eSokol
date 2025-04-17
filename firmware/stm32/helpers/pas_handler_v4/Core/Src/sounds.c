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

uint32_t sound_rudolf_czerwononosy[] = {392, 440, 392, 330, 523, 440, 392};

uint32_t sound_rudolf_czerwononosy_dur[] = {125, 250, 125, 250, 250, 250, 375};

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

void sound_nothing() { __HAL_TIM_SET_PRESCALER(TIM_SOUND, 0); }

uint32_t* currentToneSequence = NULL;
uint32_t* currentDurationSequence = NULL;
uint32_t sequenceSize = 0;
uint32_t currentIndex = 0;
uint32_t toneStartTime = 0;
bool isToneActive = 0;

uint32_t sound_getPSCForFreq(uint32_t frequency) {
    return frequency == 0 ? 0 : (TIM3_FREQ / (TIM3_MAX_CNT * frequency));
}

void sound_startSequence(uint32_t* tone, uint32_t* duration, uint32_t size) {
    currentToneSequence = tone;
    currentDurationSequence = duration;
    sequenceSize = size;
    currentIndex = 0;
    isToneActive = 0;
}

void sound_proc() {
    if (currentToneSequence == NULL || currentIndex >= sequenceSize) return;

    uint32_t now = HAL_GetTick();

    if (isToneActive &&
        (now - toneStartTime >= currentDurationSequence[currentIndex])) {
        sound_nothing();
        isToneActive = 0;
        currentIndex++;
    }

    if (!isToneActive && currentIndex < sequenceSize) {
        int frequency = currentToneSequence[currentIndex];
        if (frequency > 0) {
            int prescaler = sound_getPSCForFreq(frequency);
            __HAL_TIM_SET_PRESCALER(TIM_SOUND, prescaler);
            toneStartTime = HAL_GetTick();
            isToneActive = 1;
        } else {
            toneStartTime = HAL_GetTick();
            isToneActive = 1;
        }
    }
}

void sound_play(uint8_t number) {
    if (!soundEnabled) return;
    switch (number) {
        case SOUND_MARIO:
            sound_startSequence(sound_mario, sound_mario_dur,
                              (sizeof(sound_mario) / sizeof(sound_mario[0])));
            break;
        case SOUND_CONNECTED:
            sound_startSequence(
                sound_connected, sound_connected_dur,
                (sizeof(sound_connected) / sizeof(sound_connected[0])));
            break;
        case SOUND_DISCONNECTED:
            sound_startSequence(
                sound_disconnected, sound_disconnected_dur,
                (sizeof(sound_disconnected) / sizeof(sound_disconnected[0])));
            break;
        case SOUND_ON:
            sound_startSequence(sound_on, sound_on_dur,
                              (sizeof(sound_on) / sizeof(sound_on[0])));
            break;
        case SOUND_OFF:
            sound_startSequence(
                sound_off, sound_off_dur,
                (sizeof(sound_off_dur) / sizeof(sound_off_dur[0])));
            break;
        case SOUND_ERR:
            sound_startSequence(
                sound_err, sound_err_dur,
                (sizeof(sound_err_dur) / sizeof(sound_err_dur[0])));
            break;
        case SOUND_CLICK_ON:
            sound_startSequence(
                sound_click_on, sound_click_on_dur,
                (sizeof(sound_click_on_dur) / sizeof(sound_click_on_dur[0])));
            break;
        case SOUND_CLICK_OFF:
            sound_startSequence(
                sound_click_off, sound_click_off_dur,
                (sizeof(sound_click_off_dur) / sizeof(sound_click_off_dur[0])));
            break;
        case SOUND_RUDOLF:
            sound_startSequence(sound_rudolf_czerwononosy,
                              sound_rudolf_czerwononosy_dur,
                              (sizeof(sound_rudolf_czerwononosy_dur) /
                               sizeof(sound_rudolf_czerwononosy_dur[0])));
            break;
        default:
            break;
    }
}

void sound_playToggle(bool state) {
    uint8_t tone = state == true ? SOUND_ON : SOUND_OFF;
    sound_play(tone);
}

bool sound_isPlaying() { return isToneActive; }
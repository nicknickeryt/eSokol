/*
 * sounds.h
 *
 * Contains sounds helpers
 *
 *  Created on: Oct 26, 2024
 *      Author: nick
 */

#ifndef __SOUNDS_H
#define __SOUNDS_H

#include <stdbool.h>
#include <stdint.h>

#define TIM3_FREQ 84000000
#define TIM3_MAX_CNT 1000

#define SOUND_MARIO 1

#define SOUND_CONNECTED 2
#define SOUND_DISCONNECTED 3

#define SOUND_ON 4
#define SOUND_OFF 5

#define SOUND_ERR 6
#define SOUND_CLICK_ON 7
#define SOUND_CLICK_OFF 8

#define SOUND_RUDOLF 10

uint32_t presForFrequency(uint32_t frequency);
void noTone(void);

void startToneSequence(uint32_t* tone, uint32_t* duration, uint32_t size);
void processTone(void);

void playTone(uint8_t number);

void playToggleSound(bool state);

bool isSoundPlaying(void);

#endif /* __SOUNDS_H */
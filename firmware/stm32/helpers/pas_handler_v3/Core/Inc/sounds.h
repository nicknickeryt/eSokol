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

#define TIM3_FREQ 84000000

#define SOUND_MARIO 1

#define SOUND_CONNECTED 2
#define SOUND_DISCONNECTED 3

#define SOUND_ON 4
#define SOUND_OFF 5

uint32_t presForFrequency(uint32_t frequency);
void noTone(void);

void startToneSequence(uint32_t *tone, uint32_t *duration, uint32_t size);
void processTone();

void playTone(uint8_t number);

void playToggleSound(bool state);

#endif /* __SOUNDS_H */
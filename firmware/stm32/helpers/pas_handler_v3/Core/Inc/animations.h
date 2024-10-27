/*
 * animations.h
 *
 * Contains light animation helpers
 *
 *  Created on: Oct 12, 2024
 *      Author: nick
 */

#ifndef __ANIMATIONS_H
#define __ANIMATIONS_H

void blink1S(void);

void processAnimation(void);

typedef enum
{
	ANIM_IDLE,
	ANIM_STARTUP_PHASE1,
	ANIM_STARTUP_PHASE2,
	ANIM_STARTUP_PHASE3,
	ANIM_CONNECTED,
	ANIM_DISCONNECTED,
	ANIM_BLINK_DISCONNECTED
} AnimState;

void playAnim(AnimState animState);

#endif /* __ANIMATIONS_H */
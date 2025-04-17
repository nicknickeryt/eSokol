/*
 * blinkers.h
 *
 *  Created on: Dec 3, 2024
 *      Author: nick
 */

#ifndef __BLINKERS_H
#define __BLINKERS_H

extern bool blinkerLeftPinState;
extern bool blinkerRightPinState;

void toggleLeftBlinker(void);
void toggleRightBlinker(void);
void toggleBothBlinkers(void);
void blinkers_enableLeft(void);
void blinkers_enableRight(void);

void blinkers_init(void);
void blinkers_reset(void);
void blinkers_proc(void);

#endif /* __BLINKERS_H */
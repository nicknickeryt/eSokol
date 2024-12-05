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

void initBlinkers(void);
void processBlinkers(void);

#endif /* __BLINKERS_H */
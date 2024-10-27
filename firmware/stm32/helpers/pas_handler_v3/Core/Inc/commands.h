/*
 * commands.h
 *
 * Contains bike feature commands
 *
 *  Created on: Oct 27, 2024
 *      Author: nick
 */

#ifndef __COMMANDS_H
#define __COMMANDS_H

#include <stdbool.h>

typedef struct
{
	const char *command;
	void (*action)(void);
} Command;

extern Command commands[8];

extern bool frontColdEnabled;
extern bool frontWarmEnabled;
extern bool rearEnabled;
extern bool throttleEnabled;
extern bool sportModeDisabled;
extern bool soundEnabled;

void animStart(void);
void toggleFrontC(void);
void toggleFrontW(void);
void toggleRearLED(void);
void toggleThrottle(void);
void toggleSportMode(void);
void toggleSound(void);
void sendStatus(void);

#endif /* __COMMANDS_H */
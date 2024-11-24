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
#include <stdint.h>

typedef struct {
  const char *command;
  void (*action)(void);
} Command;

typedef struct {
  const char *command;
  void (*action)(uint16_t arg);
} VariableCommand;

extern Command commands[9];

extern VariableCommand variableCommands[1];

extern bool frontColdEnabled;
extern uint16_t frontColdBrightness;
extern bool frontWarmEnabled;
extern bool rearEnabled;
extern bool throttleEnabled;
extern bool sportModeDisabled;
extern bool soundEnabled;
extern bool bulbsEnabled;

void initStatusMessage(void);
void animStart(void);
void toggleFrontCold(void);
void toggleFrontColdNoSound(void);  // TODO CHANGE THIS AAAA
void enableFrontColdNoSound();
void disableFrontColdNoSound();
void setFrontColdBrightness(uint16_t brightness);
void toggleFrontWarm(void);
void toggleRearLED(void);
void toggleThrottle(void);
void toggleSportMode(void);
void toggleSound(void);
void toggleBulbs(void);

void sendStatus(void);

#endif /* __COMMANDS_H */
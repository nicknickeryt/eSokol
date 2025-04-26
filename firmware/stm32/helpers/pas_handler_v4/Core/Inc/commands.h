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
  void (*action)();
} Command;

typedef struct {
  const char *command;
  void (*action)(uint16_t arg);
} VariableCommand;

extern Command commands[17];

extern VariableCommand variableCommands[1];

extern bool frontColdEnabled;
extern uint16_t frontColdBrightness;
extern bool frontWarmEnabled;
extern bool rearEnabled;
extern bool throttleEnabled;
extern bool pasEnabled;
extern bool sportModeDisabled;
extern bool soundEnabled;
extern bool bulbsEnabled;
extern bool ambientLightEnabled;

void initStatusMessage();
void anim_start();

void toggleFrontCold();
void toggleFrontColdNoSound();  // TODO CHANGE THIS AAAA
void enableFrontColdNoSound();
void disableFrontColdNoSound();
void setFrontColdBrightness(uint16_t brightness);
void toggleFrontWarm();
void toggleRearLED();
void enableRearLEDNoSound();
void disableRearLEDNoSound();


void toggleThrottle();
void toggleSound();
void toggleBulbs();

void blinkBlinkerLeft();
void blinkBlinkerRight();
void blinkBlinkerBoth();

void toggleAmbientLight();

void algorithmComponentIncrement();
void algorithmComponentDecrement();
void algorithmToggleEnabled();

void sendStatus();

#endif /* __COMMANDS_H */
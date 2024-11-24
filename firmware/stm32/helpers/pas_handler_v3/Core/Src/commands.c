/*
 * commands.h
 *
 * Contains bike feature commands
 *
 *  Created on: Oct 27, 2024
 *      Author: nick
 */

#include <stdlib.h>

#include "animations.h"
#include "commands.h"
#include "gpio.h"
#include "helpers.h"
#include "logger.h"
#include "main.h"
#include "pas.h"
#include "sounds.h"
#include <algorithm.h>

Command commands[11] = {{"eskl_animStart\r\n", animStart},
                       {"eskl_frontCTog\r\n", toggleFrontCold},
                       {"eskl_frontWTog\r\n", toggleFrontWarm},
                       {"eskl_rearLETog\r\n", toggleRearLED},
                       {"eskl_handleTog\r\n", toggleThrottle},
                       {"eskl_sportmTog\r\n", toggleSportMode},
                       {"eskl_soundsTog\r\n", toggleSound},
                       {"eskl_bulbsTogg\r\n", toggleBulbs},
                       {"eskl_requestSt\r\n", sendStatus},
                       {"eskl_algCmpInc\r\n", algorithmComponentIncrement},
                       {"eskl_algCmpDec\r\n", algorithmComponentDecrement}};

VariableCommand variableCommands[1] = {{"eskl_bri__", setFrontColdBrightness}};

char* statusMessage;

// bike controls/statuses/features/idk
bool frontColdEnabled = false;
uint16_t frontColdBrightness = 75;  // [0,999]
bool frontWarmEnabled = false;
bool rearEnabled = false;
bool throttleEnabled = false;
bool sportModeDisabled = true;
bool soundEnabled = true;
bool bulbsEnabled = false;

void initStatusMessage() {
  statusMessage = (char*)malloc(21 * sizeof(char));
}

void animStart() {
  playAnim(ANIM_STARTUP_PHASE1);
  playToggleSound(1);
}

uint16_t frontColdBrightnessToDutyCycle() {
  return frontColdBrightness * (1000 / DUTY_PWM_MAX_CCR1);  // cnt is 10000
}

void toggleFrontCold() {
  frontColdEnabled = !frontColdEnabled;
  TIM1->CCR4 = frontColdEnabled ? frontColdBrightnessToDutyCycle() : 0;
  playToggleSound(frontColdEnabled);
  sendStatus();
}

// this is used for animations
void toggleFrontColdNoSound() {
  frontColdEnabled = !frontColdEnabled;
  TIM1->CCR4 = frontColdEnabled ? frontColdBrightnessToDutyCycle() : 0;
}

void enableFrontColdNoSound() {
  frontColdEnabled = 1;
  TIM1->CCR4 = frontColdBrightnessToDutyCycle();
}

void disableFrontColdNoSound() {
  frontColdEnabled = 0;
  TIM1->CCR4 = 0;
}

void setFrontColdBrightness(uint16_t brightness) {
  frontColdBrightness = brightness;
  if (!frontColdEnabled)
    return;
  TIM1->CCR4 = frontColdBrightnessToDutyCycle();
}

void toggleFrontWarm() {
  togglePin(FRONT_WARM_GPIO_Port, FRONT_WARM_Pin);
  playToggleSound(frontWarmEnabled);
  sendStatus();
}

void toggleRearLED() {
  togglePin(REAR_LED_GPIO_Port, REAR_LED_Pin);
  playToggleSound(rearEnabled);
  sendStatus();
}

void toggleThrottle() {
  togglePin(THR_DIS_GPIO_Port, THR_DIS_Pin);
  togglePWM(TIM_THROTTLE_LEDS, !throttleEnabled);
  playToggleSound(throttleEnabled);
  sendStatus();
}

void toggleSportMode() {
  togglePin(THR_SPORT_GPIO_Port, THR_SPORT_Pin);
  playToggleSound(!sportModeDisabled);  // reverse logic here!
  sendStatus();
}

void toggleBulbs() {
  togglePin(BULBS_GPIO_Port, BULBS_Pin);
  playToggleSound(bulbsEnabled);
  sendStatus();
}

void toggleSound() {
  soundEnabled = !soundEnabled;
  playToggleSound(soundEnabled);
  togglePWM(TIM_SOUND, soundEnabled);
  sendStatus();
}

void algorithmComponentIncrement() {
  algorithm_eq_component = algorithm_eq_component >= ALGORITHM_EQ_COMPONENT_MAX ? ALGORITHM_EQ_COMPONENT_MAX : algorithm_eq_component + 0.1f;
  algorithm_eq_component >= ALGORITHM_EQ_COMPONENT_MAX ? playErrorSound() : playToggleSound(1);
  sendStatus();
}

void algorithmComponentDecrement() {
  algorithm_eq_component = algorithm_eq_component <= ALGORITHM_EQ_COMPONENT_MIN ? ALGORITHM_EQ_COMPONENT_MIN : algorithm_eq_component - 0.1f;
  algorithm_eq_component <= ALGORITHM_EQ_COMPONENT_MIN ? playErrorSound() : playToggleSound(0);
  sendStatus();
}

/* status format: eskl_stABCDEFGHHHIII
 * A   =>  frontColdEnabled(bool 0:1)
 * B   =>  frontWarmEnabled(bool 0:1)
 * C   =>  rearEnabled(bool 0:1)
 * E   =>  throttleEnabled(bool 0:1)
 * E   =>  sportModeDisabled(bool 0:1)
 * F   =>  soundEnabled(bool 0:1)
 * G   =>  bulbsEnabled(bool 0:1)
 * HHH =>  frontColdBrightness(uint8_t 000:999)
 * III =>  algorithm_eq_component(uint8_t 000:999) -> float (-9.9: 9.9)
 *                    * 1st digit - sign (0 positive, 1 negative);
 *                    * 2nd digit - whole part
 *                    * 3rd digit - fractional part
 */
void sendStatus() {
  char frontColdBrightnessHundreds = (frontColdBrightness / 100) + '0';
  char frontColdBrightnessTens = ((frontColdBrightness / 10) % 10) + '0';
  char frontColdBrightnessUnits = (frontColdBrightness % 10) + '0';

  char algorithm_eq_componentSign = algorithm_eq_component > 0 ? '0' : '1';
  char algorithm_eq_componentWhole = (int)(algorithm_eq_component < 0 ? -algorithm_eq_component : algorithm_eq_component) + '0';
  char algorithm_eq_componentFrac = (int)((algorithm_eq_component < 0 ? -algorithm_eq_component : algorithm_eq_component) * 10) % 10 + '0';

  sprintf(statusMessage, "eskl_st%c%c%c%c%c%c%c%c%c%c%c%c%c\r\n",
          frontColdEnabled ? '1' : '0', frontWarmEnabled ? '1' : '0',
          rearEnabled ? '1' : '0', throttleEnabled ? '1' : '0',
          sportModeDisabled ? '1' : '0', soundEnabled ? '1' : '0',
          bulbsEnabled ? '1' : '0', 
          frontColdBrightnessHundreds, frontColdBrightnessTens, frontColdBrightnessUnits,
          algorithm_eq_componentSign, algorithm_eq_componentWhole, algorithm_eq_componentFrac);

  send_string(statusMessage);
}
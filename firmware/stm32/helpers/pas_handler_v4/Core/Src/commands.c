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
#include "ambientlight.h"
#include "commands.h"
#include "gpio.h"
#include "helpers.h"
#include "logger.h"
#include "main.h"
#include "pas.h"
#include "sounds.h"
#include "blinkers.h"
#include "algorithm.h"
#include "adc.h"

Command commands[14] = {{"eskl_animStart\r\n", animStart},
                       {"eskl_frontCTog\r\n", toggleFrontCold},
                       {"eskl_frontWTog\r\n", toggleFrontWarm},
                       {"eskl_rearLETog\r\n", toggleRearLED},
                       {"eskl_handleTog\r\n", toggleThrottle},
                       {"eskl_soundsTog\r\n", toggleSound},
                       {"eskl_bulbsTogg\r\n", toggleBulbs},
                       {"eskl_requestSt\r\n", sendStatus},
                       {"eskl_blinkLeft\r\n", blinkBlinkerLeft},
                       {"eskl_blinkRigh\r\n", blinkBlinkerRight},
                       {"eskl_blinkBoth\r\n", blinkBlinkerBoth},
                       {"eskl_algCmpInc\r\n", algorithmComponentIncrement},
                       {"eskl_algCmpDec\r\n", algorithmComponentDecrement},
                       {"eskl_ambienTog\r\n", toggleAmbientLight}};

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
bool ambientLightEnabled = true;

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

void enableRearLEDNoSound() {
  writePin(REAR_LED_GPIO_Port, REAR_LED_Pin, 1);
  sendStatus();
}

void disableRearLEDNoSound() {
  writePin(REAR_LED_GPIO_Port, REAR_LED_Pin, 0);
  sendStatus();
}

void toggleThrottle() {
  // TODO
  throttleEnabled = !throttleEnabled;
  playToggleSound(throttleEnabled);
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

void blinkBlinkerLeft() {
  toggleLeftBlinker();
  sendStatus();
}

void blinkBlinkerRight() {
  toggleRightBlinker();
  sendStatus();
}

void blinkBlinkerBoth() {
  toggleBothBlinkers();
  sendStatus();
}

void toggleAmbientLight() {
  ambientLightEnabled = !ambientLightEnabled;
  if(ambientLightEnabled) initAmbientLight();
  playToggleSound(ambientLightEnabled);
  sendStatus();
}

void algorithmComponentIncrement() {
  algorithm_eq_component = algorithm_eq_component >= ALGORITHM_EQ_COMPONENT_MAX ? ALGORITHM_EQ_COMPONENT_MAX : algorithm_eq_component + 0.1f;
  algorithm_eq_component >= ALGORITHM_EQ_COMPONENT_MAX ? playTone(SOUND_ERR) : playToggleSound(1);
  sendStatus();
}

void algorithmComponentDecrement() {
  algorithm_eq_component = algorithm_eq_component <= ALGORITHM_EQ_COMPONENT_MIN ? ALGORITHM_EQ_COMPONENT_MIN : algorithm_eq_component - 0.1f;
  algorithm_eq_component <= ALGORITHM_EQ_COMPONENT_MIN ? playTone(SOUND_ERR) : playToggleSound(0);
  sendStatus();
}

/* status format: eskl_stABCDEFGHHHIIIJJJKLM
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
 * JJJ =>  batteryVoltage(uint8_t 0:30) -> float 
 * K   =>  blinkerLeftPinState(bool 0:1)
 * L   =>  blinkerRightPinState(bool 0:1)
 * M   =>  ambientLightEnabled(bool 0:1)
 */
void sendStatus() {
  char frontColdBrightnessHundreds = (frontColdBrightness / 100) + '0';
  char frontColdBrightnessTens = ((frontColdBrightness / 10) % 10) + '0';
  char frontColdBrightnessUnits = (frontColdBrightness % 10) + '0';

  char algorithm_eq_componentSign = algorithm_eq_component > 0 ? '0' : '1';
  char algorithm_eq_componentWhole = (int)(algorithm_eq_component < 0 ? -algorithm_eq_component : algorithm_eq_component) + '0';
  char algorithm_eq_componentFrac = (int)((algorithm_eq_component < 0 ? -algorithm_eq_component : algorithm_eq_component) * 10) % 10 + '0';

  char batteryVoltageHundreds = (batteryVoltage / 100) + '0';
  char batteryVoltageTens = ((batteryVoltage / 10) % 10) + '0';
  char batteryVoltageUnits = (batteryVoltage % 10) + '0';

  char batteryCurrentThousands = (batteryCurrent / 1000) + '0';
  char batteryCurrentHundreds = ((batteryCurrent / 100) % 10) + '0';
  char batteryCurrentTens = ((batteryCurrent / 10) % 10) + '0';
  char batteryCurrentUnits = (batteryCurrent % 10) + '0';

  sprintf(statusMessage, "eskl_st%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\r\n",
          frontColdEnabled ? '1' : '0', frontWarmEnabled ? '1' : '0',
          rearEnabled ? '1' : '0', throttleEnabled ? '1' : '0',
          sportModeDisabled ? '1' : '0', soundEnabled ? '1' : '0',
          bulbsEnabled ? '1' : '0', 

          frontColdBrightnessHundreds, frontColdBrightnessTens, frontColdBrightnessUnits,
          algorithm_eq_componentSign, algorithm_eq_componentWhole, algorithm_eq_componentFrac,
          batteryVoltageHundreds, batteryVoltageTens, batteryVoltageUnits,

          blinkerLeftPinState ? '0' : '1', blinkerRightPinState ? '0' : '1',
          '0', ambientLightEnabled ? '1' : '0',

          batteryCurrentThousands, batteryCurrentHundreds, batteryCurrentTens, batteryCurrentUnits);
      // TODO remove leftover algorithm sound
  send_string(statusMessage);
}
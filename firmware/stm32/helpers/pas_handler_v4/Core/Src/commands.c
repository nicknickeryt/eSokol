/*
 * commands.h
 *
 * Contains bike feature commands
 *
 *  Created on: Oct 27, 2024
 *      Author: nick
 */

#include "commands.h"

#include <stdlib.h>

#include "adc.h"
#include "algorithm.h"
#include "ambientlight.h"
#include "animations.h"
#include "blinkers.h"
#include "gpio.h"
#include "helpers.h"
#include "logger.h"
#include "main.h"
#include "pas.h"
#include "odometer.h"
#include "sounds.h"

Command commands[14] = {{"eskl_animStart\r\n", anim_start},
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

bool frontColdEnabled = false;
uint16_t frontColdBrightness = 75;  // [0,999]
bool frontWarmEnabled = false;
bool rearEnabled = false;
bool throttleEnabled = false;
bool sportModeDisabled = true;
bool soundEnabled = true;
bool bulbsEnabled = false;
bool ambientLightEnabled = true;

void initStatusMessage() { statusMessage = (char*)malloc(41 * sizeof(char)); }

void anim_start() {
    animation_play(ANIM_STARTUP_PHASE1);
    sound_playToggle(1);
}

uint16_t frontColdBrightnessToDutyCycle() {
    return frontColdBrightness * (1000 / DUTY_PWM_MAX_CCR1);  // cnt is 10000
}

void toggleFrontCold() {
    frontColdEnabled = !frontColdEnabled;
    TIM1->CCR4 = frontColdEnabled ? frontColdBrightnessToDutyCycle() : 0;
    sound_playToggle(frontColdEnabled);
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
    if (!frontColdEnabled) return;
    TIM1->CCR4 = frontColdBrightnessToDutyCycle();
}

void toggleFrontWarm() {
    togglePin(FRONT_WARM_GPIO_Port, FRONT_WARM_Pin);
    sound_playToggle(frontWarmEnabled);
    sendStatus();
}

void toggleRearLED() {
    togglePin(REAR_LED_GPIO_Port, REAR_LED_Pin);
    sound_playToggle(rearEnabled);
    sendStatus();
}

void enableRearLEDNoSound() {
    gpio_write(REAR_LED_GPIO_Port, REAR_LED_Pin, 1);
    sendStatus();
}

void disableRearLEDNoSound() {
    gpio_write(REAR_LED_GPIO_Port, REAR_LED_Pin, 0);
    sendStatus();
}

void toggleThrottle() {
    throttleEnabled = !throttleEnabled;
    sound_playToggle(throttleEnabled);
    sendStatus();
}

void toggleBulbs() {
    togglePin(BULBS_GPIO_Port, BULBS_Pin);
    sound_playToggle(bulbsEnabled);
    sendStatus();
}

void toggleSound() {
    soundEnabled = !soundEnabled;
    sound_playToggle(soundEnabled);
    pwm_toggle(TIM_SOUND, soundEnabled);
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
    if (ambientLightEnabled) initAmbientLight();
    sound_playToggle(ambientLightEnabled);
    sendStatus();
}

void algorithmComponentIncrement() {
    algorithm_eq_component =
        algorithm_eq_component >= ALGORITHM_EQ_COMPONENT_MAX
            ? ALGORITHM_EQ_COMPONENT_MAX
            : algorithm_eq_component + 0.1f;
    algorithm_eq_component >= ALGORITHM_EQ_COMPONENT_MAX ? sound_play(SOUND_ERR)
                                                         : sound_playToggle(1);
    sendStatus();
}

void algorithmComponentDecrement() {
    algorithm_eq_component =
        algorithm_eq_component <= ALGORITHM_EQ_COMPONENT_MIN
            ? ALGORITHM_EQ_COMPONENT_MIN
            : algorithm_eq_component - 0.1f;
    algorithm_eq_component <= ALGORITHM_EQ_COMPONENT_MIN ? sound_play(SOUND_ERR)
                                                         : sound_playToggle(0);
    sendStatus();
}

/* status format: eskl_stABCDEFGHHHIIIJJJKLMNNNN
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
 * NNNN=>  batteryCurrent(0:9999) (miliVolts!)
 */
void sendStatus() {
    char frontColdBrightnessHundreds = (frontColdBrightness / 100) + '0';
    char frontColdBrightnessTens = ((frontColdBrightness / 10) % 10) + '0';
    char frontColdBrightnessUnits = (frontColdBrightness % 10) + '0';

    char algorithm_eq_componentSign = algorithm_eq_component > 0 ? '0' : '1';
    char algorithm_eq_componentWhole =
        (int)(algorithm_eq_component < 0 ? -algorithm_eq_component
                                         : algorithm_eq_component) +
        '0';
    char algorithm_eq_componentFrac =
        (int)((algorithm_eq_component < 0 ? -algorithm_eq_component
                                          : algorithm_eq_component) *
              10) %
            10 +
        '0';

    char batteryVoltageHundreds = (adc_batteryVoltage / 100) + '0';
    char batteryVoltageTens = ((adc_batteryVoltage / 10) % 10) + '0';
    char batteryVoltageUnits = (adc_batteryVoltage % 10) + '0';

    char batteryCurrentThousands = (adc_batteryCurrent / 1000) + '0';
    char batteryCurrentHundreds = ((adc_batteryCurrent / 100) % 10) + '0';
    char batteryCurrentTens = ((adc_batteryCurrent / 10) % 10) + '0';
    char batteryCurrentUnits = (adc_batteryCurrent % 10) + '0';

    // Odometer
    uint32_t distanceMeters = (uint32_t)(odometer_getDistanceMeters());
    if (distanceMeters > 9999999) distanceMeters = 9999999;
    
    uint32_t distanceKilometers = distanceMeters / 1000;
    uint32_t distanceFraction = (distanceMeters % 1000);  // metry jako część po przecinku
    
    char distanceThousands = (distanceKilometers / 1000) + '0';
    char distanceHundreds = ((distanceKilometers / 100) % 10) + '0';
    char distanceTens = ((distanceKilometers / 10) % 10) + '0';
    char distanceUnits = (distanceKilometers % 10) + '0';
    
    char distanceFracHundreds = (distanceFraction / 100) + '0';
    char distanceFracTens = ((distanceFraction / 10) % 10) + '0';
    char distanceFracUnits = (distanceFraction % 10) + '0';
    

    sprintf(statusMessage,
        "eskl_st%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\r\n",
        frontColdEnabled ? '1' : '0', frontWarmEnabled ? '1' : '0',
        rearEnabled ? '1' : '0', throttleEnabled ? '1' : '0',
        sportModeDisabled ? '1' : '0', soundEnabled ? '1' : '0',
        bulbsEnabled ? '1' : '0',

        frontColdBrightnessHundreds, frontColdBrightnessTens,
        frontColdBrightnessUnits, algorithm_eq_componentSign,
        algorithm_eq_componentWhole, algorithm_eq_componentFrac,
        batteryVoltageHundreds, batteryVoltageTens, batteryVoltageUnits,

        blinkerLeftPinState ? '0' : '1', blinkerRightPinState ? '0' : '1',
        '0', ambientLightEnabled ? '1' : '0',

        batteryCurrentThousands, batteryCurrentHundreds, batteryCurrentTens,
        batteryCurrentUnits,

        distanceThousands, distanceHundreds, distanceTens, distanceUnits,
        distanceFracHundreds, distanceFracTens, distanceFracUnits);

            
    logger_sendChar(statusMessage);
}
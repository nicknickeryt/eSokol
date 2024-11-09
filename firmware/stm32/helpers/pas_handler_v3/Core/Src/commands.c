/*
 * commands.h
 *
 * Contains bike feature commands
 *
 *  Created on: Oct 27, 2024
 *      Author: nick
 */

#include <stdlib.h>

#include "main.h"
#include "commands.h"
#include "animations.h"
#include "helpers.h"
#include "sounds.h"
#include "gpio.h"
#include "logger.h"
#include "pas.h"

Command commands[9] = {
    {"eskl_animStart\r\n", animStart},
    {"eskl_frontCTog\r\n", toggleFrontC},
    {"eskl_frontWTog\r\n", toggleFrontW},
    {"eskl_rearLETog\r\n", toggleRearLED},
    {"eskl_handleTog\r\n", toggleThrottle},
    {"eskl_sportmTog\r\n", toggleSportMode},
    {"eskl_soundsTog\r\n", toggleSound},
    {"eskl_bulbsTogg\r\n", toggleBulbs},
    {"eskl_requestSt\r\n", sendStatus}
};
    
VariableCommand variableCommands[] = {
    {"eskl_bri__", setFrontColdBrightness}
};

char *statusMessage;

// bike controls/statuses/features/idk
bool frontColdEnabled = false;
uint16_t frontColdBrightness = 75;    // [0,999] 
bool frontWarmEnabled = false;
bool rearEnabled = false;
bool throttleEnabled = false;
bool sportModeDisabled = true;
bool soundEnabled = true;
bool bulbsEnabled = false;

void initStatusMessage() {
    statusMessage = (char *)malloc(21 * sizeof(char));
}

void animStart()
{
    playAnim(ANIM_STARTUP_PHASE1);
    playToggleSound(1);
}

uint16_t frontColdBrightnessToDutyCycle() {
    return frontColdBrightness * (1000/DUTY_PWM_MAX_CCR1);        // cnt is 10000
}

void toggleFrontC()
{
    frontColdEnabled = !frontColdEnabled;
    TIM1->CCR4 = frontColdEnabled ? frontColdBrightnessToDutyCycle() : 0;
    playToggleSound(frontColdEnabled);
}

void toggleFrontCNoSound()
{
    frontColdEnabled = !frontColdEnabled;
    TIM1->CCR4 = frontColdEnabled ? frontColdBrightnessToDutyCycle() : 0;
}

void enableFrontCNoSound()
{
    frontColdEnabled = 1;
    TIM1->CCR4 = frontColdBrightnessToDutyCycle();
}

void disableFrontCNoSound()
{
    frontColdEnabled = 0;
    TIM1->CCR4 = 0;
}

void setFrontColdBrightness(uint16_t brightness) {
    frontColdBrightness = brightness;
    if(!frontColdEnabled) return;
	TIM1->CCR4 = frontColdBrightnessToDutyCycle();
}

void toggleFrontW()
{
    togglePin(FRONT_WARM_GPIO_Port, FRONT_WARM_Pin);
    playToggleSound(frontWarmEnabled);
}

void toggleRearLED()
{
    togglePin(REAR_LED_GPIO_Port, REAR_LED_Pin);
    playToggleSound(rearEnabled);
}

void toggleThrottle()
{
    togglePin(THR_DIS_GPIO_Port, THR_DIS_Pin);
    togglePWM(TIM_THROTTLE_LEDS, !throttleEnabled);
    playToggleSound(throttleEnabled);
}

void toggleSportMode()
{
    togglePin(THR_SPORT_GPIO_Port, THR_SPORT_Pin);
    playToggleSound(sportModeDisabled);
}

void toggleBulbs()
{
    togglePin(BULBS_GPIO_Port, BULBS_Pin);
    playToggleSound(bulbsEnabled);
}

void toggleSound()
{
    soundEnabled = !soundEnabled;
    playToggleSound(soundEnabled);
    togglePWM(TIM_SOUND, soundEnabled);
}

/* status format: eskl_stABCDEFGHHH
 * A  =>  frontColdEnabled(bool 0:1)
 * B  =>  frontWarmEnabled(bool 0:1)
 * C  =>  rearEnabled(bool 0:1)
 * E  =>  throttleEnabled(bool 0:1)
 * E  =>  sportModeDisabled(bool 0:1)
 * F  =>  soundEnabled(bool 0:1)
 * G  =>  bulbsEnabled(bool 0:1)
 * HH =>  frontColdBrightness(uint8_t 0:999)
 */
void sendStatus()
{
    char frontColdBrightnessHundreds = (frontColdBrightness / 100) + '0';
    char frontColdBrightnessTens = ((frontColdBrightness / 10) % 10) + '0';
    char frontColdBrightnessUnits = (frontColdBrightness % 10) + '0';
    
    sprintf(statusMessage, "eskl_st%c%c%c%c%c%c%c%c%c%c\r\n",
            frontColdEnabled ? '1' : '0',
            frontWarmEnabled ? '1' : '0',
            rearEnabled ? '1' : '0',
            throttleEnabled ? '1' : '0',
            sportModeDisabled ? '1' : '0',
            soundEnabled ? '1' : '0',
            bulbsEnabled ? '1' : '0',
            frontColdBrightnessHundreds,
            frontColdBrightnessTens,
            frontColdBrightnessUnits);

    send_string(statusMessage);
}
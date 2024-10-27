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

Command commands[8] = {
    {"eskl_animStart\r\n", animStart},
    {"eskl_frontCTog\r\n", toggleFrontC},
    {"eskl_frontWTog\r\n", toggleFrontW},
    {"eskl_rearLETog\r\n", toggleRearLED},
    {"eskl_handleTog\r\n", toggleThrottle},
    {"eskl_sportmTog\r\n", toggleSportMode},
    {"eskl_soundsTog\r\n", toggleSound},
    {"eskl_requestSt\r\n", sendStatus}};

// bike controls/statuses/features/idk
bool frontColdEnabled = false;
bool frontWarmEnabled = false;
bool rearEnabled = false;
bool throttleEnabled = false;
bool sportModeDisabled = true;
bool soundEnabled = true;

void animStart()
{
    playAnim(ANIM_STARTUP_PHASE1);
    playToggleSound(1);
}
void toggleFrontC()
{
    togglePin(FRONT_COLD_GPIO_Port, FRONT_COLD_Pin);
    playToggleSound(frontColdEnabled);
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
    togglePWM(PWM_THROTTLE, !throttleEnabled);
    playToggleSound(throttleEnabled);
}

void toggleSportMode()
{
    togglePin(THR_SPORT_GPIO_Port, THR_SPORT_Pin);
    playToggleSound(sportModeDisabled);
}

void toggleSound()
{
    soundEnabled = !soundEnabled;
    playToggleSound(soundEnabled);
    togglePWM(PWM_SOUND, soundEnabled);
}

void sendStatus()
{
    char *statusMessage = (char *)malloc(18 * sizeof(char));

    sprintf(statusMessage, "eskl_sta%c%c%c%c%c%c\r\n",
            frontColdEnabled ? '1' : '0',
            frontWarmEnabled ? '1' : '0',
            rearEnabled ? '1' : '0',
            throttleEnabled ? '1' : '0',
            sportModeDisabled ? '1' : '0',
            soundEnabled ? '1' : '0');

    send_string(statusMessage);
}
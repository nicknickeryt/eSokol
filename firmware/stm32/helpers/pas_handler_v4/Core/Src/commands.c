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
#include "motor.h"
#include "odometer.h"
#include "sounds.h"
#include "speedometer.h"

Command commands[23] = {{"eskl_animStart\r\n", anim_start},
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
                        {"eskl_ambienTog\r\n", toggleAmbientLight},
                        {"eskl_algToggle\r\n", algorithmToggleEnabled},
                        {"eskl_algPKpInc\r\n", algorithmKpIncrease},
                        {"eskl_algPKpDec\r\n", algorithmKpDecrease},
                        {"eskl_algPKiInc\r\n", algorithmKiIncrease},
                        {"eskl_algPKiDec\r\n", algorithmKiDecrease},
                        {"eskl_algPKdInc\r\n", algorithmKdIncrease},
                        {"eskl_algPKdDec\r\n", algorithmKdDecrease}};

VariableCommand variableCommands[1] = {{"eskl_bri__", setFrontColdBrightness}};

char* statusMessage;

bool frontColdEnabled = false;
uint16_t frontColdBrightness = 75;  // [0,999]
bool frontWarmEnabled = false;
bool rearEnabled = false;
bool throttleEnabled = false;
bool pasEnabled = true;
bool sportModeDisabled = true;
bool soundEnabled = true;
bool bulbsEnabled = false;
bool ambientLightEnabled = true;

void initStatusMessage() { statusMessage = (char*)malloc(65 * sizeof(char)); }

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
    pasEnabled = !throttleEnabled;
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
    float algorithm_targetSpeedPercentage =
        algorithm_getTargetSpeedPercentage();

    algorithm_targetSpeedPercentage =
        algorithm_targetSpeedPercentage >= ALGORITHM_TARGET_SPEED_PERCENTAGE_MAX
            ? ALGORITHM_TARGET_SPEED_PERCENTAGE_MAX
            : algorithm_targetSpeedPercentage + 0.01f;
    algorithm_targetSpeedPercentage >= ALGORITHM_TARGET_SPEED_PERCENTAGE_MAX
        ? sound_play(SOUND_ERR)
        : sound_playToggle(1);

    algorithm_setTargetSpeedPercentage(algorithm_targetSpeedPercentage);

    sendStatus();
}

void algorithmComponentDecrement() {
    float algorithm_targetSpeedPercentage =
        algorithm_getTargetSpeedPercentage();

    algorithm_targetSpeedPercentage =
        algorithm_targetSpeedPercentage <= ALGORITHM_TARGET_SPEED_PERCENTAGE_MIN
            ? ALGORITHM_TARGET_SPEED_PERCENTAGE_MIN
            : algorithm_targetSpeedPercentage - 0.01f;

    algorithm_targetSpeedPercentage <= ALGORITHM_TARGET_SPEED_PERCENTAGE_MIN
        ? sound_play(SOUND_ERR)
        : sound_playToggle(0);

    algorithm_setTargetSpeedPercentage(algorithm_targetSpeedPercentage);

    sendStatus();
}

void algorithmToggleEnabled() {
    algorithm_toggleEnabled();
    sound_playToggle(algorithm_isEnabled());

    sendStatus();
}

// {0.5f, 0.05f, 0.5f, 0, 0, 100};

// KP
void algorithmKpIncrease() {
    motorWheelSpeedPID.Kp += 0.01f;
    sound_playToggle(1);

    sendStatus();
}

void algorithmKpDecrease() {
    motorWheelSpeedPID.Kp -= 0.01f;
    sound_playToggle(0);

    sendStatus();
}

// KI
void algorithmKiIncrease() {
    motorWheelSpeedPID.Ki += 0.001f;
    sound_playToggle(1);

    sendStatus();
}

void algorithmKiDecrease() {
    motorWheelSpeedPID.Ki -= 0.001f;
    sound_playToggle(0);

    sendStatus();
}

// KD
void algorithmKdIncrease() {
    motorWheelSpeedPID.Kd += 0.01f;
    sound_playToggle(1);

    sendStatus();
}

void algorithmKdDecrease() {
    motorWheelSpeedPID.Kd -= 0.01f;
    sound_playToggle(0);

    sendStatus();
}

void sendStatus() {
    char frontColdBrightnessHundreds = (frontColdBrightness / 100) + '0';
    char frontColdBrightnessTens = ((frontColdBrightness / 10) % 10) + '0';
    char frontColdBrightnessUnits = (frontColdBrightness % 10) + '0';

    uint16_t algoritm_targetSpeedPercentage =
        algorithm_getTargetSpeedPercentage() * 100;

    char targetSpeedPercentageHundreds =
        (algoritm_targetSpeedPercentage / 100) + '0';
    char targetSpeedPercentageTens =
        ((algoritm_targetSpeedPercentage / 10) % 10) + '0';
    char targetSpeedPercentageUnits =
        (algoritm_targetSpeedPercentage % 10) + '0';

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
    uint32_t distanceFraction =
        (distanceMeters % 1000);  // metry jako część po przecinku

    char distanceThousands = (distanceKilometers / 1000) + '0';
    char distanceHundreds = ((distanceKilometers / 100) % 10) + '0';
    char distanceTens = ((distanceKilometers / 10) % 10) + '0';
    char distanceUnits = (distanceKilometers % 10) + '0';

    char distanceFracHundreds = (distanceFraction / 100) + '0';
    char distanceFracTens = ((distanceFraction / 10) % 10) + '0';
    char distanceFracUnits = (distanceFraction % 10) + '0';

    float motorWheelVelocity = speedometer_getMotorWheelVelocityKmh();
    int motorWheelVelocityInt = (int)motorWheelVelocity;
    int motorWheelVelocityFrac = (int)((motorWheelVelocity - motorWheelVelocityInt) * 100.0f + 0.5f);
    
    char motorWheelVelocityTens = '0' + (motorWheelVelocityInt / 10);
    char motorWheelVelocityUnits = '0' + (motorWheelVelocityInt % 10);
    char motorWheelVelocityFracTens = '0' + (motorWheelVelocityFrac / 10);
    char motorWheelVelocityFracUnits = '0' + (motorWheelVelocityFrac % 10);

    float wheelVelocity = speedometer_getWheelVelocityKmh();
    int wheelVelocityInt = (int)wheelVelocity;
    int wheelVelocityFrac = (int)((wheelVelocity - wheelVelocityInt) * 100.0f + 0.5f);
    
    char wheelVelocityTens = '0' + (wheelVelocityInt / 10);
    char wheelVelocityUnits = '0' + (wheelVelocityInt % 10);
    char wheelVelocityFracTens = '0' + (wheelVelocityFrac / 10);
    char wheelVelocityFracUnits = '0' + (wheelVelocityFrac % 10);

    int kpInt = (int)(motorWheelSpeedPID.Kp * 1000.0f + 0.5f);
    int kiInt = (int)(motorWheelSpeedPID.Ki * 1000.0f + 0.5f);
    int kdInt = (int)(motorWheelSpeedPID.Kd * 1000.0f + 0.5f);

    // Kp
    char kpThousands = (kpInt / 10000) + '0';
    char kpThousandsRest = ((kpInt / 1000) % 10) + '0';
    char kpHundreds = ((kpInt / 100) % 10) + '0';
    char kpTens = ((kpInt / 10) % 10) + '0';
    char kpUnits = (kpInt % 10) + '0';

    // Ki
    char kiThousands = (kiInt / 10000) + '0';
    char kiThousandsRest = ((kiInt / 1000) % 10) + '0';
    char kiHundreds = ((kiInt / 100) % 10) + '0';
    char kiTens = ((kiInt / 10) % 10) + '0';
    char kiUnits = (kiInt % 10) + '0';

    // Kd
    char kdThousands = (kdInt / 10000) + '0';
    char kdThousandsRest = ((kdInt / 1000) % 10) + '0';
    char kdHundreds = ((kdInt / 100) % 10) + '0';
    char kdTens = ((kdInt / 10) % 10) + '0';
    char kdUnits = (kdInt % 10) + '0';
    
    sprintf(statusMessage, "eskl_st%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\r\n",

        frontColdEnabled ? '1' : '0', frontWarmEnabled ? '1' : '0',
        rearEnabled ? '1' : '0', throttleEnabled ? '1' : '0',
        sportModeDisabled ? '1' : '0', soundEnabled ? '1' : '0',
        bulbsEnabled ? '1' : '0',

        frontColdBrightnessHundreds, frontColdBrightnessTens,
        frontColdBrightnessUnits,

        targetSpeedPercentageHundreds, targetSpeedPercentageTens,
        targetSpeedPercentageUnits,

        batteryVoltageHundreds, batteryVoltageTens, batteryVoltageUnits,

        blinkerLeftPinState ? '0' : '1', blinkerRightPinState ? '0' : '1',
        '0', ambientLightEnabled ? '1' : '0',

        batteryCurrentThousands, batteryCurrentHundreds, batteryCurrentTens,
        batteryCurrentUnits,

        distanceThousands, distanceHundreds, distanceTens, distanceUnits,
        distanceFracHundreds, distanceFracTens, distanceFracUnits,

        motorWheelVelocityTens, motorWheelVelocityUnits, motorWheelVelocityFracTens, motorWheelVelocityFracUnits,

        algorithm_isEnabled() ? '1' : '0',
    
        wheelVelocityTens, wheelVelocityUnits, wheelVelocityFracTens, wheelVelocityFracUnits,
    
        kpThousands, kpThousandsRest, kpHundreds, kpTens, kpUnits,
        kiThousands, kiThousandsRest, kiHundreds, kiTens, kiUnits,
        kdThousands, kdThousandsRest, kdHundreds, kdTens, kdUnits);

    logger_sendChar(statusMessage);
}
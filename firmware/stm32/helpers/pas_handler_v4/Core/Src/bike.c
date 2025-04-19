#include "bike.h"

#include "adc.h"
#include "algorithm.h"
#include "ambientlight.h"
#include "animations.h"
#include "blinkers.h"
#include "commands.h"
#include "gpio.h"
#include "main.h"
#include "odometer.h"
#include "motor.h"
#include "sounds.h"
#include "speedometer.h"
#include "uart.h"

#include "logger.h"

bool bike_bluetoothConnected = false;

void bike_handleGpioInterrupt(uint16_t GPIO_Pin) {
    switch (GPIO_Pin) {
        case BT_STATE_Pin:
            if (sound_isPlaying())
                return;
            else if (gpio_read(BT_STATE_GPIO_Port, BT_STATE_Pin)) {
                bike_bluetoothConnected = true;
                sound_play(SOUND_CONNECTED);
                animation_play(ANIM_CONNECTED);
            } else {
                bike_bluetoothConnected = false;
                sound_play(SOUND_DISCONNECTED);
                animation_play(ANIM_DISCONNECTED);
            }
            break;
        case PAS_SIGNAL_Pin:
            algorithm_handlePasPulse();
            break;
        case HALL_SPEED_Pin:
            if (gpio_read(HALL_SPEED_GPIO_Port, HALL_SPEED_Pin)) {
                speedometer_setVelocity(
                    speedometer_calculateVelocity(HAL_GetTick()));
                odometer_pulseInterrupt();
            }
            break;
        case BLINKER_LEFT_IN_Pin:
            gpio_read(BLINKER_LEFT_IN_GPIO_Port, BLINKER_LEFT_IN_Pin)
                ? blinkers_reset()
                : blinkers_enableLeft();
            break;
        case BLINKER_RIGHT_IN_Pin:
            gpio_read(BLINKER_RIGHT_IN_GPIO_Port, BLINKER_RIGHT_IN_Pin)
                ? blinkers_reset()
                : blinkers_enableRight();
            break;
    }
}

void bike_init() {
    HAL_UARTEx_ReceiveToIdle_IT(&huart1, rxBuffer, 16);

    TIM1->CCR1 = 0;  // OUT_PWM
    TIM1->CCR4 = 500;              // FRONT_COLD_PWM

    HAL_TIM_PWM_Start(TIM_THROTTLE_LEDS, TIM_CHANNEL_1);  // OUT_PWM
    HAL_TIM_PWM_Start(TIM_THROTTLE_LEDS, TIM_CHANNEL_4);  // FRONT_COLD_PWM
    HAL_TIM_PWM_Start(TIM_SOUND, TIM_CHANNEL_1);          // SOUND

    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_rawValues, 4);

    animation_play(ANIM_STARTUP_PHASE1);

    if (gpio_read(BT_STATE_GPIO_Port, BT_STATE_Pin))
        bike_bluetoothConnected = true;

    blinkers_init();
    initStatusMessage();
    speedometer_init();
    odometer_init();

    sound_play(SOUND_RUDOLF);

    TIM1->PSC = 168 - 1;

    algorithm_init();
}

void bike_proc() {
    status_proc();
    sound_proc();
    anim_proc();
    anim_blinkProc();
    blinkers_proc();
    speedometer_proc();
    odometer_proc();
    adc_proc();
    blinkers_proc();
    ambientlight_proc();
    algorithm_proc();
}

void sendMeasurement(uint8_t dutyCycle, float velocity) {
    char message[64];

    int velocityWhole = (int)velocity;
    int velocityFrac = (int)((velocity - velocityWhole) * 10);  // jedna cyfra po przecinku

    sprintf(message, "eskl_measure: DUTY %d VKMH %d.%d\r\n", dutyCycle, velocityWhole, velocityFrac);
    logger_sendChar(message);
}

void bike_measurementMode() {
    uint32_t startTick;

    for (uint8_t i = 0; i <= 100; i++) {
        motor_setDutyCycle(i);
        motor_updateDutyCycle();

        startTick = HAL_GetTick();
        while (HAL_GetTick() - startTick < 5000) {
            speedometer_proc();
        }

        sendMeasurement(i, speedometer_getVelocity());
    }
}
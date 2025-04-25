#include "pid.h"

void PID_SetTunings(PID* pid, float Kp, float Ki, float Kd) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
}

float constrain(float x, float minVal, float maxVal) {
    if (x > maxVal) return maxVal;
    if (x < minVal) return minVal;
    return x;
}

float PID_Calculate(PID* pid, float target, float current) {
    float error = target - current;

    pid->integral += pid->Ki * (error + pid->last_error) / 2.0f;
    float maxIntegral = pid->output_limit;
    pid->integral = constrain(pid->integral, -maxIntegral, maxIntegral);

    float derivative = (current - pid->last_error) * pid->Kd;

    pid->last_error = error;

    float output = pid->Kp * error + pid->integral - derivative;

    output = constrain(output, -pid->output_limit, pid->output_limit);

    return output;
}

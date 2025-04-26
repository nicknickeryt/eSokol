#ifndef __PID_H
#define __PID_H


typedef struct {
    float Kp, Ki, Kd;
    float integral;
    float last_error;
    float output_limit;
} PID;

void PID_SetTunings(PID* pid, float Kp, float Ki, float Kd);
float PID_Calculate(PID* pid, float target, float current);
void PID_Reset(PID* pid);

float constrain(float x, float minVal, float maxVal);

#endif /* __PID_H */
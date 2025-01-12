/*
 * pid.h
 *
 *  Created on: Jan 12, 2025
 *      Author: mohnc
 */

#ifndef INC_PID_H_
#define INC_PID_H_

typedef struct {
    float Kp;        // Proportional gain
    float Ki;        // Integral gain
    float Kd;        // Derivative gain
    float prevError; // Previous error value
    float integral;  // Integral accumulator
} PID_t;

// Initialize the PID controller
void PID_Init(PID_t *pid, float Kp, float Ki, float Kd) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->prevError = 0;
    pid->integral = 0;
}

// Compute the PID output
float PID_Compute(PID_t *pid, float setpoint, float measured) {
    float error = setpoint - measured;    // Calculate the error
    pid->integral += error;              // Accumulate the integral term
    float derivative = error - pid->prevError; // Calculate the derivative term
    pid->prevError = error;              // Store the current error for the next cycle

    // Compute the PID output
    float output = (pid->Kp * error) + (pid->Ki * pid->integral) + (pid->Kd * derivative);
    return output;
}


#endif /* INC_PID_H_ */

/*
 * Filename: PID.c
 * FileType: C Source file
 * Author: Thomas Ponter-Amor
 * Last modified: 12/8/2023
 * Description: PID implementation
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "PID.h"

void vClearAccumulator(PID_t* pid)
{
    // Reset the accumulator
    pid->accumulator = 0;
}

void vInitPID(PID_t* pid, float kp, float ki, float kd, float limit)
{
    // Initialise the PID struct
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->accumulator = 0;
    pid->error = 0;
    pid->derivative = 0;
    pid->limit = limit;
}

void vUpdatePID(PID_t* pid, float errorLocal, float dt)
{
    // Update the PID parameters
    pid->accumulator += errorLocal * dt;

    if (pid->accumulator >  pid->limit) {
        pid->accumulator =  pid->limit;
    }
    if (pid->accumulator < -pid->limit) {
        pid->accumulator = -pid->limit;
    }

    pid->derivative = (errorLocal - pid->error) / dt;

    pid->error = errorLocal;
}

float fGetPIDu(PID_t* pid)
{
    // Calculate control effort
    float up = (pid->kp * pid->error);
    float ui = (pid->ki * pid->accumulator);
    float ud = (pid->kd * pid->derivative);
    float fControlEffort = up + ui + ud;

    // Limit control effort
    if (fControlEffort < 2.0) {
        fControlEffort = 0.0;
    } else if (fControlEffort > 98.0) {
        fControlEffort = 98.0;
    }

    return fControlEffort;
}

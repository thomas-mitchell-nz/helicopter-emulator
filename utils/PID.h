/*
 * Filename: PID.h
 * FileType: C Header file
 * Author: Thomas Ponter-Amor
 * Last modified: 12/8/2023
 * Description: PID implementation
 */

#ifndef PID_H_
#define PID_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    float kp;
    float ki;
    float kd;
    float accumulator;
    float error;
    float derivative;
    float limit;
} PID_t;

// Reset the accumulator
void vClearAccumulator(PID_t* pid);

// Initialise the PID struct
void vInitPID(PID_t*, float, float, float, float);

// Update the PID parameters
void vUpdatePID(PID_t*, float, float);

// Calculate control effort
float fGetPIDu(PID_t*);

#endif /* PID_H_ */

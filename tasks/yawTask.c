/*
 * Filename: yawTask.c
 * FileType: C Source file
 * Author: Thomas Mitchell
 * Last modified: 22/8/2023
 * Description: Handles yaw control
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "priorities.h"
#include "queue.h"
#include "semphr.h"

#include "encoderHandler.h"
#include "UARTHandler.h"
#include "PWMHandler.h"
#include "PID.h"
#include "yawTask.h"

#define TASK_STACK_SIZE         256
#define DELAY_YAW_TASK          25

extern xSemaphoreHandle g_pUARTMutex;
extern xQueueHandle qDisplayQueue;

int32_t i32YawCommand = 0;
PID_t yawPID;

void vSetYawCommand (int32_t i32NewYawCommand)
{
    // Set the desired yaw position
    i32YawCommand = i32NewYawCommand;
}

static void vYawTask (void *pvParameters)
{
    float dt = DELAY_YAW_TASK / 1000.0;
    uint32_t ui32ControlEffort;
    int32_t ui32YawMeasured;
    int32_t i32Error;

    int16_t i16Message[3];

    // Get the current tick count
    portTickType ui32WakeTime = xTaskGetTickCount();

    while (1)
    {
        // Update PID control
        ui32YawMeasured = ui32GetYaw();
        i32Error = i32YawCommand - ui32YawMeasured;

        i16Message[0] = 1;
        i16Message[1] = ui32YawMeasured;
        i16Message[2] = i32YawCommand;

        if (xQueueSend(qDisplayQueue, &i16Message, portMAX_DELAY) != pdPASS) {
            while(1);
        }

        // Rotate in direction of least i32Error
        if (i32Error > -360 && i32Error < -180) {
            // Update the control
            vUpdatePID(&yawPID, 360+i32Error, dt);
            ui32ControlEffort = (uint32_t) fGetPIDu(&yawPID);
            vSetTailPWM(ui32ControlEffort);

        } else if (i32Error > 0 && i32Error < 180) {
            // Update the control
            vUpdatePID(&yawPID, i32Error, dt);
            ui32ControlEffort = (uint32_t) fGetPIDu(&yawPID);
            vSetTailPWM(ui32ControlEffort);

        } else {
            vSetTailPWM(0);
        }

        i16Message[0] = 3;
        i16Message[1] = ui32ControlEffort;

        if (xQueueSend(qDisplayQueue, &i16Message, portMAX_DELAY) != pdPASS) {
            while(1);
        }

        // Guard UART from concurrent access. Print the yaw value
        xSemaphoreTake(g_pUARTMutex, portMAX_DELAY);
        UARTprintf("Command: %d, measured: %d, i32Error: %d, effort: %d \n",
                   i32YawCommand, ui32YawMeasured, i32Error, ui32ControlEffort);
        xSemaphoreGive(g_pUARTMutex);

        // Wait for the required amount of time to check back
        vTaskDelayUntil(&ui32WakeTime, DELAY_YAW_TASK / portTICK_RATE_MS);
    }
}

uint32_t ui32YawInit (void)
{
    vInitYawConfig();
    vInitYawRef();

    // PID parameters
    float kp = 0.8;
    float ki = 0.55;
    float kd = 0.1;
    float accumLimit = 100.0;
    vInitPID(&yawPID, kp, ki, kd, accumLimit);

    // Create the yaw task
    if(xTaskCreate(vYawTask, "Yaw Task", TASK_STACK_SIZE,
                   NULL, PRIORITY_YAW_TASK, NULL) != pdTRUE)
    {
        return(1); // Failure
    }

    // Success
    return(0);
}

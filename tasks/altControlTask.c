/*
 * Filename: altControlTask.c
 * FileType: C Source file
 * Author: Thomas Mitchell
 * Last modified: 22/8/2023
 * Description: Handles altitude control
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

#include "ADCHandler.h"
#include "UARTHandler.h"
#include "PWMHandler.h"
#include "setPoints.h"
#include "PID.h"

#include "altControlTask.h"
#include "altDetectionTask.h"

#define TASK_STACK_SIZE     512
#define CONTROLLER_DELAY    ALTITUDE_DELAY*BUFFER_SIZE

#define DT_NUMERATOR        100
#define TARGET_SCALOR       134 // theoretical 124 - experimental 144

#define ZERO_REPETITIONS 25

extern xQueueHandle qAltitudeRaw;
extern xQueueHandle qDisplayQueue;

extern xSemaphoreHandle xAltQueueFull;
extern xSemaphoreHandle g_pUARTMutex;

static uint32_t ui32AltTarget;
static uint32_t ui32AltZero;

uint32_t *ui32SetPoints;
PID_t altPID;

void vSetAltTarget(uint32_t upordown)
{
    static uint8_t index = 0;

    // Get the index for the set points
    // Reset the accumulator when setting a new set point
    if (upordown == 1) {
        if (index < 10) {
            vClearAccumulator(&altPID);
            index++;
        }
    } else {
        if (index == 1) {
            vClearAccumulator(&altPID);
        }
        if (index > 0) {
            index--;
        }
    }

    // Get the set points from lookup table
    ui32AltTarget = ui32SetPoints[index];

    // Guard UART from concurrent access. Print the set point
    xSemaphoreTake(g_pUARTMutex, portMAX_DELAY);
    UARTprintf("***\nSetpoint: %d\n", ui32AltTarget);
    xSemaphoreGive(g_pUARTMutex);
}

void vGetAltZero(void)
{
    uint32_t ui32ZeroInit = ui32GetRawADC();

    // Match with the altitude calibration value
    if (ui32ZeroInit > 3300) {
        ui32AltZero = 3370;
        ui32SetPoints = ui32_2v7_SetPoints;
    } else if (ui32ZeroInit < 3300 && ui32ZeroInit > 3000) {
        ui32AltZero = 3020;
        ui32SetPoints = ui32_2v4_SetPoints;
    } else if (ui32ZeroInit < 3000 && ui32ZeroInit > 2600) {
        ui32AltZero = 2650;
    } else if (ui32ZeroInit < 2600 && ui32ZeroInit > 2200) {
        ui32AltZero = 2270;
    } else if (ui32ZeroInit < 2200 && ui32ZeroInit > 1800) {
        ui32AltZero = 1840;
    } else if (ui32ZeroInit < 1800 && ui32ZeroInit > 1300) {
        ui32AltZero = 1350;
    } else if (ui32ZeroInit < 1300) {
        ui32AltZero = 980;
    }
}

static void vAltControlTask(void *pvParameters)
{
    // PID parameters
    float dtNumerator = DT_NUMERATOR;
    float dt = dtNumerator / 1000.0;
    uint32_t ui32ControlEffort;
    int32_t i32Error;

    // Queue variables
    int16_t i16Message[3];
    uint32_t ui32RawAltitude;
    uint32_t ui32AltitudeAverage;
    uint32_t ui32ScaledAverage;
    uint32_t ui32Sum = 0;
    uint32_t itr;

    // Get the altitude calibration value
    vGetAltZero();

    // Guard UART from concurrent access. Print the yaw value
    xSemaphoreTake(g_pUARTMutex, portMAX_DELAY);
    UARTprintf("ZERO ALTITUDE: %d\n", ui32AltZero);
    xSemaphoreGive(g_pUARTMutex);

    while(1)
    {
        if( xSemaphoreTake( xAltQueueFull, ( TickType_t ) 10 ) == pdTRUE )
        {
            // Get the average altitude value
            for (itr = 0; itr < ALTITUDE_QUEUE_SIZE; itr++) {
                // Read the next message, if available on queue.
                if(xQueueReceive(qAltitudeRaw, &ui32RawAltitude, 0) == pdPASS) {
                    ui32Sum = ui32Sum + ui32RawAltitude;
                } else {
                    break;
                }
            }

            // Calculate current altitude error
            ui32AltitudeAverage = ui32Sum / itr;
            ui32ScaledAverage  = ui32AltZero - ui32AltitudeAverage;
            i32Error = (ui32AltTarget - ui32ScaledAverage);

            i16Message[0] = 0;
            i16Message[1] = ui32ScaledAverage;
            i16Message[2] = ui32AltTarget;

            if (xQueueSend(qDisplayQueue, &i16Message, portMAX_DELAY) != pdPASS) {
                while(1);
            }

            // Update PID control
            vUpdatePID(&altPID, i32Error, dt);
            ui32ControlEffort = ((uint32_t) fGetPIDu(&altPID)) + 0.5;
            vSetMainPWM(ui32ControlEffort);

            i16Message[0] = 2;
            i16Message[1] = ui32ControlEffort;

            if (xQueueSend(qDisplayQueue, &i16Message, portMAX_DELAY) != pdPASS) {
                while(1);
            }

            // Guard UART from concurrent access. Print the average value
            xSemaphoreTake(g_pUARTMutex, portMAX_DELAY);
            UARTprintf("***\nSetpoint: %d Average: %d Error: %d Control Effort : %d\n",
                       ui32AltTarget, ui32ScaledAverage, i32Error, ui32ControlEffort);
            xSemaphoreGive(g_pUARTMutex);

            ui32Sum = 0;
        }
    }
}

uint32_t ui32AltControlTaskInit(void)
{
    // PID parameters
    float kp = 0.65;
    float ki = 0.2;
    float kd = 0.0;
    float accumLimit = 350.0;
    vInitPID(&altPID, kp, ki, kd, accumLimit);

    // Create the Controller Task
    if(xTaskCreate(vAltControlTask, "Controller", TASK_STACK_SIZE,
                   NULL, PRIORITY_CONTROLLER_TASK, NULL) != pdTRUE)
    {
        return(1); // Failure
    }

    // Success
    return(0);
}

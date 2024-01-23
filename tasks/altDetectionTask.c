/*
 * Filename: altDetectionTask.c
 * FileType: C Source file
 * Author: Thomas Ponter-Amor
 * Last modified: 11/8/2023
 * Description: Handles altitude detection
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
#include "altDetectionTask.h"

#define TASK_STACK_SIZE         256
#define ALTITUDE_ITEM_SIZE      sizeof(uint32_t)
#define ALTITUDE_TASK_DELAY     75

xQueueHandle qAltitudeRaw;
xSemaphoreHandle xAltQueueFull;

static void vAltDetectionTask(void *pvParameters)
{
    uint32_t ui32RawAltitude;
    portTickType ui16LastTime;

    // Get the current tick count
    ui16LastTime = xTaskGetTickCount();

    while(1)
    {
        ui32RawAltitude = ui32GetRawADC();

        // Pass the altitude value to the ControllerTask.
        if(xQueueSend(qAltitudeRaw, &ui32RawAltitude, portMAX_DELAY) != pdPASS)
        {
            // Error
            while(1) {}
        }

        // Give semaphore to unblock altitude control task
        if(uxQueueSpacesAvailable(qAltitudeRaw) <= 0)
        {
            xSemaphoreGive(xAltQueueFull);
        }

        // Wait for the required amount of time to check back
        vTaskDelayUntil(&ui16LastTime, ALTITUDE_DELAY / portTICK_RATE_MS);
    }
}

uint32_t ui32AltDetectionTaskInit(void)
{
    vInitADC();

    // Create a queue for sending messages to the altitude task
    qAltitudeRaw = xQueueCreate(ALTITUDE_QUEUE_SIZE, ALTITUDE_ITEM_SIZE);
    xAltQueueFull = xSemaphoreCreateBinary();

    // Create the altitude detection task
    if(xTaskCreate(vAltDetectionTask, "Get Altitude", TASK_STACK_SIZE,
                   NULL, PRIORITY_ALTITUDE_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    // Success
    return(0);
}

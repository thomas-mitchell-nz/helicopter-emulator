/*
 * Filename: displayTask.c
 * FileType: C Source file
 * Author: Charlie Price
 * Last modified: 22/8/2023
 * Description: Handles display task
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

#include "displayHandler.h"
#include "displayTask.h"

#define DISPLAY_TASK_STACK_SIZE 128
#define DISPLAY_UPDATE_DELAY 5
#define DISPLAY_QUEUE_SIZE 12
#define DISPLAY_ITEM_SIZE sizeof(int16_t) * 3

xQueueHandle qDisplayQueue;

void vDisplayTask(void *pvParameters)
{
    int16_t i16Message[3];

    // Get the current tick count
    portTickType ui32WakeTime = xTaskGetTickCount();

    // Start up display
    vDisplayAltitude(0,0);
    vDisplayYaw(0,0);
    vDisplayMainDuty(0);
    vDisplayTailDuty(0);

    while (1)
    {
        // Check queue for new values to write
        if(xQueueReceive(qDisplayQueue, &i16Message, 0) == pdPASS)
        {
            // Switch Case to only update the specific line that was changed
            switch (i16Message[0])
            {
            case ALTITUDE:
                vDisplayAltitude(i16Message[1], i16Message[2]);
                break;
            case YAW:
                vDisplayYaw(i16Message[1], i16Message[2]);
                break;
            case DUTY_MAIN:
                vDisplayMainDuty(i16Message[1]);
                break;
            case DUTY_TAIL:
                vDisplayTailDuty(i16Message[1]);
                break;
            default:
                break;
            }
        }

        // Wait for the required amount of time to check back
        vTaskDelayUntil(&ui32WakeTime, DISPLAY_UPDATE_DELAY / portTICK_RATE_MS);
    }
}

uint32_t ui32DisplayTaskInit(void)
{
    vInitDisplay();

    // Create queue for the display task
    qDisplayQueue = xQueueCreate(DISPLAY_QUEUE_SIZE, DISPLAY_ITEM_SIZE);

    // Create the display task
    if (xTaskCreate(vDisplayTask, "Display", DISPLAY_TASK_STACK_SIZE, NULL,
                    tskIDLE_PRIORITY + PRIORITY_DISPLAY_TASK, NULL) != pdTRUE)
    {
        return (1); // Failure
    }

    // Success
    return (0);
}

/*
 * Filename: buttonTask.c
 * FileType: C Source file
 * Author: Charlie Price
 * Last modified: 22/8/2023
 * Description: Handles button task
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

#include "buttonTask.h"
#include "buttons4.h"
#include "altControlTask.h"
#include "yawTask.h"

// Default button polling delay value
#define BUTTON_POLL_DELAY 10

#define YAW_DELTA 15

// Declare Queues
xQueueHandle qAltitudeSetPointQueue;
xQueueHandle qYawSetPointQueue;

uint32_t ui32AltitudeSetPoint;
int32_t i32YawSetPoint;

void vButtonPollTask(void *pvParameters)
{
    portTickType ui16LastTime;
    uint8_t ui8CurrentBut;
    uint8_t ui8ButState;

    ui32AltitudeSetPoint = 0;
    i32YawSetPoint = 0;

    // Get current tick count
    ui16LastTime = xTaskGetTickCount();

    while (1)
    {
        updateButtons();

        // Check state of each button
        for (ui8CurrentBut=0; ui8CurrentBut<NUM_BUTS; ui8CurrentBut++)
        {
            ui8ButState = checkButton(ui8CurrentBut);

            if (ui8ButState == RELEASED) { // Push to queue on falling edge

                // Modify altitude or yaw set points
                switch(ui8CurrentBut){
                    case UP:
                        vSetAltTarget(1);
                        break;
                    case DOWN:
                        vSetAltTarget(0);
                        break;
                    case LEFT:
                        // Modify the yaw set point (CW)
                        i32YawSetPoint -= YAW_DELTA;

                        // Limit yaw lower range
                        if (i32YawSetPoint <= -5) {
                            i32YawSetPoint = 345;
                        }
                        vSetYawCommand(i32YawSetPoint);
                        break;
                    case RIGHT:
                        // Modify the yaw set point (CCW)
                        i32YawSetPoint += YAW_DELTA;

                        // Limits yaw upper range
                        if (i32YawSetPoint == 360)
                        {
                            i32YawSetPoint = 0;
                        }
                        else if (i32YawSetPoint > 360) {
                            i32YawSetPoint = 15;
                        }

                        vSetYawCommand(i32YawSetPoint);
                        break;
                    default:
                        break;
                }
            }
        }

        // Wait for required amount of time
        vTaskDelayUntil(&ui16LastTime, BUTTON_POLL_DELAY / portTICK_RATE_MS);
    }
}

uint32_t ui32ButtonTaskInit(void)
{
    initButtons();

    // Initialise Queues
    qAltitudeSetPointQueue = xQueueCreate(12, sizeof(uint8_t));
    qYawSetPointQueue = xQueueCreate(12, sizeof(uint8_t));

    // Create Button task
    if (xTaskCreate(vButtonPollTask, "ButtonPoll", 128, NULL,
                    tskIDLE_PRIORITY + PRIORITY_BUTTON_TASK, NULL) != pdTRUE)
    {
        return(1); // Failure
    }

    // Success
    return(0);
}


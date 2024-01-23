/*
 * Filename: altDetectionTask.h
 * FileType: C Header file
 * Author: Thomas Ponter-Amor
 * Last modified: 11/8/2023
 * Description: Handles altitude detection
 */

#ifndef TASKS_ALTDETECTIONTASK_H_
#define TASKS_ALTDETECTIONTASK_H_

#include <stdint.h>

#include "FreeRTOS.h"
#include "semphr.h"

#define ALTITUDE_DELAY          10
#define ALTITUDE_QUEUE_SIZE     10

// Initialise altitude task
uint32_t ui32AltDetectionTaskInit(void);

#endif /* TASKS_ALTDETECTIONTASK_H_ */

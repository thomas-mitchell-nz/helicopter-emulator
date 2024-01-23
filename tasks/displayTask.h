/*
 * Filename: displayTask.h
 * FileType: C Header file
 * Author: Charlie Price
 * Last modified: 22/8/2023
 * Description: Handles display task
 */

#ifndef TASKS_DISPLAYTASK_H_
#define TASKS_DISPLAYTASK_H_

#include <stdint.h>

// Get values to display from queue and display them
void vDisplayTask(void *pvParameters);

/* Initialise display task */
uint32_t ui32DisplayTaskInit(void);

#endif /* TASKS_DISPLAYTASK_H_ */

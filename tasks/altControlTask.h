/*
 * Filename: altControlTask.h
 * FileType: C Header file
 * Author: Thomas Mitchell
 * Last modified: 11/8/2023
 * Description: Handles altitude control
 */

#ifndef TASKS_ALTCONTROLTASK_H_
#define TASKS_ALTCONTROLTASK_H_

#include <stdint.h>

// Set the altitude target value
void vSetAltTarget(uint32_t ui32NewTarget);

// Initialise altitude task
uint32_t ui32AltControlTaskInit(void);

#endif /* TASKS_ALTCONTROLTASK_H_ */

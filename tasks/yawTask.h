/*
 * Filename: yawTask.h
 * FileType: C Header file
 * Author: Thomas Mitchell
 * Last modified: 22/8/2023
 * Description: Handles yaw control
 */

#ifndef TASKS_YAWTASK_H_
#define TASKS_YAWTASK_H_

// Set the desired yaw position
void vSetYawCommand(int32_t i32NewYawCommand);

// Intialise the yaw task
uint32_t ui32YawInit(void);

#endif /* TASKS_YAWTASK_H_ */

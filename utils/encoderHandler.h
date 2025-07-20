/*
 * Filename: encoderHandler.h
 * FileType: C Header file
 * Author: Thomas Mitchell
 * Last modified: 22/8/2023
 * Description: Configures and handles encoder
 */

#ifndef ENCODERHANDLER_C_
#define ENCODERHANDLER_C_

#include <stdint.h>

// Intialise encoder pin configurations
void vInitYawConfig (void);

// Intialise reference pin configuration
void vInitYawRef (void);

// Get the yaw value in degrees
uint32_t ui32GetYaw (void);

#endif /* ENCODERHANDLER_C_ */

/*
 * Filename: PWMHandler.h
 * FileType: C Header file
 * Author: Charlie Price
 * Last modified: 22/8/2023
 * Description: PWM implementation
 */

#ifndef UTILS_PWMHANDLER_H_
#define UTILS_PWMHANDLER_H_

#include <stdint.h>

// Configure the main rotor PWM peripheral
void vInitMainPWM(void);

// Configure the tail rotor PWM peripheral
void vInitTailPWM(void);

// Set the main rotor PWM parameters
void vSetMainPWM(uint32_t ui32Duty);

// Set the tail rotor PWM parameters
void vSetTailPWM(uint32_t ui32Duty);

#endif /* UTILS_PWMHANDLER_H_ */

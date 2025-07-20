/*
 * Filename: ADCHandler.c
 * FileType: C Header file
 * Author: Thomas Mitchell
 * Last modified: 22/8/2023
 * Description: Configures and interfaces with ADC
 */

#ifndef ADCHANDLER_H_
#define ADCHANDLER_H_

#include <stdint.h>

// Initialize the ADC module
void vInitADC(void);

// Get the raw ADC value
uint32_t ui32GetRawADC(void);

#endif /* ADCHANDLER_H_ */

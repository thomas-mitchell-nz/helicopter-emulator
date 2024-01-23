/*
 * Filename: UARTHandler.h
 * FileType: C Header file
 * Author: Thomas Ponter-Amor
 * Last modified: 22/8/2023
 * Description: UART handler
 */

#ifndef UARTHANDLER_H_
#define UARTHANDLER_H_

#include "utils/uartstdio.h"
#include "FreeRTOS.h"
#include "semphr.h"

// Initialise UART peripheral
void vInitUART(void);

#endif /* UARTHANDLER_H_ */

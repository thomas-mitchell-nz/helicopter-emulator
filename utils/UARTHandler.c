/*
 * Filename: UARTHandler.c
 * FileType: C Source file
 * Author: Thomas Ponter-Amor
 * Last modified: 22/8/2023
 * Description: UART handler
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "inc/hw_memmap.h"

#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"

#include "utils/ustdlib.h"
#include "utils/uartstdio.h"

#include "UARTHandler.h"

// The mutex that protects concurrent access of UART from multiple tasks
xSemaphoreHandle g_pUARTMutex;

void vInitUART(void)
{
    // Enable the GPIO Peripheral used by the UART.
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Enable UART0
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Configure GPIO Pins for UART mode.
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 115200, 16000000);

    // Create a mutex to guard the UART
    g_pUARTMutex = xSemaphoreCreateMutex();

    UARTprintf("Initialised UART \n");
}

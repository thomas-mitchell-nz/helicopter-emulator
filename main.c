/*
 * Filename: main.c
 * FileType: C Source file
 * Last modified: 17/8/2023
 * Description: Application main entry point
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"

#include "displayTask.h"
#include "buttonTask.h"
#include "altDetectionTask.h"
#include "altControlTask.h"
#include "yawTask.h"

#include "PWMHandler.h"
#include "UARTHandler.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

void vInitTIVA(void)
{
    // Set the clock rate to 80 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Allow time for the oscillator to settle down
    SysCtlDelay(100);

    // Enable interrupts globally
    IntMasterEnable();
}

int main(void)
{
    vInitTIVA();
    vInitUART();
    vInitMainPWM();
    vInitTailPWM();

    if (ui32ButtonTaskInit() != 0) {
        while (1) {} // error creating task
    }

    if (ui32DisplayTaskInit() != 0) {
        while (1) {} // error creating task
    }

    if (ui32AltDetectionTaskInit() != 0) {
        while(1) {} // error creating task
    }

    if (ui32AltControlTaskInit() != 0) {
        while(1) {} // error creating task
    }

    if (ui32YawInit() != 0) {
        while(1) {} // error creating task
    }

    vTaskStartScheduler();
}


// Error handling function called when FreeRTOS asserts
void vAssertCalled( const char * pcFile, unsigned long ulLine ) {
    (void)pcFile; // unused
    (void)ulLine; // unused
    while (1);
}

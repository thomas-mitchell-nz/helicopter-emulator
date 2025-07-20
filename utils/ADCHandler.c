/*
 * Filename: ADCHandler.c
 * FileType: C Source file
 * Author: Thomas Mitchell
 * Last modified: 22/8/2023
 * Description: Configures and interfaces with ADC
 */

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"

#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

#include "ADCHandler.h"

uint32_t ui32RawADC;

void vADCIntHandler(void)
{
    // Get the data sequence from the ADC0
    ADCSequenceDataGet(ADC0_BASE, 3, &ui32RawADC);

    // Clear the interrupt
    ADCIntClear(ADC0_BASE, 3);
}

void vInitADC(void)
{
    // Enable the ADC0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Altitude analogue attached to PE4/AIN9
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    //GPIOPinTypeADC(GPIO_PORTE_BASE,GPIO_PIN_4);

    // Enable the ADC0 peripheral on sample sequence 3 with a processor signal trigger
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    // Configure the ADC0 peripheral on sample sequence 3
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE | ADC_CTL_END);

    // Enable ADC0 sequence 3
    ADCSequenceEnable(ADC0_BASE, 3);

    // Register interrupt handler
    ADCIntRegister (ADC0_BASE, 3, vADCIntHandler);

    // Enable interrupts for ADC0 sequence 3
    ADCIntEnable(ADC0_BASE, 3);
}

uint32_t ui32GetRawADC(void)
{
    // Trigger ADC
    ADCProcessorTrigger(ADC0_BASE, 3);
    return ui32RawADC;
}

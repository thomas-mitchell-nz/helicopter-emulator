/*
 * Filename: PWMHandler.c
 * FileType: C Source file
 * Author: Charlie Price
 * Last modified: 22/8/2023
 * Description: PWM implementation
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/pin_map.h" //Needed for pin configure
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"

#include "PWMHandler.h"

#define DUTY_CYCLE_MAX          100
#define DUTY_CYCLE_START        0

#define PWM_DIVIDER_CODE        SYSCTL_PWMDIV_4
#define PWM_DIVIDER             4
#define PWM_CONST_FREQ          200

#define PWM_MAIN_BASE           PWM0_BASE
#define PWM_MAIN_GEN            PWM_GEN_3
#define PWM_MAIN_OUT            PWM_OUT_7
#define PWM_MAIN_OUTBIT         PWM_OUT_7_BIT
#define PWM_MAIN_PERIPH         SYSCTL_PERIPH_PWM0
#define PWM_MAIN_PERIPH_GPIO    SYSCTL_PERIPH_GPIOC
#define PWM_MAIN_GPIO_PORT      GPIO_PORTC_BASE
#define PWM_MAIN_GPIO_CONFIG    GPIO_PC5_M0PWM7
#define PWM_MAIN_GPIO_PIN       GPIO_PIN_5

#define PWM_TAIL_BASE           PWM1_BASE
#define PWM_TAIL_GEN            PWM_GEN_2
#define PWM_TAIL_OUT            PWM_OUT_5
#define PWM_TAIL_OUTBIT         PWM_OUT_5_BIT
#define PWM_TAIL_PERIPH         SYSCTL_PERIPH_PWM1
#define PWM_TAIL_PERIPH_GPIO    SYSCTL_PERIPH_GPIOF
#define PWM_TAIL_GPIO_PORT      GPIO_PORTF_BASE
#define PWM_TAIL_GPIO_CONFIG    GPIO_PF1_M1PWM5
#define PWM_TAIL_GPIO_PIN       GPIO_PIN_1

void vInitMainPWM(void)
{
    // Enable the main PWM peripheral
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH);
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_GPIO);

    // Configure the PWM pins
    GPIOPinConfigure(PWM_MAIN_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_MAIN_GPIO_PORT, PWM_MAIN_GPIO_PIN);

    // Configure the PWM generator
    PWMGenConfigure(PWM_MAIN_BASE, PWM_MAIN_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    // Set initial duty
    vSetMainPWM(0);

    PWMGenEnable(PWM_MAIN_BASE, PWM_MAIN_GEN);

    // Configured as enabled on startup
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, true);
}

void vInitTailPWM(void)
{
    // Enable the tail PWM peripheral
    SysCtlPeripheralEnable(PWM_TAIL_PERIPH);
    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_GPIO);

    // Configure the PWM pins
    GPIOPinConfigure(PWM_TAIL_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_TAIL_GPIO_PORT, PWM_TAIL_GPIO_PIN);

    // Configure the PWM generator
    PWMGenConfigure(PWM_TAIL_BASE, PWM_TAIL_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    vSetTailPWM(0); // Set initial duty

    PWMGenEnable(PWM_TAIL_BASE, PWM_TAIL_GEN);

    // Configured as enabled on startup
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, true);
}

void vSetMainPWM(uint32_t ui32Duty)
{
    if (ui32Duty > 100) {
        // Invalid Input
        UARTprintf("Failed: Invalid Input\n");
        while(1);
    }

    uint32_t ui32Period = SysCtlClockGet() / PWM_DIVIDER / PWM_CONST_FREQ;

    // Set the PWM parameters
    PWMGenPeriodSet(PWM_MAIN_BASE, PWM_MAIN_GEN, ui32Period);
    PWMPulseWidthSet(PWM_MAIN_BASE, PWM_MAIN_OUT, ui32Period * ui32Duty / 100);
}

void vSetTailPWM(uint32_t ui32Duty)
{
    if (ui32Duty > 100) {
        // Invalid Input
        UARTprintf("Failed: Invalid Input\n");
        while(1);
    }
    uint32_t ui32Period = SysCtlClockGet() / PWM_DIVIDER / PWM_CONST_FREQ;

    // Set the PWM parameters
    PWMGenPeriodSet(PWM_TAIL_BASE, PWM_TAIL_GEN, ui32Period);
    PWMPulseWidthSet(PWM_TAIL_BASE, PWM_TAIL_OUT, ui32Period * ui32Duty / 100);
}

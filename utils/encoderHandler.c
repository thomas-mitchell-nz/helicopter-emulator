/*
 * Filename: encoderHandler.c
 * FileType: C Source file
 * Author: Thomas Mitchell
 * Last modified: 22/8/2023
 * Description: Configures and handles encoder
 */

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"

#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

#include "encoderHandler.h"

// Define constants for encoder states
#define STATE_00 0 // Both channels A and B are low (00 state)
#define STATE_01 1 // Channel A is low, Channel B is high (01 state)
#define STATE_10 2 // Channel A is high, Channel B is low (10 state)
#define STATE_11 3 // Both channels A and B are high (11 state)

// Encoder resolution
#define ENCODER_RES 448

volatile uint8_t ui8PrevEncoderState = STATE_00;
volatile uint32_t ui32YawDegrees = 0;
volatile int32_t i32YawRaw = 0;

void vYawDetectionISR (void)
{
    int16_t yawChange;
    int16_t phaseA;
    int16_t phaseB;

    // Clear the interrupt flag
    GPIOIntClear (GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Read encoder A and B channels
    phaseA = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0);
    phaseB = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_1);

    // Determine encoder state based on A and B channels
    int encoderState;
    if (!phaseA && !phaseB) {
        encoderState = STATE_00;
    } else if (!phaseA && phaseB) {
        encoderState = STATE_01;
    } else if (phaseA && !phaseB) {
        encoderState = STATE_10;
    } else {
        encoderState = STATE_11;
    }

    // Determine yaw change based on encoder state transitions
    if (ui8PrevEncoderState == STATE_00) {
        if (encoderState == STATE_01) {
            yawChange = 1;  // Clockwise rotation
        } else if (encoderState == STATE_10) {
            yawChange = -1; // Counterclockwise rotation
        }
    } else if (ui8PrevEncoderState == STATE_01) {
        if (encoderState == STATE_11) {
            yawChange = 1;  // Clockwise rotation
        } else if (encoderState == STATE_00) {
            yawChange = -1; // Counterclockwise rotation
        }
    } else if (ui8PrevEncoderState == STATE_11) {
        if (encoderState == STATE_10) {
            yawChange = 1;  // Clockwise rotation
        } else if (encoderState == STATE_01) {
            yawChange = -1; // Counterclockwise rotation
        }
    } else if (ui8PrevEncoderState == STATE_10) {
        if (encoderState == STATE_00) {
            yawChange = 1;  // Clockwise rotation
        } else if (encoderState == STATE_11) {
            yawChange = -1; // Counterclockwise rotation
        }
    }

    // Update parameters
    i32YawRaw += yawChange;
    ui8PrevEncoderState = encoderState;

    // Limit the range
    if (i32YawRaw < 0) {
        i32YawRaw = 447;
    }
    else if (i32YawRaw >= 448) {
        i32YawRaw = 0;
    }
}

void vYawRefISR (void)
{
    // Clear the interrupt flag
    GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_4);

    // set reference position
    ui32YawDegrees = 0;
    i32YawRaw = 0;
}

uint32_t ui32GetYaw (void) {
    // Calculate the yaw in degrees
    ui32YawDegrees = (i32YawRaw * 360) / ENCODER_RES;

    // Make sure ui32YawDegrees is in the range [0, 360)
    ui32YawDegrees = ui32YawDegrees % 360;

    return ui32YawDegrees;
}

void vInitYawConfig (void)
{
    // Enable the GPIO peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // Configure the Phase A and Phase B pins as inputs
    GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_DIR_MODE_IN);

    // Enable pull-up resistors on the Phase A and Phase B pins
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Configure interrupt for Phase A and Phase B pins (rising and falling edges)
    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_BOTH_EDGES);
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOIntRegister(GPIO_PORTB_BASE, vYawDetectionISR);

    // Enable the interrupt in the NVIC
    IntEnable(INT_GPIOB);
}

void vInitYawRef (void)
{
    // Enable the GPIO peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    // Configure pin as input pull-up
    GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_4);

    // Configure interrupt for pin
    GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);
    GPIOIntRegister(GPIO_PORTC_BASE, vYawRefISR);
    GPIOIntEnable(GPIO_PORTC_BASE, GPIO_PIN_4);

    // Enable the interrupt in the NVIC
    IntEnable(INT_GPIOC);
}


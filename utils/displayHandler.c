/*
 * Filename: displayHandler.c
 * FileType: C Source file
 * Author: Thomas Mitchell
 * Last modified: 22/8/2023
 * Description: Handles display functionality
 */

#include "displayHandler.h"
#include "OrbitOLEDInterface.h"
#include "utils/ustdlib.h"

void vInitDisplay(void)
{
    OLEDInitialise();
}

void vDisplayAltitude(int32_t alt_measured, int32_t alt_command)
{
    char cTextBuffer[17]; // Display fits 16 characters
    usnprintf(cTextBuffer, sizeof(cTextBuffer), "Alt: %4d [%4d]", alt_measured, alt_command);
    OLEDStringDraw(cTextBuffer, 0, 0);
}

void vDisplayYaw(int32_t yaw_measured, int32_t yaw_command)
{
    char cTextBuffer[17]; // Display fits 16 characters
    usnprintf(cTextBuffer, sizeof(cTextBuffer), "Yaw: %4d [%4d]", yaw_measured, yaw_command);
    OLEDStringDraw(cTextBuffer, 0, 1);
}

void vDisplayMainDuty(int32_t main_duty)
{
    char cTextBuffer[17]; // Display fits 16 characters
    usnprintf(cTextBuffer, sizeof(cTextBuffer), "Main duty: %4d", main_duty);
    OLEDStringDraw(cTextBuffer, 0, 2);
}

void vDisplayTailDuty(int32_t tail_duty)
{
    char cTextBuffer[17]; // Display fits 16 characters
    usnprintf(cTextBuffer, sizeof(cTextBuffer), "Tail duty: %4d", tail_duty);
    OLEDStringDraw(cTextBuffer, 0, 3);
}

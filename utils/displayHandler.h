/*
 * Filename: displayHandler.h
 * FileType: C Header file
 * Author: Thomas Mitchell
 * Last modified: 22/8/2023
 * Description: Handles display functionality
 */

#ifndef DISPLAYHANDLER_H_
#define DISPLAYHANDLER_H_

#include <stdint.h>

enum displayLabels {ALTITUDE=0, YAW, DUTY_MAIN, DUTY_TAIL};

// Initialise Orbit OLED display
void vInitDisplay(void);

// Display altitude data
void vDisplayAltitude(int32_t alt_measured, int32_t alt_command);

// Display yaw data
void vDisplayYaw(int32_t yaw_measured, int32_t yaw_command);

// Display main rotot duty cycle
void vDisplayMainDuty(int32_t main_duty);

// Display tail rotor duty cycle
void vDisplayTailDuty(int32_t tail_duty);

#endif /* DISPLAYHANDLER_H_ */

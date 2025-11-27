/*
Filename: controls.h
Author:   AllAcacia
*/

#ifndef CONTROLS_H
#define CONTROLS_H

#include <3ds.h>


typedef struct {
    u32 kUp;   // reads inputs released on last frame
    u32 kDown; // reads inputs pressed on this frame
    u32 kHeld; // reads inputs currently held
    circlePosition vcpad;     // Circle-Pad vector
    // circlePosition vcstick;   // C-Stick vector
    accelVector vaccl;        // Accelerometer vector
    angularRate vgyro;        // Gyroscope vector
    touchPosition vtpad;      // Touchpad vector
    touchPosition vtpad_prev; // Touchpad vector (from previous iteration)
} InputState;


extern InputState input;


void hidCaptureAllInputs(void);

#endif // CONTROLS_H
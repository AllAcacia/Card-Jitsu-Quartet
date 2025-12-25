/*
Filename: controls.c
Author:   AllAcacia
*/


#include "controls.h"


InputState input;


void hidCaptureAllInputs(void)
{
    hidScanInput(); // scan first

	input.kUp = hidKeysUp();
	input.kDown = hidKeysDown();
	input.kHeld = hidKeysHeld();
	hidTouchRead(&(input.vtpad));
	hidCircleRead(&(input.vcpad));
	hidAccelRead(&(input.vaccl));
	hidGyroRead(&(input.vgyro));
}
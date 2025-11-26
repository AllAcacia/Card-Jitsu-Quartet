/*
Filename: main.c
Author:   AllAcacia

Hello World example made by Aurelio Mannara for libctru
This code was modified for the last time on: 12/12/2014 21:00 UTC+1
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <3ds.h>
#include <citro2d.h>


void getBinaryRep(int num, int len, char* out)
{
	// Taken from https://www.geeksforgeeks.org/dsa/binary-representation-of-a-given-number/
    
	// Check for each bit.
    for (int i=(len-1); i>=0; i--) {
        
        // If i'th bit is set 
        if (num&(1<<i)) out[(len-1)-i] = '1';
		else out[(len-1)-i] = '0';
    }
	out[len] = '\0';
}


void print_control_data(touchPosition* tpad, circlePosition* cpad, accelVector* accl, angularRate* gyro, u32* kheld)
{
	//To move the cursor you have to print "\x1b[r;cH", where r and c are respectively
	//the row and column where you want your cursor to move
	//The top screen has 30 rows and 50 columns
	//The bottom screen has 30 rows and 40 columns

	// Clear screen
	consoleClear();

	// Control debug header
	printf("\x1b[1;0HControls Debug:");

	// Currently pressed buttons
	char kheld_str[33] = {'0'};
	getBinaryRep(*kheld, 32, kheld_str);
	printf("\x1b[3;0HKeys Held: %s", kheld_str);

	// Circle-pad
	printf("\x1b[5;0HCircle-Pad: %+04d, %+04d", cpad->dx, cpad->dy);

	// Accelerometer
	printf("\x1b[7;0HAccelerometer: %+06d, %+06d, %+06d", accl->x, accl->y, accl->z);

	// Gyroscope
	printf("\x1b[9;0HGyroscope: %+06d, %+06d, %+06d", gyro->x, gyro->y, gyro->z);

	// tpad
	printf("\x1b[11;0HTouch-Pad: %+04d, %+04d", tpad->px, tpad->py);

	// regular display
	printf("\x1b[14;0HA = Card-Jitsu");
	printf("\x1b[15;0HB = Card-Jitsu Fire");
	printf("\x1b[16;0HY = Card-Jitsu Water");
	printf("\x1b[17;0HX = Card-Jitsu Snow");
}


typedef enum {
	MENU=0,
	ORTHO,
	PYRO,
	HYDRO,
	CRYO
} CJQ_Gamestate;


static CJQ_Gamestate gamestate = MENU;


int main(int argc, char **argv)
{
	gfxInitDefault();
	hidInit();

	// Initialize console on top screen. Using NULL as the second argument tells the console library to use the internal console structure as current one
	consoleInit(GFX_TOP, NULL);
	
	// We don't need double buffering in this example. In this way we can draw our image only once on screen.
	// gfxSetDoubleBuffering(GFX_TOP, false);
	// gfxSetDoubleBuffering(GFX_BOTTOM, false);

	// Set up both screen buffers
	// u8* fb_up = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
	// u8* fb_lw = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);

	circlePosition* vcpad = malloc(sizeof(circlePosition));   // circle pad vector
	circlePosition* vcstick = malloc(sizeof(circlePosition)); // c-stick vector

	accelVector* vaccl = malloc(sizeof(accelVector));          // accelerometer vector
	angularRate* vgyro = malloc(sizeof(angularRate));          // gyroscope vector
	touchPosition* vtpad = malloc(sizeof(touchPosition));      // touchpad vector
	touchPosition* vtpad_prev = malloc(sizeof(touchPosition)); // touchpad vector (from previous iteration)

	// Main loop
	while (aptMainLoop())
	{
		// Scan all the inputs. This should be done once for each frame
		hidScanInput();

		u32 kUp = hidKeysUp();
		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();

		hidTouchRead(vtpad);
		hidCircleRead(vcpad);
		hidAccelRead(vaccl);
		hidGyroRead(vgyro);

		if (kDown & KEY_START) break; // break in order to return to hbmenu

		print_control_data(vtpad, vcpad, vaccl, vgyro, &kHeld);

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();

		// Wait for VBlank
		gspWaitForVBlank();

		*vtpad_prev = *vtpad;
	}

	free(vcpad);
	free(vcstick);
	free(vaccl);
	free(vgyro);
	free(vtpad);
	free(vtpad_prev);

	hidExit();
	gfxExit();
	return 0;
}
/*
Filename: main.c
Author:   AllAcacia

Hello World example made by Aurelio Mannara for libctru
This code was modified for the last time on: 12/12/2014 21:00 UTC+1
*/

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>
#include <3ds.h>
#include <citro2d.h>

#define REFRESH_RATE 20 // ms


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


u64 getTickDelay(u64 delay_ms)
{
	u64 delay_ticks = (delay_ms * SYSCLOCK_ARM11) / 1000;
	return delay_ticks;
}


bool checkDelayTimer(u64 tick_ref, u64 delay_ticks)
{
	u64 tick_curr = svcGetSystemTick();
	bool result;
	if (tick_curr >= tick_ref) {
		result = (tick_curr - tick_ref) >= delay_ticks;
	} else {
		result = (tick_curr + (ULLONG_MAX - tick_ref)) >= delay_ticks;
	}
	return result;
}


void print_control_data(u64 time_s, touchPosition* tpad, circlePosition* cpad, accelVector* accl, angularRate* gyro, u32* kheld)
{
	//To move the cursor you have to print "\x1b[r;cH", where r and c are respectively
	//the row and column where you want your cursor to move
	//The top screen has 30 rows and 50 columns
	//The bottom screen has 30 rows and 40 columns

	// Control debug header
	printf("\x1b[1;0HControls Debug:");

	// Currently pressed buttons
	char kheld_str[33] = {'0'};
	getBinaryRep(*kheld, 32, kheld_str);
	printf("\x1b[3;0HKeys Held: %s", kheld_str);

	// Circle-pad
	printf("\x1b[5;0HCircle-Pad: %+04d, %+04d", cpad->dx, cpad->dy);

	// Accelerometer
	printf("\x1b[7;0HAccelerometer: %+05d, %+05d, %+05d", accl->x, accl->y, accl->z);

	// Gyroscope
	printf("\x1b[9;0HGyroscope: %+05d, %+05d, %+05d", gyro->x, gyro->y, gyro->z);

	// tpad
	printf("\x1b[11;0HTouch-Pad: %+04d, %+04d", tpad->px, tpad->py);

	// ticks
	printf("\x1b[13;0HTick Count: %llu", svcGetSystemTick());

	// time counter
	printf("\x1b[15;0HTimer (sec): %llu", time_s);

	// regular display
	printf("\x1b[18;0HA = Card-Jitsu");
	printf("\x1b[19;0HB = Card-Jitsu Fire (not available)");
	printf("\x1b[20;0HY = Card-Jitsu Water (not available)");
	printf("\x1b[21;0HX = Card-Jitsu Snow (not available)");
}


typedef enum {
	MENU=0, // Menu
	PROTO,  // CJ Base
	PYRO,   // CJ Fire
	HYDRO,  // CJ Water
	CRYO    // CJ Snow
} CJQ_Gamestate;


static CJQ_Gamestate gamestate = MENU;


int main(int argc, char **argv)
{
	gfxInitDefault();
	hidInit();

	// Initialize console on top screen. Using NULL as the second argument tells the console library to use the internal console structure as current one
	consoleInit(GFX_TOP, NULL);
	consoleClear();
	
	u64 ticks_timer_ref = svcGetSystemTick();
	u64 ticks_refresh_ref = svcGetSystemTick();
	u64 time_s = 0;

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

		if (checkDelayTimer(ticks_timer_ref, getTickDelay(1000))) {
			time_s += 1;
			ticks_timer_ref = svcGetSystemTick();
		}

		if (kDown & KEY_START) { // exit
			break; // break in order to return to hbmenu
		} else if ((gamestate == MENU) && (kDown & KEY_A)) { // CJ base
			gamestate = PROTO; // change app type
			// int code = launch_cj_proto(); // launches the base cj game
		} else {
			if (checkDelayTimer(ticks_refresh_ref, getTickDelay(REFRESH_RATE))) {
				print_control_data(time_s, vtpad, vcpad, vaccl, vgyro, &kHeld);
				ticks_refresh_ref = svcGetSystemTick();
			}
		}

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
/*
Filename: game.c
Author:   AllAcacia
*/

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>
#include <3ds.h>
#include <citro2d.h>

#include "game.h"


static CJQ_Gamestate gamestate = MENU;
static bool gamestate_change = false; // to determine if a consoleClear() is required
static u64 tick_refresh_delay = 0;

static u32 kUp;   // reads inputs released on last frame
static u32 kDown; // reads inputs pressed on this frame
static u32 kHeld; // reads inputs currently held

static circlePosition vcpad;     // Circle-Pad vector
// static circlePosition vcstick;   // C-Stick vector
static accelVector vaccl;        // Accelerometer vector
static angularRate vgyro;        // Gyroscope vector
static touchPosition vtpad;      // Touchpad vector
static touchPosition vtpad_prev; // Touchpad vector (from previous iteration)

static u64 ticks_timer_ref;
static u64 ticks_refresh_ref;
static u64 time_s;


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
	printf("\x1b[18;0HSTART = Exit Software");
	printf("\x1b[19;0HSELECT = Main Menu");
	printf("\x1b[20;0HA = Card-Jitsu");
	printf("\x1b[21;0HB = Card-Jitsu Fire");
	printf("\x1b[22;0HY = Card-Jitsu Water");
	printf("\x1b[23;0HX = Card-Jitsu Snow");

	printf("\x1b[26;0HCard-Jitsu Quartet");
	printf("\x1b[27;0HMaster the three elements within Card-Jitsu!");
	printf("\x1b[29;0HDeveloped by AllAcacia");
	printf("\x1b[30;0Hhttps://github.com/AllAcacia/Card-Jitsu-Quartet/");
}


void print_menu(void)
{
	switch(gamestate) {
		case PROTO:
			printf("\x1b[1;0HPROTO menu");
			break;
		case PYRO:
			printf("\x1b[1;0HPYRO menu");
			break;
		case HYDRO:
			printf("\x1b[1;0HHYDRO menu");
			break;
		case CRYO:
			printf("\x1b[1;0HCRYO menu");
			break;
		default:
			printf("\x1b[1;0HMisc.");
			break;
	}

	printf("\x1b[3;0HSELECT = Main Menu");
	printf("\x1b[4;0HA = Card-Jitsu");
	printf("\x1b[5;0HB = Card-Jitsu Fire");
	printf("\x1b[6;0HY = Card-Jitsu Water");
	printf("\x1b[7;0HX = Card-Jitsu Snow");
}


void hidCaptureAllInputs(void)
{
	kUp = hidKeysUp();
	kDown = hidKeysDown();
	kHeld = hidKeysHeld();

	hidTouchRead(&vtpad);
	hidCircleRead(&vcpad);
	hidAccelRead(&vaccl);
	hidGyroRead(&vgyro);
}


void navigateTitles(void)
{
	// navigate between game titles
	if ((gamestate != MENU) && (kDown & MENU_SELECT)) {
		gamestate = MENU;
		gamestate_change = true;
	} else if ((gamestate != PROTO) && (kDown & PROTO_SELECT)) { // CJ base
		gamestate = PROTO;
		gamestate_change = true;
	} else if ((gamestate != PYRO) && (kDown & PYRO_SELECT)) { // CJ fire
		gamestate = PYRO;
		gamestate_change = true;
	} else if ((gamestate != HYDRO) && (kDown & HYDRO_SELECT)) { // CJ water
		gamestate = HYDRO;
		gamestate_change = true;
	} else if ((gamestate != CRYO) && (kDown & CRYO_SELECT)) { // CJ snow
		gamestate = CRYO;
		gamestate_change = true;
	}

	if (gamestate_change) {
		consoleClear();
		gamestate_change = false;
	}

	// execute title-wise functions
	if (gamestate == MENU) {
		if (checkDelayTimer(ticks_refresh_ref, tick_refresh_delay)) {
			print_control_data(time_s, &vtpad, &vcpad, &vaccl, &vgyro, &kHeld);
			ticks_refresh_ref = svcGetSystemTick();
		}
	} else if (gamestate == PROTO) {
		// int code = launch_cj_proto(); // launches the base cj game
		if (checkDelayTimer(ticks_refresh_ref, tick_refresh_delay)) {
			print_menu();
			ticks_refresh_ref = svcGetSystemTick();
		}
	} else if (gamestate == PYRO) {
		// int code = launch_cj_pyro(); // launches the cj fire game
		if (checkDelayTimer(ticks_refresh_ref, tick_refresh_delay)) {
			print_menu();
			ticks_refresh_ref = svcGetSystemTick();
		}
	} else if (gamestate == HYDRO) {
		// int code = launch_cj_hydro(); // launches the cj hydro game
		if (checkDelayTimer(ticks_refresh_ref, tick_refresh_delay)) {
			print_menu();
			ticks_refresh_ref = svcGetSystemTick();
		}
	} else if (gamestate == CRYO) {
		// int code = launch_cj_cryo(); // launches the cj cryo game
		if (checkDelayTimer(ticks_refresh_ref, tick_refresh_delay)) {
			print_menu();
			ticks_refresh_ref = svcGetSystemTick();
		}
	}
}


int main(int argc, char **argv)
{
	gfxInitDefault();
	hidInit();

	// Initialize console on top screen.
	// Using NULL as the second argument tells the console library to use the internal console structure as current one
	consoleInit(GFX_TOP, NULL);
	consoleClear();
	
	ticks_timer_ref = svcGetSystemTick();
	ticks_refresh_ref = svcGetSystemTick();
	time_s = 0;

	tick_refresh_delay = getTickDelay(REFRESH_RATE);

	// Main loop
	while (aptMainLoop())
	{
		// Scan all the inputs. This should be done once for each frame
		hidScanInput();
		hidCaptureAllInputs();

		// update timer
		if (checkDelayTimer(ticks_timer_ref, getTickDelay(1000))) {
			time_s += 1;
			ticks_timer_ref = svcGetSystemTick();
		}

		if ((kDown & KEY_START) && (gamestate == MENU)) { // exit
			break; // break in order to return to hbmenu
		} else {
			navigateTitles();
		}

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();

		// Wait for VBlank
		gspWaitForVBlank();

		vtpad_prev = vtpad;
	}

	hidExit();
	gfxExit();
	return 0;
}
/*
Filename: game.c
Author:   AllAcacia
*/

#include "game.h"


extern CJQ_Gamestate gamestate;
extern u64 tick_refresh_delay;
extern u64 ticks_timer_ref;
extern u64 ticks_refresh_ref;
extern u64 time_s;


int main(int argc, char **argv)
{
	// Init libs
	romfsInit();
	gfxInitDefault();
	hidInit();
	consoleInit(GFX_TOP, NULL);
	
	ticks_timer_ref = svcGetSystemTick();
	ticks_refresh_ref = svcGetSystemTick();
	time_s = 0;
	gamestate = MENU;

	tick_refresh_delay = getTickDelay(REFRESH_RATE);

	// Main loop
	while (aptMainLoop())
	{
		// Scan all the inputs. This should be done once for each frame
		hidCaptureAllInputs();

		if ((input.kDown & KEY_START) && (gamestate == MENU)) { // exit
			break; // break in order to return to hbmenu
		} else {
			navigateMenu();
		}

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();

		// Wait for VBlank
		gspWaitForVBlank();
	}

	hidExit();
	gfxExit();
	return 0;
}
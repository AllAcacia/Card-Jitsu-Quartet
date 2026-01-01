/*
Filename: game.c
Author:   AllAcacia
*/

#include "game.h"


CJQ_Gamestate gamestate;
C3D_RenderTarget* TOP_SCREEN;
C3D_RenderTarget* BOT_SCREEN;

u64 time_s = 0;

float flt_to_tpadx = BOTTOM_SCREEN_WIDTH/8.10569f;
float flt_to_tpady = BOTTOM_SCREEN_HEIGHT/6.07928f;

C2D_SpriteSheet menu_top_gfx_sheet;
C2D_Sprite menu_top_gfx;
C2D_SpriteSheet menu_bot_gfx_sheet;
C2D_Sprite menu_bot_gfx;

u64 tick_prev;
u64 tick_net;

u64 SIMUL_TICKS;
u64 REFRESH_TICKS;
const uint8_t MAX_SIMUL_STEPS = 3; // prevents overloading the simulation loop

bool DEBUG_MODE = false;


int main(int argc, char **argv)
{
	// Init libs
	romfsInit();
	gfxInitDefault();
	hidInit();
	// consoleInit(GFX_TOP, NULL);
	
	// Initialize C2D
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	
	// Initialize screens
	screensInit();
	time_s = 0;
	gamestate = MENU;
	
	// Set tick tracking states
	tick_prev = svcGetSystemTick();
	tick_net = 0;
	SIMUL_TICKS = getTickDelay_hz(SIMUL_RATE_HZ);
	REFRESH_TICKS = getTickDelay_hz(REFRESH_RATE_HZ);

	menu_top_gfx_sheet = C2D_SpriteSheetLoad("romfs:/gfx/ui_menu_top.t3x");
	C2D_SpriteFromSheet(&menu_top_gfx, menu_top_gfx_sheet, 0);
	C2D_SpriteSetCenter(&menu_top_gfx, 0.5f, 0.5f);
	C2D_SpriteSetPos(&menu_top_gfx, TOP_SCREEN_WIDTH/2, TOP_SCREEN_HEIGHT/2);
	menu_bot_gfx_sheet = C2D_SpriteSheetLoad("romfs:/gfx/ui_menu_bot.t3x");
	C2D_SpriteFromSheet(&menu_bot_gfx, menu_bot_gfx_sheet, 0);
	C2D_SpriteSetCenter(&menu_bot_gfx, 0.5f, 0.5f);
	C2D_SpriteSetPos(&menu_bot_gfx, BOTTOM_SCREEN_WIDTH/2, BOTTOM_SCREEN_HEIGHT/2);
	

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
	}

	C2D_Fini();
	C3D_Fini();
	hidExit();
	gfxExit();
	romfsExit();
	return 0;
}


void screensInit(void)
{
    // Create screens
    TOP_SCREEN = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	BOT_SCREEN = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
}


int launchCJQProto(void)
{
    gamestate = PROTO;

    // Load Card Spritesheets
    C2D_SpriteSheet cards_fire_sheet = C2D_SpriteSheetLoad("romfs:/gfx/cards_basic_f.t3x");
    C2D_Sprite cards_fire[FIRE_CARDS_NUM];
    loadSpritesFromSpritesheet(cards_fire, cards_fire_sheet, C2D_SpriteSheetCount(cards_fire_sheet), 0.5f, 0.5f, BOTTOM_SCREEN_WIDTH/2, BOTTOM_SCREEN_HEIGHT/2, 0.0f);
    
    C2D_SpriteSheet cards_water_sheet = C2D_SpriteSheetLoad("romfs:/gfx/cards_basic_w.t3x");
    C2D_Sprite cards_water[WATER_CARDS_NUM];
    loadSpritesFromSpritesheet(cards_water, cards_water_sheet, C2D_SpriteSheetCount(cards_water_sheet), 0.5f, 0.5f, BOTTOM_SCREEN_WIDTH/2, BOTTOM_SCREEN_HEIGHT/2, 0.0f);

    C2D_SpriteSheet cards_snow_sheet = C2D_SpriteSheetLoad("romfs:/gfx/cards_basic_s.t3x");
    C2D_Sprite cards_snow[SNOW_CARDS_NUM];
    loadSpritesFromSpritesheet(cards_snow, cards_snow_sheet, C2D_SpriteSheetCount(cards_snow_sheet), 0.5f, 0.5f, BOTTOM_SCREEN_WIDTH/2, BOTTOM_SCREEN_HEIGHT/2, 0.0f);

    C2D_Sprite* cards_all[FIRE_CARDS_NUM+WATER_CARDS_NUM+SNOW_CARDS_NUM];
    cards_all[0] = cards_fire;
    cards_all[1] = cards_water;
    cards_all[2] = cards_snow;

    uint8_t element_i = 0;
    uint8_t rank_i = 0;

    C2D_Sprite* card_curr = &(cards_all[element_i][rank_i]);

    SecondOrderDTS touchLocX_DTS;
    SecondOrderDTS touchLocY_DTS;
    float dyn_fn = 1.0f;
    float dyn_xi = 0.75f;
    float dyn_dt = 1.0f/((float)SIMUL_RATE_HZ);
    dynamicSS_init(&touchLocX_DTS, dyn_fn, dyn_xi, dyn_dt);
    dynamicSS_setstate(&touchLocX_DTS, BOTTOM_SCREEN_WIDTH/2, 0.0f);
    dynamicSS_init(&touchLocY_DTS, dyn_fn, dyn_xi, dyn_dt);
    dynamicSS_setstate(&touchLocY_DTS, BOTTOM_SCREEN_HEIGHT/2, 0.0f);

	tick_net = 0;

    while (gamestate == PROTO && aptMainLoop()) {
        // read inputs
        hidCaptureAllInputs();
        gameTimer();

        if (input.kDown & KEY_SELECT) {
            gamestate = MENU;
        }

		// Update tick tracking
		u64 tick_curr = svcGetSystemTick();
		u64 tick_frame;
		if (tick_curr >= tick_prev) {
			tick_frame = tick_curr - tick_prev;
		} else {
			tick_frame = tick_curr + (ULLONG_MAX - tick_prev);
		}
		tick_prev = tick_curr;
		tick_net += tick_frame;
        
        // Run game
		scrollCards(&element_i, &rank_i);
		card_curr = &(cards_all[element_i][rank_i]);
		
		uint8_t simul_steps = 0;
		while (tick_net >= SIMUL_TICKS && simul_steps < MAX_SIMUL_STEPS) { // For time-sensitive actions
			if(input.kHeld & KEY_TOUCH) { // If touch-screen pressed, iterate DTS (at a preset constant rate)
				dynamicSS_iterate(&touchLocX_DTS, ((float)input.vtpad.px)*flt_to_tpadx);
				dynamicSS_iterate(&touchLocY_DTS, ((float)input.vtpad.py)*flt_to_tpady);
			} else {
				dynamicSS_setstate(&touchLocX_DTS, card_curr->params.pos.x, 0.0f);
				dynamicSS_setstate(&touchLocY_DTS, card_curr->params.pos.y, 0.0f);
			}
			tick_net -= SIMUL_TICKS;
			simul_steps += 1;
		}
		C2D_SpriteSetPos(card_curr, mat2Dfloat_return(&touchLocX_DTS.x1, 0, 0), mat2Dfloat_return(&touchLocY_DTS.x1, 0, 0));
		
		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(BOT_SCREEN, C2D_WHITE);
		C2D_SceneBegin(BOT_SCREEN);
		C2D_DrawSprite(card_curr);
		C3D_FrameEnd(0);
    }

    C2D_SpriteSheetFree(cards_fire_sheet);
    C2D_SpriteSheetFree(cards_water_sheet);
    C2D_SpriteSheetFree(cards_snow_sheet);

    return EXIT_SUCCESS;
}


int launchCJQPyro(void)
{
    gamestate = PYRO;

	tick_net = 0;

    while (gamestate == PYRO && aptMainLoop()) {
        // read inputs
        hidCaptureAllInputs();
        gameTimer();

        // run game
        if (input.kDown & KEY_SELECT) {
            gamestate = MENU;
        }

        // Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(BOT_SCREEN, C2D_BLACK);
		C2D_SceneBegin(BOT_SCREEN);
		C3D_FrameEnd(0);
    }

    return EXIT_SUCCESS;
}


int launchCJQHydro(void)
{
    gamestate = HYDRO;

	tick_net = 0;

    while (gamestate == HYDRO && aptMainLoop()) {
        // read inputs
        hidCaptureAllInputs();
        gameTimer();

        // run game
        if (input.kDown & KEY_SELECT) {
            gamestate = MENU;
        }

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(BOT_SCREEN, C2D_BLACK);
		C2D_SceneBegin(BOT_SCREEN);
		C3D_FrameEnd(0);
    }
    return EXIT_SUCCESS;
}


int launchCJQCryo(void)
{
    gamestate = CRYO;

	tick_net = 0;

    while (gamestate == CRYO && aptMainLoop()) {
        // read inputs
        hidCaptureAllInputs();
        gameTimer();

        // run game
        if (input.kDown & KEY_SELECT) {
            gamestate = MENU;
        }

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(BOT_SCREEN, C2D_BLACK);
		C2D_SceneBegin(BOT_SCREEN);
		C3D_FrameEnd(0);
    }
    return EXIT_SUCCESS;
}


int launchStickBug(void)
{
	gamestate = EXTRA;

	uint8_t stickbug_sprite_len = 27;
	uint8_t index = 0;

	C2D_SpriteSheet stickbug1_sheet = C2D_SpriteSheetLoad("romfs:/gfx/stickbug1.t3x");
    C2D_Sprite stickbug1[12];
    loadSpritesFromSpritesheet(stickbug1, stickbug1_sheet, C2D_SpriteSheetCount(stickbug1_sheet), 0.5f, 0.5f, BOTTOM_SCREEN_WIDTH/2, BOTTOM_SCREEN_HEIGHT/2, 0.0f);

	C2D_SpriteSheet stickbug2_sheet = C2D_SpriteSheetLoad("romfs:/gfx/stickbug2.t3x");
    C2D_Sprite stickbug2[12];
    loadSpritesFromSpritesheet(stickbug2, stickbug2_sheet, C2D_SpriteSheetCount(stickbug2_sheet), 0.5f, 0.5f, BOTTOM_SCREEN_WIDTH/2, BOTTOM_SCREEN_HEIGHT/2, 0.0f);

	C2D_SpriteSheet stickbug3_sheet = C2D_SpriteSheetLoad("romfs:/gfx/stickbug3.t3x");
    C2D_Sprite stickbug3[3];
    loadSpritesFromSpritesheet(stickbug3, stickbug3_sheet, C2D_SpriteSheetCount(stickbug3_sheet), 0.5f, 0.5f, BOTTOM_SCREEN_WIDTH/2, BOTTOM_SCREEN_HEIGHT/2, 0.0f);

	C2D_Sprite stickbug_all[27];
	memcpy(&stickbug_all[0], stickbug1, 12*sizeof(C2D_Sprite));
	memcpy(&stickbug_all[12], stickbug2, 12*sizeof(C2D_Sprite));
	memcpy(&stickbug_all[24], stickbug3, 3*sizeof(C2D_Sprite));

	tick_prev = svcGetSystemTick(); // set reference for tick
	u64 stickbug_ticks = getTickDelay_hz(30);
	tick_net = 0;

	while (gamestate == EXTRA && aptMainLoop()) {
		// read inputs
        hidCaptureAllInputs();
        gameTimer();

		if ((input.kDown & KEY_SELECT)) { // exit
			gamestate = MENU;
		}

		// Update tick tracking
		u64 tick_curr = svcGetSystemTick();
		u64 tick_frame;
		if (tick_curr >= tick_prev) {
			tick_frame = tick_curr - tick_prev;
		} else {
			tick_frame = tick_curr + (ULLONG_MAX - tick_prev);
		}
		tick_prev = tick_curr;
		tick_net += tick_frame;
		
		uint8_t simul_steps = 0;
		while (tick_net >= stickbug_ticks && simul_steps < 1) { // For time-sensitive actions
			if (index < stickbug_sprite_len - 1) {
				index += 1;
			} else {
				index = 0;
			}
			tick_net -= stickbug_ticks;
			simul_steps += 1;
		}

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(BOT_SCREEN, C2D_WHITE);
		C2D_SceneBegin(BOT_SCREEN);
		C2D_DrawSprite(&stickbug_all[index]);
		C3D_FrameEnd(0);
	}

	C2D_SpriteSheetFree(stickbug1_sheet);
    C2D_SpriteSheetFree(stickbug2_sheet);
    C2D_SpriteSheetFree(stickbug3_sheet);

	return EXIT_SUCCESS;
}


void navigateMenu(void)
{
    gameTimer();

	// navigate between game titles
	if ((gamestate != PROTO) && (input.kDown & PROTO_SELECT)) { // CJ base
        launchCJQProto();
	} else if ((gamestate != PYRO) && (input.kDown & PYRO_SELECT)) { // CJ fire
        // launchCJQPyro();
		launchStickBug();
	} else if ((gamestate != HYDRO) && (input.kDown & HYDRO_SELECT)) { // CJ water
        // launchCJQHydro();
		launchStickBug();
	} else if ((gamestate != CRYO) && (input.kDown & CRYO_SELECT)) { // CJ snow
        // launchCJQCryo();
		launchStickBug();
	}

	// else
	if (gamestate == MENU) {
        // if (input.kDown & DEBUG_SELECT) {
        //     DEBUG_MODE = !DEBUG_MODE;
        // }
        if (DEBUG_MODE) {
			C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

			C2D_TargetClear(TOP_SCREEN, C2D_BLACK);
			C2D_TargetClear(BOT_SCREEN, C2D_BLACK);
			print_control_data(time_s, &(input.vtpad), &(input.vcpad), &(input.vaccl), &(input.vgyro), &(input.kHeld));

			C3D_FrameEnd(0);
			
			tick_prev = svcGetSystemTick();
		}
		else if (!DEBUG_MODE) {
			C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
			
			C2D_TargetClear(TOP_SCREEN, C2D_BLACK);
			C2D_SceneBegin(TOP_SCREEN);
			C2D_DrawSprite(&menu_top_gfx);
			
			C2D_TargetClear(BOT_SCREEN, C2D_BLACK);
			C2D_SceneBegin(BOT_SCREEN);
			C2D_DrawSprite(&menu_bot_gfx);
			
			C3D_FrameEnd(0);
			tick_prev = svcGetSystemTick();
        } 
	}
}


void scrollCards(uint8_t* element, uint8_t* rank)
{
    if (input.kDown & KEY_DUP) {
        if (*rank < CARD_BASIC_SHEET_LEN-1) {
            *rank += 1;
        } else {
            *rank = CARD_RANK_MIN;
        }
    } else if (input.kDown & KEY_DDOWN) {
        if (*rank > CARD_RANK_MIN) {
            *rank -= 1;
        } else {
            *rank = CARD_BASIC_SHEET_LEN-1;
        }
    }
    if (input.kDown & KEY_DRIGHT) {
        if (*element < CARD_ELEMENT_SNOW) {
            *element += 1;
        } else {
            *element = CARD_ELEMENT_FIRE;
        }
    } else if (input.kDown & KEY_DLEFT) {
        if (*element > CARD_ELEMENT_FIRE) {
            *element -= 1;
        } else {
            *element = CARD_ELEMENT_SNOW;
        }
    }
}


void refreshWait(size_t tick_delay)
{
    while(!checkDelayTimer(tick_prev, tick_delay)) {
        continue;
    }
    tick_prev = svcGetSystemTick();
}


void gameTimer(void)
{
    if  (checkDelayTimer(tick_prev, getTickDelay_ms(1000))) {
        time_s += 1;
        tick_prev = svcGetSystemTick();
    }
}


u64 getTickDelay_ms(u64 delay_ms)
{
	u64 delay_ticks = (delay_ms * SYSCLOCK_ARM11) / 1000;
	return delay_ticks;
}


u64 getTickDelay_hz(u64 delay_hz)
{
	u64 delay_ticks = SYSCLOCK_ARM11 / delay_hz;
	return delay_ticks;
}


bool checkDelayTimer(u64 tick_ref, u64 delay_ticks)
{
	u64 tick_curr = svcGetSystemTick();
	bool result;
	if  (tick_curr >= tick_ref) {
		result = (tick_curr - tick_ref) >= delay_ticks;
	} else {
		result = (tick_curr + (ULLONG_MAX - tick_ref)) >= delay_ticks;
	}
	return result;
}


void getBinaryRep(int num, int len, char* out)
{
	// Taken from https://www.geeksforgeeks.org/dsa/binary-representation-of-a-given-number/
    
	// Check for each bit.
    for (int i=(len-1); i>=0; i--) {
        
        // if  i'th bit is set 
        if  (num&(1<<i)) out[(len-1)-i] = '1';
		else out[(len-1)-i] = '0';
    }
	out[len] = '\0';
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
}


void print_tpad_matrix_data(Matrix2D_Float* A1, Matrix2D_Float* x1, Matrix2D_Float* u1, Matrix2D_Float* A2, Matrix2D_Float* x2, Matrix2D_Float* u2)
{
    // Prints touchpad matrix data

    // A1
    printf("\x1b[5;0H[[%+3.3f, %+3.3f]", mat2Dfloat_return(A1, 0, 0), mat2Dfloat_return(A1, 0, 1));
    printf("\x1b[6;0H [%+3.3f, %+3.3f]]", mat2Dfloat_return(A1, 1, 0), mat2Dfloat_return(A1, 1, 1));
    // x1
    printf("\x1b[5;20H[[%+3.3f]", mat2Dfloat_return(x1, 0, 0));
    printf("\x1b[6;20H [%+3.3f]]", mat2Dfloat_return(x1, 1, 0));
    // u1
    printf("\x1b[5;33H[[%+3.3f]", mat2Dfloat_return(u1, 0, 0));
    printf("\x1b[6;33H [%+3.3f]]", mat2Dfloat_return(u1, 1, 0));

    // A2
    printf("\x1b[9;0H[[%+3.3f, %+3.3f]", mat2Dfloat_return(A2, 0, 0), mat2Dfloat_return(A2, 0, 1));
    printf("\x1b[10;0H [%+3.3f, %+3.3f]]", mat2Dfloat_return(A2, 1, 0), mat2Dfloat_return(A2, 1, 1));
    // x2
    printf("\x1b[9;20H[[%+3.3f]", mat2Dfloat_return(x2, 0, 0));
    printf("\x1b[10;20H [%+3.3f]]", mat2Dfloat_return(x2, 1, 0));
    // u2
    printf("\x1b[9;33H[[%+3.3f]", mat2Dfloat_return(u2, 0, 0));
    printf("\x1b[10;33H [%+3.3f]]", mat2Dfloat_return(u2, 1, 0));
}
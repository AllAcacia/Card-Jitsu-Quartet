/*
Filename: gamestate.c
Author:   AllAcacia
*/

#include "gamestate.h"


CJQ_Gamestate gamestate;
C3D_RenderTarget* top_screen;
C3D_RenderTarget* bot_screen;

u64 tick_refresh_delay;
u64 ticks_timer_ref;
u64 ticks_refresh_ref;
u64 time_s = 0;


void screensInit(void)
{
    // Create screens
    top_screen = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	bot_screen = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
    tick_refresh_delay = getTickDelay(REFRESH_RATE);
}


int launchCJQProto(void)
{
    gamestate = PROTO;

    // Clear Screen
    consoleClear();
    print_menu();

    // Initialize C2D
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
    screensInit();

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
    float dyn_dt = ((float)REFRESH_RATE)/1000.0f;
    dynamicSS_init(&touchLocX_DTS, dyn_fn, dyn_xi, dyn_dt);
    dynamicSS_setstate(&touchLocX_DTS, BOTTOM_SCREEN_WIDTH/2, 0.0f);
    dynamicSS_init(&touchLocY_DTS, dyn_fn, dyn_xi, dyn_dt);
    dynamicSS_setstate(&touchLocY_DTS, BOTTOM_SCREEN_HEIGHT/2, 0.0f);

    float flt_to_tpadx = BOTTOM_SCREEN_WIDTH/8.08036f;
    float flt_to_tpady = BOTTOM_SCREEN_HEIGHT/6.05395f;

    // Set reference time for refresh
    ticks_refresh_ref = svcGetSystemTick();
    while (gamestate == PROTO) {
        // read inputs
        hidCaptureAllInputs();
        gameTimer();

        if (input.kDown & KEY_SELECT) {
            gamestate = MENU;
        }
        
        // run game
        scrollCards(&element_i, &rank_i);
        card_curr = &(cards_all[element_i][rank_i]);
        
        if(input.kHeld & KEY_TOUCH) { // If touch-screen pressed, iterate DTS
            dynamicSS_iterate(&touchLocX_DTS, ((float)input.vtpad.px)*flt_to_tpadx);
            dynamicSS_iterate(&touchLocY_DTS, ((float)input.vtpad.py)*flt_to_tpady);
        } else {
            dynamicSS_setstate(&touchLocX_DTS, card_curr->params.pos.x, 0.0f);
            dynamicSS_setstate(&touchLocY_DTS, card_curr->params.pos.y, 0.0f);
        } C2D_SpriteSetPos(card_curr, mat2Dfloat_return(&touchLocX_DTS.x1, 0, 0), mat2Dfloat_return(&touchLocY_DTS.x1, 0, 0));
        print_tpad_matrix_data(&touchLocX_DTS.F, &touchLocX_DTS.x1, &touchLocX_DTS.u1, &touchLocY_DTS.F, &touchLocY_DTS.x1, &touchLocY_DTS.u1);

        // Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(bot_screen, C2D_WHITE);
        C2D_SceneBegin(bot_screen);
        C2D_DrawSprite(card_curr);
        C3D_FrameEnd(0);
        
        refreshWait();
    }

    C2D_SpriteSheetFree(cards_fire_sheet);
    C2D_SpriteSheetFree(cards_water_sheet);
    C2D_SpriteSheetFree(cards_snow_sheet);

    C2D_Fini();
	C3D_Fini();

    return EXIT_SUCCESS;
}


int launchCJQPyro(void)
{
    gamestate = PYRO;

    // Clear Screen
    consoleClear();
    print_menu();

    // Initialize C2D
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
    screensInit();

    while (gamestate == PYRO) {
        // read inputs
        hidCaptureAllInputs();
        gameTimer();

        // run game
        if (input.kDown & KEY_SELECT) {
            gamestate = MENU;
        }

        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(bot_screen, C2D_BLACK);
        C3D_FrameEnd(0);
    }

    C2D_Fini();
	C3D_Fini();

    return EXIT_SUCCESS;
}


int launchCJQHydro(void)
{
    gamestate = HYDRO;

    consoleClear();
    print_menu();

    // load

    while (gamestate == HYDRO) {
        // read inputs
        hidCaptureAllInputs();
        gameTimer();

        // run game
        if (input.kDown & KEY_SELECT) {
            gamestate = MENU;
        }
    }
    return EXIT_SUCCESS;
}


int launchCJQCryo(void)
{
    gamestate = CRYO;

    consoleClear();
    print_menu();

    // load

    while (gamestate == CRYO) {
        // read inputs
        hidCaptureAllInputs();
        gameTimer();

        // run game
        if (input.kDown & KEY_SELECT) {
            gamestate = MENU;
        }
    }
    return EXIT_SUCCESS;
}


void navigateMenu(void)
{
    gameTimer();

	// navigate between game titles
	if  ((gamestate != PROTO) && (input.kDown & PROTO_SELECT)) { // CJ base
        launchCJQProto();
	} else if  ((gamestate != PYRO) && (input.kDown & PYRO_SELECT)) { // CJ fire
        launchCJQPyro();
	} else if  ((gamestate != HYDRO) && (input.kDown & HYDRO_SELECT)) { // CJ water
        launchCJQHydro();
	} else if  ((gamestate != CRYO) && (input.kDown & CRYO_SELECT)) { // CJ snow
        launchCJQCryo();
	}

	// else
	if (gamestate == MENU) {
		if (checkDelayTimer(ticks_refresh_ref, tick_refresh_delay)) {
			print_control_data(time_s, &(input.vtpad), &(input.vcpad), &(input.vaccl), &(input.vgyro), &(input.kHeld));
			ticks_refresh_ref = svcGetSystemTick();
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


void refreshWait(void)
{
    while(!checkDelayTimer(ticks_refresh_ref, tick_refresh_delay)) {
        continue;
    }
    ticks_refresh_ref = svcGetSystemTick();
}


void gameTimer(void)
{
    if  (checkDelayTimer(ticks_timer_ref, getTickDelay(1000))) {
        time_s += 1;
        ticks_timer_ref = svcGetSystemTick();
    }
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
/*
Filename: gamestate.c
Author:   AllAcacia
*/

#include "gamestate.h"


CJQ_Gamestate gamestate;
C3D_RenderTarget* top;
C3D_RenderTarget* bottom;

u64 tick_refresh_delay;
u64 ticks_timer_ref;
u64 ticks_refresh_ref;
u64 time_s = 0;


void screensInit(void)
{
    // Create screens
    top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
}


Sprite* loadCardSprites(C2D_SpriteSheet sheet)
{
    if  (!sheet) svcBreak(USERBREAK_PANIC);
    size_t cards_len = C2D_SpriteSheetCount(sheet);
    Sprite* cards_ptr = initSpritesFromSpritesheet(sheet, cards_len, 0.5f, 0.5f, BOTTOM_SCREEN_WIDTH/2, BOTTOM_SCREEN_HEIGHT/2);
    return cards_ptr;
}


int launchCJQProto(void)
{
    gamestate = PROTO;

    consoleClear();
    print_menu();

    // load
    C2D_SpriteSheet cards_fire_sheet = C2D_SpriteSheetLoad("romfs:/gfx/cards_basic_f.t3x");
    Sprite* cards_fire = loadCardSprites(cards_fire_sheet);
    C2D_SpriteSheet cards_water_sheet = C2D_SpriteSheetLoad("romfs:/gfx/cards_basic_w.t3x");
    Sprite* cards_water = loadCardSprites(cards_water_sheet);
    C2D_SpriteSheet cards_snow_sheet = C2D_SpriteSheetLoad("romfs:/gfx/cards_basic_s.t3x");
    Sprite* cards_snow = loadCardSprites(cards_snow_sheet);

    Sprite** cards_all = calloc(3, sizeof(Sprite*));
    cards_all[0] = cards_fire;
    cards_all[1] = cards_water;
    cards_all[2] = cards_snow;

    uint8_t element_i = 0;
    uint8_t rank_i = 0;

    while (gamestate == PROTO) {
        // read inputs
        hidCaptureAllInputs();
        gameTimer();
        if (input.kDown & KEY_SELECT) {
            gamestate = MENU;
        }
        
        // run game
        scrollCards(&element_i, &rank_i);
        Sprite* card_curr = &(cards_all[element_i][rank_i]);

        // Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(bottom, C2D_WHITE);
        C2D_SceneBegin(bottom);
        C2D_DrawSprite(&(card_curr->spr));
        C3D_FrameEnd(0);
    }

    C2D_SpriteSheetFree(cards_fire_sheet);
    free(cards_fire);
    C2D_SpriteSheetFree(cards_water_sheet);
    free(cards_water);
    C2D_SpriteSheetFree(cards_snow_sheet);
    free(cards_snow);

    return EXIT_SUCCESS;
}


int launchCJQPyro(void)
{
    gamestate = PYRO;

    consoleClear();
    print_menu();

    // load

    while (gamestate == PYRO) {
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
		if  (checkDelayTimer(ticks_refresh_ref, tick_refresh_delay)) {
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
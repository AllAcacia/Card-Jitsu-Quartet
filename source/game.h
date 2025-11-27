/*
Filename: game.h
Author:   AllAcacia
*/

#ifndef GAME_H
#define GAME_H


#define REFRESH_RATE 20 // ms
#define MENU_SELECT KEY_SELECT
#define PROTO_SELECT KEY_A
#define PYRO_SELECT KEY_B
#define HYDRO_SELECT KEY_Y
#define CRYO_SELECT KEY_X


typedef enum {
	MENU=0, // Menu
	PROTO,  // CJ Base
	PYRO,   // CJ Fire
	HYDRO,  // CJ Water
	CRYO    // CJ Snow
} CJQ_Gamestate;


void getBinaryRep(int num, int len, char* out);

u64 getTickDelay(u64 delay_ms);

bool checkDelayTimer(u64 tick_ref, u64 delay_ticks);

void print_control_data(u64 time_s, touchPosition* tpad, circlePosition* cpad, accelVector* accl, angularRate* gyro, u32* kheld);

void print_menu(void);

void navigateTitles(void);


#endif // GAME_H
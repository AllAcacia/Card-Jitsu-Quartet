/*
Filename: gamestate.c
Author:   AllAcacia
*/


#include <stdbool.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "controls.h"


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


int launchCJQProto(void);

int launchCJQPyro(void);

int launchCJQHydro(void);

int launchCJQCryo(void);

void navigateCJQStates(void);

void gameTimer(void);

u64 getTickDelay(u64 delay_ms);

bool checkDelayTimer(u64 tick_ref, u64 delay_ticks);

void getBinaryRep(int num, int len, char* out);

void print_control_data(u64 time_s, touchPosition* tpad, circlePosition* cpad, accelVector* accl, angularRate* gyro, u32* kheld);

void print_menu(void);
/*
Filename: game.h
Author:   AllAcacia
*/

#ifndef GAME_H
#define GAME_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>
#include <3ds.h>
#include <citro2d.h>
#include "controls.h"
#include "sprites.h"
#include "controls.h"
#include "dynamics.h"
#include "sprites.h"
#include "citro2d.h"


#define REFRESH_RATE 20 // ms
#define MENU_SELECT KEY_SELECT
#define PROTO_SELECT KEY_A
#define PYRO_SELECT KEY_B
#define HYDRO_SELECT KEY_Y
#define CRYO_SELECT KEY_X

// Relocate to card module!!!
#define CARD_BASIC_SHEET_LEN 72
#define CARD_ELEMENT_FIRE 0
#define CARD_ELEMENT_WATER 1
#define CARD_ELEMENT_SNOW 2
#define CARD_RANK_MIN 0
#define CARD_RANK_MAX 12

#define FIRE_CARDS_NUM 72
#define WATER_CARDS_NUM 72
#define SNOW_CARDS_NUM 72


typedef enum {
	MENU=0, // Menu
	PROTO,  // CJ Base
	PYRO,   // CJ Fire
	HYDRO,  // CJ Water
	CRYO    // CJ Snow
} CJQ_Gamestate;


void screensInit(void);

int launchCJQProto(void);

int launchCJQPyro(void);

int launchCJQHydro(void);

int launchCJQCryo(void);

void navigateMenu(void);

void refreshWait(void);

void scrollCards(uint8_t* element, uint8_t* rank);

void gameTimer(void);

u64 getTickDelay(u64 delay_ms);

bool checkDelayTimer(u64 tick_ref, u64 delay_ticks);

void getBinaryRep(int num, int len, char* out);

void print_control_data(u64 time_s, touchPosition* tpad, circlePosition* cpad, accelVector* accl, angularRate* gyro, u32* kheld);

void print_menu(void);

void print_tpad_matrix_data(Matrix2D_Float* A1, Matrix2D_Float* x1, Matrix2D_Float* u1, Matrix2D_Float* A2, Matrix2D_Float* x2, Matrix2D_Float* u2);


#endif // GAME_H
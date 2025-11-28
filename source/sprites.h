/*
Filename: sprites.h
Author:   AllAcacia
*/

#ifndef SPRITES_H
#define SPRITES_H

#include <citro2d.h>
#include <stdlib.h>

#define MAX_SPRITES 768
#define TOP_SCREEN_WIDTH 400
#define TOP_SCREEN_HEIGHT 240
#define BOTTOM_SCREEN_WIDTH 320
#define BOTTOM_SCREEN_HEIGHT 240

#define C2D_BLACK C2D_Color32(0, 0, 0, 255)
#define C2D_WHITE C2D_Color32(255, 255, 255, 255)


typedef struct
{
	C2D_Sprite spr;
	float dx; // velocity x
    float dy; // velocity y
} Sprite;


Sprite* initSpritesFromSpritesheet(C2D_SpriteSheet spriteSheet, size_t numImages, float piv_x, float piv_y, uint8_t loc_x, uint8_t loc_y);


#endif // SPRITES_H
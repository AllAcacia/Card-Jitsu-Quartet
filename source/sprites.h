/*
Filename: sprites.h
Author:   AllAcacia
*/

#ifndef SPRITES_H
#define SPRITES_H

#include <citro2d.h>
#include <stdlib.h>

#define MAX_SPRITES 768
#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240


typedef struct
{
	C2D_Sprite spr;
    float x;  // position x
    float y;  // position y
	float dx; // velocity x
    float dy; // velocity y
} Sprite;


Sprite* initSpritesFromSpritesheet(C2D_SpriteSheet spriteSheet);


#endif // SPRITES_H
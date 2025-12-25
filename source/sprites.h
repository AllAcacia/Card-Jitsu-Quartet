/*
Filename: sprites.h
Author:   AllAcacia
*/

#ifndef SPRITES_H
#define SPRITES_H

#include <citro2d.h>
#include <stdlib.h>
#include <3ds.h>

#define MAX_SPRITES 768
#define TOP_SCREEN_WIDTH 400
#define TOP_SCREEN_HEIGHT 240
#define BOTTOM_SCREEN_WIDTH 320
#define BOTTOM_SCREEN_HEIGHT 240

#define C2D_BLACK C2D_Color32(0, 0, 0, 255)
#define C2D_WHITE C2D_Color32(255, 255, 255, 255)


void loadSpritesFromSpritesheet(C2D_Sprite* ptr, C2D_SpriteSheet sheet, size_t numImages, float piv_x, float piv_y, uint16_t loc_x, uint16_t loc_y, float rot_d);


#endif // SPRITES_H
/*
Filename: sprites.c
Author:   AllAcacia
*/

#include "sprites.h"


Sprite* initSpritesFromSpritesheet(C2D_SpriteSheet spriteSheet, size_t numImages, float piv_x, float piv_y, uint8_t loc_x, uint8_t loc_y)
{
    // Returns a pointer to an array of sprite struct objects
    Sprite* spritesPtr = calloc(numImages, sizeof(Sprite));

	for (size_t i = 0; i < numImages; i += 1)
	{
		Sprite* s = &spritesPtr[i];
		C2D_SpriteFromSheet(&s->spr, spriteSheet, i);
		C2D_SpriteSetCenter(&s->spr, piv_x, piv_y);
		C2D_SpriteSetPos(&s->spr, loc_x, loc_y);
		s->dx = 0;
		s->dy = 0;
	}

    return spritesPtr;
}

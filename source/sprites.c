/*
Filename: sprites.c
Author:   AllAcacia
*/

#include "sprites.h"


Sprite* initSpritesFromSpritesheet(C2D_SpriteSheet spriteSheet)
{
    // Returns a pointer to an array of sprite struct objects

	size_t numImages = C2D_SpriteSheetCount(spriteSheet);
    Sprite* spritesPtr = calloc(numImages, sizeof(Sprite));

	for (size_t i = 0; i < numImages; i += 1)
	{
		Sprite sprite = spritesPtr[i];

		// Random image, position, rotation and speed
		C2D_SpriteFromSheet(&sprite.spr, spriteSheet, i);
		C2D_SpriteSetCenter(&sprite.spr, 0.5f, 0.5f);
		C2D_SpriteSetPos(&sprite.spr, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
		sprite.dx = 0;
		sprite.dy = 0;
	}

    return spritesPtr;
}
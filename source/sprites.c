/*
Filename: sprites.c
Author:   AllAcacia
*/

#include "sprites.h"


void loadSpritesFromSpritesheet(C2D_Sprite* ptr, C2D_SpriteSheet sheet, size_t numImages, float piv_x, float piv_y, uint16_t loc_x, uint16_t loc_y, float rot_d)
{
    if (!sheet) svcBreak(USERBREAK_PANIC);

    for (size_t i = 0; i < numImages; i += 1)
	{
		C2D_Sprite* s = &ptr[i];
		C2D_SpriteFromSheet(s, sheet, i);
		C2D_SpriteSetCenter(s, piv_x, piv_y);
		C2D_SpriteSetPos(s, loc_x, loc_y);
		C2D_SpriteRotateDegrees(s, rot_d);
	}
}
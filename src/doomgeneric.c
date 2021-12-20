#include "doomgeneric.h"

uint32_t* DG_ScreenBuffer = 0;


void dg_Create()
{
	DG_ScreenBuffer = malloc(DOOMGENERIC_RESX * DOOMGENERIC_RESY * sizeof(uint32_t));

	DG_Init();
}


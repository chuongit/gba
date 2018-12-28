#include "common/gba.h"
#include "common/sprite.h"
#include "common/dma.h"

u16* OAM = (u16*) 0x7000000;
OAMEntry sprites[128];
pRotData rotData = (pRotData) sprites;

//void gba_cpy16(u16* target, u16* source, u16 count) {
//	int i;
//	for (i = 0; i < count; ++i) {
//		target[i] = source[i];
//	}
//}

// Copy with DMA 3
// 2 bytes each (DMA_16)
void DMAcopy(u32 source, u32 dest, int size) {
	//DMA3Copy(source, dest, size|DMA_ENABLE|DMA_TIMING_IMMEDIATE|DMA_16|DMA_SOURCE_INCREMENT|DMA_DEST_INCREMENT, 0);
	REG_DMA3SAD = (u32) source;
	REG_DMA3DAD = (u32) dest;
	REG_DMA3CNT = size | DMA_ENABLE | DMA_TIMING_IMMEDIATE | DMA_16
			| DMA_SOURCE_INCREMENT | DMA_DEST_INCREMENT;
}

void CopyOAM() {
	//	u16 loop;
	//	u16* temp;
	//	temp = (u16*) sprites;
	//	for (loop = 0; loop < 128 * 4; loop++) {
	//		OAM[loop] = temp[loop];
	//	}

	//	gba_cpy16(OAM, (u16*)sprites, 128 * 4);
	DMAcopy(sprites, OAM, 128 * 4);
}

//Set sprites to off screen
void InitializeSprites() {
	int loop;
	for (loop = 0; loop < 128; loop++) {
		sprites[loop].attribute0 = 160; //y to > 159
		sprites[loop].attribute1 = 240; //x to > 239
	}
}

//move the sprite
void MoveSprite(OAMEntry* sp, int x, int y) {
	if (x < 0) //if it is off the left correct
		x = 512 + x;
	if (y < 0) //if off the top correct
		y = 256 + y;

	sp->attribute1 = sp->attribute1 & 0xFE00; //clear the old x value
	sp->attribute1 = sp->attribute1 | x;

	sp->attribute0 = sp->attribute0 & 0xFF00; //clear the old y value
	sp->attribute0 = sp->attribute0 | y;
}


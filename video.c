#include "common/gba.h"
#include "common/screenmode.h"

// video memory
u16* videoBuffer = (u16*) 0x6000000;
u16* FrontBuffer = (u16*) 0x6000000;
u16* BackBuffer = (u16*) 0x600A000;

//  Fliping between framebuffers
void flip() {
	if (REG_DISPCNT & BACKBUFFER) {
		//back buffer is the current buffer so we need to switch it to the font buffer
		REG_DISPCNT &= ~BACKBUFFER; //flip active buffer to front buffer by clearing back buffer bit
		videoBuffer = BackBuffer; //now we point our drawing buffer to the back buffer
	} else {
		//front buffer is active so switch it to backbuffer
		REG_DISPCNT |= BACKBUFFER; //flip active buffer to back buffer by setting back buffer bit
		videoBuffer = FrontBuffer; //now we point our drawing buffer to the front buffer
	}
}

// Mode 4 is 240(120)x160 by 8bit
void plotPixel(int x, int y, unsigned short int c) {
	videoBuffer[(y) * 120 + (x)] = (c);
}

#ifndef INTERUPT_H
#define INTERUPT_H

#include "gba.h"

#define INT_VBLANK 0x0001
#define INT_HBLANK 0x0002 
#define INT_VCOUNT 0x0004 //you can set the display to generate an interrupt when it reaches a particular line on the screen
#define INT_TIMER0 0x0008
#define INT_TIMER1 0x0010
#define INT_TIMER2 0x0020 
#define INT_TIMER3 0x0040
#define INT_COMMUNICATION 0x0080 //serial communication interupt
#define INT_DMA0 0x0100
#define INT_DMA1 0x0200
#define INT_DMA2 0x0400
#define INT_DMA3 0x0800
#define INT_KEYBOARD 0x1000
#define INT_CART 0x2000 //the cart can actually generate an interupt
#define INT_ALL 0x4000 //this is just a flag we can set to allow the my function to enable or disable all interrupts. Doesn't actually correspond to a bit in REG_IE 

// LPC
#define DISP_INT_VBLANK (1 << 3)
#define DISP_INT_HBLANK (1 << 4)

#endif

#include "gba.h"
/******************************************\
* DMA.h by dovoto editet by night
\******************************************/

#ifndef DMA_H
#define DMA_H

//these defines let you control individual bit in the control register

#define DMA_ENABLE 0x80000000
#define DMA_INTERUPT_ENABLE 0x40000000
#define DMA_TIMING_IMMEDIATE 0x00000000
#define DMA_TIMING_VBLANK 0x10000000
#define DMA_TIMING_HBLANK 0x20000000
#define DMA_TIMING_SYNC_TO_DISPLAY 0x30000000
#define DMA_16 0x00000000
#define DMA_32 0x04000000
#define DMA_REPEATE 0x02000000
#define DMA_SOURCE_INCREMENT 0x00000000
#define DMA_SOURCE_DECREMENT 0x00800000
#define DMA_SOURCE_FIXED 0x01000000
#define DMA_DEST_INCREMENT 0x00000000
#define DMA_DEST_DECREMENT 0x00200000
#define DMA_DEST_FIXED 0x00400000
#define DMA_DEST_RELOAD 0x00600000

#define DMA_32NOW DMA_ENABLE | DMA_TIMING_IMMEDIATE |DMA_32 
#define DMA_16NOW DMA_ENABLE | DMA_TIMING_IMMEDIATE |DMA_16 

//macro to make a DMA channel 3 copy from src to dest. amt words are copied. The dma 3 controll register bits 22-31 are set according to params.
#define DMA3Copy(src,dest,amt,params)(REG_DMA3SAD=(u32)(src),REG_DMA3DAD=(u32)(dest),REG_DMA3CNT=(amt)|(params));

#endif //end interrupts.h


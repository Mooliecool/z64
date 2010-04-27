#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "ibg.h"
//#include "z64lib.h"

#ifndef GLOBAL_H
#define GLOBAL_H

void ColorCombiner(unsigned int w0, unsigned int w1, char * text);
void DumpTexture(int timg, int NPixels, int fmt, int siz, char C_Textures[], char texName[], int DumpTex);

typedef union {
    unsigned char b[8];
    int w[2];
} DLCommand;

typedef struct {
	char text[0x2000];
	int start;
} C_DL;

typedef struct {
	signed short x, y, z;
	signed short s, t;
	unsigned char r, g, b, a;
} Vertex;

#define Flip32(word)\
	(((word&0xFF000000) >> 0x18)|\
	 ((word&0x00FF0000) >> 0x08)|\
	 ((word&0x0000FF00) << 0x08)|\
	 ((word&0x000000FF) << 0x18))

#define Write16(Buffer, Offset, Value)\
	Buffer[Offset]	= (Value&0xFF00)>>8;\
	Buffer[Offset+1]= (Value & 0x00FF);

#define Write32(Buffer, Offset, Value)\
	Buffer[Offset] = (Value & 0xFF000000) >> 24;\
	Buffer[Offset + 1] = (Value & 0x00FF0000) >> 16;\
	Buffer[Offset + 2] = (Value & 0x0000FF00) >> 8;\
	Buffer[Offset + 3] = (Value & 0x000000FF);\

#define Write64(Buffer, Offset, Value1, Value2)\
	Buffer[Offset] = (Value1 & 0xFF000000) >> 24;\
	Buffer[Offset + 1] = (Value1 & 0x00FF0000) >> 16;\
	Buffer[Offset + 2] = (Value1 & 0x0000FF00) >> 8;\
	Buffer[Offset + 3] = (Value1 & 0x000000FF);\
	Buffer[Offset + 4] = (Value2 & 0xFF000000) >> 24;\
	Buffer[Offset + 5] = (Value2 & 0x00FF0000) >> 16;\
	Buffer[Offset + 6] = (Value2 & 0x0000FF00) >> 8;\
	Buffer[Offset + 7] = (Value2 & 0x000000FF);

#define Read16(Buffer, Offset)\
	(Buffer[Offset]<<8)|\
	Buffer[Offset+1]

/* for reading the offset in (segment<<24)|offset pairs */
#define Read24(Buffer, Offset)\
	(Buffer[Offset] << 16)|\
	(Buffer[Offset + 1] << 8)|\
	Buffer[Offset + 2]

#define Read32(Buffer, Offset)\
	(Buffer[Offset] << 24)|\
	(Buffer[Offset + 1] << 16)|\
	(Buffer[Offset + 2] << 8)|\
	Buffer[Offset + 3]

#define Copy16(SrcBuffer, DestBuffer, SrcOffset, DestOffset)\
	DestBuffer[DestOffset+0] = SrcBuffer[SrcOffset+0];\
	DestBuffer[DestOffset+1] = SrcBuffer[SrcOffset+1];

#define Copy32(SrcBuffer, DestBuffer, SrcOffset, DestOffset)\
	DestBuffer[DestOffset+0] = SrcBuffer[SrcOffset+0];\
	DestBuffer[DestOffset+1] = SrcBuffer[SrcOffset+1];\
	DestBuffer[DestOffset+2] = SrcBuffer[SrcOffset+2];\
	DestBuffer[DestOffset+3] = SrcBuffer[SrcOffset+3];

unsigned char * RAMSeg[16];
    
#define true 1
#define false 0

#endif

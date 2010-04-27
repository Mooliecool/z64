#ifndef Z64LIB_H
#define Z64LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "read.h"

//defines/macros
#define COMPRESS_NONE	0
#define COMPRESS_YAZ0	1
#define COMPRESS_YAY0	2
#define COMPRESS_UNKN	3

#define ZEROS		"/x00/x00/x00/x00/x00/x00/x00/x00/x00/x00/x00/x00/x00/x00/x00/x00"
#define Z64BOOTADDRS	"\x00\x00\x00\x00\x00\x00\x10\x60\x00\x00\x00\x00\x00\x00\x00\x00"

#define GetFileOffsets(PS, PE, VS, VE, PTR_ADDR, ROM)\
{\
	VS = Flip32(Read32(ROM, PTR_ADDR));\
	VE = Flip32(Read32(ROM, PTR_ADDR+4));\
	PS = Flip32(Read32(ROM, PTR_ADDR+8));\
	PE = Flip32(Read32(ROM, PTR_ADDR+12));\
}

#define strtolower(char *str)\
	for(;*str;str++)*str=tolower(*str);

//structs
typedef struct {
	char * data;
	int size, compress;	// compress is one of four values above OR'd to the decompressed filesize. size is physical filesize
} ZFile;

//variables
int ZSetup;
int ZFileTableOffset;
int zMaxFile;

//functions
int SetupROM(char * ROM);	//must be called first!!
ZFile getFile(int fileNo);
void zfree();

#endif

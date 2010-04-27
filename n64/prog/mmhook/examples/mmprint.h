#include "mips.h"

#ifndef MMPRINT_H
 #define MMPRINT_H

extern void _0x801AEC68(int a1, int a0);
extern int _0x801AD26C(int a0);

extern void _0x800878D0(u32 unk[]);
extern void _0x8008792C(u32 unk[], int a0);
extern void TextXY(u32 unk[], int x, int y); //800872EC
extern void TextRGBA(u32 unk[], int r, int g, int b, int a); //8008727C
extern void TextString(u32 unk[], char * str, ...); //800879AC

extern void TextDo(u32 unk[]);

//.org 80780000
extern void _start(int a0, int a1) __attribute__ ((section (".main")));
extern void message(int a0);
#endif /* MMPRINT_H */

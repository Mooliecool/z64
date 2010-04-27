#include "../mips.h"
//.org 80070f6c		//rom: 0xae810c
	.text
	
	.set noat
	.set noreorder
	.global _nop
	.ent _nop
_nop:
	nop
	
	.end _nop
	.set at
	.set reorder

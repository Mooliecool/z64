#include "../mips.h"
//.org 80000460		//rom: 0x1060	this makes it so our routine is not overwritten when oot clears all of the ram when booting.

	.text
	
	.set noat
	.set noreorder
	.global noclear
	.ent noclear
noclear:
	lui t6, 0x0060
	nop
	
	.end noclear
	.set at
	.set reorder

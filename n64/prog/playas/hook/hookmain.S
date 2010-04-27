#include "../mips.h"
//.org 80070784		//rom: 0xae7924. hook to first routine
	.text

	.set noat
	.set noreorder
	.global hookmain
	.ent hookmain
hookmain:
	jal 0x80600800
	or a0, s0, $zero
	j 0x80070bf8
	nop
	.end hookmain
	.set at
	.set reorder

#include "../mips.h"

//ROM address: 0x00C08520
//RAM address: 0x803D9820
//Null relocation that is no longer needed and messes stuff up:
//ROM @ 0x00C24DDC: 00000000
	.text
	.set noreorder
	.set noat
	.global spawnhook
	.ent spawnhook
	
spawnhook:
	addiu	$sp, $sp, -64
	//stuff that this function originally did that must be done
	sw	a0, 64($sp)
	sw	a1, 68($sp)
	sw	a2, 72($sp)
	sw	$ra, 44($sp)
	sw	a3, 76($sp)
	//Hook to custom routine
	lw	a1, 64($sp)	//arg 1
	jal	0x80600000
	addiu	a0, a1, 0x1C24	//arg 0
	lw	$ra, 44($sp)
	jr	$ra
	addiu	$sp, $sp, 64
	
	.end spawnhook
	.set at
	.set reorder

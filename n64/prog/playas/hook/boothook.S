#include "../mips.h"
//.org 80000400		//rom: 0x1000

	.text

	.set noat
	.set noreorder
	.global boothook
	.ent boothook
boothook:
	/* transfer a block from ROM using DMA */
	lui t2, 0xa460
	lui t1, 0x0060
	sw t1, 0x0000(t2)	/* RAM destination */
	lui t1, 0x1360
	sw t1, 0x0004(t2)	/* ROM source */
	lui t1, 0x0000
	ori t1, $zero, 0xffff	/* transfer size */
	sw t1, 0x000c(t2)
dmacheck:
	lw	t1, 0x0010(t2)
	bne	t1, $zero, dmacheck /* wait until transfer is done */
	nop
	lui	t2, 0x8000
	lui	$sp, 0x8001
	addiu	t2, t2, 0x0498
	jr	t2		/* begin oot */
	addiu	$sp, $sp, 0x2d60
	
	.end boothook
	.set at
	.set reorder









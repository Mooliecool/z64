/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ **
** N64 DMA  - Thanks ZZT32 :)      **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "mips.h"

typedef struct
{
	/* Pointers to data */
	u32 ramp;
	u32 romp;
 
	/* Filesizes (8-byte aligned) */
	u32 size_ramrom; /* RAM -> ROM */
	u32 size_romram; /* RAM <- ROM */
 
	/* Status register */
	u32 status;
} DMA_REG;
 
/* DMA status flags */
enum
{
	DMA_BUSY  = 0x00000001,
	DMA_ERROR = 0x00000008
};
 
/* DMA registers ptr */
static volatile DMA_REG * dmaregs = (DMA_REG*)0xA4600000;
 
/* Copy data from ROM to RAM */
int dma_write_ram ( int ram_ptr, int rom_ptr, u32 length )
{
	/* Check that DMA is not busy already */
	while( dmaregs->status & DMA_BUSY );
 
	/* Write addresses */
	dmaregs->ramp = ram_ptr & 0x00FFFFFF; /* ram pointer */
	dmaregs->romp = (rom_ptr & 0x1FFFFFFF)|0x10000000; /* rom pointer */
 
	/* Write size */
	dmaregs->size_romram = length - 1;
 
	/* Wait for transfer to finish */
	while( dmaregs->status & DMA_BUSY );
 
	/* Return size written */
	return length & 0xFFFFFFF8;
}

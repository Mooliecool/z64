/*
 modify and redistribute at will, so long as my name (spinout) is kept here
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compress.h"

/*
 prototype:
  zCompress ROMDEC.z64 ROMCOMP.z64
*/

u8 * Data;
int pos;
u8 * encodeAll(u8 * src, int srcSize );

#define DMADATA_START	0x12F70
#define FILE_END	0x19030
#define SWAP32(w)\
	(unsigned int) ((w&0xFF)<<24)|((w&0xFF00)<<8)|((w&0xFF0000)>>8)|(w>>24)
#define TRUE	1
#define FALSE	0
//#define DEBUG

typedef struct{
    unsigned vStart;
    unsigned vEnd;
    unsigned pStart;
    unsigned pEnd;
} filePtr;

const short decompress_files[]={ /* Skyboxes */
942,	944,	946,	948,	950,	952,	954,	956,
958,	960,	962,	964,	966,	968,	970,	972,
974,	976,	978,	980,	982,	984,	986,	988,
990,	992,	994,	996,	998,	1000,	1002,	1004
};

int compress_file(int filenumber)
{
    int i;
    filenumber++;
    for (i=0;i<(sizeof(decompress_files)/sizeof(short));i++)
    {
        if (decompress_files[i]==filenumber)
            return FALSE;
    }
    return TRUE;
}
int
main(int argc, char* argv[])
{
    
    
    if (argc < 3)
    {
        printf("Error: Not enough arguments supplied. Prototype:\n%s ROMDEC.z64 ROMCOMP.z64\n", argv[0]);
        return -1;
    }
    
    FILE * rom_out_file = fopen(argv[2],"wb");
    FILE * rom_in = fopen(argv[1],"rb");
    
    fseek( rom_in, 0, SEEK_END);
    int fend = ftell( rom_in);
    
    char * romIn = (char*) malloc(fend);
    
    rewind( rom_in);
    fread( romIn, 1, fend, rom_in);
    fclose(rom_in);
    
    char * romOut = (char*) malloc(0x2800000);
    
    char status[] = "                                  ";
    float pDone;
    int pDoneInt;
    
    int romOutStart = (int)&romOut[0];
    int romInStart = (int)&romIn[0];
    
    filePtr (*outFiles)[] = (void*)(romOutStart + DMADATA_START);
    filePtr (*inFiles)[] = (void*)(romInStart + DMADATA_START);
    
    int fcnt = 27;
    
    int CurrEnd = SWAP32((*inFiles)[fcnt].pStart) + SWAP32((*inFiles)[fcnt].vEnd) - SWAP32((*inFiles)[fcnt].vStart);
    fcnt++;
    
    memcpy(romOut, romIn, CurrEnd);
    
    int writeEnd=FALSE;
    int orig_siz, orig_start;
        
    char * compressed;
    
    while ( SWAP32((*inFiles)[fcnt].vEnd) )
    {
        (*outFiles)[fcnt].pStart = SWAP32(CurrEnd);
        
        orig_siz = ( SWAP32((*inFiles)[fcnt].vEnd) ) - ( SWAP32((*inFiles)[fcnt].vStart) ) ;
        orig_start = SWAP32((*inFiles)[fcnt].pStart);
        
        //status bar
        pDone = (float)fcnt / 15.31 ;
        pDoneInt = (int)pDone / 3;
        memset(status, '=', pDoneInt);
        status[pDoneInt] = '>';
        
        if (compress_file(fcnt))
        {
            printf("[%s] %.2f%% (File %i/1531) [%08X]\r", status, pDone, fcnt, orig_siz);
            fflush(stdout);
            compressed = encodeAll( (u8*)((romInStart) + orig_start), orig_siz );
            if (pos > orig_siz)
                goto decompressed;
            memcpy((char*)(romOutStart+CurrEnd), Data, pos);
            free(Data); //huge mem leak if you forget this
            CurrEnd += pos;
            if (CurrEnd&0xF)
                CurrEnd = 0x10 + (CurrEnd & 0xFFFFFFF0);
            (*outFiles)[fcnt].pEnd = SWAP32(CurrEnd);
        }
        else
        {
            decompressed:
            printf("[%s] %.2f%% (File %i/1531) [skipping]\r", status, pDone, fcnt);
            fflush(stdout);
            memcpy((char*)(romOutStart+CurrEnd), (char*)(romInStart+orig_start), orig_siz);
            CurrEnd+=orig_siz;
            (*outFiles)[fcnt].pEnd = 0;
        }
                
        fcnt++;
    }
    
    printf("\nDone, buffering and saving to file\n");
    if (CurrEnd & 0xFFFFF)
    {
        CurrEnd &= 0xFFF00000;
        CurrEnd += 0x00100000;
    }
    fwrite(romOut, 1, CurrEnd, rom_out_file);
    fclose(rom_out_file);
    
    free(romIn);
    free(romOut);
    return 0;
}

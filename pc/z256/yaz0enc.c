/*
    Yaz0 recompressor
    -----------------
    Original by `shevious`, made into more of a UNIX
    program by ZZT32 (http://64.vg/)
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <glib.h>

#include "compress.h"

FILE * out;
u8 * Data;
int dstSize;
int sflag;
int pos;

// simple and straight encoding scheme for Yaz0
u32 simpleEnc(u8* src, int size, int pos, u32 *pMatchPos)
{
  int startPos = pos - 0x1000, j, i;
  u32 numBytes = 1;
  u32 matchPos = 0;

  if (startPos < 0)
    startPos = 0;
  for (i = startPos; i < pos; i++)
  {
    for (j = 0; j < size-pos; j++)
    {
      if (src[i+j] != src[j+pos])
        break;
    }
    if (j > numBytes)
    {
      numBytes = j;
      matchPos = i;
    }
  }
  *pMatchPos = matchPos;
  if (numBytes == 2)
    numBytes = 1;
  return numBytes;
}



// a lookahead encoding scheme for ngc Yaz0
u32 nintendoEnc(u8* src, int size, int pos, u32 *pMatchPos)
{
  int startPos = pos - 0x1000;
  u32 numBytes = 1;
  static u32 numBytes1;
  static u32 matchPos;
  static int prevFlag = 0;

  // if prevFlag is set, it means that the previous position was determined by look-ahead try.
  // so just use it. this is not the best optimization, but nintendo's choice for speed.
  if (prevFlag == 1) {
    *pMatchPos = matchPos;
    prevFlag = 0;
    return numBytes1;
  }
  prevFlag = 0;
  numBytes = simpleEnc(src, size, pos, &matchPos);
  *pMatchPos = matchPos;

  // if this position is RLE encoded, then compare to copying 1 byte and next position(pos+1) encoding
  if (numBytes >= 3) {
    numBytes1 = simpleEnc(src, size, pos+1, &matchPos);
    // if the next position encoding is +2 longer than current position, choose it.
    // this does not guarantee the best optimization, but fairly good optimization with speed.
    if (numBytes1 >= numBytes+2) {
      numBytes = 1;
      prevFlag = 1;
    }
  }
  return numBytes;
}



int encodeYaz0(u8* src, int srcSize, u8 * Data)
{
  Ret r = { 0, 0 };
  u8 dst[24];    // 8 codes * 3 bytes maximum
  int dstSize = 0;
  int percent = -1;
  int i;
  static double perc, last_perc;
  
  u32 validBitCount = 0; //number of valid bits left in "code" byte
  u8 currCodeByte = 0;
  while(r.srcPos < srcSize)
  {
    u32 numBytes;
    u32 matchPos;
    u32 srcPosBak;

    numBytes = nintendoEnc(src, srcSize, r.srcPos, &matchPos);
    if (numBytes < 3)
    {
      //straight copy
      dst[r.dstPos] = src[r.srcPos];
      r.dstPos++;
      r.srcPos++;
      //set flag for straight copy
      currCodeByte |= (0x80 >> validBitCount);
    }
    else 
    {
      //RLE part
      u32 dist = r.srcPos - matchPos - 1; 
      u8 byte1, byte2, byte3;

      if (numBytes >= 0x12)  // 3 byte encoding
      {
        byte1 = 0 | (dist >> 8);
        byte2 = dist & 0xff;
        dst[r.dstPos++] = byte1;
        dst[r.dstPos++] = byte2;
        // maximum runlength for 3 byte encoding
        if (numBytes > 0xff+0x12)
          numBytes = 0xff+0x12;
        byte3 = numBytes - 0x12;
        dst[r.dstPos++] = byte3;
      } 
      else  // 2 byte encoding
      {
        byte1 = ((numBytes - 2) << 4) | (dist >> 8);
        byte2 = dist & 0xff;
        dst[r.dstPos++] = byte1;
        dst[r.dstPos++] = byte2;
      }
      r.srcPos += numBytes;
    }
    validBitCount++;
    //write eight codes
    if(validBitCount == 8)
    {
      Data[pos] = currCodeByte;
      pos++;
      for (i=0;i</*=*/r.dstPos;pos++,i++)
        Data[pos] = dst[i];
      dstSize += r.dstPos+1;

      srcPosBak = r.srcPos;
      currCodeByte = 0;
      validBitCount = 0;
      r.dstPos = 0;
    }
  }
  if(validBitCount > 0)
  {
    Data[pos] = currCodeByte;
    pos++;
    for (i=0;i</*=*/r.dstPos;pos++,i++)
      Data[pos] = dst[i];
    dstSize += r.dstPos+1;

    currCodeByte = 0;
    validBitCount = 0;
    r.dstPos = 0;
  }
    
  return dstSize;
}



u8 * encodeAll(u8 * src, int srcSize )
{
  Data = (u8*)malloc(srcSize * 2); //should be enough, sometimes outsize > insize; thats what the *2 is for

  // write 4 bytes yaz0 header
  Data[0] = 'Y';
  Data[1] = 'a';
  Data[2] = 'z';
  Data[3] = '0';
  
  // write 4 bytes uncompressed size
  Data[4] = (srcSize >> 24) & 0xFF;
  Data[5] = (srcSize >> 16) & 0xFF;
  Data[6] = (srcSize >> 8) & 0xFF;
  Data[7] = srcSize & 0xFF;

  // write 8 bytes unused dummy 
  memset(&Data[8], 0, 8);
  
  pos=16;
  
  //encode
  dstSize = encodeYaz0(src, srcSize, Data);
  
  return Data; 
}

void encodeOut(u8 * src, int srcSize)
{
  fwrite(encodeAll( src, srcSize), 1, 1, out);
}

#if 0
int main( int argc, char ** argv )
{
    int c, ch;
    GByteArray * input;
    FILE * in = stdin;
    out = stdout;
    
    /* Process args */
    while( (ch = getopt(argc, argv, "ios")) != -1 )
    {
        switch( ch )
        {
            /* Input file flag */
            case 'i':
             if( !(in = fopen(argv[optind], "rb")) )
                 exit( -1 );
            break;
            
            /* Output file flag */
            case 'o':
             if( !(out = fopen(argv[optind], "wb")) )
                 exit( -1 );
            break;
			
			/* No output */
			case 's':
			 sflag = 1;
			break;
        }
    }
    
    /* Create a new data buffer */
    input = g_byte_array_sized_new(4 * 1024 * 1024);
    
    /* Fill the buffer */
    while( (c = fgetc(in)) != EOF )
    {
        unsigned char j = c;
        
        /* Append */
        input = g_byte_array_append( input, &j, 1 );
    }
    
    /* Call it */
    encodeOut( input->data, input->len );
    
    /* Finished */
	if( !sflag )
	{
		printf( "\r%u -> %u; %.2f%% of original.\n", 
		input->len, dstSize, (double)dstSize / input->len * 100.0 );
	}
    
    /* Free list */
    g_byte_array_free( input, TRUE );
    
    return 0;
}
#endif

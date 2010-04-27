typedef unsigned char u8;
typedef unsigned int u32;

FILE * out;
u8 * Data;
int dstSize;
int sflag;
int pos;

typedef struct Ret
{
  int srcPos, dstPos;
}
Ret;

u8 * encodeAll(u8 * src, int srcSize );

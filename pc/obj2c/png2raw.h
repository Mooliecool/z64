#include "obj2c.h"

#ifndef PNG2RAW_H
 #define PNG2RAW_H

int xsize, ysize;
char CTextures[32768];
int mode;
int bpp;

int read_png(char *file_name, char * dl);

#define	GPACK_RGBA5551(r, g, b, a)	((((r)<<8) & 0xf800) | 		\
					 (((g)<<3) & 0x7c0) |		\
					 (((b)>>2) & 0x3e) | ((a >> 7)&1)) /* why did SGI do ((a)&1) ??? */

#endif /* PNG2RAW_H */

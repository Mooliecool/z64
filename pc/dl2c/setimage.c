#include <stdio.h>


#define _SHIFTL( v, s, w )	\
    (((unsigned int)v & ((0x01 << w) - 1)) << s)
#define _SHIFTR( v, s, w )	\
    (((unsigned int)v >> s) & ((0x01 << w) - 1))

#define dgsSetImage(w0, w1, fmt, f, s, w, i)	\
	fmt = _SHIFTR(w0, 24, 8); \
	f = _SHIFTR(w0, 21, 3);	\
	s = _SHIFTR(w0, 19, 2);\
	w = _SHIFTR(w0, 0, 12)+1;	\
	i = w1

int main()
{
	int w0 = 0xFD900000;
	int w1 = 0x04032490;
	int fmt, f, s, w, i;
	dgsSetImage(w0, w1, fmt, f, s, w, i);
	printf("fmt %X f %X s %X w %X i: %08X\n", fmt, f, s, w, i);
	return 0;
}

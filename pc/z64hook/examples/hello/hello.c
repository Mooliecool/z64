#include "mips.h"

void SetTextRGBA(void * DList, u8 r, u8 g, u8 b, u8 a);
void SetTextXY(void * DList, u16 x, u16 y);
void SetTextString(void * DList, char * format, ...);

u8 r=0;
u8 g=0;
u8 b=0;
u8 m=0;

void _start(void * a0)
{
    SetTextRGBA(a0, r, g, b, 255);
    SetTextXY(a0, 0, 7);
    SetTextString(a0, "Hello, world!\n#%02X%02X%02X", r, g, b);
    switch (m)
    {
    case 0:
        if (r == 254)
            m++;
        r++;
        break;
    case 1:
        if (g == 254)
            m++;
        g++;
        break;
    case 2:
        if (b == 254)
            m++;
        b++;
        break;
    case 3:
        if (r == 1)
            m++;
        r--;
        break;
    case 4:
        if (g == 1)
            m++;
        g--;
        break;
    case 5:
        if (b == 1)
            m++;
        b--;
        break;
    case 6:
        m=0;
        break;
    }
}

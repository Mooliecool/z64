#include "mmprint.h"

/* hook
.org 801AEE9C / 0x00D8D3DC

or a0, s0, r0
jal 80780000
or a1, s1, r0
nop
nop
nop

020020250C1E000002202825000000000000000000000000
*/

void _start(int a0, int a1)
{
    do {
        _0x801AEC68(a1, a0);
        message(a0);
    } while(_0x801AD26C(a0));
}


void
message(int a0)
{
    u32 buffer[13];
    int i;
    for(i=0;i<13;i++)
        buffer[i] = 0xFFFA5A5A;
    _0x800878D0(buffer);
    _0x8008792C(buffer, a0);

    TextXY(buffer, 0, 0); //800872EC
    TextRGBA(buffer, 255, 127, 255, 255); //8008727C
    TextString(buffer, "Hello, world!"); //800879AC

    TextDo(buffer); //0x80087974
}


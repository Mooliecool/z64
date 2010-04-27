#include "zelda64.h"

/*
CONTROLS (P1)
R trigger: switch type (byte, half, double)
L trigger: increase inc amount
D-Pad U/D: increase/decrease current value
D-Pad L/R: increase/decrease address
D-Pad D + Z: goto clipboard address if possible
Z + R: Copy
Z + L: Paste
*/

#define CURR_ADDR	0x807FFF00
#define INC	0x807FFF04
#define DATATYPE	0x807FFF08
#define BYTE	0
#define SHORT	1
#define LONG	2
#define ASCII_BUFF	0x807FFF10
#define CLIPBOARD	0x807FFF30


void _start(void * a0)
{
    Controller  *input  = (void*)P1_CONTROLLER;
    u32 *CurrAddr	= (void*)CURR_ADDR;
    u32 *inc	= (void*)INC;
    u32 *clipboard	= (void*)CLIPBOARD;
    u8  *datatype	= (void*)DATATYPE;
    
    u8 (*memoryu8)[0x7F0000]	= (void*)0x80000000;
    u16 (*memoryu16)[0x3F8000]	= (void*)0x80000000;
    u32 (*memoryu32)[0x1FC000]	= (void*)0x80000000;
    
    
    static char *typeStrFmt[]	= { "%02X",	"%04X",	"%08X"	};
    static u32 typeAndVals[3]	= { 0xFFFFFFFF,	0xFFFFFFFE,	0xFFFFFFFC	};
    
    u32 incSizBytes	= (0x01020400 >> ((3 - *datatype) << 3)) & 0xFF;
    
    *CurrAddr	&= typeAndVals[*datatype];
    u32 addrinc = *inc << *datatype;
    if (addrinc > 0x80000)
    {
        addrinc = 0x80000;
    }
    
    if (!(*inc))
    {
        *inc = incSizBytes;
    }
    
    SetTextRGBA( a0, 0xFF, 0xFF, 0xFF, 0xFF);
    SetTextXY( a0, 0, 7);
    SetTextString( a0, "MemEdit\nInc: %07X Addr inc: %06X\nAddress: 8%07X Type: %ibit", *inc, addrinc, *CurrAddr, incSizBytes<<3);
    
    int i	= 0;
    int CurrAddrTmp	= (*CurrAddr & 0xFFFFFFF0);
    int valNum	= (*CurrAddr) >> *datatype;
    
    while (i < 16)
    {
        if (!(i&7))
        {
            SetTextString(a0, "\n8%07X: ", CurrAddrTmp+i);
        }
        else if (!(i & 3))
        {
            SetTextString(a0, " ");
        }
        
        if (i == (*CurrAddr & 0xF))
        {
            SetTextRGBA(a0, 0x00, 0x00, 0x00, 0xFF);
            switch (*datatype)
            {
            case BYTE:
                SetTextString(a0, typeStrFmt[*datatype], (*memoryu8)[valNum]);
                break;
            case SHORT:
                SetTextString(a0, typeStrFmt[*datatype], (*memoryu16)[valNum]);
                break;
            case LONG:
                SetTextString(a0, typeStrFmt[*datatype], (*memoryu32)[valNum]);
                break;
            }
            SetTextRGBA(a0, 0xFF, 0xFF, 0xFF, 0xFF);
        }
        else
        {
            switch (*datatype)
            {
            case BYTE:
                SetTextString(a0, typeStrFmt[*datatype], (*memoryu8)[CurrAddrTmp+i]);
                break;
            case SHORT:
                SetTextString(a0, typeStrFmt[*datatype], (*memoryu16)[(CurrAddrTmp+i)>>1]);
                break;
            case LONG:
                SetTextString(a0, typeStrFmt[*datatype], (*memoryu32)[(CurrAddrTmp+i)>>2]);
                break;
            }
        }
        
        i += incSizBytes;
    }
    /*
    char (*memory_src)[16]	=(void*)(0x80000000|CurrAddrTmp);
    char (*memory_dest)[17]	=(void*)(ASCII_BUFF);
    (*memory_dest)[0]	= '\n';
    i=1;
    while (i < 17)
    {
        char src_byte = (*memory_src)[i-1];
        if ((src_byte > 0x1F) || (src_byte < 0x7F))
        {
            (*memory_dest)[i] = src_byte;
        }
        else
        {
            (*memory_dest)[i] = 0x2E;
        }
        i++;
    }
    
    SetTextString(a0, (char*)ASCII_BUFF );
    */
    switch (input->pad)
    {
    case BUTTON_R:
        if ((*datatype < 2))
        {
            *datatype+=1;
        }
        else
        {
            *datatype = 0;
        }
        break;
    case BUTTON_L:
        if ((*inc ) && ( *inc < 0x4000000))
        {
            *inc <<= 1;
        }
        else
        {
            *inc = 1;
        }
        break;
    case BUTTON_DPAD_R:
        if ((*CurrAddr + addrinc) < 0x007FE000)
        {
            *CurrAddr += addrinc;
        }
        else
        {
            *CurrAddr = 0x007FE000;
        }
        break;
    case BUTTON_DPAD_L:
        if (((signed int)*CurrAddr - (signed int)addrinc) > 0)
        {
            *CurrAddr -= addrinc;
        }
        else
        {
            *CurrAddr = 0;
        }
        break;
    case BUTTON_DPAD_U:
        switch (*datatype)
        {
        case BYTE:
            if (*inc < 0x100)
            {
                (*memoryu8)[valNum] += *inc;
            }
            break;
        case SHORT:
            if (*inc < 0x10000)
            {
                (*memoryu16)[valNum] += *inc;
            }
            break;
        case LONG:
            (*memoryu32)[valNum] += *inc;
            break;
        }
        break;
    case BUTTON_DPAD_D:
        switch (*datatype)
        {
        case BYTE:
            if ( *inc < 0x100)
            {
                (*memoryu8)[valNum] -= *inc;
            }
            break;
        case SHORT:
            if (*inc < 0x10000)
            {
                (*memoryu16)[valNum] -= *inc;
            }
            break;
        case LONG:
            (*memoryu32)[valNum] -= *inc;
            break;
        }
        break;
    case BUTTON_Z|BUTTON_R:
        switch (*datatype)
        {
        case BYTE:
            *clipboard=(*memoryu8)[valNum];
            break;
        case SHORT:
            *clipboard=(*memoryu16)[valNum];
            break;
        case LONG:
            *clipboard=(*memoryu32)[valNum];
            break;
        }
        break;
    case BUTTON_Z|BUTTON_L:
        switch (*datatype)
        {
        case BYTE:
            (*memoryu8)[valNum]=*clipboard&0xFF;
            break;
        case SHORT:
            (*memoryu16)[valNum]=*clipboard&0xFFFF;
            break;
        case LONG:
            (*memoryu32)[valNum]=*clipboard;
            break;
        }
        break;
    case BUTTON_Z|BUTTON_DPAD_D:
        if ((*clipboard & 0x7FFFFFFF) < 0x7FE000)
        {
            *CurrAddr = *clipboard & 0x07FFFFF;
        }
        break;
    }
    SetTextString(a0, "\nClipboard: %08X", *clipboard);
}


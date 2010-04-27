/*
 mipsdis.c
 redistribute and modify at will, my name is spinout, keep it here.
*/

#include <stdio.h>
#include <stdlib.h>
#include "../../pc/mipsdis/r3400i.h"
#include "zelda64.h"

u32 pc = 0x80000000;
char dis_op[64];
u32 DisplayList;
Controller *input1 = (void*)P1_CONTROLLER;
int inc=1;
int i;

extern void _start (u32 a0) __attribute__ ((section (".main"))); /* forces _start() to be at beggining of binary */

const char * gpr_rn[32]={
"r0",	"at",	"v0",	"v1",	"a0",	"a1",	"a2",	"a3",
"t0",	"t1",	"t2",	"t3",	"t4",	"t5",	"t6",	"t7",
"s0",	"s1",	"s2",	"s3",	"s4",	"s5",	"s6",	"s7",
"t8",	"t9",	"k0",	"k1",	"gp",	"sp",	"s8",	"ra"
};
const char * cop_rn[32]={
"Index",	"Random",	"EntryLo0",	"EntryLo1",	"Context",	"PageMask",	"Wired",	"Reserved",
"BadVAddr",	"Count",	"EntryHi",	"Compare",	"Status",	"Cause",	"Epc",		"PRevID",
"Config",	"LLAddr",	"WatchLo",	"WatchHi",	"XContext",	"Reserved",	"Reserved",	"Reserved",
"Reserved",	"Reserved",	"PErr",		"CacheErr",	"TagLo",	"TagHi",	"ErrorEpc",	"Reserved"
};
const char * fpr_rn[32]={
"f0",	"f1",	"f2",	"f3",	"f4",	"f5",	"f6",	"f7",
"f8",	"f9",	"f10",	"f11",	"f12",	"f13",	"f14",	"f15",
"f16",	"f17",	"f18",	"f19",	"f20",	"f21",	"f22",	"f23",
"f24",	"f25",	"f26",	"f27",	"f28",	"f29",	"f30",	"f31"
};


void COP1_NONE(u32 word)
{
    SetTextString(DisplayList, "(Invalid COP1: %08X)\n", (unsigned int)word);
}

void TLB_NONE(u32 word)
{
    SetTextString(DisplayList, "(Invalid TLB: %08X)\n", (unsigned int)word);
}

void COP0_NONE(u32 word)
{
    SetTextString(DisplayList, "(Invalid COP0: %08X)\n", (unsigned int)word);
}

void NONE(u32 word)
{
    SetTextString(DisplayList, "(Invalid opcode: %08X)\n", (unsigned int)word);
}
/*
 cop1
 MIPS R3400i Co-processor 1 (FPU processor)
*/

//COP1.L
void CVT_S_L(u32 word)
{
    SetTextString(DisplayList, "cvt.s.l\t%s, %s\n", fpr_rn[getFD(word)],fpr_rn[getFS(word)]);
}

void CVT_D_L(u32 word)
{
    SetTextString(DisplayList, "cvt.d.l\t%s, %s\n", fpr_rn[getFD(word)],fpr_rn[getFS(word)]);
}

//COP1.W
void CVT_S_W(u32 word)
{
    SetTextString(DisplayList, "cvt.s.w\t%s, %s\n", fpr_rn[getFD(word)],fpr_rn[getFS(word)]);
}

void CVT_D_W(u32 word)
{
    SetTextString(DisplayList, "cvt.d.w\t%s, %s\n", fpr_rn[getFD(word)],fpr_rn[getFS(word)]);
}

//COP1.S
void ADD_S(u32 word)
{	//00 (00)
    SetTextString(DisplayList, "add.s\t%s, %s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)], fpr_rn[getFT(word)]);
}

void SUB_S(u32 word)
{	//01 (01)
    SetTextString(DisplayList, "sub.s\t%s, %s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)], fpr_rn[getFT(word)]);
}

void MUL_S(u32 word)
{	//02 (02)
    SetTextString(DisplayList, "mul.s\t%s, %s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)], fpr_rn[getFT(word)]);
}

void DIV_S(u32 word)
{	//03 (03)
    SetTextString(DisplayList, "div.s\t%s, %s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)], fpr_rn[getFT(word)]);
}

void SQRT_S(u32 word)
{	//04 (04)
    SetTextString(DisplayList, "sqrt.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void ABS_S(u32 word)
{	//05 (05)
    SetTextString(DisplayList, "abs.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void MOV_S(u32 word)
{	//06 (06)
    SetTextString(DisplayList, "mov.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void NEG_S(u32 word)
{	//07 (07)
    SetTextString(DisplayList, "neg.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void ROUND_L_S(u32 word)
{	//08 (08)
    SetTextString(DisplayList, "round.l.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void TRUNC_L_S(u32 word)
{	//09 (09)
    SetTextString(DisplayList, "trunc.l.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void CEIL_L_S(u32 word)
{	//10 (0A)
    SetTextString(DisplayList, "ceil.l.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void FLOOR_L_S(u32 word)
{	//11 (0B)
    SetTextString(DisplayList, "floor.l.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void ROUND_W_S(u32 word)
{	//12 (0C)
    SetTextString(DisplayList, "round.w.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void TRUNC_W_S(u32 word)
{	//13 (0D)
    SetTextString(DisplayList, "trunc.w.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void CEIL_W_S(u32 word)
{	//14 (0E)
    SetTextString(DisplayList, "ceil.w.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void FLOOR_W_S(u32 word)
{	//15 (0F)
    SetTextString(DisplayList, "floor.w.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void CVT_D_S(u32 word)
{	//33 (21)
    SetTextString(DisplayList, "cvt.d.s\t%s, %s\n", fpr_rn[getFD(word)],fpr_rn[getFS(word)]);
}

void CVT_W_S(u32 word)
{	//36 (24)
    SetTextString(DisplayList, "cvt.w.s\t%s, %s\n", fpr_rn[getFD(word)],fpr_rn[getFS(word)]);
}

void CVT_L_S(u32 word)
{	//37 (25)
    SetTextString(DisplayList, "cvt.l.s\t%s, %s\n", fpr_rn[getFD(word)],fpr_rn[getFS(word)]);
}

void C_F_S(u32 word)
{	//48 (30)
    SetTextString(DisplayList, "c.f.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_UN_S(u32 word)
{	//49 (31)
    SetTextString(DisplayList, "c.un.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_EQ_S(u32 word)
{	//50 (32)
    SetTextString(DisplayList, "c.eq.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_UEQ_S(u32 word)
{	//51 (33)
    SetTextString(DisplayList, "c.ueq.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_OLT_S(u32 word)
{	//52 (34)
    SetTextString(DisplayList, "c.olt.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_ULT_S(u32 word)
{	//53 (35)
    SetTextString(DisplayList, "c.ult.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_OLE_S(u32 word)
{	//54 (36)
    SetTextString(DisplayList, "c.ole.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_ULE_S(u32 word)
{	//55 (37)
    SetTextString(DisplayList, "c.ule.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_SF_S(u32 word)
{	//56 (38)
    SetTextString(DisplayList, "c.sf.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_NGLE_S(u32 word)
{	//57 (39)
    SetTextString(DisplayList, "c.ngle.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_SEQ_S(u32 word)
{	//58 (3A)
    SetTextString(DisplayList, "c.seq.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_NGL_S(u32 word)
{	//59 (3B)
    SetTextString(DisplayList, "c.ngl.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_LT_S(u32 word)
{	//60 (3C)
    SetTextString(DisplayList, "c.lt.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_NGE_S(u32 word)
{	//61 (3D)
    SetTextString(DisplayList, "c.nge.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_LE_S(u32 word)
{	//62 (3E)
    SetTextString(DisplayList, "c.le.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_NGT_S(u32 word)
{	//63 (3F)
    SetTextString(DisplayList, "c.ngt.s\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

Handler S_T[64] = {
ADD_S,	SUB_S,	MUL_S,	DIV_S,	SQRT_S,	ABS_S,	MOV_S,	NEG_S,	
ROUND_L_S,	TRUNC_L_S,	CEIL_L_S,	FLOOR_L_S,	ROUND_W_S,	TRUNC_W_S,	CEIL_W_S,	FLOOR_W_S,	
COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	
COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	
COP1_NONE,	CVT_D_S,	COP1_NONE,	COP1_NONE,	CVT_W_S,	CVT_L_S,	COP1_NONE,	COP1_NONE,	
COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	
C_F_S,	C_UN_S,	C_EQ_S,	C_UEQ_S,	C_OLT_S,	C_ULT_S,	C_OLE_S,	C_ULE_S,	
C_SF_S,	C_NGLE_S,	C_SEQ_S,	C_NGL_S,	C_LT_S,	C_NGE_S,	C_LE_S,	C_NGT_S
};

//COP1.D    
void ADD_D(u32 word)
{	//00 (00)
    SetTextString(DisplayList, "add.d\t%s, %s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)], fpr_rn[getFT(word)]);
}

void SUB_D(u32 word)
{	//01 (01)
    SetTextString(DisplayList, "sub.d\t%s, %s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)], fpr_rn[getFT(word)]);
}

void MUL_D(u32 word)
{	//02 (02)
    SetTextString(DisplayList, "mul.d\t%s, %s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)], fpr_rn[getFT(word)]);
}

void DIV_D(u32 word)
{	//03 (03)
    SetTextString(DisplayList, "add.d\t%s, %s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)], fpr_rn[getFT(word)]);
}

void SQRT_D(u32 word)
{	//04 (04)
    SetTextString(DisplayList, "sqrt.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void ABS_D(u32 word)
{	//05 (05)
    SetTextString(DisplayList, "abs.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void MOV_D(u32 word)
{	//06 (06)
    SetTextString(DisplayList, "mov.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void NEG_D(u32 word)
{	//07 (07)
    SetTextString(DisplayList, "neg.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void ROUND_L_D(u32 word)
{	//08 (08)
    SetTextString(DisplayList, "round.l.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void TRUNC_L_D(u32 word)
{	//09 (09)
    SetTextString(DisplayList, "trunc.l.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void CEIL_L_D(u32 word)
{	//10 (0A)
    SetTextString(DisplayList, "ceil.l.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void FLOOR_L_D(u32 word)
{	//11 (0B)
    SetTextString(DisplayList, "floor.l.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void ROUND_W_D(u32 word)
{	//12 (0C)
    SetTextString(DisplayList, "round.w.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void TRUNC_W_D(u32 word)
{	//13 (0D)
    SetTextString(DisplayList, "trunc.w.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void CEIL_W_D(u32 word)
{	//14 (0E)
    SetTextString(DisplayList, "ceil.w.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void FLOOR_W_D(u32 word)
{	//15 (0F)
    SetTextString(DisplayList, "floor.w.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void CVT_S_D(u32 word)
{	//32 (20)
    SetTextString(DisplayList, "cvt.s.d\t%s, %s\n", fpr_rn[getFD(word)],fpr_rn[getFS(word)]);
}

void CVT_W_D(u32 word)
{	//36 (24)
    SetTextString(DisplayList, "cvt.w.d\t%s, %s\n", fpr_rn[getFD(word)],fpr_rn[getFS(word)]);
}

void CVT_L_D(u32 word)
{	//37 (25)
    SetTextString(DisplayList, "cvt.l.d\t%s, %s\n", fpr_rn[getFD(word)],fpr_rn[getFS(word)]);
}

void C_F_D(u32 word)
{	//48 (30)
    SetTextString(DisplayList, "c.f.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_UN_D(u32 word)
{	//49 (31)
    SetTextString(DisplayList, "c.un.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_EQ_D(u32 word)
{	//50 (32)
    SetTextString(DisplayList, "c.eq.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_UEQ_D(u32 word)
{	//51 (33)
    SetTextString(DisplayList, "c.ueq.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_OLT_D(u32 word)
{	//52 (34)
    SetTextString(DisplayList, "c.olt.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_ULT_D(u32 word)
{	//53 (35)
    SetTextString(DisplayList, "c.ult.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_OLE_D(u32 word)
{	//54 (36)
    SetTextString(DisplayList, "c.ole.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_ULE_D(u32 word)
{	//55 (37)
    SetTextString(DisplayList, "c.ule.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_SF_D(u32 word)
{	//56 (38)
    SetTextString(DisplayList, "c.df.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_NGLE_D(u32 word)
{	//57 (39)
    SetTextString(DisplayList, "c.ngle.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_SEQ_D(u32 word)
{	//58 (3A)
    SetTextString(DisplayList, "c.deq.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_NGL_D(u32 word)
{	//59 (3B)
    SetTextString(DisplayList, "c.ngl.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_LT_D(u32 word)
{	//60 (3C)
    SetTextString(DisplayList, "c.lt.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_NGE_D(u32 word)
{	//61 (3D)
    SetTextString(DisplayList, "c.nge.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_LE_D(u32 word)
{	//62 (3E)
    SetTextString(DisplayList, "c.le.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_NGT_D(u32 word)
{	//63 (3F)
    SetTextString(DisplayList, "c.ngt.d\t%s, %s\n", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

Handler D_T[64] = {
ADD_D,	SUB_D,	MUL_D,	DIV_D,	SQRT_D,	ABS_D,	MOV_D,	NEG_D,	
ROUND_L_D,	TRUNC_L_D,	CEIL_L_D,	FLOOR_L_D,	ROUND_W_D,	TRUNC_W_D,	CEIL_W_D,	FLOOR_W_D,	
COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	
COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	
CVT_S_D,	COP1_NONE,	COP1_NONE,	COP1_NONE,	CVT_W_D,	CVT_L_D,	COP1_NONE,	COP1_NONE,	
COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	
C_F_D,	C_UN_D,	C_EQ_D,	C_UEQ_D,	C_OLT_D,	C_ULT_D,	C_OLE_D,	C_ULE_D,	
C_SF_D,	C_NGLE_D,	C_SEQ_D,	C_NGL_D,	C_LT_D,	C_NGE_D,	C_LE_D,	C_NGT_D
};

//COP1.BC opcodes
void BC1F(u32 word)
{
    SetTextString(DisplayList, "bc1f\t%08X\n", getOFFSET(word) + ((i<<2) | 0x80000000) + 4);
}

void BC1T(u32 word)
{
    SetTextString(DisplayList, "bc1t\t%08X\n", getOFFSET(word) + ((i<<2) | 0x80000000) + 4);
}

void BC1FL(u32 word)
{
    SetTextString(DisplayList, "bc1fl\t%08X\n", getOFFSET(word) + ((i<<2) | 0x80000000) + 4);
}

void BC1TL(u32 word)
{
    SetTextString(DisplayList, "bc1tl\t%08X\n", getOFFSET(word) + ((i<<2) | 0x80000000) + 4);
}

Handler BC_T[4] = {BC1F, BC1T, BC1FL, BC1TL};

//COP1 op types

void MFC1(u32 word)
{		//00 (00)
    SetTextString(DisplayList, "mfc1\t%s, %s\n", gpr_rn[getRT(word)], fpr_rn[getFD(word)]);
}

void DMFC1(u32 word)
{	//01 (01)
    SetTextString(DisplayList, "dmfc1\t%s, %s\n", gpr_rn[getRT(word)], fpr_rn[getFD(word)]);
}

void CFC1(u32 word)
{		//02 (02)
    SetTextString(DisplayList, "cfc1\t%s, %s\n", gpr_rn[getRT(word)], fpr_rn[getFD(word)]);
}

void MTC1(u32 word)
{		//04 (04)
    SetTextString(DisplayList, "mtc1\t%s, %s\n", gpr_rn[getRT(word)], fpr_rn[getFD(word)]);
}

void DMTC1(u32 word)
{	//05 (05)
    SetTextString(DisplayList, "dmtc1\t%s, %s\n", gpr_rn[getRT(word)], fpr_rn[getFD(word)]);
}

void CTC1(u32 word)
{		//06 (06)
    SetTextString(DisplayList, "ctc1\t%s, %s\n", gpr_rn[getRT(word)], fpr_rn[getFD(word)]);
}

void BC(u32 word)
{		//08 (08)
    BC_T[(word >> 16) & 3](word);
}

void S(u32 word)
{		//16 (10)
    S_T[word & 63](word);
}

void D(u32 word)
{		//17 (11)
    D_T[word & 63](word);
}

void W(u32 word)
{		//20 (14)
    if (!(word & 0x1E))
    {
        if (word & 1)
            CVT_D_W(word);
        else
            CVT_S_W(word);
    }
    else
        COP1_NONE(word);
}

void L(u32 word)
{		//21 (15)
    if (!(word & 0x1E))
    {
        if (word & 1)
            CVT_D_L(word);
        else
            CVT_S_L(word);
    }
    else
        COP1_NONE(word);
}

Handler COP1_T[32] = {
MFC1,	DMFC1,	CFC1,	COP1_NONE,	MTC1,	DMTC1,	CTC1,	COP1_NONE,	
BC,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	
S,	D,	COP1_NONE,	COP1_NONE,	W,	L,	COP1_NONE,	COP1_NONE,	
COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE,	COP1_NONE
};

/*
 cop0
 MIPS R3400i Co-processor 0 (Status processor)
*/

/* TLB op types */


void TLBR(u32 word)
{
    SetTextString(DisplayList, "tlbr\n");
}

void TLBWI(u32 word)
{
    SetTextString(DisplayList, "tlbwi\n");
}

void TLBWR(u32 word)
{
    SetTextString(DisplayList, "tlbwr\n");
}

void TLBP(u32 word)
{
    SetTextString(DisplayList, "tlbp\n");
}

void ERET(u32 word)
{
    SetTextString(DisplayList, "eret\n");
}

Handler TLB_T[64] = {
TLB_NONE,	TLBR,		TLBWI,		TLB_NONE,	TLB_NONE,	TLB_NONE,	TLBWR,		TLB_NONE,
TLBP,		TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,
TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,
ERET,		TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,
TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,
TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,
TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,
TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE,	TLB_NONE
};

/* COP0 op types */

void MFC0(u32 word)
{		/* 00 */
    SetTextString(DisplayList, "mfc0\t%s, %s\n", gpr_rn[getRT(word)], cop_rn[getFS(word)]);
}

void MTC0(u32 word)
{		/* 04 */
    SetTextString(DisplayList, "mtc0\t%s, %s\n", gpr_rn[getRT(word)], cop_rn[getFS(word)]);
}

void TLB(u32 word)
{
    TLB_T[word & 63](word);
}

Handler COP0_T[32] = {
MFC0,	COP0_NONE,	COP0_NONE,	COP0_NONE,	MTC0,		COP0_NONE,	COP0_NONE,	COP0_NONE,
NONE,	COP0_NONE,	COP0_NONE,	COP0_NONE,	COP0_NONE,	COP0_NONE,	COP0_NONE,	COP0_NONE,
TLB,	COP0_NONE,	COP0_NONE,	COP0_NONE,	COP0_NONE,	COP0_NONE,	COP0_NONE,	COP0_NONE,
NONE,	COP0_NONE,	COP0_NONE,	COP0_NONE,	COP0_NONE,	COP0_NONE,	COP0_NONE,	COP0_NONE
};

/*
 mips
 Main Processor
*/

// REGIMM op types
void BLTZ(u32 word)
{
    SetTextString(DisplayList, "bltz\t%s, 0x%08X\n", gpr_rn[getRS(word)], getOFFSET(word) + ((i<<2) | 0x80000000) + 4);
}

void BGEZ(u32 word)
{
    SetTextString(DisplayList, "bgez\t%s, 0x%08X\n", gpr_rn[getRS(word)], getOFFSET(word) + ((i<<2) | 0x80000000) + 4);
}

void BLTZL(u32 word)
{
    SetTextString(DisplayList, "bltzl\t%s, 0x%08X\n", gpr_rn[getRS(word)], getOFFSET(word) + ((i<<2) | 0x80000000) + 4);
}

void BGEZL(u32 word)
{
    SetTextString(DisplayList, "bgezl\t%s, 0x%08X\n", gpr_rn[getRS(word)], getOFFSET(word) + ((i<<2) | 0x80000000) + 4);
}

void TGEI(u32 word)
{
    SetTextString(DisplayList, "tgei\t%s, 0x%08X\n", gpr_rn[getRS(word)], getIMM(word));
}

void TGEIU(u32 word)
{
    SetTextString(DisplayList, "tgeiu\t%s 0x%08X\n", gpr_rn[getRS(word)], getIMM(word));
}

void TLTI(u32 word)
{
    SetTextString(DisplayList, "tlti\t%s, 0x%08X\n", gpr_rn[getRS(word)], getIMM(word));
}

void TLTIU(u32 word)
{
    SetTextString(DisplayList, "tltiu\t%s, 0x%08X\n", gpr_rn[getRS(word)], getIMM(word));
}

void TEQI(u32 word)
{
    SetTextString(DisplayList, "tqei\t%s, 0x%08X\n", gpr_rn[getRS(word)], getIMM(word));
}

void TNEI(u32 word)
{
    SetTextString(DisplayList, "tnei\t%s, 0x%08X\n", gpr_rn[getRS(word)], getIMM(word));
}

void BLTZAL(u32 word)
{
    SetTextString(DisplayList, "bltzal\t%s, 0x%08X\n", gpr_rn[getRS(word)], getOFFSET(word) + ((i<<2) | 0x80000000) + 4);
}

void BGEZAL(u32 word)
{
    SetTextString(DisplayList, "bgezal\t%s, 0x%08X\n", gpr_rn[getRS(word)], getOFFSET(word) + ((i<<2) | 0x80000000) + 4);
}

void BLTZALL(u32 word)
{
    SetTextString(DisplayList, "bltzall\t%s, 0x%08X\n", gpr_rn[getRS(word)], getOFFSET(word) + ((i<<2) | 0x80000000) + 4);
}

void BGEZALL(u32 word)
{
    SetTextString(DisplayList, "bgezall\t%s, 0x%08X\n", gpr_rn[getRS(word)], getOFFSET(word) + ((i<<2) | 0x80000000) + 4);
}

Handler REGIMM_T[32] = {
BLTZ,	BGEZ,	BLTZL,	BGEZL,	NONE,	NONE,	NONE,	NONE,
TGEI,	TGEIU,	TLTI,	TLTIU,	TEQI,	NONE,	TNEI,	NONE,
BLTZAL,BGEZAL,	BLTZALL,BGEZALL,NONE,	NONE,	NONE,	NONE,
NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE
};

//SPECIAL op types
void SLL(u32 word)
{		//00 (00)
    if (word)
        SetTextString(DisplayList, "sll\t%s, %s, %02X\n", gpr_rn[getRD(word)], gpr_rn[getRT(word)], getSA(word) );
    else
        SetTextString(DisplayList, "nop\n");
}
void SRL(u32 word)
{		//02 (02)
    SetTextString(DisplayList, "srl\t%s, %s, %02X\n", gpr_rn[getRD(word)],gpr_rn[getRT(word)],getSA(word));
}

void SRA(u32 word)
{		//03 (03)
    SetTextString(DisplayList, "sra\t%s, %s, %02X\n", gpr_rn[getRD(word)],gpr_rn[getRT(word)],getSA(word));
}

void SLLV(u32 word)
{		//04 (04)
    SetTextString(DisplayList, "sllv\t%s, %s, %s\n", gpr_rn[getRD(word)],gpr_rn[getRT(word)],gpr_rn[getRS(word)]);
}

void SRLV(u32 word)
{		//06 (06)
    SetTextString(DisplayList, "srlv\t%s, %s, %s\n", gpr_rn[getRD(word)],gpr_rn[getRT(word)],gpr_rn[getRS(word)]);
}

void SRAV(u32 word)
{		//07 (07)
    SetTextString(DisplayList, "srav\t%s, %s, %s\n", gpr_rn[getRD(word)],gpr_rn[getRT(word)],gpr_rn[getRS(word)]);
}

void JR(u32 word)
{		//08 (08)
    SetTextString(DisplayList, "jr\t%s\n", gpr_rn[getRS(word)]);
}

void JALR(u32 word)
{		//09 (09)
    SetTextString(DisplayList, "jalr\t%s, %s\n", gpr_rn[getRS(word)],gpr_rn[getRD(word)]);
}

void SYSCALL(u32 word)
{	//12 (0C)
    SetTextString(DisplayList, "syscall\t0x%04X\n", getIMM(word));
}

void BREAK(u32 word)
{	//13 (0D)
    SetTextString(DisplayList, "break\t0x%04X\n", getIMM(word));
}

void SYNC(u32 word)
{		//15 (0F)
    SetTextString(DisplayList, "sync\n");
}

void MFHI(u32 word)
{		//16 (10)
    SetTextString(DisplayList, "mfhi\t%s\n", gpr_rn[getRD(word)]);
}

void MTHI(u32 word)
{		//17 (11)
    SetTextString(DisplayList, "mthi\t%s\n", gpr_rn[getRD(word)]);
}

void MFLO(u32 word)
{		//18 (12)
    SetTextString(DisplayList, "mflo\t%s\n", gpr_rn[getRD(word)]);
}

void MTLO(u32 word)
{		//19 (13)
    SetTextString(DisplayList, "mtlo\t%s\n", gpr_rn[getRD(word)]);
}

void DSLLV(u32 word)
{	//20 (14)
    SetTextString(DisplayList, "dsllv\t%s, %s, %s\n", gpr_rn[getRD(word)],gpr_rn[getRT(word)],gpr_rn[getRS(word)]);
}

void DSRLV(u32 word)
{	//22 (16)
    SetTextString(DisplayList, "dsrlv\t%s, %s, %s\n", gpr_rn[getRD(word)],gpr_rn[getRT(word)],gpr_rn[getRS(word)]);
}

void DSRAV(u32 word)
{	//23 (17)
    SetTextString(DisplayList, "dsrav\t%s, %s, %s\n", gpr_rn[getRD(word)],gpr_rn[getRT(word)],gpr_rn[getRS(word)]);
}

void MULT(u32 word)
{		//24 (18)
    SetTextString(DisplayList, "mult\t%s, %s\n", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void MULTU(u32 word)
{	//25 (19)
    SetTextString(DisplayList, "multu\t%s, %s\n", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void DIV(u32 word)
{		//26 (1A)
    SetTextString(DisplayList, "div\t%s, %s\n", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void DIVU(u32 word)
{		//27 (1B)
    SetTextString(DisplayList, "divu\t%s, %s\n", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void DMULT(u32 word)
{	//28 (1C)
    SetTextString(DisplayList, "dmult\t%s, %s\n", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void DMULTU(u32 word)
{	//29 (1D)
    SetTextString(DisplayList, "dmultu\t%s, %s\n", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void DDIV(u32 word)
{		//30 (1E)
    SetTextString(DisplayList, "ddiv\t%s, %s\n", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void DDIVU(u32 word)
{	//31 (1F)
    SetTextString(DisplayList, "ddivu\t%s, %s\n", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void ADD(u32 word)
{		//32 (20)
    SetTextString(DisplayList, "add\t%s, %s, %s\n", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void ADDU(u32 word)
{		//33 (21)
    SetTextString(DisplayList, "addu\t%s, %s, %s\n", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void SUB(u32 word)
{		//34 (22)
    SetTextString(DisplayList, "sub\t%s, %s, %s\n", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void SUBU(u32 word)
{		//35 (23)
    SetTextString(DisplayList, "subu\t%s, %s, %s\n", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void AND(u32 word)
{		//36 (24)
    SetTextString(DisplayList, "and\t%s, %s, %s\n", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void OR(u32 word)
{		//37 (25)
    SetTextString(DisplayList, "or\t%s, %s, %s\n", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void XOR(u32 word)
{		//38 (26)
    SetTextString(DisplayList, "xor\t%s, %s, %s\n", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void NOR(u32 word)
{		//39 (27)
    SetTextString(DisplayList, "nor\t%s, %s, %s\n", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void SLT(u32 word)
{		//42 (2A)
    SetTextString(DisplayList, "slt\t%s, %s, %s\n", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void SLTU(u32 word)
{		//43 (2B)
    SetTextString(DisplayList, "sltu\t%s, %s, %s\n", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void DADD(u32 word)
{		//44 (2C)
    SetTextString(DisplayList, "dadd\t%s, %s, %s\n", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void DADDU(u32 word)
{	//45 (2D)
    SetTextString(DisplayList, "daddu\t%s, %s, %s\n", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void DSUB(u32 word)
{		//46 (2E)
    SetTextString(DisplayList, "dsub\t%s, %s, %s\n", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void DSUBU(u32 word)
{	//47 (2F)
    SetTextString(DisplayList, "dsubu\t%s, %s, %s\n", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void TGE(u32 word)
{		//48 (30)
    SetTextString(DisplayList, "tge\t%s, %s\n", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void TGEU(u32 word)
{		//49 (31)
    SetTextString(DisplayList, "tgeu\t%s, %s\n", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void TLT(u32 word)
{		//50 (32)
    SetTextString(DisplayList, "tlt\t%s, %s\n", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void TLTU(u32 word)
{		//51 (33)
    SetTextString(DisplayList, "tltu\t%s, %s\n", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void TEQ(u32 word)
{		//52 (34)
    SetTextString(DisplayList, "teq\t%s, %s\n", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void TNE(u32 word)
{		//54 (36)
    SetTextString(DisplayList, "tne\t%s, %s\n", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void DSLL(u32 word)
{		//56 (38)
    SetTextString(DisplayList, "dsll\t%s, %s, %02X\n", gpr_rn[getRD(word)],gpr_rn[getRT(word)],getSA(word));
}

void DSRL(u32 word)
{		//58 (3A)
    SetTextString(DisplayList, "dsrl\t%s, %s, %02X\n", gpr_rn[getRD(word)],gpr_rn[getRT(word)],getSA(word));
}

void DSRA(u32 word)
{		//59 (3B)
    SetTextString(DisplayList, "dsra\t%s, %s, %02X\n", gpr_rn[getRD(word)],gpr_rn[getRT(word)],getSA(word));
}

void DSLL32(u32 word)
{	//60 (3C)
    SetTextString(DisplayList, "dsll32\t%s, %s, %02X\n", gpr_rn[getRD(word)],gpr_rn[getRT(word)],getSA(word));
}

void DSRL32(u32 word)
{	//62 (3E)
    SetTextString(DisplayList, "dsrl32\t%s, %s, %02X\n", gpr_rn[getRD(word)],gpr_rn[getRT(word)],getSA(word));
}

void DSRA32(u32 word)
{	//63 (3F)
    SetTextString(DisplayList, "dsra32\t%s, %s, %02X\n", gpr_rn[getRD(word)],gpr_rn[getRT(word)],getSA(word));
};
Handler SPECIAL_T[64] = {
SLL,	NONE,	SRL,	SRA,	SLLV,	NONE,	SRLV,	SRAV,
JR,	JALR,	NONE,	NONE,	SYSCALL,BREAK,	NONE,	SYNC,
MFHI,	MTHI,	MFLO,	MTLO,	DSLLV,	NONE,	DSRLV,	DSRAV,
MULT,	MULTU,	DIV,	DIVU,	DMULT,	DMULTU,	DDIV,	DDIVU,
ADD,	ADDU,	SUB,	SUBU,	AND,	OR,	XOR,	NOR,
NONE,	NONE,	SLT,	SLTU,	DADD,	DADDU,	DSUB,	DSUBU,
TGE,	TGEU,	TLT,	TLTU,	TEQ,	NONE,	TNE,	NONE,
DSLL,	NONE,	DSRL,	DSRA,	DSLL32,	NONE,	DSRL32,	DSRA32
};

//Main op types
void SPECIAL(u32 word)
{	//00(00)
    SPECIAL_T[word & 63](word);
}

void REGIMM(u32 word)
{	//01(01)
    REGIMM_T[getRT(word)](word);
}

void J(u32 word)
{		//02(02)
    SetTextString(DisplayList, "j\t0x0%08X\n", 0x80000000 | getTARGET(word));
}

void JAL(u32 word)
{		//03(03)
    SetTextString(DisplayList, "jal\t0x%08X\n", 0x80000000 | getTARGET(word));
}

void BEQ(u32 word)
{		//04(04)
    SetTextString(DisplayList, "beq\t%s, %s, 0x%08X\n", gpr_rn[getRS(word)], gpr_rn[getRT(word)], getOFFSET(word) + ((i<<2) | 0x80000000) + 4  );
}

void BNE(u32 word)
{		//05(05)
    SetTextString(DisplayList, "bne\t%s, %s, 0x%08X\n", gpr_rn[getRS(word)], gpr_rn[getRT(word)], getOFFSET(word) + ((i<<2) | 0x80000000) + 4 );
}

void BLEZ(u32 word)
{		//06(06)
    SetTextString(DisplayList, "blez\t%s, 0x%08X\n", gpr_rn[getRS(word)], getOFFSET(word) + ((i<<2) | 0x80000000) + 4);
}

void BGTZ(u32 word)
{		//07(07)
    SetTextString(DisplayList, "bgtz\t%s, 0x%08X\n", gpr_rn[getRS(word)], getOFFSET(word) + ((i<<2) | 0x80000000) + 4);
}

void ADDI(u32 word)
{		//08(08)
    SetTextString(DisplayList, "addi\t%s, %s, 0x%04X\n", gpr_rn[getRT(word)], gpr_rn[getRS(word)], getIMM(word));
}

void ADDIU(u32 word)
{	//09(09)
    SetTextString(DisplayList, "addiu\t%s, %s, 0x%04X\n", gpr_rn[getRT(word)], gpr_rn[getRS(word)], getIMM(word));
}

void SLTI(u32 word)
{		//10(0A)
    SetTextString(DisplayList, "slti\t%s, %s, 0x%04X\n", gpr_rn[getRT(word)], gpr_rn[getRS(word)], getIMM(word));
}

void SLTIU(u32 word)
{	//11(0B)
    SetTextString(DisplayList, "sltiu\t%s, %s, 0x%04X\n", gpr_rn[getRT(word)], gpr_rn[getRS(word)], getIMM(word));
}

void ANDI(u32 word)
{		//12(0C)
    SetTextString(DisplayList, "andi\t%s, %s, 0x%04X\n", gpr_rn[getRT(word)], gpr_rn[getRS(word)], getIMM(word));
}

void ORI(u32 word)
{		//13(0D)
    SetTextString(DisplayList, "ori\t%s, %s, 0x%04X\n", gpr_rn[getRT(word)], gpr_rn[getRS(word)], getIMM(word));
}

void XORI(u32 word)
{		//14(0E)
    SetTextString(DisplayList, "xori\t%s, %s, 0x%04X\n", gpr_rn[getRT(word)], gpr_rn[getRS(word)], getIMM(word));
}

void LUI(u32 word)
{		//15(0F)
    SetTextString(DisplayList, "lui\t%s, 0x%04X\n", gpr_rn[getRT(word)], getIMM(word));
}

void COP0(u32 word)
{		//16(10)
    COP0_T[getRS(word)](word);
}

void COP1(u32 word)
{		//17(11)
    COP1_T[getRS(word)](word);
}

void BEQL(u32 word)
{		//20(14)
    SetTextString(DisplayList, "beql\t%s, %s, 0x%08X\n", gpr_rn[getRS(word)], gpr_rn[getRT(word)], getOFFSET(word) + ((i<<2) | 0x80000000) + 4);
}

void BNEL(u32 word)
{		//21(15)
    SetTextString(DisplayList, "bnel\t%s, %s, 0x%08X\n", gpr_rn[getRS(word)], gpr_rn[getRT(word)], getOFFSET(word) + ((i<<2) | 0x80000000) + 4);
}

void BLEZL(u32 word)
{	//22(16)
    SetTextString(DisplayList, "blezl\t%s, 0x%08X\n", gpr_rn[getRS(word)], getOFFSET(word) + ((i<<2) | 0x80000000) + 4);
}

void BGTZL(u32 word)
{	//23(17)
    SetTextString(DisplayList, "bgtzl\t%s, 0x%08X\n", gpr_rn[getRS(word)], getOFFSET(word) + ((i<<2) | 0x80000000) + 4);
}

void DADDI(u32 word)
{	//24(18)
    SetTextString(DisplayList, "daddi\t%s, %s, 0x%04X\n", gpr_rn[getRT(word)], gpr_rn[getRS(word)], getIMM(word));
}

void DADDIU(u32 word)
{	//25(19)
    SetTextString(DisplayList, "daddiu\t%s, %s, 0x%04X\n", gpr_rn[getRT(word)], gpr_rn[getRS(word)], getIMM(word));
}

void LDL(u32 word)
{		//26(1A)
    SetTextString(DisplayList, "ldl\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LDR(u32 word)
{		//27(1B)
    SetTextString(DisplayList, "ldr\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LB(u32 word)
{		//32(20)
    SetTextString(DisplayList, "lb\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LH(u32 word)
{		//33(21)
    SetTextString(DisplayList, "lh\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LWL(u32 word)
{		//34(22)
    SetTextString(DisplayList, "lwl\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LW(u32 word)
{		//35(23)
    SetTextString(DisplayList, "lw\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LBU(u32 word)
{		//36(24)
    SetTextString(DisplayList, "lbu\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LHU(u32 word)
{		//37(25)
    SetTextString(DisplayList, "lhu\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LWR(u32 word)
{		//38(26)
    SetTextString(DisplayList, "lwr\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LWU(u32 word)
{		//39(27)
    SetTextString(DisplayList, "lwu\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SB(u32 word)
{		//40(28)
    SetTextString(DisplayList, "sb\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SH(u32 word)
{		//41(29)
    SetTextString(DisplayList, "sh\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SWL(u32 word)
{		//42(2A)
    SetTextString(DisplayList, "swl\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SW(u32 word)
{		//43(2B)
    SetTextString(DisplayList, "sw\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SDL(u32 word)
{		//44(2C)
    SetTextString(DisplayList, "sdl\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SDR(u32 word)
{		//45(2D)
    SetTextString(DisplayList, "sdr\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SWR(u32 word)
{		//46(2E)
    SetTextString(DisplayList, "swr\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void CACHE(u32 word)
{	//47(2F)
    SetTextString(DisplayList, "cache\t0x%02X, 0x%04X(%s)\n", getRT(word), getIMM(word), gpr_rn[getBASE(word)]);
}

void LL(u32 word)
{		//48(30)
    SetTextString(DisplayList, "ll\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LWC1(u32 word)
{		//49(31)
    SetTextString(DisplayList, "lwc1\t%s, 0x%04X(%s)\n", fpr_rn[getFS(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LLD(u32 word)
{		//52(34)
    SetTextString(DisplayList, "lld\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LDC1(u32 word)
{		//53(35)
    SetTextString(DisplayList, "ldc1\t%s, 0x%04X(%s)\n", gpr_rn[getFT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LDC2(u32 word)
{		//54(36)
    SetTextString(DisplayList, "ldc2\t%s, 0x%04X(%s)\n", gpr_rn[getFT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LD(u32 word)
{		//55(37)
    SetTextString(DisplayList, "ld\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SC(u32 word)
{		//56(38)
    SetTextString(DisplayList, "sc\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SWC1(u32 word)
{		//57(39)
    SetTextString(DisplayList, "swc1\t%s, 0x%04X(%s)\n", gpr_rn[getFT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SCD(u32 word)
{		//60(3C)
    SetTextString(DisplayList, "sdc\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SDC1(u32 word)
{		//61(3D)
    SetTextString(DisplayList, "sdc1\t%s, 0x%04X(%s)\n", gpr_rn[getFT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SDC2(u32 word)
{		//62(3E)
    SetTextString(DisplayList, "sdc2\t%s, 0x%04X(%s)\n", gpr_rn[getFT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SD(u32 word)
{		//63(3F)
    SetTextString(DisplayList, "sd\t%s, 0x%04X(%s)\n", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

Handler MAIN_T[64] = {
SPECIAL,REGIMM,J,	JAL,	BEQ,	BNE,	BLEZ,	BGTZ,
ADDI,	ADDIU,	SLTI,	SLTIU,	ANDI,	ORI,	XORI,	LUI,
COP0,	COP1,	NONE,	NONE,	BEQL,	BNEL,	BLEZL,	BGTZL,
DADDI,	DADDIU,	LDL,	LDR,	NONE,	NONE,	NONE,	NONE,
LB,	LH,	LWL,	LW,	LBU,	LHU,	LWR,	LWU,
SB,	SH,	SWL,	SW,	SDL,	SDR,	SWR,	CACHE,
LL,	LWC1,	NONE,	NONE,	LLD,	LDC1,	LDC2,	LD,
SC,	SWC1,	NONE,	NONE,	SCD,	SDC1,	SDC2,	SD	
};

void * getOP(u32 word)
{
    MAIN_T[ word >> 26 ](word);
}

void _start(u32 a0)
{
    u32 (*words)[] = (void*)(0x80000000);
    DisplayList = a0;
    SetTextXY(a0,0,7);
    i = (pc&0x00FFFFFF)>>2;
    int end = i+8;
    for (i=i;i<end;i++)
    {
        SetTextRGBA(a0, 255, 127, 127, 255);
        SetTextString(a0, "8%07X ",i<<2);
        SetTextRGBA(a0, 255, 255, 255, 255);
        getOP( (*words)[i] );
    }
    switch (input1->pad)
    {
    case BUTTON_DPAD_U:
        if (pc>0x80000000)
            pc-=inc<<2;
        break;
    case BUTTON_DPAD_D:
        if (pc<0x80600000)
            pc+=inc<<2;
        break;
    case BUTTON_DPAD_L:
        if (inc < 0x1000)
            inc<<=1;
        else inc=1;
        break;
    }
    SetTextRGBA(a0, 127, 255, 127, 255);
    SetTextString(a0, "PC inc: %08X", inc<<2);
}

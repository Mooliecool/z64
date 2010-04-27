/*
 mipsdis.c
 redistribute and modify at will, my name is spinout, keep it here.
*/

#include <stdio.h>
#include <stdlib.h>
#include "../mipsdis/r3400i.h"
#include "adis.h"

unsigned int pc = 0x80000000;
char dis_op[100];
int map_branches;
int branches[1000];
int noBranches=0;
int EndOfFunction=-1;
unsigned local_start;
unsigned local_end;
int jaltaken=0;
int html = 0;
const char * actTypes[12]={
    "Unknown",
    "Prop",
    "Unknown",
    "Bomb",
    "NPC",
    "Enemy",
    "Prop",
    "Item/Action",
    "Miscellaneous",
    "Boss",
    "Unknown",
    "Door/Chest"
};

typedef union {
    u8 ubyte[1048576];
    s8 sbyte[1048576];
    u16 uhalf[524288];
    s16 shalf[524288];
    u32 uword[262144];
    s32 sword[262144];
    f32 sfloat[262144];
} memoryUnion;

#ifdef OOTDEBUG
int target_count = 11;
#else
int target_count = 0;
#endif

char * mapped_functions;

function functions[256] = {
#ifdef OOTDEBUG
	{0x80002130,	"DebugMessage"	},
	{0x800FB3AC,	"SetTextRGBA"	},
	{0x800FB41C,	"SetTextXY"	},
	{0x800FBCB4,	"SetTextString"	},
	{0x80031F50,	"ActorSpawn"	},
	{0x801031E0,	"sqrtf"		},
	{0x801067E0,	"absf"		},
	{0x80104610,	"cosf"		},
	{0x80100290,	"sinf"		},
	{0x80104780,	"coss"		},
	{0x80100450,	"sins"		}
#endif
};
//memoryUnion memory;

unsigned gpr_regs[32]={
0,	0, 	0,	0,	0x802245B0,	0x80212020,	0,	0,
0,	0, 	0,	0,	0,	0, 	0,	0,
0,	0, 	0,	0,	0,	0, 	0,	0,
0,	0, 	0,	0,	0,	0x80160000, 	0,	0,
};

const char * gpr_rn[32]={
"$zero",	"at",	"v0",	"v1",	"a0",	"a1",	"a2",	"a3",
"t0",	"t1",	"t2",	"t3",	"t4",	"t5",	"t6",	"t7",
"s0",	"s1",	"s2",	"s3",	"s4",	"s5",	"s6",	"s7",
"t8",	"t9",	"k0",	"k1",	"gp",	"$sp",	"s8",	"$ra"
};

const char * cop_rn[32]={
"Index",	"Random",	"EntryLo0",	"EntryLo1",	"Context",	"PageMask",	"Wired",	"Reserved",
"BadVAddr",	"Count",	"EntryHi",	"Compare",	"Status",	"Cause",	"Epc",		"PRevID",
"Config",	"LLAddr",	"WatchLo",	"WatchHi",	"XContext",	"Reserved",	"Reserved",	"Reserved",
"Reserved",	"Reserved",	"PErr",		"CacheErr",	"TagLo",	"TagHi",	"ErrorEpc",	"Reserved"
};

const char * fpr_rn[32]={
"$f0",	"$f1",	"$f2",	"$f3",	"$f4",	"$f5",	"$f6",	"$f7",
"$f8",	"$f9",	"$f10",	"$f11",	"$f12",	"$f13",	"$f14",	"$f15",
"$f16",	"$f17",	"$f18",	"$f19",	"$f20",	"$f21",	"$f22",	"$f23",
"$f24",	"$f25",	"$f26",	"$f27",	"$f28",	"$f29",	"$f30",	"$f31"
};

void reset_gpr_regs(void)
{
    int i;
    for (i=0;i<32;i++)
    {
        gpr_regs[i]=0;
    }
    gpr_regs[4] = 0x802245B0;
    gpr_regs[5] = 0x80212020;
    gpr_regs[29] = 0x80160000;
}

void reset_gpr_regs_soft(void) /* for jals */
{
    gpr_regs[1] = 0;
    gpr_regs[2] = 0;
    gpr_regs[3] = 0;
    gpr_regs[4] = 0;
    gpr_regs[5] = 0;
    gpr_regs[6] = 0;
    gpr_regs[7] = 0;
    gpr_regs[8] = 0;
    gpr_regs[9] = 0;
    gpr_regs[10] = 0;
    gpr_regs[11] = 0;
    gpr_regs[12] = 0;
    gpr_regs[13] = 0;
    gpr_regs[14] = 0;
    gpr_regs[15] = 0;
    gpr_regs[24] = 0;
    gpr_regs[25] = 0;
    
}

int target2label(int addr)
{
    int i;
    addr |= 0x80000000;
    for (i=0; i<target_count; i++)
    {
        if (functions[i].addr == addr)
            return i;
    }
    return -1;
}

int add_function(int addr, char * name)
{
    if(target2label(addr)>=0) /* Function already mapped? */
        return 0;
    char * name_ = (char*)malloc(32);
    int i;
    
    
    for (i=0; i<32;i++)name_[i]=name[i];
    
    functions[target_count].name = name_;
    functions[target_count].addr = addr|0x80000000;
    
    target_count+=1;
    return target_count-1;
}

int isTarget(int pc)
{
    int i;
    for (i=0; i<=noBranches; i++)
    {        
        if (pc==branches[i])
            return i;
    }
    return -1;
}

char label[16];

char * branch2label(int addr)
{
    
    if (map_branches)
    {
        noBranches++;
        branches[noBranches]=addr;
        sprintf(label, "0x%08X", addr);
    }
    else
    {
        int lbl_no = isTarget(addr);
        if (html)
            sprintf(label, "<a href=\"#$lbl_%i\">$lbl_%i</a>", lbl_no, lbl_no);
        else
            sprintf(label, "$lbl_%i", lbl_no);
    }
    return label;
}

void COP1_NONE(unsigned long word)
{
    sprintf(dis_op, "(Invalid COP1: %08X)", (unsigned int)word);
}

void TLB_NONE(unsigned long word)
{
    sprintf(dis_op, "(Invalid TLB: %08X)", (unsigned int)word);
}

void COP0_NONE(unsigned long word)
{
    sprintf(dis_op, "(Invalid COP0: %08X)", (unsigned int)word);
}

void NONE(unsigned long word)
{
    sprintf(dis_op, "(Invalid opcode: %08X)", (unsigned int)word);
}
/*
 cop1
 MIPS R3400i Co-processor 1 (FPU processor)
*/

//COP1.L
void CVT_S_L(unsigned long word)
{
    sprintf(dis_op, "cvt.s.l\t%s, %s", fpr_rn[getFD(word)],fpr_rn[getFS(word)]);
}

void CVT_D_L(unsigned long word)
{
    sprintf(dis_op, "cvt.d.l\t%s, %s", fpr_rn[getFD(word)],fpr_rn[getFS(word)]);
}

//COP1.W
void CVT_S_W(unsigned long word)
{
    sprintf(dis_op, "cvt.s.w\t%s, %s", fpr_rn[getFD(word)],fpr_rn[getFS(word)]);
}

void CVT_D_W(unsigned long word)
{
    sprintf(dis_op, "cvt.d.w\t%s, %s", fpr_rn[getFD(word)],fpr_rn[getFS(word)]);
}

//COP1.S
void ADD_S(unsigned long word)
{	//00 (00)
    sprintf(dis_op, "add.s\t%s, %s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)], fpr_rn[getFT(word)]);
}

void SUB_S(unsigned long word)
{	//01 (01)
    sprintf(dis_op, "sub.s\t%s, %s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)], fpr_rn[getFT(word)]);
}

void MUL_S(unsigned long word)
{	//02 (02)
    sprintf(dis_op, "mul.s\t%s, %s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)], fpr_rn[getFT(word)]);
}

void DIV_S(unsigned long word)
{	//03 (03)
    sprintf(dis_op, "div.s\t%s, %s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)], fpr_rn[getFT(word)]);
}

void SQRT_S(unsigned long word)
{	//04 (04)
    sprintf(dis_op, "sqrt.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void ABS_S(unsigned long word)
{	//05 (05)
    sprintf(dis_op, "abs.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void MOV_S(unsigned long word)
{	//06 (06)
    sprintf(dis_op, "mov.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void NEG_S(unsigned long word)
{	//07 (07)
    sprintf(dis_op, "neg.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void ROUND_L_S(unsigned long word)
{	//08 (08)
    sprintf(dis_op, "round.l.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void TRUNC_L_S(unsigned long word)
{	//09 (09)
    sprintf(dis_op, "trunc.l.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void CEIL_L_S(unsigned long word)
{	//10 (0A)
    sprintf(dis_op, "ceil.l.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void FLOOR_L_S(unsigned long word)
{	//11 (0B)
    sprintf(dis_op, "floor.l.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void ROUND_W_S(unsigned long word)
{	//12 (0C)
    sprintf(dis_op, "round.w.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void TRUNC_W_S(unsigned long word)
{	//13 (0D)
    sprintf(dis_op, "trunc.w.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void CEIL_W_S(unsigned long word)
{	//14 (0E)
    sprintf(dis_op, "ceil.w.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void FLOOR_W_S(unsigned long word)
{	//15 (0F)
    sprintf(dis_op, "floor.w.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void CVT_D_S(unsigned long word)
{	//33 (21)
    sprintf(dis_op, "cvt.d.s\t%s, %s", fpr_rn[getFD(word)],fpr_rn[getFS(word)]);
}

void CVT_W_S(unsigned long word)
{	//36 (24)
    sprintf(dis_op, "cvt.w.s\t%s, %s", fpr_rn[getFD(word)],fpr_rn[getFS(word)]);
}

void CVT_L_S(unsigned long word)
{	//37 (25)
    sprintf(dis_op, "cvt.l.s\t%s, %s", fpr_rn[getFD(word)],fpr_rn[getFS(word)]);
}

void C_F_S(unsigned long word)
{	//48 (30)
    sprintf(dis_op, "c.f.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_UN_S(unsigned long word)
{	//49 (31)
    sprintf(dis_op, "c.un.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_EQ_S(unsigned long word)
{	//50 (32)
    sprintf(dis_op, "c.eq.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_UEQ_S(unsigned long word)
{	//51 (33)
    sprintf(dis_op, "c.ueq.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_OLT_S(unsigned long word)
{	//52 (34)
    sprintf(dis_op, "c.olt.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_ULT_S(unsigned long word)
{	//53 (35)
    sprintf(dis_op, "c.ult.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_OLE_S(unsigned long word)
{	//54 (36)
    sprintf(dis_op, "c.ole.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_ULE_S(unsigned long word)
{	//55 (37)
    sprintf(dis_op, "c.ule.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_SF_S(unsigned long word)
{	//56 (38)
    sprintf(dis_op, "c.sf.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_NGLE_S(unsigned long word)
{	//57 (39)
    sprintf(dis_op, "c.ngle.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_SEQ_S(unsigned long word)
{	//58 (3A)
    sprintf(dis_op, "c.seq.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_NGL_S(unsigned long word)
{	//59 (3B)
    sprintf(dis_op, "c.ngl.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_LT_S(unsigned long word)
{	//60 (3C)
    sprintf(dis_op, "c.lt.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_NGE_S(unsigned long word)
{	//61 (3D)
    sprintf(dis_op, "c.nge.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_LE_S(unsigned long word)
{	//62 (3E)
    sprintf(dis_op, "c.le.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_NGT_S(unsigned long word)
{	//63 (3F)
    sprintf(dis_op, "c.ngt.s\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
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
void ADD_D(unsigned long word)
{	//00 (00)
    sprintf(dis_op, "add.d\t%s, %s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)], fpr_rn[getFT(word)]);
}

void SUB_D(unsigned long word)
{	//01 (01)
    sprintf(dis_op, "sub.d\t%s, %s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)], fpr_rn[getFT(word)]);
}

void MUL_D(unsigned long word)
{	//02 (02)
    sprintf(dis_op, "mul.d\t%s, %s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)], fpr_rn[getFT(word)]);
}

void DIV_D(unsigned long word)
{	//03 (03)
    sprintf(dis_op, "add.d\t%s, %s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)], fpr_rn[getFT(word)]);
}

void SQRT_D(unsigned long word)
{	//04 (04)
    sprintf(dis_op, "sqrt.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void ABS_D(unsigned long word)
{	//05 (05)
    sprintf(dis_op, "abs.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void MOV_D(unsigned long word)
{	//06 (06)
    sprintf(dis_op, "mov.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void NEG_D(unsigned long word)
{	//07 (07)
    sprintf(dis_op, "neg.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void ROUND_L_D(unsigned long word)
{	//08 (08)
    sprintf(dis_op, "round.l.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void TRUNC_L_D(unsigned long word)
{	//09 (09)
    sprintf(dis_op, "trunc.l.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void CEIL_L_D(unsigned long word)
{	//10 (0A)
    sprintf(dis_op, "ceil.l.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void FLOOR_L_D(unsigned long word)
{	//11 (0B)
    sprintf(dis_op, "floor.l.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void ROUND_W_D(unsigned long word)
{	//12 (0C)
    sprintf(dis_op, "round.w.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void TRUNC_W_D(unsigned long word)
{	//13 (0D)
    sprintf(dis_op, "trunc.w.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void CEIL_W_D(unsigned long word)
{	//14 (0E)
    sprintf(dis_op, "ceil.w.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void FLOOR_W_D(unsigned long word)
{	//15 (0F)
    sprintf(dis_op, "floor.w.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void CVT_S_D(unsigned long word)
{	//32 (20)
    sprintf(dis_op, "cvt.s.d\t%s, %s", fpr_rn[getFD(word)],fpr_rn[getFS(word)]);
}

void CVT_W_D(unsigned long word)
{	//36 (24)
    sprintf(dis_op, "cvt.w.d\t%s, %s", fpr_rn[getFD(word)],fpr_rn[getFS(word)]);
}

void CVT_L_D(unsigned long word)
{	//37 (25)
    sprintf(dis_op, "cvt.l.d\t%s, %s", fpr_rn[getFD(word)],fpr_rn[getFS(word)]);
}

void C_F_D(unsigned long word)
{	//48 (30)
    sprintf(dis_op, "c.f.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_UN_D(unsigned long word)
{	//49 (31)
    sprintf(dis_op, "c.un.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_EQ_D(unsigned long word)
{	//50 (32)
    sprintf(dis_op, "c.eq.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_UEQ_D(unsigned long word)
{	//51 (33)
    sprintf(dis_op, "c.ueq.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_OLT_D(unsigned long word)
{	//52 (34)
    sprintf(dis_op, "c.olt.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_ULT_D(unsigned long word)
{	//53 (35)
    sprintf(dis_op, "c.ult.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_OLE_D(unsigned long word)
{	//54 (36)
    sprintf(dis_op, "c.ole.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_ULE_D(unsigned long word)
{	//55 (37)
    sprintf(dis_op, "c.ule.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_SF_D(unsigned long word)
{	//56 (38)
    sprintf(dis_op, "c.df.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_NGLE_D(unsigned long word)
{	//57 (39)
    sprintf(dis_op, "c.ngle.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_SEQ_D(unsigned long word)
{	//58 (3A)
    sprintf(dis_op, "c.deq.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_NGL_D(unsigned long word)
{	//59 (3B)
    sprintf(dis_op, "c.ngl.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_LT_D(unsigned long word)
{	//60 (3C)
    sprintf(dis_op, "c.lt.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_NGE_D(unsigned long word)
{	//61 (3D)
    sprintf(dis_op, "c.nge.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_LE_D(unsigned long word)
{	//62 (3E)
    sprintf(dis_op, "c.le.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
}

void C_NGT_D(unsigned long word)
{	//63 (3F)
    sprintf(dis_op, "c.ngt.d\t%s, %s", fpr_rn[getFD(word)], fpr_rn[getFS(word)]);
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
void BC1F(unsigned long word)
{
    sprintf(dis_op, "bc1f\t%s", branch2label(getOFFSET(word) + (pc | 0x80000000) + 4));
}

void BC1T(unsigned long word)
{
    sprintf(dis_op, "bc1t\t%s", branch2label(getOFFSET(word) + (pc | 0x80000000) + 4));
}

void BC1FL(unsigned long word)
{
    sprintf(dis_op, "bc1fl\t%s", branch2label(getOFFSET(word) + (pc | 0x80000000) + 4));
}

void BC1TL(unsigned long word)
{
    sprintf(dis_op, "bc1tl\t%s", branch2label(getOFFSET(word) + (pc | 0x80000000) + 4));
}

Handler BC_T[4] = {BC1F, BC1T, BC1FL, BC1TL};

//COP1 op types

void MFC1(unsigned long word)
{		//00 (00)
    sprintf(dis_op, "mfc1\t%s, %s", gpr_rn[getRT(word)], fpr_rn[getFD(word)]);
}

void DMFC1(unsigned long word)
{	//01 (01)
    sprintf(dis_op, "dmfc1\t%s, %s", gpr_rn[getRT(word)], fpr_rn[getFD(word)]);
}

void CFC1(unsigned long word)
{		//02 (02)
    sprintf(dis_op, "cfc1\t%s, %s", gpr_rn[getRT(word)], fpr_rn[getFD(word)]);
}

void MTC1(unsigned long word)
{		//04 (04)
    float * value = &gpr_regs[getRT(word)];
    sprintf(dis_op, "mtc1\t%s, %s\t\t\t/* %s = %f */", gpr_rn[getRT(word)], fpr_rn[getFD(word)], fpr_rn[getFD(word)], *value );
}

void DMTC1(unsigned long word)
{	//05 (05)
    sprintf(dis_op, "dmtc1\t%s, %s", gpr_rn[getRT(word)], fpr_rn[getFD(word)]);
}

void CTC1(unsigned long word)
{		//06 (06)
    sprintf(dis_op, "ctc1\t%s, %s", gpr_rn[getRT(word)], fpr_rn[getFD(word)]);
}

void BC(unsigned long word)
{		//08 (08)
    BC_T[(word >> 16) & 3](word);
}

void S(unsigned long word)
{		//16 (10)
    S_T[word & 63](word);
}

void D(unsigned long word)
{		//17 (11)
    D_T[word & 63](word);
}

void W(unsigned long word)
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

void L(unsigned long word)
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


void TLBR(unsigned long word)
{
    sprintf(dis_op, "tlbr");
}

void TLBWI(unsigned long word)
{
    sprintf(dis_op, "tlbwi");
}

void TLBWR(unsigned long word)
{
    sprintf(dis_op, "tlbwr");
}

void TLBP(unsigned long word)
{
    sprintf(dis_op, "tlbp");
}

void ERET(unsigned long word)
{
    sprintf(dis_op, "eret");
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

void MFC0(unsigned long word)
{		/* 00 */
    sprintf(dis_op, "mfc0\t%s, %s", gpr_rn[getRT(word)], cop_rn[getFS(word)]);
}

void MTC0(unsigned long word)
{		/* 04 */
    sprintf(dis_op, "mtc0\t%s, %s", gpr_rn[getRT(word)], cop_rn[getFS(word)]);
}

void TLB(unsigned long word)
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
void BLTZ(unsigned long word)
{
    sprintf(dis_op, "bltz\t%s, %s", gpr_rn[getRS(word)], branch2label(getOFFSET(word) + (pc | 0x80000000) + 4));
}

void BGEZ(unsigned long word)
{
    sprintf(dis_op, "bgez\t%s, %s", gpr_rn[getRS(word)], branch2label(getOFFSET(word) + (pc | 0x80000000) + 4));
}

void BLTZL(unsigned long word)
{
    sprintf(dis_op, "bltzl\t%s, %s", gpr_rn[getRS(word)], branch2label(getOFFSET(word) + (pc | 0x80000000) + 4));
}

void BGEZL(unsigned long word)
{
    sprintf(dis_op, "bgezl\t%s, %s", gpr_rn[getRS(word)], branch2label(getOFFSET(word) + (pc | 0x80000000) + 4));
}

void TGEI(unsigned long word)
{
    sprintf(dis_op, "tgei\t%s, 0x%08X", gpr_rn[getRS(word)], getIMM(word));
}

void TGEIU(unsigned long word)
{
    sprintf(dis_op, "tgeiu\t%s 0x%08X", gpr_rn[getRS(word)], getIMM(word));
}

void TLTI(unsigned long word)
{
    sprintf(dis_op, "tlti\t%s, 0x%08X", gpr_rn[getRS(word)], getIMM(word));
}

void TLTIU(unsigned long word)
{
    sprintf(dis_op, "tltiu\t%s, 0x%08X", gpr_rn[getRS(word)], getIMM(word));
}

void TEQI(unsigned long word)
{
    sprintf(dis_op, "tqei\t%s, 0x%08X", gpr_rn[getRS(word)], getIMM(word));
}

void TNEI(unsigned long word)
{
    sprintf(dis_op, "tnei\t%s, 0x%08X", gpr_rn[getRS(word)], getIMM(word));
}

void BLTZAL(unsigned long word)
{
    sprintf(dis_op, "bltzal\t%s, %s", gpr_rn[getRS(word)], branch2label(getOFFSET(word) + (pc | 0x80000000) + 4));
}

void BGEZAL(unsigned long word)
{
    sprintf(dis_op, "bgezal\t%s, %s", gpr_rn[getRS(word)], branch2label(getOFFSET(word) + (pc | 0x80000000) + 4));
}

void BLTZALL(unsigned long word)
{
    sprintf(dis_op, "bltzall\t%s, %s", gpr_rn[getRS(word)], branch2label(getOFFSET(word) + (pc | 0x80000000) + 4));
}

void BGEZALL(unsigned long word)
{
    sprintf(dis_op, "bgezall\t%s, %s", gpr_rn[getRS(word)], branch2label(getOFFSET(word) + (pc | 0x80000000) + 4));
}

Handler REGIMM_T[32] = {
BLTZ,	BGEZ,	BLTZL,	BGEZL,	NONE,	NONE,	NONE,	NONE,
TGEI,	TGEIU,	TLTI,	TLTIU,	TEQI,	NONE,	TNEI,	NONE,
BLTZAL,BGEZAL,	BLTZALL,BGEZALL,NONE,	NONE,	NONE,	NONE,
NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE,	NONE
};

//SPECIAL op types
void SLL(unsigned long word)
{		//00 (00)
    if (word)
        sprintf(dis_op, "sll\t%s, %s, %02X", gpr_rn[getRD(word)], gpr_rn[getRT(word)], getSA(word) );
    else
        sprintf(dis_op, "nop");
}
void SRL(unsigned long word)
{		//02 (02)
    sprintf(dis_op, "srl\t%s, %s, %02X", gpr_rn[getRD(word)],gpr_rn[getRT(word)],getSA(word));
}

void SRA(unsigned long word)
{		//03 (03)
    sprintf(dis_op, "sra\t%s, %s, %02X", gpr_rn[getRD(word)],gpr_rn[getRT(word)],getSA(word));
}

void SLLV(unsigned long word)
{		//04 (04)
    sprintf(dis_op, "sllv\t%s, %s, %s", gpr_rn[getRD(word)],gpr_rn[getRT(word)],gpr_rn[getRS(word)]);
}

void SRLV(unsigned long word)
{		//06 (06)
    sprintf(dis_op, "srlv\t%s, %s, %s", gpr_rn[getRD(word)],gpr_rn[getRT(word)],gpr_rn[getRS(word)]);
}

void SRAV(unsigned long word)
{		//07 (07)
    sprintf(dis_op, "srav\t%s, %s, %s", gpr_rn[getRD(word)],gpr_rn[getRT(word)],gpr_rn[getRS(word)]);
}

void JR(unsigned long word)
{		//08 (08)
    sprintf(dis_op, "jr\t%s", gpr_rn[getRS(word)]);
    if (getRS(word) == 31)
        EndOfFunction=pc+4;
}

void JALR(unsigned long word)
{		//09 (09)
    sprintf(dis_op, "jalr\t%s, %s", gpr_rn[getRS(word)],gpr_rn[getRD(word)]);
    jaltaken=2;
}

void SYSCALL(unsigned long word)
{	//12 (0C)
    sprintf(dis_op, "syscall\t0x%04X", getIMM(word));
}

void BREAK(unsigned long word)
{	//13 (0D)
    sprintf(dis_op, "break\t0x%04X", getIMM(word));
}

void SYNC(unsigned long word)
{		//15 (0F)
    sprintf(dis_op, "sync");
}

void MFHI(unsigned long word)
{		//16 (10)
    sprintf(dis_op, "mfhi\t%s", gpr_rn[getRD(word)]);
}

void MTHI(unsigned long word)
{		//17 (11)
    sprintf(dis_op, "mthi\t%s", gpr_rn[getRD(word)]);
}

void MFLO(unsigned long word)
{		//18 (12)
    sprintf(dis_op, "mflo\t%s", gpr_rn[getRD(word)]);
}

void MTLO(unsigned long word)
{		//19 (13)
    sprintf(dis_op, "mtlo\t%s", gpr_rn[getRD(word)]);
}

void DSLLV(unsigned long word)
{	//20 (14)
    sprintf(dis_op, "dsllv\t%s, %s, %s", gpr_rn[getRD(word)],gpr_rn[getRT(word)],gpr_rn[getRS(word)]);
}

void DSRLV(unsigned long word)
{	//22 (16)
    sprintf(dis_op, "dsrlv\t%s, %s, %s", gpr_rn[getRD(word)],gpr_rn[getRT(word)],gpr_rn[getRS(word)]);
}

void DSRAV(unsigned long word)
{	//23 (17)
    sprintf(dis_op, "dsrav\t%s, %s, %s", gpr_rn[getRD(word)],gpr_rn[getRT(word)],gpr_rn[getRS(word)]);
}

void MULT(unsigned long word)
{		//24 (18)
    sprintf(dis_op, "mult\t%s, %s", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void MULTU(unsigned long word)
{	//25 (19)
    sprintf(dis_op, "multu\t%s, %s", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void DIV(unsigned long word)
{		//26 (1A)
    sprintf(dis_op, "div\t%s, %s", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void DIVU(unsigned long word)
{		//27 (1B)
    sprintf(dis_op, "divu\t%s, %s", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void DMULT(unsigned long word)
{	//28 (1C)
    sprintf(dis_op, "dmult\t%s, %s", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void DMULTU(unsigned long word)
{	//29 (1D)
    sprintf(dis_op, "dmultu\t%s, %s", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void DDIV(unsigned long word)
{		//30 (1E)
    sprintf(dis_op, "ddiv\t%s, %s", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void DDIVU(unsigned long word)
{	//31 (1F)
    sprintf(dis_op, "ddivu\t%s, %s", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void ADD(unsigned long word)
{		//32 (20)
    sprintf(dis_op, "add\t%s, %s, %s", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void ADDU(unsigned long word)
{		//33 (21)
    sprintf(dis_op, "addu\t%s, %s, %s", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void SUB(unsigned long word)
{		//34 (22)
    sprintf(dis_op, "sub\t%s, %s, %s", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void SUBU(unsigned long word)
{		//35 (23)
    sprintf(dis_op, "subu\t%s, %s, %s", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void AND(unsigned long word)
{		//36 (24)
    sprintf(dis_op, "and\t%s, %s, %s", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void OR(unsigned long word)
{		//37 (25)
    gpr_regs[getRD(word)]=gpr_regs[getRS(word)] | gpr_regs[getRT(word)];
    sprintf(dis_op, "or\t%s, %s, %s\t\t/*%s = %08X*/", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)], gpr_rn[getRD(word)],gpr_regs[getRD(word)]);
    
}

void XOR(unsigned long word)
{		//38 (26)
    sprintf(dis_op, "xor\t%s, %s, %s", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void NOR(unsigned long word)
{		//39 (27)
    sprintf(dis_op, "nor\t%s, %s, %s", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void SLT(unsigned long word)
{		//42 (2A)
    sprintf(dis_op, "slt\t%s, %s, %s", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void SLTU(unsigned long word)
{		//43 (2B)
    sprintf(dis_op, "sltu\t%s, %s, %s", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void DADD(unsigned long word)
{		//44 (2C)
    sprintf(dis_op, "dadd\t%s, %s, %s", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void DADDU(unsigned long word)
{	//45 (2D)
    sprintf(dis_op, "daddu\t%s, %s, %s", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void DSUB(unsigned long word)
{		//46 (2E)
    sprintf(dis_op, "dsub\t%s, %s, %s", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void DSUBU(unsigned long word)
{	//47 (2F)
    sprintf(dis_op, "dsubu\t%s, %s, %s", gpr_rn[getRD(word)],gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void TGE(unsigned long word)
{		//48 (30)
    sprintf(dis_op, "tge\t%s, %s", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void TGEU(unsigned long word)
{		//49 (31)
    sprintf(dis_op, "tgeu\t%s, %s", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void TLT(unsigned long word)
{		//50 (32)
    sprintf(dis_op, "tlt\t%s, %s", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void TLTU(unsigned long word)
{		//51 (33)
    sprintf(dis_op, "tltu\t%s, %s", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void TEQ(unsigned long word)
{		//52 (34)
    sprintf(dis_op, "teq\t%s, %s", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void TNE(unsigned long word)
{		//54 (36)
    sprintf(dis_op, "tne\t%s, %s", gpr_rn[getRS(word)],gpr_rn[getRT(word)]);
}

void DSLL(unsigned long word)
{		//56 (38)
    sprintf(dis_op, "dsll\t%s, %s, %02X", gpr_rn[getRD(word)],gpr_rn[getRT(word)],getSA(word));
}

void DSRL(unsigned long word)
{		//58 (3A)
    sprintf(dis_op, "dsrl\t%s, %s, %02X", gpr_rn[getRD(word)],gpr_rn[getRT(word)],getSA(word));
}

void DSRA(unsigned long word)
{		//59 (3B)
    sprintf(dis_op, "dsra\t%s, %s, %02X", gpr_rn[getRD(word)],gpr_rn[getRT(word)],getSA(word));
}

void DSLL32(unsigned long word)
{	//60 (3C)
    sprintf(dis_op, "dsll32\t%s, %s, %02X", gpr_rn[getRD(word)],gpr_rn[getRT(word)],getSA(word));
}

void DSRL32(unsigned long word)
{	//62 (3E)
    sprintf(dis_op, "dsrl32\t%s, %s, %02X", gpr_rn[getRD(word)],gpr_rn[getRT(word)],getSA(word));
}

void DSRA32(unsigned long word)
{	//63 (3F)
    sprintf(dis_op, "dsra32\t%s, %s, %02X", gpr_rn[getRD(word)],gpr_rn[getRT(word)],getSA(word));
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
void SPECIAL(unsigned long word)
{	//00(00)
    SPECIAL_T[word & 63](word);
}

void REGIMM(unsigned long word)
{	//01(01)
    REGIMM_T[getRT(word)](word);
}

void J(unsigned long word)
{		//02(02)
    sprintf(dis_op, "j\t0x0%08X", 0x80000000 | getTARGET(word));
}

void JAL(unsigned long word)
{		//03(03)
    int target = getTARGET(word);
    int functionno = target2label(target);
    if(functionno >= 0)
    {
        sprintf(dis_op, "jal\t%s", functions[functionno].name);
    }
    else
        sprintf(dis_op, "jal\t0x%08X", target|0x80000000);
    
    jaltaken=2;
}

void BEQ(unsigned long word)
{		//04(04)
    sprintf(dis_op, "beq\t%s, %s, %s", gpr_rn[getRS(word)], gpr_rn[getRT(word)], branch2label(getOFFSET(word) + (pc | 0x80000000) + 4)  );
}

void BNE(unsigned long word)
{		//05(05)
    sprintf(dis_op, "bne\t%s, %s, %s", gpr_rn[getRS(word)], gpr_rn[getRT(word)], branch2label(getOFFSET(word) + (pc | 0x80000000) + 4) );
}

void BLEZ(unsigned long word)
{		//06(06)
    sprintf(dis_op, "blez\t%s, %s", gpr_rn[getRS(word)], branch2label(getOFFSET(word) + (pc | 0x80000000) + 4));
}

void BGTZ(unsigned long word)
{		//07(07)
    sprintf(dis_op, "bgtz\t%s, %s", gpr_rn[getRS(word)], branch2label(getOFFSET(word) + (pc | 0x80000000) + 4));
}

void ADDI(unsigned long word)
{		//08(08)
    gpr_regs[getRT(word)] = gpr_regs[getRS(word)] + getSIMM(word);
    sprintf(dis_op, "addi\t%s, %s, 0x%04X\t\t/*%s = %08X*/", gpr_rn[getRT(word)], gpr_rn[getRS(word)], getIMM(word), gpr_rn[getRT(word)], gpr_regs[getRT(word)]);
}

void ADDIU(unsigned long word)
{	//09(09)
    gpr_regs[getRT(word)] = gpr_regs[getRS(word)] + getSIMM(word);
    sprintf(dis_op, "addiu\t%s, %s, 0x%04X\t\t/*%s = %08X*/", gpr_rn[getRT(word)], gpr_rn[getRS(word)], getIMM(word), gpr_rn[getRT(word)], gpr_regs[getRT(word)]);
}

void SLTI(unsigned long word)
{		//10(0A)
    sprintf(dis_op, "slti\t%s, %s, 0x%04X", gpr_rn[getRT(word)], gpr_rn[getRS(word)], getIMM(word));
}

void SLTIU(unsigned long word)
{	//11(0B)
    sprintf(dis_op, "sltiu\t%s, %s, 0x%04X", gpr_rn[getRT(word)], gpr_rn[getRS(word)], getIMM(word));
}

void ANDI(unsigned long word)
{		//12(0C)
    gpr_regs[getRT(word)] = gpr_regs[getRS(word)] & getIMM(word);
    sprintf(dis_op, "andi\t%s, %s, 0x%04X\t\t/*%s = %08X*/", gpr_rn[getRT(word)], gpr_rn[getRS(word)], getIMM(word), gpr_rn[getRT(word)], gpr_regs[getRT(word)]);
}

void ORI(unsigned long word)
{		//13(0D)
    gpr_regs[getRT(word)] = gpr_regs[getRS(word)] | getIMM(word);
    sprintf(dis_op, "ori\t%s, %s, 0x%04X\t\t/*%s = %08X*/", gpr_rn[getRT(word)], gpr_rn[getRS(word)], getIMM(word), gpr_rn[getRT(word)], gpr_regs[getRT(word)]);
}

void XORI(unsigned long word)
{		//14(0E)
    gpr_regs[getRT(word)] = gpr_regs[getRS(word)] ^ getIMM(word);
    sprintf(dis_op, "xori\t%s, %s, 0x%04X\t\t/*%s = %08X*/", gpr_rn[getRT(word)], gpr_rn[getRS(word)], getIMM(word), gpr_rn[getRT(word)], gpr_regs[getRT(word)]);
}

void LUI(unsigned long word)
{		//15(0F)
    gpr_regs[getRT(word)] = getIMM(word)<<16;
    sprintf(dis_op, "lui\t%s, 0x%04X\t\t/*%s = %08X*/", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getRT(word)], gpr_regs[getRT(word)]);
}

void COP0(unsigned long word)
{		//16(10)
    COP0_T[getRS(word)](word);
}

void COP1(unsigned long word)
{		//17(11)
    COP1_T[getRS(word)](word);
}

void BEQL(unsigned long word)
{		//20(14)
    sprintf(dis_op, "beql\t%s, %s, %s", gpr_rn[getRS(word)], gpr_rn[getRT(word)], branch2label(getOFFSET(word) + (pc | 0x80000000) + 4));
}

void BNEL(unsigned long word)
{		//21(15)
    sprintf(dis_op, "bnel\t%s, %s, %s", gpr_rn[getRS(word)], gpr_rn[getRT(word)], branch2label(getOFFSET(word) + (pc | 0x80000000) + 4));
}

void BLEZL(unsigned long word)
{	//22(16)
    sprintf(dis_op, "blezl\t%s, %s", gpr_rn[getRS(word)], branch2label(getOFFSET(word) + (pc | 0x80000000) + 4));
}

void BGTZL(unsigned long word)
{	//23(17)
    sprintf(dis_op, "bgtzl\t%s, %s", gpr_rn[getRS(word)], branch2label(getOFFSET(word) + (pc | 0x80000000) + 4));
}

void DADDI(unsigned long word)
{	//24(18)
    sprintf(dis_op, "daddi\t%s, %s, 0x%04X", gpr_rn[getRT(word)], gpr_rn[getRS(word)], getIMM(word));
}

void DADDIU(unsigned long word)
{	//25(19)
    sprintf(dis_op, "daddiu\t%s, %s, 0x%04X", gpr_rn[getRT(word)], gpr_rn[getRS(word)], getIMM(word));
}

void LDL(unsigned long word)
{		//26(1A)
    sprintf(dis_op, "ldl\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LDR(unsigned long word)
{		//27(1B)
    sprintf(dis_op, "ldr\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LB(unsigned long word)
{		//32(20)
    //gpr_regs[getRT(word)] = memory.sbyte[getIMM(word)+gpr_regs[getBASE(word)]];
    sprintf(dis_op, "lb\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LH(unsigned long word)
{		//33(21)
    //gpr_regs[getRT(word)] = memory.shalf[(getIMM(word)+gpr_regs[getBASE(word)])>>1];
    sprintf(dis_op, "lh\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LWL(unsigned long word)
{		//34(22)
    sprintf(dis_op, "lwl\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LW(unsigned long word)
{		//35(23)
    //gpr_regs[getRT(word)] = memory.sword[(getIMM(word)+gpr_regs[getBASE(word)])>>2];
    sprintf(dis_op, "lw\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LBU(unsigned long word)
{		//36(24)
    //gpr_regs[getRT(word)] = memory.ubyte[getIMM(word)+gpr_regs[getBASE(word)]];
    sprintf(dis_op, "lbu\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LHU(unsigned long word)
{		//37(25)
    //gpr_regs[getRT(word)] = memory.uhalf[(getIMM(word)+gpr_regs[getBASE(word)])>>1];
    sprintf(dis_op, "lhu\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LWR(unsigned long word)
{		//38(26)
    sprintf(dis_op, "lwr\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LWU(unsigned long word)
{		//39(27)
    //gpr_regs[getRT(word)] = memory.uword[(getIMM(word)+gpr_regs[getBASE(word)])>>2];
    sprintf(dis_op, "lwu\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SB(unsigned long word)
{		//40(28)
    //memory.sbyte[getIMM(word)+gpr_regs[getBASE(word)]] = gpr_regs[getRT(word)];
    sprintf(dis_op, "sb\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SH(unsigned long word)
{		//41(29)
    //memory.shalf[(getIMM(word)+gpr_regs[getBASE(word)])>>2] = gpr_regs[getRT(word)];
    sprintf(dis_op, "sh\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SWL(unsigned long word)
{		//42(2A)
    sprintf(dis_op, "swl\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SW(unsigned long word)
{		//43(2B)
    sprintf(dis_op, "sw\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SDL(unsigned long word)
{		//44(2C)
    sprintf(dis_op, "sdl\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SDR(unsigned long word)
{		//45(2D)
    sprintf(dis_op, "sdr\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SWR(unsigned long word)
{		//46(2E)
    sprintf(dis_op, "swr\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void CACHE(unsigned long word)
{	//47(2F)
    sprintf(dis_op, "cache\t0x%02X, 0x%04X(%s)", getRT(word), getIMM(word), gpr_rn[getBASE(word)]);
}

void LL(unsigned long word)
{		//48(30)
    sprintf(dis_op, "ll\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LWC1(unsigned long word)
{		//49(31)
    sprintf(dis_op, "lwc1\t%s, 0x%04X(%s)", fpr_rn[getFS(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LLD(unsigned long word)
{		//52(34)
    sprintf(dis_op, "lld\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LDC1(unsigned long word)
{		//53(35)
    sprintf(dis_op, "ldc1\t%s, 0x%04X(%s)", gpr_rn[getFT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LDC2(unsigned long word)
{		//54(36)
    sprintf(dis_op, "ldc2\t%s, 0x%04X(%s)", gpr_rn[getFT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void LD(unsigned long word)
{		//55(37)
    sprintf(dis_op, "ld\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SC(unsigned long word)
{		//56(38)
    sprintf(dis_op, "sc\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SWC1(unsigned long word)
{		//57(39)
    sprintf(dis_op, "swc1\t%s, 0x%04X(%s)", gpr_rn[getFT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SCD(unsigned long word)
{		//60(3C)
    sprintf(dis_op, "sdc\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SDC1(unsigned long word)
{		//61(3D)
    sprintf(dis_op, "sdc1\t%s, 0x%04X(%s)", gpr_rn[getFT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SDC2(unsigned long word)
{		//62(3E)
    sprintf(dis_op, "sdc2\t%s, 0x%04X(%s)", gpr_rn[getFT(word)], getIMM(word), gpr_rn[getBASE(word)]);
}

void SD(unsigned long word)
{		//63(3F)
    sprintf(dis_op, "sd\t%s, 0x%04X(%s)", gpr_rn[getRT(word)], getIMM(word), gpr_rn[getBASE(word)]);
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

char * getOP(unsigned long int word)
{
    MAIN_T[ word >> 26 ](word);
    return dis_op;
}

/* prototype:
 adis o stalfo.s 2 ZELOOTMA.z64
*/

int main(int argc, char * argv[])
{
    int wordc=0;
    int i;
    char * buff;
    int argcp=1;
    int arge=argc-2;
    char * outname;
    FILE * outputf;
    FILE * ROM;
    int targetnum;
    
    int ATABLE_START= OOT_DEBUG_ATABLE_START;
    int NAMETABLE_ADD = 0xA771A0;
    int output=0;
    html = 0;
    int little_endian = 0;
    int anum;
    int text_size, data_size, rodata_size, bss_size, rel_count;
    
    /* Well gee I hope they gave enough arguments */
    if(argc < 3)
    {
        printf("invalid usage\n");
        return EXIT_FAILURE;
    }
    
    /* Actor number*/
    sscanf(argv[arge], "%i", &anum);
    
    /* Parse options */
    while (argcp < arge)
    {
    if(!strcmp(argv[argcp],"o")) /* output */
    {
        argcp++;
        output=1;
        outname = argv[argcp];
    }
    else if(!strcmp(argv[argcp],"f")) /* format to html */
        html/*|*/=1;
/*
    else if(!strcmp(argv[argcp],"s"))
        html|=2;
*/
    else if(!strcmp(argv[argcp],"at")){ /* set actor table offset */
        argcp++;
        sscanf(argv[argcp],"%x", &ATABLE_START);
        argcp++;
        sscanf(argv[argcp],"%x", &NAMETABLE_ADD);
    }
    else
        printf("Unknown/unhandled argument: %s\n", argv[argcp]);
    argcp++;
    }
    
    //printf("Output: %s\n", outname);
    if (output)
        outputf=fopen(outname, "w");
    else
        outputf=stdout;
    
    ROM = fopen(argv[arge+1],"rb");
    if (!ROM)
    {
        printf("Error opening file `%s`\n",argv[arge]);
        return EXIT_FAILURE;
    }
    /* Read ROM */
    fseek(ROM, 0, SEEK_END);
    int fend = ftell(ROM);
    rewind(ROM);
    buff=(char*)malloc(fend);
    fread(buff, 1, fend, ROM);
    fclose(ROM);
    
    /* Interpret ROM */
    actorCodeEntry (*actors)[470] = (void*)(ATABLE_START + buff);
    
    int actorsize = (flip32((*actors)[anum].rom_end) - flip32((*actors)[anum].rom_start));
    char * actor = (char*)(flip32((*actors)[anum].rom_start) + buff);
    unsigned long *header_inset = (void*)(actor + actorsize - 4);
    local_start = flip32((*actors)[anum].virtual_start);
    local_end = flip32((*actors)[anum].virtual_end);
    char * aname = (char*) ( (flip32((*actors)[anum].name)&0xFFFFFF) + NAMETABLE_ADD + buff );
    
    actorHeader *actorHeader = (void*)(actor + actorsize - flip32(*header_inset));
    actorInformation *actorInfo = (void*)(actor + (flip32((*actors)[anum].virtual_ainfo) - local_start));
    
    text_size = flip32(actorHeader->text_size);
    data_size = flip32(actorHeader->data_size);
    rodata_size = flip32(actorHeader->rodata_size);
    bss_size = flip32(actorHeader->bss_size);
    rel_count = flip32(actorHeader->rel_count);
    
    unsigned long (*words)[] = (void*)actor;
/*    if(html&2){
        fprintf(outputf, "<html>\n <head>\n  <title>%s Notes</title>\n", aname);
        fprintf(outputf, "\t<script type=\"text/javascript\" src=\"http://spinout182.com/sh/scripts/shCore.js\"></script>\n\t<script type=\"text/javascript\" src=\"http://spinout182.com/sh/scripts/shBrushCpp.js\"></script> \n\t<link type=\"text/css\" rel=\"stylesheet\" href=\"http://spinout182.com/sh/styles/shCore.css\"/> \n\t<link type=\"text/css\" rel=\"stylesheet\" href=\"http://spinout182.com/sh/styles/shThemeDefault.css\"/> \n\t<script type=\"text/javascript\"> \n\t\tSyntaxHighlighter.config.clipboardSwf = 'http://spinout182.com/sh/scripts/clipboard.swf';\n\t\tSyntaxHighlighter.all();\n\t</script>");
        fprintf(outputf, "\n </head>\n <body>\n  <pre class=\"brush: c\">\n#include &lt;mips.h&gt;");
    }
    else if (html&1)
*/
    if(html)
        fprintf(outputf, "<html>\n <head>\n  <title>%s Notes</title>\n </head>\n <body>\n  <pre lang=\"C\">\n#include &lt;mips.h&gt;", aname);
    else
        fprintf(outputf, "#include <mips.h>");
    fprintf(outputf, "\n\n/*\n INFO:\n Actor %i \"%s\"\n ROM offsets 0x%08X-0x%08X\n Virtual offsets: 0x%08X-0x%08X\n .text size: 0x%08X\n .data size: 0x%08X\n .rodata size: 0x%08X\n .bss size: 0x%08X\n Initial register values used in comments:\n  a0: 0x%08X\n  a1: 0x%08X\n  sp: 0x%08X\n Others: 0\n \n Type: %s (%02X)\n Object: %04X\n\n generated by %s\n*/\n\n\n",
        anum,
        aname,
        flip32((*actors)[anum].rom_start),
        flip32((*actors)[anum].rom_end),
        local_start,
        local_end,
        text_size,
        data_size,
        rodata_size,
        bss_size,
        gpr_regs[4],
        gpr_regs[5],
        gpr_regs[29],
        actTypes[actorInfo->type],
        actorInfo->type,
        flip16(actorInfo->object_number),
        argv[0]
    );
    
    map_branches = 1;
    int function_count=0;
    pc=local_start;
    /* Read relocations - TODO */
    char tmp [32];
    sprintf(tmp, "%s_Init", aname);
    add_function(flip32(actorInfo->init_func), tmp);
    if(actorInfo->draw_func){
        sprintf(tmp, "%s_Draw", aname);
        add_function(flip32(actorInfo->draw_func), tmp);
    }
    sprintf(tmp, "%s_Exec", aname);
    if(actorInfo->code_func){
        add_function(flip32(actorInfo->code_func), tmp);
        sprintf(tmp, "%s_Other", aname);
    }
    if(actorInfo->unknown_func){
        add_function(flip32(actorInfo->unknown_func), tmp);
        sprintf(tmp, "%s_Func_0", aname);
    }
    add_function(pc, tmp);
    function_count=1;
    
    mapped_functions = malloc(256);
    //memset(mapped_functions, 0x0, 256);
    
    /* First scan - just to map out branches/jumps */
    for (i=0; i<(text_size >> 2); i++)
    {
        if (pc == EndOfFunction+4)
        {
            getOP( flip32((*words)[i]) );
            if((*words)[i])
            {
                sprintf(tmp, "%s_Func_%i", aname, function_count);
                if(add_function(pc, &tmp[0]))function_count++;
                reset_gpr_regs();
            }
            else
                EndOfFunction+=4;
        }
        getOP( flip32((*words)[i]));
        pc+=4;
    }
    pc = local_start;
    map_branches = 0;
    reset_gpr_regs();
    
    //FILE * conf = fopen("conf.ld", "w");
    fprintf(outputf, "\n\n/*\n");
    for (i=0; i<target_count; i++)
    {
        fprintf(outputf, "%s = 0x%08X;\n", functions[i].name, functions[i].addr);
    }
    fprintf(outputf, "*/\n\n\t.text\n\t\n\t\n");
    //fclose(conf);
    
    char currFuncName[32];
    sprintf(currFuncName, "%s", functions[target2label(pc)].name);
    
    fprintf(outputf, "\t.set\tnoreorder\n\t.set\tnoat\n\t.global\t%s\n\t.ent\t%s\n\n", currFuncName, currFuncName);
    if (html)// && !html&2)
        fprintf(outputf, "<a href=\"#%s\" name=\"%s\">%s</a>:\t/* %08X */\n\n", currFuncName, currFuncName, currFuncName, pc);
    else
        fprintf(outputf, "%s:\t/* %08X */\n\n", currFuncName, pc);
    function_count=1;
    
    /* Second scan - fprintf()s */
    for (i=0; i<(text_size >> 2); i++)
    {
        
        map_branches = 0;
        if (pc == EndOfFunction+4)
        {
            if((*words)[i])
            {
                int functionno = target2label(pc);
                if(functionno >= 0)
                    sprintf(currFuncName, "%s", functions[functionno].name);
                else
                    sprintf(currFuncName, "function_%08X", pc|0x80000000);
                
                fprintf(outputf, "\t.set\tnoreorder\n\t.set\tnoat\n\t.global\t%s\n\t.ent\t%s\n\n",currFuncName, currFuncName);
                if (html)
                    fprintf(outputf, "<a href=\"#%s\" name=\"%s\">%s</a>:\t/* %08X */\n\n", currFuncName, currFuncName, currFuncName, pc);
                else
                    fprintf(outputf, "%s:\t/* %08X */\n\n", currFuncName, pc);
                function_count++;
                reset_gpr_regs();
                EndOfFunction = -1;
            }
            else
                EndOfFunction+=4;
        }
        
        fprintf(outputf, "\t%s\n", getOP( flip32((*words)[i]) ));
        gpr_regs[0] = 0;	/* Just in case anything tries to change $zero */
        if (pc == EndOfFunction && !(mapped_functions[function_count]))
        {
            mapped_functions[function_count] = 1;
            fprintf(outputf, "\n\t.end\t%s\n\t.set\tat\n\t.set\tnoreorder\n\n    /* #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~# */\n\n",currFuncName);
            
        }
        if (jaltaken)
        {
            jaltaken--;
            if (!jaltaken)
                reset_gpr_regs_soft();
        }
        
        pc+=4;
        
        targetnum=isTarget(pc+4);
        if (targetnum>0)
            if (html)
                fprintf(outputf, "<a href=\"#$lbl_%i\" name=\"$lbl_%i\">$lbl_%i</a>:\n", targetnum, targetnum, targetnum);
            else
                fprintf(outputf, "$lbl_%i:\n", targetnum);
    }
    local_start = flip32((*actors)[anum].virtual_start);
    
    fprintf(outputf, "\n/* TODO: */\n\n\n\t.data\n\n\t/*\n\tAddr: %08X\n\t\n\t",local_start+text_size);
    int j;
    for (i=text_size; i<text_size+data_size; i+=16)
    {
        for (j=i;j<i+16;j+=4)
            fprintf(outputf, "%08X ", flip32((*words)[j>>2]));
        fprintf(outputf, "\n\t");
    }
    fprintf(outputf, "\n\t*/\n\n\t.rodata\n\n\t/*\n\tAddr: %08X\n\t\n\t",local_start+text_size+rodata_size);
    
    for (; i<text_size+data_size+rodata_size; i+=16)
    {
        for (j=i;j<i+16;j+=4)
            fprintf(outputf, "%08X ", flip32((*words)[j>>2]));
        fprintf(outputf, "\n\t");
    }
    fprintf(outputf, "\n\t*/\n");
    if (html)
        fprintf(outputf, "  </pre>\n </body>\n</html>");
    fclose(outputf);
    free(buff);
    
    return EXIT_SUCCESS;
}

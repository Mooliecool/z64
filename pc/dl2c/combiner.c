#include "global.h"

#define	dGCCc0w0(w, saRGB0, mRGB0, saA0, mA0)	\
{						\
		saRGB0 = _SHIFTR(w, 20, 4);	\
		mRGB0 = _SHIFTR(w, 15, 5);	\
		saA0 = _SHIFTR(w, 12, 3);	\
		mA0 = _SHIFTR(w, 9, 3);		\
}

#define	dGCCc1w0(w, saRGB1, mRGB1)		\
{						\
		saRGB1 = _SHIFTR(w, 5, 4);	\
		mRGB1 = _SHIFTR(w, 0, 5);	\
}

#define dGCCc0w1(w, sbRGB0, aRGB0, sbA0, aA0)	\
{						\
                sbRGB0 = _SHIFTR(w, 28, 4);	\
                aRGB0 = _SHIFTR(w, 15, 3);	\
		sbA0 = _SHIFTR(w, 12, 3);	\
		aA0 = _SHIFTR(w, 9, 3);		\
}

#define	dGCCc1w1(w, sbRGB1, saA1, mA1, aRGB1, sbA1, aA1)	\
{								\
		sbRGB1 = _SHIFTR(w, 24, 4);			\
		saA1 = _SHIFTR(w, 21, 3);			\
		mA1 = _SHIFTR(w, 18, 3);			\
		aRGB1 = _SHIFTR(w, 6, 3);			\
		sbA1 = _SHIFTR(w, 3, 3);			\
		aA1 = _SHIFTL(w, 0, 3);				\
}

char * G_CCMUX[32] = {
"COMBINED",		//0
"TEXEL0",		//1
"TEXEL1",		//2
"PRIMITIVE",		//3
"SHADE",		//4
"ENVIRONMENT",		//5
"1",			//6
"COMBINED_ALPHA",	//7
"TEXEL0_ALPHA",		//8
"TEXEL1_ALPHA",		//9
"PRIMITIVE_ALPHA",	//10
"SHADE_ALPHA",		//11
"ENV_ALPHA",		//12
"LOD_FRACTION",		//13
"PRIM_LOD_FRAC",	//14
"K5",			//15
"0x10",
"0x11",
"0x12",
"0x13",
"0x14",
"0x15",
"0x16",
"0x17",
"0x18",
"0x19",
"0x1A",
"0x1B",
"0x1C",
"0x1D",
"0x1E",
"0",			//31
};

char * G_ACMUX[8] = {
"COMBINED",	//0
"TEXEL0",	//1
"TEXEL1",	//2
"PRIMITIVE",	//3
"SHADE",	//4
"ENVIRONMENT",	//5
"PRIM_LOD_FRAC",	//6
"0"		//7
};

void ColorCombiner(unsigned int w0, unsigned int w1, char * text)
{
	int a0, b0, c0, d0, Aa0, Ab0, Ac0, Ad0, a1, b1, c1, d1,	Aa1, Ab1, Ac1, Ad1;
	w0 &= 0xFFFFFF;
	dGCCc0w0(w0, a0, c0, Aa0, Ac0);
	dGCCc1w0(w0, a1, c1);
	dGCCc0w1(w1, b0, d0, Ab0, Ad0);
	dGCCc1w1(w1, b1, Aa1, Ac1, d1, Ab1, Ad1);
	sprintf(text, "\tgsDPSetCombineLERP(\n\t\t%s, %s, %s, %s,\n\t\t%s, %s, %s, %s,\n\t\t%s, %s, %s, %s,\n\t\t%s, %s, %s, %s)",
		G_CCMUX[a0], G_CCMUX[b0], G_CCMUX[c0], G_CCMUX[d0], G_ACMUX[Aa0], G_ACMUX[Ab0], G_ACMUX[Ac0], G_ACMUX[Ad0],
		G_CCMUX[a1], G_CCMUX[b1], G_CCMUX[c1], G_CCMUX[d1], G_ACMUX[Aa1], G_ACMUX[Ab1], G_ACMUX[Ac1], G_ACMUX[Ad1] );
}

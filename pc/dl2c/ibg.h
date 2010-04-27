
#ifndef IGB_H
#define IGB_H


#define _SHIFTL( v, s, w )	\
    (((unsigned int)v & ((0x01 << w) - 1)) << s)
#define _SHIFTR( v, s, w )	\
    (((unsigned int)v >> s) & ((0x01 << w) - 1))

#define getVtx(buff, off, vtx)\
{\
	vtx.x = (signed short)Read16(buff, off);\
	vtx.y = (signed short)Read16(buff, off+2);\
	vtx.z = (signed short)Read16(buff, off+4);\
	vtx.s = (signed short)Read16(buff, off+8);\
	vtx.t = (signed short)Read16(buff, off+10);\
	vtx.r = buff[off+12];\
	vtx.g = buff[off+13];\
	vtx.b = buff[off+14];\
	vtx.a = buff[off+15];\
}


/* RDP commands: */
#define	G_SETCIMG		0xff	/*  -1 */
#define	G_SETZIMG		0xfe	/*  -2 */
#define	G_SETTIMG		0xfd	/*  -3 */
#define	G_SETCOMBINE		0xfc	/*  -4 */
#define	G_SETENVCOLOR		0xfb	/*  -5 */
#define	G_SETPRIMCOLOR		0xfa	/*  -6 */
#define	G_SETBLENDCOLOR		0xf9	/*  -7 */
#define	G_SETFOGCOLOR		0xf8	/*  -8 */
#define	G_SETFILLCOLOR		0xf7	/*  -9 */
#define	G_FILLRECT		0xf6	/* -10 */
#define	G_SETTILE		0xf5	/* -11 */
#define	G_LOADTILE		0xf4	/* -12 */
#define	G_LOADBLOCK		0xf3	/* -13 */
#define	G_SETTILESIZE		0xf2	/* -14 */
#define	G_LOADTLUT		0xf0	/* -16 */
#define	G_RDPSETOTHERMODE	0xef	/* -17 */
#define	G_SETPRIMDEPTH		0xee	/* -18 */
#define	G_SETSCISSOR		0xed	/* -19 */
#define	G_SETCONVERT		0xec	/* -20 */
#define	G_SETKEYR		0xeb	/* -21 */
#define	G_SETKEYGB		0xea	/* -22 */
#define	G_RDPFULLSYNC		0xe9	/* -23 */
#define	G_RDPTILESYNC		0xe8	/* -24 */
#define	G_RDPPIPESYNC		0xe7	/* -25 */
#define	G_RDPLOADSYNC		0xe6	/* -26 */
#define G_TEXRECTFLIP		0xe5	/* -27 */
#define G_TEXRECT		0xe4	/* -28 */

/* F3DEX2 specific */
#define	G_NOOP			0x00
#define	G_RDPHALF_2		0xf1
#define	G_SETOTHERMODE_H	0xe3
#define	G_SETOTHERMODE_L	0xe2
#define	G_RDPHALF_1		0xe1
#define	G_SPNOOP		0xe0
#define	G_ENDDL			0xdf
#define	G_DL			0xde
#define	G_LOAD_UCODE		0xdd
#define	G_MOVEMEM		0xdc
#define	G_MOVEWORD		0xdb
#define	G_MTX			0xda
#define G_GEOMETRYMODE		0xd9
#define	G_POPMTX		0xd8
#define	G_TEXTURE		0xd7
#define	G_DMA_IO		0xd6
#define	G_SPECIAL_1		0xd5
#define	G_SPECIAL_2		0xd4
#define	G_SPECIAL_3		0xd3

#define	G_VTX			0x01
#define	G_MODIFYVTX		0x02
#define	G_CULLDL		0x03
#define	G_BRANCH_Z		0x04
#define	G_TRI1			0x05
#define G_TRI2			0x06
#define G_QUAD			0x07
#define G_LINE3D		0x08


#define G_ZBUFFER		0x00000001
#define G_SHADE			0x00000004	/* enable Gouraud interp */
#define G_TEXTURE_ENABLE	0x00000000	/* Ignored               */
#define G_SHADING_SMOOTH	0x00200000	/* flat or smooth shaded */
#define G_CULL_FRONT		0x00000200
#define G_CULL_BACK		0x00000400
#define G_CULL_BOTH		0x00000600	/* To make code cleaner */
#define G_FOG			0x00010000
#define G_LIGHTING		0x00020000
#define G_TEXTURE_GEN		0x00040000
#define G_TEXTURE_GEN_LINEAR	0x00080000
#define G_LOD			0x00100000	



#define dgsSetImage(w0, w1, fmt, f, s, w, i)	\
	fmt = _SHIFTR(w0, 24, 8); \
	f = _SHIFTR(w0, 21, 3);	\
	s = _SHIFTR(w0, 19, 2);\
	w = _SHIFTR(w0, 0, 12)+1;	\
	i = w1

#define dgsSetTextureImage(w0, w1, f, s, w, i)	\
	f = _SHIFTR(w0, 21, 3);	\
	s = _SHIFTR(w0, 19, 2);\
	w = _SHIFTR(w0, 0, 12)+1;	\
	i = w1

#define dgsDPSetTileSize(w0, w1, tile, uls, ult, lrs, lrt)	\
	tile = _SHIFTR(w1, 24, 3);\
	uls = _SHIFTR(w0, 12, 12);\
	ult = _SHIFTR(w0, 0, 12);\
	lrs = _SHIFTR(w1, 12, 12);\
	lrt = _SHIFTR(w1, 0, 12);\

#define	G_TEXTURE_IMAGE_FRAC	2
#define	G_TEXTURE_SCALE_FRAC	16
#define	G_SCALE_FRAC		8
#define	G_ROTATE_FRAC		16

#define dgsDPSetTile(w0, w1,fmt, siz, line, tmem, tile, palette, cmt,		\
		maskt, shiftt, cms, masks, shifts)			\
	fmt = _SHIFTR(w0, 21, 3);\
	siz = _SHIFTR(w0, 19, 2);\
	line = _SHIFTR(w0, 9, 9);\
	tmem = _SHIFTR(w0, 0, 9);\
	tile = _SHIFTR(w1, 24, 3);\
	palette = _SHIFTR(w1, 20, 4);\
	cmt = _SHIFTR(w1, 18, 2);\
	maskt = _SHIFTR(w1, 14, 4);\
	shiftt = _SHIFTR(w1, 10, 4);\
	cms = _SHIFTR(w1, 8, 2);\
	masks = _SHIFTR(w1, 4, 4);\
	shifts = _SHIFTR(w1, 0, 4);\

#define dDPRGBColor(w0, w1, r, g, b, a)	\
	r = _SHIFTR(w1, 24, 8);\
	g = _SHIFTR(w1, 16, 8);\
	b = _SHIFTR(w1, 8, 8);\
	a = _SHIFTR(w1, 0, 8);

#define Tri(w, a, b, c, flag)\
	flag = 0;\
	a = _SHIFTR(w, 17, 7);\
	b = _SHIFTR(w, 9, 7);\
	c = _SHIFTR(w, 1, 7);
	//hack: shiftr fed 17/7 instead of 16/8 in order to divide by two early. done with b and c as well.


#define dgsSP1Triangle(w0, w1, a, b, c, flag)\
	Tri(w0, a, b, c, flag);


#define dgsSP2Triangle(w0, w1, a, b, c, d, e, f, flag0, flag1)\
	Tri(w0, a, b, c, flag0);\
	Tri(w1, d, e, f, flag1);

#define dgsDPSetPrimColor(w0, w1, m, l, r, g, b, a)\
	m = _SHIFTR(w0, 8, 8);\
	l = _SHIFTR(w0, 0, 8);\
	r = _SHIFTR(w1, 24, 8);\
	g = _SHIFTR(w1, 16, 8);\
	b = _SHIFTR(w1, 8, 8);\
	a = _SHIFTR(w1, 0, 8);


#define dgsSPTexture(w0, w1, s, t, level, tile, on, BOWTIE_VAL)\
	BOWTIE_VAL = _SHIFTR(w0, 16, 8);\
	level = _SHIFTR(w0, 11, 3);\
	tile = _SHIFTR(w0, 8, 3);\
	on = _SHIFTR(w0, 1, 7);\
	s = (signed short)_SHIFTR(w1, 16, 16);\
	t = (signed short)_SHIFTR(w1, 0, 16);

#define dgSPVertex(w0, w1, v, n, v0)\
{\
	n = _SHIFTR(w0,12,8);\
	v0 = _SHIFTR(w0,1,7)-n;\
	v = w1;\
}

#define gsSPCullDisplayList(vstart,vend)				\
{									\
	_SHIFTL(G_CULLDL, 24, 8) | ((0x0f & (vstart))*40), 		\
	((0x0f & ((vend)+1))*40)					\
}

#define dgsSPCullDisplayList(w0, w1, vstart, vend)\
{								\
	vstart = ((w0 & 0xFFFFFF) / 40) & 0xF;			\
	vend = (((w1 & 0xFFFFFF) / 40) & 0xF) - 1;		\
}


#define G_IM_FMT_RGBA	0
#define G_IM_FMT_YUV	1
#define G_IM_FMT_CI	2
#define G_IM_FMT_IA	3
#define G_IM_FMT_I	4

/*
 * G_SETIMG siz: set image pixel size
 */
#define G_IM_SIZ_4b	0
#define G_IM_SIZ_8b	1
#define G_IM_SIZ_16b	2
#define G_IM_SIZ_32b	3
#define G_IM_SIZ_DD	5


#endif

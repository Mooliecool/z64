import struct

def dwords2string(dwords):
    ret = ""
    for dword in dwords:
        ret = "%s%s" % (ret, struct.pack(">Q", dword))
    return ret

BOWTIE_VAL		= 0
G_TX_DXT_FRAC		= 11
G_TX_LDBLK_MAX_TXL	= 4095 #Or 2047. Has not made a difference in my testing thus far.


G_NOOP			= 0x00
G_RDPHALF_2		= 0xf1
G_SETOTHERMODE_H	= 0xe3
G_SETOTHERMODE_L	= 0xe2
G_RDPHALF_1		= 0xe1
G_SPNOOP		= 0xe0
G_ENDDL			= 0xdf
G_DL			= 0xde
G_LOAD_UCODE		= 0xdd
G_MOVEMEM		= 0xdc
G_MOVEWORD		= 0xdb
G_MTX			= 0xda
G_GEOMETRYMODE		= 0xd9
G_POPMTX		= 0xd8
G_TEXTURE		= 0xd7
G_DMA_IO		= 0xd6
G_SPECIAL_1		= 0xd5
G_SPECIAL_2		= 0xd4
G_SPECIAL_3		= 0xd3

#F3DEX2 special
G_VTX			= 0x01
G_MODIFYVTX		= 0x02
G_CULLDL		= 0x03
G_BRANCH_Z		= 0x04
G_TRI1			= 0x05
G_TRI2			= 0x06
G_QUAD			= 0x07
G_LINE3D		= 0x08

#RDP commands
G_SETCIMG		= 0xff	#  -1
G_SETZIMG		= 0xfe	#  -2
G_SETTIMG		= 0xfd	#  -3
G_SETCOMBINE		= 0xfc	#  -4
G_SETENVCOLOR		= 0xfb	#  -5
G_SETPRIMCOLOR		= 0xfa	#  -6
G_SETBLENDCOLOR		= 0xf9	#  -7
G_SETFOGCOLOR		= 0xf8	#  -8
G_SETFILLCOLOR		= 0xf7	#  -9
G_FILLRECT		= 0xf6	# -10
G_SETTILE		= 0xf5	# -11
G_LOADTILE		= 0xf4	# -12
G_LOADBLOCK		= 0xf3	# -13
G_SETTILESIZE		= 0xf2	# -14
G_LOADTLUT		= 0xf0	# -16
G_RDPSETOTHERMODE	= 0xef	# -17
G_SETPRIMDEPTH		= 0xee	# -18
G_SETSCISSOR		= 0xed	# -19
G_SETCONVERT		= 0xec	# -20
G_SETKEYR		= 0xeb	# -21
G_SETKEYGB		= 0xea	# -22
G_RDPFULLSYNC		= 0xe9	# -23
G_RDPTILESYNC		= 0xe8	# -24
G_RDPPIPESYNC		= 0xe7	# -25
G_RDPLOADSYNC		= 0xe6	# -26
G_TEXRECTFLIP		= 0xe5	# -27
G_TEXRECT		= 0xe4	# -28

G_ON	= 1
G_OFF	= 0

#GeometryMode flags
G_ZBUFFER		= 0x00000001
G_SHADE			= 0x00000004	# enable Gouraud interp
# rest of low byte reserved for setup ucode
G_TEXTURE_ENABLE	= 0x00000000	# Ignored
G_SHADING_SMOOTH	= 0x00200000	# flat or smooth shaded
G_CULL_FRONT		= 0x00000200
G_CULL_BACK		= 0x00000400
G_CULL_BOTH		= 0x00000600	# To make code cleaner 
G_FOG			= 0x00010000
G_LIGHTING		= 0x00020000
G_TEXTURE_GEN		= 0x00040000
G_TEXTURE_GEN_LINEAR	= 0x00080000
G_LOD			= 0x00100000

#Texture flags
G_TX_LOADTILE	= 7
G_TX_RENDERTILE	= 0

G_TX_NOMIRROR	= 0
G_TX_WRAP	= 0
G_TX_MIRROR	= 0x1
G_TX_CLAMP	= 0x2
G_TX_NOMASK	= 0
G_TX_NOLOD	= 0

# Coordinate shift values, number of bits of fraction
G_TEXTURE_IMAGE_FRAC	= 2
G_TEXTURE_SCALE_FRAC	= 16
G_SCALE_FRAC		= 8
G_ROTATE_FRAC		= 16

#G_SETIMG fmt: set image format
G_IM_FMT_RGBA	= 0
G_IM_FMT_YUV	= 1
G_IM_FMT_CI	= 2
G_IM_FMT_IA	= 3
G_IM_FMT_I	= 4

# G_SETIMG siz: set image pixel size
G_IM_SIZ_4b	= 0
G_IM_SIZ_8b	= 1
G_IM_SIZ_16b	= 2
G_IM_SIZ_32b	= 3

G_IM_BYTES= {
 G_IM_SIZ_4b	: 0,
 G_IM_SIZ_8b	: 1,
 G_IM_SIZ_16b	: 2,
 G_IM_SIZ_32b	: 4
}

G_IM_LINE_BYTES= {
 G_IM_SIZ_4b	: 0,
 G_IM_SIZ_8b	: 1,
 G_IM_SIZ_16b	: 2,
 G_IM_SIZ_32b	: 2
}

G_IM_TILE_BYTES = G_IM_LINE_BYTES

G_IM_LOAD_BLOCK= {
 G_IM_SIZ_4b	: G_IM_SIZ_16b,
 G_IM_SIZ_8b	: G_IM_SIZ_16b,
 G_IM_SIZ_16b	: G_IM_SIZ_16b,
 G_IM_SIZ_32b	: G_IM_SIZ_32b
}

G_IM_INCR= {
 G_IM_SIZ_4b	: 3,
 G_IM_SIZ_8b	: 1,
 G_IM_SIZ_16b	: 0,
 G_IM_SIZ_32b	: 0
}


G_IM_SHIFT={
 G_IM_SIZ_4b	: 2,
 G_IM_SIZ_8b	: 1,
 G_IM_SIZ_16b	: 0,
 G_IM_SIZ_32b	: 0
}



# G_SETOTHERMODE_L sft: shift count
G_MDSFT_ALPHACOMPARE		= 0
G_MDSFT_ZSRCSEL			= 2
G_MDSFT_RENDERMODE		= 3
G_MDSFT_BLENDER			= 16

# G_SETOTHERMODE_H sft: shift count
G_MDSFT_BLENDMASK		= 0	# unsupported 
G_MDSFT_ALPHADITHER		= 4
G_MDSFT_RGBDITHER		= 6

G_MDSFT_COMBKEY			= 8
G_MDSFT_TEXTCONV		= 9
G_MDSFT_TEXTFILT		= 12
G_MDSFT_TEXTLUT			= 14
G_MDSFT_TEXTLOD			= 16
G_MDSFT_TEXTDETAIL		= 17
G_MDSFT_TEXTPERSP		= 19
G_MDSFT_CYCLETYPE		= 20
G_MDSFT_COLORDITHER		= 22	# unsupported in HW 2.0 
G_MDSFT_PIPELINE		= 23

# G_SETOTHERMODE_H gPipelineMode 
G_PM_1PRIMITIVE		= (1 << G_MDSFT_PIPELINE)
G_PM_NPRIMITIVE		= (0 << G_MDSFT_PIPELINE)

# G_SETOTHERMODE_H gSetCycleType 
G_CYC_1CYCLE		= (0 << G_MDSFT_CYCLETYPE)
G_CYC_2CYCLE		= (1 << G_MDSFT_CYCLETYPE)
G_CYC_COPY		= (2 << G_MDSFT_CYCLETYPE)
G_CYC_FILL		= (3 << G_MDSFT_CYCLETYPE)

# G_SETOTHERMODE_H gSetTexturePersp 
G_TP_NONE	= (0 << G_MDSFT_TEXTPERSP)
G_TP_PERSP	= (1 << G_MDSFT_TEXTPERSP)

# G_SETOTHERMODE_H gSetTextureDetail 
G_TD_CLAMP	= (0 << G_MDSFT_TEXTDETAIL)
G_TD_SHARPEN	= (1 << G_MDSFT_TEXTDETAIL)
G_TD_DETAIL	= (2 << G_MDSFT_TEXTDETAIL)

# G_SETOTHERMODE_H gSetTextureLOD 
G_TL_TILE	= (0 << G_MDSFT_TEXTLOD)
G_TL_LOD	= (1 << G_MDSFT_TEXTLOD)

# G_SETOTHERMODE_H gSetTextureLUT 
G_TT_NONE	= (0 << G_MDSFT_TEXTLUT)
G_TT_RGBA16	= (2 << G_MDSFT_TEXTLUT)
G_TT_IA16	= (3 << G_MDSFT_TEXTLUT)

# G_SETOTHERMODE_H gSetTextureFilter 
G_TF_POINT	= (0 << G_MDSFT_TEXTFILT)
G_TF_AVERAGE	= (3 << G_MDSFT_TEXTFILT)
G_TF_BILERP	= (2 << G_MDSFT_TEXTFILT)

# G_SETOTHERMODE_H gSetTextureConvert 
G_TC_CONV	= (0 << G_MDSFT_TEXTCONV)
G_TC_FILTCONV	= (5 << G_MDSFT_TEXTCONV)
G_TC_FILT	= (6 << G_MDSFT_TEXTCONV)

# G_SETOTHERMODE_H gSetCombineKey 
G_CK_NONE	= (0 << G_MDSFT_COMBKEY)
G_CK_KEY	= (1 << G_MDSFT_COMBKEY)

# G_SETOTHERMODE_H gSetColorDither 
G_CD_MAGICSQ		= (0 << G_MDSFT_RGBDITHER)
G_CD_BAYER		= (1 << G_MDSFT_RGBDITHER)
G_CD_NOISE		= (2 << G_MDSFT_RGBDITHER)


G_CD_ENABLE		= (1 << G_MDSFT_COLORDITHER)
G_CD_DISABLE		= (0 << G_MDSFT_COLORDITHER)

# G_SETOTHERMODE_H gSetAlphaDither 
G_AD_PATTERN		= (0 << G_MDSFT_ALPHADITHER)
G_AD_NOTPATTERN		= (1 << G_MDSFT_ALPHADITHER)
G_AD_NOISE		= (2 << G_MDSFT_ALPHADITHER)
G_AD_DISABLE		= (3 << G_MDSFT_ALPHADITHER)

# G_SETOTHERMODE_L gSetAlphaCompare 
G_AC_NONE		= (0 << G_MDSFT_ALPHACOMPARE)
G_AC_THRESHOLD		= (1 << G_MDSFT_ALPHACOMPARE)
G_AC_DITHER		= (3 << G_MDSFT_ALPHACOMPARE)

# G_SETOTHERMODE_L gSetDepthSource 
G_ZS_PIXEL		= (0 << G_MDSFT_ZSRCSEL)
G_ZS_PRIM		= (1 << G_MDSFT_ZSRCSEL)

# G_SETOTHERMODE_L gSetRenderMode 
AA_EN		= 0x8
Z_CMP		= 0x10
Z_UPD		= 0x20
IM_RD		= 0x40
CLR_ON_CVG	= 0x80
CVG_DST_CLAMP	= 0
CVG_DST_WRAP	= 0x100
CVG_DST_FULL	= 0x200
CVG_DST_SAVE	= 0x300
ZMODE_OPA	= 0
ZMODE_INTER	= 0x400
ZMODE_XLU	= 0x800
ZMODE_DEC	= 0xc00
CVG_X_ALPHA	= 0x1000
ALPHA_CVG_SEL	= 0x2000
FORCE_BL	= 0x4000
TEX_EDGE	= 0x0000 # used to be 0x8000 

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# G_SETOTHERMODE_L gSetRenderMode
AA_EN	= 0x8
Z_CMP	= 0x10
Z_UPD	= 0x20
IM_RD	= 0x40
CLR_ON_CVG	= 0x80
CVG_DST_CLAMP	= 0
CVG_DST_WRAP	= 0x100
CVG_DST_FULL	= 0x200
CVG_DST_SAVE	= 0x300
ZMODE_OPA	= 0
ZMODE_INTER	= 0x400
ZMODE_XLU	= 0x800
ZMODE_DEC	= 0xc00
CVG_X_ALPHA	= 0x1000
ALPHA_CVG_SEL	= 0x2000
FORCE_BL	= 0x4000
TEX_EDGE	= 0x0000 # used to be 0x8000

G_BL_CLR_IN	= 0
G_BL_CLR_MEM	= 1
G_BL_CLR_BL	= 2
G_BL_CLR_FOG	= 3
G_BL_1MA	= 0
G_BL_A_MEM	= 1
G_BL_A_IN	= 0
G_BL_A_FOG	= 1
G_BL_A_SHADE	= 2
G_BL_1	= 2
G_BL_0	= 3


GBL_c1	=lambda m1a, m1b, m2a, m2b: (m1a) << 30 | (m1b) << 26 | (m2a) << 22 | (m2b) << 18
GBL_c2	=lambda m1a, m1b, m2a, m2b: (m1a) << 28 | (m1b) << 24 | (m2a) << 20 | (m2b) << 16

GBL_ck ={
 1 : GBL_c1,
 2 : GBL_c2
}

RM_AA_ZB_OPA_SURF	=lambda clk: AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | ALPHA_CVG_SEL | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)

RM_RA_ZB_OPA_SURF	=lambda clk: AA_EN | Z_CMP | Z_UPD | CVG_DST_CLAMP | ZMODE_OPA | ALPHA_CVG_SEL | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)

RM_AA_ZB_XLU_SURF	=lambda clk: AA_EN | Z_CMP | IM_RD | CVG_DST_WRAP | CLR_ON_CVG | FORCE_BL | ZMODE_XLU | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)

RM_AA_ZB_OPA_DECAL	=lambda clk: AA_EN | Z_CMP | IM_RD | CVG_DST_WRAP | ALPHA_CVG_SEL | ZMODE_DEC | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)

RM_RA_ZB_OPA_DECAL	=lambda clk: AA_EN | Z_CMP | CVG_DST_WRAP | ALPHA_CVG_SEL | ZMODE_DEC | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)

RM_AA_ZB_XLU_DECAL	=lambda clk: AA_EN | Z_CMP | IM_RD | CVG_DST_WRAP | CLR_ON_CVG | FORCE_BL | ZMODE_DEC | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)

RM_AA_ZB_OPA_INTER	=lambda clk: AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | ALPHA_CVG_SEL | ZMODE_INTER | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)

RM_RA_ZB_OPA_INTER	=lambda clk: AA_EN | Z_CMP | Z_UPD | CVG_DST_CLAMP | ALPHA_CVG_SEL | ZMODE_INTER | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)

RM_AA_ZB_XLU_INTER	=lambda clk: AA_EN | Z_CMP | IM_RD | CVG_DST_WRAP | CLR_ON_CVG | FORCE_BL | ZMODE_INTER | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)

RM_AA_ZB_XLU_LINE	=lambda clk: AA_EN | Z_CMP | IM_RD | CVG_DST_CLAMP | CVG_X_ALPHA | ALPHA_CVG_SEL | FORCE_BL | ZMODE_XLU | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)

RM_AA_ZB_DEC_LINE	=lambda clk: AA_EN | Z_CMP | IM_RD | CVG_DST_SAVE | CVG_X_ALPHA | ALPHA_CVG_SEL | FORCE_BL | ZMODE_DEC | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)

RM_AA_ZB_TEX_EDGE	=lambda clk: AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | CVG_X_ALPHA | ALPHA_CVG_SEL | ZMODE_OPA | TEX_EDGE | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)

RM_AA_ZB_TEX_INTER	=lambda clk: AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | CVG_X_ALPHA | ALPHA_CVG_SEL | ZMODE_INTER | TEX_EDGE | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)

RM_AA_ZB_SUB_SURF	=lambda clk: AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_FULL | ZMODE_OPA | ALPHA_CVG_SEL | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)

RM_AA_ZB_PCL_SURF	=lambda clk: AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | G_AC_DITHER | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)

RM_AA_ZB_OPA_TERR	=lambda clk: AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | ALPHA_CVG_SEL | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)

RM_AA_ZB_TEX_TERR	=lambda clk: AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | CVG_X_ALPHA | ALPHA_CVG_SEL | ZMODE_OPA | TEX_EDGE | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)

RM_AA_ZB_SUB_TERR	=lambda clk: AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_FULL | ZMODE_OPA | ALPHA_CVG_SEL | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)


RM_AA_OPA_SURF	=lambda clk: AA_EN | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | ALPHA_CVG_SEL | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)

RM_RA_OPA_SURF	=lambda clk: AA_EN | CVG_DST_CLAMP | ZMODE_OPA | ALPHA_CVG_SEL | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)

RM_AA_XLU_SURF	=lambda clk: AA_EN | IM_RD | CVG_DST_WRAP | CLR_ON_CVG | FORCE_BL | ZMODE_OPA | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)

RM_AA_XLU_LINE	=lambda clk: AA_EN | IM_RD | CVG_DST_CLAMP | CVG_X_ALPHA | ALPHA_CVG_SEL | FORCE_BL | ZMODE_OPA | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)

RM_AA_DEC_LINE	=lambda clk: AA_EN | IM_RD | CVG_DST_FULL | CVG_X_ALPHA | ALPHA_CVG_SEL | FORCE_BL | ZMODE_OPA | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)

RM_AA_TEX_EDGE	=lambda clk: AA_EN | IM_RD | CVG_DST_CLAMP | CVG_X_ALPHA | ALPHA_CVG_SEL | ZMODE_OPA | TEX_EDGE | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)

RM_AA_SUB_SURF	=lambda clk: AA_EN | IM_RD | CVG_DST_FULL | ZMODE_OPA | ALPHA_CVG_SEL | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)

RM_AA_PCL_SURF	=lambda clk: AA_EN | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | G_AC_DITHER | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)

RM_AA_OPA_TERR	=lambda clk: AA_EN | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | ALPHA_CVG_SEL | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)

RM_AA_TEX_TERR	=lambda clk: AA_EN | IM_RD | CVG_DST_CLAMP | CVG_X_ALPHA | ALPHA_CVG_SEL | ZMODE_OPA | TEX_EDGE | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)

RM_AA_SUB_TERR	=lambda clk: AA_EN | IM_RD | CVG_DST_FULL | ZMODE_OPA | ALPHA_CVG_SEL | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)


RM_ZB_OPA_SURF	=lambda clk: Z_CMP | Z_UPD | CVG_DST_FULL | ALPHA_CVG_SEL | ZMODE_OPA | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)

RM_ZB_XLU_SURF	=lambda clk: Z_CMP | IM_RD | CVG_DST_FULL | FORCE_BL | ZMODE_XLU | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)

RM_ZB_OPA_DECAL	=lambda clk: Z_CMP | CVG_DST_FULL | ALPHA_CVG_SEL | ZMODE_DEC | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)

RM_ZB_XLU_DECAL	=lambda clk: Z_CMP | IM_RD | CVG_DST_FULL | FORCE_BL | ZMODE_DEC | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)

RM_ZB_CLD_SURF	=lambda clk: Z_CMP | IM_RD | CVG_DST_SAVE | FORCE_BL | ZMODE_XLU | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)

RM_ZB_OVL_SURF	=lambda clk: Z_CMP | IM_RD | CVG_DST_SAVE | FORCE_BL | ZMODE_DEC | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)

RM_ZB_PCL_SURF	=lambda clk: Z_CMP | Z_UPD | CVG_DST_FULL | ZMODE_OPA | G_AC_DITHER | GBL_ck[clk](G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1)


RM_OPA_SURF	=lambda clk: CVG_DST_CLAMP | FORCE_BL | ZMODE_OPA | GBL_ck[clk](G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1)

RM_XLU_SURF	=lambda clk: IM_RD | CVG_DST_FULL | FORCE_BL | ZMODE_OPA | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)

RM_TEX_EDGE	=lambda clk: CVG_DST_CLAMP | CVG_X_ALPHA | ALPHA_CVG_SEL | FORCE_BL | ZMODE_OPA | TEX_EDGE | AA_EN | GBL_ck[clk](G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1)

RM_CLD_SURF	=lambda clk: IM_RD | CVG_DST_SAVE | FORCE_BL | ZMODE_OPA | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)

RM_PCL_SURF	=lambda clk: CVG_DST_FULL | FORCE_BL | ZMODE_OPA | G_AC_DITHER | GBL_ck[clk](G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1)

RM_ADD	=lambda clk: IM_RD | CVG_DST_SAVE | FORCE_BL | ZMODE_OPA | GBL_ck[clk](G_BL_CLR_IN, G_BL_A_FOG, G_BL_CLR_MEM, G_BL_1)

RM_NOOP	=lambda clk: GBL_ck[clk](0, 0, 0, 0)

RM_VISCVG	=lambda clk: IM_RD | FORCE_BL | GBL_ck[clk](G_BL_CLR_IN, G_BL_0, G_BL_CLR_BL, G_BL_A_MEM)

# for rendering to an 8-bit framebuffer
RM_OPA_CI	=lambda clk: CVG_DST_CLAMP | ZMODE_OPA | GBL_ck[clk](G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1)



G_RM_AA_ZB_OPA_SURF	= RM_AA_ZB_OPA_SURF(1)
G_RM_AA_ZB_OPA_SURF2	= RM_AA_ZB_OPA_SURF(2)
G_RM_AA_ZB_XLU_SURF	= RM_AA_ZB_XLU_SURF(1)
G_RM_AA_ZB_XLU_SURF2	= RM_AA_ZB_XLU_SURF(2)
G_RM_AA_ZB_OPA_DECAL	= RM_AA_ZB_OPA_DECAL(1)
G_RM_AA_ZB_OPA_DECAL2	= RM_AA_ZB_OPA_DECAL(2)
G_RM_AA_ZB_XLU_DECAL	= RM_AA_ZB_XLU_DECAL(1)
G_RM_AA_ZB_XLU_DECAL2	= RM_AA_ZB_XLU_DECAL(2)
G_RM_AA_ZB_OPA_INTER	= RM_AA_ZB_OPA_INTER(1)
G_RM_AA_ZB_OPA_INTER2	= RM_AA_ZB_OPA_INTER(2)
G_RM_AA_ZB_XLU_INTER	= RM_AA_ZB_XLU_INTER(1)
G_RM_AA_ZB_XLU_INTER2	= RM_AA_ZB_XLU_INTER(2)
G_RM_AA_ZB_XLU_LINE	= RM_AA_ZB_XLU_LINE(1)
G_RM_AA_ZB_XLU_LINE2	= RM_AA_ZB_XLU_LINE(2)
G_RM_AA_ZB_DEC_LINE	= RM_AA_ZB_DEC_LINE(1)
G_RM_AA_ZB_DEC_LINE2	= RM_AA_ZB_DEC_LINE(2)
G_RM_AA_ZB_TEX_EDGE	= RM_AA_ZB_TEX_EDGE(1)
G_RM_AA_ZB_TEX_EDGE2	= RM_AA_ZB_TEX_EDGE(2)
G_RM_AA_ZB_TEX_INTER	= RM_AA_ZB_TEX_INTER(1)
G_RM_AA_ZB_TEX_INTER2	= RM_AA_ZB_TEX_INTER(2)
G_RM_AA_ZB_SUB_SURF	= RM_AA_ZB_SUB_SURF(1)
G_RM_AA_ZB_SUB_SURF2	= RM_AA_ZB_SUB_SURF(2)
G_RM_AA_ZB_PCL_SURF	= RM_AA_ZB_PCL_SURF(1)
G_RM_AA_ZB_PCL_SURF2	= RM_AA_ZB_PCL_SURF(2)
G_RM_AA_ZB_OPA_TERR	= RM_AA_ZB_OPA_TERR(1)
G_RM_AA_ZB_OPA_TERR2	= RM_AA_ZB_OPA_TERR(2)
G_RM_AA_ZB_TEX_TERR	= RM_AA_ZB_TEX_TERR(1)
G_RM_AA_ZB_TEX_TERR2	= RM_AA_ZB_TEX_TERR(2)
G_RM_AA_ZB_SUB_TERR	= RM_AA_ZB_SUB_TERR(1)
G_RM_AA_ZB_SUB_TERR2	= RM_AA_ZB_SUB_TERR(2)

G_RM_RA_ZB_OPA_SURF	= RM_RA_ZB_OPA_SURF(1)
G_RM_RA_ZB_OPA_SURF2	= RM_RA_ZB_OPA_SURF(2)
G_RM_RA_ZB_OPA_DECAL	= RM_RA_ZB_OPA_DECAL(1)
G_RM_RA_ZB_OPA_DECAL2	= RM_RA_ZB_OPA_DECAL(2)
G_RM_RA_ZB_OPA_INTER	= RM_RA_ZB_OPA_INTER(1)
G_RM_RA_ZB_OPA_INTER2	= RM_RA_ZB_OPA_INTER(2)

G_RM_AA_OPA_SURF	= RM_AA_OPA_SURF(1)
G_RM_AA_OPA_SURF2	= RM_AA_OPA_SURF(2)
G_RM_AA_XLU_SURF	= RM_AA_XLU_SURF(1)
G_RM_AA_XLU_SURF2	= RM_AA_XLU_SURF(2)
G_RM_AA_XLU_LINE	= RM_AA_XLU_LINE(1)
G_RM_AA_XLU_LINE2	= RM_AA_XLU_LINE(2)
G_RM_AA_DEC_LINE	= RM_AA_DEC_LINE(1)
G_RM_AA_DEC_LINE2	= RM_AA_DEC_LINE(2)
G_RM_AA_TEX_EDGE	= RM_AA_TEX_EDGE(1)
G_RM_AA_TEX_EDGE2	= RM_AA_TEX_EDGE(2)
G_RM_AA_SUB_SURF	= RM_AA_SUB_SURF(1)
G_RM_AA_SUB_SURF2	= RM_AA_SUB_SURF(2)
G_RM_AA_PCL_SURF	= RM_AA_PCL_SURF(1)
G_RM_AA_PCL_SURF2	= RM_AA_PCL_SURF(2)
G_RM_AA_OPA_TERR	= RM_AA_OPA_TERR(1)
G_RM_AA_OPA_TERR2	= RM_AA_OPA_TERR(2)
G_RM_AA_TEX_TERR	= RM_AA_TEX_TERR(1)
G_RM_AA_TEX_TERR2	= RM_AA_TEX_TERR(2)
G_RM_AA_SUB_TERR	= RM_AA_SUB_TERR(1)
G_RM_AA_SUB_TERR2	= RM_AA_SUB_TERR(2)

G_RM_RA_OPA_SURF	= RM_RA_OPA_SURF(1)
G_RM_RA_OPA_SURF2	= RM_RA_OPA_SURF(2)

G_RM_ZB_OPA_SURF	= RM_ZB_OPA_SURF(1)
G_RM_ZB_OPA_SURF2	= RM_ZB_OPA_SURF(2)
G_RM_ZB_XLU_SURF	= RM_ZB_XLU_SURF(1)
G_RM_ZB_XLU_SURF2	= RM_ZB_XLU_SURF(2)
G_RM_ZB_OPA_DECAL	= RM_ZB_OPA_DECAL(1)
G_RM_ZB_OPA_DECAL2	= RM_ZB_OPA_DECAL(2)
G_RM_ZB_XLU_DECAL	= RM_ZB_XLU_DECAL(1)
G_RM_ZB_XLU_DECAL2	= RM_ZB_XLU_DECAL(2)
G_RM_ZB_CLD_SURF	= RM_ZB_CLD_SURF(1)
G_RM_ZB_CLD_SURF2	= RM_ZB_CLD_SURF(2)
G_RM_ZB_OVL_SURF	= RM_ZB_OVL_SURF(1)
G_RM_ZB_OVL_SURF2	= RM_ZB_OVL_SURF(2)
G_RM_ZB_PCL_SURF	= RM_ZB_PCL_SURF(1)
G_RM_ZB_PCL_SURF2	= RM_ZB_PCL_SURF(2)

G_RM_OPA_SURF	= RM_OPA_SURF(1)
G_RM_OPA_SURF2	= RM_OPA_SURF(2)
G_RM_XLU_SURF	= RM_XLU_SURF(1)
G_RM_XLU_SURF2	= RM_XLU_SURF(2)
G_RM_CLD_SURF	= RM_CLD_SURF(1)
G_RM_CLD_SURF2	= RM_CLD_SURF(2)
G_RM_TEX_EDGE	= RM_TEX_EDGE(1)
G_RM_TEX_EDGE2	= RM_TEX_EDGE(2)
G_RM_PCL_SURF	= RM_PCL_SURF(1)
G_RM_PCL_SURF2	= RM_PCL_SURF(2)
G_RM_ADD 	= RM_ADD(1)
G_RM_ADD2 	= RM_ADD(2)
G_RM_NOOP 	= RM_NOOP(1)
G_RM_NOOP2 	= RM_NOOP(2)
G_RM_VISCVG 	= RM_VISCVG(1)
G_RM_VISCVG2 	= RM_VISCVG(2)
G_RM_OPA_CI	= RM_OPA_CI(1)
G_RM_OPA_CI2	= RM_OPA_CI(2)


G_RM_FOG_SHADE_A	= GBL_c1(G_BL_CLR_FOG, G_BL_A_SHADE, G_BL_CLR_IN, G_BL_1MA)
G_RM_FOG_PRIM_A	= GBL_c1(G_BL_CLR_FOG, G_BL_A_FOG, G_BL_CLR_IN, G_BL_1MA)
G_RM_PASS	= GBL_c1(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


##define MAX(a, b)				((a) > (b) ? (a) : (b))
MAX = lambda a, b: a if a > b else b

##define MIN(a, b)				((a) > (b) ? (a) : (b))
MIN = lambda a, b: a if a < b else b

##define TXL2WORDS(txls, b_txl)	MAX(1, ((txls)*(b_txl)/8))
TXL2WORDS = lambda txls, b_txl: MAX(1, ((txls)*(b_txl)/8))

##define CALC_DXT(width, b_txl)	\
#		(((1 << G_TX_DXT_FRAC) + TXL2WORDS(width, b_txl) - 1) / \
#					TXL2WORDS(width, b_txl))
CALC_DXT = lambda width, b_txl: (((1 << G_TX_DXT_FRAC) + TXL2WORDS(width, b_txl) - 1) / TXL2WORDS(width, b_txl))

##define _SHIFTL(v, s, w)	\
#    ((u32) (((u32)(v) & ((0x01 << (w)) - 1)) << (s)))

_SHIFTL = lambda v, s, w: (v & ((1 << w)-1)) << s

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define	gsImmp1(c, p0)							\
#{									\
#	_SHIFTL((c), 24, 8), (unsigned int)(p0)				\
#}
gImmp1 = lambda c, p0: (_SHIFTL((c), 24, 8) << 32)|(p0)

# define	gsSPVertex(v, n, v0)					\
#{									\
#	(_SHIFTL(G_VTX,24,8)|_SHIFTL((n),12,8)|_SHIFTL((v0)+(n),1,7)),	\
#        (unsigned int)(v)						\
#}
gSPVertex = lambda v, n, v0: ((_SHIFTL(G_VTX,24,8)|_SHIFTL((n),12,8)|_SHIFTL((v0)+(n),1,7)) << 32)|(v)

##define	gSPGeometryMode(pkt, c, s)					\
#{									\
#	Gfx *_g = (Gfx *)(pkt);						\
#	_g->words.w0 = _SHIFTL(G_GEOMETRYMODE,24,8)|_SHIFTL(~(u32)(c),0,24);\
#	_g->words.w1 = (u32)(s);					\
#}

GeometryMode = lambda c, s: ((_SHIFTL(G_GEOMETRYMODE,24,8)|_SHIFTL((~c)&0xFFFFFFFF,0,24)) << 32)|(s&0xFFFFFFFF)

#define gsSPEndDisplayList()						\
#{									\
#	_SHIFTL(G_ENDDL, 24, 8), 0					\
#}

EndDisplayList = _SHIFTL(G_ENDDL, 24, 8) << 32

##define	gsDPSetPrimColor(m, l, r, g, b, a)				\
#{									\
#	(_SHIFTL(G_SETPRIMCOLOR, 24, 8) | _SHIFTL(m, 8, 8) |		\
#	 _SHIFTL(l, 0, 8)),						\
#	(_SHIFTL(r, 24, 8) | _SHIFTL(g, 16, 8) | _SHIFTL(b, 8, 8) |	\
#	 _SHIFTL(a, 0, 8))						\
#}
SetPrimColor = lambda m, l, r, g, b, a: (((_SHIFTL(G_SETPRIMCOLOR, 24, 8) | _SHIFTL(m, 8, 8) | _SHIFTL(l, 0, 8))<<32)|
     (_SHIFTL(r, 24, 8) | _SHIFTL(g, 16, 8) | _SHIFTL(b, 8, 8) | _SHIFTL(a, 0, 8)))
##define	gsDPSetColor(c, d)						\
#{									\
#	_SHIFTL(c, 24, 8), (unsigned int)(d)				\
#}
SetColor = lambda c, d: (_SHIFTL(c, 24, 8)<<32)|(d&0xFFFFFFFF)

##define	sDPRGBColor(cmd, r, g, b, a)					\
#	    gsDPSetColor(cmd,						\
#			 (_SHIFTL(r, 24, 8) | _SHIFTL(g, 16, 8) | 	\
#			  _SHIFTL(b, 8, 8) | _SHIFTL(a, 0, 8)))
#
PRGBColor = lambda cmd, r, g, b, a: SetColor(cmd, (_SHIFTL(r, 24, 8) | _SHIFTL(g, 16, 8) | _SHIFTL(b, 8, 8) | _SHIFTL(a, 0, 8)))

##define	gsDPSetEnvColor(r, g, b, a)					\
#            sDPRGBColor(G_SETENVCOLOR, r,g,b,a)
SetEnvColor = lambda r,g,b,a: PRGBColor(G_SETENVCOLOR, r,g,b,a)

##define	gsDPSetBlendColor(r, g, b, a)					\
#            sDPRGBColor(G_SETBLENDCOLOR, r,g,b,a)
SetBlendColor = lambda r,g,b,a: PRGBColor(G_SETBLENDCOLOR, r,g,b,a)

##define	gsDPSetFogColor(r, g, b, a)					\
#            sDPRGBColor(G_SETFOGCOLOR, r,g,b,a)
SetFogColor = lambda r,g,b,a: PRGBColor(G_SETFOGCOLOR, r,g,b,a)

##define	gsDPSetFillColor(d)						\
#            gsDPSetColor(G_SETFILLCOLOR, (d))

SetFillColor = lambda d: SetColor(G_SETFILLCOLOR, d)
#define	gsDPNoParam(cmd)						\
#{									\
#	_SHIFTL(cmd, 24, 8), 0						\
#}
NoParam = lambda cmd: (_SHIFTL(cmd, 24, 8)<<32) | 0

#define	gsDPParam(cmd, param)						\
#{									\
#	_SHIFTL(cmd, 24, 8), (param)					\
#}
Param = lambda cmd, param: (_SHIFTL(cmd, 24, 8)<<32) | (param)

#define	gsDPFullSync()		gsDPNoParam(G_RDPFULLSYNC)
FullSync = NoParam(G_RDPFULLSYNC)
#define	gsDPTileSync()		gsDPNoParam(G_RDPTILESYNC)
TileSync = NoParam(G_RDPTILESYNC)
#define	gsDPPipeSync()		gsDPNoParam(G_RDPPIPESYNC)
PipeSync = NoParam(G_RDPPIPESYNC)
#define	gsDPLoadSync()		gsDPNoParam(G_RDPLOADSYNC)
LoadSync = NoParam(G_RDPLOADSYNC)
#define	gsDPNoOp()		gsDPNoParam(G_NOOP)
NoOp = NoParam(G_NOOP)
#define	gsDPNoOpTag(tag)	gsDPParam(G_NOOP, tag)
NoOpTag = lambda tag: Param(G_NOOP, tag)

##define	gsSetImage(cmd, fmt, siz, width, i)				\
#{									\
#	_SHIFTL(cmd, 24, 8) | _SHIFTL(fmt, 21, 3) |			\
#	_SHIFTL(siz, 19, 2) | _SHIFTL((width)-1, 0, 12),		\
#	(unsigned int)(i)						\
#}
SetImage = lambda cmd, fmt, siz, width, i: ((_SHIFTL(cmd, 24, 8) | _SHIFTL(fmt, 21, 3) | _SHIFTL(siz, 19, 2) | _SHIFTL((width)-1, 0, 12)) << 32)|(i)

##define	gsDPSetColorImage(f, s, w, i)		gsSetImage(G_SETCIMG, f, s, w, i)
SetColorImage = lambda f, s, w, i: SetImage(G_SETCIMG, f, s, w, i)

##define	gsDPSetTextureImage(f, s, w, i)	gsSetImage(G_SETTIMG, f, s, w, i)
SetTextureImage = lambda f, s, w, i: SetImage(G_SETTIMG, f, s, w, i)

##define	gsDPSetTile(fmt, siz, line, tmem, tile, palette, cmt,		\
#		maskt, shiftt, cms, masks, shifts)			\
#{									\
#	(_SHIFTL(G_SETTILE, 24, 8) | _SHIFTL(fmt, 21, 3) | 		\
#	 _SHIFTL(siz, 19, 2) | _SHIFTL(line, 9, 9) | _SHIFTL(tmem, 0, 9)),\
#        (_SHIFTL(tile, 24, 3) | _SHIFTL(palette, 20, 4) | 		\
#	 _SHIFTL(cmt, 18, 2) | _SHIFTL(maskt, 14, 4) | 			\
#	 _SHIFTL(shiftt, 10, 4) | _SHIFTL(cms, 8, 2) | 			\
#	 _SHIFTL(masks, 4, 4) | _SHIFTL(shifts, 0, 4))			\
#}
SetTile = lambda fmt, siz, line, tmem, tile, palette, cmt, maskt, shiftt, cms, masks, shifts: (((_SHIFTL(G_SETTILE, 24, 8) | _SHIFTL(fmt, 21, 3) | _SHIFTL(siz, 19, 2) |
	   _SHIFTL(line, 9, 9) | _SHIFTL(tmem, 0, 9)) << 32 ) |
	 (_SHIFTL(tile, 24, 3) | _SHIFTL(palette, 20, 4) | _SHIFTL(cmt, 18, 2) |
	  _SHIFTL(maskt, 14, 4) | _SHIFTL(shiftt, 10, 4) | _SHIFTL(cms, 8, 2)  |	
	  _SHIFTL(masks, 4, 4) | _SHIFTL(shifts, 0, 4) ))

##define gsDPLoadBlock(tile, uls, ult, lrs, dxt)				\
#{									\
#	(_SHIFTL(G_LOADBLOCK, 24, 8) | _SHIFTL(uls, 12, 12) | 		\
#	 _SHIFTL(ult, 0, 12)),						\
#	(_SHIFTL(tile, 24, 3) | 					\
#	 _SHIFTL((MIN(lrs,G_TX_LDBLK_MAX_TXL)), 12, 12) |		\
#	 _SHIFTL(dxt, 0, 12))						\
#}
LoadBlock = lambda tile, uls, ult, lrs, dxt: ( ((_SHIFTL(G_LOADBLOCK, 24, 8) | _SHIFTL(uls, 12, 12) | _SHIFTL(ult, 0, 12)) << 32) |
	 (_SHIFTL(tile, 24, 3) | _SHIFTL((MIN(lrs,G_TX_LDBLK_MAX_TXL)), 12, 12) |
	 _SHIFTL(dxt, 0, 12)))

#define gsDPLoadTileGeneric(c, tile, uls, ult, lrs, lrt)		\
#{									\
#	_SHIFTL(c, 24, 8) | _SHIFTL(uls, 12, 12) | _SHIFTL(ult, 0, 12),	\
#	_SHIFTL(tile, 24, 3) | _SHIFTL(lrs, 12, 12) | _SHIFTL(lrt, 0, 12)\
#}
LoadTileGeneric = lambda c, tile, uls, ult, lrs, lrt: (((_SHIFTL(c, 24, 8) | _SHIFTL(uls, 12, 12) | _SHIFTL(ult, 0, 12))<<32)|
	 _SHIFTL(tile, 24, 3) | _SHIFTL(lrs, 12, 12) | _SHIFTL(lrt, 0, 12))

##define	gsDPSetTileSize(t, uls, ult, lrs, lrt)				\
#		gsDPLoadTileGeneric(G_SETTILESIZE, t, uls, ult, lrs, lrt)
SetTileSize = lambda t, uls, ult, lrs, lrt: LoadTileGeneric(G_SETTILESIZE, t, uls, ult, lrs, lrt)

##define	gsDPLoadTile(t, uls, ult, lrs, lrt)				\
#		gsDPLoadTileGeneric(G_LOADTILE, t, uls, ult, lrs, lrt)
LoadTile = lambda t, uls, ult, lrs, lrt: LoadTileGeneric(G_LOADTILE, t, uls, ult, lrs, lrt)


#timg is the location of the texture image in DRAM. It must be 64-bit aligned

#fmt is the texture image format (G_IM_FMT_RGBA, G_IM_FMT_YUV, G_IM_FMT_CI,
# G_IM_FMT_IA, G_IM_FMT_I)

#siz is the texel size (G_IM_SIZ_4b, G_IM_SIZ_8b, G_IM_SIZ_16b, G_IM_SIZ_32b)

#width, height specify the texture image width and height in texels. When you
# load a texture tile, specify the width and height of the original larger
# texture image in DRAM

#uls, ult specify the upper-left s and t coordinates of a texture tile in the
# source image

#lrs, lrt specify the lower-right s and t coordinates of a texture tile in
# the source image

#pal for indexed textures, this specifies which texture lookup table palette
# to use

#cms, cmt gives the clamp, wrap, and mirror flag for the s and t axis
# (G_TX_NOMIRROR, G_TX_WRAP, G_TX_MIRROR, G_TX_CLAMP)

#masks, maskt specify the mask for the s and t axis (G_TX_NOMASK or a number
# specifying where to wrap)

#shifts, shiftt gives the shift value for the s and t coordinates for lower
# level of detail textures. Use G_TX_NOLOD for no shifts

#tile is the tile number for gDPLoadBlock.

#dxt is the t increment value per scanline for gDPLoadBlock

##define	gsDPLoadTextureBlock(timg, fmt, siz, width, height,		\
#		pal, cms, cmt, masks, maskt, shifts, shiftt)		\
#									\
#	gsDPSetTextureImage(fmt, siz##_LOAD_BLOCK, 1, timg),		\
#	gsDPSetTile(fmt, siz##_LOAD_BLOCK, 0, 0, 			\
#		G_TX_LOADTILE, 	0 , cmt, maskt,	shiftt, cms, 		\
#		masks, shifts),						\
#	gsDPLoadSync(),							\
#	gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 				\
#		(((width)*(height) + siz##_INCR) >> siz##_SHIFT)-1,	\
#		CALC_DXT(width, siz##_BYTES)), 				\
#	gsDPPipeSync(),							\
#	gsDPSetTile(fmt, siz, ((((width) * siz##_LINE_BYTES)+7)>>3), 0,	\
#		G_TX_RENDERTILE, pal, cmt, maskt, shiftt, cms, masks,	\
#		shifts),						\
#	gsDPSetTileSize(G_TX_RENDERTILE, 0, 0,				\
#		((width)-1) << G_TEXTURE_IMAGE_FRAC,			\
#		((height)-1) << G_TEXTURE_IMAGE_FRAC)
def LoadTextureBlock(timg, fmt, siz, width, height, pal, cms, cmt, masks, maskt, shifts, shiftt):
	l = []
	l.append(SetTextureImage(fmt, G_IM_LOAD_BLOCK[siz], 1, timg))
	l.append(SetTile(fmt, G_IM_LOAD_BLOCK[siz], 0, 0, G_TX_LOADTILE, 0, cmt, maskt, shiftt, cms, masks, shifts))
	l.append(LoadSync)
	l.append(LoadBlock( G_TX_LOADTILE, 0, 0, (((width * height) + G_IM_INCR[siz]) >> G_IM_SHIFT[siz])-1, CALC_DXT(width, G_IM_BYTES[siz]) ))
	l.append(PipeSync)
	l.append(SetTile( fmt, siz, ((((width) * G_IM_LINE_BYTES[siz])+7)>>3), 0, G_TX_RENDERTILE, pal, cmt, maskt, shiftt, cms, masks, shifts))
	l.append(SetTileSize( G_TX_RENDERTILE, 0, 0, (width-1) << G_TEXTURE_IMAGE_FRAC, (height-1) << G_TEXTURE_IMAGE_FRAC ) )
	return dwords2string(l)

##define	gsDPLoadTLUTCmd(tile, count)					\
#{									\
#	_SHIFTL(G_LOADTLUT, 24, 8),					\
#	_SHIFTL((tile), 24, 3) | _SHIFTL((count), 14, 10)		\
#}
LoadTLUTCmd = lambda tile, count: (_SHIFTL(G_LOADTLUT, 24, 8)<<32)|_SHIFTL((tile), 24, 3) | _SHIFTL((count), 14, 10)

#define gsDPLoadTLUT_pal16(pal, dram)					\
# 									\
#	gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dram),	\
#	gsDPTileSync(),							\
#	gsDPSetTile(0, 0, 0, (256+(((pal)&0xf)*16)),			\
#		G_TX_LOADTILE, 0 , 0, 0, 0, 0, 0, 0),			\
#	gsDPLoadSync(),							\
#	gsDPLoadTLUTCmd(G_TX_LOADTILE, 15),				\
#	gsDPPipeSync()
def LoadTLUT_pal16(pal, dram):
    l = []
    l.append(SetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dram))
    l.append(TileSync)
    l.append(SetTile(0, 0, 0, (256+(((pal)&0xf)*16)), G_TX_LOADTILE, 0 , 0, 0, 0, 0, 0, 0))
    l.append(LoadSync)
    l.append(LoadTLUTCmd(G_TX_LOADTILE, 15))
    l.append(PipeSync)
    return dwords2string(l)

##define gsDPLoadTLUT_pal256(dram)					\
# 									\
#	gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dram),	\
#	gsDPTileSync(),							\
#	gsDPSetTile(0, 0, 0, 256,					\
#		G_TX_LOADTILE, 0 , 0, 0, 0, 0, 0, 0),			\
#	gsDPLoadSync(),							\
#	gsDPLoadTLUTCmd(G_TX_LOADTILE, 255),				\
#	gsDPPipeSync()
def LoadTLUT_pal256( dram):
    l = []
    l.append(SetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dram))
    l.append(TileSync)
    l.append(SetTile(0, 0, 0, 256, G_TX_LOADTILE, 0 , 0, 0, 0, 0, 0, 0))
    l.append(LoadSync)
    l.append(LoadTLUTCmd(G_TX_LOADTILE, 255))
    l.append(PipeSync)
    return dwords2string(l)

## define gsSPTexture(s, t, level, tile, on)				\
#{									\
#	(_SHIFTL(G_TEXTURE,24,8) | _SHIFTL(BOWTIE_VAL,16,8) |		\
#	 _SHIFTL((level),11,3) | _SHIFTL((tile),8,3) | _SHIFTL((on),1,7)),\
#        (_SHIFTL((s),16,16) | _SHIFTL((t),0,16))			\
#}
Texture = lambda s, t, level, tile, on: ((_SHIFTL(G_TEXTURE,24,8) | _SHIFTL(BOWTIE_VAL,16,8) | _SHIFTL((level),11,3) | _SHIFTL((tile),8,3) | _SHIFTL((on),1,7))<<32)|(_SHIFTL((s),16,16) | _SHIFTL((t),0,16))

##define	gsDPSetCombine(muxs0, muxs1)					\
#{									\
#	_SHIFTL(G_SETCOMBINE, 24, 8) | _SHIFTL(muxs0, 0, 24),		\
#	(unsigned int)(muxs1)						\
#}
SetCombine = lambda muxs0, muxs1: ((_SHIFTL(G_SETCOMBINE, 24, 8) | _SHIFTL(muxs0, 0, 24))<<32)|(muxs1&0xFFFFFFFF)

##define	gsSPSetOtherMode(cmd, sft, len, data)				\
#{									\
#	_SHIFTL(cmd,24,8)|_SHIFTL(32-(sft)-(len),8,8)|_SHIFTL((len)-1,0,8), \
#	(unsigned int)(data)						\
#}
SetOtherMode = lambda cmd, sft, len_, data: ((_SHIFTL(cmd,24,8)|_SHIFTL(32-(sft)-(len_),8,8)|_SHIFTL((len_)-1,0,8))<<32)|(data&0xFFFFFFFF)


# RDP setothermode register commands - register shadowed in RSP

PipelineMode = lambda mode : SetOtherMode(G_SETOTHERMODE_H, G_MDSFT_PIPELINE, 1, mode)

SetCycleType = lambda type_ : SetOtherMode(G_SETOTHERMODE_H, G_MDSFT_CYCLETYPE, 2, type_)

SetTexturePersp = lambda type_ : SetOtherMode(G_SETOTHERMODE_H, G_MDSFT_TEXTPERSP, 1, type_)

SetTextureDetail = lambda type_ : SetOtherMode(G_SETOTHERMODE_H, G_MDSFT_TEXTDETAIL, 2, type_)

SetTextureLOD = lambda type_ : SetOtherMode(G_SETOTHERMODE_H, G_MDSFT_TEXTLOD, 1, type_)

SetTextureLUT = lambda type_ : SetOtherMode(G_SETOTHERMODE_H, G_MDSFT_TEXTLUT, 2, type_)

SetTextureFilter = lambda type_ : SetOtherMode(G_SETOTHERMODE_H, G_MDSFT_TEXTFILT, 2, type_)

SetTextureConvert = lambda type_ : SetOtherMode(G_SETOTHERMODE_H, G_MDSFT_TEXTCONV, 3, type_)

SetCombineKey = lambda type_ : SetOtherMode(G_SETOTHERMODE_H, G_MDSFT_COMBKEY, 1, type_)

SetColorDither = lambda mode: SetOtherMode(G_SETOTHERMODE_H, G_MDSFT_COLORDITHER, 1, mode)

SetBlendMask = lambda mask : NoOp()

SetAlphaCompare = lambda type_ : SetOtherMode(G_SETOTHERMODE_L, G_MDSFT_ALPHACOMPARE, 2, type_)

SetDepthSource = lambda src : SetOtherMode(G_SETOTHERMODE_L, G_MDSFT_ZSRCSEL, 1, src)

SetRenderMode = lambda c0, c1 : SetOtherMode(G_SETOTHERMODE_L, G_MDSFT_RENDERMODE, 29, (c0) | (c1))


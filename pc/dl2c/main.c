#include "global.h"

#ifdef DEBUG
 #define VERSION	"0.0.0 (Debug build)"
#else
 #define VERSION	"0.0.0"
#endif

unsigned char * RAMSeg[16] = {
NULL, NULL, NULL, NULL,
NULL, NULL, NULL, NULL,
NULL, NULL, NULL, NULL,
NULL, NULL, NULL, NULL
};

char * G_IM_SIZ[] = {
 "G_IM_SIZ_4b",
 "G_IM_SIZ_8b",
 "G_IM_SIZ_16b",
 "G_IM_SIZ_32b",
 "G_IM_SIZ_UNUSED",
 "G_IM_SIZ_DD"
};

char * G_ONOFF[] = {
 "G_OFF",
 "G_ON"
};

char * G_IM_FMT[] = {
 "G_IM_FMT_RGBA",
 "G_IM_FMT_YUV",
 "G_IM_FMT_CI",
 "G_IM_FMT_IA",
 "G_IM_FMT_I"
};

FILE * output;

char VtxListC[0x2000];

void DumpVtxList(int offset, int n)
{
	int i, seg = (offset >> 24) & 0xFF;
	Vertex vtx;
	if (RAMSeg[seg] == NULL)
		sprintf(VtxListC, "/* RAM segment %#02x not loaded */", seg);
	else {
		sprintf(VtxListC, "");
		for (i=0,offset&=0xFFFFFF;i<n;i++,offset+=16){
			getVtx(RAMSeg[seg], offset, vtx);
			sprintf(VtxListC, "%s{%6i, %6i, %6i, 0, %6i, %6i, %3i, %3i, %3i, %3i},\n", VtxListC, vtx.x, vtx.y, vtx.z, vtx.s, vtx.t, vtx.r, vtx.g, vtx.b, vtx.a);
		}
	}
}

void DumpDisplayList( DLCommand DisplayList[], int offset)
{	
	int DLStack[32];
	int DLStackCount=0;
	int DLPos = offset>>3;
	
	C_DL C_DisplayLists[32];
	int C_DLNo = 0, C_DLCount = 0;
	int C_DLStack[32];
	
	int ExecuteDisplayList = true;
	
	int timg, fmt, siz, width, height, pal, cms, cmt, masks, maskt, shifts, shiftt;
	int ignore, w0, w1;
	
	unsigned char r, g, b, a, m, l;
	unsigned char v0, v1, v2, v3, v4, v5, flag0, flag1;
	int vstart, vend;
	
	int Target;
	int i;
	int handled;
	
	int seg;
	
	int s, t, level, tile, on, BOWTIE_VAL = 0;
	
	int n, v, vtxListCount = 0;
	
	char flags[256];
	
	char C_Textures[0x140500];	//enough to hold 64 64x64 5551 textures. could probably be reduced.
	unsigned int ConvTex[96];	//so the same texture is not converted twice
	unsigned char TexCount=0;
	char DumpTex = 0;
	
	sprintf(C_Textures, "");
	sprintf(C_DisplayLists[0].text, "Gfx dl_%08X[] = {\t/* Main display list */\n", offset);
	
	while (ExecuteDisplayList)
	{
		handled = 0;
		switch(DisplayList[DLPos].b[0]) {
		case G_SETCIMG:		/* 0xff /  -1 */
			break;
		case G_SETZIMG:		/* 0xfe /  -2 */
			break;
		case G_SETTIMG:		/* 0xfd /  -3 */
		{
			if ((DisplayList[DLPos+1].b[0] == G_SETTILE) &&
			    (DisplayList[DLPos+2].b[0] == G_RDPLOADSYNC) &&
			    (DisplayList[DLPos+3].b[0] == G_LOADBLOCK) &&
			    (DisplayList[DLPos+4].b[0] == G_RDPPIPESYNC) &&
			    (DisplayList[DLPos+5].b[0] == G_SETTILE) &&
			    (DisplayList[DLPos+6].b[0] == G_SETTILESIZE))
			{
				/* gsDPLoadTextureBlock */
				
				/* get needed values */
				dgsSetTextureImage(Flip32(DisplayList[DLPos].w[0]), Flip32(DisplayList[DLPos].w[1]),
					fmt, ignore, ignore, timg);
				dgsDPSetTile(Flip32(DisplayList[DLPos+1].w[0]), Flip32(DisplayList[DLPos+1].w[1]),
					ignore, ignore, ignore, ignore, ignore, ignore, cmt, maskt, shiftt, cms, masks, shifts);
				dgsDPSetTile(Flip32(DisplayList[DLPos+5].w[0]), Flip32(DisplayList[DLPos+5].w[1]),
					ignore, siz, ignore, ignore, ignore, pal, ignore, ignore, ignore, ignore, ignore, ignore);
				dgsDPSetTileSize(Flip32(DisplayList[DLPos+6].w[0]), Flip32(DisplayList[DLPos+6].w[1]),
					ignore, ignore, ignore, width, height);
				width >>= G_TEXTURE_IMAGE_FRAC;
				width++;
				height >>= G_TEXTURE_IMAGE_FRAC;
				height++;
				
				sprintf(flags, "");
				/* cms,cmt,maskt,masks,shiftt,shifts flags */
				if(!cms)
					sprintf(flags, "G_TX_WRAP|G_TX_NOMIRROR");
				else if(cms & 1) {
					sprintf(flags, "G_TX_MIRROR");
					if (cms &2)
						sprintf(flags, "%s|G_TX_CLAMP", flags);
				}
				else if (cms &2)
					sprintf(flags, "G_TX_CLAMP");
				
				sprintf(flags, "%s, ", flags);
				
				if(!cmt)
					sprintf(flags, "%sG_TX_WRAP|G_TX_NOMIRROR", flags);
				else if(cmt & 1) {
					sprintf(flags, "%sG_TX_MIRROR", flags);
					if (cmt &2)
						sprintf(flags, "%s|G_TX_CLAMP", flags);
				}
				else if (cmt &2)
					sprintf(flags, "%sG_TX_CLAMP", flags);
				
				sprintf(flags, "%s,\n\t\t", flags);
				if (!masks)
					sprintf(flags, "%sG_TX_NOMASK, ", flags);
				else
					sprintf(flags, "%s%i, ", flags, masks);
				
				if (!maskt)
					sprintf(flags, "%sG_TX_NOMASK, ", flags);
				else
					sprintf(flags, "%s%i, ", flags, maskt);
				
				if (!shifts)
					sprintf(flags, "%sG_TX_NOLOD, ", flags);
				else
					sprintf(flags, "%s%i, ", flags, shifts);
				
				if (!shiftt)
					sprintf(flags, "%sG_TX_NOLOD", flags);
				else
					sprintf(flags, "%s%i", flags, shiftt);
				
				/* kk */
				char texName[32];
				sprintf(texName, "0x%08X", timg);
				
/*
#define G_IM_SIZ_4b	0
#define G_IM_SIZ_8b	1
#define G_IM_SIZ_16b	2
#define G_IM_SIZ_32b	3
#define G_IM_SIZ_DD	5
*/
				//Dump texture?
				DumpTex=1;
				for(i=0;i<TexCount;i++){
					if(ConvTex[i] == timg) {
						DumpTex=0;
						break;
					}
					
				}
				if(DumpTex)
				{
					ConvTex[TexCount] = timg;
					TexCount++;
				}
				
				int NPixels = (width * height);
				
				//Change texture name if texture has/will be dumped, dump texture if needed.
				DumpTexture(timg, NPixels, fmt, siz, C_Textures, texName, DumpTex);
				
				sprintf(C_DisplayLists[C_DLNo].text, "%s\tgsDPLoadTextureBlock(%s, %s, %s, %i, %i, %i,\n\t\t%s),\n", C_DisplayLists[C_DLNo].text,
					texName, G_IM_FMT[fmt], G_IM_SIZ[siz], width, height, pal, flags);
				DLPos+=6;
				handled = 1;
			}
			break;
		}
		case G_SETCOMBINE:	/* 0xfc /  -4 */
			w0 = Flip32(DisplayList[DLPos].w[0]);
			w1 = Flip32(DisplayList[DLPos].w[1]);
			char colorCombined[256];
			ColorCombiner(w0,  w1, colorCombined);
			sprintf(C_DisplayLists[C_DLNo].text, "%s%s,\n", C_DisplayLists[C_DLNo].text, colorCombined);
			handled = 1;
			break;
		case G_SETENVCOLOR:	/* 0xfb /  -5 */
		{
			w1 = Flip32(DisplayList[DLPos].w[1]);
			dDPRGBColor(0, w1, r, g, b, a);
			sprintf(C_DisplayLists[C_DLNo].text, "%s\tgsDPSetEnvColor(%i, %i, %i, %i),\n", C_DisplayLists[C_DLNo].text, r, g, b, a);
			handled = 1;
			break;
		}
		case G_SETPRIMCOLOR:	/* 0xfa /  -6 */
			w0 = Flip32(DisplayList[DLPos].w[0]);
			w1 = Flip32(DisplayList[DLPos].w[1]);
			dgsDPSetPrimColor(w0, w1, m, l, r, g, b, a)
			sprintf(C_DisplayLists[C_DLNo].text, "%s\tgsDPSetPrimColor(%i, %i, %i, %i, %i, %i),\n", C_DisplayLists[C_DLNo].text, m, l, r, g, b, a);
			handled = 1;
			break;
		case G_SETBLENDCOLOR:	/* 0xf9 /  -7 */
		{
			w1 = Flip32(DisplayList[DLPos].w[1]);
			dDPRGBColor(0, w1, r, g, b, a);
			sprintf(C_DisplayLists[C_DLNo].text, "%s\tgsDPSetBlendColor(%i, %i, %i, %i),\n", C_DisplayLists[C_DLNo].text, r, g, b, a);
			handled = 1;
			break;
		}
		case G_SETFOGCOLOR:	/* 0xf8 /  -8 */
		{
			w1 = Flip32(DisplayList[DLPos].w[1]);
			dDPRGBColor(0, w1, r, g, b, a);
			sprintf(C_DisplayLists[C_DLNo].text, "%s\tgsDPSetFogColor(%i, %i, %i, %i),\n", C_DisplayLists[C_DLNo].text, r, g, b, a);
			handled = 1;
			break;
		}
		case G_SETFILLCOLOR:	/* 0xf7 /  -9 */
		{
			w1 = Flip32(DisplayList[DLPos].w[1]);
			dDPRGBColor(0, w1, r, g, b, a);
			sprintf(C_DisplayLists[C_DLNo].text, "%s\tgsDPSetFillColor(%i, %i, %i, %i),\n", C_DisplayLists[C_DLNo].text, r, g, b, a);
			handled = 1;
			break;
		}
		case G_FILLRECT:	/* 0xf6 / -10 */
			break;
		case G_SETTILE:		/* 0xf5 / -11 */
			break;
		case G_LOADTILE:	/* 0xf4 / -12 */
			break;
		case G_LOADBLOCK:	/* 0xf3 / -13 */
			break;
		case G_SETTILESIZE:	/* 0xf2 / -14 */
			break;
		case G_LOADTLUT:	/* 0xf0 / -16 */
			break;
		case G_RDPSETOTHERMODE:	/* 0xef / -17 */
			break;
		case G_SETPRIMDEPTH:	/* 0xee / -18 */
			break;
		case G_SETSCISSOR:	/* 0xed / -19 */
			break;
		case G_SETCONVERT:	/* 0xec / -20 */
			break;
		case G_SETKEYR:		/* 0xeb / -21 */
			break;
		case G_SETKEYGB:	/* 0xea / -22 */
			break;
		case G_RDPFULLSYNC:	/* 0xe9 / -23 */
			sprintf(C_DisplayLists[C_DLNo].text, "%s\tgsDPFullSync(),\n", C_DisplayLists[C_DLNo].text);
			handled = 1;
			break;
		case G_RDPTILESYNC:	/* 0xe8 / -24 */
			sprintf(C_DisplayLists[C_DLNo].text, "%s\tgsDPTileSync(),\n", C_DisplayLists[C_DLNo].text);
			handled = 1;
			break;
		case G_RDPPIPESYNC:	/* 0xe7 / -25 */
			sprintf(C_DisplayLists[C_DLNo].text, "%s\tgsDPPipeSync(),\n", C_DisplayLists[C_DLNo].text);
			handled = 1;
			break;
		case G_RDPLOADSYNC:	/* 0xe6 / -26 */
			sprintf(C_DisplayLists[C_DLNo].text, "%s\tgsDPLoadSync(),\n", C_DisplayLists[C_DLNo].text);
			handled = 1;
			break;
		case G_TEXRECTFLIP:	/* 0xe5 / -27 */
			break;
		case G_TEXRECT:		/* 0xe4 / -28 */
			break;
		/* F3DEX2 speical */
		case G_NOOP:		/* 0x00 */
			break;
		case G_RDPHALF_2:	/* 0xf1 */
			break;
		case G_SETOTHERMODE_H:	/* 0xe3 */
			break;
		case G_SETOTHERMODE_L:	/* 0xe2 */
			break;
		case G_RDPHALF_1:	/* 0xe1 */
			break;
		case G_SPNOOP:		/* 0xe0 */
			break;
		case G_ENDDL:		/* 0xdf */
			sprintf(C_DisplayLists[C_DLNo].text, "%s\tgsSPEndDisplayList(),\n};\n\n", C_DisplayLists[C_DLNo].text);
			if (!DLStackCount)
				ExecuteDisplayList = false;
			else
			{
				DLStackCount--;
				C_DLNo = C_DLStack[DLStackCount];
				DLPos = DLStack[DLStackCount];
			}
			handled = 1;
			break;
		case G_DL:		/* 0xde */
			Target = Flip32(DisplayList[DLPos].w[1]);
			sprintf(C_DisplayLists[C_DLNo].text, "%s\tgsSPDisplayList( dl_%08X ),\n", C_DisplayLists[C_DLNo].text, Target);
			
			for (i=0;i<sizeof(C_DisplayLists)/sizeof(C_DL);i++) {
				if(C_DisplayLists[i].start == Target) {
					Target = 0;
					break;
				}
			}
			
			if (Target) {
				C_DLStack[DLStackCount] = C_DLNo;
				C_DLCount++;
				C_DLNo = C_DLCount;
				C_DisplayLists[C_DLNo].start = Target;
				
				DLStack[DLStackCount] = DLPos;
				DLPos = (Target&0xFFFFFF) >> 3;
				DLStackCount++;
				sprintf(C_DisplayLists[C_DLNo].text, "%s\nGfx dl_%08X[] = {\n", C_DisplayLists[C_DLNo].text, Target);
			}
			handled = 1;
			break;
		case G_LOAD_UCODE:	/* 0xdd */
			break;
		case G_MOVEMEM:		/* 0xdc */
			break;
		case G_MOVEWORD:	/* 0xdb */
			break;
		case G_MTX:		/* 0xda */
			break;
		case G_GEOMETRYMODE:	/* 0xd9 */
			w0 = 0xFFFFFF & (~Flip32(DisplayList[DLPos].w[0]));
			w1 = Flip32(DisplayList[DLPos].w[1]);
			sprintf(flags, "");
			if (!w0)	//gsSPSetGeometryMode();
			{
				if (w1 & G_ZBUFFER) sprintf(flags, "%sG_ZBUFFER|", flags);
				if (w1 & G_SHADE) sprintf(flags, "%sG_SHADE|", flags);
				if (w1 & G_SHADING_SMOOTH) sprintf(flags, "%sG_SHADING_SMOOTH|", flags);
				if (w1 & G_CULL_FRONT) sprintf(flags, "%sG_CULL_FRONT|", flags);
				if (w1 & G_CULL_BACK) sprintf(flags, "%sG_CULL_BACK|", flags);
				if (w1 & G_FOG) sprintf(flags, "%sG_FOG|", flags);
				if (w1 & G_LIGHTING) sprintf(flags, "%sG_LIGHTING|", flags);
				if (w1 & G_TEXTURE_GEN) sprintf(flags, "%sG_TEXTURE_GEN|", flags);
				if (w1 & G_TEXTURE_GEN_LINEAR) sprintf(flags, "%sG_TEXTURE_GEN_LINEAR|", flags);
				if (w1 & G_LOD) sprintf(flags, "%sG_LOD|", flags);
				
				
				sprintf(C_DisplayLists[C_DLNo].text, "%s\tgsSPSetGeometryMode(%sG_TEXTURE_ENABLE),\n", C_DisplayLists[C_DLNo].text, flags);
			}
			else if (!w1)	//gsSPClearGeometryMode();
			{
				if (w0 & G_ZBUFFER) sprintf(flags, "%sG_ZBUFFER|", flags);
				if (w0 & G_SHADE) sprintf(flags, "%sG_SHADE|", flags);
				if (w0 & G_SHADING_SMOOTH) sprintf(flags, "%sG_SHADING_SMOOTH|", flags);
				if (w0 & G_CULL_FRONT) sprintf(flags, "%sG_CULL_FRONT|", flags);
				if (w0 & G_CULL_BACK) sprintf(flags, "%sG_CULL_BACK|", flags);
				if (w0 & G_FOG) sprintf(flags, "%sG_FOG|", flags);
				if (w0 & G_LIGHTING) sprintf(flags, "%sG_LIGHTING|", flags);
				if (w0 & G_TEXTURE_GEN) sprintf(flags, "%sG_TEXTURE_GEN|", flags);
				if (w0 & G_TEXTURE_GEN_LINEAR) sprintf(flags, "%sG_TEXTURE_GEN_LINEAR|", flags);
				if (w0 & G_LOD) sprintf(flags, "%sG_LOD|", flags);
				
				
				sprintf(C_DisplayLists[C_DLNo].text, "%s\tgsSPClearGeometryMode(%sG_TEXTURE_ENABLE),\n", C_DisplayLists[C_DLNo].text, flags);
			}
			else if (w0 == -1)	//gsSPLoadGeometryMode();
			{
				if (w1 & G_ZBUFFER) sprintf(flags, "%sG_ZBUFFER|", flags);
				if (w1 & G_SHADE) sprintf(flags, "%sG_SHADE|", flags);
				if (w1 & G_SHADING_SMOOTH) sprintf(flags, "%sG_SHADING_SMOOTH|", flags);
				if (w1 & G_CULL_FRONT) sprintf(flags, "%sG_CULL_FRONT|", flags);
				if (w1 & G_CULL_BACK) sprintf(flags, "%sG_CULL_BACK|", flags);
				if (w1 & G_FOG) sprintf(flags, "%sG_FOG|", flags);
				if (w1 & G_LIGHTING) sprintf(flags, "%sG_LIGHTING|", flags);
				if (w1 & G_TEXTURE_GEN) sprintf(flags, "%sG_TEXTURE_GEN|", flags);
				if (w1 & G_TEXTURE_GEN_LINEAR) sprintf(flags, "%sG_TEXTURE_GEN_LINEAR|", flags);
				if (w1 & G_LOD) sprintf(flags, "%sG_LOD|", flags);
				
				
				sprintf(C_DisplayLists[C_DLNo].text, "%s\tgsSPLoadGeometryMode(%sG_TEXTURE_ENABLE),\n", C_DisplayLists[C_DLNo].text, flags);
			}
			else
				sprintf(C_DisplayLists[C_DLNo].text, "%s\tgsSPGeometryMode(%i, %i),\n", C_DisplayLists[C_DLNo].text, w0, w1);
			handled = 1;
			break;
		case G_POPMTX:		/* 0xd8 */
			break;
		case G_TEXTURE:		/* 0xd7 */
			w0 = Flip32(DisplayList[DLPos].w[0]);
			w1 = Flip32(DisplayList[DLPos].w[1]);
			dgsSPTexture(w0, w1, s, t, level, tile, on, BOWTIE_VAL)
			sprintf(C_DisplayLists[C_DLNo].text, "%s\tgsSPTexture(%i, %i, %i, %i, %s),\n", C_DisplayLists[C_DLNo].text, s, t, level, tile, G_ONOFF[on]);
			handled = 1;
			break;
		case G_DMA_IO:		/* 0xd6 */
			break;
		case G_SPECIAL_1:	/* 0xd5 */
			break;
		case G_SPECIAL_2:	/* 0xd4 */
			break;
		case G_SPECIAL_3:	/* 0xd3 */
			break;
		case G_VTX:		/* 0x01 */
			w0 = Flip32(DisplayList[DLPos].w[0]);
			w1 = Flip32(DisplayList[DLPos].w[1]);
			dgSPVertex(w0, w1, v, n, v0);
			sprintf(C_DisplayLists[C_DLNo].text, "%s\tgsSPVertex(&VertList%i, %i, %i),\n", C_DisplayLists[C_DLNo].text, vtxListCount, n, v0);
			DumpVtxList(v, n);
			fprintf(output, "Vtx_t VertList%i[%i] = {\n%s};\n\n", vtxListCount, n, VtxListC);
			vtxListCount++;
			handled = 1;
			break;
		case G_MODIFYVTX:	/* 0x02 */
			break;
		case G_CULLDL:		/* 0x03 */
			w0 = Flip32(DisplayList[DLPos].w[0]);
			w1 = Flip32(DisplayList[DLPos].w[1]);
			dgsSPCullDisplayList(w0, w1, vstart, vend)
			//sprintf(C_DisplayLists[C_DLNo].text, "%s\tgsSPCullDisplayList(%i, %i),\n", C_DisplayLists[C_DLNo].text, vstart, vend);
			//handled = 1;
			break;
		case G_BRANCH_Z:	/* 0x04 */
			break;
		case G_TRI1:		/* 0x05 */
			w0 = Flip32(DisplayList[DLPos].w[0]);
			dgsSP1Triangle( w0, 0, v0, v1, v2, flag0);
			sprintf(C_DisplayLists[C_DLNo].text, "%s\tgsSP1Triangle(%i, %i, %i, %i),\n", C_DisplayLists[C_DLNo].text, v0, v1, v2, flag0);
			handled = 1;
			break;
		case G_TRI2:		/* 0x06 */
			w0 = Flip32(DisplayList[DLPos].w[0]);
			w1 = Flip32(DisplayList[DLPos].w[1]);
			dgsSP2Triangle(w0, w1, v0, v1, v2, v3, v4, v5, flag0, flag1);
			sprintf(C_DisplayLists[C_DLNo].text, "%s\tgsSP2Triangles(%i, %i, %i, %i, %i, %i, %i, %i ),\n", C_DisplayLists[C_DLNo].text, v0, v1, v2, flag0, v3, v4, v5, flag1);
			handled = 1;
			break;
		case G_QUAD:		/* 0x07 */
			break;
		case G_LINE3D:		/* 0x08 */
			break;
		
		}
		if (!handled){
			w0 = Flip32(DisplayList[DLPos].w[0]);
			w1 = Flip32(DisplayList[DLPos].w[1]);
			sprintf(C_DisplayLists[C_DLNo].text, "%s\t%s{0x%08X, 0x%08X},\n", C_DisplayLists[C_DLNo].text,
				(DisplayList[DLPos].b[0] != G_CULLDL) ? "//" : "", w0, w1);
		}
		DLPos++;
	}
	fprintf(output, "%s", C_Textures);
	for (i=C_DLCount; i>=0; i--)
		fprintf(output, "%s", C_DisplayLists[i].text);
	fflush(output);
}

int
loadSegFile(char * filename, int seg)
{
	int filesize;
	FILE * infile = fopen(filename, "rb");
	if (infile == NULL)
		return -1;
	fseek(infile, 0, SEEK_END);
	filesize = ftell(infile);
	rewind(infile);
	RAMSeg[seg] = malloc(filesize);
	if (RAMSeg[seg] == NULL)
		return -1;
	fread(RAMSeg[seg], 1, filesize, infile);
	fclose(infile);
	return filesize;
}

#define LOADSCENE	0
#define LOADOBJ		1
#define ADDR		2

int main(int argc, char * argv[])
{
	if (argc < 2){
		fprintf(stderr, "Usage: %s [options]\nTry `%s --help' for more information.\n", argv[0], argv[0]);
		return -1;
	}
	int i, offset=0, seg_no, option=ADDR, sceneNo, mapNo;
	char filename[128];
	char ROMName[128] = "";
	output = stdout;
	FILE * infile;
	
	for (i=1; i<argc; i++){
		if(argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
			case 'o':
				output = fopen((char*)&argv[i][3],"w");
				break;
			case 'h':
				goto prog_help;
			case 'a':
				sscanf((char*)&argv[i][3], "%x", &offset);
				break;
			case 'b':
				sscanf(argv[i], "-b%i=%s", &seg_no, filename);
				if(loadSegFile(filename, seg_no) < 0)
					fprintf(stderr, "Warning: cannot load `%s' (errno %i)\n", filename, errno);
				break;
			case 'r':
				sscanf(argv[i], "-r=%s", ROMName);
				break;
			case 's':
				option = LOADSCENE;
				sscanf(argv[i], "-s%i,%i", &sceneNo, &mapNo);
				break;
			case '-':
				if(!strcmp((char*)&argv[i][2], "help")) {
					prog_help:
					fprintf(stderr, "Usage: %s [options]\nValid options:\n -o=FILE\tWrite output to FILE instead of stdout\n -h, --help\tThis message.\n --about\tDisplay program information.\n -bNUM=FILE\tFILE is loaded into RAM segment NUM.\n -r=FILE\tUse zelda rom FILE for RAM segments.\n -a=ADDRESS\tRead display list at ADRESS (hexadecimal) (Includes segment).\n -s=SCENE,MAP\tReads MAP of scene SCENE of ROM loaded with -r option\n", argv[0]);
					return 0;
				}
				else if(!strcmp((char*)&argv[i][2], "about")) {
					fprintf(stderr, "F3DEX2 -> C\nBy spinout (spinout182.com)\nVersion: %s\nBuilt %s %s\n", VERSION, __DATE__, __TIME__);
					return 0;
				}
				break;
			}
		}
	}
	if (option == ADDR)
		DumpDisplayList( RAMSeg[(offset>>24)&0xFF], offset&0xFFFFFF );
	else if (option == LOADSCENE) {
		;
	}

	
	for (i=0; i<16; i++){
		if(RAMSeg[i]!=NULL)
			free(RAMSeg[i]);
	}
	return 0;
}



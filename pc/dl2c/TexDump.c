#include "global.h"


void DumpTexture(int timg, int NPixels, int fmt, int siz, char C_Textures[], char texName[], int DumpTex)
{
	int i, seg = (timg >> 24) & 0xFF;
	if (RAMSeg[seg] != NULL) {
		switch(fmt){
		case G_IM_FMT_RGBA:
			if(siz == G_IM_SIZ_16b){
				sprintf(texName, "RGBA_16_%08X", timg);
				if (DumpTex){
					sprintf(C_Textures, "%sunsigned short RGBA_16_%08X [%i] = {\n", C_Textures, timg, NPixels);
					for(i=0, timg&=0xFFFFFF; i<NPixels; i++, timg+=2){
						if(i < NPixels-1)
							sprintf(C_Textures, "%s0x%04X, ", C_Textures, Read16(RAMSeg[seg], timg));
						else
							sprintf(C_Textures, "%s0x%04X\n};\n\n", C_Textures, Read16(RAMSeg[seg], timg));
						if (!((i+1) % 12) && i)
							sprintf(C_Textures, "%s\n", C_Textures);
					}
				}
			}
			else if(siz == G_IM_SIZ_32b){
				sprintf(texName, "RGBA_32_%08X", timg);
				if (DumpTex){
					sprintf(C_Textures, "%sunsigned long RGBA_32_%08X [%i] = {\n", C_Textures, timg, NPixels);
					for(i=0, timg&=0xFFFFFF; i<NPixels; i++, timg+=4){
						if(i < NPixels-1)
							sprintf(C_Textures, "%s0x%08X, ", C_Textures, Read32(RAMSeg[seg], timg));
						else
							sprintf(C_Textures, "%s0x%08X\n};\n\n", C_Textures, Read32(RAMSeg[seg], timg));
						if (!((i+1) % 8) && i)
							sprintf(C_Textures, "%s\n", C_Textures);
					}
				}
			}
			break;
		case G_IM_FMT_YUV:
			#ifdef DEBUG
			printf("YUV in OOT?!?\n")
			#endif
			break;
		case G_IM_FMT_CI:
			if(siz == G_IM_SIZ_4b){
				sprintf(texName, "CI_4_%08X", timg);
				if (DumpTex){
					NPixels>>=1;
					sprintf(C_Textures, "%sunsigned char CI_4_%08X [%i] = {\n", C_Textures, timg, NPixels);
					for(i=0, timg&=0xFFFFFF; i<NPixels; i++, timg++){
						if(i < NPixels-1)
							sprintf(C_Textures, "%s0x%02X, ", C_Textures, RAMSeg[seg][timg]);
						else
							sprintf(C_Textures, "%s0x%02X\n};\n\n", C_Textures, RAMSeg[seg][timg]);
						if (!((i+1) % 16) && i)
							sprintf(C_Textures, "%s\n", C_Textures);
					}
				}
			}
			else if(siz == G_IM_SIZ_8b){
				sprintf(texName, "CI_8_%08X", timg);
				if (DumpTex){
					sprintf(C_Textures, "%sunsigned char CI_8_%08X [%i] = {\n", C_Textures, timg, NPixels);
					for(i=0, timg&=0xFFFFFF; i<NPixels; i++, timg++){
						if(i < NPixels-1)
							sprintf(C_Textures, "%s0x%02X, ", C_Textures, RAMSeg[seg][timg]);
						else
							sprintf(C_Textures, "%s0x%02X\n};\n\n", C_Textures, RAMSeg[seg][timg]);
						if (!((i+1) % 16) && i)
							sprintf(C_Textures, "%s\n", C_Textures);
					}
				}
			}
		case G_IM_FMT_IA:
			if(siz == G_IM_SIZ_4b){
				sprintf(texName, "IA_4_%08X", timg);
				if (DumpTex){
					NPixels>>=1;
					sprintf(C_Textures, "%sunsigned char I_4_%08X [%i] = {\n", C_Textures, timg, NPixels);
					for(i=0, timg&=0xFFFFFF; i<NPixels; i++, timg++){
						if(i < NPixels-1)
							sprintf(C_Textures, "%s0x%02X, ", C_Textures, RAMSeg[seg][timg]);
						else
							sprintf(C_Textures, "%s0x%02X\n};\n\n", C_Textures, RAMSeg[seg][timg]);
						if (!((i+1) % 16) && i)
							sprintf(C_Textures, "%s\n", C_Textures);
					}
				}
			}
			else if(siz == G_IM_SIZ_8b){
				sprintf(texName, "IA_8_%08X", timg);
				if (DumpTex){
					sprintf(C_Textures, "%sunsigned char I_8_%08X [%i] = {\n", C_Textures, timg, NPixels);
					for(i=0, timg&=0xFFFFFF; i<NPixels; i++, timg++){
						if(i < NPixels-1)
							sprintf(C_Textures, "%s0x%02X, ", C_Textures, RAMSeg[seg][timg]);
						else
							sprintf(C_Textures, "%s0x%02X\n};\n\n", C_Textures, RAMSeg[seg][timg]);
						if (!((i+1) % 16) && i)
							sprintf(C_Textures, "%s\n", C_Textures);
					}
				}
			}
			else if(siz == G_IM_SIZ_16b){
				sprintf(texName, "IA_16_%08X", timg);
				if (DumpTex){
					sprintf(C_Textures, "%sunsigned short I_16_%08X [%i] = {\n", C_Textures, timg, NPixels);
					for(i=0, timg&=0xFFFFFF; i<NPixels; i++, timg++){
						if(i < NPixels-1)
							sprintf(C_Textures, "%s0x%04X, ", C_Textures, Read16(RAMSeg[seg], timg));
						else
							sprintf(C_Textures, "%s0x%04X\n};\n\n", C_Textures, Read16(RAMSeg[seg], timg));
						if (!((i+1) % 12) && i)
							sprintf(C_Textures, "%s\n", C_Textures);
					}
				}
			}
			break;
		case G_IM_FMT_I:
			if(siz == G_IM_SIZ_4b){
				sprintf(texName, "I_4_%08X", timg);
				if (DumpTex){
					NPixels>>=1;
					sprintf(C_Textures, "%sunsigned char I_4_%08X [%i] = {\n", C_Textures, timg, NPixels);
					for(i=0, timg&=0xFFFFFF; i<NPixels; i++, timg++){
						if(i < NPixels-1)
							sprintf(C_Textures, "%s0x%02X, ", C_Textures, RAMSeg[seg][timg]);
						else
							sprintf(C_Textures, "%s0x%02X\n};\n\n", C_Textures, RAMSeg[seg][timg]);
						if (!((i+1) % 16) && i)
							sprintf(C_Textures, "%s\n", C_Textures);
					}
				}
			}
			else if(siz == G_IM_SIZ_8b){
				sprintf(texName, "I_8_%08X", timg);
				if (DumpTex){
					sprintf(C_Textures, "%sunsigned char I_8_%08X [%i] = {\n", C_Textures, timg, NPixels);
					for(i=0, timg&=0xFFFFFF; i<NPixels; i++, timg++){
						if(i < NPixels-1)
							sprintf(C_Textures, "%s0x%02X, ", C_Textures, RAMSeg[seg][timg]);
						else
							sprintf(C_Textures, "%s0x%02X\n};\n\n", C_Textures, RAMSeg[seg][timg]);
						if (!((i+1) % 16) && i)
							sprintf(C_Textures, "%s\n", C_Textures);
					}
				}
			}
			break;
		}
	}
}

#include "z64lib.h"

int ZSetup = 0;
int ZFileTableOffset = 0;
int ROMSize = 0;
int zMaxFile=0;
//to be used internally
char * __ZROM;
char * __ZROMNAME;

int __getFTOffset() //not to be used externally
{
	int pos;
	char SearchForFiles = 0;
	for (pos=0; pos<0x20000; pos+=16) {
		if(!SearchForFiles && !strncmp((char*)&__ZROM[pos], "zelda@srd", 9))
			//mayhaps get build date/time
			SearchForFiles=1;
		else if(SearchForFiles && !memcmp((char*)&__ZROM[pos], Z64BOOTADDRS, 16))
			return pos;
	}
	pos=0;
	return 0;
}
void __countfiles()
{
	int pos;
	for(zMaxFile=0,pos=ZFileTableOffset; ; zMaxFile++,pos+=16){
		if (!memcmp((char*)&__ZROM[pos], ZEROS, 16))
			break;
	}
}

int getFile(int fileNo, ZFile file) // to be used externally
{
	//error check
	if(!ZSetup)
		return -1;
	if(fileNo > zMaxFile)
		return -1;
	
	int FTAddr = (fileNo << 4) + ZFileTableOffset;
	int ps, pe, vs, ve;
	GetFileOffsets(ps, pe, vs, ve, FTAddr, __ZROM);
	char comp[4];
	if (pe) {
		comp = memcpy(comp, &__ZROM[ps], 4);
		strtolower(comp)
		if (!strcmp(comp, "yaz0"))
			file.compress = COMPRESS_YAZ0;
		else if(!strcmp(comp, "yay0"))
			file.compress = COMPRESS_YAY0;
		else
			file.compress = COMPRESS_UNKN;
	}
	else
		file.compress = COMPRESS_NONE;
	file.compress |= ve-vs;
	file.data = (char*)&__ZROM[ps];
	file.size = pe-ps;
	
	return 0;
}

int getFileFromOffsets(int fileNo, ZFile file, int start, int end) // to be used externally
{
	//error check
	if(!ZSetup)
		return -1;
	if(end > start)
		return -1;
	
	int FTAddr = (fileNo << 4) + ZFileTableOffset;
	int ps, pe, vs, ve;
	GetFileOffsets(ps, pe, vs, ve, FTAddr, __ZROM);
	char comp[4];
	if (pe) {
		comp = memcpy(comp, &__ZROM[ps], 4);
		strtolower(comp)
		if (!strcmp(comp, "yaz0"))
			file.compress = COMPRESS_YAZ0;
		else if(!strcmp(comp, "yay0"))
			file.compress = COMPRESS_YAY0;
		else
			file.compress = COMPRESS_UNKN;
	}
	else
		file.compress = COMPRESS_NONE;
	file.compress |= ve-vs;
	file.data = (char*)&__ZROM[ps];
	file.size = pe-ps;
	
	return 0;
}

int zload(char * ROMName) // to be used externally
{
	strcpy(__ZROMNAME, ROMName);
	// read ROM
	FILE * ZROMF = open(__ZROMNAME, "rb");
	if (ZROMF == NULL)
		return -1;
	fseek(ZROMF, 0, SEEK_END);
	ROMSize = ftell(ZROMF);
	rewind(ZROMF);
	__ZROM = malloc(ROMSize);
	if (__ZROM == NULL)
		return -1;
	fread(__ZROM, 1, ROMSize, ZROMF);
	fclose(ZROMF);
	
	//setup stuff for other functions, global vars etc
	__getFTOffset();
	__countfiles();
	//lots of other stuff here (ROM version, name, country, crc, etc)??
	
	ZSetup = 1;
	return 0;
}

void zfree()
{
	if(!ZSetup)
		return;
	free(__ZROM);
}

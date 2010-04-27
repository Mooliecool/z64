#include "obj2c.h"
#include "png2raw.h"

typedef struct {
	signed short x, y, z;
} VERTEX;

typedef struct {
	signed short s, t;
} ST;

typedef struct {
	unsigned char r, g, b, a;
} RGBA;

typedef struct {
	unsigned int vert;
	unsigned int st;
	unsigned int rgba;
} TriVert;

typedef struct {
	TriVert a, b, c;
} Triangle;

typedef struct {
	float r, g, b;
} rgbFloat;

typedef struct {
	rgbFloat ka, kd, ks;
	float alpha;
	int flags;	//low two bits are illum, rest unused as of thus far
	char texture[128];
	char name[64];
	int tricount;
	Triangle tris[4000];
} Material;

VERTEX verts[4000];
ST sts[4000];
RGBA rgbas[4000];

Material Materials[64] = {{{0.2,0.2,0.2},
			{0.8,0.8,0.8},
			{1.0,1.0,1.0},
			1.0,
			1,
			"\0",
			"Default",
			0,
			{}	}};

int vertc=0, stc=0, rgbac=0, matc=1;

char vert_tmp[32][66];
char vert__tmp[66];
char vertbuffpos;

int interpPoint(TriVert * point)
{
	bool addvert;
	int part;
	sprintf(vert__tmp, "{%6i, %6i, %6i, 0, %6i, %6i, %3i, %3i, %3i, %3i},\n",
		verts[point->vert].x, verts[point->vert].y, verts[point->vert].z, sts[point->st].s, sts[point->st].t,
		rgbas[point->rgba].r, rgbas[point->rgba].g, rgbas[point->rgba].b, rgbas[point->rgba].a);
	addvert = true;
	for(n=0;n<vertbuffpos;n++){
		if (!strcmp(vert_tmp[n], vert__tmp)){
			addvert= false;
			part = n;
		}
	}
	if (addvert){
		strcpy(vert_tmp[vertbuffpos], vert__tmp);
		part=vertbuffpos;
		vertbuffpos++;
	}
	return part;
}

void 
buildCdl()
{
	char _verts[32768*4] = "";//this guy gets hueg
	char tris[1792*2] = "";
	char dl[8192*2] = "";
	Triangle cTri;
	int a, b, c, d, e, f;
	int vertListC=0;
	TriVert * pa, * pb, * pc;
	char ret[1000] = "Gfx dl_main[] = {\n";
	char * _ret= ret;
	
	for (i=0;i<matc;i++){
		if(!Materials[i].tricount){
			continue;
		}
		msg( "dlbuild: building mesh for material %s\n", Materials[i].name);
		sprintf(CTextures, "");
		sprintf(_verts, "");
		
		sprintf(ret, "%s\tgsSPDisplayList( dl_%s ),\n", ret, Materials[i].name);
		sprintf(dl, "Gfx dl_%s[] = {\n\tgsSPTexture(-1, -1, 0, 0, %s),\n", Materials[i].name,  *(Materials[i].texture) ? "G_ON" : "G_OFF");
		if (*(Materials[i].texture)){
			char texfn[128];
			strcpy(texfn, Materials[i].texture);
			read_png(texfn, dl);
		}
		
		for (j=0,vertbuffpos=0; j<Materials[i].tricount; j++){
			cTri = Materials[i].tris[j];
			a = interpPoint((TriVert*)&cTri.a);
			b = interpPoint((TriVert*)&cTri.b);
			c = interpPoint((TriVert*)&cTri.c);
			
			//vertex buffer full / on last triangle?
			if(vertbuffpos > 29 || j+1 == Materials[i].tricount){
				//append to vertex buff
				sprintf(_verts,"%sVtx_t VertList%i [%i] = {\n", _verts, vertListC, vertbuffpos);
				for (n=0;n<vertbuffpos;n++)
					sprintf(_verts, "%s%s", _verts, vert_tmp[n]);
				sprintf(_verts,"%s};\n\n", _verts);
				//append to display list
				sprintf(dl, "%s\tgsSPVertex(&VertList%i, %i, 0),\n", dl, vertListC, vertbuffpos);
				if (!d && !e && !f)
					sprintf(dl, "%s%s\tgsSPTriangle(%i, %i, %i, 0),\n", dl,tris, a, b, c);
				else
					sprintf(dl, "%s%s\tgsSP2Triangles(%i, %i, %i, 0, %i, %i, %i, 0),\n",dl, tris, d, e, f, a, b, c);
				//reset vars
				d=0,e=0,f=0;
				sprintf(tris, "");
				vertListC++;
				vertbuffpos=0;
			}
			else if (!(d|e|f)){
				d = a;
				e = b;
				f = c;
			}
			else {
				sprintf(tris, "%s\tgsSP2Triangles(%i, %i, %i, 0, %i, %i, %i, 0),\n", tris, d, e, f, a, b, c);
				d=0,e=0,f=0;
			}
		}
		fprintf(output, "%s\n\n%s\n\n%s\tgSPEndDisplayList()\n};\n\n\n", _verts, CTextures, dl);
	}
	
	fprintf(output, "%s\tgSPEndDisplayList()\n};\n", ret);
}

void
obj2dl(char * obj)
{
	msg( "reading obj file\n");
	prepare_arrays(obj);	//I do this separately not only to simplify code, but to make collision generation easier as well.
	msg( "building C\n");
	buildCdl();
}

void
prepare_arrays(char * obj)
{
	int linecount = 0;
	char * lines[4096];
	char command[16];
	char * face[32];
	char * point;
	char cmtl[32];
	char lastchar = '\n';
	float x,y,z, s,t, r,g,b;
	int pv, pt, pn;
	int nPts;
	int slashc;
	int currmat=0;
	bool mtllib = false;
	bool stmissing;
	
	//split obj into lines
	for (;*obj!='\0';obj++){
		if (lastchar == '\n'){
			lines[linecount] = obj;
			linecount++;
		}
		lastchar = *obj;
		if(*obj =='\n')
			*obj = '\0';
	}
	
	//read lines
	for(i=0;i<linecount;i++){
		if(lines[i][0]=='#' || !lines[i][0])
			continue;
		sscanf(lines[i], "%s ", command);
		if(!strcmp(command, "v")){
			sscanf(lines[i], "v %f %f %f", &x, &y, &z);
			verts[vertc].x = (signed short)(x*scale);
			verts[vertc].y = (signed short)(y*scale);
			verts[vertc].z = (signed short)(z*scale);
			vertc++;
		}
		else if(!strcmp(command, "vt")){
			sscanf(lines[i], "vt %f %f", &s, &t);
			sts[stc].s = (signed short)(s * 1024);
			sts[stc].t = (signed short)(t * 1024);
			stc++;
		}
		else if(!strcmp(command, "vn")){
			sscanf(lines[i], "vn %f %f %f", &r, &g, &b);
			if (r < 0.0)r=0.0;
			if (g < 0.0)g=0.0;
			if (b < 0.0)b=0.0;
			rgbas[rgbac].r = (char)(r*255);
			rgbas[rgbac].g = (char)(g*255);
			rgbas[rgbac].b = (char)(b*255);
			rgbas[rgbac].a = 255;
			rgbac++;
		}
		else if(!strcmp(command, "f")){
			//calculate # points, split face into char * array
			for(nPts=0; *lines[i]!='\0'; lines[i]++){
				if(lastchar == ' '){
					face[nPts-1] = lines[i];
				}
				lastchar = *lines[i];
				if(*lines[i] == ' '){
					*lines[i] = '\0';
					nPts++;
				}
			}
			
			//calculate # slashes
			stmissing = false;	// for faces like   f v1//vn1 v2//vn2 v3//vn3
			point = face[0];
			for(slashc=0; *point!='\0'; point++){
				if(*point == '/'){
					slashc++;
					if(lastchar == '/')stmissing = true;
				}
				lastchar = *point;
			}
			//read into TriVert list
			#if __TINYC__
			int fpoints[16][3];
			#else
			int fpoints[nPts][3];
			#endif
			
			switch(slashc){
			case 0:
				for(j=0;j<nPts;j++){
					sscanf(face[j], "%i", &fpoints[j][0]);
					fpoints[j][1]=0;
					fpoints[j][2]=0;
				}
				break;
			case 1:
				for(j=0;j<nPts;j++){
					sscanf(face[j], "%i/%i", &fpoints[j][0], &fpoints[j][1]);
					fpoints[j][2]=0;
				}
				break;
			case 2:
				for(j=0;stmissing,j<nPts;j++){
					sscanf(face[j], "%i//%i", &fpoints[j][0], &fpoints[j][2]);
					fpoints[j][1]=0;
				}
				for(j=0;(!stmissing),j<nPts;j++)
						sscanf(face[j], "%i/%i/%i", &fpoints[j][0], &fpoints[j][1], &fpoints[j][2]);
				break;
			}
			//append to tri array of current material
			nPts-=2;
			
			for(j=0; j<nPts; j++,Materials[currmat].tricount++,j++){
				Materials[currmat].tris[Materials[currmat].tricount].a.vert = fpoints[0][0] - 1;
				Materials[currmat].tris[Materials[currmat].tricount].a.st = fpoints[0][1] - 1;
				Materials[currmat].tris[Materials[currmat].tricount].a.rgba = fpoints[0][2] - 1;
				
				Materials[currmat].tris[Materials[currmat].tricount].b.vert = fpoints[j+1][0] - 1;
				Materials[currmat].tris[Materials[currmat].tricount].b.st = fpoints[j+1][1] - 1;
				Materials[currmat].tris[Materials[currmat].tricount].b.rgba = fpoints[j+2][2] - 1;
				
				Materials[currmat].tris[Materials[currmat].tricount].c.vert = fpoints[j+2][0] - 1;
				Materials[currmat].tris[Materials[currmat].tricount].c.st = fpoints[j+2][1] - 1;
				Materials[currmat].tris[Materials[currmat].tricount].c.rgba = fpoints[j+2][2] - 1;
			}
			//woah
		}
		else if(!strcmp(command, "g"))
			continue;
		else if(!strcmp(command, "o"))
			continue;
		else if(!strcmp(command, "s"))
			continue;
		else if(mtllib && !strcmp(command, "usemtl")){
			sscanf(lines[i], "usemtl %s", cmtl);
			for(j=0;j<matc;j++){
				if(!strcmp(cmtl, Materials[j].name)){
					if(currmat != j){
						currmat = j;
						msg( "obj: processing obj mesh for material %s\n", Materials[j].name);
					}
					break;
				}
			}
		}
		else if(!mtllib && !strcmp(command, "mtllib")){
			mtllib = true;
			char mtlfn[64];
			sscanf(lines[i], "mtllib %s", mtlfn);
			char mtlfn_[256];
			sprintf(mtlfn_, "%s%s", objdir, mtlfn);
			msg( "obj: opening, preprocessing, and parsing mtl: `%s'\n", mtlfn);
			read_mtl(mtlfn_);
		}
	}
}

void
read_mtl(char * fn)
{
	FILE * mtlf = openf(fn, 9000, 'm');
	fseek(mtlf, 0, SEEK_END);
	int mtlsiz = ftell(mtlf);
	fseek(mtlf, 0, SEEK_SET);
	char * mtl = malloc(mtlsiz);
	fread(mtl, 1, mtlsiz, mtlf);
	fclose(mtlf);
	bool firstmat = true;
	//clean up mtl
	mtl = preprocess(mtl);
	
	int linecount = 0;
	char * lines[1024];
	char command[8];
	char lastchar = '\n';
	int currmat=0;
	int flag;
	
	//split mtl into lines
	for (;*mtl!='\0';mtl++){
		if (lastchar == '\n'){
			lines[linecount] = mtl;
			linecount++;
		}
		lastchar = *mtl;
		if(*mtl =='\n')
			*mtl = '\0';
	}
	linecount --;
	
	//read lines
	for(j=0;j<linecount;j++){
		if(lines[j][0]=='#' || !lines[j][0])
			continue;
		sscanf(lines[j], "%s ", command);
		for(n=0;command[n]!='\0';n++)command[n]=tolower(command[n]);
		if(!strcmp(command, "ka"))
			sscanf(lines[j], "%*s %f %f %f", &Materials[currmat].ka.r, &Materials[currmat].ka.b, &Materials[currmat].ka.b);
		else if(!strcmp(command, "kd"))
			sscanf(lines[j], "%*s %f %f %f", &Materials[currmat].kd.r, &Materials[currmat].kd.b, &Materials[currmat].kd.b);
		else if(!strcmp(command, "ks"))
			sscanf(lines[j], "%*s %f %f %f", &Materials[currmat].ks.r, &Materials[currmat].ks.b, &Materials[currmat].ks.b);
		else if(!strcmp(command, "d") || !strcmp(command, "tr"))
			sscanf(lines[j], "%*s %f", &Materials[currmat].alpha);
		else if (!strcmp(command, "mapkd") || !strcmp(command, "map_kd") ||
			 !strcmp(command, "mapks") || !strcmp(command, "map_ks") ){
			char texfn[256];
			sscanf(lines[j], "%*s %s", texfn);
			sprintf(Materials[currmat].texture, "%s%s", objdir, texfn);
			msg("mtl: %s: texture: `%s'\n", Materials[currmat].name, texfn);
		}
		else if(!strcmp(command, "ns"))
			continue;
		else if(!strcmp(command, "illum")){
			sscanf(lines[j], "%*s %i", &flag);
			Materials[currmat].flags |= flag;
		}
		else if(!strcmp(command, "newmtl")){
			if(firstmat){
				currmat++;
				matc++;
			}
			Materials[currmat] = (Material){{0.2,0.2,0.2},
			{0.8,0.8,0.8},
			{1.0,1.0,1.0},
			1.0,
			1,
			"\0",
			"\0",
			0,
			{}	};
			sscanf(lines[j], "newmtl %s", Materials[currmat].name);
		}
	}
}


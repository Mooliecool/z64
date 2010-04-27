#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>

#ifndef OBJ2C_H
#define OBJ2C_H

#ifdef WIN32
 #define SLASHC '\\'
#else
 #define SLASHC '/'
#endif

typedef char bool;



int i, j, n;	/* I use loops everywhere, and I use i, j and n in them. I'm declaring them once; here */
float scale;
char * objdir;	//to include trailing slash (SLASHC)
bool verbose;

FILE * output;

#define false	0
#define true	1

#ifndef MYNAME
 #define MYNAME	"obj2c"
#endif

#define error( ... )\
{\
	fprintf(stderr, "%s: ", MYNAME);\
	fprintf(stderr, __VA_ARGS__);\
	exit(EXIT_FAILURE);\
}

#define msg( ... )\
if(verbose){fprintf(stderr, "%s: ", MYNAME);fprintf(stderr, __VA_ARGS__);}
	
char * preprocess(char * obj);
FILE * openf(char * fname, int argc, char flag);
void obj2dl(char * obj);
void reldir(char * fname);
void prepare_arrays(char * obj);
void read_mtl(char * fn);
char * fn2lbl(char *fn);

#endif /* OBJ2C_H */

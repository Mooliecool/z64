#include "obj2c.h"

char *
preprocess(char * obj)
{
	char * ret = obj;
	char * _ret = ret;
	char lastchar = '\0';
	for (;*obj!='\0';obj++){
		switch (*obj){
		case ' ':
		case '\t':
			if(lastchar != ' ' && lastchar != '\n' && lastchar != '\t'){
				*ret = ' ';
				ret++;
			}
			break;
		case '\r':
			break;
		default:
			*ret = *obj;
			ret++;
			break;
		}
		lastchar = *obj;
	}
	*ret = '\0';
	return _ret;
}

FILE *
openf(char * fname, int argc, char flag)
{
	FILE * fp;
	if (i < argc)
		fp = fopen(fname, flag == 'o' ? "w" : "r");
	else
		error("Missing argument for -%c flag\n", flag);
	if (fp == NULL)
		error("%s: No such file or directory\n", fname);
	msg("opened file %s\n", fname);
	return fp;
}

void
reldir(char * fname)
{
	objdir = fname;
	char * fn = strrchr(fname, SLASHC);
	fn++;
	*fn = '\0';
}

// file name to label, ie clean up name so it does not have names that begin with #s or are C operators. original char * is replaced as well
char *
fn2lbl(char *fn)
{
	char *r = fn;
	char *_r = r;
	bool first=true;
	for(;*fn;fn++){
		if ((*fn>'A' && *fn < '{' && *fn != ']' && *fn != '\\' && *fn != '[' && *fn != '^' && *fn != '`') ||
		    (*fn>'0' && *fn < '9' && !first)) {
			*_r = *fn;
			_r++;
			first = false;
		}
	}
	*_r = '\0';
	return r;
}


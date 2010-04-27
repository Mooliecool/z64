#include "/home/spinout/Desktop/obj2c/obj2c.h"

#ifdef DEBUG
 #define VERSION	"0.0.1 (Debug build)"
#else
 #define VERSION	"0.0.1"
#endif

FILE * input;

int
main(int argc, char * argv[])
{	
	output = stdout;
	input = stdin;
	if(input == output)
		error("input and output are same.\n");
	
	char * outputData;//, inputData;
	scale = 16.0;
	bool stopatpreprocess = false;
	verbose = false;
	
	for (i=1; i<argc; i++){
		if(argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
			case 'o':
				i++;
				output = openf(argv[i], argc, 'o');
				break;
			case 'h':
				goto prog_help;
			case 'i':
				i++;
				input = openf(argv[i], argc, 'i');
				reldir(argv[i]);
				break;
			case 'v':
				verbose = true;
				break;
			case 's':
				i++;
				sscanf(argv[i], "%f", &scale);
				break;
			case 'r':
				i++;
				if(i > argc){
					error("Missing argument for -r flag");
				}
				objdir = argv[i];
				break;
			case 'E':
				stopatpreprocess = true;
				break;
			case '-':
				if(!strcmp((char*)&argv[i][2], "help")) {
					prog_help:
					fprintf(stderr, "Usage: %s [options]\nValid options:\n -o FILE\twrite output to FILE instead of stdout\n -h, --help\tdisplay this help and exit\n --version\tdisplay program information and exit\n -i FILE\tread input from FILE instead of stdin\n -s SCALE\tscale all vertices by SCALE (float)\n -r PATH\tsearch for material definition and texture files in PATH,\n\t\tincluding trailing \"%c\"\n -v\t\tsend status messages to stderr\n -E\t\tpreprocess only; do not produce C for input\n\nExamples\n  %s -E < test.mtl > fixtest.mtl\tClean up a .mtl (not needed for %s)\n  %s -s 1 -i ..%cwv.obj > wv.h\tobj -> header\n\nReport bugs to <spinout_182@yahoo.com>\n", MYNAME, SLASHC, MYNAME, MYNAME, MYNAME, SLASHC);
					exit(EXIT_SUCCESS);
				}
				else if(!strcmp((char*)&argv[i][2], "version")) {
					fprintf(stderr, "%s %s\nWritten by spinout (spinout182.com)\nBuilt %s %s\n", MYNAME, VERSION, __DATE__, __TIME__);
					exit(EXIT_SUCCESS);
				}
				break;
			}
		}
	}
	
	if (isatty(fileno(input))){
		noinput:
		error("No input given\nUsage: %s [options]\nTry `%s --help' for more information.\n", MYNAME, MYNAME);
	}
	
	char * inputData = malloc(32768);
	
	for(i=0;!feof(input); i++)inputData[i]=getc(input);
	if (i < 3)
		goto noinput;
	inputData[i-1]='\0';
	
	msg("preprocessing %i bytes of input\n", i);
	inputData = preprocess(inputData);
	if (stopatpreprocess)
		fprintf(output, "%s", inputData);
	
	else {
		msg("converting input to C\n", i);
	
		fprintf(output, "/* Converted using obj2c-%s - http://spinout182.com */\n\n",VERSION);
		obj2dl(inputData);
	}
	
	
	fflush(output);
	
	msg("Done\n");
	
	exit(EXIT_SUCCESS);
}


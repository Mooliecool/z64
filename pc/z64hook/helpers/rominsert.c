#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char * argv[])
{
    char * buffer;
    int datalen, offset;
    if (argc < 4)
    {
        printf("Invalid usage. Usage: %s [data] [rom] [offset(hex)]\n", argv[0]);
        return -1;
    }
    /* Open files */
    FILE * rom = fopen(argv[2],"r+b");
    FILE * data = fopen(argv[1], "rb");
    /* Get filesize*/
    fseek(data, 0, SEEK_END);
    datalen = ftell(data);
    /* Rewind data */
    rewind(data);
    /* Allocate buffer to read data into */
    buffer = (char*)malloc(datalen);
    if (!buffer)
    {
        printf("Error allocating memory");
        return -1;
    }
    /* Read data into buffer */
    fread(buffer, 1, datalen, data);
    /* Seek ROM file to proper position */
    sscanf( argv[3], "%x", &offset);
    fseek(rom, offset, SEEK_SET);
    /* Write data to ROM */
    fwrite(buffer, 1, datalen, rom);
    free(buffer);
    /* Close files and return */
    fclose(rom);
    fclose(data);
    return 0;
}
    

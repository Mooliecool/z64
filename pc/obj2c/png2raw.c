/*
   modifications made to this file by spinout (spinout182.com)
   for use with 'obj2c'. Original header follows
*/

/* example.c - an example of using libpng */

/* this is an example of how to use libpng to read and write
   png files.  The file libpng.txt is much more verbose then
   this.  If you have not read it, do so first.  This was
   designed to be a starting point of an implementation.
   This is not officially part of libpng, and therefore
   does not require a copyright notice.

   This file does not currently compile, because it is missing
   certain parts, like allocating memory to hold an image.
   You will have to supply these parts to get it to compile.
   */

#include <png.h>

#include "png2raw.h"

/* read a png file.  You may want to return an error code if the read
   fails (depending upon the failure).
*/

bool MULTIBIT_ALPHA_define_written = false;
int read_png(char *file_name, char * dl)
{
   png_structp png_ptr;
   png_infop info_ptr;
   
   char header[8];   // 8 is the maximum size that can be checked

   /* open file and test for it being a png */
   FILE *fp = fopen(file_name, "rb");
   if (!fp)
      error("png: cannot open `%s'\n", file_name);
   fread(header, 1, 8, fp);
   if (png_sig_cmp(header, 0, 8))
      error("png: `%s' not a valid png\n", file_name);

   /* initialize stuff */
   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   
   if (!png_ptr)
      error("png: initialization error\n");

   info_ptr = png_create_info_struct(png_ptr);
   if (!info_ptr)
      error("png: initialization error\n");

   if (setjmp(png_jmpbuf(png_ptr)))
      error("png: unknown error\n");

   png_init_io(png_ptr, fp);
   png_set_sig_bytes(png_ptr, 8);

   png_read_info(png_ptr, info_ptr);

   
   if (info_ptr->interlace_type)
      error("png: interlaced png not supported\n");

   int color_type = info_ptr->color_type;
   int bit_depth = info_ptr->bit_depth;
   
   if(bit_depth!=8)
      error("png: each color channel's depth must be 8 bits\n");
   
   int xsize = info_ptr->width;
   int ysize = info_ptr->height;
   int maskx, masky, i;
   
   for(maskx=0;maskx<7;maskx++)if(xsize == 1<<maskx)break;
   if(maskx==7)
      error("png: Texture width must be a power of 2 less than 128\n");
   
   for(masky=0;masky<7;masky++)if(ysize == 1<<masky)break;
   if(masky==7)
      error("png: Texture height must be a power of 2 less than 128\n");
   
   
   /* the easiest way to read the image */
   png_bytep * row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * ysize);
   int x,y;
   for (y=0; y<ysize; y++)
      row_pointers[y] = (png_byte*) malloc(info_ptr->rowbytes);
   png_read_image(png_ptr, row_pointers);

   /* read the rest of the file, getting any additional chunks
      in info_ptr */
   png_read_end(png_ptr, info_ptr);
   png_byte * ptr;
   png_byte * row;
   
   /* time to get to work */
   char * fmt, * bpp, * TexName = fn2lbl(file_name);
   bool alphamap = false;
   int hack =1;
   i=1;
   switch(color_type)
   {
   case PNG_COLOR_TYPE_GRAY:
   {
      bpp = "G_IM_SIZ_8b";
      fmt = "G_IM_FMT_I";
      sprintf(CTextures, "%sunsigned char %s[%i] = {\n", CTextures, TexName, xsize * ysize);
      for(y=0;y<ysize;y++){
         row = row_pointers[y];
         for(x=0;x<xsize;x++){
            ptr = &(row[x]);
            sprintf(CTextures, "%s0x%02X, ", CTextures, ptr[0]);
            if(!(i%16))//time for a newline
               sprintf(CTextures, "%s\n", CTextures);
            i++;
         }
      }
      sprintf(CTextures, "%s};\n", CTextures);
      break;
   }
   case PNG_COLOR_TYPE_RGB:
   {
      if(xsize == 64 && ysize == 64){
         hack=2;
         xsize=32;
         maskx=5;
         ysize=32;
         masky=5;
      }
      bpp = "G_IM_SIZ_16b";
      fmt = "G_IM_FMT_RGBA";
      sprintf(CTextures, "%sunsigned short %s[%i] = {\n", CTextures, TexName, xsize * ysize);
      for(y=0;y<ysize;y+=hack){
         row = row_pointers[y];
         for(x=0;x<xsize;x+=hack){
            ptr = &(row[x<<2]);
            sprintf(CTextures, "%s0x%04X, ", CTextures, GPACK_RGBA5551(ptr[0], ptr[1], ptr[2], 255));
            if(!(i%12))//time for a newline
               sprintf(CTextures, "%s\n", CTextures);
            i++;
         }
      }
      sprintf(CTextures, "%s};\n", CTextures);
      break;
   }
   case PNG_COLOR_TYPE_RGB_ALPHA:
   {
      if(xsize == 64 && ysize == 64){
         hack=2;
         xsize=32;
         maskx=5;
         ysize=32;
         masky=5;
      }
      bpp = "G_IM_SIZ_16b";
      fmt = "G_IM_FMT_RGBA";
      sprintf(CTextures, "%sunsigned short %s[%i] = {\n", CTextures, TexName, xsize * ysize);
      for(y=0;y<ysize;y+=hack){
         row = row_pointers[y];
         for(x=0;x<xsize;x+=hack){
            ptr = &(row[x<<2]);
            sprintf(CTextures, "%s0x%04X, ", CTextures, GPACK_RGBA5551(ptr[0], ptr[1], ptr[2], 255));
            if(!(i%12))// time for a new line
               sprintf(CTextures, "%s\n", CTextures);
            i++;
            if(ptr[3] < 241)//alpha map needed?
               alphamap = true;
         }
      }
      sprintf(CTextures, "%s};\n", CTextures);
      if(alphamap && (xsize*ysize <= 2048)){
         i=1;
         sprintf(CTextures, "%s\nunsigned char %s_AlphaMap[%i] = {\n", CTextures, TexName, (xsize * ysize) >> 1);
         if(!MULTIBIT_ALPHA_define_written){
            fprintf(output, "#define MULTIBIT_ALPHA 0, 0, 0, TEXEL0, 0, 0, 0, TEXEL1\n\n");
            MULTIBIT_ALPHA_define_written=true;
         }
         for(y=0;y<ysize;y+=hack){
            row = row_pointers[y];
            for(x=0;x<xsize;x+=hack){
               ptr = &(row[x<<2]);
               if(x&1)
                  sprintf(CTextures, "%s%X, ", CTextures,  ptr[3]>>4);
               else
                  sprintf(CTextures, "%s0x%X", CTextures, ptr[3]>>4);
               if(!(i%32))
                  sprintf(CTextures, "%s\n", CTextures);
               i++;
            }
         }
         sprintf(CTextures, "%s};\n", CTextures);
         sprintf(dl, "%s\tgsDPSetCombineMode(MULTIBIT_ALPHA, G_CC_PASS2),\n\t_gsDPLoadTextureBlock_4b(%s_AlphaMap, 256, G_IM_FMT_I,\n\t\t\t%i, %i, 0,\n\t\t\tG_TX_WRAP, G_TX_WRAP,\n\t\t\t%i, %i, G_TX_NOLOD, G_TX_NOLOD),\n\tgsDPLoadTextureBlock(%s, G_IM_FMT_RGBA, G_IM_SIZ_16b,\n\t\t\t%i, %i, 0,\n\t\t\tG_TX_WRAP, G_TX_WRAP,\n\t\t\t%i, %i, G_TX_NOLOD, G_TX_NOLOD),\n\tgsDPSetTile(G_IM_FMT_I, G_IM_SIZ_4b, 2, 256, 1,\n\t\t\t0,\n\t\t\tG_TX_WRAP, %i,  G_TX_NOLOD,\n\t\t\tG_TX_WRAP, %i,  G_TX_NOLOD),\n\tgsDPSetTileSize( 1, 0, 0, %i << 2, %i << 2),\n\tgsDPSetCycleType(G_CYC_2CYCLE),\n", dl, TexName, xsize, ysize, maskx, masky, TexName, xsize, ysize, maskx, masky, maskx, masky, xsize-1, ysize-1);
      }
      else alphamap=false;
      break;
   }
   case PNG_COLOR_TYPE_GRAY_ALPHA:
   {
      bpp = "G_IM_SIZ_16b";
      fmt = "G_IM_FMT_IA";
      sprintf(CTextures, "%sunsigned char %s[%i] = {\n", CTextures, TexName, (xsize * ysize) << 1);
      for(y=0;y<ysize;y++){
         row = row_pointers[y];
         for(x=0;x<xsize;x++){
            ptr = &(row[x<<1]);
            sprintf(CTextures, "%s0x%02X, 0x%02X, ", CTextures, ptr[0], ptr[1]);
            if(!(i%10))//time for a newline
               sprintf(CTextures, "%s\n", CTextures);
            i++;
         }
      }
      sprintf(CTextures, "%s};\n", CTextures);
      break;
   }
   default:
      error("png: unkown color format (%i)", color_type);
   }
   msg("%s: %ix%i maskx/y: %i/%i bpp: %s fmt: %s\n", TexName, xsize, ysize, maskx, masky, bpp, fmt);
   
   if(!alphamap)
      sprintf(dl, "%s\tgsDPLoadTextureBlock(%s, %s, %s, %i, %i, 0,\n\t\tG_TX_WRAP, G_TX_WRAP,\n\t\t%i, %i, G_TX_NOLOD, G_TX_NOLOD),\n", dl, TexName, fmt, bpp, xsize, ysize, maskx, masky);
   
   /* clean up after the read, and free any memory allocated */
   png_read_destroy(png_ptr, info_ptr, (png_infop)0);

   /* free the structures */
   free(png_ptr);
   free(info_ptr);

   /* close the file */
   fclose(fp);
   
   /* that's it */
   return 0;
}



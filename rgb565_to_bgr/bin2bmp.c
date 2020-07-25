#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

typedef struct					   /**** BMP file header structure ****/
{
	unsigned int   bfSize;		   /* Size of file */
	unsigned short bfReserved1;	  /* Reserved */
	unsigned short bfReserved2;	  /* ... */
	unsigned int   bfOffBits;		/* Offset to bitmap data */
} BITMAPFILEHEADER;

typedef struct					   /**** BMP file info structure ****/
{
	unsigned int   biSize;		   /* Size of info header */
	int			biWidth;		  /* Width of image */
	int			biHeight;		 /* Height of image */
	unsigned short biPlanes;		 /* Number of color planes */
	unsigned short biBitCount;	   /* Number of bits per pixel */
	unsigned int   biCompression;	/* Type of compression to use */
	unsigned int   biSizeImage;	  /* Size of image data */
	int			biXPelsPerMeter;  /* X pixels per meter */
	int			biYPelsPerMeter;  /* Y pixels per meter */
	unsigned int   biClrUsed;		/* Number of colors used */
	unsigned int   biClrImportant;   /* Number of important colors */
} BITMAPINFOHEADER;

BITMAPFILEHEADER bfh;
BITMAPINFOHEADER bih;

void bitmap(char *pfbmp, unsigned int width, unsigned height, char *prgb)
{
	// pfbmp	: the name of BMP file
	// width, height	: the dimention of the BMP file
	// prgb				: the RGB binary of the image array

	/* Magic number for file. It does not fit in the header structure due to alignment requirements, so put it outside */
	unsigned short bfType=0x4d42;		   
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfSize = 2+sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)+width*height*3;
	bfh.bfOffBits = 0x36;

	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = width;
	bih.biHeight = height;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	bih.biCompression = 0;
	bih.biSizeImage = 0;
	bih.biXPelsPerMeter = 5000;
	bih.biYPelsPerMeter = 5000;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

	FILE *file = fopen(pfbmp, "wb");
	if (!file) {
		printf("Could not write file\n");
		return;
	}

	/*Write headers*/
	fwrite(&bfType,1,sizeof(bfType),file);
	fwrite(&bfh, 1, sizeof(bfh), file);
	fwrite(&bih, 1, sizeof(bih), file);

	/*Write bitmap*/
	fwrite(prgb, sizeof(char), bih.biWidth*bih.biHeight*3, file);
	// for (int y = bih.biHeight-1; y>=0; y--) { 
	// 	/*Scanline loop backwards*/ 
	// 	for (int x = 0; x < bih.biWidth; x++) { 
	// 		/*Column loop forwards*/ 
	// 		/*compute some pixel values*/
	// 		unsigned char r = *prgb++;
	// 		unsigned char g = *prgb++;
	// 		unsigned char b = *prgb++;
	// 		fwrite(&b, 1, 1, file);
	// 		fwrite(&g, 1, 1, file);
	// 		fwrite(&r, 1, 1, file);
	// 	}
	// }
	fclose(file);
}
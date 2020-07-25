#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

/*---------------------------------------------------------------------
 * Constants
 */
#define SZ_FNAME_LEN_MAX			128

/*------------------------------------------------------------------
 * Global Variables
 */
FILE *fd_src, *fd_dst;
static char *psz_prog, *psz_fnsrc, *psz_fndst;


/*---------------------------------------------------------------------
 * External reference
 */
void bitmap(char *pfbmp, unsigned int width, unsigned height, char *prgb);

/*------------------------------------------------------------------
 * Implementation
 */


/*---------------------------------------------------------------------
 * _print_prog_usage
 */
static void _print_prog_usage(char *psz_prog_name)
{
	printf("\n");
	printf("Usage:\n");
	printf("\t%s input output\n",  psz_prog_name);
	printf("Descriptions:\n");
	printf("\tConvert rgb565-image to opencv-bgr-image ...\n");
	// printf("\t\tamba.raw : specify the filename of the input raw.\n");
	// printf("\t\tmtk.raw  : specify the filename of the output raw.\n");
}

/*---------------------------------------------------------------------
 * main
 */
int main(int argc, char *argv[])
{
	char *psrcbuf, *pbufsave;

	psz_prog = argv[0];
	if (argc<3) {
		_print_prog_usage(psz_prog);
		exit(0);
	}

	/*-- open source file */
	psz_fnsrc = argv[1];
	printf("Opening source file: %s\n", psz_fnsrc);
	fd_src = fopen(psz_fnsrc, "rb");
	if (!fd_src) {
		printf("FATAL: error opening file [%s] !\n", psz_fnsrc);
		exit(1);
	}

	/*-- open/create target file */
	psz_fndst = argv[2];
	fd_dst = fopen(psz_fndst, "wb");
	if (!fd_dst) {
		printf("FATAL: error creating destination [%s] !\n", psz_fndst);
		if (fd_src) fclose(fd_src);
		exit(1);
	}

	/*-- alloc & read SRC image */
	size_t fsize;
	fseek(fd_src, 0, SEEK_END);
	fsize = ftell(fd_src);

	pbufsave = psrcbuf = (char *)malloc(fsize);
	//printf("SRC[%s] size : %d\n", psz_fnsrc, fsize);

	if (!psrcbuf) {
		printf("Error: insufficient memory!!\n");
		goto _exit;
	}

	fseek(fd_src, 0, SEEK_SET);
	if (fsize != fread(psrcbuf, sizeof(char), fsize, fd_src)) {
		printf("Error: failed to read SRC %s\n", psz_fnsrc);
		goto _exit;
	}

	printf("Converting ...\n");
	char ch0, ch1, *pdstbuf, *pdstsave;
	unsigned short *p565;
	size_t wsize;
	//unsigned int utmp;

	wsize = 3 * (fsize / 2);	// rgb565(2 butes) --> BGR(3 bytes)
	pdstbuf =(char *)malloc(wsize);
	pdstsave = pdstbuf;
	p565 = (unsigned short *)psrcbuf;
	do {
		char r, g, b;
		unsigned short color565;

#if 1	// Keep original value		
		color565 = *p565++;
		//獲取高位元組的5個bit
		r = (char)((color565 & 0xF800)>>11);
		//獲取中間6個bit
		g = (char)((color565 & 0x07E0)>>5);
		//獲取低位元組5個bit
		b = (char)(color565 & 0x001F);
#else	// Normalize
		color565 = *p565++;
		//獲取高位元組的5個bit
		r = (char)((color565 & 0xF800)>>(8));
		//獲取中間6個bit
		g = (char)((color565 & 0x07E0)>>(3));
		//獲取低位元組5個bit
		b = (char)((color565 & 0x001F)<<3);
#endif
		//printf("r= %02x, g= %02x, b= %02x");
		*pdstbuf++ = r;
		*pdstbuf++ = g;
		*pdstbuf++ = b;

		fsize -= 2;
		//printf("--> %d ", fsize);
	} while(fsize > 0);

	if (fsize) {
		printf("error: Conversion failed!!\n");
		goto _exit;
	}

	printf("Writing %s ...\n", psz_fndst);

#if 0
	fwrite(pdstsave, sizeof(char), wsize, fd_dst);
#else
	bitmap(psz_fndst, 640, 480, pdstsave);
#endif

	/*-- exiting program */
_exit:
	if (fd_src) fclose(fd_src);
	if (fd_dst) fclose(fd_dst);
	if (pbufsave) free(pbufsave);
	if (pdstsave) free(pdstsave);

    exit(0);
}

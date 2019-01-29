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
static char *psz_prog, *psz_fnsrc, sz_fndst[SZ_FNAME_LEN_MAX];


/*---------------------------------------------------------------------
 * External reference
 */


/*------------------------------------------------------------------
 * Implementation
 */


/*---------------------------------------------------------------------
 * _print_prog_usage
 */
static void _print_prog_usage(unsigned char *psz_prog_name)
{
	printf("\n");
	printf("Usage:\n");
	printf("\t%s srcfname\n",  psz_prog_name);
	printf("DESCRIPTION:\n");
	printf("To swap byte order of the srcfname...\n");
	printf(">srcfname : specify the filename of the source raw.\n");
}

/*---------------------------------------------------------------------
 * main
 */
int main(int argc, char *argv[])
{
	size_t szlen;
	unsigned err;

	psz_prog = argv[0];
	if (argc<2) {
		_print_prog_usage(psz_prog);
		exit(0);
	}

	/*-- open source file */
	psz_fnsrc = argv[1];
	printf("openning source file: %s\n", psz_fnsrc);
	fd_src = fopen(psz_fnsrc, "rb");
	if (!fd_src) {
		printf("FATAL: error openning file [%s] !\n", psz_fnsrc);
		exit(1);
	}

	/*-- open target file */
	szlen = strlen(psz_fnsrc)+1;
	if (szlen>SZ_FNAME_LEN_MAX-9) {
		printf("error: insufficient memory!\n");
		goto _exit;
	}
	strncpy(sz_fndst, psz_fnsrc, szlen);
	strncat(sz_fndst, ".swp",4);

	printf("creating target file : %s\n", sz_fndst);
	fd_dst = fopen(sz_fndst, "wb");
	if (!fd_dst) {
		printf("FATAL: error creating destination [%s] !\n", sz_fndst);
		goto _exit;
	}

	/*-- alloc & read SRC image */
	size_t fsize;
	fseek(fd_src, 0, SEEK_END);
	fsize = ftell(fd_src);

	unsigned char *psrcbuf;
	psrcbuf = (unsigned char *)malloc(fsize);
	printf("SRC[%s] size : %d\n", psz_fnsrc, fsize);

	if (!psrcbuf) {
		printf("Error: insufficient memory!!\n");
		goto _exit;
	}

	fseek(fd_src, 0, SEEK_SET);
	if (fsize != fread(psrcbuf, sizeof(char), fsize, fd_src)) {
		printf("Error: failed to read SRC %s\n", psz_fnsrc);
		goto _exit;
	}

	printf("convert src image (size %d)...\n", fsize);
	unsigned char pxlsrc[2], pxltmp;
	int i=0, doidx=0;

	do {
		memcpy(pxlsrc, psrcbuf+(doidx*2), 2);
		//printf("%x %x %x %x\n", (unsigned)pxlsrc[0], (unsigned)pxlsrc[1], (unsigned)pxlsrc[2], (unsigned)pxlsrc[3]);

		pxltmp = pxlsrc[0]; pxlsrc[0]=pxlsrc[1]; pxlsrc[1]=pxltmp;

		fwrite(pxlsrc, sizeof(char), 2, fd_dst);
		//printf("\n");
		doidx++;
		fsize -= 2;

		if (fsize==1) {
			fwrite(psrcbuf+(doidx*2), sizeof(char), 1, fd_dst);
			goto _exit;
		}
	} while(fsize);

	if (fsize) {
		printf("error: Conversion failed!!\n");
		goto _exit;
	}

	/*-- exiting program */
_exit:
	if (fd_src) fclose(fd_src);
	if (fd_dst) fclose(fd_dst);
	if (psrcbuf) free(psrcbuf);

    exit(0);
}

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
	printf("\t%s amba.raw mtk.raw\n",  psz_prog_name);
	printf("Descriptions:\n");
	printf("\tConvert amba raw image to mtk raw-8 for GM selection...\n");
	printf("\t\tamba.raw : specify the filename of the input raw.\n");
	printf("\t\tmtk.raw  : specify the filename of the output raw.\n");
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
	char ch0, ch1, *pdstbuf;//, u8tmp;
	size_t wsize;
	unsigned int utmp;

	pdstbuf = psrcbuf;
	wsize = fsize / 2;
	do {
		ch0 = *psrcbuf++;
		ch1 = *psrcbuf++;
		//printf("ch0= %02x, ch1= %02x \n", ch0, ch1);
		utmp = ch1; utmp <<= 8; utmp += ch0;
		*pdstbuf++ = (char)((utmp >> 6) & 0xFF);

		//printf("utmp= 0x%04x, u8tmp= 0x%x\n", utmp, u8tmp);
		fsize -= 2;
		//printf("--> %d ", fsize);
	} while(fsize > 0);

	if (fsize) {
		printf("error: Conversion failed!!\n");
		goto _exit;
	}

	fwrite(pbufsave, sizeof(char), wsize, fd_dst);


	/*-- exiting program */
_exit:
	if (fd_src) fclose(fd_src);
	if (fd_dst) fclose(fd_dst);
	if (pbufsave) free(pbufsave);

    exit(0);
}


#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <time.h>

#include "common.h"

/*---------------------------------------------------------------------
 * Constants
 */
#define SZ_FNAME_LEN_MAX			32

/*------------------------------------------------------------------
 * Global Variables
 */
static char *psz_prog;
static int _dbg;

/*---------------------------------------------------------------------
 * External reference
 */


/*------------------------------------------------------------------
 * Implementation
 */


/*---------------------------------------------------------------------
 * _show_usage
 */
static void _show_usage(char *psz_prog_name)
{
	printf("\n");
	printf("Usage:\n");
	printf("\t%s filename [-d]\n",  psz_prog_name);
	printf("DESCRIPTION:\n");
	printf("\tTraverse the directory and check time of each file...\n");
	printf("OPTION:\n");
	printf(" -d     enable debug printf");
//	printf(" -h     *height* : specify height of raw image\n");
//	printf(" -bayer *pattern_id* : bayer pattern\n");
//	printf("\t 1=R_GR_GB_B, 2=GR_R_B_GB, 3=B_GB_GR_R, 4=GB_B_R_GR\n");
//	printf(" -b     *bits* : specify bits per pixel\n");
//	printf(" -of    *raw_fname* : file name of output RAW image.\n");
//	printf(" -bs    *block_hsize* : specify the hsize of do block\n");
//	printf(" -cfg   *cfg_fname* : specify full name of cfg file\n");
}


///!------------------------------------------------------
///! @brief		DbgPrint
///! @param
///! @return
///! @note
///!------------------------------------------------------
static void DbgPrint(const char *fmt, ...)
{
	if (_dbg > 0) {
		va_list	args;
		va_start(args, fmt);
		printf(fmt, args); fflush(stdout);
		va_end(args);
	}
}

///!------------------------------------------------------
///! @brief
///! @param
///! @return
///! @note
///!------------------------------------------------------
int check_file_time(struct stat *p_sb)
{
	time_t atime, mtime, ctime;
	atime = p_sb->st_atime;
	mtime = p_sb->st_mtime;
	ctime = p_sb->st_ctime;

	if ((mtime < ctime) || (atime < mtime))
		return -1;
	else
		return 0;
}


///!------------------------------------------------------
///! @brief
///! @param
///! @return
///! @note
///!------------------------------------------------------
int print_ftime(char *p_fname)
{
	//-- get file info
	struct 	stat sb;
	if (stat(p_fname, &sb) == -1) {
		perror("stat");
		return -1;
	}

	switch(sb.st_mode & S_IFMT) {
		// char 	*p_new_root;
		// int		new_size;
		case S_IFREG:
			DbgPrint("case: regular file [%s]\n", p_fname); fflush(stdout);
			printf("Testing %s ... ", p_fname);
				printf(" \n>>> ctime=0x%08X, mtime=0x%08X, atime=0x%08X\n", sb.st_ctime, sb.st_mtime, sb.st_atime);
				// printf("\n[ERROR]: %s/%s got abnormal file time !!\n", psz_root, ptr->d_name);
				// printf("st_ctime=%ld, st_mtime=%ld, st_atime=%ld\n", sb.st_ctime, sb.st_mtime, sb.st_atime);
				//printf(" \n>>> st_ctime=%s, st_mtime=%s, st_atime=%s\n", ctime(&(sb.st_ctime)), ctime(&(sb.st_mtime)), ctime(&(sb.st_atime)));
			break;
		default:
			printf("case: others\n"); fflush(stdout);
			break;
	}
	return 0;
}



///!------------------------------------------------------
///! @brief		main
///! @param
///! @return
///! @note
///!------------------------------------------------------
int main(int argc, char *argv[])
{
	char	*psz_prog;
	int		errCode;

	psz_prog = basename(argv[0]);	///! need to include libgen.h
	if (argc < 2) {
		_show_usage(basename(psz_prog));
		return 0;
	}

	_dbg = 0;
	if (argc == 3 && !strcmp("-d", argv[2])) {
		_dbg = 1;
		fprintf(stdout, "Debug mode enabled!!\n"); fflush(stdout);
	}

	print_ftime(argv[1]);

	return 0;
}

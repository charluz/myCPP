
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
static int _dbg, _show_time;

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
	printf("\t%s root_path [-d]\n",  psz_prog_name);
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
int check_file_time(struct stat *p_sb, time_t *p_min_tm)
{
	time_t atime, mtime, ctime, min;
	atime = p_sb->st_atime;
	mtime = p_sb->st_mtime;
	ctime = p_sb->st_ctime;

	min = (atime < mtime) ? atime : mtime;
	min = (ctime < min) ? ctime : min;
	*p_min_tm = min;

	//if ((mtime < ctime) || (atime < mtime))
	if (atime < mtime) {
		// @Windows10, mtime=日期，atime=修改日期，ctime=建立日期(copy 會改改變這一個時間) ??
		return -1;
	}
	else
		return 0;
}


///!------------------------------------------------------
///! @brief
///! @param
///! @return
///! @note
///!------------------------------------------------------
int process_dir(char *psz_root)
{
	DIR		*pDir;
	struct	dirent	*ptr;
	int 	errCode;

	if (!psz_root) {
		return -1;
	}
	DbgPrint("%s(): [%s]\n", __FUNCTION__, psz_root); fflush(stdout);

	//-- get DIR struct of root directory
	if( !(pDir = opendir(psz_root)) ) {
		return -1;
	}

	// ///! skip "."
	// if ( (ptr = readdir(pDir)) == 0 ) {
	// 	closedir(pDir);
	// 	return 0;
	// }

	//-- iterate root directory
	while( (ptr = readdir(pDir)) != 0 ) {
		/*Processing Dir/File */
		if (!strcmp("..", ptr->d_name) || !strcmp(".", ptr->d_name)) {
			continue;
		}

		DbgPrint("Processing [%s/%s]\n", psz_root, ptr->d_name); fflush(stdout);
		//-- get file info
		struct 	stat sb;
		char	*p_test_fname;
		int 	test_path_len;
		test_path_len = strlen(psz_root)+strlen(ptr->d_name)+2;
		p_test_fname = (char *)malloc(test_path_len);
		snprintf(p_test_fname, test_path_len, "%s/%s", psz_root, ptr->d_name);
		if (stat(p_test_fname, &sb) == -1) {
			perror("stat");
			errCode = EXIT_FAILURE;
			continue;
		}

		switch(sb.st_mode & S_IFMT) {
			char 	*p_new_root;
			int		new_size;
			time_t min_tm;

			case S_IFDIR:
				DbgPrint("case: directory [%s/%s]\n", psz_root, ptr->d_name); fflush(stdout);
				new_size = strlen(psz_root)+1+strlen(ptr->d_name)+1;
				p_new_root = (char *)malloc(new_size);
				snprintf(p_new_root, new_size, "%s/%s", psz_root, ptr->d_name);
				process_dir(p_new_root);
				free(p_new_root);
				break;
			case S_IFREG:
				DbgPrint("case: regular file [%s/%s]\n", psz_root, ptr->d_name); fflush(stdout);
				printf("Testing %s/%s ... ", psz_root, ptr->d_name);
				if (check_file_time(&sb, &min_tm)< 0) {
					if (_show_time) {
						printf(" ERROR.\n\t>>> ctime=%lld, mtime=%lld, atime=%lld\n", (sb.st_ctime-min_tm), (sb.st_mtime-min_tm), (sb.st_atime-min_tm) );
					}
					else {
						printf(" ERROR.\n\t>>> mtime=%lld, atime=%lld\n", (sb.st_mtime-min_tm), (sb.st_atime-min_tm) );
					}
				}
				else {
					if (_show_time) {
						printf(" PASS.\n\t>>> ctime=%lld, mtime=%lld, atime=%lld\n", (sb.st_ctime-min_tm), (sb.st_mtime-min_tm), (sb.st_atime-min_tm));
					}
					else {
						printf(" PASS.\n");
					}
				}
				break;
			default:
				printf("case: others\n"); fflush(stdout);
				break;
		}
	}	// end of while()

	closedir(pDir);
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
	_show_time = 0;
	if (argc == 3 && !strcmp("-d", argv[2])) {
		_dbg = 1;
		fprintf(stdout, "Debug mode enabled!!\n"); fflush(stdout);
	}
	else if (argc == 3 && !strcmp("-t", argv[2])) {
		_show_time = 1;
		fprintf(stdout, "Show-time enabled!!\n"); fflush(stdout);
	}

	process_dir(argv[1]);

	return 0;
}

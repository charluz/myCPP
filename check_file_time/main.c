
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
FILE *fd_bmp, *fd_raw;
static char *psz_prog, *psz_fnbmp, sz_fnraw[SZ_FNAME_LEN_MAX];


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
	printf("\t%s root_path\n",  psz_prog_name);
	printf("DESCRIPTION:\n");
	printf("\tTraverse the directory and check time of each file...\n");
//	printf("OPTION:\n");
//	printf(" -w     *width* : specify width of raw image\n");
//	printf(" -h     *height* : specify height of raw image\n");
//	printf(" -bayer *pattern_id* : bayer pattern\n");
//	printf("\t 1=R_GR_GB_B, 2=GR_R_B_GB, 3=B_GB_GR_R, 4=GB_B_R_GR\n");
//	printf(" -b     *bits* : specify bits per pixel\n");
//	printf(" -of    *raw_fname* : file name of output RAW image.\n");
//	printf(" -bs    *block_hsize* : specify the hsize of do block\n");
//	printf(" -cfg   *cfg_fname* : specify full name of cfg file\n");
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
int process_dir(char *psz_root)
{
	DIR		*pDir;
	struct	dirent	*ptr;
	int 	errCode;

	if (!psz_root) {
		return -1;
	}
	//printf("%s(): [%s]\n", __FUNCTION__, psz_root); fflush(stdout);

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

		//printf("Processing [%s/%s]\n", psz_root, ptr->d_name); fflush(stdout);
		//-- get file info
		struct stat sb;
		if (stat(ptr->d_name, &sb) == -1) {
			perror("stat");
			errCode = EXIT_FAILURE;
			continue;
		}

		switch(sb.st_mode & S_IFMT) {
			char 	*p_new_root;
			int		new_size;
			case S_IFDIR:
				//printf("case: directory [%s/%s]\n", psz_root, ptr->d_name); fflush(stdout);
				new_size = strlen(psz_root)+1+strlen(ptr->d_name)+1;
				p_new_root = (char *)malloc(new_size);
				snprintf(p_new_root, new_size, "%s/%s", psz_root, ptr->d_name);
				process_dir(p_new_root);
				free(p_new_root);
				break;
			case S_IFREG:
				//printf("case: regular file [%s/%s]\n", psz_root, ptr->d_name); fflush(stdout);
				printf("Testing %s/%s ... ", psz_root, ptr->d_name);
				if (check_file_time(&sb)< 0) {
					printf(" ERROR, %ld, %ld, %ld\n", sb.st_ctime, sb.st_mtime, sb.st_atime);
					// printf("\n[ERROR]: %s/%s got abnormal file time !!\n", psz_root, ptr->d_name);
					// printf("st_ctime=%ld, st_mtime=%ld, st_atime=%ld\n", sb.st_ctime, sb.st_mtime, sb.st_atime);
					//!!?? printf("st_ctime=%s, st_mtime=%s, st_atime=%s\n", ctime(&(sb.st_ctime)), ctime(&(sb.st_mtime)), ctime(&(sb.st_atime)));
				}
				else {
					printf(" PASS.\n");
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
	char	*psz_prog, *psz_root_dir;
	int		errCode;

	psz_prog = basename(argv[0]);	///! need to include libgen.h
	if (argc < 2) {
		_show_usage(basename(psz_prog));
		return 0;
	}

	psz_root_dir = argv[1];
	printf("Root dir %s, size= %d\n", psz_root_dir, strlen(psz_root_dir)+1);

	process_dir(psz_root_dir);

	return 0;
}

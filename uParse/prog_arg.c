/*-------------------------------------------------------------------------*/
/**
	@file    prog_arg.c
	@author  alex wu
	@date    Nov 2008
	@version 1.0
	@brief   Program argument process routines.
*/

/*---------------------------------------------------------------------------
   								Includes
 ---------------------------------------------------------------------------*/
#include "myut_common.h"
#include "prog_arg.h"
#include "myut_llist.h"
#include "myut_string.h"

/*-- utility module printf */
#define ARG_DEBUG			0
#if ARG_DEBUG
#define xprintx(fmt, arg...)		printf(fmt, ##arg)
#else
#define xprintx(fmt, arg...)		do {} while(0)
#endif /* ARG_DEBUG */

/*---------------------------------------------------------------------------
   								Defines
 ---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
   								Types
 ---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
   								Globals
 ---------------------------------------------------------------------------*/

LINK_LIST_t *p_optlist = NULL;
LINK_LIST_t *p_tgtlist = NULL;

static char gsz_prog_name[MAX_FNAME_LEN]={0};
static unsigned g_prog_optnum = 0;
static unsigned g_prog_tgtnum = 0;


/*===========================================================================
   							Function Definitions
 ===========================================================================*/


/*-------------------------------------------------------------------------*/
/**
	@brief	To new and init a new option element.
	@param	psz_ctrl	the string pointer to the option control.
	@param	psz_data	the string pointer to the option data.
	@return	(PROG_OPTION_t *) the pointer to the new created element.
*/
static char* _tgtlist_element_new(char *psz_tgt)
{
	char *p_elmt;

	if (strlen(psz_tgt)>(PROG_TGT_SZ_SIZE-1)) {
		printf("%s: error: string size exceeds preinstall size !!\n", __FUNCTION__);
		return NULL;
	}

	p_elmt = (char *)malloc(PROG_TGT_SZ_SIZE);
	if (!p_elmt) {
		printf("%s: error: insufficient memory !\n", __FUNCTION__);
		return NULL;
	}

	memset(p_elmt, 0, PROG_TGT_SZ_SIZE);
	strcpy(p_elmt, psz_tgt);

	return p_elmt;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To new and init a new option element.
	@param	psz_ctrl	the string pointer to the option control.
	@param	psz_data	the string pointer to the option data.
	@return	(PROG_OPTION_t *) the pointer to the new created element.
*/
static PROG_OPTION_t* _optlist_element_new(char *psz_ctrl, char *psz_data)
{
	PROG_OPTION_t *p_elmt;

	if ( strlen(psz_ctrl)>(PROG_ARG_ELMT_CTRL_SIZE-1)
		|| strlen(psz_data)>(PROG_ARG_ELMT_DATA_SIZE-1) ) {
		printf("%s: error: string size exceeds preinstall size !!\n", __FUNCTION__);
		return NULL;
	}

	p_elmt = (PROG_OPTION_t *)malloc(sizeof(PROG_OPTION_t));
	if (!p_elmt) {
		printf("%s: error: insufficient memory !\n", __FUNCTION__);
		return NULL;
	}

	/*
	memset(p_elmt->sz_ctrl, 0, PROG_ARG_ELMT_CTRL_SIZE);
	memset(p_elmt->sz_data, 0, PROG_ARG_ELMT_DATA_SIZE);
	*/

	strcpy(p_elmt->sz_ctrl, psz_ctrl);
	strcpy(p_elmt->sz_data, psz_data);

	#if 0
	printf("==optlist elmt dump (%s, %s) ...\n",p_elmt->sz_ctrl, p_elmt->sz_data);
	#endif

	return p_elmt;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To look up the element in a list.
	@param	psz_ctrl		The string pointer to the option control to look for.
	@return	(PROG_OPTION_t *) the pointer to the element.
*/
static PROG_OPTION_t* _optlist_element_find(char *psz_ctrl)
{
	PROG_OPTION_t *p_elmt;
	LL_NODE_t *p_node;

	xprintx("looking for (%s) option ...\n", psz_ctrl);
	p_node = llist_head(p_optlist);
	if (!p_node) { xprintx("empty optlist !!"); return NULL; }

	while(p_node) {
		p_elmt = (PROG_OPTION_t *)p_node->data;
		if (STR_EQUAL(psz_ctrl, p_elmt->sz_ctrl)) break;
		p_elmt = NULL;
		p_node = p_node->p_next;
	}

	return p_elmt;
}


/*-------------------------------------------------------------------------*/
/**
	@brief	To determine if the option control has data followed.
	@param	psz_ctrl	The string pointer to the control.
	@return	unsigned	Flag to tell if data follows this control.
*/
static unsigned _prog_opt_parse_control(char *psz_ctrl)
{
	/*-- ret=0: ctrl w/o data, 1:ctrl w/t data */
	unsigned ret = 1;
	if (STR_EQUAL(psz_ctrl, "-dump0")) ret = 0;
	if (STR_EQUAL(psz_ctrl, "-dumpconf")) ret = 0;
	return ret;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To determine if the option control has data followed.
	@param	psz_ctrl	The string pointer to the control.
	@param	psz_ctrl	The string pointer to the data.
	@return	unsigned	FAIL/SUCCESS.
*/
static unsigned _optlist_element_add(char *psz_ctrl, char *psz_data, LINK_LIST_t *plist)
{
	PROG_OPTION_t* p_opt;

	xprintx("creating optlist element (%s, %s) ...\n", psz_ctrl, psz_data);
	p_opt = _optlist_element_new(psz_ctrl, psz_data);

	xprintx("adding option element to optlist (%s, %s) ...\n", psz_ctrl, psz_data);
	if (FAIL==llist_add_tail(plist, (unsigned)p_opt)) {
		xprintx("...error: can not add optlise !!");
		return FAIL;
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To determine if the option control has data followed.
	@param	psz_ctrl	The string pointer to the control.
	@return	unsigned	Flag to tell the type of next argv.
*/
static unsigned _prog_opt_parse_data(char *psz_data, char *psz_ctrl)
{
	/*-- ret=0: nex is data, 1:next is ctrl */
	unsigned ret = 1;
	return ret;
}
/*-------------------------------------------------------------------------*/
/**
	@brief	To get the prog cmd target string.
	@param	n			The string of n-th cmd target.
	@return	(char *)	The target string, otherwise NULL.
*/
char* prog_target_get(unsigned n)
{
	LL_NODE_t *pnode;

	if (n>p_tgtlist->num-1) return NULL;
	pnode = llist_seek(p_tgtlist, LL_SEEK_HEAD, n);
	if (!pnode) return NULL;
	return ((char *)pnode->data);
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To query an prog option.
	@param	psz_ctrl	The string of the option to query.
	@return	(PROG_OPTION_t *) the pointer to the element, otherwise NULL.
*/
PROG_OPTION_t* prog_option_query(char *psz_ctrl)
{
	return _optlist_element_find(psz_ctrl);
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To build the optlist of the program.
	@param	argc	Number of argumens.
	@param	argv	the string array of the arguments.
	@param	plist	the pointer to the option list.
	@param	pnum	pointer of the total number of found options
	@return	int		FAIL/SUCCESS.
*/
static unsigned prog_optlist_create(int argc, char *argv[], LINK_LIST_t *plist, unsigned *pnum)
{
	static char zero_sz[1]={0};
	char *p_argv, *psz_ctrl, *psz_data;
	unsigned i;
	unsigned parse_st; /*-- 0: ctrl, 1:data */
	/* PROG_OPTION_t* p_opt; */

	xprintx("%s: entering ...\n", __FUNCTION__);

	xprintx("parsing prog cmdline options...\n");
	parse_st = 0; psz_ctrl = NULL;
	for (i=1; i<argc; i++) {
		p_argv = argv[i];	xprintx("argv[%d]=%s \n", i, p_argv);

		if (!parse_st) {
			/*-- ctrl */
			psz_ctrl=p_argv;
			if (psz_ctrl[0]!='-') {
				xprintx("end of the options at argv[%d]=%s !!!\n", i, psz_ctrl);
				break;
			}
			parse_st ^= _prog_opt_parse_control(psz_ctrl);

			if (!parse_st) {
				psz_data = zero_sz;
				if (FAIL==_optlist_element_add(psz_ctrl, psz_data, plist)) return FAIL;
			}
		}
		else {
			/*-- data */
			psz_data=p_argv;
			parse_st ^= _prog_opt_parse_data(psz_data, psz_ctrl);
			if (FAIL==_optlist_element_add(psz_ctrl, psz_data, plist)) return FAIL;
		}
	}

	*pnum = (i-1); xprintx("found %d prog options ...\n", *pnum);

	return SUCCESS;
}


/*-------------------------------------------------------------------------*/
/**
	@brief	To build the tgtlist of the program.
	@param	argc	Number of argumens.
	@param	argv	the string array of the arguments.
	@param	argidx	the index of argv to look for targets.
	@param	plist	the pointer to the target list.
	@param	pnum	pointer of the total number of found targets
	@return	int		FAIL/SUCCESS.
*/
static unsigned prog_tgtlist_create(int argc, char *argv[], int argidx, LINK_LIST_t *plist, unsigned *pnum)
{
	char *p_argv;
	unsigned i;
	char* p_tgt;

	xprintx("%s: entering ...\n", __FUNCTION__);

	xprintx("scanning prog targets, starting from argv[%d]=%s...\n", argidx, argv[argidx]);
	for (i=argidx; i<argc; i++) {
		p_argv = argv[i];	xprintx("argv[%d]=%s \n", i, p_argv);

		xprintx("creating tgtlist element ...\n");
		p_tgt = _tgtlist_element_new(p_argv);

		xprintx("add option element to tgtlist ...\n");
		if (FAIL==llist_add_tail(plist, (unsigned)p_tgt)) {
			xprintx("...error: can not add tgtlist !!");
			return FAIL;
		}
	}

	*pnum = (i-argidx); xprintx("found %d prog targets ...\n", *pnum);

	return SUCCESS;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To print the usage of the program.
	@param	psz_prog_name	String of program name.
	@return	void
*/
void print_prog_usage(unsigned char *psz_prog_name)
{
	printf("\n");
	printf("SYNOPSIS:\n");
	printf("\t%s ..[OPTIONS]..JPG_FILE..\n",  psz_prog_name);
#if 0
	printf("DESCRIPTION:\n");
	printf("\tParse the implanted makernote debug info of the JPG file.\n");
	printf("\tJPG_FNAME shall specify the filename without the extension part.\n");
	printf("\tThe program default to add .jpg as the extension of the jpgeg file.\n\n");
	printf("\tTo parse the implanted info, user shall provide a .conf file.\n");
	printf("\tThe .conf file tells the format of the implanted tags.\n");
#endif
	printf("OPTION:\n");
	printf("\t -conf *conf_fname* : designate the parser conf file.\n");
	printf("\t -of *out_file* : redirect the result to out_file .\n");
	printf("\t -dbg X	: specify the debug level.\n");
	printf("\t\t X:0=SILENT, 1=ALERT, 2=VERBOSE, 3=TRACE\n");
	printf("\t -dump0	: dump active & section list.\n");
	printf("\t -dumpconf : dump the parse result of .conf file to dump_conf.txt \n");
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To release all resources created for argc_scan.
	@param	void
	@return	void
*/
static void prog_argv_release(void)
{
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To quick scan and validate the argument list.
	@param	argc	Number of argumens.
	@param	argv	the string array of the arguments.
	@return	int		FAIL/SUCCESS.
*/
unsigned prog_argv_scan(int argc, char *argv[])
{
	xprintx("%s: entering ...\n", __FUNCTION__);

	memset(gsz_prog_name, 0, MAX_FNAME_LEN);
	strcpy(gsz_prog_name, argv[0]);

	xprintx("creating optlist ... \n");
	p_optlist = llist_new("optlist");
	if (!p_optlist) {
		xprintx("... error : can not create prog option list ! \n");
		return FAIL;
	}
	if (FAIL==prog_optlist_create(argc, argv, p_optlist, &g_prog_optnum)) {
		xprintx("... error : failed to parse optlist ! \n");
		return FAIL;
	}

	xprintx("--> creating tgtlist ... \n");
	p_tgtlist = llist_new("tgtlist");
	if (!p_tgtlist) {
		xprintx("... error : can not create prog target list ! \n");
		return FAIL;
	}
	if (FAIL==prog_tgtlist_create(argc, argv, (g_prog_optnum+1), p_tgtlist, &g_prog_tgtnum)) {
		xprintx("... error : failed to parse optlist ! \n");
		return FAIL;
	}

	if (g_prog_optnum+g_prog_tgtnum+1 != argc) {
		xprintx("%s: error: opt(%d)+tgt(%d) != (argc-1)(%d) !!\n", __FUNCTION__, g_prog_optnum, g_prog_tgtnum, (argc-1));
		return FAIL;
	}

	xprintx("%s: found options(%d), targets(%d) \n", __FUNCTION__, g_prog_optnum, g_prog_tgtnum);
	return SUCCESS;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To quick scan and validate the argument list.
	@param	argc	Number of argumens.
	@param	argv	the string array of the arguments.
	@return	int		FAIL/SUCCESS.
*/
unsigned prog_argument_create(int argc, char *argv[])
{
	xprintx("%s: entering \n", __FUNCTION__);

	/*-- scan program options and targets */
	if (FAIL==prog_argv_scan(argc, argv)) {
		xprintx("argument scan failed !! \n");
		return FAIL;
	}
	return SUCCESS;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To release all resources which allocated for argument proc.
	@param	void
	@return	void
*/
void prog_argument_release(void)
{
	xprintx("%s: entering \n", __FUNCTION__);

	prog_argv_release();
}

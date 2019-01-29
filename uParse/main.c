/*-------------------------------------------------------------------------*/
/**
	@file    main.c
	@author  alex wu
	@date    Nov 2008
	@version 1.0
	@brief   Parser for JPG embedded 3a/iq info.
*/

/*---------------------------------------------------------------------------
   								Includes
 ---------------------------------------------------------------------------*/
#include <windows.h>
#include "myut_common.h"
#include "prog_arg.h"
#include "prog_dbg.h"
#include "myut_llist.h"
#include "myut_string.h"
#include "myut_numeric.h"
#include "main.h"

/*---------------------------------------------------------------------------
   								Constants
 ---------------------------------------------------------------------------*/
#define APP_SECTION_MIN_ID			(0x010)
#define ASCIILINESZ					(256)

#define SZ_SIGNATURE_MAX_LEN		(64)
#define JPG_HUNTING_MAX_SIZE		(65536)
#define SZ_DBG_CONF_DUMP_FILE		("dump_conf.txt")

/*---------------------------------------------------------------------------
   								Types
 ---------------------------------------------------------------------------*/
typedef enum  {
    LINE_UNPROCESSED,
    LINE_ERROR,
    LINE_EMPTY,
    LINE_COMMENT,
    LINE_SECTION,
    LINE_VALUE
} line_status;

typedef enum {
	SECTION_SXBASE,
	SECTION_SXSYS,
	SECTION_APP,
	SECTION_SXPRN,
	SECTION_TOTAL,
} SECTION_STAGE_t;

typedef enum {
	HUNTING_MKN,	/* maker notes */
	HUNTING_OFFSET, /* absolute offset */
	HUNTING_MVLC,	/* after main VLC */
	HUNTING_TOTAL,
} HUNTING_AREA_t;

/*---------------------------------------------------------------------------
   								Globals
 ---------------------------------------------------------------------------*/
PROG_MAIN_PARAM_t gst_mainparam =
{
	.dbg_dump_conf = FALSE,
	.fd_dump_conf = NULL,
	.fd_conf = NULL,
	.sz_conf = "uparse.conf",
	.psectlist = NULL,
	.pactlist = NULL,
};

static char line    [ASCIILINESZ+1];
static char section [ASCIILINESZ+1];
static char key     [ASCIILINESZ+1];
static char szv1    [ASCIILINESZ+1];
static char szv2    [ASCIILINESZ+1];
static char szv3    [ASCIILINESZ+1];

static unsigned		section_cnt=0;
static SECTION_STAGE_t	section_st = SECTION_SXBASE;
static unsigned 	is_act_section = 0;
static UINT8 g_act_sid=0;
static PARAM_LIST_t *g_act_paramlist = NULL;
static char gsz_signature[SZ_SIGNATURE_MAX_LEN]={0};
static HUNTING_AREA_t g_hunting_area;
static int g_hunting_offset;
static int g_hunting_size;
static char gsz_cust_show_dll[ASCIILINESZ];
static char gsz_cust_show_proc[ASCIILINESZ];
static unsigned char g_use_cust_show=FALSE;

/*===========================================================================
   							Function Definitions
 ===========================================================================*/



/*-------------------------------------------------------------------------*/
/**
*/
static void _dbg_dump_active_list(void)
{
	ACTIVE_LIST_t *pactlist;
	ACTIVE_LIST_NODE_t *pactnode;
	LL_NODE_t *pnode;
	unsigned cnt=0;

	pactlist = gst_mainparam.pactlist;
	if (!pactlist) {
		printf("\n!!! Null active list !!!\n");
		return;
	}

	printf("\n===== Active List Dump ===============================\n");
	printf("- node num : %d \n", pactlist->num);

	pnode = llist_head(pactlist);
	while(pnode) {
		pactnode = (ACTIVE_LIST_NODE_t *)pnode->data;
		if (!pactnode) { printf("\n%s,%d:... error: null node @%dth !\n", __FUNCTION__, __LINE__, cnt); break; }
		printf("%d: (%s, 0x%02X) \n", cnt, pactnode->sz_label, pactnode->sid);
		pnode = pnode->p_next;
		cnt++;
	}
	printf("===== End of Active List Dump ========================\n\n");
}

/*-------------------------------------------------------------------------*/
/**
*/
static void _dbg_dump_section_node(SECT_LIST_NODE_t *psctnode)
{
	PARAM_LIST_t *paramlist;
	LL_NODE_t *pnode;
	PARAM_LIST_NODE_t *paramnode;
	unsigned cnt;

	if (!psctnode) {
		printf("\n%s: !!! Null list !!!\n", __FUNCTION__);
		return;
	}

	printf("Section: %s, sid(0x%02X) ... \n", psctnode->sz_label, psctnode->sid);
	printf("----------------------------------------------------------\n");
	paramlist = psctnode->p_paramlist;
	pnode = llist_head(paramlist);
	cnt = 0;
	while(pnode) {
		paramnode = (PARAM_LIST_NODE_t *)pnode->data;
		if (!paramnode) break;
		printf("%04d: %s, uid(0x%04X), type(%d), size(%d) ...\n", cnt, paramnode->sz_label, paramnode->uid, paramnode->type, paramnode->len);
		pnode = pnode->p_next;
		cnt++;
	}
	printf("\n");
}

/*-------------------------------------------------------------------------*/
/**
*/
static void _dbg_dump_section_list(void)
{
	SECT_LIST_t *psctlist;
	SECT_LIST_NODE_t *psctnode;
	LL_NODE_t *pnode;
	unsigned cnt=0;

	psctlist = gst_mainparam.psectlist;
	if (!psctlist) {
		printf("\n%s: !!! Null active list !!!\n", __FUNCTION__);
		return;
	}

	printf("\n===== Section List Dump ===============================\n");
	printf("- node num : %d \n", psctlist->num);

	pnode = llist_head(psctlist);
	while(pnode) {
		psctnode = (SECT_LIST_NODE_t *)pnode->data;
		if (!psctnode) { printf("\n%s,%d:... error: null node @%dth !\n", __FUNCTION__, __LINE__, cnt); break; }
		_dbg_dump_section_node(psctnode);
		pnode = pnode->p_next;
		cnt++;
	}
	printf("===== End of Section List Dump ==========================\n\n");
}

/*-------------------------------------------------------------------------*/
/**
*/
static void _dbg_conf_dump(const char *format, ...)
{
	va_list	ap;
	char *p_vabuf;

	if (!gst_mainparam.dbg_dump_conf || !gst_mainparam.fd_dump_conf) return;

	p_vabuf = (char *)malloc(ASCIILINESZ);
	if (!p_vabuf) {
		printf("%s,%d: error: insufficient memory !!\n", __FUNCTION__, __LINE__);
		return;
	}

	va_start(ap, format);
	vsprintf(p_vabuf, format, ap);
	fprintf(gst_mainparam.fd_dump_conf, p_vabuf);
	va_end(ap);

	free(p_vabuf);
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To convert type string to numeric value.
	@param	psztype		The type label
	@return	PARAM_DTYPE_t	Numeric value
*/
static PARAM_DTYPE_t _convert_param_data_type(char *psztype)
{
	if (STR_EQUAL(psztype, "UINT16")) return PARAM_UINT16;
	else if (STR_EQUAL(psztype, "SINT16")) return PARAM_SINT16;
	else if (STR_EQUAL(psztype, "UINT32")) return PARAM_UINT32;
	else if (STR_EQUAL(psztype, "SINT32")) return PARAM_SINT32;
	else if (STR_EQUAL(psztype, "ARRAY")) return PARAM_ARRAY;
	else return PARAM_UNDEF;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To test if a specified section is a active sectoin.
	@param	pszsct		The section label
	@return	unsigned	TRUE/FALSE.
*/
static unsigned _test_section_is_existent_in_sectlist(char *pszsct)
{
	SECT_LIST_t *psectlist;
	SECT_LIST_NODE_t *psectnode;
	LL_NODE_t *pnode;

	psectlist = gst_mainparam.psectlist;
	pnode = llist_head(psectlist);
	while(pnode) {
		psectnode = (SECT_LIST_NODE_t *)pnode->data;
		if (STR_EQUAL(pszsct, psectnode->sz_label)) return TRUE;
	}
	return FALSE;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To test if a specified section is a active sectoin.
	@param	pszsct		The section label
	@return	(LL_NODE_t*)	The node ptr in the actlist if the section is active, otherwise NULL.
*/
static LL_NODE_t* _test_app_section_is_registered(char *pszsct)
{
	ACTIVE_LIST_t *pactlist = gst_mainparam.pactlist;
	LL_NODE_t *pnode;
	ACTIVE_LIST_NODE_t *p_act;

	if (!pactlist) {
		printf("%s:... fatal: null list ! \n", __FUNCTION__);
		return NULL;
	}

	pnode = llist_head(pactlist);
	while(pnode) {
		p_act = (ACTIVE_LIST_NODE_t *)pnode->data;
		if (STR_EQUAL(pszsct, p_act->sz_label)) {
			break;
		}
		pnode = pnode->p_next;
	}

	return pnode;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To query sid by section label.
	@param	pszsct		The section label
	@return	UINT8		The sid if section is registered, otherwise 0.
*/
UINT8 query_sid(char *pszsct)
{
	LL_NODE_t *pnode;
	ACTIVE_LIST_NODE_t *pactnode;

	/*-- to query if this section has been registered in SXBASE as an active section */
	if (!(pnode=_test_app_section_is_registered(pszsct))) return 0;

	pactnode = (ACTIVE_LIST_NODE_t *)pnode->data;
	return (pactnode->sid);
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To retrieve the param list by sid.
	@param	sid			The sID
	@return	(PARAM_LIST_NODE_t*)
*/
PARAM_LIST_t* get_param_list_by_sid(UINT8 sid)
{
	PARAM_LIST_t *paramlist;
	LL_NODE_t *pnode;
	SECT_LIST_t *psctlist;
	SECT_LIST_NODE_t *psctnode;

	dbg_trace("%s: sid(0x%02x) entering ...\n", __FUNCTION__, sid);

	psctlist = gst_mainparam.psectlist;
	pnode = llist_head(psctlist);
	paramlist = NULL;
	while(pnode) {
		psctnode = (SECT_LIST_NODE_t *)pnode->data;
		if (sid==psctnode->sid) { paramlist=psctnode->p_paramlist; break; }
		pnode = pnode->p_next;
	}
	if (!paramlist) return NULL;

	return paramlist;
}


/*-------------------------------------------------------------------------*/
/**
	@brief	To retrieve the param node by uid.
	@param	uid			The UID
	@return	(PARAM_LIST_NODE_t*)
*/
PARAM_LIST_NODE_t* get_param_list_node_by_uid(UINT16 uid)
{
	PARAM_LIST_t *paramlist;
	PARAM_LIST_NODE_t *paramnode;
	LL_NODE_t *pnode;
	UINT8 sid;

	dbg_trace("%s: uid(0x%04x) entering ...\n", __FUNCTION__, uid);

	sid = (UINT8)((uid & 0xFF00)>>8);
	#if 0
	psctlist = gst_mainparam.psectlist;
	pnode = llist_head(psctlist);
	paramlist = NULL;
	while(pnode) {
		psctnode = (SECT_LIST_NODE_t *)pnode->data;
		if (sid==psctnode->sid) { paramlist=psctnode->p_paramlist; break; }
		pnode = pnode->p_next;
	}
	#else
	paramlist = get_param_list_by_sid(sid);
	#endif
	if (!paramlist) return NULL;

	pnode = llist_head(paramlist);
	paramnode = NULL;
	while(pnode) {
		paramnode = (PARAM_LIST_NODE_t *)pnode->data;
		/* printf(">> this uid = 0x%04x \n", paramnode->uid); */
		if (paramnode->uid==uid) { /* printf("got it(0x%04x) <<<\n", paramnode->uid); */ break; }
		paramnode = NULL;
		pnode = pnode->p_next;
	}
	return paramnode;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To query param node by section & parameter label strings.
	@param	pszsct		The section label
	@param	pszparam	The parameter label
	@return	(PARAM_LIST_NODE_t*)
*/
PARAM_LIST_NODE_t* get_param_list_node_by_label(char *pszsct, char *pszparam)
{
	PARAM_LIST_t *paramlist;
	PARAM_LIST_NODE_t *paramnode;
	LL_NODE_t *pnode;
	UINT8 sid;

	dbg_trace("%s: section(%s), parameter(%s) entering ...\n", __FUNCTION__, pszsct, pszparam);
return NULL;
	sid = query_sid(pszsct);
	#if 0
	psctlist = gst_mainparam.psectlist;
	pnode = llist_head(psctlist);
	paramlist = NULL;
	while(pnode) {
		psctnode = (SECT_LIST_NODE_t *)pnode->data;
		if (sid==psctnode->sid) { paramlist=psctnode->p_paramlist; break; }
		pnode = pnode->p_next;
	}
	#else
	paramlist = get_param_list_by_sid(sid);
	#endif
	if (!paramlist) return NULL;

	pnode = llist_head(paramlist);
	paramnode = NULL;
	while(pnode) {
		paramnode = (PARAM_LIST_NODE_t *)pnode->data;
		/* printf(">> this uid = 0x%04x \n", paramnode->uid); */
		if (STR_EQUAL(pszparam, paramnode->sz_label)) { break; }
		paramnode = NULL;
		pnode = pnode->p_next;
	}
	return paramnode;
}


/*-------------------------------------------------------------------------*/
/**
	@brief	To parse parameters of SXSYS section.
	@param	pszkey		The section label
	@param	pszv		The string represents the value of the correspoding label.
	@return	(unsigned)	FAIL/SUCCESS.
*/
static unsigned _sxsys_param_process(char *pszkey, char *pszv)
{
	if (STR_EQUAL(pszkey, "signature")) {
		if (strlen(pszv)>SZ_SIGNATURE_MAX_LEN-1) {
			printf("... fatal: Signature is too long !!\n");
			return FAIL;
		}
		strcpy(gsz_signature, pszv);
		dbg_verbose("sxsys: signature=(%s) \n", gsz_signature);
	}
	else if (STR_EQUAL(pszkey, "hunting_area")) {
		if (STR_EQUAL(pszv, "MK_NOTES")) {
			g_hunting_area = HUNTING_MKN;
		} else if (STR_EQUAL(pszv, "MVLC")) {
			g_hunting_area = HUNTING_MVLC;
		} else if (STR_EQUAL(pszv, "OFFSET")) {
			g_hunting_area = HUNTING_OFFSET;
		} else {
			g_hunting_area = HUNTING_MKN;
		}
		dbg_verbose("sxsys: hunting_area=(%d) \n", g_hunting_area);
	}
	else if (STR_EQUAL(pszkey, "hunting_size")) {
		if (1!=sscanf(pszv, "%i", &g_hunting_size)) g_hunting_size = 0;
		if (g_hunting_size>65535) g_hunting_size = 65535;
		dbg_verbose("sxsys: hunting_size=(%d) \n", g_hunting_size);
	}
	else if (STR_EQUAL(pszkey, "hunting_offset")) {
		if (1!=sscanf(pszv, "%i", &g_hunting_offset)) g_hunting_offset = 0;
		dbg_verbose("sxsys: hunting_offset=(%d) \n", g_hunting_offset);
	}
	else if (STR_EQUAL(pszkey, "custom_show")) {
		if (STR_EQUAL(pszv, "TRUE")) {
			g_use_cust_show = TRUE;
			dbg_verbose("sxsys: using customized presentation function... \n");
		}
	}
	else if (STR_EQUAL(pszkey, "custom_show")) {
		if (STR_EQUAL(pszv, "TRUE")) {
			g_use_cust_show = TRUE;
			dbg_verbose("sxsys: enabling customized presentation function... \n");
		} else {
			g_use_cust_show = FALSE;
			dbg_verbose("sxsys: disabling customized presentation function... \n");
		}
	}
	else if (STR_EQUAL(pszkey, "custom_show_dll")) {
		memset(gsz_cust_show_dll, 0, ASCIILINESZ);
		strcpy(gsz_cust_show_dll, pszv);
		dbg_verbose("sxsys: show_dll (%s) ... \n", gsz_cust_show_dll);
	}
	else if (STR_EQUAL(pszkey, "custom_show_proc")) {
		memset(gsz_cust_show_proc, 0, ASCIILINESZ);
		strcpy(gsz_cust_show_proc, pszv);
		dbg_verbose("sxsys: show_proc (%s) ... \n", gsz_cust_show_proc);
	}
	return SUCCESS;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To register an app sectoin in the active list.
	@param	pszkey		The section label
	@param	pszbase		The string represents the sid of the correspoding label.
	@return	(ACTIVE_LIST_NODE_t*)	The node just added.
*/
static ACTIVE_LIST_NODE_t* _active_list_add(char *pszkey, char *pszsid)
{
	LL_NODE_t *pnode;
	ACTIVE_LIST_NODE_t *p_actnode;
	int ii;
	UINT8 sid = 0;

	if (1!=sscanf(pszsid, "%i", &ii)) return 0;
	sid = (UINT8)ii;
	dbg_trace("%s: %s = %s(%u(0x%2X)) ... \n", __FUNCTION__, pszkey, pszsid, sid, sid);

	if (sid<APP_SECTION_MIN_ID) {
		dbg_trace("... control section(0x%04x), ignore...\n", sid);
		return 0;
	}

	/*-- to examine if this (sect, base) param has been registered.
	 * we dont deal with the duplicated case !!
	 */
	pnode = _test_app_section_is_registered(pszkey);
	if (pnode) {
		printf("%s,%d:... warning: found duplicated key !!\n", __FUNCTION__, __LINE__);
		return 0;
	} else {
		/*-- new one */
		if (!(p_actnode=(ACTIVE_LIST_NODE_t *)malloc(sizeof(ACTIVE_LIST_NODE_t)))) {
			dbg_alert("%s,%d: insufficient memory ...\n", __FUNCTION__, __LINE__);
			return 0;
		}

		p_actnode->sid = (UINT8)sid;
		strcpy(p_actnode->sz_label, pszkey);

		if (FAIL==llist_add_tail(gst_mainparam.pactlist, (unsigned)p_actnode)) {
			dbg_alert("%s,%d: can not add actsect ...\n", __FUNCTION__, __LINE__);
			return 0;
		}
	}

	return p_actnode;
}


/*-------------------------------------------------------------------------*/
/**
	@brief	To insert the param node for current section processed.
	@param	pszsct		The section label
	@param	pszkey		The key string
	@param	pszpid		The param(key) id string
	@param	psztype		The type string
	@param	pszsize		The size string.
	@return	PARAM_LIST_NODE_t*	The ptr to the struct which stores the converted info of the parameter.
*/
static PARAM_LIST_NODE_t*  _param_list_add(char *pszsct, char *pszkey, char *pszpid, char *psztype, char *pszsize)
{
	PARAM_LIST_NODE_t *pparam;
	UINT8 pid= 0;
	UINT16 uid = 0;
	PARAM_DTYPE_t type;
	unsigned size;
	int ii;

	/*- convert param line data */
	if (1!=sscanf(pszpid, "%i", &ii)) return 0;
	pid = (UINT8)ii;
	uid = ((UINT16)(g_act_sid)<<8)+pid;

	type = _convert_param_data_type(psztype);


	if (1!=sscanf(pszsize, "%i", &ii)) return 0;
	size = (unsigned)ii;

	/*- create a node for this parameter */
	if (!(pparam=(PARAM_LIST_NODE_t *)malloc(sizeof(PARAM_LIST_NODE_t)))) {
		dbg_alert("%s,%d: insufficient memory ...\n", __FUNCTION__, __LINE__);
		return 0;
	}

	/*- construct node */
	strcpy(pparam->sz_label, pszkey);
	pparam->pid = pid;
	pparam->uid = uid;
	pparam->type = type;
	pparam->len = size;
	pparam->data_ptr = 0; /* set null pointer */

	if (FAIL==llist_add_tail(g_act_paramlist, (unsigned)pparam)) {
		dbg_alert("%s,%d: can not add param node ...\n", __FUNCTION__, __LINE__);
		return 0;
	}

	return pparam;
}


/*-------------------------------------------------------------------------*/
/**
	@brief	To add a new section node to sectlist.
	@param	psz			The section label to add.
	@return	unsigned	FAIL/SUCCESS.
*/
static unsigned _add_new_section_to_sectlist(char *pszsct)
{
	SECT_LIST_t *psectlist;
	SECT_LIST_NODE_t *psctnode;

	psectlist = gst_mainparam.psectlist;

	/*-- to check if section has existed in sectlist !!
	 * we dont allow and deal with duplicated section.
	 */
	if (_test_section_is_existent_in_sectlist(pszsct)) {
		printf("... fatal: found duplicated section (%s) !!!\n", pszsct);
		return FAIL;
	}

	dbg_trace("%s: gonna create and add a node for section (%s) ...\n", __FUNCTION__, pszsct);
	psctnode = (SECT_LIST_NODE_t *)malloc(sizeof(SECT_LIST_NODE_t));
	if (!psctnode) {
		printf("%s,%d:... fatal: insufficient memory !\n", __FUNCTION__, __LINE__);
		return FAIL;
	}
	strcpy(psctnode->sz_label, pszsct);
	g_act_sid = psctnode->sid = query_sid(pszsct);
	g_act_paramlist = psctnode->p_paramlist = (PARAM_LIST_t *)llist_new(pszsct);
	if (!psctnode->p_paramlist) {
		printf("%s,%d:... fatal: insufficient memory !\n", __FUNCTION__, __LINE__);
		free(psctnode);
		return FAIL;
	}

	return llist_add_tail(gst_mainparam.psectlist, (unsigned)psctnode);
}


/*-------------------------------------------------------------------------*/
/**
	@brief	To validate the target file with preinstalled SIGNATURE & VER strings.
	@param	pbuf		The hunting buffer
	@return	unsigned	The offset value from the pbuf where signature is found, otherwise 0(ZERO).
*/
static unsigned jpg_signature_ver_check(char *pbuf, unsigned size)
{
	char *psz;
	unsigned a;

	a = strlen(UPARSE_JPG_SIGNATURE) + strlen(UPARSE_VER_SZ) +2;

	dbg_trace("checking signature(%s) & version(%s) ...\n", UPARSE_JPG_SIGNATURE, UPARSE_VER_SZ);

	psz = pbuf;
	while ((psz+a) < (pbuf+size)) {
		if (!STR_EQUAL(psz, UPARSE_JPG_SIGNATURE)) { psz++; continue; }
		psz += (1+strlen(UPARSE_JPG_SIGNATURE));

		if (!STR_EQUAL(psz, UPARSE_VER_SZ)) { continue; }

		psz += (1+strlen(UPARSE_VER_SZ));

		return (psz-pbuf);
	}

	return 0;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To parse JPG file for embedded tag data.
	@param	pbuf		The hunting buffer
	@param	hunt_size	The size of data to look for
	@return	unsigned	FAIL/SUCCESS.
*/
static unsigned jpg_tag_pattern_parse(char *pbuf, unsigned hunt_size)
{
	UINT16 *puid, uid;
	PARAM_LIST_NODE_t *paramnode;
	char *pbufnext;
	unsigned type, size;
	UINT32 *pu32;
	SINT32 *ps32;
	UINT16 *pu16;
	SINT16 *ps16;
	int col, row, icnt;

	pbufnext = pbuf;

	/* printf("pbuf=0x%08x, pbufnext=0x%08x, hunt_size(%d) \n", pbuf, pbufnext, hunt_size);	*/
	/* hunt_size = 30; */

	printf("\n");
	while(1) {
		if ((pbufnext+sizeof(UINT16))>=(pbuf+hunt_size)) { dbg_trace("%s:... end of huntbuf \n", __FUNCTION__); break; }
		puid = (UINT16 *)pbufnext;
		uid = to_big_endian16(*puid);
		/*printf("uid=0x%04x \n", uid); */
		paramnode = get_param_list_node_by_uid(uid);
		if (!paramnode) {
			pbufnext++;
			continue;
		}
		else {
			pu16 = (UINT16 *)(pbufnext+sizeof(UINT16));
			type = (*pu16 & 0xF000)>>12; size = (*pu16 & 0x0FFF);
			if ((paramnode->type != type) || (paramnode->len != size)) {
				/*-- this is a fake parameter !!! */
				pbufnext++;
				continue;
			}
			pbufnext += 4; /* 16-bit uid, 16-bit type_size */

			switch (type) {
			default:
			case PARAM_ARRAY:
				if ((pbufnext+size)>=(pbuf+hunt_size)) { dbg_trace("%s:... end of huntbuf \n", __FUNCTION__); break; }

				if (g_use_cust_show) {
					unsigned char *pchary;
					pchary = (unsigned char *)malloc(size);
					if (!pchary) { printf("%s, %d:... fatal: insufficient memory !! \n", __FUNCTION__, __LINE__); return FAIL; }
					memcpy(pchary, pbufnext, size);
					pbufnext += size;
					paramnode->data_ptr = (unsigned)pchary;
					break;
				}

				printf("%s = \n", paramnode->sz_label);
				icnt = 0;
				for (row=0; row<(size+7)/8; row++) {
					for (col=0; col<8; col++) {
						if (icnt>=size) break;
						printf("%02x ", *pbufnext);
						pbufnext++;
						icnt++;
					}
					printf("\n");
				}
				break;

			case PARAM_UINT16:
				icnt = size*sizeof(UINT16);
				if ((pbufnext+icnt)>=(pbuf+hunt_size)) { dbg_trace("%s:... end of huntbuf \n", __FUNCTION__); break; }

				if (g_use_cust_show) {
					pu16 = (UINT16 *)malloc(icnt);
					if (!pu16) { printf("%s, %d:... fatal: insufficient memory !! \n", __FUNCTION__, __LINE__); return FAIL; }
					memcpy(pu16, pbufnext, icnt);
					pbufnext += icnt;
					paramnode->data_ptr = (unsigned)pu16;
					break;
				}

				pu16 = (UINT16 *)pbufnext;
				pbufnext += (sizeof(UINT16)*size);
				if (size==1) printf("%s = 0x%04x \n", paramnode->sz_label, *pu16);
				else {
					printf("%s = \n", paramnode->sz_label);
					icnt = 0;
					for (row=0; row<(size+7)/8; row++) {
						for (col=0; col<8; col++) {
							if (icnt>=size) break;
							printf("%04x ", *pu16);
							pu16++;
							icnt++;
						}
						printf("\n");
					}
				}
				break;

			case PARAM_SINT16:
				icnt = size*sizeof(UINT16);
				if ((pbufnext+icnt)>=(pbuf+hunt_size)) { dbg_trace("%s:... end of huntbuf \n", __FUNCTION__); break; }

				if (g_use_cust_show) {
					pu16 = (UINT16 *)malloc(icnt);
					if (!pu16) { printf("%s, %d:... fatal: insufficient memory !! \n", __FUNCTION__, __LINE__); return FAIL; }
					memcpy(pu16, pbufnext, icnt);
					pbufnext += icnt;
					paramnode->data_ptr = (unsigned)pu16;
					break;
				}

				ps16 = (SINT16 *)pbufnext;
				pbufnext += (sizeof(SINT16)*size);
				if (size==1) printf("%s = %d \n", paramnode->sz_label, *ps16);
				else {
					printf("%s = \n", paramnode->sz_label);
					icnt = 0;
					for (row=0; row<(size+7)/8; row++) {
						for (col=0; col<8; col++) {
							if (icnt>=size) break;
							printf("%d ", *ps16);
							ps16++;
							icnt++;
						}
						printf("\n");
					}
				}
				break;

			case PARAM_UINT32:
				icnt = size*sizeof(UINT32);
				if ((pbufnext+icnt)>=(pbuf+hunt_size)) { dbg_trace("%s:... end of huntbuf \n", __FUNCTION__); break; }

				if (g_use_cust_show) {
					pu32 = (UINT32 *)malloc(icnt);
					if (!pu32) { printf("%s, %d:... fatal: insufficient memory !! \n", __FUNCTION__, __LINE__); return FAIL; }
					memcpy(pu32, pbufnext, icnt);
					pbufnext += icnt;
					paramnode->data_ptr = (unsigned)pu32;
					break;
				}

				pu32 = (SINT32 *)pbufnext;
				pbufnext += (sizeof(UINT32)*size);
				if (size==1) printf("%s = 0x%08x \n", paramnode->sz_label, *pu32);
				else {
					printf("%s = \n", paramnode->sz_label);
					icnt = 0;
					for (row=0; row<(size+7)/8; row++) {
						for (col=0; col<8; col++) {
							if (icnt>=size) break;
							printf("0x%08x ", *pu32);
							pu32++;
							icnt++;
						}
						printf("\n");
					}
				}
				break;

			case PARAM_SINT32:
				icnt = size*sizeof(UINT32);
				if ((pbufnext+icnt)>=(pbuf+hunt_size)) { dbg_trace("%s:... end of huntbuf \n", __FUNCTION__); break; }

				if (g_use_cust_show) {
					pu32 = (UINT32 *)malloc(icnt);
					if (!pu32) { printf("%s, %d:... fatal: insufficient memory !! \n", __FUNCTION__, __LINE__); return FAIL; }
					memcpy(pu32, pbufnext, icnt);
					pbufnext += icnt;
					paramnode->data_ptr = (unsigned)pu32;
					break;
				}

				ps32 = (SINT32 *)pbufnext;
				pbufnext += (sizeof(SINT32)*size);
				if (size==1) printf("%s = %d \n", paramnode->sz_label, *ps32);
				else {
					printf("%s = \n", paramnode->sz_label);
					icnt = 0;
					for (row=0; row<(size+7)/8; row++) {
						for (col=0; col<8; col++) {
							if (icnt>=size) break;
							printf("%d ", *ps32);
							ps32++;
							icnt++;
						}
						printf("\n");
					}
				}
				break;
			}
		}
	}
}

/*-------------------------------------------------------------------------*/
/**
*/
static unsigned parse_key(char *line, char *key, char *sz1, char *sz2, char *sz3)
{
#if 0
	int i;
	i = sscanf(line, "%[^=] = \"%[^,\"]\" , %[^,;#] ,  %[^,;#]", key, sz1, sz2, sz3);
	if (i>1) printf("1: i=%d, key(%s), sz1(%s), sz2(%s), sz3(%s) ...\n", i, key, sz1, sz2, sz3 );
	i = sscanf(line, "%[^=] = '%[^,\']' , %[^,;#] , %[^,;#]", key, sz1, sz2, sz3);
	if (i>1) printf("2: i=%d, key(%s), sz1(%s), sz2(%s), sz3(%s) ...\n", i, key, sz1, sz2, sz3 );
	i = sscanf(line, "%[^=] = %[^,;#] , %[^,;#] , %[^,;#]", key, sz1, sz2, sz3);
	if (i>1) printf("3: i=%d, key(%s), sz1(%s), sz2(%s), sz3(%s) ...\n", i, key, sz1, sz2, sz3 );
	return SUCCESS;
#endif

	if (	sscanf(line, "%[^=] = \"%[^,\"]\" , %[^,;#] , %[^,;#]", key, sz1, sz2, sz3)>1
		||  sscanf(line, "%[^=] = '%[^,\']' , %[^,;#] , %[^,;#]", key, sz1, sz2, sz3)>1
		||  sscanf(line, "%[^=] = %[^,;#] , %[^,;#] , %[^,;#]",  key, sz1, sz2, sz3)>1 ) {
		/* Usual key=value, with or without comments */
		strcpy(key, strstrip(key)); strcpy(key, strlwc(key));
		strcpy(sz1, strstrip(sz1));
		strcpy(sz2, strstrip(sz2));
		strcpy(sz3, strstrip(sz3));
		/*
		 * sscanf cannot handle '' or "" as empty values
		 * this is done here
		 */
		if (STR_QUOTE_EMPTY(sz1)) sz1[0]=0;
		if (STR_QUOTE_EMPTY(sz2)) sz2[0]=0;
		if (STR_QUOTE_EMPTY(sz3)) sz3[0]=0;

		return SUCCESS;
	} else {
		return FAIL;
	}
}

/*-------------------------------------------------------------------------*/
/**
  @brief	To parse a single line of the conf file.
  @param    sctype		The type of section to be parse
  @param    input_line  Input line
  @param    section     Output space to store section
  @param    key         Output space to store param_label
  @param    pid         Output space to store param_id
  @param    type       	Output space to store param_type
  @param    size       	Output space to store param_size
  @return   line_status value
 */
static line_status parse_line(char *input_line, char *section, char *key, char *sz1, char *sz2, char *sz3)
{
    line_status sta;
    char        line[ASCIILINESZ+1];
    int         len ;

    strcpy(line, strstrip(input_line));
    len = (int)strlen(line);

    sta = LINE_UNPROCESSED ;
    if (len<1) {
        /* Empty line */
        sta = LINE_EMPTY ;
    } else if (line[0]==';' || line[0]=='#') {
        /* Comment line */
        sta = LINE_COMMENT ;
    } else if (line[0]=='[' && line[len-1]==']') {
        /* Section name */
        sscanf(line, "[%[^]]", section);
        strcpy(section, strstrip(section));
        strcpy(section, strlwc(section));
        sta = LINE_SECTION ;
    } else if (SUCCESS==parse_key(line, key, sz1, sz2, sz3)) {
        sta = LINE_VALUE ;
    } else {
        /* Generate syntax error */
        sta = LINE_ERROR ;
    }
    return sta ;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To process the section read.
	@param	psz			The section to process
	@return	unsigned	FAIL/SUCCESS.
*/
static unsigned conf_line_section_process(char *pszsct)
{
	dbg_trace("%s: (%s) entering \n", __FUNCTION__, pszsct);

	is_act_section = FALSE;

	if (STR_EQUAL(pszsct, "sxbase")) {
		if (section_st!=SECTION_SXBASE) {
			dbg_alert("...fatal: SXBASE is not the first section!! \n");
			return FAIL;
		}
		section_st = SECTION_SXBASE;
	} else if (STR_EQUAL(pszsct, "sxsys")) {
		section_st = SECTION_SXSYS;
	} else if (STR_EQUAL(pszsct, "sxprn")) {
		section_st = SECTION_SXPRN;
	} else {
		/*-- it is APP section
		*/
		section_st = SECTION_APP;
		/*-- to query if this section has been registered in SXBASE as an active section */
		if (_test_app_section_is_registered(pszsct)) {
			dbg_trace("processing active section(%s) ... \n", pszsct);
			is_act_section = TRUE;
			_add_new_section_to_sectlist(pszsct);
		} else {
			dbg_trace("ignoring inactive section(%s) \n", pszsct);
			g_act_sid = 0;
			g_act_paramlist = NULL;
		}
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To process the param line read.
	@param	pszsct		The 'section' string
	@param	pszkey		The 'key' string
	@param	pszv1		The 1st value string
	@param	pszv2		The 2nd value string
	@param	pszv3		The 3rd value string
	@return	unsigned	FAIL/SUCCESS.
*/
static unsigned conf_line_param_process(char *pszsct, char *pszkey, char *pszv1, char*pszv2, char *pszv3)
{
	dbg_trace("%s: %s, %s entering ...\n", __FUNCTION__, pszsct, pszkey);

	switch (section_st) {
	default:
		dbg_trace("%s: unknown section stage (%d) ...\n", __FUNCTION__, section_st);
		break;
	case SECTION_SXBASE:
		dbg_trace("sxbase: adding (%s)=%s to active list ...\n", pszkey, pszv1);
		if (!_active_list_add(pszkey, pszv1)) return FAIL;
		break;
	case SECTION_SXSYS:
		dbg_trace("sxsys: processing (%s)=(%s) ...\n", pszkey, pszv1);
		_sxsys_param_process(pszkey, pszv1);
		break;
	case SECTION_APP:
		dbg_trace("section(%s): adding (%s)=%s, %s, %s to list ...\n", pszsct, pszkey, pszv1, pszv2, pszv3);
		if (!_param_list_add(pszsct, pszkey, pszv1, pszv2, pszv3)) return FAIL;
		break;
	case SECTION_SXPRN:
		printf("sxprn: Not yet completed !!\n");
		break;
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To parse the .conf file.
	@param	void
	@return	FAIL/SUCCESS.
*/
unsigned conf_file_parse(void)
{
	FILE *fd;

    int  len ;
    int  lineno=0 ;
    int  errs=0;

	dbg_trace("%s: entering ...\n", __FUNCTION__);

	if (NULL==(fd=gst_mainparam.fd_conf)) {
		printf("%s:... fatal: null conf fd !!\n", __FUNCTION__);
		return FAIL;
	}

	memset(line,    0, ASCIILINESZ);
	memset(section, 0, ASCIILINESZ);
	memset(key,     0, ASCIILINESZ);
	memset(szv1,     0, ASCIILINESZ);
	memset(szv2,     0, ASCIILINESZ);
	memset(szv3,     0, ASCIILINESZ);

	dbg_trace("%s: begin line parse loop \n", __FUNCTION__);

	/*-- read in a line and parse */
	while (fgets(line, ASCIILINESZ, fd)!=NULL) {
		lineno++;

		if (FAIL==test_line_length(line, ASCIILINESZ)) {
			printf("line exceeds (%d), too long! 寫這麼長不累嗎？\n", ASCIILINESZ);
			return FAIL ;
		}

		/*-- Get rid of the trailing '\n' and spaces at end of line */
		len = (int)strlen(line)-1; /* to position the last char */
		while ((len>=0) && ((line[len]=='\n') || (isspace(line[len])))) {
			line[len]=0 ;
			len-- ;
		}

		switch (parse_line(line, section, key, szv1, szv2, szv3)) {
			case LINE_EMPTY:
			_dbg_conf_dump("empty line...\n");
			break;

			case LINE_COMMENT:
			_dbg_conf_dump("comment line...\n");
			break ;

			case LINE_SECTION:
			_dbg_conf_dump("section: section=(%s) ...\n", section);
			section_cnt++;
			conf_line_section_process(section);
			break ;

			case LINE_VALUE:
			_dbg_conf_dump("key-val: key(%s) = (%s), (%s), (%s) ...\n", key, szv1, szv2, szv3);
			conf_line_param_process(section, key, szv1, szv2, szv3);
			break ;

			case LINE_ERROR:
			_dbg_conf_dump("error line@(%d) : (%s) \n", lineno, line);
			errs++ ;
			break;

			default:
			break ;
		}
		memset(line, 0, ASCIILINESZ); /* clear content of the line buffer */
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To parse the .jpg file.
	@param	pszjpg		The fname string of the JPG file.
	@return	unsigned 	FAIL/SUCCESS.
*/
static unsigned jpg_file_parse(char *pszjpg)
{
	static FILE *fd_jpg;
	char *pszhuntbuf, *pbufsave;
	unsigned hunting_size;
	unsigned sig_ver_offset;

	dbg_trace("%s: JPG(%s) entering ... \n", __FUNCTION__, pszjpg);

	if (!pszjpg) { dbg_alert("... warning: null fname string !\n"); return FAIL; }

	hunting_size = (g_hunting_size>JPG_HUNTING_MAX_SIZE) ? JPG_HUNTING_MAX_SIZE : g_hunting_size;
	if (!(pszhuntbuf=(char *)malloc(hunting_size))) {
		dbg_alert("... warning: insufficient memory for huntbuf !!\n");
		return FAIL;
	}

	dbg_trace("g_hunting_size=%d, hunting_size=%d \n", g_hunting_size, hunting_size);

	pbufsave = pszhuntbuf;

	/*-- open target jpg file */
	dbg_verbose("processing jpg(%s) ...\n", pszjpg);
	fd_jpg = fopen(pszjpg, "rb");
	if (!fd_jpg) {
		printf("... fatal: can not open JPG (%s) ...\n", pszjpg);
		return FAIL;
	}

	/*-- locate hunting area */
	dbg_trace("seeking pointer to offset 0x%08x ... \n", g_hunting_offset);
	fseek(fd_jpg, g_hunting_offset, SEEK_SET);
	hunting_size = fread(pszhuntbuf, sizeof(char), hunting_size, fd_jpg);
	dbg_trace("size of hunting buf = %d ... \n", hunting_size);

	/*-- hunt signature */
	sig_ver_offset = jpg_signature_ver_check(pszhuntbuf, hunting_size);
	dbg_trace("sig_ver_offset = %d \n", sig_ver_offset);
	if (!sig_ver_offset) {
		printf("... fatal: can not find signature & ver strings !!\n");
		return FAIL;
	}

	/*-- tag pattern hunt & parse */
	jpg_tag_pattern_parse(pszhuntbuf+sig_ver_offset, hunting_size-sig_ver_offset);

	if (g_use_cust_show) {
		typedef void (*pfdll_t)(char *szjpg);
		HANDLE hdll;

		pfdll_t pf_custom_show;

		hdll = LoadLibrary("my_show.dll");

		pf_custom_show = (pfdll_t)GetProcAddress(hdll, "custom_show");

		pf_custom_show(pszjpg);
	}

	/*-- clean up */
	free(pbufsave);
	fclose(fd_jpg);

	return SUCCESS;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	The main entry.
	@param	argc	...
	@param	argv	....
	@return	...
*/
int main(int argc, char *argv[])
{
	PROG_OPTION_t *p_elmt;

	if (argc<2) { print_prog_usage(argv[0]); goto _exit; }

	/*-- program argument process
	 */
	if (FAIL==prog_argument_create(argc, argv)) {
		printf("%s: fatal: can not create program arguments ...\n", argv[0]);
		goto _exit;
	}

	/*-- preprocess according to prog_argument
	*/

	/*-- override program debug level
	*/
	unsigned dl = get_debug_level();
	p_elmt = prog_option_query("-dbg");
	if (p_elmt) {
		/* printf("... dbg level = %s \n", p_elmt->sz_data);*/
		if (1==sscanf(p_elmt->sz_data, "%u", &dl)) {
			if (dl>DEBUG_TOTAL-1) dl = DEBUG_TOTAL-1;
			printf("\n... overriding dbg_level = %d \n", dl);
		}
	}
	set_debug_level(dl);

	/*-- debug dump for post .conf parse ...
	 */
	p_elmt = prog_option_query("-dumpconf");
	if (p_elmt) {
		if (!(gst_mainparam.fd_dump_conf=fopen(SZ_DBG_CONF_DUMP_FILE, "w"))) {
			gst_mainparam.dbg_dump_conf = FALSE;
			dbg_alert("... warning: can not create/open dump file (%s) ! \n", SZ_DBG_CONF_DUMP_FILE);
		}
		gst_mainparam.dbg_dump_conf = TRUE;
	}

	/*-- validate .conf file
	 */
	p_elmt = prog_option_query("-conf");
	if (p_elmt) {
		if (strlen(p_elmt->sz_data)>(MAX_FNAME_LEN-1)) {
			dbg_alert("memory shortage for conf filename !\n");
			goto _exit;
		}
		strcpy(gst_mainparam.sz_conf, p_elmt->sz_data);
	}

	dbg_alert("openning parser .conf file ... (%s) \n", gst_mainparam.sz_conf);
	gst_mainparam.fd_conf = fopen(gst_mainparam.sz_conf, "r");
	if (!gst_mainparam.fd_conf) {
		dbg_alert("... error: failed to open conf file(%s) !!\n", gst_mainparam.sz_conf);
		goto _exit;
	}

    /*-- initialize main resources
     */
	dbg_alert("allocating program resources ... \n");
    gst_mainparam.pactlist= (ACTIVE_LIST_t *)llist_new("active_list");
    if (!gst_mainparam.pactlist) {
		printf("... fatal: failed to create list for active sections !!\n");
		goto _exit;
    }

	gst_mainparam.psectlist = (SECT_LIST_t *)llist_new("section_list");
    if (!gst_mainparam.psectlist) {
		printf("... fatal: failed to create list for the app lists !!\n");
		goto _exit;
    }

    /*-- parse .conf file
     */
    g_use_cust_show = FALSE;	/* reset customized presentation fucntion */
	dbg_alert("gonna parse .conf file ... \n");
    conf_file_parse();


    /*-- debug dump .conf parse result
     */
	p_elmt = prog_option_query("-dump0");
	if (p_elmt) {
	    _dbg_dump_active_list();
	    _dbg_dump_section_list();
	}

	/*-- process JPG file
	 */
	dbg_alert("gonna parse JPG file ... \n");
	jpg_file_parse(prog_target_get(0));

	/*-- program error exit
	 */
_exit:
	prog_argument_release();

	if (gst_mainparam.fd_dump_conf) fclose(gst_mainparam.fd_dump_conf);
	if (gst_mainparam.fd_conf) fclose(gst_mainparam.fd_conf);

	exit(0);
}

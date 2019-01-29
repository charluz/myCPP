#ifndef _MAIN_H_
#define _MAIN_H_

/*---------------------------------------------------------------------------
   								Includes
 ---------------------------------------------------------------------------*/
#include "uparse_export.h"
#include "plugin_service.h"

/*---------------------------------------------------------------------------
   								Constants
 ---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
   								Types
 ---------------------------------------------------------------------------*/


/**
	@brief		Parameter list of a section.

	This type defines the template list which is to store all parameter line of a section.
	The element node of this list is defined by type (PARAM_LIST_NODE_t).
*/
typedef LINK_LIST_t PARAM_LIST_t;

/**
	@brief		Elementary node of SECT_LIST_t.

	This type defines the elementary node of the list, SECT_LIST_t.
	Each app section found is described by this template structure with
	its label string, section id, and a list to manage its parameters.
*/
typedef struct {
	char 			sz_label[SECT_LABEL_MAX_LEN]; /* section label string */
	UINT8			sid;	/* section_id */
	PARAM_LIST_t	*p_paramlist;	/* the list to store all param (PARAM_LIST_NODE_t) of the section */
} SECT_LIST_NODE_t;

/**
	@brief		APP Secetion list template.

	This type defines the template list which is to manage all the app sectons found.
	Each node of this list represents a specific app section found in conf file.
*/
typedef LINK_LIST_t	SECT_LIST_t;

/**
	@brief		Node structure of the Active APP section list.

	This type defines node structure of the Active list.
	The app sections specified in the SXBASE section are regarded as active sections.
	All these active sectoins are managed in a list of the type ACTIVE_LIST_t
*/
typedef struct {
	char 			sz_label[SECT_LABEL_MAX_LEN];
	UINT8			sid;	/* section_id */
} ACTIVE_LIST_NODE_t;


/**
	@brief		Type tempalate of the Active section list.

	See ACTIVE_LIST_NODE_t.
*/
typedef LINK_LIST_t	ACTIVE_LIST_t;


/**
	@brief		Program Main parameters.
*/
typedef struct {
	unsigned	dbg_dump_conf;	/* debug sw to dump parsed conf file */
	FILE		*fd_dump_conf;
	/*-------------------------*/
	FILE 		*fd_conf;		/* the fd of .conf file */
	char 		sz_conf[MAX_FNAME_LEN];	/* the fname str of .conf file */
	/*-------------------------*/
	ACTIVE_LIST_t	*pactlist;		/* the llist to store the label & section id of al active sections */
	SECT_LIST_t		*psectlist;		/* the llist to store all app sections */
} PROG_MAIN_PARAM_t;

/*---------------------------------------------------------------------------
  							Function prototypes
 ---------------------------------------------------------------------------*/
UINT8 query_sid(char *pszsct);
PARAM_LIST_t* get_param_list_by_sid(UINT8 sid);
PARAM_LIST_NODE_t* get_param_list_node_by_uid(UINT16 uid);
PARAM_LIST_NODE_t* get_param_list_node_by_label(char *pszsct, char *pszparam);

#endif /* _MAIN_H_ */

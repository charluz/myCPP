#ifndef _PLUGIN_SERVICE_H_
#define _PLUGIN_SERVICE_H_

/*---------------------------------------------------------------------------
   								Includes
 ---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
   								Constants
 ---------------------------------------------------------------------------*/
#define SECT_LABEL_MAX_LEN			(32)
#define PARAM_LABEL_MAX_LEN			(32)

/*---------------------------------------------------------------------------
   								Types
 ---------------------------------------------------------------------------*/

/**
	@brief		Elementary node of the list, PARAM_LIST_t.

	This type defines the elementary node of the PARAM_LIST_t list.
	Each parameter of a section is to described as a node in the PARAM_LSIT_t list.
	The info to specify a parameter contains a label string, a param id, a unique id
	which is the combination of [sid:pid], its data type, and the length os the data.
*/
typedef struct {
	char			sz_label[PARAM_LABEL_MAX_LEN];
	UINT8			pid;	/* param_id */
	UINT16			uid;	/* the unique id of prarm, uid=[section_id:param_id] */
	/*---------*/
	PARAM_DTYPE_t	type;	/* the data type, see PARAM_DTYPE_t */
	unsigned		len;	/* the length of the data */
	/*---------*/
	unsigned		data_ptr;	/* the data pointer to the retrieved data */
} PARAM_LIST_NODE_t;


#endif /* _PLUGIN_SERVICE_H_ */

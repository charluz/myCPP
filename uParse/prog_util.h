/*-------------------------------------------------------------------------*/
/**
	@file    myut_llist.h
	@author  alex wu
	@date    Nov 2008
	@version 1.0
	@brief   The header file of prog_util.c.
*/

#ifndef _MYUT_LLIST_H_
#define _MYUT_LLIST_H_

/*---------------------------------------------------------------------------
   								Includes
 ---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/*---------------------------------------------------------------------------
   								Macros
 ---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
   								Constants
 ---------------------------------------------------------------------------*/
#define LINK_LIST_LABEL_MAX_SIZE		(32)

/*---------------------------------------------------------------------------
   								Types
 ---------------------------------------------------------------------------*/
typedef enum {
	LL_SEEK_HEAD,
	LL_SEEK_TAIL,
	LL_SEEN_TOTAL,
} LL_SEEK_OPT_t;

typedef struct  ll_node_s {
	struct ll_node_s	*p_next;
	struct ll_node_s	*p_prev;
	unsigned 	data;
} LL_NODE_t;

typedef struct {
	char		sz_label[LINK_LIST_LABEL_MAX_SIZE];
	unsigned	num;
	LL_NODE_t	*phead;
	LL_NODE_t	*ptail;
} LINK_LIST_t;


/*---------------------------------------------------------------------------
  							Function prototypes
 ---------------------------------------------------------------------------*/
LINK_LIST_t *llist_new(char *psz_label);
void llist_free(LINK_LIST_t *plist);
unsigned llist_del_tail(LINK_LIST_t *plist);
unsigned llist_del_head(LINK_LIST_t *plist);
unsigned llist_add_tail(LINK_LIST_t *plist, unsigned d);
unsigned llist_add_head(LINK_LIST_t *plist, unsigned d);
LL_NODE_t *llist_seek(LINK_LIST_t *plist, unsigned seek_opt, unsigned offset);
LL_NODE_t *llist_head(LINK_LIST_t *plist);
LL_NODE_t *llist_tail(LINK_LIST_t *plist);

#endif /* _MYUT_LLIST_H_ */

/*-------------------------------------------------------------------------*/
/**
	@file    myut_numeric.c
	@author  alex wu
	@date    Nov 2008
	@version 1.0
	@brief   utility fucntion for my project.
*/

/*---------------------------------------------------------------------------
   								Includes
 ---------------------------------------------------------------------------*/
#include "myut_common.h"
#include "prog_dbg.h"
#include "myut_llist.h"

/*-- utility module printf */
#define LLIST_DEBUG			0
#if LLIST_DEBUG
#define xprintx(fmt, arg...)		printf(fmt, ##arg)
#else
#define xprintx(fmt, arg...)		do {} while(0)
#endif /* ARG_DEBUG */


/*===========================================================================
   							Function Definitions
 ===========================================================================*/

/*-------------------------------------------------------------------------*/
/**
	@brief	To empty a node of a list.
	@param	pnode			The pointer to the node.
	@return	void
*/
static void _node_empty_init(LL_NODE_t *pnode)
{
	pnode->data = 0;
	pnode->p_next = pnode->p_prev = NULL;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To test if a empty list.
	@param	plist		The list to be tested.
	@return	unsigned	TRUE/FALSE
*/
static unsigned _llist_empty_test(LINK_LIST_t *plist)
{
	if (plist->phead==NULL || plist->phead==NULL || plist->num==0)
		return TRUE;
	else
		return FALSE;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To create a new and empty link list.
	@param	void
	@return	LINK_LIST_t*	The pointer to the created list.
*/
LINK_LIST_t *llist_new(char *psz_label)
{
	LINK_LIST_t *plist;

	if (strlen(psz_label)>(LINK_LIST_LABEL_MAX_SIZE-1)) {
		printf("%s: length of label exceeds limitation ! \n", __FUNCTION__);
		return NULL;
	}
	plist = (LINK_LIST_t *)malloc(sizeof(LINK_LIST_t));
	if (!plist) {
		printf("%s: error: insufficient memory !\n", __FUNCTION__);
		return NULL;
	}

	plist->num = 0;
	strcpy(plist->sz_label, psz_label);
	plist->phead = NULL; plist->ptail = NULL;

	return plist;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To release the link list.
	@param	LINK_LIST_t*	The pointer to the list  to be released.
	@return	void
*/
void llist_free(LINK_LIST_t *plist)
{
	LL_NODE_t *p_node;

	if (!plist) {
		printf("... warning: null list pointer ...\n");
		return;
	}

	dbg_trace("gonna release llist(%s) ...\n", plist->sz_label);
	if (plist->num==0) { return; }

	dbg_trace("releasing list nodes ...\n");
	p_node = llist_head(plist);
	while(p_node) {
		p_node = p_node->p_next;
		free(p_node);
	}

	dbg_trace("releasing list body ...\n");
	free(plist);
	plist = NULL;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To delete a node from the beginning of a list.
	@param	plist		The pointer to the link list.
	@return	unsigned	SUCCESS/FAIL.
*/
unsigned llist_del_head(LINK_LIST_t *plist)
{
	LL_NODE_t *p_newhead, *p_node;

	if (!plist) {
		printf("%s: error: null list ! \n", __FUNCTION__);
		return FAIL;
	}

	p_node = (LL_NODE_t *)llist_head(plist);
	if (!p_node) {
		printf("%s,%d: warning: empty list ... \n", __FUNCTION__, __LINE__);
		return FAIL;
	}

	if (plist->num==1) {
		plist->ptail = NULL;
		plist->phead = NULL;
	}
	else {
		p_newhead = p_node->p_next;
		p_newhead->p_prev = NULL;
		plist->phead = p_newhead;
	}

	plist->num--;
	free(p_node);

	return SUCCESS;
}



/*-------------------------------------------------------------------------*/
/**
	@brief	To delete a node at the end of a list.
	@param	plist		The pointer to the link list.
	@return	unsigned	SUCCESS/FAIL.
*/
unsigned llist_del_tail(LINK_LIST_t *plist)
{
	LL_NODE_t *p_newtail, *p_node;

	if (!plist) {
		printf("%s: error: null list ! \n", __FUNCTION__);
		return FAIL;
	}

	p_node = (LL_NODE_t *)llist_tail(plist);
	if (!p_node) {
		printf("%s,%d: warning: empty list ... \n", __FUNCTION__, __LINE__);
		return FAIL;
	}

	if (plist->num==1) {
		plist->ptail = NULL;
		plist->phead = NULL;
	}
	else {
		p_newtail = p_node->p_prev;
		p_newtail->p_next = NULL;
		plist->ptail = p_newtail;
	}

	plist->num--;
	free(p_node);

	return SUCCESS;
}


/*-------------------------------------------------------------------------*/
/**
	@brief	To add a node at the end of a list.
	@param	plist		The pointer to the link list.
	@param	d			the node data.
	@return	unsigned	SUCCESS/FAIL.
*/
unsigned llist_add_tail(LINK_LIST_t *plist, unsigned d)
{
	LL_NODE_t *p_node;

	if (!plist) {
		printf("%s: error: null list ! \n", __FUNCTION__);
		return FAIL;
	}

	p_node = (LL_NODE_t *)malloc(sizeof(LL_NODE_t));
	if (!p_node) {
		printf("%s,%d: error: insufficient memory ... \n", __FUNCTION__, __LINE__);
		return FAIL;
	}
	p_node->data = d;

	if (plist->ptail==NULL || plist->phead==NULL) {
		/*-- ptail=NULL or phead=NULL, then it is a empty list */
		plist->ptail = p_node;
		plist->phead = p_node;
		p_node->p_next = NULL;
		p_node->p_prev = NULL;
	}
	else {
		/*-- not an empty list ! */
		p_node->p_prev = plist->ptail;
		p_node->p_next = NULL;
		plist->ptail->p_next = p_node;
		plist->ptail = p_node;
	}
	plist->num++;

	return SUCCESS;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To add a node at the beginning of a list.
	@param	plist		The pointer to the link list.
	@param	d			the node data.
	@return	unsigned	SUCCESS/FAIL.
*/
unsigned llist_add_head(LINK_LIST_t *plist, unsigned d)
{
	LL_NODE_t *p_node;

	if (!plist) {
		printf("%s: error: null list ! \n", __FUNCTION__);
		return FAIL;
	}

	p_node = (LL_NODE_t *)malloc(sizeof(LL_NODE_t));
	if (!p_node) {
		printf("%s,%d: error: insufficient memory ... \n", __FUNCTION__, __LINE__);
		return FAIL;
	}
	p_node->data = d;

	if (plist->ptail==NULL || plist->phead==NULL) {
		/*-- ptail=NULL or phead=NULL, then it is a empty list */
		plist->ptail = p_node;
		plist->phead = p_node;
		p_node->p_next = NULL;
		p_node->p_prev = NULL;
	}
	else {
		/*-- not an empty list ! */
		p_node->p_next = plist->phead;
		p_node->p_prev = NULL;
		plist->phead->p_prev = p_node;
		plist->phead = p_node;
	}
	plist->num++;

	return SUCCESS;
}


/*-------------------------------------------------------------------------*/
/**
	@brief	To seek and get the node in the list.
	@param	plist		The pointer to the link list.
	@param	offset		The node data to look for.
	@param	seek_opt	SEEK_HEAD/SEEK_TAIL.
	@return	(LL_NODE_t *)	the pointer to the node.
*/
LL_NODE_t *llist_seek(LINK_LIST_t *plist, unsigned seek_opt, unsigned offset)
{
	LL_NODE_t *pnode = NULL;
	unsigned cnt;

	if (TRUE==_llist_empty_test(plist)) {
		/* printf("%s: error: null/empty list ! \n", __FUNCTION__); */
		return NULL;
	}

	if (seek_opt != LL_SEEK_HEAD) {
		printf("\n\n%s: !!!!!-- NOT YET--!!!!!\n\n", __FUNCTION__);
		return NULL;
	}

	pnode = llist_head(plist);
	for (cnt=0; cnt<plist->num; cnt++) {
		if (!pnode || cnt==offset) break;
		pnode = pnode->p_next;
	}
	return pnode;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To get the head node of the list.
	@param	plist			The pointer to the link list.
	@return	(LL_NODE_t *)	the pointer to the node.
*/
LL_NODE_t *llist_head(LINK_LIST_t *plist)
{
	return (plist->phead);
}

/*-------------------------------------------------------------------------*/
/**
	@brief	To get the tail node of the list.
	@param	plist			The pointer to the link list.
	@return	(LL_NODE_t *)	the pointer to the node.
*/
LL_NODE_t *llist_tail(LINK_LIST_t *plist)
{
	return (plist->ptail);
}



/*-------------------------------------------------------------------------*/
/**
	@file    prog_dbg.c
	@author  alex wu
	@date    Nov 2008
	@version 1.0
	@brief   commod debug utility.
*/

/*---------------------------------------------------------------------------
   								Includes
 ---------------------------------------------------------------------------*/
#include "myut_common.h"
#include "prog_dbg.h"

/*---------------------------------------------------------------------------
   								Defines
 ---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
   								Globals
 ---------------------------------------------------------------------------*/
static unsigned dbg_level = DEBUG_ALERT;


/*===========================================================================
   							Function Definitions
 ===========================================================================*/

/*--------------------------------------------------------------------------*/
/**
	@brief	To retrieve the program debug level.
	@param	void
	@return	unsigned
*/
unsigned get_debug_level(void)
{
	return dbg_level;
}

/*--------------------------------------------------------------------------*/
/**
	@brief	To set the program debug level.
	@param	unsigned	new debug level
	@return	void
*/
void set_debug_level(unsigned l)
{
	dbg_level = l;
}

/*--------------------------------------------------------------------------*/
/**
	@brief	To print alert debug message.
	@param	format	format string
	@param	...		va_list
	@return	void
*/
void dbg_alert(const char *format, ...)
{
	va_list	ap;
	char *p_vabuf;

	p_vabuf = (char *)malloc(MAX_DBG_PRINT_VA_LIST_SIZE);
	if (!p_vabuf) {
		printf("%s,%d: error: insufficient memory !!\n", __FUNCTION__, __LINE__);
		return;
	}

	if (dbg_level<DEBUG_ALERT) return;

	va_start(ap, format);
	vsprintf(p_vabuf, format, ap);
	printf(p_vabuf);
	va_end(ap);

	free(p_vabuf);
}

/*--------------------------------------------------------------------------*/
/**
	@brief	To print trace debug message.
	@param	format	format string
	@param	...		va_list
	@return	void
*/
void dbg_trace(const char *format, ...)
{
	va_list	ap;
	char *p_vabuf, sz_prefix[]="--> ";

	p_vabuf = (char *)malloc(strlen(sz_prefix)+MAX_DBG_PRINT_VA_LIST_SIZE);
	if (!p_vabuf) {
		printf("%s,%d: error: insufficient memory !!\n", __FUNCTION__, __LINE__);
		return;
	}

	if (dbg_level<DEBUG_TRACE) return;

	strcpy(p_vabuf, sz_prefix);
	va_start(ap, format);
	vsprintf(p_vabuf+strlen(sz_prefix), format, ap);
	printf(p_vabuf);
	va_end(ap);

	free(p_vabuf);
}

/*--------------------------------------------------------------------------*/
/**
	@brief	To print verbose debug message.
	@param	format	format string
	@param	...		va_list
	@return	void
*/
void dbg_verbose(const char *format, ...)
{
	va_list	ap;
	char *p_vabuf, sz_prefix[]="... ";

	p_vabuf = (char *)malloc(strlen(sz_prefix)+MAX_DBG_PRINT_VA_LIST_SIZE);
	if (!p_vabuf) {
		printf("%s,%d: error: insufficient memory !!\n", __FUNCTION__, __LINE__);
		return;
	}

	if (dbg_level<DEBUG_VERBOSE) return;

	strcpy(p_vabuf, sz_prefix);
	va_start(ap, format);
	vsprintf(strlen(sz_prefix)+p_vabuf, format, ap);
	printf(p_vabuf);
	va_end(ap);

	free(p_vabuf);
}

/*--------------------------------------------------------------------------*/
/**
	@brief	To print the entry message for function.
	@param	format	format string
	@param	...		va_list
	@return	void
*/
void dbg_fentry(const char *format, ...)
{
#if 0
	va_list	ap;
	char *p_vabuf, sz_func[MAX_DBG_PRINT_FUNC_SIZE]="... ";

	p_vabuf = (char *)malloc(strlen(sz_prefix)+MAX_DBG_PRINT_VA_LIST_SIZE);
	if (!p_vabuf) {
		printf("%s,%d: error: insufficient memory !!\n", __FUNCTION__, __LINE__);
		return;
	}

	if (dbg_level<DEBUG_VERBOSE) return;

	strcpy(p_vabuf, sz_prefix);
	va_start(ap, format);
	vsprintf(strlen(sz_prefix)+p_vabuf, format, ap);
	printf(p_vabuf);
	va_end(ap);

	free(p_vabuf);
#endif
}



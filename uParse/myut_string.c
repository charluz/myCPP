/*-------------------------------------------------------------------------*/
/**
	@file    myut_string.c
	@author  alex wu
	@date    Nov 2008
	@version 1.0
	@brief   My utility function - ascii strings manipulation.
*/

/*---------------------------------------------------------------------------
   								Includes
 ---------------------------------------------------------------------------*/
#include "myut_common.h"
#include "myut_string.h"

/*-- utility module printf */
#define MYUT_DEBUG			0
#if MYUT_DEBUG
#define utprintf(fmt, arg...)		printf(fmt, ##arg)
#else
#define utprintf(fmt, arg...)		do {} while(0)
#endif /* MYUT_DEBUG */

/*---------------------------------------------------------------------------
   								Constants
 ---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
   								Globals
 ---------------------------------------------------------------------------*/

/*===========================================================================
   							Function Definitions
 ===========================================================================*/

/*-------------------------------------------------------------------------*/
/**
	@brief	check if the input ascii line exceeds the max length.
	@param	psz_line		The ascii line.
	@param	limit			The max length allowed.
	@return	unsigned		FAIL/SUCCESS.

	This function examine the last character of the given ascii line.
	It return FAIL while the last ch is not '\n'.
*/
unsigned test_line_length(char *psz_line, unsigned limit)
{
	int len;
	len = (int)strlen(psz_line)-1; /* to position the last char */
	if (psz_line[len] != '\n') {
		utprintf("line size exceeds (%d), too long! \n", limit);
		return FAIL;
	}
	return SUCCESS;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	Convert a string to lowercase.
	@param	s			String to convert.
	@return	(char*)		Ptr to statically allocated string.

	This function returns a pointer to a statically allocated string
	containing a lowercased version of the input string. Do not free
	or modify the returned string! Since the returned string is statically
	allocated, it will be modified at each function call (not re-entrant).
 */
char* strlwc(const char *s)
{
    static char l[_UT_MAX_LINE_SZ_LEN+1];
    int i ;

    if (s==NULL) return NULL ;
    memset(l, 0, _UT_MAX_LINE_SZ_LEN+1);
    i=0 ;
    while (s[i] && i<_UT_MAX_LINE_SZ_LEN) {
        l[i] = (char)tolower((int)s[i]);
        i++ ;
    }
    l[_UT_MAX_LINE_SZ_LEN]=(char)0;
    return l ;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	Remove blanks at the beginning and the end of a string.
	@param	s			String to parse.
	@return	(char*)		Ptr to statically allocated string.

	This function returns a pointer to a statically allocated string,
	which is identical to the input string, except that all blank
	characters at the end and the beg. of the string have been removed.
	Do not free or modify the returned string! Since the returned string
	is statically allocated, it will be modified at each function call
	(not re-entrant).
 */
char* strstrip(char *s)
{
	static char l[_UT_MAX_LINE_SZ_LEN+1];
	char *last;

	if (s==NULL) return NULL ;

	/* remove blank at the beg. of sz*/
	while (isspace((int)*s) && *s) s++;

	/* copy to static allocated buffer */
	memset(l, 0, _UT_MAX_LINE_SZ_LEN+1);
	strcpy(l, s);

	/* remove blank at the end of sz */
	last = l + strlen(l);
	while (last > l) {
		if (!isspace((int)*(last-1)))
			break ;
		last -- ;
	}
	*last = (char)0;
	return (char*)l ;
}

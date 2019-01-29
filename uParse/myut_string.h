/*-------------------------------------------------------------------------*/
/**
	@file    myut_string.h
	@author  alex wu
	@date    NOV 2008
	@version 1.0
	@brief   The utility function for string manipulation.

	This module implements a simple collection of common functions to
	manipulate ascii strings.
*/
/*--------------------------------------------------------------------------*/

#ifndef _MYUT_STRING_H_
#define _MYUT_STRING_H_

/*---------------------------------------------------------------------------
   								Includes
 ---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/*---------------------------------------------------------------------------
   								Constants
 ---------------------------------------------------------------------------*/
#define _UT_MAX_LINE_SZ_LEN         	(1024)
	/*-- define the max length a line is to be */

/*---------------------------------------------------------------------------
   								Types
 ---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
   								Macros
 ---------------------------------------------------------------------------*/
#define STR_EQUAL(sza, szb)		(!(strcmp(sza, szb)))
#define STR_QUOTE_EMPTY(sz)		(STR_EQUAL(sz, "\"\"") || STR_EQUAL(sz, "''"))

/*---------------------------------------------------------------------------
  							Function prototypes
 ---------------------------------------------------------------------------*/
unsigned test_line_length(char *psz_line, unsigned limit);
char* strstrip(char *s);
char* strlwc(const char *s);

#endif /* _MYUT_STRING_H_ */

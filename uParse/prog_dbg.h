
/*-------------------------------------------------------------------------*/
/**
   @file    prog_dbg.h
   @author  alex wu
   @date    Sep 2007
   @version $Revision: 1.12 $
   @brief   Implements a dictionary for string variables.

   This module implements a simple dictionary object, i.e. a list
   of string/string associations. This object is useful to store e.g.
   informations retrieved from a configuration file (ini files).
*/
/*--------------------------------------------------------------------------*/

#ifndef _PROG_DBG_H_
#define _PROG_DBG_H_

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
#define MAX_DBG_PRINT_VA_LIST_SIZE		(256)
#define MAX_DBG_PRINT_FUNC_SIZE			(64)

/*---------------------------------------------------------------------------
   								Types
 ---------------------------------------------------------------------------*/

typedef enum {
	/*-- unsigned */
	DEBUG_SILENT	= 0,
	DEBUG_ALERT,
	DEBUG_VERBOSE,
	DEBUG_TRACE	,
	DEBUG_TOTAL,
} DEBUG_LEVLE_t;



/*---------------------------------------------------------------------------
  							Function prototypes
 ---------------------------------------------------------------------------*/
void set_debug_level(unsigned l);
unsigned get_debug_level(void);
void dbg_alert(const char *format, ...);
void dbg_trace(const char *format, ...);
void dbg_verbose(const char *format, ...);

#endif /* _PROG_DBG_H_ */

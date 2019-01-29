/*-------------------------------------------------------------------------*/
/**
	@file    prog_arg.h
	@author  alex wu
	@date    Nov 2008
	@version 1.0
	@brief   Parser for JPG embedded 3a/iq info.
*/

#ifndef _PROG_ARG_H_
#define _PROG_ARG_H_

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
#define PROG_ARG_ELMT_CTRL_SIZE				(16)
#define PROG_ARG_ELMT_DATA_SIZE				(128)

#define PROG_TGT_SZ_SIZE					(64)

/*---------------------------------------------------------------------------
   								Types
 ---------------------------------------------------------------------------*/

typedef struct {
	char sz_ctrl[PROG_ARG_ELMT_CTRL_SIZE];
	char sz_data[PROG_ARG_ELMT_DATA_SIZE];
} PROG_OPTION_t;


/*---------------------------------------------------------------------------
  							Function prototypes
 ---------------------------------------------------------------------------*/
PROG_OPTION_t* prog_option_query(char *psz_ctrl);
char* prog_target_get(unsigned n);
unsigned prog_argument_create(int argc, char *argv[]);
void prog_argument_release(void);
void print_prog_usage(unsigned char *psz_prog_name);

#endif /* _PROG_ARG_H_ */

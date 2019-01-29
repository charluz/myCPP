#ifndef _MYUT_COMMON_H_
#define _MYUT_COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FAIL							0
#define SUCCESS							1
#define TRUE							1
#define FALSE							0

#define NULL							((void *)0)

#define MAX_FNAME_BASE_LEN				(64)
#define MAX_FNAME_EXT_LEN				(16)
#define MAX_FNAME_LEN					(MAX_FNAME_BASE_LEN+MAX_FNAME_EXT_LEN+2)

/*-- for OS32 */
typedef unsigned int					UINT32;
typedef int								SINT32;
typedef unsigned char					UINT8;
typedef char							SINT8;
typedef unsigned short int				UINT16;
typedef short int						SINT16;

#endif /* _MYUT_COMMON_H_ */

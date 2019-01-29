/*-------------------------------------------------------------------------*/
/**
	@file    myut_numeric.c
	@author  alex wu
	@date    Nov 2008
	@version 1.0
	@brief   numeric utility fucntions for my project.
*/

/*---------------------------------------------------------------------------
   								Includes
 ---------------------------------------------------------------------------*/
#include "myut_numeric.h"

/*-- utility module printf */
#define NUMERIC_DEBUG			0
#if NUMERIC_DEBUG
#define xprintx(fmt, arg...)		printf(fmt, ##arg)
#else
#define xprintx(fmt, arg...)		do {} while(0)
#endif /* NUMERIC_DEBUG */


/*===========================================================================
   							Function Definitions
 ===========================================================================*/

/*-------------------------------------------------------------------------*/
/**
	@brief	Convert input vale into big endian (32)
	@param	in			The input value.
	@return	(UINT32)	Converted value.
*/
UINT32 to_big_endian32(UINT32 in)
{
	UINT32 out;
	/*
	printf("%s: in(0x%08x) \n", __FUNCTION__, in);
	*/
	out = ((in & 0xFF000000)>>24)
		+ ((in & 0x00FF0000)>>8)
		+ ((in & 0x0000FF00)<<8)
		+ ((in & 0x000000FF)<<24);
	return out;
}

/*-------------------------------------------------------------------------*/
/**
	@brief	Convert input vale into big endian (16)
	@param	in			The input value.
	@return	(UINT16)	Converted value.
*/
UINT16 to_big_endian16(UINT16 in)
{
	UINT16 out;
	/*
	printf("%s: in(0x%04x) \n", __FUNCTION__, in);
	*/

	out = ((in & 0xFF00)>>8) + ((in & 0x00FF)<<8);
	return out;
}

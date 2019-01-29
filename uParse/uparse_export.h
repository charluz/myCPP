#ifndef _UPARSE_EXPORT_H_
#define _UPARSE_EXPORT_H_

/*---------------------------------------------------------------------------
   								Includes
 ---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
   								Constants
 ---------------------------------------------------------------------------*/
#define UPARSE_JPG_SIGNATURE		("mM33++Mm")		/* 簽名，8-bytes＋結尾'\0'，實際在檔案裡是9 bytes */
#define UPARSE_VER_SZ				("V0.02")			/* 長度6 bytes(含結尾'\0') 的ASCII strings */


/*---------------------------------------------------------------------------
   								Types
 ---------------------------------------------------------------------------*/

/**
	@brief		Type template of Parameter Data.
*/
typedef enum {
	PARAM_UINT16,
	PARAM_SINT16,
	PARAM_UINT32,
	PARAM_SINT32,
	PARAM_ARRAY,
	PARAM_UNDEF,
} PARAM_DTYPE_t;

#endif /* _UPARSE_EXPORT_H_ */

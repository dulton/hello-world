//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************/

//
// Define common Windows types in terms of WMA types.  Include wmatypes.h first.
//

#ifndef __WMAWINTYPES_H__
#define __WMAWINTYPES_H__
#include "wmapro_wma_def.h"
#include "wmapro_wmatypes.h"
#include "wwmapro_mawintypes.h"
typedef U32 DWORD;
typedef U16 WORD;
typedef I32 LONG;
typedef U32 ULONG;
#if (MSDRM_WMA==0)
typedef Bool BOOL;
#endif
typedef U16 WCHAR;
typedef U8 BYTE;
typedef char CHAR;
typedef WMARESULT HRESULT;
#if (MSDRM_WMA==1)
#ifndef FALSE
#define FALSE False
#endif
#ifndef TRUE
#define TRUE True
#endif
#else
#define FALSE False
#define TRUE True
#endif
#define S_OK WMA_OK
#define FAILED WMA_FAILED
#define SUCCEEDED WMA_SUCCEEDED

#define FAR

#endif//__WMAWINTYPES_H__
//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/* Wanted to name this file wmatypes.h, but that name is already taken by asfparse */
#ifndef __WMATYPS_H__
#define __WMATYPS_H__
#include "wmapro_wma_def.h"
#if (MSDRM_WMA==1)
#include "wmapro_wmatypes.h"
#else
typedef int WMARESULT;
#endif
// define basic types
typedef unsigned long WMA_U32;
typedef long WMA_I32;
typedef unsigned short WMA_U16;
typedef short WMA_I16;
typedef unsigned char WMA_U8;
typedef __int64 WMA_I64;
typedef WMA_I32 WMA_Bool;


#endif//__WMATYPS_H__

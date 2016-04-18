//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    wmatypes.h

Abstract:

    Processor-specific data types.

Author:

    Kazuhito Koishida (kazukoi)       Jan 31, 2003

Revision History:

*************************************************************************/

#ifndef __WMATYPES_H
#define __WMATYPES_H
#include "wma_def.h"

#define WMA_TARGET_ANSI
#include "wmatypes_ansi.h"


// **************************************************************************
// Overridable Compiler Directives
// **************************************************************************
#define INLINE __inline


// ***************************************************
//            default fundamental typedefs
// ***************************************************
#ifndef PLATFORM_SPECIFIC_U64
typedef unsigned __int64 U64;
#endif
#ifndef PLATFORM_SPECIFIC_I64
typedef __int64 I64;
#endif

#ifndef PLATFORM_SPECIFIC_U32
typedef unsigned long int U32;
#define U32_MAX ((U32)0xffffffffu)
#endif

typedef long int I32;
#define I32_MIN ((I32)0x80000000)
#define I32_MAX ((I32)0x7fffffff)

#ifndef PLATFORM_SPECIFIC_U16
typedef unsigned short U16;
#define U16_MAX ((I32)0x0000ffff)
#endif

typedef short I16;
#define I16_MIN ((I32)0xffff8000)
#define I16_MAX ((I32)0x00007fff)


#ifndef PLATFORM_SPECIFIC_U8
typedef unsigned char U8;
#define U8_MAX ((I32)0x000000ff)
#endif

typedef signed char I8;
#define I8_MIN ((I32)0xffffff80)
#define I8_MAX ((I32)0x0000007f)


#ifndef PLATFORM_SPECIFIC_F32
typedef float F32;
#endif
#ifndef PLATFORM_SPECIFIC_F64
typedef double F64;
#endif

// the following "UP" types are for using n-bit or native type, in case
// using native types might be faster.
#ifndef PLATFORM_SPECIFIC_I8UP
typedef int I8UP;
#endif

#ifndef PLATFORM_SPECIFIC_U8UP
typedef unsigned U8UP;
#endif

#ifndef PLATFORM_SPECIFIC_I16UP
typedef int I16UP;
#endif

#ifndef PLATFORM_SPECIFIC_U16UP
typedef unsigned U16UP;
#endif

#ifndef PLATFORM_SPECIFIC_I32UP
typedef int I32UP;
#endif

#ifndef PLATFORM_SPECIFIC_U32UP
typedef unsigned U32UP;
#endif

#ifndef PLATFORM_SPECIFIC_I64UP
typedef __int64 I64UP;
#endif

#ifndef PLATFORM_SPECIFIC_U64UP
typedef unsigned __int64 U64UP;
#endif

#ifndef PLATFORM_SPECIFIC_F32UP
typedef float F32UP;
#endif
#ifndef PLATFORM_SPECIFIC_F64UP
typedef double F64UP;
#endif

#ifndef	PLATFORM_SPECIFIC_CBDATA
typedef void* CBDATA; // callback data parameter
#endif // PLATFORM_SPECIFIC_CBDATA

typedef double  Double;
typedef float   Float;
typedef void    Void;
typedef U32     UInt;
typedef I32     Int;
typedef I32     Bool; // In Win32, BOOL is an "int" == 4 bytes. Keep it this way to reduce problems.
typedef char    Char;

#ifndef True
#define True 1
#endif

#ifndef False
#define False 0
#endif

// define basic types
typedef __int64         WMA_I64;
typedef unsigned long   WMA_U32;
typedef long            WMA_I32;
typedef unsigned short  WMA_U16;
typedef short           WMA_I16;
typedef unsigned char   WMA_U8;
typedef WMA_I32         WMA_Bool;

typedef int WMARESULT;

#endif  // __WMATYPES_H

//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    wmatypes_ansi.h

Abstract:

    ANSI-C data types.

Author:

    Kazuhito Koishida (kazukoi)       Jan 31, 2003

Revision History:


*************************************************************************/

#ifndef __WMATYPES_ANSI_H
#define __WMATYPES_ANSI_H
#include "wma_def.h"
// Do the fundamental typedefs: {U|I}{8|16|32|64}

#define PLATFORM_SPECIFIC_U64
typedef unsigned __int64 U64;
#define PLATFORM_SPECIFIC_I64
typedef __int64 I64;

#endif  //_WMATYPES_ANSI_H

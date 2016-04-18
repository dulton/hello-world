//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************

#ifndef __WMAUDIO_INC_H_
#define __WMAUDIO_INC_H_
#include "wmapro_wma_def.h"
#define STDCALL __stdcall // For drmpd_ext.h

//#include <stdio.h>
//#include "WMA_MALLOC.h"//hans

//#pragma warning( disable : 4005 )
#if ( defined(BOOL) || defined(_WIN32_WCE) ) && !defined(_BOOL_DEFINED)
#define _BOOL_DEFINED
#endif
#include "wmapro_wmatypes.h"
#include "wmapro_wmaudio.h"
#include "wmapro_loadstuff.h"
#include "wmapro_wmaudio_type.h"
#include "wmapro_wmaguids.h"
#include "wmapro_wavfileexio.h"
#include "wmapro_AutoProfile.h"

#ifdef USE_SPDTX
//#include "spdtx.h"
//
//// !! having this typedef here is not very wise,
//// but it releases asfparse_s from its dependency
//// upon msaudiodec.h in the SPDIF transmitter case.
//// this is preferred because msaudiodec.h drags
//// in many decoder headers that are otherwise
//// irrelevant to the transmitter.
//
//typedef struct WmaGetDataParam
//{
//    U32  m_dwUser;
//    Bool m_fTimeIsValid;
//    I64  m_iTime;
//} WmaGetDataParam;

#else

#include "msaudiodec.h"
#include "wmadec.h"
#endif

#ifndef MIN
#define MIN(x, y)  ((x) < (y) ? (x) : (y))
#endif /* MIN */

#define WMAAPI_DEMO_LIMIT 60
#define CALLBACK_BUFFERSIZE WMA_MAX_DATA_REQUESTED*2

//static const WMARESULT WMA_E_ONHOLD         = 0x80040004; 

#define WMAFileGetInputParam WmaGetDataParam

WMARESULT WMAFileGetInput (tWMAFileStateInternal *pInt, U8 **ppBuffer, U32 *pcbBuffer, WMAFileGetInputParam*);
tWMAFileStatus WMAF_UpdateNewPayload (tWMAFileStateInternal *pInt);

#endif // wmaudio_inc.h
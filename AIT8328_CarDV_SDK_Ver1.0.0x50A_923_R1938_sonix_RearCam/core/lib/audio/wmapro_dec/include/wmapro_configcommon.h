//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************

#ifndef __CONFIGCOMMON_H__
#define __CONFIGCOMMON_H__
#include "wmapro_wma_def.h"
//#ifdef __cplusplus
//extern "C" {
//#endif  // __cplusplus

U32 MaxSamplesPerPacket(U32 ulVersion,
                        U32 ulSamplingRate,
                        U16 ulChannels,
                        U32 ulBitrate);

I32 msaudioGetSamplePerFrame (I32   cSamplePerSec, 
                              U32   dwBitPerSec, 
                              I32   cChannel,
                              I32   iVersion,
                              U16   wEncOpt);

//#ifdef __cplusplus
//}
//#endif  // __cplusplus

#endif//__CONFIGCOMMON_H__

//==============================================================================
//
//  File        : StateNetPlaybackFunc.h
//  Description : INCLUDE File for the StateNetPlaybackFunc function porting.
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================

#ifndef _STATENETPLAYBACKUNC_H_
#define _STATENETPLAYBACKUNC_H_

/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "AHC_General.h"

void StateNetPlaybackMode(UINT32 ulEvent);
AHC_BOOL NetPlaybackFunc_Init(void* pData);
AHC_BOOL NetPlaybackFunc_ShutDown(void* pData);

#endif
//==============================================================================
//
//  File        : StateMSDCFunc.h
//  Description : INCLUDE File for the StateMSDCFunc function porting.
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================

#ifndef _STATEMSDCFUNC_H_
#define _STATEMSDCFUNC_H_

/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "MenuCommon.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/ 

#define MSDC_POWEROFF_COUNT 2

/*===========================================================================
 * Function prototype
 *===========================================================================*/ 

void 		StateMSDCMode( UINT32 ulEvent );
AHC_BOOL 	MSDCMode_Start(void);
void 		MSDCTimer_ISR(void *tmr, void *arg);
AHC_BOOL 	MSDCTimer_Start(UINT32 ulTime);
void 		MSDCTimer_ResetCounter(void);

#endif  // _STATEMSDCFUNC_H_
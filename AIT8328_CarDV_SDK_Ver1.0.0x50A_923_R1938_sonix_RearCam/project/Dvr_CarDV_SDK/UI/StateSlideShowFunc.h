//==============================================================================
//
//  File        : StateSlideShowFunc.h
//  Description : INCLUDE File for the StateCameraFunc function porting.
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================

#ifndef _STATESLIDESHOWFUNC_H_
#define _STATESLIDESHOWFUNC_H_

/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "MenuCommon.h"

/*===========================================================================
 * Function prototype
 *===========================================================================*/ 

void 		StateSlideShowMode(UINT32 ulEvent);
void 		SlideShowFunc_SetOSDShowStatus(AHC_BOOL state);
AHC_BOOL 	SlideShowFunc_GetOSDShowStatus(void);
AHC_BOOL 	SlideShowFunc_Start(void);
AHC_BOOL 	SlideShowFunc_Play(void);
AHC_BOOL 	SlideShowFunc_Stop(void);
AHC_BOOL 	SlideShowTimer_Start(UINT32 ulTime);
AHC_BOOL 	SlideShowTimer_Stop(void);

void 		SlideShowMode_Update(void);
void 		SlideShowMode_SDMMC_In(void);
void 		SlideShowMode_SDMMC_Out(void);

#endif //_STATESLIDESHOWFUNC_H_
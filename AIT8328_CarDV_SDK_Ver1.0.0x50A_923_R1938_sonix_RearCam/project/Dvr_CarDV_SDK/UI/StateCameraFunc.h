//==============================================================================
//
//  File        : StateCameraFunc.h
//  Description : INCLUDE File for the StateCameraFunc function porting.
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================

#ifndef _STATECAMERAFUNC_H_
#define _STATECAMERAFUNC_H_

/*===========================================================================
 * Include file 
 *===========================================================================*/

#include "MenuCommon.h"

/*===========================================================================
 * Function prototype
 *===========================================================================*/

void 		StateCaptureMode( UINT32 ulEvent );
AHC_BOOL 	CaptureMode_PreviewUpdate(void);
AHC_BOOL 	CaptureMode_Start(void);
UINT32 		CaptureFunc_GetRemainCaptureNum(void);
UINT16 		CaptureFunc_GetSelfTime(void);
AHC_BOOL 	CaptureFunc_SetAWBMode(void);
AHC_BOOL 	CaptureFunc_SetScence(void);
AHC_BOOL 	CaptureFunc_SetFocusWindow(void);
AHC_BOOL 	CaptureFunc_SetFocusRange(void);
AHC_BOOL 	CaptureFunc_SetExpoMetering(void);
AHC_BOOL 	CaptureFunc_SetExpoAdjust(void);
AHC_BOOL 	CaptureFunc_SetISO(void);
AHC_BOOL 	CaptureFunc_SetColor(void);
AHC_BOOL 	CaptureFunc_SetEffect(void);
UINT8 		CaptureFunc_GetControlPageStatus(void);
AHC_BOOL 	CaptureFunc_SetResolution(IMAGESIZE_SETTING ubResolution, AHC_BOOL bSetOnly);
AHC_BOOL 	CaptureFunc_CheckMemSizeAvailable(UINT64 *pFreeBytes, UINT32 *pRemainCaptureNum);
AHC_BOOL 	CaptureFunc_SetFaceDetection(UINT8 bMode);
void
CaptureFunc_ConfigStrobe(	AHC_BOOL	bCalculated,
							UINT32		ulCalculatedNumber,
							
							AHC_BOOL	bPreFlash,
							UINT32		ulPreFlashNumber,
							UINT32		ulWaitPreFlashTime,
							UINT32		ulPreFlashTime,
							UINT32		ulTimeBetweenTwoFlash,
							
							AHC_BOOL	bMainFlash,
							UINT32		ulWaitMainFlashTime,
							UINT32		ulMainFlashTime);
AHC_ERR 	CaptureFunc_EnableStrobe(AHC_BOOL bEnable);

AHC_BOOL 	CaptureTimer_Start(UINT32 ulTime);
AHC_BOOL 	CaptureTimer_Stop(void);
AHC_BOOL 	CaptureFunc_Shutter(void);
UINT16 		CaptureFunc_CheckSelfTimer(void);
AHC_BOOL	CaptureFunc_Preview(void);

#endif //_STATECAMERAFUNC_H_

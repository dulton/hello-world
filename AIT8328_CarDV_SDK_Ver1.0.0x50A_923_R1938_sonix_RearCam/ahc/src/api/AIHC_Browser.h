/**
 @file AIHC_Browser.h
 @brief Header File for the AHC browser API.
 @author 
 @version 1.0
*/

#ifndef _AIHC_BROWSER_H_
#define _AIHC_BROWSER_H_

/*===========================================================================
 * Include files
 *===========================================================================*/

#include "AHC_Common.h"
#include "AHC_Browser.h"
#include "AHC_Config_SDK.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/

#define AHC_BROWSER_THUMBNAIL_FLIP_BUFFER 	0x4E00000

/*===========================================================================
 * Structure define
 *===========================================================================*/

typedef struct _AHC_THUMB_ATTRIBUTE {
	/*
	the arrangement of thumbnail.
	*/
	UINT16 usThumbPerRow;
	UINT16 usThumbPerCol;
	
	UINT16 usThumbHorizontalGap;
	UINT16 usThumbVerticalGap;
    
    UINT16 uwTargetHorizontal;
    UINT16 uwTargetVertical;
    
	UINT16 uwCurIdx;// the working index.
	
	UINT32 ulBkColor;
	UINT32 ulFocusColor;	
	UINT32 StartX;
	UINT32 StartY;
	UINT32 DisplayWidth;
	UINT32 DisplayHeight;
	UINT32 uiIdxMap[MAX_DCF_IN_THUMB];
} AHC_THUMB_ATTRIBUTE;

/*===========================================================================
 * Function prototype 
 *===========================================================================*/

AHC_BOOL AIHC_Thumb_GetCurrentIdx(UINT32 *pwIndex);
AHC_BOOL AIHC_Thumb_SetCurrentIdx(UINT32 pwIndex);

#endif
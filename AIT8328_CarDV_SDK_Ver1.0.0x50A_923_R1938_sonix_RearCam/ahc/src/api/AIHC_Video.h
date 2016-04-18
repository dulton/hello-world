//==============================================================================
//
//  File        : AIHC_Video.h
//  Description : INCLUDE File for the AIHC video function porting.
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================

#ifndef _AIHC_VIDEO_H_
#define _AIHC_VIDEO_H_

/*===========================================================================
 * Include files
 *===========================================================================*/

#include "AHC_Common.h"
#include "AHC_Video.h"

/*===========================================================================
 * Function prototype
 *===========================================================================*/
 
AHC_BOOL AIHC_GetMovieConfig(UINT32 wCommand, UINT32 *wOp);
AHC_BOOL AIHC_GetMovieConfigEx(UINT16 i, UINT32 wCommand, UINT32 *wOp);
AHC_BOOL AIHC_MapToRawVRes(AHC_VIDRECD_RESOL res, UINT16 *w, UINT16 *h);
AHC_BOOL AIHC_MapFromRawVRes(UINT16 w, UINT16 h, UINT16 fps, AHC_VIDRECD_RESOL *AhcRes);
void AIHC_AdjustJpegFromSensor(UINT16 w, UINT16 h, UINT16* suggestedWidth, UINT16* suggestedHeight);
INT32 AIHC_CloseTranscode(void);

#endif
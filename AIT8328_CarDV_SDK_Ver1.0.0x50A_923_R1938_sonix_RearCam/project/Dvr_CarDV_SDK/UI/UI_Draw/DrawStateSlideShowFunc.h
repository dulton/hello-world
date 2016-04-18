
//==============================================================================
//
//  File        : DrawStateSlideShowFunc.h
//  Description : INCLUDE File for the DrawStateSlideShowFunc function.
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================
#ifndef _DRAWSTATESLIDESHOWFUNC_H_
#define _DRAWSTATESLIDESHOWFUNC_H_

/*===========================================================================
 * Function prototype
 *===========================================================================*/ 

void DrawStateSlideShowUpdate( UINT32 ubEvent);

#if defined(CAR_DV)
void UpdateSlideMovPBStatus(UINT8 ubStatus);
void UpdateSlideAudPBStatus(UINT8 ubStatus);
void DrawStateSlideShowMode_Update(void);
void DrawStateSlideShowPageSwitch(UINT8 ubPage);
void DrawSlideShow_MovPbState(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawSlideShow_FileIndex(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawSlideShow_FileInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawSlideShow_MovPlayInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawSlideShow_MovTotalTime(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawSlideShow_LockCurFile(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
#endif

#endif   // _DRAWSTATESLIDESHOWFUNC_H_ 



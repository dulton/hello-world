
//==============================================================================
//
//  File        : DrawStatePhotoPBFunc.h
//  Description : INCLUDE File for the DrawStatePhotoPBFunc function.
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================
#ifndef _DRAWSTATEPHOTOPBFUNC_H_
#define _DRAWSTATEPHOTOPBFUNC_H_

/*===========================================================================
 * Function prototype
 *===========================================================================*/ 

void DrawStateJpgPlaybackUpdate( UINT32 ubEvent);

#if defined(CAR_DV)
void DrawStateJpgPbMode_Update(void);
void DrawStateJpgPbPageSwitch(UINT8 ubPage);
void DrawStateJpgPbInit(void);
void DrawJpgPb_FileIndex(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawJpgPb_FileInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawJpgPb_DateInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawJpgPb_LockCurFile(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor,INT32 Para1);
#endif

#endif   // _DRAWSTATEPHOTOPBFUNC_H_ 


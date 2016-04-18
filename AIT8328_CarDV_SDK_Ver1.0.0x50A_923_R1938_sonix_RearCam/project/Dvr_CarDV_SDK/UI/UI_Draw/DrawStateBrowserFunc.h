
//==============================================================================
//
//  File        : DrawStateBrowserFunc.h
//  Description : INCLUDE File for the DrawStateVideoFunc function.
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================
#ifndef _DRAWSTATEBROWSERFUNC_H_
#define _DRAWSTATEBROWSERFUNC_H_

/*===========================================================================
 * Function prototype
 *===========================================================================*/ 

void DrawStateBrowserUpdate( UINT32 ubEvent);
void DrawBrowser_GetPageInfo(UINT16 *CurrPage, UINT16 *TotalPage);
void DrawBrowserStatePageUpdate(void);
void DrawBrowserStateSDMMC_In(void);
void DrawBrowserStateSDMMC_Out(void);

#if defined(CAR_DV)
void DrawBrowser_NameInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawBrowser_DateInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawBrowser_TotalTime(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawBrowserStatePlaybackRearCam( AHC_BOOL bEnable );
void DrawBrowser_ResolutionInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
#endif

#endif   // _DRAWSTATEBROWSERFUNC_H_ 



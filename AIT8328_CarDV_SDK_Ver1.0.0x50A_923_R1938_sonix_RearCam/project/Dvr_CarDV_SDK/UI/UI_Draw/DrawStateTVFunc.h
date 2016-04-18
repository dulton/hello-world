
//==============================================================================
//
//  File        : DrawStateTVFunc.h
//  Description : INCLUDE File for the DrawStateTVFunc function.
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================
#ifndef _DRAWSTATETVFUNC_H_
#define _DRAWSTATETVFUNC_H_

/*===========================================================================
 * Function prototype
 *===========================================================================*/ 
 
void DrawStateTVBrowserUpdate(UINT32 ubEvent);
void DrawStateTVMoviePBUpdate(UINT32 ulEvent);
void DrawStateTVPhotoPBUpdate(UINT32 ulEvent);
void DrawStateTVSlideShowUpdate(UINT32 ulEvent);
void DrawStateTVBrowserInit(void);
void DrawStateTVPhotoPBChangeFile(void);
void DrawStateTVSlideShowChangeFile(void);

void DrawStateTVVideoUpdate(UINT32 ulEvent);
void DrawStateTVCameraUpdate(UINT32 ulEvent);

#if defined(CAR_DV)
void DrawStateTVPreviewVideoInit(UINT8 ubID);
void DrawStateTVPreviewCameraInit(UINT8 ubID);
void DrawStateTVMoviePBInit(UINT8 bID0, UINT8 bID1);
void DrawStateTVPhotoPBInit(UINT8 bID0, UINT8 bID1);
void DrawStateTVSlideShowInit(void);

void UpdateTVMovPBStatus(UINT8 ubStatus);
void DrawStateTVBrowserPageUpdate(void);
void DrawStateTVSleepUpdate(UINT32 ulEvent);
void DrawStateTVNetPlaybackInit(void);

void DrawTVMoviePB_Status(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawTVBrowser_TotalTime(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawTVBrowser_NameInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawTVBrowser_DateInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);

void DrawTVMoviePB_FileIndex(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawTVMoviePB_NameInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawTVMoviePB_PlayInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawTVMoviePB_TotalTime(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);

void DrawTVPhotoPB_FileIndex(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawTVPhotoPB_NameInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawTVPhotoPB_DateInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
#endif

#endif   // _DRAWSTATETVFUNC_H_ 



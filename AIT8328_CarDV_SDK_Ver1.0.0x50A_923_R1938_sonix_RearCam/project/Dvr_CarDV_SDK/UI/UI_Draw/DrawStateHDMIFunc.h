
//==============================================================================
//
//  File        : DrawStateHDMIFunc.h
//  Description : INCLUDE File for the DrawStateHDMIFunc function.
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================
#ifndef _DRAWSTATEHDMIFUNC_H_
#define _DRAWSTATEHDMIFUNC_H_

/*===========================================================================
 * Function prototype
 *===========================================================================*/ 

void DrawClearTrashCan(UINT8 ubID);
void DrawStateHdmiBrowserUpdate(UINT32 ubEvent);
void DrawStateHdmiMoviePBUpdate(UINT32 ulEvent);
void DrawStateHdmiPhotoPBUpdate(UINT32 ulEvent);
void DrawStateHdmiSlideShowUpdate(UINT32 ulEvent);
void DrawHdmiMoviePB_VolumeBar(UINT8 ubID, AHC_BOOL bForce);
void DrawStateHdmiMoviePBDel(UINT8 state);
void DrawStateHdmiBrowserInit(void);
void DrawStateHdmiPhotoPBChangeFile(void);
void DrawStateHdmiSlideShowChangeFile(void);
void DrawStateHdmiSlideShowInit(void);

#if defined(CAR_DV)
void DrawStateHdmiPreviewVideoInit(UINT8 ubID);
void DrawStateHdmiPreviewCameraInit(UINT8 ubID);
void DrawStateHdmiMoviePBInit(UINT8 bID0, UINT8 bID1);
void DrawStateHdmiPhotoPBInit(UINT8 bID0, UINT8 bID1);
void DrawStateHdmiVideoUpdate(UINT32 ulEvent);
void DrawStateHdmiCameraUpdate(UINT32 ulEvent);
void DrawStateHdmiBrowserPageUpdate(void);
void DrawStateHDMINetPlaybackInit(void);
void UpdateHdmiMovPBStatus(UINT8 ubStatus);
void DrawStateHdmiMoviePBStatus(void);
void DrawHdmiBrowser_SwitchMode(UINT8 ulID, OP_MODE_SETTING OpMode);
void DrawHdmiBrowser_MediaType(UINT8 ubID, AHC_BOOL OnOff);
void DrawHdmiBrowser_Battery(UINT8 ubID, AHC_BOOL OnOff);
void DrawHdmiBrowser_NameDateInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawHdmiBrowser_TotalTime(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawHdmiMoviePB_FileIndex(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawHdmiMoviePB_NameInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawHdmiMoviePB_PlayInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawHdmiMoviePB_TotalTime(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawHdmiMoviePB_Status(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawHdmiPhotoPB_FileIndex(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawHdmiPhotoPB_NameInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
void DrawHdmiPhotoPB_DateInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1);
#endif

#endif   // _DRAWSTATEHDMIFUNC_H_ 



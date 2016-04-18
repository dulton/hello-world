//==============================================================================
//
//  File        : StateHDMIFunc.h
//  Description : INCLUDE File for the StateHDMIFunc function porting.
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================

#ifndef _STATEHDMIFUNC_H_
#define _STATEHDMIFUNC_H_

/*===========================================================================
 * Include file 
 *===========================================================================*/

#include "MenuCommon.h"
#include "AHC_HDMI.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/ 

#define	HDMI_UI_OSD_ID		(16)
#define	HDMI_UI_OSD2_ID		(17)
#define	HDMI_MENU_OSD_ID	(18)
#define	HDMI_MENU_OSD2_ID	(19)

/*===========================================================================
 * Enum define
 *===========================================================================*/
/*
typedef enum _AHC_HDMI_STATUS
{
    AHC_HDMI_NONE_STATUS = 0, 
    AHC_HDMI_BROWSER_STATUS,    
    AHC_HDMI_MOVIE_PB_STATUS,
    AHC_HDMI_PHOTO_PB_STATUS,
    AHC_HDMI_SLIDE_SHOW_STATUS,
    AHC_HDMI_VIDEO_PREVIEW_STATUS,
    AHC_HDMI_DSC_PREVIEW_STATUS         
} AHC_HDMI_STATUS;
*/

/*===========================================================================
 * Function prototype
 *===========================================================================*/

AHC_BOOL 		HDMIFunc_IsConnect(void);
AHC_BOOL 		HDMIFunc_IsInMenu(void);
void 			SetHDMIState(AHC_HDMI_STATUS HdmiState); 
void 			StateHDMIMode( UINT32 ulEvent );
void 			StateHDMI_EventMode_Handler(UINT32 ulJobEvent);
AHC_BOOL 		HDMIMode_Start(void);
AHC_HDMI_STATUS HDMIFunc_Status(void);
#define			HDMIFunc_GetStatus		HDMIFunc_Status
void 			HDMIFunc_SetStatus(AHC_HDMI_STATUS status);
AHC_BOOL 		HDMIFunc_ThumbnailOption(UINT8 ubOp);
AHC_BOOL 		HDMIFunc_ThumbnailEditOneFile(void);
AHC_BOOL 		HDMIFunc_PlaybackEditOneFile(UINT8 Op);
UINT16 			HDMIFunc_GetMenuOsdID(void);
UINT16 			HDMIFunc_GetUImodeOsdID(void);
AHC_BOOL 		HDMIFunc_IsPreview(void);
AHC_BOOL 		HDMIFunc_IsPlayback(void);
AHC_BOOL 		HDMIFunc_IsInHdmiMode(void);
AHC_BOOL 		HDMIFunc_CheckHdmiStatus(UINT8 ubCheckStatus);

#endif  // _STATEHDMIFUNC_H_

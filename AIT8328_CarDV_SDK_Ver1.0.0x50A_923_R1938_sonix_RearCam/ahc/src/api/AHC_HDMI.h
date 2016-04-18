//==============================================================================
/**
 @file AHC_HDMI.h
 @brief AHC HDMI display functions
 @author Rogers Chen
 @version 1.0
*/

#ifndef __AHC_HDMI_H__
#define __AHC_HDMI_H__

#include "AHC_Common.h"

/*===========================================================================
 * Enum define
 *===========================================================================*/

typedef enum _AHC_HDMI_STATUS
{
    AHC_HDMI_NONE_STATUS = 0, 
    AHC_HDMI_BROWSER_STATUS,    
    AHC_HDMI_MOVIE_PB_STATUS,
    AHC_HDMI_PHOTO_PB_STATUS,
    AHC_HDMI_SLIDE_SHOW_STATUS,
    AHC_HDMI_VIDEO_PREVIEW_STATUS,
    AHC_HDMI_DSC_PREVIEW_STATUS,
    AHC_HDMI_MENU_STATUS,
    AHC_HDMI_NET_PLAYBACK_STATUS,
    AHC_HDMI_MENU_EXIT_STATUS
} AHC_HDMI_STATUS;

typedef enum _AHC_HDMI_OSD_MODE
{
    AHC_HDMI_720P_OSD_1280x720 = 0,
    AHC_HDMI_720P_OSD_320D4x240D3,
    AHC_HDMI_1080I_OSD_1920x1080,
    AHC_HDMI_1080I_OSD_960D2x540D2,
    AHC_HDMI_1080I_OSD_480D4x270D4
} AHC_HDMI_OSD_MODE;

/*===========================================================================
 * HDMI functions
 *===========================================================================*/ 

void AHC_HDMI_SetMode(AHC_HDMI_STATUS HdmiStatus, AHC_HDMI_OSD_MODE HdmiOsdMode);
AHC_HDMI_STATUS AHC_HDMI_GetStatus(void);
AHC_HDMI_OSD_MODE AHC_HDMI_GetOSDMode(void); 

#endif //__AHC_HDMI_H__
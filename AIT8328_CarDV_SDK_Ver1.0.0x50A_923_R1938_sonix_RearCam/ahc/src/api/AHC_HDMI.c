//==============================================================================
/**
 @file AHC_HDMI.c
 @brief AHC HDMI display functions
 @author Rogers Chen
 @version 1.0
*/
#include "AHC_Common.h"
#include "AHC_HDMI.h"
#include "AHC_general.h"
#include "AHC_display.h"
#include "AHC_UF.h"
#include "MenuDrawCommon.h"
#include "menusetting.h"
#include "ColorDefine.h"
#include "LedControl.h"

/*===========================================================================
 * define
 *===========================================================================*/

#define HDMI_OSD0_INDEX    OVL_BUFFER_INDEX	//(16)
#define HDMI_OSD1_INDEX    OVL_BUFFER_INDEX1//(17)

/*===========================================================================
 * Global varible
 *===========================================================================*/

static AHC_HDMI_OSD_MODE    m_HdmiOsdMode;
static UINT16               m_HdmiOsdWidth, m_HdmiOsdHeight;
static AHC_HDMI_STATUS		m_HdmiStatus = AHC_HDMI_NONE_STATUS;

/*===========================================================================
 * HDMI functions
 *===========================================================================*/

void AHC_HDMI_SetMode(AHC_HDMI_STATUS HdmiStatus, AHC_HDMI_OSD_MODE HdmiOsdMode)
{
	UINT32 iVal[2];

	m_HdmiStatus = HdmiStatus;
	m_HdmiOsdMode = HdmiOsdMode;

    AHC_OSDUninit();
    
	AHC_SetMode( AHC_MODE_IDLE );
    
	if((HdmiStatus == AHC_HDMI_PHOTO_PB_STATUS) || (HdmiStatus == AHC_HDMI_SLIDE_SHOW_STATUS)) {
        AIHC_SetGUIMemStartAddr(AHC_GUI_TEMP_BASE_ADDR_HDMI_PHOTO_PB);
    }
    else {
        AIHC_SetGUIMemStartAddr(AHC_GUI_TEMP_BASE_ADDR_HDMI);
    }

	if(m_HdmiStatus == AHC_HDMI_NONE_STATUS)
	{
	    extern UINT32 ObjSelect;
	    extern void InitOSD(void);

		AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
    	InitOSD();
		AHC_UF_GetCurrentIndex(&ObjSelect);

		#ifndef CAR_DV
        LedCtrl_LcdBackLightMask(AHC_FALSE);
        #endif
        LedCtrl_LcdBackLight(MMP_TRUE);
		return;
	}

	switch(MenuSettingConfig()->uiHDMIOutput) {
    case HDMI_OUTPUT_1080I:
        AHC_SetHDMIOutputMode(AHC_DISPLAY_HDMIOUTPUT_1920X1080I);
    break;
    case HDMI_OUTPUT_720P:
        AHC_SetHDMIOutputMode(AHC_DISPLAY_HDMIOUTPUT_1280X720P);
    break;
    case HDMI_OUTPUT_480P:
        AHC_SetHDMIOutputMode(AHC_DISPLAY_HDMIOUTPUT_720X480P);
    break;
    default:
	case HDMI_OUTPUT_1080P:
        AHC_SetHDMIOutputMode(AHC_DISPLAY_HDMIOUTPUT_1920X1080P);
    break;
    }

	switch(m_HdmiOsdMode) {
    case AHC_HDMI_720P_OSD_1280x720:
    	m_HdmiOsdWidth  = 1280;
    	m_HdmiOsdHeight = 720;
    	AHC_SetDisplayOutputDev(DISP_OUT_HDMI, AHC_DISPLAY_DUPLICATE_1X);
	break;
    case AHC_HDMI_720P_OSD_320D4x240D3:
	    m_HdmiOsdWidth  = 320;
	    m_HdmiOsdHeight = 240;
		AHC_SetDisplayOutputDev(DISP_OUT_HDMI, AHC_DISPLAY_DUPLICATE_4X_1X);
	break;
	case AHC_HDMI_1080I_OSD_1920x1080:
		m_HdmiOsdWidth  = 1920;
    	m_HdmiOsdHeight = 1080;
		AHC_SetDisplayOutputDev(DISP_OUT_HDMI, AHC_DISPLAY_DUPLICATE_1X);
	break;
	case AHC_HDMI_1080I_OSD_960D2x540D2:
		m_HdmiOsdWidth  = 960;
   		m_HdmiOsdHeight = 540;
		AHC_SetDisplayOutputDev(DISP_OUT_HDMI, AHC_DISPLAY_DUPLICATE_2X);
	break;
	case AHC_HDMI_1080I_OSD_480D4x270D4:
		m_HdmiOsdWidth  = 480;
    	m_HdmiOsdHeight = 270;
		AHC_SetDisplayOutputDev(DISP_OUT_HDMI, AHC_DISPLAY_DUPLICATE_4X);
	break;
    }

    AHC_OSDInit(0, 0, 0, 0, 0, 0, 0, 0, WMSG_LAYER_WIN_COLOR_FMT, AHC_FALSE);

    AHC_OSDCreateBuffer(HDMI_OSD0_INDEX,
                        m_HdmiOsdWidth,
                        m_HdmiOsdHeight,
                        OSD_COLOR_RGB565);
   	iVal[0] = 1;
    iVal[1] = OSD_COLOR_TRANSPARENT;

    AHC_OSDSetDisplayAttr(HDMI_OSD0_INDEX,
                          AHC_OSD_ATTR_TRANSPARENT_ENABLE,
                          iVal);

   	AHC_OSDCreateBuffer(HDMI_OSD1_INDEX,
                        m_HdmiOsdWidth,
                        m_HdmiOsdHeight,
                        OSD_COLOR_RGB565);

    AHC_OSDSetDisplayAttr(HDMI_OSD1_INDEX,
                          AHC_OSD_ATTR_TRANSPARENT_ENABLE,
                          iVal);

}

AHC_HDMI_OSD_MODE AHC_HDMI_GetOSDMode(void)
{
	return m_HdmiOsdMode;
}

AHC_HDMI_STATUS AHC_HDMI_GetStatus(void)
{
	return m_HdmiStatus;
}


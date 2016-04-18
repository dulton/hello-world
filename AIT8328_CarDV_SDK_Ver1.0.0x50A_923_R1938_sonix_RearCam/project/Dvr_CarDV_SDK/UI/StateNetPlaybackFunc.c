/*===========================================================================
 * Include files
 *===========================================================================*/
#include "Customer_config.h"

#include "AHC_General.h"
#include "AHC_Message.h"
#include "AHC_Display.h"
#include "AHC_Menu.h"
#include "AHC_Dcf.h"
#include "AHC_UF.h"
#include "AHC_Media.h"
#include "AHC_Os.h"
#include "AHC_Fs.h"
#include "AHC_Warningmsg.h"
#include "AHC_Usb.h"
#include "AHC_General_CarDV.h"
#include "AHC_General.h"
#include "IconPosition.h"
#include "ColorDefine.h"
#include "MenuDrawCommon.h"
#include "StateTVFunc.h"
#include "StateHDMIFunc.h"
#if defined(WIFI_PORT) && (WIFI_PORT == 1)
#include "netapp.h"
#endif

/*===========================================================================
 * Macro define
 *===========================================================================*/


/*===========================================================================
 * Global variable
 *===========================================================================*/


/*===========================================================================
 * Extern variable
 *===========================================================================*/

extern AHC_BOOL bForce_PowerOff ;
extern AHC_BOOL gbAhcDbgBrk;
/*===========================================================================
 * Extern Function
 *===========================================================================*/

/*===========================================================================
 * Main body
 *===========================================================================*/
static void Draw_NetPlayback(void)
{
	UINT8			bID0;           
    UINT8			bID1;        
    UINT16      	OSDWidth,OSDHeight,OSDColor;
    UINT32      	OSDAddr;
    UINT16			MaxBarLine = 135;                  
    UINT16			BarLineStartX = 90;       
    UINT16			BarLineStartY = 110;             
    const GUI_FONT  *pGuiFont = &GUI_Font20B_ASCII;
	AHC_DISPLAY_OUTPUTPANEL  OutputDevice;   

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    OSDDraw_EnterDrawing(&bID0, &bID1);
    BEGIN_LAYER(bID0);

	AHC_OSDGetBufferAttr(bID0,&OSDWidth,&OSDHeight,&OSDColor,&OSDAddr);
    AHC_GetDisplayOutputDev(&OutputDevice);

	BarLineStartX = (AHC_GET_ATTR_OSD_W(bID0) - MaxBarLine) >> 1;
    BarLineStartY = (AHC_GET_ATTR_OSD_H(bID0) - 20) >> 1;
	
	AHC_OSDSetColor(bID0, OSD_COLOR_BLACK);
	AHC_OSDDrawFillRect(bID0, 0, 0, OSDWidth, OSDHeight);

    //Draw Text
    AHC_OSDSetColor(bID0, RGB565_COLOR_WHITE);
	AHC_OSDSetFont(bID0, pGuiFont);   
    AHC_OSDDrawText(bID0, (UINT8*)"Network playback", BarLineStartX, BarLineStartY, 1);

    OSDDraw_ExitDrawing(&bID0, &bID1);

	END_LAYER(bID0);   
}

void StateNetPlaybackMode(UINT32 ulEvent)
{
    UINT32      ulJobEvent = ulEvent;

    switch(ulJobEvent)
    {
    	case EVENT_NET_EXIT_PLAYBACK 			:

    		if(uiGetLastState() == UI_CLOCK_SETTING_STATE)
    			StateSwitchMode(UI_CLOCK_SETTING_STATE);
    		else
    			StateSwitchMode(UI_VIDEO_STATE);
    		CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
    		
    	break;
    	case EVENT_SD_REMOVED                     :
		
			if(AHC_TRUE == AHC_SDMMC_GetMountState())
			{
				AHC_DisMountStorageMedia(AHC_MEDIA_MMC);
				Enable_SD_Power(0 /* Power Off */);
			}
	
        break;
    	case EVENT_SD_DETECT                      :
            #if (FS_FORMAT_FREE_ENABLE)
            SystemSettingConfig()->byNeedToFormatMediaAsFormatFree = 0; //Reset it in case user plug-in correct card
            if( AHC_CheckMedia_FormatFree( AHC_MEDIA_MMC ) == AHC_FALSE )
            {
                SystemSettingConfig()->byNeedToFormatMediaAsFormatFree = 1;
            }
            else
            #endif
                AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect);
            
            AHC_UF_SetFreeChar(0, DCF_SET_FREECHAR, (UINT8 *) VIDEO_DEFAULT_FLIE_FREECHAR);
        break;
        case EVENT_TV_DETECT                      :
        	if(AHC_IsTVConnectEx()) {
            	SetTVState(AHC_TV_NET_PLAYBACK_STATUS);
            	StateSwitchMode(UI_TVOUT_STATE);
            }
        break;
    #if HDMI_ENABLE
        case EVENT_HDMI_DETECT                    :
        	//HDMIFunc_SetStatus(AHC_HDMI_NET_PLAYBACK_STATUS);
        	HDMIFunc_SetStatus(AHC_HDMI_VIDEO_PREVIEW_STATUS);
            StateSwitchMode(UI_HDMI_STATE);
            HDMIFunc_Enable(AHC_TRUE);
        break;
    #endif
        case EVENT_USB_REMOVED					:
        if(AHC_USB_GetLastStatus() == AHC_USB_NORMAL) // MSDC mode
		{
			//AHC_SetMode(AHC_MODE_IDLE);
        	bForce_PowerOff = AHC_TRUE;
        	AHC_NormalPowerOffPath();
		}
		break;
    }
}
AHC_BOOL NetPlaybackFunc_Init(void* pData)
{
	//if come from JPEG playback state,must disable PIP window
    AHC_SetDisplayMode(DISPLAY_MODE_PIP_DISABLE);
    AHC_OSDSetActive(OVL_DISPLAY_BUFFER, 0);
	Draw_NetPlayback();
	AHC_OSDSetActive(OVL_DISPLAY_BUFFER, 1);
    return AHC_TRUE;
}

AHC_BOOL NetPlaybackFunc_ShutDown(void* pData)
{
    return AHC_TRUE;
}

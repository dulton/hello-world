/*===========================================================================
 * Include file
 *===========================================================================*/

#include "customer_config.h"
#include "AHC_General.h"
#include "AHC_Display.h"
#include "MenuCommon.h"
#include "MenuDrawCommon.h"
#include "MenuDrawingFunc.h"
#include "KeyParser.h"
#include "IconPosition.h"
#include "IconDefine.h"
#include "ColorDefine.h"
#include "DrawStateMSDCFunc.h"
#include "DrawStateVideoFunc.h"
#include "MMPS_Display.h"
#include "StateTVFunc.h"
#include "MenuStateModeSelectMenu.h"

/*===========================================================================
 * Extern function
 *===========================================================================*/

extern void RTNA_LCD_Backlight(MMP_BOOL bEnable);
#if (USB_MODE_SELECT_EN)
extern UINT8 ubUSBSelectedMode;
#endif
extern AHC_BOOL                 gbAhcDbgBrk;
/*===========================================================================
 * Main body
 *===========================================================================*/

void DrawMSDC_USBConnect(void)
{
    UINT8   bID0 = 0;
    UINT8   bID1 = 0;
#if (USB_MODE_SELECT_EN == 0)
    RECT  	recRECT = POS_STATE_USB_CONNECT;
#endif

#ifdef CFG_DRAW_IGNORE_MSDC //may be defined in config_xxx.h
	return;
	/*
	TVFunc_InitPreviewOSD();
	AHC_SetDisplayMode(DISPLAY_MODE_ENABLE);
	MMPS_Display_SetWinActive(MMP_DISPLAY_WIN_PIP      ,0);
	MMPS_Display_SetWinActive(MMP_DISPLAY_WIN_OVERLAY  ,1);
	MMPS_Display_SetWinActive(MMP_DISPLAY_WIN_MAIN     ,0);
	MMPS_Display_SetWinActive(MMP_DISPLAY_WIN_ICON     ,0);
	SetTVState(AHC_TV_VIDEO_PREVIEW_STATUS);
	*/
#endif
    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    OSDDraw_EnterDrawing(&bID0, &bID1);

    OSDDraw_ClearMainDrawBufferAll();
    OSDDraw_ClearOvlDrawOvlBufferAll();

#ifdef FLM_GPIO_NUM
    AHC_OSDRefresh_PLCD();
#endif

    BEGIN_LAYER(bID0);

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, 0xAA000000);
#else
    AHC_OSDSetColor(0xAA000000);
#endif

#ifdef CFG_DRAW_CUS_MSDC_FUNC //may be defined in config_xxx.h
    CFG_DRAW_CUS_MSDC_FUNC(bID0);
#else
#if USB_MODE_SELECT_EN
    if(ubUSBSelectedMode == USB_MSDC_MODE)
        AHC_OSDDrawBitmap(bID0, &bmIcon_MSDC, (320>>1)-25, (240>>1)-30);
    else if(ubUSBSelectedMode == USB_PCAM_MODE)
        AHC_OSDDrawBitmap(bID0, &bmIcon_PCCAM, (320>>1)-25, (240>>1)-30);
#else
    AHC_OSDDrawBitmap(bID0, &bmIcon_USB_Connect, recRECT.uiLeft, recRECT.uiTop);//draw 16bit bmp will hangup.
#endif		
#endif

    AHC_SetDisplayMode(DISPLAY_MODE_PIP_DISABLE); //Disable thumbnail/playback PIP window

    OSDDraw_ExitDrawing(&bID0, &bID1);

    END_LAYER(bID0);

    RTNA_LCD_Backlight(MMP_TRUE);
}

#if 0
void ____Event_Handler____(){ruturn;} //dummy
#endif

void DrawStateMSDCUpdate(UINT32 ubEvent)
{
    switch(ubEvent)
    {
        case EVENT_NONE                           :
        break;

        case EVENT_USB_DETECT                     :
			DrawMSDC_USBConnect();
        break;

        default:
        break;
	}
}

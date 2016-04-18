/*===========================================================================
 * Include file 
 *===========================================================================*/ 

#include "AHC_GUI.h"
#include "AHC_OS.h"
#include "AHC_DCF.h"
#include "AHC_Menu.h"
#include "AHC_Display.h"
#include "AHC_General_CarDV.h"
#include "MenuCommon.h"
#include "MenuDrawCommon.h"
#include "MenuTouchButton.h"
#include "KeyParser.h"
#include "MenuSetting.h"
#include "DrawStateMenuSetting.h"
#include "DrawStateMoviePBFunc.h"
#include "DrawStatePhotoPBFunc.h"
#include "ColorDefine.h"
#include "IconPosition.h"
#include "IconDefine.h"
#include "dsc_Charger.h"
#include "UI_DrawGeneral.h"

/*===========================================================================
 * Extern varible
 *===========================================================================*/ 

extern UINT8 	m_ubBatteryStatus;
extern AHC_BOOL bLockCurJpegFile;
extern AHC_BOOL gbAhcDbgBrk;
/*===========================================================================
 * Main body
 *===========================================================================*/ 

void DrawJpgPb_FileIndex(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
	DrawMovPb_FileIndex(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
}

void DrawJpgPb_FileInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
	DrawMovPb_FileInfo(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
}

void DrawJpgPb_DateInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    DrawMovPb_DateInfo(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);  
}

void DrawJpgPb_LockCurFile(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
#if (QUICK_FILE_EDIT_EN)
    GUI_BITMAP 	IconID 	= BMICOM_FILE_PROTECTKEY;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);

    if(bFlag==AHC_FALSE)
    {
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, bkColor);
#else
        AHC_OSDSetColor(bkColor);
#endif
        AHC_OSDDrawFillRect(ubID, x, y, x+IconID.XSize*ubMag, y+IconID.YSize*ubMag);
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, 0xAA000000);
#else
        AHC_OSDSetColor(0xAA000000);
#endif
    }
    else
    {	
        if(bLockCurJpegFile)
        {
            if(bLarge==LARGE_SIZE)
                AHC_OSDDrawBitmapMag(ubID, &IconID, x, y, ubMag, ubMag);
            else
                AHC_OSDDrawBitmap(ubID, &IconID, x, y);
        }
    }

    END_LAYER(ubID);
#endif	
}

void DrawStateJpgPbInit(void)
{
    UINT8  bID0 = 0;
    UINT8  bID1 = 0;
    UINT8  idx  = 0;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    CHARGE_ICON_ENABLE(AHC_FALSE);

    OSDDraw_EnterDrawing(&bID0, &bID1);

    BEGIN_LAYER(bID0);
    BEGIN_LAYER(bID1);

    OSDDraw_ClearOvlDrawBuffer(bID0);

    OSDDraw_EnableSemiTransparent(bID0, AHC_TRUE);
    OSDDraw_EnableSemiTransparent(bID1, AHC_TRUE);

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, 0xFF000000);
#else
    AHC_OSDSetColor(0xFF000000);
#endif

    END_LAYER(bID0);
    END_LAYER(bID1);

    for (idx = 0; idx < JPGPB_GUI_MAX; idx++)
    {
        UIDrawJpgPBFuncEx(bID0, idx, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    }

    OSDDraw_MainTouchButton(bID0, UI_PLAYBACK_STATE);

    OSDDraw_ExitDrawing(&bID0, &bID1);

    CHARGE_ICON_ENABLE(AHC_TRUE);
}

void DrawStateJpgPbPageSwitch(UINT8 ubPage)
{
#if (SUPPORT_TOUCH_PANEL)//disable temporally, wait touch UI
    UINT8  bID0 = 0;
    UINT8  bID1 = 0;

    OSDDraw_EnterDrawing(&bID0, &bID1);

    OSDDraw_ClearTouchButtons(bID0, JpgPBCtrlPage_TouchButton, ITEMNUM(JpgPBCtrlPage_TouchButton));

    if( TOUCH_MAIN_PAGE == ubPage )
        OSDDraw_MainTouchButton(bID0, UI_PLAYBACK_STATE);
	else
        OSDDraw_CtrlTouchButton(bID0, UI_PLAYBACK_STATE);
    
    OSDDraw_ExitDrawing(&bID0, &bID1);
#endif
}

void DrawStateJpgPbUpdate(void)
{
    UINT8    bID0 = 0;
    UINT8    bID1 = 0;
    static UINT8 SD_status = AHC_FALSE;
#if (QUICK_FILE_EDIT_EN)
    static UINT8 Lock_status = AHC_FALSE;
#endif
	
    OSDDraw_EnterDrawing(&bID0, &bID1);
    
    if(m_ubBatteryStatus != MenuSettingConfig()->uiBatteryVoltage)
	{
   		UIDrawJpgPBFuncEx(bID0, JPGPB_GUI_BATTERY, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    	m_ubBatteryStatus = MenuSettingConfig()->uiBatteryVoltage;
    }
    
    if(SD_status != AHC_SDMMC_GetMountState()) 
    {
		UIDrawJpgPBFuncEx(bID0, JPGPB_GUI_SD_STATE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
        SD_status = AHC_SDMMC_GetMountState();
    }

#if (QUICK_FILE_EDIT_EN) 
 	if(Lock_status != bLockCurJpegFile)
	{
		UIDrawJpgPBFuncEx(bID0, JPGPB_GUI_LOCK_FILE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);	
		Lock_status = bLockCurJpegFile;
	}
#endif	
    
    UIDrawJpgPBFuncEx(bID0, JPGPB_GUI_FILE_INDEX, 	AHC_TRUE,  AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);	
    UIDrawJpgPBFuncEx(bID0, JPGPB_GUI_FILENAME, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);	
 	UIDrawJpgPBFuncEx(bID0, JPGPB_GUI_FILEDATE, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);	
    
    OSDDraw_ExitDrawing(&bID0, &bID1);   
}

void DrawStateJpgPbMode_Update(void)
{
	DrawStateJpgPbInit();  
}

#if 0
void ____Event_Handler____(){ruturn;} //dummy
#endif

void DrawStateJpgPlaybackUpdate(UINT32 ubEvent)
{
    switch(ubEvent)
    {
        case EVENT_NONE                           : 
        break; 
                                                                                            
        case EVENT_KEY_LEFT                       : 
        case EVENT_KEY_RIGHT                      : 
			DrawStateJpgPbUpdate();
        break;
                                                  
        case EVENT_LCD_COVER_NORMAL               :
        case EVENT_LCD_COVER_ROTATE               : 
			AHC_OSDSetActive(OVL_DISPLAY_BUFFER, 0);
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
			AHC_OSDSetFlipDrawMode(OVL_DISPLAY_BUFFER, OSD_FLIP_DRAW_NONE_ENABLE);
			#else
			AHC_OSDSetFlipDrawMode(OSD_FLIP_DRAW_NONE_ENABLE);
			#endif
			DrawStateJpgPbInit();
			AHC_OSDSetActive(OVL_DISPLAY_BUFFER, 1);
			AHC_OSDRefresh(); 
        break;
        
        case EVENT_JPGPB_MODE_INIT                : 
			AHC_OSDSetActive(OVL_DISPLAY_BUFFER, 0);
			DrawStateJpgPbInit();
			AHC_OSDSetActive(OVL_DISPLAY_BUFFER, 1);
			AHC_OSDRefresh();
        break;

        case EVENT_JPGPB_TOUCH_PREV_PRESS         : 
        case EVENT_JPGPB_TOUCH_NEXT_PRESS         : 
			DrawStateJpgPbUpdate(); 
        break;

        case EVENT_JPGPB_UPDATE_MESSAGE			  :
			DrawStateJpgPbUpdate();
        break;
        
        default:
        break;
    }
}

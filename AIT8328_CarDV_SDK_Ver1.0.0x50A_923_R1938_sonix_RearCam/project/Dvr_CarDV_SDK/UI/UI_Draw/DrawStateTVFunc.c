/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "customer_config.h"
#include "AHC_GUI.h"
#include "AHC_OS.h"
#include "AHC_Browser.h"
#include "AHC_Macro.h"
#include "AHC_Dcf.h"
#include "AHC_Uf.h"
#include "AHC_Menu.h"
#include "AHC_General.h"
#include "AHC_Media.h"
#include "AHC_Display.h"
#include "AHC_General_CarDV.h"
#include "MenuCommon.h"
#include "MenuDrawCommon.h"
#include "KeyParser.h"
#include "MenuSetting.h"
#include "MenuDrawingFunc.h"
#include "DrawStateMenuSetting.h"
#include "DrawStateBrowserFunc.h"
#include "DrawStateHDMIFunc.h"
#include "DrawStateTVFunc.h"
#include "DrawStateVideoFunc.h"
#include "DrawStateCameraFunc.h"
#include "DrawStateMoviePBFunc.h"
#include "DrawStatePhotoPBFunc.h"
#include "ColorDefine.h"
#include "IconPosition.h"
#include "IconDefine.h"
#include "StateTVFunc.h"
#include "MediaPlaybackCtrl.h"
#include "OsdStrings.h"
#include "ShowOSDFunc.h"
#include "UI_DrawGeneral.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/ 
 
#if (DIR_KEY_TYPE!=KEY_TYPE_4KEY)
#define SHOW_TV_DIR_ICON		(0)
#else
#define SHOW_TV_DIR_ICON		(0)
#endif

/*===========================================================================
 * Extern varible
 *===========================================================================*/ 

extern UINT8 		m_ubBatteryStatus;
extern UINT8 		m_DelFile;
extern AHC_BOOL 	PowerOff_InProcess;
extern AHC_BOOL		PowerOff_Option;
extern UINT8		bTVMovieState;
extern AHC_BOOL 	Protecting_InBrowser;
extern AHC_BOOL 	Deleting_InBrowser;
extern AHC_BOOL     gbAhcDbgBrk;
/*===========================================================================
 * Main body
 *===========================================================================*/ 

#if (TVOUT_ENABLE)

void UpdateTVMovPBStatus(UINT8 ubStatus)
{
	bTVMovieState = ubStatus;
}

void DrawTVBrowser_PageInfo(UINT8 ubID)
{
    UINT16  CurrPage 	= 0;
    UINT16  MaxPage 	= 0;
    UINT8   digit 		= 0;
    UINT16  leftpos 	= 0;
    UINT16  tempOffset 	= 20;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    if(Deleting_InBrowser || Protecting_InBrowser)
    	return;

    BEGIN_LAYER(ubID);
    
	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(ubID, OSD_COLOR_TITLE);
	#else
    AHC_OSDSetColor(OSD_COLOR_TITLE);
	#endif

    switch(MenuSettingConfig()->uiTVSystem) 
    {
	    case TV_SYSTEM_NTSC:
	        tempOffset = 30;
	        AHC_OSDDrawFillRect(ubID, 50+tempOffset, 0, 145+tempOffset, POS_TV_NTSC_BROWSER_OSD_H-3);       
	    break;
	    case TV_SYSTEM_PAL:
	        tempOffset = 70;
	        AHC_OSDDrawFillRect(ubID, 50+tempOffset, 0, 145+tempOffset, POS_TV_PAL_BROWSER_OSD_H-3);        
	    break; 
	    default:
	    break;
    }    
    
    DrawBrowser_GetPageInfo(&CurrPage, &MaxPage);

    if(0 != MaxPage)
    {
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, OSD_COLOR_WHITE); 
        AHC_OSDSetBkColor(ubID, OSD_COLOR_TITLE);
        AHC_OSDSetFont(ubID, &GUI_Font16B_1);
        AHC_OSDSetPenSize(ubID, 2);
		#else
        AHC_OSDSetColor(OSD_COLOR_WHITE); 
        AHC_OSDSetBkColor(OSD_COLOR_TITLE);
        AHC_OSDSetFont(&GUI_Font16B_1);
        AHC_OSDSetPenSize(2);
		#endif
            
        digit = NumOfDigit(MaxPage);

        switch(MenuSettingConfig()->uiTVSystem) 
        {
	        case TV_SYSTEM_NTSC:
	        case TV_SYSTEM_PAL:
	            leftpos = POS_PAGEINFO(digit, 60+tempOffset);
	            AHC_OSDDrawDec(ubID, CurrPage, leftpos, 10, digit);
	        
	            leftpos = POS_PAGEINFO(digit, 70+tempOffset);
	            AHC_OSDDrawDec(ubID, MaxPage, leftpos,  45, digit);
	        
	            AHC_OSDDrawLine(ubID, 100+tempOffset, 50+tempOffset, 30, 40);
	        break;
	
			default:
	        break;
        }     
    }

    END_LAYER(ubID);
}

void DrawTVBrowser_SwitchMode(UINT8 ulID, OP_MODE_SETTING OpMode)
{
    UINT16  ulIconX, ulIconY;
    UINT32 	ulTimeX, ulTimeY;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }
    
    switch(MenuSettingConfig()->uiTVSystem) 
    {
	    case TV_SYSTEM_NTSC:
	        ulIconX = 550;
	        ulIconY = 3;
	        ulTimeX = POS_TV_NTSC_BRO_TOTALTIME_X;
	        ulTimeY = POS_TV_NTSC_BRO_TOTALTIME_Y; 
	    break;
	    case TV_SYSTEM_PAL:
	        ulIconX = 530;
	        ulIconY = 3;
			ulTimeX = POS_TV_PAL_BRO_TOTALTIME_X;
	        ulTimeY = POS_TV_PAL_BRO_TOTALTIME_Y;      
	    break;
	    default:
	    break;
    }    

    BEGIN_LAYER(ulID);
    
	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	AHC_OSDSetColor(ulID, OSD_COLOR_TITLE);
	#else
	AHC_OSDSetColor(OSD_COLOR_TITLE);
	#endif

    AHC_OSDDrawFillRect(ulID, ulIconX, ulIconY, ulIconX+50, ulIconY+55);

    if(MOVPB_MODE == OpMode)
    {   
		AHC_OSDDrawBitmapMag(ulID, &bmIcon_Movie_Playback, ulIconX, ulIconY, 2, 2);
        END_LAYER(ulID);
    }
    else if(JPGPB_MODE == OpMode)
    {        
        AHC_OSDDrawBitmapMag(ulID, &bmIcon_Still_Playback, ulIconX, ulIconY, 2, 2);
        END_LAYER(ulID);
        
        DrawTVBrowser_TotalTime(ulID, ulTimeX, ulTimeY, LARGE_SIZE, MAG_2X, AHC_FALSE, OSD_COLOR_TITLE, NULL_PARAM);
    }
}

void DrawTVBrowser_NameInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    UINT16  WordWidth 	= 14;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(ubID, bkColor);    
	#else
    AHC_OSDSetColor(bkColor);    
	#endif
        
    switch(MenuSettingConfig()->uiTVSystem) 
    {
		case TV_SYSTEM_NTSC:
			AHC_OSDDrawFillRect(ubID, x-90, y, x+WordWidth*23, POS_TV_NTSC_BROWSER_OSD_Y0+POS_TV_NTSC_BROWSER_OSD_H-3);
		break;
		case TV_SYSTEM_PAL:
			AHC_OSDDrawFillRect(ubID, x-5,  y, x+WordWidth*22, POS_TV_PAL_BROWSER_OSD_Y0+POS_TV_PAL_BROWSER_OSD_H-3);
		break;
		default:
		break;
	}

    END_LAYER(ubID);

    if(bFlag == AHC_TRUE)
    {
    	DrawBrowser_NameInfo(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
    }
}

void DrawTVBrowser_DateInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    #ifdef CFG_DRAW_IGNORE_TV_DATE_RECT //may be defined in config_xxx.h
    UINT16  WordWidth 	= 14;
    #endif
    
    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	AHC_OSDSetColor(ubID, bkColor);    
	#else
	AHC_OSDSetColor(bkColor);    
	#endif

	/* MDRS, below code will cause "SELECT A FILE" broken*/
    #ifdef CFG_DRAW_IGNORE_TV_DATE_RECT //may be defined in config_xxx.h
	switch(MenuSettingConfig()->uiTVSystem) 
	{
		case TV_SYSTEM_NTSC:
			AHC_OSDDrawFillRect(ubID, x-90, y, x+WordWidth*23, POS_TV_NTSC_BROWSER_OSD_Y0+POS_TV_NTSC_BROWSER_OSD_H-3);
		break;
		case TV_SYSTEM_PAL:
			AHC_OSDDrawFillRect(ubID, x-90, y, x+WordWidth*22, POS_TV_PAL_BROWSER_OSD_Y0+POS_TV_PAL_BROWSER_OSD_H-3);
		break;
	}
	#endif

    END_LAYER(ubID);

    if(bFlag == AHC_TRUE)
    {
    	DrawBrowser_DateInfo(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
    }
}

void DrawTVBrowser_TotalTime(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    UINT16  WordWidth 	= 14;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

	if(bFlag == AHC_FALSE)
	{
        BEGIN_LAYER(ubID);

		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    	AHC_OSDSetColor(ubID, bkColor);
		#else
    	AHC_OSDSetColor(bkColor);
		#endif
    	AHC_OSDDrawFillRect(ubID, x, y, x+WordWidth*12, POS_TV_NTSC_BROWSER_OSD_H-3);

        END_LAYER(ubID);
	}
    else
    {
     	DrawBrowser_TotalTime(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
    }
}

void DrawTVBrowser_FileInfo(UINT8 ubID)
{
    UINT32  MaxDcfObj; 
	UINT32	ulNameX, ulNameY;
	UINT32	ulDateX, ulDateY;
	UINT32	ulTimeX, ulTimeY;		

	switch(MenuSettingConfig()->uiTVSystem) 
	{
	    case TV_SYSTEM_NTSC:
			ulNameX = POS_TV_NTSC_BRO_NAME_X;
			ulNameY = POS_TV_NTSC_BRO_NAME_Y;
			ulDateX = POS_TV_NTSC_BRO_DATE_X;
			ulDateY = POS_TV_NTSC_BRO_DATE_Y;
			ulTimeX = POS_TV_NTSC_BRO_TOTALTIME_X;
			ulTimeY = POS_TV_NTSC_BRO_TOTALTIME_Y;			
	    break;
	    case TV_SYSTEM_PAL:
			ulNameX = POS_TV_PAL_BRO_NAME_X;
			ulNameY = POS_TV_PAL_BRO_NAME_Y;
			ulDateX = POS_TV_PAL_BRO_DATE_X;
			ulDateY = POS_TV_PAL_BRO_DATE_Y;
			ulTimeX = POS_TV_PAL_BRO_TOTALTIME_X;
			ulTimeY = POS_TV_PAL_BRO_TOTALTIME_Y;	
	    break;
    }

    AHC_UF_GetTotalFileCount(&MaxDcfObj);

    if(MaxDcfObj == 0)
    {
        DrawTVBrowser_NameInfo(ubID, ulNameX, ulNameY, LARGE_SIZE, MAG_2X, AHC_FALSE,  OSD_COLOR_TITLE, NULL_PARAM);
        DrawTVBrowser_DateInfo(ubID, ulDateX, ulDateY, LARGE_SIZE, MAG_2X, AHC_FALSE,  OSD_COLOR_TITLE, NULL_PARAM);
    }
    else
    {
        if(MOVPB_MODE == GetCurrentOpMode())
        {
        	DrawTVBrowser_NameInfo(ubID,  ulNameX, ulNameY, LARGE_SIZE, MAG_2X, AHC_TRUE,  OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);
        	DrawTVBrowser_DateInfo(ubID,  ulDateX, ulDateY, LARGE_SIZE, MAG_2X, AHC_TRUE,  OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);
            DrawTVBrowser_TotalTime(ubID, ulTimeX, ulTimeY, LARGE_SIZE, MAG_2X, AHC_TRUE,  OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);
        }
        else if(JPGPB_MODE == GetCurrentOpMode())
        {
        	DrawTVBrowser_NameInfo(ubID, ulNameX, ulNameY, LARGE_SIZE, MAG_2X, AHC_TRUE,  OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);
        	DrawTVBrowser_DateInfo(ubID, ulDateX, ulDateY, LARGE_SIZE, MAG_2X, AHC_TRUE,  OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);
        }
		else if(JPGPB_MOVPB_MODE == GetCurrentOpMode())
        {
        	DrawTVBrowser_NameInfo(ubID, ulNameX, ulNameY, LARGE_SIZE, MAG_2X, AHC_TRUE,  OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);
        	DrawTVBrowser_DateInfo(ubID, ulDateX, ulDateY, LARGE_SIZE, MAG_2X, AHC_TRUE,  OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);
        }
    }
}

void DrawTVMoviePB_FileIndex(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
	DrawMovPb_FileIndex(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
}

void DrawTVMoviePB_NameInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
	DrawMovPb_FileInfo(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
}

void DrawTVMoviePB_PlayInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
	DrawMovPb_PlayInfo(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
}

void DrawTVMoviePB_TotalTime(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
	DrawMovPb_TotalTime(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
}

void DrawTVMoviePB_Status(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
	DrawMovPb_Status(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
}

void DrawTVPhotoPB_FileIndex(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    DrawJpgPb_FileIndex(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
}

void DrawTVPhotoPB_NameInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    DrawJpgPb_FileInfo(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
}

void DrawTVPhotoPB_DateInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    DrawJpgPb_DateInfo(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);  
}

#if 0
void ____TV_Preview_Function_____(){ruturn;} //dummy
#endif

void DrawStateTVPreviewClear(UINT16 bID0)
{
    UINT16	uwTVWidth, uwTVHeight;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    switch(MenuSettingConfig()->uiTVSystem) 
    {
	    case TV_SYSTEM_PAL:
	        AHC_SetDisplayOutputDev(DISP_OUT_TV_PAL, AHC_DISPLAY_DUPLICATE_1X);
	        AHC_GetPalTvDisplayWidthHeight(&uwTVWidth, &uwTVHeight);
	    break;
	    	    
	    //case TV_SYSTEM_NTSC:
	    default:
	        AHC_SetDisplayOutputDev(DISP_OUT_TV_NTSC, AHC_DISPLAY_DUPLICATE_1X);
	        AHC_GetNtscTvDisplayWidthHeight(&uwTVWidth, &uwTVHeight);
		break;
    }

    BEGIN_LAYER(bID0);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	AHC_OSDSetColor(bID0, OSD_COLOR_TRANSPARENT);
	#else
	AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
	#endif

	AHC_OSDDrawFillRect(bID0, 0, 0, uwTVWidth, uwTVHeight);

    END_LAYER(bID0);
}

void DrawStateTVPreviewVideoInit(UINT8 ubID)
{
#if (TVOUT_PREVIEW_EN)
    UINT8	idx	 = 0;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    DrawStateTVPreviewClear(ubID);

    BEGIN_LAYER(ubID);

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(ubID, 0xAA000000);
#else
    AHC_OSDSetColor(0xAA000000);
#endif

    END_LAYER(ubID);
    
	SetCurrentOpMode(VIDEOREC_MODE);
	
	for (idx = 0; idx < TV_GUI_VIDEO_MAX; idx++)
	{
		if(idx==TV_GUI_VIDEO_REC_STATE)
			UpdateVideoRecordStatus(VIDEO_REC_STOP);	

		if(idx==TV_GUI_VIDEO_REC_POINT)
			continue;
			
		UIDrawTV_VideoFuncEx(ubID, idx, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
	}
#endif		
}    

void DrawStateTVPreviewCameraInit(UINT8 ubID)
{
#if (TVOUT_PREVIEW_EN)
    UINT8   idx  = 0;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    DrawStateTVPreviewClear(ubID);

    BEGIN_LAYER(ubID);

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(ubID, 0xAA000000);
#else
    AHC_OSDSetColor(0xAA000000);
#endif

    END_LAYER(ubID);

    SetCurrentOpMode(CAPTURE_MODE);

    for(idx = 0; idx<TV_GUI_DSC_MAX; idx++)
    {			
        UIDrawTV_DSCFuncEx(ubID, idx, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    }
#endif	
}

#if 0
void ____TV_Browser_Function_____(){ruturn;} //dummy
#endif

void DrawStateTVBrowserInit(void)
{
    UINT8  bID0 		= 0;
    UINT8  bID1 		= 0;
    UINT16 tempOffset 	= 20;
    RECT   OsdRect;
    UINT32 ulMediaX,ulMediaY;
    UINT32 ulBattX,ulBattY;   

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

#if (POWER_OFF_CONFIRM_PAGE_EN)
	if(PowerOff_InProcess)
	{
		OSDDraw_EnterDrawing(&bID0, &bID1);
		OSDDraw_ClearOvlDrawOvlBufferAll();
		MenuDrawSubPage_PowerOff_TV_Browser(bID0);
		PowerOff_Option  = CONFIRM_OPT_YES;
		OSDDraw_ExitDrawing(&bID0, &bID1);
		return;
	}
#endif	

	switch(MenuSettingConfig()->uiTVSystem) 
	{
	    case TV_SYSTEM_NTSC:
	        OsdRect.uiLeft 		= POS_TV_NTSC_BROWSER_OSD_X0; 
	        OsdRect.uiTop 		= POS_TV_NTSC_BROWSER_OSD_Y0;
	        OsdRect.uiWidth 	= POS_TV_NTSC_BROWSER_OSD_W;
	        OsdRect.uiHeight 	= POS_TV_NTSC_BROWSER_OSD_H;
			
			ulMediaX			= POS_TV_NTSC_BRO_MEDIA_X;
			ulMediaY			= POS_TV_NTSC_BRO_MEDIA_Y;			
			ulBattX				= POS_TV_NTSC_BRO_BATT_X;
			ulBattY				= POS_TV_NTSC_BRO_BATT_Y;
			
	        tempOffset			= 20;
	    break;
	    case TV_SYSTEM_PAL:
	        OsdRect.uiLeft 		= POS_TV_PAL_BROWSER_OSD_X0; 
	        OsdRect.uiTop 		= POS_TV_PAL_BROWSER_OSD_Y0;
	        OsdRect.uiWidth 	= POS_TV_PAL_BROWSER_OSD_W;
	        OsdRect.uiHeight 	= POS_TV_PAL_BROWSER_OSD_H;

			ulMediaX			= POS_TV_PAL_BRO_MEDIA_X;
			ulMediaY			= POS_TV_PAL_BRO_MEDIA_Y;
			ulBattX				= POS_TV_PAL_BRO_BATT_X;
			ulBattY				= POS_TV_PAL_BRO_BATT_Y;

	        tempOffset 			= 60;
	    break;
    }
    
    OSDDraw_EnterDrawing(&bID0, &bID1);

    BEGIN_LAYER(bID0);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, OSD_COLOR_TITLE);
	#else
    AHC_OSDSetColor(OSD_COLOR_TITLE);
    #endif

    AHC_OSDDrawFillRect2(bID0, &OsdRect); 

    switch(MenuSettingConfig()->uiTVSystem) 
    {
	    case TV_SYSTEM_NTSC:
	        AHC_OSDDrawBitmapMag(bID0, &bmIcon_D_Up, 	tempOffset, 0, 1, 1);
	        AHC_OSDDrawBitmapMag(bID0, &bmIcon_D_Down, 	tempOffset, POS_TV_NTSC_BROWSER_OSD_H>>1, 1, 1);
	    break;
	    case TV_SYSTEM_PAL:
	        AHC_OSDDrawBitmapMag(bID0, &bmIcon_D_Up, 	tempOffset, 0, 1, 1);
	        AHC_OSDDrawBitmapMag(bID0, &bmIcon_D_Down, 	tempOffset, POS_TV_PAL_BROWSER_OSD_H>>1, 1, 1);
	    break;
    }

    END_LAYER(bID0);

    DrawTVBrowser_SwitchMode(bID0, GetCurrentOpMode());
    
    DrawTVBrowser_FileInfo(bID0);
    
    DrawTVBrowser_PageInfo(bID0);

	OSDDrawSetting_MediaType(bID0, ulMediaX, ulMediaY, LARGE_SIZE, MAG_2X, AHC_TRUE,  OSD_COLOR_TITLE, NULL_PARAM);
    
    OSDDrawSetting_BatteryStatus(bID0, ulBattX, ulBattY, LARGE_SIZE, MAG_2X, AHC_TRUE, OSD_COLOR_TITLE, NULL_PARAM);

    BEGIN_LAYER(bID0);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, OSD_COLOR_WHITE); 
    AHC_OSDSetBkColor(bID0, OSD_COLOR_TRANSPARENT);
    AHC_OSDSetPenSize(bID0, 2);
	#else
    AHC_OSDSetColor(OSD_COLOR_WHITE); 
    AHC_OSDSetBkColor(OSD_COLOR_TRANSPARENT);
    AHC_OSDSetPenSize(2);
    #endif
    AHC_OSDDrawLine(bID0, 0, OsdRect.uiWidth-2, OsdRect.uiHeight-2, OsdRect.uiHeight-2);

    OSDDraw_ExitDrawing(&bID0, &bID1);
    END_LAYER(bID0);
    AHC_OSDSetActive(bID0, 1);
	AHC_OSDSetActive(bID1, 1);
}

void DrawStateTVBrowserSwitch(void)
{
    UINT8  bID0 = 0;
    UINT8  bID1 = 0;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    OSDDraw_EnterDrawing(&bID0, &bID1);

    BEGIN_LAYER(bID0);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, 0xAA000000);
	#else
    AHC_OSDSetColor(0xAA000000);
    #endif

    END_LAYER(bID0);

    DrawTVBrowser_SwitchMode(bID0, GetCurrentOpMode());
    
    DrawTVBrowser_PageInfo(bID0);
    
    DrawTVBrowser_FileInfo(bID0);
    
    OSDDraw_ExitDrawing(&bID0, &bID1);
    AHC_OSDSetActive(bID0, 1);
	AHC_OSDSetActive(bID1, 1);
}

void DrawStateTVBrowserPageUpdate(void)
{
    UINT8  bID0 = 0;
    UINT8  bID1 = 0;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    OSDDraw_EnterDrawing(&bID0, &bID1);

    BEGIN_LAYER(bID0);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, 0xAA000000);
	#else
    AHC_OSDSetColor(0xAA000000);
    #endif
    
    END_LAYER(bID0);

    DrawTVBrowser_FileInfo(bID0);
    
    DrawTVBrowser_PageInfo(bID0);
    
    OSDDraw_ExitDrawing(&bID0, &bID1);
    AHC_OSDSetActive(bID0, 1);
    AHC_OSDSetActive(bID1, 1);
}

#if 0
void ____TV_MovPB_Function_____(){ruturn;} //dummy
#endif

void DrawStateTVMoviePBInit(UINT8 bID0, UINT8 bID1)
{

    UINT8   idx;
    UINT32 	iVal[2];
    RECT  	OsdRect = POS_TV_PLAY_OSD;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    //OSDDraw_EnterDrawing(&bID0, &bID1);

    BEGIN_LAYER(bID0);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, OSD_COLOR_TRANSPARENT);
    AHC_OSDSetBkColor(bID0, OSD_COLOR_TRANSPARENT);
	#else
    AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
    AHC_OSDSetBkColor(OSD_COLOR_TRANSPARENT);
    #endif

    AHC_OSDDrawFillRect2(bID0, &OsdRect);
    END_LAYER(bID0);

	for (idx = 0; idx < TV_GUI_MOVPB_MAX; idx++)
	{
		if(idx==TV_GUI_MOVPB_CUR_TIME)
		{
			 if(bTVMovieState != MOVIE_STATE_PLAY)	
				UIDrawTV_MovPBFuncEx(bID0, idx, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		}
		else	
			UIDrawTV_MovPBFuncEx(bID0, idx, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
	}

#if (POWER_OFF_CONFIRM_PAGE_EN)
	if(PowerOff_InProcess)
	{
		MenuDrawSubPage_PowerOff_TV_Play(bID0);
	}
#endif	
    
    BEGIN_LAYER(bID0);
    iVal[0] = 1;
    iVal[1] = OSD_COLOR_TRANSPARENT;
    AHC_OSDSetDisplayAttr(bID0, AHC_OSD_ATTR_TRANSPARENT_ENABLE, iVal);
    AHC_OSDSetDisplayAttr(bID1, AHC_OSD_ATTR_TRANSPARENT_ENABLE, iVal);

    //OSDDraw_ExitDrawing(&bID0, &bID1);
    END_LAYER(bID0);
}

void DrawStateTVMoviePBPlayInfo(void)
{
    UINT8    bID0 = 0;

    OSDDraw_GetLastOvlDrawBuffer(&bID0);
    
    UIDrawTV_MovPBFuncEx(bID0, TV_GUI_MOVPB_CUR_TIME, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);    
}

void DrawStateTVMoviePBDel(UINT8 state)
{
    UINT8  	bID0 		= 0;
    RECT  	OsdRect  	= POS_TV_PLAY_OSD;
    RECT 	StrDelRECT 	= {30,  70,  200, 20};
    RECT 	StrYesRECT 	= {15,  100, 102, 36};
    RECT 	StrNoRECT 	= {140, 100, 102, 36};
    RECT 	tmpRECT;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    OSDDraw_GetLastOvlDrawBuffer(&bID0);

    BEGIN_LAYER(bID0);
    
	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, OSD_COLOR_TRANSPARENT);
	#else
    AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
    #endif
    AHC_OSDDrawFillRect(bID0, OsdRect.uiLeft, OsdRect.uiTop+40, OsdRect.uiLeft+OsdRect.uiWidth, OsdRect.uiTop+40+135); 

    if(state == DEL_FILE_NONE) {
        END_LAYER(bID0);

        return;
    }
        
    switch(state) {
	    #if 0
	    case DEL_FILE_NONE: //Clear Buf
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	        AHC_OSDSetColor(bID0, OSD_COLOR_TRANSPARENT);
			#else
	        AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
		    #endif
	        AHC_OSDDrawFillRect(bID0, OsdRect.uiLeft, OsdRect.uiTop+40, OsdRect.uiLeft+OsdRect.uiWidth, OsdRect.uiTop+40+135); 
	    break;
	    #endif
	    
	    case DEL_FILE_SELECT_NO:  
	        AHC_OSDDrawBitmap(bID0, &BMICON_SUBBAR_WHITE, StrYesRECT.uiLeft, StrYesRECT.uiTop); 
	        tmpRECT.uiLeft = StrYesRECT.uiLeft+2; tmpRECT.uiTop = StrYesRECT.uiTop+4; tmpRECT.uiWidth = StrYesRECT.uiWidth-5; tmpRECT.uiHeight = StrYesRECT.uiHeight-9;
	        OSD_ShowStringPool(bID0,IDS_DS_YES, tmpRECT, OSD_COLOR_FONT, OSD_COLOR_TV_BACKGROUND, GUI_TA_HCENTER|GUI_TA_VCENTER);

	        AHC_OSDDrawBitmap(bID0, &BMICON_SUBBAR_YELLOW, StrNoRECT.uiLeft, StrNoRECT.uiTop); 
	        tmpRECT.uiLeft = StrNoRECT.uiLeft+2; tmpRECT.uiTop = StrNoRECT.uiTop+4; tmpRECT.uiWidth = StrNoRECT.uiWidth-5; tmpRECT.uiHeight = StrNoRECT.uiHeight-9;
	        OSD_ShowStringPool(bID0,IDS_DS_NO, tmpRECT, OSD_COLOR_FONT, OSD_COLOR_TV_BACKGROUND, GUI_TA_HCENTER|GUI_TA_VCENTER);

	        OSD_ShowStringPool(bID0, IDS_DS_DELETE, StrDelRECT, OSD_COLOR_WHITE, OSD_COLOR_TRANSPARENT, GUI_TA_CENTER|GUI_TA_VCENTER);
	    
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	        AHC_OSDSetColor(bID0, OSD_COLOR_WHITE); 
        	AHC_OSDSetBkColor(bID0, OSD_COLOR_TRANSPARENT);
        	AHC_OSDSetFont(bID0, &GUI_Font16B_ASCII);
			#else
	        AHC_OSDSetColor(OSD_COLOR_WHITE); 
        	AHC_OSDSetBkColor(OSD_COLOR_TRANSPARENT);
        	AHC_OSDSetFont(&GUI_Font16B_ASCII);
		    #endif
        	AHC_OSDDrawText(bID0, (UINT8*)"?", 150, 72, 1);
	    break;
	    
	    case DEL_FILE_SELECT_YES: 
	        AHC_OSDDrawBitmap(bID0, &BMICON_SUBBAR_YELLOW, StrYesRECT.uiLeft, StrYesRECT.uiTop); 
	        tmpRECT.uiLeft = StrYesRECT.uiLeft+2; tmpRECT.uiTop = StrYesRECT.uiTop+4; tmpRECT.uiWidth = StrYesRECT.uiWidth-5; tmpRECT.uiHeight = StrYesRECT.uiHeight-9;
	        OSD_ShowStringPool(bID0,IDS_DS_YES, tmpRECT, OSD_COLOR_FONT, OSD_COLOR_TV_BACKGROUND, GUI_TA_HCENTER|GUI_TA_VCENTER);

	        AHC_OSDDrawBitmap(bID0, &BMICON_SUBBAR_WHITE, StrNoRECT.uiLeft, StrNoRECT.uiTop); 
	        tmpRECT.uiLeft = StrNoRECT.uiLeft+2; tmpRECT.uiTop = StrNoRECT.uiTop+4; tmpRECT.uiWidth = StrNoRECT.uiWidth-5; tmpRECT.uiHeight = StrNoRECT.uiHeight-9;
	        OSD_ShowStringPool(bID0,IDS_DS_NO, tmpRECT, OSD_COLOR_FONT, OSD_COLOR_TV_BACKGROUND, GUI_TA_HCENTER|GUI_TA_VCENTER);

	        OSD_ShowStringPool(bID0, IDS_DS_DELETE, StrDelRECT, OSD_COLOR_WHITE, OSD_COLOR_TRANSPARENT, GUI_TA_CENTER|GUI_TA_VCENTER);
	    
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	        AHC_OSDSetColor(bID0, OSD_COLOR_WHITE); 
        	AHC_OSDSetBkColor(bID0, OSD_COLOR_TRANSPARENT);
        	AHC_OSDSetFont(bID0, &GUI_Font16B_ASCII);
			#else
	        AHC_OSDSetColor(OSD_COLOR_WHITE); 
        	AHC_OSDSetBkColor(OSD_COLOR_TRANSPARENT);
        	AHC_OSDSetFont(&GUI_Font16B_ASCII);
		    #endif
        	AHC_OSDDrawText(bID0, (UINT8*)"?", 150, 72, 1);
	    break;
	    
	    case DEL_FILE_DECIDE_NO: 
	        AHC_OSDDrawBitmap(bID0, &BMICON_SUBBAR_WHITE, StrYesRECT.uiLeft, StrYesRECT.uiTop); 
	        tmpRECT.uiLeft = StrYesRECT.uiLeft+2; tmpRECT.uiTop = StrYesRECT.uiTop+4; tmpRECT.uiWidth = StrYesRECT.uiWidth-5; tmpRECT.uiHeight = StrYesRECT.uiHeight-9;
	        OSD_ShowStringPool(bID0,IDS_DS_YES, tmpRECT, OSD_COLOR_FONT, OSD_COLOR_TV_BACKGROUND, GUI_TA_HCENTER|GUI_TA_VCENTER);

	        AHC_OSDDrawBitmap(bID0, &BMICON_SUBBAR_YELLOW, StrNoRECT.uiLeft, StrNoRECT.uiTop); 
	        tmpRECT.uiLeft = StrNoRECT.uiLeft+2; tmpRECT.uiTop = StrNoRECT.uiTop+4; tmpRECT.uiWidth = StrNoRECT.uiWidth-5; tmpRECT.uiHeight = StrNoRECT.uiHeight-9;
	        OSD_ShowStringPool(bID0,IDS_DS_NO, tmpRECT, OSD_COLOR_FONT, OSD_COLOR_SELECT, GUI_TA_HCENTER|GUI_TA_VCENTER);
	        
	        OSD_ShowStringPool(bID0, IDS_DS_DELETE, StrDelRECT, OSD_COLOR_WHITE, OSD_COLOR_TRANSPARENT, GUI_TA_CENTER|GUI_TA_VCENTER);
	    
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	        AHC_OSDSetColor(bID0, OSD_COLOR_WHITE); 
        	AHC_OSDSetBkColor(bID0, OSD_COLOR_TRANSPARENT);
        	AHC_OSDSetFont(bID0, &GUI_Font16B_ASCII);
			#else
	        AHC_OSDSetColor(OSD_COLOR_WHITE); 
        	AHC_OSDSetBkColor(OSD_COLOR_TRANSPARENT);
        	AHC_OSDSetFont(&GUI_Font16B_ASCII);
		    #endif
        	AHC_OSDDrawText(bID0, (UINT8*)"?", 150, 72, 1);        
	    break;
	    
	    case DEL_FILE_DECIDE_YES: 
	        AHC_OSDDrawBitmap(bID0, &BMICON_SUBBAR_YELLOW, StrYesRECT.uiLeft, StrYesRECT.uiTop); 
	        tmpRECT.uiLeft = StrYesRECT.uiLeft+2; tmpRECT.uiTop = StrYesRECT.uiTop+4; tmpRECT.uiWidth = StrYesRECT.uiWidth-5; tmpRECT.uiHeight = StrYesRECT.uiHeight-9;
	        OSD_ShowStringPool(bID0,IDS_DS_YES, tmpRECT, OSD_COLOR_FONT, OSD_COLOR_SELECT, GUI_TA_HCENTER|GUI_TA_VCENTER);

	        AHC_OSDDrawBitmap(bID0, &BMICON_SUBBAR_WHITE, StrNoRECT.uiLeft, StrNoRECT.uiTop); 
	        tmpRECT.uiLeft = StrNoRECT.uiLeft+2; tmpRECT.uiTop = StrNoRECT.uiTop+4; tmpRECT.uiWidth = StrNoRECT.uiWidth-5; tmpRECT.uiHeight = StrNoRECT.uiHeight-9;
	        OSD_ShowStringPool(bID0,IDS_DS_NO, tmpRECT, OSD_COLOR_FONT, OSD_COLOR_TV_BACKGROUND, GUI_TA_HCENTER|GUI_TA_VCENTER);
	        
	        OSD_ShowStringPool(bID0, IDS_DS_DELETE, StrDelRECT, OSD_COLOR_WHITE, OSD_COLOR_TRANSPARENT, GUI_TA_CENTER|GUI_TA_VCENTER);
	    
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	        AHC_OSDSetColor(bID0, OSD_COLOR_WHITE); 
        	AHC_OSDSetBkColor(bID0, OSD_COLOR_TRANSPARENT);
        	AHC_OSDSetFont(bID0, &GUI_Font16B_ASCII);
			#else
	        AHC_OSDSetColor(OSD_COLOR_WHITE); 
        	AHC_OSDSetBkColor(OSD_COLOR_TRANSPARENT);
        	AHC_OSDSetFont(&GUI_Font16B_ASCII);
		    #endif
        	AHC_OSDDrawText(bID0, (UINT8*)"?", 150, 72, 1);	    
	    break;
    }

    END_LAYER(bID0);
}

void DrawStateTVMoviePBStatus(void)
{
    UINT8  bID0 = 0;

    OSDDraw_GetLastOvlDrawBuffer(&bID0);
    
    UIDrawTV_MovPBFuncEx(bID0, TV_GUI_MOVPB_STATUS, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM); 
}

void DrawStateTVMoviePBChangeFile(void)
{
    UINT8  bID0 = 0;
    
    OSDDraw_GetLastOvlDrawBuffer(&bID0);

    UpdateTVMovPBStatus(MOVIE_STATE_PLAY);

    UIDrawTV_MovPBFuncEx(bID0, TV_GUI_MOVPB_FILENAME, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM); 
    UIDrawTV_MovPBFuncEx(bID0, TV_GUI_MOVPB_TOTAL_TIME, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM); 
    UIDrawTV_MovPBFuncEx(bID0, TV_GUI_MOVPB_FILE_INDEX, AHC_TRUE,  AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM); 
    UIDrawTV_MovPBFuncEx(bID0, TV_GUI_MOVPB_STATUS, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
}

#if 0
void ____TV_PhotoPB_Function_____(){ruturn;} //dummy
#endif

void DrawStateTVPhotoPBInit(UINT8 bID0, UINT8 bID1)
{
    //UINT8  	bID0 	= 0;
    //UINT8  	bID1 	= 0;
    UINT8   idx;    
    UINT32 	iVal[2];
    RECT  	OsdRect = POS_TV_PLAY_OSD;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    //OSDDraw_EnterDrawing(&bID0, &bID1);

    BEGIN_LAYER(bID0);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, OSD_COLOR_TRANSPARENT);
	#else
    AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
	#endif
    AHC_OSDDrawFillRect2(bID0, &OsdRect); 

    END_LAYER(bID0);

	for (idx = 0; idx < TV_GUI_JPGPB_MAX; idx++)
	{	
		UIDrawTV_JpgPBFuncEx(bID0, idx, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
	}

#if (SHOW_TV_DIR_ICON)
    BEGIN_LAYER(bID0);
    AHC_OSDDrawBitmap(bID0, &bmIcon_HdmiStillPlay, 100, 45);
    END_LAYER(bID0);
#endif

#if (POWER_OFF_CONFIRM_PAGE_EN)
	if(PowerOff_InProcess)
	{
		MenuDrawSubPage_PowerOff_TV_Play(bID0);
	}
#endif	

    BEGIN_LAYER(bID0);
    iVal[0] = 1;
    iVal[1] = OSD_COLOR_TRANSPARENT;
    AHC_OSDSetDisplayAttr(bID0, AHC_OSD_ATTR_TRANSPARENT_ENABLE, iVal);
    AHC_OSDSetDisplayAttr(bID1, AHC_OSD_ATTR_TRANSPARENT_ENABLE, iVal);
    
    //OSDDraw_ExitDrawing(&bID0, &bID1);
    END_LAYER(bID0);
}

void DrawStateTVPhotoPBChangeFile(void)
{
    UINT8  bID0 = 0;
    UINT8  bID1 = 0;
    
    OSDDraw_EnterDrawing(&bID0, &bID1);

	UIDrawTV_JpgPBFuncEx(bID0, TV_GUI_JPGPB_FILENAME, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
	UIDrawTV_JpgPBFuncEx(bID0, TV_GUI_JPGPB_FILEDATE, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
	UIDrawTV_JpgPBFuncEx(bID0, TV_GUI_JPGPB_FILE_INDEX, AHC_TRUE,  AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);

#if (SHOW_TV_DIR_ICON)    
    AHC_OSDDrawBitmap(bID0, &bmIcon_HdmiStillPlay, 100, 45);
#endif
    
    OSDDraw_ExitDrawing(&bID0, &bID1);
}

#if 0
void ____TV_Slideshow_Function_____(){ruturn;} //dummy
#endif

void DrawStateTVSlideShowInit(void)
{
    UINT8  	bID0 	= 0;
    UINT8  	bID1 	= 0;
    UINT32 	iVal[2];
    RECT  	OsdRect = POS_TV_PLAY_OSD;

    OSDDraw_EnterDrawing(&bID0, &bID1);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, OSD_COLOR_TRANSPARENT);
	#else
    AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
	#endif
    AHC_OSDDrawFillRect2(bID0, &OsdRect); 

    if(GetPlayBackFileType() == PLAYBACK_IMAGE_TYPE) 
    {
		UIDrawTV_JpgPBFuncEx(bID0, TV_GUI_JPGPB_FILENAME, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		UIDrawTV_JpgPBFuncEx(bID0, TV_GUI_JPGPB_FILEDATE, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		UIDrawTV_JpgPBFuncEx(bID0, TV_GUI_JPGPB_FILE_INDEX, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    }
    else 
    {
        UpdateTVMovPBStatus(MOVIE_STATE_PLAY);

   		UIDrawTV_MovPBFuncEx(bID0, TV_GUI_MOVPB_FILENAME, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM); 
    	UIDrawTV_MovPBFuncEx(bID0, TV_GUI_MOVPB_TOTAL_TIME, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM); 
    	UIDrawTV_MovPBFuncEx(bID0, TV_GUI_MOVPB_STATUS, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    	UIDrawTV_MovPBFuncEx(bID0, TV_GUI_MOVPB_FILE_INDEX, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM); 
    }

#if (SHOW_TV_DIR_ICON)     
    AHC_OSDDrawBitmap(bID0, &bmIcon_HdmiSlideShow, 100, 45);
#endif

    iVal[0] = 1;
    iVal[1] = OSD_COLOR_TRANSPARENT;
    AHC_OSDSetDisplayAttr(bID0, AHC_OSD_ATTR_TRANSPARENT_ENABLE, iVal);
    AHC_OSDSetDisplayAttr(bID1, AHC_OSD_ATTR_TRANSPARENT_ENABLE, iVal);
    
    OSDDraw_ExitDrawing(&bID0, &bID1);
}

void DrawStateTVSlideShowChangeFile(void)
{
    UINT8  bID0 = 0;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    OSDDraw_GetLastOvlDrawBuffer(&bID0); 

    BEGIN_LAYER(bID0);
    
	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, OSD_COLOR_TRANSPARENT);
    AHC_OSDSetBkColor(bID0, OSD_COLOR_TRANSPARENT);
	#else
    AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
    AHC_OSDSetBkColor(OSD_COLOR_TRANSPARENT);
	#endif
    AHC_OSDDrawFillRect(bID0, 0, 20, 240, 40);
    END_LAYER(bID0);

    if(GetPlayBackFileType() == PLAYBACK_IMAGE_TYPE) 
    {
        BEGIN_LAYER(bID0);
        AHC_OSDDrawFillRect(bID0, 0, 170, 240, 240);
        END_LAYER(bID0);

		UIDrawTV_JpgPBFuncEx(bID0, TV_GUI_JPGPB_FILENAME, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		UIDrawTV_JpgPBFuncEx(bID0, TV_GUI_JPGPB_FILEDATE, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		UIDrawTV_JpgPBFuncEx(bID0, TV_GUI_JPGPB_FILE_INDEX, AHC_TRUE,  AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    }
    else 
    {
        UpdateTVMovPBStatus(MOVIE_STATE_PLAY);

   		UIDrawTV_MovPBFuncEx(bID0, TV_GUI_MOVPB_FILENAME, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM); 
    	UIDrawTV_MovPBFuncEx(bID0, TV_GUI_MOVPB_TOTAL_TIME, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM); 
    	UIDrawTV_MovPBFuncEx(bID0, TV_GUI_MOVPB_STATUS, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    	UIDrawTV_MovPBFuncEx(bID0, TV_GUI_MOVPB_FILE_INDEX, AHC_TRUE,  AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    }
 
#if (SHOW_TV_DIR_ICON)       
    BEGIN_LAYER(bID0);
    AHC_OSDDrawBitmap(bID0, &bmIcon_HdmiSlideShow, 100, 45);
    END_LAYER(bID0);
#endif
}

#endif

#if 0
void ____TV_Draw_Function_____(){ruturn;} //dummy
#endif

void DrawStateTVBrowserUpdate(UINT32 ulEvent)
{	
#if (TVOUT_ENABLE)


    switch( ulEvent )
    {
        case EVENT_NONE                           :  
        break;
        
        case EVENT_KEY_MODE                       :
        case EVENT_KEY_NEXT_MODE:
        case EVENT_KEY_PREV_MODE:
            DrawStateTVBrowserSwitch();
        break;
                
        case EVENT_KEY_UP                         : 
        case EVENT_KEY_DOWN                       : 
        case EVENT_KEY_LEFT                       : 
        case EVENT_KEY_RIGHT                      :
        case EVENT_TV_BROWSER_KEY_PAGE_UP         :
        case EVENT_TV_BROWSER_KEY_PAGE_DOWN       : 
            DrawStateTVBrowserPageUpdate();
        break;
       
        case EVENT_TV_CHANGE_MODE_INIT          : 
			DrawStateTVBrowserInit();
        break;
        case EVENT_PARKING_KEY:
            DrawStateTVBrowserPageUpdate();
            break;
        default:
        break;
    }

#endif    
}

void DrawStateTVMoviePBUpdate(UINT32 ulEvent)
{
#if (TVOUT_ENABLE)
	
    UINT32 uState;
    UINT8  bID0 = 0;
    UINT8  bID1 = 0;

    OSDDraw_EnterDrawing(&bID0, &bID1);
	
    switch( ulEvent )
    {
        case EVENT_NONE                           :  
        break;
                
        case EVENT_KEY_DOWN                       : 
            DrawStateTVMoviePBDel(m_DelFile);
        break;
        
        case EVENT_KEY_LEFT                       : 
            AHC_GetSystemStatus(&uState);
 
            switch(uState)
            {
	            case STATE_MOV_IDLE :                                 
	            break;
	            
	            case STATE_MOV_EXECUTE :
	            	UpdateTVMovPBStatus(MOVIE_STATE_REW);
	                DrawStateTVMoviePBStatus();	                
	            break;
	            
	            case STATE_MOV_PAUSE   :
	            	UpdateTVMovPBStatus(MOVIE_STATE_PFB);
	                DrawStateTVMoviePBStatus();	
	            break;
	            
	            default                :
	            break;
            }
        break;
        
        case EVENT_KEY_RIGHT                      : 
            AHC_GetSystemStatus(&uState);
 
            switch(uState)
            {
	            case STATE_MOV_IDLE :	                                 
	            break;
	            
	            case STATE_MOV_EXECUTE :
	            	UpdateTVMovPBStatus(MOVIE_STATE_FF);
	                DrawStateTVMoviePBStatus();	                
	            break;
	            
	            case STATE_MOV_PAUSE   :
	            	UpdateTVMovPBStatus(MOVIE_STATE_PFF);
	                DrawStateTVMoviePBStatus();	
	            break;
	            
	            default                :
	            break;
            }
        break;
                
        case EVENT_TV_DEL_FILE                  : 
            DrawStateTVMoviePBChangeFile();
        break;
        
        case EVENT_TV_BROWSER_KEY_PLAYBACK      :
            if(m_DelFile) {
                DrawStateTVMoviePBDel(m_DelFile+2);
                break;
            }  
        break;
        
        case EVENT_MOVPB_TOUCH_PLAY_PRESS         :
        	UpdateTVMovPBStatus(MOVIE_STATE_PLAY); 
			DrawStateTVMoviePBStatus();
        break;
        
        case EVENT_MOVPB_TOUCH_PAUSE_PRESS        : 

			AHC_GetVideoPlayStopStatus(&uState);
			
			if ((AHC_VIDEO_PLAY_EOF == uState) || (AHC_VIDEO_PLAY_ERROR_STOP == uState))
				UpdateTVMovPBStatus(MOVIE_STATE_STOP); 
			else
                UpdateTVMovPBStatus(MOVIE_STATE_PAUSE);
			
			DrawStateTVMoviePBStatus();	  
        break;
        
        case EVENT_TV_PLAY_STOP                 : 
			UpdateTVMovPBStatus(MOVIE_STATE_STOP);
			DrawStateTVMoviePBStatus();
        break; 
               
        case EVENT_MOVPB_UPDATE_MESSAGE           :
			DrawStateTVMoviePBPlayInfo();
        break;
        
        case EVENT_TV_CHANGE_MODE_INIT          : 
			AHC_OSDSetActive(bID0, 0);
			AHC_OSDSetActive(bID1,0);
			DrawStateTVMoviePBInit(bID0, bID1);
			AHC_OSDSetActive(bID0, 1);
			AHC_OSDSetActive(bID1,1);
        break;
                
        default:
        break;
    }
    OSDDraw_ExitDrawing(&bID0, &bID1);
	
#endif    
}

void DrawStateTVPhotoPBUpdate(UINT32 ulEvent)
{
#if (TVOUT_ENABLE)
    UINT8  bID0 = 0;
    UINT8  bID1 = 0;

    //OSDDraw_EnterDrawing(&bID0, &bID1);
	
    switch( ulEvent )
    {
        case EVENT_NONE                           :  
        break;
        
        case EVENT_KEY_DOWN                       : 
            DrawStateTVMoviePBDel(m_DelFile);
        break;
        
        case EVENT_KEY_LEFT                       : 
        case EVENT_KEY_RIGHT                      : 
            DrawStateTVPhotoPBChangeFile();
        break;
        
        case EVENT_TV_DEL_FILE                  : 
            DrawStateTVPhotoPBChangeFile();
        break;
        
        case EVENT_TV_BROWSER_KEY_PLAYBACK      :
            if(m_DelFile) {
                DrawStateTVMoviePBDel(m_DelFile+2);
                break;
            }  
        break;
              
        case EVENT_TV_CHANGE_MODE_INIT          : 
        	OSDDraw_EnterDrawing(&bID0, &bID1);
			AHC_OSDSetActive(bID0, 0);
			AHC_OSDSetActive(bID1, 0);
			DrawStateTVPhotoPBInit(bID0, bID1);
			AHC_OSDSetActive(bID0, 1);
			AHC_OSDSetActive(bID1, 1);
			OSDDraw_ExitDrawing(&bID0, &bID1);
        break;
      
        case EVENT_MOVPB_UPDATE_MESSAGE			:
        break;
         
        default:
        break;
    }
    //OSDDraw_ExitDrawing(&bID0, &bID1);
	
#endif
}

void DrawStateTVSlideShowUpdate(UINT32 ulEvent)
{
#if (TVOUT_ENABLE)
    UINT8  bID0 = 0;
    UINT8  bID1 = 0;

    OSDDraw_EnterDrawing(&bID0, &bID1);
	
    switch( ulEvent )
    {
        case EVENT_NONE                           :  
        break;
                
        case EVENT_KEY_LEFT                       : 
        case EVENT_KEY_RIGHT                      :      
        case EVENT_SLIDESHOW_UPDATE_FILE          : 
            AHC_OSDSetActive(bID0, 0);
            AHC_OSDSetActive(bID1, 0);
            DrawStateTVSlideShowChangeFile();
            AHC_OSDSetActive(bID0, 1);
            AHC_OSDSetActive(bID1, 1);
        break;
       
        case EVENT_TV_CHANGE_MODE_INIT          : 
			AHC_OSDSetActive(bID0, 0);
			AHC_OSDSetActive(bID1, 0);
			DrawStateTVSlideShowInit();
			AHC_OSDSetActive(bID0, 1);
			AHC_OSDSetActive(bID1, 1);
        break;
        
        case EVENT_MOVPB_UPDATE_MESSAGE			:
			if(GetPlayBackFileType() == PLAYBACK_VIDEO_TYPE) {
				DrawStateTVMoviePBPlayInfo();
			}
        break;
         
        default:
        break;
    }
    OSDDraw_ExitDrawing(&bID0, &bID1);
	
#endif    
}

void DrawStateTVVideoUpdate(UINT32 ulEvent)
{
#if (TVOUT_ENABLE)	
	DrawStateVideoRecUpdate(ulEvent);
#endif
}

void DrawStateTVCameraUpdate(UINT32 ulEvent)
{
#if (TVOUT_ENABLE)	
	DrawStateCaptureUpdate(ulEvent);
#endif
}

void DrawStateTVSleepUpdate(UINT32 ulEvent)
{
	// TODO:
}

void DrawStateTVNetPlaybackInit(void)
{
	UINT8			bID0;           
    UINT8			bID1;        
    UINT16      	OSDWidth,OSDHeight,OSDColor;
    UINT32      	OSDAddr;
    UINT16			MaxBarLine = 135;       
    UINT16			MaxBarLineHeigt = 10;            
    UINT16			BarLineStartX = 90;       
    UINT16			BarLineStartY = 110;            
    const GUI_FONT  *pGuiFont = &GUI_Font20B_ASCII;
	AHC_DISPLAY_OUTPUTPANEL  OutputDevice;   
#if TVOUT_PREVIEW
    MMPS_3GPRECD_CONFIGS *pVideoConfig = MMPS_3GPRECD_GetConfig();
#endif

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    OSDDraw_EnterDrawing(&bID0, &bID1);
    BEGIN_LAYER(bID0);

	AHC_OSDGetBufferAttr(bID0,&OSDWidth,&OSDHeight,&OSDColor,&OSDAddr);
    AHC_GetDisplayOutputDev(&OutputDevice);

	switch (OutputDevice) {   
#if TVOUT_PREVIEW
    case MMP_DISPLAY_SEL_NTSC_TV:
        MaxBarLine = 135 << 1;
        MaxBarLineHeigt = 30;

        BarLineStartX = ( pVideoConfig->previewdata[0].usVidDisplayW[VIDRECD_NTSC_PREVIEW_MODE] - MaxBarLine) >> 1;
        BarLineStartY = ( pVideoConfig->previewdata[0].usVidDisplayH[VIDRECD_NTSC_PREVIEW_MODE] - 60) >> 1;
        pGuiFont = &GUI_Font32B_ASCII;
        break;

    case MMP_DISPLAY_SEL_PAL_TV:
        MaxBarLine = 135 << 1;
        MaxBarLineHeigt = 30;
        BarLineStartX = ( pVideoConfig->previewdata[0].usVidDisplayW[VIDRECD_PAL_PREVIEW_MODE] - MaxBarLine) >> 1;
        BarLineStartY = ( pVideoConfig->previewdata[0].usVidDisplayH[VIDRECD_PAL_PREVIEW_MODE] - 60) >> 1;
        pGuiFont = &GUI_Font32B_ASCII;
        break;
#endif

#if HDMI_PREVIEW
    case MMP_DISPLAY_SEL_HDMI:
        // TBD
        break;
#endif
    //case MMP_DISPLAY_SEL_MAIN_LCD:
    default:
        MaxBarLine = 135;
        MaxBarLineHeigt = 10;
        BarLineStartX = (AHC_GET_ATTR_OSD_W(bID0) - MaxBarLine) >> 1;
        BarLineStartY = (AHC_GET_ATTR_OSD_H(bID0) - 20) >> 1;
        pGuiFont = &GUI_Font20B_ASCII;
   	/*#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
       	{
            UINT16  ulOSDTemp = 0;
           	//Swap width/height. Rotate OSD by GUI.
           	ulOSDTemp = OSDWidth;
           	OSDWidth = OSDHeight;
           	OSDHeight = ulOSDTemp;
       	}
	#endif*/
        break;
	}   
	
	AHC_OSDSetColor(bID0, OSD_COLOR_BLACK);
	AHC_OSDDrawFillRect(bID0, 0, 0, OSDWidth, OSDHeight);
    //Draw Text
    AHC_OSDSetColor(bID0, OSD_COLOR_WHITE);
	AHC_OSDSetFont(bID0, pGuiFont);   
    AHC_OSDDrawText(bID0, (UINT8*)"Network playback", BarLineStartX, BarLineStartY, 1);

    OSDDraw_ExitDrawing(&bID0, &bID1);

	END_LAYER(bID0);   
}
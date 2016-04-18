/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "AHC_GUI.h"
#include "AHC_OS.h"
#include "AHC_DCF.h"
#include "AHC_Browser.h"
#include "AHC_Menu.h"
#include "AHC_Display.h"
#include "AHC_General.h"
#include "AHC_General_CarDV.h"
#include "MenuCommon.h"
#include "MenuDrawCommon.h"
#include "KeyParser.h"
#include "MenuSetting.h"
#include "MenuTouchButton.h"
#include "DrawStateMenuSetting.h"
#include "DrawStateMoviePBFunc.h"
#include "DrawStatePhotoPBFunc.h"
#include "DrawStateSlideShowFunc.h"
#include "MediaPlaybackCtrl.h"
#include "IconPosition.h"
#include "IconDefine.h"
#include "ColorDefine.h"
#include "dsc_Charger.h"
#include "UI_DrawGeneral.h"

/*===========================================================================
 * Global varible
 *===========================================================================*/ 

UINT8 			iSlideshowType = 0;
UINT8 			bSlideMovieState = MOVIE_STATE_PAUSE;
UINT8			bSlideAudioState = AUDIO_STATE_PAUSE;

/*===========================================================================
 * Extern varible
 *===========================================================================*/ 

extern UINT8 	m_ubBatteryStatus;
extern AHC_BOOL	bLockCurPlayFile;
extern UINT8	ssFileType;

/*===========================================================================
 * Main body
 *===========================================================================*/ 

void UpdateSlideMovPBStatus(UINT8 ubStatus)
{
	bSlideMovieState = ubStatus;
}

void UpdateSlideAudPBStatus(UINT8 ubStatus)
{
	bSlideAudioState = ubStatus;
}

#if (SLIDESHOW_EN==1)

void DrawSlideShow_MovPbState(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    DrawMovPb_Status(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);    
}

void DrawSlideShow_FileIndex(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    DrawMovPb_FileIndex(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1); 
}

void DrawSlideShow_FileInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    DrawMovPb_FileInfo(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);    
}

void DrawSlideShow_MovPlayInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
	DrawMovPb_PlayInfo(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
}

void DrawSlideShow_MovTotalTime(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
	DrawMovPb_TotalTime(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);     		
}

void DrawSlideShow_LockCurFile(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
#if (QUICK_FILE_EDIT_EN)

    GUI_BITMAP 	IconID 	= BMICOM_FILE_PROTECTKEY;
   
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
   		if(bLockCurPlayFile)
   		{
	        if(bLarge==LARGE_SIZE)
	        	AHC_OSDDrawBitmapMag(ubID, &IconID, x, y, ubMag, ubMag);
	        else
	        	AHC_OSDDrawBitmap(ubID, &IconID, x, y);
		}
	}
#endif	
}

void DrawStateSlideShowMovPbState(void)
{
    UINT8  bID0 = 0;

    OSDDraw_GetLastOvlDrawBuffer(&bID0);
   
	UIDrawSlideFuncEx(bID0, SLIDE_GUI_STATUS, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
}

void DrawStateSlideShowPageSwitch(UINT8 ubPage)
{
#if (SUPPORT_TOUCH_PANEL)
	
	if(PLAYBACK_IMAGE_TYPE == ssFileType)
	{
	    UINT8  bID0 = 0;

		OSDDraw_GetLastOvlDrawBuffer(&bID0);

		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	    AHC_OSDSetColor(bID0, OSD_COLOR_TRANSPARENT);
		#else
	    AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
	    #endif
	    AHC_OSDDrawFillRect(bID0, 90, 50, m_OSD[ubID0]->width, 190);

	    if( TOUCH_MAIN_PAGE == ubPage )
	        OSDDraw_MainTouchButton(bID0, UI_SLIDESHOW_STATE);
		else
	        OSDDraw_CtrlTouchButton(bID0, UI_SLIDESHOW_STATE);
	    
        #ifdef FLM_GPIO_NUM
        AHC_OSDRefresh_PLCD();
        #endif
	}
	else if(PLAYBACK_VIDEO_TYPE == ssFileType)
	{
		//TBD
	}
#endif
}

void DrawStateSlideShowInit(void)
{
	iSlideshowType = GetPlayBackFileType();
	
	switch(iSlideshowType)
	{
		case PLAYBACK_IMAGE_TYPE:
			DrawStateJpgPbInit();
		break;
	
		case PLAYBACK_VIDEO_TYPE:
			DrawStateMovPbInit();
		break;
	
		case PLAYBACK_AUDIO_TYPE:
		#if (AUDIO_REC_PLAY_EN)
			DrawStateAudPbInit();
		#endif
		break;
	}
}

void DrawStateSlideShowMode(void)
{
    UINT8  bID0   = 0;    
    UINT8  ubtype = 0;

	ubtype = GetPlayBackFileType();

   	OSDDraw_GetLastOvlDrawBuffer(&bID0);

	if(iSlideshowType != ubtype)
	{
		iSlideshowType = ubtype;

	    OSDDraw_ClearOvlDrawBuffer(bID0);

		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
   	 	AHC_OSDSetColor(bID0, OSD_COLOR_BLACK);
		#else
   	 	AHC_OSDSetColor(OSD_COLOR_BLACK);
	    #endif

    	UIDrawSlideFuncEx(bID0, SLIDE_GUI_UI_MODE, 	  	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    	UIDrawSlideFuncEx(bID0, SLIDE_GUI_SD_STATE, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    	UIDrawSlideFuncEx(bID0, SLIDE_GUI_BATTERY, 	  	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
	}

	if(PLAYBACK_IMAGE_TYPE == ubtype)
	{
    	UIDrawJpgPBFuncEx(bID0, JPGPB_GUI_FILE_INDEX, 	AHC_TRUE,  AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);	
    	UIDrawJpgPBFuncEx(bID0, JPGPB_GUI_FILENAME, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);	
		UIDrawJpgPBFuncEx(bID0, JPGPB_GUI_FILEDATE, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);	
	}
	else if(PLAYBACK_VIDEO_TYPE == ubtype)
	{
		UIDrawMovPBFuncEx(bID0, MOVPB_GUI_FILENAME,   AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		UIDrawMovPBFuncEx(bID0, MOVPB_GUI_FILE_INDEX, AHC_TRUE,  AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    	UIDrawMovPBFuncEx(bID0, MOVPB_GUI_TOTAL_TIME, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);	
		UIDrawMovPBFuncEx(bID0, MOVPB_GUI_STATUS, 	  AHC_TRUE,  AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
	}
	else if(PLAYBACK_AUDIO_TYPE == ubtype)
	{
	    extern AHC_OSD_INSTANCE 		*m_OSD[];
	    
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	    AHC_OSDSetColor(bID0, OSD_COLOR_BLACK);
		#else
	    AHC_OSDSetColor(OSD_COLOR_BLACK);
	    #endif
		AHC_OSDDrawFillRect(bID0, 0, 0, m_OSD[bID0]->width, m_OSD[bID0]->height);
	
		AHC_OSDDrawBitmap(bID0, &bmIcon_Music, (m_OSD[bID0]->width-76)/2, 60);
	
		UIDrawAudPBFuncEx(bID0, AUDPB_GUI_FILENAME,   AHC_FALSE, AHC_TRUE, OSD_COLOR_BLACK, NULL_PARAM);
		UIDrawAudPBFuncEx(bID0, AUDPB_GUI_FILE_INDEX, AHC_TRUE,  AHC_TRUE, OSD_COLOR_BLACK, NULL_PARAM);
		UIDrawAudPBFuncEx(bID0, AUDPB_GUI_TOTAL_TIME, AHC_FALSE, AHC_TRUE, OSD_COLOR_BLACK, NULL_PARAM);	
		UIDrawAudPBFuncEx(bID0, AUDPB_GUI_STATUS,     AHC_TRUE,  AHC_TRUE, OSD_COLOR_BLACK, NULL_PARAM);
	}
	
    #ifdef FLM_GPIO_NUM
    AHC_OSDRefresh_PLCD();
    #endif
}

void DrawStateSlideShowUpdateInfo(void)
{
    UINT8    bID0 = 0;
	static UINT8 	SD_status = AHC_FALSE;    
#if (QUICK_FILE_EDIT_EN)
    static UINT8 	Lock_status = AHC_FALSE;
#endif

    OSDDraw_GetLastOvlDrawBuffer(&bID0);
	
	if(m_ubBatteryStatus != MenuSettingConfig()->uiBatteryVoltage)
	{
		UIDrawSlideFuncEx(bID0, SLIDE_GUI_BATTERY, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		m_ubBatteryStatus = MenuSettingConfig()->uiBatteryVoltage;
	}

    if(SD_status != AHC_SDMMC_GetMountState()) 
    {
    	UIDrawSlideFuncEx(bID0, SLIDE_GUI_SD_STATE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
        SD_status = AHC_SDMMC_GetMountState();
    }
	
#if (QUICK_FILE_EDIT_EN) 
 	if(Lock_status != bLockCurPlayFile)
	{
		UIDrawSlideFuncEx(bID0, SLIDE_GUI_LOCK_FILE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);	
		Lock_status = bLockCurPlayFile;
	}
#endif
	
#if	OSD_SHOW_BATTERY_STATUS == 1
	AHC_DrawChargingIcon_UsingTimer(bID0);
#endif

	if(PLAYBACK_VIDEO_TYPE == iSlideshowType)
		UIDrawMovPBFuncEx(bID0, MOVPB_GUI_CUR_TIME, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
	else if(PLAYBACK_AUDIO_TYPE == iSlideshowType)
		UIDrawAudPBFuncEx(bID0, AUDPB_GUI_CUR_TIME, AHC_FALSE, AHC_TRUE, OSD_COLOR_BLACK, NULL_PARAM);
}

void DrawStateSlideShowMode_Update(void)
{
	DrawStateSlideShowInit();
}
#endif

#if 0
void ____Event_Handler____(){ruturn;} //dummy
#endif

void DrawStateSlideShowUpdate(UINT32 ubEvent)
{
#if (SLIDESHOW_EN==1)

    switch(ubEvent)
    {
        case EVENT_NONE                           : 
        break; 
                                                     
        case EVENT_LCD_COVER_NORMAL               :
        case EVENT_LCD_COVER_ROTATE               :         
			AHC_OSDSetActive(OVL_BUFFER_INDEX, 0);
			AHC_OSDSetFlipDrawMode(OVL_BUFFER_INDEX, OSD_FLIP_DRAW_NONE_ENABLE);
			DrawStateSlideShowInit();
			AHC_OSDSetActive(OVL_BUFFER_INDEX, 1);
        break;
        
        case EVENT_SLIDESHOW_MODE_INIT            : 
			AHC_OSDSetActive(OVL_BUFFER_INDEX, 0);
			DrawStateSlideShowInit();
			AHC_OSDSetActive(OVL_BUFFER_INDEX, 1);
            #ifdef FLM_GPIO_NUM
            AHC_OSDRefresh_PLCD();
            #endif
        break;
                                            
        case EVENT_SLIDESHOW_MODE                 : 
			DrawStateSlideShowMode();             
        break;
                
        case EVENT_SLIDESHOW_TOUCH_PLAY_PRESS         :
        case EVENT_SLIDESHOW_TOUCH_PAUSE_PRESS        :
        	DrawStateSlideShowMovPbState();
        break;
                
        case EVENT_SLIDESHOW_UPDATE_MESSAGE       : 
			DrawStateSlideShowUpdateInfo();
        break;

        default:
        break;
    }
#endif    
}




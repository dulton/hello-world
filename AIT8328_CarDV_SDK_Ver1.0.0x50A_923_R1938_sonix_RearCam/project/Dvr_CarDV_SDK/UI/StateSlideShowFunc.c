/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "customer_config.h"
#include "AHC_Common.h"
#include "AHC_Parameter.h"
#include "AHC_General.h"
#include "AHC_Message.h"
#include "AHC_Utility.h"
#include "AHC_Display.h"
#include "AHC_Audio.h"
#include "AHC_Dcf.h"
#include "AHC_UF.h"
#include "AHC_Browser.h"
#include "AHC_Menu.h"
#include "AHC_Os.h"
#include "AHC_Media.h"
#include "AHC_Usb.h"
#include "AHC_Warningmsg.h"
#include "AHC_General_CarDV.h"
#include "MediaPlaybackCtrl.h"
#include "KeyParser.h"
#include "MenuDrawCommon.h"
#include "MenuSetting.h"
#include "MenuTouchButton.h"
#include "IconPosition.h"
#include "DrawStateSlideShowFunc.h"
#include "LedControl.h"
#include "StateTVFunc.h"
#include "StateHdmiFunc.h"
#include "StateSlideShowFunc.h"
#include "dsc_charger.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/ 

#define  STIMER_UNIT                                 (100)   //ms
#define  UNIT_TIME                                   (1000)
#define  SLIDESHOW_TIME                              (5*UNIT_TIME/STIMER_UNIT)
#define  SLIDEPB_OSD_SHOWTIME                        (8*UNIT_TIME/STIMER_UNIT)

/*===========================================================================
 * Global varible
 *===========================================================================*/ 

UINT8			ssFileType     			= 0;//PLAYBACK_TYPE_UNSUPPORTED;
UINT8			SlideTimerID 			= 0xFF;
UINT32			OSDShowCounter 			= 0;
UINT32			SlideShowCounter 		= 0;
UINT32			OSDUpdateCounter 		= 0;

AHC_BOOL 	    SlideOSDShowUp 			= AHC_TRUE;
AHC_BOOL		bSlideShow     			= AHC_FALSE;
AHC_BOOL		bSlideShowNext 			= AHC_FALSE;
AHC_BOOL		bFileFail 				= AHC_FALSE;
AHC_BOOL		bSlideShowFirstFile 	= AHC_TRUE;
UINT8			m_SlideDrawCounter  	= 0;
AHC_BOOL		bLockCurPlayFile		= AHC_FALSE;

#if (SUPPORT_TOUCH_PANEL)
AHC_BOOL		m_ubShowSlideTouchPage	= AHC_FALSE;
#endif

/*===========================================================================
 * Extern varible
 *===========================================================================*/ 

extern UINT32   	ObjSelect;
extern UINT32   	m_VideoCurPlayTime;
extern AHC_BOOL		bForce_PowerOff;
extern AHC_BOOL		bEnterMenuPause;
extern AHC_BOOL		bEnterPowerOffPagePause;
extern AHC_BOOL		bForceSwitchBrowser;

/*===========================================================================
 * Extern function
 *===========================================================================*/ 

extern void InitOSD(void);

/*===========================================================================
 * Main body
 *===========================================================================*/ 
  
void SlideShowTimer_ISR(void *tmr, void *arg)
{
#if (SLIDESHOW_EN==1)

    UINT32 ustatus;
  
    if( AHC_TRUE == bSlideShow )
    {
        SlideShowCounter++;

        if( 0 == SlideShowCounter%(SLIDESHOW_TIME) )
        {
        	if(bFileFail)
		    {
		        bSlideShowNext = AHC_TRUE;
                AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_UPDATE_FILE, 0);
                return;
            }
        
            if(PLAYBACK_IMAGE_TYPE == ssFileType)
            {
                bSlideShowNext = AHC_TRUE;
                AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_UPDATE_FILE, 0);
    			return;
    		}
    		else if(PLAYBACK_VIDEO_TYPE == ssFileType)
    		{
                if(MOV_STATE_PLAY == GetMovConfig()->iState)
                {
                    AHC_GetVideoPlayStopStatus(&ustatus);
                    
                    if(ustatus != AHC_VIDEO_PLAY)
                    {
                        GetMovConfig()->iState = MOV_STATE_STOP;
                        bSlideShowNext = AHC_TRUE;
                        AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_UPDATE_FILE, 0);
                    	return;
                    }
                }
            }
            else if(PLAYBACK_AUDIO_TYPE == ssFileType)
            {
                if(AUD_STATE_PLAY == GetAudConfig()->iState)
                {
                    AHC_GetAudioPlayStopStatus(&ustatus);
                    
                    if(ustatus != AHC_AUDIO_EVENT_PLAY)
                    {
                        GetAudConfig()->iState = AUD_STATE_STOP;
                        bSlideShowNext = AHC_TRUE;
                        AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_UPDATE_FILE, 0);
                    	return;
                    }
                }            
            }
        }
    }
    
    if(SlideOSDShowUp)
    {   
		#if !defined(CAR_DV)
        OSDShowCounter++;
        
        if( 0 == OSDShowCounter%(SLIDEPB_OSD_SHOWTIME) )
        {
            SlideOSDShowUp = AHC_FALSE;
            AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_OSD_HIDE, 0);
            return;
        }
        #endif

		if(PLAYBACK_IMAGE_TYPE == ssFileType)
		{
			OSDUpdateCounter++;
			
			if( 0 == OSDUpdateCounter%3 ){
                AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_UPDATE_MESSAGE, 0);  
                return;
            }
		}
		else if(PLAYBACK_VIDEO_TYPE == ssFileType)
		{
	        AHC_GetSystemStatus(&ustatus);
	             
	        if(STATE_MOV_EXECUTE == ustatus || STATE_MOV_PAUSE == ustatus)
	        {
	            OSDUpdateCounter++;
	            
	            if( 0 == OSDUpdateCounter%3 ){
	                AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_UPDATE_MESSAGE, 0);  
	                return;
	            }
	        }
		} 
		else if(PLAYBACK_AUDIO_TYPE == ssFileType)
		{
	        AHC_GetSystemStatus(&ustatus);
	             
	        if(STATE_AUD_PLAY == ustatus || STATE_AUD_PAUSE == ustatus)
	        {
	            OSDUpdateCounter++;
	            
	            if( 0 == OSDUpdateCounter%3 ){
	                AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_UPDATE_MESSAGE, 0);  
	                return;
	            }
	        }
		} 		
		  
    }
#endif      
}

AHC_BOOL SlideShowTimer_Start(UINT32 ulTime)
{
#if (SLIDESHOW_EN==1)

	if(SlideTimerID == 0xFF) 
	{
	    SlideShowCounter = 0;
	    OSDShowCounter 	 = 0;
	    OSDUpdateCounter = 0;
	    
	    SlideTimerID = AHC_OS_StartTimer( ulTime, AHC_OS_TMR_OPT_PERIODIC, SlideShowTimer_ISR, (void*)NULL );

	    if(0xFE <= SlideTimerID)
	        return AHC_FALSE;
	    else
	        return AHC_TRUE;
	}
	else {
        return AHC_FALSE;
	}
#else
	return AHC_TRUE;	
#endif	        
}

AHC_BOOL SlideShowTimer_Stop(void)
{
#if (SLIDESHOW_EN==1)

    UINT8 ret = 0;

	if(SlideTimerID < 0xFE) 
	{
	    ret = AHC_OS_StopTimer( SlideTimerID, AHC_OS_TMR_OPT_PERIODIC );

		SlideTimerID = 0xFF;

	    if(0xFF == ret)
	        return AHC_FALSE;
	    else
	        return AHC_TRUE;
	}
	else {
        return AHC_FALSE;
	}
#else
	return AHC_TRUE;	
#endif	 
}

AHC_BOOL SlideShowFunc_ConfigAttribute(void) 
{
#if (SLIDESHOW_EN==1)
#if (!UI_MODE_SELECT_EN)

    UINT32 MaxDcfObj = 0; 

    switch(MenuSettingConfig()->uiSlideShowFile)
    {
        case SLIDESHOW_FILE_ALL  :
        	MaxDcfObj = AHC_GetMappingFileNum(FILE_PHOTO_MOVIE);
        break;
        case SLIDESHOW_FILE_STILL:
        	MaxDcfObj = AHC_GetMappingFileNum(FILE_PHOTO);
        break;
        case SLIDESHOW_FILE_MOVIE:
        	MaxDcfObj = AHC_GetMappingFileNum(FILE_MOVIE);
        break;
        default                  :
        break;
    }
  
    if(MaxDcfObj == 0)
    {
    	AHC_WMSG_Draw(AHC_TRUE, WMSG_NO_FILE_IN_BROWSER, 2);
        return AHC_FALSE;
	}

    if(bSlideShowFirstFile) {
        AHC_UF_SetCurrentIndex(0);
    }
    else {
        if( ObjSelect > MaxDcfObj - 1)  
            ObjSelect = MaxDcfObj - 1;

        AHC_UF_SetCurrentIndex(ObjSelect);    
    }

    bSlideShowFirstFile = AHC_TRUE;
#endif
#endif    
	return AHC_TRUE;    
}

AHC_BOOL SlideShowFunc_Enable(AHC_BOOL bEnable)
{
#if (SLIDESHOW_EN==1)
    bSlideShow = bEnable;
#endif
    return AHC_TRUE;
}

void SlideShowFunc_ResetParam(void)
{
#if (SLIDESHOW_EN==1)

	switch(ssFileType)
	{
        case PLAYBACK_IMAGE_TYPE :
			JpgPlaybackParamReset();
        break;

        case PLAYBACK_VIDEO_TYPE :
			MovPlaybackParamReset();
        break;
        
        case PLAYBACK_AUDIO_TYPE :
        	AudPlaybackParamReset();
        break;
        
        default:
        break;
	}
#endif	
}

void SlideShowFunc_ResetFileType(void)
{
#if (SLIDESHOW_EN==1)
	ssFileType = PLAYBACK_TYPE_UNSUPPORTED;
#endif	
}

AHC_BOOL SlideShowFunc_Return(void)
{
#if (SLIDESHOW_EN==1)
    SlideShowTimer_Stop();
    SlideShowFunc_Stop();
    AHC_UF_GetCurrentIndex(&ObjSelect);
    SlideShowFunc_ResetFileType();
    return StateSwitchMode(UI_BROWSER_STATE);
#else
	return AHC_TRUE;    
#endif
}

AHC_BOOL SlideShowFunc_Stop(void)
{   
    MovPlaybackParamReset();
    JpgPlaybackParamReset();
    AudPlaybackParamReset();
	return AHC_TRUE;    
}

AHC_BOOL SlideShowFunc_Play(void)
{    
#if (SLIDESHOW_EN==1)

    AHC_BOOL     ahc_ret = AHC_TRUE; 
    static PLAYBACK_FILETYPE PreFileType = PLAYBACK_IMAGE_TYPE;

    ssFileType = GetPlayBackFileType();

    switch(ssFileType)
    {
        case PLAYBACK_IMAGE_TYPE :
             JpgPlaybackParamReset();
             if(PreFileType == PLAYBACK_VIDEO_TYPE) {
                AHC_SetDisplayMode(DISPLAY_MODE_PIP_DISABLE); //Rogers: Patch
             }
             ahc_ret = JpgPlayback_Play();
             PreFileType = PLAYBACK_IMAGE_TYPE;
        break;

        case PLAYBACK_VIDEO_TYPE :
             MovPlaybackParamReset();
             ahc_ret = MovPlayback_Play();
             PreFileType = PLAYBACK_VIDEO_TYPE;
        break;

        case PLAYBACK_AUDIO_TYPE :
             AudPlaybackParamReset();
             ahc_ret = AudPlayback_Play();
             PreFileType = PLAYBACK_AUDIO_TYPE;
        break;
        
        default:
        break;
    }

#if (QUICK_FILE_EDIT_EN)
	bLockCurPlayFile = AHC_CheckCurFileLockStatus();
#endif

    return ahc_ret;
#else
	return AHC_TRUE;
#endif
}

AHC_BOOL SlideShowFunc_Pause(void)
{
#if (SLIDESHOW_EN==1)
	switch(ssFileType)
	{
		case PLAYBACK_IMAGE_TYPE:
			SlideShowFunc_Enable(AHC_FALSE);
		break;
		case PLAYBACK_VIDEO_TYPE:
			MovPlayback_Pause();
		break;
		case PLAYBACK_AUDIO_TYPE:
			AudPlayback_Pause();
		break;	
	}
#endif
	return AHC_TRUE;
}

AHC_BOOL SlideShowFunc_Resume(void)
{
#if (SLIDESHOW_EN==1)
	switch(ssFileType)
	{
		case PLAYBACK_IMAGE_TYPE:
			SlideShowFunc_Enable(AHC_TRUE);
		break;
		case PLAYBACK_VIDEO_TYPE:
			MovPlayback_Resume();
		break;
		case PLAYBACK_AUDIO_TYPE:
			AudPlayback_Resume();
		break;	
	}
#endif
	return AHC_TRUE;
}

AHC_BOOL SlideShowFunc_Rotate(void)
{
#if (SLIDESHOW_EN==1)
	switch(ssFileType)
	{
		case PLAYBACK_IMAGE_TYPE:
			JpgPlayback_Rotate(MEDIAPB_ROTATE_CW);
		break;
		case PLAYBACK_VIDEO_TYPE:
			MovPlayback_Rotate(MEDIAPB_ROTATE_CW);
		break;
		case PLAYBACK_AUDIO_TYPE:
			//TBD
		break;	
	}
#endif
	return AHC_TRUE;
}

AHC_BOOL SlideShowFunc_Forward(void)
{
#if (SLIDESHOW_EN==1)
    AHC_BOOL     ahc_ret = AHC_TRUE;

    if(PLAYBACK_VIDEO_TYPE == ssFileType)
    {
        ahc_ret = MovPlayback_Forward();
    }
    else if(PLAYBACK_AUDIO_TYPE == ssFileType)
    {
        ahc_ret = AudPlayback_Forward();    
    }
    return ahc_ret;
#else
	return AHC_TRUE;
#endif
}

AHC_BOOL SlideShowFunc_Backward(void)
{
#if (SLIDESHOW_EN==1)
    AHC_BOOL     ahc_ret = AHC_TRUE;

    if(PLAYBACK_VIDEO_TYPE == ssFileType)
    {
        ahc_ret = MovPlayback_Backward();
    }
    else if(PLAYBACK_AUDIO_TYPE == ssFileType)
    {
        ahc_ret = AudPlayback_Backward();    
    }
    return ahc_ret;
#else
	return AHC_TRUE;
#endif    
}

void SlideShowFunc_SetOSDShowStatus(AHC_BOOL state)
{
#if (SLIDESHOW_EN==1)
    SlideOSDShowUp = state;
#endif
}

AHC_BOOL SlideShowFunc_GetOSDShowStatus(void)
{
#if (SLIDESHOW_EN==1)
    return SlideOSDShowUp;
#else
	return AHC_TRUE;
#endif	
}

AHC_BOOL SlideShowFunc_Start(void)
{
#if (SLIDESHOW_EN==1)

    AHC_BOOL ahc_ret = AHC_TRUE;
    
    AHC_SetMode(AHC_MODE_IDLE);

    if(SlideShowFunc_ConfigAttribute() == AHC_FALSE)
        return AHC_FALSE;
    
    SlideShowFunc_ResetParam();
    MediaPlaybackConfig(PLAYBACK_CURRENT);
    ahc_ret = SlideShowFunc_Play();

	if(AHC_FALSE == ahc_ret)
	{
        AHC_SetDisplayMode(DISPLAY_MODE_PIP_DISABLE);
    }
    else
    {
    	if(PLAYBACK_IMAGE_TYPE == ssFileType)
    	{
			#if (SUPPORT_TOUCH_PANEL)
        	KeyParser_ResetTouchVariable();
        	KeyParser_TouchItemRegister(&JpgPBMainPage_TouchButton[0], ITEMNUM(JpgPBMainPage_TouchButton));    
			#endif
		}
		else if(PLAYBACK_VIDEO_TYPE == ssFileType)
		{
			#if (SUPPORT_TOUCH_PANEL)
		    KeyParser_ResetTouchVariable();
		    KeyParser_TouchItemRegister(&MovPBMainPage_TouchButton[0], ITEMNUM(MovPBMainPage_TouchButton));    
			#endif  		
		}

		SlideOSDShowUp 		= AHC_TRUE;
		bSlideShow 			= AHC_TRUE;
		
		UpdateSlideMovPBStatus(MOVIE_STATE_PLAY);
		UpdateSlideAudPBStatus(AUDIO_STATE_PLAY);
	        
		SlideShowTimer_Start(STIMER_UNIT);
		DrawStateSlideShowUpdate(EVENT_SLIDESHOW_MODE_INIT);
	}
	
    return ahc_ret;
#else
	return AHC_TRUE;
#endif    
}

AHC_BOOL SlideShowFunc_CheckOSDShowUp(UINT32 ulEvent)
{
#if (SLIDESHOW_EN==1)
 
    AHC_BOOL state;
    
    if(ulEvent == EVENT_TV_DETECT 			|| ulEvent == EVENT_HDMI_DETECT 		|| ulEvent == EVENT_SLIDESHOW_UPDATE_FILE || 
       ulEvent == EVENT_LCD_COVER_ROTATE 	|| ulEvent == EVENT_LCD_COVER_NORMAL 	)
    {
        return AHC_TRUE;
    } 
    
    if(ulEvent == EVENT_SLIDESHOW_OSD_HIDE) 
    {
		#if !defined(CAR_DV)    
        AHC_OSDSetActive(OVL_BUFFER_INDEX, 0);
        AHC_OSDSetActive(OVL_BUFFER_INDEX1, 0);
		#endif
   	    return AHC_FALSE;
    }
    
    state = SlideShowFunc_GetOSDShowStatus();

	if(state) 
	{     
   		return AHC_TRUE;
   	}	
   	else 
   	{
   		if(ulEvent == EVENT_SLIDESHOW_UPDATE_MESSAGE) {
            return AHC_FALSE;
   	    }
   	
   	    SlideShowFunc_SetOSDShowStatus(AHC_TRUE);
   	    AHC_OSDSetActive(OVL_BUFFER_INDEX, 1);
   	    AHC_OSDSetActive(OVL_BUFFER_INDEX1, 1);
   	    DrawStateSlideShowUpdate(EVENT_SLIDESHOW_MODE_INIT);
   	    DrawStateSlideShowUpdate(EVENT_SLIDESHOW_TOUCH_NEXT_PRESS);
   	    return AHC_FALSE;
   	}    
#endif   
    return AHC_TRUE;
}

void SlideShowMode_Update(void)
{
#if (SLIDESHOW_EN==1)

	UINT32	MaxDcfObj;

	switch( MenuSettingConfig()->uiSlideShowFile )
	{
        case SLIDESHOW_FILE_ALL  :
        	MaxDcfObj = AHC_GetMappingFileNum(FILE_PHOTO_MOVIE);
        break;
        case SLIDESHOW_FILE_STILL:
        	MaxDcfObj = AHC_GetMappingFileNum(FILE_PHOTO);
        break;
        case SLIDESHOW_FILE_MOVIE:
        	MaxDcfObj = AHC_GetMappingFileNum(FILE_MOVIE);
        break;
        default                  :
        break;
	}

    if(MaxDcfObj == 0)
    {
		SlideShowFunc_Return();
    	return;
	}

	if(PLAYBACK_IMAGE_TYPE == ssFileType)
	{
		bSlideShow = AHC_TRUE;

		#if (SUPPORT_TOUCH_PANEL)
		KeyParser_ResetTouchVariable();
		KeyParser_TouchItemRegister(&JpgPBMainPage_TouchButton[0], ITEMNUM(JpgPBMainPage_TouchButton));    
		#endif
	}
	else if(PLAYBACK_VIDEO_TYPE == ssFileType)
	{
		if(GetMovConfig()->iState == MOV_STATE_PAUSE && bEnterMenuPause==AHC_TRUE)
    	{    
    		MovPlayback_Resume();	
    		bEnterMenuPause = AHC_FALSE;
    	}
		#if (SUPPORT_TOUCH_PANEL)
	    KeyParser_ResetTouchVariable();
	    KeyParser_TouchItemRegister(&MovPBMainPage_TouchButton[0], ITEMNUM(MovPBMainPage_TouchButton));    
		#endif    	
	}
	else if(PLAYBACK_AUDIO_TYPE == ssFileType)
	{
		if(GetAudConfig()->iState == AUD_STATE_PAUSE && bEnterMenuPause==AHC_TRUE)
    	{    
    		AudPlayback_Resume();	
    		bEnterMenuPause = AHC_FALSE;
    	}  	
	}	
	
#if (QUICK_FILE_EDIT_EN)
	bLockCurPlayFile = AHC_CheckCurFileLockStatus();
#endif

	DrawStateSlideShowMode_Update();
#endif	
}

void SlideShowPhoto_SDMMC_Out(void)
{
	AHC_SetMode(AHC_MODE_IDLE);

	SlideShowTimer_Stop();
	SlideShowFunc_ResetParam();
	AHC_UF_GetCurrentIndex(&ObjSelect);
}

void SlideShowVideo_SDMMC_Out(void)
{
	if(MOV_STATE_FF == GetMovConfig()->iState)
	{	
		MovPlayback_NormalSpeed();
		AHC_OS_Sleep(100);
	}
             	
	AHC_SetMode( AHC_MODE_IDLE );
        	 	
	SlideShowTimer_Stop();
	SlideShowFunc_ResetParam();
	AHC_UF_GetCurrentIndex(&ObjSelect);
}

void SlideShowAudio_SDMMC_Out(void)
{
	if(AUD_STATE_FF == GetAudConfig()->iState)
	{	
		AudPlayback_NormalSpeed();
		AHC_OS_Sleep(100);
	}
             	
	AHC_SetMode(AHC_MODE_IDLE);
        	 	
	SlideShowTimer_Stop();
	SlideShowFunc_ResetParam();
	AHC_UF_GetCurrentIndex(&ObjSelect);
}

void SlideShowMode_SDMMC_In(void)
{				
	CHARGE_ICON_ENABLE(AHC_FALSE);	
	
	bForceSwitchBrowser = AHC_TRUE;
	AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect);
						
	CHARGE_ICON_ENABLE(AHC_TRUE);	
}

void SlideShowMode_SDMMC_Out(void)
{
	CHARGE_ICON_ENABLE(AHC_FALSE);
			
	if(PLAYBACK_VIDEO_TYPE == ssFileType)
		SlideShowVideo_SDMMC_Out();
	else if	(PLAYBACK_IMAGE_TYPE == ssFileType)
		SlideShowPhoto_SDMMC_Out();
	else if(PLAYBACK_AUDIO_TYPE == ssFileType)	
		SlideShowAudio_SDMMC_Out();
			
	bForceSwitchBrowser = AHC_TRUE; 
    
    if(AHC_TRUE == AHC_SDMMC_GetMountState())
        AHC_DisMountStorageMedia(AHC_MEDIA_MMC);
				
	CHARGE_ICON_ENABLE(AHC_TRUE);
}

//******************************************************************************
//
//                              AHC State SlideShow Mode
//
//******************************************************************************

void StateSlideShowMode(UINT32 ulEvent)
{
#if (SLIDESHOW_EN==1)

    UINT32 		ulJobEvent = ulEvent;
    UINT32 		uState;
    AHC_BOOL 	ret = AHC_TRUE;
	
    if(!SlideShowFunc_CheckOSDShowUp(ulEvent))
        return;  

    switch(ulJobEvent)
    {
        case EVENT_NONE                           : 
        break; 
                                                                                                                                                                   
        case EVENT_SLIDESHOW_TOUCH_PREV_PRESS     : 
			
			if(PLAYBACK_VIDEO_TYPE == ssFileType)
			{
				UpdateSlideMovPBStatus(MOVIE_STATE_PLAY);
				MovPlayback_Pause();	 
			}
			else if(PLAYBACK_AUDIO_TYPE == ssFileType)
			{
				UpdateSlideAudPBStatus(AUDIO_STATE_PLAY);
				AudPlayback_Pause();	 
			}			
			
			SlideShowCounter = 0;
			MediaPlaybackConfig(PLAYBACK_PREVIOUS);
			SlideShowFunc_Play();		 
			DrawStateSlideShowUpdate(EVENT_SLIDESHOW_MODE);
        break; 
                                             
        case EVENT_SLIDESHOW_TOUCH_NEXT_PRESS     : 
			
			if(PLAYBACK_VIDEO_TYPE == ssFileType)
			{
				UpdateSlideMovPBStatus(MOVIE_STATE_PLAY);
				MovPlayback_Pause();	 
			}
			else if(PLAYBACK_AUDIO_TYPE == ssFileType)
			{
				UpdateSlideAudPBStatus(AUDIO_STATE_PLAY);
				AudPlayback_Pause();	 
			}
			
			SlideShowCounter = 0;
			MediaPlaybackConfig(PLAYBACK_NEXT);
			SlideShowFunc_Play();
			DrawStateSlideShowUpdate(EVENT_SLIDESHOW_MODE);
        break;                                      
 		
 		case EVENT_SLIDESHOW_TOUCH_PLAY_PRESS         :

			if(PLAYBACK_IMAGE_TYPE == ssFileType)
			{
				bSlideShow = (bSlideShow)?(AHC_FALSE):(AHC_TRUE);
				#if (SUPPORT_TOUCH_PANEL)
				DrawStateSlideShowPageSwitch(TOUCH_CTRL_PAGE);
				#endif
			}
			else if(PLAYBACK_VIDEO_TYPE == ssFileType)
			{
				AHC_GetSystemStatus(&uState);
				
				switch(uState)
				{
					case STATE_MOV_IDLE    :
						UpdateSlideMovPBStatus(MOVIE_STATE_PLAY);
						MediaPlaybackConfig(PLAYBACK_CURRENT);
						MovPlayback_Play();
						DrawStateSlideShowUpdate(EVENT_SLIDESHOW_MODE);
					break;
                     
					case STATE_MOV_EXECUTE :
						if(MOV_STATE_FF == GetMovConfig()->iState)
						{
							UpdateSlideMovPBStatus(MOVIE_STATE_PLAY);
							MovPlayback_NormalSpeed();
							DrawStateSlideShowUpdate(EVENT_SLIDESHOW_TOUCH_PLAY_PRESS);
						}
						else
						{
							UpdateSlideMovPBStatus(MOVIE_STATE_PAUSE);
							MovPlayback_Pause();
							DrawStateSlideShowUpdate(EVENT_SLIDESHOW_TOUCH_PAUSE_PRESS);
						}
					break;
                     
					case STATE_MOV_PAUSE   :
						UpdateSlideMovPBStatus(MOVIE_STATE_PLAY);
						MovPlayback_Resume();
						DrawStateSlideShowUpdate(EVENT_SLIDESHOW_TOUCH_PLAY_PRESS);
					break;
					
					default                :
					break;
				}
			}
			else if(PLAYBACK_AUDIO_TYPE == ssFileType)
			{
				AHC_GetSystemStatus(&uState);
				
				switch(uState)
				{
					case STATE_AUD_STOP    :
						UpdateSlideAudPBStatus(AUDIO_STATE_PLAY);
						MediaPlaybackConfig(PLAYBACK_CURRENT);
						AudPlayback_Play();
						DrawStateSlideShowUpdate(EVENT_SLIDESHOW_MODE);
					break;
                     
					case STATE_AUD_PLAY :
						if(AUD_STATE_FF == GetAudConfig()->iState)
						{
							UpdateSlideAudPBStatus(AUDIO_STATE_PLAY);
							AudPlayback_NormalSpeed();
							DrawStateSlideShowUpdate(EVENT_SLIDESHOW_TOUCH_PLAY_PRESS);
						}
						else
						{
							UpdateSlideAudPBStatus(AUDIO_STATE_PAUSE);
							AudPlayback_Pause();
							DrawStateSlideShowUpdate(EVENT_SLIDESHOW_TOUCH_PAUSE_PRESS);
						}
					break;
                     
					case STATE_AUD_PAUSE   :
						UpdateSlideAudPBStatus(AUDIO_STATE_PLAY);
						AudPlayback_Resume();
						DrawStateSlideShowUpdate(EVENT_SLIDESHOW_TOUCH_PLAY_PRESS);
					break;
					
					default                :
					break;
				}
			}			
        break;
 	
        case EVENT_SLIDESHOW_TOUCH_RETURN              : 
			SlideShowFunc_Return();
        break;  
                          
		case EVENT_KEY_MODE:
#if (UI_MODE_SELECT_EN)	
			if(PLAYBACK_IMAGE_TYPE == ssFileType)
			{
				bSlideShow = AHC_FALSE;
			}
			else if(PLAYBACK_VIDEO_TYPE == ssFileType)
			{
				if(GetMovConfig()->iState == MOV_STATE_PLAY || GetMovConfig()->iState == MOV_STATE_FF)
        		{
        			MovPlayback_Pause();
        			bEnterMenuPause = AHC_TRUE;	
        		}
        		else if(GetMovConfig()->iState == MOV_STATE_PAUSE)
        		{
        			bEnterMenuPause = AHC_FALSE;	
        		}
        	}
			else if(PLAYBACK_AUDIO_TYPE == ssFileType  )
			{
				if(GetAudConfig()->iState == AUD_STATE_PLAY || GetAudConfig()->iState == AUD_STATE_FF)
        		{
        			AudPlayback_Pause();
        			bEnterMenuPause = AHC_TRUE;	
        		}
        		else if(GetAudConfig()->iState == AUD_STATE_PAUSE)
        		{
        			bEnterMenuPause = AHC_FALSE;
        		}
        	}        	
			StateSwitchMode(UI_MODESELECT_MENU_STATE);
#endif		
		break;

		case EVENT_KEY_MENU:
		
			if(PLAYBACK_IMAGE_TYPE == ssFileType)
			{
				bSlideShow = AHC_FALSE;
			}
			else if(PLAYBACK_VIDEO_TYPE == ssFileType)
			{
				if(GetMovConfig()->iState == MOV_STATE_PLAY || GetMovConfig()->iState == MOV_STATE_FF)
        		{
        			MovPlayback_Pause();
        			bEnterMenuPause = AHC_TRUE;	
        		}
        		else if(GetMovConfig()->iState == MOV_STATE_PAUSE)
        		{
        			bEnterMenuPause = AHC_FALSE;
        		}
        	}
			else if(PLAYBACK_AUDIO_TYPE == ssFileType)
			{
				if(GetAudConfig()->iState == AUD_STATE_PLAY || GetAudConfig()->iState == AUD_STATE_FF)
        		{
        			AudPlayback_Pause();
        			bEnterMenuPause = AHC_TRUE;
        		}
        		else if(GetAudConfig()->iState == AUD_STATE_PAUSE)
        		{
        			bEnterMenuPause = AHC_FALSE;
        		}
        	}
        	
			StateSwitchMode(UI_PLAYBACK_MENU_STATE);
		break;
                          
        case EVENT_POWER_OFF                      :
            
            if(PLAYBACK_VIDEO_TYPE == ssFileType)
            {         
				if(GetMovConfig()->iState == MOV_STATE_PLAY || GetMovConfig()->iState == MOV_STATE_FF)
	        	{
	        		MovPlayback_Pause();
	        		bEnterPowerOffPagePause = AHC_TRUE;
	        	}
	        	else if(GetMovConfig()->iState == MOV_STATE_PAUSE)
	        	{
	        		bEnterPowerOffPagePause = AHC_FALSE;
	        	}
            }
            else if(PLAYBACK_AUDIO_TYPE == ssFileType)
            {         
				if(GetAudConfig()->iState == AUD_STATE_PLAY || GetAudConfig()->iState == AUD_STATE_FF)
	        	{
	        		AudPlayback_Pause();
	        		bEnterPowerOffPagePause = AHC_TRUE;
	        	}
	        	else if(GetAudConfig()->iState == AUD_STATE_PAUSE)
	        	{
	        		bEnterPowerOffPagePause = AHC_FALSE;
	        	}
            }
                         
			AHC_NormalPowerOffPath();
        break;
        
		//Special Event
        case EVENT_SLIDESHOW_TOUCH_BKWD_PRESS         :
			UpdateSlideMovPBStatus(MOVIE_STATE_REW);
			SlideShowFunc_Backward();
        break;

        case EVENT_SLIDESHOW_TOUCH_FRWD_PRESS         :
			UpdateSlideMovPBStatus(MOVIE_STATE_FF);
			SlideShowFunc_Forward();
        break;

		case EVENT_LCD_POWER_SAVE 			:
			AHC_SwitchLCDBackLight();
		break;

		case EVENT_SWITCH_PANEL_TVOUT:
			AHC_SwitchLCDandTVOUT();
		break;
		
	#ifdef NET_SYNC_PLAYBACK_MODE
		case EVENT_NET_ENTER_PLAYBACK : 
			SlideShowTimer_Stop();
			SlideShowFunc_Stop();
			AHC_UF_GetCurrentIndex(&ObjSelect);
			SlideShowFunc_ResetFileType();
            StateSwitchMode(UI_NET_PLAYBACK_STATE);
            CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
        break;
    #endif//NET_SYNC_PLAYBACK_MODE
    
#if (SUPPORT_TOUCH_PANEL)
		case EVENT_SWITCH_TOUCH_PAGE:
			m_ubShowSlideTouchPage ^= 1;
			
			if(m_ubShowSlideTouchPage)
			{
		    	if(PLAYBACK_IMAGE_TYPE == ssFileType)
		    	{
		        	KeyParser_ResetTouchVariable();
		        	KeyParser_TouchItemRegister(&JpgPBCtrlPage_TouchButton[0], ITEMNUM(JpgPBCtrlPage_TouchButton));
				}
				else if(PLAYBACK_VIDEO_TYPE == ssFileType)
				{
				    KeyParser_ResetTouchVariable();
				    KeyParser_TouchItemRegister(&MovPBCtrlPage_TouchButton[0], ITEMNUM(MovPBCtrlPage_TouchButton));
				}
				DrawStateSlideShowPageSwitch(TOUCH_CTRL_PAGE);
			}
			else
			{
		    	if(PLAYBACK_IMAGE_TYPE == ssFileType)
		    	{
		        	KeyParser_ResetTouchVariable();
		        	KeyParser_TouchItemRegister(&JpgPBMainPage_TouchButton[0], ITEMNUM(JpgPBMainPage_TouchButton));
				}
				else if(PLAYBACK_VIDEO_TYPE == ssFileType)
				{
				    KeyParser_ResetTouchVariable();
				    KeyParser_TouchItemRegister(&MovPBMainPage_TouchButton[0], ITEMNUM(MovPBMainPage_TouchButton));
				}
				DrawStateSlideShowPageSwitch(TOUCH_MAIN_PAGE);
			}
		break;
#endif
        
        //Device
		case EVENT_DC_CABLE_IN:
			#if (CHARGER_IN_ACT_OTHER_MODE==ACT_RECORD_VIDEO)
			SlideShowTimer_Stop();
			SlideShowFunc_Stop();
			AHC_UF_GetCurrentIndex(&ObjSelect);
			SlideShowFunc_ResetFileType();
			ret = StateSwitchMode(UI_VIDEO_STATE);
			AHC_SetRecordByChargerIn(3);
			#elif (CHARGER_IN_ACT_OTHER_MODE == ACT_FORCE_POWER_OFF)
			SetKeyPadEvent(KEY_POWER_OFF);
			#endif
		break;
		
		case EVENT_DC_CABLE_OUT:
			AHC_SetChargerEnable(AHC_FALSE);
			#if (CHARGER_OUT_ACT_OTHER_MODE==ACT_FORCE_POWER_OFF || CHARGER_OUT_ACT_OTHER_MODE==ACT_DELAY_POWER_OFF)
			AHC_SetShutdownByChargerOut(AHC_TRUE);
			#endif
		break;

        case EVENT_USB_DETECT                     :
			if(IsAdapter_Connect())
			{
				//EnterChargerMode(1 /* enable USB int to wait USB int for later */);
		     	#if (CHARGER_IN_ACT_OTHER_MODE==ACT_RECORD_VIDEO)
				SlideShowTimer_Stop();
				SlideShowFunc_Stop();
				AHC_UF_GetCurrentIndex(&ObjSelect);
				SlideShowFunc_ResetFileType();
				ret = StateSwitchMode(UI_VIDEO_STATE);
				AHC_SetRecordByChargerIn(3);
				#elif (CHARGER_IN_ACT_OTHER_MODE == ACT_FORCE_POWER_OFF)
				SetKeyPadEvent(KEY_POWER_OFF);
				#endif	
			}
			else
			{
             	SlideShowTimer_Stop();
                SlideShowFunc_Stop();
             	AHC_OSDUninit();
	            AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
    	        InitOSD();
             	#if (USB_MODE_SELECT_EN)
				if (MMPS_USB_NORMAL == MMPS_USB_GetStatus()) {
				    StateSwitchMode(UI_USBSELECT_MENU_STATE);
				}
				else
				{
					#ifdef CFG_POWER_ON_ENTER_CAMERA_STATE
					StateSwitchMode(UI_CAMERA_STATE);
					#else
					StateSwitchMode(UI_VIDEO_STATE);
					#endif
				}
				#else
				if(MenuSettingConfig()->uiUSBFunction == USB_MSDC)
				{
                	ret = StateSwitchMode(UI_MSDC_STATE);
				}
				else
				{
					ret = StateSwitchMode(UI_PCCAM_STATE);
				}
				#endif
			}
        break;

        case EVENT_USB_REMOVED                    :
	         
			if(AHC_USB_GetLastStatus() == AHC_USB_NORMAL) // MSDC mode
			{
				AHC_SetMode(AHC_MODE_IDLE);
        	 	bForce_PowerOff = AHC_TRUE;
        	 	AHC_NormalPowerOffPath();
			}
			else
			{
                if (AHC_IsDcCableConnect() == AHC_TRUE)
                    break;

		     	AHC_SetChargerEnable(AHC_FALSE);
		     	#if (CHARGER_OUT_ACT_OTHER_MODE==ACT_FORCE_POWER_OFF || CHARGER_OUT_ACT_OTHER_MODE==ACT_DELAY_POWER_OFF)
		     	AHC_SetShutdownByChargerOut(AHC_TRUE);
		     	#endif
			}
        break;
        
        case EVENT_SD_DETECT                      :
			AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect);
        break;
        
  		case EVENT_SD_REMOVED                      :
  			 
            if(ssFileType == PLAYBACK_VIDEO_TYPE)
            {
                if(MOV_STATE_FF == GetMovConfig()->iState)
                {	
                    MovPlayback_NormalSpeed();
                    AHC_OS_Sleep(100);
                }
				
				AHC_ResetVideoPlayParam();
				
                AHC_SetMode(AHC_MODE_IDLE);
            }	
            else if(ssFileType == PLAYBACK_AUDIO_TYPE)
            {
                if(AUD_STATE_FF == GetAudConfig()->iState)
                {	
                    AudPlayback_NormalSpeed();
                    AHC_OS_Sleep(100);
                }

                AHC_SetMode(AHC_MODE_IDLE);
            }

            SlideShowTimer_Stop();
            SlideShowFunc_Stop();
            AHC_UF_GetCurrentIndex(&ObjSelect);

            if(AHC_TRUE == AHC_SDMMC_GetMountState())
            {
                AHC_DisMountStorageMedia(AHC_MEDIA_MMC);
                AHC_OS_Sleep(100);
				Enable_SD_Power(0 /* Power Off */);
            }
            
            SlideShowFunc_ResetFileType();
            StateSwitchMode(UI_BROWSER_STATE);
            AHC_OSDSetActive(OVL_DISPLAY_BUFFER, 1);
        break;

       	case EVENT_TV_DETECT                      : 
			if(AHC_IsTVConnectEx()) {
				AHC_GetVideoCurrentPlayTime(&m_VideoCurPlayTime);
				SlideShowTimer_Stop();
				SlideShowFunc_Stop();
				AHC_SetVideoPlayStartTime(m_VideoCurPlayTime);
				SetTVState(AHC_TV_SLIDE_SHOW_STATUS);
				StateSwitchMode(UI_TVOUT_STATE);
			}
        break;  
        
        case EVENT_TV_REMOVED                     :
        break;
        
        case EVENT_HDMI_DETECT                    : 
			if(AHC_IsHdmiConnect()) {
				AHC_GetVideoCurrentPlayTime(&m_VideoCurPlayTime);
				SlideShowTimer_Stop();
				SlideShowFunc_Stop();
				AHC_SetVideoPlayStartTime(m_VideoCurPlayTime);
				SetHDMIState(AHC_HDMI_SLIDE_SHOW_STATUS);
				StateSwitchMode(UI_HDMI_STATE);
			}
        break;

        case EVENT_HDMI_REMOVED                     :
        break;
        
        case EVENT_LCD_COVER_OPEN                  :
			LedCtrl_LcdBackLight(AHC_TRUE);
        break;
            
        case EVENT_LCD_COVER_CLOSE                :
			LedCtrl_LcdBackLight(AHC_FALSE);
        break;    
            
        case EVENT_LCD_COVER_NORMAL               :
        case EVENT_LCD_COVER_ROTATE               :
        	SlideShowFunc_SetOSDShowStatus(AHC_TRUE);
        	AHC_DrawRotateEvent(ulJobEvent);
        break;

		//Update Message
        case EVENT_SLIDESHOW_MODE_INIT                :
			DrawStateSlideShowUpdate(ulJobEvent);
        break;      
                                        
        case EVENT_SLIDESHOW_MODE                     :
			DrawStateSlideShowUpdate(ulJobEvent);
        break;
                                                 
        case EVENT_SLIDESHOW_UPDATE_FILE           :
                     
			if(AHC_TRUE == bSlideShowNext)
			{
				bSlideShow = AHC_FALSE;
				SlideShowCounter = 0;
				MediaPlaybackConfig(PLAYBACK_NEXT);
				ret = SlideShowFunc_Play();

				if(ret == AHC_FALSE) {
					bFileFail = AHC_TRUE;
                    bSlideShow = AHC_TRUE;
                    return;
				}
				else {
                    bFileFail = AHC_FALSE;
				}

				if(AHC_TRUE == SlideOSDShowUp)
				{
                     DrawStateSlideShowUpdate(EVENT_SLIDESHOW_MODE);
				}

				bSlideShowNext = AHC_FALSE;
				bSlideShow = AHC_TRUE;
			}
        break;
              
        case EVENT_SLIDESHOW_UPDATE_MESSAGE           :
			StateLedControl(UI_PLAYBACK_STATE);
			 
			if(AHC_TRUE == SlideOSDShowUp)
			{
				DrawStateSlideShowUpdate(EVENT_SLIDESHOW_UPDATE_MESSAGE);
			}
        break;
        
        default:
        break;
    }
#endif
}



/*===========================================================================
 * Include file 
 *===========================================================================*/ 
 
#include "AHC_Common.h"
#include "AHC_Parameter.h"
#include "AHC_General.h"
#include "AHC_Message.h"
#include "AHC_Utility.h"
#include "AHC_Audio.h"
#include "AHC_Dcf.h"
#include "AHC_UF.h"
#include "AHC_Display.h"
#include "AHC_Browser.h"
#include "AHC_Menu.h"
#include "AHC_Media.h"
#include "AHC_Os.h"
#include "AHC_USB.h"
#include "AHC_General_CarDV.h"
#include "MediaPlaybackCtrl.h"
#include "MenuSetting.h"
#include "MenuDrawCommon.h"
#include "KeyParser.h"
#include "StateMoviePBFunc.h"
#include "StateHDMIFunc.h"
#include "StateTVFunc.h"
#include "DrawStateMoviePBFunc.h"
#include "IconPosition.h"
#include "LedControl.h"
#include "dsc_charger.h"

/*===========================================================================
 * Macro define 
 *===========================================================================*/ 

#define AUDPB_TIMER_UNIT                           (200)  //unit :sec.
#define AUDPB_MAX_SPEED								 (8)
#define AUDPB_MIN_SPEED								 (2)

/*===========================================================================
 * Global varible
 *===========================================================================*/ 

UINT8                      	AudPBTimerID 			= 0xFF;
UINT32                     	AudPbCounter 			= 0;
UINT32                     	m_AudioCurPlayTime 		= 0;
UINT8 						m_AudDrawCounter 		= 0;
AHC_BOOL					bLockCurAudFile			= AHC_FALSE;
UINT32						m_AudPlayFFSpeed		= AUDPB_MAX_SPEED;
UINT32						m_AudPlayREWSpeed		= AUDPB_MIN_SPEED;
UINT8 						bAudioState 			= AUDIO_STATE_PAUSE;

/*===========================================================================
 * Extern varible
 *===========================================================================*/

extern AHC_BOOL				bEnterMenuPause;
extern AHC_BOOL 			bEnterPowerOffPagePause;
extern UINT32   		   	ObjSelect;
extern AHC_BOOL			   	bForce_PowerOff;
extern AHC_BOOL				bForceSwitchBrowser;

/*===========================================================================
 * Extern function
 *===========================================================================*/

extern void InitOSD(void);

/*===========================================================================
 * Main body
 *===========================================================================*/

void AudPBTimer_ResetCounter(void)
{
    AudPbCounter = 0;
}
 
void AudPBTimer_ISR(void *tmr, void *arg)
{
    UINT32 	ustatus;

    if(	AUD_STATE_PLAY 	== GetAudConfig()->iState || 
    	AUD_STATE_FF 	== GetAudConfig()->iState || 
    	AUD_STATE_PAUSE == GetAudConfig()->iState)
    {
        AHC_GetAudioPlayStopStatus(&ustatus);
               
        if(ustatus == AHC_AUDIO_EVENT_EOF)
        {
            GetAudConfig()->iState = AUD_STATE_STOP;
            AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_UPDATE_MESSAGE, 0);
            return;
        }
        else
        {
            AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_UPDATE_MESSAGE, 0);
        }
    }  
}

AHC_BOOL AudPBTimer_Start(UINT32 ulTime)
{
	if(AudPBTimerID == 0xFF) 
	{
	    AudPBTimer_ResetCounter();
	    AudPBTimerID = AHC_OS_StartTimer( ulTime, AHC_OS_TMR_OPT_PERIODIC, AudPBTimer_ISR, (void*)NULL );

	    if(0xFE <= AudPBTimerID)
			return AHC_FALSE;
		else
			return AHC_TRUE;
	}
	else {
        return AHC_FALSE;
	}
}

AHC_BOOL AudPBTimer_Stop(void)
{
    UINT8 ret = 0;

    if(AudPBTimerID < 0xFE) 
    {
        ret = AHC_OS_StopTimer( AudPBTimerID, AHC_OS_TMR_OPT_PERIODIC );

        AudPBTimerID = 0xFF;
        
        if(0xFF == ret)
     		return AHC_FALSE;
    	else
    		return AHC_TRUE;
	}
	else {
        return AHC_FALSE;
	}
}

void AudPBFunc_ResetPlaySpeed(void)
{
	m_AudPlayFFSpeed	= 1;//AUDPB_MIN_SPEED;
	m_AudPlayREWSpeed	= 1;//AUDPB_MIN_SPEED;
}

void AudPBFunc_SDMMC_Out(void)
{             	
	AHC_SetMode(AHC_MODE_IDLE);
        	 	
	AudPBTimer_Stop();
	AudPlaybackParamReset();
	AHC_UF_GetCurrentIndex(&ObjSelect);
}

AHC_BOOL AudioPBMode_Start(void)
{
#if (AUDIO_REC_PLAY_EN)

    AHC_BOOL ahc_ret = AHC_TRUE;

    AHC_SetMode(AHC_MODE_IDLE);
    
    AudPlaybackParamReset();

    MediaPlaybackConfig(PLAYBACK_CURRENT);
        
    ahc_ret = AudPlayback_Play();

	AudPBFunc_ResetPlaySpeed();
	
#if (QUICK_FILE_EDIT_EN)
	bLockCurAudFile = AHC_CheckCurFileLockStatus();
#endif

    if(AHC_TRUE == ahc_ret)
    {
    	UpdateAudPBStatus(AUDIO_STATE_PLAY);
        AudPBTimer_Start(AUDPB_TIMER_UNIT);
        DrawStateAudPlaybackUpdate(EVENT_MOVPB_MODE_INIT);
    }
    
    return ahc_ret;
#else
	return AHC_TRUE;
#endif    
}

void AudioPBMode_Update(void)
{
#if (AUDIO_REC_PLAY_EN)

	UINT32	MaxObjIdx;

    AHC_UF_GetTotalFileCount(&MaxObjIdx);

    if(MaxObjIdx == 0)
    {
    	AudPBTimer_Stop();
		AudPlaybackParamReset();
		AHC_UF_GetCurrentIndex(&ObjSelect);
		StateSwitchMode(UI_BROWSER_STATE);	
    	return;
	}
	
	if(GetAudConfig()->iState == AUD_STATE_PAUSE && bEnterMenuPause==AHC_TRUE)
    {    
    	AudPlayback_Resume();	
    	bEnterMenuPause = AHC_FALSE;
    }

#if (QUICK_FILE_EDIT_EN)
	bLockCurAudFile = AHC_CheckCurFileLockStatus();
#endif

	DrawStateAudPlaybackUpdate(EVENT_MENU_RETURN);
#endif	
}

//******************************************************************************
//
//                              AHC State Audio Mode
//
//******************************************************************************

void StateAudioPlaybackMode(UINT32 ulEvent)
{
#if (AUDIO_REC_PLAY_EN)

    UINT32 		ulJobEvent 	= ulEvent;
    UINT32 		uState;
    AHC_BOOL 	ret 		= AHC_TRUE;

	if(ulJobEvent == EVENT_NONE)
	    return;

    if(EVENT_MOVPB_UPDATE_MESSAGE != ulJobEvent)
        AudPBTimer_ResetCounter();

	AHC_GetSystemStatus(&uState);
	
    switch(ulJobEvent)
    {
        case EVENT_NONE                           : 
        break; 
        
        case EVENT_KEY_UP	:
			AHC_ChangeVolume(AHC_FALSE, 1);
			DrawStateAudPlaybackUpdate(EVENT_KEY_TELE_PRESS);
        break;
     
        case EVENT_KEY_DOWN	:
			AHC_ChangeVolume(AHC_FALSE, 0);
        	DrawStateAudPlaybackUpdate(EVENT_KEY_WIDE_PRESS);
        break;

        case EVENT_KEY_WIDE_STOP : //Clear VolumeBar
        case EVENT_KEY_TELE_STOP :
			DrawStateAudPlaybackUpdate(EVENT_KEY_WIDE_STOP);
        break;

        case EVENT_MOVPB_TOUCH_BKWD_PRESS         : 
        	#if 0
        	if(bAudioState!=AUDIO_STATE_FF || m_AudPlayREWSpeed==1)
        	{	
         		m_AudPlayREWSpeed = ( (m_AudPlayREWSpeed<<1) > AUDPB_MAX_SPEED )?(AUDPB_MIN_SPEED):(m_AudPlayREWSpeed<<1); 	
        	}
			AHC_SetPlaySpeed(AHC_PB_MOVIE_BACKWARD_RATE, m_AudPlayREWSpeed+1); 
        	#endif
        		
        	UpdateAudPBStatus(AUDIO_STATE_REW);  
			AudPBTimer_Start(AUDPB_TIMER_UNIT);
			AudPlayback_Backward();
			DrawStateAudPlaybackUpdate(EVENT_MOVPB_UPDATE_MESSAGE);
			DrawStateAudPlaybackUpdate(EVENT_MOVPB_TOUCH_BKWD_PRESS);
        break;
		
		case EVENT_MOVPB_TOUCH_FRWD_PRESS         : 
			#if 0
			if(bAudioState!=AUDIO_STATE_REW || m_AudPlayFFSpeed==1)
			{
	        	m_AudPlayFFSpeed = ( (m_AudPlayFFSpeed<<1) > AUDPB_MAX_SPEED )?(AUDPB_MIN_SPEED):(m_AudPlayFFSpeed<<1);
			}
			AHC_SetPlaySpeed(AHC_PB_MOVIE_FAST_FORWARD_RATE, m_AudPlayFFSpeed+1);
			#endif
			
			UpdateAudPBStatus(AUDIO_STATE_FF);   
			AudPBTimer_Start(AUDPB_TIMER_UNIT);
			AudPlayback_Forward();
			DrawStateAudPlaybackUpdate(EVENT_MOVPB_UPDATE_MESSAGE);
			DrawStateAudPlaybackUpdate(EVENT_MOVPB_TOUCH_FRWD_PRESS);
        break;
		
        case EVENT_MOVPB_TOUCH_PLAY_PRESS         : 
			
			switch(uState)
			{
				case STATE_AUD_STOP :
					UpdateAudPBStatus(AUDIO_STATE_PLAY);  
					MediaPlaybackConfig(PLAYBACK_CURRENT);
					AudPlayback_Play();
					DrawStateAudPlaybackUpdate(ulJobEvent);                      
				break;
                 
				case STATE_AUD_PLAY :
					if(AUD_STATE_FF == GetAudConfig()->iState)
					{
						UpdateAudPBStatus(AUDIO_STATE_PLAY);  
						AudPlayback_NormalSpeed();
						DrawStateAudPlaybackUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);
					}
					else
					{
						UpdateAudPBStatus(AUDIO_STATE_PAUSE);  
						AudPlayback_Pause();
						DrawStateAudPlaybackUpdate(EVENT_MOVPB_TOUCH_PAUSE_PRESS);                      
					}
				break;
                 
				case STATE_AUD_PAUSE   :
					UpdateAudPBStatus(AUDIO_STATE_PLAY);  
					AudPlayback_NormalSpeed();
					AudPlayback_Resume();
					DrawStateAudPlaybackUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);
				break;
                 
				default                :
				break;
			}
        break;
        
        //GPIO Key
        case EVENT_MOVPB_TOUCH_RETURN                       : 
			AudPBTimer_Stop();
			AudPlaybackParamReset();
			DrawStateAudPlaybackUpdate(EVENT_KEY_WIDE_STOP);
			AHC_UF_GetCurrentIndex(&ObjSelect);
			ret = StateSwitchMode(UI_BROWSER_STATE);
        break;  
        
        case EVENT_KEY_MENU:
			if(GetAudConfig()->iState == AUD_STATE_PLAY || GetAudConfig()->iState == AUD_STATE_FF)
        	{
        		AudPlayback_Pause();
        		bEnterMenuPause = AHC_TRUE;	
        	}
        	else if(GetAudConfig()->iState == AUD_STATE_PAUSE)
        	{
        		bEnterMenuPause = AHC_FALSE;	
        	}
        	StateSwitchMode(UI_PLAYBACK_MENU_STATE);
        break;
        
        case EVENT_POWER_OFF                      :          
           	if(GetAudConfig()->iState == AUD_STATE_PLAY || GetAudConfig()->iState == AUD_STATE_FF)
        	{
        		AudPlayback_Pause();
        		bEnterPowerOffPagePause = AHC_TRUE;	
        	}
        	else if(GetAudConfig()->iState == AUD_STATE_PAUSE)
        	{
        		bEnterPowerOffPagePause = AHC_FALSE;	
        	}
                        
			AHC_NormalPowerOffPath();
        break;                                    
        
		//Special Event
		case EVENT_MOVPB_TOUCH_PREV_PRESS         : 
			MediaPlaybackConfig(PLAYBACK_PREVIOUS);
			AudPlayback_Play();
			DrawStateAudPlaybackUpdate(ulJobEvent);
        break;

        case EVENT_MOVPB_TOUCH_NEXT_PRESS         : 
			MediaPlaybackConfig(PLAYBACK_NEXT);
			AudPlayback_Play();
			DrawStateAudPlaybackUpdate(ulJobEvent);
        break;
	#ifdef NET_SYNC_PLAYBACK_MODE
		case EVENT_NET_ENTER_PLAYBACK :
			AudPBTimer_Stop();
			AudPlaybackParamReset();
			ret = StateSwitchMode(UI_NET_PLAYBACK_STATE);
            CGI_FEEDBACK(ulEvent, 0 /* SUCCESSFULLY */);
		break;
	#endif
        case EVENT_VIDEO_PREVIEW                       : 
			AudPBTimer_Stop();
			AudPlaybackParamReset();
			ret = StateSwitchMode(UI_VIDEO_STATE);
        break; 

        case EVENT_CAMERA_PREVIEW                       : 
			AudPBTimer_Stop();
			AudPlaybackParamReset();
			ret = StateSwitchMode(UI_CAMERA_STATE);
        break; 

		case EVENT_LCD_POWER_SAVE 			:
			AHC_SwitchLCDBackLight();
		break;

#if (QUICK_FILE_EDIT_EN)
        case EVENT_FILE_DELETING : 
       
			ret = QuickFileOperation(DCF_FILE_DELETE);

			if(ret!=EDIT_FILE_SUCCESS)
			{
				DrawFileOperationResult(ret);
			}
			else
			{
				AudPBTimer_Stop();
				AudPlaybackParamReset();
				ret = StateSwitchMode(UI_BROWSER_STATE);  
			}
        break;

        case EVENT_LOCK_FILE_M   : 
     
        	if(AHC_CheckCurFileLockStatus())
        	{
				ret = QuickFileOperation(DCF_FILE_NON_READ_ONLY);//Unprotect
				bLockCurAudFile = AHC_FALSE;
			}
			else
			{
				ret = QuickFileOperation(DCF_FILE_READ_ONLY);;//Protect
        		bLockCurAudFile = AHC_TRUE;
        	}
        	
			if(ret!=EDIT_FILE_SUCCESS)
			{
				DrawFileOperationResult(ret);
			}
        break;
#endif

		case EVENT_SWITCH_PANEL_TVOUT:
			AHC_SwitchLCDandTVOUT();
		break;
        
        //Device 
		case EVENT_DC_CABLE_IN:
			#if (CHARGER_IN_ACT_OTHER_MODE==ACT_RECORD_VIDEO)
			AudPBTimer_Stop();
			AudPlaybackParamReset();
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
		     	#if (CHARGER_IN_ACT_OTHER_MODE==ACT_RECORD_VIDEO)
             	AudPBTimer_Stop();
             	AudPlaybackParamReset();
             	ret = StateSwitchMode(UI_VIDEO_STATE);
				AHC_SetRecordByChargerIn(3);
				#elif (CHARGER_IN_ACT_OTHER_MODE == ACT_FORCE_POWER_OFF)
				SetKeyPadEvent(KEY_POWER_OFF);
				#endif
			}
			else
			{
             	AudPBTimer_Stop();
                AudPlaybackParamReset();
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
        
        case EVENT_SD_REMOVED                     :
            AudPBFunc_SDMMC_Out();

            if(AHC_TRUE == AHC_SDMMC_GetMountState())
            {
                AHC_DisMountStorageMedia(AHC_MEDIA_MMC);
                AHC_OS_Sleep(100);
				Enable_SD_Power(0 /* Power Off */);
            }
            StateSwitchMode(UI_BROWSER_STATE);
        break;
        
        case EVENT_TV_DETECT                      :
#if 0
			if(AHC_IsTVConnectEx()) {
				AHC_GetAudioCurrentTime(&m_AudioCurPlayTime);
				AudPBTimer_Stop();
				AudPlaybackParamReset();
				//AHC_SetVideoPlayStartTime(m_AudioCurPlayTime);
				SetTVState(AHC_TV_MOVIE_PB_STATUS);
				StateSwitchMode(UI_TVOUT_STATE);
			}
#endif
        break;   

        case EVENT_TV_REMOVED                     :
        break;

        case EVENT_HDMI_DETECT                    :
#if 0        
			AHC_GetAudioCurrentTime(&m_AudioCurPlayTime);
			AudPBTimer_Stop();
			AudPlaybackParamReset();     
			//AHC_SetVideoPlayStartTime(m_AudioCurPlayTime);
			SetHDMIState(AHC_HDMI_MOVIE_PB_STATUS);
			StateSwitchMode(UI_HDMI_STATE);
#endif			
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
        	AHC_DrawRotateEvent(ulJobEvent);
        break;
        
		//Update Message
        case EVENT_MOVPB_UPDATE_MESSAGE           :
			if( AUD_STATE_STOP == GetAudConfig()->iState )
			{ 
				AudPBTimer_Stop();
				AudPlaybackParamReset();
				DrawStateAudPlaybackUpdate(EVENT_KEY_WIDE_STOP);
				AHC_UF_GetCurrentIndex(&ObjSelect);
				ret = StateSwitchMode(UI_BROWSER_STATE);
				AHC_OSDSetActive(OVL_DISPLAY_BUFFER, 1);  
				return;
			}
             
			DrawStateAudPlaybackUpdate(EVENT_MOVPB_UPDATE_MESSAGE);
        break;
        
        default:
        break;
    }
#endif    
}

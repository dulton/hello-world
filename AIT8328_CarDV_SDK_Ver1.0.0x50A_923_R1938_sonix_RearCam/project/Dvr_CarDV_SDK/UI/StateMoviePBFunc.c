/*===========================================================================
 * Include file 
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
#include "AHC_USB.h"
#include "AHC_Warningmsg.h"
#include "AHC_General_CarDV.h"
#include "MediaPlaybackCtrl.h"
#include "MenuSetting.h"
#include "MenuDrawCommon.h"
#include "MenuTouchButton.h"
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

#define MOVPB_OSD_SHOWTIME                           (5)  //unit :sec.
#define MOVPB_TIMER_UNIT                           (200)  //unit :sec.
#define MOVPB_ADJVOLTIMER_UNIT                     (500)  //unit :msec.

/*===========================================================================
 * Global varible
 *===========================================================================*/ 
AHC_BOOL                   	m_bMovieOSDShowUp 		= AHC_FALSE;
AHC_BOOL                   	m_bMovieOSDForceShowUp 	= AHC_TRUE;
UINT8                      	MovPBTimerID 			= 0xFF;
UINT8                      	MovPBAdjVolumeTimerID 	= 0xFF;
UINT32                     	MovPbCounter 			= 0;
UINT32                     	MovPbVolumeCnt 			= 0;//delay time=cnt*MOVPB_TIMER_UNIT
UINT32                     	m_VideoCurPlayTime 		= 0;
UINT8 						m_MovDrawCounter 		= 0;
AHC_BOOL					bLockCurMovFile			= AHC_FALSE;
// Make FFS/REW start from x2
UINT32						m_MovPlayFFSpeed		= 1;	//MOVPB_MIN_SPEED;
UINT32						m_MovPlayREWSpeed		= 1;	//MOVPB_MIN_SPEED;
AHC_BOOL					bEnterMenuPause 		= AHC_FALSE;
AHC_BOOL					bEnterPowerOffPagePause = AHC_FALSE;
UINT8 						bMovieState 			= MOVIE_STATE_PAUSE;
AHC_BOOL					bMovPbExitFromHdmi		= AHC_FALSE;
AHC_BOOL					bMovPbEnterToHdmi		= AHC_FALSE;
AHC_BOOL					bMovPbExitFromTV		= AHC_FALSE;
AHC_BOOL					bMovPbEnterToTV			= AHC_FALSE;

#if (SUPPORT_TOUCH_PANEL)
AHC_BOOL					m_ubShowMovPBTouchPage	= AHC_FALSE;
#endif

/*===========================================================================
 * Extern varible
 *===========================================================================*/

extern UINT32   		   	ObjSelect;
extern AHC_BOOL			   	bForce_PowerOff;
extern AHC_BOOL				bForceSwitchBrowser;
extern UINT8				bHdmiMovieState;
extern UINT8				bTVMovieState;
extern AHC_BOOL             m_ubPlaybackRearCam;

/*===========================================================================
 * Extern function
 *===========================================================================*/

extern void PhotoPBFunc_SDMMC_Out(void);
extern void AudPBFunc_SDMMC_Out(void);
extern void InitOSD(void);

/*===========================================================================
 * Main body
 *===========================================================================*/

void MovPBTimer_ResetCounter(void)
{
    MovPbCounter = 0;
}
 
void MovPBTimer_ISR(void *tmr, void *arg)
{
    UINT32 ustatus;

    if(	MOV_STATE_PLAY 	== GetMovConfig()->iState || 
    	MOV_STATE_FF 	== GetMovConfig()->iState || 
    	MOV_STATE_PAUSE == GetMovConfig()->iState)
    {
        AHC_GetVideoPlayStopStatus(&ustatus);
        
		if ((AHC_VIDEO_PLAY_EOF == ustatus) || (AHC_VIDEO_PLAY_ERROR_STOP == ustatus))
        {
            GetMovConfig()->iState = MOV_STATE_STOP;
            m_bMovieOSDShowUp = AHC_TRUE;
            AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_UPDATE_MESSAGE, 0);
          //  AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_UPDATE_FILE, 0);
            MovPBFunc_ResetPlaySpeed();
            return;
        }
        else if(ustatus == AHC_VIDEO_PLAY_FF_EOS || ustatus == AHC_VIDEO_PLAY_BW_BOS) 
        {
            GetMovConfig()->iState = MOV_STATE_PAUSE;
            MovPBFunc_ResetPlaySpeed();
            m_bMovieOSDShowUp = AHC_TRUE;
        }

        if(	ustatus == AHC_VIDEO_PLAY || 
        	ustatus == AHC_VIDEO_PLAY_FF_EOS || 
        	ustatus == AHC_VIDEO_PLAY_BW_BOS) 
        {
            if( m_bMovieOSDShowUp )
            {
				#if !defined(CAR_DV)
                MovPbCounter++;
                if( 0 == MovPbCounter%(MOVPB_OSD_SHOWTIME*5) )
                {
                    MovPbCounter = 0;
                    m_bMovieOSDShowUp = AHC_FALSE;
                }
				#endif
				AHC_SendAHLMessageEnable(AHC_TRUE);
                AHC_SendAHLMessage_HP(AHLM_UI_NOTIFICATION, BUTTON_UPDATE_MESSAGE, 0);
            }
            else 
            {
                MovPbCounter = 0;

                if(m_bMovieOSDForceShowUp) {
					AHC_SendAHLMessageEnable(AHC_TRUE);
                    AHC_SendAHLMessage_HP(AHLM_UI_NOTIFICATION, BUTTON_UPDATE_MESSAGE, 0);
                }
            }
        }
    }  
}

AHC_BOOL MovPBTimer_Start(UINT32 ulTime)
{
	if (0xFE <= MovPBTimerID)
	{
	    MovPBTimer_ResetCounter();
	    MovPBTimerID = AHC_OS_StartTimer( ulTime, AHC_OS_TMR_OPT_PERIODIC, MovPBTimer_ISR, (void*)NULL );

        if(0xFE > MovPBTimerID)
            return AHC_TRUE;
    }

    return AHC_FALSE;
}

void MovPBAdjVolumeTimer_ISR(void *tmr, void *arg)
{
    UINT32 ustatus;

    //if(	MOV_STATE_PLAY 	== GetMovConfig()->iState || 
    //	MOV_STATE_FF 	== GetMovConfig()->iState || 
    //	MOV_STATE_PAUSE == GetMovConfig()->iState)
    {
        AHC_GetVideoPlayStopStatus(&ustatus);
        
        if(MovPbVolumeCnt >= 1)
        {
            printc("MovPbVolumeCnt = %d\r\n",MovPbVolumeCnt);
            MovPbVolumeCnt -= 1;

            if(MovPbVolumeCnt == 0)
            {
                printc("BUTTON_OSD_HIDE\r\n");
                AHC_SendAHLMessage_HP(AHLM_UI_NOTIFICATION, BUTTON_OSD_HIDE, 0);
            }
        }
    }  
}

//unit:msec
AHC_BOOL MovPBAdjValumeTimer_Start(UINT32 ulTime)
{
	if (0xFE <= MovPBAdjVolumeTimerID)
	{
	    //MovPBTimer_ResetCounter();
        MovPbVolumeCnt = 0;
	    MovPBAdjVolumeTimerID = AHC_OS_StartTimer( ulTime, AHC_OS_TMR_OPT_PERIODIC, MovPBAdjVolumeTimer_ISR, (void*)NULL );

        if(0xFE > MovPBAdjVolumeTimerID)
            return AHC_TRUE;
    }

    return AHC_FALSE;
}

AHC_BOOL MovPBAdjVolumeTimer_Stop(void)
{
    UINT8 ret = 0;
    if(MovPBAdjVolumeTimerID < 0xFE) 
    {
        ret = AHC_OS_StopTimer( MovPBAdjVolumeTimerID, AHC_OS_TMR_OPT_PERIODIC );

        MovPBAdjVolumeTimerID = 0xFF;
        
        if(0xFE > ret)
    		return AHC_TRUE;
	}

    return AHC_FALSE;
}

AHC_BOOL MovPBTimer_Stop(void)
{
    UINT8 ret = 0;

    if(MovPBTimerID < 0xFE) 
    {
        ret = AHC_OS_StopTimer( MovPBTimerID, AHC_OS_TMR_OPT_PERIODIC );

        MovPBTimerID = 0xFF;
        
        if(0xFE > ret)
    		return AHC_TRUE;
	}

    return AHC_FALSE;
}

void MovPBFunc_CheckLCDstatus(void)
{
	UINT8 status;
	
	AHC_LCD_GetStatus(&status);
		
	AHC_SetDisplayMode(DISPLAY_MODE_DISABLE);	
	AHC_SetKitDirection(status, AHC_TRUE, AHC_SNR_NOFLIP, AHC_FALSE);      		
}

void MovPBFunc_SetOSDShowStatus(AHC_BOOL state)
{
    m_bMovieOSDShowUp = state;
}

AHC_BOOL MovPBFunc_GetOSDShowStatus(void)
{
    return m_bMovieOSDShowUp;
}

void MovPBFunc_ResetPlaySpeed(void)
{
	m_MovPlayFFSpeed	= 1;//MOVPB_MIN_SPEED;
	m_MovPlayREWSpeed	= 1;//MOVPB_MIN_SPEED;
}

void MovPBFunc_SDMMC_Out(void)
{
	if(MOV_STATE_FF == GetMovConfig()->iState)
	{	
		MovPlayback_NormalSpeed();
		AHC_OS_Sleep(100);
	}

	AHC_ResetVideoPlayParam();
             	
	AHC_SetMode(AHC_MODE_IDLE);
        	 	
	MovPBTimer_Stop();
	MovPlaybackParamReset();
	AHC_UF_GetCurrentIndex(&ObjSelect);
}

AHC_BOOL MovPBFunc_SeekSOS(void)
{
	AHC_SetVideoPlayStartTime(0);
	
	#if (MOVIE_PB_FF_EOS_ACTION==EOS_ACT_PAUSE_AT_FIRST_FRAME)
	AHC_SetParam(PARAM_ID_MOVIE_AUTO_PLAY, 1);
	#endif
	
	MovPlaybackParamReset();
    MediaPlaybackConfig(PLAYBACK_CURRENT);
    MovPlayback_Play();
	MovPBFunc_ResetPlaySpeed();

	#if (MOVIE_PB_FF_EOS_ACTION==EOS_ACT_PAUSE_AT_FIRST_FRAME)
	AHC_SetParam(PARAM_ID_MOVIE_AUTO_PLAY, 0);
	UpdateMovPBStatus(MOVIE_STATE_PAUSE);
	DrawStateMovPlaybackUpdate(EVENT_MOVPB_TOUCH_PAUSE_PRESS);
	#else
	UpdateMovPBStatus(MOVIE_STATE_PLAY);
	DrawStateMovPlaybackUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);
	#endif
	
    return AHC_TRUE;
}

AHC_BOOL MoviePBMode_Start(void)
{
    AHC_BOOL ahc_ret = AHC_TRUE;

    m_bMovieOSDShowUp = AHC_TRUE;
	
    AHC_SetMode(AHC_MODE_IDLE);
    
    MovPlaybackParamReset();
    MovPBFunc_CheckLCDstatus();
    MediaPlaybackConfig(PLAYBACK_CURRENT);
    
    if(bMovPbExitFromHdmi || bMovPbExitFromTV)
    {
		UINT8 ubCheckState = MOVIE_STATE_PLAY;
		
		if(bMovPbExitFromHdmi)
			ubCheckState = bHdmiMovieState;
		else if(bMovPbExitFromTV)
			ubCheckState = bTVMovieState;

        switch(ubCheckState)
        {
        	case MOVIE_STATE_STOP://TBD
        		ahc_ret = MovPlayback_Play();
        		MovPBFunc_ResetPlaySpeed();
        		UpdateMovPBStatus(MOVIE_STATE_PLAY);
        	
        	break;
			
        	case MOVIE_STATE_PLAY:
        		ahc_ret = MovPlayback_Play();
        		MovPBFunc_ResetPlaySpeed();
        	break;
        	
        	case MOVIE_STATE_PAUSE:
        		ahc_ret = MovPlayback_Play();
        		MovPBFunc_ResetPlaySpeed();
        		AHC_OS_SleepMs(100);
        		ahc_ret = MovPlayback_Pause();
        	break;
        	
        	case MOVIE_STATE_FF:
        		ahc_ret = MovPlayback_Play();
        		AHC_OS_SleepMs(100);
        		
				AHC_SetPlaySpeed(AHC_PB_MOVIE_FAST_FORWARD_RATE, m_MovPlayFFSpeed+1);
	        	MovPlayback_Forward();
        	break;
        	
        	case MOVIE_STATE_REW:
        		ahc_ret = MovPlayback_Play();
				AHC_OS_SleepMs(100);
				
				AHC_SetPlaySpeed(AHC_PB_MOVIE_BACKWARD_RATE, m_MovPlayREWSpeed+1);
	       	 	MovPlayback_Backward();
        	break;
        	
        	default:
        	break;
        }
    }
    else
    {
   		ahc_ret = MovPlayback_Play();
   		MovPBFunc_ResetPlaySpeed();
	}

#if (QUICK_FILE_EDIT_EN)
	bLockCurMovFile = AHC_CheckCurFileLockStatus();
#endif

    if(AHC_TRUE == ahc_ret )
    {
#if (SUPPORT_TOUCH_PANEL)//disable temporally, wait touch UI
        KeyParser_ResetTouchVariable();
        KeyParser_TouchItemRegister(&MovPBMainPage_TouchButton[0], ITEMNUM(MovPBMainPage_TouchButton));    
#endif
		if(!bMovPbExitFromHdmi && !bMovPbExitFromTV)
		{
    		UpdateMovPBStatus(MOVIE_STATE_PLAY);
        }
        else
        {
        	bMovPbExitFromHdmi = AHC_FALSE;
			bMovPbExitFromTV   = AHC_FALSE;
        }
        MovPBTimer_Start(MOVPB_TIMER_UNIT);
        DrawStateMovPlaybackUpdate(EVENT_MOVPB_MODE_INIT);
    }
    
    return ahc_ret;
}

void MoviePBMode_Update(void)
{
	UINT32	MaxObjIdx;

    MaxObjIdx = AHC_GetMappingFileNum(FILE_MOVIE);

    if(MaxObjIdx == 0)
    {
    	MovPBTimer_Stop();
		MovPlaybackParamReset();
		AHC_UF_GetCurrentIndex(&ObjSelect);
		StateSwitchMode(UI_BROWSER_STATE);	
    	return;
	}
	
	if(GetMovConfig()->iState == MOV_STATE_PAUSE)
    {   
    	if(bEnterMenuPause==AHC_TRUE) 
    	{
    		MovPlayback_Resume();	
    		bEnterMenuPause = AHC_FALSE;
    	}
    	else
    	{
    		UpdateMovPBStatus(MOVIE_STATE_PAUSE);
    	}
    }

#if (QUICK_FILE_EDIT_EN)
	bLockCurMovFile = AHC_CheckCurFileLockStatus();
#endif

#if (SUPPORT_TOUCH_PANEL)//disable temporally, wait touch UI
    KeyParser_ResetTouchVariable();
    KeyParser_TouchItemRegister(&MovPBMainPage_TouchButton[0], ITEMNUM(MovPBMainPage_TouchButton));    
#endif

	DrawStateMovPbMode_Update();
}

void PlaybackMode_SDMMC_In(void)
{				
	CHARGE_ICON_ENABLE(AHC_FALSE);	
	
	bForceSwitchBrowser = AHC_TRUE;
	AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect);
						
	CHARGE_ICON_ENABLE(AHC_TRUE);	
}

void PlaybackMode_SDMMC_Out(void)
{
	CHARGE_ICON_ENABLE(AHC_FALSE);
			
	if(MOVPB_MODE == GetCurrentOpMode())
		MovPBFunc_SDMMC_Out();
	else if	(JPGPB_MODE == GetCurrentOpMode())
		PhotoPBFunc_SDMMC_Out();
	else if	(AUDPB_MODE == GetCurrentOpMode())
		AudPBFunc_SDMMC_Out();
	else if	(JPGPB_MOVPB_MODE == GetCurrentOpMode()) {
		if(PLAYBACK_IMAGE_TYPE == GetPlayBackFileType())
			PhotoPBFunc_SDMMC_Out();
		else
			MovPBFunc_SDMMC_Out();   
	}		
	bForceSwitchBrowser = AHC_TRUE; 
    
    if(AHC_TRUE == AHC_SDMMC_GetMountState())
        AHC_DisMountStorageMedia(AHC_MEDIA_MMC);
				
	CHARGE_ICON_ENABLE(AHC_TRUE);
}

//******************************************************************************
//
//                              AHC State Movie Playback Mode
//
//******************************************************************************

void StateMoviePlaybackMode(UINT32 ulEvent)
{
    UINT32 		ulJobEvent 	= ulEvent;
    UINT32 		uState;
    AHC_BOOL 	ret 		= AHC_TRUE;

	if(ulJobEvent == EVENT_NONE)
	    return;

    if(EVENT_MOVPB_UPDATE_MESSAGE != ulJobEvent)
        MovPBTimer_ResetCounter();

	AHC_GetSystemStatus(&uState);
	
    switch(ulJobEvent)
    {
        case EVENT_NONE                           : 
        break; 
        
        case EVENT_KEY_UP	:
            MovPBAdjValumeTimer_Start(MOVPB_ADJVOLTIMER_UNIT);
            MovPbVolumeCnt = 5;//delay time=cnt*MOVPB_ADJVOLTIMER_UNIT
			AHC_ChangeVolume(AHC_FALSE, 1);
			DrawStateMovPlaybackUpdate(EVENT_KEY_TELE_PRESS);
        break;
     
        case EVENT_KEY_DOWN	:
            MovPBAdjValumeTimer_Start(MOVPB_ADJVOLTIMER_UNIT);
            MovPbVolumeCnt = 5;
			AHC_ChangeVolume(AHC_FALSE, 0);
        	DrawStateMovPlaybackUpdate(EVENT_KEY_WIDE_PRESS);
        break;

        case EVENT_SLIDESHOW_OSD_HIDE:
        case EVENT_KEY_WIDE_STOP : //Clear VolumeBar
        case EVENT_KEY_TELE_STOP :
			DrawStateMovPlaybackUpdate(EVENT_KEY_WIDE_STOP);
        break;

        case EVENT_MOVPB_TOUCH_BKWD_PRESS         : 
       		#if (SUPPORT_TOUCH_PANEL)//disable temporally, wait touch UI
			m_ubShowMovPBTouchPage = 1;
			
			if(m_ubShowMovPBTouchPage)
			{
			    KeyParser_ResetTouchVariable();
        		KeyParser_TouchItemRegister(&MovPBCtrlPage_TouchButton[0], ITEMNUM(MovPBCtrlPage_TouchButton));

				DrawStateMovPbPageSwitch(TOUCH_CTRL_PAGE);
			}
			#endif
        	if(bMovieState!=MOVIE_STATE_FF || m_MovPlayREWSpeed==1)
        	{	
         		m_MovPlayREWSpeed = ( (m_MovPlayREWSpeed<<1) > MOVPB_MAX_SPEED )?(MOVPB_MIN_SPEED):(m_MovPlayREWSpeed<<1); 	
        	}
			AHC_SetPlaySpeed(AHC_PB_MOVIE_BACKWARD_RATE, m_MovPlayREWSpeed+1); 
        		
        	UpdateMovPBStatus(MOVIE_STATE_REW);  
			MovPBTimer_Start(MOVPB_TIMER_UNIT);
			MovPlayback_Backward();
			DrawStateMovPlaybackUpdate(EVENT_MOVPB_UPDATE_MESSAGE);
			DrawStateMovPlaybackUpdate(EVENT_MOVPB_TOUCH_BKWD_PRESS);
        break;
		
		case EVENT_MOVPB_TOUCH_FRWD_PRESS         : 
			#if (SUPPORT_TOUCH_PANEL)//disable temporally, wait touch UI
			m_ubShowMovPBTouchPage = 1;
			
			if(m_ubShowMovPBTouchPage)
			{
			    KeyParser_ResetTouchVariable();
        		KeyParser_TouchItemRegister(&MovPBCtrlPage_TouchButton[0], ITEMNUM(MovPBCtrlPage_TouchButton));

				DrawStateMovPbPageSwitch(TOUCH_CTRL_PAGE);
			}
			#endif
			if(bMovieState!=MOVIE_STATE_REW || m_MovPlayFFSpeed==1)
			{
	        	m_MovPlayFFSpeed = ( (m_MovPlayFFSpeed<<1) > MOVPB_MAX_SPEED )?(MOVPB_MIN_SPEED):(m_MovPlayFFSpeed<<1);
			}
			AHC_SetPlaySpeed(AHC_PB_MOVIE_FAST_FORWARD_RATE, m_MovPlayFFSpeed+1);
			
			UpdateMovPBStatus(MOVIE_STATE_FF);   
			MovPBTimer_Start(MOVPB_TIMER_UNIT);
			MovPlayback_Forward();
			DrawStateMovPlaybackUpdate(EVENT_MOVPB_UPDATE_MESSAGE);
			DrawStateMovPlaybackUpdate(EVENT_MOVPB_TOUCH_FRWD_PRESS);
        break;
		
        case EVENT_MOVPB_TOUCH_PLAY_PRESS         : 
			
			switch(uState)
			{
				case STATE_MOV_IDLE :
                    DrawStateMovPlaybackUpdate(EVENT_KEY_TELE_STOP);
					UpdateMovPBStatus(MOVIE_STATE_PLAY);  
					MediaPlaybackConfig(PLAYBACK_CURRENT);
					MovPlayback_Play();
					DrawStateMovPlaybackUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);                      
		        #if (SUPPORT_TOUCH_PANEL)//disable temporally, wait touch UI
				    KeyParser_ResetTouchVariable();
		    		KeyParser_TouchItemRegister(&MovPBCtrlPage_TouchButton[0], ITEMNUM(MovPBCtrlPage_TouchButton));
		    	#endif
				break;
                 
				case STATE_MOV_EXECUTE :
					if(MOV_STATE_FF == GetMovConfig()->iState)
					{
						UpdateMovPBStatus(MOVIE_STATE_PLAY);  
						MovPlayback_NormalSpeed();
						DrawStateMovPlaybackUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);
					}
					else
					{
						UpdateMovPBStatus(MOVIE_STATE_PAUSE);  
						MovPlayback_Pause();
						DrawStateMovPlaybackUpdate(EVENT_MOVPB_TOUCH_PAUSE_PRESS);                      
					}
		        #if (SUPPORT_TOUCH_PANEL)//disable temporally, wait touch UI
				    KeyParser_ResetTouchVariable();
		    		KeyParser_TouchItemRegister(&MovPBCtrlPage_TouchButton[0], ITEMNUM(MovPBCtrlPage_TouchButton));
		    	#endif
				break;
                 
				case STATE_MOV_PAUSE   :
					UpdateMovPBStatus(MOVIE_STATE_PLAY);  
					MovPlayback_NormalSpeed();
					MovPlayback_Resume();
					DrawStateMovPlaybackUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);
		        #if (SUPPORT_TOUCH_PANEL)//disable temporally, wait touch UI
				    KeyParser_ResetTouchVariable();
		    		KeyParser_TouchItemRegister(&MovPBCtrlPage_TouchButton[0], ITEMNUM(MovPBCtrlPage_TouchButton));
		    	#endif
				break;
                 
				default                :
				break;
			}
        break;
        
        //GPIO Key
        case EVENT_MOVPB_TOUCH_RETURN                       : 
			MovPBTimer_Stop();
            MovPBAdjVolumeTimer_Stop();
			MovPlaybackParamReset();
			DrawStateMovPlaybackUpdate(EVENT_KEY_WIDE_STOP);
			AHC_UF_GetCurrentIndex(&ObjSelect);
			ret = StateSwitchMode(UI_BROWSER_STATE);
        break;  
        
        case EVENT_KEY_MENU:
            #ifdef CFG_DRAW_SCREEN_128
			// Do notthing
			#else
			if(GetMovConfig()->iState == MOV_STATE_PLAY || GetMovConfig()->iState == MOV_STATE_FF)
        	{
        		MovPlayback_Pause();
        		bEnterMenuPause = AHC_TRUE;
        	}
        	else if(GetMovConfig()->iState == MOV_STATE_PAUSE)
        	{
        		bEnterMenuPause = AHC_FALSE;	
        	}
        	DrawMovPB_VolumeBar(AHC_TRUE);
        	StateSwitchMode(UI_PLAYBACK_MENU_STATE);
			#endif
        break;

		case EVENT_MOVPB_MODE:
			#if VIDRECD_MULTI_TRACK
			m_ubPlaybackRearCam = (m_ubPlaybackRearCam == AHC_TRUE)? AHC_FALSE : AHC_TRUE;
			AHC_GetVideoCurrentPlayTime(&m_VideoCurPlayTime);
			MovPlaybackParamReset();
			AHC_SetMode(AHC_MODE_IDLE);
			if( m_ubPlaybackRearCam == AHC_TRUE )
	        {
	            MMPS_VIDPLAY_SetCurrentTrack(1);  //playback rear cam.
	        }
	        else
	        {
	            MMPS_VIDPLAY_SetCurrentTrack(0);  //playback front cam.
	        }	
			AHC_SetVideoPlayStartTime(m_VideoCurPlayTime);
			MovPlayback_Play();
			#endif
		break;
		
        case EVENT_POWER_OFF                      :          
           	if(GetMovConfig()->iState == MOV_STATE_PLAY || GetMovConfig()->iState == MOV_STATE_FF)
        	{
        		MovPlayback_Pause();
        		bEnterPowerOffPagePause = AHC_TRUE;	
        	}
        	else if(GetMovConfig()->iState == MOV_STATE_PAUSE)
        	{
        		bEnterPowerOffPagePause = AHC_FALSE;	
        	}
                        
			AHC_NormalPowerOffPath();
        break;                                    
        
		//Special Event
		case EVENT_MOVPB_TOUCH_PREV_PRESS         : 
			MediaPlaybackConfig(PLAYBACK_PREVIOUS);
			MovPlayback_Play();
			DrawStateMovPlaybackUpdate(ulJobEvent);
        break;

        case EVENT_MOVPB_TOUCH_NEXT_PRESS         : 
			MediaPlaybackConfig(PLAYBACK_NEXT);
			MovPlayback_Play();
			DrawStateMovPlaybackUpdate(ulJobEvent);
        break;
	#ifdef NET_SYNC_PLAYBACK_MODE
		case EVENT_NET_ENTER_PLAYBACK : 
			MovPBTimer_Stop();
            MovPBAdjVolumeTimer_Stop();
			MovPlaybackParamReset();
            StateSwitchMode(UI_NET_PLAYBACK_STATE);
            CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
        break;
    #endif//NET_SYNC_PLAYBACK_MODE
        case EVENT_VIDEO_PREVIEW                       : 
			MovPBTimer_Stop();
            MovPBAdjVolumeTimer_Stop();
			MovPlaybackParamReset();
			ret = StateSwitchMode(UI_VIDEO_STATE);
        break; 

        case EVENT_CAMERA_PREVIEW                       : 
			MovPBTimer_Stop();
            MovPBAdjVolumeTimer_Stop();
			MovPlaybackParamReset();
			ret = StateSwitchMode(UI_CAMERA_STATE);
        break; 

		case EVENT_LCD_POWER_SAVE 			:
			AHC_SwitchLCDBackLight();
		break;

        case EVENT_FILE_DELETING : 
       
			ret = QuickFileOperation(DCF_FILE_DELETE);

			if(ret!=EDIT_FILE_SUCCESS)
			{
				DrawFileOperationResult(ret);
			}
			else
			{
				MovPBTimer_Stop();
                MovPBAdjVolumeTimer_Stop();
				MovPlaybackParamReset();
				ret = StateSwitchMode(UI_BROWSER_STATE);  
			}
        break;

        case EVENT_LOCK_FILE_M   : 
     
        	if(AHC_CheckCurFileLockStatus())
        	{
				ret = QuickFileOperation(DCF_FILE_NON_READ_ONLY);//Unprotect
				bLockCurMovFile = AHC_FALSE;
			}
			else
			{
				ret = QuickFileOperation(DCF_FILE_READ_ONLY);;//Protect
        		bLockCurMovFile = AHC_TRUE;
        	}
        	
			if(ret!=EDIT_FILE_SUCCESS)
			{
				DrawFileOperationResult(ret);
			}
        break;

		case EVENT_SWITCH_PANEL_TVOUT:
			AHC_SwitchLCDandTVOUT();
		break;

#if (SUPPORT_TOUCH_PANEL)//disable temporally, wait touch UI
		case EVENT_SWITCH_TOUCH_PAGE:
			m_ubShowMovPBTouchPage ^= 1;
			
			if(m_ubShowMovPBTouchPage)
			{
			    KeyParser_ResetTouchVariable();
        		KeyParser_TouchItemRegister(&MovPBCtrlPage_TouchButton[0], ITEMNUM(MovPBCtrlPage_TouchButton));

				DrawStateMovPbPageSwitch(TOUCH_CTRL_PAGE);
			}
			else
			{
			    KeyParser_ResetTouchVariable();
        		KeyParser_TouchItemRegister(&MovPBMainPage_TouchButton[0], ITEMNUM(MovPBMainPage_TouchButton));
			
				DrawStateMovPbPageSwitch(TOUCH_MAIN_PAGE);
			}
		break;
#endif

        //Device 
		case EVENT_DC_CABLE_IN:
			#if (CHARGER_IN_ACT_OTHER_MODE==ACT_RECORD_VIDEO)
			MovPBTimer_Stop();
            MovPBAdjVolumeTimer_Stop();
			MovPlaybackParamReset();
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
             	MovPBTimer_Stop();
                MovPBAdjVolumeTimer_Stop();
             	MovPlaybackParamReset();
             	ret = StateSwitchMode(UI_VIDEO_STATE);
				AHC_SetRecordByChargerIn(3);
				#elif (CHARGER_IN_ACT_OTHER_MODE == ACT_FORCE_POWER_OFF)
				SetKeyPadEvent(KEY_POWER_OFF);
				#endif
			}
			else
			{
             	MovPBTimer_Stop();
                MovPBAdjVolumeTimer_Stop();
                MovPlaybackParamReset();
             	//AHC_OSDUninit();//to fixed mantis:0988313. by cz
	            //AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
    	        //InitOSD();
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
            MovPBFunc_SDMMC_Out();

            if(AHC_TRUE == AHC_SDMMC_GetMountState())
            {
                AHC_DisMountStorageMedia(AHC_MEDIA_MMC);
                AHC_OS_Sleep(100);
				Enable_SD_Power(0 /* Power Off */);
            }
            StateSwitchMode(UI_BROWSER_STATE);
        break;
        
        case EVENT_TV_DETECT                      :
			if(AHC_IsTVConnectEx()) {
				AHC_GetVideoCurrentPlayTime(&m_VideoCurPlayTime);
				MovPBTimer_Stop();
                MovPBAdjVolumeTimer_Stop();
				MovPlaybackParamReset();
				AHC_SetVideoPlayStartTime(m_VideoCurPlayTime);
				bMovPbEnterToTV = AHC_TRUE;
				SetTVState(AHC_TV_MOVIE_PB_STATUS);
				StateSwitchMode(UI_TVOUT_STATE);
			}
        break;   

        case EVENT_TV_REMOVED                     :
        break;

        case EVENT_HDMI_DETECT                    :
			AHC_GetVideoCurrentPlayTime(&m_VideoCurPlayTime);
			MovPBTimer_Stop();
            MovPBAdjVolumeTimer_Stop();
			MovPlaybackParamReset();     
			AHC_SetVideoPlayStartTime(m_VideoCurPlayTime);
			bMovPbEnterToHdmi = AHC_TRUE;
			SetHDMIState(AHC_HDMI_MOVIE_PB_STATUS);
			StateSwitchMode(UI_HDMI_STATE);
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
			if(MOV_STATE_STOP == GetMovConfig()->iState)
			{ 
				MovPBTimer_Stop();
                MovPBAdjVolumeTimer_Stop();
				MovPlaybackParamReset();
				DrawStateMovPlaybackUpdate(EVENT_KEY_WIDE_STOP);
				AHC_UF_GetCurrentIndex(&ObjSelect);
				ret = StateSwitchMode(UI_BROWSER_STATE);
				AHC_OSDSetActive(OVL_DISPLAY_BUFFER, 1);  
				return;
			}
            else if(MOV_STATE_PAUSE == GetMovConfig()->iState)
            {
            	UINT32 	ulState; 
            
            	AHC_GetVideoPlayStopStatus(&ulState);
            
            	if(ulState == AHC_VIDEO_PLAY_FF_EOS || ulState == AHC_VIDEO_PLAY_BW_BOS)
				{
					#if ( MOVIE_PB_FF_EOS_ACTION==EOS_ACT_RESTART || \
						  MOVIE_PB_FF_EOS_ACTION==EOS_ACT_PAUSE_AT_FIRST_FRAME )
					MovPBFunc_SeekSOS();
					#endif
				}
            }

			if(m_bMovieOSDShowUp)
				DrawStateMovPlaybackUpdate(EVENT_MOVPB_UPDATE_MESSAGE);
			else
				DrawStateMovPlaybackUpdate(EVENT_MOVPB_OSD_HIDE);
        break;
        

        default:
        break;
    }
}

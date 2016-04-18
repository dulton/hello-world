/*===========================================================================
 * Include file 
 *===========================================================================*/ 
 
#include "AHC_Common.h"
#include "AHC_Parameter.h"
#include "AHC_General.h"
#include "AHC_Message.h"
#include "AHC_Utility.h"
#include "AHC_Display.h"
#include "AHC_Dcf.h"
#include "AHC_UF.h"
#include "AHC_Browser.h"
#include "AHC_Menu.h"
#include "AHC_Os.h"
#include "AHC_Media.h"
#include "AHC_Usb.h"
#include "AHC_Warningmsg.h"
#include "AHC_General_CarDV.h"
#include "IconPosition.h"
#include "StatePhotoPBFunc.h"
#include "StateHDMIFunc.h"
#include "StateTVFunc.h"
#include "MediaPlaybackCtrl.h"
#include "DrawStatePhotoPbFunc.h"
#include "MenuSetting.h"
#include "MenuTouchButton.h"
#include "KeyParser.h"
#include "LedControl.h"
#include "dsc_charger.h"

/*===========================================================================
 * Macro define 
 *===========================================================================*/ 

#define  PHOTOPB_TIMER_UNIT                       (100)
#define  PHOTOPB_OSD_SHOWTIME                       (3)  //unit :sec.

/*===========================================================================
 * Global varible
 *===========================================================================*/ 

UINT8		PhotoPBTimerID 			= 0xFF;
UINT32		PhotoPBCounter 			= 0;
AHC_BOOL	PhotoOSDShowUp 	 		= AHC_TRUE;
AHC_BOOL	bLockCurJpegFile 		= AHC_FALSE;
AHC_BOOL 	m_ubZoomLock 			= AHC_FALSE;

#if (SUPPORT_TOUCH_PANEL)
AHC_BOOL	m_ubShowJpgPBTouchPage	= AHC_FALSE;
#endif

/*===========================================================================
 * Extern varible
 *===========================================================================*/ 

extern UINT32   ObjSelect;
extern AHC_BOOL	bForce_PowerOff;

/*===========================================================================
 * Extern function
 *===========================================================================*/ 

extern void InitOSD(void);

/*===========================================================================
 * Main body
 *===========================================================================*/ 

void PhotoPBTimer_ResetCounter(void)
{
    PhotoPBCounter = 0;
}

void PhotoPBTimer_ISR(void *tmr, void *arg)
{
    if( PhotoOSDShowUp )
    {
        PhotoPBCounter++;
        
        if( 0 == PhotoPBCounter%(PHOTOPB_OSD_SHOWTIME*10) )
        {
            PhotoPBCounter = 0;
            #if !defined(CAR_DV)//EROY0110 Enforce show OSD
            PhotoOSDShowUp = AHC_FALSE;
			#endif
			
            switch(uiGetCurrentState()) 
            {
	            case  UI_HDMI_STATE:
	            case  UI_TVOUT_STATE:
	                AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_UPDATE_MESSAGE, 0);
	            break;
	            case  UI_PLAYBACK_STATE:
	        	#if !defined(CAR_DV)
	                DrawStateJpgPlaybackUpdate(EVENT_JPGPB_OSD_HIDE);
	        	#endif
	            break;
            }
        }
    }
}

AHC_BOOL PhotoPBTimer_Start(UINT32 ulTime)
{
    if(PhotoPBTimerID == 0xFF) 
    {
        PhotoPBTimer_ResetCounter();
        PhotoPBTimerID = AHC_OS_StartTimer( ulTime, AHC_OS_TMR_OPT_PERIODIC, PhotoPBTimer_ISR, (void*)NULL );
    
        if(0xFE <= PhotoPBTimerID)
    		return AHC_FALSE;
    	else
    		return AHC_TRUE;
	}
	else {
        return AHC_FALSE;
	}
}

AHC_BOOL PhotoPBTimer_Stop(void)
{
    UINT8 ret = 0;

    if(PhotoPBTimerID < 0xFE) 
    {
        ret = AHC_OS_StopTimer( PhotoPBTimerID, AHC_OS_TMR_OPT_PERIODIC );

        PhotoPBTimerID = 0xFF;
        
        if(0xFF == ret)
     		return AHC_FALSE;
    	else
    		return AHC_TRUE;
	}
	else {
        return AHC_FALSE;
	}
}

AHC_BOOL PhotoPBFunc_IsZoomLock(void)
{
	return m_ubZoomLock;
}

void PhotoPBFunc_SetZoomLock(AHC_BOOL bZoomlock)
{
	m_ubZoomLock = bZoomlock;
}
 
void PhotoPBFunc_CheckLCDstatus(void)
{
	UINT8 status;

	AHC_LCD_GetStatus(&status);
		
	AHC_SetDisplayMode(DISPLAY_MODE_DISABLE);		
	AHC_SetKitDirection(status, AHC_TRUE, AHC_SNR_NOFLIP, AHC_FALSE);         		
}

void PhotoPBFunc_SetOSDShowStatus(AHC_BOOL state)
{
    PhotoOSDShowUp = state;
}

AHC_BOOL PhotoPBFunc_GetOSDShowStatus(void)
{
    return PhotoOSDShowUp;
}

void PhotoPBFunc_SDMMC_Out(void)
{
	AHC_SetMode(AHC_MODE_IDLE);

	PhotoPBTimer_Stop();
	JpgPlaybackParamReset();
	AHC_UF_GetCurrentIndex(&ObjSelect);
}

AHC_BOOL PhotoPBMode_Start(void)
{
    AHC_BOOL ahc_ret = AHC_TRUE;

    PhotoOSDShowUp   = AHC_TRUE;

    AHC_SetMode(AHC_MODE_IDLE);
    
    JpgPlaybackParamReset();
	PhotoPBFunc_CheckLCDstatus();
    MediaPlaybackConfig(PLAYBACK_CURRENT);
    
    ahc_ret = JpgPlayback_Play();

#if (QUICK_FILE_EDIT_EN)
	bLockCurJpegFile = AHC_CheckCurFileLockStatus();
#endif
    
    if(AHC_TRUE == ahc_ret )
    {
#if (SUPPORT_TOUCH_PANEL)//disable temporally, wait touch UI
		KeyParser_ResetTouchVariable();
		KeyParser_TouchItemRegister(&JpgPBMainPage_TouchButton[0], ITEMNUM(JpgPBMainPage_TouchButton));    
#endif

		PhotoPBTimer_Start(PHOTOPB_TIMER_UNIT);
        DrawStateJpgPlaybackUpdate(EVENT_JPGPB_MODE_INIT);
    }
    return ahc_ret;
}

void PhotoPBMode_Update(void)
{
	UINT32	MaxObjIdx;

    MaxObjIdx = AHC_GetMappingFileNum(FILE_PHOTO);

    if(MaxObjIdx == 0)
    {
        PhotoPBTimer_Stop();
        JpgPlaybackParamReset();
        AHC_UF_GetCurrentIndex(&ObjSelect);
        StateSwitchMode(UI_BROWSER_STATE);
    	return;
	}

#if (QUICK_FILE_EDIT_EN)
	bLockCurJpegFile = AHC_CheckCurFileLockStatus();
#endif

#if (SUPPORT_TOUCH_PANEL)//disable temporally, wait touch UI
	KeyParser_ResetTouchVariable();
	KeyParser_TouchItemRegister(&JpgPBMainPage_TouchButton[0], ITEMNUM(JpgPBMainPage_TouchButton));    
#endif

	DrawStateJpgPbMode_Update();
}

//******************************************************************************
//
//                              AHC State JPG Playback Mode
//
//******************************************************************************

void StatePhotoPlaybackMode(UINT32 ulEvent)
{
    UINT32 		ulJobEvent 	= ulEvent;
    AHC_BOOL 	ret 		= AHC_TRUE;
		
	if(ulJobEvent == EVENT_NONE)
	    return;
	
    PhotoPBTimer_ResetCounter();
	
    switch(ulJobEvent)
    {
        case EVENT_NONE                           :
        break;
        
        case EVENT_KEY_UP                 :
			if(!PhotoPBFunc_IsZoomLock()) 
				JpgPlayback_Zoom(JPGPB_ZOOM_IN);
			else
             	JpgPlayback_Pan(JPGPB_PAN_UP);
        break;  

        case EVENT_KEY_DOWN                 :
			if(!PhotoPBFunc_IsZoomLock()) 
             	JpgPlayback_Zoom(JPGPB_ZOOM_OUT);
			else
             	JpgPlayback_Pan(JPGPB_PAN_DOWN);
        break;

        case EVENT_KEY_LEFT                       :
			if(!PhotoPBFunc_IsZoomLock()) 
			{
				MediaPlaybackConfig(PLAYBACK_PREVIOUS);
                JpgPlayback_Play();
                DrawStateJpgPlaybackUpdate(ulJobEvent);  
			}
			else
             	JpgPlayback_Pan(JPGPB_PAN_LEFT);
        break; 

        case EVENT_KEY_RIGHT                      :
			if(!PhotoPBFunc_IsZoomLock()) 
			{
             	MediaPlaybackConfig(PLAYBACK_NEXT);
             	JpgPlayback_Play();
             	DrawStateJpgPlaybackUpdate(ulJobEvent);
			}
			else
             	JpgPlayback_Pan(JPGPB_PAN_RIGHT);
        break; 

        case EVENT_KEY_SET                       :
			if(GetJpgConfig()->iCurZoomLevel!=0 && !PhotoPBFunc_IsZoomLock())
			{
				PhotoPBFunc_SetZoomLock(AHC_TRUE);
        	 	printc("Zoom_Lcok=1\r\n");
			}
			else if(PhotoPBFunc_IsZoomLock())
			{
        	 	PhotoPBFunc_SetZoomLock(AHC_FALSE);
        	 	printc("Zoom_Lcok=0\r\n");
			}
        break;
        
        case EVENT_JPGPB_TOUCH_RETURN:
        	PhotoPBFunc_SetZoomLock(AHC_FALSE);
        	PhotoPBTimer_Stop();
	        JpgPlaybackParamReset();
	        AHC_UF_GetCurrentIndex(&ObjSelect);
	        ret = StateSwitchMode(UI_BROWSER_STATE);
		break;
        
        case EVENT_KEY_MENU:
        	StateSwitchMode(UI_PLAYBACK_MENU_STATE);
        break;

        case EVENT_POWER_OFF                      :
			AHC_NormalPowerOffPath();
        break;
		
		//Special Event
        case EVENT_JPGPB_TOUCH_PREV_PRESS                 :
			MediaPlaybackConfig(PLAYBACK_PREVIOUS);
			JpgPlayback_Play();
			DrawStateJpgPlaybackUpdate(EVENT_KEY_LEFT);
        break;  

        case EVENT_JPGPB_TOUCH_NEXT_PRESS                 :
          	MediaPlaybackConfig(PLAYBACK_NEXT);
			JpgPlayback_Play();
			DrawStateJpgPlaybackUpdate(EVENT_KEY_RIGHT);
        break;
	#ifdef NET_SYNC_PLAYBACK_MODE
		case EVENT_NET_ENTER_PLAYBACK : 
			PhotoPBFunc_SetZoomLock(AHC_FALSE); 
			PhotoPBTimer_Stop();
			JpgPlaybackParamReset();
			AHC_UF_GetCurrentIndex(&ObjSelect);
            StateSwitchMode(UI_NET_PLAYBACK_STATE);
            CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
        break;
    #endif//NET_SYNC_PLAYBACK_MODE
        case EVENT_VIDEO_PREVIEW                       :
        	PhotoPBFunc_SetZoomLock(AHC_FALSE); 
			PhotoPBTimer_Stop();
			JpgPlaybackParamReset();
			ret = StateSwitchMode(UI_VIDEO_STATE);
        break; 

        case EVENT_CAMERA_PREVIEW                       :
        	PhotoPBFunc_SetZoomLock(AHC_FALSE);
			PhotoPBTimer_Stop();
			JpgPlaybackParamReset();
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
				PhotoPBTimer_Stop();
				JpgPlaybackParamReset();
				ret = StateSwitchMode(UI_BROWSER_STATE);  
			}	
        break;

        case EVENT_LOCK_FILE_M   : 
        
        	if(AHC_CheckCurFileLockStatus())
        	{
				ret = QuickFileOperation(DCF_FILE_NON_READ_ONLY);//Unprotect
				bLockCurJpegFile = AHC_FALSE;
			}
			else
			{
				ret = QuickFileOperation(DCF_FILE_READ_ONLY);;//Protect
        		bLockCurJpegFile = AHC_TRUE;
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
			m_ubShowJpgPBTouchPage ^= 1;
			
			if(m_ubShowJpgPBTouchPage)
			{
			    KeyParser_ResetTouchVariable();
        		KeyParser_TouchItemRegister(&JpgPBCtrlPage_TouchButton[0], ITEMNUM(JpgPBCtrlPage_TouchButton));

				DrawStateJpgPbPageSwitch(TOUCH_CTRL_PAGE);
			}
			else
			{
			    KeyParser_ResetTouchVariable();
        		KeyParser_TouchItemRegister(&JpgPBMainPage_TouchButton[0], ITEMNUM(JpgPBMainPage_TouchButton));
			
				DrawStateJpgPbPageSwitch(TOUCH_MAIN_PAGE);	
			}
		break;
#endif

#if (SLIDESHOW_EN==1)
        case EVENT_SLIDESHOW_MODE:
		{		
			UINT32 MaxObjIdx;
			extern AHC_BOOL	bSlideShowFirstFile;
			
			if(AHC_FALSE == AHC_SDMMC_GetMountState()) 
			{
				AHC_WMSG_Draw(AHC_TRUE, WMSG_NO_CARD, 2);
				break;
			}
			
			AHC_UF_GetTotalFileCount(&MaxObjIdx);
             	
			if(MaxObjIdx == 0)
				break;
			
			AHC_UF_GetCurrentIndex(&ObjSelect);
			bSlideShowFirstFile = AHC_FALSE;
			
			ret = StateSwitchMode(UI_SLIDESHOW_STATE);
        }
        break;
#endif 

		//Device
		case EVENT_DC_CABLE_IN:
			#if (CHARGER_IN_ACT_OTHER_MODE==ACT_RECORD_VIDEO)
			PhotoPBFunc_SetZoomLock(AHC_FALSE); 
			PhotoPBTimer_Stop();
			JpgPlaybackParamReset();
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
        		PhotoPBFunc_SetZoomLock(AHC_FALSE); 
				PhotoPBTimer_Stop();
				JpgPlaybackParamReset();
				ret = StateSwitchMode(UI_VIDEO_STATE);
				AHC_SetRecordByChargerIn(3);
				#elif (CHARGER_IN_ACT_OTHER_MODE == ACT_FORCE_POWER_OFF)
				SetKeyPadEvent(KEY_POWER_OFF);
				#endif
			}
			else
			{
				PhotoPBFunc_SetZoomLock(AHC_FALSE);
             	PhotoPBTimer_Stop();
                JpgPlaybackParamReset();
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
			PhotoPBFunc_SetZoomLock(AHC_FALSE);
			PhotoPBTimer_Stop();
			JpgPlaybackParamReset();
			AHC_UF_GetCurrentIndex(&ObjSelect);
        	 	
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
				PhotoPBFunc_SetZoomLock(AHC_FALSE);
				PhotoPBTimer_Stop();
				JpgPlaybackParamReset();
				SetTVState(AHC_TV_PHOTO_PB_STATUS);
				StateSwitchMode(UI_TVOUT_STATE);
			}
        break; 
        
       	case EVENT_TV_REMOVED                     :
        break;
        
        case EVENT_HDMI_DETECT                    : 
			PhotoPBFunc_SetZoomLock(AHC_FALSE);
			PhotoPBTimer_Stop();
			JpgPlaybackParamReset();
			SetHDMIState(AHC_HDMI_PHOTO_PB_STATUS);
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
        case EVENT_JPGPB_UPDATE_MESSAGE           :
			DrawStateJpgPlaybackUpdate(ulJobEvent);
        break;
                
        default:
        break;
    }
}

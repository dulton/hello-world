/*===========================================================================
 * Include files
 *===========================================================================*/
#include "Customer_config.h"

#include "AHC_Common.h"
#include "AHC_General.h"
#include "AHC_Message.h"
#include "AHC_Display.h"
#include "AHC_Audio.h"
#include "AHC_Capture.h"
#include "AHC_Dcf.h"
#include "AHC_UF.h"
#include "AHC_Media.h"
#include "AHC_Video.h"
#include "AHC_Menu.h"
#include "AHC_Os.h"
#include "AHC_Fs.h"
#include "AHC_Isp.h"
#include "AHC_Parameter.h"
#include "AHC_Warningmsg.h"
#include "AHC_Usb.h"
#include "AHC_General_CarDV.h"
#include "AHC_General.h"
#include "AIHC_Video.h"
#include "AIHC_Dcf.h"
#include "IconPosition.h"
#include "ZoomControl.h"
#include "StateVideoFunc.h"
#include "StateHDMIFunc.h"
#include "StateTVFunc.h"
#include "MenuCommon.h"
#include "MenuDrawCommon.h"
#include "MenuTouchButton.h"
#include "MenuSetting.h"
#include "DrawStateVideoFunc.h"
#include "DrawStateCameraFunc.h"
#include "KeyParser.h"
#include "LedControl.h"
#include "dsc_charger.h"
#include "lib_retina.h"
#if (GSENSOR_CONNECT_ENABLE)
#include "GSensor_ctrl.h"
#endif
#if (GPS_CONNECT_ENABLE)
#include "GPS_ctl.h"
#endif
#if (TOUCH_UART_FUNC_EN)
#include "Touch_Uart_ctrl.h"
#endif
#include "disp_drv.h"
#include "MenuDrawingFunc.h"
#include "mmps_iva.h"
#include "mmps_fs.h"
#include "mmps_3gprecd.h" // for MMPS_H264ENC_BASELINE_PROFILE
#include "mmps_sensor.h"
#include "AHC_USBHost.h"


//FIXME AMN included!!! somebody please remove this without leaving warnings.
//#include "amn_osal.h"
#include "AHC_Stream.h"
#if defined(WIFI_PORT) && (WIFI_PORT == 1)
#include "amnss_mmobjs.h"
#include "wlan.h"
#include "netapp.h"
#endif

#if (ENABLE_ADAS_LDWS)
#include "ldws_cfg.h"
#include "mmps_aui.h"
#endif
#include "hdr_cfg.h"
#include "AHC_Sensor.h"

#if 1//(DRIVING_SAFETY_FATIGUE_ALERT)||(DRIVING_SAFETY_REMIND_HEADLIGHT)
#include "Driving_Safety.h"
#endif

#if (USB_EN)&&(SUPPORT_USB_HOST_FUNC)
#include "mmpf_usbh_uvc.h"
#endif
/* Lock File functions */
//  Export functions
AHC_BOOL VideoFunc_LockFileEnabled(void);
//  Internal Functions & Variables
static AHC_BOOL     _bLockVRFile = AHC_FALSE;
static int          _nLockType;
static void EnableLockFile(AHC_BOOL bEnable, int type);
static void LockFileTypeChange(int arg);
/* Lock File functions */

// The definition is at mmpf_3gpmgr.c also.

/*===========================================================================
 * Macro define
 *===========================================================================*/
#define PARKING_RECORD_FORCE_20SEC              (0)

#define STORAGE_MIN_SIZE                    (2*1024*1024)
#define PRE_RECORD_STORAGE_MIN_SIZE         (4*1024*1024)
#define VIDEO_TIMER_UNIT                    (100)//unit :ms
#define TICKS_PER_SECOND                    (1000)
#define TICKS_PER_MINUTE                    (60 * 1000)

#define MOTION_DETECTION_STABLE_TIME        ((SUPPORT_PARKINGMODE == PARKINGMODE_NONE) ? 10 : 1)        // second // This time is used to wait AE stable after enter Pre-recording mode

/*===========================================================================
 * Global variable
 *===========================================================================*/

MOVIESIZE_SETTING           VideoRecSize            = MOVIE_SIZE_NUM;
static AHC_BOOL             bVideoRecording         = AHC_FALSE;
UINT8                       bVideoPreRecordStatus   = AHC_FALSE;
AHC_BOOL                    bAudioRecording         = AHC_FALSE;
UINT8                       VideoTimerID            = 0xFF;
UINT32                      VideoCounter            = 0;
UINT32                      RecordTimeOffset        = 0;
AHC_BOOL                    bShowHdmiWMSG           = AHC_TRUE;
AHC_BOOL                    bShowTvWMSG             = AHC_TRUE;

AHC_BOOL                    bMuteRecord             = AHC_FALSE;
AHC_BOOL                    bDisableVideoPreRecord  = AHC_FALSE;
UINT32                      m_uiSlowMediaCBCnt      = 0;
AHC_BOOL                    bNightMode              = AHC_FALSE;
AHC_BOOL                    bGPS_PageExist          = AHC_FALSE;

#if (LIMIT_MAX_LOCK_FILE_NUM)
UINT32                      m_ulLockFileNum         = 0;
UINT32                      m_ulLockEventNum        = 0;
#endif

#if (LIMIT_MAX_LOCK_FILE_TIME)
UINT32                      ulVRTotalTime           = 0;
#endif

#if (VIDEO_DIGIT_ZOOM_EN==1)
static AHC_BOOL             bZoomStop               = AHC_TRUE;
#endif

#if (GSENSOR_CONNECT_ENABLE || GPS_CONNECT_ENABLE)
static UINT32               ulCounterForGpsGsnrUpdate;
#endif

#if (MOTION_DETECTION_EN)
AHC_BOOL                    m_ubInRemindTime        = AHC_FALSE;
#ifdef CFG_REC_CUS_VMD_REMIND_TIME
UINT32                      m_ulVMDRemindTime       = CFG_REC_CUS_VMD_REMIND_TIME;
#else
UINT32                      m_ulVMDRemindTime       = 10;
#endif
UINT32						m_ulVMDCloseBacklightTime  = 41;
AHC_BOOL                    m_ubVMDStart            = AHC_FALSE;
AHC_BOOL                    m_ulVMDCancel           = AHC_FALSE;
static UINT32               m_ulVMDStableCnt        = 0;  // This counter is used to wait AE stable after enter Pre-recording mode
static UINT32               m_ulVMDStopCnt          = 0;
#endif
#ifdef CFG_ENABLE_VIDEO_LASER_LED
UINT32                      m_ulLaserTime           = 10*10; //10s
AHC_BOOL                    m_ubLaserStart          = AHC_FALSE;
static UINT32               m_ubLaserStop           = AHC_FALSE;
#endif

#if (VR_PREENCODE_EN)
AHC_BOOL                    m_ubPreRecording        = AHC_FALSE;
AHC_BOOL                    m_ubPreEncodeEn         = AHC_FALSE;
#endif

#if (SUPPORT_TOUCH_PANEL)
AHC_BOOL                    m_ubShowVRTouchPage     = AHC_FALSE;
#endif

#if (SUPPORT_GSENSOR && POWER_ON_BY_GSENSOR_EN)
UINT32                      m_ulGSNRRecStopCnt      = POWER_ON_GSNR_MOVIE_TIME * 10;
#endif

AHC_BOOL                    m_ubParkingModeRecTrigger = AHC_FALSE;
#if ((EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_DUAL_FILE)||\
     (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE))
static UINT32               m_ulEventPreRecordTime = 0;
static UINT32               m_ulEventHappenTime = 0;
#endif
AHC_BOOL m_bCurrentTimeLessThanPreRecord = AHC_FALSE;
#if (ENABLE_ADAS_LDWS)
UINT32						m_ulLDWSWarn = 0;
#endif
#if (ENABLE_ADAS_FCWS)
UINT32						m_ulFCWSWarn = 0;
#endif

#if (UVC_HOST_VIDEO_ENABLE || SUPPORT_DUAL_SNR)
static UINT8                gbWinExchangedCnt = F_LARGE_R_SMALL;
#endif

#if (UPDATE_UI_USE_MULTI_TASK)
AHC_BOOL                    m_ubUpdateUiByOtherTask = AHC_FALSE;
#endif

/*===========================================================================
 * Extern variable
 *===========================================================================*/

extern UINT8                bZoomDirect;
extern AHC_BOOL             bForce_PowerOff;
extern AHC_BOOL             bPreviewModeTVout;
extern AHC_BOOL             bPreviewModeHDMI;
extern MMP_BOOL             CalibrationMode;
extern UINT8                m_ubDSCMode;
extern UINT8                m_ubSDMMCStatus;
extern UINT8				m_ubFrameRate;
#if (AUTO_HIDE_OSD_EN)
extern AHC_BOOL             m_ubHideOSD;
extern UINT32               m_ulOSDCounter;
#endif

#if (MOTION_DETECTION_EN)
extern AHC_BOOL             m_ubMotionDtcEn;
#endif

#if (GPS_CONNECT_ENABLE)
extern GPS_ATTRIBUTE        gGPS_Attribute;
#endif

#if (SUPPORT_GSENSOR && POWER_ON_BY_GSENSOR_EN)
extern AHC_BOOL             ubGSnrPowerOn;
extern AHC_BOOL             ubGsnrPwrOnActStart;
extern AHC_BOOL             ubGsnrPwrOnFirstREC;
extern AHC_BOOL             m_ubGsnrIsObjMove;
#endif

#if (GSENSOR_CONNECT_ENABLE)
extern AHC_BOOL             m_ubGsnrIsObjMove;
#endif
extern AHC_TV_STATUS 	    m_TVStatus;
extern AHC_BOOL             gbAhcDbgBrk;
#if (USB_MODE_SELECT_EN)
extern UINT8	 ubUSBSelectedMode;
#endif

/*===========================================================================
 * Extern Function
 *===========================================================================*/
#if (SUPPORT_GSENSOR && POWER_ON_BY_GSENSOR_EN)
extern UINT32               AHC_GSNR_PWROn_MovieTimeReset(void);
#endif

extern void Oem_Switch_To_TVOUT_Mode(void);

/*===========================================================================
 * Main body
 *===========================================================================*/
extern void SetUITimeEvent(void);
#if (UPDATE_UI_USE_MULTI_TASK)
extern void SetUIUpdateEvent(void);
#endif

//ALERT_TYPE CheckAlertState(void)
//{
//	static ALERT_TYPE AlertState = ALERT_NON;
//	#if (ENABLE_ADAS_LDWS)
//	UINT32 bLDWS_En;
//	#endif
//	#if (ENABLE_ADAS_FCWS)
//	UINT32 bFCWS_En;
//	#endif
//
//	if (AlertState == ALERT_NON) {
//		#if (DRIVING_SAFETY_FATIGUE_ALERT)
//		if (m_bDriverFatigueTime && (MenuSettingConfig()->uiFatigueAlert == FATIGUE_ALERT_ON)) {
//			AlertState = ALERT_DRIVER_FATIGUE;
//		}
//		#endif
//		#if (DRIVING_SAFETY_REMIND_HEADLIGHT)
//		if (m_bRemindHeadLightTime && Driving_Safety_GetState_RemindHeadlightEnable()) {
//			AlertState = ALERT_REMIND_HEADLIGHT;
//		}
//		#endif
//		#if (ENABLE_ADAS_LDWS)
//		#ifdef CFG_ADAS_MENU_SETTING_OLD_STYLE
//		if (m_ulLDWSWarn && (MenuSettingConfig()->uiLDWS == REMIND_LDWS_ON)) {
//			AlertState = ALERT_LDWS;
//		}
//		#else
//		printc("[6]");
//		if (AHC_Menu_SettingGetCB((char *)COMMON_KEY_LDWS_EN, &bLDWS_En) == AHC_TRUE) {
//			if ((LDWS_EN_ON == bLDWS_En)) {
//				printc("[7]");
//				AlertState = ALERT_LDWS;
//			}
//		}
//		#endif
//		#endif
//		#if (ENABLE_ADAS_FCWS)
//		#ifdef CFG_ADAS_MENU_SETTING_OLD_STYLE
//		if (m_ulFCWSWarn && (MenuSettingConfig()->uiFCWS == REMIND_FCWS_ON)) {
//				AlertState = ALERT_FCWS;
//		}
//		#else
//		printc("[8]");
//		if (AHC_Menu_SettingGetCB((char *)COMMON_KEY_FCWS_EN, &bFCWS_En) == AHC_TRUE) {
//			if ((FCWS_EN_ON == bFCWS_En)) {
//				printc("[9]");
//				AlertState = ALERT_FCWS;
//			}
//		}
//		#endif
//		#endif
//		#if (MENU_MOVIE_SPEED_CAR_EN)
//		if ((CheckSpeedStatus() == SPEED_SPEEDCAM) &&
//			(MenuSettingConfig()->uiSpeedCar == SPEED_CAR_ON)) {
//			return ALERT_SPEED_CAM;
//		}
//		#endif
//		#if (MENU_MOVIE_SPEED_LIMIT_ALERT_EN)
//		if ((CheckSpeedStatus() == SPEED_CRUISESPEED) &&
//			(MenuSettingConfig()->uiSpeedCam_CruiseSpeedSetting != SPEED_CAM_CRUISE_SPEED_OFF)) {
//			return ALERT_SPEED_LIMIT_CAM;
//		}
//		#endif
//	}
//	else { // AlertState != ALERT_NON
//		#if (DRIVING_SAFETY_FATIGUE_ALERT)
//		if (AlertState == ALERT_DRIVER_FATIGUE) {
//			if (m_bDriverFatigueTime && (MenuSettingConfig()->uiFatigueAlert == FATIGUE_ALERT_ON)) {
//				AlertState = ALERT_DRIVER_FATIGUE;
//			}
//			else {
//				AlertState = ALERT_NON;
//				return AlertState;
//			}
//		}
//		#endif
//		#if (DRIVING_SAFETY_REMIND_HEADLIGHT)
//		if (AlertState == ALERT_REMIND_HEADLIGHT) {
//			if (m_bRemindHeadLightTime && (Driving_Safety_GetState_RemindHeadlightEnable())) {
//				AlertState = ALERT_REMIND_HEADLIGHT;
//			}
//			else {
//				AlertState = ALERT_NON;
//				return AlertState;
//			}
//		}
//		#endif
//		#if (ENABLE_ADAS_LDWS)
//		if (AlertState == ALERT_LDWS) {
//			#ifdef CFG_ADAS_MENU_SETTING_OLD_STYLE
//			if(m_ulLDWSWarn && (MenuSettingConfig()->uiLDWS == REMIND_LDWS_ON)) {
//				AlertState = ALERT_LDWS;
//			}
//			else {
//				AlertState = ALERT_NON;
//				return AlertState;
//			}
//			#else
//			printc("[10]");
//			if (AHC_Menu_SettingGetCB((char *)COMMON_KEY_LDWS_EN, &bLDWS_En) == AHC_TRUE) {
//				if ((LDWS_EN_ON == bLDWS_En)) {
//					printc("[11]");
//					AlertState = ALERT_LDWS;
//				}
//				else {
//					printc("[12]");
//					AlertState = ALERT_NON;
//					return AlertState;
//				}
//			}
//			#endif
//		}
//		#endif
//		#if (ENABLE_ADAS_FCWS)
//		if (AlertState == ALERT_FCWS) {
//			#ifdef CFG_ADAS_MENU_SETTING_OLD_STYLE
//			if (m_ulFCWSWarn && (MenuSettingConfig()->uiFCWS == REMIND_FCWS_ON)) {
//				AlertState = ALERT_FCWS;
//			}
//			else {
//				AlertState = ALERT_NON;
//				return AlertState;
//			}
//			#else
//			if (AHC_Menu_SettingGetCB((char *)COMMON_KEY_FCWS_EN, &bFCWS_En) == AHC_TRUE) {
//				if ((FCWS_EN_ON == bFCWS_En)) {
//					AlertState = ALERT_FCWS;
//				}
//				else {
//					AlertState = ALERT_NON;
//					return AlertState;
//				}
//			}
//			#endif
//		}
//		#endif
//		#if (MENU_MOVIE_SPEED_CAR_EN)
//		if (AlertState == ALERT_SPEED_CAM) {
//			if ((CheckSpeedStatus() == SPEED_SPEEDCAM) &&
//				(MenuSettingConfig()->uiSpeedCar == SPEED_CAR_ON)) {
//				AlertState = ALERT_SPEED_CAM;
//			}
//			else {
//				AlertState = ALERT_NON;
//				return AlertState;
//			}
//		}
//		#endif
//		#if (MENU_MOVIE_SPEED_LIMIT_ALERT_EN)
//		if (AlertState == ALERT_SPEED_LIMIT_CAM) {
//			if ((CheckSpeedStatus() == SPEED_CRUISESPEED) &&
//				(MenuSettingConfig()->uiSpeedCam_CruiseSpeedSetting != SPEED_CAM_CRUISE_SPEED_OFF)) {
//				AlertState = ALERT_SPEED_LIMIT_CAM;
//			}
//			else {
//				AlertState = ALERT_NON;
//				return AlertState;
//			}
//		}
//		#endif
//	}
//
//	return AlertState;
//}

//AHC_BOOL CheckAlertNonState(void)
//{
//	if (CheckAlertState() == ALERT_NON)
//		return AHC_TRUE;
//	else
//		return AHC_FALSE;
//}

#if (ENABLE_ADAS_LDWS)
void ResetLDWSCounter(void)
{
	m_ulLDWSWarn = 0;
}
#endif

#if (ENABLE_ADAS_FCWS)
void ResetFCWSCounter(void)
{
	m_ulFCWSWarn = 0;
}
#endif

void VideoTimer_ISR(void *tmr, void *arg)
{
    /*
     * if want to display current time/recording time and update it by every second.
     * not to define CFG_REC_CUS_EVENT_PERIOD.
     *                                          Canlet
     */
    {
        static unsigned int     _kt      = 0;
        AHC_RTC_TIME            sRtcTime;

        AHC_RTC_GetTime(&sRtcTime);

        if (_kt != sRtcTime.uwSecond) {
            _kt = sRtcTime.uwSecond;
            // time updated to compensate timer error by Shadow RTC
            VideoCounter = 0;
            SetUITimeEvent();
        }
    }

#if (ENABLE_ADAS_LDWS)
	{
	    #if 0
		if (LDWS_IsStart() == AHC_TRUE)
		{
			m_ulLDWSWarn++;
			if (m_ulLDWSWarn == (4*5)) // period = 250msec.
			{
				m_ulLDWSWarn = 0;
				//LDWS_Unlock();
				AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_LDWS_STOP, 0);
			}
		}
		#endif

		if(m_ulLDWSWarn == 1) {
			m_ulLDWSWarn = 0;
			AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_LDWS_STOP, 0);
		} else if(m_ulLDWSWarn > 0) {
			m_ulLDWSWarn--;
		}
	}
#endif

#if (ENABLE_ADAS_FCWS)
	if(m_ulFCWSWarn == 1) {
		m_ulFCWSWarn = 0;
		AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_FCWS_STOP, 0);
	} else if(m_ulFCWSWarn > 0) {
		m_ulFCWSWarn--;
	}
#endif

    #ifdef CFG_ENABLE_VIDEO_LASER_LED
    if((!m_ubLaserStart)&&(VideoFunc_RecordStatus()))
    {
        RTNA_DBG_Str(0, "m_ubLaserStart \r\n");
        AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_FUNC2_LPRESS, 0);
        m_ubLaserStart = AHC_TRUE;
    }
    else if((VideoFunc_RecordStatus())&&(m_ulLaserTime>0)&&(m_ubLaserStart))
    {
        m_ulLaserTime--;
        if(m_ulLaserTime==0)
        {
            m_ubLaserStop = AHC_TRUE;
            RTNA_DBG_Str(0, "m_ubLaserStop \r\n");
            AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_FUNC2_LPRESS, 0);
        }
    }
    #endif

    #if defined(CFG_WIFI_TIMEOUT_OFF) && defined(WIFI_PORT) && (WIFI_PORT == 1)
    {
        static  MMP_USHORT cnt;

        if ((get_NetAppStatus() != 0) && (NETAPP_WIFI_NONE != get_NetAppStatus()))
        {
            if (AHC_CheckWiFiOnOffInterval(ulWiFiSwitchToggleInterval) == 0)
            {
                if (AHC_Get_WiFi_Streaming_Status())
                {
                    cnt = 0;
                }
                else
                {
                    cnt++;
                    if (cnt>(10*3*60))  //100*10ms
                    {  // TurnOffWiFiModule
                        //AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_REC_REL, 0);  // TBD
                        cnt = 0;
                    }
                }
            }
        }
        else
        {
            cnt = 0;
        }
    }
    #endif

    if( VideoCounter == 0)
    {
        #ifdef CFG_REC_CUS_EVENT_PERIOD //may be defined in config_xxx.h, must not == 0
        VideoCounter = CFG_REC_CUS_EVENT_PERIOD;
        #else
        VideoCounter = 5;
        #endif
		#if (UPDATE_UI_USE_MULTI_TASK)
		if(m_ubUpdateUiByOtherTask)
			SetUIUpdateEvent();
		else
			AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_UPDATE_MESSAGE, 0);
		#else
        AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_UPDATE_MESSAGE, 0);
		#endif
		#if (TOUCH_UART_FUNC_EN)
		#if (DTV_UART_SEND_LDWS)
		if(DTV_UART_Get_LDWS())	
			AHC_OS_SetFlags(UartCtrlFlag, Touch_Uart_FLAG_SETPACK, AHC_OS_FLAG_SET);
		#endif //#if (DTV_UART_SEND_LDWS)		
		
		#if (DTV_UART_SEND_FCWS)
		if(DTV_UART_Get_FCWS())	
			AHC_OS_SetFlags(UartCtrlFlag, Touch_Uart_FLAG_SETPACK, AHC_OS_FLAG_SET);		
		#endif  //#if (DTV_UART_SEND_FCWS)
		#endif  //#if (TOUCH_UART_FUNC_EN)
        #ifdef VMD_EN_BY_CHARGER_OUT
        if(m_ubMotionDtcEn)
        {
            AutoPowerOffCounterReset();
            LCDPowerSaveCounterReset();
            VideoPowerOffCounterReset();
        }
        #endif
    }

    VideoCounter--;

#if((SUPPORT_GSENSOR && POWER_ON_BY_GSENSOR_EN) && \
    (GSNR_PWRON_REC_BY && (GSNR_PWRON_REC_BY_SHAKED || GSNR_PWRON_REC_BY_VMD)))
    if(ubGsnrPwrOnActStart)
    {
        LCDPowerSaveCounterReset();
        VideoPowerOffCounterReset();
        if(m_ulGSNRRecStopCnt && VideoFunc_RecordStatus())
        {
            m_ulGSNRRecStopCnt--;

            if(m_ulGSNRRecStopCnt==0)
            {
                if(POWER_ON_GSNR_IDLE_TIME == 0)
                {
                    bForce_PowerOff = AHC_TRUE;
                    AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, KEY_POWER_OFF, 0);
                    m_ulGSNRRecStopCnt = 0;
                }
                else
                {
                    AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, KEY_VIDEO_RECORD, EVENT_VRCB_MOTION_DTC);
                    AutoPowerOffCounterReset();
                }
            }
        }
    }
    else
#endif
    {
        #if (MOTION_DETECTION_EN) && (VMD_ACTION & VMD_RECORD_VIDEO)
        if (m_ulVMDStopCnt && m_ubVMDStart)
        {
            m_ulVMDStopCnt--;

            if (m_ulVMDStopCnt == 0) {
                // Stop Video Record in VMD mode.
                // Please make sure EVENT - BUTTON_REC_PRESS is to stop video recording
                AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, KEY_VIDEO_RECORD, EVENT_VRCB_MOTION_DTC);
            }
        }
        #endif

        #if (MOTION_DETECTION_EN)
        if( m_ulVMDStableCnt > 0 )
        {
            m_ulVMDStableCnt--;
            if( m_ulVMDStableCnt == 0 )
            {
                m_ubVMDStart = AHC_TRUE; // Really start to detect motions
            }
        }
        #endif
    }

    #if ( ((GPS_CONNECT_ENABLE) && (GPS_FUNC & FUNC_VIDEOSTREM_INFO))     || \
          ((GSENSOR_CONNECT_ENABLE) && (GSENSOR_FUNC & FUNC_VIDEOSTREM_INFO)) )
    if((VideoFunc_RecordStatus() || uiGetParkingModeEnable() == AHC_TRUE || AHC_IsEmerRecordStarted() == AHC_TRUE) && AHC_IsSDInserted())
    {
        AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_GPSGSENSOR_UPDATE, 0);
    }
    else
    {
        if(AHC_IsSDInserted())
            ulCounterForGpsGsnrUpdate = 0;
    }
    #endif
}

AHC_BOOL VideoTimer_Start(UINT32 ulTime)
{
    if (0xFE <= VideoTimerID) {
        VideoCounter = 0;
        VideoTimerID = AHC_OS_StartTimer( ulTime, AHC_OS_TMR_OPT_PERIODIC, VideoTimer_ISR, (void*)NULL );
		printc(FG_GREEN("VideoTimer_Start: VideoTimerID =0x%X\r\n"), VideoTimerID);
        if(0xFE <= VideoTimerID) {
            printc(FG_RED("Start Video Timer fail - 0x%X!!!")"\r\n", VideoTimerID);
            return AHC_FALSE;
        }
    }

    #ifdef CFG_CUS_VIDEO_TIMER_START
    CFG_CUS_VIDEO_TIMER_START();
    #endif

    return AHC_TRUE;
}

AHC_BOOL VideoTimer_Stop(void)
{
    UINT8 ret = 0;

    #ifdef CFG_CUS_VIDEO_TIMER_STOP
    CFG_CUS_VIDEO_TIMER_STOP();
    #endif

    if (0xFE > VideoTimerID) {
        printc(FG_GREEN("VideoTimer_Stop: VideoTimerID =0x%X\r\n"), VideoTimerID);
        
        ret = AHC_OS_StopTimer( VideoTimerID, AHC_OS_TMR_OPT_PERIODIC );
        VideoTimerID = 0xFF;

        if(0xFF == ret) {
            printc(FG_RED("Stop Video Timer fail !!!")"\r\n");
            return AHC_FALSE;
        }
    }

    return AHC_TRUE;
}

AHC_BOOL VideoFunc_PreRecordStatus(void)
{
#if (VR_PREENCODE_EN)
    return m_ubPreRecording;
#else
    return AHC_FALSE;
#endif
}

AHC_BOOL VideoFunc_RecordStatus(void)
{
    return bVideoRecording;
}

AHC_BOOL VideoFunc_GPSPageStatus(void)
{
    return bGPS_PageExist;
}

UINT32 VideoFunc_GetRecordTimeOffset(void)
{
    return RecordTimeOffset;
}

void VideoFunc_GetFreeSpace(UINT64 *pFreeBytes)
{
    AHC_MEDIA_ID        CurMedia;
    AHC_ERR             status;

    CurMedia = AHC_GetMediaPath();
    status   = AHC_FS_GetStorageFreeSpace(CurMedia, pFreeBytes);

    if(AHC_ERR_NONE != status)
    {
        AHC_DisMountStorageMedia(AHC_MEDIA_MMC);
        *pFreeBytes = 0;
        printc("AHC_FS_GetStorageFreeSpace Error!!!!\r\n");
    }
}

void VideoFunc_SetResolution(MOVIESIZE_SETTING ubResolution)
{
    VideoRecSize = ubResolution;
    printc("[VideoFunc_SetResolution] VideoRecSize = %d\r\n",VideoRecSize);
    switch(VideoRecSize)
    {
	#if (MENU_MOVIE_SIZE_1440_30P_EN)
	case MOVIE_SIZE_1440_30P:
		AHC_SetImageSize(VIDEO_CAPTURE_MODE, 2560, 1440);
	break;
	#endif
    #if (MENU_MOVIE_SIZE_SHD_30P_EN)
        case MOVIE_SIZE_SHD_30P:
            AHC_SetImageSize(VIDEO_CAPTURE_MODE, 2304, 1296);
        break;
    #endif
	#if (MENU_MOVIE_SIZE_SHD_25P_EN)
        case MOVIE_SIZE_SHD_25P:
            AHC_SetImageSize(VIDEO_CAPTURE_MODE, 2304, 1296);
        break;
    #endif
      #if (MENU_MOVIE_SIZE_SHD_24P_EN)
        case MOVIE_SIZE_SHD_24P:
            AHC_SetImageSize(VIDEO_CAPTURE_MODE, 2304, 1296);
        break;
    #endif
     #if (MENU_MOVIE_SIZE_1080_60P_EN)
        case MOVIE_SIZE_1080_60P:
            AHC_SetImageSize(VIDEO_CAPTURE_MODE, 1920, 1088);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_1080_24P_EN)
       case MOVIE_SIZE_1080_24P:
           AHC_SetImageSize(VIDEO_CAPTURE_MODE, 1920, 1088);
       break;
    #endif
    #if (MENU_MOVIE_SIZE_1080P_EN)
        case MOVIE_SIZE_1080P:
            AHC_SetImageSize(VIDEO_CAPTURE_MODE, 1920, 1088);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_720P_EN)
        case MOVIE_SIZE_720P:
            AHC_SetImageSize(VIDEO_CAPTURE_MODE, 1280, 720);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_720_60P_EN)
        case MOVIE_SIZE_720_60P:
            AHC_SetImageSize(VIDEO_CAPTURE_MODE, 1280, 720);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_720_24P_EN)
        case MOVIE_SIZE_720_24P:
            AHC_SetImageSize(VIDEO_CAPTURE_MODE, 1280, 720);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_VGA30P_EN)
        case MOVIE_SIZE_VGA_30P:
            AHC_SetImageSize(VIDEO_CAPTURE_MODE, 640, 480);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_VGA120P_EN)
        case MOVIE_SIZE_VGA_120P:
            AHC_SetImageSize(VIDEO_CAPTURE_MODE, 640, 480);
        break;
    #endif
        case MOVIE_SIZE_360_30P:
            AHC_SetImageSize(VIDEO_CAPTURE_MODE, 640, 368);
        break;
        default:
        break;
    }
}

void VideoFunc_PresetSensorMode(MOVIESIZE_SETTING ubResolution)
{
    AHC_BOOL ahc_ret = AHC_FALSE;

    switch(ubResolution)
    {
    #if (MENU_MOVIE_SIZE_1440_30P_EN)
        case MOVIE_SIZE_1440_30P:
            ahc_ret = AHC_PresetSensorMode(AHC_SENSOR_MODE_1440_30P_RESOLUTION);
        break;
    #endif			
    #if (MENU_MOVIE_SIZE_SHD_30P_EN)
        case MOVIE_SIZE_SHD_30P:
            ahc_ret = AHC_PresetSensorMode(AHC_SENSOR_MODE_SUPER_HD_30P_RESOLUTION);
        break;
    #endif
	#if (MENU_MOVIE_SIZE_SHD_25P_EN)
        case MOVIE_SIZE_SHD_25P:
            ahc_ret = AHC_PresetSensorMode(AHC_SENSOR_MODE_SUPER_HD_25P_RESOLUTION);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_SHD_24P_EN)
        case MOVIE_SIZE_SHD_24P:
            ahc_ret = AHC_PresetSensorMode(AHC_SENSOR_MODE_SUPER_HD_24P_RESOLUTION);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_1080_60P_EN)
        case MOVIE_SIZE_1080_60P:
            ahc_ret = AHC_PresetSensorMode(AHC_SENSOR_MODE_FULL_HD_60P_RESOLUTION);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_1080P_EN)
        case MOVIE_SIZE_1080P:
            ahc_ret = AHC_PresetSensorMode(AHC_SENSOR_MODE_FULL_HD_30P_RESOLUTION);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_1080_24P_EN)
        case MOVIE_SIZE_1080_24P:
            ahc_ret = AHC_PresetSensorMode(AHC_SENSOR_MODE_FULL_HD_24P_RESOLUTION);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_720P_EN)
        case MOVIE_SIZE_720P:
            ahc_ret = AHC_PresetSensorMode(AHC_SENSOR_MODE_HD_30P_RESOLUTION);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_720_60P_EN)
        case MOVIE_SIZE_720_60P:
            ahc_ret = AHC_PresetSensorMode(AHC_SENSOR_MODE_HD_60P_RESOLUTION);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_720_24P_EN)
        case MOVIE_SIZE_720_24P:
            ahc_ret = AHC_PresetSensorMode(AHC_SENSOR_MODE_HD_24P_RESOLUTION);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_VGA30P_EN)
        case MOVIE_SIZE_VGA_30P:
            ahc_ret = AHC_PresetSensorMode(AHC_SENSOR_MODE_4TO3_VGA_30P_RESOLUTION);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_VGA120P_EN)
        case MOVIE_SIZE_VGA_120P:
            ahc_ret = AHC_PresetSensorMode(AHC_SENSOR_MODE_VGA_120P_RESOLUTION);
        break;
    #endif
        case MOVIE_SIZE_360_30P:
            ahc_ret = AHC_PresetSensorMode(AHC_SENSOR_MODE_VGA_30P_RESOLUTION);
        break;
        default:
            printc( FG_RED("VideoFunc_PresetSensorMode: Unsupported resolution - %d\r\n"), ubResolution);
        break;
    }
}

void VideoFunc_PresetFrameRate(MOVIESIZE_SETTING ubResolution)
{
    AHC_BOOL ahc_ret = AHC_FALSE;

    switch(ubResolution)
    {
	#if (MENU_MOVIE_SIZE_1440_30P_EN)
        case MOVIE_SIZE_1440_30P:
            ahc_ret = AHC_ConfigMovie(AHC_FRAME_RATE, 30);
        break;
	#endif
    #if (MENU_MOVIE_SIZE_SHD_30P_EN)
        case MOVIE_SIZE_SHD_30P:
            ahc_ret = AHC_ConfigMovie(AHC_FRAME_RATE, 30);
        break;
    #endif
	#if (MENU_MOVIE_SIZE_SHD_25P_EN)
        case MOVIE_SIZE_SHD_25P:
            ahc_ret = AHC_ConfigMovie(AHC_FRAME_RATE, 25);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_SHD_24P_EN)
        case MOVIE_SIZE_SHD_24P:
            ahc_ret = AHC_ConfigMovie(AHC_FRAME_RATE, 24);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_1080_60P_EN)
        case MOVIE_SIZE_1080_60P:
            ahc_ret = AHC_ConfigMovie(AHC_FRAME_RATE, 60);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_1080_24P_EN)
        case MOVIE_SIZE_1080_24P:
            ahc_ret = AHC_ConfigMovie(AHC_FRAME_RATE, 24);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_1080P_EN)
        case MOVIE_SIZE_1080P:
            ahc_ret = AHC_ConfigMovie(AHC_FRAME_RATE, 30);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_720P_EN)
        case MOVIE_SIZE_720P:
            ahc_ret = AHC_ConfigMovie(AHC_FRAME_RATE, 30);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_720_60P_EN)
        case MOVIE_SIZE_720_60P:
            ahc_ret = AHC_ConfigMovie(AHC_FRAME_RATE, 60);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_720_24P_EN)
        case MOVIE_SIZE_720_24P:
            ahc_ret = AHC_ConfigMovie(AHC_FRAME_RATE, 24);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_VGA30P_EN)
        case MOVIE_SIZE_VGA_30P:
            ahc_ret = AHC_ConfigMovie(AHC_FRAME_RATE, 30);
        break;
    #endif
    #if (MENU_MOVIE_SIZE_VGA120P_EN)
        case MOVIE_SIZE_VGA_120P:
            ahc_ret = AHC_ConfigMovie(AHC_FRAME_RATE, 120);
        break;
    #endif
        case MOVIE_SIZE_360_30P:
            ahc_ret = AHC_ConfigMovie(AHC_FRAME_RATE, 30);
        break;
        default:
            printc( FG_RED("VideoFunc_PresetFrameRate: Unsupported resolution - %d\n"), ubResolution);
        break;
    }
}

VideoRecordStatus VideoFunc_Record(void)
{
    AHC_BOOL    ahc_ret         = AHC_TRUE;
    UINT64      ulFreeSpace     = 0;
    UINT8       Hour, Min, Sec;
    UINT32      ulAudBitRate    = 128000;//128K
    UINT32      ulVidBitRate;
    UINT32      ulTimelimit     = AHC_GetVRTimeLimit();
    UINT64      ulSpaceNeeded   = 0;
    UINT32      MaxDcfObj       = 0;
    UINT32      ulResvSize      = 0;
    UINT32      ulPreRecordedTime = 0;
    AHC_AUDIO_FORMAT aformat;
    AHC_BOOL bEnableSlowMotion = AHC_FALSE;
    AHC_BOOL bTimeLapseEnable  = AHC_FALSE;

    if(AHC_FALSE == AHC_SDMMC_GetMountState())
    {
        printc(FG_RED("No Card: Mount Fail !!!!!!\r\n"));
        return VIDEO_REC_NO_SD_CARD;
    }

    VideoFunc_GetFreeSpace(&ulFreeSpace);
    AIHC_GetMovieConfig(AHC_VIDEO_BITRATE, &ulVidBitRate);

    if(ulTimelimit==NON_CYCLING_TIME_LIMIT)
    {
        // 0 to get time for current setting of bitrate
        AHC_AvailableRecordTime(&Hour, &Min, &Sec);

        if(Hour == 0 && Min == 0 && Sec <= 2)
        {
            printc(FG_RED("Space is not enough for Non-Cycling recording, Card Full!\r\n"));
            return VIDEO_REC_CARD_FULL;
        }
    }
    else
    {
        /* For Cyclic-Record Space Guaranty */
#if (defined(VIDEO_REC_TIMELAPSE_EN) && VIDEO_REC_TIMELAPSE_EN)
        UINT32 slVRTimelapse = 0;		// Off

        if ((AHC_Menu_SettingGetCB(COMMON_KEY_VR_TIMELAPSE, &slVRTimelapse) == AHC_TRUE) && (slVRTimelapse != PRJ_VR_TIMELAPSE_OFF)){
            UINT32      Framerate, Frate;
            printc(" TimeLapse record.....\r\n");

            AIHC_GetMovieConfig(AHC_FRAME_RATE, &Frate);
            Framerate = AHC_GetVideoRealFrameRate(Frate) / AHC_VIDRECD_TIME_INCREMENT_BASE;
            VideoFunc_GetTimeLapseBitrate(Framerate, slVRTimelapse, &ulVidBitRate, &ulAudBitRate);
        }
        else
#endif
        {
            if (MenuSettingConfig()->uiMOVSoundRecord == MOVIE_SOUND_RECORD_OFF)
                ulAudBitRate = 0;
        }

#if ( FS_FORMAT_FREE_ENABLE == 0)
        #if(SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_3)
        if( uiGetParkingModeEnable() == AHC_FALSE )
        {
            if(AHC_Deletion_Romove(AHC_FALSE)==AHC_FALSE)
                return VIDEO_REC_SEAMLESS_ERROR;
        }
        else
        {
            if(AHC_Deletion_RemoveEx(DCF_DB_TYPE_2ND_DB, AHC_GetVRTimeLimit()) == AHC_FALSE)
            {
                if(AHC_Deletion_RemoveEx(DCF_DB_TYPE_2ND_DB, AHC_GetVRTimeLimit()) == AHC_FALSE)
                    return VIDEO_REC_SEAMLESS_ERROR;
            }
        }
        #else
        {
            #if (DELETION_BY_FILE_NUM)
            {
                UINT32 uiFileCount;
                DCF_DB_TYPE sCurDB;
                DCF_DB_TYPE sDB;
                sCurDB = AHC_UF_GetDB();
                #if (DCF_DB_COUNT >= 2)
                if(uiGetParkingModeEnable() == AHC_TRUE) 
                {
                    sDB = DCF_DB_TYPE_2ND_DB;
                }
                else
                #endif
                {                                     
                    sDB = DCF_DB_TYPE_1ST_DB;
                }    
                AHC_UF_SelectDB(sDB);
                AHC_UF_GetTotalFileCount(&uiFileCount);
        	    if (uiFileCount >= AHC_UF_GetFileTH(sDB))
        	    {
        		    if(AHC_Deletion_RemoveEx(sDB, AHC_GetVRTimeLimit()) == AHC_FALSE)
        		    {
        			    printc(FG_RED("AHC_Deletion_Romove Error\r\n"));
        			    return AHC_FALSE;
        		    }
        	    }
                AHC_UF_SelectDB(sCurDB); 
            }
            #endif //#if (DELETION_BY_FILE_NUM)
            AHC_GetVideoRecStorageSpeceNeed(ulVidBitRate, ulAudBitRate, ulTimelimit, &ulSpaceNeeded);

            if( ulFreeSpace < ulSpaceNeeded )
            {
                MaxDcfObj = AHC_GetMappingFileNum(FILE_MOVIE);

                if(MaxDcfObj==0)
                {
                    printc("--E-- Space is not enough for cycle recording !!!\r\nPlease Clean SD card!\r\n");
                    return VIDEO_REC_SEAMLESS_ERROR;
                }
                else
                {
                    printc("-I- Space is not enough for recording, Delete video file first!\r\n");
                    if(AHC_Delete_DcfMinKeyFile(AHC_TRUE, (const char *) AHC_GetVideoExt()) != AHC_TRUE)
                        return VIDEO_REC_SEAMLESS_ERROR;
                }
            }
        }
        #endif
#endif
    }

    //Video
    AHC_ConfigMovie(AHC_VIDEO_CODEC_TYPE        , AHC_MOVIE_VIDEO_CODEC_H264);
    AHC_ConfigMovie(AHC_VIDEO_COMPRESSION_RATIO , MenuSettingConfig()->uiMOVQuality);
    AHC_ConfigMovie(AHC_MAX_PFRAME_NUM          , 14);

    //Audio
#if (VR_AUDIO_TYPE==VR_AUDIO_TYPE_AAC)
    aformat = AHC_MOVIE_AUDIO_CODEC_AAC;
#elif (VR_AUDIO_TYPE==VR_AUDIO_TYPE_MP3)
    aformat = AHC_MOVIE_AUDIO_CODEC_MP3;
#elif (VR_AUDIO_TYPE==VR_AUDIO_TYPE_ADPCM)
    aformat = AHC_MOVIE_AUDIO_CODEC_ADPCM;
#endif

    AHC_ConfigAudio(AHC_AUDIO_STREAM_ID, aformat, AHC_AUDIO_CHANNEL_MONO_R);

    #if (VR_PREENCODE_EN)
    ulResvSize = (m_ubPreEncodeEn)?(STORAGE_MIN_SIZE+PRE_RECORD_STORAGE_MIN_SIZE):(STORAGE_MIN_SIZE);
    #else
    ulResvSize = STORAGE_MIN_SIZE;
    #endif
    AHC_ConfigMovie(AHC_VIDEO_RESERVED_SIZE     , ulResvSize);

    if( uiGetParkingModeEnable() == AHC_TRUE )
    {
        AHC_GetCurrentRecordingTime(&ulPreRecordedTime);
    }

    #if (GPS_CONNECT_ENABLE) && (GPS_FUNC & FUNC_VIDEOSTREM_INFO)
    if( uiGetParkingModeEnable() == AHC_TRUE )
    {
        AHC_GPS_TriggerRestorePreRecordInfo( AHC_TRUE, ulPreRecordedTime, AHC_FALSE );
    }
    else
    {
        GPSCtrl_ResetBufferControlVariable();
    }
    #endif

    #if (GSENSOR_CONNECT_ENABLE) && (GSENSOR_FUNC & FUNC_VIDEOSTREM_INFO)
    if( uiGetParkingModeEnable() == AHC_TRUE )
    {
        AHC_Gsensor_TriggerRestorePreRecordInfo( AHC_TRUE, ulPreRecordedTime, AHC_FALSE );
    }
    else
    {
        AHC_Gsensor_ResetBufCtrlVariable();
    }
    #endif

    #if (GSENSOR_CONNECT_ENABLE)
    {
        extern AHC_BOOL AHC_Gsensor_GetCurIntStatus(AHC_BOOL* bObjMove);

        AHC_BOOL    dummy;
        // Clean GSensor INT status to avoid getting the status of before recording
        AHC_Gsensor_GetCurIntStatus(&dummy);
    }
    #endif

    m_uiSlowMediaCBCnt  = 0;

    {
        int iSlowMotionRatioSetting = pf_SLOWMOTION_EnGet();
        UINT32 uiSlowMotionRatio = 1;
        UINT32 Param, FrameRate, usVopTimeIncrement, usVopTimeIncrResol;

        AIHC_GetMovieConfig(AHC_FRAME_RATE, &Param);
        FrameRate = AHC_GetVideoRealFrameRate(Param);

        usVopTimeIncrement = AHC_VIDRECD_TIME_INCREMENT_BASE;
        usVopTimeIncrResol = FrameRate;

        switch(iSlowMotionRatioSetting){
            case SLOWMOTION_X2:
                uiSlowMotionRatio = 2;
                bEnableSlowMotion = AHC_TRUE;
                break;
            case SLOWMOTION_X4:
                uiSlowMotionRatio = 4;
                bEnableSlowMotion = AHC_TRUE;
                break;
            case SLOWMOTION_X8:
                uiSlowMotionRatio = 8;
                bEnableSlowMotion = AHC_TRUE;
                break;
            case SLOWMOTION_X1:
            default:
                uiSlowMotionRatio = 1;
                break;
        }

        printc(FG_GREEN("SlowMotionRatio:X%x\r\n"), uiSlowMotionRatio);
        AHC_VideoSetSlowMotionFPS(bEnableSlowMotion, AHC_VIDRECD_TIME_INCREMENT_BASE, usVopTimeIncrResol * AHC_VIDRECD_TIME_INCREMENT_BASE / usVopTimeIncrement / uiSlowMotionRatio);
    }

#if (defined(VIDEO_REC_TIMELAPSE_EN) && VIDEO_REC_TIMELAPSE_EN == 1)
    {
        int iTimeLapseSetting;
        UINT32 uiTimeLapseRatio = 1;
        UINT32 Param, FrameRate, usVopTimeIncrement, usVopTimeIncrResol;

        pf_General_EnGet(COMMON_KEY_VR_TIMELAPSE, &iTimeLapseSetting);
        AIHC_GetMovieConfig(AHC_FRAME_RATE, &Param);
        FrameRate = AHC_GetVideoRealFrameRate(Param);

        usVopTimeIncrement = AHC_VIDRECD_TIME_INCREMENT_BASE;
        usVopTimeIncrResol = FrameRate;

        switch(iTimeLapseSetting){
            case PRJ_VR_TIMELAPSE_1SEC:
                bTimeLapseEnable = AHC_TRUE;
                uiTimeLapseRatio = 1;
                break;
            case PRJ_VR_TIMELAPSE_5SEC:
                bTimeLapseEnable = AHC_TRUE;
                uiTimeLapseRatio = 5;
                break;
            case PRJ_VR_TIMELAPSE_10SEC:
                bTimeLapseEnable = AHC_TRUE;
                uiTimeLapseRatio = 10;
                break;
            case PRJ_VR_TIMELAPSE_30SEC:
                bTimeLapseEnable = AHC_TRUE;
                uiTimeLapseRatio = 30;
                break;
            case PRJ_VR_TIMELAPSE_60SEC:
                bTimeLapseEnable = AHC_TRUE;
                uiTimeLapseRatio = 60;
                break;

            case PRJ_VR_TIMELAPSE_OFF:
            default:
                uiTimeLapseRatio = 1;
                break;
        }
        if(AHC_TRUE== bTimeLapseEnable){
            printc(FG_GREEN("TimeLapse:0x%x sec.\r\n"), uiTimeLapseRatio);
        }
        else{
            printc(FG_GREEN("TimeLapse: Off.\r\n"));
        }
        AHC_VideoSetTimeLapseFPS(bTimeLapseEnable, AHC_VIDRECD_TIME_INCREMENT_BASE, usVopTimeIncrement / uiTimeLapseRatio);
    }
#endif


    AHC_UF_SetFreeChar(0, DCF_SET_FREECHAR, (UINT8 *) VIDEO_DEFAULT_FLIE_FREECHAR);

    if((MenuSettingConfig()->uiMOVSoundRecord == MOVIE_SOUND_RECORD_ON) && (AHC_FALSE == bEnableSlowMotion) && (AHC_FALSE == bTimeLapseEnable))
        ahc_ret = AHC_SetMode(AHC_MODE_VIDEO_RECORD);
    else
        ahc_ret = AHC_SetMode(AHC_MODE_RECORD_VONLY);

#if (MOTION_DETECTION_EN)
    // m_ulVMDStopCnt is not zero that is trigged by MVD, update VMD stop count again.
    if (m_ulVMDStopCnt && (AHC_TRUE == ahc_ret)) {
        #if (PARKING_RECORD_FORCE_20SEC == 1)
        UINT32 ulTime = 0;
        AHC_GetCurrentRecordingTime(&ulTime);
        ulTime += VideoFunc_GetRecordTimeOffset();
        printc(FG_BLUE("Parking: Recorded time = %d ms\r\n"), ulTime);
        m_ulVMDStopCnt = 200 - (ulTime / 100); // Set remaining recording time
        #else
        m_ulVMDStopCnt = AHC_GetVMDRecTimeLimit() * 1000 / VIDEO_TIMER_UNIT;
        #endif
    }
    else
    {
        // When VR is error, stop down count for VMD VR
        m_ulVMDStopCnt = 0;
    }
#endif

    if( AHC_TRUE == ahc_ret )
    {
        uiStateSetLocked(AHC_TRUE);
        bVideoRecording = AHC_TRUE;
    }
    else
    {
        return VIDEO_REC_STOP;
    }
	#if (defined(CUS_ADAS_OUTPUT_LOG) && ADAS_OUTPUT_LOG == 1)
	 ADAS_open_txt((char *) AHC_GetCurrentVRFileName(0));
	#endif
    return VIDEO_REC_START;
}

AHC_BOOL VideoFunc_RecordStop(void)
{
    AHC_BOOL ahc_ret    = AHC_TRUE;
    UINT16   zoomratio  = 4;
    UINT8    bRotate    = 0;
    UINT16 dw, dh;

#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0) 
    bRotate		= 0;
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
    bRotate		= 1;             
#endif

#if (FLICKER_PWR_LED_AT_VR_STATE)
    LedCtrl_PowerLed(AHC_TRUE);
#endif

#if (VR_PREENCODE_EN)
    if(m_ubPreEncodeEn)
        m_ubPreRecording = AHC_FALSE;
#endif

#if (HDMI_PREVIEW_EN || TVOUT_PREVIEW_EN)
    if (bPreviewModeTVout && AHC_IsTVConnectEx())
    {
        bRotate = 0;

        if (MenuSettingConfig()->uiTVSystem == TV_SYSTEM_PAL)
        {
            AHC_GetPalTvDisplayWidthHeight(&dw, &dh);
        }
        else
        {
            AHC_GetNtscTvDisplayWidthHeight(&dw, &dh);
        }
    }
    else
#endif
#if (HDMI_PREVIEW_EN)
    if(bPreviewModeHDMI && AHC_IsHdmiConnect())
    {
        bRotate = 0;

        AHC_GetHdmiDisplayWidthHeight(&dw, &dh);
    }
    else
#endif
    {
        dw = RTNA_LCD_GetAttr()->usPanelW;
        dh = RTNA_LCD_GetAttr()->usPanelH;
    }

    if( AHC_IsEmerRecordStarted() == AHC_TRUE )
    {
        #if (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_MOVE_FILE)
        AHC_SetEmerRecordStarted(AHC_FALSE);
        #elif ((EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_DUAL_FILE) || (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE))
        AHC_StopEmerRecord();
        SystemSettingConfig()->byStartNormalRecordAfterEmergencyRecord = 0;
        AHC_EmerRecordPostProcess();
        #endif
    }
    else if( uiGetParkingModeEnable() )
    {
         AHC_ParkingModePostProcess();
    }

    AHC_SetDisplayWindow(DISPLAY_SYSMODE_VIDEOCAPTURE, AHC_TRUE, bRotate, 0, 0, dw, dh, 0);

    AHC_UF_SetFreeChar(0, DCF_SET_FREECHAR, (UINT8 *) VIDEO_DEFAULT_FLIE_FREECHAR);

    VideoFunc_SetResolution(MenuSettingConfig()->uiMOVSize);

    VideoFunc_PresetSensorMode(MenuSettingConfig()->uiMOVSize);

    AHC_SetPreviewZoomConfig(ZoomCtrl_GetVideoDigitalZoomMax(), (UINT8)zoomratio);

#if (MOTION_DETECTION_EN)
    m_ulVMDRemindTime = 1;
    #if !defined(CFG_MVD_MODE_LINK_WITH_MENU_SENSITIVITY)
    m_ubVMDStart      = AHC_FALSE;
    #endif
    m_ulVMDStopCnt    = 0;
#endif

    if(VideoFunc_LockFileEnabled()) {
        // Remove Lock Next file
        LockFileTypeChange(1 /* Remove Next */);
    }
    printc("set mode\r\n");
    ahc_ret = AHC_SetMode(AHC_MODE_RECORD_PREVIEW);

#if (VR_PREENCODE_EN)
    if(m_ubPreEncodeEn)
        m_ubPreRecording = AHC_TRUE;
#endif

    if( AHC_TRUE == ahc_ret )
    {
        uiStateSetLocked(AHC_FALSE);
        bVideoRecording = AHC_FALSE;
    }
    else
    {
        printc("Back to Preview Error\n");
    }
	#if (defined(CUS_ADAS_OUTPUT_LOG) && ADAS_OUTPUT_LOG == 1)
	 ADAS_close_txt();
	#endif
    return ahc_ret;
}

AHC_BOOL VideoFunc_RecordPause(void)
{
    AHC_BOOL ahc_ret = AHC_TRUE;
    UINT32   CurSysMode;

    AHC_GetSystemStatus(&CurSysMode);

    CurSysMode >>= 16;

    if( ( CurSysMode != AHC_MODE_VIDEO_RECORD ) &&
        ( CurSysMode != AHC_MODE_RECORD_VONLY )  )
    {
        return AHC_FALSE;
    }

    ahc_ret = AHC_CaptureClipCmd(AHC_CAPTURE_CLIP_PAUSE, 0);

    return ahc_ret;
}

AHC_BOOL VideoFunc_RecordResume(void)
{
    AHC_BOOL ahc_ret = AHC_TRUE;
    UINT32   CurSysMode;

    AHC_FS_SetCreationTime();
    AHC_GetSystemStatus(&CurSysMode);

    CurSysMode >>= 16;

    if( ( CurSysMode != AHC_MODE_VIDEO_RECORD ) &&
        ( CurSysMode != AHC_MODE_RECORD_VONLY )  )
    {
        return AHC_FALSE;
    }

    ahc_ret = AHC_CaptureClipCmd(AHC_CAPTURE_CLIP_RESUME, 0);

    return ahc_ret;
}

AHC_BOOL VideoFunc_Shutter(void)
{
    AHC_BOOL    bon;

    bon = AHC_CaptureClipCmd(AHC_CAPTURE_SNAPSHOT, 0);

    return bon;
}

AHC_BOOL VideoFunc_ShutterFail(void)
{
    return AHC_TRUE;
}

AHC_BOOL VideoFunc_Preview(void)
{
    AHC_BOOL ahc_ret        = AHC_TRUE;

    UINT16   zoomratio      = 4;
    UINT8    bRotate        = 0;
    UINT16   dw, dh;
    MMP_ULONG codec_type, streaming;

    _AHC_PRINT_FUNC_ENTRY_();
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0) 
    bRotate		= 0;
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
    bRotate		= 1;             
#endif

#if (VR_PREENCODE_EN)
    if(m_ubPreEncodeEn)
        m_ubPreRecording = AHC_FALSE;
#endif

    AHC_SetPreviewZoomConfig(ZoomCtrl_GetVideoDigitalZoomMax(), (UINT8)zoomratio);

#if (TVOUT_PREVIEW_EN)
    if (bPreviewModeTVout && AHC_IsTVConnectEx())
    {
        bRotate = 0;

        if (MenuSettingConfig()->uiTVSystem == TV_SYSTEM_PAL)
        {
	        AHC_SetDisplayOutputDev(DISP_OUT_TV_PAL, AHC_DISPLAY_DUPLICATE_1X);
	        AHC_GetPalTvDisplayWidthHeight(&dw, &dh);			
        }
        else
        {
	        AHC_SetDisplayOutputDev(DISP_OUT_TV_NTSC, AHC_DISPLAY_DUPLICATE_1X);
            AHC_GetNtscTvDisplayWidthHeight(&dw, &dh);
        }
    }
    else
#endif
#if (HDMI_PREVIEW_EN)
    if(bPreviewModeHDMI && AHC_IsHdmiConnect())
    {
        bRotate = 0;

        AHC_GetHdmiDisplayWidthHeight(&dw, &dh);
    }
    else
#endif
    {
        dw = RTNA_LCD_GetAttr()->usPanelW;
        dh = RTNA_LCD_GetAttr()->usPanelH;
    }

#if (UVC_HOST_VIDEO_ENABLE)
    //PIP
    #if (TVOUT_PREVIEW_EN||HDMI_PREVIEW_EN)
    if(AHC_IsTVConnectEx() || AHC_IsHdmiConnect()){
        AHC_SetDisplayWindow(DISPLAY_SYSMODE_VIDEOCAPTURE, AHC_TRUE, bRotate, 0, 0, dw, dh, 0);
        AHC_SetDisplayWindow(DISPLAY_SYSMODE_VIDEOCAPTURE_UVC, AHC_TRUE, bRotate, dw >> 1, dh >> 1, dw >> 1, dh >> 1, 0);
    }
    else
    #endif
    {
    #if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0) 
    AHC_SetDisplayWindow(DISPLAY_SYSMODE_VIDEOCAPTURE, AHC_TRUE, bRotate, 0, 0, dw, dh, 0);
    AHC_SetDisplayWindow(DISPLAY_SYSMODE_VIDEOCAPTURE_UVC, AHC_TRUE, bRotate, 0, 0, dw >> 1, dh >> 1, 0);
    #elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90)
    AHC_SetDisplayWindow(DISPLAY_SYSMODE_VIDEOCAPTURE, AHC_TRUE, bRotate, 0, 0, dw, dh, 0);
    AHC_SetDisplayWindow(DISPLAY_SYSMODE_VIDEOCAPTURE_UVC, AHC_TRUE, bRotate, 0, 0, dw >> 1, dh >> 1, 0);
    #elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
    AHC_SetDisplayWindow(DISPLAY_SYSMODE_VIDEOCAPTURE, AHC_TRUE, bRotate, 0, 0, dw, dh, 0);
    AHC_SetDisplayWindow(DISPLAY_SYSMODE_VIDEOCAPTURE_UVC, AHC_TRUE, bRotate, dw >> 1, 0, dw >> 1, dh >> 1, 0);
    #endif    
    }
#elif (SUPPORT_DUAL_SNR)
    AHC_SetDisplayWindow(DISPLAY_SYSMODE_VIDEOCAPTURE, AHC_TRUE, bRotate, 0, 0, dw, dh, 0);   
    {
    	AHC_DISPLAY_OUTPUTPANEL	  outputPanel;
        AHC_GetDisplayOutputDev( &outputPanel );
        if (outputPanel == MMP_DISPLAY_SEL_CCIR) {    
            AHC_SetDisplayWindow(DISPLAY_SYSMODE_VIDEOCAPTURE_TV_DEC, AHC_TRUE, bRotate, 0, 0, dw, dh, 0);
        }
        else {
            AHC_SetDisplayWindow(DISPLAY_SYSMODE_VIDEOCAPTURE_TV_DEC, AHC_TRUE, bRotate, 0, 0, dw >> 1, dh >> 1, 0);
        }
    }
#else
    AHC_SetDisplayWindow(DISPLAY_SYSMODE_VIDEOCAPTURE, AHC_TRUE, bRotate, 0, 0, dw, dh, 0);
#endif

    AHC_UF_SetFreeChar(0, DCF_SET_FREECHAR, (UINT8 *) VIDEO_DEFAULT_FLIE_FREECHAR);

    AIHC_GetMovieConfigEx(1, AHC_VIDEO_CODEC_TYPE, &codec_type);
    AIHC_GetMovieConfigEx(1, AHC_VIDEO_STREAMING, &streaming);
    if (!streaming || codec_type != AHC_MOVIE_VIDEO_CODEC_H264) {
        VideoFunc_SetResolution(MenuSettingConfig()->uiMOVSize);
    }

    VideoFunc_PresetSensorMode(MenuSettingConfig()->uiMOVSize);
    VideoFunc_PresetFrameRate(MenuSettingConfig()->uiMOVSize);

#ifdef  _OEM_
    Oem_SetVideo_Prop(MenuSettingConfig()->uiMOVSize);
#endif

    AHC_PreSetFlashLED();

#if (MOTION_DETECTION_EN)
    m_ubVMDStart      = AHC_FALSE;
    m_ulVMDStopCnt    = 0;
#endif

    //AHC_HostUVCVideoEnable(AHC_TRUE); //SUPPORT_USB_HOST_FUNC
    if (m_TVStatus == AHC_TV_DSC_PREVIEW_STATUS) {
	   //For leaving TV system mode, device can't capture image.
       ahc_ret = AHC_SetMode(AHC_MODE_CAPTURE_PREVIEW);
       return ahc_ret;
    }
    else {
       ahc_ret = AHC_SetMode(AHC_MODE_RECORD_PREVIEW);        
    }
    //AHC_HostUVCVideoPreviewStart(); //SUPPORT_USB_HOST_FUNC



#if (VR_PREENCODE_EN)
    if(m_ubPreEncodeEn)
    {
        m_ubPreRecording = AHC_TRUE;
        AHC_ConfigMovie(AHC_VIDEO_RESERVED_SIZE, STORAGE_MIN_SIZE+PRE_RECORD_STORAGE_MIN_SIZE);
    }
#endif

#if (SUPPORT_EIS)
    {
        static int testgyro_flag = 0;

        extern void MMPF_EIS_mode_init(void);
        extern void MMPF_EIS_start_gyro_sample(void);

        MMPF_EIS_mode_init();
        printc("\r\nMMPF_EIS_mode_init\r\n");

        if (testgyro_flag == 0) {
            testgyro_flag = 1;

            MMPF_EIS_start_gyro_sample();
            printc("\r\nMMPF_EIS_start_gyro_sample\r\n");
        }
        MMPF_EIS_enable(1);
    }
#endif

    return ahc_ret;
}

AHC_BOOL VideoFunc_EnterVMDMode(void)
{
#if (MOTION_DETECTION_EN)

    if (AHC_SDMMC_BasicCheck()==AHC_FALSE ||
        MenuSettingConfig()->uiMotionDtcSensitivity == MOTION_DTC_SENSITIVITY_OFF)
        return AHC_FALSE;

    if(VideoFunc_RecordStatus())
        return AHC_TRUE;

    m_ulVMDCancel    = AHC_FALSE;
    m_ubInRemindTime = AHC_TRUE;

    #ifdef CFG_REC_CUS_VMD_REMIND_TIME //may be defined in config_xxx.h
    m_ulVMDRemindTime = CFG_REC_CUS_VMD_REMIND_TIME;
    #else
    m_ulVMDRemindTime = 10;
    #endif

    m_ubMotionDtcEn = AHC_TRUE;
    #if (VR_PREENCODE_EN)
    m_ubPreEncodeEn = AHC_FALSE;
    #endif
    AHC_SetMode(AHC_MODE_IDLE);
    VideoFunc_Preview();

#endif
    return AHC_TRUE;
}

AHC_BOOL VideoFunc_ExitVMDMode(void)
{
#if (MOTION_DETECTION_EN)

    #if (VR_PREENCODE_EN)
    m_ubPreEncodeEn = AHC_FALSE;
    #endif
    m_ubMotionDtcEn = AHC_FALSE;

    m_ubInRemindTime = AHC_FALSE;

    if(VideoFunc_RecordStatus())
    {
        VideoFunc_RecordStop();
        DrawStateVideoRecUpdate(EVENT_VIDEO_KEY_RECORD_STOP);
    }
    else
    {
        bDisableVideoPreRecord = AHC_TRUE;
    }

    if (m_ulVMDRemindTime)
    {
        m_ulVMDCancel = AHC_TRUE;
    }
    else
    {
        AHC_SetMode(AHC_MODE_IDLE);
        VideoFunc_Preview();
    }
#endif
    return AHC_TRUE;
}

AHC_BOOL VideoFunc_ZoomOperation(AHC_ZOOM_DIRECTION bZoomDir)
{
    AHC_BOOL ahc_ret = AHC_TRUE;

#if (VIDEO_DIGIT_ZOOM_EN)

    if( AHC_SENSOR_ZOOM_IN == bZoomDir )
    {
        if( ZoomCtrl_GetVideoDigitalZoomMax() > ZoomCtrl_GetVideoDigitalZoomLevel() )
        {
            ahc_ret = ZoomCtrl_DigitalZoom(VIDEO_CAPTURE_MODE, AHC_SENSOR_ZOOM_IN);
        }
    }
    else if(AHC_SENSOR_ZOOM_OUT == bZoomDir )
    {
        if( 0 < ZoomCtrl_GetVideoDigitalZoomLevel() )
        {
            ahc_ret = ZoomCtrl_DigitalZoom(VIDEO_CAPTURE_MODE, AHC_SENSOR_ZOOM_OUT);
        }
    }
    else
    {
        ahc_ret = ZoomCtrl_DigitalZoom(VIDEO_CAPTURE_MODE, AHC_SENSOR_ZOOM_STOP);
    }
#endif
    return ahc_ret;
}

AHC_BOOL VideoFunc_RecordRestart(void)
{
    UINT64      ulFreeSpace     = 0;
    UINT32      ulAudBitRate    = 128000;//128K
    UINT32      ulVidBitRate    = 0;
    AHC_BOOL    ubDeleteFile    = AHC_FALSE;
    AHC_BOOL    ahc_ret         = AHC_TRUE;
    UINT32      ulTimelimit;
    UINT64      ulSpaceNeeded   = 0;
#if (DCF_DB_COUNT >= 2)
    UINT32      ulSpaceNeededInClusters = 0;
    DCF_DB_TYPE sDB;
#endif
    UINT8       bRotate         = 0;
    UINT16 dw, dh;
#if (defined(VIDEO_REC_TIMELAPSE_EN) && VIDEO_REC_TIMELAPSE_EN)
        UINT32 slVRTimelapse = 0;		// Off
#endif
    MMPS_FW_OPERATION sMergerStatus = MMPS_FW_OPERATION_NONE;

    _AHC_PRINT_FUNC_ENTRY_();

    MMPS_3GPRECD_RecordStatus(&sMergerStatus);
    if(sMergerStatus != MMPS_FW_OPERATION_PREENCODE){
        printc(FG_RED("sMergerStatus:%d\r\n"),sMergerStatus);        
        AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0);
        return 0;
    }
    
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0) 
    bRotate		= 0;
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
    bRotate		= 1;             
#endif

#if (TVOUT_PREVIEW_EN)
    if (bPreviewModeTVout && AHC_IsTVConnectEx())
    {
        bRotate = 0;

        if (MenuSettingConfig()->uiTVSystem == TV_SYSTEM_PAL)
        {
            AHC_GetPalTvDisplayWidthHeight(&dw, &dh);
        }
        else
        {
            AHC_GetNtscTvDisplayWidthHeight(&dw, &dh);
        }
    }
    else
#endif
#if (HDMI_PREVIEW_EN)
    if (bPreviewModeHDMI && AHC_IsHdmiConnect())
    {
        bRotate = 0;

        AHC_GetHdmiDisplayWidthHeight(&dw, &dh);
    }
    else
#endif
    {
        dw = RTNA_LCD_GetAttr()->usPanelW;
        dh = RTNA_LCD_GetAttr()->usPanelH;
    }

    AHC_SetDisplayWindow(DISPLAY_SYSMODE_VIDEOCAPTURE, AHC_TRUE, bRotate, 0, 0, dw, dh, 0);
#if (SUPPORT_DUAL_SNR)
    AHC_SetDisplayWindow(DISPLAY_SYSMODE_VIDEOCAPTURE_TV_DEC, AHC_TRUE, bRotate, 0, 0, dw >>1, dh >>1, 0);
#endif

    AHC_UF_SetFreeChar(0, DCF_SET_FREECHAR, (UINT8 *) VIDEO_DEFAULT_FLIE_FREECHAR);

    if(AHC_SDMMC_BasicCheck()==AHC_FALSE)
    {
        printc("AHC_SDMMC_BasicCheck Fail\r\n");
        return AHC_FALSE;
    }

    ulTimelimit = AHC_GetVRTimeLimit();
    if(ulTimelimit==NON_CYCLING_TIME_LIMIT)
    {
        UINT32 ulCurrRecTime;

        AHC_GetCurrentRecordingTime(&ulCurrRecTime);
        RecordTimeOffset   += ulCurrRecTime;
        ulSpaceNeeded       = 0x24000000;
        ubDeleteFile        = AHC_FALSE;
    }
    else
    {
#if (LIMIT_MAX_LOCK_FILE_TIME)
        //Calculate Correct Time Offset
        if (VideoFunc_LockFileEnabled()) {
            RecordTimeOffset += ulVRTotalTime;
            ulTimelimit = AHC_GetVideoMaxLockFileTime();
            ulVRTotalTime = ulTimelimit * 1000;
        } else {
            if (ulVRTotalTime != 0) {
                RecordTimeOffset += ulVRTotalTime;
                ulVRTotalTime = 0;
            } else {
                RecordTimeOffset += (ulTimelimit*1000/*ms*/);
            }
        }
#else
        RecordTimeOffset += (ulTimelimit*1000/*ms*/);
#endif

        AIHC_GetMovieConfig(AHC_VIDEO_BITRATE, &ulVidBitRate);

#if (defined(VIDEO_REC_TIMELAPSE_EN) && VIDEO_REC_TIMELAPSE_EN)
        if ((AHC_Menu_SettingGetCB(COMMON_KEY_VR_TIMELAPSE, &slVRTimelapse) == AHC_TRUE) && (slVRTimelapse != PRJ_VR_TIMELAPSE_OFF)){
            UINT32      Framerate, Frate;
            printc(" TimeLapse record.....\r\n");

            AIHC_GetMovieConfig(AHC_FRAME_RATE, &Frate);
            Framerate = AHC_GetVideoRealFrameRate(Frate) / AHC_VIDRECD_TIME_INCREMENT_BASE;
            VideoFunc_GetTimeLapseBitrate(Framerate, slVRTimelapse, &ulVidBitRate, &ulAudBitRate);
        }
        else
#endif
        {
            if (MenuSettingConfig()->uiMOVSoundRecord == MOVIE_SOUND_RECORD_OFF)
                ulAudBitRate = 0;
        }

        AHC_GetVideoRecStorageSpeceNeed(ulVidBitRate, ulAudBitRate, ulTimelimit, &ulSpaceNeeded);

        VideoFunc_GetFreeSpace(&ulFreeSpace);
#if (DCF_DB_COUNT >= 2)
        if(uiGetParkingModeEnable() == AHC_FALSE) //DCF_DB_TYPE_1ST_DB
        {
            sDB = DCF_DB_TYPE_1ST_DB;
        }
        else
        {                                      //DCF_DB_TYPE_2ND_DB
            sDB = DCF_DB_TYPE_2ND_DB;
        }
        ulSpaceNeededInClusters = ulSpaceNeeded /AHC_UF_GetClusterSize(sDB);
        if((AHC_UF_GetFileCluster(sDB)+ ulSpaceNeededInClusters > AHC_UF_GetClusterTH(sDB))
           || (ulFreeSpace < ulSpaceNeeded))
        {
            printc("--I-- %s:%d Storage space is not enough for recording, delete video file\r\n", __func__, __LINE__);
            ubDeleteFile = AHC_TRUE;
        }
#if (DELETION_BY_FILE_NUM)
        else 
		{
            UINT32 uiFileCount;
            DCF_DB_TYPE sCurDB;
            sCurDB = AHC_UF_GetDB();
            AHC_UF_SelectDB(sCurDB);
            AHC_UF_GetTotalFileCount(&uiFileCount);
            if(uiFileCount >= AHC_UF_GetFileTH(sCurDB)){
                printc("FileCount > FileNum Threshold\r\n");
            	ubDeleteFile = AHC_TRUE;
            }
        }
#endif //#if (DELETION_BY_FILE_NUM)
#else
        if( ulFreeSpace < ulSpaceNeeded )
        {
            printc("--I-- %s:%d Storage space is not enough for recording, delete video file\r\n", __func__, __LINE__);
            ubDeleteFile = AHC_TRUE;
        }
#if (DELETION_BY_FILE_NUM)
        else
		{
            UINT32 uiFileCount;
            AHC_UF_GetTotalFileCount(&uiFileCount);
            if(uiFileCount >= AHC_UF_GetFileTH(DCF_DB_TYPE_1ST_DB)){
                printc("FileCount > FileNum Threshold\r\n");
                ubDeleteFile = AHC_TRUE;
            }
        }
#endif //#if (DELETION_BY_FILE_NUM)
#endif
    }

#if (RESET_RECORDED_TIME)
    RecordTimeOffset    = 0;
#endif
    printc("Restart RecordTimeOffset [%d] ms\r\n", RecordTimeOffset);

    AHC_ConfigMovie(AHC_VIDEO_RESERVED_SIZE, STORAGE_MIN_SIZE );//TBD

    m_uiSlowMediaCBCnt = 0;

	#if (UPDATE_UI_USE_MULTI_TASK)
	m_ubUpdateUiByOtherTask = AHC_TRUE;
	#endif
    ahc_ret = AHC_RestartVRMode(ubDeleteFile);
	#if (UPDATE_UI_USE_MULTI_TASK)
	m_ubUpdateUiByOtherTask = AHC_FALSE;
	#endif	
    // Re-Set TimeLimit in case a time specfied at file locked.
    AHC_SetVRTimeLimit(ulTimelimit);

    if( AHC_TRUE == ahc_ret )
    {
        uiStateSetLocked(AHC_TRUE);
        bVideoRecording = AHC_TRUE;
    }
	#if (defined(CUS_ADAS_OUTPUT_LOG) && ADAS_OUTPUT_LOG == 1)
	 ADAS_close_txt();
	 ADAS_open_txt((char *) AHC_GetCurrentVRFileName(0));
	 #endif
    return ahc_ret;
}

AHC_BOOL VideoFunc_SetFileLock(void)
{
    if(AHC_WMSG_States())
    {
        AHC_WMSG_Draw(AHC_FALSE, WMSG_NONE, 0);
    }

    if(!VideoFunc_LockFileEnabled())
    {
        #if (LIMIT_MAX_LOCK_FILE_NUM) && (MAX_LOCK_FILE_ACT==LOCK_FILE_STOP)
        if (m_ulLockFileNum  >= MAX_LOCK_FILE_NUM )
            return AHC_FALSE;
        #endif

        AHC_WMSG_Draw(AHC_TRUE, WMSG_LOCK_CURRENT_FILE, 2);
        EnableLockFile(AHC_TRUE, VR_LOCK_FILE_TYPE);

        #if (LIMIT_MAX_LOCK_FILE_NUM)
        m_ulLockEventNum++;
        #endif

        #if (LIMIT_MAX_LOCK_FILE_TIME)
        {
            UINT32 ulCurVRTime;
            UINT32 ulMaxVRTime = AHC_GetVideoMaxLockFileTime();

            AHC_GetCurrentRecordingTime(&ulCurVRTime);//ms
            printc("Current Recording Time %d ms\r\n",ulCurVRTime);

            if(ulCurVRTime > ulMaxVRTime * 1000)
            {
                ulVRTotalTime = ulCurVRTime;
            } else {
                ulVRTotalTime = ulMaxVRTime * 1000;
            }

            AHC_SetVRTimeLimit(ulMaxVRTime);
        }
        #endif
    }
    else
    {
        #if (LIMIT_MAX_LOCK_FILE_NUM)
        m_ulLockEventNum--;
        #endif
        EnableLockFile(AHC_FALSE, 0);
        AHC_WMSG_Draw(AHC_TRUE, WMSG_UNLOCK_CUR_FILE, 2);
        AHC_Protect_SetType(AHC_PROTECT_NONE);
    }

    return AHC_TRUE;
}

AHC_BOOL VideoRecMode_Start(void)
{
    AHC_BOOL    ahc_ret = AHC_TRUE;
    UINT8       ubLCDstatus;

    bShowHdmiWMSG   = AHC_TRUE;
    bShowTvWMSG     = AHC_TRUE;

    //printc(FG_GREEN("***** pf_SLOWMOTION_EnGet() = %d \n"),pf_SLOWMOTION_EnGet());
    //20150611 Terry
    if(pf_SLOWMOTION_EnGet() == SLOWMOTION_X2) {
        printc(FG_RED("SLOWMOTION_X2\n"));
    }
    else if(pf_SLOWMOTION_EnGet() == SLOWMOTION_X4) {
        printc(FG_RED("SLOWMOTION_X4\n"));
    }
    else if(pf_SLOWMOTION_EnGet() == SLOWMOTION_X8){
        printc(FG_RED("SLOWMOTION_X8\n"));
    }
    else {
        SetCurrentOpMode(VIDEOREC_MODE);
        printc(FG_RED("SLOWMOTION_X1\n"));
    }

    AHC_OSDSetActive(0, 0);

    if(AHC_WMSG_States())
       AHC_WMSG_Draw(AHC_FALSE, WMSG_NONE, 0);

    AHC_OSDSetActive(OVL_DISPLAY_BUFFER, 0);

    ahc_ret = VideoFunc_Preview();

    if( AHC_TRUE == ahc_ret ) //Andy Liu TBD
    {
#if (SUPPORT_TOUCH_PANEL) && !defined(_OEM_TOUCH_)
        KeyParser_ResetTouchVariable();
        KeyParser_TouchItemRegister(&VideoMainPage_TouchButton[0], ITEMNUM(VideoMainPage_TouchButton));
#endif
        VideoTimer_Start(VIDEO_TIMER_UNIT);

        bMuteRecord = (MenuSettingConfig()->uiMOVSoundRecord == MOVIE_SOUND_RECORD_ON)?(AHC_FALSE):(AHC_TRUE);

#if (DAY_NIGHT_MODE_SWITCH_EN)
        bNightMode = (MenuSettingConfig()->uiScene == SCENE_TWILIGHT)?(AHC_TRUE):(AHC_FALSE);
#endif

        AHC_LCD_GetStatus(&ubLCDstatus);

        if(ubLCDstatus == AHC_LCD_NORMAL)
            DrawStateVideoRecUpdate(EVENT_LCD_COVER_NORMAL);
        else if(ubLCDstatus == AHC_LCD_REVERSE)
            DrawStateVideoRecUpdate(EVENT_LCD_COVER_ROTATE);
    }

//TODO:Need to review    AHC_OSDSetActive(0, 1);
#if 0 //Andy Liu TBD
    if(AHC_IsHdmiConnect() && bShowHdmiWMSG) {
        bShowHdmiWMSG = AHC_FALSE;
        AHC_WMSG_Draw(AHC_TRUE, WMSG_HDMI_TV, 3);
    }
    else {
        if(AHC_IsTVConnectEx() && bShowTvWMSG) {
            bShowTvWMSG = AHC_FALSE;
            AHC_WMSG_Draw(AHC_TRUE, WMSG_HDMI_TV, 3);
        }
    }
#endif
    return ahc_ret;
}

void VideoRecMode_PreviewUpdate(void)
{
    UINT8       ubLCDstatus;
    AHC_BOOL    ubUpdatePreview   = AHC_FALSE;

    if(VideoRecSize != MenuSettingConfig()->uiMOVSize)
        ubUpdatePreview = AHC_TRUE;

    #if (ENABLE_ADAS_LDWS)
    {
        UINT32 bLDWS_En;

        if (LDWS_SettingChanged() == MMP_TRUE){
            ubUpdatePreview = AHC_TRUE;
        }

        if ( AHC_Menu_SettingGetCB( (char *)COMMON_KEY_LDWS_EN, &bLDWS_En ) == AHC_TRUE ) {
            if ((LDWS_EN_OFF == bLDWS_En) && (MMPS_Sensor_GetADASFeatureEn(MMPS_ADAS_LDWS) == MMP_TRUE) ) {
                ubUpdatePreview = AHC_TRUE;
            }
            else if ((LDWS_EN_ON == bLDWS_En) && (MMPS_Sensor_GetADASFeatureEn(MMPS_ADAS_LDWS) == MMP_FALSE) ) {
                ubUpdatePreview = AHC_TRUE;
            }
        }
		if(LDWS_IsStart())
		{
			LDWS_Unlock();
			ResetLDWSCounter();
		}
    }
    #endif

    #if (ENABLE_ADAS_FCWS)
    {
        UINT32 bFCWS_En;

        if ( AHC_Menu_SettingGetCB( (char *)COMMON_KEY_FCWS_EN, &bFCWS_En ) == AHC_TRUE ) {
            if ((FCWS_EN_OFF == bFCWS_En) && (MMPS_Sensor_GetADASFeatureEn(MMPS_ADAS_FCWS) == MMP_TRUE) ) {
                ubUpdatePreview = AHC_TRUE;
            }
            else if ((pf_FCWS_EnGet() == FCWS_EN_ON) && (MMPS_Sensor_GetADASFeatureEn(MMPS_ADAS_FCWS) == MMP_FALSE) ) {
                ubUpdatePreview = AHC_TRUE;
            }
        }
    }
    #endif

    {
        UINT32 hdr_En;

        if ( AHC_Menu_SettingGetCB( (char *)COMMON_KEY_HDR_EN, &hdr_En ) == AHC_TRUE ) {
            if ((MMP_IsHDREnable() == MMP_TRUE) && (hdr_En == COMMON_HDR_EN_OFF) ) {
                ubUpdatePreview = AHC_TRUE;
            }
            else if ((MMP_IsHDREnable() == MMP_FALSE) && (hdr_En == COMMON_HDR_EN_ON) ) {
                ubUpdatePreview = AHC_TRUE;
            }
        }
    }

#if (MOTION_DETECTION_EN)
    if (m_ubMotionDtcEn && (MMPS_Sensor_IsVMDEnable() == MMP_FALSE)) {
        printc(FG_GREEN("Motion setting changed - Need turn-on\r\n"));
        ubUpdatePreview = AHC_TRUE;
    }
    else if (!m_ubMotionDtcEn && (MMPS_Sensor_IsVMDEnable() == MMP_TRUE)) {
        printc(FG_GREEN("Motion setting changed - Need turn-off\r\n"));
        ubUpdatePreview = AHC_TRUE;
    }
#endif

#if (VR_PREENCODE_EN)
    {
        UINT32  ulPreRecordStatus = 0;

        AHC_GetPreRecordStatus(&ulPreRecordStatus);

        if(bVideoPreRecordStatus != ulPreRecordStatus)
        {
            bVideoPreRecordStatus = ulPreRecordStatus;
            ubUpdatePreview       = AHC_TRUE;
        }
    }
#endif

    AHC_LCD_GetStatus(&ubLCDstatus);

    if(ubLCDstatus == AHC_LCD_NORMAL)
        DrawStateVideoRecUpdate(EVENT_LCD_COVER_NORMAL);
    else if(ubLCDstatus == AHC_LCD_REVERSE)
        DrawStateVideoRecUpdate(EVENT_LCD_COVER_ROTATE);
    
    if(ubUpdatePreview == AHC_TRUE)
    {
#if (VR_PREENCODE_EN)
        if(!m_ubPreEncodeEn)
            bDisableVideoPreRecord = AHC_TRUE;
#endif
        AHC_SetMode(AHC_MODE_IDLE);
        VideoFunc_Preview();
    }

    bMuteRecord = (MenuSettingConfig()->uiMOVSoundRecord == MOVIE_SOUND_RECORD_ON)?(AHC_FALSE):(AHC_TRUE);

#if (DAY_NIGHT_MODE_SWITCH_EN)
    bNightMode = (MenuSettingConfig()->uiScene == SCENE_TWILIGHT)?(AHC_TRUE):(AHC_FALSE);
#endif

#if (SUPPORT_TOUCH_PANEL) && !defined(_OEM_TOUCH_)
    KeyParser_ResetTouchVariable();
    KeyParser_TouchItemRegister(&VideoMainPage_TouchButton[0], ITEMNUM(VideoMainPage_TouchButton));
#endif

#if VIRTUAL_KEY_BOARD_FOR_WIFI
    {
        extern unsigned char ucWifiAPParamModified;
        extern unsigned char ucWifiSTAParamModified;
        extern unsigned char ucwifiStassid[6][64];
        extern unsigned char ucwifiStapswd[6][64];
        //amn_currConfig_get_enum( "Net.Dev.%d.BootProto", ucWifiAPParamModified )->v.strVal;

        printc("ucWifiAPParamModified %d, ucWifiSTAParamModified %d\n",ucWifiAPParamModified, ucWifiSTAParamModified);
        if(ucWifiAPParamModified)
        {
            ucWifiAPParamModified = 0;
            netapp_ResetNetwork();
        }

        if(ucWifiSTAParamModified)
        {
            //0 none,  1 AES,  2 WEP
            switch (LwIP_join_WLAN_AP(ucwifiStassid[ucWifiSTAParamModified], 1, ucwifiStapswd[ucWifiSTAParamModified], 28 ))
            {
                case 1:
                    printc("... failed\n");
                    return;
                case 0:
                    printc("joined sucessful\n");
                    break;
            }

            if (LwIP_start_netif( NULL, NULL ) < 0)
            {
                set_NetAppStatus(NETAPP_WIFI_NOIP);//3
            }
            else
            {
                set_NetAppStatus(NETAPP_WIFI_READY);//2
            }

            ucWifiSTAParamModified = 0;
        }
    }
#endif

    VideoTimer_Start(VIDEO_TIMER_UNIT);
}

AHC_BOOL VideoFunc_TriggerEmerRecord(void)
{
#if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
    if(AHC_IsEmerRecordStarted() == AHC_TRUE)
    {

        UINT32 uiTime;
        UINT32 uiInterval;

        AHC_GetEmerRecordTime(&uiTime);
        //printc("Emer Cur Time : %d \n", uiTime);

        uiInterval = AHC_GetEmerRecordInterval();
        uiTime = (uiTime+1000 - 1)/1000;
        //printc("Emer Cur Time2 : %d (%d)\n", uiTime, uiInterval);

        if(uiInterval < EMER_RECORD_DUAL_WRTIE_MAX_TIME)
        {
            if(uiTime >= (uiInterval - EMER_RECORD_DUAL_WRTIE_DELTA))
            {
                uiTime+=EMER_RECORD_DUAL_WRITE_POSTTIME;

                if(uiTime > EMER_RECORD_DUAL_WRTIE_MAX_TIME){
                    uiTime = EMER_RECORD_DUAL_WRTIE_MAX_TIME;
                }else if(uiTime < EMER_RECORD_DUAL_WRITE_INTERVAL){
                    uiTime = EMER_RECORD_DUAL_WRITE_INTERVAL;
                }

                AHC_SetEmerRecordInterval(uiTime);
            }
        }

        return AHC_FALSE;
    }
    #if((EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_DUAL_FILE)||(EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE))
    else if((AHC_IsEmerPostDone() == AHC_TRUE) && (AHC_GetKernalEmergencyStopStep() == AHC_KERNAL_EMERGENCY_AHC_DONE) )
    {
        #if (GPS_RAW_FILE_EMER_EN == 1)
        if(GPSCtrl_GetGPSRawStart_Emer() == AHC_FALSE)
        #else
        if(1)
        #endif
        {
            UINT32 ulCurrentRecordedTime = 0;
            AHC_GetCurrentRecordingTime(&ulCurrentRecordedTime);

            AHC_GetEmerRecordTimeOffset( &m_ulEventPreRecordTime );
            m_ulEventHappenTime = OSTimeGet();
            printc(FG_BLUE("VideoFunc_TriggerEmerRecord--ulEmergencyPreRecordTime-- %d (%d) \r\n"), m_ulEventPreRecordTime, ulCurrentRecordedTime);
            m_bCurrentTimeLessThanPreRecord = (m_ulEventPreRecordTime > ulCurrentRecordedTime)? AHC_TRUE : AHC_FALSE;

#if (GPS_CONNECT_ENABLE)
            AHC_GPS_FlushBackupBuffer( AHC_TRUE );
#endif
#if (GSENSOR_CONNECT_ENABLE)
            AHC_Gsensor_FlushBackupBuffer( AHC_TRUE );
#endif
            AHC_StartEmerRecord();
            AHC_SetKernalEmergencyStopStep(AHC_KERNAL_EMERGENCY_RECORD);
            return AHC_TRUE;
        }
        else
        {
            return AHC_FALSE;
        }
    }
    #else
    else if((AHC_IsEmerPostDone() == AHC_TRUE))
    {
        #if (GPS_RAW_FILE_EMER_EN == 1)
        if(GPSCtrl_GetGPSRawStart_Emer() == AHC_FALSE)
        #else
        if(1)
        #endif
        {
            AHC_StartEmerRecord();
            return AHC_TRUE;
        }
        else
        {
            return AHC_FALSE;
        }
    }
    #endif
    #if (DELETION_BY_FILE_NUM)
    {
        UINT32 uiFileCount;
        DCF_DB_TYPE sCurDB;
       
        sCurDB = AHC_UF_GetDB();
        AHC_UF_GetTotalFileCount(&uiFileCount);
        if(uiFileCount >= AHC_UF_GetFileTH(sCurDB))
        {
            if(AHC_Deletion_RemoveEx(sCurDB, AHC_GetVRTimeLimit()) == AHC_FALSE)
            {
                printc(FG_RED("AHC_Deletion_Romove Error\r\n"));
        	    return AHC_FALSE;
        	}
        }
    }
    #endif
#endif

    return AHC_FALSE;
}

//******************************************************************************
//
//                              AHC State Video Mode
//
//******************************************************************************
void StateVideoRecMode_StartRecordingProc(UINT32 ulJobEvent)
{
    VideoRecordStatus retVal;

    if(AHC_SDMMC_BasicCheck()==AHC_FALSE)
    {
        #ifdef CFG_VIDEO_RECORD_FLICK_LED_BY_CARD_ERROR
        LedCtrl_FlickerLedByCustomer(LED_GPIO_SDMMC_ERROR, LED_GPIO_SDMMC_FLICK_PERIOD, LED_GPIO_SDMMC_FLICK_TIMES);
        #endif

        RTNA_DBG_Str(0, FG_RED("--E-- Memory Card Error\r\n"));
        return;//break;
    }

    #if defined(CFG_ENABLE_VIDEO_REC_VIBRATION) && defined(CFG_VIDEO_REC_VIBRATION_TIME)
    if (AHC_SDMMC_GetMountState())
    {
        UINT64 ulFreeSpace;

        VideoFunc_GetFreeSpace(&ulFreeSpace);

        if (ulFreeSpace)
        {
            AHC_Vibration_Enable(CFG_VIDEO_REC_VIBRATION_TIME);

            #if defined(CFG_VIDEO_REC_VIBRATION_TIMES) && (CFG_VIDEO_REC_VIBRATION_TIMES >= 2)
            {
                UINT16 times = CFG_VIDEO_REC_VIBRATION_TIMES - 1;

                while (times--) {
                    AHC_OS_SleepMs(CFG_VIDEO_REC_VIBRATION_TIME + 100);
                    AHC_Vibration_Enable(CFG_VIDEO_REC_VIBRATION_TIME);
                }
            }
            #endif

            AHC_OS_SleepMs(CFG_VIDEO_REC_VIBRATION_TIME);
        }
    }
    #endif  // defined(CFG_ENABLE_VIDEO_REC_VIBRATION) && defined(CFG_VIDEO_REC_VIBRATION_TIME)

    #if (DCF_DB_COUNT >= 2)
    if(uiGetParkingModeEnable() == AHC_TRUE)
    {
        AHC_UF_SelectDB(DCF_DB_TYPE_2ND_DB);
    }
    else
    {
        AHC_UF_SelectDB(DCF_DB_TYPE_1ST_DB);
    }
    #endif

    RecordTimeOffset = 0;
    retVal = VideoFunc_Record();
    RTNA_DBG_Str(0, "StateVideoRecMode_StartRecordingProc: VideoFunc_Record return");
    RTNA_DBG_Short(0, retVal);
    RTNA_DBG_Str(0, "\r\n");

    // TODO:???
    switch (retVal) {
    case VIDEO_REC_START:
        #ifdef CFG_ENABLE_VIDEO_REC_LED
        RecLED_Timer_Start(100);
        #endif
        break;

    case VIDEO_REC_NO_SD_CARD:
    case VIDEO_REC_CARD_FULL:
        #ifdef CFG_VIDEO_RECORD_FLICK_LED_BY_CARD_ERROR
        LedCtrl_FlickerLedByCustomer(LED_GPIO_SDMMC_ERROR, LED_GPIO_SDMMC_FLICK_PERIOD, LED_GPIO_SDMMC_FLICK_TIMES);
        #endif
        break;
    }

    #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE) // jeff.li
    if(VIDEO_REC_START != retVal && AHC_TRUE == m_ubParkingModeRecTrigger)
        m_ubParkingModeRecTrigger = AHC_FALSE;
    #endif

    #if 0//(BUTTON_BIND_LED)
    if(VIDEO_REC_START != retVal)
        LedCtrl_ButtonLed(AHC_GetVideoModeLedGPIOpin(), AHC_FALSE);
    #endif

    if (VIDEO_REC_NO_SD_CARD == retVal) {
        CGI_FEEDBACK(ulJobEvent, -1 /* NO SD */);
        AHC_WMSG_Draw(AHC_TRUE, WMSG_INSERT_SD_AGAIN, 3);
        printc("%s,%d error!",__func__,__LINE__);
        return;//break;
    }
    else if (VIDEO_REC_CARD_FULL == retVal) {
        CGI_FEEDBACK(ulJobEvent, -4 /* SD FULL */);
        AHC_WMSG_Draw(AHC_TRUE, WMSG_STORAGE_FULL, 3);
        printc("%s,%d error!",__func__,__LINE__);
        return;//break;
    }
    else if(VIDEO_REC_SEAMLESS_ERROR == retVal){
        CGI_FEEDBACK(ulJobEvent, -5 /* INTERNAL ERROR */);
        AHC_WMSG_Draw(AHC_TRUE, WMSG_SEAMLESS_ERROR, 3);
        printc("%s,%d error!",__func__,__LINE__);
        return;//break;
    }
    else if(VIDEO_REC_SD_CARD_ERROR == retVal){
        CGI_FEEDBACK(ulJobEvent, -3 /* SD ERROR */);
        AHC_WMSG_Draw(AHC_TRUE, WMSG_FORMAT_SD_CARD, 3);
        printc("%s,%d error!",__func__,__LINE__);
        return;//break;
    }
    else if(VIDEO_REC_START == retVal){
        CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
    }
    else {
        CGI_FEEDBACK(ulJobEvent, -3 /* SD ERROR */);
    }

    if (VIDEO_REC_START == retVal)
        DrawStateVideoRecUpdate(ulJobEvent);
    else
        DrawStateVideoRecUpdate(EVENT_VIDEO_KEY_RECORD_STOP);
}

void StateVideoRecMode_StopRecordingProc(UINT32 ulJobEvent)
{
    VideoFunc_RecordStop();

    #if(SUPPORT_GSENSOR && POWER_ON_BY_GSENSOR_EN)
    m_ulGSNRRecStopCnt = 0;
    #endif

    //not muted recoding every time when recording starts
    #ifdef CFG_REC_FORCE_UNMUTE //may be defined in config_xxx.h
    if(bMuteRecord)
        AHC_ToggleMute();
    #endif

    //TODO Add callbacks here
    #ifdef CFG_ENABLE_VIDEO_REC_LED
    RecLED_Timer_Stop();
    #endif

    #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
    if( uiGetParkingModeEnable() == AHC_TRUE ) {
        #if (MOTION_DETECTION_EN)
        m_ulVMDStableCnt = MOTION_DETECTION_STABLE_TIME * 1000 / VIDEO_TIMER_UNIT;
        m_ubVMDStart     = AHC_FALSE;
        #endif
        m_ubParkingModeRecTrigger = AHC_FALSE;
    }
    #endif

    DrawStateVideoRecUpdate(EVENT_VIDEO_KEY_RECORD_STOP);
    
#ifdef NET_SYNC_PLAYBACK_MODE
    if(ulJobEvent != WIRELESS_ENTER_PLAYBACK)
#endif
    	CGI_FEEDBACK(ulJobEvent, 1 /* STOP */);
}

#if (ENABLE_ADAS_LDWS)

extern MMP_UBYTE ADAS_CTL_GetLDWSAttr(ldws_params_t *ldws_attr, MMP_LONG *alert);

void VideoFunc_LDWSWarn(void)
{
	//ldws_params_t ldws_attribute = {0}, cur_ldws_attribute;
	ldws_params_t cur_ldws_attribute;
    MMP_LONG  ulAlert;
//	LDWS_GetResult(&ldws_attribute);
//printc("ldws_attribute 0x%x   ", &ldws_attribute);
	ADAS_CTL_GetLDWSAttr(&cur_ldws_attribute, &ulAlert);
//	printc("cur_ldws_attribute 0x%x   \r\n", &cur_ldws_attribute);


	#if (OSD_SHOW_LDWS_ALARM)
	if (ulAlert == LDWS_STATE_DEPARTURE_LEFT)
	{   // Left Shift
		AHC_WMSG_Draw(AHC_TRUE, WMSG_LDWS_LeftShift, 1);
		#ifdef OSD_SHOW_LDWS_ALARM_WITH_MULTI_AUDIO
		AHC_PlaySoundEffect(AHC_SOUNDEFFECT_ATTENTION);
		#endif
		AHC_PlaySoundEffect(AHC_SOUNDEFFECT_LDWS_WARNING);
	}
	else if(ulAlert == LDWS_STATE_DEPARTURE_RIGHT)
	{   // Right Shift
		AHC_WMSG_Draw(AHC_TRUE, WMSG_LDWS_RightShift, 1);
		#ifdef OSD_SHOW_LDWS_ALARM_WITH_MULTI_AUDIO
		AHC_PlaySoundEffect(AHC_SOUNDEFFECT_ATTENTION);
		#endif
		AHC_PlaySoundEffect(AHC_SOUNDEFFECT_LDWS_WARNING);
	}
	#endif

  //  DrawVideo_LDWSWarn(ldws_attribute.state, AHC_TRUE);
}
#endif

#if (ENABLE_ADAS_FCWS)
extern void DrawVideo_FCWSWarn(int ldws_state, AHC_BOOL bDraw);

void VideoFunc_FCWSWarn(void)
{
    AHC_PlaySoundEffect(AHC_SOUNDEFFECT_FCWS_WARNING);
	DrawVideo_FCWSWarn(0, AHC_TRUE);
}
#endif

void StateVideoRecMode(UINT32 ulEvent)
{
    UINT32      ulJobEvent = ulEvent;
    AHC_BOOL    ret = AHC_TRUE;
    UINT32      CurSysMode;

    if (AHC_CheckErrorDialog(ulJobEvent)==AHC_FALSE && ulJobEvent!=EVENT_VIDREC_UPDATE_MESSAGE)
        return;

//may be defined in config_xxx.h. It should be something like EVENT_KEY_LEFT
#if defined(CFG_REC_CALI_ON) && defined(CFG_REC_CALI_OFF)
    if ((FW_RUN_CALI_MODE_EN) || (CalibrationMode))
    {
        extern MMP_BOOL UartExecuteCommandEx( char* szCommand );
        extern void     OSD_Draw_TestMessage(UINT32 Left, UINT32 Top, char result, char* );

        int CaliSelect = 0;

        if(ulEvent == CFG_REC_CALI_OFF)
            CaliSelect = 0;
        else if(ulEvent == CFG_REC_CALI_ON)
            CaliSelect = 1;

        if(CaliSelect==0)
        {
            printc("Calibration : AHC_SetLsCsCali @@\n");
            AHC_SetLsCsCali();
            OSD_Draw_TestMessage(100,50,4,"USB connect..");
            AHC_OS_Sleep(1000);
        }
        else if(CaliSelect==1)
        {
            printc("Calibration writing... \n");
            UartExecuteCommandEx("rdcalibin");
            //AHC_SetAeAwb1Cali();
            OSD_Draw_TestMessage(180,100,4,"LSCS done");
            AHC_OS_Sleep(1000);
        }
    }
#endif

    switch(ulJobEvent)
    {
        case EVENT_NONE                           :
        break;

#if (VIDEO_DIGIT_ZOOM_EN==1)
        case EVENT_KEY_TELE_PRESS                 :
            if((!bZoomStop)&&(bZoomDirect == AHC_SENSOR_ZOOM_OUT)){
                VideoFunc_ZoomOperation(AHC_SENSOR_ZOOM_STOP);
                bZoomStop = AHC_TRUE;
            }

            if(bZoomStop){
                ret = VideoFunc_ZoomOperation(AHC_SENSOR_ZOOM_IN);
                bZoomStop = AHC_FALSE;
                bZoomDirect = AHC_SENSOR_ZOOM_IN;
                DrawStateVideoRecUpdate(ulJobEvent);
            }
            break;

        case EVENT_KEY_WIDE_PRESS                 :
            if((!bZoomStop)&&(bZoomDirect == AHC_SENSOR_ZOOM_IN)){
                VideoFunc_ZoomOperation(AHC_SENSOR_ZOOM_STOP);
                bZoomStop = AHC_TRUE;
            }

            if(bZoomStop){
                ret = VideoFunc_ZoomOperation(AHC_SENSOR_ZOOM_OUT);
                bZoomStop = AHC_FALSE;
                bZoomDirect = AHC_SENSOR_ZOOM_OUT;
                DrawStateVideoRecUpdate(ulJobEvent);
            }
            break;

        case EVENT_KEY_WIDE_STOP                  :
        case EVENT_KEY_TELE_STOP                  :
            ret = VideoFunc_ZoomOperation(AHC_SENSOR_ZOOM_STOP);
            bZoomStop = AHC_TRUE;
            DrawStateVideoRecUpdate(ulJobEvent);
        break;
#endif

        case EVENT_EV_INCREASE                  :
            AHC_ChangeEV(AHC_FALSE, 1);
        break;

        case EVENT_EV_DECREASE                  :
            AHC_ChangeEV(AHC_FALSE, 0);
        break;

        case EVENT_KEY_LEFT                     :
            AHC_ChangeEV(AHC_TRUE, 0);
        break;

        case EVENT_KEY_RIGHT                    :
            AHC_ToggleFlashLED(LED_MODE_AUTO_ON_OFF);
        break;

        case EVENT_KEY_SET                  :

            AHC_GetSystemStatus(&CurSysMode);

            if( (CurSysMode == ((AHC_MODE_VIDEO_RECORD<<16)|AHC_SYS_VIDRECD_PAUSE)) ||
                (CurSysMode == ((AHC_MODE_RECORD_VONLY<<16)|AHC_SYS_VIDRECD_PAUSE))  ){
                VideoFunc_RecordResume();
                DrawStateVideoRecUpdate(EVENT_VIDEO_KEY_RECORD);
            }
            else if((CurSysMode == ((AHC_MODE_VIDEO_RECORD<<16)|AHC_SYS_VIDRECD_RESUME)) ||
                    (CurSysMode == ((AHC_MODE_VIDEO_RECORD<<16)|AHC_SYS_VIDRECD_START))  ||
                    (CurSysMode == ((AHC_MODE_RECORD_VONLY<<16)|AHC_SYS_VIDRECD_RESUME)) ||
                    (CurSysMode == ((AHC_MODE_RECORD_VONLY<<16)|AHC_SYS_VIDRECD_START))  ) {
                VideoFunc_RecordPause();
                DrawStateVideoRecUpdate(ulJobEvent);
            }
        break;

        case EVENT_VIDEO_TOUCH_MENU               :
        case EVENT_KEY_MENU                       :
            #if defined(WIFI_PORT) && (WIFI_PORT == 1)
            if(AHC_STREAM_OFF != AHC_GetStreamingMode())
                break;
            #endif

            if (!VideoFunc_RecordStatus())
            {
                // Clean downcount screen
                #if MOTION_DETECTION_EN
                if (m_ulVMDRemindTime)
                    UpdateMotionRemindTime(-1);
                #endif

                DrawStateVideoRecUpdate(ulJobEvent);
                VideoTimer_Stop();
                #ifdef CFG_DRAW_SCREEN_128 //may be defined in config_xxx.h
                // I don't know why!! In Video/Camera Preview mode, to on/off menu, display will hangup!
                // Off Preview for LCD is 128x128, Canlet
                AHC_SetMode(AHC_MODE_IDLE);
                #endif
                ret = StateSwitchMode(UI_VIDEO_MENU_STATE);
                AHC_OSDSetActive(OVL_DISPLAY_BUFFER, 1);
            }
        break;

        case EVENT_VIDEO_TOUCH_MODE               :
        case EVENT_KEY_MODE                       :
            #if defined(WIFI_PORT) && (WIFI_PORT == 1)
            if(AHC_STREAM_OFF != AHC_GetStreamingMode())
                break;
            #endif

            AHC_PauseKeyUI();

            if(!VideoFunc_RecordStatus())
            {
                DrawStateVideoRecUpdate(ulJobEvent);
                VideoTimer_Stop();

                #if (VR_PREENCODE_EN)
                bDisableVideoPreRecord = AHC_TRUE;
                #endif

                #if (MOTION_DETECTION_EN)
                // Reset MVD downcount to zero when switch to DSC mode
                if (m_ubInRemindTime) {
                    m_ulVMDRemindTime = 0;
                    m_ubInRemindTime = 0;
                }
                #endif

                #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
                if( uiGetParkingModeEnable() == AHC_TRUE )
                {
                    uiSetParkingModeEnable( AHC_FALSE );
                    AIHC_StopVideoRecordMode(AHC_TRUE,AHC_FALSE);
                }
                #endif

                #if (UI_MODE_SELECT_EN)
                ret = StateSwitchMode(UI_MODESELECT_MENU_STATE);
                #elif (DSC_MODE_ENABLE)
                ret = StateSwitchMode(UI_CAMERA_STATE);
                #else
                ret = StateSwitchMode(UI_BROWSER_STATE);
                #endif
            }

            AHC_ResumeKeyUI();
        break;
       
	#ifdef NET_SYNC_PLAYBACK_MODE
		case EVENT_NET_ENTER_PLAYBACK : 
			AHC_PauseKeyUI();
			
			if (VideoFunc_RecordStatus())
			{
                #ifdef CFG_ENABLE_MIN_VR_TIME
                // For min VR time
                UINT32 ulTime = CFG_ENABLE_MIN_VR_TIME;

                AHC_GetCurrentRecordingTime(&ulTime);

                if (ulTime < CFG_ENABLE_MIN_VR_TIME)
                    AHC_OS_SleepMs(CFG_ENABLE_MIN_VR_TIME - ulTime);
                #endif

                StateVideoRecMode_StopRecordingProc(ulJobEvent);
            }
            

            DrawStateVideoRecUpdate(ulJobEvent);
            VideoTimer_Stop();

            #if (VR_PREENCODE_EN)
            bDisableVideoPreRecord = AHC_TRUE;
            #endif

            #if (MOTION_DETECTION_EN)
            // Reset MVD downcount to zero when switch to DSC mode
            if (m_ubInRemindTime) {
            	m_ulVMDRemindTime = 0;
                m_ubInRemindTime = 0;
            }
            #endif

            #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
            if( uiGetParkingModeEnable() == AHC_TRUE )
            {
                uiSetParkingModeEnable( AHC_FALSE );
                AIHC_StopVideoRecordMode(AHC_TRUE,AHC_FALSE);
            }
            #endif

            ret = StateSwitchMode(UI_NET_PLAYBACK_STATE);

            AHC_ResumeKeyUI();
            
            CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
        break;
    #endif//NET_SYNC_PLAYBACK_MODE
        
        case EVENT_VIDEO_TOUCH_RECORD_PRESS       :
        case EVENT_VIDEO_KEY_RECORD               :
            printc("@@@ EVENT_VIDEO_KEY_RECORD -\r\n");

            #if (FS_FORMAT_FREE_ENABLE)
            if( SystemSettingConfig()->byNeedToFormatMediaAsFormatFree > 0 )
            {
                printc(FG_RED("MediaError!!! Need to format media as FORMAT FREE type!!!\r\n"));
                break;
            }
            #endif

            if(bAudioRecording)
                break;

            AHC_PauseKeyUI();

            if (VideoFunc_RecordStatus()){
                #ifdef CFG_ENABLE_MIN_VR_TIME
                // For min VR time
                UINT32 ulTime = CFG_ENABLE_MIN_VR_TIME;

                AHC_GetCurrentRecordingTime(&ulTime);

                if (ulTime < CFG_ENABLE_MIN_VR_TIME)
                    AHC_OS_SleepMs(CFG_ENABLE_MIN_VR_TIME - ulTime);
                #endif

                StateVideoRecMode_StopRecordingProc(ulJobEvent);
            }
            else{
                StateVideoRecMode_StartRecordingProc(ulJobEvent);
            }

            AHC_ResumeKeyUI();
        break;

        case EVENT_KEY_PLAYBACK_MODE              :
            if(!VideoFunc_RecordStatus())
            {
                DrawStateVideoRecUpdate(ulJobEvent);
                VideoTimer_Stop();

                #if (VR_PREENCODE_EN)
                bDisableVideoPreRecord = AHC_TRUE;
                #endif
                ret = StateSwitchMode(UI_BROWSER_STATE);
            }
        break;

        case EVENT_POWER_OFF                      :
            AHC_PauseKeyUI();

            if( VideoFunc_RecordStatus() )
            {
                ret = VideoFunc_RecordStop();
                DrawStateVideoRecUpdate(EVENT_VIDEO_KEY_RECORD_STOP);
                AHC_OS_Sleep(100);
            }

            #ifdef CFG_BOOT_CLEAR_ICON_WHEN_OFF //may be defined in config_xxx.h
            //If LCD gets poor quality, and icons is sticky
            //Need to clear icons
            DrawStateVideoRecClearIcon();
            #endif

            AHC_NormalPowerOffPath();
            AHC_ResumeKeyUI();
        break;

        //Special Event
        case EVENT_CHANGE_LED_MODE:
#if (LED_FLASH_CTRL!=LED_BY_NONE)
            AHC_ToggleFlashLED(LED_MODE_ON_OFF);
#endif
        break;

#ifdef LED_GPIO_LASER
        case  EVENT_LASER_LED_ONOFF:
            if (LED_GPIO_LASER != MMP_GPIO_MAX)
            {
                static MMP_UBYTE flag = AHC_FALSE;

                if(!flag)
                {
                    LedCtrl_LaserLed(LED_GPIO_LASER_ACT_LEVEL);
                    flag = AHC_TRUE;
                }
                else
                {
                    LedCtrl_LaserLed(!LED_GPIO_LASER_ACT_LEVEL);
                    flag = AHC_FALSE;
                }
            }
        break;
#endif

        case EVENT_FORMAT_MEDIA:
        case EVENT_FORMAT_RESET_ALL:
#if (QUICK_FORMAT_SD_EN || POWER_ON_BUTTON_ACTION)
            printc(">>> EVENT_FORMAT_MEDIA or EVENT_FORMAT_RESET_ALL\n");

            if( VideoFunc_RecordStatus() )
            {
                StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);

                AHC_OS_SleepMs(500);
                QuickMediaOperation(MEDIA_CMD_FORMAT);

                AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, KEY_VIDEO_RECORD, 0);
            }
            else
            {
                QuickMediaOperation(MEDIA_CMD_FORMAT);
            }
#endif
        break;

        case EVENT_VIDEO_KEY_CAPTURE:
            if(AHC_SDMMC_BasicCheck()==AHC_FALSE) {
                #if (USE_SHUTTER_SOUND)
                AHC_PlaySoundEffect(AHC_SOUNDEFFECT_BUTTON);
                #endif
                // Call trigger the op is failed!!
                VideoFunc_ShutterFail();

                break;
            }

            AHC_PauseKeyUI();

//Switch to DSC Mode then switch back
#ifdef KEY_CAPATURE_SWITCH_VIDEO //may be defined in config_xxx.h
            if(!VideoFunc_RecordStatus())
            {
                DrawStateVideoRecUpdate(EVENT_KEY_MODE);
                VideoTimer_Stop();

                #if (VR_PREENCODE_EN)
                bDisableVideoPreRecord = AHC_TRUE;
                #endif

                ret = StateSwitchMode(UI_CAMERA_STATE);

                if(ret)
                    SetKeyPadEvent(KEY_DSC_CAPTURE);
                else
                    VideoFunc_Shutter();
            }
            else
#endif
            {
                DCF_DB_TYPE sType = AHC_UF_GetDB();
                AHC_BOOL bSuccess;
                #if (DCF_DB_COUNT >= 4)
                AHC_UF_SelectDB(DCF_DB_TYPE_4TH_DB);
                #endif

                #if defined(WIFI_PORT) && (WIFI_PORT == 1)
                #if (HANDLE_JPEG_EVENT_BY_QUEUE == 0)
                AHC_SetStreamingMode(AHC_STREAM_PAUSE);
                #endif
                #endif

                #if (USE_SHUTTER_SOUND)
                AHC_PlaySoundEffect(AHC_SOUNDEFFECT_SHUTTER);
                #endif

                #ifdef CFG_CAPTURE_WITH_VIBRATION
                AHC_Vibration_Enable(CFG_CAPTURE_VIBRATION_TIME);
                #endif

                #ifdef CFG_CAPTURE_WITH_KEY_LED_CAPTURE
                LedCtrl_ButtonLed(KEY_LED_CAPTURE, AHC_TRUE);
                #endif

                DrawStateVideoRecUpdate(EVENT_VIDEO_KEY_CAPTURE);
                bSuccess = VideoFunc_Shutter();

                #if defined(WIFI_PORT) && (WIFI_PORT == 1)
                #if (HANDLE_JPEG_EVENT_BY_QUEUE == 0)
                AHC_SetStreamingMode(AHC_STREAM_RESUME);
                #endif
                netapp_CGIOpFeedback((void*)ulJobEvent, (int)(bSuccess? 0:-1)) ;
                #endif

                if (VideoFunc_RecordStatus()) {
                    DrawStateVideoRecUpdate(EVENT_VIDEO_KEY_RECORD);
                } else {
                    DrawStateVideoRecUpdate(EVENT_VIDEO_KEY_RECORD_STOP);
                }

                AHC_UF_SelectDB(sType);
            }

            AHC_ResumeKeyUI();
        break;

        case EVENT_RECORD_MUTE              :
            AHC_ToggleMute();
        break;

        case EVENT_LCD_POWER_SAVE           :
            AHC_SwitchLCDBackLight();
        break;

        case EVENT_CUS_SW1_ON:
            if(!VideoFunc_RecordStatus())
            {
                printc("MenuSettingConfig()->uiMOVSize %d\n", MenuSettingConfig()->uiMOVSize);
                if(MenuSettingConfig()->uiMOVSize == MOVIE_SIZE_1080P)
                {
                    MenuSettingConfig()->uiMOVSize= MOVIE_SIZE_720_60P;
                    AHC_SetMode(AHC_MODE_IDLE);
                    VideoFunc_Preview();
                }
            }
        break;

        case EVENT_CUS_SW1_OFF:
            if(!VideoFunc_RecordStatus())
            {
                printc("MenuSettingConfig()->uiMOVSize %d\n", MenuSettingConfig()->uiMOVSize);
                if(MenuSettingConfig()->uiMOVSize == MOVIE_SIZE_720_60P)
                {
                    MenuSettingConfig()->uiMOVSize= MOVIE_SIZE_1080P;	//MOVIE_SIZE_720P;
                    AHC_SetMode(AHC_MODE_IDLE);
                    VideoFunc_Preview();
                }
            }
        break;

        case EVENT_CHANGE_NIGHT_MODE        :
            AHC_ToggleTwilightMode();
        break;

        case EVENT_CAMERA_BROWSER:
        case EVENT_VIDEO_BROWSER:
        case EVENT_ALL_BROWSER:	
            if(!VideoFunc_RecordStatus())
            {
                DrawStateVideoRecUpdate(EVENT_KEY_PLAYBACK_MODE);
                VideoTimer_Stop();

                #if (VR_PREENCODE_EN)
                bDisableVideoPreRecord = AHC_TRUE;
                #endif

                if(ulJobEvent==EVENT_CAMERA_BROWSER)
                    SetCurrentOpMode(JPGPB_MODE);
                else if(ulJobEvent==EVENT_VIDEO_BROWSER)
                    SetCurrentOpMode(MOVPB_MODE);
                else if(ulJobEvent==EVENT_ALL_BROWSER)
                    SetCurrentOpMode(JPGPB_MOVPB_MODE);

                StateSwitchMode(UI_BROWSER_STATE);
            }
        break;

        case EVENT_CAMERA_PREVIEW                       :
            if(!VideoFunc_RecordStatus())
            {
                DrawStateVideoRecUpdate(EVENT_KEY_MODE);
                VideoTimer_Stop();

                #if (VR_PREENCODE_EN)
                bDisableVideoPreRecord = AHC_TRUE;
                #endif

                ret = StateSwitchMode(UI_CAMERA_STATE);

            }
        break;

        case EVENT_AUDIO_RECORD:
            if(VideoFunc_RecordStatus())
                break;

            if(bAudioRecording)
            {
                if(AHC_RecordAudioCmd(AHC_AUDIO_CODEC_MP3, AHC_AUDIO_RECORD_STOP))
                {
                    bAudioRecording = AHC_FALSE;
                    DrawStateVideoRecUpdate(EVENT_VIDEO_KEY_RECORD_STOP);
                }
            }
            else
            {
                if(AHC_SDMMC_BasicCheck()==AHC_FALSE)
                    break;

                if(AHC_RecordAudioCmd(AHC_AUDIO_CODEC_MP3, AHC_AUDIO_RECORD_START))
                {
                    bAudioRecording = AHC_TRUE;
                    DrawStateVideoRecUpdate(EVENT_VIDEO_KEY_RECORD);
                }
            }
        break;

        case EVENT_SWITCH_VMD_MODE:
#if (MOTION_DETECTION_EN)
            #if defined(WIFI_PORT) && (WIFI_PORT == 1)
            if (WLAN_SYS_GetMode() != -1) {
                if(AHC_Get_WiFi_Streaming_Status())
                    break;
            }
            #endif

            if (!m_ubMotionDtcEn &&
                #if VR_PREENCODE_EN
                !m_ubPreEncodeEn &&
                #endif
                1)
            {
                VideoFunc_EnterVMDMode();

                #ifdef CFG_LCD_RESET_POWER_SAVER_IN_VMD //may be defined in config_xxx.h
                LCDPowerSaveCounterReset();
                #endif
            }
            else
            {
                VideoFunc_ExitVMDMode();

                #ifdef CFG_LCD_RESET_POWER_SAVER_IN_VMD //may be defined in config_xxx.h
                LCDPowerSaveCounterReset();
                #endif
            }
#endif
        break;

        case EVENT_SWITCH_PANEL_TVOUT:
            AHC_SwitchLCDandTVOUT();
        break;

#if (SUPPORT_TOUCH_PANEL) && !defined(_OEM_TOUCH_)
        case EVENT_SWITCH_TOUCH_PAGE:
            m_ubShowVRTouchPage ^= 1;

            if(m_ubShowVRTouchPage)
            {
                KeyParser_ResetTouchVariable();
                KeyParser_TouchItemRegister(&VideoCtrlPage_TouchButton[0], ITEMNUM(VideoCtrlPage_TouchButton));

                DrawVideoRecStatePageSwitch(TOUCH_CTRL_PAGE);
            }
            else
            {
                KeyParser_ResetTouchVariable();
                KeyParser_TouchItemRegister(&VideoMainPage_TouchButton[0], ITEMNUM(VideoMainPage_TouchButton));

                DrawVideoRecStatePageSwitch(TOUCH_MAIN_PAGE);
            }
        break;

        #if 0
        case EVENT_VIDEO_TOUCH_MENU                       :
                   #if defined(WIFI_PORT) && (WIFI_PORT == 1)
                   if(AHC_STREAM_OFF != AHC_GetStreamingMode())
                       break;
                   #endif
                   if (!VideoFunc_RecordStatus())
                   {
                       // Clean downcount screen
                       #if MOTION_DETECTION_EN
                       if (m_ulVMDRemindTime)
                           UpdateMotionRemindTime(-1);
                       #endif

                       DrawStateVideoRecUpdate(ulJobEvent);
                       VideoTimer_Stop();
                       #ifdef CFG_DRAW_SCREEN_128 //may be defined in config_xxx.h
                       // I don't know why!! In Video/Camera Preview mode, to on/off menu, display will hangup!
                       // Off Preview for LCD is 128x128, Canlet
                       AHC_SetMode(AHC_MODE_IDLE);
                       #endif
                       ret = StateSwitchMode(UI_VIDEO_MENU_STATE);
                       AHC_OSDSetActive(OVL_DISPLAY_BUFFER, 1);
                   }
               break;

        case EVENT_VIDEO_TOUCH_MODE                       :
                    #if defined(WIFI_PORT) && (WIFI_PORT == 1)
                    if(AHC_STREAM_OFF != AHC_GetStreamingMode())
                        break;
                    #endif

                    if(!VideoFunc_RecordStatus())
                    {
                        DrawStateVideoRecUpdate(ulJobEvent);
                        VideoTimer_Stop();

                        #if (VR_PREENCODE_EN)
                        bDisableVideoPreRecord = AHC_TRUE;
                        #endif

                        #if (MOTION_DETECTION_EN)
                        // Reset MVD downcount to zero when switch to DSC mode
                        if (m_ubInRemindTime) {
                            m_ulVMDRemindTime = 0;
                            m_ubInRemindTime = 0;
                        }
                        #endif

                        #if (UI_MODE_SELECT_EN)
                        ret = StateSwitchMode(UI_MODESELECT_MENU_STATE);
                        #elif (DSC_MODE_ENABLE)
                        ret = StateSwitchMode(UI_CAMERA_STATE);
                        #else
                        ret = StateSwitchMode(UI_BROWSER_STATE);
                        #endif
                    }
                break;
        case EVENT_VIDEO_TOUCH_RECORD_PRESS                 :
            printc("@@@ EVENT_VIDEO_TOUCH_RECORD_PRESS -\r\n");

            if(bAudioRecording)
                break;

            if (VideoFunc_RecordStatus())
            {
                StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
                break;
            }
            else
            {
                VideoRecordStatus retVal;

                if(AHC_SDMMC_BasicCheck()==AHC_FALSE)
                {
                    #ifdef CFG_VIDEO_RECORD_FLICK_LED_BY_CARD_ERROR
                    LedCtrl_FlickerLedByCustomer(LED_GPIO_SDMMC_ERROR, LED_GPIO_SDMMC_FLICK_PERIOD, LED_GPIO_SDMMC_FLICK_TIMES);
                    #endif

                    RTNA_DBG_Str(0, "--E-- Memory Card Error\r\n");
                    break;
                }

                #if defined(CFG_ENABLE_VIDEO_REC_VIBRATION) && defined(CFG_VIDEO_REC_VIBRATION_TIME)
                if (AHC_SDMMC_GetMountState())
                {
                    UINT64 ulFreeSpace;

                    VideoFunc_GetFreeSpace(&ulFreeSpace);

                    if (ulFreeSpace)
                    {
                        AHC_Vibration_Enable(CFG_VIDEO_REC_VIBRATION_TIME);

                        #if defined(CFG_VIDEO_REC_VIBRATION_TIMES) && (CFG_VIDEO_REC_VIBRATION_TIMES >= 2)
                        {
                            UINT16 times = CFG_VIDEO_REC_VIBRATION_TIMES - 1;

                            while (times--) {
                                AHC_OS_SleepMs(CFG_VIDEO_REC_VIBRATION_TIME + 100);
                                AHC_Vibration_Enable(CFG_VIDEO_REC_VIBRATION_TIME);
                            }
                        }
                        #endif

                        AHC_OS_SleepMs(CFG_VIDEO_REC_VIBRATION_TIME);
                    }
                }
                #endif  // defined(CFG_ENABLE_VIDEO_REC_VIBRATION) && defined(CFG_VIDEO_REC_VIBRATION_TIME)

                RecordTimeOffset = 0;
                retVal = VideoFunc_Record();
                RTNA_DBG_Str(0, "VideoFunc_Record return");
                RTNA_DBG_Short(0, retVal);
                RTNA_DBG_Str(0, "\r\n");

                switch (retVal) {
                case VIDEO_REC_START:
                    #ifdef CFG_ENABLE_VIDEO_REC_LED
                    RecLED_Timer_Start(100);
                    #endif
                    break;

                case VIDEO_REC_NO_SD_CARD:
                case VIDEO_REC_CARD_FULL:
                    #ifdef CFG_VIDEO_RECORD_FLICK_LED_BY_CARD_ERROR
                    LedCtrl_FlickerLedByCustomer(LED_GPIO_SDMMC_ERROR, LED_GPIO_SDMMC_FLICK_PERIOD, LED_GPIO_SDMMC_FLICK_TIMES);
                    #endif
                    break;
                }

                #if 0//(BUTTON_BIND_LED)
                if(VIDEO_REC_START != retVal)
                    LedCtrl_ButtonLed(AHC_GetVideoModeLedGPIOpin(), AHC_FALSE);
                #endif

                if (VIDEO_REC_NO_SD_CARD == retVal) {
                    AHC_WMSG_Draw(AHC_TRUE, WMSG_INSERT_SD_AGAIN, 3);
                    break;
                }
                else if (VIDEO_REC_CARD_FULL == ret) {
                    AHC_WMSG_Draw(AHC_TRUE, WMSG_STORAGE_FULL, 3);
                    break;
                }
                else if(VIDEO_REC_SEAMLESS_ERROR == ret){
                    AHC_WMSG_Draw(AHC_TRUE, WMSG_SEAMLESS_ERROR, 3);
                    break;
                }
                else if(VIDEO_REC_SD_CARD_ERROR == ret){
                    AHC_WMSG_Draw(AHC_TRUE, WMSG_FORMAT_SD_CARD, 3);
                    break;
                }
            }

            DrawStateVideoRecUpdate(ulJobEvent);
            break;
#endif
        #endif
        //Device
        case EVENT_DC_CABLE_IN:
            #if defined(VMD_EN_BY_CHARGER_OUT)
            VideoFunc_ExitVMDMode();
            LedCtrl_LcdBackLight(AHC_TRUE);
            #endif

            #if (CHARGER_IN_ACT_VIDEO_MODE==ACT_RECORD_VIDEO)
            if(VideoFunc_RecordStatus()==AHC_FALSE)
                AHC_SetRecordByChargerIn(3);
            #elif (CHARGER_IN_ACT_VIDEO_MODE == ACT_FORCE_POWER_OFF)
            SetKeyPadEvent(KEY_POWER_OFF);
            #endif
        break;

        case EVENT_DC_CABLE_OUT:
            AHC_SetChargerEnable(AHC_FALSE);

            #if defined(VMD_EN_BY_CHARGER_OUT)
            if(MenuSettingConfig()->uiMotionDtcSensitivity != MOTION_DTC_SENSITIVITY_OFF)
            {
                LedCtrl_LcdBackLight(AHC_FALSE);
                if( VideoFunc_RecordStatus() )
                {
                    VideoFunc_RecordStop();
                    AHC_OS_Sleep(100);
                }

                VideoFunc_EnterVMDMode();
            }
            else
            #endif
            {
                if( VideoFunc_RecordStatus() )
                {
                    #if (CHARGER_OUT_ACT_VIDEO_REC==ACT_FORCE_POWER_OFF || CHARGER_OUT_ACT_VIDEO_REC==ACT_DELAY_POWER_OFF)
                    AHC_SetShutdownByChargerOut(AHC_TRUE);
                    #endif
                }
                else
                {
                    #if (CHARGER_OUT_ACT_OTHER_MODE==ACT_FORCE_POWER_OFF || CHARGER_OUT_ACT_OTHER_MODE==ACT_DELAY_POWER_OFF)
                    AHC_SetShutdownByChargerOut(AHC_TRUE);
                    #endif
                }
            }
        break;

        case EVENT_USB_DETECT:
            if(IsAdapter_Connect())
            {
        #ifdef CFG_REC_IGNORE_USB //may be defined in config_xxx.h
                if (VideoFunc_RecordStatus()) {
                    // Video Recording to ignore USB Charger
                    break;
                }
                else {
                    //Power Off
                }
        #else
                #if defined(VMD_EN_BY_CHARGER_OUT)
                VideoFunc_ExitVMDMode();
                LedCtrl_LcdBackLight(AHC_TRUE);
                #endif

                #if (CHARGER_IN_ACT_VIDEO_MODE==ACT_RECORD_VIDEO)
                if(VideoFunc_RecordStatus()==AHC_FALSE)
                    AHC_SetRecordByChargerIn(3);
                #elif (CHARGER_IN_ACT_VIDEO_MODE == ACT_FORCE_POWER_OFF)
                SetKeyPadEvent(KEY_POWER_OFF);
                #endif
        #endif
            }
            else
            {
                #ifdef CFG_REC_IGNORE_USB //may be defined in config_xxx.h
                if (VideoFunc_RecordStatus()) {
                    // Video Recording to ignore USB
                    break;
                }
                else {
                    VideoTimer_Stop();
                }
                #else
                if( VideoFunc_RecordStatus() )
                {
                    StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
                    AHC_WaitVideoWriteFileFinish();
                }

                VideoTimer_Stop();
                #endif

                #if (VR_PREENCODE_EN)
                bDisableVideoPreRecord = AHC_TRUE;
                #endif

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

        case EVENT_USB_REMOVED:
            /* In Video state, the event should always be USB adapter out,
             * the real USB plug-out event should be received in MSDC state.
             *
             * The function AHC_USB_GetLastStatus() may return wrong status,
             * when USB plug-in/out then adapter plug-in.
             *
             */
            if (AHC_IsDcCableConnect() == AHC_TRUE)
                break;

			#if (USB_MODE_SELECT_EN)
			if(ubUSBSelectedMode == USB_PTP_MODE)
				break;
			#endif
			
            AHC_SetChargerEnable(AHC_FALSE);
            #if defined(VMD_EN_BY_CHARGER_OUT)
            if(MenuSettingConfig()->uiMotionDtcSensitivity != MOTION_DTC_SENSITIVITY_OFF)
            {
                LedCtrl_LcdBackLight(AHC_FALSE);

                if( VideoFunc_RecordStatus() )
                {
                    VideoFunc_RecordStop();
                    AHC_OS_Sleep(100);
                }

                VideoFunc_EnterVMDMode();
            }
            else
            #endif
            {
                if( VideoFunc_RecordStatus() )
                {
                    #if (CHARGER_OUT_ACT_VIDEO_REC==ACT_FORCE_POWER_OFF || CHARGER_OUT_ACT_VIDEO_REC==ACT_DELAY_POWER_OFF)
                    AHC_SetShutdownByChargerOut(AHC_TRUE);
                    #endif
                }
                else
                {
                    #if (CHARGER_OUT_ACT_OTHER_MODE==ACT_FORCE_POWER_OFF || CHARGER_OUT_ACT_OTHER_MODE==ACT_DELAY_POWER_OFF)
                    AHC_SetShutdownByChargerOut(AHC_TRUE);
                    #endif
                }
            }

            #if((EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_DUAL_FILE)||(EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE))
            AHC_SetKernalEmergencyStopStep(AHC_KERNAL_EMERGENCY_AHC_DONE);
            #endif
        break;

        case EVENT_VIDEO_KEY_SWAP_PIP:
        {
        	#if(SUPPORT_DUAL_SNR)
        	AHC_WINDOW_RECT sFrontRect,sRearRect;
        	#endif
        	MMP_DISPLAY_WIN_ATTR winattr;
        	MMP_DISPLAY_DISP_ATTR	dispAtt = {0};

	        #if (UVC_HOST_VIDEO_ENABLE)
	        UINT8 action = 1;
			USB_DETECT_PHASE USBCurrentStates = 0;

	        printc("EVENT_VIDEO_KEY_SWAP_PIP \n");

            AHC_USBGetStates(&USBCurrentStates,AHC_USB_GET_PHASE_CURRENT);
            
            if (USBCurrentStates == USB_DETECT_PHASE_REAR_CAM)
            {
                action = 0;
            }

            if (action == 0)
            {
                gbWinExchangedCnt++;


                MMPS_Display_SetWinActive(FRONT_CAM_WINDOW_ID , MMP_FALSE);
                MMPS_Display_SetWinActive(REAR_CAM_WINDOW_ID , MMP_FALSE);
                
                switch (gbWinExchangedCnt % PIP_SWAP_TYPE_NUM)
                {
                    case F_LARGE_R_SMALL:
                    	printc("F_LARGE_R_SMALL \r\n");
                        MMPS_Display_SetWinPriority(MMP_DISPLAY_WIN_OSD,MMP_DISPLAY_WIN_MAIN,MMP_DISPLAY_WIN_OVERLAY,MMP_DISPLAY_WIN_PIP);

                        dispAtt.usDisplayWidth = RTNA_LCD_GetAttr()->usPanelW;
                        dispAtt.usDisplayHeight = RTNA_LCD_GetAttr()->usPanelH;
                        MMPD_Display_GetWinAttributes(FRONT_CAM_WINDOW_ID, &winattr);

                        MMPS_Display_SetWindowAttrToDisp(FRONT_CAM_WINDOW_ID, winattr , dispAtt);

                        AHC_HostUVCVideoSetWinAttribute(REAR_CAM_WINDOW_ID, (RTNA_LCD_GetAttr()->usPanelW >> 1), (RTNA_LCD_GetAttr()->usPanelH >>1), 0,0,MMP_SCAL_FITMODE_OPTIMAL,AHC_FALSE);//RTNA_LCD_GetAttr()->usPanelW >> 1, RTNA_LCD_GetAttr()->usPanelH >>1, AHC_FALSE);


                        MMPS_Display_SetWinActive(FRONT_CAM_WINDOW_ID , AHC_TRUE);
                        MMPS_Display_SetWinActive(REAR_CAM_WINDOW_ID , AHC_TRUE);
                        break;
                    case F_SMALL_R_LARGE:
                    	printc("F_SMALL_R_LARGE \r\n");
                        MMPS_Display_SetWinPriority(MMP_DISPLAY_WIN_OSD,MMP_DISPLAY_WIN_MAIN,MMP_DISPLAY_WIN_PIP,MMP_DISPLAY_WIN_OVERLAY);

                        MMPS_Display_SetWinScaleAndOffset(FRONT_CAM_WINDOW_ID, MMP_SCAL_FITMODE_OPTIMAL,
                        		RTNA_LCD_GetAttr()->usPanelW, RTNA_LCD_GetAttr()->usPanelH, RTNA_LCD_GetAttr()->usPanelW >> 1, RTNA_LCD_GetAttr()->usPanelH >>1, 0, 0);

                        AHC_HostUVCVideoSetWinAttribute(REAR_CAM_WINDOW_ID, RTNA_LCD_GetAttr()->usPanelW, RTNA_LCD_GetAttr()->usPanelH, 0,0,MMP_SCAL_FITMODE_OPTIMAL, AHC_FALSE);

                        MMPS_Display_SetWinActive(FRONT_CAM_WINDOW_ID , AHC_TRUE);
                        MMPS_Display_SetWinActive(REAR_CAM_WINDOW_ID , AHC_TRUE);
                        break;
                    case ONLY_FRONT:
                    	printc("ONLY_FRONT \r\n");
                    	MMPS_Display_SetWinPriority(MMP_DISPLAY_WIN_OSD,MMP_DISPLAY_WIN_MAIN,MMP_DISPLAY_WIN_PIP,MMP_DISPLAY_WIN_OVERLAY);

                    	dispAtt.usDisplayWidth = RTNA_LCD_GetAttr()->usPanelW;
                    	dispAtt.usDisplayHeight = RTNA_LCD_GetAttr()->usPanelH;
                    	MMPD_Display_GetWinAttributes(FRONT_CAM_WINDOW_ID, &winattr);

                    	MMPS_Display_SetWindowAttrToDisp(FRONT_CAM_WINDOW_ID, winattr , dispAtt);

                    	AHC_HostUVCVideoSetWinAttribute(REAR_CAM_WINDOW_ID, (RTNA_LCD_GetAttr()->usPanelW >> 1), (RTNA_LCD_GetAttr()->usPanelH >>1), 0,0,MMP_SCAL_FITMODE_OPTIMAL,AHC_FALSE);//RTNA_LCD_GetAttr()->usPanelW >> 1, RTNA_LCD_GetAttr()->usPanelH >>1, AHC_FALSE);


                    	MMPS_Display_SetWinActive(FRONT_CAM_WINDOW_ID , AHC_TRUE);
                    	MMPS_Display_SetWinActive(REAR_CAM_WINDOW_ID , MMP_FALSE);
                    	break;
                    case ONLY_REAR:
                    	printc("ONLY_REAR \r\n");
                    	MMPS_Display_SetWinPriority(MMP_DISPLAY_WIN_OSD,MMP_DISPLAY_WIN_MAIN,MMP_DISPLAY_WIN_OVERLAY,MMP_DISPLAY_WIN_PIP);

                    	MMPS_Display_SetWinScaleAndOffset(FRONT_CAM_WINDOW_ID, MMP_SCAL_FITMODE_OPTIMAL,
                    			RTNA_LCD_GetAttr()->usPanelW, RTNA_LCD_GetAttr()->usPanelH, RTNA_LCD_GetAttr()->usPanelW >> 1, RTNA_LCD_GetAttr()->usPanelH >>1, 0, 0);

                    	AHC_HostUVCVideoSetWinAttribute(REAR_CAM_WINDOW_ID, RTNA_LCD_GetAttr()->usPanelW, RTNA_LCD_GetAttr()->usPanelH, 0,0,MMP_SCAL_FITMODE_OPTIMAL, AHC_FALSE);

                    	MMPS_Display_SetWinActive(FRONT_CAM_WINDOW_ID , MMP_FALSE);
                    	MMPS_Display_SetWinActive(REAR_CAM_WINDOW_ID , AHC_TRUE);
                    	break;
                    case F_TOP_R_BOTTOM:
                    	printc("F_TOP_R_BOTTOM \r\n");

                    	MMPS_Display_SetWinScaleAndOffset(FRONT_CAM_WINDOW_ID, MMP_SCAL_FITMODE_OUT,
                    			RTNA_LCD_GetAttr()->usPanelW, RTNA_LCD_GetAttr()->usPanelH, RTNA_LCD_GetAttr()->usPanelW , RTNA_LCD_GetAttr()->usPanelH >>1, 0, 0);

                    	dispAtt.usDisplayWidth = RTNA_LCD_GetAttr()->usPanelW;
                    	dispAtt.usDisplayHeight = RTNA_LCD_GetAttr()->usPanelH >> 1;
                    	dispAtt.usStartX = 0;
                    	dispAtt.usStartY = RTNA_LCD_GetAttr()->usPanelH >> 2;
                    	MMPD_Display_GetWinAttributes(FRONT_CAM_WINDOW_ID, &winattr);

                    	MMPS_Display_SetWindowAttrToDisp(FRONT_CAM_WINDOW_ID, winattr , dispAtt);

                    	AHC_HostUVCVideoSetWinAttribute(REAR_CAM_WINDOW_ID, RTNA_LCD_GetAttr()->usPanelW, RTNA_LCD_GetAttr()->usPanelH >> 1, 0,RTNA_LCD_GetAttr()->usPanelH >> 1,MMP_SCAL_FITMODE_OUT, AHC_FALSE);
                    	MMPS_Display_SetWinActive(FRONT_CAM_WINDOW_ID , AHC_TRUE);
                    	MMPS_Display_SetWinActive(REAR_CAM_WINDOW_ID , AHC_TRUE);
                    	break;
                    case F_LEFT_R_RIGHT:
                    	printc("F_LEFT_R_RIGHT \r\n");

                    	MMPS_Display_SetWinScaleAndOffset(FRONT_CAM_WINDOW_ID, MMP_SCAL_FITMODE_OUT,
                    			RTNA_LCD_GetAttr()->usPanelW, RTNA_LCD_GetAttr()->usPanelH, RTNA_LCD_GetAttr()->usPanelW >> 1, RTNA_LCD_GetAttr()->usPanelH, 0, 0);

                    	dispAtt.usDisplayWidth = RTNA_LCD_GetAttr()->usPanelW >> 1 ;
                    	dispAtt.usDisplayHeight = RTNA_LCD_GetAttr()->usPanelH;
                    	dispAtt.usStartX = RTNA_LCD_GetAttr()->usPanelW >> 2;
                    	dispAtt.usStartY = 0;
                    	MMPD_Display_GetWinAttributes(FRONT_CAM_WINDOW_ID, &winattr);

                    	MMPS_Display_SetWindowAttrToDisp(FRONT_CAM_WINDOW_ID, winattr , dispAtt);


                    	AHC_HostUVCVideoSetWinAttribute(REAR_CAM_WINDOW_ID, RTNA_LCD_GetAttr()->usPanelW >> 1, RTNA_LCD_GetAttr()->usPanelH, RTNA_LCD_GetAttr()->usPanelW >> 1,0,MMP_SCAL_FITMODE_OUT, AHC_FALSE);
                    	MMPS_Display_SetWinActive(FRONT_CAM_WINDOW_ID , AHC_TRUE);
                    	MMPS_Display_SetWinActive(REAR_CAM_WINDOW_ID , AHC_TRUE);
                    	break;
                }
       		}
			#elif (SUPPORT_DUAL_SNR)
            printc("EVENT_VIDEO_KEY_SWAP_PIP \n");
            gbWinExchangedCnt++;

            AHC_PreviewWindowOp(AHC_PREVIEW_WINDOW_OP_GET | AHC_PREVIEW_WINDOW_REAR,&sRearRect);
            AHC_PreviewWindowOp(AHC_PREVIEW_WINDOW_OP_GET | AHC_PREVIEW_WINDOW_FRONT,&sFrontRect);

            MMPS_Display_SetWinActive(FRONT_CAM_WINDOW_ID , MMP_FALSE);
            MMPS_Display_SetWinActive(REAR_CAM_WINDOW_ID , MMP_FALSE);

            switch (gbWinExchangedCnt % 2)
            {
            case F_LARGE_R_SMALL:
            	printc("F_LARGE_R_SMALL \r\n");
            	MMPS_Display_SetWinPriority(MMP_DISPLAY_WIN_OSD,MMP_DISPLAY_WIN_MAIN,MMP_DISPLAY_WIN_OVERLAY,MMP_DISPLAY_WIN_PIP);

            	//printc("lucas 8  Fw %d Fh %d Rw %d Rh %d\r\n", sFrontRect.usWidth, sFrontRect.usHeight, sRearRect.usWidth, sRearRect.usHeight);
            	dispAtt.usDisplayWidth = sFrontRect.usWidth;
            	dispAtt.usDisplayHeight = sFrontRect.usHeight;
            	MMPD_Display_GetWinAttributes(FRONT_CAM_WINDOW_ID, &winattr);
            	MMPS_Display_SetWindowAttrToDisp(FRONT_CAM_WINDOW_ID, winattr , dispAtt);

            	dispAtt.usDisplayWidth = sRearRect.usWidth;
            	dispAtt.usDisplayHeight = sRearRect.usHeight;
            	MMPD_Display_GetWinAttributes(REAR_CAM_WINDOW_ID, &winattr);
            	MMPS_Display_SetWindowAttrToDisp(REAR_CAM_WINDOW_ID, winattr , dispAtt);

            	MMPS_Display_SetWinActive(FRONT_CAM_WINDOW_ID , AHC_TRUE);
            	MMPS_Display_SetWinActive(REAR_CAM_WINDOW_ID , AHC_TRUE);
            	break;
            case F_SMALL_R_LARGE:
            	printc("F_SMALL_R_LARGE \r\n");
            	MMPS_Display_SetWinPriority(MMP_DISPLAY_WIN_OSD,MMP_DISPLAY_WIN_MAIN,MMP_DISPLAY_WIN_PIP,MMP_DISPLAY_WIN_OVERLAY);


				//printc("lucas 7  Fw %d Fh %d Rw %d Rh %d\r\n", sFrontRect.usWidth, sFrontRect.usHeight, sRearRect.usWidth, sRearRect.usHeight);
            	MMPS_Display_SetWinScaleAndOffset(FRONT_CAM_WINDOW_ID, MMP_SCAL_FITMODE_OPTIMAL,
            									sFrontRect.usWidth, sFrontRect.usHeight, sRearRect.usWidth, sRearRect.usHeight, sRearRect.usLeft, sRearRect.usTop);

            	MMPS_Display_SetWinScaleAndOffset(REAR_CAM_WINDOW_ID, MMP_SCAL_FITMODE_OPTIMAL,
            									sRearRect.usWidth, sRearRect.usHeight, sFrontRect.usWidth, sFrontRect.usHeight, sFrontRect.usLeft, sFrontRect.usTop);
            	MMPS_Display_SetWinActive(FRONT_CAM_WINDOW_ID , AHC_TRUE);
            	MMPS_Display_SetWinActive(REAR_CAM_WINDOW_ID , AHC_TRUE);
            	break;
            }
			#endif
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

        case EVENT_SD_REMOVED                     :
            if(VideoFunc_RecordStatus())
            {
                StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
                AHC_WMSG_Draw(AHC_TRUE, WMSG_NO_CARD, 3);
            }
            if(AHC_TRUE == AHC_SDMMC_GetMountState())
            {
                AHC_DisMountStorageMedia(AHC_MEDIA_MMC);
                Enable_SD_Power(0 /* Power Off */);
            }

            #if((EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_DUAL_FILE)||(EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE))
            AHC_SetKernalEmergencyStopStep(AHC_KERNAL_EMERGENCY_AHC_DONE);
            #endif

            break;

        case EVENT_TV_DETECT                      :
#if (TVOUT_PREVIEW_EN==0)
        #if defined(CFG_TV_PLUGIN_SWITCH_TO_MOVIE_BROWSER_MODE)

            #if (FOCUS_ON_LATEST_FILE)
            AHC_SetPlayFileOrder(PLAY_LAST_FILE);
            #endif

            bPreviewModeTVout = AHC_FALSE;
            SetTVState(AHC_TV_BROWSER_STATUS);
            StateSwitchMode(UI_TVOUT_STATE);

        #elif defined(CFG_TV_PLUGIN_SWITCH_TO_MOVIE_PB_MODE)

            if ((AHC_SDMMC_BasicCheck() == AHC_FALSE) || (TVFunc_BrowserSetting() == 0)) {
                if(bShowTvWMSG) {
                    bShowTvWMSG = AHC_FALSE;
                    AHC_WMSG_Draw(AHC_TRUE, WMSG_NO_CARD, 3);
                }
            }
            else {
                #if (FOCUS_ON_LATEST_FILE)
                AHC_SetPlayFileOrder(PLAY_LAST_FILE);
                #endif
                bPreviewModeTVout = AHC_FALSE;
                SetTVState(AHC_TV_MOVIE_PB_STATUS);
            }

        #else

            if(bShowTvWMSG) {
                bShowTvWMSG = AHC_FALSE;
                AHC_WMSG_Draw(AHC_TRUE, WMSG_HDMI_TV, 3);
            }

        #endif
#else   // #if (TVOUT_PREVIEW_EN==0)
        if (AHC_IsTVConnectEx())
        {
    #ifdef CFG_OEM_SWITCH_TVOUT_MODE
            CFG_OEM_SWITCH_TVOUT_MODE();
    #else
            if(VideoFunc_RecordStatus()){
                StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
            }

            VideoTimer_Stop();
            if( uiGetParkingModeEnable() == AHC_TRUE )
            {
                AIHC_StopVideoRecordMode( AHC_TRUE, AHC_FALSE ); // Stop video Pre-record before uiSetParkingModeEnable( AHC_FALSE ) to avoid un-expected AHC_UF_PostAddFile()

                uiSetParkingModeEnable( AHC_FALSE );

                #if (MOTION_DETECTION_EN)
				if( MenuSettingConfig()->uiMotionDtcSensitivity != MOTION_DTC_SENSITIVITY_OFF )
                {
                    MenuSettingConfig()->uiMotionDtcSensitivity = MOTION_DTC_SENSITIVITY_OFF;
                }
                Menu_SetMotionDtcSensitivity( MenuSettingConfig()->uiMotionDtcSensitivity );
                m_ubVMDStart     = AHC_FALSE;
                m_ulVMDStopCnt   = 0;
                m_ulVMDStableCnt = 0;
                #endif

                AIHC_SetVideoRecordMode( AHC_FALSE, AHC_VIDRECD_INIT );
            }

            #if (VR_PREENCODE_EN)
            bDisableVideoPreRecord = AHC_TRUE;
            #endif

            SetTVState(AHC_TV_VIDEO_PREVIEW_STATUS);
            StateSwitchMode(UI_TVOUT_STATE);
    #endif
        }
#endif  // #if (TVOUT_PREVIEW_EN==0)
        break;

        case EVENT_TV_REMOVED                     :
            bShowTvWMSG = AHC_TRUE;
        break;

        case EVENT_HDMI_DETECT                    :
#if (HDMI_PREVIEW_EN == 0)
        #if defined(CFG_HDMI_PLUGIN_SWITCH_TO_PB_MODE)

        // TBD
/*
        if (VideoFunc_RecordStatus())
        {
            ret = VideoFunc_RecordStop();
        }
        VideoTimer_Stop();
        SetCurrentOpMode(MOVPB_MODE);

        #if (VR_PREENCODE_EN)
        bDisableVideoPreRecord = AHC_TRUE;
        #endif

        #if (FOCUS_ON_LATEST_FILE)
        AHC_SetPlayFileOrder(PLAY_LAST_FILE);
        #endif
        StateSwitchMode(UI_HDMI_STATE);
*/

        #elif defined(CFG_HDMI_PLUGIN_SWITCH_TO_BROWSER_MODE)

        if (VideoFunc_RecordStatus())
        {
            StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
        }

        #if (VR_PREENCODE_EN)
        bDisableVideoPreRecord = AHC_TRUE;
        #endif

        #if (FOCUS_ON_LATEST_FILE)
        AHC_SetPlayFileOrder(PLAY_LAST_FILE);
        #endif

        VideoTimer_Stop();
        #ifdef CFG_CUS_DECIDE_BROWSER_MODE
        CFG_CUS_DECIDE_BROWSER_MODE();
        #else
        SetCurrentOpMode(MOVPB_MODE);
        #endif

        StateSwitchMode(UI_HDMI_STATE);

        #else

        if(bShowHdmiWMSG) {
            bShowHdmiWMSG = AHC_FALSE;
            AHC_WMSG_Draw(AHC_TRUE, WMSG_HDMI_TV, 3);
        }

        #endif

#else   // #if (HDMI_PREVIEW_EN == 0)

        if(VideoFunc_RecordStatus())
            StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);

#if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
		if( uiGetParkingModeEnable() == AHC_TRUE )
		{
			AIHC_StopVideoRecordMode( AHC_TRUE, AHC_FALSE ); // Stop video Pre-record before uiSetParkingModeEnable( AHC_FALSE ) to avoid un-expected AHC_UF_PostAddFile()
		
			uiSetParkingModeEnable( AHC_FALSE );
		
	#if (MOTION_DETECTION_EN)
			if( MenuSettingConfig()->uiMotionDtcSensitivity != MOTION_DTC_SENSITIVITY_OFF )
			{
				MenuSettingConfig()->uiMotionDtcSensitivity = MOTION_DTC_SENSITIVITY_OFF;
			}
			Menu_SetMotionDtcSensitivity( MenuSettingConfig()->uiMotionDtcSensitivity );				
			m_ubVMDStart	 = AHC_FALSE;
			m_ulVMDStopCnt	 = 0;
			m_ulVMDStableCnt = 0;
	#endif
		
			AIHC_SetVideoRecordMode( AHC_FALSE, AHC_VIDRECD_INIT );
			DrawVideoParkingMode( uiGetParkingModeEnable() );
		}
#endif

        #if (VR_PREENCODE_EN)
        bDisableVideoPreRecord = AHC_TRUE;
        #endif

        VideoTimer_Stop();
        SetHDMIState(AHC_HDMI_VIDEO_PREVIEW_STATUS);
        StateSwitchMode(UI_HDMI_STATE);

#endif  // #if (HDMI_PREVIEW_EN == 0)
        break;

        case EVENT_HDMI_REMOVED                   :
            bShowHdmiWMSG = AHC_TRUE;
        break;

        case EVENT_LCD_COVER_OPEN                 :
            if (!LedCtrl_GetBacklightStatus())
                LedCtrl_LcdBackLight(AHC_TRUE);
        break;

        case EVENT_LCD_COVER_CLOSE                :
            LedCtrl_LcdBackLight(AHC_FALSE);
        break;

        case EVENT_LCD_COVER_NORMAL               :
        case EVENT_LCD_COVER_ROTATE               :
            AHC_DrawRotateEvent(ulJobEvent);
        break;

        //Record Callback
        case EVENT_VRCB_MEDIA_FULL:
            #if defined(CFG_VR_DISABLE_CYCLING_RECORDING)
            // No cycling record mode
            if (1)
            #else
            if (MenuSettingConfig()->uiMOVClipTime == MOVIE_CLIP_TIME_OFF)
            #endif
            {
                StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
                AHC_WMSG_Draw(AHC_TRUE, WMSG_STORAGE_FULL, 3);
                #ifdef CFG_CUS_HANDLER_VR_MEDIA_FULL
                CFG_CUS_HANDLER_VR_MEDIA_FULL();
                #endif
            }
            else
            {
                #if 1//For Seamless Exception
                StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
                RecordTimeOffset = 0;
                StateVideoRecMode_StartRecordingProc(EVENT_VIDEO_KEY_RECORD);
                #else
                if(VideoFunc_RecordRestart()==AHC_FALSE)
                {
                    printc("VideoFunc_RecordRestart Fail\r\n");

                    if(VideoFunc_RecordStatus())
                    {
                        StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
                    }
                    break;
                }
                #endif
                DrawStateVideoRecUpdate(EVENT_VIDEO_KEY_RECORD);
            }
            
            if(AHC_GetCurKeyEventID() == BUTTON_VRCB_MEDIA_FULL){
                AHC_SetCurKeyEventID(EVENT_NONE);
            }
            else{
                AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0);
                printc("KeyEventID: BUTTON_VRCB_MEDIA_FULL is interrupted.\r\n");
            }
        break;

        case EVENT_VRCB_FILE_FULL:

            if(AHC_IsVRSeamless()==AHC_FALSE/*MenuSettingConfig()->uiMOVClipTime == MOVIE_CLIP_TIME_OFF*/){
#if (VR_CONTAINER_TYPE==COMMON_VR_VIDEO_TYPE_3GP) || (VR_CONTAINER_TYPE==COMMON_VR_VIDEO_TYPE_AVI)

                StateVideoRecMode_StopRecordingProc(EVENT_VRCB_FILE_FULL);                
#endif
            }
            else{
                if(VideoFunc_RecordRestart()==AHC_FALSE){
                    printc("--E-- VideoFunc_RecordRestart Fail\r\n");
                    StateVideoRecMode_StopRecordingProc(EVENT_VRCB_FILE_FULL);           
                    AHC_ShowErrorDialog(WMSG_STORAGE_FULL);

#ifdef CFG_REC_WD_RESET_AFTER_RESTART_FAIL //may be defined in config_xxx.h
                    while (1);  // Let OEM WD reset.
#endif
                }
                else{                    
                    DrawStateVideoRecUpdate(EVENT_VIDEO_KEY_RECORD);
                }
            }
            
            if(AHC_GetCurKeyEventID() == BUTTON_VRCB_FILE_FULL){
                AHC_SetCurKeyEventID(EVENT_NONE);
            }
            else{
                AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0);
                printc("KeyEventID: BUTTON_VRCB_FILE_FULL is interrupted.\r\n");
            }            
            break;
        case EVENT_VRCB_LONG_TIME_FILE_FULL:
            printc("long time file full \r\n");
            StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
            bVideoRecording = 0;

            DrawStateVideoRecUpdate(EVENT_VIDEO_KEY_RECORD_STOP);

            printc("long time file full start record\r\n");
            StateVideoRecMode_StartRecordingProc(EVENT_VIDEO_KEY_RECORD);

            bVideoRecording = 1;
            break;

        case EVENT_VRCB_MEDIA_SLOW:

            if(VideoFunc_RecordStatus())
            {
                UINT8 bSDClass = 0;

                m_uiSlowMediaCBCnt++;

                if(m_uiSlowMediaCBCnt == SLOW_MEDIA_CB_THD)
                {
                    bSDClass = AHC_SDMMC_GetClass(AHC_SD_0);

                    if(bSDClass==0xFF || bSDClass < SLOW_MEDIA_CLASS)
                    {
                        StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
                        DrawStateVideoRecUpdate(EVENT_VIDEO_KEY_RECORD_STOP);
                        AHC_ShowErrorDialog(WMSG_CARD_SLOW);
                    }
                    m_uiSlowMediaCBCnt = 0;
                }
            }
        break;

        case EVENT_VRCB_RECDSTOP_CARDSLOW:
            printc("card slow\r\n");
            StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
            bVideoRecording = 0;
            
            DrawStateVideoRecUpdate(EVENT_VIDEO_KEY_RECORD_STOP);
            #if 0
            AHC_ShowErrorDialog(WMSG_CARD_SLOW);
			
            if(AHC_TRUE == AHC_SDMMC_GetMountState())
            {
                AHC_DisMountStorageMedia(AHC_MEDIA_MMC);
                Enable_SD_Power(0 /* Power Off */);
                #if((EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_DUAL_FILE)||(EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE))
                AHC_SetKernalEmergencyStopStep(AHC_KERNAL_EMERGENCY_AHC_DONE);
                #endif

                AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect);
                AHC_UF_SetFreeChar(0, DCF_SET_FREECHAR, (UINT8 *) VIDEO_DEFAULT_FLIE_FREECHAR);
            }
            #else
            printc("card slow start record\r\n");
            StateVideoRecMode_StartRecordingProc(EVENT_VIDEO_KEY_RECORD);
            //VideoFunc_Record();
            bVideoRecording = 1;
            #endif

            if(AHC_GetCurKeyEventID() == BUTTON_VRCB_RECDSTOP_CARDSLOW){
                AHC_SetCurKeyEventID(EVENT_NONE);
            }
            else{
                AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0);
                printc("KeyEventID: BUTTON_VRCB_RECDSTOP_CARDSLOW is interrupted.\r\n");
            }                           
        break;

        case EVENT_VRCB_SEAM_LESS:
            //Prevent multiple event of Filefull and Seamless
        break;

        case EVENT_VRCB_MOTION_DTC:
#if (MOTION_DETECTION_EN) && (VMD_ACTION & VMD_RECORD_VIDEO)
            #if defined(WIFI_PORT) && (WIFI_PORT == 1)
            if (WLAN_SYS_GetMode() != -1) {
                if(AHC_Get_WiFi_Streaming_Status())
                    break;
            }
            #endif

            if(m_ubVMDStart)
            {
                if (AHC_SDMMC_BasicCheck() == AHC_FALSE)
                    break;

                #if (PARKING_RECORD_FORCE_20SEC == 0)
                // Update VMD stop recording timer
                m_ulVMDStopCnt = AHC_GetVMDRecTimeLimit() * 10; // Video Time period is 100ms.
                #endif

                #if((SUPPORT_GSENSOR && POWER_ON_BY_GSENSOR_EN) && \
                    (GSNR_PWRON_REC_BY && GSNR_PWRON_REC_BY_VMD))
                if(ubGsnrPwrOnActStart)
                {
                    m_ulGSNRRecStopCnt = AHC_GSNR_PWROn_MovieTimeReset();
                }
                #endif

                #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
                if( m_ubParkingModeRecTrigger == AHC_FALSE )
                #endif
                {
                    m_ulVMDStopCnt = AHC_GetVMDRecTimeLimit() * 1000 / VIDEO_TIMER_UNIT;

                    // VMD Trigger Video record and countdown (m_ulVMDStopCnt) to stop recording.
                    printc(FG_RED("!!! SomeBody/SomeThing is Moved!!! m_ulVMDStopCnt=%d %d\r\n"), m_ulVMDStopCnt, EVENT_VRCB_MOTION_DTC);

                    if (VideoFunc_RecordStatus() == AHC_FALSE)
                        AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, KEY_VIDEO_RECORD, EVENT_VRCB_MOTION_DTC);

                    #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
                    m_ubParkingModeRecTrigger = AHC_TRUE;
                    #endif
                }
            }
#endif
        break;

        case EVENT_VRCB_MEDIA_ERROR:
            RTNA_DBG_Str(0, FG_RED("EVENT_VRCB_MEDIA_ERROR\r\n"));
            if(VideoFunc_RecordStatus())
            {
                StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
                AHC_ShowErrorDialog(WMSG_INSERT_SD_AGAIN);
            }
        break;

        case EVENT_VR_START: //Not Used
            #if (GPS_CONNECT_ENABLE) && (GPS_FUNC & FUNC_VIDEOSTREM_INFO)
            GPSCtrl_StartRecord();
            #endif
            #if (GSENSOR_CONNECT_ENABLE) && (GSENSOR_FUNC & FUNC_VIDEOSTREM_INFO)
            AHC_Gsensor_StartRecord();
            #endif
        break;

        case EVENT_VR_STOP: //Not Used
            #if (GPS_CONNECT_ENABLE) && (GPS_FUNC & FUNC_VIDEOSTREM_INFO)
            GPSCtrl_EndRecord();
            #endif
            #if (GSENSOR_CONNECT_ENABLE) && (GSENSOR_FUNC & FUNC_VIDEOSTREM_INFO)
            AHC_Gsensor_EndRecord();
            #endif
        break;

        case EVENT_VR_WRITEINFO:
        //NOP
        break;

        case EVENT_LOCK_VR_FILE:
            if(VideoFunc_RecordStatus()) {
                AHC_Protect_SetType(AHC_PROTECT_MENU);
                VideoFunc_SetFileLock();
            }
        break;

        case EVENT_LOCK_FILE_G:
            printc(FG_BLUE("@@@ EVENT_LOCK_FILE_G\r\n"));

#if (SUPPORT_GSENSOR) && (GSENSOR_FUNC & FUNC_LOCK_FILE)
            #if((SUPPORT_GSENSOR && POWER_ON_BY_GSENSOR_EN) && \
                (GSNR_PWRON_REC_BY & GSNR_PWRON_REC_BY_SHAKED) && \
                (GSNR_PWRON_MOVIE_SHAKED_ACT == GSNR_PWRON_MOVIE_LOCKED))
            if (ubGsnrPwrOnActStart && ubGsnrPwrOnFirstREC)
            {
                if ( VideoFunc_RecordStatus() && !VideoFunc_LockFileEnabled() && (m_ubGsnrIsObjMove == AHC_TRUE) )
                {
                    RTNA_DBG_Str(0, FG_BLUE("Lock file by G-Sensor Power-On !!!\r\n"));

                    AHC_Protect_SetType(AHC_PROTECT_G);

                #if (LIMIT_MAX_LOCK_FILE_NUM)
                    #if (MAX_LOCK_FILE_ACT == LOCK_FILE_STOP)
                    if (m_ulLockFileNum < MAX_LOCK_FILE_NUM)
                    #endif
                    {
                        AHC_WMSG_Draw(AHC_TRUE, WMSG_LOCK_CURRENT_FILE, 2);
                        EnableLockFile(AHC_TRUE, LOCK_FILE_CUR);
                        m_ulLockEventNum++;
                    }
                #else
                    {
                        AHC_WMSG_Draw(AHC_TRUE, WMSG_LOCK_CURRENT_FILE, 2);
                        EnableLockFile(AHC_TRUE, LOCK_FILE_CUR);
                    }
                #endif

                    m_ubGsnrIsObjMove = AHC_FALSE;  //Reset
                    ubGsnrPwrOnFirstREC = AHC_FALSE;
                }
            }
            else
            #endif//POWER_ON_BY_GSENSOR_EN
            if ( (MenuSettingConfig()->uiGsensorSensitivity != GSENSOR_SENSITIVITY_OFF) &&
                 (VideoFunc_RecordStatus() && !VideoFunc_LockFileEnabled()) )
            {
                if (m_ubGsnrIsObjMove == AHC_TRUE)
                {
                    printc("CarDV is Shaking !! Lock Previous/Current Files\r\n");

                    AHC_Protect_SetType(AHC_PROTECT_G);

                #if (LIMIT_MAX_LOCK_FILE_NUM)
                    #if (MAX_LOCK_FILE_ACT == LOCK_FILE_STOP)
                    if (m_ulLockFileNum < MAX_LOCK_FILE_NUM)
                    #endif
                    {
                        AHC_WMSG_Draw(AHC_TRUE, WMSG_LOCK_CURRENT_FILE, 2);
                        EnableLockFile(AHC_TRUE, VR_LOCK_FILE_TYPE);
                        m_ulLockEventNum++;
                    }
                #else
                    {
                        AHC_WMSG_Draw(AHC_TRUE, WMSG_LOCK_CURRENT_FILE, 2);
                        EnableLockFile(AHC_TRUE, VR_LOCK_FILE_TYPE);
                    }
                #endif

                    m_ubGsnrIsObjMove = AHC_FALSE;//Reset
                }
            }
            else
            {
                m_ubGsnrIsObjMove = AHC_FALSE;//Reset
            }
#endif
        break;

        case EVENT_VR_EMERGENT:

            if( MenuSettingConfig()->uiGsensorSensitivity != GSENSOR_SENSITIVITY_OFF && VideoFunc_RecordStatus() )
            {
                #if (GSENSOR_CONNECT_ENABLE)
                m_ubGsnrIsObjMove = AHC_FALSE;//Reset
                #endif

                printc("CarDV is Shaking !!!!!\r\n");

                //AHC_PlaySoundEffect(AHC_SOUNDEFFECT_VR_EMER);

                if( (AHC_IsEmerRecordStarted() == AHC_FALSE) &&
                    (AHC_IsEmerPostDone() == AHC_TRUE))
                {
                    #if (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_MOVE_FILE)
                    if(AHC_Deletion_RemoveEx(DCF_DB_TYPE_3RD_DB, AHC_GetVRTimeLimit()) == AHC_TRUE)
                    #else
                    if(AHC_Deletion_RemoveEx(DCF_DB_TYPE_3RD_DB, EMER_RECORD_DUAL_WRTIE_MAX_TIME) == AHC_TRUE)
                    #endif
                    {
                        printc("Emergency Record: Go !!!! \n");

                        #if (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_MOVE_FILE)
                        if(1)
                        #else
                        if(VideoFunc_TriggerEmerRecord())
                        #endif
                        {
                            UINT32 ulTickCount = OSTimeGet();
                            //DrawVideo_UpdatebyEvent(EVENT_VR_EMERGENT);
                            AHC_SetEmerRecordStarted(AHC_TRUE);
                            AHC_WMSG_Draw(AHC_TRUE, WMSG_LOCK_CURRENT_FILE, 2);
                            printc("AHC_WMSG_Draw() Time = %d ms \n\r", ((OSTimeGet() - ulTickCount)*1000)/ OS_TICKS_PER_SEC);
                        }
                    }
                }
                else
                {
                    printc("Emergency Record: Keep going !!!! \n");

                    if(VideoFunc_TriggerEmerRecord())
                    {
                        //DrawVideo_UpdatebyEvent(EVENT_VR_EMERGENT);
                    }
                }
            }
        break;

        case EVENT_VRCB_EMER_DONE:
            #if((EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_DUAL_FILE)||(EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE))
            if(AHC_GetKernalEmergencyStopStep() == AHC_KERNAL_EMERGENCY_STOP)
                AHC_SetKernalEmergencyStopStep(AHC_KERNAL_EMERGENCY_AHC_DONE);
            else
                break;
            #endif

            SystemSettingConfig()->byStartNormalRecordAfterEmergencyRecord = (UINT8)VideoFunc_RecordStatus();
            AHC_EmerRecordPostProcess();
            //DrawVideo_UpdatebyEvent(EVENT_VRCB_EMER_DONE);

            AHC_SetEmerRecordStarted(AHC_FALSE);

        break;


        //Update Message
        case EVENT_VIDREC_UPDATE_MESSAGE:
            MMPS_FS_SetCreationTime();
#if (VIDEO_DIGIT_ZOOM_EN ==1)
            if(!AHC_GetVideoCurZoomStatus()){
                if(!bZoomStop){
                    VideoFunc_ZoomOperation(AHC_SENSOR_ZOOM_STOP);
                    bZoomStop = AHC_TRUE;	               
                }
            }
#endif
#if defined(WIFI_PORT) && (WIFI_PORT == 1)
            // Reset auto power-off counter
            switch (get_NetAppStatus()) {
            case NETAPP_STREAM_TEARDOWN:
            case NETAPP_WIFI_READY:
                // remote connected
            case NETAPP_STREAM_PLAY:
                AutoPowerOffCounterReset();
                break;
            }
#endif

#if (MOTION_DETECTION_EN)
            if(m_ubInRemindTime && m_ulVMDRemindTime==0)
            {
                DrawStateVideoRecUpdate(ulJobEvent);
                m_ubInRemindTime = AHC_FALSE;
                break;
            }
#endif

#if (FLICKER_PWR_LED_AT_VR_STATE)
            if(VideoFunc_RecordStatus())
            {
                static AHC_BOOL ubLEDstatus = 0;
                LedCtrl_PowerLed(ubLEDstatus++ & 0x2);
            }
#endif

            {
                static UINT32 m_VidWarningCounter = 0;

                if(AHC_IsDialogPresent())
                    m_VidWarningCounter++;
                else
                    m_VidWarningCounter = 0;

                if(m_VidWarningCounter!=0 && m_VidWarningCounter%10==0)
                    AHC_ShowSoundWarning();
            }

            DrawStateVideoRecUpdate(ulJobEvent);
        break;

        case EVENT_SHOW_FW_VERSION:
            AHC_WMSG_Draw(AHC_TRUE, WMSG_SHOW_FW_VERSION, 5);
        break;

        case EVENT_SHOW_GPS_INFO:
            #if (SUPPORT_GPS)
            if(bGPS_PageExist)
            {
                bGPS_PageExist = AHC_FALSE;
                DrawStateVideoRecUpdate(EVENT_LCD_COVER_NORMAL);
            }
            else
            {
                UINT8 bID0 = 0, bID1 = 0;

                CHARGE_ICON_ENABLE(AHC_FALSE);
                bGPS_PageExist = AHC_TRUE;
                OSDDraw_EnterDrawing(&bID0, &bID1);
                QuickDrawGPSInfoPage(bID0, AHC_FALSE);
                QuickDrawGPSInfoPage(bID0, AHC_TRUE);
                OSDDraw_ExitDrawing(&bID0, &bID1);
                CHARGE_ICON_ENABLE(AHC_TRUE);
            }
            #endif
        break;

        case EVENT_GPSGSENSOR_UPDATE:
            #if (GPS_CONNECT_ENABLE) && (GPS_FUNC & FUNC_VIDEOSTREM_INFO)
            if( 0 == (ulCounterForGpsGsnrUpdate % ((1000/VIDEO_TIMER_UNIT)/(gGPS_Attribute.ubStoreFreq))))
                AHC_GPS_SetCurInfo();
            #endif
            #if (GSENSOR_CONNECT_ENABLE) && (GSENSOR_FUNC & FUNC_VIDEOSTREM_INFO)
            if( 0 == (ulCounterForGpsGsnrUpdate % ((1000/VIDEO_TIMER_UNIT)/(AHC_Gsensor_GetAttributeAddr()->ubStoreFreq))))
                AHC_Gsensor_SetCurInfo();
            #endif
            #if (GPS_CONNECT_ENABLE || GSENSOR_CONNECT_ENABLE)
            ulCounterForGpsGsnrUpdate++;
            #endif
        break;

        case EVENT_VR_OSD_SHOW:
        #if (AUTO_HIDE_OSD_EN)
            m_ulOSDCounter  = 0;
            m_ubHideOSD     = AHC_FALSE;
            DrawStateVideoRecInit();
        #endif
        break;

#if defined(WIFI_PORT) && (WIFI_PORT == 1)
        case EVENT_OPEN_H264_STREAM:
            #if (VR_PREENCODE_EN)
            m_ubPreEncodeEn = AHC_TRUE;
            #endif
            AHC_SetMode(AHC_MODE_IDLE);
            VideoFunc_Preview();
            break;
        case EVENT_CLOSE_H264_STREAM:
            #if (VR_PREENCODE_EN)
            m_ubPreEncodeEn = AHC_FALSE;
            #endif
            AHC_SetMode(AHC_MODE_IDLE);
            VideoFunc_Preview();
            break;

        case EVENT_SWITCH_WIFI_STREAMING_MODE:
            if (NETAPP_WIFI_NONE == get_NetAppStatus()) {
                break;
            }

            if(aitstreamer_is_ready()==AHC_FALSE) {
                printc("--E-- A-I-T streamer is not ready !!!\n");
                break;
            }

            if (get_NetAppStatus()) {
                AHC_WiFi_Toggle_StreamingMode();
            }
        break;

        case EVENT_WIFI_SWITCH_TOGGLE:
            printc("EVENT_WIFI_SWITCH_TOGGLE -\r\n");

            if ((NETAPP_WIFI_DOWN == get_NetAppStatus()) || (NETAPP_WIFI_NONE == get_NetAppStatus()) || (WLAN_SYS_GetMode() == -1))
                AHC_WiFi_Switch(AHC_TRUE);
            else
                AHC_WiFi_Switch(AHC_FALSE);
        break;

        case EVENT_WIFI_SWITCH_DISABLE:
            printc("EVENT_WIFI_SWITCH_DISABLE -\r\n");

            if (AHC_TRUE == AHC_WiFi_Switch(AHC_FALSE)) {
                Setpf_WiFi(WIFI_MODE_OFF);
                // Need save menusetting to Flash - TBD
            }
        break;

        case EVENT_WIFI_SWITCH_ENABLE:
            printc("EVENT_WIFI_SWITCH_ENABLE -\r\n");

            if (AHC_TRUE == AHC_WiFi_Switch(AHC_TRUE)) {
                Setpf_WiFi(WIFI_MODE_ON);
                // Need save menusetting to Flash - TBD
            }
        break;
#endif      // #if defined(WIFI_PORT) && (WIFI_PORT == 1)

        case EVENT_PARKING_KEY:
            #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
            if( VideoFunc_RecordStatus() == AHC_TRUE )
            {
                break;
            }

            if( uiGetParkingModeEnable() == AHC_FALSE )
            {
                uiSetParkingModeEnable( AHC_TRUE );

                #if (MOTION_DETECTION_EN)
                if( MenuSettingConfig()->uiMotionDtcSensitivity == MOTION_DTC_SENSITIVITY_OFF )
                {
                    MenuSettingConfig()->uiMotionDtcSensitivity = MOTION_DTC_SENSITIVITY_LOW;
                }
                Menu_SetMotionDtcSensitivity( MenuSettingConfig()->uiMotionDtcSensitivity );

                m_ulVMDStableCnt = MOTION_DETECTION_STABLE_TIME * 1000 / VIDEO_TIMER_UNIT;
                m_ubVMDStart     = AHC_FALSE;
                #endif

                AIHC_SetVideoRecordMode( AHC_TRUE, AHC_VIDRECD_INIT ); // Use AHC_VIDRECD_RESTART_PREVIEW if Parking setting is the same as Normal mode
            }
            else
            {
                AIHC_StopVideoRecordMode( AHC_TRUE, AHC_FALSE ); // Stop video Pre-record before uiSetParkingModeEnable( AHC_FALSE ) to avoid un-expected AHC_UF_PostAddFile()

                uiSetParkingModeEnable( AHC_FALSE );

                #if (MOTION_DETECTION_EN)
 				if( MenuSettingConfig()->uiMotionDtcSensitivity != MOTION_DTC_SENSITIVITY_OFF )
                {
                    MenuSettingConfig()->uiMotionDtcSensitivity = MOTION_DTC_SENSITIVITY_OFF;
                }
                Menu_SetMotionDtcSensitivity( MenuSettingConfig()->uiMotionDtcSensitivity );				
                m_ubVMDStart     = AHC_FALSE;
                m_ulVMDStopCnt   = 0;
                m_ulVMDStableCnt = 0;
                #endif

                AIHC_SetVideoRecordMode( AHC_FALSE, AHC_VIDRECD_INIT );
            }

            DrawVideoParkingMode( uiGetParkingModeEnable() );
            #endif

        break;

#if (UVC_HOST_VIDEO_ENABLE)
        /////////////////////////////////////////////////////// Description ////////////////////////////////////////////////////
        // EVENT_USB_B_DEVICE_DETECT: Got when Rear Cam streaming was detected
        // EVENT_USB_B_DEVICE_REMOVED: Got when Rear Cam streaming was disconnected
        // Note: When some UVC error happened (ex: AHC_HostUVCVideoSetEp0TimeoutCB() / AHC_HostUVCVideoSetFrmRxTimeout2CB() was called),
        //       EVENT_USB_B_DEVICE_REMOVED will be sent. Below sample code will stop current recording and start next recording with only Front Cam.
        //       Once AHC_USBDetectHandler() detected Rear Cam streaming was recovered, EVENT_USB_B_DEVICE_DETECT will be sent.
        //       Below sample code will stop current recording and start next recording with Front & Rear Cam.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        case EVENT_USB_B_DEVICE_DETECT:
        case EVENT_USB_B_DEVICE_REMOVED:
            {
                MMP_BOOL bStatus = MMP_FALSE;
                AHC_BOOL brecording=AHC_FALSE;

                //Receive USB B device event during booting time.
                if (ulJobEvent == EVENT_USB_B_DEVICE_DETECT) {
                    //Check if UVC preview is enabled
                    MMPF_USBH_GetUVCPrevwSts(&bStatus);
#if defined(WIFI_PORT) && (WIFI_PORT == 1)
                    netapp_setRearcamReady(AHC_TRUE);
#endif
                    if (bStatus){                        
                        AHC_USB_PauseDetection(0);
                        if((AHC_GetCurKeyEventID() == BUTTON_USB_B_DEVICE_DETECTED) ||
                            (AHC_GetCurKeyEventID() == BUTTON_USB_B_DEVICE_REMOVED)){
                            AHC_SetCurKeyEventID(EVENT_NONE);
                        }
                        else{
                            AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0);
                            printc("KeyEventID: BUTTON_USB_B_DEVICE_DETECTED,REMOVED is interrupted.\r\n");
                        }                           
                        break;
                    }
                }

                brecording = VideoFunc_RecordStatus();
                if(brecording){
                    StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD); //stop video record.
                }

                if(ulJobEvent == EVENT_USB_B_DEVICE_REMOVED){
                    AIHC_UVCPreviewStop();
                    /* BE CAREFUL, forced stop frame Rx and clear FIFO */
                    MMPS_USB_StopFrmRxClrFIFO();
                    /* BE CAREFUL, SPECIAL CASE! */
                    /* forced to clear connect status because of disconnected, not from USB_OTG_ISR (PHY) */
                    MMPS_USB_SetDevConnSts(MMP_FALSE);
                    #if defined(WIFI_PORT) && (WIFI_PORT == 1)
                    netapp_setRearcamReady(AHC_FALSE);
                    #endif
                }
#if 0
                AHC_SetMode(AHC_MODE_IDLE);
#endif

                if(ulJobEvent == EVENT_USB_B_DEVICE_REMOVED){
                    /* reset USB */
                    MMPS_USB_DisconnectDevice();
                }

                if(ulEvent == EVENT_USB_B_DEVICE_DETECT){
                    MMP_ERR sRet = MMP_ERR_NONE;
                    MMP_USHORT                  usVideoPreviewMode;
                    AHC_DISPLAY_OUTPUTPANEL OutputDevice;
                    UINT32 ulFlickerMode = COMMON_FLICKER_50HZ;
                    
                    AHC_Menu_SettingGetCB( (char *)COMMON_KEY_FLICKER, &ulFlickerMode);

                    AHC_GetDisplayOutputDev(&OutputDevice);
                    sRet = MMPS_Display_SetOutputPanel(MMP_DISPLAY_PRM_CTL, OutputDevice);
                    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); } 

                    switch(OutputDevice){
                        case MMP_DISPLAY_SEL_MAIN_LCD:
                        case MMP_DISPLAY_SEL_SUB_LCD:
                            usVideoPreviewMode = VIDRECD_NORMAL_PREVIEW_MODE;
                            break;
                        case MMP_DISPLAY_SEL_NTSC_TV:
                            usVideoPreviewMode = VIDRECD_NTSC_PREVIEW_MODE;
                            break;
                        case MMP_DISPLAY_SEL_PAL_TV:
                            usVideoPreviewMode = VIDRECD_PAL_PREVIEW_MODE;
                            break;
                        case MMP_DISPLAY_SEL_HDMI:
                            usVideoPreviewMode = VIDRECD_HDMI_PREVIEW_MODE;
                            break;
                        case MMP_DISPLAY_SEL_CCIR:	
                            usVideoPreviewMode = VIDRECD_CCIR_PREVIEW_MODE;
                            break;                    
                        case MMP_DISPLAY_SEL_NONE:
                        default:
                            usVideoPreviewMode = VIDRECD_NORMAL_PREVIEW_MODE;
                            printc("%s,%d  not support yet!"__func__, __LINE__);
                            break;
                    }            
                    
                    AIHC_UVCPreviewStart(usVideoPreviewMode, OutputDevice, ulFlickerMode);
                }
#if 0
                VideoFunc_Preview();
#endif

                if(brecording){
                    StateVideoRecMode_StartRecordingProc(EVENT_VIDEO_KEY_RECORD); //restart video record.
                }
            }

            AHC_USB_PauseDetection(0);
            if((AHC_GetCurKeyEventID() == BUTTON_USB_B_DEVICE_DETECTED) ||
                (AHC_GetCurKeyEventID() == BUTTON_USB_B_DEVICE_REMOVED)){
                AHC_SetCurKeyEventID(EVENT_NONE);
            }
            else{
                AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0);
                printc("KeyEventID: BUTTON_USB_B_DEVICE_DETECTED,REMOVED is interrupted.\r\n");
            }  
            break;
#endif

#if (SUPPORT_DUAL_SNR == 1 && DUALENC_SUPPORT == 1) //For Bitek1603
        case EVENT_TV_DECODER_SRC_TYPE:
            {
                MMP_BOOL bStatus = MMP_FALSE;
                AHC_BOOL brecording=AHC_FALSE;

                MMPF_VIF_TV_DEC_SRC_TYPE sSnrTVSrc;
                MMPF_Sensor_GetTV_Dec_Src_Type(&sSnrTVSrc);
               
                if((sSnrTVSrc == MMPF_VIF_SRC_PAL) || (sSnrTVSrc == MMPF_VIF_SRC_NTSC)){
#if defined(WIFI_PORT) && (WIFI_PORT == 1)
                        netapp_setRearcamReady(AHC_TRUE);
#endif               
                }
                else{ //sSnrTVSrc = MMPF_VIF_SRC_NO_READY
#if defined(WIFI_PORT) && (WIFI_PORT == 1)
                    netapp_setRearcamReady(AHC_FALSE);
#endif                
                }

                brecording = VideoFunc_RecordStatus();
                if(brecording){
                    StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD); //stop video record.
                    StateVideoRecMode_StartRecordingProc(EVENT_VIDEO_KEY_RECORD); //restart video record.
                }                

                if(AHC_GetCurKeyEventID() == BUTTON_TV_DECODER_SRC_TYPE){
                    AHC_SetCurKeyEventID(EVENT_NONE);
                }
                else{
                    AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0);
                    printc("KeyEventID: BUTTON_TV_DECODER_SRC_TYPE is interrupted.\r\n");
                }  
            }
            break;
#endif

		case EVENT_LDWS_START:
		#if (ENABLE_ADAS_LDWS)
//			if ((CheckAlertState() != ALERT_NON) && (CheckAlertState() != ALERT_LDWS))
//				return;

			LDWS_Lock();

			MMPS_AUI_StopWAVPlay();
            m_ulLDWSWarn = 20;
			VideoFunc_LDWSWarn();
		#endif
		break;

		case EVENT_LDWS_STOP:
		#if (ENABLE_ADAS_LDWS)
			LDWS_Unlock();
			ResetLDWSCounter();

//			if ((CheckAlertState() != ALERT_NON) && (CheckAlertState() != ALERT_LDWS))
//				return;

			MMPS_AUI_StopWAVPlay();
			DrawStateVideoRecUpdate(EVENT_LDWS_STOP);
		#endif
		break;

		case EVENT_FCWS_START:
		#if (ENABLE_ADAS_FCWS)
//			if ((CheckAlertState() != ALERT_NON) && (CheckAlertState() != ALERT_FCWS))
//				return;

			MMPS_AUI_StopWAVPlay();

			m_ulFCWSWarn = 20;
			VideoFunc_FCWSWarn();
		#endif
		break;

		case EVENT_FCWS_STOP:
		#if (ENABLE_ADAS_FCWS)
			ResetFCWSCounter();

//			if ((CheckAlertState() != ALERT_NON) && (CheckAlertState() != ALERT_FCWS))
//				return;

			MMPS_AUI_StopWAVPlay();
			DrawStateVideoRecUpdate(EVENT_FCWS_STOP);
		#endif
		break;

        default:
        break;
    }
}

/*
 * lock functions
 */
/*
 * Get lock file status;
 */
AHC_BOOL VideoFunc_LockFileEnabled(void)
{
    return _bLockVRFile;
}

void EnableLockFile(AHC_BOOL bEnable, int type)
{
    _bLockVRFile = bEnable;
    _nLockType   = type;
}

/*
 * Change Lock file type, when stop recording by manual
 */
void LockFileTypeChange(int arg)
{
    if (arg == 1 /* Remove type of lock NEXT file */) {
        // When Stop recording by manual, remove lock NEXT
        if (_nLockType == LOCK_FILE_CUR_NEXT) {
            _nLockType = LOCK_FILE_CUR;
        } else if (_nLockType == LOCK_FILE_PREV_CUR_NEXT) {
            _nLockType = LOCK_FILE_PREV_CUR;
        }
    }
}

/*
 * a Hook function called by MMPF after VID file closed.
 */
void CUS_VideoProtectProcess(void)
{
    if (VideoFunc_LockFileEnabled()) {
        switch (_nLockType) {
        case LOCK_FILE_PREV_CUR:
        case LOCK_FILE_PREV_CUR_NEXT:
            printc("LOCK PREVIOUS and CURRENT...\r\n");
            AHC_Protect_SetVRFile(AHC_PROTECTION_BOTH_FILE, AHC_Protect_GetType());

            if (_nLockType == LOCK_FILE_PREV_CUR_NEXT) {
                _nLockType = LOCK_FILE_CUR;
            } else {
                printc("LOCK FINISH...\r\n");
                EnableLockFile(AHC_FALSE, 0);
                AHC_Protect_SetType(AHC_PROTECT_NONE);
            }
            break;

        case LOCK_FILE_CUR:
        case LOCK_FILE_CUR_NEXT:
            printc("LOCK CURRENT...\r\n");
            AHC_Protect_SetVRFile(AHC_PROTECTION_CUR_FILE, AHC_Protect_GetType());

            if (_nLockType == LOCK_FILE_CUR_NEXT) {
                _nLockType = LOCK_FILE_CUR;
            } else {
                printc("LOCK FINISH...\r\n");
                EnableLockFile(AHC_FALSE, 0);
                AHC_Protect_SetType(AHC_PROTECT_NONE);
            }
        }
    }
}

AHC_BOOL VideoRec_TriggeredByVMD(void) {
#if (MOTION_DETECTION_EN && (VMD_ACTION & VMD_RECORD_VIDEO))
    return (0 == m_ulVMDStopCnt) ? AHC_FALSE : AHC_TRUE;
#else
    return AHC_FALSE;
#endif
}

// for ADAS_LDWS, TBD
AHC_BOOL VideoFunc_Init(void* pData)
{
    #if (1)
    printc("%s \n", __func__);
    #endif
#if (ENABLE_ADAS_LDWS)


    // Put here temporary. Matt.
    //LDWS_RegistCallback(LDWS_Callback);
    ADAS_CTL_RegistCallback(LDWS_Callback);
#endif

    return AHC_TRUE;
}

AHC_BOOL VideoFunc_ShutDown(void* pData)
{
    #if (defined(STATE_VIDEO_DEBUG) && STATE_VIDEO_DEBUG == 1)
    printc("%s \n", __func__);
    #endif
	
    VideoTimer_Stop();

    if (AHC_WMSG_States()) {
        AHC_WMSG_Draw(AHC_FALSE, WMSG_NONE, 0);
    }

#if (MOTION_DETECTION_EN)
    #if (defined(PROJECT_ID) && (PROJECT_ID == PROJECT_ID_SP86G || PROJECT_ID == PROJECT_ID_SP20G))
    if (m_ubMotionDtcEn)	
	{
		VideoFunc_ExitVMDMode();
	}
    #endif
#endif

    return AHC_TRUE;
}

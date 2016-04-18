/**
 @file AHC_General.c
 @brief AHC general control Function
 @author
 @version 1.0
*/

#if 0
void __HEADER__(){}
#endif

/*===========================================================================
 * Include files
 *===========================================================================*/

#include "Customer_config.h"

#include "mmp_lib.h"
#include "mmp_err.h"
#include "ait_utility.h"
#include "mmps_iva.h"
#include "mmps_i2cm.h"
#include "mmps_uart.h"
#include "mmps_rtc.h"
#include "mmps_pio.h"
#include "mmps_timer.h"
#include "mmps_sensor.h"
#include "mmps_usb.h"
#include "mmps_dsc.h"
#include "mmps_3gprecd.h"
#include "mmps_audio.h"
#include "mmps_aui.h"
#include "mmps_pwm.h"
#include "mmps_Sensor.h"
#include "mmps_3gprecd.h"
#include "wm8971.h"
#include "mmpd_system.h"
#include "mmpd_mp4venc.h"
#include "mmps_vidplay.h"
#include "mmpf_ringbuf.h"
#include "mmpf_ldc.h"
#include "mmpf_wd.h"
#include "mmpf_pll.h"
#include "lib_retina.h"
#include "mmpf_display.h"
#include "AHC_OS.h"
#include "AHC_FS.h"
#include "AHC_General.h"
#include "AHC_Parameter.h"
#include "AHC_Message.h"
#include "AHC_Menu.h"
#include "AHC_Motor.h"
#include "AHC_Parameter.h"
#include "AHC_GUI.h"
#include "AHC_Audio.h"
#include "AHC_Video.h"

#include "AHC_ISP.h"
#include "AHC_PMU.h"
#include "AHC_Sensor.h"
#include "AHC_Config_SDK.h"
#include "AHC_DateTime.h"
#include "AHC_Version.h"
#include "AHC_Stream.h"
#include "AHC_Media.h"
#include "AHC_Warningmsg.h"
#include "AHC_Utility.h"
#include "AHC_UF.h"
#include "AIHC_DCF.h"
#include "AIHC_Browser.h"
#include "AIHC_Video.h"
#include "AIHC_GUI.h"
#include "AIHC_Audio.h"
#include "AIHC_Display.h"
#include "AIHC_USB.h"
#include "AHC_USB.h"
#include "AHC_Callback.h"
#include "AHC_USBHost.h"

#include "mmpf_pwm.h"
#include "mmpf_hw_counter.h"
#include "mmpf_pio.h"
#include "mmpf_sensor.h"
#include "mmpd_ptz.h"
#include "MenuSetting.h"
#include "keyparser.h"
#include "dsc_key.h"
#include "StateHDMIFunc.h"
#include "StateTVFunc.h"
#include "StateVideoFunc.h"
#include "LedControl.h"
#include "mmpf_storage_api.h"
#include "ZoomControl.h"
#include "isp_if.h"
#include "hdr_cfg.h"
#include "mdtc_cfg.h"
#include "lcd_common.h"
#include "PCAM_API.h"

#if defined(WIFI_PORT) && (WIFI_PORT == 1)
#include "wlan.h"
#include "netapp.h"
#include "mmpf_streaming.h"
#endif
#if (GPS_CONNECT_ENABLE)
#include "GPS_ctl.h"
#endif

#if (GSENSOR_CONNECT_ENABLE)
#include "GSensor_ctrl.h"
#endif

#if (SUPPORT_IR_CONVERTER)
#include "ir_ctrl.h"
#endif

#include "MenuSetting.h"
#include "MenuDrawCommon.h"
#include "GPS_Ctl.h"
#include "mdtc_cfg.h"

#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
#include "AHC_DCFDT.h"
#endif

#if (SD_UPDATE_FW_EN)
#include "SD_Updater.h"
#endif
#if (EDOG_ENABLE)
#include "EDOG_ctl.h"
#endif

#include "SoundEffectName.h"
#include "fs_cfg.h" //for SD pad setting.

/*===========================================================================
 * Macro define
 *===========================================================================*/

#define AHC_GENERAL_DEBUG (0)

#if (ENABLE_VIDEO_ERR_LOG)
#define LOG_FILE_EXTENSION              "LOG"
#endif

/*===========================================================================
 * Structure define
 *===========================================================================*/
// this function type should aligned with MMPF_VIDMGR_WriteFile()
typedef MMP_ERR (VR_WRITEFUNC)(void *buf, MMP_ULONG size, MMP_ULONG *wr_size, MMP_ULONG ulFileID);

#if (ENABLE_VIDEO_ERR_LOG)
typedef struct _VIDEO_ERR_LOG {
    UINT32 callBackError;               ///< MMPS_3GPRECD_EVENT
    UINT32 errorParameter1;
    UINT32 errorParameter2;
    /**
    MMPS_3GPRECD_RecordStatus()
    */
    UINT32 fwOperationBeforeStopRecord; ///< MMPS_FW_OPERATION
    UINT32 fwOperationAfterStopRecord;  ///< MMPS_FW_OPERATION
    /**
    MMPS_3GPRECD_MergerStatus()

    */
    UINT32 retStatusBeforeStopRecord;   ///< MMP_ERR
    UINT32 retStatusAfterStopRecord;    ///< MMP_ERR
    UINT32 txStatusBeforeStopRecord;
    UINT32 txStatusAfterStopRecord;

} VIDEO_ERR_LOG;
#endif ///< ENABLE_VIDEO_ERR_LOG

/*===========================================================================
 * Project definition check
 *===========================================================================*/
#if (AHC_DRAM_SIZE == COMMON_DRAM_SIZE_NG)
#error "Definition AHC_DRAM_SIZE NG!!!"
#endif

#if (VR_VIDEO_TYPE == COMMON_VR_VIDEO_TYPE_NG)
#error "Definition VR_VIDEO_TYPE NG!!!"
#endif

#if (VR_VIDEO_TYPE_SETTING == COMMON_VR_VIDEO_TYPE_SETTING_PROFILE_NG)
#error "Definition VR_VIDEO_TYPE_SETTING NG!!!"
#endif

#if (VR_VIDEO_CURBUF_ENCODE_MODE == COMMON_VR_VIDEO_CURBUF_NG)
#error "Definition VR_VIDEO_CURBUF_ENCODE_MODE NG!!!"
#endif

#if 0
void _____VARIABLES_________(){ruturn;} //dummy
#endif

/** @addtogroup AHC_GENERAL
@{
*/
/*===========================================================================
 * Global varible
 *===========================================================================*/

#if defined(SA_PL)
AHC_OS_MQID                         AHC_MSG_QId;
void                                *AHC_MsgQueue[AHC_MSG_QUEUE_SIZE] = {0};

AHC_OS_MQID                         AHC_HP_MSG_QId;
void                                *AHC_HPMsgQueue[AHC_HP_MSG_QUEUE_SIZE] = {0};
#endif

AHC_MEMORY_LOCATION                 glAhcMemoryLocation[AHC_MEMORY_LOCATION_MAX];
AHC_SOUND_FILE_ATTRIBUTE            SoundEffectFile[8];

static AHC_QUEUE_MESSAGE            m_MessageQueue[AHC_MSG_QUEUE_SIZE] = {0};
static MMP_ULONG                    m_MessageQueueIndex_W;
static MMP_ULONG                    m_MessageQueueIndex_R;

static AHC_QUEUE_MESSAGE            m_HPMessageQueue[AHC_HP_MSG_QUEUE_SIZE] = {0};
static MMP_ULONG                    m_HPMessageQueueIndex_W;
static MMP_ULONG                    m_HPMessageQueueIndex_R;

static AHC_MODE_ID                  m_AhcSystemMode         = AHC_MODE_IDLE;
static MMP_I2CM_ATTR                m_AhcI2cmAttribute[AHC_I2C_HANDLE_MAX];
static MMP_BOOL	                    m_AhcUartFree[MMP_UART_MAX_COUNT - 1] = {MMP_FALSE, MMP_FALSE};  //Decrease 1 for the uart reserved for debug.
static MMP_UBYTE                    m_ulI2cHandle_count     = 0;
MMP_USHORT                          gsAhcCurrentSensor      = PRM_SENSOR;
MMP_USHORT                          gsAhcSecondSensor       = SCD_SENSOR;
MMP_DISPLAY_OUTPUT_SEL			    gsAhcCurrentDisplay     = MMP_DISPLAY_SEL_MAIN_LCD;
static UINT32                       m_ulPlaybackFileType    = DCF_OBG_JPG;
MMP_UBYTE							m_ubFlipType			= 0;


static UINT16                       m_usVideoRDirKey;
static UINT8                        m_VideoRFileName[64];
#if ((UVC_HOST_VIDEO_ENABLE == 1  || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)
static UINT8                        m_RearVideoRFileName[64];
#if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
static UINT8                        m_EmrRearVideoRFileName[64];
#endif
#endif
#if(SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
static UINT16                       m_usVideoParkDirKey;
static UINT8                        m_VideoParkFileName[64];
#endif

#if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
static AHC_BOOL                     m_bStartEmerVR = AHC_FALSE;
#if((EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_DUAL_FILE)||(EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE))
static AHC_KERNAL_EMERGENCY_STOP_STEP   m_bKernalEmergencyStopStep = AHC_KERNAL_EMERGENCY_AHC_DONE;
#endif
static AHC_BOOL                     m_bEmerVRPostDone = AHC_TRUE;
static UINT32                       m_uiEmerRecordInterval;
static UINT16                       m_uwEmerVRDirKey;
static UINT8                        m_chEmerVRFileName[64];
static MMP_BYTE                     m_chEmerVRFullName[MAX_FILE_NAME_SIZE];
#endif
#if EMERGENTRECD_SUPPORT && (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_MOVE_FILE)
static AHC_BOOL                     m_bNormal2Emergency = AHC_FALSE;
#endif

static MMP_BYTE                     m_CurPlayFileName[MAX_FILE_NAME_SIZE];
#if (VR_SEAMLESS)
static MMP_BYTE                     m_CurVRFullName[MAX_FILE_NAME_SIZE];
#if ((UVC_HOST_VIDEO_ENABLE == 1  || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)
static MMP_BYTE                     m_CurVRRearFullName[MAX_FILE_NAME_SIZE];
#if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
static MMP_BYTE                     m_EmrVRRearFullName[MAX_FILE_NAME_SIZE];
#endif
#endif
#endif
static UINT32                       m_ulCurrentPBFileType;
UINT16                       		m_ulCurrentPBWidth;
UINT16                       		m_ulCurrentPBHeight;
static UINT8                        m_bAHCGeneralInit       = 0;
static UINT32                       m_ulVideoPlayStopStatus = AHC_VIDEO_PLAY_EOF;

static MMP_ULONG                    m_ulVideoPlayStartTime  = 0;
static MMP_ULONG                    m_ulAudioPlayStartTime  = 0;
static UINT32                       m_ulAudioPlayStopStatus = AHC_AUDIO_EVENT_EOF;
static AHC_OS_SEMID                 m_AHLMessageSemID;
static AHC_OS_SEMID                 m_AHLHPMessageSemID;
static AHC_OS_SEMID                 m_AHLHPMessageCountSemID;
static UINT8                        m_bSoundEffectExist     = AHC_FALSE;
static AHC_BOOL                     m_bSendAHLMessage       = AHC_TRUE;
static AHC_BOOL                     m_VidRecdCardSlowStop   = AHC_FALSE;
static AHC_BOOL                     m_APStopVideoRecord     = AHC_FALSE;
static ULONG                        m_APStopVideoRecord_VidTime = 0;
static ULONG                        m_APStopVideoRecord_AudTime = 0;

#if (VR_SLOW_CARD_DETECT)
static AHC_BOOL                     m_bCardSlow = AHC_FALSE;
#endif

static EDIT_BROWSER_MODE            m_EditBrowserMode       = EDIT_BROWSER_NONE;
static AHC_BOOL                     m_VideoRecEvent         = AHC_FALSE;
#if ((DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME) ||\
     (EMERGENTRECD_SUPPORT && (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_MOVE_FILE)))
static AHC_RTC_TIME                 m_VideoRecStartRtcTime;
#endif
#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
static AHC_RTC_TIME                 m_EmerRecStartRtcTime;
#endif
MMP_ULONG                           glAhcBootComplete       = MMP_FALSE;
MMP_USHORT                          gsStillZoomIndex        = 0xFFFF;

#if (ENABLE_VIDEO_ERR_LOG)
static  INT8                        m_VideoRecordErrorFileName[MAX_FILE_NAME_SIZE];///< for the error callback of the video record
static  VIDEO_ERR_LOG               m_VideoErrLog = {0, 0, 0, 0, 0, 0, 0, 0, 0};
#endif


#if (AHC_ENABLE_VIDEO_STICKER)
UINT32                              m_uiVideoStampBufIndex  = 0;
#endif

#if (VR_SEAMLESS)
AHC_BOOL                            m_bSeamlessRecord       = AHC_FALSE;
#endif

#if (USE_INFO_FILE)
INFO_LOG                            m_InfoLog               = {0,0};
#endif

#if (ENABLE_SD_HOT_PLUGGING)
#if(defined(DEVICE_GPIO_2NDSD_PLUG))
static SDMMC_STATE                  m_eSDMMCState           = SDMMC_1STOUT_2NDOUT;
#else
static SDMMC_STATE                  m_eSDMMCState           = SDMMC_OUT;
#endif
static AHC_SD_MOUNT_STATUS          m_bSDMMCMountState      = AHC_FALSE;
#endif

static UINT8                        m_ubSDMMC_Class         = 0xFF;
static UINT8                        m_ub2ndSDMMC_Class      = 0xFF;
static AHC_BOOL                     gSpeaker_en             = AHC_FALSE;

static VR_WRITEFUNC                 *VR_WriteFunc           = NULL;

static AHC_BOOL                     gPIRStart               = AHC_FALSE;
static UINT32                       ulRecTimeLimit          = 0;

static AHC_BOOL                     m_bEnableFastAeAwb      = AHC_FALSE;

#if CLOCK_SETTING_TIME_LIMIT
static INT32						m_ClockSettingTimeLimit = 0;
#endif

#if (MOTION_DETECTION_EN)
UINT8       m_ubMotionMvTh          = 0xFF;
UINT8       m_ubMotionCntTh         = 0xFF;
#if defined(CFG_MVD_MODE_POWER_ON_ENABLE) && (VMD_ACTION != VMD_ACTION_NONE)
AHC_BOOL    m_ubMotionDtcEn         = AHC_TRUE;
#else
AHC_BOOL    m_ubMotionDtcEn         = AHC_FALSE;
#endif

/*
VideoFunc_EnterVMDMode() =>¡@AHC_TRUE
VideoFunc_ExitVMDMode() => AHC_FALSE
VideoRecMode_Start() => AHC_TRUE
*/
AHC_BOOL        m_bMotionDtcEn      = AHC_FALSE;

extern AHC_BOOL m_ubVMDStart;
extern UINT32   m_ulVMDRemindTime;
extern UINT32   m_ulVMDCountDown;
extern UINT32   m_ulVMDStopCnt;

#endif///< (MOTION_DETECTION_EN)

#if(SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
static PARKING_STATE_INFO   ParkingModeInfo;
static AHC_BOOL             ParkingTriggerRecord;
#endif

AHC_BOOL                            m_ubParkingModeEn       = AHC_FALSE;
AHC_BOOL                            m_ubPlaybackRearCam     = AHC_FALSE;
AHC_BOOL                            bTriggerLocked          = AHC_FALSE;
// for streaming
AHC_OS_SEMID ahc_semi = 0xff;
//
#if (SW_STICKER_EN == 1)
MMP_USHORT m_usStickerSrcW, m_usStickerSrcH;
MMP_USHORT m_usStickerCaptureW, m_usStickerCaptureH;
MMP_USHORT m_usStickerXoffset, m_usStickerYoffset;
MMP_UBYTE  m_ubStickerOsdId_0, m_ubStickerOsdId_1;
#endif///< (SW_STICKER_EN == 1)


static UINT8    RecordAGain = AHC_DEFAULT_VR_MIC_AGAIN;
static UINT8    RecordDGain = AHC_DEFAULT_VR_MIC_DGAIN; //8328 chip 0x51 is 0DB for Dgain.
static AHC_BOOL RecordBoosterOn = AHC_TRUE;

AHC_BOOL   m_ubCallBackStart = AHC_FALSE;
AHC_BOOL   m_ubMediaErrorFC  = AHC_FALSE;

#if (C005_PARKING_CLIP_Length_10PRE_10REC == 1)
static UINT32 m_uiParkRecordTime = 20;
#endif

#if (PLAY_SOUND_SEQUENTIALLY)
AHC_SOUND_FILE_QUEUE    gsAhcPlaySoundQue[PLAY_SOUND_FILE_QUEUE_SIZE] = {{AHC_SOUNDEFFECT_MAX_NUM, {0}},
																		{AHC_SOUNDEFFECT_MAX_NUM, {0}},
																		{AHC_SOUNDEFFECT_MAX_NUM, {0}},
																		#if (PLAY_SOUND_FILE_QUEUE_SIZE > 3)
																		{AHC_SOUNDEFFECT_MAX_NUM, {0}},
																		#endif
																		#if (PLAY_SOUND_FILE_QUEUE_SIZE > 4)
																		{AHC_SOUNDEFFECT_MAX_NUM, {0}},
																		#endif
																		#if (PLAY_SOUND_FILE_QUEUE_SIZE > 5)
																		{AHC_SOUNDEFFECT_MAX_NUM, {0}},
																		#endif
																		#if (PLAY_SOUND_FILE_QUEUE_SIZE > 6)
																		{AHC_SOUNDEFFECT_MAX_NUM, {0}},
																		#endif
																		#if (PLAY_SOUND_FILE_QUEUE_SIZE > 7)
																		{AHC_SOUNDEFFECT_MAX_NUM, {0}},
																		#endif
																		#if (PLAY_SOUND_FILE_QUEUE_SIZE > 8)
																		{AHC_SOUNDEFFECT_MAX_NUM, {0}},
																		#endif
																		#if (PLAY_SOUND_FILE_QUEUE_SIZE > 9)
																		{AHC_SOUNDEFFECT_MAX_NUM, {0}},
																		#endif
																		};
UINT8                   gubAhcPlaySoundQue = 0;
UINT8                   gubAhcPlaySoundQueEventNum = 0;
#endif

UINT16                  ulCheckGsensorCounter = 0;

static AHC_BOOL bRestartPreviewAsRecordingSetting = AHC_FALSE;

#if defined(WIFI_PORT) && (WIFI_PORT == 1)
UINT32 ulLastWiFiChagneTime = (UINT32) -1;
#endif

AHC_BOOL blockRealIDUIKeyTask = AHC_FALSE;
static UINT8 gbDelNormRecAfterEmr = AHC_FALSE;

static AHC_RTC_TIME                 m_VideoRecStartRtcTime;

static 	AHC_WINDOW_RECT	gsCustomerRearPrevWindow 	= {0, 0, 0, 0};
static 	AHC_WINDOW_RECT	gsCustomerFrontPrevWindow 	= {0, 0, 0, 0};

UINT8                  m_byTvSrcType = MMPF_VIF_SRC_NO_READY;

static UINT32 gulCurKeyEventID = EVENT_NONE;

/*===========================================================================
 * Extern varible
 *===========================================================================*/

extern AHC_PARAM                    glAhcParameter;
extern UINT8                        m_uiPlayAudioFmt;
extern AHC_BOOL                     m_bAHCAudioPlaying;
extern MMPS_AUDIO_MP3_INFO          m_gsAHCMp3Info;
extern MMPS_AUDIO_OGG_INFO          m_gsAHCOggInfo;
extern MMPS_AUDIO_WMA_INFO          m_gsAHCWmaInfo;
extern MMPS_AUDIO_WAV_INFO          m_gsAHCWavInfo;
#if (CLOCK_SETTING_FOR_C003_P000 == 1)
extern AHC_BOOL m_bClockSet;
#endif

extern MMP_USHORT                   gsCurPreviewMode[];

#if (SUPPORT_PARKINGMODE != PARKINGMODE_STYLE_3)
extern AHC_BOOL                     m_bFirstParkingVR;
#endif

#if (SUPPORT_GSENSOR && POWER_ON_BY_GSENSOR_EN)
extern UINT32		GSNRPowerOnThd;
extern AHC_BOOL		m_ubGsnrIsObjMove;
#endif
extern AHC_BOOL                     gbAhcDbgBrk;
/*===========================================================================
 * Extern function
 *===========================================================================*/

extern MMP_BOOL UartExecuteCommandEx( char* szCommand );
extern UINT32   AHC_VideoRecordPostWriteInfo(UINT32 ul_FileID);
extern void     AHC_CheckCallbackExisted( void );
extern void     DrawVideo_TimeStamp(UINT16 uiOSDid, UINT32 TimeStampOp, AHC_RTC_TIME* sRtcTime, UINT32* uiSrcAddr,
                                    UINT16* uwSrcW, UINT16* uwSrcH, UINT16* uwTempColorFormat,
						            UINT32* StickerX, UINT32* StickerY);

static void VRMediaSlowCB(void);

/*===========================================================================
 * Main body
 *===========================================================================*/

#if 0
void _____Utility_Function_________(){ruturn;} //dummy
#endif

void AHC_SetDelNormFileAfterEmr(UINT8 enable)
{
	gbDelNormRecAfterEmr = enable;
}

UINT8 AHC_NeedDeleteNormalAfterEmr(void)
{
	return gbDelNormRecAfterEmr;
}

void AHC_DeleteLatestNormalFile(void)
{
	UINT32 ulIndex;
	
	//AHC_UF_DumpDB();
	
#if(SORT_TYPE == SORT_TYPE_INCREASE)
	AHC_UF_GetTotalFileCount(&ulIndex);
	ulIndex = ulIndex - 1;
#else
	ulIndex = 0;
#endif
	AHC_UF_FileOperation_ByIdx(ulIndex,DCF_FILE_DELETE,NULL,NULL);
    AHC_SetDelNormFileAfterEmr(0);
}

AHC_MODE_ID AHC_GetAhcSysMode(void)
{
    return m_AhcSystemMode;
}

void AHC_SetAhcSysMode(AHC_MODE_ID uiMode)
{
    if (uiMode == AHC_MODE_IDLE) {
        MMPD_Icon_InitLinkSrc();
    }
    m_AhcSystemMode = uiMode;
}

int AHC_IsInVideoMode()
{
    return m_AhcSystemMode == AHC_MODE_VIDEO_RECORD ||
           m_AhcSystemMode == AHC_MODE_RECORD_VONLY ||
           m_AhcSystemMode == AHC_MODE_RECORD_PREVIEW;
}

void AIHC_Int2Str(UINT32 value, INT8 *string)
{
    UINT32 i,j;
    INT8   temp[16];

    for(i=0;i<16;i++) {
        if ( (value / 10) != 0 ) {
            temp[i] = '0' + value % 10;

            value = value / 10;
        }
        else {
            temp[i] = '0' + value % 10;
            break;
        }
    }

    for(j=0;j<(i+1);j++) {
        string[j] = temp[i - j];
    }

    string[j] = 0;
}

static MMP_USHORT AIHC_GetSystemPio(UINT8 pinGroup, UINT8 pinNum)
{
	MMP_GPIO_PIN piopin;

    piopin = pinNum;

    return piopin;
}

static AHC_BOOL AIHC_TimerIsrHandler(void* parameter1, void* ulParam)
{
    #if defined(SA_PL)
    AHC_SendAHLMessage(AHLM_TIMER_INDEX, (MMP_ULONG)ulParam, 0);
    #endif
    return AHC_TRUE;
}

static AHC_BOOL AIHC_InitGeneralFunc(void)
{
    MEMSET(m_MessageQueue, 0, sizeof(m_MessageQueue));
    m_MessageQueueIndex_W = 0;
    m_MessageQueueIndex_R = 0;

    return AHC_TRUE;
}

#if (USE_INFO_FILE)

INFO_LOG*  AHC_InfoLog(void)
{
    return &m_InfoLog;
}

void AHC_WriteSDInfoFile(void)
{
    AHC_ERR err;
    UINT32 ulFileId;
    UINT32 ulWriteCount;

    err = AHC_FS_FileOpen(AHC_INFO_LOG,
                    AHC_StrLen(AHC_INFO_LOG),
                    "wb",
                    AHC_StrLen("wb"),
                    &ulFileId);

    if(err == AHC_ERR_NONE){
        AHC_FS_FileWrite(ulFileId, (UINT8*)&m_InfoLog, sizeof(m_InfoLog), &ulWriteCount);
        AHC_FS_FileClose(ulFileId);
    }
}

void AHC_ReadSDInfoFile(void)
{
    AHC_ERR err;
    UINT32 ulFileId;
    UINT32 ulReadCount;

    err = AHC_FS_FileOpen(AHC_INFO_LOG,
                    AHC_StrLen(AHC_INFO_LOG),
                    "rb",
                    AHC_StrLen("rb"),
                    &ulFileId);

    if(err == AHC_ERR_NONE){
        AHC_FS_FileRead(ulFileId,(UINT8 *)&m_InfoLog, sizeof(m_InfoLog), &ulReadCount);
        AHC_FS_FileClose(ulFileId);
    }
}
#endif

#if 0
void _____PTZ_Function_________(){} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : AHC_SetPreviewZoomConfig
//  Description :
//------------------------------------------------------------------------------
/**
 @brief This API configures the abilities of digital zoom.

 This function configures the abilities of digital zoom. System calculates each
 zoom interval between steps according to the max multiplier. Currently, the zoom can
 only based on center of scene.

 Parameters:

 @param[in] bySteps        Max Zoom Step
 @param[in] byMaxRatio     Max Zoom Ratio
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_SetPreviewZoomConfig(UINT16 bySteps, UINT8 byMaxRatio)
{
    MMPS_DSC_SetZoomConfig    (bySteps, byMaxRatio);
    MMPS_3GPRECD_SetZoomConfig(bySteps, byMaxRatio);

    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetPreviewZoom
//  Description :
//------------------------------------------------------------------------------
/**
 @brief This API set Zoom-In / Zoom-Out

 This function set Zoom-In / Zoom-Out. Currently, the zoom can
 only based on center of scene.

 Parameters:

 @param[in] CaptureMode     Video mode or DSC mode
 @param[in] byDirection     Zoom-In or Zoom Out
 @param[in] byDirection     Zoom Step

 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_SetPreviewZoom(AHC_CAPTURE_MODE CaptureMode, AHC_ZOOM_DIRECTION byDirection, UINT8 bySteps)
{
    MMP_BOOL   		bInPreview  = MMP_FALSE;
    MMP_BOOL   		bInRecord   = MMP_FALSE;
    MMP_ERR 		error       = MMP_ERR_NONE;
    MMP_IBC_PIPEID 	ubPipe;
	MMP_SHORT  		sCurDir;
	MMP_USHORT 		usCurZoomStep;

    if (VIDEO_CAPTURE_MODE == CaptureMode) {
        MMPS_3GPRECD_GetPreviewPipeStatus(gsAhcCurrentSensor, &bInPreview);
        MMPS_3GPRECD_GetPreviewPipe(gsAhcCurrentSensor, &ubPipe);
        MMPS_3GPRECD_GetRecordPipeStatus(0/*ubEncIdx*/, &bInRecord);
        
        if (!bInPreview) {
            return AHC_FALSE;
        }
    }
    else if (STILL_CAPTURE_MODE == CaptureMode) {
        MMPS_DSC_GetPreviewStatus(&bInPreview);
		MMPS_DSC_GetPreviewPipe(&ubPipe);

        if (!bInPreview) {
            return AHC_FALSE;
        }
    }

	if (byDirection != AHC_SENSOR_ZOOM_STOP) {

		MMPD_PTZ_GetCurPtzStep(ubPipe, &sCurDir, &usCurZoomStep, NULL, NULL);

		if (byDirection == AHC_SENSOR_ZOOM_IN) {
	        sCurDir = MMP_PTZ_ZOOM_INC_IN;
	    }
	    else if (byDirection == AHC_SENSOR_ZOOM_OUT) {
	        sCurDir = MMP_PTZ_ZOOM_INC_OUT;
	    }

		if (((MMP_SHORT)usCurZoomStep + sCurDir * 1) <= 0)
			usCurZoomStep = 0;
		else
			usCurZoomStep += (sCurDir * 1);
	}

    if (byDirection == AHC_SENSOR_ZOOM_IN) {

        if (CaptureMode == STILL_CAPTURE_MODE) {
            error = MMPS_DSC_SetPreviewZoom(MMP_PTZ_ZOOMIN, usCurZoomStep);
        }
        else {
            if (bInRecord)
                error = MMPS_3GPRECD_SetPreviewZoom(MMPS_3GPRECD_ZOOM_PATH_BOTH, MMP_PTZ_ZOOMIN, usCurZoomStep);
            else
                error = MMPS_3GPRECD_SetPreviewZoom(MMPS_3GPRECD_ZOOM_PATH_PREV, MMP_PTZ_ZOOMIN, usCurZoomStep);
        }
    }
    else if (byDirection == AHC_SENSOR_ZOOM_OUT) {

        if (CaptureMode == STILL_CAPTURE_MODE) {
            error = MMPS_DSC_SetPreviewZoom(MMP_PTZ_ZOOMOUT, usCurZoomStep);
        }
        else {
            if (bInRecord)
                error = MMPS_3GPRECD_SetPreviewZoom(MMPS_3GPRECD_ZOOM_PATH_BOTH, MMP_PTZ_ZOOMOUT, usCurZoomStep);
            else
                error = MMPS_3GPRECD_SetPreviewZoom(MMPS_3GPRECD_ZOOM_PATH_PREV, MMP_PTZ_ZOOMOUT, usCurZoomStep);
        }
    }
    else if (byDirection == AHC_SENSOR_ZOOM_STOP) {

    	if (CaptureMode == STILL_CAPTURE_MODE) {
            error = MMPS_DSC_SetPreviewZoom(MMP_PTZ_ZOOMSTOP, usCurZoomStep);
        }
        else {
            if (bInRecord)
                error = MMPS_3GPRECD_SetPreviewZoom(MMPS_3GPRECD_ZOOM_PATH_BOTH, MMP_PTZ_ZOOMSTOP, usCurZoomStep);
            else
                error = MMPS_3GPRECD_SetPreviewZoom(MMPS_3GPRECD_ZOOM_PATH_PREV, MMP_PTZ_ZOOMSTOP, usCurZoomStep);
        }
    }
    else {
        return AHC_FALSE;
    }

    return (error == MMP_ERR_NONE) ? AHC_TRUE : AHC_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_PreviewWindowOp
//  Description :
//------------------------------------------------------------------------------
/**
 @brief This API set/get Preview window of Front Cam / Rear Cam

 
 Parameters:
 
 @param[in] 	op		AHC_PREVIEW_WINDOW_OP_GET to set window rect,AHC_PREVIEW_WINDOW_OP_SET to set window rect
 						AHC_PREVIEW_WINDOW_FRONT/AHC_PREVIEW_WINDOW_REAR to choice window
 @param[in/out] pRect	in get window case, this is addressed to the memory of structure AHC_WINDOW_RECT which will be updated.
 						in set window case, use this addresed memory to set window rect.
 @retval return 0 is success ,others are unknow operations.
*/
int	AHC_PreviewWindowOp(int op, AHC_WINDOW_RECT * pRect)
{
	int subop,win;
	
	subop = op & AHC_PREVIEW_WINDOW_OP_MASK;
	win = op & AHC_PREVIEW_WINDOW_MASK;
	
	if(subop == AHC_PREVIEW_WINDOW_OP_GET)
	{
		if(win == AHC_PREVIEW_WINDOW_FRONT)
			memcpy(pRect,&gsCustomerFrontPrevWindow,sizeof(AHC_WINDOW_RECT));
		else
			memcpy(pRect,&gsCustomerRearPrevWindow,sizeof(AHC_WINDOW_RECT));
	}
	else if(subop == AHC_PREVIEW_WINDOW_OP_SET)
	{
		if(win == AHC_PREVIEW_WINDOW_FRONT)
			memcpy(&gsCustomerFrontPrevWindow,pRect,sizeof(AHC_WINDOW_RECT));
		else
			memcpy(&gsCustomerRearPrevWindow,pRect,sizeof(AHC_WINDOW_RECT));
	}
	else
		return -1;
		
	return 0;
}

//------------------------------------------------------------------------------
//  Function    : AHC_PlaybackZoom
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set playback zoom

 Performs zoom during still image playback.
 Parameters:
 @param[in] uwStartX The top-left corner's X of the zoom window.
 @param[in] uwStartY The top-left corner' Y of the zoom window.
 @param[in] uwWidth The width of zoom window.
 @param[in] uwHeight The height of zoom window.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_PlaybackZoom(UINT16 uwStartX, UINT16 uwStartY, UINT16 uwWidth, UINT16 uwHeight)
{
    MMP_ERR error;

	error = MMPS_DSC_PlaybackExecutePTZ(uwStartX, uwStartY, uwWidth, uwHeight);

    if (MMP_ERR_NONE == error)
    	return AHC_TRUE;
    else
        return AHC_FALSE;
}

#if 0
void _____VideoRecord_Function_________(){ruturn;} //dummy
#endif

AHC_BOOL AHC_KeyEventIDCheckConflict(UINT32 ulCurKeyEventID)
{
    if(((BUTTON_USB_B_DEVICE_DETECTED == gulCurKeyEventID)
        ||(BUTTON_USB_B_DEVICE_REMOVED == gulCurKeyEventID)
#if (SUPPORT_DUAL_SNR == 1 && DUALENC_SUPPORT == 1) //For TV decoder
        ||(BUTTON_TV_DECODER_SRC_TYPE == gulCurKeyEventID)
#endif        
        ) &&
        ((BUTTON_VRCB_RECDSTOP_CARDSLOW == ulCurKeyEventID)
        //|| (BUTTON_VRCB_MEDIA_ERROR == ulCurKeyEventID)
        //(BUTTON_VRCB_LONG_TIME_FILE_FULL == gulCurKeyEventID)
        || (BUTTON_VRCB_FILE_FULL == ulCurKeyEventID)
        || (BUTTON_VRCB_MEDIA_FULL == ulCurKeyEventID))){
        AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0);
        printc("gulCurKeyEventID:0x%x, ulCurKeyEventID:0x%xr\n", gulCurKeyEventID, ulCurKeyEventID);
        return AHC_FALSE;
    }
    return AHC_TRUE;
}

void AHC_SetCurKeyEventID(UINT32 ulCurKeyEventID)
{
    gulCurKeyEventID = ulCurKeyEventID;
}

UINT32 AHC_GetCurKeyEventID(void)
{
    return gulCurKeyEventID;
}

#if (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_DUAL_FILE)
static void VRVidRecdCardSlowStopCB(void)
{
    printc(FG_RED("\n<VRVidRecdCardSlowStopCB>\r\n"));

    #if (ENABLE_VIDEO_ERR_LOG)
    m_VideoErrLog.callBackError   = MMPS_3GPRECD_EVENT_SLOWCARDSTOPVIDRECD;
    m_VideoErrLog.errorParameter1 = 0;
    m_VideoErrLog.errorParameter2 = 0;
    #endif
    AHC_SetVidRecdCardSlowStop(AHC_TRUE);

#if (GPS_CONNECT_ENABLE)
    if(AHC_GPS_Module_Attached()){
        UINT8 bgps_en;
        if((AHC_Menu_SettingGetCB((char*)COMMON_KEY_GPS_RECORD_ENABLE, &bgps_en) == AHC_TRUE) && (bgps_en != COMMON_GPS_REC_INFO_OFF)){
            MMPF_OS_SetFlags(UartCtrlFlag, GPS_FLAG_SWITCHBUFFER_CLOSE, MMPF_OS_FLAG_SET);
        	//GPSCtrl_SwitchRawFileClose();
        	//AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, GPS_RAWFILE_CLOSE, 0);
        	//SetKeyPadEvent(GPS_RAWFILE_CLOSE);
        }
    }
#endif

}
#endif

#if 0
static void VRAPStopVideoRecordCB(ULONG ulLastRecdVidTime, ULONG ulLastRecdAudTime)
{
    printc("VR APStopVideoRecord CB  ulLastRecdVidTime=%d  ulLastRecdAudTime=%d\n", ulLastRecdVidTime, ulLastRecdAudTime);

    #if (ENABLE_VIDEO_ERR_LOG)
    m_VideoErrLog.callBackError   = MMPS_3GPRECD_EVENT_APSTOPVIDRECD;
    m_VideoErrLog.errorParameter1 = 0;
    m_VideoErrLog.errorParameter2 = 0;
    #endif
    AHC_SetAPStopVideoRecord(AHC_TRUE);
    AHC_SetAPStopVideoRecordTime(ulLastRecdVidTime, ulLastRecdAudTime);

    #if (GPS_CONNECT_ENABLE)
    if(AHC_GPS_Module_Attached())
    {
        UINT8 bgps_en;

        if((AHC_Menu_SettingGetCB((char*)COMMON_KEY_GPS_RECORD_ENABLE, &bgps_en) == AHC_TRUE) && (bgps_en != COMMON_GPS_REC_INFO_OFF)){
            MMPF_OS_SetFlags(UartCtrlFlag, GPS_FLAG_SWITCHBUFFER_CLOSE, MMPF_OS_FLAG_SET);
        	//GPSCtrl_SwitchRawFileClose();
        	//AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, GPS_RAWFILE_CLOSE, 0);
        	//SetKeyPadEvent(GPS_RAWFILE_CLOSE);
        }
    }
    #endif

    AHC_SendAHLMessage_HP(AHLM_UI_NOTIFICATION, BUTTON_VRCB_AP_STOP_VIDEO_RECD, 0);
}
#endif

void VRMediaFullCB(void)
{
    AHC_BOOL ahcRet = AHC_TRUE;    

    printc(FG_GREEN("\n<VRMediaFullCB>\r\n"));

    #if (ENABLE_VIDEO_ERR_LOG)
    m_VideoErrLog.callBackError   = MMPS_3GPRECD_EVENT_MEDIA_FULL;
    m_VideoErrLog.errorParameter1 = 0;
    m_VideoErrLog.errorParameter2 = 0;
    #endif

    ahcRet = AHC_KeyEventIDCheckConflict(BUTTON_VRCB_MEDIA_FULL);
    if(ahcRet != AHC_TRUE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,ahcRet); return;}      
    
    AHC_SetCurKeyEventID(BUTTON_VRCB_MEDIA_FULL);
    AHC_SendAHLMessage_HP(AHLM_UI_NOTIFICATION, BUTTON_VRCB_MEDIA_FULL, 0);
}

void VRFileFullCB(void)
{
    AHC_BOOL ahcRet = AHC_TRUE;    
    
#if (VR_SEAMLESS)
    MMPS_FW_OPERATION sMergerStatus = MMPS_FW_OPERATION_NONE;
    MMP_ERR sRet = MMP_ERR_NONE;

    #ifdef CAR_DV
    // NOP
    #else
    extern UINT32 RecordTimeOffset;
    UINT32 ulTimelimit;
    #endif
#endif

    printc(FG_GREEN("\n<VRFileFullCB>\r\n"));

    ahcRet = AHC_KeyEventIDCheckConflict(BUTTON_VRCB_FILE_FULL);
    if(ahcRet != AHC_TRUE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,ahcRet); return;}      
    
#if (VR_SEAMLESS)
    if (!uiGetParkingModeEnable) {
        sRet = MMPS_3GPRECD_RecordStatus(&sMergerStatus);
        if((sRet != MMP_ERR_NONE) || (sMergerStatus != MMPS_FW_OPERATION_PREENCODE)){
            printc(FG_RED("sMergerStatus:%d\r\n"),sMergerStatus);        
            AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet);
            return;
        }
    }
    
    #ifdef CAR_DV
    // NOP
    #else
    ulTimelimit = AHC_GetVRTimeLimit();
    printc("AHC_GetVRTimeLimit - %d\r\n", ulTimelimit);
    RecordTimeOffset += (ulTimelimit*1000/*ms*/);
    printc("RecordTimeOffset - %d\r\n", RecordTimeOffset);
    #endif

    ulCheckGsensorCounter = 0;
#endif

#if (ENABLE_VIDEO_ERR_LOG)
    m_VideoErrLog.callBackError   = MMPS_3GPRECD_EVENT_FILE_FULL;
    m_VideoErrLog.errorParameter1 = 0;
    m_VideoErrLog.errorParameter2 = 0;
#endif

    AHC_SetCurKeyEventID(BUTTON_VRCB_FILE_FULL);
    AHC_SendAHLMessage_HP(AHLM_UI_NOTIFICATION, BUTTON_VRCB_FILE_FULL, 0);
}

void VRLongTimeFileFullCB(void)
{
    printc(FG_GREEN("\n<VRLongTimeFileFullCB>\r\n"));

    AHC_SendAHLMessage_HP(AHLM_UI_NOTIFICATION, BUTTON_VRCB_LONG_TIME_FILE_FULL, 0);
}

void VideoRecordMediaSlowCallback(void)
{
    printc(FG_RED("\n<VideoRecordMediaSlowCallback>\r\n"));
    VRMediaSlowCB();
}

static void VRMediaSlowCB(void)
{
    printc(FG_RED("\n<VRMediaSlowCB>\r\n"));
    #if (ENABLE_VIDEO_ERR_LOG)
    m_VideoErrLog.callBackError   = MMPS_3GPRECD_EVENT_MEDIA_SLOW;
    m_VideoErrLog.errorParameter1 = 0;
    m_VideoErrLog.errorParameter2 = 0;
    #endif
    AHC_SendAHLMessage_HP(AHLM_UI_NOTIFICATION, BUTTON_VRCB_MEDIA_SLOW, 0);
}

#if (VR_SLOW_CARD_DETECT)
#if 1
static void VRPreGetTimeWhenCardSlowCB(AHC_BOOL bNorRecdEnable, UINT32 ulNorRecdTime, AHC_BOOL bEmergRecdEnable, UINT32 ulEmergRecdTime)
{
    printc(FG_RED("\n<VRPreGetTimeWhenCardSlowCB>\r\n"));
    printc("bNorRecdEnable=%d ulNorRecdTime=%d\r\n", bNorRecdEnable, ulNorRecdTime);
    printc("bEmergRecdEnable=%d ulEmergRecdTime=%d\r\n", bEmergRecdEnable, ulEmergRecdTime);

    m_bCardSlow = AHC_TRUE;
    //TODO : for NMEA file

    //AHC_SendAHLMessage_HP(AHLM_UI_NOTIFICATION, BUTTON_VRCB_PREGETTIME_CARDSLOW, 0);
#ifdef SLOW_CARD_RESTART
    AHC_WMSG_Draw(AHC_TRUE, WMSG_CARD_SLOW, 3);
    AHC_SendAHLMessage_HP(AHLM_UI_NOTIFICATION, BUTTON_VRCB_MEDIA_SLOW, 0);//ken.w 0602
#endif	
}

static void VRRecdStopWhenCardSlowCB(void)
{
    AHC_BOOL ahcRet = AHC_TRUE;
    
    printc(FG_GREEN("\n<VRRecdStopWhenCardSlowCB>\r\n"));

    m_bCardSlow = AHC_FALSE;
    AHC_SetAPStopVideoRecord(AHC_FALSE);
#if((EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_DUAL_FILE)||(EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE))
    AHC_SetKernalEmergencyStopStep(AHC_KERNAL_EMERGENCY_AHC_DONE);
#endif

    ahcRet = AHC_KeyEventIDCheckConflict(BUTTON_VRCB_RECDSTOP_CARDSLOW);
    if(ahcRet != AHC_TRUE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,ahcRet); return; }    
    
    AHC_SetCurKeyEventID(BUTTON_VRCB_RECDSTOP_CARDSLOW);
    AHC_SendAHLMessage_HP(AHLM_UI_NOTIFICATION, BUTTON_VRCB_RECDSTOP_CARDSLOW, 0);
}
#endif
#endif

void VRMediaErrorCB(AHC_ERR VideoStatus, AHC_ERR VideoFileErr)
{
    printc(FG_RED("\n<VRMediaErrorCB>\r\n"));
    printc("VideoStatus  : %x \r\n", VideoStatus);
    printc("VideoFileErr : %x \r\n", VideoFileErr);
	AHC_WMSG_Draw(AHC_TRUE, WMSG_INSERT_SD_AGAIN, 2);
    #if (ENABLE_VIDEO_ERR_LOG)
    m_VideoErrLog.callBackError   = MMPS_3GPRECD_EVENT_MEDIA_ERROR;
    m_VideoErrLog.errorParameter1 = VideoStatus;
    m_VideoErrLog.errorParameter2 = VideoFileErr;
    #endif
    m_ubMediaErrorFC = AHC_TRUE;
    AHC_SendAHLMessage_HP(AHLM_UI_NOTIFICATION, BUTTON_VRCB_MEDIA_ERROR, 0);
}

static void VRStartCB(void)
{
    printc(FG_GREEN("\n<VRStartCB>\r\n"));

    AHC_VideoRecordStartWriteInfo();
}

static void VRStopCB(void)
{
    // RTNA_DBG_Str(0, "<VRStopCB>\r\n");
#if 0
    AHC_SendAHLMessage_HP(AHLM_UI_NOTIFICATION, BUTTON_VRCB_VR_STOP, 0);
#else
    #if (VR_LED_EVENT_BLINK)
    AHC_SetVideoRecordEventStatus(AHC_FALSE);
    #endif

#if (GPS_CONNECT_ENABLE  && (GPS_FUNC & FUNC_VIDEOSTREM_INFO))

    if(AHC_GPS_Module_Attached()){
        UINT8 bgps_en;
        if((AHC_Menu_SettingGetCB((char*)COMMON_KEY_GPS_RECORD_ENABLE, &bgps_en) == AHC_TRUE) && (bgps_en != COMMON_GPS_REC_INFO_OFF)){

            if( AHC_GPS_NeedToFlushBackupBuffer() == AHC_TRUE )
            {
                GPSCtrl_EndRecord_Backup();
            }
            else
            {
                GPSCtrl_EndRecord();
            }
        }
    }
#endif
    #if (GSENSOR_CONNECT_ENABLE  && (GSENSOR_FUNC & FUNC_VIDEOSTREM_INFO))
    if(AHC_Gsensor_Module_Attached())
    {
        if( AHC_Gsensor_NeedToFlushBackupBuffer() == AHC_TRUE )
        {
            AHC_Gsensor_EndRecord_Backup();
        }
        else
        {
            AHC_Gsensor_EndRecord();
        }
    }
    #endif


#if (VR_VIDEO_TYPE == COMMON_VR_VIDEO_TYPE_3GP)

    #if (GPS_CONNECT_ENABLE)
    if(AHC_GPS_Module_Attached())
    {
        #if (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_MOVE_FILE)
        {
                //if(uiGetParkingModeEnable())
                //{
                //}else
                {
                    UINT8 bgps_en;
                    if((AHC_Menu_SettingGetCB((char*)COMMON_KEY_GPS_RECORD_ENABLE, &bgps_en) == AHC_TRUE) && (bgps_en != COMMON_GPS_REC_INFO_OFF)){
                        MMPF_OS_SetFlags(UartCtrlFlag, GPS_FLAG_SWITCHBUFFER, MMPF_OS_FLAG_SET);
                    	#if (GPS_RAW_FILE_ENABLE == 1)
						//GPSCtrl_SwitchRawFilePingPongBuffer();
						//AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, GPS_SWITCH_BUFFER, 0);
						//SetKeyPadEvent(GPS_SWITCH_BUFFER);
						#endif
                    }
                }
        }
        #else
        {
            UINT8 bgps_en;
            if((AHC_Menu_SettingGetCB((char*)COMMON_KEY_GPS_RECORD_ENABLE, &bgps_en) == AHC_TRUE) && (bgps_en != COMMON_GPS_REC_INFO_OFF)){
                MMPF_OS_SetFlags(UartCtrlFlag, GPS_FLAG_SWITCHBUFFER, MMPF_OS_FLAG_SET);
            	#if (GPS_RAW_FILE_ENABLE == 1)
				//GPSCtrl_SwitchRawFilePingPongBuffer();
				//AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, GPS_SWITCH_BUFFER, 0);
				//SetKeyPadEvent(GPS_SWITCH_BUFFER);
				#endif
            }
        }
        #endif
    }
    #endif
#endif

#endif
}

static UINT32 VRPostProcessCB(MMP_ERR (*MuxWrite)(void *buf, MMP_ULONG size, MMP_ULONG *wr_size, MMP_ULONG ul_FileID),MMP_ULONG ulbackFileID)
{
    printc(FG_GREEN("\n<VRPostProcessCB>\r\n"));

    VR_WriteFunc = MuxWrite;

    // Add below code ( #if ~ #endif ) since VRStopCB() did not be called after Emergency recording finished
    // Remove it after VRStopCB() is called from driver
    #if (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE && GPS_RAW_FILE_ENABLE == 0 && GSENSOR_RAW_FILE_ENABLE == 0)
        #if (GPS_CONNECT_ENABLE && GSENSOR_CONNECT_ENABLE)
        if((AHC_GetKernalEmergencyStopStep() == AHC_KERNAL_EMERGENCY_RECORD) &&
           (AHC_GPS_NeedToFlushBackupBuffer() == AHC_FALSE && AHC_Gsensor_NeedToFlushBackupBuffer() == AHC_FALSE))
        {
            VRStopCB();
        }

        #endif
    #endif

    //Blocking action for 3gp merger task.
    AHC_VideoRecordPostWriteInfo(ulbackFileID);

#if (GPS_CONNECT_ENABLE) && (GPS_FUNC & FUNC_VIDEOSTREM_INFO) && (GPS_USE_FILE_AS_DATABUF)
    GPSCtrl_CloseInfoFile();

#if ((EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_DUAL_FILE) || (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE))
    if( AHC_GPS_Module_Attached() == AHC_TRUE && AHC_GetKernalEmergencyStopStep() == AHC_KERNAL_EMERGENCY_RECORD ){
        AHC_GPS_TriggerRestorePreRecordInfo( AHC_TRUE, m_ulEventPreRecordTime + ((OSTimeGet() - m_ulEventHappenTime)*1000)/ OS_TICKS_PER_SEC, m_bCurrentTimeLessThanPreRecord );
    }
#endif
#endif

#if (GSENSOR_CONNECT_ENABLE) && (GSENSOR_FUNC & FUNC_VIDEOSTREM_INFO) && (GSNR_USE_FILE_AS_DATABUF)
    AHC_Gsensor_CloseInfoFile();
#if ((EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_DUAL_FILE) || (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE))
    if( AHC_Gsensor_Module_Attached() == AHC_TRUE && AHC_GetKernalEmergencyStopStep() == AHC_KERNAL_EMERGENCY_RECORD ){
        AHC_Gsensor_TriggerRestorePreRecordInfo( AHC_TRUE, m_ulEventPreRecordTime + ((OSTimeGet() - m_ulEventHappenTime)*1000)/ OS_TICKS_PER_SEC, m_bCurrentTimeLessThanPreRecord );
    }
#endif
#endif
    
    return 0;
}

void VRSeamlessCB(void)
{
    printc(FG_GREEN("\n<VRSeamlessCB>\r\n"));
	
    AHC_SendAHLMessage_HP(AHLM_UI_NOTIFICATION, BUTTON_VRCB_SEAM_LESS, 0);
}

#if (MOTION_DETECTION_EN)
void VRMotionDetectCB(void)
{
    static AHC_BOOL bRunning = AHC_FALSE;
    static MMP_ULONG ulPreviousTime = 0;
    MMP_ULONG ulCurrentTime;
	MD_params_in_t param;

    if( bRunning == AHC_TRUE ) //Add this protection since it may be called by multi-tasks
    {
        return;
    }

    bRunning = AHC_TRUE;

    //printc("VR MD CB\n");

    #if(SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_1)
    if(uiGetParkingModeEnable())
    {
        #if 0 // TBD
        m_ulVMDCountDown = AHC_GetVMDRecTimeLimit() * 8;
        #endif

        if(m_ubVMDStart)
        {
            // Update VMD stop recording timer
            m_ulVMDStopCnt = AHC_GetVMDRecTimeLimit() * 8; //
        }

        if(!VideoFunc_RecordStatus())
        {
            AHC_SendAHLMessage_HP(AHLM_UI_NOTIFICATION, BUTTON_VRCB_MOTION_DTC, 0);
        }
    }
    #elif(SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_2)
        if(uiGetParkingModeEnable())
        {
            ulCurrentTime = (OSTimeGet() * 1000) / OS_TICKS_PER_SEC;
	        if( (ulCurrentTime - ulPreviousTime) > 500 ) // SW debounce
	        {
	            ulPreviousTime = ulCurrentTime;
	            AHC_SendAHLMessage_HP(AHLM_UI_NOTIFICATION, BUTTON_VRCB_MOTION_DTC, 0);
	        }
        }
    #elif(SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_3)
    if(uiGetParkingModeEnable())
    {
        ulCurrentTime = (OSTimeGet() * 1000) / OS_TICKS_PER_SEC;
        if( (ulCurrentTime - ulPreviousTime) > 500 ) // SW debounce
        {
            ulPreviousTime = ulCurrentTime;
            AHC_SendAHLMessage_HP(AHLM_UI_NOTIFICATION, BUTTON_VRCB_MOTION_DTC, 0);
        }
    }
    #else
    {
        ulCurrentTime = (OSTimeGet() * 1000) / OS_TICKS_PER_SEC;
        if( (ulCurrentTime - ulPreviousTime) > 500 ) // SW debounce
        {
            ulPreviousTime = ulCurrentTime;
            AHC_SendAHLMessage_HP(AHLM_UI_NOTIFICATION, BUTTON_VRCB_MOTION_DTC, 0);
        }
    }
    #endif

    bRunning = AHC_FALSE;

}
#endif

static void VREmerDoneCB(void)
{
    printc(FG_GREEN("\n<VREmerDoneCB>\n"));

#if (VR_SLOW_CARD_DETECT)
    if(m_bCardSlow)
        return;
#endif

    #if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
    //m_bStartEmerVR = AHC_FALSE;
    #if ((EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_DUAL_FILE) || (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE))
    AHC_SetKernalEmergencyStopStep(AHC_KERNAL_EMERGENCY_STOP);
    #endif
    AHC_SendAHLMessage_HP(AHLM_UI_NOTIFICATION, BUTTON_VRCB_EMER_DONE, 0);
    #endif
}

#if (SUPPORT_DUAL_SNR)
//------------------------------------------------------------------------------
//  Function    : SNTvSrcTypeCB
//  Description : This Callback function will be called once TV source is changed
//                or removed.
//  InParam     : byTvSrcType - TV type: 0 - None, 1 - PAL, 2 - NTSC
//                byFalg - Reserved
//------------------------------------------------------------------------------
void SNTvSrcTypeCB( MMP_UBYTE byTvSrcType, MMP_UBYTE byFalg )
{
    printc(FG_GREEN("\n<SNTvSrcTypeCB> %d\r\n"), byTvSrcType);

    m_byTvSrcType = byTvSrcType;

	if(byTvSrcType == MMPF_VIF_SRC_NO_READY)
		MMPS_Sensor_SetPreviewDelayCount( SCD_SENSOR, DISP_DISABLE_PREVIEW_DELAY_COUNT );
	else
		MMPS_Sensor_SetPreviewDelayCount( SCD_SENSOR, DISP_PREVIEW_DELAY_COUNT_SCD_SENSOR );

    AHC_SetCurKeyEventID(BUTTON_TV_DECODER_SRC_TYPE);
    AHC_SendAHLMessage_HP(AHLM_UI_NOTIFICATION, BUTTON_TV_DECODER_SRC_TYPE, 0);
}
#endif

#if (DUALENC_SUPPORT == 1)
void DualEncRecordCbFileFullCB(void)
{
    RTNA_DBG_Str0("## Dual-Enc File Full CB ##\r\n");
}

void VideoRecordCbEncStart(void)
{
    RTNA_DBG_Str0("## Encode start CB ##\r\n");
}

void VideoRecordCbEncStop(void)
{
    RTNA_DBG_Str0("## Encode stop CB ##\r\n");
}

MMP_ULONG VideoRecordCbPostProcess(MMP_ERR (*MuxWrite) (void *buf, MMP_ULONG size, MMP_ULONG *wr_size, MMP_ULONG ulFileID), MMP_ULONG ulFileID)
{
    RTNA_DBG_Str0("## Post-processing CB ##\r\n");
    RTNA_DBG_Str0("## FileID:");
    RTNA_DBG_Long0(ulFileID);
    RTNA_DBG_Str0(" ## \r\n");
    return 0;
}

MMP_BOOL VideoRecordCbStreaming(MMPS_3GPRECD_STREAMCB_TYPE cbmode, MMP_ULONG framesize, MMPS_3GPRECD_STREAM_INFO* moveinfo)
{
    RTNA_DBG_Str0("## VideoRecordCbStreaming CB ##\r\n");
    return MMP_TRUE;
}
#endif

void AHC_VideoRecordStartWriteInfo( void )
{
    // printc("--AHC_VideoRecordStartWriteInfo--\r\n");

#if (GPS_CONNECT_ENABLE  && (GPS_FUNC & FUNC_VIDEOSTREM_INFO))
    if(AHC_GPS_Module_Attached()){
        UINT8 bgps_en;
        if((AHC_Menu_SettingGetCB((char*)COMMON_KEY_GPS_RECORD_ENABLE, &bgps_en) == AHC_TRUE) && (bgps_en != COMMON_GPS_REC_INFO_OFF)){
            GPSCtrl_StartRecord();
        }
    }
#endif

#if (GSENSOR_CONNECT_ENABLE && (GSENSOR_FUNC & FUNC_VIDEOSTREM_INFO))
    if(AHC_Gsensor_Module_Attached()) {
        AHC_Gsensor_StartRecord();
    }
#endif

}

UINT32 AHC_VideoRecordPostWriteInfo(UINT32 ul_FileID)
{

    #if (((GPS_CONNECT_ENABLE) && (GPS_FUNC & FUNC_VIDEOSTREM_INFO)) || \
        ((GSENSOR_CONNECT_ENABLE) && (GSENSOR_FUNC & FUNC_VIDEOSTREM_INFO)) )
    UINT32 ulWriteSize = 0;
    UINT32 ulTotalWSize = 0;
    #endif
    printc("--AHC_VideoRecordPostWriteInfo--\r\n");


#if ((GPS_CONNECT_ENABLE) && (GPS_FUNC & FUNC_VIDEOSTREM_INFO))
#if (AVI_IDIT_CHUNK_EN == 1)
	{
		//extern unsigned int MMPS_3GPMUX_Build_IDIT(void **ptr);
		void			*ptr;
		unsigned int	cb, wr;
		
		cb = MMPS_3GPMUX_Build_IDIT(&ptr);
		VR_WriteFunc(ptr, cb, &wr, ul_FileID);
		if (cb == wr) {
			ulTotalWSize += wr;
		}
		else {
			printc("ERROR: %s %d write IDIT\r\n", __func__, __LINE__);
		}
	}
#endif //#if (AVI_IDIT_CHUNK_EN == 1)

    if(AHC_GPS_Module_Attached())
    {
        UINT8 bgps_en;
        if((AHC_Menu_SettingGetCB((char*)COMMON_KEY_GPS_RECORD_ENABLE, &bgps_en) == AHC_TRUE) && (bgps_en != COMMON_GPS_REC_INFO_OFF)){
            GPSCtrl_SetHeaderForAviChuck_GPSConfig();
            VR_WriteFunc((void *)GPSCtrl_HeaderAddressForAviChuck(0),8,&ulWriteSize, ul_FileID);
            ulTotalWSize += ulWriteSize;
            VR_WriteFunc((void *)GPSCtrl_AttributeAddressForAviChuck(), sizeof(GPS_ATTRIBUTE), &ulWriteSize, ul_FileID);
            ulTotalWSize += ulWriteSize;

            GPSCtrl_SetHeaderForAviChuck();
            VR_WriteFunc((void *)GPSCtrl_HeaderAddressForAviChuck(1),8,&ulWriteSize, ul_FileID);
            ulTotalWSize += ulWriteSize;

            #if (GPS_USE_FILE_AS_DATABUF)
            {
                extern UINT32 ulGPSInfoFileByte, ulGPSInfoLastByte;
                extern UINT32 ulGPSInfoFileId;

                UINT8  temp[256];
                UINT32 ultotalRSize, ulReadCount;
                pGPSInfoChuck InfoAddr;

                AHC_FS_FileSeek(ulGPSInfoFileId, 0, AHC_FS_SEEK_SET);

                //Read data from temp file then write to AVI.
                for (ultotalRSize = ulGPSInfoFileByte; (int)ultotalRSize > 0 ;)
                {
                    if (AHC_FS_FileRead(ulGPSInfoFileId,
                                        (UINT8 *)temp,
                                        sizeof(temp) / sizeof(UINT8),
                                        &ulReadCount) != AHC_ERR_NONE ||
                        ulReadCount == 0) {
                        break;
                    }

                    VR_WriteFunc((void *)temp,ulReadCount,&ulWriteSize, ul_FileID);
                    ulTotalWSize += ulWriteSize;
                    ultotalRSize -= ulReadCount;
                }

                //Flush last data from ping-pong buffer
                if( AHC_GPS_NeedToFlushBackupBuffer() == AHC_TRUE )
                {
                    AHC_GPS_FlushBackupBuffer( AHC_FALSE );
                    InfoAddr = (pGPSInfoChuck)GPSCtrl_GetBackupDataAddr();
                }
                else
                {
                    InfoAddr = (pGPSInfoChuck)GPSCtrl_GetLastDataAddr();
                }

                if(InfoAddr!=NULL)
                {
                    VR_WriteFunc((void *)InfoAddr,ulGPSInfoLastByte,&ulWriteSize, ul_FileID);
                    ulTotalWSize += ulWriteSize;
                }
            }
            #else
            VR_WriteFunc((void *)GPSCtrl_InfoAddressForAviChuck(),GPSCtrl_InfoSizeForAviChuck(),&ulWriteSize, ul_FileID);
            ulTotalWSize += ulWriteSize;
            #endif//GPS_USE_FILE_AS_DATABUF
        }
    }
#endif

#if (GSENSOR_CONNECT_ENABLE) && (GSENSOR_FUNC & FUNC_VIDEOSTREM_INFO)
    if(AHC_Gsensor_Module_Attached())
    {
        AHC_Gsensor_SetAttribChuckHeader();
        VR_WriteFunc((void *)AHC_Gsensor_GetChuckHeaderAddr(0),8,&ulWriteSize, ul_FileID);
        ulTotalWSize += ulWriteSize;
        VR_WriteFunc((void *)AHC_Gsensor_GetAttributeAddr(), sizeof(GSNR_ATTRIBUTE), &ulWriteSize, ul_FileID);
        ulTotalWSize += ulWriteSize;

        AHC_Gsensor_SetInfoChuckHeader();
        VR_WriteFunc((void *)AHC_Gsensor_GetChuckHeaderAddr(1),8,&ulWriteSize, ul_FileID);
        ulTotalWSize += ulWriteSize;

        #if (GSNR_USE_FILE_AS_DATABUF)
        {
            extern UINT32 m_ulInfoFileByte, m_ulInfoLastByte;
            extern UINT32 m_ulInfoFileId;

            UINT8  temp[256];
            UINT32 ultotalRSize, ulReadCount;
            PGSNR_INFOCHUCK InfoAddr;
            AHC_ERR err;

            AHC_FS_FileSeek(m_ulInfoFileId, 0, AHC_FS_SEEK_SET);

            //Read data from temp file then write to AVI.
            for (ultotalRSize = m_ulInfoFileByte; ultotalRSize > 0 ;)
            {
                err = AHC_FS_FileRead(m_ulInfoFileId,
                                    (UINT8 *)temp,
                                    256,
                                    &ulReadCount);

                if (err != AHC_ERR_NONE || ulReadCount == 0)
                {
                    break;
                }

                VR_WriteFunc((void *)temp,ulReadCount,&ulWriteSize, ul_FileID);
                ulTotalWSize += ulWriteSize;
                ultotalRSize -= ulReadCount;
            }

            //Flush last data from ping-pong buffer
            if( AHC_Gsensor_NeedToFlushBackupBuffer() == AHC_TRUE )
            {
                AHC_Gsensor_FlushBackupBuffer( AHC_FALSE );
                InfoAddr = (PGSNR_INFOCHUCK)AHC_Gsensor_GetBackupDataAddr();
            }
            else
            {
                InfoAddr = (PGSNR_INFOCHUCK)AHC_Gsensor_GetLastDataAddr();
            }

            if(InfoAddr!=NULL)
            {
                VR_WriteFunc((void *)InfoAddr,m_ulInfoLastByte,&ulWriteSize, ul_FileID);
                ulTotalWSize += ulWriteSize;
            }
        }
        #else
        VR_WriteFunc((void *)AHC_Gsensor_GetInfoChuckAddr(),AHC_Gsensor_GetInfoChuckSize(),&ulWriteSize, ul_FileID);
        ulTotalWSize += ulWriteSize;
        #endif//GSNR_USE_FILE_AS_DATABUF
    }
#endif

#if (GPS_CONNECT_ENABLE == 1)
    if(AHC_GPS_Module_Attached())
    {
#if (GPS_RAW_FILE_ENABLE == 1)
        UINT8 bgps_en;

        if (AHC_Menu_SettingGetCB((char*)COMMON_KEY_GPS_RECORD_ENABLE, &bgps_en) == AHC_TRUE) {
            switch (bgps_en) {
            case RECODE_GPS_OFF:
            case RECODE_GPS_IN_VIDEO:
                // NOP
                break;

            default:
                GPSCtrl_GPSRawClose();
                break;
            }
        }
#endif
#if (GPS_KML_FILE_ENABLE == 1)
        GPSCtrl_KMLGen_Write_TailAndClose();
#endif
    }
#endif

    #if(GSENSOR_RAW_FILE_ENABLE == 1 && GPS_CONNECT_ENABLE == 0)
        GPSCtrl_GPSRawClose();
    #endif

    #if (GPS_CONNECT_ENABLE == 1)
    if(AHC_GPS_Module_Attached() && !AHC_IsEmerRecordStarted())
    {
        #if (GPS_RAW_FILE_EMER_EN == 1)
            UINT8 bgps_en;
            if (AHC_Menu_SettingGetCB((char*)COMMON_KEY_GPS_RECORD_ENABLE, &bgps_en) == AHC_TRUE){
                switch (bgps_en) {
                case RECODE_GPS_OFF:
                case RECODE_GPS_IN_VIDEO:
                    // NOP
                    break;

                default:
                    GPSCtrl_GPSRawClose_Emer();
                    break;
                }
            }
        #endif
    }
    #endif

    #if(GSENSOR_RAW_FILE_ENABLE == 1 && GPS_CONNECT_ENABLE == 0 && GPS_RAW_FILE_EMER_EN == 1)
    	if(!AHC_IsEmerRecordStarted())
        GPSCtrl_GPSRawClose_Emer();
    #endif


    #if ( ((GPS_CONNECT_ENABLE) && (GPS_FUNC & FUNC_VIDEOSTREM_INFO))     || \
          ((GSENSOR_CONNECT_ENABLE) && (GSENSOR_FUNC & FUNC_VIDEOSTREM_INFO)) )
    return ulTotalWSize;
    #else
    return 0;
    #endif
}

UINT32 AHC_GetVRTimeLimit(void)
{
#if(SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
    if(uiGetParkingModeEnable())
    {
        #if(SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_1)
        return 60;
        #elif(SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_2)
        {
			UINT32	m_VRTimeLimit = 60;//Unit:seconds
			if(uiGetParkingModeEnable() || uiGetParkingModeMenuState())
			{
				m_VRTimeLimit = 60; // Set 16 to ensure the length is larger than 15.
			}
			return m_VRTimeLimit;
		}
        #endif
#if defined(PARKINGMODE_STYLE_4) && (SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_4) //SZ
        return 60; //UNIT: Sec
#endif        
    }
    else
#endif
    {
        return AHC_GetVideoRecTimeLimit();
    }
    return 100; //UNIT: Sec
}

UINT32 AHC_SetVRTimeLimit(UINT32 ulTimeLimit)
{
    if (ulTimeLimit < ((0x7FFFFFFF - VR_TIME_LIMIT_OFFSET) / 1000)) {
        ulTimeLimit = ulTimeLimit * 1000 + VR_TIME_LIMIT_OFFSET;
    }

    MMPS_3GPRECD_SetFileTimeLimit(ulTimeLimit);
    ulRecTimeLimit = ulTimeLimit;
    return 0;
}

void AHC_SetVRWithWNR(AHC_BOOL bEnable)
{
#if (WNR_EN)
    printc("AHC_SetVRWithWNR %d\r\n",bEnable);
    if (bEnable) {
       MMPS_AUDIO_EnableWNR();
    }
    else {
       MMPS_AUDIO_DisableWNR();
    }
#endif    
}

#if (VR_SEAMLESS) //Fix compiler error for AIT_BOARD_C000_P001
UINT8* AHC_GetCurrentVRFileName(AHC_BOOL bFullName)
{
    if(bFullName)
        return (UINT8*)m_CurVRFullName;
    else
        #if(SUPPORT_PARKINGMODE == PARKINGMODE_NONE)
        return (UINT8*)m_VideoRFileName;
        #else
        if(!uiGetParkingModeEnable())
        return (UINT8*)m_VideoRFileName;
        else
        return (UINT8*)m_VideoParkFileName;
        #endif
}

UINT16 AHC_GetCurrentVRDirKey(void)
{
    return m_usVideoRDirKey;
}

UINT16 AHC_GetCurrentParkingDirKey(void)
{
#if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
    return m_usVideoParkDirKey;
#else
    return 0;
#endif
}

AHC_BOOL AHC_ParkingModeRecStop()
{
    MMPS_3GPRECD_StopRecord();

    #if (FS_FORMAT_FREE_ENABLE)
    // Note: Below setting must be set after MMPS_3GPRECD_StopRecord() which will return after finished writing video file.
    MMPS_FORMATFREE_EnableWrite( 0 ); //Disable "Format Free Write" which will not update FAT table
    #endif

    return AHC_TRUE;
}
AHC_BOOL AHC_ParkingModePostProcess(void)
{
    #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
    if(uiGetParkingModeEnable())
    {
        //DCF_DB_TYPE sType;
        //sType = AHC_UF_GetDB();
        #if(GPS_RAW_FILE_ENABLE == 1)
        GPSCtrl_SwitchRawFilePingPongBuffer();
        #endif

        AHC_UF_SelectDB(DCF_DB_TYPE_2ND_DB);

        AHC_UF_PostAddFile(m_usVideoParkDirKey,(INT8*)m_VideoParkFileName);
        //AHC_UF_SelectDB(sType);
    }
    #endif

    return AHC_TRUE;
}

#if 0
void _____Emer_Record_________(){ruturn;} //dummy
#endif
AHC_BOOL m_bEmerVRed;
AHC_BOOL AHC_SetEmerRecorded(AHC_BOOL bEmerRecored)
{
    m_bEmerVRed = bEmerRecored;
    return AHC_TRUE;
}
AHC_BOOL AHC_GetEmerRecorded(void)
{
    return m_bEmerVRed;
}


AHC_BOOL AHC_IsEmerRecordStarted(void)
{
    #if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
    return m_bStartEmerVR;
    #else
    return AHC_FALSE;
    #endif
}

AHC_BOOL AHC_SetEmerRecordStarted(AHC_BOOL bEmerRecordStarted)
{
    #if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
    m_bStartEmerVR = bEmerRecordStarted;
    if(bEmerRecordStarted)
    AHC_SetEmerRecorded(bEmerRecordStarted);
    return AHC_TRUE;
    #else
    return AHC_FALSE;
    #endif
}

AHC_BOOL AHC_IsEmerPostDone(void)
{
    #if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
    return m_bEmerVRPostDone;
    #else
    return AHC_FALSE;
    #endif
}

#if ((EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_DUAL_FILE) || (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE))
AHC_BOOL AHC_SetKernalEmergencyStopStep(AHC_KERNAL_EMERGENCY_STOP_STEP bKernalEmergencyStopStep)
{
    #if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
    m_bKernalEmergencyStopStep = bKernalEmergencyStopStep;
    return AHC_TRUE;
    #else
    return AHC_FALSE;
    #endif
}

AHC_KERNAL_EMERGENCY_STOP_STEP AHC_GetKernalEmergencyStopStep(void)
{
    #if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
    return m_bKernalEmergencyStopStep;
    #else
    return AHC_FALSE;
    #endif
}
#endif
AHC_BOOL AHC_StartEmerRecord(void)
{
    #if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
    DCF_DB_TYPE sCurType;
    MMP_BYTE    chDirName[16];
    UINT8       bCreateNewDir;
    MMPS_3GPRECD_CONTAINER      ContainerType;
    UINT32                      Param;
    AHC_BOOL    bRet;
    #if (GPS_RAW_FILE_EMER_EN == 1 || GSENSOR_RAW_FILE_ENABLE == 1)
    MMP_BYTE                    bGPSRawFileName_Emer[MAX_FILE_NAME_SIZE];
    #endif
    MMP_ERR		err;

    #if (GPS_RAW_FILE_EMER_EN == 1)
    GPSCtrl_SetGPSRawStart_Emer(AHC_TRUE);
    GPSCtrl_SetGPSRawWriteFirst(AHC_FALSE);
    #endif

    MEMSET(m_chEmerVRFullName , 0, sizeof(m_chEmerVRFullName));
    MEMSET(chDirName         , 0, sizeof(chDirName));
    MEMSET(m_chEmerVRFileName, 0, sizeof(m_chEmerVRFileName));
#if ((UVC_HOST_VIDEO_ENABLE == 1  || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)
    MEMSET(m_EmrVRRearFullName , 0, sizeof(m_EmrVRRearFullName));
    MEMSET(m_EmrRearVideoRFileName , 0, sizeof(m_EmrRearVideoRFileName));
#endif
    sCurType = AHC_UF_GetDB();

    AHC_UF_SelectDB(DCF_DB_TYPE_3RD_DB);

#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_NORMAL)
    bRet = AHC_UF_GetName(&m_uwEmerVRDirKey, (INT8*)chDirName, (INT8*)m_chEmerVRFileName, &bCreateNewDir);
    if ( bRet == AHC_TRUE) {
        STRCPY(m_chEmerVRFullName,(INT8*)AHC_UF_GetRootDirName());
        STRCAT(m_chEmerVRFullName,"\\");
        STRCAT(m_chEmerVRFullName,chDirName);

        if ( bCreateNewDir ) {
            MMPS_FS_DirCreate((INT8*)m_chEmerVRFullName, STRLEN(m_chEmerVRFullName));
            AHC_UF_AddDir(chDirName);
        }

        STRCAT(m_chEmerVRFullName,"\\");
        STRCAT(m_chEmerVRFullName,(INT8*)m_chEmerVRFileName);
    }
#elif (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
{
    MMP_ULONG ulEmergentRecordingOffset;
    int   nSecondOffset = 0;

    AHC_RTC_GetTime(&m_EmerRecStartRtcTime);
    MMPS_3GPRECD_GetEmergentRecordingOffset(&ulEmergentRecordingOffset);

    nSecondOffset = -1*(ulEmergentRecordingOffset/1000);
    AHC_DT_ShiftRtc( &m_EmerRecStartRtcTime, nSecondOffset);
    bRet = AHC_UF_GetName2(&m_EmerRecStartRtcTime, (INT8*)m_chEmerVRFullName, (INT8*)m_chEmerVRFileName, &bCreateNewDir);
#if ((UVC_HOST_VIDEO_ENABLE == 1  || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)
    AHC_UF_SetRearCamPathFlag(AHC_TRUE);
    AHC_UF_GetName2(&m_EmerRecStartRtcTime, (INT8*)m_EmrVRRearFullName, (INT8*)m_EmrRearVideoRFileName, &bCreateNewDir);
    printc("bRearFileName %s %d\r\n",m_EmrVRRearFullName,__LINE__);
    AHC_UF_SetRearCamPathFlag(AHC_FALSE);
#endif
}
#endif

    if(bRet == AHC_TRUE) {

        AIHC_GetMovieConfig(AHC_CLIP_CONTAINER_TYPE, &Param);
        ContainerType   = Param;

        STRCAT(m_chEmerVRFullName, EXT_DOT);
#if ((UVC_HOST_VIDEO_ENABLE == 1  || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)
		STRCAT(m_EmrVRRearFullName, EXT_DOT);
#endif
        #if (GPS_CONNECT_ENABLE == 1)
        if(AHC_GPS_Module_Attached())
        {
            #if (GPS_RAW_FILE_ENABLE == 1)
            MEMSET(bGPSRawFileName_Emer, 0, sizeof(bGPSRawFileName_Emer));
            STRCPY(bGPSRawFileName_Emer, m_chEmerVRFullName);
            STRCAT(bGPSRawFileName_Emer, GPS_RAW_FILE_EXTENTION);
            #endif
        }
        #endif

        #if(GSENSOR_RAW_FILE_ENABLE == 1 && GPS_CONNECT_ENABLE == 0)
            MEMSET(bGPSRawFileName_Emer, 0, sizeof(bGPSRawFileName_Emer));
            STRCPY(bGPSRawFileName_Emer, m_chEmerVRFullName);
            STRCAT(bGPSRawFileName_Emer, GPS_RAW_FILE_EXTENTION);
        #endif

        if(ContainerType == MMPS_3GPRECD_CONTAINER_3GP) {
            STRCAT(m_chEmerVRFullName, MOVIE_3GP_EXT);
#if ((UVC_HOST_VIDEO_ENABLE == 1  || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)
            STRCAT(m_EmrVRRearFullName, MOVIE_3GP_EXT);
#endif
        }
        else {
            STRCAT(m_chEmerVRFullName, MOVIE_AVI_EXT);
#if ((UVC_HOST_VIDEO_ENABLE == 1  || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)
            STRCAT(m_EmrVRRearFullName, MOVIE_AVI_EXT);
#endif
        }

        #if (GPS_CONNECT_ENABLE == 1)
        if(AHC_GPS_Module_Attached())
        {
            #if (GPS_RAW_FILE_ENABLE == 1)
            UINT8 bgps_en;

            if (AHC_Menu_SettingGetCB((char*)COMMON_KEY_GPS_RECORD_ENABLE, &bgps_en) == AHC_TRUE) {
                switch (bgps_en) {
                case RECODE_GPS_OFF:
                case RECODE_GPS_IN_VIDEO:
                    // NOP
                    break;

                default:
                {
                    MMP_ULONG ulEmergentRecordingOffset = 0;
                    MMPS_3GPRECD_GetEmergentRecordingOffset(&ulEmergentRecordingOffset);
                    GPSCtrl_SetGPSRawFileName_Emer(bGPSRawFileName_Emer,STRLEN(bGPSRawFileName_Emer));
#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
                    GPSCtrl_SetGPSRawBufTime_Emer(ulEmergentRecordingOffset/1000);
#endif
                }
                    break;
                }
            }
            #endif
        }
        #endif

        #if(GSENSOR_RAW_FILE_ENABLE == 1 && GPS_CONNECT_ENABLE == 0)
        {
            MMP_ULONG ulEmergentRecordingOffset = 0;
            MMPS_3GPRECD_GetEmergentRecordingOffset(&ulEmergentRecordingOffset);
            GPSCtrl_SetGPSRawFileName_Emer(bGPSRawFileName_Emer,STRLEN(bGPSRawFileName_Emer));
            #if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
            GPSCtrl_SetGPSRawBufTime_Emer(ulEmergentRecordingOffset/1000);
            #endif
        }
        #endif

        /*
        SD:\\HD00000.3gp
        */
        AHC_UF_PreAddFile(m_uwEmerVRDirKey,(INT8*)m_chEmerVRFileName);
        AHC_SetVideoFileName(m_chEmerVRFullName, STRLEN(m_chEmerVRFullName), MMP_3GPRECD_FILETYPE_EMERGENCY);
        
#if (UVC_HOST_VIDEO_ENABLE == 1 && VIDRECD_MULTI_TRACK == 0)
        if(AHC_TRUE == AHC_HostUVCVideoIsEnabled()){
            AHC_SetVideoFileName(m_EmrVRRearFullName, STRLEN(m_EmrVRRearFullName), MMP_3GPRECD_FILETYPE_UVCEMERG);
        }
#elif (SUPPORT_DUAL_SNR == 1 && DUALENC_SUPPORT == 1 && VIDRECD_MULTI_TRACK == 0)      
        if(AHC_TRUE == AHC_GetSecondSensorCnntStatus()){
            AHC_SetVideoFileName(m_EmrVRRearFullName, STRLEN(m_EmrVRRearFullName), MMP_3GPRECD_FILETYPE_DUALENC);
        }
#endif
        m_uiEmerRecordInterval = EMER_RECORD_DUAL_WRITE_INTERVAL;
        AHC_SetEmerRecordInterval(m_uiEmerRecordInterval);


#if (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE)
        err = MMPS_3GPRECD_StartEmergentRecd(AHC_TRUE);
#else //EMERGENCY_RECODE_DUAL_FILE
        err = MMPS_3GPRECD_StartEmergentRecd(AHC_FALSE);
#endif

		if(err != MMP_ERR_NONE)
		{
			m_bStartEmerVR = AHC_FALSE;
			#if (GPS_RAW_FILE_EMER_EN == 1)
            GPSCtrl_SetGPSRawStart_Emer(AHC_FALSE);
            #endif
			return AHC_FALSE;
		}
        m_bStartEmerVR = AHC_TRUE;
        //AHC_UF_PreAddFile(m_uwEmerVRDirKey,(INT8*)m_chEmerVRFileName);
        m_bEmerVRPostDone = AHC_FALSE;
    }

    AHC_UF_SelectDB(sCurType);

    #endif
    return AHC_TRUE;
}

AHC_BOOL AHC_GetEmerRecordTime(UINT32 *uiTime)
{
    #if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
    MMPS_3GPRECD_GetEmergentRecordingTime(uiTime);
    #endif
    return AHC_TRUE;
}

AHC_BOOL AHC_GetEmerRecordTimeOffset(UINT32 *uiTime)
{
    #if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
    MMPS_3GPRECD_GetEmergentRecordingOffset(uiTime);
    //*uiTime = 0;
    #endif
    return AHC_TRUE;
}
AHC_BOOL AHC_SetEmerRecordInterval(UINT32 uiInterval)
{
    #if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
    m_uiEmerRecordInterval = uiInterval;

    if (uiInterval < ((0x7FFFFFFF - EM_VR_TIME_LIMIT_OFFSET) / 1000))
        uiInterval = uiInterval * 1000 + EM_VR_TIME_LIMIT_OFFSET;

    MMPS_3GPRECD_SetEmergentFileTimeLimit(uiInterval);
    printc("Max Interval : %d ms\n", uiInterval);
    #endif
    return AHC_TRUE;
}

UINT32 AHC_GetEmerRecordInterval(void)
{
    #if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
    return m_uiEmerRecordInterval;
    #else
    return 0;
    #endif
}

AHC_BOOL AHC_EmerRecordPostProcess(void)
{

    #if (GPS_RAW_FILE_EMER_EN == 1)

    GPSCtrl_GPSRawWriteFlushBuf_Emer();
    //GPSCtrl_SetGPSRawStart_Emer(AHC_FALSE);
    #endif
    #if (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE && GPS_RAW_FILE_ENABLE == 1)
    if(GPSCtrl_GPSRawWriteFirst_Normal())
    GPSCtrl_GPSRawResetBuffer();
    #endif

#if (GPS_CONNECT_ENABLE == 1)
    if(AHC_GPS_Module_Attached())
    {
        #if (GPS_RAW_FILE_EMER_EN == 1)
        UINT8 bgps_en;
        if (AHC_Menu_SettingGetCB((char*)COMMON_KEY_GPS_RECORD_ENABLE, &bgps_en) == AHC_TRUE) {
            switch (bgps_en) {
            case RECODE_GPS_OFF:
            case RECODE_GPS_IN_VIDEO:
                // NOP
                break;

            default:
            	if(!AHC_IsEmerRecordStarted())
                GPSCtrl_GPSRawClose_Emer();
                break;
            }
       }
        #endif
    }
#endif

    #if(GSENSOR_RAW_FILE_ENABLE == 1 && GPS_CONNECT_ENABLE == 0 && GPS_RAW_FILE_EMER_EN == 1)
    if(!AHC_IsEmerRecordStarted())
        GPSCtrl_GPSRawClose_Emer();
    #endif
    #if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
    if(m_bEmerVRPostDone == AHC_FALSE){
        DCF_DB_TYPE sType;

        printc("Post Emer Done\n");
        sType = AHC_UF_GetDB();
        AHC_UF_SelectDB(DCF_DB_TYPE_3RD_DB);

        AHC_UF_PostAddFile(m_uwEmerVRDirKey,(INT8*)m_chEmerVRFileName);
        AHC_UF_SelectDB(sType);
        m_bEmerVRPostDone = AHC_TRUE;

#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
{
        AHC_RTC_TIME    RtcTime;
        int   nSecondOffset;
        MMP_ULONG ulEmergentRecordingTime;

        AHC_RTC_GetTime(&RtcTime);
        if((m_EmerRecStartRtcTime.uwYear <= RTC_DEFAULT_YEAR) && (RtcTime.uwYear > RTC_DEFAULT_YEAR)) {
            ulEmergentRecordingTime = AHC_GetEmerRecordInterval();
            printc("ulEmergentRecordingTime=%d\r\n", ulEmergentRecordingTime);

            nSecondOffset = -1*ulEmergentRecordingTime;
            AHC_DT_ShiftRtc( &RtcTime, nSecondOffset);

            printc("AHC_EmerRecordPostProcess:AHC_UF_Rename::Old:%d-%d-%d:%d-%d-%d New:%d-%d-%d:%d-%d-%d \r\n",
            m_EmerRecStartRtcTime.uwYear,m_EmerRecStartRtcTime.uwMonth,m_EmerRecStartRtcTime.uwDay,
            m_EmerRecStartRtcTime.uwHour,m_EmerRecStartRtcTime.uwMinute,m_EmerRecStartRtcTime.uwSecond,
            RtcTime.uwYear,RtcTime.uwMonth,RtcTime.uwDay,RtcTime.uwHour,RtcTime.uwMinute,RtcTime.uwSecond);

            AHC_UF_Rename(AHC_UF_GetDB(), &m_EmerRecStartRtcTime, &RtcTime);
        }
}
#endif
    }
    #endif

     #if (GPS_RAW_FILE_EMER_EN == 1)
    GPSCtrl_SetGPSRawStart_Emer(AHC_FALSE);
    GPSCtrl_SetGPSRawWriteFirst(AHC_TRUE);
    #endif

    if(AHC_PostEmergencyDone != NULL )
    {
	    AHC_PostEmergencyDone();
    }

    return AHC_TRUE;
}

AHC_BOOL AHC_EmerRecordPostProcess_MediaError(void)
{

    #if (GPS_RAW_FILE_EMER_EN == 1)

    #if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
    m_bStartEmerVR = AHC_FALSE;
    #endif

    GPSCtrl_GPSRawWriteFlushBuf_Emer();
    //GPSCtrl_SetGPSRawStart_Emer(AHC_FALSE);
    #endif
    #if (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE && GPS_RAW_FILE_ENABLE == 1)
    if(GPSCtrl_GPSRawWriteFirst_Normal())
    GPSCtrl_GPSRawResetBuffer();
    #endif

    #if (GPS_CONNECT_ENABLE == 1)
    if(AHC_GPS_Module_Attached())
    {
        #if (GPS_RAW_FILE_EMER_EN == 1)
            UINT8 bgps_en;
            if (AHC_Menu_SettingGetCB((char*)COMMON_KEY_GPS_RECORD_ENABLE, &bgps_en) == AHC_TRUE) {
                switch (bgps_en) {
                case RECODE_GPS_OFF:
                case RECODE_GPS_IN_VIDEO:
                    // NOP
                    break;

                default:
                    GPSCtrl_GPSRawClose_Emer();
                    GPSCtrl_SetGPSRawStart_Emer(AHC_FALSE);
                    break;
                }
            }
        #endif
    }
    #endif

    #if(GSENSOR_RAW_FILE_ENABLE == 1 && GPS_CONNECT_ENABLE == 0 && GPS_RAW_FILE_EMER_EN == 1)
        GPSCtrl_GPSRawClose_Emer();
    #endif
    #if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
    if(m_bEmerVRPostDone == AHC_FALSE){
        DCF_DB_TYPE sType;

        printc("Post Emer Done\n");
        sType = AHC_UF_GetDB();
        AHC_UF_SelectDB(DCF_DB_TYPE_3RD_DB);

        AHC_UF_PostAddFile(m_uwEmerVRDirKey,(INT8*)m_chEmerVRFileName);
        AHC_UF_SelectDB(sType);
        m_bEmerVRPostDone = AHC_TRUE;
        m_bStartEmerVR = AHC_FALSE;

#if((EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_DUAL_FILE)||(EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE))
        AHC_SetKernalEmergencyStopStep(AHC_KERNAL_EMERGENCY_AHC_DONE);
#endif

    }
    #endif

    return AHC_TRUE;
}


#if (EMERGENTRECD_SUPPORT) && (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_MOVE_FILE)
void AHC_SetNormal2Emergency(AHC_BOOL bState)
{
    m_bNormal2Emergency = bState;
}
AHC_BOOL AHC_GetNormal2Emergency(void)
{
    return m_bNormal2Emergency;
}
#endif
AHC_BOOL AHC_IsVRSeamless(void)
{
    return m_bSeamlessRecord;
}

AHC_BOOL AHC_StopEmerRecord(void)
{
#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
    MMP_ULONG ulEmergentRecordingTime;

    MMPS_3GPRECD_GetEmergentRecordingTime(&ulEmergentRecordingTime);
#endif

    #if (EMERGENTRECD_SUPPORT)
    MMPS_3GPRECD_StopEmergentRecd(AHC_FALSE);
    #endif

    #if(EMER_RECORD_DUAL_WRITE_ENABLE == 1)
    m_bStartEmerVR = AHC_FALSE;
    #endif

#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
{
    AHC_RTC_TIME    RtcTime;
    int   nSecondOffset;

    AHC_RTC_GetTime(&RtcTime);
    if((m_EmerRecStartRtcTime.uwYear <= RTC_DEFAULT_YEAR) && (RtcTime.uwYear > RTC_DEFAULT_YEAR)) {
        nSecondOffset = -1*(ulEmergentRecordingTime/1000);
        AHC_DT_ShiftRtc( &RtcTime, nSecondOffset);

        printc("AHC_StopEmerRecord:AHC_UF_Rename::Old:%d-%d-%d:%d-%d-%d New:%d-%d-%d:%d-%d-%d \r\n",
        m_EmerRecStartRtcTime.uwYear,m_EmerRecStartRtcTime.uwMonth,m_EmerRecStartRtcTime.uwDay,
        m_EmerRecStartRtcTime.uwHour,m_EmerRecStartRtcTime.uwMinute,m_EmerRecStartRtcTime.uwSecond,
        RtcTime.uwYear,RtcTime.uwMonth,RtcTime.uwDay,RtcTime.uwHour,RtcTime.uwMinute,RtcTime.uwSecond);

        AHC_UF_Rename(AHC_UF_GetDB(), &m_EmerRecStartRtcTime, &RtcTime);
    }
}
#endif

#if 0
    if(AHC_GetVidRecdCardSlowStop()) {
        AHC_RestartVRMode(AHC_TRUE);
        AHC_SetVidRecdCardSlowStop(AHC_FALSE);
    }

    if(AHC_GetAPStopVideoRecord()) {
        AHC_RestartVRMode(AHC_TRUE);
        AHC_SetAPStopVideoRecord(AHC_FALSE);
    }
#endif

    return AHC_TRUE;
}

AHC_BOOL AHC_VRClosePreviousFile(void)
{

    AHC_UF_PostAddFile(m_usVideoRDirKey,(INT8*)m_VideoRFileName);
    
	if(AHC_UF_GetDB() == DCF_DB_TYPE_1ST_DB && AHC_NeedDeleteNormalAfterEmr())
		AHC_DeleteLatestNormalFile();
                           
#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
{
    AHC_RTC_TIME    RtcTime;
    int   nSecondOffset;

    AHC_RTC_GetTime(&RtcTime);
    if((m_VideoRecStartRtcTime.uwYear <= RTC_DEFAULT_YEAR) && (RtcTime.uwYear > RTC_DEFAULT_YEAR)) {
        nSecondOffset = -1*AHC_GetVRTimeLimit();
        AHC_DT_ShiftRtc( &RtcTime, nSecondOffset);

        printc("AHC_RestartVRMode:AHC_UF_Rename::Old:%d-%d-%d:%d-%d-%d New:%d-%d-%d:%d-%d-%d \r\n",
        m_VideoRecStartRtcTime.uwYear,m_VideoRecStartRtcTime.uwMonth,m_VideoRecStartRtcTime.uwDay,
        m_VideoRecStartRtcTime.uwHour,m_VideoRecStartRtcTime.uwMinute,m_VideoRecStartRtcTime.uwSecond,
        RtcTime.uwYear,RtcTime.uwMonth,RtcTime.uwDay,RtcTime.uwHour,RtcTime.uwMinute,RtcTime.uwSecond);

        AHC_UF_Rename(AHC_UF_GetDB(), &m_VideoRecStartRtcTime, &RtcTime);
        m_VideoRecStartRtcTime = RtcTime;
    }
}
#endif
#if (EMERGENTRECD_SUPPORT) && (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_MOVE_FILE)
    if(AHC_GetNormal2Emergency()) {
        #if 0
        UINT32          CurrentObjIdx;
        #else
        UINT8           bCreateNewDir;
        UINT32          Param;
        MMPS_3GPRECD_CONTAINER      ContainerType;
        #endif
        UINT16          DirKey;

        char            FilePathName[MAX_FILE_NAME_SIZE];
        INT8            DirName[32];
        INT8            FileName[32];

        AHC_SetNormal2Emergency(AHC_FALSE);
        LedCtrl_ForceCloseTimer(AHC_FALSE);

#if 0
        AHC_UF_GetCurrentIndex(&CurrentObjIdx);
        MEMSET(FilePathName, 0, sizeof(FilePathName));
        AHC_UF_GetFilePathNamebyIndex(CurrentObjIdx, FilePathName);
        AHC_UF_GetDirKeybyIndex(CurrentObjIdx, &DirKey);
#else
        MEMSET(FilePathName, 0, sizeof(FilePathName));
        MEMSET(FileName, 0, sizeof(FileName));
        AHC_UF_GetName2(&m_VideoRecStartRtcTime, FilePathName, FileName, &bCreateNewDir);
        AIHC_GetMovieConfig(AHC_CLIP_CONTAINER_TYPE, &Param);
        ContainerType   = Param;

        STRCAT(FilePathName, EXT_DOT);
        if(ContainerType == MMPS_3GPRECD_CONTAINER_3GP) {
            STRCAT(FilePathName, MOVIE_3GP_EXT);
        }
        else {
            STRCAT(FilePathName, MOVIE_AVI_EXT);
        }
#endif

        printc("Normal2Emer File: %s \n", FilePathName);

        MEMSET(DirName, 0, sizeof(DirName));
        MEMSET(FileName, 0, sizeof(FileName));

        MEMCPY(DirName, FilePathName, 10);       //SD:\Normal
        MEMCPY(FileName, FilePathName + 11, 21);

        AHC_UF_MoveFile(DCF_DB_TYPE_1ST_DB, DCF_DB_TYPE_3RD_DB, DirKey, FileName, AHC_TRUE);
    }
#endif
    return AHC_TRUE;
}

AHC_BOOL AHC_VRDelPreviousFile(void)
{
#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
    MMPS_3GPRECD_CONTAINER      ContainerType;
    UINT32                      Param;
    UINT8   bCreateNewDir;
    INT8    DirName[32];
    INT8    FileName[32];

    #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
    if(uiGetParkingModeEnable())
        AHC_UF_GetName2(&m_VideoRecStartRtcTime, (INT8*)m_CurVRFullName, (INT8*)m_VideoParkFileName, &bCreateNewDir);
    else
    #endif
        AHC_UF_GetName2(&m_VideoRecStartRtcTime, (INT8*)m_CurVRFullName, (INT8*)m_VideoRFileName, &bCreateNewDir);

    MEMSET(DirName, 0, sizeof(DirName));
    MEMSET(FileName, 0, sizeof(FileName));

    AIHC_GetMovieConfig(AHC_CLIP_CONTAINER_TYPE, &Param);
    ContainerType   = Param;

    #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
    if(uiGetParkingModeEnable())
    {
        if(ContainerType == MMPS_3GPRECD_CONTAINER_3GP) {
            STRCAT((INT8*)m_VideoParkFileName, EXT_DOT);
            STRCAT((INT8*)m_VideoParkFileName, MOVIE_3GP_EXT);
        }
        else {
            STRCAT((INT8*)m_VideoParkFileName, EXT_DOT);
            STRCAT((INT8*)m_VideoParkFileName, MOVIE_AVI_EXT);
        }
    }
    else
    #endif
    {
        if(ContainerType == MMPS_3GPRECD_CONTAINER_3GP) {
            STRCAT((INT8*)m_VideoRFileName, EXT_DOT);
            STRCAT((INT8*)m_VideoRFileName, MOVIE_3GP_EXT);
        }
        else {
            STRCAT((INT8*)m_VideoRFileName, EXT_DOT);
            STRCAT((INT8*)m_VideoRFileName, MOVIE_AVI_EXT);
        }
    }

    GetPathDirStr(DirName,sizeof(DirName),m_CurVRFullName);

    switch(AHC_UF_GetDB()) {
    case DCF_DB_TYPE_1ST_DB:
        memcpy(FileName, m_VideoRFileName, sizeof(FileName) - 1);
    break;

    case DCF_DB_TYPE_2ND_DB:
        #if(SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
        memcpy(FileName, m_VideoParkFileName, sizeof(FileName) - 1);
        #else
        memcpy(FileName, m_VideoRFileName, sizeof(FileName) - 1);
        #endif
    break;

    case DCF_DB_TYPE_3RD_DB:
        memcpy(FileName, m_VideoRFileName, sizeof(FileName) - 1);
    break;
    }

    printc("AHC_VRDelPreviousFile\r\n");
    printc("DirName: %s \n", DirName);
    printc("FileName: %s \n", FileName);

    #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
    if (uiGetParkingModeEnable())
        printc("FileName2: %s \n", m_VideoParkFileName);
    else
    #endif
        printc("FileName2: %s \n", m_VideoRFileName);

    AHC_UF_FileOperation((UINT8*)DirName,(UINT8*)FileName, DCF_FILE_DELETE, NULL,NULL);

#endif
    return AHC_TRUE;
}


AHC_BOOL AHC_RestartVRMode(AHC_BOOL b_delete)
{
    MMPS_3GPRECD_CONTAINER      ContainerType;
    MMP_BYTE                    DirName[16];
#if (GPS_CONNECT_ENABLE && GPS_USE_FILE_AS_DATABUF)
    MMP_BYTE                    bGpsInfoFileName[MAX_FILE_NAME_SIZE];
    MMP_ULONG                   ulGpsInfoDirID;
#endif
    #if (GPS_KML_FILE_ENABLE == 1)
    MMP_BYTE                    bKMLFileName[MAX_FILE_NAME_SIZE];
    #endif
    #if (GPS_RAW_FILE_ENABLE == 1)
    MMP_BYTE                    bGPSRawFileName[MAX_FILE_NAME_SIZE];
    #endif
#if ((GSENSOR_RAW_FILE_ENABLE == 1) && (GPS_CONNECT_ENABLE == 0))
    MMP_BYTE                    bGPSRawFileName[MAX_FILE_NAME_SIZE];
#endif
    UINT32                      Param;
    UINT8                       bCreateNewDir;
    UINT32                      TimeStampOp;
    UINT32                      TimeLimit = 0;
    MMP_ERR                     sRet = MMP_ERR_NONE;
    AHC_BOOL                    bRet;

    /**
    @brief  close the previous file.

    */
    AHC_VRClosePreviousFile();

#ifdef CFG_CUS_VIDEO_PROTECT_PROC
    CFG_CUS_VIDEO_PROTECT_PROC();
#else
    if(AHC_Protect_GetType() != AHC_PROTECT_NONE)
    {
        AHC_Protect_SetVRFile(AHC_PROTECTION_CUR_FILE, AHC_Protect_GetType());

        #if (PROTECTION_FOR_C003_P000)
        if(AHC_Protect_GetType() & AHC_PROTECT_NEXT){
            AHC_Protect_SetType(AHC_PROTECT_NEXT);
        }
        else
        #endif
        {
            AHC_Protect_SetType(AHC_PROTECT_NONE);
        }
    }
#endif

    #if (FS_FORMAT_FREE_ENABLE)
    b_delete = AHC_FALSE;
    #endif
    
    if(b_delete)
    {
        #if(SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
        if(uiGetParkingModeEnable() == AHC_FALSE){
            if(AHC_Deletion_Romove(AHC_FALSE)==AHC_FALSE)
            {
            printc("AHC_Deletion_Romove Error\r\n");
            return AHC_FALSE;
            }
        }
        else
        {
            //if(MenuSettingConfig()->uiStorageAllocation != STORAGE_ALLOCATION_TYPE2)
            {
	            #if C005_PARKING_CLIP_Length_10PRE_10REC
	            if(AHC_Deletion_RemoveEx(DCF_DB_TYPE_2ND_DB, PARK_RECORD_WRTIE_MAX_TIME) == AHC_FALSE)
	            #else
	            if(AHC_Deletion_RemoveEx(DCF_DB_TYPE_2ND_DB, AHC_GetVRTimeLimit()) == AHC_FALSE)
	            #endif
	            {
	                #if 0 //Skip Space Hint
	                uiSetParkingSpaceCheckEnable(AHC_TRUE);
	                StateSwitchMode(UI_PARKING_SPACE_HINT);
	                return AHC_TRUE;
	                #else
	                    #if C005_PARKING_CLIP_Length_10PRE_10REC
	                    if(AHC_Deletion_RemoveEx(DCF_DB_TYPE_2ND_DB, PARK_RECORD_WRTIE_MAX_TIME) == AHC_FALSE)
	                    #else
	                    if(AHC_Deletion_RemoveEx(DCF_DB_TYPE_2ND_DB, AHC_GetVRTimeLimit()) == AHC_FALSE)
	                    #endif
	                    return AHC_FALSE;
	                #endif
	            }
            }
            #if 0
            else
            {
            	if(AHC_Deletion_Romove(AHC_FALSE)==AHC_FALSE)
			    {
			     	return VIDEO_REC_PARKING_SPACE;
		    	}
            }
            #endif
        }

        #else
        if(AHC_Deletion_Romove(AHC_FALSE)==AHC_FALSE)
        {
            printc(FG_RED("AHC_Deletion_Romove Error\r\n"));
            return AHC_FALSE;
        }
        #endif
    }

    AHC_GetCaptureConfig(ACC_DATE_STAMP, &TimeStampOp);

    if (TimeStampOp & AHC_ACC_TIMESTAMP_ENABLE_MASK)
    {
        UINT32 movieSize = 0;

        AHC_Menu_SettingGetCB((char *) COMMON_KEY_MOVIE_SIZE, &movieSize);

        {
            AHC_RTC_TIME sRtcTime;

            AHC_RTC_GetTime(&sRtcTime);
            AIHC_ConfigVRTimeStamp(TimeStampOp, &sRtcTime, MMP_TRUE);
        }
    }

    #if (C005_PARKING_CLIP_Length_10PRE_10REC == 1) && (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
    if(uiGetParkingModeEnable())
    {
        #if (GPS_RAW_FILE_EMER_EN == 1)
        GPSCtrl_SetGPSRawWriteFirst(AHC_FALSE);
        #endif
    }
    #endif

    AIHC_GetMovieConfig(AHC_CLIP_CONTAINER_TYPE, &Param);
    ContainerType   = Param;

    MEMSET(m_CurVRFullName , 0, sizeof(m_CurVRFullName));
#if ((UVC_HOST_VIDEO_ENABLE == 1  || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)
    MEMSET(m_CurVRRearFullName, 0, sizeof(m_CurVRRearFullName));
#endif
    MEMSET(DirName         , 0, sizeof(DirName));

    #if(SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
    if (uiGetParkingModeEnable())
        MEMSET(m_VideoParkFileName, 0, sizeof(m_VideoParkFileName));
    else
    #endif
        MEMSET(m_VideoRFileName, 0, sizeof(m_VideoRFileName));

    #if 0 //Rogers:has issue.(EVENT_VIDEO_TOUCH_STOP_EMERGENT) AHC_EmerRecordPostProcess-> AHC_RestartVRMode(AHC_TRUE)-> AHC_EmerRecordPostProcess
    #if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
    if(m_bStartEmerVR == AHC_FALSE){
        AHC_EmerRecordPostProcess();
    }
    #endif
    #endif

#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_NORMAL)
    #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
    if(uiGetParkingModeEnable())
    {
        bRet = AHC_UF_GetName(&m_usVideoParkDirKey, (INT8*)DirName, (INT8*)m_VideoParkFileName, &bCreateNewDir);
        if (bRet  == AHC_TRUE) {

            STRCPY(m_CurVRFullName,(INT8*)AHC_UF_GetRootDirName());
            STRCAT(m_CurVRFullName,"\\");
            STRCAT(m_CurVRFullName,DirName);

            if ( bCreateNewDir ) {
                MMPS_FS_DirCreate((INT8*)m_CurVRFullName, STRLEN(m_CurVRFullName));
                AHC_UF_AddDir(DirName);
            }

            STRCAT(m_CurVRFullName,"\\");
            STRCAT(m_CurVRFullName,(INT8*)m_VideoParkFileName);

        }
    }
    else
    #endif
    {
        bRet = AHC_UF_GetName(&m_usVideoRDirKey, (INT8*)DirName, (INT8*)m_VideoRFileName, &bCreateNewDir);
        if (bRet  == AHC_TRUE) {

            STRCPY(m_CurVRFullName,(INT8*)AHC_UF_GetRootDirName());
            STRCAT(m_CurVRFullName,"\\");
            STRCAT(m_CurVRFullName,DirName);

            if ( bCreateNewDir ) {
                MMPS_FS_DirCreate((INT8*)m_CurVRFullName, STRLEN(m_CurVRFullName));
                AHC_UF_AddDir(DirName);
            }

            STRCAT(m_CurVRFullName,"\\");
            STRCAT(m_CurVRFullName,(INT8*)m_VideoRFileName);

        }
    }
#elif (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
    {
        MMP_ULONG ulRecordingOffset = 0;
        int   nSecondOffset;

        AHC_RTC_GetTime(&m_VideoRecStartRtcTime);
        MMPS_3GPRECD_Get3gpRecordingOffset (&ulRecordingOffset);
        nSecondOffset = -1*(ulRecordingOffset/1000);
        AHC_DT_ShiftRtc( &m_VideoRecStartRtcTime, nSecondOffset);
        #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
        if(uiGetParkingModeEnable()) {
            bRet = AHC_UF_GetName2(&m_VideoRecStartRtcTime, (INT8*)m_CurVRFullName, (INT8*)m_VideoParkFileName, &bCreateNewDir);
        }
        else
        #endif
        {
            bRet = AHC_UF_GetName2(&m_VideoRecStartRtcTime, (INT8*)m_CurVRFullName, (INT8*)m_VideoRFileName, &bCreateNewDir);
        }
#if ((UVC_HOST_VIDEO_ENABLE == 1  || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)
		#if (UVC_HOST_VIDEO_ENABLE == 1)
		if(AHC_TRUE == AHC_HostUVCVideoIsEnabled())//bit1603 must be connect
		#else
		if(AHC_TRUE == AHC_GetSecondSensorCnntStatus())//bit1603 must be connect
		#endif
        {
            AHC_UF_SetRearCamPathFlag(AHC_TRUE);
            AHC_UF_GetName2(&m_VideoRecStartRtcTime, (INT8*)m_CurVRRearFullName, (INT8*)m_RearVideoRFileName, &bCreateNewDir);
            printc("AHC_RestartVRMode:bRearFileName %s %d\r\n",m_CurVRRearFullName,__LINE__);
            AHC_UF_SetRearCamPathFlag(AHC_FALSE);
        }
#endif
    }
#endif

    if(bRet == AHC_TRUE) {

        STRCAT(m_CurVRFullName, EXT_DOT);
#if ((UVC_HOST_VIDEO_ENABLE == 1  || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)
        STRCAT(m_CurVRRearFullName, EXT_DOT);
#endif
        #if (GPS_CONNECT_ENABLE)
        if(AHC_GPS_Module_Attached())
        {
            #if (GPS_KML_FILE_ENABLE == 1)
            MEMSET(bKMLFileName, 0, sizeof(bKMLFileName));
            STRCPY(bKMLFileName, m_CurVRFullName);
            STRCAT(bKMLFileName, GPS_KML_FILE_EXTENTION);
            #endif
            #if (GPS_RAW_FILE_ENABLE == 1)
            MEMSET(bGPSRawFileName, 0, sizeof(bGPSRawFileName));
            STRCPY(bGPSRawFileName, m_CurVRFullName);
            STRCAT(bGPSRawFileName, GPS_RAW_FILE_EXTENTION);
            #endif
        }
        #endif

        #if(GSENSOR_RAW_FILE_ENABLE == 1 && GPS_CONNECT_ENABLE == 0)
            MEMSET(bGPSRawFileName, 0, sizeof(bGPSRawFileName));
            STRCPY(bGPSRawFileName, m_CurVRFullName);
            STRCAT(bGPSRawFileName, GPS_RAW_FILE_EXTENTION);
        #endif

        #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
        if(uiGetParkingModeEnable())
        {
            if(ContainerType == MMPS_3GPRECD_CONTAINER_3GP) {
                STRCAT(m_CurVRFullName, MOVIE_3GP_EXT);
#if ((UVC_HOST_VIDEO_ENABLE == 1  || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)
                STRCAT(m_CurVRRearFullName, MOVIE_3GP_EXT);
#endif
                STRCAT((INT8*)m_VideoParkFileName, EXT_DOT);
                STRCAT((INT8*)m_VideoParkFileName, MOVIE_3GP_EXT);
            }
            else {
                STRCAT(m_CurVRFullName, MOVIE_AVI_EXT);
#if ((UVC_HOST_VIDEO_ENABLE == 1  || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)
                STRCAT(m_CurVRRearFullName, MOVIE_AVI_EXT);
#endif
                STRCAT((INT8*)m_VideoParkFileName, EXT_DOT);
                STRCAT((INT8*)m_VideoParkFileName, MOVIE_AVI_EXT);
            }
        }
        else
        #endif
        {
            if(ContainerType == MMPS_3GPRECD_CONTAINER_3GP) {
                STRCAT(m_CurVRFullName, MOVIE_3GP_EXT);
#if ((UVC_HOST_VIDEO_ENABLE == 1  || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)                
                STRCAT(m_CurVRRearFullName, MOVIE_3GP_EXT);
#endif
                STRCAT((INT8*)m_VideoRFileName, EXT_DOT);
                STRCAT((INT8*)m_VideoRFileName, MOVIE_3GP_EXT);
            }
            else {
                STRCAT(m_CurVRFullName, MOVIE_AVI_EXT);
#if ((UVC_HOST_VIDEO_ENABLE == 1  || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)
                STRCAT(m_CurVRRearFullName, MOVIE_AVI_EXT);
#endif
                STRCAT((INT8*)m_VideoRFileName, EXT_DOT);
                STRCAT((INT8*)m_VideoRFileName, MOVIE_AVI_EXT);
            }
        }

        AHC_SetVideoFileName(m_CurVRFullName, STRLEN(m_CurVRFullName), MMP_3GPRECD_FILETYPE_VIDRECD);

#if (UVC_HOST_VIDEO_ENABLE == 1 && VIDRECD_MULTI_TRACK == 0) 
        if(AHC_TRUE == AHC_HostUVCVideoIsEnabled()){   
            MMP_3GPRECD_FILETYPE sFileType = MMP_3GPRECD_FILETYPE_UVCRECD;
#if (DUALENC_SUPPORT == 1) 
                sFileType = MMP_3GPRECD_FILETYPE_DUALENC;
#endif           
            sRet = AHC_SetVideoFileName(m_CurVRRearFullName, STRLEN(m_CurVRRearFullName), sFileType);
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}         
        }
#elif (SUPPORT_DUAL_SNR == 1 && DUALENC_SUPPORT == 1 && VIDRECD_MULTI_TRACK == 0)
		if(AHC_TRUE == AHC_GetSecondSensorCnntStatus())//bit1603 must be connect
		{
        	sRet = AHC_SetVideoFileName(m_CurVRRearFullName, STRLEN(m_CurVRRearFullName), MMP_3GPRECD_FILETYPE_DUALENC);
        	if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
		}
#endif

        #if (GPS_CONNECT_ENABLE == 1)
        if(AHC_GPS_Module_Attached())
        {
            #if (GPS_KML_FILE_ENABLE == 1)
            GPSCtrl_SetKMLFileName(bKMLFileName,STRLEN(bKMLFileName));
            #endif

            #if (GPS_RAW_FILE_ENABLE == 1)
            {
                UINT8 bgps_en;

                if (AHC_Menu_SettingGetCB((char*)COMMON_KEY_GPS_RECORD_ENABLE, &bgps_en) == AHC_TRUE) {
                    switch (bgps_en) {
                    case RECODE_GPS_OFF:
                    case RECODE_GPS_IN_VIDEO:
                        // NOP
                        break;

                    default:
                        #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
                        #if ((EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE) || C005_PARKING_CLIP_Length_10PRE_10REC == 1)
                            #if(C005_PARKING_CLIP_Length_10PRE_10REC == 0)
                            MMP_ULONG ulRecordingOffset = 0;
                            MMPS_3GPRECD_Get3gpRecordingOffset(&ulRecordingOffset);
                                #if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
                                GPSCtrl_SetGPSRawBufTime_Normal(ulRecordingOffset/1000);
                                #endif
                            #else // #if(C005_PARKING_CLIP_Length_10PRE_10REC == 0)
                            if(uiGetParkingModeEnable())
                            {
                                MMP_ULONG ulRecordingOffset = 0;
                                MMPS_3GPRECD_Get3gpRecordingOffset(&ulRecordingOffset);
                                #if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
                                GPSCtrl_SetGPSRawBufTime_Normal(ulRecordingOffset/1000);
                                #endif
                            }
                            #endif
                        #endif
                        #endif

                        GPSCtrl_SetGPSRawFileName(bGPSRawFileName,STRLEN(bGPSRawFileName));
                        break;
                    }
                }
            }
            #endif
        }
        #endif

        #if(GSENSOR_RAW_FILE_ENABLE == 1 && GPS_CONNECT_ENABLE == 0)
        {
            #if (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE || C005_PARKING_CLIP_Length_10PRE_10REC == 1 )
                #if (C005_PARKING_CLIP_Length_10PRE_10REC == 0)
                MMP_ULONG ulRecordingOffset = 0;
                MMPS_3GPRECD_Get3gpRecordingOffset(&ulRecordingOffset);
                #if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
                GPSCtrl_SetGPSRawBufTime_Normal(ulRecordingOffset/1000);
                #endif
                #else// #if (C005_PARKING_CLIP_Length_10PRE_10REC == 0)
                #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
                if(uiGetParkingModeEnable())
                {
                    MMP_ULONG ulRecordingOffset = 0;
                    MMPS_3GPRECD_Get3gpRecordingOffset(&ulRecordingOffset);
                    #if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
                    GPSCtrl_SetGPSRawBufTime_Normal(ulRecordingOffset/1000);
                    #endif
                }
                #endif
                #endif
            #endif
            GPSCtrl_SetGPSRawFileName(bGPSRawFileName,STRLEN(bGPSRawFileName));
        }
        #endif
        #if ((EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE || C005_PARKING_CLIP_Length_10PRE_10REC == 1) && GPS_RAW_FILE_ENABLE == 1)
            #if (C005_PARKING_CLIP_Length_10PRE_10REC == 0)
            GPSCtrl_GPSRawWriteFirst_Normal();
            GPSCtrl_GPSRawResetBuffer();
            #else
            #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
            if(uiGetParkingModeEnable())
            {
                GPSCtrl_GPSRawWriteFirst_Normal();
                GPSCtrl_GPSRawResetBuffer();
            }
            #endif
            #endif
        #endif

        RTNA_DBG_Str(0,m_CurVRFullName);
        RTNA_DBG_Str(0,"\r\n");

        #if (USE_INFO_FILE)
        STRCPY(m_InfoLog.DCFCurVideoFileName, m_CurVRFullName);
        #endif

#if (VR_SLOW_CARD_DETECT)
        MMPS_3GPRECD_SetVidRecdSkipModeParam(VRCB_TOTAL_SKIP_FRAME, VRCB_CONTINUOUS_SKIP_FRAME);
#endif

        #if (VR_SEAMLESS)
#if (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_DUAL_FILE)
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_SLOWCARDSTOPVIDRECD,  (void*)VRVidRecdCardSlowStopCB);
#endif
#if (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE)
        //MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_APSTOPVIDRECD,      (void*)VRAPStopVideoRecordCB);
#endif
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_MEDIA_FULL,   (void*)VRMediaFullCB);
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_FILE_FULL,    (void*)VRFileFullCB);
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_LONG_TIME_FILE_FULL, (void*)VRLongTimeFileFullCB);
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_MEDIA_ERROR,  (void*)VRMediaErrorCB);
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_MEDIA_SLOW,   (void*)VRMediaSlowCB);

#if (VR_SLOW_CARD_DETECT)
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_PREGETTIME_CARDSLOW,  (void*)VRPreGetTimeWhenCardSlowCB);
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_RECDSTOP_CARDSLOW,      (void*)VRRecdStopWhenCardSlowCB);
#endif

        #if(SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
        if(!uiGetParkingModeEnable())
        #endif
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_SEAMLESS ,    (void*)VRSeamlessCB);

        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_ENCODE_START, (void*)VRStartCB);
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_ENCODE_STOP,  (void*)VRStopCB);
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_POSTPROCESS,  (void*)VRPostProcessCB);
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_EMERGFILE_FULL,   (void*)VREmerDoneCB);
#if (DUALENC_SUPPORT == 1)
        MMPS_3GPRECD_RegisterCallback(MMPS_3GPRECD_EVENT_DUALENC_FILE_FULL, (void *)DualEncRecordCbFileFullCB);
#endif
        #else
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_MEDIA_FULL,   (void*)VRMediaFullCB);
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_FILE_FULL,    (void*)VRFileFullCB);
        //MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_MEDIA_SLOW,  (void*)VRMediaSlowCB);
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_MEDIA_ERROR,  (void*)VRMediaErrorCB);
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_ENCODE_START, (void*)VRStartCB);
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_ENCODE_STOP,  (void*)VRStopCB);
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_POSTPROCESS,  (void*)VRPostProcessCB);
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_EMERGFILE_FULL,   (void*)VREmerDoneCB);
#if (DUALENC_SUPPORT == 1)
        MMPS_3GPRECD_RegisterCallback(MMPS_3GPRECD_EVENT_DUALENC_FILE_FULL, (void *)DualEncRecordCbFileFullCB);
#endif        
        #endif

        TimeLimit = AHC_GetVRTimeLimit();

        if (AHC_SetVRTimeLimit(TimeLimit) == 0)
        {
            MMPS_3GPRECD_SetFileSizeLimit(SIGNLE_FILE_SIZE_LIMIT_3_75G);

            if(TimeLimit == NON_CYCLING_TIME_LIMIT/*Non cycle record*/)
            {
                UINT64  ulStorageFreeSpace  = 0;

                VideoFunc_GetFreeSpace(&ulStorageFreeSpace);

                if(ulStorageFreeSpace < SIGNLE_FILE_SIZE_LIMIT_4G)
                {
                    printc("Free Space Not Enough to Enable Seamless-2\r\n");
                    MMPS_3GPRECD_StartSeamless(MMP_FALSE);
                    m_bSeamlessRecord = AHC_FALSE;
                }
                else
                {
                    MMPS_3GPRECD_StartSeamless(MMP_TRUE);
                    m_bSeamlessRecord = AHC_TRUE;
                }
            }else{
                #if(SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
                if(uiGetParkingModeEnable())
                {
                    MMPS_3GPRECD_StartSeamless(MMP_FALSE);
                    m_bSeamlessRecord = MMP_FALSE;
                }
                else
                #endif
                {
                    MMPS_3GPRECD_StartSeamless(MMP_TRUE);
                    m_bSeamlessRecord = AHC_TRUE;
                }
            }

#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_NORMAL)
			// patch for AIT rear cam slow card issue. Open file for UVC here.
			// Seamless is ok because of TX start by m_container.


            sRet = MMPS_3GPRECD_StartRecord();
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

            #if(SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
            if(uiGetParkingModeEnable())
                AHC_UF_PreAddFile(m_usVideoParkDirKey,(INT8*)m_VideoParkFileName);
            else
            #endif
                AHC_UF_PreAddFile(m_usVideoRDirKey,(INT8*)m_VideoRFileName);

#elif (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
            {
                AHC_BOOL bStatus = AHC_TRUE;

                //PreAddFile will delete the file has  same name.
                #if(SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
                if(uiGetParkingModeEnable())
                    bStatus = AHC_UF_PreAddFile(m_usVideoParkDirKey,(INT8*)m_VideoParkFileName);
                else
                #endif
                    bStatus = AHC_UF_PreAddFile(m_usVideoRDirKey,(INT8*)m_VideoRFileName);
				// patch for AIT rear cam slow card issue. Open file for UVC here.
				// Seamless is ok because of TX start by m_container.
                sRet = MMPS_3GPRECD_StartRecord();
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet);} 

                if((sRet != MMP_ERR_NONE) || (bStatus !=  AHC_TRUE)){
                    INT8            DirName[64];

                    AHC_PRINT_RET_ERROR(gbAhcDbgBrk,bStatus);
                    if( GetPathDirStr(DirName, sizeof(DirName), m_CurVRFullName) )
                    {
                        #if(SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
                        if(uiGetParkingModeEnable()) {
                            AHC_UF_FileOperation((UINT8*)DirName,(UINT8*)m_VideoParkFileName, DCF_FILE_DELETE, NULL,NULL);
                        }
                        else
                        #endif
                        {
                            AHC_UF_FileOperation((UINT8*)DirName,(UINT8*)m_VideoRFileName, DCF_FILE_DELETE, NULL,NULL);
                        }
                    }

                    return AHC_FALSE;
                }
            }
#endif


			#if (SUPPORT_DUAL_SNR == 1 && DUALENC_SUPPORT == 1)
			if(AHC_TRUE == AHC_GetSecondSensorCnntStatus())//bit1603 must be connect
			{
	            if(!AHC_VideoDualRecordStart()){
	                AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); 
	                return AHC_FALSE;
	            }
            }
			#endif
			#if (DUALENC_SUPPORT)
			sRet = MMPS_3GPRECD_StartAllRecord();
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet);} 
			#endif

            #if (FS_FORMAT_FREE_ENABLE == 0) //Do NOT support GPS / Gsensor temp file in Format Free.
            #if (GPS_CONNECT_ENABLE && GPS_USE_FILE_AS_DATABUF && (GPS_FUNC & FUNC_VIDEOSTREM_INFO))
            if (AHC_GPS_Module_Attached())
            {
                STRCPY(bGpsInfoFileName, (INT8*)AHC_UF_GetRootName());
                STRCAT(bGpsInfoFileName, GPS_TEMP_INFOFILE_DIR);

                if (AHC_FS_DirOpen(bGpsInfoFileName, sizeof(bGpsInfoFileName),&ulGpsInfoDirID) != AHC_ERR_NONE)
                {
                    AHC_FS_DirCreate(bGpsInfoFileName, sizeof(bGpsInfoFileName));
                    AHC_FS_FileDirSetAttribute(bGpsInfoFileName, sizeof(bGpsInfoFileName),AHC_FS_ATTR_HIDDEN);
                }
                else {
                    AHC_FS_DirClose(ulGpsInfoDirID);
                }

                STRCAT(bGpsInfoFileName, "\\");
                STRCAT(bGpsInfoFileName, (INT8*)GPS_TEMP_INFOFILE_NAME);
                GPSCtrl_OpenInfoFile(bGpsInfoFileName);
            }
            #endif

            #if ((GSENSOR_CONNECT_ENABLE) && (GSENSOR_FUNC & FUNC_VIDEOSTREM_INFO) && GSNR_USE_FILE_AS_DATABUF)
            if (AHC_Gsensor_Module_Attached())
            {
                MMP_BYTE    bGsensorInfoFileName[MAX_FILE_NAME_SIZE];
                MMP_ULONG   ulGsensorInfoDirID;

                STRCPY(bGsensorInfoFileName, (INT8*)AHC_UF_GetRootName());
                STRCAT(bGsensorInfoFileName, GSNR_TEMP_INFOFILE_DIR);

                if(AHC_FS_DirOpen(bGsensorInfoFileName, sizeof(bGsensorInfoFileName),&ulGsensorInfoDirID) != AHC_ERR_NONE)
                {
                    AHC_FS_DirCreate(bGsensorInfoFileName, sizeof(bGsensorInfoFileName));
                    AHC_FS_FileDirSetAttribute(bGsensorInfoFileName, sizeof(bGsensorInfoFileName),AHC_FS_ATTR_HIDDEN);
                }
                else{
                    AHC_FS_DirClose(ulGsensorInfoDirID);
                }

                STRCAT(bGsensorInfoFileName, "\\");
                STRCAT(bGsensorInfoFileName, (INT8*)GSNR_TEMP_INFOFILE_NAME);
                AHC_Gsensor_OpenInfoFile(bGsensorInfoFileName);
            }
            #endif
            #endif
        }

        return AHC_TRUE;
    }
    else {
        return AHC_FALSE;
    }
}
#endif ///< #if (VR_SEAMLESS)

AHC_BOOL AHC_SetVRVolumeParam(UINT8 ubDgain, UINT8 ubAGain, AHC_BOOL bBoosterOn)
{
    RecordDGain = ubDgain;
    RecordAGain = ubAGain;
    RecordBoosterOn = bBoosterOn;
    // Save setting only, using AHC_SetVRVolume to apply it
    //MMPS_AUDIO_SetRecordVolume(RecordDGain, RecordAGain, RecordBoosterOn);

    return AHC_TRUE;
}

AHC_BOOL AHC_GetVRVolumeParam(UINT8* pubDgain, UINT8* pubAGain, AHC_BOOL* pbBoosterOn)
{
    *pubDgain = RecordDGain;
    *pubAGain = RecordAGain;
    *pbBoosterOn = RecordBoosterOn;

    return AHC_TRUE;
}

AHC_BOOL AHC_SetVRVolume(AHC_BOOL bEnable)
{
    if (bEnable) {
        printc("D gain : 0x%x\r\n",RecordDGain);
        printc("A gain : 0x%x\r\n",RecordAGain);
        printc("Booster : %d\r\n" ,RecordBoosterOn);

        MMPS_AUDIO_SetRecordVolume(RecordDGain, RecordAGain, RecordBoosterOn);
    }
    else {
        printc("AHC_SetVRVolume - Mute !!!\r\n");
        MMPS_AUDIO_SetRecordVolume(0, 0, MMP_FALSE);
    }
    return MMP_TRUE;
}

AHC_BOOL AHC_SetVideoRecordVolume(AHC_BOOL bEnable)
{
    UINT32 u32DGain = 0;
    UINT32 u32AGain = 0;
    AHC_BOOL bBoostup = AHC_TRUE;

    if (bEnable) {
    	AHC_GetParam(PARAM_ID_AUDIO_IN_DIGITAL_GAIN, &u32DGain);
    	AHC_GetParam(PARAM_ID_AUDIO_IN_GAIN, &u32AGain);
    }

    if ((0 == u32DGain) && (0 == u32AGain)) {
        bBoostup = AHC_FALSE;
    }

    AHC_SetVRVolumeParam((UINT8) u32DGain, (UINT8) u32AGain, bBoostup);
    MMPS_AUDIO_SetRecordVolume((UINT8) u32DGain, (UINT8) u32AGain, bBoostup);

    return AHC_TRUE;
}

/**
Dear Paul,
You may use Uart command "rg <D gain> <A gain> <Booster on>" to adjust the gain while video record.

The default values are 0x3F, 0xFF and 1 for the three parameters.

The definition of D gain value:
6¡¦b 11 1111~ 6¡¦b 10 0000: 0.5dB per step (0dB~ -15.5dB)
6¡¦b 01 1111~ 6¡¦b 00 0000: 1 dB per step (-16dB~-46dB)

The definition of A gain value:
4 high bits are for Left channel, 4 low bits are for Right channel.
0000~1111: 2dB per step (-10dB~20dB)

The booster on provide additional 30dB gain.

e.x.
UART>rg 0x3f 0xff 0
Means set D gain 0x3f(0dB), A gain 0xff(20dB for each channel), and booster off(No additional 30dB).

Br,
Mark

*/
AHC_BOOL AHC_GetVRVolume(ULONG* piDGain, ULONG* piLAGain, ULONG* piRAGain, AHC_BOOL* pbBoosterOn)
{

    ULONG LAGain;
    ULONG RAGain;
    ULONG DGain;

    /*
    6¡¦b 11 1111~ 6¡¦b 10 0000: 0.5dB per step (0dB~ -15.5dB)
    */
    #define DGAIN_BITSIGN       (0x20)
    #define DGAIN_STEP_DEFAULT  (0x3F)  ///< 0b11 1111
    #define DGAIN_STEP_BASE     (0x20)  ///< 0b10 0000
    #define DGAIN_DELTA         (0.5)   ///< 0.5dB
    #define DGAIN_BASE          (-15.5) ///< -15.5dB

    /*
    6¡¦b 01 1111~ 6¡¦b 00 0000: 1 dB per step (-16dB~-46dB)
    */
    #define DGAIN_STEP_DEFAULT2  (0x1F)  ///< 0b01 1111
    #define DGAIN_STEP_BASE2     (0x00)  ///< 0b00 0000
    #define DGAIN_DELTA2         (1)     ///< 1dB
    #define DGAIN_BASE2          (-46)   ///< -46dB

    if((RecordDGain & DGAIN_BITSIGN) != 0){
        printc("DGain : 0dB ~ -15.5dB \n");
        DGain = DGAIN_BASE + (RecordDGain - DGAIN_STEP_BASE)*DGAIN_DELTA;
    }else{
        printc("DGain : -16dB ~ -46dB \n");
        DGain = DGAIN_BASE2 + (RecordDGain - DGAIN_STEP_BASE2)*DGAIN_DELTA2;
    }

    printc("DGain : %d dB \n", DGain);

    /*
    The definition of A gain value:
    4 high bits are for Left channel, 4 low bits are for Right channel.
    0000~1111: 2dB per step (-10dB~20dB)
    */
    #define AGAIN_LMASK(a)      ((a >> 4) & 0xF)
    #define AGAIN_RMASK(a)      (a & 0xF)

    #define AGAIN_STEP_DEFAULT  (0xF)  ///< 0b01 1111
    #define AGAIN_STEP_BASE     (0x0)  ///< 0b00 0000
    #define AGAIN_DELTA         (2)     ///< 1dB
    #define AGAIN_BASE          (-10)   ///< -16dB

    LAGain = AGAIN_BASE + (AGAIN_LMASK(RecordAGain)  - AGAIN_STEP_BASE)*AGAIN_DELTA;
    RAGain = AGAIN_BASE + (AGAIN_RMASK(RecordAGain)  - AGAIN_STEP_BASE)*AGAIN_DELTA;

    printc("AGain : -10dB ~ 20dB \n");
    printc("LAGain : %d dB \n", LAGain);
    printc("RAGain : %d dB \n", RAGain);

    *piDGain    = DGain;
    *piLAGain   = LAGain;
    *piRAGain   = RAGain;
    *pbBoosterOn = RecordBoosterOn;
    return MMP_TRUE;
}

void AIHC_AdjustJpegFromSensor(UINT16 w, UINT16 h, UINT16* suggestedWidth, UINT16* suggestedHeight)
{
    MMP_ULONG  SensorWidth, SensorHeight;
    MMP_ULONG  SuggestWidth, SuggestHeight;

    MMPS_Sensor_GetCurPrevScalInputRes(gsAhcCurrentSensor, &SensorWidth, &SensorHeight);

    MMPS_Display_PureAdjustScaleInSize(MMP_FALSE, SensorWidth, SensorHeight, w, h, &SuggestWidth, &SuggestHeight);

    //aligned with JEPG HW
    *suggestedWidth = ALIGN16(SuggestWidth);
    *suggestedHeight = ALIGN8(SuggestHeight);
    printc("Sensor %dx%d to preview MJPEG %dx%d\r\n", SensorWidth, SensorHeight, *suggestedWidth, *suggestedHeight);
}

AHC_BOOL AHC_GetVideoResolution(UINT16 stream, UINT16 *w, UINT16 *h)
{
    UINT32 ulRes;
    MMP_USHORT configWidth, configHeight;

    *w = *h = 0;
    if (AHC_FALSE == AIHC_GetMovieConfigEx(stream, AHC_VIDEO_RESOLUTION, &ulRes)) {
        return AHC_FALSE;
    }


    if (AIHC_MapToRawVRes(ulRes, &configWidth, &configHeight) == MMP_FALSE) {
        printc ("--E-- unsupported resolution config %d\r\n", ulRes);
        return AHC_FALSE;
    }
    AIHC_AdjustJpegFromSensor(configWidth, configHeight, w, h);
    return AHC_TRUE;
}

/** @brief Get the audio sampling rate
   Typically this is for UI or network so that they could know the setting of this build.
*/
UINT32 AHC_GetAudioSamplingRate( UINT8 uiAudioType )
{
    switch( uiAudioType )
    {
        case VR_AUDIO_TYPE_AAC:
            if (( AHC_AAC_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AAC_16K_32K )||
                ( AHC_AAC_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AAC_16K_64K ))
                return 16000;
            else if (( AHC_AAC_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AAC_22d05K_64K  )||
                     ( AHC_AAC_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AAC_22d05K_128K ))
                return 22050;
            else if (( AHC_AAC_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AAC_32K_64K  )||
                     ( AHC_AAC_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AAC_32K_128K ))
                return 32000;
            else if ( AHC_AAC_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AAC_48K_128K  )
                return 48000;
            else if (( AHC_AAC_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AAC_44d1K_64K  )||
                     ( AHC_AAC_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AAC_44d1K_128K ))
                return 44100;
            else
            {
                printc(FG_RED("!!! WARNING !!! Un-considered AAC option  (1)\r\n"));
                return 32000;
            }

        case VR_AUDIO_TYPE_MP3:
            if ( AHC_MP3_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_MP3_32K_128K )
                return 32000;
            else if ( AHC_MP3_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_MP3_44d1K_128K )
                return 44100;
            else
            {
                printc(FG_RED("!!! WARNING !!! Un-considered MP3 option  (1)\r\n"));
                return 32000;
            }

        case VR_AUDIO_TYPE_ADPCM:
            if ( AHC_ADPCM_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_ADPCM_16K_22K )
                return 16000;
            else if ( AHC_ADPCM_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_ADPCM_32K_22K )
                return 32000;
            else if ( AHC_ADPCM_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_ADPCM_44d1K_22K )
                return 44100;
            else
            {
                printc(FG_RED("!!! WARNING !!! Un-considered ADPCM option  (1)\r\n"));
                return 32000;
            }

        case VR_AUDIO_TYPE_AMR:
            if ( AHC_AMR_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AMR_4d75K )
                return 4750;
            else if ( AHC_AMR_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AMR_5d15K )
                return 5150;
            else if ( AHC_AMR_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AMR_12d2K )
                return 12200;
            else
            {
                printc(FG_RED("!!! WARNING !!! Un-considered AMR option  (1)\r\n"));
                return 32000;
            }

        default:
            printc(FG_RED("!!! WARNING !!! Un-supported audio type: %d  (1)\r\n"), uiAudioType);
            return 32000;
    }
}

/** @brief Get the audio bit rate
   Typically this is for UI or network so that they could know the setting of this build.
*/
UINT32 AHC_GetAudioBitRate( UINT8 uiAudioType )
{
    switch( uiAudioType )
    {
        case VR_AUDIO_TYPE_AAC:
            if ( AHC_AAC_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AAC_16K_32K )
                return 32000;
            else if (( AHC_AAC_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AAC_16K_64K  )   ||
                     ( AHC_AAC_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AAC_22d05K_64K ) ||
                     ( AHC_AAC_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AAC_32K_64K )    ||
                     ( AHC_AAC_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AAC_44d1K_64K ))
                return 64000;
            else if (( AHC_AAC_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AAC_22d05K_128K  )||
                     ( AHC_AAC_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AAC_32K_128K )    ||
                     ( AHC_AAC_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AAC_48K_128K )    ||
                     ( AHC_AAC_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AAC_44d1K_128K ))
                return 128000;
            else
            {
                printc(FG_RED("!!! WARNING !!! Un-considered AAC option (2)\r\n"));
                return 128000;
            }

        case VR_AUDIO_TYPE_MP3:
            if (( AHC_MP3_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_MP3_32K_128K ) ||
                ( AHC_MP3_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_MP3_44d1K_128K ))
                return 128000;
            else
            {
                printc(FG_RED("!!! WARNING !!! Un-considered MP3 option (2)\r\n"));
                return 128000;
            }

        case VR_AUDIO_TYPE_ADPCM:
            if (( AHC_ADPCM_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_ADPCM_16K_22K ) ||
                ( AHC_ADPCM_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_ADPCM_32K_22K ) ||
                ( AHC_ADPCM_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_ADPCM_44d1K_22K ))
                return 22000;
            else
            {
                printc(FG_RED("!!! WARNING !!! Un-considered ADPCM option (2)\r\n"));
                return 22000;
            }

        case VR_AUDIO_TYPE_AMR:
            if (( AHC_ADPCM_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AMR_4d75K )||
                ( AHC_ADPCM_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AMR_5d15K )||
                ( AHC_ADPCM_AUDIO_OPTION == MMPS_3GPRECD_AUDIO_AMR_12d2K ))
                return 8000;
            else
            {
                printc(FG_RED("!!! WARNING !!! Un-considered AMR option (2)\r\n"));
                return 8000;
            }

        default:
            printc(FG_RED("!!! WARNING !!! Un-supported audio type: %d (2)\r\n"), uiAudioType);
            return 128000;
    }
}



/** @brief Get the default audio sampling rate
   Typically this is for UI or network so that they could know the default setting of this build.
   Example usage: RTSP default SDP for audio sampling rate. At that moment the audio parameters are not set.
*/
UINT32 AHC_GetDefaultAudioSamplingRate(void)
{
    return AHC_GetAudioSamplingRate( VR_AUDIO_TYPE );
}


/** @brief Config audio stream

  @note Audio bitrate is defined in something like AHC_AAC_AUDIO_OPTION and AHC_MP3_AUDIO_OPTION
        so sampling rate and bit rate will be ignored. Those fields are for back up status only.

  @param stream[in] The audio stream ID
  @param aFormat[in] audio format
  @param channelConfig channel configuration
*/
AHC_BOOL AHC_ConfigAudio(UINT16 stream, AHC_AUDIO_FORMAT aFormat,
                         AHC_AUDIO_CHANNEL_CONFIG channelConfig)
{
    AHC_BOOL bMute;

    //These 2 keeps the value of the setting for quicker reference only. The real settings
    // automatically applied in MMPS
    switch (aFormat) {
    case AHC_MOVIE_AUDIO_CODEC_AAC:
        AHC_SetMovieConfig(stream, AHC_AUD_BITRATE, AHC_GetAudioBitRate(VR_AUDIO_TYPE_AAC));
        AHC_SetMovieConfig(stream, AHC_AUD_SAMPLE_RATE, AHC_GetAudioSamplingRate(VR_AUDIO_TYPE_AAC));
        break;
    case AHC_MOVIE_AUDIO_CODEC_MP3:
        AHC_SetMovieConfig(stream, AHC_AUD_BITRATE, AHC_GetAudioBitRate(VR_AUDIO_TYPE_MP3));
        AHC_SetMovieConfig(stream, AHC_AUD_SAMPLE_RATE, AHC_GetAudioSamplingRate(VR_AUDIO_TYPE_MP3));
        break;
    case AHC_MOVIE_AUDIO_CODEC_ADPCM:
        AHC_SetMovieConfig(stream, AHC_AUD_BITRATE, AHC_GetAudioBitRate(VR_AUDIO_TYPE_ADPCM));
        AHC_SetMovieConfig(stream, AHC_AUD_SAMPLE_RATE, AHC_GetAudioSamplingRate(VR_AUDIO_TYPE_ADPCM));
        break;
    default:
        return AHC_FALSE;
        break;
    }
    if (MMP_FALSE == AHC_SetMovieConfig(stream, AHC_AUD_CODEC_TYPE, aFormat)) {
        //invalid stream
        return MMP_FALSE;
    }
    AHC_SetMovieConfig(stream, AHC_AUD_CHANNEL_CONFIG, channelConfig);
    if(MenuSettingConfig()->uiMOVSoundRecord == MOVIE_SOUND_RECORD_ON) {
        bMute = AHC_FALSE;
    } else {
        bMute = AHC_TRUE;
    }
    AHC_SetMovieConfig(stream, AHC_AUD_MUTE_END, bMute);
    return AHC_TRUE;
}

#if (WIFI_PORT == 1)
AHC_BOOL AHC_Config2ndVideo(AHC_VIDEO_2ND_CONFIG *cfg)
{
    const UINT16 stream = 1;
#if WIFI_PORT
    UINT32 effective_bitrate;
#endif

    switch (cfg->vFormat) {
    case AHC_MOVIE_VIDEO_CODEC_YUV422:
        AHC_SetMovieConfig(stream, AHC_VIDEO_RESOLUTION, AHC_VIDRECD_RESOL_AUTO);
        break;
    case AHC_MOVIE_VIDEO_CODEC_MJPEG:
        AHC_SetMovieConfig(stream, AHC_VIDEO_RESOLUTION, cfg->param.mj.ahcRes);
        AHC_SetMovieConfig(stream, AHC_VIDEO_BITRATE, cfg->param.mj.Bitrate);
        break;
	case AHC_MOVIE_VIDEO_CODEC_NONE:
		return AHC_SetMovieConfig(stream, AHC_VIDEO_CODEC_TYPE, cfg->vFormat);
		break;
    default:
        return AHC_FALSE;
        break;
    }

    AHC_SetMovieConfig(stream, AHC_VIDEO_CODEC_TYPE, cfg->vFormat);
    AHC_SetMovieConfig(stream, AHC_FRAME_RATE, cfg->FPS);

    cfg->FPS = AHC_GetVideoRealFrameRate(cfg->FPS);
    cfg->FPS = cfg->FPS / AHC_VIDRECD_TIME_INCREMENT_BASE;
    
    if (cfg->FpsOutx1000 > 0 && (cfg->FpsOutx1000 <= cfg->FPS * 1000)) {
        if (0 != netapp_set_FRC(cfg->FPS * 1000, cfg->FpsOutx1000, "VIDEO.0/EncMJPEG")) {
            return AHC_FALSE;
        }
        //assume max 15,000,000 * 120 FPS and still not overflow.
        effective_bitrate = cfg->param.mj.Bitrate * cfg->FPS / cfg->FpsOutx1000 * 1000;
        printc("Effective Bitrate %d by %d in/out %d/%d\r\n",
        	   effective_bitrate, cfg->param.mj.Bitrate, cfg->FPS, cfg->FpsOutx1000);
        AHC_SetMovieConfig(stream, AHC_VIDEO_BITRATE, effective_bitrate);
    }
    return AHC_TRUE;
}
#endif

#if (SUPPORT_LDC_RECD)
UINT16 AIHC_GetCurrentRecordModeIdx(void)
{
    MMP_USHORT VideoRes[VIDRECD_RESOL_MAX_NUM] =
    {
    	VIDRECD_RESOL_160x120,
        VIDRECD_RESOL_176x144,
        VIDRECD_RESOL_272x144,
        VIDRECD_RESOL_320x240,
        VIDRECD_RESOL_352x288,
        VIDRECD_RESOL_368x208,
        VIDRECD_RESOL_480x272,
        VIDRECD_RESOL_624x352,
        VIDRECD_RESOL_640x368,
        VIDRECD_RESOL_640x480,
        VIDRECD_RESOL_720x480,
        VIDRECD_RESOL_848x480,
        VIDRECD_RESOL_912x512,
        VIDRECD_RESOL_960x720,
        VIDRECD_RESOL_1280x720,
        VIDRECD_RESOL_1440x1088,
        VIDRECD_RESOL_1920x1088,
        VIDRECD_RESOL_2304x1296,
        VIDRECD_RESOL_2704x1536,
        VIDRECD_RESOL_3840x2160
    };

	UINT32               	EncWidth, EncHeight, i;
	MMPS_3GPRECD_CONFIGS 	*pVideoConfig = MMPS_3GPRECD_GetConfig();
	UINT16					usResolIdx;
	AHC_BOOL          		bFind = AHC_FALSE;

	AHC_GetImageSize(VIDEO_CAPTURE_MODE, &EncWidth, &EncHeight);

	for (i = 0; i < VIDRECD_RESOL_MAX_NUM; i++) {
        if ((EncWidth == pVideoConfig->usEncWidth[i]) && (EncHeight == pVideoConfig->usEncHeight[i])) {
            usResolIdx = i;
			bFind = AHC_TRUE;
            break;
        }
    }

	if (!bFind) {
		for (i = 0; i < VIDRECD_RESOL_MAX_NUM; i++) {
	        if ((EncWidth * EncHeight) <= (pVideoConfig->usEncWidth[i] * pVideoConfig->usEncHeight[i])) {
	            usResolIdx = i;
	            break;
	        }
    	}
	}

	return VideoRes[usResolIdx];
}
#endif

AHC_BOOL AIHC_UVCPreviewStart(MMP_USHORT usVideoPreviewMode, AHC_DISPLAY_OUTPUTPANEL OutputDevice, UINT32 ulFlickerMode)
{
#if (UVC_HOST_VIDEO_ENABLE)
    MMPS_3GPRECD_CONFIGS	*pVideoConfig = MMPS_3GPRECD_GetConfig();
    USB_DETECT_PHASE 		USBCurrentStates = 0;
    UINT32 					ulUSBTimeout = 0;
    MMP_ERR sRet = MMP_ERR_NONE;

    //Wait until rear cam enumerates done.
    ulUSBTimeout = 80;
    do{
        AHC_OS_SleepMs(10);
        AHC_USBGetStates(&USBCurrentStates,AHC_USB_GET_PHASE_CURRENT);
    }while(((USBCurrentStates == USB_DETECT_PHASE_OTG_SESSION) ||
        	(USBCurrentStates == USB_DETECT_PHASE_CHECK_CONN)) && (--ulUSBTimeout > 0));

    if(0 == ulUSBTimeout){printc("\r\n[No UVC device connected: %x]\r\n", USBCurrentStates); }


    AHC_USBDetectSetPriority(AHC_FALSE); //Set USB detection as low priority work after first preview.
        
    if (USBCurrentStates == USB_DETECT_PHASE_REAR_CAM) {
        UINT16 usdisplay_width, usdisplay_height;
        UVC_DEVICE_NAME_INDEX sUVCDevcie;

        MMP_BOOL 	bUserConfig, bRotate;
        MMP_UBYTE ubRotateDir, sFitMode;
        MMP_USHORT usBufWidth, usBufHeight, usStartX, usStartY, usWinWidth, usWinHeight;
        AHC_BOOL ahcRet = AHC_TRUE;                                      	 
        
        sRet = MMPS_3GPRECD_UVCGetCustomedPrevwAttr(&bUserConfig, &bRotate, &ubRotateDir, &sFitMode,
                                                                     &usBufWidth, &usBufHeight,&usStartX, &usStartY, &usWinWidth, &usWinHeight);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}                 
        
        AHC_HostUVCVideoEnable(AHC_TRUE);
        ahcRet = AHC_HostUVCVideoGetChipName(&sUVCDevcie);
        if(ahcRet != AHC_TRUE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); /*return AHC_FALSE;*/}                 

        if (usVideoPreviewMode == VIDRECD_NORMAL_PREVIEW_MODE) {
            usdisplay_width = pVideoConfig->previewdata[0].usVidPreviewBufW[VIDRECD_FULL_PREVIEW_MODE];
            usdisplay_height = pVideoConfig->previewdata[0].usVidPreviewBufH[VIDRECD_FULL_PREVIEW_MODE];
        }
        else{
            usdisplay_width = pVideoConfig->previewdata[0].usVidPreviewBufW[usVideoPreviewMode];
            usdisplay_height = pVideoConfig->previewdata[0].usVidPreviewBufH[usVideoPreviewMode];
        }
#if (SUPPORT_SONIX_UVC_ISO_MODE==1)
        if (sUVCDevcie == UVC_DEVICE_NAME_PID_0210 ||
            sUVCDevcie == UVC_DEVICE_NAME_PID_64AB ||
            sUVCDevcie == UVC_DEVICE_NAME_PID_9230){
            AHC_HostUVCVideoSetting(UVC_H264_RES_720P, UVC_FRAMERATE_25FPS, UVC_H264_QUALITY_HIGH, 25, UVC_YUV_RESOL_180P);
        }
        else if (sUVCDevcie == UVC_DEVICE_NAME_PID_6300){
            AHC_HostUVCVideoSetting(UVC_H264_RES_360P, UVC_FRAMERATE_25FPS, UVC_H264_QUALITY_HIGH, 25, UVC_YUV_RESOL_180P);
        }
        else {
            AHC_HostUVCVideoEnable(AHC_FALSE);
            printc(FG_RED("%s,%d error! Not suport device.\r\n"), __func__, __LINE__);
        }                
#else
        if (sUVCDevcie == UVC_DEVICE_NAME_8437){
            AHC_HostUVCVideoSetting(UVC_H264_RES_1080P, UVC_FRAMERATE_30FPS, UVC_H264_QUALITY_HIGH, 30, UVC_YUV_RESOL_180P);
            AHC_HostUVC_NV12_Mirror(MMP_TRUE); 
        }
        else if (sUVCDevcie == UVC_DEVICE_NAME_8451){
            AHC_HostUVCVideoSetting(UVC_H264_RES_720P, UVC_FRAMERATE_30FPS, UVC_H264_QUALITY_HIGH, 30, UVC_YUV_RESOL_180P);
        }
        else {
            AHC_HostUVCVideoEnable(AHC_FALSE);                    
            printc(FG_RED("%s,%d error! Not suport device.\r\n"), __func__, __LINE__);
        }
#endif
        if(AHC_TRUE == AHC_HostUVCVideoIsEnabled()){
            UINT8    ubWinID;
            UINT16   usWinW, usWinH;
            UINT16   usDispW, usDispH;
            UINT16   usOffsetX, usOffsetY;                        
            MMP_DISPLAY_COLORMODE sDisplayColorMode;                        
            
            if (COMMON_FLICKER_50HZ == ulFlickerMode)
                AHC_HostUVCSetFlicker(UVC_FLICKER_50HZ);
            else
                AHC_HostUVCSetFlicker(UVC_FLICKER_60HZ);

            if(bUserConfig){
#if (SUPPORT_SONIX_UVC_ISO_MODE==1) && (SUPPORT_DEC_MJPEG_TO_PREVIEW)     
                if (sUVCDevcie == UVC_DEVICE_NAME_PID_0210||
                    sUVCDevcie == UVC_DEVICE_NAME_PID_64AB ||
                    sUVCDevcie == UVC_DEVICE_NAME_PID_9230 ){
                    ubWinID = PREVIEW_DEVICE_YUV_WIN_ID;
                    usWinW = gUsbh_UvcDevInfo.wPrevwMaxWidth >> 1;
                    usWinH = gUsbh_UvcDevInfo.wPrevwMaxHeight >> 1;
                    usDispW = usWinWidth;
                    usDispH = usWinHeight;
                    usOffsetX = usStartX;
                    usOffsetY = usStartY;                        
                    sDisplayColorMode = MMP_DISPLAY_COLOR_YUV420;               
                }
                else if (sUVCDevcie == UVC_DEVICE_NAME_PID_6300){ //Max 640x480
                    ubWinID = PREVIEW_DEVICE_YUV_WIN_ID;
                    usWinW = gUsbh_UvcDevInfo.wPrevwMaxWidth;
                    usWinH = gUsbh_UvcDevInfo.wPrevwMaxHeight;
                    usDispW = usWinWidth;
                    usDispH = usWinHeight;
                    usOffsetX = usStartX;
                    usOffsetY = usStartY;                        
                    sDisplayColorMode = MMP_DISPLAY_COLOR_YUV420;               
                }
                else {
                    printc(FG_RED("%s,%d error! Not suport device.\r\n"), __func__, __LINE__);
                }    

#if (CCIR656_OUTPUT_ENABLE) 
                usDispW = usdisplay_width; //full screen
                usDispH = usdisplay_height;
                usOffsetX = 0;
                usOffsetY = 0;                        
                
                usWinW = (usWinW > usDispW) ? usWinW : usDispW;
                usWinH = (usWinH > usDispH) ? usWinH : usDispH;
                sDisplayColorMode = pVideoConfig->previewdata[1].DispColorFmt[OutputDevice]; //MMP_DISPLAY_COLOR_YUV422
#endif                        
                AHC_HostUVCVideoInitDecMjpegToPreview(ubWinID, usWinW, usWinH, usDispW, usDispH, usOffsetX, usOffsetY, sDisplayColorMode);                                                                           
#else                        
                AHC_HostUVCVideoSetWinAttribute(PREVIEW_DEVICE_YUV_WIN_ID, usWinWidth, usWinHeight, usStartX, usStartY, MMP_SCAL_FITMODE_OPTIMAL, bRotate);                    
#endif
            }
            else{                        
#if (SUPPORT_SONIX_UVC_ISO_MODE==1) && (SUPPORT_DEC_MJPEG_TO_PREVIEW)                             
                if (sUVCDevcie == UVC_DEVICE_NAME_PID_0210||
                    sUVCDevcie == UVC_DEVICE_NAME_PID_64AB ||
                    sUVCDevcie == UVC_DEVICE_NAME_PID_9230 ){
                    ubWinID = PREVIEW_DEVICE_YUV_WIN_ID;
                    usWinW = gUsbh_UvcDevInfo.wPrevwMaxWidth >> 1;
                    usWinH = gUsbh_UvcDevInfo.wPrevwMaxHeight >> 1;
                    usDispW = usdisplay_width >> 1;
                    usDispH = usdisplay_height >> 1;
                    usOffsetX = usdisplay_width >> 1;
                    usOffsetY = usdisplay_height >> 1;                        
                    sDisplayColorMode = MMP_DISPLAY_COLOR_YUV420_INTERLEAVE;                        
                }
                else if (sUVCDevcie == UVC_DEVICE_NAME_PID_6300){ //Max 640x480
                    ubWinID = PREVIEW_DEVICE_YUV_WIN_ID;
                    usWinW = gUsbh_UvcDevInfo.wPrevwMaxWidth;
                    usWinH = gUsbh_UvcDevInfo.wPrevwMaxHeight;
                    usDispW = usdisplay_width >> 1;
                    usDispH = usdisplay_height >> 1;
                    usOffsetX = usdisplay_width >> 1;
                    usOffsetY = usdisplay_height >> 1;                        
                    sDisplayColorMode = MMP_DISPLAY_COLOR_YUV420_INTERLEAVE;                        
                }
                else {
                    printc(FG_RED("%s,%d error! Not suport device.\r\n"), __func__, __LINE__);
                } 
#if (CCIR656_OUTPUT_ENABLE) 
                usDispW = usdisplay_width; //full screen
                usDispH = usdisplay_height;
                usOffsetX = 0;
                usOffsetY = 0;                        
                
                usWinW = (usWinW > usDispW) ? usWinW : usDispW;
                usWinH = (usWinH > usDispH) ? usWinH : usDispH;
                sDisplayColorMode = pVideoConfig->previewdata[1].DispColorFmt[OutputDevice]; //MMP_DISPLAY_COLOR_YUV422
#endif                        
                AHC_HostUVCVideoInitDecMjpegToPreview(ubWinID, usWinW, usWinH, usDispW, usDispH, usOffsetX, usOffsetY, sDisplayColorMode);                                                    
#else                                                
                AHC_HostUVCVideoSetWinAttribute(PREVIEW_DEVICE_YUV_WIN_ID, usdisplay_width >> 1, usdisplay_height >> 1, usdisplay_width >> 1, usdisplay_height >> 1, MMP_SCAL_FITMODE_OPTIMAL, bRotate);
#endif
            }
        }

        sRet = MMPS_3GPRECD_SetUVCFBMemory();
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}         
        
#if (SUPPORT_SONIX_UVC_ISO_MODE==1) && (SUPPORT_DEC_MJPEG_TO_PREVIEW)                             
        {
            MMP_USHORT usW = 0, usH = 0;

            sRet = MMPS_3GPRECD_SetUVCRearMJPGMemory();                    
            sRet |= MMPS_3GPRECD_GetDecMjpegToPreviewSrcAttr(&usW, &usH);

            sRet |= MMPS_3GPRECD_InitDecMjpegToPreview(usW, usH);

            sRet |= MMPS_3GPRECD_DecMjpegPreviewStart(gsAhcCurrentSensor);
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}                 
        }
#endif                
    }
#endif

#if (UVC_HOST_VIDEO_ENABLE)
            AHC_USBGetStates(&USBCurrentStates,AHC_USB_GET_PHASE_CURRENT);

            if ((USBCurrentStates == USB_DETECT_PHASE_REAR_CAM) && (AHC_TRUE == AHC_HostUVCVideoIsEnabled())){
#if (SUPPORT_PARKINGMODE == PARKINGMODE_NONE)
                if (MenuSettingConfig()->uiMotionDtcSensitivity != MOTION_DTC_SENSITIVITY_OFF)
#else
                if (uiGetParkingModeEnable() == AHC_TRUE && MenuSettingConfig()->uiMotionDtcSensitivity != MOTION_DTC_SENSITIVITY_OFF )
#endif
                {
                    AHC_HostUVCVideoEnableMD(AHC_TRUE);
                }
                else
                {
                    AHC_HostUVCVideoEnableMD(AHC_FALSE);
                }

                AHC_HostUVCVideoPreviewStart();
#if defined(WIFI_PORT) && (WIFI_PORT == 1)
                netapp_setRearcamReady(AHC_TRUE);
#endif
            }
#endif
    return AHC_TRUE;
}

AHC_BOOL AIHC_UVCPreviewStop(void)
{
    AHC_BOOL ahcRet = AHC_TRUE;
    MMP_ERR sRet = MMP_ERR_NONE;

#if (UVC_HOST_VIDEO_ENABLE)
    ahcRet = AHC_HostUVCVideoPreviewStop();
    ahcRet = AHC_HostUVCVideoEnable(AHC_FALSE);
#endif

#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
    sRet = MMPS_3GPRECD_DecMjpegPreviewStop(gsAhcCurrentSensor);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
#endif        


    return AHC_TRUE;
}

/**
 @brief Set VideoR previrw / record

 Set VideoR previrw / record

 @param[in] bAudioOn        encode audio or not
 @param[in] bStartRecord    Preview or record
 @retval AHC_BOOL
*/
AHC_BOOL AIHC_SetVideoRecordMode(UINT8 bAudioOn, AHC_VIDRECD_CMD bStartRecord)
{
    MMPS_3GPRECD_VIDEO_FORMAT   VideoFmt;
    MMPS_3GPRECD_AUDIO_FORMAT   AudioFmt;
    MMPS_3GPRECD_CONTAINER      ContainerType;
    MMP_USHORT                  usQuality;
    MMP_USHORT                  usVideoPreviewMode, usResolIdx;
    MMP_ULONG                   ulSamplerate;
    MMPS_AUDIO_LINEIN_CHANNEL   AudioLineIn;
    MMP_BYTE                    bFileName[MAX_FILE_NAME_SIZE];

#if ((UVC_HOST_VIDEO_ENABLE == 1 || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)
    MMP_BYTE                    bRearFileName[MAX_FILE_NAME_SIZE];
#endif
#if (GPS_CONNECT_ENABLE && GPS_USE_FILE_AS_DATABUF)
    MMP_BYTE                    bGpsInfoFileName[MAX_FILE_NAME_SIZE];
    MMP_ULONG                   ulGpsInfoDirID;
#endif
    #if (GPS_KML_FILE_ENABLE == 1)
    MMP_BYTE                    bKMLFileName[MAX_FILE_NAME_SIZE];
    #endif
    #if (GPS_RAW_FILE_ENABLE == 1)
    MMP_BYTE                    bGPSRawFileName[MAX_FILE_NAME_SIZE];
    #endif
#if ((GSENSOR_RAW_FILE_ENABLE == 1) && (GPS_CONNECT_ENABLE == 0))
    MMP_BYTE                    bGPSRawFileName[MAX_FILE_NAME_SIZE];
#endif
    MMP_BYTE                    DirName[16];
    UINT32                      Param;
    INT32                       FrameRate, PFrameNum, BFrameNum;
    UINT32                      EncWidth,EncHeight,i;
    MMPS_3GPRECD_CONFIGS        *VideoConfig = MMPS_3GPRECD_GetConfig();
    UINT8                       bCreateNewDir;
    MMP_USHORT                  zoomIndex;
    UINT8                       ubSoundEffectStatus = 0;
	AHC_DISPLAY_OUTPUTPANEL  	OutputDevice;
    AHC_BOOL                    bRet;
    #if (VR_SEAMLESS)
    UINT32                      TimeLimit = 0;
    #endif
    MMP_ULONG                   ulBitRate = 0;
    MMP_ERR sRet = MMP_ERR_NONE;
    MMP_UBYTE                   ubWMR;
    MMP_UBYTE                   ubRecordWithAudio;    
    MMP_ULONG 					ulRealPreencodeTime = 0;
    AHC_WINDOW_RECT rect;
    
    static AHC_VIDRECD_CMD      bStartRecord_Last = AHC_VIDRECD_STOP;

    if(( bStartRecord_Last == AHC_VIDRECD_STOP || bStartRecord_Last == AHC_VIDRECD_RESTART_PREVIEW ) && 
       ( bStartRecord == AHC_VIDRECD_START ))
    {
        // Note: In some cases, Ex: Change sensor mode, change Y-frame typs (size).
        // Preview buffers have to be re-allocated.
        // And if you see below messages when above cases, system may meet problems.
        
        printc(FG_RED("!!!WARNING!!! Start recording without re-allocate Preview buffers!!!\r\n"));
    }

    bStartRecord_Last = bStartRecord;

    if( bStartRecord == AHC_VIDRECD_RESTART_PREVIEW )
    {
        #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
        if (uiGetParkingModeEnable())
        #else
        if (MenuSettingConfig()->uiMOVPreRecord == MOVIE_PRE_RECORD_ON)
        #endif
        {
            AIHC_GetMovieConfig(AHC_VIDEO_PRERECORD_LENGTH,&Param);
            printc("Enable Video Pre Record: %d ms\r\n", Param);
            ulRealPreencodeTime = Param;
#if (UVC_HOST_VIDEO_ENABLE && DUALENC_SUPPORT) 
			{
				USB_DETECT_PHASE USBCurrentStates = 0;
	            AHC_USBGetStates(&USBCurrentStates,AHC_USB_GET_PHASE_CURRENT);
	            if ((USBCurrentStates == USB_DETECT_PHASE_REAR_CAM) && 
				(AHC_TRUE == AHC_HostUVCVideoIsEnabled())){
	                sRet = MMPS_3GPRECD_SetDualH264Enable(MMP_TRUE, MMP_VIDRECD_USEMODE_RECD);
					if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
					AHC_HostUVCVideoDualEncodeSetting();

	            	MMPS_3GPRECD_GetAllEncPreRecordTime(Param, &ulRealPreencodeTime);
				}
			}
#endif            
            sRet = MMPS_3GPRECD_PreRecord(ulRealPreencodeTime);
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
#if (UVC_HOST_VIDEO_ENABLE == 1 && DUALENC_SUPPORT == 1) 
            {
                USB_DETECT_PHASE USBCurrentStates = 0;

                AHC_USBGetStates(&USBCurrentStates,AHC_USB_GET_PHASE_CURRENT);
                if ((USBCurrentStates == USB_DETECT_PHASE_REAR_CAM) && 
                    (AHC_TRUE == AHC_HostUVCVideoIsEnabled())){
                    if (!MMPS_3GPRECD_DualEncPreRecord(ulRealPreencodeTime)){ 
                        AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); 
                        return AHC_FALSE;
                    }
                }
            }
#elif (SUPPORT_DUAL_SNR == 1 && DUALENC_SUPPORT == 1)
			if(AHC_TRUE == AHC_GetSecondSensorCnntStatus())//bit1603 must be connect
			{
	            if (!MMPS_3GPRECD_DualEncPreRecord(ulRealPreencodeTime)){ 
	                AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); 
	                return AHC_FALSE;
	            }
            }
#endif
            
            AHC_VideoRecordStartWriteInfo();
        }
        printc("========AHC_VIDRECD_RESTART_PREVIEW=============\r\n");
        return AHC_TRUE;
    }

    #if (AHC_ENABLE_VIDEO_STICKER == 0)
    {
        AHC_ConfigCapture(ACC_DATE_STAMP, AHC_ACC_TIMESTAMP_DISABLE);
    }
    #endif

    AHC_GetImageSize(VIDEO_CAPTURE_MODE, &EncWidth, &EncHeight);
    
    MMPS_3GPRECD_SetStillCaptureMaxRes((MMP_USHORT)EncWidth, (MMP_USHORT)EncHeight);

    for (i = 0 ; i < VIDRECD_RESOL_MAX_NUM; i++) {
        if ((EncWidth * EncHeight) <= (VideoConfig->usEncWidth[i] * VideoConfig->usEncHeight[i])) {
            usResolIdx = i;
            break;
        }
    }

    AHC_ConfigMovie(AHC_VIDEO_RESOLUTION, usResolIdx);

    sRet = MMPS_3GPRECD_SetEncResIdx(usResolIdx);
    if (sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}         

    switch (MenuSettingConfig()->uiMOVQuality) {
    case QUALITY_SUPER_FINE:
        usQuality = VIDRECD_QUALITY_HIGH;
    break;
    case QUALITY_FINE:
        usQuality = VIDRECD_QUALITY_MID;
    break;
    default:
        usQuality = VIDRECD_QUALITY_HIGH;
    break;
    }
    AHC_ConfigMovie(AHC_VIDEO_COMPRESSION_RATIO, usQuality);

    AIHC_GetMovieConfig(AHC_VIDEO_COMPRESSION_RATIO, &Param);
    usQuality        = Param;

#if (VR_VIDEO_TYPE_SETTING == COMMON_VR_VIDEO_TYPE_SETTING_H264ENC_HIGH_PROFILE)
    AHC_ConfigMovie(AHC_VIDEO_CODEC_TYPE_SETTING, MMPS_H264ENC_HIGH_PROFILE);
    sRet = MMPS_3GPRECD_SetProfile(MMPS_H264ENC_HIGH_PROFILE);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}     
#elif (VR_VIDEO_TYPE_SETTING == COMMON_VR_VIDEO_TYPE_SETTING_H264ENC_BASELINE_PROFILE)
    AHC_ConfigMovie(AHC_VIDEO_CODEC_TYPE_SETTING, MMPS_H264ENC_BASELINE_PROFILE);
    sRet = MMPS_3GPRECD_SetProfile(MMPS_H264ENC_BASELINE_PROFILE);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}     
#else
    #error "VR_VIDEO_TYPE_SETTING NG!!!"
#endif

    #if (C005_PARKING_CLIP_Length_10PRE_10REC) && (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
    if (uiGetParkingModeEnable())
    #else
	if (MenuSettingConfig()->uiMOVPreRecord == MOVIE_PRE_RECORD_ON)
	#endif
    {
        if (VideoConfig->ulFps30BitrateMap[usResolIdx][usQuality] > AHC_VIDEO_MAXBITRATE_PRERECORD)
        {
#if (MENU_MOVIE_FRAME_RATE_EN)
            switch(MenuSettingConfig()->uiMOVFrameRate)
            {
            #if (MENU_MOVIE_FRAME_RATE_30_EN)
                case MOVIE_FRAME_RATE_30FPS:
                    sRet = MMPS_3GPRECD_SetBitrate(AHC_VIDEO_MAXBITRATE_PRERECORD);
                    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
                break;
            #endif
            #if (MENU_MOVIE_FRAME_RATE_20_EN)
                case MOVIE_FRAME_RATE_20FPS:
                    sRet = MMPS_3GPRECD_SetBitrate(AHC_VIDEO_MAXBITRATE_PRERECORD*2/3);
                    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}                     
                break;
            #endif
        #if (C005_PARKING_LOW_FRAME_RATE_15FPS == 1)
            #if (MENU_MOVIE_FRAME_RATE_15_EN)
                case MOVIE_FRAME_RATE_15FPS:
                    sRet = MMPS_3GPRECD_SetBitrate(AHC_VIDEO_MAXBITRATE_PRERECORD/2);
                    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}                     
                break;
            #endif
        #endif
            #if (MENU_MOVIE_FRAME_RATE_10_EN)
                case MOVIE_FRAME_RATE_10FPS:
                    sRet = MMPS_3GPRECD_SetBitrate(AHC_VIDEO_MAXBITRATE_PRERECORD/3);
                    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}                     
                break;
            #endif
            }
#else
        #if C005_PARKING_LOW_FRAME_RATE_15FPS
        if(MenuSettingConfig()->uiParkingMode_LowFrameRate == PARKINGMODE_LOW_FRAME_RATE_ON)
            sRet = MMPS_3GPRECD_SetBitrate(AHC_VIDEO_MAXBITRATE_PRERECORD/2);
        else
            sRet = MMPS_3GPRECD_SetBitrate(AHC_VIDEO_MAXBITRATE_PRERECORD);
        #endif
#endif
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
        }
    }
    else
    {
#if (MENU_MOVIE_FRAME_RATE_EN)
        switch(MenuSettingConfig()->uiMOVFrameRate)
        {
        #if (MENU_MOVIE_FRAME_RATE_30_EN)
            case MOVIE_FRAME_RATE_30FPS:
                sRet = MMPS_3GPRECD_SetBitrate(VideoConfig->ulFps30BitrateMap[usResolIdx][usQuality]);
            break;
        #endif
        #if (MENU_MOVIE_FRAME_RATE_20_EN)
            case MOVIE_FRAME_RATE_20FPS:
                sRet = MMPS_3GPRECD_SetBitrate(VideoConfig->ulFps30BitrateMap[usResolIdx][usQuality]*2/3);
            break;
        #endif
    #if (C005_PARKING_LOW_FRAME_RATE_15FPS == 1)
        #if (MENU_MOVIE_FRAME_RATE_15_EN)
            case MOVIE_FRAME_RATE_15FPS:
                sRet = MMPS_3GPRECD_SetBitrate(VideoConfig->ulFps30BitrateMap[usResolIdx][usQuality]/2);
            break;
        #endif
    #endif
        #if (MENU_MOVIE_FRAME_RATE_10_EN)
            case MOVIE_FRAME_RATE_10FPS:
                sRet = MMPS_3GPRECD_SetBitrate(VideoConfig->ulFps30BitrateMap[usResolIdx][usQuality]/3);
            break;
        #endif
        }
#else
        if( uiGetParkingModeEnable() == AHC_TRUE )
        {
            sRet = MMPS_3GPRECD_SetBitrate( AHC_VIDEO_MAXBITRATE_PRERECORD );
        }
        #if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
        else if( MenuSettingConfig()->uiGsensorSensitivity != GSENSOR_SENSITIVITY_OFF )
        {
            sRet = MMPS_3GPRECD_SetBitrate( AHC_VIDEO_MAXBITRATE_PRERECORD );
        }
        #endif
        else
        {
            UINT32 uiFrameRateType = 0;

            AIHC_GetMovieConfig(AHC_FRAME_RATE, &uiFrameRateType);

            if( uiFrameRateType == VIDRECD_FRAMERATE_60FPS )
            {
                sRet = MMPS_3GPRECD_SetBitrate(VideoConfig->ulFps60BitrateMap[usResolIdx][usQuality]);
            }
            else
            {
                sRet = MMPS_3GPRECD_SetBitrate(VideoConfig->ulFps30BitrateMap[usResolIdx][usQuality]);
            }
        }
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
#endif
    }

    if (AHC_TRUE == AHC_IsSpeakerEnable())
    {
        printc("TurnOff Speaker!!!\r\n");
        AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_FALSE);
    }

    #if (ENALBE_MAX_PROTECT_TIME)
    {
        /**
        ex: 8Mbps video. the max. loss is 300ms.

        period size = 300*8/8000 = 0.3MB = 300KB

        MMPF_FS_SetClearCachePeriod("SD:\\", 300*1024)
        */
        MMP_ULONG ulBitRate = 0;
        sRet = MMPS_3GPRECD_GetParameter(MMPS_3GPRECD_PARAMETER_BITRATE, &ulBitRate);       
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
        
        ulBitRate = ulBitRate * AHC_VIDEO_MAX_PROTECT_TIME / 1000 *128;

        AHC_SetClearCachePeriod(AHC_GetMediaPath(), ulBitRate);
    }
    #endif///< (ENALBE_MAX_PROTECT_TIME)

    //It has to be set before enter Preview mode
    if(bStartRecord == AHC_VIDRECD_INIT)
    	AIHC_SelectYFrameUsage();
    
    //////////////////////////////////////
    // Video Record Preview Init
    //////////////////////////////////////
    if (bStartRecord == AHC_VIDRECD_STOP || bStartRecord == AHC_VIDRECD_INIT)
    {
        if (bStartRecord == AHC_VIDRECD_INIT) {

			#if (UVC_HOST_VIDEO_ENABLE)
            USB_DETECT_PHASE 		USBCurrentStates = 0;
            AHC_BOOL ahcRet = AHC_TRUE;
            #endif
            UINT32                  ulFlickerMode = COMMON_FLICKER_50HZ;

#if (UVC_HOST_VIDEO_ENABLE)
            ahcRet = AHC_HostUVCVideoPreviewStop();
            ahcRet = AHC_HostUVCVideoEnable(AHC_FALSE);
#endif

            AIHC_DrawReservedOSD(AHC_TRUE);
            AHC_OSDSetActive(THUMBNAIL_OSD_FRONT_ID, AHC_FALSE);
            AIHC_DrawReservedOSD(AHC_FALSE);

#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
            sRet = MMPS_3GPRECD_DecMjpegPreviewStop(gsAhcCurrentSensor);
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
#endif        

#if (UVC_HOST_VIDEO_ENABLE)
            ahcRet = AHC_HostUVCResetFBMemory();
#endif
#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
            ahcRet = AHC_HostUVCResetMjpegToPreviewJpegBuf();
#endif        

            sRet = MMPS_3GPRECD_PreviewStop(gsAhcCurrentSensor);

            #if (SUPPORT_DUAL_SNR)
            MMPF_Sensor_EnableTV_Dec_Src_TypeDetection(MMP_FALSE);
            sRet |= MMPS_3GPRECD_2ndSnrPreviewStop(SCD_SENSOR);
            #endif                  
            
            AHC_SetVRWithWNR(AHC_FALSE);
               
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}             
            
            if (MMPS_Sensor_IsInitialized(gsAhcCurrentSensor) == MMP_FALSE)
            {
                MMPS_Sensor_SetPreviewDelayCount( gsAhcCurrentSensor, DISP_PREVIEW_DELAY_COUNT_PRM_SENSOR );
	#if (SUPPORT_DUAL_SNR)
                //Init second sensor before primary sensor for real board issue.  
                //Root casue is unknown yet. TBD.
                //MMPS_Sensor_SetPreviewDelayCount(SCD_SENSOR, DISP_PREVIEW_DELAY_COUNT_SCD_SENSOR );
                MMPS_Sensor_RegisterCallback( MMPS_IVA_EVENT_TV_SRC_TYPE, (void*)SNTvSrcTypeCB );
                sRet = MMPS_Sensor_Initialize(SCD_SENSOR, 0, MMP_SNR_VID_PRW_MODE);
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); /*return AHC_FALSE;*/} //TBD.  If rear sensor is not connected, skip second stream record. 
    #endif          
                sRet = MMPS_Sensor_Initialize(gsAhcCurrentSensor, AHC_GetPresetSensorMode(), MMP_SNR_VID_PRW_MODE);
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
            }

            if( AHC_Menu_SettingGetCB( (char *)COMMON_KEY_FLICKER, &ulFlickerMode ) == AHC_TRUE ) {
                if (COMMON_FLICKER_60HZ == ulFlickerMode) {
                	AHC_Set_SensorLightFreq(AHC_SENSOR_VIDEO_DEBAND_60HZ);
                }
                else {
                	AHC_Set_SensorLightFreq(AHC_SENSOR_VIDEO_DEBAND_50HZ);
                }
            }
            else {
            	AHC_Set_SensorLightFreq(AHC_SENSOR_VIDEO_DEBAND_60HZ);
            }

            {
                UINT8 ubEv = COMMON_EVVALUE_00;

                if( AHC_Menu_SettingGetCB( (char *)COMMON_KEY_EV, &ubEv ) == AHC_TRUE ) {
                    Menu_SetEV(ubEv);
                }
            }

            AHC_GetDisplayOutputDev(&OutputDevice);
            sRet = MMPS_Display_SetOutputPanel(MMP_DISPLAY_PRM_CTL, OutputDevice);
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

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

            sRet = MMPS_3GPRECD_SetPreviewMode(usVideoPreviewMode);
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}             

            {
    		    MMP_IBC_PIPEID ubPipe;

                sRet = MMPS_3GPRECD_GetPreviewPipe(gsAhcCurrentSensor, &ubPipe);            
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
                
                if (gsStillZoomIndex == 0xFFFF) {
                    MMPS_3GPRECD_GetCurZoomStep(ubPipe, &zoomIndex);
                }
                else {
                    zoomIndex = gsStillZoomIndex;
                    gsStillZoomIndex = 0xFFFF;
                }

                #if (VIDEO_CAMERA_DZOOM_SYNC)
                sRet = MMPS_3GPRECD_SetPreviewZoom(MMPS_3GPRECD_ZOOM_PATH_PREV, MMP_PTZ_ZOOMIN, zoomIndex);
                #else
                sRet = MMPS_3GPRECD_SetPreviewZoom(MMPS_3GPRECD_ZOOM_PATH_PREV, MMP_PTZ_ZOOMOUT, 0);
                #endif
                if (sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
            }

			#if (SUPPORT_LDC_RECD)
            {
                UINT16 usRecdModeIdx = AIHC_GetCurrentRecordModeIdx();

                if (usRecdModeIdx == VIDRECD_RESOL_1920x1088 ||
                	usRecdModeIdx == VIDRECD_RESOL_1280x720 ||
                    usRecdModeIdx == VIDRECD_RESOL_848x480) {

                    MMPS_3GPRECD_GetConfig()->previewpath[0] = MMP_3GP_PATH_LDC_LB_SINGLE;
                    //MMPS_3GPRECD_GetConfig()->previewpath[0] = MMP_3GP_PATH_LDC_LB_MULTI;
                    //MMPS_3GPRECD_GetConfig()->previewpath[0] = MMP_3GP_PATH_LDC_MULTISLICE;

                    if (usRecdModeIdx == VIDRECD_RESOL_1920x1088) {
                        sRet = MMPS_3GPRECD_SetLdcResMode(MMP_LDC_RES_MODE_FHD, MMP_LDC_FPS_MODE_30P);
                    }
                    else if (usRecdModeIdx == VIDRECD_RESOL_1280x720) {
                        sRet = MMPS_3GPRECD_SetLdcResMode(MMP_LDC_RES_MODE_HD, MMP_LDC_FPS_MODE_60P);
                    }
                    else if (usRecdModeIdx == VIDRECD_RESOL_848x480) {
                        sRet = MMPS_3GPRECD_SetLdcResMode(MMP_LDC_RES_MODE_WVGA, MMP_LDC_FPS_MODE_30P);
                    }

                    if (MMPS_3GPRECD_GetConfig()->previewpath[0] == MMP_3GP_PATH_LDC_LB_SINGLE) {
                        sRet = MMPS_3GPRECD_SetLdcRunMode(MMP_LDC_RUN_MODE_SINGLE_RUN);
                    }
                    else if (MMPS_3GPRECD_GetConfig()->previewpath[0] == MMP_3GP_PATH_LDC_LB_MULTI) {
                        sRet = MMPS_3GPRECD_SetLdcRunMode(MMP_LDC_RUN_MODE_MULTI_RUN);
                    }
                    else if (MMPS_3GPRECD_GetConfig()->previewpath[0] == MMP_3GP_PATH_LDC_MULTISLICE) {
                        sRet = MMPS_3GPRECD_SetLdcRunMode(MMP_LDC_RUN_MODE_MULTI_SLICE);
                    }
                }
                else {
                    MMPS_3GPRECD_GetConfig()->previewpath[0] = MMP_3GP_PATH_2PIPE;
                    sRet = MMPS_3GPRECD_SetLdcRunMode(MMP_LDC_RUN_MODE_DISABLE);
                }

                if (usVideoPreviewMode == VIDRECD_HDMI_PREVIEW_MODE) {
                    MMPS_3GPRECD_GetConfig()->previewpath[0] = MMP_3GP_PATH_2PIPE;
                    sRet = MMPS_3GPRECD_SetLdcRunMode(MMP_LDC_RUN_MODE_DISABLE);
                }
                
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
            }
			#else
            MMPS_3GPRECD_GetConfig()->previewpath[0] = MMP_3GP_PATH_2PIPE;
			#endif
            //To Check SNR Flip and set dir
            {   
                AHC_BOOL ubSnrFlipEn;
                ubSnrFlipEn = AHC_CheckSNRFlipEn(CTRL_BY_ALL)&& AHC_GetSensorStatus();
		        AHC_SetKitDirection(AHC_LCD_NOFLIP, AHC_FALSE, AHC_GetSensorStatus(), ubSnrFlipEn);
            }
            //////////////////////////////////////////////////////////////////
            // Start Preview
            //////////////////////////////////////////////////////////////////
#if(UVC_HOST_VIDEO_ENABLE == 1)
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0)
            sRet = MMPS_3GPRECD_SetUVCPrevwRote(MMP_GRAPHICS_ROTATE_NO_ROTATE);
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90)
            sRet = MMPS_3GPRECD_SetUVCPrevwRote(MMP_GRAPHICS_ROTATE_RIGHT_90);
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
            sRet = MMPS_3GPRECD_SetUVCPrevwRote(MMP_GRAPHICS_ROTATE_RIGHT_270);
#endif
            if (sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}
#endif
			#if (CCIR656_OUTPUT_ENABLE) 
			//MMPS_CCIR_DisableDisplay();//To avoid display abnormal at first of switch preview mode
			#endif

            sRet = MMPS_3GPRECD_PreviewStart(gsAhcCurrentSensor, MMP_FALSE);
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}             

            {
            	MMP_ULONG dw, dh;
            	MMPS_3GPRECD_GetPreviewSize(gsAhcCurrentSensor, &dw, &dh);
                rect.usLeft     = 0;
                rect.usTop      = 0;
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0)
                rect.usWidth    = dw;
                rect.usHeight   = dh;
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
                rect.usWidth    = dh;
                rect.usHeight   = dw;
#endif
                AHC_PreviewWindowOp(AHC_PREVIEW_WINDOW_OP_SET | AHC_PREVIEW_WINDOW_FRONT,&rect);

            }
			
#if (SUPPORT_DUAL_SNR)
            {   //Wait until TV Src type ready.
                UINT32 ulTVDecTypeTimeout = 10;
                MMPF_VIF_TV_DEC_SRC_TYPE sTVDecSrc = MMPF_VIF_SRC_NO_READY;
                do{              
                    MMPF_Sensor_CheckTV_Dec_Src_Type(&sTVDecSrc);
                    if(MMPF_VIF_SRC_NO_READY == sTVDecSrc){
                        AHC_OS_Sleep(10);
                    }
                    else{
                        break;
                    }
                }while((MMPF_VIF_SRC_NO_READY == sTVDecSrc) && (--ulTVDecTypeTimeout > 0));

                if(MMPF_VIF_SRC_PAL == sTVDecSrc){
                    AHC_PresetTVDecSrcType(AHC_SENSOR_MODE_PAL_25FPS);
                    MMPS_Sensor_SetPreviewMode(SCD_SENSOR, AHC_GetPresetTVDecSrcType());
                }
                else if(MMPF_VIF_SRC_NTSC == sTVDecSrc){
                    AHC_PresetTVDecSrcType(AHC_SENSOR_MODE_NTSC_30FPS);
                    MMPS_Sensor_SetPreviewMode(SCD_SENSOR, AHC_GetPresetTVDecSrcType());
                }
                else {
                    printc(FG_GREEN("%s,%d warning! SNR2 check signal type TimeOut.\r\n"), __func__, __LINE__);
                }
                
            }

            MMPF_Sensor_EnableTV_Dec_Src_TypeDetection(MMP_TRUE);

			if(AHC_FALSE == AHC_GetSecondSensorCnntStatus())
				MMPS_Sensor_SetPreviewDelayCount( SCD_SENSOR, DISP_DISABLE_PREVIEW_DELAY_COUNT );
			else
				MMPS_Sensor_SetPreviewDelayCount( SCD_SENSOR, DISP_PREVIEW_DELAY_COUNT_SCD_SENSOR );
            
            if(AHC_TRUE/* == AHC_GetSecondSensorCnntStatus()*/){//bit1603 must be connect
                sRet = MMPS_3GPRECD_2ndSnrPreviewStart(SCD_SENSOR, MMP_FALSE);
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); /*return AHC_FALSE;*/}  //TBD.  If rear sensor is not connected, skip second stream record.              

                {
                    MMP_ULONG dw, dh;
                    MMPS_3GPRECD_GetPreviewSize(SCD_SENSOR, &dw, &dh);
                    rect.usLeft     = 0;
                    rect.usTop      = 0;
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0)
                    rect.usWidth    = dw;
                    rect.usHeight   = dh;
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
                    rect.usWidth    = dh;
                    rect.usHeight   = dw;
#endif
                    AHC_PreviewWindowOp(AHC_PREVIEW_WINDOW_OP_SET | AHC_PREVIEW_WINDOW_REAR,&rect);
                }
            
#if (DUALENC_SUPPORT == 1)
                if(sRet == MMP_ERR_NONE){
                    sRet = MMPS_3GPRECD_SetDualH264SnrId(SCD_SENSOR);
                    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}
                }
#endif
            }
#endif

    //Move to standalone funtion.
    AIHC_UVCPreviewStart(usVideoPreviewMode, OutputDevice, ulFlickerMode);

#if (MENU_MANUAL_EN && MENU_MANUAL_FACE_DETECT_EN)
            if (MenuSettingConfig()->uiFaceTouch == FACE_DETECT_ON) {
                sRet = MMPS_Sensor_SetFDEnable(MMP_TRUE);
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}                 
            }
#endif
			
			#if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE && MOTION_DETECTION_EN)
			if ( uiGetParkingModeEnable() == AHC_TRUE )
			{
			    UINT8 ubMvTh, ubCntTh;
			
			    if (MenuSettingConfig()->uiMotionDtcSensitivity == MOTION_DTC_SENSITIVITY_OFF)
			    {
			        MenuSettingConfig()->uiMotionDtcSensitivity = MOTION_DTC_SENSITIVITY_LOW;
			        Menu_SetMotionDtcSensitivity(MenuSettingConfig()->uiMotionDtcSensitivity);
			    
			    }
			        
			    AHC_GetMotionDtcSensitivity(&ubMvTh, &ubCntTh);
			    //printc("AIHC_SetVideoRecordMode: Motion ubMvTh %d, ubCntTh %d\r\n",ubMvTh, ubCntTh);
			
			    AHC_SetMotionDtcSensitivity( ubMvTh, ubCntTh );
			    sRet = MMPS_Sensor_RegisterCallback(MMPS_IVA_EVENT_MDTC, (void *)VRMotionDetectCB);
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}
			    AHC_EnableMotionDetection(AHC_TRUE);
			}
			else
			{
			    AHC_EnableMotionDetection(AHC_FALSE);
                AHC_SetMotionDetectionStatus(AHC_FALSE);
			}
			#elif ((SUPPORT_PARKINGMODE == PARKINGMODE_NONE) && MOTION_DETECTION_EN) // TBD
            if (MenuSettingConfig()->uiMotionDtcSensitivity != MOTION_DTC_SENSITIVITY_OFF)
            {
                UINT8 ubMvTh, ubCntTh;

                AHC_GetMotionDtcSensitivity(&ubMvTh, &ubCntTh);

                printc("AIHC_SetVideoRecordMode: Motion ubMvTh %d, ubCntTh %d\r\n", ubMvTh, ubCntTh);

                if ((ubMvTh == 0xFF) || (ubCntTh == 0xFF))
                {
                    AHC_EnableMotionDetection(AHC_FALSE);
                }
                else
                {
                    AHC_SetMotionDtcSensitivity(ubMvTh, ubCntTh);
                    sRet = MMPS_Sensor_RegisterCallback(MMPS_IVA_EVENT_MDTC, (void *)VRMotionDetectCB);
                    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}                     
                    AHC_EnableMotionDetection(AHC_TRUE);
                }
            }
            else
            {
                AHC_EnableMotionDetection(AHC_FALSE);
            }
			#endif
    	
		#if (ENABLE_ADAS_LDWS || ENABLE_ADAS_FCWS || ENABLE_ADAS_SAG)
		
			#if (MOTION_DETECTION_EN)
		    if (MMPS_Sensor_IsVMDEnable()== MMP_FALSE)
		    #else
		    if(1)
		    #endif
		    {
			    UINT32 uiAdasFlag = 0;
                AHC_GetParam( PARAM_ID_ADAS, &uiAdasFlag );

                #ifdef CFG_ADAS_MENU_SETTING_OLD_STYLE // temporary solution, it might be defined in Config_SDK.h
                if ((MenuSettingConfig()->uiLDWS == COMMON_LDWS_EN_ON) ||
                    (MenuSettingConfig()->uiFCWS == COMMON_FCWS_EN_ON) ||
                    (uiAdasFlag & AHC_ADAS_ENABLED_SAG))
                #else
                if((uiAdasFlag & AHC_ADAS_ENABLED_LDWS) || 
                   (uiAdasFlag & AHC_ADAS_ENABLED_FCWS) || 
                   (uiAdasFlag & AHC_ADAS_ENABLED_SAG ) )
                #endif
				{
                    AHC_EnableADAS(AHC_TRUE);
				}
				else
				{
                    AHC_EnableADAS(AHC_FALSE);
				}
			}
		#endif    
		    
        }

        AHC_SetFastAeAwbMode( AHC_TRUE );

        if(bStartRecord == AHC_VIDRECD_INIT){
#if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
            if (uiGetParkingModeEnable())
#else
            if (MenuSettingConfig()->uiMOVPreRecord == MOVIE_PRE_RECORD_ON)
#endif
            {
#if defined(PARKINGMODE_STYLE_4) && (SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_4) //SZ          
                printc("%s,%d,Do not pre-encode in parking mode 4.\r\n",__func__,__LINE__);
                return AHC_TRUE;
#endif
                // NOP
            }
            else
            {
                return AHC_TRUE;
            }
        }
        else if(bStartRecord == AHC_VIDRECD_STOP){
            return AHC_TRUE;
        }
    }

    //////////////////////////////////////
    // Start Video Record
    //////////////////////////////////////
    // Disable Fast AE/AWB when recording
    {
        UINT32 ulDCFInitTimeout = 0;
        AHC_BOOL bDCFInited = AHC_FALSE;

        ulDCFInitTimeout = 500; //Timeout value is 5 sec.
        do{
            AHC_OS_SleepMs(10);
            bDCFInited = AHC_UF_IsDBMounted();
        }while((AHC_FALSE == bDCFInited) && (--ulDCFInitTimeout > 0));

        if(0 == ulDCFInitTimeout){ printc(FG_RED("\r\n[Wait DCF init timeout!]\r\n")); }
    }
    
    AHC_SetFastAeAwbMode( AHC_FALSE );

    #if (AHC_ENABLE_VIDEO_STICKER)
    {
        UINT32  uiTimeStampOp;

        AHC_GetCaptureConfig(ACC_DATE_STAMP, &uiTimeStampOp);

        if (uiTimeStampOp & AHC_ACC_TIMESTAMP_ENABLE_MASK)
        {
            UINT32 movieSize = 0;

            AHC_Menu_SettingGetCB((char *) COMMON_KEY_MOVIE_SIZE, &movieSize);

            {
                AHC_RTC_TIME sRtcTime;

                AHC_RTC_GetTime(&sRtcTime);
                AIHC_ConfigVRTimeStamp(uiTimeStampOp, &sRtcTime, MMP_TRUE);
            }
        }
    }
    #endif///<(AHC_ENABLE_VIDEO_STICKER)

    {
        UINT32 movieSize = 0;
        MMPS_3GPRECD_FRAMERATE sSensorFps, sEncodeFps, sContainerFps;
        AHC_BOOL bSlowMotionEnable = AHC_FALSE;
        AHC_BOOL bVideoTimeLapseEnable = AHC_FALSE;
        UINT32 usSlowMotionTimeIncrement, usSlowMotionTimeIncrResol;
        UINT32 usVideoTimeLapseTimeIncrement, usVideoTimeLapseTimeIncrResol;
            
        AHC_Menu_SettingGetCB((char *) COMMON_KEY_MOVIE_SIZE, &movieSize);

        if ((usResolIdx == VIDRECD_RESOL_1920x1088) || (usResolIdx == VIDRECD_RESOL_1440x1088)||(usResolIdx == VIDRECD_RESOL_640x368)) {
            sRet = MMPS_3GPRECD_SetCropping(0/*ulEncId*/, 0, 8, 0, 0);
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}
            sRet = MMPS_3GPRECD_SetPadding(0/*ulEncId*/, MMPS_3GPRECD_PADDING_REPEAT, 8);
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}
        }
        else {
            sRet = MMPS_3GPRECD_SetCropping(0/*ulEncId*/, 0, 0, 0, 0);
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}            
            sRet = MMPS_3GPRECD_SetPadding(0/*ulEncId*/, MMPS_3GPRECD_PADDING_NONE, 0);
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}            
        }

        AIHC_GetMovieConfig(AHC_VIDEO_CODEC_TYPE, &Param);
        VideoFmt        = Param;

        AIHC_GetMovieConfig(AHC_AUD_CODEC_TYPE, &Param);
        AudioFmt        = Param;

        AIHC_GetMovieConfig(AHC_CLIP_CONTAINER_TYPE, &Param);
        ContainerType   = Param;

        AIHC_GetMovieConfig(AHC_MAX_PFRAME_NUM, &Param);
        PFrameNum       = Param;

        AIHC_GetMovieConfig(AHC_MAX_BFRAME_NUM, &Param);
        BFrameNum       = Param;

        AIHC_GetMovieConfig(AHC_AUD_SAMPLE_RATE, &Param);
        ulSamplerate    = Param;

        AIHC_GetMovieConfig(AHC_AUD_CHANNEL_CONFIG, &Param);
        AudioLineIn     = Param;

        AIHC_GetMovieConfig(AHC_FRAME_RATE, &Param);
        FrameRate       = Param;

        sRet = MMPS_3GPRECD_GetParameter(MMPS_3GPRECD_PARAMETER_BITRATE, &ulBitRate);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
        
        if (bAudioOn) {
            AIHC_InitAudioExtDACIn();
        }

        sRet = MMPS_3GPRECD_SetContainerType(ContainerType);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

        sRet = MMPS_3GPRECD_SetVideoFormat  (VideoFmt);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

#if (VR_VIDEO_CURBUF_ENCODE_MODE == COMMON_VR_VIDEO_CURBUF_RT)
        sRet = MMPS_3GPRECD_SetCurBufMode(MMPS_3GPRECD_CURBUF_RT);
        //if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}  //Remove the check for pre-recording in Parking mode
#else
        sRet = MMPS_3GPRECD_SetCurBufMode(MMPS_3GPRECD_CURBUF_FRAME);
        //if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}  //Remove the check for pre-recording in Parking mode
#endif

        #if 1
        printc("VR:...VideoFmt=%d \r\n", VideoFmt);
        printc("VR:...AudioFmt=%d \r\n", AudioFmt);
        printc("VR:...ContainerType=%d \r\n", ContainerType);
        printc("VR:...PFrameNum=%d \r\n", PFrameNum);
        printc("VR:...BFrameNum=%d \r\n", BFrameNum);
        printc("VR:...ulSamplerate=%d \r\n", ulSamplerate);
        printc("VR:...AudioLineIn=%d \r\n", AudioLineIn);
        printc("VR:...FrameRate=%d \r\n", FrameRate);
        printc("VR:...uiMOVSize=%d \r\n", movieSize);
        printc("VR:...ulBitRate=%d \r\n", ulBitRate);
        printc("VR:...uiMOVPreRecord=%d \r\n", MenuSettingConfig()->uiMOVPreRecord);
        #endif

        FrameRate = AHC_GetVideoRealFrameRate(FrameRate);        
        sSensorFps.usVopTimeIncrement = AHC_VIDRECD_TIME_INCREMENT_BASE;
        sSensorFps.usVopTimeIncrResol = FrameRate;

        AHC_VideoGetSlowMotionFPS(&bSlowMotionEnable, &usSlowMotionTimeIncrement, &usSlowMotionTimeIncrResol);
        #if (defined(VIDEO_REC_TIMELAPSE_EN) && VIDEO_REC_TIMELAPSE_EN)
        AHC_VideoGetTimeLapseFPS(&bVideoTimeLapseEnable, &usVideoTimeLapseTimeIncrement, &usVideoTimeLapseTimeIncrResol);
        #endif

#if (UVC_HOST_VIDEO_ENABLE) 
        {
            USB_DETECT_PHASE USBCurrentStates = 0;

            AHC_USBGetStates(&USBCurrentStates,AHC_USB_GET_PHASE_CURRENT);
            if ((USBCurrentStates == USB_DETECT_PHASE_REAR_CAM) && 
                (AHC_TRUE == AHC_HostUVCVideoIsEnabled())){
                    bSlowMotionEnable = AHC_FALSE;
                    bVideoTimeLapseEnable = AHC_FALSE;
                    printc(FG_RED("%s,%d Now dual cam does not support timelapse and slowmotion!\r\n"),__func__,__LINE__);
            }
        }
#elif (SUPPORT_DUAL_SNR == 1 && DUALENC_SUPPORT == 1)
        bSlowMotionEnable = AHC_FALSE;
        bVideoTimeLapseEnable = AHC_FALSE;
        printc(FG_RED("%s,%d Now dual cam does not support timelapse and slowmotion!\r\n"),__func__,__LINE__);
#endif

        if (bSlowMotionEnable) {
            // Slow Motion Record
            sContainerFps.usVopTimeIncrement = usSlowMotionTimeIncrement;
            sContainerFps.usVopTimeIncrResol = usSlowMotionTimeIncrResol;        
            if((sSensorFps.usVopTimeIncrement * sSensorFps.usVopTimeIncrResol) < (sContainerFps.usVopTimeIncrement * sContainerFps.usVopTimeIncrResol)){
                printc(FG_RED("%s,%d SlowMotion error!%d,%d,%d,%d\r\n"), sSensorFps.usVopTimeIncrement, sSensorFps.usVopTimeIncrResol, sContainerFps.usVopTimeIncrement, sContainerFps.usVopTimeIncrResol);
                sContainerFps.usVopTimeIncrement = sSensorFps.usVopTimeIncrement;
                sContainerFps.usVopTimeIncrResol = sSensorFps.usVopTimeIncrResol;   
            }
        }
        else {
            // Normal Record (Non-Slow Motion)
            sContainerFps.usVopTimeIncrement = sSensorFps.usVopTimeIncrement;
            sContainerFps.usVopTimeIncrResol = sSensorFps.usVopTimeIncrResol;
        }

        if (bVideoTimeLapseEnable) {
            // Time Lapse Record
            sEncodeFps.usVopTimeIncrement = usVideoTimeLapseTimeIncrement;
            sEncodeFps.usVopTimeIncrResol = usVideoTimeLapseTimeIncrResol;     
            if((sSensorFps.usVopTimeIncrement * sSensorFps.usVopTimeIncrResol) < (sEncodeFps.usVopTimeIncrement * sEncodeFps.usVopTimeIncrResol)){
                printc(FG_RED("%s,%d VideoTimeLapse error!%d,%d,%d,%d\r\n"), sSensorFps.usVopTimeIncrement, sSensorFps.usVopTimeIncrResol, sEncodeFps.usVopTimeIncrement, sEncodeFps.usVopTimeIncrResol);
                sEncodeFps.usVopTimeIncrement = sSensorFps.usVopTimeIncrement;
                sEncodeFps.usVopTimeIncrResol = sSensorFps.usVopTimeIncrResol;   
            }
        }
        else {
            // Normal Record (Non-VideoTimeLapse)
            sEncodeFps.usVopTimeIncrement = sSensorFps.usVopTimeIncrement;
            sEncodeFps.usVopTimeIncrResol = sSensorFps.usVopTimeIncrResol;
        }
        
        sRet = MMPS_3GPRECD_SetFrameRatePara(&sSensorFps, &sEncodeFps, &sContainerFps);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

        sRet = MMPS_3GPRECD_SetBFrameCount  (BFrameNum);
        //if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} //Temp remove it to avoid failure when Parking record
        
        sRet = MMPS_3GPRECD_SetPFrameCount  (PFrameNum);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

        #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
        if (uiGetParkingModeEnable())
        #else
        if (MenuSettingConfig()->uiMOVPreRecord == MOVIE_PRE_RECORD_ON)
        #endif
        {
            AHC_GetSoundEffectStatus(&ubSoundEffectStatus);

            if(ubSoundEffectStatus == AHC_SOUND_EFFECT_STATUS_START)
            MMPS_AUI_StopWAVPlay();

            AHC_ConfigMovie(AHC_AUD_PRERECORD_DAC, AHC_FALSE);

            sRet = MMPS_3GPRECD_SetAudioRecMode(MMPS_3GPRECD_REC_AUDIO_DATA);
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
        }
        else {
            if (bAudioOn){
                sRet = MMPS_3GPRECD_SetAudioRecMode(MMPS_3GPRECD_REC_AUDIO_DATA);
            }
            else{
                sRet = MMPS_3GPRECD_SetAudioRecMode(MMPS_3GPRECD_NO_AUDIO_DATA);
            }
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

            AHC_ConfigMovie(AHC_AUD_PRERECORD_DAC, AHC_TRUE);
        }

        MMPS_AUDIO_SetLineInChannel(AudioLineIn);

        if(AudioFmt == MMPS_3GPRECD_AUDIO_FORMAT_AMR) {
            sRet = MMPS_3GPRECD_SetAudioFormat(AudioFmt, AHC_AMR_AUDIO_OPTION);
        }
        else if (AudioFmt == MMPS_3GPRECD_AUDIO_FORMAT_AAC) {
            sRet = MMPS_3GPRECD_SetAudioFormat(AudioFmt, AHC_AAC_AUDIO_OPTION);
        }
        else if (AudioFmt == MMPS_3GPRECD_AUDIO_FORMAT_ADPCM) {
            sRet = MMPS_3GPRECD_SetAudioFormat(AudioFmt, AHC_ADPCM_AUDIO_OPTION);
        }
        else if (AudioFmt == MMPS_3GPRECD_AUDIO_FORMAT_MP3){
            sRet = MMPS_3GPRECD_SetAudioFormat(AudioFmt, AHC_MP3_AUDIO_OPTION);
        }
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
        
        AHC_Menu_SettingGetCB((char *)COMMON_KEY_WNR_EN, &ubWMR);
		#ifdef COMMON_KEY_RECORD_WITH_AUDIO
		AHC_Menu_SettingGetCB((char *)COMMON_KEY_RECORD_WITH_AUDIO, &ubRecordWithAudio);
		#else
		AHC_Menu_SettingGetCB((char *)"RecordWithAudio", &ubRecordWithAudio);
		#endif

        if ((ubWMR == WNR_ON) && bAudioOn && (ubRecordWithAudio == MOVIE_SOUND_RECORD_ON)) {
            AHC_SetVRWithWNR(AHC_TRUE);
		}
		else {
			AHC_SetVRWithWNR(AHC_FALSE);
		}	
        
        #if (VR_SEAMLESS)
        // Setting the size of VR tail buffer
        TimeLimit = AHC_GetVRTimeLimit();
        AHC_SetVRTimeLimit(TimeLimit);

#if 0 //Disable it temporary
        if(TimeLimit == NON_CYCLING_TIME_LIMIT/*Non cycle record*/)
        {
            MMP_ULONG ulSizeByFTime;

            ulSizeByFTime = MMPS_3GPRECD_GetContainerTailBufSize();

            // NON_CYCLING_TIME_LIMIT: The size of the tail buffer is difficult to predict, try and erro.
            if (ulSizeByFTime < 0x7FFFFFFF)
                MMPS_3GPRECD_GetConfig()->ulTailBufSize = (ulSizeByFTime * 3) >> 1;     // add 50% margin
            else
                MMPS_3GPRECD_GetConfig()->ulTailBufSize = ulSizeByFTime;
        }
        else
        {
            // 0: Tail buffer will be calculated by kernel.
            MMPS_3GPRECD_GetConfig()->ulTailBufSize = 0;
        }
        #endif
#endif
    }

    if(bStartRecord == AHC_VIDRECD_INIT)
    {
        #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
        if (uiGetParkingModeEnable())
        #else
        if (MenuSettingConfig()->uiMOVPreRecord == MOVIE_PRE_RECORD_ON)
        #endif
        {
#if (defined(PARKINGMODE_STYLE_4) && (SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_4)) //SZ       
            //NOP
#else
            AIHC_GetMovieConfig(AHC_VIDEO_PRERECORD_LENGTH,&Param);
            printc("Enable Video Pre Record: %d ms\r\n", Param);
            AHC_SetFastAeAwbMode( AHC_TRUE );
            sRet = MMPS_3GPRECD_EnableEmergentRecd(AHC_FALSE);
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
            ulRealPreencodeTime = Param;
#if (UVC_HOST_VIDEO_ENABLE && DUALENC_SUPPORT) 
			{
				USB_DETECT_PHASE USBCurrentStates = 0;
	            AHC_USBGetStates(&USBCurrentStates,AHC_USB_GET_PHASE_CURRENT);
	            if ((USBCurrentStates == USB_DETECT_PHASE_REAR_CAM) && 
				(AHC_TRUE == AHC_HostUVCVideoIsEnabled())){
	                sRet = MMPS_3GPRECD_SetDualH264Enable(MMP_TRUE, MMP_VIDRECD_USEMODE_RECD);
					if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
					AHC_HostUVCVideoDualEncodeSetting();

	            	MMPS_3GPRECD_GetAllEncPreRecordTime(Param, &ulRealPreencodeTime);
				}
			}
#endif            
            sRet = MMPS_3GPRECD_PreRecord(ulRealPreencodeTime);
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
#if (UVC_HOST_VIDEO_ENABLE == 1 && DUALENC_SUPPORT == 1) 
            {
                USB_DETECT_PHASE USBCurrentStates = 0;

                AHC_USBGetStates(&USBCurrentStates,AHC_USB_GET_PHASE_CURRENT);
                if ((USBCurrentStates == USB_DETECT_PHASE_REAR_CAM) && 
                    (AHC_TRUE == AHC_HostUVCVideoIsEnabled())){
                    if (MMPS_3GPRECD_DualEncPreRecord(ulRealPreencodeTime)){ 
                        AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); 
                        return AHC_FALSE;
                    }
                }
            }
#elif (SUPPORT_DUAL_SNR == 1 && DUALENC_SUPPORT == 1)
			if(AHC_TRUE == AHC_GetSecondSensorCnntStatus())//bit1603 must be connect
			{
	            if (!MMPS_3GPRECD_DualEncPreRecord(ulRealPreencodeTime)){ 
	                AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); 
	                return AHC_FALSE;
	            }
            }
#endif
            AHC_VideoRecordStartWriteInfo();
#endif            
        }
        return AHC_TRUE;
    }

    /**
    @brief the following code is run when bStartRecord is MMP_TRUE.
    */

    #if (C005_PARKING_CLIP_Length_10PRE_10REC == 1) && (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
    if(uiGetParkingModeEnable())
    {
        #if (GPS_RAW_FILE_EMER_EN == 1)
        GPSCtrl_SetGPSRawWriteFirst(AHC_FALSE);
        #endif
    }
    #endif

    sRet = MMPS_3GPRECD_SetStoragePath(MMPS_3GPRECD_SRCMODE_CARD);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

    MEMSET(bFileName       , 0, sizeof(bFileName));

#if ((UVC_HOST_VIDEO_ENABLE == 1 || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)    
    MEMSET(bRearFileName   , 0, sizeof(bRearFileName));
#endif
    MEMSET(DirName         , 0, sizeof(DirName));

    #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
    if(uiGetParkingModeEnable())
        MEMSET(m_VideoParkFileName, 0, sizeof(m_VideoParkFileName));
    else
    #endif
        MEMSET(m_VideoRFileName, 0, sizeof(m_VideoRFileName));
#if ((UVC_HOST_VIDEO_ENABLE == 1  || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)    
    MEMSET(m_RearVideoRFileName, 0, sizeof(m_RearVideoRFileName));
#endif
    #if (ENABLE_VIDEO_ERR_LOG)
    MEMSET(m_VideoRecordErrorFileName , 0, sizeof(m_VideoRecordErrorFileName));
    #endif ///< (ENABLE_VIDEO_ERR_LOG)

#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_NORMAL)
    #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
    if(uiGetParkingModeEnable())
    {
        bRet = AHC_UF_GetName( &m_usVideoParkDirKey, (INT8*)DirName, (INT8*)m_VideoParkFileName, &bCreateNewDir);
    }
    else
    #endif
    {
        bRet = AHC_UF_GetName( &m_usVideoRDirKey, (INT8*)DirName, (INT8*)m_VideoRFileName, &bCreateNewDir);
    }

    if (bRet  == AHC_TRUE ) {

        #if (!VR_SEAMLESS)
        UINT32 ulVideoReservedSize;
        #endif

        STRCPY(bFileName, (INT8*)AHC_UF_GetRootDirName());
        STRCAT(bFileName, "\\");
        STRCAT(bFileName, DirName);

        if ( bCreateNewDir ) {
            MMPS_FS_DirCreate((INT8*)bFileName, STRLEN(bFileName));
            AHC_UF_AddDir(DirName);
        }

        STRCAT(bFileName, "\\");

        #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
        if (uiGetParkingModeEnable())
            STRCAT(bFileName, (INT8*)m_VideoParkFileName);
        else
        #endif
            STRCAT(bFileName, (INT8*)m_VideoRFileName);
    }
#elif (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
    {
        MMP_ULONG ulRecordingOffset = 0;
        int   nSecondOffset;

        AHC_RTC_GetTime(&m_VideoRecStartRtcTime);

        #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
        if(uiGetParkingModeEnable() == AHC_TRUE)
        {
            sRet = MMPS_3GPRECD_Get3gpRecordingOffset (&ulRecordingOffset);
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

            nSecondOffset = -1*(ulRecordingOffset/1000);
            AHC_DT_ShiftRtc( &m_VideoRecStartRtcTime, nSecondOffset);
            bRet = AHC_UF_GetName2(&m_VideoRecStartRtcTime, (INT8*)bFileName, (INT8*)m_VideoParkFileName, &bCreateNewDir);
        }
        else
#endif
        {
            bRet = AHC_UF_GetName2(&m_VideoRecStartRtcTime, (INT8*)bFileName, (INT8*)m_VideoRFileName, &bCreateNewDir);
        }
#if ((UVC_HOST_VIDEO_ENABLE == 1 || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)
		#if(UVC_HOST_VIDEO_ENABLE == 1)
		if(AHC_TRUE == AHC_HostUVCVideoIsEnabled())
		#else
		if(AHC_TRUE == AHC_GetSecondSensorCnntStatus())//bit1603 must be connect
		#endif
        {
            AHC_UF_SetRearCamPathFlag(AHC_TRUE);
            AHC_UF_GetName2(&m_VideoRecStartRtcTime, (INT8*)bRearFileName, (INT8*)m_RearVideoRFileName, &bCreateNewDir);
            printc("bRearFileName %s %d\r\n",bRearFileName,__LINE__);
            AHC_UF_SetRearCamPathFlag(AHC_FALSE);
        }
#endif
    }
#endif

    if (bRet  == AHC_TRUE ) {

        STRCAT(bFileName, EXT_DOT);
#if ((UVC_HOST_VIDEO_ENABLE == 1 || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)        
        STRCAT(bRearFileName, EXT_DOT);
#endif
        #if(ENABLE_VIDEO_ERR_LOG)
        STRCPY(m_VideoRecordErrorFileName, bFileName);
        STRCAT(m_VideoRecordErrorFileName, LOG_FILE_EXTENSION);
        m_VideoErrLog.callBackError               = 0;///< MMPS_3GPRECD_EVENT
        m_VideoErrLog.errorParameter1             = 0;
        m_VideoErrLog.errorParameter2             = 0;
        m_VideoErrLog.fwOperationBeforeStopRecord = 0;///< MMPS_FW_OPERATION
        m_VideoErrLog.fwOperationAfterStopRecord  = 0;///< MMPS_FW_OPERATION
        m_VideoErrLog.retStatusBeforeStopRecord   = 0;///< MMP_ERR
        m_VideoErrLog.retStatusAfterStopRecord    = 0;
        m_VideoErrLog.txStatusBeforeStopRecord    = 0;
        m_VideoErrLog.txStatusAfterStopRecord     = 0;
        #endif///< (ENABLE_VIDEO_ERR_LOG)

        AIHC_GetMovieConfig(AHC_CLIP_CONTAINER_TYPE, &Param);
        ContainerType   = Param;

        #if (GPS_CONNECT_ENABLE == 1)
        if(AHC_GPS_Module_Attached())
        {
            #if (GPS_KML_FILE_ENABLE == 1)
            MEMSET(bKMLFileName, 0, sizeof(bKMLFileName));
            STRCPY(bKMLFileName, bFileName);
            STRCAT(bKMLFileName, GPS_KML_FILE_EXTENTION);
            #endif
            #if (GPS_RAW_FILE_ENABLE == 1)
            MEMSET(bGPSRawFileName, 0, sizeof(bGPSRawFileName));
            STRCPY(bGPSRawFileName, bFileName);
            STRCAT(bGPSRawFileName, GPS_RAW_FILE_EXTENTION);
            #endif
        }
        #endif

        #if(GSENSOR_RAW_FILE_ENABLE == 1 && GPS_CONNECT_ENABLE == 0)
            MEMSET(bGPSRawFileName, 0, sizeof(bGPSRawFileName));
            STRCPY(bGPSRawFileName, bFileName);
            STRCAT(bGPSRawFileName, GPS_RAW_FILE_EXTENTION);
        #endif

        #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
        if(uiGetParkingModeEnable())
        {
            if(ContainerType == MMPS_3GPRECD_CONTAINER_3GP){
                STRCAT(bFileName, MOVIE_3GP_EXT);
#if ((UVC_HOST_VIDEO_ENABLE == 1 || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)                
                STRCAT(bRearFileName, MOVIE_3GP_EXT);
#endif
                STRCAT((INT8*)m_VideoParkFileName, EXT_DOT);
                STRCAT((INT8*)m_VideoParkFileName, MOVIE_3GP_EXT);
            }else{
                STRCAT(bFileName, MOVIE_AVI_EXT);
#if ((UVC_HOST_VIDEO_ENABLE == 1 || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)                
                STRCAT(bRearFileName, MOVIE_AVI_EXT);
#endif
                STRCAT((INT8*)m_VideoParkFileName, EXT_DOT);
                STRCAT((INT8*)m_VideoParkFileName, MOVIE_AVI_EXT);
            }
        }
        else
        #endif
        {
            if(ContainerType == MMPS_3GPRECD_CONTAINER_3GP){
                STRCAT(bFileName, MOVIE_3GP_EXT);
#if ((UVC_HOST_VIDEO_ENABLE == 1 || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)                
                STRCAT(bRearFileName, MOVIE_3GP_EXT);
#endif
                STRCAT((INT8*)m_VideoRFileName, EXT_DOT);
                STRCAT((INT8*)m_VideoRFileName, MOVIE_3GP_EXT);
            }else{
                STRCAT(bFileName, MOVIE_AVI_EXT);
#if ((UVC_HOST_VIDEO_ENABLE == 1 || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)
                STRCAT(bRearFileName, MOVIE_AVI_EXT);
#endif
                STRCAT((INT8*)m_VideoRFileName, EXT_DOT);
                STRCAT((INT8*)m_VideoRFileName, MOVIE_AVI_EXT);
            }
        }

#ifndef CAR_DV
        if(uiGetCurrentState() == UI_SYS_CALIBRATION_STATE) {
            #if (DUALENC_SUPPORT == 1)
            sRet = AHC_SetVideoFileName("SD:\\test\\VideoRec.MOV", STRLEN("SD:\\test\\VideoRec.MOV"), MMP_3GPRECD_FILETYPE_DUALENC);
            #else
            sRet = AHC_SetVideoFileName("SD:\\test\\VideoRec.MOV", STRLEN("SD:\\test\\VideoRec.MOV"), MMP_3GPRECD_FILETYPE_VIDRECD);
            #endif
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}             
        }
        else
#endif
        {
            sRet = AHC_SetVideoFileName(bFileName, STRLEN(bFileName), MMP_3GPRECD_FILETYPE_VIDRECD);
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}         
#if (UVC_HOST_VIDEO_ENABLE == 1 && VIDRECD_MULTI_TRACK == 0)
            if(AHC_TRUE == AHC_HostUVCVideoIsEnabled()){                
                MMP_3GPRECD_FILETYPE sFileType = MMP_3GPRECD_FILETYPE_UVCRECD;
#if (DUALENC_SUPPORT == 1) 
                sFileType = MMP_3GPRECD_FILETYPE_DUALENC;
#endif                
                sRet = AHC_SetVideoFileName(bRearFileName, STRLEN(bRearFileName), sFileType);
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}         
            }
#elif (SUPPORT_DUAL_SNR == 1 && VIDRECD_MULTI_TRACK == 0)      
			if(AHC_TRUE == AHC_GetSecondSensorCnntStatus())//bit1603 must be connect
			{
            	sRet = AHC_SetVideoFileName(bRearFileName, STRLEN(bRearFileName), MMP_3GPRECD_FILETYPE_DUALENC);
            	if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
			}
#endif
        }

        #if (GPS_CONNECT_ENABLE == 1)
        if(AHC_GPS_Module_Attached())
        {
            #if (GPS_KML_FILE_ENABLE == 1)
            GPSCtrl_SetKMLFileName(bKMLFileName,STRLEN(bKMLFileName));
            #endif
            #if (GPS_RAW_FILE_ENABLE == 1)
            {
                UINT8 bgps_en;

                if (AHC_Menu_SettingGetCB((char*)COMMON_KEY_GPS_RECORD_ENABLE, &bgps_en) == AHC_TRUE) {
                    switch (bgps_en) {
                    case RECODE_GPS_OFF:
                    case RECODE_GPS_IN_VIDEO:
                        // NOP
                        break;

                    default:
                        #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
                        #if ((EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE) || C005_PARKING_CLIP_Length_10PRE_10REC == 1)
                            #if(C005_PARKING_CLIP_Length_10PRE_10REC == 0)
                            MMP_ULONG ulRecordingOffset = 0;
                            sRet = MMPS_3GPRECD_Get3gpRecordingOffset(&ulRecordingOffset);
                            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
                                #if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
                                GPSCtrl_SetGPSRawBufTime_Normal(ulRecordingOffset/1000);
                                #endif
                            #else // #if(C005_PARKING_CLIP_Length_10PRE_10REC == 0)
                            if(uiGetParkingModeEnable())
                            {
                                MMP_ULONG ulRecordingOffset = 0;
                                sRet = MMPS_3GPRECD_Get3gpRecordingOffset(&ulRecordingOffset);
                                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
                                #if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
                                GPSCtrl_SetGPSRawBufTime_Normal(ulRecordingOffset/1000);
                                #endif
                            }
                            #endif
                        #endif
                        #endif

                        GPSCtrl_SetGPSRawFileName(bGPSRawFileName,STRLEN(bGPSRawFileName));
                        break;
                    }
                }
            }
            #endif
        }
        #endif

        #if(GSENSOR_RAW_FILE_ENABLE == 1 && GPS_CONNECT_ENABLE == 0)
        {
            #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
            #if (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE || C005_PARKING_CLIP_Length_10PRE_10REC == 1 )
                #if (C005_PARKING_CLIP_Length_10PRE_10REC == 0)
                MMP_ULONG ulRecordingOffset = 0;
                sRet = MMPS_3GPRECD_Get3gpRecordingOffset(&ulRecordingOffset);
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
                #if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
                GPSCtrl_SetGPSRawBufTime_Normal(ulRecordingOffset/1000);
                #endif
                #else// #if (C005_PARKING_CLIP_Length_10PRE_10REC == 0)
                if(uiGetParkingModeEnable())
                {
                    MMP_ULONG ulRecordingOffset = 0;
                    sRet = MMPS_3GPRECD_Get3gpRecordingOffset(&ulRecordingOffset);
                    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
                    #if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
                    GPSCtrl_SetGPSRawBufTime_Normal(ulRecordingOffset/1000);
                    #endif
                }
                #endif
            #endif
            #endif

            GPSCtrl_SetGPSRawFileName(bGPSRawFileName,STRLEN(bGPSRawFileName));
        }
        #endif

        #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
        #if ((EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE || C005_PARKING_CLIP_Length_10PRE_10REC == 1) && GPS_RAW_FILE_ENABLE == 1)
            #if (C005_PARKING_CLIP_Length_10PRE_10REC == 0)
            GPSCtrl_GPSRawWriteFirst_Normal();
            GPSCtrl_GPSRawResetBuffer();
            #else
            if(uiGetParkingModeEnable())
            {
                GPSCtrl_GPSRawWriteFirst_Normal();
                GPSCtrl_GPSRawResetBuffer();
            }
            #endif
        #endif
        #endif

        #if (USE_INFO_FILE)
        STRCPY(m_InfoLog.DCFCurVideoFileName, bFileName);
        #endif

        RTNA_DBG_Str(0, bFileName);
        RTNA_DBG_Str(0, "\r\n");

        #if (ENABLE_VIDEO_ERR_LOG)
        RTNA_DBG_Str(0, m_VideoRecordErrorFileName);
        RTNA_DBG_Str(0, "\r\n");
        #endif

#if (VR_SLOW_CARD_DETECT)
        sRet = MMPS_3GPRECD_SetVidRecdSkipModeParam(VRCB_TOTAL_SKIP_FRAME, VRCB_CONTINUOUS_SKIP_FRAME);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
#endif

        #if (VR_SEAMLESS)
#if (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_DUAL_FILE)
        sRet = MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_SLOWCARDSTOPVIDRECD,  (void*)VRVidRecdCardSlowStopCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
#endif
#if (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE)
        //MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_APSTOPVIDRECD,      (void*)VRAPStopVideoRecordCB);
#endif
        sRet = MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_MEDIA_FULL,   (void*)VRMediaFullCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

        sRet = MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_FILE_FULL,    (void*)VRFileFullCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}         

        sRet = MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_LONG_TIME_FILE_FULL,    (void*)VRLongTimeFileFullCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}

        sRet = MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_MEDIA_ERROR,  (void*)VRMediaErrorCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}         

        sRet = MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_MEDIA_SLOW,   (void*)VRMediaSlowCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

#if (VR_SLOW_CARD_DETECT)
        sRet = MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_PREGETTIME_CARDSLOW,  (void*)VRPreGetTimeWhenCardSlowCB); 
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

        sRet = MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_RECDSTOP_CARDSLOW,      (void*)VRRecdStopWhenCardSlowCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}         
#endif

        #if(SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
        if(!uiGetParkingModeEnable())
        #endif
        sRet = MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_SEAMLESS ,    (void*)VRSeamlessCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

        sRet = MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_ENCODE_START, (void*)VRStartCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

        sRet = MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_ENCODE_STOP,  (void*)VRStopCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

        sRet = MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_POSTPROCESS,  (void*)VRPostProcessCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

        sRet = MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_EMERGFILE_FULL,   (void*)VREmerDoneCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

#if (DUALENC_SUPPORT == 1)
        sRet = MMPS_3GPRECD_RegisterCallback(MMPS_3GPRECD_EVENT_DUALENC_FILE_FULL, (void *)DualEncRecordCbFileFullCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

        sRet = MMPS_3GPRECD_RegisterCallback(MMPS_3GPRECD_EVENT_DUALENCODE_START, 	(void *)VideoRecordCbEncStart);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

        sRet = MMPS_3GPRECD_RegisterCallback(MMPS_3GPRECD_EVENT_DUALENCODE_STOP, 	(void *)VideoRecordCbEncStop);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

        sRet = MMPS_3GPRECD_RegisterCallback(MMPS_3GPRECD_EVENT_STREAMCB, 		(void *)VideoRecordCbStreaming);        
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
#endif        

        #else///< (VR_SEAMLESS)
        sRet = MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_MEDIA_FULL,   (void*)VRMediaFullCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

        sRet = MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_FILE_FULL,    (void*)VRFileFullCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

        sRet = MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_MEDIA_SLOW,   (void*)VRMediaSlowCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

        sRet = MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_MEDIA_ERROR,  (void*)VRMediaErrorCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

        sRet = MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_ENCODE_START, (void*)VRStartCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

        sRet = MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_ENCODE_STOP,  (void*)VRStopCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

        sRet = MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_POSTPROCESS,  (void*)VRPostProcessCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

        sRet = MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_EMERGFILE_FULL,   (void*)VREmerDoneCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

#if (DUALENC_SUPPORT == 1)
        sRet = MMPS_3GPRECD_RegisterCallback(MMPS_3GPRECD_EVENT_DUALENC_FILE_FULL, (void *)DualEncRecordCbFileFullCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
#endif                
        #endif///< (VR_SEAMLESS)

        #if (VR_SEAMLESS)

        TimeLimit = AHC_GetVRTimeLimit();
        printc("%s,%d,TimeLimit:%d\r\n",__func__,__LINE__,TimeLimit);

        if (AHC_SetVRTimeLimit(TimeLimit) == 0)
        {
            sRet = MMPS_3GPRECD_SetFileSizeLimit(SIGNLE_FILE_SIZE_LIMIT_3_75G);
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

            if(TimeLimit == NON_CYCLING_TIME_LIMIT/*Non cycle record*/)
            {
                UINT64  ulStorageFreeSpace  = 0;

                VideoFunc_GetFreeSpace(&ulStorageFreeSpace);

                if(ulStorageFreeSpace < SIGNLE_FILE_SIZE_LIMIT_4G)
                {
                    printc(FG_RED("--W-- Free Space Not Enough to Enable Seamless-2\r\n"));
                    sRet = MMPS_3GPRECD_StartSeamless(MMP_FALSE);
                    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
                    m_bSeamlessRecord = AHC_FALSE;
                }
                else
                {
                    sRet = MMPS_3GPRECD_StartSeamless(MMP_TRUE);
                    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
                    m_bSeamlessRecord = AHC_TRUE;
                }
            }
            else
            {
                #if(SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
                if(uiGetParkingModeEnable())
                {
                    sRet = MMPS_3GPRECD_StartSeamless(MMP_FALSE);
                    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
                    m_bSeamlessRecord = AHC_FALSE;
                }
                else
                #endif
                {
                    sRet = MMPS_3GPRECD_StartSeamless(MMP_TRUE);
                    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
                    m_bSeamlessRecord = AHC_TRUE;
                }
            }

            #if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
            if( uiGetParkingModeEnable() == AHC_FALSE &&
                MenuSettingConfig()->uiGsensorSensitivity != GSENSOR_SENSITIVITY_OFF )
            {
                #if (UVC_HOST_VIDEO_ENABLE)
                USB_DETECT_PHASE USBCurrentStates = 0;
                AHC_USBGetStates(&USBCurrentStates,AHC_USB_GET_PHASE_CURRENT);
                if ((USBCurrentStates == USB_DETECT_PHASE_REAR_CAM) && (AHC_TRUE == AHC_HostUVCVideoIsEnabled()))
                {
                    sRet = MMPS_3GPRECD_EnableUVCEmergentRecd( MMP_TRUE );
                    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
                }
                #endif
                sRet = MMPS_3GPRECD_EnableEmergentRecd(AHC_TRUE);
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

                sRet = MMPS_3GPRECD_SetEmergPreEncTimeLimit(EMER_RECORD_DUAL_WRITE_PRETIME*1000 + EM_VR_TIME_LIMIT_OFFSET);
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

                AHC_SetEmerRecordInterval(EMER_RECORD_DUAL_WRITE_INTERVAL);
            }
            else
            {
                sRet = MMPS_3GPRECD_EnableUVCEmergentRecd( MMP_FALSE );
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
            }
            #endif

            #if (VIDEO_REC_WITH_MUTE_EN)
            #if ((MENU_MOVIE_EN && MENU_MOVIE_SOUND_RECORD_EN) || (MENU_SINGLE_EN) )
            //Audio Enable/Disable Switch.
            if(MenuSettingConfig()->uiMOVSoundRecord == MOVIE_SOUND_RECORD_ON)
            #else
            if(1)
            #endif
            #if SETTING_VR_VOLUME_EVERYTIME
            {
                AHC_SetVRVolume(AHC_TRUE);
            }
            else 
            {
                AHC_SetVRVolume(AHC_FALSE);
            }
			#else
			{
				// NOP
			}
            #endif//< (SETTING_VR_VOLUME_EVERYTIME)
            #endif//< (VIDEO_REC_WITH_MUTE_EN)    

#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_NORMAL)
            {
                AHC_BOOL bStatus;
				// patch for AIT rear cam slow card issue. Open file for UVC here.
				// Seamless is ok because of TX start by m_container.
	            #if (UVC_HOST_VIDEO_ENABLE) && (UVC_VIDRECD_SUPPORT)
	            {
					USB_DETECT_PHASE USBCurrentStates = 0;
	                AHC_USBGetStates(&USBCurrentStates,AHC_USB_GET_PHASE_CURRENT);
	                if ((USBCurrentStates == USB_DETECT_PHASE_REAR_CAM) && 
	                    (AHC_TRUE == AHC_HostUVCVideoIsEnabled())){
	                    MMPS_3GPRECD_OpenUVCRecdFile();
	                }
				}
				#endif
                sRet = MMPS_3GPRECD_StartRecord();
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

                #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
                if(uiGetParkingModeEnable())
                {
                    bStatus = AHC_UF_PreAddFile(m_usVideoParkDirKey,(INT8*)m_VideoParkFileName);
                }
                else
                #endif
                {
                    bStatus = AHC_UF_PreAddFile(m_usVideoRDirKey,(INT8*)m_VideoRFileName);
                }

                //AHC_UF_PostAddFile(m_usVideoRDirKey,(INT8*)m_VideoRFileName);
            }
#elif (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
            {
                AHC_BOOL bStatus;

                //PreAddFile will delete the file has  same name.
                #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
                if(uiGetParkingModeEnable())
                    bStatus = AHC_UF_PreAddFile(m_usVideoParkDirKey,(INT8*)m_VideoParkFileName);
                else
                #endif
                    bStatus = AHC_UF_PreAddFile(m_usVideoRDirKey,(INT8*)m_VideoRFileName);
				// patch for AIT rear cam slow card issue. Open file for UVC here.
				// Seamless is ok because of TX start by m_container.
	            #if (UVC_HOST_VIDEO_ENABLE) && (UVC_VIDRECD_SUPPORT)
	            {
					USB_DETECT_PHASE USBCurrentStates = 0;
	                AHC_USBGetStates(&USBCurrentStates,AHC_USB_GET_PHASE_CURRENT);
	                if ((USBCurrentStates == USB_DETECT_PHASE_REAR_CAM) && 
	                    (AHC_TRUE == AHC_HostUVCVideoIsEnabled())){
	                    MMPS_3GPRECD_OpenUVCRecdFile();
	                }
				}
				#endif
                sRet = MMPS_3GPRECD_StartRecord();
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet);} 
                
                if((sRet != MMP_ERR_NONE) || (bStatus !=  AHC_TRUE)){
                    INT8   DirName[64]; 
					
                    AHC_PRINT_RET_ERROR(gbAhcDbgBrk,bStatus);
                    if( GetPathDirStr(DirName, sizeof(DirName), m_CurVRFullName) )
                    {
                        #if(SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
                        if(uiGetParkingModeEnable()) {
                            AHC_UF_FileOperation((UINT8*)DirName,(UINT8*)m_VideoParkFileName, DCF_FILE_DELETE, NULL,NULL);
                        }
                        else
                        #endif
                        {
                            AHC_UF_FileOperation((UINT8*)DirName,(UINT8*)m_VideoRFileName, DCF_FILE_DELETE, NULL,NULL);
                        }
                    }

                    return AHC_FALSE;
                }
            }
#endif
        }
        else
        {
            AHC_SetVRTimeLimit(180);
            printc(FG_RED("AHC_SetVRTimeLimit error, force set to 3min\r\n"));
        }

        #else///< (VR_SEAMLESS)

        AHC_SetVRTimeLimit(18000); //05:00:00

        if (MMPS_3GPRECD_SetFileSizeLimit(SIGNLE_FILE_SIZE_LIMIT_4G) == MMP_ERR_NONE)
        {
            UINT32 ulVideoReservedSize;
            AHC_BOOL bStatus = AHC_TRUE;

            AIHC_GetMovieConfig( AHC_VIDEO_RESERVED_SIZE, &ulVideoReservedSize );
            sRet = MMPS_3GPRECD_SetReservedStorageSpace(ulVideoReservedSize);
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_NORMAL)
			// patch for AIT rear cam slow card issue. Open file for UVC here.
			// Seamless is ok because of TX start by m_container.
            #if (UVC_HOST_VIDEO_ENABLE) && (UVC_VIDRECD_SUPPORT)
            {
				USB_DETECT_PHASE USBCurrentStates = 0;
                AHC_USBGetStates(&USBCurrentStates,AHC_USB_GET_PHASE_CURRENT);
                if ((USBCurrentStates == USB_DETECT_PHASE_REAR_CAM) && 
                    (AHC_TRUE == AHC_HostUVCVideoIsEnabled())){
                    MMPS_3GPRECD_OpenUVCRecdFile();
                }
			}			
			#endif
            sRet = MMPS_3GPRECD_StartRecord();
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

            #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
            if(!uiGetParkingModeEnable())
                bStatus = AHC_UF_PreAddFile(m_usVideoParkDirKey,(INT8*)m_VideoParkFileName);
            else
            #endif
                bStatus = AHC_UF_PreAddFile(m_usVideoRDirKey,(INT8*)m_VideoRFileName);

            //AHC_UF_PostAddFile(m_usVideoRDirKey,(INT8*)m_VideoRFileName);
#elif (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
            //PreAddFile will delete the file has same name.
            if(!uiGetParkingModeEnable())
                bStatus = AHC_UF_PreAddFile(m_usVideoRDirKey,(INT8*)m_VideoRFileName);
            else
                bStatus = AHC_UF_PreAddFile(m_usVideoParkDirKey,(INT8*)m_VideoParkFileName);
			// patch for AIT rear cam slow card issue. Open file for UVC here.
			// Seamless is ok because of TX start by m_container.
            #if (UVC_HOST_VIDEO_ENABLE) && (UVC_VIDRECD_SUPPORT)
            {
				USB_DETECT_PHASE USBCurrentStates = 0;
                AHC_USBGetStates(&USBCurrentStates,AHC_USB_GET_PHASE_CURRENT);
                if ((USBCurrentStates == USB_DETECT_PHASE_REAR_CAM) && 
                    (AHC_TRUE == AHC_HostUVCVideoIsEnabled())){
                    MMPS_3GPRECD_OpenUVCRecdFile();
                }
			}
			#endif
            sRet = MMPS_3GPRECD_StartRecord();
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
            
            if((sRet != MMP_ERR_NONE) || (bStatus != AHC_TRUE) )
            {
                INT8   DirName[64];
                
                AHC_PRINT_RET_ERROR(bStatus);
                if( GetPathDirStr(DirName, sizeof(DirName), m_CurVRFullName) )
                {
        #if(SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
                    if(uiGetParkingModeEnable()) {
                        AHC_UF_FileOperation((UINT8*)DirName,(UINT8*)m_VideoParkFileName, DCF_FILE_DELETE, NULL,NULL);
                    }
                    else
        #endif
                    {
                        AHC_UF_FileOperation((UINT8*)DirName,(UINT8*)m_VideoRFileName, DCF_FILE_DELETE, NULL,NULL);
                    }
                }

                return AHC_FALSE;
            }
#endif
        }

        #endif///< (VR_SEAMLESS)


#if (UVC_HOST_VIDEO_ENABLE) 
        {
            USB_DETECT_PHASE USBCurrentStates = 0;

            AHC_USBGetStates(&USBCurrentStates,AHC_USB_GET_PHASE_CURRENT);
            if ((USBCurrentStates == USB_DETECT_PHASE_REAR_CAM) &&  (AHC_TRUE == AHC_HostUVCVideoIsEnabled())){
                if (!AHC_HostUVCVideoRecordStart()){ 
                    AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); 
                    return AHC_FALSE;
                }
            }
        }
#elif (SUPPORT_DUAL_SNR == 1 && DUALENC_SUPPORT == 1)
        if(AHC_TRUE == AHC_GetSecondSensorCnntStatus())//bit1603 must be connect
        {
            if(!AHC_VideoDualRecordStart()){
                AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); 
                return AHC_FALSE;
            }
        }
#endif
#if (DUALENC_SUPPORT)
        // Dual encode start record together
        sRet = MMPS_3GPRECD_StartAllRecord();
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet);} 
#endif

        #if (FS_FORMAT_FREE_ENABLE == 0) //Do NOT support GPS / Gsensor temp file in Format Free.
        #if (GPS_CONNECT_ENABLE && GPS_USE_FILE_AS_DATABUF)
        if(AHC_GPS_Module_Attached())
        {
            STRCPY(bGpsInfoFileName, (INT8*)AHC_UF_GetRootName());
            STRCAT(bGpsInfoFileName, GPS_TEMP_INFOFILE_DIR);

            if(AHC_FS_DirOpen(bGpsInfoFileName, sizeof(bGpsInfoFileName),&ulGpsInfoDirID) != AHC_ERR_NONE)
            {
                AHC_ERR err;
                err = AHC_FS_DirCreate(bGpsInfoFileName, sizeof(bGpsInfoFileName));
                printc("AHC_FS_DirCreate :%x\r\n",err);
            }else{

                AHC_FS_DirClose(ulGpsInfoDirID);
            }

            STRCAT(bGpsInfoFileName, "\\");
            STRCAT(bGpsInfoFileName, (INT8*)GPS_TEMP_INFOFILE_NAME);
            GPSCtrl_OpenInfoFile(bGpsInfoFileName);
        }
        #endif

        #if ((GSENSOR_CONNECT_ENABLE) && (GSENSOR_FUNC & FUNC_VIDEOSTREM_INFO) && GSNR_USE_FILE_AS_DATABUF)
        if(AHC_Gsensor_Module_Attached())
        {
            MMP_BYTE    bGsensorInfoFileName[MAX_FILE_NAME_SIZE] = {0};
            MMP_ULONG   ulGsensorInfoDirID;

            STRCPY(bGsensorInfoFileName, (INT8*)AHC_UF_GetRootName());
            STRCAT(bGsensorInfoFileName, GSNR_TEMP_INFOFILE_DIR);

            if(AHC_FS_DirOpen(bGsensorInfoFileName, sizeof(bGsensorInfoFileName),&ulGsensorInfoDirID) != AHC_ERR_NONE)
            {
                AHC_ERR err;
                err = AHC_FS_DirCreate(bGsensorInfoFileName, sizeof(bGsensorInfoFileName));
                printc("AHC_FS_DirCreate :%x\r\n",err);

            }else{
                AHC_FS_DirClose(ulGsensorInfoDirID);
            }

            STRCAT(bGsensorInfoFileName, "\\");
            STRCAT(bGsensorInfoFileName, (INT8*)GSNR_TEMP_INFOFILE_NAME);
            AHC_Gsensor_OpenInfoFile(bGsensorInfoFileName);
        }
        #endif
        #endif

        {
            MMP_ULONG ulVRDramEnd;
            sRet = MMPS_3GPRECD_GetParameter(MMPS_3GPRECD_PARAMETER_DRAM_END, &ulVRDramEnd);
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}             
            printc(FG_BLUE("@@@ Dram Memory End Addr of Video Record: 0x%X")"\r\n", ulVRDramEnd);
        }

        #if (C005_PARKING_CLIP_Length_10PRE_10REC) && (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
        if(uiGetParkingModeEnable())
        {
            UINT32 uiCurrentTime = 0;
            sRet = MMPS_3GPRECD_Get3gpRecordingOffset(&uiCurrentTime);
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}
            uiCurrentTime = (uiCurrentTime+1000 - 1)/1000; //in sec
            AHC_SetParkingRecordTime(uiCurrentTime+PARK_RECORD_WRITE_POSTTIME);
        }
        #endif
        return AHC_TRUE;

    }else{
        return AHC_FALSE;
    }
}

/**
 @brief Stop VideoR record

 Stop VideoR record

 @retval AHC_BOOL
*/
#if (VR_SEAMLESS)
AHC_BOOL AIHC_StopVideoRecordModeEx(AHC_BOOL bStopRecord,AHC_BOOL bShutdownSensor, AHC_BOOL bStopPreview)
{
/**
@brief  separate the API into two part, one is for VR_SEAMLESS, another is for normal record.
*/
#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
    MMP_ULONG 	ulRecordingTime;
#endif
    UINT32 		TimeStampOp;
    MMP_ERR sRet = MMP_ERR_NONE;

    if (bStopRecord)
    {
        if (m_bSeamlessRecord == AHC_TRUE) {
            MMPS_3GPRECD_StartSeamless(MMP_FALSE);
            m_bSeamlessRecord = AHC_FALSE;
        }

		#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
        MMPS_3GPRECD_GetRecordTime(&ulRecordingTime);
		#endif

		#if (SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_2)
        {
            #if (MOTION_DETECTION_EN)
            //if(!uiGetParkingModeEnable())
            {
                m_bMotionDtcEn = AHC_FALSE;

                Menu_SetMotionDtcSensitivity_Parking(PARKINGMODE_IMAGE_DETECTION_NUM);
            }
            #endif
        }
		#endif

        MMPS_3GPRECD_StopRecord();

		AHC_ConfigMovie(AHC_AUD_PRERECORD_DAC, AHC_TRUE);

		#if (UVC_HOST_VIDEO_ENABLE)
        AHC_HostUVCVideoRecordStop();
		#endif

		#if (GPS_CONNECT_ENABLE == 1)
        if (AHC_GPS_Module_Attached())
        {
            #if (GPS_RAW_FILE_ENABLE == 1)
            UINT8 bGps_En;

            if (AHC_Menu_SettingGetCB((char*)COMMON_KEY_GPS_RECORD_ENABLE, &bGps_En) == AHC_TRUE) {
                switch (bGps_En) {
                case RECODE_GPS_OFF:
                case RECODE_GPS_IN_VIDEO:
                    // NOP
                    break;
                default:
                    GPSCtrl_GPSRawClose();
                    break;
                }
            }
            #endif

            #if (GPS_KML_FILE_ENABLE == 1)
            GPSCtrl_KMLGen_Write_TailAndClose();
            #endif
        }
		#endif

        #if (GPS_RAW_FILE_ENABLE == 1)
        GPSCtrl_GPSRawWrite_Close();
        #endif
        #if (GSENSOR_RAW_FILE_ENABLE == 1 && GPS_CONNECT_ENABLE == 0)
        GPSCtrl_GPSRawClose();
        #endif

        #if (GPS_CONNECT_ENABLE == 1)
        if (AHC_GPS_Module_Attached())
        {
            #if (GPS_RAW_FILE_EMER_EN == 1)
            UINT8 bGps_En;

            if (AHC_Menu_SettingGetCB((char*)COMMON_KEY_GPS_RECORD_ENABLE, &bGps_En) == AHC_TRUE) {
                switch (bGps_En) {
                case RECODE_GPS_OFF:
                case RECODE_GPS_IN_VIDEO:
                    // NOP
                    break;
                default:
                    GPSCtrl_GPSRawClose_Emer();
                    break;
                }
            }
            #endif
        }
        #endif

        #if(GSENSOR_RAW_FILE_ENABLE == 1 && GPS_CONNECT_ENABLE == 0 && GPS_RAW_FILE_EMER_EN == 1)
        GPSCtrl_GPSRawClose_Emer();
        #endif

        if (!uiGetParkingModeEnable())
        {
            #if (DCF_DB_COUNT > 1)
            AHC_UF_SelectDB(DCF_DB_TYPE_1ST_DB);
			#endif
            AHC_UF_PostAddFile(m_usVideoRDirKey, (INT8*)m_VideoRFileName);
        }

		#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
		{
	        AHC_RTC_TIME    RtcTime;
	        int   			nSecondOffset;

	        AHC_RTC_GetTime(&RtcTime);

	        if ((m_VideoRecStartRtcTime.uwYear <= RTC_DEFAULT_YEAR) && (RtcTime.uwYear > RTC_DEFAULT_YEAR)) {
	            nSecondOffset = -1*(ulRecordingTime/1000);
	            AHC_DT_ShiftRtc(&RtcTime, nSecondOffset);

	            printc("AIHC_StopVideoRecordMode:AHC_UF_Rename::Old:%d-%d-%d:%d-%d-%d New:%d-%d-%d:%d-%d-%d \r\n",
	            m_VideoRecStartRtcTime.uwYear,m_VideoRecStartRtcTime.uwMonth,m_VideoRecStartRtcTime.uwDay,
	            m_VideoRecStartRtcTime.uwHour,m_VideoRecStartRtcTime.uwMinute,m_VideoRecStartRtcTime.uwSecond,
	            RtcTime.uwYear,RtcTime.uwMonth,RtcTime.uwDay,RtcTime.uwHour,RtcTime.uwMinute,RtcTime.uwSecond);

	            AHC_UF_Rename(AHC_UF_GetDB(), &m_VideoRecStartRtcTime, &RtcTime);

	            m_VideoRecStartRtcTime = RtcTime;
	        }
		}
		#endif

		#if (EMERGENTRECD_SUPPORT) && (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_MOVE_FILE)
	    if (AHC_GetNormal2Emergency())
	    {
	        UINT8           bCreateNewDir;
	        UINT32          Param;
	        MMPS_3GPRECD_CONTAINER      ContainerType;
	        UINT16          DirKey;
	        char            FilePathName[MAX_FILE_NAME_SIZE];
	        INT8            DirName[32];
	        INT8            FileName[32];
            UINT32          StrLen;
	        AHC_SetNormal2Emergency(AHC_FALSE);

	        MEMSET(FilePathName, 0, sizeof(FilePathName));
	        MEMSET(FileName, 0, sizeof(FileName));

	        AHC_UF_GetName2(&m_VideoRecStartRtcTime, FilePathName, FileName, &bCreateNewDir);
	        AIHC_GetMovieConfig(AHC_CLIP_CONTAINER_TYPE, &Param);
	        ContainerType   = Param;

	        STRCAT(FilePathName, EXT_DOT);

	        if (ContainerType == MMPS_3GPRECD_CONTAINER_3GP) {
	            STRCAT(FilePathName, MOVIE_3GP_EXT);
	        }
	        else {
	            STRCAT(FilePathName, MOVIE_AVI_EXT);
	        }

	        printc("Normal2Emer File: %s \n", FilePathName);
            //
            MEMSET(DirName,0,sizeof(DirName));
            MEMSET(FileName,0,sizeof(FileName));
            GetPathDirStr(DirName,sizeof(DirName),FilePathName);
            StrLen = AHC_StrLen(FilePathName) - AHC_StrLen(DirName) - 1;
            MEMCPY(FileName,FilePathName + AHC_StrLen(DirName) + 1,StrLen);
            //
	        AHC_UF_MoveFile(DCF_DB_TYPE_1ST_DB, DCF_DB_TYPE_3RD_DB, DirKey, FileName, AHC_TRUE);
	    }
		#endif

        #if 0
        #if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
        if (m_bStartEmerVR == AHC_FALSE){
            AHC_EmerRecordPostProcess();
        }
        #endif
        #endif

        if (AHC_Protect_GetType() != AHC_PROTECT_NONE)
        {
            #ifdef CFG_CUS_VIDEO_PROTECT_PROC
            CFG_CUS_VIDEO_PROTECT_PROC();
            #else
            AHC_Protect_SetVRFile(AHC_PROTECTION_CUR_FILE, AHC_Protect_GetType());

            AHC_Protect_SetType(AHC_PROTECT_NONE);
            #endif
        }
    }

    if (bStopPreview) {
        AHC_BOOL ahcRet = AHC_TRUE;
        
#if (UVC_HOST_VIDEO_ENABLE)
        {
            USB_DETECT_PHASE USBCurrentStates = 0;

            AHC_USBGetStates(&USBCurrentStates,AHC_USB_GET_PHASE_CURRENT);
            // printc("bStopPreview: %x, %x, %x\r\n", bStopPreview, USBCurrentStates, USB_DETECT_PHASE_REAR_CAM);
            if(bStopPreview || (USBCurrentStates != USB_DETECT_PHASE_REAR_CAM)){
                AHC_HostUVCVideoPreviewStop();
                AHC_HostUVCVideoEnable(AHC_FALSE);
            }
        }
#endif
#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
        sRet = MMPS_3GPRECD_DecMjpegPreviewStop(gsAhcCurrentSensor);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
#endif     
#if (UVC_HOST_VIDEO_ENABLE)
        ahcRet = AHC_HostUVCResetFBMemory();
#endif
#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
        ahcRet = AHC_HostUVCResetMjpegToPreviewJpegBuf();
#endif        

        sRet = MMPS_3GPRECD_PreviewStop(gsAhcCurrentSensor);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

#if (SUPPORT_DUAL_SNR)
        MMPF_Sensor_EnableTV_Dec_Src_TypeDetection(MMP_FALSE);
        sRet = MMPS_3GPRECD_2ndSnrPreviewStop(gsAhcSecondSensor);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); /*return AHC_FALSE;*/} //TBD.  If rear sensor is not connected, skip second stream record.       
#endif                  
        AHC_SetVRWithWNR(AHC_FALSE);
    }


    AHC_GetCaptureConfig(ACC_DATE_STAMP, &TimeStampOp);

    if (TimeStampOp & AHC_ACC_TIMESTAMP_ENABLE_MASK) {
        MMPS_DSC_SetSticker(NULL, NULL);
    }

    if (bShutdownSensor) {
        sRet = MMPS_Sensor_PowerDown(gsAhcCurrentSensor);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
#if (SUPPORT_DUAL_SNR)
        sRet = MMPS_Sensor_PowerDown(SCD_SENSOR);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
#endif                                  
    }

    #if (FS_FORMAT_FREE_ENABLE)
    // Note: Below setting must be set after MMPS_3GPRECD_StopRecord() which will return after finished writing video file.
    MMPS_FORMATFREE_EnableWrite( 0 ); //Disable "Format Free Write" which will not update FAT table
    #endif

	return AHC_TRUE;
}

/**
 @brief Stop VideoR record

 Stop VideoR record

 @retval AHC_BOOL
*/
AHC_BOOL AIHC_StopVideoRecordMode(AHC_BOOL bStopRecord,AHC_BOOL bShutdownSensor)
{
    RTNA_DBG_Str(0, "### AIHC_StopVideoRecordMode\r\n");

    return AIHC_StopVideoRecordModeEx(bStopRecord, bShutdownSensor, AHC_TRUE);
}
#else///< #if (VR_SEAMLESS)
static AHC_BOOL AIHC_StopVideoRecordMode(AHC_BOOL bStopRecord,AHC_BOOL bShutdownSensor)
{
    UINT32 ulDacStatus;
    MMP_ERR sRet = MMP_ERR_NONE;
    AHC_BOOL ahcRet = AHC_TRUE;

    AIHC_GetMovieConfig(AHC_AUD_PRERECORD_DAC,&ulDacStatus);

    if ( bStopRecord || (ulDacStatus == AHC_FALSE)) {

        #if (ENABLE_VIDEO_ERR_LOG)
        MMPS_3GPRECD_RecordStatus((MMPS_FW_OPERATION*)&m_VideoErrLog.fwOperationBeforeStopRecord);
        MMPS_3GPRECD_MergerStatus((MMP_ERR*)&m_VideoErrLog.retStatusBeforeStopRecord,
                                  (MMP_ULONG*)&m_VideoErrLog.txStatusBeforeStopRecord);
        #endif

        MMPS_3GPRECD_StopRecord();

#if UVC_HOST_VIDEO_ENABLE
        AHC_HostUVCVideoRecordStop();
#endif

        #if (ENABLE_VIDEO_ERR_LOG)
        MMPS_3GPRECD_RecordStatus((MMPS_FW_OPERATION*)&m_VideoErrLog.fwOperationAfterStopRecord);
        MMPS_3GPRECD_MergerStatus((MMP_ERR*)&m_VideoErrLog.retStatusAfterStopRecord,
                                  (MMP_ULONG*)&m_VideoErrLog.txStatusAfterStopRecord);
        #endif

        AHC_ConfigMovie(AHC_AUD_PRERECORD_DAC, AHC_TRUE);

        /**
        @brief

        */
        #if (ENABLE_VIDEO_ERR_LOG)
        {
            UINT32  ulFileId;
            UINT32  ulWriteCount;
            AHC_ERR err;

            m_VideoErrLog.callBackError               = 0;///< MMPS_3GPRECD_EVENT
            m_VideoErrLog.errorParameter1             = 0;
            m_VideoErrLog.errorParameter2             = 0;
            m_VideoErrLog.fwOperationBeforeStopRecord = 0;///< MMPS_FW_OPERATION
            m_VideoErrLog.fwOperationAfterStopRecord  = 0;///< MMPS_FW_OPERATION
            m_VideoErrLog.retStatusBeforeStopRecord   = 0;///< MMP_ERR
            m_VideoErrLog.retStatusAfterStopRecord    = 0;
            m_VideoErrLog.txStatusBeforeStopRecord    = 0;
            m_VideoErrLog.txStatusAfterStopRecord     = 0;
            #if 0
            printc("write err log \r\n");
            printc("m_VideoErrLog.callBackError               : %x \r\n", m_VideoErrLog.callBackError );
            printc("m_VideoErrLog.errorParameter1             : %x \r\n", m_VideoErrLog.errorParameter1 );
            printc("m_VideoErrLog.errorParameter2             : %x \r\n", m_VideoErrLog.errorParameter2 );
            printc("m_VideoErrLog.fwOperationBeforeStopRecord : %x \r\n", m_VideoErrLog.fwOperationBeforeStopRecord );
            printc("m_VideoErrLog.fwOperationAfterStopRecord  : %x \r\n", m_VideoErrLog.fwOperationAfterStopRecord );
            printc("m_VideoErrLog.retStatusBeforeStopRecord   : %x \r\n", m_VideoErrLog.retStatusBeforeStopRecord );
            printc("m_VideoErrLog.retStatusAfterStopRecord    : %x \r\n", m_VideoErrLog.retStatusAfterStopRecord );
            printc("m_VideoErrLog.txStatusBeforeStopRecord    : %x \r\n", m_VideoErrLog.txStatusBeforeStopRecord );
            printc("m_VideoErrLog.txStatusAfterStopRecord     : %x \r\n", m_VideoErrLog.txStatusAfterStopRecord );
            #endif//< 0

            err = AHC_FS_FileOpen(m_VideoRecordErrorFileName,
                            AHC_StrLen(m_VideoRecordErrorFileName),
                            "wb",
                            AHC_StrLen("wb"),
                            &ulFileId);

            if(err == AHC_ERR_NONE){
                AHC_FS_FileWrite(ulFileId, (UINT8*)&m_VideoErrLog, sizeof(m_VideoErrLog), &ulWriteCount);
                AHC_FS_FileClose(ulFileId);
            }
        }
        #endif///< (ENABLE_VIDEO_ERR_LOG)
    }


#if UVC_HOST_VIDEO_ENABLE
    AHC_HostUVCVideoPreviewStop();
    AHC_HostUVCVideoEnable(AHC_FALSE);
#endif

#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
    sRet = MMPS_3GPRECD_DecMjpegPreviewStop(gsAhcCurrentSensor);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
#endif 
#if (UVC_HOST_VIDEO_ENABLE)
        ahcRet = AHC_HostUVCResetFBMemory();
#endif
#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
        ahcRet = AHC_HostUVCResetMjpegToPreviewJpegBuf();
#endif        

    sRet = MMPS_3GPRECD_PreviewStop(gsAhcCurrentSensor);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

#if (SUPPORT_DUAL_SNR)
    MMPF_Sensor_EnableTV_Dec_Src_TypeDetection(MMP_FALSE);
    sRet = MMPS_3GPRECD_2ndSnrPreviewStop(gsAhcSecondSensor);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); /*return AHC_FALSE;*/} //TBD.  If rear sensor is not connected, skip second stream record.       
#endif                  

    AHC_SetVRWithWNR(AHC_FALSE);

    #if (AHC_ENABLE_VIDEO_STICKER)
    {
        UINT32 TimeStampOp;

        AHC_GetCaptureConfig(ACC_DATE_STAMP, &TimeStampOp);

        if (TimeStampOp & AHC_ACC_TIMESTAMP_ENABLE_MASK) {
            #if (SW_STICKER_EN == 1)
            AIHC_SWSticker_SetCBFuncPtr(NULL);
            #endif
            MMPS_3GPRECD_EnableSticker(MMPS_3GPRECD_STICKER_ID_1, MMP_FALSE);
        }
    }
    #endif///< (AHC_ENABLE_VIDEO_STICKER)

    if ( bShutdownSensor ) {
        sRet = MMPS_Sensor_PowerDown(gsAhcCurrentSensor);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
#if (SUPPORT_DUAL_SNR)
        sRet = MMPS_Sensor_PowerDown(SCD_SENSOR);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
#endif                          
    }

    if ( bStopRecord ) {
        //add file to DCF database
        //AHC_UF_PreAddFile(m_usVideoRDirKey,(INT8*)m_VideoRFileName); //Rogers@20111021: for video record + Capture, move to AIHC_SetVideoRecordMode
        #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
        if(uiGetParkingModeEnable())
            AHC_UF_PostAddFile(m_usVideoParkDirKey,(INT8*)m_VideoParkFileName);
        else
        #endif
            AHC_UF_PostAddFile(m_usVideoRDirKey,(INT8*)m_VideoRFileName);

        #if 0
        #if (EMER_RECORD_DUAL_WRITE_ENABLE == 1)
        if(m_bStartEmerVR == AHC_FALSE)){
            AHC_EmerRecordPostProcess();
        }
        #endif
        #endif
    }

    #if (FS_FORMAT_FREE_ENABLE)
    // Note: Below setting must be set after MMPS_3GPRECD_StopRecord() which will return after finished writing video file.
    MMPS_FORMATFREE_EnableWrite( 0 ); //Disable "Format Free Write" which will not update FAT table
    #endif

    return AHC_TRUE;
}

#endif///< #if (VR_SEAMLESS)

AHC_BOOL AHC_TransferRGB565toIndex8(UINT32 uiSrcAddr,
                                    UINT32 uiSrcW,
                                    UINT32 uiSrcH,
                                    UINT32 uiDestAddr,
                                    UINT32 uiDestW,
                                    UINT32 uiDestH,
                                    UINT8  byForegroundColor,
                                    UINT8  byBackgroundColor)
{
    UINT16  *puwSrcBuf;
    UINT8   *pbyDestBuf;
    UINT32  uiY;
    UINT32  uiX;

    puwSrcBuf   = (UINT16 *)uiSrcAddr;
    pbyDestBuf  = (UINT8 *)uiDestAddr;

    for(uiY = 0 ; uiY < uiDestH ; ++uiY ){

        for(uiX = 0 ; uiX < uiDestW ; ++uiX){

            *(pbyDestBuf + (uiY * uiDestW) + uiX) =
            ( *(puwSrcBuf + (uiY * uiSrcW) + (uiX + (uiSrcW - uiDestW)) ) != 0 ) ? byForegroundColor : byBackgroundColor;
        }
    }

    return AHC_TRUE;
}

/**
@brief  transfer psRtcTime(int) to pbyDate(string)

*/
AHC_BOOL AHC_TransferDateToString(AHC_RTC_TIME* psRtcTime, INT8* pbyDate, UINT8 byDateConfig, UINT8 byFormatConfig)
{
    AHC_TIME_STRING sTimeString;

    AIHC_Int2Str(psRtcTime->uwYear   , sTimeString.byYear);
    AIHC_Int2Str(psRtcTime->uwMonth  , sTimeString.byMonth);
    AIHC_Int2Str(psRtcTime->uwDay    , sTimeString.byDay);
    AIHC_Int2Str(psRtcTime->uwHour   , sTimeString.byHour);
    AIHC_Int2Str(psRtcTime->uwMinute , sTimeString.byMinute);
    AIHC_Int2Str(psRtcTime->uwSecond , sTimeString.bySecond);

    if (AHC_ACC_TIMESTAMP_TIME_ONLY != byDateConfig) {

        if  ( byFormatConfig == AHC_ACC_TIMESTAMP_FORMAT_1 ){

            STRCPY(pbyDate, sTimeString.byYear);
            STRCAT(pbyDate, " ");

            if(psRtcTime->uwMonth < 10)
                STRCAT(pbyDate, "0");

            STRCAT(pbyDate, sTimeString.byMonth);
            STRCAT(pbyDate, " ");

            if(psRtcTime->uwDay < 10)
                STRCAT(pbyDate, "0");

            STRCAT(pbyDate, sTimeString.byDay);
        }
        else if  ( byFormatConfig == AHC_ACC_TIMESTAMP_FORMAT_2 ){
            STRCPY(pbyDate, sTimeString.byYear);
            STRCAT(pbyDate, "/");

            if(psRtcTime->uwMonth < 10)
                STRCAT(pbyDate, "0");

            STRCAT(pbyDate, sTimeString.byMonth);
            STRCAT(pbyDate, "/");

            if(psRtcTime->uwDay < 10)
                STRCAT(pbyDate, "0");

            STRCAT(pbyDate, sTimeString.byDay);
        }
        else if  ( byFormatConfig == AHC_ACC_TIMESTAMP_FORMAT_3 ){

            if(psRtcTime->uwDay < 10)
                STRCPY(pbyDate, "0");

            STRCAT(pbyDate, sTimeString.byDay);
            STRCAT(pbyDate, "/");

            if(psRtcTime->uwMonth < 10)
                STRCAT(pbyDate, "0");

            STRCAT(pbyDate, sTimeString.byMonth);
            STRCAT(pbyDate, "/");
            STRCAT(pbyDate, sTimeString.byYear);
        }
        else if  ( byFormatConfig == AHC_ACC_TIMESTAMP_FORMAT_4 ){

            if(psRtcTime->uwMonth < 10)
                STRCPY(pbyDate, "0");

            STRCAT(pbyDate, sTimeString.byMonth);
            STRCAT(pbyDate, "/");

            if(psRtcTime->uwDay < 10)
                STRCAT(pbyDate, "0");

            STRCAT(pbyDate, sTimeString.byDay);
            STRCAT(pbyDate, "/");
            STRCAT(pbyDate, sTimeString.byYear);
        }

        if (AHC_ACC_TIMESTAMP_DATE_AND_TIME == (byDateConfig & AHC_ACC_TIMESTAMP_DATE_MASK)) {

            STRCAT(pbyDate, " ");

            if(psRtcTime->uwHour < 10)
                STRCAT(pbyDate, "0");

            STRCAT(pbyDate, sTimeString.byHour);
            STRCAT(pbyDate, ":");

            if(psRtcTime->uwMinute < 10)
                STRCAT(pbyDate,"0");

            STRCAT(pbyDate, sTimeString.byMinute);
            STRCAT(pbyDate, ":");

            if(psRtcTime->uwSecond < 10)
                STRCAT(pbyDate,"0");

            STRCAT(pbyDate, sTimeString.bySecond);
        }
    }
    else {
        if(psRtcTime->uwHour < 10) {
            STRCPY(pbyDate, "0");
            STRCAT(pbyDate, sTimeString.byHour);
        }
        else {
            STRCPY(pbyDate, sTimeString.byHour);
        }

        STRCAT(pbyDate, ":");

        if(psRtcTime->uwMinute < 10)
            STRCAT(pbyDate, "0");

        STRCAT(pbyDate, sTimeString.byMinute);
        STRCAT(pbyDate, ":");

        if(psRtcTime->uwSecond < 10)
            STRCAT(pbyDate, "0");

        STRCAT(pbyDate, sTimeString.bySecond);
    }

    return AHC_TRUE;
}

#if (SW_STICKER_EN == 1)

AHC_BOOL AIHC_SWSticker_MoveBuf(MMP_ULONG ulYBufAddr, MMP_ULONG ulUVBufAddr)
{
    UINT16  i,j;
    UINT32  uiSrcAddr;
    UINT32  uiTimeStampOp;
    UINT8   ubY, ubU, ubV;
    ubY = 235;   //WHITE RGB to YUV
    ubU = 128;
    ubV = 133;

    AHC_GetCaptureConfig(ACC_DATE_STAMP, &uiTimeStampOp);

    if (uiTimeStampOp & AHC_ACC_TIMESTAMP_ENABLE_MASK){

        if(m_uiVideoStampBufIndex == 1)
        {
            AIHC_GUI_GetOSDBufAddr(m_ubStickerOsdId_0, &uiSrcAddr);
        }
        else
        {
            AIHC_GUI_GetOSDBufAddr(m_ubStickerOsdId_1, &uiSrcAddr);
        }

        for(j = 0; j<m_usStickerSrcH; j++)
        {
            for(i = 0; i<m_usStickerSrcW; i++)
            {
                if(*(MMP_UBYTE *)(uiSrcAddr + j * m_usStickerSrcW + i) != 0)// 0xFF)
                {
                    #if (STICKER_PATTERN == 1)
                    {
                    if((i < 110)&&(j < 32)){
                        ubY = 145;  //YELLOW RGB to YUV
                        ubU = 53;
                        ubV = 185;
                    }else{
                        ubY = 235;   //WHITE RGB to YUV
                        ubU = 128;
                        ubV = 133;
                    }
                    }
                    #endif

                    *(MMP_UBYTE *)(ulYBufAddr + (j + m_usStickerYoffset) * m_usStickerCaptureW + i + m_usStickerXoffset) =  ubY;

                    if((i%2 == 0) && (j%2 ==0))
                    {
                        *(MMP_UBYTE *)(ulUVBufAddr + (j + m_usStickerYoffset)/2 * m_usStickerCaptureW + i + m_usStickerXoffset)     =  ubU;
                        *(MMP_UBYTE *)(ulUVBufAddr + (j + m_usStickerYoffset)/2 * m_usStickerCaptureW + i + m_usStickerXoffset + 1) =  ubV;
                    }
                }
            }
        }
    }
    return AHC_TRUE;
}

AHC_BOOL AIHC_SWSticker_SetCBFuncPtr(void* pFuncPtr)
{

    MMPF_Display_SetSWStickerCB(pFuncPtr);

    return AHC_TRUE;
}

AHC_BOOL AIHC_SWSticker_SetPosition(MMP_USHORT uwSrcW, MMP_USHORT uwSrcH, MMP_USHORT uwCaptureW, MMP_USHORT uwCaptureH,
                                         MMP_USHORT uwXoffset, MMP_USHORT uwYoffset, MMP_UBYTE ubOSDid0, MMP_UBYTE ubOSDid1)
{
    m_usStickerSrcW         = uwSrcW;
    m_usStickerSrcH         = uwSrcH;
    m_usStickerCaptureW     = uwCaptureW;
    m_usStickerCaptureH     = uwCaptureH;
    m_usStickerXoffset      = uwXoffset;
    m_usStickerYoffset      = uwYoffset;
    m_ubStickerOsdId_0      = ubOSDid0;
    m_ubStickerOsdId_1      = ubOSDid1;

    return AHC_TRUE;
}

#endif ///< (SW_STICKER_EN == 1)

//Draw Text Edge
void AIHC_DrawTextEdge(UINT32 uiSrcAddr, UINT16 uwSrcW, UINT16 uwSrcH, UBYTE TextColor, UBYTE BGColor, UBYTE EdgeColor)
{
#if (STICKER_DRAW_EDGE)

    UINT16 x,y;
    MMP_UBYTE *Addr = (MMP_UBYTE *)uiSrcAddr;

    for(y=1; y<uwSrcH-1; y++ ) {
        for(x=1; x<uwSrcW-1; x++ ) {

            if(Addr[y*uwSrcW+x] == BGColor) {
                if((Addr[(y-1)*uwSrcW+(x-1)] == TextColor) || (Addr[(y-1)*uwSrcW+x] == TextColor) || (Addr[(y-1)*uwSrcW+(x+1)] == TextColor) || \
                   (Addr[y*uwSrcW+(x-1)] == TextColor) || (Addr[y*uwSrcW+(x+1)] == TextColor) || \
                   (Addr[(y+1)*uwSrcW+(x-1)] == TextColor) || (Addr[(y+1)*uwSrcW+x] == TextColor) || (Addr[(y+1)*uwSrcW+(x+1)] == TextColor) )
                   Addr[y*uwSrcW+x] = EdgeColor;
                }
        }
    }
#endif
}

void TransferStampFromIndex8ToIndex1(UINT8* pbyStampAddr, UINT16 uwStampW, UINT16 uwStampH)
{
    UINT16 i = 0;
    for( i=0; i < uwStampW*uwStampH; i++ )
    {
        if( i%8 == 0 )
        {
            pbyStampAddr[i/8] = \
                ((pbyStampAddr[i+7] > 0)? 0x80 : 0x00) | \
                ((pbyStampAddr[i+6] > 0)? 0x40 : 0x00) | \
                ((pbyStampAddr[i+5] > 0)? 0x20 : 0x00) | \
                ((pbyStampAddr[i+4] > 0)? 0x10 : 0x00) | \
                ((pbyStampAddr[i+3] > 0)? 0x08 : 0x00) | \
                ((pbyStampAddr[i+2] > 0)? 0x04 : 0x00) | \
                ((pbyStampAddr[i+1] > 0)? 0x02 : 0x00) | \
                ((pbyStampAddr[i+0] > 0)? 0x01 : 0x00) ;
        }
    }
}

AHC_BOOL AIHC_ConfigVRTimeStamp(UINT32 TimeStampOp, AHC_RTC_TIME* sRtcTime, AHC_BOOL bInitIconBuf)
{
#if (AHC_ENABLE_VIDEO_STICKER)

    MMP_STICKER_ATTR  StickerAttribute;
    UINT32                  uiSrcAddr;
    UINT16                  uwSrcW;
    UINT16                  uwSrcH;
    UINT16                  uwTempColorFormat;
    UINT16                  uwColorFormat = MMP_ICON_COLOR_INDEX8;
    UINT32                  StickerX, StickerY;
    UINT32                  uiOSDid;

    #if (CLOCK_SETTING_FOR_C003_P000 == 1)
    if(m_bClockSet == AHC_FALSE){
        return AHC_TRUE;
    }
    #endif

    AIHC_DrawReservedOSD(AHC_TRUE);

    m_uiVideoStampBufIndex ^= 1;

    if(m_uiVideoStampBufIndex == 0){
        uiOSDid = DATESTAMP_PRIMARY_OSD_ID;
    }else{
        uiOSDid = DATESTAMP_THUMB_OSD_ID;
    }
	
    
    DrawVideo_TimeStamp(uiOSDid, TimeStampOp, sRtcTime, &uiSrcAddr, &uwSrcW, &uwSrcH, &uwTempColorFormat, &StickerX,
                       &StickerY);

    AIHC_DrawReservedOSD(AHC_FALSE);
    #if (SW_STICKER_EN == 1)

    AIHC_SWSticker_SetPosition(  uwSrcW,        uwSrcH,
                                 CaptureWidth,  CaptureHeight,
                                 StickerX,      StickerY,
                                 DATESTAMP_PRIMARY_OSD_ID, DATESTAMP_THUMB_OSD_ID);

    if(bInitIconBuf == AHC_TRUE)
    {
        AIHC_SWSticker_SetCBFuncPtr((void*)AIHC_SWSticker_MoveBuf);

        AHC_OSDLoadWinPalette(DATESTAMP_PRIMARY_OSD_ID);
    }

    #else

    #if ( VIDOE_STICKER_INDEX1_EN == 1 )
    uwColorFormat = MMP_ICON_COLOR_INDEX1;
    TransferStampFromIndex8ToIndex1( (UINT8*)uiSrcAddr, uwSrcW, uwSrcH );
    #endif

    StickerAttribute.ubStickerId	  = MMP_STICKER_PIPE0_ID0;
    StickerAttribute.ulBaseAddr       = uiSrcAddr;
    StickerAttribute.usStartX         = StickerX;
    StickerAttribute.usStartY         = StickerY;
    StickerAttribute.usWidth          = uwSrcW;
    StickerAttribute.usHeight         = uwSrcH;
    StickerAttribute.colorformat      = uwColorFormat;
    StickerAttribute.ulTpColor        = GUI_BLACK;
    StickerAttribute.bTpEnable        = MMP_TRUE;
    StickerAttribute.bSemiTpEnable    = MMP_FALSE;
    StickerAttribute.ubIconWeight     = 16;
    StickerAttribute.ubDstWeight      = 0;

    MMPS_3GPRECD_SetSticker(&StickerAttribute);

    if(bInitIconBuf == AHC_TRUE)
    {
        AHC_OSDLoadIconIndexColorTable(uiOSDid,StickerAttribute.ubStickerId, StickerAttribute.colorformat);
        AHC_OSDLoadWinPalette(uiOSDid);
    }

     MMPS_3GPRECD_EnableSticker(StickerAttribute.ubStickerId, MMP_TRUE);

    #endif
#endif

    return AHC_TRUE;
}

void AIHC_UpdateVRTimeStamp(AHC_RTC_TIME* sRtcTime)
{
    
#if (AHC_ENABLE_VIDEO_STICKER)
    UINT32  ulSysStatus, TimeStampOp;
//return ;
    AHC_GetSystemStatus(&ulSysStatus);

    if(VideoFunc_RecordStatus() || uiGetParkingModeEnable())
    {
        AHC_GetCaptureConfig(ACC_DATE_STAMP, &TimeStampOp);

        if (TimeStampOp & AHC_ACC_TIMESTAMP_ENABLE_MASK){
            AIHC_ConfigVRTimeStamp(TimeStampOp, sRtcTime, MMP_FALSE);
        }
    }
#endif
}

void AHC_AvailableRecordTime(UBYTE* Hour, UBYTE* Min, UBYTE* Sec)
{
    AHC_AvailableRecordTime_Ex(0, Hour, Min, Sec);
}

/* For CarDV
 * Add bitrate input
 * In case bitrate is 0 to get current setting.
 */
void AHC_AvailableRecordTime_Ex(UINT32 bitrate, UBYTE* Hour, UBYTE* Min, UBYTE* Sec)
{
    MMPS_3GPRECD_CONTAINER      ContainerType;
    UINT32                      Param;
    UINT64         ulFreeSpace = 0;
    UINT32         ulTotalTime = 0;
    UINT32         ulVideoReservedSize;
    UINT32         ulVideoBitRate, ulAudioBitRate;
    UBYTE          ubHour;

    AIHC_GetMovieConfig(AHC_CLIP_CONTAINER_TYPE, &Param);
    ContainerType   = Param;
    MMPS_3GPRECD_SetContainerType(ContainerType);

    VideoFunc_GetFreeSpace(&ulFreeSpace);

    AIHC_GetMovieConfig(AHC_VIDEO_RESERVED_SIZE, &ulVideoReservedSize);
    if (bitrate == 0)
        AIHC_GetMovieConfig(AHC_VIDEO_BITRATE, &ulVideoBitRate);
    else
        ulVideoBitRate = bitrate;
    AIHC_GetMovieConfig(AHC_AUD_BITRATE, &ulAudioBitRate);
    if(ulFreeSpace <= ulVideoReservedSize) {
        *Hour = 0;
        *Min  = 0;
        *Sec  = 0;
        return;
    }

    ulTotalTime = MMPS_3GPRECD_GetExpectedRecordTime(ulFreeSpace - ulVideoReservedSize, ulVideoBitRate, ulAudioBitRate);

    ubHour = ulTotalTime / (60*60);

    *Hour = ubHour;

    if(ubHour <100) {
        *Min  = (ulTotalTime / 60) - (ubHour*60);
        *Sec  = ulTotalTime % 60;
    }
    else {
        *Hour = 99;
        *Min  = 59;
        *Sec  = 59;
    }
}

AHC_BOOL AHC_GetVideoRecordEventStatus(void)
{
    return m_VideoRecEvent;
}

AHC_BOOL AHC_SetVideoRecordEventStatus(AHC_BOOL trigger)
{
    m_VideoRecEvent = trigger;
    return AHC_TRUE;
}

#if 0
void _____MediaPlayback_Function_________(){ruturn;} //dummy
#endif
extern AHC_BOOL   ulPlayFC;
extern UINT32       m_CurPlayTime_MediaError;
void VideoMediaErrorPlayStopHandler()
{
    #if (SUPPORT_ESD_RECOVER_MOVIEPB == 1)
    UINT32 CurrentDcfIdx;
    INT8  chAllowedChar[MAX_ALLOWED_WORD_LENGTH];
    AHC_BOOL    err;

    #if(defined(DEVICE_GPIO_2NDSD_PLUG))

    #if(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_MASTER_SLAVE)
    if(AHC_IsSDInserted() && (m_ulVideoPlayStopStatus == AHC_VIDEO_PLAY_ERROR_STOP))
    #elif(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_COMPLEMENTARY)
    if( (AHC_IsSDInserted() || AHC_Is2ndSDInserted()) && (m_ulVideoPlayStopStatus == AHC_VIDEO_PLAY_ERROR_STOP) )
    #endif//TWOSD_WORK_MODEL

    #else//DEVICE_GPIO_2NDSD_PLUG

    if(AHC_IsSDInserted() && (m_ulVideoPlayStopStatus == AHC_VIDEO_PLAY_ERROR_STOP))

    #endif//DEVICE_GPIO_2NDSD_PLUG
    {
         UINT8 bValue = 0;
         //if(ulPlayFC == AHC_FALSE)
         {
             MovPBFunc_StopTimer();
             AHC_UF_GetCurrentIndex(&CurrentDcfIdx);
             AHC_UF_GetAllowedChar(chAllowedChar, MAX_ALLOWED_WORD_LENGTH);

             AIHC_StopPlaybackMode();

             //AHC_GetGPIO(AHC_PIO_REG_LGPIO28,&bValue);
             //if(bValue != AHC_FALSE)
             AHC_FS_IOCtl(AHC_UF_GetRootName(), 4, AHC_FS_CMD_RESET_MEDIUM, NULL, NULL);

             err = AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect);

             AHC_OS_Sleep(500);

             AHC_UF_SetFreeChar( 0,DCF_SET_FREECHAR,(UINT8*)chAllowedChar);
             AHC_UF_SetCurrentIndex(CurrentDcfIdx);
            // SetKeyPadEvent(VRCB_MEDIA_ERROR);

             MovPlaybackParamReset();
             MediaPlaybackConfig( 1 );
             MovPBFunc_StartTimer(300);
             //MovPlayback_Play();
             MovPlayback_Play_MediaError(m_CurPlayTime_MediaError);
         }
    }
    #if 1

    #if(defined(DEVICE_GPIO_2NDSD_PLUG))

    #if(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_MASTER_SLAVE)
    else if( AHC_IsSDInserted() && (m_ulVideoPlayStopStatus == AHC_VIDEO_PLAY) )
    #elif(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_COMPLEMENTARY)
    else if( (AHC_IsSDInserted() || AHC_Is2ndSDInserted()) && (m_ulVideoPlayStopStatus == AHC_VIDEO_PLAY) )
    #endif//TWOSD_WORK_MODEL

    #else//DEVICE_GPIO_2NDSD_PLUG

    else if(AHC_IsSDInserted() && (m_ulVideoPlayStopStatus == AHC_VIDEO_PLAY))

    #endif//DEVICE_GPIO_2NDSD_PLUG
    {
         UINT8 bValue = 0;
         AHC_UF_GetCurrentIndex(&CurrentDcfIdx);
         AHC_UF_GetAllowedChar(chAllowedChar, MAX_ALLOWED_WORD_LENGTH);

         AIHC_StopPlaybackMode();

         if(bValue != AHC_FALSE)
         AHC_FS_IOCtl(AHC_UF_GetRootName(), 4, AHC_FS_CMD_RESET_MEDIUM, NULL, NULL);

         AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect);

         AHC_OS_Sleep(300);
         AHC_UF_SetFreeChar( 0,DCF_SET_FREECHAR,(UINT8*)chAllowedChar);
         AHC_UF_SetCurrentIndex(CurrentDcfIdx);
         SetKeyPadEvent(VRCB_MEDIA_ERROR);
    }
    #endif
    #if 1
    #if(defined(DEVICE_GPIO_2NDSD_PLUG))

    #if(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_MASTER_SLAVE)
    else if( AHC_IsSDInserted() && (uiGetCurrentState() == UI_BROWSER_STATE) )
    #elif(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_COMPLEMENTARY)
    else if( (AHC_IsSDInserted() || AHC_Is2ndSDInserted()) && (uiGetCurrentState() == UI_BROWSER_STATE) )
    #endif//TWOSD_WORK_MODEL

    #else//DEVICE_GPIO_2NDSD_PLUG

    else if(AHC_IsSDInserted() && (uiGetCurrentState() == UI_BROWSER_STATE))

    #endif//DEVICE_GPIO_2NDSD_PLUG
    {
         AHC_FS_IOCtl(AHC_UF_GetRootName(), 4, AHC_FS_CMD_RESET_MEDIUM, NULL, NULL);
         //AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect);
         //AHC_OS_Sleep(200);
    }
    #if 1

    #if(defined(DEVICE_GPIO_2NDSD_PLUG))
    #if(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_MASTER_SLAVE)
    else if( AHC_IsSDInserted() && (uiGetCurrentState() == UI_PLAYBACK_STATE) )
    #elif(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_COMPLEMENTARY)
    else if( (AHC_IsSDInserted() || AHC_Is2ndSDInserted()) && (uiGetCurrentState() == UI_PLAYBACK_STATE) )
    #endif//TWOSD_WORK_MODEL

    #else//DEVICE_GPIO_2NDSD_PLUG

    else if(AHC_IsSDInserted() && (uiGetCurrentState() == UI_PLAYBACK_STATE))

    #endif//DEVICE_GPIO_2NDSD_PLUG
    {
         UINT8 bValue = 0;
         AHC_UF_GetCurrentIndex(&CurrentDcfIdx);
         AHC_UF_GetAllowedChar(chAllowedChar, MAX_ALLOWED_WORD_LENGTH);
         AHC_GetGPIO(AHC_PIO_REG_LGPIO28,&bValue);
         if(bValue != AHC_FALSE)
         AHC_FS_IOCtl(AHC_UF_GetRootName(), 4, AHC_FS_CMD_RESET_MEDIUM, NULL, NULL);

         AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect);
         AHC_OS_Sleep(300);
         AHC_UF_SetFreeChar( 0,DCF_SET_FREECHAR,(UINT8*)chAllowedChar);
         AHC_UF_SetCurrentIndex(CurrentDcfIdx);
         SetKeyPadEvent(VRCB_MEDIA_ERROR);
    }
    #endif
    #if 1

    #if(defined(DEVICE_GPIO_2NDSD_PLUG))
    #if(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_MASTER_SLAVE)

    else if( AHC_IsSDInserted() )
    #elif(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_COMPLEMENTARY)
    else if( AHC_IsSDInserted() || AHC_Is2ndSDInserted() )
    #endif//TWOSD_WORK_MODEL

    #else//DEVICE_GPIO_2NDSD_PLUG

    else if(AHC_IsSDInserted())

    #endif//DEVICE_GPIO_2NDSD_PLUG
    {
         UINT8 bValue = 0;

         //if(VideoFunc_RecordStatus() && MMPF_MP4VENC_GetStatus()!= 0x01)
         if(VideoFunc_RecordStatus())
         {
            // AHC_SetMode(AHC_MODE_IDLE);
             AHC_GetGPIO(AHC_PIO_REG_LGPIO28,&bValue);
             if(bValue != AHC_FALSE)
             AHC_FS_IOCtl(AHC_UF_GetRootName(), 4, AHC_FS_CMD_RESET_MEDIUM, NULL, NULL);

             DrawVideo_UpdatebyEvent(EVENT_VIDEO_KEY_RECORD_STOP);
             //AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 1);
             #if (SUPPORT_ESD_RECOVER_VR)

             //AHC_FS_IOCtl("SD:\\", 4, AHC_FS_CMD_RESET_MEDIUM, NULL, NULL);
             #if 0
             if(MMPF_MP4VENC_GetStatus() == 0x01)
             {
                MMPF_VIDMGR_CloseFile();

                MMPF_MP4VENC_SetStatus(0x04);
                // send INT to host
                MMPF_HIF_SetCpu2HostInt(0x01);
             }
             #endif
             VideoRecMode_RecordwithStatusChecking();
             #endif
         }else
         {
             AHC_GetGPIO(AHC_PIO_REG_LGPIO28,&bValue);
             if(bValue != AHC_FALSE)
             AHC_FS_IOCtl(AHC_UF_GetRootName(), 4, AHC_FS_CMD_RESET_MEDIUM, NULL, NULL);

             AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect);
             AHC_OS_Sleep(300);
             VideoRecMode_RecordwithStatusChecking();
         }

    }
    #endif
    #endif
    #endif
}

void VideoPlayStopCallback(void *Context, MMP_ULONG flag1, MMP_ULONG flag2)
{
#if DAC_NOT_OUTPUT_SPEAKER_HAS_NOISE
    if (AHC_TRUE == AHC_IsSpeakerEnable())
    {
        AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_FALSE);
    }
#endif

    m_ubCallBackStart = AHC_TRUE;
    m_ulVideoPlayStopStatus = flag1;
    //VideoMediaErrorPlayStopHandler();
    m_ubCallBackStart = AHC_FALSE;
}

void AHC_SetVideoPlayStartTime(MMP_ULONG ulStartTime)
{
    m_ulVideoPlayStartTime = ulStartTime;
}

AHC_BOOL AHC_GetVideoPlayStopStatus(UINT32 *pwValue)
{
    *pwValue = m_ulVideoPlayStopStatus;

    return AHC_TRUE;
}

AHC_BOOL AHC_SetVideoPlayStopStatus(UINT32 Value)
{
    m_ulVideoPlayStopStatus = Value;

    return AHC_TRUE;
}

void AudioPlayStopCallback(void *Context, MMP_ULONG flag1, MMP_ULONG flag2)
{
    printc("AudioPlayStopCallback flag1=%d\n", flag1);

    m_ulAudioPlayStopStatus = flag1;
}

AHC_BOOL AHC_GetAudioPlayStopStatus(UINT32 *pwValue)
{
    *pwValue = m_ulAudioPlayStopStatus;

    return AHC_TRUE;
}

AHC_BOOL AHC_SetAudioPlayStopStatus(UINT32 Value)
{
    m_ulAudioPlayStopStatus = Value;

    return AHC_TRUE;
}

/**
 @brief Get current playback file type

 Get current playback file type

 @param[out] pFileType        file type
 @retval AHC_BOOL
*/
AHC_BOOL AIHC_GetCurrentPBFileType(UINT32 *pFileType)
{
    *pFileType = m_ulCurrentPBFileType;

    return AHC_TRUE;
}

AHC_BOOL AIHC_GetCurrentPBHeight(UINT16 *pHeight)
{
    *pHeight = m_ulCurrentPBHeight;

    return AHC_TRUE;
}
#if (SUPPORT_ESD_RECOVER_MOVIEPB == 1)
AHC_BOOL AIHC_SetPlaybackMediaErrorMode(void)
{
    UINT32                      CurrentDcfIdx;
    UINT8                       FileType;
    char                        FilePathName[MAX_FILE_NAME_SIZE];
    UINT32                      Param;
	MMP_DSC_JPEG_INFO           jpeginfo;
    UINT16                      StartX, StartY,DispWidth,DispHeight;
    MMP_ERR                     sRet = MMP_ERR_NONE;
    MMP_ULONG                   ulFileNameLen;
	AHC_DISPLAY_OUTPUTPANEL	    outputPanel;
    MMPS_AUDIO_FILEINFO         AudFileInfo;
    MMPS_VIDEO_CONTAINER_INFO   VideoFileInfo;
    MMPS_VIDPLAY_SCALERCONFIG   cfg;

    #if (AHC_GENERAL_DEBUG)
    printc("%s \n", __func__);
    #endif

    AHC_UF_GetCurrentIndex(&CurrentDcfIdx);

    MEMSET(FilePathName, 0, sizeof(FilePathName));
    AHC_UF_GetFilePathNamebyIndex(CurrentDcfIdx, FilePathName);
    AHC_UF_GetFileTypebyIndex(CurrentDcfIdx, &FileType);

    m_ulCurrentPBFileType = FileType;

    if (FileType == DCF_OBG_JPG)
    {
        STRCPY(jpeginfo.bJpegFileName, FilePathName);

        jpeginfo.usJpegFileNameLength   = STRLEN(FilePathName);
        jpeginfo.ulJpegBufAddr          = 0;
        jpeginfo.ulJpegBufSize          = 0;
        jpeginfo.bDecodeThumbnail       = MMP_FALSE;
        #if (DSC_SUPPORT_BASELINE_MP_FILE)
        jpeginfo.bDecodeLargeThumb 		= MMP_FALSE;
        #endif
        jpeginfo.bValid                 = MMP_FALSE;

        AHC_GetDisplayWindowAttr(&StartX, &StartY, &DispWidth, &DispHeight);

        AHC_GetDisplayOutputDev(&outputPanel);
		MMPS_Display_SetOutputPanel(MMP_DISPLAY_PRM_CTL, outputPanel);


        sRet = MMPS_DSC_PlaybackJpeg(&jpeginfo);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
        
        m_ulPlaybackFileType = FileType;
        return AHC_TRUE;
    }
    else if ( ( FileType == DCF_OBG_MOV ) ||
              ( FileType == DCF_OBG_MP4 ) ||
              ( FileType == DCF_OBG_AVI ) ||
              ( FileType == DCF_OBG_3GP ) ||
              ( FileType == DCF_OBG_WMV ) )
    {
        AIHC_InitAudioExtDACOut();

        MMPS_VIDPLAY_SetMemoryMode(MMP_FALSE);

        #if (SUPPORT_SPEAKER == 1)
        {
            #ifdef TV_SPEAKER_OUT_EN

            #if (SUPPORT_HDMI)
            if(HDMIFunc_GetStatus() == AHC_HDMI_NONE_STATUS)
            #else
            if(1)
            #endif
            {
                AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_TRUE);
            }
            else {
                AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_FALSE);
            }

            #else

            #if (SUPPORT_HDMI && SUPPORT_TV)
            if (HDMIFunc_GetStatus() == AHC_HDMI_NONE_STATUS && TVFunc_Status() == AHC_TV_NONE_STATUS)
            #elif ((!SUPPORT_HDMI) && SUPPORT_TV)
            if (TVFunc_Status() == AHC_TV_NONE_STATUS)
            #elif (SUPPORT_HDMI && (!SUPPORT_TV))
            if(TVFunc_Status()==AHC_HDMI_NONE_STATUS)
            #else
            if(1)
            #endif
            {
                AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_TRUE);
            }
            else {
                AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_FALSE);
            }

            #endif
        }
        #endif

        AHC_GetDisplayOutputDev( &outputPanel );

        MMPS_Display_SetOutputPanelMediaError(MMP_DISPLAY_PRM_CTL, outputPanel);

        STRCPY(m_CurPlayFileName, FilePathName);
        ulFileNameLen = STRLEN(FilePathName);

        MMPS_VIDPLAY_GetFileInfo(m_CurPlayFileName, ulFileNameLen, &VideoFileInfo);
        m_ulCurrentPBHeight = VideoFileInfo.video_info[0].height;

        AHC_Display_GetWidthHdight(&DispWidth, &DispHeight);

		MMPS_VIDPLAY_SetScalerCfg(&cfg, cfg.bUseScaler, DispWidth, DispHeight, 0, 0);
        if ((m_ulCurrentPBHeight == 1088) && (1080 == cfg.ulOutHeight)) {
            // 1080P@60 VR v.s. 1080P output => no scaling
            cfg.ulOutWidth  = 0;
            cfg.ulOutHeight = 0;
            cfg.bUseScaler  = MMP_FALSE;
        }
        else if (cfg.ulOutHeight < m_ulCurrentPBHeight) {
            cfg.bUseScaler = MMP_TRUE;

            if (cfg.ulOutHeight > AHC_HD_VIDPLAY_MAX_HEIGHT) {
                cfg.ulOutWidth = AHC_HD_VIDPLAY_MAX_WIDTH;
                cfg.ulOutHeight = AHC_HD_VIDPLAY_MAX_HEIGHT;
            }
        }
        else {
            cfg.ulOutWidth  = 0;
            cfg.ulOutHeight = 0;
            cfg.bUseScaler  = MMP_FALSE;
        }

        AHC_GetParam(PARAM_ID_DISPLAY_ROTATE_ENABLE,&Param);

        if (Param) {
            MMPS_VIDPLAY_SetDisplayMode(VIDEOPLAY_MODE_MAX, Param, MMP_FALSE, NULL, &cfg);
        }

        // set audio volume
        AHC_GetParam(PARAM_ID_AUDIO_VOLUME_DB      ,&Param);

        #if (AUDIO_SET_DB == 0x01)
        MMPS_AUDIO_SetPlayVolumeDb(Param);
        #else
        MMPS_AUDIO_SetPlayVolume(Param, MMP_FALSE);
        #endif

        //TBD
        MMPS_Display_SetWinActive(FRONT_CAM_WINDOW_ID, MMP_FALSE);

        sRet = MMPS_VIDPLAY_Open(m_CurPlayFileName, ulFileNameLen, m_ulVideoPlayStartTime, MMP_TRUE, MMP_TRUE, &cfg);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
        
        #if 0
        if (err != MMP_ERR_NONE) {

            if(err != MMP_3GPPLAY_ERR_INCORRECT_STATE_OPERATION){
                PRINTF("Close Video\r\n");
                MMPS_VIDPLAY_Close();
            }

            return AHC_FALSE;
        }
        #endif
        m_ulVideoPlayStartTime = 0;

        m_ulPlaybackFileType = FileType;

        m_ulVideoPlayStopStatus = AHC_VIDEO_PLAY;

        sRet = MMPS_VIDPLAY_Play((void *)VideoPlayStopCallback, NULL);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet);} 
        if (sRet != MMP_ERR_NONE) {

            MMPS_VIDPLAY_Close();

            #if (SUPPORT_SPEAKER == 1)
            AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO,AHC_FALSE);
            #endif

            m_ulVideoPlayStopStatus = AHC_VIDEO_PLAY_ERROR_STOP;

            AHC_OSDSetActive(THUMBNAIL_OSD_FRONT_ID, AHC_TRUE);

            return AHC_FALSE;
        }

        AHC_GetParam(PARAM_ID_MOVIE_AUTO_PLAY,&Param);

        if (Param) {
            MMPS_VIDPLAY_Pause();
        }
    }
    else if ( ( FileType == DCF_OBG_MP3 ) ||
              ( FileType == DCF_OBG_WAV ) ||
              ( FileType == DCF_OBG_OGG ) ||
              ( FileType == DCF_OBG_WMA ) )
    {
        AIHC_InitAudioExtDACOut();

        MMPS_AUDIO_SetMediaPath(MMPS_AUDIO_MEDIA_PATH_CARD);

        STRCPY(AudFileInfo.bFileName, FilePathName);
        AudFileInfo.usFileNameLength = STRLEN(FilePathName);

        if ( FileType == DCF_OBG_MP3 ) {
            m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_MP3;
            sRet = MMPS_AUDIO_OpenFile(AudFileInfo, &m_gsAHCMp3Info, MMPS_AUDIO_CODEC_MP3);
        }
        else if ( FileType == DCF_OBG_WAV ) {
            m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_WAVE;
            sRet = MMPS_AUDIO_OpenFile(AudFileInfo, &m_gsAHCWavInfo, MMPS_AUDIO_CODEC_WAV);
        }
        else if ( FileType == DCF_OBG_OGG ) {
            m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_OGG;
            sRet = MMPS_AUDIO_OpenFile(AudFileInfo, &m_gsAHCOggInfo, MMPS_AUDIO_CODEC_OGG);
        }
        else if ( FileType == DCF_OBG_WMA ) {
            m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_WMA;
            sRet = MMPS_AUDIO_OpenFile(AudFileInfo, &m_gsAHCWmaInfo, MMPS_AUDIO_CODEC_WMA);
        }
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
        
        AHC_GetParam(PARAM_ID_AUDIO_VOLUME, &Param);

        #if (AUDIO_SET_DB == 0x01)
        MMPS_AUDIO_SetPlayVolumeDb(Param);
        #else
        MMPS_AUDIO_SetPlayVolume(Param, MMP_FALSE);
        #endif

        m_ulAudioPlayStartTime = 0;

        m_ulPlaybackFileType = FileType;

        m_ulAudioPlayStopStatus = AHC_AUDIO_EVENT_PLAY;

        sRet = MMPS_AUDIO_StartPlay((void *)AudioPlayStopCallback, (void *)"AUDIO");
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); m_ulAudioPlayStopStatus = AHC_AUDIO_EVENT_EOF; return AHC_FALSE;} 

        m_bAHCAudioPlaying = AHC_TRUE;

#if (SUPPORT_SPEAKER == 1)
        #ifdef TV_SPEAKER_OUT_EN

        #if (SUPPORT_HDMI)
        if(HDMIFunc_GetStatus() == AHC_HDMI_NONE_STATUS)
        #else
        if(1)
        #endif
        {
            AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_TRUE);
        }
        else {
            AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_FALSE);
        }

        #else

        #if (SUPPORT_HDMI && SUPPORT_TV)
        if(HDMIFunc_GetStatus()==AHC_HDMI_NONE_STATUS && TVFunc_Status()==AHC_TV_NONE_STATUS)
        #elif ((!SUPPORT_HDMI) && SUPPORT_TV)
        if(TVFunc_Status()==AHC_TV_NONE_STATUS)
        #elif (SUPPORT_HDMI && (!SUPPORT_TV))
        if (TVFunc_Status()==AHC_TV_NONE_STATUS)
        #else
        if(1)
        #endif
        {
            AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_TRUE);
        }
        else {
            AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_FALSE);
        }

        #endif
#endif
    }
    else {
        //TBD
        return AHC_FALSE;
    }

    return AHC_TRUE;

}
#endif //#if (SUPPORT_ESD_RECOVER_MOVIEPB == 1)
/**
 @brief Start Img / Video playback

 Start Img / Video playback

 @retval AHC_BOOL
*/
static AHC_BOOL AIHC_SetPlaybackMode(void)
{
    UINT32                      CurrentDcfIdx;
    UINT8                       FileType;
    char                        FilePathName[MAX_FILE_NAME_SIZE];
    UINT32                      Param;
	MMP_DSC_JPEG_INFO           jpeginfo;
    UINT16                      StartX, StartY,DispWidth,DispHeight;
    MMP_ERR                     sRet = MMP_ERR_NONE;
    MMP_ULONG                   ulFileNameLen;
	AHC_DISPLAY_OUTPUTPANEL	    outputPanel;
    MMPS_AUDIO_FILEINFO         AudFileInfo;
    MMPS_VIDEO_CONTAINER_INFO   VideoFileInfo;
    MMPS_VIDPLAY_SCALERCONFIG   cfg;

    #if (AHC_GENERAL_DEBUG)
    printc("%s,%d \r\n", __func__,__LINE__);
    #endif

    AHC_UF_GetCurrentIndex(&CurrentDcfIdx);

    MEMSET(FilePathName, 0, sizeof(FilePathName));

#if ((UVC_HOST_VIDEO_ENABLE == 1  || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)    
    if( m_ubPlaybackRearCam == AHC_TRUE){
    	AHC_UF_SetRearCamPathFlag(AHC_TRUE);
    }
#endif    

    AHC_UF_GetFilePathNamebyIndex(CurrentDcfIdx, FilePathName);

#if ((UVC_HOST_VIDEO_ENABLE == 1  || SUPPORT_DUAL_SNR == 1) && VIDRECD_MULTI_TRACK == 0)
    if( m_ubPlaybackRearCam == AHC_TRUE){
    	AHC_UF_SetRearCamPathFlag(AHC_FALSE);
    }
#endif    

    AHC_UF_GetFileTypebyIndex(CurrentDcfIdx, &FileType);

    m_ulCurrentPBFileType = FileType;

    if (FileType == DCF_OBG_JPG)
    {
        STRCPY(jpeginfo.bJpegFileName, FilePathName);

        jpeginfo.usJpegFileNameLength   = STRLEN(FilePathName);
        jpeginfo.ulJpegBufAddr          = 0;
        jpeginfo.ulJpegBufSize          = 0;
        jpeginfo.bDecodeThumbnail       = MMP_FALSE;
        #if (DSC_SUPPORT_BASELINE_MP_FILE)
        jpeginfo.bDecodeLargeThumb 		= MMP_FALSE;
        #endif
        jpeginfo.bValid                 = MMP_FALSE;

        AHC_GetDisplayWindowAttr(&StartX, &StartY, &DispWidth, &DispHeight);

        AHC_GetDisplayOutputDev(&outputPanel);
		MMPS_Display_SetOutputPanel(MMP_DISPLAY_PRM_CTL, outputPanel);

        #if defined(CCIR656_OUTPUT_ENABLE)&&(CCIR656_OUTPUT_ENABLE)
		if (outputPanel == MMP_DISPLAY_SEL_NTSC_TV) {
            MMPS_DSC_SetPlaybackMode(DSC_TV_NTSC_DECODE_MODE);
        }
		else if (outputPanel == MMP_DISPLAY_SEL_PAL_TV) {
            MMPS_DSC_SetPlaybackMode(DSC_TV_PAL_DECODE_MODE);
        }
		else if (outputPanel == MMP_DISPLAY_SEL_HDMI) {
            MMPS_DSC_SetPlaybackMode(DSC_HDMI_DECODE_MODE);
        }
        else if (outputPanel == MMP_DISPLAY_SEL_CCIR) {	
            MMPS_DSC_SetPlaybackMode(DSC_CCIR_DECODE_MODE);
        }
        else {
            MMPS_DSC_SetPlaybackMode(DSC_NORMAL_DECODE_MODE);
        }
        #endif

        sRet = MMPS_DSC_PlaybackJpeg(&jpeginfo);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
        
        m_ulPlaybackFileType = FileType;
        return AHC_TRUE;
    }
    else if ( ( FileType == DCF_OBG_MOV ) ||
              ( FileType == DCF_OBG_MP4 ) ||
              ( FileType == DCF_OBG_AVI ) ||
              ( FileType == DCF_OBG_3GP ) ||
              ( FileType == DCF_OBG_WMV ) )
    {
        AIHC_InitAudioExtDACOut();

        MMPS_VIDPLAY_SetMemoryMode(MMP_FALSE);

#if DAC_NOT_OUTPUT_SPEAKER_HAS_NOISE
		AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_FALSE);
#else
        #if (SUPPORT_SPEAKER == 1)
        {
            #ifdef TV_SPEAKER_OUT_EN

            #if (SUPPORT_HDMI)
            if(HDMIFunc_GetStatus() == AHC_HDMI_NONE_STATUS)
            #else
            if(1)
            #endif
            {
                AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_TRUE);
            }
            else {
                AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_FALSE);
            }

            #else

            #if (SUPPORT_HDMI && SUPPORT_TV)
            if(HDMIFunc_GetStatus()==AHC_HDMI_NONE_STATUS && TVFunc_Status()==AHC_TV_NONE_STATUS)
            #elif ((!SUPPORT_HDMI) && SUPPORT_TV)
            if(TVFunc_Status()==AHC_TV_NONE_STATUS)
            #elif (SUPPORT_HDMI && (!SUPPORT_TV))
            if (TVFunc_Status()==AHC_TV_NONE_STATUS)
            #else
            if(1)
            #endif
            {
                AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_TRUE);
            }
            else {
                AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_FALSE);
            }

            #endif
        }
        #endif
#endif

        AHC_GetDisplayOutputDev( &outputPanel );

		MMPS_Display_SetOutputPanel(MMP_DISPLAY_PRM_CTL, outputPanel);

#if (SUPPORT_SYS_CALIBRATION)
        if(uiGetCurrentState() == UI_SYS_CALIBRATION_STATE) {
            STRCPY(m_CurPlayFileName, "SD:\\test\\VideoRec.MOV");
            ulFileNameLen = STRLEN("SD:\\test\\VideoRec.MOV");
        }
        else
#endif
        {
            STRCPY(m_CurPlayFileName, FilePathName);
            ulFileNameLen = STRLEN(FilePathName);
        }

        AHC_GetParam(PARAM_ID_DISPLAY_ROTATE_ENABLE,&Param);

        if ( Param ) {

            MMPS_VIDPLAY_GetCurFileResolution(&m_ulCurrentPBWidth, &m_ulCurrentPBHeight);

            AHC_Display_GetWidthHdight(&DispWidth, &DispHeight);//#if defined(CCIR656_OUTPUT_ENABLE)&&(CCIR656_OUTPUT_ENABLE)

			MMPS_VIDPLAY_SetScalerCfg(&cfg, cfg.bUseScaler, DispWidth, DispHeight, 0, 0);
            if ((m_ulCurrentPBHeight == 1088) && (1080 == cfg.ulOutHeight)) {
                // 1080P@60 VR v.s. 1080P output => no scaling
                cfg.ulOutWidth  = 0;
                cfg.ulOutHeight = 0;
                cfg.bUseScaler  = MMP_FALSE;
            }
            else if (cfg.ulOutHeight < m_ulCurrentPBHeight) {
                cfg.bUseScaler = MMP_TRUE;
                
                if (cfg.ulOutHeight > AHC_HD_VIDPLAY_MAX_HEIGHT) {
                    cfg.ulOutWidth = AHC_HD_VIDPLAY_MAX_WIDTH;
                    cfg.ulOutHeight = AHC_HD_VIDPLAY_MAX_HEIGHT;
                }
            }
            else {
                cfg.ulOutWidth  = 0;
                cfg.ulOutHeight = 0;
                cfg.bUseScaler = MMP_FALSE;
            }
            MMPS_VIDPLAY_SetDisplayMode(VIDEOPLAY_MODE_MAX, Param, MMP_FALSE, NULL, &cfg);
        }
        else//HDMI TV out
        {
            AHC_DISPLAY_OUTPUTPANEL sOutputDevice;
            AHC_GetDisplayOutputDev(&sOutputDevice);
            if(sOutputDevice == AHC_DISPLAY_NTSC_TV)
                 MMPS_VIDPLAY_SetDisplayMode(VIDEOPLAY_MODE_NTSC_TV, Param, MMP_TRUE, NULL, &cfg);
            else if(sOutputDevice == AHC_DISPLAY_PAL_TV)
                MMPS_VIDPLAY_SetDisplayMode(VIDEOPLAY_MODE_PAL_TV, Param, MMP_TRUE, NULL, &cfg);
            else if(sOutputDevice == AHC_DISPLAY_HDMI)
                MMPS_VIDPLAY_SetDisplayMode(VIDEOPLAY_MODE_HDMI, Param, MMP_TRUE, NULL, &cfg);
        }

        // set audio volume
        AHC_GetParam(PARAM_ID_AUDIO_VOLUME_DB      ,&Param);

        #if (AUDIO_SET_DB == 0x01)
        MMPS_AUDIO_SetPlayVolumeDb(Param);
        #else
        MMPS_AUDIO_SetPlayVolume(Param, MMP_FALSE);
        #endif

        //TBD
	    MMPS_Display_SetWinActive(FRONT_CAM_WINDOW_ID, MMP_FALSE);
#if !(EN_SPEED_UP_VID & PB_CASE)
        sRet = MMPS_VIDPLAY_GetFileInfo(m_CurPlayFileName, ulFileNameLen, &VideoFileInfo);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(0,sRet); return AHC_FALSE;} 
        
        m_ulCurrentPBHeight = VideoFileInfo.video_info[0].height;

        AHC_Display_GetWidthHdight(&DispWidth, &DispHeight);//#if defined(CCIR656_OUTPUT_ENABLE)&&(CCIR656_OUTPUT_ENABLE)

        cfg.ulOutWidth = DispWidth;
        cfg.ulOutHeight = DispHeight;
        if ((m_ulCurrentPBHeight == 1088) && (1080 == cfg.ulOutHeight)) {
            // 1080P@60 VR v.s. 1080P output => no scaling
            cfg.ulOutWidth  = 0;
            cfg.ulOutHeight = 0;
            cfg.bUseScaler  = MMP_FALSE;
        }
        else if (cfg.ulOutHeight < m_ulCurrentPBHeight) {
            cfg.bUseScaler = MMP_TRUE;
            
            if (cfg.ulOutHeight > AHC_HD_VIDPLAY_MAX_HEIGHT) {
                cfg.ulOutWidth = AHC_HD_VIDPLAY_MAX_WIDTH;
                cfg.ulOutHeight = AHC_HD_VIDPLAY_MAX_HEIGHT;
            }
        }
        else {
            cfg.ulOutWidth  = 0;
            cfg.ulOutHeight = 0;
            cfg.bUseScaler = MMP_FALSE;
        }
#else
        AHC_Display_GetWidthHdight(&DispWidth, &DispHeight);//#if defined(CCIR656_OUTPUT_ENABLE)&&(CCIR656_OUTPUT_ENABLE)
		MMPS_VIDPLAY_SetScalerCfg(&cfg, cfg.bUseScaler, DispWidth, DispHeight, AHC_HD_VIDPLAY_MAX_WIDTH, AHC_HD_VIDPLAY_MAX_HEIGHT);
#endif
        if( m_ubPlaybackRearCam == AHC_TRUE )
        {
            MMPS_VIDPLAY_SetCurrentTrack(1);  //playback rear cam.
        }
        else
        {
            MMPS_VIDPLAY_SetCurrentTrack(0);  //playback front cam.
        }

        sRet = MMPS_VIDPLAY_Open(m_CurPlayFileName, ulFileNameLen, m_ulVideoPlayStartTime, MMP_TRUE, MMP_TRUE, &cfg);
#if (EN_SPEED_UP_VID & PB_CASE)		
		MMPS_VIDPLAY_GetCurFileResolution(&m_ulCurrentPBWidth, &m_ulCurrentPBHeight);
#endif		
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(0,sRet); return AHC_FALSE;} 

        #if 0
        if (err != MMP_ERR_NONE) {

            if(err != MMP_3GPPLAY_ERR_INCORRECT_STATE_OPERATION){
                PRINTF("Close Video\r\n");
                MMPS_VIDPLAY_Close();
            }

            return AHC_FALSE;
        }
        #endif

        m_ulVideoPlayStartTime = 0;

        m_ulPlaybackFileType = FileType;

        m_ulVideoPlayStopStatus = AHC_VIDEO_PLAY;

        sRet = MMPS_VIDPLAY_Play((void *)VideoPlayStopCallback, NULL);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet);} 

        if (sRet != MMP_ERR_NONE) {

            MMPS_VIDPLAY_Close();

            AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_FALSE);

            m_ulVideoPlayStopStatus = AHC_VIDEO_PLAY_ERROR_STOP;

            AHC_OSDSetActive(THUMBNAIL_OSD_FRONT_ID, AHC_TRUE);

            return AHC_FALSE;
        }

#if DAC_NOT_OUTPUT_SPEAKER_HAS_NOISE
        #if (SUPPORT_SPEAKER == 1)
		{
            #ifdef TV_SPEAKER_OUT_EN
		
            #if (SUPPORT_HDMI)
					if(HDMIFunc_GetStatus() == AHC_HDMI_NONE_STATUS)
            #else
					if(1)
            #endif
					{
						AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_TRUE);
					}
					else {
						AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_FALSE);
					}
		
            #else
		
            #if (SUPPORT_HDMI && SUPPORT_TV)
					if(HDMIFunc_GetStatus()==AHC_HDMI_NONE_STATUS && TVFunc_Status()==AHC_TV_NONE_STATUS)
            #elif ((!SUPPORT_HDMI) && SUPPORT_TV)
					if(TVFunc_Status()==AHC_TV_NONE_STATUS)
            #elif (SUPPORT_HDMI && (!SUPPORT_TV))
					if (TVFunc_Status()==AHC_TV_NONE_STATUS)
            #else
					if(1)
            #endif
					{
						AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_TRUE);
					}
					else {
						AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_FALSE);
					}
		
            #endif
		}
        #endif
#endif

        AHC_GetParam(PARAM_ID_MOVIE_AUTO_PLAY,&Param);

        if(Param) {
            MMPS_VIDPLAY_Pause();
        }
    }
    else if ( ( FileType == DCF_OBG_MP3 ) ||
              ( FileType == DCF_OBG_WAV ) ||
              ( FileType == DCF_OBG_OGG ) ||
              ( FileType == DCF_OBG_WMA ) )
    {
        AIHC_InitAudioExtDACOut();

        MMPS_AUDIO_SetMediaPath(MMPS_AUDIO_MEDIA_PATH_CARD);

        STRCPY(AudFileInfo.bFileName, FilePathName);
        AudFileInfo.usFileNameLength = STRLEN(FilePathName);

        if ( FileType == DCF_OBG_MP3 ) {
            m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_MP3;
            sRet = MMPS_AUDIO_OpenFile(AudFileInfo, &m_gsAHCMp3Info, MMPS_AUDIO_CODEC_MP3);
        }
        else if ( FileType == DCF_OBG_WAV ) {
            m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_WAVE;
            sRet = MMPS_AUDIO_OpenFile(AudFileInfo, &m_gsAHCWavInfo, MMPS_AUDIO_CODEC_WAV);
        }
        else if ( FileType == DCF_OBG_OGG ) {
            m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_OGG;
            sRet = MMPS_AUDIO_OpenFile(AudFileInfo, &m_gsAHCOggInfo, MMPS_AUDIO_CODEC_OGG);
        }
        else if ( FileType == DCF_OBG_WMA ) {
            m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_WMA;
            sRet = MMPS_AUDIO_OpenFile(AudFileInfo, &m_gsAHCWmaInfo, MMPS_AUDIO_CODEC_WMA);
        }
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

        AHC_GetParam(PARAM_ID_AUDIO_VOLUME, &Param);

        #if (AUDIO_SET_DB == 0x01)
        MMPS_AUDIO_SetPlayVolumeDb(Param);
        #else
        MMPS_AUDIO_SetPlayVolume(Param, MMP_FALSE);
        #endif

        m_ulAudioPlayStartTime = 0;

        m_ulPlaybackFileType = FileType;

        m_ulAudioPlayStopStatus = AHC_AUDIO_EVENT_PLAY;

        sRet = MMPS_AUDIO_StartPlay((void *)AudioPlayStopCallback, (void *)"AUDIO");
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); m_ulAudioPlayStopStatus = AHC_AUDIO_EVENT_EOF; return AHC_FALSE;} 

        m_bAHCAudioPlaying = AHC_TRUE;

#if (SUPPORT_SPEAKER == 1)
        #ifdef TV_SPEAKER_OUT_EN

        #if (SUPPORT_HDMI)
        if(HDMIFunc_GetStatus() == AHC_HDMI_NONE_STATUS)
        #else
        if(1)
        #endif
        {
            AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_TRUE);
        }
        else {
            AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_FALSE);
        }

        #else

        #if (SUPPORT_HDMI && SUPPORT_TV)
        if(HDMIFunc_GetStatus()==AHC_HDMI_NONE_STATUS && TVFunc_Status()==AHC_TV_NONE_STATUS)
        #elif ((!SUPPORT_HDMI) && SUPPORT_TV)
        if(TVFunc_Status()==AHC_TV_NONE_STATUS)
        #elif (SUPPORT_HDMI && (!SUPPORT_TV))
        if (TVFunc_Status()==AHC_TV_NONE_STATUS)
        #else
        if(1)
        #endif
        {
            AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_TRUE);
        }
        else {
            AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_FALSE);
        }

        #endif
#endif
    }
    else {
        //TBD
        return AHC_FALSE;
    }

    return AHC_TRUE;
}

#if (SUPPORT_ESD_RECOVER_MOVIEPB == 1)
AHC_BOOL AHC_SetPlaybackMode_MediaError(UINT32 ResumeTime)
{
        m_ulVideoPlayStartTime = ResumeTime;
        AIHC_SetPlaybackMediaErrorMode();
        return AHC_TRUE;
}
#endif

/**
 @brief Stop Img / Video playback

 Stop Img / Video playback

 @retval AHC_BOOL
*/
AHC_BOOL AIHC_StopPlaybackMode(void)
{
    MMP_M_STATE 	ubVideoState;
    MMP_ULONG 		KeyFrameNum;

    if (  ( m_ulPlaybackFileType == DCF_OBG_MOV ) ||
          ( m_ulPlaybackFileType == DCF_OBG_MP4 ) ||
          ( m_ulPlaybackFileType == DCF_OBG_AVI ) ||
          ( m_ulPlaybackFileType == DCF_OBG_3GP ) ||
          ( m_ulPlaybackFileType == DCF_OBG_WMV ) ) {

        MMPS_VIDPLAY_GetState(&ubVideoState);

        if(ubVideoState != MMP_M_STATE_IDLE){
            MMPS_VIDPLAY_Stop(&KeyFrameNum);
        }

        MMPS_VIDPLAY_Close();

        #if (SUPPORT_SPEAKER == 1)
        AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO,AHC_FALSE);
        #endif
        //turn off PIP window, to prevent customers redraw GUI without setting PIP properly
        //MMPS_Display_SetWinActive(MMPS_DISPLAY_WIN_PIP, MMP_FALSE);
    }
    else if ( ( m_ulPlaybackFileType == DCF_OBG_MP3 ) ||
              ( m_ulPlaybackFileType == DCF_OBG_WAV ) ||
              ( m_ulPlaybackFileType == DCF_OBG_OGG ) ||
              ( m_ulPlaybackFileType == DCF_OBG_WMA ) ) {

        MMPS_AUDIO_StopPlay();
        #if (SUPPORT_SPEAKER == 1)
        AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_FALSE);
        #endif
        m_bAHCAudioPlaying = AHC_FALSE;
    }
    else if ( m_ulPlaybackFileType == DCF_OBG_JPG ){
        MMPS_DSC_ExitJpegPlayback();
    }

    //turn off PIP window, to prevent customers redraw GUI without setting PIP properly
    //MMPS_Display_SetWinActive(MMPS_DISPLAY_WIN_PIP, MMP_FALSE);

    return AHC_TRUE;
}

#if 0
void _____Browser_Function_________(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : AHC_SetThumbnailMode
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set thumbnail mode

 Set thumbnail mode

 @retval AHC_BOOL
*/
static AHC_BOOL AHC_SetThumbnailMode(void)
{
    AHC_Thumb_DrawPage(AHC_TRUE);
    return AHC_TRUE;
}

#if 0
void _____I2C_Function_________(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : AHC_ConfigI2CAccess
//  Description :
//------------------------------------------------------------------------------
/**
 @brief I2C config

 This API configures following attributes of I2C so that system can send AHC_GetI2C and AHC_SendI2C to specified configuration.
 Parameters:
 @param[in] AhcI2cHandle This variable will get the handle back after configuration completed.
 @param[in] AHC_I2CM_ATTRIBUTE The attributes of I2C.
 @retval TRUE or FALSE.
*/
AHC_BOOL AHC_ConfigI2CAccess(AHC_I2C_HANDLE* AhcI2cHandle, AHC_I2CM_ATTRIBUTE sI2cAttr)
{
    MMP_UBYTE ubFreeHandlerIndex = AHC_I2C_HANDLE_MAX;
    MMP_UBYTE i = 0;

    if (m_ulI2cHandle_count < AHC_I2C_HANDLE_MAX) {

        for(i = 0; i < m_ulI2cHandle_count; i++) {

            if((ubFreeHandlerIndex == AHC_I2C_HANDLE_MAX)&&(m_AhcI2cmAttribute[i].ubRegLen == 0)&&(m_AhcI2cmAttribute[i].ubDataLen == 0)) {
                ubFreeHandlerIndex = i;
            }
            else if((m_AhcI2cmAttribute[i].ubSlaveAddr == sI2cAttr.bySlaveAddr) && (m_AhcI2cmAttribute[i].ubPadNum == sI2cAttr.byPadNum)) {
                m_AhcI2cmAttribute[i].i2cmID        = sI2cAttr.byI2cmID;
                m_AhcI2cmAttribute[i].ubRegLen      = sI2cAttr.byRegLen;
                m_AhcI2cmAttribute[i].ubDataLen     = sI2cAttr.byDataLen;
                m_AhcI2cmAttribute[i].ulI2cmSpeed   = sI2cAttr.ulClk;
                m_AhcI2cmAttribute[i].ubDelayTime   = sI2cAttr.bySwDelayTime;
                m_AhcI2cmAttribute[i].sw_clk_pin    = AIHC_GetSystemPio(sI2cAttr.bySwClkPinGourp, sI2cAttr.bySwClkPinNum);
                m_AhcI2cmAttribute[i].sw_data_pin   = AIHC_GetSystemPio(sI2cAttr.bySwDataPinGourp, sI2cAttr.bySwDataPinNum);
                *AhcI2cHandle = &m_AhcI2cmAttribute[i];
                return AHC_TRUE;
            }
        }

        if(ubFreeHandlerIndex == AHC_I2C_HANDLE_MAX) {
            ubFreeHandlerIndex = m_ulI2cHandle_count;
            m_ulI2cHandle_count ++;
        }
        m_AhcI2cmAttribute[ubFreeHandlerIndex].i2cmID       = sI2cAttr.byI2cmID;
        m_AhcI2cmAttribute[ubFreeHandlerIndex].ubSlaveAddr  = sI2cAttr.bySlaveAddr;
        m_AhcI2cmAttribute[ubFreeHandlerIndex].ubRegLen     = sI2cAttr.byRegLen;
        m_AhcI2cmAttribute[ubFreeHandlerIndex].ubDataLen    = sI2cAttr.byDataLen;
        m_AhcI2cmAttribute[ubFreeHandlerIndex].ubPadNum     = sI2cAttr.byPadNum;
        m_AhcI2cmAttribute[ubFreeHandlerIndex].ulI2cmSpeed  = sI2cAttr.ulClk;
        m_AhcI2cmAttribute[ubFreeHandlerIndex].ubDelayTime  = sI2cAttr.bySwDelayTime;
        m_AhcI2cmAttribute[ubFreeHandlerIndex].sw_clk_pin   = AIHC_GetSystemPio(sI2cAttr.bySwClkPinGourp, sI2cAttr.bySwClkPinNum);
        m_AhcI2cmAttribute[ubFreeHandlerIndex].sw_data_pin  = AIHC_GetSystemPio(sI2cAttr.bySwDataPinGourp, sI2cAttr.bySwDataPinNum);

        *AhcI2cHandle = &m_AhcI2cmAttribute[ubFreeHandlerIndex];

        return AHC_TRUE;
    }
    else {
        return AHC_FALSE;
    }
}

//------------------------------------------------------------------------------
//  Function    : AHC_SendI2C
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Write I2C command

 Send data over I2C interface to specific device according to the configuration set by AHC_ConfigI2Caccess().
 Parameters:
 @param[in] AhcI2cHandle The handle of pre-opened operation
 @param[in] wSubAdde The register sub address.
 @param[in] wData The data to be sent.
 @retval TRUE or FALSE.
*/
AHC_BOOL AHC_SendI2C(AHC_I2C_HANDLE AhcI2cHandle, UINT16 wSubAdde, UINT16 wData)
{
	if (MMPS_I2cm_WriteReg((MMP_I2CM_ATTR*)AhcI2cHandle, wSubAdde, wData) != MMP_ERR_NONE) {
        return AHC_FALSE;
    }
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetI2C
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Read I2C command

 Receives data over I2C interface to specific device according to the configuration set by AHC_ConfigI2Caccess().
 Parameters:
 @param[in] AhcI2cHandle The handle of pre-opened operation
 @param[in] wSubAdde The register sub address.//Note: usSubAdde is used for stand for data-count in the case ubRegLen = 0
 @param[in] *pwData The received data.
 @retval TRUE or FALSE.
*/
AHC_BOOL AHC_GetI2C(AHC_I2C_HANDLE AhcI2cHandle, UINT16 wSubAdde, UINT16* pwData)
{
	if (MMPS_I2cm_ReadReg((MMP_I2CM_ATTR*)AhcI2cHandle, wSubAdde, pwData) != MMP_ERR_NONE) {
        return AHC_FALSE;
    }
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_WriteI2CBurst
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Write I2C command burst

 Send data over I2C interface to specific device according to the configuration set by AHC_ConfigI2Caccess(). The data
 is taken from AHC_WRITE_I2C_MEM_LOCATION memory location, it needs to be assigned by AHC_SetMemLocationPtr().
 Parameters:
 @param[in] AhcI2cHandle The handle of pre-opened operation
 @param[in] wSubAdde The register sub address.
 @param[in] wData Number of words to be sent.
 @retval TRUE or FALSE.
*/
AHC_BOOL AHC_WriteI2CBurst(AHC_I2C_HANDLE AhcI2cHandle, UINT16 wSubAdde, UINT16 wData)
{
	if (MMPS_I2cm_WriteBurstData((MMP_I2CM_ATTR*)AhcI2cHandle, wSubAdde, wData, glAhcMemoryLocation[AHC_WRITE_I2C_MEM_LOCATION].ulLocationAddr) != MMP_ERR_NONE) {
        return AHC_FALSE;
    }
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_ReadI2CBurst
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Read I2C command burst

 Receive data from I2C interface in burst mode. The data is taken from AHC_READ_I2C_MEM_LOCATION memory location, it
 needs to be assigned by AHC_SetMemLocationPtr().
 Parameters:
 @param[in] AhcI2cHandle The handle of pre-opened operation
 @param[in] wSubAdde The register sub address.
 @param[in] wData Number of words to be read..
 @retval TRUE or FALSE.
*/
AHC_BOOL AHC_ReadI2CBurst(AHC_I2C_HANDLE AhcI2cHandle, UINT16 wSubAdde, UINT16 wData)
{
	if (MMPS_I2cm_ReadBurstData((MMP_I2CM_ATTR*)AhcI2cHandle, wSubAdde, wData, glAhcMemoryLocation[AHC_READ_I2C_MEM_LOCATION].ulLocationAddr) != MMP_ERR_NONE) {
        return AHC_FALSE;
    }
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_CloseI2C
//  Description :
//------------------------------------------------------------------------------
/**
 @brief free I2C attribute

 Free I2C attribute .
 Parameters:
 @param[in] AhcI2cHandle The handle of pre-opened operation
 @retval TRUE or FALSE.
*/
AHC_BOOL AHC_CloseI2C(AHC_I2C_HANDLE AhcI2cHandle)
{
	MMP_I2CM_ATTR* i2cm_ptr = (MMP_I2CM_ATTR*)AhcI2cHandle;

    i2cm_ptr->ubRegLen = 0;
    i2cm_ptr->ubDataLen = 0;
    return AHC_TRUE;
}

#if 0
void _____UART_Function_________(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : AHC_UartLibConfig
//  Description :
//------------------------------------------------------------------------------
/**
    @brief  Config UART module
    @param[in] bOpen : flag to indicate open/close
    @param[in] bUartID : select the UART port to be configured
    @param[in] bBaudRate : baud rete
    @param[in] bPadNum : HW pad map.
    @param[in] bPolarity : uart polarity setting
    @param[in] wLineCTL : configuration of line controls
    @return  the status of the operation
*/
AHC_BOOL AHC_UartLibConfig(UINT8 bBaudRate, UINT8 bPolarity, UINT8 bPadNum, UINT16 wLineCTL, UINT8 bFlowCtl, AHC_UART_HANDLE* pAhcUartHandle)
{
    MMP_UBYTE bSelParity = 0;
	MMP_UART_ATTR uartattribute;

    switch(bBaudRate) {
        case AHC_UART_BAUD_RATE_9600:
            uartattribute.ulBaudrate = 9600;
            break;
        case AHC_UART_BAUD_RATE_14400:
            uartattribute.ulBaudrate = 14400;
            break;
        case AHC_UART_BAUD_RATE_19200:
            uartattribute.ulBaudrate = 19200;
            break;
        case AHC_UART_BAUD_RATE_38400:
            uartattribute.ulBaudrate = 38400;
            break;
        case AHC_UART_BAUD_RATE_57600:
            uartattribute.ulBaudrate = 57600;
            break;
        case AHC_UART_BAUD_RATE_115200:
            uartattribute.ulBaudrate = 115200;
            break;
        default:
            PRINTF("Un-supported UART baudrate\r\n");
            return AHC_FALSE;
            break;
    }

    if(bFlowCtl != 0) {  //Turn-on flow control
        uartattribute.bFlowCtlEn = MMP_TRUE;
        uartattribute.parity = bPolarity;
    }

    if(wLineCTL & AHC_UART_PARITY_EN) {

        bSelParity = (wLineCTL & AHC_UART_SEL_PARITY) >> 3;

        switch(bSelParity) {
            case 0x0: // odd parity
				uartattribute.parity = MMP_UART_PARITY_ODD;
                break;
            case 0x1: // even parity
				uartattribute.parity = MMP_UART_PARITY_EVEN;
                break;
            case 0x10: //force to 0
				uartattribute.parity = MMP_UART_PARITY_FORCE0;
                break;
            case 0x11: //force to 1
				uartattribute.parity = MMP_UART_PARITY_FORCE1;
                break;
            default:
                break;
        }
        uartattribute.bParityEn = MMP_TRUE;
    }

    uartattribute.padset = bPadNum;

	if(m_AhcUartFree[MMP_UART_ID_1 - 1] == MMP_FALSE) {

		*pAhcUartHandle = (AHC_UART_HANDLE)MMP_UART_ID_1;

		if( MMPS_Uart_Open(MMP_UART_ID_1, &uartattribute) == MMP_ERR_NONE) {
			m_AhcUartFree[MMP_UART_ID_1 - 1] = MMP_TRUE;
            return AHC_TRUE;
        }
    }
	else if(m_AhcUartFree[MMP_UART_ID_2 - 1] == MMP_FALSE) {

		*pAhcUartHandle = (AHC_UART_HANDLE)MMP_UART_ID_2;

		if( MMPS_Uart_Open(MMP_UART_ID_2, &uartattribute) == MMP_ERR_NONE) {
			m_AhcUartFree[MMP_UART_ID_2 - 1] = MMP_TRUE;
            return AHC_TRUE;
        }
    }
    else {
        PRINTF("AHC Error! No Free UART \r\n");
        return AHC_FALSE;
    }

    return AHC_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_UartLibClose
//  Description :
//------------------------------------------------------------------------------
/**
    @brief  Closing the opened UART port
    @param[in] AhcUartHandle : The configured UART handle
    @retval TRUE or FALSE. // TRUE: Success, FALSE: Fail
*/
AHC_BOOL AHC_UartLibClose(AHC_UART_HANDLE AhcUartHandle)
{
	MMP_UART_ID uart_id = (MMP_ULONG)AhcUartHandle;
    if(MMPS_Uart_Close(uart_id) == MMP_ERR_NONE) {
        m_AhcUartFree[uart_id - 1] = MMP_FALSE;
        return AHC_TRUE;
    }
    return AHC_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_UartLibRead
//  Description :
//------------------------------------------------------------------------------
/**
    @brief  UART read function
    @param[in] bUartID : select the UART port to be configured
    @param[in] ulSize : number of bytes to be read from UART port
    @param[in] ubFlag : special read flag if necessary.
    @return  the status of the operation
*/
AHC_BOOL AHC_UartLibRead(AHC_UART_HANDLE AhcUartHandle, UINT16 wSize, UINT8 bFlag)
{
    /*if(MMPS_UART_ReadLib(bUartID, wSize, bFlag, AHC_UART_READ_MEM_LOCATION) != MMP_ERR_NONE) {
        return AHC_FALSE;
    }*/
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_UartLibWrite
//  Description :
//------------------------------------------------------------------------------
/**
    @brief  UART write out funtion
    @param[in] bUartID : select the UART port to be configured
    @param[in] usSize : number of bytes to be sent to UART port
    @return  the status of the operation
*/
AHC_BOOL AHC_UartLibWrite(AHC_UART_HANDLE AhcUartHandle, UINT16 wSize)
{
	MMP_UART_ID uart_id;

	if(AhcUartHandle == (AHC_UART_HANDLE)MMP_UART_ID_1) {
		uart_id = MMP_UART_ID_1;
    }

	if(AhcUartHandle == (AHC_UART_HANDLE)MMP_UART_ID_2) {
		uart_id = MMP_UART_ID_2;
    }
    else {
        PRINTF("AHC error uart ID !\r\n");
        return AHC_FALSE;
    }

	if(MMPS_UART_UseTxDMA(uart_id, (MMP_ULONG)glAhcMemoryLocation[AHC_UART_WRITE_MEM_LOCATION].ulLocationAddr, wSize, MMP_UART_TXDMA_END_EN, MMP_FALSE, NULL) == MMP_ERR_NONE) {
        return AHC_TRUE;
    }

    return AHC_FALSE;
}

#if 0
void _____General_Function_________(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : AHC_Initialize
//  Description :
//------------------------------------------------------------------------------
/**
@brief The intialize code that should be called once after system power-up

 The intialize code that should be called once after system power-up
 Parameters:
 @retval TRUE or FALSE. // TRUE: Success, FALSE: Fail
*/
AHC_BOOL AHC_Initialize(void)
{
    if ( !m_bAHCGeneralInit )
    {
        AHC_PrintBuildTime();
        AHC_PrintFwVersion();
        AIHC_PARAM_Initialize();
        AIHC_InitGeneralFunc();
        AIHC_InitAHLMessage();
        AIHC_InitAHLMessage_HP();
        AHC_WMSG_Config();
        MMPS_VIDPLAY_InitConfig(MMPS_VIDPLAY_GetConfig());

        m_bAHCGeneralInit = AHC_TRUE;
    }

    // Note: Please keep below function call to make sure functions in AHC_Callback.c can work
    AHC_CheckCallbackExisted();

    #if (FS_FORMAT_FREE_ENABLE)
    MMPS_FORMATFREE_Enable( 1 );
    #endif

    return AHC_TRUE;
}

/**
@brief  read the system required file back to the SD card
*/
AHC_BOOL AHC_LoadSystemFile(void)
{
    #if (USE_INFO_FILE)
    AHC_ReadSDInfoFile();
    #endif
    return AHC_TRUE;
}
/**
@brief  write the system required file back to the SD card
*/
AHC_BOOL AHC_UnloadSystemFile(void)
{
    #if (USE_INFO_FILE)
    AHC_WriteSDInfoFile();
    #endif
    AHC_UF_WriteInfo();
    return AHC_TRUE;
}

void AIHC_SelectYFrameUsage( void )
{
	UINT8 byType = MMPS_3GPRECD_Y_FRAME_TYPE_NONE;
		
#if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE && MOTION_DETECTION_EN)

	if ( uiGetParkingModeEnable() == AHC_TRUE )
	{
	    byType = MMPS_3GPRECD_Y_FRAME_TYPE_VMD;
        
        if( AHC_IsADASEnabled() == AHC_TRUE )
        {
            AHC_EnableADAS( AHC_FALSE );
        }
	}

#elif ((SUPPORT_PARKINGMODE == PARKINGMODE_NONE) && MOTION_DETECTION_EN) // TBD

    if (MenuSettingConfig()->uiMotionDtcSensitivity != MOTION_DTC_SENSITIVITY_OFF)
    {
        byType = MMPS_3GPRECD_Y_FRAME_TYPE_VMD;
    }

#endif

#if (ENABLE_ADAS_LDWS || ENABLE_ADAS_FCWS || ENABLE_ADAS_SAG)

	if(byType != MMPS_3GPRECD_Y_FRAME_TYPE_VMD)
    {
        UINT32 bLDWS_En = COMMON_LDWS_EN_OFF; 
        UINT32 bFCWS_En = COMMON_FCWS_EN_OFF;
        UINT32 bSAG_En  = COMMON_SAG_EN_OFF;
        UINT32 uiAdasFlag = 0;
        AHC_GetParam( PARAM_ID_ADAS, &uiAdasFlag );
		
	#if (ENABLE_ADAS_LDWS)	
        #ifdef CFG_ADAS_MENU_SETTING_OLD_STYLE // temporary solution, it might be defined in Config_SDK.h
        if( MenuSettingConfig()->uiLDWS == COMMON_LDWS_EN_ON )
        #else
		if( uiAdasFlag & AHC_ADAS_ENABLED_LDWS )
        #endif
        {
            bLDWS_En = COMMON_LDWS_EN_ON;
            MMPS_Sensor_SetADASFeatureEn(MMPS_ADAS_LDWS, MMP_TRUE);
        }
        else
        {
            MMPS_Sensor_SetADASFeatureEn(MMPS_ADAS_LDWS, MMP_FALSE);
        }
	#endif
	
	#if (ENABLE_ADAS_FCWS)	
        #ifdef CFG_ADAS_MENU_SETTING_OLD_STYLE // temporary solution, it might be defined in Config_SDK.h
        if( MenuSettingConfig()->uiFCWS == COMMON_FCWS_EN_ON )
        #else
		if( uiAdasFlag & AHC_ADAS_ENABLED_FCWS )
        #endif
        {
            bFCWS_En = COMMON_FCWS_EN_ON;
            MMPS_Sensor_SetADASFeatureEn(MMPS_ADAS_FCWS, MMP_TRUE);
        }
        else
        {
            MMPS_Sensor_SetADASFeatureEn(MMPS_ADAS_FCWS, MMP_FALSE);
        }
	#endif
	
	#if (ENABLE_ADAS_SAG)	
        if( uiAdasFlag & AHC_ADAS_ENABLED_SAG )
        {
            bSAG_En = COMMON_SAG_EN_ON;
            MMPS_Sensor_SetADASFeatureEn(MMPS_ADAS_SAG, MMP_TRUE);
        }
        else
        {
            MMPS_Sensor_SetADASFeatureEn(MMPS_ADAS_SAG, MMP_FALSE);
        }
	#endif

		if( bLDWS_En == COMMON_LDWS_EN_ON || 
			bFCWS_En == COMMON_FCWS_EN_ON || 
			bSAG_En  == COMMON_SAG_EN_ON  )
		{
			byType = MMPS_3GPRECD_Y_FRAME_TYPE_ADAS;
		}
	}
#endif        

	AHC_SetYFrameType( byType ); //It has to be set before enter Preview mode
}


//------------------------------------------------------------------------------
//  Function    : AHC_SetMode
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set system mode

 Set system to assigned mode.
 Parameters:
 @param[in] uiMode AHC_MODE_IDLE, AHC_MODE_CAPTURE_PREVIEW...etc
 @retval TRUE or FALSE.
*/
AHC_BOOL AHC_SetMode(AHC_MODE_ID uiMode)
{
    MMP_ERR         err;
    MMP_BOOL        bAudioOn = 1;

    printc("========== %s,old:0x%X,new:0x%X ========== \n", __func__, AHC_GetAhcSysMode(), uiMode);
   	
   	if (ahc_semi == 0xff) {
   		ahc_semi = AHC_OS_CreateSem(1);
    }
    
    LOCK_AHC_MODE();
    
    if(AHC_GetAhcSysMode() != uiMode) {

        if(AHC_GetAhcSysMode() == AHC_MODE_IDLE) {

            if(uiMode == AHC_MODE_CAPTURE_PREVIEW) {
                AIHC_SetCapturePreviewMode(AHC_FALSE, AHC_FALSE);
                AHC_SetAhcSysMode(AHC_MODE_CAPTURE_PREVIEW);
            }
            else if(uiMode == AHC_MODE_USB_MASS_STORAGE) {
                if(AHC_SetUsbMode(AHC_USB_MODE_MASS_STORAGE) == AHC_FALSE){
                    printc(FG_RED("%s,%d error\r\n"), __func__,__LINE__);
                }
                else{
                    AHC_SetAhcSysMode(AHC_MODE_USB_MASS_STORAGE);
                }
            }
            else if(uiMode == AHC_MODE_USB_WEBCAM) {
                extern MMP_USHORT pcam_usb_CustomedPreviewAttr(MMP_BOOL bUserConfig, MMP_UBYTE ubPrevSnrMode);

                AHC_PresetSensorMode(AHC_SENSOR_MODE_DEFAULT_PCCAM_RESOLUTION);
                pcam_usb_CustomedPreviewAttr(MMP_TRUE, AHC_GetPresetSensorMode());

                if(AHC_SetUsbMode(AHC_USB_MODE_WEBCAM) == AHC_FAIL){
                    printc(FG_RED("%s,%d error\r\n"), __func__,__LINE__);
                }
                else{
                    AHC_SetAhcSysMode(AHC_MODE_USB_WEBCAM);
                }
            }
            else if(uiMode == AHC_MODE_PLAYBACK) {
                if ( AIHC_SetPlaybackMode() == AHC_TRUE ) {
                    AHC_SetAhcSysMode(AHC_MODE_PLAYBACK);
                }
                else {
		    		UNLOCK_AHC_MODE();
                    return AHC_FALSE;
                }
            }
            else if(uiMode == AHC_MODE_THUMB_BROWSER) {
                //TBD
			    MMPS_Display_SetWinActive(FRONT_CAM_WINDOW_ID, MMP_FALSE);

                AHC_SetThumbnailMode();

                AHC_SetAhcSysMode(AHC_MODE_THUMB_BROWSER);
            }
            else if (uiMode == AHC_MODE_RECORD_PREVIEW ) {

                if ( AIHC_SetVideoRecordMode(0, AHC_VIDRECD_INIT) == AHC_TRUE ) {
                    AHC_SetAhcSysMode(AHC_MODE_RECORD_PREVIEW);
                }
            }
            else if (uiMode == AHC_MODE_RAW_PREVIEW){
                AIHC_SetCapturePreviewMode(AHC_TRUE, AHC_FALSE);
                AHC_SetAhcSysMode(AHC_MODE_RAW_PREVIEW);
            }
			else if (uiMode == AHC_MODE_NET_PLAYBACK) {
				AHC_SetAhcSysMode(AHC_MODE_NET_PLAYBACK);
			}
            else {
                RTNA_DBG_Str(AHC_GENERAL_DBG_LEVEL, FG_RED("AHC_ERROR : Set Mode Error @AHC_MODE_IDLE\r\n"));
				UNLOCK_AHC_MODE();
                return AHC_FALSE;
            }
        }///< if(m_AhcSystemMode == AHC_MODE_IDLE)
        else if(AHC_GetAhcSysMode() == AHC_MODE_RECORD_PREVIEW) {

            if(uiMode == AHC_MODE_IDLE) {
                if (AHC_STREAM_H264 == (AHC_GetStreamingMode() & AHC_STREAM_V_MASK)) {
                	UNLOCK_AHC_MODE();
                    return AHC_FALSE;
                }
                else
               	if(AHC_GetStreamingMode() != AHC_STREAM_OFF){
               		UNLOCK_AHC_MODE();
					AHC_SetStreamingMode(AHC_STREAM_OFF);
               		LOCK_AHC_MODE();
				}
		        //turn off the preview, == AIHC_StopVideoRecordModeEx(AHC_FALSE, AHC_TRUE, AHC_TRUE);
                AIHC_StopVideoRecordMode(AHC_FALSE, AHC_TRUE);


                AHC_SetAhcSysMode(AHC_MODE_IDLE);
            }
            else if(uiMode == AHC_MODE_USB_WEBCAM) {
                if(AHC_SetUsbMode(AHC_USB_MODE_WEBCAM) == AHC_FAIL){
                    printc(FG_RED("%s,%d error\r\n"), __func__,__LINE__);
                }
                else{
                    AHC_SetAhcSysMode(AHC_MODE_USB_WEBCAM);
                }
            }
            else {//typically RECORD_PREVIEW to (VIDEO_RECORD || RECORD_VONLY)

                if (uiMode == AHC_MODE_VIDEO_RECORD) {
                    bAudioOn = 1; //audio encode enable
                }
                else if (uiMode == AHC_MODE_RECORD_VONLY) {
                    bAudioOn = 0; //audio encode disable
                }
                else {
                    RTNA_DBG_Str(AHC_GENERAL_DBG_LEVEL, FG_RED("AHC_ERROR : Set Mode Error @AHC_MODE_RECORD_PREVIEW\r\n"));
    				UNLOCK_AHC_MODE();
                    return AHC_FALSE;
                }

                #if WIFI_PORT == 1
                if (!AHC_IsRecorderControllable(uiMode)) {
    				UNLOCK_AHC_MODE();
                    return AHC_FALSE;
                }
                #endif

                if (AIHC_SetVideoRecordMode(bAudioOn, AHC_VIDRECD_START) == AHC_TRUE ) {
                    AHC_SetAhcSysMode(uiMode);
                }
                else {
    				UNLOCK_AHC_MODE();
                    return AHC_FALSE;
                }
            }
        }///< else if(m_AhcSystemMode == AHC_MODE_RECORD_PREVIEW)
        else if(AHC_GetAhcSysMode() == AHC_MODE_LONG_TIME_CONTINUOUS_FIRST_CAPTURE) {
            if(uiMode == AHC_MODE_LONG_TIME_CONTINUOUS_NEXT_CAPTURE) {
                if ( AIHC_SetCaptureMode(uiMode) == AHC_TRUE ) {
                    AHC_SetAhcSysMode(AHC_MODE_LONG_TIME_CONTINUOUS_FIRST_CAPTURE);
                }
            }
            else if(uiMode == AHC_MODE_LONG_TIME_CONTINUOUS_LAST_CAPTURE) {
                if ( AIHC_SetCaptureMode(AHC_MODE_LONG_TIME_CONTINUOUS_NEXT_CAPTURE) == AHC_TRUE ) {
                    AHC_SetAhcSysMode(AHC_MODE_LONG_TIME_CONTINUOUS_LAST_CAPTURE);
                    UNLOCK_AHC_MODE();
                    err = AHC_SetMode(AHC_MODE_CAPTURE_PREVIEW);
                    LOCK_AHC_MODE();
                    if(err == AHC_FALSE) {
                        UNLOCK_AHC_MODE();
                        return AHC_FALSE;
                    }
                }
            }
        }
        else if(AHC_GetAhcSysMode() == AHC_MODE_LONG_TIME_CONTINUOUS_LAST_CAPTURE) {
            if(uiMode == AHC_MODE_CAPTURE_PREVIEW) {
                AIHC_SetCapturePreviewMode(AHC_FALSE, AHC_TRUE);
                AHC_SetAhcSysMode(AHC_MODE_CAPTURE_PREVIEW);
            }
            else {
                RTNA_DBG_Str(AHC_GENERAL_DBG_LEVEL, FG_RED("AHC_ERROR : Set Mode Error @AHC_MODE_STILL_CAPTURE\r\n"));
				UNLOCK_AHC_MODE();
                return AHC_FALSE;
            }
        }
        else if(AHC_GetAhcSysMode() == AHC_MODE_CAPTURE_PREVIEW) {

            if(uiMode == AHC_MODE_IDLE) {
		    	if(AHC_GetStreamingMode() != AHC_STREAM_OFF){
               		UNLOCK_AHC_MODE();
					AHC_SetStreamingMode(AHC_STREAM_OFF);
               		LOCK_AHC_MODE();
				}
                AIHC_StopCapturePreviewMode();

                AHC_SetAhcSysMode(AHC_MODE_IDLE);
            }
            else if ( uiMode == AHC_MODE_C2C_CAPTURE ) {
                //TBD
                if ( AIHC_SetCaptureMode(uiMode) == AHC_TRUE ) {
                    AHC_SetAhcSysMode(uiMode);
                }
			    else {
			    	UNLOCK_AHC_MODE();
			        return AHC_FALSE;
			    }
            }
            else if ( (uiMode == AHC_MODE_DRAFT_CAPTURE     ) ||
                      (uiMode == AHC_MODE_STILL_CAPTURE     ) ||
                      (uiMode == AHC_MODE_SEQUENTIAL_CAPTURE) ) {

                if ( AIHC_SetCaptureMode(uiMode) == AHC_TRUE ) {
                    AHC_SetAhcSysMode(uiMode);
					#if defined(CCIR656_OUTPUT_ENABLE)&&(CCIR656_OUTPUT_ENABLE)
					printc("## TODO:MMPS_DSC_EnablePreviewDisplay,FOR DEBUG\r\n");
					MMPS_DSC_EnablePreviewDisplay(gsAhcCurrentSensor, MMP_FALSE, MMP_TRUE);
					#endif
					
                    UNLOCK_AHC_MODE();
                    err = AHC_SetMode(AHC_MODE_CAPTURE_PREVIEW);
                    LOCK_AHC_MODE();
                    if(err == AHC_FALSE) {
                        UNLOCK_AHC_MODE();
                        return AHC_FALSE;
                    }
                }
            }
            else if (uiMode == AHC_MODE_LONG_TIME_CONTINUOUS_FIRST_CAPTURE) {

                if ( AIHC_SetCaptureMode(uiMode) == AHC_TRUE ) {
                    AHC_SetAhcSysMode(uiMode);
                }
            }
			else if (uiMode == AHC_MODE_CONTINUOUS_CAPTURE) {
			    if ( AIHC_SetCaptureMode(uiMode) == AHC_TRUE ) {
			        AHC_SetAhcSysMode(uiMode);
			    }
			}
            else {
				UNLOCK_AHC_MODE();
                return AHC_FALSE;
            }
        }///< else if(m_AhcSystemMode == AHC_MODE_CAPTURE_PREVIEW)
        else if(AHC_GetAhcSysMode() == AHC_MODE_DRAFT_CAPTURE) {

        }
        else if (AHC_GetAhcSysMode() == AHC_MODE_STILL_CAPTURE) {

            if (uiMode == AHC_MODE_CAPTURE_PREVIEW) {

                AIHC_SetCapturePreviewMode(AHC_FALSE, AHC_FALSE);

                AHC_SetAhcSysMode(AHC_MODE_CAPTURE_PREVIEW);
            }
            else {
                RTNA_DBG_Str(AHC_GENERAL_DBG_LEVEL, FG_RED("AHC_ERROR : Set Mode Error @AHC_MODE_STILL_CAPTURE\r\n"));
				UNLOCK_AHC_MODE();
                return AHC_FALSE;
            }
        }
        else if(AHC_GetAhcSysMode() == AHC_MODE_C2C_CAPTURE) {

        }
        else if (AHC_GetAhcSysMode() == AHC_MODE_CONTINUOUS_CAPTURE) {               
			if (uiMode == AHC_MODE_CAPTURE_PREVIEW) {
			    AIHC_SetCapturePreviewMode(AHC_FALSE, AHC_TRUE);
        		AHC_SetAhcSysMode(uiMode);
			}
        }
        else if(AHC_GetAhcSysMode() == AHC_MODE_SEQUENTIAL_CAPTURE) {

            if (uiMode == AHC_MODE_CAPTURE_PREVIEW) {
                AIHC_SetCapturePreviewMode(AHC_FALSE, AHC_TRUE);

                AHC_SetAhcSysMode(AHC_MODE_CAPTURE_PREVIEW);
            }
            else {
                RTNA_DBG_Str(AHC_GENERAL_DBG_LEVEL, FG_RED("AHC_ERROR : Set Mode Error @AHC_MODE_SEQUENTIAL_CAPTURE\r\n"));
				UNLOCK_AHC_MODE();
                return AHC_FALSE;
            }
        }
        else if(AHC_GetAhcSysMode() == AHC_MODE_USB_MASS_STORAGE) {

            if(uiMode == AHC_MODE_IDLE) {
                AHC_DisconnectDevice();
                AHC_SetAhcSysMode(AHC_MODE_IDLE);
                AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect);
            }
            else {
                RTNA_DBG_Str(AHC_GENERAL_DBG_LEVEL, FG_RED("AHC_ERROR : Set Mode Error @AHC_MODE_USB_MASS_STORAGE\r\n"));
				UNLOCK_AHC_MODE();
                return AHC_FALSE;
            }
        }
        else if(AHC_GetAhcSysMode() == AHC_MODE_USB_WEBCAM) {
			if(uiMode == AHC_MODE_IDLE) {
				AHC_DisconnectDevice();
                AHC_SetAhcSysMode(AHC_MODE_IDLE);
                //AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect);
			}
			else {
				RTNA_DBG_Str(AHC_GENERAL_DBG_LEVEL, FG_RED("AHC_ERROR : Set Mode Error @AHC_MODE_USB_WEBCAM\r\n"));
				UNLOCK_AHC_MODE();
				return AHC_FALSE;
			}
        }
        else if(AHC_GetAhcSysMode() == AHC_MODE_PLAYBACK) {

            if(uiMode == AHC_MODE_IDLE) {

                AIHC_StopPlaybackMode();

                AHC_SetAhcSysMode(AHC_MODE_IDLE);
            }
            else {
                RTNA_DBG_Str(AHC_GENERAL_DBG_LEVEL, FG_RED("AHC_ERROR : Set Mode Error @AHC_MODE_PLAYBACK\r\n"));
		    	UNLOCK_AHC_MODE();
                return AHC_FALSE;
            }
        }
        else if(AHC_GetAhcSysMode() == AHC_MODE_THUMB_BROWSER) {

            if(uiMode == AHC_MODE_PLAYBACK) {

                if ( AIHC_SetPlaybackMode() == AHC_TRUE ) {
                    AHC_SetAhcSysMode(AHC_MODE_PLAYBACK);
                }
                else {
                    //turn off PIP window, to prevent customers redraw GUI without setting PIP properly
                    MMPS_Display_SetWinActive(FRONT_CAM_WINDOW_ID, MMP_FALSE);
                }
            }
            //swyang test
            else if(uiMode == AHC_MODE_IDLE) {
                #if 1
                AIHC_DrawReservedOSD(AHC_TRUE);
                AHC_OSDSetActive(THUMBNAIL_OSD_FRONT_ID, AHC_FALSE);
                AIHC_DrawReservedOSD(AHC_FALSE);
                #endif
                {
                    MMP_IBC_PIPEID sIBCPipe = MMP_IBC_PIPE_0;

                    for(sIBCPipe = MMP_IBC_PIPE_0; sIBCPipe < MMP_IBC_PIPE_MAX;sIBCPipe++){
                        MMPD_IBC_SetStoreEnable(sIBCPipe, MMP_FALSE);
                    }
                }
                AHC_SetAhcSysMode(AHC_MODE_IDLE);
            }
        }///< else if(m_AhcSystemMode == AHC_MODE_THUMB_BROWSER)
        else if ( (AHC_GetAhcSysMode() == AHC_MODE_VIDEO_RECORD) || (AHC_GetAhcSysMode() == AHC_MODE_RECORD_VONLY) ) {
            UINT8 bAudioOn;
            AHC_VIDRECD_CMD bStartRecord;


            if(uiMode == AHC_MODE_IDLE) {
                if (AHC_STREAM_H264 == AHC_GetStreamingMode()) {
                	UNLOCK_AHC_MODE();
                    return AHC_FALSE;
                }


                //AIHC_StopVideoRecordMode(AHC_TRUE,AHC_TRUE);
 			    #if WIFI_PORT == 1
 			    AIHC_StopVideoRecordModeEx(AHC_TRUE,AHC_FALSE, AHC_FALSE);
                #else
                AIHC_StopVideoRecordModeEx(AHC_TRUE,AHC_TRUE, AHC_TRUE);
                #endif
                AHC_SetAhcSysMode(AHC_MODE_IDLE);
            }
            else if (uiMode == AHC_MODE_RECORD_PREVIEW ) {
                #if WIFI_PORT == 1
                if (!AHC_IsRecorderControllable(uiMode)) {
                	UNLOCK_AHC_MODE();
                    return AHC_FALSE;
                }
                #endif

                //AIHC_StopVideoRecordMode(AHC_TRUE,AHC_FALSE);

                AIHC_StopVideoRecordModeEx(AHC_TRUE,AHC_FALSE, AHC_FALSE);

                #if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
                if (uiGetParkingModeEnable())
                #else
                if (MenuSettingConfig()->uiMOVPreRecord == MOVIE_PRE_RECORD_ON)
                #endif
                {
                    bAudioOn = 1;
					if( bRestartPreviewAsRecordingSetting == AHC_TRUE )
					{
						bStartRecord = AHC_VIDRECD_RESTART_PREVIEW;
				    }
					else
					{
	                    bStartRecord = AHC_VIDRECD_INIT;
			        }
                }
                else{
                    bAudioOn = 0;
                    bStartRecord = AHC_VIDRECD_STOP;
                }

                if ( AIHC_SetVideoRecordMode(bAudioOn, bStartRecord) == AHC_TRUE ) {
                     AHC_SetAhcSysMode(AHC_MODE_RECORD_PREVIEW);
                }
            }
            else {
                RTNA_DBG_Str(AHC_GENERAL_DBG_LEVEL, FG_RED("AHC_ERROR : Set Mode Error @AHC_MODE_VIDEO_RECORD\r\n"));
				UNLOCK_AHC_MODE();
                return AHC_FALSE;
            }
        }///< else if( (m_AhcSystemMode == AHC_MODE_VIDEO_RECORD) || (m_AhcSystemMode == AHC_MODE_RECORD_VONLY) )
        else if(AHC_GetAhcSysMode() == AHC_MODE_CALIBRATION) {

        }
        else if(AHC_GetAhcSysMode() == AHC_MODE_RAW_CAPTURE){

            if(uiMode == AHC_MODE_RAW_PREVIEW) {

                AIHC_SetCapturePreviewMode(AHC_TRUE, AHC_FALSE);

                AHC_SetAhcSysMode(AHC_MODE_RAW_PREVIEW);
            }
            else if(uiMode == AHC_MODE_IDLE){
                AHC_SetAhcSysMode(AHC_MODE_IDLE);
            }
            else {
                RTNA_DBG_Str(AHC_GENERAL_DBG_LEVEL, FG_RED("AHC_ERROR : Set Mode Error @AHC_MODE_RAW_PREVIEW\r\n"));
				UNLOCK_AHC_MODE();
                return AHC_FALSE;
            }
        }
        else if(AHC_GetAhcSysMode() == AHC_MODE_RAW_PREVIEW) {

            if(uiMode == AHC_MODE_RAW_CAPTURE) {
                AIHC_SetCaptureMode(uiMode);

                AHC_SetAhcSysMode(AHC_MODE_RAW_CAPTURE);
                UNLOCK_AHC_MODE();
                AHC_SetMode(AHC_MODE_RAW_PREVIEW);
                LOCK_AHC_MODE();
            }
            else if(uiMode == AHC_MODE_IDLE){
                AIHC_StopCapturePreviewMode();
                AHC_SetAhcSysMode(AHC_MODE_IDLE);
            }
            else {
                RTNA_DBG_Str(AHC_GENERAL_DBG_LEVEL, FG_RED("AHC_ERROR : Set Mode Error @AHC_MODE_RAW_CAPTURE\r\n"));
				UNLOCK_AHC_MODE();
                return AHC_FALSE;
            }
        }
		else if (AHC_GetAhcSysMode() == AHC_MODE_NET_PLAYBACK) {
			if (uiMode == AHC_MODE_IDLE) {
				#if WIFI_PORT == 1
				netapp_stopTranscoding();
				#endif
				AHC_SetAhcSysMode(AHC_MODE_IDLE);
			} else {
				RTNA_DBG_Str(AHC_GENERAL_DBG_LEVEL, FG_RED("AHC_ERROR : Set Mode Error @AHC_MODE_NET_PLAYBACK\r\n"));
				UNLOCK_AHC_MODE();
				return AHC_FALSE;
			}
		}
    }///< if(m_AhcSystemMode != uiMode)
    else {

        if(AHC_GetAhcSysMode() == AHC_MODE_PLAYBACK) {
            AIHC_StopPlaybackMode();

            if ( AIHC_SetPlaybackMode() == AHC_FALSE ) {
	        	UNLOCK_AHC_MODE();
                return AHC_FALSE;
            }
        }
        else if(AHC_GetAhcSysMode() == AHC_MODE_THUMB_BROWSER) {
            AHC_SetThumbnailMode();
        }
    }
	UNLOCK_AHC_MODE();
    return AHC_TRUE;
}

AHC_MODE_ID AHC_GetMode(void)
{
    return m_AhcSystemMode;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetSystemStatus
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get system status

 Get current system status from the information keeper.
 Parameters:
 @param[out] *pwValue Get status result.
 @retval TRUE or FALSE.
*/
AHC_BOOL AHC_GetSystemStatus(UINT32 *pwValue)
{
    MMP_ULONG fw_status = 0;

    *pwValue = (m_AhcSystemMode << 16) & 0xFFFF0000;

    switch(m_AhcSystemMode) {
        case AHC_MODE_IDLE:
            *pwValue |= (AHC_SYS_IDLE & 0xFFFF);
            break;
        case AHC_MODE_CAPTURE_PREVIEW:
            *pwValue |= (AHC_SYS_IDLE & 0xFFFF);
            break;
        case AHC_MODE_DRAFT_CAPTURE:
            *pwValue |= (AHC_SYS_IDLE & 0xFFFF);
            break;
        case AHC_MODE_STILL_CAPTURE:
            *pwValue |= (AHC_SYS_IDLE & 0xFFFF);
            break;
        case AHC_MODE_C2C_CAPTURE:
            *pwValue |= (AHC_SYS_IDLE & 0xFFFF);
            break;
        case AHC_MODE_SEQUENTIAL_CAPTURE:
            *pwValue |= (AHC_SYS_IDLE & 0xFFFF);
            break;
        case AHC_MODE_USB_MASS_STORAGE:
            fw_status = MMPS_USB_GetStatus();
            *pwValue |= (AHC_SYS_USB_STATUS + fw_status) & 0xFFFF;
            break;
        case AHC_MODE_USB_WEBCAM:
            *pwValue |= (AHC_SYS_IDLE & 0xFFFF);
            break;
        case AHC_MODE_PLAYBACK:
            if(m_ulCurrentPBFileType == DCF_OBG_JPG) {  //JPEG playback
                *pwValue |= (AHC_SYS_IDLE & 0xFFFF);
            }
            else if( (m_ulCurrentPBFileType == DCF_OBG_MP3 ) ||
                     (m_ulCurrentPBFileType == DCF_OBG_WAV ) ||
                     (m_ulCurrentPBFileType == DCF_OBG_OGG ) ||
                     (m_ulCurrentPBFileType == DCF_OBG_WMA ) )
            {
                MMPS_AUDIO_GetPlayStatus((MMPS_AUDIO_PLAY_STATUS*)&fw_status);
                *pwValue |= (AHC_SYS_AUDPLAY_STATUS + (fw_status>>8)) & 0xFFFF;
            }
            else
            {   //Video playback
                MMPS_VIDPLAY_GetState((MMP_M_STATE*)&fw_status);
                *pwValue |= (AHC_SYS_VIDPLAY_INVALID + fw_status) & 0xFFFF;
            }
            break;
        case AHC_MODE_THUMB_BROWSER:
            *pwValue |= (AHC_SYS_IDLE & 0xFFFF);
            break;
        case AHC_MODE_VIDEO_RECORD:
        case AHC_MODE_RECORD_VONLY:
            MMPS_3GPRECD_RecordStatus((MMPS_FW_OPERATION *)&fw_status);
            *pwValue |= (AHC_SYS_VIDRECD_STATUS + fw_status) & 0xFFFF;
            break;
        case AHC_MODE_RECORD_PREVIEW:
            MMPS_3GPRECD_GetPreviewPipeStatus(gsAhcCurrentSensor, (MMP_BOOL*)&fw_status);
            if(fw_status == MMP_TRUE) {
                *pwValue |= AHC_SYS_VIDRECD_PREVIEW_NORMAL & 0xFFFF;
            }
            else {
                *pwValue |= AHC_SYS_VIDRECD_PREVIEW_ABNORMAL & 0xFFFF;
            }
            break;
        case AHC_MODE_CALIBRATION:
            *pwValue |= (AHC_SYS_IDLE & 0xFFFF);
            break;
    }

    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetMemLocationPtr
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set memory location

 This function configures the predefined memory position for further transfer of data read/write.
 Parameters:
 @param[in] wLocation According to AHC_MEMORY_LOCATION.
 @param[in] *pbBuffer Point to the buffer to read/write.
 @param[in] dwSize Number of bytes of the data.
 @retval TRUE or FALSE.
*/
AHC_BOOL AHC_SetMemLocationPtr(AHC_MEMORY_LOCATION_ID wLocation, UINT8* pbBuffer, UINT32 dwSize)
{
    glAhcMemoryLocation[wLocation].ulLocationAddr = (MMP_ULONG)pbBuffer;
    glAhcMemoryLocation[wLocation].ulSize = dwSize;
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_UnSetMemLocationPtr
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Unset memory location

 Used to free memory allocation assigned through AHC_SetMemLocationPtr().
 Parameters:
 @param[in] wLocation Specific location.
 @retval TRUE or FALSE.
*/
AHC_BOOL AHC_UnSetMemLocationPtr(AHC_MEMORY_LOCATION_ID wLocation)
{
    glAhcMemoryLocation[wLocation].ulLocationAddr = 0;
    glAhcMemoryLocation[wLocation].ulSize = 0;
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_UpdateUSP
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_UpdateUSP(UINT8 *pData, UINT32 uwSize)
{
    MMP_ULONG   ulFileID, ulWriteCount;
    MMP_ERR     err = MMP_ERR_NONE;

    err = MMPC_FS_FileOpen(AHC_USP_FILE_LOCATION, "wb", (void **)&ulFileID);

    if(err != MMP_ERR_NONE) {
         RTNA_DBG_Str(AHC_GENERAL_DBG_LEVEL, "AHC_ERROR : UpdateUSP open file error !\r\n");
         return err;
    }

    MMPC_FS_FileSeek((void *)ulFileID, 0, MMPD_FS_SEEK_SET);

    MMPC_FS_FileWrite((void *)ulFileID, pData, uwSize, &ulWriteCount);

    MMPC_FS_FileClose((void *)ulFileID);

    if(ulWriteCount != uwSize) {
        return AHC_FALSE;
    }
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_UpdateFSP
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_UpdateFSP(UINT8 *pData, UINT32 uwSize)
{
    MMP_ULONG   ulFileID, ulWriteCount;
    MMP_ERR     err = MMP_ERR_NONE;

    err = MMPC_FS_FileOpen(AHC_FSP_FILE_LOCATION, "wb", (void **)&ulFileID);

    if(err != MMP_ERR_NONE) {
         RTNA_DBG_Str(AHC_GENERAL_DBG_LEVEL, "AHC_ERROR : UpdateFSP open file error !\r\n");
         return err;
    }

    MMPC_FS_FileSeek((void *)ulFileID, 0, MMPD_FS_SEEK_SET);

    MMPC_FS_FileWrite((void *)ulFileID, pData, uwSize, &ulWriteCount);

    MMPC_FS_FileClose((void *)ulFileID);

    if(ulWriteCount != uwSize) {
        return AHC_FALSE;
    }
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_RestoreUSP
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_RestoreUSP(UINT8 *pData, UINT32 *puwSize)
{
    MMP_ULONG   ulFileID, ulReadCount;
    MMP_ERR     err = MMP_ERR_NONE;

    err = MMPC_FS_FileOpen(AHC_USP_FILE_LOCATION, "rb", (void **)&ulFileID);

    if(err != MMP_ERR_NONE) {
         RTNA_DBG_Str(AHC_GENERAL_DBG_LEVEL, "AHC_ERROR : RestoreUSP open file error !\r\n");
         return err;
    }

    if (pData == NULL) {  //return the target file's size
        MMPC_FS_FileGetSize((void *)ulFileID, (MMP_ULONG64*)puwSize);
        return AHC_TRUE;
    }
    else {
        MMPC_FS_FileRead((void *)ulFileID, pData, *puwSize, &ulReadCount);
        pData[ulReadCount] = '\0';
        MMPC_FS_FileClose((void *)ulFileID);

        if(ulReadCount != *puwSize) {
            RTNA_DBG_PrintLong(0, ulReadCount);
        }
    }
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_RevertFSP
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_RestoreFSP(UINT8 *pData, UINT32 *puwSize)
{
    MMP_ULONG   ulFileID, ulReadCount;
    MMP_ULONG64 uwSize = 0;
    MMP_ERR     err = MMP_ERR_NONE;

    err = MMPC_FS_FileOpen(AHC_FSP_FILE_LOCATION, "rb", (void **)&ulFileID);

    if(err != MMP_ERR_NONE) {
         RTNA_DBG_Str(AHC_GENERAL_DBG_LEVEL, "AHC_ERROR : RestoreFSP open file error !\r\n");
         return err;
    }

    if (pData == NULL) {  //return the target file's size
        MMPC_FS_FileGetSize((void *)ulFileID, &uwSize);
        *puwSize = (UINT32)uwSize;
        return AHC_TRUE;
    }
    else {
        MMPC_FS_FileRead((void *)ulFileID, pData, *puwSize, &ulReadCount);
        pData[ulReadCount] = '\0';
        MMPC_FS_FileClose((void *)ulFileID);

        if(ulReadCount != *puwSize) {
            RTNA_DBG_PrintLong(0, ulReadCount);
        }
    }
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetTimer - TBD
//  Description : Note!!! Need Stop timer by pTimerHandle in CB when bTimerCMD is AHC_TIMER_CMD_START_ONESHOT
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetTimer(AHC_TIMER_HANDLE* pTimerHandle, UINT32 bTimerID, AHC_TIMER_CMD bTimerCMD, UINT16 wTime)
{
#if defined(SA_PL)
    if(bTimerCMD == AHC_TIMER_CMD_START) {
        *pTimerHandle = (AHC_TIMER_HANDLE)AHC_OS_StartTimer(wTime, AHC_OS_TMR_OPT_ONE_SHOT, (OS_TMR_CALLBACK)&AIHC_TimerIsrHandler, (void*)bTimerID);
    }
    else if(bTimerCMD == AHC_TIMER_CMD_START_ONESHOT) {
        *pTimerHandle = (AHC_TIMER_HANDLE)AHC_OS_StartTimer(wTime, AHC_OS_TMR_OPT_PERIODIC, (OS_TMR_CALLBACK)&AIHC_TimerIsrHandler, (void*)bTimerID);
    }
#endif

    if(((AHC_OS_TMRID)pTimerHandle != 0xFF) && ((AHC_OS_TMRID)pTimerHandle != 0xFE)) {
        return AHC_TRUE;
    }
    else {
        return AHC_FALSE;
    }
}

//------------------------------------------------------------------------------
//  Function    : AHC_KillTimer
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_KillTimer(AHC_TIMER_HANDLE bTimerHandle)
{
#if defined(SA_PL)
    AHC_OS_StopTimer((AHC_OS_TMRID)bTimerHandle, AHC_OS_TMR_OPT_NONE);
#endif
    return AHC_TRUE;
}

AHC_BOOL AHC_SendAHLMessageEnable(AHC_BOOL Enable)
{
    m_bSendAHLMessage = Enable;
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SendAHLMessage
//  Description : This function sends an AHL message.
//  Notes       : Message Queue can store (AHC_MSG_QUEUE_SIZE - 1) messages
//------------------------------------------------------------------------------
/**
 @brief This function sends an AHL message.
 Parameters:
 @param[in] ulMsgID : Message ID
 @param[in] ulParam1 : The first parameter. sent with the operation.
 @param[in] ulParam2 : The second parameter sent with the operation.
 @retval TRUE or FALSE. // TRUE: Success, FALSE: Fail
*/
AHC_BOOL AHC_SendAHLMessage(UINT32 ulMsgID, UINT32 ulParam1, UINT32 ulParam2)
{
#if defined(SA_PL)
    UINT8 ret;

    static AHC_BOOL bShowDBG = AHC_TRUE;

    #if 0
    UINT16 usCount;
    AHC_OS_QuerySem(m_AHLMessageSemID, &usCount);
    if(usCount != 1) {
        printc("m_AHLMessageSemID count=%d Error!!\r\n", usCount);
        return AHC_FALSE;
    }
    #endif

    if(m_bSendAHLMessage == AHC_FALSE) {
        return AHC_TRUE;
    }

    if(AHC_OS_AcquireSem(m_AHLMessageSemID, 0) != OS_NO_ERR) {
        printc("AHC_SendAHLMessage OSSemPost: Fail!! \r\n");
        return AHC_FALSE;
    }

    if((glAhcParameter.AhlMsgUnblock != glAhcParameter.AhlMsgBlock) && (ulMsgID == glAhcParameter.AhlMsgBlock)) {
        //Message ID blocked !
        AHC_OS_ReleaseSem(m_AHLMessageSemID);
        return AHC_TRUE;
    }

    #if (AHC_MESSAGE_QUEUE_OVF)
    if(m_MessageQueueIndex_W > m_MessageQueueIndex_R) {
		printc("MQ : 0x%X\r\n", m_MessageQueueIndex_W - m_MessageQueueIndex_R);
    }	
	else if(m_MessageQueueIndex_W < m_MessageQueueIndex_R) {
		printc("MQ : 0x%X\r\n", (AHC_MSG_QUEUE_SIZE - m_MessageQueueIndex_R) + m_MessageQueueIndex_W);
	}	
    else {
		printc("MQ : 0x00\r\n");
    }	
	#endif
	
    if(m_MessageQueueIndex_R == (m_MessageQueueIndex_W + AHC_MSG_QUEUE_VIP_SIZE + 1)%AHC_MSG_QUEUE_SIZE) {
        //Message Queue Full !
        if(bShowDBG) {
            #if (AHC_MESSAGE_QUEUE_OVF)
            UINT8 i;
            #endif

            bShowDBG = AHC_FALSE;
            printc(BG_YELLOW("XXX : Message Queue Full ...Fail!! XXX")"\r\n");

            #if (AHC_MESSAGE_QUEUE_OVF)
            printc("Dump Message Queue from Index_R to Index_W\r\n");
            for(i=m_MessageQueueIndex_R; i<AHC_MSG_QUEUE_SIZE; i++) {
                printc("%3d:: %4d     %4d     %4d\r\n", i, m_MessageQueue[i].ulMsgID, m_MessageQueue[i].ulParam1,m_MessageQueue[i].ulParam2);
            }
            printc("------------------------------\r\n");
            for(i=0; i<m_MessageQueueIndex_W; i++) {
                printc("%3d:: %4d     %4d     %4dX\r\n", i, m_MessageQueue[i].ulMsgID, m_MessageQueue[i].ulParam1,m_MessageQueue[i].ulParam2);
            }
            #endif
        }
        AHC_OS_ReleaseSem(m_AHLMessageSemID);
        return AHC_FALSE;
    }

    m_MessageQueue[m_MessageQueueIndex_W].ulMsgID = ulMsgID;
    m_MessageQueue[m_MessageQueueIndex_W].ulParam1 = ulParam1;
    m_MessageQueue[m_MessageQueueIndex_W].ulParam2 = ulParam2;

    ret = AHC_OS_PutMessage(AHC_MSG_QId, (void *)(&m_MessageQueue[m_MessageQueueIndex_W]));

    if(ret != 0) {
        printc("AHC_OS_PutMessage: ret=%d  Fail!!\r\n", ret);
        AHC_OS_ReleaseSem(m_AHLMessageSemID);
        return AHC_FALSE;
    }

    bShowDBG = AHC_TRUE;

    m_MessageQueueIndex_W = (m_MessageQueueIndex_W + 1)%AHC_MSG_QUEUE_SIZE;

    AHC_OS_ReleaseSem(m_AHLMessageSemID);
#endif
    return AHC_TRUE;
}

AHC_BOOL AHC_SendAHLMessage_VIP(UINT32 ulMsgID, UINT32 ulParam1, UINT32 ulParam2)
{
#if defined(SA_PL)
    UINT8 ret;

    static AHC_BOOL bShowDBG = AHC_TRUE;

    #if 0
    UINT16 usCount;
    AHC_OS_QuerySem(m_AHLMessageSemID, &usCount);
    if(usCount != 1) {
        printc("m_AHLMessageSemID count=%d Error!!\r\n", usCount);
        return AHC_FALSE;
    }
    #endif

    if(m_bSendAHLMessage == AHC_FALSE) {
        return AHC_TRUE;
    }

    if(AHC_OS_AcquireSem(m_AHLMessageSemID, 0) != OS_NO_ERR) {
        printc("AHC_SendAHLMessage OSSemPost: Fail!! \r\n");
        return AHC_FALSE;
    }

    if((glAhcParameter.AhlMsgUnblock != glAhcParameter.AhlMsgBlock) && (ulMsgID == glAhcParameter.AhlMsgBlock)) {
        //Message ID blocked !
        AHC_OS_ReleaseSem(m_AHLMessageSemID);
        return AHC_TRUE;
    }

    if(m_MessageQueueIndex_R == (m_MessageQueueIndex_W + 1)%AHC_MSG_QUEUE_SIZE) {
        //Message Queue Full !
        if(bShowDBG) {
            #if (AHC_MESSAGE_QUEUE_OVF)
            UINT8 i;
            #endif

            bShowDBG = AHC_FALSE;
            printc("XXX : Message Queue (VIP) Full ...Fail!! XXX \r\n");

            #if (AHC_MESSAGE_QUEUE_OVF)
            printc("Dump Message Queue from Index_R to Index_W\r\n");
            for(i=m_MessageQueueIndex_R; i<AHC_MSG_QUEUE_SIZE; i++) {
                printc("%3d:: %4d     %4d     %4d\r\n", i, m_MessageQueue[i].ulMsgID, m_MessageQueue[i].ulParam1,m_MessageQueue[i].ulParam2);
            }
            printc("------------------------------\r\n");
            for(i=0; i<m_MessageQueueIndex_W; i++) {
                printc("%3d:: %4d     %4d     %4dX\r\n", i, m_MessageQueue[i].ulMsgID, m_MessageQueue[i].ulParam1,m_MessageQueue[i].ulParam2);
            }
            #endif
        }
        AHC_OS_ReleaseSem(m_AHLMessageSemID);
        return AHC_FALSE;
    }

    m_MessageQueue[m_MessageQueueIndex_W].ulMsgID = ulMsgID;
    m_MessageQueue[m_MessageQueueIndex_W].ulParam1 = ulParam1;
    m_MessageQueue[m_MessageQueueIndex_W].ulParam2 = ulParam2;

    ret = AHC_OS_PutMessage(AHC_MSG_QId, (void *)(&m_MessageQueue[m_MessageQueueIndex_W]));

    if(ret != 0) {
        printc("AHC_OS_PutMessage: ret=%d  Fail!!\r\n", ret);
        AHC_OS_ReleaseSem(m_AHLMessageSemID);
        return AHC_FALSE;
    }

    bShowDBG = AHC_TRUE;

    m_MessageQueueIndex_W = (m_MessageQueueIndex_W + 1)%AHC_MSG_QUEUE_SIZE;

    AHC_OS_ReleaseSem(m_AHLMessageSemID);
#endif
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SendAHLMessage_HP
//  Description : This function sends an AHL message.
//  Notes       : Message Queue can store (AHC_MSG_QUEUE_SIZE - 1) messages
//------------------------------------------------------------------------------
/**
 @brief This function sends an AHL message.
 Parameters:
 @param[in] ulMsgID : Message ID
 @param[in] ulParam1 : The first parameter. sent with the operation.
 @param[in] ulParam2 : The second parameter sent with the operation.
 @retval TRUE or FALSE. // TRUE: Success, FALSE: Fail
*/
AHC_BOOL AHC_SendAHLMessage_HP(UINT32 ulMsgID, UINT32 ulParam1, UINT32 ulParam2)
{
#if defined(SA_PL)
    UINT8 ret;

    static AHC_BOOL bShowDBG = AHC_TRUE;


    if(AHC_OS_AcquireSem(m_AHLHPMessageSemID, 0) != OS_NO_ERR) {
        printc("AHC_SendAHLMessage OSSemPost: Fail!! \r\n");
        return AHC_FALSE;
    }


    if(m_HPMessageQueueIndex_R == (m_HPMessageQueueIndex_W + 1)%AHC_HP_MSG_QUEUE_SIZE) {
        //Message Queue Full !
        if(bShowDBG) {
            #if (AHC_MESSAGE_QUEUE_OVF)
            UINT8 i;
            #endif

            bShowDBG = AHC_FALSE;
            printc("XXX : HP Message Queue Full ...Fail!! XXX \r\n");

            #if (AHC_MESSAGE_QUEUE_OVF)
            printc("Dump Message Queue from Index_R to Index_W\r\n");
            for(i=m_HPMessageQueueIndex_R; i<AHC_HP_MSG_QUEUE_SIZE; i++) {
                printc("%3d:: %4d     %4d     %4d\r\n", i, m_HPMessageQueue[i].ulMsgID, m_HPMessageQueue[i].ulParam1,m_HPMessageQueue[i].ulParam2);
            }
            printc("------------------------------\r\n");
            for(i=0; i<m_HPMessageQueueIndex_W; i++) {
                printc("%3d:: %4d     %4d     %4dX\r\n", i, m_HPMessageQueue[i].ulMsgID, m_HPMessageQueue[i].ulParam1,m_HPMessageQueue[i].ulParam2);
            }
            #endif
        }
        AHC_OS_ReleaseSem(m_AHLHPMessageSemID);
        return AHC_FALSE;
    }

    m_HPMessageQueue[m_HPMessageQueueIndex_W].ulMsgID = ulMsgID;
    m_HPMessageQueue[m_HPMessageQueueIndex_W].ulParam1 = ulParam1;
    m_HPMessageQueue[m_HPMessageQueueIndex_W].ulParam2 = ulParam2;

    ret = AHC_OS_PutMessage(AHC_HP_MSG_QId, (void *)(&m_HPMessageQueue[m_HPMessageQueueIndex_W]));

    if(ret != 0) {
        printc("HP AHC_OS_PutMessage: ret=%d  Fail!!\r\n", ret);
        AHC_OS_ReleaseSem(m_AHLHPMessageSemID);
        return AHC_FALSE;
    }

    bShowDBG = AHC_TRUE;

    m_HPMessageQueueIndex_W = (m_HPMessageQueueIndex_W + 1)%AHC_HP_MSG_QUEUE_SIZE;

    AHC_OS_ReleaseSem(m_AHLHPMessageSemID);

    if(AHC_OS_AcquireSem(m_AHLHPMessageCountSemID, 0) != OS_NO_ERR) {
        printc("AcquireSem m_AHLHPMessageCountSemID: Fail!! \r\n");
        return AHC_FALSE;
    }
#endif
    return AHC_TRUE;
}
//------------------------------------------------------------------------------
//  Function    : AHC_GetAHLMessage
//  Description :
//------------------------------------------------------------------------------
/**
 @brief
 Parameters:
 @param[in] *ulMsgID :
 @param[in] *ulParam1 :
 @param[in] *ulParam2 :
 @retval TRUE or FALSE. // TRUE: Success, FALSE: Fail
*/
AHC_BOOL AHC_GetAHLMessage(UINT32* ulMsgID, UINT32* ulParam1, UINT32* ulParam2)
{
    void *pMessageAddr = NULL;
    AHC_QUEUE_MESSAGE *ptr = NULL;

    #if 0
    UINT16 usCount;
    #endif

#if defined(SA_PL)

    if (AHC_OS_GetMessage(AHC_MSG_QId, (void*)(&pMessageAddr), AHC_AHL_MSGQ_TIMEOUT) != 0 )
    {
        if (AHC_AHL_MSGQ_TIMEOUT == 0)
            RTNA_DBG_Str(AHC_GENERAL_DBG_LEVEL,"AHC_ERROR : GetAHLMessage OS_ERR...\r\n");

        return AHC_FALSE;
    }
    else {

        ptr = (AHC_QUEUE_MESSAGE *)(pMessageAddr);
        *ulMsgID = ptr->ulMsgID;
        *ulParam1 = ptr->ulParam1;
        *ulParam2 = ptr->ulParam2;

        #if 0
        AHC_OS_QuerySem(m_AHLMessageSemID, &usCount);
        if(usCount != 1) {
            printc("m_AHLMessageSemID count=%d Error2!!\r\n", usCount);
            return FALSE;
        }
        #endif

        if(AHC_OS_AcquireSem(m_AHLMessageSemID, 0) != OS_NO_ERR) {
            printc("AHC_GetAHLMessage OSSemPost: Fail!! \r\n");
            return AHC_FALSE;
        }

        m_MessageQueueIndex_R = (m_MessageQueueIndex_R + 1)%AHC_MSG_QUEUE_SIZE;
        AHC_OS_ReleaseSem(m_AHLMessageSemID);
    }
#endif
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetAHLMessage_HP
//  Description :
//------------------------------------------------------------------------------
/**
 @brief
 Parameters:
 @param[in] *ulMsgID :
 @param[in] *ulParam1 :
 @param[in] *ulParam2 :
 @retval TRUE or FALSE. // TRUE: Success, FALSE: Fail
*/
AHC_BOOL AHC_GetAHLMessage_HP(UINT32* ulMsgID, UINT32* ulParam1, UINT32* ulParam2)
{
    void *pMessageAddr = NULL;
    AHC_QUEUE_MESSAGE *ptr = NULL;

    #if 0
    UINT16 usCount;
    #endif

#if defined(SA_PL)

    if (AHC_OS_GetMessage(AHC_HP_MSG_QId, (void*)(&pMessageAddr), 0x1) != 0 )
    {
        *ulMsgID = NULL;
        *ulParam1 = NULL;
        *ulParam2 = NULL;
        return AHC_FALSE;
    }
    else {

        ptr = (AHC_QUEUE_MESSAGE *)(pMessageAddr);
        *ulMsgID = ptr->ulMsgID;
        *ulParam1 = ptr->ulParam1;
        *ulParam2 = ptr->ulParam2;


        if(AHC_OS_AcquireSem(m_AHLHPMessageSemID, 0) != OS_NO_ERR) {
            printc("AHC_GetAHLMessage OSSemPost: Fail!! \r\n");
            return AHC_FALSE;
        }
        m_HPMessageQueueIndex_R = (m_HPMessageQueueIndex_R + 1)%AHC_HP_MSG_QUEUE_SIZE;
        AHC_OS_ReleaseSem(m_AHLHPMessageSemID);

        AHC_OS_ReleaseSem(m_AHLHPMessageCountSemID);
    }
#endif
    return AHC_TRUE;
}

void AHC_GetAHLHPMessageCount(UINT16 *usCount)
{
    AHC_OS_QuerySem(m_AHLHPMessageCountSemID, usCount);
}

//------------------------------------------------------------------------------
//  Function    : AHC_InitAHLMessage
//  Description :
//------------------------------------------------------------------------------
/**
 @brief
 @retval TRUE or FALSE. // TRUE: Success, FALSE: Fail
*/

AHC_BOOL AIHC_InitAHLMessage(void)
{
#if defined(SA_PL)
    m_AHLMessageSemID = AHC_OS_CreateSem(1);
    AHC_MSG_QId = AHC_OS_CreateMQueue(AHC_MsgQueue, AHC_MSG_QUEUE_SIZE);
#endif
    return AHC_TRUE;
}


//------------------------------------------------------------------------------
//  Function    : AHC_InitAHLMessage_HP
//  Description :
//------------------------------------------------------------------------------
/**
 @brief
 @retval TRUE or FALSE. // TRUE: Success, FALSE: Fail
*/

AHC_BOOL AIHC_InitAHLMessage_HP(void)
{
#if defined(SA_PL)
    m_AHLHPMessageCountSemID = AHC_OS_CreateSem(AHC_HP_MSG_QUEUE_SIZE);
    m_AHLHPMessageSemID = AHC_OS_CreateSem(1);
    AHC_HP_MSG_QId = AHC_OS_CreateMQueue(AHC_HPMsgQueue, AHC_HP_MSG_QUEUE_SIZE);
#endif
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_WaitForBootComplete
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_WaitForBootComplete(void)
{
    extern MMP_BOOL    gbSysBootComplete;
    
    while ((glAhcBootComplete != MMP_TRUE) && (gbSysBootComplete != MMP_TRUE)) {
        AHC_OS_Sleep(100);
    }

    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_OsCounterGet
//  Description :
//------------------------------------------------------------------------------
ULONG AHC_OsCounterGet(void)
{
    return MMPF_OsCounterGet();
}

//------------------------------------------------------------------------------
//  Function    : AHC_OsCounterGetFreqHz
//  Description :
//------------------------------------------------------------------------------
ULONG AHC_OsCounterGetFreqHz(void)
{
    return MMPF_OsCounterGetFreqHz();
}

//------------------------------------------------------------------------------
//  Function    : AHC_OsCounterGetFreqKhz
//  Description :
//------------------------------------------------------------------------------
ULONG AHC_OsCounterGetFreqKhz(void)
{
    return MMPF_OsCounterGetFreqKhz();
}

#if 0
void ____GPIO_Function_________(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    :
//  Description :
//  @brief This function configures the pull high or low of selected GPIO.
//  @param[in] ahc_piopin : PIO definition, please refer AHC_PIO_REG
//  @param[in] config :
/*
        #define PAD_NORMAL_TRIG             0x00
        #define PAD_SCHMITT_TRIG            0x01
        #define PAD_PULL_DOWN               0x02
        #define PAD_PULL_UP                 0x04
        #define PAD_FAST_SLEW               0x08
        #define PAD_SLOW_SLEW               0x00
        #define PAD_IDDQ_TEST_EN            0x10
        #define PAD_OUT_DRIVING(_a)         (((_a)&0x03)<<5)
*/
//------------------------------------------------------------------------------
AHC_BOOL AHC_ConfigGPIOPad(AHC_PIO_REG piopin, MMP_UBYTE config)
{
	if (MMPS_PIO_PadConfig(piopin, config) == MMP_ERR_NONE) {
        return AHC_TRUE;
	}

    return AHC_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_ConfigGPIO
//  Description :
//------------------------------------------------------------------------------
/**
 @brief This function configures the direction of selected GPIO.

 This function configures the direction of selected GPIO.
 Parameters:
 @param[in] ahc_piopin : PIO definition, please refer AHC_PIO_REG
 @param[in] bDirection : Configures the pin is input or output.
 @retval TRUE or FALSE.
*/

AHC_BOOL AHC_ConfigGPIO(AHC_PIO_REG ahc_piopin, UINT8 bDirection)
{
    MMP_ERR status;

    if (ahc_piopin == AHC_PIO_REG_UNKNOWN) {
        return AHC_FALSE;
    }

	if (bDirection == AHC_TRUE) {
		status = MMPS_PIO_EnableOutputMode((MMP_GPIO_PIN)ahc_piopin, MMP_TRUE);
    }
    else {
		status = MMPS_PIO_EnableOutputMode((MMP_GPIO_PIN)ahc_piopin, MMP_FALSE);
    }

    if (status != MMP_ERR_NONE) {
        return AHC_FALSE;
    }
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_ConfigGpioFunc
//  Description :
//------------------------------------------------------------------------------
/**
 @brief This API set a callback to be the handler of I/O actions.

 This API set a callback to be the handler of I/O actions.
 Parameters:
 @param[in] ahc_piopin : PIO definition, please refer AHC_PIO_REG
 @param[in] bPolarity : define when the function should be called.
 @param[in] uiCount : Configures the pin is input or output.
 @param[in] phHandleFunc : Handler function.
 @retval TRUE or FALSE.
*/
AHC_BOOL AHC_ConfigGpioFunc(AHC_PIO_REG ahc_piopin, AHC_PIO_TRIGMODE bPolarity, UINT32 uiCount)
{
    MMP_ERR status;
	MMP_GPIO_TRIG  trigmode = bPolarity;

    if (ahc_piopin == AHC_PIO_REG_UNKNOWN) {
        return AHC_FALSE;
    }

	status = MMPS_PIO_EnableTrigMode((MMP_GPIO_PIN)ahc_piopin, trigmode, MMP_TRUE);

    if (status != MMP_ERR_NONE) {
        return AHC_FALSE;
    }
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetGPIOFunc
//  Description :
//------------------------------------------------------------------------------
/**
 @brief This function enable or disable the handler in AHC_ConfigGpioFunc.

 This function enable or disable the handler in AHC_ConfigGpioFunc.
 Parameters:
 @param[in] ahc_piopin : PIO definition, please refer AHC_PIO_REG
 @param[in] bEnable : Configures the status of GPIO handler.
 @retval TRUE or FALSE.
*/
AHC_BOOL AHC_SetGPIOFunc(AHC_PIO_REG ahc_piopin, void* phHandleFunc, UINT8 bEnable)
{
    MMP_ERR status;

    if (ahc_piopin == AHC_PIO_REG_UNKNOWN) {
        return AHC_FALSE;
    }

	status = MMPS_PIO_EnableInterrupt((MMP_GPIO_PIN)ahc_piopin, bEnable, 0, (GpioCallBackFunc *)phHandleFunc);

    if (status != MMP_ERR_NONE) {
        return AHC_FALSE;
    }
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetGPIO
//  Description :
//------------------------------------------------------------------------------
/**
 @brief This function sets the state of a selected I/O pin.

 This function sets the state of a selected I/O pin.
 Parameters:
 @param[in] ahc_piopin : PIO definition, please refer AHC_PIO_REG
 @param[in] bState : Configures the output state of selected pin.
 @retval TRUE or FALSE.
*/
AHC_BOOL AHC_SetGPIO(AHC_PIO_REG ahc_piopin, UINT8 bState)
{
    MMP_ERR status;

    if (ahc_piopin == AHC_PIO_REG_UNKNOWN) {
        return AHC_FALSE;
	}

	status = MMPS_PIO_SetData((MMP_GPIO_PIN)ahc_piopin, bState);

    if (status != MMP_ERR_NONE) {
        return AHC_FALSE;
    }
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetGPIO
//  Description :
//------------------------------------------------------------------------------
/**
 @brief This function gets the state of a selected I/O pin.

 This function gets the state of a selected I/O pin.
 Parameters:
 @param[in] ahc_piopin : PIO definition, please refer AHC_PIO_REG
 @param[out] pwState : Get the input state of selected pin.
 @retval TRUE or FALSE.
*/
AHC_BOOL AHC_GetGPIO(AHC_PIO_REG ahc_piopin, UINT8 *pwState)
{
    MMP_ERR status;

    if (ahc_piopin == AHC_PIO_REG_UNKNOWN) {
        return AHC_FALSE;
    }

	status = MMPS_PIO_GetData((MMP_GPIO_PIN)ahc_piopin, pwState);

    if (status != MMP_ERR_NONE) {
        return AHC_FALSE;
    }
    return AHC_TRUE;
}

#if 0
void _____PWM_Function_________(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : AHC_PWM_Initialize
//  Description :
//------------------------------------------------------------------------------
/** @brief Driver init

Driver init
@retval TRUE or FALSE.
*/
AHC_BOOL AHC_PWM_Initialize(void)
{
    MMP_ERR status;

    status = MMPS_PWM_Initialize();

    if(status != MMP_ERR_NONE) {
        return AHC_FALSE;
    }
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_PWM_OutputPulse
//  Description :
//------------------------------------------------------------------------------
/** @brief Simplely output some pulses (According to the parameters)

Simplely output some pulses (According to the parameters)
@param[in] pwm_pin : PWM I/O pin selection, please refer AHC_PWMPIN
@param[in] bEnableIoPin : enable/disable the specific PWM pin
@param[in] ulFrquency : the pulse frequency. Note in P_V2 we only support max speed 6MHz. Note: Only support factor of 6 MHz (ex: 3MHz, 1.5MHz...)
@param[in] bHigh2Low : MMP_TRUE: High to Low pulse, MMP_FALSE: Low to High pulse
@param[in] bEnableInterrupt : enable interrupt or not
@param[in] pwm_callBack : call back function when interrupt occurs
@param[in] ulNumOfPulse : number of pulse, 0 stand for using PWM auto mode to generate infinite pulse.
@return It reports the status of the operation.
*/
AHC_BOOL AHC_PWM_OutputPulse(AHC_PWMPIN pwm_pin, AHC_BOOL bEnableIoPin, UINT32 ulFrquency, AHC_BOOL bHigh2Low, AHC_BOOL bEnableInterrupt, void* pwm_callBack, UINT32 ulNumOfPulse)
{
    MMP_ERR status;

	status = MMPS_PWM_OutputPulse(pwm_pin, bEnableIoPin, ulFrquency, 50, bHigh2Low, bEnableInterrupt, (PwmCallBackFunc*)pwm_callBack, ulNumOfPulse);

    if(status != MMP_ERR_NONE) {
        return AHC_FALSE;
    }
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_PWM_OutputPulseEx
//  Description :
//------------------------------------------------------------------------------
/** @brief Simplely output some pulses (According to the parameters)

Simplely output some pulses (According to the parameters)
@param[in] pwm_pin : PWM I/O pin selection, please refer AHC_PWMPIN
@param[in] bEnableIoPin : enable/disable the specific PWM pin
@param[in] ulFrquency : the pulse frequency. Note in P_V2 we only support max speed 6MHz. Note: Only support factor of 6 MHz (ex: 3MHz, 1.5MHz...)
@param[in] ulDuty : percentage of signal = high in 1 PWM cycle
@param[in] bHigh2Low : MMP_TRUE: High to Low pulse, MMP_FALSE: Low to High pulse
@param[in] bEnableInterrupt : enable interrupt or not
@param[in] pwm_callBack : call back function when interrupt occurs
@param[in] ulNumOfPulse : number of pulse, 0 stand for using PWM auto mode to generate infinite pulse.
@return It reports the status of the operation.
*/
AHC_BOOL AHC_PWM_OutputPulseEx(AHC_PWMPIN pwm_pin, AHC_BOOL bEnableIoPin, UINT32 ulFrquency, UINT32 ulDuty, AHC_BOOL bHigh2Low, AHC_BOOL bEnableInterrupt, void* pwm_callBack, UINT32 ulNumOfPulse)
{
    MMP_ERR status;

	status = MMPS_PWM_OutputPulse(pwm_pin, bEnableIoPin, ulFrquency, ulDuty, bHigh2Low, bEnableInterrupt, (PwmCallBackFunc*)pwm_callBack, ulNumOfPulse);

    if(status != MMP_ERR_NONE) {
        return AHC_FALSE;
    }
    return AHC_TRUE;
}

#if 0
void ____SDMMC_Function_____(){ruturn;} //dummy
#endif

AHC_BOOL AHC_SDMMC_BasicCheck(void)
{
    #if(defined(DEVICE_GPIO_2NDSD_PLUG))

    #if(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_MASTER_SLAVE)
    if(!AHC_IsSDInserted() && MenuSettingConfig()->uiMediaSelect == MEDIA_SETTING_SD_CARD)
    #elif(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_COMPLEMENTARY)
    if( (!AHC_IsSDInserted() && !AHC_Is2ndSDInserted()) && (MenuSettingConfig()->uiMediaSelect == MEDIA_SETTING_SD_CARD || MenuSettingConfig()->uiMediaSelect == MEDIA_SETTING_SD_CARD1 ))
    #endif//TWOSD_WORK_MODEL

    #else

    if(!AHC_IsSDInserted() && MenuSettingConfig()->uiMediaSelect == MEDIA_SETTING_SD_CARD)

    #endif
    {
        AHC_WMSG_Draw(AHC_TRUE, WMSG_NO_CARD, 3);
        return AHC_FALSE;
    }

    if(AHC_IsSDWriteProtect() && MenuSettingConfig()->uiMediaSelect == MEDIA_SETTING_SD_CARD)
    {
        AHC_WMSG_Draw(AHC_TRUE, WMSG_CARD_LOCK, 3);
        return AHC_FALSE;
    }
    return AHC_TRUE;
}

#if 0
void ____Power_Function_____(){ruturn;} //dummy
#endif

AHC_BOOL AHC_IsPowerLow(void)
{
#if 1
    return AHC_FALSE;
#else
    MMP_USHORT pX,pY,dir;
    TouchPanelReadVBAT1(&pX,&pY,dir);

    if(pX < 3800) {//3.8 mV
        return AHC_TRUE;
    }
    else {
        return AHC_FALSE;
    }
#endif
}
//------------------------------------------------------------------------------
//  Function    : AHC_PowerOff_Preprocess
//  Description : Store FW/User settings before power off
//------------------------------------------------------------------------------
void AHC_PowerOff_Preprocess(void)
{
#if (POWER_OFF_PREPROCESS_EN)
    #if (ENABLE_DETECT_FLOW)

    #ifdef CAR_DV
    Menu_WriteSetting();
    #else
    AHC_PARAM_Menu_Write(AHC_DEFAULT_USER_FAT);
    #endif

    AHC_OS_SleepMs(10);

    #endif

    #if (C005_MANUALLY_ADD_SPEEDCAM_POI && EDOG_ENABLE)
    if(EDOGCtrl_IsInitial()){
     EDOGCtrl_RestoesPOItoFile_Manual();
     AHC_OS_Sleep(10);
    }
    #endif

    AHC_SetMode(AHC_MODE_IDLE);
    AHC_OS_SleepMs(10);
    AHC_WaitVideoWriteFileFinish();

    AHC_UnloadSystemFile();
    AHC_UnmountVolume(AHC_MEDIA_MMC);
    AHC_OS_SleepMs(10);
#endif
}

//------------------------------------------------------------------------------
//  Function    : AHC_VirtualPowerOffPath
//  Description :
//------------------------------------------------------------------------------
void AHC_VirtualPowerOffPath(void)
{
#if (SUPPORT_VIRTUAL_POWER_OFF)
    UINT32   CurSysMode;
    AHC_GetSystemStatus(&CurSysMode);
    CurSysMode >>= 16;
    #if (SUPPORT_IR_CONVERTER)
    AHC_IR_WriteRegister(IR_CONVERTER_CLEAR_POWERON, 0x01);
    #endif

    if(uiGetCurrentState() == UI_VIRTUAL_POWER_SAVE_STATE) {
        printc("AHC_VirtualPowerOffPath VPS -> Power On\r\n");

    }
    else {
        if(AHC_IsUsbConnect()) {
            printc("AHC_VirtualPowerOffPath -> VPS\r\n");
            #if (SUPPORT_HDMI)
            if(HDMIFunc_GetStatus() != AHC_HDMI_NONE_STATUS) {
                if(HDMIFunc_GetStatus() == AHC_HDMI_MENU_STATUS) {
                    HDMIFunc_SetStatus(AHC_HDMI_MENU_EXIT_STATUS);
                    StateSwitchMode(UI_HDMI_STATE);
                    HDMIFunc_Enable(AHC_FALSE);
                    HDMIFunc_SetStatus(AHC_HDMI_NONE_STATUS);
                }
                else
                {
                    HDMIFunc_Enable(AHC_FALSE);
                    HDMIFunc_SetStatus(AHC_HDMI_NONE_STATUS);
                }
            }
            #endif
            #if 0
            if(TVFunc_Status() != AHC_TV_NONE_STATUS) {
                TVFunc_Enable(AHC_FALSE);
            }
            #endif

            uiPowerOffPicture();
            StateSwitchMode(UI_VIRTUAL_POWER_SAVE_STATE);
            return;
        }
    }

    #if(SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
        #if(SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_1)
        if(MenuSettingConfig()->uiMotionDtcSensitivity != MOTION_DTC_SENSITIVITY_OFF)
        {
            if(uiGetParkingModeMenuState() || !AHC_SDMMC_GetState() || AHC_IsHdmiConnect() || AHC_IsTVConnect() || (CurSysMode != AHC_MODE_RECORD_PREVIEW))
            goto L_PowerOff;
            else
            {
                uiSetParkingModeMenuState(AHC_TRUE, AHC_FALSE);
                StateSwitchMode(UI_PARKING_MENU_STATE);
            }

            return;
        }else
        {
            goto L_PowerOff;
        }
        #elif(SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_2)
            if((MenuSettingConfig()->uiParkingMode_Detection == PARKINGMODE_DETECTION_OFF) || !AHC_IsSDInserted() || !AHC_GetMountStorageMediaStatus() || !AHC_SDMMC_GetMountState() || MenuSettingConfig()->uiStorageAllocation == STORAGE_ALLOCATION_TYPE1)
                #if(defined(DEVICE_GPIO_2NDSD_PLUG))
                #if((TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_COMPLEMENTARY))
                || !AHC_Is2ndSDInserted()
                #endif//TWOSD_WORK_MODEL
                #endif//DEVICE_GPIO_2NDSD_PLUG
              )
            goto L_PowerOff;
            else
            {
                uiSetParkingModeMenuState(AHC_TRUE, AHC_FALSE);
                StateSwitchMode(UI_PARKING_MENU_STATE);
            }
        #else
            goto L_PowerOff;
        #endif
    #else
    goto L_PowerOff;
    #endif
L_PowerOff: AHC_PowerOff_Preprocess();

        if(uiGetCurrentState() == UI_VIRTUAL_POWER_SAVE_STATE) {

        }
        else {
            #if(ENABLE_POWER_OFF_PICTURE)
            uiPowerOffPicture();
            #endif
        }

        LedCtrl_LcdBackLight(AHC_FALSE);
        AHC_PMU_PowerOff();
#endif
}

//------------------------------------------------------------------------------
//  Function    : AHC_NormalPowerOffPath
//  Description :
//------------------------------------------------------------------------------
void AHC_NormalPowerOffPath(void)
{
    UINT32   CurSysMode;
    AHC_GetSystemStatus(&CurSysMode);
    CurSysMode >>= 16;

    #if(defined(DEVICE_GPIO_2NDSD_PLUG))
    #if(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_MASTER_SLAVE)
    MenuSettingConfig()->uiMediaSelect = MEDIA_SETTING_SD_CARD;
    #endif//TWOSD_WORK_MODEL_MASTER_SLAVE
    #endif//DEVICE_GPIO_2NDSD_PLUG

#if (SUPPORT_IR_CONVERTER)
    //AHC_IR_WriteRegister(IR_CONVERTER_CLEAR_POWERON, 0x01);//TBD
#endif

#if ((SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_1) || (SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_2))
    if(MenuSettingConfig()->uiMotionDtcSensitivity != MOTION_DTC_SENSITIVITY_OFF)
    {
        //if(uiGetParkingModeMenuState() || uiGetParkingModeEnable() || !AHC_SDMMC_GetState() || AHC_IsHdmiConnect() || !AHC_IsUsbConnect() || AHC_IsTVConnect()|| (CurSysMode != AHC_MODE_RECORD_PREVIEW)) {
        #if(SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_1)
        if( !AHC_IsUsbConnect() || uiGetParkingModeEnable() || !AHC_SDMMC_GetState() || AHC_IsHdmiConnect() ||uiGetParkingModeMenuState() || AHC_IsTVConnect()|| (CurSysMode != AHC_MODE_RECORD_PREVIEW))
        #elif (SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_2)
        if( !AHC_IsUsbConnect() || !AHC_SDMMC_GetState() || AHC_IsHdmiConnect()|| AHC_IsTVConnect()|| (CurSysMode != AHC_MODE_RECORD_PREVIEW))
        #endif
        {
            goto L_PowerOff;
        }
        else {
            uiSetParkingModeMenuState(AHC_TRUE, AHC_FALSE);
        }

        StateSwitchMode(UI_PARKING_MENU_STATE);
        return;
    }else
    {
        goto L_PowerOff;
    }
L_PowerOff:
#endif

    blockRealIDUIKeyTask = AHC_TRUE;

    AHC_PowerOff_Preprocess();

	#ifndef CFG_POWEROFF_WITHOUT_CLEAR_WMSG
    AHC_WMSG_Draw(AHC_FALSE, WMSG_NONE, 0); //Clear WMSG
	#endif

    #if(ENABLE_POWER_OFF_PICTURE)
    uiPowerOffPicture();
    #endif

    LedCtrl_LcdBackLight(AHC_FALSE);

    AHC_PMU_PowerOff();
}

void AHC_NormalPowerOffPathEx(AHC_BOOL bForce, AHC_BOOL bByebye, AHC_BOOL bSound)
{
    extern void AHC_PMU_PowerOffEx(AHC_BOOL bSound);

    if (bForce == AHC_TRUE)
    {
        AHC_PowerOff_Preprocess();

        if (bByebye == AHC_TRUE)
            uiPowerOffPicture();

        AHC_PMU_PowerOffEx(bSound);
    }
    else {
        #if (POWER_OFF_CONFIRM_PAGE_EN)
        PowerOff_InProcess  = AHC_TRUE;
        PowerOff_Option     = CONFIRM_NOT_YET;

        if(PowerOff_Option == CONFIRM_NOT_YET)
            MenuDrawSubPage_PowerOff(NULL);
        #endif
    }
}

#if (POWER_OFF_CONFIRM_PAGE_EN)

AHC_BOOL AHC_PowerOff_ResetVarible(void)
{
    PowerOff_InProcess  = AHC_FALSE;
    PowerOff_Option     = CONFIRM_NOT_YET;
    return AHC_TRUE;
}

AHC_BOOL AHC_PowerOff_Confirmed(void)
{
    /*
     * Turn off backlight, to avoid user see preview before goodbye screen.
     */
    LedCtrl_LcdBackLight(AHC_FALSE);

    AHC_PowerOff_ResetVarible();

    AHC_OSDSetColor(0 /*OSD_COLOR_TRANSPARENT*/);
    AHC_OSDDrawFillRect(0 /*MAIN_DISPLAY_BUFFER*/, 0, 0 , 320, 240);

#if (TVOUT_PREVIEW_EN || HDMI_PREVIEW_EN)
    if( TVFunc_Status()==AHC_TV_VIDEO_PREVIEW_STATUS ||
        TVFunc_Status()==AHC_TV_DSC_PREVIEW_STATUS)
    {
        AHC_OSDSetActive(0, 0);
    }
    #if (SUPPORT_HDMI)
    else if(HDMIFunc_GetStatus()==AHC_HDMI_VIDEO_PREVIEW_STATUS ||
            HDMIFunc_GetStatus()==AHC_HDMI_DSC_PREVIEW_STATUS)
    {
        AHC_OSDSetActive(0, 0);
    }
    #endif
#endif

    AHC_PowerOff_Preprocess();
    uiPowerOffPicture();
    AHC_PMU_PowerOff();
    return AHC_TRUE;
}

AHC_BOOL AHC_PowerOff_Cancel(void)
{
    AHC_PowerOff_ResetVarible();

    if(uiGetCurrentState()==UI_TVOUT_STATE)
    {
        #if (TVOUT_ENABLE)
        MenuDrawSubPage_PowerOffCancel_TV();
        #endif
        return AHC_TRUE;
    }
    else if(uiGetCurrentState()==UI_HDMI_STATE)
    {
        #if (HDMI_ENABLE)
        MenuDrawSubPage_PowerOffCancel_HDMI();
        #endif
        return AHC_TRUE;
    }
    else if(uiGetCurrentState()==UI_PLAYBACK_STATE ||
            uiGetCurrentState()==UI_SLIDESHOW_STATE)
    {
        extern AHC_BOOL bEnterPowerOffPagePause;

        if(GetMovConfig()->iState == MOV_STATE_PAUSE && bEnterPowerOffPagePause==AHC_TRUE)
        {
            MovPlayback_Resume();
            bEnterPowerOffPagePause = AHC_FALSE;
        }
    }

    AHC_OSDSetColor(0 /*OSD_COLOR_TRANSPARENT*/);
    AHC_OSDDrawFillRect(0 /*MAIN_DISPLAY_BUFFER*/, 0, 0 , m_OSD[0]->width, m_OSD[0]->height);

#ifdef FLM_GPIO_NUM //#if (VERTICAL_LCD == 1 && defined(FLM_GPIO_NUM))
    AHC_OSDRefresh_PLCD();
#endif

    return AHC_TRUE;
}

AHC_BOOL AHC_PowerOff_Option(AHC_BOOL op)
{
    if(op==CONFIRM_OPT_YES)
        AHC_PowerOff_Confirmed();
    else if(op==CONFIRM_OPT_NO)
        AHC_PowerOff_Cancel();

    return AHC_TRUE;
}
#endif

AHC_ERR AHC_PowerOffPicture(INT8 *charStr)
{
    MMP_DSC_JPEG_INFO 			jpeginfo;
    MMP_DISPLAY_OUTPUT_SEL      displaypanel;
    MMP_ERR                     err;
    UINT16                      ImgW;
    UINT16                      ImgH;

    MMPD_System_EnableClock(  MMPD_SYS_CLK_BAYER, MMP_TRUE );

#if (FS_INPUT_ENCODE == UCS2)
    uniStrcpy(jpeginfo.bJpegFileName, charStr);
    jpeginfo.usJpegFileNameLength = uniStrlen((short *)charStr);
#elif (FS_INPUT_ENCODE == UTF8)
    STRCPY(jpeginfo.bJpegFileName, charStr);
    jpeginfo.usJpegFileNameLength = STRLEN(charStr);
#endif

    jpeginfo.ulJpegBufAddr          = NULL;
    jpeginfo.ulJpegBufSize          = 0;
    jpeginfo.bDecodeThumbnail       = MMP_FALSE;
#if (DSC_SUPPORT_BASELINE_MP_FILE)
    jpeginfo.bDecodeLargeThumb 		= MMP_FALSE;
#endif
    jpeginfo.bValid                 = MMP_FALSE;

#if (SUPPORT_HDMI)
    if (HDMIFunc_IsInHdmiMode())
    {
		MMPS_Display_SetOutputPanel(MMP_DISPLAY_PRM_CTL, MMP_DISPLAY_SEL_HDMI);
    }
    else
#endif
#if (SUPPORT_TV || (MENU_SINGLE_EN))
    if (TVFunc_IsInTVMode()) {
        UINT8 ctv_sys = 0;
        if((AHC_Menu_SettingGetCB((char*)COMMON_KEY_TV_SYSTEM, &ctv_sys) == AHC_FALSE)){
            ctv_sys = COMMON_TV_SYSTEM_NTSC;//default value
        }

        switch(ctv_sys)
        {
        case COMMON_TV_SYSTEM_NTSC:
            MMPS_Display_SetOutputPanel(MMP_DISPLAY_PRM_CTL, MMP_DISPLAY_SEL_NTSC_TV);
            break;
        case COMMON_TV_SYSTEM_PAL:
        default:
            MMPS_Display_SetOutputPanel(MMP_DISPLAY_PRM_CTL, MMP_DISPLAY_SEL_PAL_TV);
            break;
        }
    }
    else
#endif
    {
        MMPS_Display_SetOutputPanel(MMP_DISPLAY_PRM_CTL, MMP_DISPLAY_SEL_MAIN_LCD);
    }

    MMPS_Display_SetWinActive(OSD_LAYER_WINDOW_ID, MMP_FALSE);

    MMPS_Display_GetOutputPanel(MMP_DISPLAY_PRM_CTL, &displaypanel);

#if (SUPPORT_HDMI && SUPPORT_TV)
    if(HDMIFunc_IsInHdmiMode() || TVFunc_IsInTVMode())
#elif   ((!SUPPORT_HDMI) && SUPPORT_TV)
    if (TVFunc_Status() == AHC_TV_NONE_STATUS)
#else
    if(0)
#endif
    {
#if (SUPPORT_TV)
        if (TVFunc_IsInTVMode())
        {
            UINT8 ctv_sys = 0;
            if((AHC_Menu_SettingGetCB((char*)COMMON_KEY_TV_SYSTEM, &ctv_sys) == AHC_FALSE)){
                ctv_sys = COMMON_TV_SYSTEM_NTSC; //default value
            }

            switch(ctv_sys)
            {
            case COMMON_TV_SYSTEM_PAL:
                ImgW = TV_PAL_POWEROFF_IMG_W;
                ImgH = TV_PAL_POWEROFF_IMG_H;
                break;
            case COMMON_TV_SYSTEM_NTSC:
            default:
                ImgW = TV_NTSC_POWEROFF_IMG_W;
                ImgH = TV_NTSC_POWEROFF_IMG_H;
              break;
            }
        }
        else
#endif
#if (SUPPORT_HDMI)
        if (HDMIFunc_IsInHdmiMode())
        {
            switch(MenuSettingConfig()->uiHDMIOutput)
            {
            case HDMI_OUTPUT_1080I:
                ImgW = HDMI_1080I_POWEROFF_IMG_W;
                ImgH = HDMI_1080I_POWEROFF_IMG_H;
                break;
            case HDMI_OUTPUT_720P:
                ImgW = HDMI_720P_POWEROFF_IMG_W;
                ImgH = HDMI_720P_POWEROFF_IMG_H;
                break;
            case HDMI_OUTPUT_480P:      // TBD
                ImgW = HDMI_480P_POWEROFF_IMG_W;
                ImgH = HDMI_480P_POWEROFF_IMG_H;
                break;
            default:
            case HDMI_OUTPUT_1080P:     // TBD
                ImgW = HDMI_1080P_POWEROFF_IMG_W;
                ImgH = HDMI_1080P_POWEROFF_IMG_H;
                break;
            }
        }
#endif

#if((SUPPORT_HDMI) && SUPPORT_TV)
            ImgW = RTNA_LCD_GetAttr()->usPanelW;
            ImgH = RTNA_LCD_GetAttr()->usPanelH;

            #if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0)
            AHC_SetDisplayWindow(DISPLAY_SYSMODE_STILLPLAYBACK, AHC_TRUE, AHC_FALSE, 0, 0, ImgW, ImgH, 0);
            #elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
            AHC_SetDisplayWindow(DISPLAY_SYSMODE_STILLPLAYBACK, AHC_TRUE, AHC_FALSE, 0, 0, ImgH, ImgW, 0);        
            #endif
#else

        #if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0)
        //???      
        #elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270) 
        //For Reset DSC Rotate config
        AHC_SetDisplayWindow(DISPLAY_SYSMODE_STILLPLAYBACK, AHC_TRUE, AHC_FALSE, 0, 0, ImgW, ImgH, 0); //What is it for?
        #endif

#endif

        err = MMPS_DSC_PlaybackJpeg(&jpeginfo);
    }
    else if (displaypanel == MMP_DISPLAY_SEL_MAIN_LCD)
    {
        ImgW = RTNA_LCD_GetAttr()->usPanelW;
        ImgH = RTNA_LCD_GetAttr()->usPanelH;

        #ifdef FLM_GPIO_NUM
        AHC_OSDSetActive(0,  AHC_FALSE);
        AHC_OSDSetActive(20, AHC_FALSE);
        AHC_OSDRefresh_PLCD();
        #endif

#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0)
        AHC_SetDisplayWindow(DISPLAY_SYSMODE_STILLPLAYBACK, AHC_TRUE, AHC_FALSE, 0, 0, ImgW, ImgH, 0);
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
        AHC_SetDisplayWindow(DISPLAY_SYSMODE_STILLPLAYBACK, AHC_TRUE, AHC_FALSE, 0, 0, ImgH, ImgW, 0);
#endif
        err = MMPS_DSC_PlaybackJpeg(&jpeginfo);
    }

    return AHC_SUCCESS;
}

ULONG AHC_BatteryDetection_Once(void)
{
#if (BATTERY_DETECTION_EN)

    UINT32  uLevel = 0;
    UINT8   uSource = 0;

    AHC_PMUCtrl_ADC_Measure_Key(&uLevel, &uSource, BATTERY_DETECT_ADC_CHANNEL);
    uLevel = uLevel * REAL_BAT_VOLTAGE_RATIO_M / REAL_BAT_VOLTAGE_RATIO_N;
    #if (ENABLE_BATTERY_DET_DEBUG == 1)
    printc("### %s - Battery Level Average -> : %d mV\r\n", __func__, uLevel);
    #endif

    return uLevel; //success in detecting
#else
    return BATT_FULL_LEVEL;
#endif
}

AHC_BOOL AHC_IsPowerPressed(void)
{
    UINT8 ubValue;

    if (MMP_GPIO_MAX != KEY_GPIO_POWER) {
        AHC_GetGPIO(KEY_GPIO_POWER, &ubValue);

        if (KEY_PRESSLEVEL_POWER == ubValue)
            return AHC_TRUE;
    }

    return AHC_FALSE;
}

#if 0
void ____SoundEffect_Function_____(){ruturn;} //dummy
#endif
AHC_BOOL AHC_IsSpeakerEnable(void)
{
    return gSpeaker_en;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SpeakerEnable
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SpeakerEnable(AHC_PIO_REG piopin, AHC_BOOL enable)
{
#if SUPPORT_SPEAKER
    #if (NEVER_TURN_OFF_SPEAKER == 1)
    enable = 1;
    #endif

    #ifdef CFG_CUS_CONTROL_SPEAKER
    enable = CFG_CUS_CONTROL_SPEAKER();
    #endif

	if (piopin != MMP_GPIO_MAX)
    {
        AHC_ConfigGPIOPad(piopin, PAD_OUT_DRIVING(0));

        if(enable)
        {
            AHC_ConfigGPIO(piopin, AHC_TRUE);
            AHC_SetGPIO(piopin, SPEAKER_ENABLE_LEVEL);
            gSpeaker_en = AHC_TRUE;
        }
        else
        {
            #ifdef SPEAKER_ENABLE_GPIO_USING_INPUT_MODE
            AHC_ConfigGPIO(piopin, AHC_FALSE);
            #else
            AHC_ConfigGPIO(piopin, AHC_TRUE);
            AHC_SetGPIO(piopin, !SPEAKER_ENABLE_LEVEL);
            #endif
            gSpeaker_en = AHC_FALSE;
        }
    }
    else
    {
        gSpeaker_en = AHC_FALSE;
        return AHC_FAIL;
    }
#endif

    return AHC_SUCCESS;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetSoundEffectStatus
//  Description :
//------------------------------------------------------------------------------
#define AIT_BOOT_HEADER_ADDR     0x106200
#define SOUNDFILE_DRAM_ADDR      AHC_SOUNDFILE_DRAM_ADDR

AHC_BOOL AHC_GetSoundEffectStatus(UINT8 *ubSoundEffectStatus)
{
    MMPS_AUDIO_PLAY_STATUS audio_status = 0;

    MMPS_AUI_GetPlayStatus(&audio_status);

    switch(audio_status)
    {
        case MMPS_AUDIO_PLAY_START:
            *ubSoundEffectStatus = AHC_SOUND_EFFECT_STATUS_START;
        break;
        case MMPS_AUDIO_PLAY_PAUSE:
            *ubSoundEffectStatus = AHC_SOUND_EFFECT_STATUS_PAUSE;
        break;
        case MMPS_AUDIO_PLAY_STOP:
            *ubSoundEffectStatus = AHC_SOUND_EFFECT_STATUS_STOP;
        break;
        case MMPS_AUDIO_PLAY_INVALID:
            *ubSoundEffectStatus = AHC_SOUND_EFFECT_STATUS_INVALID;
        break;
    }
    return AHC_TRUE;
}

#define USE_WAV_SOUND_EFFECT (0)
#define USE_MP3_SOUND_EFFECT (1)

char* GetAudioFileExtension(char* path)
{
	int i;
	int nLength = STRLEN(path);
	int nTmp    = nLength;
	int nExtSize = 0;
	char szAudioExt[6];
	
	while( nTmp && path[nTmp] != '.' )
	{
		nTmp--;
	}

	if(nTmp==0) return AHC_FALSE;

	nTmp++;

	nExtSize = nLength - nTmp;

	for( i=0; i< nExtSize; i++ )
	{
		szAudioExt[i] = path[nTmp+i];
	}

	szAudioExt[i] = '\0';
    
    return szAudioExt;
}

//------------------------------------------------------------------------------
//  Function    : AHC_PlaySoundEffect
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_PlaySoundEffect(AHC_SOUNDEFFECT_TYPE soundtype)
{
    MMPS_AUDIO_FILEINFO fileinfo;
    MMPS_AUDIO_WAV_INFO wavinfo;
    MMPS_AUDIO_MP3_INFO mp3info;
    static UINT8		ubFileType = USE_WAV_SOUND_EFFECT;
    MMP_ERR             mmps_status;
    UINT32              value;
    UINT8               ubSoundEffectStatus = 0;
    UINT32              ulDacStatus = 0;
    char                path[256];
    #if(PLAY_SOUND_SEQUENTIALLY)
    AHC_RTC_TIME        sCurTime;
    UINT8               ubCnt = 0;
    #endif
    char                *pSoundEffectPath = NULL;

    printc("soundtype : %d\r\n",soundtype);
	
	#ifdef SoundEffectBreakExcptionCase
	if (AHC_DetectSoundEffectBreak());
		return AHC_FAIL;
	#endif
	
    AIHC_GetMovieConfig(AHC_AUD_PRERECORD_DAC, &ulDacStatus );

    if(soundtype < AHC_SOUNDEFFECT_SPCAM_START || soundtype > AHC_SOUNDEFFECT_SPCAM_END)
    {
        if((MenuSettingConfig()->uiBeep == BEEP_OFF)||(ulDacStatus == AHC_FALSE))
        {
            printc("--I-- Beep is OFF or Volume is 0\r\n");
            return AHC_FAIL;
        }
    }

    #if (PLAY_SOUND_SEQUENTIALLY)
    if(soundtype == AHC_SOUNDEFFECT_PLAYSOUND_EVENT)
    {
        if(gubAhcPlaySoundQue == 1)
		{
			gubAhcPlaySoundQue = 0;
		}
        goto L_SOUNDPLAY;
    }

    AHC_RTC_GetTime(&sCurTime);
    for(ubCnt = 0; ubCnt < PLAY_SOUND_FILE_QUEUE_SIZE; ubCnt++)
    {
        #if (PLAY_SOUND_DELETE_SAME_AUDIO)
        if(gsAhcPlaySoundQue[ubCnt].soundtype == soundtype)
        {
            printc("same AUI type\r\n");
            break;
        }
        #endif
		
        if(gsAhcPlaySoundQue[ubCnt].soundtype == AHC_SOUNDEFFECT_MAX_NUM)
        {
            gsAhcPlaySoundQue[ubCnt].soundtype = soundtype;
            memcpy(&(gsAhcPlaySoundQue[ubCnt].soundInitTime), &sCurTime, sizeof(AHC_RTC_TIME));
            gubAhcPlaySoundQueEventNum = ubCnt + 1;
            //printc("AUI Index=%d, Q.Yesr=%d, cur.Year=%d\r\n",ubCnt, gsAhcPlaySoundQue[ubCnt].soundInitTime.uwYear, sCurTime.uwYear);
            break;
        }
    }

    #if(PLAY_SOUND_TIMEOUT_TIME != 0)
    if(ubCnt == PLAY_SOUND_FILE_QUEUE_SIZE)
    {
        UINT32  uiCurTime = 0xFFFFFFFF;
        UINT32  uiOldestFileTime  = 0xFFFFFFFF;

        uiCurTime = sCurTime.uwYear - 2000;
        uiCurTime = uiCurTime << 26;
        uiCurTime = uiCurTime | sCurTime.uwMonth << 22  |\
                                        sCurTime.uwDay << 17    |\
                                        sCurTime.uwHour << 12   |\
                                        sCurTime.uwMinute << 6  |\
                                        sCurTime.uwSecond;

        uiOldestFileTime = gsAhcPlaySoundQue[0].soundInitTime.uwYear - 2000;
        uiOldestFileTime = uiOldestFileTime << 26;
        uiOldestFileTime = uiOldestFileTime |   gsAhcPlaySoundQue[0].soundInitTime.uwMonth << 22    |\
                                                gsAhcPlaySoundQue[0].soundInitTime.uwDay << 17  |\
                                                gsAhcPlaySoundQue[0].soundInitTime.uwHour << 12 |\
                                                gsAhcPlaySoundQue[0].soundInitTime.uwMinute << 6    |\
                                                gsAhcPlaySoundQue[0].soundInitTime.uwSecond + PLAY_SOUND_TIMEOUT_TIME;
        if(uiCurTime > uiOldestFileTime)
        {
            printc("Old Sound Timeout \r\n");
            for(ubCnt = 0 ; ubCnt < PLAY_SOUND_FILE_QUEUE_SIZE-1; ubCnt++)
            {
                memcpy(&(gsAhcPlaySoundQue[ubCnt]), &(gsAhcPlaySoundQue[ubCnt+1]), sizeof(AHC_SOUND_FILE_QUEUE));
            }
            gubAhcPlaySoundQueEventNum = gubAhcPlaySoundQueEventNum - 1;
            gsAhcPlaySoundQue[PLAY_SOUND_FILE_QUEUE_SIZE-1].soundtype = soundtype;
            memcpy(&(gsAhcPlaySoundQue[PLAY_SOUND_FILE_QUEUE_SIZE-1].soundInitTime), &sCurTime, sizeof(AHC_RTC_TIME));
        }
    }
    #endif

    for(ubCnt = 0 ; ubCnt < PLAY_SOUND_FILE_QUEUE_SIZE; ubCnt++)
    {
        //printc("1 Q[%d].sound=%d\r\n", ubCnt, gsAhcPlaySoundQue[ubCnt].soundtype);
    }
    #endif

    AHC_GetSoundEffectStatus(&ubSoundEffectStatus);

    #if(PLAY_SOUND_SEQUENTIALLY)
    if(ubSoundEffectStatus != AHC_SOUND_EFFECT_STATUS_STOP)
    {
        return AHC_SUCCESS;
    }
    #else
    if(ubSoundEffectStatus == AHC_SOUND_EFFECT_STATUS_START) {
		if (ubFileType == USE_MP3_SOUND_EFFECT)
			mmps_status = MMPS_AUI_StopMP3Play();
		else
			mmps_status = MMPS_AUI_StopWAVPlay();
    }
    #endif

#if(PLAY_SOUND_SEQUENTIALLY)
L_SOUNDPLAY:
#endif

    #if(PLAY_SOUND_SEQUENTIALLY)
    pSoundEffectPath = AHC_GetSoundEffectFileName(gsAhcPlaySoundQue[0].soundtype);
    #else
    pSoundEffectPath = AHC_GetSoundEffectFileName(soundtype);
    #endif

    if (NULL == pSoundEffectPath) {
        return AHC_FAIL;
    }

    STRCPY(path, pSoundEffectPath);

    if( STRCMP(GetAudioFileExtension(path), "mp3") == 0 )
    {
    	ubFileType = USE_MP3_SOUND_EFFECT;
    }
    else
    {
    	ubFileType = USE_WAV_SOUND_EFFECT;
    }

#if(PLAY_SOUND_SEQUENTIALLY)
    for(ubCnt = 0 ; ubCnt < PLAY_SOUND_FILE_QUEUE_SIZE-1; ubCnt++)
    {
        memcpy(&(gsAhcPlaySoundQue[ubCnt]), &(gsAhcPlaySoundQue[ubCnt+1]), sizeof(AHC_SOUND_FILE_QUEUE));
    }
    gubAhcPlaySoundQueEventNum = gubAhcPlaySoundQueEventNum - 1;
    gsAhcPlaySoundQue[PLAY_SOUND_FILE_QUEUE_SIZE-1].soundtype = AHC_SOUNDEFFECT_MAX_NUM;
    memset(&(gsAhcPlaySoundQue[PLAY_SOUND_FILE_QUEUE_SIZE-1].soundInitTime), 0, sizeof(AHC_RTC_TIME));

    for(ubCnt = 0 ; ubCnt < PLAY_SOUND_FILE_QUEUE_SIZE; ubCnt++)
    {
        //printc("2 Q[%d].sound=%d\r\n", ubCnt, gsAhcPlaySoundQue[ubCnt].soundtype);
    }
    printc("file path: %s\r\n", path);
#endif

    #if (FS_INPUT_ENCODE == UCS2)
    uniStrcpy(fileinfo.bFileName, path);
    fileinfo.usFileNameLength = uniStrlen((short *)fileinfo.bFileName);
    #elif (FS_INPUT_ENCODE == UTF8)
    STRCPY(fileinfo.bFileName, path);
    fileinfo.usFileNameLength = STRLEN(path);
    #endif

    MMPS_AUI_SetMediaPath(MMPS_AUI_MEDIA_PATH_CARD);
	if (ubFileType == USE_MP3_SOUND_EFFECT)
		mmps_status = MMPS_AUI_InitializeMP3Play(&fileinfo, &mp3info);
	else
    mmps_status = MMPS_AUI_InitializeWAVPlay(&fileinfo, &wavinfo);
 
    AHC_GetParam(PARAM_ID_AUDIO_VOLUME_DB,&value);

    #if (AUDIO_SET_DB == 0x1)
    MMPS_AUDIO_SetPlayVolumeDb(value);
    #else
    MMPS_AUDIO_SetPlayVolume(value, AHC_FALSE);
    #endif

    if (ubFileType == USE_MP3_SOUND_EFFECT)
    	MMPS_AUI_StartMP3Play();
    else
    	MMPS_AUI_StartWAVPlay();

    #if (SUPPORT_SPEAKER == 1)
    {
        #ifdef TV_SPEAKER_OUT_EN

        #if (SUPPORT_HDMI)
        if(HDMIFunc_GetStatus() == AHC_HDMI_NONE_STATUS)
        #else
        if(1)
        #endif
        {
            AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_TRUE);
        }
        else {
            AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_FALSE);
        }

        #else

        #if (SUPPORT_HDMI && SUPPORT_TV)
        if(HDMIFunc_GetStatus()==AHC_HDMI_NONE_STATUS && TVFunc_Status()==AHC_TV_NONE_STATUS)
        #elif ((!SUPPORT_HDMI) && SUPPORT_TV)
        if(TVFunc_Status()==AHC_TV_NONE_STATUS)
        #elif (SUPPORT_HDMI && (!SUPPORT_TV))
        if (TVFunc_Status()==AHC_TV_NONE_STATUS)
        #else
        if(1)
        #endif
        {
            AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_TRUE);
        }
        else {
            AHC_SpeakerEnable(SPEAKER_ENABLE_GPIO, AHC_FALSE);
        }

        #endif
    }
    #endif
    #if (SUPPORT_HDMI)
    if(HDMIFunc_GetStatus() != AHC_HDMI_NONE_STATUS) {
        AHC_OS_SleepMs(400); //Fix Issue: Audio I2S ISR doesn't disable Reg0x80008800=0x08
    }
    #endif

	#if ( (MENU_STYLE == 1) && (!STYLE001_SUPPORT_TOUCH_PANEL) )
	 // Fix no Beep sound issue when UI leave Menu state to Video state in charging case.
 	#if (ENABLE_CHARGER_IC && defined(Car_DV))
        if (!AHC_GetChargerStatus())
    #endif
            AHC_OS_SleepMs(200);
	#endif

    return AHC_SUCCESS;
}

AHC_BOOL AHC_SetSoundEffectExist(AHC_BOOL bExist)
{
    m_bSoundEffectExist = bExist;
    return AHC_SUCCESS;
}

UINT8 AHC_GetSoundEffectExist(void)
{
    return m_bSoundEffectExist;
}

AHC_BOOL AHC_LoadSoundEffect(void)
{
    char *SoundEffectfile[256] = {"SM:1:\\Shutter.wav"     ,"SM:1:\\start.wav"          ,"SM:1:\\sound_operation.wav"   ,"SM:1:\\Record_start.wav",
                                  "SM:1:\\Record_stop.wav" ,"SM:1:\\sound_focus.wav"    ,"SM:1:\\sound_error.wav",
                                  "SM:1:\\Record_start.wav","SM:1:\\SelfTimer.wav"};
    UINT8   i;
    UINT32  ulFileId,ulFileSize,ulFileLoad;
    UINT32  error = 0;

    if(SOUNDFILE_DRAM_ADDR == 0) {
        printc("AHC_LoadSoundEffect: SOUNDFILE_DRAM_ADDR=0\r\n");
        AHC_SetSoundEffectExist(AHC_FALSE);
        return AHC_FALSE;
    }

    for(i=0;i<9;i++)
    {
        STRCPY(SoundEffectFile[i].path,SoundEffectfile[i]);

        error |= AHC_FS_FileOpen(SoundEffectFile[i].path,AHC_StrLen(SoundEffectFile[i].path),"rb",AHC_StrLen("rb"),&ulFileId);
        error |= AHC_FS_FileGetSize(ulFileId,&ulFileSize);

        SoundEffectFile[i].ulSize = ulFileSize;

        if(i == 0)
            SoundEffectFile[i].ulStartAddress = SOUNDFILE_DRAM_ADDR;
        else
            SoundEffectFile[i].ulStartAddress = (SoundEffectFile[i-1].ulStartAddress + SoundEffectFile[i-1].ulSize);

        error |= AHC_FS_FileRead(ulFileId,(UINT8 *)SoundEffectFile[i].ulStartAddress,SoundEffectFile[i].ulSize,&ulFileLoad);
        error |= AHC_FS_FileClose(ulFileId);
    }

    if(error)
    {
        AHC_SetSoundEffectExist(AHC_FALSE);
        return AHC_FALSE;
    }
    else
    {
        AHC_SetSoundEffectExist(AHC_TRUE);
        return AHC_SUCCESS;
    }
}


#if 0
void ____MenuSet_Function_________(){ruturn;} //dummy
#endif

AHC_BOOL Menu_SetMotionDtcSensitivity(UINT8 val)
{
#if (MOTION_DETECTION_EN)

    #if defined(CFG_MVD_MODE_LINK_WITH_MENU_SENSITIVITY) && (VMD_ACTION != VMD_ACTION_NONE)
    extern void check_mvd_mode_with_menu_sensitivity(UINT8 val);
    check_mvd_mode_with_menu_sensitivity(val);
    #endif

    switch(val)
    {
    case MOTION_DTC_SENSITIVITY_LOW:
        m_ubMotionMvTh  = MVD_LO_MVTH;
        m_ubMotionCntTh = MVD_LO_CNTH;
        break;

    case MOTION_DTC_SENSITIVITY_MID:
        m_ubMotionMvTh  = MVD_MI_MVTH;
        m_ubMotionCntTh = MVD_MI_CNTH;
        break;

    case MOTION_DTC_SENSITIVITY_HIGH:
        m_ubMotionMvTh  = MVD_HI_MVTH;
        m_ubMotionCntTh = MVD_HI_CNTH;
        break;

    default:
    //case MOTION_DTC_SENSITIVITY_OFF:
        m_ubMotionMvTh  = 0xFF;
        m_ubMotionCntTh = 0xFF;
        break;
    }
#endif

    return AHC_TRUE;
}

#ifdef CAR_DV
// NOP
#else
AHC_BOOL Menu_SetMotionDtcSensitivity_Parking(UINT8 val)
{
    #if (MOTION_DETECTION_EN)
    switch(val)
    {
        case PARKINGMODE_IMAGE_DETECTION_LOW:
            m_ubMotionMvTh  = MVD_LO_MVTH;
            m_ubMotionCntTh = MVD_LO_CNTH;
        break;
        case PARKINGMODE_IMAGE_DETECTION_MEDIUM:
            m_ubMotionMvTh  = MVD_MI_MVTH;
            m_ubMotionCntTh = MVD_MI_CNTH;
        break;
        case PARKINGMODE_IMAGE_DETECTION_HIGH:
            m_ubMotionMvTh  = MVD_HI_MVTH;
            m_ubMotionCntTh = MVD_HI_CNTH;
        break;
        default:
            m_ubMotionMvTh  = 0xFF;
            m_ubMotionCntTh = 0xFF;
        break;
    }
    #endif
    return AHC_TRUE;
}
#endif

#if 0
void _____LCD_Function_________(){ruturn;} //dummy
#endif

void AHC_LCD_SetBrightness(UINT8 brightness)
{
    RTNA_LCD_AdjustBrightness(brightness);
}

void AHC_LCD_SetContrast(UINT8 Contrast)
{
    RTNA_LCD_AdjustContrast(Contrast);
}

void AHC_LCD_SetBrightness_R(UINT8 brightness)
{
    RTNA_LCD_AdjustBrightness_R(brightness);
}

void AHC_LCD_SetContrast_R(UINT8 Contrast)
{
    RTNA_LCD_AdjustContrast_R(Contrast);
}

void AHC_LCD_SetBrightness_B(UINT8 brightness)
{
    RTNA_LCD_AdjustBrightness_B(brightness);
}

void AHC_LCD_SetContrast_B(UINT8 Contrast)
{
    RTNA_LCD_AdjustContrast_B(Contrast);
}

void AHC_LCD_Direction(AHC_LCD_DIRECTION dir)
{
    RTNA_LCD_Direction(dir);
}

void AHC_LCD_Write_Reg(UINT32 reg, UINT8 value)
{
    RTNA_LCD_SetReg(reg,value);
}

AHC_BOOL AHC_LCD_GetRotateConfig(AHC_BOOL *bWithHallSnr, AHC_BOOL *bSnrFlipEn, AHC_BOOL *bOSDFlipEn)
{
    *bWithHallSnr   = (LCD_ROTATE_WITH_HALL_SENSOR);
    *bSnrFlipEn     = SENSOR_FLIP_ENABLE;
    *bOSDFlipEn     = PANEL_OSD_FILP_ENABLE;
    return AHC_TRUE;
}

void AHC_LCD_GetStatus(UINT8 *status)
{
#ifdef CAR_DV   // TBD

    UINT32 LCDstatus;

    AHC_GetParam(PARAM_ID_LCD_STATUS, &LCDstatus);
    *status = LCDstatus;

#else

    UINT8 temp = 0;
    AHC_BOOL ubWithHallSnr = 0,ubSnrFlipEn = 0,ubOSDFlipEn = 0;

#if(PANEL_OSD_FILP_ENABLE == 0)
    *status = AHC_LCD_NORMAL;
    return;
#endif

    AHC_LCD_GetRotateConfig(&ubWithHallSnr, &ubSnrFlipEn, &ubOSDFlipEn);

    if(DEVICE_GPIO_LCD_INV == AHC_PIO_REG_UNKNOWN)
    {
        *status = AHC_LCD_REVERSE;
    }
    else
    {
        AHC_GetGPIO(DEVICE_GPIO_LCD_INV,&temp);
    }

    if(ubWithHallSnr==AHC_FALSE)
    {
        if(ubSnrFlipEn==AHC_TRUE && ubOSDFlipEn==AHC_FALSE)
            *status = AHC_LCD_NORMAL;
        else if(ubSnrFlipEn==AHC_TRUE && ubOSDFlipEn==AHC_TRUE)
#if (MENU_GENERAL_EN && MENU_GENERAL_LCD_ROTATE_EN)
            *status = (MenuSettingConfig()->uiLCDRotate==LCD_ROTATE_ON)?(AHC_LCD_REVERSE):(AHC_LCD_NORMAL);
#else
            *status = AHC_LCD_NORMAL;
#endif
        else if(ubSnrFlipEn==AHC_FALSE && ubOSDFlipEn==AHC_TRUE)
#if (MENU_GENERAL_EN && MENU_GENERAL_LCD_ROTATE_EN)
            *status = (MenuSettingConfig()->uiLCDRotate==LCD_ROTATE_ON)?(AHC_LCD_REVERSE):(AHC_LCD_NORMAL);
#else
            *status = AHC_LCD_NORMAL;
#endif
        else
            *status = AHC_LCD_NORMAL;
    }
    else
    {
        if(ubSnrFlipEn==AHC_TRUE && ubOSDFlipEn==AHC_FALSE)
        {
            *status = (temp)?(AHC_LCD_NORMAL):(AHC_LCD_REVERSE);
        }
        else if(ubSnrFlipEn==AHC_TRUE && ubOSDFlipEn==AHC_TRUE)
        {
#if (MENU_GENERAL_EN && MENU_GENERAL_LCD_ROTATE_EN)
            if(MenuSettingConfig()->uiLCDRotate==LCD_ROTATE_ON)
#else
            if(0)
#endif
                *status = (temp)?(AHC_LCD_REVERSE):(AHC_LCD_NORMAL);
            else
                *status = (temp)?(AHC_LCD_NORMAL):(AHC_LCD_REVERSE);
        }
        else if(ubSnrFlipEn==AHC_FALSE && ubOSDFlipEn==AHC_TRUE)
        {
#if (MENU_GENERAL_EN && MENU_GENERAL_LCD_ROTATE_EN)
            if(MenuSettingConfig()->uiLCDRotate==LCD_ROTATE_ON)
#else
            if(0)
#endif
                *status = (temp)?(AHC_LCD_REVERSE):(AHC_LCD_NORMAL);
            else
                *status = (temp)?(AHC_LCD_NORMAL):(AHC_LCD_REVERSE);
        }
        else
            *status = (temp)?(AHC_LCD_NORMAL):(AHC_LCD_REVERSE);//General Case TBD
    }
#endif
}

#if 0
void _____Misc_Function_________(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : AHC_SetLsCsCali
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetLsCsCali(void)
{
    UartExecuteCommandEx("lscscali");
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetAwbCali
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetAwb0Cali(void)
{
    UartExecuteCommandEx("awb0cali");
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetAeCali
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetAeAwb1Cali(void)
{
    UartExecuteCommandEx("aeawbcali");
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetAfCali
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetAfCali(void)
{
    UartExecuteCommandEx("afinfposcali");
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetRdCaliBin
//  Description :
//------------------------------------------------------------------------------
extern MMP_BOOL CalibrationMode;
extern void OSD_Draw_TestMessage(UINT32 Left, UINT32 Top, char result,char  sText[20] );

AHC_BOOL AHC_SetRdCaliBin(void)
{
    OSD_Draw_TestMessage(100,100,4,"Writing..");//set
    UartExecuteCommandEx("genispbin");
    AHC_OS_Sleep(1500);
    //OSD_Draw_TestMessage(100,100,0,"ISP gen");//set
    UartExecuteCommandEx("rdcalibin");
    AHC_OS_Sleep(1500);
    OSD_Draw_TestMessage(100,120,0,"To flash");//set

    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetRdCaliBin
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_CaptureRawAfterCS(void)
{
    UartExecuteCommandEx("getcsraw 0 0");
    return AHC_TRUE;
}

AHC_BOOL AHC_Gsensor_SetCurInfo(void)
{
#if (GSENSOR_CONNECT_ENABLE)
    if(AHC_Gsensor_Module_Attached())
        AHC_Gsensor_RestoreAviChuck();
    else
    return AHC_TRUE;
#endif
    return AHC_TRUE;
}

AHC_BOOL AHC_GPS_SetCurInfo(void)
{
#if (GPS_CONNECT_ENABLE)
    if(AHC_GPS_Module_Attached())
    {
        UINT8 bgps_en = 0;
        if((AHC_Menu_SettingGetCB((char*)COMMON_KEY_GPS_RECORD_ENABLE, &bgps_en) == AHC_TRUE) && (bgps_en != COMMON_GPS_REC_INFO_OFF)){
           GPSCtrl_RestoreAviChuck();
    }
    }
    else
    return AHC_TRUE;
#endif
    return AHC_TRUE;
}

int AHC_GetRAMExtension(void)
{
    if (MMPS_System_GetConfig()->ulStackMemorySize >> 20 == 64) {
        return 32 * 1024 * 1024;
    }
    return 0;
}

UINT32 AHC_GetPreRecordTime(void)
{
    static UINT32       m_ulPreRecordingTime        = 3000; //Unit:mili-seconds
    /* When the DRAM is more than 32M, the preencode time can be more, UNDER TESTING */
    m_ulPreRecordingTime = (AHC_GetRAMExtension())?(6000):(3000);
    return m_ulPreRecordingTime;
}

#if 0
void ____GPS_Function_____(){ruturn;} //dummy
#endif

AHC_BOOL AHC_GPS_Module_Attached(void)
{

    #if (GPS_MODULE_ATT_COMPILER)
     #if(GPS_CONNECT_ENABLE)
        return AHC_TRUE;
     #else
        return AHC_FALSE;
     #endif
    #elif (GPS_MODULE_ATT_GPIO)
        UINT8 ubValue = AHC_FALSE;
		if(GPS_MODULE_GPIO != MMP_GPIO_MAX)
        {
            AHC_GetGPIO(GPS_MODULE_GPIO, &ubValue);
            return ubValue;
        }else
        {
            return AHC_FALSE;
        }
    #endif
    return AHC_FALSE;
}

#if 0
void ____MenuSet_Function_____(){ruturn;} //dummy
#endif

AHC_BOOL AHC_RestoreFromDefaultSetting(void)
{
    #ifdef FACTORY_RESET
    SetKeyPadEvent(FACTORY_RESET);
    #endif
    
    return AHC_TRUE;
}

AHC_BOOL Menu_SetGsensorSensitivity(UINT8 val)
{
#if (GSENSOR_CONNECT_ENABLE)
    AHC_Gsensor_SetIntThreshold();
#endif

    return AHC_TRUE;
}

AHC_BOOL Menu_SetLCDDirection(UINT8 val, UINT8 backupVal)
{
#ifdef CAR_DV   // TBD
    // NOP
#else
    UINT32      LCDStatus;
    UINT32      CurMode;
    AHC_BOOL    bPreviewMode = AHC_TRUE;
    AHC_BOOL    ubWithHallSnr,ubSnrFlipEn,ubOSDFlipEn;

    AHC_LCD_GetRotateConfig(&ubWithHallSnr, &ubSnrFlipEn, &ubOSDFlipEn);

    AHC_GetParam(PARAM_ID_LCD_STATUS,&LCDStatus);

    AHC_GetSystemStatus(&CurMode);

    CurMode >>= 16;

    if( CurMode!=AHC_MODE_CAPTURE_PREVIEW &&
        CurMode!=AHC_MODE_RECORD_PREVIEW  &&
        CurMode!=AHC_MODE_RAW_PREVIEW   )
    {
        printc("Not Flip Sensor in Non-Preview Mode!!\r\n");
        bPreviewMode =  AHC_FALSE;
    }

    if(ubSnrFlipEn==AHC_TRUE && ubOSDFlipEn==AHC_FALSE)
    {
        if(bPreviewMode == AHC_FALSE)
            return AHC_TRUE;

        LedCtrl_LcdBackLight(AHC_FALSE);

        #if (MENU_GENERAL_EN && MENU_GENERAL_LCD_ROTATE_EN)
        if(val == LCD_ROTATE_ON)
        #else
        if(val == 0)
        #endif
        {
            AHC_SetParam(PARAM_ID_LCD_STATUS,1);
            #if (LCD_PLACEMENT_TYPE == LCD_DIR_0)
            AHC_LCD_Direction(AHC_LCD_DOWNRIGHT_UPLEFT);
            #elif (LCD_PLACEMENT_TYPE == LCD_DIR_1)
            AHC_LCD_Direction(AHC_LCD_UPRIGHT_DOWNLEFT);
            #elif (LCD_PLACEMENT_TYPE == LCD_DIR_2)
            AHC_LCD_Direction(AHC_LCD_DOWNLEFT_UPRIGHT);
            #elif (LCD_PLACEMENT_TYPE == LCD_DIR_3)
            AHC_LCD_Direction(AHC_LCD_UPLEFT_DOWNRIGHT);
            #endif
            AHC_OS_SleepMs(100);
        }
        #if (MENU_GENERAL_EN && MENU_GENERAL_LCD_ROTATE_EN)
        else if(val == LCD_ROTATE_OFF)
        #else
        else
        #endif
        {
            AHC_SetParam(PARAM_ID_LCD_STATUS,0);
            #if (LCD_PLACEMENT_TYPE == LCD_DIR_0)
            AHC_LCD_Direction(AHC_LCD_UPLEFT_DOWNRIGHT);
            #elif (LCD_PLACEMENT_TYPE == LCD_DIR_1)
            AHC_LCD_Direction(AHC_LCD_DOWNLEFT_UPRIGHT);
            #elif (LCD_PLACEMENT_TYPE == LCD_DIR_2)
            AHC_LCD_Direction(AHC_LCD_UPRIGHT_DOWNLEFT);
            #elif (LCD_PLACEMENT_TYPE == LCD_DIR_3)
            AHC_LCD_Direction(AHC_LCD_DOWNRIGHT_UPLEFT);
            #endif
            AHC_OS_SleepMs(100);
        }
        LedCtrl_LcdBackLight(AHC_TRUE);
    }
    else if( (ubSnrFlipEn==AHC_TRUE  && ubOSDFlipEn==AHC_TRUE) ||
             (ubSnrFlipEn==AHC_FALSE && ubOSDFlipEn==AHC_TRUE)  )
    {
        LedCtrl_LcdBackLight(AHC_FALSE);

        #if (MENU_GENERAL_EN && MENU_GENERAL_LCD_ROTATE_EN)
        if(val == LCD_ROTATE_ON && backupVal == LCD_ROTATE_OFF)
        #else
        if(0)
        #endif
        {
            if(ubWithHallSnr==AHC_FALSE)
            {
                AHC_SetParam(PARAM_ID_LCD_STATUS,1);
                #if (LCD_PLACEMENT_TYPE == LCD_DIR_0)
                AHC_LCD_Direction(AHC_LCD_DOWNRIGHT_UPLEFT);
                #elif (LCD_PLACEMENT_TYPE == LCD_DIR_1)
                AHC_LCD_Direction(AHC_LCD_UPRIGHT_DOWNLEFT);
                #elif (LCD_PLACEMENT_TYPE == LCD_DIR_2)
                AHC_LCD_Direction(AHC_LCD_DOWNLEFT_UPRIGHT);
                #elif (LCD_PLACEMENT_TYPE == LCD_DIR_3)
                AHC_LCD_Direction(AHC_LCD_UPLEFT_DOWNRIGHT);
                #endif
            }
            else
            {
                if(LCDStatus==0)
                    AHC_SendAHLMessage(AHLM_GPIO_BUTTON_NOTIFICATION, DEVICE_LCD_INV, 0);
                else
                    AHC_SendAHLMessage(AHLM_GPIO_BUTTON_NOTIFICATION, DEVICE_LCD_NOR, 0);
            }

            if(bPreviewMode && ubSnrFlipEn==AHC_TRUE)
            {
                #if (SENSOR_PLACEMENT_TYPE == SENSOR_DIR_0)
                AHC_Set_SensorFlip(AHC_SENSOR_NO_FLIP);
                #elif (SENSOR_PLACEMENT_TYPE == SENSOR_DIR_1)
                AHC_Set_SensorFlip(AHC_SENSOR_COLROW_FLIP);
                #endif
                AHC_OS_SleepMs(100);
            }
        }
        #if (MENU_GENERAL_EN && MENU_GENERAL_LCD_ROTATE_EN)
        else if(val == LCD_ROTATE_OFF && backupVal == LCD_ROTATE_ON)
        #else
        else if(0)
        #endif
        {
            if(ubWithHallSnr==AHC_FALSE)
            {
                AHC_SetParam(PARAM_ID_LCD_STATUS,0);
                #if (LCD_PLACEMENT_TYPE == LCD_DIR_0)
                AHC_LCD_Direction(AHC_LCD_UPLEFT_DOWNRIGHT);
                #elif (LCD_PLACEMENT_TYPE == LCD_DIR_1)
                AHC_LCD_Direction(AHC_LCD_DOWNLEFT_UPRIGHT);
                #elif (LCD_PLACEMENT_TYPE == LCD_DIR_2)
                AHC_LCD_Direction(AHC_LCD_UPRIGHT_DOWNLEFT);
                #elif (LCD_PLACEMENT_TYPE == LCD_DIR_3)
                AHC_LCD_Direction(AHC_LCD_DOWNRIGHT_UPLEFT);
                #endif
            }
            else
            {
                if (LCDStatus==1)
                    AHC_SendAHLMessage(AHLM_GPIO_BUTTON_NOTIFICATION, DEVICE_LCD_NOR, 0);
                else
                    AHC_SendAHLMessage(AHLM_GPIO_BUTTON_NOTIFICATION, DEVICE_LCD_INV, 0);
            }

            if(bPreviewMode && ubSnrFlipEn==AHC_TRUE)
            {
                #if (SENSOR_PLACEMENT_TYPE == SENSOR_DIR_0)
                AHC_Set_SensorFlip(AHC_SENSOR_COLROW_FLIP);
                #elif (SENSOR_PLACEMENT_TYPE == SENSOR_DIR_1)
                AHC_Set_SensorFlip(AHC_SENSOR_NO_FLIP);
                #endif
                AHC_OS_SleepMs(100);
            }
        }
        LedCtrl_LcdBackLight(AHC_TRUE);
    }
#endif

    return AHC_TRUE;
}

#if (POWER_OFF_DELAY_EN)
UINT32 AHC_GetChargerPowerOffTime(void)
{
    UINT32 PowerOffTime;
    UINT32 Multiplier = 7;
#if (MENU_MOVIE_EN && MENU_MOVIE_POWER_OFF_DELAY_EN)
    switch(MenuSettingConfig()->uiMOVPowerOffTime)
    {

        case MOVIE_POWEROFF_TIME_5SEC:
            PowerOffTime = 5;
            break;
         case MOVIE_POWEROFF_TIME_15SEC:
            PowerOffTime = 15;
            break;
        case MOVIE_POWEROFF_TIME_30SEC:
            PowerOffTime = 30;
            break;
        case MOVIE_POWEROFF_TIME_1MIN:
            PowerOffTime = 60;
            break;
        default:
            PowerOffTime = 0;
            break;
    }
#endif
#if (MENU_SINGLE_EN)
    switch(MenuSettingConfig()->uiDelayPowerOff)
    {
        case DELAY_POWEROFF_10SEC:
            PowerOffTime = 10;
            break;
        default:
            PowerOffTime = 0;
            break;
    }
#endif


    #if((MENU_MOVIE_EN && MENU_MOVIE_POWER_OFF_DELAY_EN) || (MENU_SINGLE_EN))
    return (PowerOffTime * Multiplier); //Multiplier is set to adjust PowerOffTime (sec) with real time.
    #else
    return (POWER_OFF_DELAY_TIME_DEFAULT * Multiplier); //Delay 1.5 second.(1.5*7)
    #endif
}
#endif

#if 0
void ____SD_Card_Function_____(){ruturn;} //dummy
#endif

SDMMC_STATE AHC_SDMMC_GetState(void)
{
    return m_eSDMMCState;
}

void AHC_SDMMC_SetState(SDMMC_STATE val)
{
    m_eSDMMCState = val;
}

#if (ENABLE_SD_HOT_PLUGGING)

AHC_BOOL AHC_SDMMC_GetMountState(void)
{
    return m_bSDMMCMountState;
}

void AHC_SDMMC_SetMountState(SDMMC_STATE val)
{
    m_bSDMMCMountState = val;
}

AHC_BOOL AHC_SDMMC_IsSD1MountDCF(void)
{
    if (AHC_SDMMC_GetMountState() == AHC_SD1_MASTER_MOUNT_SD2_NO_MOUNT ||
    	AHC_SDMMC_GetMountState() == AHC_SD1_MASTER_MOUNT_SD2_SLAVE_MOUNT)
        return AHC_TRUE;
	else
		return AHC_FALSE;
}

AHC_BOOL AHC_SDMMC_IsSD2MountDCF(void)
{
    if (AHC_SDMMC_GetMountState() == AHC_SD1_NO_MOUNT_SD2_MASTER_MOUNT ||
    	AHC_SDMMC_GetMountState() == AHC_SD1_SLAVE_MOUNT_SD2_MASTER_MOUNT)
        return AHC_TRUE;
	else
		return AHC_FALSE;
}
#endif

UINT8 AHC_SDMMC_GetClass(UINT32 id)
{
    #if(defined(DEVICE_GPIO_2NDSD_PLUG))
    AHC_SDMMC_Get1stCardClass(SD_SOCKET0_MAP_ID);
    AHC_SDMMC_Get1stCardClass(SD_SOCKET1_MAP_ID);

    printc("Main Media: %d\r\n", MenuSettingConfig()->uiMediaSelect);
    printc("Current SD Card ID1 Class %d\r\n", m_ubSDMMC_Class);
    printc("Current SD Card ID2 Class %d\r\n", m_ub2ndSDMMC_Class);

    if(MenuSettingConfig()->uiMediaSelect == MEDIA_SETTING_SD_CARD)
    {
        return m_ubSDMMC_Class;
    }
    else if(MenuSettingConfig()->uiMediaSelect == MEDIA_SETTING_SD_CARD1)
    {
        return m_ub2ndSDMMC_Class;
    }
    else
    {
        return 0xFF;
    }

    #else//DEVICE_GPIO_2NDSD_PLUG

    MMPS_FS_GetSDClass(id, &m_ubSDMMC_Class);

    printc("Current SD Card ID%d Class %d\r\n", id, m_ubSDMMC_Class);

    return m_ubSDMMC_Class;

    #endif//DEVICE_GPIO_2NDSD_PLUG
}

UINT8 AHC_SDMMC_Get1stCardClass(UINT32 id)
{
    MMPS_FS_GetSDClass(SD_SOCKET0_MAP_ID, &m_ubSDMMC_Class);

    return m_ubSDMMC_Class;
}

UINT8 AHC_SDMMC_Get2ndCardClass(UINT32 id)
{
    MMPS_FS_GetSDClass(SD_SOCKET1_MAP_ID, &m_ub2ndSDMMC_Class);

    return m_ubSDMMC_Class;
}

#if (SD_UPDATE_FW_EN)
void AHC_SDUpdateMode(void)
{
    SD_UPDATER_ERR eError;

    printc("\n SDUpdate \n");

    AHC_SetMode(AHC_MODE_IDLE);

    eError = SDUpdateCheckFileExisted(SD_FW_UPDATER_BIN_NAME);

    if(eError != SD_UPDATER_ERR_FILE)
    {
        if (eError == SD_UPDATER_ERR_FAIL)
            AHC_OS_Sleep(3000);

        // Update Success
        uiPowerOffPicture();
        AHC_PMU_PowerOff();
    }

    #if(EDOG_ENABLE)
    eError = SDUpdateCheckDBExisted(SD_DB_UPDATER_BIN_NAME);

    if(eError != SD_UPDATER_ERR_FILE)
    {
        if (eError == SD_UPDATER_ERR_FAIL)
            AHC_OS_Sleep(3000);

        // Update Success
        uiPowerOffPicture();
        AHC_PMU_PowerOff();
    }
    #endif
}
#endif


#if 0
void ____FileIO_Function_____(){ruturn;} //dummy
#endif

int strcmpi(char *s1, char *s2)
{
    int     i;

    for (i = 0; *(s1 + i) != '\0' && *(s2 + i) != '\0'; i++)
    {
        if (toupper(*(s1 + i)) != toupper(*(s2 + i)))
            return 1;
    }
    return (int)(*(s1 + i) - *(s2 + i));
}

#if (ENABLE_PIR_MODE)
void AHC_CheckPIRStart(void)
{
    MMP_UBYTE tempValue;

    if(DEVICE_GPIO_PIR_INT != MMP_GPIO_MAX) {

        MMPF_PIO_GetData( DEVICE_GPIO_PIR_INT, &tempValue);

        if(tempValue) {
            gPIRStart = AHC_FALSE;
        }
        else {
            gPIRStart = AHC_TRUE;
        }
    }
    else {
        gPIRStart = AHC_FALSE;
    }
}

void AHC_EnablePIR(AHC_BOOL bEnable)
{
	if(DEVICE_GPIO_PIR_EN != MMP_GPIO_MAX) {
        MMPF_PIO_EnableOutputMode(DEVICE_GPIO_PIR_EN, MMP_TRUE, MMP_FALSE);
        if(bEnable)
            MMPF_PIO_SetData(DEVICE_GPIO_PIR_EN, MMP_FALSE, MMP_FALSE);
        else
            MMPF_PIO_SetData(DEVICE_GPIO_PIR_EN, MMP_TRUE, MMP_FALSE);
    }
}
#endif

AHC_BOOL AHC_IsPIRStart(void)
{
    return gPIRStart;
}

AHC_BOOL AHC_Check2SysCalibMode(void)
{
#if (SUPPORT_SYS_CALIBRATION)

    MMP_UBYTE tempValue1, tempValue2;

    printc("AHC_Check2SysCalibMode\r\n");
    printc("tempValue1=%d\r\n", tempValue1);
    printc("tempValue2=%d\r\n", tempValue2);

    return ((tempValue1==0)&&(tempValue2==0))?(AHC_TRUE):(AHC_FALSE);
#else
    return AHC_FALSE;
#endif
}

AHC_BOOL AHC_Check2SysCalibMode2(void)
{
    UINT32 ulFileId;
    AHC_ERR error = AHC_ERR_NONE;

    if (AHC_IsSDInserted() == AHC_FALSE) {
        printc("Check FactoryModeFile - No SD\r\n");
        return AHC_FALSE;
    }

    error = AHC_FS_FileOpen("SD:\\Test\\FACTORY.CAL",AHC_StrLen("SD:\\Test\\FACTORY.CAL"),"rb",AHC_StrLen("rb"),&ulFileId);
    if (error == AHC_ERR_NONE) {
        AHC_FS_FileClose(ulFileId);
        printc("FactoryModeFile FACTORY.CAL was found in SD\r\n");
        return AHC_TRUE;
    }
    else {
        printc("No FactoryModeFile FACTORY.CAL in SD\r\n");
    }

    return AHC_FALSE;
}

#if (SUPPORT_HDMI_OUT_FOCUS)
AHC_BOOL AHC_Check2HdmiOutFocus(void)
{
    UINT32  ulFileId;
    AHC_ERR error = AHC_ERR_NONE;

    if (AHC_IsSDInserted() == AHC_FALSE) {
        printc("Check HdmiOutFousFile - No SD\r\n");
        return AHC_FALSE;
    }

    error = AHC_FS_FileOpen(CFG_HDMI_OUT_FOCUS_FILE, AHC_StrLen(CFG_HDMI_OUT_FOCUS_FILE), "rb", AHC_StrLen("rb"), &ulFileId);
    if (error == AHC_ERR_NONE) {
        AHC_FS_FileClose(ulFileId);
        printc("HdmiOutFousFile %s was found in SD\r\n", CFG_HDMI_OUT_FOCUS_FILE);
        return AHC_TRUE;
    }
    else {
        printc("No HdmiOutFousFile %s in SD\r\n", CFG_HDMI_OUT_FOCUS_FILE);
    }

    return AHC_FALSE;
}
#endif

#if 0
void ____Motion_Detect_Function_____(){ruturn;} //dummy
#endif

#if (MOTION_DETECTION_EN)
AHC_BOOL AHC_GetMotionDetectionStatus(void)
{
	return m_ubMotionDtcEn;
}

AHC_BOOL AHC_SetMotionDetectionStatus(AHC_BOOL bEn)
{
	m_ubMotionDtcEn = bEn;
	return AHC_TRUE;
}

AHC_BOOL AHC_EnableMotionDetection(AHC_BOOL bEn)
{
    MMPS_Sensor_EnableVMD(bEn);

    return AHC_TRUE;
}

AHC_BOOL AHC_GetMotionDtcSensitivity(UINT8 *pubMvTh, UINT8 *pubCntTh)
{
    *pubMvTh    = m_ubMotionMvTh;
    *pubCntTh   = m_ubMotionCntTh;
    return AHC_TRUE;
}

int AHC_SetMotionDtcSensitivity(unsigned char mvth,  unsigned char cnth)
{
    UINT8 i, j;

    m_ubMotionMvTh  = mvth;
    m_ubMotionCntTh = cnth;

    for( i=0; i<MDTC_WIN_W_DIV; i++ )
    {
        for( j=0; j<MDTC_WIN_H_DIV; j++ )
        {
            gstMdtcWinParam[i][j].sensitivity        = mvth;
            gstMdtcWinParam[i][j].size_perct_thd_min = cnth;
        }
    }

    //MMPS_Sensor_ConfigVMDThreshold(m_ubMotionMvTh, m_ubMotionCntTh, MVD_FRAME_GAP);
    return 2;
}

UINT32 AHC_GetVMDRecTimeLimit(void)
{
#ifdef CAR_DV
    UINT32  m_ulVMDRecTime  = 0;

    switch(MenuSettingConfig()->uiVMDRecTime)
    {
    #if (MENU_MOVIE_VMD_REC_TIME_5SEC_EN)
        case VMD_REC_TIME_5SEC:
            m_ulVMDRecTime = 5;
            break;
    #endif
    #if (MENU_MOVIE_VMD_REC_TIME_10SEC_EN)
        case VMD_REC_TIME_10SEC:
            m_ulVMDRecTime = 10;
            break;
    #endif
    #if (MENU_MOVIE_VMD_REC_TIME_30SEC_EN)
        case VMD_REC_TIME_30SEC:
            m_ulVMDRecTime = 30;
            break;
    #endif
    #if (MENU_MOVIE_VMD_REC_TIME_1MIN_EN)
        case VMD_REC_TIME_1MIN:
            m_ulVMDRecTime = 60;
            break;
    #endif
        default:
            m_ulVMDRecTime = 5;
            break;
    }
#else
    UINT32  m_ulVMDRecTime  = 10;
#endif

    return m_ulVMDRecTime;
}

UINT32 AHC_GetVMDShotNumber(void)
{
    UINT32  m_ulVMDShotNum  = 1;

#ifdef CAR_DV

    switch(MenuSettingConfig()->uiVMDShotNum)
    {
    #if (MENU_STILL_VMD_SHOT_NUM_1P_EN)
        case VMD_SHOT_NUM_1P:
            m_ulVMDShotNum = 1;
            break;
    #endif
    #if (MENU_STILL_VMD_SHOT_NUM_2P_EN)
        case VMD_SHOT_NUM_2P:
            m_ulVMDShotNum = 2;;
            break;
    #endif
    #if (MENU_STILL_VMD_SHOT_NUM_3P_EN)
        case VMD_SHOT_NUM_3P:
            m_ulVMDShotNum = 3;
            break;
    #endif
    #if (MENU_STILL_VMD_SHOT_NUM_5P_EN)
        case VMD_SHOT_NUM_5P:
            m_ulVMDShotNum = 5;
            break;
    #endif
        default:
            m_ulVMDShotNum = 1;
            break;
    }

#endif

    return m_ulVMDShotNum;
}

#endif

void AHC_AdjustVMDPreviewRes(int *w, int *h)
{
    #define VMD_MB_SIZE (16)

    UINT32 ulWidth, ulHeight;

    ulWidth  = *w;
    ulHeight = *h;

#if (TVOUT_PREVIEW_EN) && (MENU_MOVIE_SIZE_VGA30P_EN)
    {
        UINT8 ctv_sys = 0;
        UINT32 movieSize = 0;

        AHC_Menu_SettingGetCB((char *) COMMON_KEY_MOVIE_SIZE, &movieSize);

        if((AHC_Menu_SettingGetCB((char*)COMMON_KEY_TV_SYSTEM, &ctv_sys) == AHC_FALSE)){
            ctv_sys = COMMON_TV_SYSTEM_NTSC; //default value
        }

        if( TVFunc_CheckTVStatus(AHC_TV_VIDEO_PREVIEW_STATUS)   &&
        #if (MENU_MOVIE_SIZE_EN)||(MENU_SINGLE_EN)
            movieSize == MOVIE_SIZE_VGA30P  &&
        #elif (MENU_MOVIE_RESOLUTION_EN)
            MenuSettingConfig()->uiMOVResolution==MOVIE_RESOLUTION_640x480 &&
        #endif
            ctv_sys == COMMON_TV_SYSTEM_NTSC)
        {
            // Original Resotion : ulWidth==720 && ulHeight==540 will be OutOfRange
            ulWidth  = 640;
            ulHeight = 480;
            printc("TVOUT NTSC VGA!!!\r\n");
        }
    }
#endif

    printc("Original Preview W/H for VMD [%d]*[%d]\r\n", ulWidth, ulHeight);

    if( ulWidth%VMD_MB_SIZE != 0)
    {
        ulWidth = ((ulWidth/VMD_MB_SIZE)+1)*VMD_MB_SIZE;
    }

    if( ulHeight%VMD_MB_SIZE != 0)
    {
        ulHeight = ((ulHeight/VMD_MB_SIZE)+1)*VMD_MB_SIZE;
    }

    printc("Adjust Preview W/H for VMD [%d]*[%d]\r\n", ulWidth, ulHeight);

    *w = ulWidth;
    *h = ulHeight;
}

UINT16 AHC_VMDGetMotionCnt(void)
{
    UINT16 x, y;
    UINT16 usTotalObjCnt = 0;

    for (x = 0; x < MDTC_WIN_W_DIV; x++) {
        for (y = 0; y < MDTC_WIN_H_DIV; y++) {
            usTotalObjCnt += gstMdtcWinResult[x][y].obj_cnt;
        }
    }

    return usTotalObjCnt;
}

#if 1 // TBD
UINT8 AHC_VMDGetMotionRatio(void)
{
    UINT16  usTotalObjCnt = 0, usTotalObjTh = 0;
    UINT8   ubRatio = 0;
    UINT16  x, y;

    usTotalObjCnt = AHC_VMDGetMotionCnt();

    if (usTotalObjCnt >= (MDTC_WIDTH * MDTC_HEIGTH))
        return 100;

    for (x = 0; x < MDTC_WIN_W_DIV; x++) {
        for (y = 0; y < MDTC_WIN_H_DIV; y++) {
            usTotalObjTh += gstMdtcWinParam[x][y].size_perct_thd_min;
        }
    }

    if (usTotalObjCnt >= (((MDTC_WIDTH * MDTC_HEIGTH) *  usTotalObjTh) / 100)) {
        ubRatio = 100;
    }
    else {
        if (usTotalObjTh != 0)
            ubRatio = (usTotalObjCnt * 100) / (((MDTC_WIDTH * MDTC_HEIGTH) *  usTotalObjTh) / 100);
        else
            ubRatio = 100;
    }

    #if 0 // for DBG
    printc("usTotalObjCnt : %d\r\n", usTotalObjCnt);

    printc("usTotalObjTh : %d\r\n", usTotalObjTh);

    printc("ubRatio : %d\r\n", ubRatio);
    #endif

    return ubRatio;
}
#else
UINT8 AHC_VMDGetMotionRatio(void)
{
     UINT16  usTotalObjCnt, usTotalObjTh;
     UINT8	 ubRatio;
     UINT16  x, y;

     usTotalObjCnt = AHC_VMDGetMotionCnt();

     if(usTotalObjCnt > MDTC_WIDTH * MDTC_HEIGTH)
     return 100;
     else
     {
		for (x = 0; x < MDTC_WIN_W_DIV; x++) {
         for (y = 0; y < MDTC_WIN_H_DIV; y++) {
             usTotalObjTh += gstMdtcWinParam[x][y].size_perct_thd_min;
         }
     	}
     	if(usTotalObjTh != 0)
		ubRatio = (usTotalObjCnt*100)/usTotalObjTh;
		else
		ubRatio = 100;
     }

     //printc("usTotalObjCnt : %d\r\n",usTotalObjCnt);

     //printc("usTotalObjTh : %d\r\n",usTotalObjTh);

     //printc("ubRatio : %d\r\n",ubRatio);
     return ubRatio;
}
#endif

#if 0
void _____Charger_Control_________(){ruturn;} //dummy
#endif

void AHC_Charger_IO_Initial(void)
{
#ifdef CHARGER_STATUS
    if(CHARGER_STATUS != AHC_PIO_REG_UNKNOWN) {
        AHC_ConfigGPIOPad(CHARGER_STATUS, PAD_NORMAL_TRIG);
        AHC_ConfigGPIO(CHARGER_STATUS, AHC_FALSE);
    }

    AHC_SetTempCtrlEnable(AHC_FALSE);
#endif
}

void AHC_SetChargerEnable(AHC_BOOL bEn)
{
#if defined(ENABLE_CHARGER_IC) && defined(CHARGER_ENABLE_GPIO)
    if(CHARGER_ENABLE_GPIO != AHC_PIO_REG_UNKNOWN)
    {
        AHC_ConfigGPIOPad(CHARGER_ENABLE_GPIO, PAD_OUT_DRIVING(0));
        AHC_ConfigGPIO(CHARGER_ENABLE_GPIO, AHC_TRUE);
        AHC_SetGPIO(CHARGER_ENABLE_GPIO, bEn ? (CHARGER_ENABLE_GPIO_ACT_LEVEL) : !(CHARGER_ENABLE_GPIO_ACT_LEVEL));
    }
#endif
}

AHC_BOOL AHC_GetChargerEnableStatus(void)
{
#if defined(ENABLE_CHARGER_IC) && defined(CHARGER_ENABLE_GPIO)
    AHC_BOOL bReturn = AHC_FALSE;

    if (CHARGER_ENABLE_GPIO != AHC_PIO_REG_UNKNOWN)
    {
        UINT8 gpioState;

        AHC_GetGPIO(CHARGER_ENABLE_GPIO, &gpioState);
        bReturn = ((CHARGER_ENABLE_GPIO_ACT_LEVEL == gpioState) ? AHC_TRUE : AHC_FALSE);
    }

    return bReturn;
#else
    return AHC_FALSE;
#endif
}

void AHC_SetTempCtrlEnable(AHC_BOOL bEn)
{
#if defined(ENABLE_CHARGER_IC) && defined(CHARGER_TEMP_CTL)
    if(CHARGER_TEMP_CTL != AHC_PIO_REG_UNKNOWN)
    {
        AHC_ConfigGPIOPad(CHARGER_TEMP_CTL, PAD_OUT_DRIVING(0));
        AHC_ConfigGPIO(CHARGER_TEMP_CTL, AHC_TRUE);
        AHC_SetGPIO(CHARGER_TEMP_CTL,bEn);
    }
#endif
}

AHC_BOOL AHC_GetTempCtrlStatus(void)
{
#if defined(ENABLE_CHARGER_IC) && defined(CHARGER_TEMP_CTL)
    AHC_BOOL bReturn = AHC_FALSE;

    if(CHARGER_TEMP_CTL != AHC_PIO_REG_UNKNOWN)
    {
        AHC_GetGPIO(CHARGER_TEMP_CTL,&bReturn);
    }

    return bReturn;
#else
    return AHC_FALSE;
#endif
}

#ifdef CAR_DV
AHC_BOOL AHC_GetChargerStatus(void)
{
#if defined(ENABLE_CHARGER_IC) && defined(CHARGER_STATUS)
    if (CHARGER_STATUS != AHC_PIO_REG_UNKNOWN)
    {
        UINT8 state;

        AHC_ConfigGPIOPad(CHARGER_STATUS, PAD_NORMAL_TRIG);
        AHC_ConfigGPIO(CHARGER_STATUS, AHC_FALSE);
        AHC_GetGPIO(CHARGER_STATUS, &state);

        if (CHARGER_STATUS_ACT_LEVEL == state) {
            return AHC_TRUE;
        }
    }
#endif

    if (AHC_IsDcCableConnect())
        return AHC_TRUE;

    if (AHC_IsUsbConnect())
        return AHC_TRUE;

    return AHC_FALSE;
}
#else
AHC_BOOL AHC_GetChargerStatus(void) // for AMY (modified from LW)
{
    UINT8       ubState;
    AHC_BOOL    bReturn = AHC_FALSE;

    #if defined(ENABLE_CHARGER_IC) && defined(CHARGER_STATUS)
    if (CHARGER_STATUS != AHC_PIO_REG_UNKNOWN)
    {
        AHC_ConfigGPIOPad(CHARGER_STATUS, PAD_NORMAL_TRIG);
        AHC_ConfigGPIO(CHARGER_STATUS, AHC_FALSE);
        AHC_GetGPIO(CHARGER_STATUS, &ubState);
    }
    bReturn = ((CHARGER_STATUS_ACT_LEVEL == ubState) ? AHC_TRUE : AHC_FALSE);
    #endif

    if (!AHC_IsUsbConnect())
        bReturn = AHC_FALSE;

    return bReturn;
}
#endif

void AHC_SetVidRecdCardSlowStop(AHC_BOOL bState)
{
    m_VidRecdCardSlowStop = bState;
}

AHC_BOOL AHC_GetVidRecdCardSlowStop(void)
{
    return m_VidRecdCardSlowStop;
}

void AHC_SetAPStopVideoRecord(AHC_BOOL bState)
{
    m_APStopVideoRecord = bState;
}

AHC_BOOL AHC_GetAPStopVideoRecord(void)
{
    return m_APStopVideoRecord;
}

void AHC_SetAPStopVideoRecordTime(ULONG ulLastRecdVidTime, ULONG ulLastRecdAudTime)
{
    m_APStopVideoRecord_VidTime = ulLastRecdVidTime;
    m_APStopVideoRecord_AudTime = ulLastRecdAudTime;
}

AHC_BOOL AHC_GetAPStopVideoRecordTime(ULONG *ulLastRecdVidTime, ULONG *ulLastRecdAudTime)
{
    *ulLastRecdVidTime = m_APStopVideoRecord_VidTime; //UNIT: ms
    *ulLastRecdAudTime = m_APStopVideoRecord_AudTime; //UNIT: ms
    return AHC_TRUE;
}

void AHC_SetEditBrowserMode(EDIT_BROWSER_MODE Mode)
{
    m_EditBrowserMode = Mode;
}

EDIT_BROWSER_MODE AHC_GetEditBrowserMode(void)
{
    return m_EditBrowserMode;
}


AHC_BOOL AHC_ProcessOneFile(AHC_FILE_OP op, PFDrawBarCB pFuncPtr)
{
    UINT32          uiCurrentIdx;
    UINT32          uiMaxCount;

    UINT8           byOP;
    DCF_FILE_PROTECT sType;
    AHC_BOOL    bResult = AHC_TRUE;
    printc("AHC_ProcessOneFile\n");

    AHC_UF_GetCurrentIndex(&uiCurrentIdx);
    AHC_UF_GetTotalFileCount(&uiMaxCount);

    printc("CurrentObjIdx : %d, MaxObj: %d\n",uiCurrentIdx, uiMaxCount);

    if(uiMaxCount == 0)
        return AHC_FALSE;

    switch(op){

    case AHC_FILE_OP_DELETE:
        printc("DELETE ONE\n");
        byOP = DCF_FILE_DELETE;
        break;

    case AHC_FILE_OP_PROTECT:
        printc("PROTECT ONE\n");
        byOP = DCF_FILE_READ_ONLY;
        break;

    case AHC_FILE_OP_UNPROTECT:
        printc("UNPROTECT ONE\n");
        byOP = DCF_FILE_NON_READ_ONLY;
        break;

    default:
        return AHC_FALSE;
        break;
    }

    if(op == AHC_FILE_OP_DELETE){
        AHC_UF_GetFileDeletedMask(&sType);
        AHC_UF_SetFileDeletedMask(DCF_FILE_PROTECT_CHARLOCK|DCF_FILE_PROTECT_READONLY);
    }



    if(AHC_UF_FileOperation_ByIdx(uiCurrentIdx, byOP, NULL, NULL) == AHC_FALSE){
        if(byOP == DCF_FILE_DELETE){
            AHC_WMSG_Draw(AHC_TRUE, WMSG_CANNOT_DELETE, 3);
            AHC_SendAHLMessageEnable(AHC_FALSE);
            AHC_OS_SleepMs(2700);
            AHC_SendAHLMessageEnable(AHC_TRUE);
        }

        bResult = AHC_FALSE;
    }

    AHC_UF_SetFileDeletedMask(sType);

    return bResult;
}

AHC_BOOL AHC_ProcessAllFile(AHC_FILE_OP op, INT8* pchChar, PFDrawBarCB pFuncPtr)
{
    AHC_BOOL        bError = AHC_TRUE;
    UINT32          uiMaxCount, i;

    UINT32          uiCurrentIdx;

    char            FilePathName[MAX_FILE_NAME_SIZE];
    INT8            chAllowedChar[MAX_ALLOWED_WORD_LENGTH];
    UINT8           byOP;
    AHC_BOOL        bFirst = AHC_TRUE;

    DCF_FILE_PROTECT sType;

    printc("AHC_ProcessAllFile \n");

    AHC_UF_GetAllowedChar(chAllowedChar, MAX_ALLOWED_WORD_LENGTH);


    AHC_UF_SetFreeChar(0, DCF_SET_ALLOWED, (UINT8*)pchChar);
    AHC_UF_GetTotalFileCount(&uiMaxCount);


    if(uiMaxCount == 0)
        return bError;

    switch(op){

    case AHC_FILE_OP_DELETE:
        printc("DELETE ALL\n");
        byOP = DCF_FILE_DELETE;
        break;

    case AHC_FILE_OP_PROTECT:
        printc("PROTECT ALL\n");
        byOP = DCF_FILE_READ_ONLY;
        break;

    case AHC_FILE_OP_UNPROTECT:
        printc("UNPROTECT ALL\n");
        byOP = DCF_FILE_NON_READ_ONLY;
        break;

    default:
        return AHC_FALSE;
        break;
    }

    if(op == AHC_FILE_OP_DELETE){
        AHC_UF_GetFileDeletedMask(&sType);
        AHC_UF_SetFileDeletedMask(DCF_FILE_PROTECT_CHARLOCK|DCF_FILE_PROTECT_READONLY);
    }

    AHC_UF_SetCurrentIndex(0);

    MEMSET(FilePathName, 0, sizeof(FilePathName));

    for(i = 0; i < uiMaxCount; i++)
    {
        AHC_UF_GetCurrentIndex(&uiCurrentIdx);

        bError = AHC_UF_GetFilePathNamebyIndex(uiCurrentIdx, FilePathName);

        if(bError == AHC_FALSE) {
            AHC_UF_SetFileDeletedMask(sType);
            AHC_UF_SetFreeChar(0, DCF_SET_ALLOWED, (UINT8*)chAllowedChar);
            return bError;
        }



        if(AHC_UF_FileOperation_ByIdx(uiCurrentIdx, byOP, NULL, NULL) == AHC_FALSE) {
            if(byOP == DCF_FILE_DELETE){
                AHC_WMSG_Draw(AHC_TRUE, WMSG_CANNOT_DELETE, 3);

                if(bFirst) {
                    AHC_SendAHLMessageEnable(AHC_FALSE);
                    AHC_OS_SleepMs(2700);
                    AHC_SendAHLMessageEnable(AHC_TRUE);
                    bFirst = AHC_FALSE;
                }
            }
        }

        uiCurrentIdx++;
        AHC_UF_SetCurrentIndex(uiCurrentIdx);

        if(pFuncPtr != NULL){
            pFuncPtr(i, uiMaxCount, AHC_FALSE);
        }
    }

    if(pFuncPtr != NULL){
        pFuncPtr(i, uiMaxCount, AHC_TRUE);
    }

    AHC_UF_SetFileDeletedMask(sType);
    AHC_UF_SetFreeChar(0, DCF_SET_ALLOWED, (UINT8*)chAllowedChar);
    return AHC_TRUE;
}

#if 0
void _____ParkingMode_Function_________(){ruturn;} //dummy
#endif

#if (C005_PARKING_CLIP_Length_10PRE_10REC)
UINT32 AHC_GetParkiingRecordTimeLimit(void)
{

    return m_uiParkRecordTime;
}
AHC_BOOL AHC_SetParkingRecordTime(UINT32 ulTimeLimit)
{
    m_uiParkRecordTime = ulTimeLimit;
    AHC_SetVRTimeLimit(ulTimeLimit);
    return AHC_TRUE;
}
#endif

#if 0
void ____Buzzer_Function____(){ruturn;} //dummy
#endif

void AHC_PWM_CallBack(MMP_ULONG status)
{
    RTNA_DBG_Str(0, "Buzzer Stop\r\n\r\n");
    RTNA_DBG_PrintLong(0, status);
}

void AHC_BuzzerAlert(UINT32 ulFrquency, UINT32 ulTimes, UINT32 ulMs)
{
#if defined(SUPPORT_BUZZER) && (SUPPORT_BUZZER)
    #if (BUZZER_USING_SW_PWMN)

    #if defined(DEVICE_GPIO_BUZZER) && (DEVICE_GPIO_BUZZER != MMP_GPIO_MAX)
    {
        UINT32	ulPlusTimes = 300;
	
	AHC_ConfigGPIO(DEVICE_GPIO_BUZZER, AHC_TRUE);
	
	while( ulTimes > 0 )
	{
		ulPlusTimes = 100*ulMs;
		while( ulPlusTimes > 0 )
		{
			MMPF_PIO_SetData(DEVICE_GPIO_BUZZER, 0x01, MMP_TRUE);
			RTNA_WAIT_US(121);///110---4.46KHZ///122---3.968Khz////137---3.6KHZ
			MMPF_PIO_SetData(DEVICE_GPIO_BUZZER, 0x00, MMP_TRUE);
			RTNA_WAIT_US(121);
			ulPlusTimes--;
		}
		ulTimes--;
		AHC_OS_SleepMs(60);
	}	

        AHC_OS_SleepMs(150);
    }
    #endif

    #else

    {    
        MMP_BYTE i;

        AHC_PWM_Initialize();

        for(i=0; i<ulTimes; i++) {
            #if defined(DEVICE_GPIO_BUZZER)
            if (DEVICE_GPIO_BUZZER != MMP_GPIO_MAX)
                AHC_PWM_OutputPulse(DEVICE_GPIO_BUZZER, MMP_TRUE, (MMP_ULONG)ulFrquency, MMP_TRUE, MMP_FALSE, NULL, 0xFF);
            #else
                AHC_PWM_OutputPulse(PWM2_PIN_AGPIO3, MMP_TRUE, (MMP_ULONG)ulFrquency, MMP_TRUE, MMP_FALSE, NULL, 0xFF);
            #endif
            //AHC_OS_Sleep(1000);
            //AHC_PWM_OutputPulse(MMPF_PWM2_PIN_AGPIO3, MMP_FALSE, 0x0, MMP_TRUE, MMP_FALSE, NULL, 0x0);

            if(i<(ulTimes-1))
                AHC_OS_SleepMs(ulMs);
        }
    }

    #endif
#endif
}

void AHC_SetFastAeAwbMode( AHC_BOOL bEnable )
{
    if( bEnable == AHC_TRUE )
    {
        AHC_SetAeFastMode( 1 );
        AHC_SetAwbFastMode( 1 );
        m_bEnableFastAeAwb = AHC_TRUE;
    }
    else
    {
        m_bEnableFastAeAwb = AHC_FALSE;
        AHC_SetAeFastMode( 0 );
        AHC_SetAwbFastMode( 0 );
    }
}

AHC_BOOL AHC_GetFastAeAwbMode( void )
{
    return m_bEnableFastAeAwb;
}

void AHC_SetYFrameType( UINT8 byType )
{
    if( byType >= MMPS_3GPRECD_Y_FRAME_TYPE_MAX )
    {
        byType = MMPS_3GPRECD_Y_FRAME_TYPE_NONE;
    }

    printc("AHC_SetYFrameType - %d\r\n", byType);
    MMPS_3GPRECD_SetYFrameType( byType );
}

UINT8 AHC_GetBootingSrc(void)
{
//  PWR_ON_BY_KEY       0x01
//  PWR_ON_BY_VBUS      0x02
//  PWR_ON_BY_GSENSOR   0x04
//  PWR_ON_BY_IR        0x08
//  PWR_ON_BY_DC        0x10
    static MMP_UBYTE bStatus = 0xFF;

    if(bStatus == 0xFF)
    {
        bStatus = *(volatile MMP_UBYTE *) (TEMP_POWER_ON_SRC_INFO_ADDR);
        printc(FG_BLUE("ubPowerOnTriggerSrc = 0x%X\r\n"), *(volatile MMP_UBYTE *) (TEMP_POWER_ON_SRC_INFO_ADDR));
    }

    return bStatus;
}

#if 0
void ____LDWS_Detect_Function_____(){ruturn;} //dummy
#endif

AHC_BOOL AHC_EnableADAS(AHC_BOOL bEn)
{
#if (SUPPORT_ADAS)
    MMP_BOOL bVideoPreview = MMP_FALSE, bDSCPreview = MMP_FALSE;
    MMP_ERR sRet = MMP_ERR_NONE;
    
    printc("### %s - %d\r\n", __func__, bEn);
    sRet = MMPS_3GPRECD_GetPreviewPipeStatus(gsAhcCurrentSensor, &bVideoPreview);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

    sRet = MMPS_DSC_GetPreviewStatus(&bDSCPreview);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}    

    if(bVideoPreview){
        //NOP
    }
    else if(bDSCPreview){
        //TBD
        printc(FG_RED("%s,%d Not support DSC ADAS now!\r\n"),__func__,__LINE__); return AHC_FALSE;
    }
    else{
       AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;
    }   
    
    sRet = MMPS_Sensor_EnableADAS(bEn);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}    
#endif

    return AHC_TRUE;
}

AHC_BOOL AHC_IsADASEnabled( void )
{
#if (SUPPORT_ADAS)
    return (AHC_BOOL)MMPS_Sensor_IsADASEnable();
#endif

    return AHC_FALSE;
}

#if 0
void ____Parking_Function_____(){ruturn;} //dummy
#endif

#if(SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
void uiSetParkingModeStateInit(void)
{
    ParkingModeInfo.bParkingModeConfirmMenu = AHC_FALSE;
    ParkingModeInfo.bParkingMode = AHC_FALSE;
    ParkingModeInfo.bRecordVideo = AHC_FALSE;
    ParkingModeInfo.bParkingIdling = AHC_FALSE;
    ParkingModeInfo.bParkingExitState = AHC_FALSE;
    ParkingModeInfo.bAutoStopParkingMode = AHC_FALSE;
    ParkingModeInfo.bParkingMoveHintMenu = AHC_FALSE;
    ParkingModeInfo.bParkingSpaceState = AHC_FALSE;
    ParkingModeInfo.bParkingStartDrawed = AHC_FALSE;
}
void uiSetParkingAutoStop(AHC_BOOL enable)
{
    ParkingModeInfo.bAutoStopParkingMode = enable;
}

AHC_BOOL uiGetParkingAutoStop(void)
{
    return ParkingModeInfo.bAutoStopParkingMode;
}
void uiSetParkingModeIdling(AHC_BOOL enable)
{
    ParkingModeInfo.bParkingIdling = enable;
    ParkingModeInfo.bParkingMode   = AHC_FALSE;
    ParkingModeInfo.bParkingModeConfirmMenu = AHC_FALSE;
    ParkingModeInfo.bParkingExitState = AHC_FALSE;
    ParkingModeInfo.bParkingMoveHintMenu = AHC_FALSE;
    ParkingModeInfo.bParkingSpaceState = AHC_FALSE;
    ParkingModeInfo.bParkingStartDrawed = AHC_FALSE;
}

AHC_BOOL uiGetParkingModeIdlingEnable(void)
{
    return ParkingModeInfo.bParkingIdling;
}
void uiSetParkingModeEnable(UINT8 enable)
{
    ParkingModeInfo.bParkingIdling = AHC_FALSE;
    ParkingModeInfo.bParkingMode   = enable;
    ParkingModeInfo.bParkingModeConfirmMenu = AHC_FALSE;
    ParkingModeInfo.bParkingExitState = AHC_FALSE;
    ParkingModeInfo.bParkingSpaceState = AHC_FALSE;
    ParkingModeInfo.bParkingMoveHintMenu = AHC_FALSE;
    ParkingModeInfo.bParkingStartDrawed = AHC_FALSE;

#ifdef CAR_DV
    //NOP
#else    
	#if (SUPPORT_PARKINGMODE != PARKINGMODE_STYLE_3)
    if(enable == PARKING_OFF)
    m_bFirstParkingVR = AHC_TRUE;
    ParkingModeResetCounter();
    ParkingModeResetConfirmTime();
    #if (DRIVING_SAFETY_REMIND_HEADLIGHT)
    if (enable == PARKING_OFF) {
        extern AHC_BOOL bRemindHeadlightFirst;

        bRemindHeadlightFirst = AHC_FALSE;
    }
    #endif
#endif
#endif
}

UINT8 uiGetParkingModeEnable(void)
{
    return ParkingModeInfo.bParkingMode;
}

void uiSetParkingModeMenuState(AHC_BOOL enable, AHC_BOOL ParkingStartDrawed)
{
    ParkingModeInfo.bParkingIdling = AHC_FALSE;
    ParkingModeInfo.bParkingMode   = AHC_FALSE;
    ParkingModeInfo.bParkingModeConfirmMenu = enable;
    ParkingModeInfo.bParkingExitState = AHC_FALSE;
    ParkingModeInfo.bParkingSpaceState = AHC_FALSE;
    ParkingModeInfo.bParkingMoveHintMenu = AHC_FALSE;
    ParkingModeInfo.bParkingStartDrawed = ParkingStartDrawed;
}

AHC_BOOL uiGetParkingModeMenuState(void)
{
    return ParkingModeInfo.bParkingModeConfirmMenu;
}

void  uiSetParkingMoveHintState(AHC_BOOL enable, AHC_BOOL ParkingStartDrawed)
{
    ParkingModeInfo.bParkingIdling = AHC_FALSE;
    ParkingModeInfo.bParkingMode   = AHC_FALSE;
    ParkingModeInfo.bParkingModeConfirmMenu = AHC_FALSE;
    ParkingModeInfo.bParkingExitState = AHC_FALSE;
    ParkingModeInfo.bParkingSpaceState = AHC_FALSE;
    ParkingModeInfo.bParkingMoveHintMenu = enable;
    ParkingModeInfo.bParkingStartDrawed = ParkingStartDrawed;
}
AHC_BOOL uiGetParkingMoveHintState(void)
{
    return ParkingModeInfo.bParkingMoveHintMenu;
}

AHC_BOOL uiGetParkingStartDrawed(void)
{
    return ParkingModeInfo.bParkingStartDrawed;
}

void uiSetParkingExitEnable(AHC_BOOL enable)
{
    ParkingModeInfo.bParkingIdling = AHC_FALSE;
    ParkingModeInfo.bParkingMode   = AHC_TRUE;
    ParkingModeInfo.bParkingModeConfirmMenu = AHC_FALSE;
    ParkingModeInfo.bParkingExitState = enable;
    ParkingModeInfo.bParkingMoveHintMenu = AHC_FALSE;
    ParkingModeInfo.bParkingSpaceState = AHC_FALSE;
}
AHC_BOOL uiGetParkingModeExitState(void)
{
    return ParkingModeInfo.bParkingExitState;
}
void uiSetParkingSpaceCheckEnable(AHC_BOOL enable)
{
    ParkingModeInfo.bParkingIdling = AHC_FALSE;
    ParkingModeInfo.bParkingMode   = AHC_FALSE;
    ParkingModeInfo.bParkingModeConfirmMenu = AHC_FALSE;
    ParkingModeInfo.bParkingMoveHintMenu = AHC_FALSE;
    ParkingModeInfo.bParkingExitState = AHC_FALSE;
    ParkingModeInfo.bParkingSpaceState = enable;
}
AHC_BOOL uiGetParkingSpaceCheckState(void)
{
    return  ParkingModeInfo.bParkingSpaceState;
}

void uiSetParkingModeRecordState(AHC_BOOL enable)
{
    ParkingModeInfo.bRecordVideo = enable;
}

AHC_BOOL uiGetParkingModeRecordState(void)
{
    return ParkingModeInfo.bRecordVideo;
}
void uiSetParkingModeRecord(AHC_BOOL recorded)
{
    ParkingTriggerRecord = recorded;
}

AHC_BOOL uiGetParkingModeRecord(void)
{
    return ParkingTriggerRecord;
}
#else
UINT8 uiGetParkingModeEnable(void)
{
    return AHC_FALSE;
}
AHC_BOOL uiGetParkingModeMenuState(void)
{
    return AHC_FALSE;
}

AHC_BOOL uiGetParkingModeRecordState(void)
{
    return AHC_FALSE;
}
#endif

#if 0
void ____NFC_Function____(){ruturn;} //dummy
#endif

/*
*if sucessed return 0
*ow return -1
*/
int AHC_WriteSSIDPW2NFC(char * pszSSID ,char * pszPassword)
{
#if 0   // TBD
    #define NFC_WRITE_RETRY 3

    int dwRetryCtr = 0;

    if(!pszSSID || !pszPassword)
        return -1;

    do{
        if(M24SR_Reset() == MMP_ERR_NONE){
            if(M24SR_I2C_SUCCESS == M24SR_WriteWiFiData( (MMP_UBYTE *)pszSSID, strlen(pszSSID), (MMP_UBYTE *)pszPassword, strlen(pszPassword) )){
                return 0;
            }
            else{
                dwRetryCtr++;
            }
        }
        else{
            dwRetryCtr++;
        }
    }while(dwRetryCtr < NFC_WRITE_RETRY);
#else
    printc("--I-- %s is TBD\r\n", __func__);
#endif

    return -1;
}

void AHC_DumpRegister(ULONG startAddress, ULONG bytes)
{
    static MMP_ULONG preTime = 0;
    MMP_ULONG curTime;
	ULONG Register;

	RTNA_DBG_Str(0, "\r\n\r\n=====================================Register Dump Start\r\n");
    MMPF_OS_GetTime(&curTime);

    if (curTime >= preTime) {
        curTime -= preTime;
    }
    else {
        curTime += ((MMP_ULONG) -1) - preTime + 1;
    }

    if (curTime < 5000) {
        return;
    }

    MMPF_OS_GetTime(&preTime);

	for (Register = 0; Register < bytes; Register += 4) {
        if ((Register % 16) == 0) {
            RTNA_DBG_Str(0, "\r\n");
            RTNA_DBG_Long(0, startAddress + Register);
            RTNA_DBG_Str(0, ": ");
        }

        RTNA_DBG_Long(0, *(AIT_REG_D *) (startAddress + Register));
	}

	RTNA_DBG_Str(0, "\r\n========================================Register Dump End\r\n\r\n");
}

#if 0
void ____WIFI_Function____(){ruturn;} //dummy
#endif

/***********************************************************************************************/
//		function: AHC_Get_WiFi_Streaming_Status()
//	 description: To read whether WiFi is streaming or not..
//		output	: True stands for Streaming.
/***********************************************************************************************/
AHC_BOOL AHC_Get_WiFi_Streaming_Status()
{
#if defined(WIFI_PORT) && (WIFI_PORT == 1)
    return get_NetAppStatus() & NETAPP_STREAM_PLAY;
#else
    return AHC_FALSE;
#endif
}

void AHC_WiFi_Toggle_StreamingMode(void)
{
#if defined(WIFI_PORT) && (WIFI_PORT == 1)
	short	en;
	printc(BG_GREEN("%s %d\r\n"), __func__, __LINE__);

    if(!AHC_Get_WiFi_Streaming_Status() && AHC_GetStreamingMode() == AHC_STREAM_OFF)
        en = 1;
    else
        en = 0;
    NetApp_SetStreaming(en);
#endif
}

ULONG AHC_CheckWiFiOnOffInterval(MMP_ULONG waiting_time)
{
#if defined(WIFI_PORT) && (WIFI_PORT == 1)
    MMP_ULONG ulNow;
    MMP_ULONG ulExpend;

    MMPF_OS_GetTime(&ulNow);

    if (-1 != ulLastWiFiChagneTime) {
        if (ulNow >= ulLastWiFiChagneTime) {
            ulExpend = TICK_TO_MS(ulNow - ulLastWiFiChagneTime);
        }
        else {
            ulExpend = TICK_TO_MS(((MMP_ULONG) (-1)) - ulLastWiFiChagneTime + ulNow);
        }

        if (ulExpend < waiting_time) {
            return (waiting_time - ulExpend);
        }
    }
#endif

    return 0;
}

AHC_BOOL AHC_WiFi_Switch(AHC_BOOL wifi_switch_on)
{
#if defined(WIFI_PORT) && (WIFI_PORT == 1)
    #undef WIFI_ON_DELAY_TIME
    #undef WIFI_OFF_DELAY_TIME
    #define WIFI_ON_DELAY_TIME                  (4000)      // ms
    #define WIFI_OFF_DELAY_TIME                 (4000)      // ms

    UINT32 ulWiFiFlag = WIFI_MODE_OFF;
	AHC_BOOL wifi_sw;

	wifi_sw = wifi_switch_on;
	
    if (NETAPP_WIFI_FAIL == get_NetAppStatus())
        return AHC_FALSE;

    if (NETAPP_WIFI_INIT == get_NetAppStatus()) {
        MMPF_OS_GetTime(&ulLastWiFiChagneTime);
        return AHC_TRUE;
    }

    AHC_Menu_SettingGetCB((char*) COMMON_KEY_WIFI_EN, &ulWiFiFlag);

	if (WIFI_MODE_OFF == ulWiFiFlag) {
	    wifi_sw = AHC_FALSE;
	}

	if (AHC_TRUE == wifi_switch_on) {
        if ((NETAPP_WIFI_NONE == get_NetAppStatus()) ||
            (NETAPP_WIFI_DOWN == get_NetAppStatus())) {
            if (AHC_CheckWiFiOnOffInterval(WIFI_OFF_DELAY_TIME)) {
                return AHC_FALSE;
            }

            RTNA_DBG_Str0(BG_GREEN("\r\nnetapp_InitNetwork\r\n"));
        	printc(FG_GREEN("%s: get_NetAppStatus = 0x%X, %d\r\n"), __func__, get_NetAppStatus(), wifi_sw);

            netapp_InitNetwork();
            MMPF_OS_GetTime(&ulLastWiFiChagneTime);
        }
	}
	else {
        if (WLAN_SYS_GetMode() != -1) {
            if (AHC_Get_WiFi_Streaming_Status()){
                RTNA_DBG_Str0(BG_GREEN("\r\nNetApp_SetStreaming off\r\n"));
                NetApp_SetStreaming(0);
            }

            if (NETAPP_WIFI_DOWN != get_NetAppStatus()) {
                RTNA_DBG_Str0(BG_GREEN("\r\nnetapp_StopNetwork\r\n"));
            	printc(FG_GREEN("%s: get_NetAppStatus = 0x%X, %d\r\n"), __func__, get_NetAppStatus(), wifi_sw);

                netapp_StopNetwork();
                MMPF_OS_GetTime(&ulLastWiFiChagneTime);
            }
        }
	}
#endif

    return AHC_TRUE;
}

void AHC_RestartPreviewAsRecordingSetting( AHC_BOOL bEnable )
{
    bRestartPreviewAsRecordingSetting = bEnable;
}

#if CLOCK_SETTING_TIME_LIMIT
void AHC_SetClockSettingTimeLimit(INT32 ms)
{
	m_ClockSettingTimeLimit = ms;
}

INT32 AHC_GetClockSettingTimeLimit(void)
{
	return m_ClockSettingTimeLimit;
}

void AHC_ResetClockSettingTimeLimit(void)
{ 
	m_ClockSettingTimeLimit = CLOCK_SETTING_TIME_LIMIT;
}
#endif

#if defined(AIT_HW_WATCHDOG_ENABLE) && (AIT_HW_WATCHDOG_ENABLE)
void AHC_WD_Enable(AHC_BOOL bEnable)
{
    MMP_ERR sRet = MMP_ERR_NONE;
    
    printc("%s,%d,%d\r\n", __func__, __LINE__, bEnable);        

    if(bEnable){
        MMP_ULONG clk_div,CLK_DIV[] = {1024, 128, 32 , 8 } ;
        MMP_ULONG ms;
        MMP_ULONG g0_slow /*,rst_c_1s*/,rst_c,c ;

        ms = 4000;
        
        MMPF_PLL_GetGroupFreq(0,&g0_slow );
        g0_slow = g0_slow / 2 ;
        c = 0 ;

        printc("ms : %d, G0 slow:%d\r\n",ms, g0_slow);        

        do {
            clk_div = CLK_DIV[c] ;
            //rst_c_1s =  (g0_slow * 1000) / (  clk_div * 16384 ) ; // 1 secs
            //rst_c = (rst_c_1s * ms ) / 1000 ;
            rst_c = (g0_slow * ms) / (clk_div * 16384);
            printc("rst_c : %d,DIV:%d\r\n",rst_c, clk_div);
            c++ ;
            if(c>=3) {
                printc("%s,%d parameters error!\r\n", __func__, __LINE__);
                break ;
            }
        }
        while ( (rst_c > 31) || (!rst_c) ) ;
                       
        sRet = MMPF_WD_SetTimeOut(rst_c, clk_div);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return;}    

        sRet = MMPF_WD_EnableWD(MMP_TRUE,MMP_TRUE,MMP_FALSE,0,MMP_TRUE); 
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return;}    

        sRet = MMPF_WD_SW_EnableWD(MMP_TRUE, AIT_HW_WATCHDOG_TIMEOUT);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return;}    
    }
    else{
        sRet = MMPF_WD_EnableWD(MMP_FALSE,MMP_FALSE,MMP_FALSE,0,MMP_FALSE);         
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return;}    

        sRet = MMPF_WD_SW_EnableWD(MMP_FALSE, 0);        
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return;}    
    }              
}

void AHC_WD_Kick(void)
{
    MMPF_WD_SW_Kick();
}
#endif

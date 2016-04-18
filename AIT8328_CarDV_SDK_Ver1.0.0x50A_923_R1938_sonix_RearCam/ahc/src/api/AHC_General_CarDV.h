
#ifndef _AHC_GENERAL_CARDV_H_
#define _AHC_GENERAL_CARDV_H_

/*===========================================================================
 * Include file 
 *===========================================================================*/ 

#include "AHC_Common.h"
#include "AHC_General.h"
#include "AHC_WarningMsg.h"
#include "AHC_Isp.h"
#include "mmps_display.h"
#include "ctype.h"
#include "ait_bsp.h"
#include "GUI_Type.h"
#if defined(_OEM_) && defined(ALL_FW)
#include "Oem.h"
#endif

/*===========================================================================
 * Macro definition
 *===========================================================================*/ 

/* For Video Lock File */
#define DEFAULT_LOCKED_FILENAME			"SOS_0000.AVI"

/* For Error Dialog */
#define	MSG_PRESENT						(1)
#define	MSG_DISMISS						(0)
#define	ERROR_DIALOG_PRESENT(state)		(state == MSG_PRESENT)

/* For RTC Use Case */
#define RTC_USE_CASE_STAMP 				(0)
#define RTC_USE_CASE_PANEL 				(1)
#define MAX_RTC_USE_CASE				(2)

/* For RTC Default Time */
#ifndef RTC_DEFAULT_YEAR //may be defined in config_xxx.h
#define RTC_DEFAULT_YEAR				(2015)
#endif
#ifndef RTC_DEFAULT_MONTH //may be defined in config_xxx.h
#define RTC_DEFAULT_MONTH				(1)
#endif
#ifndef RTC_DEFAULT_DAY //may be defined in config_xxx.h
#define RTC_DEFAULT_DAY					(1)
#endif

#ifndef RTC_DEFAULT_HOUR 
#define RTC_DEFAULT_HOUR				(8)
#endif
#ifndef RTC_DEFAULT_MIN
#define RTC_DEFAULT_MIN					(0)
#endif
#ifndef RTC_DEFAULT_SEC
#define RTC_DEFAULT_SEC					(0)
#endif

#ifndef RTC_MAX_YEAR
#define RTC_MAX_YEAR					(2089)
#endif
#ifndef RTC_MIN_YEAR
#define RTC_MIN_YEAR					(RTC_DEFAULT_YEAR)
#endif
#define RTC_MAX_MONTH					(12)
#define RTC_MIN_MONTH					(1)
#define RTC_MAX_DAY_31					(31)
#define RTC_MAX_DAY_30					(30)
#define RTC_MAX_DAY_FEB_LEAP_YEAR		(29)
#define RTC_MAX_DAY_FEB_NONLEAP_YEAR	(28)
#define RTC_MIN_DAY						(1)
#define RTC_MAX_HOUR					(23)
#define RTC_MIN_HOUR					(0)
#define RTC_MAX_MIN						(59)
#define RTC_MIN_MIN						(0)
#define RTC_MAX_SEC						(59)
#define RTC_MIN_SEC						(0)

/* For Clock Setting : YYYY/MM/DD HH/MM/SS */
#define IDX_YEAR						(0)
#define IDX_MONTH						(1)
#define IDX_DAY							(2)
#define IDX_HOUR						(3)
#define IDX_MIN							(4)
#define IDX_SEC							(5)
#define CHECK_PASS						(0xFF)

#define CHECK_YEAR						(0x01)
#define CHECK_MONTH						(0x02)
#define CHECK_DAY						(0x04)
#define CHECK_HOUR						(0x08)
#define CHECK_MIN						(0x10)
#define CHECK_SEC						(0x20)
#define CHECK_ALL						(CHECK_YEAR | CHECK_MONTH | CHECK_DAY | CHECK_HOUR | CHECK_MIN | CHECK_SEC)

/* For USB String */
#define	VEN_LEN							(8)
#define	POD_LEN							(16)
#define	DESC_OFF						(8)

/* For Movie Playback Status */
#define MOVIE_STATE_PAUSE				(0)
#define MOVIE_STATE_PLAY				(1)
#define MOVIE_STATE_FF					(2)
#define MOVIE_STATE_REW					(3)
#define MOVIE_STATE_STOP				(4)
#define MOVIE_STATE_PFF					(5)
#define MOVIE_STATE_PFB					(6)

/* For Audio Playback Status */
#define AUDIO_STATE_PAUSE				(0)
#define AUDIO_STATE_PLAY				(1)
#define AUDIO_STATE_FF					(2)
#define AUDIO_STATE_REW					(3)
#define AUDIO_STATE_STOP				(4)
#define AUDIO_STATE_PFF					(5)
#define AUDIO_STATE_PFB					(6)

/*===========================================================================
 * Structure definition
 *===========================================================================*/ 

typedef	void (*TMHOOK)(void);

typedef struct 
{
	const unsigned char		**strpool;
	const GUI_FONT			*fontmap;
} OSD_LANGSET;

/*===========================================================================
 * Enum definition
 *===========================================================================*/ 

typedef enum{

	PLAY_FIRST_FILE = 0,
	PLAY_CUR_FILE,
	PLAY_LAST_FILE
} PLAY_FILE_ORDER;

typedef enum{

	SRC_MENU = 0,
	SRC_KEY,
	SRC_HALL_SNR,
	SRC_G_SNR,
	SRC_MAX
} ROTATE_SRC;

typedef enum{

    EDIT_OP_DELETE = 0,
   	EDIT_OP_PROTECT,
   	EDIT_OP_UNPROTECT
}EDIT_OP;

typedef enum{

    BOOTUP_CHARGER = 0,
   	BOOTUP_USB,
   	BOOTUP_MANUAL,
   	BOOTUP_NONE
}BOOTUP_SELECT;

typedef enum{

   	FILE_PHOTO = 0,
	FILE_MOVIE,
   	FILE_PHOTO_MOVIE
}SEARCH_FILE_TYPE;

typedef enum{

    BROWSER_UP = 0,
    BROWSER_DOWN,
    BROWSER_LEFT,
    BROWSER_RIGHT,
	BROWSER_PLAY,
	BROWSER_PAGEUP,
	BROWSER_PAGEDOWN,
	BROWSER_HDMI,
	BROWSER_TVOUT
}BROWSER_SELECT;

typedef enum{

   	UP = 0,
	DOWN,
   	LEFT,
   	RIGHT
}DIRECTION;

extern TMHOOK	gTmKeypadHook;

/*===========================================================================
 * Function prototype
 *===========================================================================*/ 

/* Language */
UINT8 			ShowOSD_GetLanguageEx(void);
void 			ShowOSD_SetLanguageEx(UINT8 id, UINT8 nlang);
/* Motion Detection */
AHC_BOOL 		AHC_EnableMotionDetection(AHC_BOOL bEn);
AHC_BOOL 		AHC_GetMotionDtcSensitivity(UINT8 *pubMvTh, UINT8 *pubCntTh);
UINT32 			AHC_GetVMDRecTimeLimit(void);
UINT32 			AHC_GetVMDShotNumber(void);
void 			AHC_AdjustVMDPreviewRes(int *w, int *h);
/* Timer Hook */
void 			AHC_TmKeypadHook(void *cb);
/* RTC Timer */
AHC_BOOL 		AHC_Validate_Year(UINT16 year);
UINT8			AHC_Validate_ClockSetting(INT32 *pDatetime, UINT8 ubCheckType, AHC_BOOL bAutoRestore);
AHC_BOOL        AHC_RestoreDefaultTime(UINT16 *py, UINT16 *pm, UINT16 *pd, UINT16 *pd_in_week,UINT16 *ph, UINT16 *pmm, UINT16 *ps, UINT8 *pam_pm, UINT8 *p12format_en);
/* Video Record */
UINT32 			AHC_GetVideoRealFrameRate(UINT32 Frate);
UINT32 			AHC_GetVideoPowerOffTime(void);
UINT32 			AHC_GetVideoMaxLockFileTime(void);
INT8* 			AHC_GetLockedFileName(UINT8 bSelect);
AHC_BOOL 		AHC_SetMediaErrorLossTime(UINT32 ulTime);
AHC_BOOL 		AHC_SetFSClearCachePeriod(UINT32 ulBitrate);
UINT32 			AHC_GetPreRecordTime(void);
void 			AHC_SetShutdownByChargerOut(AHC_BOOL bEnable);
AHC_BOOL 		AHC_GetShutdownByChargerOut(void);
void 			AHC_SetRecordByChargerIn(UINT8 ubDelay);
UINT8 			AHC_GetRecordByChargerIn(void);
AHC_BOOL		AHC_GetAutoRec(void);
const char *    AHC_GetVideoExt(void);
AHC_BOOL        AHC_IsCurFileCharLock(void);

/* DSC */
UINT32 			AHC_GetTimeLapseInterval(void);
/* Display */
AHC_BOOL 		AHC_PreviewOnDisplayEx(AHC_BOOL bOn);
void 			AHC_SetCurrentDisplayEx(MMP_DISPLAY_OUTPUT_SEL kind);
/* TV */
void 			AHC_SetTVBooting(AHC_BOOL ubBoot);
int 			AHC_IsTVBooting(void);
void 			AHC_InitTV(AHC_BOOL inited);
int 			AHC_IsTVInited(void);
void 			AHC_SwitchLCDandTVOUT(void);
/* LCD */
void 			AHC_InitLCD(AHC_BOOL inited);
void 			AHC_OpenLCD(void);
void 			AHC_CloseLCD(void);
AHC_BOOL 		AHC_SwitchLCDBackLight(void);
AHC_BOOL 		AHC_GetFlipSelection(UINT8 ubCtrl,UINT8* pSelection,UINT8* pbLinkMenu);
AHC_BOOL 		AHC_CheckOSDFlipEn(UINT8 ubCheck);
AHC_BOOL 		AHC_CheckSNRFlipEn(UINT8 ubCheck);
AHC_BOOL 		AHC_CheckLinkWithMenu(UINT8 ubCheck);
void	 		AHC_SetSensorStatus(UINT8 ubSNRState);
UINT8 			AHC_GetSensorStatus(void);
void 			AHC_SetRotateSrc(UINT8 src);
UINT8 			AHC_GetRotateSrc(void);
AHC_BOOL 		AHC_LinkLCDStatusToPara(UINT32 wValue);
AHC_BOOL 		AHC_DrawRotateEvent(UINT32 Event);
AHC_BOOL 		AHC_SetKitDirection(UINT8 LCDdir, AHC_BOOL bSetLCD, UINT8 SNRdir, AHC_BOOL bSetSnr);
AHC_BOOL 		AHC_PreSetLCDDirection(void);
/* Menu Setting */
AHC_BOOL 		Menu_SetFlashLEDMode(UINT8 val);
AHC_BOOL 		Menu_SetLanguage(UINT8 val);
AHC_BOOL 		Menu_SetFlickerHz(UINT8 val);
AHC_BOOL 		Menu_SetSceneMode(UINT8 val);
AHC_BOOL 		Menu_SetEV(UINT8 val);
AHC_BOOL 		Menu_SetISO(UINT8 val);
AHC_BOOL 		Menu_SetAWB(UINT8 val);
AHC_BOOL 		Menu_SetColor(UINT8 val);
AHC_BOOL 		Menu_SetEffect(UINT8 val);
AHC_BOOL 		Menu_SetMovieMode(UINT8 val);
AHC_BOOL        Menu_SetMovieMenuResoution(UINT32 *w, UINT32 *h, UINT32 *fps);
AHC_BOOL 		Menu_SetMovieQuality(UINT8 val);
AHC_BOOL 		Menu_SetMICSensitivity(UINT8 val);
AHC_BOOL 		Menu_SetVolume(UINT8 val);
AHC_BOOL 		Menu_SetSoundRecord(UINT8 val);
AHC_BOOL 		Menu_SetLCDDirectionEx(UINT8 val,UINT8 backupVal);
AHC_BOOL 		Menu_SetGsensorSensitivity(UINT8 val);
AHC_BOOL 		Menu_RenewMenuSetting(UINT8 bReset);
AHC_BOOL 		Menu_WriteLcdSnrTVHdmiStauts2SF(void);
AHC_BOOL 		Menu_WriteSetting(void);
AHC_BOOL 		AHC_ChangeEV(AHC_BOOL bCycle,UINT8 bDir);
AHC_BOOL 		AHC_ChangeVolume(AHC_BOOL bCycle,UINT8 bDir);
AHC_BOOL 		AHC_ToggleMute(void);
AHC_BOOL		AHC_ToggleTwilightMode(void);
AHC_AE_EV_BIAS 	Menu_EV_To_AE_EV_BIAS(UINT8 val);
AHC_BOOL        Menu_SetContrast(INT32 val);
AHC_BOOL        Menu_SetSaturation(INT32 val);
AHC_BOOL        Menu_SetSharpness(INT32 val);
AHC_BOOL        Menu_SetGamma(INT32 val);

/* LED */
void 			AHC_SetMenuFlashLEDStatus(UINT8 ubLed);
UINT8 			AHC_GetMenuFlashLEDStatus(void);
AHC_BOOL 		AHC_ToggleFlashLED(UINT8 ubMode);
void 			AHC_PreSetFlashLED(void);
/* Misc */
AHC_BOOL 		AHC_PowerOn_Welcome(void);
AHC_BOOL 		AHC_GetDramEndAddress(UINT32 *pval);
AHC_BOOL 		AHC_ShowErrorDialog(WMSG_INFO iInfo);
AHC_BOOL 		AHC_CheckErrorDialog(UINT32 ulEvent);
AHC_BOOL 		AHC_IsDialogPresent(void);
AHC_BOOL 		AHC_ShowSoundWarning(void);
AHC_BOOL 		AHC_CheckCurSysMode(UINT16 CheckMode);
AHC_BOOL 		AHC_PlayMediaFiles(UINT8 bType);
AHC_BOOL 		AHC_CheckCurFileLockStatus(void);
AHC_BOOL 		AHC_SetPlayFileOrder(UINT8 ubOrder);
UINT8 			AHC_GetProjectID(void);
UINT32 			AHC_GetMappingFileNum(UINT8 ubFileType);
AHC_BOOL 		AHC_SetBootUpUIMode(void);
AHC_BOOL		AHC_ResetVideoPlayParam(void);
/* FileIO */
int 			AHC_ReadlLine(UINT32 fd, char* buf, int size);
int 			AHC_ReadLineMem(char *fbuf, int* fp, int fs, char* outl, int size);
/* SD MMC */
AHC_BOOL 		AHC_SDMMC_BasicCheck(void);
UINT32 			AHC_SDMMC_GetLockFileNum(void);
/* Power-On Option */
AHC_BOOL 		AHC_SetButtonStatus(UINT32 keyId);
AHC_BOOL 		AHC_PowerOnOption(void);
INT32			AHC_GetQuickKey(int qk);
/* USB */
void 			AHC_CheckUSB_KeyFile(char *kf, int app);
AHC_BOOL 		AHC_SetUSBLable(char *pszLable);
/* EXIF */
//AHC_BOOL 		AHC_SetExifInfoEx(char *pszMaker, char *pszModel, char *pszSw, char *pszArtist, char *pszCr);
//void 			AHC_InitExifInfo(void);
/* Buzzer */
//void 			AHC_BuzzerAlert(UINT32 ulTimes, UINT32 ulMs);
void 			AHC_BuzzerDisAlert(void);
/* Vibration */
AHC_BOOL AHC_Vibration_Enable(UINT32 ulMs);
AHC_BOOL AHC_Vibration_Disable(void);

//MenuStateEditToolMenu.c
AHC_BOOL 		DrawFileOperationResult(UINT8 status);
UINT8 			QuickFileOperation(UINT8 Op);
AHC_BOOL 		QuickMediaOperation(UINT8 Op);

/* Others */
extern int 		sprintf(char * /*s*/, const char * /*format*/, ...);
extern int 		snprintf(char * /*s*/, size_t /*n*/, const char * /*format*/, ...);
int				strcmpi(char *s1, char *s2);

/* Pause Keypad input to protect some procedures that takes long time to run */
int 			AHC_PauseKeyUI(void);
int 			AHC_ResumeKeyUI(void);
int 			AHC_KeyUIWorking(void);

/*
 * When Kit is busy to do something (ex. Sequential Shotting)
 * to call AHC_Device_Busy to make polling (ex. GUI charging battery icon)
 * NOT to send event to update it, otherwise the same events will be queue
 * then to many time in short time after job finished
 * *NOTE*: AHC_Device_Busy() and AHC_Device_Idle() must be matched
 */
int AHC_Device_Busy(void);
int AHC_Device_Idle(void);
int AHC_Device_IsBusy(void);

/*Wi-fi*/
//AHC_BOOL Menu_SetWiFiMode(UINT8 val);
void AHC_WiFi_Switch_Mode(void);
AHC_BOOL AHC_Get_WiFi_Streaming_Status(void);
AHC_BOOL AHC_Gsensor_SetPowerOnIntThreshold(void);

#if(SUPPORT_GSENSOR && POWER_ON_BY_GSENSOR_EN)
UINT32 AHC_GSNR_PWROn_MovieTimeReset(void);
#endif

#endif//_AHC_GENERAL_CARDV_H_

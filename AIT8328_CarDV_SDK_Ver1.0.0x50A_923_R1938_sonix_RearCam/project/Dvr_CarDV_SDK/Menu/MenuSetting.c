/*===========================================================================
 * Include file
 *===========================================================================*/

#include "customer_config.h"
#include "stdlib.h"
#include "ait_utility.h"
#include "AHC_Common.h"
#include "AHC_OS.h"
#include "AHC_Parameter.h"
#include "AHC_General.h"
#include "AHC_Browser.h"
#include "AHC_Audio.h"
#include "AHC_Media.h"
#include "AHC_General_CarDV.h"
#include "AHC_Fs.h"
#include "AHC_Utility.h"
#include "AHC_Menu.h"
#include "AHC_Version.h"
#include "LedControl.h"
#include "MenuSetting.h"
#include "ShowOSDFunc.h"
#include "StateVideoFunc.h"
#include "StateCameraFunc.h"
#include "mmps_3gprecd.h"
#include "MenuDrawingFunc.h"

#ifndef __AMN_OS_LINUX__
#if defined(WIFI_PORT) && (WIFI_PORT == 1)
#include "amn_sysobjs.h"
#include "netapp.h"
#include "wlan.h"
#endif
#endif

#if(MIMF_CUSTOMIZED_SORT_TYPE)
#include "ahc_mimf.h"
#endif

/*===========================================================================
 * Macro define
 *===========================================================================*/

#define MENUSETTING_DEBUG 	(0)

#if (MENUSETTING_DEBUG==1)
#define mPRINT( ... ) 		printc(  __VA_ARGS__ )
#else
#define mPRINT( ... )
#endif

#define	MAX_CHAR_LINE		(80)
#define	IS_LINE_ENDING(c)	((c) == 0x0a || (c) == 0x0d)

#define DRIVER_ID_KEY				COMMON_KEY_DRIVER_ID
#define GSNR_PWRON_SENS_KEY			COMMON_KEY_GSENSOR_PWR_ON_SENS
#define GSNR_SENS_KEY    			COMMON_KEY_GSENSOR_SENS
#define WIFI_KEY					COMMON_KEY_WIFI_EN
#define TIME_STAMP_LOGO_TXT_KEY		COMMON_KEY_STICKER_LOGO_TXT
#define VIDEO_REC_TIMELAPSE_KEY		COMMON_KEY_VR_TIMELAPSE	// VIDEO_REC_TIMELAPSE_EN
#define LDWS_EN_KEY					COMMON_KEY_LDWS_EN
#define FCWS_EN_KEY					COMMON_KEY_FCWS_EN
#define SAG_EN_KEY					COMMON_KEY_SAG_EN
#define STILL_QUALITY_KEY           COMMON_KEY_STILL_QUALITY
#define TV_SYSTEM_KEY               COMMON_KEY_TV_SYSTEM
#define HDR_EN_KEY					COMMON_KEY_HDR_EN
#define MOTION_SENS                 COMMON_KEY_MOTION_SENS
#define FLICKER                     COMMON_KEY_FLICKER
#define CONTRAST_KEY                COMMON_KEY_CONTRAST
#define SATURATION_KEY              COMMON_KEY_SATURATION
#define SHARPNESS_KEY               COMMON_KEY_SHARPNESS
#define GAMMA_KEY                   COMMON_KEY_GAMMA
#define SLOWMOTION_EN_KEY			COMMON_KEY_SLOWMOTION_EN
#define WNR_EN_KEY			        COMMON_KEY_WNR_EN

/*===========================================================================
 * Local function
 *===========================================================================*/

static void DriverIDReset(void);

/*===========================================================================
 * Local varible
 *===========================================================================*/

static SystemInfo gSystemInfo = {0};
static MenuInfo gCurMenuInfo = {0};
static MenuInfo gTempMenuInfo = {0};
static MenuInfo gDefaultMenuInfo;
static UINT8    uiOpMode;
const  char     model[CUSTOMER_NAME_MAX_LENGTH] = {CUSTOMER_NAME};

/*===========================================================================
 * Global varible
 *===========================================================================*/
UINT8    uiCycleRecord;

/*===========================================================================
 * Main body
 *===========================================================================*/

void MenuSettingInit(void)
{
    gCurMenuInfo.uiIMGSize          	= IMAGE_SIZE_3M;//IMAGE_SIZE_5M;
    gCurMenuInfo.uiIMGQuality       	= QUALITY_SUPER_FINE;
    gCurMenuInfo.uiIMGStabilize     	= IMAGE_STABILIZE_OFF;
    gCurMenuInfo.uiFlashMode        	= FLASH_OFF;
    gCurMenuInfo.uiFlashLevel       	= FLASHLEVEL_NORMAL;
    gCurMenuInfo.uiRedEyeReduce     	= REDEYE_REDUCE_OFF;
    gCurMenuInfo.uiSelfTimer       		= SELF_TIMER_OFF;
    gCurMenuInfo.uiBurstShot        	= BURST_SHOT_OFF;
    gCurMenuInfo.uiVMDShotNum        	= VMD_SHOT_NUM_1P;
    gCurMenuInfo.uiTimeLapseTime   		= TIMELAPSE_INTERVAL_DEFAULT;

    gCurMenuInfo.uiMOVSize          	= MOVIE_SIZE_1080_30P;
    gCurMenuInfo.uiMOVQuality       	= QUALITY_SUPER_FINE;
    gCurMenuInfo.uiMOVStabilize     	= IMAGE_STABILIZE_OFF;
    gCurMenuInfo.uiSlowShutter      	= SLOW_SHUTTER_OFF;
    gCurMenuInfo.uiMicSensitivity   	= MIC_SEN_STANDARD;
	gCurMenuInfo.uiMOVPreRecord			= MOVIE_PRE_RECORD_OFF;
#ifdef CFG_MENU_DEFAULT_MOV_CLIP_TIME //may be defined in config_xxx.h, e.g. MOVIE_CLIP_TIME_OFF
   	gCurMenuInfo.uiMOVClipTime   		= CFG_MENU_DEFAULT_MOV_CLIP_TIME;
#else
   	gCurMenuInfo.uiMOVClipTime   		= MOVIE_CLIP_TIME_1MIN;
#endif
   	gCurMenuInfo.uiMOVPowerOffTime		= MOVIE_POWEROFF_TIME_DEFAULT;
	gCurMenuInfo.uiMOVSoundRecord		= MOVIE_SOUND_RECORD_ON;
	gCurMenuInfo.uiVMDRecTime			= VMD_REC_TIME_5SEC;
	gCurMenuInfo.uiAutoRec				= AUTO_REC_ON;

    gCurMenuInfo.uiScene            	= SCENE_AUTO;
    gCurMenuInfo.uiFocus            	= FOCUS_MULTI;
    gCurMenuInfo.uiFocusRange       	= FOCUS_RANGE_NORMAL;
    gCurMenuInfo.uiMetering         	= METERING_WEIGHTED;
    gCurMenuInfo.uiEV               	= EVVALUE_00;
    gCurMenuInfo.uiISO              	= ISO_AUTO;
    gCurMenuInfo.uiWB               	= WB_AUTO;
    gCurMenuInfo.uiColor            	= COLOR_NATURAL;
    gCurMenuInfo.uiEffect           	= EFFECT_NORMAL;
    gCurMenuInfo.uiFaceTouch        	= FACE_DETECT_OFF;
    gCurMenuInfo.uiSmileDetect      	= SMILE_DETECT_1PEOPLE;
    gCurMenuInfo.uiSmileSensitivity 	= SMILE_SEN_LOW;

    gCurMenuInfo.uiSlideShowStart   	= SLIDESHOW_DISABLE;
    gCurMenuInfo.uiSlideShowFile    	= SLIDESHOW_FILE_ALL;
    gCurMenuInfo.uiSlideShowEffect 		= SLIDESHOW_EFFECT_SIMPLE;
    gCurMenuInfo.uiSlideShowMusic   	= SLIDESHOW_MUSIC_OFF;
    gCurMenuInfo.uiVolume           	= VOLUME_05;

    gCurMenuInfo.uiFileEdit         	= FILEEDIT_DELETE;
    gCurMenuInfo.uiMediaSelect      	= MEDIA_SETTING_SD_CARD;
    gCurMenuInfo.uiDisplayInfo      	= DISPLAY_INFO_SIMPLE;

    gCurMenuInfo.uiBeep             	= BEEP_OFF;
    gCurMenuInfo.uiLCDBrightness    	= LCD_BRIGHTNESS_NORMAL;
    gCurMenuInfo.uiAutoPowerOff     	= AUTO_POWER_OFF_NEVER;
    gCurMenuInfo.uiDateTimeSetup    	= 0;
#ifdef CFG_MENU_DEFAULT_DATE_TIME //may be defined in config_xxx.h, e.g. DATETIME_SETUP_NONE
    gCurMenuInfo.uiDateTimeFormat   	= CFG_MENU_DEFAULT_DATE_TIME;
#else
    gCurMenuInfo.uiDateTimeFormat   	= DATETIME_SETUP_YMD;
#endif
    gCurMenuInfo.uiDateLogoStamp		= DATE_STAMP;
    gCurMenuInfo.uiGPSStamp				= GPS_STAMP_ON;
    gCurMenuInfo.uiSpeedStamp			= SPEED_STAMP_OFF;
    gCurMenuInfo.uiLanguage         	= LANGUAGE_ENGLISH;
    gCurMenuInfo.uiTVSystem         	= TV_SYSTEM_NTSC;
    gCurMenuInfo.uiTVType           	= TV_TYPE_16_9;
    gCurMenuInfo.uiHDMIOutput       	= HDMI_OUTPUT_1080P;
    gCurMenuInfo.uiResetSetting     	= 0;
	gCurMenuInfo.uiFlickerHz			= FLICKER_50HZ;
	gCurMenuInfo.uiUSBFunction			= USB_MSDC;
	gCurMenuInfo.uiLCDRotate			= LCD_ROTATE_OFF;
	gCurMenuInfo.uiLCDPowerSave			= LCD_POWER_SAVE_OFF;
	gCurMenuInfo.uiLEDFlash				= LED_FALSH_OFF;
	gCurMenuInfo.uiGsensorSensitivity 	= GSENSOR_SENSITIVITY_OFF;
	gCurMenuInfo.uiMotionDtcSensitivity = MOTION_DTC_SENSITIVITY_OFF;
    gCurMenuInfo.uiTimeZone         	= TIMEZONE_GMT_P_8;
    gCurMenuInfo.uiBatteryVoltage   	= BATTERY_VOLTAGE_FULL;
    gCurMenuInfo.uiBootUpMode   		= BOOT_UP_MODE_DVR;
    gCurMenuInfo.uiWNR	            	= WNR_DEFAULT; 
}

void DefaultMenuSettingInit(void)
{
    gDefaultMenuInfo.uiIMGSize          	= IMAGE_SIZE_3M;//IMAGE_SIZE_14M;
    gDefaultMenuInfo.uiIMGQuality       	= QUALITY_SUPER_FINE;
    gDefaultMenuInfo.uiIMGStabilize     	= IMAGE_STABILIZE_OFF;
    gDefaultMenuInfo.uiFlashMode        	= FLASH_OFF;
    gDefaultMenuInfo.uiFlashLevel       	= FLASHLEVEL_NORMAL;
    gDefaultMenuInfo.uiRedEyeReduce     	= REDEYE_REDUCE_OFF;
    gDefaultMenuInfo.uiSelfTimer        	= SELF_TIMER_OFF;
    gDefaultMenuInfo.uiBurstShot        	= BURST_SHOT_OFF;
    gDefaultMenuInfo.uiVMDShotNum        	= VMD_SHOT_NUM_1P;
    gDefaultMenuInfo.uiTimeLapseTime   		= TIMELAPSE_INTERVAL_DEFAULT;

    gDefaultMenuInfo.uiMOVSize          	= MOVIE_SIZE_1080_30P;//MOVIE_SIZE_1080_60P;
    gDefaultMenuInfo.uiMOVQuality       	= QUALITY_SUPER_FINE;
    gDefaultMenuInfo.uiMOVStabilize     	= IMAGE_STABILIZE_OFF;
    gDefaultMenuInfo.uiSlowShutter      	= SLOW_SHUTTER_OFF;
    gDefaultMenuInfo.uiMicSensitivity   	= MIC_SEN_STANDARD;
    gDefaultMenuInfo.uiMOVPreRecord			= MOVIE_PRE_RECORD_OFF;
#ifdef CFG_MENU_DEFAULT_MOV_CLIP_TIME //may be defined in config_xxx.h, e.g. MOVIE_CLIP_TIME_OFF
   	gDefaultMenuInfo.uiMOVClipTime   		= CFG_MENU_DEFAULT_MOV_CLIP_TIME;
#else
   	gDefaultMenuInfo.uiMOVClipTime   		= MOVIE_CLIP_TIME_1MIN;
#endif
   	gDefaultMenuInfo.uiMOVPowerOffTime		= MOVIE_POWEROFF_TIME_DEFAULT;
	gDefaultMenuInfo.uiMOVSoundRecord		= MOVIE_SOUND_RECORD_ON;
   	gDefaultMenuInfo.uiVMDRecTime			= VMD_REC_TIME_5SEC;
	gDefaultMenuInfo.uiAutoRec				= AUTO_REC_ON;

    gDefaultMenuInfo.uiScene            	= SCENE_AUTO;
    gDefaultMenuInfo.uiFocus            	= FOCUS_MULTI;
    gDefaultMenuInfo.uiFocusRange       	= FOCUS_RANGE_NORMAL;
    gDefaultMenuInfo.uiMetering         	= METERING_WEIGHTED;
    gDefaultMenuInfo.uiEV               	= EVVALUE_00;
    gDefaultMenuInfo.uiISO              	= ISO_AUTO;
    gDefaultMenuInfo.uiWB               	= WB_AUTO;
    gDefaultMenuInfo.uiColor            	= COLOR_NATURAL;
    gDefaultMenuInfo.uiEffect           	= EFFECT_NORMAL;
    gDefaultMenuInfo.uiFaceTouch        	= FACE_DETECT_OFF;
    gDefaultMenuInfo.uiSmileDetect      	= SMILE_DETECT_1PEOPLE;
    gDefaultMenuInfo.uiSmileSensitivity 	= SMILE_SEN_LOW;

    gDefaultMenuInfo.uiSlideShowStart   	= SLIDESHOW_DISABLE;
    gDefaultMenuInfo.uiSlideShowFile    	= SLIDESHOW_FILE_ALL;
    gDefaultMenuInfo.uiSlideShowEffect  	= SLIDESHOW_EFFECT_SIMPLE;
    gDefaultMenuInfo.uiSlideShowMusic   	= SLIDESHOW_MUSIC_OFF;
    gDefaultMenuInfo.uiVolume           	= VOLUME_05;

    gDefaultMenuInfo.uiFileEdit         	= FILEEDIT_DELETE;
    gDefaultMenuInfo.uiMediaSelect      	= MEDIA_SETTING_SD_CARD;
    gDefaultMenuInfo.uiDisplayInfo      	= DISPLAY_INFO_SIMPLE;

    gDefaultMenuInfo.uiBeep             	= BEEP_ON;
    gDefaultMenuInfo.uiLCDBrightness    	= LCD_BRIGHTNESS_NORMAL;
#ifdef CFG_MENU_DEFAULT_AUTO_POWER_OFF_TIME //may be defined in config_xxx.h, e.g. AUTO_POWER_OFF_30SEC
    gDefaultMenuInfo.uiAutoPowerOff     	= CFG_MENU_DEFAULT_AUTO_POWER_OFF_TIME;
#else
    gDefaultMenuInfo.uiAutoPowerOff     	= AUTO_POWER_OFF_NEVER;
#endif
    gDefaultMenuInfo.uiDateTimeSetup    	= 0;
#ifdef CFG_MENU_DEFAULT_DATE_TIME //may be defined in config_xxx.h, e.g. DATETIME_SETUP_NONE
    gDefaultMenuInfo.uiDateTimeFormat   	= CFG_MENU_DEFAULT_DATE_TIME;
#else
    gDefaultMenuInfo.uiDateTimeFormat   	= DATETIME_SETUP_YMD;
#endif
    gDefaultMenuInfo.uiDateLogoStamp		= DATE_STAMP;
    gDefaultMenuInfo.uiGPSStamp				= GPS_STAMP_ON;
    gDefaultMenuInfo.uiSpeedStamp			= SPEED_STAMP_OFF;
    gDefaultMenuInfo.uiLanguage         	= LANGUAGE_ENGLISH;
    gDefaultMenuInfo.uiTVSystem         	= TV_SYSTEM_NTSC;
    gDefaultMenuInfo.uiTVType           	= TV_TYPE_16_9;
    gDefaultMenuInfo.uiHDMIOutput       	= HDMI_OUTPUT_1080P;
    gDefaultMenuInfo.uiResetSetting     	= 0;
	gDefaultMenuInfo.uiFlickerHz			= FLICKER_50HZ;
	gDefaultMenuInfo.uiUSBFunction			= USB_MSDC;
	gDefaultMenuInfo.uiLCDRotate			= LCD_ROTATE_OFF;
	gDefaultMenuInfo.uiLCDPowerSave			= LCD_POWER_SAVE_OFF;
	gDefaultMenuInfo.uiLEDFlash				= LED_FALSH_OFF;
	gDefaultMenuInfo.uiGsensorSensitivity 	= GSENSOR_SENSITIVITY_OFF;
	gDefaultMenuInfo.uiMotionDtcSensitivity = MOTION_DTC_SENSITIVITY_OFF;
    gDefaultMenuInfo.uiTimeZone         	= TIMEZONE_GMT_P_8;
    gDefaultMenuInfo.uiBatteryVoltage   	= BATTERY_VOLTAGE_FULL;
    gDefaultMenuInfo.uiBootUpMode   		= BOOT_UP_MODE_DVR;
    gDefaultMenuInfo.uiWNR   		        = WNR_DEFAULT;
}

void SystemSettingInit(void)
{
    gSystemInfo.byPlaybackVideoType        	= 0; //Normal video
}

pMenuInfo TempMenuConfig(void)
{
    return &gTempMenuInfo;
}

pMenuInfo MenuSettingConfig(void)
{
    return &gCurMenuInfo;
}

pMenuInfo DefaultMenuSettingConfig(void)
{
    return &gDefaultMenuInfo;
}

pSystemInfo SystemSettingConfig(void)
{
    return &gSystemInfo;
}

void CurrentMenuClear(void)
{
	memset(&gCurMenuInfo,0,sizeof(MenuInfo));
}

void DefaultMenuClear(void)
{
	memset(&gDefaultMenuInfo,0,sizeof(MenuInfo));
}

void Save2DefaultSetting(void)
{
    memcpy(&gDefaultMenuInfo, &gCurMenuInfo, sizeof(MenuInfo));
}

void RestoreFromDefaultSetting(void)
{
	extern const char txtDefFile[];

    UINT8 	BatteryVoltTemp;
    UINT8 	MediaTemp;
    UINT8	LCDRotateTemp;

#if ( LED_FLASH_CTRL==LED_BY_KEY || \
	  LED_FLASH_CTRL==LED_BY_KEY_WITH_MENU_CONFIRMED)
    UINT8	FlashLEDTemp = MenuSettingConfig()->uiFlashMode;
#endif

    BatteryVoltTemp 	= MenuSettingConfig()->uiBatteryVoltage;
    MediaTemp 			= MenuSettingConfig()->uiMediaSelect;
    LCDRotateTemp		= MenuSettingConfig()->uiLCDRotate;

	if (!ParseMenuSet((char*)txtDefFile, &gCurMenuInfo, 0))
	{
    	memcpy(&gCurMenuInfo, &gDefaultMenuInfo, sizeof(MenuInfo));
	}

	MenuSettingConfig()->uiBatteryVoltage = BatteryVoltTemp;

	Menu_SetVolume(MenuSettingConfig()->uiVolume);

	Menu_SetLCDDirectionEx(MenuSettingConfig()->uiLCDRotate, LCDRotateTemp);

#if ( LED_FLASH_CTRL==LED_BY_KEY || \
	  LED_FLASH_CTRL==LED_BY_KEY_WITH_MENU_CONFIRMED)
	MenuSettingConfig()->uiFlashMode = FlashLEDTemp;
#endif

#ifdef CFG_MENU_TURN_OFF_LED_WHEN_RESTORE_DEFAULT //may be defined in config_xxx.h
	if(LedCtrl_GetFlashLEDStatus())
	{
		LedCtrl_FlashLed(AHC_FALSE);
		MenuSettingConfig()->uiFlashMode = FLASH_OFF;
	}
#endif

#if defined(WIFI_PORT) && (WIFI_PORT == 1)
	printc("get_NetAppStatus() = 0x%X\r\n", get_NetAppStatus());
	printc("WLAN_SYS_GetMode()() = 0x%X\r\n", WLAN_SYS_GetMode());

	switch (get_NetAppStatus()) {
	case NETAPP_WIFI_NONE:
	case NETAPP_WIFI_INIT:
    case NETAPP_WIFI_FAIL:
		// NOP
		break;

	case NETAPP_WIFI_IDLE:
		if (WLAN_SYS_GetMode() == -1) {
			// NOP when Wi-Fi is off
			break;
		}

	default:
	//case NETAPP_WIFI_READY:
		set_NetAppStatus(NETAPP_WIFI_INIT);
		printc("### apply_user_net_setting -\r\n");
		apply_user_net_setting(CONFIG_SPACE_TYPE_CURRENT, NCFG_DEFSET_FILE_NAME);
		printc("### export_net_config_file -\r\n");
		export_net_config_file(CONFIG_SPACE_TYPE_CURRENT, NCFG_USERSET_FILE_NAME);
		printc("### netapp_ResetNetwork -\r\n");
		netapp_ResetNetwork();
		break;
	}
#endif

	AutoPowerOffCounterReset();

	LCDPowerSaveCounterReset();

	if(MediaTemp != MenuSettingConfig()->uiMediaSelect)
	{
        AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect);

        if(AHC_CheckCurSysMode(AHC_MODE_THUMB_BROWSER))
        {
            AHC_Thumb_DrawPage(AHC_TRUE);
        }
    }

	DriverIDReset();
}

void ListCurrentMenuSetting(void)
{
    mPRINT("List all Current Setting\n");
    ListAllMenuSetting(&gCurMenuInfo);
}

void ListAllMenuSetting(MenuInfo *Info)
{
#if (AIT_INT_USE==1)
    mPRINT("---------------------------------------------------\n");
    mPRINT(" uiIMGSize          = %d \n", Info->uiIMGSize          );
    mPRINT(" uiIMGQuality       = %d \n", Info->uiIMGQuality       );
    mPRINT(" uiIMGStabilize     = %d \n", Info->uiIMGStabilize     );
    mPRINT(" uiFlashMode        = %d \n", Info->uiFlashMode        );
    mPRINT(" uiFlashLevel       = %d \n", Info->uiFlashLevel       );
    mPRINT(" uiRedEyeReduce     = %d \n", Info->uiRedEyeReduce     );
    mPRINT(" uiSelfTimer        = %d \n", Info->uiSelfTimer        );
    mPRINT(" uiBurstShot        = %d \n", Info->uiBurstShot        );
    mPRINT(" uiVMDShotNum       = %d \n", Info->uiVMDShotNum       );
    mPRINT(" uiTimeLapseTime    = %d \n", Info->uiTimeLapseTime    );

    mPRINT(" uiMOVSize          = %d \n", Info->uiMOVSize          );
    mPRINT(" uiMOVQuality       = %d \n", Info->uiMOVQuality       );
    mPRINT(" uiMOVStabilize     = %d \n", Info->uiMOVStabilize     );
    mPRINT(" uiSlowShutter      = %d \n", Info->uiSlowShutter      );
    mPRINT(" uiMicSensitivity   = %d \n", Info->uiMicSensitivity   );
    mPRINT(" uiMOVPreRecord     = %d \n", Info->uiMOVPreRecord     );
    mPRINT(" uiMOVClipTime      = %d \n", Info->uiMOVClipTime      );
    mPRINT(" uiMOVPowerOffTime  = %d \n", Info->uiMOVPowerOffTime  );
    mPRINT(" uiMOVSoundRecord   = %d \n", Info->uiMOVSoundRecord   );
    mPRINT(" uiVMDRecTime       = %d \n", Info->uiVMDRecTime       );
    mPRINT(" uiAutoRec          = %d \n", Info->uiAutoRec          );

    mPRINT(" uiScene            = %d \n", Info->uiScene            );
    mPRINT(" uiFocus            = %d \n", Info->uiFocus            );
    mPRINT(" uiFocusRange       = %d \n", Info->uiFocusRange       );
    mPRINT(" uiMetering         = %d \n", Info->uiMetering         );
    mPRINT(" uiEV               = %d \n", Info->uiEV               );
    mPRINT(" uiISO              = %d \n", Info->uiISO              );
    mPRINT(" uiWB               = %d \n", Info->uiWB               );
    mPRINT(" uiColor            = %d \n", Info->uiColor            );
    mPRINT(" uiEffect           = %d \n", Info->uiEffect           );
    mPRINT(" uiFaceTouch        = %d \n", Info->uiFaceTouch        );
    mPRINT(" uiSmileDetect      = %d \n", Info->uiSmileDetect      );
    mPRINT(" uiSmileSensitivity = %d \n", Info->uiSmileSensitivity );

    mPRINT(" uiSlideShowStart   = %d \n", Info->uiSlideShowStart   );
    mPRINT(" uiSlideShowFile    = %d \n", Info->uiSlideShowFile    );
    mPRINT(" uiSlideShowEffect  = %d \n", Info->uiSlideShowEffect  );
    mPRINT(" uiSlideShowMusic   = %d \n", Info->uiSlideShowMusic   );
    mPRINT(" uiVolume           = %d \n", Info->uiVolume           );

    mPRINT(" uiFileEdit         = %d \n", Info->uiFileEdit         );
    mPRINT(" uiMediaSelect      = %d \n", Info->uiMediaSelect      );
    mPRINT(" uiDisplayInfo      = %d \n", Info->uiDisplayInfo      );

    mPRINT(" uiBeep             = %d \n", Info->uiBeep             );
    mPRINT(" uiLCDBrightness    = %d \n", Info->uiLCDBrightness    );
    mPRINT(" uiAutoPowerOff     = %d \n", Info->uiAutoPowerOff     );
    mPRINT(" uiDateTimeSetup    = %d \n", Info->uiDateTimeSetup    );
    mPRINT(" uiDateTimeFormat   = %d \n", Info->uiDateTimeFormat   );
    mPRINT(" uiDateLogoStamp    = %d \n", Info->uiDateLogoStamp    );
    mPRINT(" uiGPSStamp         = %d \n", Info->uiGPSStamp         );
    mPRINT(" uiSpeedStamp       = %d \n", Info->uiSpeedStamp       );
    mPRINT(" uiLanguage         = %d \n", Info->uiLanguage         );
    mPRINT(" uiTVSystem         = %d \n", Info->uiTVSystem         );
    mPRINT(" uiTVType           = %d \n", Info->uiTVType           );
    mPRINT(" uiHDMIOutput       = %d \n", Info->uiHDMIOutput       );
    mPRINT(" uiResetSetting     = %d \n", Info->uiResetSetting     );
    mPRINT(" uiFlickerHz        = %d \n", Info->uiFlickerHz        );
    mPRINT(" uiUSBFunction      = %d \n", Info->uiUSBFunction      );
    mPRINT(" uiLCDRotate        = %d \n", Info->uiLCDRotate        );
    mPRINT(" uiLCDPowerSave     = %d \n", Info->uiLCDPowerSave     );
    mPRINT(" uiLEDFlash         = %d \n", Info->uiLEDFlash         );
	mPRINT(" uiGsensorSensitivity   = %d \n", Info->uiGsensorSensitivity);
	mPRINT(" uiMotionDtcSensitivity = %d \n", Info->uiMotionDtcSensitivity);
    mPRINT(" uiTimeZone         = %d \n", Info->uiTimeZone         );
    mPRINT(" uiBootUpMode       = %d \n", Info->uiBootUpMode       );
    mPRINT(" uiWNR              = %d \n", Info->uiWNR              );
    mPRINT("---------------------------------------------------\n");
#endif
}

void SetCurrentOpMode(OP_MODE_SETTING ubMode)
{
#if(MIMF_CUSTOMIZED_SORT_TYPE && MIMF_CUSTOMIZED_SORT_MOV_ALL)
	if(ubMode == MOVPB_MODE)
	{
		AHC_MIMF_SetBrowserTypeMask(AHC_MIMF_BROWSER_MASK_MOV);
		AHC_MIMF_SetBrowserMask(AHC_MIMF_BROWSER_MASK_DB0|AHC_MIMF_BROWSER_MASK_DB1|AHC_MIMF_BROWSER_MASK_DB2|AHC_MIMF_BROWSER_MASK_REAR_CAM);
	}
#endif
#if (MIMF_CUSTOMIZED_SORT_TYPE && MIMF_CUSTOMIZED_SORT_JPG_ALL)
	if(ubMode == JPGPB_MODE)
	{
		AHC_MIMF_SetBrowserTypeMask(AHC_MIMF_BROWSER_MASK_JPG);
		AHC_MIMF_SetBrowserMask(AHC_MIMF_BROWSER_MASK_DB3|AHC_MIMF_BROWSER_MASK_REAR_CAM);
	}
#endif
    uiOpMode = ubMode;
}

UINT8 GetCurrentOpMode(void)
{
    return uiOpMode;
}

static void MenuSettingInRange(UINT8 *cur, INT32 low, INT32 high)
{
	*cur = (*cur>=high)?(high):( (*cur<=low)?(low):(*cur) );
}

AHC_BOOL CheckMenuSetting(MenuInfo* CurMenu)
{
	if(!CurMenu)
		return AHC_FALSE;

	//Still
	MenuSettingInRange(&(CurMenu->uiIMGSize), 			0, 	IMAGE_SIZE_NUM-1		);
	MenuSettingInRange(&(CurMenu->uiIMGQuality), 		0, 	QUALITY_NUM-1			);
	MenuSettingInRange(&(CurMenu->uiIMGStabilize), 		0, 	IMAGE_STABILIZE_NUM-1	);
	MenuSettingInRange(&(CurMenu->uiFlashMode), 		0, 	FLASH_NUM-1				);
	MenuSettingInRange(&(CurMenu->uiFlashLevel), 		0, 	FLASHLEVEL_NUM-1		);
	MenuSettingInRange(&(CurMenu->uiRedEyeReduce), 		0, 	REDEYE_REDUCE_NUM-1		);
	MenuSettingInRange(&(CurMenu->uiSelfTimer), 		0, 	SELF_TIMER_NUM-1		);
	MenuSettingInRange(&(CurMenu->uiBurstShot), 		0, 	BURST_SHOT_NUM-1		);
	MenuSettingInRange(&(CurMenu->uiVMDShotNum), 		0, 	VMD_SHOT_NUM_NUM-1		);
	MenuSettingInRange(&(CurMenu->uiTimeLapseTime), 	0, 	TIMELAPSE_INTERVAL_NUM-1);

	//Movie
	MenuSettingInRange(&(CurMenu->uiMOVSize), 			0, 	MOVIE_SIZE_NUM-1		);
	MenuSettingInRange(&(CurMenu->uiMOVQuality), 		0, 	QUALITY_NUM-1			);
	MenuSettingInRange(&(CurMenu->uiMOVStabilize), 		0, 	IMAGE_STABILIZE_NUM-1	);
	MenuSettingInRange(&(CurMenu->uiSlowShutter), 		0, 	SLOW_SHUTTER_NUM-1		);
	MenuSettingInRange(&(CurMenu->uiMicSensitivity), 	0, 	MIC_SEN_NUM-1		);
	// Not to check the range for MENU_TXT, MenuSettingInRange(&(CurMenu->uiMOVClipTime), 		0, 	MOVIE_CLIP_TIME_NUM);
	MenuSettingInRange(&(CurMenu->uiMOVPowerOffTime), 	0,  MOVIE_POWEROFF_TIME_NUM-1);
	MenuSettingInRange(&(CurMenu->uiMOVSoundRecord), 	0, 	MOVIE_SOUND_RECORD_NUM-1);
	MenuSettingInRange(&(CurMenu->uiMOVPreRecord), 		0, 	MOVIE_PRE_RECORD_NUM-1);
	MenuSettingInRange(&(CurMenu->uiVMDRecTime), 		0, 	VMD_REC_TIME_NUM-1		);
	MenuSettingInRange(&(CurMenu->uiAutoRec),	 		0, 	AUTO_REC_NUM-1			);

	//Manual
	MenuSettingInRange(&(CurMenu->uiScene), 			0, 	SCENE_NUM-1				);
	MenuSettingInRange(&(CurMenu->uiFocus), 			0, 	FOCUS_NUM-1				);
	MenuSettingInRange(&(CurMenu->uiFocusRange), 		0, 	FOCUS_RANGE_NUM-1		);
	MenuSettingInRange(&(CurMenu->uiMetering), 			0, 	METERING_NUM-1			);
	MenuSettingInRange(&(CurMenu->uiEV), 				0, 	EVVALUE_NUM-1			);
	MenuSettingInRange(&(CurMenu->uiISO), 				0, 	ISO_NUM-1				);
	MenuSettingInRange(&(CurMenu->uiWB), 				0, 	WB_NUM-1				);
	MenuSettingInRange(&(CurMenu->uiColor), 			0, 	COLOR_NUM-1				);
	MenuSettingInRange(&(CurMenu->uiEffect), 			0, 	EFFECT_NUM-1			);
	MenuSettingInRange(&(CurMenu->uiFaceTouch), 		0, 	FACE_DETECT_NUM-1		);
	MenuSettingInRange(&(CurMenu->uiSmileDetect), 		0, 	SMILE_DETECT_NUM-1		);
	MenuSettingInRange(&(CurMenu->uiSmileSensitivity), 	0, 	SMILE_SEN_NUM-1			);

	//Playback
	MenuSettingInRange(&(CurMenu->uiSlideShowStart), 	0, 	SLIDESHOW_NUM-1			);
	MenuSettingInRange(&(CurMenu->uiSlideShowFile), 	0,	SLIDESHOW_FILE_NUM-1	);
	MenuSettingInRange(&(CurMenu->uiSlideShowEffect), 	0, 	SLIDESHOW_EFFECT_NUM-1	);
	MenuSettingInRange(&(CurMenu->uiSlideShowMusic), 	0, 	SLIDESHOW_MUSIC_NUM-1	);
	MenuSettingInRange(&(CurMenu->uiVolume), 			0, 	VOLUME_NUM-1			);

	//Edit
	MenuSettingInRange(&(CurMenu->uiFileEdit), 			0, 	FILEEDIT_NUM-1			);

	//Media
	MenuSettingInRange(&(CurMenu->uiMediaSelect), 		0, 	MEDIA_SETTING_SD_CARD	);
	MenuSettingInRange(&(CurMenu->uiDisplayInfo), 		0, 	DISPLAY_INFO_NUM-1		);

	//General
	MenuSettingInRange(&(CurMenu->uiBeep), 				0, 	BEEP_NUM-1				);
	MenuSettingInRange(&(CurMenu->uiLCDBrightness), 	0, 	LCD_BRIGHTNESS_NUM-1	);
	MenuSettingInRange(&(CurMenu->uiAutoPowerOff), 		0, 	AUTO_POWER_OFF_NUM-1	);
	MenuSettingInRange(&(CurMenu->uiDateTimeSetup), 	0, 	0						);
	MenuSettingInRange(&(CurMenu->uiDateTimeFormat), 	0, 	DATETIME_SETUP_NUM-1	);
	MenuSettingInRange(&(CurMenu->uiDateLogoStamp), 	0, 	DATALOGO_SETUP_NUM-1	);
	MenuSettingInRange(&(CurMenu->uiGPSStamp), 			0, 	GPS_STAMP_NUM-1			);
	MenuSettingInRange(&(CurMenu->uiSpeedStamp), 		0, 	SPEED_STAMP_NUM-1		);
	MenuSettingInRange(&(CurMenu->uiLanguage), 			0, 	LANGUAGE_NUM-1			);
	MenuSettingInRange(&(CurMenu->uiTVSystem), 			0, 	TV_SYSTEM_NUM-1			);
	MenuSettingInRange(&(CurMenu->uiTVType), 			0, 	TV_TYPE_NUM-1			);
	MenuSettingInRange(&(CurMenu->uiHDMIOutput), 		0, 	HDMI_OUTPUT_NUM-1		);
	MenuSettingInRange(&(CurMenu->uiResetSetting), 		0, 	0						);
	MenuSettingInRange(&(CurMenu->uiBatteryVoltage), 	0, 	BATTERY_VOLTAGE_NUM-1	);
	MenuSettingInRange(&(CurMenu->uiFlickerHz), 		0, 	FLICKER_NUM-1			);
	MenuSettingInRange(&(CurMenu->uiUSBFunction), 		0, 	USB_NUM-1				);
	MenuSettingInRange(&(CurMenu->uiLCDRotate), 		0, 	LCD_ROTATE_NUM-1		);
	MenuSettingInRange(&(CurMenu->uiLCDPowerSave), 		0, 	LCD_POWER_SAVE_NUM-1	);
	MenuSettingInRange(&(CurMenu->uiLEDFlash), 			0, 	LED_FLASH_NUM-1			);
	MenuSettingInRange(&(CurMenu->uiGsensorSensitivity),0, 	GSENSOR_SENSITIVITY_NUM-1);
	MenuSettingInRange(&(CurMenu->uiMotionDtcSensitivity),0, MOTION_DTC_SENSITIVITY_NUM-1);
	MenuSettingInRange(&(CurMenu->uiTimeZone),			0, 	TIMEZONE_GMT_NUM-1		);
	MenuSettingInRange(&(CurMenu->uiBootUpMode),		0, 	BOOT_UP_MODE_NUM-1		);
	MenuSettingInRange(&(CurMenu->uiWNR),		        0, 	WNR_NUM-1		);

	return AHC_TRUE;
}

#if 0
void ____Menu_Extend_Function_____(){ruturn;} //dummy
#endif

#define	NUMBER_SETTING		(100)
#define	FOURCC(a, b, c, d)	((int)a + ((int)b << 8) + ((int)c << 16) + ((int)d << 24))
#define	MAGIC_SETTING		FOURCC('a', 'i', 't', 'm')
#define	VERSION_SETTING		0x0100
#define	NACT				(0)
#define	ACT					(1)
#define	IN_RANGE(v, p)		((v) >= ((p)->nMin) && (v) <= ((p)->nMax))

struct _setting_atom;

typedef	int	(*MFUN)(struct _setting_atom*, int, char*, void*);
typedef	int	(*GET_FUN)(int icur_val, va_list *ap);

typedef struct _setting_atom {
    char*		szSKey;
    int			nSVal;
    int			bSAct;
    int			nMin;
    int			nMax;
    MFUN		fnSet;
    GET_FUN     fnGet;
    const char*	szSRem;
} SETTING_ATOM;

typedef struct {
	int				idSet;
	int				vrSet;
	int				nuSet;
	SETTING_ATOM	sAtom[NUMBER_SETTING];
} MENU_ATOMS;

static const char 	szReserved[] 	= "Reserved";
static char			txtDoneFile[] 	= "SD:0:\\done.txt";

static int	ParamFun(SETTING_ATOM *pa, int idx, char *szVal, void *arg);
static int	pf_DateTime(SETTING_ATOM *pa, int idx, char *szVal, void *arg);
static int	pf_BitRate(SETTING_ATOM *pa, int idx, char *szVal, void *arg);
static int	pf_LoopRec(SETTING_ATOM *pa, int idx, char *szVal, void *arg);
static int	pf_VideoTime(SETTING_ATOM *pa, int idx, char *szVal, void *arg);
static int	pf_ResetTime(SETTING_ATOM *pa, int idx, char *szVal, void *arg);
static int 	pf_DriverID(SETTING_ATOM *pa, int idx, char *szVal, void *arg);
static int  pf_General(SETTING_ATOM *pa, int idx, char *szVal, void *arg);
static int	pf_TimeStampLogoTXT(SETTING_ATOM *pa, int idx, char *szVal, void *arg);

int         Setpf_GSNR_THR(int value);
int         Getpf_GSNR_THR(void);

static int Menu_Setting_Get_General(int icur_val, va_list *ap);
static int Menu_Setting_Get_TVSystem(int icur_val, va_list *ap);
static int Menu_Setting_Get_MicSensitivity(int icur_val, va_list *ap);
static int Menu_Setting_Get_GPS_RecordEnable(int icur_val, va_list *ap);
static int Menu_Setting_Get_VR_ClipTime(int icur_val, va_list *ap);
static int Menu_Setting_Get_Scene(int icur_val, va_list *ap);
//static int Menu_Setting_Get_VideoTimelapse(int icur_val, va_list *ap);
static int Menu_Setting_Get_Metering(int icur_val, va_list *ap);
static int Menu_Setting_Get_EV(int icur_val, va_list *ap);
static int Menu_Setting_Get_ISO(int icur_val, va_list *ap);
static int Menu_Setting_Get_WB(int icur_val, va_list *ap);
static int Menu_Setting_Get_Still_Quality(int icur_val, va_list *ap);
static int Menu_Setting_Get_VideoSticker(int icur_val, va_list *ap);
static int Menu_Setting_Get_Flicker(int icur_val, va_list *ap);

static MENU_ATOMS  temp_menu_atoms;

static MENU_ATOMS  menu_atoms =
{
    MAGIC_SETTING,
    VERSION_SETTING,
    NUMBER_SETTING,
    {
        // Still
        {/* 000 */COMMON_KEY_STILL_CAP_SIZE, 3, MENU_STILL_SIZE_EN,     0, IMAGE_SIZE_NUM - 1,              ParamFun, Menu_Setting_Get_General, "0~number of submenu - 1"},
        {/* 001 */STILL_QUALITY_KEY,0, MENU_STILL_QUALITY_EN,           0, QUALITY_NUM - 1,                 ParamFun, Menu_Setting_Get_Still_Quality, "0:Super fine, 1:fine"},
        {/* 002 */"StillStabilize", 1, MENU_STILL_STABILIZE_EN,         0, IMAGE_STABILIZE_NUM - 1,         ParamFun, NULL,     szReserved},
        {/* 003 */"StillFlash",     2, MENU_STILL_FLASH_EN,             0, FLASH_NUM - 1,                   ParamFun, NULL,     szReserved},
        {/* 004 */"StillFlashLevel",1, MENU_STILL_FLASH_LEVEL_EN,       0, FLASHLEVEL_NUM - 1,              ParamFun, NULL,     szReserved},
        {/* 005 */"RedEyeReduce",   1, MENU_STILL_REDEYE_REDUCE_EN,     0, REDEYE_REDUCE_NUM - 1,           ParamFun, NULL,     szReserved},
        {/* 006 */"SelfTimer",      0, MENU_STILL_SELF_TIMER_EN,        0, SELF_TIMER_NUM - 1,              ParamFun, NULL,     "Self timer, 0"},
        {/* 007 */"BurstShot",      0, MENU_STILL_BURST_SHOT_EN,        0, BURST_SHOT_NUM - 1,              ParamFun, NULL,     "Continuous short: 0:OFF, 1:2 shots"},
        // Movie
        {/* 008 */COMMON_KEY_MOVIE_SIZE,0, MENU_MOVIE_SIZE_EN,          0, MOVIE_SIZE_NUM - 1,              ParamFun, Menu_Setting_Get_General, "0:1920x1080 30fps, 1:1280x720 30fps, 2:1280x720 60fps, 3:640x480 30fps"},
        {/* 009 */"VideoQuality",   0, MENU_MOVIE_QUALITY_EN,           0, QUALITY_NUM - 1,                 ParamFun, NULL,     "0:Super fine, 1:fine"},
        {/* 010 */"VideoStabilize", 1, MENU_MOVIE_STABILIZE_EN,         0, IMAGE_STABILIZE_NUM - 1,         ParamFun, NULL,     szReserved},
        {/* 011 */"SlowShutter",    1, MENU_MOVIE_SLOW_SHUTTER_EN,      0, SLOW_SHUTTER_NUM - 1,            ParamFun, NULL,     szReserved},
        {/* 012 */COMMON_KEY_MIC_SENSITIVITY,0,MENU_MOVIE_MIC_SEN_EN,   0, MIC_SEN_NUM - 1,                 ParamFun, Menu_Setting_Get_MicSensitivity,     "MIC sensitivity, 0:STANDARD, 1:LOW"},
        {/* 013 */"VideoPreRecord", MOVIE_PRE_RECORD_OFF, MENU_MOVIE_PRE_RECORD_EN,0, MOVIE_PRE_RECORD_NUM - 1,ParamFun, NULL,  "Video pre-record, 0:ON, 1:OFF"},
        {/* 014 */COMMON_KEY_VR_CLIP_TIME, 2, MENU_MOVIE_CLIP_TIME_EN,  0, MOVIE_CLIP_TIME_NUM - 1,         ParamFun, Menu_Setting_Get_VR_ClipTime,     "Video clip time, 0: Off loop record, 1:1min, 2:3min 3:5min, 4:10min"},
        {/* 015 */"VideoOffTime",   1, MENU_MOVIE_POWER_OFF_DELAY_EN,   0, MOVIE_POWEROFF_TIME_NUM - 1,     ParamFun, NULL,     "0: 0sec 1:5sec, 2:1min, 3:5min, 4:30min, 5:60min"},
        {/* 016 */"RecordWithAudio",0, MENU_MOVIE_SOUND_RECORD_EN,      0, MOVIE_SOUND_RECORD_NUM - 1,      ParamFun, NULL,     "0:WITH, 1:WITHOUT"},
        // Manual
        {/* 017 */COMMON_KEY_SCENE, 0, MENU_MANUAL_SCENE_EN,            0, SCENE_NUM - 1,                   ParamFun, Menu_Setting_Get_Scene, "0:AUTO, see this submenu for others"},
        {/* 018 */"Focus",          0, NACT,                            0, FOCUS_NUM - 1,                   ParamFun, NULL,     szReserved},
        {/* 019 */"FocusRange",     0, NACT,                            0, FOCUS_RANGE_NUM - 1,             ParamFun, NULL,     szReserved},
        {/* 020 */COMMON_KEY_METER, 0, NACT,                            0, METERING_NUM - 1,                ParamFun, Menu_Setting_Get_Metering,     szReserved},
        {/* 021 */COMMON_KEY_EV,    6, MENU_MANUAL_EV_EN,               0, EVVALUE_NUM - 1,                 ParamFun, Menu_Setting_Get_EV,     "0:-2.0, 1:-1.66, 2:-1.33, 3:-1.00, 4:-0.66, 5:-0.33, 6:0, 7:+0.33, 8:+0.66, 9:+1.00, 10:+1.33, 11:+1,66, 12:+2.0"},
        {/* 022 */COMMON_KEY_ISO,   0, MENU_MANUAL_ISO_EN,              0, ISO_NUM - 1,                     ParamFun, Menu_Setting_Get_ISO,     "0:AUTO, 1:ISO-100, 2:ISO-200, 3:ISO-400"},
        {/* 023 */COMMON_KEY_WB,    1, MENU_MANUAL_WB_EN,               0, WB_NUM - 1,                      ParamFun, Menu_Setting_Get_WB,     "0:AUTO, see this submenu for others"},
        {/* 024 */"Color",          0, MENU_MANUAL_COLOR_EN,            0, COLOR_NUM - 1,                   ParamFun, NULL,     "0:Natural, 1:Vidi, 2:Pale"},
        {/* 025 */"Effect",         0, MENU_MANUAL_EFFECT_EN,           0, EFFECT_NUM - 1,                  ParamFun, NULL,     "Effect:0:Normal"},
        {/* 026 */"FaceTouch",      0, MENU_MANUAL_FACE_DETECT_EN,      0, FACE_DETECT_NUM - 1,             ParamFun, NULL,     szReserved},
        {/* 027 */"SmileDetect",    0, MENU_MANUAL_SMILE_DETECT_EN,     0, SMILE_DETECT_NUM - 1,            ParamFun, NULL,     szReserved},
        {/* 028 */"SmileSensitivity",0, MENU_MANUAL_SMILE_SEN_EN,       0, SMILE_SEN_NUM - 1,               ParamFun, NULL,     szReserved},
        // Playback
        {/* 029 */"SlideShow",          0, SLIDESHOW_EN,                        0, SLIDESHOW_NUM - 1,       ParamFun, NULL,     "0:Disable, 1:Enable"},
        {/* 030 */"SlideShowFileType",  0, MENU_PLAYBACK_SLIDESHOW_FILE_EN,     0, SLIDESHOW_FILE_NUM - 1,  ParamFun, NULL,     "FileType:0:PHOTO and VIDEO, 1:PHOTO only, 2:VIDEO only"},
        {/* 031 */"SlideShowEffect",    0, MENU_PLAYBACK_SLIDESHOW_EFFECT_EN,   0, SLIDESHOW_EFFECT_NUM - 1,ParamFun, NULL,     "Effect:0:Simple, see this submenu for others"},
        {/* 032 */"SlideShowMusic",     0, MENU_PLAYBACK_SLIDESHOW_MUSIC_EN,    0, SLIDESHOW_MUSIC_NUM - 1, ParamFun, NULL,     "Music:0:On, 1:Off"},
        {/* 033 */"PlaybackVolume",     4, MENU_PLAYBACK_VOLUME_EN,             0, VOLUME_NUM - 1,          ParamFun, NULL,     "Volume:0~9, 0 is Min, 9 is Max"},
        // Edit Tool
        {/* 034 */"FileEdit",       0, NACT,                            0, 0,                               ParamFun, NULL,     szReserved},
        // Media Tool
        {/* 035 */"MediaSelect",    0, NACT,                            0, 0,                               ParamFun, NULL,     szReserved},
        {/* 036 */"DisplayInfo",    0, NACT,                            0, 0,                               ParamFun, NULL,     szReserved},
        // General
        {/* 037 */"Beep",           0, MENU_GENERAL_BEEP_EN,            0, BEEP_NUM - 1,                    ParamFun, NULL,     "KeyBeep:0:On, 1:Off"},
        {/* 038 */"LCDBrightness",  0, MENU_GENERAL_LCD_BRIGHTNESS_EN,  0, LCD_BRIGHTNESS_NUM - 1,          ParamFun, NULL,     szReserved},
        {/* 039 */"AutoPowerOff",   0, MENU_GENERAL_AUTO_POWEROFF_EN,   0, AUTO_POWER_OFF_NUM - 1,          ParamFun, NULL,     "Auto Power Off:0:Never, 1:30sec, 2:1min, 3:2min, 4:5min"},
        {/* 040 */"DateTime",       0, NACT,                            0, 0,                               pf_DateTime,NULL,   szReserved},
        {/* 041 */"DateTimeFormat", 1, MENU_GENERAL_DATE_FORMAT_EN,     0, DATETIME_SETUP_NUM - 1,          ParamFun, NULL,     "0:NOT Display Date/Time, 1:YMD, 2:MDY, 3:DMY"},
        {/* 042 */"Language",       0, MENU_GENERAL_LANGUAGE_EN,        0, LANGUAGE_NUM - 1,                ParamFun, NULL,     "0~.., see language submenu list"},
        {/* 043 */TV_SYSTEM_KEY,    0, MENU_GENERAL_TV_SYSTEM_EN,       0, TV_SYSTEM_NUM - 1,               ParamFun, Menu_Setting_Get_TVSystem, "0:NTSC, 1:PAL"},
        {/* 044 */"TVType",         0, NACT,                            0, 1,                               ParamFun, NULL,     szReserved},
        {/* 045 */"HDMIOutput",     0, MENU_GENERAL_HDMI_OUTPUT_EN,     0, HDMI_OUTPUT_NUM - 1,             ParamFun, NULL,     "0:1080, 1:720"},
        {/* 046 */"Reset",          0, NACT,                            0, 1,                               ParamFun, NULL,     szReserved},
        {/* 047 */"BatteryVoltage", 0, NACT,                            0, BATTERY_VOLTAGE_NUM - 1,         ParamFun, NULL,     szReserved},
        {/* 048 */FLICKER,          0, MENU_GENERAL_FLICKER_EN,         0, FLICKER_NUM - 1,                 ParamFun, Menu_Setting_Get_Flicker,    "0:50Hz, 1:60Hz"},
        {/* 049 */"USB",            0, MENU_GENERAL_USB_FUNCTION_EN,    0, USB_NUM - 1,                     ParamFun, NULL,     "0:MSDC, 1:PCCam"},
        {/* 050 */"LCDRotate",      0, MENU_GENERAL_LCD_ROTATE_EN,      0, LCD_ROTATE_NUM - 1,              ParamFun, NULL,     "0:180deg, 1:0deg"},
        {/* 051 */"LCDPowerSaving", 0, MENU_GENERAL_LCD_POWER_SAVE_EN,  0, LCD_POWER_SAVE_NUM - 1,          ParamFun, NULL,     "0:Nerver, 1:1min, 2:3min"},
        {/* 052 */"LEDFash",        1, MENU_GENERAL_FLASH_LED_EN,       0, LED_FLASH_NUM - 1,               ParamFun, NULL,     "0:On, 1:Off"},
        {/* 053 */GSNR_SENS_KEY,    0, MENU_GENERAL_GSENSOR_EN,         0, GSENSOR_SENSITIVITY_NUM - 1,     ParamFun, Menu_Setting_Get_General, "0:Off, see this submenu for others"},
        {/* 054 */"TimeZone",       13, MENU_GENERAL_TIME_ZONE_EN,      0, TIMEZONE_GMT_NUM - 1,            ParamFun, NULL,     szReserved},
        {/* 055 */"TimeStamp",      0, MENU_GENERAL_DATE_LOGO_STAMP_EN, 0, DATALOGO_SETUP_NUM - 1,          ParamFun, NULL,     "0:Time+Logo, 1:Time, 2:Logo, 3:None"},
        {/* 056 */MOTION_SENS,      0, MENU_GENERAL_MOTION_DTC_EN,      0, MOTION_DTC_SENSITIVITY_NUM - 1,  ParamFun, Menu_Setting_Get_General, "0:Off, 1:Low, 2:Middle, 3:High"},
        {/* 057 */"MotionVideoTime",0, MENU_MOVIE_VMD_REC_TIME_EN,      0, VMD_REC_TIME_NUM - 1,            ParamFun, NULL,     "0:5sec, 1:10sec, 2:30sec, 3:1min"},
        {/* 058 */"MotionShot",     0, MENU_STILL_VMD_SHOT_NUM_EN,      0, VMD_SHOT_NUM_NUM - 1,            ParamFun, NULL,     "0:1 shot, 1:2 shots, 2:3 shots, 3:5 shots"},
        {/* 059 */"BootMode",       0, NACT,                            0, 0,                               ParamFun, NULL,     szReserved},
        {/* 060 */"TimeLapse",      0, MENU_STILL_TIMELAPSE_TIME_EN,    0, TIMELAPSE_INTERVAL_NUM - 1,      ParamFun, NULL,     "0~ see this submenu."},
        {/* 061 */"AutoRec",        0, MENU_MOVIE_AUTO_REC_EN,          0, AUTO_REC_NUM - 1,                ParamFun, NULL,     "0:On, 1:Off"},
        {/* 062 */"GPSStamp",       0, MENU_GENERAL_GPS_STAMP_EN,       0, GPS_STAMP_NUM - 1,               ParamFun, NULL,     "0:On, 1:Off"},
        {/* 063 */"SpeedStamp",     0, MENU_GENERAL_SPEED_STAMP_EN,     0, SPEED_STAMP_NUM - 1,             ParamFun, NULL,     "0:On, 1:Off"},
        {/* 064 */COMMON_KEY_GPS_RECORD_ENABLE, 0, MENU_GENERAL_GPS_RECORD_EN, 0, RECODE_GPS_NUM - 1,       ParamFun, Menu_Setting_Get_GPS_RecordEnable, "0:Disable, 1:Enable"},
        {/* 065 */"IMGUpSideDown",  0, 0,                               0, 1,                               ParamFun, NULL,     szReserved},
        {/* 066 */"WDR",            0, 0,                               0, 1,                               ParamFun, NULL,     szReserved},
        {/* 067 */"StatusLight",    0, 0,                               0, 1,                               ParamFun, NULL,     szReserved},
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /* NEW!! Extension setting */
        {/* 068 */"BitRate",        0, MENU_TXT_BITRATE,                1, 15,                              pf_BitRate, NULL,   "1 ~ 15, Unit: MBits"},
        {/* 069 */"LoopRec",        0, MENU_TXT_VIDEOTIME,              0, 1,                               pf_LoopRec, NULL,   "Loop Record: 0:Off, 1:On"},
        {/* 070 */"VideoTime",      0, MENU_TXT_VIDEOTIME,              1, 60,                              pf_VideoTime,NULL,  "Unit: minutes"},
        {/* 071 */"ResetTime",      0, ACT,                             0, 1,                               pf_ResetTime,NULL,  "Reset RTC time"},
        {/* 072 */GSNR_PWRON_SENS_KEY,0,MENU_GENERAL_POWERON_GSENSOR_EN,0, GSENSOR_POWERON_SENSITIVITY_NUM - 1,pf_General, Menu_Setting_Get_General,   "0:Off, see this submenu for others"},
        {/* 073 */DRIVER_ID_KEY,    0, MENU_GENERAL_DRIVER_ID_SETTING_EN,0,0,                               pf_DriverID,NULL,   "---------"},
        {/* 074 */WIFI_KEY,         1, MENU_GENERAL_WIFI_EN,            0, 1,                               pf_General, Menu_Setting_Get_General,   "0:On, 1:Off"},
        {/* 075 */TIME_STAMP_LOGO_TXT_KEY, 0, 1,                        0, 0,                               pf_TimeStampLogoTXT, Menu_Setting_Get_VideoSticker, model},
        {/* 076 */VIDEO_REC_TIMELAPSE_KEY, 0, MENU_VIDEO_TIMELAPSE_MODE_EN, 0, PRJ_VR_TIMELAPSE_NUM - 1,    pf_General, /*Menu_Setting_Get_VideoTimelapse*/Menu_Setting_Get_General, "0:Off, 1:1s, 2:5s, 3:10s, 4:30s, 5:60s"},
        {/* 077 */LDWS_EN_KEY,      1, MENU_MOVIE_LDWS_MODE_EN,         0, 1,                               pf_General, Menu_Setting_Get_General,   "0:On, 1:Off"},
        {/* 078 */FCWS_EN_KEY,      1, MENU_MOVIE_FCWS_MODE_EN,         0, 1,                               pf_General, Menu_Setting_Get_General,   "0:On, 1:Off"},

        {/* 079 */SAG_EN_KEY,       1, MENU_MOVIE_SAG_MODE_EN,          0, 1,                               pf_General, Menu_Setting_Get_General,   "0:On, 1:Off"},
        {/* 080 */HDR_EN_KEY,       1, MENU_MOVIE_HDR_MODE_EN,          0, 1,                               pf_General, Menu_Setting_Get_General,   "0:On, 1:Off"},
        {/* 081 */CONTRAST_KEY,     0, MENU_MANUAL_CONTRAST_EN,     MENU_MANUAL_CONTRAST_MIN,   MENU_MANUAL_CONTRAST_MAX,   pf_General, Menu_Setting_Get_General,   szReserved},
        {/* 082 */SATURATION_KEY,   0, MENU_MANUAL_SATURATION_EN,   MENU_MANUAL_SATURATION_MIN, MENU_MANUAL_SATURATION_MAX, pf_General, Menu_Setting_Get_General,   szReserved},
        {/* 083 */SHARPNESS_KEY,    0, MENU_MANUAL_SHARPNESS_EN,    MENU_MANUAL_SHARPNESS_MIN,  MENU_MANUAL_SHARPNESS_MAX,  pf_General, Menu_Setting_Get_General,   szReserved},
        {/* 084 */GAMMA_KEY,        0, MENU_MANUAL_GAMMA_EN,        MENU_MANUAL_GAMMA_MIN,      MENU_MANUAL_GAMMA_MAX,      pf_General, Menu_Setting_Get_General,   szReserved},
        {/* 085 */SLOWMOTION_EN_KEY,0, MENU_MOVIE_SLOWMOTION_MODE_EN,   0,                      SLOWMOTION_NUM - 1,         pf_General, Menu_Setting_Get_General,   "0:Off, 1:X2, 2:X4, 3:X8"},
        {/* 086 */WNR_EN_KEY,       0, MENU_MOVIE_WNR_MODE_EN,               0, 1,                                               pf_General, Menu_Setting_Get_General,   "0:Off, 1:On"},
        
        /* END OF MARK */
        {NULL,                      0, NACT,                            0, 0,                               NULL,       NULL,                       NULL}
    }
};

AHC_BOOL Menu_Setting_CheckMenuAtoms(void)
{
    if (memcmp((char *) &temp_menu_atoms, (char *) &menu_atoms, sizeof(MENU_ATOMS)) == 0) {
        RTNA_DBG_Str(0, FG_GREEN("Menu setting is same\r\n"));
        return AHC_TRUE;
    }

    RTNA_DBG_Str(0, FG_RED("Menu setting is changed\r\n"));
    return AHC_FALSE;
}

void Menu_Setting_BackupMenuAtoms(void)
{
	printc(FG_GREEN("Menu_Setting_BackupMenuAtoms\r\n"));
	memcpy((char *) TempMenuConfig(), (char *) MenuSettingConfig(), sizeof(MenuInfo));
	ImportMenuInfo(MenuSettingConfig());
    memcpy((char *) &temp_menu_atoms, (char *) &menu_atoms, sizeof(MENU_ATOMS));
}

/* Read Text File Setting to MENU_ATOMS and MenuInfo */
int ParseMenuSet(char *file, MenuInfo* pmi /*out*/, int filter)
{
	AHC_BOOL	err;
	UINT32		fd;
	UINT32		fs;
	char		line[MAX_CHAR_LINE];
	char		*fb;
	MMP_ULONG   fb_addr;
	int			fp;

	mPRINT("ParseMenuSet READ %s\r\n", file);
	/* Note: the characters in line  must less then MAX_CHAR_LINE in set txt file */

	err = AHC_FS_FileOpen(file, AHC_StrLen(file), "rb", AHC_StrLen("rb"), &fd);

	if (err)
		return 0;

	err = AHC_FS_FileGetSize(fd, &fs);

	if (err) {
		err = 0;
		goto END_ParseMenuSet;
	}

	if (fs > AHC_MENU_SETTING_TEMP_BUFFER_SIZE) {
		RTNA_DBG_Str(0, FG_RED("--E-- To protect system, the menu file should be less then 8K\r\n"));
		err = 0;
		goto END_ParseMenuSet;
	}

    MMPS_System_GetPreviewFrameStart(&fb_addr);

    if (fb_addr) {
        fb = (char *) ((fb_addr - (AHC_MENU_SETTING_TEMP_BUFFER_SIZE + 31)) & 0xFFFFFE0);
    }
    else {
        fb = (char *) AHC_THUMB_TEMP_BASE_ADDR;
    }

    //printc(FG_YELLOW("### ParseMenuSet - using 0x%X + 8192 as temp buffer\r\n"), (UINT32) fb);

	err = AHC_FS_FileRead(fd, (unsigned char*)fb, fs, (UINT32*)&fp);

	if (fs != fp || err != 0) {
		err = 0;
		goto END_ParseMenuSet;
	}

    err = 1;
	fp = 0;

	while (AHC_ReadLineMem(fb, &fp, fs, line, MAX_CHAR_LINE) != 0)
	{
		char	*key, *val;
		int		i;

		key = strtok(line, " :=\t");
		val = strtok(NULL, " ;\t\r\n");

		if (key == NULL)
			continue;

		if (*key == ';' || strcmpi(key, "REM") == 0) {
			continue;
		}

		if (strcmpi(key, "delete") == 0) {
			err = POST_DELETE;
			continue;
		}

		if (strcmpi(key, "rename") == 0) {
			err = POST_RENAME;
			continue;
		}

		if (val == NULL)
			continue;

		for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
		{
			if (strcmpi(key, menu_atoms.sAtom[i].szSKey) == 0)
			{
				/* call ParamFun or pf_XXXXX */
				menu_atoms.sAtom[i].fnSet(&menu_atoms.sAtom[0], i, val, pmi);
				break;
			}
		}
	}

END_ParseMenuSet:
    AHC_FS_FileClose(fd) ;
	return err;
}

#ifdef CFG_MENU_DATE_TIME_FORMAT_2 //may be defined in config_xxx.h
int ParseDateTime(char *dt, int format,
					unsigned short *y, unsigned short *m, unsigned short *d,
					unsigned short *hh, unsigned short *mm, unsigned short *ss)
{
	char	*tok;
	char	*sep1 = "/";
	char	*sep2 = "-";
	char	*sep3 = ":";
	char	*sep4 = "/- :";
	int		ret;
	int		i;

	ret = 0;

    *y = 0;
    *m = 0;
    *d = 0;
    *hh = 24;
    *mm = 60;
    *ss = 60;

    if(format > 0) {

		tok = strtok(dt, sep1);
		if (!tok) {
		    printc("ParseDateTime: 00 error!!\r\n");
		    return 0;
		}
		for(i=0; i<strlen(tok); i++) {
		    printc("%d:%c\r\n", i, tok[i]);
            if((tok[i]<'0') || (tok[i]>'9')) {
                printc("ParseDateTime: 01 error!! %d %c\r\n", i, tok[i]);
                return 0;
            }
		}
		*y = atoi(tok);

		tok = strtok(NULL, sep1);
		if (!tok) {
		    printc("ParseDateTime: 02 error!!\r\n");
		    return 0;
		}
		for(i=0; i<strlen(tok); i++) {
		    printc("%d:%c\r\n", i, tok[i]);
            if((tok[i]<'0') || (tok[i]>'9')) {
                printc("ParseDateTime: 03 error!! %d %c\r\n", i, tok[i]);
                return 0;
            }
		}
		*m = atoi(tok);

		tok = strtok(NULL, sep2);
		if (!tok) {
		    printc("ParseDateTime: 04 error!!\r\n");
		    return 0;
		}
		for(i=0; i<strlen(tok); i++) {
		    printc("%d:%c\r\n", i, tok[i]);
            if((tok[i]<'0') || (tok[i]>'9')) {
                printc("ParseDateTime: 05 error!! %d %c\r\n", i, tok[i]);
                return 0;
            }
		}
		*d = atoi(tok);

		tok = strtok(NULL, sep3);
		if (!tok) {
		    printc("ParseDateTime: 06 error!!\r\n");
		    return 0;
		}
		for(i=0; i<strlen(tok); i++) {
		    printc("%d:%c\r\n", i, tok[i]);
            if((tok[i]<'0') || (tok[i]>'9')) {
                printc("ParseDateTime: 07 error!! %d %c\r\n", i, tok[i]);
                return 0;
            }
		}
		*hh = atoi(tok);

		tok = strtok(NULL, sep3);
		if (!tok) {
		    printc("ParseDateTime: 08 error!!\r\n");
		    return 0;
		}
		for(i=0; i<strlen(tok); i++) {
		    printc("%d:%c\r\n", i, tok[i]);
            if((tok[i]<'0') || (tok[i]>'9')) {
                printc("ParseDateTime: 09 error!! %d %c\r\n", i, tok[i]);
                return 0;
            }
		}
		*mm = atoi(tok);

		tok = strtok(NULL, sep4);
		if (!tok) {
		    printc("ParseDateTime: 10 error!!\r\n");
		    return 0;
		}
		for(i=0; i<strlen(tok); i++) {
		    printc("%d:%c\r\n", i, tok[i]);
            if((tok[i]<'0') || (tok[i]>'9')) {
                printc("ParseDateTime: 11 error!! %d %c\r\n", i, tok[i]);
                return 0;
            }
		}
		*ss = atoi(tok);
		ret = 1;
	}

    printc("ParseDateTime %d/%d/%d-%d:%d:%d\r\n", *y, *m, *d, *hh, *mm, *ss);
    if((*y>9999) || (*y<2000)) {
        printc("ParseDateTime: y error!!\r\n");
        ret = 0;
    }
    if((*m>12) || (*m<1)) {
        printc("ParseDateTime: m error!!\r\n");
        ret = 0;
    }
    if((*d>31) || (*d<1)) {
        printc("ParseDateTime: d error!!\r\n");
        ret = 0;
    }
    if(*hh>23) {
        printc("ParseDateTime: hh error!!\r\n");
        ret = 0;
    }
    if(*mm>59) {
        printc("ParseDateTime: mm error!!\r\n");
        ret = 0;
    }
    if(*ss>59) {
        printc("ParseDateTime: ss error!!\r\n");
        ret = 0;
    }

	return ret;
}
#else
int ParseDateTime(char *dt, int format,
					unsigned short *y, unsigned short *m, unsigned short *d,
					unsigned short *hh, unsigned short *mm, unsigned short *ss)
{
	char	*tok;
	char	*sep = "/- :";
	int		ret;

	ret = 0;

	while (format > 0) {

		tok = strtok(dt, sep); 	 if (!tok) break;
		*y = atoi(tok);
		tok = strtok(NULL, sep); if (!tok) break;
		*m = atoi(tok);
		tok = strtok(NULL, sep); if (!tok) break;
		*d = atoi(tok);
		tok = strtok(NULL, sep); if (!tok) break;
		*hh = atoi(tok);
		tok = strtok(NULL, sep); if (!tok) break;
		*mm = atoi(tok);
		tok = strtok(NULL, sep); if (!tok) break;
		*ss = atoi(tok);
		ret = 1;
	}
	return ret;
}
#endif

AHC_BOOL ApplyUserTextSetting(char *file, MenuInfo* pmi)
{
	AHC_BOOL	ret;

	extern MMP_ERR MMPF_FS_Rename(const MMP_BYTE *sOldName, const MMP_BYTE *sNewName);

	printc("### %s -\r\n", __func__);

	ret = ParseMenuSet(file, pmi, MENU_FILTER_DATETIME);

	if (!ret)
		return ret;

	mPRINT(">>>>> Apply New setting %d<<<<<\r\n", ret);

	if (ret == POST_DELETE) {
		AHC_FS_FileRemove(file, AHC_StrLen(file));
	} else if (ret == POST_RENAME) {
		AHC_FS_FileRemove(txtDoneFile, AHC_StrLen(txtDoneFile));
		MMPF_FS_Rename(file, txtDoneFile);
	}

	// Write to User setting file in SF
	AHC_PARAM_Menu_Write(AHC_DEFAULT_USER_FAT);
	Menu_WriteLcdSnrTVHdmiStauts2SF();

	return ret;
}

/*
 * MENU_ATOMS Export to Text File
 * flag: TXT_NO_COMMENT -> Not write comment into file (DEFAULT)
 *       TXT_COMMENT    -> Write comment into file
 *		 TXT_MENU_ALL	-> Write All of menu setting (for useset.txt in SF only)
 *		 TXT_MENU_ACT	-> Not write disable menu setting item (DEFAULT)
 *		 TXT_VERSION	-> Write Version at head of file
 */
AHC_BOOL ExportMenuAtom(char *file, int flag)
{
	UINT32		fd;
	int 		i;
	char		line[MAX_CHAR_LINE * 2];
	char		*speedup;
	UINT32      speedup_addr;
	int			ls, os;
	int			ci = 0;

    if (AHC_FS_FileOpen(file, AHC_StrLen(file), "wb", AHC_StrLen("wb"), &fd )) {
    	return AHC_FALSE;
    }

    MMPS_System_GetPreviewFrameStart(&speedup_addr);

    if (speedup_addr) {
        speedup = (char *) ((speedup_addr - (AHC_MENU_SETTING_TEMP_BUFFER_SIZE + 31)) & 0xFFFFFE0);
    }
    else {
        speedup = (char *) AHC_THUMB_TEMP_BASE_ADDR;
    }

    //printc(FG_YELLOW("### ExportMenuAtom - using 0x%X + 8192 as temp buffer\r\n"), (UINT32) speedup);

	if (flag != TXT_VERSION)
	{	// Not TXT_VERSION only
		for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
		{
			if (flag & TXT_MENU_ALL || menu_atoms.sAtom[i].bSAct)
			{
	            #ifdef CFG_MENU_DATE_TIME_FORMAT_2 //may be defined in config_xxx.h
	            if((AHC_Strcmp((char *)"DateTime", (char *)menu_atoms.sAtom[i].szSKey)==0) && (AHC_StrLen((char *)menu_atoms.sAtom[i].szSKey)==8)) {
	                ls = snprintf(line, MAX_CHAR_LINE * 2 - 1, "DateTime=0000/00/00-00:00:00\t\t;%s\r\n", menu_atoms.sAtom[i].szSRem);
			    }
			    else
				#endif
				{
					if (flag & TXT_COMMENT) {
						if (AHC_Strcmp((const char *)DRIVER_ID_KEY, (const char *)menu_atoms.sAtom[i].szSKey) == 0)
						{
#ifdef CFG_MENU_CUS_DRIVER_ID //may be defined in config_xxx.h
							int j;

							// transfer ' '  to '-'
							for (j = 0; j < 9/*(sizeof(Driverid) - 1)*/; j++)
							{
								if (*(((char *)menu_atoms.sAtom[i].szSRem) + j) == ' ')
								{
									*(((char *)menu_atoms.sAtom[i].szSRem) + j) = '-';
								}
							}

							*(((char *)menu_atoms.sAtom[i].szSRem) + 9) = '\0'; // TBC
#endif

							ls = snprintf(line, MAX_CHAR_LINE * 2 - 1, "%s=%s\t\t;%s [Min:%d, Max:%d]\r\n",
												menu_atoms.sAtom[i].szSKey,
												menu_atoms.sAtom[i].szSRem,
												menu_atoms.sAtom[i].szSRem,
												menu_atoms.sAtom[i].nMin,
												menu_atoms.sAtom[i].nMax);
						}
						else if (AHC_Strcasecmp((const char *) TIME_STAMP_LOGO_TXT_KEY, (const char *)menu_atoms.sAtom[i].szSKey) == 0)
						{
							int j = 0;

							// transfer ' '  to '-'
							do {
								if (*(((char *) menu_atoms.sAtom[i].szSRem) + j) == ' ')
								{
									*(((char *) menu_atoms.sAtom[i].szSRem) + j) = '-';
								}

								j++;
							} while (*(((char *) menu_atoms.sAtom[i].szSRem) + j));

							ls = snprintf(line, MAX_CHAR_LINE * 2 - 1, "%s=%s\t\t;%s [Min:%d, Max:%d]\r\n",
												menu_atoms.sAtom[i].szSKey,
												menu_atoms.sAtom[i].szSRem,
												menu_atoms.sAtom[i].szSRem,
												menu_atoms.sAtom[i].nMin,
												menu_atoms.sAtom[i].nMax);
						}
						else
						{
							ls = snprintf(line, MAX_CHAR_LINE * 2 - 1, "%s=%d\t\t;%s [Min:%d, Max:%d]\r\n",
												menu_atoms.sAtom[i].szSKey,
												menu_atoms.sAtom[i].nSVal,
												menu_atoms.sAtom[i].szSRem,
												menu_atoms.sAtom[i].nMin,
												menu_atoms.sAtom[i].nMax);
						}
					} else {
						if (AHC_Strcmp((const char *)DRIVER_ID_KEY, (const char *)menu_atoms.sAtom[i].szSKey) == 0)
						{
#ifdef CFG_MENU_CUS_DRIVER_ID //may be defined in config_xxx.h
							int j;

							// transfer ' '  to '-'
							for (j = 0; j < 9/*(sizeof(Driverid) - 1)*/; j++)
							{
								if (*(((char *)menu_atoms.sAtom[i].szSRem) + j) == ' ')
								{
									*(((char *)menu_atoms.sAtom[i].szSRem) + j) = '-';
								}
							}

							*(((char *)menu_atoms.sAtom[i].szSRem) + 9) = '\0'; // TBC
#endif

							ls = snprintf(line, MAX_CHAR_LINE * 2 - 1, "%s=%s\r\n",
												menu_atoms.sAtom[i].szSKey,
												menu_atoms.sAtom[i].szSRem);
						}
						else if (AHC_Strcasecmp((const INT8 *) TIME_STAMP_LOGO_TXT_KEY, (const INT8 *) menu_atoms.sAtom[i].szSKey) == 0)
						{
							int j = 0;

							// transfer ' '  to '-'
							do {
								if (*(((char *) menu_atoms.sAtom[i].szSRem) + j) == ' ')
								{
									*(((char *) menu_atoms.sAtom[i].szSRem) + j) = '-';
								}

								j++;
							} while (*(((char *) menu_atoms.sAtom[i].szSRem) + j));

							ls = snprintf(line, MAX_CHAR_LINE * 2 - 1, "%s=%s\r\n",
												menu_atoms.sAtom[i].szSKey,
												menu_atoms.sAtom[i].szSRem);
						}
						else
						{
							ls = snprintf(line, MAX_CHAR_LINE * 2 - 1, "%s=%d\r\n",
												menu_atoms.sAtom[i].szSKey,
												menu_atoms.sAtom[i].nSVal);
						}
					}
				}
				memcpy(speedup + ci, line, ls);
				ci += ls;

				if (ci >= AHC_MENU_SETTING_TEMP_BUFFER_SIZE) {
				    RTNA_DBG_Str(0, FG_RED("--E-- ExportMenuAtom: Speedup buffer exceed to MAX_SPEEDUP_BUFFER_SIZE\r\n"));
				    //RTNA_DBG_Str(0, FG_RED("      System Break\r\n"));
                    //while (1);
				}
			}
		}
	}

	if (flag | TXT_VERSION)
    {
    	unsigned short	mj, mm, mi, mb, mt;
    	char			*mp;

    	AHC_GetFwVersion(&mj, &mm, &mi, &mb, &mt, &mp);
		ls = snprintf(line, MAX_CHAR_LINE * 2 - 1, ";;Version - %d-%d-%d-%d-%d, %s\r\n",
							mj, mm, mi, mb, mt,
							mp);
		memcpy(speedup + ci, line, ls);
		ci += ls;
    }

    AHC_FS_FileWrite(fd, (UINT8 *)speedup, ci, (UINT32*)&os);
    AHC_FS_FileClose(fd);

    return AHC_TRUE;
}

/*
 * MenuInfo Import to MENU_ATOMS
 */
AHC_BOOL ImportMenuInfo(MenuInfo *pmi)
{
	char	*p;
	int		i;

	p = (char*)pmi;

	for (i = 0; i < sizeof(MenuInfo); i++) {
		menu_atoms.sAtom[i].nSVal = *(p + i);
	}
	return AHC_TRUE;
}

/*
 *
 */
int ParamFun(SETTING_ATOM *pa, int idx, char *szVal, void *arg)
{
	char	*p; // point to MenuInfo
    int     v;

	/* pmi is old style menu struct (MenuInfo), here use it as char* to set value.
	 * BE CAREFUL the order of MenuInfo and SETTING_ATOM must be same! */
	p = (char *) arg;
	v = atoi(szVal);

	if (IN_RANGE(v,  (pa + idx)))
	{
		(pa + idx)->nSVal = v;

		if (idx  < sizeof(MenuInfo))
		{
			*(p + idx) = (char)(pa + idx)->nSVal;
		}
	}
	else
	{
		printc("--E-- %s: Out of range - Index-%d\r\n", __func__, idx);
	}

	return	0;
}

/*
 *
 */
int pf_DateTime(SETTING_ATOM *pa, int idx, char *szVal, void *arg)
{
	// Setup Date/Time
	unsigned short	y, m, d, hh, mm, ss;

	if (ParseDateTime(szVal, 1/*format*/, &y, &m, &d, &hh, &mm, &ss)) {
		if(y==0)
			AHC_SetClock(RTC_DEFAULT_YEAR, RTC_DEFAULT_MONTH, RTC_DEFAULT_DAY, 0, RTC_DEFAULT_HOUR, RTC_DEFAULT_MIN, RTC_DEFAULT_SEC, 0, 0);
		else
			AHC_SetClock(y, m, d, 0, hh, mm, ss, 0, 0);
	}
	return 1;
}

/*
 * BitRate = n (Mbits)
 */
int pf_BitRate(SETTING_ATOM *pa, int idx, char *szVal, void *arg)
{
	int		v;

	v = atoi(szVal);

	if (IN_RANGE(v,  (pa + idx)))
	{
        MMPS_3GPRECD_CONFIGS *pVideoConfig = MMPS_3GPRECD_GetConfig();

		(pa + idx)->nSVal = v;

	    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_1920x1088][VIDRECD_QUALITY_LOW]  = v * 1000000;
	    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_1920x1088][VIDRECD_QUALITY_MID]  = v * 1000000;
	    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_1920x1088][VIDRECD_QUALITY_HIGH] = v * 1000000;
	    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_1280x720][VIDRECD_QUALITY_LOW]   = v * 1000000;
	    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_1280x720][VIDRECD_QUALITY_MID]   = v * 1000000;
	    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_1280x720][VIDRECD_QUALITY_HIGH]  = v * 1000000;

	    return v;
	}

	return 0;
}

/*
 * LoopRec = 0:OFF, 1:ON,
 * IF loopRec exist in txt, the uiMOVClipTime will be OFF or 127 (refer VideoTime, AHC_GetVideoRecTimeLimit)
 */
int pf_LoopRec(SETTING_ATOM *pa, int idx, char *szVal, void *arg)
{
#if (MENU_TXT_VIDEOTIME == 1)
	int			v;
	MenuInfo	*p;

	v = atoi(szVal);

	if (IN_RANGE(v,  (pa + idx)))
	{
		(pa + idx)->nSVal = v;

		p = (MenuInfo*)arg;

		if (v == 0) {
			uiCycleRecord = 0;
			p->uiMOVClipTime = MOVIE_CLIP_TIME_NUM;
		}
		else
		{
			uiCycleRecord = 1;
			p->uiMOVClipTime = MOVIE_CLIP_TIME_NUM;	/* Make Out Of uiMOVClipTime Range to refer VideoTime */
		}
    }
#endif
	return 1;
}

/*
 * LoopRec must be 1:ON
 * VideoTime = unit is Minute,
 */
int pf_VideoTime(SETTING_ATOM *pa, int idx, char *szVal, void *arg)
{
#if	(MENU_TXT_VIDEOTIME == 1)
	int		v;
	UINT8	i;

	v = atoi(szVal);

	for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
	{
		if (strcmpi("VideoTime", menu_atoms.sAtom[i].szSKey) == 0)
			break;
	}

	if (menu_atoms.sAtom[i].szSKey == NULL)
		return 0;

	if (IN_RANGE(v,  (pa + idx))) {
		(pa + idx)->nSVal = v;
	} else {
		(pa + idx)->nSVal = menu_atoms.sAtom[i].nMin;
	}
#endif
	return 1;
}

int	Getfp_VideoTime(void)
{
	UINT8 i;

	for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
	{
		if (strcmpi("VideoTime", menu_atoms.sAtom[i].szSKey) == 0)
		{
			return menu_atoms.sAtom[i].nSVal * 60;
		}
	}

	return 0;
}

int Setpf_GSNR_THR(int value)
{
	UINT8 i;

	// menu_atoms.sAtom[i].szSRem
	for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
	{
		if (strcmpi(GSNR_PWRON_SENS_KEY, menu_atoms.sAtom[i].szSKey) == 0)
		{
			menu_atoms.sAtom[i].nSVal = value;
		}
	}

	return value;
}

int Getpf_GSNR_THR(void)
{
	UINT8 i;

	// menu_atoms.sAtom[i].szSRem
	for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
	{
		if (strcmpi(GSNR_PWRON_SENS_KEY, menu_atoms.sAtom[i].szSKey) == 0)
		{
			return menu_atoms.sAtom[i].nSVal;
		}
	}

	return 0;
}

int Setpf_WiFi(int value)
{
	UINT8 i;

	// menu_atoms.sAtom[i].szSRem
	for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
	{
		if (strcmpi(WIFI_KEY, menu_atoms.sAtom[i].szSKey) == 0)
		{
			if (IN_RANGE(value,  &menu_atoms.sAtom[i])) {
				menu_atoms.sAtom[i].nSVal = value;
			}
		}
	}

	return value;
}

int Getpf_WiFi(void)
{
	UINT8 i;

	// menu_atoms.sAtom[i].szSRem
	for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
	{
		if (strcmpi(WIFI_KEY, menu_atoms.sAtom[i].szSKey) == 0)
		{
			return menu_atoms.sAtom[i].nSVal;
		}
	}

	return 0;
}


int pf_General(SETTING_ATOM *pa, int idx, char *szVal, void *arg)
{
    int v;

    v = atoi(szVal);

    if (IN_RANGE(v, (pa + idx)))
    {
        (pa + idx)->nSVal = v;
    }
    else
    {
        (pa + idx)->nSVal = menu_atoms.sAtom[idx].nMin;
    }

    return 0;
}

/*
 * Reset Time
 * To let user input Date/Time when power kit on
 * ResetTime=1 or 0
 */
static int	_bResetTime = 0;

int pf_ResetTime(SETTING_ATOM *pa, int idx, char *szVal, void *arg)
{
	/* the setting need not store in menu_autom */
	_bResetTime = atoi(szVal);
	return _bResetTime;
}

int Getfp_ResetTime(void)
{
	return _bResetTime;
}

/*
 *
 */
int pf_LDWS_EnGet(void)
{
	UINT8 i;

	for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
	{
		if (strcmpi(LDWS_EN_KEY, menu_atoms.sAtom[i].szSKey) == 0)
		{
			return menu_atoms.sAtom[i].nSVal;
		}
	}

	return LDWS_EN_OFF;
}

int pf_LDWS_EnSet(AHC_BOOL value)
{
	UINT8 i;

	for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
	{
		if (strcmpi(LDWS_EN_KEY, menu_atoms.sAtom[i].szSKey) == 0)
		{
			if (IN_RANGE(value, &menu_atoms.sAtom[i]))
			{
			    UINT32 uiAdasFlag = 0;
                AHC_GetParam( PARAM_ID_ADAS, &uiAdasFlag );
                if( value == LDWS_EN_ON )
                {
                    uiAdasFlag |= AHC_ADAS_ENABLED_LDWS;
                }
                else
                {
                    uiAdasFlag &= ~AHC_ADAS_ENABLED_LDWS;
                }
                AHC_SetParam( PARAM_ID_ADAS, uiAdasFlag );
            
				menu_atoms.sAtom[i].nSVal = value;

				#if MENU_MOVIE_FCWS_LINK_WITH_LDWS
				if (value == LDWS_EN_ON)
					pf_FCWS_EnSet(FCWS_EN_ON);
				else
					pf_FCWS_EnSet(FCWS_EN_OFF);
				#endif
			}
		}
	}

	return 0;
}

/*
 *
 */
int pf_FCWS_EnGet(void)
{
#if MENU_MOVIE_FCWS_LINK_WITH_LDWS
    if (pf_LDWS_EnGet() == LDWS_EN_ON)
        return FCWS_EN_ON;
#else
	UINT8 i;

	for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
	{
		if (strcmpi(FCWS_EN_KEY, menu_atoms.sAtom[i].szSKey) == 0)
		{
			return menu_atoms.sAtom[i].nSVal;
		}
	}
#endif

	return FCWS_EN_OFF;
}

int pf_FCWS_EnSet(AHC_BOOL value)
{
	UINT8 i;

#if MENU_MOVIE_FCWS_LINK_WITH_LDWS
    if (pf_LDWS_EnGet() == LDWS_EN_ON)
        value = FCWS_EN_ON;
    else
        value = FCWS_EN_OFF;
#endif

	for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
	{
		if (strcmpi(FCWS_EN_KEY, menu_atoms.sAtom[i].szSKey) == 0)
		{
			if (IN_RANGE(value, &menu_atoms.sAtom[i]))
			{
			    UINT32 uiAdasFlag = 0;
                AHC_GetParam( PARAM_ID_ADAS, &uiAdasFlag );
                if( value == FCWS_EN_ON )
                {
                    uiAdasFlag |= AHC_ADAS_ENABLED_FCWS;
                }
                else
                {
                    uiAdasFlag &= ~AHC_ADAS_ENABLED_FCWS;
                }
                AHC_SetParam( PARAM_ID_ADAS, uiAdasFlag );
            
				menu_atoms.sAtom[i].nSVal = value;
			}
		}
	}

	return 0;
}

/*
 *
 */
int pf_SAG_EnGet(void)
{
	UINT8 i;

	for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
	{
		if (strcmpi(SAG_EN_KEY, menu_atoms.sAtom[i].szSKey) == 0)
		{
			return menu_atoms.sAtom[i].nSVal;
		}
	}

	return SAG_EN_OFF;
}

int pf_SAG_EnSet(AHC_BOOL value)
{
	UINT8 i;

	for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
	{
		if (strcmpi(SAG_EN_KEY, menu_atoms.sAtom[i].szSKey) == 0)
		{
			if (IN_RANGE(value, &menu_atoms.sAtom[i]))
			{
			    UINT32 uiAdasFlag = 0;
                AHC_GetParam( PARAM_ID_ADAS, &uiAdasFlag );
                if( value == SAG_EN_ON )
                {
                    uiAdasFlag |= AHC_ADAS_ENABLED_SAG;
                }
                else
                {
                    uiAdasFlag &= ~AHC_ADAS_ENABLED_SAG;
                }
                AHC_SetParam( PARAM_ID_ADAS, uiAdasFlag );

				menu_atoms.sAtom[i].nSVal = value;
			}
		}
	}

	return 0;
}

/*
 *
 */
int pf_HDR_EnGet(void)
{
	UINT8 i;

	for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
	{
		if (strcmpi(HDR_EN_KEY, menu_atoms.sAtom[i].szSKey) == 0)
		{
			return menu_atoms.sAtom[i].nSVal;
		}
	}

	return HDR_EN_OFF;
}

int pf_HDR_EnSet(AHC_BOOL value)
{
	UINT8 i;

	for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
	{
		if (strcmpi(HDR_EN_KEY, menu_atoms.sAtom[i].szSKey) == 0)
		{
			if (IN_RANGE(value, &menu_atoms.sAtom[i]))
			{
				menu_atoms.sAtom[i].nSVal = value;
			}
		}
	}

	return 0;
}

int pf_SLOWMOTION_EnGet(void)
{
	UINT8 i;

	for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
	{
		if (strcmpi(SLOWMOTION_EN_KEY, menu_atoms.sAtom[i].szSKey) == 0)
		{
			return menu_atoms.sAtom[i].nSVal;
		}
	}

	return 0;
}

int pf_SLOWMOTION_EnSet(SLOWMOTION_SETTING value)
{
	UINT8 i;

	for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
	{
		if (strcmpi(SLOWMOTION_EN_KEY, menu_atoms.sAtom[i].szSKey) == 0)
		{
			if (IN_RANGE(value, &menu_atoms.sAtom[i]))
			{
				menu_atoms.sAtom[i].nSVal = value;
				printc(FG_BLUE("*** menu_atoms.sAtom[%d].nSVal = %d \n"),i,value);
			}
		}
	}

	return 0;
}

/*
 *
 */
int pf_WNR_EnGet(void)
{
	UINT8 i;

	for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
	{
		if (strcmpi(WNR_EN_KEY, menu_atoms.sAtom[i].szSKey) == 0)
		{
			return menu_atoms.sAtom[i].nSVal;
		}
	}

	return HDR_EN_OFF;
}

int pf_WNR_EnSet(AHC_BOOL value)
{
	UINT8 i;

	for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
	{
		if (strcmpi(WNR_EN_KEY, menu_atoms.sAtom[i].szSKey) == 0)
		{
			if (IN_RANGE(value, &menu_atoms.sAtom[i]))
			{
				menu_atoms.sAtom[i].nSVal = value;
			}
		}
	}

	return 0;
}

/*
 *
 */
int pf_DriverID(SETTING_ATOM *pa, int idx, char *szVal, void *arg)
{
#if MENU_GENERAL_DRIVER_ID_SETTING_EN
	int i;

	// transfer '-'  to ' '
	for (i = 0; i < sizeof(Driverid) - 1; i++)
	{
		if (*(((char *)szVal) + i) == '-')
		{
			*(((char *)szVal) + i) = ' ';
		}
	}
	*(((char *)szVal) + sizeof(Driverid) - 1) = '\0'; // TBC

	// (pa + idx)->szSRem => menu_atoms.sAtom[idx].szSRem
	MEMCPY((char *)((pa + idx)->szSRem), (char *)szVal, sizeof(Driverid) - 1);
	*(((char *)((pa + idx)->szSRem)) + sizeof(Driverid) - 1) = '\0'; // TBC

	// Driverid
	MEMCPY(Driverid, (char *)((pa + idx)->szSRem), sizeof(Driverid) - 1);
	*(((char *)Driverid) + sizeof(Driverid) - 1) = '\0'; // TBC
#endif

	return 1;
}

/*
 *
 */
int pf_SetDriverID(void)
{
#if MENU_GENERAL_DRIVER_ID_SETTING_EN
	int i;

	// Driverid
	*(((char *)tmpDriverId) + sizeof(Driverid) - 1) = '\0'; // TBC
	MEMCPY(Driverid, tmpDriverId, sizeof(Driverid) - 1);
	*(((char *)Driverid) + sizeof(Driverid) - 1) = '\0'; // TBC

	// menu_atoms.sAtom[i].szSRem
	for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
	{
		if (strcmpi(DRIVER_ID_KEY, menu_atoms.sAtom[i].szSKey) == 0)
		{
			break;
		}
	}
	MEMCPY((char *)menu_atoms.sAtom[i].szSRem, Driverid, sizeof(Driverid) - 1);
	*(((char *)menu_atoms.sAtom[i].szSRem) + sizeof(Driverid) - 1) = '\0'; // TBC
#endif

	return 1;
}

/*
 *
 */
void DriverIDReset(void)
{
#if MENU_GENERAL_DRIVER_ID_SETTING_EN
	// tmpDriverId[0~8], set default value, DRVIER_ID_DEFAULT is defined in Oem_Menu.h
	MEMCPY(tmpDriverId, (char *)DRIVER_ID_DEFAULT, sizeof(Driverid) - 1);
	*(((char *)tmpDriverId) + sizeof(Driverid) - 1) = '\0'; // TBC

	// Driverid[0~8], set default value, DRVIER_ID_DEFAULT is defined in Oem_Menu.h
	MEMCPY(Driverid, (char *)DRIVER_ID_DEFAULT, sizeof(Driverid) - 1);
	*(((char *)Driverid) + sizeof(Driverid) - 1) = '\0'; // TBC
#endif
}

/*
 *
 */
int pf_TimeStampLogoTXT(SETTING_ATOM *pa, int idx, char *szVal, void *arg)
{
	if (strlen(szVal) >= CUSTOMER_NAME_MAX_LENGTH) {
    	printc("--E-- %s: LOGO string too long - %s\r\n", __func__, szVal);
		return 0;
	}

	STRCPY((void *) (pa+idx)->szSRem, szVal);
	printc("New TimeStamp Logo TXT is %s\r\n", (pa+idx)->szSRem);
	return 1;
}

static int Menu_Setting_Get_VideoSticker(int icur_val, va_list *ap)
{
    UINT32 *pSticker;

    pSticker = va_arg(*ap, UINT32 *);
    *pSticker = (UINT32) &model;

    return AHC_TRUE;
}

/*
 *
 */
static int Menu_Setting_Get_TVSystem(int icur_val, va_list *ap)
{
    UINT8* pctv_sys;

    pctv_sys = va_arg(*ap, UINT8*);

    switch(icur_val){
        case TV_SYSTEM_PAL:
            *pctv_sys = COMMON_TV_SYSTEM_PAL;
        break;

        //case TV_SYSTEM_NTSC:
        default:
            *pctv_sys = COMMON_TV_SYSTEM_NTSC;
        break;
    }

    return AHC_TRUE;
}

/*
 *
 */
static int Menu_Setting_Get_MicSensitivity(int icur_val, va_list *ap)
{
    UINT8* pbyDGain;        // Get next argument (note: typedef must be correct)
    UINT8* pbyAGain;        // Get next argument (note: typedef must be correct)
    UINT8* pbyBoostup;      // Get next argument (note: typedef must be correct)
    UINT32 ulDGain;
    UINT32 ulAGain;
    
    pbyDGain   = va_arg(*ap, UINT8*);
    pbyAGain   = va_arg(*ap, UINT8*);
    pbyBoostup = va_arg(*ap, UINT8*);

	AHC_GetParam(PARAM_ID_AUDIO_IN_GAIN, &ulAGain);
	AHC_GetParam(PARAM_ID_AUDIO_IN_DIGITAL_GAIN, &ulDGain);

    if (pbyDGain)
        *pbyDGain = ulDGain;
        
    if (pbyAGain)
        *pbyAGain = ulAGain;

    if (pbyBoostup) {
        if ((0 == ulDGain) && (0 == ulAGain))
            *pbyBoostup = (UINT8) MMP_FALSE;
        else
            *pbyBoostup = (UINT8) MMP_TRUE;
    }

    return AHC_TRUE;
}

/*
 *
 */
AHC_BOOL Menu_Setting_Set_MicSensitivity(UINT8 val)
{
    if( val == MIC_SEN_STANDARD) {
    	AHC_SetParam(PARAM_ID_AUDIO_IN_GAIN, AHC_DEFAULT_VR_MIC_AGAIN);
    	AHC_SetParam(PARAM_ID_AUDIO_IN_DIGITAL_GAIN, DEFAULT_ADC_DIGITAL_GAIN);
    }
    else {
    	AHC_SetParam(PARAM_ID_AUDIO_IN_GAIN, AHC_DEFAULT_VR_MIC_AGAIN);
    	AHC_SetParam(PARAM_ID_AUDIO_IN_DIGITAL_GAIN, 0x1F);
    }
    
    return AHC_TRUE;
}

/*
 *
 */
static int Menu_Setting_Get_GPS_RecordEnable(int icur_val, va_list *ap)
{
    UINT8* pGPS_record_en;

    pGPS_record_en = va_arg(*ap, UINT8*);          // Get next argument (note: typedef must be correct)

    switch(icur_val){
    case RECODE_GPS_IN_LOG_VIDEO:
        *pGPS_record_en = COMMON_GPS_REC_INFO_LOG_VIDEO;
        break;
    case RECODE_GPS_IN_LOG:
        *pGPS_record_en = COMMON_GPS_REC_INFO_LOG_ONLY;
        break;
    case RECODE_GPS_IN_VIDEO:
        *pGPS_record_en = COMMON_GPS_REC_INFO_VIDEO_ONLY;
        break;
    //case RECODE_GPS_OFF:
    default:
        *pGPS_record_en = COMMON_GPS_REC_INFO_OFF;
        break;
    }

    return AHC_TRUE;
}

/*
 *
 */
static int Menu_Setting_Get_Flicker(int icur_val, va_list *ap)
{
    UINT8* pFlicker;

    pFlicker = va_arg(*ap, UINT8*);          // Get next argument (note: typedef must be correct)

    switch(icur_val){
    case FLICKER_50HZ:
        *pFlicker = COMMON_FLICKER_50HZ;
        break;
    //case FLICKER_60HZ:
    default:
        *pFlicker = COMMON_FLICKER_60HZ;
        break;
    }

    return AHC_TRUE;
}

/*
 *
 */
static int Menu_Setting_Get_General(int icur_val, va_list *ap)
{
    UINT32* pSetting;

    pSetting = va_arg(*ap, UINT32*);        // Get next argument (note: typedef must be correct)
    *pSetting = (UINT32) icur_val;

    return AHC_TRUE;
}

static int Menu_Setting_Get_VR_ClipTime(int icur_val, va_list *ap)
{
    UINT32* pVR_ClipTime;

    pVR_ClipTime = va_arg(*ap, UINT32*);        // Get next argument (note: typedef must be correct)

    switch (icur_val){
        case MOVIE_CLIP_TIME_6SEC:
            *pVR_ClipTime = COMMON_MOVIE_CLIP_TIME_6SEC;
            break;
        case MOVIE_CLIP_TIME_1MIN:
            *pVR_ClipTime = COMMON_MOVIE_CLIP_TIME_1MIN;
            break;
        case MOVIE_CLIP_TIME_2MIN:
            *pVR_ClipTime = COMMON_MOVIE_CLIP_TIME_2MIN;
            break;
        case MOVIE_CLIP_TIME_3MIN:
            *pVR_ClipTime = COMMON_MOVIE_CLIP_TIME_3MIN;
            break;
        case MOVIE_CLIP_TIME_5MIN:
            *pVR_ClipTime = COMMON_MOVIE_CLIP_TIME_5MIN;
            break;
        case MOVIE_CLIP_TIME_10MIN:
            *pVR_ClipTime = COMMON_MOVIE_CLIP_TIME_10MIN;
            break;
        case MOVIE_CLIP_TIME_25MIN:
            *pVR_ClipTime = COMMON_MOVIE_CLIP_TIME_25MIN;
            break;
        case MOVIE_CLIP_TIME_30MIN:
            *pVR_ClipTime = COMMON_MOVIE_CLIP_TIME_30MIN;
            break;
        case MOVIE_CLIP_TIME_OFF:
        default:
            *pVR_ClipTime = COMMON_MOVIE_CLIP_TIME_OFF;
            break;
    }

    return AHC_TRUE;
}

static int Menu_Setting_Get_Still_Quality(int icur_val, va_list *ap)
{
    UINT32* pRetVal;

    pRetVal = va_arg(*ap, UINT32*);        // Get next argument (note: typedef must be correct)

    switch (icur_val){
        case QUALITY_SUPER_FINE:
            *pRetVal = MMP_DSC_CAPTURE_HIGH_QUALITY;
            break;
        case QUALITY_FINE:
            *pRetVal = MMP_DSC_CAPTURE_NORMAL_QUALITY;
            break;
        default:
            *pRetVal = MMP_DSC_CAPTURE_HIGH_QUALITY;
            break;
    }

    return AHC_TRUE;
}

int Menu_Setting_Get_Scene(int icur_val, va_list *ap)
{
    UINT8* pcsc;
    pcsc = va_arg(*ap, UINT8*);
    switch(icur_val){
#if (MENU_MANUAL_SCENE_TWILIGHT_EN)
        case SCENE_TWILIGHT:
            *pcsc= COMMON_SCENE_TWILIGHT;
        break;
#endif
        case SCENE_AUTO:
        default:
            *pcsc= COMMON_SCENE_AUTO;
        break;
    	}
    return AHC_TRUE;
}


static int Menu_Setting_Get_Metering(int icur_val, va_list *ap)
{
    UINT8* pcmeter;
    pcmeter = va_arg(*ap, UINT8*);
    switch(icur_val){
        case METERING_MULTI:
            *pcmeter= COMMON_METERING_MULTI;
            break;
        case METERING_CENTER:
            *pcmeter= COMMON_METERING_CENTER;
            break;
        case METERING_CENTER_SPOT:
            *pcmeter= COMMON_METERING_CENTER_SPOT;
            break;
        case METERING_NUM:
            *pcmeter= COMMON_METERING_NUM;
            break;
        case METERING_AVERAGE:
            *pcmeter= COMMON_METERING_AVERAGE;
            break;
        case METERING_MATRIX:
            *pcmeter= COMMON_METERING_MATRIX;
            break;
        case METERING_WEIGHTED:
            *pcmeter= COMMON_METERING_WEIGHTED;
            break;
        default:
            *pcmeter= COMMON_METERING_DEFAULT;
            break;
	}

    return AHC_TRUE;
}

static int Menu_Setting_Get_EV(int icur_val, va_list *ap)
{
    UINT8* pcev;

    pcev = va_arg(*ap, UINT8*);

    switch(icur_val){
        case EVVALUE_N20:
            *pcev= COMMON_EVVALUE_N20;
            break;
        case EVVALUE_N17:
            *pcev= COMMON_EVVALUE_N17;
            break;
        case EVVALUE_N13:
            *pcev= COMMON_EVVALUE_N13;
            break;
        case EVVALUE_N10:
            *pcev= COMMON_EVVALUE_N10;
            break;
        case EVVALUE_N07:
            *pcev= COMMON_EVVALUE_N07;
            break;
        case EVVALUE_N03:
            *pcev= COMMON_EVVALUE_N03;
            break;
        case EVVALUE_00:
            *pcev= COMMON_EVVALUE_00;
            break;
        case EVVALUE_P03:
            *pcev= COMMON_EVVALUE_P03;
            break;
        case EVVALUE_P07:
            *pcev= COMMON_EVVALUE_P07;
            break;
        case EVVALUE_P10:
            *pcev= COMMON_EVVALUE_P10;
            break;
        case EVVALUE_P13:
            *pcev= COMMON_EVVALUE_P13;
            break;
        case EVVALUE_P17:
            *pcev= COMMON_EVVALUE_P17;
            break;
        case EVVALUE_P20:
            *pcev= COMMON_EVVALUE_P20;
            break;
        case EVVALUE_NUM:
            *pcev= COMMON_EVVALUE_NUM;
            break;
        default:
            *pcev= COMMON_EVVALUE_DEFAULT;
            break;
    }

    return AHC_TRUE;
}

int Menu_Setting_Get_ISO(int icur_val, va_list *ap)
{
    UINT8* pciso;

    pciso = va_arg(*ap, UINT8*);

    switch(icur_val){
        case ISO_AUTO:
            *pciso= COMMON_ISO_AUTO;
            break;
        case ISO_100:
            *pciso= COMMON_ISO_100;
            break;
        case ISO_200:
            *pciso= COMMON_ISO_200;
            break;
        case ISO_400:
            *pciso= COMMON_ISO_400;
            break;
        case ISO_800:
            *pciso= COMMON_ISO_800;
            break;
        case ISO_1600:
            *pciso= COMMON_ISO_1600;
            break;
        case ISO_3200:
            *pciso= COMMON_ISO_3200;
            break;
        case ISO_NUM:
            *pciso= COMMON_ISO_NUM;
            break;
        default:
            *pciso= COMMON_ISO_DEFAULT;
            break;
    }

    return AHC_TRUE;
}

int Menu_Setting_Get_WB(int icur_val, va_list *ap)
{
    UINT8* pcwb;

    pcwb = va_arg(*ap, UINT8*);

    switch(icur_val){
    	case WB_AUTO:
            *pcwb= COMMON_WB_AUTO;
            break;
    	case WB_DAYLIGHT:
            *pcwb= COMMON_WB_DAYLIGHT;
            break;
    	case WB_CLOUDY:
            *pcwb= COMMON_WB_CLOUDY;
            break;
    	case WB_FLUORESCENT1:
            *pcwb= COMMON_WB_FLUORESCENT1;
            break;
    	case WB_FLUORESCENT2:
            *pcwb= COMMON_WB_FLUORESCENT2;
            break;
    	case WB_FLUORESCENT3:
            *pcwb= COMMON_WB_FLUORESCENT3;
            break;
    	case WB_INCANDESCENT:
            *pcwb= COMMON_WB_INCANDESCENT;
            break;
    	case WB_FLASH:
            *pcwb= COMMON_WB_FLASH;
            break;
    	case WB_ONEPUSH:
            *pcwb= COMMON_WB_ONEPUSH;
            break;
    	case WB_ONE_PUSH_SET:
            *pcwb= COMMON_WB_ONE_PUSH_SET;
            break;
    	case WB_NUM:
            *pcwb= COMMON_WB_NUM;
            break;
        default:
            *pcwb= COMMON_WB_DEFAULT;
            break;
	}

    return AHC_TRUE;
}

/*
 *
 */
int pf_General_EnSet(char *itemKey, INT32 value)
{
	UINT8 i;

	for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
	{
		if (strcmpi(itemKey, menu_atoms.sAtom[i].szSKey) == 0)
		{
			if (IN_RANGE(value, &menu_atoms.sAtom[i]))
			{
				menu_atoms.sAtom[i].nSVal = value;
				return i;
			}
			else {
			    printc("%s:%s - out of range, %d\r\n", __func__, itemKey, value);
			}
		}
	}

	return -1;
}

int pf_General_EnGet(char *itemKey, INT32 *value)
{
	UINT8 i;

	for (i = 0; menu_atoms.sAtom[i].szSKey != NULL; i++)
	{
		if (strcmpi(itemKey, menu_atoms.sAtom[i].szSKey) == 0)
		{
			*value = (INT32) menu_atoms.sAtom[i].nSVal;
			return i;
		}
	}

    printc("Not found %s\r\n", itemKey);

	return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////////////////
AHC_BOOL AHC_Menu_SettingSetCB( char *cmenu_string, ... )
{
    AHC_BOOL retVal = AHC_FALSE;
    UINT32 uloop;
    va_list ap;

    for (uloop = 0; menu_atoms.sAtom[uloop].szSKey != NULL; uloop++){
        //case sensitive
        if (/*strcmpi*/strcmp(cmenu_string, menu_atoms.sAtom[uloop].szSKey) == 0){
            va_start(ap, cmenu_string);     // Set pointer to 1st argument of "Varaible List"
            if(menu_atoms.sAtom[uloop].fnSet != NULL){
                //menu_atoms.sAtom[uloop].fnSet(menu_atoms.sAtom[uloop].nSVal, &ap);
                //TBD
            }
            va_end(ap);
            return retVal;
        }
    }

    return AHC_FALSE;
}

AHC_BOOL AHC_Menu_SettingGetCB( char *cmenu_string, ... )
{
    AHC_BOOL retVal;
    UINT32 uloop;
    va_list ap;

    for (uloop = 0; menu_atoms.sAtom[uloop].szSKey != NULL; uloop++){
        //case sensitive
        if (/*strcmpi*/strcmp(cmenu_string, menu_atoms.sAtom[uloop].szSKey) == 0){
            va_start(ap, cmenu_string);     // Set pointer to 1st argument of "Varaible List"
            if(menu_atoms.sAtom[uloop].fnGet != NULL){
                retVal = menu_atoms.sAtom[uloop].fnGet(menu_atoms.sAtom[uloop].nSVal, &ap);
            }
            va_end(ap);
            return retVal;
        }
    }

    return AHC_FALSE;
}

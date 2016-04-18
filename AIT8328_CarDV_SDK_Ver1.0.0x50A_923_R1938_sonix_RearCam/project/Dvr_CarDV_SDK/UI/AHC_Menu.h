//==============================================================================
//
//  File        : AHC_Menu.h
//  Description : INCLUDE File for the AHC Menu function porting.
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================

#ifndef _AHC_MENU_H_
#define _AHC_MENU_H_

/*===========================================================================
 * Include files
 *===========================================================================*/

#include "customer_config.h"
#include "AHC_common.h"
#include "config_fw.h"
#include "MenuSetting.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/

#ifndef FW_RUN_CALI_MODE_EN
#define FW_RUN_CALI_MODE_EN         (0)    // FW run calibration mode (special mode for calibration)
#endif

// Key pressed
#define	FLGKEY_UP					0x00000001
#define	FLGKEY_DOWN					0x00000002
#define	FLGKEY_LEFT					0x00000004
#define	FLGKEY_RIGHT        		0x00000008
#define	FLGKEY_SET          		0x00000010
#define	FLGKEY_ZOOMT        		0x00000020
#define	FLGKEY_ZOOMW        		0x00000040
#define	FLGKEY_MENU					0x00000080
#define	FLGKEY_MODE					0x00000100
#define	FLGKEY_RECORD       		0x00000200
#define	FLGKEY_BROWSER      		0x00000400
#define	FLGKEY_NONE					0x00000800
#define	FLGKEY_PRESS_MASK   		0x0000FFFF

extern UINT32 KeyFlag;

//Keypad Definition
#define BUTTON_NONE              	KEYPAD_NONE

#if !defined(CAR_DV)

#define BUTTON_PLAYBACK_PRESS    	KEYPAD_S12_PRESS
#define BUTTON_TELE_PRESS        	KEYPAD_S07_PRESS
#define BUTTON_WIDE_PRESS        	KEYPAD_S10_PRESS
#define BUTTON_MODE_PRESS       	KEYPAD_S11_PRESS

#define BUTTON_UP_PRESS          	KEYPAD_S04_PRESS
#define BUTTON_DOWN_PRESS        	KEYPAD_S01_PRESS
#define BUTTON_LEFT_PRESS        	KEYPAD_S02_PRESS
#define BUTTON_RIGHT_PRESS       	KEYPAD_S03_PRESS

#define BUTTON_MENU_PRESS        	KEYPAD_S05_PRESS
#define BUTTON_SET_PRESS         	KEYPAD_S06_PRESS
#define BUTTON_REC_PRESS         	KEYPAD_S13_PRESS
#define BUTTON_FOCUS_PRESS       	KEYPAD_S09_PRESS
#define BUTTON_SHUTTER_PRESS     	KEYPAD_S08_PRESS
#define BUTTON_FOCUS_SHUTTER_PRESS  KEYPAD_S08S09_PRESS

#define BUTTON_USB_DETECTED      	USB_CABLE_IN
#define BUTTON_SD_DETECTED       	SD_CARD_IN
#define BUTTON_TV_DETECTED       	TV_CABLE_IN
#define BUTTON_HDMI_DETECTED     	HDMI_CABLE_IN
#define BUTTON_USB_B_DEVICE_DETECTED      	USB_B_DEVICE_IN
#define BUTTON_TV_DECODER_SRC_TYPE      	TV_DECODER_SRC_TYPE_CHANGED

#if (POWER_KEY_DETECTION == POWER_KEY_DETECT_GPIO)
#define BUTTON_POWER_PRESS       	KEYPAD_POWER_PRESS
#else
#define BUTTON_POWER_PRESS       	DEVICE_POWER_PRESS
#endif

#define BUTTON_LCD_ROTATE        	DEVICE_LCD_INV
#define BUTTON_COVER_OPEN        	DEVICE_LCD_OPEN
#define BUTTON_DISP_PRESS			KEYPAD_S16_PRESS

#define BUTTON_PLAYBACK_REL      	KEYPAD_S12_RELEASE
#define BUTTON_TELE_REL          	KEYPAD_S07_RELEASE
#define BUTTON_WIDE_REL          	KEYPAD_S10_RELEASE
#define BUTTON_MODE_REL          	KEYPAD_S11_RELEASE

#define BUTTON_UP_REL            	KEYPAD_S04_RELEASE
#define BUTTON_DOWN_REL          	KEYPAD_S01_RELEASE
#define BUTTON_LEFT_REL          	KEYPAD_S02_RELEASE
#define BUTTON_RIGHT_REL         	KEYPAD_S03_RELEASE

#define BUTTON_MENU_REL          	KEYPAD_S05_RELEASE
#define BUTTON_SET_REL           	KEYPAD_S06_RELEASE
#define BUTTON_REC_REL           	KEYPAD_S13_RELEASE
#define BUTTON_SHUTTER_REL       	KEYPAD_S08_RELEASE
#define BUTTON_FOCUS_REL         	KEYPAD_S09_RELEASE

#define BUTTON_USB_REMOVED       	USB_CABLE_OUT
#define BUTTON_SD_REMOVED        	SD_CARD_OUT
#define BUTTON_TV_REMOVED        	TV_CABLE_OUT
#define BUTTON_HDMI_REMOVED      	HDMI_CABLE_OUT
#define BUTTON_POWER_REL		 	KEYPAD_POWER_RELEASE
#define BUTTON_USB_B_DEVICE_REMOVED      	USB_B_DEVICE_OUT

#define BUTTON_LCD_NORMAL        	DEVICE_LCD_NOR
#define BUTTON_COVER_CLOSED      	DEVICE_LCD_COVERED
#define BUTTON_DISP_REL		 	 	KEYPAD_S16_RELEASE

/* Long Press */
#define BUTTON_UP_LPRESS         	KEYPAD_S03_LPRESS
#define BUTTON_DOWN_LPRESS       	KEYPAD_S04_LPRESS
#define BUTTON_LEFT_LPRESS       	KEYPAD_S05_LPRESS
#define BUTTON_RIGHT_LPRESS      	KEYPAD_S06_LPRESS
#define BUTTON_SET_LPRESS        	KEYPAD_S07_LPRESS

#define BUTTON_MENU_LPRESS       	KEYPAD_S02_LPRESS
#define BUTTON_MODE_LPRESS       	KEYPAD_S11_LPRESS
#define BUTTON_REC_LPRESS        	KEYPAD_S01_LPRESS
#define BUTTON_PLAYBACK_LPRESS   	KEYPAD_S08_LPRESS

#define BUTTON_POWER_LPRESS   	 	KEYPAD_POWER_LPRESS
#define BUTTON_SHUTTER_LPRESS	 	KEYPAD_S13_LPRESS
#define BUTTON_FUNC1_LPRESS		 	KEYPAD_S13_LPRESS
#define BUTTON_MUTE_LPRESS   	 	KEYPAD_S16_LPRESS

#define BUTTON_TELE_LPRESS       	KEYPAD_S07_LPRESS
#define BUTTON_WIDE_LPRESS       	KEYPAD_S10_LPRESS

/* Long Release */
#define BUTTON_UP_LREL 			 	KEYPAD_S03_LRELEASE
#define BUTTON_DOWN_LREL		 	KEYPAD_S04_LRELEASE
#define BUTTON_LEFT_LREL         	KEYPAD_S05_LRELEASE
#define BUTTON_RIGHT_LREL      	 	KEYPAD_S06_LRELEASE
#define BUTTON_SET_LREL          	KEYPAD_S07_LRELEASE

#define BUTTON_MENU_LREL         	KEYPAD_S02_LRELEASE
#define BUTTON_MODE_LREL         	KEYPAD_S11_LRELEASE
#define BUTTON_REC_LREL        	 	KEYPAD_S01_LRELEASE
#define BUTTON_PLAYBACK_LREL   	 	KEYPAD_S08_LRELEASE

#define BUTTON_POWER_LREL   	 	KEYPAD_POWER_LRELEASE
#define BUTTON_SHUTTER_LREL	 	 	KEYPAD_S13_LRELEASE
#define BUTTON_FUNC1_LREL		 	KEYPAD_S13_LRELEASE
#define BUTTON_MUTE_LREL   	 	 	KEYPAD_S16_LRELEASE

#define BUTTON_TELE_LREL      	 	KEYPAD_S07_LRELEASE
#define BUTTON_WIDE_LREL       	 	KEYPAD_S10_LRELEASE

#else//defined(CAR_DV)

/* Short Press */
#define BUTTON_UP_PRESS          	KEY_PRESS_UP
#define BUTTON_DOWN_PRESS        	KEY_PRESS_DOWN
#define BUTTON_LEFT_PRESS        	KEY_PRESS_LEFT
#define BUTTON_RIGHT_PRESS       	KEY_PRESS_RIGHT
#define BUTTON_SET_PRESS         	KEY_PRESS_OK
#define BUTTON_MENU_PRESS        	KEY_PRESS_MENU
#define BUTTON_MODE_PRESS        	KEY_PRESS_MODE
#define BUTTON_REC_PRESS         	KEY_PRESS_REC
#define BUTTON_PLAYBACK_PRESS    	KEY_PRESS_PLAY
#define BUTTON_POWER_PRESS       	KEY_PRESS_POWER

#define BUTTON_TELE_PRESS        	KEY_PRESS_TELE
#define BUTTON_WIDE_PRESS        	KEY_PRESS_WIDE
#define BUTTON_SOS_PRESS    	 	KEY_PRESS_SOS
#define BUTTON_MUTE_PRESS        	KEY_PRESS_MUTE
#define BUTTON_CAPTURE_PRESS     	KEY_PRESS_CAPTURE
#define BUTTON_FUNC1_PRESS		 	KEY_PRESS_FUNC1
#define BUTTON_FUNC2_PRESS       	KEY_PRESS_FUNC2
#define BUTTON_FUNC3_PRESS       	KEY_PRESS_FUNC3

#define BUTTON_USB_DETECTED      	USB_CABLE_IN
#define BUTTON_SD_DETECTED       	SD_CARD_IN
#define BUTTON_TV_DETECTED       	TV_CABLE_IN
#define BUTTON_HDMI_DETECTED     	HDMI_CABLE_IN
#define BUTTON_LCD_ROTATE        	DEVICE_LCD_INV
#define BUTTON_COVER_OPEN        	DEVICE_LCD_OPEN
#define BUTTON_DC_CABLE_IN       	DC_CABLE_IN
#define BUTTON_CUS_SW1_ON       	CUS_SW1_ON
#define BUTTON_CUS_SW2_ON       	CUS_SW2_ON
#define BUTTON_USB_B_DEVICE_DETECTED      	USB_B_DEVICE_IN
#define BUTTON_TV_DECODER_SRC_TYPE      	TV_DECODER_SRC_TYPE_CHANGED

/* Short Release */
#define BUTTON_UP_REL          	 	KEY_REL_UP
#define BUTTON_DOWN_REL        	 	KEY_REL_DOWN
#define BUTTON_LEFT_REL        	 	KEY_REL_LEFT
#define BUTTON_RIGHT_REL      	 	KEY_REL_RIGHT
#define BUTTON_SET_REL         	 	KEY_REL_OK
#define BUTTON_MENU_REL        	 	KEY_REL_MENU
#define BUTTON_MODE_REL        	 	KEY_REL_MODE
#define BUTTON_REC_REL        	 	KEY_REL_REC
#define BUTTON_PLAYBACK_REL   	 	KEY_REL_PLAY
#define BUTTON_POWER_REL		 	KEY_REL_POWER

#define BUTTON_TELE_REL        	 	KEY_REL_TELE
#define BUTTON_WIDE_REL        	 	KEY_REL_WIDE
#define BUTTON_SOS_REL    	 		KEY_REL_SOS
#define BUTTON_MUTE_REL          	KEY_REL_MUTE
#define BUTTON_CAPTURE_REL       	KEY_REL_CAPTURE
#define BUTTON_FUNC1_REL		 	KEY_REL_FUNC1
#define BUTTON_FUNC2_REL         	KEY_REL_FUNC2
#define BUTTON_FUNC3_REL         	KEY_REL_FUNC3

#define BUTTON_USB_REMOVED      	USB_CABLE_OUT
#define BUTTON_SD_REMOVED        	SD_CARD_OUT
#define BUTTON_TV_REMOVED        	TV_CABLE_OUT
#define BUTTON_HDMI_REMOVED      	HDMI_CABLE_OUT
#define BUTTON_LCD_NORMAL        	DEVICE_LCD_NOR
#define BUTTON_COVER_CLOSED      	DEVICE_LCD_COVERED
#define BUTTON_DC_CABLE_OUT      	DC_CABLE_OUT
#define BUTTON_CUS_SW1_OFF      	CUS_SW1_OFF
#define BUTTON_CUS_SW2_OFF      	CUS_SW2_OFF
#define BUTTON_USB_B_DEVICE_REMOVED      	USB_B_DEVICE_OUT

#define	BUTTON_DEVICES_READY		DEVICES_READY

/* Long Press */
#define BUTTON_UP_LPRESS         	KEY_LPRESS_UP
#define BUTTON_DOWN_LPRESS       	KEY_LPRESS_DOWN
#define BUTTON_LEFT_LPRESS       	KEY_LPRESS_LEFT
#define BUTTON_RIGHT_LPRESS      	KEY_LPRESS_RIGHT
#define BUTTON_SET_LPRESS        	KEY_LPRESS_OK
#define BUTTON_MENU_LPRESS       	KEY_LPRESS_MENU
#define BUTTON_MODE_LPRESS       	KEY_LPRESS_MODE
#define BUTTON_REC_LPRESS        	KEY_LPRESS_REC
#define BUTTON_PLAYBACK_LPRESS   	KEY_LPRESS_PLAY
#define BUTTON_POWER_LPRESS   	 	KEY_LPRESS_POWER

#define BUTTON_TELE_LPRESS       	KEY_LPRESS_TELE
#define BUTTON_WIDE_LPRESS       	KEY_LPRESS_WIDE
#define BUTTON_SOS_LPRESS    	 	KEY_LPRESS_SOS
#define BUTTON_MUTE_LPRESS       	KEY_LPRESS_MUTE
#define BUTTON_CAPTURE_LPRESS    	KEY_LPRESS_CAPTURE
#define BUTTON_FUNC1_LPRESS		 	KEY_LPRESS_FUNC1
#define BUTTON_FUNC2_LPRESS      	KEY_LPRESS_FUNC2
#define BUTTON_FUNC3_LPRESS      	KEY_LPRESS_FUNC3

/* Long Release */
#define BUTTON_UP_LREL 			 	KEY_LREL_UP
#define BUTTON_DOWN_LREL		 	KEY_LREL_DOWN
#define BUTTON_LEFT_LREL         	KEY_LREL_LEFT
#define BUTTON_RIGHT_LREL      	 	KEY_LREL_RIGHT
#define BUTTON_SET_LREL          	KEY_LREL_OK
#define BUTTON_MENU_LREL         	KEY_LREL_MENU
#define BUTTON_MODE_LREL         	KEY_LREL_MODE
#define BUTTON_REC_LREL        	 	KEY_LREL_REC
#define BUTTON_PLAYBACK_LREL   	 	KEY_LREL_PLAY
#define BUTTON_POWER_LREL   	 	KEY_LREL_POWER

#define BUTTON_TELE_LREL       	 	KEY_LREL_TELE
#define BUTTON_WIDE_LREL        	KEY_LREL_WIDE
#define BUTTON_SOS_LREL    	 		KEY_LREL_SOS
#define BUTTON_MUTE_LREL         	KEY_LREL_MUTE
#define BUTTON_CAPTURE_LREL      	KEY_LREL_CAPTURE
#define BUTTON_FUNC1_LREL		 	KEY_LREL_FUNC1
#define BUTTON_FUNC2_LREL        	KEY_LREL_FUNC2
#define BUTTON_FUNC3_LREL        	KEY_LREL_FUNC3

/* Touch Panel */
#define BUTTON_TOUCH_PRESS       	TOUCH_PANEL_PRESS
#define BUTTON_TOUCH_MOVE        	TOUCH_PANEL_MOVE
#define BUTTON_TOUCH_REL         	TOUCH_PANEL_REL

/* Power On Button */
#define BUTTON_POWER_ON_MENU	 	301
#define BUTTON_POWER_ON_MODE     	302
#define BUTTON_POWER_ON_REC      	303
#define BUTTON_POWER_ON_PLAY	 	304
#define BUTTON_POWER_ON_UP			305
#define BUTTON_POWER_ON_DOWN		306

/* Combo Key */
#define BUTTON_COMBO_SET1			311
#define BUTTON_COMBO_SET2			312
#define BUTTON_COMBO_SET3			313

#endif//!defined(CAR_DV)

#define BUTTON_GPSGSENSOR_UPDATE 	90
#define BUTTON_MENU_TIMEOUT      	91 
#define BUTTON_LOCK_FILE_G       	92 
#define BUTTON_LOCK_FILE_M       	93 
#define BUTTON_FOCUS_PASS        	94 
#define BUTTON_LENS_WMSG         	95 
#define BUTTON_CLEAR_WMSG        	96 
#define BUTTON_UPDATE_FILE       	97 
#define BUTTON_OSD_HIDE          	98 
#define BUTTON_UPDATE_MESSAGE    	99
#define BUTTON_UPDATE_CHARGE_ICON	100
#define GSENSOR_ACT                 (BUTTON_LOCK_FILE_G)

#if !defined(CAR_DV)
#define BUTTON_TOUCH_PRESS       TOUCH_S14_PRESS
#define BUTTON_TOUCH_MOVE        TOUCH_S14_MOVE
#define BUTTON_TOUCH_REL         TOUCH_S14_RELEASE
#endif

#define BUTTON_VRCB_RECDSTOP_CARDSLOW    VRCB_RECDSTOP_CARDSLOW
#define BUTTON_VRCB_AP_STOP_VIDEO_RECD   VRCB_AP_STOP_VIDEO_RECD
#define BUTTON_VRCB_MEDIA_FULL   VRCB_MEDIA_FULL
#define BUTTON_VRCB_FILE_FULL    VRCB_FILE_FULL
#define BUTTON_VRCB_LONG_TIME_FILE_FULL    VRCB_LONG_TIME_FILE_FULL
#define BUTTON_VRCB_MEDIA_SLOW   VRCB_MEDIA_SLOW
#define BUTTON_VRCB_MEDIA_ERROR  VRCB_MEDIA_ERROR
#define BUTTON_VRCB_SEAM_LESS    VRCB_SEAM_LESS
#define BUTTON_VRCB_MOTION_DTC   VRCB_MOTION_DTC
#define BUTTON_VRCB_VR_START  	 VRCB_VR_START
#define BUTTON_VRCB_VR_STOP   	 VRCB_VR_STOP
#define BUTTON_VRCB_VR_POSTPROC  VRCB_VR_POSTPROCESS
#define BUTTON_VRCB_EMER_DONE    VRCB_VR_EMER_DONE

#define BUTTON_BATTERY_DETECTION BATTERY_DETECTION

#define BUTTON_LDWS_START                       (272)
#define BUTTON_LDWS_STOP                        (273)
#define BUTTON_FCWS_START                       (275)
#define BUTTON_FCWS_STOP                        (276)
/*===========================================================================
 * Enum define
 *===========================================================================*/

typedef enum _UI_STATE_ID {
    UI_CAMERA_STATE           ,
    UI_VIDEO_STATE            ,
    UI_BROWSER_STATE    	  ,
    UI_PLAYBACK_STATE         ,
    UI_SLIDESHOW_STATE        ,
    UI_MSDC_STATE             ,
    UI_HDMI_STATE             ,
    UI_TVOUT_STATE            ,
    UI_CAMERA_MENU_STATE      ,
    UI_VIDEO_MENU_STATE       ,
    UI_PLAYBACK_MENU_STATE    ,
	UI_CLOCK_SETTING_STATE    ,
	UI_MODESELECT_MENU_STATE  ,
	UI_POWERON_SET			  ,
	UI_SD_UPDATE_STATE		  ,
	UI_USBSELECT_MENU_STATE   ,
	UI_BROWSELECT_MENU_STATE  ,
    UI_VIDEO_RECORD_STATE,	// Touch Panel project
    UI_PCCAM_STATE,
    UI_CAMERA_BURST_STATE,
    UI_TIME_LAPSE_STATE,
    UI_CAMERA_SETTINGS_STATE,
    UI_NET_PLAYBACK_STATE,
    UI_NET_FWUPDATE_STATE,
    UI_STATE_UNSUPPORTED,
    UI_STATE_NUM
}UI_STATE_ID;

/*===========================================================================
 * Structure define
 *===========================================================================*/

typedef struct 
{
    UI_STATE_ID        CurrentState;
    UI_STATE_ID        LastState;
    AHC_BOOL           bModeLocked;///< true means the state of state machine is locked and we can not change the state by
}UI_STATE_INFO;

typedef AHC_BOOL (*pfFunc)(void*);
typedef struct _UI_STATE_OP {
    pfFunc pfInitFunc;
    pfFunc pfShutDownFunc;
} UI_STATE_OP;
/*===========================================================================
 * Function prototype
 *===========================================================================*/

UINT32 		MenuButtonToMenuOp(UINT32 ulEvent ,UINT32 ulPosition);
AHC_BOOL 	StateSwitchMode(UI_STATE_ID mState);
void 		uiStateSystemStart(void);
void 		uiStateMachine( UINT32 ulMsgId, UINT32 ulEvent ,UINT32 ulParam );
void 		uiStateSetLocked(AHC_BOOL bLock);
UINT8 		uiGetCurrentState(void);
UINT8 		uiGetLastState(void);

void 		AHC_DCF_Init(void);
void 		uiPowerOffPicture(void);
void 		uiCheckDefaultMenuExist(void);
void 		StateLedControl( UINT32 uiMode );

#if defined(CAR_DV)
/* Text Menu setting */
#define	MENU_FILTER_NONE			0			/* not allow to set all of reserved */
#define	MENU_FILTER_DATETIME		1			/* allow to set date/time as enabled */
#define	MENU_FILTER_ALL				0xffffffff	/* allow all of setting */

#define	POST_DELETE					2
#define	POST_RENAME					3

#define		TXT_NO_COMMENT			0
#define		TXT_COMMENT				1
#define		TXT_VERSION				2
#define		TXT_MENU_ACT			0
#define		TXT_MENU_ALL			4

int 		ParseMenuSet(char *file, MenuInfo* pmi /*out*/, int fliter /* MENU_FILTER_** */);
int 		ParseDateTime(char *dt, int format, unsigned short *y, unsigned short *m, unsigned short *d,
						  unsigned short *hh, unsigned short *mm, unsigned short *ss);
AHC_BOOL 	ImportMenuInfo(MenuInfo *pmi);
AHC_BOOL 	ExportMenuAtom(char *file, int flag);
AHC_BOOL 	ApplyUserTextSetting(char *file, MenuInfo* pmi);
int			Getfp_VideoTime(void);
int			Getfp_ResetTime(void);

#define	IN_MENU()	(GetCurrentMenu() != NULL)
#endif

#endif //_AHC_MENU_H_

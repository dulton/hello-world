//==============================================================================
//
//  File        : ahc_general.h
//  Description : INCLUDE File for the AHC general function porting.
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================

#ifndef _AHC_GENERAL_H_
#define _AHC_GENERAL_H_

/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "AHC_Common.h"
#include "AHC_DCF.h"
#include "ait_bsp.h"
#include "config_fw.h"
#include "AHC_Config_SDK.h"
#include "AHC_Capture.h"
#ifdef CAR_DV
#include "AHC_General_CarDV.h"
#endif
#include "AHC_video.h" //For AHC_AUDIO_CHANNEL_CONFIG.

/*===========================================================================
 * Global variable
 *===========================================================================*/ 
extern AHC_BOOL             m_ubParkingModeEn;

/*===========================================================================
 * Type define
 *===========================================================================*/ 

//move to c file
//typedef MMP_ERR (VR_WRITEFUNC)(void *buf, MMP_ULONG size, MMP_ULONG *wr_size);

/*===========================================================================
 * Macro define
 *===========================================================================*/ 

#define AHC_I2C_HANDLE		void*
#define AHC_UART_HANDLE 	void*
#define AHC_TIMER_HANDLE 	void*

#define AHC_UART_PARITY_EN 					(0x0004)
#define AHC_UART_SEL_PARITY 				(0x0018)
#define AHC_USP_FILE_LOCATION 				"SD:1:\\USP.dat"
#define AHC_FSP_FILE_LOCATION 				"SD:1:\\FSP.dat"

//Recference mmpf_player.c VIDEO_EVENT_XXX
#define AHC_VIDEO_PLAY         				(0x00000000)
#define AHC_VIDEO_PLAY_EOF     				(0x00000001)
#define AHC_VIDEO_PLAY_FF_EOS  				(0x00000002)
#define AHC_VIDEO_PLAY_BW_BOS  				(0x00000004)
#define AHC_VIDEO_PLAY_ERROR_STOP  			(0x00000008)
#define AHC_VIDEO_PLAY_UNSUPPORTED_AUDIO  	(0x00000010)

#define ENALBE_MAX_PROTECT_TIME     		(1)
#define AHC_VIDEO_MAX_PROTECT_TIME  		(500)//ms
#define AHC_STILL_MAX_PROTECT_TIME  		(0)//ms

#define NON_CYCLING_TIME_LIMIT				(0xFFFFFFFF)

//For Video Record
#define SIGNLE_FILE_SIZE_LIMIT_4G			(0xFFFFFFFF)
#define SIGNLE_FILE_SIZE_LIMIT_3_75G		(0xF0000000)
#define SIGNLE_FILE_SIZE_LIMIT_3_5G			(0xE0000000)

#ifndef VR_MIC_VOLUME_DGAIN
	#define	VR_MIC_VOLUME_DGAIN				0x3F
#endif

//For Search File Format Char */
#define SEARCH_PHOTO						"JPG"
#ifdef CAR_DV
#define SEARCH_MOVIE						"AVI,MOV"
#else
#define SEARCH_MOVIE						"AVI,MP4"
#endif
#define SEARCH_PHOTO_MOVIE					"AVI,MOV,JPG"

#define EXT_DOT                             "."
#define PHOTO_JPG_EXT                       "JPG"
#ifdef CAR_DV
#define MOVIE_3GP_EXT                       "MOV"
#else
#define MOVIE_3GP_EXT                       "MP4"
#endif
#define MOVIE_AVI_EXT                       "AVI"


//For Audio Callback Event [Ref:mmpf_audio_ctl.h]
#define AHC_AUDIO_EVENT_EOF     			(0x00000001)
#define AHC_AUDIO_EVENT_PLAY				(0x00000002)

//For SD Card
#define SLOW_MEDIA_CLASS				(4)//Class4
#define SLOW_MEDIA_CB_THD				(3)

#define MAX_FILE_NAME_LENGTH    (32)
#define MAX_ALLOWED_WORD_LENGTH (57)

#define AHC_PREVIEW_WINDOW_OP_MASK	 	0xff00
#define	AHC_PREVIEW_WINDOW_OP_GET 	 	0x0000
#define	AHC_PREVIEW_WINDOW_OP_SET 	 	0x0100
#define AHC_PREVIEW_WINDOW_MASK		 	0x00ff
#define	AHC_PREVIEW_WINDOW_FRONT 	 	0x0000
#define	AHC_PREVIEW_WINDOW_REAR		 	0x0001

#if defined(WIFI_PORT) && (WIFI_PORT == 1)
#include "netapp.h"
#define	CGI_FEEDBACK(j, c) netapp_CGIOpFeedback((void*)j, c)
#else
#define	CGI_FEEDBACK(j, c)
#endif
/*===========================================================================
 * Structure define
 *===========================================================================*/ 

typedef enum{
	EDIT_BROWSER_NONE           = 0x00,
	EDIT_BROWSER_DELETE_ONE     = 0x01,
	EDIT_BROWSER_DELETE_ALL     = 0x02,
	EDIT_BROWSER_PROTECT_ONE    = 0x04,
	EDIT_BROWSER_PROTECT_ALL    = 0x08,
	EDIT_BROWSER_UNPROTECT_ONE  = 0x10,
	EDIT_BROWSER_UNPROTECT_ALL  = 0x20
}EDIT_BROWSER_MODE;

#if (USE_INFO_FILE)
typedef struct _INFO_LOG {
	INT8  DCFCurVideoFileName[32];
    INT8  DCFCurStillFileName[32];
} INFO_LOG;
#endif

typedef struct _AHC_RTC_TIME
{
    UINT16 uwYear;		
    UINT16 uwMonth;
    UINT16 uwDay;		//The date of January is 1~31
    UINT16 uwDayInWeek; //Sunday ~ Saturday
    UINT16 uwHour;   
    UINT16 uwMinute;
    UINT16 uwSecond;
    UINT8 ubAmOrPm;		//am: 0, pm: 1, work only at b_12FormatEn = MMP_TURE
    UINT8 b_12FormatEn;  //for set time, to indacate which format, 0 for 24 Hours,   //1 for 12 Hours format
}AHC_RTC_TIME; //Sync it with AUTL_DATETIME

typedef struct _AHC_TIME_STRING
{
	INT8 byYear[8];
	INT8 byMonth[8];
	INT8 byDay[8];
	INT8 byHour[8];
	INT8 byMinute[8];
	INT8 bySecond[8]; 

}AHC_TIME_STRING;

typedef struct _AHC_QUEUE_MESSAGE 
{
	UINT32 ulMsgID; 
	UINT32 ulParam1;
	UINT32 ulParam2;
} AHC_QUEUE_MESSAGE;

typedef struct _AHC_I2CM_ATTRIBUTE 
{
	UINT8 	byI2cmID;
	UINT8 	bySlaveAddr;
	UINT8 	byRegLen;
	UINT8 	byDataLen;
	UINT8 	byPadNum;
	UINT8 	bySwDelayTime;
	UINT8 	bySwClkPinGourp;
	UINT8 	bySwClkPinNum;
	UINT32 	ulClk;
	UINT8 	bySwDataPinGourp;
	UINT8 	bySwDataPinNum;
} AHC_I2CM_ATTRIBUTE;

typedef struct _AHC_MEMORY_LOCATION 
{
	UINT32 ulLocationAddr;
	UINT32 ulSize;	
} AHC_MEMORY_LOCATION;

typedef struct _AHC_SOUND_FILE_ATTRIBUTE 
{
	char  	path[256];
	UINT32 	ulStartAddress;
	UINT32 	ulSize;
} AHC_SOUND_FILE_ATTRIBUTE;

typedef struct _PARKING_STATE_INFO
{
    AHC_BOOL           bParkingModeConfirmMenu;
	UINT8		       bParkingMode;
    AHC_BOOL           bRecordVideo;
    AHC_BOOL		   bParkingIdling;
    AHC_BOOL		   bParkingExitState;
    AHC_BOOL		   bAutoStopParkingMode;
    AHC_BOOL           bParkingMoveHintMenu;
    AHC_BOOL		   bParkingSpaceState;
	AHC_BOOL		   bParkingStartDrawed;

}PARKING_STATE_INFO;

/*===========================================================================
 * Enum define
 *===========================================================================*/

typedef enum{
	#if(defined(DEVICE_GPIO_2NDSD_PLUG))
    SDMMC_1STOUT_2NDOUT = 0,
    SDMMC_1STIN_2NDOUT,
    SDMMC_1STOUT_2NDIN,
    SDMMC_1STIN_2NDIN
    #else
    SDMMC_OUT = 0,
    SDMMC_IN
    #endif
}SDMMC_STATE;

typedef enum{

    MEDIA_CMD_FORMAT = 0,
   	MEDIA_CMD_GET_ATTRIBUTE,
   	MEDIA_CMD_MAX
}MEDIA_CMD;

typedef enum{

    AHC_SD_0 = 0,
    AHC_SD_1,
    AHC_SD_2,
    AHC_SD_3,
    AHC_SD_DEV_NUM
} AHC_SD_ID;

typedef enum _AHC_PIO_TRIGMODE {
	AHC_PIO_TRIGMODE_EDGE_H2L		=	0x0,
	AHC_PIO_TRIGMODE_EDGE_L2H		=	0x1,
	AHC_PIO_TRIGMODE_LEVEL_H		=	0x2,
	AHC_PIO_TRIGMODE_LEVEL_L		=	0x3,
	AHC_PIO_TRIGMODE_UNKNOWN		=	0xFF
} AHC_PIO_TRIGMODE;

typedef enum _AHC_RTC_WAKE_UP_FLAG {
	AHC_RTC_WAKE_UP_ARM = 0,
	AHC_RTC_WAKE_UP_DISARM
} AHC_RTC_WAKE_UP_FLAG;

typedef enum _AHC_MEMORY_LOCATION_ID {
	AHC_CCD_TG_MEM_LOCATION = 0,
	AHC_UART_WRITE_MEM_LOCATION,
	AHC_UART_READ_MEM_LOCATION,
	AHC_SET_PARAMARRAY_MEM_LOCATION,
	AHC_GET_PARAM_ARRAY_MEM_LOCATION,
	AHC_FLASH_BURST_MEM_LOCATION,
	AHC_GET_IMAGE_MEM_LOCATION,
	AHC_CURR_PATH_MEM_LOCATION,
	AHC_LUMA_HIST_MEM_LOCATION,
	AHC_GET_TIME_MEM_LOCATION,
	AHC_SET_FREE_PARAMS_MEM_LOCATION,
	AHC_GET_FREE_PARAMS_MEM_LOCATION,
	AHC_AWB_TAB_MEM_LOCATION ,
	AHC_TSMT_FILE_MEM_LOCATION,
	AHC_TSMT_THUMBNAIL_MEM_LOCATION,
	AHC_SET_EXIF_MEM_LOCATION,
	AHC_GET_EXIF_MEM_LOCATION,
	AHC_RCV_FILE_MEM_LOCATION,
	AHC_SET_DPOF_GET_HEADR_PARAM_MEM_LOCATION,
	AHC_SET_DPOF_GET_JOB_PARAM_STR_MEM_LOCATION,
	AHC_SET_DPOF_GET_SRC_PARAM_MEM_LOCATION,
	AHC_OSD_TEXT_OUT_MEM_LOCATION,
	AHC_OSD_DRAW_TEXT_MEM_LOCATION,
	AHC_WRITE_I2C_MEM_LOCATION,
	AHC_READ_I2C_MEM_LOCATION,
	AHC_DRAW_TEXT_MEM_LOCATION,
	AHC_MEMORY_LOCATION_MAX	
} AHC_MEMORY_LOCATION_ID;

typedef enum _AHC_TIMER_CMD {
	AHC_TIMER_CMD_START	= 0,
	AHC_TIMER_CMD_START_ONESHOT,
	AHC_TIMER_CMD_MAX
} AHC_TIMER_CMD;

typedef enum _AHC_UART_BAUDRATE {
	AHC_UART_BAUD_RATE_9600 = 0,
	AHC_UART_BAUD_RATE_14400,	
	AHC_UART_BAUD_RATE_19200,	
	AHC_UART_BAUD_RATE_38400,	
	AHC_UART_BAUD_RATE_57600,	
	AHC_UART_BAUD_RATE_115200	
} AHC_UART_BAUDRATE;

typedef enum _AHC_BUTTON_DRV {
	AHC_BUTTON_MASK = 0,
	AHC_BUTTON_PRESS_DLY,	
	AHC_GPIO_RPT_DLY,	
	AHC_GPIO_RPT_MASK	
} AHC_BUTTON_DRV;

typedef enum _AHC_PWMPIN {
	 AHC_PWMPIN_NONE = 0x0,
	 AHC_PWMPIN_MAX
}AHC_PWMPIN;

typedef enum _AHC_MODE_ID {
	AHC_MODE_IDLE 				= 0x00,
	AHC_MODE_CAPTURE_PREVIEW 	= 0x10,
	AHC_MODE_DRAFT_CAPTURE 		= 0x11,
	AHC_MODE_STILL_CAPTURE 		= 0x20,
	AHC_MODE_C2C_CAPTURE 		= 0x21,
	AHC_MODE_SEQUENTIAL_CAPTURE = 0x22,
	AHC_MODE_LONG_TIME_CONTINUOUS_FIRST_CAPTURE = 0x23,
	AHC_MODE_LONG_TIME_CONTINUOUS_NEXT_CAPTURE = 0x24,
	AHC_MODE_LONG_TIME_CONTINUOUS_LAST_CAPTURE = 0x25,
	AHC_MODE_CONTINUOUS_CAPTURE = 0x26,
	AHC_MODE_USB_MASS_STORAGE 	= 0x30,
	AHC_MODE_USB_WEBCAM 		= 0x31,
	AHC_MODE_PLAYBACK  			= 0x40,
	AHC_MODE_THUMB_BROWSER 		= 0x50,
	AHC_MODE_VIDEO_RECORD 		= 0x60,
	AHC_MODE_RECORD_VONLY 		= 0x61,
	AHC_MODE_RECORD_PREVIEW 	= 0x62,
	AHC_MODE_RAW_PREVIEW 		= 0x70,
    AHC_MODE_RAW_CAPTURE 		= 0x71,
    AHC_MODE_NET_PLAYBACK       = 0x80,
	AHC_MODE_CALIBRATION 		= 0xF0,
	AHC_MODE__NO_CHANGE             ,       // temp Special ID 
	AHC_MODE_MAX 				= 0xFF
}AHC_MODE_ID;

// Does SMODE means sub-mode or streaming-mode?? Or both
typedef enum _AHC_STREAMING_MODE {
    //streaming mode operation
	AHC_STREAM_OFF   = 0x00,
	AHC_STREAM_PAUSE = 0x08,
	AHC_STREAM_RESUME= 0x80,
	//stream configuration
	AHC_STREAM_TRANSCODE = 0x04,
    AHC_STREAM_NONE  = AHC_STREAM_OFF,
	AHC_STREAM_MJPEG = 0x01,
	AHC_STREAM_H264  = 0x02,
	AHC_STREAM_V_MAX = 0x03,
	AHC_STREAM_V_MASK= 0x03,
	AHC_STREAM_ADPCM = 0x10,
	AHC_STREAM_MP3   = 0x20,
	AHC_STREAM_AAC   = 0x30,
	AHC_STREAM_G711  = 0x40,
    AHC_STREAM_A_MAX = 0x40,
    AHC_STREAM_A_MASK= 0x70,
	AHC_STREAM_MJPEG_ADPCM = AHC_STREAM_MJPEG | AHC_STREAM_ADPCM,
    AHC_STREAM_MJPEG_MP3   = AHC_STREAM_MJPEG | AHC_STREAM_MP3,
    AHC_STREAM_MJPEG_AAC   = AHC_STREAM_MJPEG | AHC_STREAM_AAC,
    AHC_STREAM_V_MASK2  = 0x100,
    AHC_STREAM_MJPEG2	= 0x100
}AHC_STREAMING_MODE;

#define AHC_SMODE_is_streaming(uiSMode)  (uiSMode != AHC_STREAM_OFF)

typedef enum _AHC_SYS_STATUS {
	AHC_SYS_VIDRECD_STATUS = 0x00,
    //AHC_SYS_VIDRECD_NONE,				///< Video record operation, none
    AHC_SYS_VIDRECD_START,				///< Video record operation, start
    AHC_SYS_VIDRECD_PAUSE,				///< Video record operation, pause
    AHC_SYS_VIDRECD_RESUME,				///< Video record operation, resume
    AHC_SYS_VIDRECD_STOP,				///< Video record operation, stop
    AHC_SYS_VIDRECD_PREVIEW_NORMAL,   
	AHC_SYS_VIDRECD_PREVIEW_ABNORMAL,
	
	AHC_SYS_VIDPLAY_STATUS = 0x10,
	AHC_SYS_VIDPLAY_INVALID,			/**< The component has detected that it's internal data 
                                		structures are corrupted to the point that
                                		it cannot determine it's state properly */
	AHC_SYS_VIDPLAY_LOADED,      		/**< The component has been loaded but has not completed
                                		initialization. */
    AHC_SYS_VIDPLAY_IDLE,        		/**< The component initialization has been completed
                                		successfully and the component is ready to start.*/
    AHC_SYS_VIDPLAY_EXECUTING,   		/**< The component has accepted the start command and
                                		is processing data (if data is available) */
    AHC_SYS_VIDPLAY_PAUSE,       		/**< The component has received pause command */
    
    
	AHC_SYS_USB_STATUS 		= 0x20,
	AHC_SYS_USB_NORMAL,
	AHC_SYS_USB_ISADAPTER,
	AHC_SYS_USB_IDLE,
	
	AHC_SYS_CAPTURE_STATUS 	= 0x30,
	
	AHC_SYS_AUDPLAY_STATUS			= 0x40,
	AHC_SYS_AUDPLAY_START			= 0x41,			
	AHC_SYS_AUDPLAY_PAUSE			= 0x42,	
	AHC_SYS_AUDPLAY_STOP			= 0x44,	
	AHC_SYS_AUDPLAY_INVALID			= 0x48,
	
	AHC_SYS_IDLE,
	AHC_SYS_ERROR,
	
	AHC_SYS_OPERATION_MAX
} AHC_SYS_STATUS;

#if (CHIP == MCR_V2)
typedef enum _AHC_PIO_REG 
{
    //===========================GPIO
    AHC_PIO_REG_GPIO0 	= 	0x0,
    AHC_PIO_REG_GPIO1	= 	0x1,
    AHC_PIO_REG_GPIO2	= 	0x2,
    AHC_PIO_REG_GPIO3	= 	0x3,
    AHC_PIO_REG_GPIO4	= 	0x4,
    AHC_PIO_REG_GPIO5	= 	0x5,
    AHC_PIO_REG_GPIO6	= 	0x6,
    AHC_PIO_REG_GPIO7	= 	0x7,
    AHC_PIO_REG_GPIO8	= 	0x8,
    AHC_PIO_REG_GPIO9	= 	0x9,
    AHC_PIO_REG_GPIO10  = 	0xa,
    AHC_PIO_REG_GPIO11	= 	0xb,
    AHC_PIO_REG_GPIO12	= 	0xc,
    AHC_PIO_REG_GPIO13	= 	0xd,
    AHC_PIO_REG_GPIO14	= 	0xe,
    AHC_PIO_REG_GPIO15	= 	0xf,
    AHC_PIO_REG_GPIO16	= 	0x10,
    AHC_PIO_REG_GPIO17	= 	0x11,
    AHC_PIO_REG_GPIO18	= 	0x12,
    AHC_PIO_REG_GPIO19	= 	0x13,
    AHC_PIO_REG_GPIO20  = 	0x14,
    AHC_PIO_REG_GPIO21	= 	0x15,
    AHC_PIO_REG_GPIO22	= 	0x16,
    AHC_PIO_REG_GPIO23	= 	0x17,
    AHC_PIO_REG_GPIO24	= 	0x18,
    AHC_PIO_REG_GPIO25	= 	0x19,
    AHC_PIO_REG_GPIO26	= 	0x1a,
    AHC_PIO_REG_GPIO27	= 	0x1b,
    AHC_PIO_REG_GPIO28	= 	0x1c,
    AHC_PIO_REG_GPIO29	= 	0x1d,
    AHC_PIO_REG_GPIO30  = 	0x1e,
    AHC_PIO_REG_GPIO31	= 	0x1f,
    AHC_PIO_REG_GPIO32	= 	0x20,
    AHC_PIO_REG_GPIO33	= 	0x21,
    AHC_PIO_REG_GPIO34	= 	0x22,
    AHC_PIO_REG_GPIO35	= 	0x23,
    AHC_PIO_REG_GPIO36	= 	0x24,
    AHC_PIO_REG_GPIO37	= 	0x25,
    AHC_PIO_REG_GPIO38	= 	0x26,
    AHC_PIO_REG_GPIO39	= 	0x27,
    AHC_PIO_REG_GPIO40  = 	0x28,
    AHC_PIO_REG_GPIO41	= 	0x29,
    AHC_PIO_REG_GPIO42	= 	0x2a,
    AHC_PIO_REG_GPIO43	= 	0x2b,
    AHC_PIO_REG_GPIO44	= 	0x2c,
    AHC_PIO_REG_GPIO45	= 	0x2d,
    AHC_PIO_REG_GPIO46	= 	0x2e,
    AHC_PIO_REG_GPIO47	= 	0x2f,
    AHC_PIO_REG_GPIO48	= 	0x30,
    AHC_PIO_REG_GPIO49	= 	0x31,
    AHC_PIO_REG_GPIO50  = 	0x32,
    AHC_PIO_REG_GPIO51	= 	0x33,
    AHC_PIO_REG_GPIO52	= 	0x34,
    AHC_PIO_REG_GPIO53	= 	0x35,
    AHC_PIO_REG_GPIO54	= 	0x36,
    AHC_PIO_REG_GPIO55	= 	0x37,
    AHC_PIO_REG_GPIO56	= 	0x38,
    AHC_PIO_REG_GPIO57	= 	0x39,
    AHC_PIO_REG_GPIO58	= 	0x3a,
    AHC_PIO_REG_GPIO59	= 	0x3b,
    AHC_PIO_REG_GPIO60	= 	0x3c,
    AHC_PIO_REG_GPIO61	= 	0x3d,
    AHC_PIO_REG_GPIO62	= 	0x3e,
    AHC_PIO_REG_GPIO63	= 	0x3f,
    AHC_PIO_REG_GPIO64  =   0x40,
    AHC_PIO_REG_GPIO65  =   0x41,
    AHC_PIO_REG_GPIO66  =   0x42,
    AHC_PIO_REG_GPIO67  =   0x43,
    AHC_PIO_REG_GPIO68  =   0x44,
    AHC_PIO_REG_GPIO69  =   0x45,
    AHC_PIO_REG_GPIO70  =   0x46,
    AHC_PIO_REG_GPIO71  =   0x47,
    AHC_PIO_REG_GPIO72  =   0x48,
    AHC_PIO_REG_GPIO73  =   0x49,
    AHC_PIO_REG_GPIO74  =   0x4A,
    AHC_PIO_REG_GPIO75  =   0x4B,
    AHC_PIO_REG_GPIO76  =   0x4C,
    AHC_PIO_REG_GPIO77  =   0x4D,
    AHC_PIO_REG_GPIO78  =   0x4E,
    AHC_PIO_REG_GPIO79  =   0x4F,
    AHC_PIO_REG_GPIO80  =   0x50,
    AHC_PIO_REG_GPIO81  =   0x51,
    AHC_PIO_REG_GPIO82  =   0x52,
    AHC_PIO_REG_GPIO83  =   0x53,
    AHC_PIO_REG_GPIO84  =   0x54,
    AHC_PIO_REG_GPIO85  =   0x55,
    AHC_PIO_REG_GPIO86  =   0x56,
    AHC_PIO_REG_GPIO87  =   0x57,
    AHC_PIO_REG_GPIO88  =   0x58,
    AHC_PIO_REG_GPIO89  =   0x59,
    AHC_PIO_REG_GPIO90  =   0x5A,
    AHC_PIO_REG_GPIO91  =   0x5B,
    AHC_PIO_REG_GPIO92  =   0x5C,
    AHC_PIO_REG_GPIO93  =   0x5D,
    AHC_PIO_REG_GPIO94  =   0x5E,
    AHC_PIO_REG_GPIO95  =   0x5F,
    AHC_PIO_REG_GPIO96  =   0x60,
    AHC_PIO_REG_GPIO97  =   0x61,
    AHC_PIO_REG_GPIO98  =   0x62,
    AHC_PIO_REG_GPIO99  =   0x63,
    AHC_PIO_REG_GPIO100 =   0x64,
    AHC_PIO_REG_GPIO101 =   0x65,
    AHC_PIO_REG_GPIO102 =   0x66,
    AHC_PIO_REG_GPIO103 =   0x67,
    AHC_PIO_REG_GPIO104 =   0x68,
    AHC_PIO_REG_GPIO105 =   0x69,
    AHC_PIO_REG_GPIO106 =   0x6A,
    AHC_PIO_REG_GPIO107 =   0x6B,
    AHC_PIO_REG_GPIO108 =   0x6C,
    AHC_PIO_REG_GPIO109 =   0x6D,
    AHC_PIO_REG_GPIO110 =   0x6E,
    AHC_PIO_REG_GPIO111 =   0x6F,
    AHC_PIO_REG_GPIO112 =   0x70,
    AHC_PIO_REG_GPIO113 =   0x71,
    AHC_PIO_REG_GPIO114 =   0x72,
    AHC_PIO_REG_GPIO115 =   0x73,
    AHC_PIO_REG_GPIO116 =   0x74,
    AHC_PIO_REG_GPIO117 =   0x75,
    AHC_PIO_REG_GPIO118 =   0x76,
    AHC_PIO_REG_GPIO119 =   0x77,
    AHC_PIO_REG_GPIO120 =   0x78,
    AHC_PIO_REG_GPIO121 =   0x79,
    AHC_PIO_REG_GPIO122 =   0x7A,
    AHC_PIO_REG_GPIO123 =   0x7B,
    AHC_PIO_REG_GPIO124 =   0x7C,
    AHC_PIO_REG_GPIO125 =   0x7D,
    AHC_PIO_REG_GPIO126 =   0x7E,
    AHC_PIO_REG_GPIO127 =   0x7F,
    AHC_PIO_REG_UNKNOWN =   0xFFF
} AHC_PIO_REG;
#endif

// DUPLICATED_DEFINITION: From config_fw.h AIT_BOARD_TYPE list.
typedef enum _AHC_BOARD_TYPE {
	AHC_BOARD_UNKNOWN = 0,
	AHC_BOARD_AIT_8426_V11,
	AHC_BOARD_AIT_8427_V11,
	AHC_BOARD_AIT_8427_REAL_SIZE,
	AHC_BOARD_AIT_8427_EP1,
	AHC_BOARD_AIT_8427_89_EP1,
	AHC_BOARD_AIT_8427_89_EP2,
	AHC_BOARD_AIT_8427_JWD_EP2,
	AHC_BOARD_AIT_8427_WANMA_3IN1,
	AHC_BOARD_AIT_8427D_CARDV,
	AHC_BOARD_AIT_8427_WANMA_2IN1,
	AHC_BOARD_AIT_8427_M8_DEMO,
	AHC_BOARD_AIT_8427_GSI_G4
}AHC_BOARD_TYPE;

typedef enum _AHC_SOUNDEFFECT_TYPE {
	AHC_SOUNDEFFECT_SHUTTER = 0,
	AHC_SOUNDEFFECT_START,
	AHC_SOUNDEFFECT_BUTTON,
	AHC_SOUNDEFFECT_VR_START,
	//from HCarDV.wifi porting
	AHC_SOUNDEFFECT_VIDEO_REC = AHC_SOUNDEFFECT_VR_START,
	AHC_SOUNDEFFECT_VR_STOP,
	
	AHC_SOUNDEFFECT_VR_SAVE,
	AHC_SOUNDEFFECT_VR_EMER,
	AHC_SOUNDEFFECT_FOCUS,
	AHC_SOUNDEFFECT_ERROR,
	AHC_SOUNDEFFECT_CONFIRM,
	AHC_SOUNDEFFECT_SELFTIMER,
	AHC_SOUNDEFFECT_NO_SD_CARD,
	
	AHC_SOUNDEFFECT_POWERON,
	AHC_SOUNDEFFECT_POWEROFF,

	AHC_SOUNDEFFECT_MICON,
	AHC_SOUNDEFFECT_MICOFF,

	AHC_SOUNDEFFECT_FILELOCK,
	AHC_SOUNDEFFECT_FILEUNLOCK,
	
	AHC_SOUNDEFFECT_CARDFULL,
	
	AHC_SOUNDEFFECT_TEST,
	
	AHC_SOUNDEFFECT_SPCAM_START = 0x20,
	AHC_SOUNDEFFECT_SPCAMWARN,
	AHC_SOUNDEFFECT_SPCAMVOICE_CHT,
	AHC_SOUNDEFFECT_SPCAMVOICE_CHS,
	
	AHC_SOUNDEFFECT_SPCAMVOICE_RUS,
	AHC_SOUNDEFFECT_SPCAMVOICE_ENG,
	AHC_SOUNDEFFECT_SPCAMRELEASE,
	AHC_SOUNDEFFECT_SPCAMOVERSPEED,
	AHC_SOUNDEFFECT_SPCAMVOICE_15KM_CHT,
	
	AHC_SOUNDEFFECT_SPCAMVOICE_20KM_CHT,
	AHC_SOUNDEFFECT_SPCAMVOICE_25KM_CHT,
	AHC_SOUNDEFFECT_SPCAMVOICE_30KM_CHT,
	AHC_SOUNDEFFECT_SPCAMVOICE_35KM_CHT,
	AHC_SOUNDEFFECT_SPCAMVOICE_40KM_CHT,
	
	AHC_SOUNDEFFECT_SPCAMVOICE_50KM_CHT,
	AHC_SOUNDEFFECT_SPCAMVOICE_60KM_CHT,
	AHC_SOUNDEFFECT_SPCAMVOICE_70KM_CHT,
	AHC_SOUNDEFFECT_SPCAMVOICE_80KM_CHT,
	AHC_SOUNDEFFECT_SPCAMVOICE_90KM_CHT,
	
	AHC_SOUNDEFFECT_SPCAMVOICE_100KM_CHT,
	AHC_SOUNDEFFECT_SPCAMVOICE_110KM_CHT,
	AHC_SOUNDEFFECT_SPCAMVOICE_120KM_CHT,
	
	AHC_SOUNDEFFECT_SPCAMVOICE_SPEED_DESCRIPTION,
	AHC_SOUNDEFFECT_SPCAMVOICE_SPEED_LIMIT,
	
	AHC_SOUNDEFFECT_SPCAMVOICE_10,
	AHC_SOUNDEFFECT_SPCAMVOICE_20,
	AHC_SOUNDEFFECT_SPCAMVOICE_30,
	AHC_SOUNDEFFECT_SPCAMVOICE_35,
	AHC_SOUNDEFFECT_SPCAMVOICE_40,
	AHC_SOUNDEFFECT_SPCAMVOICE_50,
	AHC_SOUNDEFFECT_SPCAMVOICE_55,
	AHC_SOUNDEFFECT_SPCAMVOICE_60,
	AHC_SOUNDEFFECT_SPCAMVOICE_65,
	
	AHC_SOUNDEFFECT_SPCAMVOICE_70,
	AHC_SOUNDEFFECT_SPCAMVOICE_75,
	AHC_SOUNDEFFECT_SPCAMVOICE_80,
	AHC_SOUNDEFFECT_SPCAMVOICE_85,
	AHC_SOUNDEFFECT_SPCAMVOICE_90,
	AHC_SOUNDEFFECT_SPCAMVOICE_100,
	AHC_SOUNDEFFECT_SPCAMVOICE_105,
	AHC_SOUNDEFFECT_SPCAMVOICE_110,
	AHC_SOUNDEFFECT_SPCAMVOICE_115,
	
	AHC_SOUNDEFFECT_SPCAMVOICE_120,
	AHC_SOUNDEFFECT_SPCAMVOICE_125,
	AHC_SOUNDEFFECT_SPCAMVOICE_130,
	AHC_SOUNDEFFECT_SPCAMVOICE_140,
	AHC_SOUNDEFFECT_SPCAMVOICE_150,
	
	AHC_SOUNDEFFECT_SPCAMVOICE_160,
	AHC_SOUNDEFFECT_SPCAMVOICE_170,
	AHC_SOUNDEFFECT_SPCAMVOICE_180,
	AHC_SOUNDEFFECT_SPCAMVOICE_190,
	AHC_SOUNDEFFECT_SPCAMVOICE_200,
	
	AHC_SOUNDEFFECT_SPCAMVOICE_KM,
	AHC_SOUNDEFFECT_SPCAMVOICE_MPH,
	
	#if (PLAY_SOUND_SEQUENTIALLY)
	AHC_SOUNDEFFECT_PLAYSOUND_EVENT,
	#endif

	AHC_SOUNDEFFECT_PARKING,
	AHC_SOUNDEFFECT_CARD_ERROR,
	AHC_SOUNDEFFECT_INSERT_SD_CARD,
	AHC_SOUNDEFFECT_CHECK_SD_CARD,
	AHC_SOUNDEFFECT_FONTT_CAM_ERROR,

	AHC_SOUNDEFFECT_SPCAM_END,

    /////////////////////////////////////////////////////////////////////////////////
    //from HCarDV.wifi
    /////////////////////////////////////////////////////////////////////////////////
	AHC_SOUNDEFFECT_BATWARNING,	
	AHC_SOUNDEFFECT_LOCK_DELETE,
	AHC_SOUNDEFFECT_FINDME,
	AHC_SOUNDEFFECT_WIFIFAIL,
	
	AHC_SOUNDEFFECT_ATTENTION,
	AHC_SOUNDEFFECT_LDWS_WARNING,
	AHC_SOUNDEFFECT_FCWS_WARNING,
	
	AHC_SOUNDEFFECT_HEADLIGHT_WARNING,
	AHC_SOUNDEFFECT_FATIGUE_ALERT,	//0x3F
	
	AHC_SOUNDEFFECT_TPMS_ERR,
	
	AHC_SOUNDEFFECT_GPS_LINK,

    AHC_SOUNDEFFECT_MAX_NUM
}AHC_SOUNDEFFECT_TYPE;

typedef enum _AHC_LCD_DIRECTION {
	AHC_LCD_UPLEFT_DOWNRIGHT = 0,
	AHC_LCD_DOWNLEFT_UPRIGHT,
	AHC_LCD_UPRIGHT_DOWNLEFT,
	AHC_LCD_DOWNRIGHT_UPLEFT	
}AHC_LCD_DIRECTION;

typedef enum _AHC_LCD_STATUS {
	AHC_LCD_NORMAL = 0,
	AHC_LCD_REVERSE,
	AHC_LCD_NOFLIP		
}AHC_LCD_STATUS;

typedef enum _AHC_SNR_STATUS {
	AHC_SNR_NORMAL = 0,
	AHC_SNR_REVERSE,
	AHC_SNR_NOFLIP	
}AHC_SNR_STATUS;

typedef enum _AHC_SOUND_EFFECT_STATUS {
	AHC_SOUND_EFFECT_STATUS_START = 0,  ///< play start
    AHC_SOUND_EFFECT_STATUS_PAUSE ,     ///< play pause
    AHC_SOUND_EFFECT_STATUS_STOP  ,     ///< play stop
    AHC_SOUND_EFFECT_STATUS_INVALID		///< invalid status
}AHC_SOUND_EFFECT_STATUS;

typedef enum _AHC_PROTECTION_OP{
    AHC_PROTECTION_PRE_FILE = 0x01,// previous
    AHC_PROTECTION_CUR_FILE = 0x02,// current
    AHC_PROTECTION_BOTH_FILE = AHC_PROTECTION_PRE_FILE|AHC_PROTECTION_CUR_FILE// previous + current
}AHC_PROTECTION_OP;

typedef enum _AHC_VR_SEAMLESS_PROTECT_FILE {
	AHC_VR_SEAMLESS_PROTECT_PREFILE = 0,
	AHC_VR_SEAMLESS_PROTECT_CURFILE,
	AHC_VR_SEAMLESS_PROTECT_BOTHFILE
}AHC_VR_SEAMLESS_PROTECT_FILE_OP;

typedef enum _AHC_PROTECT_TYPE {
	AHC_PROTECT_NONE    = 0x00, 
	AHC_PROTECT_G       = 0x01, //GSensor Lock
	AHC_PROTECT_MENU    = 0x02, //Menu lock
	AHC_PROTECT_D       = AHC_PROTECT_G|AHC_PROTECT_MENU,   //GSensor+Menu Lock
	AHC_PROTECT_MOVE  	= 0x04,  //Move File to another DIR
	AHC_PROTECT_NEXT  	= 0x08  //Move next File to another DIR
}AHC_PROTECT_TYPE;

typedef enum _AHC_FILE_OP{
    AHC_FILE_OP_DELETE,
    AHC_FILE_OP_PROTECT,
    AHC_FILE_OP_UNPROTECT
}AHC_FILE_OP;

typedef enum _AHC_ZOOM_DIRECTION
{
	AHC_SENSOR_ZOOM_IN = 0,
	AHC_SENSOR_ZOOM_OUT,
	AHC_SENSOR_ZOOM_STOP
} AHC_ZOOM_DIRECTION;

#if ((EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_DUAL_FILE) || (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE))
typedef enum _AHC_KERNAL_EMERGENCY_STOP_STEP{
    AHC_KERNAL_EMERGENCY_RECORD,
    AHC_KERNAL_EMERGENCY_STOP,
    AHC_KERNAL_EMERGENCY_AHC_DONE
}AHC_KERNAL_EMERGENCY_STOP_STEP;
#endif
typedef void (*PFDrawBarCB)(UINT32, UINT32, AHC_BOOL);

#if (PLAY_SOUND_SEQUENTIALLY)
typedef struct _AHC_SOUND_FILE_QUEUE_
{
	AHC_SOUNDEFFECT_TYPE soundtype;
	AHC_RTC_TIME		 soundInitTime;
} AHC_SOUND_FILE_QUEUE;
#endif

typedef struct _AHC_WINDOW_RECT {
    UINT16		usLeft;
    UINT16      usTop;
    UINT16      usWidth;
    UINT16      usHeight;
} AHC_WINDOW_RECT;

/*===========================================================================
 * Function prototype
 *===========================================================================*/

// Utility Function 
void 		AIHC_Int2Str(UINT32 value, INT8 *string);

#if (USE_INFO_FILE)
INFO_LOG*  	AHC_InfoLog(void);
void 		AHC_WriteSDInfoFile(void);
void 		AHC_ReadSDInfoFile(void);
#endif

AHC_BOOL 	AHC_SetPreviewZoomConfig(UINT16 bySteps, UINT8 byMaxRatio);
AHC_BOOL 	AHC_SetPreviewZoom(AHC_CAPTURE_MODE CaptureMode, AHC_ZOOM_DIRECTION byDirection, UINT8 bySteps);
AHC_BOOL 	AHC_PlaybackZoom(UINT16 uwStartX, UINT16 uwStartY, UINT16 uwWidth, UINT16 uwHeight);
int			AHC_PreviewWindowOp(int op, AHC_WINDOW_RECT * pRect);
//Video Record Function
UINT32 		AHC_GetVRTimeLimit(void);
UINT32		AHC_SetVRTimeLimit(UINT32 ulTimeLimit);
void        AHC_SetVRWithWNR(AHC_BOOL bEnable);
#if 0 // TBD
AHC_BOOL    AHC_Deletion_DeleteProtectFilebyLockFileNum(void);
#endif
AHC_BOOL    AHC_Deletion_DeleteMinKeyProtectFile(void);
AHC_BOOL 	AHC_Deletion_Romove(AHC_BOOL bFirstTime);
AHC_BOOL    AHC_VRClosePreviousFile(void);
AHC_BOOL    AHC_VRDelPreviousFile(void);
AHC_BOOL	AHC_RestartVRMode(AHC_BOOL b_delete);
AHC_BOOL 	AHC_SetVRVolume(AHC_BOOL bEnable);
AHC_BOOL 	AHC_TransferRGB565toIndex8(UINT32 uiSrcAddr,  UINT32 uiSrcW,  UINT32 uiSrcH, 
									   UINT32 uiDestAddr, UINT32 uiDestW, UINT32 uiDestH,
									   UINT8  byForegroundColor, UINT8  byBackgroundColor);

AHC_BOOL 	AHC_TransferDateToString(AHC_RTC_TIME* psRtcTime, INT8* pbyDate, UINT8 byDateConfig, UINT8 byFormatConfig);
AHC_BOOL 	AIHC_ConfigVRTimeStamp(UINT32 TimeStampOp, AHC_RTC_TIME* sRtcTime, AHC_BOOL bInitIconBuf);
void 		AIHC_UpdateVRTimeStamp(AHC_RTC_TIME* sRtcTime);
UINT32      AHC_GetDefaultAudioSamplingRate(void);
void 		AHC_AvailableRecordTime(UBYTE* Hour, UBYTE* Min, UBYTE* Sec);
void		AHC_AvailableRecordTime_Ex(UINT32 bitrate, UBYTE* Hour, UBYTE* Min, UBYTE* Sec);
UINT8* 		AHC_GetCurrentVRFileName(AHC_BOOL bFullName);
UINT16      AHC_GetCurrentVRDirKey(void);
AHC_BOOL 	AHC_GetVideoRecordEventStatus(void);
AHC_BOOL 	AHC_SetVideoRecordEventStatus(AHC_BOOL trigger);

#if (VR_SEAMLESS)
AHC_BOOL	AHC_IsVRSeamless(void);
#endif
#define AHC_AUDIO_STREAM_ID 0 //should be 0 or 1.
AHC_BOOL AHC_ConfigAudio(UINT16 stream, AHC_AUDIO_FORMAT aFormat,
                         AHC_AUDIO_CHANNEL_CONFIG channelConfig);

AHC_BOOL    AHC_Protect_SpellFileName(char* FilePathName, INT8* pchDirName, INT8* pchFileName);
AHC_PROTECT_TYPE AHC_Protect_GetType(void);
void 		AHC_Protect_SetType(AHC_PROTECT_TYPE Type);
AHC_BOOL 	AHC_Protect_SetVRFile(AHC_PROTECTION_OP Op, AHC_PROTECT_TYPE Type);
//Video/Audio Playback Function
void 		AHC_SetVideoPlayStartTime(MMP_ULONG ulStartTime);
AHC_BOOL 	AHC_GetVideoPlayStopStatus(UINT32 *pwValue);
AHC_BOOL 	AHC_SetVideoPlayStopStatus(UINT32 Value);
AHC_BOOL 	AHC_GetAudioPlayStopStatus(UINT32 *pwValue);
AHC_BOOL 	AHC_SetAudioPlayStopStatus(UINT32 Value);
AHC_BOOL 	AIHC_GetCurrentPBFileType(UINT32 *pFileType);
AHC_BOOL 	AIHC_GetCurrentPBHeight(UINT16 *pHeight);
AHC_BOOL 	AIHC_StopPlaybackMode(void);

//RTC Function
AHC_ERR 	AHC_RTC_UpdateTime(void);
void AHC_RTC_GetTime(AHC_RTC_TIME *psAHC_RTC_Time);
void AHC_RTC_GetTimeEx(AHC_RTC_TIME *psAHC_RTC_Time, AHC_BOOL bUpdateRelativeTime);

AHC_BOOL    AHC_SetClock(UINT16 uiYear, UINT16 uiMonth, UINT16 uiDay, UINT16 uiDayInWeek, UINT16 uiHour, UINT16 uiMinute, UINT16 uiSecond,  UINT8 ubAmOrPm, UINT8 b_12FormatEn);
AHC_BOOL    AHC_GetClock(UINT16* puwYear, UINT16* puwMonth, UINT16* puwDay, UINT16* puwDayInWeek, UINT16* puwHour, UINT16* puwMinute, UINT16* puwSecond, UINT8* ubAmOrPm, UINT8* b_12FormatEn);
AHC_BOOL 	AHC_RtcSetWakeUpEn(AHC_BOOL bEnable, UINT16 uiYear, UINT16 uiMonth, UINT16 uiDay, UINT16 uiHour, UINT16 uiMinute, UINT16 uiSecond, void *phHandleFunc);

//I2C Function
AHC_BOOL 	AHC_ConfigI2CAccess(AHC_I2C_HANDLE* AhcI2cHandle, AHC_I2CM_ATTRIBUTE sI2cAttr);
AHC_BOOL 	AHC_SendI2C(AHC_I2C_HANDLE AhcI2cHandle, UINT16 wSubAdde, UINT16 wData);
AHC_BOOL 	AHC_GetI2C(AHC_I2C_HANDLE AhcI2cHandle, UINT16 wSubAdde, UINT16* pwData);
AHC_BOOL 	AHC_WriteI2CBurst(AHC_I2C_HANDLE AhcI2cHandle, UINT16 wSubAdde, UINT16 wData);
AHC_BOOL 	AHC_ReadI2CBurst(AHC_I2C_HANDLE AhcI2cHandle, UINT16 wSubAdde, UINT16 wData);
AHC_BOOL	AHC_CloseI2C(AHC_I2C_HANDLE AhcI2cHandle);

//UART Function
AHC_BOOL 	AHC_UartLibConfig(UINT8 bBaudRate, UINT8 bPolarity,UINT8 bPadNum, UINT16 wLineCTL, UINT8 bFlowCtl, AHC_UART_HANDLE* AhcUartHandle);
AHC_BOOL 	AHC_UartLibClose(AHC_UART_HANDLE AhcUartHandle);
AHC_BOOL 	AHC_UartLibRead(AHC_UART_HANDLE AhcUartHandle, UINT16 wSize, UINT8 bFlag);
AHC_BOOL 	AHC_UartLibWrite(AHC_UART_HANDLE AhcUartHandle, UINT16 wSize);

//General Function
void 		AHC_SetDelNormFileAfterEmr(UINT8 enable);
UINT8 		AHC_NeedDeleteNormalAfterEmr(void);
AHC_BOOL 	AHC_Initialize(void);
AHC_BOOL 	AHC_SetMode(AHC_MODE_ID uiMode);
AHC_STREAMING_MODE AHC_GetStreamingMode(void);
AHC_BOOL 	AHC_GetSystemStatus(UINT32* pwValue);
AHC_MODE_ID AHC_GetAhcSysMode(void);
void        AHC_SetAhcSysMode(AHC_MODE_ID uiMode);
AHC_BOOL 	AHC_SetMemLocationPtr(AHC_MEMORY_LOCATION_ID wLocation, UINT8* pbBuffer, UINT32 dwSize);
AHC_BOOL 	AHC_UnSetMemLocationPtr(AHC_MEMORY_LOCATION_ID wLocation);
AHC_BOOL 	AHC_UpdateUSP(UINT8 *pData, UINT32 uwSize);
AHC_BOOL 	AHC_UpdateFSP(UINT8 *pData, UINT32 uwSize);
AHC_BOOL 	AHC_RestoreUSP(UINT8 *pData, UINT32 *puwSize);
AHC_BOOL 	AHC_RestoreFSP(UINT8 *pData, UINT32 *puwSize);
AHC_BOOL	AHC_SetTimer(AHC_TIMER_HANDLE* hTimerHandle, UINT32 bTimerID, AHC_TIMER_CMD bTimerCMD, UINT16 wTime);
AHC_BOOL 	AHC_KillTimer(AHC_TIMER_HANDLE hTimerHandle);
AHC_BOOL    AHC_SendAHLMessageEnable(AHC_BOOL Enable);
AHC_BOOL 	AHC_SendAHLMessage(UINT32 ulMsgID, UINT32 ulParam1, UINT32 ulParam2);
AHC_BOOL 	AHC_SendAHLMessage_VIP(UINT32 ulMsgID, UINT32 ulParam1, UINT32 ulParam2);
AHC_BOOL 	AHC_SendAHLMessage_HP(UINT32 ulMsgID, UINT32 ulParam1, UINT32 ulParam2);
AHC_BOOL 	AHC_GetAHLMessage(UINT32* ulMsgID, UINT32* ulParam1, UINT32* ulParam2);
AHC_BOOL 	AHC_GetAHLMessage_HP(UINT32* ulMsgID, UINT32* ulParam1, UINT32* ulParam2);
void 		AHC_GetAHLHPMessageCount(UINT16 *usCount);
AHC_BOOL 	AIHC_InitAHLMessage(void);
AHC_BOOL 	AIHC_InitAHLMessage_HP(void);
AHC_BOOL 	AHC_WaitForBootComplete(void);
ULONG 		AHC_OsCounterGet(void);
ULONG 		AHC_OsCounterGetFreqHz(void);
ULONG 		AHC_OsCounterGetFreqKhz(void);
AHC_BOOL	AHC_LoadSystemFile(void);
AHC_BOOL	AHC_UnloadSystemFile(void);

//GPIO Function
AHC_BOOL    AHC_ConfigGPIOPad(AHC_PIO_REG piopin, MMP_UBYTE config);
AHC_BOOL 	AHC_ConfigGPIO(AHC_PIO_REG ahc_piopin, UINT8 bDirection);
AHC_BOOL 	AHC_ConfigGpioFunc(AHC_PIO_REG ahc_piopin, AHC_PIO_TRIGMODE bPolarity, UINT32 uiCount);
AHC_BOOL 	AHC_SetGPIOFunc(AHC_PIO_REG ahc_piopin, void* (phHandleFunc), UINT8 bEnable);
AHC_BOOL 	AHC_SetGPIO(AHC_PIO_REG ahc_piopin, UINT8 bState);
AHC_BOOL 	AHC_GetGPIO(AHC_PIO_REG ahc_piopin, UINT8 *pwState);

//PWM Function
AHC_BOOL 	AHC_PWM_Initialize(void);
AHC_BOOL 	AHC_PWM_OutputPulse(AHC_PWMPIN pwm_pin, AHC_BOOL bEnableIoPin, UINT32 ulFrquency, AHC_BOOL bHigh2Low, AHC_BOOL bEnableInterrupt, void* pwm_callBack, UINT32 ulNumOfPulse);
AHC_BOOL 	AHC_PWM_OutputPulseEx(AHC_PWMPIN pwm_pin, AHC_BOOL bEnableIoPin, UINT32 ulFrquency, UINT32 ulDuty, AHC_BOOL bHigh2Low, AHC_BOOL bEnableInterrupt, void* pwm_callBack, UINT32 ulNumOfPulse);

//Power Function
AHC_BOOL 	AHC_IsPowerLow(void);
void 		AHC_PowerOff_Preprocess(void);
void        AHC_VirtualPowerOffPath(void);
void	 	AHC_NormalPowerOffPath(void);
AHC_ERR 	AHC_PowerOffPicture(INT8 *charStr);
ULONG 		AHC_BatteryDetection_Once(void);
AHC_BOOL AHC_IsPowerPressed(void);


//Sound Effect Function
AHC_BOOL 	AHC_IsSpeakerEnable(void);
AHC_BOOL 	AHC_SpeakerEnable(AHC_PIO_REG piopin, AHC_BOOL enable);
AHC_BOOL 	AHC_GetSoundEffectStatus(UINT8 *ubSoundEffectStatus);
AHC_BOOL 	AHC_PlaySoundEffect(AHC_SOUNDEFFECT_TYPE soundtype);
AHC_BOOL 	AHC_SetSoundEffectExist(AHC_BOOL bExist);
UINT8 		AHC_GetSoundEffectExist(void);
AHC_BOOL 	AHC_LoadSoundEffect(void);

AHC_BOOL 		Menu_SetMotionDtcSensitivity(UINT8 val);

//LCD Function
void 		AHC_LCD_SetBrightness(UINT8 brightness);
void 		AHC_LCD_SetContrast(UINT8 Contrast);
void 		AHC_LCD_SetContrast_R(UINT8 Contrast);
void 		AHC_LCD_SetBrightness_R(UINT8 brightness);
void 		AHC_LCD_SetContrast_B(UINT8 Contrast);
void 		AHC_LCD_SetBrightness_B(UINT8 brightness);
void 		AHC_LCD_Direction(AHC_LCD_DIRECTION dir);
void 		AHC_LCD_Write_Reg(UINT32 reg, UINT8 value);
void 		AHC_LCD_GetStatus(UINT8 *status);

//Misc Function
AHC_BOOL 	AHC_SetLsCsCali(void);
AHC_BOOL 	AHC_SetAfCali(void);
AHC_BOOL 	AHC_SetAwb0Cali(void);
AHC_BOOL 	AHC_SetAeAwb1Cali(void);
AHC_BOOL 	AHC_SetRdCaliBin(void);
AHC_BOOL 	AHC_CaptureRawAfterCS(void);
AHC_BOOL 	AHC_Gsensor_SetCurInfo(void);
AHC_BOOL 	AHC_Gsensor_SetIntThreshold(void);
AHC_BOOL 	AHC_GPS_SetCurInfo(void);
int			AHC_GetRAMExtension(void);
UINT32      AHC_GetPreRecordTime(void);
AHC_BOOL 	AHC_Gsensor_Polling(UINT8 ubType);
AHC_BOOL 	AHC_Gsensor_Module_Attached(void);
AHC_BOOL 	AHC_GPS_Module_Attached(void);

//Menu Setting Function
AHC_BOOL 	Menu_SetGsensorSensitivity(UINT8 val);
AHC_BOOL 	Menu_SetLCDDirection(UINT8 val, UINT8 backupVal);

//SD Card Function
#if (ENABLE_SD_HOT_PLUGGING)
SDMMC_STATE AHC_SDMMC_GetState(void);
void 		AHC_SDMMC_SetState(SDMMC_STATE val);
AHC_BOOL 	AHC_SDMMC_GetMountState(void);
void 		AHC_SDMMC_SetMountState(SDMMC_STATE val);
AHC_BOOL 	AHC_SDMMC_IsSD1MountDCF(void);
#endif
UINT8 		AHC_SDMMC_GetClass(UINT32 id);
UINT8 		AHC_SDMMC_Get1stCardClass(UINT32 id);
#if(defined(DEVICE_GPIO_2NDSD_PLUG))
AHC_BOOL 	AHC_SDMMC_IsSD2MountDCF(void);
UINT8 		AHC_SDMMC_Get2ndCardClass(UINT32 id);
#endif

#if (SD_UPDATE_FW_EN)
void AHC_SDUpdateMode(void);
#endif

//Others
void 		AHC_PMU_PowerOff(void);
UINT32 		AHC_GetChargerPowerOffTime(void);

//PIR
#if (ENABLE_PIR_MODE)
void AHC_CheckPIRStart(void) ;
void AHC_EnablePIR(AHC_BOOL bEnable); 
#endif
AHC_BOOL AHC_IsPIRStart(void);

AHC_BOOL AHC_Check2SysCalibMode(void);

#if (MOTION_DETECTION_EN)
AHC_BOOL AHC_GetMotionDetectionStatus(void);
AHC_BOOL AHC_SetMotionDetectionStatus(AHC_BOOL bEn);
AHC_BOOL AHC_EnableMotionDetection(AHC_BOOL bEn);
AHC_BOOL AHC_GetMotionDtcSensitivity(UINT8 *pubMvTh, UINT8 *pubCntTh);
int      AHC_SetMotionDtcSensitivity(unsigned char mvth,  unsigned char cnth);
UINT32   AHC_GetVMDRecTimeLimit(void);
UINT32   AHC_GetVMDShotNumber(void);
void     AHC_AdjustVMDPreviewRes(int *w, int *h);
#endif

void AHC_Charger_IO_Initial(void);
void AHC_SetChargerEnable(AHC_BOOL bEn);
AHC_BOOL AHC_GetChargerEnableStatus(void);
void AHC_SetTempCtrlEnable(AHC_BOOL bEn);
AHC_BOOL AHC_GetTempCtrlStatus(void);
AHC_BOOL AHC_GetChargerStatus(void);

void AHC_SetVidRecdCardSlowStop(AHC_BOOL bState);
AHC_BOOL AHC_GetVidRecdCardSlowStop(void) ;

#if ((EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_DUAL_FILE) || (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE))
AHC_BOOL AHC_SetKernalEmergencyStopStep(AHC_KERNAL_EMERGENCY_STOP_STEP bKernalEmergencyStopStep);
AHC_KERNAL_EMERGENCY_STOP_STEP AHC_GetKernalEmergencyStopStep(void);
#endif


void AHC_SetAPStopVideoRecord(AHC_BOOL bState);
AHC_BOOL AHC_GetAPStopVideoRecord(void) ;
void AHC_SetAPStopVideoRecordTime(ULONG ulLastRecdVidTime, ULONG ulLastRecdAudTime);
AHC_BOOL AHC_GetAPStopVideoRecordTime(ULONG *ulLastRecdVidTime, ULONG *ulLastRecdAudTime);

void AHC_SetEditBrowserMode(EDIT_BROWSER_MODE Mode);
EDIT_BROWSER_MODE AHC_GetEditBrowserMode(void);
AHC_BOOL AHC_ProcessOneFile(AHC_FILE_OP op, PFDrawBarCB pFuncPtr);
AHC_BOOL AHC_ProcessAllFile(AHC_FILE_OP op, INT8* pchChar, PFDrawBarCB pFuncPtr);
void VideoMediaErrorPlayStopHandler(void);
AHC_BOOL AHC_Deletion_Confirm(DCF_DB_TYPE sDB, UINT32 uiTime);
AHC_BOOL AHC_Deletion_ConfirmByNum(DCF_DB_TYPE sDB, UINT32 uiTime);
AHC_BOOL AHC_Deletion_RemoveEx(DCF_DB_TYPE sDB, UINT32 uiTime);

void AIHC_AdjustJpegFromSensor(UINT16 w, UINT16 h, UINT16* suggestedWidth, UINT16* suggestedHeight);
AHC_BOOL AHC_GetVideoResolution(UINT16 stream, UINT16 *w, UINT16 *h);
UINT32 AHC_GetAudioSamplingRate( UINT8 uiAudioType );

AHC_BOOL AHC_Check2SysCalibMode2(void);
AHC_BOOL AHC_IsEmerRecordStarted(void);
AHC_BOOL AHC_SetEmerRecordStarted(AHC_BOOL bEmerRecordStarted);
AHC_BOOL AHC_IsEmerPostDone(void);
AHC_BOOL AHC_StartEmerRecord(void);
AHC_BOOL AHC_GetEmerRecordTime(UINT32 *uiTime);
AHC_BOOL AHC_GetEmerRecordTimeOffset(UINT32 *uiTime);
AHC_BOOL AHC_SetEmerRecordInterval(UINT32 uiInterval);
UINT32	 AHC_GetEmerRecordInterval(void);
AHC_BOOL AHC_EmerRecordPostProcess(void);
AHC_BOOL AHC_EmerRecordPostProcess_MediaError(void);
AHC_BOOL AHC_StopEmerRecord(void);
AHC_BOOL AHC_GetVRVolumeParam(UINT8* pubDgain, UINT8* pubAGain, AHC_BOOL* pbBoosterOn);
AHC_BOOL AHC_GetVRVolume(ULONG* piDGain, ULONG* piLAGain, ULONG* piRAGain, AHC_BOOL* pbBoosterOn);
AHC_BOOL AHC_SetVRVolumeParam(UINT8 ubDgain, UINT8 ubAGain, AHC_BOOL bBoosterOn);
AHC_BOOL AHC_SetVideoRecordVolume(AHC_BOOL bEnable);
AHC_BOOL AHC_ParkingModePostProcess(void);
AHC_BOOL Menu_SetMotionDtcSensitivity_Parking(UINT8 val);
AHC_BOOL AHC_SDMMC_BasicCheck(void);
AHC_BOOL AHC_SetEmerRecorded(AHC_BOOL bEmerRecored);
AHC_BOOL AHC_GetEmerRecorded(void);
#if (C005_PARKING_CLIP_Length_10PRE_10REC)
UINT32 AHC_GetParkiingRecordTimeLimit(void);
AHC_BOOL AHC_SetParkingRecordTime(UINT32 ulTimeLimit);
#endif
AHC_BOOL AHC_ParkingModeRecStop(void);

void AHC_SetNormal2Emergency(AHC_BOOL bState);
void AHC_BuzzerAlert(UINT32 ulFrquency, UINT32 ulTimes, UINT32 ulMs);
void AHC_NormalPowerOffPathEx(AHC_BOOL bForce, AHC_BOOL bByebye, AHC_BOOL bSound);

AHC_BOOL AIHC_SetVideoRecordMode(UINT8 bAudioOn, AHC_VIDRECD_CMD bStartRecord);
AHC_BOOL AIHC_StopVideoRecordMode(AHC_BOOL bStopRecord,AHC_BOOL bShutdownSensor);
void AHC_SetFastAeAwbMode( AHC_BOOL bEnable );
AHC_BOOL AHC_GetFastAeAwbMode( void );
void     AHC_SetYFrameType( UINT8 byType );

#if(SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
void		uiSetParkingModeStateInit(void);
void		uiSetParkingAutoStop(AHC_BOOL enable);
AHC_BOOL	uiGetParkingAutoStop(void);
void		uiSetParkingModeEnable(UINT8 enable);

void		uiSetParkingModeIdling(AHC_BOOL enable);
AHC_BOOL	uiGetParkingModeIdlingEnable(void);

void		uiSetParkingModeMenuState(AHC_BOOL enable, AHC_BOOL ParkingStartDrawed);

void		uiSetParkingModeRecordState(AHC_BOOL enable);

void		uiSetParkingModeRecord(AHC_BOOL recorded);
AHC_BOOL	uiGetParkingModeRecord(void);

void		uiSetParkingSpaceCheckEnable(AHC_BOOL enable);
AHC_BOOL	uiGetParkingSpaceCheckState(void);	

void		uiSetParkingMoveHintState(AHC_BOOL enable, AHC_BOOL ParkingStartDrawed);
AHC_BOOL	uiGetParkingMoveHintState(void);

AHC_BOOL uiGetParkingStartDrawed(void);

AHC_BOOL uiGetParkingModeExitState(void);
void uiSetParkingExitEnable(AHC_BOOL enable);
AHC_BOOL uiSetBootUpUIMode(void);
#endif

UINT8		uiGetParkingModeEnable(void);
AHC_BOOL	uiGetParkingModeMenuState(void);
AHC_BOOL	uiGetParkingModeRecordState(void);
void        AIHC_SelectYFrameUsage( void );

AHC_BOOL	AHC_EnableADAS(AHC_BOOL bEn);
AHC_BOOL	AHC_IsADASEnabled( void );

ULONG       AHC_CheckWiFiOnOffInterval(MMP_ULONG waiting_time);
AHC_BOOL    AHC_WiFi_Switch(AHC_BOOL wifi_switch_on);
void        AHC_WiFi_Toggle_StreamingMode(void);

void	    AHC_RestartPreviewAsRecordingSetting( AHC_BOOL bEnable );
void	    AHC_VideoRecordStartWriteInfo( void );

#define     AHC_SetVideoFileTimeLimit           AHC_SetVRTimeLimit

// HDMI-out Focus
#if (SUPPORT_HDMI_OUT_FOCUS)
AHC_BOOL    AHC_Check2HdmiOutFocus(void);
#endif

#if defined(AIT_HW_WATCHDOG_ENABLE) && (AIT_HW_WATCHDOG_ENABLE)
void AHC_WD_Enable(AHC_BOOL bEnable);
void AHC_WD_Kick(void);
#endif

#if CLOCK_SETTING_TIME_LIMIT
void        AHC_SetClockSettingTimeLimit(INT32 ms);
#endif

void DualEncRecordCbFileFullCB(void);

AHC_BOOL AHC_KeyEventIDCheckConflict(UINT32 ulCurKeyEventID);
void AHC_SetCurKeyEventID(UINT32 ulCurKeyEventID);
UINT32 AHC_GetCurKeyEventID(void);

AHC_BOOL AHC_RestoreFromDefaultSetting(void);

#endif		// _AHC_GENERAL_H_


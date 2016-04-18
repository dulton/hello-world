//==============================================================================
//
//  File        : AHC_Common.h
//  Description : INCLUDE File for the AHC common function porting.
//  Author      :
//  Revision    : 1.0
//
//==============================================================================

#ifndef _AHC_COMMON_H_
#define _AHC_COMMON_H_

/*===========================================================================
 * Type define
 *===========================================================================*/
typedef unsigned char 		AHC_BOOL;
typedef unsigned char 		UINT8;
typedef unsigned short 		UINT16;
typedef unsigned int   		UINT;
typedef unsigned int  		UINT32;
typedef unsigned long 		ULONG;
typedef unsigned long*      PULONG; //Andy Liu TBD
typedef unsigned long 		ULONG32;
typedef unsigned long long 	UINT64;
typedef unsigned char  		UBYTE;
typedef long long    		INT64;
typedef int                 INT;//Andy Liu TBD
typedef signed int   		INT32;
typedef signed short 		INT16;
typedef char         		INT8;
#ifndef BYTE
typedef signed char  		BYTE;//Andy Liu TBD
#endif
#ifndef PBYTE
typedef unsigned char*  	PBYTE;//Andy Liu TBD
#endif

#define AHC_PRINT_RET_ERROR(force_stop,ret_val) {printc(FG_RED("%s,L:%d Err:0x%x!\r\n"),__func__,__LINE__, ret_val); if(force_stop) {while(1);}}

/*===========================================================================
 * Macro define
 *===========================================================================*/

#define AHC_FAIL        (1)
#define AHC_SUCCESS     (0)

#define AHC_TRUE		(1)
#define AHC_FALSE		(0)

#define	MODULE_ERR_SHIFT                    24

#define COMMON_KEY_SCENE                    "SCENE"
#define COMMON_KEY_METER                    "METER"
#define COMMON_KEY_EV                       "EV"
#define COMMON_KEY_ISO                      "ISO"
#define COMMON_KEY_WB                       "WB"
#define COMMON_KEY_TV_SYSTEM                "TVSYS"
#define COMMON_KEY_GPS_RECORD_ENABLE        "GPSRecEn"
#define COMMON_KEY_VR_CLIP_TIME             "VideoClipTime"
#define COMMON_KEY_VR_TIMELAPSE             "VideoTimelapseEn"	// VIDEO_REC_TIMELAPSE_EN
#define COMMON_KEY_LDWS_EN                  "LDWS_EN"
#define COMMON_KEY_FCWS_EN                  "FCWS_EN"
#define COMMON_KEY_SAG_EN                   "SAG_EN"
#define COMMON_KEY_WIFI_EN                  "WiFi"
#define COMMON_KEY_DRIVER_ID                "DriverID"
#define COMMON_KEY_STICKER_LOGO_TXT         "TimeStampLogoTXT"
#define COMMON_KEY_MIC_SENSITIVITY          "MicSensitivity"
#define COMMON_KEY_MOVIE_SIZE               "VideoSize"
#define COMMON_KEY_STILL_CAP_SIZE           "StillSize"
#define COMMON_KEY_STILL_QUALITY            "StillQuality"
#define COMMON_KEY_STILL_STABILIZE          "StillStabilize"
#define COMMON_KEY_STILL_FLASH              "StillFlash"
#define COMMON_KEY_STILL_FLASH_LEVEL        "StillFlashLevel"
#define COMMON_KEY_RED_EYE_REDUCE           "RedEyeReduce"
#define COMMON_KEY_SELF_TIMER               "SelfTimer"
#define COMMON_KEY_BURST_SHOT               "BurstShot"
#define COMMON_KEY_WDR_EN                   "WDR"
#define COMMON_KEY_HDR_EN                   "HDR"
#define COMMON_KEY_GSENSOR_PWR_ON_SENS      "GSensorPowerOnSens"
#define COMMON_KEY_MOTION_SENS              "MotionSensitivity"
#define COMMON_KEY_FLICKER                  "Flicker"
#define COMMON_KEY_GSENSOR_SENS             "GSensorSensitivity"
#define COMMON_KEY_SPEED_CAR                "SpeedCar"
#define COMMON_KEY_DISTANCE_UINT            "DistanceUnit"
#define COMMON_KEY_SPEED_CAR_CRUISE_SETTING "SpeedCamCruiseSpeedSetting"
#define COMMON_KEY_FATIGUE_ALERT            "FatigueAlert"
#define COMMON_KEY_REMIND_HEADLIGHT         "RemindHeadlight"
#define COMMON_KEY_LANGUAGE                 "Language"
#define COMMON_KEY_STATUS_LIGHT             "StatusLight"
#define COMMON_KEY_PARK_TYPE                "ParkingType"
#define COMMON_KEY_CONTRAST                 "Contrast"
#define COMMON_KEY_SATURATION               "Saturation"
#define COMMON_KEY_SHARPNESS                "Sharpness"
#define COMMON_KEY_GAMMA                    "Gamma"
#define COMMON_KEY_SLOWMOTION_EN        	"SlowMotion"
#define COMMON_KEY_WNR_EN        	        "WNR"
#define COMMON_KEY_PREVIEW_MODE             "PreviewMode"//CarDV~

//Common definition
#define COMMON_DRAM_SIZE_NG (0)
#define COMMON_DRAM_SIZE_32MB (COMMON_DRAM_SIZE_NG + 1)
#define COMMON_DRAM_SIZE_64MB (COMMON_DRAM_SIZE_32MB + 1)
#define COMMON_DRAM_SIZE_128MB (COMMON_DRAM_SIZE_64MB + 1)
#define COMMON_DRAM_SIZE_256MB (COMMON_DRAM_SIZE_128MB + 1)

#define COMMON_VR_VIDEO_TYPE_NG (0)
#define COMMON_VR_VIDEO_TYPE_3GP (COMMON_VR_VIDEO_TYPE_NG + 1)
#define COMMON_VR_VIDEO_TYPE_AVI (COMMON_VR_VIDEO_TYPE_3GP + 1)

#define COMMON_VR_VIDEO_TYPE_SETTING_PROFILE_NG (0)
#define COMMON_VR_VIDEO_TYPE_SETTING_H263ENC_PROFILE_NONE (COMMON_VR_VIDEO_TYPE_SETTING_PROFILE_NG + 1)
#define COMMON_VR_VIDEO_TYPE_SETTING_H263ENC_PROFILE_0 (COMMON_VR_VIDEO_TYPE_SETTING_H263ENC_PROFILE_NONE + 1)
#define COMMON_VR_VIDEO_TYPE_SETTING_H263ENC_PROFILE_3 (COMMON_VR_VIDEO_TYPE_SETTING_H263ENC_PROFILE_0 + 1)
#define COMMON_VR_VIDEO_TYPE_SETTING_H263ENC_PROFILE_MAX (COMMON_VR_VIDEO_TYPE_SETTING_H263ENC_PROFILE_3 + 1)
#define COMMON_VR_VIDEO_TYPE_SETTING_MP4VENC_PROFILE_NONE (COMMON_VR_VIDEO_TYPE_SETTING_H263ENC_PROFILE_MAX + 1)
#define COMMON_VR_VIDEO_TYPE_SETTING_MP4VENC_SIMPLE_PROFILE (COMMON_VR_VIDEO_TYPE_SETTING_MP4VENC_PROFILE_NONE + 1)
#define COMMON_VR_VIDEO_TYPE_SETTING_MP4VENC_ADV_SIMPLE_PROFILE (COMMON_VR_VIDEO_TYPE_SETTING_MP4VENC_SIMPLE_PROFILE + 1)
#define COMMON_VR_VIDEO_TYPE_SETTING_MP4VENC_PROFILE_MAX (COMMON_VR_VIDEO_TYPE_SETTING_MP4VENC_ADV_SIMPLE_PROFILE + 1)
#define COMMON_VR_VIDEO_TYPE_SETTING_H264ENC_PROFILE_NONE (COMMON_VR_VIDEO_TYPE_SETTING_MP4VENC_PROFILE_MAX + 1)
#define COMMON_VR_VIDEO_TYPE_SETTING_H264ENC_BASELINE_PROFILE (COMMON_VR_VIDEO_TYPE_SETTING_H264ENC_PROFILE_NONE + 1)
#define COMMON_VR_VIDEO_TYPE_SETTING_H264ENC_MAIN_PROFILE (COMMON_VR_VIDEO_TYPE_SETTING_H264ENC_BASELINE_PROFILE + 1)
#define COMMON_VR_VIDEO_TYPE_SETTING_H264ENC_HIGH_PROFILE (COMMON_VR_VIDEO_TYPE_SETTING_H264ENC_MAIN_PROFILE + 1)
#define COMMON_VR_VIDEO_TYPE_SETTING_H264ENC_PROFILE_MAX (COMMON_VR_VIDEO_TYPE_SETTING_H264ENC_HIGH_PROFILE + 1)

#define COMMON_VR_VIDEO_CURBUF_NG       (0)
#define COMMON_VR_VIDEO_CURBUF_FRAME    (COMMON_VR_VIDEO_CURBUF_NG + 1)
#define COMMON_VR_VIDEO_CURBUF_RT       (COMMON_VR_VIDEO_CURBUF_FRAME + 1)

/*===========================================================================
 * Structure define
 *===========================================================================*/

typedef struct
{
	UINT16 PointX;
	UINT16 PointY;
} POINT, *PPOINT;

typedef struct _RECT
{
	UINT16 uiLeft;
	UINT16 uiTop;
	UINT16 uiWidth;
	UINT16 uiHeight;
} RECT, *PRECT;

typedef struct _RECTOFFSET
{
	INT16 uiOffX;
	INT16 uiOffY;
	INT16 uiOffW;
	INT16 uiOffH;
} RECTOFFSET, *PRECTOFFSET;

/*===========================================================================
 * Enum define
 *===========================================================================*/

typedef enum _AHC_MODULE
{
    AHC_HIF 		= 0,
    AHC_SYSTEM 		= 1,
    AHC_SENSOR 		= 2,
    AHC_VIF 		= 3,
    AHC_ISP 		= 4,
    AHC_SCALER 		= 5,
    AHC_ICON 		= 6,
    AHC_IBC 		= 7,
	AHC_GRA			= 8,
	AHC_INT 		= 9,
	AHC_DSPY 		= 10,
	AHC_DRAM 		= 11,
	AHC_I2CM 		= 12,
	AHC_PSPI 		= 13,
	AHC_DMA 		= 14,
	AHC_SD 			= 15,
	AHC_NAND 		= 16,
	AHC_MP4VE 		= 17,
	AHC_MP4VD 		= 18,
	AHC_H264D 		= 19,
	AHC_USB 		= 20,
	AHC_FS 			= 21,
	AHC_3GPMGR 		= 22,
	AHC_3GPPSR 		= 23,
	AHC_AUDIO 		= 24,
	AHC_DSC 		= 25,
	AHC_FCTL 		= 26,
    AHC_VCONF 		= 27,
    AHC_3GPPLAY 	= 28,
    AHC_3GPRECD 	= 29,
    AHC_UART 		= 30,
    AHC_MDTV 		= 31,
    AHC_SPI 		= 32,
    AHC_PLL 		= 33,
    AHC_USER 		= 34,
    AHC_VT 			= 35,
    AHC_CCIR 		= 36,
    AHC_STORAGE 	= 37,
    AHC_PIO 		= 38,
    AHC_MFD 		= 39,
    AHC_PWM			= 40,
	AHC_RAWPROC 	= 41,
	AHC_SIF 		= 42,
	AHC_EVENT 		= 43,
	AHC_MJPGD 		= 44,
	AHC_BAYER		= 45,
	AHC_LDC			= 46,
	AHC_RTC     	= 47
} AHC_MODULE;

// DUPLICATED_DEFINITION: From _MMP_ERR in MMP_ERR.h.
typedef enum _AHC_ERR
{
	AHC_ERR_NONE = 0x00000000,

    // AHC_HIF
    AHC_HIF_ERR_PARAMETER = (AHC_HIF << MODULE_ERR_SHIFT) | 0x000001,
	AHC_HIF_ERR_MODE_NOT_SET,

    // AHC_SYSTEM
    AHC_SYSTEM_ERR_PARAMETER = (AHC_SYSTEM << MODULE_ERR_SHIFT) | 0x000001,
    AHC_SYSTEM_ERR_CMDTIMEOUT,
	AHC_SYSTEM_ERR_HW,
	AHC_SYSTEM_ERR_CPUBOOT,
    AHC_SYSTEM_ERR_SETOPMODE,
    AHC_SYSTEM_ERR_NOT_IMPLEMENTED,
	AHC_SYSTEM_ERR_SETAPMODE,
	AHC_SYSTEM_ERR_GET_FW,
	AHC_SYSTEM_ERR_SETPSMODE,
	AHC_SYSTEM_ERR_VERIFY_FW,
	AHC_SYSTEM_ERR_SETPLL,
	AHC_SYSTEM_ERR_REGISTER_TEST_FAIL,
	AHC_SYSTEM_ERR_MEMORY_TEST_FAIL,
    AHC_SYSTEM_ERR_NOT_SUPPORT,
    AHC_SYSTEM_ERR_MALLOC,
	AHC_SYSTEM_ERR_FORMAT,
	AHC_SYSTEM_ERR_TIMER,
	AHC_SYSTEM_ERR_PMU,
	AHC_SYSTEM_ERR_DOUBLE_SET_ALARM,
	AHC_SYSTEM_ERR_ADC,

    // AHC_SENSOR
    AHC_SENSOR_ERR_PARAMETER = (AHC_SENSOR << MODULE_ERR_SHIFT) | 0x000001,
	AHC_SENSOR_ERR_INITIALIZE,
	AHC_SENSOR_ERR_INITIALIZE_NONE,
	AHC_SENSOR_ERR_FDTC,
	AHC_SENSOR_ERR_SETMODE,
	AHC_SENSOR_ERR_AF_MISS,

    // AHC_VIF
    AHC_VIF_ERR_PARAMETER = (AHC_VIF << MODULE_ERR_SHIFT) | 0x000001,

    // AHC_ISP
    AHC_ISP_ERR_PARAMETER = (AHC_ISP << MODULE_ERR_SHIFT) | 0x000001,

    // AHC_SCALER
    AHC_SCALER_ERR_PARAMETER = (AHC_SCALER << MODULE_ERR_SHIFT) | 0x000001,
    AHC_SCALER_ERR_CMDTIMEOUT,

    // AHC_ICON
    AHC_ICON_ERR_PARAMETER = (AHC_ICON << MODULE_ERR_SHIFT) | 0x000001,
	AHC_ICON_ERR_CMDTIMEOUT,

	// AHC_IBC
    AHC_IBC_ERR_PARAMETER = (AHC_IBC << MODULE_ERR_SHIFT) | 0x000001,
    AHC_IBC_ERR_CMDTIMEOUT,

	// AHC_GRA
    AHC_GRA_ERR_PARAMETER = (AHC_GRA << MODULE_ERR_SHIFT) | 0x000001,
    AHC_GRA_ERR_CMDTIMEOUT,
    AHC_GRA_ERR_HW,
    AHC_GRA_ERR_NOT_IMPLEMENT,

	// AHC_INT
    AHC_INT_ERR_PARAMETER = (AHC_INT << MODULE_ERR_SHIFT) | 0x000001,

	// AHC_DISPLAY
    AHC_DISPLAY_ERR_PARAMETER = (AHC_DSPY << MODULE_ERR_SHIFT) | 0x000001,
    AHC_DISPLAY_ERR_PRM_NOT_INITIALIZE,
    AHC_DISPLAY_ERR_SCD_NOT_INITIALIZE,
    AHC_DISPLAY_ERR_NON_CONTROLLER_ENABLE,
    AHC_DISPLAY_ERR_NOT_SUPPORT,
    AHC_DISPLAY_ERR_HW,
    AHC_DISPLAY_ERR_OVERRANGE,
    AHC_DISPLAY_ERR_NOT_IMPLEMENTED,
	AHC_DISPLAY_ERR_INSUFFICIENT_OSDMEMORY,
	AHC_DISPLAY_ERR_RGBLCD_NOT_ENABLED,
	AHC_DISPLAY_ERR_LCD_BUSY,
	AHC_DISPLAY_ERR_FRAME_END,

	// AHC_DRAM
    AHC_DRAM_ERR_PARAMETER = (AHC_DRAM << MODULE_ERR_SHIFT) | 0x000001,
    AHC_DRAM_ERR_INITIALIZE,
    AHC_DRAM_ERR_NOT_SUPPORT,

	// AHC_I2CM
    AHC_I2CM_ERR_PARAMETER = (AHC_I2CM << MODULE_ERR_SHIFT) | 0x000001,
	AHC_I2CM_ERR_SLAVE_NO_ACK,
	AHC_I2CM_ERR_READ_TIMEOUT,

	// AHC_PSPI
    AHC_PSPI_ERR_PARAMETER = (AHC_PSPI << MODULE_ERR_SHIFT) | 0x000001,

    // AHC_DMA
    AHC_DMA_ERR_PARAMETER = (AHC_DMA << MODULE_ERR_SHIFT) | 0x000001,
	AHC_DMA_ERR_OTHER,
	AHC_DMA_ERR_NOT_SUPPORT,
	AHC_DMA_ERR_BUSY,

    // AHC_SD
    AHC_SD_ERR_COMMAND_FAILED = (AHC_SD << MODULE_ERR_SHIFT) | 0x000001,
    AHC_SD_ERR_RESET,
    AHC_SD_ERR_PARAMETER,
    AHC_SD_ERR_DATA,
    AHC_SD_ERR_NO_CMD,
    AHC_SD_ERR_BUSY,
    AHC_SD_ERR_CARD_REMOVED,
    AHC_SD_NO_SPACE,

    // AHC_NAND
    AHC_NAND_ERR_PARAMETER = (AHC_NAND << MODULE_ERR_SHIFT) | 0x000001,
    AHC_NAND_ERR_RESET,
    AHC_NAND_ERR_HW_INT_TO,
    AHC_NAND_ERR_ECC,
    AHC_NAND_ERR_NOT_COMPLETE,
    AHC_NAND_ERR_ERASE,
    AHC_NAND_ERR_PROGRAM,
    AHC_NAND_ERR_ECC_CORRECTABLE,

	// AHC_MP4VENC
    AHC_MP4VE_ERR_PARAMETER = (AHC_MP4VE << MODULE_ERR_SHIFT) | 0x000001,
	AHC_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS,
	AHC_MP4VE_ERR_WRONG_STATE_OP,
	AHC_MP4VE_ERR_ARRAY_IDX_OUT_OF_BOUND,
	AHC_MP4VE_ERR_QUEUE_OVERFLOW,
	AHC_MP4VE_ERR_QUEUE_UNDERFLOW,

	// AHC_MP4VD
    AHC_MP4VD_ERR_BASE = AHC_MP4VD << MODULE_ERR_SHIFT, // 0x12
	    /** One or more parameters were not valid.
	    The input parameters are supported but are not valid value. E.g. it's out of range.*/
        AHC_MP4VD_ERR_PARAMETER = AHC_MP4VD_ERR_BASE | 0x1005,
        /** The buffer was emptied before the next buffer was ready */
        AHC_MP4VD_ERR_UNDERFLOW = AHC_MP4VD_ERR_BASE | 0x1007,
        /** The buffer was not available when it was needed */
        AHC_MP4VD_ERR_OVERFLOW = AHC_MP4VD_ERR_BASE | 0x1008,
        /** The hardware failed to respond as expected */
        AHC_MP4VD_ERR_HW = AHC_MP4VD_ERR_BASE | 0x1009,
        /** Stream is found to be corrupt */
        AHC_MP4VD_ERR_STREAM_CORRUPT = AHC_MP4VD_ERR_BASE | 0x100B,
        /** The component is not ready to return data at this time */
        AHC_MP4VD_ERR_NOT_READY = AHC_MP4VD_ERR_BASE | 0x1010,
        /** There was a timeout that occurred */
        AHC_MP4VD_ERR_TIME_OUT = AHC_MP4VD_ERR_BASE | 0x1011,
        /** Attempting a command that is not allowed during the present state. */
        AHC_MP4VD_ERR_INCORRECT_STATE_OPERATION = AHC_MP4VD_ERR_BASE | 0x1018,
        /** The values encapsulated in the parameter or config structure are not supported. */
        AHC_MP4VD_ERR_UNSUPPORTED_SETTING = AHC_MP4VD_ERR_BASE | 0x1019,

	// AHC_H264D
    AHC_H264D_ERR_BASE = AHC_H264D << MODULE_ERR_SHIFT, // 0x13
	    /** There were insufficient resources to perform the requested operation
	    E.g. The bitstream buffer is overflow. Since the video bitstream buffer is a
	    plain buffer, the buffer is not able to be full loaded when one video
	    frame is greater than the buffer.  For the size of the video bitstream
	    buffer, refer AIT for more detail.*/
        AHC_H264D_ERR_INSUFFICIENT_RESOURCES = AHC_H264D_ERR_BASE | 0x1000,
	    /** One or more parameters were not valid.
	    The input parameters are supported but are not valid value. E.g. it's out of range.*/
        AHC_H264D_ERR_PARAMETER = AHC_H264D_ERR_BASE | 0x1005,
        /** The buffer was emptied before the next buffer was ready */
        AHC_H264D_ERR_UNDERFLOW = AHC_H264D_ERR_BASE | 0x1007,
        /** The buffer was not available when it was needed */
        AHC_H264D_ERR_OVERFLOW = AHC_H264D_ERR_BASE | 0x1008,
        /** The hardware failed to respond as expected */
        AHC_H264D_ERR_HW = AHC_H264D_ERR_BASE | 0x1009,
        /** The component is in the state AHC_STATE_INVALID */
        AHC_H264D_ERR_INVALID_STATE = AHC_H264D_ERR_BASE | 0x100A,
        /** Stream is found to be corrupt */
        AHC_H264D_ERR_STREAM_CORRUPT = AHC_H264D_ERR_BASE | 0x100B,
        /** The component is not ready to return data at this time */
        AHC_H264D_ERR_NOT_READY = AHC_H264D_ERR_BASE | 0x1010,
        /** There was a timeout that occurred */
        AHC_H264D_ERR_TIME_OUT = AHC_H264D_ERR_BASE | 0x1011,
        /** Attempting a command that is not allowed during the present state. */
        AHC_H264D_ERR_INCORRECT_STATE_OPERATION = AHC_H264D_ERR_BASE | 0x1018,
        /** The values encapsulated in the parameter or config structure are not supported. */
        AHC_H264D_ERR_UNSUPPORTED_SETTING = AHC_H264D_ERR_BASE | 0x1019,
    AHC_H264D_ERR_MEM_UNAVAILABLE,//AHC_H264D_ERR_INSUFFICIENT_RESOURCES
    AHC_H264D_ERR_INIT_VIDEO_FAIL,//AHC_H264D_ERR_INVALID_STATE
    AHC_H264D_ERR_FRAME_NOT_READY,//AHC_H264D_ERR_NOT_READY

    // AHC_USB
    AHC_USB_ERR_PARAMETER = (AHC_USB << MODULE_ERR_SHIFT) | 0x000001,
    AHC_USB_ERR_PCSYNC_BUSY,
    AHC_USB_ERR_MEMDEV_ACK_TIMEOUT,
    AHC_USB_ERR_MEMDEV_NACK,
    AHC_USB_ERR_UNSUPPORT_MODE,

    // AHC_FS
    AHC_FS_ERR_PARAMETER = (AHC_FS << MODULE_ERR_SHIFT) | 0x000001,
    AHC_FS_ERR_TARGET_NOT_FOUND,
    AHC_FS_ERR_OPEN_FAIL,
    AHC_FS_ERR_CLOSE_FAIL,
    AHC_FS_ERR_READ_FAIL,
    AHC_FS_ERR_WRITE_FAIL,
    AHC_FS_ERR_FILE_SEEK_FAIL,
    AHC_FS_ERR_FILE_POS_ERROR,
    AHC_FS_ERR_FILE_COPY_FAIL,
    AHC_FS_ERR_FILE_ATTR_FAIL,
    AHC_FS_ERR_FILE_TIME_FAIL,
    AHC_FS_ERR_FILE_NAME_FAIL,
    AHC_FS_ERR_FILE_MOVE_FAIL,
    AHC_FS_ERR_FILE_REMOVE_FAIL,
    AHC_FS_ERR_FILE_REMNAME_FAIL,
    AHC_FS_ERR_INVALID_SIZE,
    AHC_FS_ERR_FILE_TRUNCATE_FAIL,
    AHC_FS_ERR_EXCEED_MAX_OPENED_NUM,
    AHC_FS_ERR_NO_MORE_ENTRY,
    AHC_FS_ERR_CREATE_DIR_FAIL,
    AHC_FS_ERR_DELETE_FAIL,
    AHC_FS_ERR_FILE_INIT_FAIL,
    AHC_FS_ERR_PATH_NOT_FOUND,
    AHC_FS_ERR_RESET_STORAGE,
    AHC_FS_ERR_EOF,
    AHC_FS_ERR_FILE_EXIST,
    AHC_FS_ERR_DIR_EXIST,
    AHC_FS_ERR_SEMAPHORE_FAIL,
    AHC_FS_ERR_NOT_SUPPORT,
	AHC_FS_ERR_GET_FREE_SPACE_FAIL,
	AHC_FS_ERR_COPY_FINISH,
	AHC_FS_ERR_COPY_NOT_FINISH,
	AHC_FS_ERR_FILE_HANDLE_INVALID,

    // AHC_3GPMGR
    AHC_3GPMGR_ERR_PARAMETER = (AHC_3GPMGR << MODULE_ERR_SHIFT) | 0x000001,
	AHC_3GPMGR_ERR_HOST_CANCEL_SAVE,
    AHC_3GPMGR_ERR_MEDIA_FILE_FULL,
    AHC_3GPMGR_ERR_AVBUF_FULL,
    AHC_3GPMGR_ERR_AVBUF_EMPTY,
    AHC_3GPMGR_ERR_AVBUF_OVERFLOW,
    AHC_3GPMGR_ERR_AVBUF_FAILURE,
    AHC_3GPMGR_ERR_FTABLE_FULL,
    AHC_3GPMGR_ERR_FTABLE_EMPTY,
    AHC_3GPMGR_ERR_FTABLE_OVERFLOW,
    AHC_3GPMGR_ERR_FTABLE_FAILURE,
    AHC_3GPMGR_ERR_TTABLE_FULL,
    AHC_3GPMGR_ERR_TTABLE_EMPTY,
    AHC_3GPMGR_ERR_TTABLE_OVERFLOW,
    AHC_3GPMGR_ERR_TTABLE_FAILURE,
    AHC_3GPMGR_ERR_ITABLE_FULL,
    AHC_3GPMGR_ERR_QUEUE_UNDERFLOW,
    AHC_3GPMGR_ERR_QUEUE_OVERFLOW,
    AHC_3GPMGR_ERR_EVENT_UNSUPPORT,
    AHC_3GPMGR_ERR_FILL_TAIL,
    AHC_3GPMGR_ERR_POST_PROCESS,

	// AHC_3GPPSR
    AHC_VIDPSR_ERR_BASE = (AHC_3GPPSR << MODULE_ERR_SHIFT), // 0x17
        //----- The followings ( < 0x1000) are not error -----
        /** End of stream */
        AHC_VIDPSR_ERR_EOS = AHC_VIDPSR_ERR_BASE | 0x0001,
        /** The max correct number of the error code*/
        AHC_VIDPSR_ERR_MIN = AHC_VIDPSR_ERR_BASE | 0x0FFF,
	    /** There were insufficient resources to perform the requested operation
	    E.g. The bitstream buffer is overflow. Since the video bitstream buffer is a
	    plain buffer, the buffer is not able to be full loaded when one video
	    frame is greater than the buffer.  For the size of the video bitstream
	    buffer, refer AIT for more detail.*/
        AHC_VIDPSR_ERR_INSUFFICIENT_RESOURCES = AHC_VIDPSR_ERR_BASE | 0x1000,
        /** There was an error, but the cause of the error could not be determined */
	    AHC_VIDPSR_ERR_UNDEFINED = AHC_VIDPSR_ERR_BASE | 0x1001,
	    /** One or more parameters were not valid.
	    The input parameters are supported but are not valid value. E.g. it's out of range.*/
        AHC_VIDPSR_ERR_PARAMETER = AHC_VIDPSR_ERR_BASE | 0x1005,
	    /** This functions has not been implemented yet.*/
	    AHC_VIDPSR_ERR_NOT_IMPLEMENTED = AHC_VIDPSR_ERR_BASE | 0x1006,
        /** The buffer was emptied before the next buffer was ready */
        AHC_VIDPSR_ERR_UNDERFLOW = AHC_VIDPSR_ERR_BASE | 0x1007,
        /** The buffer was not available when it was needed */
        AHC_VIDPSR_ERR_OVERFLOW = AHC_VIDPSR_ERR_BASE | 0x1008,
        /** The component is in the state AHC_STATE_INVALID */
        AHC_VIDPSR_ERR_INVALID_STATE = AHC_VIDPSR_ERR_BASE | 0x100A,
        /** There was a timeout that occurred */
        AHC_VIDPSR_ERR_TIME_OUT = AHC_VIDPSR_ERR_BASE | 0x1011,
        /** Resources allocated to an executing or paused component have been
          preempted, causing the component to return to the idle state */
        AHC_VIDPSR_ERR_RESOURCES_PREEMPTED = AHC_VIDPSR_ERR_BASE | 0x1013,
        /** Attempting a state transtion that is not allowed.
        The video player encounter an invalid state transition.
	    Such as try to PLAY while it is playing.*/
        AHC_VIDPSR_ERR_INCORRECT_STATE_TRANSITION = AHC_VIDPSR_ERR_BASE | 0x1017,
        /** Attempting a command that is not allowed during the present state. */
        AHC_VIDPSR_ERR_INCORRECT_STATE_OPERATION = AHC_VIDPSR_ERR_BASE | 0x1018,
        /** The values encapsulated in the parameter or config structure are not supported. */
        AHC_VIDPSR_ERR_UNSUPPORTED_SETTING = AHC_VIDPSR_ERR_BASE | 0x1019,
        /** A component reports this error when it cannot parse or determine the format of an input stream. */
        AHC_VIDPSR_ERR_FORMAT_NOT_DETECTED = AHC_VIDPSR_ERR_BASE | 0x1020,
        /** The content open operation failed. */
        AHC_VIDPSR_ERR_CONTENT_PIPE_OPEN_FAILED = AHC_VIDPSR_ERR_BASE | 0x1021,
        AHC_VIDPSR_ERR_CONTENT_CORRUPT          = AHC_VIDPSR_ERR_BASE | 0x1022,
        AHC_VIDPSR_ERR_CONTENT_UNKNOWN_DATA     = AHC_VIDPSR_ERR_BASE | 0x1023,
        AHC_VIDPSE_ERR_CONTENT_CANNOTSEEK       = AHC_VIDPSR_ERR_BASE | 0x1024,

	// AHC_AUDIO
    AHC_AUDIO_ERR_PARAMETER = (AHC_AUDIO << MODULE_ERR_SHIFT) | 0x000001,
    AHC_AUDIO_ERR_END_OF_FILE,
    AHC_AUDIO_ERR_STREAM_UNDERFLOW,
    AHC_AUDIO_ERR_STREAM_BUF_FULL,
    AHC_AUDIO_ERR_STREAM_BUF_EMPTY,
    AHC_AUDIO_ERR_STREAM_OVERFLOW,
    AHC_AUDIO_ERR_STREAM_POINTER,
    AHC_AUDIO_ERR_COMMAND_INVALID,
    AHC_AUDIO_ERR_OPENFILE_FAIL,
    AHC_AUDIO_ERR_FILE_CLOSED,
    AHC_AUDIO_ERR_FILE_IO_FAIL,
    AHC_AUDIO_ERR_INSUFFICIENT_BUF,
    AHC_AUDIO_ERR_UNSUPPORT_FORMAT,
    AHC_AUDIO_ERR_NO_AUDIO_FOUND,
    AHC_AUDIO_ERR_INVALID_EQ,
    AHC_AUDIO_ERR_INVALID_FLOW,
    AHC_AUDIO_ERR_DATABASE_SORT,
    AHC_AUDIO_ERR_DATABASE_FLOW,
    AHC_AUDIO_ERR_DATABASE_MEMORY_FULL,
    AHC_AUDIO_ERR_DATABASE_NOT_FOUND,
    AHC_AUDIO_ERR_DATABASE_NOT_SUPPORT,
    AHC_AUDIO_ERR_NO_MIXER_DATA,
    AHC_AUDIO_ERR_BUF_ALLOCATION,
    AHC_AUDIO_ERR_DECODER_INIT,
    AHC_AUDIO_ERR_SEEK,
    AHC_AUDIO_ERR_DECODE,

	// AHC_DSC
    AHC_DSC_ERR_PARAMETER = (AHC_DSC << MODULE_ERR_SHIFT) | 0x000001,
    AHC_DSC_ERR_SAVEOF_FAIL,
    AHC_DSC_ERR_JPEG_NODATASAVE,
    AHC_DSC_ERR_FILE_END,
    AHC_DSC_ERR_FILE_ERROR,

    AHC_DSC_ERR_DECODE_FAIL,
    AHC_DSC_ERR_FLOW_FAIL,
    AHC_DSC_ERR_HW,
    AHC_DSC_ERR_JPEGINFO_FAIL,
    AHC_DSC_ERR_CAPTURE_FAIL,
    AHC_DSC_ERR_CAPTURE_BUFFER_OVERFLOW,

    AHC_DSC_ERR_FIFOOF_FAIL,
    AHC_DSC_ERR_OUTOFRANGE,
    AHC_DSC_ERR_INITIALIZE_FAIL,
    AHC_DSC_ERR_PREVIEW_FAIL,
    AHC_DSC_ERR_PLAYBACK_FAIL,

    AHC_DSC_ERR_ICON_FAIL,
    AHC_DSC_ERR_TIMEOUT_FAIL,
	AHC_DSC_ERR_EXIF_ENC,
	AHC_DSC_ERR_EXIF_DEC,
	AHC_DSC_ERR_EXIF_NOT_SUPPORT,

	// AHC_FCTL
    AHC_FCTL_ERR_PARAMETER = (AHC_FCTL << MODULE_ERR_SHIFT) | 0x000001,
    AHC_FCTL_ERR_HW,

	// AHC_VCONF
    AHC_VCONF_ERR_PARAMETER = (AHC_VCONF << MODULE_ERR_SHIFT) | 0x000001,

	// AHC_3GPPLAY
    AHC_3GPPLAY_ERR_BASE = (AHC_3GPPLAY << MODULE_ERR_SHIFT), // 0x1C
	    /** There were insufficient resources to perform the requested operation
	    E.g. The bitstream buffer is overflow. Since the video bitstream buffer is a
	    plain buffer, the buffer is not able to be full loaded when one video
	    frame is greater than the buffer.  For the size of the video bitstream
	    buffer, refer AIT for more detail.*/
        AHC_3GPPLAY_ERR_INSUFFICIENT_RESOURCES = AHC_3GPPLAY_ERR_BASE | 0x1000,
        /** There was an error, but the cause of the error could not be determined */
	    AHC_3GPPLAY_ERR_UNDEFINED = AHC_3GPPLAY_ERR_BASE | 0x1001,
	    /** One or more parameters were not valid.
	    The input parameters are supported but are not valid value. E.g. it's out of range.*/
        AHC_3GPPLAY_ERR_PARAMETER = AHC_3GPPLAY_ERR_BASE | 0x1005,
	    /** This functions has not been implemented yet.*/
	    AHC_3GPPLAY_ERR_NOT_IMPLEMENTED = AHC_3GPPLAY_ERR_BASE | 0x1006,
        /** The buffer was emptied before the next buffer was ready */
        AHC_3GPPLAY_ERR_UNDERFLOW = AHC_3GPPLAY_ERR_BASE | 0x1007,
        /** The buffer was not available when it was needed */
        AHC_3GPPLAY_ERR_OVERFLOW = AHC_3GPPLAY_ERR_BASE | 0x1008,
        /** The hardware failed to respond as expected */
        AHC_3GPPLAY_ERR_HW = AHC_3GPPLAY_ERR_BASE | 0x1009,
        /** The component is in the state AHC_STATE_INVALID */
        AHC_3GPPLAY_ERR_INVALID_STATE = AHC_3GPPLAY_ERR_BASE | 0x100A,
        /** The component is not ready to return data at this time */
        AHC_3GPPLAY_ERR_NOT_READY = AHC_3GPPLAY_ERR_BASE | 0x1010,
        /** There was a timeout that occurred */
        AHC_3GPPLAY_ERR_TIME_OUT = AHC_3GPPLAY_ERR_BASE | 0x1011,
        /** Attempting a state transtion that is not allowed.
        The video player encounter an invalid state transition.
	    Such as try to PLAY while it is playing.*/
        AHC_3GPPLAY_ERR_INCORRECT_STATE_TRANSITION = AHC_3GPPLAY_ERR_BASE | 0x1017,
        /** Attempting a command that is not allowed during the present state. */
        AHC_3GPPLAY_ERR_INCORRECT_STATE_OPERATION = AHC_3GPPLAY_ERR_BASE | 0x1018,
        /** The values encapsulated in the parameter or config structure are not supported. */
        AHC_3GPPLAY_ERR_UNSUPPORTED_SETTING = AHC_3GPPLAY_ERR_BASE | 0x1019,

	// AHC_3GPRECD
    AHC_3GPRECD_ERR_PARAMETER = (AHC_3GPRECD << MODULE_ERR_SHIFT) | 0x000001,
	AHC_3GPRECD_ERR_UNSUPPORTED_PARAMETERS, 	///< The input parameters are not supported.
	AHC_3GPRECD_ERR_INVALID_PARAMETERS,     	///< The input parameters are supported but are not valid value. E.g. it's out of range.
	AHC_3GPRECD_ERR_GENERAL_ERROR,          	///< General Error without specific define.
	AHC_3GPRECD_ERR_NOT_ENOUGH_SPACE,		 	///< Not enough space for minimum recording.
	AHC_3GPRECD_ERR_OPEN_FILE_FAILURE,		 	///< Open file failed.
	AHC_3GPRECD_ERR_CLOSE_FILE_FAILURE,	 		///< Close file failed.
	AHC_3GPRECD_ERR_BUFFER_OVERFLOW,			///< Buffer overflow
	AHC_3GPRECD_ERR_WRITE_FAILURE,				///< Host FWrite failure

    // AHC_MDTV
    AHC_MDTV_ERR_FAIL = (AHC_MDTV << MODULE_ERR_SHIFT) | 0x000001,
    AHC_MDTV_ERR_MEM_UNAVAILABLE,
    AHC_MDTV_ERR_DATA_UNAVAILABLE,
    AHC_MDTV_ERR_INIT_VIDEO_FAIL,
    AHC_MDTV_ERR_PLAY_SERVICE_FAIL,
    AHC_MDTV_ERR_STOP_SERVICE_FAIL,
    AHC_MDTV_ERR_SET_FREQUENCY_FAIL,
    AHC_MDTV_ERR_INIT_ESG_FAIL,
    AHC_MDTV_ERR_GET_SERVICE_NUM_FAIL,
    AHC_MDTV_ERR_GET_SERVICE_INFO_FAIL,
    AHC_MDTV_ERR_GET_PROGRAM_NUM_FAIL,
    AHC_MDTV_ERR_GET_PROGRAM_INFO_FAIL,
    AHC_MDTV_ERR_START_UPDATE_ESG_FAIL,
    AHC_MDTV_ERR_STOP_UPDATE_ESG_FAIL,
    AHC_MDTV_ERR_FLUSH_ESG_FAIL,
    AHC_MDTV_ERR_NO_SUPPORT_CA_SERVICE,

	// AHC_UART
    AHC_UART_ERR_PARAMETER = (AHC_UART << MODULE_ERR_SHIFT) | 0x000001,
    AHC_UART_SYSTEM_ERR,

    // AHC_SPI
    AHC_SPI_ERR_PARAMETER = (AHC_SPI << MODULE_ERR_SHIFT) | 0x000001,
    AHC_SPI_ERR_INIT,
    AHC_SPI_ERR_CMDTIMEOUT,
    AHC_SPI_ERR_TX_UNDERFLOW,
    AHC_SPI_ERR_RX_OVERFLOW,

	// AHC_PLL
	AHC_PLL_ERR_PARAMETER = (AHC_PLL << MODULE_ERR_SHIFT) | 0x000001,

	// AHC_USER
	AHC_USER_ERR_PARAMETER = (AHC_USER << MODULE_ERR_SHIFT) | 0x000001,
	AHC_USER_ERR_UNSUPPORTED,
	AHC_USER_ERR_INSUFFICIENT_BUF,
	AHC_USER_ERR_INIT,
	AHC_USER_ERR_UNINIT,

    // AHC_3GPRECD
    AHC_VT_ERR_PARAMETER = (AHC_VT << MODULE_ERR_SHIFT) | 0x000001,
	AHC_VT_ERR_UNSUPPORTED_PARAMETERS, 	        ///< The input parameters are not supported.
	AHC_VT_ERR_INVALID_PARAMETERS,     	        ///< The input parameters are supported but are not valid value. E.g. it's out of range.
	AHC_VT_ERR_GENERAL_ERROR,          	        ///< General Error without specific define.
	AHC_VT_ERR_INSUFFICIENT_BUF,                ///< No enough buffer for usage.
	AHC_VT_ERR_NOT_IMPLEMENTED,                 ///< Not implement functions.
    AHC_VT_ERR_TIME_OUT,                        ///< time out error

    // AHC_CCIR
    AHC_CCIR_ERR_PARAMETER = (AHC_CCIR << MODULE_ERR_SHIFT) | 0x000001,
    AHC_CCIR_ERR_UNSUPPORTED_PARAMETERS,
    AHC_CCIR_ERR_NOT_INIT,

    // AHC_STORAGE
    AHC_STORAGE_ERR_PARAMETER = (AHC_STORAGE << MODULE_ERR_SHIFT) | 0x000001,
    AHC_STORAGE_ERR_NOT_FOUND,
    AHC_STORAGE_ERR_PARTITION_INFO,
    AHC_STORAGE_ERR_IO_ACCESS,

     // AHC_PIO
    AHC_PIO_ERR_PARAMETER = (AHC_PIO << MODULE_ERR_SHIFT) | 0x000001,
    AHC_PIO_ERR_INPUTMODESETDATA,
    AHC_PIO_ERR_OUTPUTMODEGETDATA,

    //AHC_MFD
    AHC_MFD_ERR_PARAMETER = (AHC_MFD << MODULE_ERR_SHIFT) | 0x000001,
    AHC_MFD_ERR_INIT_FAILED,

	//AHC_PWM
	AHC_PWM_ERR_PARAMETER = (AHC_PWM << MODULE_ERR_SHIFT) | 0x000001,

	// AHC_RAWPROC
    AHC_RAWPROC_ERR_PARAMETER = (AHC_RAWPROC << MODULE_ERR_SHIFT) | 0x000001,
    AHC_RAWPROC_ERR_UNSUPPORTED_PARAMETERS,

    //AHC_SIF
	AHC_SIF_ERR_PARAMETER = (AHC_SIF << MODULE_ERR_SHIFT) | 0x000001,
	
	// AHC_EVENT
    AHC_EVENT_ERR_PARAMETER = (AHC_EVENT << MODULE_ERR_SHIFT) | 0x000001,
    AHC_EVENT_ERR_MSGQ_FULL,
    AHC_EVENT_ERR_POST,

    // AHC_BAYER
	AHC_BAYER_ERR_PARAMETER = (AHC_BAYER << MODULE_ERR_SHIFT) | 0x000001,

    // AHC_LDC
	AHC_LDC_ERR_PARAMETER = (AHC_LDC << MODULE_ERR_SHIFT) | 0x000001,

    // AHC_RTC
    AHC_RTC_ERR_ISR = (AHC_RTC << MODULE_ERR_SHIFT) | 0x000001,
    AHC_RTC_ERR_FMT,
    AHC_RTC_ERR_HW,
    AHC_RTC_ERR_SEM,
    AHC_RTC_ERR_PARAM,
    AHC_RTC_ERR_INVALID
} AHC_ERR;

#define AIT_INT_USE 		(0)//For AIT internal use

#define QUICK_PCCAM         (QUICK_REC)

typedef enum{
    COMMON_MOVIE_CLIP_TIME_OFF  = 0,
    COMMON_MOVIE_CLIP_TIME_6SEC,
    COMMON_MOVIE_CLIP_TIME_1MIN,
    COMMON_MOVIE_CLIP_TIME_2MIN,
    COMMON_MOVIE_CLIP_TIME_3MIN,
    COMMON_MOVIE_CLIP_TIME_5MIN,
    COMMON_MOVIE_CLIP_TIME_10MIN,
    COMMON_MOVIE_CLIP_TIME_25MIN,
    COMMON_MOVIE_CLIP_TIME_30MIN,
    COMMON_MOVIE_CLIP_TIME_NUM,
    COMMON_MOVIE_CLIP_TIME_DEFAULT = 0
}COMMON_MOVIE_CLIPTIME;

typedef enum{
    COMMON_VR_TIMELAPSE_OFF  = 0,
    COMMON_VR_TIMELAPSE_1SEC,
    COMMON_VR_TIMELAPSE_5SEC,
    COMMON_VR_TIMELAPSE_10SEC,
    COMMON_VR_TIMELAPSE_30SEC,
    COMMON_VR_TIMELAPSE_60SEC,
    COMMON_VR_TIMELAPSE_NUM,
    COMMON_VR_TIMELAPSE_DEFAULT = 0
}COMMON_VR_TIMELAPSE;

typedef enum{
    COMMON_TV_SYSTEM_NTSC = 0,
    COMMON_TV_SYSTEM_PAL,
    COMMON_TV_SYSTEM_NUM,
    COMMON_TV_SYSTEM_DEFAULT = 0
}COMMON_TV_SYSTEM_NTSC_SETTING;

typedef enum{
    COMMON_GPS_REC_INFO_LOG_VIDEO = 0,
    COMMON_GPS_REC_INFO_VIDEO_ONLY,
    COMMON_GPS_REC_INFO_LOG_ONLY,
    COMMON_GPS_REC_INFO_OFF
} COMMON_GPS_REC_INFO;

typedef enum{
    COMMON_SCENE_AUTO = 0,
    COMMON_SCENE_SPORT,
    COMMON_SCENE_PORTRAIT,
    COMMON_SCENE_LANDSCAPE,
    COMMON_SCENE_TWILIGHT_PORTRAIT,
    COMMON_SCENE_TWILIGHT,
    COMMON_SCENE_SNOW,
    COMMON_SCENE_BEACH,
    COMMON_SCENE_FIREWORKS,
    COMMON_SCENE_NUM,
    COMMON_SCENE_DEFAULT = 0
 } COMMON_SCENE_SETTING;

typedef enum {
    COMMON_METERING_CENTER = 0,
    COMMON_METERING_CENTER_SPOT,
    COMMON_METERING_NUM,
    COMMON_METERING_AVERAGE,
    COMMON_METERING_MATRIX,
    COMMON_METERING_WEIGHTED,
    COMMON_METERING_MULTI,
    COMMON_METERING_DEFAULT = 0
} COMMON_METERING_SETTING;

typedef enum {
    COMMON_EVVALUE_N20 ,
    COMMON_EVVALUE_N17 ,
    COMMON_EVVALUE_N13 ,
    COMMON_EVVALUE_N10 ,
    COMMON_EVVALUE_N07 ,
    COMMON_EVVALUE_N03 ,
    COMMON_EVVALUE_00  ,
    COMMON_EVVALUE_P03 ,
    COMMON_EVVALUE_P07 ,
    COMMON_EVVALUE_P10 ,
    COMMON_EVVALUE_P13 ,
    COMMON_EVVALUE_P17 ,
    COMMON_EVVALUE_P20 ,
    COMMON_EVVALUE_NUM ,
    COMMON_EVVALUE_DEFAULT = COMMON_EVVALUE_00
}COMMON_EVVALUE_SETTING;

typedef enum {
    COMMON_ISO_AUTO = 0,
    COMMON_ISO_100,
    COMMON_ISO_200,
    COMMON_ISO_400,
    COMMON_ISO_800,
    COMMON_ISO_1200,
    COMMON_ISO_1600,
    COMMON_ISO_3200,
    COMMON_ISO_NUM,
    COMMON_ISO_DEFAULT = 0
} COMMON_ISOMode_SETTING;

typedef enum {
    COMMON_WB_AUTO,
    COMMON_WB_DAYLIGHT,
    COMMON_WB_CLOUDY,
    COMMON_WB_FLUORESCENT1,
    COMMON_WB_FLUORESCENT2,
    COMMON_WB_FLUORESCENT3,
    COMMON_WB_INCANDESCENT,
    COMMON_WB_FLASH,
    COMMON_WB_ONEPUSH,
    COMMON_WB_ONE_PUSH_SET,
    COMMON_WB_NUM,
    COMMON_WB_DEFAULT = 0
}COMMON_WB_SETTING;

typedef enum {
    COMMON_LDWS_EN_ON = 0,
    COMMON_LDWS_EN_OFF,
    COMMON_LDWS_EN_NUM,
    COMMON_LDWS_EN_DEFAULT = COMMON_LDWS_EN_OFF
}COMMON_LDWS_EN;

typedef enum {
    COMMON_SLOWMOTION_EN_X1 = 0,
    COMMON_SLOWMOTION_EN_X3,
    COMMON_SLOWMOTION_EN_X5,
    COMMON_SLOWMOTION_EN_X7,
    COMMON_SLOWMOTION_EN_DEFAULT = COMMON_SLOWMOTION_EN_X1
}COMMON_SLOWMOTION_EN;

typedef enum {
    COMMON_FCWS_EN_ON = 0,
    COMMON_FCWS_EN_OFF,
    COMMON_FCWS_EN_NUM,
    COMMON_FCWS_EN_DEFAULT = COMMON_FCWS_EN_OFF
}COMMON_FCWS_EN;

typedef enum {
    COMMON_SAG_EN_ON = 0,
    COMMON_SAG_EN_OFF,
    COMMON_SAG_EN_NUM,
    COMMON_SAG_EN_DEFAULT = COMMON_SAG_EN_OFF
}COMMON_SAG_EN;
    
typedef enum {
    COMMON_HDR_EN_ON = 0,
    COMMON_HDR_EN_OFF,
    COMMON_HDR_EN_NUM,
    COMMON_HDR_EN_DEFAULT = COMMON_HDR_EN_OFF
}COMMON_HDR_EN;

typedef enum {
    COMMON_WDR_EN_ON = 0,
    COMMON_WDR_EN_OFF,
    COMMON_WDR_EN_NUM,
    COMMON_WDR_EN_DEFAULT = COMMON_WDR_EN_OFF
}COMMON_WDR_EN;

typedef enum {
    COMMON_FLICKER_50HZ = 0,
    COMMON_FLICKER_60HZ,
    COMMON_FLICKER_NUM,
    COMMON_FLICKER_DEFAULT = COMMON_FLICKER_50HZ
}COMMON_FLICKER_MODE;

typedef enum {
    COMMON_WNR_EN_OFF = 0,
    COMMON_WNR_EN_ON,
    COMMON_WNR_EN_NUM,
    COMMON_WNR_EN_DEFAULT = COMMON_WNR_EN_OFF
}COMMON_WNR_EN;

#endif // _AHC_COMMON_H_

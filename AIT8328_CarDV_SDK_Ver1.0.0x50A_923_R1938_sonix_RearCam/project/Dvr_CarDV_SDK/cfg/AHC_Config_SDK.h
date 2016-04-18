/**
 @file AHC_Config_SDK.h
 @brief Header File for the AHC configuration.
 @author 
 @version 1.0
*/

#ifndef _AHC_CONFIG_SDK_H_
#define _AHC_CONFIG_SDK_H_

/*===========================================================================
 * Include files
 *===========================================================================*/
#include "config_fw.h"
#include "Customer_config.h"
#include "AHC_OS.h"

/*===========================================================================
 * Marco define
 *===========================================================================*/
//UI DRAM address++
#ifdef MCR_V2_32MB      // defined in MCP
    #undef AHC_DRAM_SIZE
    #define AHC_DRAM_SIZE                           (COMMON_DRAM_SIZE_32MB)
#else
    #ifndef AHC_DRAM_SIZE
    #define AHC_DRAM_SIZE                           (COMMON_DRAM_SIZE_64MB) //Default DRAM setting
    #endif
#endif

#if (AHC_DRAM_SIZE == COMMON_DRAM_SIZE_64MB)

    //64MB version
    #define AHC_THUMB_TEMP_BASE_ADDR                (0x2700000)
    #define AHC_THUMB_BUF_NUM                       (2) //1 or 2
    #define AHC_DCF_TEMP_BASE_ADDR                  (0x4E00000)

    // VIDEO_DIGIT_ZOOM_EN and HDR, change it to 0x04600000
    #if ((EMERGENTRECD_SUPPORT == 1) || VIDEO_DIGIT_ZOOM_EN)
    #define AHC_GUI_TEMP_BASE_ADDR                  (0x4B00000)
    #else
    #define AHC_GUI_TEMP_BASE_ADDR                  (0x4B00000) // HDR - 0x4600000, else 0x4000000
    #endif

    #define AHC_GUI_TEMP_BASE_ADDR_HDMI             (0x4000000) // Andy Liu.  It causes system lock up if it is in 0x3C00000.
    #define AHC_GUI_TEMP_BASE_ADDR_HDMI_PHOTO_PB    (0x4000000)
    #define AHC_GUI_STRUCT_MEMORY_SIZE              (0x80000)
    #define AHC_SOUNDFILE_DRAM_ADDR                 (0x0000000) //0: SOUNDFILE doesn't at DRAM

    #define AHC_MENU_SETTING_TEMP_BUFFER_SIZE       (8192)      // front of MMPS_System_GetPreviewFrameStart

#elif (AHC_DRAM_SIZE == COMMON_DRAM_SIZE_32MB)

    //32MB version
    #define AHC_THUMB_TEMP_BASE_ADDR                (0x2370000)
    #define AHC_THUMB_BUF_NUM                       (1) //1 or 2 
    #define AHC_DCF_TEMP_BASE_ADDR                  (0x2E30000)
    #define AHC_GUI_TEMP_BASE_ADDR                  (0x2B60000)       // for 320x240 Panel
    //#define AHC_GUI_TEMP_BASE_ADDR                  (0x2900000)     // for 480x272 Panel
    #define AHC_GUI_TEMP_BASE_ADDR_HDMI             (0x2800000)
    #define AHC_GUI_TEMP_BASE_ADDR_HDMI_PHOTO_PB    (0x2800000)
    #define AHC_GUI_STRUCT_MEMORY_SIZE              (0x80000)
    #define AHC_SOUNDFILE_DRAM_ADDR                 (0x0000000) //0: SOUNDFILE doesn't at DRAM 

    #define AHC_MENU_SETTING_TEMP_BUFFER_SIZE       (8192)      // front of MMPS_System_GetPreviewFrameStart

#else

#error "AHC_DRAM_SIZE NG!!!"

#endif
//UI DRAM address--
//Dynamic Download OSD
#define DDOSD                           (0)

// Use ARGB format ( So far alpha is skipped )
#define DATESTAMP_COLOR_BLACK           (0x00000000)
#define DATESTAMP_COLOR_YELLOW          (0xFFFFFF00)
#define DATESTAMP_COLOR_ORANGE          (0xFFFF8000)
#define DATESTAMP_COLOR_WHITE           (0xFFFFFFFF)

#define PROTECT_FILE_NONE               (0)
#define PROTECT_FILE_RENAME				(1)
#define PROTECT_FILE_MOVE_SUBDB         (2)
#define PROTECT_FILE_MOVE_DB            (3)

#define DELETION_STYLE_BASIC            (0)
#define DELETION_STYLE_C003             (1)
#define DELETION_CARDV                  (2)
#define DELETION_STYLE_C004             (3)
#define DELETION_STYLE_C005             (4)

#define AHC_BROWSER_THUMBNAIL_STYLE_0   (0)
#define AHC_BROWSER_THUMBNAIL_STYLE_1   (1)

#define AHC_BROWSER_PROTECT_KEY_STYLE_0 (0)
#define AHC_BROWSER_PROTECT_KEY_STYLE_1 (1)

#define UI_STYLE_DEAULT     			(0)
#define UI_STYLE_C005_BUTTON      		(1)
#define UI_STYLE_C005_TOUCH     		(2)

#define AHC_VIDEO_BITRATE_1440_30FPS_SUPPERFINE (VR_BITRATE_1440_30P_SUPER) //16000000
#define AHC_VIDEO_BITRATE_1440_30FPS_FINE       (VR_BITRATE_1440_30P_FINE)  // 8000000
#define AHC_VIDEO_BITRATE_SHD30FPS_SUPPERFINE  	(VR_BITRATE_SHD30P_SUPER)   //16000000
#define AHC_VIDEO_BITRATE_SHD30FPS_FINE        	(VR_BITRATE_SHD30P_FINE)    // 8000000
#define AHC_VIDEO_BITRATE_FHD60FPS_SUPPERFINE  	(VR_BITRATE_FHD60P_SUPER)   //32000000
#define AHC_VIDEO_BITRATE_FHD60FPS_FINE        	(VR_BITRATE_FHD60P_FINE)    //16000000
#define AHC_VIDEO_BITRATE_FHD30FPS_SUPPERFINE  	(VR_BITRATE_FHD30P_SUPER)   //16000000
#define AHC_VIDEO_BITRATE_FHD30FPS_FINE        	(VR_BITRATE_FHD30P_FINE)    // 8000000
#define AHC_VIDEO_BITRATE_HD30FPS_SUPPERFINE    (VR_BITRATE_HD30P_SUPER)    // 7200000
#define AHC_VIDEO_BITRATE_HD30FPS_FINE          (VR_BITRATE_HD30P_FINE)     // 4000000
#define AHC_VIDEO_BITRATE_HD60FPS_SUPPERFINE    (VR_BITRATE_HD60P_SUPER)    //12000000
#define AHC_VIDEO_BITRATE_HD60FPS_FINE          (VR_BITRATE_HD60P_FINE)     // 7200000
#define AHC_VIDEO_BITRATE_VGA30FPS_SUPPERFINE   (VR_BITRATE_VGA30P_SUPER)   //12000000
#define AHC_VIDEO_BITRATE_VGA30FPS_FINE         (VR_BITRATE_VGA30P_FINE)    //10000000

#define AHC_VIDEO_MAXBITRATE_PRERECORD			(8000000)

//General
#define AHC_GENERAL_DBG_LEVEL	    	0x0
#define AHC_MESSAGE_MAX			    	16
#define AHC_I2C_HANDLE_MAX		    	5
#define AHC_MSG_QUEUE_SIZE		    	0x80
#define AHC_MSG_QUEUE_VIP_SIZE		    0x10
#define AHC_HP_MSG_QUEUE_SIZE		    0x08
#define MAX_FILE_NAME_SIZE          	256
#define SHOW_ALL_MENU_FUNCTIONS     	0
#define AHC_DATA_DIR					"DATA"
#define AHC_MESSAGE_QUEUE_OVF			(0)
#define AHC_AHL_MSGQ_TIMEOUT            (MS_TO_TICK(100))

#define USE_INFO_FILE           	    (0)

#define AHC_INFO_LOG					"SD:\\DATA\\Info.txt"

//DCF
#define MAX_DCF_SUPPORT_FILE_TYPE   	15
/**
@brief	the total memory space is about 512 kB
*/
#define MAX_DCF_MEM             	    ( 512 * 1024 )
#define AHC_DCF_BACKGROUND				(0)
#define DCF_DB_COUNT                    (4)
#define DCF_SUB_DB                      (0)
#define DCF_DCFX                        (1)

/**
@brief	DCF file name
*/
#define DCF_FILE_NAME_TYPE_NORMAL       (0)  	//Use AHC_DCF   (FILE0001.MOV)
#define DCF_FILE_NAME_TYPE_DATE_TIME    (1)   	//Use AHC_DCFDT (FILE131005-092011.MOV)

#if (VIDRECD_MULTI_TRACK==0)
//only for TYPE_DATE_TIME, TYPE_NORMAL not finished yet
#define DCF_FILE_NAME_TYPE              (DCF_FILE_NAME_TYPE_DATE_TIME)
#define DCF_DUAL_CAM_ENABLE             (1)
#else
#define DCF_DUAL_CAM_ENABLE             (0)
#define DCF_FILE_NAME_TYPE              (DCF_FILE_NAME_TYPE_NORMAL)
#endif

#define ALL_TYPE_FILE_BROWSER_PLAYBACK 	((DCF_DB_COUNT == 1) & 0)

#define DSC_DEFAULT_FLIE_FREECHAR		"IMAG"
#define SNAP_DEFAULT_FILE_FREECHAR		"SNAP"
#define VIDEO_DEFAULT_FLIE_FREECHAR		"MOV_"
/*
DB0
*/
#define DCF_ROOT_DIR_NAME				"DCIM"
#define DCF_DEFAULT_DIR_FREECHAR    	"_NOML"
#define DCF_DEFAULT_FILE_FREECHAR   	(VIDEO_DEFAULT_FLIE_FREECHAR)
/*
DB1
*/
#define DCF_ROOT_DIR_NAME_2				"PARK"
#define DCF_DEFAULT_DIR_FREECHAR_2    	"_PARK"
#define DCF_DEFAULT_FILE_FREECHAR_2   	"PARK"
/*
DB2
*/
#define DCF_ROOT_DIR_NAME_3				"EMER"
#define DCF_DEFAULT_DIR_FREECHAR_3    	"_EMER"
#define DCF_DEFAULT_FILE_FREECHAR_3   	"EMER"
/*
DB3
*/
#define DCF_ROOT_DIR_NAME_4				"IMAG"
#define DCF_DEFAULT_DIR_FREECHAR_4      "_IMAG"
#define DCF_DEFAULT_FILE_FREECHAR_4     DSC_DEFAULT_FLIE_FREECHAR

/*
DCFX
*/
#define DCFX_DB_NAME					"BASE.dat"

/*
DB Cluster TH
*/
#define DCF_CLUSTER_NUMERATOR_DB0       (49)// NORMAL
#define DCF_CLUSTER_NUMERATOR_DB1       (20)// PARK
#define DCF_CLUSTER_NUMERATOR_DB2       (30)// EMER
#define DCF_CLUSTER_NUMERATOR_DB3       (1) // PICT

#define DCF_CLUSTER_NUMERATOR1_DB0       (34)// NORMAL
#define DCF_CLUSTER_NUMERATOR1_DB1       (20)// PARK
#define DCF_CLUSTER_NUMERATOR1_DB2       (45)// EMER
#define DCF_CLUSTER_NUMERATOR1_DB3       (1) // PICT

#define DCF_CLUSTER_NUMERATOR2_DB0       (39)// NORMAL
#define DCF_CLUSTER_NUMERATOR2_DB1       (30)// PARK
#define DCF_CLUSTER_NUMERATOR2_DB2       (30)// EMER
#define DCF_CLUSTER_NUMERATOR2_DB3       (1) // PICT

#define DCF_CLUSTER_NUMERATOR3_DB0       (69)// NORMAL
#define DCF_CLUSTER_NUMERATOR3_DB1       (0) // PARK
#define DCF_CLUSTER_NUMERATOR3_DB2       (30)// EMER
#define DCF_CLUSTER_NUMERATOR3_DB3       (1) // PICT

#define DCF_FILE_NUMERATOR2_DB0       	 (500)// NORMAL
#define DCF_FILE_NUMERATOR2_DB1       	 (500)// PARK
#define DCF_FILE_NUMERATOR2_DB2       	 (500)// EMER
#define DCF_FILE_NUMERATOR2_DB3       	 (500) // PICT

#define DCF_CLUSTER_DENOMINATOR         (100)

#define DELETION_STYLE                  (DELETION_CARDV)
#define DELETION_BY_FILE_NUM             (1)   //TBD, add it temporarily till SFN DB update, by Jerry Li
//OSD
#define MAX_OSD_NUM                    	32
#define ENABLE_ALPHA_BLENDING           (0)

#define DATESTAMP_COLOR                 DATESTAMP_COLOR_WHITE

// video
#define AHC_VIDEO_STICKER_POS               AHC_ACC_TIMESTAMP_BOTTOM_RIGHT    
#define STICKER_PATTERN 					(1)
#define STICKER_PATTERN_COLOR				(DATESTAMP_COLOR_ORANGE)
#define AHC_VIDEO_STICKER_X_DEFAULT_POS     (160)
#define AHC_VIDEO_STICKER_Y_DEFAULT_POS     (40)
#define AHC_VIDEO_STICKER_Y_DELTA           (30)	// 60

//capture
#define AHC_CAPTURE_STICKER_POS             AHC_ACC_TIMESTAMP_BOTTOM_RIGHT    

#define AHC_CAPTURE_STICKER_X_DEFAULT_POS   (160)
#define AHC_CAPTURE_STICKER_Y_DEFAULT_POS   (40)
#define AHC_CAPTURE_STICKER_Y_DELTA         (40)

//gps
#define AHC_GPS_STICKER_ENABLE              (0)

#define THUMBNAIL_OSD_COLOR_FORMAT     		OSD_COLOR_RGB565
#define THUMBNAIL_BACKGROUND_COLOR          RGB565_COLOR_GRAY
#define THUMBNAIL_FOCUS_COLOR               RGB565_COLOR_BLUE2

//Browser/Thumbnail
#define MAX_DCF_IN_THUMB            	256
#define VIDEO_BS_SIZE_FOR_THUMB     	(512 * 1024)

#define SORT_TYPE_INCREASE              (0)
#define SORT_TYPE_DECREASE              (1)
#define SORT_TYPE                       SORT_TYPE_INCREASE
#define MIMF_CUSTOMIZED_SORT_TYPE       (0)
#define MIMF_CUSTOMIZED_SORT_MOV_ALL       (1)
#define MIMF_CUSTOMIZED_SORT_JPG_ALL       (1)

//Video Record
#define MIN_VIDEO_ENC_WIDTH         	64
#define MIN_VIDEO_ENC_HEIGHT        	64

#if (AHC_DRAM_SIZE == COMMON_DRAM_SIZE_32MB)

#define MAX_VIDEO_ENC_WIDTH         	(2304)
#define MAX_VIDEO_ENC_HEIGHT        	(1296)

#else

#define MAX_VIDEO_ENC_WIDTH         	(4096)
#define MAX_VIDEO_ENC_HEIGHT        	(2160)

#endif

#define ENABLE_DVS						0
#define ENABLE_VIDEO_ERR_LOG			0

#if (ENABLE_DVS == 1)
#define AHC_ENABLE_VIDEO_STICKER		0
#else
#define AHC_ENABLE_VIDEO_STICKER	    1
#endif

#define VIDOE_STICKER_INDEX1_EN         0

#define AHC_ENABLE_STILL_STICKER		1

#if (DCF_DB_COUNT == 1)
#define PROTECT_FILE_TYPE               (PROTECT_FILE_RENAME)
#else
#define PROTECT_FILE_TYPE               (PROTECT_FILE_MOVE_DB)
#endif

#ifdef FILE_RENAME_STRING
#define DCF_CHARLOCK_PATTERN            FILE_RENAME_STRING
#else
#define DCF_CHARLOCK_PATTERN            "SOS_"
#endif

#define VR_SEAMLESS 					(1)  

#define VR_SLOW_CARD_DETECT             (1)

#define VRCB_TOTAL_SKIP_FRAME           (100)

#define VRCB_CONTINUOUS_SKIP_FRAME      (5)

//Audio Record
#define MAX_AUDIO_ENC_FORMAT        	3

//DSC DBG
#define MAX_DBG_ARRAY_SIZE          	(64*1024)

//EXIF
#define MAX_EXIF_ASCII_LENGTH       	64
#define MAX_EXIF_CONFIG_ARRAY_SIZE  	(16 * 1024)

//STROBE
#define ENABLE_STROBE					0

//IRIS
#define ENABLE_IRIS					    0

//M SHUTTER
#define ENABLE_MECHANICAL_SHUTTER   	0

// USB
#define MTP_FUNC						0///< SUPPORT_MTP_FUNC is connected with this definition
//#define CONFIG_USB_PRODUCT_STR          {'M','i','V','u','e',' ','D','V','R',' ',' ',' ',' ',' '}

#define UVC_HOST_VIDEO_ENABLE           (SUPPORT_USB_HOST_FUNC & (AHC_DRAM_SIZE != COMMON_DRAM_SIZE_32MB) & 1)

// SD Card
#define ENABLE_SD_HOT_PLUGGING      	1

//Battery detection UART message
#define ENABLE_BATTERY_DET_DEBUG		(BATTERY_DETECTION_EN & 0)
#define ENABLE_BATTERY_TEMP_DEBUG	    (BATTERY_DETECTION_EN & 0)

//Speaker Control
#define NEVER_TURN_OFF_SPEAKER			0

// GPS to RTC time offset
#define GPS_TO_RTC_TIME_OFFSET          0

// RTC
#define RTC_DEFAULT_YEAR				(2011)
#define RTC_MAX_YEAR                	(2099)

//#define PRIMARY_DATESTAMP_WIDTH        	320
//#define PRIMARY_DATESTAMP_HEIGHT       	64
//#define PRIMARY_DATESTAMP_COLOR_FORMAT 	OSD_COLOR_RGB332
//#define THUMB_DATESTAMP_WIDTH          	320
//#define THUMB_DATESTAMP_HEIGHT         	64
//#define THUMB_DATESTAMP_COLOR_FORMAT   	OSD_COLOR_RGB332

#define AHC_CAPTURE_STICKER_HEIGHT      PRIMARY_DATESTAMP_HEIGHT
#define AHC_BROWSER_THUMBNAIL_STYLE     AHC_BROWSER_THUMBNAIL_STYLE_1
#define AHC_BROWSER_THUMBNAIL_NO_GRAY_BORDER

#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0) 
#define AHC_HD_VIDPLAY_MAX_WIDTH        (1280)
#define AHC_HD_VIDPLAY_MAX_HEIGHT       (720)
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
#define AHC_HD_VIDPLAY_MAX_WIDTH        (720)
#define AHC_HD_VIDPLAY_MAX_HEIGHT       (1280)
#endif

#define STICKER_FRONT_1                 GUI_Font32B_1
#define STICKER_FRONT_2                 GUI_Font24B_1

#define PARKINGMODE_NONE				(0)
#define PARKINGMODE_STYLE_1				(1)
#define PARKINGMODE_STYLE_2				(2)
#define PARKINGMODE_STYLE_3				(3)
#define PARKINGMODE_STYLE_4				(4)
#define SUPPORT_PARKINGMODE				(PARKINGMODE_STYLE_4)//(PARKINGMODE_NONE) ///(PARKINGMODE_STYLE_2)

#define EMER_RECORD_DUAL_WRITE_ENABLE   (0)	
#define EMER_RECORD_DUAL_WRITE_PRETIME  (5)  // 5s  // Pre-recording time of emergency recording
#define EMER_RECORD_DUAL_WRTIE_DELTA    (5)
#define EMER_RECORD_DUAL_WRITE_POSTTIME (10) //10s  // The increased time if triggered again and remaining emergency recording time <= EMER_RECORD_DUAL_WRTIE_DELTA
#define EMER_RECORD_DUAL_WRITE_INTERVAL (20) //10s  // The emergency recorded time after 1st trigger (including pre-recording time)
#define EMER_RECORD_DUAL_WRTIE_MAX_TIME (120)//120s = 2mins // Max time for an emergency recording

#define UUID_XML_FEATURE                (0) ///(1)

#define SPEEM_CAM_MANUAL_DB				("SF:2:\\SpeedCam_Data_Manual.bin")

#define LOAD_MENU_DEFFAULT_SETTING_FROM_FILE  0
#define MENU_DEFAULT_SETTING_FILENAME         "SF:0:\\MenuSetting.bin"

#define AHC_VIDEO_PLUG_OUT_AC				   (0)///(1)

#define PARK_RECORD_WRTIE_MAX_TIME				(180)
#define PARK_RECORD_WRITE_POSTTIME 				(10)
#define PARK_RECORD_INTERVAL					(10) 

#define ENABLE_GUI_SUPPORT_MULTITASK

#define CFG_MVD_MODE_LINK_WITH_MENU_SENSITIVITY
//#define CFG_ENABLE_MIN_VR_TIME                  (1000)      // ms
#define LOCK_KEYPAD_UNDER_WIFI_STREAMING		
//EDOG Setting
#define   AHC_EDOG_POI_DATA_SEGMENTATION         				(1)		//1:for China, 0:for other
#include "AHC_Config_Dummy.h"

#endif	// __AHC_CONFIG_SDK_H__

/**
 @file mmps_3gprecd.h
 @brief Header File for the Host 3GP RECORDER API.
 @author Will Tseng
 @version 1.0
*/

#ifndef _MMPS_3GPRECD_H_
#define _MMPS_3GPRECD_H_

//===============================================================================
//
//                               INCLUDE FILE
//
//===============================================================================

#include "mmp_lib.h"
#include "config_fw.h"
#include "mmp_graphics_inc.h"
#include "mmp_rawproc_inc.h"
#include "mmp_icon_inc.h"
#include "mmp_snr_inc.h"
#include "mmp_mjpeg_inc.h"
#include "mmp_media_def.h"
#include "mmp_mux_inc.h"
#include "mmps_dsc.h"
#include "mmps_rtc.h"
#include "mmps_system.h"
#include "mmpd_mp4venc.h"
#include "mmpd_3gpmgr.h"
#include "mmpf_mp4venc.h"

//===============================================================================
//
//                               MACRO DEFINE
//
//===============================================================================

#define VR_MAX_RAWSTORE_BUFFER_NUM  	(3)
#define VR_MIN_RAWSTORE_BUFFER_NUM  	(2)

#define VR_MAX_PREVIEW_NUM              (SENSOR_MAX_NUM)
#define VR_MAX_ENCODE_NUM				(MAX_VIDEO_STREAM_NUM)

#if (HANDLE_JPEG_EVENT_BY_QUEUE) && (!USE_H264_CUR_BUF_AS_CAPT_BUF)
#define VR_MAX_CAPTURE_WIDTH          	(1280)
#define VR_MAX_CAPTURE_HEIGHT         	(720)
#else
#define VR_MAX_CAPTURE_WIDTH          	(1920)
#define VR_MAX_CAPTURE_HEIGHT         	(1080)
#endif
#define VR_MAX_THUMBNAIL_WIDTH          (640)
#define VR_MAX_THUMBNAIL_HEIGHT         (480)

#if(DUALENC_SUPPORT)
#define VR_MAX_RECORD_WIDTH          	(2304)
#define VR_MAX_RECORD_HEIGHT         	(1296)
#endif
#define MIN_TIME_TO_RECORD              (2)              ///< 2 seconds

//===============================================================================
//
//                               ENUMERATION
//
//===============================================================================

/// Firmware Status
typedef enum _MMPS_FW_OPERATION {
    MMPS_FW_OPERATION_NONE = 0x00,						///< FW operation, none
    MMPS_FW_OPERATION_START,							///< FW operation, start
    MMPS_FW_OPERATION_PAUSE,							///< FW operation, pause
    MMPS_FW_OPERATION_RESUME,							///< FW operation, resume
    MMPS_FW_OPERATION_STOP,								///< FW operation, stop
    MMPS_FW_OPERATION_PREENCODE                         ///< FW operation, pre encode
} MMPS_FW_OPERATION;

/// Video Format
typedef enum _MMPS_3GPRECD_VIDEO_FORMAT {
    MMPS_3GPRECD_VIDEO_FORMAT_OTHERS = 0x00,			///< Video format, none
    MMPS_3GPRECD_VIDEO_FORMAT_H264,                     ///< Video format, H.264
    MMPS_3GPRECD_VIDEO_FORMAT_MJPEG,                    ///< Video format, MJPEG
    MMPS_3GPRECD_VIDEO_FORMAT_YUV422,                   ///< Video format, YUV422
    MMPS_3GPRECD_VIDEO_FORMAT_YUV420,
    MMPS_3GPRECD_VIDEO_FORMAT_MAXNUM
} MMPS_3GPRECD_VIDEO_FORMAT;

/// Video Current Buffer Mode
typedef enum _MMPS_3GPRECD_CURBUF_MODE {
    MMPS_3GPRECD_CURBUF_FRAME = 0x00,
    MMPS_3GPRECD_CURBUF_RT,
    MMPS_3GPRECD_CURBUF_MAX
} MMPS_3GPRECD_CURBUF_MODE;

/// Video Speed Mode
typedef enum _MMPS_3GPRECD_SPEED_MODE {
    MMPS_3GPRECD_SPEED_NORMAL = 0,                      ///< Video speed, normal
    MMPS_3GPRECD_SPEED_SLOW,                            ///< Video speed, slow
    MMPS_3GPRECD_SPEED_FAST                             ///< Video speed, fast
} MMPS_3GPRECD_SPEED_MODE;

/// Video Speed Ratio
typedef enum _MMPS_3GPRECD_SPEED_RATIO {
    MMPS_3GPRECD_SPEED_1X = 0,                          ///< Video speed ratio
    MMPS_3GPRECD_SPEED_2X,
    MMPS_3GPRECD_SPEED_3X,
    MMPS_3GPRECD_SPEED_4X,
    MMPS_3GPRECD_SPEED_5X,
    MMPS_3GPRECD_SPEED_MAX
} MMPS_3GPRECD_SPEED_RATIO;

/// Video Event
typedef enum _MMPS_3GPRECD_EVENT {
    MMPS_3GPRECD_EVENT_NONE = 0,
    MMPS_3GPRECD_EVENT_MEDIA_FULL,
    MMPS_3GPRECD_EVENT_FILE_FULL,
    MMPS_3GPRECD_EVENT_LONG_TIME_FILE_FULL,
    MMPS_3GPRECD_EVENT_MEDIA_SLOW,
    MMPS_3GPRECD_EVENT_SEAMLESS,
    MMPS_3GPRECD_EVENT_MEDIA_ERROR,
    MMPS_3GPRECD_EVENT_ENCODE_START,
    MMPS_3GPRECD_EVENT_ENCODE_STOP,
	#if (DUALENC_SUPPORT == 1)
    MMPS_3GPRECD_EVENT_DUALENCODE_START,
    MMPS_3GPRECD_EVENT_DUALENCODE_STOP,
	#endif
    MMPS_3GPRECD_EVENT_POSTPROCESS,
    MMPS_3GPRECD_EVENT_BITSTREAM_DISCARD,
    MMPS_3GPRECD_EVENT_MEDIA_WRITE,
    MMPS_3GPRECD_EVENT_STREAMCB,
    MMPS_3GPRECD_EVENT_EMERGFILE_FULL,
    MMPS_3GPRECD_EVENT_RECDSTOP_CARDSLOW,
    MMPS_3GPRECD_EVENT_APSTOPVIDRECD,
    MMPS_3GPRECD_EVENT_PREGETTIME_CARDSLOW,
    MMPS_3GPRECD_EVENT_UVCFILE_FULL,
    MMPS_3GPRECD_EVENT_COMPBUF_FREE_SPACE,
    MMPS_3GPRECD_EVENT_APNEED_STOP_RECD,
    MMPS_3GPRECD_EVENT_DUALENC_FILE_FULL,
    MMPS_3GPRECD_EVENT_MAX
} MMPS_3GPRECD_EVENT;

/// Audio Format, Sample rate, Bit rate
typedef enum _MMPS_3GPRECD_AUDIO_OPTION {
    MMPS_3GPRECD_AUDIO_AAC_BASE 		= 0x00,			///< Audio AAC format
    MMPS_3GPRECD_AUDIO_AAC_16K_32K		= 0x02,			///< AAC    16KHz with 32kbps
    MMPS_3GPRECD_AUDIO_AAC_16K_64K,						///< AAC    16KHz with 64kbps
    MMPS_3GPRECD_AUDIO_AAC_22d05K_64K,					///< AAC 22.05KHz with 64kbps
    MMPS_3GPRECD_AUDIO_AAC_22d05K_128K,					///< AAC 22.05KHz with 128kbps
    MMPS_3GPRECD_AUDIO_AAC_32K_64K,						///< AAC 32KHz with 64kbps
    MMPS_3GPRECD_AUDIO_AAC_32K_128K,					///< AAC 32KHz with 128kbps
    MMPS_3GPRECD_AUDIO_AAC_48K_128K,					///< AAC 48KHz with 128kbps
    MMPS_3GPRECD_AUDIO_AAC_44d1K_64K,					///< AAC 44.1KHz with 64kbps
    MMPS_3GPRECD_AUDIO_AAC_44d1K_128K,					///< AAC 44.1KHz with 128kbps

    MMPS_3GPRECD_AUDIO_AMR_BASE 		= 0x10,			///< Audio AMR format
    MMPS_3GPRECD_AUDIO_AMR_4d75K,						///< AMR 4.75KHz with 8kbps
    MMPS_3GPRECD_AUDIO_AMR_5d15K,						///< AMR 5.15KHz with 8kbps
    MMPS_3GPRECD_AUDIO_AMR_12d2K,						///< AMR 12.2KHz with 8kbps

    //ADPCM section, need to sync with MMPF_SetADPCMEncMode
    MMPS_3GPRECD_AUDIO_ADPCM_16K_22K,					///< ADPCM 16KHz with 22kbps
    MMPS_3GPRECD_AUDIO_ADPCM_32K_22K,					///< ADPCM 32KHz with 22kbps
    MMPS_3GPRECD_AUDIO_ADPCM_44d1K_22K,					///< ADPCM 44.1KHz with 22kbps
    MMPS_3GPRECD_AUDIO_ADPCM_48K_22K,                   ///< ADPCM 48KHz with 22kbps

    MMPS_3GPRECD_AUDIO_MP3_32K_128K,                    ///< MP3 32KHz with 128kbps
    MMPS_3GPRECD_AUDIO_MP3_44d1K_128K,                  ///< MP3 44.1KHz with 128kbps

    MMPS_3GPRECD_AUDIO_PCM_16K,                         ///< Raw PCM 16KHz
    MMPS_3GPRECD_AUDIO_PCM_32K                          ///< Raw PCM 32KHz
} MMPS_3GPRECD_AUDIO_OPTION;

/// Audio Format
typedef enum _MMPS_3GPRECD_AUDIO_FORMAT {
    MMPS_3GPRECD_AUDIO_FORMAT_AAC = 0x00,               ///< Video encode with AAC audio
    MMPS_3GPRECD_AUDIO_FORMAT_AMR,                      ///< Video encode with AMR audio
    MMPS_3GPRECD_AUDIO_FORMAT_ADPCM,           	        ///< Video encode with ADPCM audio
    MMPS_3GPRECD_AUDIO_FORMAT_MP3,                      ///< video encode with MP3 audio
    MMPS_3GPRECD_AUDIO_FORMAT_PCM                       ///< video encode with raw PCM audio
} MMPS_3GPRECD_AUDIO_FORMAT;

/// Audio mode
typedef enum _MMPS_3GPRECD_AUDIO_DATATYPE {
    MMPS_3GPRECD_NO_AUDIO_DATA = 0,
    MMPS_3GPRECD_REC_AUDIO_DATA,
    MMPS_3GPRECD_SILENCE_AUDIO_DATA
} MMPS_3GPRECD_AUDIO_DATATYPE;

/// Video Source
typedef enum _MMPS_3GPRECD_SRCMODE {
    MMPS_3GPRECD_SRCMODE_MEM = 0x00,            		///< Video encode by memory mode
    MMPS_3GPRECD_SRCMODE_CARD,            				///< Video encode by card mode
    MMPS_3GPRECD_SRCMODE_STREAM,                        ///< Video encode by call-back
    MMPS_3GPRECD_SRCMODE_MAX
} MMPS_3GPRECD_SRCMODE;

/// Video Profile
typedef enum _MMPS_3GPRECD_PROFILE {
    MMPS_H263ENC_PROFILE_NONE = 0x00,
    MMPS_H263ENC_PROFILE_0,                             ///< H.263 profile 0
    MMPS_H263ENC_PROFILE_3,                             ///< H.263 profile 3
    MMPS_H263ENC_PROFILE_MAX,
    MMPS_MP4VENC_PROFILE_NONE,
    MMPS_MP4VENC_SIMPLE_PROFILE,                        ///< MPEG4 simple profile
    MMPS_MP4VENC_ADV_SIMPLE_PROFILE,                    ///< MPEG4 advanced simple profile
    MMPS_MP4VENC_PROFILE_MAX,
    MMPS_H264ENC_PROFILE_NONE,
    MMPS_H264ENC_BASELINE_PROFILE,                      ///< H.264 baseline profile
    MMPS_H264ENC_MAIN_PROFILE,                          ///< H.264 main profile
    MMPS_H264ENC_HIGH_PROFILE,                          ///< H.264 high profile
    MMPS_H264ENC_PROFILE_MAX
} MMPS_3GPRECD_PROFILE;

/// Encode Parameters
typedef enum _MMPS_3GPRECD_PARAMETER {
	MMPS_3GPRECD_PARAMETER_PREVIEWMODE = 0x0,			///< Encode sensor mode
	MMPS_3GPRECD_PARAMETER_SRCMODE,						///< Card or memory mode
	MMPS_3GPRECD_PARAMETER_VIDEO_FORMAT,				///< Video format
	MMPS_3GPRECD_PARAMETER_AUDIO_FORMAT,				///< Audio format
	MMPS_3GPRECD_PARAMETER_RESOLUTION,					///< Video resolution
	MMPS_3GPRECD_PARAMETER_BITRATE,
	MMPS_3GPRECD_PARAMETER_FRAME_RATE,                  ///< Video frame rate
	MMPS_3GPRECD_PARAMETER_PROFILE,                     ///< Video codec profile
	MMPS_3GPRECD_PARAMETER_DRAM_END                     ///< End of DRAM address for video encode
} MMPS_3GPRECD_PARAMETER;

// Stream Callback Type
typedef enum _MMPS_3GPRECD_STREAMCB_TYPE {
	MMPS_3GPRECD_STREAMCB_VIDMOVE = 0,    // move video stream
	MMPS_3GPRECD_STREAMCB_VIDUPDATE,      // update video stream
	MMPS_3GPRECD_STREAMCB_AUDMOVE,        // move audio stream
	MMPS_3GPRECD_STREAMCB_AUDUPDATE       // update audio stream
} MMPS_3GPRECD_STREAMCB_TYPE;

/// Video Container
typedef enum _MMPS_3GPRECD_CONTAINER {
    MMPS_3GPRECD_CONTAINER_3GP = 0,
    MMPS_3GPRECD_CONTAINER_AVI,
    MMPS_3GPRECD_CONTAINER_NONE,
    MMPS_3GPRECD_CONTAINER_UNKNOWN
} MMPS_3GPRECD_CONTAINER;

/// Video Frame Type
typedef enum _MMPS_3GPRECD_FRAME_TYPE {
    MMPS_3GPRECD_FRAME_TYPE_I = 0,
    MMPS_3GPRECD_FRAME_TYPE_P,
    MMPS_3GPRECD_FRAME_TYPE_B,
    MMPS_3GPRECD_FRAME_TYPE_MAX
} MMPS_3GPRECD_FRAME_TYPE;

/// H264 Padding Type
typedef enum _MMPS_3GPRECD_PADDING_TYPE {
    MMPS_3GPRECD_PADDING_NONE,
    MMPS_3GPRECD_PADDING_ZERO,
    MMPS_3GPRECD_PADDING_REPEAT
} MMPS_3GPRECD_PADDING_TYPE;

typedef enum _MMPS_UVCRECD_FILE_TYPE {
    MMPS_UVCRECD_MULFILE,
    MMPS_UVCRECD_MULTRACK
} MMPS_UVCRECD_FILE_TYPE;

#if (SUPPORT_USB_HOST_FUNC)
//sync MMPF_USBH_UPD_UVC_CFG_OP
typedef enum _MMPS_UVCRECD_UPD_UVC_CFG_OP {
    MMPS_UVCRECD_UPD_OP_NONE            = 0,
    MMPS_UVCRECD_UPD_OPEN_UVC_CB        = 1,
    MMPS_UVCRECD_UPD_START_UVC_CB       = 2,
    MMPS_UVCRECD_UPD_NALU_TBL           = 3,
    MMPS_UVCRECD_UPD_OP_MAX             = 4
} MMPS_UVCRECD_UPD_UVC_CFG_OP;

//sync MMPF_USBH_REG_UVC_ADDI_CFG_TYP
typedef enum _MMPS_UVCRECD_REG_UVC_ADDI_CFG_TYP {
    MMPS_UVCRECD_REG_TYP_NONE           = 0,
    MMPS_UVCRECD_REG_TYP_GUID_EU        = 1,
    MMPS_UVCRECD_REG_TYP_MAX            = 2
} MMPS_UVCRECD_REG_UVC_ADDI_CFG_TYP;
#endif

#if (SUPPORT_VUI_INFO)
typedef enum _MMPS_SEI_SHUTTER_TYPE {
	MMPS_SEI_SHUTTER_1920x1080_29d97 = 0,
	MMPS_SEI_SHUTTER_1280x720_59d94,
	MMPS_SEI_SHUTTER_1280x720_29d97,
	MMPS_SEI_SHUTTER_848x480_29d97,
	MMPS_SEI_SHUTTER_NONE
} MMPS_SEI_SHUTTER_TYPE;
#endif

typedef enum _MMPS_3GPRECD_Y_FRAME_TYPE {
    MMPS_3GPRECD_Y_FRAME_TYPE_NONE = 0,
    MMPS_3GPRECD_Y_FRAME_TYPE_VMD,
    MMPS_3GPRECD_Y_FRAME_TYPE_ADAS,
    MMPS_3GPRECD_Y_FRAME_TYPE_MAX
} MMPS_3GPRECD_Y_FRAME_TYPE;

// Zoom Path
typedef enum _MMPS_3GPRECD_ZOOM_PATH {
	MMPS_3GPRECD_ZOOM_PATH_NONE = 0,
	MMPS_3GPRECD_ZOOM_PATH_PREV,
	MMPS_3GPRECD_ZOOM_PATH_RECD,
	MMPS_3GPRECD_ZOOM_PATH_BOTH,
	MMPS_3GPRECD_ZOOM_PATH_MAX
} MMPS_3GPRECD_ZOOM_PATH;

// For 3GP preview/encode path selection
typedef enum _MMP_3GPRECD_PATH {
	MMP_3GP_PATH_2PIPE 			= 0,
	MMP_3GP_PATH_LDC_LB_SINGLE,
	MMP_3GP_PATH_LDC_LB_MULTI,
	MMP_3GP_PATH_LDC_MULTISLICE, 
	MMP_3GP_PATH_INVALID 		= 0xFF
} MMP_3GPRECD_PATH;

// For 3GP still capture source selection
typedef enum _MMP_3GPRECD_CAPTURE_SRC {
	MMP_3GPRECD_CAPTURE_SRC_FRONT_CAM   = 0x01,
	MMP_3GPRECD_CAPTURE_SRC_REAR_CAM    = 0x02
} MMP_3GPRECD_CAPTURE_SRC;

//===============================================================================
//
//                               STRUCTURES
//
//===============================================================================

// Stream callback buffer info
typedef struct _MMPS_3GPRECD_STREAM_INFO {
    MMP_ULONG  baseaddr;
    MMP_ULONG  segsize;
    MMP_ULONG  startaddr;
    MMP_ULONG  availsize;
} MMPS_3GPRECD_STREAM_INFO;

/// Frame rate
typedef struct _MMPS_3GPRECD_FRAMERATE {
    MMP_ULONG usVopTimeIncrement;  			///< Video encode VOP time increment (can be provided by the user)
    MMP_ULONG usVopTimeIncrResol;  			///< Video encode VOP time resolution (can be provided by the user)
} MMPS_3GPRECD_FRAMERATE;

// Still Capture information
typedef struct _MMPS_3GPRECD_STILL_CAPTURE_INFO {
    MMP_BYTE    *bFileName;
    MMP_ULONG   ulFileNameLen;
    MMP_USHORT  usWidth;
    MMP_USHORT  usHeight;
    MMP_USHORT  usThumbWidth;
    MMP_USHORT  usThumbHeight;
    MMP_BOOL    bThumbEn;
    MMP_BOOL    bExifEn;
    MMP_BOOL    bTargetCtl;
    MMP_BOOL    bLimitCtl;
    MMP_ULONG   bTargetSize;
    MMP_ULONG   bLimitSize;
    MMP_USHORT  bMaxTrialCnt;
    MMP_DSC_JPEG_QUALITY    Quality;
    MMP_3GPRECD_CAPTURE_SRC sCaptSrc;
    MMP_BYTE    *bRearFileName;
    MMP_ULONG   ulRearFileNameLen;
} MMPS_3GPRECD_STILL_CAPTURE_INFO;

/// Preview buffer info
typedef struct _MMPS_3GPRECD_PREVIEW_BUFINFO {
    MMP_ULONG  ulYBuf[4];       						///< Video encode preview Y buffer
    MMP_ULONG  ulUBuf[4];       						///< Video encode preview U buffer
    MMP_ULONG  ulVBuf[4];       						///< Video encode preview V buffer
    MMP_ULONG  ulRotateYBuf[2];                         ///< Video preview rotate DMA dst Y buffer
    MMP_ULONG  ulRotateUBuf[2];                         ///< Video preview rotate DMA dst U buffer
    MMP_ULONG  ulRotateVBuf[2];                         ///< Video preview rotate DMA dst V buffer
    MMP_USHORT usRotateBufCnt;                          ///< Video preview rotate DMA dst buffer count
    MMP_USHORT usBufCnt;								///< Video encode preview buffer count
} MMPS_3GPRECD_PREVIEW_BUFINFO;

/// Motion Detection buffer info
typedef struct _MMPS_3GPRECD_MDTC_BUFINFO {
    MMP_ULONG  ulYBuf[2];       						///< MDTC source Y buffer (Pipe4)
    MMP_USHORT usBufCnt;								///< MDTC source buffer count
} MMPS_3GPRECD_MDTC_BUFINFO;

/// Preview and display configuration
typedef struct _MMPS_3GPRECD_PREVIEW_DATA {
    MMP_USHORT  			usVidPreviewBufW[VIDRECD_PREV_MODE_MAX_NUM];	///< Specify preview buffer width
    MMP_USHORT  			usVidPreviewBufH[VIDRECD_PREV_MODE_MAX_NUM];	///< Specify preview buffer height
	MMP_SCAL_FIT_MODE		sFitMode[VIDRECD_PREV_MODE_MAX_NUM];			///< Specify scaler fit mode

	//++ For Display Window/Device Attribute
	MMP_DISPLAY_COLORMODE 	DispColorFmt[VIDRECD_PREV_MODE_MAX_NUM];		///< Preview color format
    MMP_DISPLAY_DEV_TYPE 	DispDevice[VIDRECD_PREV_MODE_MAX_NUM];			///< Display mode of NORMAL, FLM or TV
    MMP_DISPLAY_WIN_ID 		DispWinId[VIDRECD_PREV_MODE_MAX_NUM];			///< Display window index
    MMP_USHORT 				usVidDispBufCnt[VIDRECD_PREV_MODE_MAX_NUM]; 	///< preview pipe buffer count
    MMP_USHORT 				usVidDisplayW[VIDRECD_PREV_MODE_MAX_NUM];		///< Preview pipe display window width
    MMP_USHORT 				usVidDisplayH[VIDRECD_PREV_MODE_MAX_NUM];		///< Preview pipe display window height    
    MMP_USHORT 				usVidDispStartX[VIDRECD_PREV_MODE_MAX_NUM];		///< Preview display window X offset
    MMP_USHORT 				usVidDispStartY[VIDRECD_PREV_MODE_MAX_NUM];		///< Preview display window Y offset
    MMP_BOOL   				bVidDispMirror[VIDRECD_PREV_MODE_MAX_NUM];		///< Mirror mode
    MMP_DISPLAY_ROTATE_TYPE VidDispDir[VIDRECD_PREV_MODE_MAX_NUM];			///< Rotate mode
   	//-- For Display Window/Device Attribute
   	
   	//++ For DMA rotate
   	MMP_BOOL   				bUseRotateDMA[VIDRECD_PREV_MODE_MAX_NUM];    	///< Preview uses rotate DMA to rotate or not
    MMP_USHORT 				usRotateBufCnt[VIDRECD_PREV_MODE_MAX_NUM];   	///< Preview rotate DMA dst buffer count
    MMP_DISPLAY_ROTATE_TYPE	ubDMARotateDir[VIDRECD_PREV_MODE_MAX_NUM];		///< Preview rotate DMA direction
    //-- For DMA rotate
} MMPS_3GPRECD_PREVIEW_DATA;

/// Global settings
typedef struct _MMPS_3GPRECD_MODES {
    MMP_USHORT  usVideoPreviewMode;                                 ///< Index of video encode preview modes
    MMP_USHORT  usVideoEncResIdx[VR_MAX_ENCODE_NUM];                ///< Index of video encode resolutions
    MMPS_3GPRECD_FRAMERATE SnrInputFrameRate[VR_MAX_ENCODE_NUM];    ///< Sensor input frame rate settings
    MMPS_3GPRECD_FRAMERATE VideoEncFrameRate[VR_MAX_ENCODE_NUM];    ///< Video encode frame rate settings
    MMPS_3GPRECD_FRAMERATE ContainerFrameRate[VR_MAX_ENCODE_NUM];   ///< Container frame rate settings
    MMP_USHORT  usPFrameCount[VR_MAX_ENCODE_NUM];                   ///< P frame count in 1 GOP
    MMP_USHORT  usBFrameCount[VR_MAX_ENCODE_NUM];                   ///< Consecutive B frame count
    MMP_ULONG   ulBitrate[VR_MAX_ENCODE_NUM];                       ///< Video bitrate
    MMP_ULONG   ulAudBitrate;                                       ///< Audio bitrate
    MMP_ULONG   ulSizeLimit;                                        ///< Video stream size limit
    MMP_ULONG   ulTimeLimitMs;                                      ///< Video stream time limit in unit of ms
    MMP_ULONG   ulReservedSpace;                                    ///< Reserved storage space which can't be used by recorder or other application
    MMPS_3GPRECD_SRCMODE VideoSrcMode[VR_MAX_ENCODE_NUM];           ///< Memory mode or card mode
    MMPS_3GPRECD_PROFILE VisualProfile[VR_MAX_ENCODE_NUM];          ///< Visual profile
    MMPS_3GPRECD_CURBUF_MODE VidCurBufMode[VR_MAX_ENCODE_NUM];      ///< Video Current Buffer Mode
    MMP_BOOL    bSlowMotionEn;
} MMPS_3GPRECD_MODES;

typedef struct _MMPS_3GPRECD_CONFIGS {
    //++ For Preview 
    MMP_UBYTE            				previewpath[VR_MAX_PREVIEW_NUM];
    MMPS_3GPRECD_PREVIEW_DATA 	        previewdata[VR_MAX_PREVIEW_NUM];                                    ///< Customized preview configuration
    //-- For Preview 

	//++ For Encode 
    MMP_USHORT                          usEncWidth[VIDRECD_RESOL_MAX_NUM];                            		///< Video encode width by resolution
    MMP_USHORT                          usEncHeight[VIDRECD_RESOL_MAX_NUM];                          		///< Video encode height by resolution
    MMP_ULONG                           ulFps30BitrateMap[VIDRECD_RESOL_MAX_NUM][VIDRECD_QLEVEL_MAX_NUM];	///< Bitrate map for 30fps
    MMP_ULONG                           ulFps60BitrateMap[VIDRECD_RESOL_MAX_NUM][VIDRECD_QLEVEL_MAX_NUM];   ///< Bitrate map for 60fps
    MMPS_3GPRECD_FRAMERATE 		        framerate[VIDRECD_FPS_MAX_NUM];                                     ///< Video encode frame rate settings
    //-- For Encode

    //++ For Feature
    MMP_BOOL                            bAsyncMode;                                                         ///< Async mode enable or not
    MMP_BOOL                            bSeamlessMode;                                                      ///< Support seamless recording or not
    MMP_BOOL                            bFdtcEnable;
    MMP_BOOL                            bRawPreviewEnable[VR_MAX_PREVIEW_NUM];                              ///< Raw preview enable or not (Per Raw module)
    MMP_UBYTE							ubRawPreviewBitMode[VR_MAX_PREVIEW_NUM];
	MMP_BOOL                            bStillCaptureEnable;
	MMP_BOOL                            bDualCaptureEnable;
	MMP_BOOL							bWifiStreamEnable;
	#if (SUPPORT_VR_WIFI_STREAM)
	MMP_ULONG							ulMaxWifiStreamWidth;
	MMP_ULONG							ulMaxWifiStreamHeight;
	#endif

   	//-- For Feature

    //++ For Buffer Reserved Size 
    MMP_ULONG                           ulRawStoreBufCnt;
    MMP_ULONG                           ulTailBufSize;
    MMP_ULONG                           ulVideoCompBufSize;
    MMP_ULONG                           ulAudioCompBufSize;
    MMP_ULONG                           ulUVCVidCompBufSize;
    MMP_ULONG                           ul2ndStreamVidCompBufSize;
    #if (VIDRECD_MULTI_TRACK == 0)
    MMP_ULONG                           ulUVCTailBufSize;
    #endif
    #if (UVC_EMERGRECD_SUPPORT == 1 && VIDRECD_MULTI_TRACK == 0)
    MMP_ULONG                           ulUVCEmergTailBufSize;
    #endif
    #if (VIDRECD_REFIXRECD_SUPPORT == 1)
    MMP_ULONG                		    ulReFixTailBufSize;
    #endif
	//-- For Buffer Reserved Size 
} MMPS_3GPRECD_CONFIGS;

#if (SUPPORT_VR_WIFI_STREAM)
/// Wifi global settings
typedef struct _MMPS_3GPRECD_WIFI_MODES {
    MMP_USHORT  usVideoEncResIdx;                ///< Index of video encode resolutions
    MMPS_3GPRECD_FRAMERATE SnrInputFrameRate;    ///< Sensor input frame rate settings
    MMPS_3GPRECD_FRAMERATE VideoEncFrameRate;    ///< Video encode frame rate settings
    MMPS_3GPRECD_FRAMERATE ContainerFrameRate;   ///< Container frame rate settings
    MMP_USHORT  usPFrameCount;                   ///< P frame count in 1 GOP
    MMP_USHORT  usBFrameCount;                   ///< Consecutive B frame count
    MMP_ULONG   ulBitrate;                       ///< Video bitrate
    MMPS_3GPRECD_SRCMODE VideoSrcMode;           ///< Memory mode or card mode
    MMPS_3GPRECD_PROFILE VisualProfile;          ///< Visual profile
    MMPS_3GPRECD_CURBUF_MODE VidCurBufMode;      ///< Video Current Buffer Mode
} MMPS_3GPRECD_WIFI_MODES;

typedef struct _MMPS_WIFISTREAM_OBJ
{
	MMP_BOOL				   bEnableWifi;
    MMP_USHORT		 		   usWifiStreamSrc;
    MMP_IBC_PIPEID 			   PipeID; 	         // IBC pipeline being used
    MMP_MJPEG_LINKATTR 	       *pLinkAttr;
    MMPS_3GPRECD_WIFI_MODES    WifiEncModes;
} MMPS_WIFISTREAM_OBJ;
#endif

#if (SUPPORT_TIMELAPSE)

#define MKTAG(a,b,c,d) (a | (b << 8) | (c << 16) | (d << 24))

// for time lapse
typedef struct _MMPS_3GPRECD_AVI_LIST {
    MMP_ULONG   List;
    MMP_ULONG   ulSize;
    MMP_ULONG   FourCC;
} MMPS_3GPRECD_AVI_LIST;

typedef struct _MMPS_3GPRECD_AVI_MainHeader {
    MMP_ULONG   FourCC;
    MMP_ULONG   ulSize;   // always 0x38bytes
    MMP_ULONG   ulMSecPreFrame;
    MMP_ULONG   ulMaxByteRate;
    MMP_ULONG   ulReServed;
    MMP_ULONG   ulFlag;
    MMP_ULONG   ulFrameNum;
    MMP_ULONG   ulInitFrame;
    MMP_ULONG   ulStreamNum;
    MMP_ULONG   ulBufferSize;
    MMP_ULONG   ulWidth;
    MMP_ULONG   ulHeight;
    MMP_ULONG   ulScale;
    MMP_ULONG   ulRate;
    MMP_ULONG   ulStart;
    MMP_ULONG   ulLength;
} MMPS_3GPRECD_AVI_MainHeader;

typedef struct _MMPS_3GPRECD_AVI_StreamHeader {
    MMP_ULONG   FourCC;
    MMP_ULONG   ulSize;   // always 0x38bytes
    MMP_ULONG   Type;
    MMP_ULONG   Handler;
    MMP_ULONG   ulFlag;
    MMP_ULONG   ulReServed;
    MMP_ULONG   InitFrame;
    MMP_ULONG   ulScale;
    MMP_ULONG   ulRate;
    MMP_ULONG   ulStart;
    MMP_ULONG   ulLength;
    MMP_ULONG   ulBufferSize;
    MMP_ULONG   ulQuality;
    MMP_ULONG   ulSampleSize;
    MMP_ULONG   ulReServed2;
    MMP_ULONG   ulReServed3;
} MMPS_3GPRECD_AVI_StreamHeader;

typedef struct _MMPS_3GPRECD_AVI_StreamFormat {
    MMP_ULONG   FourCC;
    MMP_ULONG   ulSize;     // always 0x28bytes
    MMP_ULONG   ulSize2;    // always 0x28
    MMP_ULONG   ulWidth;
    MMP_ULONG   ulHeight;
    MMP_ULONG   ulPlaneAndBitCount;
    MMP_ULONG   ubCompression;
    MMP_ULONG   ulImageSize;
    MMP_ULONG   ulXPelPerMeter;
    MMP_ULONG   ulYPelPerMeter;
    MMP_ULONG   ulColorUse;
    MMP_ULONG   ulColorBit;
} MMPS_3GPRECD_AVI_StreamFormat;

typedef struct _MMPS_3GPRECD_AVI_Header {
    MMP_ULONG   ubFourCC;
    MMP_ULONG   ulSize;   // always 327680bytes
} MMPS_3GPRECD_AVI_Header;

typedef struct _MMPS_3GPRECD_AVI_IndexEntry {
    MMP_ULONG   ubFourCC;
    MMP_ULONG   ulFlag;
    MMP_ULONG   ulPos;
    MMP_ULONG   ulSize;
} MMPS_3GPRECD_AVI_IndexEntry;

#endif

typedef struct _MMPS_3GPRECD_CONTAINER_INFO{
	MMP_USHORT		VideoEncodeFormat;  ///< video encode format
	MMP_ULONG       ulFrameWidth;       ///< video frame width
    MMP_ULONG       ulFrameHeight;      ///< video frame height
    MMP_ULONG       ulTimeIncrement;    ///< video time increment
    MMP_ULONG       ulTimeResolution;   ///< video time resolution   
    MMP_USHORT	    usPFrameCount;      ///< P frames count in one GOP
    MMP_USHORT      usBFrameCount;   	///< # consecutive B frames
    
    MMP_ULONG       ulSPSAddr;
    MMP_ULONG       ulPPSAddr;
    MMP_USHORT      ulSPSSize;
    MMP_USHORT      ulPPSSize;
    MMP_USHORT      ulProfileIDC;
    MMP_USHORT      ulLevelIDC;
	
} MMPS_3GPRECD_CONTAINER_INFO;

#if /*(UVC_VIDRECD_SUPPORT)||*/(SUPPORT_USB_HOST_FUNC)
#define MMPS_3GPRECD_UVC_CHIP_STR_LEN   (8)//sync (MMPF_USBH_UVC_CHIP_STR_LEN)
#define MMPS_3GPRECD_UVC_CFG_MAX_NUM    (5)//sync (MMPF_USBH_DEV_CFG_MAX_NUM)

/// Sync MMPF_USBH_UVC_H264_NALU_CFG
typedef __packed struct _MMPS_3GPRECD_UVC_NALU_CFG 
{
    MMP_UBYTE   ubSpsOfst;          //from *UVC_HDR to SPS_TYPE_HDR(0x67)
    MMP_UBYTE   ubPpsOfst;          //from *UVC_HDR to PPS_TYPE_HDR(0x68)
    MMP_UBYTE   ubSpsSize;          //size of SPS_TYPE w/o startcode
    MMP_UBYTE   ubPpsSize;          //size of PPS_TYPE w/o startcode
    MMP_UBYTE   ubIFAddNaluSize;    //size of additional nalu w/o startcode in IDR frame
    MMP_UBYTE   ubPFAddNaluSize;    //size of additional nalu w/o startcode in NIDR frame
    MMP_UBYTE   ubIFOfst;           //from *PAYLOAD_DATA(1st startcode) to IDR_TYPE_HDR(0x65)
    MMP_UBYTE   ubPFOfst;           //from *PAYLOAD_DATA(1st startcode) to NIDR_TYPE_HDR(0x41)
} MMPS_3GPRECD_UVC_NALU_CFG ;
#endif

/// AHC Parameters
typedef struct _MMPS_3GPRECD_AHC_PREVIEW_INFO 
{
	MMP_BOOL                bUserDefine;
	MMP_BOOL             	bPreviewRotate;
	MMP_DISPLAY_ROTATE_TYPE	sPreviewDmaDir;
	MMP_SCAL_FIT_MODE		sFitMode;
	MMP_ULONG               ulPreviewBufW;
	MMP_ULONG              	ulPreviewBufH;
	MMP_ULONG               ulDispStartX;
	MMP_ULONG               ulDispStartY;
	MMP_ULONG               ulDispWidth;
	MMP_ULONG               ulDispHeight;
    MMP_DISPLAY_COLORMODE 	sDispColor;
} MMPS_3GPRECD_AHC_PREVIEW_INFO;

typedef struct _MMPS_3GPRECD_AHC_VIDEO_INFO 
{
	MMP_BOOL      			bUserDefine;
	MMP_SCAL_FIT_MODE		sFitMode;
	MMP_ULONG               ulVideoEncW;
	MMP_ULONG               ulVideoEncH;
} MMPS_3GPRECD_AHC_VIDEO_INFO;

//===============================================================================
//
//                               FUNCTION PROTOTYPES
//
//===============================================================================

/** @addtogroup MMPS_3GPRECD
@{
*/

MMPS_3GPRECD_CONFIGS* MMPS_3GPRECD_GetConfig(void);

/* Preview Function */
MMP_ERR MMPS_3GPRECD_CustomedPreviewAttr(MMP_UBYTE  ubSnrSel,
                                         MMP_BOOL 	bUserConfig,
										 MMP_BOOL 	bRotate,
										 MMP_UBYTE 	ubRotateDir,
										 MMP_UBYTE	sFitMode,
										 MMP_USHORT usBufWidth, MMP_USHORT usBufHeight, 
										 MMP_USHORT usStartX, 	MMP_USHORT usStartY,
                                      	 MMP_USHORT usWinWidth, MMP_USHORT usWinHeight);
MMP_ERR MMPS_3GPRECD_EnablePreviewPipe(MMP_UBYTE ubSnrSel, MMP_BOOL bEnable, MMP_BOOL bCheckFrameEnd);
MMP_ERR MMPS_3GPRECD_PreviewStop(MMP_UBYTE ubSnrSel);
MMP_ERR MMPS_3GPRECD_PreviewStart(MMP_UBYTE ubSnrSel, MMP_BOOL bCheckFrameEnd);
MMP_ERR MMPS_3GPRECD_SetPreviewMode(MMP_USHORT usPreviewMode);
MMP_USHORT MMPS_3GPRECD_GetPreviewMode(void);
MMP_ULONG MMPS_3GPRECD_GetPreviewEndAddr(void);
MMP_ERR MMPS_3GPRECD_GetPreviewPipe(MMP_UBYTE ubSnrSel, MMP_IBC_PIPEID *pPipe);
MMP_ERR MMPS_3GPRECD_GetPreviewSize(MMP_UBYTE ubSnrSel, MMP_ULONG *width, MMP_ULONG *height);
MMP_ERR MMPS_3GPRECD_GetPreviewPipeStatus(MMP_UBYTE ubSnrSel, MMP_BOOL *bEnable);

/* 2nd Preview Function */
MMP_ERR MMPS_3GPRECD_2ndSnrPreviewStop(MMP_UBYTE ubSnrSel);
MMP_ERR MMPS_3GPRECD_2ndSnrPreviewStart(MMP_UBYTE ubSnrSel, MMP_BOOL bCheckFrameEnd);

/* Record Function */
MMP_ERR MMPS_3GPRECD_GetRecordPipeBufWidth_Height(MMP_UBYTE ubEncIdx, MMP_ULONG *pulBufW, MMP_ULONG *pulBufH);
MMP_ERR MMPS_3GPRECD_GetRecordPipe(MMP_UBYTE ubEncIdx, MMP_IBC_PIPEID *pPipe);
MMP_ERR MMPS_3GPRECD_GetStillCaptureAddr(MMP_ULONG *pulSramAddr, MMP_ULONG *pulDramAddr);
MMP_ERR MMPS_3GPRECD_SetDecMjpegToEncodeAttr(MMP_UBYTE sFitMode, 
                                             MMP_USHORT usSrcWidth, MMP_USHORT usSrcHeight, 
                                             MMP_USHORT usEncWidth, MMP_USHORT usEncHeight);

MMP_ERR MMPS_3GPRECD_CustomedEncResol(MMP_UBYTE ubEncIdx, MMP_UBYTE sFitMode, MMP_USHORT usWidth, MMP_USHORT usHeight);
MMP_ERR MMPS_3GPRECD_EnableRecordPipe(MMP_BOOL bEnable);
MMP_ERR MMPS_3GPRECD_GetRecordPipeStatus(MMP_UBYTE ubEncIdx, MMP_BOOL *bEnable);
MMP_ERR MMPS_3GPRECD_SetFileName(MMP_BYTE bFileName[], MMP_USHORT usLength, MMP_3GPRECD_FILETYPE filetype);
MMP_ERR MMPS_3GPRECD_SetUserDataAtom(MMP_BYTE AtomName[], MMP_BYTE UserDataBuf[], MMP_USHORT UserDataLength, MMP_3GPRECD_FILETYPE filetype);
MMP_ERR MMPS_3GPRECD_SetStoragePath(MMPS_3GPRECD_SRCMODE SrcMode);
MMP_ERR MMPS_3GPRECD_SetReservedStorageSpace(MMP_ULONG ulReservedSize);
MMP_ERR MMPS_3GPRECD_SetFileSizeLimit(MMP_ULONG ulFileLimit);
MMP_ERR MMPS_3GPRECD_SetFileTimeLimit(MMP_ULONG ulTimeLimitMs);
MMP_ERR MMPS_3GPRECD_Set3GPCreateModifyTimeInfo(MMP_3GPRECD_FILETYPE filetype, AUTL_DATETIME datetimenew);
MMP_ERR MMPS_3GPRECD_ChangeCurFileTimeLimit(MMP_ULONG ulTimeLimitMs);
MMP_ULONG MMPS_3GPRECD_GetMaxPreEncDuration(void);
MMP_ERR MMPS_3GPRECD_PreRecord(MMP_ULONG ulPreCaptureMs);
MMP_ERR MMPS_3GPRECD_StartRecord(void);
MMP_ERR MMPS_3GPRECD_StopRecord(void);
MMP_ERR MMPS_3GPRECD_PauseRecord(void);
MMP_ERR MMPS_3GPRECD_ResumeRecord(void);
MMP_ERR MMPS_3GPRECD_Get3gpRecordingSize(MMP_ULONG *ulSize);
MMP_ERR MMPS_3GPRECD_Get3gpFileSize(MMP_ULONG *ulfilesize);
MMP_ERR MMPS_3GPRECD_GetParameter(MMPS_3GPRECD_PARAMETER type, MMP_ULONG *ulValue);
MMP_ERR MMPS_3GPRECD_UpdateParameter(MMPS_3GPRECD_PARAMETER type, void *param);
MMP_LONG MMPS_3GPRECD_GetExpectedRecordTime(MMP_ULONG64 ullSpace, MMP_ULONG ulVidBitRate, MMP_ULONG ulAudBitRate);
MMP_ERR MMPS_3GPRECD_GetRecordTime(MMP_ULONG *ulTime);
MMP_ERR MMPS_3GPRECD_GetRecordDuration(MMP_ULONG *ulTime);
MMP_ERR MMPS_3GPRECD_SetEncodeMode(void);
MMP_ERR MMPS_3GPRECD_SetFrameRatePara(MMPS_3GPRECD_FRAMERATE *snr_fps,
                                      MMPS_3GPRECD_FRAMERATE *enc_fps,
                                      MMPS_3GPRECD_FRAMERATE *container_fps);
MMP_ERR MMPS_3GPRECD_SetEncResIdx(MMP_USHORT usResol);
MMP_ERR MMPS_3GPRECD_SetCropping (MMP_ULONG ulEncId, MMP_USHORT usTop, MMP_USHORT usBottom, MMP_USHORT usLeft, MMP_USHORT usRight);
MMP_ERR MMPS_3GPRECD_SetPadding (MMP_ULONG ulEncId, MMP_USHORT usType, MMP_USHORT usCnt);
MMP_ERR MMPS_3GPRECD_ModifyAVIListAtom(MMP_BOOL bEnable, MMP_BYTE *pStr);
MMP_ERR MMPS_3GPRECD_GetStillCaptureAddr(MMP_ULONG *pulSramAddr, MMP_ULONG *pulDramAddr);
MMP_ERR MMPS_3GPRECD_SetBitrate(MMP_ULONG ulBitrate);
MMP_ERR MMPS_3GPRECD_SetVideoFormat(MMPS_3GPRECD_VIDEO_FORMAT Format);
MMP_ERR MMPS_3GPRECD_SetCurBufMode(MMPS_3GPRECD_CURBUF_MODE CurBufMode);
MMP_ERR MMPS_3GPRECD_SetAudioFormat(MMPS_3GPRECD_AUDIO_FORMAT Format, MMPS_3GPRECD_AUDIO_OPTION Option);
MMP_ERR MMPS_3GPRECD_SetAudioRecMode(MMPS_3GPRECD_AUDIO_DATATYPE mode);
MMP_ERR MMPS_3GPRECD_RecordStatus(MMPS_FW_OPERATION *retstatus);
MMP_ULONG MMPS_3GPRECD_GetContainerTailBufSize(void);
MMP_ERR MMPS_3GPRECD_GetMergerStatus(MMP_ERR *retstatus, MMP_ULONG *txstatus);
MMP_ERR MMPS_3GPRECD_SetPFrameCount(MMP_USHORT usFrameCount);
MMP_ERR MMPS_3GPRECD_SetBFrameCount(MMP_USHORT usFrameCount);
MMP_USHORT MMPS_3GPRECD_GetBFrameCount(void);
MMP_ERR MMPS_3GPRECD_SetProfile(MMPS_3GPRECD_PROFILE profile);
MMP_ERR MMPS_3GPRECD_PreAllocateMemory(void);
MMPS_3GPRECD_CONFIGS* MMPS_3GPRECD_GetConfig(void);

MMP_ERR MMPS_3GPRECD_GetFrame(MMP_USHORT usTargetWidth, MMP_USHORT usTargetHeight,
                              MMP_USHORT *pOutBuf, MMP_ULONG *ulSize);
MMP_ERR MMPS_3GPRECD_SetRecordSpeed(MMPS_3GPRECD_SPEED_MODE ubSpeedMode, MMPS_3GPRECD_SPEED_RATIO ubSpeedRatio);
MMP_ERR MMPS_3GPRECD_StartSeamless(MMP_BOOL bStart);
MMP_ERR MMPS_3GPRECD_SetStillCaptureMaxRes(MMP_USHORT usJpegW, MMP_USHORT usJpegH);
MMP_ERR MMPS_3GPRECD_StillCapture(MMPS_3GPRECD_STILL_CAPTURE_INFO *pCaptureInfo);
MMP_ERR MMPS_3GPRECD_SetContainerType(MMPS_3GPRECD_CONTAINER type);
MMP_ERR MMPS_3GPRECD_SetQPBoundary(MMP_ULONG ulLowerBound,MMP_ULONG ulUpperBound);
MMP_ERR MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT Event, void *CallBack);
MMP_ERR MMPS_3GPRECD_SetSkipCntThreshold(MMP_USHORT threshold);
MMP_ERR MMPS_3GPRECD_SetVidRecdSkipModeParam(MMP_ULONG ulTotalCount, MMP_ULONG ulContinCount);
MMP_ERR MMPS_3GPRECD_Get3gpRecordingOffset(MMP_ULONG *ulTime);
MMP_ERR MMPS_3GPRECD_StartAllRecord(void);
MMP_ERR MMPS_3GPRECD_GetAllEncPreRecordTime(MMP_ULONG ulPreCaptureMs, MMP_ULONG *ulRealPreCaptureMs);
#if (SUPPORT_VUI_INFO)
MMP_ERR MMPS_3GPRECD_SetSEIShutterMode(MMPS_SEI_SHUTTER_TYPE ulMode);
#endif
MMP_ERR MMPS_3GPRECD_SetTime2FlushFSCache(MMP_ULONG time);
#if (VIDRECD_REFIXRECD_SUPPORT == 1)
MMP_ERR MMPS_3GPRECD_CheckFile2Refix(void);
#endif
#if (SUPPORT_VR_THUMBNAIL)
MMP_ERR MMPS_3GPRECD_TakeThumbnail2Chunk(MMPS_3GPRECD_STILL_CAPTURE_INFO *pCaptureInfo);
#endif

/* Zoom Function */
MMP_ERR 	MMPS_3GPRECD_SetPreviewZoom(MMPS_3GPRECD_ZOOM_PATH sPath, MMP_PTZ_ZOOM_DIR sZoomDir, MMP_USHORT usCurZoomStep);
MMP_ERR 	MMPS_3GPRECD_GetCurZoomStep(MMP_UBYTE ubPipe, MMP_USHORT *usZoomStepNum);
MMP_ERR 	MMPS_3GPRECD_SetZoomConfig(MMP_USHORT usMaxSteps, MMP_USHORT usMaxRatio);
MMP_UBYTE 	MMPS_3GPRECD_GetCurZoomStatus(void);

/* LDC Function */
MMP_ERR MMPS_3GPRECD_SetLdcRunMode(MMP_UBYTE ubRunMode);
MMP_ERR MMPS_3GPRECD_SetLdcResMode(MMP_UBYTE ubResMode, MMP_UBYTE ubFpsMode);
MMP_ERR MMPS_3GPRECD_GetLdcMaxRes(MMP_ULONG *pulMaxW, MMP_ULONG *pulMaxH);

/* Sticker Function */
MMP_ERR MMPS_3GPRECD_SetSticker(MMP_STICKER_ATTR *pStickerAtrribute);
MMP_ERR MMPS_3GPRECD_EnableSticker(MMP_STICKER_ID stickerID, MMP_BOOL bEnable);

/* FDTC Function */
#if (SUPPORT_FDTC)
MMP_ERR MMPS_3GPRECD_StartFDTC(MMP_BOOL bStart);
#endif

/* TimeLapse Function */
#if (SUPPORT_TIMELAPSE)
MMP_ERR MMPS_3GPRECD_InitAVIFile(MMP_BYTE *bAviName, MMP_ULONG ulNameSize, MMP_ULONG ulWidth, MMP_ULONG ulHeight,
                                 MMP_ULONG CoedcFourCC, MMP_ULONG FrameRate, MMP_ULONG ulBitRate, MMP_BOOL bInit, MMP_ULONG *FileID);
MMP_ERR MMPS_3GPRECD_AVIAppendFrame(MMP_ULONG ulFID, MMP_UBYTE *pData, MMP_ULONG ulSize, MMP_ULONG64 *ulFileSize, MMP_ULONG *ulFrameNum);
#endif

/* Emergency Record Function */
MMP_ERR MMPS_3GPRECD_SetEmergFileName(MMP_BYTE bFileName[], MMP_USHORT usLength);
MMP_ERR MMPS_3GPRECD_EnableEmergentRecd(MMP_BOOL bEnable);
MMP_ERR MMPS_3GPRECD_StartEmergentRecd(MMP_BOOL bStopVidRecd);
MMP_ERR MMPS_3GPRECD_StopEmergentRecd(MMP_BOOL bBlocking);
MMP_ERR MMPS_3GPRECD_SetEmergentFileTimeLimit(MMP_ULONG ulTimeLimitMs);
MMP_ERR MMPS_3GPRECD_SetEmergPreEncTimeLimit(MMP_ULONG ulTimeLimitMs);
MMP_ERR MMPS_3GPRECD_GetEmergentRecordingTime(MMP_ULONG *ulTime);
MMP_ERR MMPS_3GPRECD_GetEmergentRecordingOffset(MMP_ULONG *ulTime);
#if (UVC_EMERGRECD_SUPPORT)
MMP_ERR MMPS_3GPRECD_EnableUVCEmergentRecd(MMP_BOOL bEnable);
#endif

/* MultiStream Record Function */
#if (DUALENC_SUPPORT)
MMP_ERR MMPS_3GPRECD_SetDualH264SnrId(MMP_UBYTE ubSnrSel);
MMP_ERR MMPS_3GPRECD_GetDualH264Status(MMPS_FW_OPERATION *retstatus);
MMP_ERR MMPS_3GPRECD_SetDualH264Enable(MMP_BOOL bEnable, MMP_ULONG type);
MMP_ERR MMPS_3GPRECD_SetDualEncResIdx(MMP_USHORT usResol);
MMP_ERR MMPS_3GPRECD_SetDualProfile(MMPS_3GPRECD_PROFILE profile);
MMP_ERR MMPS_3GPRECD_SetDualCurBufMode(MMPS_3GPRECD_CURBUF_MODE CurBufMode);
MMP_ERR MMPS_3GPRECD_SetDualFrameRatePara(MMPS_3GPRECD_FRAMERATE *snr_fps,
                                  	  	  MMPS_3GPRECD_FRAMERATE *enc_fps,
										  MMPS_3GPRECD_FRAMERATE *container_fps);
MMP_ERR MMPS_3GPRECD_SetDualH264Bitrate(MMP_ULONG ulBitrate);
MMP_ERR MMPS_3GPRECD_SetDualPFrameCount(MMP_USHORT usFrameCount);
MMP_ERR MMPS_3GPRECD_SetDualBFrameCount(MMP_USHORT usFrameCount);
MMP_ERR MMPS_3GPRECD_EnableDualH264Pipe(MMP_UBYTE ubSnrSel, MMP_BOOL bEnable);
MMP_ERR MMPS_3GPRECD_DualEncPreRecord(MMP_ULONG ulPreCaptureMs);
MMP_ERR MMPS_3GPRECD_StartDualH264(void);
MMP_ERR MMPS_3GPRECD_StopDualH264(void);
#endif

/* UVC Record Function */
MMP_ERR MMPS_3GPRECD_UVCSetCustomedPrevwAttr(MMP_BOOL 	bUserConfig,
												MMP_BOOL 	bRotate,
												MMP_UBYTE 	ubRotateDir,
												MMP_UBYTE	sFitMode,
												MMP_USHORT usBufWidth, MMP_USHORT usBufHeight, 
												MMP_USHORT usStartX, 	MMP_USHORT usStartY,
												MMP_USHORT usWinWidth, MMP_USHORT usWinHeight);
MMP_ERR MMPS_3GPRECD_UVCGetCustomedPrevwAttr(MMP_BOOL 	*pbUserConfig,
												MMP_BOOL 	*pbRotate,
												MMP_UBYTE 	*pubRotateDir,
												MMP_UBYTE	*psFitMode,
												MMP_USHORT *pusBufWidth, MMP_USHORT*pusBufHeight, 
												MMP_USHORT *pusStartX, 	MMP_USHORT *pusStartY,
												MMP_USHORT *pusWinWidth, MMP_USHORT *pusWinHeight);
#if (UVC_VIDRECD_SUPPORT)
MMP_ERR MMPS_3GPRECD_SetUVCRecdSupport(MMP_BOOL bSupport);
MMP_ERR MMPS_3GPRECD_GetUVCRecdSupport(MMP_BOOL *bSupport);
MMP_ERR MMPS_3GPRECD_StartUVCRecd(MMP_UBYTE type);
MMP_ERR MMPS_3GPRECD_OpenUVCRecdFile(void);
MMP_ERR MMPS_3GPRECD_EnableUVCRecd(void);
MMP_ERR MMPS_3GPRECD_StopUVCRecd(void);
MMP_ERR MMPS_3GPRECD_UVCRecdInputFrame(MMP_ULONG bufaddr, MMP_ULONG size, MMP_ULONG timestamp,
									   MMP_USHORT frametype, MMP_USHORT vidaudtype);
MMP_ERR MMPS_3GPRECD_SetUVCRecdInfo(MMPS_3GPRECD_CONTAINER_INFO *pInfo);
#endif
#if (SUPPORT_USB_HOST_FUNC)
MMP_ERR MMPS_3GPRECD_StartUVCPrevw(void);
MMP_ERR MMPS_3GPRECD_StopUVCPrevw(void);
MMP_ERR MMPS_3GPRECD_SetUVCPrevwWinID(MMP_UBYTE ubWinID);
MMP_ERR MMPS_3GPRECD_SetUVCPrevwRote(MMP_GRAPHICS_ROTATE_TYPE ubRoteType);
MMP_ERR MMPS_3GPRECD_SetUVCRecdResol(MMP_UBYTE ubResol);
MMP_ERR MMPS_3GPRECD_SetUVCRecdBitrate(MMP_ULONG ulBitrate);
MMP_ERR MMPS_3GPRECD_SetUVCRecdFrameRate(MMP_USHORT usTimeIncrement, MMP_USHORT usTimeIncrResol);
MMP_ERR MMPS_3GPRECD_SetUVCRecdPFrameCount(MMP_USHORT usFrameCount);
MMP_ERR MMPS_3GPRECD_SetUVCPrevwResol(MMP_USHORT usWidth, MMP_USHORT usHeight);
MMP_ERR MMPS_3GPRECD_SetUVCPrevwFrameRate(MMP_UBYTE ubFps);
MMP_ERR MMPS_3GPRECD_AddDevCFG(MMP_UBYTE *pubStr, void *pOpenDevCallback, void *pStartDevCallback, void *pNaluInfo);
MMP_ERR MMPS_3GPRECD_UpdDevCFG(MMPS_UVCRECD_UPD_UVC_CFG_OP Event, MMP_UBYTE *pubStr, void *pParm);
MMP_ERR MMPS_3GPRECD_RegDevAddiCFG(MMP_ULONG ulRegTyp, MMP_UBYTE *pubStr, MMP_ULONG ulParm0, MMP_ULONG ulParm1, MMP_ULONG ulParm2, MMP_ULONG ulParm3);
MMP_ERR MMPS_3GPRECD_SetClassIfCmd(MMP_UBYTE bReq, MMP_USHORT wVal, MMP_USHORT wInd, MMP_USHORT wLen, MMP_UBYTE *UVCDataBuf);
MMP_ERR MMPS_3GPRECD_GetClassIfCmd(MMP_UBYTE bReq, MMP_USHORT wVal, MMP_USHORT wInd, MMP_USHORT wLen, MMP_ULONG *UVCDataLength, MMP_UBYTE *UVCDataBuf);
MMP_ERR MMPS_3GPRECD_SetStdIfCmd(MMP_UBYTE bReq, MMP_USHORT wVal, MMP_USHORT wInd, MMP_USHORT wLen, MMP_UBYTE *UVCDataBuf);
MMP_ERR MMPS_3GPRECD_GetStdDevCmd(MMP_UBYTE bReq, MMP_USHORT wVal, MMP_USHORT wInd, MMP_USHORT wLen, MMP_ULONG *UVCDataLength, MMP_UBYTE *UVCDataBuf);
#endif

/* Decode MJPEG to Preview/Encode Function */
MMP_UBYTE MMPS_3GPRECD_GetDecMjpegToPreviewPipeId(void);
MMP_ERR MMPS_3GPRECD_GetDecMjpegToPreviewSrcAttr(MMP_USHORT *pusW, MMP_USHORT *pusH);
MMP_ERR MMPS_3GPRECD_SetDecMjpegToPreviewSrcAttr(MMP_USHORT usSrcW, MMP_USHORT usSrcH);
MMP_ERR MMPS_3GPRECD_GetDecMjpegToPreviewDispAttr(  MMP_USHORT *pusDispWinId,
                                                    MMP_USHORT *pusWinOfstX,  MMP_USHORT *pusWinOfstY,
                                                    MMP_USHORT *pusWinWidth,  MMP_USHORT *pusWinHeight,
                                                    MMP_USHORT *pusDispWidth, MMP_USHORT *pusDispHeight,                                              
                                                    MMP_USHORT *pusDispColor);
MMP_ERR MMPS_3GPRECD_SetDecMjpegToPreviewDispAttr(  MMP_USHORT usDispWinId,
            									    MMP_BOOL   bRotate,
            									    MMP_UBYTE  ubRotateDir,
            									    MMP_UBYTE  sFitMode,
                                                    MMP_USHORT usWinOfstX,  MMP_USHORT usWinOfstY,
                                                    MMP_USHORT usWinWidth,  MMP_USHORT usWinHeight,
                                                    MMP_USHORT usDispWidth, MMP_USHORT usDispHeight,
                                                    MMP_USHORT usDispColor);
MMP_ERR MMPS_3GPRECD_SetDecMjpegToPreviewBuf(   MMP_USHORT                      usPreviewW,  
                                                MMP_USHORT                      usPreviewH, 
                                                MMP_ULONG                       ulAddr, 
                                                MMP_ULONG                       *pulSize,
                                                MMPS_3GPRECD_PREVIEW_BUFINFO    *pPreviewBuf);
MMP_ERR MMPS_3GPRECD_InitDecMjpegToPreview(MMP_USHORT usJpegSrcW, MMP_USHORT usJpegSrcH);
MMP_ERR MMPS_3GPRECD_DecMjpegPreviewStart(MMP_UBYTE ubSnrSel);
MMP_ERR MMPS_3GPRECD_DecMjpegPreviewStop(MMP_UBYTE ubSnrSel);

/* Set Color format for Jpeg Attribute */
MMP_ERR MMPS_3GPRECD_SetDColorformatToJpgAttr(MMP_UBYTE ubColorformat);

/* Wifi Function */
#if defined(ALL_FW)
MMP_ERR MMPS_MJPEG_OpenMjpeg(MMP_USHORT usEncID, MMP_USHORT usMode, MMP_MJPEG_OBJ_PTR *ppHandle);
MMP_ERR MMPS_MJPEG_StartFrontCamMjpeg(  MMP_UBYTE               ubSnrSel,
                                        MMP_UBYTE               ubMode,
                                        MMP_MJPEG_OBJ_PTR       pHandle,
								        MMP_MJPEG_ENC_INFO      *pMjpegInfo,
                                        MMP_MJPEG_RATE_CTL      *pRateCtrl);
MMP_ERR MMPS_MJPEG_StartRearCamMjpeg(   MMP_UBYTE               ubMode,
                                        MMP_MJPEG_OBJ_PTR       pHandle,
							            MMP_MJPEG_ENC_INFO      *pMjpegInfo,
                                        MMP_MJPEG_RATE_CTL      *pRateCtrl);
MMP_ERR MMPS_MJPEG_StopMjpeg(MMP_MJPEG_OBJ_PTR pHandle);
MMP_ERR MMPS_MJPEG_CloseMjpeg(MMP_MJPEG_OBJ_PTR* ppHandle);
MMP_ERR MMPS_MJPEG_Return2Display(MMP_MJPEG_OBJ_PTR pHandle);
MMP_ERR MMPS_MJPEG_SetCaptureAddr(MMP_ULONG ulDramAddr, MMP_ULONG ulSramAddr);
MMP_ERR MMPS_MJPEG_GetCaptureAddr(MMP_ULONG *pulDram, MMP_ULONG *pulSram);
#endif

MMP_ERR MMPS_3GPRECD_SetYFrameType(MMPS_3GPRECD_Y_FRAME_TYPE enType);

#if (MGR_SUPPORT_AVI == 1)
#if (AVI_IDIT_CHUNK_EN == 1)
unsigned int MMPS_3GPMUX_Build_IDIT(void **ptr);
#endif
#endif

MMP_ERR MMPS_3GPRECD_SetUVCFBMemory(void);
MMP_ERR MMPS_3GPRECD_SetUVCRearMJPGMemory(void);

/// @}

#endif //  _MMPS_3GPRECD_H_


/**
 @file AHC_Capture.h
 @brief Header File for the AHC capture API.
 @author 
 @version 1.0
*/

#ifndef _AHC_CAPTURE_H_
#define _AHC_CAPTURE_H_

/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "AHC_Common.h"
#include "AHC_Config_SDK.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/ 

// For Exif Config
#define	EF_MAKER_LEN					(32)
#define	EF_MODEL_LEN					(32)
#define	EF_SOFTW_LEN					(32)
#define	EF_ARTIST_LEN					(16)
#define	EF_COPYR_LEN					(16)

// Debug after JPG, Test vesion debug only!
#define ENABLE_DEBUG_AFTER_JPG      	(0)

/*===========================================================================
 * Structure define
 *===========================================================================*/ 

typedef struct _IMG_ATTRIB {
	UINT16 ImageHSize;
	UINT16 ImageVSize;	
	UINT16 ThumbHSize;
	UINT16 ThumbVSize;	
} IMG_ATTRIB;

typedef struct _EXIFINFO {
	char efMaker[EF_MAKER_LEN];
	char efModel[EF_MODEL_LEN];
	char efSoftw[EF_SOFTW_LEN];
	char efArtist[EF_ARTIST_LEN];
	char efCopyr[EF_COPYR_LEN];
} EXIFINFO;

typedef struct _AHC_EXIF_CONFIG {
    UINT32  AtomSize;   //AtomSize should be ALIGN32(DataSize + 18)
    UINT32  DataSize;
    UINT32  TagID;
    UINT32  IfdID;
    UINT32  TagType;
    UINT32  TagDataSize;
    UINT8   *Data;
} AHC_EXIF_CONFIG;

/*===========================================================================
 * Enum define
 *===========================================================================*/ 

typedef enum _AHC_CAPTURE_CONFIG {
	ACC_DATE_STAMP,
	ACC_THUMB_SIZE,
	ACC_CUS_THUMB_W,
	ACC_CUS_THUMB_H,
	ACC_CAPTURE_RAW_WITH_JPEG
} AHC_CAPTURE_CONFIG;

typedef enum _AHC_ACC_THUMB_SIZE {
	THUMB_TYPE_DISABLE = 0,
	THUMB_TYPE_QVGA,
	THUMB_TYPE_VGA,
	THUMB_TYPE_CUSTOMIZE
} AHC_ACC_THUMB_SIZE;

typedef enum _AHC_CUSTOMER_QT {
	CUSTOM_QT_OFF = 0,
	CUSTOM_QT_ALL,
	CUSTOM_QT_MAIN,
	CUSTOM_QT_THUMB
} AHC_CUSTOMER_QT;

typedef enum _AHC_CAPTURE_MODE {
    STILL_CAPTURE_MODE,
    STILL_SET_ONLY_MODE,
    VIDEO_CAPTURE_MODE
} AHC_CAPTURE_MODE;

typedef enum _AHC_ACC_TIMESTAMP_CONFIG {
	AHC_ACC_TIMESTAMP_DISABLE       = 0x0,
	AHC_ACC_TIMESTAMP_ENABLE        = 0x1,	
	AHC_ACC_TIMESTAMP_DATE_ONLY     = (0x0 << 1),
	AHC_ACC_TIMESTAMP_TIME_ONLY     = (0x1 << 1),
	AHC_ACC_TIMESTAMP_DATE_AND_TIME = (0x2 << 1),
	AHC_ACC_TIMESTAMP_BOTTOM_RIGHT  = (0x0 << 3),
	AHC_ACC_TIMESTAMP_BOTTOM_LEFT   = (0x1 << 3),
	AHC_ACC_TIMESTAMP_TOP_RIGHT     = (0x2 << 3),
	AHC_ACC_TIMESTAMP_TOP_LEFT      = (0x3 << 3),
	AHC_ACC_TIMESTAMP_FORMAT_1      = (0x0 << 5),
	AHC_ACC_TIMESTAMP_FORMAT_2      = (0x1 << 5),
	AHC_ACC_TIMESTAMP_FORMAT_3      = (0x2 << 5),
	AHC_ACC_TIMESTAMP_FORMAT_4      = (0x3 << 5),
	AHC_ACC_TIMESTAMP_LOGOEX		= (0x1 << 7),
	AHC_ACC_TIMESTAMP_DATEEX		= (0x1 << 8)
} AHC_ACC_TIMESTAMP_CONFIG;

typedef enum _AHC_ACC_TIMESTAMP_CONFIG_MASK {
	AHC_ACC_TIMESTAMP_ENABLE_MASK   = 0x1,
	AHC_ACC_TIMESTAMP_DATE_MASK     = 0x6,
	AHC_ACC_TIMESTAMP_POSITION_MASK = 0x18,
	AHC_ACC_TIMESTAMP_FORMAT_MASK   = 0x60,
	AHC_ACC_TIMESTAMP_LOGOEX_MASK	= AHC_ACC_TIMESTAMP_LOGOEX,
	AHC_ACC_TIMESTAMP_DATEEX_MASK	= AHC_ACC_TIMESTAMP_DATEEX
} AHC_ACC_TIMESTAMP_CONFIG_MASK;

/*===========================================================================
 * Function prototype
 *===========================================================================*/ 

/** @addtogroup AHC_CAPTURE
@{
*/

/* Exif Function */
AHC_BOOL 	AHC_SetImageEXIF(UINT8 *pData,UINT32 DataSize,UINT16 uwIfdIdx, UINT16 uwTagID, UINT16 uwType, UINT16 uwCount);
AHC_BOOL 	AHC_GetImageEXIF(UINT32 Index, AHC_EXIF_CONFIG *pExif);
AHC_BOOL 	AHC_OpenEXIFFileByIdx(UINT32 DcfIdx);
AHC_BOOL 	AHC_OpenEXIFFileByName(UINT8 *sFileName);
AHC_BOOL 	AHC_CloseEXIFFile(void);
AHC_BOOL 	AHC_ConfigEXIF_SystemInfo(void);
AHC_BOOL 	AHC_ConfigEXIF_MENU(void);
AHC_BOOL 	AHC_ConfigEXIF_RTC(void);

/* Capture Function */
AHC_BOOL 	AHC_ConfigCapture(AHC_CAPTURE_CONFIG byConfig, UINT16 wOp);
AHC_BOOL 	AHC_GetCaptureConfig(AHC_CAPTURE_CONFIG byConfig, UINT32 *wOp);
AHC_BOOL 	AHC_GetImageAttributes(IMG_ATTRIB *pImgAttr);
AHC_BOOL 	AHC_SetCompressionRatio(UINT16 wRatio);
UINT32 		AHC_GetStillImageTargetSize(void);
AHC_BOOL 	AHC_SetCompressionQuality(UINT32 Quality);
AHC_BOOL 	AHC_SetSeqCaptureParam(UINT8 bySeqNum, UINT8 byDelay);
AHC_BOOL 	AHC_GetSeqCaptureParam(UINT8 *bySeqNum, UINT8 *byDelay);
AHC_BOOL 	AHC_GetCaptureStatus(UINT16 *pwDramStatus, UINT16 *pwPending);
AHC_BOOL 	AHC_SetImageTime(UINT16 wReviewTime, UINT16 wWindowsTime);
AHC_BOOL 	AHC_GetImageTime(UINT16 *wReviewTime, UINT16 *wWindowsTime);
UINT16 	 	AHC_GetStillImageResIdx(void);
AHC_BOOL 	AHC_SetImageSize(UINT8 byMode, UINT16 wSizeX, UINT16 wSizeY);
AHC_BOOL 	AHC_GetImageSize(UINT8 byMode, UINT32 *wSizeX, UINT32 *wSizeY);
AHC_BOOL 	AHC_SetCustomQT(AHC_CUSTOMER_QT byFlag, UINT8 *pbyCusQT, UINT8 *pbyCusUQT, UINT8 *pbyCusVQT);
AHC_BOOL 	AHC_SetFaceDetection(UINT8 bMode);

AHC_BOOL 	AHC_GetCaptureCurZoomStep(UINT16 *usZoomStepNum);
AHC_BOOL 	AHC_GetCaptureMaxZoomStep(UINT16 *usZoomStepNum);
AHC_BOOL 	AHC_GetCaptureMaxZoomRatio(UINT16 *usZoomRatio);
AHC_BOOL 	AHC_GetCameraDigitalZoomRatio(UINT32 *usZoomRatio);
MMP_UBYTE 	AHC_GetCaptureCurZoomStatus(void);

AHC_BOOL 	AHC_SetDBGDataAfterJPEG(UINT8 *ubAddress, UINT32 *ulLength);
AHC_BOOL 	AIHC_SetCapturePreviewMode(AHC_BOOL bRawPreview, AHC_BOOL bMultiReturn);
AHC_BOOL 	AIHC_StopCapturePreviewMode(void);
AHC_BOOL 	AIHC_ConfigCaptureTimeStamp(UINT32 TimeStampOp);
AHC_BOOL	AIHC_SetCaptureMode(UINT8 uiMode);

AHC_BOOL    AHC_GetCaptureFileName(MMP_USHORT *CaptureDirKey, MMP_BYTE *pJpgFileName, MMP_UBYTE *CaptureFileName);
AHC_BOOL    AHC_CaptureAddFile(UINT16 CaptureDirKey, MMP_BYTE *CaptureFileName);

#if (ENABLE_STROBE)
AHC_ERR  	AHC_STROBE_Initialize(void);
AHC_ERR  	AHC_STROBE_EnableStrobeStatus(AHC_BOOL bEnable);
AHC_BOOL 	AHC_STROBE_GetStrobeStatus(void);
AHC_ERR  	AHC_STROBE_ChargeOn(AHC_BOOL bEnable);
AHC_BOOL 	AHC_STROBE_CheckChargeReady(void);
AHC_ERR  	AHC_STROBE_TriggerDuration(UINT32 ulBlankDuration, UINT32 ulTriggerDuration);
AHC_ERR  	AHC_STROBE_Trigger(void);
#endif

/// @}

#endif //_AHC_CAPTURE_H_
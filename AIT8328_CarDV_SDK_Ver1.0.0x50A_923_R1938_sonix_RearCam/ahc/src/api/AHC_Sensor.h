//==============================================================================
//
//  File        : AHC_Sensor.h
//  Description : INCLUDE File for the AHC sensor control function porting.
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================

#ifndef _AHC_SENSOR_H_
#define _AHC_SENSOR_H_

/*============================================================================
 * Include files
 *============================================================================*/

#include "AHC_Common.h"
#include "AHC_Capture.h"

/*============================================================================
 * Enum define
 *============================================================================*/

typedef enum _AHC_SENSOR_MODE 
{
    // Index 0
    AHC_SENSOR_MODE_VGA_30P_RESOLUTION, 		// 640*360 30P
    AHC_SENSOR_MODE_VGA_50P_RESOLUTION, 		// 640*360 50P
    AHC_SENSOR_MODE_VGA_60P_RESOLUTION, 		// 640*360 60P
    AHC_SENSOR_MODE_VGA_100P_RESOLUTION, 		// 640*360 100P
    AHC_SENSOR_MODE_VGA_120P_RESOLUTION, 		// 640*360 120P

    // Index 5
    AHC_SENSOR_MODE_HD_24P_RESOLUTION,          // 1280*720 24P
    AHC_SENSOR_MODE_HD_30P_RESOLUTION, 			// 1280*720 30P
    AHC_SENSOR_MODE_HD_50P_RESOLUTION, 			// 1280*720 50P
    AHC_SENSOR_MODE_HD_60P_RESOLUTION, 			// 1280*720 60P
    AHC_SENSOR_MODE_HD_100P_RESOLUTION, 		// 1280*720 100P

    // Index 10
    AHC_SENSOR_MODE_HD_120P_RESOLUTION, 		// 1280*720 120P
    AHC_SENSOR_MODE_FULL_HD_15P_RESOLUTION, 	// 1920*1080 15P
    AHC_SENSOR_MODE_FULL_HD_24P_RESOLUTION,     // 1920*1080 24P
    AHC_SENSOR_MODE_FULL_HD_25P_RESOLUTION, 	// 1920*1080 25P
    AHC_SENSOR_MODE_FULL_HD_30P_RESOLUTION, 	// 1920*1080 30P

    // Index 15
    AHC_SENSOR_MODE_FULL_HD_50P_RESOLUTION, 	// 1920*1080 50P
    AHC_SENSOR_MODE_FULL_HD_60P_RESOLUTION, 	// 1920*1080 60P
    AHC_SENSOR_MODE_SUPER_HD_30P_RESOLUTION, 	// 2304x1296 30P
	AHC_SENSOR_MODE_SUPER_HD_25P_RESOLUTION, 	// 2304x1296 25P
    AHC_SENSOR_MODE_SUPER_HD_24P_RESOLUTION, 	// 2304x1296 24P
	
    // Index 20
	AHC_SENSOR_MODE_1440_30P_RESOLUTION,   
    AHC_SENSOR_MODE_2D7K_15P_RESOLUTION, 		// 2704*1524 15P
    AHC_SENSOR_MODE_2D7K_30P_RESOLUTION, 		// 2704*1524 30P
    AHC_SENSOR_MODE_4K2K_15P_RESOLUTION, 		// 3840*2160 15P
    AHC_SENSOR_MODE_4K2K_30P_RESOLUTION, 		// 3840*2160 30P

    //Index 25
	AHC_SENSOR_MODE_4TO3_VGA_30P_RESOLUTION, 	// 640*480 	 30P
    AHC_SENSOR_MODE_4TO3_1D2M_30P_RESOLUTION, 	// 1280*960  30P
    AHC_SENSOR_MODE_4TO3_1D5M_30P_RESOLUTION, 	// 1440*1080 30P
    AHC_SENSOR_MODE_4TO3_3M_15P_RESOLUTION, 	// 2048*1536 15P
    AHC_SENSOR_MODE_4TO3_3M_30P_RESOLUTION, 	// 2048*1536 30P


    //Index 30
	AHC_SENSOR_MODE_4TO3_5M_15P_RESOLUTION, 	// 2560*1920 15P
    AHC_SENSOR_MODE_4TO3_5M_30P_RESOLUTION, 	// 2560*1920 30P
    AHC_SENSOR_MODE_4TO3_8M_15P_RESOLUTION, 	// 3264*2448 15P
    AHC_SENSOR_MODE_4TO3_8M_30P_RESOLUTION, 	// 3264*2448 30P
    AHC_SENSOR_MODE_4TO3_10M_15P_RESOLUTION, 	// 3648*2736 15P
    //Index 35
	AHC_SENSOR_MODE_4TO3_10M_30P_RESOLUTION, 	// 3648*2736 30P
    AHC_SENSOR_MODE_4TO3_12M_15P_RESOLUTION, 	// 4032*3024 15P
    AHC_SENSOR_MODE_4TO3_12M_30P_RESOLUTION, 	// 4032*3024 30P
    AHC_SENSOR_MODE_4TO3_14M_15P_RESOLUTION, 	// 4352*3264 15P
    AHC_SENSOR_MODE_4TO3_14M_30P_RESOLUTION, 	// 4352*3264 30P

    ///////////////////////////////////////////////////////////////////////
    AHC_SENSOR_MODE_BEST_CAMERA_PREVIEW_RESOLUTION,
    AHC_SENSOR_MODE_BEST_CAMERA_CAPTURE_4TO3_RESOLUTION,
    AHC_SENSOR_MODE_BEST_CAMERA_CAPTURE_16TO9_RESOLUTION,
    AHC_SENSOR_MODE_DEFAULT_PCCAM_RESOLUTION,

    AHC_SENSOR_MODE_PAL_25FPS,
    AHC_SENSOR_MODE_NTSC_30FPS,

    AHC_SENSOR_MODE_MAX,
    AHC_SENSOR_MODE_AUTO = 0xFFFF
} AHC_SENSOR_MODE;

typedef enum _AHC_SENSOR_DEBAND_MODE
{
	AHC_SENSOR_DSC_DEBAND_60HZ = 0,
	AHC_SENSOR_DSC_DEBAND_50HZ,
	AHC_SENSOR_VIDEO_DEBAND_60HZ,
	AHC_SENSOR_VIDEO_DEBAND_50HZ
} AHC_SENSOR_DEBAND_MODE;

typedef enum _AHC_SENSOR_FLIP_TYPE
{
	AHC_SENSOR_NO_FLIP = 0,
	AHC_SENSOR_COLUMN_FLIP,
	AHC_SENSOR_ROW_FLIP,
	AHC_SENSOR_COLROW_FLIP
} AHC_SENSOR_FLIP_TYPE;

/*============================================================================
 * Function prototype
 *============================================================================*/

AHC_BOOL 	AHC_SetPreviewExposure(UINT8 byIndex);
AHC_BOOL 	AHC_GetPreviewExposure(UINT16 *pwValue);
AHC_BOOL 	AHC_SetPreviewAgc(UINT8 byIndex);
AHC_BOOL 	AHC_GetPreviewAgc(UINT16 *pwValue);
AHC_BOOL 	AHC_SetCaptureExposure(UINT8 byIndex);
AHC_BOOL 	AHC_GetCaptureExposure(UINT16 *pwValue);
AHC_BOOL 	AHC_SetCaptureAgc(UINT8 byIndex);
AHC_BOOL 	AHC_GetCaptureAgc(UINT16 *pwValue);
AHC_BOOL 	AHC_SetPreviewExposureLimit(UINT32 buf_ptr, UINT32 DatatypeByByte, UINT32 length);
AHC_BOOL 	AHC_SetCaptureExposureLimit(UINT32 buf_ptr, UINT32 DatatypeByByte, UINT32 length);

AHC_BOOL 	AHC_PresetSensorMode(INT16 wMode);
INT16 		AHC_GetPresetSensorMode(void);
#if (SUPPORT_DUAL_SNR)
AHC_BOOL 	AHC_PresetTVDecSrcType(INT32 wTVDecSrcType);
INT32 		AHC_GetPresetTVDecSrcType(void);
#endif
AHC_BOOL 	AHC_PresetCaptureSensorMode(INT16 wMode);
INT16 		AHC_GetPresetCaptureSensorMode(void);

AHC_BOOL 	AHC_Sensor_IsInitialized(void);
AHC_BOOL 	AHC_Set_SensorFlip(AHC_SENSOR_FLIP_TYPE ubFlipType);
AHC_BOOL 	AHC_Set_SensorLightFreq(AHC_SENSOR_DEBAND_MODE smode);
AHC_BOOL 	AHC_GetScalInputWH(UINT16 wMode, UINT16* puwWidth, UINT16* puwHeight);
UINT32 		AHC_GetCurSensorFPS(void);
AHC_BOOL 	AHC_SensorPowerCtrl(AHC_BOOL bOn);
AHC_BOOL	AHC_GetSecondSensorCnntStatus(void);
AHC_BOOL 	AHC_GetSecondSensorCurrentCnntStatus(void);

#endif //_AHC_SENSOR_H_

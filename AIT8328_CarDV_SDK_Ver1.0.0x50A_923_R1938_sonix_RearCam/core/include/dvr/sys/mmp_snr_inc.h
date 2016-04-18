//==============================================================================
//
//  File        : mmp_snr_inc.h
//  Description : INCLUDE File for the Sensor Function
//  Author      : Alterman
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_SNR_INC_H_
#define _MMP_SNR_INC_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "includes_fw.h"

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

#define PRM_SENSOR           	    (0)
#define SCD_SENSOR           	    (1)
#define UNDEF_SENSOR			    (0xFF)
#define SENSOR_MAX_NUM        	    (2)

#define SENSOR_DSC_MODE 	        (1)
#define SENSOR_VIDEO_MODE 	        (2)

#define MAX_AE_CONVERGE_FRAME_NUM   (30)    // CarDV
#define MIN_AE_CONVERGE_FRAME_NUM   (9)     // CarDV

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

/* Sensor Parameter */
typedef enum _MMP_SNR_PARAM {
    MMP_SNR_PARAM_RAWPREVIEW_EN = 0,
    MMP_SNR_PARAM_ISP_FRAME_CNT,
    MMP_SNR_PARAM_CUR_PREVIEW_MODE,
    MMP_SNR_PARAM_CUR_CAPTURE_MODE,
    MMP_SNR_PARAM_CUR_FPSx10,
    MMP_SNR_PARAM_NUM
} MMP_SNR_PARAM;

/* Sensor Device Driver Parameter */
typedef enum _MMP_SNR_DRV_PARAM {
    MMP_SNR_DRV_PARAM_SCAL_IN_RES = 0,
    MMP_SNR_DRV_PARAM_PREVIEW_RES_NUM,
    MMP_SNR_DRV_PARAM_DEF_PREVIEW_RES,
    MMP_SNR_DRV_PARAM_FPSx10,
    MMP_SNR_DRV_PARAM_NUM
} MMP_SNR_DRV_PARAM;

/* Sensor Preview Mode */
typedef enum _MMP_SNR_MODE {
	MMP_SNR_DSC_PRW_MODE = 1,
	MMP_SNR_VID_PRW_MODE = 2
} MMP_SNR_MODE;

/* Sensor Priority */
typedef enum _MMP_SNR_PRIO {
	MMP_SNR_PRIO_PRM = 0,
	MMP_SNR_PRIO_SCD
} MMP_SNR_PRIO;

/* Sensor Function Mode */
typedef enum _MMP_SNR_FUNCTION_MODE {
	MMP_SNR_PREV_MODE = 0,
	MMP_SNR_CAPT_MODE
} MMP_SNR_FUNCTION_MODE;

/* Sensor Deband Mode */
typedef enum _MMP_SNR_DEBAND_MODE {
	MMP_SNR_DEBAND_DSC_60HZ = 0,
	MMP_SNR_DEBAND_DSC_50HZ,
	MMP_SNR_DEBAND_VIDEO_60HZ,
	MMP_SNR_DEBAND_VIDEO_50HZ
} MMP_SNR_DEBAND_MODE;

/* ISP IQ Function */
typedef enum _MMP_ISP_IQ_FUNC {
    MMP_ISP_IQ_FUNC_SET_SHARPNESS = 0,
    MMP_ISP_IQ_FUNC_SET_EFFECT,
    MMP_ISP_IQ_FUNC_SET_GAMMA,
    MMP_ISP_IQ_FUNC_SET_CONTRAST,
    MMP_ISP_IQ_FUNC_SET_SATURATION,
    MMP_ISP_IQ_FUNC_SET_WDR,
    MMP_ISP_IQ_FUNC_SET_HUE,
    MMP_ISP_IQ_FUNC_NUM
} MMP_ISP_IQ_FUNC;

/* ISP 3A Function */
typedef enum _MMP_ISP_3A_FUNC {
    MMP_ISP_3A_FUNC_SET_AF_ENABLE = 0,
    MMP_ISP_3A_FUNC_SET_AF_MODE,
    MMP_ISP_3A_FUNC_SET_AF_POS,
    MMP_ISP_3A_FUNC_SET_AE_ENABLE,
    MMP_ISP_3A_FUNC_SET_EV,
    MMP_ISP_3A_FUNC_SET_ISO,
    MMP_ISP_3A_FUNC_SET_AE_FAST_MODE,
    MMP_ISP_3A_FUNC_SET_AE_FLICKER_MODE,
    MMP_ISP_3A_FUNC_SET_AE_METER_MODE,
    MMP_ISP_3A_FUNC_SET_AWB_ENABLE,
    MMP_ISP_3A_FUNC_SET_AWB_MODE,
    MMP_ISP_3A_FUNC_SET_AWB_FAST_MODE,
    MMP_ISP_3A_FUNC_SET_AWB_COLOR_TEMP,
    MMP_ISP_3A_FUNC_SET_3A_ENABLE,
    MMP_ISP_3A_FUNC_NUM
} MMP_ISP_3A_FUNC;

#endif //_MMP_SNR_INC_H_
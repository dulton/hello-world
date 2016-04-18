//------------------------------------------------------------------------------
//
//  File        : ptz_cfg.c
//  Description : Source file of Pan/Tilt/Zoom configuration
//  Author      : Eroy
//  Revision    : 0.1
//
//------------------------------------------------------------------------------

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "Customer_Config.h"
#include "ptz_cfg.h"

//==============================================================================
//
//                              VARIABLES
//
//==============================================================================

/*
 * Configure of Ptz
 */
PTZ_CFG gsVidPtzCfg = {
#if (VIDEO_DIGIT_ZOOM_EN == 1)
    MMP_TRUE, 	// bEnable
#else
    MMP_FALSE, 	// bEnable
#endif
    VIDEO_DIGIT_ZOOM_RATIO_MAX,	// usMaxZoomRatio
    400,     	// usMaxZoomSteps
    200,    	// sMaxPanSteps
    -200,   	// sMinPanSteps
    200,		// sMaxTiltSteps
	-200		// sMinTiltSteps
};

PTZ_CFG gsDscPtzCfg = {
#if (DSC_DIGIT_ZOOM_ENABLE == 1)
    MMP_TRUE, 	// bEnable
#else
    MMP_FALSE, 	// bEnable
#endif
    DSC_DIGIT_ZOOM_RATIO_MAX,	// usMaxZoomRatio
    400,     	// usMaxZoomSteps
    200,    	// sMaxPanSteps
    -200,   	// sMinPanSteps
    200,		// sMaxTiltSteps
	-200		// sMinTiltSteps
};

MMP_BOOL MMP_IsVidPtzEnable(void)
{
    return gsVidPtzCfg.bEnable;
}

MMP_BOOL MMP_IsDscPtzEnable(void)
{
    return gsDscPtzCfg.bEnable;
}


/*===========================================================================
 * Include file 
 *===========================================================================*/
 
#include "AHC_Common.h"
#include "AHC_Capture.h"
#include "AHC_Video.h"
#include "ZoomControl.h"

/*===========================================================================
 * Main body
 *===========================================================================*/

//------------------------------------------------------------------------------
//  Function    : ZoomCtrl_GetVideoDigitalZoomLevel
//  Description :
//------------------------------------------------------------------------------
UINT16 ZoomCtrl_GetVideoDigitalZoomLevel(void)
{
    UINT16 usCurZoomStep = 0;
    
    AHC_GetVideoCurZoomStep(&usCurZoomStep);
    
    return usCurZoomStep;
}

//------------------------------------------------------------------------------
//  Function    : ZoomCtrl_GetVideoDigitalZoomMax
//  Description :
//------------------------------------------------------------------------------
UINT16 ZoomCtrl_GetVideoDigitalZoomMax(void)
{
    UINT16 usMaxZoomStep = 0;
    
    AHC_GetVideoMaxZoomStep(&usMaxZoomStep);
    
    return usMaxZoomStep;
}

//------------------------------------------------------------------------------
//  Function    : ZoomCtrl_GetStillDigitalZoomLevel
//  Description :
//------------------------------------------------------------------------------
UINT16 ZoomCtrl_GetStillDigitalZoomLevel(void)
{
    UINT16 usCurZoomStep;
    
    AHC_GetCaptureCurZoomStep(&usCurZoomStep);
    
    return usCurZoomStep;
}

//------------------------------------------------------------------------------
//  Function    : ZoomCtrl_GetStillDigitalZoomMax
//  Description :
//------------------------------------------------------------------------------
UINT16 ZoomCtrl_GetStillDigitalZoomMax(void)
{
	UINT16 usZoomMaxStep;
	
	AHC_GetCaptureMaxZoomStep(&usZoomMaxStep);

	return usZoomMaxStep;
}

//------------------------------------------------------------------------------
//  Function    : ZoomCtrl_DigitalZoom
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL ZoomCtrl_DigitalZoom(AHC_CAPTURE_MODE iMode, AHC_ZOOM_DIRECTION bZoomDir)
{
    return AHC_SetPreviewZoom(iMode, bZoomDir, 1);
}

//------------------------------------------------------------------------------
//  Function    : ZoomCtrl_GetDigitalZoomRatio
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL ZoomCtrl_GetDigitalZoomRatio(UINT32 *ZoomRatio)
{
	AHC_BOOL ahc_ret = AHC_TRUE;
	UINT32   CurMode;
   
	AHC_GetSystemStatus(&CurMode);
	CurMode >>= 16;
   
	if ((CurMode == AHC_MODE_CAPTURE_PREVIEW)) {
		ahc_ret = AHC_GetCameraDigitalZoomRatio(ZoomRatio);
	}
	else if((CurMode == AHC_MODE_VIDEO_RECORD)||
			(CurMode == AHC_MODE_RECORD_VONLY)||
			(CurMode == AHC_MODE_RECORD_PREVIEW)) {
		ahc_ret = AHC_GetVideoDigitalZoomRatio(ZoomRatio);
	}
   
	return ahc_ret;
}

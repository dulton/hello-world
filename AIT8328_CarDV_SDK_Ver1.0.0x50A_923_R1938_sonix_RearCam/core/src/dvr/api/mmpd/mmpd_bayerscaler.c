//------------------------------------------------------------------------------
//
//  File        : mmpd_bayerscaler.c
//  Description : Firmware Bayer Scaler Control Function
//  Author      : Eroy Yang
//  Revision    : 1.0
//
//------------------------------------------------------------------------------

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "lib_retina.h"
#include "mmpd_scaler.h"
#include "mmpd_bayerscaler.h"
#include "mmpf_bayerscaler.h"

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

//------------------------------------------------------------------------------
//  Function    : MMPD_BayerScaler_SetEngine
//  Description : This function set bayer scaler engine.
//------------------------------------------------------------------------------
/** 
 * @brief This function set bayer scaler engine.
 * 
 *  This function set bayer scaler engine.
 * @param[in]     bUserdefine  : stands for use user define range.
 * @param[in/out] fitrange     : stands for input/output range.
 * @param[in/out] grabInfo     : stands for output grab range. 
 * @return It return the function status.  
 */
MMP_ERR MMPD_BayerScaler_SetEngine(	MMP_SCAL_FIT_RANGE 	*fitrange,
                        			MMP_SCAL_GRAB_CTRL	*grabInfo)
{
    return MMPF_BayerScaler_SetEngine(fitrange, grabInfo);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_BayerScaler_SetEnable
//  Description : This function enable bayer scaler.
//------------------------------------------------------------------------------
/** 
 * @brief This function enable bayer scaler.
 * 
 *  This function enable bayer scaler.
 * @param[in]     bEnable  : stands for enable bayer scaler or not.
 * @return It return the function status.  
 */
MMP_ERR MMPD_BayerScaler_SetEnable(MMP_BOOL bEnable)
{
	return MMPF_BayerScaler_SetEnable(bEnable);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_BayerScaler_GetResolution
//  Description : This function get bayer scaler resolution.
//------------------------------------------------------------------------------
/** 
 * @brief This function get bayer scaler resolution.
 * 
 *  This function get bayer scaler resolution.
 * @param[in]     ubInOut  	: stands for bayer scaler input or output range.
 * @param[out]    pusW  	: stands for horizontal resolution.
 * @param[out]    pusH  	: stands for vertical resolution.
 * @return It return the function status.  
 */
MMP_ERR MMPD_BayerScaler_GetResolution(MMP_UBYTE ubInOut, MMP_USHORT *pusW, MMP_USHORT *pusH)
{
	return MMPF_BayerScaler_GetResolution(ubInOut, pusW, pusH);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_BayerScaler_GetZoomInfo
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_BayerScaler_GetZoomInfo(MMP_BAYER_SCALER_MODE 		nBayerMode, 
								     MMP_SCAL_FIT_RANGE 		*fitrange, 
								     MMP_SCAL_GRAB_CTRL 		*grabctl)
{
    return MMPF_BayerScaler_GetZoomInfo(nBayerMode, (MMP_SCAL_FIT_RANGE* )fitrange, (MMP_SCAL_GRAB_CTRL* )grabctl);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_BayerScaler_SetZoomInfo
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_BayerScaler_SetZoomInfo(MMP_BAYER_SCALER_MODE 	nBayerMode, 
									 MMP_SCAL_FIT_MODE 		sFitMode,
				                     MMP_ULONG ulSnrInputW, MMP_ULONG ulSnrInputH,
				                     MMP_ULONG ulFovInStX,	MMP_ULONG ulFovInStY, 
				                     MMP_ULONG ulFovInputW, MMP_ULONG ulFovInputH, 
				                     MMP_ULONG ulOutputW,   MMP_ULONG ulOutputH, 
				                     MMP_ULONG ulDummyOutX, MMP_ULONG ulDummyOutY)
{
    return MMPF_BayerScaler_SetZoomInfo(nBayerMode, 	sFitMode,
    									ulSnrInputW, 	ulSnrInputH,
    									ulFovInStX,		ulFovInStY,
    									ulFovInputW, 	ulFovInputH,
    									ulOutputW, 		ulOutputH,
    									ulDummyOutX, 	ulDummyOutY);
}
//==============================================================================
//
//  File        : mmpd_bayerscaler.h
//  Description : INCLUDE File for the Firmware Bayer Scaler Control driver function
//  Author      : Eroy Yang
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPD_BAYERSCALER_H_
#define _MMPD_BAYERSCALER_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_scal_inc.h"
#include "mmp_bayerscal_inc.h"

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

MMP_ERR MMPD_BayerScaler_SetEngine(	MMP_SCAL_FIT_RANGE 	*fitrange,
                        			MMP_SCAL_GRAB_CTRL 	*grabctl);
MMP_ERR MMPD_BayerScaler_SetEnable(MMP_BOOL bEnable);
MMP_ERR MMPD_BayerScaler_GetResolution(MMP_UBYTE ubInOut, MMP_USHORT *pusW, MMP_USHORT *pusH);

MMP_ERR MMPD_BayerScaler_GetZoomInfo(MMP_BAYER_SCALER_MODE 	nBayerMode, 
								     MMP_SCAL_FIT_RANGE 	*fitrange, 
								     MMP_SCAL_GRAB_CTRL 	*grabctl);
MMP_ERR MMPD_BayerScaler_SetZoomInfo(MMP_BAYER_SCALER_MODE 	nBayerMode, 
									 MMP_SCAL_FIT_MODE 		sFitMode,
				                     MMP_ULONG ulSnrInputW, MMP_ULONG ulSnrInputH, 
									 MMP_ULONG ulFovInStX,	MMP_ULONG ulFovInStY, 
				                     MMP_ULONG ulFovInputW, MMP_ULONG ulFovInputH, 
				                     MMP_ULONG ulOutputW,   MMP_ULONG ulOutputH, 
				                     MMP_ULONG ulDummyOutX, MMP_ULONG ulDummyOutY);

#endif // _MMPD_BAYERSCALER_H_

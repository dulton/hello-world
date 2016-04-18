/// @ait_only
//==============================================================================
//
//  File        : mmpd_scaler.h
//  Description : INCLUDE File for the Host Scaler Driver.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPD_SCALER_H_
#define _MMPD_SCALER_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_scal_inc.h"

/** @addtogroup MMPD_Scaler
 *  @{
 */

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

/* Scaler Function */
MMP_ERR MMPD_Scaler_ResetModule(MMP_SCAL_PIPEID pipeID);
MMP_ERR MMPD_Scaler_SetStopEnable(MMP_SCAL_PIPEID pipeID, MMP_UBYTE ubStopSrc, MMP_BOOL bEn);
MMP_ERR MMPD_Scaler_SetBusyMode(MMP_SCAL_PIPEID pipeID, MMP_BOOL bEn);
MMP_ULONG MMPD_Scaler_GetMaxDelayLineWidth(MMP_SCAL_PIPEID pipeID);
MMP_ERR MMPD_Scaler_BypassScaler(MMP_SCAL_PIPEID pipeID, MMP_BOOL bBypass);
MMP_ERR MMPD_Scaler_SetEngine(	MMP_SCAL_USER_DEF_TYPE	bUserdefine,
								MMP_SCAL_PIPEID			pipeID,
								MMP_SCAL_FIT_RANGE		*fitrange, 
								MMP_SCAL_GRAB_CTRL		*grabctl);
MMP_ERR MMPD_Scaler_SetLPF( MMP_SCAL_PIPEID		pipeID, 
                            MMP_SCAL_FIT_RANGE *fitrange,
                            MMP_SCAL_GRAB_CTRL *grabctl);
MMP_ERR MMPD_Scaler_SetOutColor(MMP_SCAL_PIPEID pipeID, MMP_SCAL_COLORMODE  outcolor);
MMP_ERR MMPD_Scaler_SetOutColorTransform(MMP_SCAL_PIPEID pipeID, MMP_BOOL bEnable, MMP_SCAL_COLRMTX_MODE MatrixMode);

MMP_ERR MMPD_Scaler_SetPixelDelay(MMP_SCAL_PIPEID pipeID, MMP_UBYTE ubPixelDelayN, MMP_UBYTE ubPixelDelayM);
MMP_ERR MMPD_Scaler_SetLineDelay(MMP_SCAL_PIPEID pipeID, MMP_UBYTE ubLineDelay);

MMP_ERR MMPD_Scaler_SetEnable(MMP_SCAL_PIPEID pipeID, MMP_BOOL bEnable);
MMP_ERR MMPD_Scaler_GetEnable(MMP_SCAL_PIPEID pipeID, MMP_BOOL *bEnable);

MMP_ERR MMPD_Scaler_GetPath(MMP_SCAL_PIPEID		pipeID, 
                            MMP_SCAL_SOURCE		*source);
MMP_ERR MMPD_Scaler_SetPath(MMP_SCAL_PIPEID		pipeID, 
                            MMP_SCAL_SOURCE		source, 
							MMP_BOOL			bFrameSync);
MMP_ERR MMPD_Scaler_SetSerialLinkPipe(MMP_SCAL_PIPEID srcPath, MMP_SCAL_PIPEID dstPath);

MMP_ERR MMPD_Scaler_CheckLPFAbility(MMP_SCAL_PIPEID pipeID, MMP_SCAL_LPF_ABILITY *ability);
MMP_ERR MMPD_Scaler_GetLPFDownsample(MMP_SCAL_PIPEID pipeID, MMP_SCAL_LPF_DNSAMP *downsample);
MMP_ERR MMPD_Scaler_GetLPFEnable(MMP_SCAL_PIPEID pipeID, MMP_BOOL *bEnable);
MMP_ERR MMPD_Scaler_GetGrabPosition(MMP_SCAL_PIPEID 		pipeID,
									MMP_SCAL_GRAB_STAGE 	grabstage,
									MMP_USHORT 				*usHorizontalStartPos, 
									MMP_USHORT 				*usHorizontalEndPos, 
									MMP_USHORT 				*usVerticalStartPos, 
									MMP_USHORT 				*usVerticalEndPos);
MMP_ERR MMPD_Scaler_GetScalingRatio(MMP_SCAL_PIPEID 		pipeID, 
									MMP_USHORT 				*usHorizontalN,
									MMP_USHORT 				*usHorizontalM,
									MMP_USHORT 				*usVerticalN,
									MMP_USHORT 				*usVerticalM );
MMP_ERR MMPD_Scaler_GetGrabRange(	MMP_SCAL_PIPEID 		pipeID,
									MMP_SCAL_GRAB_STAGE 	grabstage,
									MMP_USHORT 				*usHorizontal, 
									MMP_USHORT 				*usVertical);
	
MMP_ERR MMPD_Scaler_RestoreAttributes(MMP_SCAL_PIPEID pipeID);
MMP_ERR MMPD_Scaler_BackupAttributes(MMP_SCAL_PIPEID pipeID);

/* Flow Function */
MMP_ERR MMPD_Scaler_GetGCDBestFitScale(MMP_SCAL_FIT_RANGE *fitrange, MMP_SCAL_GRAB_CTRL *grabctl);
MMP_ERR MMPD_Scaler_GetBestFitScale(MMP_SCAL_FIT_RANGE *fitrange, MMP_SCAL_GRAB_CTRL *grabctl);
MMP_ERR MMPD_Scaler_TransferCoordinate(	MMP_SCAL_PIPEID 	pipeID,
										MMP_USHORT			us3rdGrabXposition, 
										MMP_USHORT 			us3rdGrabYposition, 
										MMP_USHORT 			*us1stGrabXposition, 
										MMP_USHORT 			*us1stGrabYposition);

/// @}

#endif // _MMPD_SCALER_H_

/// @end_ait_only

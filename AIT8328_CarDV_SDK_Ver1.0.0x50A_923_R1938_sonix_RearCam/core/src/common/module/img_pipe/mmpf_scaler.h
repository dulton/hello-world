//==============================================================================
//
//  File        : mmpf_scaler.h
//  Description : INCLUDE File for the Firmware Scaler Control driver function
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPF_SCALER_H_
#define _MMPF_SCALER_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_scal_inc.h"

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#define	SCALER_PATH0_MAX_WIDTH	    	(4608)
#define	SCALER_PATH1_MAX_WIDTH	        (1920)
#define	SCALER_PATH2_MAX_WIDTH	        (1280)
#define	SCALER_PATH3_MAX_WIDTH	        (1920)
#define	SCALER_PATH4_MAX_WIDTH	        (1920)
#define	LPF_MAX_WIDTH			        (SCALER_PATH0_MAX_WIDTH)

#define SCALER_ISR_EN                   (1)

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

/* Scaler Function */
MMP_USHORT Greatest_Common_Divisor(MMP_USHORT a, MMP_USHORT b);

MMP_ERR MMPF_Scaler_ResetModule(MMP_SCAL_PIPEID pipeID);
MMP_ERR MMPF_Scaler_SetStopEnable(MMP_SCAL_PIPEID pipeID, MMP_SCAL_STOP_SRC ubStopSrc, MMP_BOOL bEn);
MMP_ERR MMPF_Scaler_SetBusyMode(MMP_SCAL_PIPEID pipeID, MMP_BOOL bEn);
MMP_ULONG MMPF_Scaler_GetMaxDelayLineWidth(MMP_SCAL_PIPEID pipeID);
MMP_ERR MMPF_Scaler_RegisterIntrCallBack(MMP_SCAL_PIPEID scalpath, MMP_SCAL_EVENT event, ScalCallBackFunc *pCallBack, void *pArgument);
MMP_ERR MMPF_Scaler_EnableInterrupt(MMP_SCAL_PIPEID pipeID, MMP_SCAL_EVENT event, MMP_BOOL bEnable);
MMP_ERR MMPF_Scaler_OpenInterrupt(MMP_BOOL bEn);
MMP_ERR MMPF_Scaler_BypassScaler(MMP_SCAL_PIPEID pipeID, MMP_BOOL bBypass);
MMP_ERR MMPF_Scaler_SetEngine(MMP_SCAL_USER_DEF_TYPE	bUserdefine, 
							  MMP_SCAL_PIPEID 			pipeID,
                        	  MMP_SCAL_FIT_RANGE 		*fitrange, 
                        	  MMP_SCAL_GRAB_CTRL 		*grabctl);
MMP_ERR MMPF_Scaler_SetLPF( MMP_SCAL_PIPEID 	pipeID, 
							MMP_SCAL_FIT_RANGE 	*fitrange,
                            MMP_SCAL_GRAB_CTRL 	*grabctl);
MMP_ERR MMPF_Scaler_SetOutputFormat(MMP_SCAL_PIPEID pipeID, MMP_SCAL_COLORMODE outcolor);
MMP_ERR MMPF_Scaler_SetOutColorTransform(MMP_SCAL_PIPEID pipeID, MMP_BOOL bEnable, MMP_SCAL_COLRMTX_MODE MatrixMode);
MMP_ERR MMPF_Scaler_SetEnable(MMP_SCAL_PIPEID pipeID, MMP_BOOL bEnable);
MMP_ERR MMPF_Scaler_GetEnable(MMP_SCAL_PIPEID pipeID, MMP_BOOL* bEnable);
MMP_ERR MMPF_Scaler_SetPixelDelay(MMP_SCAL_PIPEID pipeID, MMP_UBYTE ubPixelDelayN, MMP_UBYTE ubPixelDelayM);
MMP_ERR MMPF_Scaler_SetLineDelay(MMP_SCAL_PIPEID pipeID, MMP_UBYTE ubLineDelay);
MMP_ERR MMPF_Scaler_SetEqDownModeDelay(MMP_SCAL_PIPEID pipeID, MMP_BOOL ubEnable, MMP_USHORT usThd);
MMP_ERR MMPF_Scaler_SetInputMuxAttr(MMP_SCAL_PIPEID pipeID, MMP_SCAL_IN_MUX_ATTR* pAttr);
MMP_UBYTE MMPF_Scaler_SetInputSrc(MMP_SCAL_PIPEID pipeID, MMP_SCAL_SOURCE source, MMP_BOOL bUpdate);
MMP_ERR MMPF_Scaler_GetInputSrc(MMP_SCAL_PIPEID pipeID, MMP_SCAL_SOURCE* source);
MMP_ERR MMPF_Scaler_SetPath(MMP_SCAL_PIPEID pipeID, MMP_SCAL_SOURCE source);
MMP_ERR MMPF_Scaler_ChangePath(MMP_SCAL_PIPEID pipeID);
MMP_ERR MMPF_Scaler_SetSerialLinkPipe(MMP_SCAL_PIPEID srcPath, MMP_SCAL_PIPEID dstPath);

MMP_ERR MMPF_Scaler_SetGrabPosition(MMP_SCAL_PIPEID 		pipeID,
									MMP_SCAL_GRAB_STAGE 	grabstage,
									MMP_USHORT 				usHStartPos, 
									MMP_USHORT 				usHEndPos, 
									MMP_USHORT 				usVStartPos, 
									MMP_USHORT 				usVEndPos);
MMP_ERR MMPF_Scaler_GetGrabPosition(MMP_SCAL_PIPEID pipeID, MMP_SCAL_GRAB_STAGE 	grabstage,
									MMP_USHORT *usHStartPos, MMP_USHORT *usHEndPos, 
									MMP_USHORT *usVStartPos, MMP_USHORT *usVEndPos);
MMP_ERR MMPF_Scaler_GetGrabRange(	MMP_SCAL_PIPEID 		pipeID,
									MMP_SCAL_GRAB_STAGE 	grabstage,
									MMP_USHORT 				*usWidth, 
									MMP_USHORT 				*usHeight);

MMP_ERR MMPF_Scaler_CheckLPFAbility(MMP_SCAL_PIPEID pipeID, MMP_SCAL_LPF_ABILITY *ability);
MMP_ERR MMPF_Scaler_GetLPFEnable(MMP_SCAL_PIPEID pipeID, MMP_BOOL *bEnable);
MMP_ERR MMPF_Scaler_GetLPFDownSample(MMP_SCAL_PIPEID pipeID, MMP_SCAL_LPF_DNSAMP *downsample);
MMP_ERR MMPF_Scaler_GetScalingRatio(MMP_SCAL_PIPEID 	pipeID, 
									MMP_USHORT *usHN, MMP_USHORT *usHM,
									MMP_USHORT *usVN, MMP_USHORT *usVM);

MMP_ERR MMPF_Scaler_GetAttributes(MMP_SCAL_PIPEID pipeID, MMP_SCAL_SETTING* setting);
MMP_ERR MMPF_Scaler_SetAttributes(MMP_SCAL_PIPEID pipeID, MMP_SCAL_SETTING* setting);

/* Flow Function */
MMP_ERR MMPF_Scaler_GetScaleUpHBlanking(MMP_SCAL_PIPEID pipeID, MMP_ULONG64 *pHBlanking);
MMP_ERR MMPF_Scaler_GetGCDBestFitScale(MMP_SCAL_FIT_RANGE *fitrange, MMP_SCAL_GRAB_CTRL *grabctl);
MMP_ERR MMPF_Scaler_GetBestInGrabRange(MMP_SCAL_FIT_RANGE *fitrange, MMP_SCAL_GRAB_CTRL *grabctl);
MMP_ERR MMPF_Scaler_GetBestOutGrabRange(MMP_SCAL_FIT_RANGE *fitrange, MMP_SCAL_GRAB_CTRL *grabctl);
MMP_ERR MMPF_Scaler_TransferCoordinate(MMP_SCAL_PIPEID  pipeID,
                                       MMP_USHORT       usOutGrabXpos, 
                                       MMP_USHORT       usOutGrabYpos, 
                                       MMP_USHORT       *us1stGrabXpos, 
                                       MMP_USHORT       *us1stGrabYpos);
MMP_ERR MMPF_Scaler_CheckScaleUp(MMP_SCAL_PIPEID pipeID, MMP_BOOL *bScaleup);

#ifdef SCAL_FUNC_DBG
MMP_ERR MMPF_Scaler_DumpSetting(MMP_UBYTE			*pFuncName,
								MMP_SCAL_FIT_RANGE 	*fitrange,
                                MMP_SCAL_GRAB_CTRL 	*grabctl);
#endif

#endif // _MMPF_SCALER_H_

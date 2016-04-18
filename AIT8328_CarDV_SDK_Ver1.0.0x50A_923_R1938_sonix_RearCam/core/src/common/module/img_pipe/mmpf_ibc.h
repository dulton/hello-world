/// @ait_only
//==============================================================================
//
//  File        : mmpf_ibc.h
//  Description : INCLUDE File for the Host IBC Driver.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPF_IBC_H_
#define _MMPF_IBC_H_

/** @addtogroup MMPF_IBC
 *  @{
 */

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_ibc_inc.h"

typedef void IbcCallBackFunc(void *argu);

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

MMP_ERR MMPF_IBC_Init(void);
MMP_ERR MMPF_IBC_SetH264RT_Enable(MMP_BOOL ubEn);
MMP_ERR MMPF_IBC_SetRtPingPongAddr(MMP_IBC_PIPEID pipeID, 
                                   MMP_ULONG ulYAddr,  MMP_ULONG ulUAddr, MMP_ULONG ulVAddr,
                                   MMP_ULONG ulY1Addr, MMP_ULONG ulU1Addr, MMP_ULONG ulV1Addr, 
                                   MMP_USHORT usFrmW);
MMP_ERR MMPF_IBC_SetMCI_ByteCount(MMP_IBC_PIPEID pipeID, MMP_USHORT ubByteCntSel);

MMP_ERR MMPF_IBC_ResetModule(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPF_IBC_UpdateStoreAddress(MMP_IBC_PIPEID pipeID, MMP_ULONG ulBaseAddr,
                                    MMP_ULONG ulBaseUAddr, MMP_ULONG ulBaseVAddr);
MMP_ERR MMPF_IBC_SetColorFmt(MMP_IBC_PIPEID pipeID, MMP_IBC_COLOR fmt);
MMP_ERR MMPF_IBC_GetColorFmt(MMP_IBC_PIPEID pipeID, MMP_IBC_COLOR *pFmt);
MMP_ERR MMPF_IBC_SetPartialStoreAttr(MMP_IBC_PIPEID 	pipeID,
									 MMP_IBC_PIPE_ATTR  *pAttr,
									 MMP_IBC_RECT       *pRect);
MMP_ERR MMPF_IBC_SetAttributes(MMP_IBC_PIPEID pipeID, MMP_IBC_PIPE_ATTR *pipeAttr);
MMP_ERR MMPF_IBC_GetAttributes(MMP_IBC_PIPEID pipeID, MMP_IBC_PIPE_ATTR *pipeAttr);
MMP_BOOL MMPF_IBC_IsStoreEnable(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPF_IBC_SetStoreEnable(MMP_IBC_PIPEID pipeID, MMP_BOOL bEnable);
MMP_ERR MMPF_IBC_SetSingleFrmEnable(MMP_IBC_PIPEID pipeID, MMP_BOOL bEnable);
MMP_ERR MMPF_IBC_SetPreFrameRdy(MMP_IBC_PIPEID pipeID, MMP_USHORT usLineNum, MMP_BOOL bEnable);
MMP_ERR MMPF_IBC_ClearFrameEnd(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPF_IBC_ClearFrameReady(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPF_IBC_CheckFrameEnd(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPF_IBC_CheckFrameReady(MMP_IBC_PIPEID pipeID);
MMP_BOOL MMPF_IBC_IsH264Overlap(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPF_IBC_SetMirrorEnable(MMP_IBC_PIPEID pipeID, MMP_BOOL bEnable, MMP_USHORT usWidth);
MMP_ERR MMPF_IBC_SetInterruptEnable(MMP_IBC_PIPEID pipeID, MMP_IBC_EVENT event, MMP_BOOL bEnable);
MMP_ERR MMPF_IBC_RegisterIntrCallBack(MMP_IBC_PIPEID pipeID, MMP_IBC_EVENT event, IbcCallBackFunc *pCallBack, void *pArgu);

/// @}

#endif // _MMPF_IBC_H_

/// @end_ait_only

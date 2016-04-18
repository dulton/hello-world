/// @ait_only
//==============================================================================
//
//  File        : mmpd_ibc.h
//  Description : INCLUDE File for the Host IBC Driver.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPD_IBC_H_
#define _MMPD_IBC_H_

/** @addtogroup MMPD_IBC
 *  @{
 */

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_ibc_inc.h"

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

MMP_ERR MMPD_IBC_SetH264RT_Enable(MMP_BOOL ubEn);
MMP_ERR MMPD_IBC_SetRtPingPongAddr(MMP_IBC_PIPEID pipeID, 
                                   MMP_ULONG ulYAddr,  MMP_ULONG ulUAddr, MMP_ULONG ulVAddr,
                                   MMP_ULONG ulY1Addr, MMP_ULONG ulU1Addr, MMP_ULONG ulV1Addr, 
                                   MMP_USHORT usFrmW);
MMP_ERR MMPD_IBC_SetMCI_ByteCount(MMP_IBC_PIPEID pipeID, MMP_USHORT ubByteCntSel);

MMP_ERR MMPD_IBC_ResetModule(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPD_IBC_SetColorFmt(MMP_IBC_PIPEID pipeID, MMP_IBC_COLOR fmt);
MMP_ERR MMPD_IBC_SetAttributes(MMP_IBC_PIPEID pipeID, MMP_IBC_PIPE_ATTR *pipeAttr);
MMP_ERR MMPD_IBC_GetAttributes(MMP_IBC_PIPEID pipeID, MMP_IBC_PIPE_ATTR *pipeAttr);
MMP_ERR MMPD_IBC_SetStoreEnable(MMP_IBC_PIPEID pipeID, MMP_BOOL bEnable);

MMP_ERR MMPD_IBC_ClearFrameEnd(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPD_IBC_ClearFrameReady(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPD_IBC_CheckFrameEnd(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPD_IBC_CheckFrameReady(MMP_IBC_PIPEID pipeID);

MMP_ERR MMPD_IBC_SetMirrorEnable(MMP_IBC_PIPEID pipeID, MMP_BOOL bEnable, MMP_USHORT usWidth);
MMP_ERR MMPD_IBC_SetInterruptEnable(MMP_IBC_PIPEID pipeID, MMP_IBC_EVENT event, MMP_BOOL bEnable);
MMP_ERR MMPD_IBC_RegisterIntrCallBack(MMP_IBC_PIPEID pipeID, MMP_IBC_EVENT event, void* pCallBack, void* pCallBackArgu);

/// @}

#endif // _MMPD_IBC_H_

/// @end_ait_only


//==============================================================================
//
//  File        : mmpf_icon.h
//  Description : INCLUDE File for the Firmware ICON/ICO engine control  
//  Author      : Jerry Lai
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPF_ICON_H_
#define _MMPF_ICON_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_icon_inc.h"

/** @addtogroup MMPF_Icon
 *  @{
 */

#define ICONDELAYTH     30

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
#if (SYSTEM_FLOW_MONITOR_ENABLE)
MMP_BOOL MMPF_ICON_Check(void);
void MMPF_ICON_Overflow(void);
#endif

MMP_ERR MMPF_Icon_ResetModule(MMP_ICO_PIPEID pipeID);
MMP_ERR MMPF_Icon_GetAttributes(MMP_UBYTE ubIconID, MMP_STICKER_ATTR *pBufAttr);
MMP_ERR MMPF_Icon_SetAttributes(MMP_UBYTE ubIconID, MMP_STICKER_ATTR *pBufAttr);

MMP_ERR MMPF_Icon_SetSemiTP(MMP_UBYTE ubIconID, MMP_BOOL bSemiTPEn, MMP_ULONG ulWeight);
MMP_ERR MMPF_Icon_GetSemiTP_Weight(MMP_UBYTE ubIconID, MMP_USHORT *usWeight);
MMP_ERR MMPF_Icon_SetTransparent(MMP_UBYTE ubIconID, MMP_BOOL bTranspActive, MMP_ULONG ulTranspColor);
MMP_ERR MMPF_Icon_SetEnable(MMP_UBYTE ubIconID, MMP_BOOL bEnable);
MMP_ERR MMPF_Icon_LoadIndexColorTable(MMP_UBYTE ubIconID, MMP_ICON_COLOR ubColor, 
									  MMP_USHORT* pLUT, MMP_USHORT usColorNum);

MMP_ERR MMPF_Icon_SetDLAttributes(MMP_ICO_PIPEID pipeID, MMP_ICO_PIPE_ATTR *pipeattribute);
MMP_ERR MMPF_Icon_SetDLEnable(MMP_ICO_PIPEID pipeID, MMP_BOOL bEnable);
MMP_ULONG MMPF_Icon_GetDLMaxUsage(MMP_ICO_PIPEID pipeID);
MMP_ULONG MMPF_Icon_GetDLMaxLimit(MMP_ICO_PIPEID pipeID);
MMP_ERR MMPF_Icon_InitLinkSrc(void);
MMP_ERR MMPF_Icon_BackupDLSetting(MMP_ICO_PIPEID pipeID, MMP_ICO_DL_SETTING *pIcoSetting);
MMP_ERR MMPF_Icon_RestoreDLSetting(MMP_ICO_PIPEID pipeID, MMP_ICO_DL_SETTING *pIcoSetting);


/// @}

#endif //_MMPF_ICON_H_
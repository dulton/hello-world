/// @ait_only
//==============================================================================
//
//  File        : mmpd_icon.h
//  Description : INCLUDE File for the Host Icon Driver.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

/**
 *  @file mmpd_icon.h
 *  @brief The header File for the Host Icon Driver
 *  @author Penguin Torng
 *  @version 1.0
 */

#ifndef _MMPD_ICON_H_
#define _MMPD_ICON_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_icon_inc.h"

/** @addtogroup MMPD_Icon
 *  @{
 */

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

MMP_ERR MMPD_Icon_ResetModule(MMP_ICO_PIPEID pipeID);
MMP_ERR MMPD_Icon_GetAttributes(MMP_UBYTE ubIconID, MMP_STICKER_ATTR *pBufAttr);
MMP_ERR MMPD_Icon_SetAttributes(MMP_UBYTE ubIconID, MMP_STICKER_ATTR *pBufAttr);
                                
MMP_ERR MMPD_Icon_SetSemiTP(MMP_UBYTE ubIconID, MMP_BOOL bSemiTPEn, MMP_ULONG ulWeight);
MMP_ERR MMPD_Icon_GetSemiTP_Weight(MMP_UBYTE ubIconID, MMP_USHORT *usWeight);
MMP_ERR MMPD_Icon_SetTransparent(MMP_UBYTE ubIconID, MMP_BOOL bTranspActive, MMP_ULONG ulTranspColor);
MMP_ERR MMPD_Icon_SetEnable(MMP_UBYTE ubIconID, MMP_BOOL bEnable);
MMP_ERR MMPD_Icon_LoadIndexColorTable(MMP_UBYTE ubIconID, MMP_ICON_COLOR ubColor, 
									  MMP_USHORT* pLUT, MMP_USHORT usColorNum);

MMP_ERR MMPD_Icon_SetDLAttributes(MMP_ICO_PIPEID pipeID, MMP_ICO_PIPE_ATTR *pipeattribute);                      
MMP_ERR MMPD_Icon_SetDLEnable(MMP_ICO_PIPEID pipeID, MMP_BOOL bEnable);
MMP_ERR MMPD_Icon_BackupDLSetting(MMP_ICO_PIPEID pipeID);
MMP_ERR MMPD_Icon_RestoreDLSetting(MMP_ICO_PIPEID pipeID);
MMP_ERR MMPD_Icon_InitLinkSrc(void);

/// @}

#endif // _MMPD_ICON_H_

/// @end_ait_only


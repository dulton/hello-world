//==============================================================================
//
//  File        : mmp_icon_inc.h
//  Description : INCLUDE File for the Firmware ICON engine control  
//  Author      : Jerry Lai
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_ICON_INC_H_
#define _MMP_ICON_INC_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmpf_scaler.h"

/** @addtogroup MMPF_Icon
 *  @{
 */

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

typedef enum _MMP_ICON_COLOR
{
    MMP_ICON_COLOR_INDEX8 = 0,
    MMP_ICON_COLOR_RGB565,
    MMP_ICON_COLOR_INDEX1,
    MMP_ICON_COLOR_INDEX2,
    MMP_ICON_COLOR_NUM
} MMP_ICON_COLOR;

typedef enum _MMP_ICO_PIPEID
{
    MMP_ICO_PIPE_0 = 0,
    MMP_ICO_PIPE_1,
    MMP_ICO_PIPE_2,
    MMP_ICO_PIPE_3,
    MMP_ICO_PIPE_4,
    MMP_ICO_PIPE_NUM
} MMP_ICO_PIPEID;

typedef enum _MMP_STICKER_ID 
{
	MMP_STICKER_PIPE0_ID0 = 0,
	MMP_STICKER_PIPE0_ID1,	
	MMP_STICKER_PIPE1_ID0,
	MMP_STICKER_PIPE1_ID1,
	MMP_STICKER_ID_NUM
} MMP_STICKER_ID;

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef struct _MMP_STICKER_ATTR
{
	MMP_UBYTE			ubStickerId;
    MMP_ULONG           ulBaseAddr;
    MMP_USHORT          usStartX;
    MMP_USHORT          usStartY;
    MMP_USHORT          usWidth;
    MMP_USHORT          usHeight;
    MMP_ICON_COLOR     	colorformat;
    MMP_ULONG           ulTpColor;
    MMP_BOOL            bTpEnable;
    MMP_BOOL            bSemiTpEnable;
    MMP_UBYTE           ubIconWeight;
    MMP_UBYTE           ubDstWeight;
} MMP_STICKER_ATTR;

typedef struct _MMP_ICO_PIPE_ATTR
{
    MMP_SCAL_PIPEID    	inputsel;
    MMP_BOOL            bDlineEn;
    MMP_USHORT         	usFrmWidth;
} MMP_ICO_PIPE_ATTR;

typedef struct _MMP_ICO_DL_SETTING
{
    MMP_UBYTE    	ubDlineCfg;
    MMP_USHORT      usFrameWidth;
} MMP_ICO_DL_SETTING;

/// @}

#endif //_MMP_ICON_INC_H_
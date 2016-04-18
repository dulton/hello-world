/// @ait_only
//==============================================================================
//
//  File        : mmp_ibc_inc.h
//  Description : INCLUDE File for the Common IBC Driver.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_IBC_INC_H_
#define _MMP_IBC_INC_H_

/** @addtogroup MMPF_IBC
 *  @{
 */

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmpf_icon.h"

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#define IBC_WHILE_TIMEOUT	(0x1000000)

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

typedef enum _MMP_IBC_LINK_TYPE
{
	MMP_IBC_LINK_NONE 			= 0x0000,
	MMP_IBC_LINK_DISPLAY		= 0x0001,
	MMP_IBC_LINK_VIDEO			= 0x0002,
	MMP_IBC_LINK_ROTATE			= 0x0004,
	MMP_IBC_LINK_FDTC      		= 0x0008,
	MMP_IBC_LINK_GRAPHIC   		= 0x0010,
	MMP_IBC_LINK_USB			= 0x0020,
	MMP_IBC_LINK_LDC			= 0x0040,
	MMP_IBC_LINK_MDTC			= 0x0080,
	MMP_IBC_LINK_GRA2MJPEG      = 0x0100,
	MMP_IBC_LINK_GRA2STILLJPG   = 0x0200,
	MMP_IBC_LINK_WIFI		    = 0x0400,
	MMP_IBC_LINK_GRA2UVC		= 0x0800,
	MMP_IBC_LINK_MASK			= 0xFFFF
} MMP_IBC_LINK_TYPE;

typedef enum _MMP_IBC_COLOR
{
    MMP_IBC_COLOR_RGB565       		= 0,
    MMP_IBC_COLOR_YUV422       		= 1,
    MMP_IBC_COLOR_RGB888       		= 2,
    MMP_IBC_COLOR_I420         		= 3,
    MMP_IBC_COLOR_YUV420_LUMA_ONLY 	= 4,
    MMP_IBC_COLOR_NV12         		= 5,
    MMP_IBC_COLOR_NV21         		= 6,
    MMP_IBC_COLOR_M420_CBCR    		= 7,
    MMP_IBC_COLOR_M420_CRCB    		= 8,
    MMP_IBC_COLOR_YUV422_YUYV  		= 9,
    MMP_IBC_COLOR_YUV422_UYVY  		= 10,
    MMP_IBC_COLOR_YUV422_YVYU  		= 11,
    MMP_IBC_COLOR_YUV422_VYUY  		= 12,
    MMP_IBC_COLOR_YUV444_2_YUV422_YUYV = 13,
    MMP_IBC_COLOR_YUV444_2_YUV422_YVYU = 14,
    MMP_IBC_COLOR_YUV444_2_YUV422_UYVY = 15,
    MMP_IBC_COLOR_YUV444_2_YUV422_VYUY = 16,
    MMP_IBC_COLOR_Y_ONLY            = 17,
    MMP_IBC_COLOR_MAX_NUM    
} MMP_IBC_COLOR;

typedef enum _MMP_IBC_FX
{
    MMP_IBC_FX_TOFB = 0,
    MMP_IBC_FX_JPG, 
    MMP_IBC_FX_RING_BUF,
    MMP_IBC_FX_H264_RT,
    MMP_IBC_FX_NUM
} MMP_IBC_FX;

typedef enum _MMP_IBC_PIPEID
{
    MMP_IBC_PIPE_0 = 0,
    MMP_IBC_PIPE_1,
    MMP_IBC_PIPE_2,
    MMP_IBC_PIPE_3,
    MMP_IBC_PIPE_4,
    MMP_IBC_PIPE_MAX
} MMP_IBC_PIPEID;

typedef enum _MMP_IBC_EVENT {
    MMP_IBC_EVENT_FRM_ST 		= 0,
    MMP_IBC_EVENT_FRM_RDY 		= 1,
    MMP_IBC_EVENT_FRM_END 		= 2,
    MMP_IBC_EVENT_FRM_PRERDY 	= 3,
    MMP_IBC_EVENT_MAX 			= 4
} MMP_IBC_EVENT;

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef struct _MMP_IBC_RECT {
    MMP_USHORT       	usLeft;
    MMP_USHORT       	usTop;
    MMP_USHORT         	usWidth;
    MMP_USHORT      	usHeight;
} MMP_IBC_RECT;

typedef struct _MMP_IBC_PIPE_ATTR
{
    MMP_ULONG           ulBaseAddr;
    MMP_ULONG           ulBaseUAddr;
    MMP_ULONG           ulBaseVAddr;
    MMP_ULONG           ulBaseEndAddr;
    MMP_ULONG           ulBaseEndUAddr;
    MMP_ULONG           ulBaseEndVAddr;
    MMP_IBC_COLOR      	colorformat;
    MMP_IBC_FX         	function;
    MMP_ULONG           ulLineOffset;
    MMP_ULONG           ulCbrLineOffset;
    MMP_BOOL            bMirrorEnable;
    MMP_USHORT          usMirrorWidth;
    MMP_ICO_PIPEID     	InputSource;
    MMP_USHORT			usBufWidth;         ///< For Partical Store
} MMP_IBC_PIPE_ATTR;

typedef void IbcCallBackFunc(void *argu);

/// @}

#endif // _MMP_IBC_INC_H_

/// @end_ait_only

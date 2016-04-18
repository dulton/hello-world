//==============================================================================
//
//  File        : mmp_rawproc_inc.h
//  Description : INCLUDE File for the Firmware Rawproc Control driver function
//  Author      : Ted
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_RAWPROC_INC_H_
#define _MMP_RAWPROC_INC_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "includes_fw.h"

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#define MAX_RAW_STORE_BUF_NUM	(3)

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

typedef enum _MMP_RAW_MDL_ID 
{
	MMP_RAW_MDL_ID0 = 0,
	MMP_RAW_MDL_ID1,
	MMP_RAW_MDL_NUM
} MMP_RAW_MDL_ID;

typedef enum _MMP_RAW_COLORFMT 
{
	MMP_RAW_COLORFMT_BAYER8 = 0,
	MMP_RAW_COLORFMT_BAYER10,
	MMP_RAW_COLORFMT_BAYER12,
	MMP_RAW_COLORFMT_BAYER14,
	MMP_RAW_COLORFMT_YUV420,
	MMP_RAW_COLORFMT_YUV422,
	MMP_RAW_COLORFMT_NUM
} MMP_RAW_COLORFMT;

typedef enum _MMP_RAW_IOPATH 
{
	MMP_RAW_IOPATH_NONE	 	= 0x00,
    MMP_RAW_IOPATH_VIF2RAW 	= 0x01,
    MMP_RAW_IOPATH_ISP2RAW 	= 0x02,
    MMP_RAW_IOPATH_RAW2ISP 	= 0x04,
    MMP_RAW_IOPATH_VIF2ISP 	= 0x08
} MMP_RAW_IOPATH;

typedef enum _MMP_RAW_FETCH_ROTATE_TYPE
{
    MMP_RAW_FETCH_NO_ROTATE = 0,
    MMP_RAW_FETCH_ROTATE_RIGHT_90,
    MMP_RAW_FETCH_ROTATE_RIGHT_180,
    MMP_RAW_FETCH_ROTATE_RIGHT_270
} MMP_RAW_FETCH_ROTATE_TYPE;

typedef enum _MMP_RAW_FETCH_MIRROR_TYPE
{
    MMP_RAW_FETCH_NO_MIRROR = 0,
    MMP_RAW_FETCH_V_MIRROR,
    MMP_RAW_FETCH_H_MIRROR
} MMP_RAW_FETCH_MIRROR_TYPE;

typedef enum _MMP_RAW_STORE_SRC 
{
	/* Ref : RAWPROC_S_SRC_SEL */
	MMP_RAW_S_SRC_VIF0 				= 0,
	MMP_RAW_S_SRC_VIF1				= 1,
	MMP_RAW_S_SRC_ISP_BEFORE_BS 	= 4,
	MMP_RAW_S_SRC_ISP_AFTER_BS		= 5,
	MMP_RAW_S_SRC_ISP_Y_DATA		= 6,
	MMP_RAW_S_SRC_ISP_AFTER_HDR		= 7,
	MMP_RAW_S_SRC_NUM
} MMP_RAW_STORE_SRC;

typedef enum _MMP_RAW_STORE_PLANE
{
    MMP_RAW_STORE_PLANE0 = 0,
    MMP_RAW_STORE_PLANE1,
    MMP_RAW_STORE_PLANE2, 
    MMP_RAW_STORE_PLANE3,
    MMP_RAW_STORE_PLANE_NUM
} MMP_RAW_STORE_PLANE;

typedef enum _MMP_RAW_EVENT 
{
    MMP_RAW_EVENT_STORE_DONE = 0,
    MMP_RAW_EVENT_STORE_FIFO_FULL,
    MMP_RAW_EVENT_FETCH_DONE,
    MMP_RAW_EVENT_MAX
} MMP_RAW_EVENT;

typedef enum _MMP_RAW_VC_SRC
{
    MMP_RAW_VC_SRC_VC0 = 0,
    MMP_RAW_VC_SRC_VC1,
    MMP_RAW_VC_SRC_VC2,
    MMP_RAW_VC_SRC_VC3,
    MMP_RAW_VC_SRC_NUM
} MMP_RAW_VC_SRC;

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef struct _MMP_RAW_FETCH_ATTR {
    MMP_USHORT                  usLeft;
    MMP_USHORT                  usTop;
    MMP_USHORT                  usWidth;
    MMP_USHORT                  usHeight;
    MMP_USHORT                  usLineOffset;
    MMP_RAW_FETCH_ROTATE_TYPE	rotateType;
    MMP_RAW_FETCH_MIRROR_TYPE	mirrorType;
} MMP_RAW_FETCH_ATTR;

typedef struct _MMP_RAW_STORE_BUF {
    MMP_UBYTE 					ulRawBufCnt;
    MMP_ULONG 					ulRawBufAddr[MAX_RAW_STORE_BUF_NUM];
    MMP_ULONG 					ulRawBufUAddr[MAX_RAW_STORE_BUF_NUM];
    MMP_ULONG 					ulRawBufVAddr[MAX_RAW_STORE_BUF_NUM];
} MMP_RAW_STORE_BUF;

typedef struct _MMP_RAW_STORE_GRAB {
    MMP_USHORT   				usPixelSt;
    MMP_USHORT   				usPixelEd;
    MMP_USHORT   				usLineSt;
    MMP_USHORT   				usLineEd;
} MMP_RAW_STORE_GRAB;

typedef void RawCallBackFunc(void*);

#endif // _MMP_RAWPROC_INC_H_


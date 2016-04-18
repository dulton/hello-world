//==============================================================================
//
//  File        : mmpf_dma.h
//  Description : INCLUDE File for the Firmware Graphic Driver (DMA portion).
//  Author      : Alan Wu
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPF_DMA_H_
#define _MMPF_DMA_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_graphics_inc.h"
#include "mmp_dma_inc.h"

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#define DMA_SEM_WAITTIME 	(0xF0000000)

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

typedef enum _MMPF_DMA_M_ID
{
    MMPF_DMA_M_0 = 0,
    MMPF_DMA_M_1,
    MMPF_DMA_M_MAX
} MMPF_DMA_M_ID;

typedef enum _MMPF_DMA_R_ID
{ 
    MMPF_DMA_R_0 = 0,
    MMPF_DMA_R_1,
    MMPF_DMA_R_MAX
} MMPF_DMA_R_ID;

typedef enum _MMPF_DMA_R_BPP
{
    MMPF_DMA_R_BPP8 = 0,
    MMPF_DMA_R_BPP16,
    MMPF_DMA_R_BPP24,
    MMPF_DMA_R_BPP32,
    MMPF_DMA_R_BPP_MAX
} MMPF_DMA_R_BPP;

typedef enum _MMPF_DMA_R_BLOCK
{
    MMPF_DMA_R_BLOCK64 = 0,
    MMPF_DMA_R_BLOCK128
} MMPF_DMA_R_BLOCK;

typedef enum _MMPF_DMA_R_TYPE
{
    MMPF_DMA_R_NO = 0,
    MMPF_DMA_R_90,
    MMPF_DMA_R_180,
    MMPF_DMA_R_270
} MMPF_DMA_R_TYPE;

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef struct _MMPF_DMA_ROT_DATA{
	MMP_SHORT               BufferNum;
	MMP_SHORT               BufferIndex;
    MMP_SHORT               SrcWidth[3];
    MMP_SHORT               SrcHeight[3];
    MMP_ULONG               SrcAddr[3];
    MMP_ULONG               DstAddr[3];
    MMP_USHORT              SrcLineOffset[3];
    MMP_USHORT              DstLineOffset[3];
    MMP_SHORT               BytePerPixel[3];
    MMPF_DMA_R_TYPE         RotateType;
    MMP_GRAPHICS_COLORDEPTH ColorDepth;
    MMP_BOOL                MirrorEnable;
    MMP_DMA_MIRROR_DIR      MirrorType;
} MMPF_DMA_ROT_DATA;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

MMP_ERR MMPF_DMA_Initialize(void);
MMP_ERR MMPF_DMA_MoveData(MMP_ULONG 		ulSrcAddr, 
						  MMP_ULONG 		ulDstAddr, 
                          MMP_ULONG 		ulCount,
                          DmaCallBackFunc 	*CallBackFunc, 
                          void 				*CallBackArg, 
                          MMP_BOOL 			bEnLineOfst, 
                          MMP_DMA_LINE_OFST *pLineOfst);
MMP_ERR MMPF_DMA_RotateImageBuftoBuf(MMP_GRAPHICS_BUF_ATTR 		*srcBufAttr, 
                                     MMP_GRAPHICS_RECT          *srcrect, 
                                     MMP_GRAPHICS_BUF_ATTR  	*dstBufAttr, 
                                     MMP_USHORT	 				usDstStartX, 
                                     MMP_USHORT 				usDstStartY, 
                                     MMP_GRAPHICS_ROTATE_TYPE 	rotatetype, 
                                     DmaCallBackFunc 			*CallBackFunc, 
                                     void 						*CallBackArgu, 
                                     MMP_BOOL 					bMirrorEn, 
                                     MMP_DMA_MIRROR_DIR			mirrortype);

#endif // _MMPF_DMA_H_

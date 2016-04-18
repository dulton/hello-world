/// @ait_only
//==============================================================================
//
//  File        : mmpd_fctl.h
//  Description : INCLUDE File for the Host Flow Control Driver.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

/**
 *  @file mmpd_fctl.h
 *  @brief The header File for the Host Flow Control Driver
 *  @author Penguin Torng
 *  @version 1.0
 */

#ifndef _MMPD_FCTL_H_
#define _MMPD_FCTL_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "mmp_ibc_inc.h"
#include "mmpd_ibc.h"
#include "mmpd_icon.h"
#include "mmpd_display.h"

/** @addtogroup MMPD_FCtl
 *  @{
 */

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#define MAX_PIPELINE_NUM    	(5)

#define MAX_FCTL_BUF_NUM		(4)
#define MAX_FCTL_ROT_BUF_NUM	(2)

#define FCTL_PIPE_TO_LINK(p, flink)	{	flink.scalerpath = (MMP_SCAL_PIPEID) p; \
										flink.icopipeID	 = (MMP_ICO_PIPEID)p; \
										flink.ibcpipeID  = (MMP_IBC_PIPEID)p; \
									}	

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef struct  _MMPD_FCTL_LINK
{
    MMP_SCAL_PIPEID    		scalerpath;
   	MMP_ICO_PIPEID     		icopipeID;
    MMP_IBC_PIPEID     		ibcpipeID;
} MMPD_FCTL_LINK;

typedef struct _MMPD_FCTL_ATTR 
{
    MMP_DISPLAY_COLORMODE	colormode;
    MMPD_FCTL_LINK			fctllink;
	MMP_SCAL_FIT_RANGE 		fitrange;
	MMP_SCAL_GRAB_CTRL 		grabctl;
	MMP_SCAL_SOURCE			scalsrc;
	MMP_BOOL				bSetScalerSrc;
	
    MMP_USHORT          	usBufCnt;
    MMP_ULONG           	ulBaseAddr[MAX_FCTL_BUF_NUM];
    MMP_ULONG           	ulBaseUAddr[MAX_FCTL_BUF_NUM];
    MMP_ULONG           	ulBaseVAddr[MAX_FCTL_BUF_NUM];
        
    MMP_BOOL            	bUseRotateDMA;      					// Use rotate DMA to rotate or not
    MMP_ULONG           	ulRotateAddr[MAX_FCTL_ROT_BUF_NUM];    	// Dest Y buffer address for rotate DMA
    MMP_ULONG           	ulRotateUAddr[MAX_FCTL_ROT_BUF_NUM];   	// Dest U buffer address for rotate DMA
    MMP_ULONG           	ulRotateVAddr[MAX_FCTL_ROT_BUF_NUM];   	// Dest V buffer address for rotate DMA
    MMP_USHORT          	usRotateBufCnt;     					// Dest buffer count for rotate DMA
} MMPD_FCTL_ATTR;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

MMP_ERR MMPD_Fctl_RawBuf2IbcBuf(MMPD_FCTL_LINK 			*fctllink, 
			                    MMP_GRAPHICS_BUF_ATTR 	*srcBufAttr, 
			                    MMP_GRAPHICS_RECT 		*rect, 
                                MMP_GRAPHICS_BUF_ATTR 	*dstBufAttr, 
                                MMP_USHORT 				usUpscale);

MMP_ERR MMPD_Fctl_SetPipeAttrForIbcFB(MMPD_FCTL_ATTR *pAttr);
MMP_ERR MMPD_Fctl_SetPipeAttrForH264FB(MMPD_FCTL_ATTR *pAttr);
MMP_ERR MMPD_Fctl_SetPipeAttrForH264Rt(MMPD_FCTL_ATTR *pAttr, MMP_ULONG ulEncWidth);
MMP_ERR MMPD_Fctl_SetPipeAttrForJpeg(MMPD_FCTL_ATTR         *pAttr,
                                    MMP_BOOL                bSetScalerGrab,
                                    MMP_BOOL                bSrcIsBT601);
MMP_ERR MMPD_Fctl_SetSubPipeAttr(MMPD_FCTL_ATTR *pAttr);

MMP_ERR MMPD_Fctl_GetAttributes(MMP_IBC_PIPEID pipeID, MMPD_FCTL_ATTR *pAttr);
MMP_ERR MMPD_Fctl_ClearPreviewBuf(MMP_IBC_PIPEID pipeID, MMP_ULONG ulClearColor);
MMP_ERR MMPD_Fctl_EnablePreview(MMP_UBYTE       ubSnrSel, 
                                MMP_IBC_PIPEID  pipeID,
                                MMP_BOOL        bEnable,
                                MMP_BOOL        bCheckFrameEnd);

/* IBC Link Type */
MMP_ERR MMPD_Fctl_GetIBCLinkAttr(MMP_IBC_PIPEID 		    pipeID, 
								 MMP_IBC_LINK_TYPE 		    *IBCLinkType,
								 MMP_DISPLAY_DEV_TYPE	    *previewdev,
								 MMP_DISPLAY_WIN_ID		    *winID,
								 MMP_DISPLAY_ROTATE_TYPE    *rotateDir);
MMP_ERR MMPD_Fctl_RestoreIBCLinkAttr(MMP_IBC_PIPEID 		    pipeID, 
									 MMP_IBC_LINK_TYPE 		    IBCLinkType,
									 MMP_DISPLAY_DEV_TYPE	    previewdev,
									 MMP_DISPLAY_WIN_ID		    winID,
									 MMP_DISPLAY_ROTATE_TYPE    rotateDir);

MMP_ERR MMPD_Fctl_ResetIBCLinkType(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPD_Fctl_LinkPipeToGraphic(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPD_Fctl_UnlinkPipeToGraphic(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPD_Fctl_LinkPipeToUsb(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPD_Fctl_UnlinkPipeToUsb(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPD_Fctl_LinkPipeToDma(    MMP_IBC_PIPEID          pipeID,
                                    MMP_DISPLAY_WIN_ID      winID,
                                    MMP_DISPLAY_DEV_TYPE    previewDev,
                                    MMP_DISPLAY_ROTATE_TYPE rotateDir);
MMP_ERR MMPD_Fctl_LinkPipeToDisplay(MMP_IBC_PIPEID          pipeID, 
                                    MMP_DISPLAY_WIN_ID      winID,
                                    MMP_DISPLAY_DEV_TYPE    previewDev);
MMP_ERR MMPD_Fctl_LinkPipeToVideo(MMP_IBC_PIPEID pipeID, MMP_USHORT ubEncId);
MMP_ERR MMPD_Fctl_LinkPipeToFdtc(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPD_Fctl_UnlinkPipeToFdtc(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPD_Fctl_LinkPipeToLdc(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPD_Fctl_UnLinkPipeToLdc(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPD_Fctl_LinkPipeToMdtc(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPD_Fctl_UnLinkPipeToMdtc(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPD_Fctl_LinkPipeToLdws(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPD_Fctl_UnLinkPipeToLdws(MMP_IBC_PIPEID pipeID);
MMP_ERR MMPD_Fctl_LinkPipeToGra2JPEG(MMP_IBC_PIPEID 	    pipeID,
								     MMP_DISPLAY_WIN_ID     winID,
									 MMP_DISPLAY_DEV_TYPE   previewDev,
									 MMP_BOOL               bStillCapture,
									 MMP_BOOL               bDisplayEnable);

MMP_ERR MMPD_Fctl_InitialBuffer(MMP_USHORT usIBCPipe);
/// @}

#endif // _MMPD_FCTL_H_

/// @end_ait_only

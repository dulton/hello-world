//==============================================================================
//
//  File        : mmpd_fctl.c
//  Description : Ritian Flow Control driver function
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

/**
*  @file mmpd_fctl.c
*  @brief The FLOW control functions
*  @author Penguin Torng
*  @version 1.0
*/

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "lib_retina.h"
#include "mmp_lib.h"
#include "mmpd_system.h"
#include "mmpd_fctl.h"
#include "mmpd_scaler.h"
#include "mmph_hif.h"
#include "mmpf_ldc.h"
#include "mmpf_mci.h"
#include "mmpf_graphics.h"
#include "mmpf_display.h"
/** @addtogroup MMPD_FCTL
 *  @{
 */

//==============================================================================
//
//                              GLOBAL VARIABLES
//
//==============================================================================

static MMPD_FCTL_ATTR m_FctlAttr[MAX_PIPELINE_NUM];

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_RawBuf2IbcBuf
//  Description : Use Graphic module to do buffer to IBC buffer operation.
//------------------------------------------------------------------------------
/** @brief Transformation of the RAW data image in the frame buffer, The transform
opertaion includes color transform and dimension transform

The function can transfer the picture from one format to another format, and
change the scale ratio of the pictures.

@return It reports the status of the operation.
*/
MMP_ERR MMPD_Fctl_RawBuf2IbcBuf(MMPD_FCTL_LINK 			*fctllink, 
			                    MMP_GRAPHICS_BUF_ATTR 	*srcBufAttr, 
			                    MMP_GRAPHICS_RECT 		*rect, 
                                MMP_GRAPHICS_BUF_ATTR 	*dstBufAttr, 
                                MMP_USHORT 				usUpscale)
{
   	MMP_SCAL_FIT_RANGE 		fitrange;
   	MMP_SCAL_GRAB_CTRL		grabctl;
   	MMP_ICO_PIPE_ATTR 		icoAttr;
   	MMP_IBC_PIPE_ATTR 		ibcAttr;
   	MMP_SCAL_COLORMODE  	scal_outcolor;
	MMP_ERR					retstatus;
    
    /* Parameter Check */
	if (srcBufAttr == NULL || rect == NULL || dstBufAttr == NULL) {
		return MMP_FCTL_ERR_PARAMETER;
	}

	if (rect->usWidth > srcBufAttr->usWidth || rect->usHeight > srcBufAttr->usHeight) {
		return MMP_FCTL_ERR_PARAMETER;
    }

	if (srcBufAttr->colordepth == MMP_GRAPHICS_COLORDEPTH_YUV420 ||
		srcBufAttr->colordepth == MMP_GRAPHICS_COLORDEPTH_YUV420_INTERLEAVE) {
		
		/* YUV420 not support grab partial frame : TBD */ 
        if (rect->usLeft || rect->usTop)
		    return MMP_FCTL_ERR_PARAMETER;
        if (srcBufAttr->usLineOffset != rect->usWidth)
		    return MMP_FCTL_ERR_PARAMETER;
    }

	/* Config Graphic Module */
	MMPD_Graphics_SetDelayType(MMP_GRAPHICS_DELAY_CHK_SCA_BUSY);

    retstatus = MMPD_Graphics_SetScaleAttr(srcBufAttr, rect, usUpscale);
    
    /* Config IBC Module */
    switch (dstBufAttr->colordepth) {
    case MMP_GRAPHICS_COLORDEPTH_16:
        scal_outcolor = MMP_SCAL_COLOR_RGB565;
		ibcAttr.colorformat = MMP_IBC_COLOR_RGB565;
        break;
    case MMP_GRAPHICS_COLORDEPTH_24:
        scal_outcolor = MMP_SCAL_COLOR_RGB888;
		ibcAttr.colorformat = MMP_IBC_COLOR_RGB888;
        break;
    case MMP_GRAPHICS_COLORDEPTH_YUV422_UYVY:
        scal_outcolor = MMP_SCAL_COLOR_YUV444;
		ibcAttr.colorformat = MMP_IBC_COLOR_YUV444_2_YUV422_UYVY;
        break;
    case MMP_GRAPHICS_COLORDEPTH_YUV422_VYUY:
        scal_outcolor = MMP_SCAL_COLOR_YUV444;
		ibcAttr.colorformat = MMP_IBC_COLOR_YUV444_2_YUV422_VYUY;
        break;
    case MMP_GRAPHICS_COLORDEPTH_YUV422_YUYV:
        scal_outcolor = MMP_SCAL_COLOR_YUV444;
		ibcAttr.colorformat = MMP_IBC_COLOR_YUV444_2_YUV422_YUYV;
        break;
    case MMP_GRAPHICS_COLORDEPTH_YUV422_YVYU:
        scal_outcolor = MMP_SCAL_COLOR_YUV444;
		ibcAttr.colorformat = MMP_IBC_COLOR_YUV444_2_YUV422_YVYU;
        break;
    case MMP_GRAPHICS_COLORDEPTH_YUV420:
        scal_outcolor = MMP_SCAL_COLOR_YUV444;
		ibcAttr.colorformat = MMP_IBC_COLOR_I420;
        break;
    case MMP_GRAPHICS_COLORDEPTH_YUV420_INTERLEAVE:
        scal_outcolor = MMP_SCAL_COLOR_YUV444;
		ibcAttr.colorformat = MMP_IBC_COLOR_NV12;
        break;
    }

	ibcAttr.ulBaseAddr      = dstBufAttr->ulBaseAddr;
	ibcAttr.ulBaseUAddr     = dstBufAttr->ulBaseUAddr;
	ibcAttr.ulBaseVAddr     = dstBufAttr->ulBaseVAddr;
    ibcAttr.ulLineOffset    = 0;
    ibcAttr.InputSource     = fctllink->icopipeID;
	ibcAttr.function        = MMP_IBC_FX_TOFB;
	ibcAttr.bMirrorEnable   = MMP_FALSE;

    MMPD_Icon_ResetModule(fctllink->icopipeID);
	MMPD_IBC_SetAttributes(fctllink->ibcpipeID, &ibcAttr);
    MMPD_IBC_SetStoreEnable(fctllink->ibcpipeID, MMP_TRUE);

	/* Config Icon Module */
    icoAttr.inputsel        = fctllink->scalerpath;
	icoAttr.bDlineEn        = MMP_TRUE;
	icoAttr.usFrmWidth 		= dstBufAttr->usWidth;
	MMPD_Icon_SetDLAttributes(fctllink->icopipeID, &icoAttr);
	MMPD_Icon_SetDLEnable(fctllink->icopipeID, MMP_TRUE);

	/* Config Scaler Module */
    fitrange.fitmode		= MMP_SCAL_FITMODE_OUT;
	fitrange.scalerType		= MMP_SCAL_TYPE_SCALER;
	fitrange.ulInWidth      = rect->usWidth * usUpscale;
    fitrange.ulInHeight     = rect->usHeight * usUpscale;
	fitrange.ulOutWidth 	= dstBufAttr->usWidth;
    fitrange.ulOutHeight 	= dstBufAttr->usHeight;

    fitrange.ulInGrabX 		= 1;
    fitrange.ulInGrabY 		= 1;
    fitrange.ulInGrabW 		= fitrange.ulInWidth;
    fitrange.ulInGrabH 		= fitrange.ulInHeight;

    MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);

    MMPD_Scaler_SetOutColor(fctllink->scalerpath, scal_outcolor);

	if (scal_outcolor == MMP_SCAL_COLOR_RGB565 || 
		scal_outcolor == MMP_SCAL_COLOR_RGB888) {
		MMPD_Scaler_SetOutColorTransform(fctllink->scalerpath, MMP_TRUE, MMP_SCAL_COLRMTX_FULLRANGE_TO_RGB);
	}
	else {
	    #if (CCIR656_FORCE_SEL_BT601)
		MMPD_Scaler_SetOutColorTransform(fctllink->scalerpath, MMP_TRUE, MMP_SCAL_COLRMTX_FULLRANGE_TO_BT601);
		#else        
		MMPD_Scaler_SetOutColorTransform(fctllink->scalerpath, MMP_TRUE, MMP_SCAL_COLRMTX_YUV_FULLRANGE);
        #endif
	}
    MMPD_Scaler_SetEngine(MMP_SCAL_USER_DEF_TYPE_IN_OUT, fctllink->scalerpath, &fitrange, &grabctl);
    MMPD_Scaler_SetLPF(fctllink->scalerpath, &fitrange, &grabctl);

    MMPD_Scaler_SetPixelDelay(fctllink->scalerpath, 1, 1);
    MMPD_Scaler_SetLineDelay(fctllink->scalerpath, 0x10);
    
	MMPD_Scaler_SetPath(fctllink->scalerpath, MMP_SCAL_SOURCE_GRA, MMP_TRUE);
	MMPD_Scaler_SetEnable(fctllink->scalerpath, MMP_TRUE);

	MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_TRUE);

	/* Trigger Graphic Engine */
	if (grabctl.ulScaleM >= grabctl.ulScaleN) {
    	MMPD_Graphics_SetPixDelay(10, 20);
    }
    else {
        MMPD_Graphics_SetPixDelay(15, 20);  //For G0 264MHz
    }

	MMPF_Graphics_TriggerAndWaitDone(srcBufAttr->colordepth);

	/* Check IBC store done then close clock */
    MMPD_IBC_CheckFrameReady(fctllink->ibcpipeID);

	MMPD_IBC_SetStoreEnable(fctllink->ibcpipeID, MMP_FALSE);
	MMPD_Scaler_SetEnable(fctllink->scalerpath, MMP_FALSE);

	MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_FALSE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_FALSE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_FALSE);
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_SetPipeAttrForIbcFB
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_SetPipeAttrForIbcFB(MMPD_FCTL_ATTR *pAttr)
{	
   	MMP_ULONG			i;
   	MMP_ERR				mmpstatus;
   	MMP_SCAL_FIT_RANGE 	fitrange = pAttr->fitrange;
   	MMP_SCAL_GRAB_CTRL 	grabctl  = pAttr->grabctl;
   	MMP_SCAL_COLORMODE 	scal_outcolor;
   	MMP_ICO_PIPE_ATTR	icoAttr;
   	MMP_IBC_PIPE_ATTR	ibcAttr;
    MMP_USHORT          usScalInW;
    MMP_USHORT          usScalOutW;
	/* Config IBC Module */
    switch (pAttr->colormode) {

    case MMP_DISPLAY_COLOR_RGB565:
		ibcAttr.colorformat = MMP_IBC_COLOR_RGB565;
        scal_outcolor 		= MMP_SCAL_COLOR_RGB565;
        break;
    case MMP_DISPLAY_COLOR_RGB888:
		ibcAttr.colorformat = MMP_IBC_COLOR_RGB888;
        scal_outcolor 		= MMP_SCAL_COLOR_RGB888;
        break;
    case MMP_DISPLAY_COLOR_YUV422:
		ibcAttr.colorformat = MMP_IBC_COLOR_YUV444_2_YUV422_UYVY;
        scal_outcolor 		= MMP_SCAL_COLOR_YUV444;
		break;
    case MMP_DISPLAY_COLOR_YUV420:
		ibcAttr.colorformat = MMP_IBC_COLOR_I420;
        scal_outcolor 		= MMP_SCAL_COLOR_YUV444;
        break;
    case MMP_DISPLAY_COLOR_YUV420_INTERLEAVE:
		ibcAttr.colorformat = MMP_IBC_COLOR_NV12;
        scal_outcolor 		= MMP_SCAL_COLOR_YUV444;
        break;
    }

	ibcAttr.ulBaseAddr 	    = pAttr->ulBaseAddr[0];
	ibcAttr.ulBaseUAddr     = pAttr->ulBaseUAddr[0];
	ibcAttr.ulBaseVAddr 	= pAttr->ulBaseVAddr[0];
    ibcAttr.ulLineOffset 	= 0;
    ibcAttr.InputSource     = pAttr->fctllink.icopipeID;
	ibcAttr.function 		= MMP_IBC_FX_TOFB;
	ibcAttr.bMirrorEnable   = MMP_FALSE;

    MMPD_Icon_ResetModule(pAttr->fctllink.icopipeID);
    
    #if (MCR_V2_UNDER_DBG)
    /* Have to reset MCI byte count to 256. Otherwise, ICON overflowed. */
    MMPD_IBC_SetMCI_ByteCount(pAttr->fctllink.ibcpipeID, MMPF_MCI_BYTECNT_SEL_256BYTE);
    #endif	
	MMPD_IBC_SetAttributes(pAttr->fctllink.ibcpipeID, &ibcAttr);

	/* Config Icon Module */
    icoAttr.inputsel 	= pAttr->fctllink.scalerpath;
	icoAttr.bDlineEn 	= MMP_TRUE;
	icoAttr.usFrmWidth 	= grabctl.ulOutEdX - grabctl.ulOutStX + 1;
	MMPD_Icon_SetDLAttributes(pAttr->fctllink.icopipeID, &icoAttr);
	MMPD_Icon_SetDLEnable(pAttr->fctllink.icopipeID, MMP_TRUE);
	
	/* Config Scaler Module */
    MMPD_Scaler_SetOutColor(pAttr->fctllink.scalerpath, scal_outcolor);

	if (scal_outcolor == MMP_SCAL_COLOR_RGB565 || 
		scal_outcolor == MMP_SCAL_COLOR_RGB888) {
		MMPD_Scaler_SetOutColorTransform(pAttr->fctllink.scalerpath, MMP_TRUE, MMP_SCAL_COLRMTX_FULLRANGE_TO_RGB);
	}
	else {
	    #if (CCIR656_FORCE_SEL_BT601)
		MMPD_Scaler_SetOutColorTransform(pAttr->fctllink.scalerpath, MMP_TRUE, MMP_SCAL_COLRMTX_FULLRANGE_TO_BT601);
		#else
		MMPD_Scaler_SetOutColorTransform(pAttr->fctllink.scalerpath, MMP_TRUE, MMP_SCAL_COLRMTX_YUV_FULLRANGE);
		#endif
	}

	MMPD_Scaler_SetEngine(MMP_SCAL_USER_DEF_TYPE_IN_OUT, pAttr->fctllink.scalerpath, &fitrange, &grabctl);
	MMPD_Scaler_SetLPF(pAttr->fctllink.scalerpath, &fitrange, &grabctl);
    
    /* Check Bypass Scaler or not */
    /* if scale up, should limit Input W, else if scale down, should limit Ouput W*/
    MMPD_Scaler_GetGrabRange(pAttr->fctllink.scalerpath, MMP_SCAL_GRAB_STAGE_SCA, &usScalInW, NULL);
    MMPD_Scaler_GetGrabRange(pAttr->fctllink.scalerpath, MMP_SCAL_GRAB_STAGE_OUT, &usScalOutW, NULL);

    if ((usScalOutW > MMPD_Scaler_GetMaxDelayLineWidth(pAttr->fctllink.scalerpath)) &&
        (usScalOutW <= usScalInW)){
        MMPD_Scaler_BypassScaler(pAttr->fctllink.scalerpath, MMP_TRUE);
    }
    else if ((usScalInW > MMPD_Scaler_GetMaxDelayLineWidth(pAttr->fctllink.scalerpath)) &&
         (usScalOutW > usScalInW)){
	    MMPD_Scaler_BypassScaler(pAttr->fctllink.scalerpath, MMP_TRUE);
	}
	else {
	    MMPD_Scaler_BypassScaler(pAttr->fctllink.scalerpath, MMP_FALSE);
	}

    MMPD_Scaler_SetPixelDelay(pAttr->fctllink.scalerpath, 1, 1);
    
    if (pAttr->bSetScalerSrc) {
		MMPD_Scaler_SetPath(pAttr->fctllink.scalerpath, pAttr->scalsrc, MMP_TRUE);
	}
	MMPD_Scaler_SetEnable(pAttr->fctllink.scalerpath, MMP_TRUE);
    
    /* Set preview buffer count and address */
	{
	    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
	    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pAttr->fctllink.ibcpipeID);
		MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 2, pAttr->usBufCnt);
    	MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_PREVIEW_BUF | BUFFER_COUNT);
    	
		for (i = 0; i < pAttr->usBufCnt; i++) 
		{   
	        MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pAttr->fctllink.ibcpipeID);
	        MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 2, i);
	        MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 4, pAttr->ulBaseAddr[i]);
	        MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 8, pAttr->ulBaseUAddr[i]);
	        MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 12, pAttr->ulBaseVAddr[i]);

	        mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_PREVIEW_BUF | BUFFER_ADDRESS);
		}

        MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pAttr->fctllink.ibcpipeID);
        MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 2, grabctl.ulOutEdX - grabctl.ulOutStX + 1);
        MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_PREVIEW_BUF | BUFFER_WIDTH);
        MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pAttr->fctllink.ibcpipeID); 
        MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 2, grabctl.ulOutEdY - grabctl.ulOutStY + 1);
        MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_PREVIEW_BUF | BUFFER_HEIGHT);

		MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
	}
	
	/* Set rotate buffer count and address */
    if (pAttr->bUseRotateDMA) {
    
        MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
        MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pAttr->usRotateBufCnt);  
        MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_ROTATE_BUF | BUFFER_COUNT);

    	for (i = 0; i < pAttr->usRotateBufCnt; i++) {

            MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, i);
            MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 4, pAttr->ulRotateAddr[i]);
            MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 8, pAttr->ulRotateUAddr[i]);
            MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 12, pAttr->ulRotateVAddr[i]);
            MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 16, (MMP_ULONG)pAttr);            

            MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_ROTATE_BUF | BUFFER_ADDRESS);
		}
		
		MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
	}
    
	/* Reset Link pipes */
	MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pAttr->fctllink.ibcpipeID);
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | LINK_NONE);
	MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
	
	m_FctlAttr[pAttr->fctllink.ibcpipeID] = *pAttr;

	return mmpstatus;
}

#if (TV_JAGGY_WORKAROUND)
//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_SetPipeAttrForIbcFbForTV
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_SetPipeAttrForIbcFbForTV(MMP_ULONG ulSensorW, MMP_ULONG ulSensorH, MMPD_FCTL_ATTR *pAttr)
{	
   	MMP_ULONG			i;
   	MMP_ERR				mmpstatus;
   	MMP_SCAL_FIT_RANGE 	fitrange 	= pAttr->fitrange;
   	MMP_SCAL_GRAB_CTRL 	grabctl  	= pAttr->grabctl;
   	MMP_SCAL_PIPEID		srcPipeId 	= MMP_SCAL_PIPE_3;
   	MMP_SCAL_FIT_RANGE	srcFitRange;
   	MMP_SCAL_GRAB_CTRL 	srcGrabCtl;
   	MMP_SCAL_COLORMODE 	scal_outcolor;
   	MMP_ICO_PIPE_ATTR	icoAttr;
   	MMP_IBC_PIPE_ATTR	ibcAttr;
    MMP_USHORT			usScalInW, usScalOutW;

	/* Config IBC Module */
    switch (pAttr->colormode) {

    case MMP_DISPLAY_COLOR_RGB565:
		ibcAttr.colorformat = MMP_IBC_COLOR_RGB565;
        scal_outcolor 		= MMP_SCAL_COLOR_RGB565;
        break;       
    case MMP_DISPLAY_COLOR_RGB888:
		ibcAttr.colorformat = MMP_IBC_COLOR_RGB888;
        scal_outcolor 		= MMP_SCAL_COLOR_RGB888;
        break;        
    case MMP_DISPLAY_COLOR_YUV422:
		ibcAttr.colorformat = MMP_IBC_COLOR_YUV444_2_YUV422_UYVY;
        scal_outcolor 		= MMP_SCAL_COLOR_YUV444;
		break;		
    case MMP_DISPLAY_COLOR_YUV420:
		ibcAttr.colorformat = MMP_IBC_COLOR_I420;
        scal_outcolor 		= MMP_SCAL_COLOR_YUV444;
        break;        
    case MMP_DISPLAY_COLOR_YUV420_INTERLEAVE:
		ibcAttr.colorformat = MMP_IBC_COLOR_NV12;
        scal_outcolor 		= MMP_SCAL_COLOR_YUV444;
        break;
    }

	ibcAttr.ulBaseAddr 	    = pAttr->ulBaseAddr[0];
	ibcAttr.ulBaseUAddr     = pAttr->ulBaseUAddr[0];
	ibcAttr.ulBaseVAddr 	= pAttr->ulBaseVAddr[0];
    ibcAttr.ulLineOffset 	= 0;
    ibcAttr.InputSource     = pAttr->fctllink.icopipeID;
	ibcAttr.function 		= MMP_IBC_FX_TOFB;
	ibcAttr.bMirrorEnable   = MMP_FALSE;

    MMPD_Icon_ResetModule(pAttr->fctllink.icopipeID);
	MMPD_IBC_SetAttributes(pAttr->fctllink.ibcpipeID, &ibcAttr);
    
	/* Config Icon Module */
    icoAttr.inputsel 	= pAttr->fctllink.scalerpath;
	icoAttr.bDlineEn 	= MMP_TRUE;
	icoAttr.usFrmWidth 	= grabctl.ulOutEdX - grabctl.ulOutStX + 1;
	MMPD_Icon_SetDLAttributes(pAttr->fctllink.icopipeID, &icoAttr);
	MMPD_Icon_SetDLEnable(pAttr->fctllink.icopipeID, MMP_TRUE);
	
	/* Config Scaler Module */
    MMPD_Scaler_SetOutColor(pAttr->fctllink.scalerpath, scal_outcolor);

	if (scal_outcolor == MMP_SCAL_COLOR_RGB565 || 
		scal_outcolor == MMP_SCAL_COLOR_RGB888) {
		MMPD_Scaler_SetOutColorTransform(pAttr->fctllink.scalerpath, MMP_TRUE, MMP_SCAL_COLRMTX_FULLRANGE_TO_RGB);
	}
	else {
    	#if (CCIR656_FORCE_SEL_BT601)
	    MMPD_Scaler_SetOutColorTransform(pAttr->scalerpath, MMP_TRUE, MMP_SCAL_COLRMTX_FULLRANGE_TO_BT601);
        #else        
		MMPD_Scaler_SetOutColorTransform(pAttr->fctllink.scalerpath, MMP_TRUE, MMP_SCAL_COLRMTX_YUV_FULLRANGE);
        #endif
	}

	MMPD_Scaler_SetEngine(MMP_SCAL_USER_DEF_TYPE_IN_OUT, pAttr->fctllink.scalerpath, &fitrange, &grabctl);
	MMPD_Scaler_SetLPF(pAttr->fctllink.scalerpath, &fitrange, &grabctl);
    
    /* Check Bypass Scaler or not */
    /* if scale up, should limit Input W, else if scale down, should limit Ouput W*/
    MMPD_Scaler_GetGrabRange(pAttr->fctllink.scalerpath, MMP_SCAL_GRAB_STAGE_SCA, &usScalInW, NULL);
    MMPD_Scaler_GetGrabRange(pAttr->fctllink.scalerpath, MMP_SCAL_GRAB_STAGE_OUT, &usScalOutW, NULL);
    if ((usScalOutW > MMPD_Scaler_GetMaxDelayLineWidth(pAttr->fctllink.scalerpath)) &&
        (usScalOutW <= usScalInW)){
        MMPD_Scaler_BypassScaler(pAttr->fctllink.scalerpath, MMP_TRUE);
    }
    else if ((usScalInW > MMPD_Scaler_GetMaxDelayLineWidth(pAttr->fctllink.scalerpath)) &&
         (usScalOutW > usScalInW)){
        MMPD_Scaler_BypassScaler(pAttr->fctllink.scalerpath, MMP_TRUE);
    }
    else {
        MMPD_Scaler_BypassScaler(pAttr->fctllink.scalerpath, MMP_FALSE);
    }
    MMPD_Scaler_SetPixelDelay(pAttr->fctllink.scalerpath, 1, 1);
    
    MMPD_Scaler_SetSerialLinkPipe(srcPipeId, pAttr->fctllink.scalerpath);
	MMPD_Scaler_SetEnable(pAttr->fctllink.scalerpath, MMP_TRUE);
	
	/* Config Source Scaler Module */
    srcFitRange.ulInWidth  	= ulSensorW;
    srcFitRange.ulInHeight 	= ulSensorH;
    srcFitRange.fitmode    	= fitrange.fitmode;
    srcFitRange.scalerType	= MMP_SCAL_TYPE_SCALER;
	srcFitRange.ulOutWidth  = TV_JAGGY_1ST_OUT_W;
	srcFitRange.ulOutHeight = TV_JAGGY_1ST_OUT_H;

    srcFitRange.ulInGrabX	= 1;
    srcFitRange.ulInGrabY	= 1;
    srcFitRange.ulInGrabW	= srcFitRange.ulInWidth;
    srcFitRange.ulInGrabH	= srcFitRange.ulInHeight;

	MMPD_Scaler_GetGCDBestFitScale(&srcFitRange, &srcGrabCtl);

    MMPD_Scaler_SetOutColor(srcPipeId, MMP_SCAL_COLOR_YUV444);
   	#if (CCIR656_FORCE_SEL_BT601)
    MMPD_Scaler_SetOutColorTransform(srcPipeId, MMP_TRUE, MMP_SCAL_COLRMTX_FULLRANGE_TO_BT601);
    #else
	MMPD_Scaler_SetOutColorTransform(srcPipeId, MMP_TRUE, MMP_SCAL_COLRMTX_YUV_FULLRANGE);
    #endif
	MMPD_Scaler_SetEngine(MMP_SCAL_USER_DEF_TYPE_IN_OUT, srcPipeId, &srcFitRange, &srcGrabCtl);
	MMPD_Scaler_SetLPF(srcPipeId, &srcFitRange, &srcGrabCtl);

	MMPD_Scaler_SetPath(srcPipeId, MMP_SCAL_SOURCE_ISP, MMP_TRUE);
	MMPD_Scaler_SetEnable(srcPipeId, MMP_TRUE);

    /* Set preview buffer count and address */
	{
	    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
	    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pAttr->fctllink.ibcpipeID);
		MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 2, pAttr->usBufCnt);
    	MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_PREVIEW_BUF | BUFFER_COUNT);
    	
		for (i = 0; i < pAttr->usBufCnt; i++) 
		{   
	        MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pAttr->fctllink.ibcpipeID);
	        MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 2, i);
	        MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 4, pAttr->ulBaseAddr[i]);
	        MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 8, pAttr->ulBaseUAddr[i]);
	        MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 12, pAttr->ulBaseVAddr[i]);

	        mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_PREVIEW_BUF | BUFFER_ADDRESS);
		}

        MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pAttr->fctllink.ibcpipeID);
        MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 2, grabctl.ulOutEdX - grabctl.ulOutStX + 1);
        MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_PREVIEW_BUF | BUFFER_WIDTH); 
        MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pAttr->fctllink.ibcpipeID);
        MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 2, grabctl.ulOutEdY - grabctl.ulOutStY + 1);
        MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_PREVIEW_BUF | BUFFER_HEIGHT);

		MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
	}
	
	/* Set rotate buffer count and address */
    if (pAttr->bUseRotateDMA) {
    
        MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
        MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pAttr->usRotateBufCnt);  
        MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_ROTATE_BUF | BUFFER_COUNT);

    	for (i = 0; i < pAttr->usRotateBufCnt; i++) {

            MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, i);
            MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 4, pAttr->ulRotateAddr[i]);
            MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 8, pAttr->ulRotateUAddr[i]);
            MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 12, pAttr->ulRotateVAddr[i]);

            MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_ROTATE_BUF | BUFFER_ADDRESS);
		}
		
		MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
	}
    
	/* Reset Link pipes */
	MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pAttr->fctllink.ibcpipeID);
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | LINK_NONE);
	MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
	
	m_FctlAttr[pAttr->fctllink.ibcpipeID] = *pAttr;

	return mmpstatus;
}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_SetPipeAttrForH264FB
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_SetPipeAttrForH264FB(MMPD_FCTL_ATTR *pAttr)
{	
   	MMP_ULONG			i;
   	MMP_ERR				mmpstatus;
   	MMP_ICO_PIPE_ATTR 	icoAttr;
   	MMP_IBC_PIPE_ATTR	ibcAttr;
    MMP_USHORT			usScalInW, usScalOutW;

	/* Config IBC Module */
    switch (pAttr->colormode) {

    case MMP_DISPLAY_COLOR_YUV420_INTERLEAVE:
		ibcAttr.colorformat = MMP_IBC_COLOR_NV12;
        break;
    default:
		RTNA_DBG_Str(0, "InCorrect H264 supported format\r\n");
		return MMP_FCTL_ERR_PARAMETER;
    	break;
    }

	ibcAttr.ulBaseAddr 	    = pAttr->ulBaseAddr[0];
	ibcAttr.ulBaseUAddr     = pAttr->ulBaseUAddr[0];
	ibcAttr.ulBaseVAddr 	= pAttr->ulBaseVAddr[0];
    ibcAttr.ulLineOffset 	= 0;
    ibcAttr.InputSource     = pAttr->fctllink.icopipeID;
	ibcAttr.function 		= MMP_IBC_FX_TOFB;
	ibcAttr.bMirrorEnable   = MMP_FALSE;

    MMPD_Icon_ResetModule(pAttr->fctllink.icopipeID);
	MMPD_IBC_SetAttributes(pAttr->fctllink.ibcpipeID, &ibcAttr);

    #if (MCR_V2_UNDER_DBG)
    /* Have to reset MCI byte count to 256. Otherwise, ICON overflowed. */
    MMPD_IBC_SetMCI_ByteCount(pAttr->fctllink.ibcpipeID, MMPF_MCI_BYTECNT_SEL_256BYTE);
    #endif

	/* Config Icon Module */
    icoAttr.inputsel 	= pAttr->fctllink.scalerpath;
	icoAttr.bDlineEn 	= MMP_TRUE;
	icoAttr.usFrmWidth 	= pAttr->grabctl.ulOutEdX - pAttr->grabctl.ulOutStX + 1;
	MMPD_Icon_SetDLAttributes(pAttr->fctllink.icopipeID, &icoAttr);
	MMPD_Icon_SetDLEnable(pAttr->fctllink.icopipeID, MMP_TRUE);
	
	/* Config Scaler Module */
    MMPD_Scaler_SetOutColor(pAttr->fctllink.scalerpath, MMP_SCAL_COLOR_YUV444);
	MMPD_Scaler_SetOutColorTransform(pAttr->fctllink.scalerpath, MMP_TRUE, MMP_SCAL_COLRMTX_FULLRANGE_TO_BT601);
	MMPD_Scaler_SetStopEnable(pAttr->fctllink.scalerpath, MMP_SCAL_STOP_SRC_H264, MMP_TRUE);

    MMPD_Scaler_SetEngine(MMP_SCAL_USER_DEF_TYPE_IN_OUT, pAttr->fctllink.scalerpath, &(pAttr->fitrange), &(pAttr->grabctl));
    MMPD_Scaler_SetLPF(pAttr->fctllink.scalerpath, &(pAttr->fitrange), &(pAttr->grabctl));  

    /* Check Bypass Scaler or not */
    /* if scale up, should limit Input W, else if scale down, should limit Ouput W*/
    MMPD_Scaler_GetGrabRange(pAttr->fctllink.scalerpath, MMP_SCAL_GRAB_STAGE_SCA, &usScalInW, NULL);
    MMPD_Scaler_GetGrabRange(pAttr->fctllink.scalerpath, MMP_SCAL_GRAB_STAGE_OUT, &usScalOutW, NULL);
    if ((usScalOutW > MMPD_Scaler_GetMaxDelayLineWidth(pAttr->fctllink.scalerpath)) &&
        (usScalOutW <= usScalInW)){
        MMPD_Scaler_BypassScaler(pAttr->fctllink.scalerpath, MMP_TRUE);
    }
    else if ((usScalInW > MMPD_Scaler_GetMaxDelayLineWidth(pAttr->fctllink.scalerpath)) &&
         (usScalOutW > usScalInW)){
        MMPD_Scaler_BypassScaler(pAttr->fctllink.scalerpath, MMP_TRUE);
    }
    else {
        MMPD_Scaler_BypassScaler(pAttr->fctllink.scalerpath, MMP_FALSE);
    }
    
    MMPD_Scaler_SetPixelDelay(pAttr->fctllink.scalerpath, 1, 1);
    
    if (pAttr->bSetScalerSrc) {
		MMPD_Scaler_SetPath(pAttr->fctllink.scalerpath, pAttr->scalsrc, MMP_TRUE);    
	}
	MMPD_Scaler_SetEnable(pAttr->fctllink.scalerpath, MMP_TRUE);
    
    /* Set encode buffer (current buffer) address */
	for (i = 0; i < pAttr->usBufCnt; i++) 
	{
	    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
        MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pAttr->fctllink.ibcpipeID);
        MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 2, i);
        MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 4, pAttr->ulBaseAddr[i]);
        MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 8, pAttr->ulBaseUAddr[i]);
        MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 12, pAttr->ulBaseVAddr[i]);

        mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_PREVIEW_BUF | BUFFER_ADDRESS);
	    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
	}

    /* Set encode buffer count */
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pAttr->fctllink.ibcpipeID);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 2, pAttr->usBufCnt);  
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_PREVIEW_BUF | BUFFER_COUNT);
	
	/* Reset Link pipes */
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pAttr->fctllink.ibcpipeID);
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | LINK_NONE);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);

	m_FctlAttr[pAttr->fctllink.ibcpipeID] = *pAttr;

	return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_SetPipeAttrForH264Rt
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_SetPipeAttrForH264Rt(MMPD_FCTL_ATTR *pAttr, MMP_ULONG ulEncWidth)
{
   	MMP_ERR				mmpstatus = MMP_ERR_NONE;
   	MMP_ICO_PIPE_ATTR	icoAttr;
   	MMP_IBC_PIPE_ATTR	ibcAttr;

	/* Config IBC Module */
    switch (pAttr->colormode) {

    case MMP_DISPLAY_COLOR_YUV420_INTERLEAVE:
		ibcAttr.colorformat = MMP_IBC_COLOR_NV12;
        break;
    default:
		RTNA_DBG_Str(0, "InCorrect H264 supported format\r\n");
		return MMP_FCTL_ERR_PARAMETER;    
    	break;
    }

    ibcAttr.ulLineOffset 	= 0;
    ibcAttr.InputSource     = pAttr->fctllink.icopipeID;
	ibcAttr.function 		= MMP_IBC_FX_H264_RT;
	ibcAttr.bMirrorEnable   = MMP_FALSE;

    MMPD_Icon_ResetModule(pAttr->fctllink.icopipeID);
	MMPD_IBC_SetAttributes(pAttr->fctllink.ibcpipeID, &ibcAttr);
    MMPD_IBC_SetRtPingPongAddr(pAttr->fctllink.ibcpipeID,
                               pAttr->ulBaseAddr[0],
                               pAttr->ulBaseUAddr[0],
                               pAttr->ulBaseVAddr[0],
                               pAttr->ulBaseAddr[1],
                               pAttr->ulBaseUAddr[1],
                               pAttr->ulBaseVAddr[1],
                               ulEncWidth);

    #if (MCR_V2_UNDER_DBG)
    /* Have to reset MCI byte count to 128 after DSC capture,
     * otherwise, no H.264 encode done, and ICON overflowed. */
    MMPD_IBC_SetMCI_ByteCount(pAttr->fctllink.ibcpipeID, MMPF_MCI_BYTECNT_SEL_128BYTE);
    #endif

	/* Config Icon Module */
    icoAttr.inputsel 	= pAttr->fctllink.scalerpath;
	icoAttr.bDlineEn 	= MMP_TRUE;
	icoAttr.usFrmWidth 	= pAttr->grabctl.ulOutEdX - pAttr->grabctl.ulOutStX + 1;
	MMPD_Icon_SetDLAttributes(pAttr->fctllink.icopipeID, &icoAttr);
	MMPD_Icon_SetDLEnable(pAttr->fctllink.icopipeID, MMP_TRUE);
	
	/* Config Scaler Module */
    MMPD_Scaler_SetOutColor(pAttr->fctllink.scalerpath, MMP_SCAL_COLOR_YUV444);
	MMPD_Scaler_SetOutColorTransform(pAttr->fctllink.scalerpath, MMP_TRUE, MMP_SCAL_COLRMTX_FULLRANGE_TO_BT601);
    MMPD_Scaler_SetStopEnable(pAttr->fctllink.scalerpath, MMP_SCAL_STOP_SRC_H264, MMP_TRUE);

    MMPD_Scaler_SetEngine(MMP_SCAL_USER_DEF_TYPE_IN_OUT, pAttr->fctllink.scalerpath, &(pAttr->fitrange), &(pAttr->grabctl));
    MMPD_Scaler_SetLPF(pAttr->fctllink.scalerpath, &(pAttr->fitrange), &(pAttr->grabctl));

	if ((pAttr->fitrange).ulInGrabW < (pAttr->fitrange).ulOutWidth || 
		(pAttr->fitrange).ulInGrabH < (pAttr->fitrange).ulOutHeight) {
		// RealTime mode need to set pixel delay to prevent H264 buffer overlap.
    	MMPD_Scaler_SetPixelDelay(pAttr->fctllink.scalerpath, 16, 20); // For ISP:480MHz, G0:264MHz case
    	//MMPD_Scaler_SetPixelDelay(pAttr->fctllink.scalerpath, 11, 20); // For ISP:528MHz, G0:264MHz case
	}
	else {
	    MMPD_Scaler_SetPixelDelay(pAttr->fctllink.scalerpath, 1, 1);
	}

    if (pAttr->bSetScalerSrc) {
		MMPD_Scaler_SetPath(pAttr->fctllink.scalerpath, pAttr->scalsrc, MMP_TRUE);    
	}
	MMPD_Scaler_SetEnable(pAttr->fctllink.scalerpath, MMP_TRUE);

	/* Reset Link pipes */
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pAttr->fctllink.ibcpipeID);
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | LINK_NONE);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    	
	m_FctlAttr[pAttr->fctllink.ibcpipeID] = *pAttr;

	return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_SetPipeAttrForJpeg
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_SetPipeAttrForJpeg(MMPD_FCTL_ATTR *pAttr,
                                     MMP_BOOL       bSetScalerGrab,
                                     MMP_BOOL       bSrcIsBT601)
{
   	MMP_ERR				mmpstatus = MMP_ERR_NONE;
   	MMP_ICO_PIPE_ATTR 	icoAttr;
   	MMP_IBC_PIPE_ATTR	ibcAttr;
    MMP_USHORT			usScalInW, usScalOutW;
    
	/* Config IBC Module */
    ibcAttr.InputSource     = pAttr->fctllink.icopipeID;
	ibcAttr.function 		= MMP_IBC_FX_JPG;
	ibcAttr.bMirrorEnable   = MMP_FALSE;
    MMPD_Icon_ResetModule(pAttr->fctllink.icopipeID);    
	MMPD_IBC_SetAttributes(pAttr->fctllink.ibcpipeID, &ibcAttr);

	/* Config Icon Module */
    icoAttr.inputsel 	= pAttr->fctllink.scalerpath;
	icoAttr.bDlineEn 	= MMP_TRUE;
	icoAttr.usFrmWidth 	= pAttr->grabctl.ulOutEdX - pAttr->grabctl.ulOutStX + 1;
	MMPD_Icon_SetDLAttributes(pAttr->fctllink.icopipeID, &icoAttr);
	MMPD_Icon_SetDLEnable(pAttr->fctllink.icopipeID, MMP_TRUE);
	
	/* Config Scaler Module */
    MMPD_Scaler_SetOutColor(pAttr->fctllink.scalerpath, MMP_SCAL_COLOR_YUV444);
    if (bSrcIsBT601) {
        MMPD_Scaler_SetOutColorTransform(pAttr->fctllink.scalerpath, MMP_TRUE,
                                         MMP_SCAL_COLRMTX_BT601_TO_FULLRANGE);
    }
    else {
	    MMPD_Scaler_SetOutColorTransform(pAttr->fctllink.scalerpath, MMP_TRUE,
                                         MMP_SCAL_COLRMTX_YUV_FULLRANGE);
    }
	
	if (bSetScalerGrab) {
    	MMPD_Scaler_SetEngine(MMP_SCAL_USER_DEF_TYPE_IN_OUT, pAttr->fctllink.scalerpath, &(pAttr->fitrange), &(pAttr->grabctl));
    	MMPD_Scaler_SetLPF(pAttr->fctllink.scalerpath, &(pAttr->fitrange), &(pAttr->grabctl));

        /* Check Bypass Scaler or not */
        /* if scale up, should limit Input W, else if scale down, should limit Ouput W*/
        MMPD_Scaler_GetGrabRange(pAttr->fctllink.scalerpath, MMP_SCAL_GRAB_STAGE_SCA, &usScalInW, NULL);
        MMPD_Scaler_GetGrabRange(pAttr->fctllink.scalerpath, MMP_SCAL_GRAB_STAGE_OUT, &usScalOutW, NULL);
        if ((usScalOutW > MMPD_Scaler_GetMaxDelayLineWidth(pAttr->fctllink.scalerpath)) &&
	    (usScalOutW <= usScalInW)) {
	        MMPD_Scaler_BypassScaler(pAttr->fctllink.scalerpath, MMP_TRUE);
	    }
	    else if ((usScalInW > MMPD_Scaler_GetMaxDelayLineWidth(pAttr->fctllink.scalerpath)) &&
	         (usScalOutW > usScalInW)) {
	        MMPD_Scaler_BypassScaler(pAttr->fctllink.scalerpath, MMP_TRUE);
		}
		else {
            MMPD_Scaler_BypassScaler(pAttr->fctllink.scalerpath, MMP_FALSE);
		}
	}
    if (pAttr->bSetScalerSrc) {
		MMPD_Scaler_SetPath(pAttr->fctllink.scalerpath, pAttr->scalsrc, MMP_TRUE);
	}

	return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_SetSubPipeAttr
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_SetSubPipeAttr(MMPD_FCTL_ATTR *pAttr)
{	
   	MMP_ULONG					i;
   	MMP_ERR						mmpstatus;
   	MMP_ICO_PIPE_ATTR 			icoSubAttr;
   	MMP_IBC_PIPE_ATTR			ibcSubAttr;
    
    if (pAttr->fctllink.ibcpipeID != MMP_IBC_PIPE_4) {
    	return MMP_ERR_NONE;
    }

	/* Config IBC Module */
	ibcSubAttr.ulBaseAddr		= pAttr->ulBaseAddr[0];
	ibcSubAttr.bMirrorEnable	= MMP_FALSE;
	ibcSubAttr.usMirrorWidth 	= 0;

    MMPD_Icon_ResetModule(pAttr->fctllink.icopipeID);
	MMPD_IBC_SetAttributes(pAttr->fctllink.ibcpipeID, &ibcSubAttr);
    MMPD_IBC_SetStoreEnable(pAttr->fctllink.ibcpipeID, MMP_TRUE);

	/* Config Icon Module */
    icoSubAttr.inputsel 	= pAttr->fctllink.scalerpath;
	icoSubAttr.bDlineEn 	= MMP_TRUE;
	icoSubAttr.usFrmWidth 	= pAttr->grabctl.ulOutEdX - pAttr->grabctl.ulOutStX + 1;
	MMPD_Icon_SetDLAttributes(pAttr->fctllink.icopipeID, &icoSubAttr);
	MMPD_Icon_SetDLEnable(pAttr->fctllink.icopipeID, MMP_TRUE);
	
	/* Config Scaler Module */
    MMPD_Scaler_SetEngine(MMP_SCAL_USER_DEF_TYPE_IN_OUT, pAttr->fctllink.scalerpath, &(pAttr->fitrange), &(pAttr->grabctl)); 
    MMPD_Scaler_SetLPF(pAttr->fctllink.scalerpath, &(pAttr->fitrange), &(pAttr->grabctl));
    
    if (pAttr->bSetScalerSrc) {
		MMPD_Scaler_SetPath(pAttr->fctllink.scalerpath, pAttr->scalsrc, MMP_TRUE);    
	}
	MMPD_Scaler_SetEnable(pAttr->fctllink.scalerpath, MMP_TRUE);
    
    /* Set preview buffer count and address */
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pAttr->fctllink.ibcpipeID);
	MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 2, pAttr->usBufCnt);
	MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_PREVIEW_BUF | BUFFER_COUNT);
	
	for (i = 0; i < pAttr->usBufCnt; i++) 
	{   
        MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0,  pAttr->fctllink.ibcpipeID);
        MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 2,  i);
        MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 4,  pAttr->ulBaseAddr[i]);
		MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 8,  0);
		MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 12, 0);
		
        mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_PREVIEW_BUF | BUFFER_ADDRESS);
	}
	
	MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
	/* Set Link Type */
	MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pAttr->fctllink.ibcpipeID);
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | LINK_NONE);
	MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
	
	m_FctlAttr[pAttr->fctllink.ibcpipeID] = *pAttr;

	return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_GetAttributes
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_GetAttributes(MMP_IBC_PIPEID pipeID, MMPD_FCTL_ATTR *pAttr)
{
	*pAttr = m_FctlAttr[pipeID];

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_ClearPreviewBuf
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_ClearPreviewBuf(MMP_IBC_PIPEID pipeID, MMP_ULONG ulClearColor)
{
    MMP_GRAPHICS_DRAWRECT_ATTR 	drawAttr;
    MMP_GRAPHICS_RECT          	rect;
    MMP_ULONG                   oldColor;
    MMP_ULONG					i;
	MMP_USHORT					usBufWidth, usBufHeight;

	return MMP_ERR_NONE; //TBD

	usBufWidth  = m_FctlAttr[pipeID].grabctl.ulOutEdX - m_FctlAttr[pipeID].grabctl.ulOutStX + 1;
	usBufHeight = m_FctlAttr[pipeID].grabctl.ulOutEdY - m_FctlAttr[pipeID].grabctl.ulOutStY + 1;

	for (i = 0; i < m_FctlAttr[pipeID].usBufCnt; i++) 
	{
        drawAttr.type       = MMP_GRAPHICS_SOLID_FILL;
        drawAttr.bUseRect   = MMP_TRUE;
	    drawAttr.usWidth    = usBufWidth;
    	drawAttr.usHeight   = usBufHeight;
	    drawAttr.ulBaseAddr = m_FctlAttr[pipeID].ulBaseAddr[i];
	    drawAttr.ulColor    = ulClearColor;
	    drawAttr.ulPenSize  = 1;
	    drawAttr.ropcode    = MMP_GRAPHICS_ROP_SRCCOPY;

	    if (m_FctlAttr[pipeID].colormode == MMP_DISPLAY_COLOR_RGB565) {
        	drawAttr.usLineOfst = usBufWidth*2;
	        drawAttr.colordepth = MMP_GRAPHICS_COLORDEPTH_16;
    	}
	    else if (m_FctlAttr[pipeID].colormode == MMP_DISPLAY_COLOR_RGB888) {
        	drawAttr.usLineOfst = usBufWidth*3;
	        drawAttr.colordepth = MMP_GRAPHICS_COLORDEPTH_24;
    	}
	    else if (m_FctlAttr[pipeID].colormode == MMP_DISPLAY_COLOR_YUV422) {
        	drawAttr.usLineOfst = usBufWidth*2;
	        drawAttr.colordepth = MMP_GRAPHICS_COLORDEPTH_YUV422_UYVY;
    	}
	    else if (m_FctlAttr[pipeID].colormode == MMP_DISPLAY_COLOR_YUV420) {
        	drawAttr.usLineOfst = usBufWidth;
	        drawAttr.colordepth = MMP_GRAPHICS_COLORDEPTH_8;
    	}
	    else if (m_FctlAttr[pipeID].colormode == MMP_DISPLAY_COLOR_YUV420_INTERLEAVE) {
        	drawAttr.usLineOfst = usBufWidth;
	        drawAttr.colordepth = MMP_GRAPHICS_COLORDEPTH_8;
    	}
    	
	    rect.usLeft     = 0;
	    rect.usTop      = 0;
    	rect.usWidth    = drawAttr.usWidth;
    	rect.usHeight   = drawAttr.usHeight;
    	
	    MMPD_Graphics_DrawRectToBuf(&drawAttr, &rect, &oldColor);

	    if (m_FctlAttr[pipeID].colormode == MMP_DISPLAY_COLOR_YUV420) 
	    {
    	    drawAttr.usWidth    = usBufWidth / 2;
        	drawAttr.usHeight   = usBufHeight / 2;
	        drawAttr.ulBaseAddr = m_FctlAttr[pipeID].ulBaseUAddr[i];
        	drawAttr.usLineOfst = usBufWidth / 2;

	        rect.usLeft     	= 0;
        	rect.usTop      	= 0;
    	    rect.usWidth    	= drawAttr.usWidth;
	        rect.usHeight   	= drawAttr.usHeight;
	        
    	    MMPD_Graphics_DrawRectToBuf(&drawAttr, &rect, NULL);

	        drawAttr.ulBaseAddr = m_FctlAttr[pipeID].ulBaseVAddr[i];
	        
    	    MMPD_Graphics_DrawRectToBuf(&drawAttr, &rect, NULL);
	    }
	    else if (m_FctlAttr[pipeID].colormode == MMP_DISPLAY_COLOR_YUV420_INTERLEAVE) 
	    {
    	    drawAttr.usWidth     = usBufWidth;
        	drawAttr.usHeight    = usBufHeight / 2;
	        drawAttr.ulBaseAddr  = m_FctlAttr[pipeID].ulBaseUAddr[i];
        	drawAttr.usLineOfst  = usBufWidth;

	        rect.usLeft     	= 0;
        	rect.usTop      	= 0;
    	    rect.usWidth    	= drawAttr.usWidth;
	        rect.usHeight   	= drawAttr.usHeight;
	        
    	    MMPD_Graphics_DrawRectToBuf(&drawAttr, &rect, NULL);
	    }
	}

    // Restore the old key color
    MMPD_Graphics_SetKeyColor(MMP_GRAPHICS_FG_COLOR, oldColor);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_EnablePreview
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_EnablePreview(MMP_UBYTE ubSnrSel, MMP_IBC_PIPEID pipeID, MMP_BOOL bEnable, MMP_BOOL bCheckFrameEnd)
{
	MMP_ERR	mmpstatus;

	if (bEnable) {

    	MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, 	MMP_TRUE);
        MMPD_System_EnableClock(MMPD_SYS_CLK_DMA, 	MMP_TRUE);
		MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);
		MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, 	MMP_TRUE);
        MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, 	MMP_TRUE);
	
	    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
	    MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 0, ubSnrSel);
	    MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 4, pipeID);
		MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 8, bCheckFrameEnd);

		mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_PREVIEW_ENABLE | ENABLE_PREVIEW);
        
		if (MMPH_HIF_GetParameterL(GRP_IDX_FLOWCTL, 0) == 0) {
		    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
	        return MMP_ERR_NONE;
	    }    
	    else {
	        MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
	        return MMP_DISPLAY_ERR_FRAME_END;
	    }    
	}
	else {

        MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
        MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 0, ubSnrSel);
        MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 4, pipeID);
        MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 8, bCheckFrameEnd);

		mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_PREVIEW_ENABLE | DISABLE_PREVIEW);
		MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
        
        MMPD_Scaler_SetEnable(pipeID, MMP_FALSE);
		
		// It should ensure all pipe are disabled then close clock.
		//MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, 	MMP_FALSE);
        //MMPD_System_EnableClock(MMPD_SYS_CLK_DMA, 	MMP_FALSE);
		//MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, 	MMP_FALSE);
		//MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, 	MMP_FALSE);
		//MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, 	MMP_FALSE);

        return mmpstatus;
	}
}

#if 0
void ____Link_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_GetIBCLinkAttr
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_GetIBCLinkAttr(MMP_IBC_PIPEID 		    pipeID,
								 MMP_IBC_LINK_TYPE 		    *IBCLinkType,
								 MMP_DISPLAY_DEV_TYPE	    *previewdev,
								 MMP_DISPLAY_WIN_ID		    *winID,
								 MMP_DISPLAY_ROTATE_TYPE    *rotateDir)
{
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);

    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pipeID);
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | GET_IBC_LINK_ATTR);
    
    if (IBCLinkType)
	    *IBCLinkType = MMPH_HIF_GetParameterW(GRP_IDX_FLOWCTL, 0);
	if (previewdev)
	    *previewdev  = MMPH_HIF_GetParameterW(GRP_IDX_FLOWCTL, 2);
	if (winID)
	    *winID       = MMPH_HIF_GetParameterW(GRP_IDX_FLOWCTL, 4);
	if (rotateDir)
	    *rotateDir   = MMPH_HIF_GetParameterW(GRP_IDX_FLOWCTL, 6);
	
	MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_RestoreIBCLinkAttr
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_RestoreIBCLinkAttr(MMP_IBC_PIPEID 		    pipeID, 
									 MMP_IBC_LINK_TYPE 		    IBCLinkType,
									 MMP_DISPLAY_DEV_TYPE	    previewdev,
									 MMP_DISPLAY_WIN_ID		    winID,
									 MMP_DISPLAY_ROTATE_TYPE    rotateDir)
{

	if (IBCLinkType & MMP_IBC_LINK_DISPLAY) {
        MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
		MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pipeID);
		MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 2, previewdev);
		MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 4, winID);
		MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | LINK_DISPLAY);	
	    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
	}
	if (IBCLinkType & MMP_IBC_LINK_VIDEO) {
	    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
	    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pipeID);
	    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | LINK_VIDEO);
	    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
	}
	if (IBCLinkType & MMP_IBC_LINK_ROTATE) {
	    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
		MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pipeID);
		MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 2, previewdev);
		MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 4, winID);
		MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 6, rotateDir);
		MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | LINK_DMA);
	    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
	}
	if (IBCLinkType & MMP_IBC_LINK_FDTC) {
		MMPD_Fctl_LinkPipeToFdtc(pipeID);
	}
	if (IBCLinkType & MMP_IBC_LINK_GRAPHIC) {
		MMPD_Fctl_LinkPipeToGraphic(pipeID);
	}
	if (IBCLinkType & MMP_IBC_LINK_LDC) {
		MMPD_Fctl_LinkPipeToLdc(pipeID);
	}
	if (IBCLinkType & MMP_IBC_LINK_MDTC) {
		MMPD_Fctl_LinkPipeToMdtc(pipeID);
	}

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_ResetIBCLinkType
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_ResetIBCLinkType(MMP_IBC_PIPEID pipeID)
{
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pipeID);
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | LINK_NONE);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_LinkPipeToDisplay
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_LinkPipeToDisplay(MMP_IBC_PIPEID 			pipeID, 
									MMP_DISPLAY_WIN_ID 		winID, 
									MMP_DISPLAY_DEV_TYPE 	previewdev)
{
   	MMP_GRAPHICS_BUF_ATTR  bufAttr;

    bufAttr.usWidth	 = m_FctlAttr[pipeID].grabctl.ulOutEdX - m_FctlAttr[pipeID].grabctl.ulOutStX + 1;
    bufAttr.usHeight = m_FctlAttr[pipeID].grabctl.ulOutEdY - m_FctlAttr[pipeID].grabctl.ulOutStY + 1;
    
    switch (m_FctlAttr[pipeID].colormode) {

	    case MMP_DISPLAY_COLOR_RGB565:
	        bufAttr.usLineOffset = bufAttr.usWidth*2;
	        bufAttr.colordepth	 = MMP_GRAPHICS_COLORDEPTH_16;
	        break;
	    case MMP_DISPLAY_COLOR_RGB888:
	        bufAttr.usLineOffset = bufAttr.usWidth*3;
	        bufAttr.colordepth 	 = MMP_GRAPHICS_COLORDEPTH_24;
	        break;
	    case MMP_DISPLAY_COLOR_YUV422:
	        bufAttr.usLineOffset = bufAttr.usWidth*2;
	        bufAttr.colordepth 	 = MMP_GRAPHICS_COLORDEPTH_YUV422_UYVY;
	        break;
	    case MMP_DISPLAY_COLOR_YUV420:
	        bufAttr.usLineOffset = bufAttr.usWidth;
	        bufAttr.colordepth 	 = MMP_GRAPHICS_COLORDEPTH_YUV420;
	        break;
	    case MMP_DISPLAY_COLOR_YUV420_INTERLEAVE:
	        bufAttr.usLineOffset = bufAttr.usWidth;
            bufAttr.colordepth 	 = MMP_GRAPHICS_COLORDEPTH_YUV420_INTERLEAVE;
	        break;
    }

	MMPD_Display_SetWinActive(winID, MMP_FALSE);

    bufAttr.ulBaseAddr  = m_FctlAttr[pipeID].ulBaseAddr[0];
    bufAttr.ulBaseUAddr = m_FctlAttr[pipeID].ulBaseUAddr[0];
    bufAttr.ulBaseVAddr = m_FctlAttr[pipeID].ulBaseVAddr[0];
    MMPD_Display_BindBufToWin(&bufAttr, winID);

    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pipeID);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 2, previewdev);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 4, winID);

    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | LINK_DISPLAY);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_LinkPipeToVideo
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_LinkPipeToVideo(MMP_IBC_PIPEID pipeID, MMP_USHORT ubEncId)
{
	if ((m_FctlAttr[pipeID].colormode != MMP_DISPLAY_COLOR_YUV420) && 
	    (m_FctlAttr[pipeID].colormode != MMP_DISPLAY_COLOR_YUV420_INTERLEAVE)) {
		return MMP_FCTL_ERR_PARAMETER;
    }
    
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pipeID);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 2, ubEncId);
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | LINK_VIDEO);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_LinkPipeToDma
//  Description : For rotation
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_LinkPipeToDma(MMP_IBC_PIPEID          pipeID, 
                                MMP_DISPLAY_WIN_ID      winID, 
								MMP_DISPLAY_DEV_TYPE  	previewDev,
								MMP_DISPLAY_ROTATE_TYPE	rotateDir)
{
    MMP_GRAPHICS_BUF_ATTR  bufAttr;

	/* Note: It use the rotated buffer width/height as window buffer width/height */
	bufAttr.usWidth  = m_FctlAttr[pipeID].grabctl.ulOutEdY - m_FctlAttr[pipeID].grabctl.ulOutStY + 1;
    bufAttr.usHeight = m_FctlAttr[pipeID].grabctl.ulOutEdX - m_FctlAttr[pipeID].grabctl.ulOutStX + 1;

    switch (m_FctlAttr[pipeID].colormode) {
    case MMP_DISPLAY_COLOR_RGB565:
        bufAttr.usLineOffset = bufAttr.usWidth*2;
        bufAttr.colordepth   = MMP_GRAPHICS_COLORDEPTH_16;
        break;
    case MMP_DISPLAY_COLOR_RGB888:
        bufAttr.usLineOffset = bufAttr.usWidth*3;
        bufAttr.colordepth   = MMP_GRAPHICS_COLORDEPTH_24;
        break;
    case MMP_DISPLAY_COLOR_YUV422:
        bufAttr.usLineOffset = bufAttr.usWidth*2;
        bufAttr.colordepth   = MMP_GRAPHICS_COLORDEPTH_YUV422_UYVY;
        break;
    case MMP_DISPLAY_COLOR_YUV420:
        bufAttr.usLineOffset = bufAttr.usWidth;
        bufAttr.colordepth   = MMP_GRAPHICS_COLORDEPTH_YUV420;
        break;
    case MMP_DISPLAY_COLOR_YUV420_INTERLEAVE:
        bufAttr.usLineOffset = bufAttr.usWidth;
        bufAttr.colordepth   = MMP_GRAPHICS_COLORDEPTH_YUV420_INTERLEAVE;
        break;
    default:
    	return MMP_FCTL_ERR_PARAMETER;
    }

    bufAttr.ulBaseAddr  = m_FctlAttr[pipeID].ulRotateAddr[0];
    bufAttr.ulBaseUAddr = m_FctlAttr[pipeID].ulRotateUAddr[0];
    bufAttr.ulBaseVAddr = m_FctlAttr[pipeID].ulRotateVAddr[0];
    MMPD_Display_BindBufToWin(&bufAttr, winID);

    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pipeID);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 2, previewDev);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 4, winID);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 6, rotateDir);

    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | LINK_DMA);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_LinkPipeToFdtc
//  Description : 
//------------------------------------------------------------------------------
/** @brief Establish the scaler path and link to FDTC function.

The function link the flow path from sensor to FDTC module, the path used is always
different to preview path.

@return It reports the status of the operation.
*/
MMP_ERR MMPD_Fctl_LinkPipeToFdtc(MMP_IBC_PIPEID pipeID)
{
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0); 
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pipeID);
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | LINK_FDTC);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_UnlinkPipeToFdtc
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_UnlinkPipeToFdtc(MMP_IBC_PIPEID pipeID)
{
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pipeID);
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | UNLINK_FDTC);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_LinkPipeToGraphic
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_LinkPipeToGraphic(MMP_IBC_PIPEID pipeID)
{
   	MMP_GRAPHICS_BUF_ATTR  	bufAttr = {0, };
   	MMP_GRAPHICS_RECT		srcrect = {0, };

    bufAttr.usWidth	 = m_FctlAttr[pipeID].grabctl.ulOutEdX - m_FctlAttr[pipeID].grabctl.ulOutStX + 1;
    bufAttr.usHeight = m_FctlAttr[pipeID].grabctl.ulOutEdY - m_FctlAttr[pipeID].grabctl.ulOutStY + 1;

    switch (m_FctlAttr[pipeID].colormode) {

	    case MMP_DISPLAY_COLOR_RGB565:
	        bufAttr.usLineOffset = bufAttr.usWidth*2;
	        bufAttr.colordepth 	 = MMP_GRAPHICS_COLORDEPTH_16;
	        break;
	    case MMP_DISPLAY_COLOR_RGB888:
	        bufAttr.usLineOffset = bufAttr.usWidth*3;
	        bufAttr.colordepth 	 = MMP_GRAPHICS_COLORDEPTH_24;
	        break;
	    case MMP_DISPLAY_COLOR_YUV422:
	        bufAttr.usLineOffset = bufAttr.usWidth*2;
	        bufAttr.colordepth 	 = MMP_GRAPHICS_COLORDEPTH_YUV422_UYVY;
	        break;
	    case MMP_DISPLAY_COLOR_YUV420:
	        bufAttr.usLineOffset = bufAttr.usWidth;
	        bufAttr.colordepth 	 = MMP_GRAPHICS_COLORDEPTH_YUV420;
	        break;
	    case MMP_DISPLAY_COLOR_YUV420_INTERLEAVE:
	        bufAttr.usLineOffset = bufAttr.usWidth;
	        bufAttr.colordepth 	 = MMP_GRAPHICS_COLORDEPTH_YUV420_INTERLEAVE;
	        break;
    }

    srcrect.usLeft 		= 0;
    srcrect.usTop 		= 0;
    srcrect.usWidth 	= bufAttr.usWidth;
	srcrect.usHeight 	= bufAttr.usHeight;
	
	// Source buffer address will be assigned in the IBC ISR
	MMPD_Graphics_SetScaleAttr(&bufAttr, &srcrect, 1);
    
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pipeID);
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | LINK_GRAPHIC);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_UnlinkPipeToGraphic
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_UnlinkPipeToGraphic(MMP_IBC_PIPEID pipeID)
{
	MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pipeID);
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | UNLINK_GRAPHIC);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_LinkPipeToUsb
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_LinkPipeToUsb(MMP_IBC_PIPEID pipeID)
{
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pipeID);
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | LINK_USB);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_LinkPipeToUsb
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_UnlinkPipeToUsb(MMP_IBC_PIPEID pipeID)
{
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pipeID);
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | UNLINK_USB);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_LinkPipeToLdc
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_LinkPipeToLdc(MMP_IBC_PIPEID pipeID)
{
#if (SUPPORT_LDC_RECD)||(SUPPORT_LDC_CAPTURE)
	MMP_GRAPHICS_BUF_ATTR	bufAttr = {0, };
	MMP_GRAPHICS_RECT		srcrect = {0, };
	MMP_LDC_ATTR			ldcAttr;
	MMP_LDC_LINK			ldcLink;
	MMP_UBYTE				ubResMode;
	MMP_UBYTE				ubFpsMode;
	MMP_UBYTE				ubRunMode;
	
	/* Set LDC attribute */
	ubResMode = MMPF_LDC_GetResMode();
	ubFpsMode = MMPF_LDC_GetFpsMode();
	ubRunMode = MMPF_LDC_GetRunMode();
	
	MMPF_LDC_GetLinkPipe(&ldcLink);
	
	if (ubResMode >= MMP_LDC_RES_MODE_NUM) {
		return MMP_LDC_ERR_PARAMETER;
	}	

 	MMPF_LDC_Initialize();
	
	if (ubRunMode == MMP_LDC_RUN_MODE_SINGLE_RUN ||
		ubRunMode == MMP_LDC_RUN_MODE_MULTI_RUN) 
	{
		ldcAttr.bLdcEnable		= MMP_TRUE;
		ldcAttr.ubInputPath 	= MMPF_LDC_INPUT_FROM_GRA;
		ldcAttr.usInputStX		= 0;
		ldcAttr.usInputStY		= 0;
		ldcAttr.usInputW		= m_FctlAttr[pipeID].grabctl.ulOutEdX - m_FctlAttr[pipeID].grabctl.ulOutStX + 1;
		ldcAttr.usInputH		= m_FctlAttr[pipeID].grabctl.ulOutEdY - m_FctlAttr[pipeID].grabctl.ulOutStY + 1;
		ldcAttr.ubInputXratio	= 0;
		ldcAttr.ubInputYratio	= 0;
		ldcAttr.bOutCrop		= MMP_FALSE;
		ldcAttr.usOutputStX 	= 0;
		ldcAttr.usOutputStY 	= 0;
		ldcAttr.usOutputW		= ldcAttr.usInputW;
		ldcAttr.usOutputH		= ldcAttr.usInputH;
		ldcAttr.ubOutLineDelay	= 0;
		ldcAttr.ulDmaAddr		= 0x0;
		MMPF_LDC_SetAttribute(&ldcAttr);
		MMPF_LDC_UpdateLUT(NULL, NULL);
		
		bufAttr.usWidth 		= m_FctlAttr[pipeID].grabctl.ulOutEdX - m_FctlAttr[pipeID].grabctl.ulOutStX + 1;
		bufAttr.usHeight		= m_FctlAttr[pipeID].grabctl.ulOutEdY - m_FctlAttr[pipeID].grabctl.ulOutStY + 1;
		bufAttr.usLineOffset	= bufAttr.usWidth;
		bufAttr.colordepth		= MMP_GRAPHICS_COLORDEPTH_YUV420_INTERLEAVE;

		srcrect.usLeft			= 0;
		srcrect.usTop			= 0;
		srcrect.usWidth 		= bufAttr.usWidth;
		srcrect.usHeight		= bufAttr.usHeight;
		
		// Source buffer address will be assigned in the IBC ISR
		MMPD_Graphics_SetScaleAttr(&bufAttr, &srcrect, 1); 
		MMPD_Graphics_SetDelayType(MMP_GRAPHICS_DELAY_CHK_LINE_END/*MMP_GRAPHICS_DELAY_CHK_SCA_BUSY*/);
	}
	else if (ubRunMode == MMP_LDC_RUN_MODE_MULTI_SLICE)
	{
		MMPF_LDC_MultiSliceInitAttr();
	}
	
	switch(ubResMode)
	{
		case MMP_LDC_RES_MODE_FHD:
			if (ubFpsMode == MMP_LDC_FPS_MODE_60P) {
				MMPD_Graphics_SetPixDelay(11, 20);
				MMPD_Graphics_SetLineDelay(0);
			}
			else if (ubFpsMode == MMP_LDC_FPS_MODE_30P) {
				MMPD_Graphics_SetPixDelay(6, 20);
				MMPD_Graphics_SetLineDelay(0);
			}
		break;
		case MMP_LDC_RES_MODE_HD:
			if (ubFpsMode == MMP_LDC_FPS_MODE_120P) {
				MMPD_Graphics_SetPixDelay(10, 20);
				MMPD_Graphics_SetLineDelay(1);
			}
			else if (ubFpsMode == MMP_LDC_FPS_MODE_60P) {
				MMPD_Graphics_SetPixDelay(6, 20);
				MMPD_Graphics_SetLineDelay(1);
			}
		break;
		case MMP_LDC_RES_MODE_WVGA:
			MMPD_Graphics_SetPixDelay(4, 20);
			MMPD_Graphics_SetLineDelay(1);
		break;
	}
	
	/* For display and self-loopback pipe */
	MMPD_IBC_SetMCI_ByteCount(ldcLink.prvPipeID, MMPF_MCI_BYTECNT_SEL_256BYTE);
	
	/* For encode H264 pipe */
	MMPD_Scaler_SetStopEnable((MMP_SCAL_PIPEID)ldcLink.encPipeID, MMP_SCAL_STOP_SRC_H264, MMP_TRUE);
	
	/* For LDC source pipe */
	MMPD_IBC_SetMCI_ByteCount(ldcLink.srcPipeID, MMPF_MCI_BYTECNT_SEL_256BYTE);
	
	MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_TRUE);

	MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
	MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pipeID);
	MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | LINK_LDC);
	MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
#endif
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_UnLinkPipeToLdc
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_UnLinkPipeToLdc(MMP_IBC_PIPEID pipeID)
{
	MMPF_LDC_UnInitialize();

    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pipeID);
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | UNLINK_LDC);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_LinkPipeToMdtc
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_LinkPipeToMdtc(MMP_IBC_PIPEID pipeID)
{
	MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
	MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pipeID);
	MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | LINK_MDTC);
	MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_UnLinkPipeToMdtc
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_UnLinkPipeToMdtc(MMP_IBC_PIPEID pipeID)
{
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pipeID);
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | UNLINK_MDTC);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_LinkPipeToLdws
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_LinkPipeToLdws(MMP_IBC_PIPEID pipeID)
{
	MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
	MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pipeID);
	MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | LINK_LDWS);
	MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_UnLinkPipeToLdws
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_UnLinkPipeToLdws(MMP_IBC_PIPEID pipeID)
{
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pipeID);
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | UNLINK_LDWS);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_LinkPipeToGra2JPEG
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_LinkPipeToGra2JPEG(MMP_IBC_PIPEID 	    pipeID, 
								     MMP_DISPLAY_WIN_ID     winID, 
									 MMP_DISPLAY_DEV_TYPE   previewDev,
									 MMP_BOOL               bStillCapture,
									 MMP_BOOL               bDisplayEnable)
{
   	MMP_GRAPHICS_BUF_ATTR  bufAttr;

    if (bDisplayEnable)
    {
        bufAttr.usWidth	 = m_FctlAttr[pipeID].grabctl.ulOutEdX - m_FctlAttr[pipeID].grabctl.ulOutStX + 1;
        bufAttr.usHeight = m_FctlAttr[pipeID].grabctl.ulOutEdY - m_FctlAttr[pipeID].grabctl.ulOutStY + 1;
        
        switch (m_FctlAttr[pipeID].colormode) {

    	    case MMP_DISPLAY_COLOR_RGB565:
    	        bufAttr.usLineOffset = bufAttr.usWidth*2;
    	        bufAttr.colordepth	 = MMP_GRAPHICS_COLORDEPTH_16;
    	        break;
    	    case MMP_DISPLAY_COLOR_RGB888:
    	        bufAttr.usLineOffset = bufAttr.usWidth*3;
    	        bufAttr.colordepth 	 = MMP_GRAPHICS_COLORDEPTH_24;
    	        break;
    	    case MMP_DISPLAY_COLOR_YUV422:
    	        bufAttr.usLineOffset = bufAttr.usWidth*2;
    	        bufAttr.colordepth 	 = MMP_GRAPHICS_COLORDEPTH_YUV422_UYVY;
    	        break;
    	    case MMP_DISPLAY_COLOR_YUV420:
    	        bufAttr.usLineOffset = bufAttr.usWidth;
    	        bufAttr.colordepth 	 = MMP_GRAPHICS_COLORDEPTH_YUV420;
    	        break;
    	    case MMP_DISPLAY_COLOR_YUV420_INTERLEAVE:
    	        bufAttr.usLineOffset = bufAttr.usWidth;
                bufAttr.colordepth 	 = MMP_GRAPHICS_COLORDEPTH_YUV420_INTERLEAVE;
    	        break;
        }

    	MMPD_Display_SetWinActive(winID, MMP_FALSE);

        bufAttr.ulBaseAddr  = m_FctlAttr[pipeID].ulBaseAddr[0];
        bufAttr.ulBaseUAddr = m_FctlAttr[pipeID].ulBaseUAddr[0];
        bufAttr.ulBaseVAddr = m_FctlAttr[pipeID].ulBaseVAddr[0];
        MMPD_Display_BindBufToWin(&bufAttr, winID);
    }

    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, pipeID);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 2, previewDev);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 4, winID);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 6, bDisplayEnable);
    
    if (bStillCapture)
        MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | LINK_GRA2STILLJPEG);
    else
        MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_IBC_LINK_MODE | LINK_GRA2MJEPG);

    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Fctl_InitialBuffer
//  Description : Use for re-init the preview buffer.
//------------------------------------------------------------------------------
MMP_ERR MMPD_Fctl_InitialBuffer(MMP_USHORT usIBCPipe) 
{
	return MMPF_Display_InitalBuffer(usIBCPipe);
}
/// @}


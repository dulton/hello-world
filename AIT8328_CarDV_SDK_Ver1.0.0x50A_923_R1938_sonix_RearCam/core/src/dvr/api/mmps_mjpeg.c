//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "ait_utility.h"
#include "mmps_system.h"
#include "mmps_3gprecd.h"
#include "mmps_dsc.h"
#include "mmps_sensor.h"
#include "mmpd_fctl.h"
#include "mmpd_scaler.h"
#include "mmpf_sensor.h"
#include "mmpf_ldc.h"
#include "mmpf_ringbuf.h"

//==============================================================================
//
//                              GLOBAL VARIABLES
//
//==============================================================================

static MMP_ULONG	        m_ulMjpegCaptDramAddr = 0; 
static MMP_ULONG            m_ulMjpegCaptSramAddr = 0;

static MMP_MJPEG_OBJ 	    m_sMjpegStreamObj[MAX_MJPEG_STREAM_NUM];
static MMP_MJPEG_LINKATTR   m_sMjpgLinkAttr[MAX_MJPEG_STREAM_NUM];

//==============================================================================
//
//                              EXTERN VARIABLES
//
//==============================================================================

extern MMP_ULONG        m_ulPrimaryJpegCompStart;
extern MMP_ULONG        m_ulPrimaryJpegCompEnd;

#if (HANDLE_JPEG_EVENT_BY_QUEUE)
extern MMPD_FCTL_ATTR   m_VRPreviewFctlAttr[];
extern MMPD_FCTL_ATTR   m_DSCPreviewFctlAttr;
#endif
#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
extern MMPD_FCTL_ATTR   m_DecMjpegToPrevwFctlAttr;
#endif

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

#if 0
void _____MJPEG_Streaming_Function_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_MJPEG_IsValidObj
//  Description :
//------------------------------------------------------------------------------
static MMP_BOOL MMPS_MJPEG_IsValidObj(MMP_MJPEG_OBJ_PTR pHandle)
{
    if ((pHandle == NULL) || (pHandle->ul4cc != CODEC_MJPG)) {
        return MMP_FALSE;
    }
    return MMP_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_MJPEG_SetCaptureAddr
//  Description : The function saves head memory address to serve Still Capture.
//                Becasue that live streaming of MJPEG is always allocate memory as
//                Video record mode, but if UI is at Capture Mode (Capture preview)
//                and do capture, that memory allocated by Capture mode from FW ending,
//                it will overlay with streaming even streaming is at PAUSE state.
//                The function saves the head address for Capture in streaming.
//                To get the address to allocate buffer to capture in streaming.
//------------------------------------------------------------------------------
MMP_ERR MMPS_MJPEG_SetCaptureAddr(MMP_ULONG ulDramAddr, MMP_ULONG ulSramAddr)
{
    m_ulMjpegCaptDramAddr = ulDramAddr;
    m_ulMjpegCaptSramAddr = ulSramAddr;
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_MJPEG_GetCaptureAddr
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_MJPEG_GetCaptureAddr(MMP_ULONG *pulDram, MMP_ULONG *pulSram)
{
	*pulDram = m_ulMjpegCaptDramAddr;
	*pulSram = m_ulMjpegCaptSramAddr;
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_MJPEG_OpenMjpeg
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_MJPEG_OpenMjpeg(MMP_USHORT usEncID, MMP_USHORT usMode, MMP_MJPEG_OBJ_PTR *ppHandle)
{
    MMP_IBC_PIPEID  ubPipe;

    if ((usEncID != MMP_WIFI_STREAM_FRONTCAM_VIDEO) &&
        (usEncID != MMP_WIFI_STREAM_REARCAM_VIDEO)) {
        return MMP_WIFI_ERR_PARAMETER;
    }

    *ppHandle = NULL;
    
    if (usMode == MMP_WIFI_MODE_VR) {
        if (usEncID == MMP_WIFI_STREAM_FRONTCAM_VIDEO)
            MMPS_3GPRECD_GetPreviewPipe(0/*PRM_SENSOR*/, &ubPipe);
        else if (usEncID == MMP_WIFI_STREAM_REARCAM_VIDEO)
            ubPipe = MMPS_3GPRECD_GetDecMjpegToPreviewPipeId();
        else
            return MMP_WIFI_ERR_PARAMETER;
    }
    else if (usMode == MMP_WIFI_MODE_DSC) {
        if (usEncID == MMP_WIFI_STREAM_FRONTCAM_VIDEO)
            MMPS_DSC_GetPreviewPipe(&ubPipe);
        else
            return MMP_WIFI_ERR_UNDER_DEVELOP;
    }

	MMPD_MJPEG_NewOneJPEG(usEncID);
    
    *ppHandle = &m_sMjpegStreamObj[usEncID];
    
    m_sMjpegStreamObj[usEncID].usEncID   = usEncID;
    m_sMjpegStreamObj[usEncID].ul4cc     = CODEC_MJPG;
    m_sMjpegStreamObj[usEncID].PipeID    = ubPipe;
    m_sMjpegStreamObj[usEncID].pLinkAttr = &m_sMjpgLinkAttr[usEncID];

    MMPD_MJPEG_SetLinkPipe(usEncID, ubPipe);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_MJPEG_StartFrontCamMjpeg
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_MJPEG_StartFrontCamMjpeg(  MMP_UBYTE             ubSnrSel,
                                        MMP_UBYTE             ubMode,
                                        MMP_MJPEG_OBJ_PTR     pHandle, 
							            MMP_MJPEG_ENC_INFO    *pMjpegInfo,
                                        MMP_MJPEG_RATE_CTL    *pRateCtrl)
{
    MMP_ULONG           	ulDramAddr, ulSramAddr;
    MMP_ULONG               ulInWidth, ulInHeight;
    MMP_SCAL_FIT_RANGE   	fitrange;
    MMP_SCAL_GRAB_CTRL 		grabctl;
	MMPD_FCTL_ATTR	        fctlAttr;
	MMPD_FCTL_LINK	        fctllink;
	MMP_ERR 				err;

	if (pHandle == NULL) {
	    return MMP_WIFI_ERR_PARAMETER;
	}

	/* Get Buffer Address */
    if (ubMode == MMP_WIFI_MODE_VR) {
        // Get the reserved buffer address (for Still Capture)
        //MMPS_3GPRECD_GetStillCaptureAddr(&ulSramAddr, &ulDramAddr);//lucas
        MMPS_3GPRECD_GetMjpgStreamAddr(&ulSramAddr, &ulDramAddr);
    }
    else {
        // Get the preview end address
        MMPS_DSC_SetCaptureBuf(NULL, NULL, MMP_FALSE, MMP_FALSE, MMP_DSC_CAPTURE_NO_ROTATE, MMP_TRUE);
        MMPS_DSC_GetCaptureBuf(&ulSramAddr, &ulDramAddr);
    }

	/* Save the memory address that used when Still Capture called in DSC mode */
    MMPS_MJPEG_SetCaptureAddr(ulDramAddr, ulSramAddr);
    
    /* Set JPEG Parameters */
    MMPS_DSC_SetShotMode(MMPS_DSC_SINGLE_SHOTMODE);
    MMPS_DSC_SetJpegEncParam(DSC_RESOL_IDX_UNDEF, pMjpegInfo->usEncWidth, pMjpegInfo->usEncHeight, MMP_DSC_JPEG_RC_ID_MJPEG_1ST_STREAM);
    MMPS_DSC_ConfigThumbnail(0, 0, MMP_DSC_THUMB_SRC_NONE);
	#if (HANDLE_JPEG_EVENT_BY_QUEUE)
	MMPS_DSC_SetCaptureJpegQualityEx(MMP_DSC_JPEG_RC_ID_MJPEG_1ST_STREAM,
                                     pMjpegInfo->bTargetCtl, pMjpegInfo->bLimitCtl, pMjpegInfo->bTargetSize,
            					     pMjpegInfo->bLimitSize, pMjpegInfo->bMaxTrialCnt, pMjpegInfo->Quality);
	#else
	MMPS_DSC_SetCaptureJpegQuality(MMP_DSC_JPEG_RC_ID_MJPEG_1ST_STREAM,
                                   pMjpegInfo->bTargetCtl, pMjpegInfo->bLimitCtl, pMjpegInfo->bTargetSize,
            					   pMjpegInfo->bLimitSize, pMjpegInfo->bMaxTrialCnt, pMjpegInfo->Quality);	
	#endif
    MMPS_DSC_SetCaptureBuf(&ulSramAddr, &ulDramAddr, MMP_TRUE, MMP_FALSE ,MMP_DSC_CAPTURE_NO_ROTATE, MMP_TRUE);

    /* Set Pipeline Parameters */
    MMPS_Sensor_GetCurPrevScalInputRes(ubSnrSel, &ulInWidth, &ulInHeight);

	fitrange.fitmode    	= MMP_SCAL_FITMODE_OUT;
	fitrange.scalerType     = MMP_SCAL_TYPE_SCALER;
    
    #if (SUPPORT_LDC_RECD)
    if (ubMode == MMP_WIFI_MODE_VR && (MMPS_3GPRECD_GetConfig()->previewpath[0] == MMP_3GP_PATH_LDC_LB_SINGLE)) {
        MMPS_3GPRECD_GetLdcMaxRes(&fitrange.ulInWidth, &fitrange.ulInHeight);
    }
    else {
        fitrange.ulInWidth  = ulInWidth;
        fitrange.ulInHeight = ulInHeight;
    }
    #else
    fitrange.ulInWidth      = ulInWidth;
    fitrange.ulInHeight     = ulInHeight;
    #endif
    
    fitrange.ulOutWidth	    = pMjpegInfo->usEncWidth;
    fitrange.ulOutHeight	= pMjpegInfo->usEncHeight;

    fitrange.ulInGrabX  	= 1;
    fitrange.ulInGrabY  	= 1;
    fitrange.ulInGrabW  	= fitrange.ulInWidth;
    fitrange.ulInGrabH  	= fitrange.ulInHeight;

    MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);

    if (ubMode == MMP_WIFI_MODE_VR) {
    	if (MMPS_3GPRECD_GetConfig()->previewpath[0] == MMP_3GP_PATH_LDC_LB_SINGLE) {
    		pHandle->pLinkAttr->scalerSrc = MMP_SCAL_SOURCE_LDC;
        }
    	else {
    		pHandle->pLinkAttr->scalerSrc = MMP_SCAL_SOURCE_ISP;
        }
    }
    else {
        pHandle->pLinkAttr->scalerSrc = MMP_SCAL_SOURCE_ISP;
    }

	FCTL_PIPE_TO_LINK(pHandle->PipeID, fctllink);

	/* Back up the Scaler and IBC setting and close the IBC of preview path */
	MMPD_Scaler_BackupAttributes(pHandle->PipeID);
	
	MMPD_IBC_ClearFrameEnd(pHandle->PipeID);
	MMPD_IBC_CheckFrameEnd(pHandle->PipeID);
    MMPD_IBC_SetInterruptEnable(pHandle->PipeID, MMP_IBC_EVENT_FRM_END, MMP_FALSE);
    MMPD_IBC_SetStoreEnable(pHandle->PipeID, MMP_FALSE);
	// IBC can't sync with ISR, wait 40 msec for IBC store done.
    MMPF_OS_Sleep(40);
	MMPD_Fctl_InitialBuffer(pHandle->PipeID);

	MMPD_Fctl_GetIBCLinkAttr(pHandle->PipeID, &(pHandle->pLinkAttr->IBCLinkType), &(pHandle->pLinkAttr->previewDev), &(pHandle->pLinkAttr->winID), &(pHandle->pLinkAttr->rotateDir));
    MMPD_Fctl_ResetIBCLinkType(pHandle->PipeID);

	pHandle->pLinkAttr->pipeAttr.function = MMP_IBC_FX_TOFB;
	MMPD_IBC_GetAttributes(pHandle->PipeID, &(pHandle->pLinkAttr->pipeAttr));

	/* Change the path to LDC/JPEG in order to keep the setting of pipe normal */
	#if (HANDLE_JPEG_EVENT_BY_QUEUE)
	MMPD_Scaler_SetPath(pHandle->PipeID, MMP_SCAL_SOURCE_LDC, MMP_TRUE);

	if (ubMode == MMP_WIFI_MODE_VR) {
	    fctlAttr = m_VRPreviewFctlAttr[0];

	}
	else {
	    fctlAttr = m_DSCPreviewFctlAttr;
	}
	#else
    MMPD_Scaler_SetPath(pHandle->PipeID, MMP_SCAL_SOURCE_JPG, MMP_TRUE);
	#endif
	
    fctlAttr.fctllink       = fctllink;
    fctlAttr.fitrange       = fitrange;
    fctlAttr.grabctl        = grabctl;
    fctlAttr.scalsrc		= pHandle->pLinkAttr->scalerSrc;
    fctlAttr.bSetScalerSrc	= MMP_FALSE;
    
    #if (HANDLE_JPEG_EVENT_BY_QUEUE)
    // Fix WIFI have problem.
    fctlAttr.colormode = MMP_DISPLAY_COLOR_RGB565;
    MMPD_Fctl_SetPipeAttrForIbcFB(&fctlAttr);

    MMPD_Fctl_LinkPipeToGra2JPEG(pHandle->PipeID,
                                 pHandle->pLinkAttr->winID,
                                 pHandle->pLinkAttr->previewDev,
                                 MMP_FALSE,
                                 MMP_FALSE);
                                 
    MMPD_DSC_SetDualStreamingEnable(MMP_TRUE, MMP_FALSE);
    #else
    MMPD_Fctl_SetPipeAttrForJpeg(&fctlAttr, MMP_TRUE, MMP_FALSE);
	
	MMPD_DSC_SetCapturePath(pHandle->PipeID, pHandle->PipeID, pHandle->PipeID);
    #endif
	
	if (pRateCtrl != NULL) {
	    MMPD_MJPEG_SetFPS(MMP_WIFI_STREAM_FRONTCAM_VIDEO, pRateCtrl->FPSx10, 10);
	} 
	else {
        MMPD_MJPEG_SetFPS(MMP_WIFI_STREAM_FRONTCAM_VIDEO, 300, 10);
	}

	/* Change the path to the frame source */
	MMPD_IBC_ClearFrameEnd(pHandle->PipeID);
    MMPD_Scaler_SetPath(pHandle->PipeID, pHandle->pLinkAttr->scalerSrc, MMP_TRUE);
	MMPD_IBC_CheckFrameEnd(pHandle->PipeID);
    
    /* Enable Streaming */
	MMPD_Streaming_SetCompBuf(VIDEO1_RING_STREAM, m_ulPrimaryJpegCompStart,
	                          m_ulPrimaryJpegCompEnd - m_ulPrimaryJpegCompStart);
	MMPD_Streaming_Enable(VIDEO1_RING_STREAM, MMP_TRUE);

    MMPD_DSC_SetMJPEGPipe(pHandle->PipeID);
	MMPD_DSC_StartMJPEGStream(PRM_SENSOR);

    /* Enable IBC store for trigger loop-back JPEG encode */
	#if (HANDLE_JPEG_EVENT_BY_QUEUE)
	MMPD_IBC_SetStoreEnable(pHandle->PipeID, MMP_TRUE);
	#endif

    if (err != MMP_ERR_NONE) {
        return MMP_WIFI_ERR_PARAMETER;
    }

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_MJPEG_StartRearCamMjpeg
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_MJPEG_StartRearCamMjpeg(   MMP_UBYTE             ubMode,
                                        MMP_MJPEG_OBJ_PTR     pHandle, 
							            MMP_MJPEG_ENC_INFO    *pMjpegInfo,
                                        MMP_MJPEG_RATE_CTL    *pRateCtrl)
{
#if (HANDLE_JPEG_EVENT_BY_QUEUE) && (REAR_CAM_TYPE == REAR_CAM_TYPE_SONIX_MJPEG2H264)
    MMP_ULONG           	ulCompAddr, ulCompSize, ulLineBuf;
    MMP_SCAL_FIT_RANGE   	fitrange;
    MMP_SCAL_GRAB_CTRL 		grabctl;
	MMPD_FCTL_ATTR	        fctlAttr;
	MMPD_FCTL_LINK	        fctllink;
	MMP_ERR 				err;

	if (pHandle == NULL) {
	    return MMP_WIFI_ERR_PARAMETER;
	}

    /* Get the Reserved Buffer Information */
	MMPD_DSC_GetRearCamBufForDualStreaming(&ulCompAddr, &ulCompSize, &ulLineBuf);
    
    /* Set JPEG Parameters */
    MMPS_DSC_SetJpegEncParam(DSC_RESOL_IDX_UNDEF, pMjpegInfo->usEncWidth, pMjpegInfo->usEncHeight, MMP_DSC_JPEG_RC_ID_MJPEG_2ND_STREAM);
    MMPS_DSC_ConfigThumbnail(0, 0, MMP_DSC_THUMB_SRC_NONE);
	#if 1
    MMPS_DSC_SetCaptureJpegQualityEx(MMP_DSC_JPEG_RC_ID_MJPEG_2ND_STREAM,
                                   pMjpegInfo->bTargetCtl, pMjpegInfo->bLimitCtl, pMjpegInfo->bTargetSize,
            					   pMjpegInfo->bLimitSize, pMjpegInfo->bMaxTrialCnt, pMjpegInfo->Quality);
	#else
    MMPS_DSC_SetCaptureJpegQuality(MMP_DSC_JPEG_RC_ID_MJPEG_2ND_STREAM,
                                   pMjpegInfo->bTargetCtl, pMjpegInfo->bLimitCtl, pMjpegInfo->bTargetSize,
            					   pMjpegInfo->bLimitSize, pMjpegInfo->bMaxTrialCnt, pMjpegInfo->Quality);
	#endif
    /* Set Pipeline Parameters */
	MMPS_3GPRECD_GetDecMjpegToPreviewSrcAttr((MMP_USHORT*)&fitrange.ulInWidth, (MMP_USHORT*)&fitrange.ulInHeight);

	fitrange.fitmode    	= MMP_SCAL_FITMODE_OUT;
	fitrange.scalerType     = MMP_SCAL_TYPE_SCALER;
    fitrange.ulOutWidth	    = pMjpegInfo->usEncWidth;
    fitrange.ulOutHeight	= pMjpegInfo->usEncHeight;

    fitrange.ulInGrabX  	= 1;
    fitrange.ulInGrabY  	= 1;
    fitrange.ulInGrabW  	= fitrange.ulInWidth;
    fitrange.ulInGrabH  	= fitrange.ulInHeight;

    MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);

	pHandle->pLinkAttr->scalerSrc = MMP_SCAL_SOURCE_JPG;

	FCTL_PIPE_TO_LINK(pHandle->PipeID, fctllink);

	/* Back up the Scaler and IBC setting and close the IBC of preview path */
	MMPD_Scaler_BackupAttributes(pHandle->PipeID);

	MMPD_IBC_ClearFrameEnd(pHandle->PipeID);
	MMPD_IBC_CheckFrameEnd(pHandle->PipeID);
    MMPD_IBC_SetInterruptEnable(pHandle->PipeID, MMP_IBC_EVENT_FRM_END, MMP_FALSE);
    MMPD_IBC_SetStoreEnable(pHandle->PipeID, MMP_FALSE);
	// IBC can't sync with ISR, wait 40 msec for IBC store done.
    MMPF_OS_Sleep(40);
	MMPD_Fctl_InitialBuffer(pHandle->PipeID);
	
	MMPD_Fctl_GetIBCLinkAttr(pHandle->PipeID, &(pHandle->pLinkAttr->IBCLinkType), &(pHandle->pLinkAttr->previewDev), &(pHandle->pLinkAttr->winID), &(pHandle->pLinkAttr->rotateDir));
    MMPD_Fctl_ResetIBCLinkType(pHandle->PipeID);

	pHandle->pLinkAttr->pipeAttr.function = MMP_IBC_FX_TOFB;
	MMPD_IBC_GetAttributes(pHandle->PipeID, &(pHandle->pLinkAttr->pipeAttr));

	/* Change the path to LDC in order to keep the setting of pipe normal. */
	MMPD_Scaler_SetPath(pHandle->PipeID, MMP_SCAL_SOURCE_LDC, MMP_TRUE);
	
	fctlAttr = m_DecMjpegToPrevwFctlAttr;

    fctlAttr.fctllink       = fctllink;
    fctlAttr.fitrange       = fitrange;
    fctlAttr.grabctl        = grabctl;
    fctlAttr.scalsrc		= pHandle->pLinkAttr->scalerSrc;
    fctlAttr.bSetScalerSrc	= MMP_FALSE;
   
    MMPD_Fctl_SetPipeAttrForIbcFB(&fctlAttr);

    MMPD_Fctl_LinkPipeToGra2JPEG(pHandle->PipeID,
                                 pHandle->pLinkAttr->winID,
                                 pHandle->pLinkAttr->previewDev,
                                 MMP_FALSE,
                                 MMP_FALSE); 
    
    MMPD_DSC_SetDualStreamingEnable(MMP_TRUE, MMP_TRUE);

	if (pRateCtrl != NULL) {
	    MMPD_MJPEG_SetFPS(MMP_WIFI_STREAM_REARCAM_VIDEO, pRateCtrl->FPSx10, 10);
	}
	else {
        MMPD_MJPEG_SetFPS(MMP_WIFI_STREAM_REARCAM_VIDEO, 300, 10);
	}

    /* Enable IBC store for trigger loop-back JPEG encode */
	MMPD_IBC_SetStoreEnable(pHandle->PipeID, MMP_TRUE);

    /* Enable Streaming */
	MMPD_Streaming_SetCompBuf(VIDEO2_RING_STREAM, ulCompAddr, ulCompSize);
	MMPD_Streaming_Enable(VIDEO2_RING_STREAM, MMP_TRUE);

    if (err != MMP_ERR_NONE) {
        return MMP_WIFI_ERR_PARAMETER;
    }
#endif
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_MJPEG_StopMjpeg
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_MJPEG_StopMjpeg(MMP_MJPEG_OBJ_PTR pHandle)
{
    MMP_BOOL bStreamEnable = MMP_FALSE;

    if (MMPS_MJPEG_IsValidObj(pHandle)) {
        
        if (pHandle->usEncID == MMP_WIFI_STREAM_FRONTCAM_VIDEO)
            bStreamEnable = MMPF_StreamRing_IsEnabled(VIDEO1_RING_STREAM);
        else if (pHandle->usEncID == MMP_WIFI_STREAM_REARCAM_VIDEO)
            bStreamEnable = MMPF_StreamRing_IsEnabled(VIDEO2_RING_STREAM);  
        
        if (bStreamEnable) {
            MMPD_Streaming_Enable(pHandle->usEncID, MMP_FALSE);
            
            MMPD_MJPEG_CheckEncode(pHandle->usEncID);

            if (pHandle->usEncID == MMP_WIFI_STREAM_FRONTCAM_VIDEO) {
                MMPD_DSC_StopMJPEGStream(pHandle->usEncID);
            }
        }
    } 
    else {
        return MMP_WIFI_ERR_PARAMETER;
    }
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_MJPEG_CloseMjpeg
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_MJPEG_CloseMjpeg(MMP_MJPEG_OBJ_PTR* ppHandle)
{
    MMP_ERR err = MMP_ERR_NONE;
    
    err = MMPS_MJPEG_StopMjpeg(*ppHandle);
    
    if (err == MMP_ERR_NONE) {
        (*ppHandle)->usEncID 	= MAX_MJPEG_STREAM_NUM;
        (*ppHandle)->PipeID     = MMP_IBC_PIPE_MAX;
        (*ppHandle)->pLinkAttr 	= NULL;
        (*ppHandle) 			= NULL;
    }
    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_MJPEG_Return2Display
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_MJPEG_Return2Display(MMP_MJPEG_OBJ_PTR pHandle)
{
    if (!MMPS_MJPEG_IsValidObj(pHandle)) {
        return MMP_WIFI_ERR_PARAMETER;
    }
    /* Disable scaler because it might still previewing or recording */
    MMPD_Scaler_SetEnable(pHandle->PipeID, MMP_FALSE);
    /* Change the path to JPEG in order to keep the setting of scaler normal */
    #if (HANDLE_JPEG_EVENT_BY_QUEUE)
    MMPD_Scaler_SetPath(pHandle->PipeID, MMP_SCAL_SOURCE_LDC, MMP_TRUE);
    #else
    MMPD_Scaler_SetPath(pHandle->PipeID, MMP_SCAL_SOURCE_JPG, MMP_TRUE);
	#endif
	
	/* Restore the setting of Scaler */
	if (pHandle->usEncID == MMP_WIFI_STREAM_FRONTCAM_VIDEO) {
	
#if (HANDLE_JPEG_EVENT_BY_QUEUE)
		// Fix WIFI preview -> LCD preview have white line issue.   
	    MMPD_IBC_SetStoreEnable(pHandle->PipeID, MMP_FALSE);
	    // IBC can't sync with ISR, wait 40 msec for IBC store done.
		MMPF_OS_Sleep (40);
		MMPD_Fctl_InitialBuffer(pHandle->PipeID);
        MMPD_Fctl_SetPipeAttrForIbcFB(&m_VRPreviewFctlAttr[0]);
#else
        MMPD_Scaler_RestoreAttributes(pHandle->PipeID);
        MMPD_IBC_SetAttributes(pHandle->PipeID, &pHandle->pLinkAttr->pipeAttr);	
#endif
	}
	else if (pHandle->usEncID == MMP_WIFI_STREAM_REARCAM_VIDEO) {
	    // For restore gsPreviewBufWidth and gsPreviewBufHeight. (TBD)
        #if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
	    MMPD_Fctl_SetPipeAttrForIbcFB(&m_DecMjpegToPrevwFctlAttr);
	    #endif
	}
	/* Restore the setting of IBC */
    MMPD_Fctl_RestoreIBCLinkAttr(pHandle->PipeID, 
    							 pHandle->pLinkAttr->IBCLinkType, 
    							 pHandle->pLinkAttr->previewDev, 
    							 pHandle->pLinkAttr->winID,
    							 pHandle->pLinkAttr->rotateDir);

    MMPD_IBC_ClearFrameEnd(pHandle->PipeID);
    MMPD_Scaler_SetPath(pHandle->PipeID, pHandle->pLinkAttr->scalerSrc, MMP_TRUE);
    MMPD_Scaler_SetEnable(pHandle->PipeID, MMP_TRUE);
	MMPD_IBC_CheckFrameEnd(pHandle->PipeID);
    MMPD_IBC_SetInterruptEnable(pHandle->PipeID, MMP_IBC_EVENT_FRM_END, MMP_TRUE);
    MMPD_IBC_SetStoreEnable(pHandle->PipeID, MMP_TRUE);

    return MMP_ERR_NONE;
}

//==============================================================================
//
//  File        : mmps_dsc.c
//  Description : Ritian DSC Control driver function
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

/**
 *  @file mmps_dsc.c
 *  @brief The DSC control functions
 *  @author Penguin Torng
 *  @version 1.0
 */

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "includes_fw.h"
#include "ait_utility.h"
#include "ptz_cfg.h"
#include "mmp_media_def.h"
#include "mmp_icon_inc.h"
#include "mmps_system.h"
#include "mmps_dsc.h"
#include "mmps_3gprecd.h"
#include "mmps_sensor.h"
#include "mmps_fs.h"
#include "mmps_iva.h"
#include "mmpd_fctl.h"
#include "mmpd_ptz.h"
#include "mmpd_system.h"
#include "mmpd_rawproc.h"
#include "mmpd_bayerscaler.h"
#include "mmpd_dma.h"
#include "mmpd_ccir.h"
#include "mmpd_scaler.h"
#include "mmpf_scaler.h"
#include "mmpf_exif.h"
#include "mmpf_ldc.h"
#include "mmpf_sensor.h"
#include "mmpf_streaming.h"
#include "mmpf_ringbuf.h"
#include "mmpf_display.h"

#if (SUPPORT_VR_THUMBNAIL == 1)
#include "mmpd_3gpmgr.h"
#endif

#if (HANDLE_JPEG_EVENT_BY_QUEUE)
#include "mmpf_ibc.h"
#endif
/** @addtogroup MMPS_DSC
@{
*/
#if defined(ALL_FW)
//==============================================================================
//
//                              INTERNAL STRUCTURES
//
//==============================================================================

#if (VIDEO_DEC_TO_MJPEG)
static struct MMPS_TRANSCODE_CONFIG {
	MMP_ULONG 				InFpsx10;
	MMP_ULONG 				OutFpsx10;
	MMP_ULONG 				ulBitrate;
} m_transcode;
#endif

//==============================================================================
//
//                              GLOBAL VARIABLES
//
//==============================================================================

/**@brief The dsc configuration

Use @ref MMPS_DSC_GetConfig to assign the field value of it.
You should read this functions for more information.
*/
static MMPS_DSC_CONFIG				m_dscConfig;

/**@brief The dsc preview mode.
         
Use @ref MMPS_DSC_SetPreviewMode to set it.
It will be referred when accessing preview function.

For example	:
	DSC Mode0: For 240 X 180 to LCD (Normal Mode)
	DSC Mode1: For 240 X 320 full screen to LCD (FullScreen Mode)
	DSC Mode2: For 720 X 480 to TV (TV Mode)
*/
static MMP_USHORT  					m_usDscPreviewMode = 0;

static MMP_UBYTE  					m_ubDscModeSnrId = PRM_SENSOR;

/**@brief The dsc decode mode.
         
Use @ref MMPS_DSC_SetPlaybackMode to set it.
It will be referred when accessing decode function.

For example :
	DSC_NORMAL_DECODE_MODE      0
	DSC_FULL_DECODE_MODE        1
	DSC_TV_NTSC_DECODE_MODE     2
	DSC_TV_PAL_DECODE_MODE      3
	DSC_HDMI_DECODE_MODE    	4
	DSC_CCIR_DECODE_MODE    	5
*/
static MMP_USHORT					m_usDecodeMode = 0;

/**@brief The dsc capture mode.

For example:
	MMPS_DSC_SINGLE_SHOTMODE		0
    MMPS_DSC_MULTI_SHOTMODE			1
	MMPS_DSC_CONTINUOUS_SHOTMODE	2
	
Use @ref MMPS_DSC_SetShotMode to set it.
It will be referred when accessing capture function.
*/
static MMP_USHORT					m_usCaptureMode = MMPS_DSC_SINGLE_SHOTMODE;

/**@brief The dsc capture JPEG width.
         
Use @ref MMPS_DSC_SetJpegEncParam to set it.
It will be referred when accessing preview and capture function.
*/
static MMP_USHORT 					m_usJpegWidth   = 320;
static MMP_USHORT 					m_usJpegHeight  = 240;
static MMP_BOOL 					m_bScaleUpCapture = MMP_FALSE;

/**@brief The Motion JPEG width / height.
         
Use @ref MMPS_DSC_SetMJpegResol to set it.
*/
MMP_USHORT 							m_usMJpegWidth  = 1920;
MMP_USHORT 							m_usMJpegHeight = 1080;

/**@brief The Video to Motion JPEG enable.
         
Use @ref MMPS_DSC_SetVideo2MJpegEnable to set it.
*/
#if (VIDEO_DEC_TO_MJPEG)
static MMP_BOOL						m_bVideo2MJpegEnable = MMP_FALSE;
#endif

/**@brief The dsc current preview On/Off.
         
Use @ref MMPS_DSC_EnablePreviewDisplay to change it state.
And use @ref MMPS_DSC_GetPreviewStatus to get the value
It will be referred when accessing preview function.
*/
static MMP_BOOL						m_bDscPreviewActive = MMP_FALSE;

/**@brief The playback information for one picture
         
Use @ref MMPS_DSC_PlaybackJpeg will set the initial value. 
*/
static MMPS_DSC_PLAYBACK_INFO		m_JpegPlayInfo;

/**@brief The preview zoom information
         
Use @ref MMPS_DSC_InitPreview will set the initial value. 
It will be adjusted or referred by @ref MMPS_DSC_SetPreviewZoom.
*/
static MMPS_DSC_ZOOM_INFO			m_DscPreviewZoomInfo;
static MMP_USHORT                 	m_usDSCStaticZoomIndex = 0;

/**@brief The preview memory end address 
        
Use @ref MMPS_DSC_InitPreview to set the memory end address. 
*/
static MMP_ULONG   					m_ulDSCPreviewEndAddr;

/**@brief The line / compress buffer address.
        
Use @ref MMPS_DSC_SetCaptureBuf to set it. 
*/
MMP_ULONG	 				        m_ulPrimaryJpegLineStart;
MMP_ULONG 					        m_ulPrimaryJpegCompStart;				    // Capture buffer start address
MMP_ULONG 					        m_ulPrimaryJpegCompEnd;					    // Capture buffer end address

#if (DSC_SUPPORT_BASELINE_MP_FILE)
static MMP_BOOL						m_bMultiPicFormatEnable	= MMP_FALSE;		// Enable Multi-Picture Format.
static MMP_BOOL						m_bEncLargeThumbEnable	= MMP_FALSE;		// Enable encode large thumbnail.
static MMP_ULONG 					m_ulLargeThumbWidth 	= 1920;	
static MMP_ULONG 					m_ulLargeThumbHeight 	= 1080;
static MMP_UBYTE					m_ubLargeThumbResMode 	= MMP_DSC_LARGE_THUMB_RES_FHD;
static MMP_ULONG 					m_ulLargeThumbJpegCompStart;				// Capture buffer start address for large thumbnail 
static MMP_ULONG 					m_ulLargeThumbJpegCompEnd;					// Capture buffer end address for large thumbnail
static MMP_ULONG 					m_ulPrimaryJpeg4LargeThumbDecodeOutStart;	// The start address of the buffer which store decoded primary jpeg for large thumbnail
static MMP_ULONG 					m_ulPrimaryJpeg4LargeThumbDecodeOutSize;	// The size of decoded primary jpeg for large thumbnail
#endif

/* For MMP_DSC_THUMB_SRC_DECODED_JPEG mode use */
static MMP_ULONG 					m_ulPrimaryJpeg4ExifThumbDecodeOutStart;	// The start address of the buffer which store decoded primary jpeg for thumbnail
static MMP_ULONG 					m_ulPrimaryJpeg4ExifThumbDecodeOutSize;		// The size of decoded primary jpeg for thumbnail
static MMP_ULONG 					m_ulBlackThumbnailDecodeOutStart;
static MMP_ULONG 					m_ulBlackThumbnailDecodeOutSize;

/**@brief The rotate capture attributes.
For encode thumbnail when rotate capture
*/
static MMP_DSC_CAPTURE_ROTATE_TYPE 	m_CaptureRotateType = MMP_DSC_CAPTURE_NO_ROTATE;

/**	@brief	m_ulSubJpegExtCompStart is the second address for taking stable shot 
			because the implementation of taking stable shot is to take two shot.
			Compare the first shot with the second shot by the size of compressed data. */
static MMP_ULONG 					m_ulSubJpegExtCompStart;

/**@brief The preview attributes. 
*/
MMPD_FCTL_ATTR   			        m_DSCPreviewFctlAttr;

/**@brief The multi-shot attributes. 
*/
//static MMPS_DSC_MULTISHOT_INFO 		m_MultiShotInfo;
MMPS_DSC_MULTISHOT_INFO 		m_MultiShotInfo;

/** @brief we use preview mode with rawproc, and its color depth of rawproc.
*/
static MMP_RAW_COLORFMT			    m_DscRawColorDepth			= MMP_RAW_COLORFMT_BAYER8;
static MMP_DSC_PREVIEW_PATH 		m_DscPreviewPath			= MMP_DSC_STD_PREVIEW;

/* For rate control */
static MMP_BOOL                     m_bUseCustomJpegQT 			= MMP_FALSE;
static MMP_BOOL                     m_bUseCustomThumbQT 		= MMP_FALSE;
static MMP_UBYTE                    m_ubCustomJpegQT[3][DSC_QTABLE_ARRAY_SIZE];
static MMP_UBYTE                    m_ubCustomThumbQT[3][DSC_QTABLE_ARRAY_SIZE];
static MMP_ULONG                    m_ulImageTargetSize			= 0;

/* For sticker */
static MMP_BOOL                     m_bEnablePrimaryJpegSticker = MMP_FALSE;
static MMP_BOOL                     m_bEnableThumbnailSticker 	= MMP_FALSE;
static MMP_UBYTE                	m_ubPrimaryJpegStickerId    = 0;
static MMP_UBYTE                 	m_ubThumbnailStickerId		= 1;

/* Thumbnail / Preview / Capture / Playback pipe selection */
static  MMPD_FCTL_LINK 				m_fctlLinkPlayback 			= {MMP_SCAL_PIPE_0, MMP_ICO_PIPE_0, MMP_IBC_PIPE_0};
static  MMPD_FCTL_LINK 				m_fctlLinkCapture 			= {MMP_SCAL_PIPE_0,	MMP_ICO_PIPE_0, MMP_IBC_PIPE_0};
static  MMPD_FCTL_LINK 				m_fctlLinkPreview 			= {MMP_SCAL_PIPE_1, MMP_ICO_PIPE_1, MMP_IBC_PIPE_1};
static  MMPD_FCTL_LINK 				m_fctlLinkThumb 			= {MMP_SCAL_PIPE_2, MMP_ICO_PIPE_2, MMP_IBC_PIPE_2};
#if (SUPPORT_LDC_CAPTURE)
static MMPD_FCTL_LINK				m_fctlLinkLdcSrc 			= {MMP_SCAL_PIPE_3, MMP_ICO_PIPE_3, MMP_IBC_PIPE_3};
#endif

/* LDC attribute */
#if (SUPPORT_LDC_CAPTURE)
static MMP_LDC_SRC_BUFINFO			m_sDscLdcSrcBuf;
MMP_ULONG							m_ulDscLdcMaxSrcW 			= 1920;
MMP_ULONG							m_ulDscLdcMaxSrcH 			= 1080;
MMP_BOOL							m_bDscLdcEnable				= MMP_FALSE;
#endif

/* For AHC layer use */
static MMPS_DSC_AHC_PREVIEW_INFO	m_sAhcDscPrevInfo;
static MMPS_DSC_AHC_PLAYBACK_INFO	m_sAhcDscPlayInfo;
static MMP_BOOL     				m_bAhcConfigDscZoom         = MMP_FALSE;

/* For playback attribute */
static MMP_ULONG    				m_ulDSCCurMemAddr;
static MMP_DISPLAY_CONTROLLER 		m_DscPlaybackDispCtrl		= MMP_DISPLAY_PRM_CTL;
static MMP_DISPLAY_WIN_ID			m_DscPlaybackWinId			= MMP_DISPLAY_WIN_PIP;

/* For Thumbnail */
static MMP_ULONG    				m_ulDSCThumbWidth           = 160;
static MMP_ULONG    				m_ulDSCThumbHeight          = 120;
static MMP_DSC_THUMB_INPUT_SRC		m_thumbnailInputMode 		= MMP_DSC_THUMB_SRC_DECODED_JPEG;
static volatile MMP_BOOL 			m_bDMADone					= MMP_FALSE;

/* For Ring buffer */
static MMP_ULONG					m_ulMaxRingBufferSize		= 10 * 1024 * 1024;
static MMP_ULONG                    m_ulCompRingBufferAddr  	= 0;

#if (EXIF_MANUAL_UPDATE)
static MMP_UBYTE                    m_ubExifSettingArray[EXIF_CONFIG_ARRAY_SIZE] = {0};
#endif

/* For Callback */
MMP_DSC_CALLBACK  					*MMP_DSC_StartCaptureCallback 	= NULL;
MMP_DSC_CALLBACK  					*MMP_DSC_StartStoreCardCallback = NULL;
MMP_DSC_CALLBACK  					*MMP_DSC_EndStoreCardCallback 	= NULL;
MMP_DSC_CALLBACK  			        *MMP_DSC_ErrorStoreCardCallback = NULL;

/* For Conti-Shot */
static MMP_ULONG 					m_ulContiShotInterval 		= 0; //ms
static MMP_ULONG 					m_ulContiShotMaxNum 		= 10;
static MMP_ULONG					m_ulDSCMaxDramAddr			= 0;

/* For playback in CCIR output mode */
#define DSC_CCIR_DISP_BUF_CNT       (2)
static MMP_ULONG                    m_ulCcirDispBuf[DSC_CCIR_DISP_BUF_CNT];
static MMP_UBYTE                    m_ulCcirDispBufIdx          = 0;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

extern MMP_USHORT Greatest_Common_Divisor(MMP_USHORT a, MMP_USHORT b);

static MMP_ERR MMPS_DSC_InitDigitalZoomParam(void);
static MMP_ERR MMPS_DSC_RestoreDigitalZoomRange(void);
static MMP_ERR MMPS_DSC_EncodeThumbnail(MMPS_DSC_CAPTURE_INFO 	*pCaptureInfo,
										MMP_DSC_JPEG_INFO 		*pJpegInfo,
										MMP_ULONG 				ulThumbWidth, 
										MMP_ULONG 				ulThumbHeight,
										MMP_ULONG 				*pThumbnailSize,
										MMP_DSC_THUMB_INPUT_SRC	ubThumbnailInputMode,
										MMP_SCAL_PIPEID 		pipeID);

#if (DSC_SUPPORT_BASELINE_MP_FILE)
static MMP_ERR MMPS_DSC_EncodeLargeThumb(MMPS_DSC_CAPTURE_INFO   	*pCaptureInfo,
										 MMP_DSC_JPEG_INFO       	*pJpegInfo,
										 MMP_ULONG               	ThumbWidth, 
										 MMP_ULONG               	ThumbHeight, 
										 MMP_ULONG               	*pThumbnailSize,
										 MMP_DSC_THUMB_INPUT_SRC	ubThumbnailInputMode,
										 MMP_SCAL_PIPEID        	pipeID);
#endif

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

#if 0		 
void _____Config_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_GetConfig
//  Description :
//------------------------------------------------------------------------------
/** @brief Get the confuguration for the DSC function, including parameters for preview/capture/zoom and playback.

@return It reports configuration of the DSC function.
*/
MMPS_DSC_CONFIG* MMPS_DSC_GetConfig(void)
{
    return &m_dscConfig;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetPreviewMode
//  Description :
//------------------------------------------------------------------------------
/** @brief Configure DSC preview mode.

The function configures the DSC preview mode.
@param[in] usMode the DSC preview mode
@return It reports the status of the mode config.
*/
MMP_ERR MMPS_DSC_SetPreviewMode(MMP_USHORT usMode)
{
	MMP_ERR error = MMP_ERR_NONE;
	    
    m_usDscPreviewMode = usMode;
    
	error = MMPS_DSC_SetSystemBuf(NULL, MMP_TRUE, MMP_TRUE, MMP_TRUE);
	
	return error;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_ResetPreviewMode
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_ResetPreviewMode(void)
{
	m_usDscPreviewMode = 0xFFFF;
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_GetPreviewMode
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_GetPreviewMode(MMP_USHORT *usMode)
{
    *usMode = m_usDscPreviewMode;
    
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetCallbackFunc
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_SetCallbackFunc(MMP_ULONG ulEvent, void *pCallback)
{
    switch(ulEvent) {
    	case MMP_DSC_CALLBACK_CAPTURE_START:
    		MMP_DSC_StartCaptureCallback 	= (MMP_DSC_CALLBACK *)pCallback;
    	break;
    	case MMP_DSC_CALLBACK_STORECARD_START:
    		MMP_DSC_StartStoreCardCallback 	= (MMP_DSC_CALLBACK *)pCallback;
    	break;    
    	case MMP_DSC_CALLBACK_STORECARD_END:
    		MMP_DSC_EndStoreCardCallback 	= (MMP_DSC_CALLBACK *)pCallback;
    	break;
    	case MMP_DSC_CALLBACK_STORECARD_ERROR:
    		MMP_DSC_ErrorStoreCardCallback 	= (MMP_DSC_CALLBACK *)pCallback;
    	break;
    	default:
    	break;
    }
    
	return MMP_ERR_NONE;
}

#if (SUPPORT_LDC_CAPTURE)
//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetLdcRunMode
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_SetLdcRunMode(MMP_UBYTE ubRunMode)
{
	MMPF_LDC_SetRunMode(ubRunMode);
	
	if (ubRunMode == MMP_LDC_RUN_MODE_MULTI_RUN) {
		MMPF_LDC_MultiRunSetLoopBackCount(MMPF_LDC_MultiRunGetMaxLoopBackCount());
	}
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetLdcResMode
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_SetLdcResMode(MMP_UBYTE ubResMode, MMP_UBYTE ubFpsMode)
{	
	switch(ubResMode)
	{
		case MMP_LDC_RES_MODE_FHD:
			m_ulDscLdcMaxSrcW = 1920;
			m_ulDscLdcMaxSrcH = 1080;
		break;	
		case MMP_LDC_RES_MODE_HD:
			m_ulDscLdcMaxSrcW = 1280;
			m_ulDscLdcMaxSrcH = 720;
		break;
		case MMP_LDC_RES_MODE_WVGA:
			m_ulDscLdcMaxSrcW = 848;
			m_ulDscLdcMaxSrcH = 480;
		break;
		default:
			m_bDscLdcEnable = MMP_FALSE;
			return MMP_DSC_ERR_PARAMETER;
		break;
	}
	
	MMPF_LDC_SetResMode(ubResMode);
	MMPF_LDC_SetFpsMode(ubFpsMode);
	
	m_bDscLdcEnable = MMP_TRUE;

    return MMP_ERR_NONE;
}
#endif

#if 0
void _____Memory_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_ExitMode
//  Description : Release memory for DSC capture
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_ExitMode(void)
{
    MMPS_DSC_SetPreviewBuf(&m_DSCPreviewFctlAttr, 0, 0, MMP_FALSE);  // Release DSC preview buffer.
	MMPS_DSC_SetSystemBuf(NULL, MMP_TRUE, MMP_TRUE, MMP_FALSE);  // Release DSC system buffer.
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetPreviewBuf
//  Description : Allocate/Release memory for DSC preview buffer
//------------------------------------------------------------------------------
/** @brief Allocate/Release memory for DSC preview buffer

@param[in] pFctlPreviewAttr    Preview attribute
@param[in] bAllocate           Allocate or Free
@return MMP_ERR_NONE.
*/
MMP_ERR MMPS_DSC_SetPreviewBuf(	MMPD_FCTL_ATTR 	*pFctlPreviewAttr, 
								MMP_ULONG 		ulDispWidth, 
								MMP_ULONG 		ulDispHeight, 
								MMP_BOOL 		bAllocate)
{
	#if (SUPPORT_FDTC) || (SUPPORT_MDTC)
	MMP_ERR				retstatus;
	#endif
	MMP_RAW_STORE_BUF	rawPreviewBuf;
	MMP_ULONG			ulRawBufSize;
    MMP_USHORT          usSensorPreviewW;
    MMP_USHORT          usSensorPreviewH;
    #if (SUPPORT_LDC_CAPTURE)
    MMP_ULONG			ulTmpBufSize = 0;
    #endif
    MMP_USHORT			byte_per_twopix;
	MMP_UBYTE			i = 0;
    MMP_UBYTE		    ubVifId = MMPD_Sensor_GetVIFPad(m_ubDscModeSnrId);
    
	/* Free Memory */
	if (!bAllocate) {
		return MMP_ERR_NONE;
	}
    
	/* Pixel select */
    switch(m_dscConfig.dispParams.DispColorFmt[m_usDscPreviewMode]) 
    {
		case MMP_DISPLAY_COLOR_RGB565:
    	case MMP_DISPLAY_COLOR_YUV422:
			byte_per_twopix = 4;
			break;
   	 	case MMP_DISPLAY_COLOR_YUV420:
   	 	case MMP_DISPLAY_COLOR_YUV420_INTERLEAVE:
			byte_per_twopix = 3;
            break;
		case MMP_DISPLAY_COLOR_RGB888:
			byte_per_twopix = 6;
            break;
		default:
			return MMP_DSC_ERR_PARAMETER;
			break;
    }
	
	/* Specify Y buffer of preview */
	pFctlPreviewAttr->ulBaseAddr[0] = m_ulDSCCurMemAddr;
	m_ulDSCPreviewEndAddr       	= pFctlPreviewAttr->ulBaseAddr[0] + 
                                     	 ALIGN32((ulDispWidth * byte_per_twopix) >> 1) * ulDispHeight;
	
	/* Specify U and V buffer for YUV420 */
	if (m_dscConfig.dispParams.DispColorFmt[m_usDscPreviewMode] == MMP_DISPLAY_COLOR_YUV420) 
	{
		pFctlPreviewAttr->ulBaseUAddr[0] = pFctlPreviewAttr->ulBaseAddr[0] + (ulDispWidth * ulDispHeight);
		pFctlPreviewAttr->ulBaseVAddr[0] = pFctlPreviewAttr->ulBaseUAddr[0] + (ulDispWidth * ulDispHeight >> 2);
    }
    else if (m_dscConfig.dispParams.DispColorFmt[m_usDscPreviewMode] == MMP_DISPLAY_COLOR_YUV420_INTERLEAVE)
	{
		pFctlPreviewAttr->ulBaseUAddr[0] = pFctlPreviewAttr->ulBaseAddr[0] + (ulDispWidth * ulDispHeight);
        pFctlPreviewAttr->ulBaseVAddr[0] = pFctlPreviewAttr->ulBaseUAddr[0];
	}

	/* Multi preview buffer */
   	if (m_dscConfig.dispParams.usDscDispBufCnt[m_usDscPreviewMode] > 1) 
   	{
		for (i = 1; i < m_dscConfig.dispParams.usDscDispBufCnt[m_usDscPreviewMode]; i++) 
		{
			// For Y buffer
    		pFctlPreviewAttr->ulBaseAddr[i] = pFctlPreviewAttr->ulBaseAddr[i - 1] + 
    		                                  ALIGN32((ulDispWidth * byte_per_twopix) >> 1) * ulDispHeight;

	 		m_ulDSCPreviewEndAddr       	= pFctlPreviewAttr->ulBaseAddr[i] + 
	 		                                  ALIGN32((ulDispWidth * byte_per_twopix) >> 1) * ulDispHeight;
			
			// For YUV420 U/V buffer
			if (m_dscConfig.dispParams.DispColorFmt[m_usDscPreviewMode] == MMP_DISPLAY_COLOR_YUV420) 
			{
				pFctlPreviewAttr->ulBaseUAddr[i] = pFctlPreviewAttr->ulBaseAddr[i] + (ulDispWidth * ulDispHeight);
				pFctlPreviewAttr->ulBaseVAddr[i] = pFctlPreviewAttr->ulBaseUAddr[i] + (ulDispWidth * ulDispHeight >> 2);
		    }
			else if (m_dscConfig.dispParams.DispColorFmt[m_usDscPreviewMode] == MMP_DISPLAY_COLOR_YUV420_INTERLEAVE) 
			{
				pFctlPreviewAttr->ulBaseUAddr[i] = pFctlPreviewAttr->ulBaseAddr[i] + (ulDispWidth * ulDispHeight);
				pFctlPreviewAttr->ulBaseVAddr[i] = pFctlPreviewAttr->ulBaseUAddr[i];
		    }
		}
 	}
	
	/* Fill the rest buffer address */
   	for (i = m_dscConfig.dispParams.usDscDispBufCnt[m_usDscPreviewMode]; i < MAX_FCTL_BUF_NUM; i++)
   	{
    	pFctlPreviewAttr->ulBaseAddr[i]  = pFctlPreviewAttr->ulBaseAddr[m_dscConfig.dispParams.usDscDispBufCnt[m_usDscPreviewMode]-1];    	
   		pFctlPreviewAttr->ulBaseUAddr[i] = pFctlPreviewAttr->ulBaseUAddr[m_dscConfig.dispParams.usDscDispBufCnt[m_usDscPreviewMode]-1];
   	    pFctlPreviewAttr->ulBaseVAddr[i] = pFctlPreviewAttr->ulBaseVAddr[m_dscConfig.dispParams.usDscDispBufCnt[m_usDscPreviewMode]-1];
   	}
	
	/* Rotate relative config */
	pFctlPreviewAttr->bUseRotateDMA  = m_dscConfig.dispParams.bUseRotateDMA[m_usDscPreviewMode];
    pFctlPreviewAttr->usRotateBufCnt = m_dscConfig.dispParams.usRotateBufCnt[m_usDscPreviewMode];
    
    if ((pFctlPreviewAttr->bUseRotateDMA) && 
        (pFctlPreviewAttr->usRotateBufCnt > 0))
    {        
        pFctlPreviewAttr->ulRotateAddr[0] = pFctlPreviewAttr->ulBaseAddr[MAX_FCTL_BUF_NUM-1] + 
                                            ALIGN32((ulDispWidth * byte_per_twopix) >> 1) * ALIGN32(ulDispHeight);

 		m_ulDSCPreviewEndAddr          	  = pFctlPreviewAttr->ulRotateAddr[0] + 
                                            ALIGN32((ulDispWidth * byte_per_twopix) >> 1) * ALIGN32(ulDispHeight);                                  
	
		if (m_dscConfig.dispParams.DispColorFmt[m_usDscPreviewMode] == MMP_DISPLAY_COLOR_YUV420) 
		{
		    pFctlPreviewAttr->ulRotateUAddr[0] = pFctlPreviewAttr->ulRotateAddr[0]  + (ulDispWidth * ulDispHeight);
            pFctlPreviewAttr->ulRotateVAddr[0] = pFctlPreviewAttr->ulRotateUAddr[0] + (ulDispWidth * ulDispHeight >> 2);
		}
		else if (m_dscConfig.dispParams.DispColorFmt[m_usDscPreviewMode] == MMP_DISPLAY_COLOR_YUV420_INTERLEAVE) 
		{
		    pFctlPreviewAttr->ulRotateUAddr[0] = pFctlPreviewAttr->ulRotateAddr[0]  + (ulDispWidth * ulDispHeight);
            pFctlPreviewAttr->ulRotateVAddr[0] = pFctlPreviewAttr->ulRotateUAddr[0];
		}
		
		if (pFctlPreviewAttr->usRotateBufCnt > 1)
		{            
            pFctlPreviewAttr->ulRotateAddr[1] = pFctlPreviewAttr->ulRotateAddr[0] + 
                                                ALIGN32((ulDispWidth * byte_per_twopix) >> 1) * ALIGN32(ulDispHeight);

     		m_ulDSCPreviewEndAddr          	  = pFctlPreviewAttr->ulRotateAddr[1] + 
                                                ALIGN32((ulDispWidth * byte_per_twopix) >> 1) * ALIGN32(ulDispHeight);
    	
    		if (m_dscConfig.dispParams.DispColorFmt[m_usDscPreviewMode] == MMP_DISPLAY_COLOR_YUV420) 
    		{
    		    pFctlPreviewAttr->ulRotateUAddr[1] = pFctlPreviewAttr->ulRotateAddr[1] + (ulDispWidth * ulDispHeight);											
    		    pFctlPreviewAttr->ulRotateVAddr[1] = pFctlPreviewAttr->ulRotateUAddr[1] + (ulDispWidth * ulDispHeight >> 2);
    		}	
    		else if (m_dscConfig.dispParams.DispColorFmt[m_usDscPreviewMode] == MMP_DISPLAY_COLOR_YUV420_INTERLEAVE) 
    		{
    		    pFctlPreviewAttr->ulRotateUAddr[1] = pFctlPreviewAttr->ulRotateAddr[1] + (ulDispWidth * ulDispHeight);											
    		    pFctlPreviewAttr->ulRotateVAddr[1] = pFctlPreviewAttr->ulRotateUAddr[1];
    		}
		}
    }

    /* For preview via Raw path (Raw store buffer) */
    if (m_DscPreviewPath == MMP_DSC_RAW_PREVIEW)
    {
        // Raw mode need to get real raw store width and height.
        MMPD_RAWPROC_GetStoreRange(ubVifId, &usSensorPreviewW, &usSensorPreviewH);
        MMPD_RAWPROC_CalcBufSize(m_DscRawColorDepth, usSensorPreviewW, usSensorPreviewH, &ulRawBufSize);

		//ulRawBufSize = 4096 * 2048; //For Max 4k2k sensor mode (TBD)

        rawPreviewBuf.ulRawBufCnt = m_dscConfig.dispParams.ulRawPreviewBufCnt;
        
        for (i = 0; i < rawPreviewBuf.ulRawBufCnt; i++) {
            
    		rawPreviewBuf.ulRawBufAddr[i]	= m_ulDSCPreviewEndAddr;
   	 		m_ulDSCPreviewEndAddr		   += ulRawBufSize;
    	 	m_ulDSCPreviewEndAddr		    = ALIGN32(m_ulDSCPreviewEndAddr);
	    }

		MMPD_RAWPROC_SetStoreBuf(ubVifId, &rawPreviewBuf);
    }
	
	/* Get Graphics loopback source buffer config and size */
	#if (SUPPORT_LDC_CAPTURE)
	if (m_bDscLdcEnable) {
		m_sDscLdcSrcBuf.usInBufCnt 	= 2;
	}
	else {
		m_sDscLdcSrcBuf.usInBufCnt 	= 0;
	}

	/* Allocate memory for LDC source buffer (NV12) */
    for (i = 0; i < m_sDscLdcSrcBuf.usInBufCnt; i++) {

		ulTmpBufSize = m_ulDscLdcMaxSrcW * m_ulDscLdcMaxSrcH;

        m_sDscLdcSrcBuf.ulInYBuf[i] = m_ulDSCPreviewEndAddr;
        m_sDscLdcSrcBuf.ulInUBuf[i] = m_sDscLdcSrcBuf.ulInYBuf[i] + ALIGN32(ulTmpBufSize);
        m_sDscLdcSrcBuf.ulInVBuf[i] = m_sDscLdcSrcBuf.ulInUBuf[i];

        m_ulDSCPreviewEndAddr += (ALIGN32(ulTmpBufSize) + ALIGN32(ulTmpBufSize>>1));
	}
	#endif
	
	/* Face detection buffer */
	#if (SUPPORT_FDTC)
    if (MMPS_Fdtc_GetConfig()->bFaceDetectEnable) 
    {
        retstatus = MMPS_Sensor_AllocateFDBuffer(&m_ulDSCPreviewEndAddr, 
                                                 ulDispWidth,
 												 ulDispHeight,
                                                 bAllocate);

        if (retstatus != MMP_ERR_NONE) {
            PRINTF("FDTC: allocate buffers failed.\r\n");
            return retstatus;
        }
    }
    #endif

    /* Motion detection buffer */
    #if (SUPPORT_MDTC)
    retstatus = MMPS_Sensor_AllocateVMDBuffer(&m_ulDSCPreviewEndAddr, MMP_TRUE);

    if (retstatus) {
        PRINTF("VMD: allocate buffers failed.\r\n");
        return retstatus;
    }
    #endif

    #if defined(ALL_FW)
    if (m_ulDSCPreviewEndAddr > MMPS_System_GetMemHeapEnd()) {
        printc("\t= [HeapMemErr] PreviewBuf =\r\n");
        return MMP_DSC_ERR_MEM_EXHAUSTED;
    }
    printc("End of DSC preview buffers = 0x%X\r\n", m_ulDSCPreviewEndAddr);
    #endif

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetCaptureBuf
//  Description :
//------------------------------------------------------------------------------
/** @brief Configure the buffer for capture operation

The function sets the buffer address for compress buffer start, compress buffer end and line buffer by
sending host command request to the firmware and waiting for the DSC command clear. 

@param[in] CaptureRotateType rotate capture type
@retval MMP_ERR_NONE success
@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_SetCaptureBuf(	MMP_ULONG	*ulSramPos,
								MMP_ULONG	*ulDramPos,
								MMP_BOOL	bAllocateForVideoRec,
								MMP_BOOL 	bAllocateForRawproc,
								MMP_DSC_CAPTURE_ROTATE_TYPE CaptureRotateType, 
								MMP_BOOL 	bAllocate)
{
	#if (HANDLE_JPEG_EVENT_BY_QUEUE)
	extern MMP_ULONG    gulFrontCamEncLineBuf;
	#endif
	MMP_DSC_CAPTURE_BUF  	JpgCaptureBuf;
    MMP_GRAPHICS_DRAWRECT_ATTR drawAttr;
	
	MMP_ULONG	            ulBufSize;
	MMP_ULONG               ThumbWidth;
	MMP_ULONG	 			ThumbHeight;
	MMP_ULONG               ThumbDecodeW, ThumbDecodeH;
	MMP_ULONG               ThumbBufW, ThumbBufH;
	
	MMP_ULONG               ulDramCurPos = m_ulDSCPreviewEndAddr;
	MMP_ULONG				ulSramCurPos = 0x104000;
	
	MMPD_System_GetSramEndAddr(&ulSramCurPos);

	if (ulSramPos != NULL) {
		ulSramCurPos = *ulSramPos;
	}	
	if (ulDramPos != NULL) {
		ulDramCurPos = *ulDramPos;
	}

    ThumbWidth	= m_ulDSCThumbWidth;
    ThumbHeight	= m_ulDSCThumbHeight;
    
    ThumbDecodeW = ThumbBufW = ThumbWidth;
    ThumbDecodeH = ThumbBufH = ThumbHeight;

	/* Exif working buffer */
    MMPD_DSC_GetBufferSize(MMPD_DSC_EXIF_WORK_BUFFER, 0, 0 ,0, &ulBufSize);
    
    ulDramCurPos = ALIGN4(ulDramCurPos);
	MMPD_DSC_SetExifWorkingBuffer(EXIF_NODE_ID_PRIMARY, (MMP_UBYTE *)ulDramCurPos, ulBufSize, MMP_FALSE); 
	ulDramCurPos +=	ulBufSize;
	
	#if (DSC_SUPPORT_BASELINE_MP_FILE)
	if (m_bMultiPicFormatEnable && m_bEncLargeThumbEnable) 
	{
	    ulDramCurPos = ALIGN4(ulDramCurPos);
		MMPD_DSC_SetExifWorkingBuffer(EXIF_NODE_ID_LARGE_THUMB1, (MMP_UBYTE *)ulDramCurPos, ulBufSize, MMP_FALSE); 
		ulDramCurPos +=	ulBufSize;	
	}
	#endif
	
	/* Compress buffer and line buffer */
	m_ulPrimaryJpegLineStart = ulSramCurPos;

	MMPD_DSC_GetBufferSize(MMPD_DSC_ENC_DOUBLE_LINE_BUFFER, 0, m_usJpegWidth, m_usJpegHeight, &ulBufSize);

	ulSramCurPos += ulBufSize;

	if (bAllocateForVideoRec) {
		MMPD_DSC_GetBufferSize(MMPD_DSC_ENC_VR_CAPTURE_COMP_BUFFER, 0, m_usJpegWidth, m_usJpegHeight, &ulBufSize);
	}
	else {
		MMPD_DSC_GetBufferSize(MMPD_DSC_ENC_EXTERNAL_COMPRESS_BUFFER, 0, m_usJpegWidth, m_usJpegHeight, &ulBufSize);
	}
	
    m_ulPrimaryJpegCompStart = ulDramCurPos = ALIGN_X(ulDramCurPos, DSC_BUF_ALIGN_BASE);

    if (m_usCaptureMode == MMPS_DSC_MULTI_SHOTMODE) {

        // Multi-shot reserved 4 buffer
        ulDramCurPos += (ulBufSize << 2);
        ulDramCurPos = ALIGN_X(ulDramCurPos, DSC_BUF_ALIGN_BASE);
        m_ulPrimaryJpegCompEnd = ulDramCurPos;
    }
    else if (m_usCaptureMode == MMPS_DSC_SINGLE_SHOTMODE) {

        if (m_dscConfig.encParams.bStableShotEn)
        {
            // Stable-shot reserved 2 buffer
    	 	ulDramCurPos	+= ulBufSize;  
    	 	ulDramCurPos	= ALIGN_X(ulDramCurPos, DSC_BUF_ALIGN_BASE); 
    	 	m_ulSubJpegExtCompStart = ulDramCurPos;
    	 	
    	 	ulDramCurPos	+= ulBufSize; 
    	 	ulDramCurPos	= ALIGN_X(ulDramCurPos, DSC_BUF_ALIGN_BASE); 
            m_ulPrimaryJpegCompEnd = ulDramCurPos;
        }
        else {

            ulDramCurPos  	+= ulBufSize;
            ulDramCurPos  	= ALIGN_X(ulDramCurPos, DSC_BUF_ALIGN_BASE);
            m_ulPrimaryJpegCompEnd = ulDramCurPos;
        }  
    }
    else if (m_usCaptureMode == MMPS_DSC_CONTINUOUS_SHOTMODE) { 
	
        if (m_ulDSCMaxDramAddr != 0)
        {
            m_ulPrimaryJpegCompEnd = m_ulPrimaryJpegCompStart + ulBufSize;

    		/* Allocate ring buffer memory */
            ulBufSize = m_ulDSCMaxDramAddr - ulDramCurPos - (1*1024*1024) /* Reserved for thumbnail decode */ - (256 * 1024)/*EXIF*/;

            MMPD_DSC_SetRingBuffer(m_ulPrimaryJpegCompStart, ulBufSize, 
                				   (m_ulPrimaryJpegCompEnd - m_ulPrimaryJpegCompStart), 
                					m_ulContiShotInterval);
    	
            ulDramCurPos += ALIGN_X(ulBufSize, DSC_BUF_ALIGN_BASE);
        }
	    else
	    {
            ulDramCurPos += ulBufSize;
            ulDramCurPos  = ALIGN_X(ulDramCurPos, DSC_BUF_ALIGN_BASE);
            m_ulPrimaryJpegCompEnd = ulDramCurPos;

    		/* Allocate ring buffer memory */ 
    		ulDramCurPos += ALIGN_X(m_ulMaxRingBufferSize - ulBufSize, DSC_BUF_ALIGN_BASE);

            MMPD_DSC_SetRingBuffer(m_ulPrimaryJpegCompStart, m_ulMaxRingBufferSize, 
            					   (m_ulPrimaryJpegCompEnd - m_ulPrimaryJpegCompStart),
            					   m_ulContiShotInterval);
        }
        
        MMPD_DSC_SetContiShotParam(ulDramCurPos, m_ulContiShotMaxNum);

        ulDramCurPos += (256 * 1024); //TBD For EXIF
    }
    else {
        PRINTF("MMPS_DSC_SetCaptureBuf, m_usCaptureMode error!\r\n");
        return MMP_ERR_NONE;
    }

	#if (DSC_SUPPORT_BASELINE_MP_FILE)
	if (m_bMultiPicFormatEnable && m_bEncLargeThumbEnable) 
	{
		m_ulLargeThumbJpegCompStart = ulDramCurPos = ALIGN_X(ulDramCurPos, DSC_BUF_ALIGN_BASE);

		MMPD_DSC_GetBufferSize(MMPD_DSC_ENC_LARGE_THUMB_COMP_BUFFER, 0, m_ulLargeThumbWidth, m_ulLargeThumbHeight, &ulBufSize);

	    ulDramCurPos  	+= ulBufSize;
	    ulDramCurPos  	= ALIGN_X(ulDramCurPos, DSC_BUF_ALIGN_BASE);
	    m_ulLargeThumbJpegCompEnd = ulDramCurPos;
    }
	#endif

	JpgCaptureBuf.ulCompressStart	= m_ulPrimaryJpegCompStart;
	JpgCaptureBuf.ulCompressEnd		= m_ulPrimaryJpegCompEnd;
	JpgCaptureBuf.ulLineStart		= m_ulPrimaryJpegLineStart;

	#if (HANDLE_JPEG_EVENT_BY_QUEUE)
	/*
	In HANDLE_JPEG_EVENT_BY_QUEUE case ,"MMPD_DSC_SetCaptureBuffers" has no effect on JPEG encoding
	It depends on two parameters for front cam case: 
	1.gulFrontCamTmpCompBuf - Will be assigned in function MMPF_DSC_StreamJPEGInfogul
	2.gulFrontCamEncLineBuf - Assigned here 
	*/
	gulFrontCamEncLineBuf = m_ulPrimaryJpegLineStart;
	#endif
	
    MMPD_DSC_SetCaptureBuffers(&JpgCaptureBuf);

    /* Calculate ThumbDecodeW/H and ThumbBufW/H for thumbnail */
    if (m_thumbnailInputMode == MMP_DSC_THUMB_SRC_DECODED_JPEG || 
        m_thumbnailInputMode == MMP_DSC_THUMB_SRC_THUMB_PIPE) {
    
        if ( (ThumbWidth * m_usJpegHeight) > (m_usJpegWidth * ThumbHeight) ) {
    		ThumbDecodeW = m_usJpegWidth * ThumbHeight / m_usJpegHeight;
            ThumbDecodeH = ThumbHeight;
        }
        else if ( (ThumbWidth * m_usJpegHeight) <= (m_usJpegWidth * ThumbHeight) ) {
            ThumbDecodeW = ThumbWidth;
    		ThumbDecodeH = m_usJpegHeight * ThumbWidth / m_usJpegWidth;
        }
 
        ThumbDecodeW = ALIGN16(ThumbDecodeW);
        ThumbDecodeH = ALIGN8(ThumbDecodeH);
    
	    /* Allocate thumbnail buffer : Suppose decode output store format is YUV422. */
        // m_ulPrimaryJpeg4ExifThumbDecodeOutSize = Thumbnail buffer size + reserved decode line buffer size 
        if (ThumbDecodeW < ThumbBufW) {
            m_ulPrimaryJpeg4ExifThumbDecodeOutSize	= ALIGN_X(ThumbDecodeW * 2, DSC_BUF_ALIGN_BASE) * ThumbDecodeH +
                                                      MMPS_DSC_GetConfig()->decParams.ulDscDecMaxLineBufSize;
            
            m_ulBlackThumbnailDecodeOutSize         = ALIGN_X(ThumbBufW * 2, DSC_BUF_ALIGN_BASE) * ThumbBufH;
        }
        else {
            m_ulPrimaryJpeg4ExifThumbDecodeOutSize	= ALIGN_X(ThumbBufW * 2, DSC_BUF_ALIGN_BASE) * ThumbBufH +
                                                      MMPS_DSC_GetConfig()->decParams.ulDscDecMaxLineBufSize;
                                                      
        	m_ulBlackThumbnailDecodeOutSize			= 0;
        }
      
        m_ulPrimaryJpeg4ExifThumbDecodeOutStart = ulDramCurPos;
        ulDramCurPos 							+= m_ulPrimaryJpeg4ExifThumbDecodeOutSize;
        
        m_ulBlackThumbnailDecodeOutStart 		= 0;

        if (ThumbDecodeW < ThumbBufW) {
            m_ulBlackThumbnailDecodeOutStart	= ulDramCurPos;
            ulDramCurPos 						+= m_ulBlackThumbnailDecodeOutSize;
        }
        else {
            m_ulBlackThumbnailDecodeOutStart	= 0;
        }
        
        /* Clear the black / thumbnail buffer */
        if (ThumbDecodeW < ThumbBufW) {
        
            drawAttr.type       = MMP_GRAPHICS_SOLID_FILL;
            drawAttr.bUseRect   = MMP_FALSE;
            drawAttr.usWidth    = ThumbDecodeW;    
            drawAttr.usHeight   = ThumbDecodeH;
            drawAttr.usLineOfst = ThumbDecodeW * 2;
            drawAttr.ulBaseAddr = m_ulPrimaryJpeg4ExifThumbDecodeOutStart;
            drawAttr.colordepth = MMP_GRAPHICS_COLORDEPTH_16;
            drawAttr.ulColor    = 0x0080; //For YUV422;
            drawAttr.ulPenSize  = 0;
            drawAttr.ropcode    = MMP_GRAPHICS_ROP_SRCCOPY;
            
            MMPD_Graphics_DrawRectToBuf(&drawAttr, NULL, NULL);
 
            drawAttr.type       = MMP_GRAPHICS_SOLID_FILL;
            drawAttr.bUseRect   = MMP_FALSE;
            drawAttr.usWidth    = ThumbBufW;    
            drawAttr.usHeight   = ThumbBufH;
            drawAttr.usLineOfst = ThumbBufW * 2;
            drawAttr.ulBaseAddr = m_ulBlackThumbnailDecodeOutStart;
            drawAttr.colordepth = MMP_GRAPHICS_COLORDEPTH_16;
            drawAttr.ulColor    = 0x0080; //For YUV422;
            drawAttr.ulPenSize  = 0;
            drawAttr.ropcode    = MMP_GRAPHICS_ROP_SRCCOPY;
            
            MMPD_Graphics_DrawRectToBuf(&drawAttr, NULL, NULL);
        }
        else {

            drawAttr.type       = MMP_GRAPHICS_SOLID_FILL;
            drawAttr.bUseRect   = MMP_FALSE;
            drawAttr.usWidth    = ThumbBufW;    
            drawAttr.usHeight   = ThumbBufH;
            drawAttr.usLineOfst = ThumbBufW * 2;
            drawAttr.ulBaseAddr = m_ulPrimaryJpeg4ExifThumbDecodeOutStart;
            drawAttr.colordepth = MMP_GRAPHICS_COLORDEPTH_16;
            drawAttr.ulColor    = 0x0080; //For YUV422
            drawAttr.ulPenSize  = 0;
            drawAttr.ropcode    = MMP_GRAPHICS_ROP_SRCCOPY;

            MMPD_Graphics_DrawRectToBuf(&drawAttr, NULL, NULL);
        }
        
        #if (DSC_SUPPORT_BASELINE_MP_FILE)
        if (m_bMultiPicFormatEnable && m_bEncLargeThumbEnable)
        {
	        m_ulPrimaryJpeg4LargeThumbDecodeOutSize = ALIGN_X(m_ulLargeThumbWidth * 2, DSC_BUF_ALIGN_BASE) * m_ulLargeThumbHeight + 
	        								  		  MMPS_DSC_GetConfig()->decParams.ulDscDecMaxLineBufSize;

	        m_ulPrimaryJpeg4LargeThumbDecodeOutStart = ALIGN_X(ulDramCurPos, DSC_BUF_ALIGN_BASE);

	        ulDramCurPos += m_ulPrimaryJpeg4LargeThumbDecodeOutSize;      
        }
        #endif
    }
    
    /* Raw capture rotate buffer : Total 1 (if rotated) buffer */
	if (bAllocateForRawproc == MMP_TRUE)
	{
		if (m_DscPreviewPath == MMP_DSC_RAW_PREVIEW) 
		{
			MMP_USHORT	usSensorPreviewW;
    		MMP_USHORT	usSensorPreviewH;

			m_CaptureRotateType = CaptureRotateType;
		
		    /* Rotate buffer for raw capture */
	    	if (CaptureRotateType != MMP_DSC_CAPTURE_NO_ROTATE) 
	    	{
	            MMPD_RAWPROC_CalcBufSize(m_DscRawColorDepth, usSensorPreviewW, usSensorPreviewH, &ulBufSize);
			 	ulBufSize = ALIGN32(ulBufSize);
            	            
	            ulDramCurPos = ALIGN32(ulDramCurPos);
	            MMPD_DSC_SetRotateCapture(CaptureRotateType, ulDramCurPos);
	    	 	ulDramCurPos += ulBufSize;
	    	}
	    	else {
	    	    MMPD_DSC_SetRotateCapture(MMP_DSC_CAPTURE_NO_ROTATE, 0);
	    	} 
		}
		else {
			/* Capture from ISP path can't support roatate capture */
		    m_CaptureRotateType = MMP_DSC_CAPTURE_NO_ROTATE;
		    
		    MMPD_DSC_SetRotateCapture(MMP_DSC_CAPTURE_NO_ROTATE, 0);
		}
	}
	else {

		/* Capture from ISP path can't support roatate capture */
	    m_CaptureRotateType = MMP_DSC_CAPTURE_NO_ROTATE;
	    
	    MMPD_DSC_SetRotateCapture(MMP_DSC_CAPTURE_NO_ROTATE, 0);	
	}

	if (ulSramPos != NULL) {
		*ulSramPos = ulSramCurPos;
	}	
	if (ulDramPos != NULL) {
		*ulDramPos = ulDramCurPos;
	}

    #if defined(ALL_FW)
    if (ulDramCurPos > MMPS_System_GetMemHeapEnd()) {
        printc("\t= [HeapMemErr] CaptureBuf =\r\n");
        return MMP_DSC_ERR_MEM_EXHAUSTED;
    }
    printc("End of DSC capture buffers = 0x%X\r\n", ulDramCurPos);
    #endif

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetSystemBuf
//  Description : Allocate/Release memory for ISP/DSC Filename
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_SetSystemBuf(MMP_ULONG *pulCurBufPos, MMP_BOOL bISP, MMP_BOOL bFilename, MMP_BOOL bAllocate)
{
    MMP_ULONG	ulSize;
	MMP_ULONG	ulCuBufAddr;
	
	if (!bAllocate) {
	    //Free memory
		return MMP_ERR_NONE;
	}

	if (pulCurBufPos == NULL) {
		MMPD_System_GetFWEndAddr(&ulCuBufAddr);
	}
	else {
		ulCuBufAddr = *pulCurBufPos;
	}
	
	if (bISP == MMP_TRUE) {
	    MMPD_Sensor_AllocateBuffer(ulCuBufAddr, &ulSize);
	    ulCuBufAddr += ulSize;
    }
    
    if (bFilename == MMP_TRUE) {
		MMPD_DSC_SetFileNameAddr(ulCuBufAddr);
	    ulCuBufAddr += (DSC_MAX_FILE_NAME_SIZE * 3);
    }
    
    if (pulCurBufPos != NULL) {
    	*pulCurBufPos = ulCuBufAddr;
	}
	
    m_ulDSCCurMemAddr = ulCuBufAddr;
    
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_GetCaptureBuf
//  Description : Get the memory address of DSC capture.
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_GetCaptureBuf(MMP_ULONG *pulLineBuf, MMP_ULONG *pulCompBuf)
{
    *pulLineBuf = m_ulPrimaryJpegLineStart;
    *pulCompBuf = m_ulPrimaryJpegCompStart;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetMJpegEncBuf
//  Description : Allocate/Release memory for MJPEG line/compress buffer.
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_SetMJpegEncBuf(MMP_ULONG *pulCurBufPos, MMP_USHORT usEncW, MMP_USHORT usEncH, MMP_BOOL bAllocate)
{
    MMP_DSC_CAPTURE_BUF  	JpgCaptureBuf;
    MMP_ULONG				ulBufSize;
	MMP_ULONG				ulDramCurPos;
	MMP_ULONG				ulSramCurPos = 0x104000;
	
	MMPD_System_GetSramEndAddr(&ulSramCurPos);

	if (!bAllocate) {
	    //Free memory
		return MMP_ERR_NONE;
	}

	if (pulCurBufPos == NULL) {
		return MMP_DSC_ERR_PARAMETER;
	}	
	else {
		ulDramCurPos = *pulCurBufPos;
	}
	
	// Initial line buffer
	MMPD_DSC_GetBufferSize(MMPD_DSC_ENC_DOUBLE_LINE_BUFFER, 0, usEncW, usEncH, &ulBufSize);
	JpgCaptureBuf.ulLineStart = ulSramCurPos;
	ulSramCurPos += ulBufSize;

	// Initial compress buffer (ring buffer)
    MMPD_DSC_GetBufferSize(MMPD_DSC_ENC_EXTERNAL_COMPRESS_BUFFER, 0, usEncW, usEncH, &ulBufSize);
	JpgCaptureBuf.ulCompressStart	= ALIGN_X(ulDramCurPos, DSC_BUF_ALIGN_BASE);
	JpgCaptureBuf.ulCompressEnd		= JpgCaptureBuf.ulCompressStart + ulBufSize;

   	MMPD_DSC_SetCaptureBuffers(&JpgCaptureBuf);
   	
   	MMPD_DSC_SetRingBuffer(JpgCaptureBuf.ulCompressStart, m_ulMaxRingBufferSize, 
            			   (JpgCaptureBuf.ulCompressEnd - JpgCaptureBuf.ulCompressStart),
            			   0);

	ulDramCurPos += m_ulMaxRingBufferSize;
	
	m_ulCompRingBufferAddr = JpgCaptureBuf.ulCompressStart;
	
	*pulCurBufPos = ulDramCurPos;

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_GetMJpegCompBuf
//  Description : Get MJPEG compress buffer info.
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_GetMJpegCompBuf(MMP_ULONG *pulBufAddr, MMP_ULONG *pulBufSize)
{
    *pulBufAddr = m_ulCompRingBufferAddr;
    *pulBufSize = m_ulMaxRingBufferSize;
    
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetMaxRingBufferSize
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_SetMaxRingBufferSize(MMP_ULONG ulMaxSize)
{
	m_ulMaxRingBufferSize = ulMaxSize;
	return MMP_ERR_NONE;
}

#if 0
void _____Preview_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetPreviewPipe
//  Description :
//------------------------------------------------------------------------------
/** @brief
@param[in] ubPipe	which pipe for preview want to use.
@return It reports the status of the configuration.
*/
MMP_ERR MMPS_DSC_SetPreviewPipe(MMP_UBYTE ubPipe)
{	
	m_fctlLinkPreview.scalerpath 	= (MMP_SCAL_PIPEID) ubPipe;
	m_fctlLinkPreview.icopipeID		= (MMP_ICO_PIPEID) ubPipe;
	m_fctlLinkPreview.ibcpipeID		= (MMP_IBC_PIPEID) ubPipe;
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetPreviewPath
//  Description :
//------------------------------------------------------------------------------
/** @brief 
@param[in] ubPath       which kind of preview mode you use
@param[in] colordepth   which kind of raw bit mode, the preview mode use.
@return It reports the status of the configuration.
*/
MMP_ERR MMPS_DSC_SetPreviewPath(MMP_DSC_PREVIEW_PATH ubPath, MMP_RAW_COLORFMT colorFormat)
{	
	m_DscPreviewPath 	= ubPath;
	m_DscRawColorDepth 	= colorFormat;
	
	if (MMP_IsDscPtzEnable()) {
		m_DscPreviewPath 	= MMP_DSC_RAW_PREVIEW;
		m_DscRawColorDepth 	= MMP_RAW_COLORFMT_BAYER8;
	}
	
	if (m_bScaleUpCapture) {
	    m_DscPreviewPath 	= MMP_DSC_RAW_PREVIEW;
	    m_DscRawColorDepth 	= MMP_RAW_COLORFMT_BAYER8;
	}
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_InitPreview
//  Description :
//------------------------------------------------------------------------------
/** @brief Config the preview mode parameters needed
@return It reports the status of the configuration.
*/
MMP_ERR MMPS_DSC_InitPreview(MMP_UBYTE ubSnrSel)
{ 
	MMP_ERR					retstatus;
    MMP_DISPLAY_DISP_ATTR  	dispAttr;
    MMP_SCAL_FIT_RANGE     	fitrange;
    MMP_SCAL_GRAB_CTRL   	grabctl;
    MMP_SCAL_GRAB_CTRL   	DispGrabctl;
	MMP_SCAL_FIT_MODE		sFitMode;
	MMP_DISPLAY_ROTATE_TYPE	ubDmaRotateDir;

	MMP_ULONG           	ulPreviewBufW;
	MMP_ULONG				ulPreviewBufH;
	MMP_ULONG          		ulRotateW, ulRotateH;
	MMP_ULONG               ulDispWidth;
	MMP_ULONG 				ulDispHeight;
	MMP_ULONG				ulDispStartX;
	MMP_ULONG				ulDispStartY;
    MMP_ULONG             	ulScalInW;
    MMP_ULONG				ulScalInH;
    MMP_SCAL_FIT_RANGE		sFitRangeBayer;
    MMP_SCAL_GRAB_CTRL 		sGrabctlBayer;
    MMP_USHORT 				usCurZoomStep = 0;
    MMP_UBYTE				ubVifId = MMPD_Sensor_GetVIFPad(ubSnrSel);

    m_ubDscModeSnrId = ubSnrSel;

	MMPS_Sensor_GetCurPrevScalInputRes(ubSnrSel, &ulScalInW, &ulScalInH);

    MMPD_BayerScaler_GetZoomInfo(MMP_BAYER_SCAL_DOWN, &sFitRangeBayer, &sGrabctlBayer);

	/* Set DSC preview resolution and allocate buffer */
    if ((m_sAhcDscPrevInfo.bUserDefine == MMP_TRUE) && 
        (m_usDscPreviewMode != DSC_CCIR_PREVIEW_MODE)) 
    {
        ulPreviewBufW 	= m_sAhcDscPrevInfo.ulPreviewBufW;
        ulPreviewBufH	= m_sAhcDscPrevInfo.ulPreviewBufH;
        sFitMode		= m_sAhcDscPrevInfo.sFitMode;
        ubDmaRotateDir	= m_sAhcDscPrevInfo.sPreviewDmaDir;

        ulDispWidth     = m_sAhcDscPrevInfo.ulDispWidth;
        ulDispHeight	= m_sAhcDscPrevInfo.ulDispHeight;
        ulDispStartX    = m_sAhcDscPrevInfo.ulDispStartX;
        ulDispStartY  	= m_sAhcDscPrevInfo.ulDispStartY;

        MMPS_Display_AdjustScaleInSize( MMP_DISPLAY_PRM_CTL,
                                        ulScalInW, 
                                        ulScalInH, 
                                        ulPreviewBufW, 
                                        ulPreviewBufH,
                                        &ulPreviewBufW,
                                        &ulPreviewBufH);
    }
    else {
        ulPreviewBufW	= m_dscConfig.dispParams.usDscPreviewBufW[m_usDscPreviewMode];
        ulPreviewBufH	= m_dscConfig.dispParams.usDscPreviewBufH[m_usDscPreviewMode];
        sFitMode		= m_dscConfig.dispParams.sFitMode[m_usDscPreviewMode];
		ubDmaRotateDir	= m_dscConfig.dispParams.ubDMARotateDir[m_usDscPreviewMode];

        ulDispWidth   	= m_dscConfig.dispParams.usDscDisplayW[m_usDscPreviewMode];
        ulDispHeight 	= m_dscConfig.dispParams.usDscDisplayH[m_usDscPreviewMode];
        ulDispStartX 	= m_dscConfig.dispParams.usDscDispStartX[m_usDscPreviewMode];
        ulDispStartY 	= m_dscConfig.dispParams.usDscDispStartY[m_usDscPreviewMode];
    }

	if (MMPS_DSC_SetPreviewBuf(&m_DSCPreviewFctlAttr, ulPreviewBufW, ulPreviewBufH, MMP_TRUE))
        return MMP_DSC_ERR_MEM_EXHAUSTED;

	/* Set zoom parameters */
	MMPS_DSC_InitDigitalZoomParam();
	
	#if (SUPPORT_LDC_CAPTURE)
	if (m_bDscLdcEnable)
	{
    	MMP_SCAL_GRAB_CTRL  ldcGrabCtl;
		MMPD_FCTL_ATTR  	ldcFctlAttr;
		MMP_ULONG			i = 0;
		
		MMPF_LDC_SetLinkPipe(m_fctlLinkLdcSrc.ibcpipeID,
							 m_fctlLinkPreview.ibcpipeID,
							 m_fctlLinkCapture.ibcpipeID,
							 m_fctlLinkPreview.ibcpipeID);
		
        // Config pipe for graphic loopback
        fitrange.fitmode        	= MMP_SCAL_FITMODE_OUT;
        fitrange.scalerType			= MMP_SCAL_TYPE_SCALER;
		fitrange.ulInWidth  		= ulScalInW;
        fitrange.ulInHeight 		= ulScalInH; 
        fitrange.ulOutWidth     	= m_ulDscLdcMaxSrcW;
        fitrange.ulOutHeight    	= m_ulDscLdcMaxSrcH;

        fitrange.ulInGrabX 			= 1;
        fitrange.ulInGrabY 			= 1;
        fitrange.ulInGrabW 			= fitrange.ulInWidth;
        fitrange.ulInGrabH 			= fitrange.ulInHeight;

        MMPD_Scaler_GetGCDBestFitScale(&fitrange, &ldcGrabCtl);

        ldcFctlAttr.colormode      	= MMP_DISPLAY_COLOR_YUV420_INTERLEAVE;
        ldcFctlAttr.fctllink       	= m_fctlLinkLdcSrc;
        ldcFctlAttr.fitrange		= fitrange;
        ldcFctlAttr.grabctl        	= ldcGrabCtl;
        ldcFctlAttr.scalsrc			= MMP_SCAL_SOURCE_ISP;
        ldcFctlAttr.bSetScalerSrc	= MMP_TRUE;
        ldcFctlAttr.usBufCnt  		= m_sDscLdcSrcBuf.usInBufCnt;

        for (i = 0; i < m_sDscLdcSrcBuf.usInBufCnt; i++) {
            ldcFctlAttr.ulBaseAddr[i]  = m_sDscLdcSrcBuf.ulInYBuf[i];
            ldcFctlAttr.ulBaseUAddr[i] = m_sDscLdcSrcBuf.ulInUBuf[i];
            ldcFctlAttr.ulBaseVAddr[i] = m_sDscLdcSrcBuf.ulInVBuf[i];
        }
        
        ldcFctlAttr.bUseRotateDMA  = MMP_FALSE;
        ldcFctlAttr.usRotateBufCnt = 0;

        MMPD_Fctl_SetPipeAttrForIbcFB(&ldcFctlAttr);
        MMPD_Fctl_ClearPreviewBuf(m_fctlLinkLdcSrc.ibcpipeID, 0xFFFFFF);
        MMPD_Fctl_LinkPipeToLdc(m_fctlLinkLdcSrc.ibcpipeID);
	}
	#endif

	/* Config preview pipe */
    fitrange.fitmode		= sFitMode;
    fitrange.scalerType 	= MMP_SCAL_TYPE_SCALER;

    if (m_DscPreviewPath == MMP_DSC_RAW_PREVIEW) {

        #if (SUPPORT_LDC_CAPTURE)
        fitrange.ulInWidth	= (m_bDscLdcEnable) ? (m_ulDscLdcMaxSrcW) : (sFitRangeBayer.ulOutWidth);
        fitrange.ulInHeight	= (m_bDscLdcEnable) ? (m_ulDscLdcMaxSrcH) : (sFitRangeBayer.ulOutHeight);
        #else
        fitrange.ulInWidth	= sFitRangeBayer.ulOutWidth;
        fitrange.ulInHeight	= sFitRangeBayer.ulOutHeight;
        #endif
    }
    else {

		#if (SUPPORT_LDC_CAPTURE)
        fitrange.ulInWidth	= (m_bDscLdcEnable) ? (m_ulDscLdcMaxSrcW) : (ulScalInW);
        fitrange.ulInHeight	= (m_bDscLdcEnable) ? (m_ulDscLdcMaxSrcH) : (ulScalInH);
        #else
        fitrange.ulInWidth	= ulScalInW;
        fitrange.ulInHeight	= ulScalInH;
        #endif
    }

    fitrange.ulOutWidth		= ulPreviewBufW;
    fitrange.ulOutHeight	= ulPreviewBufH;

	fitrange.ulInGrabX		= 1; 
	fitrange.ulInGrabY		= 1;
	fitrange.ulInGrabW		= fitrange.ulInWidth;
	fitrange.ulInGrabH		= fitrange.ulInHeight;

    MMPD_PTZ_InitPtzInfo(m_fctlLinkPreview.scalerpath,
			             fitrange.fitmode,
			             fitrange.ulInWidth, fitrange.ulInHeight, 
			             fitrange.ulOutWidth, fitrange.ulOutHeight);

	MMPD_PTZ_GetCurPtzStep(m_fctlLinkPreview.scalerpath, NULL, &usCurZoomStep, NULL, NULL);

	MMPD_PTZ_CalculatePtzInfo(m_fctlLinkPreview.scalerpath, usCurZoomStep, 0, 0);

	MMPD_PTZ_GetCurPtzInfo(m_fctlLinkPreview.scalerpath, &fitrange, &grabctl);

	if (m_DscPreviewPath == MMP_DSC_RAW_PREVIEW) {
		MMPD_PTZ_ReCalculateGrabRange(&fitrange, &grabctl);
	}

	m_DSCPreviewFctlAttr.colormode      = m_dscConfig.dispParams.DispColorFmt[m_usDscPreviewMode];
	m_DSCPreviewFctlAttr.fctllink       = m_fctlLinkPreview;
	m_DSCPreviewFctlAttr.fitrange		= fitrange;
	m_DSCPreviewFctlAttr.grabctl        = grabctl;
	#if (SUPPORT_LDC_CAPTURE)
    m_DSCPreviewFctlAttr.scalsrc		= (m_bDscLdcEnable) ? (MMP_SCAL_SOURCE_LDC) : (MMP_SCAL_SOURCE_ISP);
    #else
    m_DSCPreviewFctlAttr.scalsrc		= MMP_SCAL_SOURCE_ISP;
    #endif
    m_DSCPreviewFctlAttr.bSetScalerSrc	= MMP_TRUE;
	m_DSCPreviewFctlAttr.usBufCnt       = m_dscConfig.dispParams.usDscDispBufCnt[m_usDscPreviewMode];

	/* Set the pipeline attribute (include scaling setting) and clear buffer */
	MMPD_Fctl_SetPipeAttrForIbcFB(&m_DSCPreviewFctlAttr);
	MMPD_Fctl_ClearPreviewBuf(m_fctlLinkPreview.ibcpipeID, 0xFFFFFF);

	/* Set Face detection interface */
	#if (SUPPORT_FDTC)
    if (MMPS_Fdtc_GetConfig()->bFaceDetectEnable) {
		MMPS_Sensor_FDPathInterface(MMPS_Fdtc_GetConfig()->faceDetectInputPath, &m_fctlLinkPreview, &m_fctlLinkCapture);
	}
    #endif
    
	/* Link preview to display or Rotate DMA */
    if (m_DSCPreviewFctlAttr.bUseRotateDMA) {
        MMPD_Fctl_LinkPipeToDma(m_fctlLinkPreview.ibcpipeID, 
        						m_dscConfig.dispParams.DispWinId[m_usDscPreviewMode], 
                           		m_dscConfig.dispParams.DispDevice[m_usDscPreviewMode],
                           		ubDmaRotateDir);
    }
    else {
        MMPD_Fctl_LinkPipeToDisplay(m_fctlLinkPreview.ibcpipeID, 
        							m_dscConfig.dispParams.DispWinId[m_usDscPreviewMode], 
                                	m_dscConfig.dispParams.DispDevice[m_usDscPreviewMode]);
    }
    
	/* Display Setting */
    if ((m_dscConfig.dispParams.DscDispDir[m_usDscPreviewMode] == MMP_DISPLAY_ROTATE_NO_ROTATE) ||
        (m_dscConfig.dispParams.DscDispDir[m_usDscPreviewMode] == MMP_DISPLAY_ROTATE_RIGHT_180)) {                        
        ulRotateW = ulPreviewBufW;
        ulRotateH = ulPreviewBufH;
    }
    else {
        ulRotateW = ulPreviewBufH;
        ulRotateH = ulPreviewBufW;
    }
    
    if (m_sAhcDscPrevInfo.bPreviewRotate || m_DSCPreviewFctlAttr.bUseRotateDMA) {
    	// Rotate 90/270 for vertical panel
    	ulRotateW = ulPreviewBufH;
        ulRotateH = ulPreviewBufW;
    }

    dispAttr.usStartX           = 0;
    dispAttr.usStartY           = 0;
    dispAttr.usDisplayWidth		= ulRotateW;
    dispAttr.usDisplayHeight	= ulRotateH;
    dispAttr.usDisplayOffsetX	= (ulDispWidth - ulRotateW) >> 1;
    dispAttr.usDisplayOffsetY	= (ulDispHeight - ulRotateH) >> 1; 
    dispAttr.bMirror            = m_dscConfig.dispParams.bDscDispMirror[m_usDscPreviewMode];
    dispAttr.rotatetype         = m_dscConfig.dispParams.DscDispDir[m_usDscPreviewMode];
    
    MMPD_Display_SetWinToDisplay(m_dscConfig.dispParams.DispWinId[m_usDscPreviewMode], &dispAttr);
    
	/* Set the scaler in the display module */
	if ((m_dscConfig.dispParams.bDscDispScaleEn[m_usDscPreviewMode] == MMP_TRUE) || 
		(m_sAhcDscPrevInfo.bUserDefine == MMP_TRUE) )
	{
        fitrange.fitmode        = sFitMode;
        fitrange.scalerType     = MMP_SCAL_TYPE_WINSCALER;
		fitrange.ulInWidth 		= ulRotateW;
        fitrange.ulInHeight		= ulRotateH;
        fitrange.ulOutWidth    	= dispAttr.usDisplayWidth;
        fitrange.ulOutHeight   	= dispAttr.usDisplayHeight;

        fitrange.ulInGrabX 		= 1;
        fitrange.ulInGrabY 		= 1;
        fitrange.ulInGrabW 		= fitrange.ulInWidth;
        fitrange.ulInGrabH 		= fitrange.ulInHeight;
        
        MMPD_Scaler_GetGCDBestFitScale(&fitrange, &DispGrabctl);

   	    retstatus =	MMPD_Display_SetWinScaling(m_dscConfig.dispParams.DispWinId[m_usDscPreviewMode], 
   	    									   &fitrange, &DispGrabctl);
	}
	
	/* Set RAW preview attribute */
    if (m_DscPreviewPath == MMP_DSC_RAW_PREVIEW)
    {
		MMP_USHORT  usStoreW, usStoreH;
		
		MMPD_RAWPROC_GetStoreRange(ubVifId, &usStoreW, &usStoreH);
        MMPD_RAWPROC_SetFetchRange(0, 0, usStoreW, usStoreH, usStoreW);

        MMPD_RAWPROC_EnablePath(ubSnrSel, ubVifId, MMP_TRUE, MMP_RAW_IOPATH_VIF2RAW | MMP_RAW_IOPATH_RAW2ISP, m_DscRawColorDepth);
        MMPD_RAWPROC_EnablePreview(ubVifId, MMP_TRUE);
    }
    else
    {
    	MMPD_RAWPROC_EnablePath(ubSnrSel, ubVifId, MMP_FALSE, MMP_RAW_IOPATH_NONE, MMP_RAW_COLORFMT_BAYER8);
    	MMPD_RAWPROC_EnablePreview(ubVifId, MMP_FALSE);
    }

	MMPS_DSC_RestoreDigitalZoomRange();

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_CustomedPreviewAttr
//  Description :
//------------------------------------------------------------------------------
/** 
 @brief The function set the display window for DSC
 @param[in] bUserConfig		Use user defined preview configuration.
 @param[in] bRotate			Use DMA rotate to rotate preview buffer.
 @param[in] usBufWidth		Preview buffer width.
 @param[in] usBufHeight		Preview buffer height.
 @param[in] usStartX 		The X Offset of the display window.
 @param[in] usStartY 		The Y Offset of the display window.
 @param[in] usWinWidth  	The width of the display window.
 @param[in] usWinHeight 	The height of the display window.
@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_CustomedPreviewAttr(MMP_BOOL 	bUserConfig,
									 MMP_BOOL 	bRotate,
									 MMP_UBYTE 	ubRotateDir,
									 MMP_UBYTE	sFitMode,
									 MMP_USHORT usBufWidth, MMP_USHORT usBufHeight, 
									 MMP_USHORT usStartX, 	MMP_USHORT usStartY,
                                     MMP_USHORT usWinWidth, MMP_USHORT usWinHeight)
{
    m_sAhcDscPrevInfo.bUserDefine 		= bUserConfig;
    m_sAhcDscPrevInfo.bPreviewRotate  	= bRotate;
    m_sAhcDscPrevInfo.sPreviewDmaDir	= ubRotateDir;
    m_sAhcDscPrevInfo.sFitMode			= sFitMode;
    m_sAhcDscPrevInfo.ulPreviewBufW		= usBufWidth;
    m_sAhcDscPrevInfo.ulPreviewBufH		= usBufHeight;
    m_sAhcDscPrevInfo.ulDispStartX   	= usStartX;
    m_sAhcDscPrevInfo.ulDispStartY   	= usStartY;
    m_sAhcDscPrevInfo.ulDispWidth    	= usWinWidth;
    m_sAhcDscPrevInfo.ulDispHeight		= usWinHeight;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_EnablePreviewDisplay
//  Description :
//------------------------------------------------------------------------------
/** @brief Enable and disable the DSC preview

The function enables or disables DSC preview display .

@param[in] bEnable 			enable or disable
@param[in] bCheckFrameEnd 	When "bEnable" is enable, the setting whill check frame end or not.

@retval MMP_ERR_NONE success
@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_EnablePreviewDisplay(MMP_UBYTE ubSnrSel, MMP_BOOL bEnable, MMP_BOOL bCheckFrameEnd)
{
	MMP_BOOL    bComplete = MMP_FALSE;
	MMP_UBYTE   ubVifId = MMPD_Sensor_GetVIFPad(ubSnrSel);
	
	if (!(bEnable ^ m_bDscPreviewActive)) {
		return MMP_ERR_NONE;
	}
	
    if (bEnable) {
        #if (SUPPORT_LDC_CAPTURE)
        MMPD_Fctl_EnablePreview(ubSnrSel, m_fctlLinkLdcSrc.ibcpipeID, MMP_TRUE, MMP_FALSE); 
        #endif
        
        if (MMPD_Fctl_EnablePreview(ubSnrSel, m_fctlLinkPreview.ibcpipeID, MMP_TRUE, bCheckFrameEnd)) {
			PRINTF("Enable Preview: Fail\r\n");
			return MMP_DISPLAY_ERR_FRAME_END;
        }	
		m_bDscPreviewActive = MMP_TRUE;
    }
    else {
    
		MMPD_PTZ_SetDigitalZoom(m_fctlLinkPreview.scalerpath, MMP_PTZ_ZOOMSTOP, MMP_TRUE);
		
        while (!bComplete) {
	        MMPD_PTZ_CheckZoomComplete(m_fctlLinkPreview.scalerpath, &bComplete);
	    }
	    
        #if (SUPPORT_MDTC)
        if (MMPS_Sensor_IsVMDEnable()) {
            MMPS_Sensor_EnableVMD(MMP_FALSE);
        }
        #endif
        #if (SUPPORT_ADAS)
        if (MMPS_Sensor_IsADASEnable()) {
            MMPS_Sensor_EnableADAS(MMP_FALSE);
        }
        #endif

        MMPD_Fctl_EnablePreview(ubSnrSel, m_fctlLinkPreview.ibcpipeID, MMP_FALSE, MMP_TRUE);
        #if (SUPPORT_LDC_CAPTURE)
        MMPD_Fctl_EnablePreview(ubSnrSel, m_fctlLinkLdcSrc.ibcpipeID, MMP_FALSE, MMP_TRUE);
        #endif

        if (m_DscPreviewPath == MMP_DSC_RAW_PREVIEW) {
            MMPD_RAWPROC_EnablePreview(ubVifId, MMP_FALSE);
            MMPD_RAWPROC_EnablePath(ubSnrSel, ubVifId, MMP_FALSE, MMP_RAW_IOPATH_VIF2RAW | MMP_RAW_IOPATH_RAW2ISP, m_DscRawColorDepth);
        }
        else {
        	MMPD_RAWPROC_EnablePreview(ubVifId, MMP_FALSE);
			MMPD_RAWPROC_EnablePath(ubSnrSel, ubVifId, MMP_FALSE, MMP_RAW_IOPATH_VIF2RAW | MMP_RAW_IOPATH_VIF2ISP, MMP_RAW_COLORFMT_BAYER8);
        }
        
        MMPD_Display_SetWinActive(m_dscConfig.dispParams.DispWinId[m_usDscPreviewMode], MMP_FALSE);
		
        #if (MCR_V2_UNDER_DBG)
        MMPD_IBC_ResetModule(m_fctlLinkPreview.ibcpipeID);
        MMPD_IBC_ResetModule(m_fctlLinkCapture.ibcpipeID);
        #if (SUPPORT_LDC_CAPTURE)
        MMPD_IBC_ResetModule(m_fctlLinkLdcSrc.ibcpipeID);
        #endif
        #endif

        #if (SUPPORT_LDC_CAPTURE)
        MMPD_Fctl_UnLinkPipeToLdc(m_fctlLinkLdcSrc.ibcpipeID);
        #endif
		
		m_bDscPreviewActive = MMP_FALSE;
		m_usDSCStaticZoomIndex = 0;
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_GetPreviewStatus
//  Description :
//------------------------------------------------------------------------------
/** @brief Enable and disable the DSC preview

The function enables or disables DSC preview display .

@param[in] bEnable enable or disable
@retval MMP_ERR_NONE success
@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_GetPreviewStatus(MMP_BOOL *bEnable)
{
	*bEnable = m_bDscPreviewActive;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_RestorePreview
//  Description :
//------------------------------------------------------------------------------
/** @brief The function use in restore preview

The function restore preview for multishot capture

@param[in] usWindowsTime 	 The time of review time
@param[in] bChangeSensorBack The change the sensor resolution as preview mode
@return MMP_ERR_NONE
*/
MMP_ERR MMPS_DSC_RestorePreview(MMP_UBYTE ubSnrSel, MMP_USHORT usWindowsTime)
{
	return MMPD_DSC_RestorePreview( ubSnrSel,
	                                m_dscConfig.encParams.bDscPreviewKeepOn,
							        usWindowsTime);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_GetPreviewPipe
//  Description :
//------------------------------------------------------------------------------
void MMPS_DSC_GetPreviewPipe(MMP_IBC_PIPEID *pPipe)
{
    *pPipe = m_fctlLinkPreview.ibcpipeID;
}

#if 0
void _____Preview_PTZ_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetZoomConfig
//  Description :
//------------------------------------------------------------------------------
/** @brief The function set zoom config

The function set zoom config, total step and max zoom ratio

@param[in] ubSteps 		zoom total step
@param[in] ubMaxRatio 	max zoom ratio
@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_SetZoomConfig(MMP_USHORT usMaxSteps, MMP_USHORT usMaxRatio)
{
    m_DscPreviewZoomInfo.usMaxZoomSteps     = usMaxSteps;
    m_DscPreviewZoomInfo.usMaxZoomRatio   	= usMaxRatio;
    
    m_bAhcConfigDscZoom            			= MMP_TRUE;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_InitDigitalZoomParam
//  Description :
//------------------------------------------------------------------------------
static MMP_ERR MMPS_DSC_InitDigitalZoomParam(void)
{
	m_DscPreviewZoomInfo.scalerpath 	= m_fctlLinkPreview.scalerpath;
	m_DscPreviewZoomInfo.usCurZoomStep 	= 0;
   	m_DscPreviewZoomInfo.usMaxZoomRatio = gsDscPtzCfg.usMaxZoomRatio;
	m_DscPreviewZoomInfo.usMaxZoomSteps = gsDscPtzCfg.usMaxZoomSteps;
	m_DscPreviewZoomInfo.sMaxPanSteps	= gsDscPtzCfg.sMaxPanSteps;
	m_DscPreviewZoomInfo.sMinPanSteps	= gsDscPtzCfg.sMinPanSteps;
	m_DscPreviewZoomInfo.sMaxTiltSteps	= gsDscPtzCfg.sMaxTiltSteps;
	m_DscPreviewZoomInfo.sMinTiltSteps	= gsDscPtzCfg.sMinTiltSteps;
   	
    MMPD_PTZ_InitPtzRange(m_DscPreviewZoomInfo.scalerpath,
    					  m_DscPreviewZoomInfo.usMaxZoomRatio,
    					  m_DscPreviewZoomInfo.usMaxZoomSteps,
			              m_DscPreviewZoomInfo.sMaxPanSteps,
			              m_DscPreviewZoomInfo.sMinPanSteps,
			              m_DscPreviewZoomInfo.sMaxTiltSteps,
			              m_DscPreviewZoomInfo.sMinTiltSteps);

	MMPD_PTZ_SetDigitalZoom(m_DscPreviewZoomInfo.scalerpath, MMP_PTZ_ZOOMSTOP, MMP_TRUE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_RestoreDigitalZoomRange
//  Description :
//------------------------------------------------------------------------------
static MMP_ERR MMPS_DSC_RestoreDigitalZoomRange(void)
{
	if (m_usDSCStaticZoomIndex) {

	    m_DscPreviewZoomInfo.usCurZoomStep = m_usDSCStaticZoomIndex;
		
    	MMPD_PTZ_SetTargetPtzStep(m_DscPreviewZoomInfo.scalerpath, 
    							  MMP_PTZ_ZOOM_INC_IN, 
    							  m_DscPreviewZoomInfo.usCurZoomStep, 0, 0);

    	MMPD_PTZ_SetDigitalZoomOP(m_ubDscModeSnrId,
    	                          m_DscPreviewZoomInfo.scalerpath,
                                  MMP_PTZ_ZOOMIN,
                                  MMP_TRUE);

		// Reset zoom index
        m_usDSCStaticZoomIndex = 0;
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_GetCurZoomStep
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_GetCurZoomStep(MMP_USHORT *usCurZoomStep)
{
    MMP_USHORT  usCurStep = 0;
    MMP_ERR     status = MMP_ERR_NONE;

    status = MMPD_PTZ_GetCurPtzStep(m_DscPreviewZoomInfo.scalerpath,
                                    NULL, &usCurStep, NULL, NULL);
    
    *usCurZoomStep = usCurStep;

    if (m_bDscPreviewActive) {
        m_DscPreviewZoomInfo.usCurZoomStep = usCurStep;
    }

    return status;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_GetCurZoomStatus
//  Description :
//------------------------------------------------------------------------------
MMP_UBYTE MMPS_DSC_GetCurZoomStatus(void)
{
	return MMPD_PTZ_GetCurPtzStatus();
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetPreviewZoom
//  Description :
//------------------------------------------------------------------------------
/** @brief Setup the zoom parameter and start the zoom operation

The function configures the digital zoom parameter and starts the zoom operation
@param[in] zoomdir 	the zoom in , zoom out or zoom stop
@param[in] usCurZoomStep
@retval MMP_ERR_NONE success
@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_SetPreviewZoom(MMP_PTZ_ZOOM_DIR sZoomDir, MMP_USHORT usCurZoomStep)
{
    MMP_BOOL 			bComplete = MMP_FALSE;
	MMP_PTZ_ZOOM_INC  	sZoomInc;
	
    if (!m_bDscPreviewActive) {
    	m_usDSCStaticZoomIndex = usCurZoomStep;
    	return MMP_ERR_NONE;
    }

	/* Decide the zoom step increment */
	if (sZoomDir == MMP_PTZ_ZOOMIN) {
        sZoomInc = MMP_PTZ_ZOOM_INC_IN;
    }
    else if (sZoomDir == MMP_PTZ_ZOOMOUT) {
        sZoomInc = MMP_PTZ_ZOOM_INC_OUT;
    }

	MMPD_PTZ_CheckZoomComplete(m_DscPreviewZoomInfo.scalerpath, &bComplete);

    if (!bComplete) {
    
        MMPD_PTZ_SetDigitalZoom(m_DscPreviewZoomInfo.scalerpath, MMP_PTZ_ZOOMSTOP, MMP_TRUE);
        
        do {
        	MMPD_PTZ_CheckZoomComplete(m_DscPreviewZoomInfo.scalerpath, &bComplete);
        } while(!bComplete);
    }

	if (sZoomDir == MMP_PTZ_ZOOMIN) {
		MMPD_PTZ_SetTargetPtzStep(m_DscPreviewZoomInfo.scalerpath, 
	    						  sZoomInc, 
	    						  m_DscPreviewZoomInfo.usMaxZoomSteps, 0, 0);
	}
	else if (sZoomDir == MMP_PTZ_ZOOMOUT) {
		MMPD_PTZ_SetTargetPtzStep(m_DscPreviewZoomInfo.scalerpath, 
	    						  sZoomInc, 
	    						  0, 0, 0);
	}

	MMPD_PTZ_SetDigitalZoom(m_DscPreviewZoomInfo.scalerpath, sZoomDir, MMP_TRUE);

    return MMP_ERR_NONE;
}

#if 0
void _____MJPEG_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetMJpegResol
//  Description :
//------------------------------------------------------------------------------
/** @brief The function set Motion JPEG resolution.
@param[in] usW The width of image
@param[in] usH The height of image
@return MMP_ERR_NONE
*/
MMP_ERR MMPS_DSC_SetMJpegResol(MMP_USHORT usW, MMP_USHORT usH)
{
    m_usMJpegWidth  = usW;
    m_usMJpegHeight = usH;
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetVideo2MJpegEnable
//  Description :
//------------------------------------------------------------------------------
/** @brief The function set video decode to motion JPEG enable.
@param[in] bEnable enable Motion Jpeg encode.
@return MMP_ERR_NONE
*/
MMP_ERR MMPS_DSC_SetVideo2MJpegEnable(MMP_BOOL bEnable)
{
#if (VIDEO_DEC_TO_MJPEG)
    m_bVideo2MJpegEnable = bEnable;
    MMPD_DSC_SetVideo2MJPEGEnable(bEnable);
#endif    
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_GetVideo2MJpegEnable
//  Description :
//------------------------------------------------------------------------------
/** @brief The function get video decode to motion JPEG enable.

The function get video decode to motion JPEG enable.

@param[out] bEnable enable Motion Jpeg encode.
@return MMP_ERR_NONE
*/
MMP_BOOL MMPS_DSC_GetVideo2MJpegEnable(void)
{
#if (VIDEO_DEC_TO_MJPEG)
	return m_bVideo2MJpegEnable;
#else
	return MMP_FALSE;
#endif	
}

#if (VIDEO_DEC_TO_MJPEG)
//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetTranscodeRate
//  Description :
//------------------------------------------------------------------------------
/** @brief Config transcoding Parameters
 *
 * @post This should be called before MMPS_VIDPLAY_Open
 * @param[in] fpsx10 frame per second x 10. So that it could be something like 7.5FPS
 * @param[in] ulBitrate The bit rate of the output file.
 * @return MMP_ERR_NONE
 */
MMP_ERR MMPS_DSC_SetTranscodeRate(MMP_ULONG InFpsx10, MMP_ULONG OutFpsx10, MMP_ULONG ulBitrate)
{
	m_transcode.InFpsx10 	= InFpsx10;
	m_transcode.OutFpsx10 	= OutFpsx10;
	m_transcode.ulBitrate 	= ulBitrate;
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_ConfigVideo2Mjpeg
//  Description : Config the WiFi transcoding Parameters
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_ConfigVideo2Mjpeg(MMP_SHORT usInputW, MMP_USHORT usInputH)
{
    MMP_ERR 					error = MMP_ERR_NONE;
	MMP_SCAL_GRAB_CTRL			grabctl;
	MMP_SCAL_FIT_RANGE			fitrange;
	MMPD_FCTL_LINK				fctllink = {MMP_SCAL_PIPE_0, MMP_ICO_PIPE_0, MMP_IBC_PIPE_0};
	MMP_GRAPHICS_BUF_ATTR 		bufAttr;
	MMP_GRAPHICS_RECT           rect;
	MMP_MJPEG_OBJ_PTR			handle;
	MMP_ULONG                   ulTargetSize;
	MMP_ULONG  					ulSuggestWidth, ulSuggestHeight;
	MMP_ULONG 					ulDramAddr, ulBufSize;
	
	if (m_transcode.OutFpsx10 == 0) {
		return MMP_DSC_ERR_PARAMETER;
	}
    
    /* Reset relative module */
	MMPD_Scaler_ResetModule(fctllink.scalerpath);
	MMPD_Icon_ResetModule(fctllink.icopipeID);
	MMPD_IBC_ResetModule(fctllink.ibcpipeID);
	MMPD_System_ResetHModule(MMPD_SYS_MDL_JPG, MMP_FALSE);
	MMPD_System_ResetHModule(MMPD_SYS_MDL_GRA, MMP_FALSE);
	
	bufAttr.usWidth         = usInputW;
	bufAttr.usHeight        = usInputH;
	bufAttr.usLineOffset    = bufAttr.usWidth;
	bufAttr.colordepth      = MMP_GRAPHICS_COLORDEPTH_YUV420_INTERLEAVE;
	bufAttr.ulBaseAddr      = bufAttr.ulBaseUAddr = bufAttr.ulBaseVAddr = 0x0;

	rect.usLeft             = 0;
	rect.usTop              = 0;
	rect.usWidth            = bufAttr.usWidth;
	rect.usHeight           = bufAttr.usHeight;

	MMPS_Display_PureAdjustScaleInSize(	MMP_FALSE, 
										usInputW, usInputH, 
										m_usMJpegWidth, m_usMJpegHeight,
										&ulSuggestWidth, &ulSuggestHeight);

	// Aligned with JEPG HW
	m_usMJpegWidth = ALIGN16(ulSuggestWidth);
	m_usMJpegHeight = ALIGN8(ulSuggestHeight);

	fitrange.fitmode        = MMP_SCAL_FITMODE_OUT;
	fitrange.scalerType 	= MMP_SCAL_TYPE_SCALER;
	fitrange.ulInWidth      = rect.usWidth;
	fitrange.ulInHeight     = rect.usHeight;
	fitrange.ulOutWidth     = m_usMJpegWidth;
	fitrange.ulOutHeight    = m_usMJpegHeight;

	fitrange.ulInGrabX  	= 1;
   	fitrange.ulInGrabY  	= 1;
   	fitrange.ulInGrabW  	= fitrange.ulInWidth;
	fitrange.ulInGrabH  	= fitrange.ulInHeight;

	MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);

	ulTargetSize = ((m_transcode.ulBitrate * 10 / m_transcode.OutFpsx10) >> (3 + 10)) + 1;
	printc("%s %d: %d %d\n", __func__, __LINE__, m_transcode.ulBitrate, ulTargetSize);

	MMPD_IBC_SetH264RT_Enable(MMP_FALSE);

	error = MMPS_MJPEG_OpenMjpeg(MMP_WIFI_STREAM_FRONTCAM_VIDEO, MMP_WIFI_MODE_VR, &handle);

	MMPS_DSC_SetCaptureJpegQuality(MMP_DSC_JPEG_RC_ID_MJPEG_1ST_STREAM, MMP_TRUE, MMP_FALSE, 
	                               ulTargetSize, ulTargetSize, 1, MMP_DSC_CAPTURE_HIGH_QUALITY);
	
	MMPS_DSC_GetMJpegCompBuf(&ulDramAddr, &ulBufSize);
	MMPD_Streaming_SetCompBuf(VIDEO1_RING_STREAM, ulDramAddr, ulBufSize);

	MMPD_DSC_InitGra2MJPEGAttr(fctllink, &bufAttr, &rect, &fitrange, &grabctl);
	MMPD_Streaming_Enable(VIDEO1_RING_STREAM, MMP_TRUE);
	
	return error;
}
#endif

#if 0
void _____Capture_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_CheckContiShotStreamBusy
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_CheckContiShotStreamBusy(MMP_ULONG *pbBusy)
{
    return MMPD_DSC_CheckContiShotStreamBusy(pbBusy);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetContiShotMaxNum
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_SetContiShotMaxNum(MMP_ULONG ulMaxShotNum)
{
    m_ulContiShotMaxNum = ulMaxShotNum;
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetContiShotInterval
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_SetContiShotInterval(MMP_ULONG ulInterval)
{    
    m_ulContiShotInterval = ulInterval; 
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetContiShotMaxDramAddr
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_SetContiShotMaxDramAddr(MMP_ULONG ulAddr)
{
	m_ulDSCMaxDramAddr = ulAddr;
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_EnableScaleUpCapture
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_EnableScaleUpCapture(MMP_BOOL bScaleUpCaptureEn)
{	    
    m_bScaleUpCapture = bScaleUpCaptureEn;
    
    return MMPD_DSC_EnableScaleUpCapture(bScaleUpCaptureEn);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetJpegEncParam
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_SetJpegEncParam(MMP_USHORT usJpegResol, MMP_USHORT usW, MMP_USHORT usH, MMP_UBYTE ubRcIdx)
{
	if (usJpegResol != DSC_RESOL_IDX_UNDEF) {

		m_usJpegWidth  = m_dscConfig.encParams.usDscEncW[usJpegResol];
		m_usJpegHeight = m_dscConfig.encParams.usDscEncH[usJpegResol];
    }
    else {
	    m_usJpegWidth  = usW;
	    m_usJpegHeight = usH;
    }

    #if (SUPPORT_LDC_CAPTURE)
    if (m_usJpegWidth > m_ulDscLdcMaxSrcW)
    	m_usJpegWidth = m_ulDscLdcMaxSrcW;
    if (m_usJpegHeight > m_ulDscLdcMaxSrcH)
    	m_usJpegHeight = m_ulDscLdcMaxSrcH;
    #endif

    MMPD_DSC_SetJpegResol(m_usJpegWidth, m_usJpegHeight, ubRcIdx);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetCapturePipe
//  Description :
//------------------------------------------------------------------------------
/** @brief
@param[in] ubPipe	which pipe for capture want to use.
@return It reports the status of the configuration.
*/
MMP_ERR MMPS_DSC_SetCapturePipe(MMP_UBYTE ubPipe)
{	
	m_fctlLinkCapture.scalerpath = (MMP_SCAL_PIPEID) ubPipe;
	m_fctlLinkCapture.icopipeID	 = (MMP_ICO_PIPEID) ubPipe;
	m_fctlLinkCapture.ibcpipeID	 = (MMP_IBC_PIPEID) ubPipe;
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetShotMode
//  Description :
//------------------------------------------------------------------------------
/** @brief Configure the shot mode of DSC

The function configures the shot mode of DSC
@param[in] ubMode the capture modes to be selected

@retval MMP_ERR_NONE success
@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_SetShotMode(MMPS_DSC_CAPTUREMODE ubMode)
{
	m_usCaptureMode = (MMP_USHORT) ubMode;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetCustomQTable
//  Description :
//------------------------------------------------------------------------------
/** @brief The function set custom q table

The function set global custom q table(zigzag order)

@param[in] ubMode Set custom q table mode: Off/All/Main/Thumbnail
@param[in] pTable Q table value
@return MMP_ERR_NONE
*/
MMP_ERR MMPS_DSC_SetCustomQTable(MMP_UBYTE ubMode, MMP_UBYTE *pTable, MMP_UBYTE *pTableU, MMP_UBYTE *pTableV)
{
    MMP_ULONG i;

    switch(ubMode) {
        case MMPS_DSC_CUSTOM_QTABLE_OFF:
            m_bUseCustomJpegQT = MMP_FALSE;
            m_bUseCustomThumbQT = MMP_FALSE;
            break;
        case MMPS_DSC_CUSTOM_QTABLE_ALL:
            m_bUseCustomJpegQT = MMP_TRUE;
            m_bUseCustomThumbQT = MMP_TRUE;

            for (i = 0; i < DSC_QTABLE_ARRAY_SIZE; i++) {
                m_ubCustomJpegQT[0][i]  = pTable[i];
                m_ubCustomThumbQT[0][i] = pTable[i];
                
                m_ubCustomJpegQT[1][i]  = pTableU[i];
                m_ubCustomThumbQT[1][i] = pTableU[i];
                
                m_ubCustomJpegQT[2][i]  = pTableV[i];
                m_ubCustomThumbQT[2][i] = pTableV[i];
            }
            break;
        case MMPS_DSC_CUSTOM_QTABLE_MAIN:
            m_bUseCustomJpegQT = MMP_TRUE;

            for (i = 0; i < DSC_QTABLE_ARRAY_SIZE; i++) {
                m_ubCustomJpegQT[0][i] = pTable[i];
                m_ubCustomJpegQT[1][i] = pTableU[i];
                m_ubCustomJpegQT[2][i] = pTableV[i];
            }
            break;
        case MMPS_DSC_CUSTOM_QTABLE_THUMB:
            m_bUseCustomThumbQT = MMP_TRUE;

            for (i = 0; i < DSC_QTABLE_ARRAY_SIZE; i++) {
                m_ubCustomThumbQT[0][i] = pTable[i];
                m_ubCustomThumbQT[1][i] = pTableU[i];
                m_ubCustomThumbQT[2][i] = pTableV[i];
            }
            break;
    }
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetCaptureJpegQuality
//  Description :
//------------------------------------------------------------------------------
/** @brief The function sets Jpeg output quality for image capturing

The function sets Jpeg output quality for image capturing. 
The function calls MMPD_DSC_SetJpegQuality to set the Jpeg quality 
and adjusts the quality by calling MMPD_DSC_SetJpegQualityControl with the proper capturing Jpeg size.

@param[in] bTargetCtrl	the flag of target control
@param[in] bLimitCtrl 	the flag of limit control
@param[in] ulTargetSize the rate control target size (Unit:KB)
@param[in] ulLimitSize  the rate control limit size (Unit:KB)
@param[in] usMaxCount 	the maximum count for the Jpeg encode trial
@param[in] qualitylevel jpeg quality level

@retval MMP_ERR_NONE success
@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_SetCaptureJpegQuality(	MMP_UBYTE               ubRcIdx,
                                        MMP_BOOL 				bTargetCtrl, 
										MMP_BOOL 				bLimitCtrl,
										MMP_ULONG   			ulTargetSize,
										MMP_ULONG   			ulLimitSize,
                                        MMP_USHORT 				usMaxCount, 
                                        MMP_DSC_JPEG_QUALITY 	qualitylevel)
{
	if (bTargetCtrl && bLimitCtrl && (ulLimitSize < ulTargetSize * 11 / 10)) {
		PRINTF("ulLimitSize should larger then ulTargetSize * 110%");
		return MMP_ERR_NONE;
	}

	m_ulImageTargetSize = ulTargetSize;

    if (bLimitCtrl == MMP_FALSE) {
        ulLimitSize = ulTargetSize;
	}
	
    MMPD_DSC_SetJpegQualityControl(	ubRcIdx,
                                    bTargetCtrl,
		    	                    bLimitCtrl,
		    	                    ulTargetSize,
		    	                    ulLimitSize,
		    	                    usMaxCount);
    if (!m_bUseCustomJpegQT) {
	    MMPD_DSC_SetJpegQTable((MMP_UBYTE *)m_dscConfig.encParams.ubDscQtable[qualitylevel],
	                           (MMP_UBYTE *)m_dscConfig.encParams.ubDscQtable[qualitylevel] + DSC_QTABLE_ARRAY_SIZE,
	                           (MMP_UBYTE *)m_dscConfig.encParams.ubDscQtable[qualitylevel] + DSC_QTABLE_ARRAY_SIZE);
	} 
	else {
	    MMPD_DSC_SetJpegQTable(m_ubCustomJpegQT[0], m_ubCustomJpegQT[1], m_ubCustomJpegQT[2]);
	}
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetCaptureJpegQualityEX
//  Description :
//------------------------------------------------------------------------------
/** @brief The function sets Jpeg output quality for image capturing

The function sets Jpeg output quality for image capturing. 
The function calls MMPD_DSC_SetJpegQuality to set the Jpeg quality 
and adjusts the quality by calling MMPD_DSC_SetJpegQualityControl with the proper capturing Jpeg size.

@param[in] bTargetCtrl	the flag of target control
@param[in] bLimitCtrl 	the flag of limit control
@param[in] ulTargetSize the rate control target size (Unit:KB)
@param[in] ulLimitSize  the rate control limit size (Unit:KB)
@param[in] usMaxCount 	the maximum count for the Jpeg encode trial
@param[in] qualitylevel jpeg quality level

@retval MMP_ERR_NONE success
@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_SetCaptureJpegQualityEx(	MMP_UBYTE               ubRcIdx,
                                        MMP_BOOL 				bTargetCtrl, 
										MMP_BOOL 				bLimitCtrl,
										MMP_ULONG   			ulTargetSize,
										MMP_ULONG   			ulLimitSize,
                                        MMP_USHORT 				usMaxCount, 
                                        MMP_DSC_JPEG_QUALITY 	qualitylevel)
{
	if (bTargetCtrl && bLimitCtrl && (ulLimitSize < ulTargetSize * 11 / 10)) {
		PRINTF("ulLimitSize should larger then ulTargetSize * 110%");
		return MMP_ERR_NONE;
	}

	m_ulImageTargetSize = ulTargetSize;

    if (bLimitCtrl == MMP_FALSE) {
        ulLimitSize = ulTargetSize;
	}
	
    MMPD_DSC_SetJpegQualityControl(	ubRcIdx,
                                    bTargetCtrl,
		    	                    bLimitCtrl,
		    	                    ulTargetSize,
		    	                    ulLimitSize,
		    	                    usMaxCount);
    if (!m_bUseCustomJpegQT) {
	    MMPD_DSC_SetJpegQTableEx(ubRcIdx, (MMP_UBYTE *)m_dscConfig.encParams.ubDscQtable[qualitylevel],
	                           (MMP_UBYTE *)m_dscConfig.encParams.ubDscQtable[qualitylevel] + DSC_QTABLE_ARRAY_SIZE,
	                           (MMP_UBYTE *)m_dscConfig.encParams.ubDscQtable[qualitylevel] + DSC_QTABLE_ARRAY_SIZE, qualitylevel);
	} 
	else {
	    MMPD_DSC_SetJpegQTableEx(ubRcIdx, m_ubCustomJpegQT[0], m_ubCustomJpegQT[1], m_ubCustomJpegQT[2], MMP_DSC_JPEG_QT_CUSTOMER);
	}
    return MMP_ERR_NONE;
}

#if (HANDLE_JPEG_EVENT_BY_QUEUE)
//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_CaptureByQueue
//  Description : For video snapshot by Queue
//------------------------------------------------------------------------------
/** @brief preview and capture at the same pipe.

@retval MMP_ERR_NONE success
@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_CaptureByQueue(MMP_IBC_PIPEID 			        pipeID,
						        MMPS_DSC_CAPTURE_INFO 	        *pCaptureInfo,
						        MMP_SCAL_SOURCE			        ScalerSrc,
							    MMP_SCAL_FIT_RANGE 		        *pCaptureFitrange,
						        MMP_SCAL_GRAB_CTRL 		        *pCaptureGrabctl,
						        MMP_ULONG              	        *pCaptureSize,
							    MMP_ULONG                       ulDramAddr,
							    MMP_ULONG                       ulSramAddr,
							    MMP_ULONG                       ulFrmBufAddr,
							    MMPS_3GPRECD_STILL_CAPTURE_INFO *pVRCaptInfo,
							    MMP_BOOL                        bRecording)
{
    MMP_ULONG				ulThumbnailSize = 0;
    //MMP_SCAL_SOURCE         ScalerSrcTmp;
    MMP_IBC_PIPE_ATTR       pipeAttr;
    MMP_DSC_JPEG_INFO       jpegInfo;
    MMP_IBC_LINK_TYPE 		IBCLinkType;
    MMP_DISPLAY_DEV_TYPE	previewDev;
    MMP_DISPLAY_WIN_ID		winID;
    MMP_DISPLAY_ROTATE_TYPE rotateDir;
    MMPD_FCTL_ATTR  		fctlAttr;
    MMPD_FCTL_LINK			fctllink;
    MMP_ERR                 err = MMP_ERR_NONE;
    MMP_ULONG               ulEncW, ulEncH;
    MMP_ULONG               i;

    //extern MMPD_FCTL_ATTR   m_VRPreviewFctlAttr[];

    MMPS_DSC_SetShotMode(MMPS_DSC_SINGLE_SHOTMODE);
    MMPS_DSC_SetJpegEncParam(DSC_RESOL_IDX_UNDEF, pVRCaptInfo->usWidth, pVRCaptInfo->usHeight, MMP_DSC_JPEG_RC_ID_CAPTURE);

    MMPS_DSC_ConfigThumbnail(pVRCaptInfo->usThumbWidth, pVRCaptInfo->usThumbHeight, MMP_DSC_THUMB_SRC_NONE);

    MMPS_DSC_SetCaptureJpegQualityEx(MMP_DSC_JPEG_RC_ID_CAPTURE,
                                   pVRCaptInfo->bTargetCtl, pVRCaptInfo->bLimitCtl, pVRCaptInfo->bTargetSize, 
    							   pVRCaptInfo->bLimitSize, pVRCaptInfo->bMaxTrialCnt, pVRCaptInfo->Quality);

    MMPS_DSC_SetSystemBuf(&ulDramAddr, MMP_FALSE, MMP_TRUE, MMP_TRUE);
    MMPS_DSC_SetCaptureBuf(&ulSramAddr, &ulDramAddr, MMP_TRUE, MMP_FALSE, MMP_DSC_CAPTURE_NO_ROTATE, MMP_TRUE);

	pCaptureInfo->bFirstShot 	= MMP_TRUE;
	pCaptureInfo->bExif 	    = pVRCaptInfo->bExifEn;
    pCaptureInfo->bThumbnail    = pVRCaptInfo->bThumbEn;
    
    /* Control the flow of encoding JPEG */
	m_MultiShotInfo.usShotIndex		= 0;
	m_MultiShotInfo.usStoreIndex	= 0;
	m_MultiShotInfo.ulPrimaryJpegAddr[m_MultiShotInfo.usShotIndex] = m_ulPrimaryJpegCompStart;

    #if (USE_H264_CUR_BUF_AS_CAPT_BUF) 
    if (bRecording) {
        MMP_IBC_PIPEID ubRecPipe = 0;
        MMP_ULONG ulBufW, ulBufH;
            
        MMPS_3GPRECD_GetRecordPipe(0, &ubRecPipe);
        MMPS_3GPRECD_GetRecordPipeBufWidth_Height(0, &ulBufW, &ulBufH);
        ulBufH = (ulBufH == 1088) ? 1080 : ulBufH;
        MMPF_Display_UseH264Cur2StillJpeg((MMP_UBYTE)ubRecPipe, ulBufW, ulBufH, pVRCaptInfo->usWidth, pVRCaptInfo->usHeight);
    }
    else
    #endif
    {
    	FCTL_PIPE_TO_LINK(pipeID, fctllink);

    	/* Back up the Scaler and IBC setting and close the IBC of preview path */
        //MMPD_Scaler_GetPath((MMP_SCAL_PIPEID) pipeID, &ScalerSrcTmp);
    	MMPD_Scaler_BackupAttributes((MMP_SCAL_PIPEID) pipeID);
        /* Because of changing pipeline from pipe3(preview) to pipe0 */
        /* There's no need to clear FrameEnd and close StoreEnable */
    	//MMPD_IBC_ClearFrameEnd(pipeID);
    	//MMPD_IBC_CheckFrameEnd(pipeID);
    	//MMPD_IBC_SetStoreEnable(pipeID, MMP_FALSE);

    	MMPD_Fctl_GetIBCLinkAttr(pipeID, &IBCLinkType, &previewDev, &winID, &rotateDir);
    	MMPD_Fctl_ResetIBCLinkType(pipeID);	

    	pipeAttr.function = MMP_IBC_FX_TOFB;
    	MMPD_IBC_GetAttributes(pipeID, &pipeAttr);

        /* Change the path to LDC in order to keep the setting of pipe normal */
        MMPD_Scaler_SetPath(pipeID, MMP_SCAL_SOURCE_LDC, MMP_TRUE);

        fctlAttr.colormode      = MMP_DISPLAY_COLOR_YUV420_INTERLEAVE;
        fctlAttr.fctllink       = fctllink;
        fctlAttr.fitrange       = *pCaptureFitrange;
        fctlAttr.grabctl        = *pCaptureGrabctl;
        fctlAttr.scalsrc		= ScalerSrc;
        fctlAttr.bSetScalerSrc	= MMP_FALSE;
        fctlAttr.usBufCnt  		= 2;

        ulEncW = pCaptureGrabctl->ulOutEdX - pCaptureGrabctl->ulOutStX + 1;
        ulEncH = pCaptureGrabctl->ulOutEdY - pCaptureGrabctl->ulOutStY + 1;

        for (i = 0; i < fctlAttr.usBufCnt; i++) {
            fctlAttr.ulBaseAddr[i]  = ulFrmBufAddr;
            fctlAttr.ulBaseUAddr[i] = ALIGN32(ulFrmBufAddr + (ulEncW * ulEncH));
            fctlAttr.ulBaseVAddr[i] = 0;
        }

        fctlAttr.bUseRotateDMA  = MMP_FALSE;
        fctlAttr.usRotateBufCnt = 0;

        //MMPD_Scaler_ResetModule(pipeID); 
        //MMPD_Icon_ResetModule(pipeID);
        //MMPD_IBC_ResetModule(pipeID);

        MMPD_Fctl_SetPipeAttrForIbcFB(&fctlAttr);
        
        MMPD_Fctl_LinkPipeToGra2JPEG(pipeID, winID, previewDev, MMP_TRUE, MMP_FALSE);
        
        /* Change the path to the frame source */
        MMPF_IBC_SetInterruptEnable(pipeID, MMP_IBC_EVENT_FRM_RDY, MMP_TRUE);
        MMPD_IBC_SetStoreEnable(pipeID, MMP_TRUE);
        MMPD_Scaler_SetPath(pipeID, ScalerSrc, MMP_TRUE);
        MMPD_IBC_ClearFrameReady(pipeID);
        MMPD_IBC_CheckFrameReady(pipeID);
        MMPD_IBC_SetStoreEnable(pipeID, MMP_FALSE);
        MMPF_IBC_SetInterruptEnable(pipeID, MMP_IBC_EVENT_FRM_RDY, MMP_FALSE);
        MMPD_Scaler_SetPath(pipeID, MMP_SCAL_SOURCE_LDC, MMP_TRUE);
    }
    
    /* Check the size of JPEG is over the compressed buffer or not */
    MMPD_DSC_GetJpegSizeAfterEncodeDone(&(pCaptureInfo->ulJpegFileSize));
    
    *pCaptureSize = pCaptureInfo->ulJpegFileSize;
	
	m_MultiShotInfo.ulPrimaryJpegSize[m_MultiShotInfo.usShotIndex] = pCaptureInfo->ulJpegFileSize;
	
    m_MultiShotInfo.ulUsedEndAddr[m_MultiShotInfo.usShotIndex] = m_MultiShotInfo.ulPrimaryJpegAddr[m_MultiShotInfo.usShotIndex] 
    														   + m_MultiShotInfo.ulPrimaryJpegSize[m_MultiShotInfo.usShotIndex];
    
    if (m_MultiShotInfo.ulUsedEndAddr[m_MultiShotInfo.usShotIndex] > m_ulPrimaryJpegCompEnd) {
        
        printc("The Compressed Data is over JPEG compressed buffer %x/%x/%x/%x\r\n",
                m_MultiShotInfo.ulUsedEndAddr[m_MultiShotInfo.usShotIndex],
                m_ulPrimaryJpegCompStart,
                m_ulPrimaryJpegCompEnd,
                pCaptureInfo->ulJpegFileSize);
        err = MMP_DSC_ERR_CAPTURE_BUFFER_OVERFLOW;
        goto L_Restore;
    }
	
	/* Encode Exif and Thumbnail */
	if (pCaptureInfo->bExif) {
		
		MMPD_DSC_SetCardModeExifEnc(EXIF_NODE_ID_PRIMARY,
									pCaptureInfo->bExif, 
			                        pCaptureInfo->bThumbnail,
			                        m_ulDSCThumbWidth, 
			                        m_ulDSCThumbHeight);

        if (pCaptureInfo->bThumbnail) {

			jpegInfo.usPrimaryWidth = m_usJpegWidth;
    		jpegInfo.usPrimaryHeight = m_usJpegHeight;

            m_MultiShotInfo.ulExifThumbnailAddr[m_MultiShotInfo.usShotIndex] = ALIGN_X(m_MultiShotInfo.ulUsedEndAddr[m_MultiShotInfo.usShotIndex], DSC_BUF_ALIGN_BASE);
            
            MMPS_DSC_EncodeThumbnail(	pCaptureInfo,
            							&jpegInfo,
								 		m_ulDSCThumbWidth, 
								 		m_ulDSCThumbHeight, 
								 		&ulThumbnailSize,
										m_thumbnailInputMode,
										pipeID);
        	
        	/* m_MultiShotInfo.ulUsedEndAddr[] will be updated in MMPS_DSC_EncodeThumbnail() */
        	
        	MMPS_DSC_UpdateExifBuffer(EXIF_NODE_ID_PRIMARY, IFD_IDX_IFD1, IFD1_THUMBNAIL_LENGTH, EXIF_TYPE_LONG, 1, (MMP_UBYTE*)(&ulThumbnailSize), 4, MMP_FALSE);
            *pCaptureSize += ulThumbnailSize;
        }

        #if (EXIF_MANUAL_UPDATE)
		{
			MMP_ULONG i;
			MMPS_DSC_EXIF_CONFIG Exif;

			for (i = 0; ; i++) {
                if (MMP_TRUE == MMPS_DSC_GetImageEXIF(i, &Exif)) {
                    MMPS_DSC_UpdateExifBuffer(EXIF_NODE_ID_PRIMARY, Exif.IfdID, Exif.TagID, Exif.TagType, Exif.TagDataSize, (MMP_UBYTE*)Exif.Data, Exif.DataSize, MMP_FALSE);
                }
                else {
                    break;
                }
            }
        }
        #endif
		
		m_MultiShotInfo.ulExifHeaderStartAddr[m_MultiShotInfo.usShotIndex] = m_MultiShotInfo.ulUsedEndAddr[m_MultiShotInfo.usShotIndex];
        
        MMPD_DSC_FinishExif(EXIF_NODE_ID_PRIMARY,
        					m_MultiShotInfo.ulExifHeaderStartAddr[m_MultiShotInfo.usShotIndex], 
        					&(m_MultiShotInfo.ulExifHeaderSize[m_MultiShotInfo.usShotIndex]), 
        					ulThumbnailSize);

        *pCaptureSize += m_MultiShotInfo.ulExifHeaderSize[m_MultiShotInfo.usShotIndex];

		m_MultiShotInfo.ulUsedEndAddr[m_MultiShotInfo.usShotIndex] = m_MultiShotInfo.ulExifHeaderStartAddr[m_MultiShotInfo.usShotIndex] 
		                                                           + m_MultiShotInfo.ulExifHeaderSize[m_MultiShotInfo.usShotIndex];
    }

L_Restore:

    #if (USE_H264_CUR_BUF_AS_CAPT_BUF) 
    if (bRecording) {
        // NOP
    }
    else
    #endif
    {
    	/* Change the path to LDC in order to keep the setting of pipe normal */
        MMPD_Scaler_SetPath(pipeID, MMP_SCAL_SOURCE_LDC, MMP_TRUE);

        /* Restore the setting of Scaler */
        MMPD_Scaler_RestoreAttributes(pipeID);

        /* Restore the setting of Fctl */
        //MMPD_Fctl_SetPipeAttrForIbcFB(&m_VRPreviewFctlAttr[0]);

        //MMPD_Fctl_LinkPipeToGra2JPEG(pipeID, winID, previewDev, MMP_FALSE, MMP_TRUE);

    	/* Restore the setting of IBC */
        MMPD_IBC_SetAttributes(pipeID, &pipeAttr);

    	/* Change the path to the original source */
    	//MMPD_IBC_ClearFrameEnd(pipeID);        //this would cause preview failed when stillcapture     
        //MMPD_Scaler_SetPath(pipeID, ScalerSrcTmp, MMP_TRUE);
        //MMPD_IBC_CheckFrameEnd(pipeID);

        //MMPD_IBC_SetStoreEnable(pipeID, MMP_TRUE);
    }

    return err;
}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_CaptureAfterSrcReady
//  Description : For video snapshot
//------------------------------------------------------------------------------
/** @brief preview and capture at the same pipe.

@retval MMP_ERR_NONE success
@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_CaptureAfterSrcReady(	MMP_IBC_PIPEID 			pipeID,
								 		MMPS_DSC_CAPTURE_INFO 	*pCaptureInfo,
								 		MMP_SCAL_SOURCE			ScalerSrc,
								 		MMP_SCAL_FIT_RANGE 		*pCaptureFitrange,
								 		MMP_SCAL_GRAB_CTRL 		*pCaptureGrabctl,
								 		MMP_ULONG              	*pCaptureSize,
								 		MMP_BOOL                bDualCamCapture)
{
    MMP_ULONG				ulThumbnailSize = 0;
    MMP_SCAL_SOURCE         ScalerSrcTmp;
    MMP_IBC_PIPE_ATTR       pipeAttr;
    MMP_DSC_JPEG_INFO       jpegInfo;
    MMP_IBC_LINK_TYPE 		IBCLinkType;
    MMP_DISPLAY_DEV_TYPE	previewDev;
    MMP_DISPLAY_WIN_ID		winID;
    MMP_DISPLAY_ROTATE_TYPE rotateDir;
    MMPD_FCTL_ATTR  		fctlAttr;
    MMPD_FCTL_LINK			fctllink;
    MMP_ERR                 err = MMP_ERR_NONE;

	FCTL_PIPE_TO_LINK(pipeID, fctllink);

	/* Back up the Scaler and IBC setting and close the IBC of preview path */
    MMPD_Scaler_GetPath((MMP_SCAL_PIPEID) pipeID, &ScalerSrcTmp);
	MMPD_Scaler_BackupAttributes((MMP_SCAL_PIPEID) pipeID);
	// Also Backup Icon setting
	MMPD_Icon_BackupDLSetting(pipeID);
	
	MMPD_IBC_ClearFrameEnd(pipeID);
	MMPD_IBC_CheckFrameEnd(pipeID);
	MMPD_IBC_SetStoreEnable(pipeID, MMP_FALSE);
	MMPD_Fctl_GetIBCLinkAttr(pipeID, &IBCLinkType, &previewDev, &winID, &rotateDir);
	/*Preview rotate DMA src & dst index may not the same here after disable IBC store, so reset here*/
	if(rotateDir != MMP_DISPLAY_ROTATE_NO_ROTATE)
	    MMPD_Display_WaitRotateDMA(pipeID);
	MMPD_Fctl_ResetIBCLinkType(pipeID);
	
	pipeAttr.function = MMP_IBC_FX_TOFB;
	MMPD_IBC_GetAttributes(pipeID, &pipeAttr);

    /* Change the path to JPEG in order to keep the setting of pipe normal */
    #if (VR_STILL_USE_REF_FRM)
    if (ScalerSrc == MMP_SCAL_SOURCE_GRA)
        MMPD_Scaler_SetPath(pipeID, MMP_SCAL_SOURCE_GRA, MMP_TRUE);
    else
        MMPD_Scaler_SetPath(pipeID, MMP_SCAL_SOURCE_JPG, MMP_TRUE);
    #else
        if (bDualCamCapture)
            MMPD_Scaler_SetPath(pipeID, MMP_SCAL_SOURCE_LDC, MMP_TRUE);
        else
            MMPD_Scaler_SetPath(pipeID, MMP_SCAL_SOURCE_JPG, MMP_TRUE);
    #endif

    fctlAttr.fctllink       = fctllink;
    fctlAttr.fitrange       = *pCaptureFitrange;
    fctlAttr.grabctl        = *pCaptureGrabctl;
    fctlAttr.scalsrc		= ScalerSrc;
    fctlAttr.bSetScalerSrc	= MMP_FALSE;

    #if (VR_STILL_USE_REF_FRM)
    if (ScalerSrc == MMP_SCAL_SOURCE_GRA)
	    MMPD_Fctl_SetPipeAttrForJpeg(&fctlAttr, MMP_TRUE, MMP_TRUE);
    else
        MMPD_Fctl_SetPipeAttrForJpeg(&fctlAttr, MMP_TRUE, MMP_FALSE);
    #else
#if (REAR_CAM_TYPE == REAR_CAM_TYPE_TV_DECODER)    
    MMPD_Fctl_SetPipeAttrForJpeg(&fctlAttr, MMP_TRUE, MMP_TRUE);
#else
    MMPD_Fctl_SetPipeAttrForJpeg(&fctlAttr, MMP_TRUE, MMP_FALSE);
#endif
    #endif
	
	/* Control the flow of encoding JPEG */
	m_MultiShotInfo.usShotIndex		= 0;
	m_MultiShotInfo.usStoreIndex	= 0;
	m_MultiShotInfo.ulPrimaryJpegAddr[m_MultiShotInfo.usShotIndex] = m_ulPrimaryJpegCompStart;

    MMPD_DSC_SetPreviewPath(pipeID, pipeID, pipeID);
    MMPD_DSC_SetCapturePath(pipeID, pipeID, pipeID);

    #if (MCR_V2_UNDER_DBG)
    MMPD_IBC_ResetModule(pipeID);
    #endif

    /* Change the path to the frame source */
    #if (VR_STILL_USE_REF_FRM)
    if (ScalerSrc == MMP_SCAL_SOURCE_GRA) {
        err = MMPD_DSC_VidRefBuf2Jpeg();
    }
    else
    #endif
    {
        MMPD_IBC_ClearFrameEnd(pipeID);
        MMPD_Scaler_SetPath(pipeID, ScalerSrc, MMP_TRUE);
    	MMPD_IBC_CheckFrameEnd(pipeID);

        err = MMPD_DSC_TakeJpegAfterSrcReady(0/*m_ubVidRecModeSnrId*/);
    }

    if (err) {
        PRINTF("Still capture failed\r\n");
        goto L_Restore;
    }

    /* Check the size of JPEG is over the compressed buffer or not */
    if (bDualCamCapture)
        MMPD_DSC_GetJpegSizeAfterEncodeDone(&(pCaptureInfo->ulJpegFileSize));
    else
        MMPD_DSC_GetJpegSize(&(pCaptureInfo->ulJpegFileSize));
    
    *pCaptureSize = pCaptureInfo->ulJpegFileSize;
	
	m_MultiShotInfo.ulPrimaryJpegSize[m_MultiShotInfo.usShotIndex] = pCaptureInfo->ulJpegFileSize;
	
    m_MultiShotInfo.ulUsedEndAddr[m_MultiShotInfo.usShotIndex] = m_MultiShotInfo.ulPrimaryJpegAddr[m_MultiShotInfo.usShotIndex] 
    														   + m_MultiShotInfo.ulPrimaryJpegSize[m_MultiShotInfo.usShotIndex];
    
    if (m_MultiShotInfo.ulUsedEndAddr[m_MultiShotInfo.usShotIndex] > m_ulPrimaryJpegCompEnd) {
        
        PRINTF("The Compressed Data is over JPEG compressed buffer\r\n");        
        err = MMP_DSC_ERR_CAPTURE_BUFFER_OVERFLOW;
        goto L_Restore;
    }
	
	/* Encode Exif and Thumbnail */
	if (pCaptureInfo->bExif) {
		
		MMPD_DSC_SetCardModeExifEnc(EXIF_NODE_ID_PRIMARY,
									pCaptureInfo->bExif, 
			                        pCaptureInfo->bThumbnail,
			                        m_ulDSCThumbWidth, 
			                        m_ulDSCThumbHeight);
			                        	
        if (pCaptureInfo->bThumbnail && bDualCamCapture == MMP_FALSE) {
			
			jpegInfo.usPrimaryWidth = m_usJpegWidth;
    		jpegInfo.usPrimaryHeight = m_usJpegHeight;
            
            m_MultiShotInfo.ulExifThumbnailAddr[m_MultiShotInfo.usShotIndex] = ALIGN_X(m_MultiShotInfo.ulUsedEndAddr[m_MultiShotInfo.usShotIndex], DSC_BUF_ALIGN_BASE);
            
            MMPS_DSC_EncodeThumbnail(	pCaptureInfo,
            							&jpegInfo,
								 		m_ulDSCThumbWidth, 
								 		m_ulDSCThumbHeight, 
								 		&ulThumbnailSize,
										m_thumbnailInputMode,
										pipeID);
        	
        	/* m_MultiShotInfo.ulUsedEndAddr[] will be updated in MMPS_DSC_EncodeThumbnail() */
        	
        	MMPS_DSC_UpdateExifBuffer(EXIF_NODE_ID_PRIMARY, IFD_IDX_IFD1, IFD1_THUMBNAIL_LENGTH, EXIF_TYPE_LONG, 1, (MMP_UBYTE*)(&ulThumbnailSize), 4, MMP_FALSE);
            *pCaptureSize += ulThumbnailSize;
        }

        #if (EXIF_MANUAL_UPDATE)
		{
			MMP_ULONG i;
			MMPS_DSC_EXIF_CONFIG Exif;

			for (i = 0; ; i++) {
                if (MMP_TRUE == MMPS_DSC_GetImageEXIF(i, &Exif)) {
                    MMPS_DSC_UpdateExifBuffer(EXIF_NODE_ID_PRIMARY, Exif.IfdID, Exif.TagID, Exif.TagType, Exif.TagDataSize, (MMP_UBYTE*)Exif.Data, Exif.DataSize, MMP_FALSE);
                }
                else {
                    break;
                }
            }
        }
        #endif
		
		m_MultiShotInfo.ulExifHeaderStartAddr[m_MultiShotInfo.usShotIndex] = m_MultiShotInfo.ulUsedEndAddr[m_MultiShotInfo.usShotIndex];
        
        MMPD_DSC_FinishExif(EXIF_NODE_ID_PRIMARY,
        					m_MultiShotInfo.ulExifHeaderStartAddr[m_MultiShotInfo.usShotIndex], 
        					&(m_MultiShotInfo.ulExifHeaderSize[m_MultiShotInfo.usShotIndex]), 
        					ulThumbnailSize);

        *pCaptureSize += m_MultiShotInfo.ulExifHeaderSize[m_MultiShotInfo.usShotIndex];

		m_MultiShotInfo.ulUsedEndAddr[m_MultiShotInfo.usShotIndex] = m_MultiShotInfo.ulExifHeaderStartAddr[m_MultiShotInfo.usShotIndex] 
		                                                           + m_MultiShotInfo.ulExifHeaderSize[m_MultiShotInfo.usShotIndex];
    }

L_Restore:

	/* Change the path to JPEG in order to keep the setting of pipe normal */
    #if (VR_STILL_USE_REF_FRM)
    if (ScalerSrc != MMP_SCAL_SOURCE_GRA)
    #endif
    {
        if (bDualCamCapture)
            MMPD_Scaler_SetPath(pipeID, MMP_SCAL_SOURCE_LDC, MMP_TRUE);
        else
            MMPD_Scaler_SetPath(pipeID, MMP_SCAL_SOURCE_JPG, MMP_TRUE);
    }
	if(rotateDir != MMP_DISPLAY_ROTATE_NO_ROTATE)
        MMPD_Display_ResetRotateDMAIdx();
    MMPD_IBC_ClearFrameEnd(pipeID);
	MMPD_IBC_SetStoreEnable(pipeID, MMP_FALSE);    
	MMPD_IBC_CheckFrameEnd(pipeID);
	// Restore Icon Attribute.
    MMPD_Icon_RestoreDLSetting(pipeID);
    #if (MCR_V2_UNDER_DBG)
    MMPD_Icon_ResetModule(fctlAttr.fctllink.icopipeID);        
    MMPD_IBC_ResetModule(pipeID);
    #endif
    /* Restore the setting of Scaler */
    MMPD_Scaler_RestoreAttributes(pipeID);

    /* Restore the setting of Fctl */
    MMPD_Fctl_RestoreIBCLinkAttr(pipeID, IBCLinkType, previewDev, winID, rotateDir);

	/* Restore the setting of IBC */
    MMPD_IBC_SetAttributes(pipeID, &pipeAttr);

	/* Change the path to the original source */
	MMPD_IBC_ClearFrameEnd(pipeID);
    MMPD_Scaler_SetPath(pipeID, ScalerSrcTmp, MMP_TRUE);
    MMPD_IBC_CheckFrameEnd(pipeID);
	
    MMPD_IBC_SetStoreEnable(pipeID, MMP_TRUE);

    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_CaptureTakeJpeg
//  Description :
//------------------------------------------------------------------------------
/** @brief Capture picture in JPEG format

The function captures images into Jpeg picture buffer. The capture mode can be selected and the
size in bytes after capturing can be got.

If capture mode is multishot, call MMPS_DSC_RestorePreview to change preview back.

@param[in] 	pCaptureInfo set the buffer address of JPEG picture captured,
@param[out] pCaptureInfo set the buffer address of JPEG picture captured
file name, file name length, the JPEG file size, turn on thumbnail function or not.
@retval MMP_ERR_NONE success
@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_CaptureTakeJpeg(MMP_UBYTE ubSnrSel, MMPS_DSC_CAPTURE_INFO *pCaptureInfo)
{
    MMP_ERR			            mmpstatus;
	MMP_DSC_JPEG_INFO           jpegInfo;
	MMP_BOOL 					bComplete = MMP_FALSE;
	MMPD_FCTL_ATTR  			fctlAttr;
    MMP_BOOL					bRestorePreview = MMP_TRUE;
   	MMP_ULONG 					ThumbnailJpegSize = 0;
    MMP_DSC_THUMB_INPUT_SRC		ubThumbnailInputMode = m_thumbnailInputMode;
    MMP_SCAL_PIPEID 			ThumbPathSel = m_fctlLinkCapture.scalerpath;
  	
  	if (MMP_DSC_StartCaptureCallback != NULL) {
  		MMP_DSC_StartCaptureCallback(pCaptureInfo);
  	}

    if ((m_usCaptureMode == MMPS_DSC_CONTINUOUS_SHOTMODE) && (pCaptureInfo->bContiShotEnable == MMP_FALSE))
    {
        return MMPD_DSC_TakeContinuousJpeg(ubSnrSel,
                                           m_dscConfig.encParams.bDscPreviewKeepOn,
										   pCaptureInfo->usReviewTime,
										   pCaptureInfo->usWindowsTime,
										   m_usCaptureMode,
										   MMP_FALSE);
    }

  	if (!m_bDscPreviewActive) {
  		PRINTF("Preview is closed\r\n");
  		return MMP_DSC_ERR_CAPTURE_FAIL;
  	}

  	/* Make sure that scaler finishes zoom operation */
    MMPD_PTZ_SetDigitalZoom(m_fctlLinkPreview.scalerpath, MMP_PTZ_ZOOMSTOP, MMP_TRUE);

	while(!bComplete) {
        MMPD_PTZ_CheckZoomComplete(m_fctlLinkPreview.scalerpath, &bComplete);
    }

	/* JPEG buffer assignment */
	if (pCaptureInfo->bFirstShot == MMP_TRUE) {
		m_MultiShotInfo.usShotIndex    = 0;
		m_MultiShotInfo.usStoreIndex   = 0;
		m_MultiShotInfo.ulPrimaryJpegAddr[m_MultiShotInfo.usShotIndex] = m_ulPrimaryJpegCompStart;
	}
	else {
		m_MultiShotInfo.usShotIndex++;
		m_MultiShotInfo.ulPrimaryJpegAddr[m_MultiShotInfo.usShotIndex] = ALIGN_X(m_MultiShotInfo.ulUsedEndAddr[m_MultiShotInfo.usShotIndex - 1], DSC_BUF_ALIGN_BASE);

		MMPD_DSC_ChangeExtCompAddr(m_MultiShotInfo.ulPrimaryJpegAddr[m_MultiShotInfo.usShotIndex], m_ulPrimaryJpegCompEnd);
	}

	m_MultiShotInfo.ulExifThumbnailAddr[m_MultiShotInfo.usShotIndex] = 0;
	m_MultiShotInfo.ulExifThumbnailSize[m_MultiShotInfo.usShotIndex] = 0;
	#if (DSC_SUPPORT_BASELINE_MP_FILE)
	if (m_bMultiPicFormatEnable && m_bEncLargeThumbEnable)
	{
		m_MultiShotInfo.ulLargeThumbJpegAddr[m_MultiShotInfo.usShotIndex] = 0;
		m_MultiShotInfo.ulLargeThumbJpegSize[m_MultiShotInfo.usShotIndex] = 0;
	}
	#endif

    /* Initial the Exif structure */
	if (pCaptureInfo->bExif) {

		MMPD_DSC_SetCardModeExifEnc(EXIF_NODE_ID_PRIMARY,
									pCaptureInfo->bExif, 
		                            pCaptureInfo->bThumbnail,
		                            m_ulDSCThumbWidth, 
		                            m_ulDSCThumbHeight);
		                            
		#if 0//(DSC_SUPPORT_BASELINE_MP_FILE)
		if (m_bMultiPicFormatEnable && m_bEncLargeThumbEnable)
		{
			MMPD_DSC_SetCardModeExifEnc(EXIF_NODE_ID_LARGE_THUMB1,
										MMP_TRUE, 
			                            MMP_FALSE,
			                            0, 
			                            0);
		}
		#endif
	}
	
    fctlAttr.fctllink       = m_fctlLinkCapture;
    fctlAttr.scalsrc		= MMP_SCAL_SOURCE_ISP;
    fctlAttr.bSetScalerSrc	= MMP_FALSE;

	MMPD_Fctl_SetPipeAttrForJpeg(&fctlAttr, MMP_FALSE, MMP_FALSE);
	
	/* Set preveiew/capture pipes */
    MMPD_DSC_SetPreviewPath(m_fctlLinkPreview.scalerpath,
                            m_fctlLinkPreview.icopipeID,
                            m_fctlLinkPreview.ibcpipeID);
                                	
    MMPD_DSC_SetCapturePath(m_fctlLinkCapture.scalerpath,
                            m_fctlLinkCapture.icopipeID,
                            m_fctlLinkCapture.ibcpipeID);

    /* Set JPEG sticker	*/
	if (m_bEnablePrimaryJpegSticker) {
		MMPD_Icon_SetEnable(m_ubPrimaryJpegStickerId, MMP_TRUE);
	}

	/* Take Primary Jpeg */	
    if (m_usCaptureMode == MMPS_DSC_CONTINUOUS_SHOTMODE) 
    {
        mmpstatus = MMPD_DSC_TakeContinuousJpeg(ubSnrSel,
                                                m_dscConfig.encParams.bDscPreviewKeepOn,
										        pCaptureInfo->usReviewTime,
										        pCaptureInfo->usWindowsTime,
										        m_usCaptureMode,
										        MMP_TRUE);

        if (mmpstatus != MMP_ERR_NONE) {
            RTNA_DBG_Str(0, "MMPD_DSC_TakeContinuousJpeg Fail\r\n");
            return MMP_DSC_ERR_CAPTURE_FAIL;
        }

        return mmpstatus; 
    }
    else if (m_usCaptureMode == MMPS_DSC_SINGLE_SHOTMODE) 
    {
        if (m_dscConfig.encParams.bStableShotEn)
        {
            MMPD_DSC_SetStableShotParam(m_MultiShotInfo.ulPrimaryJpegAddr[m_MultiShotInfo.usShotIndex], 
            							m_ulSubJpegExtCompStart, 
            							m_dscConfig.encParams.usStableShotNum);
        }

    	if (m_thumbnailInputMode == MMP_DSC_THUMB_SRC_THUMB_PIPE) {
    		bRestorePreview = MMP_FALSE;
    	}
				
		mmpstatus = MMPD_DSC_TakeJpegPicture(ubSnrSel,
		                                     m_dscConfig.encParams.bDscPreviewKeepOn,
				        					 pCaptureInfo->usReviewTime,
				        					 pCaptureInfo->usWindowsTime,
				        					 m_usCaptureMode,
				        					 bRestorePreview);
    	
        if (mmpstatus != MMP_ERR_NONE) {
			RTNA_DBG_Str(0, "SINGLE_SHOTMODE CaptureTakeJpeg Fail\r\n");
			return MMP_DSC_ERR_CAPTURE_FAIL;
    	}
	}
    else if (m_usCaptureMode == MMPS_DSC_MULTI_SHOTMODE) 
    {
		if (pCaptureInfo->bFirstShot == MMP_TRUE) {

			bRestorePreview = MMP_FALSE;

    		mmpstatus = MMPD_DSC_TakeJpegPicture(ubSnrSel,
    		                                     m_dscConfig.encParams.bDscPreviewKeepOn,
					        					 pCaptureInfo->usReviewTime,
					        					 pCaptureInfo->usWindowsTime,
					        					 m_usCaptureMode,
					        					 bRestorePreview);
    	}
    	else {
    		mmpstatus = MMPD_DSC_TakeNextJpegPicture(ubSnrSel, pCaptureInfo->usReviewTime);
    	}
    	
        if (mmpstatus != MMP_ERR_NONE) {
			RTNA_DBG_Str(0, "MULTI_SHOTMODE CaptureTakeJpeg Fail\r\n");
			return MMP_DSC_ERR_CAPTURE_FAIL;
    	}
	}
	
	if (m_bEnablePrimaryJpegSticker) {
		MMPD_Icon_SetEnable(m_ubPrimaryJpegStickerId, MMP_FALSE);
	}

    /* Check the JPEG size is over the compress buffer or not */
    MMPD_DSC_GetJpegSize(&(pCaptureInfo->ulJpegFileSize));
    
    pCaptureInfo->ulJpegFileAddr = m_MultiShotInfo.ulPrimaryJpegAddr[m_MultiShotInfo.usShotIndex];

	m_MultiShotInfo.ulPrimaryJpegSize[m_MultiShotInfo.usShotIndex] = pCaptureInfo->ulJpegFileSize;

    m_MultiShotInfo.ulUsedEndAddr[m_MultiShotInfo.usShotIndex] = m_MultiShotInfo.ulPrimaryJpegAddr[m_MultiShotInfo.usShotIndex] 
    														   + m_MultiShotInfo.ulPrimaryJpegSize[m_MultiShotInfo.usShotIndex];
    
    if (m_MultiShotInfo.ulUsedEndAddr[m_MultiShotInfo.usShotIndex] > m_ulPrimaryJpegCompEnd) {
    	PRINTF("The Compressed Data is over JPEG compressed buffer \r\n");
    	return MMP_DSC_ERR_CAPTURE_BUFFER_OVERFLOW;
    }

	/* Encode Exif and thumbnail */
    if (pCaptureInfo->bExif) {

        if (pCaptureInfo->bThumbnail) {

   	        m_MultiShotInfo.ulExifThumbnailAddr[m_MultiShotInfo.usShotIndex] = ALIGN_X(m_MultiShotInfo.ulUsedEndAddr[m_MultiShotInfo.usShotIndex], DSC_BUF_ALIGN_BASE);
   	        
            MMPD_DSC_ChangeExtCompAddr(	m_MultiShotInfo.ulExifThumbnailAddr[m_MultiShotInfo.usShotIndex], 
            							m_ulPrimaryJpegCompEnd);

			jpegInfo.usPrimaryWidth  = m_usJpegWidth;
    		jpegInfo.usPrimaryHeight = m_usJpegHeight;

			if (m_thumbnailInputMode == MMP_DSC_THUMB_SRC_THUMB_PIPE) {
				ThumbPathSel = m_fctlLinkThumb.scalerpath;
			}

            MMPS_DSC_EncodeThumbnail(	pCaptureInfo,
            							&jpegInfo,
								 		m_ulDSCThumbWidth, 
								 		m_ulDSCThumbHeight, 
								 		&ThumbnailJpegSize,
										ubThumbnailInputMode,
										ThumbPathSel);

        	MMPS_DSC_UpdateExifBuffer(EXIF_NODE_ID_PRIMARY, IFD_IDX_IFD1, IFD1_THUMBNAIL_LENGTH, EXIF_TYPE_LONG, 1, (MMP_UBYTE *)(&ThumbnailJpegSize), 4, MMP_FALSE);
        	
        	// Restore
            MMPD_DSC_SetJpegResol(m_usJpegWidth, m_usJpegHeight, MMP_DSC_JPEG_RC_ID_CAPTURE);
        }

		m_MultiShotInfo.ulExifHeaderStartAddr[m_MultiShotInfo.usShotIndex] = m_MultiShotInfo.ulUsedEndAddr[m_MultiShotInfo.usShotIndex];
        
        MMPD_DSC_FinishExif(EXIF_NODE_ID_PRIMARY,
        					m_MultiShotInfo.ulExifHeaderStartAddr[m_MultiShotInfo.usShotIndex], 
        					&(m_MultiShotInfo.ulExifHeaderSize[m_MultiShotInfo.usShotIndex]), 
        					ThumbnailJpegSize);

		m_MultiShotInfo.ulUsedEndAddr[m_MultiShotInfo.usShotIndex] = m_MultiShotInfo.ulExifHeaderStartAddr[m_MultiShotInfo.usShotIndex] 
		                                                           + m_MultiShotInfo.ulExifHeaderSize[m_MultiShotInfo.usShotIndex];

        #if (DSC_SUPPORT_BASELINE_MP_FILE)
        if (m_bMultiPicFormatEnable) {
       		m_MultiShotInfo.ulMpfDataStartAddr[m_MultiShotInfo.usShotIndex] = m_MultiShotInfo.ulUsedEndAddr[m_MultiShotInfo.usShotIndex];
    	}
    	#endif
	}

	#if (DSC_SUPPORT_BASELINE_MP_FILE)
	/* Encode large thumbnail and its Exif */
    if (m_bMultiPicFormatEnable && m_bEncLargeThumbEnable) 
    {
        m_MultiShotInfo.ulLargeThumbJpegAddr[m_MultiShotInfo.usShotIndex] = m_ulLargeThumbJpegCompStart;

        MMPD_DSC_ChangeExtCompAddr(m_MultiShotInfo.ulLargeThumbJpegAddr[m_MultiShotInfo.usShotIndex], 
        						   m_ulLargeThumbJpegCompEnd);

		jpegInfo.usPrimaryWidth  = m_usJpegWidth;
		jpegInfo.usPrimaryHeight = m_usJpegHeight;

		if (m_thumbnailInputMode == MMP_DSC_THUMB_SRC_THUMB_PIPE) {
			ThumbPathSel = m_fctlLinkThumb.scalerpath;
		}

		// Update m_MultiShotInfo.ulLargeThumbUsedEndAddr[] in MMPS_DSC_EncodeLargeThumb()
        MMPS_DSC_EncodeLargeThumb(	pCaptureInfo,
        							&jpegInfo,
							 		m_ulLargeThumbWidth, 
							 		m_ulLargeThumbHeight, 
							 		&ThumbnailJpegSize,
									ubThumbnailInputMode,
									ThumbPathSel);

		// Update MPF Entry and LargeThumb Exif
		MMPD_DSC_UpdateMpfEntry(IFD_IDX_MPF_INDEX_IFD, 
								MPF_ENTRY_ID_PRIMARY_IMG, 
								MPF_ATTR_FLAG_DEP_PARENT_IMG | MPF_ATTR_FLAG_DATA_FMT_JPEG | MPF_ATTR_FLAG_TYPE_BASELINE_PRI_IMG,
								m_MultiShotInfo.ulPrimaryJpegSize[m_MultiShotInfo.usStoreIndex],
								0, 
								2, 0,
								MMP_FALSE);

		if (m_ubLargeThumbResMode == MMP_DSC_LARGE_THUMB_RES_FHD) {
			MMPD_DSC_UpdateMpfEntry(IFD_IDX_MPF_INDEX_IFD, 
									MPF_ENTRY_ID_LARGE_THUMB1, 
									MPF_ATTR_FLAG_DEP_CHILD_IMG | MPF_ATTR_FLAG_DATA_FMT_JPEG | MPF_ATTR_FLAG_TYPE_LARGE_THUMB_FHD,
									m_MultiShotInfo.ulLargeThumbJpegSize[m_MultiShotInfo.usStoreIndex] + 0x4A /* Large Thumb Exif Size */,
									(m_MultiShotInfo.ulPrimaryJpegSize[m_MultiShotInfo.usStoreIndex] +
									 /*m_MultiShotInfo.ulMpfDataSize[m_MultiShotInfo.usShotIndex] - 6*/ 0x52),
									0, 0,
									MMP_FALSE);
		}
		else {
			MMPD_DSC_UpdateMpfEntry(IFD_IDX_MPF_INDEX_IFD, 
									MPF_ENTRY_ID_LARGE_THUMB1, 
									MPF_ATTR_FLAG_DEP_CHILD_IMG | MPF_ATTR_FLAG_DATA_FMT_JPEG | MPF_ATTR_FLAG_TYPE_LARGE_THUMB_VGA,
									m_MultiShotInfo.ulLargeThumbJpegSize[m_MultiShotInfo.usStoreIndex] + 0x4A /* Large Thumb Exif Size */,
									(m_MultiShotInfo.ulPrimaryJpegSize[m_MultiShotInfo.usStoreIndex] +
									 /*m_MultiShotInfo.ulMpfDataSize[m_MultiShotInfo.usShotIndex] - 6*/ 0x52),
									0, 0,
									MMP_FALSE);
		}
		
		MMPD_DSC_FinishMpf(m_MultiShotInfo.ulMpfDataStartAddr[m_MultiShotInfo.usShotIndex], 
        				   &(m_MultiShotInfo.ulMpfDataSize[m_MultiShotInfo.usShotIndex])); 

    	// Restore
        MMPD_DSC_SetJpegResol(m_usJpegWidth, m_usJpegHeight, MMP_DSC_JPEG_RC_ID_CAPTURE);

		m_MultiShotInfo.ulLargeThumbExifAddr[m_MultiShotInfo.usShotIndex] = m_MultiShotInfo.ulLargeThumbUsedEndAddr[m_MultiShotInfo.usShotIndex];
        
  		MMPS_DSC_UpdateExifBuffer(EXIF_NODE_ID_LARGE_THUMB1, IFD_IDX_EXIFIFD, EXIFIFD_PIXL_X_DIMENSION, EXIF_TYPE_SHORT, 1, (MMP_UBYTE *)(&m_ulLargeThumbWidth), 2, MMP_FALSE);
    	MMPS_DSC_UpdateExifBuffer(EXIF_NODE_ID_LARGE_THUMB1, IFD_IDX_EXIFIFD, EXIFIFD_PIXL_Y_DIMENSION, EXIF_TYPE_SHORT, 1, (MMP_UBYTE *)(&m_ulLargeThumbHeight), 2, MMP_FALSE);
        
        MMPD_DSC_FinishExif(EXIF_NODE_ID_LARGE_THUMB1,
        					m_MultiShotInfo.ulLargeThumbExifAddr[m_MultiShotInfo.usShotIndex], 
        					&(m_MultiShotInfo.ulLargeThumbExifSize[m_MultiShotInfo.usShotIndex]), 
        					0);

		m_MultiShotInfo.ulLargeThumbUsedEndAddr[m_MultiShotInfo.usShotIndex] = 
						m_MultiShotInfo.ulLargeThumbExifAddr[m_MultiShotInfo.usShotIndex] +
		                m_MultiShotInfo.ulLargeThumbExifSize[m_MultiShotInfo.usShotIndex];             
	}
	#endif

	if (ubThumbnailInputMode == MMP_DSC_THUMB_SRC_THUMB_PIPE) {
		MMPS_DSC_RestorePreview(ubSnrSel, pCaptureInfo->usWindowsTime);
	}

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_DMADone
//  Description :
//------------------------------------------------------------------------------
static void MMPS_DSC_DMADone(void)
{
	m_bDMADone = MMP_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_EncodeThumbnail
//  Description :
//------------------------------------------------------------------------------
static MMP_ERR MMPS_DSC_EncodeThumbnail(MMPS_DSC_CAPTURE_INFO   *pCaptureInfo,
										MMP_DSC_JPEG_INFO       *pJpegInfo,
										MMP_ULONG               ulThumbWidth, 
										MMP_ULONG               ulThumbHeight, 
										MMP_ULONG               *pThumbnailSize,
										MMP_DSC_THUMB_INPUT_SRC	ubThumbnailInputMode,
										MMP_SCAL_PIPEID        	pipeID)
{
   	MMP_ERR 					mmpstatus;
	MMP_SCAL_FIT_RANGE       	fitrange;
	MMP_SCAL_GRAB_CTRL 			grabctl;
	MMP_GRAPHICS_RECT          	rect;
	MMP_DSC_DECODE_BUF 			decodebuf;
	MMP_DSC_CAPTURE_BUF			capturebuf;
	MMP_GRAPHICS_BUF_ATTR 	 	bufAttr;
	MMP_ULONG 					ulThumbDecodeOutStart;
	MMPD_FCTL_ATTR				fctlAttr;
	MMPD_FCTL_LINK				fctllink;

	if (ubThumbnailInputMode != MMP_DSC_THUMB_SRC_DECODED_JPEG &&
		ubThumbnailInputMode != MMP_DSC_THUMB_SRC_THUMB_PIPE) {
		// Fix DSC capture -> Video capture. Jpeg file abnormal issue.
		m_MultiShotInfo.ulExifThumbnailSize[m_MultiShotInfo.usShotIndex] = 0;
	    return MMP_ERR_NONE;
	}

	FCTL_PIPE_TO_LINK(pipeID, fctllink);

	/* Step 1: Set Qtable for thumbnail */
	MMPD_DSC_SetJpegQTable((MMP_UBYTE *)m_dscConfig.encParams.ubDscQtable[MMP_DSC_CAPTURE_LOW_QUALITY],
                           (MMP_UBYTE *)m_dscConfig.encParams.ubDscQtable[MMP_DSC_CAPTURE_LOW_QUALITY] + DSC_QTABLE_ARRAY_SIZE,
	                       (MMP_UBYTE *)m_dscConfig.encParams.ubDscQtable[MMP_DSC_CAPTURE_LOW_QUALITY] + DSC_QTABLE_ARRAY_SIZE);

	MMPD_DSC_SetJpegQualityControl( MMP_DSC_JPEG_RC_ID_CAPTURE,
	                                MMP_FALSE,
		                            MMP_TRUE,
		                            m_dscConfig.encParams.ulMaxThumbnailSize>>10,
		                            m_dscConfig.encParams.ulMaxThumbnailSize>>10,
	    	                        3);

	if (ubThumbnailInputMode == MMP_DSC_THUMB_SRC_DECODED_JPEG ||
		ubThumbnailInputMode == MMP_DSC_THUMB_SRC_THUMB_PIPE)
	{
		#if 0// TBD for LDC video still capture
	    MMP_ULONG 	ulStoreWidth = ulThumbWidth;
		MMP_ULONG 	ulStoreHeight = ulThumbHeight;
		MMP_USHORT 	usHrange = 0;
		MMP_USHORT 	usVrange = 0;
		
		/* Step 2: prepare raw image */
        ulStoreWidth  = ulThumbWidth;
   		ulStoreHeight = m_usJpegHeight * ulThumbWidth / m_usJpegWidth;

        ulStoreWidth  = ALIGN16(ulStoreWidth);
        ulStoreHeight = ALIGN8(ulStoreHeight);

		MMPD_Scaler_GetGrabRange(m_fctlLinkCapture.scalerpath, MMP_SCAL_GRAB_STAGE_OUT, &usHrange, &usVrange);

  		fitrange.fitmode     	= MMP_SCAL_FITMODE_OUT;
  		fitrange.scalerType 	= MMP_SCAL_TYPE_SCALER;
        fitrange.ulInWidth   	= usHrange;
        fitrange.ulInHeight  	= usVrange;
        fitrange.ulOutWidth  	= ulStoreWidth;
        fitrange.ulOutHeight 	= ulStoreHeight;

		fitrange.ulInGrabX  	= 1;
	   	fitrange.ulInGrabY  	= 1;
	   	fitrange.ulInGrabW  	= fitrange.ulInWidth;
		fitrange.ulInGrabH  	= fitrange.ulInHeight;

        MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);

		fctlAttr.fctllink       = m_fctlLinkThumb.scalerpath;
	    fctlAttr.fitrange       = fitrange;
	    fctlAttr.grabctl        = grabctl;
	    fctlAttr.scalsrc		= MMP_SCAL_SOURCE_ISP;	// Not used here.
	    fctlAttr.bSetScalerSrc	= MMP_FALSE;

		/* Step 3: Encode thumbnail */
	    capturebuf.ulCompressStart	= m_MultiShotInfo.ulExifThumbnailAddr[m_MultiShotInfo.usShotIndex];
    	capturebuf.ulCompressEnd	= m_ulPrimaryJpegCompEnd;
    	capturebuf.ulLineStart		= m_ulPrimaryJpegLineStart;
	    MMPD_DSC_SetCaptureBuffers(&capturebuf);

        MMPD_DSC_EncodeThumb(m_ubDscModeSnrId, m_fctlLinkCapture.scalerpath, m_fctlLinkThumb.scalerpath, &fctlAttr);

	    MMPD_DSC_GetJpegSize(pThumbnailSize);

		m_MultiShotInfo.ulExifThumbnailSize[m_MultiShotInfo.usShotIndex] = *pThumbnailSize;

		m_MultiShotInfo.ulUsedEndAddr[m_MultiShotInfo.usShotIndex] = m_MultiShotInfo.ulExifThumbnailAddr[m_MultiShotInfo.usShotIndex] 
		                                                           + m_MultiShotInfo.ulExifThumbnailSize[m_MultiShotInfo.usShotIndex];		
		#else

	    MMP_ULONG 	ulStoreWidth = ulThumbWidth;
		MMP_ULONG 	ulStoreHeight = ulThumbHeight;
		MMP_USHORT 	usHrange = 0;
		MMP_USHORT 	usVrange = 0;
		
		/* Step 2: prepare raw image */
        if ( (ulThumbWidth * m_usJpegHeight) > (m_usJpegWidth * ulThumbHeight) ) {
    		// Black part at left and right side
    		ulStoreWidth 	= m_usJpegWidth * ulThumbHeight / m_usJpegHeight;
            ulStoreHeight	= ulThumbHeight;
        }
        else if ( (ulThumbWidth * m_usJpegHeight) <= (m_usJpegWidth * ulThumbHeight) ) {
			#if 0 //Mark for fill up the thumb without black part
        	// Black part at top and bottum side
            ulStoreWidth	= ulThumbWidth;
    		ulStoreHeight   = m_usJpegHeight * ulThumbWidth / m_usJpegWidth;
			#endif
        }
 
        ulStoreWidth  = ALIGN16(ulStoreWidth);
        ulStoreHeight = ALIGN8(ulStoreHeight);
        
		// Set decode out buffer for primary jpeg decode to thumbnail (YUV422).
	    ulThumbDecodeOutStart = m_ulPrimaryJpeg4ExifThumbDecodeOutStart;
	    
	    if (ulStoreWidth == ulThumbWidth) {
			bufAttr.ulBaseAddr = m_ulPrimaryJpeg4ExifThumbDecodeOutStart + ((ulThumbHeight - ulStoreHeight) / 2) * ulThumbWidth * 2;
		}
		else {
		    bufAttr.ulBaseAddr = m_ulPrimaryJpeg4ExifThumbDecodeOutStart;
		}
		
		if (ubThumbnailInputMode == MMP_DSC_THUMB_SRC_DECODED_JPEG)
		{
			decodebuf.ulLineBufStart    = bufAttr.ulBaseAddr + ALIGN_X(ulThumbWidth * 2, DSC_BUF_ALIGN_BASE) * ulThumbHeight;
			decodebuf.ulLineBufSize     = m_dscConfig.decParams.ulDscDecMaxLineBufSize;
			decodebuf.ulStorageTmpStart = decodebuf.ulLineBufStart + decodebuf.ulLineBufSize;
			decodebuf.ulStorageTmpSize  = m_dscConfig.decParams.ulDscDecMaxInbufSize;

	    	MMPD_DSC_SetDecodeBuffers(&decodebuf);

	    	// The setting for MMPD_DSC_DecodeJpegPic()
            pJpegInfo->bJpegFileName[0]     = NULL;
            pJpegInfo->ulJpegBufAddr        = m_MultiShotInfo.ulPrimaryJpegAddr[m_MultiShotInfo.usShotIndex]; 
            pJpegInfo->ulJpegBufSize        = pCaptureInfo->ulJpegFileSize;
            pJpegInfo->bDecodeThumbnail     = MMP_FALSE;
	        #if (DSC_SUPPORT_BASELINE_MP_FILE)
	        pJpegInfo->bDecodeLargeThumb 	= MMP_FALSE;
	        #endif
            pJpegInfo->bValid               = MMP_TRUE;

	        // Get Jpeg information from SOF marker
		    if (MMPD_DSC_GetJpegInfo(pJpegInfo) != MMP_ERR_NONE) {
		        return MMP_DSC_ERR_JPEGINFO_FAIL;
		    }

	  		fitrange.fitmode    = MMP_SCAL_FITMODE_OPTIMAL;
	  		fitrange.scalerType	= MMP_SCAL_TYPE_SCALER;
	        fitrange.ulInWidth  = pJpegInfo->usPrimaryWidth;
	        fitrange.ulInHeight	= pJpegInfo->usPrimaryHeight;
	        
            if ( (m_CaptureRotateType == MMP_DSC_CAPTURE_NO_ROTATE) ||
                 (m_CaptureRotateType == MMP_DSC_CAPTURE_ROTATE_RIGHT_180) ) 
	        {
	            fitrange.ulOutWidth  = ulStoreWidth;
	            fitrange.ulOutHeight = ulStoreHeight;
	        }
	        else {
	            fitrange.ulOutWidth  = ulStoreHeight;
	            fitrange.ulOutHeight = ulStoreWidth;
	        }

			fitrange.ulInGrabX	= 1;
			fitrange.ulInGrabY	= 1;
			fitrange.ulInGrabW	= fitrange.ulInWidth;
			fitrange.ulInGrabH	= fitrange.ulInHeight;
			
	        MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);

	        // Decode JPEG file into buffer
        	MMPD_DSC_DecodeJpegPic(	MMP_DISPLAY_COLOR_YUV422, 
                                    pJpegInfo, 
	        						&bufAttr,
	        						&fitrange,
	        						&grabctl, 
        							pipeID);
		}
		else if (ubThumbnailInputMode == MMP_DSC_THUMB_SRC_THUMB_PIPE)
		{
			MMPD_Scaler_GetGrabRange(m_fctlLinkCapture.scalerpath, MMP_SCAL_GRAB_STAGE_OUT, &usHrange, &usVrange);

	  		fitrange.fitmode    = MMP_SCAL_FITMODE_OUT;
	  		fitrange.scalerType	= MMP_SCAL_TYPE_SCALER;
	        fitrange.ulInWidth  = usHrange;
            fitrange.ulInHeight = usVrange;

            if ( (m_CaptureRotateType == MMP_DSC_CAPTURE_NO_ROTATE) ||
                 (m_CaptureRotateType == MMP_DSC_CAPTURE_ROTATE_RIGHT_180) ) 
	        {
	            fitrange.ulOutWidth  = ulStoreWidth;
	            fitrange.ulOutHeight = ulStoreHeight;
	        }
	        else {
	            fitrange.ulOutWidth  = ulStoreHeight;
	            fitrange.ulOutHeight = ulStoreWidth;
	        }

			fitrange.ulInGrabX	= 1;
			fitrange.ulInGrabY	= 1;
			fitrange.ulInGrabW	= fitrange.ulInWidth;
			fitrange.ulInGrabH	= fitrange.ulInHeight;

	        MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);

			MMPD_DSC_StoreThumbSrcFrame(&bufAttr,
										&fitrange,
	        							&grabctl,
	        							m_fctlLinkCapture.scalerpath, 
	        							pipeID);
		}

        // Move decoded data for black-thumbnail at left right side case
        if (ulStoreWidth != ulThumbWidth) 
        {
            MMP_GRAPHICS_BUF_ATTR   srcBufAttr;
			MMP_GRAPHICS_BUF_ATTR 	dstBufAttr;
			
			rect.usLeft             = 0;
	    	rect.usTop              = 0;
			rect.usWidth            = ulStoreWidth;
			rect.usHeight           = ulStoreHeight;
			
			srcBufAttr.ulBaseAddr 	= bufAttr.ulBaseAddr;
	    	srcBufAttr.usWidth		= ulStoreWidth;
			srcBufAttr.usHeight	    = ulStoreHeight;
			srcBufAttr.usLineOffset = srcBufAttr.usWidth * 2;
			srcBufAttr.colordepth   = MMP_GRAPHICS_COLORDEPTH_16;
	                            
			dstBufAttr.ulBaseAddr	= m_ulBlackThumbnailDecodeOutStart;
			dstBufAttr.usWidth		= ulThumbWidth;
			dstBufAttr.usHeight	    = ulThumbHeight;
			dstBufAttr.usLineOffset = dstBufAttr.usWidth * 2;
	 		dstBufAttr.colordepth	= MMP_GRAPHICS_COLORDEPTH_16;
	 				
			m_bDMADone = MMP_FALSE;
	    	
	    	mmpstatus = MMPD_DMA_RotateImageBuftoBuf(&srcBufAttr, 
	                        						&rect, 
	                       	 						&dstBufAttr, 
	                        						(dstBufAttr.usWidth  - srcBufAttr.usWidth) / 2, 
	                        						(dstBufAttr.usHeight - srcBufAttr.usHeight) / 2, 
	                        						MMP_GRAPHICS_ROTATE_NO_ROTATE, 
	                        						(DmaCallBackFunc *)MMPS_DSC_DMADone, 
	                        						MMP_FALSE, 
	                        						DMA_NO_MIRROR);

	   		if (mmpstatus != MMP_ERR_NONE) {
	        	PRINTF("MMPD_DMA_RotateImageBuftoBuf Failed\r\n");
	        }
	        
	        // Waiting the DMA move buf to buf done.
	    	while(!m_bDMADone);

			ulThumbDecodeOutStart = m_ulBlackThumbnailDecodeOutStart;
        }

		/* Step 3: Encode thumbnail */
	    capturebuf.ulCompressStart	= m_MultiShotInfo.ulExifThumbnailAddr[m_MultiShotInfo.usShotIndex];
    	capturebuf.ulCompressEnd	= m_ulPrimaryJpegCompEnd;
    	capturebuf.ulLineStart		= m_ulPrimaryJpegLineStart;
	    MMPD_DSC_SetCaptureBuffers(&capturebuf);

		// Set the graphics source buffer attribute	
	    bufAttr.ulBaseAddr = ulThumbDecodeOutStart;
        bufAttr.colordepth = MMP_GRAPHICS_COLORDEPTH_YUV422_UYVY;
	    
		// Thumbnail(RAW) encode to thumbnail(Jpeg)
	    if ( (m_CaptureRotateType == MMP_DSC_CAPTURE_NO_ROTATE) ||
	         (m_CaptureRotateType == MMP_DSC_CAPTURE_ROTATE_RIGHT_180) ) 
	    {                                     
            bufAttr.usWidth         = ulThumbWidth;
            bufAttr.usHeight        = ulThumbHeight;
            bufAttr.usLineOffset    = bufAttr.usWidth * 2;
			
	        rect.usLeft        		= 0;
	        rect.usTop           	= 0;
	        rect.usWidth        	= ulThumbWidth;
	        rect.usHeight        	= ulThumbHeight;
	    }
	    else {

	        bufAttr.usWidth        	= ulThumbHeight;
	        bufAttr.usHeight       	= ulThumbWidth;
            bufAttr.usLineOffset    = bufAttr.usWidth * 2;

	        rect.usLeft          	= 0;
	        rect.usTop           	= 0;
	        rect.usWidth        	= ulThumbHeight;
	        rect.usHeight        	= ulThumbWidth;
	    }	    
        
	    fitrange.fitmode     	= MMP_SCAL_FITMODE_OUT;
	    fitrange.scalerType		= MMP_SCAL_TYPE_SCALER;
    	fitrange.ulInWidth   	= rect.usWidth;
    	fitrange.ulInHeight  	= rect.usHeight;
    	fitrange.ulOutWidth  	= rect.usWidth;
    	fitrange.ulOutHeight 	= rect.usHeight;

	    fitrange.ulInGrabX		= 1;
	    fitrange.ulInGrabY		= 1;
	    fitrange.ulInGrabW		= fitrange.ulInWidth;
	    fitrange.ulInGrabH		= fitrange.ulInHeight;

    	MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);

	    fctlAttr.fctllink       = fctllink;
	    fctlAttr.fitrange       = fitrange;
	    fctlAttr.grabctl        = grabctl;
	    fctlAttr.scalsrc		= MMP_SCAL_SOURCE_GRA;
	    fctlAttr.bSetScalerSrc	= MMP_TRUE;

        // Encode JPEG from graphic path.
	    MMPD_DSC_RawBuf2Jpeg(&bufAttr, &rect, 1, &fctlAttr);

	    MMPD_DSC_GetJpegSize(pThumbnailSize);

	    if (*pThumbnailSize > m_dscConfig.encParams.ulMaxThumbnailSize - 32) {
			
	        MMPD_DSC_SetJpegQTable((MMP_UBYTE *)m_dscConfig.encParams.ubDscQtable[DSC_QTABLE_MAX_NUM-1],
	                               (MMP_UBYTE *)m_dscConfig.encParams.ubDscQtable[DSC_QTABLE_MAX_NUM-1] + DSC_QTABLE_ARRAY_SIZE,
	                               (MMP_UBYTE *)m_dscConfig.encParams.ubDscQtable[DSC_QTABLE_MAX_NUM-1] + DSC_QTABLE_ARRAY_SIZE);

	        MMPD_DSC_RawBuf2Jpeg(&bufAttr, &rect, 1, &fctlAttr);

	        MMPD_DSC_GetJpegSize(pThumbnailSize);

	        if (*pThumbnailSize > m_dscConfig.encParams.ulMaxThumbnailSize - 32) {
	            PRINTF("FAIL: Thumbnail size is too large. Please expand ulMaxThumbnailSize\r\n");
            }
	    }

		m_MultiShotInfo.ulExifThumbnailSize[m_MultiShotInfo.usShotIndex] = *pThumbnailSize;

		m_MultiShotInfo.ulUsedEndAddr[m_MultiShotInfo.usShotIndex] = m_MultiShotInfo.ulExifThumbnailAddr[m_MultiShotInfo.usShotIndex] 
		                                                           + m_MultiShotInfo.ulExifThumbnailSize[m_MultiShotInfo.usShotIndex];
		#endif
	}

	return MMP_ERR_NONE;
}

#if (DSC_SUPPORT_BASELINE_MP_FILE)
//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_EncodeLargeThumb
//  Description :
//------------------------------------------------------------------------------
static MMP_ERR MMPS_DSC_EncodeLargeThumb(MMPS_DSC_CAPTURE_INFO   	*pCaptureInfo,
										 MMP_DSC_JPEG_INFO       	*pJpegInfo,
										 MMP_ULONG               	ulThumbWidth, 
										 MMP_ULONG               	ulThumbHeight, 
										 MMP_ULONG               	*pThumbnailSize,
										 MMP_DSC_THUMB_INPUT_SRC	ubThumbnailInputMode,
										 MMP_SCAL_PIPEID        	pipeID)
{
	MMP_SCAL_FIT_RANGE       	fitrange;
	MMP_SCAL_GRAB_CTRL 			grabctl;
	MMP_GRAPHICS_RECT          	rect;
	MMP_DSC_DECODE_BUF 			decodebuf;
	MMP_DSC_CAPTURE_BUF			capturebuf;
	MMP_GRAPHICS_BUF_ATTR 	 	bufAttr;
	MMPD_FCTL_ATTR				fctlAttr;
	MMPD_FCTL_LINK				fctllink;

	if (!m_bMultiPicFormatEnable || !m_bEncLargeThumbEnable) {
		return MMP_ERR_NONE;
	}

	FCTL_PIPE_TO_LINK(pipeID, fctllink);

	/* Step 1: Set Qtable for thumbnail */
    if (!m_bUseCustomJpegQT) {
	    MMPD_DSC_SetJpegQTable((MMP_UBYTE *)m_dscConfig.encParams.ubDscQtable[MMP_DSC_CAPTURE_HIGH_QUALITY],
	                           (MMP_UBYTE *)m_dscConfig.encParams.ubDscQtable[MMP_DSC_CAPTURE_HIGH_QUALITY] + DSC_QTABLE_ARRAY_SIZE,
	                           (MMP_UBYTE *)m_dscConfig.encParams.ubDscQtable[MMP_DSC_CAPTURE_HIGH_QUALITY] + DSC_QTABLE_ARRAY_SIZE);
	} 
	else {
	    MMPD_DSC_SetJpegQTable(m_ubCustomJpegQT[0], m_ubCustomJpegQT[1], m_ubCustomJpegQT[2]);
	}

	MMPD_DSC_SetJpegQualityControl( MMP_DSC_JPEG_RC_ID_CAPTURE,
	                                MMP_TRUE,
		                            MMP_FALSE,
		                            m_dscConfig.encParams.ulMaxLargeThumbSize>>10,
		                            m_dscConfig.encParams.ulMaxLargeThumbSize>>10,
	    	                        3);
	
	if (ubThumbnailInputMode == MMP_DSC_THUMB_SRC_DECODED_JPEG ||
		ubThumbnailInputMode == MMP_DSC_THUMB_SRC_THUMB_PIPE)
	{
	    MMP_ULONG 	ulStoreWidth = ulThumbWidth;
		MMP_ULONG 	ulStoreHeight = ulThumbHeight;
		MMP_USHORT 	usHrange = 0;
		MMP_USHORT 	usVrange = 0;

		/* Step 2: prepare raw image */
        ulStoreWidth  = ALIGN16(ulStoreWidth);
        ulStoreHeight = ALIGN8(ulStoreHeight);

		// Set decode out buffer for primary jpeg decode to large thumbnail (YUV422).   
		bufAttr.ulBaseAddr = m_ulPrimaryJpeg4LargeThumbDecodeOutStart;

		if (ubThumbnailInputMode == MMP_DSC_THUMB_SRC_DECODED_JPEG)
		{
			decodebuf.ulLineBufStart    = bufAttr.ulBaseAddr + ALIGN_X(ulThumbWidth * 2, DSC_BUF_ALIGN_BASE) * ulThumbHeight;
			decodebuf.ulLineBufSize     = m_dscConfig.decParams.ulDscDecMaxLineBufSize;
			decodebuf.ulStorageTmpStart = decodebuf.ulLineBufStart + decodebuf.ulLineBufSize;
			decodebuf.ulStorageTmpSize  = m_dscConfig.decParams.ulDscDecMaxInbufSize;

	    	MMPD_DSC_SetDecodeBuffers(&decodebuf);

	    	// The setting for MMPD_DSC_DecodeJpegPic()
	    	pJpegInfo->bJpegFileName[0] 	= NULL;
	    	pJpegInfo->ulJpegBufAddr    	= m_MultiShotInfo.ulPrimaryJpegAddr[m_MultiShotInfo.usShotIndex]; 
	   		pJpegInfo->ulJpegBufSize    	= pCaptureInfo->ulJpegFileSize;
	        pJpegInfo->bDecodeThumbnail 	= MMP_FALSE;
	        pJpegInfo->bDecodeLargeThumb 	= MMP_FALSE;
	        pJpegInfo->bValid           	= MMP_TRUE;

	        // Get Jpeg information from SOF marker
		    if (MMPD_DSC_GetJpegInfo(pJpegInfo) != MMP_ERR_NONE) {
		        return MMP_DSC_ERR_JPEGINFO_FAIL;
		    }

	  		fitrange.fitmode    = MMP_SCAL_FITMODE_OUT;
	  		fitrange.scalerType	= MMP_SCAL_TYPE_SCALER;
	        fitrange.ulInWidth  = pJpegInfo->usPrimaryWidth;
	        fitrange.ulInHeight	= pJpegInfo->usPrimaryHeight;
	        
	        if ( (m_CaptureRotateType == MMP_DSC_CAPTURE_NO_ROTATE) ||
	             (m_CaptureRotateType == MMP_DSC_CAPTURE_ROTATE_RIGHT_180) ) 
	        {
	            fitrange.ulOutWidth  = ulStoreWidth;
	            fitrange.ulOutHeight = ulStoreHeight;
	        }
	        else {
	            fitrange.ulOutWidth  = ulStoreHeight;
	            fitrange.ulOutHeight = ulStoreWidth;
	        }

		    fitrange.ulInGrabX	= 1;
		    fitrange.ulInGrabY	= 1;
		    fitrange.ulInGrabW	= fitrange.ulInWidth;
		    fitrange.ulInGrabH	= fitrange.ulInHeight;

	        MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);

	        // Decode JPEG file into buffer
        	MMPD_DSC_DecodeJpegPic(	MMP_DISPLAY_COLOR_YUV422, 
	        						pJpegInfo, 
	        						&bufAttr,
	        						&fitrange,
	        						&grabctl, 
        							pipeID);
		}
		else if (ubThumbnailInputMode == MMP_DSC_THUMB_SRC_THUMB_PIPE)
		{
			MMPD_Scaler_GetGrabRange(m_fctlLinkCapture.scalerpath, MMP_SCAL_GRAB_STAGE_OUT, &usHrange, &usVrange);

	  		fitrange.fitmode    = MMP_SCAL_FITMODE_OUT;
	  		fitrange.scalerType	= MMP_SCAL_TYPE_SCALER;
	        fitrange.ulInWidth  = usHrange;
	        fitrange.ulInHeight = usVrange;

            if ( (m_CaptureRotateType == MMP_DSC_CAPTURE_NO_ROTATE) ||
                 (m_CaptureRotateType == MMP_DSC_CAPTURE_ROTATE_RIGHT_180) ) 
	        {
	            fitrange.ulOutWidth  = ulStoreWidth;
	            fitrange.ulOutHeight = ulStoreHeight;
	        }
	        else {
	            fitrange.ulOutWidth  = ulStoreHeight;
	            fitrange.ulOutHeight = ulStoreWidth;
	        }

		    fitrange.ulInGrabX	= 1;
		    fitrange.ulInGrabY	= 1;
		    fitrange.ulInGrabW	= fitrange.ulInWidth;
		    fitrange.ulInGrabH	= fitrange.ulInHeight;

	        MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);

			MMPD_DSC_StoreThumbSrcFrame(&bufAttr,
										&fitrange,
	        							&grabctl,
	        							m_fctlLinkCapture.scalerpath, 
	        							pipeID);
		}

		/* Step 3: Encode thumbnail */
	    capturebuf.ulCompressStart	= m_MultiShotInfo.ulLargeThumbJpegAddr[m_MultiShotInfo.usShotIndex];
    	capturebuf.ulCompressEnd	= m_ulLargeThumbJpegCompEnd;
    	capturebuf.ulLineStart		= m_ulPrimaryJpegLineStart;
	    MMPD_DSC_SetCaptureBuffers(&capturebuf);

		// Set the graphics source buffer attribute	    
	    bufAttr.ulBaseAddr = m_ulPrimaryJpeg4LargeThumbDecodeOutStart;
        bufAttr.colordepth = MMP_GRAPHICS_COLORDEPTH_YUV422_UYVY;

		// Thumbnail(RAW) encode to thumbnail(Jpeg)
	    if ( (m_CaptureRotateType == MMP_DSC_CAPTURE_NO_ROTATE) ||
	         (m_CaptureRotateType == MMP_DSC_CAPTURE_ROTATE_RIGHT_180) ) 
	    {                                     
	        bufAttr.usWidth   		= ulThumbWidth;
	        bufAttr.usHeight  		= ulThumbHeight;
	        bufAttr.usLineOffset	= bufAttr.usWidth * 2;  
			
	        rect.usLeft        		= 0;
	        rect.usTop           	= 0;
	        rect.usWidth        	= ulThumbWidth;
	        rect.usHeight        	= ulThumbHeight;
	    }
	    else {

	        bufAttr.usWidth        	= ulThumbHeight;
	        bufAttr.usHeight       	= ulThumbWidth;
	        bufAttr.usLineOffset   	= bufAttr.usWidth * 2;  

	        rect.usLeft          	= 0;
	        rect.usTop           	= 0;
	        rect.usWidth        	= ulThumbHeight;
	        rect.usHeight        	= ulThumbWidth;
	    }
        
	    fitrange.fitmode     	= MMP_SCAL_FITMODE_OUT;
	    fitrange.scalerType		= MMP_SCAL_TYPE_SCALER;
    	fitrange.ulInWidth   	= rect.usWidth;
    	fitrange.ulInHeight  	= rect.usHeight;
    	fitrange.ulOutWidth  	= rect.usWidth;
    	fitrange.ulOutHeight 	= rect.usHeight;

	    fitrange.ulInGrabX		= 1;
	    fitrange.ulInGrabY		= 1;
	    fitrange.ulInGrabW		= fitrange.ulInWidth;
	    fitrange.ulInGrabH		= fitrange.ulInHeight;

    	MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);

	    fctlAttr.fctllink       = fctllink;
	    fctlAttr.fitrange       = fitrange;
	    fctlAttr.grabctl        = grabctl;
	    fctlAttr.scalsrc		= MMP_SCAL_SOURCE_GRA;
	    fctlAttr.bSetScalerSrc	= MMP_TRUE;

        // Encode JPEG from graphic path.
	    MMPD_DSC_RawBuf2Jpeg(&bufAttr, &rect, 1, &fctlAttr);

	    MMPD_DSC_GetJpegSize(pThumbnailSize);
	    
	    if (*pThumbnailSize > m_dscConfig.encParams.ulMaxLargeThumbSize - 32) {
			
	        MMPD_DSC_SetJpegQTable((MMP_UBYTE *)m_dscConfig.encParams.ubDscQtable[MMP_DSC_CAPTURE_NORMAL_QUALITY],
	                               (MMP_UBYTE *)m_dscConfig.encParams.ubDscQtable[MMP_DSC_CAPTURE_NORMAL_QUALITY] + DSC_QTABLE_ARRAY_SIZE,
	                               (MMP_UBYTE *)m_dscConfig.encParams.ubDscQtable[MMP_DSC_CAPTURE_NORMAL_QUALITY] + DSC_QTABLE_ARRAY_SIZE);

	        MMPD_DSC_RawBuf2Jpeg(&bufAttr, &rect, 1, &fctlAttr);
	        							
	        MMPD_DSC_GetJpegSize(pThumbnailSize);

	        if (*pThumbnailSize > m_dscConfig.encParams.ulMaxLargeThumbSize - 32) {
	            PRINTF("FAIL: Thumbnail size is too large. Please expand ulMaxLargeThumbSize\r\n");
            }
	    }

		m_MultiShotInfo.ulLargeThumbJpegSize[m_MultiShotInfo.usShotIndex] = *pThumbnailSize;

		m_MultiShotInfo.ulLargeThumbUsedEndAddr[m_MultiShotInfo.usShotIndex] = 
						m_MultiShotInfo.ulLargeThumbJpegAddr[m_MultiShotInfo.usShotIndex] +
		                m_MultiShotInfo.ulLargeThumbJpegSize[m_MultiShotInfo.usShotIndex];
	}

	return MMP_ERR_NONE;
}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_GetCaptureStatus
//  Description :
//------------------------------------------------------------------------------
/** @brief The function check memory and number of image

The function report memory is enough or not and number of images in dram

@param[out] usDramStatus memory is enough or not, 0 : not enough, 1 : enough
@param[out] usIamgeNum number of images that still stored in DRAM.
@return MMP_ERR_NONE
*/
MMP_ERR MMPS_DSC_GetCaptureStatus(MMP_USHORT *usDramStatus, MMP_USHORT *usImgNumInBuf)
{
    if ((m_MultiShotInfo.ulPrimaryJpegAddr[m_MultiShotInfo.usShotIndex] + m_ulImageTargetSize) > m_ulPrimaryJpegCompEnd) {
        *usDramStatus = 0;
    } 
    else {
        *usDramStatus = 1;
    }
    
    *usImgNumInBuf = m_MultiShotInfo.usShotIndex - m_MultiShotInfo.usStoreIndex;
    
    return MMP_ERR_NONE;
}

#if 0
void _____Playback_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetPlaybackMode
//  Description :
//------------------------------------------------------------------------------
/** @brief The function sets the DSC system into playback mode

The function sets the DSC system into playback mode. 
It can be implemented by calling MMPS_DSC_EnablePreviewDisplay to disable preview mode

@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_SetPlaybackMode(MMP_USHORT usDecodeMode)
{
	MMPS_DSC_SetSystemBuf(NULL, MMP_FALSE, MMP_TRUE, MMP_TRUE);

	MMPS_DSC_EnablePreviewDisplay(m_ubDscModeSnrId, MMP_FALSE, MMP_FALSE);

	m_usDecodeMode = usDecodeMode;
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetPlaybackDispCtrl
//  Description :
//------------------------------------------------------------------------------
/** @brief The function sets the DSC playback mode display control parameter

The function sets the DSC playback mode display control parameter.
@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_SetPlaybackDispCtrl(MMP_USHORT usDispCtrl, MMP_USHORT usDispWinId)
{	
	m_DscPlaybackDispCtrl   = (MMP_DISPLAY_CONTROLLER) usDispCtrl;
	m_DscPlaybackWinId 		= (MMP_DISPLAY_WIN_ID) usDispWinId;

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetPlaybackPipe
//  Description :
//------------------------------------------------------------------------------
/** @brief
@param[in] ubPipe	which pipe for preview want to use.
@return It reports the status of the configuration.
*/
MMP_ERR MMPS_DSC_SetPlaybackPipe(MMP_UBYTE ubPipe)
{	
	m_fctlLinkPlayback.scalerpath 	= (MMP_SCAL_PIPEID) ubPipe;
	m_fctlLinkPlayback.icopipeID	= (MMP_ICO_PIPEID) ubPipe;
	m_fctlLinkPlayback.ibcpipeID	= (MMP_IBC_PIPEID) ubPipe;
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetJpegDisplay
//  Description :
//------------------------------------------------------------------------------
/** @brief Set the Display module for JPEG playback

The function set the Display module for JPEG playback, This API can only be called after MMPS_DSC_PlaybackJpeg()

@param[in] ulGrabX 		start x
@param[in] ulGrabY 		start y
@param[in] ulGrabW   	width
@param[in] ulGrabH  	height
@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_SetJpegDisplay(MMP_ULONG ulGrabX, MMP_ULONG ulGrabY, MMP_ULONG ulGrabW, MMP_ULONG ulGrabH)//
{
    MMP_SCAL_FIT_RANGE       	fitrange;
    MMP_SCAL_GRAB_CTRL     		grabctl;
    MMP_DISPLAY_DISP_ATTR  		dispAttr;
    MMP_ERR						retstatus;
    MMP_ULONG                   ratioX, ratioY;
    
    MMP_GRAPHICS_RECT       	rect;
    MMP_GRAPHICS_BUF_ATTR 		dstBufAttr;


    /* Note: Input parameter is based on the real JPEG frame domain. 
     * We should transform coordinate from real JPEG domain to decoded frame domain
     */  
    ratioX = (m_JpegPlayInfo.usJpegWidth * 1000) / m_JpegPlayInfo.usDecodeWidth;
    ratioY = (m_JpegPlayInfo.usJpegHeight * 1000) / m_JpegPlayInfo.usDecodeHeight;

    ulGrabX = (ulGrabX * 1000) / ratioX;
    ulGrabY = (ulGrabY * 1000) / ratioY;
    ulGrabW = (ulGrabW * 1000) / ratioX;
    ulGrabH = (ulGrabH * 1000) / ratioY;

    /* Parameter Check */
    if ((ulGrabW > m_JpegPlayInfo.usDecodeWidth) || 
        (ulGrabH > m_JpegPlayInfo.usDecodeHeight))
    {
        return MMP_DISPLAY_ERR_OVERRANGE;
    }
    if (((ulGrabX + ulGrabW) > m_JpegPlayInfo.usDecodeWidth) || 
        ((ulGrabY + ulGrabH) > m_JpegPlayInfo.usDecodeHeight))
    {
        return MMP_DISPLAY_ERR_OVERRANGE;
    }
    if ((ulGrabW == 0) || (ulGrabH == 0)) 
    {
        return MMP_DISPLAY_ERR_OVERRANGE;
    }

    /* Set display window parameter */
    dispAttr.usStartX           = ulGrabX;
    dispAttr.usStartY           = ulGrabY;
    dispAttr.usDisplayOffsetX   = m_JpegPlayInfo.usDispWinOfstX;
   	dispAttr.usDisplayOffsetY   = m_JpegPlayInfo.usDispWinOfstY;
   	dispAttr.bMirror    = m_JpegPlayInfo.bMirror;
   	dispAttr.rotatetype = m_JpegPlayInfo.rotateType;
    
   	if ((m_JpegPlayInfo.rotateType  == MMP_DISPLAY_ROTATE_NO_ROTATE) ||
 		(m_JpegPlayInfo.rotateType  == MMP_DISPLAY_ROTATE_RIGHT_180)) {
    
        dispAttr.usDisplayWidth   = ulGrabW;
        dispAttr.usDisplayHeight  = ulGrabH;

        /* Set buffer attribute to display window */
        MMPD_Display_BindBufToWin(&m_JpegPlayInfo.bufAttr, m_DscPlaybackWinId);
    }
    else {
        dispAttr.usDisplayWidth   = ulGrabH;
        dispAttr.usDisplayHeight  = ulGrabW;


        rect.usLeft             = 0;
        rect.usTop              = 0;
        rect.usWidth            = m_JpegPlayInfo.bufAttr.usWidth;
        rect.usHeight           = m_JpegPlayInfo.bufAttr.usHeight;

        dstBufAttr.ulBaseAddr	=  m_ulDSCCurMemAddr;
        dstBufAttr.ulBaseUAddr	=  dstBufAttr.ulBaseAddr + m_JpegPlayInfo.bufAttr.ulBaseUAddr - m_JpegPlayInfo.bufAttr.ulBaseAddr;
        dstBufAttr.ulBaseVAddr	=  dstBufAttr.ulBaseUAddr + m_JpegPlayInfo.bufAttr.ulBaseVAddr - m_JpegPlayInfo.bufAttr.ulBaseUAddr;

        dstBufAttr.usWidth		= m_JpegPlayInfo.bufAttr.usHeight;
        dstBufAttr.usHeight	    = m_JpegPlayInfo.bufAttr.usWidth;
        dstBufAttr.usLineOffset = dstBufAttr.usWidth;
        dstBufAttr.colordepth	= m_JpegPlayInfo.bufAttr.colordepth;

        m_bDMADone = MMP_FALSE;
        if (dispAttr.rotatetype == MMP_DISPLAY_ROTATE_RIGHT_90)
        {
        	retstatus = MMPD_DMA_RotateImageBuftoBuf(&m_JpegPlayInfo.bufAttr,
        			&rect,
        			&dstBufAttr,
        			0,
        			0,
        			MMP_GRAPHICS_ROTATE_RIGHT_90,
        			(DmaCallBackFunc *)MMPS_DSC_DMADone,
        			MMP_FALSE,
        			DMA_NO_MIRROR);
        }
        else if(dispAttr.rotatetype == MMP_DISPLAY_ROTATE_RIGHT_270)
        {
        	retstatus = MMPD_DMA_RotateImageBuftoBuf(&m_JpegPlayInfo.bufAttr,
        			&rect,
        			&dstBufAttr,
        			0,
        			0,
        			MMP_GRAPHICS_ROTATE_RIGHT_270,
        			(DmaCallBackFunc *)MMPS_DSC_DMADone,
        			MMP_FALSE,
        			DMA_NO_MIRROR);
        }

        if (retstatus != MMP_ERR_NONE) {
        	PRINTF("MMPD_DMA_RotateImageBuftoBuf Failed\r\n");
        }
        // Waiting the DMA move buf to buf done.
        while(!m_bDMADone);

        /* Set buffer attribute to display window */
        MMPD_Display_BindBufToWin(&dstBufAttr, m_DscPlaybackWinId);
    }
   	dispAttr.rotatetype = 0;
   	MMPD_Display_SetWinToDisplay(m_DscPlaybackWinId, &dispAttr);
    
    /* Set window scaling function */
   	do {
        
        fitrange.fitmode     = m_JpegPlayInfo.sFitMode;
 		fitrange.scalerType	 = MMP_SCAL_TYPE_WINSCALER;
        fitrange.ulInWidth   = dispAttr.usDisplayWidth;
        fitrange.ulInHeight  = dispAttr.usDisplayHeight;
        fitrange.ulOutWidth  = dispAttr.usDisplayWidth;
        fitrange.ulOutHeight = dispAttr.usDisplayHeight;
        
        fitrange.ulInGrabX	 = 1;
        fitrange.ulInGrabY	 = 1;
        fitrange.ulInGrabW	 = fitrange.ulInWidth;
        fitrange.ulInGrabH	 = fitrange.ulInHeight;
        
        MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);

   	    retstatus =	MMPD_Display_SetWinScaling(m_DscPlaybackWinId, &fitrange, &grabctl);

	} while (retstatus == MMP_DISPLAY_ERR_OVERRANGE);
		
	/* Refresh display */
	MMPD_Display_SetWinActive(m_DscPlaybackWinId, MMP_TRUE);
	MMPD_Display_SetDisplayRefresh(m_DscPlaybackDispCtrl);
	
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetJpegDisplayCcir
//  Description :
//------------------------------------------------------------------------------
/** @brief Set the CCIR display for JPEG playback

The function set the CCIR display for JPEG playback,
This API can only be called after MMPS_DSC_PlaybackJpeg()

@param[in] ulCcirFrameBuf   frame buffer address for CCIR display

@return It reports the status of the operation.
*/
static MMP_ERR MMPS_DSC_SetJpegDisplayCcir(MMP_ULONG ulCcirFrameBuf)
{
    MMP_USHORT  				ulCcirOutW;
    MMP_USHORT  				ulCcirOutH;
    MMP_GRAPHICS_DRAWRECT_ATTR  drawAttr;
    MMP_GRAPHICS_BUF_ATTR       srcBufAttr;
	MMP_GRAPHICS_BUF_ATTR 	    dstBufAttr;
    MMP_GRAPHICS_RECT          	rect;
    MMP_ERR                     err;

    ulCcirOutW = MMPS_Display_GetConfig()->ccir.usDisplayWidth;
    ulCcirOutH = MMPS_Display_GetConfig()->ccir.usDisplayHeight;

    /* Fill background color with black */
    if ((m_JpegPlayInfo.usDecodeWidth != ulCcirOutW) ||
        (m_JpegPlayInfo.usDecodeHeight != ulCcirOutH)) {

        drawAttr.type       = MMP_GRAPHICS_SOLID_FILL;
        drawAttr.bUseRect   = MMP_FALSE;
        drawAttr.usWidth    = ulCcirOutW;    
        drawAttr.usHeight   = ulCcirOutH;
        drawAttr.usLineOfst = ulCcirOutW * 2;
        drawAttr.ulBaseAddr = ulCcirFrameBuf;
        drawAttr.colordepth = MMP_GRAPHICS_COLORDEPTH_16;
        drawAttr.ulColor    = 0x0080; //For YUV422
        drawAttr.ulPenSize  = 0;
        drawAttr.ropcode    = MMP_GRAPHICS_ROP_SRCCOPY;

        MMPD_Graphics_DrawRectToBuf(&drawAttr, NULL, NULL);
    }

    /* Move decoded frame to ccir display buffer */
	rect.usLeft             = 0;
	rect.usTop              = 0;
	rect.usWidth            = m_JpegPlayInfo.usDecodeWidth;
	rect.usHeight           = m_JpegPlayInfo.usDecodeHeight;

	srcBufAttr.ulBaseAddr 	= m_JpegPlayInfo.bufAttr.ulBaseAddr;
	srcBufAttr.usWidth		= m_JpegPlayInfo.usDecodeWidth;
	srcBufAttr.usHeight	    = m_JpegPlayInfo.usDecodeHeight;
	srcBufAttr.usLineOffset = srcBufAttr.usWidth << 1;
	srcBufAttr.colordepth   = MMP_GRAPHICS_COLORDEPTH_16;

	dstBufAttr.ulBaseAddr	= ulCcirFrameBuf;
	dstBufAttr.usWidth		= ulCcirOutW;
	dstBufAttr.usHeight	    = ulCcirOutH;
	dstBufAttr.usLineOffset = dstBufAttr.usWidth << 1;
    dstBufAttr.colordepth	= MMP_GRAPHICS_COLORDEPTH_16;

	m_bDMADone = MMP_FALSE;

	err = MMPD_DMA_RotateImageBuftoBuf( &srcBufAttr, 
                						&rect, 
               	 						&dstBufAttr, 
                						(dstBufAttr.usWidth  - srcBufAttr.usWidth) / 2, 
                						(dstBufAttr.usHeight - srcBufAttr.usHeight) / 2, 
                						MMP_GRAPHICS_ROTATE_NO_ROTATE, 
                						(DmaCallBackFunc *)MMPS_DSC_DMADone, 
                						MMP_FALSE, 
                						DMA_NO_MIRROR);
    if (err) {
        return err;
    }
    
    // Waiting the DMA move buf to buf done.
    while(!m_bDMADone);

    /* Refresh ccir display */
    MMPD_CCIR_SetDisplayBufAddr(ulCcirFrameBuf);
    MMPD_CCIR_SetDisplayRefresh();

    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_PlaybackJpeg
//  Description :
//------------------------------------------------------------------------------
/** @brief Playback JPEG file on the display device

The function playbacks a Jpeg stream which is given its buffer address, buffer size, display start
location on the specified LCD panel and display direction.
@param[out]	pJpegInfo 			the Jpeg information
@retval MMP_ERR_NONE success
@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_PlaybackJpeg(MMP_DSC_JPEG_INFO *pJpegInfo)
{
    MMP_ERR						retstatus = MMP_ERR_NONE;
    MMP_SCAL_FIT_RANGE       	fitrange;
    MMP_SCAL_GRAB_CTRL     		grabctl;
    MMP_SCAL_FIT_MODE  			sFitmode;
    MMP_GRAPHICS_BUF_ATTR  		BufAttr;
    MMP_DSC_DECODE_BUF 			decodebuf;
    MMP_ULONG                   ulSramCurPos;
		
	static MMP_USHORT			usCurDecBufIdx = 0;
    MMP_ULONG					cur_buf, buf_num = 2;

	MMP_USHORT  				usPanelW;
    MMP_USHORT  				usPanelH;
	MMP_USHORT                  usWinOffsetX;
	MMP_USHORT 					usWinOffsetY;
	MMP_USHORT                  usDecodedW;
	MMP_USHORT					usDecodedH;
	MMP_DISPLAY_DEV_TYPE 		outputDev;
	MMP_DISPLAY_WIN_ID			winID 		= m_DscPlaybackWinId;
	MMP_DISPLAY_ROTATE_TYPE    	RotateType 	= MMP_DISPLAY_ROTATE_NO_ROTATE;
	MMP_ULONG                   ulTmpBufSize = 0;
	
    MEMSET(&fitrange, 0, sizeof(MMP_SCAL_FIT_RANGE));
    /* Disable display window */
	MMPD_Display_GetOutputDev(m_DscPlaybackDispCtrl, &outputDev);

	switch(outputDev){
		case MMP_DISPLAY_NONE:
			PRINTF("Output panel is not selected\r\n");
			return MMP_DSC_ERR_PLAYBACK_FAIL;
		case MMP_DISPLAY_RGB_LCD:
		case MMP_DISPLAY_TV:
		case MMP_DISPLAY_HDMI:
        case MMP_DISPLAY_CCIR:
			MMPD_Display_SetWinActive(winID, MMP_FALSE);
			break;
	}
	
    /* Allocate filename buffer, decompressed buffer, line buffer */
    MMPD_System_GetSramEndAddr(&ulSramCurPos);
    decodebuf.ulLineBufStart 	= ulSramCurPos;
    decodebuf.ulLineBufSize 	= m_dscConfig.decParams.ulDscDecMaxLineBufSize;

    MMPS_DSC_SetSystemBuf(NULL, MMP_FALSE, MMP_TRUE, MMP_TRUE);
    cur_buf = m_ulDSCCurMemAddr;

    decodebuf.ulDecompressStart = ALIGN32(cur_buf);
    decodebuf.ulDecompressEnd 	= ((decodebuf.ulDecompressStart + 
                                  m_dscConfig.decParams.ulDscDecMaxCompBufSize - 1)>>4)<<4;

    decodebuf.ulStorageTmpStart = decodebuf.ulDecompressEnd;
    decodebuf.ulStorageTmpSize 	= m_dscConfig.decParams.ulDscDecMaxInbufSize;
	
    cur_buf = ALIGN32(decodebuf.ulStorageTmpStart + decodebuf.ulStorageTmpSize);

    /*
     * For CCIR output mode, we allocate additional display buffer to 
     * support fit-in mode (by Move DMA line offset mode).
     */
    if (m_usDecodeMode == DSC_CCIR_DECODE_MODE) {
        MMP_ULONG   i, ulDispWidth, ulDispHeight, ulDispBufSize;

        ulDispWidth = m_dscConfig.decParams.usDecodeOutW[m_usDecodeMode];
        ulDispHeight = m_dscConfig.decParams.usDecodeOutH[m_usDecodeMode];

        switch (m_dscConfig.decParams.DecColorMode[m_usDecodeMode]) {
        case MMP_DISPLAY_COLOR_YUV422:
            ulDispBufSize = (((MMP_ULONG)ulDispWidth * ulDispHeight) << 1);
            break;
        default:
            return MMP_DSC_ERR_PLAYBACK_FAIL;
        }
        
        ulDispBufSize = ALIGN32(ulDispBufSize);
        
        for (i = 0; i < DSC_CCIR_DISP_BUF_CNT; i++) {
            m_ulCcirDispBuf[i] = cur_buf;
            cur_buf += ulDispBufSize;
        }
    }

    #if defined(ALL_FW)
    if (cur_buf > MMPS_System_GetMemHeapEnd()) {
        printc("\t= [HeapMemErr] MMPS_DSC_PlaybackJpeg =\r\n");
        return MMP_DSC_ERR_MEM_EXHAUSTED;
    }
    #endif

	MMPD_DSC_SetDecodeBuffers(&decodebuf);
	
	/* Get JPEG information */
    if (MMPD_DSC_OpenJpegFile(pJpegInfo) != MMP_ERR_NONE) {
        return MMP_DSC_ERR_JPEGINFO_FAIL;
    }

    if (pJpegInfo->ulFileSize > m_dscConfig.decParams.ulDscDecMaxCompBufSize) {
        return MMP_DSC_ERR_DEC_BUFFER_OVERFLOW;
    }

    if (MMPD_DSC_GetJpegInfo(pJpegInfo) != MMP_ERR_NONE) {
        MMPD_DSC_CloseJpegFile(pJpegInfo);
        MMPS_DSC_SetSystemBuf(NULL, MMP_FALSE, MMP_TRUE, MMP_FALSE);
		return MMP_DSC_ERR_JPEGINFO_FAIL;
    }

    if (pJpegInfo->jpgFormat == MMP_DSC_JPEG_FMT_NOT_BASELINE) {
        MMPD_DSC_CloseJpegFile(pJpegInfo);
        MMPS_DSC_SetSystemBuf(NULL, MMP_FALSE, MMP_TRUE, MMP_FALSE);
		return MMP_DSC_ERR_JPEGINFO_FAIL;
    }
	
	/* Initial Decode Output Resolution */
	if (m_sAhcDscPlayInfo.bUserDefine) {

		RotateType 		= m_sAhcDscPlayInfo.sRotateDir;
		sFitmode		= m_sAhcDscPlayInfo.sFitMode;

	    usWinOffsetX 	= m_sAhcDscPlayInfo.ulDispStartX;
	    usWinOffsetY 	= m_sAhcDscPlayInfo.ulDispStartY;

	    usDecodedW 		= m_sAhcDscPlayInfo.ulDecodeOutW;
	    usDecodedH 		= m_sAhcDscPlayInfo.ulDecodeOutH;
	}
	else {
		
		RotateType 		= m_dscConfig.decParams.rotatetype[m_usDecodeMode];
		sFitmode		= m_dscConfig.decParams.fitmode[m_usDecodeMode];

	    usWinOffsetX 	= 0;
	    usWinOffsetY 	= 0;

        usDecodedW 		= m_dscConfig.decParams.usDecodeOutW[m_usDecodeMode];
        usDecodedH 		= m_dscConfig.decParams.usDecodeOutH[m_usDecodeMode];	    
	}
	
  	/* Adjust display window resolution */
    if (m_usDecodeMode == DSC_CCIR_DECODE_MODE) {
        usPanelW = MMPS_Display_GetConfig()->ccir.usDisplayWidth;
        usPanelH = MMPS_Display_GetConfig()->ccir.usDisplayHeight;
    }
    else {
	    MMPD_Display_GetWidthHeight(m_DscPlaybackDispCtrl, &usPanelW, &usPanelH);
    }

	fitrange.ulInWidth  	= pJpegInfo->usPrimaryWidth;
	fitrange.ulInHeight 	= pJpegInfo->usPrimaryHeight;
	fitrange.ulOutWidth  	= usPanelW;
	fitrange.ulOutHeight	= usPanelH;

 	if ((RotateType == MMP_DISPLAY_ROTATE_RIGHT_90) || 
	    (RotateType == MMP_DISPLAY_ROTATE_RIGHT_270)) {

		fitrange.ulOutWidth  = usPanelH;
		fitrange.ulOutHeight = usPanelW;
	}

	/* Adjust output range when Jpeg size is smaller than display range */
	if (fitrange.ulOutWidth > fitrange.ulInWidth) {
		fitrange.ulOutWidth = fitrange.ulInWidth;
	}
	if (fitrange.ulOutHeight > fitrange.ulInHeight) {
		fitrange.ulOutHeight = fitrange.ulInHeight;
	}
	
	/* Adjust decode output resolution */
    if ((pJpegInfo->usPrimaryWidth * pJpegInfo->usPrimaryHeight) < (usDecodedW * usDecodedH)) {
        usDecodedW = pJpegInfo->usPrimaryWidth;
        usDecodedH = pJpegInfo->usPrimaryHeight;
    }
    else {
        MMP_ULONG ulSuggestW,ulSuggestH;

        MMPS_Display_AdjustScaleInSize(m_DscPlaybackDispCtrl,  
                                       fitrange.ulInWidth, 
                                       fitrange.ulInHeight, 
                                       fitrange.ulOutWidth, 
                                       fitrange.ulOutHeight, 
                                       &ulSuggestW, &ulSuggestH);

		usDecodedW = (MMP_USHORT)ulSuggestW;
		usDecodedH = (MMP_USHORT)ulSuggestH;
    }

	/* Check the picture is crossed panel boundary or not */
 	if ((RotateType == MMP_DISPLAY_ROTATE_NO_ROTATE) || 
	    (RotateType == MMP_DISPLAY_ROTATE_RIGHT_180)) {

		if (usWinOffsetX + usDecodedW > usPanelW) {
			PRINTF("DSC_PlaybackJpeg Fail: usWinOffsetX + usDecodedW > usPanelW (%d + %d > %d)\r\n",
					usWinOffsetX, usDecodedW, usPanelW);

			if (usWinOffsetY + usDecodedH > usPanelH) {
				PRINTF("DSC_PlaybackJpeg Fail: usWinOffsetY + usDecodedH > usPanelH (%d + %d > %d)\r\n", 
                        usWinOffsetY, usDecodedH, usPanelH);
			}
			
			MMPD_DSC_CloseJpegFile(pJpegInfo);
			return MMP_DSC_ERR_PLAYBACK_FAIL;
		}
	}
	else {

		if (usWinOffsetX + usDecodedH > usPanelW) {
			PRINTF("DSC_PlaybackJpeg Fail: usWinOffsetX + usDecodedH > usPanelW (%d + %d > %d)\r\n",
					usWinOffsetX, usDecodedH, usPanelW);

			if (usWinOffsetY + usDecodedW > usPanelH) {
				PRINTF("DSC_PlaybackJpeg Fail: usWinOffsetY + usDecodedW > usPanelH (%d + %d > %d)\r\n", 
						usWinOffsetY, usDecodedW, usPanelH);
			}
			
			MMPD_DSC_CloseJpegFile(pJpegInfo);
			return MMP_DSC_ERR_PLAYBACK_FAIL;
		}
	}

	/* Allocate decode output buffer (window buffer) */
    switch (m_dscConfig.decParams.DecColorMode[m_usDecodeMode]) {
	case MMP_DISPLAY_COLOR_RGB565:
    case MMP_DISPLAY_COLOR_YUV422:
		ulTmpBufSize = ALIGN32(usDecodedW * usDecodedH * 2);
	    if (usCurDecBufIdx && (m_usDecodeMode != DSC_CCIR_DECODE_MODE)) {
	        cur_buf += usCurDecBufIdx * ulTmpBufSize;
        }

        BufAttr.ulBaseAddr = cur_buf;
		cur_buf += ulTmpBufSize;
		break;	
	case MMP_DISPLAY_COLOR_RGB888:
		ulTmpBufSize = ALIGN32(usDecodedW * usDecodedH * 3);
	    if (usCurDecBufIdx && (m_usDecodeMode != DSC_CCIR_DECODE_MODE)) {
	        cur_buf += usCurDecBufIdx * ulTmpBufSize;
        }
        
        BufAttr.ulBaseAddr = cur_buf;
		cur_buf += ulTmpBufSize;
        break;	
	case MMP_DISPLAY_COLOR_YUV420:
		ulTmpBufSize = usDecodedW * usDecodedH;
	    if (usCurDecBufIdx && (m_usDecodeMode != DSC_CCIR_DECODE_MODE)) {
	        cur_buf += usCurDecBufIdx * ((ulTmpBufSize * 3) / 2);
        }
        
		BufAttr.ulBaseAddr	= cur_buf;
		BufAttr.ulBaseUAddr	= BufAttr.ulBaseAddr + ALIGN32(ulTmpBufSize);	
		BufAttr.ulBaseVAddr = BufAttr.ulBaseUAddr + ALIGN32(ulTmpBufSize >> 2);
		cur_buf += (ALIGN32(ulTmpBufSize) + ALIGN32(ulTmpBufSize>>2)*2);
		break;	
	case MMP_DISPLAY_COLOR_YUV420_INTERLEAVE:
		ulTmpBufSize = usDecodedW * usDecodedH;
	    if (usCurDecBufIdx && (m_usDecodeMode != DSC_CCIR_DECODE_MODE)) {
	        cur_buf += usCurDecBufIdx * ((ulTmpBufSize * 3) / 2);
        }
        
		BufAttr.ulBaseAddr 	= cur_buf;
		BufAttr.ulBaseUAddr	= BufAttr.ulBaseAddr + ALIGN32(ulTmpBufSize);
		BufAttr.ulBaseVAddr = BufAttr.ulBaseUAddr;
		cur_buf += (ALIGN32(ulTmpBufSize) + ALIGN32(ulTmpBufSize>>1));
        break;
	}

    #if defined(ALL_FW)
    if (cur_buf > MMPS_System_GetMemHeapEnd()) {
        printc("\t= [HeapMemErr] MMPS_DSC_PlaybackJpeg =\r\n");
        return MMP_DSC_ERR_MEM_EXHAUSTED;
    }
    #endif

	/* Calculate the grab range of decode out image */
	fitrange.fitmode    	= sFitmode;
    fitrange.scalerType 	= MMP_SCAL_TYPE_SCALER;
    fitrange.ulInWidth  	= pJpegInfo->usPrimaryWidth;
    fitrange.ulInHeight 	= pJpegInfo->usPrimaryHeight;
	fitrange.ulOutWidth  	= usDecodedW;
	fitrange.ulOutHeight 	= usDecodedH;

    fitrange.ulInGrabX 		= 1;
    fitrange.ulInGrabY 		= 1;
    fitrange.ulInGrabW 		= fitrange.ulInWidth;
    fitrange.ulInGrabH 		= fitrange.ulInHeight;

	if ((fitrange.ulOutWidth > fitrange.ulInWidth) && 
		(fitrange.ulOutHeight > fitrange.ulInHeight)) {
	
        fitrange.ulOutWidth  = fitrange.ulInWidth;
        fitrange.ulOutHeight = fitrange.ulInHeight;
	}

    MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);

    /* Decode JPEG to IBC buffer */
    retstatus = MMPD_DSC_DecodeJpegPic(	m_dscConfig.decParams.DecColorMode[m_usDecodeMode],
                                        pJpegInfo, 
                                        &BufAttr, 
	        							&fitrange,
                                        &grabctl, 
	        							m_fctlLinkPlayback.scalerpath);

	MMPD_DSC_CloseJpegFile(pJpegInfo);

    if (retstatus != MMP_ERR_NONE) {
        return retstatus;
    }

    // If sFitmode is MMP_SCAL_FITMODE_IN,  output size may change.
    usDecodedW = 1 + (grabctl.ulOutEdX - grabctl.ulOutStX);
    usDecodedH = 1 + (grabctl.ulOutEdY - grabctl.ulOutStY);
    
    MMPD_DSC_SetFileNameAddr(0);

	/* Fill the m_JpegPlayInfo structure */
    m_JpegPlayInfo.usJpegWidth          = pJpegInfo->usPrimaryWidth;
    m_JpegPlayInfo.usJpegHeight         = pJpegInfo->usPrimaryHeight;
    m_JpegPlayInfo.usDecodeWidth        = usDecodedW;
    m_JpegPlayInfo.usDecodeHeight       = usDecodedH;
    m_JpegPlayInfo.usDisplayWidth       = usDecodedW; //Not Used
    m_JpegPlayInfo.usDisplayHeight      = usDecodedH; //Not Used
	m_JpegPlayInfo.jpeginfo             = *pJpegInfo;
	m_JpegPlayInfo.bufAttr         		= BufAttr;
	m_JpegPlayInfo.sFitMode				= sFitmode;
	m_JpegPlayInfo.rotateType           = RotateType;
	m_JpegPlayInfo.DecodeColorFmt       = m_dscConfig.decParams.DecColorMode[m_usDecodeMode];
	m_JpegPlayInfo.bMirror				= m_dscConfig.decParams.bMirror[m_usDecodeMode];

    /* Put Image at The Center of Panel */
    if ((m_JpegPlayInfo.rotateType  == MMP_DISPLAY_ROTATE_NO_ROTATE) || 
 		(m_JpegPlayInfo.rotateType  == MMP_DISPLAY_ROTATE_RIGHT_180)) {
    
        m_JpegPlayInfo.usDispWinOfstX   = ((usPanelW - m_JpegPlayInfo.usDecodeWidth) > 0) ? (usPanelW - m_JpegPlayInfo.usDecodeWidth) >> 1 : 0;
        m_JpegPlayInfo.usDispWinOfstY   = ((usPanelH - m_JpegPlayInfo.usDecodeHeight) > 0) ? (usPanelH - m_JpegPlayInfo.usDecodeHeight) >> 1 : 0;
    }
    else {
        m_JpegPlayInfo.usDispWinOfstX   = ((usPanelW - m_JpegPlayInfo.usDecodeHeight) > 0) ? (usPanelW - m_JpegPlayInfo.usDecodeHeight) >> 1 : 0;
       	m_JpegPlayInfo.usDispWinOfstY   = ((usPanelH - m_JpegPlayInfo.usDecodeWidth) > 0) ? (usPanelH - m_JpegPlayInfo.usDecodeWidth) >> 1 : 0; 
    }

    if (m_usDecodeMode == DSC_CCIR_DECODE_MODE) {
    	retstatus = MMPS_DSC_SetJpegDisplayCcir(m_ulCcirDispBuf[m_ulCcirDispBufIdx]);
        m_ulCcirDispBufIdx = (m_ulCcirDispBufIdx + 1) % DSC_CCIR_DISP_BUF_CNT;
    }
    else {
        /* The setting of window and display */
        retstatus = MMPS_DSC_SetJpegDisplay(0, 0, pJpegInfo->usPrimaryWidth, pJpegInfo->usPrimaryHeight);
        usCurDecBufIdx = (usCurDecBufIdx + 1) % buf_num;
    }

    return retstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_PlaybackCalculatePTZ
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_PlaybackCalculatePTZ(MMP_ULONG ulZoomStep,
									  MMP_ULONG *pulGrabX, 	MMP_ULONG *pulGrabY, 
									  MMP_ULONG *pulGrabW,	MMP_ULONG *pulGrabH)
{
    MMP_ULONG  	ulMaxZoomStep 	= 10; //TBD
    MMP_ULONG   ulMaxZoomRatioX, ulMaxZoomRatioY;
    MMP_ULONG  	ulZoomRatioX, ulZoomRatioY;
	MMP_ULONG	ulJpegW 		= m_JpegPlayInfo.usJpegWidth;
	MMP_ULONG	ulJpegH 		= m_JpegPlayInfo.usJpegHeight;
	MMP_ULONG	ulDecOutW 		= m_JpegPlayInfo.usDecodeWidth;
	MMP_ULONG	ulDecOutH 		= m_JpegPlayInfo.usDecodeHeight;
	
	ulMaxZoomRatioX = (ulJpegW * 1000) / ulDecOutW;
	ulMaxZoomRatioY = (ulJpegH * 1000) / ulDecOutH;
	
    ulZoomRatioX = 1000 + ((ulMaxZoomRatioX - 1000) * ulZoomStep) / ulMaxZoomStep;
    ulZoomRatioY = 1000 + ((ulMaxZoomRatioY - 1000) * ulZoomStep) / ulMaxZoomStep;

    *pulGrabW = (ulJpegW * 1000) / ulZoomRatioX;
    *pulGrabH = (ulJpegH * 1000) / ulZoomRatioY;

    *pulGrabX = (ulJpegW - *pulGrabW) / 2;
    *pulGrabY = (ulJpegH - *pulGrabH) / 2;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_PlaybackExecutePTZ
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_PlaybackExecutePTZ(MMP_ULONG ulGrabX, MMP_ULONG ulGrabY,
								 	MMP_ULONG ulGrabW, MMP_ULONG ulGrabH)
{
	MMP_SCAL_FIT_RANGE     	fitrange;
	MMP_SCAL_GRAB_CTRL   	grabctl;
	MMP_ERR					retstatus;
	
	MEMSET(&fitrange, 0, sizeof(MMP_SCAL_FIT_RANGE));
    if (MMPD_DSC_OpenJpegFile(&(m_JpegPlayInfo.jpeginfo)) != MMP_ERR_NONE) {
        return MMP_DSC_ERR_CAPTURE_FAIL;
    }

	/* Calculate the grab range of decode out image */
	fitrange.fitmode    	= m_JpegPlayInfo.sFitMode;
    fitrange.ulInWidth  	= m_JpegPlayInfo.jpeginfo.usPrimaryWidth;
    fitrange.ulInHeight 	= m_JpegPlayInfo.jpeginfo.usPrimaryHeight;
	fitrange.ulOutWidth  	= m_JpegPlayInfo.usDecodeWidth;
	fitrange.ulOutHeight 	= m_JpegPlayInfo.usDecodeHeight;

    fitrange.ulInGrabX 		= (ulGrabX == 0) ? (1) : (ulGrabX);
    fitrange.ulInGrabY 		= (ulGrabY == 0) ? (1) : (ulGrabY);
    fitrange.ulInGrabW 		= ulGrabW;
    fitrange.ulInGrabH 		= ulGrabH;

	if ((fitrange.ulOutWidth > fitrange.ulInWidth) && (fitrange.ulOutHeight > fitrange.ulInHeight)) {
	
		fitrange.ulOutWidth  = fitrange.ulInWidth;
		fitrange.ulOutHeight = fitrange.ulInHeight;
	}
	
    MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);

    /* Decode JPEG to IBC buffer */
    retstatus = MMPD_DSC_DecodeJpegPic(m_JpegPlayInfo.DecodeColorFmt,
	        						   &(m_JpegPlayInfo.jpeginfo), 
	        						   &(m_JpegPlayInfo.bufAttr),
	        						   &fitrange,
	        						   &grabctl, 
	        						   m_fctlLinkPlayback.scalerpath);

	MMPD_DSC_CloseJpegFile(&(m_JpegPlayInfo.jpeginfo));

    if (retstatus != MMP_ERR_NONE) {
        return retstatus;
    }

    if (m_usDecodeMode == DSC_CCIR_DECODE_MODE) {
    	retstatus = MMPS_DSC_SetJpegDisplayCcir(m_ulCcirDispBuf[m_ulCcirDispBufIdx]);
        m_ulCcirDispBufIdx = (m_ulCcirDispBufIdx + 1) % DSC_CCIR_DISP_BUF_CNT;
    }
    else {
        retstatus = MMPD_Display_SetDisplayRefresh(m_DscPlaybackDispCtrl);
    }

    return retstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_ExitJpegPlayback
//  Description : Release memory allocated by JPEG playback
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_ExitJpegPlayback(void)
{
    MMPS_DSC_SetSystemBuf(NULL, MMP_FALSE, MMP_TRUE, MMP_FALSE);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_GetJpegInfo
//  Description :
//------------------------------------------------------------------------------
/** @brief Get the JPEG information

The function reads an input Jpeg buffer (usJpegBufAddr) from an input Jpeg information structure and
writes back the required fields in the same structure. It parses the input Jpeg stream data and
writes/updates the fields in the input structure

@param[in] pJpegInfo the jpeg information data structure
@param[out] pJpegInfo the jpeg information data structure
@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_GetJpegInfo(MMP_DSC_JPEG_INFO *pJpegInfo)
{
    MMPS_DSC_SetSystemBuf(NULL, MMP_FALSE, MMP_TRUE, MMP_TRUE);
    
    if (MMPD_DSC_OpenJpegFile(pJpegInfo) != MMP_ERR_NONE) {
        MMPS_DSC_SetSystemBuf(NULL, MMP_FALSE, MMP_TRUE, MMP_FALSE);
        return MMP_DSC_ERR_JPEGINFO_FAIL;
    }
    
	if (MMPD_DSC_GetJpegInfo(pJpegInfo) != MMP_ERR_NONE) {
	    MMPD_DSC_CloseJpegFile(pJpegInfo);
	    MMPS_DSC_SetSystemBuf(NULL, MMP_FALSE, MMP_TRUE, MMP_FALSE);  
		return MMP_DSC_ERR_JPEGINFO_FAIL;
    }
	else {
	    MMPD_DSC_CloseJpegFile(pJpegInfo);
	    MMPS_DSC_SetSystemBuf(NULL, MMP_FALSE, MMP_TRUE, MMP_FALSE);
	}
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_DecodeThumbFirst
//  Description :
//------------------------------------------------------------------------------
/** @brief Decode JPEG file from the EXIF first. 
           If EXIF does not exists, or the thumb image size is smaller the the required size, then decode the JPEG file.
           
The function decodes and plays back the Jpeg picture
@param[in] pJpegInfo        The jpeg information
@param[in] usOutputWidth    The output width for Scaler
@param[in] usOutputHeight   The output height for Scaler
@param[in] outputformat     The output format
@retval MMP_ERR_NONE success
@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_DecodeThumbFirst(MMP_DSC_JPEG_INFO       	*pJpegInfo, 
                                  MMP_USHORT              	usOutputWidth, 
                                  MMP_USHORT              	usOutputHeight, 
                                  MMP_DISPLAY_COLORMODE  	outputformat)
{
    MMP_ERR						retstatus;
    MMP_SCAL_FIT_RANGE       	fitrange;
    MMP_SCAL_GRAB_CTRL     		grabctl;
    MMP_GRAPHICS_BUF_ATTR  		BufAttr;
    MMP_DSC_DECODE_BUF 			decodebuf;
    MMP_ULONG					cur_buf;
    MMP_DSC_JPEG_INFO           TempJpegInfo;

    /* Allocate filename buffer, decompressed buffer, line buffer */
    MMPS_DSC_SetSystemBuf(NULL, MMP_FALSE, MMP_TRUE, MMP_TRUE);

    cur_buf = m_ulDSCCurMemAddr;

    BufAttr.ulBaseAddr          = cur_buf;

    decodebuf.ulDecompressStart = BufAttr.ulBaseAddr + ALIGN32(usOutputWidth * 2) * usOutputHeight;
    decodebuf.ulDecompressEnd 	= ((decodebuf.ulDecompressStart + 
    								m_dscConfig.decParams.ulDscDecMaxCompBufSize - 1)>>4)<<4;

    decodebuf.ulLineBufStart    = decodebuf.ulDecompressEnd + 16;
    decodebuf.ulLineBufSize     = m_dscConfig.decParams.ulDscDecMaxLineBufSize;

    decodebuf.ulStorageTmpStart = decodebuf.ulLineBufStart + decodebuf.ulLineBufSize;
    decodebuf.ulStorageTmpSize  = m_dscConfig.decParams.ulDscDecMaxInbufSize;

    #if defined(ALL_FW)
    if (cur_buf > MMPS_System_GetMemHeapEnd()) {
        printc("\t= [HeapMemErr] MMPS_DSC_DecodeThumbFirst =\r\n");
        return MMP_DSC_ERR_MEM_EXHAUSTED;
    }
    #endif

    MMPD_DSC_SetDecodeBuffers(&decodebuf);

    /* Get JPEG information */
    if (MMPD_DSC_OpenJpegFile(pJpegInfo) != MMP_ERR_NONE) {
        return MMP_FS_ERR_OPEN_FAIL;
    }

    if (MMPD_DSC_GetJpgAndExifInfo(pJpegInfo) != MMP_ERR_NONE) {
        MMPD_DSC_CloseJpegFile(pJpegInfo);
        return MMP_DSC_ERR_DECODE_FAIL;
    }
  	
  	/* Calculate the grab range of decode out image */
    if ((pJpegInfo->usThumbWidth >= usOutputWidth) && (pJpegInfo->usThumbHeight >= usOutputHeight)) {
    
        fitrange.fitmode     = MMP_SCAL_FITMODE_OPTIMAL;
        fitrange.ulInWidth   = pJpegInfo->usThumbWidth;
        fitrange.ulInHeight  = pJpegInfo->usThumbHeight;
        fitrange.ulOutWidth  = usOutputWidth;
        fitrange.ulOutHeight = usOutputHeight;

        fitrange.ulInGrabX   = 1;
        fitrange.ulInGrabY   = 1;
        fitrange.ulInGrabW   = fitrange.ulInWidth;
        fitrange.ulInGrabH   = fitrange.ulInHeight;

        MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);

        TempJpegInfo = *pJpegInfo;
        
        // Cheat MMPD_DSC_DecodeJpegPic(), TBD
        TempJpegInfo.usPrimaryWidth  = pJpegInfo->usThumbWidth;
        TempJpegInfo.usPrimaryHeight = pJpegInfo->usThumbHeight;
        
        MMPD_DSC_SetDecodeImgOffset(pJpegInfo->ulThumbOffset, pJpegInfo->ulThumbOffset + pJpegInfo->ulThumbSize);
    }
    else {
        // Use primary Jpeg to decode
        fitrange.fitmode     = MMP_SCAL_FITMODE_OPTIMAL;
        fitrange.ulInWidth   = pJpegInfo->usPrimaryWidth;
        fitrange.ulInHeight  = pJpegInfo->usPrimaryHeight;
        fitrange.ulOutWidth  = usOutputWidth;
        fitrange.ulOutHeight = usOutputHeight;

        fitrange.ulInGrabX   = 1;
        fitrange.ulInGrabY   = 1;
        fitrange.ulInGrabW   = fitrange.ulInWidth;
        fitrange.ulInGrabH   = fitrange.ulInHeight;

        MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);

        TempJpegInfo = *pJpegInfo;
        
        MMPD_DSC_SetDecodeImgOffset(0, pJpegInfo->ulFileSize);
    }

	/* Decode JPEG to IBC buffer */
    retstatus = MMPD_DSC_DecodeJpegPic(	outputformat, 
    									&TempJpegInfo, 
    									&BufAttr, 
    									&fitrange,
    									&grabctl, 
    									m_fctlLinkPlayback.scalerpath);

    MMPD_DSC_CloseJpegFile(pJpegInfo);

    if (retstatus != MMP_ERR_NONE) {
        return retstatus;
    }
    
    /* Fill the m_JpegPlayInfo structure */
	m_JpegPlayInfo.bufAttr = BufAttr;

    /* Release memory allocated by JPEG playback */
    MMPS_DSC_SetSystemBuf(NULL, MMP_FALSE, MMP_TRUE, MMP_FALSE);
    
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_GetJpegDispBufAttr
//  Description :
//------------------------------------------------------------------------------	
/**
  @brief Get the buffer attributer of decode jpeg.
  @retval MMP_ERR_NONE success
  @return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_GetJpegDispBufAttr(MMP_GRAPHICS_BUF_ATTR *pJpegBufAttr) 
{
    *pJpegBufAttr = m_JpegPlayInfo.bufAttr;

    return MMP_ERR_NONE;
}

#if 0
void _____Sticker_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetSticker
//  Description :
//------------------------------------------------------------------------------
/*
  @brief Set Capture sticker Jpeg.
  @param[in] pPrimaryJpgSticker Set the sticker attribute of primary jpeg
  @param[in] pThumbnailSticker 	Set the sticker attribute of thumbnail
  @return MMP_ERR_NONE: Success
  @return Others: Fail
*/
MMP_ERR MMPS_DSC_SetSticker(MMP_STICKER_ATTR *pPrimaryJpgSticker, MMP_STICKER_ATTR *pThumbnailSticker)
{
    if (pPrimaryJpgSticker != NULL) {
		m_bEnablePrimaryJpegSticker = MMP_TRUE;
		m_ubPrimaryJpegStickerId = pPrimaryJpgSticker->ubStickerId;
        MMPD_Icon_SetAttributes(m_ubPrimaryJpegStickerId, pPrimaryJpgSticker);
    }
    else { 
		m_bEnablePrimaryJpegSticker = MMP_FALSE;
		MMPD_Icon_SetEnable(m_ubPrimaryJpegStickerId, MMP_FALSE);
    }

    if (pThumbnailSticker != NULL) {
		m_bEnableThumbnailSticker = MMP_TRUE; 
		m_ubThumbnailStickerId = pThumbnailSticker->ubStickerId;
        MMPD_Icon_SetAttributes(m_ubThumbnailStickerId, pThumbnailSticker);
    }
    else {
		m_bEnableThumbnailSticker = MMP_FALSE;    
		MMPD_Icon_SetEnable(m_ubThumbnailStickerId, MMP_FALSE);
    }

	return MMP_ERR_NONE;
}

#if 0
void _____Exif_Functions_____(){}
#endif

#if (DSC_SUPPORT_BASELINE_MP_FILE)
//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_EnableMultiPicFormat
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_EnableMultiPicFormat(MMP_BOOL bEnableMpf, MMP_BOOL bEnableLargeThumb)
{
	m_bMultiPicFormatEnable = bEnableMpf;
	m_bEncLargeThumbEnable 	= bEnableLargeThumb;

	MMPD_DSC_EnableMultiPicFormat(bEnableMpf);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_ConfigLargeThumbRes
//  Description : The primary Jpeg width/height need to configurate first.
//------------------------------------------------------------------------------
/** @brief The function set the thumbnail width and height
@param[in] ulThumbWidth   The large thumbnail widht
@param[in] ulThumbHeight  The large thumbnail height

@return MMP_ERR_NONE.
*/
MMP_ERR MMPS_DSC_ConfigLargeThumbRes(MMP_UBYTE ubResMode)
{
    MMP_USHORT	usGCD = 0;
    MMP_ULONG	ulRatioH, ulRatioV;
    
    m_ubLargeThumbResMode = ubResMode;
	
	/* Calculate Jpeg ratio */
	usGCD = Greatest_Common_Divisor(m_usJpegWidth, m_usJpegHeight);

	ulRatioH = m_usJpegWidth / usGCD;
	ulRatioV = m_usJpegHeight / usGCD;

    /* Calculate Large Thumbnail resolution */
    if (m_ubLargeThumbResMode == MMP_DSC_LARGE_THUMB_RES_FHD) 
    {
    	if (m_usJpegWidth >= m_usJpegHeight) {
			m_ulLargeThumbHeight	= 1080;
			m_ulLargeThumbWidth 	= (m_ulLargeThumbHeight / ulRatioV) * ulRatioH;
    	}
    	else {
			// TBD
    	}
    }
    else if (m_ubLargeThumbResMode == MMP_DSC_LARGE_THUMB_RES_VGA) 
    {
		if (m_usJpegWidth >= m_usJpegHeight) {
			m_ulLargeThumbHeight	= 480;
			m_ulLargeThumbWidth 	= (m_ulLargeThumbHeight / ulRatioV) * ulRatioH;
    	}
    	else {
			// TBD
    	}
    } 
    else {
    	PRINTF("Wrong Large Thumbnail Mode\r\n");
    }
    
    return MMP_ERR_NONE;
}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_ConfigThumbnail
//  Description :
//------------------------------------------------------------------------------
/** @brief The function set the thumbnail width and height
@param[in] ThumbWidth   The thumbnail widht
@param[in] ThumbHeight  The thumbnail height

@return MMP_ERR_NONE.
*/
MMP_ERR MMPS_DSC_ConfigThumbnail(MMP_ULONG ulThumbWidth, MMP_ULONG ulThumbHeight, MMP_DSC_THUMB_INPUT_SRC thumbInputMode)
{
    m_ulDSCThumbWidth    = ulThumbWidth;
    m_ulDSCThumbHeight   = ulThumbHeight;
    m_thumbnailInputMode = thumbInputMode;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_OpenEXIFFile
//  Description :
//------------------------------------------------------------------------------
/** @brief The function open exif file

The function open jpeg file for getting exif info

@param[in] pubFileName image file name
@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_OpenEXIFFile(MMP_UBYTE *pubFileName, MMP_USHORT usExifNodeId)
{
    MMP_ERR 	error = MMP_ERR_NONE;
    MMP_ULONG 	ulFileNameLen = 0;

	MMPS_DSC_SetSystemBuf(NULL, MMP_FALSE, MMP_TRUE, MMP_TRUE);
	
    MMPD_DSC_SetExifWorkingBuffer(usExifNodeId, (MMP_UBYTE*)m_ulDSCCurMemAddr, m_dscConfig.decParams.ulExifWorkingBufSize, MMP_TRUE);
 
    ulFileNameLen = STRLEN((MMP_BYTE *)pubFileName);
    
    error = MMPD_DSC_OpenEXIFFile(pubFileName, ulFileNameLen, usExifNodeId);
    
    return error;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_UpdateExifBuffer
//  Description :
//------------------------------------------------------------------------------
/** @brief The function will update exif memory

The function will update exif memory which is pre-set by MMPS_DSC_SetExifWorkingBuffer

@param[in] pData the update data buffer
@param[in] ulDataSize update data size
@param[in] usTag the tag id for update or add
@param[in] usIfd the ifd id for update 
usIfd = 0 Modify IFD0 TAG
usIfd = 1 Modify IFD1 TAG
usIfd = 2 Modify GPS TAG
usIfd = 3 Modify EXIF TAG
@param[in] usType updated tag data type
@param[in] ulCount updated tag data count
@param[in] bFotUpdate update the exif update buffer or capture buffer
@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_UpdateExifBuffer(MMP_USHORT 	usExifNodeId, 
								  MMP_USHORT 	usIfd, 	MMP_USHORT 	usTag,
								  MMP_USHORT 	usType, MMP_ULONG 	ulCount,
								  MMP_UBYTE		*pData, MMP_ULONG 	ulDataSize, 
                                  MMP_BOOL 		bForUpdate)
{
    return MMPD_DSC_UpdateExifNode(usExifNodeId, usIfd, usTag, usType, ulCount, pData, ulDataSize, bForUpdate);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_GetImageEXIFInfo
//  Description :
//------------------------------------------------------------------------------
/** @brief The function get image exif info

The function get image exif info by ifd and tag id and return data size

@param[in] 	usIfd 	The IFD index of tag field
@param[in] 	usTag 	The ID of tag field
@param[out] pulSize The data size of image EXIF info
@param[out] pData 	The data of image EXIF info

@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_GetImageEXIFInfo(MMP_USHORT usExifNodeId, MMP_USHORT usIfd, MMP_USHORT usTag, MMP_UBYTE *pData, MMP_ULONG *pulSize)
{
    return MMPD_DSC_GetImageEXIFInfo(usExifNodeId, usIfd, usTag, pData, pulSize);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_CloseEXIFFile
//  Description : The function close Exif file
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_CloseEXIFFile(MMP_USHORT usExifNodeId)
{
    return MMPD_DSC_CloseEXIFFile(usExifNodeId);
}

#if (EXIF_MANUAL_UPDATE)
//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_GetImageEXIF
//  Description : 
//------------------------------------------------------------------------------
MMP_BOOL MMPS_DSC_GetImageEXIF(MMP_ULONG Index, MMPS_DSC_EXIF_CONFIG *pExif)
{
    MMPS_DSC_EXIF_CONFIG	*ptr;
    MMP_ULONG              	Temp;
    MMP_ULONG              	ArrayEnd = (MMP_ULONG)m_ubExifSettingArray + EXIF_CONFIG_ARRAY_SIZE;
    MMP_ULONG              	i = 0;
    
    pExif->Data = NULL;
    
    Temp = ALIGN4((MMP_ULONG)m_ubExifSettingArray);
    ptr  = (MMPS_DSC_EXIF_CONFIG*)(Temp);
    
    for(;;) {
        if (ptr->AtomSize != 0) {
            if(i >= Index)
                break;
                
            ptr = (MMPS_DSC_EXIF_CONFIG*)((MMP_ULONG)ptr + ptr->AtomSize);
            
            i++;
        }
        else {
            return MMP_FALSE;
        }
        
        if ( ((MMP_ULONG)ptr) >= ArrayEnd )
            return MMP_FALSE;
    }
    
    pExif->DataSize     = ptr->DataSize;
    pExif->TagID        = ptr->TagID;
    pExif->IfdID        = ptr->IfdID;
    pExif->TagType      = ptr->TagType;
    pExif->TagDataSize  = ptr->TagDataSize;
    pExif->Data         = (MMP_UBYTE*)((MMP_ULONG)ptr + sizeof(MMPS_DSC_EXIF_CONFIG) - 4);
    
    return MMP_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_SetImageEXIF
//  Description : 
//------------------------------------------------------------------------------
MMP_BOOL MMPS_DSC_SetImageEXIF(MMP_UBYTE *pData, MMP_ULONG DataSize, MMP_USHORT uwIfdIdx, MMP_USHORT uwTagID, MMP_USHORT uwType, MMP_USHORT uwCount)
{
	MMP_ERR                 error = MMP_ERR_NONE;
    MMPS_DSC_EXIF_CONFIG    *pExif;
    MMP_ULONG               Temp;
    MMP_BOOL                bUpdate = MMP_FALSE;
    MMP_ULONG               ArrayEnd = (MMP_ULONG)m_ubExifSettingArray + EXIF_CONFIG_ARRAY_SIZE;
    MMP_ULONG               AtomLen;

    if (pData == NULL) {
        return MMP_FALSE;
	}
	
    if (EXIF_TYPE_ASCII == uwType) {
        if (DataSize > 256) {
            return MMP_FALSE;
        }
    }

    Temp = ALIGN4((MMP_ULONG)m_ubExifSettingArray);
    pExif = (MMPS_DSC_EXIF_CONFIG*)(Temp);

    if (EXIF_TYPE_ASCII == uwType) {
        AtomLen = ALIGN4(EXIF_ASCII_LENGTH + sizeof(MMPS_DSC_EXIF_CONFIG) - 4);
    }
    else {
        AtomLen = ALIGN4(DataSize + sizeof(MMPS_DSC_EXIF_CONFIG) - 4);
    }

    for(;;) {
        if (pExif->AtomSize != 0) {
            if ((pExif->TagID == uwTagID) && (pExif->IfdID == uwIfdIdx)) {
                bUpdate = MMP_TRUE;
                break;
            }
            
            pExif = (MMPS_DSC_EXIF_CONFIG*)((MMP_ULONG)pExif + pExif->AtomSize);
        }
        else {
            break;
        }
        
        if (((MMP_ULONG)pExif + AtomLen) >= ArrayEnd) {
            return MMP_FALSE;
    	}
    }

    if (EXIF_TYPE_ASCII == uwType) {
        pExif->AtomSize = ALIGN4(EXIF_ASCII_LENGTH + sizeof(MMPS_DSC_EXIF_CONFIG) - 4);
    }
    else {
        pExif->AtomSize = ALIGN4(DataSize + sizeof(MMPS_DSC_EXIF_CONFIG) - 4);
    }

    pExif->DataSize      = DataSize;
    pExif->TagID         = uwTagID;
    pExif->IfdID         = uwIfdIdx;
    pExif->TagType       = uwType;
    pExif->TagDataSize   = uwCount;
    pExif->Data          = (MMP_UBYTE*)pExif + sizeof(MMPS_DSC_EXIF_CONFIG) - 4;

    MEMCPY(pExif->Data, pData, DataSize);

    if (!bUpdate) {
        pExif         	= (MMPS_DSC_EXIF_CONFIG*)((MMP_ULONG)pExif + pExif->AtomSize);        
        pExif->AtomSize	= 0;
    }

	return (error == MMP_ERR_NONE)? MMP_TRUE : MMP_FALSE;
}
#endif

#if 0
void _____Storage_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_GetFileNameAddr
//  Description :
//------------------------------------------------------------------------------
/** @brief The function get file name buffer address

The function get the file name buffer address and current dsc mode

@param[out] ulFileNameAddr Current file name buffer address
@param[out] usMode Current dsc mode
@return MMP_ERR_NONE
*/
MMP_ERR MMPS_DSC_GetFileNameAddr(MMP_ULONG *ulFileNameAddr)
{
    return MMPD_DSC_GetFileNameAddr(ulFileNameAddr);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_JpegDram2Card
//  Description :
//------------------------------------------------------------------------------
/** @brief The function saved Jpeg data from DRAM (JPG_EXT_MEM_ST) to card

The function saved Jpeg data from DRAM (JPG_EXT_MEM_ST) to card. It used for card mode extermal memory capture.

@param[in] pCaptureInfo the capture file name and filename length information.

@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_JpegDram2Card(MMPS_DSC_CAPTURE_INFO *pCaptureInfo)
{
	MMP_ERR 	ret = MMP_ERR_NONE;
	MMP_BOOL	bEnableExtraBuf;

  	if (MMP_DSC_StartStoreCardCallback != NULL) {
  		MMP_DSC_StartStoreCardCallback(pCaptureInfo);
  	}
	
	if (pCaptureInfo->bFirstShot) 
		m_MultiShotInfo.usStoreIndex = 0;
	else
		m_MultiShotInfo.usStoreIndex++;

	if (pCaptureInfo->ulExtraBufAddr != 0)
		bEnableExtraBuf = MMP_TRUE;
	else
		bEnableExtraBuf = MMP_FALSE;	
	
	MMPD_DSC_SetFileName(pCaptureInfo->ubFilename, pCaptureInfo->usFilenamelen);

	if (pCaptureInfo->bExif) {
	
		// Exif header
		ret = MMPD_DSC_JpegDram2Card(m_MultiShotInfo.ulExifHeaderStartAddr[m_MultiShotInfo.usStoreIndex], 
									 m_MultiShotInfo.ulExifHeaderSize[m_MultiShotInfo.usStoreIndex], 
									 MMP_TRUE, MMP_FALSE);
		
		if (ret != MMP_ERR_NONE) {
            RTNA_DBG_Str(0, "MMPD_DSC_JpegDram2Card ret:");
            RTNA_DBG_Long(0, ret);  RTNA_DBG_Long(0, __LINE__);
            RTNA_DBG_Str(0, "\r\n");
			ret = MMP_DSC_ERR_SAVE_CARD_FAIL;
			goto L_ret;
		}
			
		// Thumbnail
		if (pCaptureInfo->bThumbnail) {
			ret = MMPD_DSC_JpegDram2Card(m_MultiShotInfo.ulExifThumbnailAddr[m_MultiShotInfo.usStoreIndex],
										 m_MultiShotInfo.ulExifThumbnailSize[m_MultiShotInfo.usStoreIndex],
										 MMP_FALSE, MMP_FALSE);
		}
		
		if (ret == MMP_ERR_NONE) {

			#if (DSC_SUPPORT_BASELINE_MP_FILE)
			if (m_bMultiPicFormatEnable)
			{
				if (!pCaptureInfo->bThumbnail) {
					printc("Error! Thumbnail Should be Enabled\r\n");
				}

				ret = MMPD_DSC_JpegDram2Card(m_MultiShotInfo.ulMpfDataStartAddr[m_MultiShotInfo.usStoreIndex],
											 m_MultiShotInfo.ulMpfDataSize[m_MultiShotInfo.usStoreIndex],
											 MMP_FALSE, MMP_FALSE);

				// Primary JPEG, skip SOI marker
				ret =  MMPD_DSC_JpegDram2Card(m_MultiShotInfo.ulPrimaryJpegAddr[m_MultiShotInfo.usStoreIndex] + 2,
											  m_MultiShotInfo.ulPrimaryJpegSize[m_MultiShotInfo.usStoreIndex],
					 						  MMP_FALSE, MMP_FALSE);

				if (m_bEncLargeThumbEnable) {
				
					// Large Thumbnail with its Exif
					ret = MMPD_DSC_JpegDram2Card(m_MultiShotInfo.ulLargeThumbExifAddr[m_MultiShotInfo.usStoreIndex],
												 m_MultiShotInfo.ulLargeThumbExifSize[m_MultiShotInfo.usStoreIndex],
												 MMP_FALSE, MMP_FALSE);
					
					// Large Thumbnail, skip SOI marker
					ret = MMPD_DSC_JpegDram2Card(m_MultiShotInfo.ulLargeThumbJpegAddr[m_MultiShotInfo.usStoreIndex] + 2,
												 m_MultiShotInfo.ulLargeThumbJpegSize[m_MultiShotInfo.usStoreIndex],
												 MMP_FALSE, !bEnableExtraBuf);			
				}
			}
			else
			#endif
			{
				// Primary JPEG, skip SOI marker
				ret =  MMPD_DSC_JpegDram2Card(m_MultiShotInfo.ulPrimaryJpegAddr[m_MultiShotInfo.usStoreIndex] + 2,
											  m_MultiShotInfo.ulPrimaryJpegSize[m_MultiShotInfo.usStoreIndex],
					 						  MMP_FALSE, !bEnableExtraBuf);			
			}
		}
		else {
            RTNA_DBG_Str(0, "MMPD_DSC_JpegDram2Card ret:");
            RTNA_DBG_Long(0, ret);  RTNA_DBG_Long(0, __LINE__);
            RTNA_DBG_Str(0, "\r\n");
			ret = MMP_DSC_ERR_SAVE_CARD_FAIL;
			goto L_ret;
		}
	}
	else
	{
		// Primary JPEG
		ret =  MMPD_DSC_JpegDram2Card(m_MultiShotInfo.ulPrimaryJpegAddr[m_MultiShotInfo.usStoreIndex], 
									  m_MultiShotInfo.ulPrimaryJpegSize[m_MultiShotInfo.usStoreIndex], 
									  MMP_TRUE, !bEnableExtraBuf);
		
		if (ret != MMP_ERR_NONE) {
            RTNA_DBG_Str(0, "MMPD_DSC_JpegDram2Card ret:");
            RTNA_DBG_Long(0, ret);  RTNA_DBG_Long(0, __LINE__);
            RTNA_DBG_Str(0, "\r\n");
			ret = MMP_DSC_ERR_SAVE_CARD_FAIL;
			goto L_ret;
		}
	}	
	
	if (bEnableExtraBuf) {
		MMPD_DSC_JpegDram2Card(pCaptureInfo->ulExtraBufAddr, pCaptureInfo->ulExtraBufSize, MMP_FALSE, MMP_TRUE);
	}

L_ret:

  	if (MMP_DSC_EndStoreCardCallback != NULL) {
  		MMP_DSC_EndStoreCardCallback(pCaptureInfo);
  	}
	
    return ret;
}

#if (SUPPORT_VR_THUMBNAIL)
//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_JpegDram2Chunk
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_JpegDram2Chunk(void)
{
    return MMPD_3GPMGR_SetThumbnailBuf(
            m_MultiShotInfo.ulPrimaryJpegAddr[m_MultiShotInfo.usStoreIndex], 
            m_MultiShotInfo.ulPrimaryJpegSize[m_MultiShotInfo.usStoreIndex]);
}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_EncodeRaw2Jpeg
//  Description :
//------------------------------------------------------------------------------
/** @brief The function encode raw data to JPEG from DRAM

1. Call MMPS_DSC_SetSystemBuf set file name address
2. Call MMPS_DSC_SetCustomQTable set Q table use non-custom
3. Call MMPS_DSC_SetCaptureJpegQuality set jpeg quality
4. Call MMPS_DSC_EncodeRaw2Jpeg Encode Raw data to Jpeg and save in card

@param[in]  pJpegBufAttr the decode buffer information.
            pJpegBufAttr->usWidth   		raw data Width.
            pJpegBufAttr->usHeight  		raw data height. 
            pJpegBufAttr->usLineOffset 		raw data line offset.
            pJpegBufAttr->colordepth   		raw data colordepth.
            pJpegBufAttr->ulBaseAddr   		raw data start address.
@param[in]	pJpegInfo the encode jpeg Width and Height information.
			pJpegInfo->bJpegFileName		Jpeg encode file name.
			pJpegInfo->usJpegFileNameLength	Jpeg encode file name length.
            pJpegInfo->usWidth				Jpeg encode width size(must ALIGN16).
            pJpegInfo->usHeight				Jpeg encode height size(must ALIGN8).

@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_EncodeRaw2Jpeg(MMP_DSC_JPEG_INFO *pJpegInfo, MMP_GRAPHICS_BUF_ATTR *pSrcBufAttr)
{
    MMP_SCAL_FIT_RANGE		fitrange;
    MMP_SCAL_GRAB_CTRL     	grabctl;
    MMP_GRAPHICS_BUF_ATTR  	BufAttr;
	MMP_DSC_CAPTURE_BUF		capturebuf;
    MMP_GRAPHICS_RECT       rect;
	MMP_ULONG				ulJpegSize;
	MMP_ULONG				ulSramCurPos = 0x104000;
	MMPD_FCTL_ATTR  		fctlAttr;

	MMPD_System_GetSramEndAddr(&ulSramCurPos);

	/* Assign the graphics source buffer attribute */
    BufAttr.usWidth 			= pSrcBufAttr->usWidth;
    BufAttr.usHeight 			= pSrcBufAttr->usHeight;
    BufAttr.usLineOffset		= pSrcBufAttr->usLineOffset;
    BufAttr.colordepth 			= pSrcBufAttr->colordepth;
    BufAttr.ulBaseAddr 			= pSrcBufAttr->ulBaseAddr;

    rect.usLeft     			= 0;
    rect.usTop      			= 0;
    rect.usWidth    			= BufAttr.usWidth;
    rect.usHeight   			= BufAttr.usHeight;

	/* Assign the JPEG compress/line buffer  */
    capturebuf.ulCompressStart  = BufAttr.ulBaseAddr + ALIGN32(BufAttr.usWidth * 2) * BufAttr.usHeight;
    capturebuf.ulCompressEnd    = ((capturebuf.ulCompressStart + pJpegInfo->usPrimaryWidth * 2 * pJpegInfo->usPrimaryHeight)>>4)<<4;
	capturebuf.ulLineStart      = ulSramCurPos;
    MMPD_DSC_SetCaptureBuffers(&capturebuf);

	/* Config Scaler Module */
    fitrange.fitmode     	= MMP_SCAL_FITMODE_OUT;
    fitrange.scalerType		= MMP_SCAL_TYPE_SCALER;
    fitrange.ulInWidth   	= BufAttr.usWidth;
    fitrange.ulInHeight  	= BufAttr.usHeight;
    fitrange.ulOutWidth  	= pJpegInfo->usPrimaryWidth;
    fitrange.ulOutHeight 	= pJpegInfo->usPrimaryHeight;

    fitrange.ulInGrabX		= 1;
    fitrange.ulInGrabY		= 1;
    fitrange.ulInGrabW		= fitrange.ulInWidth;
    fitrange.ulInGrabH		= fitrange.ulInHeight;
    
    MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);
	
    fctlAttr.fctllink       = m_fctlLinkCapture;
    fctlAttr.fitrange       = fitrange;
    fctlAttr.grabctl        = grabctl;
    fctlAttr.scalsrc		= MMP_SCAL_SOURCE_GRA;
    fctlAttr.bSetScalerSrc	= MMP_TRUE;
	
    MMPD_DSC_RawBuf2Jpeg(&BufAttr, &rect, 1, &fctlAttr);
	MMPD_DSC_GetJpegSize(&ulJpegSize);
	   
	MMPD_DSC_SetFileName(pJpegInfo->bJpegFileName, pJpegInfo->usJpegFileNameLength);
	MMPD_DSC_JpegDram2Card(capturebuf.ulCompressStart, ulJpegSize, MMP_TRUE, MMP_TRUE);
	
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_GetRawData
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_DSC_GetRawData(MMP_UBYTE   ubSnrSel,
                            MMP_ULONG 	ulStoreAddr,	MMP_BOOL 	bStopPreviw,
							MMP_BYTE 	ubFilename[], 	MMP_USHORT 	usLength, 
							MMP_ULONG 	*ulWidth, 		MMP_ULONG 	*ulHeight)
{
    MMP_BOOL	bComplete = 0;
    MMP_ERR     error;
    MMP_UBYTE   ubVifId = MMPD_Sensor_GetVIFPad(ubSnrSel);
    
    if (m_DscPreviewPath == MMP_DSC_STD_PREVIEW) {
    	return MMP_DSC_ERR_PARAMETER;
    }
    
    if (bStopPreviw) {
    	MMPD_PTZ_SetDigitalZoom(m_fctlLinkPreview.scalerpath, MMP_PTZ_ZOOMSTOP, MMP_TRUE);
    		
    	while (!bComplete) {
            MMPD_PTZ_CheckZoomComplete(m_fctlLinkPreview.scalerpath, &bComplete);
        }
           
        MMPD_Fctl_EnablePreview(ubSnrSel, m_fctlLinkPreview.ibcpipeID, MMP_FALSE, MMP_TRUE);
          
        MMPD_RAWPROC_EnablePreview(ubVifId, MMP_FALSE);
        
    	MMPD_System_ResetHModule(MMPD_SYS_MDL_VIF0, MMP_FALSE);
	}	
		
	MMPD_DSC_SetFileName(ubFilename, usLength);
	
	error = MMPD_DSC_GetRawData(ubSnrSel, ulStoreAddr, m_DscRawColorDepth, ulWidth, ulHeight);
	
	if (bStopPreviw) 
	{
	    MMPD_RAWPROC_EnablePath(ubSnrSel, ubVifId, MMP_TRUE, MMP_RAW_IOPATH_VIF2RAW | MMP_RAW_IOPATH_RAW2ISP, m_DscRawColorDepth);

	    m_bDscPreviewActive = MMP_FALSE;
	}
	
	return error;
}

extern MMP_ERR MMPF_FT_EnableStream(MMP_UBYTE ubSnrSel, MMP_USHORT usPipe);
extern MMP_ERR MMPF_FT_GetFrameData(MMP_UBYTE ubSnrSel, MMP_USHORT usPipe);
//------------------------------------------------------------------------------
//  Function    : MMPS_FT_InitGetRawData
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_FT_InitGetRawData(MMP_UBYTE ubSnrSel,
                               MMP_UBYTE ubSenorMode,
							   MMP_UBYTE ubColorMode, 
							   MMP_ULONG *ulRawStoreAddr,
							   MMP_ULONG *ulRawWidth,
							   MMP_ULONG *ulRawHeight,
							   MMP_ULONG *ulRawStoreSize,
							   MMP_ULONG *ulIbcStoreAddr,
							   MMP_ULONG *ulIbcStoreWidth,
							   MMP_ULONG *ulIbcStoreHeight,
							   MMP_ULONG *ulIbcStoreSize)
{
	MMP_ERR 				error;
    MMP_SCAL_FIT_RANGE    	fitrange;
    MMP_SCAL_GRAB_CTRL 		grabctl;
	MMP_USHORT				usRawStoreW, usRawStoreH;
	MMP_USHORT				usScaInputW, usScaInputH;
	MMP_ULONG				ulSize;
	MMP_ULONG				ulCurBufAddr;
	MMP_RAW_STORE_BUF		rawPreviewBuf;
   	MMP_ICO_PIPE_ATTR 		icoAttr;
   	MMP_IBC_PIPE_ATTR 		ibcAttr;
   	MMP_ULONG				ulBaseAddr = 0, ulBaseUAddr = 0, ulBaseVAddr = 0;
   	MMP_UBYTE				ubPipeId = 0;
	MMP_UBYTE				ubVifId = MMPD_Sensor_GetVIFPad(ubSnrSel);
	
	/* Allocate sensor buffer and initial sensor */
	MMPD_System_GetFWEndAddr(&ulCurBufAddr);
	
	MMPD_Sensor_AllocateBuffer(ulCurBufAddr, &ulSize);
	ulCurBufAddr += ulSize;
	
	error = MMPS_Sensor_Initialize(0/*PRM_SENSOR*/, ubSenorMode, MMP_SNR_DSC_PRW_MODE);  

	/* Allocate Raw store buffer and IBC store buffer */
	MMPD_RAWPROC_GetStoreRange(ubVifId, &usRawStoreW, &usRawStoreH);
    MMPD_RAWPROC_CalcBufSize(MMP_RAW_COLORFMT_BAYER8, usRawStoreW, usRawStoreH, &ulSize);

    *ulRawWidth  = usRawStoreW;
    *ulRawHeight = usRawStoreH;
   
    rawPreviewBuf.ulRawBufCnt  		= 1;
    rawPreviewBuf.ulRawBufAddr[0] 	= ALIGN32(ulCurBufAddr);
    
	MMPD_RAWPROC_SetStoreBuf(ubVifId, &rawPreviewBuf);
	
	*ulRawStoreAddr = rawPreviewBuf.ulRawBufAddr[0];
	*ulRawStoreSize	= ulSize;
	
	ulCurBufAddr += ALIGN32(ulSize);
	
	/* Calculate Scaler setting */
	usScaInputW = usRawStoreW - 2;
	usScaInputH = usRawStoreH - 2;
	
    fitrange.fitmode		= MMP_SCAL_FITMODE_OUT;
    fitrange.scalerType		= MMP_SCAL_TYPE_SCALER;
    fitrange.ulInWidth		= usScaInputW;
    fitrange.ulInHeight		= usScaInputH;
    fitrange.ulOutWidth		= usScaInputW;
    fitrange.ulOutHeight	= usScaInputH;

	fitrange.ulInGrabX		= 1;
    fitrange.ulInGrabY		= 1;
    fitrange.ulInGrabW		= fitrange.ulInWidth;
    fitrange.ulInGrabH		= fitrange.ulInHeight;

	MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);

	*ulIbcStoreWidth 		= fitrange.ulOutWidth;
	*ulIbcStoreHeight 		= fitrange.ulOutHeight;

	/* Config IBC Module and allocate IBC store buffer */
    switch (ubColorMode) {       
    case MMP_DISPLAY_COLOR_YUV422:
		ibcAttr.colorformat = MMP_IBC_COLOR_YUV444_2_YUV422_UYVY;
        
        ulBaseAddr = ulCurBufAddr;
        ulCurBufAddr += ALIGN32(fitrange.ulOutWidth * fitrange.ulOutHeight * 2);
        
		*ulIbcStoreAddr = ulBaseAddr;
		*ulIbcStoreSize	= fitrange.ulOutWidth * fitrange.ulOutHeight * 2;
		break;		
    case MMP_DISPLAY_COLOR_YUV420:
		ibcAttr.colorformat = MMP_IBC_COLOR_I420;
        
        ulBaseAddr = ulCurBufAddr;
        ulCurBufAddr += ALIGN32(fitrange.ulOutWidth * fitrange.ulOutHeight);
        ulBaseUAddr = ulCurBufAddr;
        ulCurBufAddr += ALIGN32(fitrange.ulOutWidth * fitrange.ulOutHeight >> 2);
        ulBaseVAddr = ulCurBufAddr;
 
 		*ulIbcStoreAddr = ulBaseAddr;
		*ulIbcStoreSize	= fitrange.ulOutWidth * fitrange.ulOutHeight * 3 / 2;
        break;        
    case MMP_DISPLAY_COLOR_YUV420_INTERLEAVE:
		ibcAttr.colorformat = MMP_IBC_COLOR_NV12;

        ulBaseAddr = ulCurBufAddr;
        ulCurBufAddr += ALIGN32(fitrange.ulOutWidth * fitrange.ulOutHeight);
        ulBaseUAddr = ulCurBufAddr;
        ulCurBufAddr += ALIGN32(fitrange.ulOutWidth * fitrange.ulOutHeight >> 1);

 		*ulIbcStoreAddr = ulBaseAddr;
		*ulIbcStoreSize	= fitrange.ulOutWidth * fitrange.ulOutHeight * 3 / 2;
        break;
    }
	
	ibcAttr.ulBaseAddr 	    = ulBaseAddr;
	ibcAttr.ulBaseUAddr     = ulBaseUAddr;
	ibcAttr.ulBaseVAddr 	= ulBaseVAddr;
    ibcAttr.ulLineOffset 	= 0;
    ibcAttr.InputSource     = ubPipeId;
	ibcAttr.function 		= MMP_IBC_FX_TOFB;
	ibcAttr.bMirrorEnable   = MMP_FALSE;
	MMPD_IBC_SetAttributes(ubPipeId, &ibcAttr);
	
	PRINTF("IBC BufYAddr = %x, BufUAddr = %x, BufVAddr = %x\r\n",ulBaseAddr, ulBaseUAddr, ulBaseVAddr);
	
	/* Config Icon Module */
    icoAttr.inputsel 	= ubPipeId;
	icoAttr.bDlineEn 	= MMP_TRUE;
	icoAttr.usFrmWidth 	= grabctl.ulOutEdX - grabctl.ulOutStX + 1;
	MMPD_Icon_SetDLAttributes(ubPipeId, &icoAttr);
	MMPD_Icon_SetDLEnable(ubPipeId, MMP_TRUE);
	
	/* Config Scaler Module */
    MMPD_Scaler_SetOutColor(ubPipeId, MMP_SCAL_COLOR_YUV444);
  	#if (CCIR656_FORCE_SEL_BT601)
    MMPD_Scaler_SetOutColorTransform(ubPipeId, MMP_TRUE, MMP_SCAL_COLRMTX_FULLRANGE_TO_BT601);
    #else    
	MMPD_Scaler_SetOutColorTransform(ubPipeId, MMP_TRUE, MMP_SCAL_COLRMTX_YUV_FULLRANGE);
    #endif
    MMPD_Scaler_SetEngine(MMP_SCAL_USER_DEF_TYPE_IN_OUT, ubPipeId, &fitrange, &grabctl);
	MMPD_Scaler_SetLPF(ubPipeId, &fitrange, &grabctl);
	MMPD_Scaler_SetPath(ubPipeId, MMP_SCAL_SOURCE_ISP, MMP_TRUE);    
	MMPD_Scaler_SetEnable(ubPipeId, MMP_TRUE);
    
	/* Config Raw Module */
    MMPD_RAWPROC_SetFetchRange(0, 0, usRawStoreW, usRawStoreH, usRawStoreW);
    MMPD_RAWPROC_EnablePath(ubSnrSel, ubVifId, MMP_TRUE, MMP_RAW_IOPATH_VIF2RAW | MMP_RAW_IOPATH_RAW2ISP, MMP_RAW_COLORFMT_BAYER8);
    MMPD_RAWPROC_EnablePreview(ubVifId, MMP_TRUE);
	
	/* Config Prevew Flow */
	MMPD_System_EnableClock(MMPD_SYS_CLK_VIF, 		MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S0, 	MMP_TRUE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_F, 	MMP_TRUE);   
	MMPD_System_EnableClock(MMPD_SYS_CLK_ISP, 		MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, 	MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, 		MMP_TRUE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, 		MMP_TRUE);
	
	MMPF_FT_EnableStream(ubSnrSel, ubPipeId);
	
	return error;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_FT_EnableGetRawData
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_FT_EnableGetRawData(MMP_UBYTE ubSnrSel)
{	
	MMPF_FT_GetFrameData(ubSnrSel, 0);
	
	return MMP_ERR_NONE;
}
#endif //#if defined(ALL_FW)	

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_CustomedPlaybackAttr
//  Description :
//------------------------------------------------------------------------------
/** 
 @brief The function set the display window for DSC
 @param[in] bUserConfig		Use user defined preview configuration.
 @param[in] bRotate			Use DMA rotate to rotate preview buffer.
 @param[in] usBufWidth		Preview buffer width.
 @param[in] usBufHeight		Preview buffer height.
 @param[in] usStartX 		The X Offset of the display window.
 @param[in] usStartY 		The Y Offset of the display window.
 @param[in] usWinWidth  	The width of the display window.
 @param[in] usWinHeight 	The height of the display window.
@return It reports the status of the operation.
*/
static MMPS_DSC_AHC_PLAYBACK_INFO	m_sAhcDscPlayInfo;
MMP_ERR MMPS_DSC_CustomedPlaybackAttr(MMP_BOOL 	bUserConfig,
									  MMP_BOOL 	bRotate,
									  MMP_UBYTE ubRotateDir,
									  MMP_UBYTE	sFitMode,
									  MMP_USHORT usBufWidth, MMP_USHORT usBufHeight, 
									  MMP_USHORT usStartX, 	 MMP_USHORT usStartY,
                                      MMP_USHORT usWinWidth, MMP_USHORT usWinHeight)
{
    m_sAhcDscPlayInfo.bUserDefine 		= bUserConfig;
    m_sAhcDscPlayInfo.bRotate  			= bRotate;
    m_sAhcDscPlayInfo.sRotateDir		= ubRotateDir;
    m_sAhcDscPlayInfo.sFitMode			= sFitMode;
    m_sAhcDscPlayInfo.ulDecodeOutW		= usBufWidth;
    m_sAhcDscPlayInfo.ulDecodeOutH		= usBufHeight;
    m_sAhcDscPlayInfo.ulDispStartX   	= usStartX;
    m_sAhcDscPlayInfo.ulDispStartY   	= usStartY;
    m_sAhcDscPlayInfo.ulDispWidth    	= usWinWidth;
    m_sAhcDscPlayInfo.ulDispHeight		= usWinHeight;

    return MMP_ERR_NONE;
}

#if defined(MBOOT_FW)
/**@brief The dsc configuration

Use @ref MMPS_DSC_GetConfig to assign the field value of it.
You should read this functions for more information.
*/
static MMPS_DSC_CONFIG				m_dscConfig;

/**@brief The dsc decode mode.
         
Use @ref MMPS_DSC_SetPlaybackMode to set it.
It will be referred when accessing decode function.

For example :
	DSC_NORMAL_DECODE_MODE      0
	DSC_FULL_DECODE_MODE        1
	DSC_TV_NTSC_DECODE_MODE     2
	DSC_TV_PAL_DECODE_MODE      3
	DSC_HDMI_DECODE_MODE    	4
	DSC_CCIR_DECODE_MODE    	5
*/
static MMP_USHORT					m_usDecodeMode = 0;

/* For playback attribute */
static MMP_DISPLAY_CONTROLLER 		m_DscPlaybackDispCtrl;

/* Thumbnail / Preview / Capture / Playback pipe selection */
static  MMPD_FCTL_LINK 				m_fctlLinkPlayback; 	

//------------------------------------------------------------------------------
//  Function    : MMPS_DSC_PlaybackJpegEx
//  Description :
//------------------------------------------------------------------------------
/** @brief Playback JPEG file on the display device
@param[out] pJpegInfo the ipeg information
@retval MMP_ERR_NONE success
@return It reports the status of the operation.
*/
MMP_ERR MMPS_DSC_PlaybackJpegEx(MMP_DSC_JPEG_INFO           *pJpegInfo,
                                MMP_BOOL                    bMirror,
                                MMP_DISPLAY_COLORMODE      	DecodeColorFormat)
{
	MMP_ERR						retstatus;
    MMP_SCAL_FIT_RANGE       	fitrange;
    MMP_SCAL_GRAB_CTRL     		grabctl;
    MMP_SCAL_FIT_MODE  			sFitmode;
    MMP_GRAPHICS_BUF_ATTR  		BufAttr;
    MMP_DSC_DECODE_BUF 			decodebuf;

	static MMP_USHORT			usCurDecBufIdx = 0;
    MMP_ULONG					cur_buf;//, buf_num = 1;

	MMP_USHORT  				usPanelW;
    MMP_USHORT  				usPanelH;
    MMP_USHORT                  usWinOffsetX;
    MMP_USHORT					usWinOffsetY;
	MMP_USHORT                  usDecodedW;
	MMP_USHORT					usDecodedH;
    //MMP_DISPLAY_DEV_TYPE 		outputDev;
    //MMP_DISPLAY_WIN_ID			winID 		= m_DscPlaybackWinId;
	MMP_DISPLAY_ROTATE_TYPE    	RotateType 	= m_dscConfig.decParams.rotatetype[m_usDecodeMode];
    MMP_ULONG                   ulTmpBufSize = 0;

    MEMSET(&fitrange, 0, sizeof(MMP_SCAL_FIT_RANGE));    
	/* Disable display window */
	//MMPD_Display_GetOutputDev(m_DscPlaybackDispCtrl, &outputDev);
		
	//switch(outputDev){
	//	case MMP_DISPLAY_NONE:
	//		PRINTF("Output panel is not selected\r\n");
	//		return MMP_DSC_ERR_PLAYBACK_FAIL;
	//	case MMP_DISPLAY_RGB_LCD:
	//	case MMP_DISPLAY_TV:
	//	case MMP_DISPLAY_HDMI:
	//		//MMPD_Display_SetWinActive(winID, MMP_FALSE);
	//		break;
	//}
	
	/* Allocate filename buffer, decompressed buffer, line buffer */
	//MMPS_DSC_SetSystemBuf(NULL, MMP_FALSE, MMP_TRUE, MMP_TRUE);
	  	
	cur_buf = pJpegInfo->ulJpegBufAddr;
	
	decodebuf.ulDecompressStart = ALIGN256(cur_buf);
    decodebuf.ulDecompressEnd 	= decodebuf.ulDecompressStart + pJpegInfo->ulJpegBufSize;
    								
    decodebuf.ulLineBufStart 	= ALIGN256(decodebuf.ulDecompressEnd);
    //In decoding, JPEG needs[Horizontal pixels * Y-vert * 8 * 3] * Dual_Buffer Bytes
    decodebuf.ulLineBufSize 	= (pJpegInfo->usPrimaryWidth * 3 * 8 * 3) * 2;
    
	decodebuf.ulStorageTmpStart = decodebuf.ulLineBufStart + decodebuf.ulLineBufSize;
    if (DecodeColorFormat == MMP_DISPLAY_COLOR_RGB888) {
        decodebuf.ulStorageTmpSize 	= (pJpegInfo->usPrimaryWidth * pJpegInfo->usPrimaryHeight) * 3;
    }
    else {
        decodebuf.ulStorageTmpSize 	= ((pJpegInfo->usPrimaryWidth * pJpegInfo->usPrimaryHeight) * 3)>>1;        
    }
#if 0
    RTNA_DBG_Str(0, FG_YELLOW("CompressBufSt:"));
    RTNA_DBG_Long(0, decodebuf.ulDecompressStart);
    RTNA_DBG_Str(0, "\r\n");            
    RTNA_DBG_Str(0, FG_YELLOW("LineBufSt:"));            
    RTNA_DBG_Long(0, decodebuf.ulLineBufStart);            
    RTNA_DBG_Str(0, "\r\n");
    RTNA_DBG_Str(0, FG_YELLOW("DecBufSt:"));            
    RTNA_DBG_Long(0, decodebuf.ulStorageTmpStart);            
    RTNA_DBG_Str(0, "\r\n");    
#endif    
	cur_buf = ALIGN256(decodebuf.ulStorageTmpStart);
   
	MMPD_DSC_SetDecodeBuffers(&decodebuf);
	
	/* Get JPEG information */
    //if (MMPD_DSC_OpenJpegFile(pJpegInfo) != MMP_ERR_NONE) {
    //    return MMP_DSC_ERR_JPEGINFO_FAIL;
    //}

    //if (MMPD_DSC_GetJpegInfo(pJpegInfo) != MMP_ERR_NONE) {
    //    MMPD_DSC_CloseJpegFile(pJpegInfo);
    //    MMPS_DSC_SetSystemBuf(NULL, MMP_FALSE, MMP_TRUE, MMP_FALSE);
	//	return MMP_DSC_ERR_JPEGINFO_FAIL;
    //}

    //if (pJpegInfo->jpgFormat == MMP_DSC_JPEG_FMT_NOT_BASELINE) {
    //    MMPD_DSC_CloseJpegFile(pJpegInfo);
    //    MMPS_DSC_SetSystemBuf(NULL, MMP_FALSE, MMP_TRUE, MMP_FALSE);
	//	return MMP_DSC_ERR_JPEGINFO_FAIL;
    //}
 
	/* Initial Decode Output Resolution */
	//if (m_sAhcDscPlayInfo.bUserDefine) 
	{
	
		RotateType 		= m_sAhcDscPlayInfo.sRotateDir;
		sFitmode		= m_sAhcDscPlayInfo.sFitMode;
	    
	    usWinOffsetX 	= m_sAhcDscPlayInfo.ulDispStartX;
	    usWinOffsetY 	= m_sAhcDscPlayInfo.ulDispStartY;
    	    
	    usDecodedW 		= m_sAhcDscPlayInfo.ulDecodeOutW;
	    usDecodedH 		= m_sAhcDscPlayInfo.ulDecodeOutH;
	}	
	//else {
	//
	//	RotateType 		= m_dscConfig.decParams.rotatetype[m_usDecodeMode];
	//	sFitmode		= m_dscConfig.decParams.fitmode[m_usDecodeMode];
	    
	//    usWinOffsetX 	= 0;
	//    usWinOffsetY 	= 0;
 		    
    //    usDecodedW 		= m_dscConfig.decParams.usDecodeOutW[m_usDecodeMode];
    //    usDecodedH 		= m_dscConfig.decParams.usDecodeOutH[m_usDecodeMode];	    
	//}
    
  	/* Adjust display window resolution */
	MMPD_Display_GetWidthHeight(m_DscPlaybackDispCtrl, &usPanelW, &usPanelH);

	fitrange.ulInWidth  	= pJpegInfo->usPrimaryWidth;
	fitrange.ulInHeight 	= pJpegInfo->usPrimaryHeight;
	fitrange.ulOutWidth  	= usPanelW;
	fitrange.ulOutHeight	= usPanelH;

 	if ((RotateType == MMP_DISPLAY_ROTATE_RIGHT_90) || 
	    (RotateType == MMP_DISPLAY_ROTATE_RIGHT_270)) {

		fitrange.ulOutWidth  = usPanelH;
		fitrange.ulOutHeight = usPanelW;
	}

	/* Adjust output range when Jpeg size is smaller than display range */
	if (fitrange.ulOutWidth > fitrange.ulInWidth) {
		fitrange.ulOutWidth = fitrange.ulInWidth;	
	}
	if (fitrange.ulOutHeight > fitrange.ulInHeight) {
		fitrange.ulOutHeight = fitrange.ulInHeight;
	}
        
	/* Adjust decode output resolution */
    if ((pJpegInfo->usPrimaryWidth * pJpegInfo->usPrimaryHeight) < (usDecodedW * usDecodedH)) {
        usDecodedW = pJpegInfo->usPrimaryWidth;
        usDecodedH = pJpegInfo->usPrimaryHeight;
    }
    else {
//      #if 0
//      MMP_ULONG ulSuggestW,ulSuggestH;
//
//      MMPS_Display_AdjustScaleInSize(m_DscPlaybackDispCtrl,  
//                                     fitrange.ulInWidth, 
//                                     fitrange.ulInHeight, 
//                                     fitrange.ulOutWidth, 
//                                     fitrange.ulOutHeight, 
//                                     &ulSuggestW, &ulSuggestH);
//	   			
//		usDecodedW = (MMP_USHORT)ulSuggestW;
//		usDecodedH = (MMP_USHORT)ulSuggestH;       
//      #else
		MMP_USHORT	usGCD = 0;
    	MMP_ULONG	ulRatioH, ulRatioV;
		
		usGCD = Greatest_Common_Divisor(pJpegInfo->usPrimaryWidth, pJpegInfo->usPrimaryHeight);
	   			
		ulRatioH = pJpegInfo->usPrimaryWidth / usGCD;
		ulRatioV = pJpegInfo->usPrimaryHeight / usGCD;
		
		usDecodedW = (usDecodedW / ulRatioH) * ulRatioH;
		usDecodedH = (usDecodedW * ulRatioV) / ulRatioH;
//    	#endif
    }
         
	/* Check the picture is crossed panel boundary or not */
// 	if ((RotateType == MMP_DISPLAY_ROTATE_NO_ROTATE) || 
//	    (RotateType == MMP_DISPLAY_ROTATE_RIGHT_180)) {
//	    
//		if (usWinOffsetX + usDecodedW > usPanelW) {
//			PRINTF("DSC_PlaybackJpeg Fail: usWinOffsetX + usDecodedW > usPanelW (%d + %d > %d)\r\n",
//					usWinOffsetX, usDecodedW, usPanelW);
//							
//			if (usWinOffsetY + usDecodedH > usPanelH) {
//				PRINTF("DSC_PlaybackJpeg Fail: usWinOffsetY + usDecodedH > usPanelH (%d + %d > %d)\r\n", 
//					    usWinOffsetY, usDecodedH, usPanelH);
//				}
//				
//				//MMPD_DSC_CloseJpegFile(pJpegInfo);
//				return MMP_DSC_ERR_PLAYBACK_FAIL;
//			}			
//		}
//	else {
//	
//		if (usWinOffsetX + usDecodedH > usPanelW) {
//			PRINTF("DSC_PlaybackJpeg Fail: usWinOffsetX + usDecodedH > usPanelW (%d + %d > %d)\r\n",
//					usWinOffsetX, usDecodedH, usPanelW);
//			
//			if (usWinOffsetY + usDecodedW > usPanelH) {
//				PRINTF("DSC_PlaybackJpeg Fail: usWinOffsetY + usDecodedW > usPanelH (%d + %d > %d)\r\n", 
//					usWinOffsetY, usDecodedW, usPanelH);
//			}
//			
//			//MMPD_DSC_CloseJpegFile(pJpegInfo);
//			return MMP_DSC_ERR_PLAYBACK_FAIL;
//		}
//	}

	/* Allocate decode output buffer (window buffer) */
	switch (DecodeColorFormat) {
	case MMP_DISPLAY_COLOR_RGB565:
	case MMP_DISPLAY_COLOR_YUV422:
		ulTmpBufSize = ALIGN32(usDecodedW * usDecodedH * 2);
	    //if (usCurDecBufIdx && (m_usDecodeMode != DSC_CCIR_DECODE_MODE)) {
	    //    cur_buf += usCurDecBufIdx * ulTmpBufSize;
        //}
			
        BufAttr.ulBaseAddr = cur_buf;
		cur_buf += ulTmpBufSize;
		break;	
	case MMP_DISPLAY_COLOR_RGB888:
		ulTmpBufSize = ALIGN32(usDecodedW * usDecodedH * 3);
	    //if (usCurDecBufIdx && (m_usDecodeMode != DSC_CCIR_DECODE_MODE)) {
	    //    cur_buf += usCurDecBufIdx * ulTmpBufSize;
        //}
		BufAttr.ulBaseAddr = cur_buf;
		cur_buf += ulTmpBufSize;
		break;	
	case MMP_DISPLAY_COLOR_YUV420:
		ulTmpBufSize = usDecodedW * usDecodedH;
	    //if (usCurDecBufIdx && (m_usDecodeMode != DSC_CCIR_DECODE_MODE)) {
	    //    cur_buf += usCurDecBufIdx * ((ulTmpBufSize * 3) / 2);
        //}
        
		BufAttr.ulBaseAddr = cur_buf;
		BufAttr.ulBaseUAddr	= BufAttr.ulBaseAddr + ALIGN32(ulTmpBufSize);		
		BufAttr.ulBaseVAddr = BufAttr.ulBaseUAddr + ALIGN32(ulTmpBufSize >> 2);
		cur_buf += (ALIGN32(ulTmpBufSize) + ALIGN32(ulTmpBufSize>>2)*2);
		break;	
	case MMP_DISPLAY_COLOR_YUV420_INTERLEAVE:
		ulTmpBufSize = usDecodedW * usDecodedH;
	    //if (usCurDecBufIdx && (m_usDecodeMode != DSC_CCIR_DECODE_MODE)) {
	    //    cur_buf += usCurDecBufIdx * ((ulTmpBufSize * 3) / 2);
        //}
        
		BufAttr.ulBaseAddr = cur_buf;
		BufAttr.ulBaseUAddr	= BufAttr.ulBaseAddr + ALIGN32(ulTmpBufSize);
		BufAttr.ulBaseVAddr = BufAttr.ulBaseUAddr + ALIGN32(ulTmpBufSize >> 1);
		cur_buf += (ALIGN32(ulTmpBufSize) + ALIGN32(ulTmpBufSize>>1));
		break;	
	}

    #if 0
    if (cur_buf > MMPS_System_GetMemHeapEnd()) {
        printc("\t= [HeapMemErr] MMPS_DSC_PlaybackJpeg =\r\n");
        return MMP_DSC_ERR_MEM_EXHAUSTED;
    }
    #endif
	/* Calculate the grab range of decode out image */
	fitrange.fitmode    	= sFitmode;
	fitrange.scalerType 	= MMP_SCAL_TYPE_SCALER;
    fitrange.ulFitResol 	= 64;
    fitrange.ulInWidth  	= pJpegInfo->usPrimaryWidth;
    fitrange.ulInHeight 	= pJpegInfo->usPrimaryHeight; 
	fitrange.ulOutWidth  	= usDecodedW;
	fitrange.ulOutHeight 	= usDecodedH;

    fitrange.ulInGrabX 		= 1;
    fitrange.ulInGrabY 		= 1;
    fitrange.ulInGrabW 		= fitrange.ulInWidth;
    fitrange.ulInGrabH 		= fitrange.ulInHeight; 

	if ((fitrange.ulOutWidth > fitrange.ulInWidth) && 
		(fitrange.ulOutHeight > fitrange.ulInHeight)) {
	
		fitrange.ulOutWidth = fitrange.ulInWidth;	
		fitrange.ulOutHeight = fitrange.ulInHeight;
	}
	
    MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);
	
    /* Decode Jpeg to IBC buffer */
    retstatus = MMPD_DSC_DecodeJpegPic(	DecodeColorFormat, 
    									pJpegInfo, 
    									&BufAttr, 
    									&fitrange,
    									&grabctl, 
    									m_fctlLinkPlayback.scalerpath);
	
	//MMPD_DSC_CloseJpegFile(pJpegInfo);
	        
    if (retstatus != MMP_ERR_NONE) {
        return retstatus;
    }
    pJpegInfo->ulJpegBufAddr = BufAttr.ulBaseAddr;	
	//MMPD_DSC_SetFileNameAddr(0);
	
	/* Fill the m_JpegPlayInfo structure */
	//m_JpegPlayInfo.usJpegWidth          = pJpegInfo->usPrimaryWidth;
    //m_JpegPlayInfo.usJpegHeight         = pJpegInfo->usPrimaryHeight;
    //m_JpegPlayInfo.usDecodeWidth        = usDecodedW;
    //m_JpegPlayInfo.usDecodeHeight       = usDecodedH;
    //m_JpegPlayInfo.usDisplayWidth       = usDecodedW; //Not Used
    //m_JpegPlayInfo.usDisplayHeight      = usDecodedH; //Not Used
	//m_JpegPlayInfo.jpeginfo             = *pJpegInfo;
	//m_JpegPlayInfo.bufAttr         		= BufAttr;
	//m_JpegPlayInfo.sFitMode				= sFitmode;
	//m_JpegPlayInfo.rotateType           = RotateType;
	//m_JpegPlayInfo.DecodeColorFmt       = DecodeColorFormat;
	//m_JpegPlayInfo.bMirror              = bMirror;
	
	/* The setting of window and display */
	//MMPS_DSC_SetJpegDisplay(0, 0, pJpegInfo->usPrimaryWidth, pJpegInfo->usPrimaryHeight);

    //usCurDecBufIdx = (usCurDecBufIdx + 1) % buf_num;
        
    return MMP_ERR_NONE;
}
#endif //#if defined(MBOOT_FW)
/// @}

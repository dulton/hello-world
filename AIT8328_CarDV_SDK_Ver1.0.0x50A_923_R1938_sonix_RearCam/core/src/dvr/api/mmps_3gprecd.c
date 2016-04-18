/**
 @file mmps_3gprecd.c
 @brief 3GP Recorder Control Function
 @author Will Tseng
 @version 1.0
*/

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================
//#include "customer_config.h"

#include "mmp_lib.h"
#include "ait_utility.h"
#include "hdr_cfg.h"
#include "ptz_cfg.h"
#include "mmp_icon_inc.h"
#include "mmp_ldc_inc.h"
#include "mmps_iva.h"
#include "mmps_system.h"
#include "mmps_3gprecd.h"
#include "mmps_dsc.h"
#include "mmps_sensor.h"
#include "mmps_audio.h"
#include "mmps_fs.h"
#include "mmpd_system.h"
#include "mmpd_fctl.h"
#include "mmpd_dma.h"
#include "mmpd_usb.h"
#include "mmpd_ptz.h"
#include "mmpd_scaler.h"
#include "mmpd_rawproc.h"
#include "mmpf_rawproc.h"
#include "mmpd_bayerscaler.h"
#include "mmpd_mp4venc.h"
#include "mmpf_sensor.h"
#include "mmpf_mp4venc.h"
#include "mmpf_3gpmgr.h"
#include "mmpd_3gpmgr.h"
#include "mmpf_avimux.h"
#include "mmpf_audio_ctl.h"
#include "mmpf_mci.h"
#if (SUPPORT_LDC_RECD)
#include "mmpf_ldc.h"
#endif
#include "usb_cfg.h"
#include "mmpf_monitor.h"
#include "mmpf_adas_ctl.h"
#include "mmph_hif.h"
#include "mmpf_display.h"
#include <stdio.h>
/** @addtogroup MMPS_3GPRECD
@{
*/

//==============================================================================
//
//                              CONSTANT
//
//==============================================================================

#define INVALID_ENC_ID      (0xFFFFFFFF)

//==============================================================================
//
//                              GLOBAL VARIABLES
//
//==============================================================================

/// Video encode system mode
static MMPS_3GPRECD_MODES       m_VidRecdModes = {
    0,                              // usVideoPreviewMode
    {0, 0},                         // usVideoEncResIdx
    {{0, 0},{0, 0}},                // SnrInputFrameRate
    {{0, 0},{0, 0}},                // VideoEncFrameRate
    {{0, 0},{0, 0}},                // ContainerFrameRate
    {0, 0},                         // usPFrameCount
    {0, 0},                         // usBFrameCount
    {1048576, 0},                   // ulBitrate
    0,                              // ulAudBitrate
    0xFFFFFFFF,                     // ulSizeLimit
    0xFFFFFFFF,                     // ulTimeLimitMs
    0,                              // ulReservedSpace
    {MMPS_3GPRECD_SRCMODE_CARD, MMPS_3GPRECD_SRCMODE_CARD},     // VideoSrcMode
    {MMPS_H264ENC_HIGH_PROFILE, MMPS_H264ENC_HIGH_PROFILE},     // VisualProfile
#if (DUALENC_SUPPORT)	
    {MMPS_3GPRECD_CURBUF_FRAME,    MMPS_3GPRECD_CURBUF_FRAME},        // VidCurBufMode
#else
	{MMPS_3GPRECD_CURBUF_RT,    MMPS_3GPRECD_CURBUF_RT},        // VidCurBufMode
#endif    
    MMP_FALSE,                      // bSlowMotionEn;
};

/// Video context handle
MMP_ULONG                       m_VidRecdID = INVALID_ENC_ID;
#if (DUALENC_SUPPORT)
MMP_ULONG                       m_VidDualID = INVALID_ENC_ID;
#endif
#if (SUPPORT_VR_WIFI_STREAM)
static MMPS_WIFISTREAM_OBJ 	    m_sWifiStreamObj[MAX_WIFI_STREAM_NUM];
MMP_ULONG                       m_VidWifiFrontID = INVALID_ENC_ID;
MMP_ULONG                       m_VidWifiRearID  = INVALID_ENC_ID;
#endif

/// Video encode config mode
static MMPS_3GPRECD_CONFIGS     m_VidRecdConfigs;

/// Status of video preview/Encode
MMP_BOOL                        m_bVidPreviewActive[VR_MAX_PREVIEW_NUM] = {MMP_FALSE, MMP_FALSE};
MMP_BOOL                        m_bVidRecordActive[VR_MAX_ENCODE_NUM]  = {MMP_FALSE, MMP_FALSE};

/// Parameters of preview/record zoom
static MMPS_DSC_ZOOM_INFO       m_VidPreviewZoomInfo;
static MMPS_DSC_ZOOM_INFO       m_VidRecordZoomInfo;
static MMP_USHORT               m_usVidStaticZoomIndex  = 0;

/// End of video preview memory
static MMP_ULONG                m_ulVideoPreviewEndAddr = 0;

/// Container format
static MMPS_3GPRECD_CONTAINER   m_VidRecdContainer 		= MMPS_3GPRECD_CONTAINER_UNKNOWN;

/// Parameters of video input buffers
static MMPD_MP4VENC_INPUT_BUF   m_VidRecdInputBuf[VR_MAX_ENCODE_NUM];

/// AHC parameters
static MMPS_3GPRECD_AHC_PREVIEW_INFO 	m_sAhcVideoPrevInfo[VR_MAX_PREVIEW_NUM] = {{0},{0}};
static MMPS_3GPRECD_AHC_VIDEO_INFO		m_sAhcVideoRecdInfo[VR_MAX_ENCODE_NUM] = {{0},{0}};

/// For Decode MJPEG to Preview attribute
#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
static MMPS_3GPRECD_AHC_PREVIEW_INFO    m_sAhcDecMjpegToPreviewInfo;
static MMP_DISPLAY_WIN_ID               m_sDecMjpegToPreviewWinId   = MMP_DISPLAY_WIN_OVERLAY;
static MMP_USHORT                       m_usDecMjpegToPreviewSrcW   = 1280;
static MMP_USHORT                       m_usDecMjpegToPreviewSrcH   = 720;
static MMPS_3GPRECD_PREVIEW_BUFINFO     m_sDecMjpegPrevwBufInfo;
#endif

/// For Decode MJPEG to Encode attribute
#if (SUPPORT_DEC_MJPEG_TO_ENC_H264)
static MMPS_3GPRECD_AHC_VIDEO_INFO		m_sAhcDecMjpegToEncodeInfo 	= {0};
static MMP_USHORT                       m_usAhcDecMjpegToEncodeSrcW = 1280;
static MMP_USHORT                       m_usAhcDecMjpegToEncodeSrcH = 720;
#endif

static MMP_BOOL                 		m_bAhcConfigVideoRZoom  = MMP_FALSE;

static MMP_ULONG                m_ulVidRecRefGenBufAddr = 0;
static MMP_ULONG                m_ulVidRecEncodeAddr    = 0;
static MMP_ULONG                m_ulVidRecSramAddr      = 0;

static MMP_ULONG                m_ulVidRecCaptureSramAddr = 0;
static MMP_ULONG                m_ulVidRecCaptureDramAddr = 0;

static MMP_ULONG                m_ulVidRecMjpgStreamSramAddr = 0;
static MMP_ULONG                m_ulVidRecMjpgStreamDramAddr = 0;
#if (HANDLE_JPEG_EVENT_BY_QUEUE)
static MMP_ULONG                m_ulVidRecCaptureFrmBufAddr = 0;
#if (!USE_H264_CUR_BUF_AS_CAPT_BUF)
static MMP_ULONG                m_ulVidRecCaptureFrmBufSize = 0;
#endif
static MMP_USHORT               m_usMJPEGMaxEncWidth        = 640;
static MMP_USHORT               m_usMJPEGMaxEncHeight       = 480;
#endif

static MMP_ULONG                m_ulVidRecDualCaptureDramAddr = 0;
#if (SUPPORT_USB_HOST_FUNC)
static MMP_ULONG                m_ulVidRecDualCaptureDramSize = ISO_MJPEG_MAX_VIDEO_FRM_SIZE;
#else
static MMP_ULONG                m_ulVidRecDualCaptureDramSize = 512*1024;
#endif

static MMP_BOOL                 m_bSeamlessEnabled 		= MMP_FALSE;

static MMP_ULONG                m_ulVidShareMvAddr 	    = 0;
static MMP_ULONG                m_ulVidRecDramEndAddr 	= 0;
#if (SUPPORT_VR_WIFI_STREAM)
static MMP_ULONG                m_ulVidWifiStreamStartAddr  = 0;
#endif

static MMP_UBYTE                m_ubVidRecModeSnrId     = PRM_SENSOR;
#if (DUALENC_SUPPORT)
static MMP_UBYTE                m_ub2ndH264SnrId        = PRM_SENSOR;
#endif

#if (SUPPORT_UVC_FUNC)
MMP_BOOL                        gbVideoEncQualityCustom = MMP_FALSE;
MMP_UBYTE                       gbCustomQp              = 51;
MMP_ULONG                       glCustomTargetSizePerFrame = 4096;
#endif

static MMPD_FCTL_LINK			m_RecordFctlLink 	    = {MMP_SCAL_PIPE_0, MMP_ICO_PIPE_0, MMP_IBC_PIPE_0};
#if (!SUPPORT_DUAL_SNR)&&(!DUALENC_SUPPORT)&&(!SUPPORT_DEC_MJPEG_TO_ENC_H264)
static MMPD_FCTL_LINK			m_PreviewFctlLink 	    = {MMP_SCAL_PIPE_1, MMP_ICO_PIPE_1, MMP_IBC_PIPE_1};
#else
static MMPD_FCTL_LINK			m_PreviewFctlLink 	    = {MMP_SCAL_PIPE_3, MMP_ICO_PIPE_3, MMP_IBC_PIPE_3};
#endif
#if (SUPPORT_DUAL_SNR)||(DUALENC_SUPPORT)
static MMPD_FCTL_LINK			m_2ndRecFctlLink        = {MMP_SCAL_PIPE_1, MMP_ICO_PIPE_1, MMP_IBC_PIPE_1};
#endif
#if (SUPPORT_DEC_MJPEG_TO_ENC_H264)
static MMPD_FCTL_LINK			m_DecMjpegToEncFctlLink = {MMP_SCAL_PIPE_1, MMP_ICO_PIPE_1, MMP_IBC_PIPE_1};
#endif
#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
static MMPD_FCTL_LINK 		    m_DecMjpegToPrevwFctlLink = {MMP_SCAL_PIPE_2, MMP_ICO_PIPE_2, MMP_IBC_PIPE_2};
#endif
#if (SUPPORT_DUAL_SNR)
static MMPD_FCTL_LINK			m_2ndPrewFctlLink       = {MMP_SCAL_PIPE_2, MMP_ICO_PIPE_2, MMP_IBC_PIPE_2};
#endif
static MMPD_FCTL_LINK			m_StreamingFctlLink     = {MMP_SCAL_PIPE_2, MMP_ICO_PIPE_2, MMP_IBC_PIPE_2};
#if (SUPPORT_LDC_RECD)
static MMPD_FCTL_LINK			m_LdcSrcFctlLink 	    = {MMP_SCAL_PIPE_3, MMP_ICO_PIPE_3, MMP_IBC_PIPE_3};
#endif
#if (SUPPORT_MDTC)||(SUPPORT_ADAS)
static MMPD_FCTL_LINK			m_MdtcFctlLink 		    = {MMP_SCAL_PIPE_4, MMP_ICO_PIPE_4, MMP_IBC_PIPE_4};
#endif

#if (SUPPORT_LDC_RECD)
static MMP_ULONG				m_ulLdcMaxSrcWidth 	    = 1920;
static MMP_ULONG				m_ulLdcMaxSrcHeight     = 1080;
static MMP_ULONG				m_ulLdcMaxOutWidth 	    = 1920;
static MMP_ULONG				m_ulLdcMaxOutHeight     = 1080;
#endif

#if (SUPPORT_VR_THUMBNAIL)
static MMP_ULONG  				m_ulVRThumbWidth  	    = 416;
static MMP_ULONG  				m_ulVRThumbHeight 	    = 240;
#endif

static MMP_USHORT               m_usMaxStillJpegW       = VR_MAX_CAPTURE_WIDTH;
static MMP_USHORT               m_usMaxStillJpegH       = VR_MAX_CAPTURE_HEIGHT;

#if (SUPPORT_MDTC)||(SUPPORT_ADAS)
static MMPS_3GPRECD_Y_FRAME_TYPE m_YFrameType           = MMPS_3GPRECD_Y_FRAME_TYPE_NONE;
MMP_SCAL_FIT_RANGE gsADASFitRange = {0};
MMP_SCAL_GRAB_CTRL gsADASGrabctl = {0};
MMPD_FCTL_ATTR     m_ADASFctlAttr = {0};
#endif

static MMP_ULONG 				m_ulVRPreviewW[VR_MAX_PREVIEW_NUM] 	= {0, 0};
static MMP_ULONG	 			m_ulVRPreviewH[VR_MAX_PREVIEW_NUM]	= {0, 0};
static MMP_ULONG 				m_ulVREncodeW[VR_MAX_ENCODE_NUM] 	= {0, 0};
static MMP_ULONG 				m_ulVREncodeH[VR_MAX_ENCODE_NUM]    = {0, 0};
static MMP_ULONG 				m_ulVREncodeBufW[VR_MAX_ENCODE_NUM] = {0, 0};
static MMP_ULONG 				m_ulVREncodeBufH[VR_MAX_ENCODE_NUM] = {0, 0};
#if (SUPPORT_VR_WIFI_STREAM)
static MMP_ULONG 				m_ulWifiStreamW[MAX_WIFI_STREAM_NUM]    = {0};
static MMP_ULONG 				m_ulWifiStreamH[MAX_WIFI_STREAM_NUM]    = {0};
static MMP_ULONG 				m_ulWifiStreamBufW[MAX_WIFI_STREAM_NUM] = {0};
static MMP_ULONG 				m_ulWifiStreamBufH[MAX_WIFI_STREAM_NUM] = {0};
#endif

MMPD_FCTL_ATTR  		        m_VRPreviewFctlAttr[VR_MAX_PREVIEW_NUM];
#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
MMPD_FCTL_ATTR  		        m_DecMjpegToPrevwFctlAttr;
#endif
static MMP_BOOL                 m_bInitRecPipeConfig = MMP_FALSE;

#if (DUALENC_SUPPORT)
MMP_3GPRECD_FILETYPE gEncFileType[2] = {MMP_3GPRECD_FILETYPE_MAX, MMP_3GPRECD_FILETYPE_MAX};
MMP_ULONG glEncID[2] = {INVALID_ENC_ID, INVALID_ENC_ID};
static MMP_UBYTE    gbTotalEncNum = 0;   
#endif
//==============================================================================
//
//                              EXTERN VARIABLE
//
//==============================================================================

extern MMP_BOOL 	m_bHdmiInterlace;
#if (UVC_VIDRECD_SUPPORT)
extern MMP_BOOL     gbUVCRecdSupport;
#endif
extern MMP_BOOL     gubMmpDbgBk;
//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

static MMP_ERR MMPS_3GPRECD_SetPreviewMemory(MMP_UBYTE ubSnrSel, MMP_USHORT usPreviewW, MMP_USHORT usPreviewH, MMP_ULONG *ulStackAddr, MMP_ULONG *ulFbAddr);
MMP_ERR MMPS_3GPRECD_SetH264MemoryMap(MMP_ULONG *ulEncId, MMP_USHORT usEncW, MMP_USHORT usEncH, MMP_ULONG ulFBufAddr, MMP_ULONG ulStackAddr);
MMP_ERR MMPS_3GPRECD_InitDecMjpegToEncode(MMPD_MP4VENC_INPUT_BUF *pInputBuf, MMP_ULONG ubEncId);
#if (DUALENC_SUPPORT)
MMP_ERR MMPS_3GPRECD_SetDualH264MemoryMap(MMP_ULONG *ulEncId, MMP_USHORT usEncW, MMP_USHORT usEncH, MMP_ULONG ulFBufAddr, MMP_ULONG ulStackAddr);
#endif
MMP_ERR MMPS_3GPRECD_StopAllPipeZoom(void);

static MMP_ERR MMPS_3GPRECD_InitDigitalZoomParam(MMP_UBYTE ubPipe);
static MMP_ERR MMPS_3GPRECD_RestoreDigitalZoomRange(MMP_UBYTE ubPipe);

static MMP_ULONG MMPS_3GPRECD_TargetFrmSize(MMP_ULONG ulEncId);
static MMP_ERR MMPS_3GPRECD_SetEncodeRes(MMP_UBYTE ubEncIdx);

#if (SUPPORT_DUAL_SNR)
static MMP_ERR MMPS_3GPRECD_Set2ndSnrPreviewMemory( MMP_UBYTE   ubSnrSel,
                                                    MMP_USHORT  usPreviewW,  
                                                    MMP_USHORT  usPreviewH, 
									  		        MMP_ULONG   *ulStackAddr);
MMP_ERR MMPS_3GPRECD_Enable2ndSnrPreviewPipe(MMP_UBYTE ubSnrSel, MMP_BOOL bEnable, MMP_BOOL bCheckFrameEnd);
#endif

#if (HANDLE_JPEG_EVENT_BY_QUEUE)
extern MMP_ERR MMPS_DSC_CaptureByQueue(MMP_IBC_PIPEID 			        pipeID,
						        	MMPS_DSC_CAPTURE_INFO 	        *pCaptureInfo,
						        	MMP_SCAL_SOURCE			        ScalerSrc,
							    	MMP_SCAL_FIT_RANGE 		        *pCaptureFitrange,
						        	MMP_SCAL_GRAB_CTRL 		        *pCaptureGrabctl,
						        	MMP_ULONG              	        *pCaptureSize,
							    	MMP_ULONG                       ulDramAddr,
							    	MMP_ULONG                       ulSramAddr,
							    	MMP_ULONG                       ulFrmBufAddr,
							    	MMPS_3GPRECD_STILL_CAPTURE_INFO *pVRCaptInfo,
							    	MMP_BOOL                        bRecording);
#endif
//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

#if 0
void ____VR_Common_Preview_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetConfig
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get pointer to initialize customer configs.
 @retval Return pointer.
*/
MMPS_3GPRECD_CONFIGS* MMPS_3GPRECD_GetConfig(void)
{
    return (&m_VidRecdConfigs);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_CustomedPreviewAttr
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set video preview resolution.
 @param[in] bUserConfig		Use user defined preview configuration.
 @param[in] bRotate			Use DMA rotate to rotate preview buffer.
 @param[in] ubRotateDir		DMA rotate direction.
 @param[in] sFitMode		Scaler fit mode.
 @param[in] usBufWidth		Preview buffer width.
 @param[in] usBufHeight		Preview buffer height.
 @param[in] usStartX 		The X Offset of the display window.
 @param[in] usStartY 		The Y Offset of the display window.
 @param[in] usWinWidth  	The width of the display window.
 @param[in] usWinHeight 	The height of the display window.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_CustomedPreviewAttr(MMP_UBYTE  ubSnrSel,
                                         MMP_BOOL 	bUserConfig,
										 MMP_BOOL 	bRotate,
										 MMP_UBYTE 	ubRotateDir,
										 MMP_UBYTE	sFitMode,
										 MMP_USHORT usBufWidth, MMP_USHORT usBufHeight, 
										 MMP_USHORT usStartX, 	MMP_USHORT usStartY,
                                      	 MMP_USHORT usWinWidth, MMP_USHORT usWinHeight)
{
    m_sAhcVideoPrevInfo[ubSnrSel].bUserDefine  	    = bUserConfig;
	m_sAhcVideoPrevInfo[ubSnrSel].bPreviewRotate 	= bRotate;
	m_sAhcVideoPrevInfo[ubSnrSel].sPreviewDmaDir	= ubRotateDir;
	m_sAhcVideoPrevInfo[ubSnrSel].sFitMode		    = sFitMode;
 	m_sAhcVideoPrevInfo[ubSnrSel].ulPreviewBufW	    = usBufWidth;
 	m_sAhcVideoPrevInfo[ubSnrSel].ulPreviewBufH 	= usBufHeight;
    m_sAhcVideoPrevInfo[ubSnrSel].ulDispStartX  	= usStartX;
    m_sAhcVideoPrevInfo[ubSnrSel].ulDispStartY  	= usStartY;
    m_sAhcVideoPrevInfo[ubSnrSel].ulDispWidth   	= usWinWidth;
    m_sAhcVideoPrevInfo[ubSnrSel].ulDispHeight  	= usWinHeight;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_AdjustPreviewRes
//  Description : This function need to called before MMPS_3GPRECD_PreviewStart()
//------------------------------------------------------------------------------
static MMP_ERR MMPS_3GPRECD_AdjustPreviewRes(MMP_UBYTE ubSnrSel)
{
    MMP_ULONG 	ulScalInW, ulScalInH;
  	MMP_ULONG   ulPreviewW, ulPreviewH;
	
	if (m_VidRecdConfigs.previewpath[ubSnrSel] == MMP_3GP_PATH_INVALID) {
		return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
	}

	MMPS_Sensor_GetCurPrevScalInputRes(ubSnrSel, &ulScalInW, &ulScalInH);
    
    // Calculate preview parameters.
    if (m_sAhcVideoPrevInfo[ubSnrSel].bUserDefine) {
        ulPreviewW = m_sAhcVideoPrevInfo[ubSnrSel].ulPreviewBufW;
        ulPreviewH = m_sAhcVideoPrevInfo[ubSnrSel].ulPreviewBufH;
    }
    else {
        ulPreviewW = m_VidRecdConfigs.previewdata[ubSnrSel].usVidPreviewBufW[m_VidRecdModes.usVideoPreviewMode];
        ulPreviewH = m_VidRecdConfigs.previewdata[ubSnrSel].usVidPreviewBufH[m_VidRecdModes.usVideoPreviewMode];
    }
  
    if ((m_sAhcVideoPrevInfo[ubSnrSel].bUserDefine) &&
        (m_VidRecdModes.usVideoPreviewMode != VIDRECD_CCIR_PREVIEW_MODE)) {

#if (REAR_CAM_TYPE == REAR_CAM_TYPE_TV_DECODER)
    	if (ubSnrSel != SCD_SENSOR)
#endif
        {
        	MMPS_Display_AdjustScaleInSize( MMP_DISPLAY_PRM_CTL,
                                        ulScalInW,
                                        ulScalInH,
                                        ulPreviewW,
                                        ulPreviewH,
                                        &ulPreviewW,
                                        &ulPreviewH);
        }
    }

    if (ubSnrSel != SCD_SENSOR)
    {
    	m_ulVRPreviewW[ubSnrSel] = ulPreviewW;
    	m_ulVRPreviewH[ubSnrSel] = ulPreviewH;
    }
   	else{
		m_ulVRPreviewW[ubSnrSel] = ALIGN8(ulPreviewW);
		m_ulVRPreviewH[ubSnrSel] = ALIGN8(ulPreviewH);
    }
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetPreviewMode
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set customized preview mode.
 @param[in] usPreviewMode Assign preview display mode. 
            0: VIDRECD_NORMAL_PREVIEW_MODE, 
            1: VIDRECD_FULL_PREVIEW_MODE, 
            2: VIDRECD_NTSC_PREVIEW_MODE, 
            3: VIDRECD_PAL_PREVIEW_MODE,
            4: VIDRECD_HDMI_PREVIEW_MODE
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetPreviewMode(MMP_USHORT usPreviewMode)
{
    m_VidRecdModes.usVideoPreviewMode = usPreviewMode;

    return MMP_ERR_NONE;
}
//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetPreviewMode
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get customized preview mode.
 @param[in] usPreviewMode Assign preview display mode. 
            0: VIDRECD_NORMAL_PREVIEW_MODE, 
            1: VIDRECD_FULL_PREVIEW_MODE, 
            2: VIDRECD_NTSC_PREVIEW_MODE, 
            3: VIDRECD_PAL_PREVIEW_MODE,
            4: VIDRECD_HDMI_PREVIEW_MODE
 @retval usVideoPreviewMode
*/
MMP_USHORT MMPS_3GPRECD_GetPreviewMode(void)
{
    return m_VidRecdModes.usVideoPreviewMode;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetPreviewPipeStatus
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Return in video preview status or not.
 @param[out] bEnable preview enable.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_GetPreviewPipeStatus(MMP_UBYTE ubSnrSel, MMP_BOOL *bEnable)
{
    *bEnable = m_bVidPreviewActive[ubSnrSel];
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetPreviewPipe
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_GetPreviewPipe(MMP_UBYTE ubSnrSel, MMP_IBC_PIPEID *pPipe)
{
    if (ubSnrSel == PRM_SENSOR) {
        *pPipe = m_PreviewFctlLink.ibcpipeID;
    }
    #if (SUPPORT_DUAL_SNR)
    else if (ubSnrSel == SCD_SENSOR) {
        *pPipe = m_2ndPrewFctlLink.ibcpipeID;
    }
    #endif
    else {
        RTNA_DBG_Str0("UNDEFINE SNR:");  RTNA_DBG_Byte0(ubSnrSel);  RTNA_DBG_Str0(" \r\n");
    }
          
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetPreviewSize
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_GetPreviewSize(MMP_UBYTE ubSnrSel, MMP_ULONG *width, MMP_ULONG *height)
{
    *width = m_ulVRPreviewW[ubSnrSel];
    *height = m_ulVRPreviewH[ubSnrSel];

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetPreviewSize
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_SetPreviewSize(MMP_UBYTE ubSnrSel, MMP_ULONG *width, MMP_ULONG *height)
{
    m_ulVRPreviewW[ubSnrSel] = *width;
    m_ulVRPreviewH[ubSnrSel] = *height;

    return MMP_ERR_NONE;
}
#if 0
void ____VR_1st_Preview_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetPreviewPipeConfig
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set pipe config.
 @param[in] *pPreviewBuf 	Pointer to Preview buffer.
 @param[in] *pLdcSrcBuf 	Pointer to LDC source buffer.
 @param[in] *pMdtcBuf 		Pointer to Motion detection buffer.
 @param[in] usPreviewW 		The preview buffer width.
 @param[in] usPreviewH 		The preview buffer height.
 @retval MMP_ERR_NONE Success.
*/
static MMP_ERR MMPS_3GPRECD_SetPreviewPipeConfig(MMPS_3GPRECD_PREVIEW_BUFINFO 	*pPreviewBuf,
										  		 MMP_LDC_SRC_BUFINFO 			*pLdcSrcBuf,
										  		 MMPS_3GPRECD_MDTC_BUFINFO		*pMdtcBuf,
										  		 MMP_USHORT 					usPreviewW, 
										  		 MMP_USHORT 					usPreviewH,
										  		 MMP_UBYTE                      ubSnrSel)
{
    MMP_USHORT				usModeIdx = m_VidRecdModes.usVideoPreviewMode;
    MMP_ULONG				ulScalInW, ulScalInH;
	MMP_ULONG          		ulRotateW, ulRotateH;
	MMP_BOOL				bDmaRotateEn;
	MMP_SCAL_FIT_MODE		sFitMode;
 	MMP_SCAL_FIT_MODE		sPreFitMode;
   	MMP_SCAL_FIT_RANGE		fitrange;
    MMP_SCAL_GRAB_CTRL   	previewGrabctl, DispGrabctl;
    MMP_DISPLAY_DISP_ATTR	dispAttr;
    MMP_DISPLAY_ROTATE_TYPE	ubDmaRotateDir;
    MMPD_FCTL_ATTR  		fctlAttr;
    MMP_USHORT				i;
	MMP_ULONG             	ulDispStartX, ulDispStartY, ulDispWidth, ulDispHeight;
	MMP_SCAL_FIT_RANGE 		sFitRangeBayer;
	MMP_SCAL_GRAB_CTRL		sGrabctlBayer;
	MMP_USHORT				usCurZoomStep = 0;
    MMP_ULONG               ulInWidth, ulInHeight;
	/* Parameter Check */
	if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_INVALID) {
		return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
	}

	if (pPreviewBuf == NULL) {
		return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
	}
	
	#if (SUPPORT_LDC_RECD)
	if ((m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_LB_SINGLE || 
	  	 m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_LB_MULTI  ||
	  	 m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_MULTISLICE) && pLdcSrcBuf == NULL) {
		return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
	}
	#endif

	#if (SUPPORT_MDTC)
	if (pMdtcBuf == NULL) {
		return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
    }
	#endif

	/* Get the preivew display parameters */
    MMPS_Sensor_GetCurPrevScalInputRes(ubSnrSel, &ulScalInW, &ulScalInH);

	MMPD_BayerScaler_GetZoomInfo(MMP_BAYER_SCAL_DOWN, &sFitRangeBayer, &sGrabctlBayer); 

    if (m_sAhcVideoPrevInfo[ubSnrSel].bUserDefine) {
    	bDmaRotateEn    = m_sAhcVideoPrevInfo[ubSnrSel].bPreviewRotate;
    	ubDmaRotateDir	= m_sAhcVideoPrevInfo[ubSnrSel].sPreviewDmaDir;
    	sFitMode 		= m_sAhcVideoPrevInfo[ubSnrSel].sFitMode;
        ulDispStartX  	= m_sAhcVideoPrevInfo[ubSnrSel].ulDispStartX;
        ulDispStartY  	= m_sAhcVideoPrevInfo[ubSnrSel].ulDispStartY;
        ulDispWidth   	= m_sAhcVideoPrevInfo[ubSnrSel].ulDispWidth;
        ulDispHeight  	= m_sAhcVideoPrevInfo[ubSnrSel].ulDispHeight;
    }
    else {
    	bDmaRotateEn	= m_VidRecdConfigs.previewdata[0].bUseRotateDMA[usModeIdx];
    	ubDmaRotateDir	= m_VidRecdConfigs.previewdata[0].ubDMARotateDir[usModeIdx];
    	sFitMode 		= m_VidRecdConfigs.previewdata[0].sFitMode[usModeIdx];
        ulDispStartX  	= m_VidRecdConfigs.previewdata[0].usVidDispStartX[usModeIdx];
        ulDispStartY  	= m_VidRecdConfigs.previewdata[0].usVidDispStartY[usModeIdx];
        ulDispWidth   	= m_VidRecdConfigs.previewdata[0].usVidDisplayW[usModeIdx];
        ulDispHeight  	= m_VidRecdConfigs.previewdata[0].usVidDisplayH[usModeIdx];
    }

	/* Initial zoom relative config */ 
	MMPS_3GPRECD_InitDigitalZoomParam(m_PreviewFctlLink.scalerpath);

	MMPS_3GPRECD_RestoreDigitalZoomRange(m_PreviewFctlLink.scalerpath);

    if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_2PIPE) 
    {
        // Config Video Preview Pipe
        #if (TV_JAGGY_WORKAROUND)
        if (usModeIdx == VIDRECD_NTSC_PREVIEW_MODE || 
        	usModeIdx == VIDRECD_PAL_PREVIEW_MODE)
 		{
	 		fitrange.ulInWidth 	= TV_JAGGY_1ST_OUT_W;
	 		fitrange.ulInHeight = TV_JAGGY_1ST_OUT_H;
        }
        else
        #endif
        {
        	if (MMP_IsVidPtzEnable()) {
         		ulInWidth 	= sFitRangeBayer.ulOutWidth;
        		ulInHeight	= sFitRangeBayer.ulOutHeight;       	
        	}
        	else {
        		ulInWidth 	= ulScalInW;
        		ulInHeight	= ulScalInH;
        	}
        }
        sPreFitMode             = sFitMode;
        fitrange.fitmode    	= sFitMode;
        fitrange.scalerType		= MMP_SCAL_TYPE_SCALER;
        fitrange.ulOutWidth     = usPreviewW;
       	fitrange.ulOutHeight    = usPreviewH;
        fitrange.ulInWidth      = ulInWidth;
        fitrange.ulInHeight     = ulInHeight;

        fitrange.ulInGrabX		= 1;
        fitrange.ulInGrabY		= 1;
        fitrange.ulInGrabW		= fitrange.ulInWidth;
        fitrange.ulInGrabH		= fitrange.ulInHeight;

        MMPD_PTZ_InitPtzInfo(m_PreviewFctlLink.scalerpath,
				             fitrange.fitmode,
				             fitrange.ulInWidth, fitrange.ulInHeight,
				             fitrange.ulOutWidth, fitrange.ulOutHeight);

		MMPD_PTZ_GetCurPtzStep(m_PreviewFctlLink.scalerpath, NULL, &usCurZoomStep, NULL, NULL);

		MMPD_PTZ_CalculatePtzInfo(m_PreviewFctlLink.scalerpath, usCurZoomStep, 0, 0);

		MMPD_PTZ_GetCurPtzInfo(m_PreviewFctlLink.scalerpath, &fitrange, &previewGrabctl);

		if (MMP_IsVidPtzEnable()) {
			MMPD_PTZ_ReCalculateGrabRange(&fitrange, &previewGrabctl);
		}

        fctlAttr.colormode      = m_VidRecdConfigs.previewdata[0].DispColorFmt[usModeIdx];
        fctlAttr.fctllink       = m_PreviewFctlLink;
        fctlAttr.fitrange		= fitrange;
        fctlAttr.grabctl        = previewGrabctl;
        if ((m_VidRecdConfigs.bRawPreviewEnable[0]) &&
            (m_VidRecdConfigs.ubRawPreviewBitMode[0] == MMP_RAW_COLORFMT_YUV420 ||
             m_VidRecdConfigs.ubRawPreviewBitMode[0] == MMP_RAW_COLORFMT_YUV422))
        {
            fctlAttr.scalsrc    = MMP_SCAL_SOURCE_GRA;
        }
        else {
            fctlAttr.scalsrc    = MMP_SCAL_SOURCE_ISP;
        }
        fctlAttr.bSetScalerSrc	= MMP_TRUE;
        fctlAttr.usBufCnt  		= pPreviewBuf->usBufCnt;

        for (i = 0; i < fctlAttr.usBufCnt; i++) {
            fctlAttr.ulBaseAddr[i] = pPreviewBuf->ulYBuf[i];
            fctlAttr.ulBaseUAddr[i] = pPreviewBuf->ulUBuf[i];
            fctlAttr.ulBaseVAddr[i] = pPreviewBuf->ulVBuf[i];
        }

        if (bDmaRotateEn) {
            fctlAttr.bUseRotateDMA  = MMP_TRUE;
            fctlAttr.usRotateBufCnt = pPreviewBuf->usRotateBufCnt;
            
            for (i = 0; i < fctlAttr.usRotateBufCnt; i++) {
                fctlAttr.ulRotateAddr[i] = pPreviewBuf->ulRotateYBuf[i];
                fctlAttr.ulRotateUAddr[i] = pPreviewBuf->ulRotateUBuf[i];
                fctlAttr.ulRotateVAddr[i] = pPreviewBuf->ulRotateVBuf[i];
            }
        }
        else {
            fctlAttr.bUseRotateDMA = MMP_FALSE;
            fctlAttr.usRotateBufCnt = 0;
        }

        m_VRPreviewFctlAttr[0] = fctlAttr;

		#if (TV_JAGGY_WORKAROUND)
		if (usModeIdx == VIDRECD_NTSC_PREVIEW_MODE || 
        	usModeIdx == VIDRECD_PAL_PREVIEW_MODE)
			MMPD_Fctl_SetPipeAttrForIbcFbForTV(ulScalInW, ulScalInH, &fctlAttr);
		else
		#endif
        	MMPD_Fctl_SetPipeAttrForIbcFB(&fctlAttr);

		MMPD_Fctl_ClearPreviewBuf(m_PreviewFctlLink.ibcpipeID, 0xFFFFFF);

        if (bDmaRotateEn) {
            MMPD_Fctl_LinkPipeToDma(m_PreviewFctlLink.ibcpipeID,
            						m_VidRecdConfigs.previewdata[0].DispWinId[usModeIdx], 
            						m_VidRecdConfigs.previewdata[0].DispDevice[usModeIdx],
            						ubDmaRotateDir);
		}
        else {
        	MMPD_Fctl_LinkPipeToDisplay(m_PreviewFctlLink.ibcpipeID, 
        								m_VidRecdConfigs.previewdata[0].DispWinId[usModeIdx], 
        								m_VidRecdConfigs.previewdata[0].DispDevice[usModeIdx]);
		}
		
        #if (SUPPORT_FDTC)
        if (m_VidRecdConfigs.bFdtcEnable) {
            MMPS_Sensor_FDPathInterface(MMPS_IVA_FDTC_YUV420,
						                &(fctlAttr.fctllink),
						                &(fctlAttr.fctllink));
        }
        #endif

        // Config Display Window
        if ((m_VidRecdConfigs.previewdata[0].VidDispDir[usModeIdx] == MMP_DISPLAY_ROTATE_NO_ROTATE) ||
            (m_VidRecdConfigs.previewdata[0].VidDispDir[usModeIdx] == MMP_DISPLAY_ROTATE_RIGHT_180)) {                        
            ulRotateW = usPreviewW;
            ulRotateH = usPreviewH;
        }
        else {
            ulRotateW = usPreviewH;
            ulRotateH = usPreviewW;
        }
        
        if (bDmaRotateEn) {
        	// Rotate 90/270 for vertical panel
        	ulRotateW = usPreviewH;
            ulRotateH = usPreviewW;
        }
        
        dispAttr.usStartX          = 0;
        dispAttr.usStartY          = 0;
		#if 1 // Image at center
        dispAttr.usDisplayOffsetX  = (ulDispWidth > ulRotateW) ? ((ulDispWidth - ulRotateW) >> 1) : (0);
        dispAttr.usDisplayOffsetY  = (ulDispHeight > ulRotateH) ? ((ulDispHeight - ulRotateH) >> 1) : (0);
        #else
        dispAttr.usDisplayOffsetX  = ulDispStartX;
        dispAttr.usDisplayOffsetY  = ulDispStartY;
		#endif
        dispAttr.bMirror           = m_VidRecdConfigs.previewdata[0].bVidDispMirror[usModeIdx];

        if (bDmaRotateEn) {
            dispAttr.rotatetype    = MMP_DISPLAY_ROTATE_NO_ROTATE;
        }
        else {
            dispAttr.rotatetype    = m_VidRecdConfigs.previewdata[0].VidDispDir[usModeIdx];
        }

        dispAttr.usDisplayWidth    = ulRotateW;
        dispAttr.usDisplayHeight   = ulRotateH;

        if (m_bHdmiInterlace) {
            dispAttr.usDisplayHeight = ulRotateH / 2;
        }
        
        MMPD_Display_SetWinToDisplay(m_VidRecdConfigs.previewdata[0].DispWinId[usModeIdx], &dispAttr);

        fitrange.fitmode        = sFitMode;
        fitrange.scalerType 	= MMP_SCAL_TYPE_WINSCALER;
        fitrange.ulInWidth      = ulRotateW;
        fitrange.ulInHeight     = ulRotateH;
        fitrange.ulOutWidth     = ulRotateW;
        fitrange.ulOutHeight    = ulRotateH;
 
 		fitrange.ulInGrabX 		= 1;
        fitrange.ulInGrabY 		= 1;
        fitrange.ulInGrabW 		= fitrange.ulInWidth;
        fitrange.ulInGrabH 		= fitrange.ulInHeight;
        
        MMPD_Scaler_GetGCDBestFitScale(&fitrange, &DispGrabctl);

        MMPD_Display_SetWinScaling(m_VidRecdConfigs.previewdata[0].DispWinId[usModeIdx], &fitrange, &DispGrabctl);

        #if (SUPPORT_LDC_RECD)
        MMPD_Fctl_UnLinkPipeToLdc(m_LdcSrcFctlLink.ibcpipeID);
        #endif
    }
    #if (SUPPORT_LDC_RECD)
    else if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_LB_SINGLE || 
    		 m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_LB_MULTI 	||
    		 m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_MULTISLICE) 
    {
    	MMP_SCAL_GRAB_CTRL  ldcGrabCtl;

		MMPF_LDC_SetLinkPipe(m_LdcSrcFctlLink.ibcpipeID,
							 m_PreviewFctlLink.ibcpipeID,
							 m_RecordFctlLink.ibcpipeID,
							 m_PreviewFctlLink.ibcpipeID);
		
        // Config pipe for graphic loopback
        fitrange.fitmode        = MMP_SCAL_FITMODE_OUT;
        fitrange.scalerType		= MMP_SCAL_TYPE_SCALER;
		fitrange.ulInWidth  	= ulScalInW;
        fitrange.ulInHeight 	= ulScalInH; 
        fitrange.ulOutWidth     = m_ulLdcMaxSrcWidth;
        fitrange.ulOutHeight    = m_ulLdcMaxSrcHeight;

        fitrange.ulInGrabX 		= 1;
        fitrange.ulInGrabY 		= 1;
        fitrange.ulInGrabW 		= fitrange.ulInWidth;
        fitrange.ulInGrabH 		= fitrange.ulInHeight;

        MMPD_Scaler_GetGCDBestFitScale(&fitrange, &ldcGrabCtl);

        fctlAttr.colormode      = MMP_DISPLAY_COLOR_YUV420_INTERLEAVE;
        fctlAttr.fctllink       = m_LdcSrcFctlLink;
        fctlAttr.fitrange		= fitrange;
        fctlAttr.grabctl        = ldcGrabCtl;
        fctlAttr.scalsrc		= MMP_SCAL_SOURCE_ISP;
        fctlAttr.bSetScalerSrc	= MMP_TRUE;
        fctlAttr.usBufCnt  		= pLdcSrcBuf->usInBufCnt;

        for (i = 0; i < pLdcSrcBuf->usInBufCnt; i++) {
            fctlAttr.ulBaseAddr[i]  = pLdcSrcBuf->ulInYBuf[i];
            fctlAttr.ulBaseUAddr[i] = pLdcSrcBuf->ulInUBuf[i];
            fctlAttr.ulBaseVAddr[i] = pLdcSrcBuf->ulInVBuf[i];
        }
        
        fctlAttr.bUseRotateDMA  = MMP_FALSE;
        fctlAttr.usRotateBufCnt = 0;

        MMPD_Fctl_SetPipeAttrForIbcFB(&fctlAttr);
        MMPD_Fctl_ClearPreviewBuf(m_LdcSrcFctlLink.ibcpipeID, 0xFFFFFF);
        MMPD_Fctl_LinkPipeToLdc(m_LdcSrcFctlLink.ibcpipeID);
        
        if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_LB_MULTI ||
        	m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_MULTISLICE)
        {
	        // Config pipe for preview first
            fitrange.fitmode        = sFitMode;
            fitrange.scalerType		= MMP_SCAL_TYPE_SCALER;
	        fitrange.ulInWidth  	= m_ulLdcMaxOutWidth;
	        fitrange.ulInHeight 	= m_ulLdcMaxOutHeight;
            fitrange.ulOutWidth     = usPreviewW;
            fitrange.ulOutHeight    = usPreviewH;

			fitrange.ulInGrabX 		= 1;
    		fitrange.ulInGrabY 		= 1;
    		fitrange.ulInGrabW 		= fitrange.ulInWidth;
    		fitrange.ulInGrabH 		= fitrange.ulInHeight;

	        MMPD_PTZ_InitPtzInfo(m_PreviewFctlLink.scalerpath,
	        					 fitrange.fitmode,
					             fitrange.ulInWidth, fitrange.ulInHeight, 
					             fitrange.ulOutWidth, fitrange.ulOutHeight);

			MMPD_PTZ_GetCurPtzStep(m_PreviewFctlLink.scalerpath, NULL, &usCurZoomStep, NULL, NULL);

			MMPD_PTZ_CalculatePtzInfo(m_PreviewFctlLink.scalerpath, usCurZoomStep, 0, 0);

			MMPD_PTZ_GetCurPtzInfo(m_PreviewFctlLink.scalerpath, &fitrange, &previewGrabctl);

			if (MMP_IsVidPtzEnable()) {
				MMPD_PTZ_ReCalculateGrabRange(&fitrange, &previewGrabctl);
			}

	        fctlAttr.colormode      = m_VidRecdConfigs.previewdata[0].DispColorFmt[usModeIdx];
	        fctlAttr.fctllink       = m_PreviewFctlLink;
	        fctlAttr.fitrange		= fitrange;
	        fctlAttr.grabctl        = previewGrabctl;
	        fctlAttr.scalsrc		= MMP_SCAL_SOURCE_LDC;
	        fctlAttr.bSetScalerSrc	= MMP_TRUE;
	        fctlAttr.usBufCnt  		= pPreviewBuf->usBufCnt;
	        fctlAttr.bUseRotateDMA 	= MMP_FALSE;
	        fctlAttr.usRotateBufCnt = 0;
				        
	        for (i = 0; i < fctlAttr.usBufCnt; i++) {
	            fctlAttr.ulBaseAddr[i] = pPreviewBuf->ulYBuf[i];
	            fctlAttr.ulBaseUAddr[i] = pPreviewBuf->ulUBuf[i];
	            fctlAttr.ulBaseVAddr[i] = pPreviewBuf->ulVBuf[i];
	        }

	        MMPD_Fctl_SetPipeAttrForIbcFB(&fctlAttr);
			MMPD_Fctl_ClearPreviewBuf(m_PreviewFctlLink.ibcpipeID, 0xFFFFFF);
			MMPD_Fctl_LinkPipeToDisplay(m_PreviewFctlLink.ibcpipeID, 
										m_VidRecdConfigs.previewdata[0].DispWinId[usModeIdx], 
										m_VidRecdConfigs.previewdata[0].DispDevice[usModeIdx]);
			
	        // Config pipe for loopback 
	        MMPD_Fctl_ResetIBCLinkType(m_PreviewFctlLink.ibcpipeID);
	        
	        fitrange.fitmode        = MMP_SCAL_FITMODE_OUT;
	        fitrange.scalerType		= MMP_SCAL_TYPE_SCALER;
	  		fitrange.ulInWidth  	= m_ulLdcMaxOutWidth;
	        fitrange.ulInHeight 	= m_ulLdcMaxOutHeight;
	       	fitrange.ulOutWidth     = m_ulLdcMaxOutWidth;
	        fitrange.ulOutHeight    = m_ulLdcMaxOutHeight;

			fitrange.ulInGrabX 		= 1;
    		fitrange.ulInGrabY 		= 1;
    		fitrange.ulInGrabW 		= fitrange.ulInWidth;
    		fitrange.ulInGrabH 		= fitrange.ulInHeight;

	        MMPD_Scaler_GetGCDBestFitScale(&fitrange, &ldcGrabCtl);

	        fctlAttr.colormode      = MMP_DISPLAY_COLOR_YUV420_INTERLEAVE;
	        fctlAttr.fctllink       = m_PreviewFctlLink;
	        fctlAttr.fitrange		= fitrange;
	        fctlAttr.grabctl        = ldcGrabCtl;
	        fctlAttr.scalsrc		= MMP_SCAL_SOURCE_LDC;
	        fctlAttr.bSetScalerSrc	= MMP_TRUE;
	        fctlAttr.usBufCnt		= pPreviewBuf->usBufCnt;
	        fctlAttr.bUseRotateDMA 	= MMP_FALSE;
	        fctlAttr.usRotateBufCnt = 0;

	        for (i = 0; i < fctlAttr.usBufCnt; i++) {
	            fctlAttr.ulBaseAddr[i] = pPreviewBuf->ulYBuf[i];
	            fctlAttr.ulBaseUAddr[i] = pPreviewBuf->ulUBuf[i];
	            fctlAttr.ulBaseVAddr[i] = pPreviewBuf->ulVBuf[i];
	        }
	        
	        MMPD_Fctl_SetPipeAttrForIbcFB(&fctlAttr);
			MMPD_Fctl_ClearPreviewBuf(m_PreviewFctlLink.ibcpipeID, 0xFFFFFF);		
			MMPD_Fctl_LinkPipeToLdc(m_PreviewFctlLink.ibcpipeID);
        }
        else
        {
	        // Config pipe for preview
            fitrange.fitmode        = sFitMode;
            fitrange.scalerType 	= MMP_SCAL_TYPE_SCALER;
	        fitrange.ulInWidth  	= m_ulLdcMaxOutWidth;
	        fitrange.ulInHeight 	= m_ulLdcMaxOutHeight;
            fitrange.ulOutWidth     = usPreviewW;
            fitrange.ulOutHeight    = usPreviewH;

			fitrange.ulInGrabX 		= 1;
    		fitrange.ulInGrabY 		= 1;
    		fitrange.ulInGrabW 		= fitrange.ulInWidth;
    		fitrange.ulInGrabH 		= fitrange.ulInHeight;

	        MMPD_PTZ_InitPtzInfo(m_PreviewFctlLink.scalerpath,
	        					 fitrange.fitmode,
					             fitrange.ulInWidth, fitrange.ulInHeight, 
					             fitrange.ulOutWidth, fitrange.ulOutHeight);

			MMPD_PTZ_GetCurPtzStep(m_PreviewFctlLink.scalerpath, NULL, &usCurZoomStep, NULL, NULL);

			MMPD_PTZ_CalculatePtzInfo(m_PreviewFctlLink.scalerpath, usCurZoomStep, 0, 0);

			MMPD_PTZ_GetCurPtzInfo(m_PreviewFctlLink.scalerpath, &fitrange, &previewGrabctl);

			if (MMP_IsVidPtzEnable()) {
				MMPD_PTZ_ReCalculateGrabRange(&fitrange, &previewGrabctl);
			}

	        fctlAttr.colormode      = m_VidRecdConfigs.previewdata[0].DispColorFmt[usModeIdx];
	        fctlAttr.fctllink       = m_PreviewFctlLink;
	        fctlAttr.fitrange		= fitrange;
	        fctlAttr.grabctl        = previewGrabctl;
	        fctlAttr.scalsrc		= MMP_SCAL_SOURCE_LDC;
	        fctlAttr.bSetScalerSrc	= MMP_TRUE;
	        fctlAttr.usBufCnt  		= pPreviewBuf->usBufCnt;

	        for (i = 0; i < fctlAttr.usBufCnt; i++) {
	            fctlAttr.ulBaseAddr[i] = pPreviewBuf->ulYBuf[i];
	            fctlAttr.ulBaseUAddr[i] = pPreviewBuf->ulUBuf[i];
	            fctlAttr.ulBaseVAddr[i] = pPreviewBuf->ulVBuf[i];
	        }

	        if (bDmaRotateEn) {
	            fctlAttr.bUseRotateDMA  = MMP_TRUE;
	            fctlAttr.usRotateBufCnt = pPreviewBuf->usRotateBufCnt;
	            
	            for (i = 0; i < fctlAttr.usRotateBufCnt; i++) {
	                fctlAttr.ulRotateAddr[i]  = pPreviewBuf->ulRotateYBuf[i];
	                fctlAttr.ulRotateUAddr[i] = pPreviewBuf->ulRotateUBuf[i];
	                fctlAttr.ulRotateVAddr[i] = pPreviewBuf->ulRotateVBuf[i];
	            }
	        }
	        else {
	            fctlAttr.bUseRotateDMA = MMP_FALSE;
	            fctlAttr.usRotateBufCnt = 0;
	        }

	        MMPD_Fctl_SetPipeAttrForIbcFB(&fctlAttr);
			MMPD_Fctl_ClearPreviewBuf(m_PreviewFctlLink.ibcpipeID, 0xFFFFFF);

	        if (fctlAttr.bUseRotateDMA) {
	            MMPD_Fctl_LinkPipeToDma(m_PreviewFctlLink.ibcpipeID, 
	            						m_VidRecdConfigs.previewdata[0].DispWinId[usModeIdx], 
	            						m_VidRecdConfigs.previewdata[0].DispDevice[usModeIdx],
	            						ubDmaRotateDir);
	        }
	        else {
	            MMPD_Fctl_LinkPipeToDisplay(m_PreviewFctlLink.ibcpipeID, 
	            							m_VidRecdConfigs.previewdata[0].DispWinId[usModeIdx], 
	            							m_VidRecdConfigs.previewdata[0].DispDevice[usModeIdx]);
			}
		}

        // Config Display Window
        if ((m_VidRecdConfigs.previewdata[0].VidDispDir[usModeIdx] == MMP_DISPLAY_ROTATE_NO_ROTATE) ||
            (m_VidRecdConfigs.previewdata[0].VidDispDir[usModeIdx] == MMP_DISPLAY_ROTATE_RIGHT_180)) {                        
            ulRotateW = usPreviewW;
            ulRotateH = usPreviewH;
        }
        else {
            ulRotateW = usPreviewH;
            ulRotateH = usPreviewW;
        }
        
        if (bDmaRotateEn) {
        	// Rotate 90/270 for vertical panel
        	ulRotateW = usPreviewH;
            ulRotateH = usPreviewW;
        }
        
        dispAttr.usStartX          = 0;
        dispAttr.usStartY          = 0;
        dispAttr.usDisplayOffsetX  = (ulDispWidth > ulRotateW) ? ((ulDispWidth - ulRotateW) >> 1) : (0);
        dispAttr.usDisplayOffsetY  = (ulDispHeight > ulRotateH) ? ((ulDispHeight - ulRotateH) >> 1) : (0);
        dispAttr.bMirror           = m_VidRecdConfigs.previewdata[0].bVidDispMirror[usModeIdx];

        if (bDmaRotateEn) {
            dispAttr.rotatetype    = MMP_DISPLAY_ROTATE_NO_ROTATE;
        }
        else {
            dispAttr.rotatetype    = m_VidRecdConfigs.previewdata[0].VidDispDir[usModeIdx];
        }

        dispAttr.usDisplayWidth    = ulRotateW;
        dispAttr.usDisplayHeight   = ulRotateH;
        
        if (m_bHdmiInterlace) {
            dispAttr.usDisplayHeight = ulRotateH / 2;
        }
        
        MMPD_Display_SetWinToDisplay(m_VidRecdConfigs.previewdata[0].DispWinId[usModeIdx], &dispAttr);

        fitrange.fitmode        = sFitMode;
        fitrange.scalerType     = MMP_SCAL_TYPE_WINSCALER;
        fitrange.ulInWidth      = ulRotateW;
        fitrange.ulInHeight     = ulRotateH;

        if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_LB_MULTI ||
        	m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_MULTISLICE) {
        	/* Use display scaler to fit the desired output range */
	        fitrange.ulOutWidth  = m_ulVRPreviewW[ubSnrSel];
	        fitrange.ulOutHeight = m_ulVRPreviewH[ubSnrSel];
        }
        else {
            fitrange.ulOutWidth  = ulRotateW;
            fitrange.ulOutHeight = ulRotateH;
		}

        fitrange.ulInGrabX      = 1;
        fitrange.ulInGrabY 		= 1;
        fitrange.ulInGrabW 		= fitrange.ulInWidth;
        fitrange.ulInGrabH 	    = fitrange.ulInHeight;

        MMPD_Scaler_GetGCDBestFitScale(&fitrange, &DispGrabctl);

        MMPD_Display_SetWinScaling(m_VidRecdConfigs.previewdata[0].DispWinId[usModeIdx], &fitrange, &DispGrabctl);
    }
    #endif

// Config Motion Detection Pipe
#if (SUPPORT_MDTC)||(SUPPORT_ADAS)
    gsADASFitRange.fitmode        = sPreFitMode; //ADAS fitmode refer by preview's mode
    gsADASFitRange.scalerType 	= MMP_SCAL_TYPE_SCALER;
    gsADASFitRange.ulInWidth      = ulScalInW;
    gsADASFitRange.ulInHeight     = ulScalInH;

    gsADASFitRange.ulInGrabX 		= 1;
    gsADASFitRange.ulInGrabY 		= 1;
    gsADASFitRange.ulInGrabW 		= ulInWidth;
    gsADASFitRange.ulInGrabH 		= ulInHeight;
#if (ADAS_OUTPUT_LOG == 1)
    RTNA_DBG_Str (0, "\r\n");
    RTNA_DBG_Str (0, "====ADAS Parameter Calculate Result1====");
    RTNA_DBG_Str (0, "\r\n");
    RTNA_DBG_Str (0, "====ADASFitRange Paramters====");
    RTNA_DBG_Str (0, "\r\n");
    RTNA_DBG_Str (0, "gsADASFitRange.fitmode");
    RTNA_DBG_Long(0, gsADASFitRange.fitmode);
    RTNA_DBG_Str (0, "\r\n");
    RTNA_DBG_Str (0, "gsADASFitRange.scalerType");
    RTNA_DBG_Long(0, gsADASFitRange.scalerType);
    RTNA_DBG_Str (0, "\r\n");
    RTNA_DBG_Str (0, "gsADASFitRange.ulInWidth");
    RTNA_DBG_Long(0, gsADASFitRange.ulInWidth);
    RTNA_DBG_Str (0, "\r\n");
    RTNA_DBG_Str (0, "gsADASFitRange.ulInHeight");
    RTNA_DBG_Long(0, gsADASFitRange.ulInHeight);
    RTNA_DBG_Str (0, "\r\n");
    RTNA_DBG_Str (0, "gsADASFitRange.ulInGrabX");
    RTNA_DBG_Long(0, gsADASFitRange.ulInGrabX);
    RTNA_DBG_Str (0, "\r\n");
    RTNA_DBG_Str (0, "gsADASFitRange.ulInGrabY");
    RTNA_DBG_Long(0, gsADASFitRange.ulInGrabY);
    RTNA_DBG_Str (0, "\r\n");
    RTNA_DBG_Str (0, "gsADASFitRange.ulInGrabW");
    RTNA_DBG_Long(0, gsADASFitRange.ulInGrabW);
    RTNA_DBG_Str (0, "\r\n");
    RTNA_DBG_Str (0, "gsADASFitRange.ulInGrabH");
    RTNA_DBG_Long(0, gsADASFitRange.ulInGrabH);
    RTNA_DBG_Str (0, "\r\n");
    RTNA_DBG_Str (0, "====End of ADASFitRange Paramters====");
    RTNA_DBG_Str (0, "\r\n");
#endif
    MMPD_Scaler_GetGCDBestFitScale(&gsADASFitRange, &gsADASGrabctl);

    fctlAttr.colormode      = MMP_DISPLAY_COLOR_YUV420_INTERLEAVE;
    fctlAttr.fctllink       = m_MdtcFctlLink;
    fctlAttr.fitrange       = gsADASFitRange;
    fctlAttr.grabctl        = gsADASGrabctl;
    fctlAttr.scalsrc		= MMP_SCAL_SOURCE_ISP;
    fctlAttr.bSetScalerSrc	= MMP_TRUE;
    fctlAttr.usBufCnt  		= pMdtcBuf->usBufCnt;
    fctlAttr.bUseRotateDMA 	= MMP_FALSE;
	fctlAttr.usRotateBufCnt = 0;

    for (i = 0; i < fctlAttr.usBufCnt; i++) {
        fctlAttr.ulBaseAddr[i] = pMdtcBuf->ulYBuf[i];
    }
    MEMCPY(&m_ADASFctlAttr, &fctlAttr, sizeof(fctlAttr));

    MMPD_Fctl_SetSubPipeAttr(&fctlAttr);
    MMPS_Sensor_SetVMDPipe(m_MdtcFctlLink.ibcpipeID);

#if (CPU_ID == CPU_A && SUPPORT_ADAS)
    {
        MMP_ULONG ulFocalLength = 0, ulPixelSize = 0, ulDzoomN = 0, ulDzoomM = 0;
        MMPF_SENSOR_RESOLUTION* pResTable = gsSensorFunction->MMPF_Sensor_GetResTable(ubSnrSel);
        MMP_USHORT usCurPreviewMode = 0;

        _ADAS_GetCustomerPara(&ulFocalLength, &ulPixelSize, &ulDzoomN, &ulDzoomM);   

        //ulDzoomN/ulDzoomM are based on Y direction in ADAS library.
        ulDzoomN = gsADASGrabctl.ulScaleYN;
        ulDzoomM = gsADASGrabctl.ulScaleYM;

        //Restore bayer scaler ratio.
        ulDzoomN *= sGrabctlBayer.ulScaleYN;
        ulDzoomM *= sGrabctlBayer.ulScaleYM;


        MMPF_Sensor_GetParam(ubSnrSel, MMPF_SENSOR_CURRENT_PREVIEW_MODE, &usCurPreviewMode);
        
        // Restore sensor binning ratio.
        ulDzoomN *= pResTable->ubHBinningN[usCurPreviewMode];
        ulDzoomM *= pResTable->ubHBinningM[usCurPreviewMode];

        //Get sensor pixel size.
        ulPixelSize = (pResTable->usPixelSize /*Unit:nm*/ * 100) / 1000; //unit: um*100
#if (ADAS_OUTPUT_LOG == 1)
        RTNA_DBG_Str (0, "\r\n");
        RTNA_DBG_Str (0, "====ADAS Parameter Calculate Result2====");
        RTNA_DBG_Str (0, "\r\n");
        RTNA_DBG_Str (0, "====ScaleYN/YM of ADAS ====");
        RTNA_DBG_Str (0, "\r\n");
        RTNA_DBG_Str (0, "gsADASGrabctl.ulScaleYN");
        RTNA_DBG_Long(0, gsADASGrabctl.ulScaleYN);
        RTNA_DBG_Str (0, "\r\n");
        RTNA_DBG_Str (0, "gsADASGrabctl.ulScaleYM");
        RTNA_DBG_Long(0, gsADASGrabctl.ulScaleYM);
        RTNA_DBG_Str (0, "\r\n");
        RTNA_DBG_Str (0, "sGrabctlBayer.ulScaleYN");
        RTNA_DBG_Long(0, sGrabctlBayer.ulScaleYN);
        RTNA_DBG_Str (0, "\r\n");
        RTNA_DBG_Str (0, "sGrabctlBayer.ulScaleYM");
        RTNA_DBG_Long(0, sGrabctlBayer.ulScaleYM);
        RTNA_DBG_Str (0, "\r\n");
        RTNA_DBG_Str (0, "pResTable->ubHBinningN[usCurPreviewMode]");
        RTNA_DBG_Long(0, pResTable->ubHBinningN[usCurPreviewMode]);
        RTNA_DBG_Str (0, "\r\n");
        RTNA_DBG_Str (0, "pResTable->ubHBinningM[usCurPreviewMode]");
        RTNA_DBG_Long(0, pResTable->ubHBinningM[usCurPreviewMode]);
        RTNA_DBG_Str (0, "\r\n");

        RTNA_DBG_Str(0, "ADAS Focal Len:");
        RTNA_DBG_Long(0, ulFocalLength);
        RTNA_DBG_Str(0, ", PixelSize:");
        RTNA_DBG_Long(0, ulPixelSize);
        RTNA_DBG_Str(0, "\r\nZoom N:");
        RTNA_DBG_Long(0, ulDzoomN);
        RTNA_DBG_Str(0, ", Zoom M:");
        RTNA_DBG_Long(0, ulDzoomM);
        RTNA_DBG_Str(0, "\r\n");
        RTNA_DBG_Str (0, "====End of ScaleYN/YM of ADAS ====");
        RTNA_DBG_Str (0, "\r\n");
#endif        
        _ADAS_CustomerSpecifyPara(ulFocalLength, ulPixelSize, ulDzoomN, ulDzoomM);           
    }
#endif
#endif

    // Config Video Streaming (Wifi) Pipe
    #if (SUPPORT_VR_WIFI_STREAM)
    {
    	extern MMP_USHORT m_usMJpegWidth, m_usMJpegHeight;

        fitrange.fitmode    	= MMP_SCAL_FITMODE_OPTIMAL;
		fitrange.scalerType 	= MMP_SCAL_TYPE_SCALER;
        fitrange.ulInWidth      = ulScalInW;
        fitrange.ulInHeight     = ulScalInH;
        fitrange.ulOutWidth     = m_usMJpegWidth;
        fitrange.ulOutHeight    = m_usMJpegHeight;
        
        fitrange.ulInGrabX  	= 1;
        fitrange.ulInGrabY  	= 1;
        fitrange.ulInGrabW  	= fitrange.ulInWidth;
        fitrange.ulInGrabH  	= fitrange.ulInHeight;
		
		MMPD_Scaler_GetGCDBestFitScale(&fitrange, &previewGrabctl);
		
        fctlAttr.fctllink       = m_StreamingFctlLink;
        fctlAttr.fitrange       = fitrange;
        fctlAttr.grabctl        = previewGrabctl;
        fctlAttr.scalsrc		= MMP_SCAL_SOURCE_ISP;
        fctlAttr.bSetScalerSrc	= MMP_TRUE;
		
		MMPD_DSC_SetMJPEGPipe(fctlAttr.fctllink.scalerpath);
		
		MMPD_Fctl_SetPipeAttrForJpeg(&fctlAttr, MMP_TRUE, MMP_FALSE);
		
		MMPD_Scaler_SetEnable(fctlAttr.fctllink.scalerpath, MMP_FALSE);

	    // Setting for JPEG engine
		#if 1
		MMPS_DSC_SetCaptureJpegQualityEx(MMP_DSC_JPEG_RC_ID_MJPEG_1ST_STREAM, MMP_TRUE, MMP_FALSE, 
		                               200, 220, 3, MMP_DSC_CAPTURE_HIGH_QUALITY);
		#else
		MMPS_DSC_SetCaptureJpegQuality(MMP_DSC_JPEG_RC_ID_MJPEG_1ST_STREAM, MMP_TRUE, MMP_FALSE, 
		                               200, 220, 3, MMP_DSC_CAPTURE_HIGH_QUALITY);
		#endif
	    MMPD_DSC_SetJpegResol(m_usMJpegWidth, m_usMJpegHeight, MMP_DSC_JPEG_RC_ID_MJPEG_1ST_STREAM);

	    MMPD_DSC_SetCapturePath(fctlAttr.fctllink.scalerpath,
	                            fctlAttr.fctllink.icopipeID,
	                            fctlAttr.fctllink.ibcpipeID);
    }
    #endif

    #if (SUPPORT_DEC_MJPEG_TO_PREVIEW) //TBD
    //MMPS_3GPRECD_InitDecMjpegToPreview(m_usDecMjpegToPreviewSrcW, m_usDecMjpegToPreviewSrcH);
    #else
	#if (HANDLE_JPEG_EVENT_BY_QUEUE)
	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, 	MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_ICON,	MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, 	MMP_TRUE);	
	#endif
    #endif
    // Tune MCI priority
    #if (SUPPORT_LDC_RECD)
   	if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_LB_SINGLE) {
   		if (m_LdcSrcFctlLink.ibcpipeID == MMP_IBC_PIPE_0)
        	MMPD_VIDENC_TuneMCIPriority(MMPD_VIDENC_MCI_GRA_LDC_P0);
        else if (m_LdcSrcFctlLink.ibcpipeID == MMP_IBC_PIPE_1)
        	MMPD_VIDENC_TuneMCIPriority(MMPD_VIDENC_MCI_GRA_LDC_P1);
        else if (m_LdcSrcFctlLink.ibcpipeID == MMP_IBC_PIPE_2)
        	MMPD_VIDENC_TuneMCIPriority(MMPD_VIDENC_MCI_GRA_LDC_P2);
        else if (m_LdcSrcFctlLink.ibcpipeID == MMP_IBC_PIPE_3)
        	MMPD_VIDENC_TuneMCIPriority(MMPD_VIDENC_MCI_GRA_LDC_P3);

        MMPD_H264ENC_SetEncByteCnt(128);
    }
   	else if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_LB_MULTI ||
   			 m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_MULTISLICE) {
        MMPD_VIDENC_TuneMCIPriority(MMPD_VIDENC_MCI_GRA_LDC_P3P1_H264_P0);
        MMPD_H264ENC_SetEncByteCnt(128);
    }
    else
    #endif
    {
        if (m_VidRecdConfigs.bRawPreviewEnable[0]) {
            if (m_PreviewFctlLink.ibcpipeID == MMP_IBC_PIPE_0)
                MMPD_VIDENC_TuneMCIPriority(MMPD_VIDENC_MCI_RAW0_DVI_P0);
            else if (m_PreviewFctlLink.ibcpipeID == MMP_IBC_PIPE_1)
                MMPD_VIDENC_TuneMCIPriority(MMPD_VIDENC_MCI_RAW0_DVI_P1);
            else if (m_PreviewFctlLink.ibcpipeID == MMP_IBC_PIPE_2)
                MMPD_VIDENC_TuneMCIPriority(MMPD_VIDENC_MCI_RAW0_DVI_P2);
            else if (m_PreviewFctlLink.ibcpipeID == MMP_IBC_PIPE_3)
                MMPD_VIDENC_TuneMCIPriority(MMPD_VIDENC_MCI_RAW0_DVI_P3); 
        }
        else if (m_VidRecdConfigs.previewdata[0].bUseRotateDMA[usModeIdx]) {
        	if (m_PreviewFctlLink.ibcpipeID == MMP_IBC_PIPE_0)
            	MMPD_VIDENC_TuneMCIPriority(MMPD_VIDENC_MCI_DMAR_H264_P0);
            else if (m_PreviewFctlLink.ibcpipeID == MMP_IBC_PIPE_1)
            	MMPD_VIDENC_TuneMCIPriority(MMPD_VIDENC_MCI_DMAR_H264_P1);
            else if (m_PreviewFctlLink.ibcpipeID == MMP_IBC_PIPE_2)
            	MMPD_VIDENC_TuneMCIPriority(MMPD_VIDENC_MCI_DMAR_H264_P2);
            else if (m_PreviewFctlLink.ibcpipeID == MMP_IBC_PIPE_3)
            	MMPD_VIDENC_TuneMCIPriority(MMPD_VIDENC_MCI_DMAR_H264_P3);
        }
        else {
            if (m_PreviewFctlLink.ibcpipeID == MMP_IBC_PIPE_0)
                MMPD_VIDENC_TuneMCIPriority(MMPD_VIDENC_MCI_DMAR_H264_P0);
            else if (m_PreviewFctlLink.ibcpipeID == MMP_IBC_PIPE_1)
                MMPD_VIDENC_TuneMCIPriority(MMPD_VIDENC_MCI_DMAR_H264_P1);
            else if (m_PreviewFctlLink.ibcpipeID == MMP_IBC_PIPE_2)
                MMPD_VIDENC_TuneMCIPriority(MMPD_VIDENC_MCI_DMAR_H264_P2);
            else if (m_PreviewFctlLink.ibcpipeID == MMP_IBC_PIPE_3)
                MMPD_VIDENC_TuneMCIPriority(MMPD_VIDENC_MCI_DMAR_H264_P3);
        }
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_EnablePreviewPipe
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Turn on and off preview for video encode.

 @param[in] bEnable 			Enable and disable scaler path for video preview.
 @param[in] bCheckFrameEnd 		When "bEnable" is MMP_TRUE, the setting means check frame end or not.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_EnablePreviewPipe(MMP_UBYTE ubSnrSel, MMP_BOOL bEnable, MMP_BOOL bCheckFrameEnd)
{
    MMP_DISPLAY_WIN_ID  winId = m_VidRecdConfigs.previewdata[0].DispWinId[m_VidRecdModes.usVideoPreviewMode];
    MMP_UBYTE		    ubVifId = MMPD_Sensor_GetVIFPad(ubSnrSel);
    MMP_ERR sRet = MMP_ERR_NONE;
    
    if (!(bEnable ^ m_bVidPreviewActive[ubSnrSel])) {
        return MMP_ERR_NONE;
    }

    if (bEnable) {
        // Reset all pipe
        MMP_UBYTE i = 0;

        for (i = MMP_IBC_PIPE_0; i < MMP_IBC_PIPE_MAX; i++) {
            sRet = MMPD_Icon_ResetModule(i);
            sRet = MMPD_IBC_ResetModule(i);
        }

        if(sRet != MMP_ERR_NONE){MMP_PRINT_RET_ERROR(0, sRet, "",gubMmpDbgBk); return sRet;}

        sRet = MMPD_VIDENC_EnableTimer(bEnable);
        if(sRet != MMP_ERR_NONE){MMP_PRINT_RET_ERROR(0, sRet, "",gubMmpDbgBk); return sRet;}

        switch(m_VidRecdConfigs.previewpath[0]) {
            case MMP_3GP_PATH_2PIPE:
            sRet = MMPD_Fctl_EnablePreview(ubSnrSel, m_PreviewFctlLink.ibcpipeID, bEnable, bCheckFrameEnd);
            break;
#if (SUPPORT_LDC_RECD)
            case MMP_3GP_PATH_LDC_LB_SINGLE:
            case MMP_3GP_PATH_LDC_LB_MULTI:
            case MMP_3GP_PATH_LDC_MULTISLICE:
            sRet = MMPD_Fctl_EnablePreview(ubSnrSel, m_LdcSrcFctlLink.ibcpipeID,  bEnable, MMP_FALSE); 
            sRet |= MMPD_Fctl_EnablePreview(ubSnrSel, m_PreviewFctlLink.ibcpipeID, bEnable, MMP_FALSE);
            break;
#endif
        }
        
        if(sRet != MMP_ERR_NONE){MMP_PRINT_RET_ERROR(0, sRet, "",gubMmpDbgBk); return MMP_3GPRECD_ERR_GENERAL_ERROR;}            

#if (SUPPORT_VR_WIFI_STREAM)
        sRet = MMPD_Fctl_EnablePreview(ubSnrSel, m_StreamingFctlLink.ibcpipeID, bEnable, MMP_FALSE);
#endif
#if (SUPPORT_MDTC)||(SUPPORT_ADAS)
        sRet |= MMPD_Fctl_EnablePreview(ubSnrSel, m_MdtcFctlLink.ibcpipeID, bEnable, MMP_FALSE);
#endif
#if 0//(SUPPORT_DEC_MJPEG_TO_PREVIEW) //TBD 
#if (CCIR656_FORCE_SEL_BT601)//Workaround for MMPF_Display_FrameDoneTrigger handle
        sRet |= MMPD_Fctl_EnablePreview(SCD_SENSOR, m_DecMjpegToPrevwFctlLink.ibcpipeID, bEnable, MMP_FALSE);
#else
        sRet |= MMPD_Fctl_EnablePreview(ubSnrSel, m_DecMjpegToPrevwFctlLink.ibcpipeID, bEnable, MMP_FALSE); 
#endif
#endif
        if(sRet != MMP_ERR_NONE){MMP_PRINT_RET_ERROR(0, sRet, "",gubMmpDbgBk); return sRet;}            
    }
    else 
    {
        sRet = MMPS_3GPRECD_StopAllPipeZoom();
#if 0//(SUPPORT_DEC_MJPEG_TO_PREVIEW) //TBD 
        sRet |= MMPD_Fctl_EnablePreview(ubSnrSel, m_DecMjpegToPrevwFctlLink.ibcpipeID, bEnable, MMP_FALSE); 
#endif

#if (SUPPORT_FDTC)
        if (m_VidRecdConfigs.bFdtcEnable && MMPS_Sensor_GetFDEnable()) {
            sRet |= MMPS_Sensor_SetFDEnable(MMP_FALSE);
        }
#endif
        if(sRet != MMP_ERR_NONE){MMP_PRINT_RET_ERROR(0, sRet, "",gubMmpDbgBk); return sRet;}            

#if (SUPPORT_MDTC)
        if (MMPS_Sensor_IsVMDEnable()) {
            sRet = MMPS_Sensor_EnableVMD(MMP_FALSE);
            if(sRet != MMP_ERR_NONE){MMP_PRINT_RET_ERROR(0, sRet, "",gubMmpDbgBk); return sRet;}                        
        }
#endif
#if (SUPPORT_ADAS)
        if (MMPS_Sensor_IsADASEnable()) {
            sRet = MMPS_Sensor_EnableADAS(MMP_FALSE);
            if(sRet != MMP_ERR_NONE){MMP_PRINT_RET_ERROR(0, sRet, "",gubMmpDbgBk); return sRet;}                        
        }
#endif

        switch(m_VidRecdConfigs.previewpath[0]) {
            case MMP_3GP_PATH_2PIPE:
            sRet = MMPD_Fctl_EnablePreview(ubSnrSel, m_PreviewFctlLink.ibcpipeID, bEnable, MMP_TRUE);
            break;
#if (SUPPORT_LDC_RECD)
            case MMP_3GP_PATH_LDC_LB_SINGLE:
            case MMP_3GP_PATH_LDC_LB_MULTI:
            case MMP_3GP_PATH_LDC_MULTISLICE:
            sRet = MMPD_Fctl_EnablePreview(ubSnrSel, m_PreviewFctlLink.ibcpipeID, bEnable, MMP_TRUE);
            sRet |= MMPD_Fctl_EnablePreview(ubSnrSel, m_LdcSrcFctlLink.ibcpipeID,  bEnable, MMP_TRUE);
            break;
#endif
        }

        if(sRet != MMP_ERR_NONE){MMP_PRINT_RET_ERROR(0, sRet, "",gubMmpDbgBk); return MMP_3GPRECD_ERR_GENERAL_ERROR;}                                    

#if (SUPPORT_VR_WIFI_STREAM)
        sRet = MMPD_Fctl_EnablePreview(ubSnrSel, m_StreamingFctlLink.ibcpipeID, bEnable, MMP_TRUE);
#endif
#if (SUPPORT_MDTC)||(SUPPORT_ADAS)
        sRet |= MMPD_Fctl_EnablePreview(ubSnrSel, m_MdtcFctlLink.ibcpipeID, bEnable, MMP_TRUE);
#endif

        sRet |= MMPD_VIDENC_EnableTimer(MMP_FALSE);

#if defined(SW2_CAPTURE_MODE_SHOW_BG)&&(SW2_CAPTURE_MODE_SHOW_BG)
        sRet |= MMPD_Display_SetWinActive(winId, MMP_FALSE);
#endif
        
        if(sRet != MMP_ERR_NONE){MMP_PRINT_RET_ERROR(0, sRet, "",gubMmpDbgBk); return sRet;}                                    

        if (m_VidRecdConfigs.bRawPreviewEnable[0]) {
            sRet = MMPD_RAWPROC_EnablePreview(ubVifId, MMP_FALSE);

            if (m_VidRecdConfigs.ubRawPreviewBitMode[0] == MMP_RAW_COLORFMT_YUV420 ||
                m_VidRecdConfigs.ubRawPreviewBitMode[0] == MMP_RAW_COLORFMT_YUV422) {
                sRet |= MMPD_RAWPROC_EnablePath(ubSnrSel,
                            ubVifId,
                            MMP_FALSE,
                            MMP_RAW_IOPATH_VIF2RAW,
                            m_VidRecdConfigs.ubRawPreviewBitMode[0]);
            }
            else {    
                sRet |= MMPD_RAWPROC_EnablePath(ubSnrSel,
                            ubVifId,
                            MMP_FALSE,
                            MMP_RAW_IOPATH_VIF2RAW | MMP_RAW_IOPATH_RAW2ISP,
                            m_VidRecdConfigs.ubRawPreviewBitMode[0]);
            }
            if(sRet != MMP_ERR_NONE){MMP_PRINT_RET_ERROR(0, sRet, "",gubMmpDbgBk); return sRet;}                                                
        }

        sRet |= MMPD_VIDENC_TuneMCIPriority(MMPD_VIDENC_MCI_DEFAULT);
        if(sRet != MMP_ERR_NONE){MMP_PRINT_RET_ERROR(0, sRet, "",gubMmpDbgBk); return sRet;}                                                        
    }

    m_bVidPreviewActive[ubSnrSel] = bEnable;

    if (bEnable == MMP_FALSE) {
        m_usVidStaticZoomIndex = 0;
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_PreviewStop
//  Description : Stop preview display mode.
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_PreviewStop(MMP_UBYTE ubSnrSel)
{
	if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_INVALID) {
		return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
	}

    MMPS_3GPRECD_EnablePreviewPipe(ubSnrSel, MMP_FALSE, MMP_FALSE);
    MMPS_3GPRECD_EnableRecordPipe(MMP_FALSE);

    if (m_VidRecdModes.VidCurBufMode[0] == MMPS_3GPRECD_CURBUF_RT) {
        MMPD_IBC_SetH264RT_Enable(MMP_FALSE);
    }
   	
    /* This is a work around. After real-time H264 encode,
     * we have to reset IBC to avoid a corrupted still JPEG later.
     * Here we reset all of IBC pipes, instead of H.264 pipe only.
     */
    #if (MCR_V2_UNDER_DBG)
    MMPD_IBC_ResetModule(m_RecordFctlLink.ibcpipeID);
    MMPD_IBC_ResetModule(m_PreviewFctlLink.ibcpipeID);
    MMPD_IBC_ResetModule(m_StreamingFctlLink.ibcpipeID);
	#if (SUPPORT_LDC_RECD)
    MMPD_IBC_ResetModule(m_LdcSrcFctlLink.ibcpipeID);
	#endif
    #endif

    #if (SUPPORT_LDC_RECD)
    MMPD_Fctl_UnLinkPipeToLdc(m_LdcSrcFctlLink.ibcpipeID);
    #endif

	if (gsHdrCfg.bEnable) {
		MMPD_System_ResetHModule(MMPD_SYS_MDL_VIF0, 	MMP_FALSE);
		MMPD_System_ResetHModule(MMPD_SYS_MDL_RAW_S0, 	MMP_TRUE);
		MMPD_System_ResetHModule(MMPD_SYS_MDL_RAW_F, 	MMP_TRUE);
		MMPD_System_ResetHModule(MMPD_SYS_MDL_ISP, 		MMP_FALSE);	

		MMPD_HDR_UnInitModule(ubSnrSel);
	}
	
	#if (SUPPORT_LDC_RECD)
	m_VidRecdConfigs.previewpath[0] = MMP_3GP_PATH_INVALID;
	#endif

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_PreviewStart
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set buffers and parameters. Then display preview mode.
 @param[in] bCheckFrameEnd 	The setting will check VIF frame end or not.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS Unsupport resolution.
*/
MMP_ERR MMPS_3GPRECD_PreviewStart(MMP_UBYTE ubSnrSel, MMP_BOOL bCheckFrameEnd)
{
    MMP_ERR     err;
    MMP_ULONG   ulStackAddr = 0;
	
	if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_INVALID) {
		return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
	}

	MMPS_Sensor_GetMemEnd(&ulStackAddr);
    m_ulVidRecDramEndAddr = m_ulVideoPreviewEndAddr = ulStackAddr;

    #if 0//(SUPPORT_DUAL_SNR)
	if (m_VidRecdConfigs.previewpath[1] == MMP_3GP_PATH_INVALID) {
		return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
	}

    MMPS_3GPRECD_AdjustPreviewRes(SCD_SENSOR);

	m_VidRecdConfigs.bRawPreviewEnable[1] = MMP_TRUE;
    
    if (MMPS_3GPRECD_Set2ndSnrPreviewMemory(SCD_SENSOR,
                                            m_ulVRPreviewW[SCD_SENSOR],
                                            m_ulVRPreviewH[SCD_SENSOR],
                                            &ulStackAddr))
    {
        PRINTF("Alloc mem for 2nd preview failed\r\n");
        return MMP_3GPRECD_ERR_MEM_EXHAUSTED;
    }
    #endif
	
	MMPS_3GPRECD_AdjustPreviewRes(ubSnrSel);

	m_ubVidRecModeSnrId = ubSnrSel;
	
	/* Only Raw path support PTZ or HDR functions */
	if (MMP_IsVidPtzEnable() || MMP_IsHDREnable()) {
		m_VidRecdConfigs.bRawPreviewEnable[0] = MMP_TRUE;
	}

    ulStackAddr = m_ulVidRecDramEndAddr = m_ulVideoPreviewEndAddr;
    MMPD_System_GetSramEndAddr(&m_ulVidRecSramAddr);

    #if (SUPPORT_LDC_RECD)
	if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_LB_MULTI ||
		m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_MULTISLICE) {
	    err = MMPS_3GPRECD_SetPreviewMemory(ubSnrSel,
                                            m_ulLdcMaxOutWidth,
                                            m_ulLdcMaxOutHeight,
                                            &ulStackAddr,
                                            &m_ulVidRecSramAddr);
    }
    else
    #endif
    {
        err = MMPS_3GPRECD_SetPreviewMemory(ubSnrSel,
                                            m_ulVRPreviewW[ubSnrSel],
                                            m_ulVRPreviewH[ubSnrSel],
                                            &ulStackAddr,
                                            &m_ulVidRecSramAddr);
    }

    if (err) {
        PRINTF("Alloc mem for video preview failed\r\n");
        return err;
    }
    //MMPF_OS_Sleep(200);// Remove for faster preview.//CZ patch...20160204
    
    #if 0//(SUPPORT_DUAL_SNR)
    if (MMPS_3GPRECD_Enable2ndSnrPreviewPipe(SCD_SENSOR, MMP_TRUE, bCheckFrameEnd) != MMP_ERR_NONE) {
		PRINTF("Enable Video Preview: Fail\r\n");
        return MMP_3GPRECD_ERR_GENERAL_ERROR;
    }
    #endif
    
    if (MMPS_3GPRECD_EnablePreviewPipe(ubSnrSel, MMP_TRUE, bCheckFrameEnd) != MMP_ERR_NONE) {
		PRINTF("Enable Video Preview: Fail\r\n");
        return MMP_3GPRECD_ERR_GENERAL_ERROR;
    }
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetPreviewMemory
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set memory map for VideoR preview
 @param[in] usPreviewW Preview width.
 @param[in] usPreviewH Preview height.
 @param[in/out] ulStackAddr Available start address of dram buffer.
 @retval MMP_ERR_NONE Success.
*/
static MMP_ERR MMPS_3GPRECD_SetPreviewMemory(MMP_UBYTE ubSnrSel,
                                             MMP_USHORT usPreviewW,  MMP_USHORT usPreviewH, 
									  		 MMP_ULONG *ulStackAddr, MMP_ULONG *ulFbAddr) 
{
#define MAX_3GPRECD_FILENAME_LENGTH (512)

	MMP_USHORT						usModeIdx = m_VidRecdModes.usVideoPreviewMode; 
    MMP_USHORT                      i;
    MMP_ULONG                       ulCurAddr = *ulStackAddr; 
    MMP_ULONG                       ulTmpBufSize;
    MMP_ULONG                       ulFilenameAddr;
    MMP_ULONG                       ulPreviewBufSize    = 0;
    MMP_ULONG                       ulRawBufSize        = 0;
    MMP_ULONG                       ulLdcSrcBufSize     = 0;
    //MMP_ULONG                       ulMdtcSrcBufSize    = 0;
    MMP_ULONG                       ulCaptureMaxBufSize;
	#if (UVC_VIDRECD_SUPPORT)||(SUPPORT_USB_HOST_FUNC)
    MMP_ULONG                       ulPreviewUVCMaxBufSize;
    #endif
    MMPS_3GPRECD_PREVIEW_DATA       *previewdata = &m_VidRecdConfigs.previewdata[0];
    MMPS_3GPRECD_PREVIEW_BUFINFO    previewbuf;
    MMP_RAW_STORE_BUF               rawbuf;
    MMP_LDC_SRC_BUFINFO				ldcSrcBuf;
    MMPS_3GPRECD_MDTC_BUFINFO       MdtcSrcBuf;
    MMP_UBYTE				        ubVifId = MMPD_Sensor_GetVIFPad(ubSnrSel);
    MMP_USHORT                      usOrigPreviewW = usPreviewW;
    MMP_USHORT                      usOrigPreviewH = usPreviewH;
    
    #if (VIDRECD_REFIXRECD_SUPPORT)
    MMP_USHORT                      FixedTailInfoTempBuf  = 2048;
    MMP_ULONG                       AVRepackBufSize       = 512 * 1024;
    MMP_ULONG                       ulTmpAddr[3];
    #endif
	#if (DUALENC_SUPPORT == 1)
	MMP_ULONG                 		MVBufSize             = ((VR_MAX_RECORD_WIDTH>>4) * (VR_MAX_RECORD_HEIGHT>>4)) * 16 * 4; //MBs * (Blocks per MB)* (4 Bytes)
	#endif

    ///////////////////////////////////////////////////////////////////////////
    //
    //  Get Preview Config setting and calculate how many memory will be used    
    //
    ///////////////////////////////////////////////////////////////////////////
    
    // Get Preview buffer config and size
    previewbuf.usBufCnt = previewdata->usVidDispBufCnt[usModeIdx];

    #if (SUPPORT_LDC_RECD)
    if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_LB_MULTI) {
    	if ((MMPF_LDC_MultiRunGetMaxLoopBackCount() + 1) > previewdata->usVidDispBufCnt[usModeIdx])
    		previewbuf.usBufCnt = (MMPF_LDC_MultiRunGetMaxLoopBackCount() + 1);	
    }
    else if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_MULTISLICE) {
    	//TBD
    }
    #endif

    if (previewdata->bUseRotateDMA[usModeIdx])
		previewbuf.usRotateBufCnt = previewdata->usRotateBufCnt[usModeIdx];
	else
		previewbuf.usRotateBufCnt = 0;

    #if (HANDLE_JPEG_EVENT_BY_QUEUE)
    // These parameters are for allocate preview/rotate buffer 
    usPreviewW = (usOrigPreviewW > m_usMJPEGMaxEncWidth) ? (usOrigPreviewW) : (m_usMJPEGMaxEncWidth);
    usPreviewH = (usOrigPreviewH > m_usMJPEGMaxEncHeight) ? (usOrigPreviewH) : (m_usMJPEGMaxEncHeight);
    #endif

    switch(previewdata->DispColorFmt[usModeIdx]) {
    case MMP_DISPLAY_COLOR_YUV420:
        ulPreviewBufSize = ALIGN32(usPreviewW * usPreviewH) + ALIGN32((usPreviewW*usPreviewH)>>2)*2;
        break;
    case MMP_DISPLAY_COLOR_YUV420_INTERLEAVE:
        ulPreviewBufSize = ALIGN32(usPreviewW * usPreviewH) + ALIGN32(usPreviewW*(usPreviewH>>1));
        break;
    case MMP_DISPLAY_COLOR_YUV422:
    case MMP_DISPLAY_COLOR_RGB565:
        ulPreviewBufSize = ALIGN32(usPreviewW * 2 * usPreviewH);
        break;
    case MMP_DISPLAY_COLOR_RGB888:
        ulPreviewBufSize = ALIGN32(usPreviewW * 3 * usPreviewH);
        break;
    default:
    	PRINTF("Not Support Color Format\r\n");
        return MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS;
        break;
    }

    // Get Raw preview buffer config and size
	if (MMP_TRUE == m_VidRecdConfigs.bRawPreviewEnable[0]) {

		MMP_USHORT	usSensorW;
    	MMP_USHORT	usSensorH;

        if (m_VidRecdConfigs.ulRawStoreBufCnt > VR_MAX_RAWSTORE_BUFFER_NUM) {
            m_VidRecdConfigs.ulRawStoreBufCnt = VR_MAX_RAWSTORE_BUFFER_NUM;
        }

        rawbuf.ulRawBufCnt = m_VidRecdConfigs.ulRawStoreBufCnt;

		MMPD_RAWPROC_GetStoreRange(ubVifId, &usSensorW, &usSensorH);

		if (gsHdrCfg.bEnable) {

			if (m_VidRecdConfigs.ulRawStoreBufCnt < VR_MIN_RAWSTORE_BUFFER_NUM) {
                rawbuf.ulRawBufCnt = m_VidRecdConfigs.ulRawStoreBufCnt = VR_MIN_RAWSTORE_BUFFER_NUM;
            }

			/* Plus 256 for buffer address alignment */
			if (gsHdrCfg.ubRawStoreBitMode == HDR_BITMODE_10BIT)
				ulRawBufSize = (2 * usSensorW * usSensorH * 4 / 3) + 256;
			else
				ulRawBufSize = (2 * usSensorW * usSensorH) + 256;
		}
		else {

			if (m_VidRecdConfigs.ubRawPreviewBitMode[0] == MMP_RAW_COLORFMT_BAYER10)
				ulRawBufSize = ALIGN32(usSensorW * usSensorH * 4 / 3);
			else
				ulRawBufSize = ALIGN32(usSensorW * usSensorH);
    	}
    }
    else {
        rawbuf.ulRawBufCnt = 0;
        ulRawBufSize = 0;
    }
	
	// Get Graphics loopback source buffer config and size
	#if (SUPPORT_LDC_RECD)
	if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_LB_SINGLE   ||
	    m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_LB_MULTI    ||
	    m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_MULTISLICE) {
		ldcSrcBuf.usInBufCnt 	= 2;
		ulLdcSrcBufSize 		= ALIGN32(m_ulLdcMaxSrcWidth * m_ulLdcMaxSrcHeight * 3 / 2);
	}
	else 
	#endif
	{
		ldcSrcBuf.usInBufCnt    = 0;
		ulLdcSrcBufSize         = 0;
	}

	// Get Motion detection buffer config and size
	#if (SUPPORT_MDTC)||(SUPPORT_ADAS)
    if (m_YFrameType == MMPS_3GPRECD_Y_FRAME_TYPE_ADAS) {
        #if (SUPPORT_ADAS)
        MMPS_Sensor_GetADASResolution(&gsADASFitRange.ulOutWidth, &gsADASFitRange.ulOutHeight);
        #endif
    }
    else {
    	#if (SUPPORT_MDTC)
        MMPS_Sensor_GetVMDResolution(&gsADASFitRange.ulOutWidth, &gsADASFitRange.ulOutHeight);
    	#endif
    }

    MdtcSrcBuf.usBufCnt = 2;
    //ulMdtcSrcBufSize    = ALIGN32(gsADASFitRange.ulOutWidth * gsADASFitRange.ulOutHeight);
    #else
    MdtcSrcBuf.usBufCnt = 0;
    //ulMdtcSrcBufSize    = 0;
    #endif

    //////////////////////////////////////////////////////////////////////////
    //
    //  Allocate memory for preview buffer
    //
    //////////////////////////////////////////////////////////////////////////

	ulCurAddr = ALIGN32(*ulStackAddr);
    MMPD_VIDENC_SetMemStart(ulCurAddr);

	// Allocate memory for preview buffer
    for (i = 0; i < previewbuf.usBufCnt; i++) {
		if (previewdata->DispColorFmt[usModeIdx] == MMP_DISPLAY_COLOR_YUV420) {
    		ulTmpBufSize = usPreviewW * usPreviewH;

            previewbuf.ulYBuf[i] = ulCurAddr;
            previewbuf.ulUBuf[i] = previewbuf.ulYBuf[i] + ALIGN32(ulTmpBufSize);
            previewbuf.ulVBuf[i] = previewbuf.ulUBuf[i] + ALIGN32(ulTmpBufSize >> 2);

            ulCurAddr += (ALIGN32(ulTmpBufSize) + ALIGN32(ulTmpBufSize>>2)*2);
        }
        else if (previewdata->DispColorFmt[usModeIdx] == MMP_DISPLAY_COLOR_YUV420_INTERLEAVE) {
    		ulTmpBufSize = usPreviewW * usPreviewH;

    		previewbuf.ulYBuf[i] = ulCurAddr;
            previewbuf.ulUBuf[i] = previewbuf.ulYBuf[i] + ALIGN32(ulTmpBufSize);
            previewbuf.ulVBuf[i] = previewbuf.ulUBuf[i];

            ulCurAddr += (ALIGN32(ulTmpBufSize) + ALIGN32(ulTmpBufSize>>1));
        }
		else if (previewdata->DispColorFmt[usModeIdx] == MMP_DISPLAY_COLOR_YUV422 ||
				 previewdata->DispColorFmt[usModeIdx] == MMP_DISPLAY_COLOR_RGB565) {
    		ulTmpBufSize = ALIGN32(usPreviewW * 2 * usPreviewH);

    		previewbuf.ulYBuf[i] = ulCurAddr;
            previewbuf.ulUBuf[i] = 0;
            previewbuf.ulVBuf[i] = 0;

            ulCurAddr += ulTmpBufSize;
		}
        else if (previewdata->DispColorFmt[usModeIdx] == MMP_DISPLAY_COLOR_RGB888) {
    		ulTmpBufSize = ALIGN32(usPreviewW * 3 * usPreviewH);

    		previewbuf.ulYBuf[i] = ulCurAddr;
            previewbuf.ulUBuf[i] = 0;
            previewbuf.ulVBuf[i] = 0;

            ulCurAddr += ulTmpBufSize;
		}
		else {
		    PRINTF("Not supported preview format\r\n");  
		    return MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS;
		}
	}

    // Allocate memory for rotate DMA destination buffer
	for (i = 0; i < previewbuf.usRotateBufCnt; i++) {
	
		if (previewdata->DispColorFmt[usModeIdx] == MMP_DISPLAY_COLOR_YUV420) {
		    ulTmpBufSize = usPreviewW * usPreviewH;
		       
		    previewbuf.ulRotateYBuf[i] = ALIGN32(ulCurAddr);
            previewbuf.ulRotateUBuf[i] = previewbuf.ulRotateYBuf[i] + ALIGN32(ulTmpBufSize);
            previewbuf.ulRotateVBuf[i] = previewbuf.ulRotateUBuf[i] + ALIGN32(ulTmpBufSize >> 2);

            ulCurAddr += (ALIGN32(ulTmpBufSize) + ALIGN32(ulTmpBufSize>>2)*2);
	    }
		else if (previewdata->DispColorFmt[usModeIdx] == MMP_DISPLAY_COLOR_YUV420_INTERLEAVE) {
		    ulTmpBufSize = usPreviewW * usPreviewH;

		    previewbuf.ulRotateYBuf[i] = ALIGN32(ulCurAddr);
            previewbuf.ulRotateUBuf[i] = previewbuf.ulRotateYBuf[i] + ALIGN32(ulTmpBufSize);
            previewbuf.ulRotateVBuf[i] = previewbuf.ulRotateUBuf[i];
 
            ulCurAddr += (ALIGN32(ulTmpBufSize) + ALIGN32(ulTmpBufSize>>1));
	    }
		else if (previewdata->DispColorFmt[usModeIdx] == MMP_DISPLAY_COLOR_YUV422 ||
				 previewdata->DispColorFmt[usModeIdx] == MMP_DISPLAY_COLOR_RGB565) {
		    ulTmpBufSize = ALIGN32(usPreviewW * 2 * usPreviewH);
		    
		    previewbuf.ulRotateYBuf[i] = ALIGN32(ulCurAddr);
            previewbuf.ulRotateUBuf[i] = 0;
            previewbuf.ulRotateVBuf[i] = 0;

            ulCurAddr += ulTmpBufSize;
		}
        else if (previewdata->DispColorFmt[usModeIdx] == MMP_DISPLAY_COLOR_RGB888) {
		    ulTmpBufSize = ALIGN32(usPreviewW * 3 * usPreviewH);
		    
		    previewbuf.ulRotateYBuf[i] = ALIGN32(ulCurAddr);
            previewbuf.ulRotateUBuf[i] = 0;
            previewbuf.ulRotateVBuf[i] = 0;

            ulCurAddr += ulTmpBufSize;
		}
		else {
		    PRINTF("Not supported preview format\r\n");  
		    return MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS;
		}
	}

	// Allocate memory for raw preview buffer (include HDR)
    if (m_VidRecdConfigs.bRawPreviewEnable[0]) {

    	MMP_USHORT usStoreW, usStoreH;

	    if (gsHdrCfg.bEnable)
		{
			/* Use 256Bytes alignment address for HDR store buffer */
	        for (i = 0; i < rawbuf.ulRawBufCnt; i++) {
			    rawbuf.ulRawBufAddr[i] = ALIGN256(ulCurAddr);
			    ulCurAddr = rawbuf.ulRawBufAddr[i] + ulRawBufSize;
		    }

		    MMPD_RAWPROC_SetStoreBuf(ubVifId, &rawbuf);

		    MMPD_RAWPROC_EnablePreview(ubVifId, MMP_TRUE);

	        MMPD_RAWPROC_GetStoreRange(ubVifId, &usStoreW, &usStoreH);
	        MMPD_RAWPROC_SetFetchRange(0, 0, usStoreW * 2, usStoreH, usStoreW * 2);

	        MMPD_HDR_InitModule(usStoreW, usStoreH);
			MMPD_HDR_SetBufEnd(ALIGN32(ulCurAddr));

			if (gsHdrCfg.ubRawStoreBitMode == HDR_BITMODE_10BIT)
		        MMPD_RAWPROC_EnablePath(ubSnrSel,
		                                ubVifId,
		                                MMP_TRUE,
						                MMP_RAW_IOPATH_ISP2RAW | MMP_RAW_IOPATH_RAW2ISP,
						                MMP_RAW_COLORFMT_BAYER10);
			else
		        MMPD_RAWPROC_EnablePath(ubSnrSel,
		                                ubVifId,
		                                MMP_TRUE,
						                MMP_RAW_IOPATH_ISP2RAW | MMP_RAW_IOPATH_RAW2ISP,
						                MMP_RAW_COLORFMT_BAYER8);
		}
	    else
		{
	        for (i = 0; i < rawbuf.ulRawBufCnt; i++) {
			    rawbuf.ulRawBufAddr[i] = ALIGN256(ulCurAddr);
			    ulCurAddr = rawbuf.ulRawBufAddr[i] + ulRawBufSize;
		    }

		    MMPD_RAWPROC_SetStoreBuf(ubVifId, &rawbuf);

	        MMPD_HDR_UnInitModule(ubSnrSel);

	        MMPD_RAWPROC_EnablePreview(ubVifId, MMP_TRUE);

            MMPD_RAWPROC_GetStoreRange(ubVifId, &usStoreW, &usStoreH);
           	MMPD_RAWPROC_SetFetchRange(0, 0, usStoreW, usStoreH, usStoreW);

            if (m_VidRecdConfigs.ubRawPreviewBitMode[0] == MMP_RAW_COLORFMT_YUV420 ||
                m_VidRecdConfigs.ubRawPreviewBitMode[0] == MMP_RAW_COLORFMT_YUV422) {
                MMPD_RAWPROC_EnablePath(ubSnrSel,
                                        ubVifId,
                                        MMP_TRUE,
    					                MMP_RAW_IOPATH_VIF2RAW,
    					                m_VidRecdConfigs.ubRawPreviewBitMode[0]);
            }
            else {
                MMPD_RAWPROC_EnablePath(ubSnrSel,
                                        ubVifId,
                                        MMP_TRUE,
    					                MMP_RAW_IOPATH_VIF2RAW | MMP_RAW_IOPATH_RAW2ISP,
    					                m_VidRecdConfigs.ubRawPreviewBitMode[0]);
            }
        }
        ulCurAddr = ALIGN256(ulCurAddr);
    }
    else {

		MMPD_HDR_UnInitModule(ubSnrSel);

    	MMPD_RAWPROC_EnablePreview(ubVifId, MMP_FALSE);
    }

	// Allocate memory for LDC source buffer (NV12)
	#if (SUPPORT_LDC_RECD)
	ulTmpBufSize = m_ulLdcMaxSrcWidth * m_ulLdcMaxSrcHeight;
	
    for (i = 0; i < ldcSrcBuf.usInBufCnt; i++) {

        ldcSrcBuf.ulInYBuf[i] = ulCurAddr;
        ldcSrcBuf.ulInUBuf[i] = ldcSrcBuf.ulInYBuf[i] + ALIGN32(ulTmpBufSize);
        ldcSrcBuf.ulInVBuf[i] = ldcSrcBuf.ulInUBuf[i];

        ulCurAddr += (ALIGN32(ulTmpBufSize) + ALIGN32(ulTmpBufSize>>1));
	}
	
	ulTmpBufSize = m_ulLdcMaxOutWidth * m_ulLdcMaxOutHeight;
	
	if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_MULTISLICE) {

	    MMPF_LDC_MultiSliceInitOutStoreBuf(ulCurAddr,
	                                       ulCurAddr + ALIGN32(ulTmpBufSize),
	                                       ulCurAddr + ALIGN32(ulTmpBufSize));
        
        MEMSET((void*)ulCurAddr, 0x00, ulTmpBufSize * 3 / 2);
        
	    ulCurAddr += (ALIGN32(ulTmpBufSize) + ALIGN32(ulTmpBufSize>>1));
	}
	#endif
	
	// Allocate memory for Motion detection source buffer (Y only)
	#if (SUPPORT_MDTC)||(SUPPORT_ADAS)
    ulCurAddr = ALIGN32(ulCurAddr);

    ulTmpBufSize = gsADASFitRange.ulOutWidth * gsADASFitRange.ulOutHeight;

    #if (SUPPORT_ADAS)
    if (m_YFrameType == MMPS_3GPRECD_Y_FRAME_TYPE_ADAS) {
        if (MMPS_Sensor_AllocateADASBuffer(&ulCurAddr, MMP_TRUE, ulTmpBufSize) != MMP_ERR_NONE) {
            PRINTF("Allocate LDWS buffer failed\r\n");
            return MMP_SENSOR_ERR_LDWS;
        }
    }
    else
    #endif
    {
    	#if (SUPPORT_MDTC)
        if (MMPS_Sensor_AllocateVMDBuffer(&ulCurAddr, MMP_TRUE) != MMP_ERR_NONE) {
            PRINTF("Allocate MDTC buffer failed\r\n");
            return MMP_SENSOR_ERR_VMD;
        }
        #endif
    }

    for (i = 0; i < MdtcSrcBuf.usBufCnt; i++) {
        MdtcSrcBuf.ulYBuf[i] = ulCurAddr;

        ulCurAddr += ALIGN32(ulTmpBufSize);
	}
	#endif

	// Allocate memory for File name buffer
    ulFilenameAddr = ulCurAddr;
    ulCurAddr += MAX_3GPRECD_FILENAME_LENGTH;

    MMPD_3GPMGR_SetTempFileNameAddr(ulFilenameAddr,
                                    MAX_3GPRECD_FILENAME_LENGTH);
	
	// Allocate memory for FDTC buffer
	#if (SUPPORT_FDTC)
    if (m_VidRecdConfigs.bFdtcEnable) {

        MMPS_Sensor_AllocateFDBuffer(&ulCurAddr,
							         usOrigPreviewW,
							         usOrigPreviewH,
							         MMP_TRUE);
    }
    #endif

	// Allocate memory for wifi stream
	#if (SUPPORT_VR_WIFI_STREAM)
    if (m_VidRecdConfigs.bWifiStreamEnable) {

        MMPS_DSC_SetMJpegEncBuf(&ulCurAddr,
							    m_usMaxStillJpegW,
							    m_usMaxStillJpegH,
							    MMP_TRUE);
    }
    #endif


	// Allocate memory for still capture buffer
    if (m_VidRecdConfigs.bStillCaptureEnable) {

        ulCurAddr = ALIGN32(ulCurAddr);
        m_ulVidRecCaptureDramAddr = ulCurAddr;
        m_ulVidRecCaptureSramAddr = *ulFbAddr;

        // Pre-calculating maximum memory usage
        MMPS_DSC_SetShotMode(MMPS_DSC_SINGLE_SHOTMODE);
        #if (DSC_SUPPORT_BASELINE_MP_FILE)
        MMPS_DSC_EnableMultiPicFormat(MMP_FALSE, MMP_FALSE);
        #endif
        MMPS_DSC_SetJpegEncParam(DSC_RESOL_IDX_UNDEF, m_usMaxStillJpegW, m_usMaxStillJpegH, MMP_DSC_JPEG_RC_ID_CAPTURE);

        #ifdef MCR_V2_32MB
        MMPS_DSC_ConfigThumbnail(0, 0, MMP_DSC_THUMB_SRC_NONE); //Disable Thumbnail to get 800 KB extra DRAM space
        #else
        MMPS_DSC_ConfigThumbnail(VR_MAX_THUMBNAIL_WIDTH, VR_MAX_THUMBNAIL_HEIGHT, MMP_DSC_THUMB_SRC_DECODED_JPEG);
        #endif
        
        MMPS_DSC_SetSystemBuf(&ulCurAddr, MMP_FALSE, MMP_TRUE, MMP_TRUE);
        MMPS_DSC_SetCaptureBuf(ulFbAddr, &ulCurAddr, MMP_TRUE, MMP_FALSE, MMP_DSC_CAPTURE_NO_ROTATE, MMP_TRUE);

        // Maximum memory usage for rear cam capture
        if (m_VidRecdConfigs.bDualCaptureEnable) {
            ulCurAddr = ALIGN32(ulCurAddr);
            m_ulVidRecDualCaptureDramAddr = ulCurAddr;
            ulCurAddr += m_ulVidRecDualCaptureDramSize;
        }
        
        #if (HANDLE_JPEG_EVENT_BY_QUEUE)
        #if (USE_H264_CUR_BUF_AS_CAPT_BUF)
        {
            MMP_ULONG ulEncW, ulEncH;
            
            MMPS_3GPRECD_SetEncodeRes(0);

            ulEncW = m_ulVREncodeW[0];
            ulEncH = m_ulVREncodeH[0];
            
            m_VidRecdInputBuf[0].ulBufCnt = 2;
            
            for (i = 0; i < m_VidRecdInputBuf[0].ulBufCnt; i++) {
                ulTmpBufSize = ulEncW * ulEncH;

                m_VidRecdInputBuf[0].ulY[i] = ALIGN32(ulCurAddr);
                m_VidRecdInputBuf[0].ulU[i] = m_VidRecdInputBuf[0].ulY[i] + ulTmpBufSize;
                m_VidRecdInputBuf[0].ulV[i] = m_VidRecdInputBuf[0].ulU[i];

                ulCurAddr += (ulTmpBufSize * 3) / 2;
            }
            
            m_ulVidRecCaptureFrmBufAddr = m_VidRecdInputBuf[0].ulY[0];
        }
        #else
        // For store front cam capture frame (NV12/I420)
        m_ulVidRecCaptureFrmBufAddr = ulCurAddr;
        m_ulVidRecCaptureFrmBufSize = (VR_MAX_CAPTURE_WIDTH * VR_MAX_CAPTURE_HEIGHT) * 3 / 2;

        ulCurAddr += m_ulVidRecCaptureFrmBufSize;
        MEMSET((MMP_UBYTE*)m_ulVidRecCaptureFrmBufAddr, 0x00, m_ulVidRecCaptureFrmBufSize);
        #endif
        #endif
    }
    else {
        m_ulVidRecCaptureDramAddr = 0;
        ulCaptureMaxBufSize = 0;
    }


	#if 0//(UVC_VIDRECD_SUPPORT)||(SUPPORT_USB_HOST_FUNC)
    // Allocate memory for UVC record
    #if (UVC_VIDRECD_SUPPORT)
    if (gbUVCRecdSupport) 
    #elif (SUPPORT_USB_HOST_FUNC)
    if (1)
    #endif
    {
        ulCurAddr = ALIGN32(ulCurAddr);

        MMPD_3GPMGR_SetUVCFBMemory(ulCurAddr, &ulPreviewUVCMaxBufSize);
        ulCurAddr += ulPreviewUVCMaxBufSize;
    }
    #endif

    #if 0//(SUPPORT_DEC_MJPEG_TO_PREVIEW)
    // Allocate memory for Decode MJPEG to Preview
    {
        MMP_ULONG ulDecPreviewBufSize = 0;
        
        ulCurAddr = ALIGN32(ulCurAddr);

        MMPS_3GPRECD_SetDecMjpegToPreviewBuf(m_sAhcDecMjpegToPreviewInfo.ulPreviewBufW,
                                             m_sAhcDecMjpegToPreviewInfo.ulPreviewBufH,
                                             ulCurAddr,
                                             &ulDecPreviewBufSize,
                                             &m_sDecMjpegPrevwBufInfo);
        ulCurAddr += ulDecPreviewBufSize;
        
        MMPD_DSC_SetDecMjpegToPreviewJpegBuf(ulCurAddr, &ulDecPreviewBufSize);
        ulCurAddr += ulDecPreviewBufSize;
    }
    #endif

    #if (VIDRECD_REFIXRECD_SUPPORT)
    ulCurAddr   = ALIGN32(ulCurAddr);
    ulTmpAddr[0] = ulCurAddr;
    ulCurAddr   += ALIGN32(FixedTailInfoTempBuf); 
    ulTmpAddr[1] = ulCurAddr;
    ulCurAddr   += ALIGN32(AVRepackBufSize); 
    ulTmpAddr[2] = ulCurAddr;
    ulCurAddr   += m_VidRecdConfigs.ulReFixTailBufSize;
    MMPD_3GPMGR_SetTempBuf2FixedTailInfo(tmp_addr[0], FixedTailInfoTempBuf,
                                         tmp_addr[1], AVRepackBufSize,
                                         tmp_addr[2], m_VidRecdConfigs.ulReFixTailBufSize);
	#endif



#if (defined(WIFI_PORT) && WIFI_PORT == 1)
    {
    	//MMP_ULONG	MjpgStreamBufSize;

    	ulCurAddr = ALIGN32(ulCurAddr);
        m_ulVidRecMjpgStreamDramAddr = ulCurAddr;
        m_ulVidRecMjpgStreamSramAddr = *ulFbAddr;

     //   MMPD_DSC_GetBufferSize(MMPD_DSC_ENC_EXTERNAL_COMPRESS_BUFFER, MMP_DSC_JPEG_FMT422,
    //    		MJPEG_STREAM_BUF_W, MJPEG_STREAM_BUF_H, &MjpgStreamBufSize);
      //  printc("\r\nSetPreviewMemory  m_ulVidRecMjpgStreamDramAddr 0x%x ", m_ulVidRecMjpgStreamDramAddr);
        ulCurAddr += 0x100000;
    //    printc("SetPreviewMemory  MJ stream buf end 0x%x \r\n", ulCurAddr);
    }
#endif
	#if (DUALENC_SUPPORT == 1)
	m_ulVidShareMvAddr                  = ulCurAddr;
    ulCurAddr                           += MVBufSize; // 2 bytes per MB
	#endif
    *ulStackAddr = ulCurAddr;
    m_ulVidRecDramEndAddr = m_ulVideoPreviewEndAddr = ulCurAddr;

    #if defined(ALL_FW)
    if (m_ulVidRecDramEndAddr > MMPS_System_GetMemHeapEnd()) {
        printc("\t= [HeapMemErr] Video preview =\r\n");
        return MMP_3GPRECD_ERR_MEM_EXHAUSTED;
    }
    printc("End of 1st video preview buffers = 0x%X\r\n", m_ulVidRecDramEndAddr);
    #endif

    MMPD_VIDENC_SetMemStart(ulCurAddr);

    MMPS_3GPRECD_SetPreviewPipeConfig(&previewbuf, &ldcSrcBuf, &MdtcSrcBuf, usOrigPreviewW, usOrigPreviewH, ubSnrSel);

    return MMP_ERR_NONE;
}

MMP_ERR MMPS_3GPRECD_SetUVCFBMemory(void)
{
#if (SUPPORT_USB_HOST_FUNC)    
    MMP_ERR sRet = MMP_ERR_NONE;
    MMP_ULONG ulPreviewUVCMaxBufSize = 0, ulCurAddr = 0;
    MMP_UBYTE ubUVCFBMemSet = 0;
    MMP_UBYTE ubEncIdx = 0;
    MMP_BOOL bEnable = MMP_FALSE;

    for(ubEncIdx = 0 ;ubEncIdx < VR_MAX_ENCODE_NUM; ++ubEncIdx){      
        sRet = MMPS_3GPRECD_GetRecordPipeStatus(ubEncIdx, &bEnable);   
        if(bEnable != MMP_FALSE){
            MMP_PRINT_RET_ERROR(0, sRet, "Can not set UVC FB memory when recording!",gubMmpDbgBk);
            return MMP_SYSTEM_ERR_NOT_SUPPORT;
        }
    }
    
    sRet = MMPF_USBH_IsSetFBMemory(&ubUVCFBMemSet);
    if(1 == ubUVCFBMemSet){
        RTNA_DBG_Str(0, FG_GREEN("MMPS_3GPRECD_SetUVCFBMemory has been set before!\r\n"));
        return sRet;
    }

    // Allocate memory for UVC record    
    ulCurAddr = (m_ulVidRecDramEndAddr >= m_ulVideoPreviewEndAddr) ? m_ulVidRecDramEndAddr : m_ulVideoPreviewEndAddr;  
    ulCurAddr = ALIGN32(ulCurAddr);

    sRet = MMPD_3GPMGR_SetUVCFBMemory(ulCurAddr, &ulPreviewUVCMaxBufSize);
    ulCurAddr += ulPreviewUVCMaxBufSize;   

    if(sRet != MMP_ERR_NONE){MMP_PRINT_RET_ERROR(0, sRet, "SetUVCFBMemory error!",gubMmpDbgBk); return sRet;}

    m_ulVidRecDramEndAddr = m_ulVideoPreviewEndAddr = ulCurAddr;

#if defined(ALL_FW)
    if (m_ulVidRecDramEndAddr > MMPS_System_GetMemHeapEnd()) {
        printc("\t= [HeapMemErr] UVC rear FB Memory =\r\n");
        return MMP_3GPRECD_ERR_MEM_EXHAUSTED;
    }
    printc("End of UVC rear FB Memory = 0x%X\r\n", m_ulVidRecDramEndAddr);
#endif

    MMPD_VIDENC_SetMemStart(ulCurAddr);
    
    return sRet;
#else
    MMP_PRINT_RET_ERROR(0, 0, "SetUVCFBMemory not support!",gubMmpDbgBk);
    return MMP_ERR_NONE;
#endif
}

MMP_ERR MMPS_3GPRECD_SetUVCRearMJPGMemory(void)
{
#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
    MMP_ERR sRet = MMP_ERR_NONE;

    // Allocate memory for Decode MJPEG to Preview
    MMP_ULONG ulDecPreviewBufSize = 0, ulCurAddr;
    //extern MMP_ULONG    m_ulVidRecCurMemAddr;
    MMP_UBYTE ubMjpegToPreviewJpegSet = 0;
    MMP_UBYTE ubEncIdx = 0;
    MMP_BOOL bEnable = MMP_FALSE;

    for(ubEncIdx = 0 ;ubEncIdx < VR_MAX_ENCODE_NUM; ++ubEncIdx){      
        sRet = MMPS_3GPRECD_GetRecordPipeStatus(ubEncIdx, &bEnable);   
        if(bEnable != MMP_FALSE){
            MMP_PRINT_RET_ERROR(0, sRet, "Can not set UVC Rear MJPG Memory when recording!",gubMmpDbgBk);
            return MMP_SYSTEM_ERR_NOT_SUPPORT;
        }
    }
    
    sRet = MMPF_USBH_IsSetDecMjpegToPreviewJpegBuf(&ubMjpegToPreviewJpegSet);
    if(1 == ubMjpegToPreviewJpegSet){

        MMPF_USBH_InitDecMjpegToPreviewJpegBuf();

        RTNA_DBG_Str(0, FG_GREEN("MMPS_3GPRECD_SetUVCRearMJPGMemory has been set before!\r\n"));
        return sRet;
    }

    if((0 == m_sAhcDecMjpegToPreviewInfo.ulPreviewBufW) || 0 == m_sAhcDecMjpegToPreviewInfo.ulPreviewBufH){
        MMP_PRINT_RET_ERROR(0, 0, "Parameter error!",gubMmpDbgBk);
        sRet = MMP_SYSTEM_ERR_PARAMETER;
        return sRet;
    }
    
    ulCurAddr = (m_ulVidRecDramEndAddr >= m_ulVideoPreviewEndAddr) ? m_ulVidRecDramEndAddr : m_ulVideoPreviewEndAddr;  
    //ulCurAddr = m_ulVidRecCurMemAddr;
    ulCurAddr = ALIGN32(ulCurAddr);

    sRet = MMPS_3GPRECD_SetDecMjpegToPreviewBuf(m_sAhcDecMjpegToPreviewInfo.ulPreviewBufW,
                            m_sAhcDecMjpegToPreviewInfo.ulPreviewBufH,
                            ulCurAddr,
                            &ulDecPreviewBufSize,
                            &m_sDecMjpegPrevwBufInfo);
    
    ulCurAddr += ulDecPreviewBufSize;

    sRet = MMPD_DSC_SetDecMjpegToPreviewJpegBuf(ulCurAddr, &ulDecPreviewBufSize);
    ulCurAddr += ulDecPreviewBufSize;

    if(sRet != MMP_ERR_NONE){MMP_PRINT_RET_ERROR(0, sRet, "SetMJPG Memory error!",gubMmpDbgBk); return sRet;}

    m_ulVidRecDramEndAddr = m_ulVideoPreviewEndAddr = ulCurAddr;

#if defined(ALL_FW)
    if (m_ulVidRecDramEndAddr > MMPS_System_GetMemHeapEnd()) {
        printc("\t= [HeapMemErr] UVC rear MJPG Video preview =\r\n");
        return MMP_3GPRECD_ERR_MEM_EXHAUSTED;
    }
    printc("End of UVC rear MJPG video preview buffers = 0x%X\r\n", m_ulVidRecDramEndAddr);
#endif

    MMPD_VIDENC_SetMemStart(ulCurAddr);
    
    return sRet;
#else
    MMP_PRINT_RET_ERROR(0, 0, "SetMJPG Memory not support!",gubMmpDbgBk);
    return MMP_ERR_NONE;
#endif
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetPreviewEndAddr
//  Description :
//------------------------------------------------------------------------------
MMP_ULONG MMPS_3GPRECD_GetPreviewEndAddr(void)
{
	return m_ulVideoPreviewEndAddr;
}

#if 0
void ____VR_2nd_Preview_Function____(){ruturn;} //dummy
#endif

#if (SUPPORT_DUAL_SNR)
//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_Set2ndSnrPreviewPipeConfig
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set pipe config.
 @param[in] *pPreviewBuf 	Pointer to Preview buffer.
 @param[in] usPreviewW 		The preview buffer width.
 @param[in] usPreviewH 		The preview buffer height.
 @retval MMP_ERR_NONE Success.
*/
static MMP_ERR MMPS_3GPRECD_Set2ndSnrPreviewPipeConfig( MMPS_3GPRECD_PREVIEW_BUFINFO 	*pPreviewBuf,
										  		        MMP_USHORT 					    usPreviewW, 
										  		        MMP_USHORT 					    usPreviewH,
										  		        MMP_UBYTE                       ubSnrSel)
{
    MMP_USHORT				usModeIdx = m_VidRecdModes.usVideoPreviewMode; 
    MMP_ULONG				ulScalInW, ulScalInH;
	MMP_ULONG          		ulRotateW, ulRotateH;
	MMP_BOOL				bDmaRotateEn;
	MMP_SCAL_FIT_MODE		sFitMode;
    MMP_SCAL_FIT_RANGE		fitrange;
    MMP_SCAL_GRAB_CTRL   	previewGrabctl, DispGrabctl;
    MMP_DISPLAY_DISP_ATTR	dispAttr;
    MMP_DISPLAY_ROTATE_TYPE	ubDmaRotateDir;
    MMPD_FCTL_ATTR  		fctlAttr;
    MMP_USHORT				i;
	MMP_ULONG             	ulDispStartX, ulDispStartY, ulDispWidth, ulDispHeight;
	MMP_SCAL_FIT_RANGE 		sFitRangeBayer;
	MMP_SCAL_GRAB_CTRL		sGrabctlBayer;
	//MMP_USHORT			usCurZoomStep = 0;

	/* Parameter Check */
	if (m_VidRecdConfigs.previewpath[1] == MMP_3GP_PATH_INVALID) {
		return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
	}

	if (pPreviewBuf == NULL) {
		return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
	}

	/* Get the preivew display parameters */
    MMPS_Sensor_GetCurPrevScalInputRes(ubSnrSel, &ulScalInW, &ulScalInH);
    
	MMPD_BayerScaler_GetZoomInfo(MMP_BAYER_SCAL_DOWN, &sFitRangeBayer, &sGrabctlBayer); 

    if (m_sAhcVideoPrevInfo[ubSnrSel].bUserDefine) {
    	bDmaRotateEn    = m_sAhcVideoPrevInfo[ubSnrSel].bPreviewRotate;
    	ubDmaRotateDir	= m_sAhcVideoPrevInfo[ubSnrSel].sPreviewDmaDir;
    	sFitMode 		= m_sAhcVideoPrevInfo[ubSnrSel].sFitMode;
        ulDispStartX  	= m_sAhcVideoPrevInfo[ubSnrSel].ulDispStartX;
        ulDispStartY  	= m_sAhcVideoPrevInfo[ubSnrSel].ulDispStartY;
        ulDispWidth   	= m_sAhcVideoPrevInfo[ubSnrSel].ulDispWidth;
        ulDispHeight  	= m_sAhcVideoPrevInfo[ubSnrSel].ulDispHeight;
    }
    else {
    	bDmaRotateEn	= m_VidRecdConfigs.previewdata[1].bUseRotateDMA[usModeIdx];
    	ubDmaRotateDir	= m_VidRecdConfigs.previewdata[1].ubDMARotateDir[usModeIdx];
    	sFitMode 		= m_VidRecdConfigs.previewdata[1].sFitMode[usModeIdx];
        ulDispStartX  	= m_VidRecdConfigs.previewdata[1].usVidDispStartX[usModeIdx];
        ulDispStartY  	= m_VidRecdConfigs.previewdata[1].usVidDispStartY[usModeIdx];
        ulDispWidth   	= m_VidRecdConfigs.previewdata[1].usVidDisplayW[usModeIdx];
        ulDispHeight  	= m_VidRecdConfigs.previewdata[1].usVidDisplayH[usModeIdx];
    }

	/* Initial zoom relative config */ 
	MMPS_3GPRECD_InitDigitalZoomParam(m_2ndPrewFctlLink.scalerpath);

	MMPS_3GPRECD_RestoreDigitalZoomRange(m_2ndPrewFctlLink.scalerpath);

    if (m_VidRecdConfigs.previewpath[1] == MMP_3GP_PATH_2PIPE) 
    {
        // Config Video Preview Pipe
		fitrange.ulInWidth 	    = ulScalInW;
		fitrange.ulInHeight	    = ulScalInH;
        fitrange.fitmode    	= sFitMode;
        fitrange.scalerType		= MMP_SCAL_TYPE_SCALER;
 		fitrange.ulOutWidth     = usPreviewW;
 		fitrange.ulOutHeight    = usPreviewH;

        fitrange.ulInGrabX		= 1;
        fitrange.ulInGrabY		= 1;
        fitrange.ulInGrabW		= fitrange.ulInWidth;
        fitrange.ulInGrabH		= fitrange.ulInHeight;

        #if 1
        MMPD_Scaler_GetGCDBestFitScale(&fitrange, &previewGrabctl);
        #else
        MMPD_PTZ_InitPtzInfo(m_2ndPrewFctlLink.scalerpath,
				             fitrange.fitmode,
				             fitrange.ulInWidth, fitrange.ulInHeight,
				             fitrange.ulOutWidth, fitrange.ulOutHeight);

		MMPD_PTZ_GetCurPtzStep(m_2ndPrewFctlLink.scalerpath, NULL, &usCurZoomStep, NULL, NULL);

		MMPD_PTZ_CalculatePtzInfo(m_2ndPrewFctlLink.scalerpath, usCurZoomStep, 0, 0);

		MMPD_PTZ_GetCurPtzInfo(m_2ndPrewFctlLink.scalerpath, &fitrange, &previewGrabctl);
        #endif

        fctlAttr.colormode      = m_VidRecdConfigs.previewdata[1].DispColorFmt[usModeIdx];
        fctlAttr.fctllink       = m_2ndPrewFctlLink;
        fctlAttr.fitrange		= fitrange;
        fctlAttr.grabctl        = previewGrabctl;
        fctlAttr.scalsrc		= MMP_SCAL_SOURCE_GRA;
        fctlAttr.bSetScalerSrc	= MMP_TRUE;
        fctlAttr.usBufCnt  		= pPreviewBuf->usBufCnt;

        for (i = 0; i < fctlAttr.usBufCnt; i++) {
            fctlAttr.ulBaseAddr[i] = pPreviewBuf->ulYBuf[i];
            fctlAttr.ulBaseUAddr[i] = pPreviewBuf->ulUBuf[i];
            fctlAttr.ulBaseVAddr[i] = pPreviewBuf->ulVBuf[i];
        }

        if (bDmaRotateEn) {
            fctlAttr.bUseRotateDMA  = MMP_TRUE;
            fctlAttr.usRotateBufCnt = pPreviewBuf->usRotateBufCnt;
            
            for (i = 0; i < fctlAttr.usRotateBufCnt; i++) {
                fctlAttr.ulRotateAddr[i] = pPreviewBuf->ulRotateYBuf[i];
                fctlAttr.ulRotateUAddr[i] = pPreviewBuf->ulRotateUBuf[i];
                fctlAttr.ulRotateVAddr[i] = pPreviewBuf->ulRotateVBuf[i];
            }
        }
        else {
            fctlAttr.bUseRotateDMA = MMP_FALSE;
            fctlAttr.usRotateBufCnt = 0;
        }
	
        MMPD_Fctl_SetPipeAttrForIbcFB(&fctlAttr);

		MMPD_Fctl_ClearPreviewBuf(m_2ndPrewFctlLink.ibcpipeID, 0xFFFFFF);

        if (bDmaRotateEn) {
            MMPD_Fctl_LinkPipeToDma(m_2ndPrewFctlLink.ibcpipeID, 
            						m_VidRecdConfigs.previewdata[1].DispWinId[usModeIdx], 
            						m_VidRecdConfigs.previewdata[1].DispDevice[usModeIdx],
            						ubDmaRotateDir);
		}
        else {
        	MMPD_Fctl_LinkPipeToDisplay(m_2ndPrewFctlLink.ibcpipeID, 
        								m_VidRecdConfigs.previewdata[1].DispWinId[usModeIdx], 
        								m_VidRecdConfigs.previewdata[1].DispDevice[usModeIdx]);
		}

        // Config Display Window
        if ((m_VidRecdConfigs.previewdata[1].VidDispDir[usModeIdx] == MMP_DISPLAY_ROTATE_NO_ROTATE) ||
            (m_VidRecdConfigs.previewdata[1].VidDispDir[usModeIdx] == MMP_DISPLAY_ROTATE_RIGHT_180)) {                        
            ulRotateW = usPreviewW;
            ulRotateH = usPreviewH;
        }
        else {
            ulRotateW = usPreviewH;
            ulRotateH = usPreviewW;
        }
        
        if (bDmaRotateEn) {
        	// Rotate 90/270 for vertical panel
        	ulRotateW = usPreviewH;
            ulRotateH = usPreviewW;
        }
        
        dispAttr.usStartX          = 0;
        dispAttr.usStartY          = 0;
		#if 0 // Image at center
		dispAttr.usDisplayOffsetX  = (ulDispWidth > ulRotateW) ? ((ulDispWidth - ulRotateW) >> 1) : (0);
		dispAttr.usDisplayOffsetY  = (ulDispHeight > ulRotateH) ? ((ulDispHeight - ulRotateH) >> 1) : (0);
		#else
		dispAttr.usDisplayOffsetX  = ulDispStartX;
		dispAttr.usDisplayOffsetY  = ulDispStartY;
		#endif
        dispAttr.bMirror           = m_VidRecdConfigs.previewdata[1].bVidDispMirror[usModeIdx];

        if (bDmaRotateEn) {
            dispAttr.rotatetype    = MMP_DISPLAY_ROTATE_NO_ROTATE;
        }
        else {
            dispAttr.rotatetype    = m_VidRecdConfigs.previewdata[1].VidDispDir[usModeIdx];
        }

        dispAttr.usDisplayWidth    = ulRotateW;
        dispAttr.usDisplayHeight   = ulRotateH;

        if (m_bHdmiInterlace) {
            dispAttr.usDisplayHeight = ulRotateH / 2;
        }
        
        MMPD_Display_SetWinToDisplay(m_VidRecdConfigs.previewdata[1].DispWinId[usModeIdx], &dispAttr);

        fitrange.fitmode        = sFitMode;
        fitrange.scalerType 	= MMP_SCAL_TYPE_WINSCALER;
        fitrange.ulInWidth      = ulRotateW;
        fitrange.ulInHeight     = ulRotateH;
        fitrange.ulOutWidth     = ulRotateW;
        fitrange.ulOutHeight    = ulRotateH;
 
 		fitrange.ulInGrabX 		= 1;
        fitrange.ulInGrabY 		= 1;
        fitrange.ulInGrabW 		= fitrange.ulInWidth;
        fitrange.ulInGrabH 		= fitrange.ulInHeight;
        
        MMPD_Scaler_GetGCDBestFitScale(&fitrange, &DispGrabctl);

        MMPD_Display_SetWinScaling(m_VidRecdConfigs.previewdata[1].DispWinId[usModeIdx], &fitrange, &DispGrabctl);
        // Tune TV IN preview MCI priority for Icon overflow issue.
        #if (REAR_CAM_TYPE == REAR_CAM_TYPE_TV_DECODER)
        MMPD_VIDENC_TuneEncodeSecondPipeMCIPriority(m_2ndPrewFctlLink.ibcpipeID);
        #endif
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_Enable2ndSnrPreviewPipe
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Turn on and off preview for video encode.

 @param[in] bEnable 			Enable and disable scaler path for video preview.
 @param[in] bCheckFrameEnd 		When "bEnable" is MMP_TRUE, the setting means check frame end or not.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_Enable2ndSnrPreviewPipe(MMP_UBYTE ubSnrSel, MMP_BOOL bEnable, MMP_BOOL bCheckFrameEnd)
{
    MMP_DISPLAY_WIN_ID  winId = m_VidRecdConfigs.previewdata[1].DispWinId[m_VidRecdModes.usVideoPreviewMode];
    MMP_UBYTE		    ubVifId = MMPD_Sensor_GetVIFPad(ubSnrSel);
    
	if (!(bEnable ^ m_bVidPreviewActive[ubSnrSel])) {
		return MMP_ERR_NONE;
	}

    if (bEnable) 
    {
    	if (MMPD_Fctl_EnablePreview(ubSnrSel, m_2ndPrewFctlLink.ibcpipeID, bEnable, bCheckFrameEnd)) {
			return MMP_3GPRECD_ERR_GENERAL_ERROR;
		}
    }
    else 
    {
    	if (MMPD_Fctl_EnablePreview(ubSnrSel, m_2ndPrewFctlLink.ibcpipeID, bEnable, MMP_TRUE)) {
			return MMP_3GPRECD_ERR_GENERAL_ERROR;
		}	

		MMPD_Display_SetWinActive(winId, MMP_FALSE);

		if (m_VidRecdConfigs.bRawPreviewEnable[1]) {
            MMPD_RAWPROC_EnablePreview(ubVifId, MMP_FALSE);
            MMPD_RAWPROC_EnablePath(ubSnrSel,
                                    ubVifId,
                                    MMP_FALSE,
				                    MMP_RAW_IOPATH_VIF2RAW,
				                    m_VidRecdConfigs.ubRawPreviewBitMode[1]);
		}
    }

    m_bVidPreviewActive[ubSnrSel] = bEnable;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_Set2ndSnrPreviewMemory
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set memory map for VideoR preview
 @param[in] usPreviewW Preview width.
 @param[in] usPreviewW Preview height.
 @param[in/out] ulStackAddr Available start address of dram buffer.
 @retval MMP_ERR_NONE Success.
*/
static MMP_ERR MMPS_3GPRECD_Set2ndSnrPreviewMemory( MMP_UBYTE   ubSnrSel,
                                                    MMP_USHORT  usPreviewW,  
                                                    MMP_USHORT  usPreviewH, 
									  		        MMP_ULONG   *ulStackAddr) 
{
	MMP_USHORT						usModeIdx = m_VidRecdModes.usVideoPreviewMode; 
	MMP_USHORT						i;
    MMP_ULONG						ulCurAddr = *ulStackAddr; 
    MMP_ULONG						ulTmpBufSize;
    MMP_ULONG                       ulPreviewBufSize = 0;
    MMP_ULONG                       ulRawBufSize = 0;
    MMPS_3GPRECD_PREVIEW_DATA       *previewdata = &m_VidRecdConfigs.previewdata[1];
    MMPS_3GPRECD_PREVIEW_BUFINFO	previewbuf;
    MMP_RAW_STORE_BUF          		rawbuf;
    MMP_ULONG          		        rawendbuf[MAX_RAW_STORE_BUF_NUM];
    MMP_UBYTE				        ubVifId = MMPD_Sensor_GetVIFPad(ubSnrSel);
    
    ///////////////////////////////////////////////////////////////////////////
    //
    //  Get Preview Config setting and calculate how many memory will be used    
    //
    ///////////////////////////////////////////////////////////////////////////
    
    // Get Preview buffer config and size
    previewbuf.usBufCnt = previewdata->usVidDispBufCnt[usModeIdx];

    if (previewdata->bUseRotateDMA[usModeIdx])
		previewbuf.usRotateBufCnt = previewdata->usRotateBufCnt[usModeIdx];
	else
		previewbuf.usRotateBufCnt = 0;

    switch(previewdata->DispColorFmt[usModeIdx]) {
    case MMP_DISPLAY_COLOR_YUV420:
        ulPreviewBufSize = ALIGN32(usPreviewW * usPreviewH) + ALIGN32((usPreviewW*usPreviewH)>>2)*2;
        break;
    case MMP_DISPLAY_COLOR_YUV420_INTERLEAVE:
        ulPreviewBufSize = ALIGN32(usPreviewW * usPreviewH) + ALIGN32(usPreviewW*(usPreviewH>>1));
        break;
    case MMP_DISPLAY_COLOR_YUV422:
    case MMP_DISPLAY_COLOR_RGB565:
        ulPreviewBufSize = ALIGN32(usPreviewW * 2 * usPreviewH);
        break;
    case MMP_DISPLAY_COLOR_RGB888:
        ulPreviewBufSize = ALIGN32(usPreviewW * 3 * usPreviewH);
        break;
    default:
    	PRINTF("Not Support Color Format\r\n");
        return MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS;
        break;
    }

	// Get Raw preview buffer config and size
	if (MMP_TRUE == m_VidRecdConfigs.bRawPreviewEnable[1]) {
	
		MMP_USHORT	usSensorW;
    	MMP_USHORT	usSensorH;

        if (m_VidRecdConfigs.ulRawStoreBufCnt > VR_MAX_RAWSTORE_BUFFER_NUM) {
            m_VidRecdConfigs.ulRawStoreBufCnt = VR_MAX_RAWSTORE_BUFFER_NUM;
        }

        rawbuf.ulRawBufCnt = m_VidRecdConfigs.ulRawStoreBufCnt;

		MMPD_RAWPROC_GetStoreRange(ubVifId, &usSensorW, &usSensorH);

		if (m_VidRecdConfigs.ubRawPreviewBitMode[1] == MMP_RAW_COLORFMT_BAYER10)
			ulRawBufSize = ALIGN32(usSensorW * usSensorH * 4 / 3);
		else if (m_VidRecdConfigs.ubRawPreviewBitMode[1] == MMP_RAW_COLORFMT_YUV420)
			ulRawBufSize = usSensorW * usSensorH; // Y plane only
		else if (m_VidRecdConfigs.ubRawPreviewBitMode[1] == MMP_RAW_COLORFMT_YUV422)
		    //#if (BIND_SENSOR_BIT1603)
			if(0XBBBB1603 == gsSensorFunction->MMPF_Sensor_GetSnrID(SCD_SENSOR)){
		    	ulRawBufSize = ALIGN32(usSensorW * usSensorH * 1);
			}else{
				ulRawBufSize = ALIGN32(usSensorW * usSensorH * 2);
			}
		else
			ulRawBufSize = ALIGN32(usSensorW * usSensorH);
    }
    else {
        rawbuf.ulRawBufCnt = 0;
        ulRawBufSize = 0;
    }

    //////////////////////////////////////////////////////////////////////////
    //
    //  Allocate memory for preview buffer
    //
    //////////////////////////////////////////////////////////////////////////

	ulCurAddr = ALIGN32(*ulStackAddr);
    MMPD_VIDENC_SetMemStart(ulCurAddr);
	
	// Allocate memory for preview buffer
    for (i = 0; i < previewbuf.usBufCnt; i++) {
		if (previewdata->DispColorFmt[usModeIdx] == MMP_DISPLAY_COLOR_YUV420) {
    		ulTmpBufSize = usPreviewW * usPreviewH;

            previewbuf.ulYBuf[i] = ulCurAddr;
            previewbuf.ulUBuf[i] = previewbuf.ulYBuf[i] + ALIGN32(ulTmpBufSize);
            previewbuf.ulVBuf[i] = previewbuf.ulUBuf[i] + ALIGN32(ulTmpBufSize >> 2);

            ulCurAddr += (ALIGN32(ulTmpBufSize) + ALIGN32(ulTmpBufSize>>2)*2);
        }
        else if (previewdata->DispColorFmt[usModeIdx] == MMP_DISPLAY_COLOR_YUV420_INTERLEAVE) {
    		ulTmpBufSize = usPreviewW * usPreviewH;

    		previewbuf.ulYBuf[i] = ulCurAddr;
            previewbuf.ulUBuf[i] = previewbuf.ulYBuf[i] + ALIGN32(ulTmpBufSize);
            previewbuf.ulVBuf[i] = previewbuf.ulUBuf[i];

            ulCurAddr += (ALIGN32(ulTmpBufSize) + ALIGN32(ulTmpBufSize>>1));
        }
		else if (previewdata->DispColorFmt[usModeIdx] == MMP_DISPLAY_COLOR_YUV422 ||
				 previewdata->DispColorFmt[usModeIdx] == MMP_DISPLAY_COLOR_RGB565) {
    		ulTmpBufSize = ALIGN32(usPreviewW * 2 * usPreviewH);

    		previewbuf.ulYBuf[i] = ulCurAddr;
            previewbuf.ulUBuf[i] = 0;
            previewbuf.ulVBuf[i] = 0;
            
            ulCurAddr += ulTmpBufSize;
		}
        else if (previewdata->DispColorFmt[usModeIdx] == MMP_DISPLAY_COLOR_RGB888) {
    		ulTmpBufSize = ALIGN32(usPreviewW * 3 * usPreviewH);

    		previewbuf.ulYBuf[i] = ulCurAddr;
            previewbuf.ulUBuf[i] = 0;
            previewbuf.ulVBuf[i] = 0;

            ulCurAddr += ulTmpBufSize;
		}
		else {
		    PRINTF("Not supported preview format\r\n");  
		    return MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS;
		}
	}
	
    // Allocate memory for rotate DMA destination buffer
	for (i = 0; i < previewbuf.usRotateBufCnt; i++) {
	
		if (previewdata->DispColorFmt[usModeIdx] == MMP_DISPLAY_COLOR_YUV420) {
		    ulTmpBufSize = usPreviewW * usPreviewH;
		       
		    previewbuf.ulRotateYBuf[i] = ALIGN32(ulCurAddr);
            previewbuf.ulRotateUBuf[i] = previewbuf.ulRotateYBuf[i] + ALIGN32(ulTmpBufSize);
            previewbuf.ulRotateVBuf[i] = previewbuf.ulRotateUBuf[i] + ALIGN32(ulTmpBufSize >> 2);

            ulCurAddr += (ALIGN32(ulTmpBufSize) + ALIGN32(ulTmpBufSize>>2)*2);
	    }
		else if (previewdata->DispColorFmt[usModeIdx] == MMP_DISPLAY_COLOR_YUV420_INTERLEAVE) {
		    ulTmpBufSize = usPreviewW * usPreviewH;

		    previewbuf.ulRotateYBuf[i] = ALIGN32(ulCurAddr);
            previewbuf.ulRotateUBuf[i] = previewbuf.ulRotateYBuf[i] + ALIGN32(ulTmpBufSize);
            previewbuf.ulRotateVBuf[i] = previewbuf.ulRotateUBuf[i];
 
            ulCurAddr += (ALIGN32(ulTmpBufSize) + ALIGN32(ulTmpBufSize>>1));
	    }
		else if (previewdata->DispColorFmt[usModeIdx] == MMP_DISPLAY_COLOR_YUV422 ||
				 previewdata->DispColorFmt[usModeIdx] == MMP_DISPLAY_COLOR_RGB565) {
		    ulTmpBufSize = ALIGN32(usPreviewW * 2 * usPreviewH);
		    
		    previewbuf.ulRotateYBuf[i] = ALIGN32(ulCurAddr);
            previewbuf.ulRotateUBuf[i] = 0;
            previewbuf.ulRotateVBuf[i] = 0;

            ulCurAddr += ulTmpBufSize;
		}
        else if (previewdata->DispColorFmt[usModeIdx] == MMP_DISPLAY_COLOR_RGB888) {
		    ulTmpBufSize = ALIGN32(usPreviewW * 3 * usPreviewH);
		    
		    previewbuf.ulRotateYBuf[i] = ALIGN32(ulCurAddr);
            previewbuf.ulRotateUBuf[i] = 0;
            previewbuf.ulRotateVBuf[i] = 0;

            ulCurAddr += ulTmpBufSize;
		}
		else {
		    PRINTF("Not supported preview format\r\n");  
		    return MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS;
		}
	}

	// Allocate memory for raw preview buffer
    if (m_VidRecdConfigs.bRawPreviewEnable[1]) {

        for (i = 0; i < rawbuf.ulRawBufCnt; i++) {
		    rawbuf.ulRawBufAddr[i] = ALIGN32(ulCurAddr);
		    ulCurAddr = rawbuf.ulRawBufAddr[i] + ulRawBufSize;
		    if (m_VidRecdConfigs.ubRawPreviewBitMode[1] == MMP_RAW_COLORFMT_YUV420) {
		        // For I420
		        rawbuf.ulRawBufUAddr[i] = (ulCurAddr);
		        ulCurAddr = rawbuf.ulRawBufUAddr[i] + (ulRawBufSize >> 2); 
		        rawbuf.ulRawBufVAddr[i] = (ulCurAddr);
		        ulCurAddr = rawbuf.ulRawBufVAddr[i] + (ulRawBufSize >> 2);
		    }
		    rawendbuf[i] = ulCurAddr;
	    }
        for (i = 0; i < rawbuf.ulRawBufCnt; i++) {
		    MMPD_RAWPROC_SetRawStoreBufferEnd(ubVifId, MMP_RAW_STORE_PLANE0, i, ulCurAddr);
        }
	    MMPD_RAWPROC_EnableRingStore(ubVifId, MMP_RAW_STORE_PLANE0, MMP_TRUE);
	    MMPD_RAWPROC_SetStoreBuf(ubVifId, &rawbuf);
        
        #if (SCD_SNR_USE_DMA_DEINTERLACE) // For YUV422, YUV420 TBD
        #define DMA_DEINTERLACE_BUF_CNT 2
        #define DMA_DEINTERLACE_DOUBLE_FIELDS 2
        MMPD_RAWPROC_SetDeInterlaceBuf(ulCurAddr, 0, 0, DMA_DEINTERLACE_BUF_CNT);
        ulCurAddr += ALIGN32(ulRawBufSize * DMA_DEINTERLACE_DOUBLE_FIELDS * DMA_DEINTERLACE_BUF_CNT); 
        #endif

        MMPD_RAWPROC_SetStoreOnly(ubVifId, MMP_TRUE);
        MMPD_RAWPROC_EnablePreview(ubVifId, MMP_TRUE);

        MMPD_RAWPROC_EnablePath(ubSnrSel,
                                ubVifId,
                                MMP_TRUE,
				                MMP_RAW_IOPATH_VIF2RAW,
				                m_VidRecdConfigs.ubRawPreviewBitMode[1]);
    }
    else {
    	MMPD_RAWPROC_EnablePreview(ubVifId, MMP_FALSE);
    }

	*ulStackAddr = ulCurAddr;
	m_ulVidRecDramEndAddr = m_ulVideoPreviewEndAddr = ulCurAddr;
	
	#if defined(ALL_FW)
    if (m_ulVidRecDramEndAddr > MMPS_System_GetMemHeapEnd()) {
        printc("\t= [HeapMemErr] 2nd Video preview =\r\n");
        return MMP_3GPRECD_ERR_MEM_EXHAUSTED;
    }
    printc("End of 2nd video preview buffers = 0x%X\r\n", m_ulVidRecDramEndAddr);
    #endif

	MMPD_VIDENC_SetMemStart(ulCurAddr);

    MMPS_3GPRECD_Set2ndSnrPreviewPipeConfig(&previewbuf, usPreviewW, usPreviewH, ubSnrSel);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_2ndSnrPreviewStop
//  Description : Stop preview display mode.
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_2ndSnrPreviewStop(MMP_UBYTE ubSnrSel)
{
	if (m_VidRecdConfigs.previewpath[1] == MMP_3GP_PATH_INVALID) {
		return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
	}

    MMPS_3GPRECD_Enable2ndSnrPreviewPipe(ubSnrSel, MMP_FALSE, MMP_FALSE);
   	
    /* This is a work around. After real-time H264 encode,
     * we have to reset IBC to avoid a corrupted still JPEG later.
     * Here we reset all of IBC pipes, instead of H.264 pipe only.
     */
    MMPD_IBC_ResetModule(m_2ndPrewFctlLink.ibcpipeID);
    MMPD_IBC_ResetModule(m_2ndRecFctlLink.ibcpipeID);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_2ndSnrPreviewStart
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set buffers and parameters. Then display preview mode.
 @param[in] bCheckFrameEnd 	The setting will check VIF frame end or not.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS Unsupport resolution.
*/
MMP_ERR MMPS_3GPRECD_2ndSnrPreviewStart(MMP_UBYTE ubSnrSel, MMP_BOOL bCheckFrameEnd)
{
    MMP_ULONG ulStackAddr = 0;
	
	if (m_VidRecdConfigs.previewpath[1] == MMP_3GP_PATH_INVALID) {
		return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
	}

    MMPS_3GPRECD_AdjustPreviewRes(ubSnrSel);

	m_VidRecdConfigs.bRawPreviewEnable[1] = MMP_TRUE;//MMP_TRUE;//bRawPrewEn;
    /* Use first sensor preivew buffer end as the buffer start */
    ulStackAddr = m_ulVidRecDramEndAddr = m_ulVideoPreviewEndAddr;

    if (MMPS_3GPRECD_Set2ndSnrPreviewMemory(ubSnrSel,
                                            m_ulVRPreviewW[ubSnrSel],
                                            m_ulVRPreviewH[ubSnrSel],
                                            &ulStackAddr))
    {
        PRINTF("Alloc mem for 2nd preview failed\r\n");
        return MMP_3GPRECD_ERR_MEM_EXHAUSTED;
    }
    
    if (MMPS_3GPRECD_Enable2ndSnrPreviewPipe(ubSnrSel, MMP_TRUE, bCheckFrameEnd) != MMP_ERR_NONE) {
		PRINTF("Enable Video Preview: Fail\r\n");
        return MMP_3GPRECD_ERR_GENERAL_ERROR;
    }
    
    return MMP_ERR_NONE;
}

#endif

#if 0
void ____VR_DecMJPEGToPreview_Record_Function____(){ruturn;} //dummy
#endif

#if (SUPPORT_SONIX_UVC_ISO_MODE) && (SUPPORT_DEC_MJPEG_TO_PREVIEW)
//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetDColorformatToJpgAttr
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_SetDColorformatToJpgAttr(MMP_UBYTE ubColorformat)
{	
	extern MMP_UBYTE gubIBCColorFormat;
	//Colorformat transfer between display and ibc
	switch(ubColorformat) {
	case MMP_DISPLAY_COLOR_RGB565:
	    gubIBCColorFormat = MMP_IBC_COLOR_RGB565;
        break;
    case MMP_DISPLAY_COLOR_RGB888:
	    gubIBCColorFormat = MMP_IBC_COLOR_RGB888;
        break;
	case MMP_DISPLAY_COLOR_YUV420:
	    gubIBCColorFormat = MMP_IBC_COLOR_I420;
        break;
	case MMP_DISPLAY_COLOR_YUV420_INTERLEAVE:
	    gubIBCColorFormat = MMP_IBC_COLOR_NV12;
        break;
	case MMP_DISPLAY_COLOR_YUV422:
	    gubIBCColorFormat = MMP_IBC_COLOR_YUV422_YUYV;
        break;        
    default:
	    DBG_S(0, "[ERR]: Need to assign IBC colorformat for JpegAttr \r\n");	
        return -1;
	}
    return 0;
}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetDecMjpegToPreviewPipeId
//  Description :
//------------------------------------------------------------------------------
MMP_UBYTE MMPS_3GPRECD_GetDecMjpegToPreviewPipeId(void)
{
#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
    return m_DecMjpegToPrevwFctlLink.ibcpipeID;
#else
    return 0;
#endif
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetDecMjpegToPreviewSrcAttr
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_GetDecMjpegToPreviewSrcAttr(MMP_USHORT *pusW, MMP_USHORT *pusH)
{
#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
    *pusW = m_usDecMjpegToPreviewSrcW;
    *pusH = m_usDecMjpegToPreviewSrcH;
#endif
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetDecMjpegToPreviewSrcAttr
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_SetDecMjpegToPreviewSrcAttr(MMP_USHORT usSrcW, MMP_USHORT usSrcH)
{
#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
    m_usDecMjpegToPreviewSrcW = usSrcW;
    m_usDecMjpegToPreviewSrcH = usSrcH;
#endif
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetDecMjpegToPreviewDispAttr
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_GetDecMjpegToPreviewDispAttr(  MMP_USHORT *pusDispWinId,
                                                    MMP_USHORT *pusWinOfstX,  MMP_USHORT *pusWinOfstY,
                                                    MMP_USHORT *pusWinWidth,  MMP_USHORT *pusWinHeight,
                                                    MMP_USHORT *pusDispWidth, MMP_USHORT *pusDispHeight,                                              
                                                    MMP_USHORT *pusDispColor)
{
#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
    *pusDispWinId   = (MMP_USHORT)m_sDecMjpegToPreviewWinId;
    *pusWinOfstX    = m_sAhcDecMjpegToPreviewInfo.ulDispStartX;
    *pusWinOfstY    = m_sAhcDecMjpegToPreviewInfo.ulDispStartY;
    *pusWinWidth    = m_sAhcDecMjpegToPreviewInfo.ulPreviewBufW;
    *pusWinHeight   = m_sAhcDecMjpegToPreviewInfo.ulPreviewBufH;
    *pusDispWidth   = m_sAhcDecMjpegToPreviewInfo.ulDispWidth;
    *pusDispHeight  = m_sAhcDecMjpegToPreviewInfo.ulDispHeight;
    *pusDispColor   = m_sAhcDecMjpegToPreviewInfo.sDispColor;
#endif
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetDecMjpegToPreviewDispAttr
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_SetDecMjpegToPreviewDispAttr(  MMP_USHORT usDispWinId,
            									    MMP_BOOL   bRotate,
            									    MMP_UBYTE  ubRotateDir,
            									    MMP_UBYTE  sFitMode,
                                                    MMP_USHORT usWinOfstX,  MMP_USHORT usWinOfstY,
                                                    MMP_USHORT usWinWidth,  MMP_USHORT usWinHeight,
                                                    MMP_USHORT usDispWidth, MMP_USHORT usDispHeight,
                                                    MMP_USHORT usDispColor)
{
#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
    m_sDecMjpegToPreviewWinId = (MMP_DISPLAY_WIN_ID) usDispWinId;

    m_sAhcDecMjpegToPreviewInfo.bPreviewRotate  = bRotate;
    m_sAhcDecMjpegToPreviewInfo.sPreviewDmaDir  = ubRotateDir;
    m_sAhcDecMjpegToPreviewInfo.sFitMode	    = sFitMode;
    m_sAhcDecMjpegToPreviewInfo.ulPreviewBufW   = usWinWidth;
	//m_sAhcDecMjpegToPreviewInfo.ulPreviewBufH   = 480; // sync from Rony    
	m_sAhcDecMjpegToPreviewInfo.ulPreviewBufH   = usWinHeight;	
    m_sAhcDecMjpegToPreviewInfo.ulDispStartX   	= usWinOfstX;
    m_sAhcDecMjpegToPreviewInfo.ulDispStartY   	= usWinOfstY;
    m_sAhcDecMjpegToPreviewInfo.ulDispWidth    	= usDispWidth;
    m_sAhcDecMjpegToPreviewInfo.ulDispHeight    = usDispHeight;
    m_sAhcDecMjpegToPreviewInfo.sDispColor      = (MMP_DISPLAY_COLORMODE)usDispColor;

#endif
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetDecMjpegToPreviewBuf
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_SetDecMjpegToPreviewBuf(   MMP_USHORT                      usPreviewW,  
                                                MMP_USHORT                      usPreviewH, 
                                                MMP_ULONG                       ulAddr, 
                                                MMP_ULONG                       *pulSize,
                                                MMPS_3GPRECD_PREVIEW_BUFINFO    *pPreviewBuf)
{
#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
    MMP_ULONG   cur_buf = ulAddr;
    MMP_USHORT  i = 0;
    MMP_ULONG   ulTmpBufSize = 0;
    MMP_ULONG   ulYSize;
    MMP_BOOL    bIsPrevwResolSet = MMP_FALSE;

    #define YUV420_Y_BLACK_COLOR    (0x00)
    #define YUV420_UV_BLACK_COLOR   (0x80)

    #if (HANDLE_JPEG_EVENT_BY_QUEUE)
    /* Reserved for Max MJPEG resolution */
	usPreviewW  = (usPreviewW > m_usMJPEGMaxEncWidth) ? (usPreviewW) : (m_usMJPEGMaxEncWidth);
	usPreviewH  = (usPreviewH > m_usMJPEGMaxEncHeight) ? (usPreviewH) : (m_usMJPEGMaxEncHeight);
    #endif

	/* Allocate preview buffer (window buffer) */
	pPreviewBuf->usBufCnt = 2;
	
	for (i = 0; i < pPreviewBuf->usBufCnt; i++)
	{
        switch (m_sAhcDecMjpegToPreviewInfo.sDispColor) {
    	case MMP_DISPLAY_COLOR_RGB565:
    	case MMP_DISPLAY_COLOR_YUV422:
    	    ulTmpBufSize = ALIGN32(usPreviewW * 2 * usPreviewH);
    	
    	    pPreviewBuf->ulYBuf[i] = cur_buf;
    	    pPreviewBuf->ulUBuf[i] = 0;
    	    pPreviewBuf->ulVBuf[i] = 0;
            cur_buf += ulTmpBufSize;
    		break;
    	case MMP_DISPLAY_COLOR_RGB888:
    	    ulTmpBufSize = ALIGN32(usPreviewW * 3 * usPreviewH);
    	
    	    pPreviewBuf->ulYBuf[i] = cur_buf;
    	    pPreviewBuf->ulUBuf[i] = 0;
    	    pPreviewBuf->ulVBuf[i] = 0;
            cur_buf += ulTmpBufSize;
    		break;
    	case MMP_DISPLAY_COLOR_YUV420:
    	    ulTmpBufSize = usPreviewW * usPreviewH;
    	    
     	    pPreviewBuf->ulYBuf[i] = cur_buf;
     	    pPreviewBuf->ulUBuf[i] = pPreviewBuf->ulYBuf[i] + ALIGN32(ulTmpBufSize);
     	    pPreviewBuf->ulVBuf[i] = pPreviewBuf->ulUBuf[i] + ALIGN32(ulTmpBufSize >> 2);
            cur_buf += (ALIGN32(ulTmpBufSize) + ALIGN32(ulTmpBufSize>>2)*2);
    		break;
    	case MMP_DISPLAY_COLOR_YUV420_INTERLEAVE:
    	    ulTmpBufSize = usPreviewW * usPreviewH;
    	
     	    pPreviewBuf->ulYBuf[i] = cur_buf;
     	    pPreviewBuf->ulUBuf[i] = pPreviewBuf->ulYBuf[i] + ALIGN32(ulTmpBufSize);
     	    pPreviewBuf->ulVBuf[i] = pPreviewBuf->ulUBuf[i];
            cur_buf += (ALIGN32(ulTmpBufSize) + ALIGN32(ulTmpBufSize>>1));
    		break;
    	}
	}
    
    /* Preset/Clear window buffer */
    ulYSize = usPreviewW * usPreviewH;
    
    MMPD_3GPMGR_IsUVCPrevwResolSet(&bIsPrevwResolSet);

    if (bIsPrevwResolSet == MMP_FALSE)
    {
        DBG_S(0, "[ERR]: PREVW (W, H) not set yet, incorrect seq.! \r\n");
    }
    else
    {
        /* Set buffer to black color */
        if (m_sAhcDecMjpegToPreviewInfo.sDispColor == MMP_DISPLAY_COLOR_YUV420_INTERLEAVE) {
            MEMSET((void*)(pPreviewBuf->ulYBuf[0]), YUV420_Y_BLACK_COLOR, ulYSize);
            MEMSET((void*)(pPreviewBuf->ulUBuf[0]), YUV420_UV_BLACK_COLOR, (ulYSize>>1));
            MEMSET((void*)(pPreviewBuf->ulYBuf[1]), YUV420_Y_BLACK_COLOR, ulYSize);
            MEMSET((void*)(pPreviewBuf->ulUBuf[1]), YUV420_UV_BLACK_COLOR, (ulYSize>>1));
        }
        else {
            // TBD
        }
        
        /* First time: Update Window Address */
        /* Fix refresh hang, set UVC preview address once buffer allocated */
        if (!m_sAhcDecMjpegToPreviewInfo.bPreviewRotate) {
            MMPD_Display_UpdateWinAddr( m_sDecMjpegToPreviewWinId,
                                        pPreviewBuf->ulYBuf[1],
                                        pPreviewBuf->ulUBuf[1],
                                        pPreviewBuf->ulVBuf[1]);
        }
    }
    
    /* Allocate rotate buffer */
    if (m_sAhcDecMjpegToPreviewInfo.bPreviewRotate)
    {
		pPreviewBuf->usRotateBufCnt = 2;
 
 		for (i = 0; i < pPreviewBuf->usRotateBufCnt; i++) 
 		{
        	switch (m_sAhcDecMjpegToPreviewInfo.sDispColor) {
    		case MMP_DISPLAY_COLOR_RGB565:
    		case MMP_DISPLAY_COLOR_YUV422:
    	    	ulTmpBufSize = ALIGN32(usPreviewW * 2 * usPreviewH);

    	    	pPreviewBuf->ulRotateYBuf[i] = cur_buf;
    	    	pPreviewBuf->ulRotateUBuf[i] = 0;
    	    	pPreviewBuf->ulRotateVBuf[i] = 0;
            	cur_buf += ulTmpBufSize;
    			break;
    		case MMP_DISPLAY_COLOR_RGB888:
    	    	ulTmpBufSize = ALIGN32(usPreviewW * 3 * usPreviewH);

    	    	pPreviewBuf->ulRotateYBuf[i] = cur_buf;
    	    	pPreviewBuf->ulRotateUBuf[i] = 0;
    	    	pPreviewBuf->ulRotateVBuf[i] = 0;
            	cur_buf += ulTmpBufSize;
    			break;
    		case MMP_DISPLAY_COLOR_YUV420:
    	    	ulTmpBufSize = usPreviewW * usPreviewH;

     	    	pPreviewBuf->ulRotateYBuf[i] = cur_buf;
     	    	pPreviewBuf->ulRotateUBuf[i] = pPreviewBuf->ulRotateYBuf[i] + ALIGN32(ulTmpBufSize);
     	    	pPreviewBuf->ulRotateVBuf[i] = pPreviewBuf->ulRotateUBuf[i] + ALIGN32(ulTmpBufSize >> 2);
            	cur_buf += (ALIGN32(ulTmpBufSize) + ALIGN32(ulTmpBufSize>>2)*2);
    			break;
    		case MMP_DISPLAY_COLOR_YUV420_INTERLEAVE:
    	    	ulTmpBufSize = usPreviewW * usPreviewH;

     	    	pPreviewBuf->ulRotateYBuf[i] = cur_buf;
     	    	pPreviewBuf->ulRotateUBuf[i] = pPreviewBuf->ulRotateYBuf[i] + ALIGN32(ulTmpBufSize);
     	    	pPreviewBuf->ulRotateVBuf[i] = pPreviewBuf->ulRotateUBuf[i];
            	cur_buf += (ALIGN32(ulTmpBufSize) + ALIGN32(ulTmpBufSize>>1));
    			break;
    		}
		}
    
        if (bIsPrevwResolSet)
        {
            /* Set buffer to black color */
            if (m_sAhcDecMjpegToPreviewInfo.sDispColor == MMP_DISPLAY_COLOR_YUV420_INTERLEAVE) {
                MEMSET((void*)(pPreviewBuf->ulRotateYBuf[0]), YUV420_Y_BLACK_COLOR, ulYSize);
                MEMSET((void*)(pPreviewBuf->ulRotateUBuf[0]), YUV420_UV_BLACK_COLOR, (ulYSize>>1));
                MEMSET((void*)(pPreviewBuf->ulRotateYBuf[1]), YUV420_Y_BLACK_COLOR, ulYSize);
                MEMSET((void*)(pPreviewBuf->ulRotateUBuf[1]), YUV420_UV_BLACK_COLOR, (ulYSize>>1));
            }
            else {
                // TBD
            }
            
            /* First time: Update Window Address */
            /* Fix refresh hang, set UVC preview address once buffer allocated */
            MMPD_Display_UpdateWinAddr( m_sDecMjpegToPreviewWinId,
                                        pPreviewBuf->ulRotateYBuf[1],
                                        pPreviewBuf->ulRotateUBuf[1],
                                        pPreviewBuf->ulRotateVBuf[1]);
        }
    }
    else {
        pPreviewBuf->usRotateBufCnt = 0;
    }
    
    *pulSize = ALIGN32(cur_buf - ulAddr);
#endif
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_InitDecMjpegToPreview
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_InitDecMjpegToPreview(MMP_USHORT usJpegSrcW, MMP_USHORT usJpegSrcH)
{
#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
    MMP_ERR						    retstatus;
    MMP_SCAL_FIT_RANGE       	    fitrange;
    MMP_SCAL_GRAB_CTRL     		    grabctl;
    MMP_SCAL_FIT_MODE  			    sFitmode;
    MMP_DSC_JPEG_INFO               sJpegInfo;
    MMPD_FCTL_ATTR  		        fctlAttr;
    MMP_USHORT				        usModeIdx = m_VidRecdModes.usVideoPreviewMode;
    MMP_USHORT                      i = 0;

	MMP_ULONG                       ulWinOffsetX, ulWinOffsetY;
	MMP_ULONG                       ulPrevwBufW, ulPrevwBufH;
	MMP_ULONG          		        ulRotateW, ulRotateH;
	MMP_ULONG                       ulDispWidth, ulDispHeight;
	MMP_DISPLAY_ROTATE_TYPE    	    RotateType 	= MMP_DISPLAY_ROTATE_NO_ROTATE;
	MMP_BOOL                        bDmaRotateEn;
	MMP_DISPLAY_COLORMODE           DispColor   = MMP_DISPLAY_COLOR_YUV422;
    MMP_DISPLAY_DISP_ATTR  	        dispAttr;
    MMPS_3GPRECD_PREVIEW_BUFINFO    *pPreviewBuf = &m_sDecMjpegPrevwBufInfo;
	
	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, 	MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_ICON,	MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, 	MMP_TRUE);

	/* If we have known the Jpeg resolution, we don't need to parse it again */
	if (usJpegSrcW != 0 && usJpegSrcH != 0) {
	    sJpegInfo.bValid = MMP_TRUE;

	    sJpegInfo.usPrimaryWidth   = usJpegSrcW;
	    sJpegInfo.usPrimaryHeight  = usJpegSrcH;
	    sJpegInfo.jpgFormat        = MMP_DSC_JPEG_FMT422;//TBD
	}
	else {
	    sJpegInfo.bValid = MMP_FALSE;
	}
	
    /* Get JPEG information */
	if (!sJpegInfo.bValid)
	{
	    // Need to set sJpegInfo.ulJpegBufAddr and sJpegInfo.ulJpegBufSize for parse bitstream.
        if (MMPD_DSC_GetJpegInfo(&sJpegInfo) != MMP_ERR_NONE) {
            RTNA_DBG_Str(0, "Parse JPEG Info\r\n");
    		return MMP_DSC_ERR_JPEGINFO_FAIL;
        }

        if (sJpegInfo.jpgFormat == MMP_DSC_JPEG_FMT_NOT_BASELINE) {
    		return MMP_DSC_ERR_JPEGINFO_FAIL;
        }
	}
	
	/* Inform USBH the source frame information */
	MMPD_DSC_SetDecMjpegToPreviewSrcAttr(sJpegInfo.usPrimaryWidth, sJpegInfo.usPrimaryHeight);

	MMPD_DSC_SetDecMjpegToPreviewPipe((MMP_UBYTE)m_DecMjpegToPrevwFctlLink.ibcpipeID);
	// For 854x480 with sonix rear cam and LCD is 854x480. Tune IBC 2 performance 
    //   for WIFI preview sometime enc fail and cause image shift issue.
    MMPD_VIDENC_TuneEncodeSecondPipeMCIPriority((MMP_UBYTE)m_DecMjpegToPrevwFctlLink.ibcpipeID);
	/* Initial Decode Output Resolution */
	bDmaRotateEn    = m_sAhcDecMjpegToPreviewInfo.bPreviewRotate;
	RotateType 		= m_sAhcDecMjpegToPreviewInfo.sPreviewDmaDir;
	sFitmode		= m_sAhcDecMjpegToPreviewInfo.sFitMode;
    ulWinOffsetX 	= m_sAhcDecMjpegToPreviewInfo.ulDispStartX;
    ulWinOffsetY 	= m_sAhcDecMjpegToPreviewInfo.ulDispStartY;
    ulDispWidth   	= m_sAhcDecMjpegToPreviewInfo.ulDispWidth;
    ulDispHeight  	= m_sAhcDecMjpegToPreviewInfo.ulDispHeight;
    ulPrevwBufW     = m_sAhcDecMjpegToPreviewInfo.ulPreviewBufW;
    ulPrevwBufH     = m_sAhcDecMjpegToPreviewInfo.ulPreviewBufH;
    DispColor       = m_sAhcDecMjpegToPreviewInfo.sDispColor;
	
	/* Calculate the grab range of decode out image */
	fitrange.fitmode    	= sFitmode;
    fitrange.scalerType 	= MMP_SCAL_TYPE_SCALER;
    fitrange.ulInWidth  	= sJpegInfo.usPrimaryWidth;
    fitrange.ulInHeight 	= sJpegInfo.usPrimaryHeight;
	fitrange.ulOutWidth  	= ulPrevwBufW;
	fitrange.ulOutHeight 	= ulPrevwBufH;

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

    fctlAttr.colormode      = DispColor;
    fctlAttr.fctllink       = m_DecMjpegToPrevwFctlLink;
    fctlAttr.fitrange		= fitrange;
    fctlAttr.grabctl        = grabctl;
    fctlAttr.scalsrc        = MMP_SCAL_SOURCE_JPG;
    fctlAttr.bSetScalerSrc	= MMP_TRUE;
    fctlAttr.usBufCnt  		= pPreviewBuf->usBufCnt;

    for (i = 0; i < fctlAttr.usBufCnt; i++) {
        fctlAttr.ulBaseAddr[i]  = pPreviewBuf->ulYBuf[i];
        fctlAttr.ulBaseUAddr[i] = pPreviewBuf->ulUBuf[i];
        fctlAttr.ulBaseVAddr[i] = pPreviewBuf->ulVBuf[i];
    }

    if (bDmaRotateEn) {
        fctlAttr.bUseRotateDMA  = MMP_TRUE;
        fctlAttr.usRotateBufCnt = pPreviewBuf->usRotateBufCnt;
        
        for (i = 0; i < fctlAttr.usRotateBufCnt; i++) {
            fctlAttr.ulRotateAddr[i]  = pPreviewBuf->ulRotateYBuf[i];
            fctlAttr.ulRotateUAddr[i] = pPreviewBuf->ulRotateUBuf[i];
            fctlAttr.ulRotateVAddr[i] = pPreviewBuf->ulRotateVBuf[i];
        }
    }
    else {
        fctlAttr.bUseRotateDMA = MMP_FALSE;
        fctlAttr.usRotateBufCnt = 0;
    }
    
    m_DecMjpegToPrevwFctlAttr = fctlAttr;
    
    retstatus = MMPD_Fctl_SetPipeAttrForIbcFB(&fctlAttr);

    if (retstatus != MMP_ERR_NONE) {
        return retstatus;
    }

	MMPD_Fctl_ClearPreviewBuf(m_DecMjpegToPrevwFctlLink.ibcpipeID, 0xFFFFFF);

    if (bDmaRotateEn) {
        MMPD_Fctl_LinkPipeToDma(m_DecMjpegToPrevwFctlLink.ibcpipeID,
        						m_sDecMjpegToPreviewWinId, 
        						m_VidRecdConfigs.previewdata[0].DispDevice[usModeIdx],
        						RotateType);
	}
    else {
    	MMPD_Fctl_LinkPipeToDisplay(m_DecMjpegToPrevwFctlLink.ibcpipeID, 
    								m_sDecMjpegToPreviewWinId, 
    								m_VidRecdConfigs.previewdata[0].DispDevice[usModeIdx]);
	}
	
    /* Set display window parameter */
    if (bDmaRotateEn) {
    	// Rotate 90/270 for vertical panel
    	ulRotateW = ulPrevwBufH;
        ulRotateH = ulPrevwBufW;
    }
    else {
    	ulRotateW = ulPrevwBufW;
        ulRotateH = ulPrevwBufH;
    }
    
    dispAttr.usStartX           = 0;
    dispAttr.usStartY           = 0;
    dispAttr.usDisplayOffsetX   = ulWinOffsetX;
   	dispAttr.usDisplayOffsetY   = ulWinOffsetY;
    dispAttr.usDisplayWidth     = ulRotateW;
    dispAttr.usDisplayHeight    = ulRotateH;
    dispAttr.bMirror            = MMP_FALSE;
    
    if (bDmaRotateEn) {
        dispAttr.rotatetype     = MMP_DISPLAY_ROTATE_NO_ROTATE;
    }
    else {
        dispAttr.rotatetype     = m_sAhcDecMjpegToPreviewInfo.sPreviewDmaDir;
    }
       
    MMPD_Display_SetWinToDisplay(m_sDecMjpegToPreviewWinId, &dispAttr);

    /* Set window scaling function */
   	do {
        
        fitrange.fitmode     = sFitmode;
 		fitrange.scalerType	 = MMP_SCAL_TYPE_WINSCALER;
        fitrange.ulInWidth   = dispAttr.usDisplayWidth;
        fitrange.ulInHeight  = dispAttr.usDisplayHeight;
        fitrange.ulOutWidth  = m_sAhcDecMjpegToPreviewInfo.ulDispWidth;
        fitrange.ulOutHeight = m_sAhcDecMjpegToPreviewInfo.ulDispHeight;
        
        fitrange.ulInGrabX	 = 1;
        fitrange.ulInGrabY	 = 1;
        fitrange.ulInGrabW	 = fitrange.ulInWidth;
        fitrange.ulInGrabH	 = fitrange.ulInHeight;

        MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);

   	    retstatus =	MMPD_Display_SetWinScaling(m_sDecMjpegToPreviewWinId, &fitrange, &grabctl);

	} while (retstatus == MMP_DISPLAY_ERR_OVERRANGE);

    /* Must set before preview start. */
    MMPF_USBH_InitDecMjpegToPreview();

#endif
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetDecMjpegToEncodeAttr
//  Description : Need to set before MMPS_3GPRECD_InitDecMjpegToEncode()
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_SetDecMjpegToEncodeAttr(MMP_UBYTE sFitMode, 
                                             MMP_USHORT usSrcWidth, MMP_USHORT usSrcHeight, 
                                             MMP_USHORT usEncWidth, MMP_USHORT usEncHeight)
{
#if (SUPPORT_DEC_MJPEG_TO_ENC_H264)
    m_sAhcDecMjpegToEncodeInfo.bUserDefine  = MMP_TRUE;
    m_sAhcDecMjpegToEncodeInfo.sFitMode		= sFitMode;
    m_sAhcDecMjpegToEncodeInfo.ulVideoEncW  = usEncWidth;
    m_sAhcDecMjpegToEncodeInfo.ulVideoEncH  = usEncHeight;
    
    m_usAhcDecMjpegToEncodeSrcW = usSrcWidth;
    m_usAhcDecMjpegToEncodeSrcH = usSrcHeight;
#endif
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_InitDecMjpegToEncode
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set video record pipe configuration.
 @param[in] *pInputBuf 		Pointer to HW used buffer.
 @param[in] ubEncId         Encode instance ID
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_InitDecMjpegToEncode(MMPD_MP4VENC_INPUT_BUF *pInputBuf, MMP_ULONG ubEncId)
{
#if (SUPPORT_DEC_MJPEG_TO_ENC_H264)  
    MMP_SCAL_FIT_RANGE  	fitrange;
    MMP_SCAL_GRAB_CTRL  	EncodeGrabctl;
    MMPD_FCTL_ATTR 			fctlAttr;
    MMP_USHORT				i;

	/* Parameter Check */
	if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_INVALID) {
		return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
	}

	if (pInputBuf == NULL) {
		return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
	}

    MMPD_DSC_SetDecMjpegToEncodePipe((MMP_UBYTE)m_DecMjpegToEncFctlLink.ibcpipeID);

    /* Config Video Record Pipe */
    fitrange.fitmode        = m_sAhcDecMjpegToEncodeInfo.sFitMode;
	fitrange.scalerType 	= MMP_SCAL_TYPE_SCALER;
    fitrange.ulInWidth 	    = m_usAhcDecMjpegToEncodeSrcW;
   	fitrange.ulInHeight	    = m_usAhcDecMjpegToEncodeSrcH;
    fitrange.ulOutWidth     = m_sAhcDecMjpegToEncodeInfo.ulVideoEncW;
    fitrange.ulOutHeight    = m_sAhcDecMjpegToEncodeInfo.ulVideoEncH;

    fitrange.ulInGrabX 		= 1;
    fitrange.ulInGrabY 		= 1;
    fitrange.ulInGrabW 		= fitrange.ulInWidth;
    fitrange.ulInGrabH 		= fitrange.ulInHeight;

    MMPD_Scaler_GetGCDBestFitScale(&fitrange, &EncodeGrabctl);

    fctlAttr.colormode      = MMP_DISPLAY_COLOR_YUV420_INTERLEAVE;
    fctlAttr.fctllink       = m_DecMjpegToEncFctlLink;
    fctlAttr.fitrange       = fitrange;
    fctlAttr.grabctl        = EncodeGrabctl;
    fctlAttr.scalsrc		= MMP_SCAL_SOURCE_JPG;
    fctlAttr.bSetScalerSrc	= MMP_TRUE;
    fctlAttr.usBufCnt  		= pInputBuf->ulBufCnt;
    fctlAttr.bUseRotateDMA 	= MMP_FALSE;

    for (i = 0; i < fctlAttr.usBufCnt; i++) {
    	if (pInputBuf->ulY[i] != 0)
        	fctlAttr.ulBaseAddr[i] = pInputBuf->ulY[i];
        if (pInputBuf->ulU[i] != 0)	
        	fctlAttr.ulBaseUAddr[i] = pInputBuf->ulU[i];
        if (pInputBuf->ulV[i] != 0)
        	fctlAttr.ulBaseVAddr[i] = pInputBuf->ulV[i];
    }
    
    /* Dual H264 Only support frame mode */
    if (m_VidRecdModes.VidCurBufMode[1] == MMPS_3GPRECD_CURBUF_RT) {
        RTNA_DBG_Str0("Dual H264 Only support frame mode\r\n");
    }
    else {
   		MMPD_IBC_SetH264RT_Enable(MMP_FALSE);
        m_bInitRecPipeConfig = MMP_TRUE;
        MMPD_Fctl_SetPipeAttrForH264FB(&fctlAttr);
    }
	
    MMPD_Fctl_LinkPipeToVideo(m_DecMjpegToEncFctlLink.ibcpipeID, ubEncId);

    /* Tune MCI priority of encode pipe for frame based mode */
    if (m_VidRecdModes.VidCurBufMode[1] == MMPS_3GPRECD_CURBUF_FRAME) {
        MMPD_VIDENC_TuneEncodeSecondPipeMCIPriority(m_DecMjpegToEncFctlLink.ibcpipeID);
    }

#endif
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_InitDecMjpegPreviewStart
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_DecMjpegPreviewStart(MMP_UBYTE ubSnrSel)
{

#if (SUPPORT_DEC_MJPEG_TO_PREVIEW) 
    MMP_ERR sRet = MMP_ERR_NONE;
#if (CCIR656_FORCE_SEL_BT601)//Workaround for MMPF_Display_FrameDoneTrigger handle
        sRet = MMPD_Fctl_EnablePreview(SCD_SENSOR, m_DecMjpegToPrevwFctlLink.ibcpipeID, MMP_TRUE, MMP_FALSE);
#else
        sRet = MMPD_Fctl_EnablePreview(ubSnrSel, m_DecMjpegToPrevwFctlLink.ibcpipeID, MMP_TRUE, MMP_FALSE); 
#endif

    if(sRet != MMP_ERR_NONE){MMP_PRINT_RET_ERROR(0, sRet, "",gubMmpDbgBk); return sRet;}    
#endif    
    return MMP_ERR_NONE;
}

MMP_ERR MMPS_3GPRECD_DecMjpegPreviewStop(MMP_UBYTE ubSnrSel)
{

#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
    MMP_ERR sRet = MMP_ERR_NONE;

    sRet = MMPD_Fctl_EnablePreview(ubSnrSel, m_DecMjpegToPrevwFctlLink.ibcpipeID, MMP_FALSE, MMP_FALSE); 
    
    if(sRet != MMP_ERR_NONE){MMP_PRINT_RET_ERROR(0, sRet, "",gubMmpDbgBk); return sRet;}        
#endif
    return MMP_ERR_NONE;
}

#if 0
void ____VR_Common_Record_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_CustomedEncResol
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set VideoR encode resolution
 @param[in] ubEncIdx    encode instance ID
 @param[in] sFitMode  	scaler fit mode
 @param[in] usWidth  	encode width
 @param[in] usHeight 	encode height
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_CustomedEncResol(MMP_UBYTE ubEncIdx, MMP_UBYTE sFitMode, MMP_USHORT usWidth, MMP_USHORT usHeight)
{
    m_sAhcVideoRecdInfo[ubEncIdx].bUserDefine  	= MMP_TRUE;
    m_sAhcVideoRecdInfo[ubEncIdx].sFitMode		= sFitMode;
    m_sAhcVideoRecdInfo[ubEncIdx].ulVideoEncW  	= usWidth;
    m_sAhcVideoRecdInfo[ubEncIdx].ulVideoEncH 	= usHeight;
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetEncodeRes
//  Description : This function need to called before MMPS_3GPRECD_RecordStart()
//------------------------------------------------------------------------------
static MMP_ERR MMPS_3GPRECD_SetEncodeRes(MMP_UBYTE ubEncIdx)
{
    MMP_ULONG 	ulScalInW, ulScalInH;
    MMP_ULONG   ulEncWidth, ulEncHeight;
	
	if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_INVALID) {
		return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
	}

    if (ubEncIdx == 0)
	    MMPS_Sensor_GetCurPrevScalInputRes(m_ubVidRecModeSnrId, &ulScalInW, &ulScalInH);
    #if (DUALENC_SUPPORT)
    else
	    MMPS_Sensor_GetCurPrevScalInputRes(m_ub2ndH264SnrId, &ulScalInW, &ulScalInH);
    #endif

	/* Calculate encode parameters */
    if (m_sAhcVideoRecdInfo[ubEncIdx].bUserDefine) {
        ulEncWidth  = m_sAhcVideoRecdInfo[ubEncIdx].ulVideoEncW;
        ulEncHeight = m_sAhcVideoRecdInfo[ubEncIdx].ulVideoEncH;
    }
    else {
        ulEncWidth  = m_VidRecdConfigs.usEncWidth[m_VidRecdModes.usVideoEncResIdx[ubEncIdx]];
        ulEncHeight = m_VidRecdConfigs.usEncHeight[m_VidRecdModes.usVideoEncResIdx[ubEncIdx]];
    }
    m_ulVREncodeW[ubEncIdx] = ulEncWidth;
    m_ulVREncodeH[ubEncIdx] = ulEncHeight;

	if (ulEncWidth & 0x0F || ulEncHeight & 0x0F) {
		RTNA_DBG_Str0("The Encode Width/Height must be 16x\r\n");
	}

    //if ( ubEncIdx == 0 ) //Only do below because 2nd sensor (TV decoder) may need to scale-up input data
    //{
        // Align with source ratio
    //    ulEncHeight = ulEncWidth * ulScalInH / ulScalInW;
    //}

	// The RT mode need padding 8 lines and Frame/RT mode need set crop 8 line
	if (m_VidRecdModes.VidCurBufMode[ubEncIdx] == MMPS_3GPRECD_CURBUF_RT) {
        ulEncHeight = (ulEncHeight == 1088) ? 1080 : ulEncHeight;
	}
	else {
        //ulEncHeight = (ulEncHeight == 1080) ? 1088 : ulEncHeight;
	}

	if ((ulEncWidth > ulScalInW) || (ulEncHeight > ulScalInH)) {
		RTNA_DBG_Str0("Check the input/output setting!\r\n");
	}

	m_ulVREncodeBufW[ubEncIdx] = ulEncWidth;
	m_ulVREncodeBufH[ubEncIdx] = ulEncHeight;

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetRecordPipeStatus
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Return in video record status or not.
 @param[in]  ubEncIdx   encode instance ID.
 @param[out] bEnable record enable.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_GetRecordPipeStatus(MMP_UBYTE ubEncIdx, MMP_BOOL *bEnable)
{
    *bEnable = m_bVidRecordActive[ubEncIdx];
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetRecordPipeBufWidth_Height
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_GetRecordPipeBufWidth_Height(MMP_UBYTE ubEncIdx, MMP_ULONG *pulBufW, MMP_ULONG *pulBufH)
{
    *pulBufW   = m_ulVREncodeBufW[ubEncIdx];
    *pulBufH  = m_ulVREncodeBufH[ubEncIdx];

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetRecordPipe
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_GetRecordPipe(MMP_UBYTE ubEncIdx, MMP_IBC_PIPEID *pPipe)
{
    if (ubEncIdx == 0)
        *pPipe = m_RecordFctlLink.ibcpipeID;
    #if (SUPPORT_DUAL_SNR)||(DUALENC_SUPPORT)
    else
        *pPipe = m_2ndRecFctlLink.ibcpipeID;
    #endif    
          
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetCropping
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set crop for video width/height.

 The input paramters should be even and < 16,
  e.g. usBottom=8 for 1088 => 1080
 @param[in] usTop cropping offset from top.
 @param[in] usBottom cropping offset from bottom.
 @param[in] usLeft cropping offset from left.
 @param[in] usRight cropping offset from right.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetCropping(MMP_ULONG  ulEncId,
		                         MMP_USHORT usTop,
							     MMP_USHORT usBottom,
							     MMP_USHORT usLeft,
							     MMP_USHORT usRight)
{
    return MMPD_VIDENC_SetCropping(ulEncId, usTop, usBottom, usLeft, usRight);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetPadding
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set padding info for video height.

 @param[in] usType padding type 0: zero, 1: repeat
 @param[in] usCnt  the height line offset which need to pad
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetPadding(MMP_ULONG ulEncId, MMP_USHORT usType, MMP_USHORT usCnt)
{
    return MMPD_H264ENC_SetPadding(ulEncId, usType, usCnt);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_ModifyAVIListAtom
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Modify LIST Atom for AVI format.

 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_ModifyAVIListAtom(MMP_BOOL bEnable, MMP_BYTE *pStr)
{
    return MMPD_3GPMGR_ModifyAVIListAtom(bEnable, pStr);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetStillCaptureAddr
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_GetStillCaptureAddr(MMP_ULONG *pulSramAddr, MMP_ULONG *pulDramAddr)
{
    *pulSramAddr = m_ulVidRecCaptureSramAddr;
    *pulDramAddr = m_ulVidRecCaptureDramAddr;

    return MMP_ERR_NONE;
}

#if 0
void ____VR_1st_Record_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetMjpgStreamAddr
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_GetMjpgStreamAddr(MMP_ULONG *pulSramAddr, MMP_ULONG *pulDramAddr)
{
#if (defined(WIFI_PORT) && WIFI_PORT == 1)
    *pulSramAddr = m_ulVidRecMjpgStreamSramAddr;
    *pulDramAddr = m_ulVidRecMjpgStreamDramAddr;
#endif
    return MMP_ERR_NONE;
}
//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetRecdPipeConfig
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set video record pipe configuration.
 @param[in] *pInputBuf 		Pointer to HW used buffer.
 @param[in] usEncInputW 	The encode buffer width (for scaler out stage).
 @param[in] usEncInputH 	The encode buffer height (for scaler out stage).
 @param[in] ubEncId         encode instance ID
 @retval MMP_ERR_NONE Success.
*/
static MMP_ERR MMPS_3GPRECD_SetRecdPipeConfig(MMPD_MP4VENC_INPUT_BUF 	*pInputBuf,
											  MMP_USHORT			 	usEncInputW,
											  MMP_USHORT			 	usEncInputH,
											  MMP_USHORT                ubEncId)
{
    MMP_ULONG				ulScalInW, ulScalInH;
    MMP_SCAL_FIT_MODE		sFitMode;
    MMP_SCAL_FIT_RANGE  	fitrange;
    MMP_SCAL_GRAB_CTRL  	EncodeGrabctl;
    MMPD_FCTL_ATTR 			fctlAttr;
    MMP_USHORT				i;
	MMP_SCAL_FIT_RANGE 		sFitRangeBayer;
	MMP_SCAL_GRAB_CTRL		sGrabctlBayer;
	MMP_USHORT				usCurZoomStep = 0;

	/* Parameter Check */
	if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_INVALID) {
		return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
	}

	if (pInputBuf == NULL) {
		return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
	}

	/* Get the sensor parameters */
    MMPS_Sensor_GetCurPrevScalInputRes(m_ubVidRecModeSnrId, &ulScalInW, &ulScalInH);

	MMPD_BayerScaler_GetZoomInfo(MMP_BAYER_SCAL_DOWN, &sFitRangeBayer, &sGrabctlBayer); 

    if (m_sAhcVideoRecdInfo[0].bUserDefine) {
    	sFitMode = m_sAhcVideoRecdInfo[0].sFitMode;
    }
    else {
    	sFitMode = MMP_SCAL_FITMODE_OUT;
    }

	/* Initial zoom relative config */ 
	MMPS_3GPRECD_InitDigitalZoomParam(m_RecordFctlLink.scalerpath);

	MMPS_3GPRECD_RestoreDigitalZoomRange(m_RecordFctlLink.scalerpath);

    if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_2PIPE)
    {
        // Config Video Record Pipe
        fitrange.fitmode        = sFitMode;
		fitrange.scalerType 	= MMP_SCAL_TYPE_SCALER;

		if (MMP_IsVidPtzEnable()) {
        	fitrange.ulInWidth	= sFitRangeBayer.ulOutWidth;
        	fitrange.ulInHeight	= sFitRangeBayer.ulOutHeight;
		}
		else {
        	fitrange.ulInWidth 	= ulScalInW;
       	 	fitrange.ulInHeight	= ulScalInH;
		}
		fitrange.ulOutWidth     = usEncInputW;
		if (usEncInputW == 1920 && usEncInputH == 1088) {
	        fitrange.ulOutHeight    = 1080;
		}
		else {
	        fitrange.ulOutHeight    = usEncInputH;
		}
	    fitrange.ulInGrabX 		= 1;
	    fitrange.ulInGrabY 		= 1;
	    fitrange.ulInGrabW 		= fitrange.ulInWidth;
	    fitrange.ulInGrabH 		= fitrange.ulInHeight;

        MMPD_PTZ_InitPtzInfo(m_RecordFctlLink.scalerpath,
        					 fitrange.fitmode,
				             fitrange.ulInWidth, fitrange.ulInHeight,
				             fitrange.ulOutWidth, fitrange.ulOutHeight);
		
		// Be sync with preview path : TBD
		MMPD_PTZ_GetCurPtzStep(m_PreviewFctlLink.scalerpath, NULL, &usCurZoomStep, NULL, NULL);

        if (usCurZoomStep > m_VidRecordZoomInfo.usMaxZoomSteps) {
            usCurZoomStep = m_VidRecordZoomInfo.usMaxZoomSteps;
        }
        
		MMPD_PTZ_CalculatePtzInfo(m_RecordFctlLink.scalerpath, usCurZoomStep, 0, 0);

		MMPD_PTZ_GetCurPtzInfo(m_RecordFctlLink.scalerpath, &fitrange, &EncodeGrabctl);
		
		if (MMP_IsVidPtzEnable()) {
			MMPD_PTZ_ReCalculateGrabRange(&fitrange, &EncodeGrabctl);
		}

        fctlAttr.colormode      = MMP_DISPLAY_COLOR_YUV420_INTERLEAVE;
        fctlAttr.fctllink       = m_RecordFctlLink;
        fctlAttr.fitrange       = fitrange;
        fctlAttr.grabctl        = EncodeGrabctl;
        fctlAttr.scalsrc		= MMP_SCAL_SOURCE_ISP;
        fctlAttr.bSetScalerSrc	= MMP_TRUE;
        fctlAttr.usBufCnt  		= pInputBuf->ulBufCnt;
        fctlAttr.bUseRotateDMA 	= MMP_FALSE;

        for (i = 0; i < fctlAttr.usBufCnt; i++) {
        	if (pInputBuf->ulY[i] != 0)
            	fctlAttr.ulBaseAddr[i] = pInputBuf->ulY[i];
            if (pInputBuf->ulU[i] != 0)	
            	fctlAttr.ulBaseUAddr[i] = pInputBuf->ulU[i];
            if (pInputBuf->ulV[i] != 0)
            	fctlAttr.ulBaseVAddr[i] = pInputBuf->ulV[i];
        }

        if (m_VidRecdModes.VidCurBufMode[0] == MMPS_3GPRECD_CURBUF_RT) {
            MMPD_IBC_SetH264RT_Enable(MMP_TRUE);
            MMPD_Fctl_SetPipeAttrForH264Rt(&fctlAttr, usEncInputW);
        }
        else {
       		MMPD_IBC_SetH264RT_Enable(MMP_FALSE);
            m_bInitRecPipeConfig = MMP_TRUE;
            MMPD_Fctl_SetPipeAttrForH264FB(&fctlAttr);
        }
        // For raw zoom case. If zoom in happens change scaler delay and raw fetch line delay.
        if (m_VidRecdConfigs.bRawPreviewEnable[0] == MMP_TRUE) {
            MMP_BOOL ubCkScalarUp;
            MMPF_Scaler_CheckScaleUp(m_RecordFctlLink.scalerpath, &ubCkScalarUp);
			if (ubCkScalarUp) {
	            MMPF_Scaler_SetPixelDelay(m_RecordFctlLink.ibcpipeID, 17, 20);
	            MMPF_RAWPROC_SetFetchLineDelay(0xC);
			}
			else {
				MMPF_Scaler_SetPixelDelay(m_RecordFctlLink.ibcpipeID, 1, 1);
	            MMPF_RAWPROC_SetFetchLineDelay(0x20);
			}
		}		
        MMPD_Fctl_LinkPipeToVideo(m_RecordFctlLink.ibcpipeID, ubEncId);
    }
    #if (SUPPORT_LDC_RECD)
    else if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_LB_SINGLE ||
    		 m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_LB_MULTI 	||
    		 m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_MULTISLICE)
    {
        // Config pipe for Encode
        fitrange.fitmode        = MMP_SCAL_FITMODE_OUT;
        fitrange.scalerType 	= MMP_SCAL_TYPE_SCALER;
        fitrange.ulInWidth      = m_ulLdcMaxOutWidth;
        fitrange.ulInHeight     = m_ulLdcMaxOutHeight;
        fitrange.ulOutWidth     = usEncInputW;
        fitrange.ulOutHeight	= usEncInputH;

	    fitrange.ulInGrabX 		= 1;
	    fitrange.ulInGrabY 		= 1;
	    fitrange.ulInGrabW 		= fitrange.ulInWidth;
	    fitrange.ulInGrabH 		= fitrange.ulInHeight;

        MMPD_PTZ_InitPtzInfo(m_RecordFctlLink.scalerpath,
        					 fitrange.fitmode,
				             fitrange.ulInWidth, fitrange.ulInHeight, 
				             fitrange.ulOutWidth, fitrange.ulOutHeight);

		// Be sync with preview path : TBD
		MMPD_PTZ_GetCurPtzStep(m_PreviewFctlLink.scalerpath, NULL, &usCurZoomStep, NULL, NULL);

        if (usCurZoomStep > m_VidRecordZoomInfo.usMaxZoomSteps) {
            usCurZoomStep = m_VidRecordZoomInfo.usMaxZoomSteps;
        }

		MMPD_PTZ_CalculatePtzInfo(m_RecordFctlLink.scalerpath, usCurZoomStep, 0, 0);

		MMPD_PTZ_GetCurPtzInfo(m_RecordFctlLink.scalerpath, &fitrange, &EncodeGrabctl);

		if (MMP_IsVidPtzEnable()) {
			MMPD_PTZ_ReCalculateGrabRange(&fitrange, &EncodeGrabctl);
		}

        fctlAttr.colormode      = MMP_DISPLAY_COLOR_YUV420_INTERLEAVE;
        fctlAttr.fctllink       = m_RecordFctlLink;
        fctlAttr.fitrange       = fitrange;
        fctlAttr.grabctl        = EncodeGrabctl;
        fctlAttr.usBufCnt  		= pInputBuf->ulBufCnt;
    	fctlAttr.scalsrc		= MMP_SCAL_SOURCE_LDC;
    	fctlAttr.bSetScalerSrc	= MMP_TRUE;
        fctlAttr.bUseRotateDMA 	= MMP_FALSE;
		fctlAttr.usRotateBufCnt = 0;

        for (i = 0; i < fctlAttr.usBufCnt; i++) {
        	if (pInputBuf->ulY[i] != 0)
            	fctlAttr.ulBaseAddr[i] = pInputBuf->ulY[i];
            if (pInputBuf->ulU[i] != 0)	
            	fctlAttr.ulBaseUAddr[i] = pInputBuf->ulU[i];
            if (pInputBuf->ulV[i] != 0)
            	fctlAttr.ulBaseVAddr[i] = pInputBuf->ulV[i];
        }

        if (m_VidRecdModes.VidCurBufMode[0] == MMPS_3GPRECD_CURBUF_RT) {
            MMPD_IBC_SetH264RT_Enable(MMP_TRUE);
            MMPD_Fctl_SetPipeAttrForH264Rt(&fctlAttr, usEncInputW);
        }
       	else {
       		MMPD_IBC_SetH264RT_Enable(MMP_FALSE);
            m_bInitRecPipeConfig = MMP_TRUE;
            MMPD_Fctl_SetPipeAttrForH264FB(&fctlAttr);
        }

        MMPD_Fctl_LinkPipeToVideo(m_RecordFctlLink.ibcpipeID, ubEncId);
    }
    #endif

    // Tune MCI priority of encode pipe for frame based mode
    if (m_VidRecdModes.VidCurBufMode[0] == MMPS_3GPRECD_CURBUF_FRAME) {
        MMPD_VIDENC_TuneEncodePipeMCIPriority(m_RecordFctlLink.ibcpipeID);

        #if (HANDLE_JPEG_EVENT_BY_QUEUE) && (REAR_CAM_TYPE == REAR_CAM_TYPE_SONIX_MJPEG2H264)
        // ICON0 may overflow with sensor OV2710. Decrease other pipe priority setting. 
        //MMPD_VIDENC_TuneEncodePipeMCIPriority(MMPS_3GPRECD_GetDecMjpegToPreviewPipeId());
        
        MMPD_VIDENC_TuneEncodeSecondPipeMCIPriority(MMPS_3GPRECD_GetDecMjpegToPreviewPipeId());
        // MMPF_MCI_SetModuleMaxPriority(MMPF_MCI_SRC_DMAM0_SD0_JPGDEC);
        //MMPF_MCI_SetModuleMaxPriority(MMPF_MCI_SRC_DMAM1_SD1_JPGCB);
        //MMPF_MCI_SetModuleMaxPriority(MMPF_MCI_SRC_RAWS2_JPGLB);
        #endif
    }

    return MMP_ERR_NONE;
}
static MMP_ERR MMPS_3GPRECD_StillCaptureMemConfig(MMPD_MP4VENC_INPUT_BUF *pInputBuf)
{
    MMP_ERR	    mmpstatus;
    MMP_USHORT  i;
    for (i = 0; i < pInputBuf->ulBufCnt; i++) 
	{
	    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
        MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, m_RecordFctlLink.ibcpipeID);
        MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 2, i);
        MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 4, pInputBuf->ulY[i]);
        MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 8, pInputBuf->ulU[i]);
        MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 12, pInputBuf->ulV[i]);
        mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_PREVIEW_BUF | BUFFER_ADDRESS);
	    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
	}
    return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_EnableRecordPipe
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Turn on and off record for video encode.

 @param[in] bEnable Enable and disable scaler path for video encode.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_EnableRecordPipe(MMP_BOOL bEnable)
{
    if (m_bVidRecordActive[0] ^ bEnable)
    {
        if (bEnable) {
            MMPD_Fctl_EnablePreview(m_ubVidRecModeSnrId, m_RecordFctlLink.ibcpipeID, bEnable, MMP_FALSE);
        }
        else {
            MMPD_Fctl_EnablePreview(m_ubVidRecModeSnrId, m_RecordFctlLink.ibcpipeID, bEnable, MMP_TRUE);
        }

        m_bVidRecordActive[0] = bEnable;
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_StopRecord
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Stop video recording and fill 3GP tail.

 It works after video start, pause and resume.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
 @retval MMP_3GPRECD_ERR_OPEN_FILE_FAILURE Open file failed.
 @retval MMP_3GPRECD_ERR_CLOSE_FILE_FAILURE Close file failed.
*/
MMP_ERR MMPS_3GPRECD_StopRecord(void)
{
	MMP_ULONG          ulVidNumOpening = 0;
    MMPD_MP4VENC_FW_OP status_fw;

    #if (DUALENC_SUPPORT == 1)
    MMPS_3GPRECD_StopDualH264();
	RTNA_DBG_Str(0, "Stop Dual H264\r\n");
	#endif

    MMPD_VIDENC_GetNumOpen(&ulVidNumOpening);

    if (m_VidRecdID == INVALID_ENC_ID)
    	status_fw = MMPD_MP4VENC_FW_OP_NONE;
    else
    	MMPD_VIDENC_GetStatus(m_VidRecdID, &status_fw);

    if ((status_fw == MMPD_MP4VENC_FW_OP_START) ||
        (status_fw == MMPD_MP4VENC_FW_OP_PAUSE) ||
        (status_fw == MMPD_MP4VENC_FW_OP_RESUME) ||
        (status_fw == MMPD_MP4VENC_FW_OP_STOP)) {
        
        MMPD_3GPMGR_StopCapture(m_VidRecdID, MMP_3GPRECD_FILETYPE_VIDRECD);

        do {
            MMPD_VIDENC_GetStatus(m_VidRecdID, &status_fw);
        } while (status_fw != MMPD_MP4VENC_FW_OP_STOP);

        MMPD_VIDENC_GetNumOpen(&ulVidNumOpening);

        if (ulVidNumOpening == 0) {
            if (MMPD_VIDENC_IsModuleInit(MMPF_VIDENC_MODULE_H264)) {
            	MMPD_VIDENC_DeinitModule(MMPF_VIDENC_MODULE_H264);
            }
    	}

        m_VidRecdID = INVALID_ENC_ID;
#if (DUALENC_SUPPORT)		
		glEncID[0] = m_VidRecdID;
		gbTotalEncNum -= 1;   
#endif
        MMPS_3GPRECD_EnableRecordPipe(MMP_FALSE);

        MMPD_VIDENC_EnableClock(MMP_FALSE);

        if (m_VidRecdConfigs.bAsyncMode) {
            MMPD_AUDIO_SetRecordSilence(MMP_FALSE);
        }
    }
    else if (status_fw == MMPD_MP4VENC_FW_OP_PREENCODE) {
        
        MMPD_3GPMGR_StopCapture(m_VidRecdID, MMP_3GPRECD_FILETYPE_VIDRECD);

        do {
            MMPD_VIDENC_GetStatus(m_VidRecdID, &status_fw);
        } while (status_fw != MMPD_MP4VENC_FW_OP_STOP);

        MMPD_VIDENC_GetNumOpen(&ulVidNumOpening);

    	if (ulVidNumOpening == 0) {
            if (MMPD_VIDENC_IsModuleInit(MMPF_VIDENC_MODULE_H264)) {
                MMPD_VIDENC_DeinitModule(MMPF_VIDENC_MODULE_H264);
            }
        }

        m_VidRecdID = INVALID_ENC_ID;
#if (DUALENC_SUPPORT)		
		glEncID[0] = m_VidRecdID;
		gbTotalEncNum -= 1;   
#endif        
        MMPS_3GPRECD_EnableRecordPipe(MMP_FALSE);

        MMPD_VIDENC_EnableClock(MMP_FALSE);
        
        MMPD_AUDIO_SetRecordSilence(MMP_FALSE);
    }
    else if (status_fw == MMPD_MP4VENC_FW_OP_NONE) {

    	if (ulVidNumOpening > 0 && m_VidRecdID != INVALID_ENC_ID) {

    		MMPD_VIDENC_DeInitInstance(m_VidRecdID);

    		m_VidRecdID = INVALID_ENC_ID;
#if (DUALENC_SUPPORT)			
			glEncID[0] = m_VidRecdID;
			gbTotalEncNum -= 1;   
#endif
    		if (ulVidNumOpening == 0) {
    			MMPD_VIDENC_IsModuleInit(MMPF_VIDENC_MODULE_H264);
	            if (MMPD_VIDENC_IsModuleInit(MMPF_VIDENC_MODULE_H264)) {
	            	MMPD_VIDENC_DeinitModule(MMPF_VIDENC_MODULE_H264);
	            }
	    	}
    	}	
    }
    else {
        //Restore DRAM end address to preview end address
        m_ulVidRecDramEndAddr = m_ulVideoPreviewEndAddr;

        return MMP_3GPRECD_ERR_GENERAL_ERROR;
    }

    //Restore DRAM end address to preview end address
    m_ulVidRecDramEndAddr = m_ulVideoPreviewEndAddr;
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_PauseRecord
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Pause video recording. It works after video start and resume.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPS_3GPRECD_PauseRecord(void)
{
    MMPD_MP4VENC_FW_OP status_fw;

    MMPD_VIDENC_GetStatus(m_VidRecdID, &status_fw);

    if ((status_fw == MMPD_MP4VENC_FW_OP_START) ||
        (status_fw == MMPD_MP4VENC_FW_OP_RESUME)) {

        MMPD_3GPMGR_PauseCapture();

        do {
            MMPD_VIDENC_GetStatus(m_VidRecdID, &status_fw);
            if (status_fw == MMPD_MP4VENC_FW_OP_STOP) {
                return MMP_ERR_NONE;
            }
        } while (status_fw != MMPD_MP4VENC_FW_OP_PAUSE);

        return MMP_ERR_NONE;
    }
    else {
        return MMP_3GPRECD_ERR_GENERAL_ERROR;
    }
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_ResumeRecord
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Resume video recording. It works after video pause.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPS_3GPRECD_ResumeRecord(void)
{
    MMPD_MP4VENC_FW_OP status_fw;

    MMPD_VIDENC_GetStatus(m_VidRecdID, &status_fw);

    if (status_fw == MMPD_MP4VENC_FW_OP_PAUSE) {

        MMPD_3GPMGR_ResumeCapture();

        do {
            MMPD_VIDENC_GetStatus(m_VidRecdID, &status_fw);
            if (status_fw == MMPD_MP4VENC_FW_OP_STOP) {
                return MMP_ERR_NONE;
            }            
        } while (status_fw != MMPD_MP4VENC_FW_OP_RESUME);

        return MMP_ERR_NONE;
    }
    else {
        return MMP_3GPRECD_ERR_GENERAL_ERROR;
    }
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_PreRecord
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Pre start video recording.

 It start record without enable file saving
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPS_3GPRECD_PreRecord(MMP_ULONG ulPreCaptureMs)
{
    MMP_ULONG           enc_id = 0;
    MMPD_MP4VENC_FW_OP  status_vid;
    MMP_ERR             status;
    MMP_ULONG           ulFps;
    MMP_ULONG           EncWidth, EncHeight;
    MMP_ULONG64         ullBitrate64 		= m_VidRecdModes.ulBitrate[0];
    MMP_ULONG64         ullTimeIncr64 		= m_VidRecdModes.ContainerFrameRate[0].usVopTimeIncrement;
    MMP_ULONG64         ullTimeResol64 		= m_VidRecdModes.ContainerFrameRate[0].usVopTimeIncrResol;
    MMP_ULONG           ulTargetFrameSize 	= (MMP_ULONG)((ullBitrate64 * ullTimeIncr64)/(ullTimeResol64 * 8));
    MMP_ULONG           ulExpectBufSize 	= 0;
    MMP_ULONG           ulMaxPreEncMs 		= 0;
    #if (EMERGENTRECD_SUPPORT == 1)
    MMP_ULONG           ulMaxAudPreEncMs 	= 0;
    #endif

    MMPS_3GPRECD_SetEncodeRes(0);

    EncWidth  = m_ulVREncodeW[0];
    EncHeight = m_ulVREncodeH[0];

    ulFps = (m_VidRecdModes.SnrInputFrameRate[0].usVopTimeIncrResol - 1 +
            m_VidRecdModes.SnrInputFrameRate[0].usVopTimeIncrement) /
            m_VidRecdModes.SnrInputFrameRate[0].usVopTimeIncrement;

    status = MMPD_VIDENC_CheckCapability(EncWidth, EncHeight, ulFps);

    if (status != MMP_ERR_NONE)
        return status;

    #if (EMERGENTRECD_SUPPORT == 1)
    ulMaxAudPreEncMs = (m_VidRecdConfigs.ulAudioCompBufSize)/(m_VidRecdModes.ulAudBitrate >> 3) * 1000 - 1000;
    #endif

    ulExpectBufSize = m_VidRecdConfigs.ulVideoCompBufSize - (ulTargetFrameSize * 3);

    #if (EMERGENTRECD_SUPPORT == 1)
    ulMaxPreEncMs = (MMP_ULONG)(((MMP_ULONG64)ulExpectBufSize * 1000) / (m_VidRecdModes.ulBitrate[0] >> 3)) - 1000;
    #else
    ulMaxPreEncMs = (MMP_ULONG)(((MMP_ULONG64)ulExpectBufSize * 900) / (m_VidRecdModes.ulBitrate[0] >> 3));
    #endif

    #if (EMERGENTRECD_SUPPORT == 1)
    if (ulMaxPreEncMs > ulMaxAudPreEncMs) {
    	ulMaxPreEncMs = ulMaxAudPreEncMs;
    }
    #endif	
    
    if (ulPreCaptureMs > ulMaxPreEncMs) {
        PRINTF("The pre-record duration %d is over preferred %d ms\r\n", ulPreCaptureMs, ulMaxPreEncMs);
    	ulPreCaptureMs = ulMaxPreEncMs;
    }

    if (m_VidRecdID == INVALID_ENC_ID)
    	status_vid = MMPD_MP4VENC_FW_OP_NONE;
    else
    	MMPD_VIDENC_GetStatus(m_VidRecdID, &status_vid);

    if ((status_vid == MMPD_MP4VENC_FW_OP_NONE) ||
        (status_vid == MMPD_MP4VENC_FW_OP_STOP)) {

        if (MMPD_VIDENC_GetFormat() == MMPD_MP4VENC_FORMAT_H264) {

            if (!MMPD_VIDENC_IsModuleInit(MMPD_VIDENC_MODULE_H264)) {
            	MMP_ERR	mmpstatus;

            	mmpstatus = MMPD_VIDENC_InitModule(MMPD_VIDENC_MODULE_H264);
                if (mmpstatus != MMP_ERR_NONE)
                	return mmpstatus;
            }

            MMPD_VIDENC_SetMemStart(m_ulVideoPreviewEndAddr);

            if (MMPS_3GPRECD_SetH264MemoryMap(  &enc_id,
                                                EncWidth,
                                                EncHeight,
                                                m_ulVidRecSramAddr,
                                                m_ulVideoPreviewEndAddr))
            {
                PRINTF("Alloc mem for video pre-record failed\r\n");
                return MMP_3GPRECD_ERR_MEM_EXHAUSTED;
            }
        
            switch (m_VidRecdModes.VidCurBufMode[0]) {
            case MMPS_3GPRECD_CURBUF_FRAME:
                MMPD_VIDENC_SetCurBufMode(enc_id, MMPD_MP4VENC_CURBUF_FRAME);
                break;
            case MMPS_3GPRECD_CURBUF_RT:
                MMPD_VIDENC_SetCurBufMode(enc_id, MMPD_MP4VENC_CURBUF_RT);
                break;
            default:
                return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
            }

        	if (MMPS_3GPRECD_EnableRecordPipe(MMP_TRUE) != MMP_ERR_NONE) {
        		PRINTF("Enable Video Record: Fail\r\n");
                return MMP_3GPRECD_ERR_GENERAL_ERROR;
            }
        }
        else {
            return MMP_3GPRECD_ERR_GENERAL_ERROR;
        }

        // Encoder parameters
        if (((EncWidth == 1920) && (EncHeight == 1088))||((EncWidth == 640) && (EncHeight == 368))) {
            MMPS_3GPRECD_SetCropping(enc_id, 0, 8, 0, 0);
            MMPS_3GPRECD_SetPadding(enc_id, MMPS_3GPRECD_PADDING_REPEAT, 8);
        }
        else {
            MMPS_3GPRECD_SetCropping(enc_id, 0, 0, 0, 0);
            MMPS_3GPRECD_SetPadding(enc_id, MMPS_3GPRECD_PADDING_NONE, 0);
        }

        if (MMPD_VIDENC_SetResolution(enc_id, EncWidth, EncHeight) != MMP_ERR_NONE) {
            return MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS;
        }

        if (MMPD_VIDENC_SetProfile(enc_id, m_VidRecdModes.VisualProfile[0]) != MMP_ERR_NONE) {
            return MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS;
        }
        MMPD_VIDENC_SetEncodeMode();

        MMPD_3GPMGR_SetGOP(m_VidRecdModes.usPFrameCount[0], m_VidRecdModes.usBFrameCount[0], MMP_3GPRECD_FILETYPE_VIDRECD);
        MMPD_VIDENC_SetGOP(enc_id, m_VidRecdModes.usPFrameCount[0], m_VidRecdModes.usBFrameCount[0]);

        MMPD_VIDENC_SetQuality(enc_id, ulTargetFrameSize, (MMP_ULONG)ullBitrate64);

        // Sensor input frame rate
        MMPD_VIDENC_SetSnrFrameRate(enc_id,
        		                    m_VidRecdModes.SnrInputFrameRate[0].usVopTimeIncrement,
        		                    m_VidRecdModes.SnrInputFrameRate[0].usVopTimeIncrResol);
        // Encode frame rate
        MMPD_VIDENC_SetEncFrameRate(enc_id,
        		                    m_VidRecdModes.VideoEncFrameRate[0].usVopTimeIncrement,
				                    m_VidRecdModes.VideoEncFrameRate[0].usVopTimeIncrResol);
        // Container frame rate
        MMPD_3GPMGR_SetFrameRate(MMP_3GPRECD_FILETYPE_VIDRECD,
        		                 m_VidRecdModes.ContainerFrameRate[0].usVopTimeIncrResol,
				                 m_VidRecdModes.ContainerFrameRate[0].usVopTimeIncrement);

        MMPD_VIDENC_EnableClock(MMP_TRUE);
        MMPD_3GPMGR_PreCapture(enc_id, ulPreCaptureMs);

        do {
            MMPD_VIDENC_GetStatus(enc_id, &status_vid);
        } while (status_vid != MMPD_MP4VENC_FW_OP_PREENCODE);

        #if (UVC_VIDRECD_SUPPORT == 1)
    	if (gbUVCRecdSupport) { 
    		MMPD_3GPMGR_EnableUVCRecd();
    	}
    	#endif
    }
    else {
        return MMP_3GPRECD_ERR_GENERAL_ERROR;
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_StartRecord
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Start video recording.

 It can saves the 3GP file to host memory or memory card.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPS_3GPRECD_StartRecord(void)
{
    MMP_ULONG           enc_id = 0;
    MMPD_MP4VENC_FW_OP  status_vid;
    MMP_ERR             status;
    MMP_ULONG           EncWidth, EncHeight;
    MMP_ULONG64         ullBitrate64 = m_VidRecdModes.ulBitrate[0];
    MMP_ULONG           ulTargetFrameSize;
    #if (DUALENC_SUPPORT == 0)
    void                *pSeamlessCB;
    MMP_ULONG			ulAvaSize;
    MMP_ULONG           status_tx;
    #endif
    MMP_ULONG           ulFps;
	// Add error handle for start record.
	// Root cause: Slow card happens during seamless record. Will call seamless flow and cause gbTotalEncNum still have value.
	// need to call stop record here to correct kernel state.
	#if (DUALENC_SUPPORT == 1)
	MMP_ULONG          ulVidNumOpening = 0;
	if (gbTotalEncNum) {
	    MMPD_VIDENC_GetNumOpen(&ulVidNumOpening);
		if (ulVidNumOpening == 0) {
			MMPS_3GPRECD_StopRecord();
			printc("start record and h264 instance not match\r\n");
		}
	}
	
	#endif
    
    MMPS_3GPRECD_SetEncodeRes(0);

    EncWidth  = m_ulVREncodeW[0];
    EncHeight = m_ulVREncodeH[0];

    ulFps = (m_VidRecdModes.SnrInputFrameRate[0].usVopTimeIncrResol - 1 +
            m_VidRecdModes.SnrInputFrameRate[0].usVopTimeIncrement) /
            m_VidRecdModes.SnrInputFrameRate[0].usVopTimeIncrement;

    status = MMPD_VIDENC_CheckCapability(EncWidth, EncHeight, ulFps);

    if (status != MMP_ERR_NONE)
        return status;

    if (m_VidRecdID == INVALID_ENC_ID)
    	status_vid = MMPD_MP4VENC_FW_OP_NONE;
    else
    	MMPD_VIDENC_GetStatus(m_VidRecdID, &status_vid);

    if ((status_vid == MMPD_MP4VENC_FW_OP_NONE) ||
        (status_vid == MMPD_MP4VENC_FW_OP_STOP)) {

        if (MMPD_VIDENC_GetFormat() == MMPD_MP4VENC_FORMAT_H264) {

            MMPF_AUDIO_WaitForSync();

            if (!MMPD_VIDENC_IsModuleInit(MMPD_VIDENC_MODULE_H264)) {
            	MMP_ERR	mmpstatus;

            	mmpstatus = MMPD_VIDENC_InitModule(MMPD_VIDENC_MODULE_H264);
                if (mmpstatus != MMP_ERR_NONE) {
                	printc("module init fail\r\n");
                	return mmpstatus;
               	}
            }
            /* Now stop record will not stop preview (all pipes actually)
             * User can call MMPS_3GPRECD_StopRecord() and then call
             * MMPS_3GPRECD_StartRecord() to re-start recording without
             * stop/start preview. So we have to reset start address of mem
             * for encoder here.
             */
            MMPD_VIDENC_SetMemStart(m_ulVideoPreviewEndAddr);

            if (MMPS_3GPRECD_SetH264MemoryMap(  &enc_id,
                                                EncWidth,
                                                EncHeight,
                                                m_ulVidRecSramAddr,
                                                m_ulVideoPreviewEndAddr))
            {
                printc("Alloc mem for video record failed\r\n");
                return MMP_3GPRECD_ERR_MEM_EXHAUSTED;
            }

            switch (m_VidRecdModes.VidCurBufMode[0]) {
            case MMPS_3GPRECD_CURBUF_FRAME:
                MMPD_VIDENC_SetCurBufMode(enc_id, MMPD_MP4VENC_CURBUF_FRAME);
                break;
            case MMPS_3GPRECD_CURBUF_RT:
                MMPD_VIDENC_SetCurBufMode(enc_id, MMPD_MP4VENC_CURBUF_RT);
                break;
            default:
                return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
            }

            if (MMPS_3GPRECD_EnableRecordPipe(MMP_TRUE) != MMP_ERR_NONE) {
        		printc("Enable Video Record: Fail\r\n");
                return MMP_3GPRECD_ERR_GENERAL_ERROR;
            }

        }
        else {
            return MMP_3GPRECD_ERR_GENERAL_ERROR;
        }
        
        // Encoder parameters
        if (((EncWidth == 1920) && (EncHeight == 1088))||((EncWidth == 640) && (EncHeight == 368))) {
            MMPS_3GPRECD_SetCropping(enc_id, 0, 8, 0, 0);
            if (m_VidRecdModes.VidCurBufMode[0] == MMPS_3GPRECD_CURBUF_RT)
                MMPS_3GPRECD_SetPadding(enc_id, MMPS_3GPRECD_PADDING_REPEAT, 8);
        }
        else {
            MMPS_3GPRECD_SetCropping(enc_id, 0, 0, 0, 0);
            MMPS_3GPRECD_SetPadding(enc_id, MMPS_3GPRECD_PADDING_NONE, 0);
        }

        if (MMPD_VIDENC_SetResolution(enc_id, EncWidth, EncHeight) != MMP_ERR_NONE)
                return MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS;
        
        if (MMPD_VIDENC_SetProfile(enc_id, m_VidRecdModes.VisualProfile[0]) != MMP_ERR_NONE) {
            return MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS;
        }
        MMPD_VIDENC_SetEncodeMode();

        MMPD_3GPMGR_SetGOP(m_VidRecdModes.usPFrameCount[0], m_VidRecdModes.usBFrameCount[0], MMP_3GPRECD_FILETYPE_VIDRECD);
        MMPD_VIDENC_SetGOP(enc_id, m_VidRecdModes.usPFrameCount[0], m_VidRecdModes.usBFrameCount[0]);

        ulTargetFrameSize = MMPS_3GPRECD_TargetFrmSize(0);
        MMPD_VIDENC_SetQuality(enc_id, ulTargetFrameSize, (MMP_ULONG)ullBitrate64);

        // Sensor input frame rate
        MMPD_VIDENC_SetSnrFrameRate(enc_id,
        		                    m_VidRecdModes.SnrInputFrameRate[0].usVopTimeIncrement,
        		                    m_VidRecdModes.SnrInputFrameRate[0].usVopTimeIncrResol);
        // Encode frame rate
        MMPD_VIDENC_SetEncFrameRate(enc_id,
        		                    m_VidRecdModes.VideoEncFrameRate[0].usVopTimeIncrement,
				                    m_VidRecdModes.VideoEncFrameRate[0].usVopTimeIncrResol);
        // Container frame rate
        MMPD_3GPMGR_SetFrameRate(MMP_3GPRECD_FILETYPE_VIDRECD,
        		                 m_VidRecdModes.ContainerFrameRate[0].usVopTimeIncrResol,
				                 m_VidRecdModes.ContainerFrameRate[0].usVopTimeIncrement);
    }

    if ((status_vid == MMPD_MP4VENC_FW_OP_NONE) ||
        (status_vid == MMPD_MP4VENC_FW_OP_STOP) ||
        (status_vid == MMPD_MP4VENC_FW_OP_PREENCODE)) {
		#if (DUALENC_SUPPORT == 0) // Change start capture to start all capture function for dual encode slow card issue.

        MMPD_3GPMGR_SetFileLimit(m_VidRecdModes.ulSizeLimit, m_VidRecdModes.ulReservedSpace, &ulAvaSize);

        if (MMPS_3GPRECD_GetExpectedRecordTime(ulAvaSize, m_VidRecdModes.ulBitrate[0], m_VidRecdModes.ulAudBitrate) <= 0) {
        	printc("MMPS_3GPRECD_GetExpectedRecordTime: Fail\r\n");
            return MMP_3GPRECD_ERR_MEM_EXHAUSTED;
        }
        else {
            MMPD_3GPMGR_SetTimeLimit(m_VidRecdModes.ulTimeLimitMs);
        }

        if (m_VidRecdConfigs.bSeamlessMode == MMP_TRUE) {
            // seamless callback must be registered if seamless mode is enabled.
            MMPD_3GPMGR_GetRegisteredCallback(MMPD_3GPMGR_EVENT_SEAMLESS, &pSeamlessCB);

            if (m_bSeamlessEnabled && (pSeamlessCB == NULL)) {
            	printc("Get seamless: Fail\r\n");
                return MMP_3GPRECD_ERR_GENERAL_ERROR;
            }
        }

        if (status_vid != MMPD_MP4VENC_FW_OP_PREENCODE) {
            MMPD_VIDENC_EnableClock(MMP_TRUE);
        }

        if (MMPD_3GPMGR_StartCapture(m_VidRecdID, MMP_3GPRECD_FILETYPE_VIDRECD) != MMP_ERR_NONE) {
        	MMPD_VIDENC_EnableClock(MMP_FALSE);
        	return MMP_3GPRECD_ERR_GENERAL_ERROR;
        }

        do {
            //MMPD_3GPMGR_GetStatus(&status, &status_tx);
            MMPD_VIDENC_GetMergerStatus(&status, &status_tx);
            MMPD_VIDENC_GetStatus(enc_id, &status_vid);
            if (status_vid == MMPD_MP4VENC_FW_OP_STOP)
            	return MMP_3GPRECD_ERR_STOPRECD_SLOWCARD;
        } while ((status_vid != MMPD_MP4VENC_FW_OP_START) && (status != MMP_FS_ERR_OPEN_FAIL));

        if (status == MMP_FS_ERR_OPEN_FAIL) {
        	printc("Get MGR file open Fail\r\n");
            return status;
        }

        if (status_vid == MMPD_MP4VENC_FW_OP_START) {
            return MMP_ERR_NONE;
        }
        else if (status == MMP_FS_ERR_OPEN_FAIL) {
            return MMP_3GPRECD_ERR_OPEN_FILE_FAILURE;
        }
        else {
            return MMP_ERR_NONE;
        }
        
    	#else
        // Just keep encode ID and encode total number here.
        if ((status_vid == MMPD_MP4VENC_FW_OP_NONE) ||
        	(status_vid == MMPD_MP4VENC_FW_OP_STOP)) {
	        gEncFileType[gbTotalEncNum] = MMP_3GPRECD_FILETYPE_VIDRECD;
	        if (gbTotalEncNum > 2) {
	        	printc("err:some enc not disable: %x\r\n", gbTotalEncNum);
	        }
	        glEncID[gbTotalEncNum] = m_VidRecdID;
	        gbTotalEncNum ++;
		}
        return MMP_ERR_NONE;
	    #endif    
    }
    else {
        return MMP_3GPRECD_ERR_GENERAL_ERROR;
    }
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetEncodeMode
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set encode mode when change to video H263 format.
 @note This API should be used after setting resolution and video format.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetEncodeMode(void)
{
    MMPD_VIDENC_SetEncodeMode();

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetFrameRate
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set frame rate for recorded video.
 @param[in] snr_fps Sensor input frame rate
 @param[in] enc_fps expected encode frame rate (normal/timelapse/slow motion)
 @param[in] container_fps Frame rate for playback
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetFrameRatePara(MMPS_3GPRECD_FRAMERATE *snr_fps,
                                  MMPS_3GPRECD_FRAMERATE *enc_fps,
                                  MMPS_3GPRECD_FRAMERATE *container_fps)
{
    if ((snr_fps->usVopTimeIncrement == 0) ||
        (snr_fps->usVopTimeIncrResol == 0) ||
        (enc_fps->usVopTimeIncrement == 0) ||
        (enc_fps->usVopTimeIncrResol == 0) ||
        (container_fps->usVopTimeIncrement == 0) ||
        (container_fps->usVopTimeIncrResol == 0)) {
        return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
    }
    
    m_VidRecdModes.SnrInputFrameRate[0].usVopTimeIncrement = snr_fps->usVopTimeIncrement;
    m_VidRecdModes.SnrInputFrameRate[0].usVopTimeIncrResol = snr_fps->usVopTimeIncrResol;
    
    m_VidRecdModes.VideoEncFrameRate[0].usVopTimeIncrement = enc_fps->usVopTimeIncrement;
    m_VidRecdModes.VideoEncFrameRate[0].usVopTimeIncrResol = enc_fps->usVopTimeIncrResol;
    
    m_VidRecdModes.ContainerFrameRate[0].usVopTimeIncrement = container_fps->usVopTimeIncrement;
    m_VidRecdModes.ContainerFrameRate[0].usVopTimeIncrResol = container_fps->usVopTimeIncrResol;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetEncResIdx
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set encoded resolution.
 @param[in] usResol Resolution for record video.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetEncResIdx(MMP_USHORT usResol)
{
    m_VidRecdModes.usVideoEncResIdx[0] = usResol;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_TargetFrmSize
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get record target frame size according to the frame rate & bitrate.
 @retval Record target frame size.
*/
static MMP_ULONG MMPS_3GPRECD_TargetFrmSize(MMP_ULONG ulEncId)
{
    MMP_ULONG64         ullBitrate64;
    MMP_ULONG64         ullTimeIncr64;
    MMP_ULONG64         ullTimeResol64;
    MMP_ULONG           ulTargetFrameSize;

    ullBitrate64 = m_VidRecdModes.ulBitrate[ulEncId];
    ullTimeIncr64 = m_VidRecdModes.ContainerFrameRate[ulEncId].usVopTimeIncrement;
    ullTimeResol64 = m_VidRecdModes.ContainerFrameRate[ulEncId].usVopTimeIncrResol;
    ulTargetFrameSize = (MMP_ULONG)((ullBitrate64 * ullTimeIncr64)/(ullTimeResol64 * 8));

    return ulTargetFrameSize;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetBitrate
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set the recording video bit-rate according to the frame rate info in
        container. i.e. set the video bit-rate for playback

 a. For normal speed recording case:
    The bit-rate settings are identical for recording & playback.

 b. For slow motion recording case:
    The bit-rate settings should be set based on the container fps.
    e.g. Sensor inputs 60fps, and encode frame rate is also 60fps, but
           the container frame rate is set as 30fps to achieve 2x slow motion.
           For normal recording, we may set bit-rate to 16Mb for 720p 60fps;
           for 2x slow motion recording, please set bit-rate as 8Mb to achieve
           the same video quality.

 c. For fast action/time-lapse recording case:
    The bit-rate settings should be set also based on the container fps.
    e.g. Sensor inputs 60fps, and encode frame rate is set to 30fps only,
           the container frame rate is set as 60fps to achieve 2x fast action.
           For normal recording, we may set bit-rate to 8Mb for 720p 30fps;
           for 2x fast action recording, please set bit-rate as 16Mb to achieve
           the same video quality.

 @param[in] ulBitrate Bit per second of the video bitstream in recorded file.
 @retval MMP_ERR_NONE Success.
 @note It must be set after choosing resolution and format.
*/
MMP_ERR MMPS_3GPRECD_SetBitrate(MMP_ULONG ulBitrate)
{
    MMPD_MP4VENC_FW_OP  status_vid;

    m_VidRecdModes.ulBitrate[0] = ulBitrate;
    //printc(FG_YELLOW("F_Bitrate=%d\r\n"), m_VidRecdModes.ulBitrate[0]); 

    if (m_VidRecdID == INVALID_ENC_ID)
    	status_vid = MMPD_MP4VENC_FW_OP_NONE;
    else
    	MMPD_VIDENC_GetStatus(m_VidRecdID, &status_vid);

    if ((status_vid == MMPD_MP4VENC_FW_OP_START) ||
        (status_vid == MMPD_MP4VENC_FW_OP_PREENCODE)) {
	    MMP_ULONG64         ullTimeIncr64 = m_VidRecdModes.VideoEncFrameRate[0].usVopTimeIncrement;
        MMP_ULONG64         ullTimeResol64 = m_VidRecdModes.VideoEncFrameRate[0].usVopTimeIncrResol;
    	MMP_ULONG           ulTargetFrameSize;

    	ulTargetFrameSize = (MMP_ULONG)((m_VidRecdModes.ulBitrate[0] * ullTimeIncr64)/(ullTimeResol64 * 8));
        MMPD_VIDENC_SetBitrate(m_VidRecdID, ulTargetFrameSize, m_VidRecdModes.ulBitrate[0]);
    }
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetContainerType
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_SetContainerType(MMPS_3GPRECD_CONTAINER type)
{
    switch (type) {
    case MMPS_3GPRECD_CONTAINER_3GP:
        m_VidRecdContainer = MMPS_3GPRECD_CONTAINER_3GP;
        MMPD_3GPMGR_SetContainerType(MMPD_3GPMGR_CONTAINER_3GP);
        break;
    case MMPS_3GPRECD_CONTAINER_AVI:
        m_VidRecdContainer = MMPS_3GPRECD_CONTAINER_AVI;
        MMPD_3GPMGR_SetContainerType(MMPD_3GPMGR_CONTAINER_AVI);
        break;
    case MMPS_3GPRECD_CONTAINER_NONE:
        m_VidRecdContainer = MMPS_3GPRECD_CONTAINER_NONE;
        MMPD_3GPMGR_SetContainerType(MMPD_3GPMGR_CONTAINER_NONE);
        break;    
    default:
        m_VidRecdContainer = MMPS_3GPRECD_CONTAINER_UNKNOWN;
        return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetVideoFormat
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set encoded video format.
 @param[in] Format Video encode format for record video.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetVideoFormat(MMPS_3GPRECD_VIDEO_FORMAT Format)
{
    switch (Format) {
    case MMPS_3GPRECD_VIDEO_FORMAT_H264:
        MMPD_VIDENC_SetFormat(MMPD_MP4VENC_FORMAT_H264);
        return MMP_ERR_NONE;
    case MMPS_3GPRECD_VIDEO_FORMAT_OTHERS:
    default:
        return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
    }
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetCurBufMode
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set encoded video current buffer mode.
 @param[in] CurBufMode Video encode current buffer mode for record video.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetCurBufMode(MMPS_3GPRECD_CURBUF_MODE CurBufMode)
{
	MMP_ULONG          ulVidNumOpening = 0;
    MMPD_MP4VENC_FW_OP status_fw;

    MMPD_VIDENC_GetNumOpen(&ulVidNumOpening);

    if (m_VidRecdID == INVALID_ENC_ID)
    	status_fw = MMPD_MP4VENC_FW_OP_NONE;
    else
    	MMPD_VIDENC_GetStatus(m_VidRecdID, &status_fw);

    if ((ulVidNumOpening == MAX_NUM_ENC_SET) ||
		(status_fw == MMPD_MP4VENC_FW_OP_START) ||
        (status_fw == MMPD_MP4VENC_FW_OP_PAUSE) ||
        (status_fw == MMPD_MP4VENC_FW_OP_RESUME)) {
        RTNA_DBG_Str0("Can't change H264EncMode when recording\r\n");
        return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
    }

    m_VidRecdModes.VidCurBufMode[0] = CurBufMode;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetAudioFormat
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set audio mode to be AMR or AAC.
 @param[in] Format Assign the audio type is AMR or AAC with video encoder.
 @param[in] Option Assign audio sampling rate and bit rate.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_INVALID_PARAMETERS Invalid parameter.
 @note

 The parameter @a Option support:
 - MMPS_3GPRECD_AUDIO_AAC_22d05K_64K : 22.05KHz with 64kbps
 - MMPS_3GPRECD_AUDIO_AAC_22d05K_128K : 22.05KHz with 128kbps
 - MMPS_3GPRECD_AUDIO_AAC_32K_64K : 32KHz with 64kbps
 - MMPS_3GPRECD_AUDIO_AAC_32K_128K : 32KHz with 128kbps
 - MMPS_3GPRECD_AUDIO_AMR_4d75K : 4.75 KHz
 - MMPS_3GPRECD_AUDIO_AMR_5d15K : 5.15 KHz
 - MMPS_3GPRECD_AUDIO_AMR_12d2K : 12.2 KHz
*/
MMP_ERR MMPS_3GPRECD_SetAudioFormat(MMPS_3GPRECD_AUDIO_FORMAT Format, MMPS_3GPRECD_AUDIO_OPTION Option)
{
    MMP_ULONG audioparam = 0;
    MMP_USHORT audiomode = MMPS_AUDIO_AAC_RECORD_32K_128K;
    MMP_ERR ret = MMP_ERR_NONE;

    if (Format == MMPS_3GPRECD_AUDIO_FORMAT_AAC) {
    
        MMPD_AUDIO_SetEncodeFormat(MMPD_AUDIO_VAAC_ENCODE);

        switch (Option) {
        case MMPS_3GPRECD_AUDIO_AAC_16K_32K:
            audiomode = MMPS_AUDIO_AAC_RECORD_16K_32K;
            audioparam = 16000;
            m_VidRecdModes.ulAudBitrate = 32000;
            MMPD_AUDIO_SetPLL(audioparam);
            break;
        case MMPS_3GPRECD_AUDIO_AAC_16K_64K:
            audiomode = MMPS_AUDIO_AAC_RECORD_16K_64K;
            audioparam = 16000;
            m_VidRecdModes.ulAudBitrate = 64000;
            MMPD_AUDIO_SetPLL(audioparam);
            break;
        case MMPS_3GPRECD_AUDIO_AAC_22d05K_64K:
            audiomode 	= MMPS_AUDIO_AAC_RECORD_22K_64K;
            audioparam 	= 22050;
            m_VidRecdModes.ulAudBitrate = 64000;
            MMPD_AUDIO_SetPLL(audioparam);
            break;
        case MMPS_3GPRECD_AUDIO_AAC_22d05K_128K:
            audiomode 	= MMPS_AUDIO_AAC_RECORD_22K_128K;
            audioparam	= 22050;
            m_VidRecdModes.ulAudBitrate = 128000;
            MMPD_AUDIO_SetPLL(audioparam);
            break;
        case MMPS_3GPRECD_AUDIO_AAC_32K_64K:
            audiomode 	= MMPS_AUDIO_AAC_RECORD_32K_64K;
            audioparam 	= 32000;
            m_VidRecdModes.ulAudBitrate = 64000;
            MMPD_AUDIO_SetPLL(audioparam);
            break;
        case MMPS_3GPRECD_AUDIO_AAC_32K_128K:
            audiomode 	= MMPS_AUDIO_AAC_RECORD_32K_128K;
            audioparam 	= 32000;
            m_VidRecdModes.ulAudBitrate = 128000;
            MMPD_AUDIO_SetPLL(audioparam);
            break;
        case MMPS_3GPRECD_AUDIO_AAC_48K_128K:
            audiomode 	= MMPS_AUDIO_AAC_RECORD_48K_128K;
            audioparam 	= 48000;
            m_VidRecdModes.ulAudBitrate = 128000;
            MMPD_AUDIO_SetPLL(audioparam);
            break;
        default:
            ret = MMP_3GPMGR_ERR_PARAMETER;
            break;
        }
        MMPD_AUDIO_SetEncodeFileSize(0xFFFFFFFF);
        MMPD_AUDIO_SetEncodeMode(audiomode);
        MMPD_3GPMGR_SetAudioParam(audioparam, MMPD_3GPMGR_AUDIO_FORMAT_AAC);
    }
    else if (Format == MMPS_3GPRECD_AUDIO_FORMAT_AMR) {
    
        MMPD_AUDIO_SetEncodeFormat(MMPD_AUDIO_VAMR_ENCODE);
        MMPD_AUDIO_SetPLL(8000);

        switch (Option) {
        case MMPS_3GPRECD_AUDIO_AMR_4d75K:
            audiomode 	= MMPS_AUDIO_AMR_MR475_ENCODE_MODE;
            audioparam 	= 13;
            m_VidRecdModes.ulAudBitrate = 4750;
            break;
        case MMPS_3GPRECD_AUDIO_AMR_5d15K:
            audiomode 	= MMPS_AUDIO_AMR_MR515_ENCODE_MODE;
            audioparam 	= 14;
            m_VidRecdModes.ulAudBitrate = 5150;
            break;
        case MMPS_3GPRECD_AUDIO_AMR_12d2K:
            audiomode 	= MMPS_AUDIO_AMR_MR122_ENCODE_MODE;
            audioparam 	= 32;
            m_VidRecdModes.ulAudBitrate = 1220;
            break;
        default:
            ret = MMP_3GPMGR_ERR_PARAMETER;
            break;
        }
        MMPD_AUDIO_SetEncodeMode(audiomode);
        ret |= MMPD_3GPMGR_SetAudioParam(audioparam, MMPD_3GPMGR_AUDIO_FORMAT_AMR);
    }
    else if (Format == MMPS_3GPRECD_AUDIO_FORMAT_ADPCM) {
    
        MMPD_AUDIO_SetEncodeFormat(MMPD_AUDIO_VADPCM_ENCODE);

        switch (Option) {
        case MMPS_3GPRECD_AUDIO_ADPCM_32K_22K:
            audiomode 	= MMPS_3GPRECD_AUDIO_ADPCM_32K_22K;
            audioparam 	= 32000;
            m_VidRecdModes.ulAudBitrate = 256000;
            MMPD_AUDIO_SetPLL(audioparam);
            break;
        case MMPS_3GPRECD_AUDIO_ADPCM_44d1K_22K:
            audiomode 	= MMPS_3GPRECD_AUDIO_ADPCM_44d1K_22K;
            audioparam 	= 44100;
            m_VidRecdModes.ulAudBitrate = 352800;
            MMPD_AUDIO_SetPLL(audioparam);
            break;
        case MMPS_3GPRECD_AUDIO_ADPCM_48K_22K:
            audiomode 	= MMPS_3GPRECD_AUDIO_ADPCM_48K_22K;
            audioparam 	= 48000;
            m_VidRecdModes.ulAudBitrate = 384000;
            MMPD_AUDIO_SetPLL(audioparam);
            break;
        default:
            ret = MMP_3GPMGR_ERR_PARAMETER;
            break;
        }
        MMPD_AUDIO_SetEncodeFileSize(0xFFFFFFFF);
        MMPD_AUDIO_SetEncodeMode(audiomode);
        MMPD_3GPMGR_SetAudioParam(audioparam, MMPD_3GPMGR_AUDIO_FORMAT_ADPCM);
    }
    else if (Format == MMPS_3GPRECD_AUDIO_FORMAT_MP3) {
    
        MMPD_AUDIO_SetEncodeFormat(MMPD_AUDIO_VMP3_ENCODE);

        switch (Option) {
        case MMPS_3GPRECD_AUDIO_MP3_32K_128K:
            audiomode 	= MMPS_AUDIO_MP3_RECORD_32K_128K;
            audioparam 	= 32000;
            m_VidRecdModes.ulAudBitrate = 128000;
            MMPD_AUDIO_SetPLL(audioparam);
            break;
        case MMPS_3GPRECD_AUDIO_MP3_44d1K_128K:
            audiomode 	= MMPS_AUDIO_MP3_RECORD_44d1K_128K;
            audioparam 	= 44100;
            m_VidRecdModes.ulAudBitrate = 128000;
            MMPD_AUDIO_SetPLL(audioparam);
            break;
        default:
            ret = MMP_3GPMGR_ERR_PARAMETER;
            break;
        }
        MMPD_AUDIO_SetEncodeFileSize(0xFFFFFFFF);
        MMPD_AUDIO_SetEncodeMode(audiomode);
        MMPD_3GPMGR_SetAudioParam(audioparam, MMPD_3GPMGR_AUDIO_FORMAT_MP3);
    }
    else if (Format == MMPS_3GPRECD_AUDIO_FORMAT_PCM) {
        MMPD_AUDIO_SetEncodeFormat(MMPD_AUDIO_VPCM_ENCODE);

        switch (Option) {
        case MMPS_3GPRECD_AUDIO_PCM_16K:
            audiomode 	= MMPS_AUDIO_WAV_RECORD_16K;
            m_VidRecdModes.ulAudBitrate = 16000 * 2 * 2; // 2ch 16-bit sample
            audioparam 	= 16000;
            MMPD_AUDIO_SetPLL(audioparam);
            break;
        case MMPS_3GPRECD_AUDIO_PCM_32K:
            audiomode 	= MMPS_AUDIO_WAV_RECORD_32K;
            audioparam 	= 32000;
            m_VidRecdModes.ulAudBitrate = 32000 * 2 * 2; // 2ch 16-bit sample
            MMPD_AUDIO_SetPLL(audioparam);
            break;
        default:
            ret = MMP_3GPMGR_ERR_PARAMETER;
            break;
        }
        MMPD_AUDIO_SetEncodeFileSize(0xFFFFFFFF);
        MMPD_AUDIO_SetEncodeMode(audiomode);
        MMPD_3GPMGR_SetAudioParam(audioparam, MMPD_3GPMGR_AUDIO_FORMAT_PCM);
    }
    else {
        ret = MMP_3GPMGR_ERR_PARAMETER;
    }

    if (ret == MMP_ERR_NONE) {
        return MMP_ERR_NONE;
    }
    else {
        return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
    }
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetAudioRecMode
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Enable audio encode.
 @param[in] bEnable Enable video record with audio.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetAudioRecMode(MMPS_3GPRECD_AUDIO_DATATYPE mode)
{
	if (mode != MMPS_3GPRECD_NO_AUDIO_DATA) {
        MMPD_AUDIO_SetInPath(MMPS_Audio_GetConfig()->AudioInPath);
        MMPD_3GPMGR_EnableAVSyncEncode(MMP_TRUE);
	}
    else {
        m_VidRecdModes.ulAudBitrate = 0;
        MMPD_3GPMGR_EnableAVSyncEncode(MMP_FALSE);
    }

    if (mode == MMPS_3GPRECD_SILENCE_AUDIO_DATA)
        MMPD_AUDIO_SetRecordSilence(MMP_TRUE);
    else
        MMPD_AUDIO_SetRecordSilence(MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetPFrameCount
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set P frame count of one cycle.
 @param[in] usFrameCount Count of P frame.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetPFrameCount(MMP_USHORT usFrameCount)
{
    m_VidRecdModes.usPFrameCount[0] = usFrameCount;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetBFrameCount
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set B frame count per P frame.
 @param[in] usFrameCount Count of B frame.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetBFrameCount(MMP_USHORT usFrameCount)
{
    MMPD_MP4VENC_FW_OP status_vid;

    if (usFrameCount) {
        // For single current frame mode, B-frame enc is not supported
        return MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS;
    }

    if (m_VidRecdID == INVALID_ENC_ID)
    	status_vid = MMPD_MP4VENC_FW_OP_NONE;
    else
    	MMPD_VIDENC_GetStatus(m_VidRecdID, &status_vid);
    
    if (MMPD_VIDENC_GetFormat() != MMPD_MP4VENC_FORMAT_H264) {
	    return MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS;
    }

    if ((status_vid == MMPD_MP4VENC_FW_OP_NONE) ||
        (status_vid == MMPD_MP4VENC_FW_OP_STOP)) {

        m_VidRecdModes.usBFrameCount[0] = usFrameCount;
    }
    else {
        return MMP_3GPRECD_ERR_GENERAL_ERROR;
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetBFrameCount
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get B frame count per P frame.
 @retval Count of B frame.
*/
MMP_USHORT MMPS_3GPRECD_GetBFrameCount(void)
{
    return m_VidRecdModes.usBFrameCount[0];
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetMaxPreEncDuration
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get the max pre-encode duration in MS.

 It returns the max pre-encode duration allowed
*/
MMP_ULONG MMPS_3GPRECD_GetMaxPreEncDuration(void)
{	
	MMP_ULONG64	ullBitrate64 		= m_VidRecdModes.ulBitrate[0];
	MMP_ULONG64 ullTimeIncr64 		= m_VidRecdModes.VideoEncFrameRate[0].usVopTimeIncrement;
    MMP_ULONG64	ullTimeResol64 		= m_VidRecdModes.VideoEncFrameRate[0].usVopTimeIncrResol;
    
	MMP_ULONG 	ulTargetFrameSize 	= (MMP_ULONG)((ullBitrate64 * ullTimeIncr64)/(ullTimeResol64 * 8));
    MMP_ULONG 	ulExpectBufSize		= 0;
    MMP_ULONG 	ulMaxPreEncMs 		= 0;

	ulExpectBufSize = m_VidRecdConfigs.ulVideoCompBufSize - (ulTargetFrameSize * 3);
    ulMaxPreEncMs = ((ulExpectBufSize) * 800) / (m_VidRecdModes.ulBitrate[0] >> 3);
    
    return ulMaxPreEncMs;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetProfile
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set profile for video encode.
 @param[in] profile Visual profile for the specified encoder.
 @retval none.
*/
MMP_ERR MMPS_3GPRECD_SetProfile(MMPS_3GPRECD_PROFILE profile)
{
    m_VidRecdModes.VisualProfile[0] = profile;
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetStoragePath
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Save encoded 3GP file to card mode or memory mode.
 @param[in] SrcMode Card or memory mode.
 @retval MMP_ERR_NONE Success.
 @note

 The parameter @a SrcMode can be:
 - MMPS_3GPRECD_SRCMODE_MEM : save file to memory.
 - MMPS_3GPRECD_SRCMODE_CARD : save file to storage.
*/
MMP_ERR MMPS_3GPRECD_SetStoragePath(MMPS_3GPRECD_SRCMODE SrcMode)
{
    m_VidRecdModes.VideoSrcMode[0] = SrcMode;
    
    MMPD_3GPMGR_SetStoragePath((MMP_UBYTE)m_VidRecdModes.VideoSrcMode[0]);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetFileName
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Send encoded 3GP file name to firmware for card mode and creat file time.
 @param[in] bFilename File name.
 @param[in] usLength Length of file name in unit of byte.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetFileName(MMP_BYTE bFilename[], MMP_USHORT usLength, MMP_3GPRECD_FILETYPE filetype)
{
    MMPS_FS_SetCreationTime();
    MMPD_3GPMGR_SetFileName(bFilename, usLength, filetype);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetUserDataAtom
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Send user data atom to firmware for card mode.
 @param[in] AtomName atom name, maximum 4 bytes.
 @param[in] UserDataBuf user data atom buffer.
 @param[in] UserDataLength Length of user data atom.
 @param[in] filetype file type.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetUserDataAtom(MMP_BYTE AtomName[],MMP_BYTE UserDataBuf[], MMP_USHORT UserDataLength, MMP_3GPRECD_FILETYPE filetype)
{
    return MMPD_3GPMGR_SetUserDataAtom(AtomName, UserDataBuf, UserDataLength, filetype);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetReservedStorageSpace
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set reserved storage space which recorder or other system can not use it
        during recording. Ex. if user wants to reserve 10MB while 100MB free space,
        then total 90MB remained for recording & capture in recording to share.
 @pre MMPS_3GPRECD_SetStoragePath(MMPS_3GPRECD_SRCMODE_CARD) was called, for card mode only
 @param[in] ulReservedSize Reserved stroage space.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetReservedStorageSpace(MMP_ULONG ulReservedSize)
{
    if (m_VidRecdModes.VideoSrcMode[0] == MMPS_3GPRECD_SRCMODE_CARD){
        m_VidRecdModes.ulReservedSpace = ulReservedSize;
	}
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetFileSizeLimit
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set the maximum 3GP file size for video encoding.
 @param[in] ulFileLimit Maximum file size.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetFileSizeLimit(MMP_ULONG ulSizeLimit)
{
    m_VidRecdModes.ulSizeLimit = ulSizeLimit;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetFileTimeLimit
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set the maximum 3GP file time for video encoding.
 @param[in] ulTimeLimitMs Maximum file time in unit of ms.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_NOT_ENOUGH_SPACE Space not enough.

 @warn If ulTimeLimitMs is set to 0, then it doesn't take any effect.
*/
MMP_ERR MMPS_3GPRECD_SetFileTimeLimit(MMP_ULONG ulTimeLimitMs)
{
    if (ulTimeLimitMs) {
        m_VidRecdModes.ulTimeLimitMs = ulTimeLimitMs;
        return MMPD_3GPMGR_SetTimeLimit(ulTimeLimitMs);
    }

    return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_Set3GPCreateModifyTimeInfo
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Set creation time and modification time of 3GP file.
 @param[in] addr address of temp buffer.
 @param[in] size size of temp buffer.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_Set3GPCreateModifyTimeInfo(MMP_3GPRECD_FILETYPE filetype, AUTL_DATETIME datetimenew)
{
	MMP_ULONG CreateTime, ModifyTime;
	MMP_ULONG ulseconds = 0; 
	AUTL_DATETIME datetime = {2015, 3, 3, 2, 0, 0, 0, 0, 0};
	
	datetime.usYear = 1904;
	datetime.usMonth = 1;
	datetime.usDay = 1;
	datetime.usHour = 0;
	datetime.usMinute = 0;
	datetime.usSecond = 0;
	
	ulseconds  = AUTL_Calendar_DateToSeconds(&datetimenew, &datetime);
	CreateTime = ulseconds;
	ModifyTime = ulseconds; 
	
	RTNA_DBG_Str(0, "create & modify time:");
	RTNA_DBG_Long(0, ulseconds);
	RTNA_DBG_Str(0, "\r\n");
	
	MMPD_3GPMGR_Set3GPCreateModifyTimeInfo(filetype, CreateTime, ModifyTime);
	return MMP_ERR_NONE;  
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_ChangeCurFileTimeLimit
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Change the maximum 3GP file time for current video encoding .
 @param[in] ulTimeLimitMs Maximum file time in unit of ms.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_NOT_ENOUGH_SPACE Space not enough.
*/
MMP_ERR MMPS_3GPRECD_ChangeCurFileTimeLimit(MMP_ULONG ulTimeLimitMs)
{
	if (ulTimeLimitMs) {
		return MMPD_3GPMGR_SetTimeDynamicLimit(ulTimeLimitMs);
	}
	
	return MMP_3GPRECD_ERR_INVALID_PARAMETERS;	
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetExpectedRecordTime
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get the expected video encoding time in unit of second according to
        the specified storage space.
 @param[in] ulSpace the available free space
 @param[in] ulVidBitRate Video bitrate in bit per second.
 @param[in] ulAudBitRate Audio bitrate in bit per second (set to 0 if audio disabled).
 @retval the expected recording time, negative value indicates error.
*/
MMP_LONG MMPS_3GPRECD_GetExpectedRecordTime(MMP_ULONG64 ullSpace, MMP_ULONG ulVidBitRate, MMP_ULONG ulAudBitRate)
{
    MMP_LONG    lRecordSec;
    MMP_ULONG   ulContainerOverhead;

    if ((ulVidBitRate + ulAudBitRate) < 8)
        return 0;

    switch (m_VidRecdContainer) {
    case MMPS_3GPRECD_CONTAINER_3GP:
    case MMPS_3GPRECD_CONTAINER_NONE:
        ulContainerOverhead = 0; // TODO: set to 3GP header size
        break;
    case MMPS_3GPRECD_CONTAINER_AVI:
        switch(MMPD_3GPMGR_GetAudioFormat()) {
        case MMPD_3GPMGR_AUDIO_FORMAT_AAC:
            ulContainerOverhead = AVI_AACLC_HEADERS_SIZE;
            break;
        case MMPD_3GPMGR_AUDIO_FORMAT_ADPCM:
            ulContainerOverhead = AVI_WAVE_HEADERS_SIZE;
            break;
        case MMPD_3GPMGR_AUDIO_FORMAT_MP3:
            ulContainerOverhead = AVI_MP3_HEADERS_SIZE;
            break;
        default:
            ulContainerOverhead = AVI_NO_AUD_HEADERS_SIZE;
            break;
        }
        break;
    default:
        return -1;
    }

    if (ullSpace <= ulContainerOverhead)
        return 0;
    else
        ullSpace -= ulContainerOverhead;

    lRecordSec = (MMP_LONG)(ullSpace / ((ulVidBitRate + ulAudBitRate) >> 3));

    if (lRecordSec < MIN_TIME_TO_RECORD)
        return 0;

    return (lRecordSec - MIN_TIME_TO_RECORD);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetRecordTime
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get how many ms passed after starting recording.
 @param[out] ulTime Recording time in unit of ms.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_GetRecordTime(MMP_ULONG *ulTime)
{
    return MMPD_3GPMGR_GetRecordingTime(ulTime, MMP_3GPRECD_FILETYPE_VIDRECD);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetRecordDuration
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get current video duration.
 @param[out] ulTime Recording duration in unit of ms.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_GetRecordDuration(MMP_ULONG *ulTime)
{
    return MMPD_3GPMGR_GetRecordingDuration(ulTime, MMP_3GPRECD_FILETYPE_VIDRECD);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_Get3gpRecordingSize
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get current size of encoded 3GP file.

 This size only counts file header and current AV bitstream.
 @param[out] ulSize Current file size.
 @return Error status.
*/
MMP_ERR MMPS_3GPRECD_Get3gpRecordingSize(MMP_ULONG *ulSize)
{
    return MMPD_3GPMGR_Get3gpFileCurSize(ulSize);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_Get3gpFileSize
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get final size of encoded 3GP file.
 @param[out] ulfilesize Final 3GP file size.
 @return Error status.
*/
MMP_ERR MMPS_3GPRECD_Get3gpFileSize(MMP_ULONG *ulfilesize)
{
    return MMPD_3GPMGR_Get3gpFileSize(ulfilesize);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_CalTailBufSize
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Calculate the container tail buffer size according to the specified time.
 @param[in] time The specified time in unit of second.
 @return The size of tail buffer needed.
*/
static MMP_ULONG MMPS_3GPRECD_CalTailBufSize(MMP_ULONG time)
{
    MMP_ULONG ulTailVidSize = 0;
    MMP_ULONG ulTailAudSize = 0;
    MMP_ULONG ulSlowMoRatio = 1;

    /* Calculate tail info size for video frames */
    ulTailVidSize = (MMP_ULONG)(((MMP_ULONG64)time *
                    m_VidRecdModes.VideoEncFrameRate[0].usVopTimeIncrResol) /
                    m_VidRecdModes.VideoEncFrameRate[0].usVopTimeIncrement);
    ulTailVidSize = ulTailVidSize << 2; // 4-byte for size info

    /* Calculate tail info size for audio frames */
    if (MMPD_3GPMGR_GetAVSyncEncode()) {
        // record with audio
        if (m_VidRecdModes.bSlowMotionEn) {
            ulSlowMoRatio = (m_VidRecdModes.VideoEncFrameRate[0].usVopTimeIncrResol *
                m_VidRecdModes.ContainerFrameRate[0].usVopTimeIncrement) /
                (m_VidRecdModes.ContainerFrameRate[0].usVopTimeIncrResol *
                m_VidRecdModes.VideoEncFrameRate[0].usVopTimeIncrement);
            if (ulSlowMoRatio != 0)
                time = time * ulSlowMoRatio;
        }
        MMPD_AUDIO_GetRecordFrameCntInSeconds(time, &ulTailAudSize);
        ulTailAudSize = ulTailAudSize << 2; // 4-byte for size info
    }

    return (ulTailVidSize + ulTailAudSize);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetContainerTailBufSize
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get the container tail buffer size.
 @return The size of tail buffer needed.

 @pre Container type is already set by MMPS_3GPRECD_SetContainerType();
      File size is already set by MMPS_3GPRECD_SetFileSizeLimit();
      File time is already set by MMPS_3GPRECD_SetFileTimeLimit();
      Video frame rate is already set by MMPS_3GPRECD_SetFrameRatePara();
      Audio mode is alreay selected by MMPS_3GPRECD_SetAudioRecMode();
      Audio format is already set by MMPS_3GPRECD_SetAudioFormat().
*/
MMP_ULONG MMPS_3GPRECD_GetContainerTailBufSize(void)
{
    MMP_LONG  lSpace2FTime;
    MMP_ULONG ulSizeByFTime = 0;
    MMP_ULONG ulSizeByFSize = 0;
    MMP_ULONG ulMarginTime;
    MMP_ULONG ulAudBitrate = 0;

    switch (m_VidRecdContainer) {
    case MMPS_3GPRECD_CONTAINER_3GP:
        /* Currently only 3GP needs tail buffer */
        break;
    case MMPS_3GPRECD_CONTAINER_NONE:
    case MMPS_3GPRECD_CONTAINER_AVI:
    default:
        return 0;
    }

    /* If file time limit is set, calculate tail buffer size by time limit */
    if (m_VidRecdModes.ulTimeLimitMs != 0xFFFFFFFF) {
        ulMarginTime = (m_VidRecdModes.ulTimeLimitMs * 6) / 5000; // 20% margin
        ulSizeByFTime = MMPS_3GPRECD_CalTailBufSize(ulMarginTime);
    }

    /* Calculate tail buffer size by size limit*/
    if (MMPD_3GPMGR_GetAVSyncEncode())
        ulAudBitrate = m_VidRecdModes.ulAudBitrate;

    lSpace2FTime = MMPS_3GPRECD_GetExpectedRecordTime(
                                            m_VidRecdModes.ulSizeLimit,
                                            m_VidRecdModes.ulBitrate[0],
                                            ulAudBitrate);
    ulMarginTime = (lSpace2FTime * 27) / 20; // 35% margin
    ulSizeByFSize = MMPS_3GPRECD_CalTailBufSize(ulMarginTime);

    /* Select the smaller one */
    if (ulSizeByFTime)
        return (ulSizeByFTime < ulSizeByFSize) ? ulSizeByFTime : ulSizeByFSize;
    else
        return ulSizeByFSize;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_RecordStatus
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get status of firmware video recorder.
 @param[out] retstatus Firmware operation status.
 @return Error status.
 @note

 The parameter @a retstatus can be:
 - MMPS_FW_OPERATION_NONE Idle.
 - MMPS_FW_OPERATION_START Under recording.
 - MMPS_FW_OPERATION_PAUSE Pause recording.
 - MMPS_FW_OPERATION_RESUME Restart recording.
 - MMPS_FW_OPERATION_STOP Stop recording.
*/
MMP_ERR MMPS_3GPRECD_RecordStatus(MMPS_FW_OPERATION *retstatus)
{
    MMP_ERR _ret;
    MMPD_MP4VENC_FW_OP _status;

    if (m_VidRecdID == INVALID_ENC_ID) {
    	_status = MMPD_MP4VENC_FW_OP_NONE;
    	_ret = MMP_ERR_NONE;
    }
    else {
    	_ret = MMPD_VIDENC_GetStatus(m_VidRecdID, &_status);
    }
    
    switch (_status) {
    case MMPD_MP4VENC_FW_OP_NONE:
        *retstatus = MMPS_FW_OPERATION_NONE;
        break;
    case MMPD_MP4VENC_FW_OP_START:
        *retstatus = MMPS_FW_OPERATION_START;
        break;
    case MMPD_MP4VENC_FW_OP_PAUSE:
        *retstatus = MMPS_FW_OPERATION_PAUSE;
        break;
    case MMPD_MP4VENC_FW_OP_RESUME:
        *retstatus = MMPS_FW_OPERATION_RESUME;
        break;
    case MMPD_MP4VENC_FW_OP_STOP:
        *retstatus = MMPS_FW_OPERATION_STOP;
        break;
    case MMPD_MP4VENC_FW_OP_PREENCODE:
        *retstatus = MMPS_FW_OPERATION_PREENCODE;
        break;
    default:
        *retstatus = MMPS_FW_OPERATION_NONE;
        break;
    }

    return _ret;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetMergerStatus
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get status of firmware 3GP merger.
 @param[out] retstatus Firmware merger engine status.
 @return Error status.
 @note

 The parameter @a retstatus can be:
 - MMP_ERR_NONE
 - MMP_FS_ERR_OPEN_FAIL
 - MMP_FS_ERR_TARGET_NOT_FOUND
 - MMP_3GPRECD_ERR_HOST_CANCEL_SAVE
 - MMP_3GPRECD_ERR_MEDIA_FILE_FULL
*/
MMP_ERR MMPS_3GPRECD_GetMergerStatus(MMP_ERR *retstatus, MMP_ULONG *txstatus)
{
    return  MMPD_3GPMGR_GetStatus(retstatus, txstatus);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetH264MemoryMap
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set memory layout by different resolution and memory type for H264.

 Depends on encoded resolution and memory type to map buffers. It supports two types
 of memory, INTER(823) and STACK(821).
 @param[in] usEncW Encode width.
 @param[in] usEncH Encode height.
 @param[in] ulFBufAddr Available start address of frame buffer.
 @param[in] ulStackAddr Available start address of dram buffer.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetH264MemoryMap(MMP_ULONG *ulEncId, MMP_USHORT usEncW, MMP_USHORT usEncH, MMP_ULONG ulFBufAddr, MMP_ULONG ulStackAddr)
{
    const MMP_ULONG                 VideoRegStoreSize     = 60;
    const MMP_ULONG                 AVSyncBufSize         = 32;
    #if (EMERGENTRECD_SUPPORT == 1)
    const MMP_ULONG                	VideoSizeTableSize    = 1024 * 4;// video record and emergent record
    const MMP_ULONG                 VideoTimeTableSize    = 1024 * 2;// video record and emergent record
    #else
    const MMP_ULONG                	VideoSizeTableSize    = 1024;
    const MMP_ULONG                 VideoTimeTableSize    = 1024;
    #endif
    const MMP_ULONG                 AudioCompBufSize      = m_VidRecdConfigs.ulAudioCompBufSize;
    const MMP_ULONG                 SPSSize               = 48;
    const MMP_ULONG                 PPSSize               = 16;
    const MMP_ULONG                 VideoCompBufSize      = m_VidRecdConfigs.ulVideoCompBufSize;
    #if (EMERGENTRECD_SUPPORT == 1)
    const MMP_ULONG                	AVRepackBufSize       = 512 * 1024 * 2;
    #else
    const MMP_ULONG                	AVRepackBufSize       = 512 * 1024;
    #endif
    const MMP_ULONG                 SliceLengBufSize      = ((4 * ((usEncH>>4) + 2) + 63) >> 6) << 6;//align 64
	#if (DUALENC_SUPPORT == 0)	
	const MMP_ULONG                 MVBufSize             = ((usEncW>>4) * (usEncH>>4)) * 16 * 4; //MBs * (Blocks per MB)* (4 Bytes)	#endif	MMP_USHORT						i;
	#endif    
	MMP_USHORT						i;
	MMP_ULONG						curaddr;
	#if (USE_H264_CUR_BUF_AS_CAPT_BUF == 0)
    MMP_ULONG						ulTmpBufSize;
    #endif
	MMPD_MP4VENC_VIDEOBUF			videohwbuf;
	MMPD_H264ENC_HEADER_BUF         headerbuf;
	MMPD_3GPMGR_AVCOMPRESSEDBUFFER	mergercompbuf;
	MMPD_3GPMGR_REPACKBUF			repackmiscbuf;
	MMP_ULONG                       bufferEndAddr;
    MMP_ULONG                       MergerBufSize;
    MMP_ULONG                       ulEncFrameSize;
    #if (UVC_VIDRECD_SUPPORT == 1)
   	MMPD_H264ENC_HEADER_BUF         UVCheaderbuf;
    MMPD_3GPMGR_AVCOMPRESSEDBUFFER	UVCmergercompbuf;
    const MMP_ULONG                 UVCVidCompBufSize      = m_VidRecdConfigs.ulUVCVidCompBufSize;
	#if (VIDRECD_MULTI_TRACK == 0)   
	MMPD_3GPMGR_REPACKBUF			UVCrepackmiscbuf;
    const MMP_ULONG                	UVCRepackBufSize       = 512 * 1024;
    const MMP_ULONG                	UVCVideoSizeTableSize  = 1024;
    const MMP_ULONG                 UVCVideoTimeTableSize  = 1024;
    #if (UVC_EMERGRECD_SUPPORT == 1)
    MMPD_3GPMGR_REPACKBUF			UVCE_repackmiscbuf;
    const MMP_ULONG                	UVCE_RepackBufSize       = 512 * 1024;
    const MMP_ULONG                	UVCE_VideoSizeTableSize  = 1024;
    const MMP_ULONG                 UVCE_VideoTimeTableSize  = 1024;
    #endif // UVC_EMERGRECD_SUPPORT == 1
    #endif // VIDRECD_MULTI_TRACK == 0
    #endif // UVC_VIDRECD_SUPPORT == 1
    #if (SUPPORT_VR_THUMBNAIL == 1)
    MMP_ULONG                       ThumbJpegSize;
    #endif

    MMPD_VIDENC_SetMemStart(ulStackAddr);
    m_ulVidRecEncodeAddr = curaddr = ulStackAddr;

    MergerBufSize = VideoRegStoreSize  + AVSyncBufSize + VideoSizeTableSize + VideoTimeTableSize + AudioCompBufSize;
	MergerBufSize = ((MergerBufSize + 31) >> 5) << 5;

	// Get Encode buffer config and size (YUV420)
    if (m_VidRecdModes.VidCurBufMode[0] == MMPS_3GPRECD_CURBUF_RT) {
        m_VidRecdInputBuf[0].ulBufCnt = 2;
    }
    else {
        #if (VR_SINGLE_CUR_FRAME == 1)
        m_VidRecdInputBuf[0].ulBufCnt = 1;
        #else
	    m_VidRecdInputBuf[0].ulBufCnt = 2 + MAX_B_FRAME_NUMS;
        #endif
    }

	ulEncFrameSize = ALIGN32((usEncW * usEncH * 3) / 2);

    #if (CHIP == MCR_V2)
    if (((usEncW>>4)*(usEncH>>4)) > 34560) //4096x2160
        return MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS;
    #endif
 
    //*************************
    // reg buffer ......... 60
    // AV Sync ............ 32
    // Frame Table ........ 1k
    // Time Table ......... 1k
    // Audio BS ........... 16k
    //*************************
	{
        // set reg store buffer
    	repackmiscbuf.ulVideoRegStoreAddr   = curaddr;
	    curaddr                             += VideoRegStoreSize;

	    // set av sync buffer
   		repackmiscbuf.ulVideoEncSyncAddr    = curaddr;
    	curaddr                             += AVSyncBufSize;

	    // set frame table buffer, must be 512 byte alignment
	   	repackmiscbuf.ulVideoSizeTableAddr  = curaddr;
	   	repackmiscbuf.ulVideoSizeTableSize  = VideoSizeTableSize;
	    curaddr                             += VideoSizeTableSize;

	   	// set time table buffer, must be 512 byte alignment
    	repackmiscbuf.ulVideoTimeTableAddr  = curaddr;
	    repackmiscbuf.ulVideoTimeTableSize  = VideoTimeTableSize;
    	curaddr                             += VideoTimeTableSize;

       	// set audio compressed buffer
        mergercompbuf.ulAudioCompBufStart   = curaddr;
        curaddr                             += AudioCompBufSize;
        mergercompbuf.ulAudioCompBufEnd     = curaddr;
	}
	
    if (MMPD_VIDENC_InitInstance(ulEncId, MMPD_VIDENC_MODULE_H264) != MMP_ERR_NONE)
    {
    	RTNA_DBG_Str(0, "\r\n#Err not available h264 instance\r\n");
		return MMP_3GPRECD_ERR_INVALID_PARAMETERS; //For WIFI, TBD
    }

    m_VidRecdID = *ulEncId;

	// set YUV REF/GEN buffer
	curaddr = ALIGN32(curaddr);
    m_ulVidRecRefGenBufAddr = curaddr;

    // Allocate memory for Video Ref/Gen frames, curaddr will be advanced
    MMPD_H264ENC_CalculateRefBuf(usEncW, usEncH, &(videohwbuf.refgenbd), &curaddr);

    // Allocate memory for encoder buffer (I420/NV12)
    if (m_VidRecdModes.VidCurBufMode[0] == MMPS_3GPRECD_CURBUF_RT) {
        for (i = 0; i < m_VidRecdInputBuf[0].ulBufCnt; i++) {
            m_VidRecdInputBuf[0].ulY[i] = ALIGN32(ulFBufAddr);
            ulFBufAddr += (usEncW << 4);
            m_VidRecdInputBuf[0].ulU[i] = ulFBufAddr;
            m_VidRecdInputBuf[0].ulV[i] = m_VidRecdInputBuf[0].ulU[i] + (usEncW << 2);
            ulFBufAddr += (usEncW << 3);
        }
        MMPD_H264ENC_SetSourceBuf(&m_VidRecdInputBuf[0]);
    }
    else {
        #if (USE_H264_CUR_BUF_AS_CAPT_BUF == 0)
        for (i = 0; i < m_VidRecdInputBuf[0].ulBufCnt; i++) {
            ulTmpBufSize = usEncW * usEncH;

            m_VidRecdInputBuf[0].ulY[i] = ALIGN32(curaddr);
            m_VidRecdInputBuf[0].ulU[i] = m_VidRecdInputBuf[0].ulY[i] + ulTmpBufSize;
            m_VidRecdInputBuf[0].ulV[i] = m_VidRecdInputBuf[0].ulU[i];
            curaddr += (ulTmpBufSize * 3)/2;
        }
        #endif
    }

    MMPS_3GPRECD_SetRecdPipeConfig(&m_VidRecdInputBuf[0], m_ulVREncodeBufW[0], m_ulVREncodeBufH[0], *ulEncId);

    // Set Slice Length Buffer, align32
    videohwbuf.miscbuf.ulSliceLenBuf    = curaddr;
    curaddr                             = curaddr + SliceLengBufSize;

    // Set MV buffer, #MB/Frame * #MVs/MB * #byte/MV
	curaddr                             = (curaddr + 63) >> 6 << 6; //align64
	#if (DUALENC_SUPPORT == 1)
	videohwbuf.miscbuf.ulMVBuf =        m_ulVidShareMvAddr;
	#else
	videohwbuf.miscbuf.ulMVBuf          = curaddr;
	curaddr                             += MVBufSize; // 2 bytes per MB
	#endif	

    // set h264 parameter set buf
	curaddr = ALIGN32(curaddr);
	headerbuf.ulSPSSize                 = SPSSize;
	headerbuf.ulPPSSize                 = PPSSize;
    //SPS
    headerbuf.ulSPSStart                = curaddr;
    curaddr                             += headerbuf.ulSPSSize;
    #if (SUPPORT_VUI_INFO)
    // Rebuild-SPS
    headerbuf.ulTmpSPSSize              = SPSSize+16;
    headerbuf.ulTmpSPSStart             = curaddr;
    curaddr                             += headerbuf.ulTmpSPSSize;
    #endif
    //PPS
    headerbuf.ulPPSStart                = curaddr;
    curaddr                             += headerbuf.ulPPSSize;

    #if (UVC_VIDRECD_SUPPORT == 1)
    if (gbUVCRecdSupport) {
	    // set h264 parameter set buf
		curaddr = ALIGN32(curaddr);
		UVCheaderbuf.ulSPSSize          = SPSSize;
		UVCheaderbuf.ulPPSSize          = PPSSize;
	    //SPS
	    UVCheaderbuf.ulSPSStart         = curaddr;
	    curaddr                         += UVCheaderbuf.ulSPSSize;
	    //PPS
	    UVCheaderbuf.ulPPSStart         = curaddr;
	    curaddr                         += UVCheaderbuf.ulPPSSize;
    }
    #endif

	// set video compressed buffer, 32 byte alignment
	curaddr                             = ALIGN32(curaddr);
	videohwbuf.bsbuf.ulStart            = curaddr;
	mergercompbuf.ulVideoCompBufStart   = curaddr;
	curaddr                             += VideoCompBufSize;
	
	videohwbuf.bsbuf.ulEnd              = curaddr;
	mergercompbuf.ulVideoCompBufEnd     = curaddr;
	
	#if (UVC_VIDRECD_SUPPORT == 1)
	if (gbUVCRecdSupport) { 
		UVCmergercompbuf.ulVideoCompBufStart   = curaddr;
		curaddr += UVCVidCompBufSize;
		UVCmergercompbuf.ulVideoCompBufEnd     = curaddr;
	}	
	#endif //  #if (UVC_VIDRECD_SUPPORT == 1)
        
	// set av repack buffer
	repackmiscbuf.ulAvRepackStartAddr   = curaddr;
	repackmiscbuf.ulAvRepackSize        = AVRepackBufSize;
	curaddr                             += repackmiscbuf.ulAvRepackSize;
	
	#if (UVC_VIDRECD_SUPPORT == 1)
	if (gbUVCRecdSupport) { 
		#if (VIDRECD_MULTI_TRACK == 0)
		UVCrepackmiscbuf.ulAvRepackStartAddr = curaddr;
		UVCrepackmiscbuf.ulAvRepackSize      = UVCRepackBufSize;
		curaddr                              += repackmiscbuf.ulAvRepackSize;
		// set frame table buffer, must be 512 byte alignment
	   	UVCrepackmiscbuf.ulVideoSizeTableAddr  = curaddr;
	   	UVCrepackmiscbuf.ulVideoSizeTableSize  = UVCVideoSizeTableSize;
	    curaddr                             += UVCVideoSizeTableSize;

	   	// set time table buffer, must be 512 byte alignment
    	UVCrepackmiscbuf.ulVideoTimeTableAddr  = curaddr;
	    UVCrepackmiscbuf.ulVideoTimeTableSize  = UVCVideoTimeTableSize;
    	curaddr                             += UVCVideoTimeTableSize;
		#endif
	}	
	
	#if (UVC_EMERGRECD_SUPPORT == 1 && VIDRECD_MULTI_TRACK == 0)
	if (gbUVCRecdSupport) { 
		#if (VIDRECD_MULTI_TRACK == 0)
		UVCE_repackmiscbuf.ulAvRepackStartAddr = curaddr;
		UVCE_repackmiscbuf.ulAvRepackSize      = UVCE_RepackBufSize;
		curaddr                                += repackmiscbuf.ulAvRepackSize;
		// set frame table buffer, must be 512 byte alignment
	   	UVCE_repackmiscbuf.ulVideoSizeTableAddr  = curaddr;
	   	UVCE_repackmiscbuf.ulVideoSizeTableSize  = UVCE_VideoSizeTableSize;
	    curaddr                                  += UVCE_VideoSizeTableSize;

	   	// set time table buffer, must be 512 byte alignment
    	UVCE_repackmiscbuf.ulVideoTimeTableAddr  = curaddr;
	    UVCE_repackmiscbuf.ulVideoTimeTableSize  = UVCE_VideoTimeTableSize;
    	curaddr                                  += UVCE_VideoTimeTableSize;  	
		#endif
	}	
	#endif 
	
	#endif // UVC_VIDRECD_SUPPORT = 1
	
    MMPD_3GPMGR_SetRepackMiscBuf(&repackmiscbuf, MMP_3GPRECD_FILETYPE_VIDRECD);
    MMPD_3GPMGR_SetEncodeCompBuf(&mergercompbuf, MMP_3GPRECD_FILETYPE_VIDRECD);
    
    #if (UVC_VIDRECD_SUPPORT == 1)
	if (gbUVCRecdSupport) { 
		MMPD_3GPMGR_SetEncodeCompBuf(&UVCmergercompbuf, MMP_3GPRECD_FILETYPE_UVCRECD);
		#if (VIDRECD_MULTI_TRACK == 0)
		MMPD_3GPMGR_SetUVCRepackMiscBuf(MMP_3GPRECD_FILETYPE_UVCRECD, &UVCrepackmiscbuf);
		#endif
		MMPD_H264ENC_SetUVCHdrBuf(&UVCheaderbuf);
	}
	#endif
	
	#if (UVC_EMERGRECD_SUPPORT == 1)
	if (gbUVCRecdSupport) { 
		#if (VIDRECD_MULTI_TRACK == 0)
		MMPD_3GPMGR_SetUVCRepackMiscBuf(MMP_3GPRECD_FILETYPE_UVCEMERG, &UVCE_repackmiscbuf);
		#endif
	}
	#endif

    MMPD_H264ENC_SetStoreBufParams (repackmiscbuf.ulVideoRegStoreAddr);
    MMPD_H264ENC_SetHeaderBuf      (*ulEncId, &headerbuf);
    MMPD_H264ENC_SetBitstreamBuf   (*ulEncId, &(videohwbuf.bsbuf));
    MMPD_H264ENC_SetRefGenBound    (*ulEncId, &(videohwbuf.refgenbd));
    MMPD_H264ENC_SetMiscBuf        (*ulEncId, &(videohwbuf.miscbuf));
    
    MMPD_AUDIO_SetEncodeBuf(mergercompbuf.ulAudioCompBufStart,
                           (mergercompbuf.ulAudioCompBufEnd - mergercompbuf.ulAudioCompBufStart));
	
    MMPS_System_GetPreviewFrameStart(&bufferEndAddr);

    m_ulVidRecDramEndAddr = curaddr;

    // Buffer for front-cam tail info
    {
        MMP_ULONG ulTailBufSize;

        if (m_VidRecdConfigs.ulTailBufSize)
            ulTailBufSize = m_VidRecdConfigs.ulTailBufSize;
        else
            ulTailBufSize = MMPS_3GPRECD_GetContainerTailBufSize();

        curaddr = ALIGN16(curaddr); // not necessary, but ALIGN4 is must
        ulTailBufSize = ALIGN32(ulTailBufSize); // not necessary
        #if (EMERGENTRECD_SUPPORT == 1)
        ulTailBufSize <<= 2;
        #endif

    	if ((curaddr + ulTailBufSize) > bufferEndAddr) {
    		PRINTF("Can't use high speed videor\r\n");
    		MMPD_3GPMGR_SetRecordTailSpeed(MMP_FALSE, 0, 0, MMP_3GPRECD_FILETYPE_VIDRECD);
    	}
    	else {
    		PRINTF("USE AIT high speed videor\r\n");
    		MMPD_3GPMGR_SetRecordTailSpeed(MMP_TRUE, curaddr, ulTailBufSize, MMP_3GPRECD_FILETYPE_VIDRECD);
    		curaddr += ulTailBufSize;
    		m_ulVidRecDramEndAddr = curaddr; 
    	}
    }

	#if (UVC_VIDRECD_SUPPORT == 1) && (VIDRECD_MULTI_TRACK == 0)
    // Buffer for rear-cam tail info
	if (gbUVCRecdSupport) { 
		curaddr = ALIGN16(curaddr); // not necessary, but ALIGN4 is must
		if ((curaddr + m_VidRecdConfigs.ulUVCTailBufSize) > bufferEndAddr) {
			PRINTF("Can't use high speed videor for UVC recording\r\n");
			MMPD_3GPMGR_SetRecordTailSpeed(MMP_FALSE, 0, 0, MMP_3GPRECD_FILETYPE_UVCRECD);
		}
		else {
			PRINTF("USE AIT high speed videor for UVC recording\r\n");
			MMPD_3GPMGR_SetRecordTailSpeed(MMP_TRUE, curaddr, m_VidRecdConfigs.ulUVCTailBufSize, MMP_3GPRECD_FILETYPE_UVCRECD);
			curaddr += m_VidRecdConfigs.ulUVCTailBufSize;
			m_ulVidRecDramEndAddr = curaddr; 
        }
    }
    #endif
    
    #if (UVC_EMERGRECD_SUPPORT == 1) && (VIDRECD_MULTI_TRACK == 0)
    // Buffer for rear-cam tail info
	if (gbUVCRecdSupport) { 
		curaddr = ALIGN16(curaddr); // not necessary, but ALIGN4 is must
		if ((curaddr + m_VidRecdConfigs.ulUVCEmergTailBufSize) > bufferEndAddr) {
			PRINTF("Can't use high speed videor for UVC recording\r\n");
			MMPD_3GPMGR_SetRecordTailSpeed(MMP_FALSE, 0, 0, MMP_3GPRECD_FILETYPE_UVCEMERG);
		}
		else {
			PRINTF("USE AIT high speed videor for UVC recording\r\n");
			MMPD_3GPMGR_SetRecordTailSpeed(MMP_TRUE, curaddr, m_VidRecdConfigs.ulUVCEmergTailBufSize, MMP_3GPRECD_FILETYPE_UVCEMERG);
			curaddr += m_VidRecdConfigs.ulUVCEmergTailBufSize;
			m_ulVidRecDramEndAddr = curaddr; 
        }
    }
    #endif   

    #if (SUPPORT_VR_THUMBNAIL == 1)
    curaddr = ALIGN32(curaddr);
    ThumbJpegSize = ALIGN32((m_ulVRThumbWidth * m_ulVRThumbHeight) >> 1);
    MMPD_3GPMGR_SetThumbnailInfo(curaddr, ThumbJpegSize);
    curaddr += ThumbJpegSize;
    m_ulVidRecDramEndAddr = curaddr;
    #endif

    #if defined(ALL_FW)
    if (m_ulVidRecDramEndAddr > MMPS_System_GetMemHeapEnd()) {
        printc("\t= [HeapMemErr] Video record = %x, %x\r\n", m_ulVidRecDramEndAddr, MMPS_System_GetMemHeapEnd());
        return MMP_3GPRECD_ERR_MEM_EXHAUSTED;
    }
    printc("End of video record buffers = 0x%X\r\n", m_ulVidRecDramEndAddr);
    #endif
    PRINTF("ctx%d end x%x/x%x\r\n", *ulEncId, m_ulVidRecDramEndAddr, ulFBufAddr);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetParameter
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Return parameter with specific type.
 @param[in] SetParam Specific parameter.
 @param[out] usValue Parameter value.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_GetParameter(MMPS_3GPRECD_PARAMETER SetParam, MMP_ULONG *ulValue)
{
    MMP_ULONG ulParam;

    switch (SetParam) {
    case MMPS_3GPRECD_PARAMETER_PREVIEWMODE:
        ulParam = m_VidRecdModes.usVideoPreviewMode;
        break;
    case MMPS_3GPRECD_PARAMETER_SRCMODE:
        ulParam = m_VidRecdModes.VideoSrcMode[0];
        break;
    case MMPS_3GPRECD_PARAMETER_VIDEO_FORMAT:
        switch (MMPD_VIDENC_GetFormat()) {
        case MMPD_MP4VENC_FORMAT_H264:
            ulParam = MMPS_3GPRECD_VIDEO_FORMAT_H264;
            break;
        default:
            ulParam = 0xFFFFFFFF;
            break;
        }
        break;
    case MMPS_3GPRECD_PARAMETER_AUDIO_FORMAT:
        switch (MMPD_3GPMGR_GetAudioFormat()) {
        case MMPD_3GPMGR_AUDIO_FORMAT_AAC:
            ulParam = MMPS_3GPRECD_AUDIO_FORMAT_AAC;
            break;
        case MMPD_3GPMGR_AUDIO_FORMAT_AMR:
            ulParam = MMPS_3GPRECD_AUDIO_FORMAT_AMR;
            break;
        case MMPD_3GPMGR_AUDIO_FORMAT_ADPCM:
            ulParam = MMPS_3GPRECD_AUDIO_FORMAT_ADPCM;
            break;
        case MMPD_3GPMGR_AUDIO_FORMAT_MP3:
            ulParam = MMPS_3GPRECD_AUDIO_FORMAT_MP3;
            break;
        case MMPD_3GPMGR_AUDIO_FORMAT_PCM:
            ulParam = MMPS_3GPRECD_AUDIO_FORMAT_PCM;
            break;
        default:
            ulParam = 0xFFFFFFFF;
            break;
        }
        break;
    case MMPS_3GPRECD_PARAMETER_RESOLUTION:
        ulParam = m_VidRecdModes.usVideoEncResIdx[0];
        break;
    case MMPS_3GPRECD_PARAMETER_BITRATE:
        ulParam = m_VidRecdModes.ulBitrate[0];
        break;
    case MMPS_3GPRECD_PARAMETER_FRAME_RATE:
        ulParam = (m_VidRecdModes.VideoEncFrameRate[0].usVopTimeIncrement << 16) |
                   m_VidRecdModes.VideoEncFrameRate[0].usVopTimeIncrResol;
        break;
    case MMPS_3GPRECD_PARAMETER_PROFILE:
        ulParam = m_VidRecdModes.VisualProfile[0];
        break;
    case MMPS_3GPRECD_PARAMETER_DRAM_END:
        // Can not get correct end address of allocated memory until start record!
        // When preview start, the value specifies the allocated memory for preview only,
        // encoder buffers are allocated in starting record.
        ulParam = m_ulVidRecDramEndAddr;
        break;
    default:
        ulParam = 0xFFFFFFFF;
        break;
    }

    *ulValue = ulParam;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_UpdateParameter
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Update record parameter dynamically. This function should not be used if
        recording is not yet started.
 @param[in] type    the specific parameter type.
 @param[in] param   Parameter value to update.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_UpdateParameter(MMPS_3GPRECD_PARAMETER type, void *param)
{
    MMP_ERR                 err;
    MMPD_MP4VENC_FW_OP      status_vid;
    MMPS_3GPRECD_FRAMERATE  *fps;

    if (m_VidRecdID == INVALID_ENC_ID)
        status_vid = MMPD_MP4VENC_FW_OP_NONE;
    else
        MMPD_VIDENC_GetStatus(m_VidRecdID, &status_vid);

    if ((status_vid == MMPD_MP4VENC_FW_OP_NONE) ||
        (status_vid == MMPD_MP4VENC_FW_OP_STOP)) {
        return MMP_3GPRECD_ERR_GENERAL_ERROR;
    }

    switch (type) {
    case MMPS_3GPRECD_PARAMETER_FRAME_RATE:
        fps = (MMPS_3GPRECD_FRAMERATE *)param;
        if ((fps->usVopTimeIncrement == 0) || (fps->usVopTimeIncrResol == 0))
            return MMP_3GPRECD_ERR_INVALID_PARAMETERS;

        // Update encode frame rate
        err = MMPD_VIDENC_UpdateEncFrameRate(m_VidRecdID,
                                             fps->usVopTimeIncrement, 
                                             fps->usVopTimeIncrResol);

        m_VidRecdModes.VideoEncFrameRate[0].usVopTimeIncrement = fps->usVopTimeIncrement;
        m_VidRecdModes.VideoEncFrameRate[0].usVopTimeIncrResol = fps->usVopTimeIncrResol;
        break;
    default:
        return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
    }

    return err;
}


//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetFrame
//  Description :
//------------------------------------------------------------------------------
/**
@brief Return the last video preview frame in RGB format
@param[in] usTargetW The width of the requested frame.
@param[in] usTargetH The height of the requested frame.
@param[out] pusOutBuf Pointer to the output buffer.
@param[in,out] ulSize The size of output buffer which contains requested frame.
@retval MMP_ERR_NONE Success.
@retval MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS No support input parameter
*/
MMP_ERR MMPS_3GPRECD_GetFrame(MMP_USHORT usTargetW, MMP_USHORT usTargetH, MMP_USHORT *pusOutBuf, MMP_ULONG *pulSize)
{
    MMP_GRAPHICS_BUF_ATTR	srcBufAttr, dstBufAttr;
    MMP_GRAPHICS_RECT    	rect;
    MMPD_FCTL_LINK       	fctllink;
    MMPD_FCTL_ATTR  		fctlAttr;
    
    // For H264 frame mode only
    if (m_VidRecdModes.VidCurBufMode[0] != MMPS_3GPRECD_CURBUF_FRAME) {
    	return MMP_ERR_NONE;
    }

    MMPD_Fctl_GetAttributes(m_RecordFctlLink.ibcpipeID, &fctlAttr);

	fctllink = fctlAttr.fctllink;
          
    srcBufAttr.usWidth  	= m_ulVREncodeW[0];
    srcBufAttr.usHeight 	= m_ulVREncodeH[0];
	srcBufAttr.usLineOffset = srcBufAttr.usWidth;
	srcBufAttr.colordepth   = MMP_GRAPHICS_COLORDEPTH_YUV420_INTERLEAVE;
	srcBufAttr.ulBaseAddr  	= fctlAttr.ulBaseAddr[0];
	srcBufAttr.ulBaseUAddr 	= fctlAttr.ulBaseUAddr[0];
	srcBufAttr.ulBaseVAddr 	= fctlAttr.ulBaseVAddr[0];

	rect.usLeft   			= 0;
	rect.usTop    			= 0;
	rect.usWidth  			= srcBufAttr.usWidth;
	rect.usHeight 			= srcBufAttr.usHeight;

    *pulSize = usTargetW * usTargetH * 2;
	
	if (*pulSize <= (srcBufAttr.usWidth * srcBufAttr.usHeight * 3 / 2)) {
		if (fctlAttr.usBufCnt > 1) {
            dstBufAttr.ulBaseAddr = fctlAttr.ulBaseAddr[1];
        }
        else if ((fctlAttr.usBufCnt == 1) && (fctlAttr.usRotateBufCnt > 0)) {
            dstBufAttr.ulBaseAddr = fctlAttr.ulRotateAddr[0];
        }
	}
	else {

        dstBufAttr.ulBaseAddr = m_ulVideoPreviewEndAddr;
	}

	dstBufAttr.ulBaseUAddr  = 0;
	dstBufAttr.ulBaseVAddr  = 0;
	dstBufAttr.usWidth      = usTargetW;
	dstBufAttr.usHeight     = usTargetH;
	dstBufAttr.usLineOffset = usTargetW * 2;
	dstBufAttr.colordepth   = MMP_GRAPHICS_COLORDEPTH_16; // RGB565

	if (MMPD_Fctl_RawBuf2IbcBuf(&fctllink, &srcBufAttr, &rect, &dstBufAttr, 1)) {
		return MMP_3GPRECD_ERR_PARAMETER;
	}

	// Copy the image back to the host
	if (MMPD_DMA_MoveData(dstBufAttr.ulBaseAddr, (MMP_ULONG)pusOutBuf, *pulSize, NULL, NULL)) {
		return MMP_3GPRECD_ERR_PARAMETER;
	}

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetRecordSpeed
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_SetRecordSpeed(MMPS_3GPRECD_SPEED_MODE SpeedMode, MMPS_3GPRECD_SPEED_RATIO SpeedRatio)
{
    if ((MMPD_VIDENC_GetFormat() == MMPD_MP4VENC_FORMAT_H264) && (SpeedMode == MMPS_3GPRECD_SPEED_SLOW)) {
        PRINTF("Not support H264 slow record mode !\r\n");
        return MMPD_3GPMGR_SetRecordSpeed(MMPS_3GPRECD_SPEED_NORMAL, MMPS_3GPRECD_SPEED_1X);
    }
    
    return MMPD_3GPMGR_SetRecordSpeed(SpeedMode, SpeedRatio);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_StartSeamless
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Start video seamless recording, 
        can be called before or after @ref MMPS_3GPRECD_StartRecord
 @param[in] bStart start or stop
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_StartSeamless(MMP_BOOL bStart)
{
    if (m_VidRecdConfigs.bSeamlessMode == MMP_TRUE) {
        m_bSeamlessEnabled = bStart;
        return MMPD_3GPMGR_SetSeamless(bStart);
    }

    return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetStillCaptureMaxRes
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Start face detection
 @param[in] bStart start or not
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetStillCaptureMaxRes(MMP_USHORT usJpegW, MMP_USHORT usJpegH)
{
    m_usMaxStillJpegW = usJpegW;
    m_usMaxStillJpegH = usJpegH;
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_StillCapture
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_StillCapture(MMPS_3GPRECD_STILL_CAPTURE_INFO *pVRCaptInfo)
{
    MMP_USHORT              ulInWidth, ulInHeight;
    MMP_ULONG               ulJpegSize;
    MMP_BOOL                bDualCamCapture = MMP_FALSE;
    MMPD_MP4VENC_FW_OP      status_fw;
    MMPS_DSC_CAPTURE_INFO 	captureinfo;
    MMP_IBC_PIPEID     		ibc_pipe;
    MMP_SCAL_FIT_RANGE   	fitrange;
    MMP_SCAL_GRAB_CTRL 		grabctl;
    MMP_SCAL_SOURCE			ScalerSrc   = MMP_SCAL_SOURCE_ISP;
	MMP_ERR 				err         = MMP_ERR_NONE;
	MMP_ERR 				RearCamErr  = MMP_ERR_NONE;
	MMP_ULONG				ulSramAddr  = m_ulVidRecCaptureSramAddr;
	MMP_ULONG				ulDramAddr  = m_ulVidRecCaptureDramAddr;
	#if (HANDLE_JPEG_EVENT_BY_QUEUE)
	MMP_BOOL                bRecording  = MMP_FALSE;
	#endif
    #if (VR_STILL_USE_REF_FRM)
    MMP_BOOL                bVidRefFrmAsSrc = MMP_FALSE;
    MMP_GRAPHICS_BUF_ATTR	bufAttr = {0, };
	MMP_GRAPHICS_RECT		srcrect = {0, };
    #endif

    if (!m_VidRecdConfigs.bStillCaptureEnable) {
        return err;
    }
    if ((pVRCaptInfo->sCaptSrc & MMP_3GPRECD_CAPTURE_SRC_REAR_CAM) &&
        (m_VidRecdConfigs.bDualCaptureEnable))
    {
        #if (REAR_CAM_TYPE == REAR_CAM_TYPE_SONIX_MJPEG2H264 || \
             REAR_CAM_TYPE == REAR_CAM_TYPE_SONIX_MJPEG)

        bDualCamCapture = MMP_TRUE;

        // RearCam enable capture here(non blocking). Delay 2 frames for sync with front cam jpeg capture.
		RearCamErr = MMPD_USB_CaptureJpeg(m_ulVidRecDualCaptureDramAddr, 
		                                  m_ulVidRecDualCaptureDramSize, 
		                                  2);
		#else
		RearCamErr = MMP_ERR_NONE; //TBD 
        #endif
    }
    if (pVRCaptInfo->sCaptSrc & MMP_3GPRECD_CAPTURE_SRC_FRONT_CAM)
    {
        /* Check the pipe is alive or not */
        if (!m_bVidPreviewActive[0]) {
            return MMP_3GPRECD_ERR_STILL_CAPTURE;
        }

        if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_2PIPE ||
        	m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_LB_SINGLE) {
#if (REAR_CAM_TYPE == REAR_CAM_TYPE_SONIX_MJPEG2H264)
            ibc_pipe =m_RecordFctlLink.ibcpipeID;
#elif (REAR_CAM_TYPE == REAR_CAM_TYPE_AIT)
			#if (HANDLE_JPEG_EVENT_BY_QUEUE)
            ibc_pipe =m_RecordFctlLink.ibcpipeID;
			#else
            ibc_pipe =m_PreviewFctlLink.ibcpipeID;
            #endif
#else
            ibc_pipe =m_PreviewFctlLink.ibcpipeID;
#endif

            #if (VR_STILL_USE_REF_FRM)
            if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_2PIPE) {
                /* Use H.264 reference frame to encode JPEG */
                MMPD_VIDENC_GetStatus(m_VidRecdID, &status_fw);
                
                if ((status_fw == MMPD_MP4VENC_FW_OP_START) ||
                    (status_fw == MMPD_MP4VENC_FW_OP_PREENCODE)) {
                    bVidRefFrmAsSrc = MMP_TRUE;
                }
            }
            #endif
        }
        else {
            return MMP_3GPRECD_ERR_STILL_CAPTURE;
        }

        #if (VR_STILL_USE_REF_FRM)
        if (bVidRefFrmAsSrc) {
            ulInWidth   = m_ulVREncodeBufW[0];
            ulInHeight  = m_ulVREncodeBufH[0];
        }
        else {
            MMPD_Scaler_GetGrabRange(m_PreviewFctlLink.scalerpath,
                                     MMP_SCAL_GRAB_STAGE_LPF,
                                     &ulInWidth, &ulInHeight);
        }
        #else
        //MMPD_Scaler_GetGrabRange(m_PreviewFctlLink.scalerpath, MMP_SCAL_GRAB_STAGE_LPF, &ulInWidth, &ulInHeight);
		MMPS_Sensor_GetCurPrevScalInputRes(m_ubVidRecModeSnrId, (MMP_ULONG *)&ulInWidth, (MMP_ULONG *)&ulInHeight);
        #endif

		fitrange.fitmode    	= MMP_SCAL_FITMODE_OUT;
        fitrange.scalerType 	= MMP_SCAL_TYPE_SCALER;
        fitrange.ulInWidth	    = ulInWidth;
        fitrange.ulInHeight 	= ulInHeight;
        fitrange.ulOutWidth	    = pVRCaptInfo->usWidth;
        fitrange.ulOutHeight	= pVRCaptInfo->usHeight;

        fitrange.ulInGrabX  	= 1;
        fitrange.ulInGrabY  	= 1;
        fitrange.ulInGrabW  	= fitrange.ulInWidth;
        fitrange.ulInGrabH  	= fitrange.ulInHeight;

        MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);

        #if (HANDLE_JPEG_EVENT_BY_QUEUE)
        
        #if (USE_H264_CUR_BUF_AS_CAPT_BUF)
        MMPD_VIDENC_GetStatus(m_VidRecdID, &status_fw);
        
        if ((status_fw == MMPD_MP4VENC_FW_OP_START) ||
            (status_fw == MMPD_MP4VENC_FW_OP_PREENCODE)) {
            
            bRecording = MMP_TRUE;
        }
        else {
            bRecording = MMP_FALSE;
              
            if(!m_bInitRecPipeConfig)
            {
                m_bInitRecPipeConfig = MMP_TRUE;
                MMPS_3GPRECD_StillCaptureMemConfig(&m_VidRecdInputBuf[0]);
            }
            MMPF_Display_GetCurIbcFrameBuffer(m_RecordFctlLink.ibcpipeID,
                                              &m_ulVidRecCaptureFrmBufAddr,
                                              NULL,
                                              NULL);
        }
        #endif
        
		MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_TRUE);
        err = MMPS_DSC_CaptureByQueue(ibc_pipe, 
                                      &captureinfo,
                                      ScalerSrc,
                                      &fitrange, 
                                      &grabctl,
                                      &ulJpegSize,
                                      ulDramAddr,
                                      ulSramAddr,
                                      m_ulVidRecCaptureFrmBufAddr,
                                      pVRCaptInfo,
                                      bRecording);
        
		MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_FALSE);
        #else

        MMPS_DSC_SetShotMode(MMPS_DSC_SINGLE_SHOTMODE);
        MMPS_DSC_SetJpegEncParam(DSC_RESOL_IDX_UNDEF, pVRCaptInfo->usWidth, pVRCaptInfo->usHeight, MMP_DSC_JPEG_RC_ID_CAPTURE);

#if 1 //No thumbnail in Video capture mode.
        MMPS_DSC_ConfigThumbnail(pVRCaptInfo->usThumbWidth, pVRCaptInfo->usThumbHeight, MMP_DSC_THUMB_SRC_NONE);
#else
        if (bDualCamCapture)
            MMPS_DSC_ConfigThumbnail(pVRCaptInfo->usThumbWidth, pVRCaptInfo->usThumbHeight, MMP_DSC_THUMB_SRC_NONE);
        else
            MMPS_DSC_ConfigThumbnail(pVRCaptInfo->usThumbWidth, pVRCaptInfo->usThumbHeight, MMP_DSC_THUMB_SRC_DECODED_JPEG);
#endif        
#if 1
		//MMPS_DSC_SetCaptureJpegQualityEX(MMP_DSC_JPEG_RC_ID_CAPTURE, MMP_TRUE, MMP_FALSE, 
		//                               200, 220, 3, MMP_DSC_CAPTURE_HIGH_QUALITY);
		MMPS_DSC_SetCaptureJpegQuality(MMP_DSC_JPEG_RC_ID_CAPTURE,
                                       pVRCaptInfo->bTargetCtl, pVRCaptInfo->bLimitCtl, pVRCaptInfo->bTargetSize,
        							   pVRCaptInfo->bLimitSize, pVRCaptInfo->bMaxTrialCnt, pVRCaptInfo->Quality);
#else
            MMPS_DSC_SetCaptureJpegQuality(MMP_DSC_JPEG_RC_ID_CAPTURE,
                pVRCaptInfo->bTargetCtl, pVRCaptInfo->bLimitCtl, pVRCaptInfo->bTargetSize, 
                pVRCaptInfo->bLimitSize, pVRCaptInfo->bMaxTrialCnt, pVRCaptInfo->Quality);
#endif
        MMPS_DSC_SetSystemBuf(&ulDramAddr, MMP_FALSE, MMP_TRUE, MMP_TRUE);
        MMPS_DSC_SetCaptureBuf(&ulSramAddr, &ulDramAddr, MMP_TRUE, MMP_FALSE, MMP_DSC_CAPTURE_NO_ROTATE, MMP_TRUE);

		captureinfo.bFirstShot 	= MMP_TRUE;
		captureinfo.bExif 	    = pVRCaptInfo->bExifEn;

        if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_LB_SINGLE) {

        	captureinfo.bThumbnail = MMP_FALSE;
			ScalerSrc = MMP_SCAL_SOURCE_LDC;
        }
        else {
	        captureinfo.bThumbnail = pVRCaptInfo->bThumbEn;

            #if (VR_STILL_USE_REF_FRM)
            if (bVidRefFrmAsSrc) {
                /* Setup graphic settings for loop-back */
                ScalerSrc = MMP_SCAL_SOURCE_GRA;

                bufAttr.usWidth 		= m_ulVREncodeBufW[0];
        		bufAttr.usHeight		= m_ulVREncodeBufH[0];
        		bufAttr.usLineOffset	= bufAttr.usWidth;
        		bufAttr.colordepth		= MMP_GRAPHICS_COLORDEPTH_YUV420_INTERLEAVE;

        		srcrect.usLeft			= 0;
        		srcrect.usTop			= 0;
        		srcrect.usWidth 		= bufAttr.usWidth;
        		srcrect.usHeight		= bufAttr.usHeight;

                MMPD_Graphics_SetScaleAttr(&bufAttr, &srcrect, 1);
		        MMPD_Graphics_SetDelayType(MMP_GRAPHICS_DELAY_CHK_SCA_BUSY);
                MMPD_Graphics_SetPixDelay(10, 20);
				MMPD_Graphics_SetLineDelay(0);
            }
            else {
                ScalerSrc = MMP_SCAL_SOURCE_ISP;
            }
            #else
            ScalerSrc = MMP_SCAL_SOURCE_ISP;
            #endif
        }

        err = MMPS_DSC_CaptureAfterSrcReady(ibc_pipe, &captureinfo, ScalerSrc, &fitrange, &grabctl, &ulJpegSize, bDualCamCapture);

        #endif

        if (err == MMP_ERR_NONE) 
        {
            captureinfo.ubFilename      = pVRCaptInfo->bFileName;
            captureinfo.usFilenamelen   = pVRCaptInfo->ulFileNameLen;
            captureinfo.bFirstShot 		= MMP_TRUE;	
            captureinfo.ulExtraBufAddr	= 0;

            MMPD_VIDENC_GetStatus(m_VidRecdID, &status_fw);
            
            if ((status_fw == MMPD_MP4VENC_FW_OP_START) ||
                (status_fw == MMPD_MP4VENC_FW_OP_PAUSE) ||
                (status_fw == MMPD_MP4VENC_FW_OP_RESUME)) {
                
                /* Inform encoder the reserved space is reduced */
                if ((err = MMPD_3GPMGR_MakeRoom(m_VidRecdID, ulJpegSize)) != MMP_ERR_NONE) {
                    RTNA_DBG_Str(0, "FrontCam MakeRoom Err:");
                    RTNA_DBG_Long(0, err);
                    RTNA_DBG_Str(0, "\r\n");
                    return MMP_3GPRECD_ERR_NOT_ENOUGH_SPACE;
                }
            }
            if ((err = MMPS_DSC_JpegDram2Card(&captureinfo)) != MMP_ERR_NONE) {
                RTNA_DBG_Str(0, "FrontCam Save Card Err:");
                RTNA_DBG_Long(0, err);
                RTNA_DBG_Str(0, ", ");
                RTNA_DBG_Str(0, captureinfo.ubFilename);
                RTNA_DBG_Str(0, "\r\n");
                return MMP_3GPRECD_ERR_STILL_CAPTURE;
			}
        }
        else {
            RTNA_DBG_Str(0, "FrontCam Capture After SrcReady Err:");
            RTNA_DBG_Long(0, err);
            RTNA_DBG_Str(0, "\r\n");
            return MMP_3GPRECD_ERR_STILL_CAPTURE;
        }
    }

    if ((pVRCaptInfo->sCaptSrc & MMP_3GPRECD_CAPTURE_SRC_REAR_CAM) &&
        (m_VidRecdConfigs.bDualCaptureEnable))
    {
        #if (REAR_CAM_TYPE == REAR_CAM_TYPE_SONIX_MJPEG2H264 || \
             REAR_CAM_TYPE == REAR_CAM_TYPE_SONIX_MJPEG)

        if (RearCamErr != MMP_ERR_NONE) {
            RTNA_DBG_Str(0, "Capture rear cam error: ");
            RTNA_DBG_Long(0, RearCamErr);
            RTNA_DBG_Str(0, " \r\n");
        }
        
        if (MMPD_USB_QueryJpeg(&ulJpegSize) != MMP_ERR_NONE) {
            RTNA_DBG_Str(0, "Capture rear cam failed \r\n");
            return MMP_3GPRECD_ERR_STILL_CAPTURE;
        }
        
        if (ulJpegSize == 0) {
            RTNA_DBG_Str(0, "Capture rear cam size is Zero\r\n");
            return MMP_3GPRECD_ERR_STILL_CAPTURE;
        }
        
        MMPD_VIDENC_GetStatus(m_VidRecdID, &status_fw);
        
        if ((status_fw == MMPD_MP4VENC_FW_OP_START) ||
            (status_fw == MMPD_MP4VENC_FW_OP_PAUSE) ||
            (status_fw == MMPD_MP4VENC_FW_OP_RESUME)) {
            
            /* Inform encoder the reserved space is reduced */
            if ((RearCamErr = MMPD_3GPMGR_MakeRoom(m_VidRecdID, ulJpegSize)) != MMP_ERR_NONE) {
                RTNA_DBG_Str(0, "RearCam MakeRoom Err:");
                RTNA_DBG_Long(0, RearCamErr);
                RTNA_DBG_Str(0, "\r\n");
                return MMP_3GPRECD_ERR_NOT_ENOUGH_SPACE;
            }
        }
        
        MMPD_DSC_SetFileName(pVRCaptInfo->bRearFileName, pVRCaptInfo->ulRearFileNameLen);
		RearCamErr = MMPD_DSC_JpegDram2Card(m_ulVidRecDualCaptureDramAddr, 
									  ulJpegSize, 
									  MMP_TRUE,
									  MMP_TRUE);

        if (RearCamErr != MMP_ERR_NONE) {
            RTNA_DBG_Str(0, "RearCam Save Card Err:");
            RTNA_DBG_Long(0, RearCamErr);
            RTNA_DBG_Str(0, ", ");
            RTNA_DBG_Str(0, pVRCaptInfo->bRearFileName);
            RTNA_DBG_Str(0, "\r\n");
            return MMP_3GPRECD_ERR_STILL_CAPTURE;
        }
#elif (REAR_CAM_TYPE == REAR_CAM_TYPE_TV_DECODER)
        ibc_pipe = m_2ndPrewFctlLink.ibcpipeID;

        MMPD_Scaler_GetGrabRange(m_2ndPrewFctlLink.scalerpath, MMP_SCAL_GRAB_STAGE_LPF, &ulInWidth, &ulInHeight);

        fitrange.fitmode    	= MMP_SCAL_FITMODE_OUT;
        fitrange.scalerType 	= MMP_SCAL_TYPE_SCALER;
        fitrange.ulInWidth	    = ulInWidth;
        fitrange.ulInHeight 	= ulInHeight;
        fitrange.ulOutWidth	    = ulInWidth; // 1 : 1 capture.
        fitrange.ulOutHeight	= ulInHeight;

        pVRCaptInfo->usWidth = ulInWidth; // 1 : 1 capture.
        pVRCaptInfo->usHeight = ulInHeight;

        fitrange.ulInGrabX  	= 1;
        fitrange.ulInGrabY  	= 1;
        fitrange.ulInGrabW  	= fitrange.ulInWidth;
        fitrange.ulInGrabH  	= fitrange.ulInHeight;

        MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);

        
        MMPS_DSC_SetShotMode(MMPS_DSC_SINGLE_SHOTMODE);
        MMPS_DSC_SetJpegEncParam(DSC_RESOL_IDX_UNDEF, pVRCaptInfo->usWidth, pVRCaptInfo->usHeight, MMP_DSC_JPEG_RC_ID_CAPTURE);
        
        MMPS_DSC_ConfigThumbnail(pVRCaptInfo->usThumbWidth, pVRCaptInfo->usThumbHeight, MMP_DSC_THUMB_SRC_NONE);
        MMPS_DSC_SetCaptureJpegQuality(MMP_DSC_JPEG_RC_ID_CAPTURE,
                                       pVRCaptInfo->bTargetCtl, pVRCaptInfo->bLimitCtl, pVRCaptInfo->bTargetSize, 
        							   pVRCaptInfo->bLimitSize, pVRCaptInfo->bMaxTrialCnt, pVRCaptInfo->Quality);
        MMPS_DSC_SetSystemBuf(&ulDramAddr, MMP_FALSE, MMP_TRUE, MMP_TRUE);
        MMPS_DSC_SetCaptureBuf(&ulSramAddr, &ulDramAddr, MMP_TRUE, MMP_FALSE, MMP_DSC_CAPTURE_NO_ROTATE, MMP_TRUE);

        captureinfo.bFirstShot 	= MMP_TRUE;
        captureinfo.bExif 	    = pVRCaptInfo->bExifEn;

        captureinfo.bThumbnail = pVRCaptInfo->bThumbEn;

        ScalerSrc = MMP_SCAL_SOURCE_GRA;

        err = MMPS_DSC_CaptureAfterSrcReady(ibc_pipe, &captureinfo, ScalerSrc, &fitrange, &grabctl, &ulJpegSize, bDualCamCapture);
        
        if (err == MMP_ERR_NONE) {
            captureinfo.ubFilename      = pVRCaptInfo->bFileName;
            captureinfo.usFilenamelen   = pVRCaptInfo->ulFileNameLen;
            captureinfo.bFirstShot 		= MMP_TRUE;	
            captureinfo.ulExtraBufAddr	= 0;

            MMPD_VIDENC_GetStatus(m_VidRecdID, &status_fw);
            
            if ((status_fw == MMPD_MP4VENC_FW_OP_START) ||
                (status_fw == MMPD_MP4VENC_FW_OP_PAUSE) ||
                (status_fw == MMPD_MP4VENC_FW_OP_RESUME)) {
                
                /* Inform encoder the reserved space is reduced */
                if ((err = MMPD_3GPMGR_MakeRoom(m_VidRecdID, ulJpegSize)) != MMP_ERR_NONE) {
                    RTNA_DBG_Str(0, "FrontCam MakeRoom Err:");
                    RTNA_DBG_Long(0, err);
                    RTNA_DBG_Str(0, "\r\n");
                    return MMP_3GPRECD_ERR_NOT_ENOUGH_SPACE;
                }
            }
#if 1
            {
                extern MMPS_DSC_MULTISHOT_INFO 		m_MultiShotInfo;

                err = MMPD_DSC_SetFileName(pVRCaptInfo->bRearFileName, pVRCaptInfo->ulRearFileNameLen);
                err =  MMPD_DSC_JpegDram2Card(m_MultiShotInfo.ulPrimaryJpegAddr[0], 
    									  m_MultiShotInfo.ulPrimaryJpegSize[0], 
    									  MMP_TRUE, MMP_TRUE);
            }          
#else
            if ((err = MMPS_DSC_JpegDram2Card(&captureinfo)) != MMP_ERR_NONE) {
                RTNA_DBG_Str(0, "FrontCam Save Card Err:");
                RTNA_DBG_Long(0, err);
                RTNA_DBG_Str(0, ", ");
                RTNA_DBG_Str(0, captureinfo.ubFilename);
                RTNA_DBG_Str(0, "\r\n");
                return MMP_3GPRECD_ERR_STILL_CAPTURE;
		}
#endif            
        }
        else {
            RTNA_DBG_Str(0, "FrontCam Capture After SrcReady Err:");
            RTNA_DBG_Long(0, err);
            RTNA_DBG_Str(0, "\r\n");
            return MMP_3GPRECD_ERR_STILL_CAPTURE;
        }

#endif
    }

    return MMP_ERR_NONE;
}

#if (SUPPORT_VR_THUMBNAIL)
//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_TakeThumbnail2Chunk
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Start face detection
 @param[in] bStart start or not
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_TakeThumbnail2Chunk(MMPS_3GPRECD_STILL_CAPTURE_INFO *pVRCaptInfo)
{
    MMP_USHORT              ulInWidth, ulInHeight;
    MMP_ULONG				ulJpegSize;
    MMPD_MP4VENC_FW_OP      status_fw;
    MMPS_DSC_CAPTURE_INFO 	captureinfo;
    MMP_IBC_PIPEID     		ibc_pipe;
    MMP_SCAL_FIT_RANGE   	fitrange;
    MMP_SCAL_GRAB_CTRL 		grabctl;
    MMP_SCAL_SOURCE			ScalerSrc = MMP_SCAL_SOURCE_ISP;
	MMP_ERR 				err;
    MMP_ULONG				ulSramAddr = m_ulVidRecCaptureSramAddr;
	MMP_ULONG				ulDramAddr = m_ulVidRecCaptureDramAddr;

    if (m_VidRecdConfigs.bStillCaptureEnable && m_bVidPreviewActive[0]) {

        if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_2PIPE ||
        	m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_LB_SINGLE) {
            ibc_pipe = m_PreviewFctlLink.ibcpipeID;
        }
        else {
            return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
        }

        MMPD_Scaler_GetGrabRange(m_PreviewFctlLink.scalerpath, MMP_SCAL_GRAB_STAGE_LPF, &ulInWidth, &ulInHeight);

		fitrange.fitmode    	= MMP_SCAL_FITMODE_OUT;
        fitrange.scalerType 	= MMP_SCAL_TYPE_SCALER;
        fitrange.ulInWidth	    = ulInWidth;
        fitrange.ulInHeight 	= ulInHeight;
        fitrange.ulOutWidth	    = pVRCaptInfo->usWidth;
        fitrange.ulOutHeight	= pVRCaptInfo->usHeight;

        fitrange.ulInGrabX  	= 1;
        fitrange.ulInGrabY  	= 1;
        fitrange.ulInGrabW  	= fitrange.ulInWidth;
        fitrange.ulInGrabH  	= fitrange.ulInHeight;

        MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);

        MMPS_DSC_SetShotMode(MMPS_DSC_SINGLE_SHOTMODE);
        MMPS_DSC_SetJpegEncParam(DSC_RESOL_IDX_UNDEF, pVRCaptInfo->usWidth, pVRCaptInfo->usHeight, MMP_DSC_JPEG_RC_ID_CAPTURE);
        MMPS_DSC_ConfigThumbnail(pVRCaptInfo->usThumbWidth, pVRCaptInfo->usThumbHeight, MMP_DSC_THUMB_SRC_NONE);

        MMPS_DSC_SetCaptureJpegQuality(MMP_DSC_JPEG_RC_ID_CAPTURE,
                                       pVRCaptInfo->bTargetCtl, pVRCaptInfo->bLimitCtl, pVRCaptInfo->bTargetSize, 
        							   pVRCaptInfo->bLimitSize, pVRCaptInfo->bMaxTrialCnt, pVRCaptInfo->Quality);

        MMPS_DSC_SetSystemBuf(&ulDramAddr, MMP_FALSE, MMP_TRUE, MMP_TRUE);
        MMPS_DSC_SetCaptureBuf(&ulSramAddr, &ulDramAddr, MMP_TRUE, MMP_FALSE, MMP_DSC_CAPTURE_NO_ROTATE, MMP_TRUE);

		captureinfo.bFirstShot 	= MMP_TRUE;
		captureinfo.bExif 	    = MMP_FALSE;
		captureinfo.bThumbnail 	= MMP_FALSE;
		
        if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_LB_SINGLE) {
			ScalerSrc = MMP_SCAL_SOURCE_LDC;
        }
        else {
			ScalerSrc = MMP_SCAL_SOURCE_ISP;
        }

		err = MMPS_DSC_CaptureAfterSrcReady(ibc_pipe, &captureinfo, ScalerSrc, &fitrange, &grabctl, &ulJpegSize, MMP_FALSE);

        if (err == MMP_ERR_NONE) 
        {
            MMPD_VIDENC_GetStatus(m_VidRecdID, &status_fw);
            
            if ((status_fw == MMPD_MP4VENC_FW_OP_START) ||
                (status_fw == MMPD_MP4VENC_FW_OP_PAUSE) ||
                (status_fw == MMPD_MP4VENC_FW_OP_RESUME)) {
                
                /* Inform encoder the reserved space is reduced */
                if (MMPD_3GPMGR_MakeRoom(m_VidRecdID, ulJpegSize) != MMP_ERR_NONE) {
                    return MMP_3GPRECD_ERR_NOT_ENOUGH_SPACE;
                }
            }

			err = MMPS_DSC_JpegDram2Chunk();

            return err;
        }
        else {
            return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
        }
    }

    return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetQPBoundary
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set QP boundary
 @param[in] ulLowerBound lower bound
 @param[in] ulUpperBound upper bound
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetQPBoundary(MMP_ULONG ulLowerBound, MMP_ULONG ulUpperBound)
{
    return MMPD_VIDENC_SetQPBoundary(ulLowerBound, ulUpperBound);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_RegisterCallback
//  Description :
//------------------------------------------------------------------------------
/** @brief The function set callbacks for recording events

The function sets callbacks for the following events.
    * media full  : record stopped due to card full (Task)
    * file full   : record stopped due to the specified time/space limitation (Task)
    * media slow  : record stopped due to the insufficient speed of media (ISR)
    * media error : record stopped due to some error in media writting (Task)
    * seamless    : to trigger the next recording in seamless mode (Task)
    * encode start: encoder started (Task/ISR)
    * encode stop : encoder stopped (ISR)
    * postprocess : for appending the user data in the file tail (Task)

@param[in] Event    Specified the event for register.
@param[in] CallBack The callback to be executed when the specified event happens.

@return It reports the status of the operation.

@warn The registered callback will not be auto deleted, please register a NULL
      to delete it.
*/
MMP_ERR MMPS_3GPRECD_RegisterCallback(MMPS_3GPRECD_EVENT Event, void *CallBack)
{
    MMPD_3GPMGR_EVENT e;

    switch (Event) {
    case MMPS_3GPRECD_EVENT_MEDIA_FULL:
        e = MMPD_3GPMGR_EVENT_MEDIA_FULL;
        break;
    case MMPS_3GPRECD_EVENT_FILE_FULL:
        e = MMPD_3GPMGR_EVENT_FILE_FULL;
        break;
    case MMPS_3GPRECD_EVENT_LONG_TIME_FILE_FULL:
        e = MMPD_3GPMGR_EVENT_LONG_TIME_FILE_FULL;
        break;
    case MMPS_3GPRECD_EVENT_MEDIA_SLOW:
        e = MMPD_3GPMGR_EVENT_MEDIA_SLOW;
        break;
    case MMPS_3GPRECD_EVENT_SEAMLESS:
        e = MMPD_3GPMGR_EVENT_SEAMLESS;
        break;
    case MMPS_3GPRECD_EVENT_MEDIA_ERROR:
        e = MMPD_3GPMGR_EVENT_MEDIA_ERROR;
        break;
    case MMPS_3GPRECD_EVENT_ENCODE_START:
        e = MMPD_3GPMGR_EVENT_ENCODE_START;
        break;
    case MMPS_3GPRECD_EVENT_ENCODE_STOP:
        e = MMPD_3GPMGR_EVENT_ENCODE_STOP;
        break;
	#if (DUALENC_SUPPORT == 1)
    case MMPS_3GPRECD_EVENT_DUALENCODE_START:
        e = MMPD_3GPMGR_EVENT_DUALENCODE_START;
        break;
    case MMPS_3GPRECD_EVENT_DUALENCODE_STOP:
        e = MMPD_3GPMGR_EVENT_DUALENCODE_STOP;
        break;
	#endif
    case MMPS_3GPRECD_EVENT_POSTPROCESS:
        e = MMPD_3GPMGR_EVENT_POSTPROCESS;
        break;
    case MMPS_3GPRECD_EVENT_BITSTREAM_DISCARD:
        e = MMPD_3GPMGR_EVENT_BITSTREAM_DISCARD;
        break;
    case MMPS_3GPRECD_EVENT_MEDIA_WRITE:
        e = MMPD_3GPRECD_EVENT_MEDIA_WRITE;
        break;
    case MMPS_3GPRECD_EVENT_STREAMCB:
    	e = MMPD_3GPRECD_EVENT_STREAMCB;
    	break;  
    case MMPS_3GPRECD_EVENT_EMERGFILE_FULL:
    	e = MMPD_3GPRECD_EVENT_EMERGFILE_FULL;
    	break;	
    case MMPS_3GPRECD_EVENT_UVCFILE_FULL:
    	e = MMPD_3GPRECD_EVENT_UVCFILE_FULL;
        break;
    case MMPS_3GPRECD_EVENT_PREGETTIME_CARDSLOW:
    	e = MMPD_3GPRECD_EVENT_PREGETTIME_CARDSLOW;
    	break;	
    case MMPS_3GPRECD_EVENT_RECDSTOP_CARDSLOW:
    	e = MMPD_3GPRECD_EVENT_RECDSTOP_CARDSLOW;
        break;
    case MMPS_3GPRECD_EVENT_COMPBUF_FREE_SPACE:
    	e = MMPD_3GPRECD_EVENT_COMPBUF_FREE_SPACE;
        break;
    case MMPS_3GPRECD_EVENT_APNEED_STOP_RECD:
    	e = MMPD_3GPRECD_EVENT_APNEED_STOP_RECD;
        break;
    case MMPS_3GPRECD_EVENT_DUALENC_FILE_FULL:
    	e = MMPD_3GPRECD_EVENT_DUALENC_FILE_FULL;
    	break;
    case MMPS_3GPRECD_EVENT_NONE:
    default:
        e = MMPD_3GPMGR_EVENT_NONE;
        break;
    }

    return MMPD_3GPMGR_RegisterCallback(e, CallBack);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetSkipCntThreshold
//  Description :
//------------------------------------------------------------------------------
/** @brief The function set the threshold of skip frame counts.

The function specified the threshold of skip frame counts to define what called
a slow media.

@param[in] threshold number of skip frames happen in 1 sec represents a slow media
@return It reports the status of the operation.
*/
MMP_ERR MMPS_3GPRECD_SetSkipCntThreshold(MMP_USHORT threshold)
{
    return MMPD_3GPMGR_SetSkipCntThreshold(threshold);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetVidRecdSkipModeParam
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Enalbe video record skip mode to set total skip count and continuous skip count.

 @param[in] ulTotalCount  - limitation of total skip count
 @param[in] ulContinCount - limitation of continuous skip count
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPS_3GPRECD_SetVidRecdSkipModeParam(MMP_ULONG ulTotalCount, MMP_ULONG ulContinCount)
{
	return MMPD_3GPMGR_SetVidRecdSkipModeParas(ulTotalCount, ulContinCount);
}

#if (SUPPORT_VUI_INFO)
//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetSEIShutterMode
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_SetSEIShutterMode(MMPS_SEI_SHUTTER_TYPE ulMode)
{
	return MMPD_3GPMGR_SetSEIShutterMode((MMP_ULONG)ulMode);
}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_Get3gpRecordingOffset
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get recording time offset.
 @param[out] ulTime Recording time offset in unit of ms.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_Get3gpRecordingOffset(MMP_ULONG *ulTime)
{
    return MMPD_3GPMGR_GetRecordingOffset(ulTime, MMP_3GPRECD_FILETYPE_VIDRECD);
}

#if 0
void ____VR_2nd_Record_Function____(){ruturn;} //dummy
#endif

#if (DUALENC_SUPPORT == 1)
//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetAllEncPreRecordTime
//  Description :
//------------------------------------------------------------------------------
/**
 @brief For dual encode. Get all preencode time and select the smaller.

 Get all encode stream preencode time and select the smaller.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPS_3GPRECD_GetAllEncPreRecordTime(MMP_ULONG ulPreCaptureMs, MMP_ULONG *ulRealPreCaptureMs)
{
    MMP_ULONG64         ullBitrate64 		= 0;//m_VidRecdModes.ulBitrate[0];
    MMP_ULONG64         ullTimeIncr64 		= 0;//m_VidRecdModes.ContainerFrameRate[0].usVopTimeIncrement;
    MMP_ULONG64         ullTimeResol64 		= 0;//m_VidRecdModes.ContainerFrameRate[0].usVopTimeIncrResol;
    MMP_ULONG           ulTargetFrameSize 	= 0;//(MMP_ULONG)((ullBitrate64 * ullTimeIncr64)/(ullTimeResol64 * 8));
    MMP_ULONG           ulExpectBufSize 	= 0;

    MMP_ULONG           ulFrontMaxPreEncMs 	= 0;
    #if (EMERGENTRECD_SUPPORT == 1)
    MMP_ULONG           ulMaxAudPreEncMs 	= 0;
    #endif
	MMP_ULONG           ulRearMaxPreEncMs 	= 0;
	MMP_ULONG           ulMaxPreEncMs 		= 0;

	// Check front cam record information.
    ullBitrate64 		= m_VidRecdModes.ulBitrate[0];
    ullTimeIncr64 		= m_VidRecdModes.ContainerFrameRate[0].usVopTimeIncrement;
    ullTimeResol64 		= m_VidRecdModes.ContainerFrameRate[0].usVopTimeIncrResol;
    ulTargetFrameSize 	= (MMP_ULONG)((ullBitrate64 * ullTimeIncr64)/(ullTimeResol64 * 8));	
    #if (EMERGENTRECD_SUPPORT == 1)
    ulMaxAudPreEncMs = (m_VidRecdConfigs.ulAudioCompBufSize)/(m_VidRecdModes.ulAudBitrate >> 3) * 1000 - 1000;
    #endif
    ulExpectBufSize = m_VidRecdConfigs.ulVideoCompBufSize - (ulTargetFrameSize * 3);

    #if (EMERGENTRECD_SUPPORT == 1)
    ulFrontMaxPreEncMs = (MMP_ULONG)(((MMP_ULONG64)ulExpectBufSize * 1000) / (m_VidRecdModes.ulBitrate[0] >> 3)) - 1000;
    #else
    ulFrontMaxPreEncMs = (MMP_ULONG)(((MMP_ULONG64)ulExpectBufSize * 900) / (m_VidRecdModes.ulBitrate[0] >> 3));
    #endif
	printc("front cam preencode: %x, %x, %x, %x\r\n", ulFrontMaxPreEncMs, ulExpectBufSize, m_VidRecdModes.ulBitrate[0], ulTargetFrameSize);
    #if (EMERGENTRECD_SUPPORT == 1)
    if (ulMaxPreEncMs > ulMaxAudPreEncMs) {
    	ulMaxPreEncMs = ulMaxAudPreEncMs;
    }
    #endif	
    ullBitrate64 		= m_VidRecdModes.ulBitrate[1];
    ullTimeIncr64 		= m_VidRecdModes.ContainerFrameRate[1].usVopTimeIncrement;
    ullTimeResol64 		= m_VidRecdModes.ContainerFrameRate[1].usVopTimeIncrResol;
    ulTargetFrameSize 	= (MMP_ULONG)((ullBitrate64 * ullTimeIncr64)/(ullTimeResol64 * 8));	
    ulExpectBufSize = m_VidRecdConfigs.ulVideoCompBufSize - (ulTargetFrameSize * 3);

    #if (EMERGENTRECD_SUPPORT == 1)
    ulRearMaxPreEncMs = (MMP_ULONG)(((MMP_ULONG64)ulExpectBufSize * 1000) / (ullBitrate64 >> 3)) - 1000;
    #else
    ulRearMaxPreEncMs = (MMP_ULONG)(((MMP_ULONG64)ulExpectBufSize * 900) / (ullBitrate64 >> 3));
    #endif
	printc("rear cam preencode: %x, %x, %x, %x\r\n", ulRearMaxPreEncMs, ulExpectBufSize, m_VidRecdModes.ulBitrate[1], ulTargetFrameSize);
	if (ulRearMaxPreEncMs > ulFrontMaxPreEncMs) {
		ulMaxPreEncMs = ulFrontMaxPreEncMs;
	}
	else {
		ulMaxPreEncMs = ulRearMaxPreEncMs;
	}
	 
    #if (EMERGENTRECD_SUPPORT == 1)
    if (ulMaxPreEncMs > ulMaxAudPreEncMs) {
    	ulMaxPreEncMs = ulMaxAudPreEncMs;
    }
    #endif	
    
    if (ulPreCaptureMs > ulMaxPreEncMs) {
        PRINTF("The pre-record duration %d is over preferred %d ms\r\n", ulPreCaptureMs, ulMaxPreEncMs);
    	ulPreCaptureMs = ulMaxPreEncMs;
    }
    *ulRealPreCaptureMs = ulPreCaptureMs;
    return MMP_ERR_NONE;

}
//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetDualH264SnrId
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_SetDualH264SnrId(MMP_UBYTE ubSnrSel)
{	
	m_ub2ndH264SnrId = ubSnrSel;
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetDualH264Status
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get status of firmware video recorder.
 @param[out] retstatus Firmware operation status.
 @return Error status.
 @note

 The parameter @a retstatus can be:
 - MMPS_FW_OPERATION_NONE Idle.
 - MMPS_FW_OPERATION_START Under recording.
 - MMPS_FW_OPERATION_PAUSE Pause recording.
 - MMPS_FW_OPERATION_RESUME Restart recording.
 - MMPS_FW_OPERATION_STOP Stop recording.
*/
MMP_ERR MMPS_3GPRECD_GetDualH264Status(MMPS_FW_OPERATION *retstatus)
{
    MMP_ERR _ret;
    MMPD_MP4VENC_FW_OP _status;

    if (m_VidDualID == INVALID_ENC_ID) {
    	_status = MMPD_MP4VENC_FW_OP_NONE;
    	_ret = MMP_ERR_NONE;
    }
    else {
    	_ret = MMPD_VIDENC_GetStatus(m_VidDualID, &_status);
    }

    switch (_status) {
    case MMPD_MP4VENC_FW_OP_NONE:
        *retstatus = MMPS_FW_OPERATION_NONE;
        break;
    case MMPD_MP4VENC_FW_OP_START:
        *retstatus = MMPS_FW_OPERATION_START;
        break;
    case MMPD_MP4VENC_FW_OP_PAUSE:
        *retstatus = MMPS_FW_OPERATION_PAUSE;
        break;
    case MMPD_MP4VENC_FW_OP_RESUME:
        *retstatus = MMPS_FW_OPERATION_RESUME;
        break;
    case MMPD_MP4VENC_FW_OP_STOP:
        *retstatus = MMPS_FW_OPERATION_STOP;
        break;
    case MMPD_MP4VENC_FW_OP_PREENCODE:
        *retstatus = MMPS_FW_OPERATION_PREENCODE;
        break;
    default:
        *retstatus = MMPS_FW_OPERATION_NONE;
        break;
    }

    return _ret;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetMulEncEnable
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_SetDualH264Enable(MMP_BOOL bEnable, MMP_ULONG type)
{	
	MMPD_3GPMGR_SetDualH264Enable(bEnable, type);
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetDualEncResIdx
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set encoded resolution.
 @param[in] usResol Resolution for record video.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetDualEncResIdx(MMP_USHORT usResol)
{
	m_VidRecdModes.usVideoEncResIdx[1] = usResol;
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetDualProfile
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set profile for video encode.
 @param[in] profile Visual profile for the specified encoder.
 @retval none.
*/
MMP_ERR MMPS_3GPRECD_SetDualProfile(MMPS_3GPRECD_PROFILE profile)
{
    m_VidRecdModes.VisualProfile[1] = profile;
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetDualCurBufMode
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set encoded video current buffer mode.
 @param[in] CurBufMode Video encode current buffer mode for record video.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetDualCurBufMode(MMPS_3GPRECD_CURBUF_MODE CurBufMode)
{
    MMPD_MP4VENC_FW_OP status_fw;

    if (m_VidDualID == INVALID_ENC_ID)
    	status_fw = MMPD_MP4VENC_FW_OP_NONE;
    else
    	MMPD_VIDENC_GetStatus(m_VidDualID, &status_fw);

    if ((status_fw == MMPD_MP4VENC_FW_OP_START) ||
        (status_fw == MMPD_MP4VENC_FW_OP_PAUSE) ||
        (status_fw == MMPD_MP4VENC_FW_OP_RESUME)) {
        RTNA_DBG_Str0("Can't change H264EncMode when recording\r\n");
        return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
    }

    /* Dual H264 Only support frame mode */
    if (CurBufMode == MMPS_3GPRECD_CURBUF_RT) {
        RTNA_DBG_Str0("Dual H264 Only support frame mode\r\n");
        return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
    }

    m_VidRecdModes.VidCurBufMode[1] = CurBufMode;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetDualFrameRatePara
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set frame rate for recorded video.
 @param[in] snr_fps Sensor input frame rate
 @param[in] enc_fps expected encode frame rate (normal/timelapse/slow motion)
 @param[in] container_fps Frame rate for playback
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetDualFrameRatePara(MMPS_3GPRECD_FRAMERATE *snr_fps,
                                  	  	  MMPS_3GPRECD_FRAMERATE *enc_fps,
										  MMPS_3GPRECD_FRAMERATE *container_fps)
{
    if ((snr_fps->usVopTimeIncrement == 0) ||
        (snr_fps->usVopTimeIncrResol == 0) ||
        (enc_fps->usVopTimeIncrement == 0) ||
        (enc_fps->usVopTimeIncrResol == 0) ||
        (container_fps->usVopTimeIncrement == 0) ||
        (container_fps->usVopTimeIncrResol == 0)) {
        return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
    }
    
    m_VidRecdModes.SnrInputFrameRate[1].usVopTimeIncrement = snr_fps->usVopTimeIncrement;
    m_VidRecdModes.SnrInputFrameRate[1].usVopTimeIncrResol = snr_fps->usVopTimeIncrResol;
    
    m_VidRecdModes.VideoEncFrameRate[1].usVopTimeIncrement = enc_fps->usVopTimeIncrement;
    m_VidRecdModes.VideoEncFrameRate[1].usVopTimeIncrResol = enc_fps->usVopTimeIncrResol;
    
    m_VidRecdModes.ContainerFrameRate[1].usVopTimeIncrement = container_fps->usVopTimeIncrement;
    m_VidRecdModes.ContainerFrameRate[1].usVopTimeIncrResol = container_fps->usVopTimeIncrResol;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetStreamBitrate
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_SetDualH264Bitrate(MMP_ULONG ulBitrate)
{
    MMPD_MP4VENC_FW_OP  status_vid;

    m_VidRecdModes.ulBitrate[1] = ulBitrate;
    //printc(FG_YELLOW("R_Bitrate=%d\r\n"), m_VidRecdModes.ulBitrate[1]); 
    if (m_VidDualID == INVALID_ENC_ID)
    	status_vid = MMPD_MP4VENC_FW_OP_NONE;
    else
    	MMPD_VIDENC_GetStatus(m_VidDualID, &status_vid);

    if ((status_vid == MMPD_MP4VENC_FW_OP_START) ||
        (status_vid == MMPD_MP4VENC_FW_OP_PREENCODE)) {
	    MMP_ULONG64 ullTimeIncr64 = m_VidRecdModes.VideoEncFrameRate[1].usVopTimeIncrement;
        MMP_ULONG64 ullTimeResol64 = m_VidRecdModes.VideoEncFrameRate[1].usVopTimeIncrResol;
    	MMP_ULONG   ulTargetFrameSize;

    	ulTargetFrameSize = (MMP_ULONG)((m_VidRecdModes.ulBitrate[1] * ullTimeIncr64)/(ullTimeResol64 * 8));
        MMPD_VIDENC_SetBitrate(m_VidDualID, ulTargetFrameSize, m_VidRecdModes.ulBitrate[1]);
    }
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetDualPFrameCount
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set P frame count of one cycle.
 @param[in] usFrameCount Count of P frame.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetDualPFrameCount(MMP_USHORT usFrameCount)
{
    m_VidRecdModes.usPFrameCount[1] = usFrameCount;
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetDualBFrameCount
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set B frame count per P frame.
 @param[in] usFrameCount Count of B frame.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetDualBFrameCount(MMP_USHORT usFrameCount)
{
    MMPD_MP4VENC_FW_OP status_vid;

    if (usFrameCount) {
        // For single current frame mode, B-frame enc is not supported
        return MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS;
    }

    if (m_VidDualID == INVALID_ENC_ID)
    	status_vid = MMPD_MP4VENC_FW_OP_NONE;
    else
    	MMPD_VIDENC_GetStatus(m_VidDualID, &status_vid);

    if (MMPD_VIDENC_GetFormat() != MMPD_MP4VENC_FORMAT_H264) {
	    return MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS;
    }

    if ((status_vid == MMPD_MP4VENC_FW_OP_NONE) ||
        (status_vid == MMPD_MP4VENC_FW_OP_STOP) ||
        (status_vid == MMPD_MP4VENC_FW_OP_PREENCODE)) {

        m_VidRecdModes.usBFrameCount[1] = usFrameCount;
    }
    else {
        printc(FG_RED("MMP_3GPRECD_ERR_GENERAL_ERROR %x\r\n"), status_vid);
        return MMP_3GPRECD_ERR_GENERAL_ERROR;
    }

    return MMP_ERR_NONE;
}

#if (SUPPORT_DEC_MJPEG_TO_ENC_H264 == 0)
//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetDualH264PipeConfig
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set video record pipe configuration.
 @param[in] *pInputBuf 		Pointer to HW used buffer.
 @param[in] usEncInputW 	The encode buffer width (for scaler out stage).
 @param[in] usEncInputH 	The encode buffer height (for scaler out stage).
 @param[in] ubEncId         Encode instance ID.
 @retval MMP_ERR_NONE Success.
*/
static MMP_ERR MMPS_3GPRECD_SetDualH264PipeConfig(  MMPD_MP4VENC_INPUT_BUF 	*pInputBuf,
											        MMP_USHORT			 	usEncInputW,
											        MMP_USHORT			 	usEncInputH,
											        MMP_USHORT              ubEncId)
{
    MMP_ULONG				ulScalInW, ulScalInH;
    MMP_SCAL_FIT_MODE		sFitMode;
    MMP_SCAL_FIT_RANGE  	fitrange;
    MMP_SCAL_GRAB_CTRL  	EncodeGrabctl;
    MMPD_FCTL_ATTR 			fctlAttr;
    MMP_USHORT				i;
	MMP_SCAL_FIT_RANGE 		sFitRangeBayer;
	MMP_SCAL_GRAB_CTRL		sGrabctlBayer;
	MMP_USHORT				usCurZoomStep = 0;

	/* Parameter Check */
	if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_INVALID) {
		return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
	}

	if (pInputBuf == NULL) {
		return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
	}

	/* Get the sensor parameters */
    MMPS_Sensor_GetCurPrevScalInputRes(m_ub2ndH264SnrId, &ulScalInW, &ulScalInH);

	MMPD_BayerScaler_GetZoomInfo(MMP_BAYER_SCAL_DOWN, &sFitRangeBayer, &sGrabctlBayer);

    if (m_sAhcVideoRecdInfo[1].bUserDefine) {
    	sFitMode = m_sAhcVideoRecdInfo[1].sFitMode;
    }
    else {
    	sFitMode = MMP_SCAL_FITMODE_OUT;
    }

	/* Initial zoom relative config */
	MMPS_3GPRECD_InitDigitalZoomParam(m_2ndRecFctlLink.scalerpath);

	MMPS_3GPRECD_RestoreDigitalZoomRange(m_2ndRecFctlLink.scalerpath);

    // Config Video Record Pipe
    fitrange.fitmode        = sFitMode;
    fitrange.scalerType 	= MMP_SCAL_TYPE_SCALER;

	if (MMP_IsVidPtzEnable()) {
        fitrange.ulInWidth	= sFitRangeBayer.ulOutWidth;
        fitrange.ulInHeight	= sFitRangeBayer.ulOutHeight;
	}
	else {
        fitrange.ulInWidth 	= ulScalInW;
        fitrange.ulInHeight	= ulScalInH;
	}

    fitrange.ulOutWidth     = usEncInputW;
    fitrange.ulOutHeight    = usEncInputH;

    fitrange.ulInGrabX 		= 1;
    fitrange.ulInGrabY 		= 1;
    fitrange.ulInGrabW 		= fitrange.ulInWidth;
    fitrange.ulInGrabH 		= fitrange.ulInHeight;

    MMPD_PTZ_InitPtzInfo(m_2ndRecFctlLink.scalerpath,
    					 fitrange.fitmode,
			             fitrange.ulInWidth, fitrange.ulInHeight,
			             fitrange.ulOutWidth, fitrange.ulOutHeight);

	// Be sync with preview path : TBD
	if (m_ub2ndH264SnrId == PRM_SENSOR)
	    MMPD_PTZ_GetCurPtzStep(m_PreviewFctlLink.scalerpath, NULL, &usCurZoomStep, NULL, NULL);
    #if (SUPPORT_DUAL_SNR)
    else
        MMPD_PTZ_GetCurPtzStep(m_2ndPrewFctlLink.scalerpath, NULL, &usCurZoomStep, NULL, NULL); 
    #endif

    if (usCurZoomStep > m_VidRecordZoomInfo.usMaxZoomSteps) {
        usCurZoomStep = m_VidRecordZoomInfo.usMaxZoomSteps;
    }

	MMPD_PTZ_CalculatePtzInfo(m_2ndRecFctlLink.scalerpath, usCurZoomStep, 0, 0);

    MMPD_PTZ_GetCurPtzInfo(m_2ndRecFctlLink.scalerpath, &fitrange, &EncodeGrabctl);
	
	if (MMP_IsVidPtzEnable()) {
		MMPD_PTZ_ReCalculateGrabRange(&fitrange, &EncodeGrabctl);
	}

    fctlAttr.colormode      = MMP_DISPLAY_COLOR_YUV420_INTERLEAVE;
    fctlAttr.fctllink       = m_2ndRecFctlLink;
    fctlAttr.fitrange       = fitrange;
    fctlAttr.grabctl        = EncodeGrabctl;
    if (m_ub2ndH264SnrId == SCD_SENSOR)
        fctlAttr.scalsrc    = MMP_SCAL_SOURCE_GRA;
    else
        fctlAttr.scalsrc    = MMP_SCAL_SOURCE_ISP;
    fctlAttr.bSetScalerSrc	= MMP_TRUE;
    fctlAttr.usBufCnt  		= pInputBuf->ulBufCnt;
    fctlAttr.bUseRotateDMA 	= MMP_FALSE;

    for (i = 0; i < fctlAttr.usBufCnt; i++) {
    	if (pInputBuf->ulY[i] != 0)
        	fctlAttr.ulBaseAddr[i] = pInputBuf->ulY[i];
        if (pInputBuf->ulU[i] != 0)
        	fctlAttr.ulBaseUAddr[i] = pInputBuf->ulU[i];
        if (pInputBuf->ulV[i] != 0)
        	fctlAttr.ulBaseVAddr[i] = pInputBuf->ulV[i];
    }

    if (m_VidRecdModes.VidCurBufMode[1] == MMPS_3GPRECD_CURBUF_RT) {
        RTNA_DBG_Str0("Dual H264 Only support frame mode\r\n");
    }
    else {
   		MMPD_IBC_SetH264RT_Enable(MMP_FALSE);
        m_bInitRecPipeConfig = MMP_TRUE;
        MMPD_Fctl_SetPipeAttrForH264FB(&fctlAttr);
    }

    MMPD_Fctl_LinkPipeToVideo(m_2ndRecFctlLink.ibcpipeID, ubEncId);

    // Tune MCI priority of encode pipe for frame based mode
    if (m_VidRecdModes.VidCurBufMode[1] == MMPS_3GPRECD_CURBUF_FRAME) {
        MMPD_VIDENC_TuneEncodePipeMCIPriority(m_2ndRecFctlLink.ibcpipeID);
    }

    return MMP_ERR_NONE;
}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_EnableDualH264Pipe
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Turn on and off record for video encode.

 @param[in] bEnable Enable and disable scaler path for video encode.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_EnableDualH264Pipe(MMP_UBYTE ubSnrSel, MMP_BOOL bEnable)
{
    if (m_bVidRecordActive[1] ^ bEnable)
    {
        if (bEnable) {
            MMPD_Fctl_EnablePreview(ubSnrSel, m_2ndRecFctlLink.ibcpipeID, bEnable, MMP_FALSE);
        }
        else {
            #if (SUPPORT_DEC_MJPEG_TO_ENC_H264)
            MMPD_Fctl_EnablePreview(ubSnrSel, m_2ndRecFctlLink.ibcpipeID, bEnable, MMP_FALSE);
            #else
            MMPD_Fctl_EnablePreview(ubSnrSel, m_2ndRecFctlLink.ibcpipeID, bEnable, MMP_TRUE);
            #endif
        }

    	m_bVidRecordActive[1] = bEnable;
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_DualEncPreRecord
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Pre start video recording for 2nd H264-Encode.

 It start record without enable file saving
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPS_3GPRECD_DualEncPreRecord(MMP_ULONG ulPreCaptureMs)
{
    MMP_ULONG           enc_id = 1;
    MMPD_MP4VENC_FW_OP  status_vid;
    MMP_ERR             status;
    MMP_ULONG           ulFps;
    MMP_ULONG           EncWidth, EncHeight;
    MMP_ULONG64         ullBitrate64 		= m_VidRecdModes.ulBitrate[1];
    MMP_ULONG64         ullTimeIncr64 		= m_VidRecdModes.ContainerFrameRate[1].usVopTimeIncrement;
    MMP_ULONG64         ullTimeResol64 		= m_VidRecdModes.ContainerFrameRate[1].usVopTimeIncrResol;
    MMP_ULONG           ulTargetFrameSize 	= (MMP_ULONG)((ullBitrate64 * ullTimeIncr64)/(ullTimeResol64 * 8));
    MMP_ULONG           ulExpectBufSize 	= 0;
    MMP_ULONG           ulMaxPreEncMs 		= 0;
    #if (EMERGENTRECD_SUPPORT == 1)
    MMP_ULONG           ulMaxAudPreEncMs 	= 0;
    #endif

    MMPS_3GPRECD_SetEncodeRes(1);

    EncWidth  = m_ulVREncodeW[1];
    EncHeight = m_ulVREncodeH[1];

    ulFps = (m_VidRecdModes.SnrInputFrameRate[1].usVopTimeIncrResol - 1 +
            m_VidRecdModes.SnrInputFrameRate[1].usVopTimeIncrement) /
            m_VidRecdModes.SnrInputFrameRate[1].usVopTimeIncrement;

    status = MMPD_VIDENC_CheckCapability(EncWidth, EncHeight, ulFps);

    if (status != MMP_ERR_NONE)
        return status;

    #if (EMERGENTRECD_SUPPORT == 1)
    ulMaxAudPreEncMs = (m_VidRecdConfigs.ulAudioCompBufSize)/(m_VidRecdModes.ulAudBitrate >> 3) * 1000 - 1000;
    #endif

    ulExpectBufSize = m_VidRecdConfigs.ulVideoCompBufSize - (ulTargetFrameSize * 3);

    #if (EMERGENTRECD_SUPPORT == 1)
    ulMaxPreEncMs = (MMP_ULONG)(((MMP_ULONG64)ulExpectBufSize * 1000) / (m_VidRecdModes.ulBitrate[1] >> 3)) - 1000;
    #else
    ulMaxPreEncMs = (MMP_ULONG)(((MMP_ULONG64)ulExpectBufSize * 900) / (m_VidRecdModes.ulBitrate[1] >> 3));
    #endif

    #if (EMERGENTRECD_SUPPORT == 1)
    if (ulMaxPreEncMs > ulMaxAudPreEncMs) {
    	ulMaxPreEncMs = ulMaxAudPreEncMs;
    }
    #endif  

    if (ulPreCaptureMs > ulMaxPreEncMs) {
        PRINTF("The pre-record duration %d is over preferred %d ms\r\n", ulPreCaptureMs, ulMaxPreEncMs);
    	ulPreCaptureMs = ulMaxPreEncMs;
    }

    if (m_VidDualID == INVALID_ENC_ID)
    	status_vid = MMPD_MP4VENC_FW_OP_NONE;
    else
    	MMPD_VIDENC_GetStatus(m_VidDualID, &status_vid);

    if ((status_vid == MMPD_MP4VENC_FW_OP_NONE) ||
        (status_vid == MMPD_MP4VENC_FW_OP_STOP)) {

        if (MMPD_VIDENC_GetFormat() == MMPD_MP4VENC_FORMAT_H264) {

            if (!MMPD_VIDENC_IsModuleInit(MMPD_VIDENC_MODULE_H264)) {
            	MMP_ERR	mmpstatus;

            	mmpstatus = MMPD_VIDENC_InitModule(MMPD_VIDENC_MODULE_H264);
                if (mmpstatus != MMP_ERR_NONE)
                	return mmpstatus;
            }

            MMPD_VIDENC_SetMemStart(m_ulVideoPreviewEndAddr);

            if (MMPS_3GPRECD_SetDualH264MemoryMap(  &enc_id,
                                                EncWidth,
                                                EncHeight,
                                                m_ulVidRecSramAddr,
                                                m_ulVidRecDramEndAddr))
            {
                PRINTF("Alloc mem for video pre-record failed\r\n");
                return MMP_3GPRECD_ERR_MEM_EXHAUSTED;
            }
        
            switch (m_VidRecdModes.VidCurBufMode[1]) {
            case MMPS_3GPRECD_CURBUF_FRAME:
                MMPD_VIDENC_SetCurBufMode(enc_id, MMPD_MP4VENC_CURBUF_FRAME);
                break;
            case MMPS_3GPRECD_CURBUF_RT:
                MMPD_VIDENC_SetCurBufMode(enc_id, MMPD_MP4VENC_CURBUF_RT);
                break;
            default:
                return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
            }

        	if (MMPS_3GPRECD_EnableDualH264Pipe(m_ub2ndH264SnrId, MMP_TRUE) != MMP_ERR_NONE) {
        		PRINTF("Enable Video Record: Fail\r\n");
                return MMP_3GPRECD_ERR_GENERAL_ERROR;
            }
        }
        else {
            return MMP_3GPRECD_ERR_GENERAL_ERROR;
        }

        // Encoder parameters
        if ((EncWidth == 1920) && (EncHeight == 1088)) {
            MMPS_3GPRECD_SetCropping(enc_id, 0, 8, 0, 0);
            MMPS_3GPRECD_SetPadding(enc_id, MMPS_3GPRECD_PADDING_REPEAT, 8);
        }
        else {
            MMPS_3GPRECD_SetCropping(enc_id, 0, 0, 0, 0);
            MMPS_3GPRECD_SetPadding(enc_id, MMPS_3GPRECD_PADDING_NONE, 0);
        }

        if (MMPD_VIDENC_SetResolution(enc_id, EncWidth, EncHeight) != MMP_ERR_NONE) {
            return MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS;
        }

        if (MMPD_VIDENC_SetProfile(enc_id, m_VidRecdModes.VisualProfile[1]) != MMP_ERR_NONE) {
            return MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS;
        }
        MMPD_VIDENC_SetEncodeMode();

        MMPD_3GPMGR_SetGOP(m_VidRecdModes.usPFrameCount[1], m_VidRecdModes.usBFrameCount[1], MMP_3GPRECD_FILETYPE_VIDRECD);
        MMPD_VIDENC_SetGOP(enc_id, m_VidRecdModes.usPFrameCount[1], m_VidRecdModes.usBFrameCount[1]);

        MMPD_VIDENC_SetQuality(enc_id, ulTargetFrameSize, (MMP_ULONG)ullBitrate64);

        // Sensor input frame rate
        MMPD_VIDENC_SetSnrFrameRate(enc_id,
        		                    m_VidRecdModes.SnrInputFrameRate[1].usVopTimeIncrement,
        		                    m_VidRecdModes.SnrInputFrameRate[1].usVopTimeIncrResol);
        // Encode frame rate
        MMPD_VIDENC_SetEncFrameRate(enc_id,
        		                    m_VidRecdModes.VideoEncFrameRate[1].usVopTimeIncrement,
				                    m_VidRecdModes.VideoEncFrameRate[1].usVopTimeIncrResol);
        // Container frame rate
        MMPD_3GPMGR_SetFrameRate(MMP_3GPRECD_FILETYPE_DUALENC,
        		                 m_VidRecdModes.ContainerFrameRate[1].usVopTimeIncrResol,
				                 m_VidRecdModes.ContainerFrameRate[1].usVopTimeIncrement);

        MMPD_VIDENC_EnableClock(MMP_TRUE);
        MMPD_3GPMGR_PreCapture(enc_id, ulPreCaptureMs);

        do {
            MMPD_VIDENC_GetStatus(enc_id, &status_vid);
        } while (status_vid != MMPD_MP4VENC_FW_OP_PREENCODE);
    }
    else {
        return MMP_3GPRECD_ERR_GENERAL_ERROR;
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_StartDualH264
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Start multi video encode.
*/
MMP_ERR MMPS_3GPRECD_StartDualH264(void)
{
    MMP_ULONG           enc_id = 0;
	MMPD_MP4VENC_FW_OP  status_vid;
    MMP_ERR             status;
    MMP_ULONG           EncWidth, EncHeight;
    MMP_ULONG64         ullBitrate64 = m_VidRecdModes.ulBitrate[1];
    MMP_ULONG           ulTargetFrameSize;
    MMP_ULONG           ulFps;
    #if (DUALENC_SUPPORT == 0)
    MMP_ULONG ulAvaSize;
	// Enlarge time out for encode with front cam and rear cam plug in
    MMP_ULONG           ulTimeout = 10000000; 
    #endif
    
    MMPS_3GPRECD_SetEncodeRes(1);

    EncWidth  = m_ulVREncodeW[1];
    EncHeight = m_ulVREncodeH[1];

    ulFps = (m_VidRecdModes.SnrInputFrameRate[1].usVopTimeIncrResol - 1 +
            m_VidRecdModes.SnrInputFrameRate[1].usVopTimeIncrement) /
            m_VidRecdModes.SnrInputFrameRate[1].usVopTimeIncrement;

    status = MMPD_VIDENC_CheckCapability(EncWidth, EncHeight, ulFps);

    if (status != MMP_ERR_NONE)
        return status;

    if (m_VidDualID == INVALID_ENC_ID)
    	status_vid = MMPD_MP4VENC_FW_OP_NONE;
    else
    	MMPD_VIDENC_GetStatus(m_VidDualID, &status_vid);

    if ((status_vid == MMPD_MP4VENC_FW_OP_NONE) ||
        (status_vid == MMPD_MP4VENC_FW_OP_STOP)) {

        if (MMPD_VIDENC_GetFormat() == MMPD_MP4VENC_FORMAT_H264) {

            if (!MMPD_VIDENC_IsModuleInit(MMPD_VIDENC_MODULE_H264)) {
            	MMP_ERR	mmpstatus;

            	mmpstatus = MMPD_VIDENC_InitModule(MMPD_VIDENC_MODULE_H264);
                if (mmpstatus != MMP_ERR_NONE)
                	return mmpstatus;
            }
            /* Now stop record will not stop preview (all pipes actually)
             * User can call MMPS_3GPRECD_StopRecord() and then call
             * MMPS_3GPRECD_StartRecord() to re-start recording without
             * stop/start preview. So we have to reset start address of mem
             * for encoder here.
             */
            MMPD_VIDENC_SetMemStart(m_ulVidRecDramEndAddr);

            if (MMPS_3GPRECD_SetDualH264MemoryMap(  &enc_id,
            		                                EncWidth,
                                                    EncHeight,
                                                    m_ulVidRecSramAddr,
											        m_ulVidRecDramEndAddr))
            {
                PRINTF("Alloc mem for 2nd record failed\r\n");
                return MMP_3GPRECD_ERR_MEM_EXHAUSTED;
            }

            // Set current buf mode
            switch (m_VidRecdModes.VidCurBufMode[1]) {
            case MMPS_3GPRECD_CURBUF_FRAME:
                MMPD_VIDENC_SetCurBufMode(enc_id, MMPD_MP4VENC_CURBUF_FRAME);
                break;
            default:
                return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
            }

            if (MMPS_3GPRECD_EnableDualH264Pipe(m_ub2ndH264SnrId, MMP_TRUE) != MMP_ERR_NONE) {
        		PRINTF("Enable Video Record: Fail\r\n");
                return MMP_3GPRECD_ERR_GENERAL_ERROR;
            }
        }
        else {
            return MMP_3GPRECD_ERR_GENERAL_ERROR;
        }

        // Encoder parameters
        if ((EncWidth == 1920) && (EncHeight == 1088)) {
            MMPS_3GPRECD_SetCropping(enc_id, 0, 8, 0, 0);
            if (m_VidRecdModes.VidCurBufMode[1] == MMPS_3GPRECD_CURBUF_RT)
                MMPS_3GPRECD_SetPadding(enc_id, MMPS_3GPRECD_PADDING_REPEAT, 8);
        }
        else {
            MMPS_3GPRECD_SetCropping(enc_id, 0, 0, 0, 0);
            MMPS_3GPRECD_SetPadding(enc_id, MMPS_3GPRECD_PADDING_NONE, 0);
        }
        
        if (MMPD_VIDENC_SetResolution(enc_id, EncWidth, EncHeight) != MMP_ERR_NONE) {
        	return MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS;
        }
        if (MMPD_VIDENC_SetProfile(enc_id, m_VidRecdModes.VisualProfile[1]) != MMP_ERR_NONE) {
        	return MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS;
        }
        
        MMPD_VIDENC_SetEncodeMode();

        MMPD_3GPMGR_SetGOP(m_VidRecdModes.usPFrameCount[1], m_VidRecdModes.usBFrameCount[1], MMP_3GPRECD_FILETYPE_DUALENC);
        MMPD_VIDENC_SetGOP(enc_id, m_VidRecdModes.usPFrameCount[1], m_VidRecdModes.usBFrameCount[1]);

        ulTargetFrameSize = MMPS_3GPRECD_TargetFrmSize(1);
        MMPD_VIDENC_SetQuality(enc_id, ulTargetFrameSize, (MMP_ULONG)ullBitrate64);

        // Sensor input frame rate
        MMPD_VIDENC_SetSnrFrameRate(enc_id,
        		                    m_VidRecdModes.SnrInputFrameRate[1].usVopTimeIncrement,
        		                    m_VidRecdModes.SnrInputFrameRate[1].usVopTimeIncrResol);
        // Encode frame rate
        MMPD_VIDENC_SetEncFrameRate(enc_id,
        		                    m_VidRecdModes.VideoEncFrameRate[1].usVopTimeIncrement,
				                    m_VidRecdModes.VideoEncFrameRate[1].usVopTimeIncrResol);
        // Container frame rate
        MMPD_3GPMGR_SetFrameRate(MMP_3GPRECD_FILETYPE_DUALENC,
        		                 m_VidRecdModes.ContainerFrameRate[1].usVopTimeIncrResol,
				                 m_VidRecdModes.ContainerFrameRate[1].usVopTimeIncrement);
    }

    if ((status_vid == MMPD_MP4VENC_FW_OP_NONE) ||
        (status_vid == MMPD_MP4VENC_FW_OP_STOP) ||
        (status_vid == MMPD_MP4VENC_FW_OP_PREENCODE)) {
		#if (DUALENC_SUPPORT == 0) // Change start capture to start all capture function for dual encode slow card issue.
		MMPD_3GPMGR_SetFileLimit(m_VidRecdModes.ulSizeLimit, m_VidRecdModes.ulReservedSpace, &ulAvaSize);

        if (MMPS_3GPRECD_GetExpectedRecordTime(ulAvaSize, m_VidRecdModes.ulBitrate[1], m_VidRecdModes.ulAudBitrate) <= 0) {
            return MMP_3GPRECD_ERR_MEM_EXHAUSTED;
        }
		
        if (status_vid != MMPD_MP4VENC_FW_OP_PREENCODE) {
            MMPD_VIDENC_EnableClock(MMP_TRUE);
        }

        if (MMPD_3GPMGR_StartCapture(m_VidDualID, MMP_3GPRECD_FILETYPE_DUALENC) != MMP_ERR_NONE) {
        	MMPD_VIDENC_EnableClock(MMP_FALSE);
        	return MMP_3GPRECD_ERR_GENERAL_ERROR;
        }

        do {
            MMPD_VIDENC_GetStatus(enc_id, &status_vid);
        } while ((status_vid != MMPD_MP4VENC_FW_OP_START) && (--ulTimeout) > 0);

        if (ulTimeout == 0) {
            RTNA_DBG_Str(0, "\r\nDual H264 NG...\r\n");
            return MMP_3GPRECD_ERR_GENERAL_ERROR;
        }
        
        if (status == MMP_FS_ERR_OPEN_FAIL) {
            return status;
        }

        if (status_vid == MMPD_MP4VENC_FW_OP_START) {
            return MMP_ERR_NONE;
        }
        else if (status == MMP_FS_ERR_OPEN_FAIL) {
            return MMP_3GPRECD_ERR_OPEN_FILE_FAILURE;
        }
        else {
            return MMP_ERR_NONE;
        }
#else
		// Just save file ID and encid and enc number.
	    if ((status_vid == MMPD_MP4VENC_FW_OP_NONE) ||
	        (status_vid == MMPD_MP4VENC_FW_OP_STOP)) {
	        gEncFileType[gbTotalEncNum] = MMP_3GPRECD_FILETYPE_DUALENC;
	        glEncID[gbTotalEncNum] = m_VidDualID;
	        gbTotalEncNum ++;
        }
		
		return MMP_ERR_NONE;		
#endif   
     
    }
    else {
        return MMP_3GPRECD_ERR_GENERAL_ERROR;
    }
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetDualH264MemoryMap
//  Description :
//------------------------------------------------------------------------------
/**
     @brief Set memory layout by different resolution and memory type for H264.

     Depends on encoded resolution and memory type to map buffers. It supports two types
     of memory, INTER(823) and STACK(821).
     @param[in] usEncW Encode width.
     @param[in] usEncH Encode height.
     @param[in] ulFBufAddr Available start address of frame buffer.
     @param[in] ulStackAddr Available start address of dram buffer.
     @retval MMP_ERR_NONE Success.
 */
MMP_ERR MMPS_3GPRECD_SetDualH264MemoryMap(MMP_ULONG *ulEncId, MMP_USHORT usEncW, MMP_USHORT usEncH, MMP_ULONG ulFBufAddr, MMP_ULONG ulStackAddr)
{
	const MMP_ULONG                 VideoRegStoreSize     = 60;
	const MMP_ULONG                 AVSyncBufSize         = 32;
	const MMP_ULONG                	VideoSizeTableSize    = 1024;
	const MMP_ULONG                 VideoTimeTableSize    = 1024;
	const MMP_ULONG                 AudioCompBufSize      = 0;
	const MMP_ULONG                 SPSSize               = 48;
	const MMP_ULONG                 PPSSize               = 16;
	const MMP_ULONG                 VideoCompBufSize      = m_VidRecdConfigs.ul2ndStreamVidCompBufSize;
	const MMP_ULONG                 AVRepackBufSize       = 512 * 1024;
	const MMP_ULONG                 SliceLengBufSize      = ((4 * ((usEncH>>4) + 2) + 63) >> 6) << 6;//align 64

	MMP_USHORT						i;
	MMP_ULONG						curaddr;
	MMP_ULONG						ulTmpBufSize;
	MMPD_MP4VENC_VIDEOBUF			videohwbuf;
	MMPD_H264ENC_HEADER_BUF         headerbuf;
	MMPD_3GPMGR_AVCOMPRESSEDBUFFER	mergercompbuf;
	MMPD_3GPMGR_REPACKBUF			repackmiscbuf;
	MMP_ULONG                       bufferEndAddr;
	MMP_ULONG                       MergerBufSize;
	MMP_ULONG                       ulEncFrameSize;
	#if (SUPPORT_VR_THUMBNAIL == 1)
	MMP_ULONG                       ThumbJpegSize;
	#endif

	MMPD_VIDENC_SetMemStart(ulStackAddr);
    m_ulVidRecEncodeAddr = curaddr = ulStackAddr;

	MergerBufSize = VideoRegStoreSize  + AVSyncBufSize + VideoSizeTableSize + VideoTimeTableSize + AudioCompBufSize;
	MergerBufSize = ((MergerBufSize + 31) >> 5) << 5;

	// Get Encode buffer config and size (YUV420)
	if (m_VidRecdModes.VidCurBufMode[1] == MMPS_3GPRECD_CURBUF_RT) {
		m_VidRecdInputBuf[1].ulBufCnt = 2;
	}
	else {
		#if (VR_SINGLE_CUR_FRAME == 1)
		m_VidRecdInputBuf[1].ulBufCnt = 1;
		#else
		m_VidRecdInputBuf[1].ulBufCnt = 2 + MAX_B_FRAME_NUMS;
		#endif
	}

	ulEncFrameSize = ALIGN32((usEncW * usEncH * 3) / 2);

	#if (CHIP == MCR_V2)
	if (((usEncW>>4)*(usEncH>>4)) > 34560) //4096x2160
		return MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS;
	#endif

	//*************************
	// reg buffer ......... 60
	// AV Sync ............ 32
	// Frame Table ........ 1k
	// Time Table ......... 1k
	// Audio BS ........... 16k
	//*************************
	{
		// set reg store buffer
		repackmiscbuf.ulVideoRegStoreAddr   = curaddr;
		curaddr                             += VideoRegStoreSize;

		// set av sync buffer
		repackmiscbuf.ulVideoEncSyncAddr    = curaddr;
		curaddr                             += AVSyncBufSize;

		// set frame table buffer, must be 512 byte alignment
		repackmiscbuf.ulVideoSizeTableAddr  = curaddr;
		repackmiscbuf.ulVideoSizeTableSize  = VideoSizeTableSize;
		curaddr                             += VideoSizeTableSize;

		// set time table buffer, must be 512 byte alignment
		repackmiscbuf.ulVideoTimeTableAddr  = curaddr;
		repackmiscbuf.ulVideoTimeTableSize  = VideoTimeTableSize;
		curaddr                             += VideoTimeTableSize;

		// set audio compressed buffer
		mergercompbuf.ulAudioCompBufStart   = curaddr;
		curaddr                             += AudioCompBufSize;
		mergercompbuf.ulAudioCompBufEnd     = curaddr;
	}

	if (MMPD_VIDENC_InitInstance(ulEncId, MMPD_VIDENC_MODULE_H264) != MMP_ERR_NONE)
	{
		RTNA_DBG_Str(0, "\r\n#DualEnc Err not available h264 instance\r\n");
		return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
	}

	m_VidDualID = *ulEncId;

	// Set YUV REF/GEN buffer
	curaddr = ALIGN32(curaddr);
	m_ulVidRecRefGenBufAddr = curaddr;

	// Allocate memory for Video Ref/Gen frames, curaddr will be advanced
	MMPD_H264ENC_CalculateRefBuf(usEncW, usEncH, &(videohwbuf.refgenbd), &curaddr);

	// Allocate memory for encoder buffer (I420/NV12)
	if (m_VidRecdModes.VidCurBufMode[1] == MMPS_3GPRECD_CURBUF_RT) {
		RTNA_DBG_Str0("Dual H264 Only support frame mode\r\n");
	}
	else {
		for (i = 0; i < m_VidRecdInputBuf[1].ulBufCnt; i++) {
			ulTmpBufSize = usEncW * usEncH;

			m_VidRecdInputBuf[1].ulY[i] = ALIGN32(curaddr);
			m_VidRecdInputBuf[1].ulU[i] = m_VidRecdInputBuf[1].ulY[i] + ulTmpBufSize;
			m_VidRecdInputBuf[1].ulV[i] = m_VidRecdInputBuf[1].ulU[i];
			curaddr += (ulTmpBufSize * 3)/2;
		}
	}

    #if (SUPPORT_DEC_MJPEG_TO_ENC_H264)
    MMPS_3GPRECD_InitDecMjpegToEncode(&m_VidRecdInputBuf[1], *ulEncId);
    #else
	MMPS_3GPRECD_SetDualH264PipeConfig(&m_VidRecdInputBuf[1], m_ulVREncodeBufW[1], m_ulVREncodeBufH[1], *ulEncId);
    #endif

	// Set Slice Length Buffer, align32
	videohwbuf.miscbuf.ulSliceLenBuf    = curaddr;
	curaddr                             = curaddr + SliceLengBufSize;

	// Set MV buffer, #MB/Frame * #MVs/MB * #byte/MV
	videohwbuf.miscbuf.ulMVBuf          = m_ulVidShareMvAddr;

	// Set h264 parameter set buf
	curaddr = ALIGN32(curaddr);
	headerbuf.ulSPSSize                 = SPSSize;
	headerbuf.ulPPSSize                 = PPSSize;
	
	//SPS
	headerbuf.ulSPSStart                = curaddr;
	curaddr                             += headerbuf.ulSPSSize;
    #if (SUPPORT_VUI_INFO)
    // Rebuild-SPS
    headerbuf.ulTmpSPSSize              = SPSSize+16;
    headerbuf.ulTmpSPSStart             = curaddr;
    curaddr                             += headerbuf.ulTmpSPSSize;
    #endif

	// PPS
	headerbuf.ulPPSStart                = curaddr;
	curaddr                             += headerbuf.ulPPSSize;

	// Set video compressed buffer, 32 byte alignment
	curaddr                             = ALIGN32(curaddr);
	videohwbuf.bsbuf.ulStart            = curaddr;
	mergercompbuf.ulVideoCompBufStart   = curaddr;
	curaddr                            += VideoCompBufSize;

	videohwbuf.bsbuf.ulEnd              = curaddr;
	mergercompbuf.ulVideoCompBufEnd     = curaddr;

	// Set av repack buffer
	repackmiscbuf.ulAvRepackStartAddr   = curaddr;
	repackmiscbuf.ulAvRepackSize        = AVRepackBufSize;
	curaddr                            += repackmiscbuf.ulAvRepackSize;

	MMPD_3GPMGR_SetRepackMiscBuf(&repackmiscbuf, MMP_3GPRECD_FILETYPE_DUALENC);
	MMPD_3GPMGR_SetEncodeCompBuf(&mergercompbuf, MMP_3GPRECD_FILETYPE_DUALENC);

	MMPD_H264ENC_SetStoreBufParams (repackmiscbuf.ulVideoRegStoreAddr);
	MMPD_H264ENC_SetHeaderBuf      (*ulEncId, &headerbuf);
	MMPD_H264ENC_SetBitstreamBuf   (*ulEncId, &(videohwbuf.bsbuf));
	MMPD_H264ENC_SetRefGenBound    (*ulEncId, &(videohwbuf.refgenbd));
	MMPD_H264ENC_SetMiscBuf        (*ulEncId, &(videohwbuf.miscbuf));

	MMPS_System_GetPreviewFrameStart(&bufferEndAddr);

    curaddr = ALIGN16(curaddr); // not necessary, but ALIGN4 is must
	m_ulVidRecDramEndAddr = curaddr;
    
	if ((curaddr + m_VidRecdConfigs.ulTailBufSize) > bufferEndAddr) {
		PRINTF("Can't use high speed videor\r\n");
		MMPD_3GPMGR_SetRecordTailSpeed(MMP_FALSE, 0, 0, MMP_3GPRECD_FILETYPE_DUALENC);
	}
	else {
		PRINTF("USE AIT high speed videor\r\n");
		MMPD_3GPMGR_SetRecordTailSpeed(MMP_TRUE, curaddr, m_VidRecdConfigs.ulTailBufSize, MMP_3GPRECD_FILETYPE_DUALENC);
		curaddr += m_VidRecdConfigs.ulTailBufSize;
		m_ulVidRecDramEndAddr = curaddr;
	}
    
	#if (SUPPORT_VR_THUMBNAIL)
	curaddr = ALIGN32(curaddr);
	ThumbJpegSize = ALIGN32((m_ulVRThumbWidth * m_ulVRThumbHeight) >> 1);
	MMPD_3GPMGR_SetThumbnailInfo(curaddr, ThumbJpegSize);
	curaddr += ThumbJpegSize;
	m_ulVidRecDramEndAddr = curaddr;
	#endif

    #if defined(ALL_FW)
    if (m_ulVidRecDramEndAddr > MMPS_System_GetMemHeapEnd()) {
        printc("\t= [HeapMemErr] 2nd Video record %x/%x\r\n",
                m_ulVidRecDramEndAddr,MMPS_System_GetMemHeapEnd());
        return MMP_3GPRECD_ERR_MEM_EXHAUSTED;
    }
    printc("End of 2nd video record buffers = 0x%X\r\n", m_ulVidRecDramEndAddr);
    #endif
    
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_StopDualH264
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Stop video recording and fill 3GP tail.

 It works after video start, pause and resume.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
 @retval MMP_3GPRECD_ERR_OPEN_FILE_FAILURE Open file failed.
 @retval MMP_3GPRECD_ERR_CLOSE_FILE_FAILURE Close file failed.
*/
MMP_ERR MMPS_3GPRECD_StopDualH264(void)
{
	MMP_ULONG          ulVidNumOpening = 0;
    MMPD_MP4VENC_FW_OP status_fw;

    MMPD_VIDENC_GetNumOpen(&ulVidNumOpening);
    if (m_VidDualID == INVALID_ENC_ID)
    	status_fw = MMPD_MP4VENC_FW_OP_NONE;
    else
    	MMPD_VIDENC_GetStatus(m_VidDualID, &status_fw);
    if ((status_fw == MMPD_MP4VENC_FW_OP_START) ||
        (status_fw == MMPD_MP4VENC_FW_OP_PAUSE) ||
        (status_fw == MMPD_MP4VENC_FW_OP_RESUME) ||
        (status_fw == MMPD_MP4VENC_FW_OP_STOP)) {
		
        MMPD_3GPMGR_StopCapture(m_VidDualID, MMP_3GPRECD_FILETYPE_DUALENC);

        do {
            MMPD_VIDENC_GetStatus(m_VidDualID, &status_fw);
        } while (status_fw != MMPD_MP4VENC_FW_OP_STOP);

    	if (ulVidNumOpening == 0) {
            if (MMPD_VIDENC_IsModuleInit(MMPF_VIDENC_MODULE_H264)) {
            	MMPD_VIDENC_DeinitModule(MMPF_VIDENC_MODULE_H264);
            }
    	}

        m_VidDualID = INVALID_ENC_ID;
		#if (DUALENC_SUPPORT)
		glEncID[1] = m_VidDualID;
		gbTotalEncNum -= 1;  
		#endif 
        MMPS_3GPRECD_EnableDualH264Pipe(m_ub2ndH264SnrId, MMP_FALSE);

        MMPD_VIDENC_EnableClock(MMP_FALSE);
    }
    else if (status_fw == MMPD_MP4VENC_FW_OP_PREENCODE) {

        MMPD_3GPMGR_StopCapture(m_VidDualID, MMP_3GPRECD_FILETYPE_DUALENC);

        do {
            MMPD_VIDENC_GetStatus(m_VidDualID, &status_fw);
        } while (status_fw != MMPD_MP4VENC_FW_OP_STOP);

        MMPD_VIDENC_GetNumOpen(&ulVidNumOpening);

    	if (ulVidNumOpening == 0) {
            if (MMPD_VIDENC_IsModuleInit(MMPF_VIDENC_MODULE_H264)) {
            	MMPD_VIDENC_DeinitModule(MMPF_VIDENC_MODULE_H264);
            }
    	}

        m_VidDualID = INVALID_ENC_ID;
		#if (DUALENC_SUPPORT)
		glEncID[1] = m_VidDualID;
		gbTotalEncNum -= 1;  
		#endif

        MMPS_3GPRECD_EnableDualH264Pipe(m_ub2ndH264SnrId, MMP_FALSE);

        MMPD_VIDENC_EnableClock(MMP_FALSE);
    }
	else if (status_fw == MMPD_MP4VENC_FW_OP_NONE) {
    	if (ulVidNumOpening > 0 && m_VidDualID != INVALID_ENC_ID) {
    		
    		MMPD_VIDENC_DeInitInstance(m_VidDualID);

    		m_VidDualID = INVALID_ENC_ID;
			#if (DUALENC_SUPPORT)
			glEncID[1] = m_VidDualID;
			gbTotalEncNum -= 1; 
			#endif
			if (ulVidNumOpening == 0) {
	            if (MMPD_VIDENC_IsModuleInit(MMPF_VIDENC_MODULE_H264)) {
	            	MMPD_VIDENC_DeinitModule(MMPF_VIDENC_MODULE_H264);
	            }
	    	}
    	}	
    }
    else {
        return MMP_3GPRECD_ERR_GENERAL_ERROR;
    }

    return MMP_ERR_NONE;
}

#endif
#if (DUALENC_SUPPORT)
//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_StartAllRecord
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Start all record with MGR functoin.
 @return Error status.
*/

MMP_ERR MMPS_3GPRECD_StartAllRecord(void) {
	MMP_ERR startCaptureStatus = MMP_ERR_NONE;
    MMPD_MP4VENC_FW_OP  status_vid;
	MMP_ULONG           ulAvaSize;  
	void                *pSeamlessCB;  
	// Enlarge time out for encode with front cam and rear cam plug in
    MMP_ULONG           ulTimeout = 10000000; 
    MMP_ERR             status;
    MMP_ULONG           status_tx;
	
	// Initial some front cam info.
    if (m_VidRecdID == INVALID_ENC_ID)
    	status_vid = MMPD_MP4VENC_FW_OP_NONE;
    else
    	MMPD_VIDENC_GetStatus(m_VidRecdID, &status_vid);
    if ((status_vid == MMPD_MP4VENC_FW_OP_NONE) ||
        (status_vid == MMPD_MP4VENC_FW_OP_STOP) ||
        (status_vid == MMPD_MP4VENC_FW_OP_PREENCODE)) {
        MMPD_3GPMGR_SetFileLimit(m_VidRecdModes.ulSizeLimit, m_VidRecdModes.ulReservedSpace, &ulAvaSize);

        if (MMPS_3GPRECD_GetExpectedRecordTime(ulAvaSize, m_VidRecdModes.ulBitrate[0], m_VidRecdModes.ulAudBitrate) <= 0) {
        	printc("All start front get except time error\r\n");
            return MMP_3GPRECD_ERR_MEM_EXHAUSTED;
        }
        else {
            MMPD_3GPMGR_SetTimeLimit(m_VidRecdModes.ulTimeLimitMs);
        }

        if (m_VidRecdConfigs.bSeamlessMode == MMP_TRUE) {
            // seamless callback must be registered if seamless mode is enabled.
            MMPD_3GPMGR_GetRegisteredCallback(MMPD_3GPMGR_EVENT_SEAMLESS, &pSeamlessCB);

            if (m_bSeamlessEnabled && (pSeamlessCB == NULL))
                return MMP_3GPRECD_ERR_GENERAL_ERROR;
        }

        if (status_vid != MMPD_MP4VENC_FW_OP_PREENCODE) {
            MMPD_VIDENC_EnableClock(MMP_TRUE);
        }
	}	
	if (gbTotalEncNum == 2) {
	    if (m_VidDualID == INVALID_ENC_ID)
	    	status_vid = MMPD_MP4VENC_FW_OP_NONE;
	    else
	    	MMPD_VIDENC_GetStatus(m_VidDualID, &status_vid);
	    if ((status_vid == MMPD_MP4VENC_FW_OP_NONE) ||
	        (status_vid == MMPD_MP4VENC_FW_OP_STOP) ||
	        (status_vid == MMPD_MP4VENC_FW_OP_PREENCODE)) {
			MMPD_3GPMGR_SetFileLimit(m_VidRecdModes.ulSizeLimit, m_VidRecdModes.ulReservedSpace, &ulAvaSize);

	        if (MMPS_3GPRECD_GetExpectedRecordTime(ulAvaSize, m_VidRecdModes.ulBitrate[1], m_VidRecdModes.ulAudBitrate) <= 0) {
	        	printc("All start dual get except time error\r\n");
	            return MMP_3GPRECD_ERR_MEM_EXHAUSTED;
	        }
			
	        if (status_vid != MMPD_MP4VENC_FW_OP_PREENCODE) {
	            MMPD_VIDENC_EnableClock(MMP_TRUE);
	        }
		}	    	
	}	
	
	#if (DUALENC_SUPPORT)
	startCaptureStatus = MMPD_3GPMGR_StartAllCapture(gbTotalEncNum, &glEncID[0]);
	#endif
	
    do {
        //MMPD_3GPMGR_GetStatus(&status, &status_tx);
        MMPD_VIDENC_GetMergerStatus(&status, &status_tx);
        MMPD_VIDENC_GetStatus(glEncID[0], &status_vid);
        if (status_vid == MMPD_MP4VENC_FW_OP_STOP) {
        	printc("All start meet slow card Fail\r\n");
        	return MMP_3GPRECD_ERR_STOPRECD_SLOWCARD;
        }
    } while ((status_vid != MMPD_MP4VENC_FW_OP_START) && (status != MMP_FS_ERR_OPEN_FAIL));

    if (status == MMP_FS_ERR_OPEN_FAIL) {
    	printc("All start file open Fail\r\n");
        return status;
    }

    if (status_vid == MMPD_MP4VENC_FW_OP_START) {
        return MMP_ERR_NONE;
    }
    else if (status == MMP_FS_ERR_OPEN_FAIL) {
        return MMP_3GPRECD_ERR_OPEN_FILE_FAILURE;
    }
    else {
    	
    }
    if (gbTotalEncNum == 2) {
        do {
	        MMPD_VIDENC_GetMergerStatus(&status, &status_tx);
            MMPD_VIDENC_GetStatus(glEncID[1], &status_vid);
	        if (status_vid == MMPD_MP4VENC_FW_OP_STOP) {
	        	printc("All start meet slow card Fail\r\n");
	        	return MMP_3GPRECD_ERR_STOPRECD_SLOWCARD;
	        }
        } while ((status_vid != MMPD_MP4VENC_FW_OP_START) && (status != MMP_FS_ERR_OPEN_FAIL) && (--ulTimeout) > 0);

        if (ulTimeout == 0) {
            printc("\r\nDual H264 NG...\r\n");
            return MMP_3GPRECD_ERR_GENERAL_ERROR;
        }
        
        if (status == MMP_FS_ERR_OPEN_FAIL) {
	    	printc("All start DUAL file open Fail\r\n");    
            return status;
        }

        if (status_vid == MMPD_MP4VENC_FW_OP_START) {
            return MMP_ERR_NONE;
        }
        else if (status == MMP_FS_ERR_OPEN_FAIL) {
            return MMP_3GPRECD_ERR_OPEN_FILE_FAILURE;
        }
        else {
        }
    }
    return MMP_ERR_NONE;
	
}
#endif
#if 0
void ____VR_Wifi_Stream_Function____(){ruturn;} //dummy
#endif

#if (SUPPORT_VR_WIFI_STREAM == 1)
//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetWifiStreamStatus
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get status of firmware video recorder.
 @param[out] retstatus Firmware operation status.
 @return Error status.
 @note

 The parameter @a retstatus can be:
 - MMPS_FW_OPERATION_NONE Idle.
 - MMPS_FW_OPERATION_START Under recording.
 - MMPS_FW_OPERATION_PAUSE Pause recording.
 - MMPS_FW_OPERATION_RESUME Restart recording.
 - MMPS_FW_OPERATION_STOP Stop recording.
*/
MMP_ERR MMPS_3GPRECD_GetWifiStreamStatus(MMP_UBYTE ubSnrSel, MMPS_FW_OPERATION *retstatus)
{
    MMP_ERR			   _ret;
    MMP_ULONG		   ulwifiID;
    MMPD_MP4VENC_FW_OP _status;

    if (ubSnrSel == PRM_SENSOR)
    	ulwifiID = m_VidWifiFrontID;
    else if (ubSnrSel == SCD_SENSOR)
    	ulwifiID = m_VidWifiFrontID;
    else
    	return MMP_WIFI_ERR_INVALID_PARAMETERS;

    if (ulwifiID == INVALID_ENC_ID) {
    	_status = MMPD_MP4VENC_FW_OP_NONE;
    	_ret = MMP_ERR_NONE;
    }
    else {
    	_ret = MMPD_VIDENC_GetStatus(ulwifiID, &_status);
    }

    switch (_status) {
    case MMPD_MP4VENC_FW_OP_NONE:
        *retstatus = MMPS_FW_OPERATION_NONE;
        break;
    case MMPD_MP4VENC_FW_OP_START:
        *retstatus = MMPS_FW_OPERATION_START;
        break;
    case MMPD_MP4VENC_FW_OP_PAUSE:
        *retstatus = MMPS_FW_OPERATION_PAUSE;
        break;
    case MMPD_MP4VENC_FW_OP_RESUME:
        *retstatus = MMPS_FW_OPERATION_RESUME;
        break;
    case MMPD_MP4VENC_FW_OP_STOP:
        *retstatus = MMPS_FW_OPERATION_STOP;
        break;
    case MMPD_MP4VENC_FW_OP_PREENCODE:
        *retstatus = MMPS_FW_OPERATION_PREENCODE;
        break;
    default:
        *retstatus = MMPS_FW_OPERATION_NONE;
        break;
    }

    return _ret;
}
#endif

#if 0
void ____VR_Zoom_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetZoomConfig
//  Description :
//------------------------------------------------------------------------------
/** @brief The function set zoom config

The function set zoom config, total step and max zoom multiplier

@param[in] usMaxSteps 	max zoom steps.
@param[in] usMaxRatio 	max zoom ratio.
@return It reports the status of the operation.
*/
MMP_ERR MMPS_3GPRECD_SetZoomConfig(MMP_USHORT usMaxSteps, MMP_USHORT usMaxRatio)
{
    m_VidPreviewZoomInfo.usMaxZoomSteps	= usMaxSteps;
    m_VidPreviewZoomInfo.usMaxZoomRatio	= usMaxRatio;
    
    m_VidRecordZoomInfo.usMaxZoomSteps	= usMaxSteps;
    m_VidRecordZoomInfo.usMaxZoomRatio	= usMaxRatio;
    
    m_bAhcConfigVideoRZoom = MMP_TRUE;
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_InitDigitalZoomParam
//  Description :
//------------------------------------------------------------------------------
static MMP_ERR MMPS_3GPRECD_InitDigitalZoomParam(MMP_UBYTE ubPipe)
{
	if (ubPipe == m_PreviewFctlLink.scalerpath) 
	{
		// Initial preview zoom parameters
	    m_VidPreviewZoomInfo.scalerpath 	= m_PreviewFctlLink.scalerpath;
	    m_VidPreviewZoomInfo.usCurZoomStep 	= 0;
	    m_VidPreviewZoomInfo.usMaxZoomRatio	= gsVidPtzCfg.usMaxZoomRatio;
		m_VidPreviewZoomInfo.usMaxZoomSteps	= gsVidPtzCfg.usMaxZoomSteps;
		m_VidPreviewZoomInfo.sMaxPanSteps	= gsVidPtzCfg.sMaxPanSteps;
		m_VidPreviewZoomInfo.sMinPanSteps	= gsVidPtzCfg.sMinPanSteps;
		m_VidPreviewZoomInfo.sMaxTiltSteps	= gsVidPtzCfg.sMaxTiltSteps;
		m_VidPreviewZoomInfo.sMinTiltSteps	= gsVidPtzCfg.sMinTiltSteps;

	    MMPD_PTZ_InitPtzRange(m_VidPreviewZoomInfo.scalerpath,
				              m_VidPreviewZoomInfo.usMaxZoomRatio,
				              m_VidPreviewZoomInfo.usMaxZoomSteps,
				              m_VidPreviewZoomInfo.sMaxPanSteps,
				              m_VidPreviewZoomInfo.sMinPanSteps,
				              m_VidPreviewZoomInfo.sMaxTiltSteps,
				              m_VidPreviewZoomInfo.sMinTiltSteps);

	    MMPD_PTZ_SetDigitalZoom(m_VidPreviewZoomInfo.scalerpath, MMP_PTZ_ZOOMSTOP, MMP_TRUE);
	}
	else if (ubPipe == m_RecordFctlLink.scalerpath)
	{
	    // Initail video zoom parameters
        m_VidRecordZoomInfo.scalerpath      = m_RecordFctlLink.scalerpath;
        m_VidRecordZoomInfo.usCurZoomStep   = 0;
	    m_VidRecordZoomInfo.usMaxZoomRatio	= gsVidPtzCfg.usMaxZoomRatio;
		m_VidRecordZoomInfo.usMaxZoomSteps	= gsVidPtzCfg.usMaxZoomSteps;
		m_VidRecordZoomInfo.sMaxPanSteps	= gsVidPtzCfg.sMaxPanSteps;
		m_VidRecordZoomInfo.sMinPanSteps	= gsVidPtzCfg.sMinPanSteps;
		m_VidRecordZoomInfo.sMaxTiltSteps	= gsVidPtzCfg.sMaxTiltSteps;
		m_VidRecordZoomInfo.sMinTiltSteps	= gsVidPtzCfg.sMinTiltSteps;

		MMPD_PTZ_InitPtzRange(m_VidRecordZoomInfo.scalerpath,
				              m_VidRecordZoomInfo.usMaxZoomRatio,
				              m_VidRecordZoomInfo.usMaxZoomSteps,
				              m_VidRecordZoomInfo.sMaxPanSteps,
				              m_VidRecordZoomInfo.sMinPanSteps,
				              m_VidRecordZoomInfo.sMaxTiltSteps,
				              m_VidRecordZoomInfo.sMinTiltSteps);

	    MMPD_PTZ_SetDigitalZoom(m_VidRecordZoomInfo.scalerpath, MMP_PTZ_ZOOMSTOP, MMP_TRUE);
	}
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_RestoreDigitalZoomRange
//  Description :
//------------------------------------------------------------------------------
static MMP_ERR MMPS_3GPRECD_RestoreDigitalZoomRange(MMP_UBYTE ubPipe)
{
    if (m_usVidStaticZoomIndex) 
    {
		if (ubPipe == m_PreviewFctlLink.scalerpath)
		{
	        m_VidPreviewZoomInfo.usCurZoomStep = m_usVidStaticZoomIndex;

			MMPD_PTZ_SetTargetPtzStep(m_VidPreviewZoomInfo.scalerpath, 
	    							  MMP_PTZ_ZOOM_INC_IN, 
	    							  m_VidPreviewZoomInfo.usCurZoomStep, 0, 0);
			
	        MMPD_PTZ_SetDigitalZoomOP(m_ubVidRecModeSnrId,
	                                  m_VidPreviewZoomInfo.scalerpath,
	                                  MMP_PTZ_ZOOMIN,
	                                  MMP_TRUE);
		}
		else if (ubPipe == m_RecordFctlLink.scalerpath)
		{
	        m_VidRecordZoomInfo.usCurZoomStep = m_usVidStaticZoomIndex;

			MMPD_PTZ_SetTargetPtzStep(m_VidRecordZoomInfo.scalerpath, 
	    							  MMP_PTZ_ZOOM_INC_IN, 
	    							  m_VidRecordZoomInfo.usCurZoomStep, 0, 0);

	        MMPD_PTZ_SetDigitalZoomOP(m_ubVidRecModeSnrId,
	                                  m_VidRecordZoomInfo.scalerpath,
	                                  MMP_PTZ_ZOOMIN,
	                                  MMP_TRUE);
		} 

        // Reset zoom index
 		m_usVidStaticZoomIndex = 0;
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetCurZoomStep
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_GetCurZoomStep(MMP_UBYTE ubPipe, MMP_USHORT *usCurZoomStep)
{
    MMP_USHORT	usCurStep;
    MMP_ERR     status = MMP_ERR_NONE;
	
	if (ubPipe == m_PreviewFctlLink.scalerpath) 
	{
	    status = MMPD_PTZ_GetCurPtzStep(m_VidPreviewZoomInfo.scalerpath,
	                                    NULL, &usCurStep, NULL, NULL);

	    *usCurZoomStep = usCurStep;

	    if (m_bVidPreviewActive[0]) {
	        m_VidPreviewZoomInfo.usCurZoomStep = usCurStep;
	    }
    }
	else if (ubPipe == m_RecordFctlLink.scalerpath) 
	{
	    status = MMPD_PTZ_GetCurPtzStep(m_VidRecordZoomInfo.scalerpath,
	                                    NULL, &usCurStep, NULL, NULL);
	    
	    *usCurZoomStep = usCurStep;

	    if (m_bVidRecordActive[0]) {
	        m_VidRecordZoomInfo.usCurZoomStep = usCurStep;
	    }
    }

	return status;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetCurZoomStatus
//  Description :
//------------------------------------------------------------------------------
MMP_UBYTE MMPS_3GPRECD_GetCurZoomStatus(void)
{
	return MMPD_PTZ_GetCurPtzStatus();
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetPreviewZoom
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_SetPreviewZoom(MMPS_3GPRECD_ZOOM_PATH sPath, MMP_PTZ_ZOOM_DIR sZoomDir, MMP_USHORT usCurZoomStep)
{
    MMP_BOOL 			bComplete[2] = {MMP_FALSE, MMP_FALSE};
	MMP_PTZ_ZOOM_INC  	sZoomInc;
	MMP_UBYTE			ubPipe;
	MMP_USHORT			usMaxStep = 0;
	
    if (m_bVidPreviewActive[0] != MMP_TRUE) {
        m_usVidStaticZoomIndex = usCurZoomStep;
        return MMP_ERR_NONE;
    }

	/* Decide the zoom step increment */
	if (sZoomDir == MMP_PTZ_ZOOMIN) {
        sZoomInc = MMP_PTZ_ZOOM_INC_IN;
    }
    else if (sZoomDir == MMP_PTZ_ZOOMOUT) {
        sZoomInc = MMP_PTZ_ZOOM_INC_OUT;
    }
	
	if ((sPath == MMPS_3GPRECD_ZOOM_PATH_PREV && m_bVidPreviewActive[0]) || 
		(sPath == MMPS_3GPRECD_ZOOM_PATH_RECD && m_bVidRecordActive[0]))
	{
		if (sPath == MMPS_3GPRECD_ZOOM_PATH_PREV) {
			ubPipe 		= m_VidPreviewZoomInfo.scalerpath;
			usMaxStep 	= m_VidPreviewZoomInfo.usMaxZoomSteps;
		}
		else {
			ubPipe 		= m_VidRecordZoomInfo.scalerpath;
			usMaxStep 	= m_VidRecordZoomInfo.usMaxZoomSteps;
		}

	    MMPD_PTZ_CheckZoomComplete(ubPipe, bComplete);
	    
	    if (!bComplete[0]) {

	        MMPD_PTZ_SetDigitalZoom(ubPipe, MMP_PTZ_ZOOMSTOP, MMP_TRUE);

	        do {
	            MMPD_PTZ_CheckZoomComplete(ubPipe, bComplete);
	        } while (!bComplete[0]);
	    }

		if (sZoomDir == MMP_PTZ_ZOOMIN) {
			MMPD_PTZ_SetTargetPtzStep(ubPipe, 
		    						  sZoomInc, 
		    						  usMaxStep, 0, 0);
		}
		else if (sZoomDir == MMP_PTZ_ZOOMOUT) {
			MMPD_PTZ_SetTargetPtzStep(ubPipe, 
		    						  sZoomInc, 
		    						  0, 0, 0);
		}

	    MMPD_PTZ_SetDigitalZoom(ubPipe, sZoomDir, MMP_TRUE);
	}
	else if (sPath == MMPS_3GPRECD_ZOOM_PATH_BOTH && m_bVidPreviewActive[0] && m_bVidRecordActive[0])
	{
	    MMPD_PTZ_CheckZoomComplete(m_VidPreviewZoomInfo.scalerpath, bComplete);
	    MMPD_PTZ_CheckZoomComplete(m_VidRecordZoomInfo.scalerpath, bComplete + 1);

	    if (!bComplete[0]) {

	        MMPD_PTZ_SetDigitalZoom(m_VidPreviewZoomInfo.scalerpath, MMP_PTZ_ZOOMSTOP, MMP_TRUE);
			MMPD_PTZ_SetDigitalZoom(m_VidRecordZoomInfo.scalerpath, MMP_PTZ_ZOOMSTOP, MMP_TRUE);

	        do {
	            MMPD_PTZ_CheckZoomComplete(m_VidPreviewZoomInfo.scalerpath, bComplete);
	            MMPD_PTZ_CheckZoomComplete(m_VidRecordZoomInfo.scalerpath, bComplete + 1);
	        } while (!bComplete[0] && !bComplete[1]);
	    }

		if (sZoomDir == MMP_PTZ_ZOOMIN) {
			MMPD_PTZ_SetTargetPtzStep(m_VidPreviewZoomInfo.scalerpath, 
		    						  sZoomInc, 
		    						  m_VidPreviewZoomInfo.usMaxZoomSteps, 0, 0);
		}
		else if (sZoomDir == MMP_PTZ_ZOOMOUT) {
			MMPD_PTZ_SetTargetPtzStep(m_VidPreviewZoomInfo.scalerpath, 
		    						  sZoomInc, 
		    						  0, 0, 0);
		}

	    MMPD_PTZ_SetDigitalZoom(m_VidPreviewZoomInfo.scalerpath, sZoomDir, MMP_TRUE);

		if (sZoomDir == MMP_PTZ_ZOOMIN) {
			MMPD_PTZ_SetTargetPtzStep(m_VidRecordZoomInfo.scalerpath, 
		    						  sZoomInc, 
		    						  m_VidRecordZoomInfo.usMaxZoomSteps, 0, 0);
		}
		else if (sZoomDir == MMP_PTZ_ZOOMOUT) {
			MMPD_PTZ_SetTargetPtzStep(m_VidRecordZoomInfo.scalerpath, 
		    						  sZoomInc, 
		    						  0, 0, 0);
		}

	    MMPD_PTZ_SetDigitalZoom(m_VidRecordZoomInfo.scalerpath, sZoomDir, MMP_TRUE);
	}

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_StopAllPipeZoom
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_StopAllPipeZoom(void)
{
	MMP_BOOL bComplete[2] = {MMP_FALSE, MMP_FALSE};

	return MMP_ERR_NONE; //TBD

	if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_2PIPE) 
	{
        MMPD_PTZ_CheckZoomComplete(m_VidPreviewZoomInfo.scalerpath, bComplete);
        MMPD_PTZ_CheckZoomComplete(m_VidRecordZoomInfo.scalerpath, bComplete + 1);
        
        if ((!bComplete[0]) || (!bComplete[1])) {
            MMPD_PTZ_SetDigitalZoom(m_VidPreviewZoomInfo.scalerpath, MMP_PTZ_ZOOMSTOP, MMP_TRUE);
            MMPD_PTZ_SetDigitalZoom(m_VidRecordZoomInfo.scalerpath, MMP_PTZ_ZOOMSTOP, MMP_TRUE);

            do {
                MMPD_PTZ_CheckZoomComplete(m_VidPreviewZoomInfo.scalerpath, bComplete);
                MMPD_PTZ_CheckZoomComplete(m_VidRecordZoomInfo.scalerpath, bComplete + 1);
            } while((!bComplete[0]) || (!bComplete[1]));
        }
	}
    #if (SUPPORT_LDC_RECD)
    else if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_LB_SINGLE || 
    		 m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_LB_MULTI	||
    		 m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_MULTISLICE) 
    {
        MMPD_PTZ_CheckZoomComplete(m_VidPreviewZoomInfo.scalerpath, bComplete);
        MMPD_PTZ_CheckZoomComplete(m_VidRecordZoomInfo.scalerpath, bComplete + 1);
        
        if ((!bComplete[0]) || (!bComplete[1])) {
            MMPD_PTZ_SetDigitalZoom(m_VidPreviewZoomInfo.scalerpath, MMP_PTZ_ZOOMSTOP, MMP_TRUE);
            MMPD_PTZ_SetDigitalZoom(m_VidRecordZoomInfo.scalerpath, MMP_PTZ_ZOOMSTOP, MMP_TRUE);

            do {
                MMPD_PTZ_CheckZoomComplete(m_VidPreviewZoomInfo.scalerpath, bComplete);
                MMPD_PTZ_CheckZoomComplete(m_VidRecordZoomInfo.scalerpath, bComplete + 1);
            } while((!bComplete[0]) || (!bComplete[1]));
        }
	}
    #endif

	return MMP_ERR_NONE;
}

#if 0
void ____VR_LDC_Function____(){ruturn;} //dummy
#endif

#if (SUPPORT_LDC_RECD)
//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetLdcRunMode
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_SetLdcRunMode(MMP_UBYTE ubRunMode)
{
	MMPF_LDC_SetRunMode(ubRunMode);
	
	if (ubRunMode == MMP_LDC_RUN_MODE_MULTI_RUN) {
		MMPF_LDC_MultiRunSetLoopBackCount(MMPF_LDC_MultiRunGetMaxLoopBackCount());
	}
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetLdcResMode
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_SetLdcResMode(MMP_UBYTE ubResMode, MMP_UBYTE ubFpsMode)
{
	switch(ubResMode)
	{
		case MMP_LDC_RES_MODE_FHD:
		    if (m_VidRecdConfigs.previewpath[0] == MMP_3GP_PATH_LDC_MULTISLICE) {
			    m_ulLdcMaxSrcWidth 	= 2304; //TBD
			    m_ulLdcMaxSrcHeight	= 1296;
		    }
		    else {
			    m_ulLdcMaxSrcWidth 	= 1920;
			    m_ulLdcMaxSrcHeight	= 1080;
			}
			m_ulLdcMaxOutWidth 	= 1920;
			m_ulLdcMaxOutHeight	= 1080;
		break;
		case MMP_LDC_RES_MODE_HD:
			m_ulLdcMaxSrcWidth 	= 1280;
			m_ulLdcMaxSrcHeight	= 720;
			m_ulLdcMaxOutWidth 	= 1280;
			m_ulLdcMaxOutHeight	= 720;
		break;
		case MMP_LDC_RES_MODE_WVGA:
			m_ulLdcMaxSrcWidth 	= 848;
			m_ulLdcMaxSrcHeight	= 480;
			m_ulLdcMaxOutWidth 	= 848;
			m_ulLdcMaxOutHeight	= 480;
		break;
		default:
			return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
		break;
	}
	
	MMPF_LDC_SetResMode(ubResMode);
	MMPF_LDC_SetFpsMode(ubFpsMode);
	
	MMPF_LDC_SetFrameRes(m_ulLdcMaxSrcWidth, m_ulLdcMaxSrcHeight,
						 m_ulLdcMaxOutWidth, m_ulLdcMaxOutHeight);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetLdcMaxRes
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_GetLdcMaxRes(MMP_ULONG *pulMaxW, MMP_ULONG *pulMaxH)
{
    *pulMaxW = m_ulLdcMaxSrcWidth;
	*pulMaxH = m_ulLdcMaxSrcHeight;

    return MMP_ERR_NONE;
}
#endif

#if 0
void ____VR_Sticker_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetSticker
//  Description :
//------------------------------------------------------------------------------
/** @brief The function sets the attributes to the video sticker

The function sets the attributes to the specified video sticker with 
its sticker ID.

  @param[in] stickerID the sticker ID
  @param[in] stickerAtrribute the sticker attributes
  @return It reports the status of the operation.
*/
MMP_ERR MMPS_3GPRECD_SetSticker(MMP_STICKER_ATTR *pStickerAtrr)
{
	MMP_ERR err = MMP_ERR_NONE;
    if (pStickerAtrr != NULL) {
        err = MMPD_Icon_SetAttributes(pStickerAtrr->ubStickerId, pStickerAtrr);
    }
    else {
        return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
    }

    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_EnableSticker
//  Description :
//------------------------------------------------------------------------------
/** @brief The function enable or disable video recording with sticker

The function enables or disables the specified video sticker with its sticker ID.

@param[in] sStickerID specified the sticker id for video recording
@param[in] bEnable enable or disable the sticker of the specified ID.
@return It reports the status of the operation.
*/
MMP_ERR MMPS_3GPRECD_EnableSticker(MMP_STICKER_ID stickerID, MMP_BOOL bEnable)
{
    return MMPD_Icon_SetEnable(stickerID, bEnable);
}

#if 0
void ____VR_FDTC_Function____(){ruturn;} //dummy
#endif

#if (SUPPORT_FDTC)
//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_StartFDTC
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Start face detection
 @param[in] bStart start or not
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_StartFDTC(MMP_BOOL bStart)
{
    if (m_VidRecdConfigs.bFdtcEnable && m_bVidPreviewActive[0]) {
        if (bStart != MMPS_Sensor_GetFDEnable()) {
            MMPS_Sensor_SetFDEnable(bStart);
        }
    }
    else {
        return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
    }

    return MMP_ERR_NONE;
}
#endif

#if 0
void ____VR_TimeLapse_Function____(){ruturn;} //dummy
#endif

#if (SUPPORT_TIMELAPSE)
MMPS_3GPRECD_AVI_LIST AVIRiff = 
{
    MKTAG('R','I','F','F'),
    327904,
    MKTAG('A','V','I',' ')
};

MMPS_3GPRECD_AVI_LIST AVIHdrl = 
{
    MKTAG('L','I','S','T'),
    192,
    MKTAG('h','d','r','l')
};

MMPS_3GPRECD_AVI_LIST AVIStrl = 
{
    MKTAG('L','I','S','T'),
    116,
    MKTAG('s','t','r','l')
};

MMPS_3GPRECD_AVI_LIST AVIMovi = 
{
    MKTAG('L','I','S','T'),
    4,
    MKTAG('m','o','v','i')
};

MMPS_3GPRECD_AVI_MainHeader AVIMainHdr = 
{
    MKTAG('a','v','i','h'),
    0x38,
    33333,
    20000,
    0,
    0x00000010,
    0,
    0,
    1,  // Stream number
    0,
    1280,  // width
    720,   // height
    0,
    0,
    0,
    0
};

MMPS_3GPRECD_AVI_StreamHeader AVIStrHdr = 
{
    MKTAG('s','t','r','h'),
    0x38,
    MKTAG('v','i','d','s'),
    MKTAG('M','J','P','G'),
    0,
    0,
    0,
    1000,
    30000,
    0,
    0,
    0,
    0xFFFFFFFF,
    0,
    0,
    0
};

MMPS_3GPRECD_AVI_StreamFormat AVIStrFormat = 
{
    MKTAG('s','t','r','f'),
    0x28,
    0x28,
    1280,
    720,
    0x00180001,
    MKTAG('M','J','P','G'),
    1843200,
    0,
    0,
    0,
    0
};

#define AVIINDEX_BUFFER_SIZE (0x50000)

MMPS_3GPRECD_AVI_Header AVIIndex = 
{
    MKTAG('i','d','x','1'),
    AVIINDEX_BUFFER_SIZE
};

MMP_UBYTE m_IndexBuff[512];

MMP_ULONG m_ulHeaderSizePos = 0x04;
MMP_ULONG m_ulMoviSizePos = AVIINDEX_BUFFER_SIZE + 0xE0;
MMP_ULONG m_ulMainHdrFNumPos = 0x30;
MMP_ULONG m_ulStrHdrLengthPos = 0x8C;
MMP_ULONG m_ulIndexStartPos = 0xDC;

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_InitAVIFile
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_InitAVIFile(MMP_BYTE *bAviName, MMP_ULONG ulNameSize, MMP_ULONG ulWidth, MMP_ULONG ulHeight,
                            	 MMP_ULONG CoedcFourCC, MMP_ULONG FrameRate, MMP_ULONG ulBitRate, MMP_BOOL bInit, MMP_ULONG *FileID)
{
    MMP_ERR error;
    MMP_ULONG ulWriteCnt = 0, i, BufSize = sizeof(m_IndexBuff);

    if(!bInit) {
        return MMPS_FS_FileOpen(bAviName, ulNameSize, "r+b", sizeof("r+b"), FileID);
    } else {

        error = MMPS_FS_FileOpen(bAviName, ulNameSize, "w+b", sizeof("w+b"), FileID);
        if(error != MMP_ERR_NONE || *FileID == 0) {
            *FileID = 0;
            return error;
        }
        
        memset(m_IndexBuff, 0, BufSize);
        if (ulWidth != 0) {
            AVIMainHdr.ulWidth = ulWidth;
            AVIStrFormat.ulWidth = ulWidth;
        }
        if (ulHeight != 0) {
            AVIMainHdr.ulHeight = ulHeight;
            AVIStrFormat.ulHeight = ulHeight;
        }
        if (CoedcFourCC != 0) {
            AVIStrHdr.Handler = CoedcFourCC;
            AVIStrFormat.ubCompression = CoedcFourCC;
        }
        if (FrameRate != 0) {
            AVIStrHdr.ulRate = FrameRate;
            AVIMainHdr.ulMSecPreFrame = (1000*1000*1000) / FrameRate;
        }
        if (ulBitRate != 0) {
            AVIMainHdr.ulMaxByteRate = ulBitRate/8;
        }

        AVIStrFormat.ulImageSize = (AVIMainHdr.ulWidth * AVIMainHdr.ulHeight) << 1;
        
        error = MMPS_FS_FileWrite(*FileID, (MMP_UBYTE *)&AVIRiff, sizeof(AVIRiff), &ulWriteCnt);
        if(error != MMP_ERR_NONE) {
            return error;
        }
        error = MMPS_FS_FileWrite(*FileID, (MMP_UBYTE *)&AVIHdrl, sizeof(AVIHdrl), &ulWriteCnt);
        if(error != MMP_ERR_NONE) {
            return error;
        }
        error = MMPS_FS_FileWrite(*FileID, (MMP_UBYTE *)&AVIMainHdr, sizeof(AVIMainHdr), &ulWriteCnt);
        if(error != MMP_ERR_NONE) {
            return error;
        }
        error = MMPS_FS_FileWrite(*FileID, (MMP_UBYTE *)&AVIStrl, sizeof(AVIStrl), &ulWriteCnt);
        if(error != MMP_ERR_NONE) {
            return error;
        }
        error = MMPS_FS_FileWrite(*FileID, (MMP_UBYTE *)&AVIStrHdr, sizeof(AVIStrHdr), &ulWriteCnt);
        if(error != MMP_ERR_NONE) {
            return error;
        }
        error = MMPS_FS_FileWrite(*FileID, (MMP_UBYTE *)&AVIStrFormat, sizeof(AVIStrFormat), &ulWriteCnt);
        if(error != MMP_ERR_NONE) {
            return error;
        }
        error = MMPS_FS_FileWrite(*FileID, (MMP_UBYTE *)&AVIIndex, sizeof(AVIIndex), &ulWriteCnt);
        if(error != MMP_ERR_NONE) {
            return error;
        }

        for(i = 0; i < AVIIndex.ulSize; i+=BufSize){
            if((AVIIndex.ulSize - i) < BufSize) {
                error = MMPS_FS_FileWrite(*FileID, m_IndexBuff, (AVIIndex.ulSize - i), &ulWriteCnt);
                if(error != MMP_ERR_NONE) {
                    return error;
                }
            } else {
                error = MMPS_FS_FileWrite(*FileID, m_IndexBuff, BufSize, &ulWriteCnt);
                if(error != MMP_ERR_NONE) {
                    return error;
                }
            }
        }

        error = MMPS_FS_FileWrite(*FileID, (MMP_UBYTE *)&AVIMovi, sizeof(AVIMovi), &ulWriteCnt);
        if(error != MMP_ERR_NONE) {
            return error;
        }
    }
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_AVIAppendFrame
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_AVIAppendFrame(MMP_ULONG ulFID, MMP_UBYTE *pData, MMP_ULONG ulSize, MMP_ULONG64 *ulFileSize, MMP_ULONG *ulFrameNum)
{
    MMP_ERR error;
    MMP_ULONG ulWriteCnt = 0;
    MMP_ULONG64 ulFSize = 0, ulTempSize, ulTempLen;
    MMPS_3GPRECD_AVI_Header Chunk;
    MMP_UBYTE TempData[4];
    MMPS_3GPRECD_AVI_IndexEntry Entry;
    
    Chunk.ubFourCC = MKTAG('0','0','d','c');
    Chunk.ulSize = ulSize;
    ulSize = ulSize + (ulSize&0x01);

    MMPS_FS_FileGetSize(ulFID, &ulFSize);

    if(((MMP_ULONG64)((MMP_ULONG64)ulFSize + (MMP_ULONG64)ulSize)) > 0xFFFFFFFF) {
        *ulFileSize = ulFSize;
        return MMP_FS_ERR_WRITE_FAIL;
    }
    
    MMPS_FS_FileSeek(ulFID, m_ulMainHdrFNumPos, MMPS_FS_SEEK_SET);
    MMPS_FS_FileRead(ulFID, TempData, 4, &ulWriteCnt);

    ulTempLen = TempData[0] | TempData[1] << 8 | TempData[2] << 16 | TempData[3] << 24;
    if(ulTempLen >= (AVIIndex.ulSize / 16)) {
        *ulFrameNum = ulTempLen;
        return MMP_FS_ERR_WRITE_FAIL;
    }
    
    MMPS_FS_FileSeek(ulFID, 0, MMPS_FS_SEEK_END);
    
    error = MMPS_FS_FileWrite(ulFID, (MMP_UBYTE *)&Chunk, sizeof(Chunk), &ulWriteCnt);
    if(error != MMP_ERR_NONE) {
        return error;
    }
    error = MMPS_FS_FileWrite(ulFID, pData, ulSize, &ulWriteCnt);
    if(error != MMP_ERR_NONE) {
        return error;
    }

    *ulFileSize = ulFSize + ulSize + sizeof(Chunk);
    MMPS_FS_FileSeek(ulFID, m_ulHeaderSizePos, MMPS_FS_SEEK_SET);
    ulTempSize = *ulFileSize - m_ulHeaderSizePos - 4;

    error = MMPS_FS_FileWrite(ulFID, (MMP_UBYTE *)&ulTempSize, 4, &ulWriteCnt);
    if(error != MMP_ERR_NONE) {
        return error;
    }

    // Set Main header frame number
    MMPS_FS_FileSeek(ulFID, m_ulMainHdrFNumPos, MMPS_FS_SEEK_SET);
    MMPS_FS_FileRead(ulFID, TempData, 4, &ulWriteCnt);
    ulTempLen = TempData[0] | TempData[1] << 8 | TempData[2] << 16 | TempData[3] << 24;
    ulTempLen++;
    *ulFrameNum = ulTempLen;

    MMPS_FS_FileSeek(ulFID, m_ulMainHdrFNumPos, MMPS_FS_SEEK_SET);
    error = MMPS_FS_FileWrite(ulFID, (MMP_UBYTE *)&ulTempLen, 4, &ulWriteCnt);
    if(error != MMP_ERR_NONE) {
        return error;
    }

    // Set Stream header frame number
    MMPS_FS_FileSeek(ulFID, m_ulStrHdrLengthPos, MMPS_FS_SEEK_SET);
    error = MMPS_FS_FileWrite(ulFID, (MMP_UBYTE *)&ulTempLen, 4, &ulWriteCnt);
    if(error != MMP_ERR_NONE) {
        return error;
    }
    
    // Set Index entry
    Entry.ubFourCC = MKTAG('0','0','d','c');
    Entry.ulFlag = 0x00000010;
    Entry.ulPos = ulFSize;
    Entry.ulSize = Chunk.ulSize;
    ulTempLen--;

    MMPS_FS_FileSeek(ulFID, (m_ulIndexStartPos + (ulTempLen*16)), MMPS_FS_SEEK_SET);
    error = MMPS_FS_FileWrite(ulFID, (MMP_UBYTE *)&Entry, sizeof(Entry), &ulWriteCnt);
    if(error != MMP_ERR_NONE) {
        return error;
    }
        
    // Set Movi Size
    MMPS_FS_FileSeek(ulFID, m_ulMoviSizePos, MMPS_FS_SEEK_SET);
    ulTempSize = *ulFileSize - m_ulMoviSizePos - 4;
    error = MMPS_FS_FileWrite(ulFID, (MMP_UBYTE *)&ulTempSize, 4, &ulWriteCnt);
    if(error != MMP_ERR_NONE) {
        return error;
    }

    return error;
}
#endif

#if 0
void ____VR_Emergent_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetEmergFileName
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Send emergent 3GP file name to firmware for card mode and creat file time.
 @param[in] bFilename File name.
 @param[in] usLength Length of file name in unit of byte.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetEmergFileName(MMP_BYTE bFilename[], MMP_USHORT usLength)
{
	#if (EMERGENTRECD_SUPPORT == 1)
    MMPS_FS_SetCreationTime();
    MMPD_3GPMGR_SetFileName(bFilename, usLength, MMP_3GPRECD_FILETYPE_EMERGENCY);
	#endif
	
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_EnableEmergentRecd
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Enable emergent video recording.

 Enable emergent video recording.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPS_3GPRECD_EnableEmergentRecd(MMP_BOOL bEnable)
{
	#if (EMERGENTRECD_SUPPORT == 1)
	MMPD_3GPMGR_EnableEmergentRecd(bEnable);
	#endif
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_EnableUVCEmergentRecd
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Enable UVC emergent video recording.

 Enable uvc emergent video recording.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
#if (UVC_EMERGRECD_SUPPORT == 1)
MMP_ERR MMPS_3GPRECD_EnableUVCEmergentRecd(MMP_BOOL bEnable)
{
	MMPD_3GPMGR_EnableUVCEmergentRecd(bEnable);
	return MMP_ERR_NONE;
}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_StartEmergentRecd
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Start emergent video recording.

 Start to save the 3GP file.
 @param[in] bStopVidRecd: stop normal record, keep emergent record
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPS_3GPRECD_StartEmergentRecd(MMP_BOOL bStopVidRecd)
{
	#if (EMERGENTRECD_SUPPORT == 1)
	return MMPD_3GPMGR_StartEmergentRecd(bStopVidRecd);
	#else
	return MMP_3GPMGR_ERR_UNSUPPORT;
    #endif
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_StopEmergentRecd
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Stop emergent video recording.

 Stop to save the 3GP file.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPS_3GPRECD_StopEmergentRecd(MMP_BOOL bBlocking)
{
    #if (EMERGENTRECD_SUPPORT == 1)
    MMP_BOOL bEnable, bRecord;

    MMPD_3GPMGR_GetEmergentRecStatus(&bEnable, &bRecord);

    if (bEnable & bRecord) {
        MMPD_3GPMGR_StopEmergentRecd();

        if (!bBlocking)
            return MMP_ERR_NONE;

        do {
            MMPD_3GPMGR_GetEmergentRecStatus(&bEnable, &bRecord);
        } while (bRecord != MMP_FALSE);
    }
    else {
        return MMP_3GPMGR_ERR_INVLAID_STATE;
    }
    #endif
	
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetEmergentFileTimeLimit
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set the maximum 3GP file time for video encoding.
 @param[in] ulTimeLimitMs Maximum file time in unit of ms.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_NOT_ENOUGH_SPACE Space not enough.
*/
MMP_ERR MMPS_3GPRECD_SetEmergentFileTimeLimit(MMP_ULONG ulTimeLimitMs)
{
	#if (EMERGENTRECD_SUPPORT == 1)
    if (ulTimeLimitMs) {
        return MMPD_3GPMGR_SetEmergentTimeLimit(ulTimeLimitMs);
    }
	#endif

    return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetEmergPreEncTimeLimit
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set the the pre-encoding time limit.
 @param[in] ulTimeLimitMs Maximum file time in ms.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_NOT_ENOUGH_SPACE Space not enough.
*/
MMP_ERR MMPS_3GPRECD_SetEmergPreEncTimeLimit(MMP_ULONG ulTimeLimitMs)
{
	#if (EMERGENTRECD_SUPPORT == 1)
    if (ulTimeLimitMs) {
        return MMPD_3GPMGR_SetEmergPreEncTimeLimit(ulTimeLimitMs);
    }
	#endif

    return MMP_3GPRECD_ERR_INVALID_PARAMETERS;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetEmergentRecordingTime
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get emergent recording time.
 @param[out] ulTime Recording time in unit of ms.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_GetEmergentRecordingTime(MMP_ULONG *ulTime)
{
	#if (EMERGENTRECD_SUPPORT == 1)
    return MMPD_3GPMGR_GetRecordingTime(ulTime, MMP_3GPRECD_FILETYPE_EMERGENCY);
	#else
	return MMP_ERR_NONE;
	#endif	
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetEmergentRecordingOffset
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get emergent recording time offset.
 @param[out] ulTime Recording time offset in unit of ms.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_GetEmergentRecordingOffset(MMP_ULONG *ulTime)
{
	#if (EMERGENTRECD_SUPPORT == 1)
    return MMPD_3GPMGR_GetRecordingOffset(ulTime, MMP_3GPRECD_FILETYPE_EMERGENCY);
	#else
	return MMP_ERR_NONE;
	#endif
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetTime2FlushFSCache
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set max time to flush FS cache buffer to SD.

 @param[in] time Set max time (s).
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetTime2FlushFSCache(MMP_ULONG time)
{
	MMPD_3GPMGR_SetTime2FlushFSCache(time*1000);
	
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_CheckFile2Refix
//  Description :
//------------------------------------------------------------------------------
/**
 @brief check if there is a file need to be refixed.

 @retval MMP_ERR_NONE Success.
*/
#if (VIDRECD_REFIXRECD_SUPPORT == 1)
MMP_ERR MMPS_3GPRECD_CheckFile2Refix(void)
{
	MMPD_3GPMGR_CheckFile2Refix();
	
    return MMP_ERR_NONE;
}
#endif

#if 0
void ____VR_Misc_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_PreAllocateMemory
//  Description :
//------------------------------------------------------------------------------
/** @brief Preallocate memory just like start recording
 * Typically used for streaming case.
 * @return MMP_3GPRECD_ERR_GENERAL_ERROR if it is already recording.
 */
MMP_ERR MMPS_3GPRECD_PreAllocateMemory(void)
{
	MMP_ULONG           enc_id = 0;
    MMPD_MP4VENC_FW_OP  status_vid;

    if (m_VidRecdID == INVALID_ENC_ID) {
    	status_vid = MMPD_MP4VENC_FW_OP_NONE;
    }
    else {
        MMPD_VIDENC_GetStatus(m_VidRecdID, &status_vid);
        enc_id = m_VidRecdID;
    }

    MMPS_3GPRECD_SetEncodeRes(enc_id);

    if ((status_vid == MMPD_MP4VENC_FW_OP_NONE) ||
        (status_vid == MMPD_MP4VENC_FW_OP_STOP)) {

        if (MMPS_3GPRECD_SetH264MemoryMap(  &enc_id,
                                            m_ulVREncodeW[enc_id],
                                            m_ulVREncodeH[enc_id],
                                            m_ulVidRecSramAddr,
                                            m_ulVideoPreviewEndAddr))
        {
            RTNA_DBG_Str(0, "PreAlloc Mem Failed\r\n");
            MMPD_VIDENC_DeInitInstance(enc_id);
            return MMP_3GPRECD_ERR_MEM_EXHAUSTED;
        }
	
	    MMPD_VIDENC_DeInitInstance(enc_id);

		// Restore to video record encoding addr so that record could be done again.					                
        MMPD_VIDENC_SetMemStart(m_ulVidRecEncodeAddr);
    }
    else {
        return MMP_3GPRECD_ERR_GENERAL_ERROR;
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetYFrameType
//  Description : Configure the usage of Y-frame pipe (ex: Motion Detection or LDWS)
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_SetYFrameType(MMPS_3GPRECD_Y_FRAME_TYPE enType)
{
#if (SUPPORT_MDTC)||(SUPPORT_ADAS)
    m_YFrameType = enType;
#endif

    return MMP_ERR_NONE;
}


/**
 @brief Add Atom IDIT for GVP, the atom store starting time of recording.

 @retval size of atom IDIT
*/
#if (MGR_SUPPORT_AVI == 1)
#if (AVI_IDIT_CHUNK_EN == 1)
__inline unsigned int CONVERT_ENDIAN(unsigned int d)
{
	unsigned int t = d;
	*((char*)&d + 3) = *((char*)&t + 0);
	*((char*)&d + 2) = *((char*)&t + 1);
	*((char*)&d + 1) = *((char*)&t + 2);
	*((char*)&d + 0) = *((char*)&t + 3);
	return d;
}

#define	_3GP_FOURCC(b0, b1, b2, b3)	(((unsigned long)(b0)      ) +	\
									 ((unsigned long)(b1) <<  8) +	\
									 ((unsigned long)(b2) << 16) +	\
									 ((unsigned long)(b3) << 24))
#define	ATOM_IDIT		_3GP_FOURCC('I', 'D', 'I', 'T')
#define	ATOM_IDIT_SIZE	sizeof(struct idit_t)
#define	ATOM_IDIT_DACB	20
typedef struct idit_t {
	unsigned long	atombc;
	unsigned long	atomid;
	unsigned char	tmstr[ATOM_IDIT_DACB];
} IDIT_T;

extern AUTL_DATETIME            gVidRecdRTCTime;

unsigned int MMPS_3GPMUX_Build_IDIT(void **ptr)
{
	static IDIT_T	idit;
	idit.atombc = CONVERT_ENDIAN(ATOM_IDIT_SIZE);
	idit.atomid = ATOM_IDIT;
	snprintf((void*)idit.tmstr, ATOM_IDIT_DACB, 
			"%04d-%02d-%02d %02d:%02d:%02d",
			gVidRecdRTCTime.usYear,
			gVidRecdRTCTime.usMonth,
			gVidRecdRTCTime.usDay,
			gVidRecdRTCTime.usHour,
			gVidRecdRTCTime.usMinute,
			gVidRecdRTCTime.usSecond);
	*ptr = &idit;
	return ATOM_IDIT_SIZE;
}
#endif //#if (AVI_IDIT_CHUNK_EN == 1
#endif //#if (MGR_SUPPORT_AVI == 1)
/// @}

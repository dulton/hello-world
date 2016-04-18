//==============================================================================
//
//  File        : mmps_iva.c
//  Description : Intelligent video analysis function
//  Author      :
//  Revision    : 1.0
//
//==============================================================================

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "ait_utility.h"
#include "mmps_iva.h"
#include "mmpd_system.h"
#include "mmpd_iva.h"
#include "mmpd_sensor.h"
#include "lib_retina.h"

//==============================================================================
//
//                              GLOBAL VARIABLES
//
//==============================================================================

#if (SUPPORT_FDTC)
/**@brief The face detection configuration
*/
static	MMPS_FDTC_CONFIG        m_FdtcConfig;

/** @brief 	The face detection attribute.

Use @ref MMPS_Sensor_InitializeFaceDetect to set it.
*/
static MMPD_FDTC_ATTR           m_FdtcAttribute;

/** @brief 	The face detection mode.

Use @ref MMPS_Sensor_GetFDEnable to get it,
         and use MMPS_Sensor_SetFDEnable to set it.
*/
static MMP_BOOL                 m_CurFdtcMode = MMP_FALSE;

/**	@brief	fdtc path controller */
static MMPS_SENSOR_FDTC_PATH    m_FdtcPath    = MMPS_SENSOR_FDTC_LOOPBACK;

static MMPD_FCTL_LINK m_fctlLinkFDPreview = {MMP_SCAL_PIPE_1, MMP_ICO_PIPE_1, MMP_IBC_PIPE_1};

static MMPD_FCTL_LINK m_fctlLinkFDLoopback = {MMP_SCAL_PIPE_0, MMP_ICO_PIPE_0, MMP_IBC_PIPE_0};
#endif

#if (SUPPORT_MDTC)
/** @brief The video motion detection mode.

   Use @ref MMPS_Sensor_IsVMDEnable to get it,
   and use @ref MMPS_Sensor_EnableVMD to set it.
*/
static MMP_BOOL                 m_bCurVMDMode = MMP_FALSE;

/** @brief The pipe to output luma for motion detection.

   Use @ref MMPS_Sensor_SetVMDPipe to set it,
*/
static MMP_IBC_PIPEID           m_VMDPipe = MMP_IBC_PIPE_MAX;
#endif

#if (SUPPORT_ADAS)
/** @brief The ADAS mode.

   Use @ref MMPS_Sensor_IsADASEnable to get it,
   and use @ref MMPS_Sensor_EnableADAS to set it.
*/
static MMP_BOOL                 m_bCurADASMode = MMP_FALSE;
static MMP_BOOL                 m_bCurADASState[MMPS_ADAS_FEATURE_NUM] = {0, };
#endif

extern MMP_BOOL                 gubMmpDbgBk;
//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

#if 0		 
void _____FDTC_Functions_____(){}
#endif

#if (SUPPORT_FDTC)

static MMP_ERR MMPS_Sensor_SetupFDSubpipe(void);
static MMP_ERR MMPS_Sensor_SetupFDLoopback(void);

//------------------------------------------------------------------------------
//  Function    : MMPS_Fdtc_GetConfig
//  Description :
//------------------------------------------------------------------------------
/** @brief Retrives sensor configuration

The function retrives all sensor configuration, including sensor output width/heigh in preview mode or in capture mode.
It also report the framerate selection and exposure value level.

@return It returns all sensor configuration
*/
MMPS_FDTC_CONFIG* MMPS_Fdtc_GetConfig(void)
{
    return &m_FdtcConfig;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_AllocateFDBuffer
//  Description :
//------------------------------------------------------------------------------
/** @brief the function will allocate buffers for face detection.

The function should be called in MMPS_DSC_SetPreviewMode().
@param[in] ulStartAddr 		The start address to allocate buffers.
@param[in] usInputWidth 	The width of FDTC input buffer.
@param[in] usInputHeight 	The height of FDTC input buffer.
@param[in] ulResevOsdSize 	The reserved OSD size for host to draw face rectangles.
@return It reports the status of buffer allocation.
*/
MMP_ERR MMPS_Sensor_AllocateFDBuffer(	MMP_ULONG 	*ulStartAddr, 
										MMP_USHORT 	usInputWidth, 
                                     	MMP_USHORT 	usInputHeight, 
                                     	MMP_BOOL 	bAllocate)
{
    MMP_ERR     err 		= MMP_ERR_NONE;
    MMP_ULONG   ulCurBufPos = 0;
    MMP_ULONG	ulBufSize;
    MMP_LONG    i			= 0;
    MMP_USHORT  usFdtcMaxWidth, usFdtcMaxHeight;
    MMP_UBYTE   ubFdtcMaxFaceNum;

    MMPD_Sensor_GetFaceDetectSpec(&usFdtcMaxWidth, &usFdtcMaxHeight, &ubFdtcMaxFaceNum);
    
	if ((usInputWidth > usFdtcMaxWidth) || (usInputHeight > usFdtcMaxHeight))
	{
		PRINTF("FDTC: Unsupport Resolution. \r\n");
		return MMP_SENSOR_ERR_FDTC;
    }
    
    if (m_FdtcConfig.ubDetectFaceNum > ubFdtcMaxFaceNum)
    {
	    PRINTF("FDTC: Unsupport Face Number. \r\n");
	    return MMP_SENSOR_ERR_FDTC;
	}
	
    m_FdtcAttribute.usInputW 		= usInputWidth;								
    m_FdtcAttribute.usInputH 		= usInputHeight;							
    m_FdtcAttribute.ubDetectFaceNum = m_FdtcConfig.ubDetectFaceNum;		
    m_FdtcAttribute.ubInputBufCnt 	= m_FdtcConfig.ubFaceDetectInputBufCnt;	
    m_FdtcAttribute.usGrabScaleM 	= m_FdtcConfig.usFaceDetectGrabScaleM;
    m_FdtcAttribute.ubSmileDetectEn	= m_FdtcConfig.bSmileDetectEnable;

	ulCurBufPos = *ulStartAddr;
	ulCurBufPos = ALIGN4K(ulCurBufPos);				///< 4K alignment for dynamic adjustment cache mechanism
    m_FdtcAttribute.ulFDWorkBufAddr = ulCurBufPos;
    
    MMPD_Sensor_GetFDBufSize(	MMPD_SENSOR_FD_WORK_BUF, 
    							m_FdtcAttribute.usInputW, 
    							m_FdtcAttribute.usInputH, 
    							m_FdtcAttribute.ubDetectFaceNum, 
    							&ulBufSize);
    							
    ulCurBufPos += ulBufSize;	///< need cacheable
	    
	/**	@brief	need to add the interface to select smile detection. */
	if (m_FdtcAttribute.ubSmileDetectEn == MMP_TRUE) {
	    m_FdtcAttribute.ulSDWorkBufAddr = ulCurBufPos;
	    	
        MMPD_Sensor_GetFDBufSize(	MMPD_SENSOR_SD_WORK_BUF, 
									m_FdtcAttribute.usInputW, 
									m_FdtcAttribute.usInputH, 
									m_FdtcAttribute.ubDetectFaceNum, 
									&ulBufSize);
									
	    ulCurBufPos += ulBufSize;	///< need cacheable
	}
    
    m_FdtcAttribute.ulFeatureBufAddr = ulCurBufPos;
    ulCurBufPos += m_FdtcAttribute.usInputW * m_FdtcAttribute.usInputH;///< need cacheable. only Y data.
    
    ulCurBufPos = ALIGN4K(ulCurBufPos);		///< 4K alignment for dynamic adjustment cache mechanism
    m_FdtcAttribute.ulInfoBufAddr = ulCurBufPos;
    
    MMPD_Sensor_GetFDBufSize(	MMPD_SENSOR_FD_INFO_BUF, 
								m_FdtcAttribute.usInputW, 
								m_FdtcAttribute.usInputH, 
								m_FdtcAttribute.ubDetectFaceNum, 
								&ulBufSize);
    ulCurBufPos += ulBufSize;
	
	/**	@brief	Only allocate UV buffer for the first one */
    m_FdtcAttribute.ulInputBufAddr[0] = ulCurBufPos;
    ulCurBufPos += ((m_FdtcAttribute.usInputW * m_FdtcAttribute.usInputH * 3) >> 1); 

    for (i = 1; i < m_FdtcAttribute.ubInputBufCnt; i++) {
    
        m_FdtcAttribute.ulInputBufAddr[i] = ulCurBufPos;
        ulCurBufPos += (m_FdtcAttribute.usInputW * m_FdtcAttribute.usInputH);
    }
    
    err = MMPD_Sensor_InitFaceDetectConfig(&m_FdtcAttribute);
    
    if (err != MMP_ERR_NONE) {
        PRINTF("Initialize face detection configuration failed\r\n");
        return err;
    }
    
    err = MMPD_Sensor_SetFaceDetectBuf(&m_FdtcAttribute);
    
    if (err != MMP_ERR_NONE) {
        PRINTF("Set face detection buffers failed\r\n");
        return err;
    }
    PRINTF("End of FDTC buffer = 0x%X\r\n", ulCurBufPos);
    *ulStartAddr = ulCurBufPos;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_FDPathInterface
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_Sensor_FDPathInterface(MMPS_SENSOR_FDTC_PATH pathselect, MMPD_FCTL_LINK *fctlLinkFDPreview, MMPD_FCTL_LINK *fctlLinkFDLoopback)
{
	m_FdtcPath				= pathselect;
	m_fctlLinkFDPreview  	= *fctlLinkFDPreview;
	m_fctlLinkFDLoopback 	= *fctlLinkFDLoopback;
	
	if (pathselect == MMPS_SENSOR_FDTC_LOOPBACK) {
        #if (MCR_V2_UNDER_DBG == 0)
        // Have bug. Remove first on Mercury V2 test
		MMPS_Sensor_SetupFDLoopback();
		#endif
	}	
	else if (pathselect == MMPS_SENSOR_FDTC_SUBPIPE) {
		MMPS_Sensor_SetupFDSubpipe();
	}	
	else if (pathselect == MMPS_SENSOR_FDTC_YUV420) {
		PRINTF("MMPS_SENSOR_FDTC_YUV420 \r\n");
    }
	else {
		PRINTF("FDTC: Unsupport mode. \r\n");
		return MMP_SENSOR_ERR_FDTC;
	}
	return MMP_ERR_NONE;
}

#if (MCR_V2_UNDER_DBG == 0)
//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_SetupFDLoopback
//  Description :
//------------------------------------------------------------------------------
/** @brief the function will set up the scaler path for face detection.

The function set command to firmware to establish the scale path for face detection.
According to path configuration to set two pipes or graphic loopback path.
@return It reports the status of the operation.
*/
static MMP_ERR MMPS_Sensor_SetupFDLoopback(void)
{
    MMP_SCAL_FIT_RANGE  	fitrange 			= {0, };
    MMP_SCAL_GRAB_CTRL		grabctl 			= {0, };
    MMPD_FCTL_ATTR  		fctlfdtcattribute 	= {0, };
    MMPD_FCTL_LINK			fctllink 			= {0, };
    MMP_ULONG        		i = 0;

    // Calaulate scaling parameters
    // Get preview width/height
    MMPD_Fctl_GetAttributes(m_fctlLinkFDPreview.ibcpipeID, &fctlfdtcattribute);

    fitrange.fitmode 		= MMP_SCAL_FITMODE_OUT;
    fitrange.scalerType 	= MMP_SCAL_TYPE_SCALER;
    fitrange.ulInWidth		= fctlfdtcattribute.grabctl.ulOutEdX - fctlfdtcattribute.grabctl.ulOutStX + 1;
    fitrange.ulInHeight 	= fctlfdtcattribute.grabctl.ulOutEdY - fctlfdtcattribute.grabctl.ulOutStY + 1;
    fitrange.ulOutWidth 	= m_FdtcAttribute.usInputW;
    fitrange.ulOutHeight 	= m_FdtcAttribute.usInputH;

    fitrange.ulInGrabX 		= 1;
    fitrange.ulInGrabY 		= 1;
    fitrange.ulInGrabW 		= fitrange.ulInWidth;
    fitrange.ulInGrabH 		= fitrange.ulInHeight;
    
    MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);

	fctlfdtcattribute.ulBaseAddr[0] = m_FdtcAttribute.ulInputBufAddr[0];
	fctlfdtcattribute.ulBaseUAddr[0] = fctlfdtcattribute.ulBaseAddr[0]  + fitrange.ulOutWidth * fitrange.ulOutHeight;
	fctlfdtcattribute.ulBaseVAddr[0] = fctlfdtcattribute.ulBaseUAddr[0] + ((fitrange.ulOutWidth * fitrange.ulOutHeight) >> 2);

   	if (m_FdtcAttribute.ubInputBufCnt > 1) {
        // Only allocate U/V buffer to the first input buffer.
        fctlfdtcattribute.ulBaseAddr[1]		= fctlfdtcattribute.ulBaseAddr[0] + ((fitrange.ulOutWidth * fitrange.ulOutHeight * 3) >> 1);
        fctlfdtcattribute.ulBaseUAddr[1] 	= fctlfdtcattribute.ulBaseUAddr[0];
        fctlfdtcattribute.ulBaseVAddr[1] 	= fctlfdtcattribute.ulBaseVAddr[0];

        for (i = 2; i < m_FdtcAttribute.ubInputBufCnt; i++) {

            fctlfdtcattribute.ulBaseAddr[i] = fctlfdtcattribute.ulBaseAddr[i - 1] + (fitrange.ulOutWidth * fitrange.ulOutHeight);
            // Always use the same U/V buffer
            fctlfdtcattribute.ulBaseUAddr[i] = fctlfdtcattribute.ulBaseUAddr[0];
            fctlfdtcattribute.ulBaseVAddr[i] = fctlfdtcattribute.ulBaseVAddr[0];
        }
 	}
	else {
        for (i = 1; i < m_FdtcAttribute.ubInputBufCnt; i++) {
           	fctlfdtcattribute.ulBaseAddr[i]  = fctlfdtcattribute.ulBaseAddr[0];
            fctlfdtcattribute.ulBaseUAddr[i] = fctlfdtcattribute.ulBaseUAddr[0];
           	fctlfdtcattribute.ulBaseVAddr[i] = fctlfdtcattribute.ulBaseVAddr[0];
        }
   	}

    fctllink.scalerpath = m_fctlLinkFDLoopback.scalerpath;
    fctllink.icopipeID 	= m_fctlLinkFDLoopback.icopipeID;
    fctllink.ibcpipeID 	= m_fctlLinkFDLoopback.ibcpipeID;

	fctlfdtcattribute.colormode 		= MMP_DISPLAY_COLOR_YUV420;
	fctlfdtcattribute.fctllink 			= fctllink;
	fctlfdtcattribute.usInputW 			= fitrange.ulInWidth;
	fctlfdtcattribute.usInputH 			= fitrange.ulInHeight;
	fctlfdtcattribute.fitrange			= fitrange;
	fctlfdtcattribute.grabctl 			= grabctl;
    fctlfdtcattribute.scalsrc			= MMP_SCAL_SOURCE_ISP;
    fctlfdtcattribute.bSetScalerSrc		= MMP_TRUE;
	fctlfdtcattribute.usBufCnt 			= m_FdtcAttribute.ubInputBufCnt;
    fctlfdtcattribute.bUseRotateDMA		= MMP_FALSE;
    fctlfdtcattribute.usRotateBufCnt 	= 0;
	
	MMPD_Fctl_SetPipeAttrForIbcFB(&fctlfdtcattribute);
	MMPD_Fctl_ClearPreviewBuf(fctllink.ibcpipeID, 0xFFFFFF);
	
    // Change scaler input path to graphic engine
    MMPD_Scaler_SetPath(fctllink.scalerpath, MMP_SCAL_SOURCE_GRA, MMP_TRUE);

    MMPD_Fctl_LinkPipeToFdtc(fctllink.ibcpipeID);
    
    return MMP_ERR_NONE;
}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_SetupFDSubpipe
//  Description :
//------------------------------------------------------------------------------
static MMP_ERR MMPS_Sensor_SetupFDSubpipe(void)
{
#if 0//TBD
    MMPD_FCTL_ATTR	fctlfdtcattribute 	= {0, };
    MMP_ULONG     	i = 0;
    
	MMPD_Scaler_SetEnable(m_fctlLinkFDPreview.scalerpath, MMP_FALSE);
	
    MMPD_Fctl_GetAttributes(m_fctlLinkFDPreview.ibcpipeID, &fctlfdtcattribute);
    
    fctlfdtcattribute.bUseSubPipe 				= MMP_TRUE;
    fctlfdtcattribute.usSubPipeBufCnt 			= m_FdtcAttribute.ubInputBufCnt;
    fctlfdtcattribute.ubSubPipeDownsampleValue	= 1;
	fctlfdtcattribute.ulSubPipeAddr[0]          = m_FdtcAttribute.ulInputBufAddr[0];
   	
   	if (m_FdtcAttribute.ubInputBufCnt > 1) {
                                        
        for (i = 1; i < m_FdtcAttribute.ubInputBufCnt; i++)
            fctlfdtcattribute.ulSubPipeAddr[i] = fctlfdtcattribute.ulSubPipeAddr[i - 1] + m_FdtcAttribute.usInputW * m_FdtcAttribute.usInputH;
 	}
 		
	MMPD_Fctl_SetPipeAttrForIbcFB(&fctlfdtcattribute);
#endif
	return MMP_ERR_NONE;
}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_SetFDEnable
//  Description :
//------------------------------------------------------------------------------
/** @brief the function will enable/disable face detection.

The function set command to firmware to enable/disable face detection.
@param[in] bEnable Set face detection enable or disable.
@return It reports the status of the operation.
*/
MMP_ERR MMPS_Sensor_SetFDEnable(MMP_BOOL bEnable)
{
    #if (SUPPORT_FDTC)
    MMP_USHORT  	usPreviewBufWidth;
    MMP_USHORT 		usPreviewBufHeight;
    MMPD_FCTL_ATTR  fctlPreviewAttr;

    MMPD_Fctl_GetAttributes(m_fctlLinkFDPreview.ibcpipeID, &fctlPreviewAttr);
    
    usPreviewBufWidth 	= fctlPreviewAttr.grabctl.ulOutEdX - fctlPreviewAttr.grabctl.ulOutStX + 1;
    usPreviewBufHeight 	= fctlPreviewAttr.grabctl.ulOutEdY - fctlPreviewAttr.grabctl.ulOutStY + 1;
    
    MMPD_Sensor_SetFaceDetectDisplayInfo(m_fctlLinkFDPreview.ibcpipeID, usPreviewBufWidth, usPreviewBufHeight);

    if (bEnable) {
        
		MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_TRUE); ///< draw rectangle
            
        /**	@brief 	Enable face detection, switch working buffer to cacheable first, then enable path */
        MMPD_Sensor_ActivateFDBuffer(bEnable, m_FdtcPath);
        
        if (m_FdtcPath == MMPS_SENSOR_FDTC_LOOPBACK)
        {
	        /**	@brief	connect the IBC of preview path to graphic engine. */
	        MMPD_Fctl_LinkPipeToGraphic(m_fctlLinkFDPreview.ibcpipeID);
	        
	        // Check if scaler is not yet enabled
	        MMPD_Scaler_SetEnable(m_fctlLinkFDLoopback.scalerpath, MMP_TRUE);
	        
	        MMPD_Fctl_EnablePreview(ubSnrSel, m_fctlLinkFDLoopback.ibcpipeID, bEnable, MMP_FALSE);
        }
        else if (m_FdtcPath == MMPS_SENSOR_FDTC_YUV420) {

        	if( fctlPreviewAttr.colormode != MMP_DISPLAY_COLOR_YUV420)
        		return MMP_SENSOR_ERR_FDTC;
        		
        	// need to check the color mode of display
        	MMPD_Fctl_LinkPipeToFdtc(m_fctlLinkFDPreview.ibcpipeID);
        
        }
        else {
        	PRINTF("Unsupport FD type\r\n");
        }
    }
    else {
    
    	// Disable face detection, disable path, then switch working buffer to non-cacheable.
        MMPD_Sensor_ActivateFDBuffer(bEnable, m_FdtcPath);  
              
        if (m_FdtcPath == MMPS_SENSOR_FDTC_LOOPBACK) {
	        MMPD_Fctl_EnablePreview(ubSnrSel, m_fctlLinkFDLoopback.ibcpipeID, bEnable, MMP_FALSE);        
	        MMPD_Fctl_UnlinkPipeToGraphic(m_fctlLinkFDPreview.ibcpipeID);     
        }
		else if (m_FdtcPath == MMPS_SENSOR_FDTC_YUV420){	
			MMPD_Fctl_UnlinkPipeToFdtc(m_fctlLinkFDPreview.ibcpipeID);
        }
        else {
        	PRINTF("Unsupport FD type\r\n");
        }
        
        // Need to check the graphic already done
		MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_FALSE);
    }

    m_CurFdtcMode = bEnable;

    return MMP_ERR_NONE;
    #else
    return MMP_SENSOR_ERR_FDTC;
    #endif
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_GetFDEnable
//  Description :
//------------------------------------------------------------------------------
/** @brief the function will get face detection current status.

The function get the current status of face detection.
@return It reports the status of the operation.
*/
MMP_BOOL MMPS_Sensor_GetFDEnable(void)
{
    #if (SUPPORT_FDTC)
    return m_CurFdtcMode;
    #else
    return MMP_FALSE;
    #endif
}

#if 0
void _____VMD_Functions_____(){}
#endif

#if (SUPPORT_MDTC)
//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_SetVMDPipe
//  Description :
//------------------------------------------------------------------------------
/** @brief the function set which pipe outputs luma for motion detection.

The function should be called before MMPS_Sensor_EnableVMD().
@param[in] pipe     The pipe to output Luma frame.
@return None.
*/
void MMPS_Sensor_SetVMDPipe(MMP_IBC_PIPEID pipe)
{
    m_VMDPipe = pipe;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_AllocateVMDBuffer
//  Description :
//------------------------------------------------------------------------------
/** @brief the function will allocate buffers for video motion detection.

The function should be called in MMPS_3GPRECD_SetPreviewMemory().
@param[in] ulStartAddr  The start address to allocate buffers.
@param[in] usWidth      The width of VMD input buffer.
@param[in] usHeight     The height of VMD input buffer.
@param[in] bAllocate    Allocate buffers from memory pool.
@return It reports the status of buffer allocation.
*/
MMP_ERR MMPS_Sensor_AllocateVMDBuffer(MMP_ULONG *ulStartAddr, MMP_BOOL bAllocate)
{
    #if (SUPPORT_MDTC)
    MMP_ERR     err 		= MMP_ERR_NONE;
    MMP_ULONG   ulCurBufPos = 0, ulBufSize = 0;

    ulCurBufPos = *ulStartAddr;
    ulCurBufPos = ALIGN4K(ulCurBufPos); ///< 4K alignment for dynamic adjustment cache mechanism

    err = MMPD_Sensor_InitializeVMD();

    if (err != MMP_ERR_NONE) {
        PRINTF("Initialize video motion detection config failed\r\n");
        return err;
    }

    err = MMPD_Sensor_SetVMDBuf(ulCurBufPos, &ulBufSize);
    if (err != MMP_ERR_NONE) {
        PRINTF("Set video motion detection buffers failed\r\n");
        return err;
    }

    ulCurBufPos += ulBufSize;
    PRINTF("End of VMD buffer = 0x%X\r\n", ulCurBufPos);
    *ulStartAddr = ulCurBufPos;

    return MMP_ERR_NONE;
    #else
    return MMP_SENSOR_ERR_VMD;
    #endif
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_RegisterVMDCallback
//  Description :
//------------------------------------------------------------------------------
/** @brief the function registers the callback function to inform motion detected.

The function registers the callback which will be called when motion detected.
@param[in] Callback The Callback function
@return It reports the status of the operation.
*/
MMP_ERR MMPS_Sensor_RegisterCallback(MMPS_IVA_EVENT event, void *callback)
{
    MMPD_SENSOR_EVENT e;

    switch(event) {
    case MMPS_IVA_EVENT_MDTC:
        #if (SUPPORT_MDTC)
        return MMPD_Sensor_RegisterVMDCallback(callback);
        #endif
    case MMPS_IVA_EVENT_TV_SRC_TYPE:
        e = MMPD_SENSOR_EVENT_TV_SRC_TYPE;
        break;
    default:
        return MMP_SENSOR_ERR_NOT_SUPPORT;
    }

    return MMPD_Sensor_RegisterCallback(e, callback);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_GetVMDResolution
//  Description :
//------------------------------------------------------------------------------
/** @brief the function gets the configured resolution of motion detection frame.

The function gets the configured resolution of motion detection frame.
@param[out] width   The width of motion detection source frame
@param[out] height  The height of motion detection source frame
@return It reports the status of the operation.
*/
MMP_ERR MMPS_Sensor_GetVMDResolution(MMP_ULONG *ulWidth, MMP_ULONG *ulHeight)
{
    #if (SUPPORT_MDTC)
    return MMPD_Sensor_GetVMDResolution(ulWidth, ulHeight);
    #else
    return MMP_SENSOR_ERR_VMD;
    #endif
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_SetVMDEnable
//  Description :
//------------------------------------------------------------------------------
/** @brief the function will enable/disable the video motion detection.

The function set command to firmware to enable/disable the motion detection.
@param[in] bEnable Enable or disable motion detection.
@return It reports the status of the operation.
*/
MMP_ERR MMPS_Sensor_EnableVMD(MMP_BOOL bEnable)
{
    #if (SUPPORT_MDTC)
    if (bEnable != m_bCurVMDMode) {

        MMPD_Sensor_EnableVMD(bEnable);
        if (bEnable)
            MMPD_Fctl_LinkPipeToMdtc(m_VMDPipe);
        else
            MMPD_Fctl_UnLinkPipeToMdtc(m_VMDPipe);

        m_bCurVMDMode = bEnable; 
    }
    return MMP_ERR_NONE;
    #else
    return MMP_SENSOR_ERR_VMD;
    #endif
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_IsVMDEnable
//  Description :
//------------------------------------------------------------------------------
/** @brief the function will check if motion detection is enabled.

The function get the current status of motion detection.
@return It reports the status of the operation.
*/
MMP_BOOL MMPS_Sensor_IsVMDEnable(void)
{
    #if (SUPPORT_MDTC)
    return m_bCurVMDMode;
    #else
    return MMP_FALSE;
    #endif
}
#endif

#if 0
void _____ADAS_Functions_____(){}
#endif

#if (SUPPORT_ADAS)
//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_AllocateADASBuffer
//  Description :
//------------------------------------------------------------------------------
/** @brief the function will allocate buffers for video ADAS.

The function should be called in MMPS_3GPRECD_SetPreviewMemory().
@param[in] ulStartAddr  The start address to allocate buffers.
@param[in] bAllocate    Allocate buffers from memory pool.
@param[in] ulDMABufSize The dma destination buffer size of ADAS.
@return It reports the status of buffer allocation.
*/
MMP_ERR MMPS_Sensor_AllocateADASBuffer(MMP_ULONG *ulStartAddr, MMP_BOOL bAllocate, MMP_ULONG ulDMABufSize)
{
    MMP_ERR     err 		= MMP_ERR_NONE;
    MMP_ULONG   ulCurBufPos = 0, ulBufSize = 0;
    MMP_ULONG   ulDMAAddr = 0;

    ulCurBufPos = *ulStartAddr;
    ulCurBufPos = ALIGN4K(ulCurBufPos); ///< 4K alignment for dynamic adjustment cache mechanism
    // Allocate DMA buffer
    ulDMAAddr = ulCurBufPos;
    ulCurBufPos += ulDMABufSize;
    ulCurBufPos = ALIGN4K(ulCurBufPos); ///< 4K alignment for dynamic adjustment cache mechanism

    err = MMPD_Sensor_InitializeADAS();

    if (err != MMP_ERR_NONE) {
        PRINTF("Initialize video ADAS config failed\r\n");
        return err;
    }

    err = MMPD_Sensor_SetADASFeature(m_bCurADASState[MMPS_ADAS_LDWS],
                                     m_bCurADASState[MMPS_ADAS_FCWS],
                                     m_bCurADASState[MMPS_ADAS_SAG]);
    if (err != MMP_ERR_NONE) {
        PRINTF("Set video ADAS features failed\r\n");
        return err;
    }

    err = MMPD_Sensor_SetADASBuf(ulCurBufPos, &ulBufSize, ulDMAAddr);

    if (err != MMP_ERR_NONE) {
        PRINTF("Set video ADAS buffers failed\r\n");
        return err;
    }

    ulCurBufPos += ulBufSize;
    PRINTF("End of ADAS buffer = 0x%X\r\n", ulCurBufPos);
    *ulStartAddr = ulCurBufPos;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_GetADASResolution
//  Description :
//------------------------------------------------------------------------------
/** @brief the function gets the configured resolution of ADAS frame.

The function gets the configured resolution of ADAS frame.
@param[out] width   The width of ADAS source frame
@param[out] height  The height of ADAS source frame
@return It reports the status of the operation.
*/
MMP_ERR MMPS_Sensor_GetADASResolution(MMP_ULONG *ulWidth, MMP_ULONG *ulHeight)
{
    return MMPD_Sensor_GetADASResolution(ulWidth, ulHeight);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_SetADASFeatureEn
//  Description :
//------------------------------------------------------------------------------
/** @brief the function will enable/disable sub-feature of ADAS.

The function set command to firmware to enable/disable the ADAS.
@param[in] feature Sub-feature of ADAS
@param[in] bEnable Enable or disable ADAS.
@return It reports the status of the operation.
*/
MMP_ERR MMPS_Sensor_SetADASFeatureEn(MMPS_IVA_ADAS_FEATURE feature, MMP_BOOL bEnable)
{
    if (feature < MMPS_ADAS_FEATURE_NUM) {
        m_bCurADASState[feature] = bEnable;
        return MMP_ERR_NONE;
    }

    return MMP_SENSOR_ERR_PARAMETER;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_GetADASFeatureEn
//  Description :
//------------------------------------------------------------------------------
/** @brief the function gets the current state of ADAS sub-feature.

The function gets the current state of the specified ADAS sub-feature.
@param[in] feature Sub-feature of ADAS
@return It reports the status of the specified ADAS sub-feature.
*/
MMP_BOOL MMPS_Sensor_GetADASFeatureEn(MMPS_IVA_ADAS_FEATURE feature)
{
    if (feature < MMPS_ADAS_FEATURE_NUM)
        return m_bCurADASState[feature] & m_bCurADASMode;

    return MMP_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_EnableADAS
//  Description :
//------------------------------------------------------------------------------
/** @brief the function will enable/disable the ADAS operation.

The function set command to firmware to enable/disable the ADAS.
Note: Use VMD flow/pipe for ADAS
@param[in] bEnable Enable or disable ADAS.
@return It reports the status of the operation.
*/
MMP_ERR MMPS_Sensor_EnableADAS(MMP_BOOL bEnable)
{
    MMP_ERR sRet = MMP_ERR_NONE;
    
    if (bEnable != m_bCurADASMode) {

        sRet = MMPD_Sensor_EnableADAS(bEnable);
        if(sRet != MMP_ERR_NONE) { MMP_PRINT_RET_ERROR(0, sRet, "",gubMmpDbgBk); return sRet;}
        
        if (bEnable){
            sRet = MMPD_Fctl_LinkPipeToMdtc(m_VMDPipe);
            if(sRet != MMP_ERR_NONE) { MMP_PRINT_RET_ERROR(0, sRet, "",gubMmpDbgBk); return sRet;}
        }
        else{
            sRet = MMPD_Fctl_UnLinkPipeToMdtc(m_VMDPipe);
            if(sRet != MMP_ERR_NONE) { MMP_PRINT_RET_ERROR(0, sRet, "",gubMmpDbgBk); return sRet;}
        }

        m_bCurADASMode = bEnable; 
    }
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_IsADASEnable
//  Description :
//------------------------------------------------------------------------------
/** @brief the function will check if ADAS is enabled.

The function get the current status of ADAS.
@return It reports the status of the operation.
*/
MMP_BOOL MMPS_Sensor_IsADASEnable(void)
{
    return m_bCurADASMode;
}
#endif


//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "ait_utility.h"
#include "mmps_3gprecd.h"
#include "mmpd_mp4venc.h"

//==============================================================================
//
//                              GLOBAL VARIABLES
//
//==============================================================================

MMP_BOOL 	                            gbUVCRecdSupport        = MMP_FALSE;
static MMPS_3GPRECD_AHC_PREVIEW_INFO 	m_sAhcVideoUVCPrevInfo  = {0};

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

#if 0
void ____UVC_Rec_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_UVCSetCustomedPrevwAttr
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
MMP_ERR MMPS_3GPRECD_UVCSetCustomedPrevwAttr(MMP_BOOL 	bUserConfig,
										 MMP_BOOL 	bRotate,
										 MMP_UBYTE 	ubRotateDir,
										 MMP_UBYTE	sFitMode,
										 MMP_USHORT usBufWidth, MMP_USHORT usBufHeight, 
										 MMP_USHORT usStartX, 	MMP_USHORT usStartY,
                                      	 MMP_USHORT usWinWidth, MMP_USHORT usWinHeight)
{
    m_sAhcVideoUVCPrevInfo.bUserDefine  	= bUserConfig;
    m_sAhcVideoUVCPrevInfo.bPreviewRotate 	= bRotate;
    m_sAhcVideoUVCPrevInfo.sPreviewDmaDir	= ubRotateDir;
    m_sAhcVideoUVCPrevInfo.sFitMode		= sFitMode;
    m_sAhcVideoUVCPrevInfo.ulPreviewBufW	= usBufWidth;
    m_sAhcVideoUVCPrevInfo.ulPreviewBufH 	= usBufHeight;
    m_sAhcVideoUVCPrevInfo.ulDispStartX  	= usStartX;
    m_sAhcVideoUVCPrevInfo.ulDispStartY  	= usStartY;
    m_sAhcVideoUVCPrevInfo.ulDispWidth   	= usWinWidth;
    m_sAhcVideoUVCPrevInfo.ulDispHeight  	= usWinHeight;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_UVCGetCustomedPrevwAttr
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_3GPRECD_UVCGetCustomedPrevwAttr(MMP_BOOL 	*pbUserConfig,
										 MMP_BOOL 	*pbRotate,
										 MMP_UBYTE 	*pubRotateDir,
										 MMP_UBYTE	*psFitMode,
										 MMP_USHORT *pusBufWidth, MMP_USHORT*pusBufHeight, 
										 MMP_USHORT *pusStartX, 	MMP_USHORT *pusStartY,
                                      	 MMP_USHORT *pusWinWidth, MMP_USHORT *pusWinHeight)    
{
    *pbUserConfig   = m_sAhcVideoUVCPrevInfo.bUserDefine;
    *pbRotate       = m_sAhcVideoUVCPrevInfo.bPreviewRotate; 
    *pubRotateDir   = m_sAhcVideoUVCPrevInfo.sPreviewDmaDir;
    *psFitMode      = m_sAhcVideoUVCPrevInfo.sFitMode;
    *pusBufWidth    = m_sAhcVideoUVCPrevInfo.ulPreviewBufW;
    *pusBufHeight   = m_sAhcVideoUVCPrevInfo.ulPreviewBufH;
    *pusStartX      = m_sAhcVideoUVCPrevInfo.ulDispStartX;
    *pusStartY      = m_sAhcVideoUVCPrevInfo.ulDispStartY;
    *pusWinWidth    = m_sAhcVideoUVCPrevInfo.ulDispWidth;
    *pusWinHeight   = m_sAhcVideoUVCPrevInfo.ulDispHeight;

    return MMP_ERR_NONE;
}

#if (UVC_VIDRECD_SUPPORT)
//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetUVCRecdSupport
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Enable UVC video recording.

 Enable UVC video recording.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPS_3GPRECD_SetUVCRecdSupport(MMP_BOOL bSupport)
{
	gbUVCRecdSupport = bSupport;
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetUVCRecdSupport
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get UVC video recording enable states.

Get UVC video recording enable states.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPS_3GPRECD_GetUVCRecdSupport(MMP_BOOL *bSupport)
{
    *bSupport = gbUVCRecdSupport;
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_StartUVCRecd
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Start UVC video recording.

 Start to save the 3GP file.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPS_3GPRECD_StartUVCRecd(MMP_UBYTE type)
{
	return MMPD_3GPMGR_StartUVCRecd(type);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_OpenUVCRecdFile
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Open UVC video recording file.

 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPS_3GPRECD_OpenUVCRecdFile(void)
{
	MMP_BOOL bStatus;
    #if (SUPPORT_USB_HOST_FUNC)
    MMPF_USBH_GetUVCPrevwSts(&bStatus);
    if (bStatus==MMP_FALSE) {
        RTNA_DBG_Str(0, "Enter preview first!\r\n");
        return MMP_USB_ERR_UNSUPPORT_MODE;
    }

    MMPF_USBH_GetUVCRecdSts(&bStatus);
    if(bStatus==MMP_TRUE)
    {
    	RTNA_DBG_Str(0, "[WARN]: RECDING \r\n");
        return MMP_USB_ERR_UNSUPPORT_MODE;
    }


#if (UVC_VIDRECD_SUPPORT == 1 && VIDRECD_MULTI_TRACK == 0)
    {
    	extern MMP_ULONG m_VidRecdID;
		if (MMPF_MP4VENC_GetStatus(m_VidRecdID) != MMPF_MP4VENC_FW_STATUS_PREENCODE) {
			MMPD_3GPMGR_UVCOpenFile();
		}
    }
#endif
	#endif
	return MMP_ERR_NONE;
}
//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_EnableUVCRecd
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Enable UVC video recording.

 Enable to inform USBH, and driver will start recording later when I-frame received.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPS_3GPRECD_EnableUVCRecd(void)
{
	return MMPD_3GPMGR_EnableUVCRecd();
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_StopUVCRecd
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Stop UVC video recording.

 Stop UVC recording and save the 3GP file.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPS_3GPRECD_StopUVCRecd(void)
{
	return MMPD_3GPMGR_StopUVCRecd();
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_UVCRecdInputFrame
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief input frame for UVC video recording.

 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPS_3GPRECD_UVCRecdInputFrame(MMP_ULONG bufaddr, MMP_ULONG size, MMP_ULONG timestamp, 
								       MMP_USHORT frametype, MMP_USHORT vidaudtype)
{	
	return MMPD_3GPMGR_UVCRecdInputFrame(bufaddr, size, timestamp, frametype, vidaudtype);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetUVCRecdInfo
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Set UVC video record information.

 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPS_3GPRECD_SetUVCRecdInfo(MMPS_3GPRECD_CONTAINER_INFO *pInfo)
{
	MMP_USHORT  usFileType = MMP_3GPRECD_FILETYPE_UVCRECD;
	
	// Set video encode format
	if (pInfo->VideoEncodeFormat == MMPS_3GPRECD_VIDEO_FORMAT_H264) {
		MMPD_3GPMGR_SetEncodeFormat(usFileType, MMPD_MP4VENC_FORMAT_H264);
	}
	
	// Set video encode resolution
	MMPD_3GPMGR_SetEncodeResolution(usFileType, pInfo->ulFrameWidth, pInfo->ulFrameHeight);
	
	// Set video encode frame rate
	MMPD_3GPMGR_SetFrameRate(usFileType, pInfo->ulTimeResolution, pInfo->ulTimeIncrement);
	
	// Set video encode GOP size
	MMPD_3GPMGR_SetEncodeGOP(usFileType, pInfo->usPFrameCount, pInfo->usBFrameCount);
	
	// Set sps, pps header
	MMPD_3GPMGR_SetEncodeSPSPPSHdr(usFileType, pInfo->ulSPSAddr, pInfo->ulSPSSize, pInfo->ulPPSAddr, 
								   pInfo->ulPPSSize, pInfo->ulProfileIDC, pInfo->ulLevelIDC);
	
	return MMP_ERR_NONE;
}
#endif

#if (SUPPORT_USB_HOST_FUNC)
//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_StartUVCPrevw
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Start UVC display preview mode, include stream on(UVC VS_COMMIT_CONTROL).
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS Unsupport resolution.
*/
MMP_ERR MMPS_3GPRECD_StartUVCPrevw(void)
{
	return MMPD_3GPMGR_StartUVCPrevw();
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_StopUVCPrevw
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Stop UVC display preview mode, and stream off(UVC CLEAR_FEATURE).
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS Unsupport resolution.
*/
MMP_ERR MMPS_3GPRECD_StopUVCPrevw(void)
{
	return MMPD_3GPMGR_StopUVCPrevw();
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetUVCDisplayWInID
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set UVC display Window ID.
 @param[in] ubWinID the display window ID of UVC YUV stream used.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS Unsupport resolution.
*/
MMP_ERR MMPS_3GPRECD_SetUVCPrevwWinID(MMP_UBYTE ubWinID)
{
	return MMPD_3GPMGR_SetUVCPrevwWinID(ubWinID);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetUVCPrevwRote
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set UVC preview rotate.
 @param[in] ubRoteType rotate type of UVC preview.
 @retval MMP_ERR_NONE Success.
 @return It reports the status of the operation.
*/
MMP_ERR MMPS_3GPRECD_SetUVCPrevwRote(MMP_GRAPHICS_ROTATE_TYPE ubRoteType)
{
	return MMPD_3GPMGR_SetUVCPrevwRote(ubRoteType);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetUVCRecdResol
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set UVC H264 encoded resolution.
 @param[in] usResol Resolution for UVC H264 record video.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS Unsupport resolution.
*/
MMP_ERR MMPS_3GPRECD_SetUVCRecdResol(MMP_UBYTE ubResol)
{
    return MMPD_3GPMGR_SetUVCRecdResol(ubResol, 
                                       MMPS_3GPRECD_GetConfig()->usEncWidth[ubResol], 
                                       MMPS_3GPRECD_GetConfig()->usEncHeight[ubResol]);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetUVCRecdBitrate
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set UVC encoded quality.
 @param[in] ulBitrate Bitrate of UVC record video.
 @retval MMP_ERR_NONE Success.
 @note It must be set after choosing resolution and format.
 Here are recommended bitrate settings, "@30FPS", for reference

    Resolution          Visual Quality Level        Bitrate(bits/sec)
    ======================================================================
    640x480             Low                         500000
                        Mid                         750000
                        High                        1000000

    1280x720            Low                         2000000
                        Mid                         4000000
                        High                        8000000

    1920x1088           Low                         4000000
                        Mid                         8000000
                        High                        16000000
*/
MMP_ERR MMPS_3GPRECD_SetUVCRecdBitrate(MMP_ULONG ulBitrate)
{
	return MMPD_3GPMGR_SetUVCRecdBitrate(ulBitrate);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetUVCRecdFrameRate
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set frame rate for UVC H264 recorded video.
 @param[in] usFrameRate Frame rate for UVC H264 record video.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetUVCRecdFrameRate(MMP_USHORT usTimeIncrement, MMP_USHORT usTimeIncrResol)
{
	return MMPD_3GPMGR_SetUVCRecdFrameRate(usTimeIncrement, usTimeIncrResol);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetUVCRecdPFrameCount
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set P frame count of one cycle.
 @param[in] usFrameCount Count of P frame.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetUVCRecdPFrameCount(MMP_USHORT usFrameCount)
{
	return MMPD_3GPMGR_SetUVCRecdPFrameCount(usFrameCount);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetUVCPrevwResol
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set UVC YUV encoded resolution.
 @param[in] usWidth Resolution width for UVC YUV preview video.
 @param[in] usHeight Resolution height for UVC YUV preview video.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS Unsupport resolution.
*/
MMP_ERR MMPS_3GPRECD_SetUVCPrevwResol(MMP_USHORT usWidth, MMP_USHORT usHeight)
{
    return MMPD_3GPMGR_SetUVCPrevwResol(usWidth,usHeight);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetUVCPrevwFrameRate
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set frame rate for UVC YUV preview video.
 @param[in] ubFps Frame rate for UVC YUV preview video.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_3GPRECD_SetUVCPrevwFrameRate(MMP_UBYTE ubFps)
{
	return MMPD_3GPMGR_SetUVCPrevwFrameRate(ubFps);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_AddDevCFG
//  Description :
//------------------------------------------------------------------------------
/**
@brief The function sets UVC device following configs.
@param[in] pubStr Device Chip info strings.
@param[in] pOpenDevCallback The callback to be executed when USB device connected, one part of prob.
@param[in] pStartDevCallback The callback to be executed when UVC device commit(one part of commit).
@param[in] pNaluInfo The H264 nalu info, mejors are sps/pps related.
@return It reports the status of the operation.
*/
MMP_ERR MMPS_3GPRECD_AddDevCFG(MMP_UBYTE *pubStr, void *pOpenDevCallback, void *pStartDevCallback, void *pNaluInfo)
{
    return MMPD_3GPMGR_AddDevCFG(pubStr, pOpenDevCallback, pStartDevCallback, pNaluInfo);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_UpdDevCFG
//  Description :
//------------------------------------------------------------------------------
/**
@brief The function sets to update UVC device following configs.
@param[in] Event Operation event as update open CV, start CB, and nalu table.
@param[in] pubStr Device Chip info strings.
@param[in] pParm The parameters to be updated info.
@return It reports the status of the operation.
*/
MMP_ERR MMPS_3GPRECD_UpdDevCFG(MMPS_UVCRECD_UPD_UVC_CFG_OP Event, MMP_UBYTE *pubStr, void *pParm)
{
    MMPD_3GPMGR_UPD_UVC_CFG_OP e;
    
    switch (Event) {
    case MMPS_UVCRECD_UPD_OPEN_UVC_CB:
        e = MMPD_3GPMGR_UPD_OPEN_UVC_CB;
        break;
    case MMPS_UVCRECD_UPD_START_UVC_CB:
        e = MMPD_3GPMGR_UPD_START_UVC_CB;
        break;
    case MMPS_UVCRECD_UPD_NALU_TBL:
        e = MMPD_3GPMGR_UPD_NALU_TBL;
        break;
    default:
        e = MMPD_3GPMGR_UPD_OPEN_UVC_CB;
        break;
    }
    
    return MMPD_3GPMGR_UpdDevCFG(e, pubStr, pParm);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_RegDevAddiCFG
//  Description :
//------------------------------------------------------------------------------
/**
@brief The function sets to register UVC device info configs.
@param[in] ulRegTyp Register info type.
@param[in] pubStr Device Chip info strings.
@param[in] ulParm0 The parameter 0 to be registered.
@param[in] ulParm1 The parameter 1 to be registered.
@param[in] ulParm2 The parameter 2 to be registered.
@param[in] ulParm3 The parameter 3 to be registered.
@return It reports the status of the operation.
*/
MMP_ERR MMPS_3GPRECD_RegDevAddiCFG(MMP_ULONG ulRegTyp, MMP_UBYTE *pubStr, MMP_ULONG ulParm0, MMP_ULONG ulParm1, MMP_ULONG ulParm2, MMP_ULONG ulParm3)
{
    return MMPD_3GPMGR_RegDevAddiCFG(ulRegTyp, pubStr, ulParm0,ulParm1,ulParm2,ulParm3);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetClassIfCmd
//  Description :
//------------------------------------------------------------------------------
/**
@brief Set UVC class IF command.
@param[in] bReq SETUP field bRequest.
@param[in] wVal SETUP field wValue.
@param[in] wInd SETUP field wIndex.
@param[in] wLen SETUP field wLength.
@param[in] UVCDataBuf received data.
@return It reports the status of the operation.
*/
MMP_ERR MMPS_3GPRECD_SetClassIfCmd(MMP_UBYTE bReq, MMP_USHORT wVal, MMP_USHORT wInd, MMP_USHORT wLen, MMP_UBYTE *UVCDataBuf)
{
    return MMPD_3GPMGR_SetClassIfCmd(bReq, wVal, wInd, wLen, UVCDataBuf);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetClassIfCmd
//  Description :
//------------------------------------------------------------------------------
/**
@brief Get UVC class IF command.
@param[in] bReq SETUP field bRequest.
@param[in] wVal SETUP field wValue.
@param[in] wInd SETUP field wIndex.
@param[in] wLen SETUP field wLength.
@param[in] UVCDataLength exact received data length.
@param[in] UVCDataBuf received data.
@return It reports the status of the operation.
*/
MMP_ERR MMPS_3GPRECD_GetClassIfCmd(MMP_UBYTE bReq, MMP_USHORT wVal, MMP_USHORT wInd, MMP_USHORT wLen, MMP_ULONG *UVCDataLength, MMP_UBYTE *UVCDataBuf)
{
    return MMPD_3GPMGR_GetClassIfCmd(bReq, wVal, wInd, wLen, UVCDataLength, UVCDataBuf);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_SetStdIfCmd
//  Description :
//------------------------------------------------------------------------------
/**
@brief Set UVC standard IF command.
@param[in] bReq SETUP field bRequest.
@param[in] wVal SETUP field wValue.
@param[in] wInd SETUP field wIndex.
@param[in] wLen SETUP field wLength.
@param[in] UVCDataBuf received data.
@return It reports the status of the operation.
*/
MMP_ERR MMPS_3GPRECD_SetStdIfCmd(MMP_UBYTE bReq, MMP_USHORT wVal, MMP_USHORT wInd, MMP_USHORT wLen, MMP_UBYTE *UVCDataBuf)
{
    return MMPD_3GPMGR_SetStdIfCmd(bReq, wVal, wInd, wLen, UVCDataBuf);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_3GPRECD_GetStdDevCmd
//  Description :
//------------------------------------------------------------------------------
/**
@brief Get UVC standard DEV command.
@param[in] bReq SETUP field bRequest.
@param[in] wVal SETUP field wValue.
@param[in] wInd SETUP field wIndex.
@param[in] wLen SETUP field wLength.
@param[in] UVCDataLength exact received data length.
@param[in] UVCDataBuf received data.
@return It reports the status of the operation.
*/
MMP_ERR MMPS_3GPRECD_GetStdDevCmd(MMP_UBYTE bReq, MMP_USHORT wVal, MMP_USHORT wInd, MMP_USHORT wLen, MMP_ULONG *UVCDataLength, MMP_UBYTE *UVCDataBuf)
{
    return MMPD_3GPMGR_GetStdDevCmd(bReq, wVal, wInd, wLen, UVCDataLength, UVCDataBuf);
}
#endif

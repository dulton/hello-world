/// @ait_only
/**
 @file mmpd_3gpmgr.c
 @brief Retina 3GP Merger Control Driver Function
 @author Will Tseng
 @version 1.0
*/

/** @addtogroup MMPD_3GPMGR
 *  @{
 */

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "mmpd_3gpmgr.h"
#include "mmpd_mp4venc.h"
#include "mmpd_fs.h"
#include "mmph_hif.h"
#include "mmp_reg_video.h"
#include "mmp_reg_gbl.h"
#include "ait_utility.h"
#include "mmpf_3gpmgr.h"
#include "lib_retina.h"

#if (SUPPORT_USB_HOST_FUNC)
#include "mmpf_usbh_uvc.h"
#include "mmpf_usbh_ctl.h"
#endif

//==============================================================================
//
//                              GLOBAL VARIABLE
//
//==============================================================================

static MMP_UBYTE    m_ub3gpMgrStoragePath;  ///< save card mode or memory mode
static MMP_BOOL     m_bAVSyncEncode;        ///< encode with audio
static MMP_ULONG	m_ulTempFileNameAddr;   ///< temp address for file name
static MMP_ULONG    m_ulTempFileNameBufSize;///< size of temp buf for file name

static MMPD_3GPMGR_AUDIO_FORMAT m_3gpAudioMode; ///< audio mode

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_PreCapture
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Pre-capture audio/video without saving
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_PreCapture(MMP_ULONG ulEncId, MMP_ULONG ulPreCaptureMs)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulEncId);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, ulPreCaptureMs);
    
    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_PRECAPTURE);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_StartCapture
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Start capture audio/video
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_StartCapture(MMP_ULONG ubEncId, MMP_USHORT filetype)
{
	MMP_ERR status = MMP_ERR_NONE;
	
	if (filetype == MMP_3GPRECD_FILETYPE_VIDRECD) {
    	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
		MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ubEncId);
		MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_START);
		status = MMPH_HIF_GetParameterL(GRP_IDX_VID, 4);
    	MMPH_HIF_ReleaseSem(GRP_IDX_VID);
	}
	#if (DUALENC_SUPPORT == 1)
    else if (filetype == MMP_3GPRECD_FILETYPE_DUALENC) {
		MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
		MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ubEncId);
		MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_DUALENC_START);
		status = MMPH_HIF_GetParameterL(GRP_IDX_VID, 4);
		MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    }
	#endif
	
    return status;
}
#if (DUALENC_SUPPORT)
//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_StartAllCapture
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Start all encoder audio/video together.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_StartAllCapture(MMP_UBYTE ubTotalEncCnt, MMP_ULONG *pEncID)
{
	MMP_ERR status = MMP_ERR_NONE;
	MMP_ERR dualstatus = MMP_ERR_NONE;

	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ubTotalEncCnt);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, pEncID[0]);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 8, pEncID[1]);
	
	MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_ALL_START);
	status = MMPH_HIF_GetParameterL(GRP_IDX_VID, 4);
	dualstatus = MMPH_HIF_GetParameterL(GRP_IDX_VID, 4);
	MMPH_HIF_ReleaseSem(GRP_IDX_VID);
	if (status != MMP_ERR_NONE)
	    return status;
	if (dualstatus != MMP_ERR_NONE)
	    return dualstatus; 
	return MMP_ERR_NONE;   
	    
}
#endif
//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_StopCapture
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Stop capture audio/video
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_StopCapture(MMP_ULONG ubEncId, MMP_USHORT filetype)
{
    if (filetype == MMP_3GPRECD_FILETYPE_VIDRECD) {
        MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
        MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ubEncId);
        MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_STOP);
        MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    }
    #if (DUALENC_SUPPORT == 1)
    else if (filetype == MMP_3GPRECD_FILETYPE_DUALENC) {
        MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
        MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ubEncId);
        MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_DUALENC_STOP);
        MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    }
    #endif
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_PauseCapture
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Pause capture audio/video
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_PauseCapture(void)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_PAUSE);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_ResumeCapture
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Resume capture audio/video
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_ResumeCapture(void)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_RESUME);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetContainerType
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_3GPMGR_SetContainerType(MMPD_3GPMGR_CONTAINER type)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterB(GRP_IDX_VID, 0, type);
    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | SET_CONTAINER_TYPE);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetRecordSpeed
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_3GPMGR_SetRecordSpeed(MMPD_3GPMGR_SPEED_MODE SpeedMode, MMPD_3GPMGR_SPEED_RATIO SpeedRatio)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterB(GRP_IDX_VID, 0, SpeedMode);
    MMPH_HIF_SetParameterB(GRP_IDX_VID, 1, SpeedRatio);

    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | ENCODE_SPEED_CTL);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetStoragePath
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Save encoded 3GP file to card mode or memory mode.
 @param[in] ubEnable Enable to card or disable to memory.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_SetStoragePath(MMP_UBYTE ubEnable)
{
    m_ub3gpMgrStoragePath = ubEnable;
    
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterB(GRP_IDX_VID, 0, m_ub3gpMgrStoragePath);

    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | ENCODE_STORAGE_PATH);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_GetStoragePath
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Check encoded 3GP file if card mode or memory mode.
 @return Card or memory mode.
*/
MMP_UBYTE MMPD_3GPMGR_GetStoragePath(void)
{
    return m_ub3gpMgrStoragePath;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetFileName
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Send encoded 3GP file name to firmware for card mode.
 @param[in] bFileName File name.
 @param[in] usLength Length of file name in unit of byte.
 @retval MMP_ERR_NONE Success.
 @note It just use video compressed buffer to store file name. Then it should be set after MMPD_3GPMGR_SetEncodeCompBuf.
*/
MMP_ERR MMPD_3GPMGR_SetFileName(MMP_BYTE bFileName[], MMP_USHORT ulFileNameLength, MMP_USHORT usFileType)
{
	MMP_UBYTE	null = 0;

    if (ulFileNameLength > m_ulTempFileNameBufSize)
        return MMP_3GPMGR_ERR_UNSUPPORT;

    MMPH_HIF_MemCopyHostToDev(m_ulTempFileNameAddr, (MMP_UBYTE *)bFileName, ulFileNameLength);
    MMPH_HIF_MemCopyHostToDev(m_ulTempFileNameAddr + ulFileNameLength, (MMP_UBYTE*)&null, 1);
    MMPH_HIF_MemCopyHostToDev(m_ulTempFileNameAddr + ulFileNameLength + 1, (MMP_UBYTE*)&null, 1);

    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, m_ulTempFileNameAddr);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, ulFileNameLength);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 8, usFileType);

    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | ENCODE_FILE_NAME);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetUserDataAtom
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Send user data atom to firmware for card mode.
 @param[in] UserDataBuf user data atom buffer.
 @param[in] UserDataLength Length of user data atom.
 @param[in] usFileType file type.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_SetUserDataAtom(MMP_BYTE AtomName[], MMP_BYTE UserDataBuf[], MMP_USHORT UserDataLength, MMP_USHORT usFileType)
{
	MMP_UBYTE	null = 0;
	MMP_UBYTE 	*ptr;
	MMP_ULONG   AtomLen;
	MMP_LONG    i;
	
	AtomLen = UserDataLength + 8;
    if (AtomLen > m_ulTempFileNameBufSize)
        return MMP_3GPMGR_ERR_UNSUPPORT;

	ptr = (MMP_UBYTE *)m_ulTempFileNameAddr;
	for (i = (4-1); i >= 0; i--) {
        *ptr++ = (MMP_UBYTE)((AtomLen >> (i << 3)) & 0xFF);
    }
	
	MMPH_HIF_MemCopyHostToDev(m_ulTempFileNameAddr + 4, (MMP_UBYTE *)AtomName, 4);
    MMPH_HIF_MemCopyHostToDev(m_ulTempFileNameAddr + 8, (MMP_UBYTE *)UserDataBuf, UserDataLength);
    UserDataLength += 8;
    MMPH_HIF_MemCopyHostToDev(m_ulTempFileNameAddr + UserDataLength, (MMP_UBYTE*)&null, 1);
    MMPH_HIF_MemCopyHostToDev(m_ulTempFileNameAddr + UserDataLength + 1, (MMP_UBYTE*)&null, 1);

    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, m_ulTempFileNameAddr);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, UserDataLength);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 8, usFileType);

    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | SET_USER_DATA_ATOM);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_EnableAVSyncEncode
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Set video encode with audio or not.
 @param[in] bEnable Enable with audio or disable without audio.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_EnableAVSyncEncode(MMP_BOOL bEnable)
{
	m_bAVSyncEncode = bEnable;
	
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterB(GRP_IDX_VID, 0, (MMP_UBYTE)m_bAVSyncEncode);

    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | AUDIO_ENCODE_CTL);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_GetAVSyncEncode
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Get video encode with audio or not.
 @retval Enable with audio or disable without audio.
*/
MMP_BOOL MMPD_3GPMGR_GetAVSyncEncode(void)
{
	return m_bAVSyncEncode;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetAudioParam
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Set audio parameter.
 @param[in] param Audio parameter.
 @param[in] AudioMode Audio operation mode.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_SetAudioParam(  MMP_ULONG                   param,
                                    MMPD_3GPMGR_AUDIO_FORMAT    AudioMode)
{
    MMP_ULONG   ulParam, ret, mode;
    MMP_ERR	    err;

    ulParam         = param;
    m_3gpAudioMode  = AudioMode;

    if (AudioMode == MMPD_3GPMGR_AUDIO_FORMAT_AMR) {
        mode = AUDIO_AMR_MODE;
    }
    else {
        switch(AudioMode) {
        case MMPD_3GPMGR_AUDIO_FORMAT_AAC:
            mode = AUDIO_AAC_MODE;
            break;
        case MMPD_3GPMGR_AUDIO_FORMAT_ADPCM:
            mode = AUDIO_ADPCM_MODE;
            break;
        case MMPD_3GPMGR_AUDIO_FORMAT_MP3:
            mode = AUDIO_MP3_MODE;
            break;
        case MMPD_3GPMGR_AUDIO_FORMAT_PCM:
            mode = AUDIO_PCM_MODE;
            break;
        default:
            return MMP_3GPMGR_ERR_PARAMETER;
        }
    }

    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);

    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulParam);
    err = MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | mode);
    ret = MMPH_HIF_GetParameterL(GRP_IDX_VID, 4);

    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

    if (err)
        return err;
    else if (ret)
        return MMP_3GPMGR_ERR_PARAMETER;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_GetAudioFormat
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Return audio format.
 @retval MMPD_3GPMGR_AUDIO_FORMAT_AAC AAC.
 @retval MMPD_3GPMGR_AUDIO_FORMAT_AMR AMR.
 @retval MMPD_3GPMGR_AUDIO_FORMAT_ADPCM ADPCM.
 @retval MMPD_3GPMGR_AUDIO_FORMAT_MP3 MP3.
 @retval MMPD_3GPMGR_AUDIO_FORMAT_PCM raw PCM.
*/
MMPD_3GPMGR_AUDIO_FORMAT MMPD_3GPMGR_GetAudioFormat(void)
{
	return m_3gpAudioMode;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetEncodeCompBuf
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Set start address and size of firmware compressed buffer.
 @param[in] *BufInfo Pointer of encode buffer structure.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_SetEncodeCompBuf(   MMPD_3GPMGR_AVCOMPRESSEDBUFFER  *BufInfo,
                                        MMP_3GPRECD_FILETYPE            filetype)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0,  BufInfo->ulVideoCompBufStart);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4,  BufInfo->ulVideoCompBufEnd -
                                            BufInfo->ulVideoCompBufStart);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 8,  BufInfo->ulAudioCompBufStart);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 12, BufInfo->ulAudioCompBufEnd -
                                            BufInfo->ulAudioCompBufStart);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 16, filetype);

    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | COMPRESS_BUF_ADDR);  
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

    return MMP_ERR_NONE;   
}

//#if (VIDRECD_REFIXRECD_SUPPORT == 1)
//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetTempBuf2FixedTailInfo
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Set start address and size of temp buffer for fixed-tail-info record.
 @param[in] addr address of temp buffer.
 @param[in] size size of temp buffer.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_SetTempBuf2FixedTailInfo(MMP_ULONG tempaddr, MMP_ULONG tempsize, MMP_ULONG AVaddr, MMP_ULONG AVsize, MMP_ULONG Reseraddr, MMP_ULONG Resersize)
{
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, tempaddr);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, tempsize);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 8, AVaddr);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 12, AVsize);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 16, Reseraddr);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 20, Resersize);
    
    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | SET_REFIXRECD_BUFFER);  
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
	return MMP_ERR_NONE;  
}
//#endif

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_Set3GPCreateModifyTimeInfo
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Set creation time and modification time of 3GP file.
 @param[in] addr address of temp buffer.
 @param[in] size size of temp buffer.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_Set3GPCreateModifyTimeInfo(MMP_3GPRECD_FILETYPE filetype, MMP_ULONG CreateTime, MMP_ULONG ModifyTime)
{
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, filetype);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, CreateTime);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 8, ModifyTime);
    
    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | SET_3GPMUX_TIMEATOM);  
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_ModifyAVIListAtom
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Modify LIST Atom for AVI format.

 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_ModifyAVIListAtom(MMP_BOOL bEnable, MMP_BYTE *pStr)
{
	MMP_ULONG  ultmp;
	
	ultmp = (MMP_ULONG)*pStr | ((MMP_ULONG)*(pStr+1) << 8) | ((MMP_ULONG)*(pStr+2) << 16) 
			| ((MMP_ULONG)*(pStr+3) << 24);

	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, bEnable);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, ultmp);
    
    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | MODIFY_AVI_LIST_ATOM);  
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetRepackMiscBuf
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Set start address and size of firmware AV repack buffer.

 This buffer is for packing video and audio bitstream sequentially.
 @param[in] repackbuf information of AV repack buffer, such as start address/size,
 sync buffer address, buffer address/size for frame size, and buffer address/size for frame time.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_SetRepackMiscBuf(MMPD_3GPMGR_REPACKBUF *repackbuf, MMP_3GPRECD_FILETYPE filetype)
{
	MMP_ERR	status;

	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, repackbuf->ulAvRepackStartAddr);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, repackbuf->ulAvRepackSize);
    MMPH_HIF_SetParameterB(GRP_IDX_VID, 8, filetype);
    
    status = MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | AV_REPACK_BUF);
    if (status != MMP_ERR_NONE) {
        MMPH_HIF_ReleaseSem(GRP_IDX_VID);
        return MMP_SYSTEM_ERR_CMDTIMEOUT;
    }
    
    if (filetype == MMP_3GPRECD_FILETYPE_VIDRECD) {
        MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, repackbuf->ulVideoEncSyncAddr);
        status = MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | AV_SYNC_BUF_INFO);
    }

    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, repackbuf->ulVideoSizeTableAddr);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 4, repackbuf->ulVideoSizeTableSize);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 6, MMPD_3GPMGR_AUX_FRAME_TABLE);
    MMPH_HIF_SetParameterB(GRP_IDX_VID, 8, filetype);
    status = MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | AUX_TABLE_ADDR);

    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, repackbuf->ulVideoTimeTableAddr);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 4, repackbuf->ulVideoTimeTableSize);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 6, MMPD_3GPMGR_AUX_TIME_TABLE);
    MMPH_HIF_SetParameterB(GRP_IDX_VID, 8, filetype);
    status = MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | AUX_TABLE_ADDR);

    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetGOP
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Set P frame count of one cycle.
 @param[in] ulFrameCnt Count of P frame.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_SetGOP(MMP_USHORT usPFrame, MMP_USHORT usBFrame, MMP_USHORT usfiletype)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 0, usPFrame);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 2, usBFrame);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, usfiletype);
    
    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | GOP_FRAME_TYPE);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_Get3gpFileCurSize
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Get current size of encoded 3GP file.

 This size only counts file header and current AV bitstream.
 @param[out] ulCurSize Current file size.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_Get3gpFileCurSize(MMP_ULONG *ulCurSize)
{
	MMP_ULONG	status;

    *ulCurSize = 0;
    if (m_ub3gpMgrStoragePath) {
        MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
        status = MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | GET_3GP_DATA_RECODE);
		
		if (status == MMP_ERR_NONE) {
			*ulCurSize = MMPH_HIF_GetParameterL(GRP_IDX_VID, 0);
		}
        MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_Get3gpSaveStatus
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Check if encoded 3GP file is saved to card or not.
 @param[out] status File saving status.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_Get3gpSaveStatus(MMP_UBYTE *status)
{
	if (m_ub3gpMgrStoragePath) {
	    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
		MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | GET_3GP_FILE_STATUS);
		*status = MMPH_HIF_GetParameterB(GRP_IDX_VID, 0);
		MMPH_HIF_ReleaseSem(GRP_IDX_VID);
	}
    else {
	    *status = MMP_FALSE;
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_Get3gpFileSize
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Get the final size of encoded 3GP file.

 This size should be counted after finishing 3GP file tail filling.
 @param[out] filesize Final 3GP file size.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_Get3gpFileSize(MMP_ULONG *filesize)
{
	MMP_ULONG 	ulFileSize;

    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | GET_3GP_FILE_SIZE);
    ulFileSize = MMPH_HIF_GetParameterL(GRP_IDX_VID, 0);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

	*filesize = ulFileSize;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_GetRecordingTime
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Get audio recording time in unit of ms.
 @param[out] ulTime Recording time.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_GetRecordingTime(MMP_ULONG *ulTime, MMP_ULONG ulFileType)
{	
#if 0
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulFileType);
	
    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | GET_RECORDING_TIME);

    *ulTime = MMPH_HIF_GetParameterL(GRP_IDX_VID, 0);
        
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);    
#else
    MMPF_3GPMGR_GetRecordingTime(ulTime, ulFileType);
#endif        
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_GetRecordingDuration
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Get current video duration in unit of ms.
 @param[out] ulTime Recording duration.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_GetRecordingDuration(MMP_ULONG *ulTime, MMP_ULONG ulFileType)
{	
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulFileType);
    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | GET_RECORD_DURATION);
    *ulTime = MMPH_HIF_GetParameterL(GRP_IDX_VID, 0);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);    

	return MMP_ERR_NONE;
}

/**
 @brief Get recording time offset in unit of ms.
 @param[out] ulTime Recording time offset.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_GetRecordingOffset(MMP_ULONG *ulTime, MMP_ULONG ulFileType)
{

	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulFileType);
	
	MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | GET_RECORDING_OFFSET);

    *ulTime = MMPH_HIF_GetParameterL(GRP_IDX_VID, 0);
        
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);    
        
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetFileLimit
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Set the maximum 3GP file size for video encoding.
 @param[in] ulFileMax Maximum file size.
 @param[in] ulFileMin Minimum file size.
 @param[out] ulSpace Available space size for recording.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_SetFileLimit(MMP_ULONG ulFileMax, MMP_ULONG ulReserved, MMP_ULONG *ulSpace)
{	
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulFileMax);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, ulReserved);
    
    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | AV_FILE_LIMIT);

    if (m_ub3gpMgrStoragePath) {
	    *ulSpace = MMPH_HIF_GetParameterL(GRP_IDX_VID, 4);
	}
	else {
		*ulSpace = ulFileMax;
	}
	
	MMPH_HIF_ReleaseSem(GRP_IDX_VID);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetTimeLimit
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Set the maximum 3GP file time for video encoding.
 @param[in] ulTimeMax Maximum file time in unit of ms.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_SetTimeLimit(MMP_ULONG ulTimeMax)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);

	if (ulTimeMax > 0x7fffffff) {
	    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, 0x7fffffff);
	}
	else {
	    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulTimeMax);
	}
	
    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | AV_TIME_LIMIT);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetTimeDynamicLimit
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Change the maximum 3GP file time for current video encoding.
 @param[in] ulTimeMax Maximum file time.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_SetTimeDynamicLimit(MMP_ULONG ulTimeMax)
{
	MMP_BOOL bCheck;
	
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	
	if (ulTimeMax > 0x7fffffff) {
	    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, 0x7fffffff);
	}
	else {
	    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulTimeMax);
	}
    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | AV_TIME_DYNALIMIT);
        
    bCheck = MMPH_HIF_GetParameterL(GRP_IDX_VID, 0);	
    
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    if (bCheck == MMP_TRUE)
    	return MMP_ERR_NONE;
    else	
        return MMP_HIF_ERR_PARAMETER;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetSeamless
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Set seamless recording mode enable or not
 @param[in] enable Enable or disable seamless recording.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_SetSeamless(MMP_BOOL enable)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterB(GRP_IDX_VID, 0, enable);
    
    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | SEAMLESS_MODE);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_GetStatus
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Check the firmware merger engine status.
 @param[out] status Firmware merger engine status.
 @retval MMP_ERR_NONE Success.
 @note

 The parameter @a status can not be changed because it sync with the firmware merger engine
 status definitions. It can be
 - 0x0000 MMP_ERR_NONE
 - 0x0001 MMP_FS_ERR_OPEN_FAIL
 - 0x0002 MMP_FS_ERR_TARGET_NOT_FOUND
 - 0x0003 MMP_3GPMGR_ERR_HOST_CANCEL_SAVE
 - 0x0004 MMP_3GPMGR_ERR_MEDIA_FILE_FULL
*/
MMP_ERR MMPD_3GPMGR_GetStatus(MMP_ERR *status, MMP_ULONG *tx_status)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);

    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | GET_MERGER_STATUS);

    *status     = MMPH_HIF_GetParameterL(GRP_IDX_VID, 0);
    *tx_status  = MMPH_HIF_GetParameterL(GRP_IDX_VID, 4);

    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_GetEmergentRecStatus
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Check the firmware merger engine emergent record status.
 @param[out] enable    If emergent record is enable
 @param[out] recording If emergent record is under recording
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_GetEmergentRecStatus(MMP_BOOL *bEnable, MMP_BOOL *bRecording)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);

    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | GET_EMERG_REC_STATUS);

    *bEnable    = MMPH_HIF_GetParameterL(GRP_IDX_VID, 0);
    *bRecording = MMPH_HIF_GetParameterL(GRP_IDX_VID, 4);

    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetRecordTailSpeed
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Set video record with high speed to make tail mode.
 @param[in] ubHighSpeedEn Enable high speed mode or not.
 @param[in] ulTailInfoAddress Tail information address.
 @param[in] ulTailInfoSize Tail information size.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_SetRecordTailSpeed( MMP_BOOL                ubHighSpeedEn,
                                        MMP_ULONG               ulTailInfoAddress,
                                        MMP_ULONG               ulTailInfoSize,
                                        MMP_3GPRECD_FILETYPE    filetype)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterB(GRP_IDX_VID, 0, ubHighSpeedEn);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, ulTailInfoAddress);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 8, ulTailInfoSize);
    MMPH_HIF_SetParameterB(GRP_IDX_VID, 12, filetype);
    
    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_TAILSPEEDMODE);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_RegisterCallback
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Set callback function pointer for the specified event
 @param[in] event the specified event
 @param[in] Callback the callback function pointer for the specified event
 @retval MMP_ERR_NONE Success.

 @warn The registered callback will not be auto deleted, please register a NULL
       to delete it.
*/
MMP_ERR MMPD_3GPMGR_RegisterCallback(MMPD_3GPMGR_EVENT event, void *CallBack)
{
    MMP_ULONG e;

    switch (event) {
    case MMPD_3GPMGR_EVENT_MEDIA_FULL:
        e = MMPF_3GPMGR_EVENT_MEDIA_FULL;
        break;
    case MMPD_3GPMGR_EVENT_FILE_FULL:
        e = MMPF_3GPMGR_EVENT_FILE_FULL;
        break;
    case MMPD_3GPMGR_EVENT_LONG_TIME_FILE_FULL:
        e = MMPF_3GPMGR_EVENT_LONG_TIME_FILE_FULL;
        break;
    case MMPD_3GPMGR_EVENT_MEDIA_SLOW:
        e = MMPF_3GPMGR_EVENT_MEDIA_SLOW;
        break;
    case MMPD_3GPMGR_EVENT_SEAMLESS:
        e = MMPF_3GPMGR_EVENT_SEAMLESS;
        break;
    case MMPD_3GPMGR_EVENT_MEDIA_ERROR:
        e = MMPF_3GPMGR_EVENT_MEDIA_ERROR;
        break;
    case MMPD_3GPMGR_EVENT_ENCODE_START:
        e = MMPF_3GPMGR_EVENT_ENCODE_START;
        break;
    case MMPD_3GPMGR_EVENT_ENCODE_STOP:
        e = MMPF_3GPMGR_EVENT_ENCODE_STOP;
        break;
	#if (DUALENC_SUPPORT == 1)
    case MMPD_3GPMGR_EVENT_DUALENCODE_START:
        e = MMPF_3GPMGR_EVENT_DUALENCODE_START;
        break;
    case MMPD_3GPMGR_EVENT_DUALENCODE_STOP:
        e = MMPF_3GPMGR_EVENT_DUALENCODE_STOP;
        break;
	#endif
    case MMPD_3GPMGR_EVENT_POSTPROCESS:
        e = MMPF_3GPMGR_EVENT_POSTPROCESS;
        break;
    case MMPD_3GPMGR_EVENT_BITSTREAM_DISCARD:
        e = MMPF_3GPMGR_EVENT_BITSTREAM_DISCARD;
        break;
    case MMPD_3GPRECD_EVENT_MEDIA_WRITE:
        e = MMPF_3GPMGR_EVENT_MEDIA_WRITE;
        break;
    case MMPD_3GPRECD_EVENT_STREAMCB:
    	e = MMPF_3GPMGR_EVENT_STREAMCB;
    	break;
    case MMPD_3GPRECD_EVENT_EMERGFILE_FULL:
    	e = MMPF_3GPMGR_EVENT_EMERGFILE_FULL;
    	break;
    case MMPD_3GPRECD_EVENT_RECDSTOP_CARDSLOW:
    	e = MMPF_3GPMGR_EVENT_RECDSTOP_CARDSLOW;
    	break;
    case MMPD_3GPRECD_EVENT_APSTOPVIDRECD:
    	e = MMPF_3GPMGR_EVENT_APSTOPVIDRECD;
    	break;
    case MMPD_3GPRECD_EVENT_PREGETTIME_CARDSLOW:
    	e = MMPF_3GPMGR_EVENT_PREGETTIME_CARDSLOW;
    	break;
    case MMPD_3GPRECD_EVENT_UVCFILE_FULL:
    	e = MMPF_3GPMGR_EVENT_UVCFILE_FULL;
    	break;
    case MMPD_3GPRECD_EVENT_COMPBUF_FREE_SPACE:
    	e = MMPF_3GPMGR_EVENT_COMPBUF_FREE_SPACE;
    	break;
    case MMPD_3GPRECD_EVENT_APNEED_STOP_RECD:
    	e = MMPF_3GPMGR_EVENT_APNEED_STOP_RECD;
    	break;
	case MMPD_3GPRECD_EVENT_DUALENC_FILE_FULL:
    	e = MMPF_3GPMGR_EVENT_DUALENC_FILE_FULL;
    	break;
    case MMPD_3GPMGR_EVENT_NONE:
    default:
        e = 0;
        break;
    }

    return MMPF_VIDMGR_RegisterCallback (e, CallBack);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_GetRegisteredCallback
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Get callback function pointer for the specified event
 @param[in] event the specified event
 @param[out] Callback the callback function pointer for the specified event
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_GetRegisteredCallback(MMPD_3GPMGR_EVENT event, void **CallBack)
{
    MMP_ULONG e;

    switch (event) {
    case MMPD_3GPMGR_EVENT_MEDIA_FULL:
        e = MMPF_3GPMGR_EVENT_MEDIA_FULL;
        break;
    case MMPD_3GPMGR_EVENT_FILE_FULL:
        e = MMPF_3GPMGR_EVENT_FILE_FULL;
        break;
    case MMPD_3GPMGR_EVENT_LONG_TIME_FILE_FULL:
        e = MMPF_3GPMGR_EVENT_LONG_TIME_FILE_FULL;
        break;
    case MMPD_3GPMGR_EVENT_MEDIA_SLOW:
        e = MMPF_3GPMGR_EVENT_MEDIA_SLOW;
        break;
    case MMPD_3GPMGR_EVENT_SEAMLESS:
        e = MMPF_3GPMGR_EVENT_SEAMLESS;
        break;
    case MMPD_3GPMGR_EVENT_MEDIA_ERROR:
        e = MMPF_3GPMGR_EVENT_MEDIA_ERROR;
        break;
    case MMPD_3GPMGR_EVENT_ENCODE_START:
        e = MMPF_3GPMGR_EVENT_ENCODE_START;
        break;
    case MMPD_3GPMGR_EVENT_ENCODE_STOP:
        e = MMPF_3GPMGR_EVENT_ENCODE_STOP;
        break;
    case MMPD_3GPMGR_EVENT_POSTPROCESS:
        e = MMPF_3GPMGR_EVENT_POSTPROCESS;
        break;
    case MMPD_3GPRECD_EVENT_EMERGFILE_FULL:
    	e = MMPF_3GPMGR_EVENT_EMERGFILE_FULL;
    	break;
    case MMPD_3GPRECD_EVENT_RECDSTOP_CARDSLOW:
    	e = MMPF_3GPMGR_EVENT_RECDSTOP_CARDSLOW;
    	break;	
    case MMPD_3GPRECD_EVENT_APSTOPVIDRECD:
    	e = MMPF_3GPMGR_EVENT_APSTOPVIDRECD;
    	break;
    case MMPD_3GPRECD_EVENT_PREGETTIME_CARDSLOW:
    	e = MMPF_3GPMGR_EVENT_PREGETTIME_CARDSLOW;
    	break;				
    case MMPD_3GPRECD_EVENT_UVCFILE_FULL:
    	e = MMPF_3GPMGR_EVENT_UVCFILE_FULL;
    	break;
    case MMPD_3GPRECD_EVENT_COMPBUF_FREE_SPACE:
    	e = MMPF_3GPMGR_EVENT_COMPBUF_FREE_SPACE;
    	break;
    case MMPD_3GPRECD_EVENT_APNEED_STOP_RECD:
    	e = MMPF_3GPMGR_EVENT_APNEED_STOP_RECD;
    	break;
	case MMPD_3GPRECD_EVENT_DUALENC_FILE_FULL:
    	e = MMPF_3GPMGR_EVENT_DUALENC_FILE_FULL;
    	break;
    case MMPD_3GPMGR_EVENT_NONE:
    default:
        e = 0;
        break;
    }

    return MMPF_VIDMGR_GetRegisteredCallback(e, CallBack);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetSkipCntThreshold
//  Description : 
//------------------------------------------------------------------------------
/** @brief The function specified the threshold of skip frame counts 
           to define what calleda slow media.
@param[in] threshold number of skip frames happen in 1 sec represents a slow media
@retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_SetSkipCntThreshold(MMP_USHORT threshold)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 0, threshold);
    
    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | SKIP_THRESHOLD);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_MakeRoom
//  Description : 
//------------------------------------------------------------------------------
/** @brief The function make extra room from record storage space for other usage
@param[in] ulRequiredSize specify the size to ask recorder system make room for
@retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_MakeRoom(MMP_ULONG ulEncId, MMP_ULONG ulRequiredSize)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulEncId);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, ulRequiredSize);
    
    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | MAKE_EXTRA_ROOM);
    MMPH_HIF_GetParameterL(GRP_IDX_VID, 4);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_EnableEmergentRecd
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Enable emergent video recording.

 Enable emergent video recording.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPD_3GPMGR_EnableEmergentRecd(MMP_BOOL bEnabled)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SetParameterB(GRP_IDX_VID, 0, bEnabled);
	
    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_EMERGENABLE);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_EnableUVCEmergentRecd
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Enable uvc emergent video recording.

 Enable uvc emergent video recording.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPD_3GPMGR_EnableUVCEmergentRecd(MMP_BOOL bEnabled)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SetParameterB(GRP_IDX_VID, 0, bEnabled);
	
    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_UVCEMERG_ENABLE);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_StartEmergentRecd
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Start emergent video recording.

 Start to save the 3GP file.
 @param[in] bStopVidRecd: stop normal record, keep emergent record
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPD_3GPMGR_StartEmergentRecd(MMP_BOOL bStopVidRecd)
{
    MMP_ERR status;

	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SetParameterB(GRP_IDX_VID, 0, bStopVidRecd);
	MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_EMERGSTART);
    status = MMPH_HIF_GetParameterL(GRP_IDX_VID, 0);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    return status;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_StopEmergentRecd
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Stop emergent video recording.

 Stop to save the 3GP file.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPD_3GPMGR_StopEmergentRecd(void)
{	
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_EMERGSTOP);
	MMPH_HIF_ReleaseSem(GRP_IDX_VID);
	
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetEmergentTimeLimit
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Set the maximum 3GP file time for emergent video encoding.
 @param[in] ulTimeMax Maximum file time in unit of ms.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_SetEmergentTimeLimit(MMP_ULONG ulTimeMax)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);

	if (ulTimeMax > 0x7fffffff) {
	    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, 0x7fffffff);
	}
	else {
	    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulTimeMax);
	}
    MMPH_HIF_SendCmd(GRP_IDX_VID, EMERGFILE_TIME_LIMIT | HIF_VID_CMD_MERGER_PARAMETER);

    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetEmergPreEncTimeLimit
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Set the the pre-encoding time limit.
 @param[in] ulTimeMax Maximum file time.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_SetEmergPreEncTimeLimit(MMP_ULONG ulTimeMax)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulTimeMax);
	
    MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | SET_EMERGENT_PREENCTIME);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetTempFileNameAddr
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Set temp address for filename buffer
 @param[in] addr address
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_SetTempFileNameAddr(MMP_ULONG addr, MMP_ULONG size)
{
    m_ulTempFileNameAddr = addr;
    m_ulTempFileNameBufSize = size;

    return MMP_ERR_NONE;
}

/**
 @brief Enalbe video record skip mode to set total skip count and continuous skip count.

 @param[in] ulTotalCount  - limitation of total skip count
 @param[in] ulContinCount - limitation of continuous skip count
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPD_3GPMGR_SetVidRecdSkipModeParas(MMP_ULONG ulTotalCount, MMP_ULONG ulContinCount)
{
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);	
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulTotalCount);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, ulContinCount);
	
	MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_SKIPMODE_ENABLE);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
	
    return MMP_ERR_NONE;
}

/**
 @brief Enable dual h264 encode and Set type for callback or record .

 @param[in] bEnable  - Enable dual h264 encode
 @param[in] type     - Set type for callback or record
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_SetDualH264Enable(MMP_BOOL bEnable, MMP_ULONG type)
{
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);	
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, bEnable);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, type);
	
	MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_DUALENC_ENABLE);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
	
    return MMP_ERR_NONE;
}

MMP_ERR MMPD_3GPMGR_SetSEIShutterMode(MMP_ULONG ulMode)
{
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);	
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulMode);
	
	MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_SEI_MODE);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
	
    return MMP_ERR_NONE;
}

//#if (UVC_VIDRECD_SUPPORT)
//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_StartUVCRecd
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Start UVC video recording.

 Start to save the 3GP file.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPD_3GPMGR_StartUVCRecd(MMP_UBYTE type)
{	
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SetParameterB(GRP_IDX_VID, 0, type);
	MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_UVCRECD_START);
	MMPH_HIF_ReleaseSem(GRP_IDX_VID);
	
    return MMP_ERR_NONE;
}


MMP_ERR MMPD_3GPMGR_UVCOpenFile(void)
{
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_UVCRECD_OPEN_FILE);
	MMPH_HIF_ReleaseSem(GRP_IDX_VID);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_EnableUVCRecd
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief STart UVC video recording.

 Enable to inform USBH, and driver will start recording later when I-frame received.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPD_3GPMGR_EnableUVCRecd(void)
{
	//MMP_BOOL bStatus;
	MMP_ERR err = MMP_ERR_NONE;
#if (SUPPORT_USB_HOST_FUNC)
	err = MMPF_USBH_SetStartFrameofUVCRecd();
#endif
    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_StopUVCRecd
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Stop UVC video recording.

 Stop to save the 3GP file.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPD_3GPMGR_StopUVCRecd(void)
{	
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_UVCRECD_STOP);
	MMPH_HIF_ReleaseSem(GRP_IDX_VID);
	
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_UVCRecdInputFrame
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief input frame for UVC video recording.

 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPD_3GPMGR_UVCRecdInputFrame(MMP_ULONG bufaddr, MMP_ULONG size, MMP_ULONG timestamp
	, MMP_USHORT frametype, MMP_USHORT vidaudtype)
{	
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, bufaddr);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, size);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 8, timestamp);
	MMPH_HIF_SetParameterW(GRP_IDX_VID, 12, frametype);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 14, vidaudtype);
	
	MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_UVCRECD_INPUTFRAME);
	MMPH_HIF_ReleaseSem(GRP_IDX_VID);
	
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetUVCFBMemory
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Set start address to allocate buffers of UVC preview/record FB.
 @param[in] ulStartAddr the start memory address allowed to be used.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPD_3GPMGR_SetUVCFBMemory(MMP_LONG plStartAddr, MMP_ULONG *plSize)
{	
	MMP_ERR err = MMP_ERR_NONE;
    #if (SUPPORT_USB_HOST_FUNC)
	err = MMPF_USBH_SetFBMemory(plStartAddr, plSize);
    #endif
	
    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_StartUVCPrevw
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Start UVC display preview mode.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPD_3GPMGR_StartUVCPrevw(void)
{	
	MMP_ERR err = MMP_ERR_NONE;
    #if (SUPPORT_USB_HOST_FUNC)
	err = MMPF_USBH_StartUVCPrevw();
    #endif
	
    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_StopUVCPrevw
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Stop UVC display preview mode.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPD_3GPMGR_StopUVCPrevw(void)
{	
	MMP_ERR err = MMP_ERR_NONE;
    #if (SUPPORT_USB_HOST_FUNC)
	err = MMPF_USBH_StopUVCPrevw();
    #endif
	
    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetUVCPrevwWinID
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Set UVC display Window ID.
 @param[in] ubWinID the display window ID of UVC YUV stream used.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPD_3GPMGR_SetUVCPrevwWinID(MMP_UBYTE ubWinID)
{	
	MMP_ERR err = MMP_ERR_NONE;
    #if (SUPPORT_USB_HOST_FUNC)
	err = MMPF_USBH_SetUVCPrevwWinID(ubWinID);
    #endif
	
	return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetUVCPrevwRote
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set UVC preview rotate.
 @param[in] ubRoteType rotate type of UVC preview.
 @retval MMP_ERR_NONE Success.
 @return It reports the status of the operation.
*/
MMP_ERR MMPD_3GPMGR_SetUVCPrevwRote(MMP_GRAPHICS_ROTATE_TYPE ubRoteType)
{
	MMP_ERR err = MMP_ERR_NONE;
    #if (SUPPORT_USB_HOST_FUNC)
	err = MMPF_USBH_SetUVCPrevwRote(ubRoteType);
    #endif
	
	return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetUVCRecdResol
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set UVC H264 encoded resolution.
 @param[in] usResol Resolution for UVC H264 record video.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS Unsupport resolution.
*/
MMP_ERR MMPD_3GPMGR_SetUVCRecdResol(MMP_UBYTE ubResol, MMP_USHORT usWidth, MMP_USHORT usHeight)
{
	MMP_ERR err = MMP_ERR_NONE;
    #if (SUPPORT_USB_HOST_FUNC)
	err = MMPF_USBH_SetUVCRecdResol(ubResol,usWidth,usHeight);
	#endif
	return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetUVCRecdBitrate
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set UVC encoded quality.
 @param[in] ulBitrate Bitrate of UVC record video.
 @retval MMP_ERR_NONE Success.
 @note It must be set after choosing resolution and format.
*/
MMP_ERR MMPD_3GPMGR_SetUVCRecdBitrate(MMP_ULONG ulBitrate)
{
	MMP_ERR err = MMP_ERR_NONE;
    #if (SUPPORT_USB_HOST_FUNC)
	err = MMPF_USBH_SetUVCRecdBitrate(ulBitrate);
	#endif
	return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetUVCRecdFrameRate
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set frame rate for UVC H264 recorded video.
 @param[in] usFrameRate Frame rate for UVC H264 record video.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_SetUVCRecdFrameRate(MMP_USHORT usTimeIncrement, MMP_USHORT usTimeIncrResol)
{
	MMP_ERR err = MMP_ERR_NONE;
    #if (SUPPORT_USB_HOST_FUNC)
	err = MMPF_USBH_SetUVCRecdFrameRate(usTimeIncrement, usTimeIncrResol);
    #endif
    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetUVCRecdPFrameCount
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set P frame count of one cycle.
 @param[in] ubFrameCnt Count of P frame.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_SetUVCRecdPFrameCount(MMP_USHORT usFrameCount)
{
	MMP_ERR err = MMP_ERR_NONE;
    #if (SUPPORT_USB_HOST_FUNC)
	err = MMPF_USBH_SetUVCRecdFrameCount(usFrameCount);
    #endif
    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetUVCPrevwResol
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set UVC YUV encoded resolution.
 @param[in] usWidth Resolution width for UVC YUV preview video.
 @param[in] usHeight Resolution height for UVC YUV preview video.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS Unsupport resolution.
*/
MMP_ERR MMPD_3GPMGR_SetUVCPrevwResol(MMP_USHORT usWidth, MMP_USHORT usHeight)
{
	MMP_ERR err = MMP_ERR_NONE;
    #if (SUPPORT_USB_HOST_FUNC)
	err = MMPF_USBH_SetUVCPrevwResol(usWidth,usHeight);
	#endif
	return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_IsUVCPrevwResolSet
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get UVC preview resolution is set or not.
 @param[in] pubIsSet Get preview video resolution set or not.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_UNSUPPORTED_PARAMETERS Unsupport resolution.
*/
MMP_ERR MMPD_3GPMGR_IsUVCPrevwResolSet(MMP_BOOL *pbIsSet)
{
	MMP_ERR err = MMP_ERR_NONE;
    #if (SUPPORT_USB_HOST_FUNC)
    MMPF_USBH_UVC_STREAM_CFG *pUVCCfg = MMPF_USBH_GetUVCCFG();
    #endif

    *pbIsSet = MMP_FALSE;
    #if (SUPPORT_USB_HOST_FUNC)
	*pbIsSet = pUVCCfg->mPrevw.ubSet;
	#endif
	return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetUVCPrevwFrameRate
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set frame rate for UVC YUV preview video.
 @param[in] ubFps Frame rate for UVC YUV preview video.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_SetUVCPrevwFrameRate(MMP_UBYTE ubFps)
{
	MMP_ERR err = MMP_ERR_NONE;
    #if (SUPPORT_USB_HOST_FUNC)
	err = MMPF_USBH_SetUVCPrevwFrameRate(ubFps);
    #endif
    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_AddDevCFG
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
MMP_ERR MMPD_3GPMGR_AddDevCFG(MMP_UBYTE *pubStr, void *pOpenDevCallback, void *pStartDevCallback, void *pNaluInfo)
{
	MMP_ERR err = MMP_ERR_NONE;
    #if (SUPPORT_USB_HOST_FUNC)
    err = MMPF_USBH_AddDevCFG(pubStr, pOpenDevCallback, pStartDevCallback, pNaluInfo);
    #endif
    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_UpdDevCfg
//  Description :
//------------------------------------------------------------------------------
/**
@brief The function sets to update UVC device following configs.
@param[in] Event Operation event as update open CV, start CB, and nalu table.
@param[in] pubStr Device Chip info strings.
@param[in] pParm The parameters to be updated info.
@return It reports the status of the operation.
*/
MMP_ERR MMPD_3GPMGR_UpdDevCFG(MMPD_3GPMGR_UPD_UVC_CFG_OP Event, MMP_UBYTE *pubStr, void *pParm)
{
#if (SUPPORT_USB_HOST_FUNC)
    MMPF_USBH_UPD_UVC_CFG_OP e;

    switch (Event) {
    case MMPD_3GPMGR_UPD_OPEN_UVC_CB:
        e = MMPF_USBH_UPD_OPEN_UVC_CB;
        break;
    case MMPD_3GPMGR_UPD_START_UVC_CB:
        e = MMPF_USBH_UPD_START_UVC_CB;
        break;
    case MMPD_3GPMGR_UPD_NALU_TBL:
        e = MMPF_USBH_UPD_NALU_TBL;
        break;
    default:
        e = MMPF_USBH_UPD_OP_NONE;
        break;
    }

    return MMPF_USBH_UpdDevCFG(e, pubStr, pParm);
#else
    return MMP_USB_ERR_UNSUPPORT_MODE;
#endif
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_RegDevAddiCFG
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
MMP_ERR MMPD_3GPMGR_RegDevAddiCFG(MMP_ULONG ulRegTyp, MMP_UBYTE *pubStr, MMP_ULONG ulParm0, MMP_ULONG ulParm1, MMP_ULONG ulParm2, MMP_ULONG ulParm3)
{
	MMP_ERR err = MMP_ERR_NONE;
    #if (SUPPORT_USB_HOST_FUNC)
    err = MMPF_USBH_RegDevAddiCFG(ulRegTyp, pubStr, ulParm0,ulParm1,ulParm2,ulParm3);
    #endif
    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetClassIfCmd
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
MMP_ERR MMPD_3GPMGR_SetClassIfCmd(MMP_UBYTE bReq, MMP_USHORT wVal, MMP_USHORT wInd, MMP_USHORT wLen, MMP_UBYTE *UVCDataBuf)
{
	MMP_ERR err = MMP_ERR_NONE;
    #if (SUPPORT_USB_HOST_FUNC)
    err = MMPF_USBH_SetClassIfCmd(bReq, wVal, wInd, wLen, UVCDataBuf);
    #endif
    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_GetClassIfCmd
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
MMP_ERR MMPD_3GPMGR_GetClassIfCmd(MMP_UBYTE bReq, MMP_USHORT wVal, MMP_USHORT wInd, MMP_USHORT wLen, MMP_ULONG *UVCDataLength, MMP_UBYTE *UVCDataBuf)
{
	MMP_ERR err = MMP_ERR_NONE;
    #if (SUPPORT_USB_HOST_FUNC)
    err = MMPF_USBH_GetClassIfCmd(bReq, wVal, wInd, wLen, UVCDataLength, UVCDataBuf);
    #endif
    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetStdIfCmd
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
MMP_ERR MMPD_3GPMGR_SetStdIfCmd(MMP_UBYTE bReq, MMP_USHORT wVal, MMP_USHORT wInd, MMP_USHORT wLen, MMP_UBYTE *UVCDataBuf)
{
	MMP_ERR err = MMP_ERR_NONE;
    #if (SUPPORT_USB_HOST_FUNC)
    err = MMPF_USBH_SetStdIfCmd(bReq, wVal, wInd, wLen, UVCDataBuf);
    #endif
    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_GetStdDevCmd
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
MMP_ERR MMPD_3GPMGR_GetStdDevCmd(MMP_UBYTE bReq, MMP_USHORT wVal, MMP_USHORT wInd, MMP_USHORT wLen, MMP_ULONG *UVCDataLength, MMP_UBYTE *UVCDataBuf)
{
	MMP_ERR err = MMP_ERR_NONE;
    #if (SUPPORT_USB_HOST_FUNC)
    err = MMPF_USBH_GetStdDevCmd(bReq, wVal, wInd, wLen, UVCDataLength, UVCDataBuf);
    #endif
    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_3GPMGR_SetUVCRepackMiscBuf
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Set start address and size of firmware AV repack buffer.

 This buffer is for packing video and audio bitstream sequentially.
 @param[in] repackbuf information of AV repack buffer, such as start address/size,
 sync buffer address, buffer address/size for frame size, and buffer address/size for frame time.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_SetUVCRepackMiscBuf(MMP_ULONG ulFileType, MMPD_3GPMGR_REPACKBUF *repackbuf)
{
	MMP_ERR	status;
	
	// AV Repack Buffer
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, repackbuf->ulAvRepackStartAddr);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, repackbuf->ulAvRepackSize);
    MMPH_HIF_SetParameterB(GRP_IDX_VID, 8, ulFileType);
    
    status = MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | AV_REPACK_BUF);
    if (status != MMP_ERR_NONE) {
    	MMPH_HIF_ReleaseSem(GRP_IDX_VID);
        return MMP_SYSTEM_ERR_CMDTIMEOUT;
    }

	// Aux Frame Table
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0,  repackbuf->ulVideoSizeTableAddr);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 4, repackbuf->ulVideoSizeTableSize);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 6, MMPD_3GPMGR_AUX_FRAME_TABLE);
    MMPH_HIF_SetParameterB(GRP_IDX_VID, 8, ulFileType);
    status = MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | AUX_TABLE_ADDR);

	// Aux Time Table
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, repackbuf->ulVideoTimeTableAddr);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 4, repackbuf->ulVideoTimeTableSize);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 6, MMPD_3GPMGR_AUX_TIME_TABLE);
    MMPH_HIF_SetParameterB(GRP_IDX_VID, 8, ulFileType);
    status = MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_PARAMETER | AUX_TABLE_ADDR);

    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

    return MMP_ERR_NONE;
}

/**
 @brief Set video encode format to container information.

 @param[in] usFileType  - video file type
 @param[in] usFormat - video encode format
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPD_3GPMGR_SetEncodeFormat(MMP_USHORT usFileType, MMP_USHORT usFormat)
{
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);	
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, usFileType);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, usFormat);
	
	MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_ENCODE_FORMAT);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
	
    return MMP_ERR_NONE;
}

/**
 @brief Set video encode resolution to container information.

 @param[in] usFileType  - video file type
 @param[in] ResolW - video encode width
 @param[in] ResolH - video encode height
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPD_3GPMGR_SetEncodeResolution(MMP_USHORT usFileType, MMP_USHORT ResolW, MMP_USHORT ResolH)
{
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);	
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, usFileType);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, ResolW);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 8, ResolH);
	
	MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_ENCODE_RESOLUTION);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
	
    return MMP_ERR_NONE;
}

/**
 @brief Set video encode frame rate to container information.

 @param[in] usFileType  - video file type
 @param[in] timeresol Time resolution.
 @param[in] timeincrement Time increment.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPD_3GPMGR_SetFrameRate(MMP_USHORT usFileType, MMP_ULONG timeresol, MMP_ULONG timeincrement)
{
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);	
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, usFileType);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, timeresol);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 8, timeincrement);
	
	MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_ENCODE_FRAMERATE);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
	
    return MMP_ERR_NONE;
}

/**
 @brief Set video encode GOP size to container information.

 @param[in] usFileType  - video file type
 @param[in] ubPFrame Count of P frame.
 @param[in] ubBFrame Count of B frame.
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPD_3GPMGR_SetEncodeGOP(MMP_USHORT usFileType, MMP_USHORT usPFrame, MMP_USHORT usBFrame)
{
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);	
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, usFileType);
	MMPH_HIF_SetParameterW(GRP_IDX_VID, 4, usPFrame);
	MMPH_HIF_SetParameterW(GRP_IDX_VID, 6, usBFrame);
	
	MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_ENCODE_GOP);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
	
    return MMP_ERR_NONE;
}

/**
 @brief Set video encode sps,pps headerto container information.

 @param[in] usFileType  - video file type
 @param[in] ulSPSAddr - sps buffer address
 @param[in] ulSPSSize - sps buffer size
 @param[in] ulPPSAddr - pps buffer address
 @param[in] ulPPSSize - pps buffer size
 @param[in] ulProfileIDC - h264 profile idc
 @param[in] ulLevelIDC - h264 level idc
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPD_3GPMGR_SetEncodeSPSPPSHdr(MMP_USHORT usFileType, MMP_ULONG ulSPSAddr, MMP_USHORT ulSPSSize, MMP_ULONG ulPPSAddr, 
	MMP_USHORT ulPPSSize, MMP_USHORT ulProfileIDC, MMP_USHORT ulLevelIDC)
{
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);	
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, usFileType);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, ulSPSAddr);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 8, ulPPSAddr);
	MMPH_HIF_SetParameterW(GRP_IDX_VID, 12, ulSPSSize);
	MMPH_HIF_SetParameterW(GRP_IDX_VID, 14, ulPPSSize);
	MMPH_HIF_SetParameterW(GRP_IDX_VID, 16, ulProfileIDC);
	MMPH_HIF_SetParameterW(GRP_IDX_VID, 18, ulLevelIDC);
	
	MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_ENCODE_SPSPPSHDR);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
	
    return MMP_ERR_NONE;
}

/**
 @brief Set max time to flush FS cache buffer to SD.

 @param[in] time Set max time(ms).
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_SetTime2FlushFSCache(MMP_ULONG time)
{
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);	
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, time);
	
	MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_TIME_FLUSH_FSCACHE);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
	
    return MMP_ERR_NONE;
}

/**
 @brief check if there is a file need to be refixed.

 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_3GPMGR_CheckFile2Refix(void)
{
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);	
	
	MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_REFIX_VIDRECD);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
	
    return MMP_ERR_NONE;
}

/**
 @brief Set thumb nail buffer address and size for video recording.

 @param[in] uladdr - buffer address.
 @param[in] ulsize - buffer size
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPD_3GPMGR_SetThumbnailInfo(MMP_ULONG uladdr, MMP_ULONG ulsize)
{
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);	
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, uladdr);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, ulsize);
	
	MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_SET_THUMB_INFO);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
	
    return MMP_ERR_NONE;
}

/**
 @brief Save thumb nail buffer for video recording.

 @param[in] uladdr - buffer address.
 @param[in] ulsize - buffer size
 @retval MMP_ERR_NONE Success.
 @retval MMP_3GPRECD_ERR_GENERAL_ERROR Not allowed procedure.
*/
MMP_ERR MMPD_3GPMGR_SetThumbnailBuf(MMP_ULONG uladdr, MMP_ULONG ulsize)
{
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);	
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, uladdr);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, ulsize);
	
	MMPH_HIF_SendCmd(GRP_IDX_VID, HIF_VID_CMD_MERGER_OPERATION | MERGER_SET_THUMB_BUF);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
	
    return MMP_ERR_NONE;
}

/// @}

/// @end_ait_only

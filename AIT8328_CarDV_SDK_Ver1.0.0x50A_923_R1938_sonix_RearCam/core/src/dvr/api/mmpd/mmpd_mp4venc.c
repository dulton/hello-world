/// @ait_only
/**
 @file mmpd_mp4venc.c
 @brief Retina Video Encoder Control Driver Function
 @author Will Tseng
 @version 1.0
*/

#ifdef BUILD_CE
#undef BUILD_FW
#endif

#include "mmp_lib.h"
#include "mmpd_mp4venc.h"
#include "mmpd_h264enc.h"
#include "mmpd_system.h"
#include "mmph_hif.h"
#include "mmp_reg_video.h"
#include "mmp_reg_h264enc.h"
#include "mmp_reg_h264dec.h"
#include "mmp_reg_gbl.h"
#include "ait_utility.h"
#ifdef BUILD_CE
#include "lib_retina.h"
#endif
#include "mmpf_mp4venc.h"
#include "mmpf_mci.h"
#include "mmpf_vif.h"

/** @addtogroup MMPD_MP4VENC
 *  @{
 */

//==============================================================================
//
//                              VARIABLES
//
//==============================================================================

extern MMPD_VIDENC_FUNCTIONS m_VidencFunction_H264;     ///< set of H264 encode functions

MMPD_VIDENC_FUNCTIONS *glVidencFunction = NULL;

static MMPD_MP4VENC_FW_OP m_VidRecdStatus;				///< status of firmware video engine, include START, PAUSE, RESUME and STOP
static MMPD_MP4VENC_FORMAT m_VidRecdEncFormat;			///< encode format
static MMPD_MP4VENC_CURBUF_MODE m_VidRecdCurBufMode;    ///< encode current buffer mode
static MMP_USHORT   m_usVidRecdWidth;                   ///< encode width
static MMP_USHORT   m_usVidRecdHeight;                  ///< encode height

static MMP_ULONG    m_ulVopTimeIncrement;           	///< VOP time increment
static MMP_ULONG    m_ulTimeInCrResol;              	///< VOP time resolution

static MMP_ULONG    m_ulVidRecdRegStoreBuf;             ///< register store buffer address
static MMP_ULONG    m_ulVidRecCurMemAddr = 0x0;

static MMP_ULONG    m_ulMaxBitsPer3600MB[VIDEO_INIT_QP_STEP_NUM] =
                        {66667, 133334, 266667};

static MMP_ULONG    m_ulH264QpStep[VIDEO_INIT_QP_STEP_NUM][3] = {
                        {36, 36, 36}, //i, p, b
                        {30, 30, 30},
                        {24, 24, 24}};

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

/**
 @brief Set mem start address
 @param[in] MemStartAddr mem start address
 @retval MMP_ERR_NONE Success.
*/
void  MMPD_VIDENC_SetMemStart(MMP_ULONG MemStartAddr)
{
    m_ulVidRecCurMemAddr = MemStartAddr;
}

/**
 @brief Alloc mem for VideoR
 @param[in] size size
 @retval MMP_ERR_NONE Success.
*/
MMP_ULONG MMPD_VIDENC_MemAlloc(MMP_ULONG size)
{
    MMP_ULONG   MemAddr;
    
    MemAddr = ALIGN32(m_ulVidRecCurMemAddr);
    m_ulVidRecCurMemAddr = MemAddr + size;
    
    return MemAddr;
}

/**
 @brief Free mem for VideoR
 @param[in] ptr  mem pointer
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_VIDENC_MemFree(void* ptr)
{
    return MMP_ERR_NONE;
}

MMP_BOOL MMPD_VIDENC_IsModuleInit(MMPD_VIDENC_MODULE_ID ModId)
{
	MMP_USHORT usModId;

	switch (ModId)
	{
	case MMPD_VIDENC_MODULE_H264:
		usModId = MMPF_VIDENC_MODULE_H264;
		break;
	default:
	    PRINTF("Not supported video module Id !\r\n");
	    break;
	}

	return MMPF_VIDENC_IsModuleInit(usModId);
}

MMP_ERR MMPD_VIDENC_DeinitModule(MMPD_VIDENC_MODULE_ID ModId)
{
	MMP_USHORT usModId;

	switch (ModId)
	{
	case MMPD_VIDENC_MODULE_H264:
		usModId = MMPF_VIDENC_MODULE_H264;
		break;
	default:
	    PRINTF("Not supported video module Id !\r\n");
	    break;
	}

	return MMPF_VIDENC_DeinitModule(usModId);
}

/**
 @brief Set encoding format to H264, MPEG-4 or H.263 & API I/F.
 @param[in] VideoFormat Video format.
 @retval MMP_ERR_NONE Success.
 @note These modes can be added by different requests.
 The parameter @a VideoFormat can be:
 - MMPD_MP4VENC_FORMAT_OTHERS
 - MMPD_MP4VENC_FORMAT_H264
*/
MMP_ERR MMPD_VIDENC_SetFormat(MMPD_MP4VENC_FORMAT VideoFormat)
{
	MMP_ERR	mmpstatus;
    m_VidRecdEncFormat = VideoFormat;
    
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 0, m_VidRecdEncFormat);

	mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_VID, ENCODE_FORMAT | HIF_VID_CMD_RECD_PARAMETER);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

	if (mmpstatus)
		return mmpstatus;
    switch(m_VidRecdEncFormat) {
	case MMPD_MP4VENC_FORMAT_H264:
        glVidencFunction = &m_VidencFunction_H264;
        break;
	default:
	    PRINTF("Not supported video format !\r\n");
	    break;
	}

    return MMP_ERR_NONE;
}

/**
 @brief Start timer for recording
 @param[in] bEnable enable/disable timer
 @retval MMP_ERR_NONE Success
*/
MMP_ERR MMPD_VIDENC_EnableTimer(MMP_BOOL bEnable)
{
    MMP_ERR	mmpstatus = MMP_ERR_NONE;
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterB(GRP_IDX_VID, 0, bEnable);
    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_VID, ENCODE_TIMER_EN | HIF_VID_CMD_RECD_PARAMETER);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    return mmpstatus;
}

/**
 @brief Get encoding format of MPEG-4 or H.263.
 @retval MMPD_MP4VENC_FORMAT_OTHERS Not support format.
*/
MMPD_MP4VENC_FORMAT MMPD_VIDENC_GetFormat(void)
{
    return m_VidRecdEncFormat;
}

/**
 @brief Get video width.
 @return Encode width.
*/
MMP_USHORT MMPD_VIDENC_GetVideoWidth(void)
{
    return m_usVidRecdWidth;
}

/**
 @brief Get video height.
 @return Encode height.
*/
MMP_USHORT MMPD_VIDENC_GetVideoHeight(void)
{
    return m_usVidRecdHeight;
}

/**
 @brief Get video encode frame rate resolution.
 @return Encode width.
*/
MMP_ULONG MMPD_VIDENC_GetTimeResolution(void)
{
    return m_ulTimeInCrResol;
}

/**
 @brief Get video encode frame rate time increment
 @return Encode height.
*/
MMP_ULONG MMPD_VIDENC_GetTimeIncrement(void)
{
    return m_ulVopTimeIncrement;
}

MMP_ERR MMPD_VIDENC_SetQuality(MMP_ULONG ulEncId, MMP_ULONG ulTargetSize, MMP_ULONG ulBitrate)
{
    MMP_ULONG   ulBaseTargetBits, ulMBNum, i;
    MMP_ULONG   InitQp[3];

    if (glVidencFunction == NULL) {
        PRINTF("Error: un-initialed video format ...\r\n");
        return MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS;
    }

    ulMBNum = (((MMP_ULONG)m_usVidRecdWidth)*((MMP_ULONG)m_usVidRecdHeight)) / 256;
    if (ulMBNum == 0) {
        return MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS;
    }

    ulBaseTargetBits = ((ulTargetSize * 3600) / ulMBNum) * 8;

    for (i = 0; i < (VIDEO_INIT_QP_STEP_NUM-1); i++) {
        if (ulBaseTargetBits <= m_ulMaxBitsPer3600MB[i]) {
            break;
        }
    }

    switch (m_VidRecdEncFormat) {

    case MMPD_MP4VENC_FORMAT_H264:
        InitQp[0] = m_ulH264QpStep[i][0];
        InitQp[1] = m_ulH264QpStep[i][1];
        InitQp[2] = m_ulH264QpStep[i][2];
        break;
    default:
        return MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS;
    }
    
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulEncId);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, InitQp[0]);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 8, InitQp[1]);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 12, InitQp[2]);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 16, ulTargetSize);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 20, ulBitrate);
    MMPH_HIF_SendCmd(GRP_IDX_VID, ENCODE_QUALITY_CTL | HIF_VID_CMD_RECD_PARAMETER);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    return MMP_ERR_NONE;
}

MMP_ERR MMPD_VIDENC_SetCropping (MMP_ULONG ulEncId, MMP_USHORT usTop, MMP_USHORT usBottom, MMP_USHORT usLeft, MMP_USHORT usRight)
{
    if ((usTop >= 16) || (usBottom >= 16) || (usBottom >= 16) || (usBottom >= 16) ||
        (usTop & 0x01) || (usBottom & 0x01) || (usBottom & 0x01) || (usBottom & 0x01)) {
        return MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS;
    }
    
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulEncId);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 4, usTop);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 6, usBottom);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 8, usLeft);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 10, usRight);
    MMPH_HIF_SendCmd(GRP_IDX_VID, ENCODE_CROPPING | HIF_VID_CMD_RECD_PARAMETER);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    return MMP_ERR_NONE;
}

/**
 @brief Set encoding current buffer mode to H264, MPEG-4 or H.263 & API I/F.
 @param[in] VideoFormat Video current buffer mode.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_VIDENC_SetCurBufMode(MMP_ULONG ulEncId, MMPD_MP4VENC_CURBUF_MODE VideoCurBufMode)
{
    MMP_ERR mmpstatus;
    m_VidRecdCurBufMode = VideoCurBufMode;

    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulEncId);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 4, m_VidRecdCurBufMode);

    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_VID, ENCODE_CURBUFMODE | HIF_VID_CMD_RECD_PARAMETER);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

    if (mmpstatus)
        return mmpstatus;

    return MMP_ERR_NONE;
}

MMP_ERR MMPD_VIDENC_InitModule (MMPD_VIDENC_MODULE_ID ModId)
{
    if (glVidencFunction == NULL) {
        PRINTF("Error: un-initialed video format ...\r\n");
        return MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS;
    }
    return glVidencFunction->MMPD_VIDENC_InitModule(ModId);
}

MMP_ERR MMPD_VIDENC_InitInstance(MMP_ULONG *InstId, MMPD_VIDENC_MODULE_ID ModuleId)
{
    if (glVidencFunction == NULL) {
        printc("Error: un-initialed video format ...\r\n");
        return MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS;
    }
    return glVidencFunction->MMPD_VIDENC_InitInstance(InstId, ModuleId);
}

MMP_ERR MMPD_VIDENC_DeInitInstance(MMP_ULONG InstId)
{
    if (glVidencFunction == NULL) {
        PRINTF("Error: un-initialed video format ...\r\n");
        return MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS;
    }
    return glVidencFunction->MMPD_VIDENC_DeInitInstance(InstId);
}


MMP_ERR MMPD_VIDENC_SetBitstreamBuf(MMP_ULONG ulEncId, MMPD_MP4VENC_BITSTREAM_BUF *bsbuf)
{
    if (glVidencFunction == NULL) {
        PRINTF("Error: un-initialed video format ...\r\n");
        return MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS;
    }
    return glVidencFunction->MMPD_VIDENC_SetBitstreamBuf(ulEncId, bsbuf);
}

// Feature related API
MMP_ERR MMPD_VIDENC_SetResolution(MMP_ULONG ulEncId, MMP_USHORT usWidth, MMP_USHORT usHeight)
{
    if (glVidencFunction == NULL) {
        PRINTF("Error: un-initialed video format ...\r\n");
        return MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS;
    }
    return glVidencFunction->MMPD_VIDENC_SetResolution(ulEncId, usWidth, usHeight);
}

MMP_ERR MMPD_VIDENC_SetProfile(MMP_ULONG ulEncId, MMPD_VIDENC_PROFILE profile)
{
    MMP_ULONG ulProfile;

    switch(m_VidRecdEncFormat) {
    case MMPD_MP4VENC_FORMAT_H264:
        if ((profile <= MMPD_H264ENC_PROFILE_NONE) || (profile >= MMPD_H264ENC_PROFILE_MAX))
            return MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS;
        break;
    }

    switch(profile) {
    case MMPD_H264ENC_BASELINE_PROFILE:
    	ulProfile = BASELINE_PROFILE;
        break;
    case MMPD_H264ENC_MAIN_PROFILE:
    	ulProfile = MAIN_PROFILE;
        break;
    case MMPD_H264ENC_HIGH_PROFILE:
    	ulProfile = FREXT_HP;
        break;
    default:
        PRINTF("Unsupported profile\r\n");
        break;
    }

    if (glVidencFunction == NULL) {
        PRINTF("Error: un-initialed video format ...\r\n");
        return MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS;
    }
    return glVidencFunction->MMPD_VIDENC_SetProfile(ulEncId, ulProfile);
}

MMP_ERR MMPD_VIDENC_SetEncodeMode(void)
{
    if (glVidencFunction == NULL) {
        PRINTF("Error: un-initialed video format ...\r\n");
        return MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS;
    }
    return glVidencFunction->MMPD_VIDENC_SetEncodeMode();
}

MMP_ERR MMPD_VIDENC_SetGOP(MMP_ULONG ulEncId, MMP_USHORT usPFrame, MMP_USHORT usBFrame)
{
    if (glVidencFunction == NULL) {
        PRINTF("Error: un-initialed video format ...\r\n");
        return MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS;
    }
    return glVidencFunction->MMPD_VIDENC_SetGOP(ulEncId, usPFrame, usBFrame);
}

MMP_ERR MMPD_VIDENC_SetBitrate(MMP_ULONG ulEncId, MMP_ULONG ulTargetSize, MMP_ULONG ulBitrate)
{
    if (glVidencFunction == NULL) {
        PRINTF("Error: un-initialed video format ...\r\n");
        return MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS;
    }
    return glVidencFunction->MMPD_VIDENC_SetBitrate(ulEncId, ulTargetSize, ulBitrate);
}

MMP_ERR MMPD_VIDENC_SetEncFrameRate(MMP_ULONG ulEncId, MMP_ULONG ulTimeIncrement, MMP_ULONG ulTimeResol)
{
    if (glVidencFunction == NULL) {
        PRINTF("Error: un-initialed video format ...\r\n");
        return MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS;
    }
    return glVidencFunction->MMPD_VIDENC_SetEncFrameRate(ulEncId, ulTimeIncrement, ulTimeResol);
}

MMP_ERR MMPD_VIDENC_UpdateEncFrameRate(MMP_ULONG ulEncId, MMP_ULONG ulTimeIncrement, MMP_ULONG ulTimeResol)
{
    if (glVidencFunction == NULL) {
        PRINTF("Error: un-initialed video format ...\r\n");
        return MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS;
    }
    return glVidencFunction->MMPD_VIDENC_UpdateEncFrameRate(ulEncId, ulTimeIncrement, ulTimeResol);
}

MMP_ERR MMPD_VIDENC_SetSnrFrameRate(MMP_ULONG ulEncId, MMP_ULONG ulTimeIncrement, MMP_ULONG ulTimeResol)
{
    if (glVidencFunction == NULL) {
        PRINTF("Error: un-initialed video format ...\r\n");
        return MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS;
    }
    return glVidencFunction->MMPD_VIDENC_SetSnrFrameRate(ulEncId, ulTimeIncrement, ulTimeResol);
}

MMP_ERR MMPD_VIDENC_EnableLineBuf(MMP_BOOL bLineEn, MMP_BOOL bLCLEn)
{
    if (glVidencFunction == NULL) {
        PRINTF("Error: un-initialed video format ...\r\n");
        return MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS;
    }
    return glVidencFunction->MMPD_VIDENC_EnableLineBuf(bLineEn, bLCLEn);
}

MMP_ERR MMPD_VIDENC_EnableClock(MMP_BOOL bEnable)
{
    return glVidencFunction->MMPD_VIDENC_EnableClock(bEnable);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_VIDENC_GetNumOpen
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Check the video encoding number.
 @param[out] ulNumOpening The number of the opening stream.
 @retval MMP_ERR_NONE Success.
 @note

*/
MMP_ERR MMPD_VIDENC_GetNumOpen(MMP_ULONG *ulNumOpening)
{
	MMP_ERR	mmpstatus = MMP_ERR_NONE;

    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_VID, GET_VIDRECD_NUMOPEN | HIF_VID_CMD_RECD_PARAMETER);

	if (mmpstatus) {
        MMPH_HIF_ReleaseSem(GRP_IDX_VID);
		return mmpstatus;
    }

	*ulNumOpening = MMPH_HIF_GetParameterL(GRP_IDX_VID, 0);

    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

    return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_VIDENC_GetStatus
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Check the firmware video encoding engine status.
 @param[out] status Firmware video engine status.
 @retval MMP_ERR_NONE Success.
 @note

 The parameter @a status can not be changed because it sync with the firmware video engine
 status definitions. It can be
 - 0x0000 MMP_VIDRECD_FW_STATUS_OP_NONE
 - 0x0001 MMP_VIDRECD_FW_STATUS_OP_START
 - 0x0002 MMP_VIDRECD_FW_STATUS_OP_PAUSE
 - 0x0003 MMP_VIDRECD_FW_STATUS_OP_RESUME
 - 0x0004 MMP_VIDRECD_FW_STATUS_OP_STOP
*/
MMP_ERR MMPD_VIDENC_GetStatus(MMP_ULONG ulEncId, MMPD_MP4VENC_FW_OP *status)
{
	MMP_ERR	mmpstatus = MMP_ERR_NONE;
    
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulEncId);
    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_VID, GET_VIDRECD_STATUS | HIF_VID_CMD_RECD_PARAMETER);
	
	if (mmpstatus) {
        MMPH_HIF_ReleaseSem(GRP_IDX_VID);
		return mmpstatus;
    }
    
    *status = m_VidRecdStatus = MMPH_HIF_GetParameterW(GRP_IDX_VID, 0);

    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

    return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_VIDENC_GetMergerStatus
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
MMP_ERR MMPD_VIDENC_GetMergerStatus(MMP_ERR *status, MMP_ULONG *tx_status)
{
	MMP_ERR	mmpstatus = MMP_ERR_NONE;
    
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    mmpstatus =MMPH_HIF_SendCmd(GRP_IDX_VID, MERGER_STATUS | HIF_VID_CMD_RECD_PARAMETER);
	if (mmpstatus) {
        MMPH_HIF_ReleaseSem(GRP_IDX_VID);
		return mmpstatus;
    }
    *status    = MMPH_HIF_GetParameterL(GRP_IDX_VID, 0);
    *tx_status = MMPH_HIF_GetParameterL(GRP_IDX_VID, 4);
    
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

    return mmpstatus;
}

/**
 @brief Check the capability of video encoding engine.
 @param[out] If the specified resolution and frame rate can be supported.
 @retval MMP_ERR_NONE for Supported.
 @note
*/
MMP_ERR MMPD_VIDENC_CheckCapability(MMP_ULONG w, MMP_ULONG h, MMP_ULONG fps)
{
    MMP_ERR	    ret = MMP_ERR_NONE;
    MMP_BOOL    supported = MMP_FALSE;

    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ((w + 15) >> 4) * ((h + 15) >> 4));
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, fps);
    ret = MMPH_HIF_SendCmd(GRP_IDX_VID, ENCODE_CAPABILITY | HIF_VID_CMD_RECD_PARAMETER);
    if (ret == MMP_ERR_NONE)
        supported = MMPH_HIF_GetParameterB(GRP_IDX_VID, 0);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

    if (!supported) // unsupported resol & frame rate
        ret = MMP_MP4VE_ERR_CAPABILITY;

    return ret;
}

/**
 @brief Fine tune MCI priority to fit VGA size encoding. It's an access issue.
 @param[in] ubMode Mode.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_VIDENC_TuneMCIPriority(MMPD_VIDENC_MCI_MODE mciMode)
{
    return MMPF_MCI_TunePriority(mciMode);
}

/**
 @brief Fine tune MCI priority of encode pipe
 @param[in] ubPipe Encode pipe
 @retval MMP_ERR_NONE Success.
*/
void MMPD_VIDENC_TuneEncodePipeMCIPriority(MMP_UBYTE ubPipe)
{
    MMPF_MCI_SetIBCMaxPriority(ubPipe);
}

/**
 @brief Fine tune MCI priority of second encode pipe (smaller resolution)
 @param[in] ubPipe Encode pipe
 @retval MMP_ERR_NONE Success.
*/
void MMPD_VIDENC_TuneEncodeSecondPipeMCIPriority(MMP_UBYTE ubPipe)
{
    MMPF_MCI_SetIBCSecondPriority(ubPipe);
}
                                                    
MMP_ERR MMPD_VIDENC_SetQPBoundary(MMP_ULONG ulLowerBound,MMP_ULONG ulUpperBound)
{
	MMP_ERR	mmpstatus = MMP_ERR_NONE;

    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulLowerBound);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, ulUpperBound);
    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_VID, SET_QP_BOUNDARY | HIF_VID_CMD_RECD_PARAMETER);	
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    return mmpstatus;
}

/**
 @brief Set video current buffer.

 This buffer is for sensor raw data and should be counted to the maximum encoding
 resolution. It can be pingpong or not, just depends on the memory requirement.
 @param[in] *inputbuf Pointer of encode buffer structure.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_H264ENC_SetSourceBuf(MMPD_MP4VENC_INPUT_BUF *inputbuf)
{
	MMPD_System_EnableClock(MMPD_SYS_CLK_H264, MMP_TRUE);

    MMPH_HIF_RegSetL(H264ENC_CURR_Y_ADDR, inputbuf->ulY[0]);
    MMPH_HIF_RegSetL(H264ENC_CURR_U_ADDR, inputbuf->ulU[0]);
    MMPH_HIF_RegSetL(H264ENC_CURR_V_ADDR, inputbuf->ulV[0]);

    MMPH_HIF_RegSetL(H264ENC_CURR_Y_ADDR1, inputbuf->ulY[1]);
    MMPH_HIF_RegSetL(H264ENC_CURR_U_ADDR1, inputbuf->ulU[1]);
    MMPH_HIF_RegSetL(H264ENC_CURR_V_ADDR1, inputbuf->ulV[1]);

	MMPD_System_EnableClock(MMPD_SYS_CLK_H264, MMP_FALSE);

    return MMP_ERR_NONE;
}

/**
 @brief Set video Reference/Generate buffer bound
 @param[in] *refgenbd Pointer of encode buffer structure.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_H264ENC_SetRefGenBound(MMP_ULONG ubEncId, MMPD_MP4VENC_REFGEN_BD *refgenbd)
{
    // Set ref buf
    // Set ref buf bound
	
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ubEncId);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, refgenbd->ulYStart);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 8, refgenbd->ulUEnd);

	MMPH_HIF_SendCmd(GRP_IDX_VID, ENCODE_REFGENBD | HIF_VID_CMD_RECD_PARAMETER);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

    return MMP_ERR_NONE;
}

/**
 @brief Set video compressed buffer for video encoding bitstream.

 Assign this in the internal frame buffer can get better performance. Minus buffer
 end address by 16 is to avoid buffer overflow.
 @param[in] *bsbuf Pointer of encode buffer structure.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_H264ENC_SetBitstreamBuf(MMP_ULONG ubEncId, MMPD_MP4VENC_BITSTREAM_BUF *bsbuf)
{
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ubEncId);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, bsbuf->ulStart);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 8, bsbuf->ulEnd);

	MMPH_HIF_SendCmd(GRP_IDX_VID, ENCODE_BSBUF | HIF_VID_CMD_RECD_PARAMETER);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

    return MMP_ERR_NONE;
}

/**
 @brief Set slice length, MV buffer.
 @param[in] *miscbuf Pointer of encode buffer structure.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_H264ENC_SetMiscBuf(MMP_ULONG ubEncId, MMPD_MP4VENC_MISC_BUF *miscbuf)
{
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ubEncId);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, miscbuf->ulMVBuf);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 8, miscbuf->ulSliceLenBuf);

	MMPH_HIF_SendCmd(GRP_IDX_VID, ENCODE_MISCBUF | HIF_VID_CMD_RECD_PARAMETER);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

    return MMP_ERR_NONE;
}

/**
 @brief Set buffer for SPS, PPS
 @param[in] hdrbuf Buffer address
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_H264ENC_SetHeaderBuf(MMP_ULONG ubEncId, MMPD_H264ENC_HEADER_BUF *hdrbuf)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ubEncId);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, hdrbuf->ulSPSStart);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 8, hdrbuf->ulPPSStart);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 12, hdrbuf->ulTmpSPSStart);
    
    MMPH_HIF_SendCmd(GRP_IDX_VID, SET_HEADER_BUF | HIF_VID_CMD_RECD_PARAMETER);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    return MMP_ERR_NONE;
}

/**
 @brief Get SPS, PPS address and size
 @param[out] hdrbuf Info
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_H264ENC_GetHeaderInfo(MMPD_H264ENC_HEADER_BUF *hdrbuf)
{
	MMP_ERR	mmpstatus;

    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_VID, GET_HEADER_INFO | HIF_VID_CMD_RECD_PARAMETER);
   	if (mmpstatus != MMP_ERR_NONE) {
        MMPH_HIF_ReleaseSem(GRP_IDX_VID);
       	return mmpstatus;
   	}

    hdrbuf->ulSPSStart = MMPH_HIF_GetParameterL(GRP_IDX_VID, 0);
    hdrbuf->ulSPSSize  = MMPH_HIF_GetParameterL(GRP_IDX_VID, 4);
    hdrbuf->ulPPSStart = MMPH_HIF_GetParameterL(GRP_IDX_VID, 8);
    hdrbuf->ulPPSSize  = MMPH_HIF_GetParameterL(GRP_IDX_VID, 12);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
	return MMP_ERR_NONE;
}

/**
 @brief Calculate and Generate the REF/GEN buffer for Video Encode Module.

 Depends on encoded resolution to generate the REF/GEN buffer
 @param[in] usWidth Encode width.
 @param[in] usHeight Encode height.
 @param[out] refgenbd Mp4 engine require REF/GEN buffer.
 @param[in,out] ulCurAddr Available start address for buffer start.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_H264ENC_CalculateRefBuf(MMP_USHORT usWidth, MMP_USHORT usHeight, 
											MMPD_MP4VENC_REFGEN_BD *refgenbd, MMP_ULONG *ulCurAddr)
{
	MMP_ULONG	bufsize;
	
    *ulCurAddr = ALIGN32(*ulCurAddr);

	bufsize = usWidth * usHeight;
	refgenbd->ulYStart  = *ulCurAddr;
	*ulCurAddr += bufsize;
	refgenbd->ulYEnd    = *ulCurAddr;

	bufsize /= 2;
	refgenbd->ulUStart  = *ulCurAddr;
	*ulCurAddr += bufsize;
	refgenbd->ulUEnd    = *ulCurAddr;
	refgenbd->ulVStart  = 0;
	refgenbd->ulVEnd    = 0;

    *ulCurAddr = ALIGN32(*ulCurAddr);

    #if (SHARE_REF_GEN_BUF == 1)
    refgenbd->ulGenStart    = refgenbd->ulYStart;
	refgenbd->ulGenEnd      = refgenbd->ulUEnd;
    #else
    #if (H264ENC_ICOMP_EN)
	bufsize = usWidth * usHeight;

	refgenbd->ulGenYStart = *ulCurAddr;
	*ulCurAddr += bufsize;
	refgenbd->ulGenYEnd = *ulCurAddr;
	refgenbd->ulGenUVStart = *ulCurAddr;
	*ulCurAddr += bufsize/2;
	refgenbd->ulGenUVEnd = *ulCurAddr;
    #else
	bufsize = (usWidth * usHeight * 3)/2; // Frame total size
	refgenbd->ulGenStart    = *ulCurAddr;
	*ulCurAddr += bufsize;
	refgenbd->ulGenEnd      = *ulCurAddr;
    #endif//(H264ENC_ICOMP_EN)
    #endif

	return	MMP_ERR_NONE;
}

/**
 @brief Set store buffer to backup registers.
 @param[in] ulBufAddr Start address of store buffer.
 @retval MMP_ERR_NONE Success.

 Currently use REG_STORE_BUF_INFO cmd for setting SPS/PPS buffer
*/
MMP_ERR MMPD_H264ENC_SetStoreBufParams(MMP_ULONG ulBufAddr)
{
	m_ulVidRecdRegStoreBuf = ulBufAddr;

    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, m_ulVidRecdRegStoreBuf);
    MMPH_HIF_SendCmd(GRP_IDX_VID, REG_STORE_BUF_INFO | HIF_VID_CMD_RECD_PARAMETER);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

    return MMP_ERR_NONE;
}

/**
 @brief Set video padding for video height
 @param[in] usType padding type 0: zero, 1: repeat
 @param[in] usCnt  the height line offset which need to pad
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_H264ENC_SetPadding(MMP_ULONG ulEncId, MMP_USHORT usType, MMP_USHORT usCnt)
{
    if (usCnt > 15) {
        return MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS;
    }
    
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulEncId);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 4, usType);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 6, usCnt);
    MMPH_HIF_SendCmd(GRP_IDX_VID, ENCODE_PADDING | HIF_VID_CMD_RECD_PARAMETER);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

    return MMP_ERR_NONE;
}

/**
 @brief Set encode mci byte count
 @param[in] usByteCnt byte count value (128 or 256)
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_H264ENC_SetEncByteCnt(MMP_USHORT usByteCnt)
{
	MMPD_System_EnableClock(MMPD_SYS_CLK_H264, MMP_TRUE);

    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 0, usByteCnt);
    MMPH_HIF_SendCmd(GRP_IDX_VID, ENCODE_BYTE_COUNT | HIF_VID_CMD_RECD_PARAMETER);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_H264, MMP_FALSE);

    return MMP_ERR_NONE;
}

MMP_ERR MMPD_H264ENC_InitModule(MMPD_VIDENC_MODULE_ID ModId)
{
	MMP_ERR	mmpstatus;

	MMPD_System_EnableClock(MMPD_SYS_CLK_H264, MMP_TRUE);

    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 0, ModId);
    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_VID, ENCODE_INITMOD | HIF_VID_CMD_RECD_PARAMETER);
   	if (mmpstatus != MMP_ERR_NONE) {
        MMPH_HIF_ReleaseSem(GRP_IDX_VID);

        MMPD_System_EnableClock(MMPD_SYS_CLK_H264, MMP_FALSE);

       	return mmpstatus;
   	}
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

    MMPD_System_EnableClock(MMPD_SYS_CLK_H264, MMP_FALSE);

    return MMP_ERR_NONE;
}

MMP_ERR MMPD_H264ENC_InitInstance(MMP_ULONG *InstId, MMPD_VIDENC_MODULE_ID ModuleId)
{
    switch (m_VidRecdEncFormat) {
    case MMPD_MP4VENC_FORMAT_H264:
        return MMPF_VIDENC_InitInstance(InstId, ModuleId);
    default:
        return MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS;
    }

    return MMP_ERR_NONE;
}


MMP_ERR MMPD_H264ENC_DeInitInstance(MMP_ULONG InstId)
{
    switch (m_VidRecdEncFormat) {
    case MMPD_MP4VENC_FORMAT_H264:
        return MMPF_VIDENC_DeInitInstance(InstId);
    default:
        return MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS;
    }

    return MMP_ERR_NONE;
}

/**
 @brief Set encoding resolution, width and height.
 @param[in] usWidth Encode width.
 @param[in] usHeight Encode height.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_H264ENC_SetResolution(MMP_ULONG ulEncId, MMP_USHORT usWidth, MMP_USHORT usHeight)
{
    MMP_ERR		mmpstatus = MMP_ERR_NONE;

    m_usVidRecdWidth = usWidth;
    m_usVidRecdHeight = usHeight;
    
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulEncId);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 4, m_usVidRecdWidth);
    MMPH_HIF_SetParameterW(GRP_IDX_VID, 6, m_usVidRecdHeight);
    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_VID, ENCODE_RESOLUTION | HIF_VID_CMD_RECD_PARAMETER);

    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

    return mmpstatus;
}

MMP_ERR MMPD_H264ENC_SetProfile(MMP_ULONG ulEncId, MMP_ULONG ulProfile)
{
    MMP_ERR	mmpstatus = MMP_ERR_NONE;

    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulEncId);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, ulProfile);
    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_VID, ENCODE_PROFILE | HIF_VID_CMD_RECD_PARAMETER);

    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

    return mmpstatus;
}

/**
 @brief Enable encoder OPR setting
 @retval MMP_ERR_NONE Success.
 @retval MMP_MP4VE_ERR_NOT_SUPPORTED_PARAMETERS Failure.
*/
MMP_ERR MMPD_H264ENC_SetEncodeMode(void)
{
	MMPD_System_EnableClock(MMPD_SYS_CLK_H264, MMP_TRUE);

    MMPH_HIF_RegSetB(H264_HP_REG_3, 0);

    MMPH_HIF_RegSetB(H264_HP_REG_4, MMPH_HIF_RegGetB(H264_HP_REG_4) & ~(H264_SEQ_SCAL_LIST_PRESENT_FLAG |
                                                                        H264_SEQ_SCAL_MATRIX_PRESENT_FLAG));
    MMPH_HIF_RegSetB(H264_HP_REG_5, MMPH_HIF_RegGetB(H264_HP_REG_5) & ~(H264_PIC_SCAL_LIST_PRESENT_FLAG |
                                                                        H264_PIC_SCAL_MATRIX_PRESENT_FLAG));

    MMPH_HIF_RegSetB(H264_HP_REG_4, (MMPH_HIF_RegGetB(H264_HP_REG_4) & ~(H264_POC_TYPE_MASK)));
    MMPH_HIF_RegSetB(H264_HP_REG_4, (MMPH_HIF_RegGetB(H264_HP_REG_4) & ~(H264_SPS_ID_MASK)));
    MMPH_HIF_RegSetB(H264_HP_REG_6, (MMPH_HIF_RegGetB(H264_HP_REG_6) & ~(H264_PPS_ID_MASK)));
    MMPH_HIF_RegSetB(H264_HP_REG_7, (MMPH_HIF_RegGetB(H264_HP_REG_7) & ~(H264_LONG_TERM_REF_FLAG)));

	MMPD_System_EnableClock(MMPD_SYS_CLK_H264, MMP_FALSE);

    return MMP_ERR_NONE;
}

/**
 @brief Set P frame number
 @param[in] ubFrameCnt 
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_H264ENC_SetGOP(MMP_ULONG ulEncId, MMP_USHORT usPFrame, MMP_USHORT usBFrame)
{
	MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulEncId);
	MMPH_HIF_SetParameterW(GRP_IDX_VID, 4, usPFrame);
	MMPH_HIF_SetParameterW(GRP_IDX_VID, 6, usBFrame);

	MMPH_HIF_SendCmd(GRP_IDX_VID, ENCODE_GOP | HIF_VID_CMD_RECD_PARAMETER);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

	return MMP_ERR_NONE;
}

MMP_ERR MMPD_H264ENC_SetBitrate(MMP_ULONG ulEncId, MMP_ULONG ulTargetSize, MMP_ULONG ulBitrate)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulEncId);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, ulTargetSize);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 8, ulBitrate);
    MMPH_HIF_SendCmd(GRP_IDX_VID, ENCODE_BIT_RATE | HIF_VID_CMD_RECD_PARAMETER);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    return MMP_ERR_NONE;
}

/**
 @brief Set encode frame rate and time frame factor.
 @param[in] usTimeIncrement Time increment.
 @param[in] usTimeResol Time resolution.
 @retval MMP_ERR_NONE Success.
 @note These modes can be added by different sensors and requests.

 These are used to create the mp4v encode bitstream and to fill the tail of 3GP file.
*/
MMP_ERR MMPD_H264ENC_SetEncFrameRate(MMP_ULONG ulEncId, MMP_ULONG ulTimeIncrement, MMP_ULONG ulTimeResol)
{
	m_ulTimeInCrResol = ulTimeResol;
	m_ulVopTimeIncrement = ulTimeIncrement;

    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulEncId);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, ulTimeResol);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 8, ulTimeIncrement);
    MMPH_HIF_SendCmd(GRP_IDX_VID, ENCODE_FRAME_RATE | HIF_VID_CMD_RECD_PARAMETER);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    return MMP_ERR_NONE;
}

/**
 @brief Dynamically update encode frame rate and time frame factor.
 @param[in] usTimeIncrement Time increment.
 @param[in] usTimeResol Time resolution.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_H264ENC_UpdateEncFrameRate(MMP_ULONG ulEncId, MMP_ULONG ulIncr, MMP_ULONG ulResol)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulEncId);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, ulResol);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 8, ulIncr);
    MMPH_HIF_SendCmd(GRP_IDX_VID, UPD_ENC_FRAME_RATE | HIF_VID_CMD_RECD_PARAMETER);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);

    m_ulTimeInCrResol = ulResol;
	m_ulVopTimeIncrement = ulIncr;
    return MMP_ERR_NONE;
}

/**
 @brief Set sensor input frame rate and time frame factor.
 @param[in] usTimeIncrement Time increment.
 @param[in] usTimeResol Time resolution.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_H264ENC_SetSnrFrameRate(MMP_ULONG ulEncId, MMP_ULONG usTimeIncrement, MMP_ULONG usTimeResol)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
	MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, ulEncId);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, usTimeResol);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 8, usTimeIncrement);
    MMPH_HIF_SendCmd(GRP_IDX_VID, SNR_FRAME_RATE | HIF_VID_CMD_RECD_PARAMETER);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    return MMP_ERR_NONE;
}

/**
 @brief Enable/Disable video line buffer.

 Line buffers are used for motion estimation then need dedicated frame buffers
 for best performance.
 @param[in] bLineEn Set MMP_TRUE to turn on the line buffer function
 @param[in] bLCLEn Dummy parameter
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_H264ENC_EnableLineBuf(MMP_BOOL bLineEn, MMP_BOOL bLCLEn)
{
	return MMP_ERR_NONE;
}

/**
 @brief Set video module clock

 This function will turn on/off the video module clock. Video module is also shared with 
 decoder, so the final decision of the clock should be controlled by system.
 @param[in] bEnable Turn On/Off the VID module clock
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_H264ENC_EnableClock(MMP_BOOL bEnable)
{
	MMPD_System_EnableClock(MMPD_SYS_CLK_H264, bEnable);
	MMPD_System_EnableClock(MMPD_SYS_CLK_AUD, bEnable);
    MMPD_System_EnableClock(MMPD_SYS_CLK_ADC, bEnable);

    return MMP_ERR_NONE;
}

/**
 @brief Config the module used for ME when encoding H264 format

 @param[in] usWidth ME width.
 @param[in] usHeight ME height.
 @param[in,out] ulHWMVBufAddr return the MV buffer addr
 @param[in,out] rawbuf config ME buffer
 @param[in,out] ulSramAddr frame buffer start addr
 @param[in,out] ulDramAddr DRAM start addr
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_H264ENC_SetDVSMode(MMP_USHORT usWidth, MMP_USHORT usHeight,
            MMP_ULONG *ulMVBufAddr, MMP_USHORT *usMVUnitSize, MMP_RAW_STORE_BUF *rawbuf, MMP_ULONG *ulSramAddr,
            MMP_ULONG *ulDramAddr)
{
    MMP_ERR ret = MMP_ERR_NONE;

    return ret;
}

/**
 @brief Set buffer for SPS, PPS to UVC record
 @param[in] hdrbuf Buffer address
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_H264ENC_SetUVCHdrBuf(MMPD_H264ENC_HEADER_BUF *hdrbuf)
{
    MMPH_HIF_WaitSem(GRP_IDX_VID, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 0, hdrbuf->ulSPSStart);
    MMPH_HIF_SetParameterL(GRP_IDX_VID, 4, hdrbuf->ulPPSStart);
    MMPH_HIF_SendCmd(GRP_IDX_VID, SET_UVC_HDRBUF | HIF_VID_CMD_RECD_PARAMETER);
    MMPH_HIF_ReleaseSem(GRP_IDX_VID);
    return MMP_ERR_NONE;
}

// Initail encoder function interface
MMPD_VIDENC_FUNCTIONS m_VidencFunction_H264 = {
	MMPD_H264ENC_InitModule,
	MMPD_H264ENC_InitInstance,
	MMPD_H264ENC_DeInitInstance,
	MMPD_H264ENC_SetBitstreamBuf,
    MMPD_H264ENC_SetResolution,
	MMPD_H264ENC_SetProfile,
    MMPD_H264ENC_SetEncodeMode,
    MMPD_H264ENC_SetGOP,
    MMPD_H264ENC_SetBitrate,
    MMPD_H264ENC_SetEncFrameRate,
    MMPD_H264ENC_UpdateEncFrameRate,
    MMPD_H264ENC_SetSnrFrameRate,
	MMPD_H264ENC_EnableLineBuf,
	MMPD_H264ENC_EnableClock,
	MMPD_H264ENC_SetDVSMode
};

#ifdef BUILD_CE
#define BUILD_FW
#endif

/// @}
/// @end_ait_only

/// @ait_only
//==============================================================================
//
//  File        : mmpd_iva.c
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
#include "mmpd_system.h"
#include "mmpd_sensor.h"
#include "mmpd_iva.h"
#include "mmph_hif.h"

/** @addtogroup MMPD_IVA
 *  @{
 */

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================


#if 0
void _____FDTC_Functions_____(){}
#endif

#if (SUPPORT_FDTC)
//------------------------------------------------------------------------------
//  Function    : MMPD_Sensor_GetFaceDetectSpec
//  Description :
//------------------------------------------------------------------------------
/** @brief Check face detection spec.

The function gets the ability of face detection.
@param[out] usMaxH the max width of face detectable.
@param[out] usMaxV the max height of face detectable.
@param[out] ubMaxFaceNum the max number of faces detectable.
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Sensor_GetFaceDetectSpec(MMP_USHORT *usMaxH, MMP_USHORT *usMaxV, MMP_UBYTE *ubMaxFaceNum)
{
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_FDTC | GET_FD_SPEC);
    
    *usMaxH = MMPH_HIF_GetParameterW(GRP_IDX_FLOWCTL, 0);
    *usMaxV = MMPH_HIF_GetParameterW(GRP_IDX_FLOWCTL, 2);
    *ubMaxFaceNum = MMPH_HIF_GetParameterB(GRP_IDX_FLOWCTL, 4);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Sensor_InitFaceDetectConfig
//  Description :
//------------------------------------------------------------------------------
/** @brief Initialize face detection configuration.

The function set face detection initail configuration
@param[in] fdtcattribute the attributes of the face detection.
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Sensor_InitFaceDetectConfig(MMPD_FDTC_ATTR *fdtcAttr)
{
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, fdtcAttr->usInputW);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 2, fdtcAttr->usInputH);
	MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 4, fdtcAttr->ubInputBufCnt|(fdtcAttr->ubDetectFaceNum << 8));
	MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 6, fdtcAttr->ubSmileDetectEn);
	
	MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_FDTC | INIT_FDTC_CONFIG);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Sensor_SetFaceDetectBuf
//  Description :
//------------------------------------------------------------------------------
/** @brief Set face detection buffer.

The function set face detection buffer for operation
@param[in] fdtcattribute the attributes of the face detection.
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Sensor_SetFaceDetectBuf(MMPD_FDTC_ATTR *fdtcAttr)
{
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 0, fdtcAttr->ulFDWorkBufAddr);
	MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 4, fdtcAttr->ulSDWorkBufAddr);
    MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 8, fdtcAttr->ulFeatureBufAddr);
	MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 12, fdtcAttr->ulInfoBufAddr);

	MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_FDTC | SET_FDTC_BUFFER);
	MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
	
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Sensor_ActivateFDBuffer
//  Description :
//------------------------------------------------------------------------------
/** @brief the function will calculated face detection.

The function set command to firmware to do face detection.
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Sensor_ActivateFDBuffer(MMP_BOOL bEnable, MMPD_SENSOR_FDTC_PATH fdtcPath) 
{
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 0, (MMP_ULONG)bEnable);
    MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 4, (MMP_ULONG)fdtcPath);
    
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_FDTC | ACTIVATE_BUFFER);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Sensor_SetFaceDetectDisplayInfo
//  Description :
//------------------------------------------------------------------------------
/** @brief 	the function set necessary information to firmware for displaying face rectangles. 
           	This function can be called only when using firware to draw face rectangles to preview buffers
			The function set command to firmware to set necessary information for face rectangles displaying.

@param[in] bDrawFaceByFw draw face rectangles by FW or not.
@param[in] pipeID the IBC pipe ID of preview path.
@param[in] usPreviewW the width of preview buffer.
@param[in] usPreviewH the height of preview buffer.
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Sensor_SetFaceDetectDisplayInfo(	MMP_IBC_PIPEID 	pipeID, 
                                             	MMP_USHORT 		usPreviewW, 
                                             	MMP_USHORT 		usPreviewH)
{
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, (MMP_USHORT)pipeID);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 2, usPreviewW);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 4, usPreviewH);
    
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_FDTC | SET_DISPLAY_INFO);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Sensor_GetFDBufSize
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_Sensor_GetFDBufSize(	MMPD_SENSOR_FD_BUF_TYPE fdBufType,
									MMP_ULONG ulWidth, 
									MMP_ULONG ulHeight, 
									MMP_ULONG ulFaceNum,
									MMP_ULONG *ulSize)
{
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);

	switch(fdBufType){
	
	case MMPD_SENSOR_FD_WORK_BUF:
	    MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 0, ulWidth);
		MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 4, ulHeight);
	    MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 8, ulFaceNum);
		MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_FDTC | GET_FD_WORK_BUF_SIZE);
		*ulSize = MMPH_HIF_GetParameterL(GRP_IDX_FLOWCTL, 0);
		break;
	case MMPD_SENSOR_FD_INFO_BUF:
	    MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 0, ulFaceNum);
		MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_FDTC | GET_FD_INFO_BUF_SIZE);
		*ulSize = MMPH_HIF_GetParameterL(GRP_IDX_FLOWCTL, 0);
		break;
	case MMPD_SENSOR_SD_WORK_BUF:
	    MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 0, ulWidth);
		MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 4, ulHeight);
	    MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 8, ulFaceNum);
		MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_FDTC | GET_SD_WORK_BUF_SIZE);
		*ulSize = MMPH_HIF_GetParameterL(GRP_IDX_FLOWCTL, 0);
		break;

	}
	MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
	return MMP_ERR_NONE;
}
#endif

#if 0
void _____MDTC_Functions_____(){}
#endif

#if (SUPPORT_MDTC)
//------------------------------------------------------------------------------
//  Function    : MMPD_Sensor_InitializeVMD
//  Description :
//------------------------------------------------------------------------------
/** @brief Function to initialize VMD

The function initailizes the VMD configuration
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Sensor_InitializeVMD(void)
{
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_VMD | INIT_VMD);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Sensor_SetVMDBuf
//  Description :
//------------------------------------------------------------------------------
/** @brief Set working buffer address for motion detection operation.

The function set video motion detection buffer for operation
@param[in] buf_addr the address of motion detection working buffer
@param[out] buf_size the total size of working buffer
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Sensor_SetVMDBuf(MMP_ULONG buf_addr, MMP_ULONG *buf_size)
{
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 0, buf_addr);
    
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_VMD | SET_VMD_BUF);
    *buf_size = MMPH_HIF_GetParameterL(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Sensor_GetVMDResolution
//  Description :
//------------------------------------------------------------------------------
/** @brief the function gets the configured resolution of motion detection frame.

The function gets the configured resolution of motion detection frame.
@param[out] width   The width of motion detection source frame
@param[out] height  The height of motion detection source frame
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Sensor_GetVMDResolution(MMP_ULONG *width, MMP_ULONG *height)
{
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_VMD | GET_VMD_RESOL);
    *width  = MMPH_HIF_GetParameterL(GRP_IDX_FLOWCTL, 0);
    *height = MMPH_HIF_GetParameterL(GRP_IDX_FLOWCTL, 4);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Sensor_RegisterVMDCallback
//  Description :
//------------------------------------------------------------------------------
/** @brief the function configures the sensitivity of video motion detection.

The function set command to firmware to configures the sensitivity of motion detection.
@param[in] ulDiffThreshold The threshold of movement to be regarded as motion
@param[in] ulCntThreshold  The threshold of MB counts with difference larger than
                           ulDiffThreshold
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Sensor_RegisterVMDCallback(void *Callback)
{
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 0, (MMP_ULONG)Callback);

    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_VMD | REG_VMD_CALLBACK);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Sensor_EnableVMD
//  Description :
//------------------------------------------------------------------------------
/** @brief the function enable the video motion detection.

The function set command to firmware to start the video motion detection.
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Sensor_EnableVMD(MMP_BOOL bEnable)
{
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 0, (MMP_ULONG)bEnable);

    MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_VMD | ENABLE_VMD);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
    return MMP_ERR_NONE;
}
#endif

#if 0
void _____ADAS_Functions_____(){}
#endif

#if (SUPPORT_ADAS)
//------------------------------------------------------------------------------
//  Function    : MMPD_Sensor_InitializeADAS
//  Description :
//------------------------------------------------------------------------------
/** @brief Function to initialize ADAS

The function initailizes the ADAS configuration
@param[in] usWidth the width of input frame for ADAS detection
@param[in] usHeight the height of input frame for ADAS detection
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Sensor_InitializeADAS(void)
{
    MMP_ERR status;

    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    status = MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_ADAS | INIT_ADAS);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);

    return status;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Sensor_GetADASResolution
//  Description :
//------------------------------------------------------------------------------
/** @brief the function gets the configured resolution of ADAS frame.

The function gets the configured resolution of ADAS frame.
@param[out] width   The width of ADAS source frame
@param[out] height  The height of ADAS source frame
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Sensor_GetADASResolution(MMP_ULONG *width, MMP_ULONG *height)
{
    MMP_ERR status;

    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    status = MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_ADAS | GET_ADAS_RESOL);
    *width  = MMPH_HIF_GetParameterL(GRP_IDX_FLOWCTL, 0);
    *height = MMPH_HIF_GetParameterL(GRP_IDX_FLOWCTL, 4);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
    return status;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Sensor_SetADASFeature
//  Description :
//------------------------------------------------------------------------------
/** @brief Enable or disable each feature in ADAS.

The function control each feature within ADAS ON/OFF.
@param[in] ldws_on Enable LDWS feature
@param[in] fcws_on Enable FCWS feature
@param[in] sng_on Enable SnG feature
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Sensor_SetADASFeature(MMP_BOOL ldws_on, MMP_BOOL fcws_on, MMP_BOOL sag_on)
{
    MMP_ERR status;

    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterB(GRP_IDX_FLOWCTL, 0, ldws_on);
    MMPH_HIF_SetParameterB(GRP_IDX_FLOWCTL, 1, fcws_on);
    MMPH_HIF_SetParameterB(GRP_IDX_FLOWCTL, 2, sag_on);

    status = MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_ADAS | CTL_ADAS_MODE);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);

    return status;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Sensor_SetADASBuf
//  Description :
//------------------------------------------------------------------------------
/** @brief Set working buffer address for ADAS operation.

The function set video ADAS buffer for operation
@param[in] buf_addr the address of ADAS working buffer
@param[in] buf_addr the address of ADAS DMA destination buffer
@param[out] buf_size the total size of working buffer
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Sensor_SetADASBuf(MMP_ULONG buf_addr, MMP_ULONG *buf_size, MMP_ULONG dma_buf_addr)
{
    MMP_ERR status;

    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 0, buf_addr);
    MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 4, dma_buf_addr);
        
    status = MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_ADAS | SET_ADAS_BUF);
    *buf_size = MMPH_HIF_GetParameterL(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
    return status;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Sensor_EnableADAS
//  Description :
//------------------------------------------------------------------------------
/** @brief the function enable ADAS feature.

The function set command to firmware to start ADAS feature.
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Sensor_EnableADAS(MMP_BOOL bEnable)
{
    MMP_ERR status;

    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 0, (MMP_ULONG)bEnable);

    status = MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_ADAS | ENABLE_ADAS);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);

    return status;
}
#endif


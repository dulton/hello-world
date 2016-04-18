/// @ait_only
//==============================================================================
//
//  File        : mmpd_dsc.c
//  Description : Ritian Camera Control driver function
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "ait_utility.h"
#include "mmp_lib.h"
#include "lib_retina.h"
#include "mmp_register.h"
#include "mmp_dsc_inc.h"
#include "mmpd_dsc.h"
#include "mmpd_sensor.h"
#include "mmpd_scaler.h"
#include "mmpd_fctl.h"
#include "mmpd_system.h"
#include "mmph_hif.h"
#include "mmpf_dsc.h"
#include "mmpf_scaler.h"
#if (SUPPORT_DEC_MJPEG_TO_PREVIEW || HANDLE_JPEG_EVENT_BY_QUEUE)
#include "mmpf_usbh_ctl.h"
#endif

/** @addtogroup MMPD_DSC
 *  @{
 */

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#define MIN_EXTERNAL_COMPRESS_BUF_TH   	(256*1024) 	// 256KB is enough for MIN 320*240 preview 
#define MAX_EXTERNAL_COMPRESS_BUF_TH   	(2*1024*1024)

//==============================================================================
//
//                              GLOBAL VARIABLES
//
//==============================================================================
#if defined(ALL_FW)	
/** @brief	use stack/internal memory to capture JPEG file.
*/
static	MMP_ULONG		m_ulDSCFileNameAddr	= 0;

// For Exif initialization
static MMP_BOOL			bExifInitialize[DSC_MAX_EXIF_NODE_NUM] = {MMP_FALSE, MMP_FALSE};

#if (DSC_SUPPORT_BASELINE_MP_FILE)
// For MPF initialization
static MMP_BOOL			bMpfInitialize		    = MMP_FALSE;
#endif

// For MJPEG stream pipe selection
static MMPD_FCTL_LINK	m_MJPEGStreamSrcPipe    = {MMP_SCAL_PIPE_0, MMP_ICO_PIPE_0, MMP_IBC_PIPE_0};

//==============================================================================
//
//                              FUNCTIONS PROTOTYPE
//
//==============================================================================

#if 0
void _____Memory_Ctl_Functions_____(){}
#endif
//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_GetBufferSize
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_GetBufferSize(	MMPD_DSC_BUF_TYPE 	ubBufType,
							    MMP_DSC_JPEG_FMT 	jpegformat,
								MMP_USHORT 			usWidth,
								MMP_USHORT 			usHeight,
								MMP_ULONG			*ulSize)
{
	MMP_USHORT usTempFactor;
	
	switch(ubBufType){
	
	case MMPD_DSC_ENC_EXTERNAL_COMPRESS_BUFFER: //DRAM
        ///< Includes primary jpeg and thumbnail
        *ulSize = (usWidth * usHeight) * 6 / 10;
        break;

	case MMPD_DSC_ENC_VR_CAPTURE_COMP_BUFFER:
        ///< Includes primary jpeg and thumbnail
        *ulSize = (usWidth * usHeight) >> 1;

		*ulSize +=  256 *1024;  //welly for VR capture buffer Over
        ///< If uiSize too small, the liveview wouldn't streaming good
        ///< For video record with capture.
        if (*ulSize < MIN_EXTERNAL_COMPRESS_BUF_TH) {
            *ulSize = MIN_EXTERNAL_COMPRESS_BUF_TH;
        }
        
        /* To avoid no enough memory, we limit the max compress buf size,
         * but it might risky if encoded JPEG size is larger than it.
         */
        if (*ulSize > MAX_EXTERNAL_COMPRESS_BUF_TH) {
            *ulSize = MAX_EXTERNAL_COMPRESS_BUF_TH;
        }
		break;

	case MMPD_DSC_ENC_LARGE_THUMB_COMP_BUFFER:
        *ulSize = (usWidth * usHeight) >> 1;
        break;

	case MMPD_DSC_ENC_SINGLE_LINE_BUFFER:
		if ((usWidth < 1024) || (usWidth & 0x7F != 0)) {
			PRINTF("The width must large than 1024 and be multiple of 128\r\n");
			*ulSize = 0;
			return MMP_DSC_ERR_CAPTURE_FAIL;
		}

		*ulSize = usWidth*2*8; ///< width * 2 (byte per pixel, YUV422) * 8 (lines).
		break;
			
	case MMPD_DSC_ENC_DOUBLE_LINE_BUFFER:
		*ulSize = usWidth*2*16;///< width * 2 (byte per pixel, YUV422) * 16 (lines).
		break;
		
	case MMPD_DSC_DEC_DECOMPRESS_BUFFER:
		*ulSize = 4*1024*1024;
		break;
		
	case MMPD_DSC_DEC_SINGLE_LINE_BUFFER:
	case MMPD_DSC_DEC_DOUBLE_LINE_BUFFER:
		
		if (ubBufType == MMPD_DSC_DEC_SINGLE_LINE_BUFFER)
			usTempFactor = 1;
		else
			usTempFactor = 2;
			
	    switch(jpegformat){
    	
    	case MMP_DSC_JPEG_FMT411:
			*ulSize	= (usWidth*8*3)*usTempFactor;
        	break;      
   	 	case MMP_DSC_JPEG_FMT411_V:
			*ulSize	= (usWidth*4*8*3)*usTempFactor;
			break;	   	
    	case MMP_DSC_JPEG_FMT420:
			*ulSize	= (usWidth*2*8*3)*usTempFactor;
        	break;    	
    	case MMP_DSC_JPEG_FMT422:
			*ulSize	= (usWidth*8*3)*usTempFactor;
        	break;       	
	    case MMP_DSC_JPEG_FMT422_V:
			*ulSize	= (usWidth*2*8*3)*usTempFactor;
			break;				
    	case MMP_DSC_JPEG_FMT444:
			*ulSize	= (usWidth*8*3)*usTempFactor;
        	break;        	
		case MMP_DSC_JPEG_FMT_NOT_3_COLOR:
		case MMP_DSC_JPEG_FMT_NOT_BASELINE:
    	default:
        	PRINTF("Unsupported JPEG format !!!\r\n");
			return	MMP_DSC_ERR_CAPTURE_FAIL;
	    }
		break;

	case MMPD_DSC_EXIF_WORK_BUFFER:
		*ulSize = 0x40000;	// 256KB
		break;

	default:
		PRINTF("Unsupported Buffer Type !!!\r\n");
		*ulSize = 0;
		return MMP_DSC_ERR_CAPTURE_FAIL;
	}
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetRingBuffer
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_SetRingBuffer(MMP_ULONG ulStartAddr, MMP_ULONG ulSize, 
							   MMP_ULONG ulRsvdSize, MMP_ULONG ulInterval)
{
    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0,  ulStartAddr);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 4,  ulSize);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 8,  ulRsvdSize);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 12, ulInterval);

    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_SET_BUFFER | SET_RING_BUF);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

    return MMP_ERR_NONE;
}

#if 0
void _____Capture_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_EnableScaleUpCapture
//  Description : Enable scale up capture
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_EnableScaleUpCapture(MMP_BOOL bScaleUpCaptureEn)
{
	MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0, bScaleUpCaptureEn);
    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_SET_JPEG_ATTR | SET_SCALE_UP_CAPTURE);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetPreviewPath
//  Description : Set the pipe for DSC preview
//------------------------------------------------------------------------------
/** @brief Set the Image Path for DSC preview
@param[in] usScaPipe    capture scaler pipe
@param[in] usIcoPipe    catpure icon pipe
@param[in] usIBCPipe    capture use IBC Pipe 
@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_SetPreviewPath(MMP_USHORT	usScaPipe,
                                MMP_USHORT	usIcoPipe,
                                MMP_USHORT	usIbcPipe)
{
	MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 0, usScaPipe);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 2, usIcoPipe);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 4, usIbcPipe);

    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_SET_IMAGE_PATH | DISPLAY_PATH);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetCapturePath
//  Description : Set the pipe for DSC capture
//------------------------------------------------------------------------------
/** @brief Set the Image Path for DSC capture
@param[in] usScaPipe    capture scaler pipe
@param[in] usIcoPipe    catpure icon pipe
@param[in] usIBCPipe    capture use IBC Pipe
@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_SetCapturePath(MMP_USHORT	usScaPipe,
                                MMP_USHORT	usIcoPipe,
                                MMP_USHORT	usIbcPipe)
{
	MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 0, usScaPipe);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 2, usIcoPipe);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 4, usIbcPipe);

    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_SET_IMAGE_PATH | CAPTURE_PATH);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetCaptureBuffers
//  Description :
//------------------------------------------------------------------------------
/** @brief Configure the buffer for capture operation

The function sets the buffer address for compress buffer start, compress buffer end and line buffer 
@param[in] pCaptureBuf 	The buffer information for firmware to store compressed JPEG bitstream 
						and to set "Line Buffer" for incoming image.

@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_SetCaptureBuffers(MMP_DSC_CAPTURE_BUF *pCaptureBuf)
{
	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_TRUE);

	MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0, (MMP_ULONG) pCaptureBuf);

    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_SET_BUFFER | SET_ENC_BUF);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetJpegResol
//  Description :
//------------------------------------------------------------------------------
/** @brief Configure the JPEG resolution for Capture operation

The function sets jpeg resolution by saving it into internal system memory. The function sends host
command request to the firmware for the resolution setup.

@param[in] usJpegWidth 	Width for JPEG resolution
@param[in] usJpegHeight Height for JPEG resolution
@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_SetJpegResol(MMP_USHORT usJpegWidth, MMP_USHORT usJpegHeight, MMP_UBYTE ubRcIdx)
{	
	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_TRUE);
    
    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 0, usJpegWidth);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 2, usJpegHeight);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 4, ubRcIdx);

    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_SET_JPEG_ATTR | SET_JPEG_RESOL);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
    
	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetJpegQTable
//  Description :
//------------------------------------------------------------------------------
/** @brief Set the JPEG Q-Table

The function sets Jpeg image quality table. It is implemented by sending host command
request to the firmware.

@param[in] ubQtable the Jpeg Q-table.
@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_SetJpegQTable(MMP_UBYTE *ubQtable, MMP_UBYTE *ubUQtable, MMP_UBYTE *ubVQtable)
{
    MMP_ULONG i;

	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_TRUE);

    // Q table can only be accessed in BYTE
    for (i = 0; i < DSC_QTABLE_ARRAY_SIZE; i++) {
		MMPH_HIF_RegSetB((MMP_ULONG)AITC_BASE_TBL_Q + i, ubQtable[i]); 
	}
    for (i = 0; i < DSC_QTABLE_ARRAY_SIZE; i++) {
        MMPH_HIF_RegSetB((MMP_ULONG)AITC_BASE_TBL_Q + i + 0x40, ubUQtable[i]);  
	}
    for (i = 0; i < DSC_QTABLE_ARRAY_SIZE; i++) {
        MMPH_HIF_RegSetB((MMP_ULONG)AITC_BASE_TBL_Q + i + 0x80, ubVQtable[i]);   
	}

	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_FALSE);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetJpegQTableEx
//  Description :
//------------------------------------------------------------------------------
/** @brief Set the JPEG Q-Table to firmware

The function sets Jpeg image quality table. It is implemented by sending host command
request to the firmware.

@param[in] ubQtable the Jpeg Q-table.@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_SetJpegQTableEx(MMP_UBYTE ubIdx, MMP_UBYTE *ubQtable, MMP_UBYTE *ubUQtable, MMP_UBYTE *ubVQtable, MMP_DSC_JPEG_QTABLE QTSet)
{
	MMPF_DSC_SetQTableInfo(ubIdx, ubQtable, ubUQtable, ubVQtable, QTSet);    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetJpegQualityControl
//  Description :
//------------------------------------------------------------------------------
/** @brief Configure the JPEG quality control patameters

The function controls the Jpeg capturing quality by sending host command request
to the firmware and waiting for the DSC command clear.

@param[in] bTargetCtrl 	The flag of target control
@param[in] bLimitCtrl 	The flag of limit control
@param[in] usTargetSize The target size for the Jpeg encode (Unit:KB)
@param[in] usLimitSize 	The limit size for the Jpeg encode (Unit:KB)
@param[in] usMaxCount 	The maximum count for the Jpeg encode trial
@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_SetJpegQualityControl( MMP_UBYTE   ubRcIdx,
                                        MMP_BOOL    bTargetCtrl, 
                                        MMP_BOOL    bLimitCtrl, 
                                        MMP_ULONG   ulTargetSize,
                                        MMP_ULONG   ulLimitSize,
                                        MMP_USHORT  usMaxCount)
{
    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 0,  (MMP_USHORT)bTargetCtrl);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 2,  (MMP_USHORT)bLimitCtrl);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 4,  ulTargetSize);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 8,  ulLimitSize);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 12, usMaxCount);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 14, (MMP_USHORT)ubRcIdx);

    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_SET_JPEG_ATTR | SET_QSCALE_CTRL);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetRotateCapture
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_SetRotateCapture(MMP_DSC_CAPTURE_ROTATE_TYPE RotateType, MMP_ULONG ulFrameAddr)
{
    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0, ulFrameAddr);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 4, RotateType);
    
    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_SET_ROTATE_CAPTURE);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetStableShotParam
//  Description :
//------------------------------------------------------------------------------
/** @brief Configure DSC stable shot

The function sets DSC stable shot parameters 

@param[in] ulMainAddr First main compressed buffer address
@param[in] ulSubAddr Second sub compressed buffer address
@param[in] usShotNum Shot number
@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_SetStableShotParam(MMP_ULONG ulMainAddr, MMP_ULONG ulSubAddr, MMP_USHORT usShotNum)
{
    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0, ulMainAddr);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 4, ulSubAddr);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 8, usShotNum);
    
    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_JPEG_STABLESHOT | SET_STABLESHOT_PARAM);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_ChangeExtCompAddr
//  Description :
//------------------------------------------------------------------------------
/** @brief Change the setting of Jpeg capture extern compress buffer. 
@param[in] ulExtCompStartAddr extern compress buffer start address.
@param[in] ulExtCompEndAddr extern compress buffer end address.
@return It reports the status of the operation.
*/	
MMP_ERR MMPD_DSC_ChangeExtCompAddr(MMP_ULONG ulExtCompStartAddr, MMP_ULONG ulExtCompEndAddr)
{
	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_TRUE);
    
    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0, ulExtCompStartAddr);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 4, ulExtCompEndAddr);
    
    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_SET_BUFFER | CHANGE_EXT_BUF);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_FALSE);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_TakeJpegAfterSrcReady
//  Description : For video still capture
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_TakeJpegAfterSrcReady(MMP_UBYTE ubSnrSel)
{
    MMP_ERR mmpstatus = MMP_ERR_NONE;
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_TRUE);
    
    MMPF_DSC_ClearEncodeDoneStatus();

    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0, ubSnrSel);

    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_TAKE_JPEG | ONE_PIPE);

    if (mmpstatus == MMP_ERR_NONE) {
        mmpstatus = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
    }

    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_FALSE);

	if (mmpstatus) {
        return mmpstatus;
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_TakeJpegPicture
//  Description :
//------------------------------------------------------------------------------
/** @brief Capture picture into JPEG file format
The function takes the picture into default Jpeg capture output buffer 
@return It reports the status of the operation.
*/ 
MMP_ERR MMPD_DSC_TakeJpegPicture(MMP_UBYTE      ubSnrSel,
                                 MMP_BOOL 		bPreviewOn,
								 MMP_USHORT		usReviewTime,
								 MMP_USHORT		usWindowsTime,
								 MMP_USHORT  	usShotMode,
								 MMP_BOOL		bRestorePreview)
{
    MMP_ERR mmpstatus = MMP_ERR_NONE;
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, 		MMP_TRUE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S0, 	MMP_TRUE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S1, 	MMP_TRUE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_F, 	MMP_TRUE);

	MMPF_DSC_ClearEncodeDoneStatus();

    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 0,  (MMP_USHORT)ubSnrSel);
	MMPH_HIF_SetParameterW(GRP_IDX_DSC, 2,  (MMP_USHORT)bPreviewOn);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 4,  (MMP_USHORT)usReviewTime);
	MMPH_HIF_SetParameterW(GRP_IDX_DSC, 6,  (MMP_USHORT)usWindowsTime);
	MMPH_HIF_SetParameterW(GRP_IDX_DSC, 8,  (MMP_USHORT)usShotMode);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 10, (MMP_USHORT)bRestorePreview);
    
    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_TAKE_JPEG | TWO_PIPE);

    if (mmpstatus == MMP_ERR_NONE) {
        mmpstatus = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
    }

    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

    MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, 		MMP_FALSE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S0, 	MMP_FALSE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S1, 	MMP_FALSE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_F, 	MMP_FALSE);

	if (mmpstatus) {
        return mmpstatus;
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_TakeContinuousJpeg
//  Description :
//------------------------------------------------------------------------------
/** @brief Capture picture into JPEG file format
The function takes the picture into default Jpeg capture output buffer 
@return It reports the status of the operation.
*/ 
MMP_ERR MMPD_DSC_TakeContinuousJpeg(MMP_UBYTE   ubSnrSel,
                                    MMP_BOOL 	bPreviewOn,
									MMP_USHORT	usReviewTime,
									MMP_USHORT	usWindowsTime,
									MMP_USHORT  usShotMode,
									MMP_BOOL 	bContiShotEnable)
{
    MMP_ERR mmpstatus = MMP_ERR_NONE;

    if (bContiShotEnable == MMP_TRUE)
    {
        MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, 		MMP_TRUE);
        MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S0, 	MMP_TRUE);
        MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S1, 	MMP_TRUE);
        MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_F, 	MMP_TRUE);

        MMPF_DSC_ClearEncodeDoneStatus();

        MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
        MMPH_HIF_SetParameterW(GRP_IDX_DSC, 0,  (MMP_USHORT)ubSnrSel);
        MMPH_HIF_SetParameterW(GRP_IDX_DSC, 2,  (MMP_USHORT)bPreviewOn);
        MMPH_HIF_SetParameterW(GRP_IDX_DSC, 4,  (MMP_USHORT)usReviewTime);
        MMPH_HIF_SetParameterW(GRP_IDX_DSC, 6,  (MMP_USHORT)usWindowsTime);
        MMPH_HIF_SetParameterW(GRP_IDX_DSC, 8,  (MMP_USHORT)usShotMode);
        MMPH_HIF_SetParameterW(GRP_IDX_DSC, 10, (MMP_USHORT)MMP_FALSE);
 
        mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_TAKE_JPEG | TWO_PIPE);

        if (mmpstatus == MMP_ERR_NONE) {
            mmpstatus = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
        }

        MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
        return mmpstatus;
    }
    else {

        MMP_BOOL bDscJpgStreamBusy;
        
        // To stop continuous shot in ISR
        MMPF_JPG_ResetEncDoneMode();

		MMPH_HIF_WaitSem(GRP_IDX_DSC, 0); 

        do {
            MMPF_OS_Sleep(1);

            MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_MJPEG_OPERATION | GET_STREAM_STATUS);
            bDscJpgStreamBusy = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);

        } while (bDscJpgStreamBusy == MMP_TRUE);

  		MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

        MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, 		MMP_FALSE);
        MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S0, 	MMP_FALSE);
        MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S1, 	MMP_FALSE);
        MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_F, 	MMP_FALSE);
    }    

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_TakeNextJpegPicture
//  Description :
//------------------------------------------------------------------------------
/** @brief Capture picture into JPEG file format

The function takes the picture into default Jpeg capture output buffer by sending host command request
to the firmware and waiting for the DSC command clear.

@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_TakeNextJpegPicture(MMP_UBYTE ubSnrSel, MMP_USHORT usReviewTime)
{
    MMP_ERR mmpstatus = MMP_ERR_NONE;

    MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_TRUE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S0, MMP_TRUE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S1, MMP_TRUE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_F, MMP_TRUE);

	MMPF_DSC_ClearEncodeDoneStatus();

    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 0, ubSnrSel);
	MMPH_HIF_SetParameterW(GRP_IDX_DSC, 2, usReviewTime);
    
    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_TAKE_NEXT_JPEG);
    
    if (mmpstatus == MMP_ERR_NONE) {
        mmpstatus = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
    }
    
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
    
	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_FALSE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S0, MMP_FALSE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S1, MMP_FALSE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_F, MMP_FALSE);
    
    return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_RestorePreview
//  Description :
//------------------------------------------------------------------------------
/** @brief The function used in restore preview

The function restore preview for multishot capture

@param[in] bPreviewOn
@param[in] usWindowsTime 	 The time of review time
@param[in] bChangeSensorBack The change the sensor resolution as preview mode
@return MMP_ERR_NONE
*/
MMP_ERR MMPD_DSC_RestorePreview(MMP_UBYTE ubSnrSel, MMP_BOOL bPreviewOn, MMP_USHORT usWindowsTime)
{
    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterB(GRP_IDX_DSC, 0, ubSnrSel);
    MMPH_HIF_SetParameterB(GRP_IDX_DSC, 1, bPreviewOn);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 2, usWindowsTime);
    
    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_RESTORE_PREVIEW);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
    
    return MMP_ERR_NONE;
}

#if (VR_STILL_USE_REF_FRM)
//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_VidRefBuf2Jpeg
//  Description : Take Jpeg from H.264 reference frame by GRA.
//------------------------------------------------------------------------------
/** @brief Issue request to take JPEG by graphic loop-back path, the source of
           graphic is H.264 reference frame
The function takes the picture with source from H.264 reference frame.
@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_VidRefBuf2Jpeg(void)
{
    MMP_ERR mmpstatus;

    MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_TRUE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_TRUE);

    MMPF_DSC_ClearEncodeDoneStatus();

    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);

    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_TAKE_H264_JPEG);

	if (mmpstatus == MMP_ERR_NONE) {
    	mmpstatus = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
	}

    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

    MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_FALSE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_FALSE);

    return mmpstatus;
}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_RawBuf2Jpeg
//  Description : Take Jpeg from GRA.
//------------------------------------------------------------------------------
/** @brief Capture picture into JPEG file format
The function takes the picture into default Jpeg capture output buffer 
@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_RawBuf2Jpeg(MMP_GRAPHICS_BUF_ATTR	*pBufAttr,
			                 MMP_GRAPHICS_RECT      *pRect,
			                 MMP_USHORT             usUpscale,
			                 MMPD_FCTL_ATTR 		*pFctlAttr)
{
	MMP_USHORT	usJpegWidth, usJpegHeight;
	MMP_ERR 	mmpstatus;
	
	usJpegWidth   = (pFctlAttr->grabctl).ulOutEdX - (pFctlAttr->grabctl).ulOutStX + 1;
	usJpegHeight  = (pFctlAttr->grabctl).ulOutEdY - (pFctlAttr->grabctl).ulOutStY + 1;

    // The width of JPEG must be 16x, the height of JPEG must be 8x.
	if ((usJpegWidth & 0xF) || (usJpegHeight & 0x7)) {	
		PRINTF("The W is not 16x or H is not 8x\r\n");
		return MMP_DSC_ERR_PARAMETER;
    }

	// Setting for image pipe
	MMPD_Graphics_SetDelayType(MMP_GRAPHICS_DELAY_CHK_SCA_BUSY);
    MMPD_Graphics_SetPixDelay(7, 20);
    MMPD_Graphics_SetLineDelay(1);
	MMPD_Graphics_SetScaleAttr(pBufAttr, pRect, usUpscale);

	MMPD_Fctl_SetPipeAttrForJpeg(pFctlAttr, MMP_TRUE, MMP_FALSE);

	MMPD_Scaler_SetEnable(pFctlAttr->fctllink.scalerpath, MMP_TRUE);

	// Setting for JPEG engine
    MMPD_DSC_SetJpegResol(usJpegWidth, usJpegHeight, MMP_DSC_JPEG_RC_ID_CAPTURE);

	MMPD_System_EnableClock(MMPD_SYS_CLK_GRA,   MMP_TRUE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_JPG,   MMP_TRUE);
   	MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);
   	MMPD_System_EnableClock(MMPD_SYS_CLK_ICON,  MMP_TRUE);
   	MMPD_System_EnableClock(MMPD_SYS_CLK_IBC,   MMP_TRUE);

	MMPF_DSC_ClearEncodeDoneStatus();

    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    
   	if (pBufAttr->colordepth == MMP_GRAPHICS_COLORDEPTH_YUV420) {
		MMPH_HIF_SetParameterB(GRP_IDX_DSC, 0, MMPF_DSC_SRC_GRAPHICS_YUV420);
	}
   	else if (pBufAttr->colordepth == MMP_GRAPHICS_COLORDEPTH_YUV420_INTERLEAVE) {
		MMPH_HIF_SetParameterB(GRP_IDX_DSC, 0, MMPF_DSC_SRC_GRAPHICS_NV12);
	}
	else {
		MMPH_HIF_SetParameterB(GRP_IDX_DSC, 0, MMPF_DSC_SRC_GRAPHICS);
	}

    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_TAKE_GRA_JPEG);

	if (mmpstatus == MMP_ERR_NONE) {
    	mmpstatus = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
	}

    MMPD_Scaler_SetEnable(pFctlAttr->fctllink.scalerpath, MMP_FALSE);

    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
    
	MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, 	MMP_FALSE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, 	MMP_FALSE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, 	MMP_FALSE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, 	MMP_FALSE);
	    
	return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_GetJpegPicture
//  Description :
//------------------------------------------------------------------------------
/** @brief The function reads the compressed data from the FIFO port and writes into the input Jpeg buffer

After the whole set of compressed data has been transferred to the input Jpeg buffer, 
the Jpeg block clock can be disabled. 

Normally the function can be called after the call of MMPD_DSC_TakeJpegPicture to get the exact capture size in order to prepare
enough Jpeg buffer size to hold the compressed Jpeg data.

@param[out] usJpegBufAddr 	the jpeg buffer address
@param[in] 	ulHostBufLimit 	the limit size in host
@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_GetJpegPicture(MMP_USHORT *usJpegBufAddr, MMP_ULONG ulHostBufLimit)
{
	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_TRUE);

    MMPF_DSC_GetJpegBitStream(usJpegBufAddr, ulHostBufLimit);

	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_FALSE);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_GetJpegSize
//  Description :
//------------------------------------------------------------------------------
/** @brief Get JPEG jpeg data size

The function retrives the jpeg data size
@param[out] ulJpegFileSize the jpeg size
@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_GetJpegSize(MMP_ULONG *ulJpegFileSize)
{
	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_TRUE);
	
	MMPF_DSC_GetJpegSize(MMP_FALSE, ulJpegFileSize);

	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_FALSE);

    return MMP_ERR_NONE;
} 

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_GetJpegSizeAfterEncodeDone
//  Description : For Dual Cam case
//------------------------------------------------------------------------------
/** @brief Get JPEG jpeg data size after encode done (non-blocking)

The function retrives the jpeg data size
@param[out] ulJpegFileSize the jpeg size
@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_GetJpegSizeAfterEncodeDone(MMP_ULONG *ulJpegFileSize)
{
    MMPF_DSC_GetJpegSizeAfterEncodeDone(ulJpegFileSize);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_EncodeThumb
//  Description : TBD for LDC video still capture
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_EncodeThumb(MMP_UBYTE          ubSnrSel,
                             MMP_SCAL_PIPEID	srcPath,
                             MMP_SCAL_PIPEID 	dstPath,
                             MMPD_FCTL_ATTR 	*pFctlAttr)
{
	MMP_ERR 	mmpstatus 		= MMP_ERR_NONE;
	MMP_USHORT	usJpegWidth   	= (pFctlAttr->grabctl).ulOutEdX - (pFctlAttr->grabctl).ulOutStX + 1;
	MMP_USHORT	usJpegHeight	= (pFctlAttr->grabctl).ulOutEdY - (pFctlAttr->grabctl).ulOutStY + 1;

	MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_ICON,	MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, 	MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG,   MMP_TRUE);

    /* Switch scaler source to other source first before update pipe settings */
    MMPD_Scaler_SetSerialLinkPipe(srcPath, dstPath);

	MMPD_Fctl_SetPipeAttrForJpeg(pFctlAttr, MMP_TRUE, MMP_FALSE);
	
	MMPD_Scaler_SetEnable(pFctlAttr->fctllink.scalerpath, MMP_TRUE);
    
    #if (MCR_V2_UNDER_DBG)
    /* Have to reset scaler because of double frame start, to be checked */
    MMPD_Scaler_ResetModule(pFctlAttr->fctllink.scalerpath);
    #endif

	// Setting for JPEG engine
    MMPD_DSC_SetJpegResol(usJpegWidth, usJpegHeight, MMP_DSC_JPEG_RC_ID_CAPTURE);
    
	MMPF_DSC_ClearEncodeDoneStatus();

    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0, ubSnrSel);
    
   	mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_TAKE_JPEG | ONE_PIPE);
    
    if (mmpstatus == MMP_ERR_NONE) {
        mmpstatus = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
    }

    MMPD_Scaler_SetEnable(pFctlAttr->fctllink.scalerpath, MMP_FALSE);

    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_StoreThumbSrcFrame
//  Description : Store thumbnail source frame into IBC buffer
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_StoreThumbSrcFrame(MMP_GRAPHICS_BUF_ATTR 	*pBufAttr,
									MMP_SCAL_FIT_RANGE		*pFitrange,
                                 	MMP_SCAL_GRAB_CTRL 		*pGrabctl,
                                 	MMP_SCAL_PIPEID 		srcPath,
                                 	MMP_SCAL_PIPEID 		dstPath)
{
	MMPD_FCTL_LINK 		fctllink;
	MMP_ICO_PIPE_ATTR	icoAttr;
   	MMP_IBC_PIPE_ATTR	ibcAttr;

	MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_ICON,	MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, 	MMP_TRUE);

    /* Switch scaler source to other source first before update pipe settings */
    MMPD_Scaler_SetSerialLinkPipe(srcPath, dstPath);

	fctllink.scalerpath = dstPath;
	fctllink.icopipeID 	= (MMP_ICO_PIPEID)dstPath;
	fctllink.ibcpipeID 	= (MMP_IBC_PIPEID)dstPath;

	// Config IBC module
	ibcAttr.ulBaseAddr 		= pBufAttr->ulBaseAddr;
	ibcAttr.ulBaseUAddr 	= pBufAttr->ulBaseUAddr;
	ibcAttr.ulBaseVAddr 	= pBufAttr->ulBaseVAddr;
    ibcAttr.ulLineOffset 	= 0;
    ibcAttr.colorformat     = MMP_IBC_COLOR_YUV444_2_YUV422_UYVY;
	ibcAttr.function 		= MMP_IBC_FX_TOFB;
	ibcAttr.InputSource		= fctllink.icopipeID;
	ibcAttr.bMirrorEnable   = MMP_FALSE;
	MMPD_IBC_SetAttributes(fctllink.ibcpipeID, &ibcAttr);
	MMPD_IBC_SetStoreEnable(fctllink.ibcpipeID, MMP_TRUE);
	
	// Config Icon module
    icoAttr.inputsel	= fctllink.scalerpath;
	icoAttr.bDlineEn	= MMP_TRUE;
	icoAttr.usFrmWidth 	= pGrabctl->ulOutEdX - pGrabctl->ulOutStX + 1;
	MMPD_Icon_SetDLAttributes(fctllink.icopipeID, &icoAttr);
	MMPD_Icon_SetDLEnable(fctllink.icopipeID, MMP_TRUE);
	
	// Config Scaler module
	MMPD_Scaler_SetOutColor(fctllink.scalerpath, MMP_SCAL_COLOR_YUV444);
	#if (CCIR656_FORCE_SEL_BT601)
	MMPD_Scaler_SetOutColorTransform(fctllink.scalerpath, MMP_TRUE, MMP_SCAL_COLRMTX_FULLRANGE_TO_BT601);
    #else
	MMPD_Scaler_SetOutColorTransform(fctllink.scalerpath, MMP_TRUE, MMP_SCAL_COLRMTX_YUV_FULLRANGE);
    #endif
	MMPD_Scaler_SetEngine(MMP_SCAL_USER_DEF_TYPE_IN_OUT, fctllink.scalerpath, pFitrange, pGrabctl);
	MMPD_Scaler_SetLPF(fctllink.scalerpath, pFitrange, pGrabctl);
	
	MMPD_Scaler_SetEnable(fctllink.scalerpath, MMP_TRUE);
    
    #if (MCR_V2_UNDER_DBG)
    /* Have to reset scaler because of double frame start, to be checked */
    MMPD_Scaler_ResetModule(fctllink.scalerpath);
    #endif

    MMPD_IBC_CheckFrameReady(fctllink.ibcpipeID);
	MMPD_IBC_SetStoreEnable(fctllink.ibcpipeID, MMP_FALSE);

	MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_ICON,	MMP_FALSE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, 	MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetContiShotParam
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_SetContiShotParam(MMP_ULONG ulExifBufAddr, MMP_ULONG ulMaxShotNum)
{
	MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0, ulExifBufAddr);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 4, ulMaxShotNum);

    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_SET_JPEG_ATTR | SET_CONTI_SHOT_PARAM);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_CheckContiShotStreamBusy
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_CheckContiShotStreamBusy(MMP_ULONG *pbBusy)
{
	MMP_ERR	 mmpstatus;

	MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);

    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_MJPEG_OPERATION | GET_STREAM_STATUS);

	if (mmpstatus == MMP_ERR_NONE) {
    	*pbBusy = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
	}

    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

    return MMP_ERR_NONE;
}

#if 0
void _____MJPEG_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_InitGra2MJPEGAttr
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_InitGra2MJPEGAttr(	MMPD_FCTL_LINK			fctllink,
									MMP_GRAPHICS_BUF_ATTR  	*pBufAttr,
									MMP_GRAPHICS_RECT      	*pRect,
			                   		MMP_SCAL_FIT_RANGE    	*pFitrange,
			                   		MMP_SCAL_GRAB_CTRL  	*pGrabctl)
{
	MMP_USHORT      usJpegW, usJpegH;
	MMPD_FCTL_ATTR	fctlAttr;

	m_MJPEGStreamSrcPipe = fctllink;

    usJpegW = pGrabctl->ulOutEdX - pGrabctl->ulOutStX + 1;
	usJpegH = pGrabctl->ulOutEdY - pGrabctl->ulOutStY + 1;
    
    // The width of JPEG must be 16x, the height of JPEG must be 8x.
	if ((usJpegW & 0xF) || (usJpegH & 0x7)) {	
		return MMP_DSC_ERR_PARAMETER;
    }

    // Config GRA module
	MMPD_Graphics_SetDelayType(MMP_GRAPHICS_DELAY_CHK_SCA_BUSY);
    MMPD_Graphics_SetPixDelay(15, 20);  //For G0 264MHz
    MMPD_Graphics_SetLineDelay(1);
	MMPD_Graphics_SetScaleAttr(pBufAttr, pRect, 1);

    fctlAttr.fctllink       = fctllink;
    fctlAttr.fitrange       = *pFitrange;
    fctlAttr.grabctl        = *pGrabctl;
    fctlAttr.scalsrc		= MMP_SCAL_SOURCE_GRA;
    fctlAttr.bSetScalerSrc	= MMP_TRUE;

    MMPD_Fctl_SetPipeAttrForJpeg(&fctlAttr, MMP_TRUE, MMP_TRUE);

	MMPD_Scaler_SetEnable(fctllink.scalerpath, MMP_TRUE);

	// Setting for JPEG engine
    MMPD_DSC_SetJpegResol(usJpegW, usJpegH, MMP_DSC_JPEG_RC_ID_MJPEG_1ST_STREAM);
	
    MMPD_DSC_SetCapturePath(fctllink.scalerpath,
            				fctllink.icopipeID,
            				fctllink.ibcpipeID);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetMJPEGPipe
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_SetMJPEGPipe(MMP_UBYTE ubPipe)
{
	m_MJPEGStreamSrcPipe.scalerpath = (MMP_SCAL_PIPEID)ubPipe;
	m_MJPEGStreamSrcPipe.icopipeID  = (MMP_ICO_PIPEID)ubPipe;
	m_MJPEGStreamSrcPipe.ibcpipeID  = (MMP_IBC_PIPEID)ubPipe;
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_StartMJPEGStream
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_StartMJPEGStream(MMP_UBYTE ubSnrSel)
{
	MMP_ERR mmpstatus;
	
	MMPD_Scaler_SetEnable(m_MJPEGStreamSrcPipe.scalerpath, MMP_TRUE);
	
    MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_TRUE);
   	MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_TRUE);
   	MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_TRUE);
   	MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);

	MMPF_DSC_ClearEncodeDoneStatus();

    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0, ubSnrSel);

    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_MJPEG_OPERATION | START_MJPEG);

	if (mmpstatus == MMP_ERR_NONE) {
    	mmpstatus = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
	}
 
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
    
    return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_StopMJPEGStream
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_StopMJPEGStream(MMP_USHORT usEncID)
{    
	MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);

    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0, usEncID);
    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_MJPEG_OPERATION | STOP_MJPEG);
 
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
    
    MMPD_Scaler_SetEnable(m_MJPEGStreamSrcPipe.scalerpath, MMP_FALSE);
    
    #if (HANDLE_JPEG_EVENT_BY_QUEUE)
    // TBD
    #else
    MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_FALSE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_FALSE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_FALSE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);
    #endif

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetMJPEGFpsInfo
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_SetMJPEGFpsInfo(MMP_ULONG ulInc, MMP_ULONG ulInRes, MMP_ULONG ulOutRes)
{
	MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
	MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0, ulInc);
	MMPH_HIF_SetParameterL(GRP_IDX_DSC, 4, ulInRes);
	MMPH_HIF_SetParameterL(GRP_IDX_DSC, 8, ulOutRes);

    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_MJPEG_OPERATION | SET_FPS);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetVideo2MJPEGEnable
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_SetVideo2MJPEGEnable(MMP_BOOL bEnable)
{
#if (VIDEO_DEC_TO_MJPEG)
	MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
	MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0, bEnable);

    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_MJPEG_OPERATION | SET_VID2MJPEG_ENABLE);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
#endif
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_GetRearCamBufForDualStreaming
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_GetRearCamBufForDualStreaming(MMP_ULONG *pulCompAddr, MMP_ULONG *pulCompSize, MMP_ULONG *pulLineBuf)
{
    MMP_ERR err = MMP_ERR_NONE;
#if (USB_EN)&&(SUPPORT_USB_HOST_FUNC)&&(SUPPORT_DEC_MJPEG_TO_PREVIEW)
	err = MMPF_USBH_GetRearCamBufForDualStreaming(pulCompAddr, pulCompSize, pulLineBuf);
#endif
    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetDualStreamingEnable
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_SetDualStreamingEnable(MMP_BOOL bFrontCamEn, MMP_BOOL bRearCamEn)
{
    MMP_ERR err = MMP_ERR_NONE;
#if (HANDLE_JPEG_EVENT_BY_QUEUE)
	err = MMPF_USBH_SetDualStreamingEnable(bFrontCamEn, bRearCamEn);
#endif
    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetDecMjpegToPreviewSrcAttr
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_SetDecMjpegToPreviewSrcAttr(MMP_USHORT usSrcW, MMP_USHORT usSrcH)
{
    MMP_ERR err = MMP_ERR_NONE;
#if (USB_EN)&&(SUPPORT_USB_HOST_FUNC)&&(SUPPORT_DEC_MJPEG_TO_PREVIEW)
	err = MMPF_USBH_SetDecMjpegToPreviewSrcAttr(usSrcW, usSrcH);
#endif
    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetDecMjpegToPreviewPipe
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_SetDecMjpegToPreviewPipe(MMP_UBYTE ubPipe)
{
    MMP_ERR err = MMP_ERR_NONE;
#if (USB_EN)&&(SUPPORT_USB_HOST_FUNC)&&(SUPPORT_DEC_MJPEG_TO_PREVIEW)
	err = MMPF_USBH_SetDecMjpegToPreviewPipe(ubPipe);
#endif
	return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetDecMjpegToEncodePipe
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_SetDecMjpegToEncodePipe(MMP_UBYTE ubPipe)
{
    MMP_ERR err = MMP_ERR_NONE;
#if (USB_EN)&&(SUPPORT_USB_HOST_FUNC)&&(SUPPORT_DEC_MJPEG_TO_PREVIEW)
	err = MMPF_USBH_SetDecMjpegToEncodePipe(ubPipe);
#endif
	return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetDecMjpegToPreviewJpegBuf
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_SetDecMjpegToPreviewJpegBuf(MMP_ULONG ulAddr, MMP_ULONG* pulSize)
{
    MMP_ERR err = MMP_ERR_NONE;
#if (USB_EN)&&(SUPPORT_USB_HOST_FUNC)&&(SUPPORT_DEC_MJPEG_TO_PREVIEW)
	err = MMPF_USBH_SetDecMjpegToPreviewJpegBuf(ulAddr, pulSize);
#endif
	return err;
}

#if 0
void _____Playback_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SelectJpegFormat
//  Description :
//------------------------------------------------------------------------------
/** @brief	select the format of Jpeg depending on the value of JPG_EXIF_SOF

@param[in] 	usFormatType 	the value of JPG_EXIF_SOF
@param[out] jpegformat 		the format of jpeg
@return It reports the status of the operation.
*/
static MMP_ERR MMPD_DSC_SelectJpegFormat(MMP_UBYTE ubFormatType, MMP_DSC_JPEG_FMT* pJpegFormat)
{
    if (ubFormatType == 0x22)         		
        *pJpegFormat = MMP_DSC_JPEG_FMT420;
    else if (ubFormatType == 0x21)        	
        *pJpegFormat = MMP_DSC_JPEG_FMT422;
    else if (ubFormatType == 0x11)        	
        *pJpegFormat = MMP_DSC_JPEG_FMT444;
    else if (ubFormatType == 0x41)        	
        *pJpegFormat = MMP_DSC_JPEG_FMT411;
	else if (ubFormatType == 0x12)
		*pJpegFormat = MMP_DSC_JPEG_FMT422_V;
	else if (ubFormatType == 0x14)
		*pJpegFormat = MMP_DSC_JPEG_FMT411_V;
	else {
		*pJpegFormat = MMP_DSC_JPEG_FMT_NOT_BASELINE;
		return MMP_DSC_ERR_JPEGINFO_FAIL;
	}
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_GetJfifTagInDev
//  Description :
//------------------------------------------------------------------------------
/** @brief Get the JFIF tag of image

The function retrives the JFIF tag from an image buffer. It is an internal function to MMPD_DSC.
@param[out] usTagID the tag ID
@param[out] usTagLength the tag length
@param[in] ulImgAddr the image buffer address
@param[in] ulLength the length
@return It reports the status of the operation.
*/
static MMP_ERR MMPD_DSC_GetJfifTagInDev(MMP_USHORT 	*usTagID, 
										MMP_USHORT 	*usTagLength,
                                     	MMP_ULONG 	*ulImgAddr, 
                                     	MMP_ULONG 	ulLength)
{
    MMP_ULONG i = 0;

    while (MMPH_HIF_MemGetB(*ulImgAddr+i) != JPG_EXIF_PREFIX && i < ulLength) {
        i++;
    }

    if (i >= ulLength) {
        return MMP_DSC_ERR_JPEGINFO_FAIL;
	}

    *ulImgAddr 	= *ulImgAddr + i;
    *usTagID 	= (MMPH_HIF_MemGetB(*ulImgAddr) << 8) + MMPH_HIF_MemGetB(*ulImgAddr + 1);
    
    if (*usTagID >= JPG_EXIF_RST && *usTagID <= JPG_EXIF_EOI) {
        *usTagLength = 0;
    }
    else {
        *usTagLength = (MMPH_HIF_MemGetB(*ulImgAddr + 2) << 8) + MMPH_HIF_MemGetB(*ulImgAddr + 3);
    }

    return MMP_ERR_NONE;
}
#endif //#if defined(ALL_FW)

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetDecodeSpeed
//  Description :
//------------------------------------------------------------------------------
/**	@brief	adjust the pixel delay of JPEG engine automatically or not.
*/
static MMP_ERR MMPD_DSC_SetDecodeSpeed(MMP_BOOL bAuto, MMP_UBYTE ubPixelDelay)
{
	MMP_ULONG ulG0Freq;
	MMP_ULONG ulG1Freq;
	MMP_ULONG ulPixelDelay;

	if (bAuto) {
		MMPD_System_GetGroupFreq(CLK_GRP_GBL,   &ulG0Freq);
		MMPD_System_GetGroupFreq(CLK_GRP_COLOR, &ulG1Freq);

        /** @brief	Ceil[ G0_clk_rate / (G1_clk_rate/2) ] - 1 */
		ulPixelDelay = ( ((ulG0Freq<<1) + ulG1Freq - 1)/ ulG1Freq ) - 1;
	}
    else {
		ulPixelDelay = ubPixelDelay;
	}

	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_TRUE);
	
	MMPF_DSC_SetJpgDecOutPixelDelay(ulPixelDelay);

	MMPF_DSC_SetJpgDecOutBusy(MMP_TRUE);
		
	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_FALSE);
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_DecodeJpegPic
//  Description : Decode JPEG file into IBC buffer
//------------------------------------------------------------------------------
/** @brief Decode JPEG file into buffer

==========================INPUT Path Selection=============================
if (jpeginfo->ulJpegBufAddr != 0)
	input path (Memory mode) : Memory or External memory -> JPEG
else if (jpeginfo->bJpegFileName != NULL)
	input path (FIFO mode): SD card -> CPU -> FIFO -> compressed buf -> JPEG

@param[in] dispColor 		The output color format
@param[in] pJpegInfo 		The jpeg info
@param[in] bufAttr 	        The buffer attrribute
@param[in] grabctl 			The grab and scale control parameters
@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_DecodeJpegPic(	MMP_DISPLAY_COLORMODE 	dispColor,
                                MMP_DSC_JPEG_INFO 		*pJpegInfo, 
                                MMP_GRAPHICS_BUF_ATTR 	*pBufAttr,
                                MMP_SCAL_FIT_RANGE      *pFitrange,
                                MMP_SCAL_GRAB_CTRL 		*pGrabctl,
                                MMP_SCAL_PIPEID 		pipeID)
{
	MMP_ERR		         	mmpstatus;
	MMPD_FCTL_LINK 			fctllink;
	MMP_ICO_PIPE_ATTR 		icoAttr;
   	MMP_IBC_PIPE_ATTR		ibcAttr;
   	MMP_SCAL_COLORMODE		scal_outcolor;

	/* Format check */
#if defined(ALL_FW)	
    if (!pJpegInfo->bValid) {
        if (MMPD_DSC_GetJpegInfo(pJpegInfo) != MMP_ERR_NONE) {
			return MMP_DSC_ERR_PLAYBACK_FAIL;
	    }
    }
#endif
    if ((pJpegInfo->jpgFormat == MMP_DSC_JPEG_FMT_NOT_3_COLOR) ||
    	(pJpegInfo->jpgFormat == MMP_DSC_JPEG_FMT_NOT_BASELINE)){

        PRINTF("Unsupported JPEG format !!\r\n");
        return MMP_DSC_ERR_PLAYBACK_FAIL;
    }

	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, 	MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_ICON,	MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, 	MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_ISP,	MMP_TRUE);

	MMPD_DSC_SetDecodeSpeed(MMP_TRUE, 0);

    /* Switch scaler source to JPEG first before update pipe settings */
    MMPD_Scaler_SetPath(pipeID, MMP_SCAL_SOURCE_JPG, MMP_TRUE);

	fctllink.scalerpath = pipeID;
	fctllink.icopipeID 	= (MMP_ICO_PIPEID)pipeID;
	fctllink.ibcpipeID 	= (MMP_IBC_PIPEID)pipeID;
	
	pBufAttr->usWidth 	= pGrabctl->ulOutEdX - pGrabctl->ulOutStX + 1;
    pBufAttr->usHeight 	= pGrabctl->ulOutEdY - pGrabctl->ulOutStY + 1;

	// Config IBC module
	switch (dispColor) {
	
    case MMP_DISPLAY_COLOR_RGB565:
		pBufAttr->usLineOffset	= pBufAttr->usWidth * 2;
        pBufAttr->colordepth   	= MMP_GRAPHICS_COLORDEPTH_16;
		ibcAttr.colorformat 	= MMP_IBC_COLOR_RGB565;
		scal_outcolor 			= MMP_SCAL_COLOR_RGB565;
	    break;

    case MMP_DISPLAY_COLOR_RGB888:
       	pBufAttr->usLineOffset  = pBufAttr->usWidth * 3;
        pBufAttr->colordepth 	= MMP_GRAPHICS_COLORDEPTH_24;
		ibcAttr.colorformat	    = MMP_IBC_COLOR_RGB888;
    	scal_outcolor 			= MMP_SCAL_COLOR_RGB888;
	    break;

    case MMP_DISPLAY_COLOR_YUV422:
        pBufAttr->usLineOffset  = pBufAttr->usWidth * 2;
        pBufAttr->colordepth    = MMP_GRAPHICS_COLORDEPTH_YUV422_UYVY;
		ibcAttr.colorformat     = MMP_IBC_COLOR_YUV444_2_YUV422_UYVY;
    	scal_outcolor 	        = MMP_SCAL_COLOR_YUV444;
        break;

	case MMP_DISPLAY_COLOR_YUV420:
//	    if (pBufAttr->usWidth & 0x0F) {  // Width must be a multiple of 16 //no this limitaion
//			return MMP_DSC_ERR_PARAMETER;
//        }
	    pBufAttr->usLineOffset  = pBufAttr->usWidth;
        pBufAttr->colordepth    = MMP_GRAPHICS_COLORDEPTH_YUV420;
		ibcAttr.colorformat     = MMP_IBC_COLOR_I420;
    	scal_outcolor 			= MMP_SCAL_COLOR_YUV444;
	    break;

    case MMP_DISPLAY_COLOR_YUV420_INTERLEAVE:
	    if (pBufAttr->usWidth & 0x0F) {  // Width must be a multiple of 16
			return MMP_DSC_ERR_PARAMETER;
        }
	    pBufAttr->usLineOffset  = pBufAttr->usWidth;
        pBufAttr->colordepth    = MMP_GRAPHICS_COLORDEPTH_YUV420_INTERLEAVE;
		ibcAttr.colorformat     = MMP_IBC_COLOR_NV12;
    	scal_outcolor 			= MMP_SCAL_COLOR_YUV444;    
        break;
    }
    
	ibcAttr.ulBaseAddr 		= pBufAttr->ulBaseAddr;
	ibcAttr.ulBaseUAddr 	= pBufAttr->ulBaseUAddr;
	ibcAttr.ulBaseVAddr 	= pBufAttr->ulBaseVAddr;
    ibcAttr.ulLineOffset 	= 0;
	ibcAttr.function 		= MMP_IBC_FX_TOFB;
	ibcAttr.InputSource		= fctllink.icopipeID;
	ibcAttr.bMirrorEnable	= MMP_FALSE;
	MMPD_IBC_SetAttributes(fctllink.ibcpipeID, &ibcAttr);
	MMPD_IBC_SetStoreEnable(fctllink.ibcpipeID, MMP_TRUE);
	
	// Config Icon module
    icoAttr.inputsel	= fctllink.scalerpath;
	icoAttr.bDlineEn	= MMP_TRUE;
	icoAttr.usFrmWidth 	= pGrabctl->ulOutEdX - pGrabctl->ulOutStX + 1;
	MMPD_Icon_SetDLAttributes(fctllink.icopipeID, &icoAttr);
	MMPD_Icon_SetDLEnable(fctllink.icopipeID, MMP_TRUE);
	
	// Config Scaler module
	MMPD_Scaler_SetOutColor(fctllink.scalerpath, scal_outcolor);
	
	if (scal_outcolor == MMP_SCAL_COLOR_RGB565 || 
		scal_outcolor == MMP_SCAL_COLOR_RGB888) {
		MMPD_Scaler_SetOutColorTransform(fctllink.scalerpath, MMP_TRUE, MMP_SCAL_COLRMTX_FULLRANGE_TO_RGB);
	}
	else {
        #if (CCIR656_FORCE_SEL_BT601)
        MMPD_Scaler_SetOutColorTransform(fctllink.scalerpath, MMP_TRUE, MMP_SCAL_COLRMTX_FULLRANGE_TO_BT601);
        #else
        MMPD_Scaler_SetOutColorTransform(fctllink.scalerpath, MMP_TRUE, MMP_SCAL_COLRMTX_YUV_FULLRANGE);
        #endif
	}

	MMPD_Scaler_SetEngine(MMP_SCAL_USER_DEF_TYPE_IN_OUT, fctllink.scalerpath, pFitrange, pGrabctl);
	MMPD_Scaler_SetLPF(fctllink.scalerpath, pFitrange, pGrabctl);
	MMPD_Scaler_SetEnable(fctllink.scalerpath, MMP_TRUE);

    #if (MCR_V2_UNDER_DBG)
    /* Have to reset scaler because of double frame start, to be checked */
    /* With scaler reset, IBC pipe1 seems writes more 128-Byte 
     * and overwritten EXIF buf, to-be-check
     */
    //MMPD_Scaler_ResetModule(fctllink.scalerpath);
    #endif
    MMPD_System_ResetHModule(MMPD_SYS_MDL_JPG, MMP_FALSE);

	if (pJpegInfo->ulJpegBufAddr != 0) {
#if defined(MBOOT_FW)
        MMPD_DSC_SetDecodeSpeed(MMP_FALSE, 7);
        mmpstatus= MMPF_DSC_DecodeJpegInMem(pJpegInfo->ulJpegBufAddr, pJpegInfo->ulJpegBufSize, MMP_TRUE, MMP_TRUE);
        if (mmpstatus == MMP_ERR_NONE) {
		    MMPD_IBC_CheckFrameReady(fctllink.ibcpipeID);
		}		
#else
		MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);

		MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0,  pJpegInfo->ulJpegBufAddr);
		MMPH_HIF_SetParameterL(GRP_IDX_DSC, 4,  pJpegInfo->ulJpegBufSize);

      mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_DECODE_JPEG | DECODE_JPEG_IN_MEM);

		if (mmpstatus == MMP_ERR_NONE) {
        	mmpstatus = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
		}
      if (mmpstatus == MMP_ERR_NONE) {
		    MMPD_IBC_CheckFrameReady(fctllink.ibcpipeID);
		}
		MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
#endif		
    }
    else if(pJpegInfo->bJpegFileName != NULL) {
                
        MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
               
        mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_DECODE_JPEG | DECODE_JPEG_FILE);
		
		if (mmpstatus == MMP_ERR_NONE) {
        	mmpstatus = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
		}
        if (mmpstatus == MMP_ERR_NONE) {
		    MMPD_IBC_CheckFrameReady(fctllink.ibcpipeID);
        }
		MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
	}
	
	// Patch : After decode end, the max buffer usage will be 0x3FF and the JPEG engine will be abnormal.
	MMPD_System_ResetHModule(MMPD_SYS_MDL_JPG, MMP_FALSE);

	MMPD_Scaler_SetEnable(fctllink.scalerpath,  MMP_FALSE);
	MMPD_Icon_SetDLEnable(fctllink.icopipeID,   MMP_FALSE);
	MMPD_IBC_SetStoreEnable(fctllink.ibcpipeID, MMP_FALSE);

	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, 	MMP_FALSE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_ICON,	MMP_FALSE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, 	MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetDecodeBuffers
//  Description :
//------------------------------------------------------------------------------
/** @brief Configure the buffer for jpeg decode operation

The function sets the buffer address for decompress buffer start, decompress buffer end, line buffer,
storage temperarily-used buffer, and size of it. 
To configure the storage temp address, 
we have to send host command request to the firmware and waiting for the DSC command clear.

@param[in] decodebuf is the structure for the memory usage.
@param[in] biscardmode is card mode decode or not.
@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_SetDecodeBuffers(MMP_DSC_DECODE_BUF *pDecodeBuf)
{
	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_TRUE);

	MMPF_DSC_SetDecodeCompBuf(pDecodeBuf->ulDecompressStart, 
							  pDecodeBuf->ulDecompressEnd,
							  pDecodeBuf->ulLineBufStart);
#if defined(MBOOT_FW)
	MMPF_DSC_SetDecodeInputBuf(pDecodeBuf->ulStorageTmpStart, 
	                           pDecodeBuf->ulStorageTmpSize, 
	                           pDecodeBuf->ulDecompressStart, 
	                           pDecodeBuf->ulDecompressEnd - pDecodeBuf->ulDecompressStart);
#else
	MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
	MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0,  pDecodeBuf->ulStorageTmpStart);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 4,  pDecodeBuf->ulStorageTmpSize);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 8,  pDecodeBuf->ulDecompressStart);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 12, (pDecodeBuf->ulDecompressEnd - pDecodeBuf->ulDecompressStart));

	MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_DECODE_JPEG | SET_DATA_INPUT_BUF);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
#endif
	MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetDecodeImgOffset
//  Description :
//------------------------------------------------------------------------------
/** @brief Set the decode bitstream offset for JPEG decodeing

The function set the decode bitstream offset for JPEG decodeing

@param[in] StartOffset the jpeg start offset
@param[in] EndOffset   the jpeg end offset
@return It reports the status of the operation.
*/
#if defined(ALL_FW)	
MMP_ERR MMPD_DSC_SetDecodeImgOffset(MMP_ULONG ulStartOffset, MMP_ULONG ulEndOffset)
{
    MMP_ERR mmpstatus = MMP_ERR_NONE;
     
    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0, ulStartOffset);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 4, ulEndOffset);
          
    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_DECODE_JPEG | SET_DECODE_JPG_OFFSET);
     
	if (mmpstatus == MMP_ERR_NONE) {
    	mmpstatus = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
   	}
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

    return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_GetJpegInfo
//  Description :
//------------------------------------------------------------------------------
/** @brief Get the JPEG information

The function reads an input Jpeg buffer (usJpegBufAddr) from an input Jpeg information structure and
writes back the required fields in the same structure.

@param[in] jpeginfo the jpeg information data structure
@param[out] jpeginfo the jpeg information data structure
@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_GetJpegInfo(MMP_DSC_JPEG_INFO *pJpegInfo)
{
    MMP_ULONG   ulImgLength;
    MMP_ULONG   ulJpegAddr;
    MMP_USHORT  tagID, tagLength;
    MMP_UBYTE   formatType;
    MMP_ERR		mmpstatus;

	if (pJpegInfo->ulJpegBufAddr != 0) {

        ulJpegAddr = pJpegInfo->ulJpegBufAddr;
        ulImgLength = pJpegInfo->ulJpegBufSize;

        do {
            if (!MMPD_DSC_GetJfifTagInDev(&tagID, &tagLength, &ulJpegAddr, ulImgLength)) {
            
                ulImgLength -= (tagLength + 2) /* Marker length */;
                
                if ((tagID > JPG_EXIF_SOF && tagID < JPG_EXIF_DHT) || 
                	(tagID > JPG_EXIF_DHT && tagID < JPG_EXIF_DAC) || 
                	(tagID > JPG_EXIF_DAC && tagID < JPG_EXIF_RST)) {
                    
                    pJpegInfo->usPrimaryWidth  	= 0;
                    pJpegInfo->usPrimaryHeight 	= 0;
                    pJpegInfo->jpgFormat 		= MMP_DSC_JPEG_FMT_NOT_BASELINE;
                    return MMP_ERR_NONE;
                }
                else if (tagID == JPG_EXIF_SOF) {

                    pJpegInfo->usPrimaryHeight	= (MMPH_HIF_MemGetB(ulJpegAddr + 5) << 8) + MMPH_HIF_MemGetB(ulJpegAddr + 6);
                    pJpegInfo->usPrimaryWidth 	= (MMPH_HIF_MemGetB(ulJpegAddr + 7) << 8) + MMPH_HIF_MemGetB(ulJpegAddr + 8);
                    
                    if (MMPH_HIF_MemGetB(ulJpegAddr + 9) == 3) {
                    
                        formatType = MMPH_HIF_MemGetB(ulJpegAddr + 11);

                        MMPD_DSC_SelectJpegFormat(formatType, &(pJpegInfo->jpgFormat));
                    }
                    else {
                        pJpegInfo->jpgFormat = MMP_DSC_JPEG_FMT_NOT_3_COLOR;
                    }

                    pJpegInfo->bValid = MMP_TRUE;
                    return MMP_ERR_NONE;
                }
                else {
                    ulJpegAddr += (tagLength + 2);
                }
            }
            else {
                return MMP_DSC_ERR_JPEGINFO_FAIL;
            }
            
        } while (tagID != JPG_EXIF_SOF && ulImgLength > 0);
    }
    else if (pJpegInfo->bJpegFileName[0] != NULL) {
                
	    #if (DSC_SUPPORT_BASELINE_MP_FILE)
	    MMPD_DSC_SetCardModeExifDec(pJpegInfo->bDecodeThumbnail, pJpegInfo->bDecodeLargeThumb);
	    #else
		MMPD_DSC_SetCardModeExifDec(pJpegInfo->bDecodeThumbnail, MMP_FALSE);
	    #endif

		mmpstatus = MMPF_DSC_GetJpegInfo(pJpegInfo);
        
	    if (mmpstatus != MMP_ERR_NONE) {
            pJpegInfo->usPrimaryWidth 	= 0;
            pJpegInfo->usPrimaryHeight 	= 0;
            pJpegInfo->usThumbWidth 	= 0;
            pJpegInfo->usThumbHeight 	= 0;
            pJpegInfo->jpgFormat 		= MMP_DSC_JPEG_FMT_NOT_BASELINE;
            pJpegInfo->ulThumbOffset	= 0;
            pJpegInfo->ulThumbSize		= 0;
            return mmpstatus;
        }
    }
    
    pJpegInfo->bValid = MMP_TRUE;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_OpenJpegFile
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_OpenJpegFile(MMP_DSC_JPEG_INFO *pJpegInfo)
{
    MMP_ERR	mmpstatus;
    
    if (pJpegInfo->bJpegFileName[0] != NULL) {
    
        MMPD_DSC_SetFileName(pJpegInfo->bJpegFileName, pJpegInfo->usJpegFileNameLength);
        
        MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
        
        mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_DECODE_JPEG | OPEN_JPEG_FILE);
        
        if (mmpstatus == MMP_ERR_NONE) {
            mmpstatus = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
        }
		if (mmpstatus == MMP_ERR_NONE) {
        	pJpegInfo->ulFileSize = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 4);
        }

        MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

        return mmpstatus;
    }
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_CloseJpegFile
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_CloseJpegFile(MMP_DSC_JPEG_INFO *pJpegInfo)
{
    MMP_ERR	mmpstatus = MMP_ERR_NONE;
    
    if (pJpegInfo->bJpegFileName[0] != NULL) {
    
        MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
        mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_DECODE_JPEG | CLOSE_JPEG_FILE);

        if (mmpstatus == MMP_ERR_NONE) {
            mmpstatus = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
        }
        MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
    }
    
    return mmpstatus;
}

#if 0
void _____MPF_Functions_____(){}
#endif

#if (DSC_SUPPORT_BASELINE_MP_FILE)
//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_EnableMultiPicFormat
//  Description : Enable the MPF function.
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_EnableMultiPicFormat(MMP_BOOL bEnable)
{
	MMP_ERR mmpstatus = MMP_ERR_NONE;

	MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
	MMPH_HIF_SetParameterB(GRP_IDX_DSC, 0, bEnable);
	
	mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_CARD_MODE_MPF | MPF_ENABLE);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

    return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_InitMpfNodeStruc
//  Description : Initialize the basic structure of the MPF
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_InitMpfNodeStruc(void)
{
	MMP_ERR mmpstatus = MMP_ERR_NONE;

	MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
	
	mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_CARD_MODE_MPF | MPF_INITIALIZE);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

    return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_FinishMpf
//  Description :
//------------------------------------------------------------------------------
/** @brief move the data of the exif to the start address

@param[in] HeaderStartAddr  The start address you want to write the data of the MPF.
@param[out] pulMpfSize 		The size of MPF
@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_FinishMpf(MMP_ULONG ulHeaderStartAddr, MMP_ULONG *pulMpfSize)
{
    MMP_ERR mmpstatus = MMP_ERR_NONE;

    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0, ulHeaderStartAddr);

	mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_CARD_MODE_MPF | MPF_FINISH);

	if (mmpstatus == MMP_ERR_NONE) {
		*pulMpfSize = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
	}
	MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

	return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_UpdateMpfNode
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_UpdateMpfNode(MMP_USHORT 	usIfd, 	MMP_USHORT 	usTag,
							   MMP_USHORT 	usType, MMP_ULONG 	ulCount,
							   MMP_UBYTE	*pData, MMP_ULONG 	ulDataSize, 
                               MMP_BOOL 	bForUpdate)
{
    MMP_ERR mmpstatus = MMP_ERR_NONE;
	
    if ((!bForUpdate) && (!bMpfInitialize)) {
    	MMPD_DSC_InitMpfNodeStruc();
    	bMpfInitialize = MMP_TRUE;
	}
	
	MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
	MMPH_HIF_SetParameterW(GRP_IDX_DSC, 0,  usIfd);
	MMPH_HIF_SetParameterW(GRP_IDX_DSC, 2,  usTag);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 4,  usType);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 8,  ulCount);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 12, (MMP_ULONG)pData);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 16, ulDataSize);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 20, bForUpdate);
          
    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_CARD_MODE_MPF | MPF_UPDATE_TAG);

	if (mmpstatus == MMP_ERR_NONE) {
    	mmpstatus = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
    }

    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
    return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_UpdateMpfEntry
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_UpdateMpfEntry(MMP_USHORT  	usIfd,			MMP_USHORT  usEntryID,
							    MMP_ULONG 		ulImgAttr, 	 	MMP_ULONG   ulImgSize,  
							    MMP_ULONG 		ulImgOffset, 
							    MMP_USHORT		usImg1EntryNum, MMP_USHORT	usImg2EntryNum,
							    MMP_BOOL    	bForUpdate)
{
    MMP_ERR mmpstatus = MMP_ERR_NONE;
	
    if ((!bForUpdate) && (!bMpfInitialize)) {
    	MMPD_DSC_InitMpfNodeStruc();
    	bMpfInitialize = MMP_TRUE;
	}
	
	MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
	MMPH_HIF_SetParameterW(GRP_IDX_DSC, 0,  usIfd);
	MMPH_HIF_SetParameterW(GRP_IDX_DSC, 2,  usEntryID);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 4,  ulImgAttr);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 8,  ulImgSize);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 12, ulImgOffset);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 16, usImg1EntryNum);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 18, usImg2EntryNum);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 20, bForUpdate);
          
    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_CARD_MODE_MPF | MPF_UPDATE_ENTRY);

	if (mmpstatus == MMP_ERR_NONE) {
    	mmpstatus = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
    }

    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
    return mmpstatus;
}

#endif

#if 0
void _____Exif_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_InitExifNodeStruc
//  Description : Initialize the basic structure of the Exif
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_InitExifNodeStruc(MMP_USHORT usExifNodeId)
{
	MMP_ERR mmpstatus = MMP_ERR_NONE;

	MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
	MMPH_HIF_SetParameterW(GRP_IDX_DSC, 0, usExifNodeId);
	
	mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_CARD_MODE_EXIF | EXIF_INITIALIZE);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

    return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_FinishExif
//  Description :
//------------------------------------------------------------------------------
/** @brief move the data of the exif to the start address

@param[in] HeaderStartAddr  the start address you want to write the data of the exif
@param[in] ulThumbnailSize  the size of thumbnail
@param[out] ExifSize 		the size of exif
@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_FinishExif(MMP_USHORT usExifNodeId, MMP_ULONG ulHeaderStartAddr, MMP_ULONG *ExifSize, MMP_ULONG ulThumbnailSize)
{
    MMP_ERR mmpstatus = MMP_ERR_NONE;

    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0, usExifNodeId);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 4, ulHeaderStartAddr);
	MMPH_HIF_SetParameterL(GRP_IDX_DSC, 8, ulThumbnailSize);

	mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_CARD_MODE_EXIF | EXIF_FINISH);

	if (mmpstatus == MMP_ERR_NONE) {
		*ExifSize = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
	}
	MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

	return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_GetImageEXIFInfo
//  Description : Get tag data by given usExifNodeId, usIfd and usTag.
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_GetImageEXIFInfo(MMP_USHORT usExifNodeId, MMP_USHORT usIfd, MMP_USHORT usTag, MMP_UBYTE *pData, MMP_ULONG *pulSize)
{
    MMP_ERR mmpstatus = MMP_ERR_NONE;
    
    if (pulSize == NULL || pData == NULL) {
        return MMP_DSC_ERR_PARAMETER;
	}

	MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
	MMPH_HIF_SetParameterW(GRP_IDX_DSC, 0, usExifNodeId);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 2, usIfd);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 4, usTag);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 8, (MMP_ULONG)pData);
    
    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_DECODE_JPEG | GET_EXIF_INFO);

	if (mmpstatus == MMP_ERR_NONE) {
    	mmpstatus = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
	}
    
    if (mmpstatus != MMP_ERR_NONE) {
        *pulSize = 0;
    }
    else {
        *pulSize = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 4);
    }

    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

    return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_GetJpgAndExifInfo
//  Description :
//------------------------------------------------------------------------------
/** @brief Get the JPEG and Exif information

The function reads an input Jpeg buffer (usJpegBufAddr) from an input Jpeg information structure and
writes back the required fields in the same structure. It parses the input Jpeg stream data and
writes/updates the fields in the input structure

@param[in] jpeginfo the jpeg information data structure
@param[out] jpeginfo the jpeg information data structure
@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_GetJpgAndExifInfo(MMP_DSC_JPEG_INFO *pJpegInfo)
{
    MMP_ERR mmpstatus = MMP_ERR_NONE;
    
    #if (DSC_SUPPORT_BASELINE_MP_FILE)
    MMPD_DSC_SetCardModeExifDec(pJpegInfo->bDecodeThumbnail, pJpegInfo->bDecodeLargeThumb);
    #else
	MMPD_DSC_SetCardModeExifDec(pJpegInfo->bDecodeThumbnail, MMP_FALSE);
    #endif
    
    mmpstatus = MMPF_DSC_GetJpegInfo(pJpegInfo);

    if (mmpstatus != MMP_ERR_NONE) {
    	return MMP_DSC_ERR_DECODE_FAIL;
    }
    
    pJpegInfo->bValid = MMP_TRUE;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_ResetEXIFWorkingBuffer
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_ResetEXIFWorkingBuffer(MMP_USHORT usExifNodeId, MMP_BOOL bForUpdate)
{
	MMP_ERR mmpstatus = MMP_ERR_NONE;
	
	MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
	MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0, usExifNodeId);
	MMPH_HIF_SetParameterL(GRP_IDX_DSC, 4, bForUpdate);
	
	mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_DECODE_JPEG | RESET_WORKING_BUF);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

    return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetExifWorkingBuffer
//  Description :
//------------------------------------------------------------------------------
/** @brief allocation the buffer for exif working.

//WARNING: Be careful to use this API becuase it will restart the initialization of exif

@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_SetExifWorkingBuffer(MMP_USHORT usExifNodeId, MMP_UBYTE *pBuf, MMP_ULONG ulBufSize, MMP_BOOL bForUpdate)
{
	MMP_ERR mmpstatus = MMP_ERR_NONE;
     
    // Restart the initialization of Exif.
    bExifInitialize[usExifNodeId] = MMP_FALSE; 
    
    #if (DSC_SUPPORT_BASELINE_MP_FILE)
    // Restart the initialization of MPF.
    if (usExifNodeId == EXIF_NODE_ID_PRIMARY) {
    	bMpfInitialize = MMP_FALSE;
	}
	#endif
	
    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0,  usExifNodeId);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 4,  (MMP_ULONG)pBuf);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 8,  ulBufSize);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 12, bForUpdate);
          
    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_DECODE_JPEG | SET_EXIF_WORKINGBUF);
     
	if (mmpstatus == MMP_ERR_NONE) {
    	mmpstatus = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
    }
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

    return mmpstatus;
}
 
//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetCardModeExifEnc
//  Description :
//------------------------------------------------------------------------------
/** @brief Configure the Exif setting for card mode encode
@param[in] bEncodeExif support Exif encode or not
@param[in] bEncodeThumbnail support Exif-Thumbnail encode or not
@param[in] usThumbnailwidth the width of thumbnail
@param[in] usThumbnailheight the height of thumbnail
@return It reports the status of the operation.
*/	
MMP_ERR MMPD_DSC_SetCardModeExifEnc(MMP_USHORT	usExifNodeId,
								    MMP_BOOL 	bEncodeExif, 
									MMP_BOOL 	bEncodeThumbnail, 
									MMP_USHORT 	usThumbnailW, 
									MMP_USHORT 	usThumbnailH)
{
	MMP_ERR	mmpstatus;
	
    bExifInitialize[usExifNodeId] = MMP_TRUE;
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_TRUE);
    
    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 0, usExifNodeId);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 2, bEncodeExif);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 4, bEncodeThumbnail);
    
	if (bEncodeThumbnail) {
		MMPH_HIF_SetParameterW(GRP_IDX_DSC, 6, usThumbnailW);
		MMPH_HIF_SetParameterW(GRP_IDX_DSC, 8, usThumbnailH);
	}
	    
    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_CARD_MODE_EXIF | EXIF_ENC);
    
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_FALSE);
	
	return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetCardModeExifDec
//  Description :
//------------------------------------------------------------------------------
/** @brief Configure the Exif setting for card mode decode
@param[in] bDecodeThumbnail support Exif decode or not
@return It reports the status of the operation.
*/	
MMP_ERR MMPD_DSC_SetCardModeExifDec(MMP_BOOL bDecodeThumb, MMP_BOOL bDecodeLargeThumb)
{
	#if (DSC_SUPPORT_BASELINE_MP_FILE)
    if (bDecodeLargeThumb) {
    	MMPD_DSC_EnableMultiPicFormat(MMP_TRUE);
	}
	#endif

    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0, bDecodeThumb);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 4, bDecodeLargeThumb);
    
    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_CARD_MODE_EXIF | EXIF_DEC);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
    
    return MMP_ERR_NONE;
} 

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_OpenEXIFFile
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_OpenEXIFFile(MMP_UBYTE *pubFileName, MMP_ULONG ulFileNameLen, MMP_USHORT usExifNodeId)
{
     MMP_ERR mmpstatus = MMP_ERR_NONE;
     
     if (pubFileName == NULL) {
     	return MMP_DSC_ERR_PARAMETER;
     }

     MMPD_DSC_SetFileName((MMP_BYTE *)pubFileName, (MMP_USHORT)ulFileNameLen);
     
     MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
     MMPH_HIF_SetParameterW(GRP_IDX_DSC, 0, usExifNodeId);
     MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_DECODE_JPEG | OPEN_EXIF_FILE);
     
     mmpstatus = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
     MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
     
     return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_CloseEXIFFile
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_CloseEXIFFile(MMP_USHORT usExifNodeId)
{
    MMP_ERR mmpstatus = MMP_ERR_NONE;

    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 0, usExifNodeId);
    
    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_DECODE_JPEG | CLOSE_EXIF_FILE);
   
	if (mmpstatus == MMP_ERR_NONE) {
    	mmpstatus = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
    }
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
    
    return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_UpdateExifNode
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_UpdateExifNode(MMP_USHORT 	usExifNodeId, 
								MMP_USHORT 	usIfd, 	MMP_USHORT 	usTag,
								MMP_USHORT 	usType, MMP_ULONG 	ulCount,
								MMP_UBYTE	*pData, MMP_ULONG 	ulDataSize, 
                                MMP_BOOL 	bForUpdate)
{
    MMP_ERR mmpstatus = MMP_ERR_NONE;
    
    if ((!bForUpdate) && (!bExifInitialize[usExifNodeId])) {    	
    	MMPD_DSC_InitExifNodeStruc(usExifNodeId);
    	bExifInitialize[usExifNodeId] = MMP_TRUE;
	}
	
	MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
	MMPH_HIF_SetParameterW(GRP_IDX_DSC, 0,  usExifNodeId);
	MMPH_HIF_SetParameterW(GRP_IDX_DSC, 2,  usIfd);
	MMPH_HIF_SetParameterW(GRP_IDX_DSC, 4,  usTag);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 6,  usType);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 8,  ulCount);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 12, (MMP_ULONG)pData);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 16, ulDataSize);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 20, bForUpdate);
          
    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_DECODE_JPEG | UPDATE_EXIF_NODE);

	if (mmpstatus == MMP_ERR_NONE) {
    	mmpstatus = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
    }

    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);
    return mmpstatus;
}

#if 0
void _____Storage_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetFileName
//  Description :
//------------------------------------------------------------------------------
/** @brief Configure the storage path for capture

The function specifies the current file by name by sending host command request to the firmware and
waiting for the STORAGE command clear.

@param[in] ubFilename the file path
@param[in] usLength the length of file path
@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_SetFileName(MMP_BYTE ubFilename[], MMP_USHORT usLength)
{
	MMP_UBYTE null = 0;

	if (!m_ulDSCFileNameAddr) {
		PRINTF("MMPD_DSC_SetFileNameAddr need to be initialized\r\n");
		return MMP_DSC_ERR_PARAMETER;
	}
	
	if ((ubFilename == NULL) || (usLength == 0)) {
		PRINTF("MMPD_DSC_SetFileName need to be given parameters\r\n");	
		return MMP_DSC_ERR_FILE_ERROR;
	}
	
    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_MemCopyHostToDev(m_ulDSCFileNameAddr, (MMP_UBYTE *)ubFilename, usLength);
    MMPH_HIF_MemCopyHostToDev(m_ulDSCFileNameAddr + usLength, (MMP_UBYTE*)&null, 1);
    MMPH_HIF_MemCopyHostToDev(m_ulDSCFileNameAddr + usLength  + 1, (MMP_UBYTE*)&null, 1);
    
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0, m_ulDSCFileNameAddr);

    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_SET_JPEG_MEDIA | JPEG_FILE_NAME);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_SetFileNameAddr
//  Description :
//------------------------------------------------------------------------------
/** @brief Configure the storage path for capture

The function specifies the current file by name by sending host command request to the firmware and
waiting for the STORAGE command clear.

@param[in] ulFilenameAddr the file nams passing address
@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_SetFileNameAddr(MMP_ULONG ulFilenameAddr)
{
	m_ulDSCFileNameAddr = ulFilenameAddr;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_GetFileNameAddr
//  Description :
//------------------------------------------------------------------------------
/** @brief Get the storage path for capture

The function get the current file name buffer address

@param[out] ulFilenameAddr the file name passing address
@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_GetFileNameAddr(MMP_ULONG *pulFilenameAddr)
{
	*pulFilenameAddr = m_ulDSCFileNameAddr;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_JpegDram2Card
//  Description :
//------------------------------------------------------------------------------
/** @brief The function saved Jpeg data from DRAM (JPG_EXT_MEM_ST) to card

The function saved Jpeg data from DRAM (JPG_EXT_MEM_ST) to card. It used for card mode extermal memory capture.

@param[in] ulWritesize	the capture Jpeg file size (contain exif head/ Thumbnail Jpeg/ Primary Jpeg).
@param[in] bFirstwrite 	Is first data write or not?
@param[in] bLastwrite 	Is last data write or not?

@return It reports the status of the operation.
*/
MMP_ERR MMPD_DSC_JpegDram2Card(MMP_ULONG ulStartAddr, MMP_ULONG ulWritesize, MMP_BOOL bFirstWrite, MMP_BOOL bLastWrite)
{	
	MMP_ERR	mmpstatus;

    if (ulStartAddr == 0) {
        RTNA_DBG_Str(0, "ulStartAddr = 0 \r\n");
    	return MMP_DSC_ERR_SAVE_CARD_FAIL;
    }
    
    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0,	ulStartAddr);
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 4, 	ulWritesize);  
	MMPH_HIF_SetParameterL(GRP_IDX_DSC, 8, 	bFirstWrite); 
	MMPH_HIF_SetParameterL(GRP_IDX_DSC, 12, bLastWrite); 
	
    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_CMD_JPEG_SAVE_CARD);
    
    if (mmpstatus == MMP_ERR_NONE) {
	    mmpstatus = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
	}
	MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

	return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_DSC_GetRawData
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_DSC_GetRawData(MMP_UBYTE   ubSnrSel,
                            MMP_ULONG 	ulStoreAddr,
							MMP_USHORT 	usRawBitMode,
							MMP_ULONG 	*pulWidth,
							MMP_ULONG 	*pulHeight)
{
    MMP_ERR mmpstatus = MMP_ERR_NONE;
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_VIF, MMP_TRUE);
    
    MMPH_HIF_SetParameterL(GRP_IDX_DSC, 0, (MMP_ULONG)ubSnrSel);
	MMPH_HIF_SetParameterL(GRP_IDX_DSC, 4, (MMP_ULONG)ulStoreAddr);
	MMPH_HIF_SetParameterL(GRP_IDX_DSC, 8, (MMP_ULONG)usRawBitMode);
	
	MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_RAW_OPERATION | GET_RAW_DATA);
	
    if (mmpstatus == MMP_ERR_NONE) {
		*pulWidth  = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 0);
	    *pulHeight = MMPH_HIF_GetParameterL(GRP_IDX_DSC, 4);
	}
	
    MMPD_System_EnableClock(MMPD_SYS_CLK_VIF, MMP_FALSE);
    
    return mmpstatus;
}

#if 0
void _____Streaming_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPD_MJPEG_NewOneJPEG
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_MJPEG_NewOneJPEG(MMP_USHORT usEncID)
{
	MMP_USHORT ustype = MMP_DSC_JPEGCMD_NEWONE;

    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 0, usEncID);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 2, ustype);
    
    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_MJPEG_OPERATION | STREAM_JPEG_INFO);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_MJPEG_SetCompBuf
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_MJPEG_SetCompBuf(MMP_USHORT usEncID, MMP_ULONG ulAddr, MMP_ULONG ulSize)
{
	MMP_USHORT ustype = MMP_DSC_JPEGCMD_SETCOMPBUF;

    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 0, usEncID);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 2, ustype);
	MMPH_HIF_SetParameterL(GRP_IDX_DSC, 4, ulAddr);
	MMPH_HIF_SetParameterL(GRP_IDX_DSC, 8, ulSize);
    
    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_MJPEG_OPERATION | STREAM_JPEG_INFO);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_MJPEG_StreamEnable
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_MJPEG_StreamEnable(MMP_USHORT usEncID, MMP_BOOL bEnable)
{
	MMP_USHORT ustype = MMP_DSC_JPEGCMD_STREAMEN;

    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 0, usEncID);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 2, ustype);
	MMPH_HIF_SetParameterW(GRP_IDX_DSC, 4, bEnable);
    
    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_MJPEG_OPERATION | STREAM_JPEG_INFO);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_MJPEG_CheckEncode
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_MJPEG_CheckEncode(MMP_USHORT usEncID)
{
	MMP_USHORT ustype = MMP_DSC_JPEGCMD_CHECKENC;

    MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_TRUE);

    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 0, usEncID);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 2, ustype);

    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_MJPEG_OPERATION | STREAM_JPEG_INFO);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

    MMPD_System_EnableClock(MMPD_SYS_CLK_JPG, MMP_FALSE);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_MJPEG_SetFPS
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_MJPEG_SetFPS(MMP_USHORT usEncID, MMP_ULONG ulIncrReso, MMP_ULONG ulIncrement)
{
	MMP_USHORT ustype = MMP_DSC_JPEGCMD_SET_FPS;

    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 0, usEncID);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 2, ustype);
	MMPH_HIF_SetParameterL(GRP_IDX_DSC, 4, ulIncrReso);
	MMPH_HIF_SetParameterL(GRP_IDX_DSC, 8, ulIncrement);
    
    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_MJPEG_OPERATION | STREAM_JPEG_INFO);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_MJPEG_SetLinkPipe
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_MJPEG_SetLinkPipe(MMP_USHORT usEncID, MMP_UBYTE ubPipe)
{
	MMP_USHORT ustype = MMP_DSC_JPEGCMD_SET_LINKPIPE;

    MMPH_HIF_WaitSem(GRP_IDX_DSC, 0);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 0, usEncID);
    MMPH_HIF_SetParameterW(GRP_IDX_DSC, 2, ustype);
	MMPH_HIF_SetParameterW(GRP_IDX_DSC, 4, ubPipe);
    
    MMPH_HIF_SendCmd(GRP_IDX_DSC, HIF_DSC_MJPEG_OPERATION | STREAM_JPEG_INFO);
    MMPH_HIF_ReleaseSem(GRP_IDX_DSC);

	return MMP_ERR_NONE;
}
#endif //#if defined(ALL_FW)	
/// @}
/// @end_ait_only

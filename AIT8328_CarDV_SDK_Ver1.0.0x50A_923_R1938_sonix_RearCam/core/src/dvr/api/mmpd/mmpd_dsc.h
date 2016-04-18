/// @ait_only
//==============================================================================
//
//  File        : mmpd_dsc.h
//  Description : INCLUDE File for the Host DSC Driver.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPD_DSC_H_
#define _MMPD_DSC_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "ait_config.h"
#include "mmpd_fctl.h"
#include "mmp_dsc_inc.h"

/** @addtogroup MMPD_DSC
 *  @{
 */

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

typedef enum _MMPD_DSC_BUF_TYPE
{
	MMPD_DSC_ENC_EXTERNAL_COMPRESS_BUFFER = 0,  //DRAM
	MMPD_DSC_ENC_VR_CAPTURE_COMP_BUFFER,
	MMPD_DSC_ENC_LARGE_THUMB_COMP_BUFFER,
	MMPD_DSC_ENC_SINGLE_LINE_BUFFER,
	MMPD_DSC_ENC_DOUBLE_LINE_BUFFER,
	MMPD_DSC_DEC_DECOMPRESS_BUFFER,
	MMPD_DSC_DEC_SINGLE_LINE_BUFFER,
	MMPD_DSC_DEC_DOUBLE_LINE_BUFFER,
	MMPD_DSC_EXIF_WORK_BUFFER
} MMPD_DSC_BUF_TYPE;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

// Memory Control Function
MMP_ERR MMPD_DSC_GetBufferSize(	MMPD_DSC_BUF_TYPE 	buftype, 
							    MMP_DSC_JPEG_FMT 	jpegformat,
								MMP_USHORT 			usWidth, 
								MMP_USHORT 			usHeight, 
								MMP_ULONG			*ulSize);
MMP_ERR MMPD_DSC_SetRingBuffer(MMP_ULONG ulStartAddr, MMP_ULONG ulSize, 
							   MMP_ULONG ulRsvdSize, MMP_ULONG ulInterval);

// Encode Function
MMP_ERR MMPD_DSC_EnableScaleUpCapture(MMP_BOOL bScaleUpCaptureEn);
MMP_ERR MMPD_DSC_SetPreviewPath(MMP_USHORT	usScaPipe,
                                MMP_USHORT	usIcoPipe,
                                MMP_USHORT	usIbcPipe);
MMP_ERR MMPD_DSC_SetCapturePath(MMP_USHORT	usScaPipe,
                                MMP_USHORT	usIcoPipe,
                                MMP_USHORT	usIbcPipe);
MMP_ERR MMPD_DSC_SetCaptureBuffers(MMP_DSC_CAPTURE_BUF *pCaptureBuf);
MMP_ERR MMPD_DSC_SetJpegResol(MMP_USHORT usJpegWidth, MMP_USHORT usJpegHeight, MMP_UBYTE ubRcIdx);
MMP_ERR MMPD_DSC_SetJpegQTable(MMP_UBYTE *ubQtable, MMP_UBYTE *ubUQtable, MMP_UBYTE *ubVQtable);
MMP_ERR MMPD_DSC_SetJpegQualityControl( MMP_UBYTE   ubRcIdx,
                                        MMP_BOOL    bTargetCtrl, 
                                        MMP_BOOL    bLimitCtrl, 
                                        MMP_ULONG   ulTargetSize,
                                        MMP_ULONG   ulLimitSize,
                                        MMP_USHORT  usMaxCount);
MMP_ERR MMPD_DSC_SetRotateCapture(MMP_DSC_CAPTURE_ROTATE_TYPE RotateType, MMP_ULONG ulFrameAddr);
MMP_ERR MMPD_DSC_SetStableShotParam(MMP_ULONG ulMainAddr, MMP_ULONG ulSubAddr, MMP_USHORT usShotNum);
MMP_ERR MMPD_DSC_ChangeExtCompAddr(MMP_ULONG ulExtCompStartAddr, MMP_ULONG ulExtCompEndAddr);

MMP_ERR MMPD_DSC_TakeJpegAfterSrcReady(MMP_UBYTE ubSnrSel);
MMP_ERR MMPD_DSC_TakeJpegPicture(MMP_UBYTE  ubSnrSel,
                                 MMP_BOOL 	bPreviewOn,
								 MMP_USHORT	usReviewTime,
								 MMP_USHORT	usWindowsTime,
							 	 MMP_USHORT usShotMode,
							 	 MMP_BOOL	bRestorePreview);
MMP_ERR MMPD_DSC_TakeContinuousJpeg(MMP_UBYTE   ubSnrSel,
                                    MMP_BOOL 	bPreviewOn,
									MMP_USHORT	usReviewTime,
									MMP_USHORT	usWindowsTime,
									MMP_USHORT  usShotMode,
									MMP_BOOL 	bContiShotEnable);
MMP_ERR MMPD_DSC_TakeNextJpegPicture(MMP_UBYTE ubSnrSel, MMP_USHORT usReviewTime);
MMP_ERR MMPD_DSC_RestorePreview(MMP_UBYTE ubVifId, MMP_BOOL bPreviewOn,MMP_USHORT usWindowsTime);

MMP_ERR MMPD_DSC_VidRefBuf2Jpeg(void);
MMP_ERR MMPD_DSC_RawBuf2Jpeg(MMP_GRAPHICS_BUF_ATTR  *bufAttr,
                             MMP_GRAPHICS_RECT      *rect, 
                             MMP_USHORT             usUpscale, 
                             MMPD_FCTL_ATTR         *pFctlAttr);

MMP_ERR MMPD_DSC_GetJpegPicture(MMP_USHORT *usJpegBufAddr, MMP_ULONG ulHostBufLimit);
MMP_ERR MMPD_DSC_GetJpegSize(MMP_ULONG *ulJpegFileSize);
MMP_ERR MMPD_DSC_GetJpegSizeAfterEncodeDone(MMP_ULONG *ulJpegFileSize);
MMP_ERR MMPD_DSC_EncodeThumb(MMP_UBYTE          ubSnrSel,
                             MMP_SCAL_PIPEID 	srcPath,
                             MMP_SCAL_PIPEID 	dstPath,
                             MMPD_FCTL_ATTR		*pFctlAttr);
MMP_ERR MMPD_DSC_StoreThumbSrcFrame(MMP_GRAPHICS_BUF_ATTR 	*pBufAttr,
									MMP_SCAL_FIT_RANGE		*pFitrange,
                                 	MMP_SCAL_GRAB_CTRL 		*pGrabctl,
                                 	MMP_SCAL_PIPEID 		srcPath,
                                 	MMP_SCAL_PIPEID 		dstPath);
MMP_ERR MMPD_DSC_SetContiShotParam(MMP_ULONG ulExifBufAddr, MMP_ULONG ulMaxShotNum);
MMP_ERR MMPD_DSC_CheckContiShotStreamBusy(MMP_ULONG *pulInterval);

// MJPEG Function
MMP_ERR MMPD_DSC_InitGra2MJPEGAttr(	MMPD_FCTL_LINK 			fctllink,
									MMP_GRAPHICS_BUF_ATTR  	*pBufAttr,
									MMP_GRAPHICS_RECT      	*pRect,
			                   		MMP_SCAL_FIT_RANGE    	*pFitrange,
			                   		MMP_SCAL_GRAB_CTRL  	*pGrabctl);

MMP_ERR MMPD_DSC_SetMJPEGPipe(MMP_UBYTE ubPipe);
MMP_ERR MMPD_DSC_StartMJPEGStream(MMP_UBYTE ubSnrSel);
MMP_ERR MMPD_DSC_StopMJPEGStream(MMP_USHORT usEncID);
MMP_ERR MMPD_DSC_SetMJPEGFpsInfo(MMP_ULONG ulInc, MMP_ULONG ulInRes, MMP_ULONG ulOutRes);
MMP_ERR MMPD_DSC_SetVideo2MJPEGEnable(MMP_BOOL bEnable);

MMP_ERR MMPD_DSC_GetRearCamBufForDualStreaming(MMP_ULONG *pulCompAddr, MMP_ULONG *pulCompSize, MMP_ULONG *pulLineBuf);
MMP_ERR MMPD_DSC_SetDualStreamingEnable(MMP_BOOL bFrontCamEn, MMP_BOOL bRearCamEn);
MMP_ERR MMPD_DSC_SetDecMjpegToPreviewSrcAttr(MMP_USHORT usSrcW, MMP_USHORT usSrcH);
MMP_ERR MMPD_DSC_SetDecMjpegToPreviewPipe(MMP_UBYTE ubPipe);
MMP_ERR MMPD_DSC_SetDecMjpegToEncodePipe(MMP_UBYTE ubPipe);
MMP_ERR MMPD_DSC_SetDecMjpegToPreviewJpegBuf(MMP_ULONG ulAddr, MMP_ULONG* pulSize);

// Decode Function
MMP_ERR MMPD_DSC_DecodeJpegPic(	MMP_DISPLAY_COLORMODE 	dispColor,
                                MMP_DSC_JPEG_INFO 		*pJpegInfo, 
                                MMP_GRAPHICS_BUF_ATTR 	*pBufAttr,
                                MMP_SCAL_FIT_RANGE      *pFitrange,
                                MMP_SCAL_GRAB_CTRL 		*pGrabctl,
                                MMP_SCAL_PIPEID 		pipeID);
MMP_ERR MMPD_DSC_SetDecodeBuffers(MMP_DSC_DECODE_BUF *pDecodeBuf);
MMP_ERR MMPD_DSC_SetDecodeImgOffset(MMP_ULONG ulStartOffset,MMP_ULONG ulEndOffset);
MMP_ERR MMPD_DSC_GetJpegInfo(MMP_DSC_JPEG_INFO *jpeginfo);
MMP_ERR MMPD_DSC_OpenJpegFile(MMP_DSC_JPEG_INFO *jpeginfo);
MMP_ERR MMPD_DSC_CloseJpegFile(MMP_DSC_JPEG_INFO *jpeginfo);

// MPF Function
MMP_ERR MMPD_DSC_EnableMultiPicFormat(MMP_BOOL bEnable);
MMP_ERR MMPD_DSC_InitMpfNodeStruc(void);
MMP_ERR MMPD_DSC_FinishMpf(MMP_ULONG ulHeaderStartAddr, MMP_ULONG *pulMpfSize);
MMP_ERR MMPD_DSC_UpdateMpfNode(MMP_USHORT 	usIfd, 	MMP_USHORT 	usTag,
							   MMP_USHORT 	usType, MMP_ULONG 	ulCount,
							   MMP_UBYTE	*pData, MMP_ULONG 	ulDataSize, 
                               MMP_BOOL 	bForUpdate);
MMP_ERR MMPD_DSC_UpdateMpfEntry(MMP_USHORT  	usIfd,			MMP_USHORT  usEntryID,
							    MMP_ULONG 		ulImgAttr, 	 	MMP_ULONG   ulImgSize,  
							    MMP_ULONG 		ulImgOffset, 
							    MMP_USHORT		usImg1EntryNum, MMP_USHORT	usImg2EntryNum,
							    MMP_BOOL    	bForUpdate);

// EXIF Function
MMP_ERR MMPD_DSC_InitExifNodeStruc(MMP_USHORT usExifNodeId);
MMP_ERR MMPD_DSC_FinishExif(MMP_USHORT usExifNodeId, MMP_ULONG ulHeaderStartAddr, MMP_ULONG *ExifSize, MMP_ULONG ulThumbnailSize);
MMP_ERR MMPD_DSC_GetImageEXIFInfo(MMP_USHORT usExifNodeId, MMP_USHORT usIfd, MMP_USHORT usTag, MMP_UBYTE *pData, MMP_ULONG *pulSize);
MMP_ERR MMPD_DSC_GetJpgAndExifInfo(MMP_DSC_JPEG_INFO *pJpegInfo);
MMP_ERR MMPD_DSC_ResetEXIFWorkingBuffer(MMP_USHORT usExifNodeId, MMP_BOOL bForUpdate);
MMP_ERR MMPD_DSC_SetExifWorkingBuffer(MMP_USHORT usExifNodeId, MMP_UBYTE *pBuf, MMP_ULONG ulBufSize, MMP_BOOL bForUpdate);
MMP_ERR MMPD_DSC_SetCardModeExifEnc(MMP_USHORT	usExifNodeId,
									MMP_BOOL 	bEncodeExif, 
									MMP_BOOL 	bEncodeThumbnail, 
									MMP_USHORT 	usThumbnailwidth, 
									MMP_USHORT 	usThumbnailheight);
MMP_ERR MMPD_DSC_SetCardModeExifDec(MMP_BOOL bDecodeThumb, MMP_BOOL bDecodeLargeThumb);
MMP_ERR MMPD_DSC_OpenEXIFFile(MMP_UBYTE *pubFileName, MMP_ULONG ulFileNameLen, MMP_USHORT usExifNodeId);
MMP_ERR MMPD_DSC_CloseEXIFFile(MMP_USHORT usExifNodeId);
MMP_ERR MMPD_DSC_UpdateExifNode(MMP_USHORT 	usExifNodeId, 
								MMP_USHORT 	usIfd, 	MMP_USHORT 	usTag,
								MMP_USHORT 	usType, MMP_ULONG 	ulCount,
								MMP_UBYTE	*pData, MMP_ULONG 	ulDataSize, 
                                MMP_BOOL 	bForUpdate);

// Storage Function
MMP_ERR MMPD_DSC_SetFileName(MMP_BYTE ubFilename[], MMP_USHORT usLength);
MMP_ERR MMPD_DSC_SetFileNameAddr(MMP_ULONG ulFilenameAddr);
MMP_ERR MMPD_DSC_GetFileNameAddr(MMP_ULONG *pulFilenameAddr);
MMP_ERR MMPD_DSC_JpegDram2Card(MMP_ULONG ulStartAddr, MMP_ULONG ulWritesize, MMP_BOOL bFirstwrite, MMP_BOOL bLastwrite);
MMP_ERR MMPD_DSC_GetRawData(MMP_UBYTE   ubSnrSel,
                            MMP_ULONG   ulStoreAddr,
                            MMP_USHORT  usRawBitMode,
                            MMP_ULONG   *pulWidth,
                            MMP_ULONG   *pulHeight);
MMP_ERR MMPD_DSC_SetJpegQTableEx(MMP_UBYTE ubIdx, MMP_UBYTE *ubQtable, MMP_UBYTE *ubUQtable, MMP_UBYTE *ubVQtable, MMP_DSC_JPEG_QTABLE QTSet);
// Streaming Function
MMP_ERR MMPD_MJPEG_NewOneJPEG(MMP_USHORT usEncID);
MMP_ERR MMPD_MJPEG_SetCompBuf(MMP_USHORT usEncID, MMP_ULONG uladdr, MMP_ULONG ulsize);
MMP_ERR MMPD_MJPEG_StreamEnable(MMP_USHORT usEncID, MMP_BOOL bEnable);
MMP_ERR MMPD_MJPEG_CheckEncode(MMP_USHORT usEncID);
MMP_ERR MMPD_MJPEG_SetFPS(MMP_USHORT usEncID, MMP_ULONG ulIncrReso, MMP_ULONG ulIncrement);
MMP_ERR MMPD_MJPEG_SetLinkPipe(MMP_USHORT usEncID, MMP_UBYTE ubPipe);

__inline MMP_ERR MMPD_Streaming_Init(MMP_USHORT usEncID) {return MMPD_MJPEG_NewOneJPEG(usEncID);}
__inline MMP_ERR MMPD_Streaming_SetCompBuf(MMP_USHORT usEncID, MMP_ULONG uladdr, MMP_ULONG ulsize)
    										{ return MMPD_MJPEG_SetCompBuf(usEncID, uladdr, ulsize); }
__inline MMP_ERR MMPD_Streaming_Enable(MMP_USHORT usEncID, MMP_BOOL bEnable) {return MMPD_MJPEG_StreamEnable(usEncID, bEnable);}

/// @}

#endif // _MMPD_DSC_H_

/// @end_ait_only

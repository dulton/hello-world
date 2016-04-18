//==============================================================================
//
//  File        : mmps_dsc.h
//  Description : INCLUDE File for the Host Application for DSC.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

/**
 *  @file mmps_dsc.h
 *  @brief The header File for the Host DSC control functions
 *  @author Penguin Torng
 *  @version 1.0
 */

#ifndef _MMPS_DSC_H_
#define _MMPS_DSC_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmps_display.h" 
#include "mmpd_dsc.h"
#include "mmpd_fctl.h"
#include "mmp_media_def.h"
#include "mmp_ptz_inc.h"
#include "mmp_rawproc_inc.h"
/** @addtogroup MMPS_DSC
@{
*/

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#define DSC_MAX_RAWSTORE_BUF_NUM	(2)

#define EXIF_MANUAL_UPDATE        	(1)
#define EXIF_ASCII_LENGTH       	(64)
#define EXIF_CONFIG_ARRAY_SIZE  	(16*1024)

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

typedef enum _MMPS_DSC_CAPTUREMODE
{
    MMPS_DSC_SINGLE_SHOTMODE = 0,   	///< Single shot mode
    MMPS_DSC_MULTI_SHOTMODE,        	///< Multi-shot mode
    MMPS_DSC_CONTINUOUS_SHOTMODE    	///< Continuous-shot mode 
} MMPS_DSC_CAPTUREMODE;

typedef enum _MMPS_DSC_JPEGSAVEPATH
{
    MMPS_DSC_CAPTURE_SAVE_CARD = 0,  	///< Capture to card
    MMPS_DSC_CAPTURE_SAVE_CONTAINER	 	///< Capture to container
} MMPS_DSC_JPEGSAVEPATH;

typedef enum _MMPS_DSC_CUSTOM_QTABLE_TYPE
{
    MMPS_DSC_CUSTOM_QTABLE_OFF,
    MMPS_DSC_CUSTOM_QTABLE_ALL,
    MMPS_DSC_CUSTOM_QTABLE_MAIN,
    MMPS_DSC_CUSTOM_QTABLE_THUMB
} MMPS_DSC_CUSTOM_QTABLE_TYPE;

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef struct _MMPS_DSC_CAPTURE_INFO 
{
	MMP_USHORT	       		*usJpegBufAddr;									///< Host Memory mode jpeg picture buffer address
	MMP_ULONG 	        	ulHostBufLimit;									///< Host Memory mode jpeg buffer size limit
	MMP_ULONG	       		ulJpegFileSize;									///< Captured JPEG size
	MMP_ULONG	        	ulJpegFileAddr;									///< JPEG compress buffer address

	MMP_BYTE 	        	*ubFilename;									///< Card mode file name
	MMP_USHORT	        	usFilenamelen;									///< Card mode file name length
	MMP_BYTE 	        	*ubRawFilename;									///< Card mode raw file name
	MMP_USHORT	       		usRawFilenamelen;								///< Card mode raw file name length
	
	MMP_BOOL	        	bExif;											///< Enable EXIF function or not
	MMP_BOOL	       		bThumbnail;										///< Enable thumbnail function or not

	MMP_BOOL	        	bFirstShot;										///< First shot or second shot
	
	MMP_USHORT				usReviewTime;									///< The time of review time
	MMP_USHORT				usWindowsTime;              					///< The time of window disable time
	
	/**	@brief	Its to add ISP related Info for AHC. 
				AHC team will assign the address and the size of the buffer by themself.
				If we dont wanna use the extra buffer, we must set the addr of the buffer is 0.*/				
	MMP_ULONG				ulExtraBufAddr;									///< The extra buf is written after primary jpeg
	MMP_ULONG				ulExtraBufSize;									///< The size of the extra buffer
	MMP_BOOL 				bContiShotEnable;								///< The enable of continuous shot
} MMPS_DSC_CAPTURE_INFO;

typedef struct _MMPS_DSC_MULTISHOT_INFO 
{
	/**	@brief	The compressed buffer between ulPrimaryJpegAddr and ulUsedEndAddr 
				including primary Jpeg, Thumbnail, and Exif header. */
	MMP_USHORT  			usShotIndex;				                	///< Multishot capture index used for MMPS_DSC_CaptureTakeJpeg()
	MMP_USHORT  			usStoreIndex;                                   ///< Multishot store index used for MMPS_DSC_JpegDram2Card()

	MMP_ULONG   			ulPrimaryJpegAddr[DSC_MULTISHOT_MAX_NUM];		///< Primary Jpeg start address 
	MMP_ULONG				ulPrimaryJpegSize[DSC_MULTISHOT_MAX_NUM];		///< Primary Jpeg file size

	MMP_ULONG   			ulExifThumbnailAddr[DSC_MULTISHOT_MAX_NUM];		///< Exif thumbnail start address
	MMP_ULONG				ulExifThumbnailSize[DSC_MULTISHOT_MAX_NUM];		///< Exif thumbnail size
	MMP_ULONG				ulExifHeaderStartAddr[DSC_MULTISHOT_MAX_NUM];	///< Exif header start address
	MMP_ULONG				ulExifHeaderSize[DSC_MULTISHOT_MAX_NUM];		///< Exif header size
	#if (DSC_SUPPORT_BASELINE_MP_FILE)
	MMP_ULONG				ulMpfDataStartAddr[DSC_MULTISHOT_MAX_NUM];		///< MPF data start address
	MMP_ULONG				ulMpfDataSize[DSC_MULTISHOT_MAX_NUM];			///< MPF data size
	MMP_ULONG				ulLargeThumbExifAddr[DSC_MULTISHOT_MAX_NUM];	///< Large thumbnail Exif start address
	MMP_ULONG				ulLargeThumbExifSize[DSC_MULTISHOT_MAX_NUM];	///< Large thumbnail Exif data size
	MMP_ULONG				ulLargeThumbJpegAddr[DSC_MULTISHOT_MAX_NUM];	///< Large thumbnail Jpeg start address
	MMP_ULONG				ulLargeThumbJpegSize[DSC_MULTISHOT_MAX_NUM];	///< Large thumbnail Jpeg size
	#endif
	
	MMP_ULONG				ulUsedEndAddr[DSC_MULTISHOT_MAX_NUM];			///< Total compressed buffer end addr Jpeg range size
	MMP_ULONG				ulLargeThumbUsedEndAddr[DSC_MULTISHOT_MAX_NUM];

} MMPS_DSC_MULTISHOT_INFO;

typedef struct _MMPS_DSC_PLAYBACK_INFO
{
	MMP_USHORT          	usJpegWidth;	    							///< Jpeg picture original image width
	MMP_USHORT	         	usJpegHeight;	    							///< Jpeg picture original image height
	MMP_USHORT            	usDecodeWidth;									///< Jpeg picture decoded width (in IBC buffer)
	MMP_USHORT	           	usDecodeHeight;	    							///< Jpeg picture decoded height (in IBC buffer)
	MMP_USHORT            	usDispWinOfstX;									///< Jpeg picture display window offset X
	MMP_USHORT	          	usDispWinOfstY;                                 ///< Jpeg picture display window offset Y
	MMP_USHORT            	usDisplayWidth;									///< Jpeg picture display width
	MMP_USHORT	          	usDisplayHeight;								///< Jpeg picture display height
	MMP_DISPLAY_COLORMODE	DecodeColorFmt;     							///< Decode color format
	MMP_DSC_JPEG_INFO 		jpeginfo;	        							///< Jpeg file information
	MMP_GRAPHICS_BUF_ATTR	bufAttr;	   									///< Decoded raw data buffer attribute
	MMP_SCAL_FIT_MODE  		sFitMode;										///< Scaling fit mode
	MMP_DISPLAY_ROTATE_TYPE rotateType;		    							///< Rotation attribute when display
	MMP_BOOL  				bMirror;           								///< LCD mirror or not
} MMPS_DSC_PLAYBACK_INFO;

typedef struct _MMPS_DSC_ZOOM_INFO
{
    MMP_SCAL_PIPEID  		scalerpath;

    MMP_USHORT 				usMaxZoomRatio;
    MMP_USHORT 				usMaxZoomSteps;
    MMP_USHORT 				usCurZoomStep;

    MMP_SHORT 				sMaxPanSteps;
    MMP_SHORT 				sMinPanSteps;
    MMP_SHORT 				sCurPanStep;

    MMP_SHORT 				sMaxTiltSteps;
    MMP_SHORT 				sMinTiltSteps;
    MMP_SHORT 				sCurTiltStep;
} MMPS_DSC_ZOOM_INFO;

typedef struct _MMPS_DSC_DISP_PARAMS 
{
	MMP_ULONG				ulRawPreviewBufCnt;                				///< The number of rawstore buffer
	
    MMP_USHORT  			usDscPreviewBufW[DSC_PREV_MODE_MAX_NUM];		///< Specify DSC preview buffer width
    MMP_USHORT  			usDscPreviewBufH[DSC_PREV_MODE_MAX_NUM];		///< Specify DSC preview buffer height
    MMP_SCAL_FIT_MODE		sFitMode[DSC_PREV_MODE_MAX_NUM];				///< Specify scaler fit mode
    
    //++ For Display Window/Device Attribute
    MMP_DISPLAY_COLORMODE	DispColorFmt[DSC_PREV_MODE_MAX_NUM];			///< Specify preview color format
    MMP_DISPLAY_DEV_TYPE	DispDevice[DSC_PREV_MODE_MAX_NUM];	      		///< Specify output display control type
    MMP_DISPLAY_WIN_ID      DispWinId[DSC_PREV_MODE_MAX_NUM];             	///< Specify output display window id
    MMP_USHORT  			usDscDispBufCnt[DSC_PREV_MODE_MAX_NUM];			///< Specify output buffer counts for preview
    MMP_USHORT  			usDscDisplayW[DSC_PREV_MODE_MAX_NUM];			///< Specify display window width
    MMP_USHORT  			usDscDisplayH[DSC_PREV_MODE_MAX_NUM];			///< Specify display window height
    MMP_USHORT  			usDscDispStartX[DSC_PREV_MODE_MAX_NUM];			///< Specify display window offset X relative to panel
    MMP_USHORT  			usDscDispStartY[DSC_PREV_MODE_MAX_NUM];			///< Specify display window offset Y relative to panel
    MMP_BOOL    			bDscDispMirror[DSC_PREV_MODE_MAX_NUM];			///< Specify display mirror attribute
    MMP_DISPLAY_ROTATE_TYPE DscDispDir[DSC_PREV_MODE_MAX_NUM];       		///< Specify display rotate attribute
    MMP_BOOL    			bDscDispScaleEn[DSC_PREV_MODE_MAX_NUM];			///< Specify display window scale enable attribute
	//-- For Display Window/Device Attribute
	
    //++ For DMA rotate
    MMP_BOOL    			bUseRotateDMA[DSC_PREV_MODE_MAX_NUM];			///< Specify DMA rotate enable  
    MMP_USHORT  			usRotateBufCnt[DSC_PREV_MODE_MAX_NUM];			///< Specify DMA rotate buffer counts
    MMP_DISPLAY_ROTATE_TYPE ubDMARotateDir[DSC_PREV_MODE_MAX_NUM];       	///< Specify DMA rotate direction
    //-- For DMA rotate
} MMPS_DSC_DISP_PARAMS;

typedef struct _MMPS_DSC_ENC_PARAMS 
{
    MMP_BOOL 				bStableShotEn;                      			///< Enable/Disable stable shot. 
    MMP_USHORT				usStableShotNum;			             		///< DSC stable shot capture number
	MMP_BOOL    			bDscPreviewKeepOn;								///< DSC keep preview output when capture

    MMP_USHORT  			usDscEncW[DSC_RESOL_MAX_NUM];					///< DSC capture width for each resolution
    MMP_USHORT  			usDscEncH[DSC_RESOL_MAX_NUM];		     		///< DSC capture height for each resolution

    // ++ Quality control
    MMP_UBYTE   			ubDscQtable[DSC_QTABLE_MAX_NUM][128];			///< DSC capture, quality table
    // -- Quality control

    // ++ For Thumbnail
	MMP_ULONG  				ulMaxThumbnailSize;								///< DSC capture, thumbnail maximum size
    #if (DSC_SUPPORT_BASELINE_MP_FILE)
    MMP_ULONG  				ulMaxLargeThumbSize;							///< DSC capture, large thumbnail maximum size
    #endif
    // -- For Thumbnail
} MMPS_DSC_ENC_PARAMS;

typedef struct _MMPS_DSC_DEC_PARAMS 
{
    MMP_ULONG 				ulExifWorkingBufSize;                     		///< EXIF working buffer size   
    MMP_ULONG   			ulDscDecMaxLineBufSize;							///< DSC supported maximum Jpeg decode line buffer size
    MMP_ULONG   			ulDscDecMaxCompBufSize;							///< DSC decode, reserve data input buffer size
    MMP_ULONG  				ulDscDecMaxInbufSize;							///< DSC decode, reserve data temp buffer size
    MMP_DISPLAY_COLORMODE  	DecColorMode[DSC_DECODE_MODE_MAX_NUM];			///< DSC decode, decode output color mode
	MMP_USHORT				usDecodeOutW[DSC_DECODE_MODE_MAX_NUM];			///< DSC decode output width (IBC frame buffer width)
	MMP_USHORT				usDecodeOutH[DSC_DECODE_MODE_MAX_NUM];			///< DSC decode output height (IBC frame buffer height)
	MMP_BOOL				bMirror[DSC_DECODE_MODE_MAX_NUM];				///< DSC decode display mirror attribute
	MMP_DISPLAY_ROTATE_TYPE rotatetype[DSC_DECODE_MODE_MAX_NUM];  			///< DSC decode display rotation attribute
    MMP_SCAL_FIT_MODE		fitmode[DSC_DECODE_MODE_MAX_NUM];		  		///< DSC decode display scale auto-fit mode
    MMP_BOOL				bDoubleOutputBuf[DSC_DECODE_MODE_MAX_NUM];		///< DSC decode, double buffer for store decoded raw buffer
} MMPS_DSC_DEC_PARAMS;

typedef struct _MMPS_DSC_CONFIG 
{
    MMPS_DSC_DISP_PARAMS 	dispParams;                      				///< DSC preview related settings
    MMPS_DSC_ENC_PARAMS   	encParams;                        				///< DSC capture related settings
    MMPS_DSC_DEC_PARAMS	 	decParams;										///< DSC decode related settings
} MMPS_DSC_CONFIG;

#if (EXIF_MANUAL_UPDATE)
typedef struct _MMPS_DSC_EXIF_CONFIG
{
    MMP_ULONG  				AtomSize;   //AtomSize should be ALIGN32(DataSize + 18)
    MMP_ULONG  				DataSize;
    MMP_ULONG  				TagID;
    MMP_ULONG  				IfdID;
    MMP_ULONG  				TagType;
    MMP_ULONG  				TagDataSize;
    MMP_UBYTE  				*Data;
} MMPS_DSC_EXIF_CONFIG;
#endif

typedef struct _MMPS_DSC_AHC_PREVIEW_INFO 
{
	MMP_BOOL                bUserDefine;
	MMP_BOOL             	bPreviewRotate;
	MMP_DISPLAY_ROTATE_TYPE	sPreviewDmaDir;
	MMP_SCAL_FIT_MODE		sFitMode;
	MMP_ULONG               ulPreviewBufW;
	MMP_ULONG              	ulPreviewBufH;
	MMP_ULONG               ulDispStartX;
	MMP_ULONG               ulDispStartY;
	MMP_ULONG               ulDispWidth;
	MMP_ULONG               ulDispHeight;
} MMPS_DSC_AHC_PREVIEW_INFO;

typedef struct _MMPS_DSC_AHC_PLAYBACK_INFO 
{
	MMP_BOOL                bUserDefine;
	MMP_BOOL             	bRotate;
	MMP_DISPLAY_ROTATE_TYPE	sRotateDir;
	MMP_SCAL_FIT_MODE		sFitMode;
	MMP_ULONG               ulDecodeOutW;
	MMP_ULONG              	ulDecodeOutH;
	MMP_ULONG               ulDispStartX;
	MMP_ULONG               ulDispStartY;
	MMP_ULONG               ulDispWidth;
	MMP_ULONG               ulDispHeight;
    MMP_DISPLAY_COLORMODE 	sDispColor;
} MMPS_DSC_AHC_PLAYBACK_INFO;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
/* Config Function */
MMPS_DSC_CONFIG*  MMPS_DSC_GetConfig(void);
MMP_ERR MMPS_DSC_SetPreviewMode(MMP_USHORT usMode);
MMP_ERR MMPS_DSC_ResetPreviewMode(void);
MMP_ERR MMPS_DSC_GetPreviewMode(MMP_USHORT *usMode);
MMP_ERR MMPS_DSC_SetCallbackFunc(MMP_ULONG ulEvent, void *pCallback);
#if (SUPPORT_LDC_CAPTURE)
MMP_ERR MMPS_DSC_SetLdcRunMode(MMP_UBYTE ubRunMode);
MMP_ERR MMPS_DSC_SetLdcResMode(MMP_UBYTE ubResMode, MMP_UBYTE ubFpsMode);
#endif

/* Memory Function */
MMP_ERR MMPS_DSC_ExitMode(void);
MMP_ERR MMPS_DSC_SetPreviewBuf(	MMPD_FCTL_ATTR 	*pFctlPreviewAttr, 
								MMP_ULONG 		ulDispWidth, 
								MMP_ULONG 		ulDispHeight, 
								MMP_BOOL 		bAllocate);
MMP_ERR MMPS_DSC_SetCaptureBuf(	MMP_ULONG	*ulSramPos,
								MMP_ULONG	*ulDramPos,
								MMP_BOOL	bAllocateForVideoRec,
								MMP_BOOL 	bAllocateForRawproc,
								MMP_DSC_CAPTURE_ROTATE_TYPE CaptureRotateType, 
								MMP_BOOL 	bAllocate);
MMP_ERR MMPS_DSC_SetSystemBuf(MMP_ULONG *ulCurBufPos, MMP_BOOL bISP, MMP_BOOL bFilename, MMP_BOOL bAllocate);
MMP_ERR MMPS_DSC_GetCaptureBuf(MMP_ULONG *pulLineBuf, MMP_ULONG *pulCompBuf);
MMP_ERR MMPS_DSC_SetMJpegEncBuf(MMP_ULONG *ulCurBufPos, MMP_USHORT usEncW, MMP_USHORT usEncH, MMP_BOOL bAllocate);
MMP_ERR MMPS_DSC_GetMJpegCompBuf(MMP_ULONG *pulBufAddr, MMP_ULONG *pulBufSize);
MMP_ERR MMPS_DSC_SetMaxRingBufferSize(MMP_ULONG ulMaxSize);

/* Preview Function */
MMP_ERR MMPS_DSC_SetPreviewPipe(MMP_UBYTE ubPipe);
MMP_ERR MMPS_DSC_SetPreviewPath(MMP_DSC_PREVIEW_PATH path, MMP_RAW_COLORFMT rawColorFormat);
MMP_ERR MMPS_DSC_InitPreview(MMP_UBYTE ubSnrSel);
MMP_ERR MMPS_DSC_CustomedPreviewAttr(MMP_BOOL 	bUserConfig,
									 MMP_BOOL 	bRotate,
									 MMP_UBYTE 	ubRotateDir,
									 MMP_UBYTE	sFitMode,
									 MMP_USHORT usBufWidth, MMP_USHORT usBufHeight, 
									 MMP_USHORT usStartX, 	MMP_USHORT usStartY,
                                     MMP_USHORT usWinWidth, MMP_USHORT usWinHeight);
MMP_ERR MMPS_DSC_EnablePreviewDisplay(MMP_UBYTE ubSnrSel, MMP_BOOL bEnable, MMP_BOOL bCheckFrameEnd);
MMP_ERR MMPS_DSC_GetPreviewStatus(MMP_BOOL *bEnable);
MMP_ERR MMPS_DSC_RestorePreview(MMP_UBYTE ubSnrSel, MMP_USHORT usWindowsTime);
void MMPS_DSC_GetPreviewPipe(MMP_IBC_PIPEID *pPipe);

/* PTZ Function */
MMP_ERR 	MMPS_DSC_SetZoomConfig(MMP_USHORT usSteps, MMP_USHORT ubMaxRatio);
MMP_ERR 	MMPS_DSC_GetCurZoomStep(MMP_USHORT *usCurZoomStep);
MMP_ERR 	MMPS_DSC_SetPreviewZoom(MMP_PTZ_ZOOM_DIR sZoomDir, MMP_USHORT usCurZoomStep);
MMP_UBYTE 	MMPS_DSC_GetCurZoomStatus(void);
/* MJPEG Function */
MMP_ERR MMPS_DSC_SetMJpegResol(MMP_USHORT usW, MMP_USHORT usH);
MMP_ERR MMPS_DSC_SetVideo2MJpegEnable(MMP_BOOL bEnable);
MMP_BOOL MMPS_DSC_GetVideo2MJpegEnable(void);
MMP_ERR MMPS_DSC_SetTranscodeRate(MMP_ULONG InFpsx10, MMP_ULONG OutFpsx10, MMP_ULONG ulBitrate);
MMP_ERR MMPS_DSC_ConfigVideo2Mjpeg(MMP_SHORT usInputW, MMP_USHORT usInputH);

/* Capture Function */
MMP_ERR MMPS_DSC_CheckContiShotStreamBusy(MMP_ULONG *pbBusy);
MMP_ERR MMPS_DSC_SetContiShotMaxNum(MMP_ULONG ulMaxShotNum);
MMP_ERR MMPS_DSC_SetContiShotInterval(MMP_ULONG ulInterval);
MMP_ERR MMPS_DSC_SetContiShotMaxDramAddr(MMP_ULONG ulAddr);

MMP_ERR MMPS_DSC_EnableScaleUpCapture(MMP_BOOL bScaleUpCaptureEn);
MMP_ERR MMPS_DSC_SetJpegEncParam(MMP_USHORT usJpegResol, MMP_USHORT usW, MMP_USHORT usH, MMP_UBYTE ubRcIdx);
MMP_ERR MMPS_DSC_SetCapturePipe(MMP_UBYTE ubPipe);
MMP_ERR MMPS_DSC_SetShotMode(MMPS_DSC_CAPTUREMODE ubMode);
MMP_ERR MMPS_DSC_SetCustomQTable(MMP_UBYTE ubMode, MMP_UBYTE *pTable, MMP_UBYTE *pTableU, MMP_UBYTE *pTableV);
MMP_ERR MMPS_DSC_SetCaptureJpegQuality(	MMP_UBYTE               ubRcIdx,
                                        MMP_BOOL 				bTargetCtrl, 
										MMP_BOOL 				bLimitCtrl,
										MMP_ULONG   			ulTargetSize,
										MMP_ULONG   			ulLimitSize,
                                        MMP_USHORT 				usMaxCount, 
                                        MMP_DSC_JPEG_QUALITY 	qualitylevel);
MMP_ERR MMPS_DSC_SetCaptureJpegQualityEx(MMP_UBYTE              ubRcIdx,
                                         MMP_BOOL 				bTargetCtrl, 
										 MMP_BOOL 				bLimitCtrl,
										 MMP_ULONG   			ulTargetSize,
										 MMP_ULONG   			ulLimitSize,
                                         MMP_USHORT 			usMaxCount, 
                                         MMP_DSC_JPEG_QUALITY 	qualitylevel);
MMP_ERR MMPS_DSC_CaptureAfterSrcReady(	MMP_IBC_PIPEID 			pipeID,
								 		MMPS_DSC_CAPTURE_INFO 	*pCaptureInfo,
								 		MMP_SCAL_SOURCE			ScalerSrc,
								 		MMP_SCAL_FIT_RANGE 		*pCaptureFitrange, 
								 		MMP_SCAL_GRAB_CTRL 		*pCaptureGrabctl,
								 		MMP_ULONG              	*pCaptureSize,
								 		MMP_BOOL                bDualCamCapture);
MMP_ERR MMPS_DSC_CaptureTakeJpeg(MMP_UBYTE ubSnrSel, MMPS_DSC_CAPTURE_INFO *pCaptureInfo);
MMP_ERR MMPS_DSC_GetCaptureStatus(MMP_USHORT *usDramStatus, MMP_USHORT *usImgNumInBuf);
/* Playback Function */
MMP_ERR MMPS_DSC_SetPlaybackMode(MMP_USHORT usDecodeMode);
MMP_ERR MMPS_DSC_SetPlaybackDispCtrl(MMP_USHORT usDispCtrl, MMP_USHORT usDispWinId);
MMP_ERR MMPS_DSC_SetPlaybackPipe(MMP_UBYTE ubPipe);
MMP_ERR MMPS_DSC_CustomedPlaybackAttr(MMP_BOOL 	bUserConfig,
									  MMP_BOOL 	bRotate,
									  MMP_UBYTE ubRotateDir,
									  MMP_UBYTE	sFitMode,
									  MMP_USHORT usBufWidth, MMP_USHORT usBufHeight, 
									  MMP_USHORT usStartX, 	 MMP_USHORT usStartY,
                                      MMP_USHORT usWinWidth, MMP_USHORT usWinHeight);
MMP_ERR MMPS_DSC_SetJpegDisplay(MMP_ULONG ulGrabX, MMP_ULONG ulGrabY, MMP_ULONG ulGrabW, MMP_ULONG ulGrabH);
MMP_ERR MMPS_DSC_PlaybackJpeg(MMP_DSC_JPEG_INFO *pJpegInfo);
MMP_ERR MMPS_DSC_PlaybackCalculatePTZ(MMP_ULONG ulZoomStep,
									  MMP_ULONG *pulGrabX, 	MMP_ULONG *pulGrabY, 
									  MMP_ULONG *pulGrabW,	MMP_ULONG *pulGrabH);
MMP_ERR MMPS_DSC_PlaybackExecutePTZ(MMP_ULONG ulGrabX, MMP_ULONG ulGrabY,
								 	MMP_ULONG ulGrabW, MMP_ULONG ulGrabH);
MMP_ERR MMPS_DSC_ExitJpegPlayback(void);
MMP_ERR MMPS_DSC_GetJpegInfo(MMP_DSC_JPEG_INFO *pJpegInfo);
MMP_ERR MMPS_DSC_DecodeThumbFirst(MMP_DSC_JPEG_INFO       *pJpegInfo, 
                                  MMP_USHORT              usOutputWidth, 
                                  MMP_USHORT              usOutputHeight, 
                                  MMP_DISPLAY_COLORMODE  outputformat);
MMP_ERR MMPS_DSC_GetJpegDispBufAttr(MMP_GRAPHICS_BUF_ATTR *pJpegBufAttr);

/* Sticker Function */
MMP_ERR MMPS_DSC_SetSticker(MMP_STICKER_ATTR *pPrimaryJpgSticker, MMP_STICKER_ATTR *pThumbnailSticker);

/* Exif Function */
MMP_ERR MMPS_DSC_EnableMultiPicFormat(MMP_BOOL bEnableMpf, MMP_BOOL bEnableLargeThumb);
MMP_ERR MMPS_DSC_ConfigLargeThumbRes(MMP_UBYTE ubResMode);
MMP_ERR MMPS_DSC_ConfigThumbnail(MMP_ULONG ThumbWidth, MMP_ULONG ThumbHeight, MMP_DSC_THUMB_INPUT_SRC thumbInputMode);
MMP_ERR MMPS_DSC_OpenEXIFFile(MMP_UBYTE *pubFileName, MMP_USHORT usExifNodeId);
MMP_ERR MMPS_DSC_UpdateExifBuffer(MMP_USHORT 	usExifNodeId, 
								  MMP_USHORT 	usIfd, 	MMP_USHORT 	usTag,
								  MMP_USHORT 	usType, MMP_ULONG 	ulCount,
								  MMP_UBYTE		*pData, MMP_ULONG 	ulDataSize, 
                                  MMP_BOOL 		bForUpdate);
MMP_ERR MMPS_DSC_GetImageEXIFInfo(MMP_USHORT usExifNodeId, MMP_USHORT usIfd, MMP_USHORT usTag, MMP_UBYTE *pData, MMP_ULONG *pulSize);
MMP_ERR MMPS_DSC_CloseEXIFFile(MMP_USHORT usExifNodeId);

#if (EXIF_MANUAL_UPDATE)
MMP_BOOL MMPS_DSC_GetImageEXIF(MMP_ULONG Index, MMPS_DSC_EXIF_CONFIG *pExif);
MMP_BOOL MMPS_DSC_SetImageEXIF(MMP_UBYTE *pData, MMP_ULONG DataSize, MMP_USHORT uwIfdIdx, MMP_USHORT uwTagID, 
							   MMP_USHORT uwType, MMP_USHORT uwCount);
#endif

/* Storage Function */
MMP_ERR MMPS_DSC_GetFileNameAddr(MMP_ULONG *ulFileNameAddr);
MMP_ERR MMPS_DSC_JpegDram2Card(MMPS_DSC_CAPTURE_INFO *pCaptureInfo);
MMP_ERR MMPS_DSC_EncodeRaw2Jpeg(MMP_DSC_JPEG_INFO *pJpegInfo, MMP_GRAPHICS_BUF_ATTR *pJpegBufAttr);
MMP_ERR MMPS_DSC_GetRawData(MMP_UBYTE   ubSnrSel,
                            MMP_ULONG 	ulStoreAddr,	MMP_BOOL 	bStopPreviw,
							MMP_BYTE 	ubFilename[], 	MMP_USHORT 	usLength, 
							MMP_ULONG 	*ulWidth, 		MMP_ULONG 	*ulHeight);
#if (SUPPORT_VR_THUMBNAIL)
MMP_ERR MMPS_DSC_JpegDram2Chunk(void);
#endif

/* FactoryTool Function */
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
							   MMP_ULONG *ulIbcStoreSize);

MMP_ERR MMPS_FT_EnableGetRawData(MMP_UBYTE ubSnrSel);
/// @}

#endif // _MMPS_DSC_H_

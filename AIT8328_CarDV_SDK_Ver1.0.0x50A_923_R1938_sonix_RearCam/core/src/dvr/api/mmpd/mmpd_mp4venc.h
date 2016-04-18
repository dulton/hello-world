/// @ait_only
/**
 @file mmpd_mp4venc.h
 @brief INCLUDE File of Host VIDEO ENCODE Driver.
 @author Will Tseng
 @version 1.0
*/


#ifndef _MMPD_MP4VENC_H_
#define _MMPD_MP4VENC_H_

#include "mmp_lib.h"
#include "ait_config.h"
#include "mmpd_rawproc.h"

/** @addtogroup MMPD_3GP
 *  @{
 */

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef enum _MMPD_VIDENC_MODULE_ID {
	MMPD_VIDENC_MODULE_H264 = 0,
    MMPD_VIDENC_MODULE_MAX
} MMPD_VIDENC_MODULE_ID;

/// Firmware status. For video encode, the sequence is the same as firmware.
typedef enum _MMPD_MP4VENC_FW_OP {
    MMPD_MP4VENC_FW_OP_NONE = 0x00,
    MMPD_MP4VENC_FW_OP_START,
    MMPD_MP4VENC_FW_OP_PAUSE,
    MMPD_MP4VENC_FW_OP_RESUME,
    MMPD_MP4VENC_FW_OP_STOP,
    MMPD_MP4VENC_FW_OP_PREENCODE
} MMPD_MP4VENC_FW_OP;

/// Video format
typedef enum _MMPD_MP4VENC_FORMAT {
    MMPD_MP4VENC_FORMAT_OTHERS = 0x00,
    MMPD_MP4VENC_FORMAT_H264
} MMPD_MP4VENC_FORMAT;

/// Video Current Buffer Mode
typedef enum _MMPD_MP4VENC_CURBUF_MODE {
    MMPD_MP4VENC_CURBUF_FRAME = 0x00,
    MMPD_MP4VENC_CURBUF_RT,
    MMPD_MP4VENC_CURBUF_MAX
} MMPD_MP4VENC_CURBUF_MODE;

/// Video profile
typedef enum _MMPD_VIDENC_PROFILE {
    MMPD_H263ENC_PROFILE_NONE = 0x00,
    MMPD_H263ENC_PROFILE_0,                             ///< H.263 profile 0
    MMPD_H263ENC_PROFILE_3,                             ///< H.263 profile 3
    MMPD_H263ENC_PROFILE_MAX,
    MMPD_MP4VENC_PROFILE_NONE,
    MMPD_MP4VENC_SIMPLE_PROFILE,                        ///< MPEG4 simple profile
    MMPD_MP4VENC_ADV_SIMPLE_PROFILE,                    ///< MPEG4 advanced simple profile
    MMPD_MP4VENC_PROFILE_MAX,
    MMPD_H264ENC_PROFILE_NONE,
    MMPD_H264ENC_BASELINE_PROFILE,                      ///< H.264 baseline profile
    MMPD_H264ENC_MAIN_PROFILE,                          ///< H.264 main profile
    MMPD_H264ENC_HIGH_PROFILE,                          ///< H.264 high profile
    MMPD_H264ENC_PROFILE_MAX
} MMPD_VIDENC_PROFILE;

typedef enum _MMPD_VIDENC_MCI_MODE {
    #define MCI_MODE_MASK       	(0x000F)
    #define MCI_PIPE_MASK       	(0x00F0)
    #define MCI_RAWS_MASK       	(0x0F00)
    #define MCI_2NDPIPE_MASK    	(0xF000)

    #define MCI_SET_MODE(m)     	(m)
    #define MCI_SET_PIPE(p)     	((p) << 4)
    #define MCI_SET_RAWS(s)     	((s) << 8)
    #define MCI_SET_2NDPIPE(p)		((p) << 12)

    #define MCI_GET_MODE(_mode) 	(_mode & MCI_MODE_MASK)
    #define MCI_GET_PIPE(_mode) 	((_mode & MCI_PIPE_MASK) >> 4)
    #define MCI_GET_RAWS(_mode) 	((_mode & MCI_RAWS_MASK) >> 8)
    #define MCI_GET_2NDPIPE(_mode) 	((_mode & MCI_2NDPIPE_MASK) >> 12)

    MMPD_VIDENC_MCI_DEFAULT      			= MCI_SET_MODE(1),
    MMPD_VIDENC_MCI_DMAR_H264    			= MCI_SET_MODE(2),
    MMPD_VIDENC_MCI_DMAR_H264_P0 			= MCI_SET_MODE(2)|MCI_SET_PIPE(0),
    MMPD_VIDENC_MCI_DMAR_H264_P1 			= MCI_SET_MODE(2)|MCI_SET_PIPE(1),
    MMPD_VIDENC_MCI_DMAR_H264_P2 			= MCI_SET_MODE(2)|MCI_SET_PIPE(2),
    MMPD_VIDENC_MCI_DMAR_H264_P3 			= MCI_SET_MODE(2)|MCI_SET_PIPE(3),
    MMPD_VIDENC_MCI_RAW_DVI      			= MCI_SET_MODE(3),
    MMPD_VIDENC_MCI_RAW0_DVI_P0   			= MCI_SET_MODE(3)|MCI_SET_PIPE(0),
    MMPD_VIDENC_MCI_RAW0_DVI_P1   			= MCI_SET_MODE(3)|MCI_SET_PIPE(1),
    MMPD_VIDENC_MCI_RAW0_DVI_P2   			= MCI_SET_MODE(3)|MCI_SET_PIPE(2),
    MMPD_VIDENC_MCI_RAW0_DVI_P3   			= MCI_SET_MODE(3)|MCI_SET_PIPE(3),
    MMPD_VIDENC_MCI_RAWS1_DVI_P0 			= MCI_SET_MODE(3)|MCI_SET_PIPE(0)|MCI_SET_RAWS(1),
    MMPD_VIDENC_MCI_RAWS1_DVI_P1 			= MCI_SET_MODE(3)|MCI_SET_PIPE(1)|MCI_SET_RAWS(1),
    MMPD_VIDENC_MCI_GRA_LDC_H264 			= MCI_SET_MODE(4),
    MMPD_VIDENC_MCI_GRA_LDC_P0				= MCI_SET_MODE(4)|MCI_SET_PIPE(0),
    MMPD_VIDENC_MCI_GRA_LDC_P1				= MCI_SET_MODE(4)|MCI_SET_PIPE(1),
    MMPD_VIDENC_MCI_GRA_LDC_P2				= MCI_SET_MODE(4)|MCI_SET_PIPE(2),
    MMPD_VIDENC_MCI_GRA_LDC_P3				= MCI_SET_MODE(4)|MCI_SET_PIPE(3),
    MMPD_VIDENC_MCI_GRA_LDC_P0P2_H264_P1	= MCI_SET_MODE(4)|MCI_SET_PIPE(0)|MCI_SET_2NDPIPE(2),
    MMPD_VIDENC_MCI_GRA_LDC_P3P1_H264_P0	= MCI_SET_MODE(4)|MCI_SET_PIPE(3)|MCI_SET_2NDPIPE(1),
    MMPD_VIDENC_MCI_INVALID      			= MCI_SET_MODE(MCI_MODE_MASK)
} MMPD_VIDENC_MCI_MODE;

#define	VIDEO_INIT_QP_STEP_NUM      3
#define	VIDEO_INPUT_FB_MAX_CNT      4

typedef struct _MMPD_MP4VENC_INPUT_BUF {
	MMP_ULONG ulBufCnt;
    MMP_ULONG ulY[VIDEO_INPUT_FB_MAX_CNT];	///< Video encode input Y buffer start address (can be calculated)
    MMP_ULONG ulU[VIDEO_INPUT_FB_MAX_CNT]; 	///< Video encode input U buffer start address (can be calculated)
    MMP_ULONG ulV[VIDEO_INPUT_FB_MAX_CNT]; 	///< Video encode input V buffer start address (can be calculated)
} MMPD_MP4VENC_INPUT_BUF;

typedef struct _MMPD_MP4VENC_MISC_BUF {
    MMP_ULONG ulYDCBuf;			            ///< Video encode Y DC start buffer (can be calculated)
    MMP_ULONG ulYACBuf;						///< Video encode Y AC start buffer (can be calculated)
    MMP_ULONG ulUVDCBuf; 			        ///< Video encode UV DC start buffer (can be calculated)
    MMP_ULONG ulUVACBuf;					///< Video encode UV AC start buffer (can be calculated)
    MMP_ULONG ulMVBuf;						///< Video encode MV start buffer (can be calculated)
    MMP_ULONG ulSliceLenBuf;                ///< H264 slice length buf
} MMPD_MP4VENC_MISC_BUF;

typedef struct _MMPD_H264ENC_HEADER_BUF {
    MMP_ULONG ulSPSStart;
    MMP_ULONG ulSPSSize;
    MMP_ULONG ulTmpSPSStart;
    MMP_ULONG ulTmpSPSSize;
    MMP_ULONG ulPPSStart;
    MMP_ULONG ulPPSSize;
} MMPD_H264ENC_HEADER_BUF;

typedef struct _MMPD_MP4VENC_BITSTREAM_BUF {
    MMP_ULONG ulStart;      		  		///< Video encode compressed buffer start address (can be calculated)
    MMP_ULONG ulEnd;		          		///< Video encode compressed buffer end address (can be calculated)
} MMPD_MP4VENC_BITSTREAM_BUF;

typedef struct _MMPD_MP4VENC_LINE_BUF {
    MMP_ULONG ulY[12];          ///< ME Y line start address, 12 lines for h264 high profile
    MMP_ULONG ulU[12];		    ///< ME U line start address, 12 lines for h264 high profile UV interleave mode
    MMP_ULONG ulV[12];		    ///< ME Y line start address
    MMP_ULONG ulUP[4];          ///< ME Y up sample, for h264
} MMPD_MP4VENC_LINE_BUF;

typedef struct _MMPD_MP4VENC_REFGEN_BUF {
    MMP_ULONG ulGenY;			///< Video encode Y generate buffer
    MMP_ULONG ulGenU;			///< Video encode U generate buffer
    MMP_ULONG ulGenV;			///< Video encode V generate buffer
    MMP_ULONG ulRefY;     		///< Video encode Y reference buffer
    MMP_ULONG ulRefU;    		///< Video encode U reference buffer
    MMP_ULONG ulRefV;	   		///< Video encode V reference buffer
} MMPD_MP4VENC_REFGEN_BUF;

typedef struct _MMPD_MP4VENC_REFGEN_BD {
    MMP_ULONG ulYStart;			///< Video encode Y reference buffer start address (can be calculated)
    MMP_ULONG ulYEnd;			///< Video encode Y reference buffer end address (can be calculated)
    MMP_ULONG ulUStart;			///< Video encode U reference buffer start address (can be calculated)
    MMP_ULONG ulUEnd;     		///< Video encode U reference buffer end address (can be calculated)
    MMP_ULONG ulVStart;    		///< Video encode V reference buffer start address (can be calculated)
    MMP_ULONG ulVEnd;      		///< Video encode V reference buffer end address (can be calculated)
    MMP_ULONG ulGenStart;       ///< deblock out YUV start addr, H264
    MMP_ULONG ulGenEnd;         ///< deblock out YUV end addr, H264
    MMP_ULONG ulGenYStart;
    MMP_ULONG ulGenYEnd;
    MMP_ULONG ulGenUVStart;
    MMP_ULONG ulGenUVEnd;
} MMPD_MP4VENC_REFGEN_BD;

typedef struct _MMPD_MP4VENC_VIDEOBUF {
	MMPD_MP4VENC_MISC_BUF	miscbuf;
	MMPD_MP4VENC_BITSTREAM_BUF	bsbuf;
	MMPD_MP4VENC_LINE_BUF	linebuf;
	MMPD_MP4VENC_REFGEN_BD	refgenbd;
} MMPD_MP4VENC_VIDEOBUF;

typedef struct _MMPD_VIDENC_FUNCTIONS {
	MMP_ERR (*MMPD_VIDENC_InitModule)(MMPD_VIDENC_MODULE_ID ModId);
	MMP_ERR (*MMPD_VIDENC_InitInstance)(MMP_ULONG *InstId, MMPD_VIDENC_MODULE_ID ModuleId);
	MMP_ERR (*MMPD_VIDENC_DeInitInstance)(MMP_ULONG InstId);
// Buffer related API
	MMP_ERR (*MMPD_VIDENC_SetBitstreamBuf)(MMP_ULONG ulEncId, MMPD_MP4VENC_BITSTREAM_BUF *bsbuf);
// Feature related API
    MMP_ERR (*MMPD_VIDENC_SetResolution)(MMP_ULONG ulEncId, MMP_USHORT usWidth, MMP_USHORT usHeight);
    MMP_ERR (*MMPD_VIDENC_SetProfile)(MMP_ULONG ulEncId, MMP_ULONG ulProfile);
    MMP_ERR (*MMPD_VIDENC_SetEncodeMode)(void);
    MMP_ERR (*MMPD_VIDENC_SetGOP)(MMP_ULONG ulEncId, MMP_USHORT usPFrame, MMP_USHORT usBFrame);
    MMP_ERR (*MMPD_VIDENC_SetBitrate)(MMP_ULONG ulEncId, MMP_ULONG ulTargetSize, MMP_ULONG ulBitrate);
    MMP_ERR (*MMPD_VIDENC_SetEncFrameRate)(MMP_ULONG ulEncId, MMP_ULONG ulTimeIncrement, MMP_ULONG ulTimeResol);
    MMP_ERR (*MMPD_VIDENC_UpdateEncFrameRate)(MMP_ULONG ulEncId, MMP_ULONG ulTimeIncrement, MMP_ULONG ulTimeResol);
    MMP_ERR (*MMPD_VIDENC_SetSnrFrameRate)(MMP_ULONG ulEncId, MMP_ULONG ulTimeIncrement, MMP_ULONG ulTimeResol);
	MMP_ERR (*MMPD_VIDENC_EnableLineBuf)(MMP_BOOL bLineEn, MMP_BOOL bLCLEn);
// Operation related API
	MMP_ERR (*MMPD_VIDENC_EnableClock)(MMP_BOOL bEnable);
    MMP_ERR (*MMPD_VIDENC_SetDVSMode)(MMP_USHORT usWidth, MMP_USHORT usHeight, MMP_ULONG *ulMVBufAddr,
            MMP_USHORT *usMVUnitSize, MMP_RAW_STORE_BUF *rawbuf, MMP_ULONG *ulSramAddr, MMP_ULONG *ulDramAddr);
} MMPD_VIDENC_FUNCTIONS;

// DUALENC_SUPPORT
typedef enum{
	MMPD_SET_REFLOW = 0,
	MMPD_SET_REFHIGH,
	MMPD_SET_RECBUF
}MMPD_VIDENC_MULTI_SETBUFTYPE;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

// Generic API -----------------------------------------------------------------
MMP_BOOL MMPD_VIDENC_IsModuleInit(MMPD_VIDENC_MODULE_ID ModId);
MMP_ERR MMPD_VIDENC_DeinitModule(MMPD_VIDENC_MODULE_ID ModId);
MMP_ERR MMPD_VIDENC_SetFormat(MMPD_MP4VENC_FORMAT VideoFormat);
MMP_ERR MMPD_VIDENC_EnableTimer(MMP_BOOL bEnable);
MMP_ERR MMPD_VIDENC_InitModule (MMPD_VIDENC_MODULE_ID ModId);
MMP_ERR MMPD_VIDENC_InitInstance(MMP_ULONG *InstId, MMPD_VIDENC_MODULE_ID ModuleId);
MMP_ERR MMPD_VIDENC_DeInitInstance(MMP_ULONG InstId);
MMP_ERR MMPD_VIDENC_SetBitstreamBuf(MMP_ULONG ulEncId, MMPD_MP4VENC_BITSTREAM_BUF *bsbuf);
MMP_ERR MMPD_VIDENC_SetResolution(MMP_ULONG ulEncId, MMP_USHORT usWidth, MMP_USHORT usHeight);
MMP_ERR MMPD_VIDENC_SetQuality(MMP_ULONG ulEncId, MMP_ULONG ulTargetSize, MMP_ULONG ulBitrate);
MMP_ERR MMPD_VIDENC_SetProfile(MMP_ULONG ulEncId, MMPD_VIDENC_PROFILE profile);
MMP_ERR MMPD_VIDENC_SetEncodeMode(void);
MMP_ERR MMPD_VIDENC_SetGOP(MMP_ULONG ulEncId, MMP_USHORT usPFrame, MMP_USHORT usBFrame);
MMP_ERR MMPD_VIDENC_SetBitrate(MMP_ULONG ulEncId, MMP_ULONG ulTargetSize, MMP_ULONG ulBitrate);
MMP_ERR MMPD_VIDENC_SetEncFrameRate(MMP_ULONG ulEncId, MMP_ULONG ulTimeIncrement, MMP_ULONG ulTimeResol);
MMP_ERR MMPD_VIDENC_UpdateEncFrameRate(MMP_ULONG ulEncId, MMP_ULONG ulTimeIncrement, MMP_ULONG ulTimeResol);
MMP_ERR MMPD_VIDENC_SetSnrFrameRate(MMP_ULONG ulEncId, MMP_ULONG ulTimeIncrement, MMP_ULONG ulTimeResol);
MMP_ERR MMPD_VIDENC_SetCropping (MMP_ULONG ulEncId, MMP_USHORT usTop, MMP_USHORT usBottom, MMP_USHORT usLeft, MMP_USHORT usRight);
MMP_ERR MMPD_VIDENC_EnableLineBuf(MMP_BOOL bLineEn, MMP_BOOL bLCLEn);
MMP_ERR MMPD_VIDENC_SetCurBufMode(MMP_ULONG ulEncId, MMPD_MP4VENC_CURBUF_MODE VideoCurBufMode);
MMP_ERR MMPD_VIDENC_GetNumOpen(MMP_ULONG *ulNumOpening);
MMP_ERR MMPD_VIDENC_GetStatus(MMP_ULONG ulEncId, MMPD_MP4VENC_FW_OP *status);
MMP_ERR MMPD_VIDENC_GetMergerStatus(MMP_ERR *status, MMP_ULONG *tx_status);
MMP_ERR MMPD_VIDENC_CheckCapability(MMP_ULONG w, MMP_ULONG h, MMP_ULONG fps);
MMP_ERR MMPD_VIDENC_EnableClock(MMP_BOOL bEnable);

MMPD_MP4VENC_FORMAT MMPD_VIDENC_GetFormat(void);
MMP_USHORT MMPD_VIDENC_GetVideoWidth(void);
MMP_USHORT MMPD_VIDENC_GetVideoHeight(void);
MMP_ULONG  MMPD_VIDENC_GetTimeResolution(void);
MMP_ULONG  MMPD_VIDENC_GetTimeIncrement(void);
MMP_ERR MMPD_VIDENC_TuneMCIPriority(MMPD_VIDENC_MCI_MODE mode);
void MMPD_VIDENC_TuneEncodePipeMCIPriority(MMP_UBYTE ubPipe);
// End of Generic API ----------------------------------------------------------

// [H264] API
MMP_ERR MMPD_H264ENC_SetSourceBuf(MMPD_MP4VENC_INPUT_BUF *inputbuf);
MMP_ERR MMPD_H264ENC_SetRefGenBound(MMP_ULONG ubEncId, MMPD_MP4VENC_REFGEN_BD *refgenbd);
MMP_ERR MMPD_H264ENC_SetBitstreamBuf(MMP_ULONG ubEncId, MMPD_MP4VENC_BITSTREAM_BUF *bsbuf);
MMP_ERR MMPD_H264ENC_SetMiscBuf(MMP_ULONG ubEncId, MMPD_MP4VENC_MISC_BUF *miscbuf);
MMP_ERR MMPD_H264ENC_CalculateRefBuf(MMP_USHORT usWidth, MMP_USHORT usHeight,
									MMPD_MP4VENC_REFGEN_BD *refgenbd, MMP_ULONG *ulCurAddr);
MMP_ERR MMPD_H264ENC_InitModule(MMPD_VIDENC_MODULE_ID ModId);
MMP_ERR MMPD_H264ENC_InitInstance(MMP_ULONG *InstId, MMPD_VIDENC_MODULE_ID ModuleId);
MMP_ERR MMPD_H264ENC_DeInitInstance(MMP_ULONG InstId);
MMP_ERR MMPD_H264ENC_SetResolution(MMP_ULONG ulEncId, MMP_USHORT usWidth, MMP_USHORT usHeight);
MMP_ERR MMPD_H264ENC_SetProfile(MMP_ULONG ulEncId, MMP_ULONG ulProfile);
MMP_ERR MMPD_H264ENC_SetEncodeMode(void);
MMP_ERR MMPD_H264ENC_SetGOP(MMP_ULONG ulEncId, MMP_USHORT usPFrame, MMP_USHORT usBFrame);
MMP_ERR MMPD_H264ENC_SetPadding(MMP_ULONG ulEncId, MMP_USHORT usType, MMP_USHORT usCnt);
MMP_ERR MMPD_H264ENC_SetEncByteCnt(MMP_USHORT usByteCnt);
MMP_ERR MMPD_H264ENC_SetBitrate(MMP_ULONG ulEncId, MMP_ULONG ulTargetSize, MMP_ULONG ulBitrate);
MMP_ERR MMPD_H264ENC_SetEncFrameRate(MMP_ULONG ulEncId, MMP_ULONG usTimeIncrement, MMP_ULONG usTimeResol);
MMP_ERR MMPD_H264ENC_SetSnrFrameRate(MMP_ULONG ulEncId, MMP_ULONG usTimeIncrement, MMP_ULONG usTimeResol);
MMP_ERR MMPD_H264ENC_EnableLineBuf(MMP_BOOL bLineEn, MMP_BOOL bLCLEn);

MMP_ERR MMPD_H264ENC_SetStoreBufParams(MMP_ULONG ulBufAddr);
MMP_ERR MMPD_H264ENC_EnableClock(MMP_BOOL bEnable);
MMP_ERR MMPD_H264ENC_SetHeaderBuf(MMP_ULONG ulEncId, MMPD_H264ENC_HEADER_BUF *hdrbuf);

MMP_ERR MMPD_H264ENC_GetHeaderInfo(MMPD_H264ENC_HEADER_BUF *hdrbuf);

MMP_ERR MMPD_VIDENC_SetQPBoundary(MMP_ULONG ulLowerBound,MMP_ULONG ulUpperBound);

void        MMPD_VIDENC_SetMemStart(MMP_ULONG MemStartAddr);
MMP_ULONG   MMPD_VIDENC_MemAlloc(MMP_ULONG size);
MMP_ERR     MMPD_VIDENC_MemFree(void* ptr);
void MMPD_VIDENC_TuneEncodeSecondPipeMCIPriority(MMP_UBYTE ubPipe);
// UVC_VIDRECD_SUPPORT
MMP_ERR MMPD_H264ENC_SetUVCHdrBuf(MMPD_H264ENC_HEADER_BUF *hdrbuf);

/// @}

#endif // _MMPD_MP4VENC_H_

/// @end_ait_only

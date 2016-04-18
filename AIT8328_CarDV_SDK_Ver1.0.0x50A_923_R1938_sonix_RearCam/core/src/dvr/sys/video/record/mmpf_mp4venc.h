//==============================================================================
//
//  File        : mmpf_mp4venc.h
//  Description : Header function of video codec
//  Author      : Will Tseng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPF_MP4VENC_H_
#define _MMPF_MP4VENC_H_

#include "includes_fw.h"
#include "mmpf_3gpmgr.h"
#include "mmpf_h264enc.h"
#include "mmpf_vidcmn.h"

/** @addtogroup MMPF_VIDEO
@{
*/

//==============================================================================
//
//                              COMPILER OPTION
//
//==============================================================================

#if (SHARE_REF_GEN_BUF)&&(H264ENC_ICOMP_EN)
    #error With share ref/gen buffer, h264 image compression is not supported
#endif

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

// Audio format (only used in recoder)
#define	MMPF_AUD_FORMAT_OTHERS		        0x00 
#define	MMPF_AUD_FORMAT_AMR			        0x01
#define	MMPF_AUD_FORMAT_MP4A		        0x02

// Video record speed control
#define MMPF_VIDENC_SPEED_NORMAL            0x00
#define MMPF_VIDENC_SPEED_SLOW              0x01
#define MMPF_VIDENC_SPEED_FAST              0x02
#define MMPF_VIDENC_SPEED_1X                0x00
#define MMPF_VIDENC_SPEED_2X                0x01
#define MMPF_VIDENC_SPEED_3X                0x02
#define MMPF_VIDENC_SPEED_4X                0x03
#define MMPF_VIDENC_SPEED_5X                0x04
#define MMPF_VIDENC_SPEED_MAX               0x05

#define BASELINE_PROFILE        66
#define MAIN_PROFILE            77

// FREXT Profile IDC definitions
#define FREXT_HP                100      ///< YUV 4:2:0/8 "High"
#define FREXT_Hi10P             110      ///< YUV 4:2:0/10 "High 10"
#define FREXT_Hi422             122      ///< YUV 4:2:2/10 "High 4:2:2"
#define FREXT_Hi444             244      ///< YUV 4:4:4/12 "High 4:4:4"

// H264 qp bound
#define H264E_MAX_MB_QP         (46)
#define H264E_MIN_MB_QP         (10) //6

//The default threshold of accumulate skip frames within one second.
//While the number of accumulate skip frames in 1 second is over the
//threshold, card_slow event will be triggered by callback.
#define CARD_SLOW_THRESHOLD     (6)

#define VIDENC_ENCNUM           MAX_NUM_ENC_SET


//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

typedef enum _MMPF_H264ENC_HWRC_MODE {
    MMPF_H264ENC_HWRC_QPTUNE = 0,
    MMPF_H264ENC_HWRC_RDO,
    MMPF_H264ENC_HWRC_DIS
} MMPF_H264ENC_HWRC_MODE;

typedef enum _MMPF_VIDENC_DUMMY_FRAME_MODE {
    MMPF_VIDENC_DUMMY_FRAME_BY_REENC,
    MMPF_VIDENC_DUMMY_FRAME_BY_CONTAINER
} MMPF_VIDENC_DUMMY_FRAME_MODE;

typedef enum _MMPF_VIDENC_CB_TYPE {
    MMPF_VIDENC_CB_ME_DONE  = 0x00,
    MMPF_VIDENC_CB_MAX
} MMPF_VIDENC_CB_TYPE;

typedef enum _MMPF_VIDENC_CMPBUF_STATUS {
    MMPF_VIDENC_CMPBUF_LV0 = 0,
    MMPF_VIDENC_CMPBUF_LV1,
    MMPF_VIDENC_CMPBUF_LV2,
    MMPF_VIDENC_CMPBUF_LV3,
    MMPF_VIDENC_CMPBUF_LV4
} MMPF_VIDENC_CMPBUF_STATUS;

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef void MMPF_VIDENC_Callback(void);

typedef struct _MMPF_VIDENC_MODULE {
    MMP_BOOL            bInitMod;
    MMP_USHORT          Format;
    MMPF_H264ENC_MODULE H264EMod;
} MMPF_VIDENC_MODULE;

typedef struct _MMPF_VIDNEC_INSTANCE {
    MMP_BOOL                bInitInst;
    MMPF_VIDENC_MODULE      *Module;
    MMPF_H264ENC_ENC_INFO   h264e;
} MMPF_VIDENC_INSTANCE;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

#define get_vidinst_format(_p)      ((_p)->Module->Format)
#define get_vidinst_id(_p)          ((MMPF_VIDENC_INSTANCE*)(_p) - MMPF_VIDENC_GetInstance(0))

MMPF_VIDENC_MODULE   * MMPF_VIDENC_GetModule(MMP_USHORT ModId);
MMPF_VIDENC_INSTANCE * MMPF_VIDENC_GetInstance (MMP_UBYTE InstId);

MMP_ERR MMPF_VIDMULENC_QueueProc(MMPF_DUALENC_QUEUE *queue, MMP_ULONG qmaxsize, MMP_ULONG *data, MMP_USHORT type, void* pres);

void MMPF_VIDENC_SetVideoPath(MMP_USHORT usIBCPipe);
MMP_ULONG MMPF_VIDENC_GetQueueDepth (MMPF_VIDENC_QUEUE *queue, MMP_BOOL weighted);
MMP_ERR MMPF_VIDENC_ShowQueue(MMPF_VIDENC_QUEUE *queue, MMP_ULONG offset, MMP_ULONG *data, MMP_BOOL weighted);
void MMPF_VIDENC_ResetQueue(MMPF_VIDENC_QUEUE *queue);
MMP_ERR MMPF_VIDENC_PushQueue(MMPF_VIDENC_QUEUE *queue, MMP_ULONG buffer, MMP_BOOL weighted);
MMP_ERR MMPF_VIDENC_PopQueue(MMPF_VIDENC_QUEUE *queue, MMP_ULONG offset, MMP_ULONG *data, MMP_BOOL weighted);
MMP_ERR MMPF_VIDENC_GetParameter(MMP_UBYTE ubEncId, MMPF_VIDENC_ATTRIBUTE attrib, void *arg);
void MMPF_VIDENC_SetFrameInfoList (MMP_UBYTE ubEncID, MMP_ULONG ulYBuf[], MMP_ULONG ulUBuf[], MMP_ULONG ulVBuf[], MMP_UBYTE ubBufCnt);
MMP_ERR MMPF_VIDENC_SetEncodeEnable (MMPF_H264ENC_ENC_INFO *pEnc);
MMP_ERR MMPF_VIDENC_SetEncodeDisable(MMPF_H264ENC_ENC_INFO *pEnc);
MMPF_3GPMGR_FRAME_TYPE MMPF_VIDENC_GetVidRecdFrameType(MMPF_H264ENC_ENC_INFO *pEnc);
MMP_ULONG MMPF_VIDENC_GetTotalEncodeSize (void);
MMP_ERR MMPF_VIDENC_SetCropping (MMP_ULONG ulEncId, MMP_USHORT usTop, MMP_USHORT usBottom, MMP_USHORT usLeft, MMP_USHORT usRight);

MMP_ERR MMPF_VIDENC_TriggerEncode (void);
MMP_ERR MMPF_VIDENC_SetFrameReady(MMP_USHORT usEncID, MMP_ULONG *plCurBuf, MMP_ULONG *plIBCBuf);
MMP_BOOL MMPF_VIDENC_IsModuleInit(MMP_USHORT ModId);
MMP_ERR MMPF_VIDENC_InitModule(MMP_USHORT ModId);
MMP_ERR MMPF_VIDENC_DeinitModule(MMP_USHORT ModId);
MMP_ERR MMPF_VIDENC_InitInstance(MMP_ULONG *InstId, MMP_USHORT ModId);
MMP_ERR MMPF_VIDENC_DeInitInstance(MMP_ULONG InstId);
MMP_ERR MMPF_VIDENC_Start(MMPF_H264ENC_ENC_INFO *pEnc);
MMP_ERR MMPF_VIDENC_Stop(MMPF_H264ENC_ENC_INFO *pEnc);
MMP_ERR MMPF_VIDENC_Resume(MMP_ULONG ulEncId);
MMP_ERR MMPF_VIDENC_Pause(MMP_ULONG ulEncId);
MMP_ERR MMPF_VIDENC_PreEncode (MMPF_H264ENC_ENC_INFO *pEnc);
MMP_ERR MMPF_VIDENC_RegisterCallback(MMPF_VIDENC_CB_TYPE type, MMPF_VIDENC_Callback *pCallback);
MMP_BOOL MMPF_VIDENC_CheckCapability(MMP_ULONG total_mb, MMP_ULONG fps);

// functions of video encoder
void MMPF_MP4VENC_SetVideoEncodeQuality(MMP_ULONG ulEncId, MMP_ULONG ulIQp, MMP_ULONG ulPQp, MMP_ULONG ulBQp, MMP_ULONG TargetSize, MMP_ULONG Bitrate);
MMP_ERR MMPF_MP4VENC_SetVideoResolution(MMP_ULONG ulEncId, MMP_USHORT usResolW, MMP_USHORT usResolH);
MMP_ERR MMPF_MP4VENC_SetVideoProfile(MMP_ULONG ulEncId, MMP_ULONG ulProfile);
MMP_ERR MMPF_MP4VENC_SetVideoGOP(MMP_ULONG ulEncId, MMP_USHORT usPFrame, MMP_USHORT usBFrame);
void MMPF_MP4VENC_SetVideoEncodeFormat(MMP_USHORT videofmt);
void MMPF_MP4VENC_SetCurBufMode(MMP_ULONG ulEncId, MMP_USHORT curbufmode);
void MMPF_MP4VENC_SetStatus(MMP_ULONG ulEncId, MMP_USHORT status);
MMP_USHORT MMPF_MP4VENC_GetStatus(MMP_ULONG ulEncId);
MMP_USHORT MMPF_MP4VENC_GetVideoEncodeFormat(void);
MMP_USHORT MMPF_MP4VENC_GetVideoWidth(void);
MMP_USHORT MMPF_MP4VENC_GetVideoHeight(void);

// functions of H264 encoder
MMP_ERR MMPF_H264ENC_TriggerME(MMP_ULONG ulCurFrame, MMP_ULONG ulRefFrame);
MMP_ERR MMPF_H264ENC_SetMEEnable(MMP_BOOL bEnable);

void MMPF_MP4VENC_SetBitRate(MMP_ULONG ulEncId, MMP_ULONG updatets, MMP_ULONG updatebr);
void MMPF_MP4VENC_SetEncFrameRate(MMP_ULONG ulEncId, MMP_ULONG timeresol, MMP_ULONG timeincrement);
void MMPF_MP4VENC_SetSnrFrameRate(MMP_ULONG ulEncId, MMP_ULONG timeresol, MMP_ULONG timeincrement);
void MMPF_MP4VENC_SetVideoStoreBufInfo(MMP_ULONG bufaddr);
MMP_ERR MMPF_VIDENC_StopRecordForStorage (void);
MMP_ERR MMPF_VIDENC_CheckSeamless(MMPF_H264ENC_ENC_INFO *pEnc, MMP_UBYTE *ubSeamLess);

#if (NEW_BITRATE_CTL)
void MMPF_VidRateCtl_GetRcVersion(MMP_USHORT* RcMajorVersion, MMP_USHORT* RcMinorVersion);
MMP_LONG  MMPF_VidRateCtl_Get_VOP_QP(void* RCHandle, MMP_LONG vop_type, MMP_ULONG *target_size, MMP_ULONG *qp_delta, MMP_BOOL *bSkipFrame, MMPF_VIDENC_CMPBUF_STATUS BufLevel, MMP_ULONG ulMaxFrameSize);
MMP_ERR MMPF_VidRateCtl_ForceQP(void* RCHandle, MMP_LONG vop_type, MMP_ULONG QP);
MMP_ULONG MMPF_VidRateCtl_UpdateModel(void* RCHandle, MMP_LONG vop_type, MMP_ULONG CurSize, MMP_ULONG HeaderSize, MMP_ULONG last_QP, MMP_BOOL bForceSkip, MMP_BOOL *bSkipFrame, MMP_ULONG *pending_bytes);
MMP_ERR MMPF_VidRateCtl_Init(void* *handle, MMP_ULONG idx, MMP_USHORT gsVidRecdFormat, MMP_LONG targetsize, MMP_ULONG framerate, MMP_ULONG nP, MMP_ULONG nB, /*MMP_ULONG InitQP,*/ MMP_BOOL PreventBufOverflow, /*MMP_LONG is_vbr_mode,*/ RC_CONFIG_PARAM RcConfig);
MMP_ERR MMPF_VidRateCtl_DeInit(void* RCHandle, MMP_ULONG handle_idx, MMP_LONG target_framesize, MMP_ULONG bit_rate, RC_CONFIG_PARAM	RcConfig);
void MMPF_VidRateCtl_ResetBitrate(void* RCHandle, MMP_LONG bit_rate, MMP_ULONG framerate, MMP_BOOL ResetParams, MMP_ULONG ulVBVSize, MMP_BOOL bResetBufUsage);
void MMPF_VidRateCtl_SetQPBoundary(void* RCHandle, MMP_ULONG frame_type, MMP_LONG QP_LowerBound, MMP_LONG QP_UpperBound);
void MMPF_VidRateCtl_GetQPBoundary(void* RCHandle, MMP_ULONG frame_type, MMP_LONG *QP_LowerBound, MMP_LONG *QP_UpperBound);
void MMPF_VidRateCtl_ResetBufSize(void* RCHandle, MMP_LONG BufSize);
#else
void MMPF_MP4VENC_KeepSizeToScaleQueue(MMP_USHORT CurSize);
void MMPF_MP4VENC_InitScaleQueue(void);
#endif

/// @}

#endif	// _MMPF_MP4VENC_H_

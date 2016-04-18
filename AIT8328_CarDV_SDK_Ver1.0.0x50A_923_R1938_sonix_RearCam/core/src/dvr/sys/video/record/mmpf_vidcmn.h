/**
 @file mmpf_vidcmn.h
 @brief Header function of video driver related define
 @author
 @version 0.0
*/

#ifndef _MMPF_VIDCMN_H_
#define _MMPF_VIDCMN_H_

//==============================================================================
//
//                              COMPILER OPTION
//
//==============================================================================

#define H264ENC_I2MANY_EN                   (0)
#define H264ENC_HBR_EN                      (0)
#define H264ENC_TNR_EN                      (1) // CarDV
#define H264ENC_ICOMP_EN                    (0) // Support in MCR_V2 MP only

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

#define MMPF_VIDENC_MAX_QUEUE_SIZE          (4)

#define MMPF_VIDENC_MODULE_H264             (0)
#define MMPF_VIDENC_MODULE_MAX              (1)

// video format
#define MMPF_MP4VENC_FORMAT_OTHERS          0x00
#define MMPF_MP4VENC_FORMAT_H264            0x01
#define MMPF_MP4VENC_FORMAT_MJPEG           0x02

// video operation status
#define	MMPF_MP4VENC_FW_STATUS_RECORD       0x0000  ///< status of video encoder
#define	MMPF_MP4VENC_FW_STATUS_START        0x0001  ///< status of START
#define	MMPF_MP4VENC_FW_STATUS_PAUSE        0x0002  ///< status of PAUSE
#define	MMPF_MP4VENC_FW_STATUS_RESUME       0x0003  ///< status of RESUME
#define	MMPF_MP4VENC_FW_STATUS_STOP         0x0004  ///< status of STOP
#define	MMPF_MP4VENC_FW_STATUS_PREENCODE    0x0005  ///< status of PRE_ENCODE

#define MAX_VIDEO_STREAM_NUM				(2)
#if (SUPPORT_VR_WIFI_STREAM)
#if (SUPPORT_DUAL_SNR)
#define MAX_WIFI_STREAM_NUM					(2)
#else
#define MAX_WIFI_STREAM_NUM					(1)
#endif
#else
#define MAX_WIFI_STREAM_NUM					(0)
#endif
#define MAX_NUM_ENC_SET                     (MAX_VIDEO_STREAM_NUM + MAX_WIFI_STREAM_NUM)
#define MAX_NUM_TMP_LAYERS                  (2)
#define MAX_NUM_PARAM_CTL                   (16)

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================
typedef void VidEncEndCallBackFunc(void *);

typedef enum _MMPF_VIDENC_ATTRIBUTE {
    MMPF_VIDENC_ATTRIBUTE_PROFILE = 0,
    MMPF_VIDENC_ATTRIBUTE_FRM_QP,
    MMPF_VIDENC_ATTRIBUTE_BR,
    MMPF_VIDENC_ATTRIBUTE_CROPPING,
    MMPF_VIDENC_ATTRIBUTE_GOP_CTL,
    MMPF_VIDENC_ATTRIBUTE_MAX_FPS,
	MMPF_VIDENC_ATTRIBUTE_ENC_FPS,
	MMPF_VIDENC_ATTRIBUTE_SNR_FPS,
    MMPF_VIDENC_ATTRIBUTE_CURBUF_MODE,
    MMPF_VIDENC_ATTRIBUTE_RESOLUTION
} MMPF_VIDENC_ATTRIBUTE;

// video current buffer mode
typedef enum _MMPF_VIDENC_CURBUF_MODE {
	MMPF_MP4VENC_CURBUF_FRAME,
	MMPF_MP4VENC_CURBUF_RT,
	MMPF_MP4VENC_CURBUF_MAX
} MMPF_VIDENC_CURBUF_MODE;

typedef enum _MMPF_VIDENC_RC_MODE {
    MMPF_VIDENC_RC_MODE_CBR = 0,
    MMPF_VIDENC_RC_MODE_VBR,
    MMPF_VIDENC_RC_MODE_CQP,
    MMPF_VIDENC_RC_MODE_MAX
} MMPF_VIDENC_RC_MODE;

typedef enum _MMPF_DUALENC_QUEUEACT {
	MMPF_DEQ_GETDEPTH = 0,
	MMPF_DEQ_SHOWDATA,
	MMPF_DEQ_PUSHDATA,
	MMPF_DEQ_POPDATA
} MMPF_DUALENC_QUEUEACT; // dual enc queue process

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef struct _MMPF_VIDENC_RESOLUTION {
    MMP_USHORT  usWidth;
    MMP_USHORT  usHeight;
} MMPF_VIDENC_RESOLUTION;

typedef struct _MMPF_VIDENC_GOP_CTL {
    MMP_USHORT  usGopSize;
    MMP_USHORT  usContPFrameNum;
    MMP_USHORT  usMaxContBFrameNum;
} MMPF_VIDENC_GOP_CTL;

typedef struct _MMPF_VIDENC_CROPPING {
    MMP_USHORT  usTop;
    MMP_USHORT  usBottom;
    MMP_USHORT  usLeft;
    MMP_USHORT  usRight;
} MMPF_VIDENC_CROPPING;

typedef struct _MMPF_VIDENC_BITRATE_CTL {   ///< bitrate param control
	MMP_ULONG ulVidRecdFrameTargetSize; ///< target size, byte
    MMP_ULONG ulBitrate;                ///< bitrate, bits
} MMPF_VIDENC_BITRATE_CTL;

typedef struct _MMPF_VIDENC_QP_CTL {        ///< QP control, for initail QP and CQP
    MMP_UBYTE ubQP[3];
} MMPF_VIDENC_QP_CTL;

typedef struct _MMPF_VIDENC_QUEUE {
    MMP_ULONG   buffers[MMPF_VIDENC_MAX_QUEUE_SIZE];  ///< queue for buffer ready to encode, in display order
    MMP_ULONG   weight[MMPF_VIDENC_MAX_QUEUE_SIZE];   ///< the times to encode the same frame
    MMP_ULONG   head;
    MMP_ULONG   size;
    MMP_ULONG   weighted_size;
} MMPF_VIDENC_QUEUE;

#define DUALENC_QMAXSIZE 	(20)

typedef struct _MMPF_DUALENC_QUEUE {
    MMP_ULONG   buffers[DUALENC_QMAXSIZE];  ///< queue for buffer ready to encode, in display order
    MMP_ULONG   weight[DUALENC_QMAXSIZE];   ///< the times to encode the same frame
    MMP_ULONG   head;
    MMP_ULONG   size;
    MMP_ULONG   weighted_size;
} MMPF_DUALENC_QUEUE;

typedef struct _MMPF_VIDENC_FRAME{
    MMP_ULONG   ulYAddr;
    MMP_ULONG   ulUAddr;
    MMP_ULONG   ulVAddr;
    MMP_ULONG   ulTimestamp;
} MMPF_VIDENC_FRAME;

typedef struct _MMPF_VIDENC_FRAMEBUF_BD {
    MMPF_VIDENC_FRAME   LowBound;
    MMPF_VIDENC_FRAME   HighBound;
} MMPF_VIDENC_FRAMEBUF_BD;

typedef struct _MMPF_VIDENC_DUMMY_DROP_INFO {
	MMP_ULONG	ulDummyFrmCnt;           ///< specified how many video frames to be duplicated
	MMP_ULONG   ulDropFrmCnt;            ///< specified how many video frames to be dropped
	MMP_USHORT  usAccumSkipFrames;              ///< number of skip frames within 1 sec
	MMP_ULONG   ulBeginSkipTimeInMS;            ///< the absolute timer counter of the beginning skip frame within 1 sec.
} MMPF_VIDENC_DUMMY_DROP_INFO;

typedef struct _MMPF_VIDENC_PARAM_CTL {
    MMPF_VIDENC_ATTRIBUTE Attrib;
    void (*CallBack)(MMP_ERR);
    union {
        MMPF_VIDENC_BITRATE_CTL Bitrate;
        MMPF_VIDENC_RC_MODE     RcMode;
        MMPF_VIDENC_QP_CTL      Qp;
        MMPF_VIDENC_GOP_CTL     Gop;
    } Ctl;
} MMPF_VIDENC_PARAM_CTL;

#endif //_MMPF_VIDCMN_H_

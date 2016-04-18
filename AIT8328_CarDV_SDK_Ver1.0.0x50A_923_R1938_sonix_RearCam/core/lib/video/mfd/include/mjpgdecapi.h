/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2006 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Description : API of H.264 Decoder
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: MJPGdecapi.h,v $
--  $Date: 2010-09-01 15:13:17 +0800 (星期三, 01 九月 2010) $
--  $Revision: 892 $
--
------------------------------------------------------------------------------*/

#ifndef __MJPGDECAPI_H__
#define __MJPGDECAPI_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "basetype.h"
#include "decapicommon.h"

/*------------------------------------------------------------------------------
    API type definitions
------------------------------------------------------------------------------*/

    /* Return values */
    typedef enum MJPGDecRet_
    {
        MJPGDEC_OK = 0,
        MJPGDEC_STRM_PROCESSED = 1,
        MJPGDEC_PIC_RDY = 2,
        MJPGDEC_PIC_DECODED = 3,
        MJPGDEC_HDRS_RDY = 4,
        MJPGDEC_ADVANCED_TOOLS = 5,
        MJPGDEC_PENDING_FLUSH = 6,

        MJPGDEC_PARAM_ERROR = -1,
        MJPGDEC_STRM_ERROR = -2,
        MJPGDEC_NOT_INITIALIZED = -3,
        MJPGDEC_MEMFAIL = -4,
        MJPGDEC_INITFAIL = -5,
        MJPGDEC_HDRS_NOT_RDY = -6,
        MJPGDEC_STREAM_NOT_SUPPORTED = -8,

        MJPGDEC_HW_RESERVED = -254,
        MJPGDEC_HW_TIMEOUT = -255,
        MJPGDEC_HW_BUS_ERROR = -256,
        MJPGDEC_SYSTEM_ERROR = -257,
        MJPGDEC_DWL_ERROR = -258,

        MJPGDEC_EVALUATION_LIMIT_EXCEEDED = -999,
        MJPGDEC_FORMAT_NOT_SUPPORTED = -1000
    } MJPGDecRet;

    /* decoder  output picture format */
    typedef enum MJPGDecOutFormat_
    {
        MJPGDEC_SEMIPLANAR_YUV420 = 0x020001,
        MJPGDEC_TILED_YUV420 = 0x020002,
        MJPGDEC_YUV400 = 0x080000
    } MJPGDecOutFormat;

    /* Decoder instance */
    typedef const void *MJPGDecInst;

    /* Input structure */
    typedef struct MJPGDecInput_
    {
        u8 *pStream;         /* Pointer to the input */
        u32 streamBusAddress;   /* DMA bus address of the input stream */
        u32 dataLen;         /* Number of bytes to be decoded         */
        u32 picId;           /* Identifier for the picture to be decoded */
        //swyang
        u32 timestamp;
        u8  flag;
    } MJPGDecInput;

    /* Output structure */
    typedef struct MJPGDecOutput_
    {
        u8 *pStrmCurrPos;    /* Pointer to stream position where decoding ended */
        u32 strmCurrBusAddress; /* DMA bus address location where the decoding ended */
        u32 dataLeft;        /* how many bytes left undecoded */
        u32 RetVal;
    } MJPGDecOutput;

    /* cropping info */
    typedef struct MJPGCropParams_
    {
        u32 cropLeftOffset;
        u32 cropOutWidth;
        u32 cropTopOffset;
        u32 cropOutHeight;
    } MJPGCropParams;

    /* Output structure for MJPGDecNextPicture */
    typedef struct MJPGDecPicture_
    {
        u32 picWidth;        /* pixels width of the picture as stored in memory */
        u32 picHeight;       /* pixel height of the picture as stored in memory */
        MJPGCropParams cropParams;  /* cropping parameters */
        const u32 *pOutputPicture;  /* Pointer to the picture */
        u32 outputPictureBusAddress;    /* DMA bus address of the output picture buffer */
        u32 picId;           /* Identifier of the picture to be displayed */
        u32 isIdrPicture;    /* Indicates if picture is an IDR picture */
        u32 nbrOfErrMBs;     /* Number of concealed MB's in the picture  */
        u32 interlaced;      /* non-zero for interlaced picture */
        u32 fieldPicture;    /* non-zero if interlaced and only one field present */
        u32 topField;        /* if only one field, non-zero signals TOP field otherwise BOTTOM */
        //swyang
        u32 timestamp;
        u8  flag;
    } MJPGDecPicture;

    /* stream info filled by MJPGDecGetInfo */
    typedef struct MJPGDecInfo_
    {
        u32 picWidth;        /* decoded picture width in pixels */
        u32 picHeight;       /* decoded picture height in pixels */
        u32 videoRange;      /* samples' video range */
        u32 matrixCoefficients;
        MJPGCropParams cropParams;  /* display cropping information */
        MJPGDecOutFormat outputFormat;  /* format of the output picture */
        u32 sarWidth;        /* sample aspect ratio */
        u32 sarHeight;       /* sample aspect ratio */
        u32 monoChrome;      /* is sequence monochrome */
        u32 interlacedSequence;      /* is sequence interlaced */
        u32 picBuffSize;     /* number of picture buffers allocated&used by decoder */
        u32 multiBuffPpSize; /* number of picture buffers needed in decoder+postprocessor multibuffer mode */
    } MJPGDecInfo;

    /* Version information */
    typedef struct MJPGDecApiVersion_
    {
        u32 major;           /* API major version */
        u32 minor;           /* API minor version */
    } MJPGDecApiVersion;

    typedef struct MJPGDecBuild_
    {
        u32 swBuild;         /* Software build ID */
        u32 hwBuild;         /* Hardware build ID */
        DecHwConfig hwConfig;   /* hardware supported configuration */
    } MJPGDecBuild;

/*------------------------------------------------------------------------------
    Prototypes of Decoder API functions
------------------------------------------------------------------------------*/

    MJPGDecApiVersion MJPGDecGetAPIVersion(void);

    MJPGDecBuild MJPGDecGetBuild(void);

    MJPGDecRet MJPGDecInit(MJPGDecInst * pDecInst, u32 noOutputReordering,
                           u32 useVideoFreezeConcealment,
                           u32 useDisplaySmoothing,
                           //swyang
                           u8  bDisableTimeReorder,
                           u32 MaxRef
                           );

    void MJPGDecRelease(MJPGDecInst decInst);

    MJPGDecRet MJPGDecDecode(MJPGDecInst decInst,
                             const MJPGDecInput * pInput,
                             MJPGDecOutput * pOutput);

    MJPGDecRet MJPGDecNextPicture(MJPGDecInst decInst,
                                  MJPGDecPicture * pOutput, u32 endOfStream);

    MJPGDecRet MJPGDecGetInfo(MJPGDecInst decInst, MJPGDecInfo * pDecInfo);

    MJPGDecRet MJPGDecPeek(MJPGDecInst decInst, MJPGDecPicture * pOutput);

    //swyang
    MJPGDecRet MJPGDecGetFreeFrame(MJPGDecInst decInst, unsigned int *ulFrameAddr);
    
    void MJPGDecSetBackward(MJPGDecInst decInst, int IsBackward);
    
/*------------------------------------------------------------------------------
    Prototype of the API trace funtion. Traces all API entries and returns.
    This must be implemented by the application using the decoder API!
    Argument:
        string - trace message, a null terminated string
------------------------------------------------------------------------------*/
    void MJPGDecTrace(const char *string);

#ifdef __cplusplus
}
#endif

#endif                       /* __MJPGDECAPI_H__ */

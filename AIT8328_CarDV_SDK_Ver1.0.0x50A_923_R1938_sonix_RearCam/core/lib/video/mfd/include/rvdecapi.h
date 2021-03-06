/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2007 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Description : API for the 8190 RV Decoder
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: rvdecapi.h,v $
--  $Date: 2010-09-01 15:13:17 +0800 (星期三, 01 九月 2010) $
--  $Revision: 892 $
--
------------------------------------------------------------------------------*/

#ifndef __RVDECAPI_H__
#define __RVDECAPI_H__

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
    typedef enum
    {
        RVDEC_OK = 0,
        RVDEC_STRM_PROCESSED = 1,
        RVDEC_PIC_RDY = 2,
        RVDEC_HDRS_RDY = 3,
        RVDEC_HDRS_NOT_RDY = 4,
        RVDEC_PIC_DECODED = 5,

        RVDEC_PARAM_ERROR = -1,
        RVDEC_STRM_ERROR = -2,
        RVDEC_NOT_INITIALIZED = -3,
        RVDEC_MEMFAIL = -4,
        RVDEC_INITFAIL = -5,
        RVDEC_STREAM_NOT_SUPPORTED = -8,

        RVDEC_HW_RESERVED = -254,
        RVDEC_HW_TIMEOUT = -255,
        RVDEC_HW_BUS_ERROR = -256,
        RVDEC_SYSTEM_ERROR = -257,
        RVDEC_DWL_ERROR = -258,
        RVDEC_FORMAT_NOT_SUPPORTED = -1000
    } RvDecRet;

    /* decoder output picture format */
    typedef enum
    {
        RVDEC_SEMIPLANAR_YUV420 = 0x020001,
        RVDEC_TILED_YUV420 = 0x020002
    } RvDecOutFormat;

    typedef struct
    {
        u32 *pVirtualAddress;
        u32 busAddress;
    } RvDecLinearMem;

    /* Decoder instance */
    typedef void *RvDecInst;

    typedef struct
    {
        u32 offset;
        u32 isValid;
    } RvDecSliceInfo;

    /* Input structure */
    typedef struct
    {
        u8 *pStream;             /* Pointer to stream to be decoded              */
        u32 streamBusAddress;    /* DMA bus address of the input stream */
        u32 dataLen;             /* Number of bytes to be decoded                */
        u32 picId;
        u32 timestamp;       /* timestamp of current picture from rv frame header.
                              * NOTE: timestamp of a B-frame should be adjusted referring 
							  * to its forward reference frame's timestamp */

        u32 sliceInfoNum;    /* The number of slice offset entries. */ 
        RvDecSliceInfo *pSliceInfo;     /* Pointer to the sliceInfo.
                                         * It contains offset value of each slice 
                                         * in the data buffer, including start point "0" 
                                         * and end point "dataLen" */
        //swyang
        u8  flag;
    } RvDecInput;

    /* Time code */
    typedef struct
    {
        u32 hours;
        u32 minutes;
        u32 seconds;
        u32 pictures;
    } RvDecTime;

    typedef struct
    {
        RvDecLinearMem outputPicture;    /* Pointer and bus address to the
                                          * recent decoded picture decoder
                                          * output, YCbCr format  */
        u8 *pStrmCurrPos;
        u32 strmCurrBusAddress;          /* DMA bus address location where the decoding
                                          * ended */
        u32 dataLeft;
        u32 picId;              /* Identifier of the picture (in enc/dec order) */
        u32 picType;            /* picture coding type */
        u32 timestamp;          /* timestamp for media player */
        u32 picWidth;           /* Pixel width, up round */
        u32 picHeight;          /* Pixel height, up round */
        RvDecOutFormat outputFormat;    /* format of the output picture */	
    } RvDecOutput;

    /* stream info filled by RvDecGetInfo */
    typedef struct
    {
        u32 frameWidth;
        u32 frameHeight;
        u32 codedWidth;
        u32 codedHeight;
        u32 profileAndLevelIndication;
        u32 sampleAspectRatio;
        u32 DisplayAspectRatio;
        u32 streamFormat;
        u32 videoFormat;
        u32 videoRange;      /* ??? only [0-255] */
        u32 interlacedSequence;
        u32 multiBuffPpSize;
        RvDecOutFormat outputFormat;
    } RvDecInfo;

    typedef struct
    {
        u8 *pOutputPicture;
        u32 outputPictureBusAddress;
        u32 frameWidth;
        u32 frameHeight;
        u32 codedWidth;
        u32 codedHeight;
        u32 keyPicture;
        u32 bPicture;
        u32 picId;
        u32 interlaced;
        u32 fieldPicture;
        u32 topField;
        u32 firstField;
        u32 repeatFirstField;
        u32 repeatFrameCount;
        u32 numberOfErrMBs;
        RvDecTime timeCode;
        
        //swyang
        u8  flag;
    } RvDecPicture;

    /* Version information */
    typedef struct
    {
        u32 major;           /* API major version */
        u32 minor;           /* API minor version */

    } RvDecApiVersion;

    typedef struct
    {
        u32 swBuild;         /* Software build ID */
        u32 hwBuild;         /* Hardware build ID */
        DecHwConfig hwConfig;   /* hardware supported configuration */
    } RvDecBuild;

/*------------------------------------------------------------------------------
    Prototypes of Decoder API functions
------------------------------------------------------------------------------*/

    RvDecApiVersion RvDecGetAPIVersion(void);

    RvDecBuild RvDecGetBuild(void);

    RvDecRet RvDecInit(RvDecInst * pDecInst,
                             u32 useVideoFreezeConcealment,
                             u32 frameCodeLength,
                             u32 *frameSizes,
                             u32 rvVersion,
                             u32 maxFrameWidth, u32 maxFrameHeight,
                             u32 numFrameBuffers );

    RvDecRet RvDecDecode(RvDecInst decInst,
                               RvDecInput * pInput,
                               RvDecOutput * pOutput);

    RvDecRet RvDecGetInfo(RvDecInst decInst, RvDecInfo * pDecInfo);

    RvDecRet RvDecNextPicture(RvDecInst decInst,
                                    RvDecPicture * pPicture,
                                    u32 endOfStream);

    void RvDecRelease(RvDecInst decInst);

    RvDecRet RvDecPeek(RvDecInst decInst, RvDecPicture * pPicture);

/*------------------------------------------------------------------------------
    Prototype of the API trace funtion. Traces all API entries and returns.
    This must be implemented by the application using the decoder API!
    Argument:
        string - trace message, a null terminated string
------------------------------------------------------------------------------*/
    void RvDecTrace(const char *string);

#ifdef __cplusplus
}
#endif

#endif                       /* __RVDECAPI_H__ */

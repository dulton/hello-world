/*
   decoder main
 */

#ifndef AACDECODER_H
#define AACDECODER_H

#define  FRAME_SIZE 1024


#include "mmpf_audio_typedef.h"

//#include "stdio.h"
#include "aacplus_sbrdecoder.h"
#include "aacplus_FFR_bitbuffer.h"

#include "aacplus_streaminfo.h"
#include "aacplus_datastream.h"
#include "aacplus_channelinfo.h"
#include "aacplus_channel.h"
#include "aacplus_conceal.h"


typedef enum {
  AAC_DEC_OK = 0x0,
  AAC_DEC_UNSUPPORTED_FORMAT,
  AAC_DEC_DECODE_FRAME_ERROR,
  AAC_DEC_INVALID_CODE_BOOK,
  AAC_DEC_UNSUPPORTED_WINOW_SHAPE,
  AAC_DEC_LTP_NOT_SUPPORTED_IN_AAC_LC,
  AAC_DEC_UNIMPLEMENTED_CCE,
  AAC_DEC_UNIMPLEMENTED_PCE,
  AAC_DEC_UNIMPLEMENTED_LFE,
  AAC_DEC_UNIMPLEMENTED_GAIN_CONTROL_DATA,
  AAC_DEC_OVERWRITE_BITS_IN_INPUT_BUFFER,
  AAC_DEC_CANNOT_REACH_BUFFER_FULLNESS,
  AAC_DEC_TNS_RANGE_ERROR,
  AAC_DEC_TNS_ORDER_ERROR,
  AAC_DEC_UNSUPPORTED_CHANNEL_NUM
}
AAC_DEC_STATUS;

typedef enum
{
	AAC_MAIN = 1, // mpeg2 main + PNS
	AAC_LC = 2,   // mpeg2 LC + PNS
	AAC_SSR = 3,  // mpeg2 SSR + PNS
	AAC_LTP = 4,  // AAC_LC + LTP
	SBR = 5,
	PS = 29,
	UNKNOWN
}
AacObjectType;

struct AAC_DECODER_INSTANCE {
  Flag frameOK;
  Word32 bitCount;
  Word32 byteAlignBits;
  HANDLE_BIT_BUF pBs;
  SBRBITSTREAM *pStreamSbr;
  CStreamInfo *pStreamInfo;
  CDataStreamElement *pDataStreamElement;
  CAacDecoderChannelInfo *pAacDecoderChannelInfo[Channels];
  CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo[Channels];
  CConcealmentInfo *hConcealmentInfo;
};


typedef struct AAC_DECODER_INSTANCE *AACDECODER;



/* initialization of aac decoder */
AACDECODER AACPLUS_CAacDecoderOpen(HANDLE_BIT_BUF hBitBufCore,
                           SBRBITSTREAM *streamSbr,
                           Word32         samplingRate);


/* aac decoder */
Word16 AACPLUS_CAacDecoder_DecodeFrame(AACDECODER aacDecoderInstance,
                               Word16    *frameSize,
                               Word32    *sampleRate,
                               Word8     *channelMode,
                               Word16    *numChannels,
                               Word16    *timeData,
                               Flag       frameOK
);

/* Set aac decoder working buffer */
int AACPLUS_SetWorkingBuf(int *buf);

#endif /* #ifndef AACDECODER_H */

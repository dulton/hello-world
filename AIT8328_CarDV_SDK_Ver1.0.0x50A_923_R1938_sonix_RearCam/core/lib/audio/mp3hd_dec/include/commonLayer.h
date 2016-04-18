/*!
 * \file commonLayer.h
 * Layer comprehensive include file that declares functions and structures 
 * accessible in all layer. 
 * 
 * Copyright (c) 2008 by <Sven Kordon / Deutsche Thomson OHG (DTO)>
 */
/*
  Copyright (c) 2008, Deutsche Thomson OHG (DTO).
  site:    Corporate Research Hannover, Germany
  lab:     Digital Audio (HDA)
  project: Audio Coding
   
                   All Rights Reserved. Thomson Confidential.
   
  DTO considers this source code as an unpublished, proprietary knowledge secret.
  It can be available from the author inside the frame work of defined actual 
  projects. If not affected by special agreements (non-disclosure agreements, 
  intercompany cooperations or similar) it is strictly prohibited to use this 
  software or parts of it in another environment than the defined project work 
  or to distribute it to any other person without the prior written permission 
  of the author. 
   
  This software is provided "as is" and without warranties as to performance, 
  merchantibility, non-infringement or any other warranties whether expressed 
  or implied. Because of the various hardware and software environments into 
  which this software may be put, no warranty of fitness for a particular 
  purpose is offered. In no event shall DTO be liable for any damages, 
  including without limitation, direct, special, indirect, or consequential 
  damages arising out of, or relating to, use of this software by any customer 
  or any third party. DTO is under no obligation to provide support to customer. 
*/


#ifndef __COMMONLAYER_H_
#define __COMMONLAYER_H_

/* DEFINES *****************************************/
#include "mp3hddec_settings.h"
#include "mp3lossless.h"
#include "Mp3InfoFrame.h"

#ifdef __cplusplus
  extern "C" {
#endif

#define noOPTIMIZE_SEEKING 
    /** Note that with optimized seeking the getNumberOfDecodedSamples()
        function might return invalid values directly after the seekPositon call.
        Recall the get...Samples() function for each decode() function call 
        with decode number of samples equal to zero */

#define MP3HD_MAX_OUT_SAMPLES 1152
#define MP3HD_MAX_NUM_CHANNELS 2
#define MP3HD_MAX_BYTES_PER_SAMPLE 4
#define MP3HD_MAX_NUMBER_OF_LAYERS 5
#define MP3HD_MAX_MP3INFOFRAME_SIZE 2048
#define MAX_INIT_EOF_COUNT 2

#define MP3HD_MP3LS_INPUT_OFFSET 191
#define MP3HD_MP3_CODEC_DELAY 1201 /* 1201 := 144 + 481 + 576 (Blockswitching LA + Polyphase + Mdct) */
#define MP3HD_CDLAYER_DELAY 2113 /* 1152 - 191 + 1152 */
#define MP3HD_PRECEDING_ZEROS 336  /* 336 := 1152 - 1632/2; 1632 := (32*36-31)+512-1 (32B-FB + Mdct Impulse Responce) */
#ifdef OPTIMIZE_SEEKING
#define MP3HD_RESYNC_DELAY 5*576
#else
#define MP3HD_RESYNC_DELAY (6*1152+5*576)
#endif
/*!
 * \brief
 * Handle to the MP3HDLAYER structure defined in mp3hdDecoder.c
 * 
 * \remarks
 * Only the definition of the handle to the MP3HDLAYER structure.
 * 
 * \see
 * MP3HDLAYER
 */
typedef struct MP3HDLAYER *HMP3HDLAYER;

typedef struct BITBUFFER *HBITBUFFER;

typedef struct BITBUFFER *HBITSTREAM;

typedef struct FILEPROPERTIES *HFILEPROPERTIES;

typedef struct ID3V2Info *hID3V2Info;

typedef void*  MP3HD_LAYERDECODER;

/**
  \brief struct with modes from the mp3hdID
*/
struct MP3HDMODEBITS
{
  /* data from modeBits1 */
  char versionNumber;     /* 0..7 */
  char fileModeStreaming; /* 1: multiple mp3hdID, mp3Data, cdData chunks */ 
  /* char multiplemp3hdDataUsed; */
  char mp3IsScrambled;    /* 1: scrambling of the mp3 data is used */
  char hdIsUsed;          /* 1: hd data chunk is used */
  char srIsUsed;          /* 1: sr data chunk is used */

  /* data from modeBits2 */
  char exIsUsed;          /* 1: ex data chunk is used */
  char mp3PrelistenIsUsed;/* 1: mp3 prelistening frame number is present */ 
  char fileCrcIsUsed;     /* 1: CRC over mp3hd file is used */
  char includesFirstFrame;  /* 1: first super frame included in Super-Frame else 1 */
  char useCustomDelay;	    /* 0: Decoder uses default delay of 961 Samples else
								  Decoder uses CustomDelay of muteFrames and muteSamples, 
								  mandatory for includesFirstFrame = 0;*/
}; /* end of struct mp3hdMode */


/**
  \brief struct with data read from the mp3hdID
*/
struct MP3HDIDDATA
{
  /* data for the "empty" mp3 header */
  UINT  mp3hdIdSize;      /* total size of mp3hdID, dependent on mp3 bit rate and sampling rate */

  /* fixed-length data for the "mp3hd" tag */
  UCHAR mp3hdID[3]; /* "mp3hd" */
  UCHAR mp3hdId3TagCounter;
  UCHAR mp3hdIdCRC; /* CRC over the mp3hd tag */
  UCHAR mp3hdId3UnsyncTest[3]; /*0x00 FF 00 */
  UCHAR mp3hdModeBits1;
  UCHAR mp3hdModeBits2;
  UINT  mp3hdVerificationId;
  UCHAR mp3hdBytesPerSize;
  UINT  ofl_totallength;
  struct MP3HDMODEBITS mp3hdMode; /* decoded values of the mode bits */
  UINT mp3DataSize; /* number of bytes of mp3 data */
  UINT cdDataSize;  /* number of byte of cd data, formerly known as p3m data */
  
  /* variable-length (optional) data for the "mp3hd" tag */
  UINT hdDataSize; /* number of bytes of hd data, PCM-bits > 16, fs > 48 kHz */
  UINT srDataSize; /* number of bytes of surround (multichannel) data */
  UINT exDataSize; /* number of bytes of extended (e.g. post processing) layer data */
  UINT preListenStartFrame; /* mp3 frame number for start of prelistening part */
  UINT fileCrc; /* CRC over the mp3hd file without the mp3hd tag */
  USHORT muteFrames; /* Number of mp3 frames to skip (3 bit)*/
  USHORT muteSamples; /* Number of Samples to skip for sample exact previews (11 bit)*/
  UCHAR streamingCRC;    /* 8 bit CRC6 to identify previous super frame */

  /* more to come: hd, multichannel, post processing ... */
}; /* end of struct mp3hdIdData */

/*!
 * \brief
 * Structure that carries global file properties.
 */
typedef struct FILEPROPERTIES{
  pMp3InfoFrame pMp3hdInfoFrame;
  struct MP3HDIDDATA mp3hdIdData;
  UINT numChannel; /**< Number of audio channels */
  UINT sampleFrequency; /**< Sampling frequnecy of decoded audio signal*/
  UINT bitPerSample; /**< Resolution on bit of a decoded sample */
  UINT bytePerSample; /**< Resolution in byte of a decoded Sample */
  UINT mp3VbrHeaderDuration; /** mp3 chunk duration in samples read from the VBR header */
  UINT mp3VbrHeaderSize; /** mp3 chunk size in bytes read from the VBR header */
  UCHAR skipVbrFrame; /** the VBR header needs to be skipped */
}FILEPROPERTIES;

/*!
 * \brief
 * Structure that carries global properties of one mp3hd frame.
 * 
 */
typedef struct {
  UINT mp3Bitrate; /**< Bitrate of the mp3 layer */
  UINT mp3BlockSize[2]; 
  UINT zeroStartIndex[2];
  /*...*/
}FRAMEPROPERTIES;

/*!
 * \brief
 * layer Status.
 * 
 * Not really defined yet. 
 * 
 * \remarks
 * Need to be defined!!
 * 
 */
typedef enum
  { LAYER_CREATED,
    LAYER_OFF,
    LAYER_GETSYNC,
    LAYER_ON,
    LAYER_INIT
  }LAYER_STATUS;

/* INTERFACES *****************************************/
SSC getNewBuffer( UCHAR **ppOutBuffer, 
                  UINT *pOutBufferSize, 
                  UINT *pNumBytesUsed, 
                  INT skipBytes,
                  void *pLayer);

#ifdef __cplusplus
  }
#endif /* __cplusplus */
#endif /* __COMMONLAYER_H_ */

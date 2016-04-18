//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*
 * wmatypes.h:
 * Copyright (C) ARM Limited 1999. All Rights Reserved.
 */

//+-------------------------------------------------------------------------
//
//  Microsoft Windows Media
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       wmatypes.h
//
//--------------------------------------------------------------------------
#include "includes_fw.h"
#if (WMA_EN)
#ifndef WMATYPES_H
#define WMATYPES_H
#include "wma_def.h"
#include "mmpf_audio_typedef.h"

/****************************************************************************/
#if 0
#ifndef NULL
#ifdef  __cplusplus
#define NULL    0
#else  /* __cplusplus */
#define NULL    ((void *)0)
#endif /* __cplusplus */
#endif /* NULL */
#endif

/****************************************************************************/


/****************************************************************************/
#ifndef GUID_DEFINED
#define GUID_DEFINED

typedef struct tGUID
{
    DWORD   Data1;
    WORD    Data2;
    WORD    Data3;
    BYTE    Data4[ 8 ];

}   GUID;

#endif

/****************************************************************************/
#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_
typedef struct tWAVEFORMATEX
{
    WORD    wFormatTag;         /* format type */
    WORD    nChannels;          /* number of channels (i.e. mono, stereo...) */
    DWORD   nSamplesPerSec;     /* sample rate */
    DWORD   nAvgBytesPerSec;    /* for buffer estimation */
    WORD    nBlockAlign;        /* block size of data */
    WORD    wBitsPerSample;     /* number of bits per sample of mono data */
    WORD    cbSize;             /* the count in bytes of the size of */

}   WAVEFORMATEX;
typedef const WAVEFORMATEX *LPCWAVEFORMATEX;

#endif // _WAVEFORMATEX_

#ifndef _WAVEFORMATEXTENSIBLE_
#define _WAVEFORMATEXTENSIBLE_
typedef struct 
{
    WAVEFORMATEX Format;       /* The traditional wave file header */
    union {
        unsigned short wValidBitsPerSample; /* bits of precision */
        unsigned short wSamplesPerBlock;    /* valid if wBitsPerSample==0 */
        unsigned short wReserved;           /* If neither applies, set to zero */
    } Samples;
    unsigned long    dwChannelMask;        /* which channels are present in stream */
    GUID SubFormat;           /* specialization */
} WAVEFORMATEXTENSIBLE, *PWAVEFORMATEXTENSIBLE;

#endif /* _WAVEFORMATEXTENSIBLE_ */

#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM 1
#endif /* WAVE_FORMAT_PCM */

#ifndef WAVE_FORMAT_EXTENSIBLE
#define WAVE_FORMAT_EXTENSIBLE 65534
#endif /* WAVE_FORMAT_EXTENSIBLE */

#ifndef _SPEAKER_POSITIONS_
#define _SPEAKER_POSITIONS_
/* Speaker Positions for dwChannelMask in WAVEFORMATEXTENSIBLE: from ksmedia.h */
#define SPEAKER_FRONT_LEFT              0x1
#define SPEAKER_FRONT_RIGHT             0x2
#define SPEAKER_FRONT_CENTER            0x4
#define SPEAKER_LOW_FREQUENCY           0x8
#define SPEAKER_BACK_LEFT               0x10
#define SPEAKER_BACK_RIGHT              0x20
#define SPEAKER_FRONT_LEFT_OF_CENTER    0x40
#define SPEAKER_FRONT_RIGHT_OF_CENTER   0x80
#define SPEAKER_BACK_CENTER             0x100
#define SPEAKER_SIDE_LEFT               0x200
#define SPEAKER_SIDE_RIGHT              0x400
#define SPEAKER_TOP_CENTER              0x800
#define SPEAKER_TOP_FRONT_LEFT          0x1000
#define SPEAKER_TOP_FRONT_CENTER        0x2000
#define SPEAKER_TOP_FRONT_RIGHT         0x4000
#define SPEAKER_TOP_BACK_LEFT           0x8000
#define SPEAKER_TOP_BACK_CENTER         0x10000
#define SPEAKER_TOP_BACK_RIGHT          0x20000
#endif /* _SPEAKER_POSITIONS_ */

// get definitions for  MSAUDIO1WAVEFORMAT etc from official place.
#include "msaudiofmt.h"

/****************************************************************************/
typedef struct tAsfXAcmAudioErrorMaskingData
{
    BYTE    span;
    WORD    virtualPacketLen;
    WORD    virtualChunkLen;
    WORD    silenceLen;
    BYTE    silence[1];

}   AsfXAcmAudioErrorMaskingData;


/****************************************************************************/
typedef struct tAsfXSignatureAudioErrorMaskingData
{
    DWORD maxObjectSize;
    WORD  chunkLen;
    WORD  signatureLen;
    BYTE  signature[1];

}   AsfXSignatureAudioErrorMaskingData;


/****************************************************************************/
typedef struct tPACKET_PARSE_INFO_EX
{
    BOOL fParityPacket;
    DWORD cbParseOffset;

    BOOL fEccPresent;
    BYTE bECLen;

    BYTE bPacketLenType;
    DWORD cbPacketLenTypeOffset;

    BYTE bPadLenType;
    BYTE bSequenceLenType;

    BOOL fMultiPayloads;

    BYTE bOffsetBytes;

    BYTE bOffsetLenType;

    DWORD cbPacketLenOffset;

    DWORD cbExplicitPacketLength; 

    DWORD cbSequenceOffset;
    
    DWORD dwSequenceNum; 

    DWORD cbPadLenOffset;
    DWORD cbPadding;

    DWORD dwSCR;
    WORD wDuration;

    BYTE bPayLenType;
    BYTE bPayBytes;
    DWORD cbPayLenTypeOffset;

    DWORD cPayloads;

}   PACKET_PARSE_INFO_EX;


/****************************************************************************/
typedef struct tPAYLOAD_MAP_ENTRY_EX
{
    WORD cbPacketOffset;
    WORD cbTotalSize;

    BYTE bStreamId;
    BYTE bObjectId;

    DWORD cbObjectOffset;
    DWORD cbObjectSize;
    DWORD msObjectPres;

    BYTE cbRepData;
    BYTE bJunk;
    WORD cbPayloadSize;
 // Added by Amit
    
//    BYTE bIsKeyFrame;
//    BYTE bFrameRate;
    WORD wBytesRead;
    WORD wTotalDataBytes;
    BYTE bSubPayloadState;
    BYTE bIsCompressedPayload;
    BYTE bNextSubPayloadSize;
    WORD wSubpayloadLeft;
    DWORD dwDeltaTime;
    WORD  wSubCount;    

}   PAYLOAD_MAP_ENTRY_EX;

typedef enum {MAS_ALLOCATE, MAS_LOCKED, MAS_DELETE} MEMALLOCSTATE;


#endif  /* WMATYPES_H */
#endif

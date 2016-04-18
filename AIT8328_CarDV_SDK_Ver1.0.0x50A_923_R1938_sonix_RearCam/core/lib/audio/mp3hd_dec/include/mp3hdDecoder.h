/*!
 * \file mp3HdDecoder.h
 * Declaration of the mp3HdDec_decoder user interface. The declared parameters and 
 * functions are visible to the user and offer the interface to the mp3hd decoder lib.
 * 
 * Copyright (c) 2005 by <Sven Kordon/ Deutsche Thomson OHG (DTO)>
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

#ifndef __MP3HDDECODER_H_
#define __MP3HDDECODER_H_

#include "mp3hddec_settings.h"
#include "commonMp3HD.h"
#include "genericStds.h"

#ifdef WORKLOAD_MEASUREMENT
  #include "CDK_workload.h"
#endif

#ifdef __cplusplus
  extern "C" {
#endif
/* DEFINES *****************************************/

    /*
 * calling convention
 */
#ifndef MP3HDDECAPI
#ifdef WIN32
#define MP3HDDECAPI /*__stdcall*/
#else
#define MP3HDDECAPI
#endif
#endif


/*!
 * \brief
 * The mp3hd decoder handle created by the user.
 * Points to the private structure MP3HD.
 * 
 * \see
 * MP3HD
 */
typedef struct MP3HD_DECODER *MP3HD_DECODER_HANDLE;


/*!
 * \brief
 * The MP3HD_DECODER_INFO structure is filled by the function called 
 * mp3HdDec_getMp3hdDecoderInfo and includes information 
 * on the mp3HD bit stream.
 * 
 * \remarks
 * To get the MP3HD_DECODER_INFO from the mp3HdDec_getMp3hdDecoderInfo
 * function the mp3HdDec_initDone function has to return a true value. 
 * That means the mp3HD decoder needs to be in the decoding state. 
 * 
 */
typedef struct
{
  UCHAR mp3Type; /**< 0: off \n 1: MPEG-1 III \n 2: MPEG-2 III\n 3: MPEG-2.5 III */
  UCHAR cdLayerExists; /**< 0: not in the file \n 1: in the file*/
  UCHAR hdLayerExists; /**< 0: not in the file \n 1: in the file*/
  UCHAR srLayerExists; /**< 0: not in the file \n 1: in the file*/
  UCHAR exLayerExists; /**< 0: not in the file \n 1: in the file*/
  
  UCHAR mp3LayerState; /**< mp3 Layer States: \n 0: doesn't exist \n 1: created \n 2: off \n 3: on */
  UCHAR cdLayerState; /**< cd Layer States: \n 0: doesn't exist \n 1: created \n 2: off \n 3: on */
  UCHAR hdLayerState; /**< hd Layer States: \n 0: doesn't exist \n 1: created \n 2: off \n 3: on */
  UCHAR srLayerState; /**< sr Layer States: \n 0: doesn't exist \n 1: created \n 2: off \n 3: on */
  UCHAR exLayerState; /**< ex Layer States: \n 0: doesn't exist \n 1: created \n 2: off \n 3: on */
  /* Other file information */
  UINT mp3Bitrate; /**< average mp3 bit rate in bit/s */
  UINT totalBitrate; /**< average mp3HD bit rate bit/s */
  UINT numChannels; /**< number of channles of the output signal */
  UINT sampleRate; /**< sampling rate of the output signal in Hz */
  USHORT bitDepth; /**< bit depth of each channel in bit */
}MP3HD_DECODER_INFO;


/*!
 * \brief
 * The MP3HD_DECODER_LIBINFO structure contains build information
 * of the mp3HD Decoder Lib. Call the mp3HdDec_getLibInfo() 
 * function to fill the MP3HD_DECODER_LIBINFO structure
 * 
 */
typedef struct
{
  char  buildDate[20]; /**< The Lib build date mmm dd yyyy */
  char  versionNo[20]; /**< The Lib version number */
  char  svnRevision[256]; /**< The Lib SVN revision number */
}MP3HD_DECODER_LIBINFO;

/* INTERFACES *****************************************/

/*!
 * \brief
 * Initialization of the mp3hd decoder.
 *
 * This function needs to be called to get a vaild MP3HD_DECODER_HANDLE. 
 * 
 * @param[out] pMp3hd
 * A pointer to a NULL initialized mp3HD Decoder Handle.
 * 
 * @param[in] pUserData
 * A void pointer to user defined data that is required to open the 
 * extension layers in the openExtraLayers callback function. 
 * 
 * @param[in] refillMp3hdBuffer
 * A function pointer to the refillMp3hdBuffer callback function. 
 * 
 * @param[in] openExtraLayers
 * A function pointer to the openExtraLayers callback function. 
 * 
 * @param[in] closeExtraLayers
 * A function pointer to the closeExtraLayers callback function. 
 * 
 * \returns
 * Error codes: SSC_OK or an SSC code.
 * 
 */
SSC MP3HDDECAPI mp3HdDec_init 
  ( MP3HD_DECODER_HANDLE	*pMp3hd,
	  void *pUserData,
    LP_REFILLEXLAYER refillMp3hdBuffer,
	  LP_OPENEXLAYER openExtraLayers,
 	  LP_CLOSEEXLAYER closeExtraLayers
  );

/*!
 * \brief
 * Check if mp3HD Decoder is initialized
 * 
 * \param Mp3hd
 * A valid mp3HD decoder handle.
 * 
 * \returns
 * 0: not initialized 1: initialized
 *
 * \remarks
 * This functions is used to identify the current state of the mp3HdDec_decode function.
 * The function has to return one to get inforamtion about the mp3HD bit 
 * stream and to decode samples from the bit stream.  
 * 
 */
UCHAR MP3HDDECAPI mp3HdDec_initDone(MP3HD_DECODER_HANDLE	Mp3hd);

/*!
 * \brief
 * The mp3HdDec_decode function initializes the bit stream and decodes samples from the stream.
 *
 * This function operates in two states. \n
 * The first state is for the initialization of the mp3HD bit stream. The initialization state
 * parses for all available layers and sets the main bit stream parameters like 
 * number of channels, sampling frequency and bit depth. Depending on the bit 
 * stream the function needs to be called several times to finish the initialization state. \n
 * The decoding state is reached if the mp3HdDec_initDone function returns one. 
 * In the decoding state the samples of the bitstream are decoded.   
 * 
 * @param[in] Mp3hd
 * A valid mp3HD Decoder Handle.
 * 
 * @param[in] pInBuffer
 * A pointer to the encoded mp3 data buffer.
 * 
 * @param[in] InBytes
 * Number of encoded bytes in the buffer.
 * 
 * @param[out] pInBytesUsed
 * Number of bytes that have been used by the decoder.
 * 
 * @param[out] pOutBuffer
 * Pointer to the interleaved decoded audio samples or\n
 * 
 * @param[in] OutBufferSize
 * Size of pOutBuffer in samples (including all channels).
 * 
 * @param[out] pOutBufferSamples
 * Number of decoded Samples.
 * 
 * @param[out] pSkipBytes
 * Number of bytes to skip before filling the next buffer.
 * pSkipBytes can be negative as well.
 * 
 * \returns
 * Error codes: SSC_OK or otherwise an SSC code.
 * 
 * \remarks
 * pSkipBytes is used to jump within the mp3 chunk. Always seek pSkipBytes from 
 * the current position before refilling the buffer pInBuffer. \n
 * This function calls the callback function openExtraLayers to open an new extension layer. 
 * The openExtraLayers function has to return SSC_I_CD_SKIPFRAME to omit a layer  \n
 * This function calls the callback function refillMp3hdBuffer to request 
 * new data from the bitstream. \n
 * The parameter nSkipBytes of the refillMp3hdBuffer callback function corresponds 
 * to the number of bytes to skip from the last read byte in an successive chunk 
 * of data before refilling the buffer. \n
 * 
 */
#ifndef AIT_RELEASE_MP3HD_LIB
SSC MP3HDDECAPI mp3HdDec_decode 
  (	MP3HD_DECODER_HANDLE	Mp3hd,
	  const UCHAR* const	pInBuffer,
	  UINT	InBytes,
	  UINT	*pInBytesUsed,
	  INT	*pOutBuffer,
	  UINT	OutBufferSize,
 	  UINT	*pOutBufferSamples,
    INT *pSkipBytes
 	  /*?legacy mp3Decode Parameter for Surrount etc.*/
  );
#else
SSC MP3HDDECAPI mp3HdDec_decode 
  (	MP3HD_DECODER_HANDLE	Mp3hd,
	  const UCHAR* const	pInBuffer,
	  UINT	InBytes,
	  UINT	*pInBytesUsed,
	  SHORT	*pOutBuffer,
	  UINT	OutBufferSize,
 	  UINT	*pOutBufferSamples,
    INT *pSkipBytes
 	  /*?legacy mp3Decode Parameter for Surrount etc.*/
  );
#endif

/*!
 * \brief
 * The mp3HdDec_seekToPositionInPercent function is for seeking to 
 * a position within the decoded signal in percent.
 * 
 * @param[in] Mp3hd
 * A vaild mp3HD Decoder Handle
 * 
 * @param[in] posInPercent
 * Seek position in percent
 *
 * @param[in] pInBuffer
 * mp3HD input buffer the same as used in the decode function. 
 *
 * @param[in] InBytes
 * Size of the pInBuffer in bytes
 * 
 * @param[out] pInBytesUsed
 * Number of bytes used from the pInBuffer
 * 
 * @param[in,out] pSkipBytes
 * Number of bytes to skip before refilling the next mp3HD input buffer.\n
 * Use the same variable as used in the decode function.
 * 
 * \returns
 * SSC error codes.
 * 
 * \remarks
 * Never perform this function in parallel to the decode function. \n
 * To call this function the pSkipBytes value has to be set to zero by 
 * the mp3hdDec_decode function. \n 
 * The mp3HD decoder needs to be initialized before calling this function.
 * 
 */
SSC MP3HDDECAPI mp3HdDec_seekToPositionInPercent(MP3HD_DECODER_HANDLE	Mp3hd, UINT posInPercent, 
                            UCHAR* const pInBuffer,
                            UINT	InBytes,
	                          UINT	*pInBytesUsed,
                            INT  *pSkipBytes);

/*!
 * \brief
 * Frees the memory allocated for the mp3hd decoder.
 * 
 * @param[in] pMp3hd
 * A valid valid mp3HD Decoder Handle.
 * 
 * \returns
 * SSC error code.
 * 
 */
SSC MP3HDDECAPI mp3HdDec_free( MP3HD_DECODER_HANDLE	hMp3hd );

/********************************************************************************************
 *        Helper function for file handling 
 *
 */

/*!
 * \brief
 * The function call of mp3HdDec_setEof indicates that the 
 * end of the input file has been reached.
 * The decodeMp3hd function won't get new bytes until now.
 * 
 * @param[in] Mp3hd
 * A valid mp3HD Decoder Handle.
 * 
 */
void MP3HDDECAPI mp3HdDec_setEof (MP3HD_DECODER_HANDLE	Mp3hd);

/*!
 * \brief
 * The function mp3HdDec_isEof checks if the decoder has reached the end of the file.
 * (All samples are decoded )
 * 
 * @param[in] Mp3hd
 * A valid mp3HD Decoder Handle.
 * 
 * \returns
 * True: All samples are decoded.\n
 * Flase: Samples remain.
 * 
 */
INT MP3HDDECAPI mp3HdDec_isEof (MP3HD_DECODER_HANDLE	Mp3hd);

/********************************************************************************************
 *        set information function
 *
 */

/*!
 * \brief
 * The function mp3HdDec_setFileSize sets the super frame 
 * (file) size for initialization of standard mp3 file.
 * 
 * @param[in] Mp3hd
 * A valid mp3HD Decoder Handle
 * 
 * @param[in] size
 * Super frame (file) size in bytes
 * 
 * \returns
 * SSC error code
 * 
 * \remarks
 * Call this function after the init function and before the first call of the decode function.
 * This functions is required for seeking in standard mp3 files. 
 * 
 */
SSC MP3HDDECAPI mp3HdDec_setFileSize(MP3HD_DECODER_HANDLE	Mp3hd, UINT size);

/********************************************************************************************
 *        get information function
 *
 */

/*!
 * \brief
 * The mp3HdDec_getSamplerate function returns 
 * the sample rate of the decoded file in Hz.
 * 
 * @param[in] Mp3hd
 * A valid mp3HD Decoder Handle
 * 
 * \returns
 * Sample rate in Hz
 *
 * \remarks
 * The mp3HD decoder needs to be in the decoding state for calling.
 *
 */
UINT MP3HDDECAPI mp3HdDec_getSamplerate(MP3HD_DECODER_HANDLE	Mp3hd);

/*!
 * \brief
 * The mp3HdDec_getNumChannels function returns 
 * the number of channels of the signal to decode.
 * 
 * @param[in] Mp3hd
 * A valid mp3HD Decoder Handle
 * 
 * \returns
 * Number of channels
 *
 * \remarks
 * The mp3HD decoder needs to be in the decoding state for calling.
 *
 */
UINT MP3HDDECAPI mp3HdDec_getNumChannels(MP3HD_DECODER_HANDLE	Mp3hd);

/*!
 * \brief
 * The mp3HdDec_getNumberOfDecodedSamples function returns the number of 
 * already decoded samples per channel.
 * 
 * @param[in] Mp3hd
 * A valid mp3HD Decoder Handle
 * 
 * \returns
 * Already decoded samples per channel
 *
 * \remarks
 * The mp3HD decoder needs to be in the decoding state for calling.
 *
 */
UINT MP3HDDECAPI mp3HdDec_getNumberOfDecodedSamples(MP3HD_DECODER_HANDLE	Mp3hd);

/*!
 * \brief
 * The mp3HdDec_getSuperFrameLengthInSamples function returns 
 * the number of samples per channel coded in the current super frame 
 * 
 * @param[in] Mp3hd
 * A valid mp3HD Decoder Handle
 * 
 * \returns
 * Samples per channel of super frame
 *
 * \remarks
 * The mp3HD decoder needs to be in the decoding state for calling.
 *
 */
UINT MP3HDDECAPI mp3HdDec_getSuperFrameLengthInSamples(MP3HD_DECODER_HANDLE	Mp3hd);

/*!
 * \brief
 * The mp3HdDec_getFrameNumber returns the number of already
 * decoded frames.
 * 
 * @param[in] Mp3hd
 * A valid mp3HD Decoder Handle
 * 
 * \returns
 * Returns the current frame number. \n
 * The mp3HD decoder needs to be in the decoding state for calling.
 * 
 */
UINT MP3HDDECAPI mp3HdDec_getFrameNumber(MP3HD_DECODER_HANDLE	Mp3hd);

/*!
 * \brief
 * The mp3HdDec_getInfoComment function returns 
 * the mp3HD info frame comment string. This string is 
 * stored in the first mp3 frame called mp3HD mp3 info frame.
 * 
 * @param[in] Mp3hd
 * A valid mp3HD Decoder Handle
 * 
 * @param[out] ppComment
 * A pointer to a NULL initialized array. \n
 * The required memory will be allocated 
 * and the string will be copied into the allocated array.\n
 * 
 * @param[out] pCommentSize
 * A pointer to an allocated integer value.\n
 * The value of the pointer will be set to the size in byte
 * of the allocated output array ppComment. 
 * 
 * \returns
 * SSC code
 *
 * \remarks
 * The mp3HD decoder needs to be in the decoding state for calling.
 * 
 */
SSC MP3HDDECAPI mp3HdDec_getInfoComment(MP3HD_DECODER_HANDLE	Mp3hd, 
                        char **ppComment, 
                        UINT *pCommentSize);

/*!
 * \brief
 * The function mp3HdDec_getCurrentBitrate Greturns 
 * the bit rate in bit/s of the last decoded Frame.
 * 
 * @param[in] Mp3hd
 * A valid mp3HD Decoder Handle
 * 
 * @param[out] pBitRate
 * A pointer to a allocated integer value. 
 * The value of the pointer will be set to 
 * the bit rate of the last decode frame.
 * 
 * \returns
 * SSC code
 *
 * \remarks
 * The mp3HD decoder needs to be in the decoding state for calling.
 * 
 */
SSC MP3HDDECAPI mp3HdDec_getCurrentBitrate(MP3HD_DECODER_HANDLE	Mp3hd, 
                      UINT *pBitRate);

/*!
 * \brief
 * The mp3HdDec_switchLayerState function switches 
 * the layer state from on to off and vice versa. 
 * Use this function to disable and enable an mp3HD layer even
 * during decoding. 
 * 
 * @param[in] Mp3hd
 * A valid mp3HD Decoder Handle
 * 
 * @param[in] layer
 * The layer type you like to switch.
 * 
 * \returns
 * SSC code
 *
 * \remarks
 * The mp3 layer can't be switched. 
 * The mp3HD decoder needs to be in the decoding state for calling.
 * 
 */
SSC MP3HDDECAPI mp3HdDec_switchLayerState(MP3HD_DECODER_HANDLE	Mp3hd,
                                       MP3HDLAYERTYPE layer);

/*!
 * \brief
 * The mp3HdDec_getMp3hdDecoderInfo function fills the MP3HD_DECODER_INFO structure.
 * 
 * @param[in] Mp3hd
 * A valid mp3HD Decoder Handle
 * 
 * @param[out] pMp3hdInfo
 * A pointer to a valid MP3HD_DECODER_INFO structure.
 * 
 * \returns
 * SSC code
 *
 * \remarks
 * The mp3HD decoder needs to be in the decoding state for calling.
 * 
 */
SSC MP3HDDECAPI mp3HdDec_getMp3hdDecoderInfo(MP3HD_DECODER_HANDLE	Mp3hd,
                                 MP3HD_DECODER_INFO *pMp3hdInfo);

/*!
 * \brief
 * The mp3HdDec_getLibInfo() function fills the MP3HD_DECODER_LIBINFO
 * structure with the build information of the mp3HD Decoder Lib.
 * 
 * \param pLibInfo
 * A pointer to a valid MP3HD_DECODER_LIBINFO structure.  
 * 
 * \returns
 * SSC_OK or SSC_E_INVALIDHANDLE if the MP3HD_DECODER_LIBINFO pointer is NULL
 *
 */
SSC MP3HDDECAPI mp3HdDec_getLibInfo(MP3HD_DECODER_LIBINFO *pLibInfo);

#ifdef WORKLOAD_MEASUREMENT
CDK_WRKLOAD **mp3HdDec_getWorkloadArray(MP3HD_DECODER_HANDLE Mp3hd);
#endif



/*******************************************************************************
 Definition of hidden functions                                                *
 these functions are not part of the standard mp3HD Decoder SDK interface      *
********************************************************************************/

/*!
 * \brief
 * Hidden mp3HD Decoder interface function for internal use only. 
 * This function returns the mp3HD ID3v2 frame ID. 
 * 
 * \param hMp3Hd
 * A valid mp3HD Decoder handle
 * 
 * \param frameId
 * An allocated y byte character array as output buffer
 * 
 * \returns
 * SSC error code
 * 
 */
SSC MP3HDDECAPI mp3HdDecHidden_getId3FrameId(MP3HD_DECODER_HANDLE hMp3Hd, char frameId[4] );

#ifdef __cplusplus
  }
#endif /* __cplusplus */
#endif /* __MP3HDDECODER_H_ */

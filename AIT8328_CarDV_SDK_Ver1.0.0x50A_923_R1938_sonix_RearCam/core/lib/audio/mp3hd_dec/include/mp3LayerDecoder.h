/*!
 * \file mp3LayerDecoder.h
 * Interface Declarations for the mp3 layer decoder.\n
 * The declared functions encapsulate the 
 * communication between the mp3hdDecoder.c
 * interface functions and the FHG mp3 library. 
 * 
 * Copyright (c) 2008 by <Sven Kordon / Deutsche Thomson OHG (DTO) >
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


#ifndef __MP3LAYERDECODER_H_
#define __MP3LAYERDECODER_H_

#include "mp3hddec_settings.h"
#include "mp3decifc.h"
#include "commonLayer.h"

#ifdef __cplusplus
  extern "C" {
#endif
/* DEFINES *****************************************/

/* INTERFACES *****************************************/

/*!
 * \brief
 * Allocates Memory and Initializes the mp3 layer.
 * 
 * @param[in] pThisLayer
 * Pointer to HMP3LAYER handle.\n 
 * If the pointer value is equal to NULL memory will be allocated\n
 * otherwise the existing MP3LAYER structure is re-initialized.   
 * 
 * @param[in] pFileProperties
 * Pointer to an existing FILEPROPERTIES structure.
 * 
 * @param[in] pFrameProperties
 * Pointer to an existing FRAMEPROPERTIES structure.
 * 
 * @param[in] pMp3LosslessData
 * Pointer to an existing MP3_LOSSLESS_DATA structure.
 * 
 * \returns
 * Error codes: 0 is OK otherwise an error occured.
 * 
 */
SSC mp3LayerDecoder_init(MP3HD_LAYERDECODER *pThisLayer, 
                         FILEPROPERTIES *pFileProperties,  
                         FRAMEPROPERTIES *pFrameProperties,
                         MP3_LOSSLESS_DATA *pMp3LosslessData);

/*!
 * \brief
 * Function for the evaluation of the mp3 Info Frame used in mp3hd decoder to get the file properties.
 * 
 * @param[in] thisLayer
 * A valid HMP3LAYER handle. 
 * 
 * @param[in] pMp3InfoFrameBuffer
 * Pointer to a buffer that contains the complete mp3 Info Frame.
 *
 * @param[in] bufferSize
 * Size in bytes of the pMp3InfoFrameBuffer.
 * 
 * @param[in] mp3ChunkSize
 * estimated Size in bytes of the mp3 chunk.
 *
 * @param[out] pIsScrambled
 * A scrambled MPEG-1 Layer III stream was found.
 *
 * \returns
 * Error codes: 0 is OK otherwise an error occured.
 * 
 */
 SSC mp3LayerDecoder_readInfoFrame(MP3HD_LAYERDECODER thisLayer, 
                                   UCHAR* pMp3InfoFrameBuffer, 
                                   UINT bufferSize, 
                                   UINT mp3ChunkSize,
                                   UCHAR *pIsScrambled);

/*!
 * \brief
 * Decodes one mp3 Frame to time or frequency domain.
 * 
 * @param[in] thisLayer
 * A valid HMP3LAYER handle. 
 * 
 * @param[in] pInBuffer
 * A pointer to the mp3 encoded data buffer.
 * 
 * @param[in] InBytes
 * Number of encoded bytes in the buffer.
 * 
 * @param[out] pInBytesUsed
 * Number of bytes that have been used by the decoder.
 * 
 * @param[out] pOutBuffer
 * Pointer to the interleaved decoded audio samples or\n
 * Pointer to the decoded spectra of two granules. 
 * 
 * @param[in] OutBufferSize
 * Size of pOutBuffer in samples (including all channels).
 * 
 * @param[out] pOutBufferSamples
 * Number of decoded Samples.
 * 
 * @param[in] getTimeSamples
 * 0: decode only spectra\n
 * 1: use synthesis filter bank to create time domain signal
 * 
 * \returns
 * Error codes: 0 is OK otherwise an error occured.
 * 
 */
#ifndef AIT_RELEASE_MP3HD_LIB
SSC mp3LayerDecoder_decode(MP3HD_LAYERDECODER thisLayer, 
                           const UCHAR* const	pInBuffer,
	                         UINT	InBytes,
	                         UINT	*pInBytesUsed,
	                         INT	*pOutBuffer,
	                         UINT	OutBufferSize,
 	                         UINT	*pOutBufferSamples,
                           UCHAR getTimeSamples);
#else
SSC mp3LayerDecoder_decode(MP3HD_LAYERDECODER thisLayer, 
                           const UCHAR* const	pInBuffer,
	                         UINT	InBytes,
	                         UINT	*pInBytesUsed,
	                         SHORT	*pOutBuffer,
	                         UINT	OutBufferSize,
 	                         UINT	*pOutBufferSamples,
                           UCHAR getTimeSamples);
#endif

/*
 * indicate EOF (end-of-file) to the mp3 decoder.
 */
SSC mp3LayerDecoder_setInputEof(MP3HD_LAYERDECODER thisLayer);

/*
 * check if the mp3 decoder reached EOF.
 */
INT mp3LayerDecoder_isEof(MP3HD_LAYERDECODER thisLayer);

/*
 * reset mp3 decoder 
 */
SSC mp3LayerDecoder_reset(MP3HD_LAYERDECODER thisLayer);

/*
 * goto seek position in percent 
 */
UINT mp3LayerDecoder_seekInPercent(MP3HD_LAYERDECODER thisLayer, UINT posInPercent);

  /*!
	 * \brief
	 * get the mp3 frame size. buffer has to start with mp3 sync word.
	 * 
	 * \param pIn
	 * input buffer, first byte is beginning of mp3 header
	 * 
	 * \param nInSize
	 * Size of input buffer in bytes
	 * 
	 * \param pFrameSize 
	 * Output value equal to frame size.
	 * 
	 * \returns
	 * SSC_OK or SSC_E_CD_CANTREADMP3HEADER
	 * 
	 */
SSC mp3LayerDecoder_getMp3FrameSize(const char *pIn, UINT nInSize, UINT *pFrameSize);
/* INT mp3LayerDecoder_GetLosslessData(MP3DEC_HANDLE hMp3Decoder, MP3_LOSSLESS_DATA* pLossLess); */

/*!
 * \brief
 * Frees the memory allocated for the MP3LAYER structure.
 * 
 * @param[out] pThisLayer
 * Pointer to a valid HMP3LAYER handle. 
 * 
 * \returns
 * Error codes: 0 is OK otherwise an error occured.
 * 
 * \throws <exception class>
 * Description of criteria for throwing this exception.
 * 
 */
SSC mp3LayerDecoder_free(MP3HD_LAYERDECODER *pThisLayer);

#ifdef __cplusplus
  }
#endif /* __cplusplus */
#endif /* __MP3LAYERDECODER_H_ */

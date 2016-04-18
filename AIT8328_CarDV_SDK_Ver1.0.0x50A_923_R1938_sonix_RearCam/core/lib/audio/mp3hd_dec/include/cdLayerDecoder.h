/*!
 * \file cdLayerDecoder.h
 * Interface Declarations for the extension layer decoder.\n
 * The declared functions encapsulate the 
 * communication between the mp3hdDecoder.c
 * interface functions and one extension layer decoder using variable length coding. 
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


#ifndef __LAYERDECODER_H_
#define __LAYERDECODER_H_

#include "mp3hddec_settings.h"
#include "commonLayer.h"

#ifdef __cplusplus
  extern "C" {
#endif
/* DEFINES *****************************************/

#define MP3_GRAN_LEN 576

/* INTERFACES *****************************************/

/*!
 * \brief
 * Allocates Memory and Initializes the extension layer.
 * 
 * @param[in, out] pThisLayer
 * Pointer to MP3HD_LAYERDECODER handle.\n 
 * If the pointer value is equal to NULL memory will be allocated\n
 * otherwise the existing MP3LAYER structure will be re-initialized.  
 * 
 * @param[in] hMp3hdLayer
 * Handle to a valid HMP3HDLAYER structure used to track the bitstream position.
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
 * @param[in] pBitStream
 * A valid pointer to the pBitStream structure.
 *
 * @param[out] pHeaderSize
 * Will be set to the size of the layer header
 *
 * \returns
 * Error codes: 0 is OK otherwise an error occured.
 * 
 */
SSC cdLayerDecoder_init(MP3HD_LAYERDECODER *pThisLayer, 
              HMP3HDLAYER hMp3hdLayer, 
              FILEPROPERTIES *pFileProperties, 
              FRAMEPROPERTIES *pFrameProperties,
              MP3_LOSSLESS_DATA *pMp3LosslessData,
              HBITSTREAM pBitStream,
              UINT *pHeaderSize);

/*!
 * \brief
 * resets all decoder values of the cd Layer decoder
 * The bit tream information will not be reseted.
 * 
 * \param ThisLayer
 * cd Layer Decoder handle.
 * 
 * \returns
 * Write description of return value here.
 * 
 */
SSC cdLayerDecoder_softReset(MP3HD_LAYERDECODER ThisLayer);

/*!
 * \brief
 * Decodes the extension layer and enhaces the results of the previously decoded layer.
 * 
 * @param[in] hThisLayer
 * A valid MP3HD_LAYERDECODER handle.
 * 
 * @param[out] pOutBuffer
 * Pointer to the decoded samples of the current layer. 
 * 
 * @param[in] outBufferSize
 * Number of 32 bit Samples to store in pOutBuffer. 
 * 
 * @param[out] pDecodedSamples
 * Number of decoded samples
 * 
 * @param[out] pHashFailed
 * 0: Hash comparison was OK\n
 * 1: Hash comparison failed -> decoding error 
 * 
 * \returns
 * Error codes: 0 is OK otherwise an error occured.
 * 
 */
#ifndef AIT_RELEASE_MP3HD_LIB
SSC cdLayerDecoder_decode(MP3HD_LAYERDECODER hThisLayer,  
                INT *pOutBuffer,
                UINT outBufferSize, 
                UINT *pDecodedSamples,
                UCHAR *pHashFailed);
#else
SSC cdLayerDecoder_decode(MP3HD_LAYERDECODER hThisLayer,  
                SHORT *pOutBuffer,
                UINT outBufferSize, 
                UINT *pDecodedSamples,
                UCHAR *pHashFailed);
#endif

/*!
 * \brief
 * Frees the memory of the LAYER structure.
 * 
 * @param pThisLayer
 * Pointer to the handle of a valid LAYER structure.
 * 
 * \returns
 * Error codes: 0 is OK otherwise an error occured.
 * 
 */
SSC cdLayerDecoder_free(MP3HD_LAYERDECODER *pThisLayer);

SSC cdLayerDecoder_getFATEntriesInPercent(MP3HD_LAYERDECODER hThisLayer, 
                  UINT posInPercent, 
                  UINT *npMp3Position, 
                  UINT *npCdPosition,
                  UINT *pnNewFrame);

SSC cdLayerDecoder_getFATEntries(MP3HD_LAYERDECODER hThisLayer, 
                  UINT posInFrameEntry, 
                  UINT *npMp3Position, 
                  UINT *npCdPosition,
                  UINT *pnNewFrame);

void cdLayerDecoder_clearInitialFrameFlag(MP3HD_LAYERDECODER ThisLayer);

/*!
 * \brief
 * get the current state of the initial frame flag.
 * 
 * @param hThisLayer
 * Pointer to the handle of a valid LAYER structure.
 * 
 * \returns
 * Error codes: 0 is OK otherwise an error occured.
 * 
 */
INT cdLayerDecoder_getInitialFrameFlag(MP3HD_LAYERDECODER hThisLayer);

/*!
 * \brief
 * returns the granularity in number of frames
 * 
 * @param[in] hThisLayer
 * Handle of the cd layer decoder
 * 
 * @param[out] numEntryFrames
 * Granularity in number of frames
 * 
 * \returns
 * SSC_OK: both positions were computed\n
 * SSC_W_CD_SEEKINGNOTSUPPORTED: No FAT stored in the mp3HD bit stream 
 * 
 */
SSC cdLayerDecoder_getFATEntryFrames(MP3HD_LAYERDECODER hThisLayer, 
                  UINT *numEntryFrames);


#ifdef __cplusplus
  }
#endif /* __cplusplus */
#endif /* __LAYERDECODER_H_ */

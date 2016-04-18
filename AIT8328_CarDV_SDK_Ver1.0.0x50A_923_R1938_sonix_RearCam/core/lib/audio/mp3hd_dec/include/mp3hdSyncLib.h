/*!
 * \file mp3hdSyncLib.h
 * Declaration of functions for the synchronization to the mp3HD bitstream.
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


#ifndef __MP3HDSYNCLIB_H_
#define __MP3HDSYNCLIB_H_

#include "mp3hddec_settings.h"
#include "commonMp3HD.h"
#include "commonLayer.h"

#ifdef __cplusplus
  extern "C" {
#endif

typedef struct 
{
   /********************************* 
    mp3hd Initialization parameters */
  UCHAR initDone;
  UCHAR syncFound;
  UINT id3FrameFound;
  /* Data from User */ 
  void *pUserData;
  /* id3 tag parameters */
  UINT id3TagHeaderSize;
  UINT id3TagStart; 
  UCHAR id3TagBuffer[10];
  USHORT id3TagBufferBytesUsed; 
  UINT id3FrameStart;
  /* mp3hd ID header */
  UINT mp3hdIdSize;
  /* mp3 info frame parameters */
  char mp3hdInfoFrameBuffer[MP3HD_MAX_MP3INFOFRAME_SIZE];
  USHORT mp3hdInfoFrameBufferUsed; 
} MP3HD_INIT;

/*!
 * \brief
 * function for searching the beginnig of an id3 tag or mp3 frame.
 * 
 * \param ppGeneralLayer
 * pointer to a pointer of the general layer structure.
 * If the structure doesn't exists it will be allocated. 
 * 
 * \param pMp3Layer
 * A valid pointer to the mp3 layer structure.
 * 
 * \param pInitParameter
 * A valid pointer to the mp3HD init Parameter structure.
 * 
 * \param pId3v2Info
 * A valid pointer to the ID3v2 Info structure.
 * 
 * \param refillMp3hdBuffer
 * A pointer to the refillMp3hdBuffer callback function
 * 
 * \param openExtraLayers
 * A pointer to the openExtraLayers callback function
 * 
 * \param closeExtraLayers
 * A pointer to the closeExtraLayers callback function
 * 
 * \param pMp3LayerStart
 * Pointer to the current mp3 layer start position in byte. 
 * 
 * \param pInBuffer
 * pointer to the current mp3 input buffer
 * 
 * \param InBytes
 * Size of the current mp3 input buffer
 * 
 * \param pInBytesUsed
 * Will be set  to the number of bytes used from the mp3 input buffer
 * 
 * \param pSkipBytes
 * Number of bytes to skip before reading the next mp3 input buffer. 
 * 
 * \returns
 * SSC
 *
 */
SSC mp3HdSync_synchronizeToStream(HMP3HDLAYER *ppGeneralLayer,
                               HMP3HDLAYER pMp3Layer,
                               MP3HD_INIT *pInitParameter,
                               hID3V2Info pId3v2Info, 
                               LP_REFILLEXLAYER refillMp3hdBuffer,
                               LP_OPENEXLAYER openExtraLayers,
                               LP_CLOSEEXLAYER closeExtraLayers,
                               UINT *pMp3LayerStart,
                               const UCHAR* const	pInBuffer,
                               UINT	InBytes,
                               UINT	*pInBytesUsed,
                               INT  *pSkipBytes);

/*!
 * \brief
 * This function is for the evaluation of the mp3hd Info Frame. 
 * A buffer of fixed size is filled and evaluated.
 * 
 * \param ppGeneralLayer
 * Pointer to a valid pointer to the GeneralLayer structure.
 * 
 * \param pMp3Layer
 * Valid pointer to the mp3 layer structure.
 * 
 * \param pInitParameter
 * Valid pointer to the mp3HD init parameter structure.
 * 
 * \param pId3v2Info
 * Valid pointer to the ID3v2 tag structure. 
 * 
 * \param pMp3hdFileProperties
 * Valid pointer to the mp3HD file properties structure.
 * 
 * \param closeExtraLayers
 * Vaild pointer to the closeExtraLayers callback function.
 * 
 * \param pnSuperFrameSize
 * DValid pointer to the super frame size.
 * 
 * \param pMp3LayerStart
 * Valid pointer to the mp3 start position value.
 * 
 * \param pMp3IsScrambled
 * Valid pointer to the mp3 is scambled flag.
 * 
 * \param pTotalSamples
 * Valid pointer to the total number of decoded samples value. 
 * 
 * @param[in] pInBuffer
 * Input buffer of size InBytes bytes.
 * 
 * @param[in] InBytes
 * Number of encoded bytes in the buffer.
 * 
 * @param[out] pInBytesUsed
 * Number of bytes that have been used by the decoder.
 * 
 * @param[out] pSkipBytes
 * Number of bytes to skip before filling the next buffer.
 * pSkipBytes can be negative as well.
 * 
 * @param[in] isEof
 * The end of file flag for very short mp3 chunks.
 * 
 * \returns
 * Error codes: 0 is OK otherwise SSC
 * 
 */
SSC mp3HdSync_evaluateMp3InfoFrame( HMP3HDLAYER *ppGeneralLayer,
                                 HMP3HDLAYER pMp3Layer,
                                 MP3HD_INIT *pInitParameter,
                                 struct ID3V2Info *pId3v2Info,
                                 FILEPROPERTIES *pMp3hdFileProperties,
                                 LP_CLOSEEXLAYER closeExtraLayers,
                                 UINT *pnSuperFrameSize, 
                                 UINT *pMp3LayerStart,
                                 UCHAR *pMp3IsScrambled,
                                 UINT *pTotalSamples, 
                                 const UCHAR* const	pInBuffer,
                                 UINT	InBytes,
                                 UINT	*pInBytesUsed,
                                 INT  *pSkipBytes,
                                 UCHAR isEof);


/*!
 * \brief
 * This function is for the evaluation of the mp3hd ID3 Frame. 
 * 
 * \param ppGeneralLayer
 * Pointer to a valid pointer to the GeneralLayer structure.
 * 
 * \param pInitParameter
 * Valid pointer to the mp3HD init parameter structure.
 * 
 * \param pId3v2Info
 * Valid pointer to the ID3v2 tag structure. 
 * 
 * \param pMp3hdInfoFrame
 * Valid pointer to the mp3HD mp3 info frame structure.
 *
 * \param pMp3hdFileProperties
 * Valid pointer to the mp3HD file properties structure. * 
 * 
 * \param pMp3hdIdData
 * Valid pointer to the mp3HD ID header structure.
 * 
 * \param closeExtraLayers
 * Vaild pointer to the closeExtraLayers callback function.
 * 
 * \param pMp3IsScrambled
 * Valid pointer to the mp3 is scambled flag.
 * 
 * \returns
 * Error codes: 0 is OK otherwise SSC
 * 
 */
SSC mp3HdSync_evaluateId3Frame(HMP3HDLAYER *ppGeneralLayer,
                            MP3HD_INIT *pInitParameter,
                            struct ID3V2Info *pId3v2Info,
                            pMp3InfoFrame pMp3hdInfoFrame,
                            FILEPROPERTIES *pMp3hdFileProperties,
                            struct MP3HDIDDATA *pMp3hdIdData,
                            LP_CLOSEEXLAYER closeExtraLayers,
                            UCHAR *pMp3IsScrambled
                            );

/*!
 * \brief
 * Computes the delay in number of samples and corrects the
 * total number of samples decoded from the super frame.
 * Clears the inital flag of the cd layer decoder if required.
 * 
 * \param hCdLayer
 * Handle to the CD Layer structure. Can be equal to NULL.
 * 
 * \param pMp3hdIdData
 * Valid pointer to the mp3HD ID Header structure 
 * for reading the bitstream information.
 * 
 * \param pNumSampleDelay
 * The value of this pointer is set to the decoder delay in number of samples per channel. 
 * 
 * \param pTotalSamples
 * Pointer to the number of samples per channel of the super frame.
 * 
 */
void mp3HdSync_setNumberOfSamplesToMute(HMP3HDLAYER hCdLayer,
                                     struct MP3HDIDDATA *pMp3hdIdData,
                                     UINT *pNumSampleDelay,
                                     UINT *pTotalSamples 
                                     );


#ifdef __cplusplus
  }
#endif /* __cplusplus */
#endif /* __MP3HDLAYERLIB_H_ */

/*!
 * \file mp3hdLayerLib.h
 * Declaration of layer dependent bit stream functions for the mp3HD decoder lib.
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


#ifndef __MP3HDLAYERLIB_H_
#define __MP3HDLAYERLIB_H_

#include "mp3hddec_settings.h"
#include "commonMp3HD.h"
#include "commonLayer.h"

#ifdef __cplusplus
  extern "C" {
#endif

/* DEFINES *****************************************/

/*!
 * \brief
 * Structure for tracking the read position within a layer. 
 * 
 * 
 * \remarks
 * One instance for each layer
 * 
 * \see
 * HMP3HDLAYER
 */
struct MP3HDLAYER
{
  /* Layer type */
  MP3HDLAYERTYPE type;
  HBITBUFFER pBitBuffer;
  /* Buffer handlich for middle Layer */
  UCHAR *pBuffer;
  UINT nBufferSize;
  LP_REFILLEXLAYER refillMp3hdBuffer;
  /* User Data for the Layer */
  void *pLayerData;
  /* bitsteam read information */
  UINT nTotalLayerBytesFree;
  UINT nTotalLayerBytesRead;
  UINT nLayerHeaderSize;
  UINT nLayerSize;
  /* handle to a layer specific structure */
  MP3HD_LAYERDECODER hLayerDecoder;

  LAYER_STATUS layerStat;
  UINT layerErrCnt;
  UINT cntFrameSinceOn;
};

/*!
 * \brief
 * Function for initialization of the CD layer decoder.
 * The final bit stream parameter for the cd layer are set here. 
 * 
 * @param[in] cdDataSize
 * Size of the cd Layer in Byte.
 * 
 * @param[in] closeExtraLayers
 * Valid pointer to the closeExtraLayers callback function.
 * 
 * @param[in] nSuperFrameSize
 * Size of the super frame in byte.
 * 
 * @param[in] pMp3hdFileProperties
 * Valid pointer to the FILEPROPERTIES structure.
 * 
 * @param[in] pMp3hdFrameProperties
 * Valid pointer to the FRAEMPROPERTIES structure.
 * 
 * @param[in] pMp3LosslessData
 * Valid pointer to the MP3_LOSSLESS_DATA structure.
 * 
 * @param[out] ppCdLayer
 * Pointer to a valid HMP3HDLAYER handle for the CD layer type.
 * 
 * @param[out] pCdLayerStartPtr
 * Valid pointer to the cd layer start position in byte. 
 * 
 * @param[out] pFoundLayers
 * Valid pointer to a MP3HDLAYERTYPE value to set the cd layer flag on correct initialization.
 * 
 * @param[in,out] pLayerStart
 * Valid pointer to the value of the beginnig of the cd layer in byte
 * that will be set to the beginning of the next Layer.
 * 
 * \returns
 * SSC
 * 
 */
SSC mp3HdLayer_createCdLayer(UINT cdDataSize, /* Mp3hd->Mp3hdFileProperties.mp3hdIdData.cdDataSize*/
                               LP_CLOSEEXLAYER closeExtraLayers, /*Mp3hd->closeExtraLayers*/
                               UINT nSuperFrameSize, /*Mp3hd->nSuperFrameSize*/
                               FILEPROPERTIES *pMp3hdFileProperties, 
                               FRAMEPROPERTIES *pMp3hdFrameProperties,
                               MP3_LOSSLESS_DATA *pMp3LosslessData, 
                               HMP3HDLAYER  *ppCdLayer, /** Input/Output: valid cdLayer pointer */
                               UINT *pCdLayerStartPtr, /** Output: cd Layer start position in the super frame */
                               MP3HDLAYERTYPE *pFoundLayers,/** Output: cd Layer flag is set if correct initialized*/
                               UINT *pLayerStart/** Input/Output: in-> cd Layer start position. 
                                                            out-> start position of the next layer */ 
                              );


/*!
 * \brief
 * Function to create (allocate memory) and initialize a new Layer.
 * 
 * @param[out] ppNewLayer
 * Pointer to a HMP3HDLAYER handle for creation.
 * 
 * @param[in] type
 * The layer type of the new layer.
 * 
 * @param[in] pUserData
 * User defined data for the createExtraLayers callback function.
 * 
 * @param[in] refillMp3hdBuffer
 * Pointer to the callback function refillMp3hdBuffer.
 * 
 * @param openExtraLayers
 * Pointer to the callback function openExtraLayers.
 * 
 * @param closeExtraLayers
 * Pointer to the callback function closeExtraLayers.
 * 
 * \returns
 * Error codes: 0 is OK otherwise an error occured.
 * 
 */
SSC mp3HdLayer_createNewLayer(struct MP3HDLAYER **ppNewLayer, 
                                MP3HDLAYERTYPE type,
                                void *pUserData,
                                LP_REFILLEXLAYER refillMp3hdBuffer,
                                LP_OPENEXLAYER openExtraLayers,
                                LP_CLOSEEXLAYER closeExtraLayers                 
                                );

/*!
 * \brief
 * Frees the memory of a Layer 
 * 
 * @param[in] ppLayer
 * Pointer to the HMP3HDLAYER handle to be freed. 
 * 
 * @param[in] closeExtraLayers
 * Pointer to the callback function closeExtraLayers for user indication.
 * 
 * \returns
 * Error codes: 0 is OK otherwise an error occured.
 * 
 */
SSC mp3HdLayer_closeLayer(HMP3HDLAYER *ppLayer,
                            LP_CLOSEEXLAYER closeExtraLayers);

/*!
 * \brief
 * Tries to allocate and reset each mp3HD layer. 
 * 
 * @param[out] pLayer
 * Pointer array of size MP3HD_MAX_NUMBER_OF_LAYERS points to NULL terminated MP3HDLAYER pointers.
 * 
 * @param[in] pUserData
 * Void pointer to user data.
 * 
 * @param[in] refillMp3hdBuffer
 * Valid pointer to the refillMp3hdBuffer callback function.
 * 
 * @param[in] openExtraLayers
 * Valid pointer to the openExtraLayers callback function.
 * 
 * @param[in] closeExtraLayers
 * Valid pointer to the closeExtraLayers callback function.
 * 
 * @param[in] id3TagStart
 * Number of bytes between the beginning of the file and the ID3 tag ID.
 * 
 * \returns
 * SSC
 * 
 */
SSC mp3HdLayer_allocAllLayers(struct MP3HDLAYER *pLayer[/*MP3HD_MAX_NUMBER_OF_LAYERS*/],
                                void *pUserData,
                                LP_REFILLEXLAYER refillMp3hdBuffer,
                                LP_OPENEXLAYER openExtraLayers,
                                LP_CLOSEEXLAYER closeExtraLayers,
                                UINT id3TagStart
                                );


/*!
 * \brief
 * Dummy function that sets the layer values for unsupported mp3HD layers.
 * 
 * @param[in,out] ppLayer
 * Pointer to a valid HMP3HDLAYER of the unsupported layer.
 * 
 * @param[in] closeExtraLayers
 * Valid pointer to the closeExtraLayers callback function.
 * 
 * @param[in,out] pLayerStart
 * Valid pointer to the value of the beginnig of this layer in byte
 * that will be set to the beginning of the next Layer.
 * 
 * @param[in] LayerSize
 * Size of the layer in byte
 * 
 * @param[in] SuperFrameSize
 * Size of the super frame in byte.
 * 
 * @param[out] pFoundLayers
 * Valid pointer to a MP3HDLAYERTYPE value to set the layer flag on correct initialization.
 * 
 * @param[out] pLayerStartPtr
 * Array of five integer values to store the beginning of each layer in the super frame.
 * Sets the beginning in byte from the super frame start to the array index of this layer. 
 * 
 * @param[in] type
 * The layer type
 * CD_LAYER = 1
 * HD_LAYER = 2
 * SR_LAYER = 3
 * EX_LAYER = 4
 * 
 * \returns
 * SS
 *
 */
SSC mp3HdLayer_createUnsupportedLayer( HMP3HDLAYER  *ppLayer,
                                       LP_CLOSEEXLAYER closeExtraLayers, 
                                       UINT *pLayerStart,
                                       UINT LayerSize,
                                       UINT SuperFrameSize,
                                       MP3HDLAYERTYPE *pFoundLayers,
                                       UINT pLayerStartPtr[/*5*/], 
                                       MP3HDLAYERTYPE type
                                      );
/*!
 * \brief
 * Set all layer parameter to their initial values.
 * 
 * @param[in] NewLayer
 * Layer to reset.
 *
 */
void mp3HdLayer_resetLayer(HMP3HDLAYER NewLayer);

/*!
 * \brief
 * This function places the layer in the bit stream using the skipBytes
 * argument. skipBytes relates to the end of the current Buffer.
 * Additionally, the size of the buffer can be set.
 * 
 * \param pLayer
 * Handle of the Layer to be arranged.
 * 
 * \param skipBytes
 * Bytes to skip from the end of the current Layer to 
 * the beginning of the reaaranged Layer.
 * 
 * \param newLayerSize
 * Size of the rearranged Layer.
 *
 * \param newLayerSize
 * Size of the Header of the rearranged Layer.
 * 
 * \returns
 * SSC error Code.
 * 
 */
SSC mp3HdLayer_setLayerValues(HMP3HDLAYER pLayer, 
                              INT skipBytes,
                              UINT newLayerSize,
                              UINT newLayerHeaderSize
                              );

#ifdef __cplusplus
  }
#endif /* __cplusplus */
#endif /* __MP3HDLAYERLIB_H_ */

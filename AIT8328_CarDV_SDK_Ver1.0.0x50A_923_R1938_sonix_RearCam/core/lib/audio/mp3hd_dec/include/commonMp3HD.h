/*!
 * \file commonMp3HD.h
 * Layer comprehensive include file that declares functions and structures 
 * accessible in all layer. 
 * 
 * Copyright (c) 2009 by <Peter Steinborn / Deutsche Thomson OHG (DTO)>
 */
/*
  Copyright (c) 2009, Deutsche Thomson OHG (DTO).
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


#ifndef __COMMONMP3HD_H_
#define __COMMONMP3HD_H_


/* DEFINES *****************************************/
#include "mp3hddec_settings.h"
#include "mp3hdssc.h"

#ifdef __cplusplus
  extern "C" {
#endif

/*!
 * \brief
 * Names of all posible layers.
 * 
 * \remarks
 * Each real layer is conected with a bit of an interger value.\n
 * The GENERAL_LAYER is equal to zero because it is only used for initialization of the extension layers.
 * 
 */

/* definition with preprocessor define necessary because of c++ compatibility */
#define MP3HDLAYERTYPE UINT

#define GENERAL_LAYER  0x00000000 /**< Initialization layer (ID3 tag parsing)*/
#define MP3_LAYER      0x00000001 /**< Decodes the mp3 chunk */
#define CD_LAYER       0x00000002 /**< Decodes the cd chunk: bit resolution <= 16 bit and Fs <= 48 kHz*/
#define HD_LAYER       0x00000004 /**< Decodes the hd chunk: bit resolution > 16 bit and Fs > 48 kHz*/
#define SR_LAYER       0x00000008 /**< Decodes the surround chunk: more than two decoded audio channels*/
#define EX_LAYER       0x00010010 /**< Decodes the extension chunk: further undefined data*/


typedef SSC (*LP_REFILLEXLAYER) (MP3HDLAYERTYPE, void *, UINT, UCHAR **, UINT *, INT);
typedef SSC (*LP_OPENEXLAYER) (MP3HDLAYERTYPE , void *, void **);
typedef SSC (*LP_CLOSEEXLAYER) ( void **);

#ifdef __cplusplus
  }
#endif /* __cplusplus */
#endif /* __COMMONLAYER_H_ */

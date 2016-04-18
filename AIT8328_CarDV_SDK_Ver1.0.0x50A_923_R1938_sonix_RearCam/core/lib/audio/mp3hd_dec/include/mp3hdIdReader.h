/**
  \file

  \brief Header file with definition of struct with info from mp3hdID.
  
  \author Florian Keiler
  \date 2007-11-22
  Copyright (c) 2007, Deutsche Thomson OHG (DTO).

  $Id
*/

/*
  Copyright (c) 2007, Deutsche Thomson OHG (DTO).
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

#ifndef MP3HDIDREADER_H
#define MP3HDIDREADER_H

#include "mp3hddec_settings.h"
#include "commonLayer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief
 * Reset all mp3hd ID data in the struct.
 * 
 * \param[out] pMp3hdId
 * Pointer to the struct.
 * 
 * \returns
 * Zero, if successful.
 * 
 * \author Florian Keiler
 * \date 2007-11-22
 */
INT initMp3hdIdData(struct MP3HDIDDATA* pMp3hdId);

/**
 * \brief
 * reads the mp3hd ID information from the mp3hd ID header
 * 
 * \param[out] pFileProperties
 * A valid pointer to the file propertiy structure. 
 *
 * \param[in] pBitBuffer
 * Valid pointer to a mp3hd Bit buffer structure for reading the bit stream.
 * 
 * \author P.Steinborn / Florian Keiler
 * \date 2008-02-29
 */
UINT readMp3hdId( HFILEPROPERTIES pFileProperties, HBITBUFFER pBitBuffer ); 



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HD3_ID_READER_H */


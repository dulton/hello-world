/***************************************************************************\
 *
 *               (C) copyright Fraunhofer - IIS (1999)
 *                        All Rights Reserved
 *
 *   $Id$
 *   filename: mp3hdssc.h
 *   project : ---
 *   author  : Martin Sieler
 *   date    : 1999-02-15
 *   contents/description: ssc helper class (Structured Status Code)
 *
 *   integer version: Stefan Gewinner gew@iis.fhg.de 991019
 *
\***************************************************************************/


#ifndef __MP3HDSSC_H__
#define __MP3HDSSC_H__

/* ------------------------ includes --------------------------------------*/

#include "mp3hdsscdef.h"
/*
#include "mp3sscdef.h"
*/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
 * calling convention
 */
#ifndef MP3HDDECAPI
#ifdef WIN32
#define MP3HDDECAPI  /*__stdcall*/
#else
#define MP3HDDECAPI
#endif
#endif

/*-------------------------- functions ------------------------------------*/

char* MP3HDDECAPI Ssc2Msg(INT ssc);  /* conversion to a text string */
void MP3HDDECAPI checkSsc(SSC* pSsc, INT quiet);

/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif

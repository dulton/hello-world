/***************************************************************************\
 *                  Fraunhofer IIS MP3s Decoder
 *                 (c) 1996 - 2006 Fraunhofer IIS
 *                    (c) 2006 Fraunhofer IIS
 *                      All Rights Reserved.
 *
 *   $Id$
 *   filename: mp3scec_settings.h
 *   project : MP3s Decoder
 *   contents/description: MP3s Decoder Settings
 *
 *
 *    This software and/or program is protected by copyright law and
 *    international treaties. Any reproduction or distribution of this
 *    software and/or program, or any portion of it, may result in severe
 *    civil and criminal penalties, and will be prosecuted to the maximum
 *    extent possible under law.
 *
\***************************************************************************/
#ifndef MP3DEC_SETTINGS_H
#define MP3DEC_SETTINGS_H

/*
 *  Filterbank Synthesis Configuration
 *
 * Undefine the following (HIGH_QUALITY_HIGH_COMPLEXITY) to save a significant amount of processing power
 * and pay for that with slightly less accuracy. The "important" conformance bitstreams "M1L3_compl.mpg",
 * "M2L3_compl24.mpg", and "M2_5_compl12.mpg" still pass the conformance test then. With other conformance
 * bitstreams the decoder might not pass the test if HIGH_QUALITY_HIGH_COMPLEXITY is undefined.
 * We recommend to undefine HIGH_QUALITY_HIGH_COMPLEXITY for embedded, low-power implementations, where
 * significant savings in processing power outweigh a very slight decrease in audio quality.
 */

#define HIGH_QUALITY_HIGH_COMPLEXITY  /* see explanation above */

/* enable ancillary data processing default - disabled define saves memory in stereo mode only */
#define IMP3_DECINFO_ENABLE
#define IMP3_DECANC_ENABLE

/* ------------------------------------------------------------------------------------ */

#define INPUT_BUFFER_SIZE 4096 /* size of decoder input buffer in byte, once set to 8192 */

#define SAVE_MEM /* if set and _ARM_ not set, 2304 byte of memory will be saved */

/* ------------------------------------------------------------------------------------ */

//#define WORKLOAD_MEASUREMENT

#endif /* MP3DEC_SETTINGS_H */


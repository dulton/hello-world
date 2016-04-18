
#ifndef __AACDEC_H__
#define __AACDEC_H__

#ifndef AACAPI
  #define AACAPI
#endif

#include "structs.h"
#include "AACdec.h"
/* object types for AAC */
#define MAIN       1
#define LC         2
#define SSR        3
#define LTP        4
#define AACPLUS_V1 5
#define AACPLUS_V2 6
#define ER_LC     17
#define ER_LTP    19
#define LD        23
#define DRM_ER_LC 27 /* special object type for DRM */

/* header types */
#define RAW        0
#define ADIF       1
#define ADTS       2

/* SBR signalling */
#define NO_SBR           0
#define SBR_UPSAMPLED    1
#define SBR_DOWNSAMPLED  2
#define NO_SBR_UPSAMPLED 3

/* library output formats */
#define AAC_FMT_16BIT  1
#define AAC_FMT_24BIT  2
#define AAC_FMT_32BIT  3
#define AAC_FMT_FLOAT  4
#define AAC_FMT_DOUBLE 5

/* Capabilities */
#define LC_DEC_CAP           (1<<0) /* Can decode LC */
#define MAIN_DEC_CAP         (1<<1) /* Can decode MAIN */
#define LTP_DEC_CAP          (1<<2) /* Can decode LTP */
#define LD_DEC_CAP           (1<<3) /* Can decode LD */
#define ERROR_RESILIENCE_CAP (1<<4) /* Can decode ER */
#define FIXED_POINT_CAP      (1<<5) /* Fixed point */

/* Channel definitions */
#define FRONT_CHANNEL_CENTER (1)
#define FRONT_CHANNEL_LEFT   (2)
#define FRONT_CHANNEL_RIGHT  (3)
#define SIDE_CHANNEL_LEFT    (4)
#define SIDE_CHANNEL_RIGHT   (5)
#define BACK_CHANNEL_LEFT    (6)
#define BACK_CHANNEL_RIGHT   (7)
#define BACK_CHANNEL_CENTER  (8)
#define LFE_CHANNEL          (9)
#define UNKNOWN_CHANNEL      (0)

/* DRM channel definitions */
#define DRMCH_MONO          1
#define DRMCH_STEREO        2
#define DRMCH_SBR_MONO      3
#define DRMCH_SBR_LC_STEREO 4
#define DRMCH_SBR_STEREO    5


/* A decode call can eat up to AAC_MIN_STREAMSIZE bytes per decoded channel,
   so at least so much bytes per channel should be available in this stream */
#define AAC_MIN_STREAMSIZE 768 /* 6144 bits/channel */


faacDecHandle AACAPI faacDecOpen(void);

faacDecConfigurationPtr AACAPI faacDecGetCurrentConfiguration(faacDecHandle hDecoder);

MMP_UBYTE AACAPI faacDecSetConfiguration(faacDecHandle hDecoder,
                                    faacDecConfigurationPtr config);


/* Init the library based on info from the AAC file (ADTS/ADIF) */
MMP_ULONG AACAPI faacDecInit(faacDecHandle hDecoder,
                            MMP_UBYTE *buffer,
                            MMP_ULONG buffer_size,
                            MMP_ULONG *samplerate,
                            MMP_UBYTE *channels);


void AACAPI faacDecDecode(faacDecHandle hDecoder,
                            faacDecFrameInfo *hInfo,
                            MMP_UBYTE *buffer,
                            MMP_ULONG buffer_size);


#endif

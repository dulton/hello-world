
#ifndef __AACDEC_H__
#define __AACDEC_H__


#ifndef AACAPI
  #define AACAPI
#endif


#include "bits.h"
#include "syntax.h"
#include "drc.h"
#include "specrec.h"


/* library output formats */
#define AAC_FMT_16BIT  1
#define AAC_FMT_24BIT  2
#define AAC_FMT_32BIT  3
#define AAC_FMT_FLOAT  4
#define AAC_FMT_DOUBLE 5

#define LC_DEC_CAP            (1<<0)
#define MAIN_DEC_CAP          (1<<1)
#define LTP_DEC_CAP           (1<<2)
#define LD_DEC_CAP            (1<<3)
#define ERROR_RESILIENCE_CAP  (1<<4)
#define FIXED_POINT_CAP       (1<<5)

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


uint32_t AACAPI faacDecGetCapabilities(void);

faacDecHandle AACAPI faacDecOpen(void);

faacDecConfigurationPtr AACAPI faacDecGetCurrentConfiguration(faacDecHandle hDecoder);

uint8_t AACAPI faacDecSetConfiguration(faacDecHandle hDecoder,
                                    faacDecConfigurationPtr config);

/* Init the library based on info from the AAC file (ADTS/ADIF) */
int32_t AACAPI faacDecInit(faacDecHandle hDecoder,
                            uint8_t *buffer,
                            uint32_t buffer_size,
                            uint32_t *samplerate,
                            uint8_t *channels);


void AACAPI faacDecDecode(faacDecHandle hDecoder,
                            faacDecFrameInfo *hInfo,
                            uint8_t *buffer,
                            uint32_t buffer_size);


#endif

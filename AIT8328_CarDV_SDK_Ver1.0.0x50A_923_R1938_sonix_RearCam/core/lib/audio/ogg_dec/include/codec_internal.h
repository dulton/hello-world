
#ifndef _V_CODECI_H_
#define _V_CODECI_H_
#include "includes_fw.h"
#if (OGG_EN)

#define CHUNKSIZE 2048//4096

#include "ivorbiscodec.h"


struct vorbis_dsp_state{
  vorbis_info    *vi;
  oggpack_buffer  opb;

  ogg_int32_t   **work;
  ogg_int32_t   **mdctright;
  int             out_begin;
  int             out_end;

  long lW;
  long W;

  ogg_int64_t granulepos;
  ogg_int64_t sequence;
  ogg_int64_t sample_count;

};
extern int      OGGDec_output_pcm(vorbis_dsp_state *v,
				  ogg_int16_t *pcm,int samples);

#endif
#endif

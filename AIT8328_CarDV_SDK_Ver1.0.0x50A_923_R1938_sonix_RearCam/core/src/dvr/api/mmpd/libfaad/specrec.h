
#ifndef __SPECREC_H__
#define __SPECREC_H__

#include "syntax.h"

uint8_t window_grouping_info(faacDecHandle hDecoder, ic_stream *ics);
void apply_scalefactors(faacDecHandle hDecoder, ic_stream *ics, real_t *x_invquant,
                        uint16_t frame_len);
uint8_t reconstruct_channel_pair(faacDecHandle hDecoder, ic_stream *ics1, ic_stream *ics2,
                                 element *cpe, int16_t *spec_data1, int16_t *spec_data2);
uint8_t reconstruct_single_channel(faacDecHandle hDecoder, ic_stream *ics, element *sce,
                                int16_t *spec_data);

extern void iquant_function(real_t *x_invquant, const int16_t *x_quant, const real_t *tab, const real_t *errcor, const uint16_t frame_len);


#endif

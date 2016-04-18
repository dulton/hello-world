
#ifndef __SBR_HUFF_H__
#define __SBR_HUFF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "aac_comm.h"

void sbr_envelope(bitfile *ld, sbr_info *sbr, uint8_t ch);
void sbr_noise(bitfile *ld, sbr_info *sbr, uint8_t ch);

#ifdef __cplusplus
}
#endif
#endif


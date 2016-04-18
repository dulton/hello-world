
#ifndef __SBR_SYNTAX_H__
#define __SBR_SYNTAX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "aac_comm.h"
#include "sbr_dec.h"
#include "bits.h"

#define T_HFGEN 8
#define T_HFADJ 2

#define EXT_SBR_DATA     13
#define EXT_SBR_DATA_CRC 14

#define FIXFIX 0
#define FIXVAR 1
#define VARFIX 2
#define VARVAR 3

#define LO_RES 0
#define HI_RES 1

#define NO_TIME_SLOTS_960 15
#define NO_TIME_SLOTS     16
#define RATE              2

#define NOISE_FLOOR_OFFSET 6


uint8_t sbr_extension_data(bitfile *ld, sbr_info *sbr, uint16_t cnt);
sbr_info *sbrDecodeInit(uint16_t framelength, uint8_t id_aac,
                        uint32_t sample_rate
                        );


#ifdef __cplusplus
}
#endif
#endif /* __SBR_SYNTAX_H__ */


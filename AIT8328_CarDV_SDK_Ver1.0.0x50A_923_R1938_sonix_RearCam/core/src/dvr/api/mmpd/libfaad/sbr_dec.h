

#ifndef __SBR_DEC_H__
#define __SBR_DEC_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PS_DEC
//#include "ps_dec.h"
#endif
#ifdef DRM_PS
#include "drm_dec.h"
#endif

/* MAX_NTSRHFG: maximum of number_time_slots * rate + HFGen. 16*2+8 */
#define MAX_NTSRHFG 40
#define MAX_NTSR    32 /* max number_time_slots * rate, ok for DRM and not DRM mode */


typedef struct
{
    uint32_t sample_rate;

    uint8_t rate;
    uint8_t just_seeked;
    uint8_t ret;
    uint8_t amp_res[2];

    uint8_t M;
    uint8_t N_high;
    uint8_t N_Q;
    uint8_t n[2];
    int8_t L_E[2];
    uint8_t L_Q[2];

    uint8_t t_E[2][6];
    uint8_t t_Q[2][3];
    uint8_t f[2][6];

//    int16_t E[2][64][5];
//    int32_t Q[2][64][2];
    uint8_t bs_invf_mode[2][5];
    uint8_t bs_add_harmonic[2][64];

    int8_t bs_start_freq_prev;
    uint8_t bs_stop_freq_prev;
    uint8_t bs_xover_band_prev;
    uint8_t bs_freq_scale_prev;
    uint8_t bs_alter_scale_prev;
    uint8_t bs_noise_bands_prev;
    uint8_t Reset;
    uint32_t frame;
    uint32_t header_count;
    uint8_t id_aac;
	uint8_t numTimeSlotsRate;
	uint8_t numTimeSlots;
	
    uint8_t ps_used;

    /* to get it compiling */
    /* we'll see during the coding of all the tools, whether
       these are all used or not.
    */
    uint8_t bs_header_flag;
    uint8_t bs_crc_flag;
    uint16_t bs_sbr_crc_bits;
    uint8_t bs_amp_res;
    uint8_t bs_start_freq;
    uint8_t bs_stop_freq;
    uint8_t bs_xover_band;
    uint8_t bs_freq_scale;
    uint8_t bs_alter_scale;
    uint8_t bs_noise_bands;
    uint8_t bs_limiter_bands;
    uint8_t bs_limiter_gains;
    uint8_t bs_interpol_freq;
    uint8_t bs_smoothing_mode;
    uint8_t bs_samplerate_mode;
    uint8_t bs_add_harmonic_flag[2];
    uint8_t bs_extended_data;
    uint8_t bs_extension_id;
    uint8_t bs_extension_data;
    uint8_t bs_coupling;
    uint8_t bs_frame_class[2];
    uint8_t bs_rel_bord[2][9];
    uint8_t bs_rel_bord_0[2][9];
    uint8_t bs_rel_bord_1[2][9];
    uint8_t bs_pointer[2];
    uint8_t bs_abs_bord_0[2];
    uint8_t bs_abs_bord_1[2];
    uint8_t bs_num_rel_0[2];
    uint8_t bs_num_rel_1[2];
    uint8_t bs_df_env[2][9];
    uint8_t bs_df_noise[2][3];
} sbr_info;

sbr_info *sbrDecodeInit(uint16_t framelength, uint8_t id_aac,
                        uint32_t sample_rate
#ifdef DRM
						, uint8_t IsDRM
#endif
						);
void sbrDecodeEnd(sbr_info *sbr);

uint8_t sbrDecodeCoupleFrame(sbr_info *sbr, real_t *left_chan, real_t *right_chan,
                             const uint8_t just_seeked, const uint8_t upsample_only);
uint8_t sbrDecodeSingleFrame(sbr_info *sbr, real_t *channel,
                             const uint8_t just_seeked, const uint8_t upsample_only);



#ifdef __cplusplus
}
#endif
#endif


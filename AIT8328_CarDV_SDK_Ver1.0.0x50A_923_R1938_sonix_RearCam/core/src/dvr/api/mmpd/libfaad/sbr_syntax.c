
#include "structs.h"

#ifdef SBR_DEC

#include "sbr_syntax.h"
#include "syntax.h"
#include "sbr_huff.h"
#include "bits.h"
#ifdef PS_DEC
#include "ps_dec.h"
#endif
#ifdef DRM_PS
#include "drm_dec.h"
#endif

#include "ait_utility.h"
#if (AAC_HOST_PARSER_EN == 1)


sbr_info iSbr;

/* static function declarations */
static void sbr_header(bitfile *ld, sbr_info *sbr);
static uint8_t sbr_data(bitfile *ld, sbr_info *sbr);
static uint16_t sbr_extension(bitfile *ld, sbr_info *sbr,
                              uint8_t bs_extension_id, uint16_t num_bits_left);
static uint8_t sbr_single_channel_element(bitfile *ld, sbr_info *sbr);
static uint8_t sbr_channel_pair_element(bitfile *ld, sbr_info *sbr);
static uint8_t sbr_grid(bitfile *ld, sbr_info *sbr, uint8_t ch);
static void sbr_dtdf(bitfile *ld, sbr_info *sbr, uint8_t ch);
static void invf_mode(bitfile *ld, sbr_info *sbr, uint8_t ch);
static void sinusoidal_coding(bitfile *ld, sbr_info *sbr, uint8_t ch);

sbr_info *sbrDecodeInit(uint16_t framelength, uint8_t id_aac,
                        uint32_t sample_rate
                        )
{
//    sbr_info *sbr = (sbr_info *)malloc(sizeof(sbr_info));
    sbr_info *sbr = (sbr_info *)&iSbr;
    MEMSET(sbr, 0, sizeof(sbr_info));

    /* save id of the parent element */
    sbr->id_aac = id_aac;
    sbr->sample_rate = sample_rate;

    sbr->bs_freq_scale = 2;
    sbr->bs_alter_scale = 1;
    sbr->bs_noise_bands = 2;
    sbr->bs_limiter_bands = 2;
    sbr->bs_limiter_gains = 2;
    sbr->bs_interpol_freq = 1;
    sbr->bs_smoothing_mode = 1;
    sbr->bs_start_freq = 5;
    sbr->bs_amp_res = 1;
    sbr->bs_samplerate_mode = 1;
//    sbr->prevEnvIsShort[0] = -1;
//    sbr->prevEnvIsShort[1] = -1;
    sbr->header_count = 0;
    sbr->bs_samplerate_mode = 1;

    /* force sbr reset */
    sbr->bs_start_freq_prev = -1;

    if (framelength == 960)
    {
        sbr->numTimeSlotsRate = RATE * NO_TIME_SLOTS_960;
        sbr->numTimeSlots = NO_TIME_SLOTS_960;
    } else {
        sbr->numTimeSlotsRate = RATE * NO_TIME_SLOTS;
        sbr->numTimeSlots = NO_TIME_SLOTS;
    }

    return sbr;
}

static void sbr_reset(sbr_info *sbr)
{

    /* if these are different from the previous frame: Reset = 1 */
    if ((sbr->bs_start_freq != sbr->bs_start_freq_prev) ||
        (sbr->bs_stop_freq != sbr->bs_stop_freq_prev) ||
        (sbr->bs_freq_scale != sbr->bs_freq_scale_prev) ||
        (sbr->bs_alter_scale != sbr->bs_alter_scale_prev))
    {
        sbr->Reset = 1;
    } else {
        sbr->Reset = 0;
    }

    if ((sbr->bs_start_freq != sbr->bs_start_freq_prev) ||
        (sbr->bs_stop_freq != sbr->bs_stop_freq_prev) ||
        (sbr->bs_freq_scale != sbr->bs_freq_scale_prev) ||
        (sbr->bs_alter_scale != sbr->bs_alter_scale_prev) ||
        (sbr->bs_xover_band != sbr->bs_xover_band_prev) ||
        (sbr->bs_noise_bands != sbr->bs_noise_bands_prev))
    {
        sbr->Reset = 1;
    } else {
        sbr->Reset = 0;
    }

    sbr->bs_start_freq_prev = sbr->bs_start_freq;
    sbr->bs_stop_freq_prev = sbr->bs_stop_freq;
    sbr->bs_freq_scale_prev = sbr->bs_freq_scale;
    sbr->bs_alter_scale_prev = sbr->bs_alter_scale;
    sbr->bs_xover_band_prev = sbr->bs_xover_band;
    sbr->bs_noise_bands_prev = sbr->bs_noise_bands;

    if (sbr->frame == 0)
    {
        sbr->Reset = 1;
    }
}

/* table 2 */
uint8_t sbr_extension_data(bitfile *ld, sbr_info *sbr, uint16_t cnt)
{
    uint8_t result = 0;
    uint16_t num_align_bits = 0;
    uint16_t num_sbr_bits = (uint16_t)aac_get_processed_bits(ld);

    {
        uint8_t bs_extension_type = (uint8_t)aac_getbits(ld, 4);

        if (bs_extension_type == EXT_SBR_DATA_CRC)
        {
            sbr->bs_sbr_crc_bits = (uint16_t)aac_getbits(ld, 10);
        }
    }

    sbr->bs_header_flag = aac_get1bit(ld);

    if (sbr->bs_header_flag)
        sbr_header(ld, sbr);

    /* Reset? */
    sbr_reset(sbr);

    /* first frame should have a header */
    //if (!(sbr->frame == 0 && sbr->bs_header_flag == 0))
    if (sbr->header_count != 0)
    {

        if (result == 0)
            result = sbr_data(ld, sbr);
    } else {
        result = 1;
    }

    {
        num_sbr_bits = (uint16_t)aac_get_processed_bits(ld) - num_sbr_bits;
        /* -4 does not apply, bs_extension_type is re-read in this function */
        num_align_bits = 8*cnt /*- 4*/ - num_sbr_bits;

        while (num_align_bits > 7)
        {
            aac_getbits(ld, 8);
            num_align_bits -= 8;
        }
        aac_getbits(ld, num_align_bits);
    }

//    return result;
    return 0;
}

/* table 3 */
static void sbr_header(bitfile *ld, sbr_info *sbr)
{
    uint8_t bs_header_extra_1, bs_header_extra_2;

    sbr->header_count++;

    sbr->bs_amp_res = aac_get1bit(ld);

    /* bs_start_freq and bs_stop_freq must define a fequency band that does
       not exceed 48 channels */
    sbr->bs_start_freq = (uint8_t)aac_getbits(ld, 4);

    sbr->bs_stop_freq = (uint8_t)aac_getbits(ld, 4);

    sbr->bs_xover_band = (uint8_t)aac_getbits(ld, 3);

    aac_getbits(ld, 2);

    bs_header_extra_1 = (uint8_t)aac_get1bit(ld);

    bs_header_extra_2 = (uint8_t)aac_get1bit(ld);


    if (bs_header_extra_1)
    {
        sbr->bs_freq_scale = (uint8_t)aac_getbits(ld, 2);

        sbr->bs_alter_scale = (uint8_t)aac_get1bit(ld);

        sbr->bs_noise_bands = (uint8_t)aac_getbits(ld, 2);

    } else {
        /* Default values */
        sbr->bs_freq_scale = 2;
        sbr->bs_alter_scale = 1;
        sbr->bs_noise_bands = 2;
    }

    if (bs_header_extra_2)
    {
        sbr->bs_limiter_bands = (uint8_t)aac_getbits(ld, 2);

        sbr->bs_limiter_gains = (uint8_t)aac_getbits(ld, 2);

        sbr->bs_interpol_freq = (uint8_t)aac_get1bit(ld);

        sbr->bs_smoothing_mode = (uint8_t)aac_get1bit(ld);

    } else {
        /* Default values */
        sbr->bs_limiter_bands = 2;
        sbr->bs_limiter_gains = 2;
        sbr->bs_interpol_freq = 1;
        sbr->bs_smoothing_mode = 1;
    }

#if 0
    /* print the header to screen */
    printf("bs_amp_res: %d\n", sbr->bs_amp_res);
    printf("bs_start_freq: %d\n", sbr->bs_start_freq);
    printf("bs_stop_freq: %d\n", sbr->bs_stop_freq);
    printf("bs_xover_band: %d\n", sbr->bs_xover_band);
    if (bs_header_extra_1)
    {
        printf("bs_freq_scale: %d\n", sbr->bs_freq_scale);
        printf("bs_alter_scale: %d\n", sbr->bs_alter_scale);
        printf("bs_noise_bands: %d\n", sbr->bs_noise_bands);
    }
    if (bs_header_extra_2)
    {
        printf("bs_limiter_bands: %d\n", sbr->bs_limiter_bands);
        printf("bs_limiter_gains: %d\n", sbr->bs_limiter_gains);
        printf("bs_interpol_freq: %d\n", sbr->bs_interpol_freq);
        printf("bs_smoothing_mode: %d\n", sbr->bs_smoothing_mode);
    }
    printf("\n");
#endif
}

/* table 4 */
static uint8_t sbr_data(bitfile *ld, sbr_info *sbr)
{
    uint8_t result;
#if 0
    sbr->bs_samplerate_mode = aac_get1bit(ld
        DEBUGVAR(1,219,"sbr_data(): bs_samplerate_mode"));
#endif

    sbr->rate = (sbr->bs_samplerate_mode) ? 2 : 1;

    switch (sbr->id_aac)
    {
    case ID_SCE:
		if ((result = sbr_single_channel_element(ld, sbr)) > 0)
			return result;
        break;
    case ID_CPE:
		if ((result = sbr_channel_pair_element(ld, sbr)) > 0)
			return result;
        break;
    }

	return 0;
}

/* table 5 */
static uint8_t sbr_single_channel_element(bitfile *ld, sbr_info *sbr)
{
    uint8_t result;

    if (aac_get1bit(ld))
    {
        aac_getbits(ld, 4);
    }


    if ((result = sbr_grid(ld, sbr, 0)) > 0)
        return result;
    sbr_dtdf(ld, sbr, 0);
    invf_mode(ld, sbr, 0);
    sbr_envelope(ld, sbr, 0);
    sbr_noise(ld, sbr, 0);

//    envelope_noise_dequantisation(sbr, 0);

    memset(sbr->bs_add_harmonic[0], 0, 64*sizeof(uint8_t));

    sbr->bs_add_harmonic_flag[0] = aac_get1bit(ld);

    if (sbr->bs_add_harmonic_flag[0])
        sinusoidal_coding(ld, sbr, 0);

    sbr->bs_extended_data = aac_get1bit(ld);

    if (sbr->bs_extended_data)
    {
        uint16_t nr_bits_left;
        uint16_t cnt = (uint16_t)aac_getbits(ld, 4);

        if (cnt == 15)
        {
            cnt += (uint16_t)aac_getbits(ld, 8);
        }

        nr_bits_left = 8 * cnt;
        while (nr_bits_left > 7)
        {
            sbr->bs_extension_id = (uint8_t)aac_getbits(ld, 2);
            nr_bits_left -= 2;
            nr_bits_left -= sbr_extension(ld, sbr, sbr->bs_extension_id, nr_bits_left);
        }

        /* Corrigendum */
        if (nr_bits_left > 0)
        {
            aac_getbits(ld, nr_bits_left);
        }
    }

    sbr->ps_used = 1;
    return 0;
}

/* table 6 */
static uint8_t sbr_channel_pair_element(bitfile *ld, sbr_info *sbr)
{
    uint8_t n, result;

    if (aac_get1bit(ld))
    {
        aac_getbits(ld, 4);
        aac_getbits(ld, 4);
    }

    sbr->bs_coupling = aac_get1bit(ld);

    if (sbr->bs_coupling)
    {
        if ((result = sbr_grid(ld, sbr, 0)) > 0)
            return result;

        /* need to copy some data from left to right */
        sbr->bs_frame_class[1] = sbr->bs_frame_class[0];
        sbr->L_E[1] = sbr->L_E[0];
        sbr->L_Q[1] = sbr->L_Q[0];
        sbr->bs_pointer[1] = sbr->bs_pointer[0];

        for (n = 0; n <= sbr->L_E[0]; n++)
        {
            sbr->t_E[1][n] = sbr->t_E[0][n];
            sbr->f[1][n] = sbr->f[0][n];
        }
        for (n = 0; n <= sbr->L_Q[0]; n++)
            sbr->t_Q[1][n] = sbr->t_Q[0][n];

        sbr_dtdf(ld, sbr, 0);
        sbr_dtdf(ld, sbr, 1);
        invf_mode(ld, sbr, 0);

        /* more copying */
        for (n = 0; n < sbr->N_Q; n++)
            sbr->bs_invf_mode[1][n] = sbr->bs_invf_mode[0][n];

        sbr_envelope(ld, sbr, 0);
        sbr_noise(ld, sbr, 0);
        sbr_envelope(ld, sbr, 1);
        sbr_noise(ld, sbr, 1);

        memset(sbr->bs_add_harmonic[0], 0, 64*sizeof(uint8_t));
        memset(sbr->bs_add_harmonic[1], 0, 64*sizeof(uint8_t));

        sbr->bs_add_harmonic_flag[0] = aac_get1bit(ld);

        if (sbr->bs_add_harmonic_flag[0])
            sinusoidal_coding(ld, sbr, 0);

        sbr->bs_add_harmonic_flag[1] = aac_get1bit(ld);

        if (sbr->bs_add_harmonic_flag[1])
            sinusoidal_coding(ld, sbr, 1);
    } else {
        if ((result = sbr_grid(ld, sbr, 0)) > 0)
            return result;
        if ((result = sbr_grid(ld, sbr, 1)) > 0)
            return result;
        sbr_dtdf(ld, sbr, 0);
        sbr_dtdf(ld, sbr, 1);
        invf_mode(ld, sbr, 0);
        invf_mode(ld, sbr, 1);
        sbr_envelope(ld, sbr, 0);
        sbr_envelope(ld, sbr, 1);
        sbr_noise(ld, sbr, 0);
        sbr_noise(ld, sbr, 1);

        memset(sbr->bs_add_harmonic[0], 0, 64*sizeof(uint8_t));
        memset(sbr->bs_add_harmonic[1], 0, 64*sizeof(uint8_t));

        sbr->bs_add_harmonic_flag[0] = aac_get1bit(ld);

        if (sbr->bs_add_harmonic_flag[0])
            sinusoidal_coding(ld, sbr, 0);

        sbr->bs_add_harmonic_flag[1] = aac_get1bit(ld);

        if (sbr->bs_add_harmonic_flag[1])
            sinusoidal_coding(ld, sbr, 1);
    }
//    envelope_noise_dequantisation(sbr, 0);
//    envelope_noise_dequantisation(sbr, 1);

//    if (sbr->bs_coupling)
//        unmap_envelope_noise(sbr);

    sbr->bs_extended_data = aac_get1bit(ld);

    if (sbr->bs_extended_data)
    {
        uint16_t nr_bits_left;
        uint16_t cnt = (uint16_t)aac_getbits(ld, 4);

        if (cnt == 15)
        {
            cnt += (uint16_t)aac_getbits(ld, 8);
        }

        nr_bits_left = 8 * cnt;
        while (nr_bits_left > 7)
        {
            sbr->bs_extension_id = (uint8_t)aac_getbits(ld, 2);
            nr_bits_left -= 2;
            sbr_extension(ld, sbr, sbr->bs_extension_id, nr_bits_left);
        }

        /* Corrigendum */
        if (nr_bits_left > 0)
        {
            aac_getbits(ld, nr_bits_left);
        }
    }

    return 0;
}

/* integer log[2](x): input range [0,10) */
static int8_t sbr_log2(const int8_t val)
{
    int8_t log2tab[] = { 0, 0, 1, 2, 2, 3, 3, 3, 3, 4 };
    if (val < 10 && val >= 0)
        return log2tab[val];
    else
        return 0;
}


/* table 7 */
static uint8_t sbr_grid(bitfile *ld, sbr_info *sbr, uint8_t ch)
{
    uint8_t i, env, rel;
    uint8_t bs_abs_bord, bs_abs_bord_1;
    uint8_t bs_num_env = 0;

    sbr->bs_frame_class[ch] = (uint8_t)aac_getbits(ld, 2);

    switch (sbr->bs_frame_class[ch])
    {
    case FIXFIX:
        i = (uint8_t)aac_getbits(ld, 2);

        bs_num_env = min(1 << i, 5);

        i = (uint8_t)aac_get1bit(ld);
        for (env = 0; env < bs_num_env; env++)
            sbr->f[ch][env] = i;

//        sbr->abs_bord_lead[ch] = 0;
//        sbr->abs_bord_trail[ch] = sbr->numTimeSlots;
//        sbr->n_rel_lead[ch] = bs_num_env - 1;
//        sbr->n_rel_trail[ch] = 0;
        break;

    case FIXVAR:
        bs_abs_bord = (uint8_t)aac_getbits(ld, 2) + sbr->numTimeSlots;

        bs_num_env = (uint8_t)aac_getbits(ld, 2) + 1;

        for (rel = 0; rel < bs_num_env-1; rel++)
        {
            sbr->bs_rel_bord[ch][rel] = 2 * (uint8_t)aac_getbits(ld, 2) + 2;
        }
        i = sbr_log2(bs_num_env + 1);
        sbr->bs_pointer[ch] = (uint8_t)aac_getbits(ld, i);

        for (env = 0; env < bs_num_env; env++)
        {
            sbr->f[ch][bs_num_env - env - 1] = (uint8_t)aac_get1bit(ld);
        }

//        sbr->abs_bord_lead[ch] = 0;
//        sbr->abs_bord_trail[ch] = bs_abs_bord;
//        sbr->n_rel_lead[ch] = 0;
//        sbr->n_rel_trail[ch] = bs_num_env - 1;
        break;

    case VARFIX:
        bs_abs_bord = (uint8_t)aac_getbits(ld, 2);
        bs_num_env = (uint8_t)aac_getbits(ld, 2) + 1;

        for (rel = 0; rel < bs_num_env-1; rel++)
        {
            sbr->bs_rel_bord[ch][rel] = 2 * (uint8_t)aac_getbits(ld, 2) + 2;
        }
        i = sbr_log2(bs_num_env + 1);
        sbr->bs_pointer[ch] = (uint8_t)aac_getbits(ld, i);

        for (env = 0; env < bs_num_env; env++)
        {
            sbr->f[ch][env] = (uint8_t)aac_get1bit(ld);
        }

//        sbr->abs_bord_lead[ch] = bs_abs_bord;
//        sbr->abs_bord_trail[ch] = sbr->numTimeSlots;
//        sbr->n_rel_lead[ch] = bs_num_env - 1;
//        sbr->n_rel_trail[ch] = 0;
        break;

    case VARVAR:
        bs_abs_bord = (uint8_t)aac_getbits(ld, 2);

        bs_abs_bord_1 = (uint8_t)aac_getbits(ld, 2) + sbr->numTimeSlots;

        sbr->bs_num_rel_0[ch] = (uint8_t)aac_getbits(ld, 2);

        sbr->bs_num_rel_1[ch] = (uint8_t)aac_getbits(ld, 2);


        bs_num_env = min(5, sbr->bs_num_rel_0[ch] + sbr->bs_num_rel_1[ch] + 1);

        for (rel = 0; rel < sbr->bs_num_rel_0[ch]; rel++)
        {
            sbr->bs_rel_bord_0[ch][rel] = 2 * (uint8_t)aac_getbits(ld, 2) + 2;
        }
        for(rel = 0; rel < sbr->bs_num_rel_1[ch]; rel++)
        {
            sbr->bs_rel_bord_1[ch][rel] = 2 * (uint8_t)aac_getbits(ld, 2) + 2;
        }
        i = sbr_log2(sbr->bs_num_rel_0[ch] + sbr->bs_num_rel_1[ch] + 2);
        sbr->bs_pointer[ch] = (uint8_t)aac_getbits(ld, i);

        for (env = 0; env < bs_num_env; env++)
        {
            sbr->f[ch][env] = (uint8_t)aac_get1bit(ld);
        }

//        sbr->abs_bord_lead[ch] = bs_abs_bord;
//        sbr->abs_bord_trail[ch] = bs_abs_bord_1;
//        sbr->n_rel_lead[ch] = sbr->bs_num_rel_0[ch];
//        sbr->n_rel_trail[ch] = sbr->bs_num_rel_1[ch];
        break;
    }

    if (sbr->bs_frame_class[ch] == VARVAR)
        sbr->L_E[ch] = min(bs_num_env, 5);
    else
        sbr->L_E[ch] = min(bs_num_env, 4);

    if (sbr->L_E[ch] <= 0)
        return 1;

    if (sbr->L_E[ch] > 1)
        sbr->L_Q[ch] = 2;
    else
        sbr->L_Q[ch] = 1;

    return 0;
}

/* table 8 */
static void sbr_dtdf(bitfile *ld, sbr_info *sbr, uint8_t ch)
{
    uint8_t i;

    for (i = 0; i < sbr->L_E[ch]; i++)
    {
        sbr->bs_df_env[ch][i] = aac_get1bit(ld);
    }

    for (i = 0; i < sbr->L_Q[ch]; i++)
    {
        sbr->bs_df_noise[ch][i] = aac_get1bit(ld);
    }
}

/* table 9 */
static void invf_mode(bitfile *ld, sbr_info *sbr, uint8_t ch)
{
    uint8_t n;

    for (n = 0; n < sbr->N_Q; n++)
    {
        sbr->bs_invf_mode[ch][n] = (uint8_t)aac_getbits(ld, 2);
    }
}

#define  EXTENSION_ID_PS   2 

static uint16_t sbr_extension(bitfile *ld, sbr_info *sbr,
                              uint8_t bs_extension_id, uint16_t num_bits_left)
{    
    switch (bs_extension_id)
    {
    case EXTENSION_ID_PS:
        sbr->ps_used = 1;
//        return ps_data(&(sbr->ps), ld);

#ifdef DRM_PS
    case DRM_PARAMETRIC_STEREO:
        sbr->ps_used = 1;
        return drm_ps_data(&(sbr->drm_ps), ld);
#endif
    default:
        sbr->bs_extension_data = (uint8_t)aac_getbits(ld, 6);
        return 6;
    }
}

/* table 12 */
static void sinusoidal_coding(bitfile *ld, sbr_info *sbr, uint8_t ch)
{
    uint8_t n;

    for (n = 0; n < sbr->N_high; n++)
    {
        sbr->bs_add_harmonic[ch][n] = aac_get1bit(ld);
    }
}


#endif /* SBR_DEC */
#endif

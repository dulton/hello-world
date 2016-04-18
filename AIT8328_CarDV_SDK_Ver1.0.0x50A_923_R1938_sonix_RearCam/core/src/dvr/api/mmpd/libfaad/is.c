
#include "aac_comm.h"
#include "structs.h"

#include "syntax.h"
#include "is.h"

static real_t pow05_table[] = {
    COEF_CONST(1.68179283050743), /* 0.5^(-3/4) */
    COEF_CONST(1.41421356237310), /* 0.5^(-2/4) */
    COEF_CONST(1.18920711500272), /* 0.5^(-1/4) */
    COEF_CONST(1.0),              /* 0.5^( 0/4) */
    COEF_CONST(0.84089641525371), /* 0.5^(+1/4) */
    COEF_CONST(0.70710678118655), /* 0.5^(+2/4) */
    COEF_CONST(0.59460355750136)  /* 0.5^(+3/4) */
};


void is_decode(ic_stream *ics, ic_stream *icsr, real_t *l_spec, real_t *r_spec,
               uint16_t frame_len)
{
    uint32_t g, sfb, b;
    uint32_t i;
    int32_t exp, frac;

    uint32_t nshort = frame_len/8;
    uint32_t group = 0;

    for (g = 0; g < icsr->num_window_groups; g++)
    {
        /* Do intensity stereo decoding */
        for (b = 0; b < icsr->window_group_length[g]; b++)
        {
            for (sfb = 0; sfb < icsr->max_sfb; sfb++)
            {
                if (is_intensity(icsr, g, sfb))
                {
                    /* For scalefactor bands coded in intensity stereo the
                       corresponding predictors in the right channel are
                       switched to "off".
                     */
//                    ics->pred.prediction_used[sfb] = 0;
//                    icsr->pred.prediction_used[sfb] = 0;

                    exp = icsr->scale_factors[g][sfb] >> 2;
                    frac = icsr->scale_factors[g][sfb] & 3;

                    /* Scale from left to right channel,
                       do not touch left channel */
                    for (i = icsr->swb_offset[sfb]; i < icsr->swb_offset[sfb+1]; i++)
                    {
                        if (exp < 0)
                            r_spec[(group*nshort)+i] = l_spec[(group*nshort)+i] << -exp;
                        else
                            r_spec[(group*nshort)+i] = l_spec[(group*nshort)+i] >> exp;
                        r_spec[(group*nshort)+i] = MUL_C(r_spec[(group*nshort)+i], pow05_table[frac + 3]);

                        if (is_intensity(icsr, g, sfb) != invert_intensity(ics, g, sfb))
                            r_spec[(group*nshort)+i] = -r_spec[(group*nshort)+i];
                    }
                }
            }
            group++;
        }
    }
}

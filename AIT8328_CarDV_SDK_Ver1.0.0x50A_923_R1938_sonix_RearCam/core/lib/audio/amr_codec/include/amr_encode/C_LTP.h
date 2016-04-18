/*
********************************************************************************
*
*      GSM AMR-NB speech codec   REV1.0   May 25, 2004
*
*      Modified by  Eric Huang 
********************************************************************************
*
*      File             : cl_ltp.h
*      Purpose          : Closed-loop fractional pitch search
*
********************************************************************************
*/
#ifndef cl_ltp_h
#define cl_ltp_h "$Id $"

#if	defined(AMR_RP_FW)

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "cnst.h"
#include "mode.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/

/*
********************************************************************************
*                         DEFINITION OF DATA TYPES
********************************************************************************
*/
typedef struct {
   Word16 T0_prev_subframe;   /* integer pitch lag of previous sub-frame */
} Pitch_frState;


typedef struct {
    Pitch_frState pitchSt;
} clLtpState;


typedef struct {

   /* counters */
   Word16 count;
   
   /* gain history Q11 */
   Word16 gp[N_FRAME];
   
} tonStabState;


/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
int cl_ltp_init (clLtpState *st);
/* initialize one instance of the pre processing state.
   Stores pointer to filter status struct in *st. This pointer has to
   be passed to cl_ltp in each call.
   returns 0 on success
 */

int cl_ltp_reset (clLtpState *st);
/* reset of pre processing state (i.e. set state memory to zero)
   returns 0 on success
 */

int cl_ltp(
    clLtpState *clSt,    /* i/o : State struct                              */
    tonStabState *tonSt, /* i/o : State struct                              */
    enum Mode mode,      /* i   : coder mode                                */
    Word16 frameOffset,  /* i   : Offset to subframe                        */
    Word16 T_op[],       /* i   : Open loop pitch lags                      */
    Word16 *h1,          /* i   : Impulse response vector               Q12 */
    Word16 *exc,         /* i/o : Excitation vector                      Q0 */
    Word16 res2[],       /* i/o : Long term prediction residual          Q0 */
    Word16 xn[],         /* i   : Target vector for pitch search         Q0 */
    Word16 lsp_flag,     /* i   : LSP resonance flag                        */
    Word16 xn2[],        /* o   : Target vector for codebook search      Q0 */
    Word16 y1[],         /* o   : Filtered adaptive excitation           Q0 */
    Word16 *T0,          /* o   : Pitch delay (integer part)                */
    Word16 *T0_frac,     /* o   : Pitch delay (fractional part)             */
    Word16 *gain_pit,    /* o   : Pitch gain                            Q14 */
    Word16 g_coeff[],    /* o   : Correlations between xn, y1, & y2         */
    Word16 **anap,       /* o   : Analysis parameters                       */
    Word16 *gp_limit     /* o   : pitch gain limit                          */
);


extern void cl_ltp_fun(
    Word16 y1[],         /* o   : Filtered adaptive excitation           Q0 */
    Word16 xn[],         /* i   : Target vector for pitch search         Q0 */
    Word16 xn2[],        /* o   : Target vector for codebook search      Q0 */
    Word16 res2[]       /* i/o : Long term prediction residual          Q0 */
);



int Pitch_fr_init (Pitch_frState *st);
/* initialize one instance of the pre processing state.
   Stores pointer to filter status struct in *st. This pointer has to
   be passed to Pitch_fr in each call.
   returns 0 on success
 */

int Pitch_fr_reset (Pitch_frState *st);
/* reset of pre processing state (i.e. set state memory to zero)
   returns 0 on success
 */

//void Pitch_fr_exit (Pitch_frState *st);
/* de-initialize pre processing state (i.e. free status struct)
   stores NULL in *st
 */

Word16 Pitch_fr (        /* o   : pitch period (integer)                    */
    Pitch_frState *st,   /* i/o : State struct                              */
    enum Mode mode,      /* i   : codec mode                                */
    Word16 T_op[],       /* i   : open loop pitch lags                      */
    Word16 exc[],        /* i   : excitation buffer                         */
    Word16 xn[],         /* i   : target vector                             */
    Word16 h[],          /* i   : impulse response of synthesis and
                                  weighting filters                         */
    Word16 L_subfr,      /* i   : Length of subframe                        */
    Word16 i_subfr,      /* i   : subframe offset                           */
    Word16 *pit_frac,    /* o   : pitch period (fractional)                 */
    Word16 *resu3,       /* o   : subsample resolution 1/3 (=1) or 1/6 (=0) */
    Word16 *ana_index    /* o   : index of encoding                         */
);


extern Word32 Norm_corr_fun1 (  /* o   : filtered excitation energy (32bit)        */
    Word16 excf[],       /* i   : filtered excitation                       */
    Word16 scal_excf[],  /* o   : scaled excitation                         */
    Word16 len           /* i   : process length                            */
);


extern void Norm_corr_fun2 (
    Word16 s_excf[],     /* i   : scaled excitation                         */
    Word16 xn[],         /* i   : target vector                             */
    Word16 *corr_norm,   /* o   : normalized correlation                    */
    Word16 len           /* i   : process length                            */
);


extern Word16 Norm_corr_fun3 (
    Word16 exc[],        /* i   : excitation buffer                         */
    Word16 xn[],         /* i   : target vector                             */
    Word16 s_excf[],     /* i   : scaled excitation                         */
    Word16 idx           /* i   : index                                     */
);


extern void Convolve (
    Word16 x[],        /* (i)  : input vector                               */
    Word16 h[],        /* (i)  : impulse response                           */
    Word16 y[],        /* (o)  : output vector                              */
    Word16 L           /* (i)  : vector size                                */
);


extern Word32 Inv_sqrt (      /* (o) : output value   (range: 0<=val<1)            */
    Word32 L_x                /* (i) : input value    (range: 0<=val<=7fffffff)    */
);


Word16 Enc_lag3(          /* o  : Return index of encoding     */
    Word16 T0,            /* i  : Pitch delay                          */
    Word16 T0_frac,       /* i  : Fractional pitch delay               */
	 Word16 T0_prev,      /* i  : Integer pitch delay of last subframe */
	 Word16 T0_min,       /* i  : minimum of search range              */
	 Word16 T0_max,       /* i  : maximum of search range              */
    Word16 delta_flag,    /* i  : Flag for 1st (or 3rd) subframe       */
	 Word16 flag4         /* i  : Flag for encoding with 4 bits        */
);


Word16 Enc_lag6 (        /* o  : Return index of encoding             */
    Word16 T0,           /* i  : Pitch delay                          */
    Word16 T0_frac,      /* i  : Fractional pitch delay               */
	 Word16 T0_min,      /* i  : minimum of search range              */
    Word16 delta_flag    /* i  : Flag for 1st (or 3rd) subframe       */
);
 

Word16 Interpol_3or6 (  /* (o)  : interpolated value                        */
    Word16 *x,          /* (i)  : input vector                              */
    Word16 frac,        /* (i)  : fraction  (-2..2 for 3*, -3..3 for 6*)    */
    Word16 flag3        /* (i)  : if set, upsampling rate = 3 (6 otherwise) */
);


extern void Pred_lt_3or6 (
    Word16 exc[],     /* in/out: excitation buffer                         */
    Word16 T0,        /* input : integer pitch lag                         */
    Word16 frac,      /* input : fraction of lag                           */
    Word16 flag3      /* input : if set, upsampling rate = 3 (6 otherwise) */
);


Word16 G_pitch     (    /* o : Gain of pitch lag saturated to 1.2       */
    enum Mode mode,     /* i : AMR mode                                 */
    Word16 xn[],        /* i : Pitch target.                            */
    Word16 y1[],        /* i : Filtered adaptive codebook.              */
    Word16 g_coeff[],   /* i : Correlations need for gain quantization.
                               (7.4 only). Pass NULL if not needed      */
    Word16 L_subfr      /* i : Length of subframe.                      */
);

extern Word16 G_pitch_common (  /* o : Gain of pitch lag saturated to 1.2       */
    Word16 xn[],        /* i : Pitch target.                            */
    Word16 y1[],        /* i : Filtered adaptive codebook.              */
    Word16 g_coeff[],   /* i : Correlations need for gain quantization.
                               (7.4 only). Pass NULL if not needed      */
    Word16 L_subfr      /* i : Length of subframe.                      */
);


Word16 check_gp_clipping(tonStabState *st, /* i/o : State struct            */
                         Word16 g_pitch    /* i   : pitch gain              */
);


extern Word16 Interpol_3or6 (  /* o : interpolated value                        */
    Word16 *x,          /* i : input vector                              */
    Word16 frac,        /* i : fraction  (-2..2 for 3*, -3..3 for 6*)    */
    Word16 flag3        /* i : if set, upsampling rate = 3 (6 otherwise) */
);




#endif


#endif
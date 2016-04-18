/*
********************************************************************************
*
*      GSM AMR-NB speech codec   REV1.0   May 25, 2004
*
*      Modified by  Eric Huang 
********************************************************************************
*
*      File             : STP.h
*      Purpose          : Short-term Prediction of input speech.
*
********************************************************************************
*/
#ifndef STP_h
#define STP_h

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
*                         DEFINITION OF DATA TYPES
********************************************************************************
*/
typedef struct {
  Word16 old_A[M + 1];     /* Last A(z) for case of unstable filter */
} LevinsonState;

typedef struct {
   LevinsonState levinsonSt;
} lpcState;

typedef struct {
    Word16 past_rq[M];    /* Past quantized prediction error, Q15 */

} Q_plsfState;

typedef struct {

   /* Past LSPs */
   Word16 lsp_old[M];
   Word16 lsp_old_q[M];

   /* Quantization state */
   Q_plsfState qSt;

} lspState;



/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/

extern int Pre_Process (
    Word16 signal[],   /* Input/output signal                               */
    Word16 lg          /* Lenght of signal                                  */
);


int lpc_init (lpcState *st);
/* initialize one instance of the pre processing state.
   Stores pointer to filter status struct in *st. This pointer has to
   be passed to lpc in each call.
   returns 0 on success
 */
 
int lpc_reset (lpcState *st);
/* reset of pre processing state (i.e. set state memory to zero)
   returns 0 on success
 */


int lpc(
    lpcState *st,     /* i/o: State struct                */
    enum Mode mode,   /* i  : coder mode                  */
    Word16 x[],       /* i  : Input signal           Q15  */
    Word16 x_12k2[],  /* i  : Input signal (EFR)     Q15  */
    Word16 a[]        /* o  : predictor coefficients Q12  */
);


/*
**************************************************************************
*
*  Function    : autocorr
*  Purpose     : Compute autocorrelations of signal with windowing
*  Description : - Windowing of input speech:   s'[n] = s[n] * w[n]
*                - Autocorrelations of input speech:
*                  r[k] = sum_{i=k}^{239} s'[i]*s'[i-k]    k=0,...,10
*                The autocorrelations are expressed in normalized
*                double precision format.
*  Returns     : Autocorrelation
*
**************************************************************************
*/
extern Word16 Autocorr (
    Word16 x[],        /* (i)    : Input signal (L_WINDOW)             */
    Word16 r_h[],      /* (o)    : Autocorrelations  (msb)  (MP1)      */
    Word16 r_l[],      /* (o)    : Autocorrelations  (lsb)  (MP1)      */
    const Word16 wind[]/* (i)    : window for LPC analysis. (L_WINDOW) */
);


extern void Lag_window (
    Word16 m,          /* (i)    : LPC order                                */
    Word16 r_h[],      /* (i/o)  : Autocorrelations  (msb)                  */
    Word16 r_l[]       /* (i/o)  : Autocorrelations  (lsb)                  */
);


int Levinson_init (LevinsonState *st);
/* initialize one instance of the pre processing state.
   Stores pointer to filter status struct in *st. This pointer has to
   be passed to Levinson in each call.
   returns 0 on success
 */
 
int Levinson_reset (LevinsonState *st);
/* reset of pre processing state (i.e. set state memory to zero)
   returns 0 on success
 */

int Levinson (
    LevinsonState *st,
    Word16 Rh[],       /* i : Rh[m+1] Vector of autocorrelations (msb) */
    Word16 Rl[],       /* i : Rl[m+1] Vector of autocorrelations (lsb) */
    Word16 A[],        /* o : A[m]    LPC coefficients  (m = 10)       */
    Word16 rc[]        /* o : rc[4]   First 4 reflection coefficients  */
);



/*
**************************************************************************
*
*  Function    : lsp_init
*  Purpose     : Allocates memory and initializes state variables
*  Description : Stores pointer to filter status struct in *st. This
*                pointer has to be passed to lsp in each call.
*  Returns     : 0 on success
*
**************************************************************************
*/ 
int lsp_init (lspState *st);
 
/*
**************************************************************************
*
*  Function    : lsp_reset
*  Purpose     : Resets state memory
*  Returns     : 0 on success
*
**************************************************************************
*/
int lsp_reset (lspState *st);


/*
**************************************************************************
*
*  Function    : lsp
*  Purpose     : Conversion from LP coefficients to LSPs.
*                Quantization of LSPs.
*  Description : Generates 2 sets of LSPs from 2 sets of
*                LP coefficients for mode 12.2. For the other
*                modes 1 set of LSPs is generated from 1 set of
*                LP coefficients. These LSPs are quantized with
*                Matrix/Vector quantization (depending on the mode)
*                and interpolated for the subframes not yet having
*                their own LSPs.
*                
**************************************************************************
*/
int lsp(lspState *st,        /* i/o : State struct                            */
        enum Mode req_mode,  /* i   : requested coder mode                    */
        enum Mode used_mode, /* i   : used coder mode                         */        
        Word16 az[],         /* i/o : interpolated LP parameters Q12          */
        Word16 azQ[],        /* o   : quantization interpol. LP parameters Q12*/
        Word16 lsp_new[],    /* o   : new lsp vector                          */ 
        Word16 **anap        /* o   : analysis parameters                     */
        );


/*
**************************************************************************
*
*  Function    : Az_lsp
*  Purpose     : Compute the LSPs from the LP coefficients
*  Description : - The sum and difference filters are computed
*                  and divided by 1+z^{-1} and 1-z^{-1}, respectively.
*
*                  f1[i] = a[i] + a[11-i] - f1[i-1] ;   i=1,...,5
*                  f2[i] = a[i] - a[11-i] + f2[i-1] ;   i=1,...,5
*
*                - The roots of F1(z) and F2(z) are found using
*                  Chebyshev polynomial evaluation. The polynomials
*                  are evaluated at 60 points regularly spaced in the
*                  frequency domain. The sign change interval is
*                  subdivided 4 times to better track the root. The
*                  LSPs are found in the cosine domain [1,-1].
*
*                - If less than 10 roots are found, the LSPs from
*                  the past frame are used.
*  Returns     : void
*
**************************************************************************
*/
extern void Az_lsp (
    Word16 a[],        /* (i)  : predictor coefficients (MP1)              */
    Word16 lsp[],      /* (o)  : line spectral pairs (M)                   */
    Word16 old_lsp[]   /* (i)  : old lsp[] (in case not found 10 roots) (M)*/
);


/*
**************************************************************************
*
*  Function    : Int_lpc_1to3
*  Purpose     : Interpolates the LSPs and converts to LPC parameters 
*                to get a different LP filter in each subframe.
*  Description : The 20 ms speech frame is divided into 4 subframes.
*                The LSPs are quantized and transmitted at the 4th 
*                subframes (once per frame) and interpolated at the
*                1st, 2nd and 3rd subframe.
*
*                      |------|------|------|------|
*                         sf1    sf2    sf3    sf4
*                   F0                          F1
*      
*                 sf1:   3/4 F0 + 1/4 F1         sf3:   1/4 F0 + 3/4 F1
*                 sf2:   1/2 F0 + 1/2 F1         sf4:       F1
*  Returns     : void
*
**************************************************************************
*/
void Int_lpc_1to3 (
    Word16 lsp_old[], /* i : LSP vector at the 4th SF of past frame (M)      */
    Word16 lsp_new[], /* i : LSP vector at the 4th SF of present frame (M)   */
    Word16 Az[]       /* o : interpolated LP parameters in all SFs (AZ_SIZE) */
);
 
/*
**************************************************************************
*
*  Function    : Int_lpc_1to3_2
*  Purpose     : Interpolation of the LPC parameters. Same as the Int_lpc
*                function but we do not recompute Az() for subframe 4
*                because it is already available.
*  Returns     : void
*
**************************************************************************
*/
void Int_lpc_1to3_2 (
    Word16 lsp_old[],  /* i : LSP vector at the 4th SF of past frame (M) */
    Word16 lsp_new[],  /* i : LSP vector at the 4th SF present frame (M) */
    Word16 Az[]        /* o :interpolated LP parameters in SFs 1, 2, 3 
                             (AZ_SIZE)                                   */
);



void Lsp_Az (
    Word16 lsp[],      /* (i)    : line spectral frequencies                */
    Word16 a[]         /* (o)    : predictor coefficients (order = 10)      */
);



extern void Lsf_lsp (
    Word16 lsf[],      /* (i)    : lsf[m] normalized (range: 0.0<=val<=0.5) */
    Word16 lsp[],      /* (o)    : lsp[m] (range: -1<=val<1)                */
    Word16 m           /* (i)    : LPC order                                */
);
extern void Lsp_lsf (
    Word16 lsp[],      /* (i)    : lsp[m] (range: -1<=val<1)                */
    Word16 lsf[],      /* (o)    : lsf[m] normalized (range: 0.0<=val<=0.5) */
    Word16 m           /* (i)    : LPC order                                */
);


extern void Lsf_wt (
    Word16 *lsf,         /* input : LSF vector                  */
    Word16 *wf);         /* output: square of weighting factors */


void Reorder_lsf (
    Word16 *lsf,       /* (i/o)  : vector of LSFs   (range: 0<=val<=0.5)    */
    Word16 min_dist,   /* (i)    : minimum required distance                */
    Word16 n           /* (i)    : LPC order                                */
);



int Q_plsf_init (Q_plsfState *st);
/* initialize one instance of the state.
   Stores pointer to filter status struct in *st. This pointer has to
   be passed to Q_plsf_5 / Q_plsf_3 in each call.
   returns 0 on success
 */

int Q_plsf_reset (Q_plsfState *st);
/* reset of state (i.e. set state memory to zero)
   returns 0 on success
 */

//void Q_plsf_exit (Q_plsfState *st);
/* de-initialize state (i.e. free status struct)
   stores NULL in *st
 */

void Q_plsf_3(
    Q_plsfState *st,    /* i/o: state struct                             */
    enum Mode mode,     /* i  : coder mode                               */
    Word16 *lsp1,       /* i  : 1st LSP vector                      Q15  */
    Word16 *lsp1_q,     /* o  : quantized 1st LSP vector            Q15  */
    Word16 *indice,     /* o  : quantization indices of 3 vectors   Q0   */
    Word16 *pred_init_i /* o  : init index for MA prediction in DTX mode */
);


extern Word16
Vq_search4(             /* o: quantization index,            Q0  */
    Word16 * lsf_r1,    /* i: 1st LSF residual vector,       Q15 */
    Word16 * dico,      /* i: quantization codebook,         Q15 */
    Word16 * wf1,       /* i: 1st LSF weighting factors,     Q13 */
    Word16 dico_size    /* i: size of quantization codebook, Q0  */
);


extern Word16
Vq_search3(             /* o: quantization index,            Q0  */
    Word16 * lsf_r1,    /* i: 1st LSF residual vector,       Q15 */
    Word16 * dico,      /* i: quantization codebook,         Q15 */
    Word16 * wf1,       /* i: 1st LSF weighting factors,     Q13 */
    Word16 dico_size    /* i: size of quantization codebook, Q0  */
);


extern Word16
Vq_search3_1(           /* o: quantization index,            Q0  */
    Word16 * lsf_r1,    /* i: 1st LSF residual vector,       Q15 */
    Word16 * dico,      /* i: quantization codebook,         Q15 */
    Word16 * wf1,       /* i: 1st LSF weighting factors,     Q13 */
    Word16 dico_size    /* i: size of quantization codebook, Q0  */
);


#endif


#endif
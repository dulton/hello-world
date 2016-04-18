/*
********************************************************************************
*
*      GSM AMR-NB speech codec   REV1.0   May 25, 2004
*
*      Modified by  Eric Huang 
********************************************************************************
*
*      File             : O_LTP.h
*      Purpose          : Compute the open loop pitch lag.
*
********************************************************************************
*/
#ifndef ol_ltp_h
#define ol_ltp_h
 
#if	defined(AMR_RP_FW) 
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "mode.h"

/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
int pre_big(
    enum Mode mode,            /* i  : coder mode                             */
    const Word16 gamma1[],     /* i  : spectral exp. factor 1                 */
    const Word16 gamma2[],     /* i  : spectral exp. factor 2                 */
    Word16 A_t[],              /* i  : A(z) unquantized, for 4 subframes, Q12 */
    Word16 frameOffset,        /* i  : Start position in speech vector,   Q0  */
    Word16 speech[],           /* i  : speech,                            Q0  */
    Word16 mem_w[],            /* i/o: synthesis filter memory state,     Q0  */
    Word16 wsp[]               /* o  : weighted speech                    Q0  */
);


extern void Weight_Ai (
    Word16 a[],        /* (i)  : a[m+1]  LPC coefficients   (m=10)          */
    const Word16 fac[],/* (i)  : Spectral expansion factors.                */
    Word16 a_exp[]     /* (o)  : Spectral expanded LPC coefficients         */
);


extern void Syn_filter (
    Word16 a[],        /* (i)  : a[m+1] prediction coefficients   (m=10)    */
    Word16 x[],        /* (i)  : input signal                               */
    Word16 y[],        /* (o)  : output signal                              */
    Word16 mem[]       /* (i/o)   : memory associated with this filtering.  */
);


extern void Residu (
    Word16 a[],        /* (i)  : prediction coefficients                    */
    Word16 x[],        /* (i)  : speech signal                              */
    Word16 y[],        /* (o)  : residual signal                            */
    Word16 lg          /* (i)  : size of filtering                          */
);


int ol_ltp(
    enum Mode mode,       /* i   : coder mode                              */
    Word16 wsp[],         /* i   : signal used to compute the OL pitch, Q0 */
                          /*       uses signal[-pit_max] to signal[-1]     */
    Word16 *T_op,         /* o   : open loop pitch lag,                 Q0 */
    Word16 old_lags[],    /* i   : history with old stored Cl lags         */
    Word16 ol_gain_flg[], /* i   : OL gain flag                            */
    Word16 idx,           /* i   : index                                   */
    Flag dtx              /* i   : dtx flag; use dtx=1, do not use dtx=0   */
);


Word16 Pitch_ol (      /* o   : open loop pitch lag                         */
    enum Mode mode,    /* i   : coder mode                                  */
    Word16 signal[],   /* i   : signal used to compute the open loop pitch  */
                       /*    signal[-pit_max] to signal[-1] should be known */
    Word16 pit_min,    /* i   : minimum pitch lag                           */
    Word16 pit_max,    /* i   : maximum pitch lag                           */
    Word16 L_frame,    /* i   : length of frame to compute pitch            */
    Word16 idx,        /* i   : frame index                                 */
    Flag dtx           /* i   : dtx flag; use dtx=1, do not use dtx=0       */
    );

extern Word16 Lag_max ( /* o   : lag found                               */
    Word32 corr[],      /* i   : correlation vector.                     */
    Word16 scal_sig[],  /* i   : scaled signal.                          */
    Word16 *cor_max     /* o   : normalized correlation of selected lag  */
    );


/*************************************************************************
 *
 *  FUNCTION: comp_corr
 *
 *  PURPOSE: Calculate all correlations of scal_sig[] in a given delay
 *           range.
 *
 *  DESCRIPTION:
 *      The correlation is given by
 *           cor[t] = <scal_sig[n], scal_sig[n-t]>,  t=lag_min,...,lag_max
 *      The functions outputs all correlations in the given range
 *
 *************************************************************************/
extern void comp_corr (Word16 scal_sig[],  /* i   : scaled signal.                     */
                Word32 corr[]              /* o   : correlation of selected lag        */
                );
                
                
                
                

#endif


#endif

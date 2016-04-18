/*
********************************************************************************
*
*      GSM AMR-NB speech codec   REV1.0   May 25, 2004
*
*      Modified by  Eric Huang 
********************************************************************************
*
*      File             : sprepro.h
*      Purpose          : Subframe preprocessing
*
********************************************************************************
*/
#ifndef spreproc_h
#define spreproc_h

#if	defined(AMR_RP_FW)
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "cnst.h"
#include "typedef.h"
#include "mode.h"

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


extern void Copy (
    const Word16 x[],  /* i : input vector (L)    */
    Word16 y[],        /* o : output vector (L)   */
    Word16 L           /* i : vector length       */
);

/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
int subframePreProc(
    enum Mode mode,            /* i  : coder mode                            */
    const Word16 gamma1[],     /* i  : spectral exp. factor 1                */
    const Word16 gamma1_12k2[],/* i  : spectral exp. factor 1 for EFR        */    
    const Word16 gamma2[],     /* i  : spectral exp. factor 2                */
    Word16 *A,                 /* i  : A(z) unquantized for the 4 subframes  */
    Word16 *Aq,                /* i  : A(z)   quantized for the 4 subframes  */
    Word16 *speech,            /* i  : speech segment                        */
    Word16 *mem_err,           /* i  : pointer to error signal               */
    Word16 *mem_w0,            /* i  : memory of weighting filter            */
    Word16 *zero,              /* i  : pointer to zero vector                */
    Word16 ai_zero[],          /* o  : history of weighted synth. filter     */
    Word16 exc[],              /* o  : long term prediction residual         */
    Word16 h1[],               /* o  : impulse response                      */
    Word16 xn[],               /* o  : target vector for pitch search        */
    Word16 res2[],             /* o  : long term prediction residual         */
    Word16 error[]             /* o  : error of LPC synthesis filter         */
);

#endif


#endif
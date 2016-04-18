/*
********************************************************************************
*
*      GSM AMR-NB speech codec   REV1.0   May 26, 2004
*
*      Modified by  Eric Huang 
********************************************************************************
*
*      File             : common.h
*      Purpose          : Common functions for AMR encoder
*
********************************************************************************
*/
#ifndef common_h
#define common_h
 
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
 
/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
Word32 Pow2 (          /* (o) : result       (range: 0<=val<=0x7fffffff)    */
    Word16 exponent,   /* (i) : Integer part.      (range: 0<=val<=30)      */
    Word16 fraction    /* (i) : Fractional part.  (range: 0.0<=val<1.0)     */
); 

void Log2 (
    Word32 L_x,        /* (i) : input value                                 */
    Word16 *exponent,  /* (o) : Integer part of Log2.   (range: 0<=val<=30) */
    Word16 *fraction   /* (o) : Fractional part of Log2. (range: 0<=val<1)*/
);

void Log2_norm (
    Word32 L_x,         /* (i) : input value (normalized)                    */
    Word16 exp,         /* (i) : norm_l (L_x)                                */
    Word16 *exponent,   /* (o) : Integer part of Log2.   (range: 0<=val<=30) */
    Word16 *fraction    /* (o) : Fractional part of Log2. (range: 0<=val<1)  */
); 
 
 
Word32 Mac_32 (Word32 L_32, Word16 hi1, Word16 lo1, Word16 hi2, Word16 lo2);

Word32 Mac_32_16 (Word32 L_32, Word16 hi, Word16 lo, Word16 n);

Word32 sqrt_l_exp (/* o : output value,                        Q31 */
    Word32 L_x,    /* i : input value,                         Q31 */
    Word16 *exp    /* o : right shift to be applied to result, Q0  */
);

void set_sign(Word16 dn[],   /* i/o : correlation between target and h[]    */
              Word16 sign[], /* o   : sign of dn[]                          */
              Word16 dn2[],  /* o   : maximum of correlation in each track. */
              Word16 n       /* i   : # of maximum correlations in dn2[]    */
); 
 
extern void Copy (
    const Word16 x[],  /* i : input vector (L)    */
    Word16 y[],        /* o : output vector (L)   */
    Word16 L           /* i : vector length       */
);


extern void CopyByte (
    const UWord8 x[],  /* i : input vector (L)    */
    UWord8 y[],        /* o : output vector (L)   */
    Word16 L           /* i : vector length       */
);



extern void cor_h_x (
    Word16 h[],     /* (i) : impulse response of weighted synthesis filter */
    Word16 x[],     /* (i) : target                                        */
    Word16 dn[],    /* (o) : correlation between target and h[]            */
    Word16 sf       /* (i) : scaling factor: 2 for 12.2, 1 for 7.4         */
);


extern void cor_h (
    Word16 h[],     /* (i) : impulse response of weighted synthesis filter */
    Word16 sign[],      /* (i) : sign of d[n]                              */
    Word16 rr[][L_CODE] /* (o) : matrix of autocorrelation                 */
);


extern void Convolve (
    Word16 x[],        /* (i)  : input vector                               */
    Word16 h[],        /* (i)  : impulse response                           */
    Word16 y[],        /* (o)  : output vector                              */
    Word16 L           /* (i)  : vector size                                */
);


extern Word32 Inv_sqrt (      /* (o) : output value   (range: 0<=val<1)            */
    Word32 L_x           /* (i) : input value    (range: 0<=val<=7fffffff)    */
);

extern void Set_zero (
    Word16 x[],        /* (o)  : vector to clear                            */
    Word16 L           /* (i)  : length of vector                           */
); 
 
 
extern void L_Extract (Word32 L_32, Word16 *hi, Word16 *lo);
extern Word32 L_Comp (Word16 hi, Word16 lo);
extern Word32 Mpy_32 (Word16 hi1, Word16 lo1, Word16 hi2, Word16 lo2);
extern Word32 Mpy_32_16 (Word16 hi, Word16 lo, Word16 n);
extern Word32 Div_32 (Word32 L_num, Word16 denom_hi, Word16 denom_lo);
 
#endif


#endif
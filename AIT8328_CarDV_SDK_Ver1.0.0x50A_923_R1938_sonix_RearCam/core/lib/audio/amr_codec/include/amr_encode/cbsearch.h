/*
********************************************************************************
*
*      GSM AMR-NB speech codec   REV1.0   May 26, 2004
*
*      Modified by  Eric Huang 
********************************************************************************
*
*      File             : cbsearch.h
*      Purpose          : Inovative codebook search (find index and gain)
*
*****************************************************************************
*/
#ifndef cbsearch_h
#define cbsearch_h
 
#if	defined(AMR_RP_FW)
 
/*
*****************************************************************************
*                         INCLUDE FILES                               
*****************************************************************************
*/                                                                    
#include "typedef.h"       
#include "mode.h"                                                                            
                                                                      
/*                                                                    
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES                  
*****************************************************************************
*/                                                                    
                                                                      
/*                                                                    
*****************************************************************************
*                         DEFINITION OF DATA TYPES                    
*****************************************************************************
*/
 
/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
int cbsearch(Word16 x[],     /* i : target vector, Q0                      */
             Word16 h[],     /* i : impulse response of weighted synthesis */
                             /*     filter h[-L_subfr..-1] must be set to  */
                             /*    zero. Q12                               */
             Word16 T0,      /* i : Pitch lag                              */
             Word16 pitch_sharp, /* i : Last quantized pitch gain, Q14     */
             Word16 gain_pit,/* i : Pitch gain, Q14                        */ 
             Word16 res2[],  /* i : Long term prediction residual, Q0      */
             Word16 code[],  /* o : Innovative codebook, Q13               */
             Word16 y[],     /* o : filtered fixed codebook excitation, Q12 */
             Word16 **anap,  /* o : Signs of the pulses                    */
             enum Mode mode, /* i : coder mode                             */
             Word16 subNr)   /* i : subframe number                        */
;


Word16 code_2i40_9bits(
    Word16 subNr,       /* i : subframe number                               */
    Word16 x[],         /* i : target vector                                 */
    Word16 h[],         /* i : impulse response of weighted synthesis filter */
                        /*     h[-L_subfr..-1] must be set to zero.          */
    Word16 T0,          /* i : Pitch lag                                     */
    Word16 pitch_sharp, /* i : Last quantized pitch gain                     */
    Word16 code[],      /* o : Innovative codebook                           */
    Word16 y[],         /* o : filtered fixed codebook excitation            */
    Word16 * sign       /* o : Signs of 2 pulses                             */
);

extern void build_code_func475(
    Word16  *Ptr0,
    Word16  *Ptr1,
    Word16  _sign[],
    Word16  y[]        /* o : filtered innovative code                      */    
);



#endif


#endif
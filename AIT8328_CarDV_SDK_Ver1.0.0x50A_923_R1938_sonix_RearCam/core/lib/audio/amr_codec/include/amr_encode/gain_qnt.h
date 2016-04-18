/*
********************************************************************************
*
*      GSM AMR-NB speech codec   REV1.0   May 26, 2004
*
*      Modified by  Eric Huang 
********************************************************************************
*
*      File             : gainQuant.h
*      Purpose          : Quantazation of gains
*
********************************************************************************
*/
#ifndef gain_q_h
#define gain_q_h
 
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
#define LTPG_MEM_SIZE 5 /* number of stored past LTP coding gains + 1 */

typedef struct {
    Word16 onset;                   /* onset state,                   Q0  */
    Word16 prev_alpha;              /* previous adaptor output,       Q15 */
    Word16 prev_gc;                 /* previous code gain,            Q1  */

    Word16 ltpg_mem[LTPG_MEM_SIZE]; /* LTP coding gain history,       Q13 */
                                    /* (ltpg_mem[0] not used for history) */
} GainAdaptState;

typedef struct {
   Word16 past_qua_en[4];         /* normal MA predictor memory,         Q10 */
                                  /* (contains 20*log10(qua_err))            */
   Word16 past_qua_en_MR122[4];   /* MA predictor memory for MR122 mode, Q10 */
                                  /* (contains log2(qua_err))                */
} gc_predState;


typedef struct {
    Word16 sf0_exp_gcode0;
    Word16 sf0_frac_gcode0;
    Word16 sf0_exp_target_en;
    Word16 sf0_frac_target_en;
    Word16 sf0_exp_coeff[5];
    Word16 sf0_frac_coeff[5];
    Word16 *gain_idx_ptr;
    
    gc_predState     gc_predSt;
    gc_predState     gc_predUnqSt;
    GainAdaptState   adaptSt;
} gainQuantState;
  

 
/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
int gainQuant_init (gainQuantState *st);
/* initialize one instance of the pre processing state.
   Stores pointer to filter status struct in *st. This pointer has to
   be passed to gainQuant in each call.
   returns 0 on success
 */
int gainQuant_reset (gainQuantState *st);
/* reset of pre processing state (i.e. set state memory to zero)
   returns 0 on success
 */


int gainQuant(
    gainQuantState *st,   /* i/o : State struct                      */
    enum Mode mode,       /* i   : coder mode                        */
    Word16 res[],         /* i   : LP residual,                 Q0   */
    Word16 exc[],         /* i   : LTP excitation (unfiltered), Q0   */
    Word16 code[],        /* i   : CB innovation (unfiltered),  Q13  */
                          /*       (unsharpened for MR475)           */
    Word16 xn[],          /* i   : Target vector.                    */
    Word16 xn2[],         /* i   : Target vector.                    */
    Word16 y1[],          /* i   : Adaptive codebook.                */
    Word16 Y2[],          /* i   : Filtered innovative vector.       */
    Word16 g_coeff[],     /* i   : Correlations <xn y1> <y1 y1>      */
                          /*       Compute in G_pitch().             */
    Word16 even_subframe, /* i   : even subframe indicator flag      */
    Word16 gp_limit,      /* i   : pitch gain limit                  */
    Word16 *sf0_gain_pit, /* o   : Pitch gain sf 0.   MR475          */
    Word16 *sf0_gain_cod, /* o   : Code gain sf 0.    MR475          */
    Word16 *gain_pit,     /* i/o : Pitch gain.                       */
    Word16 *gain_cod,     /* o   : Code gain.                        */
                          /*       MR475: gain_* unquantized in even */
                          /*       subframes, quantized otherwise    */
    Word16 **anap         /* o   : Index of quantization             */
);


int gc_pred_init (gc_predState *st);
/* initialize one instance of codebook gain MA predictor
   Stores pointer to state struct in *st. This pointer has to
   be passed to gc_pred in each call.
   returns 0 on success
 */
 
int gc_pred_reset (gc_predState *st);
/* reset of codebook gain MA predictor state (i.e. set state memory to zero)
   returns 0 on success
 */
//void gc_pred_exit (gc_predState **st);
/* de-initialize codebook gain MA predictor state (i.e. free state struct)
   stores NULL in *st
 */

void
gc_pred_copy(
    gc_predState *st_src,  /* i : State struct                           */
    gc_predState *st_dest  /* o : State struct                           */
);

/*************************************************************************
 *
 * FUNCTION:  gc_pred()
 *
 * PURPOSE: MA prediction of the innovation energy
 *          (in dB/(20*log10(2))) with mean  removed).
 *
 *************************************************************************/
void gc_pred(
    gc_predState *st,   /* i/o: State struct                           */
    enum Mode mode,     /* i  : AMR mode                               */
    Word16 *code,       /* i  : innovative codebook vector (L_SUBFR)   */
                        /*      MR122: Q12, other modes: Q13           */
    Word16 *exp_gcode0, /* o  : exponent of predicted gain factor, Q0  */
    Word16 *frac_gcode0,/* o  : fraction of predicted gain factor  Q15 */
    Word16 *exp_en,     /* o  : exponent of innovation energy,     Q0  */
                        /*      (only calculated for MR795)            */
    Word16 *frac_en     /* o  : fraction of innovation energy,     Q15 */
                        /*      (only calculated for MR795)            */
);

/*************************************************************************
 *
 * FUNCTION:  gc_pred_update()
 *
 * PURPOSE: update MA predictor with last quantized energy
 *
 *************************************************************************/
void gc_pred_update(
    gc_predState *st,      /* i/o: State struct                     */
    Word16 qua_ener_MR122, /* i  : quantized energy for update, Q10 */
                           /*      (log2(qua_err))                  */
    Word16 qua_ener        /* i  : quantized energy for update, Q10 */
                           /*      (20*log10(qua_err))              */
);


extern Word32 calc_enerfy_common(
    Word16  len,    /* i : process length, must be even number            */
    Word16  in1[],  /* i : input vector1, address must be 4-byte aligned  */
    Word16  in2[],  /* i : input vector2, address must be 4-byte aligned  */
    Word32  acc     /* i : initial value for accumulation                 */    
);    


void calc_filt_energies(
    enum Mode mode,     /* i  : coder mode                                   */
    Word16 xn[],        /* i  : LTP target vector,                       Q0  */
    Word16 xn2[],       /* i  : CB target vector,                        Q0  */
    Word16 y1[],        /* i  : Adaptive codebook,                       Q0  */
    Word16 Y2[],        /* i  : Filtered innovative vector,              Q12 */
    Word16 g_coeff[],   /* i  : Correlations <xn y1> <y1 y1>                 */
                        /*      computed in G_pitch()                        */

    Word16 frac_coeff[],/* o  : energy coefficients (5), fraction part,  Q15 */
    Word16 exp_coeff[], /* o  : energy coefficients (5), exponent part,  Q0  */
    Word16 *cod_gain_frac,/* o: optimum codebook gain (fraction part),   Q15 */
    Word16 *cod_gain_exp  /* o: optimum codebook gain (exponent part),   Q0  */
);


void calc_target_energy(
    Word16 xn[],     /* i: LTP target vector,                       Q0  */
    Word16 *en_exp,  /* o: optimum codebook gain (exponent part),   Q0  */
    Word16 *en_frac  /* o: optimum codebook gain (fraction part),   Q15 */
);


extern Word32 calc_enerfy_common(
    Word16  len,    /* i : process length, must be even number            */
    Word16  in1[],  /* i : input vector1, address must be 4-byte aligned  */
    Word16  in2[],  /* i : input vector2, address must be 4-byte aligned  */
    Word32  acc     /* i : initial value for accumulation                 */    
);    


extern Word32 calc_enerfy_common1(
    Word16  len,    /* i : process length                                 */
    Word16  in1[],  /* i : input vector1                                  */
    Word16  in2[],  /* i : input vector2                                  */
    Word32  acc     /* i : initial value for accumulation                 */        
);    


/*************************************************************************
 *
 * FUNCTION:  Qua_gain()
 *
 * PURPOSE: Quantization of pitch and codebook gains.
 *          (using predicted codebook gain)
 *
 *************************************************************************/
Word16 Qua_gain(            /* o  : index of quantization.                 */   
    enum Mode mode,         /* i  : AMR mode                               */
    Word16 exp_gcode0,      /* i  : predicted CB gain (exponent),      Q0  */
    Word16 frac_gcode0,     /* i  : predicted CB gain (fraction),      Q15 */
    Word16 frac_coeff[],    /* i  : energy coeff. (5), fraction part,  Q15 */
    Word16 exp_coeff[],     /* i  : energy coeff. (5), exponent part,  Q0  */
                            /*      (frac_coeff and exp_coeff computed in  */
                            /*       calc_filt_energies())                 */
    Word16 gp_limit,        /* i  : pitch gain limit                       */
    Word16 *gain_pit,       /* o  : Pitch gain,                        Q14 */
    Word16 *gain_cod,       /* o  : Code gain,                         Q1  */
    Word16 *qua_ener_MR122, /* o  : quantized energy error,            Q10 */
                            /*      (for MR122 MA predictor update)        */
    Word16 *qua_ener        /* o  : quantized energy error,            Q10 */
                            /*      (for other MA predictor update)        */
);


extern Word16 qgain_search(
    Word16 gp_limit,          /* i  : pitch gain limit                       */
    Word32 coeff[], 
    const Word16 *tab_ptr,
    Word16 gcode0             /* i  : predicted CB gain (norm.),    Q14 */
);



int gain_adapt_init (GainAdaptState *st);
/* initialize one instance of the gain adaptor
   Stores pointer to state struct in *st. This pointer has to
   be passed to gain_adapt and gain_adapt_update in each call.
   returns 0 on success
 */
 
int gain_adapt_reset (GainAdaptState *st);
/* reset of gain adaptor state (i.e. set state memory to zero)
   returns 0 on success
 */


/*************************************************************************
 *
 *  Function:   gain_adapt()
 *  Purpose:    calculate pitch/codebook gain adaptation factor alpha
 *              (and update the adaptor state)
 *
 **************************************************************************
 */
void gain_adapt(
    GainAdaptState *st,  /* i  : state struct                  */
    Word16 ltpg,         /* i  : ltp coding gain (log2()), Q   */
    Word16 gain_cod,     /* i  : code gain,                Q13 */
    Word16 *alpha        /* o  : gain adaptation factor,   Q15 */
);


Word16 gmed_n (   /* o : index of the median value (0...N-1)      */
    Word16 ind[], /* i : Past gain values                         */
    Word16 n      /* i : The number of gains; this routine        */
                  /*     is only valid for a odd number of gains  */ 
);


/*************************************************************************
 *
 * FUNCTION:  MR475_update_unq_pred()
 *
 * PURPOSE:   use optimum codebook gain and update "unquantized"
 *            gain predictor with the (bounded) prediction error
 *
 *************************************************************************/
void 
MR475_update_unq_pred(
    gc_predState *pred_st, /* i/o: gain predictor state struct            */
    Word16 exp_gcode0,     /* i  : predicted CB gain (exponent),      Q0  */
    Word16 frac_gcode0,    /* i  : predicted CB gain (fraction),      Q15 */
    Word16 cod_gain_exp,   /* i  : optimum codebook gain (exponent),  Q0  */
    Word16 cod_gain_frac   /* i  : optimum codebook gain (fraction),  Q15 */
);

/*************************************************************************
 *
 * FUNCTION:  MR475_gain_quant()
 *
 * PURPOSE: Quantization of pitch and codebook gains for two subframes
 *          (using predicted codebook gain)
 *
 *************************************************************************/

Word16
MR475_gain_quant(              /* o  : index of quantization.                 */
    gc_predState *pred_st,     /* i/o: gain predictor state struct            */
      
                               /* data from subframe 0 (or 2) */
    Word16 sf0_exp_gcode0,     /* i  : predicted CB gain (exponent),      Q0  */
    Word16 sf0_frac_gcode0,    /* i  : predicted CB gain (fraction),      Q15 */
    Word16 sf0_exp_coeff[],    /* i  : energy coeff. (5), exponent part,  Q0  */
    Word16 sf0_frac_coeff[],   /* i  : energy coeff. (5), fraction part,  Q15 */
                               /*      (frac_coeff and exp_coeff computed in  */
                               /*       calc_filt_energies())                 */
    Word16 sf0_exp_target_en,  /* i  : exponent of target energy,         Q0  */
    Word16 sf0_frac_target_en, /* i  : fraction of target energy,         Q15 */
      
                               /* data from subframe 1 (or 3) */
    Word16 sf1_code_nosharp[], /* i  : innovative codebook vector (L_SUBFR)   */
                               /*      (whithout pitch sharpening)            */
    Word16 sf1_exp_gcode0,     /* i  : predicted CB gain (exponent),      Q0  */
    Word16 sf1_frac_gcode0,    /* i  : predicted CB gain (fraction),      Q15 */
    Word16 sf1_exp_coeff[],    /* i  : energy coeff. (5), exponent part,  Q0  */
    Word16 sf1_frac_coeff[],   /* i  : energy coeff. (5), fraction part,  Q15 */
                               /*      (frac_coeff and exp_coeff computed in  */
                               /*       calc_filt_energies())                 */
    Word16 sf1_exp_target_en,  /* i  : exponent of target energy,         Q0  */
    Word16 sf1_frac_target_en, /* i  : fraction of target energy,         Q15 */

    Word16 gp_limit,           /* i  : pitch gain limit                       */

    Word16 *sf0_gain_pit,      /* o  : Pitch gain,                        Q14 */
    Word16 *sf0_gain_cod,      /* o  : Code gain,                         Q1  */
      
    Word16 *sf1_gain_pit,      /* o  : Pitch gain,                        Q14 */
    Word16 *sf1_gain_cod       /* o  : Code gain,                         Q1  */
);


extern Word16 MR475Gain_search(
    const Word16 *TabPtr,     /* i  : Gain quantization table           */
    Word16 sf0_gcode0,        /* i  : Predicted codebook gain           */ 
    Word16 sf1_fcode0,        /* i  : Predicted codebook gain           */ 
    Word32 coeff[], 
    Word16 gp_limit           /* i  : pitch gain limit                  */
);



#endif


#endif
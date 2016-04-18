/*
********************************************************************************
*
*      GSM AMR-NB speech codec   REV1.0   May 25, 2004
*
*      Modified by  Eric Huang 
********************************************************************************
*
*      File             : coder.h
*      Purpose          : Encoder main program
*
*****************************************************************************
*/
#ifndef coder_h
#define coder_h 1

#if	defined(AMR_RP_FW)

/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include "typedef.h"
#include "cnst.h"
#include "STP.h"
#include "encframe.h"
#include "C_LTP.h"
#include "O_LTP.h"
#include "gain_qnt.h"
#include "mode.h"


/*
*****************************************************************************
*                         DEFINITION OF DATA TYPES
*****************************************************************************
*/

/*-----------------------------------------------------------*
 *    Coder constant parameters (defined in "enc_cnst.h")    *
 *-----------------------------------------------------------*
 *   L_WINDOW    : LPC analysis window size.                 *
 *   L_NEXT      : Samples of next frame needed for autocor. *
 *   L_FRAME     : Frame size.                               *
 *   L_FRAME_BY2 : Half the frame size.                      *
 *   L_SUBFR     : Sub-frame size.                           *
 *   M           : LPC order.                                *
 *   MP1         : LPC order+1                               *
 *   L_TOTAL7k4  : Total size of speech buffer.              *
 *   PIT_MIN7k4  : Minimum pitch lag.                        *
 *   PIT_MAX     : Maximum pitch lag.                        *
 *   L_INTERPOL  : Length of filter for interpolation        *
 *-----------------------------------------------------------*/
typedef struct {
   /* Speech vector */
   Word16 old_speech[L_TOTAL];
   Word16 *speech, *p_window, *p_window_12k2;
   Word16 *new_speech;             /* Global variable */
   
   /* Weight speech vector */
   Word16 old_wsp[L_FRAME + PIT_MAX];
   Word16 *wsp;

   /* OL LTP states */
   Word16 old_lags[5];
   Word16 ol_gain_flg[2];

   /* Excitation vector */
   Word16 old_exc[L_FRAME + PIT_MAX + L_INTERPOL];
   Word16 *exc;

   /* Zero vector */
   Word16 ai_zero[L_SUBFR + MP1];
   Word16 *zero;

   /* Impulse response vector */
   Word16 *h1;
   Word16 hvec[L_SUBFR * 2];

   /* Substates */
   lpcState   lpcSt;
   lspState   lspSt;
   clLtpState clLtpSt;
   gainQuantState  gainQuantSt;
//   pitchOLWghtState pitchOLWghtSt;
   tonStabState tonStabSt;
//   vadState vadSt;
   Flag dtx;
//   dtx_encState dtx_encSt;

   /* Filter's memory */
   Word16 mem_syn[M], mem_w0[M], mem_w[M];
   Word16 mem_err[M + L_SUBFR], *error;

   Word16 sharp;
} cod_amrState;



typedef struct{
//    Pre_ProcessState pre_state;
    cod_amrState   cod_amr_state;
    Flag dtx;
    int complexityCounter;   /* Only for complexity computation            */
} Speech_Encode_FrameState;

typedef struct {
    Word16 sid_update_rate;  /* Send SID Update every sid_update_rate frame */
    Word16 sid_update_counter; /* Number of frames since last SID          */
    Word16 sid_handover_debt;  /* Number of extra SID_UPD frames to schedule*/
    enum TXFrameType prev_ft;
} sid_syncState;
 
 


/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
void amr_init(void);
/* Initialisation of the coder.                                          */

int Speech_Encode_Frame_init (Speech_Encode_FrameState *st,Flag dtx,char *id);
                                           
/* initialize one instance of the speech encoder
   Stores pointer to filter status struct in *st. This pointer has to
   be passed to Speech_Encode_Frame in each call.
   returns 0 on success
 */

int Speech_Encode_Frame_reset (Speech_Encode_FrameState *st);
/* reset speech encoder (i.e. set state memory to zero)
   returns 0 on success
 */

//void Speech_Encode_Frame_exit (Speech_Encode_FrameState **st);
/* de-initialize speech encoder (i.e. free status struct)
   stores NULL in *s
 */

int Speech_Encode_Frame (
    Speech_Encode_FrameState *st, /* i/o : encoder states         */
    enum Mode mode,               /* i   : speech coder mode      */
    Word16 *new_speech,           /* i   : input speech           */
    Word16 *serial,               /* o   : serial bit stream      */
    enum Mode *usedMode           /* o   : used speech coder mode */
);
/*    return 0 on success
 */

#ifdef MMS_IO

Word16 PackBits(
    enum Mode used_mode,       /* i : actual AMR mode             */
    enum Mode mode,            /* i : requested AMR (speech) mode */
    enum TXFrameType fr_type,  /* i : frame type                  */
    Word16 bits[],             /* i : serial bits                 */
    UWord8 packed_bits[]       /* o : sorted&packed bits          */
);

#endif




/*
**************************************************************************
*
*  Function    : cod_amr_init
*  Purpose     : Allocates memory and initializes state variables
*  Description : Stores pointer to filter status struct in *st. This
*                pointer has to be passed to cod_amr in each call.
 *                 - initilize pointers to speech buffer
 *                 - initialize static  pointers
 *                 - set static vectors to zero
*  Returns     : 0 on success
*
**************************************************************************
*/ 
int cod_amr_init (cod_amrState *st, Flag dtx);
 
/*
**************************************************************************
*
*  Function    : cod_amr_reset
*  Purpose     : Resets state memory
*  Returns     : 0 on success
*
**************************************************************************
*/
int cod_amr_reset (cod_amrState *st);


/***************************************************************************
 *   FUNCTION:   cod_amr
 *
 *   PURPOSE:  Main encoder routine.
 *
 *   DESCRIPTION: This function is called every 20 ms speech frame,
 *       operating on the newly read 160 speech samples. It performs the
 *       principle encoding functions to produce the set of encoded parameters
 *       which include the LSP, adaptive codebook, and fixed codebook
 *       quantization indices (addresses and gains).
 *
 *   INPUTS:
 *       No input argument are passed to this function. However, before
 *       calling this function, 160 new speech data should be copied to the
 *       vector new_speech[]. This is a global pointer which is declared in
 *       this file (it points to the end of speech buffer minus 160).
 *
 *   OUTPUTS:
 *
 *       ana[]:     vector of analysis parameters.
 *       synth[]:   Local synthesis speech (for debugging purposes)
 *
 ***************************************************************************/

int cod_amr(cod_amrState *st,         /* i/o : State struct                 */
            enum Mode mode,           /* i   : AMR mode                     */
            Word16 new_speech[],      /* i   : speech input (L_FRAME)       */
            Word16 ana[],             /* o   : Analysis parameters          */
            enum Mode *usedMode,      /* o   : used mode                    */
            Word16 synth[]            /* o   : Local synthesis              */
);



Word16 encoder_homing_frame_test (Word16 input_frame[]);


void Prm2bits (
    enum Mode mode,    /* i : AMR mode */
    Word16 prm[],      /* input : analysis parameters                       */
    Word16 bits[]      /* output: serial bits                               */
);


int sid_sync_init (sid_syncState *st);
/* initialize one instance of the sid_sync module
   Stores pointer to state struct in *st. This pointer has to
   be passed to sid_sync in each call.
   returns 0 on success
 */
 
int sid_sync_reset (sid_syncState *st);
/* reset of sid_sync module (i.e. set state memory to zero)
   returns 0 on success
 */

 
void sid_sync(sid_syncState *st , /* i/o: sid_sync state      */
              enum Mode mode,
              enum TXFrameType *tx_frame_type); 


void update_gp_clipping(tonStabState *st, /* i/o : State struct            */
                        Word16 g_pitch    /* i   : pitch gain              */
);

Word16 check_lsp(tonStabState *st, /* i/o : State struct            */
                 Word16 *lsp       /* i   : unquantized LSP's       */
);

int ton_stab_reset (tonStabState *st);
/* reset of pre processing state (i.e. set state memory to zero)
   returns 0 on success
 */
 
int ton_stab_init (tonStabState *st);
/* initialize one instance of the pre processing state.
   Stores pointer to filter status struct in *st. This pointer has to
   be passed to ton_stab in each call.
   returns 0 on success
 */ 





#endif


#endif
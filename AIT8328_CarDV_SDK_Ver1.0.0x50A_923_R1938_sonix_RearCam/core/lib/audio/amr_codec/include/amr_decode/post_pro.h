#if	defined(AMR_RP_FW)
/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : post_pro.h
*      Purpose          : Postprocessing of output speech.
*
*                         - 2nd order high pass filtering with cut
*                           off frequency at 60 Hz.
*                         - Multiplication of output by two.
*
********************************************************************************
*/
#ifndef post_pro_h
#define post_pro_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
 
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
#ifndef Post_Proc32

typedef struct {
   Word16 y2_hi;
   Word16 y2_lo;
   Word16 y1_hi;
   Word16 y1_lo;
   Word16 x0;
   Word16 x1;
} Post_ProcessState;

#else

typedef struct {
	Word32	Y2;
	Word32	Y1;
	Word16	x0;
	Word16	x1;
} Post_ProcessState32;

#endif
/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
#ifndef Post_Proc32 
int Post_Process_init (Post_ProcessState *st);
#else
int Post_Process_init (Post_ProcessState32 *st);
#endif
/* initialize one instance of the Post processing state.
   Stores pointer to filter status struct in *st. This pointer has to
   be passed to Post_Process in each call.
   returns 0 on success
 */
#ifndef Post_Proc32
int Post_Process_reset (Post_ProcessState *st);
#else
int Post_Process_reset (Post_ProcessState32 *st);
#endif
/* reset of Post processing state (i.e. set state memory to zero)
   returns 0 on success
 */
#ifndef Post_Proc32
int Post_Process (
    Post_ProcessState *st,  /* i/o : post process state                   */
    Word16 signal[],        /* i/o : signal                               */
    Word16 lg               /* i   : lenght of signal                     */
    );
#else
int Post_Process (
    Post_ProcessState32 *st,  /* i/o : post process state                   */
    Word16 signal[],        /* i/o : signal                               */
    Word16 lg               /* i   : lenght of signal                     */
    );
#endif

#endif
#endif
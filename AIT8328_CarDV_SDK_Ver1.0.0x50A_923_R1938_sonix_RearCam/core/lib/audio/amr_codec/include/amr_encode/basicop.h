/*___________________________________________________________________________
 |                                                                           |
 |   Constants and Globals                                                   |
 |                                                                           |
 | $Id $
 |___________________________________________________________________________|
*/
#if	defined(AMR_RP_FW)

extern Flag Overflow;
extern Flag Carry;

#define MAX_32 (Word32)0x7fffffffL
#define MIN_32 (Word32)0x80000000L

#define MAX_16 (Word16)0x7fff
#define MIN_16 (Word16)0x8000

/*___________________________________________________________________________
 |                                                                           |
 |   Prototypes for basic arithmetic operators                               |
 |___________________________________________________________________________|
*/

extern Word16 add (Word16 var1, Word16 var2);    /* Short add,           1   */
extern Word16 sub (Word16 var1, Word16 var2);    /* Short sub,           1   */
extern Word16 abs_s (Word16 var1);               /* Short abs,           1   */
extern Word16 shl (Word16 var1, Word16 var2);    /* Short shift left,    1   */
extern Word16 shr (Word16 var1, Word16 var2);    /* Short shift right,   1   */
extern Word16 mult (Word16 var1, Word16 var2);   /* Short mult,          1   */
extern Word32 L_mult (Word16 var1, Word16 var2); /* Long mult,           1   */
extern Word16 negate (Word16 var1);              /* Short negate,        1   */
extern Word16 extract_h (Word32 L_var1);         /* Extract high,        1   */
//#define extract_h(L_var1)	(Word16)(L_var1>>16)
extern Word16 extract_l (Word32 L_var1);         /* Extract low,         1   */
//#e definextract_l(L_var1)	(Word16)(L_var1)
extern Word16 round (Word32 L_var1);             /* Round,               1   */
extern Word32 L_mac (Word32 L_var3, Word16 var1, Word16 var2);   /* Mac,  1  */
extern Word32 L_msu (Word32 L_var3, Word16 var1, Word16 var2);   /* Msu,  1  */
extern Word32 L_add (Word32 L_var1, Word32 L_var2);    /* Long add,        2 */
extern Word32 L_sub (Word32 L_var1, Word32 L_var2);    /* Long sub,        2 */
extern Word32 L_negate (Word32 L_var1);                /* Long negate,     2 */
extern Word16 mult_r (Word16 var1, Word16 var2);       /* Mult with round, 2 */
extern Word32 L_shl (Word32 L_var1, Word16 var2);      /* Long shift left, 2 */
//Word32 L_shr (Word32 L_var1, Word16 var2);      /* Long shift right, 2*/
Word16 shr_r (Word16 var1, Word16 var2);        /* Shift right with
                                                   round, 2           */
extern Word32 L_deposit_h (Word16 var1);        /* 16 bit var1 -> MSB,     2 */
extern Word32 L_deposit_l (Word16 var1);        /* 16 bit var1 -> LSB,     2 */
//#define L_deposit_h(var1)	(Word32)(var1<<16)
//#define L_deposit_l(var1)	(Word32)(var1)

Word32 L_shr_r (Word32 L_var1, Word16 var2); /* Long shift right with
                                         round,  3             */
extern Word32 L_abs (Word32 L_var1);            /* Long abs,              3  */
Word32 L_sat (Word32 L_var1);            /* Long saturation,       4  */
extern norm_s (Word16 var1);             /* Short norm,           15  */
extern Word16 div_s (Word16 var1, Word16 var2); /* Short division,       18  */
extern Word16 norm_l (Word32 L_var1);           /* Long norm,            30  */   

#endif
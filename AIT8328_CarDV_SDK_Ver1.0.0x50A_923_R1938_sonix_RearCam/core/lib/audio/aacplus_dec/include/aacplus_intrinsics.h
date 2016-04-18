/*
   additional intrinsics
 */
#ifndef INTRINSIC_LIB_H
#define INTRINSIC_LIB_H


#include "aacplus_basic_op.h"
#include "aacplus_oper_32b.h"

#define SHORT_BITS 16
#define INT_BITS   32
#define OVL_BITS    8

//#define memop32(x)   { Word32 i; for(i=0;i<((Word32)(x));i++) {  } }
//#define memop16(x)   { Word32 i; for(i=0;i<((Word32)(x));i++) {  } }

Word16 S_max( Word16 a, Word16 b );
Word32 L_max( Word32 a, Word32 b );

Word16 S_min( Word16 a, Word16 b );
Word32 L_min( Word32 a, Word32 b );


/*
  min max macros
*/

#ifndef min
#define min(a,b) ( a < b ? a : b)
#endif

#ifndef max
#define max(a,b) ( a > b ? a : b)
#endif



#endif

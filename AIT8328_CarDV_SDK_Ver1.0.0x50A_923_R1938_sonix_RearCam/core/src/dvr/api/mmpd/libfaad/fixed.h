
#ifndef __FIXED_H__
#define __FIXED_H__

#include "aac_comm.h"

  /* multiply with real shift */
  #define MUL_R(A,B) (real_t)(((int64_t)(A)*(int32_t)(B)) >> REAL_BITS)
  /* multiply with coef shift */
  #define MUL_C(A,B) (real_t)(((int64_t)(A)*(int32_t)(B)) >> COEF_BITS)
  /* multiply with fractional shift */

  #define _MulHigh(A,B) (real_t)(((int64_t)(A)*(int32_t)(B)) >> FRAC_SIZE)

  /* eVC for PocketPC has an intrinsic function that returns only the high 32 bits of a 32x32 bit multiply */
  static INLINE real_t MUL_F(real_t A, real_t B)
  {
      return _MulHigh(A,B) << (32-FRAC_BITS);
  }

/* Complex multiplication */
static INLINE void ComplexMult(real_t *y1, real_t *y2,
    real_t x1, real_t x2, real_t c1, real_t c2)
{
    *y1 = (_MulHigh(x1, c1) + _MulHigh(x2, c2))<<(FRAC_SIZE-FRAC_BITS);
    *y2 = (_MulHigh(x2, c1) - _MulHigh(x1, c2))<<(FRAC_SIZE-FRAC_BITS);
}

#endif

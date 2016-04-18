
#ifndef EQ_COEF_H
#define EQ_COEF_H

#include "equalizer.h"

/* FRAC is the fractional only part of the fixed point number [0.0..1.0) */
#define FRAC_SIZE 32 /* frac is a 32 bit integer */
#define FRAC_BITS 31
#define FRAC_PRECISION ((unsigned int)(1 << FRAC_BITS))
#define FRAC_MAX 0x7FFFFFFF

#define FRAC_CONST(A) (((A) == 1.00) ? ((int)FRAC_MAX) : (((A) >= 0) ? ((int)((A)*(FRAC_PRECISION)+0.5)) : ((int)((A)*(FRAC_PRECISION)-0.5))))

#define FRAC_SIZE_16 16
#define FRAC_BITS_16 15
#define FRAC_PRECISION_16 ((unsigned int)(1 << FRAC_BITS_16))
#define FRAC_MAX_16 0x7FFF

#define FRAC_CONST_16(A) (((A) == 1.00) ? ((int)FRAC_MAX_16) : (((A) >= 0) ? ((int)((A)*(FRAC_PRECISION_16)+0.5)) : ((int)((A)*(FRAC_PRECISION_16)-0.5))))

extern const int Filter_60Hz[16*2];
extern const int Filter_170Hz[16*2];
extern const int Filter_310Hz[16*2];
extern const int Filter_600Hz[16*2];
extern const int Filter_1000Hz[16*2];
extern const int Filter_3000Hz[16*2];
extern const int Filter_6000Hz[16*2];
extern const int Filter_12000Hz[16*2];
extern const int Filter_14000Hz[16*2];
extern const int Filter_16000Hz[16*2];

extern const int *EQFilter[10];
extern const short GainTab[GAIN_STEPS+1];

#endif
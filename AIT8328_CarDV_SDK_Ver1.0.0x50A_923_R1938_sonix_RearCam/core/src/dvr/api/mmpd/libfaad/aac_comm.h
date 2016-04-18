
#ifndef __AAC_COMM_H__
#define __AAC_COMM_H__

#define INLINE __inline
#define ALIGN

typedef unsigned __int64 uint64_t;
typedef unsigned long uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef __int64 int64_t;
typedef long int32_t;
typedef short int16_t;
typedef char  int8_t;
typedef float float32_t;
typedef int32_t real_t;

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define USE_ASM
#define SBR_DEC
//#define PS_DEC
// Define LC_ONLY_DECODER if you want a pure AAC LC decoder (independant of SBR_DEC)
#define LC_ONLY_DECODER

/* FIXED POINT: No MAIN decoding, no SBR decoding */
//#ifndef SBR_LOW_POWER
//#define SBR_LOW_POWER
//#endif

#define SBR_DIV(A, B) (((int64_t)A << REAL_BITS)/B)

#ifndef SBR_LOW_POWER
#define qmf_t complex_t
#define QMF_RE(A) RE(A)
#define QMF_IM(A) IM(A)
#else
#define qmf_t real_t
#define QMF_RE(A) (A)
#define QMF_IM(A)
#endif

#undef ARCH_IS_BIG_ENDIAN

#define COEF_BITS 28
#define COEF_PRECISION (1 << COEF_BITS)
#define COEF_BITS16 12
#define COEF_PRECISION16 (uint32_t)((1 << COEF_BITS16)-1)
#define REAL_BITS 14 // MAXIMUM OF 14 FOR FIXED POINT SBR
#define REAL_PRECISION (1 << REAL_BITS)

/* FRAC is the fractional only part of the fixed point number [0.0..1.0) */
#define FRAC_SIZE 32 /* frac is a 32 bit integer */
#define FRAC_BITS 31
#define FRAC_PRECISION ((uint32_t)(1 << FRAC_BITS))
#define FRAC_MAX 0x7FFFFFFF

#define FRAC_SIZE16 16 /* frac is a 32 bit integer */
#define FRAC_BITS16 15
#define FRAC_PRECISION16 ((uint32_t)((1 << FRAC_BITS16)-1))
#define FRAC_MAX16 0x7FFF


#define REAL_CONST(A) (((A) >= 0) ? ((real_t)((A)*(REAL_PRECISION)+0.5)) : ((real_t)((A)*(REAL_PRECISION)-0.5)))
#define COEF_CONST(A) (((A) >= 0) ? ((real_t)((A)*(COEF_PRECISION)+0.5)) : ((real_t)((A)*(COEF_PRECISION)-0.5)))
#define FRAC_CONST(A) (((A) == 1.00) ? ((real_t)FRAC_MAX) : (((A) >= 0) ? ((real_t)((A)*(FRAC_PRECISION)+0.5)) : ((real_t)((A)*(FRAC_PRECISION)-0.5))))

#define COEF_CONST16(A) (((A) >= 0) ? ((int16_t)((A)*(COEF_PRECISION16))) : ((int16_t)((A)*(COEF_PRECISION16))))
#define FRAC_CONST16(A) (((A) == 1.00) ? ((int16_t)FRAC_MAX16) : (((A) >= 0) ? ((int16_t)((A)*(FRAC_PRECISION16)+0)) : ((int16_t)((A)*(FRAC_PRECISION16)-0))))

typedef real_t complex_t[2];
typedef int16_t complex_t16[2];
#define RE(A) A[0]
#define IM(A) A[1]


/* common functions */
uint32_t random_int(void);
uint8_t get_sr_index(const uint32_t samplerate);
uint8_t max_pred_sfb(const uint8_t sr_index);
uint8_t max_tns_sfb(const uint8_t sr_index, const uint8_t object_type,
                    const uint8_t is_short);
uint32_t get_sample_rate(const uint8_t sr_index);
int16_t can_decode_ot(const uint8_t object_type);




#endif

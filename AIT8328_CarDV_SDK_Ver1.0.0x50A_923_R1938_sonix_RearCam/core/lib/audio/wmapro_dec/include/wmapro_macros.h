//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    Macros.h

Abstract:

    Processor-specific macros, defines, and data types.

Author:

    Raymond Cheng (raych)       Jul 29, 1999

Revision History:


*************************************************************************/

#ifndef __COMMON_MACROS_H
#define __COMMON_MACROS_H
#include "wmapro_wma_def.h"
// ***********************************************************************
// Arithmetic coding, and testing
// **********************************************************************

//#define DEFINE_ARITH_CODER

// ***********************************************************************

// If BUILD_INTEGER or BUILD_INT_FLOAT is not defined by project set default state

#include "wmapro_constants.h"

// The following are ANSI C includes, and so should be safe for all platforms (end)
// This is a walk around to turn on WMAProDec_ASSERT in build environment. Since we are
// not allowed to have debug bits CRT, we don't have DEBUG_CRT defined and NDEBUG is 
// defined in makefile.def. Later in WMAProDec_ASSERT.h WMAProDec_ASSERT is defined as void. But since 
// v9 does not go into windows we can walk around the problem by turning off NDEBUG 
// temporarily. VC project does not have this problem (end)
//#if defined (NDEBUG) && ( defined (DBG) || defined (_DEBUG) || defined (DEBUG) )
//#undef NDEBUG
//#define DISABLE_NDEBUG_TEMP
//#endif 
//#include <WMAProDec_ASSERT.h>     // If this doesn't exist, you must write an "WMAProDec_ASSERT" macro
//#if !defined (NDEBUG) && ( defined (DBG) || defined (_DEBUG) || defined (DEBUG) ) && defined (DISABLE_NDEBUG_TEMP)
//#define NDEBUG
//#endif 

//#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
//#include <stdarg.h>

//#if !defined(BUILD_WMASTD) && !defined(BUILD_WMAPRO) && !defined(BUILD_WMALSL)
//// We must build for everything :)
//#define BUILD_WMASTD
//#define BUILD_WMAPRO
//    #define BUILD_WMAPRO_PLLM
//#define BUILD_WMALSL
//#endif // !BUILD_WMASTD && !BUILD_WMAPRO && !BUILD_WMALSL

//#if defined (BUILD_WMAPRO) && !defined (BUILD_WMALSL)
//// Disable now.
////    #define SAVE_PCMBUFFER_IN_PLLM
//#endif

// Must define one of following two modes. TRANSFORMED based is 
// more accurate but requires full size DCT in half transform.
// FILTERBASED does not need full size DCT therefore save mem.
// #define MLLM_HALFDCT_TRANSFORMBASED
#define MLLM_HALFDCT_FILTERBASED

// **************************************************************************
// Platform-Specific #ifdefs
// **************************************************************************


//#ifndef DISABLE_OPT // If this is defined, use the ANSI build
//
//#ifdef __QNX__
//#undef _M_IX86
//#endif
//
//#if defined(_M_IX86) && !defined(INTEGER_ENCODER)
////#ifdef __QNX__
////#define WMA_TARGET_QNX_X86
////#else
//#define WMA_TARGET_X86
////#endif
//#endif
//
//#ifdef _MIPS_
//#define WMA_TARGET_MIPS
//#endif
//
//#if defined(_SH3_) && !defined(_SH4_)
//#define WMA_TARGET_SH3
//#endif
//
//#ifdef _SH4_
//#define WMA_TARGET_SH4
//#endif
//
//#ifdef _Embedded_x86
//#define WMA_TARGET_Embedded_x86
//#endif
//
//
//#ifdef S_SUNOS5
//#define WMA_TARGET_S_SUNOS5
//#endif
//
//#ifdef _ARM_
//#define WMA_TARGET_ARM
//#endif
//
//// It's often nice to be able to compare the output between optimized versions
//// and the ANSI version, to verify that the optimizations are in sync. Alas,
//// some optimizations sacrifice reproducibility for speed. The following #define allows
//// us to disable those optimizations to verify the main algorithms.
////#define WMA_ANSI_COMPATIBILITY_MODE
//
//#endif  // !DISABLE_OPT

//// ======================================================
//// ANSI-C version (no platform-specific details
//// ======================================================
//#else

#define WMA_TARGET_ANSI

// Default endian
#if !defined(LITTLE_ENDIAN) && !defined(BIG_ENDIAN)
#define LITTLE_ENDIAN
#endif

// Do the fundamental typedefs: {U|I}{8|16|32|64}
//#ifdef NOINT64 
//#include "int64.h"
//typedef UINT64 U64;
//#else
//#ifdef __QNX__
//typedef unsigned long long int U64;
//typedef long long int I64;
//#else
//#ifdef macintosh
//typedef unsigned long long U64;
//typedef long long I64;
//#else
typedef unsigned __int64 U64;
typedef __int64 I64;
//#endif
//#endif
//#endif //NOINT64
// Default all other typedefs to standard definitions (see below)

//#endif // Platform-specific #ifdefs



// **************************************************************************
// Default Fundamental Typedefs
// **************************************************************************
#ifndef PLATFORM_SPECIFIC_I32
#ifdef HITACHI
typedef unsigned long U32;
typedef long I32;
#else
typedef unsigned long int U32;
typedef long int I32;
#endif
#endif

#ifndef PLATFORM_SPECIFIC_I16
typedef unsigned short U16;
typedef short I16;
#endif

#ifndef PLATFORM_SPECIFIC_I8
typedef unsigned char U8;
typedef signed char I8;
#endif

#include "wmatyps.h"
#include "wmaerror.h"

// internal SUCCESS codes
static const WMARESULT WMA_S_SWITCHCHMODE   = 0x00010001; // Internal error, corrected internally
// the matrix input to matrixinvert is or close to singular. can not invert or the inversion is not precise.
static const WMARESULT WMA_S_SINGULARMATRIX = 0x00010002;
static const WMARESULT WMA_S_MODELLING      = 0x00010003; // Modelling, skip full encode steps
static const WMARESULT WMA_S_CANTMAKEBITS   = 0x00010004; // Can't make the cMin/cMax bit constraint
static const WMARESULT WMA_S_DEFAULT        = 0x00010005;
static const WMARESULT WMA_S_NOMOREOUTPUT   = 0x00010006;
static const WMARESULT WMA_S_PLLM_NOLPC     = 0x00010007; // Did not get reliable LPC filter.
static const WMARESULT WMA_S_PLLM_UNSTABLELPC= 0x00010008; // unstable LPC can not be converted to LSP domain.

//ON_HOLD: the decoder can't continue operating because of sth, most
//       likely no more data for flushing, getbits or peekbits. all functions
//       stop *immediately*.

// internal error codes
static const WMARESULT WMA_E_BUFFERUNDERFLOW =  0x80010001;
static const WMARESULT WMA_E_INVALIDRECHDR  =   0x80010002;
static const WMARESULT WMA_E_SYNCERROR      =   0x80010003;
static const WMARESULT WMA_E_NOTIMPL        =   0x80010004;
static const WMARESULT WMA_E_FLUSHOVERFLOW  =   0x80010005; // Too much input data provided to flush
static const WMARESULT WMA_E_CANNOTPREDICT  =   0x80010007; // Cannot make bit-constrained VBR prediction due to insufficient data
static const WMARESULT WMA_E_COEFSTRMFILE   =   0x80010008; // Failure reading or writing from coefficient stream file.
static const WMARESULT WMA_E_FILESTRMFILEIO =   0x80010009; // Error in filestrm.c with File I/O (opening, reading or writing).
static const WMARESULT WMA_E_MODELINVALID   =   0x8001000A; // Two-pass VBR model cannot predict using given dependent var
static const WMARESULT WMA_E_INPUTBUFUNDERFLOW= 0x8001000B; // Two-pass VBR, 2nd pass, not enough input PCM supplied to correlate to output
static const WMARESULT WMA_E_FRAMECAPOVERFLOW = 0x8001000C; // v2 only
static const WMARESULT WMA_E_NMRPREDICTFAILURE= 0x8001000D; // Two-pass VBR: predictor is unable to generate a meaningful estimate for global NMR
static const WMARESULT WMA_E_MODELDEVIATION =   0x8001000E; // Two-pass VBR: current results directly conflict with previous model datapoints (eg, same QStep, diff NMR).
static const WMARESULT WMA_E_MODELNOROOM =      0x8001000F; // Two-pass VBR: no more room for additional datapoints!

#define WMAB_TRUE 1
#define WMAB_FALSE 0

#define own // used as "reserved word" to indicate ownership or transfer to distinguish from const

typedef double Double;
typedef float Float;
typedef void Void;
typedef U32 UInt;
typedef I32 Int;
typedef I32 Bool; // In Win32, BOOL is an "int" == 4 bytes. Keep it this way to reduce problems.



//***************************************************************************
// 24-bit Encoding
//***************************************************************************

#define PCMSAMPLE_MAX(iValidBits)   ((1 << ((iValidBits) - 1)) - 1)
#define PCMSAMPLE_MIN(iValidBits)   (~((1 << ((iValidBits) - 1)) - 1))

typedef I32 PCMSAMPLE;
#define PCMSAMPLE_BITS  32

#define I24_MIN     ((I32)0xFF800000)
#define I24_MAX     ((I32)0x007FFFFF)
#define I2024_MIN   ((I32)0xFFF80000)
#define I2024_MAX   ((I32)0x0007FFFF)
#define I25_MIN     ((I32)0xFF000000)
#define I25_MAX     ((I32)0x00FFFFFF)
#define I2025_MIN   ((I32)0xFFF00000)
#define I2025_MAX   ((I32)0x000FFFFF)

#define I32_MIN ((I32)0x80000000)

// **************************************************************************
// Overridable Compiler Directives
// **************************************************************************
#ifndef PLATFORM_SPECIFIC_INLINE
// This should hopefully work for most compilers. It works for VC
#define INLINE __inline
#endif  // PLATFORM_SPECIFIC_INLINE

#ifndef PLATFORM_SPECIFIC_COMPILER_MESSAGE
#define COMPILER_MESSAGE(x)         message(x)
#endif

#ifndef PLATFORM_SPECIFIC_COMPILER_PACKALIGN
#define COMPILER_PACKALIGN(x)       pack(x)
#endif

#ifndef PLATFORM_SPECIFIC_COMPILER_PACKALIGN_DEFAULT
#define COMPILER_PACKALIGN_DEFAULT  pack()
#endif

#ifdef _MSC_VER
#define MSVC_DISABLE_WARNING(x)     warning(disable:x)
#define MSVC_RESTORE_WARNING(x)     warning(default:x)
#define MSVC_CDECL                  __cdecl
#else   // _MSC_VER
// I'm hoping that compilers can take an empty #pragma (VC can)
#define MSVC_DISABLE_WARNING(x)
#define MSVC_RESTORE_WARNING(x)
#define MSVC_CDECL
#endif  // _MSC_VER


// **************************************************************************
// Macros Common to All Platforms
// **************************************************************************
// If you need to override the following for just one platform, #define a
// switch to override as in the case for ROUNDF/ROUNDD above in the
// platform-specific #ifdefs so that it is plain to see.

#define MULT_HI(a,b) (MULT_HI_DWORD(a,b)<<1)
#ifndef BUILD_INTEGER
    // these macros are not used by integer decoder. 
    // Where/when needed, provide cpu depended optimized versions as well as these generic ones.
#   define UMASR(a,b,c) (U32)((((U64)a)*((U64)b))>>c)
#   define MASR(a,b,c) (I32)((((I64)a)*((I64)b))>>(c))
#   define DASR(a,b,c) (I32)((((I64)a)*((I64)b))>>(c))
#   define DASL(a,b,c) (I32)((((I64)a)*((I64)b))<<(c))

    // The following macro has no perf-enhanced equivalents, although one can
    // easily be written a la x86 MULT_HI_DWORD:
    // I32HI,LOW = a*b; if (I32LOW & 0x80000000) I32HI += 1; return I32HI;
    // NOTE that it only rounds UP, and does not consider negative numbers.
#   define MULT_HI_DWORD_ROUND(a,b)    (I32)((((I64)(a))*((I64)(b)) + ((I64)1 << 31))>>32)
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b)  (((a) < (b)) ? (a) : (b))
#endif
#ifndef checkRange
#define checkRange(x, a, b) (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))
#endif
#define DELETE_ARRAY(ptr) if (ptr!=NULL) { auFree (ptr); ptr = NULL;}
#define DELETE_PTR(ptr) if (ptr!=NULL) {auFree (ptr); ptr = NULL;}
#define ARRAY_SIZE(ar) (sizeof(ar)/sizeof((ar)[0]))

#  ifdef DEBUG_ONLY
// WCE already has this macro defined in the same way, avoid warning messages
#  undef DEBUG_ONLY
#  endif
#ifdef _DEBUG
#define DEBUG_ONLY(x)   x
#else   // _DEBUG
#define DEBUG_ONLY(x)
#endif  // _DEBUG

#if defined(LITTLE_ENDIAN)
#define DEFAULTENDIAN_TO_BIGENDIAN(i)   ByteSwap(i)
#define BIGENDIAN_TO_DEFAULTENDIAN(i)   ByteSwap(i)
#elif defined(BIG_ENDIAN)
#define DEFAULTENDIAN_TO_BIGENDIAN(i)   (i)
#define BIGENDIAN_TO_DEFAULTENDIAN(i)   (i)
#else
#error Must define the endian of the target platform
#endif // ENDIAN

#define SWAP_WORD( w )      (w) = ((w) << 8) | ((w) >> 8)

#define SWAP_DWORD( dw )    (dw) = ((dw) << 24) | ( ((dw) & 0xFF00) << 8 ) | ( ((dw) & 0xFF0000) >> 8 ) | ( ((dw) & 0xFF000000) >> 24);

// WMARESULT macros (stolen from winerror.h)
#define WMA_SUCCEEDED(Status) ((WMARESULT)(Status) >= 0)
#define WMA_FAILED(Status) ((WMARESULT)(Status)<0)

// Use 64-bit buffers in integer build to get better match in int vs. intfloat
#if defined(BUILD_INTEGER) && defined(BUILD_WMAPRO)
#define COEF64BIT
#define RECALC_SINCOS
#else
#undef COEF64BIT
#endif

// Use integer calculations when calculating inverse channel transform matrix
#define FLOAT_INT_CHXFORM

#ifdef COEF64BIT
//#define HIGHBIT_CH
//#define HIGHBIT_IQ
#define HIGHBIT_FFT
#else
#undef HIGHBIT_CH
#undef HIGHBIT_IQ
#undef HIGHBIT_FFT
#endif

#if defined(BUILD_INTEGER) // already integer calcs
#undef FLOAT_INT_CHXFORM
#elif defined(BUILD_INT_FLOAT) // float cannont support 64 bit buffers
#undef COEF64BIT
#else // assume it is floating build, unless otherwise specified.
#undef COEF64BIT
//#error "At least one of BUILD_INTEGER or BUILD_INT_FLOAT must be defined."
#endif

#ifdef COEF64BIT
typedef I64 CBT;
#else
typedef Int CBT;
#endif

#ifdef COEF64BIT
// a is guaranteed to be less than 40 bits.
INLINE I64 multHi64(I64 a, I32 b, Int bits)
{
  if (bits <= 32) {
    if (a > 0)
      return ((((a & 0xffffffff)*b)>>bits) + (((a>>32)*b)<<(32-bits)));
    else {
      a = -a;
      return -((((a & 0xffffffff)*b)>>bits) + (((a>>32)*b)<<(32-bits)));
    }
  } else {
    if (a > 0)
      return ((((a & 0xffffffff)*b)>>bits) + (((a>>32)*b)>>(bits-32)));
    else {
      a = -a;
      return -((((a & 0xffffffff)*b)>>bits) + (((a>>32)*b)>>(bits-32)));
    }
  }
}
INLINE I64 multHi64B31(I64 a, I32 b)
{
#if 0 // non-branching versions of the above for 30, 31 bits
  U64 r;
  U64 s = a >> 63;
  a = (a^s)-s;
  r = ((((a & 0xffffffff)*b)>>31) + (((a>>32)*b)<<1));
  return (r^s)-s;
#else
  if (a > 0)
    return ((((a & 0xffffffff)*b)>>31) + (((a>>32)*b)<<1));
  else {
    a = -a;
    return -((((a & 0xffffffff)*b)>>31) + (((a>>32)*b)<<1));
  }
#endif
}
INLINE I64 multHi64B30(I64 a, I32 b)
{
#if 0
  U64 r;
  U64 s = a >> 63;
  a = (a^s)-s;
  r = ((((a & 0xffffffff)*b)>>30) + (((a>>32)*b)<<2));
  return (r^s)-s;
#else
  if (a > 0)
    return ((((a & 0xffffffff)*b)>>30) + (((a>>32)*b)<<2));
  else {
    a = -a;
    return -((((a & 0xffffffff)*b)>>30) + (((a>>32)*b)<<2));
  }
#endif
}
#endif

INLINE I32 wmaiiround2toN(const I32 x, const I32 N) {
  return x >= 0 ? (x+(1<<N>>1)) & ~((1<<N)-1) : (x-1+(1<<N>>1)) & ~((1<<N)-1);
}

// Create types which are change implementation between BUILD_INTEGER and BUILD_INT_FLOAT
// Some platforms, like the X86 and the SH4, can implement either build efficently.
// Other platforms, with only emulated floating point support, are typically only build with BUILD_INTEGER
//  
// Each type defined below is either a float (BUILD_INT_FLOAT) and an I32 with some number of fractional bits (BUILD_INTEGER)
// Use float or and I32 with five fractional bits
// And create a type for trig functions (Float or I32 with 30 fractional bits)

#define FLOAT_FROM_FRAC(a, bits) ((a)/((Float)(((I64)1<<(bits)))))
#define FRAC_FROM_FLOAT(a, bits) ((I32)((a)*(Float)(((I64)1<<(bits)))))

#if 0 // the following loses one bit of precision.
#define MULT_HI32_SHIFT(a, scale, bits) \
  (MULT_HI_DWORD(a, (scale)<<(31-(bits))) << 1)
#else // this maintains full precision
#define MULT_HI32_SHIFT(a, b, bits) \
  ((I32)(((I64)(a)*(I64)(b))>>bits))
#endif

#if defined(BUILD_INTEGER)

// Coefficents type (output of inverse quantization, transformed by DCT and overlapped/added)
// Range is +/- 2^26 with five fractional bits
#ifdef COEF64BIT
    typedef I64 CoefType;
#else
    typedef I32 CoefType;
#endif
#   define COEF_FRAC_BITS TRANSFORM_FRACT_BITS // == 5
#   define COEF_FRAC_SCALE (1<<COEF_FRAC_BITS)
#   define COEF_FROM_FLOAT(flt) ((CoefType)(flt*COEF_FRAC_SCALE))
#   define FLOAT_FROM_COEF(coef) (coef/((Float)COEF_FRAC_SCALE))
//#   define COEF_FROM_INT(i) ((i)<<COEF_FRAC_BITS)
//#   define INT_FROM_COEF(cf) ((cf)>>COEF_FRAC_BITS)
#   define COEF_FROM_INT(i) ((i)<<pau->m_cLeftShiftBitsFixedPre)
#   define INT_FROM_COEF(cf) ((cf)>>pau->m_cLeftShiftBitsFixedPost)

#   define FRAC_FROM_RATIO(a, b, bits) (DIVI64BYU32((I64)(a)<<(bits), (b)))

// Fractional Type with range -2.0 <= x < 2.0 used by FFT Trig Recurrsion 
    typedef I32 BP2Type;
#   define BP2_FRAC_BITS 30
#   define BP2_FRAC_SCALE NF2BP2
#   define BP2_FROM_FLOAT(flt) ((BP2Type)(flt*BP2_FRAC_SCALE))
#   define FLOAT_FROM_BP2(bp2) (bp2/((Float)BP2_FRAC_SCALE))
#   define BP2_FROM_BP1(bp1) ((bp1)>>1)
#   define MULT_BP2(a,b) MULT_HI_DWORD_DOWN((a),(b))

// Fractional type with range -1.0 <= x < 1.0 used by DCT Trig Recurrsion
    typedef I32 BP1Type;
#   define BP1_FRAC_BITS 31
#   define BP1_FRAC_SCALE NF2BP1
#   define BP1_FROM_FLOAT(flt) ((BP2Type)(flt*BP1_FRAC_SCALE))
#   define FLOAT_FROM_BP1(bp1) (bp1/((Float)BP1_FRAC_SCALE))
#   define MULT_BP1(a,b) MULT_HI((a),(b))

#   define MAX40BITVAL ((I64)0xfffffffff0)
#   define LOG2MAX40BITVAL (39)
#   define MAX32BITVAL ((I32)0x7ffffff0)
#   define LOG2MAX32BITVAL (30)

#ifdef HIGHBIT_FFT
#   define MAXINTVAL_FFT MAX40BITVAL
#   define MAXINTVAL_FFT_LOG2 (LOG2MAX40BITVAL)  //LOG2_64(MAXINTVAL_FFT)
#   define MULT_CBP2(a,b) (multHi64B30((b), (a)))
#   define MULT_CBP1(a,b) (multHi64B31((b), (a)))
#else
#   define MAXINTVAL_FFT MAX32BITVAL
#   define MAXINTVAL_FFT_LOG2 (LOG2MAX32BITVAL)  //LOG2(MAXINTVAL_FFT)
#   define MULT_CBP2(a,b) MULT_BP2(a,b)
#   define MULT_CBP1(a,b) MULT_BP1(a,b)
#endif

// Channel transform
    typedef I32 ChXFormType;
#   define CH_FRAC_BITS (30)
#ifdef HIGHBIT_CH
#   define MAXINTVAL_CH MAX40BITVAL
#   define MAXINTVAL_CH_LOG2 (LOG2MAX40BITVAL)   //LOG2_64(MAXINTVAL_CH)
#   define MULT_CH(a,b) (multHi64(b, a, CH_FRAC_BITS))
#   define MULT_CH_SQRT2(a) (multHi64((a), INT_SQRT2, INT_SQRT2_BITS))
#else
#   define MAXINTVAL_CH MAX32BITVAL
#   define MAXINTVAL_CH_LOG2 (LOG2MAX32BITVAL)   //LOG2(MAXINTVAL_CH)
#   define MULT_CH(a,b) MULT_HI32_SHIFT(b, a, CH_FRAC_BITS)
#   define MULT_CH_SQRT2(a) MULT_HI32_SHIFT(a, INT_SQRT2, INT_SQRT2_BITS)
#endif
#   define MULT_CH_MATRIX(a,b) MULT_HI32_SHIFT(a, b, CH_FRAC_BITS)
#   define CH_FROM_FLOAT(flt) FRAC_FROM_FLOAT(flt, CH_FRAC_BITS)
#   define FLOAT_FROM_CH(ch) FLOAT_FROM_FRAC(ch, CH_FRAC_BITS)
#   define ROUND_CH_TO_2TON(x, N) (((N) >= CH_FRAC_BITS) ? (x) : wmaiiround2toN(x, CH_FRAC_BITS-N))

    typedef ChXFormType ChXFormMType;
#   define CHM_FROM_FLOAT(flt) CH_FROM_FLOAT(flt)
#   define FLOAT_FORM_CHM(ch) FLOAT_FROM_CH(ch)

    typedef I32 ChPPXFormType;
#   define CHPP_FRAC_BITS (30)
#   define MULT_CHPP(a,b) MULT_HI32_SHIFT(a, b, CHPP_FRAC_BITS)
#   define CHPP_FROM_FLOAT(flt) FRAC_FROM_FLOAT(flt, CHPP_FRAC_BITS)

// Downmix transform
    typedef I32 ChDnMixType;
#   define CHDN_FRAC_BITS (23)
#   define MULT_CHDN(a,b) MULT_HI32_SHIFT(a, b, CHDN_FRAC_BITS)
#   define CHDN_FROM_FLOAT(flt) FRAC_FROM_FLOAT(flt, CHDN_FRAC_BITS)

// Half transform for mllm
    typedef I32 HTFilterType;
#   define HTFT_FRAC_BITS (28)
#   define MULT_HTFT(a,b) MULT_HI32_SHIFT(a, b, HTFT_FRAC_BITS)
#   define HTFT_FROM_FLOAT(flt) FRAC_FROM_FLOAT(flt, HTFT_FRAC_BITS)


// fix-point binary points conversion factors
// convert float or double to BP1 integer ( -1.0 <= x < +1.0 )
#define NF2BP1 0x7FFFFFFF
// convert float or double to BP2 integer ( -2.0 <= x < +2.0 )
#define NF2BP2 0x3FFFFFFF

// Fraction is stored at 2^32 (BP0)
#define UBP0_FROM_FLOAT(x) ((U32)(x*4294967296.0F))

    typedef I32 FracBitsType;
    typedef struct {
    //I8 exponent;  Old exponent was shift from binary point at 24th position
        FracBitsType iFracBits;
        I32 iFraction;
    } FastFloat;

// Type for holding quantization magnitide returned by prvWeightedQuantization
    typedef FastFloat QuantFloat;

    // Some utility functions with obvious meaning
#   define DIV2(a) ((a)>>1)
#   define MUL2(a) ((a)<<1)
#   define DIV4(a) ((a)>>2)
#   define MUL4(a) ((a)<<2)
#   define MUL8(a) ((a)<<3)

#   define INTEGER_ONLY(a) a
#   define INTEGER_OR_INT_FLOAT(a,b) a

#define MULT_SQRT2(a) (I32)(((I64)(a) * INT_SQRT2) >> INT_SQRT2_BITS)

#else // ************ so must be BUILD_INT_FLOAT *****************

// Coefficents type (output of inverse quantization, transformed by DCT and overlapped/added)
    typedef Float CoefType;
#   define COEF_FRAC_BITS 0
#   define COEF_FRAC_SCALE 1
#   define COEF_FROM_FLOAT(flt) ((CoefType)(flt))
#   define FLOAT_FROM_COEF(coef) ((Float)(coef))
#   define COEF_FROM_INT(i) (i)
#   define INT_FROM_COEF(cf) (cf)

#   define FRAC_FROM_RATIO(a, b, bits) ((a)/(b))

// Fractional Type used by FFT Trig Recurrsion 
    typedef Float BP2Type;
#   define BP2_FRAC_BITS 0
#   define BP2_FRAC_SCALE 1
#   define BP2_FROM_FLOAT(flt) ((BP2Type)(flt))
#   define FLOAT_FROM_BP2(bp2) ((Float)(bp2))
#   define BP2_FROM_BP1(bp1) (bp1)
#   define MULT_BP2(a,b) ((a)*(b))

// Fractional type used by DCT Trig Recurrsion
    typedef Float BP1Type;
#   define BP1_FRAC_BITS 0
#   define BP1_FRAC_SCALE 1
#   define BP1_FROM_FLOAT(flt) ((BP1Type)(flt))
#   define FLOAT_FROM_BP1(bp1) ((Float)(bp1))
#   define MULT_BP1(a,b) ((a)*(b))

// Calculation of output of channel transform
#define CH_FRAC_BITS (30)
    typedef Float ChXFormType;
#   define MULT_CH(a,b) ((a)*(b))
#   define MULT_CH_SQRT2(a) ((a) * (FLT_SQRT2))
#   define CH_FROM_FLOAT(flt) (flt)
#   define FLOAT_FROM_CH(ch) (ch)

// Channel transform
#ifdef FLOAT_INT_CHXFORM
    typedef I32 ChXFormMType;
#   define MULT_CH_MATRIX(a,b) MULT_HI32_SHIFT(a, b, CH_FRAC_BITS)
#   define ROUND_CH_TO_2TON(x, N) (((N) >= CH_FRAC_BITS) ? (x) : wmaiiround2toN(x, CH_FRAC_BITS-N))
#   define CHM_FROM_FLOAT(flt) FRAC_FROM_FLOAT(flt, CH_FRAC_BITS)
#   define FLOAT_FROM_CHM(ch) FLOAT_FROM_FRAC(ch, CH_FRAC_BITS)
#else
    typedef ChXFormType ChXFormMType;
#   define MULT_CH_MATRIX(a,b) ((a)*(b))
#   define ROUND_CH_TO_2TON(x, N) (ROUNDF((x)*(Float)(1<<(N))) / (Float)(1<<(N)))
#   define CHM_FROM_FLOAT(flt) (flt)
#   define FLOAT_FROM_CHM(ch) (ch)
#endif

    typedef Float ChPPXFormType;
#   define MULT_CHPP(a,b) ((a)*(b))
#   define CHPP_FROM_FLOAT(flt) (flt)

// Downmix transform
    typedef Float ChDnMixType;
#   define MULT_CHDN(a,b) ((a)*(b))
#   define CHDN_FROM_FLOAT(flt) (flt)

// Half transform for mllm
    typedef Float HTFilterType;
#   define MULT_HTFT(a,b) ((a)*(b))
#   define HTFT_FROM_FLOAT(flt) (flt)

#   define MULT_CBP1(a,b) MULT_BP1(a,b)
#   define MULT_CBP2(a,b) MULT_BP2(a,b)

// fix-point binary points conversion factors
// convert float or double to BP1 integer ( -1.0 <= x < +1.0 )
#define NF2BP1 2
// convert float or double to BP2 integer ( -2.0 <= x < +2.0 )
#define NF2BP2 1

// Another form of floating point 
    typedef Float FastFloat;

// Type for holding quantization magnitide returned by prvWeightedQuantization
    typedef Float QuantFloat;

// Some utility functions with obvious meaning
#   define DIV2(a) ((a)*0.5f)
#   define MUL2(a) ((a)*2.0f)
#   define DIV4(a) ((a)*0.25f)
#   define MUL4(a) ((a)*4.0f)
#   define MUL8(a) ((a)*8.0f)

#   define INTEGER_ONLY(a)
#   define INTEGER_OR_INT_FLOAT(a,b) b

#define MULT_SQRT2(a) ((a) * (FLT_SQRT2))

#endif  // either BUILD_INTEGER or BUILD_INT_FLOAT

#ifdef BUILD_INTEGER
#define BUILD_INTEGER_LPC
#endif
// always build integer lpc as long as not using PLATFORM_LPC_DIRECT
#ifndef PLATFORM_LPC_DIRECT
#define BUILD_INTEGER_LPC
#endif // PLATFORM_LPC_DIRECT

#ifdef BUILD_INTEGER_LPC

//****************************************************************************
//         Types, constants related to LPC.
//         Lot of these could be moved to constants.h or lpcConst.h for cleanliness
//****************************************************************************

//  LSP (aka LSF), used for inverse quantized LSF.
    typedef Int LspType;
#   define FRACT_BITS_LSP 30
#   define LSP_SCALE (1<<FRACT_BITS_LSP)
#   define LSP_FROM_FLOAT(a) ((LpType)((a)*LSP_SCALE))
#   define MULT_LSP(a,b) (I32)((((I64)(a))*((I64)(b)))>>FRACT_BITS_LSP)
// MULT_LSP may be redefined by processor specific code in macros_cpu.h

//  LP, used for P, Q, and LPC values
    typedef Int LpType;
#   ifndef MATCH_ARM_WAY
#       define FRACT_BITS_LP 27
#   else
#       define FRACT_BITS_LP 26
#   endif
#   define LP_SCALE (1<<FRACT_BITS_LP)
#   define FLOAT_FROM_LP(a) ((Float)((a)*(1.0f/LP_SCALE)))
#   define LP_FROM_FLOAT(a) ((LpType)((a)*LP_SCALE))

//  LP Spectrum: used during FFT of LPC values
//  LP Spectrum power: sum-squares of FFT output
    typedef Int LpSpecType;
//                Uses 64-bit intermediates
#   ifndef MATCH_ARM_WAY
#       define FRACT_BITS_LP_SPEC 25
#   else
        //  ARM's use of 26 instead of 25 produces BIG relative errors compared to LPC_COMPARE
        //  at least when using the old version of the Lpc code.
//#       define FRACT_BITS_LP_SPEC 26
#       define FRACT_BITS_LP_SPEC 25
#   endif
    typedef U64 LpSpecPowerType;
#   define BITS_LP_SPEC_POWER 64
#   define FRACT_BITS_LP_SPEC_POWER 30 
#   define QR_FRACTION_FRAC_BITS 24
#   define QR_EXPONENT_FRAC_BITS 29

#   define LP_SPEC_SCALE (1<<FRACT_BITS_LP_SPEC)
#   define LP_SPEC_POWER_SCALE ((float)(1L<<FRACT_BITS_LP_SPEC_POWER))
#   define LP_SPEC_FROM_FLOAT(a) ((LpSpecType)((a)*LP_SPEC_SCALE))
#   define LP_SPEC_POWER_FROM_FLOAT(a) ((LpSpecType)((a)*LP_SPEC_POWER_SCALE))
#   define FLOAT_FROM_LP_SPEC(a) ((Float)((a)*(1.0f/LP_SPEC_SCALE)))
#   define LP_SPEC_FROM_LP(a) ((LpSpecType)((a)>>(FRACT_BITS_LP-FRACT_BITS_LP_SPEC)))

//  Weights, computed from LPC (and Bark?).
    typedef UInt WeightType;
#ifdef BUILD_INTEGER
    typedef UInt FinalWeightType;
#   define FINALWEIGHT_FROM_FLOAT(flt) WEIGHT_FROM_FLOAT(flt)
#   define FINALWEIGHT_FROM_WEIGHT(wt) (wt)
#else
    typedef Float FinalWeightType;
#   define FINALWEIGHT_FROM_FLOAT(flt) (flt)
#   define FINALWEIGHT_FROM_WEIGHT(wt) FLOAT_FROM_WEIGHT(wt)
#endif
//               We see weight factor values upto 64.53440857: 7 bits enough to cover integer part, 
//               leaving upto 25 bits for fraction. However, only 21 bits of fraction are generated 
//               upstream, making other 4 bits zero, even if used.
#   ifndef MATCH_ARM_WAY
#       define WEIGHTFACTOR_FRACT_BITS     21
#   else
#       define WEIGHTFACTOR_FRACT_BITS     25
#   endif
#   define WEIGHT_FROM_FLOAT(a) ((WeightType)((a)*(1<<WEIGHTFACTOR_FRACT_BITS)))
#   define FLOAT_FROM_WEIGHT(a) ((Float)((a)*(1.0f/(1<<WEIGHTFACTOR_FRACT_BITS))))

// convert float or double to BP2 integer ( -2.0 <= x < +2.0 )
#define NF2BP2LPC 0x3FFFFFFF

// Fractional Type with range -2.0 <= x < 2.0 used by FFT Trig Recurrsion 
    typedef I32 BP2LPCType;
#   define BP2LPC_FRAC_BITS 30
#   define BP2LPC_FRAC_SCALE NF2BP2LPC
#   define BP2LPC_FROM_FLOAT(flt) ((BP2LPCType)((flt)*BP2LPC_FRAC_SCALE))
#   define FLOAT_FROM_BP2LPC(bp2) ((bp2)/((Float)BP2LPC_FRAC_SCALE))
#if defined(BUILD_INTEGER)
#   define BP2LPC_FROM_BP1(bp1) ((bp1)>>1)
#   define BP2LPC_FROM_BP2(bp2) (bp2)
#else
#   define BP2LPC_FROM_BP1(bp1) BP2LPC_FROM_FLOAT(bp1)
#   define BP2LPC_FROM_BP2(bp2) BP2LPC_FROM_FLOAT(bp2)
#endif
#   define MULT_BP2LPC(a,b) MULT_HI_DWORD_DOWN((a),(b))
#   define MULT_BP2LPC_I32(a,b) ((I32) (((a) * (I64) (b)) >> BP2LPC_FRAC_BITS))

// Fraction is stored at 2^32 (BP0)
#define UBP0_FROM_FLOAT_LPC(x) ((U32)(x*4294967296.0F))

#   define DIV2LPC(a) ((a)>>1)
#   define MUL2LPC(a) ((a)<<1)

#   define INTEGER_ONLY_LPC(a) a
#   define INTEGER_OR_INT_FLOAT_LPC(a,b) a

#else // BUILD_INTEGER_LPC

//****************************************************************************
//         Types, constants related to LPC.
//****************************************************************************

//  LSP (aka LSF), used for inverse quantized LSF.
    typedef Float LspType;
#   define LSP_FROM_FLOAT(a) ((LspType)(a))
#   define MULT_LSP(x,y) ((x)*(y))

// LP, used for P, Q, and LPC values
    typedef Float LpType;
#   define LP_FROM_FLOAT(a) ((LpType)(a))
#   define FLOAT_FROM_LP(a) ((Float)(a))

// LP Spectrum: used during FFT of LPC values
// LP Spectrum power: sum-squares of FFT output
    typedef Float LpSpecType;
    typedef Float LpSpecPowerType;
#   define LP_SPEC_FROM_FLOAT(a) ((LpSpecType)(a))
#   define FLOAT_FROM_LP_SPEC(a) ((Float)(a))
#   define LP_SPEC_FROM_LP(a)    ((LpSpecType)(a))

//  Weights, computed from LPC   (and Bark?).
    typedef Float WeightType;
    typedef Float FinalWeightType;
#   define FINALWEIGHT_FROM_FLOAT(flt) (flt)
#   define FINALWEIGHT_FROM_WEIGHT(wt) (wt)
#   define WEIGHT_FROM_FLOAT(a) ((WeightType)(a))
#   define FLOAT_FROM_WEIGHT(a) ((Float)(a))

// Fractional Type used by FFT Trig Recurrsion 
    typedef Float BP2LPCType;
#   define BP2LPC_FRAC_BITS 0
#   define BP2LPC_FRAC_SCALE 1
#   define BP2LPC_FROM_FLOAT(flt) ((BP2LPCType)(flt))
#   define FLOAT_FROM_BP2LPC(bp2) ((Float)(bp2))
#   define BP2LPC_FROM_BP1(bp1) (bp1)
#   define BP2LPC_FROM_BP2(bp2) (bp2)
#   define MULT_BP2LPC(a,b) ((a)*(b))
#   define MULT_BP2LPC_I32(a,b) ((a) * (b))

#   define DIV2LPC(a) ((a)*0.5f)
#   define MUL2LPC(a) ((a)*2.0f)

#   define INTEGER_ONLY_LPC(a)
#   define INTEGER_OR_INT_FLOAT_LPC(a,b) b

#endif // BUILD_INTEGER_LPC


INLINE Float FloatFromCoeff(const CoefType c, int ls)
{
  int rs = ls + COEF_FRAC_BITS;
  Float s = (rs > 0) ? (1.0f/((I64)1<<rs)) : (Float)((I64)1<<-rs);
  return (Float)c*s;
}

INLINE Double DoubleFromCoeff(const CoefType c, int ls)
{
  int rs = ls + COEF_FRAC_BITS;
  Double s = (rs > 0) ? (1.0/((I64)1<<rs)) : (Double)((I64)1<<-rs);
  return (Double)c*s;
}

INLINE I32 CoeffFromDouble(const Double c, int ls)
{
  int rs = ls + COEF_FRAC_BITS;
  Double s = (rs > 0) ? (1.0/((I64)1<<rs)) : (Double)((I64)1<<-rs);
  return (I32)((Double)c/s);
}

INLINE I32 CoeffFromFloat(const Float c, int ls)
{
  int rs = ls + COEF_FRAC_BITS;
  Double s = (rs > 0) ? (1.0/((I64)1<<rs)) : (Double)((I64)1<<-rs);
  return (I32)((Double)c/s);
}

#define CH_FFC(c) (c)

#ifdef BUILD_INTEGER
#define WMACFF(c) CoeffFromFloat(c, ppcinfo->m_cLeftShiftBitsTotal)
#define WMADFC(c) DoubleFromCoeff(c, ppcinfo->m_cLeftShiftBitsTotal)
#define WMACFD(c) CoeffFromDouble(c, ppcinfo->m_cLeftShiftBitsTotal)
#define WMAFFC(c) FloatFromCoeff(c, ppcinfo->m_cLeftShiftBitsTotal)
#define WMACFFPQ(c) CoeffFromFloat(c, ppcinfo->m_cLeftShiftBitsTotal-ppcinfo->m_cLeftShiftBitsQuant)
#define WMADFCPQ(c) DoubleFromCoeff(c, ppcinfo->m_cLeftShiftBitsTotal-ppcinfo->m_cLeftShiftBitsQuant)
#define WMAFFCPQ(c) FloatFromCoeff(c, ppcinfo->m_cLeftShiftBitsTotal-ppcinfo->m_cLeftShiftBitsQuant)
#endif

// some utility macros that are type specific to avoid the compiler's type conversion rules.
#define COEF_ABS(a) (((a)<0) ? -(a) : (a))
#define BPX_ABS(a) (((a)<0) ? -(a) : (a))

#define ABS(x) ((x) > 0 ? (x) : -(x))


#if !(defined(REFERENCE_RAND_24) || defined(REFERENCE_RAND_16))

    //****************************************************************************
    // quick and dirty rand generator, assuming low word multiply, only works for 
    // 32-bit machines see Numerical Recipes p.284.  
    // Note this is the referenced generator but the 24-bit or 16-bit versions can
    // be used for compliance comparisions with 24-bit or 16-bit implementations.
    //****************************************************************************

    typedef struct tagRandState {
        I32 iPrior;         // prior value
        U32 uiRand;         // current value
    } tRandState;

    INLINE I32 quickRand(tRandState* ptRandState)
    {
        const U32 a = 1664525;
        const U32 c = 1013904223;
        I32 iTemp, iTemp1;
        //a*x + c has to be done with unsigned 32 bit
        ptRandState->uiRand =  a * ptRandState->uiRand + c;

        // uiRand values starting from a 0 seed are: 0x3c6ef35f, 0x47502932, 0xd1ccf6e9, 0xaaf95334, 0x6252e503, 0x9f2ec686, 0x57fe6c2d, ...

        // do not change the above - this reference generator has been extensively tested and has excellent randomness properties
        // a truism in the world of random number generator theory and practice is:
        // "any change, no matter how small, can and will change the properties of the generator and must be fully tested"
        // In case you don't know, it can easily take a person-month to fully test a generator.
    
        // Notwithstanding the above, it is fair to take a function of a random number to shape its range or distribution.
        // This we do below to give it a triangular distrbution between -2.5 and 2.5 to roughly approximate a Guassian distribution.

        // cast and shift to make the range (-1, 1) with Binary Point 3.
        iTemp = ((I32)(ptRandState->uiRand)) >> 2; 

        // *1.25 to make the range (-1.25, 1.25) 
        iTemp += (iTemp>>2);
    
        // Difference of two random numbers gives a triangle distribution and a range of (-2.5, 2.5)
        // it also gives a serial correlation of -0.5 at lag 1.  But all the other lags have normally small correlations.
        iTemp1 = iTemp - ptRandState->iPrior;

        // Save first term of this difference for next time.
        ptRandState->iPrior = iTemp;

        //return -2.5 to 2.5 with Binary Point = 3 with a triangle distribution
        return iTemp1;
    }

    INLINE void RandStateClear(tRandState* ptRandState) {
        ptRandState->iPrior = 0;
        ptRandState->uiRand = 0;
    }

    // protect encoder from having these defined by mistake
#   ifdef REFERENCE_RAND_24
#       undef REFERENCE_RAND_24
#   endif
#   ifdef REFERENCE_RAND_16
#       undef REFERENCE_RAND_16
#   endif

#elif defined(REFERENCE_RAND_24)

    //****************************************************************************
    // Quick and dirty 24-bit rand generator.
    // Simulates 24-bits on a 32-bit reference machine.
    // Note this is the 24-bit referenced generator which can be used for compliance 
    // comparisions with 24-bit implementations by defining REFERENCE_RAN_24
    //****************************************************************************
    typedef struct tagRandState {
        I32 iPrior;         // prior value   (only top 24-bits used)
        U32 uiRand;         // current value (only top 24-bits used)
    } tRandState;

    INLINE I32 quickRand(tRandState* ptRandState)
    {
        // OK to 512 lags, then ChiSquare is suspect at 1024 and rejects at 2048 (length 1024000)
        const U32 uLCa = 69857;         // 0x000110E1
        const U32 uLCc = 3546581;       // 0x00361DD5

        I32 iTemp, iTemp1;
        //a*x + c has to be done with unsigned 32 bit - but we similate a 24x24
        ptRandState->uiRand =  (uLCa * ((ptRandState->uiRand)>>8) + uLCc)<<8;

        // uiRand values starting from a 0 seed are: 0x361dd500, 0x78a60a00, 0xaaac9f00, 0xa0c59400, 0x7104e900, 0xc2fe9e00, 0x1fc6b300

        // do not change the above - see comment in 32-bit generator

        // cast and shift to make the range (-1, 1) with Binary Point 3.
        iTemp = (((I32)(ptRandState->uiRand)) >> 2) & 0xFFFFFF00; 

        // *1.25 to make the range (-1.25, 1.25) 
        iTemp += ((iTemp>>2) & 0xFFFFFF00);
    
        // Difference of two random numbers gives a triangle distribution and a range of (-2.5, 2.5)
        // it also gives a serial correlation of -0.5 at lag 1.  But all the other lags have normally small correlations.
        iTemp1 = iTemp - ptRandState->iPrior;

        // Save first term of this difference for next time.
        ptRandState->iPrior = iTemp;

        //return -2.5 to 2.5 with Binary Point = 3 with a triangle distribution
        return iTemp1;
    }

    INLINE void RandStateClear(tRandState* ptRandState) {
        ptRandState->iPrior = 0;
        ptRandState->uiRand = 0;
    }


#elif defined(REFERENCE_RAND_16)
    //****************************************************************************
    // Quick and dirty 16-bit rand generator.
    // Simulates 16-bits on a 32-bit reference machine.
    // Note this is the 16-bit referenced generator for compliance comparisions 
    // with 16-bit implementations by defining REFERENCE_RAN_16.
    // The function actually returns 21 random bits at the top of the 32 
    // so is not a pure 16-bit generator, but does limit itself to a 16x16=>32 multiply
    //****************************************************************************

    // Background
    // A pure 16-bit Linear Congruent generator has limited sequence length.  
    // Use 13 A's and 3 C's choosen from good values in the middle of the range.
    // Tested OK up to lag 1024 (length 32768) and pretty good at 2048 and 2049.
    // Sequence length quite long.

    // these two arrays are defined once in dectables.c
#   define LCA_SET 13
#   define LCC_SET 3
    extern const unsigned int g_uWMALCA[LCA_SET];   // {1637,1033,1825,1621, 1657,1861,1229,1549, 2017,941,1409,1777, 1153};
    extern const unsigned int g_uWMALCC[LCC_SET];   // {13849,13841,13859};

    typedef struct tagRandState {
        I32 iPrior;         // prior value
        U16 uiRand;         // current value
        char bIdxLCA;       // index for uLCA
        char bIdxLCC;       // index for uLCC
    } tRandState;

    INLINE I32 quickRand(tRandState* ptRandState)
    {
        // This 16-bit implementation returns a 32-bit result with the top 21 bits random.
        // But it base implementation is 16x16=>32, e.g. it requires only a single precision 16-bit multiply.

        I32 iTemp, iTemp1;
        U32 uTemp;

        //a*x + c is with unsigned 32 bit - but we similate a 16x16+16 =32
        uTemp =  (g_uWMALCA[ptRandState->bIdxLCA++] * (ptRandState->uiRand)) + g_uWMALCC[ptRandState->bIdxLCC++];

        // save bottom 16 bits
        ptRandState->uiRand  = (unsigned short)uTemp;

        // uiRand values starting from a 0 seed are: 0x3623, 0x259c, 0x5add, 0x5698, 0xb511, 0x78ae, 0x6af9, 0x09f2, 0xc49b, 0x4f3e, 0x4377, 0x1108

        // wrap indexes
        if (ptRandState->bIdxLCA>=LCA_SET) 
            ptRandState->bIdxLCA = 0;
        if (ptRandState->bIdxLCC>=LCC_SET)
            ptRandState->bIdxLCC = 0;

        // get best 19 bits into top
        uTemp <<= 13;           

        // do not change the above - see comment in 32-bit generator

        // cast and shift to make the range (-1, 1) with Binary Point 3.
        iTemp = (((I32)(uTemp)) >> 2); 

        // *1.25 to make the range (-1.25, 1.25) 
        iTemp += (iTemp>>2);
    
        // Difference of two random numbers gives a triangle distribution and a range of (-2.5, 2.5)
        // it also gives a serial correlation of -0.5 at lag 1.  But all the other lags have normally small correlations.
        iTemp1 = iTemp - ptRandState->iPrior;

        // Save first term of this difference for next time.
        ptRandState->iPrior = iTemp;

        //return -2.5 to 2.5 with Binary Point = 3 with a triangle distribution
        return iTemp1;
    }

    INLINE void RandStateClear(tRandState* ptRandState) {
        ptRandState->iPrior = 0;
        ptRandState->uiRand = 0;
        ptRandState->bIdxLCA = 1;
        ptRandState->bIdxLCC = 2;
    }

#endif

//****************************************************************************
// floor of log base 2 of a number which is a power of 2 
//****************************************************************************
INLINE I32 LOG2(U32 i)
{   // returns n where n = log2(2^n) = log2(2^(n+1)-1)
    U32 iLog2 = 0;
    WMAProDec_ASSERT (i != 0);

    while ((i >> iLog2) > 1)
        iLog2++;

    return iLog2;
}

#ifndef macintosh
#define log2 LOG2
#endif

//****************************************************************************
// Normalize a dynamically scaled unsigned int 
//****************************************************************************
INLINE void NormUInt( UInt* puiValue, Int* pcFracBits, const UInt uiTarget ){
    const UInt uiTarget2 = uiTarget>>1;
    register UInt uiV = *puiValue;
    register Int cFB = *pcFracBits;
    WMAProDec_ASSERT( uiV > 0 );    
    if (uiV == 0) return; // useful if asserts are disabled

    while ( uiV < uiTarget2 )
    {
        uiV <<= 2;        
        cFB += 2;
    }
    if ( uiV < uiTarget )
    {
        uiV <<= 1;        
        cFB += 1;
    }
    *puiValue = uiV;
    *pcFracBits = cFB;
}

//****************************************************************************
// Align a dynamically scaled int to a particular binary point position
//****************************************************************************
INLINE Int Align2FracBits( const Int iValue, const Int cFracBits, const Int cAlignFracBits )
{
    const Int iShift = cFracBits-cAlignFracBits;

    if (iShift < 0) {
        return (iValue << -iShift);
    } else if (iShift < 32) { 
        return (iValue >> iShift);
    } else {
        return 0;
    }
}


// V4 Compatibility Mode: This mode allows us to compare V5 encoder with V4 encoder
#ifdef V4V5_COMPARE_MODE
#define V4V5COMPARE_SNAPTOZERO(x)   if (fabs(x) <= 1.0e-15) {(x) = 0;}
#define ROUNDF_V4V5COMPARE(x)  ROUNDD(x)
typedef double V4V5COMPARE;
#else   // V4V5_COMPARE_MODE
#define V4V5COMPARE_SNAPTOZERO(x)
#define ROUNDF_V4V5COMPARE(x)  ROUNDF(x)
typedef float  V4V5COMPARE;
#endif  // V4V5_COMPARE_MODE

#ifdef BUILD_INTEGER
typedef I32     FLOATORI32;
typedef U32     FLOATORU32;
typedef U16     FLOATORU16;
#define FLOATORU32_CONST(flop,fb)           ((U32)((flop)*(1 << (fb))))
#define FLOATORINT_TOFLOAT(x,fb)            ((Float)(x)/(Float)(1 << (fb)))
#define FLOATORINT_SELECTTOFLOAT(f,i,fb)    ((Float)(i)/(Float)(1 << (fb)))
#define FLOATORINT_ASSERT(x)                WMAProDec_ASSERT(x)
#define FLOATORINT_AB(a,b)                  (a)

// The following is used to allow non-integerized code to convert a FLOP
// result to a bit-discarded integer result
#define FLOATORU32_DISCARDBITSFROMFLT(x,fb) ((U32)((x)/(float)(1 << (fb))))
#define FLOATORU32_DISCARDBITSFROMU32(x,fb) ((U32)(x) >> (fb))
#define FLOATORU32_DISCARDBITSFROMI64(x,fb) ((I64)(x) >> (fb))
#define FLOATORU32_ADDFRACTBITS(i,fb)       ((i) << (fb))
#define FLOATORI32_DISCARDBITSFROMI32(x,fb) ((I32)(x) >> (fb))

#else   // BUILD_INTEGER

typedef Float   FLOATORI32;
typedef Float   FLOATORU32;
typedef Float   FLOATORU16;
#define FLOATORU32_CONST(flop,fb)           (flop)
#define FLOATORINT_TOFLOAT(x,fb)            (x)
#define FLOATORINT_SELECTTOFLOAT(f,i,fb)    (f)
#define FLOATORINT_ASSERT(x)
#define FLOATORINT_AB(a,b)                  (b)

#define FLOATORU32_DISCARDBITSFROMFLT(x,fb) (x) // FLOP-to-FLOP, no conversion required
#define FLOATORU32_DISCARDBITSFROMU32(x,fb) ((Float)x)
#define FLOATORU32_DISCARDBITSFROMI64(x,fb) (x)
#define FLOATORU32_ADDFRACTBITS(i,fb)       (i) // FLOP, no fract bits required
#define FLOATORI32_DISCARDBITSFROMI32(x,fb) (x)
#endif  // BUILD_INTEGER


// **************************************************************************
// Bring in platform-specific macros
// **************************************************************************
// Include them here because some of them use the global macros above
#include "wmaOS.h"
#if defined(WMA_TARGET_X86) || defined(WMA_TARGET_QNX_X86)

// Output target
#ifdef COMMONMACROS_OUTPUT_TARGET
//#pragma COMPILER_MESSAGE("-----MACROS.H: Compiling for Intel x86 Target")
#endif  // COMMONMACROS_OUTPUT_TARGET

//#ifdef __QNX__
#include "../x86/macros.h"
//#else
//#include "..\x86\macros.h"
//#endif

//---------------------------------------------------------------------------
#elif defined(WMA_TARGET_MIPS)

// Output target
#ifdef BUILD_INTEGER
#ifdef COMMONMACROS_OUTPUT_TARGET
//#pragma COMPILER_MESSAGE("-----MACROS.H: Compiling for MIPS integer Target")
#endif // COMMONMACROS_OUTPUT_TARGET
#else
#ifdef COMMONMACROS_OUTPUT_TARGET
//#pragma COMPILER_MESSAGE("-----MACROS.H: Compiling for MIPS floating point  Target")
#endif // COMMONMACROS_OUTPUT_TARGET

#endif

#include "..\mips\macros_mips.h"

#elif defined(WMA_TARGET_SH3)

// Output target
#ifdef COMMONMACROS_OUTPUT_TARGET
//#pragma COMPILER_MESSAGE("-----MACROS.H: Compiling for SH3 (Hitachi & HP WinCE) Target")
#endif // COMMONMACROS_OUTPUT_TARGET

#include "..\sh3\macros_sh3.h"

#elif defined(WMA_TARGET_SH4)

// Output target
#ifdef COMMONMACROS_OUTPUT_TARGET
//#pragma COMPILER_MESSAGE("-----MACROS.H: Compiling for SH4 (Hitachi WinCE) Target")
#endif // COMMONMACROS_OUTPUT_TARGET

#ifndef HITACHI
static 
#endif
INLINE U32 ByteSwap(U32 i){ 
    U8 *p = (U8 *)&i;
    return (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|p[3];
}

#include "..\sh4\macros_SH4.h"

//---------------------------------------------------------------------------
#else

// Output target
#ifdef COMMONMACROS_OUTPUT_TARGET
//#pragma COMPILER_MESSAGE("-----MACROS.H: Compiling for ANSI-C Target")
#endif // COMMONMACROS_OUTPUT_TARGET

// Bring in platform-independent macros
#define MULT_HI_DWORD_DOWN(a,b) (I32)((((I64)(a))*((I64)(b)))>>30)
#define MULT_HI_DWORD(a,b) (I32)((((I64)(a))*((I64)(b)))>>32)

// Unsigned version of multiply
#define MULT_HI_UDWORD(a,b) (U32)((((U64)(a))*((U64)(b)))>>32)

static INLINE U32 ByteSwap(U32 i){ 
    U8 *p = (U8 *)&i;
    return (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|p[3];
}

#endif // Platform-specific #ifdefs


// **************************************************************************
// Overridable Macros (optimized for some platforms, but not all)
// **************************************************************************
// The following can be overridden within a particular platform macro file
#ifndef PLATFORM_SPECIFIC_DIVI64BYU32
static INLINE I32 DIVI64BYU32(I64 a, U32 b) {
    return (I32)((a)/(b));
}
#endif

#ifndef PLATFORM_SPECIFIC_CEILOFLOG2
#define CEILOFLOG2(iDivPow2, iMean) {for (iDivPow2 = 0; ((U32)1 << iDivPow2) < iMean; iDivPow2++);} 
#endif //PLATFORM_SPECIFIC_CEILOFLOG2

#ifndef PLATFORM_SPECIFIC_ROUND

#ifdef HITACHI
static I32 ROUNDF(float f) {
#else
static INLINE I32 ROUNDF(float f) {
#endif
    if (f < 0.0f)
        return (I32) (f - 0.5f);
    else
        return (I32) (f + 0.5f);
}

//---------------------------------------------------------------------------
#ifdef HITACHI
static I32 ROUNDD(double f) {
#else
static INLINE I32 ROUNDD(double f) {
#endif
    if (f < 0.0f)
        return (I32) (f - 0.5f);
    else
        return (I32) (f + 0.5f);
}
#endif // PLATFORM_SPECIFIC_ROUND

//---------------------------------------------------------------------------
#ifndef PLATFORM_SPECIFIC_BITCOPY
//assuming bsrc is zeroed out
#ifdef HITACHI
static void bitCpy (const U8* pbSrc, Int iBitStartSrc, Int cBits, U8* pbDst)
#else
static INLINE void bitCpy (const U8* pbSrc, Int iBitStartSrc, Int cBits, U8* pbDst)
#endif
{
    const U8* pbSrcEnd;
    Int iShiftDown;
    U8 b2, b1;

    WMAProDec_ASSERT (pbSrc != NULL && pbDst != NULL);
    WMAProDec_ASSERT (iBitStartSrc < BITS_PER_DWORD && iBitStartSrc >= 0);
    WMAProDec_ASSERT (cBits >= 0);

    // Caller assumes we're copying DWORDs at a time (hangover from Intel)
    // Normalize pointers 
    pbSrc += iBitStartSrc / BITS_PER_BYTE;
    iBitStartSrc %= BITS_PER_BYTE;

    pbSrcEnd = pbSrc + (iBitStartSrc > 0) + 
                            (((cBits - iBitStartSrc) + 7) & ~7) / 8; //open
    iShiftDown = (BITS_PER_BYTE - iBitStartSrc);
    b1 = *pbSrc;
    while (pbSrc < pbSrcEnd) {
        pbSrc++;
        b2 = *pbSrc;
        *pbDst = (b1 << iBitStartSrc) | (b2 >> iShiftDown);
        b1 = b2;
        pbDst++;
    }
}
#endif // PLATFORM_SPECIFIC_BITCOPY

#ifndef PLATFORM_SPECIFIC_FNPTR
#define PLATFORM_SPECIFIC_FNPTR     //nothing for non-x86
#endif // PLATFORM_SPECIFIC_FNPTR


//**********************************************************************
// Support for FastFloat
//**********************************************************************

//#if defined(BUILD_INTEGER)
    // FastFloat
    // FastFloat is a quick way of handling values that exceed I32 range without incurring
    // the expense of floating point emulation on integer only platforms.
    // real value = iFraction * pow( 2, -iFracBits )
    // In debugger, iFraction*1.0F/(1<<iFracBits) works if 0<=iFracBits<31

    // Normalize a FastFloat
#ifdef HITACHI  
//#   pragma inline(Norm4FastFloat)
#else   
    INLINE
#endif
static void Norm4FastFloat( FastFloat* pfflt )
    {   // use the faster Norm4FastFloatU when you know the value is positive
        register UInt uiF = abs(pfflt->iFraction);
        register Int iFB = 0;
        if ( uiF == 0 )
        {
            pfflt->iFracBits = 0;
            return;
        }
        while ( uiF < 0x1FFFFFFF )
        {
            uiF <<= 2;
            iFB +=  2;
        }
        if ( uiF < 0x3FFFFFFF )
        {
            iFB +=  1;
        }
        pfflt->iFraction <<= iFB;
        pfflt->iFracBits += iFB;
    }


#ifdef HITACHI
//#   pragma inline(Norm4FastFloatU)
#else   
    INLINE
#endif
static void Norm4FastFloatU( FastFloat* pfflt )
    {   // same as above when we know value is positive (which we often do)
        register UInt uiF = pfflt->iFraction;
        register Int iFB = 0;
        WMAProDec_ASSERT( uiF > 0 );
        while ( uiF < 0x1FFFFFFF )
        {
            uiF  <<= 2;
            iFB +=  2;
        }
        if ( uiF < 0x3FFFFFFF )
        {
            uiF  <<= 1;
            iFB +=  1;
        }
        pfflt->iFraction = uiF;
        pfflt->iFracBits += iFB;
    }


#ifdef HITACHI
//    #pragma inline(ffltMultiply)
#else   
    INLINE
#endif  
static FastFloat ffltMultiply( FastFloat ffltA, FastFloat ffltB )
    {
        FastFloat ffltP;
        ffltP.iFraction = MULT_HI( ffltA.iFraction, ffltB.iFraction );
        ffltP.iFracBits = (ffltA.iFracBits + ffltB.iFracBits - 31);
        Norm4FastFloat( &ffltP );
        return ffltP;
    }
#   define FASTFLOAT_MULT(a,b) ffltMultiply((a),(b))

#ifdef HITACHI
//    #pragma inline(ffltMultiply)
#else   
    INLINE
#endif  
static FastFloat ffltfltMultiply( FastFloat ffltA, Int B , Int bits)
    {
        FastFloat ffltP;
        ffltP.iFracBits = ffltA.iFracBits;
        ffltP.iFraction = MULT_HI32_SHIFT(ffltA.iFraction, B, bits);
        Norm4FastFloat(&ffltP);
        return ffltP;
    }
#   define FASTFLOAT_FLOAT_MULT(a,b,bits) ffltfltMultiply((a), (b), (bits))

#ifdef HITACHI
//    #pragma inline(ffltMultiply)
#else   
    INLINE
#endif  
static FastFloat ffltAdd( FastFloat ffltA, FastFloat ffltB )
    {
        FastFloat ffltP;

        if (ffltA.iFraction > 0x3FFFFFFF)
        {
            ffltA.iFraction >>= 1;
            ffltA.iFracBits--;
        }
        if (ffltB.iFraction > 0x3FFFFFFF)
        {
            ffltB.iFraction >>= 1;
            ffltB.iFracBits--;
        }

        if (ffltA.iFracBits >= ffltB.iFracBits)
        {
            ffltP.iFracBits = ffltB.iFracBits;
            ffltP.iFraction = ffltB.iFraction + (ffltA.iFraction>>(ffltA.iFracBits-ffltB.iFracBits));
        }
        else
        {
            ffltP.iFracBits = ffltA.iFracBits;
            ffltP.iFraction = ffltA.iFraction + (ffltB.iFraction>>(ffltB.iFracBits-ffltA.iFracBits));
        }
        Norm4FastFloat(&ffltP);
        return ffltP;
    }
#   define FASTFLOAT_ADD(a,b) ffltAdd((a),(b))


#ifdef HITACHI
//    #pragma inline(FastFloatFromFloat)
#else   
    INLINE
#endif  
static FastFloat FastFloatFromFloat(Float flt) {
        FastFloat fflt;
        Float fltScale = (Float)(1<<(31-24));
        fflt.iFracBits = 24;
        while( flt < -fltScale || fltScale < flt )
        {
            fflt.iFracBits -= 1;
            fltScale *= 2;
        }
        fflt.iFraction = (I32)(flt*(1<<fflt.iFracBits));
        Norm4FastFloat( &fflt );
        return fflt;
    }   

    // floor of log2(flt)
#ifdef HITACHI
//    #pragma inline(FastFloatFromFloat)
#else   
    INLINE
#endif  
static Int Log2FromFloat(Float flt) {
        Int i = 0;
        Float fltScale = 2.0;
        WMAProDec_ASSERT(flt >= 0.0);
        while (fltScale <= flt) {
            i++;
            fltScale *= 2;
        }
        return i;
    }

    // floor of log2(fflt.iFraction*2^-fflt.iFracBits)
    //  = floor(log2(fflt.iFraction)) - fflt.iFracBits
#ifdef HITACHI
//    #pragma inline(FastFloatFromFloat)
#else   
    INLINE
#endif  
static Int Log2FromFastFloat(FastFloat fflt) {
        return LOG2(fflt.iFraction) - fflt.iFracBits;
    }

#ifdef HITACHI  
//    #pragma inline(FloatFromFastFloat)
#else   
    INLINE
#endif
static Float FloatFromFastFloat( FastFloat fflt )
    {
        WMAProDec_ASSERT( 0<= fflt.iFracBits && fflt.iFracBits <= 50 );
        if ( fflt.iFracBits > 30 )
            return fflt.iFraction/(1048576.0F*(1<<(fflt.iFracBits-20)));
        else
            return fflt.iFraction/((Float)(1<<fflt.iFracBits));

    }
#   define FASTFLOAT_FROM_FLOAT(flt) FastFloatFromFloat(flt)
#   define FLOAT_FROM_FASTFLOAT(fflt) FloatFromFastFloat(fflt)
#   define DOUBLE_FROM_FASTFLOAT(fflt) ((double)fflt.iFraction/(1<<fflt.iFracBits))


#ifdef HITACHI
//    #pragma inline(FastFloatFromU64)
#else   
    INLINE
#endif  
static FastFloat FastFloatFromU64(U64 u64, Int cExp) 
    {
        FastFloat fflt;
        U32 uiMSF = (U32)(u64>>32);
        Int iExp = 0;
        if ( uiMSF==0 ) {
            iExp = 32;
            uiMSF = (U32)u64;
        }
        if (uiMSF==0) {
            fflt.iFracBits = 0;
            fflt.iFraction = 0;
            return fflt;
        }
        // normalize the most significant fractional part
        while( (uiMSF & 0xF0000000)==0 ) {
            iExp += 3;
            uiMSF <<= 3;
        }
        while( (uiMSF & 0xC0000000)==0 ) {
            iExp++;
            uiMSF <<= 1;
        }
        // number of fractional bits
        fflt.iFracBits = iExp+cExp-32;
#if defined(PLATFORM_OPTIMIZE_MINIMIZE_BRANCHING)
        fflt.iFraction = (U32)((u64<<iExp)>>32);
#else
        fflt.iFraction = (iExp>32) ? (U32)(u64<<(iExp-32)) : (U32)(u64>>(32-iExp));
#endif
        return fflt;
    }   
#define FASTFLOAT_FROM_U64(u64,exp) FastFloatFromU64(u64,exp)


    typedef FastFloat QuantStepType;
#define DOUBLE_FROM_QUANTSTEPTYPE(qst) DOUBLE_FROM_FASTFLOAT(qst)
#define  FLOAT_FROM_QUANTSTEPTYPE(qst) FLOAT_FROM_FASTFLOAT(qst)
#define FASTFLOAT_FROM_QUANTSTEPTYPE(qst) (qst)

//#else   // must be BUILD_INT_FLOAT
//
//#   define FASTFLOAT_FROM_FLOAT(flt) (flt)
//#   define FLOAT_FROM_FASTFLOAT(fflt) (fflt)
//#   define FASTFLOAT_MULT(a,b) ((a)*(b))
//#   define FASTFLOAT_FLOAT_MULT(a,b,bits) ((a)*(b))
//#   define FASTFLOAT_ADD(a,b) ((a)+(b))
//#   define DOUBLE_FROM_FASTFLOAT(fflt) ((double)fflt)
//
//#   define FASTFLOAT_FROM_U64(u64,exp) (((Float)(u64))/(((U64)1)<<exp))
//
//    typedef Float QuantStepType;
//#define DOUBLE_FROM_QUANTSTEPTYPE(qst) ((Double)(qst))
//#define  FLOAT_FROM_QUANTSTEPTYPE(qst) (qst)
//#define FASTFLOAT_FROM_QUANTSTEPTYPE(qst) ((FastFloat)(qst))

//#endif


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Define Macros to switch auReconMono and auSaveHistoryMono between Integer and Float

#define ROUND_SATURATE_STORE16(piDst,cf,mn,mx,iTmp)      \
    WMAProDec_ASSERT(pau->m_nBytePerSample == 2);                  \
    *((I16*)piDst) = (I16) checkRange (cf, mn, mx);      \
    iTmp = cf;

#define ROUND_SATURATE_STORE24(piDst,cf,mn,mx,iTmp)      \
    WMAProDec_ASSERT(pau->m_nBytePerSample == 3);                  \
    prvSetSample24INLINE(checkRange (cf, mn, mx), piDst, pau, 0);   \
    iTmp = cf;

#define ROUND_SATURATE_STORE2024(piDst,cf,mn,mx,iTmp)    \
    WMAProDec_ASSERT(pau->m_nBytePerSample == 3);                  \
    prvSetSample2024INLINE(checkRange (cf, mn, mx), piDst, pau, 0); \
    iTmp = cf;

#define ROUND_SATURATE_STORE(piDst,cf,mn,mx,iTmp)      \
    pau->m_pfnSetSample((PCMSAMPLE) checkRange (cf, mn, mx), piDst, pau, 0);    \
    iTmp = cf;

#define ROUND_AND_CHECK_RANGE(it,flt,mn,mx) \
    if (flt < mn) \
        it = mn; \
    else if (flt > mx) \
        it = mx; \
    else \
        it = flt;

// For lossless mode. reconstructed values are I32.
#define ROUND_SATURATE_STORE16_LLM(piDst,iResult,mn,mx)           \
    WMAProDec_ASSERT(pau->m_nBytePerSample == 2);                           \
    iResult = checkRange(iResult,mn,mx);                          \
    *((I16*)piDst) = (I16)iResult;

#define ROUND_SATURATE_STORE24_LLM(piDst,iResult,mn,mx)           \
    WMAProDec_ASSERT(pau->m_nBytePerSample == 3);                           \
    iResult = checkRange(iResult,mn,mx);                          \
    prvSetSample24INLINE(iResult, piDst, pau, 0);

#define ROUND_SATURATE_STORE2024_LLM(piDst,iResult,mn,mx)         \
    WMAProDec_ASSERT(pau->m_nBytePerSample == 3);                           \
    iResult = checkRange(iResult,mn,mx);                          \
    prvSetSample2024INLINE(iResult, piDst, pau, 0);

#define ROUND_SATURATE_STORE_LLM(piDst,iResult,mn,mx)             \
    iResult = checkRange(iResult,mn,mx);                          \
    pau->m_pfnSetSample((PCMSAMPLE)iResult, piDst, pau, 0);

//V3 LLM
// Dump data (float or int) to a file. If file exists, attach the data to the end.


//  Examples:
//
//  DUMPFLOATDATATOFILE(rgfltInput0,    iLen, "c:\\temp\\input2.dat");
//  DUMPFLOATDATATOFILE(rgfltInput1,    iLen, "c:\\temp\\input3.dat");
//

#define WMA_TRY_AND_EXCEPT_AV(fn) return fn;

//#define ENCODE_FOR_XBOX
//#define PRINT_STATS

//#define SDE_WANTS_PRINTFIO

#ifdef FORCEINLINE
#undef FORCEINLINE
#endif

//#if defined(UNDER_CE) && !defined(DEBUG) && !defined(__arm)
//#   define FORCEINLINE __forceinline
//#else
#   define FORCEINLINE __inline
//#endif

//FORCEINLINE int WMAPrintf(const char* pszFmt, ... )
//{
////#if !defined (UNDER_CE) || defined (SDE_WANTS_PRINTFIO)
//    int ret;
//    va_list vargs;
//    va_start(vargs, pszFmt);
//    ret = vprintf(pszFmt, vargs);
//    va_end(vargs);
//    return ret;
////#else
////    return 0;
////#endif
//}

//FORCEINLINE int WMAFprintf(FILE *stream, const char* pszFmt, ... )
//{
////#if !defined (UNDER_CE) || defined (SDE_WANTS_PRINTFIO)
//    int ret;
//    va_list vargs;
//    va_start(vargs, pszFmt);
//    ret = vfprintf(stream, pszFmt, vargs);
//    va_end(vargs);
//    return ret;
////#else
////    return 0;
////#endif
//}

//FORCEINLINE int WMASprintf(char *buffer, const char* pszFmt, ... )
//{
////#if !defined (UNDER_CE) || defined (SDE_WANTS_PRINTFIO)
//    int ret;
//    va_list vargs;
//    va_start(vargs, pszFmt);
//    ret = vsprintf(buffer, pszFmt, vargs);
//    va_end(vargs);
//    return ret;
////#else
////    return 0;
////#endif
//}

//#if !defined (UNDER_CE) || defined (SDE_WANTS_PRINTFIO)
//    #ifdef _XBOX
//        #define XFILE void
//        #define STDOUT (void *)
//        extern XFILE * _XFOPEN(char *filename, char *mode);
//        extern int  _XFSEEK(XFILE *strmnum, long offset, int origin);
//        extern int  _XFWRITE(const void *buffer, int size, int count, XFILE *strmnum);
//        extern long  _XFTELL(XFILE *strmnum);
//        extern int  _XFCLOSE(XFILE *strmnum);
//        extern int  _XFFLUSH();
//        extern int  _XFPRINTF(XFILE *strmnum, const char *format, ...);
//        #define FOPEN _XFOPEN
//        #define FSEEK _XFSEEK
//        #define FTELL _XFTELL
//        #define FCLOSE _XFCLOSE
//        #define FWRITE _XFWRITE
//        #define FFLUSH _XFFLUSH
//        #define WMAFprintf _XFPRINTF
//    #else
//        #define FOPEN   fopen
        #define FSEEK   fseek
        #define FTELL   ftell
//        #define FCLOSE  fclose
//        #define FWRITE  fwrite
//        #define FREAD   fread
        #define FFLUSH  fflush
//    #endif //_XBOX
//#else
//    #define FOPEN(a, b)        NULL
//    #define FSEEK(a, b, c)     -1
//    #define FTELL(a)           -1
//    #define FCLOSE(a)          EOF
//    #define FWRITE(a, b, c, d) 0
//    #define FREAD(a, b, c, d)  0
//    #define FFLUSH(a)          EOF
//#endif //!defined (UNDER_CE) || defined (SDE_WANTS_PRINTFIO)

//#define DEBUG_ENCODE_FOR_XBOX


#endif  // __COMMON_MACROS_H


/***************************  Fraunhofer IIS CDK Tools  **********************

                        (C) Copyright Fraunhofer IIS (2003)
                               All Rights Reserved

   $Id$
   Author(s):
   Description: type defines for various processors and compiler tools

   This software and/or program is protected by copyright law and international
   treaties. Any reproduction or distribution of this software and/or program,
   or any portion of it, may result in severe civil and criminal penalties, and
   will be prosecuted to the maximum extent possible under law.

******************************************************************************/

#if !defined(__MACHINE_TYPE_H__)
#define __MACHINE_TYPE_H__

#ifdef __arm
#include "AIT_MP3HDDec_Setting.h"
#endif

/*
  #MULTI_INSTANCES\n

    -- If set, all RAM of all modules using the CDK_tools is dynamic allocated. Multi instances of all modules are supported.\n
    -- If not set, modules are allocated once in fixed memory areas. */

#define MULTI_INSTANCES


//samyu add str
#ifdef AIT_RELEASE_MP3HD_LIB
#define USE_AIT_SET_WORKING_BUFFER
#endif
//samyu add end


/*
  #GET_EXTERNAL_MEMORY\n

    -- If set, all modules using the CDK_tools doesn't allocate RAM by themselves. Pointer to memory areas will be handed in and
       the CDK_tools administrate and distribute parts of this areas to the different modules.\n
    -- If not set, modules are allocated dynamically (see MULTI_INSTANCES). */

#ifndef USE_AIT_SET_WORKING_BUFFER //samyu add str
//#define GET_EXTERNAL_MEMORY
#endif //samyu add end

/* force CDK_ASSERT()s to work, regardless of compile mode (even in release) */
//#define CDK_ASSERT_ENABLE

#if defined(_TMS320C6400)

  /* char        8 bits */
  /* short      16 bits */
  /* int        32 bits */
  /* long       40 bits */
  /* long long   ? bits */
  /* float      32 bits */
  /* double     64 bits */

  typedef signed int INT;
  typedef unsigned int UINT;
  typedef signed int LONG;
  typedef unsigned int ULONG;
  typedef signed short SHORT;
  typedef unsigned short USHORT;
  /*typedef _int64 INT64; */
  /* #define INT64(a) a##ULL */

  #ifdef _DEBUG
    #define CDK_ASSERT(a) if (!(a)) CDKprintfErr("Assert " #a " failed!\n")
  #else
    #define CDK_ASSERT(a)
  #endif

  #define SHORT_BITS 16
  #define CHAR_BITS 8
  typedef signed char SCHAR;
  typedef unsigned char UCHAR;

#elif defined(_TMS320C6700)

  /* char        8 bits */
  /* short      16 bits */
  /* int        32 bits */
  /* long       40 bits */
  /* long long  64 bits */
  /* float      32 bits */
  /* double     64 bits */

  typedef signed int INT;
  typedef unsigned int UINT;
  typedef signed int LONG;
  typedef unsigned int ULONG;
  typedef signed short SHORT;
  typedef unsigned short USHORT;

  #ifdef _DEBUG
    #define CDK_ASSERT(a) if (!(a)) CDKprintfErr("Assert " #a " failed!\n")
  #else
    #define CDK_ASSERT(a)
  #endif

  #define SHORT_BITS 16
  #define CHAR_BITS 8
  /*typedef char CHAR;*/
  typedef signed char SCHAR;
  typedef unsigned char UCHAR;



#elif defined(__TMS320C55X__)  /* careful: int is 16 bits */

  /* char         16 bits */
  /* short        16 bits */
  /* int          16 bits */
  /* long         32 bits */
  /* long long    40 bits */
  /* float        32 bits */
  /* double       32 bits */
  /* long double  32 bits */

  typedef signed long INT;
  typedef unsigned long UINT;
  typedef signed long LONG;
  typedef unsigned long ULONG;
  typedef signed short SHORT;
  typedef unsigned short USHORT;
  /*typedef long long  int64;*/
  /*#define INT64(a) a##ULL*/

  #define SHORT_BITS 16
  #define CHAR_BITS 16

  typedef signed char SCHAR;
  typedef unsigned char UCHAR;

  #define CDK_ASSERT(a) if (!(a)) if (!(a)) CDKprintfErr("Assert " #a " failed!\n")

#else /* Any other toolchain */

  typedef signed int INT;
  typedef unsigned int UINT;
  typedef signed long LONG;
  typedef unsigned long ULONG;
  typedef signed short SHORT;
  typedef unsigned short USHORT;
  typedef signed char SCHAR;
  typedef unsigned char UCHAR;

  #define SHORT_BITS 16
  #define CHAR_BITS 8

#endif /* Toolchain specific base types. */

/* Windows Mobile workaround. */
#ifdef WIN32_WCE
  #define _WIN32_WCE
#endif

#ifdef _WIN32_WCE
  #if !defined(_DEBUG) || defined(CDK_ASSERT_ENABLE)
    #define CDK_ASSERT(exp) if (!(exp)) CDKprintfErr("assert failed at file %s, line %d\n", __FILE__, __LINE__);
  #endif
#endif

#ifdef _WIN32
  /* Define 64bit base integer type. */
  #define BASETYPES
  typedef __int64 INT64;
  typedef unsigned __int64 UINT64;
  typedef unsigned long *PULONG;
#else /* _WIN32 */
  typedef long long INT64;
  typedef unsigned long long UINT64;
#endif /* _WIN32 */

/* Use assert only on x86 PC's and when debugging is turned on. */
#ifndef CDK_ASSERT
  #if (defined(_M_IX86) && !defined(NDEBUG)) || defined(__i686__) || defined(__i586__) || defined(__i386__) || defined(CDK_DEBUG) || defined(DEBUG) || defined(CDK_ASSERT_ENABLE)
    #include <assert.h>
  #else
    #define assert(ignore) ((void)0)
  #endif

  #define CDK_ASSERT assert
#endif

/* These defines determine the maximum supported PCM quantization rate.
   They must be set before compilation. For example, libraries compiled with PCM16
   do not support 24 bit samples. */
#ifdef CONF_TEST
  #define PCM24
#else
  #define PCM16
  /*#define PCM24*/
  /*#define PCM32*/
#endif

#if defined(PCM24)
  typedef LONG  INT_PCM ;
  #define WAV_BITS 24
  #define SAMPLE_BITS     32 /* Bits used for storage per sample. */
  #define SAMPLE_MAX (((LONG)1<<(SAMPLE_BITS-1))-1)
  #define SAMPLE_MIN (~SAMPLE_MAX)
#elif defined(PCM32)
  #define WAV_BITS 32
  typedef LONG  INT_PCM ;
  #define SAMPLE_BITS     32 /* Bits used for storage per sample. */
  #define SAMPLE_MAX (((LONG)1<<(SAMPLE_BITS-1))-1)
  #define SAMPLE_MIN (~SAMPLE_MAX)
#elif defined(PCM16)
  #define WAV_BITS 16
  typedef SHORT INT_PCM ;
  #define SAMPLE_BITS     16 /* Bits used for storage per sample. */
  #define SAMPLE_MAX (((LONG)1<<(SAMPLE_BITS-1))-1)
  #define SAMPLE_MIN (~SAMPLE_MAX)
#else
  /* INT_PCM is not defined. */
  #error
#endif

#ifndef NULL
#ifdef __cplusplus
  #define NULL    0
#else
  #define NULL    ((void *)0)
#endif
#endif


#if defined(__CC_ARM)
  #define ALIGNMENT_DEFAULT 8
  #define RAM_ALIGN __align(8)
#elif defined(_WIN32_WCE) && defined(_ARM_)
  #define ALIGNMENT_DEFAULT 8
  #define RAM_ALIGN
  #pragma pack(8)
#elif defined(__GNUC__)
  #define ALIGNMENT_DEFAULT 8
  #define RAM_ALIGN __attribute__((aligned(ALIGNMENT_DEFAULT)))
#elif defined(__ADSPBLACKFIN__)
  #define ALIGNMENT_DEFAULT 4
  #define RAM_ALIGN
  #pragma pack(4)
#elif defined(_TMS320C6400) || defined(_TMS320C6700)
  #define ALIGNMENT_DEFAULT 8
  #define RAM_ALIGN
#else
  #define RAM_ALIGN
  #define ALIGNMENT_DEFAULT 8
#endif

#if defined(_TMS320C6400)
  #include <assert.h>
  #define RESTRICT restrict
  #define WORD_ALIGNED(x) (_nassert(((int)x % 4) == 0))
  #define DWORD_ALIGNED(x) (_nassert((int)x % 8 == 0))
#elif defined(__ADSPBLACKFIN__) && !defined(BFIN)
  #define RESTRICT restrict
  #define WORD_ALIGNED(x)
  #define DWORD_ALIGNED(x)
#else
  #define RESTRICT
  #define WORD_ALIGNED(x)
  #define DWORD_ALIGNED(x)
#endif

#define ALIGN_SIZE(a) ( (((LONG)(a))+(ALIGNMENT_DEFAULT-1) & ~(ALIGNMENT_DEFAULT-1) ))

/* Hardware weaknesses/limitations */

/* Enforce extra checks on multiplication usage for 16 bit machines, where sign flipping may occur. */
#if defined(__ADSPBLACKFIN__) || defined(BFIN) || defined(__TMS320C55X__) || defined(_TMS320C6400) || defined(_TMS320C6700)
#define ARCH_WA_16BITMULT
#endif

/* Hardware where the console is extremely slow (stdio through jtag polling). */
#if defined(__ADSPBLACKFIN__) || defined(__TMS320C55X__) || defined(_TMS320C6400) || defined(_TMS320C6700) || defined (__TI_TMS470_V5__)
#define ARCH_WA_SLOWCON
#endif

/* Hardware where commandline parameter passing is not possible. */
#if (!defined(BFIN) && defined(__ADSPBLACKFIN__)) || defined(__TMS320C55X__) || defined(_TMS320C6400) || defined (__TI_TMS470_V5__) || defined (_TMS320C6700)
#define ARCH_WA_NOCMDLINE
#endif

/* Platform where flushing of console pipes is required in order to get output shown. */
#if defined(_TMS320C6400) || defined(_TMS320C55xx_)|| defined(__ADSPBLACKFIN__) || defined(_TMS320C6700)
#define ARCH_WA_FLUSH_CONSOLE
#endif


/* Code Section macros. */

#if defined(__ADSPBLACKFIN__) && !defined(BFIN)
  #define LNK_SECTION_CODE_L1 section("L1_code")
  #define LNK_SECTION_CODE_L2 section("L2_code")
  #define LNK_SECTION_INITCODE
#elif defined(__GNUC__) && defined(BFIN)
  #define LNK_SECTION_CODE_L1 //__attribute__ ((l1_text))
  #define LNK_SECTION_CODE_L2
  #define LNK_SECTION_INITCODE
#elif defined(_TMS320C6400)
  #define LNK_SECTION_CODE_L1 /*asm(".usect \".L1_code\"");*/
  #define LNK_SECTION_CODE_L2 /*asm(".usect \".L1_code\"");*/
  #define LNK_SECTION_INITCODE /*asm(".usect \".sdram\"");*/
#else
  #define LNK_SECTION_CODE_L1
  #define LNK_SECTION_CODE_L2
  #define LNK_SECTION_INITCODE
#endif

/* Memory section macros. */

#if defined(__ADSPBLACKFIN__) && !defined(BFIN)
  #define LNK_SECTION_DATA
  #define LNK_SECTION_DATA_L1 section("L1_data")
  #define LNK_SECTION_CONSTDATA
  #define LNK_SECTION_CONSTDATA_L1 section("L1_data")

  #define LNK_SECTION_L1_DATA_A section("L1_data_a")
  #define LNK_SECTION_L1_DATA_B section("L1_data_b")
#elif defined(BFIN)
  #define LNK_SECTION_DATA
  #define LNK_SECTION_DATA_L1 //__attribute__ ((l1_data))
  #define LNK_SECTION_CONSTDATA
  #define LNK_SECTION_CONSTDATA_L1 //__attribute__ ((l1_data))

  #define LNK_SECTION_L1_DATA_A //__attribute__ ((l1_data_A))
  #define LNK_SECTION_L1_DATA_B //__attribute__ ((l1_data_B))

#elif defined(_TMS320C6400)
  #define LNK_SECTION_DATA_L1 /*#pragma DATA_SECTION("L1_data")*/
  #define LNK_SECTION_DATA_L2
  #define LNK_SECTION_CONSTDATA
  #define LNK_SECTION_CONSTDATA_L1

  #define LNK_SECTION_L1_DATA_A
  #define LNK_SECTION_L1_DATA_B
#else
  /* default fall back */
  #define LNK_SECTION_DATA_L1
  #define LNK_SECTION_DATA_L2
  #define LNK_SECTION_CONSTDATA
  #define LNK_SECTION_CONSTDATA_L1

  #define LNK_SECTION_L1_DATA_A
  #define LNK_SECTION_L1_DATA_B
#endif


typedef enum {
  /* Internal */

  SECT_DATA_L1 = 0x2000,
  SECT_DATA_L2,
  SECT_DATA_L1_A,
  SECT_DATA_L1_B,
  SECT_CONSTDATA_L1,

  /* External */
  SECT_DATA_EXTERN = 0x4000,
  SECT_CONSTDATA_EXTERN

} MEMORY_SECTION;

/* Memory allocation macros */

/**
 * These macros declare memory areas. The H_ prefix indicates header file version, the C_
 * prefix indicates the corresponding object version.
 *   Declaring memory areas requires to specify a unique name and a data type. Use the H_ macro
 * for this purpose inside a header file.
 *   For defining a memory area your require additionally one or two sizes, depending if the
 * memory should be organized into one or two dimensions.
 *   The macros containing the keyword AALLOC instead of ALLOC also do take care of returning
 * aligned memory addresses (beyond the natural alignment of its type). The preprocesor macro
 * ALIGNMENT_DEFAULT indicates the aligment to be used (this is hardware specific).
 *   The _L suffix indicates that the memory will be located in a specific section. This is
 * useful to allocate critical memory section into fast internal SRAM for example.
 *   If the preprocessor macro MULTI_INSTANCES is defined, then heap memory will be used. If
 * not then static memory will be used, thus limiting to only one instance at a time.
 * To get the memory during runtime, call the function Get{name}(). Free'ing memory is done
 * by he functions Free{name}(). The free functions must be called always, even when using
 * the static memory setting and it would maybe not produce any error. This if for the sake of
 * consistency.
 */

#define H_ALLOC_MEM(name,type)          type * Get ## name(int n=0); void Free ## name(type** p);

#define H_ALLOC_MEM_OVERLAY(name,type)  type * Get ## name(void); void Free ## name(type** p); \
                                        type * GetArea ## name(void); void SetArea ## name(void *addr, INT asize);

#ifndef MULTI_INSTANCES

  #define C_ALLOC_MEM(name,type,num) \
    type name[num]; \
    type* Get ## name (int n) { CDK_ASSERT((n) == 0); CDKmemclear(name, (num)*sizeof(type)); return (name); } \
    void Free ## name(type** p) {;}

  #define C_ALLOC_MEM_STATIC(name,type,num) \
    type name[num]; \
    static type* Get ## name (int n) { CDK_ASSERT((n) == 0); CDKmemclear(name, (num)*sizeof(type)); return (name); } \
    static void Free ## name(type** p) {;}

  #define C_ALLOC_MEM2(name,type,n1,n2) \
    type name[n2][n1]; \
    type* Get ## name (int n) { CDK_ASSERT((n) < (n2)); CDKmemclear(name[n], (n1)*sizeof(type)); return (name[n]); } \
    void Free ## name(type** p) {;}

  #define C_AALLOC_MEM(name,type,num) \
    type name[(num)+((ALIGNMENT_DEFAULT+sizeof(type)-1)/sizeof(type))]; \
    type* Get ## name (int n) { CDK_ASSERT((n) == 0); CDKmemclear(name, (num)*sizeof(type)); return (type*)ALIGN_SIZE(name); } \
    void Free ## name(type** p) {;}

  #define C_ALLOC_MEM_OVERLAY(name,type,num,sect,tag) \
    type * Get ## name(void) { return ((type*)CDKgetOverlay((num)*sizeof(type), tag, sect)); } \
    type * GetArea ## name(void) { return ((type*)CDKgetOverlayArea((num)*sizeof(type), tag)); } \
    void SetArea ## name(void *addr, INT asize) { CDKsetOverlayArea(addr, asize, tag); } \
    void Free ## name(type** p) { if (p != NULL) { *p=0; } }

  #define C_AALLOC_MEM2(name,type,n1,n2) \
    type name[n2][(n1)+((ALIGNMENT_DEFAULT+sizeof(type)-1)/sizeof(type))]; \
    type* Get ## name (int n) { CDK_ASSERT((n) < (n2)); CDKmemclear(name[n], (n1)*sizeof(type)); return (type*)ALIGN_SIZE(name[n]); } \
    void Free ## name(type** p) {;}

  #define C_ALLOC_MEM_L(name,type,num,s) \
    type * Get ## name(int n) { CDK_ASSERT((n) == 0); return ((type*)CDKcalloc_L(num, sizeof(type), s)); } \
    void Free ## name(type** p) { if (p != NULL) { CDKfree_L(*p); *p=0; } }

  #define C_ALLOC_MEM2_L(name,type,n1,n2,s) \
    type * Get ## name (int n) { CDK_ASSERT((n) < (n2)); return (type*)CDKcalloc_L(n1, sizeof(type), s); } \
    void Free ## name(type** p) { if (p != NULL) { CDKfree_L(*p); *p=0; } }

  #define C_AALLOC_MEM_L(name,type,num,s) \
    type * Get ## name(int n) { CDK_ASSERT((n) == 0); return ((type*)CDKaalloc_L((num)*sizeof(type), ALIGNMENT_DEFAULT, s)); } \
    void Free ## name(type** p) { if (p != NULL) { CDKafree_L(*p); *p=0; } }

  #define C_AALLOC_MEM2_L(name,type,n1,n2,s) \
    type * Get ## name (int n) { CDK_ASSERT((n) < (n2)); return ((type*)CDKaalloc_L((n1)*sizeof(type), ALIGNMENT_DEFAULT, s)); } \
    void Free ## name(type** p) { if (p != NULL) { CDKafree_L(*p); *p=0; } }

#else /* !MULTI_INSTANCES */

  #define C_ALLOC_MEM(name,type,num) \
    type * Get ## name(int n) { CDK_ASSERT((n) == 0); return ((type*)CDKcalloc(num, sizeof(type))); } \
    void Free ## name(type** p) { if (p != NULL) { CDKfree(*p); *p=0; } }

  #define C_ALLOC_MEM_STATIC(name,type,num) \
    static type * Get ## name(int n) { CDK_ASSERT((n) == 0); return ((type*)CDKcalloc(num, sizeof(type))); } \
    static void Free ## name(type** p) { if (p != NULL) { CDKfree(*p); *p=0; } }

  #define C_ALLOC_MEM2(name,type,n1,n2) \
    type * Get ## name (int n) { CDK_ASSERT((n) < (n2)); return ((type*)CDKcalloc(n1, sizeof(type))); } \
    void Free ## name(type** p) { if (p != NULL) { CDKfree(*p); *p=0; } }

  #define C_AALLOC_MEM(name,type,num) \
    type * Get ## name(int n) { CDK_ASSERT((n) == 0); return ((type*)CDKaalloc((num)*sizeof(type), ALIGNMENT_DEFAULT)); } \
    void Free ## name(type** p) { if (p != NULL) { CDKafree(*p); *p=0; } }

  #define C_ALLOC_MEM_OVERLAY(name,type,num,sect,tag) \
    type * Get ## name(void) { return ((type*)CDKgetOverlay((num)*sizeof(type), tag, sect)); } \
    type * GetArea ## name(void) { return ((type*)CDKgetOverlayArea((num)*sizeof(type), tag)); } \
    void SetArea ## name(void *addr, INT asize) { CDKsetOverlayArea(addr, asize, tag); } \
    void Free ## name(type** p) { if (p != NULL) { CDKfreeOverlay(*p); *p=0; } }

  #define C_AALLOC_MEM2(name,type,n1,n2) \
    type * Get ## name (int n) { CDK_ASSERT((n) < (n2)); return ((type*)CDKaalloc((n1)*sizeof(type), ALIGNMENT_DEFAULT)); } \
    void Free ## name(type** p) { if (p != NULL) { CDKafree(*p); *p=0; } }

  #define C_ALLOC_MEM_L(name,type,num,s) \
    type * Get ## name(int n) { CDK_ASSERT((n) == 0); return ((type*)CDKcalloc_L(num, sizeof(type), s)); } \
    void Free ## name(type** p) { if (p != NULL) { CDKfree_L(*p); *p=0; } }

  #define C_ALLOC_MEM2_L(name,type,n1,n2,s) \
    type * Get ## name (int n) { CDK_ASSERT((n) < (n2)); return (type*)CDKcalloc_L(n1, sizeof(type), s); } \
    void Free ## name(type** p) { if (p != NULL) { CDKfree_L(*p); *p=0; } }

  #define C_AALLOC_MEM_L(name,type,num,s) \
    type * Get ## name(int n) { CDK_ASSERT((n) == 0); return ((type*)CDKaalloc_L((num)*sizeof(type), ALIGNMENT_DEFAULT, s)); } \
    void Free ## name(type** p) { if (p != NULL) { CDKafree_L(*p); *p=0; } }

  #define C_AALLOC_MEM2_L(name,type,n1,n2,s) \
    type * Get ## name (int n) { CDK_ASSERT((n) < (n2)); return ((type*)CDKaalloc_L((n1)*sizeof(type), ALIGNMENT_DEFAULT, s)); } \
    void Free ## name(type** p) { if (p != NULL) { CDKafree_L(*p); *p=0; } }

#endif /* MULTI_INSTANCES */


/* Linker macros */
#if defined(__SYMBIAN32__)
  #include <e32cons.h>
  #define LINKSPEC_H IMPORT_C
  #define LINKSPEC_CPP EXPORT_C
#else
  #define LINKSPEC_H
  #define LINKSPEC_CPP
#endif

#endif /* __MACHINE_TYPE_H__ */

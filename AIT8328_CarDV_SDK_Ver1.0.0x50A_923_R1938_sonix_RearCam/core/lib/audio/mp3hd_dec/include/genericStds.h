/***************************  Fraunhofer IIS CDK Tools  **********************

                        (C) Copyright Fraunhofer IIS (2002)
                               All Rights Reserved

   $Id$
   Author(s):
   Description: - Generic memory, stdio, string, etc. function wrappers or
                  builtins.
                - OS dependant function wrappers.

   This software and/or program is protected by copyright law and international
   treaties. Any reproduction or distribution of this software and/or program,
   or any portion of it, may result in severe civil and criminal penalties, and
   will be prosecuted to the maximum extent possible under law.

******************************************************************************/

#if !defined(__GENERICSTDS_H__)
#define __GENERICSTDS_H__

#ifdef __arm
#include "AIT_MP3HDDec_Setting.h"
#endif

#include "machine_type.h"

#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif

#ifdef UNICODE
  #define ARGV_TYPE wchar_t*
#else
  #define ARGV_TYPE char*
#endif

#ifndef AIT_RELEASE_MP3HD_LIB
/* Platform specific tweaks */
#if defined(_WIN32_WCE)
  #if defined (_M_CEE)
	typedef struct { void* p[4]; } va_list;
  #else
	typedef char *va_list;
  #endif
#elif defined(__ADSPBLACKFIN__) && !defined(BFIN)
  #include <stdlib.h>
  #include <stdio.h>
#endif

#include <stdarg.h>
#include <limits.h>
#endif

/* Runtime support declarations */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AIT_RELEASE_MP3HD_LIB
void CDKprintf    ( const char* szFmt, ...);
void CDKprintfErr ( const char* szFmt, ...);
char CDKgetchar(void);
#if !defined(__SYMBIAN32__)
INT  CDKfprintf(void  *stream,  const  char *format, ...);
INT  CDKsprintf(char *str, const char *format, ...);
#endif
extern void RTNA_DBG_Str(unsigned int level, const char *str);
extern void RTNA_DBG_Byte(unsigned int  level, unsigned char val);
extern void RTNA_DBG_Long(unsigned int level, unsigned int val);
#else
extern void RTNA_DBG_Str(unsigned int level, const char *str);
extern void RTNA_DBG_Byte(unsigned int  level, unsigned char val);
extern void RTNA_DBG_Long(unsigned int level, unsigned int val);

#define CDKprintf(X...) {}
#define CDKprintfErr(X) {}
#define DKfprintf(X,  Y...) {}
#define CDKsprintf(X, Y...) {}
#endif

#if defined(CDK_DEBUG)

extern int CDKdebugLev;

#define CDK_DEBUG_LEV_MEM 0x4 /*!< Common debug level for memory
                                requirements */
#define CDK_DEBUG_LEV_MIN 0x8 /*!< Minimum debug level for other
                                 application must reside _above_ this
                                 value */

#define CDK_DEBUG_EXTHLP "\
--- CDKlib ---\n\
    0x00000001 Arithmetic related info, e.g, saturation\n\
    0x00000002 Overflow-bits info for accumulators\n\
    0x00000004 Memory Requirements\n\
"

/*!
  \page debug03 Debugging Switches (CDKlib)

  Debugging messages are controlled by one bit per debugging level.
  I.e. to get all debugging output, specify -d 0x03ff0000 on the
  command line.

  \li \c 0x01 Arithmetic related info (e.g. saturation)
  \li \c 0x02 Overflow-bits info
  \li \c 0x04 Memory Requirements
  \li \c 0x08 RESERVED
*/

#define CDK_NODEBUG                0x00
#define CDK_INFO                   0x01
#define CDK_OVERFLOW               0x02
#define CDK_RESERVED2              0x04
#define CDK_RESERVED3              0x8

#define CDK_DEBUG_LEV_INTERNAL_MAX CDK_RESERVED3

#if CDK_DEBUG_LEV_MIN < CDK_DEBUG_LEV_INTERNAL_MAX
#error CDK_DEBUG_LEV_MIN (genericStds.h) less than CDK_DEBUG_LEV_INTERN_MAX
#endif

void   CDKdprintf(INT level, char *format, ...);

#endif /* defined(CDK_DEBUG) */


const char *CDKstrchr(const char *s, INT c);
const char *CDKstrstr(const char *haystack, const char *needle);
char *CDKstrcpy(char *dest, const char *src);
char *CDKstrncpy(char *dest, const char *src, const UINT n);

//samyu add str
#ifdef USE_AIT_SET_WORKING_BUFFER
int AITMP3HDDecS_SetWorkingBuffer (void *working_buffer, unsigned int working_buffer_bytesize);
#endif
//samyu add end

#ifdef GET_EXTERNAL_MEMORY

  UINT CDK_GetMemRequire(void);

  int  CDK_FeedMemory(const void *pStatMem,
                      const UINT  statSize,
                      const void *pDynMem,
                      const UINT  dynSize,
                      const void *pScratchMem,
                      const UINT  scratchSize);

  void *CDK_GetStaticMem(const UINT num, const UINT tsize);

  void  CDK_FreeStaticMem(void *ptr);

#endif /* GET_EXTERNAL_MEMORY */

void *CDKcalloc (const UINT n, const UINT size);
void *CDKmalloc (const UINT size);
void  CDKfree   (void *ptr);

/**
 * Allocate and clear a aligned memory area. Use CDKafree() instead of CDKfree().
 */
void *CDKaalloc (const UINT size, const UINT alignment);
/**
 * Free a aligned memory area.
 */
void  CDKafree  (void *ptr);

#define CDK_MAX_OVERLAYS 8
/**
 * Get memory overlay of a given tag. If the overlay never has been requested,
 *  the memory is allocated.
 */
void *CDKgetOverlay(const UINT size, int tag, MEMORY_SECTION s);
/**
 * Free memory overlay of address. If the overlay internal reference counting is zero,
 *  the memory is free'ed.
 */
void CDKfreeOverlay(void *ptr);
/**
 * Set shared area address of overlay
 */
void CDKsetOverlayArea(void *addr, const UINT size, int tag);
/**
 * Get shared area of overlay
 */
void *CDKgetOverlayArea(const UINT size, int tag);

void *CDKcalloc_L (const UINT n, const UINT size, MEMORY_SECTION s);
void  CDKfree_L   (void *ptr);
void *CDKaalloc_L (const UINT size, const UINT alignment, MEMORY_SECTION s);
void  CDKafree_L  (void *ptr);

void *CDKgetScratchBuf(int size);
void  CDKfreeScratchBuf(int size);

void  CDKmemcpy(void *dst, const void *src, const UINT size);
void  CDKmemmove(void *dst, const void *src, const UINT size);
void  CDKmemset(void *memPtr, const INT value, const UINT size);
void  CDKmemclear(void *memPtr, const UINT size);
UINT  CDKstrlen(const char *s);

/* Compare function wrappers */
INT   CDKmemcmp(const void *s1, const void *s2, const UINT size);
INT   CDKstrcmp(const char *s1, const char *s2);
INT   CDKstrncmp(const char *s1, const char *s2, const UINT size);

#define CDKmax(a,b) ( (a) > (b) ? (a):(b))
#define CDKmin(a,b) ( (a) < (b) ? (a):(b))

#define CDK_INT_MAX ((INT)0x7FFFFFFF)
#define CDK_INT_MIN ((INT)0x80000000)

/* Math function wrappers. Only intended for compatibility, not to be highly optimized. */

INT CDKabs(INT j);
double CDKfabs(double x);
double CDKpow(double x, double y);
double CDKsqrt(double x);
double CDKatan(double x);
double CDKlog(double x);
double CDKsin(double x);
double CDKcos(double x);
double CDKexp(double x);
#define CDKlog2(a) (CDKlog(a)*1.442695041) /* log(2.0) = 1.442695041 */
#define CDKlog10(a) (CDKlog(a)*0.434294482) /* 1.0/log(10.0) = 0.434294482 */
double CDKatan2(double y, double x);
double CDKacos(double x);
double CDKtan(double x);

#if 0
void CDKqsort(void *base, int nmemb, int size,
                  int(*compar)(const void *, const void *));
#endif
double CDKfloor(double x);
double CDKceil(double x);

#define CDK_DOUBLE
INT   CDKatoi(const char *nptr);
long  CDKatol(const char *nptr);
#if defined(CDK_FLOAT) || defined(CDK_DOUBLE)
float CDKatof(const char *nptr);
#endif
/* LONG LONG CDKatoll(const char *nptr); */
/* LONG LONG CDKatoq(const char *nptr); */


/* System calls. */

/* clock wrapper. */

#include <time.h>
#if !defined(CLOCKS_PER_SEC)
  #define CLOCKS_PER_SEC 1000
#endif /* architecture specific clock selector */

#define CDK_CLOCKS_PER_SEC CLOCKS_PER_SEC

INT CDKclock(void);

/* FILE I/O */
#ifndef	AIT_RELEASE_MP3HD_LIB
#ifdef __cplusplus
inline
#else
static
#endif
int IS_LITTLE_ENDIAN(void) {
  int __dummy = 1;
  return ( *( (UCHAR*)(&(__dummy) ) ) );
}

#define TO_LITTLE_ENDIAN(val) \
  ( (IS_LITTLE_ENDIAN()) ? \
     (val) \
   : ( (((val) & 0xff) << 24) || (((val) & 0xff00)<< 8) || (((val) & 0xff0000)>>8) || (((val) & 0xff000000) >> 24) ) )
#else
#define IS_LITTLE_ENDIAN() (0)
#endif

typedef void CDKFILE;

CDKFILE *CDKfopen(const char *filename, const char *mode);
INT CDKfclose(CDKFILE *FP);
INT CDKfseek(CDKFILE *FP, LONG OFFSET, int WHENCE);
INT CDKftell(CDKFILE *FP);
extern INT CDKSEEK_SET, CDKSEEK_CUR, CDKSEEK_END;
INT CDKfwrite(void *ptrf, INT size, UINT nmemb, CDKFILE *fp);
INT CDKfwrite_EL(void *ptrf, INT size, UINT nmemb, CDKFILE *fp);
INT CDKfread(void *dst, INT size, UINT nmemb, CDKFILE *fp);
INT CDKfread_EL(void *dst, INT size, UINT nmemb, CDKFILE *fp);
char* CDKfgets(void *dst, INT size, CDKFILE *fp);
void CDKrewind(CDKFILE *fp);
INT CDKfeof(CDKFILE *fp);

/* Call at startup */
void CDKprolog(void);
/* Call at end */
void CDKepilog(void);

#ifdef __cplusplus
}
#endif

#endif /* __GENERICSTDS_H__ */

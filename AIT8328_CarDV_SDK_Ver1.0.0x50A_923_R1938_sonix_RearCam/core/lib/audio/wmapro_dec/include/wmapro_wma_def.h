#ifndef __WMA_DEF_H__
#define __WMA_DEF_H__

//#include <assert.h>
#define		DISABLE_OPT
#define		NDEBUG
#define 	ENABLE_LPC
#define		BITSTREAM_CORRUPTION_ASSERTS
#define 	ENABLE_ALL_ENCOPT
#define 	BUILD_INTEGER
#define		BUILD_WMASTD
#define		WMAAPI_NO_DRM_STREAM

#if 0//(MSDRM_WMA==1)
//#define		WMAAPI_NO_DRM
#define     WMDRM_PORTABLE
#define     BUILD_WMALIB
#else
//#define		WMAAPI_NO_DRM			
#endif

#define SHRT_MAX  0x7fff
    /* maximum value for an object of type short int */
#define UINT_MAX  0xffffffffU
    /* maximum value for an object of type unsigned int */

//#define 	WMAProDec_MEMSET(dest, c, count)  mem_reset(dest,count)
#if (MSDRM_WMA==1)
#define WMAProDec_MEMSET(dest, c, count)    memset(dest,c,count)
#define WMAProDec_ASSERT(exp)  {if (!(exp)) WMAProDec_assert();}
#define WMAProDec_MEMCPY(dest,src,count)    memcpy(dest,src,count)
#define WMAProDec_MEMMOVE(dest,src,count)	memmove(dest,src,count)
//#define AIT_malloc                                OEM_malloc
#else
#define WMAProDec_MEMSET(dest, c, count)    memset(dest,c,count)
#define WMAProDec_ASSERT(exp)  {if (!(exp)) WMAProDec_assert();}
#define WMAProDec_MEMCPY(dest,src,count)	memcpy(dest,src,count)
#define WMAProDec_MEMMOVE(dest,src,count)	memmove(dest,src,count)
#endif
#define WMAProDec_PRINTF1(str)			    RTNA_DBG_Str(1,str)//printf(str)
#define	WMAProDec_PRINTF2(str,var)          RTNA_DBG_Str(2,str)//printf(str,var)
#define	WMAProDec_PRINTF3(str,var1,var2)    RTNA_DBG_Str(3,str)//printf(str,var1,var2)
#define	WMAPRO_MALLOC(size)				    WMAProDec_wmmalloc(size)//malloc(size)
#define	WMAPRO_FREE(ptr)                    WMAProDec_mfree(ptr)

void *WMAProDec_assert(void);
void *WMAProDec_memmove( void *dest, const void *src, unsigned int count );
void *WMAProDec_wmmalloc(unsigned int size);
void WMAProDec_mfree(void *ptr);

#endif//__WMA_DEF_H__


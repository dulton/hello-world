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
#define		WMAAPI_NO_DRM


#define SHRT_MAX  0x7fff
    /* maximum value for an object of type short int */
#define UINT_MAX  0xffffffffU
    /* maximum value for an object of type unsigned int */

//#define 	WMA_MEMSET(dest, c, count)  wmalib_memset(dest,c,count)
//#define 	WMA_MEMSET(dest, c, count)  mem_reset(dest,count)
#define 	WMA_MEMSET(dest, c, count)  memset(dest,c,count)
//#define   WMA_MEMCPY(dest,src,count)		wmalib_memmove(dest,src,count)
//#define   WMA_MEMMOVE(dest,src,count)		wmalib_memmove(dest,src,count)
#define   WMA_MEMCPY(dest,src,count)		memcpy(dest,src,count)
#define   WMA_MEMMOVE(dest,src,count)		memmove(dest,src,count)
#ifdef	BUILD_WMALIB
#define 	WMA_ASSERT(exp)
// Marked for 20070815 build library test.
//#define   WMA_PRINTF1(str)							{}//ait81x_uart_write(str)//printf(str)
//#define		WMA_PRINTF2(str,var)					{}//ait81x_uart_write(str)//printf(str,var)
//#define		WMA_PRINTF3(str,var1,var2)		{}//ait81x_uart_write(str)//printf(str,var1,var2)
#define   WMA_PRINTF1(str)							//printf(str)
#define		WMA_PRINTF2(str,var)					//printf(str,var)
#define		WMA_PRINTF3(str,var1,var2)					//printf(str,var1,var2)

#else
#define 	WMA_ASSERT(exp)  							//{if (!(exp)) wmalib_assert();}
#define   WMA_PRINTF1(str)							//printf(str)
#define		WMA_PRINTF2(str,var)					//printf(str,var)
#define		WMA_PRINTF3(str,var1,var2)					//printf(str,var1,var2)
#endif
#define		WMA_MALLOC(size)										wmalib_malloc(size)//malloc(size)
#define		WMA_MALLOC_1(size)						wmalib_malloc_1(size)//malloc(size)
#define		WMA_FREE(ptr)														wmalib_free(ptr)

void *wmalib_memset( void *dest, unsigned int c, unsigned int count );
void *wmalib_assert(void);
void *wmalib_memmove( void *dest, const void *src, unsigned int count );
void  mem_reset(void *ptr,unsigned int length);
void *wmalib_malloc(unsigned int size);
void *wmalib_malloc_1(unsigned int size);
void wmalib_free(void *ptr);
#endif//__WMA_DEF_H__


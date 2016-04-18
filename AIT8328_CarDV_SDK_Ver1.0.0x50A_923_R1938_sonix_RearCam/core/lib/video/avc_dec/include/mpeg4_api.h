/**
 *  @file   mpeg4_api.h
 *  @brief  the mpeg4 decoder APIs and structures are defined in this header file, 
 *          the caller should include this file to use this library.
 */

#ifndef _DECORE_H_
#define _DECORE_H_

#ifdef WIN32
    typedef char            MMP_BOOL;
    typedef char            MMP_BYTE;
    typedef short           MMP_SHORT;
    typedef int             MMP_LONG;
    typedef unsigned char   MMP_UBYTE;
    typedef unsigned short  MMP_USHORT;
    typedef unsigned int    MMP_ULONG;
    typedef unsigned long   MMP_U_LONG;
#else
    //#include "includes.h"
    #ifndef MMP_BOOL
    #define MMP_BOLL        char
    #endif
    #ifndef MMP_BYTE
    #define MMP_BYTE        char
    #endif
    #ifndef MMP_SHORT
    #define MMP_SHORT       short
    #endif
    #ifndef MMP_LONG
    #define MMP_LONG        int
    #endif
    #ifndef MMP_UBYTE
    #define MMP_UBYTE       unsigned char
    #endif
    #ifndef MMP_USHORT
    #define MMP_USHORT      unsigned short
    #endif
    #ifndef MMP_ULONG
    #define MMP_ULONG       unsigned int
    #endif
    #ifndef MMP_U_LONG
    #define MMP_U_LONG      unsigned long
    #endif
#endif

/**
 *  @brief      this struct is used to give(or receive) info to(or from) the decoder
 */

typedef struct _DEC_FRAME_
{
	void        *bitstream;	        ///< the bitstream buffer, malloc by the caller
    MMP_ULONG    bitstream_pos;     ///< the cuurent read position in the bitstream buffer, given by the caller
    MMP_ULONG    bitstream_len;     ///< the bistream buffer size, given by the caller
	MMP_LONG     bmp_size;		    ///< the size of output frame

    //! if there is the VOL header exist with the first frame, width and height should be set to 0 by the caller
    //! otherwise, if there is no VOL header, the caller should set the width/height manually, and the decoder will 
    //! "guess" the vol format
    MMP_LONG     width;			    ///< the width of output frame
	MMP_LONG     height;		    ///< the height of output frame
	MMP_ULONG    time_increment_resolution;
	MMP_ULONG    is_H263;

	MMP_LONG     Render_flag;	    ///< 1 : render , 0 : not 
	MMP_UBYTE	*Display_bmp;		///< the display frame buffer, malloc by the caller

    //! if there are not malloc/free available, 
    //! the following 2 parameters are used to indicate the valid memory space

    MMP_ULONG    mem_curr_addr;     ///< the start address of the valid memory space
    MMP_ULONG    mem_end_addr;      ///< the end address of the valid memory space
}	DEC_FRAME;

#ifdef __cplusplus
extern "C" {
#endif

typedef void* MPEG4_DEC_HANDLE;

/**
 * @brief               initial the mpeg4 decoder
 *
 * @param handle    :   the decoder instance
 * @param dec_frame :   this struct is used to give(or receive) info to(or from) the decoder
 */
MMP_LONG	mpeg4_decoeder_init   (MPEG4_DEC_HANDLE *handle, DEC_FRAME *dec_frame);

/**
 * @brief               decode one mpeg4 frame
 *
 * @param handle    :   the decoder instance
 * @param dec_frame :   this struct is used to give(or receive) info to(or from) the decoder
 */
MMP_LONG	mpeg4_decode_frame    (MPEG4_DEC_HANDLE *handle, DEC_FRAME *dec_frame);

/**
 * @brief               initial the avc decoder
 * @param handle    :   the decoder instance
 */
MMP_LONG	mpeg4_decoder_release (MPEG4_DEC_HANDLE *handle);


#ifdef __cplusplus
}
#endif

#endif

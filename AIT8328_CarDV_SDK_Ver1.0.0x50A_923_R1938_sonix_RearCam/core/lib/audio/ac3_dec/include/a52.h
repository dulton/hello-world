/*
 * a52.h
 * Copyright (C) 2000-2003 Michel Lespinasse <walken@zoy.org>
 * Copyright (C) 1999-2000 Aaron Holtzman <aholtzma@ess.engr.uvic.ca>
 *
 * This file is part of a52dec, a free ATSC A-52 stream decoder.
 * See http://liba52.sourceforge.net/ for updates.
 *
 * a52dec is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * a52dec is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
	"liba52"  is completly a freeware for non-commerical use.
	For Alpha Imaging Technology Corp. (AIT) indoor research purpose only.
*/

#ifndef __A52_H__
#define __A52_H__

#define A52_MAX_RESYNC_CNT  (100)   //test

#define FIXEDP_IMPLEMENT_A52DEC

typedef enum
{
    A52DEC_DECFRM_NORMAL_DECODE = 0,
    A52DEC_DECFRM_SEARCH_SYNC   = 1
    
}A52Dec_DecFrmMode;


typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
#ifdef WIN32 //ARCH_X86
typedef signed __int64 int64_t;
#else
typedef signed long long int64_t;
#endif

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#ifdef WIN32 //ARCH_X86
typedef unsigned __int64 uint64_t;
#else
typedef unsigned long long uint64_t;
#endif


#if defined(FIXEDP_IMPLEMENT_A52DEC)
typedef int32_t sample_t;
typedef int32_t level_t;
#elif defined(LIBA52_DOUBLE)
typedef double sample_t;
typedef double level_t;
#else
typedef float sample_t;
typedef float level_t;
#endif

typedef struct a52_state_s a52_state_t;

typedef enum
{
    A52_CHANNEL       = 0, 
    A52_MONO          = 1,
    A52_STEREO        = 2,
    A52_3F            = 3,
    A52_2F1R          = 4,
    A52_3F1R          = 5,
    A52_2F2R          = 6,
    A52_3F2R          = 7,
    A52_CHANNEL1      = 8,
    A52_CHANNEL2      = 9,
    A52_DOLBY         = 10,
    A52_CHANNEL_MASK  = 15,
        
    A52_LFE           = 16,
    A52_ADJUST_LEVEL  = 32
    
}A52Dec_DownMixMode;


/***************************************************************************************

   Purpose:
    (1) Init. A52 decoder.
    (2) Set working buffer.

    argu. 1: working buffer addr. Shall be a memory space aligned by 4 (bytes).
    argu. 2: working buffer size in byte.

    Returns decoder's internal woking varibles ptr if success,
    otherwise returns NULL.
    
 ***************************************************************************************/
a52_state_t * A52DecS_Init (uint32_t *working_buffer, uint32_t working_buffer_size);



/***************************************************************************************

    Purpose: 
    Get A52 decoder internal output buffer address

    argu. 1(i): decoder's internal woking varibles ptr. (from A52DecS_Init() ).

    Returns output buffer address.
    
 ***************************************************************************************/
sample_t * A52DecS_GetOutBufAddr (a52_state_t * state);



/***************************************************************************************
    Purpose: 
    (1) Check Sync Word & parse simple audio info.

    argu. 1(i): input buffer address
    argu. 2(o): audio flags (Ch config, etc.)
    argu. 3(o): sample rate (Hz)
    argu. 4(o): bit rate (bps)

    Returns 0 if success,
    otherwise return < 0
    
 ***************************************************************************************/
int A52DecS_GetSyncInfo (uint8_t * buf, int * flags, int * sample_rate, int * bit_rate);



/*************************************************************************************** 

    Purpose: 
    (1) Parse frame header

    argu. 1(i/o): decoder's internal woking varibles ptr. (from A52DecS_Init() ).
    argu. 2(i): input buffer address
    argu. 3(i/o): audio flags
    argu. 4(i/o): output level
    argu. 5(i): output bias

    Returns 0 if success,
    otherwise return < 0
    
 ***************************************************************************************/
int A52DecS_ParseFrame (a52_state_t * state, uint8_t * buf, int * flags, level_t * level, sample_t bias);



/*************************************************************************************** 

    Purpose: 
    (1) Decode audio data & synthesis a frame

    argu. 1(i/o): decoder's internal woking varibles ptr. (from A52DecS_Init() ).

    Returns 0 if success,
    otherwise return < 0

 ***************************************************************************************/
int A52DecS_Block (a52_state_t * state);



#endif /* __A52_H__ */


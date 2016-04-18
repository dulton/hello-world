/*
   Bit Buffer Management
 */

#ifndef FFR_BITBUFFER_H
#define FFR_BITBUFFER_H

#include "aacplus_intrinsics.h"


enum ffr_direction
{
  forwardDirection,
  backwardDirection
};


/*!
   The pointer 'pReadNext' points to the next available word, where bits can be read from. The pointer
   'pWriteNext' points to the next available word, where bits can be written to. The pointer pBitBufBase
   points to the start of the bitstream buffer and the pointer pBitBufEnd points to the end of the bitstream
   buffer. The two pointers are used as lower-bound respectively upper-bound address for the modulo addressing
   mode.

   The element cntBits contains the currently available bits in the bit buffer. It will be incremented when
   bits are written to the bitstream buffer and decremented when bits are read from the bitstream buffer.
*/
struct BIT_BUF
{
  UWord8 *pBitBufBase;          /*!< pointer points to first position in bitstream buffer */
  UWord8 *pBitBufEnd;           /*!< pointer points to last position in bitstream buffer */

  UWord8 *pReadNext;            /*!< pointer points to next available word in bitstream buffer to read */
  UWord8 *pWriteNext;           /*!< pointer points to next available word in bitstream buffer to write */

  Word32  rBitPos;              /*!< 7<=rBitPos<=0*/
  Word32  wBitPos;              /*!< 7<=wBitPos<=0*/
  Word32  cntBits;              /*!< number of available bits in the bitstream buffer
                                     write bits to bitstream buffer  => increment cntBits
                                     read bits from bitstream buffer => decrement cntBits */
  Word32  size;                 /*!< size of bitbuffer in bits */
}; /* size Word16: 8 */

/*! Define pointer to bit buffer structure */
typedef struct BIT_BUF *HANDLE_BIT_BUF;


/*---------------------------------------------------------------------------

functionname:AACPLUS_CreateBitBuffer
description: initialize bit buffer
returns:     bit buffer handler

---------------------------------------------------------------------------*/
HANDLE_BIT_BUF AACPLUS_CreateBitBuffer(HANDLE_BIT_BUF hBitBuf,
                               UWord8 *pBitBufBase,
                               Word16  bitBufSize);

/*---------------------------------------------------------------------------

functionname:AACPLUS_interleaveSamples
description: Interleave output samples. 
             In case of mono input, copy left channel to right channel.

---------------------------------------------------------------------------*/
void AACPLUS_interleaveSamples(Word16 *pTimeCh0, 
                       Word16 *pTimeCh1, 
                       Word16 *pTimeOut, 
                       Word32  frameSize, 
                       Word16 *channels);


#endif /* FFR_BITBUFFER_H */

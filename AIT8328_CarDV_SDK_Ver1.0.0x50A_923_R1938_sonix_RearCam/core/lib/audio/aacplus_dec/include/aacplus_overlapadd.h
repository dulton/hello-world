/*
   overlap & add
 */

#ifndef OVERLAPADD_H
#define OVERLAPADD_H

#include "aacplus_intrinsics.h"
#include "mmpf_audio_typedef.h"

enum
{
  OverlapBufferSize = 512
};

typedef struct
{
  Word16 WindowShape;
  Word16 WindowSequence;
  Word16 *pOverlapBuffer;
} COverlapAddData;

#endif /* #ifndef OVERLAPADD_H */

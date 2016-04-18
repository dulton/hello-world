//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    MsAudio.h

Abstract:

    Implementation of public member functions for CAudioObject.

Author:

    Wei-ge Chen (wchen) 11-March-1998

Revision History:


*************************************************************************/

#ifndef __AUMALLOC_H_
#define __AUMALLOC_H_
#include "wmapro_wma_def.h"
// ***********************************************
//
//#ifdef __cplusplus
//extern "C" {  // only need to export C interface if
//              // used by C++ source code
//#endif

//#include <stdio.h>
//#include <stdlib.h>
#include "wmapro_macros.h" // For Int etc. Should be replaced with smaller file

	
// ----- Memory Allocation Functions -----
typedef enum {MAS_ALLOCATE, MAS_LOCKED, MAS_DELETE} MEMALLOCSTATE;

I32 auMallocGetCount(void);
void WMAProDec_auMallocSetState(const MEMALLOCSTATE maState, void *pBuf, const U32 iBufSize);


#ifndef WANT_HEAP_MEASURE

//mallocAligned allocates a buffer of size (+iAlignToBytes for allignment padding) bytes 
//which is alligned on an iAllignToBytes byte boundary
//the buffer must be deallocated using freeAligned
//and it is only safe to use size bytes as the padding could be at the begining or ending
void *mallocAligned(size_t size,Int iAlignToBytes);
void freeAligned(void *ptr);

void *auMalloc(const size_t iSize);
void auFree(void *pFree);

#define     START_MEASURE_MEM(log_file)
#define     WRITE_MEM_LOG()            
#define     GET_MEM_STATS(PeakHeapSize, MemLeakSize)

#else

#endif //WANT_HEAP_MEASURE




//#ifdef __cplusplus
//}
//#endif

#endif //__AUMALLOC_H_


//==============================================================================
//
//  File        : mmpf_nr.h
//  Description : INCLUDE File for the Noise Reduction Algorithm
//  Author      : Alterman
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPF_NR_H_
#define	_MMPF_NR_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================
#include "includes_fw.h"
#include "aitu_ringbuf.h"
#include "AudioProcess.h"

#if (NR_EN)
/** @addtogroup MMPF_NR
 *  @{
 */

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

#define NR_WORKBUF_SIZE         (62512)
#define NR_FRAME_SIZE           (256)   ///< NR frame size per channel
//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
extern MMP_ERR MMPF_NR_Initialize(MMP_ULONG workbuf, MMP_UBYTE ch, MMP_ULONG samplerate);
extern void     MMPF_NR_Operate(AUTL_RINGBUF *rbuf, MMP_ULONG sample);
/// @}
#else
#define NR_WORKBUF_SIZE         (0)
#endif // (NR_EN)
#endif // _MMPF_NR_H_
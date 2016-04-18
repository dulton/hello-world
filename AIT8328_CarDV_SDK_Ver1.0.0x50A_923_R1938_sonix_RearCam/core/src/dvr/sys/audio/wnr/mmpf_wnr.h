//==============================================================================
//
//  File        : mmpf_mnr.h
//  Description : INCLUDE File for the Wind Noise Reduction Algorithm
//  Author      : Alterman
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPF_WNR_H_
#define	_MMPF_WNR_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================
#include "includes_fw.h"
#include "aitu_ringbuf.h"
#include "AudioWnrProcess.h"

#if (WNR_EN)
/** @addtogroup MMPF_WNR
 *  @{
 */

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

#define WNR_WORKBUF_SIZE        (4140)
#define WNR_FRAME_SIZE          (256)   ///< WNR frame size per channel

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

/* Wind noise reduction handler */
typedef struct _MMPF_WNR_HANDLE {
    MMP_BOOL    bEnWNR;         ///< WNR global enable/disable control
    MMP_BOOL    bEnNF;          ///< Noise filter enable
    MMP_BOOL    bEnEQ;          ///< Noise reduction enable
    AudioWnrProcessStruct cfg;  ///< WNR config parameters
} MMP_WNR_HANDLE;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

MMP_WNR_HANDLE *MMPF_WNR_GetHandle(void);
void     MMPF_WNR_SetActivity(MMP_BOOL enable);
MMP_BOOL MMPF_WNR_IsActivating(void);
MMP_ERR  MMPF_WNR_Initialize(MMP_ULONG workbuf, MMP_UBYTE ch, MMP_ULONG srate);
void     MMPF_WNR_Operate(AUTL_RINGBUF *rbuf, MMP_ULONG sample);

/// @}
#else

#define WNR_WORKBUF_SIZE        (0)

#endif // (WNR_EN)
#endif // _MMPF_WNR_H_
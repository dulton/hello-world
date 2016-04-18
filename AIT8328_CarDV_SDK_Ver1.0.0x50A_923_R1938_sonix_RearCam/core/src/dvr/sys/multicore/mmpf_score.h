//==============================================================================
//
//  File        : mmpf_score.h
//  Description : INCLUDE File for Slave Core to Master Core function
//  Author      : Alterman
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPF_SCORE_H_
#define _MMPF_SCORE_H_

#include "includes_fw.h"
#include "cpucomm.h"
#include "cpucomm_bus.h"
#include "mmp_multicore_inc.h"

/** @addtogroup MMPF_MULITCORE
@{
*/

//==============================================================================
//
//                              MACRO
//
//==============================================================================

#define SCORE_ERR(str)          __UartWrite(str)
#define SCORE_WARN(str)         __UartWrite(str)
#define SCORE_DBG(str)
#define SCORE_LOG(str)

//==============================================================================
//
//                              TASK CONFIGURES
//
//==============================================================================
/*
 * Task Priority (Note: Priority 1 has been used)
 */
#define SCORE_TASK_MDTC_PRIO        (6)
#define SCORE_TASK_ADAS_PRIO        (7)

/*
 * Task Stack Size
 */
#define SCORE_TASK_MDTC_STK_SIZE    (768)
#define SCORE_TASK_ADAS_STK_SIZE    (768)

//==============================================================================
//
//                              DATA TYPES
//
//==============================================================================

typedef void (SCORE_ReqHandler)(DUALCORE_REQ *req, DUALCORE_RESP *resp);

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

extern void __UartWrite(const MMP_BYTE *str);

/// @}

#endif // _MMPF_SCORE_H_

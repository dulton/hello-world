//==============================================================================
//
//  File        : mmpf_mcore.h
//  Description : INCLUDE File for Master Core to Slave Core function
//  Author      : Alterman
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPF_MCORE_H_
#define _MMPF_MCORE_H_

#include "mmp_multicore_inc.h"

/** @addtogroup MMPF_MULITCORE
@{
*/

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

MMP_ERR MMPF_MCORE_Initial(void);
MMP_ERR MMPF_MCORE_IssueRequest(DUALCORE_REQ *req, DUALCORE_RESP *resp);

/// @}

#endif // _MMPF_MCORE_H_

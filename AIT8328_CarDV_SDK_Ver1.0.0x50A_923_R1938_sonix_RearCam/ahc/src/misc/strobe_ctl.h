//==============================================================================
//
//  File        : strobe_ctl.h
//  Description : 
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================
#ifndef _strobe_ctl_H_
#define _strobe_ctl_H_

#include "includes_fw.h"
#include "mmpf_typedef.h"
#include "mmpf_timer.h"
//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
MMP_ERR STROBE_Initialize(void);
MMP_ERR STROBE_EanbleCharge(MMP_BOOL bEnable);
MMP_ERR STROBE_CheckChargeReady(MMP_BOOL *bEnable);
MMP_ERR STROBE_TriggerDuration(	MMP_ULONG ulBlankDuration, MMP_ULONG ulTriggerDuration);
void STROBE_Trigger(void);
MMP_ERR STROBE_GetTimerStatus(MMP_BOOL* bTimerStatus, MMP_ULONG* ulHardwareTime, TimerCallBackFunc* FuncPtr);
MMP_ERR STROBE_ForceTimerClosed(MMP_BOOL bForce);
#endif
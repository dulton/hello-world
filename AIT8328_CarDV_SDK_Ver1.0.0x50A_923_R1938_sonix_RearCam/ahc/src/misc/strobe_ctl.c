//==============================================================================
//
//  File        : strobe_ctl.c
//  Description : 
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================
#if 0		 
void _____HEADER_____(){}
#endif

#include "strobe_ctl.h"
#include "strobe_ncp5080.h"
#include "config_fw.h"
#include "mmpf_pio.h"
#include "mmpf_pwm.h"
//==============================================================================
//
//                              VARIABLES
//
//==============================================================================

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

MMP_ERR STROBE_Initialize(void)
{
	STROBE_Initialize_5080(STORBE_CHARGE_ON_PIN, STORBE_CHARGE_READY_PIN, STORBE_TRIGGER_PIN, STORBE_TIMER_ID);
	return MMP_ERR_NONE;
}

MMP_ERR STROBE_EanbleCharge(MMP_BOOL bEnable)
{
	return STROBE_EanbleCharge_5080(bEnable);
}

MMP_ERR STROBE_CheckChargeReady(MMP_BOOL *bEnable)
{

	return STROBE_CheckChargeReady_5080(bEnable);
}

MMP_ERR STROBE_TriggerDuration(	MMP_ULONG ulBlankDuration, MMP_ULONG ulTriggerDuration)
{
	return STROBE_TriggerInitialization_5080(ulBlankDuration, ulTriggerDuration);
}

void STROBE_Trigger(void)
{
	STROBE_Trigger_5080();
}

MMP_ERR STROBE_GetTimerStatus(MMP_BOOL* bTimerStatus, MMP_ULONG* ulHardwareTime, TimerCallBackFunc* FuncPtr)
{
	STROBE_GetTimerStatus_5080(bTimerStatus, ulHardwareTime, FuncPtr);
	return MMP_ERR_NONE;

}

MMP_ERR STROBE_ForceTimerClosed(MMP_BOOL bForce)
{
	STROBE_ForceTimerClosed_5080(bForce);
	return MMP_ERR_NONE;

}
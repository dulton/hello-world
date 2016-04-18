//==============================================================================
//
//  File        : mmps_timer.c
//  Description : Timer Functions.
//  Author      : Ben Lu
//  Revision    : 1.0
//
//==============================================================================
/**
 *  @file mmps_timer.c
 *  @brief The timer control functions
 *  @author Ben Lu
 *  @version 1.0
 */
#include "mmp_lib.h"
#include "mmps_timer.h"
#include "ait_utility.h"
#include "mmpd_timer.h"

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

//------------------------------------------------------------------------------
//  Function    : MMPS_Timer_SetTimer
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set syterm Timer

 Set syterm Timer
 Parameters:
 @param[in] bTimerID : Timer ID
 @param[in] bTimerCMD : Timer command
 @param[in] usTime : the time set to the timer
 @return the status of the operation.
*/

/*MMP_ERR MMPS_Timer_SetTimer(MMP_ULONG* bTimerID, MMPS_TIMER_CMD bTimerCMD, MMP_USHORT usTime, void* pTimerIsrHandler, void* pMessage)
{
	return MMPD_Timer_SetTimer(bTimerID, (MMPD_TIMER_CMD)bTimerCMD, usTime, pTimerIsrHandler, pMessage);
}*/

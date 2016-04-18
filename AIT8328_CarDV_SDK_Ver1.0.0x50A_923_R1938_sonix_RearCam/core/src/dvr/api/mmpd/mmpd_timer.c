//==============================================================================
//
//  File        : mmpd_timer.c
//  Description : Timer Functions.
//  Author      : Ben Lu
//  Revision    : 1.0
//
//==============================================================================
/**
 *  @file mmpd_timer.c
 *  @brief The timer control functions
 *  @author Ben Lu
 *  @version 1.0
 */
#include "mmp_lib.h"
#include "mmpd_timer.h"
#include "ait_utility.h"
#if defined(SA_PL)
#include "mmpf_timer.h"
#include "os_wrap.h"
#endif
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
//  Function    : MMPD_Timer_SetTimer
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set syterm Timer

 Set syterm Timer.
 Parameters:
 @param[in] bTimerID : Timer ID
 @param[in] bTimerCMD : Timer command
 @param[in] usTime : the time set to the timer
 @return the status of the operation.
*/

/*MMP_ERR MMPD_Timer_SetTimer(MMP_ULONG* bTimerID, MMPD_TIMER_CMD bTimerCMD, MMP_USHORT usTime, void* pTimerIsrHandler, void* pMessage)
{ 
	#if defined(SA_PL)
	*bTimerID = MMPF_OS_StartTimer(usTime , MMPF_OS_TMR_OPT_PERIODIC, (MMPF_OS_TMR_CALLBACK)&pTimerIsrHandler, pMessage); 
	if ((*bTimerID == 0xFF)||(*bTimerID == 0xFE)) {
		return MMP_SYSTEM_ERR_TIMER;
	}
	#endif
	#if defined(HT_PL)
	PRINTF("MMPD_Timer_SetTimer() is not implemented in host platform.\r\n");
	#endif
	return MMP_ERR_NONE;
}*/

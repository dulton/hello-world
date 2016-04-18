//==============================================================================
//
//  File        : mmpd_timer.h
//  Description : INCLUDE File for the system timer
//  Author      : Ben Lu
//  Revision    : 1.0
//
//==============================================================================
/**
 *  @file mmpd_timer.h
 *  @brief Header File for the system timer.
 *  @author Ben Lu
 *  @version 1.0
 */

#ifndef _MMPD_TIMER_H_
#define _MMPD_TIMER_H_

//==============================================================================
//
//                              COMPILER OPTION
//
//==============================================================================


//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================
typedef enum _MMPD_TIMER_ID
{
	MMPD_TIMER_0 =0,
	MMPD_TIMER_1,
	MMPD_TIMER_2,
	MMPD_TIMER_3,
	MMPD_TIMER_4,
	MMPD_TIMER_5,
	MMPD_TIMER_MAX
} MMPD_TIMER_ID;

typedef enum _MMPD_TIMER_CMD {
	MMPD_TIMER_CMD_START = 0,
	MMPD_TIMER_CMD_START_ONESHOT,
	MMPD_TIMER_CMD_MAX	
} MMPD_TIMER_CMD;
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
//MMP_ERR MMPD_Timer_SetTimer(MMP_ULONG* bTimerID, MMPD_TIMER_CMD bTimerCMD, MMP_USHORT usTime, void* pTimerIsrHandler, void* pMessage);
#endif // _MMPD_TIMER_H_
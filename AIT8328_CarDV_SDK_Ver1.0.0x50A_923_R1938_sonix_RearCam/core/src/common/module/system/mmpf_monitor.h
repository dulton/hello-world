/// @ait_only
//==============================================================================
//
//  File        : mmpf_monitor.h
//  Description : INCLUDE File for the MCI Monitor Driver.
//  Author      : Alterman
//  Revision    : 1.0
//
//==============================================================================

/**
 *  @file mmpf_monitor.h
 *  @brief The header File for the MCI Monitor Driver
 *  @author Alterman
 *  @version 1.0
 */

#ifndef _MMPF_MONNITOR_H_
#define _MMPF_MONNITOR_H_

//==============================================================================
//
//                              COMPILER OPTIONS
//
//==============================================================================

#if defined(ALL_FW)
#define MEM_MONITOR_EN          (1)
#else
#define MEM_MONITOR_EN          (0)
#endif

#if (MEM_MONITOR_EN)

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

#define INVALID_MONITOR_SEQ     (0xFE)
#define NO_FREE_MONITOR_SEQ     (0xFF)

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef struct _MMPF_MONITOR_SEQ_ATTR {
    MMP_ULONG   ulAddrLowBD;  // Low bound address of segment
    MMP_ULONG   ulAddrUpBD;   // Up bound address of segment
    MMP_ULONG   ulWrAllowSrc; // Bitmap of source allowed to write this segment
    MMP_ULONG   ulRdAllowSrc; // Bitmap of source allowed to read this segment
} MMPF_MONITOR_SEQ_ATTR;

typedef struct _MMPF_MONITOR_CB_ARGU {
    MMP_ULONG   ulFaultSrc;   // Bitmap of source causes the fault access
    MMP_ULONG   ulFaultAddr;  // Address of fault access
    MMP_UBYTE   ubFaultSize;  // Byte count of fault access
    MMP_UBYTE   ubSegID;      // ID of segment traps the fault access
    MMP_BOOL    bFaultWr;     // Fault access due to write
    MMP_BOOL    bFaultRd;     // Fault access due to read
} MMPF_MONITOR_CB_ARGU;

//==============================================================================
//
//                              DATA TYPES
//
//==============================================================================

typedef void (*MONITOR_SEQ_FAULT_CB)(MMPF_MONITOR_CB_ARGU *argu);

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

void MMPF_Monitor_ISR(void);
void MMPF_Monitor_Init(void);
MMP_UBYTE MMPF_Monitor_Enable(MMPF_MONITOR_SEQ_ATTR *pAttr,
                              MONITOR_SEQ_FAULT_CB cb);
void MMPF_Monitor_Disable(MMP_UBYTE ubSegID);

#endif //(MEM_MONITOR_EN)

#endif //_MMPF_MONNITOR_H_
//==============================================================================
//
//  File        : os_wrap.h
//  Description : OS wrapper function for uC/OS-II
//  Author      : Jerry Tsao
//  Revision    : 1.0
//
//==============================================================================

#ifndef _OS_WRAP_H_
#define _OS_WRAP_H_

#include "includes_fw.h"

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

#define MMPF_OS_SEMID_MAX   		OS_MAX_EVENTS
#define MMPF_OS_FLAGID_MAX  		OS_MAX_FLAGS
#define MMPF_OS_MUTEXID_MAX  		4
#define MMPF_OS_MQID_MAX   			OS_MAX_QS
#define MMPF_OS_TMRID_MAX   		OS_TMR_CFG_MAX

#define MMPF_OS_CREATE_SEM_INT_ERR		(0xFF)
#define MMPF_OS_CREATE_SEM_EXCEED_MAX	(0xFE)

#define MMPF_OS_CREATE_FLAG_INT_ERR		(0xFF)
#define MMPF_OS_CREATE_FLAG_EXCEED_MAX	(0xFE)

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef MMP_UBYTE   MMPF_OS_TASKID;
typedef MMP_ULONG   MMPF_OS_SEMID;
typedef MMP_ULONG   MMPF_OS_FLAGID;
typedef MMP_ULONG   MMPF_OS_MUTEXID;
typedef MMP_ULONG   MMPF_OS_MQID;
typedef MMP_ULONG   MMPF_OS_TMRID;

#if OS_FLAGS_NBITS == 16
typedef MMP_USHORT   MMPF_OS_FLAGS;
#endif
#if OS_FLAGS_NBITS == 32
typedef MMP_ULONG    MMPF_OS_FLAGS;
#endif

typedef MMP_USHORT  MMPF_OS_FLAG_WTYPE;
    #define MMPF_OS_FLAG_WAIT_CLR_ALL       OS_FLAG_WAIT_CLR_ALL
    #define MMPF_OS_FLAG_WAIT_CLR_ANY       OS_FLAG_WAIT_CLR_ANY
    #define MMPF_OS_FLAG_WAIT_SET_ALL       OS_FLAG_WAIT_SET_ALL
    #define MMPF_OS_FLAG_WAIT_SET_ANY       OS_FLAG_WAIT_SET_ANY
    #define MMPF_OS_FLAG_CONSUME            OS_FLAG_CONSUME
typedef MMP_USHORT  MMPF_OS_FLAG_OPT;
    #define MMPF_OS_FLAG_CLR                OS_FLAG_CLR
    #define MMPF_OS_FLAG_SET                OS_FLAG_SET
typedef MMP_UBYTE   MMPF_OS_TMR_OPT;
    #define MMPF_OS_TMR_OPT_ONE_SHOT        OS_TMR_OPT_ONE_SHOT
    #define MMPF_OS_TMR_OPT_PERIODIC        OS_TMR_OPT_PERIODIC
    #define MMPF_OS_TMR_OPT_NONE			OS_TMR_OPT_NONE
    #define MMPF_OS_TMR_OPT_CALLBACK		OS_TMR_OPT_CALLBACK

typedef OS_TMR_CALLBACK	MMPF_OS_TMR_CALLBACK;

typedef struct {
    MMP_ULONG   pbos;           // lower address
    MMP_ULONG   ptos;           // higer address
	MMP_UBYTE	ubPriority;
} MMPF_TASK_CFG;

typedef struct {
    OS_TMR               *tmr;
    OS_TMR_CALLBACK     OSTmrCallback;      /* Callback when timer expires */
    void                *OSTmrCallbackArg;  /* Callback arguments          */
    MMP_BOOL            bStarted;           /* Is timer started            */
} MMPF_OS_TMR;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

void                MMPF_OS_Initialize(void);
void                MMPF_OS_StartTask(void);

// Task Related
MMPF_OS_TASKID      MMPF_OS_CreateTask(void (*taskproc)(void *param), MMPF_TASK_CFG *task_cfg, void *param);
MMP_UBYTE           MMPF_OS_SetTaskName(MMP_UBYTE ubNewPriority, MMP_UBYTE *pTaskName);
MMPF_OS_TASKID  	MMPF_OS_ChangeTaskPriority(MMPF_OS_TASKID taskid, MMP_UBYTE ubNewPriority);
MMP_UBYTE  			MMPF_OS_DeleteTask(MMPF_OS_TASKID taskid);
MMP_UBYTE           MMPF_OS_SuspendTask(MMPF_OS_TASKID taskid);
MMP_UBYTE           MMPF_OS_ResumeTask(MMPF_OS_TASKID taskid);
MMP_UBYTE           MMPF_OS_LockSchedule(void);
MMP_UBYTE           MMPF_OS_UnlockSchedule(void);

// Semaphore Related
MMPF_OS_SEMID       MMPF_OS_CreateSem(MMP_UBYTE ubSemValue);
MMP_UBYTE	        MMPF_OS_DeleteSem(MMPF_OS_SEMID ulSemId);
MMP_UBYTE 			MMPF_OS_ReleaseSem(MMPF_OS_SEMID ulSemId);
MMP_UBYTE      		MMPF_OS_AcquireSem(MMPF_OS_SEMID ulSemId, MMP_ULONG ulTimeout);
MMP_UBYTE           MMPF_OS_AcceptSem(MMPF_OS_SEMID ulSemId, MMP_USHORT *usCount);
MMP_UBYTE 			MMPF_OS_QuerySem(MMPF_OS_SEMID ulSemId, MMP_USHORT *usCount);
MMP_UBYTE			MMPF_OS_QuerySemEx(MMPF_OS_SEMID ulSemId, MMP_USHORT *usCount, MMP_USHORT *usWait);
MMP_UBYTE           MMPF_OS_SetSem(MMPF_OS_SEMID semID,MMP_USHORT count);

// Task Related
MMPF_OS_FLAGID 		MMPF_OS_CreateEventFlagGrp(MMP_ULONG ulFlagValues);
MMP_UBYTE  			MMPF_OS_DeleteEventFlagGrp(MMPF_OS_FLAGID ulFlagId);
MMP_UBYTE 			MMPF_OS_SetFlags(MMPF_OS_FLAGID ulFlagID, MMPF_OS_FLAGS flags, MMPF_OS_FLAG_OPT opt);
MMP_UBYTE 			MMPF_OS_WaitFlags(MMPF_OS_FLAGID ulFlagID, MMPF_OS_FLAGS flags, MMPF_OS_FLAG_WTYPE waitType, 
							MMP_ULONG ulTimeout, MMPF_OS_FLAGS *ret_flags);
MMP_UBYTE 			MMPF_OS_QueryFlags(MMPF_OS_FLAGID ulFlagID, MMPF_OS_FLAGS *ret_flags);

// Mutex Related
MMPF_OS_MUTEXID 	MMPF_OS_CreateMutex(MMP_UBYTE	ubPriority);
MMP_UBYTE  			MMPF_OS_DeleteMutex(MMPF_OS_MUTEXID ulMutexId);
MMP_UBYTE 			MMPF_OS_AcquireMutex(MMPF_OS_MUTEXID ulMutexId, MMP_ULONG ulTimeout);
MMP_UBYTE 			MMPF_OS_ReleaseMutex(MMPF_OS_MUTEXID ulMutexId);

// Message Queue Related
MMPF_OS_MQID 		MMPF_OS_CreateMQueue(void **msg, MMP_USHORT ubQueueSize);
MMP_UBYTE  			MMPF_OS_DeleteMQueue(MMPF_OS_MQID ulMQId);
MMP_UBYTE 			MMPF_OS_GetMessage(MMPF_OS_MQID ulMQId,void **msg, MMP_ULONG ulTimeout);
MMP_UBYTE 			MMPF_OS_PutMessage(MMPF_OS_MQID ulMQId, void *msg);
MMP_UBYTE           MMPF_OS_FlushMQueue(MMPF_OS_MQID ulMQId);
MMP_UBYTE           MMPF_OS_QueryMQueue(MMPF_OS_MQID ulMQId, MMP_USHORT *usCount);

// Time Related
MMP_UBYTE 			MMPF_OS_Sleep(MMP_USHORT usTickCount);
MMP_UBYTE           MMPF_OS_SleepMs(MMP_ULONG ulMiliSecond);
MMP_UBYTE 			MMPF_OS_SetTime(MMP_ULONG ulTickCount);
MMP_UBYTE 			MMPF_OS_GetTime(MMP_ULONG *ulTickCount);
MMP_UBYTE 			MMPF_OS_GetTimeMs(MMP_ULONG *ulTickCount);

// Timer Related
MMPF_OS_TMRID 		MMPF_OS_StartTimer(MMP_ULONG ulPeriod, MMPF_OS_TMR_OPT opt, MMPF_OS_TMR_CALLBACK callback,
										void *callback_arg);
MMP_UBYTE 			MMPF_OS_StopTimer(MMPF_OS_TMRID ulTmrId, MMPF_OS_TMR_OPT opt);

// Memory Heap Related
void*				MMPF_OS_Malloc(int len);
void* 				MMPF_OS_Calloc(int num, int size);
void    			MMPF_OS_MemFree(char *mem_ptr);

// CPU State Releated
MMP_BOOL            MMPF_OS_InInterrupt(void);

//------------------------------
// User Specified Configuration
//------------------------------

extern MMPF_TASK_CFG        task_cfg[];

#endif // _OS_WRAP_H_
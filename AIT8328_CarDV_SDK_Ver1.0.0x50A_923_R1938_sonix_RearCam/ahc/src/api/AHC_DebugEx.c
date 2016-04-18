//==============================================================================
//
//  File        : AHC_DebugEx.c
//  Description : AHC Debug function
//  Author      : Caesar Chang
//  Revision    : 1.0
//
//==============================================================================

/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "ait_utility.h"
#include "mmps_system.h"
#include "mmpf_system.h"
#include "ucos_ii.h"
#include "os_wrap.h"
#include "AHC_General.h"
#include "AHC_DebugEx.h"
#include "AHC_UF.h"
#include "PMUCtrl.h"
#include "mmpf_monitor.h"
#include "mmps_dma.h"
#include "mmu.h"
#include "mmp_reg_mci.h"

/*******************************************************************************
 *
 *   STRUCTURE
 *
*******************************************************************************/
//for Mem Monitor use
typedef enum{
    MEM_MONITOR_ITCM_REGION = 0,
    MEM_MONITOR_ALL_DRAM_RO_REGION,
    MEM_MONITOR_GUI_BUFFER_TOP,
    MEM_MONITOR_DCF_BUFFER_TOP,
    MEM_MONITOR_FS_BUFFER_TOP,
    MEM_MONITOR_END
}MEM_MONITOR_ID;

static MMP_UBYTE ubMemMonitorInitFlag = AHC_FALSE;
static MMP_ULONG ulSegIdx = 0;  //record monitor enable seg by bit offset

#undef MEMORY_MONITOR_MAX_NUM
#define MEMORY_MONITOR_MAX_NUM            8

/*===========================================================================
 * Extern function
 *===========================================================================*/ 

extern void MMPF_TimingStop(void);
extern void MMPF_TimingDump(int);
extern void printc( char*, ...);

/*===========================================================================
 * Main body
 *===========================================================================*/
static void AIHC_MemMonitor_Callback(MMPF_MONITOR_CB_ARGU *argu)
{
    RTNA_DBG_Str(0, FG_RED("\r\n### AIHC_MemMonitor_Callback -\r\n"));
    RTNA_DBG_Str(0, "Fault: Seg");
    RTNA_DBG_Byte(0, argu->ubSegID);
    if (argu->bFaultWr)
        RTNA_DBG_Str(0, " write");
    if (argu->bFaultRd)
        RTNA_DBG_Str(0, " read");
    RTNA_DBG_Long(0, argu->ulFaultSrc);
    RTNA_DBG_Long(0, argu->ulFaultAddr);
    RTNA_DBG_Byte(0, argu->ubFaultSize);
    RTNA_DBG_Str(0, "\r\n");
}

static void AIHC_MemoryMonitorSetPara(MMPF_MONITOR_SEQ_ATTR *attr)
{
    //MMP_ULONG 	FSbuf_start = 0;

    extern unsigned int Image$$ALL_DRAM_RO$$Base;
    extern unsigned int Image$$ALL_DRAM_RO$$Limit;

#if 0
    //ITCM Region 0
    (attr + MEM_MONITOR_ITCM_REGION)->ulAddrLowBD = 0x00000000;
    (attr + MEM_MONITOR_ITCM_REGION)->ulAddrUpBD  = 0x00001000;
    (attr + MEM_MONITOR_ITCM_REGION)->ulWrAllowSrc = 0;
    (attr + MEM_MONITOR_ITCM_REGION)->ulRdAllowSrc = MCI_SRC_MASK;
#endif

    //ALL_DRAM RO Load Region 1 
    (attr + MEM_MONITOR_ALL_DRAM_RO_REGION)->ulAddrLowBD = Image$$ALL_DRAM_RO$$Base;
    (attr + MEM_MONITOR_ALL_DRAM_RO_REGION)->ulAddrUpBD  = Image$$ALL_DRAM_RO$$Limit;
    (attr + MEM_MONITOR_ALL_DRAM_RO_REGION)->ulWrAllowSrc   = 0;
    (attr + MEM_MONITOR_ALL_DRAM_RO_REGION)->ulRdAllowSrc   = MCI_SRC_MASK;

#if 0
    //OSD Start Region 2
    (attr + MEM_MONITOR_GUI_BUFFER_TOP)->ulAddrLowBD = AHC_GUI_TEMP_BASE_ADDR - 0x20;
    (attr + MEM_MONITOR_GUI_BUFFER_TOP)->ulAddrUpBD  = AHC_GUI_TEMP_BASE_ADDR - 0x01;
    (attr + MEM_MONITOR_GUI_BUFFER_TOP)->ulWrAllowSrc = 0;
    (attr + MEM_MONITOR_GUI_BUFFER_TOP)->ulRdAllowSrc = MCI_SRC_MASK;
    //DCF Start Region 3
    (attr + MEM_MONITOR_DCF_BUFFER_TOP)->ulAddrLowBD = AHC_DCF_TEMP_BASE_ADDR - 0x20;
    (attr + MEM_MONITOR_DCF_BUFFER_TOP)->ulAddrUpBD  = AHC_DCF_TEMP_BASE_ADDR - 0x01;
    (attr + MEM_MONITOR_DCF_BUFFER_TOP)->ulWrAllowSrc = 0;
    (attr + MEM_MONITOR_DCF_BUFFER_TOP)->ulRdAllowSrc = MCI_SRC_MASK;
    //FS Start Region 4
    MMPS_System_GetAudioFrameStart(0,&FSbuf_start);
    (attr + MEM_MONITOR_FS_BUFFER_TOP)->ulAddrLowBD = FSbuf_start - 0x20;
    (attr + MEM_MONITOR_FS_BUFFER_TOP)->ulAddrUpBD  = FSbuf_start - 0x01;
    (attr + MEM_MONITOR_FS_BUFFER_TOP)->ulWrAllowSrc = 0;
    (attr + MEM_MONITOR_FS_BUFFER_TOP)->ulRdAllowSrc = MCI_SRC_MASK;
#endif
}
 
void AHC_MemoryMonitor( AHC_BOOL enable )
{
    static MMP_UBYTE ubMemSeg[MEMORY_MONITOR_MAX_NUM] = {0};
    MMP_ULONG i;

    if (AHC_TRUE == enable)
    {   
        if (ubMemMonitorInitFlag == AHC_TRUE)
        {
            printc(FG_BLUE("Memory Monitor already enabled!\r\n"));
            return;
        }
        else
        {
            MMPF_MONITOR_SEQ_ATTR attr[MEMORY_MONITOR_MAX_NUM] = {0};

            ubMemMonitorInitFlag = AHC_TRUE;

            //set all attr to zero
            for( i=0; i<MEMORY_MONITOR_MAX_NUM; i++)
            {
                memset(&attr[i], 0, sizeof(MMPF_MONITOR_SEQ_ATTR));
            }

            //set parameter to each attr 
            AIHC_MemoryMonitorSetPara(attr);

            //init Monitor
            MMPF_Monitor_Init();

            //Enable Monitor
            for( i=0; i<MEMORY_MONITOR_MAX_NUM; i++)
            {
                if((attr[i].ulAddrLowBD != 0x0)||(attr[i].ulAddrUpBD != 0x0))
                {
                    ubMemSeg[i] = MMPF_Monitor_Enable(&attr[i], AIHC_MemMonitor_Callback);
                    ulSegIdx |= (1 << i);
                    printc("Monitor num : %d\r\n",i);
                    printc("attr[%d].ulAddrLowBD 0x%x\r\n",i,attr[i].ulAddrLowBD);
                    printc("attr[%d].ulAddrUpBD 0x%x\r\n",i,attr[i].ulAddrUpBD);
                }
            }
        }
    }
    else
    {
        if (ubMemMonitorInitFlag == AHC_FALSE)
        {
            printc("Mem Monitor does not initiate yet!\r\n");
        }
        else
        {
            ubMemMonitorInitFlag = AHC_FALSE;
            //Disable Monitor
            for(i = 0; i<MEMORY_MONITOR_MAX_NUM; i++)
            {
                if(ulSegIdx & (1 << i))
                {
                    MMPF_Monitor_Disable(ubMemSeg[i]);  //disable monitor
                    printc("Disable Seg Section[%d]\r\n",i);
                }
            }
        }
    }
}

void AIHC_DBG_PrintHorizontalLine(void)
{
    printc( "\r\n\r\n===================================\n");
}

void AIHC_DBG_PrintTaskTable( INT8U* pTaskTable )
{
	int 	i;
	int 	n;
	INT8U  	bValue;

	for( i =0, n=0; i<OS_LOWEST_PRIO+1; i++ )
	{
		int offset = i%8;
	
		if( offset == 0 )
		{
			bValue = pTaskTable[n++];
		}
		
		if( bValue & 0x1 )
		{
			printc( "TASK[%d] ", i );
		} 
		
		bValue >>= 1;	
	} 

	printc( "\n" );
}

void AIHC_DBG_PrintTCB(OS_TCB* pTCB)
{
    int 		i = 0;
    MMP_ULONG 	*pulStackData = pTCB->OSTCBStkPtr;
    MMP_ULONG 	*pulBottom    = pTCB->OSTCBStkBottom;
    MMP_ULONG 	*pulTop       = pTCB->OSTCBStkBottom +( pTCB->OSTCBStkSize );

    AIHC_DBG_PrintHorizontalLine();
    
    printc( "TaskName: %s\n", pTCB->OSTCBTaskName);
    printc( "Thread ID: %d\n", pTCB->OSTCBPrio);
    printc( "OSTCBStkBottom : 0x%X = %X\n", pulBottom, *pulBottom );
    printc( "OSTCBStkSize   : %d\n"  , pTCB->OSTCBStkSize );
    printc( "OSTCBStkPtr    : 0x%X\n", pTCB->OSTCBStkPtr );
    printc( "pulTop         : 0x%X\n", pulTop );
    
	if( pTCB->OSTCBPendTO != FALSE )
	{
	    printc( "Pending time out: %d\n"  , pTCB->OSTCBDly );	
	}

	if( pTCB->OSTCBFlagNode != 0 )
	{
		printc( "Flag 0x%08x\n", pTCB->OSTCBFlagNode->OSFlagNodeFlagGrp );
		printc( "wait for: 0x%08x wait type:%d\n", pTCB->OSTCBFlagNode->OSFlagNodeFlags
		                                         , pTCB->OSTCBFlagNode->OSFlagNodeWaitType);
	}

	if( pTCB->OSTCBEventPtr != 0 )
	{
		printc( "Event 0x%08X \n", pTCB->OSTCBEventPtr );
		printc( "Pending Task:");
		AIHC_DBG_PrintTaskTable(pTCB->OSTCBEventPtr->OSEventTbl);
	}

	while( pulStackData < pulTop )
	{
	    if( i%4 == 0 )
		    printc( "\n");

        printc( "S_%04d:0x%08X\t",i,*pulStackData++); 
	    i++;
	} 

    AIHC_DBG_PrintHorizontalLine();
}

/*
    Check if a stack is overflowed.
*/
AHC_BOOL AIHC_DBG_CheckStack(OS_TCB* pTCB)
{
    MMP_ULONG 	*pulBottom    = pTCB->OSTCBStkBottom;

	if( OS_STACK_EYE_CATCH != *pulBottom )
	{
		return AHC_FALSE;
	}

	return AHC_TRUE;
}

AHC_BOOL AHC_CheckAllTaskStack(void)
{
    int 	 i;
    AHC_BOOL bRet;

    for( i=0; i<OS_LOWEST_PRIO + 1 ; i++ )
    {
		if( OSTCBPrioTbl[i] != 0 &&
            OSTCBPrioTbl[i] != (OS_TCB *)1 )
		{
    		if( !AIHC_DBG_CheckStack(OSTCBPrioTbl[i]) )
    		{
                bRet = AHC_FALSE;
                printc( "The stack of task%d might overflow!\n", i);
    		}
		}
	}

    return bRet;
}

void AHC_DebugPrintReadyTable(void)
{
	printc( " ===== READY TABLE =====\n" );
	
	AIHC_DBG_PrintTaskTable( OSRdyTbl );
	
	printc( " ===== READY TABLE END =====\n" );
}

void AHC_PrintAllTask(void)
{
    int 	i;
    OS_TCB* pTCB = OSTCBCur;

    printc( "Current Thread\n");
    AIHC_DBG_PrintTCB(pTCB);

    for( i=0; i<OS_LOWEST_PRIO + 1 ; i++ )
    {
#if defined(AIT_HW_WATCHDOG_ENABLE) && (AIT_HW_WATCHDOG_ENABLE)
        AHC_WD_Kick();
#endif
    
        if( OSTCBPrioTbl[i] != 0 &&
            OSTCBPrioTbl[i] != (OS_TCB *)1 )
        {
            AIHC_DBG_PrintTCB(OSTCBPrioTbl[i]);
        }
    }
}

void AHC_PrintCurrentTask(void)
{
    OS_TCB* pTCB = OSTCBCur;

    OS_CRITICAL_INIT();
    printc( "Current Thread\n");
	OS_ENTER_CRITICAL();
    AIHC_DBG_PrintTCB(pTCB);
    OS_EXIT_CRITICAL();
}

void ARM_ExceptionHandlerCB(void)
{
#if 1 //def __ARM_EXCEPTION_PRINT_TASK__
    AIHC_DBG_PrintHorizontalLine();
    AHC_PrintAllTask();
#endif

#ifdef __ARM_EXCEPTION_PRINT_TIMING_MARK__
    AIHC_DBG_PrintHorizontalLine();
    AHC_UF_DumpDB();
    
    #if (SUPPORT_TIMING_MARK == 1)
    MMPF_TimingStop();
    MMPF_TimingDump(0);
    #endif
#endif

#if (TASK_MONITOR_ENABLE)
    MMPF_TaskMonitor_PrintAllTaskInfo();
#endif

#ifdef CFG_CUS_EXCEPTION_POWER_OFF
    CFG_CUS_EXCEPTION_POWER_OFF();
#else
    PMUCtrl_Power_Off_For_Exception();
#endif
    while(1);
}

int ARM_ExceptionHandler_Module_Init(void)
{
    printc("[ARM_ExceptionHandler_Module_Init]\r\n");
    MMPF_Register_ARM_ExceptionHandlerCB(ARM_ExceptionHandlerCB);
    return 0;    
}

#pragma arm section code = "initcall1", rodata = "initcall1", rwdata = "initcall1",  zidata = "initcall1" 
#pragma O0
ait_core_init(ARM_ExceptionHandler_Module_Init);
#pragma
#pragma arm section rodata, rwdata ,  zidata 

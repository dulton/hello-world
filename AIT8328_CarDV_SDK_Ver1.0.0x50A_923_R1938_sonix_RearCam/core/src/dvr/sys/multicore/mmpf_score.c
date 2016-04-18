//==============================================================================
//
//  File        : mmpf_score.c
//  Description : Slave Core to Master Core function
//  Author      : Alterman
//  Revision    : 1.0
//
//==============================================================================
#if 0		 
void _____HEADER_____(){}
#endif

#include "mmpf_score.h"
#include "mmp_reg_uart.h"
#if (MDTC_ON_SCORE)
#include "mmpf_vmd.h"
#endif

/** @addtogroup MMPF_MULITCORE
@{
*/

//==============================================================================
//
//                              VARIABLES
//
//==============================================================================

#if (MDTC_ON_SCORE)
static OS_STK   MDTC_Task_Stk[SCORE_TASK_MDTC_STK_SIZE];
#endif
#if (ADAS_ON_SCORE)
static OS_STK   ADAS_Task_Stk[SCORE_TASK_ADAS_STK_SIZE];
#endif

//==============================================================================
//
//                              EXTERNAL FUNCTION
//
//==============================================================================

#if (ADAS_ON_SCORE)
extern void ADAS_CTL_ProcRequest(DUALCORE_REQ *req, DUALCORE_RESP *resp);
#endif

//==============================================================================
//
//                              FUNCTION
//
//==============================================================================
#if (MDTC_ON_SCORE)||(ADAS_ON_SCORE)
void MMPF_SCORE_HandleRequest(MMP_UBYTE ubID, SCORE_ReqHandler *pfReqHandler)
{
    CPU_COMM_ERR        err;
    DUALCORE_REQ        req;
    DUALCORE_REQ_OBJ    *req_obj = (DUALCORE_REQ_OBJ *)&req;
    DUALCORE_RESP       resp;
    DUALCORE_RESP_OBJ   *resp_obj = (DUALCORE_RESP_OBJ *)&resp;

    req_obj->ubID       = ubID;
    req_obj->ubLength   = MAX_REQ_PARAM_SIZE;   // can't know the exact length
    req_obj->usTimeout  = 0;                    // ToDo: now never timeout

    resp_obj->ubID      = ubID;
    resp_obj->ubLength  = MAX_REQ_PARAM_SIZE;   // shoule be updated by command
    resp_obj->usTimeout = 0;                    // ToDo: now never timeout

    while(1) {
        /* Wait request from master CPU */
        err = CpuComm_DataReceive(req_obj->ubID, req.ubData,
                                  req_obj->ubLength, req_obj->usTimeout, NULL);
        if (err != CPU_COMM_ERR_NONE) {
            SCORE_ERR("#SC_Err: WaitReq\r\n");
            continue;
        }

        pfReqHandler(&req, &resp);

        /* Feedback the execution result */
        err = CpuComm_DataSend(resp_obj->ubID, resp.ubData, resp_obj->ubLength,
                               resp_obj->usTimeout);
        if (err != CPU_COMM_ERR_NONE) {
            SCORE_ERR("#SC_Err: SendResp\r\n");
        }
    }
}
#endif

#if (MDTC_ON_SCORE)
//------------------------------------------------------------------------------
//  Function    : MMPF_SCORE_HandleMDTC
//  Description : 
//------------------------------------------------------------------------------
/**	@brief The function allows slave core to handle MDTC request from master core.
           After slave core completes the request, it will feedback the result
           to master core.

@warning   This function isn't thread-safe, don't call it by different tasks
           concurrently.
*/
void MMPF_SCORE_HandleMDTC(void *p_arg)
{
    MMPF_SCORE_HandleRequest(CPU_COMM_ID_MDTC, MMPF_VMD_ProcRequest);
}
#endif

#if (ADAS_ON_SCORE)
//------------------------------------------------------------------------------
//  Function    : MMPF_SCORE_HandleADAS
//  Description : 
//------------------------------------------------------------------------------
/**	@brief The function allows slave core to handle ADAS request from master core.
           After slave core completes the request, it will feedback the result
           to master core.

@warning   This function isn't thread-safe, don't call it by different tasks
           concurrently.
*/
void MMPF_SCORE_HandleADAS(void *p_arg)
{
    MMPF_SCORE_HandleRequest(CPU_COMM_ID_ADAS, ADAS_CTL_ProcRequest);
}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPF_SCORE_Init
//  Description : 
//------------------------------------------------------------------------------
/**	@brief The function initializes the slave core to create task for handling
           request from master core and feedback response.
*/
CPU_COMM_ERR MMPF_SCORE_Init(void)
{
    #if (SCORE_EN)
    MMPF_TASK_CFG   task_cfg;
    #endif
    CPU_COMM_ERR    err = CPU_COMM_ERR_NONE;

    #if (MDTC_ON_SCORE)
    /* Register entries for MDTC request ID */
    err = CpuComm_RegisterEntry(CPU_COMM_ID_MDTC, CPU_COMM_TYPE_DATA);

    /* Create task for MDTC request ID */
    if (err == CPU_COMM_ERR_NONE) {
        task_cfg.ubPriority = SCORE_TASK_MDTC_PRIO;
        task_cfg.pbos = (MMP_ULONG)&MDTC_Task_Stk[0];
        task_cfg.ptos = (MMP_ULONG)&MDTC_Task_Stk[SCORE_TASK_MDTC_STK_SIZE - 1];
        MMPF_OS_CreateTask(MMPF_SCORE_HandleMDTC, &task_cfg, (void *)0);
        MMPF_OS_SetTaskName(task_cfg.ubPriority, (MMP_UBYTE *)"MDTC");
    }
    if (err) {
        SCORE_ERR("#SC_Err: MDTC task\r\n");
        return err;
    }
    #endif

    #if (ADAS_ON_SCORE)
    /* Register entries for ADAS request ID */
    err = CpuComm_RegisterEntry(CPU_COMM_ID_ADAS, CPU_COMM_TYPE_DATA);

    /* Create task for ADAS request ID */
    if (err == CPU_COMM_ERR_NONE) {
        task_cfg.ubPriority = SCORE_TASK_ADAS_PRIO;
        task_cfg.pbos = (MMP_ULONG)&ADAS_Task_Stk[0];
        task_cfg.ptos = (MMP_ULONG)&ADAS_Task_Stk[SCORE_TASK_ADAS_STK_SIZE - 1];
        MMPF_OS_CreateTask(MMPF_SCORE_HandleADAS, &task_cfg, (void *)0);
        MMPF_OS_SetTaskName(task_cfg.ubPriority, (MMP_UBYTE *)"ADAS");
    }
    if (err) {
        SCORE_ERR("#SC_Err: ADAS task\r\n");
        return err;
    }
    #endif

    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_SCORE_Exit
//  Description : 
//------------------------------------------------------------------------------
/**	@brief The function is the Destructor routine of the slave core.
*/
CPU_COMM_ERR MMPF_SCORE_Exit(void)
{
    CPU_COMM_ERR err;

    #if (MDTC_ON_SCORE)
    err = CpuComm_UnregisterEntry(CPU_COMM_ID_MDTC);
    #endif
    #if (ADAS_ON_SCORE)
    err = CpuComm_UnregisterEntry(CPU_COMM_ID_ADAS);
    #endif

    return err;
}

CPUCOMM_MODULE_INIT(MMPF_SCORE_Init)
CPUCOMM_MODULE_EXIT(MMPF_SCORE_Exit)

/// @}


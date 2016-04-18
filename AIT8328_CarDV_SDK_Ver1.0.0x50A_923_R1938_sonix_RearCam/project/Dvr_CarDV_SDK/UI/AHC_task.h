#ifndef _AHC_TASK_H
#define _AHC_TASK_H

/*===========================================================================
 * Include file 
 *===========================================================================*/ 

#include "includes_fw.h"
#include "customer_config.h"
#include "lib_retina.h"

void KeyPadTask(void *p_arg);
void MotorCtrlTask(void *p_arg);
void AhcTask(void *p_arg);
void AhcWorkTask(void *p_arg);
void AhcWorkJobTask(void *p_arg);

#if (FW_DCF_BACKGROUND == 1)
void AhcDcfTask(void *p_arg);
#endif

#if  (KEYPAD_DETECT_METHOD == KEYPAD_DETECT_TASK)	
void RealIDKeyPadTask(void *p_arg); 
#endif

void AhcMotorTask(void *p_arg);
void AhcMotorJobTask(void *p_arg);
void UARTCtrl_TaskHandler(void *p_arg);

//Wifi
void NETWORK_TaskHandler(void *p_arg); //Andy Liu TBD
void MMPF_Streaming_TaskHandler(void *p_arg); //Andy Liu TBD
//void SOFTIRQ_TaskHandler(void *p_arg);

void AHC_SWPACK_TaskInit(void);

#endif //#ifndef _AHC_TASK_H

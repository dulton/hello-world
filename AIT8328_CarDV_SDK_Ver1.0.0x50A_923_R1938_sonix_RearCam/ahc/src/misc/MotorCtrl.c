//******************************************************************************
//
//  File        : MotorCtrl.c
//  Description : Motor Control 
//  Author      : Wayne.
//  Revision    : 1.0
//
//******************************************************************************

#include "includes_fw.h"
#include "isp_if.h"
#include "MotorCtrl.h"
#include "AHC_message.h"
#include "AHC_OS.h"

//******************************************************************************
//
//                              DEFINITION
//
//******************************************************************************

AHC_OS_FLAGID   MotorCtrlFlag;
UINT32 MT_Event;
//******************************************************************************
//
//                              VARIABLES
//
//******************************************************************************

//******************************************************************************
//
//                              FUNCTION PROTOTYPES
//
//******************************************************************************
extern void AhcMotorTask(void *p_arg);
extern void AhcMotorJobTask(void *p_arg);
//extern void Zoom_Go2Section(unsigned char section);
//******************************************************************************
//
//                              Function API
//
//******************************************************************************
INT16 Motor_Get_Current_Pos()
{
 #if 0
    return Check_CurrZoomPos();
 #else
    return 1;
 #endif
}

void Motor_Set_Zoom_Pos(UINT16 pos)
{

}

void Motor_Set_Zoom_Event(UINT32 ulEvent)
{
    MT_Event = ulEvent;
    AHC_OS_SetFlags(MotorCtrlFlag, MOTOR_FLAG_ZOOM_JOB, AHC_OS_FLAG_SET);
}

void Ahc_Motor_Task(void *p_arg)
{
    //AHC_OS_FLAGS  flags;
    //AHC_BOOL      mStatus = AHC_FALSE;
    //UINT32        ulParam1;
    //UINT32        ulParam2;
    //UINT8         ubCurrState;


    while(1)
    {
#if 0
        AHC_OS_WaitFlags( MotorCtrlFlag, MOTOR_FLAG_ZOOM_JOB,
                                       AHC_OS_FLAG_WAIT_SET_ANY|AHC_OS_FLAG_CONSUME, 
                                       0, &flags);

        if ( flags & MOTOR_FLAG_ZOOM_JOB )
        {
         	switch(MT_Event)
            {
                case MOTOR_FLAG_ZOOM_IN      :
        			 ulParam1 = MOTOR_FLAG_ZOOM_IN;
        			 ulParam2 = 11;    
                   #if 0
					 if( MOTOR_FLAG_ZOOM_OUT == ubCurrState )
        			 {
                         ZoomStop_ByKEY(AHC_TRUE);
        				 AHC_OS_Delay(0, 0, 0, 50);
        			 }
        		   #endif
        		//if( AHC_FALSE == mStatus )
        			 {
                         mStatus = AHC_TRUE;
                         ubCurrState = MOTOR_FLAG_ZOOM_IN;
                         AHC_OS_SetFlags(MotorCtrlFlag, MOTOR_FLAG_ZOOM_IN, AHC_OS_FLAG_SET);
        			 }
        
                break;
        
                case MOTOR_FLAG_ZOOM_OUT     :
        			 ulParam1 = MOTOR_FLAG_ZOOM_OUT;
        			 ulParam2 = 0;
                   #if 0
        			 if( MOTOR_FLAG_ZOOM_IN == ubCurrState )
        			 {
                         ZoomStop_ByKEY(AHC_TRUE);
        				 AHC_OS_Delay(0, 0, 0, 50);
        			 }
        		   #endif
                   //if( AHC_FALSE == mStatus )
        			 {
                         mStatus = AHC_TRUE;
                         AHC_OS_SetFlags(MotorCtrlFlag, MOTOR_FLAG_ZOOM_OUT, AHC_OS_FLAG_SET);
        			 }
        
                break;
        
                case MOTOR_FLAG_ZOOM_STOP    :

                   //if( AHC_TRUE == mStatus )
                     {
                         mStatus = AHC_FALSE;
                         ubCurrState = MOTOR_FLAG_ZOOM_STOP;
                         ZoomStop_ByKEY(AHC_TRUE);
        			 }
                break;
        
                default:
                break;
            }

	    }
#else
    MMPF_OS_Sleep(1000);
#endif
	}

}

void Ahc_Motor_Job_Task(void *p_arg)
{
    //AHC_OS_FLAGS  flags;

    while(1)
    {
#if 0
        AHC_OS_WaitFlags( MotorCtrlFlag, MOTOR_FLAG_ZOOM_IN | MOTOR_FLAG_ZOOM_OUT,
                                       AHC_OS_FLAG_WAIT_SET_ANY|AHC_OS_FLAG_CONSUME, 
                                       0, &flags);

        if ( flags & MOTOR_FLAG_ZOOM_IN )
        {
            Zoom_Go2Section(11);
		}
		else if( flags & MOTOR_FLAG_ZOOM_OUT )
		{
            Zoom_Go2Section(0);
		}
		else
		{

		}
#else
        MMPF_OS_Sleep(1000);
#endif
	}

}

void AhcMotorJobTask(void *p_arg)
{
    Ahc_Motor_Job_Task(p_arg);
}


void AhcMotorTask(void *p_arg)
{
    MotorCtrlFlag = AHC_OS_CreateEventFlag(0);
    Ahc_Motor_Task(p_arg);
}


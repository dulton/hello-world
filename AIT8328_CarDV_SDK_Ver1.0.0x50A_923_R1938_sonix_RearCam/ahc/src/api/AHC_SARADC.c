//==============================================================================
//
//  File        : AHC_SARADC.c
//  Description : SAR ADC Functions.
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================
/**
 *  @file AHC_SARADC.c
 *  @brief The SAR ADC control functions
 *  @author 
 *  @version 1.0
 */

//==============================================================================
//
//                              INCLUDE FILE
//
//============================================================================== 
#include "customer_config.h"
 
#include "mmp_lib.h"
#include "AHC_OS.h"
#include "AHC_SARADC.h"
#include "mmpf_saradc.h"
#include "mmp_err.h"

//==============================================================================
//
//                              LOCAL VARIABLES
//
//==============================================================================
#ifdef ALL_FW    
static MMPF_OS_SEMID sSARADCSemID = 0xFF;
#endif

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================
//------------------------------------------------------------------------------
//  Function    : AHC_SARADC_ReadData
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get ADC Data

 @return the status of the operation.
*/
AHC_BOOL AHC_SARADC_ReadData(UINT16 channel, UINT16 *psaradc_data)
{
    UINT16 saradc_data = 0;
	MMPF_SARADC_ATTR sar_att = {0};

    if (psaradc_data)
        *psaradc_data = 0;

#ifdef ALL_FW    
	if (sSARADCSemID == 0xFF) {
		sSARADCSemID = AHC_OS_CreateSem(1);
	}

	if (0 != MMPF_OS_AcquireSem(sSARADCSemID, 1000)) {
        printc(FG_RED("--E-- %s:%d Acquire sSARADCSemID Semaphore fail\r\n"), __func__, __LINE__);
        return AHC_FALSE;
	}
#endif
    
	sar_att.TPWait = MMP_TRUE;
	MMPF_SARADC_Init(&sar_att); 
	MMPF_SARADC_SetChannel(channel + 1);
	MMPF_SARADC_SetMode(POLLING_MODE);
	MMPF_SARADC_SetMeasMode(FAST_MODE, FS, MMP_FALSE);
	MMPF_SARADC_SetFastRead();
	MMPF_SARADC_StartADC();
	/*start polling*/
	MMPF_SARADC_SetPoll();

	MMPF_SARADC_SetChannel(channel + 1);
	MMPF_SARADC_GetData(&saradc_data);
	MMPF_SARADC_PowerDown();

    if (channel == 0) {
        if (psaradc_data)
            *psaradc_data = (UINT32) saradc_data * (UINT32) 3 + BATTERY_VOLTAGE_OFFSET;
    }
    else {
        if (psaradc_data)
            *psaradc_data = (UINT32) saradc_data * (UINT32) SARADC_REFERENCE_VOLTAGE / (UINT32) SARADC_FULL_SCALE;
    }

#ifdef ALL_FW    
	MMPF_OS_ReleaseSem(sSARADCSemID);
#endif

    return AHC_TRUE;
}



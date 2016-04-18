
#ifndef _CONTROLIC_CUSTOMER_PMU_H_
#define _CONTROLIC_CUSTOMER_PMU_H_

#include "config_fw.h"

#if (CUSTOMER_PMU_EN)

#include "mmpf_rtc.h"
#include "mmpf_pio.h"
#include "controlIC_reg_customer_pmu.h"
#if defined(EXT_RTC_DS1302_EN) && EXT_RTC_DS1302_EN

#endif

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================
// HW I2CM
#define CUSTOMER_PMU_SLAVE_ADDR 	0x12

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================


//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
MMP_ERR CUSTOMER_PMU_Initialize(void);
MMP_ERR CUSTOMER_PMU_RTC_Initialize(void);
MMP_ERR CUSTOMER_PMU_RTC_SetTime(AUTL_DATETIME* ptr);
MMP_ERR CUSTOMER_PMU_RTC_GetTime(AUTL_DATETIME* ptr);
MMP_USHORT CUSTOMER_PMU_Measure_Power_Key(void);

MMP_ERR CUSTOMER_PMU_ADC_Measure_Key(MMP_ULONG *level, CUSTOMER_PMU_ADC_TYPE type);
MMP_ERR CUSTOMER_PMU_PowerOff(void);
MMP_ERR CUSTOMER_PMU_Write_Reg(CUSTOMER_PMU_REG regNAME, MMP_UBYTE nValue); 
MMP_ERR CUSTOMER_PMU_Read_Reg(CUSTOMER_PMU_REG regNAME, MMP_USHORT *returnValue);

#endif // #if (CUSTOMER_PMU_EN)

#endif // #ifndef _CONTROLIC_CUSTOMER_PMU_H_

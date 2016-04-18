
#ifndef _CONTROLIC_ADX2003_H_
#define _CONTROLIC_ADX2003_H_

#include "mmpf_pio.h"
#include "controlIC_reg_adx2003.h"
#include "aitu_calendar.h"

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================
//Software I2CM
//#define ADX2003_SLAVE_ADDR		0x24

//HW I2CM
#define ADX2003_SLAVE_ADDR		0x12

#define ADX2003_LEVEL_BIT		0x3F
#define ADX2003_RTC_LMASK		0x0F

#define ADX2003_RTC_START_YEAR	2000
#define ADX2003_RTC_MAX_YEAR	2160

#define ADX2003_LDO_EN			0xC0
#define ADX2003_LDO_MAX_LEVEL	0x1F
#define ADX2003_DCDC_BUCK1_EN	0x40
#define ADX2003_DCDC_REG_SIZE	0x06
#define ADX2003_CLASSG_MAX_VOL	0x4E
#define ADX2003_DCDC_DISEN		0x80

#define ADX2003_SEM_TIMEOUT   	0x0
#define ADX2003_WDT_EN 			0x1  // 1: To turn-on watch-dog timer,  0: turn-off
#define ADX2003_DEBUG_LEVEL	   	0x0

#define ADX2003_ACT_DISCHARGE	0x1

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef enum _ADX2003_DCDC_ID
{
	ADX2003_DCDC_ID_1 = 0x0,
	ADX2003_DCDC_ID_2,
	ADX2003_DCDC_ID_3,
	ADX2003_DCDC_MAX
}ADX2003_DCDC_ID;

typedef enum _ADX2003_LDO_ID
{
	ADX2003_LDO_ID_1 = 0x0,
	ADX2003_LDO_ID_2,
	ADX2003_LDO_ID_3,
	ADX2003_LDO_ID_4,
	ADX2003_LDO_ID_5,
	ADX2003_LDO_MAX
}ADX2003_LDO_ID;

typedef enum _ADX2003_ALARM_ID
{
	ADX2003_ALARM_ID_NONE = 0x0,
	ADX2003_ALARM_ID_A,
	ADX2003_ALARM_ID_B,
	ADX2003_ALARM_MAX
}ADX2003_ALARM_ID;

typedef enum _ADX2003_INT_EVENT
{
	ADX2003_INT_EVENT_WAKE = 0x0,
	ADX2003_INT_EVENT_RTC,		//In ADX2012, for alarm A only
	ADX2003_INT_EVENT_USBJIG,
	ADX2003_INT_EVENT_VBUS,
	ADX2003_INT_EVENT_POWERK,
	ADX2003_INT_EVENT_RTC2,  	//In ADX2012, for alarm B only
	ADX2003_INT_EVENT_MAX
}ADX2003_INT_EVENT;

#if ADX2003_EN
typedef enum _ADX2003_PMU_VERSION
{
	ADX2003_PMU_V_NONE = 0x0,
	ADX2003_PMU_V_2003,
	ADX2003_PMU_V_2013,
	ADX2003_PMU_V_2013E,
	ADX2003_PMU_V_MAX
}AIT_PMU_VERSION;

typedef enum _ADX2003_ECO_VER_ID
{
	ADX2003_ECO_VER_NONE = 0x00,
	ADX2003_ECO_VER_0430 = 0xFF,
	ADX2003_ECO_VER_0510 = 0x02,
	ADX2003_ECO_VER_MAX = 0x03
}ADX2003_ECO_VER_ID;
#endif

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
MMP_ERR ADX2003_Initialize(void);
MMP_ERR ADX2003_LDO_SetVoltage(ADX2003_LDO_ID ldoID, MMP_USHORT vLevel);
MMP_ERR ADX2003_LDO_GetVoltage(ADX2003_LDO_ID ldoID, MMP_USHORT* returnVoltage);
MMP_ERR ADX2003_DCDC_SetVoltage(ADX2003_DCDC_ID dcdcID, MMP_USHORT vLevel);
MMP_ERR ADX2003_DCDC_GetVoltage(ADX2003_DCDC_ID dcdcID, MMP_USHORT* returnVoltage);
MMP_ERR ADX2003_CLASSG_En(MMP_BOOL bEnable);
MMP_ERR ADX2003_CLASSG_SetVol(MMP_UBYTE vol_db);
MMP_ERR ADX2003_RTC_Initialize(void);
MMP_ERR ADX2003_RTC_SetTime(AUTL_DATETIME *ptr);
MMP_ERR ADX2003_RTC_GetTime(AUTL_DATETIME *ptr);
MMP_ERR ADX2003_RTC_SetAlarmEnable(ADX2003_ALARM_ID id, MMP_BOOL bEnable, AUTL_DATETIME *ptr, void* p_callback);
MMP_ERR ADX2003_CheckBootComplete(MMP_ULONG ulMilliSecond, MMP_GPIO_PIN pio_power_key);
void 	ADX2003_IsrHandler(void);
MMP_ERR ADX2003_PowerOff(void);
MMP_ERR ADX2003_EnableLDO(ADX2003_LDO_ID ldoID, MMP_BOOL bEnable);
MMP_ERR ADX2003_EnableDCDC(ADX2003_DCDC_ID dcdcID, MMP_BOOL bEnable);
MMP_ERR ADX2003_ADC_Measure(MMP_USHORT *level);
MMP_ERR ADX2003_ActiveDisaharge_En(MMP_BOOL bEnable);

#if (ADX2003_WDT_EN == 0x1)
MMP_ERR ADX2003_WDT_CleanStatus(void);
MMP_ERR ADX2003_WDT_Reset(void);
MMP_ERR ADX2003_WDT_Enable(MMP_BOOL bEnable);
MMP_ERR ADX2003_WDT_SetTimeOut(MMP_ULONG ulMilleSec);
#endif

MMP_USHORT ADX2003_Measure_Power_Key(void);
MMP_ERR ADX2003_Write_Reg(ADX2003_REG regNAME, MMP_UBYTE nValue);
MMP_ERR ADX2003_Read_Reg(ADX2003_REG regNAME, MMP_USHORT *returnValue);
MMP_ERR ADX2003_ADC_Measure_Key( MMP_ULONG *level, MMP_UBYTE *source, ADX2003_ADC_TYPE type);
MMP_ERR ADX2003_ADC_Measure_Lens_Temp( MMP_ULONG *level, MMP_UBYTE *source);
MMP_UBYTE ADX2003_GetIntPinState(MMP_BOOL bGetNewData );

#endif
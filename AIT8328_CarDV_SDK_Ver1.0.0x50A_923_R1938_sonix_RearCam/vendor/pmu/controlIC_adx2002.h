
#ifndef _CONTROLIC_ADX2002_H_
#define _CONTROLIC_ADX2002_H_

#include "mmp_gpio_inc.h"
#include "controlIC_reg_adx2002.h"
#include "aitu_calendar.h"

#if (ADX2002_EN)
//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================
//Software I2CM
//#define ADX2002_SLAVE_ADDR		0x24

//HW I2CM
#define ADX2002_SLAVE_ADDR		0x12

#define ADX2002_LEVEL_BIT		0x3F
#define ADX2002_RTC_LMASK		0x0F
#define ADX2002_RTC_START_YEAR	2000
#define ADX2002_RTC_MAX_YEAR	2089

#define ADX2002_LDO_EN			0xC0
#define ADX2002_LDO_MAX_LEVEL	0x1F
#define ADX2002_DCDC_BUCK1_EN	0x40
#define ADX2002_DCDC_REG_SIZE	0x06
#define ADX2002_CLASSG_MAX_VOL	0x4E

#define ADX2002_SEM_TIMEOUT   	0x0
#define ADX2002_WDT_EN 			0x1  // 1: To turn-on watch-dog timer,  0: turn-off
#define ADX2002_DEBUG_LEVEL	   	0x0
//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================


typedef enum _ADX2002_DCDC_ID
{
	ADX2002_DCDC_ID_1 = 0x0,
	ADX2002_DCDC_ID_2,
	ADX2002_DCDC_ID_3,
	ADX2002_DCDC_MAX
}ADX2002_DCDC_ID;

typedef enum _ADX2002_LDO_ID
{
	ADX2002_LDO_ID_1 = 0x0,
	ADX2002_LDO_ID_2,
	ADX2002_LDO_ID_3,
	ADX2002_LDO_ID_4,
	ADX2002_LDO_ID_5,
	ADX2002_LDO_MAX
}ADX2002_LDO_ID;

typedef enum _ADX2002_ALARM_ID
{
	ADX2002_ALARM_ID_NONE = 0x0,
	ADX2002_ALARM_ID_A,
	ADX2002_ALARM_ID_B,
	ADX2002_ALARM_MAX
}ADX2002_ALARM_ID;


typedef enum _ADX2002_INT_EVENT
{
	ADX2002_INT_EVENT_WAKE = 0x0,
	ADX2002_INT_EVENT_RTC,		//In ADX2012, for alarm A only
	ADX2002_INT_EVENT_USBJIG,
	ADX2002_INT_EVENT_VBUS,
	ADX2002_INT_EVENT_POWERK,
	ADX2002_INT_EVENT_RTC2,  	//In ADX2012, for alarm B only
	ADX2002_INT_EVENT_MAX
}ADX2002_INT_EVENT;

typedef enum _ADX2002_PMU_MODE
{
	ADX2002_PMU_V_NONE = 0x0,
	ADX2002_PMU_V_2002,
	ADX2002_PMU_V_2012,
	ADX2002_PMU_V_2012E,
	ADX2002_PMU_V_MAX
}AIT_PMU_MODE;
//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
MMP_ERR ADX2002_Initialize(void);
MMP_ERR ADX2002_LDO_SetVoltage(ADX2002_LDO_ID ldoID, MMP_USHORT vLevel);
MMP_ERR ADX2002_LDO_GetVoltage(ADX2002_LDO_ID ldoID, MMP_USHORT* returnVoltage);
MMP_ERR ADX2002_DCDC_SetVoltage(ADX2002_DCDC_ID dcdcID, MMP_USHORT vLevel);
MMP_ERR ADX2002_DCDC_GetVoltage(ADX2002_DCDC_ID dcdcID, MMP_USHORT* returnVoltage);
MMP_ERR ADX2002_CLASSG_En(MMP_BOOL bEnable);
MMP_ERR ADX2002_CLASSG_SetVol(MMP_UBYTE vol_db);
MMP_ERR ADX2002_RTC_Initialize(void);
MMP_ERR ADX2002_RTC_SetTime(AUTL_DATETIME *ptr);
MMP_ERR ADX2002_RTC_GetTime(AUTL_DATETIME *ptr);
MMP_ERR ADX2002_RTC_SetAlarmEnable(ADX2002_ALARM_ID id, MMP_BOOL bEnable, AUTL_DATETIME *ptr, void* p_callback);
MMP_ERR ADX2002_CheckBootComplete(MMP_ULONG ulMilliSecond, MMP_GPIO_PIN pio_power_key);
void 	ADX2002_IsrHandler(void);
MMP_ERR ADX2002_PowerOff(void);
MMP_ERR ADX2002_EnableLDO(ADX2002_LDO_ID ldoID, MMP_BOOL bEnable);
MMP_ERR ADX2002_EnableDCDC(ADX2002_DCDC_ID dcdcID, MMP_BOOL bEnable);
MMP_ERR ADX2002_ADC_Measure(MMP_USHORT *level);
void 	ADX2002_DumpAll(void);
#if (ADX2002_WDT_EN == 0x1)
MMP_ERR ADX2002_WDT_CleanStatus(void);
MMP_ERR ADX2002_WDT_Reset(void);
MMP_ERR ADX2002_WDT_Enable(MMP_BOOL bEnable);
MMP_ERR ADX2002_WDT_SetTimeOut(MMP_ULONG ulMilleSec);
MMP_ERR ADX2002_WDT_Kick(void);
#endif
#endif //ADX2002_EN
#endif

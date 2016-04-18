/*===========================================================================
 * Include file
 *===========================================================================*/

#include "Customer_Config.h"
#include "controlIC_customer_pmu.h"
#include "lib_retina.h"
#include "mmpf_i2cm.h"
#include "mmpf_pio.h"
#include "mmpf_rtc.h"
#include "mmpf_timer.h"
#include "os_wrap.h"

#if (CUSTOMER_PMU_EN)

#if defined(EXT_RTC_DS1302_EN) && (EXT_RTC_DS1302_EN)
#include "controlIC_DS1302_RTC.h"
#endif
#ifdef MCU_HT46R002
#include "Mcu_HT46r002.h"
#endif

/*===========================================================================
 * Macro define
 *===========================================================================*/


/*===========================================================================
 * Global varible
 *===========================================================================*/

// HW I2CM
//MMP_I2CM_ATTR 	gI2cmAttribute_Customer_PMU = {MMP_I2CM2, CUSTOMER_PMU_SLAVE_ADDR, 8, 8, 0x2, MMP_FALSE, MMP_FALSE, MMP_FALSE, MMP_FALSE, 0, 0, 6, 50000/*450000*/ /*250KHZ*/, MMP_TRUE, NULL, NULL, MMP_FALSE, MMP_FALSE, MMP_FALSE};

/*===========================================================================
 * Extern varible
 *===========================================================================*/ 

#if defined(MBOOT_FW) || defined(UPDATER_FW)
	extern MMP_BOOL ConfigGPIOPad(MMP_GPIO_PIN piopin, MMP_UBYTE config);
	#define AHC_ConfigGPIOPad 	ConfigGPIOPad
#endif

/*===========================================================================
 * Main body
 *===========================================================================*/ 

#if 0
static void CUSTOMER_PMU_Timer_TaskMode(void)
{

}

static void CUSTOMER_PMU_IsrTimerCallBack(OS_TMR *ptmr, void *p_arg)
{

}
#endif

//------------------------------------------------------------------------------
//  Function    : CUSTOMER_PMU_Read_Reg
//  Description : This function is used for reading PMU chip's registers.
//------------------------------------------------------------------------------
/** @brief This function is used for reading PMU chip's registers.
@param[in] 	regNAME is main register.
@param[out] returnValue is the value get from the register.
@return 	It reports the status of the operation.
*/
MMP_ERR CUSTOMER_PMU_Read_Reg(CUSTOMER_PMU_REG regNAME, MMP_USHORT *returnValue) 
{
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : CUSTOMER_PMU_Write_Reg
//  Description : This function is used for writing PMU chip's registers.
//------------------------------------------------------------------------------
/** @brief This function is used for writing PMU chip's registers.
@param[in] 	regNAME is main register.
@param[out] nValue is the value set the register.
@return 	It reports the status of the operation.
*/
MMP_ERR CUSTOMER_PMU_Write_Reg(CUSTOMER_PMU_REG regNAME, MMP_UBYTE nValue) 
{
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : CUSTOMER_PMU_WDT_CleanStatus
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to kick Watch dog one time.

The function is used to kick Watch dog one time.
@return It reports the status of the operation.
*/
MMP_ERR CUSTOMER_PMU_WDT_CleanStatus(void)
{
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : CUSTOMER_PMU_WDT_Reset
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to reset watch dog counter.

The function is used to reset watch dog counter.
@return It reports the status of the operation.
*/
MMP_ERR CUSTOMER_PMU_WDT_Reset(void) 
{
	return MMP_ERR_NONE;
}

#if 0
static void CUSTOMER_PMU_WDT_TimerCallBack(OS_TMR *ptmr, void *p_arg)
{
	MMPF_OS_PutMessage(LTask_QId, (void *)(&CUSTOMER_PMU_WDT_CleanStatus));
}
#endif

//------------------------------------------------------------------------------
//  Function    : CUSTOMER_PMU_WDT_Enable
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to enable/disable CUSTOMER_PMU watch dog.

The function is used to enable/disable CUSTOMER_PMU watch dog.
@param[in] bEnable : enable/diable watch dog.
@return It reports the status of the operation.
*/
MMP_ERR CUSTOMER_PMU_WDT_Enable(MMP_BOOL bEnable) 
{
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : CUSTOMER_PMU_WDT_SetTimeOut
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to enable/disable CUSTOMER_PMU watch dog.

The function is used to enable/disable CUSTOMER_PMU watch dog.
@param[in] ulMilleSec : Time out seting. The unit is "ms".
@return It reports the status of the operation.
*/
MMP_ERR CUSTOMER_PMU_WDT_SetTimeOut(MMP_ULONG ulMilleSec) 
{
	return MMP_ERR_NONE;
}

#if 0
static void CUSTOMER_PMU_BOOT_TimerISR(void)
{

}

MMP_ERR CUSTOMER_PMU_ActiveDischarge_En(MMP_BOOL bEnable)
{
	return MMP_ERR_NONE;
}
#endif

//------------------------------------------------------------------------------
//  Function    : CUSTOMER_PMU_PowerOff
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to power off PMU.

The function is used to power off PMU.
@return It reports the status of the operation.
*/
MMP_ERR CUSTOMER_PMU_PowerOff(void)
{
#if defined(POWER_EN_GPIO) && defined(POWER_EN_GPIO_LEVEL)
	/* Power Pin must be pulled high when System Power On */
	if (POWER_EN_GPIO != MMP_GPIO_MAX)
	{
		MMPF_PIO_EnableOutputMode(POWER_EN_GPIO, MMP_TRUE, MMP_TRUE);
		MMPF_PIO_SetData(POWER_EN_GPIO, !POWER_EN_GPIO_LEVEL, MMP_TRUE);
	}
#else
	#error "Power Enable Pin, POWER_EN_GPIO must be defined!"
#endif

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : CUSTOMER_PMU_EnableLDO
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to turn on/off PMU LDO.

The function is used to turn on/off PMU LDO.
@param[in] CUSTOMER_PMU_LDO_ID is the specific ID number.
@param[in] bEnable : MMP_TRUE to turn on and MMP_FALSE to turn off
@return It reports the status of the operation.
*/
#if 0
MMP_ERR CUSTOMER_PMU_EnableLDO(CUSTOMER_PMU_LDO_ID ldoID, MMP_BOOL bEnable)
{
	return MMP_ERR_NONE;
}
#endif

//------------------------------------------------------------------------------
//  Function    : CUSTOMER_PMU_EnableDCDC
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to turn on/off PMU DCDC.

The function is used to turn on/off PMU DCDC.
@param[in] CUSTOMER_PMU_DCDC_ID is the specific ID number.
@param[in] bEnable : MMP_TRUE to turn on and MMP_FALSE to turn off
@return It reports the status of the operation.
*/
#if 0
MMP_ERR CUSTOMER_PMU_EnableDCDC(CUSTOMER_PMU_DCDC_ID dcdcID, MMP_BOOL bEnable)
{
	return MMP_ERR_NONE;
}
#endif

//------------------------------------------------------------------------------
//  Function    : CUSTOMER_PMU_CheckBootComplete
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to check if system can initialize normally or not.

The function is used to check if system can initialize normally or not.
@param[in] ulMilliSecond : the time interval to check user press the power-key or not.
@param[in] pio_power_key : the PIO number, please reference the data structure of MMPD_PIO_REG
@return It reports the status of the operation.
*/
MMP_ERR CUSTOMER_PMU_CheckBootComplete(MMP_ULONG ulMilliSecond, MMP_GPIO_PIN pio_power_key) 
{
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : CUSTOMER_PMU_GetBootStatus
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to send back the boot-up events.

The function is used to send back the boot-up events.
@param[out] usRetStatus : the bit 0 stands for CUSTOMER_PMU_INT_EVENT_WAKE, and bit 4 stands for CUSTOMER_PMU_INT_EVENT_POWERK...etc., please refer CUSTOMER_PMU_INT_EVENT
@return It reports the status of the operation.
*/
MMP_ERR CUSTOMER_PMU_GetBootStatus(MMP_USHORT *usRetStatus)
{
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : CUSTOMER_PMU_RTC_Initialize
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to intial RTC driver.
The function is used to intial RTC driver.
@return It reports the status of the operation.
*/
MMP_ERR CUSTOMER_PMU_RTC_Initialize(void)
{
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : CUSTOMER_PMU_RTC_CheckTimeFormat
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used for checking the validation of Time format.

This function is used for checking the validation of Time format.
@param[in] pointer of structure AUTL_DATETIME.
@return It reports the status of the operation.
*/
#if 0
static MMP_ERR CUSTOMER_PMU_RTC_CheckTimeFormat(AUTL_DATETIME* ptr)
{
	return MMP_ERR_NONE;
}
#endif

//------------------------------------------------------------------------------
//  Function    : CUSTOMER_PMU_RTC_GetTime
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used for getting RTC information in CUSTOMER_PMU.

This function is used for getting RTC information in CUSTOMER_PMU.
@param[in] pointer of structure AUTL_DATETIME.
@return It reports the status of the operation.
*/
MMP_ERR CUSTOMER_PMU_RTC_GetTime(AUTL_DATETIME* ptr)
{
#if defined(EXT_RTC_DS1302_EN) && (EXT_RTC_DS1302_EN)
	return DS1302_RTC_GetTime(ptr);
#else
	RTNA_DBG_Str(0, "--W-- CUSTOMER_PMU_RTC_GetTime is TBD\r\n");

	ptr->usYear 		= 2014;
	ptr->usMonth 		= 3;
	ptr->usDay 			= 10;
	ptr->usDayInWeek 	= 1; 	// Sunday ~ Saturday
	ptr->usHour 		= 0;
	ptr->usMinute 		= 0;
	ptr->usSecond 		= 0;
	ptr->ubAmOrPm 		= 0; 	// am: 0, pm: 1, work only at b_12FormatEn = MMP_TURE
	ptr->b_12FormatEn 	= 0; 	// for set time, to indacate which format
								// 0 for 24 Hours, 1 for 12 Hours format

	return MMP_ERR_NONE;
#endif
}

//------------------------------------------------------------------------------
//  Function    : CUSTOMER_PMU_RTC_SetTime
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used for setting time information to RTC in CUSTOMER_PMU.

This function is used for setting time information to RTC in CUSTOMER_PMU.
@param[in] pointer of structure AUTL_DATETIME.
@return It reports the status of the operation.
*/
MMP_ERR CUSTOMER_PMU_RTC_SetTime(AUTL_DATETIME* ptr)
{
#if defined(EXT_RTC_DS1302_EN) && (EXT_RTC_DS1302_EN)
	return DS1302_RTC_SetTime(ptr);
#else
	RTNA_DBG_Str(0, "--W-- CUSTOMER_PMU_RTC_SetTime is TBD\r\n");

	return MMP_ERR_NONE;
#endif
}

//------------------------------------------------------------------------------
//  Function    : CUSTOMER_PMU_CLASSG_En
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used to enable/disable Class G module(AMP).
@param[in] bEnable is true for enable and flase for disable Cleass G module (AMP).
This function is used to enable/disable AMP.
@return It reports the status of the operation.
*/
MMP_ERR CUSTOMER_PMU_CLASSG_En(MMP_BOOL bEnable)
{
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : CUSTOMER_PMU_CLASSG_SetVol
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used to set out volume(in moudule class G ,AMP) as target value.
@param[in] vol_db stands for the volume gain output, 0 stands for mute(silent), 1~78 maps to -57~20 db.
This function is used to set out volume as target value.
@return It reports the status of the operation.
*/
MMP_ERR CUSTOMER_PMU_CLASSG_SetVol(MMP_UBYTE vol_db)
{
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : CUSTOMER_PMU_CheckECOVersion
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used to check if 506 ECO2.
@param[in] bReturnECOVersion is the return flag value, indicate which ECO version or not.
This function is used to check if 506 ECO.
@return It reports the status of the operation.
*/
MMP_ERR CUSTOMER_PMU_CheckECOVersion(MMP_UBYTE *bReturnECOVersion)
{
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : CUSTOMER_PMU_Initialize
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used to init CUSTOMER_PMU driver.

This function is used to init CUSTOMER_PMU driver.
@return It reports the status of the operation.
*/
MMP_ERR CUSTOMER_PMU_Initialize(void)
{
#if defined(POWER_EN_GPIO) && defined(POWER_EN_GPIO_LEVEL)
	/* Power Pin must be pulled high when System Power On */
	if (POWER_EN_GPIO != MMP_GPIO_MAX)
	{
	#if 0 // TBD
		AHC_ConfigGPIOPad(POWER_EN_GPIO, PAD_PULL_HIGH | PAD_E4_CURRENT);
		MMPF_PIO_EnableOutputMode(POWER_EN_GPIO, MMP_TRUE, MMP_TRUE);
		MMPF_PIO_SetData(POWER_EN_GPIO, POWER_EN_GPIO_LEVEL, MMP_TRUE);
	#endif
	}
#else
	#error "Power Enable Pin, POWER_EN_GPIO must be defined!"
#endif

#if defined(EXT_RTC_DS1302_EN) && (EXT_RTC_DS1302_EN)
	DS1302_RTC_Initialize();
#endif

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : CUSTOMER_PMU_ADC_Measure
//  Description :
//------------------------------------------------------------------------------
/** @brief The function return the measured voltage.
The function return the measured voltage.
@param[out] level is the return value of voltage (unit: mV)
@return It reports the status of the operation.
*/
MMP_ERR CUSTOMER_PMU_ADC_Measure(MMP_USHORT *level)
{
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : CUSTOMER_PMU_ADC_Measure_Key
//  Description :
//------------------------------------------------------------------------------
/** @brief The function return the measured voltage.
The function return the measured voltage.
@param[out] level is the return value of voltage (unit: mV)
@return It reports the status of the operation.
*/
MMP_ERR CUSTOMER_PMU_ADC_Measure_Key(MMP_ULONG *level, CUSTOMER_PMU_ADC_TYPE type)
{
#ifdef MCU_HT46R002
	*level = ReadMCU(CMD_ADCDATA);
#else
	RTNA_DBG_Str(0, "--W-- CUSTOMER_PMU_ADC_Measure_Key is TBD\r\n");
	*level = 4500;
#endif

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : CUSTOMER_PMU_Measure_Power_Key
//  Description :
//------------------------------------------------------------------------------
/** @brief The function return the measured voltage.
The function return the measured voltage.
@return It reports the status of the operation.
*/MMP_USHORT CUSTOMER_PMU_Measure_Power_Key(void)
{
#ifdef KEY_GPIO_POWER
	MMP_UBYTE keyStstus;

	if (KEY_GPIO_POWER != MMP_GPIO_MAX)
	{
#if 0 //TBD
	#if (KEY_PRESSLEVEL_POWER == LEVEL_HIGH)
		AHC_ConfigGPIOPad(KEY_GPIO_POWER, PAD_PULL_LOW | PAD_E4_CURRENT);
	#else
		AHC_ConfigGPIOPad(KEY_GPIO_POWER, PAD_PULL_HIGH | PAD_E4_CURRENT);
	#endif

		MMPF_PIO_EnableOutputMode(POWER_EN_GPIO, MMP_FALSE, MMP_TRUE);
		MMPF_PIO_GetData(POWER_EN_GPIO, &keyStstus);

		if (KEY_PRESSLEVEL_POWER == keyStstus)
		{
			return (MMP_USHORT) (1 << 4); // POWERK_INT
		}
#endif

		return 0;
	}
#else
	RTNA_DBG_Str(0, "--W-- CUSTOMER_PMU_Measure_Power_Key is TBD\r\n");
	return (1 << 4); // POWERK_INT
#endif
}

#endif // #if (CUSTOMER_PMU_EN)

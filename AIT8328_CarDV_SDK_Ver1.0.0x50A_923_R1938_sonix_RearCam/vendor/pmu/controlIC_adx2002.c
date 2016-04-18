#include "config_fw.h"
#include "controlIC_reg_adx2002.h"
#include "controlIC_adx2002.h"
#include "lib_retina.h"
#include "mmpf_i2cm.h"
#include "mmpf_rtc.h"
#include "os_wrap.h"
#include "mmpf_pio.h"
#include "mmpf_timer.h"
#include "mmpf_pll.h"

#if (ADX2002_EN)
extern MMPF_OS_MQID  	HighTask_QId;
//Attention: Please note the SLAVE_ADDR is different among software and HW I2CM
//HW I2CM
MMP_I2CM_ATTR gI2cmAttribute_adx2002 = {
	MMP_I2CM1,			//i2cmID
	ADX2002_SLAVE_ADDR, //ubSlaveAddr
	8, 					//ubRegLen
	8, 					//ubDataLen
	0x2, 				//ubDelayTime
	MMP_FALSE, 			//bDelayWaitEn
	MMP_FALSE, 			//bInputFilterEn
	MMP_FALSE, 			//b10BitModeEn
	MMP_FALSE, 			//bClkStretchEn
	0, 					//ubSlaveAddr1
	0, 					//ubDelayCycle
	6, 					//ubPadNum
	250000 /*250KHZ*/, 	//ulI2cmSpeed
	MMP_TRUE, 			//bOsProtectEn
	NULL, 				//sw_clk_pin
	NULL, 				//sw_data_pin
	MMP_FALSE,			//bRfclModeEn 
	MMP_FALSE,			//bWfclModeEn
	MMP_FALSE,			//bRepeatModeEn
	0                   //ubVifPioMdlId
};

AIT_PMU_MODE	 glPmuMode = ADX2002_PMU_V_2012E;
volatile MMP_BOOL m_alarmUse_A = MMP_FALSE, m_alarmUse_B = MMP_FALSE;
     
static MMPF_OS_SEMID  m_RtcAlamSemID;
static RTC_CallBackFunc *(m_RtcCallBack[ADX2002_ALARM_MAX - 1]);
static MMP_USHORT m_usTimerID[MMPF_OS_TMRID_MAX];
static MMP_USHORT m_usTimerReadIndex = 0, m_usTimerWriteIndex = 0;
static MMP_ULONG m_glAdx2002AccessTime = 0x20;		//unit: ms
//static MMP_ULONG m_glAdx2002CleanWdtTime = 3000;  //Timer interval to clean ADX2002 watch-dog , unit: ms

MMP_USHORT glAdx2002IntStatus = 0;
#if defined(MBOOT_FW)
static MMP_BOOL m_bBootTimeOut = MMP_FALSE;
#endif  

void ADX2002_DumpAll(void) 
{
	MMP_USHORT Register = 0x0, value = 0x0;
	RTNA_DBG_Str(0, "\r\n\r\n=====================================Register Dump Start\r\n");
	for(Register = 0; Register < 0x96; Register++) {
		value = 0x0;
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, Register, &value);
		RTNA_DBG_Str(0, "Register = ");
		RTNA_DBG_Short(0, Register);
		RTNA_DBG_Str(0, ",  ");
		RTNA_DBG_PrintShort(0,value); 
	}
	RTNA_DBG_Str(0, "\r\n========================================Register Dump End\r\n\r\n");
}

static void ADX2002_Timer_TaskMode(void)
{
	AITPS_GBL pGBL = AITC_BASE_GBL;
	
	if(MMPF_OS_StopTimer(m_usTimerID[m_usTimerReadIndex], OS_TMR_OPT_NONE) != 0) {
		RTNA_DBG_Str(0, "ADX2002 Stop OS Timer error !!\r\n");
	}
	m_usTimerID[m_usTimerReadIndex] = OS_TMR_OPT_NONE; //clean the timer id by writing one invalidate ID.
	m_usTimerReadIndex = ((m_usTimerReadIndex + 1) % MMPF_OS_TMRID_MAX);
	
	//Re-open the interrupt enable in AIT chip
	pGBL->GBL_POC_CPU_INT_EN = GBL_PMIC_INT_EN;	
}

static void ADX2002_IsrTimerCallBack(OS_TMR *ptmr, void *p_arg)
{
    ADX2002_Timer_TaskMode();
}


#if defined(MBOOT_FW)
static void ADX2002_BOOT_TimerISR(void)
{
	m_bBootTimeOut = MMP_TRUE;
}
#endif


#if ((ADX2002_EN)&&(ADX2002_WDT_EN == 0x1))
extern MMPF_OS_MQID  	LTask_QId;

//------------------------------------------------------------------------------
//  Function    : ADX2002_WDT_CleanStatus
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to kick Watch dog one time.

The function is used to kick Watch dog one time.
@return It reports the status of the operation.
*/
MMP_ERR ADX2002_WDT_CleanStatus(void) {
	return MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_CHIP_INT_STATUS, 0x1);
}

//------------------------------------------------------------------------------
//  Function    : s
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to reset watch dog counter.

The function is used to reset watch dog counter.
@return It reports the status of the operation.
*/
MMP_ERR ADX2002_WDT_Reset(void) {
	MMP_ERR status = MMP_ERR_NONE;
	MMP_USHORT value = 0x0;
	MMP_ULONG ul1msCounter = 0x48A;  //96 MHz
	
	status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_WDT_CTL, &value);
	status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_WDT_CTL, (value&0x7F));  //Turn-off
	MMPF_PLL_WaitCount(ul1msCounter*1000);
	status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_WDT_CTL, (value | 0x80)); //Turn-on
	MMPF_PLL_WaitCount(ul1msCounter*1000);
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	
	return MMP_ERR_NONE; 
}
/*static void ADX2002_WDT_TimerCallBack(OS_TMR *ptmr, void *p_arg)
{
	MMPF_OS_PutMessage(LTask_QId, (void *)(&ADX2002_WDT_CleanStatus));
}*/


//------------------------------------------------------------------------------
//  Function    : ADX2002_WDT_Reset
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to enable/disable ADX2002 watch dog.

The function is used to enable/disable ADX2002 watch dog.
@param[in] bEnable : enable/diable watch dog.
@return It reports the status of the operation.
*/
MMP_ERR ADX2002_WDT_Enable(MMP_BOOL bEnable) {
	MMP_ERR status = MMP_ERR_NONE;
	MMP_USHORT value = 0;
	
	status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_WDT_CTL, &value);
	RTNA_DBG_PrintLong(0, value);
	
	if(bEnable) {
		status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_WDT_CTL, (value|0x80));
	}
	else {
		status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_WDT_CTL, (value&0x7F));
	}
	
	#if 0
	status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_WDT_CTL, &value);
	RTNA_DBG_PrintLong(0, value);
	#endif
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return MMP_ERR_NONE; 
}

//------------------------------------------------------------------------------
//  Function    : ADX2002_WDT_SetTimeOut
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to enable/disable ADX2002 watch dog.

The function is used to enable/disable ADX2002 watch dog.
@param[in] ulMilleSec : Time out seting. The unit is "ms".
@return It reports the status of the operation.
*/
MMP_ERR ADX2002_WDT_SetTimeOut(MMP_ULONG ulMilleSec) {
	MMP_ERR status = MMP_ERR_NONE;
	MMP_USHORT value = 0;
	
	RTNA_DBG_Str(ADX2002_DEBUG_LEVEL, "ADX2002_WDT_SetTimeOut.........\r\n");
	status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_WDT_CTL, &value);
	switch (ulMilleSec) {
		case 3000:
			#if defined(ALL_FW)
			status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_WDT_CTL, (value | 0x40));
			#endif
			break;
		case 10000:
			status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_WDT_CTL, (value & 0xB0));
			//MMPF_OS_Sleep(1000);
			break;
		default:
			RTNA_DBG_Str(ADX2002_DEBUG_LEVEL, "Un-supported WDT timeout\r\n");
			break;
	}
	#if 0 //debug
	status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_WDT_CTL, &value);
	RTNA_DBG_PrintLong(0, value);
	#endif
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return MMP_ERR_NONE; 
}

//------------------------------------------------------------------------------
//  Function    : ADX2002_WDT_Kick
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to reset watch dog counter.

 The function is used to reset watch dog counter.
@param[in] void :
@return It reports the status of the operation.
*/
MMP_ERR ADX2002_WDT_Kick(void) {
	MMP_USHORT value = 0;
	MMP_ERR status = MMP_ERR_NONE;
	
	if (glPmuMode == ADX2002_PMU_V_2002) {
		status |= ADX2002_WDT_CleanStatus();
	}
	else if ((glPmuMode == ADX2002_PMU_V_2012E)||(glPmuMode == ADX2002_PMU_V_2012)) {
		status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_WDT_CTL, &value);
		status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_WDT_CTL, (value | 0x20));
	}
	
	//MMPF_PLL_WaitCount(ul1msCounter*1000);
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return MMP_ERR_NONE; 
}

#endif



//------------------------------------------------------------------------------
//  Function    : ADX2002_PowerOff
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to power off PMU.

The function is used to power off PMU.
@return It reports the status of the operation.
*/
MMP_ERR ADX2002_PowerOff(void)
{
	MMP_ERR status = MMP_ERR_NONE;
	MMP_USHORT value = 0x0;
	
	status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_LDO_DISCHARGE_EN, 0x0F);//Vin@0602: Active Dicharge
	status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_3_CTL2, 0xE8);
	status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_2_CTL2, 0xD4);
	status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL6, 0xCB);//Vin@0602: DC1 Active Discharge
	if (glPmuMode == ADX2002_PMU_V_2002) {
		status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_POWERON_CTL, 0x83);
		status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_BAT_CTL2, 0x00);
	}
	else if (glPmuMode == ADX2002_PMU_V_2012){
		status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_BAT_CTL2, 0x01);  //Set No CLK

		value = 0x0;
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_BAT_CTL2, &value);
		while(value != 0x1) {
			MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_BAT_CTL2, 0x01);
			MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_BAT_CTL2, &value);
		}
	}
	else if(glPmuMode == ADX2002_PMU_V_2012E) {
		status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_BAT_CTL2, 0x00);

		value = 0x0;
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_BAT_CTL2, &value);
		while(value != 0x0) {
			MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_BAT_CTL2, 0x00);
			MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_BAT_CTL2, &value);
		}
	}
	
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return status;
}

//------------------------------------------------------------------------------
//  Function    : ADX2002_EnableLDO
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to turn on/off PMU LDO.

The function is used to turn on/off PMU LDO.
@param[in] ADX2002_LDO_ID is the specific ID number.
@param[in] bEnable : MMP_TRUE to turn on and MMP_FALSE to turn off
@return It reports the status of the operation.
*/
MMP_ERR ADX2002_EnableLDO(ADX2002_LDO_ID ldoID, MMP_BOOL bEnable)
{
	MMP_ERR status = MMP_ERR_NONE;
	MMP_USHORT usValue = 0;
	status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, (ADX2002_LDO_1_CTL + ldoID), &usValue);
	if (bEnable) {
		usValue |= 0x80;  
	}
	else {
		usValue &= 0x7F;
	}
	status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, (ADX2002_LDO_1_CTL + ldoID), usValue);
	
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return status;
}

//------------------------------------------------------------------------------
//  Function    : ADX2002_EnableDCDC
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to turn on/off PMU DCDC.

The function is used to turn on/off PMU DCDC.
@param[in] ADX2002_DCDC_ID is the specific ID number.
@param[in] bEnable : MMP_TRUE to turn on and MMP_FALSE to turn off
@return It reports the status of the operation.
*/
MMP_ERR ADX2002_EnableDCDC(ADX2002_DCDC_ID dcdcID, MMP_BOOL bEnable)
{
	MMP_ERR status = MMP_ERR_NONE;
	MMP_USHORT usValue = 0;
	status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, (ADX2002_DCDC_1_CTL2 + dcdcID*ADX2002_DCDC_REG_SIZE), &usValue);
	
	if (bEnable) {
		usValue |= 0x40;  
	}
	else {
		usValue &= ~(0x40);  
	}
	
	status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, (ADX2002_DCDC_1_CTL2 + dcdcID*ADX2002_DCDC_REG_SIZE), usValue);
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return status;
}

//------------------------------------------------------------------------------
//  Function    : ADX2002_CheckBootComplete
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to check if system can initialize normally or not.

The function is used to check if system can initialize normally or not.
@param[in] ulMilliSecond : the time interval to check user press the power-key or not.
@param[in] pio_power_key : the PIO number, please reference the data structure of MMP_GPIO_PIN
@return It reports the status of the operation.
*/

MMP_ERR ADX2002_CheckBootComplete(MMP_ULONG ulMilliSecond, MMP_GPIO_PIN pio_power_key) {
	MMP_ERR status = MMP_ERR_NONE;	
	#if defined(MBOOT_FW)
	MMP_UBYTE ubPowerKeyStatus = 0x1;
	MMP_USHORT value = 0x0;
	MMPF_TIMER_ATTR TimerAttribute;
	
	#if (ADX2002_WDT_EN == 0x1)
	ADX2002_WDT_Enable(MMP_FALSE);
	#endif
	if((glAdx2002IntStatus & (1 << ADX2002_INT_EVENT_POWERK)) != 0) {
		
		status |= MMPF_PIO_EnableOutputMode(pio_power_key, MMP_FALSE, MMP_TRUE);
    	TimerAttribute.TimePrecision = MMPF_TIMER_MILLI_SEC;
    	TimerAttribute.ulTimeUnits   = ulMilliSecond;
    	TimerAttribute.Func          = ADX2002_BOOT_TimerISR;
    	TimerAttribute.bIntMode      = MMPF_TIMER_PERIODIC;
    	status |= MMPF_Timer_Start(MMPF_TIMER_1, &TimerAttribute);
		while((!m_bBootTimeOut)&&(ubPowerKeyStatus == 1)) {
			status |= MMPF_PIO_GetData(pio_power_key, &ubPowerKeyStatus);
		}
		
		MMPF_Timer_Stop(MMPF_TIMER_1);
    	
		if (ubPowerKeyStatus) {
			RTNA_DBG_Str(ADX2002_DEBUG_LEVEL, "\r\nBoot Success!!\r\n");
			
			#if defined(MBOOT_FW)
			//PMP_POWER_OFF,201104
			//Turn-off Discharge
			status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_LDO_DISCHARGE_EN, 0x0);
			
			MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_3_CTL2, &value);
			status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_3_CTL2, (value & 0x7F));
			value = 0x0;
			MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_2_CTL2, &value);
			status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_2_CTL2, (value & 0x7F));
			value = 0x0;
			MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL2, &value);
			status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL2, (value & 0x7F));
			#endif
		}
		else {
			RTNA_DBG_Str(ADX2002_DEBUG_LEVEL, "\r\nBoot fail !!\r\n");
			
			#if defined(MBOOT_FW)
			//PMP_POWER_OFF,201104
			//ADX2002_WDT_Enable(MMP_TRUE);
			MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_CHIP_INT_STATUS, 0x1F); //Ben_20110526: Clear INT Status
			MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_CHIP_INT_EN, 0x0);
			ADX2002_LDO_SetVoltage(ADX2002_LDO_ID_1, 0x0);
			ADX2002_LDO_SetVoltage(ADX2002_LDO_ID_2, 0x0);
			ADX2002_LDO_SetVoltage(ADX2002_LDO_ID_3, 0x0);
			ADX2002_LDO_SetVoltage(ADX2002_LDO_ID_4, 0x0);
			if ((glPmuMode == ADX2002_PMU_V_2002) || (glPmuMode == ADX2002_PMU_V_2012)) {//Vin@0602: From Ben
			ADX2002_LDO_SetVoltage(ADX2002_LDO_ID_5, 0x0);
			}
			ADX2002_EnableLDO(ADX2002_LDO_ID_1, MMP_FALSE);
			ADX2002_EnableLDO(ADX2002_LDO_ID_2, MMP_FALSE);
			ADX2002_EnableLDO(ADX2002_LDO_ID_3, MMP_FALSE);
			ADX2002_EnableLDO(ADX2002_LDO_ID_4, MMP_FALSE);
			if ((glPmuMode == ADX2002_PMU_V_2002) || (glPmuMode == ADX2002_PMU_V_2012)) {//Vin@0602: From Ben
			ADX2002_EnableLDO(ADX2002_LDO_ID_5, MMP_FALSE);
			}
			
			if (glPmuMode == ADX2002_PMU_V_2002) {
				status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_POWERON_CTL, 0x83);
				status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_BAT_CTL2, 0x00);
			}
			else if (glPmuMode == ADX2002_PMU_V_2012) {
			status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_BAT_CTL2, 0x01);

				value = 0x0;
				MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_BAT_CTL2, &value);
				while(value != 0x1) {
					MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_BAT_CTL2, 0x01);
					MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_BAT_CTL2, &value);
				}
			}
			else if (glPmuMode == ADX2002_PMU_V_2012E) {
				status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_BAT_CTL2, 0x00);

				
				value = 0x0;
				MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_BAT_CTL2, &value);
				while(value != 0x0) {
					MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_BAT_CTL2, 0x00);
					MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_BAT_CTL2, &value);
			}

			#endif
			while(1)
			{
				RTNA_DBG_Str(0, "Power not off\r\n");
				RTNA_WAIT_MS(100);
			}
		}
		}
		
		//Note: In MBOOT_FW, we should not clean the status, because customer may ask power-on status in ALL_FW
	}
	else {
		#if defined(MBOOT_FW)
		//Turn-off Discharge
		status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_LDO_DISCHARGE_EN, 0x0);
			
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_3_CTL2, &value);
		status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_3_CTL2, (value & 0x7F));
		value = 0x0;
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_2_CTL2, &value);
		status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_2_CTL2, (value & 0x7F));
		value = 0x0;
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL2, &value);
		status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL2, (value & 0x7F));
		#endif
	
	}
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	#endif //#if defined(MBOOT_FW)
	return status;
}

//------------------------------------------------------------------------------
//  Function    : ADX2002_CheckBootComplete
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to send back the boot-up events

The function is used to send back the boot-up events.
@param[out] usRetStatus : the bit 0 stands for ADX2002_INT_EVENT_WAKE, and bit 4 stands for ADX2002_INT_EVENT_POWERK...etc., please refer ADX2002_INT_EVENT
@return It reports the status of the operation.
*/
MMP_ERR ADX2002_GetBootStatus(MMP_USHORT * usRetStatus)
{
	*usRetStatus = glAdx2002IntStatus;
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : ADX2002_RTC_Initialize
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to intial RTC driver
The function is used to intial RTC driver.
@return It reports the status of the operation.
*/
MMP_ERR ADX2002_RTC_Initialize(void)
{
	static MMP_BOOL bRtcInitFlag = MMP_FALSE;
	MMP_ERR status = MMP_ERR_NONE;
	MMP_ULONG i = 0;
	if (bRtcInitFlag == MMP_FALSE) {
		m_RtcAlamSemID =  MMPF_OS_CreateSem(1);
		status |= MMPF_PIO_Initialize();
		for (i = 0; i < (ADX2002_ALARM_MAX - 1); i++) {
			m_RtcCallBack[i] = NULL;
		}
		bRtcInitFlag = MMP_TRUE;
	}
	
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return status;
}

//------------------------------------------------------------------------------
//  Function    : ADX2002_RTC_CheckTimeFormat
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used for checking the validation of Time format.

This function is used for checking the validation of Time format.
@param[in] pointer of structure AUTL_DATETIME.
@return It reports the status of the operation.
*/
static MMP_ERR ADX2002_RTC_CheckTimeFormat(AUTL_DATETIME *ptr)
{
	MMP_ULONG ulDays[13]={0,31,0,31,30,31,30,31,31,30,31,30,31};
	if ((ptr->usYear < ADX2002_RTC_START_YEAR) || (ptr->usYear > ADX2002_RTC_MAX_YEAR)) {
		RTNA_DBG_Str(ADX2002_DEBUG_LEVEL, "Time Year\r\n");
		return MMP_SYSTEM_ERR_FORMAT;
	}
	
	if (((ptr->usYear%4 == 0)&&(ptr->usYear%100 != 0)) || (ptr->usYear%400 == 0)) {
		ulDays[2] = 29;	
	}
	else {
		ulDays[2] = 28;	
	}
	
	if((ptr->usMonth > 12) || (ptr->usMonth == 0) || (ptr->usDay > ulDays[ptr->usMonth]) || (ptr->usDay == 0)) {
		RTNA_DBG_Str(ADX2002_DEBUG_LEVEL, "Time Month day\r\n");
		return MMP_SYSTEM_ERR_FORMAT;
	}
	
	
	if((ptr->usHour > 23) || (ptr->usMinute > 59) || (ptr->usSecond > 59)) {
		RTNA_DBG_Str(ADX2002_DEBUG_LEVEL, "Time Month ubHour Minute Second\r\n");
		return MMP_SYSTEM_ERR_FORMAT;
	}
	
	if((ptr->b_12FormatEn == MMP_TRUE)&&(ptr->usHour > 12)) {
		RTNA_DBG_Str(ADX2002_DEBUG_LEVEL, "Time 12 Hours\r\n");
		return MMP_SYSTEM_ERR_FORMAT;
	}
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : ADX2002_RTC_GetTime
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used for getting RTC information in ADX2002.

This function is used for getting RTC information in ADX2002.
@param[in] pointer of structure AUTL_DATETIME.
@return It reports the status of the operation.
*/
MMP_ERR ADX2002_RTC_GetTime(AUTL_DATETIME *ptr)
{
	MMP_USHORT value = 0;
	MMP_ERR	status = MMP_ERR_NONE;
	
	status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_RTC_DAY, &value);
	ptr->usDayInWeek = value & 0x7;
	 
	status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_RTC_YEAR, &value);
	ptr->usYear = (value >> 4)*10 + (value & ADX2002_RTC_LMASK) + ADX2002_RTC_START_YEAR;
	
	status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_RTC_MON, &value);
	ptr->usMonth = ((value & 0x10) >> 4)*10 +  (value & ADX2002_RTC_LMASK);
	
	status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_RTC_DATE, &value);
	ptr->usDay = ((value & 0x30) >> 4)*10 +  (value & ADX2002_RTC_LMASK);
	
	status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_RTC_HOUR, &value);
	if (value & 0x80) {  //12-hour format
		ptr->usHour = (((value & 0x10) >> 4)*10 + (value & ADX2002_RTC_LMASK))%12;
		if(value & 0x20) { //Check if the time at P.M.
			ptr->ubAmOrPm = 0x1;
		}
	}
	else {	//24-hour format
		ptr->usHour = ((value & 0x30) >> 4)*10 + (value & ADX2002_RTC_LMASK);
	}
	
	status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_RTC_MIN, &value);
	ptr->usMinute = ((value & 0x70) >> 4)*10 +  (value & ADX2002_RTC_LMASK);
	
	status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_RTC_SEC, &value);
	ptr->usSecond = ((value & 0x70) >> 4)*10 +  (value & ADX2002_RTC_LMASK);
	
	
	#if 0
	RTNA_DBG_Str(ADX2002_DEBUG_LEVEL, "\r\n---------Read Time---------\r\n");
	RTNA_DBG_PrintLong(0, ptr->usYear);
	RTNA_DBG_PrintLong(0, ptr->usMonth);
	RTNA_DBG_PrintLong(0, ptr->usDay);
	if(ptr->ubAmOrPm == 0x1) {
		RTNA_DBG_Str(0, "PM :");
	}
	else {
		RTNA_DBG_Str(0, "AM :");
	}
	RTNA_DBG_PrintLong(0, ptr->usHour);
	RTNA_DBG_PrintLong(0, ptr->usMinute);
	RTNA_DBG_PrintLong(0, ptr->usSecond);
	RTNA_DBG_Str(0, "\r\n");
	#endif
	
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : ADX2002_RTC_SetTime
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used for setting time information to RTC in ADX2002.

This function is used for setting time information to RTC in ADX2002.
@param[in] pointer of structure AUTL_DATETIME.
@return It reports the status of the operation.
*/
MMP_ERR ADX2002_RTC_SetTime(AUTL_DATETIME *ptr)
{
	MMP_ERR status;
	MMP_USHORT value = 0;
	status = ADX2002_RTC_CheckTimeFormat(ptr);
	if ( status != MMP_ERR_NONE) {
		return status;
	}
	
	value =  (((ptr->usYear - ADX2002_RTC_START_YEAR)/10) << 4)  + ((ptr->usYear - ADX2002_RTC_START_YEAR)%10);
	status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_YEAR, value);
	
	value = (((ptr->usMonth/10) << 4)&0x10) + (ptr->usMonth%10);
	status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_MON, value);
	
	value = (((ptr->usDay/10) << 4)&0x30) + (ptr->usDay%10);
	status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_DATE, value);
	
	if (ptr->usHour >= 10) {
		value = ((ptr->usHour/10) << 4) + (ptr->usHour % 10);
	}
	else {
		value = ptr->usHour;
	}
	
	if (ptr->b_12FormatEn == MMP_TRUE) {
		value = 0x80 | value;
		if(ptr->ubAmOrPm == 0x1) {//set as P.M.
			value = 0x20 | value;
		}
	}
	status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_HOUR, value);
	
	value = (((ptr->usMinute/10) << 4)&0x70) + (ptr->usMinute%10);
	status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_MIN, value);
	
	value = (((ptr->usSecond/10) << 4)&0x70) + (ptr->usSecond%10);
	status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_SEC, value);
	
	#if 0
	RTNA_DBG_Str(0, "---------Set Time---------\r\n\r\n");
	RTNA_DBG_PrintLong(0, ptr->usYear);
	RTNA_DBG_PrintLong(0, ptr->usMonth);
	RTNA_DBG_PrintLong(0, ptr->usDay);
	RTNA_DBG_PrintLong(0, ptr->usHour);
	RTNA_DBG_PrintLong(0, ptr->usMinute);
	RTNA_DBG_PrintLong(0, ptr->usSecond);
	RTNA_DBG_PrintLong(0, ptr->usDayInWeek);
	#endif
	
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return status;
}


//------------------------------------------------------------------------------
//  Function    : ADX2002_RTC_AlarmCheckID
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used for check which alarm happens when we turn on 2 alarms.

This function is used for check which alarm happens when we turn on 2 alarms.
@param[out] alarm_id : the alarm ID which alarm expires.
@return It reports the status of the operation.
*/
static MMP_ERR ADX2002_RTC_AlarmCheckID(ADX2002_ALARM_ID* alarm_id)
{
	AUTL_DATETIME current_time, alarm_time;
	MMP_ERR status = MMP_ERR_NONE;
	status |= ADX2002_RTC_GetTime(&current_time);
	
	//Read Alarm1 settings first
	status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_RTC_ALARM1_H, (MMP_USHORT*)&(alarm_time.usHour));
	status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_RTC_ALARM1_DATE, (MMP_USHORT*)&(alarm_time.usDay));
	status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_RTC_ALARM1_M, (MMP_USHORT*)&(alarm_time.usMinute));
	
	//Because of ADX2002 alarm is "minute" level, 
	//so we assume that we can make sure that we can check alarm at "this minute"
	if((alarm_time.usDay == current_time.usDay)&&(alarm_time.usHour == current_time.usHour)&&(alarm_time.usMinute == current_time.usMinute)) {
		*alarm_id = ADX2002_ALARM_ID_A; 
	}
	else {
		status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_RTC_ALARM1_H, (MMP_USHORT*)&(alarm_time.usHour));
		status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_RTC_ALARM1_DATE, (MMP_USHORT*)&(alarm_time.usDay));
		status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_RTC_ALARM1_M, (MMP_USHORT*)&(alarm_time.usMinute));
		if((alarm_time.usDay == current_time.usDay)&&(alarm_time.usHour == current_time.usHour)&&(alarm_time.usMinute == current_time.usMinute)) {
			*alarm_id = ADX2002_ALARM_ID_B; 
		}
		else {
			*alarm_id = ADX2002_ALARM_ID_NONE;
		}
	}
	
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return status;
}

//------------------------------------------------------------------------------
//  Function    : ADX2002_RTC_SetAlarmEnable
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used to enable RTC alarm.

This function is used to enable RTC alarm.
@param[in] alarm_id : select which alarm to operate.
@param[in] bEnable : turn on or turn off.
@param[in] ptr : alarm time settings.
@param[in] p_callback : call-back function when alarm time expired.
@return It reports the status of the operation.
*/
MMP_ERR ADX2002_RTC_SetAlarmEnable(ADX2002_ALARM_ID alarm_id, MMP_BOOL bEnable, AUTL_DATETIME *ptr, void* p_callback)
{
	MMP_USHORT adx2002_value = 0;
	AITPS_GBL pGBL = AITC_BASE_GBL;
	MMP_ULONG ulDates[13]={0,31,0,31,30,31,30,31,31,30,31,30,31};
	MMP_UBYTE ubRet = 0xFF;
	MMP_ERR status = MMP_ERR_NONE;
	
	//MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_CHIP_INT_EN, 0x1E);
	if (bEnable) {
		
		//Alarm time format checking
		if (((ptr->usYear%4 == 0)&&(ptr->usYear%100 != 0)) || (ptr->usYear%400 == 0)) {
				ulDates[2] = 29; //February days check	
		}
		else {
				ulDates[2] = 28;	//February days check
		}
		
		if((ptr->usDay > ulDates[ptr->usMonth]) || (ptr->usDay == 0)) {
			return MMP_SYSTEM_ERR_FORMAT;
		}
		
		if((ptr->usHour > 23) || (ptr->usMinute > 59)) {
			return MMP_SYSTEM_ERR_FORMAT;
		}
		
		if((ptr->ubAmOrPm == 0x1)&& (ptr->b_12FormatEn == 0x1)) {  //12-hours format check
			if((ptr->usHour > 12)) {
				return MMP_SYSTEM_ERR_FORMAT;
			}
		}
	
		ubRet = MMPF_OS_AcquireSem(m_RtcAlamSemID, ADX2002_SEM_TIMEOUT);
		if (alarm_id == ADX2002_ALARM_ID_A){
			if (m_alarmUse_A == MMP_FALSE) {
				adx2002_value = (((ptr->usDay/10) << 4)&0x30) + (ptr->usDay%10);
				status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_ALARM1_DATE, adx2002_value);

                status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_RTC_HOUR, &adx2002_value);
                if (adx2002_value & 0x80) { //RTC in 12-hour format
                    if (ptr->b_12FormatEn == 0x1) { //input in 12-hour format
                        adx2002_value = ((ptr->usHour/10) << 4) + (ptr->usHour % 10);
                        if (ptr->ubAmOrPm == 0x1)
                            adx2002_value |= 0x20; // PM
                    }
                    else { //input in 24-hour format
                        adx2002_value = (ptr->usHour >= 12) ? 0x20 : 0; //set PM or AM
                        ptr->usHour = (ptr->usHour > 12) ? ptr->usHour - 12 : ptr->usHour;
                        adx2002_value |= (((ptr->usHour/10) << 4) + (ptr->usHour % 10));
                    }
                }
				else { //RTC in 24-hour format
				    if (ptr->b_12FormatEn == 0x1) { //input in 12-hour format
				        if ((ptr->ubAmOrPm == 0x1) && (ptr->usHour != 12))
				            ptr->usHour += 12;
                    }
                    adx2002_value = ((ptr->usHour/10) << 4) + (ptr->usHour % 10);
                }
				status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_ALARM1_H, adx2002_value);
				
				adx2002_value = 0;
				adx2002_value = (((ptr->usMinute/10) << 4)&0x70) + (ptr->usMinute%10);
				status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_ALARM1_M, adx2002_value);
				
				adx2002_value = 0;
				//adx2002_value = 1 << (ptr->usDay - 1);  //for ADX2002 alarm, Day and Date settings can "only" use one of them !!
				adx2002_value |= 0x80;  //Alarm on
				
				status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_ALARM1_DAY, adx2002_value);
				m_alarmUse_A = MMP_TRUE;
			}
			else {
				if (ubRet == 0) {
					MMPF_OS_ReleaseSem(m_RtcAlamSemID);
				}
				
				#if 0
				ADX2002_RTC_SetAlarmEnable(ADX2002_ALARM_ID_A, MMP_FALSE, NULL, NULL);
				ADX2002_RTC_SetAlarmEnable(ADX2002_ALARM_ID_A, MMP_TRUE, ptr, p_callback);
				return MMP_ERR_NONE;
				#else
				return MMP_SYSTEM_ERR_DOUBLE_SET_ALARM;
				#endif
			}
			
		}
		else if (alarm_id == ADX2002_ALARM_ID_B) {
			if(m_alarmUse_B == MMP_FALSE) {
				adx2002_value = 0;
				adx2002_value = (((ptr->usDay/10) << 4)&0x30) + (ptr->usDay%10);
				status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_ALARM2_DATE, adx2002_value);
				
				status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_RTC_HOUR, &adx2002_value);
                if (adx2002_value & 0x80) { //RTC in 12-hour format
                    if (ptr->b_12FormatEn == 0x1) { //input in 12-hour format
                        adx2002_value = ((ptr->usHour/10) << 4) + (ptr->usHour % 10);
                        if (ptr->ubAmOrPm == 0x1)
                            adx2002_value |= 0x20; // PM
                    }
                    else { //input in 24-hour format
                        adx2002_value = (ptr->usHour >= 12) ? 0x20 : 0; //set PM or AM
                        ptr->usHour = (ptr->usHour > 12) ? ptr->usHour - 12 : ptr->usHour;
                        adx2002_value |= (((ptr->usHour/10) << 4) + (ptr->usHour % 10));
                    }
                }
				else { //RTC in 24-hour format
				    if (ptr->b_12FormatEn == 0x1) { //input in 12-hour format
				        if ((ptr->ubAmOrPm == 0x1) && (ptr->usHour != 12))
				            ptr->usHour += 12;
                    }
                    adx2002_value = ((ptr->usHour/10) << 4) + (ptr->usHour % 10);
                }
				status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_ALARM2_H, adx2002_value);
				
				adx2002_value = 0;
				adx2002_value = (((ptr->usMinute/10) << 4)&0x70) + (ptr->usMinute%10);
				status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_ALARM2_M, adx2002_value);
				
				adx2002_value = 0;
				//adx2002_value = 1 << (ptr->usDay - 1); //for ADX2002 alarm, Day and Date settings can "only" use one of them !!
				adx2002_value |= 0x80;  //Alarm on
				status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_ALARM2_DAY, adx2002_value);
				
				m_alarmUse_B = MMP_TRUE;
			}
			else {			
				if (ubRet == 0) {
					MMPF_OS_ReleaseSem(m_RtcAlamSemID);
				}
				
				#if 0
				ADX2002_RTC_SetAlarmEnable(ADX2002_ALARM_ID_B, MMP_FALSE, NULL, NULL);
				ADX2002_RTC_SetAlarmEnable(ADX2002_ALARM_ID_B, MMP_TRUE, ptr, p_callback);
				return MMP_ERR_NONE;
				#else
				return MMP_SYSTEM_ERR_DOUBLE_SET_ALARM;
				#endif
			}
			
		}
		else {
			RTNA_DBG_Str(ADX2002_DEBUG_LEVEL, "Error RTC Alarm settings 1 !\r\n");
			if (ubRet == 0) {
				MMPF_OS_ReleaseSem(m_RtcAlamSemID);
			}
			return MMP_SYSTEM_ERR_FORMAT;
		}
		
		m_RtcCallBack[alarm_id - 1] = (RTC_CallBackFunc*)p_callback;
		status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_RTC_INT_CTL, &adx2002_value);	
		RTNA_DBG_PrintLong(0, adx2002_value);
		
		if(glPmuMode == ADX2002_PMU_V_2002) {
			adx2002_value |= (1 << (5 + alarm_id));
		}
		else if ((glPmuMode == ADX2002_PMU_V_2012)||(glPmuMode == ADX2002_PMU_V_2012E)){
			adx2002_value |= 0xC0;   //Always turn-on Alarm A & Alarm B output in ADX2012
		}
		
		//Enable Interrupt
		status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_INT_CTL, adx2002_value);
		
		/*if(m_alarmUse_A & m_alarmUse_B) {
			adx2002_value = 0xC0;
		}
		RTNA_DBG_PrintLong(0, (MMP_ULONG)ADX2002_RTC_INT_CTL);
		//adx2002_value = 0x40;
		RTNA_DBG_PrintLong(0, adx2002_value);
		*/
		
		
		
		 
		#if 0 //waiting time for RTC alarm settings to work.
		MMPF_OS_Sleep(7000);  //For previous ADX2002, we need some time to let ADX2002 read and done the settings. 7(s) is experiments value.
		#endif
		
		#if 0//debug
		MMPF_OS_Sleep(2000);
		status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_RTC_INT_CTL, &adx2002_value);
		RTNA_DBG_Str(0, "ADX2002_RTC_INT_CTL :");
		RTNA_DBG_Long(0, adx2002_value);
		RTNA_DBG_Str(0, "\r\n");
		
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_RTC_ALARM1_DATE, &adx2002_value);
		RTNA_DBG_Str(0, "ADX2002_RTC_ALARM1_DATE :");
		RTNA_DBG_Long(0, adx2002_value);
		RTNA_DBG_Str(0, "\r\n");
		
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_RTC_ALARM1_H, &adx2002_value);
		RTNA_DBG_Str(0, "ADX2002_RTC_ALARM1_H :");
		RTNA_DBG_Long(0, adx2002_value);
		RTNA_DBG_Str(0, "\r\n");
		
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_RTC_ALARM1_M, &adx2002_value);
		RTNA_DBG_Str(0, "ADX2002_RTC_ALARM1_M :");
		RTNA_DBG_Long(0, adx2002_value);
		RTNA_DBG_Str(0, "\r\n");
		
		
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_RTC_ALARM1_DAY, &adx2002_value);
		RTNA_DBG_Str(0, "ADX2002_RTC_ALARM1_DAY :");
		RTNA_DBG_Long(0, adx2002_value);
		RTNA_DBG_Str(0, "\r\n");
		#endif
		
		
		if (ubRet == 0) {
			MMPF_OS_ReleaseSem(m_RtcAlamSemID);
		}
		
		if (pGBL->GBL_POC_CPU_INT_EN != GBL_PMIC_INT_EN) {
			pGBL->GBL_POC_CPU_INT_EN = GBL_PMIC_INT_EN;
		}
	}
	else {
		ubRet = MMPF_OS_AcquireSem(m_RtcAlamSemID, ADX2002_SEM_TIMEOUT);
		
		if((m_alarmUse_A == MMP_TRUE)&&(m_alarmUse_B == MMP_TRUE)) {
			if (alarm_id == ADX2002_ALARM_ID_A) {
				RTNA_DBG_Str(ADX2002_DEBUG_LEVEL, "Turn-off alram A\r\n");
				status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_ALARM1_DAY, 0x0);
				if(glPmuMode == ADX2002_PMU_V_2002) {
					status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_INT_CTL, 0x80);  //keep Alarm B on
				}
				
			}
			else if (alarm_id == ADX2002_ALARM_ID_B) {
				RTNA_DBG_Str(ADX2002_DEBUG_LEVEL, "Turn-off alram B\r\n");
				status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_ALARM2_DAY, 0x0);
				if(glPmuMode == ADX2002_PMU_V_2002) {
					status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_INT_CTL, 0x40);	//keep Alarm A on
				}
			}
			else {
				RTNA_DBG_Str(ADX2002_DEBUG_LEVEL, "ADX2002 Alarm Error alarm_ID !!\r\n");
				if (ubRet == 0) {
					MMPF_OS_ReleaseSem(m_RtcAlamSemID);
				}
				return MMP_SYSTEM_ERR_FORMAT;
			}
		}
		else if(m_alarmUse_A == MMP_TRUE) {
			alarm_id = ADX2002_ALARM_ID_A;
			RTNA_DBG_Str(ADX2002_DEBUG_LEVEL, "---------Turn-off alram A\r\n");
			status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_ALARM1_DAY, 0x0);
			if(glPmuMode == ADX2002_PMU_V_2002) {
				status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_INT_CTL, 0x00);
			}
		}
		else if(m_alarmUse_B == MMP_TRUE) {
			alarm_id = ADX2002_ALARM_ID_B;
			RTNA_DBG_Str(ADX2002_DEBUG_LEVEL, "---------Turn-off alram B\r\n");
			status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_ALARM2_DAY, 0x0);
			if(glPmuMode == ADX2002_PMU_V_2002) {
				status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_RTC_INT_CTL, 0x00);
			}
		}
		else {
			if (ubRet == 0) {
				MMPF_OS_ReleaseSem(m_RtcAlamSemID);
			}
			return MMP_ERR_NONE;
		}
	
		if ((alarm_id == ADX2002_ALARM_ID_A) && (m_alarmUse_A == MMP_TRUE)) {
			m_alarmUse_A = MMP_FALSE;
		}
		else if ((alarm_id == ADX2002_ALARM_ID_B) && (m_alarmUse_B == MMP_TRUE)) {
			m_alarmUse_B = MMP_FALSE;
		}
		else {
			RTNA_DBG_Str(ADX2002_DEBUG_LEVEL, "Error RTC Alarm settings 2 !\r\n");
			if (ubRet == 0) {
				MMPF_OS_ReleaseSem(m_RtcAlamSemID);
			}
			return MMP_SYSTEM_ERR_FORMAT;
		}
		
		if (ubRet == 0) {
			MMPF_OS_ReleaseSem(m_RtcAlamSemID);
		}
	}
	
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return status;
}

//------------------------------------------------------------------------------
//  Function    : ADX2002_LDO_SetV
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used for setting LDO voltage.

This function is used for setting LDO voltage.
@param[in] ADX2002_LDO_ID is the specific ID number.
@param[in] vLevel is between 0x00~0x3F, one level stands 0.05v, the minimum voltage(level 0) is 0.6V
@return It reports the status of the operation.
*/
MMP_ERR ADX2002_LDO_SetVoltage(ADX2002_LDO_ID ldoID, MMP_USHORT vLevel){
	MMP_ERR status = MMP_ERR_NONE;
	vLevel = ADX2002_LDO_EN | (vLevel & ADX2002_LEVEL_BIT);
	status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, (ADX2002_LDO_1_CTL + ldoID), vLevel);
	
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return status;
}


//------------------------------------------------------------------------------
//  Function    : ADX2002_LDO_GetV
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used to get LDO voltage.

This function is used to get LDO voltage.
@param[in] ADX2002_LDO_ID is the specific ID number.
@param[in] returnVol is the return volage value, the unit is mV
@return It reports the status of the operation.
*/
MMP_ERR ADX2002_LDO_GetVoltage(ADX2002_LDO_ID ldoID, MMP_USHORT *returnVoltage){
	MMP_ERR status = MMP_ERR_NONE;
	MMP_USHORT voltage = 0;
	status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, (ADX2002_LDO_1_CTL + ldoID), &voltage);
	voltage &= ADX2002_LEVEL_BIT;
	*returnVoltage = 600 + voltage*50;
	
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return status;
}
//------------------------------------------------------------------------------
//  Function    : ADX2002_DCDC_SetV
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used for setting DCDC voltage.

This function is used for setting DCDC voltage.
@param[in] ADX2002_DCDC_ID is the specific ID number.
@param[in] vLevel is between 0x00~0x3F, one level stands 0.05v, the minimum voltage(level 0) is 0.8V
@return It reports the status of the operation.
*/
MMP_ERR ADX2002_DCDC_SetVoltage(ADX2002_DCDC_ID dcdcID, MMP_USHORT vLevel) {
	MMP_ERR status = MMP_ERR_NONE;
	vLevel = ADX2002_DCDC_BUCK1_EN | (vLevel & ADX2002_LEVEL_BIT);
	status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, (ADX2002_DCDC_1_CTL2 + dcdcID*ADX2002_DCDC_REG_SIZE), vLevel);
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return status;
}

//------------------------------------------------------------------------------
//  Function    : ADX2002_DCDC_GetVoltage
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used for Getting DCDC voltage.

This function is used for Getting DCDC voltage.
@param[in] ADX2002_DCDC_ID is the specific ID number.
@param[in] returnVol is the return volage value, the unit is mV
@return It reports the status of the operation.
*/
MMP_ERR ADX2002_DCDC_GetVoltage(ADX2002_DCDC_ID dcdcID, MMP_USHORT* returnVoltage) {
	MMP_USHORT voltage = 0;
	MMP_ERR status = MMP_ERR_NONE;
	status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, (ADX2002_DCDC_1_CTL2 + dcdcID*ADX2002_DCDC_REG_SIZE), &voltage);
	voltage &= ADX2002_LEVEL_BIT;
	*returnVoltage = 800 + voltage*50;
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : ADX2002_CLASSG_En
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used to enable/disable Class G module(AMP).
@param[in] bEnable is true for enable and flase for disable Cleass G module (AMP).
This function is used to enable/disable AMP.
@return It reports the status of the operation.
*/
MMP_ERR ADX2002_CLASSG_En(MMP_BOOL bEnable)
{
	MMP_ERR status = MMP_ERR_NONE;
	if(glPmuMode == ADX2002_PMU_V_2002) {
		if (bEnable == MMP_TRUE) {
			status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_CLASSG_HEAD_SET2, 0x0C);
			MMPF_OS_Sleep(100);
			status |= ADX2002_CLASSG_SetVol(0x3A);
		}
		else {
			status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_CLASSG_HEAD_SET2, 0x0F);
			status |= ADX2002_CLASSG_SetVol(0);
		}
		
		if(status != MMP_ERR_NONE) {
			return MMP_SYSTEM_ERR_PMU;
		}
	}
	return status;
}


//------------------------------------------------------------------------------
//  Function    : ADX2002_CLASSG_SetVol
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used to set out volume(in moudule class G ,AMP) as target value.
@param[in] vol_db stands for the volume gain output, 0 stands for mute(silent), 1~78 maps to -57~20 db.
This function is used to set out volume as target value.
@return It reports the status of the operation.
*/
MMP_ERR ADX2002_CLASSG_SetVol(MMP_UBYTE vol_db)
{
	MMP_USHORT currentVol = 0;
	MMP_ERR status = MMP_ERR_NONE;
	if(glPmuMode == ADX2002_PMU_V_2002) {
		if (vol_db <= ADX2002_CLASSG_MAX_VOL) {
			status |= MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_CLASSG_VOL_CTL1, &currentVol);
			if(currentVol != vol_db) {
				status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_CLASSG_VOL_CTL1, vol_db);
				status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_CLASSG_VOL_CTL2, vol_db);
			}
		}
		
		if(status != MMP_ERR_NONE) {
			return MMP_SYSTEM_ERR_PMU;
		}
	}
	return status;
}

//------------------------------------------------------------------------------
//  Function    : ADX2002_Initialize
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used to init ADX2002 driver.

This function is used to init ADX2002 driver.
@return It reports the status of the operation.
*/
MMP_ERR ADX2002_Initialize(void)
{
	MMP_ERR status = MMP_ERR_NONE;
	MMP_USHORT usVID = 0x0, usTempValue = 0x0;
	MMP_ULONG i = 0;
	static MMP_BOOL bAdx2002InitFlag = MMP_FALSE;
	
	if (bAdx2002InitFlag == MMP_FALSE) {
 
		AITPS_GBL pGBL = AITC_BASE_GBL;
		#if (ADX2002_WDT_EN == 0x1)
		//MMP_UBYTE ubTimerID = 0xFF;
		#endif
		
		for(i = 0 ; i < MMPF_OS_TMRID_MAX; i++) {
			m_usTimerID[i] = MMPF_OS_TMRID_MAX;  //initial ID  as  invalid timer id.
		}
		m_usTimerReadIndex = 0;
		m_usTimerWriteIndex = 0;
		
		status |= MMPF_I2cm_Initialize(&gI2cmAttr_ADX2002);
		status |= ADX2002_RTC_Initialize();
		
		#if defined(ALL_FW)&&(ADX2002_EN)&(ADX2002_WDT_EN)
		ADX2002_WDT_Enable(MMP_FALSE);
		#endif
		
		if(gbSystemCoreID == 0x32) {
			MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_CHIP_INT_EN, 0x1E);
		}
		
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_REVISON, &usVID);
		if (usVID == 0xFF) {
			glPmuMode = ADX2002_PMU_V_2002;
			m_glAdx2002AccessTime = 0x20;
		}
		else if(usVID == 0x02) {  //for new version ADX2002 (ADX2012)
			MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_REG_SRAM, &usTempValue);
			
			if((usTempValue == ADX2002_PMU_V_2012)||(usTempValue == ADX2002_PMU_V_2012E)) {
				glPmuMode = usTempValue;
				m_glAdx2002AccessTime = 0x5;
				
				RTNA_DBG_PrintLong(0, glPmuMode);
				RTNA_DBG_Str(0, "==========================\r\n");
			}
			else {
				RTNA_DBG_Str(0, "Error !! PMU Get version information error(2012)\r\n");
				glPmuMode = ADX2002_PMU_V_2012E;  //Default set as latest version
				m_glAdx2002AccessTime = 0x5;
			}
		}
		else {
			RTNA_DBG_Str(0, "Error !! PMU Get version information error\r\n");
			glPmuMode = ADX2002_PMU_V_2012E;  //Default set as latest version
			m_glAdx2002AccessTime = 0x5;
		}
		//Default turn off PMU interrupt in P_V2, if customer wish to use, please change 0 to 1
		//#if defined(ALL_FW)&&(HARDWARE_PLATFORM == SEC_PLATFORM_PV2)  
		  
		//At ALL_FW, we can get the boot interrupt status because in AitBootloader we do not clean interrupt status.
		/*if (glAdx2002IntStatus == 0) {
			MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_CHIP_INT_STATUS, &glAdx2002IntStatus);
			RTNA_DBG_PrintShort(0, glAdx2002IntStatus);
		}*/
		
		//Enable the chip get the interrupt signal
		pGBL->GBL_POC_CPU_INT_EN = GBL_PMIC_INT_EN; //for P_V2
		
		#if (ADX2002_WDT_EN == 0x1)
		//For customer's request, Watch dog is controled by UI
		/*status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_WDT_CTL, 0x84);
		ubTimerID = MMPF_OS_StartTimer(m_glAdx2002CleanWdtTime, MMPF_OS_TMR_OPT_PERIODIC, (MMPF_OS_TMR_CALLBACK)&ADX2002_WDT_TimerCallBack, NULL);
		if (ubTimerID >= MMPF_OS_TMRID_MAX) {
			status |= MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_WDT_CTL, 0x04);
		}*/
		#endif
		//#endif //(HARDWARE_PLATFORM == SEC_PLATFORM_PV2) 
		
		if(status != MMP_ERR_NONE) {
			return MMP_SYSTEM_ERR_PMU;
		}
	
		bAdx2002InitFlag = MMP_TRUE;	
	}//if (bDmaInitFlag == MMP_FALSE)
	return status;
}

#if (ADX2002_EN)&&(defined(MBOOT_FW)||defined(UPDATER_FW))
void ADX2002_PMU_Initialize(void)
{
    MMP_UBYTE  ubI2cTimeOut = 0x0, ubI2cTimeOutLimiet = 0xA;
    MMP_USHORT value = 0x0;

	gI2cmAttr_ADX2002.bOsProtectEn = MMP_FALSE;
	#if (ADX2002_WDT_EN == 0x1)
	#if defined(ALL_FW)
	ADX2002_WDT_Kick();  //kick watch dog
	#endif
	#if defined(MBOOT_FW)
	//ADX2002_WDT_Enable(MMP_TRUE);  //On Code-Base, default turn-off WDT
	#endif
	#endif
	gI2cmAttr_ADX2002.bOsProtectEn = MMP_TRUE;

	if (gbSystemCoreID == 0xC2) {
		gI2cmAttr_ADX2002.bOsProtectEn = MMP_FALSE;
		
		#if defined (MBOOT_FW)
			MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_CHIP_INT_STATUS, &glAdx2002IntStatus);
			MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_CHIP_INT_STATUS, glAdx2002IntStatus);
			value = 0x0;
			ubI2cTimeOut = 0x0;
			MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_CHIP_INT_STATUS, &value);
			while(value != 0x0) {
				MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_CHIP_INT_STATUS, value);
				MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_CHIP_INT_STATUS, &value);
				ubI2cTimeOut ++;
				if (ubI2cTimeOut >= ubI2cTimeOutLimiet) {
					break;					//To Avoid enter infinite while loop
				}
			}
		#endif
		
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_REG_SRAM, &value);
		
		if(value == 0x0) {	
			//Check PMU Version
			MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_REVISON, &value);
			
			if(value != 0xFF) {
				value = 0x0;
				/*Use DCDC_1_CTL1 register's default values are different in these 2 version PMU to identify it*/
				MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL1, &value);
				RTNA_DBG_PrintLong(0, value);
				if(value&0x80) {
					glPmuMode = ADX2002_PMU_V_2012;
					RTNA_DBG_Str(0, "ADX2002_PMU_V_2012......\r\n");
				}
				else {
					glPmuMode = ADX2002_PMU_V_2012E;
					RTNA_DBG_Str(0, "ADX2002_PMU_V_2012 ECOV1......\r\n");
				}
			}
			else {
				glPmuMode = ADX2002_PMU_V_2002;
			}
				//	gI2cmAttr_ADX2002.bOsProtectEn = FALSE;
			MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_REG_SRAM, (MMP_USHORT)glPmuMode);
		}
		else {
			glPmuMode = value;
			glAdx2002IntStatus = 0x0;
		}
	}

	if(glPmuMode == ADX2002_PMU_V_2012) {
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC1_CTL7, 0xFF);
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC2_CTL7, 0xEF);
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC3_CTL7, 0xEF);

		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL1, 0xD2);	
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_2_CTL1, 0xD2);	
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_3_CTL1, 0xD2);	
			
		//DC1 PWM 1.3v
			
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL2, 0x4B);	// DC1 1.35v, or 1.3v or 1.0x
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL4, 0xA8);	// PFM peak current
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL5, 0x6A);	// ZCD setting

		//MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, 0x77, 0x05);	// current limit
			
		//DC2 PWM 1.8v
			
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_2_CTL4, 0xA8);	// PFM peak current
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_2_CTL5, 0x69);	// ZCD setting
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC2_CTL8, 0x05);	// ZCHS=0, current limit enable

		//DC3 PFM 2.8v
			
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_3_CTL4, 0xA8);	// PFM peak current
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_3_CTL5, 0x69);	// ZCD setting
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC3_CTL8, 0x05);	// ZCHS=0, current limit enable
	}
	else if(glPmuMode == ADX2002_PMU_V_2012E) {

		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL1, 0xD2);
		value = 0x0;
		ubI2cTimeOut = 0x0;
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL1, &value);
		while(value != 0xD2) {
			MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL1, 0xD2);
			MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL1, &value);
			ubI2cTimeOut ++;
			if (ubI2cTimeOut >= ubI2cTimeOutLimiet) {
				break;					//To Avoid enter infinite while loop
			}
		}
	
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_2_CTL1, 0x30);
		value = 0x0;
		ubI2cTimeOut = 0x0;
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_2_CTL1, &value);
		while(value != 0x30) {
			MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_2_CTL1, 0x30);
			MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_2_CTL1, &value);
			ubI2cTimeOut ++;
			if (ubI2cTimeOut >= ubI2cTimeOutLimiet) {
				break;					//To Avoid enter infinite while loop
			}
		}
			
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_3_CTL1, 0x30);
		value = 0x0;
		ubI2cTimeOut = 0x0;
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_3_CTL1, &value);
		while(value != 0x30) {
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_3_CTL1, 0x30);
			MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_3_CTL1, &value);
			ubI2cTimeOut ++;
			if (ubI2cTimeOut >= ubI2cTimeOutLimiet) {
				break;					//To Avoid enter infinite while loop
			}
		}
		
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL2, 0x4B);  	//1.35V
		value = 0x0;
		ubI2cTimeOut = 0x0;
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL2, &value);
		while(value != 0x4B) {
			MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL2, 0x4B);
			MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL2, &value);
			ubI2cTimeOut ++;
			if (ubI2cTimeOut >= ubI2cTimeOutLimiet) {
				break;					//To Avoid enter infinite while loop
			}
		}

		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL4, 0xAC); 
		value = 0x0;
		ubI2cTimeOut = 0x0;
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL4, &value);
		while(value != 0xAC) {
			MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL4, 0xAC); 
			MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL4, &value);
			ubI2cTimeOut ++;
			if (ubI2cTimeOut >= ubI2cTimeOutLimiet) {
				break;					//To Avoid enter infinite while loop
			}
		} 
			
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL5, 0x69); 
		value = 0x0;
		ubI2cTimeOut = 0x0;
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL5, &value);
		while(value != 0x69) {
			MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL5, 0x69);
			MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_1_CTL5, &value);
			ubI2cTimeOut ++;
			if (ubI2cTimeOut >= ubI2cTimeOutLimiet) {
				break;					//To Avoid enter infinite while loop
			}
		}
		
		 
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC1_CTL7, 0xEF); 
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC1_CTL8, 0x01); 
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_2_CTL4, 0xAC); 
		value = 0x0;
		ubI2cTimeOut = 0x0;
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_2_CTL4, &value);
		while(value != 0xAC) {
			MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_2_CTL4, 0xAC); 
			MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_2_CTL4, &value);
			ubI2cTimeOut ++;
			if (ubI2cTimeOut >= ubI2cTimeOutLimiet) {
				break;					//To Avoid enter infinite while loop
			}
		}
		
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_2_CTL5, 0x69);
		value = 0x0;
		ubI2cTimeOut = 0x0;
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_2_CTL5, &value);
		while(value != 0x69) {
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_2_CTL5, 0x69); 
			MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_2_CTL5, &value);
			ubI2cTimeOut ++;
			if (ubI2cTimeOut >= ubI2cTimeOutLimiet) {
				break;					//To Avoid enter infinite while loop
			}
		}
		
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC2_CTL7, 0xEF); 
		value = 0x0;
		ubI2cTimeOut = 0x0;
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC2_CTL7, &value);
		while(value != 0xEF) {
			MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC2_CTL7, 0xEF);
			MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC2_CTL7, &value);
			ubI2cTimeOut ++;
			if (ubI2cTimeOut >= ubI2cTimeOutLimiet) {
				break;					//To Avoid enter infinite while loop
			}
		}
		
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC2_CTL8, 0x01); 
		value = 0x0;
		ubI2cTimeOut = 0x0;
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC2_CTL8, &value);
		while(value != 0x01) {
			MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC2_CTL8, 0x01);
			MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC2_CTL8, &value);
			ubI2cTimeOut ++;
			if (ubI2cTimeOut >= ubI2cTimeOutLimiet) {
				break;					//To Avoid enter infinite while loop
			}
		}
		 
		
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_3_CTL4, 0xAC);
		value = 0x0;
		ubI2cTimeOut = 0x0;
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_3_CTL4, &value);
		while(value != 0xAC) {
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_3_CTL4, 0xAC); 
			MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_3_CTL4, &value);
			ubI2cTimeOut ++;
			if (ubI2cTimeOut >= ubI2cTimeOutLimiet) {
				break;					//To Avoid enter infinite while loop
			}
		}
		
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_3_CTL5, 0x69); 
		value = 0x0;
		ubI2cTimeOut = 0x0;
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_3_CTL5, &value);
		while(value != 0x69) {
			MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_3_CTL5, 0x69); 
			MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_3_CTL5, &value);
			ubI2cTimeOut ++;
			if (ubI2cTimeOut >= ubI2cTimeOutLimiet) {
				break;					//To Avoid enter infinite while loop
			}
		}
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC3_CTL7, 0xEF); 
		MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC3_CTL8, 0x01); 
	}
	
	//After ADX2002 Settings, turn-on DCDC3 & LDO1~4 Discharge
	MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_DCDC_3_CTL2, 0xE8);
	MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_LDO_DISCHARGE_EN, 0x0F);	
	#if ((ADX2002_EN)&&(ADX2002_WDT_EN == 0x1))
	ADX2002_WDT_Kick();  //kick watch dog
	#endif
	gI2cmAttr_ADX2002.bOsProtectEn = MMP_TRUE;
}
#endif //(ADX2002_EN)&&(defined(MBOOT_FW)||defined(UPDATER_FW))

//------------------------------------------------------------------------------
//  Function    : ADX2002_IsrHandler
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used to handle the interrupt of ADX2002.

This function is used to handle the interrupt of ADX2002.
@return It reports the status of the operation.
*/
void ADX2002_IsrHandler(void) {
	ADX2002_ALARM_ID alarm_id = ADX2002_ALARM_ID_NONE;
	AITPS_GBL pGBL = AITC_BASE_GBL;
	MMP_USHORT usAdx2002IntStatus = 0, usCleanStatus;
	MMP_UBYTE  ubRet = 0xFF;
	
	
	MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_CHIP_INT_STATUS, &usAdx2002IntStatus);
	usCleanStatus = usAdx2002IntStatus;
	
	if(glPmuMode == ADX2002_PMU_V_2002) {
		MMPF_OS_Sleep(8);
	}
	while(usAdx2002IntStatus != 0) {
		if ((usAdx2002IntStatus & (1 << ADX2002_INT_EVENT_WAKE)) != 0) {
			usAdx2002IntStatus &= (~(1 << ADX2002_INT_EVENT_WAKE));
			RTNA_DBG_Str(ADX2002_DEBUG_LEVEL, "ADX2002 Wake up Event !!\r\n");
		}
		else if((usAdx2002IntStatus & (1 << ADX2002_INT_EVENT_RTC)) != 0) {
			ubRet = MMPF_OS_AcquireSem(m_RtcAlamSemID, ADX2002_SEM_TIMEOUT);
			if(glPmuMode == ADX2002_PMU_V_2002) { //for ADX2002
				if((m_alarmUse_A == MMP_TRUE)&&(m_alarmUse_B == MMP_TRUE)) {
					ADX2002_RTC_AlarmCheckID(&alarm_id);
				}
				else if(m_alarmUse_A == MMP_TRUE) {
					alarm_id = ADX2002_ALARM_ID_A;
				}
				else if(m_alarmUse_B == MMP_TRUE) {
					alarm_id = ADX2002_ALARM_ID_B;
				}
				else {
					//Because the Alarm continue about  1 minutes, althought we can stop it during 10 second,
					//But in this 10 s, we will continue receive the Alarm interrupt
					MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_CHIP_INT_STATUS, usCleanStatus);
					
					m_usTimerID[m_usTimerWriteIndex] = MMPF_OS_StartTimer(m_glAdx2002AccessTime, MMPF_OS_TMR_OPT_ONE_SHOT, (MMPF_OS_TMR_CALLBACK)&ADX2002_IsrTimerCallBack, NULL);
					if (m_usTimerID[m_usTimerWriteIndex] >= MMPF_OS_TMRID_MAX) {
						RTNA_DBG_Str(0, "Error !! : ADX2002 Get OS Timer error !\r\rn");
						MMPF_OS_Sleep(m_glAdx2002AccessTime);
						pGBL->GBL_POC_CPU_INT_EN = GBL_PMIC_INT_EN;
					}
					else {
						m_usTimerWriteIndex = ((m_usTimerWriteIndex + 1) % MMPF_OS_TMRID_MAX);
					}
					
					if (ubRet == 0) {
						MMPF_OS_ReleaseSem(m_RtcAlamSemID);
					}
					
					return;
				}
			}
			else {	//for ADX2012
				if(m_alarmUse_A == MMP_TRUE) {
					alarm_id = ADX2002_ALARM_ID_A;
				}
				else {
					//Because the Alarm continue about  1 minutes, althought we can stop it during 10 second,
					//But in this 10 s, we will continue receive the Alarm interrupt
					MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_CHIP_INT_STATUS, usCleanStatus);
					
					m_usTimerID[m_usTimerWriteIndex] = MMPF_OS_StartTimer(m_glAdx2002AccessTime, MMPF_OS_TMR_OPT_ONE_SHOT, (MMPF_OS_TMR_CALLBACK)&ADX2002_IsrTimerCallBack, NULL);
					if (m_usTimerID[m_usTimerWriteIndex] >= MMPF_OS_TMRID_MAX) {
						RTNA_DBG_Str(0, "Error !! : ADX2002 Get OS Timer error !\r\rn");
						MMPF_OS_Sleep(m_glAdx2002AccessTime);
						pGBL->GBL_POC_CPU_INT_EN = GBL_PMIC_INT_EN;
					}
					else {
						m_usTimerWriteIndex = ((m_usTimerWriteIndex + 1) % MMPF_OS_TMRID_MAX);
					}
					
					if (ubRet == 0) {
						MMPF_OS_ReleaseSem(m_RtcAlamSemID);
					}
					
					return;
				}
			}
			
			if (ubRet == 0) {
				MMPF_OS_ReleaseSem(m_RtcAlamSemID);
			}
			ADX2002_RTC_SetAlarmEnable(alarm_id, MMP_FALSE, NULL, NULL);
			
			if (m_RtcCallBack[alarm_id - 1] != NULL) {
				(*m_RtcCallBack[alarm_id - 1]) ();
			}
			//MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_CHIP_INT_EN, 0x1E);
			//MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_CHIP_INT_STATUS, 0x2);
			if(glPmuMode == ADX2002_PMU_V_2002) {
				MMPF_OS_Sleep(1000);
			}
			usAdx2002IntStatus &= (~(1 << ADX2002_INT_EVENT_RTC));
		}
		else if((usAdx2002IntStatus & (1 << ADX2002_INT_EVENT_USBJIG)) != 0) {
			usAdx2002IntStatus &= (~(1 << ADX2002_INT_EVENT_USBJIG));
			RTNA_DBG_Str(ADX2002_DEBUG_LEVEL, "ADX2002 USB JIG Event !!\r\n");
		}
		else if((usAdx2002IntStatus & (1 << ADX2002_INT_EVENT_VBUS)) != 0) {
			usAdx2002IntStatus &= (~(1 << ADX2002_INT_EVENT_VBUS));
			RTNA_DBG_Str(ADX2002_DEBUG_LEVEL, "ADX2002 USB VBUS event !!\r\n");
		}
		else if((usAdx2002IntStatus & (1 << ADX2002_INT_EVENT_POWERK)) != 0) {
			usAdx2002IntStatus &= (~(1 << ADX2002_INT_EVENT_POWERK));
			RTNA_DBG_Str(ADX2002_DEBUG_LEVEL, "ADX2002 Power Key event !!\r\n");
		}
		else if((usAdx2002IntStatus & (1 << ADX2002_INT_EVENT_RTC2)) != 0) {
			ubRet = MMPF_OS_AcquireSem(m_RtcAlamSemID, ADX2002_SEM_TIMEOUT);
			
			if(m_alarmUse_B == MMP_TRUE) {
				alarm_id = ADX2002_ALARM_ID_B;
			}
			else {
				//Because the Alarm continue about  1 minutes, althought we can stop it during 10 second,
				//But in this 10 s, we will continue receive the Alarm interrupt
				MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_CHIP_INT_STATUS, usCleanStatus);
				
				m_usTimerID[m_usTimerWriteIndex] = MMPF_OS_StartTimer(m_glAdx2002AccessTime, MMPF_OS_TMR_OPT_ONE_SHOT, (MMPF_OS_TMR_CALLBACK)&ADX2002_IsrTimerCallBack, NULL);
				if (m_usTimerID[m_usTimerWriteIndex] >= MMPF_OS_TMRID_MAX) {
					RTNA_DBG_Str(0, "Error !! : ADX2002 Get OS Timer error !\r\rn");
					MMPF_OS_Sleep(m_glAdx2002AccessTime);
					pGBL->GBL_POC_CPU_INT_EN = GBL_PMIC_INT_EN;
				}
				else {
					m_usTimerWriteIndex = ((m_usTimerWriteIndex + 1) % MMPF_OS_TMRID_MAX);
				}
				
				if (ubRet == 0) {
					MMPF_OS_ReleaseSem(m_RtcAlamSemID);
				}
				
				return;
			}
			
			if (ubRet == 0) {
				MMPF_OS_ReleaseSem(m_RtcAlamSemID);
			}
			ADX2002_RTC_SetAlarmEnable(alarm_id, MMP_FALSE, NULL, NULL);
			
			if (m_RtcCallBack[alarm_id - 1] != NULL) {
				(*m_RtcCallBack[alarm_id - 1]) ();
			}
			//MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_CHIP_INT_EN, 0x1E);
			//MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_CHIP_INT_STATUS, 0x2);
			if(glPmuMode == ADX2002_PMU_V_2002) {
				MMPF_OS_Sleep(1000);
			}
			usAdx2002IntStatus &= (~(1 << ADX2002_INT_EVENT_RTC2));
		}
		else {
			RTNA_DBG_Str(ADX2002_DEBUG_LEVEL, "ADX2002 Int processing error !\r\n");
			RTNA_DBG_PrintShort(0, usAdx2002IntStatus);
			return;
		}
	} //while(usRegValue != 0)
	
	MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_CHIP_INT_STATUS, usCleanStatus);
	
	m_usTimerID[m_usTimerWriteIndex] = MMPF_OS_StartTimer(m_glAdx2002AccessTime, MMPF_OS_TMR_OPT_ONE_SHOT, (MMPF_OS_TMR_CALLBACK)&ADX2002_IsrTimerCallBack, NULL);
	if (m_usTimerID[m_usTimerWriteIndex] >= MMPF_OS_TMRID_MAX) {
		RTNA_DBG_Str(0, "Error !! : ADX2002 Get OS Timer error !\r\rn");
		MMPF_OS_Sleep(m_glAdx2002AccessTime);
		pGBL->GBL_POC_CPU_INT_EN = GBL_PMIC_INT_EN;
	}
	else {
		m_usTimerWriteIndex = ((m_usTimerWriteIndex + 1) % MMPF_OS_TMRID_MAX);
	}
}

//------------------------------------------------------------------------------
//  Function    : ADX2002_ADC_Measure
//  Description :
//------------------------------------------------------------------------------
/** @brief The function return the measured voltage.
The function return the measured voltage.
@param[out] level is the return value of voltage (unit: mV)
@return It reports the status of the operation.
*/
MMP_ERR ADX2002_ADC_Measure(MMP_USHORT *level)
{
	MMP_ULONG timeout = 0;
	MMP_USHORT get_val = 0;
	MMP_USHORT get_measured_value_low = 0;
 	MMP_USHORT get_measured_value_high = 0;
 	MMP_USHORT VDD_LDO4 = 0;
 	MMP_USHORT voltage = 0; // unit : mV
 	
 	//Enable Battery Measure
	MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_POWERON_CTL, 0x14);
	MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_TP_7, 0x01);
	timeout = 0;
	do
	{
		MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_TP_7, &get_val);
		MMPF_OS_Sleep(0x10);
		timeout ++;
	}while((get_val != 0x02) && (timeout < 500));
						
	if (get_val == 0x02)
 	{
		RTNA_DBG_Str(0, "ADC Initialize success\r\n");
 	}
	else
	{
		RTNA_DBG_Str(0, "ADC Initialize Fail\r\n");
		return MMP_SYSTEM_ERR_PMU;			
	}
	
	
	MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_TP_8, &get_measured_value_low);
	MMPF_I2cm_ReadReg(&gI2cmAttr_ADX2002, ADX2002_TP_9, &get_measured_value_high);
	voltage = ((get_measured_value_high & 0x03) << 8) | get_measured_value_low;
	
	ADX2002_LDO_GetVoltage(ADX2002_LDO_ID_4, &VDD_LDO4);
	voltage = (voltage*3*VDD_LDO4)/1024;
	*level = voltage;
	
	#if 0
	RTNA_DBG_PrintShort(ADX2002_DEBUG_LEVEL, get_measured_value_low);
	RTNA_DBG_PrintShort(ADX2002_DEBUG_LEVEL, get_measured_value_high);
	RTNA_DBG_PrintShort(ADX2002_DEBUG_LEVEL, VDD_LDO4);
	RTNA_DBG_PrintShort(ADX2002_DEBUG_LEVEL, (MMP_USHORT)*level);
	#endif
	//Disable Battery Measure
	MMPF_I2cm_WriteReg(&gI2cmAttr_ADX2002, ADX2002_POWERON_CTL, 0x03);
	return MMP_ERR_NONE;
}
#endif //#if (ADX2002_EN)

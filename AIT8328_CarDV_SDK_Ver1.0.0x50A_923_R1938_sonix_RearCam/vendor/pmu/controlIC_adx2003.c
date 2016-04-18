/*===========================================================================
 * Include file
 *===========================================================================*/ 

#include "config_fw.h"
#include "controlIC_reg_adx2003.h"
#include "controlIC_adx2003.h"
#include "lib_retina.h"
#include "mmpf_i2cm.h"
#include "mmpf_rtc.h"
#include "os_wrap.h"
#include "mmpf_pio.h"
#include "mmpf_timer.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/ 

#define USE_ADC_SEMI (1)

#if defined(ALL_FW)
#define USE_ADX2003_SEMI (0)
#else
#define USE_ADX2003_SEMI (0)
#endif

#if (ADX2003_EN)

#define ADX2003_ADC_VOLTAGE 	(ADC_REFERENCE_VOLTAGE)

/*===========================================================================
 * Global varible
 *===========================================================================*/ 

//Attention: Please note the SLAVE_ADDR is different among software and HW I2CM
//HW I2CM
MMP_I2CM_ATTR gI2cmAttribute_adx2003 = {
	MMP_I2CM2,			//i2cmID
	ADX2003_SLAVE_ADDR, //ubSlaveAddr
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
	450000 /*450KHZ*/, 	//ulI2cmSpeed
	MMP_TRUE, 			//bOsProtectEn
	NULL, 				//sw_clk_pin
	NULL, 				//sw_data_pin
	MMP_FALSE,			//bRfclModeEn 
	MMP_FALSE,			//bWfclModeEn
	MMP_FALSE,			//bRepeatModeEn
    0                   //ubVifPioMdlId
};

volatile MMP_BOOL m_alarmUse_A = MMP_FALSE, m_alarmUse_B = MMP_FALSE;

#pragma arm section code, rodata, rwdata, zidata
     
static MMPF_OS_SEMID  m_RtcAlamSemID;

#if (USE_ADC_SEMI)
static MMPF_OS_SEMID   m_ADCSemID;
#endif
#if (USE_ADX2003_SEMI)
static MMPF_OS_SEMID   m_ADX2003SemID;
#endif
#if defined(MBOOT_FW)
static MMP_BOOL 		m_bBootTimeOut = MMP_FALSE;
#endif 

static RTC_CallBackFunc *(m_RtcCallBack[ADX2003_ALARM_MAX - 1]);
static MMP_USHORT m_usTimerID[MMPF_OS_TMRID_MAX];
static MMP_USHORT m_usTimerReadIndex = 0, m_usTimerWriteIndex = 0;
static MMP_ULONG m_glAdx2003AccessTime = 0x20;		//unit: ms
static MMP_BOOL	 m_bADX2013 = MMP_FALSE;
//static MMP_ULONG m_glAdx2003CleanWdtTime = 3000;  //Timer interval to clean ADX2003 watch-dog , unit: ms

MMP_USHORT glAdx2003IntStatus = 0;
 
/*===========================================================================
 * Extern varible
 *===========================================================================*/ 

extern 	MMPF_OS_MQID  	HighTask_QId;

#if defined(MBOOT_FW)
#define printc(x, ...)
#else
extern void printc( char* szFormat, ... );
#endif

/*===========================================================================
 * Main body
 *===========================================================================*/ 

static void ADX2003_Timer_TaskMode(void)
{
	AITPS_GBL pGBL = AITC_BASE_GBL;
	
	if(MMPF_OS_StopTimer(m_usTimerID[m_usTimerReadIndex], OS_TMR_OPT_NONE) != 0) {
		RTNA_DBG_Str(0, "ADX2003 Stop OS Timer error !!\r\n");
	}
	m_usTimerID[m_usTimerReadIndex] = OS_TMR_OPT_NONE; //clean the timer id by writing one invalidate ID.
	m_usTimerReadIndex = ((m_usTimerReadIndex + 1) % MMPF_OS_TMRID_MAX);
	
	#if (CHIP == P_V2)
	//Re-open the interrupt enable in AIT chip
	pGBL->GBL_POC_INT_EN = GBL_PMIC_INT_EN;	
	#endif
}

static void ADX2003_IsrTimerCallBack(OS_TMR *ptmr, void *p_arg)
{
	ADX2003_Timer_TaskMode();
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_Read_Reg
//  Description : This function is used for reading PMU chip's registers.
//------------------------------------------------------------------------------
/** @brief This function is used for reading PMU chip's registers.
@param[in] 	regNAME is main register.
@param[out] returnValue is the value get from the register.
@return 	It reports the status of the operation.
*/
MMP_ERR ADX2003_Read_Reg(ADX2003_REG regNAME, MMP_USHORT *returnValue) 
{
	MMP_ERR status = MMP_ERR_NONE;
	
	if(regNAME > ADX2003_OTHER)
		return -1;

    status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, regNAME, returnValue);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_Write_Reg
//  Description : This function is used for writing PMU chip's registers.
//------------------------------------------------------------------------------
/** @brief This function is used for writing PMU chip's registers.
@param[in] 	regNAME is main register.
@param[out] nValue is the value set the register.
@return 	It reports the status of the operation.
*/
MMP_ERR ADX2003_Write_Reg(ADX2003_REG regNAME, MMP_UBYTE nValue) 
{
	if(regNAME > ADX2003_OTHER)
		return -1;

    MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, regNAME, nValue);

	return MMP_ERR_NONE;
}

#if ((ADX2003_EN) && (ADX2003_WDT_EN == 0x1))
extern MMPF_OS_MQID  	LTask_QId;

//------------------------------------------------------------------------------
//  Function    : ADX2003_WDT_CleanStatus
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to kick Watch dog one time.

The function is used to kick Watch dog one time.
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_WDT_CleanStatus(void) 
{
	return MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_CHIP_INT_STATUS, 0x1);
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_WDT_Reset
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to reset watch dog counter.

The function is used to reset watch dog counter.
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_WDT_Reset(void) 
{
	MMP_ERR status = MMP_ERR_NONE;
	
	status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_WDT_CTL, 0x04);
	MMPF_OS_Sleep(1000);
	status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_WDT_CTL, 0x84);

	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	
	return MMP_ERR_NONE; 
}

/*
static void ADX2003_WDT_TimerCallBack(OS_TMR *ptmr, void *p_arg)
{
	MMPF_OS_PutMessage(LTask_QId, (void *)(&ADX2003_WDT_CleanStatus));
}
*/

//------------------------------------------------------------------------------
//  Function    : ADX2003_WDT_Enable
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to enable/disable ADX2003 watch dog.

The function is used to enable/disable ADX2003 watch dog.
@param[in] bEnable : enable/diable watch dog.
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_WDT_Enable(MMP_BOOL bEnable) 
{
	MMP_ERR status = MMP_ERR_NONE;
	MMP_USHORT value = 0;
	
	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_WDT_CTL, &value);
	
	if(bEnable) {
		status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_WDT_CTL, (value|0x80));
	}
	else {
		status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_WDT_CTL, (value&0x7F));
	}
	
	#if 0
	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_WDT_CTL, &value);
	RTNA_DBG_PrintLong(0, value);
	#endif
	
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return MMP_ERR_NONE; 
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_WDT_SetTimeOut
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to enable/disable ADX2003 watch dog.

The function is used to enable/disable ADX2003 watch dog.
@param[in] ulMilleSec : Time out seting. The unit is "ms".
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_WDT_SetTimeOut(MMP_ULONG ulMilleSec) 
{
	MMP_ERR status = MMP_ERR_NONE;
	MMP_USHORT value = 0;
	
	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_WDT_CTL, &value);
	
	switch (ulMilleSec) {
		case 3000:
			status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_WDT_CTL, (value | 0x40));
			//MMPF_OS_Sleep(1000);
			break;
		case 10000:
			status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_WDT_CTL, (value & 0xB0));
			//MMPF_OS_Sleep(1000);
			break;
		default:
			RTNA_DBG_Str(ADX2003_DEBUG_LEVEL, "Un-supported WDT timeout\r\n");
			break;
	}
	
	#if 0 //debug
	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_WDT_CTL, &value);
	RTNA_DBG_PrintLong(0, value);
	#endif
	
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return MMP_ERR_NONE; 
}
#endif

#if defined(MBOOT_FW)
static void ADX2003_BOOT_TimerISR(void)
{
	m_bBootTimeOut = MMP_TRUE;
}
#endif

#if (ADX2003_ACT_DISCHARGE==1)
MMP_ERR ADX2003_ActiveDischarge_En(MMP_BOOL bEnable)
{
	MMP_ERR status = MMP_ERR_NONE;
 	MMP_USHORT nRegValue = 0;

	if (bEnable == MMP_TRUE) {

		status |=MMPF_I2cm_ReadReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_1_CTL2, &nRegValue);
		nRegValue |= ADX2003_DCDC_DISEN; // enable DC-DC1 active discharge
		status |=MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_1_CTL2, nRegValue);

		status |=MMPF_I2cm_ReadReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_2_CTL2, &nRegValue);
		nRegValue |= ADX2003_DCDC_DISEN; // enable DC-DC2 active discharge
		status |=MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_2_CTL2, nRegValue);

		status |=MMPF_I2cm_ReadReg( &gI2cmAttribute_adx2003, ADX2003_LDO_DISCHARGE_EN, &nRegValue);
		
		nRegValue |= 0x0F; // enable LDO1-4 active discharge
		status |=MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_LDO_DISCHARGE_EN, nRegValue);

	} else {

		status |=MMPF_I2cm_ReadReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_1_CTL2, &nRegValue);
		nRegValue &= (~ADX2003_DCDC_DISEN); // disable DC-DC1 active discharge
		status |=MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_1_CTL2, nRegValue);

		status |=MMPF_I2cm_ReadReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_2_CTL2, &nRegValue);
		nRegValue &= (~ADX2003_DCDC_DISEN); // disable DC-DC2 active discharge
		status |=MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_2_CTL2, nRegValue);

		status |=MMPF_I2cm_ReadReg( &gI2cmAttribute_adx2003, ADX2003_LDO_DISCHARGE_EN, &nRegValue);
		nRegValue &= (~0x0F); // disable LDO1-4 active discharge
		status |=MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_LDO_DISCHARGE_EN, nRegValue);
	}

	return status;
}
#endif

//------------------------------------------------------------------------------
//  Function    : ADX2003_PowerOff
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to power off PMU.

The function is used to power off PMU.
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_PowerOff(void)
{
	MMP_ERR status = MMP_ERR_NONE;

#if (USE_ADX2003_SEMI)
    if(MMPF_OS_AcquireSem(m_ADX2003SemID, 0)) 
    {
		RTNA_DBG_Str(0, "m_ADX2003SemID OSSemPost: Fail!! \r\n");
		return 1;
	}
#endif
    
#if (ADX2003_ACT_DISCHARGE)
	ADX2003_ActiveDischarge_En(1); // turn on active discharge when power off
#endif

	//status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_CHIP_INT_STATUS, 0x05); 
	status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_POWERON_CTL, 0x83);
	status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_BAT_CTL2, 0x00);
	while(1){
        RTNA_DBG_Str(0, "ADX2003_PowerOff fail.\r\n");
        status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_POWERON_CTL, 0x83);
        status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_BAT_CTL2, 0x00);
        MMPF_OS_Sleep(50);
    }
#if (USE_ADX2003_SEMI)
    MMPF_OS_ReleaseSem(m_ADX2003SemID);
#endif
	
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return status;
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_EnableLDO
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to turn on/off PMU LDO.

The function is used to turn on/off PMU LDO.
@param[in] ADX2003_LDO_ID is the specific ID number.
@param[in] bEnable : MMP_TRUE to turn on and MMP_FALSE to turn off
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_EnableLDO(ADX2003_LDO_ID ldoID, MMP_BOOL bEnable)
{
	MMP_ERR status = MMP_ERR_NONE;
	MMP_USHORT usValue = 0;
	
	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, (ADX2003_LDO_1_CTL + ldoID), &usValue);
	
	if (bEnable) {
		usValue |= 0x80;  
	}
	else {
		usValue &= 0x7F;
	}
	
	status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, (ADX2003_LDO_1_CTL + ldoID), usValue);
	
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return status;
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_EnableDCDC
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to turn on/off PMU DCDC.

The function is used to turn on/off PMU DCDC.
@param[in] ADX2003_DCDC_ID is the specific ID number.
@param[in] bEnable : MMP_TRUE to turn on and MMP_FALSE to turn off
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_EnableDCDC(ADX2003_DCDC_ID dcdcID, MMP_BOOL bEnable)
{
	MMP_ERR status = MMP_ERR_NONE;
	MMP_USHORT usValue = 0;
	
	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, (ADX2003_DCDC_1_CTL2 + dcdcID*ADX2003_DCDC_REG_SIZE), &usValue);
	
	if (bEnable) {
		usValue |= 0x40;  
	}
	else {
		usValue &= ~(0x40);  
	}
	
	status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, (ADX2003_DCDC_1_CTL2 + dcdcID*ADX2003_DCDC_REG_SIZE), usValue);

	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return status;
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_CheckBootComplete
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to check if system can initialize normally or not.

The function is used to check if system can initialize normally or not.
@param[in] ulMilliSecond : the time interval to check user press the power-key or not.
@param[in] pio_power_key : the PIO number, please reference the data structure of MMP_GPIO_PIN
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_CheckBootComplete(MMP_ULONG ulMilliSecond, MMP_GPIO_PIN pio_power_key) 
{
	MMP_ERR status = MMP_ERR_NONE;
	
	#if defined(MBOOT_FW)
	MMP_USHORT value = 0x0;
	MMP_UBYTE ubPowerKeyStatus = 0x1;
	MMPF_TIMER_ATTR TimerAttribute;
	
	#if (ADX2003_WDT_EN == 0x1)
	ADX2003_WDT_Enable(MMP_FALSE);
	#endif

	if((glAdx2003IntStatus & (1 << ADX2003_INT_EVENT_POWERK)) != 0) 
	{	
		status |= MMPF_PIO_EnableOutputMode(pio_power_key, MMP_FALSE, MMP_TRUE);

    	TimerAttribute.TimePrecision = MMPF_TIMER_MILLI_SEC;
    	TimerAttribute.ulTimeUnits   = ulMilliSecond;
    	TimerAttribute.Func          = ADX2003_BOOT_TimerISR;
    	TimerAttribute.bIntMode      = MMPF_TIMER_PERIODIC;
    	status |= MMPF_Timer_Start(MMPF_TIMER_1, &TimerAttribute);

		while((!m_bBootTimeOut)&&(ubPowerKeyStatus == 1)) {
			status |= MMPF_PIO_GetData(pio_power_key, &ubPowerKeyStatus);
		}
		
		MMPF_Timer_Stop(MMPF_TIMER_1);

		if (ubPowerKeyStatus) {
			RTNA_DBG_Str(ADX2003_DEBUG_LEVEL, "\r\nBoot Success!!\r\n");
		}
		else {
			RTNA_DBG_Str(ADX2003_DEBUG_LEVEL, "\r\nBoot fail !!\r\n");
			
			//Power turn-off
			status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_POWERON_CTL, 0x83);
			status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_BAT_CTL2, 0x00);
		}
		
		//Note: In MBOOT_FW, we should not clean the status, because customer may ask power-on status in ALL_FW
	}
	else {
		#if defined(MBOOT_FW)
		//Turn-off Discharge
		status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, 0x85, 0x0);
			
		MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, 0x12, &value);
		status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, 0x12, (value & 0x7F));
		value = 0x0;
		
		MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, 0x0C, &value);
		status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, 0x0C, (value & 0x7F));
		value = 0x0;
		
		MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, 0x06, &value);
		status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, 0x06, (value & 0x7F));
		#endif
	
	}
	
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
#endif

	return status;
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_GetBootStatus
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to send back the boot-up events

The function is used to send back the boot-up events.
@param[out] usRetStatus : the bit 0 stands for ADX2003_INT_EVENT_WAKE, and bit 4 stands for ADX2003_INT_EVENT_POWERK...etc., please refer ADX2003_INT_EVENT
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_GetBootStatus(MMP_USHORT * usRetStatus)
{
	*usRetStatus = glAdx2003IntStatus;
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_RTC_Initialize
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to intial RTC driver
The function is used to intial RTC driver.
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_RTC_Initialize(void)
{
	static MMP_BOOL bRtcInitFlag = MMP_FALSE;
	MMP_ERR status = MMP_ERR_NONE;
	MMP_ULONG i = 0;
	
	if (bRtcInitFlag == MMP_FALSE) 
	{
		m_RtcAlamSemID =  MMPF_OS_CreateSem(1);
		status |= MMPF_PIO_Initialize();
	
		for (i = 0; i < (ADX2003_ALARM_MAX - 1); i++) {
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
//  Function    : ADX2003_RTC_CheckTimeFormat
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used for checking the validation of Time format.

This function is used for checking the validation of Time format.
@param[in] pointer of structure AUTL_DATETIME.
@return It reports the status of the operation.
*/
static MMP_ERR ADX2003_RTC_CheckTimeFormat(AUTL_DATETIME *ptr)
{
	MMP_ULONG ulDays[13]={0,31,0,31,30,31,30,31,31,30,31,30,31};
	
	if ((ptr->usYear < ADX2003_RTC_START_YEAR) || (ptr->usYear > ADX2003_RTC_MAX_YEAR)) {
		RTNA_DBG_Str(ADX2003_DEBUG_LEVEL, "Time Year\r\n");
		return MMP_SYSTEM_ERR_FORMAT;
	}
	
	if (((ptr->usYear%4 == 0)&&(ptr->usYear%100 != 0)) || (ptr->usYear%400 == 0)) {
		ulDays[2] = 29;	
	}
	else {
		ulDays[2] = 28;	
	}
	
	if((ptr->usMonth > 12) || (ptr->usMonth == 0) || (ptr->usDay > ulDays[ptr->usMonth]) || (ptr->usDay == 0)) {
		RTNA_DBG_Str(ADX2003_DEBUG_LEVEL, "Time Month day\r\n");
		return MMP_SYSTEM_ERR_FORMAT;
	}

	if((ptr->usHour > 23) || (ptr->usMinute > 59) || (ptr->usSecond > 59)) {
		RTNA_DBG_Str(ADX2003_DEBUG_LEVEL, "Time Month ubHour Minute Second\r\n");
		return MMP_SYSTEM_ERR_FORMAT;
	}
	
	if((ptr->b_12FormatEn == MMP_TRUE)&&(ptr->usHour > 12)) {
		RTNA_DBG_Str(ADX2003_DEBUG_LEVEL, "Time 12 Hours\r\n");
		return MMP_SYSTEM_ERR_FORMAT;
	}
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_RTC_GetTime
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used for getting RTC information in ADX2003.

This function is used for getting RTC information in ADX2003.
@param[in] pointer of structure AUTL_DATETIME.
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_RTC_GetTime(AUTL_DATETIME *ptr)
{
	MMP_USHORT value = 0;
	MMP_ERR	status = MMP_ERR_NONE;
	
#if (USE_ADX2003_SEMI)
    if(MMPF_OS_AcquireSem(m_ADX2003SemID, 0)) 
    {
		RTNA_DBG_Str(0, "m_ADX2003SemID OSSemPost: Fail!! \r\n");
		return 1;
	}
#endif
	
	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_RTC_DAY, &value);
	ptr->usDayInWeek = value & 0x7;
	 
	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_RTC_YEAR, &value);
	ptr->usYear = (value >> 4)*10 + (value & ADX2003_RTC_LMASK) + ADX2003_RTC_START_YEAR;
	
	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_RTC_MON, &value);
	ptr->usMonth = ((value & 0x10) >> 4)*10 +  (value & ADX2003_RTC_LMASK);
	
	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_RTC_DATE, &value);
	ptr->usDay = ((value & 0x30) >> 4)*10 +  (value & ADX2003_RTC_LMASK);
	
	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_RTC_HOUR, &value);

	if (value & 0x80) 
	{	//12-hour format
		ptr->usHour = (((value & 0x10) >> 4)*10 + (value & ADX2003_RTC_LMASK))%12;
		if(value & 0x20) { //Check if the time at P.M.
			ptr->ubAmOrPm = 0x1;
		}
	}
	else 
	{	//24-hour format
		ptr->usHour = ((value & 0x30) >> 4)*10 + (value & ADX2003_RTC_LMASK);
	}
	
	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_RTC_MIN, &value);
	ptr->usMinute = ((value & 0x70) >> 4)*10 +  (value & ADX2003_RTC_LMASK);
	
	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_RTC_SEC, &value);
	ptr->usSecond = ((value & 0x70) >> 4)*10 +  (value & ADX2003_RTC_LMASK);
	
#if (USE_ADX2003_SEMI)
    MMPF_OS_ReleaseSem(m_ADX2003SemID);
#endif
	
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_RTC_SetTime
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used for setting time information to RTC in ADX2003.

This function is used for setting time information to RTC in ADX2003.
@param[in] pointer of structure AUTL_DATETIME.
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_RTC_SetTime(AUTL_DATETIME *ptr)
{
	MMP_ERR status;
	MMP_USHORT value = 0;
	
	status = ADX2003_RTC_CheckTimeFormat(ptr);
	
	if ( status != MMP_ERR_NONE) {
		return status;
	}
	
#if (USE_ADX2003_SEMI)
    if(MMPF_OS_AcquireSem(m_ADX2003SemID, 0))
    {
		RTNA_DBG_Str(0, "m_ADX2003SemID OSSemPost: Fail!! \r\n");
		return 1;//error
	}
#endif
	
	value =  (((ptr->usYear - ADX2003_RTC_START_YEAR)/10) << 4)  + ((ptr->usYear - ADX2003_RTC_START_YEAR)%10);
	status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_RTC_YEAR, value);
	
	value = (((ptr->usMonth/10) << 4)&0x10) + (ptr->usMonth%10);
	status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_RTC_MON, value);
	
	value = (((ptr->usDay/10) << 4)&0x30) + (ptr->usDay%10);
	status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_RTC_DATE, value);
	
	if (ptr->usHour >= 10) {
		value = ((ptr->usHour/10) << 4) + (ptr->usHour % 10);
	}
	else {
		value = ptr->usHour;
	}
	
	if (ptr->b_12FormatEn == MMP_TRUE) 
	{
		value = 0x80 | value;
		if(ptr->ubAmOrPm == 0x1) {//set as P.M.
			value = 0x20 | value;
		}
	}
	status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_RTC_HOUR, value);
	
	value = (((ptr->usMinute/10) << 4)&0x70) + (ptr->usMinute%10);
	status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_RTC_MIN, value);
	
	value = (((ptr->usSecond/10) << 4)&0x70) + (ptr->usSecond%10);
	status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_RTC_SEC, value);
	
#if (USE_ADX2003_SEMI)
    MMPF_OS_ReleaseSem(m_ADX2003SemID);
#endif
	
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return status;
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_RTC_AlarmCheckID
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used for check which alarm happens when we turn on 2 alarms.

This function is used for check which alarm happens when we turn on 2 alarms.
@param[out] alarm_id : the alarm ID which alarm expires.
@return It reports the status of the operation.
*/
static MMP_ERR ADX2003_RTC_AlarmCheckID(ADX2003_ALARM_ID* alarm_id)
{
	AUTL_DATETIME current_time, alarm_time;
	MMP_ERR status = MMP_ERR_NONE;
	
	status |= ADX2003_RTC_GetTime(&current_time);
	
	//Read Alarm1 settings first
	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_RTC_ALARM1_H, (MMP_USHORT*)&(alarm_time.usHour));
	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_RTC_ALARM1_DATE, (MMP_USHORT*)&(alarm_time.usDay));
	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_RTC_ALARM1_M, (MMP_USHORT*)&(alarm_time.usMinute));
	
	//Because of ADX2003 alarm is "minute" level, 
	//so we assume that we can make sure that we can check alarm at "this minute"
	if((alarm_time.usDay == current_time.usDay)&&(alarm_time.usHour == current_time.usHour)&&(alarm_time.usMinute == current_time.usMinute)) 
	{
		*alarm_id = ADX2003_ALARM_ID_A; 
	}
	else 
	{
		status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_RTC_ALARM1_H, (MMP_USHORT*)&(alarm_time.usHour));
		status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_RTC_ALARM1_DATE, (MMP_USHORT*)&(alarm_time.usDay));
		status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_RTC_ALARM1_M, (MMP_USHORT*)&(alarm_time.usMinute));

		if((alarm_time.usDay == current_time.usDay)&&(alarm_time.usHour == current_time.usHour)&&(alarm_time.usMinute == current_time.usMinute)) {
			*alarm_id = ADX2003_ALARM_ID_B; 
		}
		else {
			*alarm_id = ADX2003_ALARM_ID_NONE;
		}
	}
	
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return status;
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_RTC_SetAlarmEnable
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
MMP_ERR ADX2003_RTC_SetAlarmEnable(ADX2003_ALARM_ID alarm_id, MMP_BOOL bEnable, AUTL_DATETIME *ptr, void* p_callback)
{
	MMP_USHORT adx2003_value = 0;
	AITPS_GBL pGBL = AITC_BASE_GBL;
	MMP_ULONG ulDays[13]={0,31,0,31,30,31,30,31,31,30,31,30,31};
	MMP_UBYTE ubRet = 0xFF;
	MMP_ERR status = MMP_ERR_NONE;
	
	//MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_CHIP_INT_EN, 0x1E);
		
	if (bEnable) 
	{	
		//Alarm time format checking
		if (((ptr->usYear%4 == 0)&&(ptr->usYear%100 != 0)) || (ptr->usYear%400 == 0)) {
            ulDays[2] = 29; //February days check	
		}
		else {
            ulDays[2] = 28;	//February days check
		}
		
		if((ptr->usDay > ulDays[ptr->usMonth]) || (ptr->usDay == 0)) {
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
	
		ubRet = MMPF_OS_AcquireSem(m_RtcAlamSemID, ADX2003_SEM_TIMEOUT);
		
		if (alarm_id == ADX2003_ALARM_ID_A)
		{
			if (m_alarmUse_A == MMP_FALSE) 
			{
				adx2003_value = (((ptr->usDay/10) << 4)&0x30) + (ptr->usDay%10);
			
				status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_RTC_ALARM1_DATE, adx2003_value);
                status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_RTC_HOUR, &adx2003_value);
				
                if (adx2003_value & 0x80) { //RTC in 12-hour format
                    if (ptr->b_12FormatEn == 0x1) { //input in 12-hour format
                        adx2003_value = ((ptr->usHour/10) << 4) + (ptr->usHour % 10);
                        if (ptr->ubAmOrPm == 0x1)
                            adx2003_value |= 0x20; // PM
                    }
                    else { //input in 24-hour format
                        adx2003_value = (ptr->usHour >= 12) ? 0x20 : 0; //set PM or AM
                        ptr->usHour = (ptr->usHour > 12) ? ptr->usHour - 12 : ptr->usHour;
                        adx2003_value |= (((ptr->usHour/10) << 4) + (ptr->usHour % 10));
                    }
                }
				else { //RTC in 24-hour format
				    if (ptr->b_12FormatEn == 0x1) { //input in 12-hour format
				        if ((ptr->ubAmOrPm == 0x1) && (ptr->usHour != 12))
				            ptr->usHour += 12;
                    }
                    adx2003_value = ((ptr->usHour/10) << 4) + (ptr->usHour % 10);
                }
                
				status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_RTC_ALARM1_H, adx2003_value);
				
				adx2003_value = 0;
				adx2003_value = (((ptr->usMinute/10) << 4)&0x70) + (ptr->usMinute%10);
				
				status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_RTC_ALARM1_M, adx2003_value);
				
				adx2003_value = 0;
				//adx2003_value = 1 << (ptr->usDayInWeek - 1);  //for ADX2003 alarm, Day and Date settings can "only" use one of them !!
				adx2003_value |= 0x80;  //Alarm on
				
				status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_RTC_ALARM1_DAY, adx2003_value);
				m_alarmUse_A = MMP_TRUE;
			}
			else 
			{
				if (ubRet == 0) {
					MMPF_OS_ReleaseSem(m_RtcAlamSemID);
				}
				
				#if 0
				ADX2003_RTC_SetAlarmEnable(ADX2003_ALARM_ID_A, MMP_FALSE, NULL, NULL);
				ADX2003_RTC_SetAlarmEnable(ADX2003_ALARM_ID_A, MMP_TRUE, ptr, p_callback);
				return MMP_ERR_NONE;
				#else
				return MMP_SYSTEM_ERR_DOUBLE_SET_ALARM;
				#endif
			}
		}
		else if (alarm_id == ADX2003_ALARM_ID_B) 
		{
			if(m_alarmUse_B == MMP_FALSE) 
			{
				adx2003_value = 0;
				adx2003_value = (((ptr->usDay/10) << 4)&0x30) + (ptr->usDay%10);
				
				status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_RTC_ALARM2_DATE, adx2003_value);
				
				adx2003_value = 0;
				if (ptr->usHour > 10) {
					adx2003_value = ((ptr->usHour/10) << 4) + (ptr->usHour % 10);
				}
				else {
					adx2003_value = ptr->usHour;
				}
				
				status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_RTC_HOUR, &adx2003_value);
                
                if (adx2003_value & 0x80) { //RTC in 12-hour format
                    if (ptr->b_12FormatEn == 0x1) { //input in 12-hour format
                        adx2003_value = ((ptr->usHour/10) << 4) + (ptr->usHour % 10);
                        if (ptr->ubAmOrPm == 0x1)
                            adx2003_value |= 0x20; // PM
                    }
                    else { //input in 24-hour format
                        adx2003_value = (ptr->usHour >= 12) ? 0x20 : 0; //set PM or AM
                        ptr->usHour = (ptr->usHour > 12) ? ptr->usHour - 12 : ptr->usHour;
                        adx2003_value |= (((ptr->usHour/10) << 4) + (ptr->usHour % 10));
                    }
                }
				else { //RTC in 24-hour format
				    if (ptr->b_12FormatEn == 0x1) { //input in 12-hour format
				        if ((ptr->ubAmOrPm == 0x1) && (ptr->usHour != 12))
				            ptr->usHour += 12;
                    }
                    adx2003_value = ((ptr->usHour/10) << 4) + (ptr->usHour % 10);
                }
				status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_RTC_ALARM2_DAY, adx2003_value);
				
				m_alarmUse_B = MMP_TRUE;
			}
			else 
			{			
				if (ubRet == 0) {
					MMPF_OS_ReleaseSem(m_RtcAlamSemID);
				}
				
				#if 0
				ADX2003_RTC_SetAlarmEnable(ADX2003_ALARM_ID_B, MMP_FALSE, NULL, NULL);
				ADX2003_RTC_SetAlarmEnable(ADX2003_ALARM_ID_B, MMP_TRUE, ptr, p_callback);
				return MMP_ERR_NONE;
				#else
				return MMP_SYSTEM_ERR_DOUBLE_SET_ALARM;
				#endif
			}
		}
		else 
		{
			RTNA_DBG_Str(ADX2003_DEBUG_LEVEL, "Error RTC Alarm settings 1 !\r\n");
		
			if (ubRet == 0) {
				MMPF_OS_ReleaseSem(m_RtcAlamSemID);
			}
			return MMP_SYSTEM_ERR_FORMAT;
		}
		
		m_RtcCallBack[alarm_id - 1] = (RTC_CallBackFunc*)p_callback;
		status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_RTC_INT_CTL, &adx2003_value);	
		
		if(m_bADX2013 == MMP_FALSE) {
			adx2003_value |= (1 << (5 + alarm_id));
		}
		else {
			adx2003_value |= 0xC0;   //Always turn-on Alarm A & Alarm B output in ADX2012
		}
		
		//Enable Interrupt
		status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_RTC_INT_CTL, adx2003_value);
		
		/*
		if(m_alarmUse_A & m_alarmUse_B) {
			adx2003_value = 0xC0;
		}
		RTNA_DBG_PrintLong(0, (MMP_ULONG)ADX2003_RTC_INT_CTL);
		//adx2003_value = 0x40;
		RTNA_DBG_PrintLong(0, adx2003_value);
		*/
		
		#if 0 //waiting time for RTC alarm settings to work.
		MMPF_OS_Sleep(7000);  //For previous ADX2003, we need some time to let ADX2003 read and done the settings. 7(s) is experiments value.
		#endif
		
		#if 0//debug
		MMPF_OS_Sleep(5);
		status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_RTC_INT_CTL, &adx2003_value);
		RTNA_DBG_Str(0, "ADX2003_RTC_INT_CTL :");
		RTNA_DBG_Long(0, adx2003_value);
		RTNA_DBG_Str(0, "\r\n");
		
		MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_RTC_ALARM1_DATE, &adx2003_value);
		RTNA_DBG_Str(0, "ADX2003_RTC_ALARM1_DATE :");
		RTNA_DBG_Long(0, adx2003_value);
		RTNA_DBG_Str(0, "\r\n");
		
		MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_RTC_ALARM1_H, &adx2003_value);
		RTNA_DBG_Str(0, "ADX2003_RTC_ALARM1_H :");
		RTNA_DBG_Long(0, adx2003_value);
		RTNA_DBG_Str(0, "\r\n");
		
		MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_RTC_ALARM1_M, &adx2003_value);
		RTNA_DBG_Str(0, "ADX2003_RTC_ALARM1_M :");
		RTNA_DBG_Long(0, adx2003_value);
		RTNA_DBG_Str(0, "\r\n");
		
		MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_RTC_ALARM1_DAY, &adx2003_value);
		RTNA_DBG_Str(0, "ADX2003_RTC_ALARM1_DAY :");
		RTNA_DBG_Long(0, adx2003_value);
		RTNA_DBG_Str(0, "\r\n");
		#endif
		
		if (ubRet == 0) {
			MMPF_OS_ReleaseSem(m_RtcAlamSemID);
		}
		
		#if (CHIP == P_V2)
		if (pGBL->GBL_POC_INT_EN != GBL_PMIC_INT_EN) {
			pGBL->GBL_POC_INT_EN = GBL_PMIC_INT_EN;
		}
		#endif
	}
	else 
	{
		ubRet = MMPF_OS_AcquireSem(m_RtcAlamSemID, ADX2003_SEM_TIMEOUT);
		
		if((m_alarmUse_A == MMP_TRUE) && (m_alarmUse_B == MMP_TRUE)) 
		{
			if (alarm_id == ADX2003_ALARM_ID_A) {
				RTNA_DBG_Str(ADX2003_DEBUG_LEVEL, "Turn-off alram A\r\n");
				status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_RTC_ALARM1_DAY, 0x0);
				if(m_bADX2013 == MMP_FALSE) {
					status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_RTC_INT_CTL, 0x80);  //keep Alarm B on
				}
			}
			else if (alarm_id == ADX2003_ALARM_ID_B) {
				RTNA_DBG_Str(ADX2003_DEBUG_LEVEL, "Turn-off alram B\r\n");
				status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_RTC_ALARM2_DAY, 0x0);
				if(m_bADX2013 == MMP_FALSE) {
					status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_RTC_INT_CTL, 0x40);	//keep Alarm A on
				}
			}
			else {
				RTNA_DBG_Str(ADX2003_DEBUG_LEVEL, "ADX2003 Alarm Error alarm_ID !!\r\n");
				if (ubRet == 0) {
					MMPF_OS_ReleaseSem(m_RtcAlamSemID);
				}
				return MMP_SYSTEM_ERR_FORMAT;
			}
		}
		else if(m_alarmUse_A == MMP_TRUE) 
		{
			alarm_id = ADX2003_ALARM_ID_A;
			RTNA_DBG_Str(ADX2003_DEBUG_LEVEL, "---------Turn-off alram A\r\n");
			status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_RTC_ALARM1_DAY, 0x0);
			if(m_bADX2013 == MMP_FALSE) {
				status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_RTC_INT_CTL, 0x00);
			}
		}
		else if(m_alarmUse_B == MMP_TRUE) 
		{
			alarm_id = ADX2003_ALARM_ID_B;
			RTNA_DBG_Str(ADX2003_DEBUG_LEVEL, "---------Turn-off alram B\r\n");
			status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_RTC_ALARM2_DAY, 0x0);
			if(m_bADX2013 == MMP_FALSE) {
				status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_RTC_INT_CTL, 0x00);
			}
		}
		else 
		{
			if (ubRet == 0) {
				MMPF_OS_ReleaseSem(m_RtcAlamSemID);
			}
			return MMP_ERR_NONE;
		}
	
		if ((alarm_id == ADX2003_ALARM_ID_A) && (m_alarmUse_A == MMP_TRUE)) {
			m_alarmUse_A = MMP_FALSE;
		}
		else if ((alarm_id == ADX2003_ALARM_ID_B) && (m_alarmUse_B == MMP_TRUE)) {
			m_alarmUse_B = MMP_FALSE;
		}
		else {
			RTNA_DBG_Str(ADX2003_DEBUG_LEVEL, "Error RTC Alarm settings 2 !\r\n");
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
//  Function    : ADX2003_LDO_SetV
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used for setting LDO voltage.

This function is used for setting LDO voltage.
@param[in] ADX2003_LDO_ID is the specific ID number.
@param[in] vLevel is between 0x00~0x3F, one level stands 0.05v, the minimum voltage(level 0) is 0.6V
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_LDO_SetVoltage(ADX2003_LDO_ID ldoID, MMP_USHORT vLevel)
{
	MMP_ERR status = MMP_ERR_NONE;

	vLevel = ADX2003_LDO_EN | (vLevel & ADX2003_LEVEL_BIT);

	status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, (ADX2003_LDO_1_CTL + ldoID), vLevel);
	
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return status;
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_LDO_GetV
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used to get LDO voltage.

This function is used to get LDO voltage.
@param[in] ADX2003_LDO_ID is the specific ID number.
@param[in] returnVol is the return volage value, the unit is mV
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_LDO_GetVoltage(ADX2003_LDO_ID ldoID, MMP_USHORT *returnVoltage)
{
	MMP_ERR status = MMP_ERR_NONE;
	MMP_USHORT voltage = 0;

	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, (ADX2003_LDO_1_CTL + ldoID), &voltage);

	voltage &= ADX2003_LEVEL_BIT;
	*returnVoltage = 600 + voltage*50;
	
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return status;
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_DCDC_SetV
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used for setting DCDC voltage.

This function is used for setting DCDC voltage.
@param[in] ADX2003_DCDC_ID is the specific ID number.
@param[in] vLevel is between 0x00~0x3F, one level stands 0.05v, the minimum voltage(level 0) is 0.8V
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_DCDC_SetVoltage(ADX2003_DCDC_ID dcdcID, MMP_USHORT vLevel) 
{
	MMP_ERR status = MMP_ERR_NONE;

	vLevel = ADX2003_DCDC_BUCK1_EN | (vLevel & ADX2003_LEVEL_BIT);

	status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, (ADX2003_DCDC_1_CTL2 + dcdcID*ADX2003_DCDC_REG_SIZE), vLevel);

	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return status;
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_DCDC_GetVoltage
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used for Getting DCDC voltage.

This function is used for Getting DCDC voltage.
@param[in] ADX2003_DCDC_ID is the specific ID number.
@param[in] returnVol is the return volage value, the unit is mV
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_DCDC_GetVoltage(ADX2003_DCDC_ID dcdcID, MMP_USHORT* returnVoltage) 
{
	MMP_USHORT voltage = 0;
	MMP_ERR status = MMP_ERR_NONE;

	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, (ADX2003_DCDC_1_CTL2 + dcdcID*ADX2003_DCDC_REG_SIZE), &voltage);

	voltage &= ADX2003_LEVEL_BIT;
	*returnVoltage = 800 + voltage*50;

	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_CLASSG_En
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used to enable/disable Class G module(AMP).
@param[in] bEnable is true for enable and flase for disable Cleass G module (AMP).
This function is used to enable/disable AMP.
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_CLASSG_En(MMP_BOOL bEnable)
{
	MMP_ERR status = MMP_ERR_NONE;
	
	if(m_bADX2013 == MMP_FALSE) 
	{
		if (bEnable == MMP_TRUE) {
			status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_CLASSG_HEAD_SET2, 0x0C);
			MMPF_OS_Sleep(100);
			status |= ADX2003_CLASSG_SetVol(0x3A);
		}
		else {
			status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_CLASSG_HEAD_SET2, 0x0F);
			status |= ADX2003_CLASSG_SetVol(0);
		}
		
		if(status != MMP_ERR_NONE) {
			return MMP_SYSTEM_ERR_PMU;
		}
	}
	return status;
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_CLASSG_SetVol
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used to set out volume(in moudule class G ,AMP) as target value.
@param[in] vol_db stands for the volume gain output, 0 stands for mute(silent), 1~78 maps to -57~20 db.
This function is used to set out volume as target value.
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_CLASSG_SetVol(MMP_UBYTE vol_db)
{
	MMP_USHORT currentVol = 0;
	MMP_ERR status = MMP_ERR_NONE;

	if(m_bADX2013 == MMP_FALSE) 
	{
		if (vol_db <= ADX2003_CLASSG_MAX_VOL) 
		{
			status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_CLASSG_VOL_CTL1, &currentVol);
			if(currentVol != vol_db) {
				status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_CLASSG_VOL_CTL1, vol_db);
				status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_CLASSG_VOL_CTL2, vol_db);
			}
		}
		
		if(status != MMP_ERR_NONE) {
			return MMP_SYSTEM_ERR_PMU;
		}
	}
	return status;
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_CheckECOVersion
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used to check if 506 ECO2.
@param[in] bReturnECOVersion is the return flag value, indicate which ECO version or not.
This function is used to check if 506 ECO.
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_CheckECOVersion(MMP_UBYTE *bReturnECOVersion)
{
	MMP_ERR status = MMP_ERR_NONE;
	MMP_USHORT nRevValue = 0;

	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, 0x7D, &nRevValue);

	//check BOOT mode and ADX Revision, if ROM_BOOT && Rev 0x02, then AIT 5062 is ECO_0510,
	//else if ROM_BOOT && Rev 0xFF, then AIT 5062 is ECO_0430, otherwise non ECO
	if(ADX2003_ECO_VER_0510==nRevValue) {
		*bReturnECOVersion = ADX2003_ECO_VER_0510; // ROM BOOT is for ECO_0510
	} else if(ADX2003_ECO_VER_0430==nRevValue) {
		*bReturnECOVersion = ADX2003_ECO_VER_0430; // ROM BOOT is for ECO_0430
	} else {
		*bReturnECOVersion = ADX2003_ECO_VER_NONE;
	}

	return status;
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_Initialize
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used to init ADX2003 driver.

This function is used to init ADX2003 driver.
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_Initialize(void)
{
	MMP_ERR status = MMP_ERR_NONE;
	MMP_UBYTE usVID = 0x0;
	MMP_ULONG i = 0;
	MMP_USHORT ret;
#if  defined(ALL_FW)&&(HARDWARE_PLATFORM == SEC_PLATFORM_PV2)  
	AITPS_GBL pGBL = AITC_BASE_GBL;
	#if (ADX2003_WDT_EN == 0x1)
	//MMP_UBYTE ubTimerID = 0xFF;
	#endif
#endif
	
#if defined(ALL_FW)
#if (USE_ADC_SEMI)
	m_ADCSemID =  MMPF_OS_CreateSem(1);
#endif
#endif

#if (USE_ADX2003_SEMI)
    m_ADX2003SemID = MMPF_OS_CreateSem(1);
#endif

	for(i = 0 ; i < MMPF_OS_TMRID_MAX; i++) {
		m_usTimerID[i] = MMPF_OS_TMRID_MAX;  //initial ID  as  invalid timer id.
	}
	m_usTimerReadIndex = 0;
	m_usTimerWriteIndex = 0;
	
	status |= MMPF_I2cm_Initialize(&gI2cmAttribute_adx2003);
	status |= ADX2003_RTC_Initialize();
	
	#if (ADX2003_ACT_DISCHARGE)
	ADX2003_ActiveDischarge_En(1);
	#endif
	
	#if (ADX2003_EN) && (ADX2003_WDT_EN)
	ADX2003_WDT_Enable(MMP_FALSE);
	#endif
	
	status |= ADX2003_CheckECOVersion(&usVID);
	//MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, 0x7D, &usVID);
	
	if(usVID == ADX2003_ECO_VER_0510) {  //for new version ADX2003 (ADX2013)
		m_bADX2013 = MMP_TRUE;
		m_glAdx2003AccessTime = 0x5;
	}
	
	switch (usVID)
	{
		case ADX2003_ECO_VER_0510:
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_1_CTL1, 0xD2);
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_2_CTL1, 0xD2); // Gerry_chou confirm update!
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_1_CTL2, 0x4B);
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_1_CTL4, 0xFC);
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_1_CTL5, 0x69);
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, 0x76, 0xEF);
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, 0x77, 0x01);
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_2_CTL4, 0xAC);
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_2_CTL5, 0x69);
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, 0x78, 0xEF);
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, 0x79, 0x05);
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_CLASSG_PUMP_SET1, 0x41); 
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_CLASSG_PUMP_SET2, 0x14); 
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, 0x96, 0x0C); 
		//status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_2_CTL1, 0xD2);
		break;
		case ADX2003_ECO_VER_0430:
			//status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_1_CTL1, 0x6A); 
			//status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_1_CTL4, 0xAC); 
			//status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_2_CTL1, 0x6A); 
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_1_CTL4, 0xFC);
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_2_CTL1, 0xD2);
		break;
		case ADX2003_ECO_VER_NONE:
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_1_CTL1, 0x4a);
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_1_CTL4, 0xFC);
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_1_CTL2, 0x4a);
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_2_CTL1, 0x4a /*0x2a*/); //change to FWM mode
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_2_CTL2, 0x54);
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_CURRENT_SET2, 0x34); // Bias Current : 4.5uA, Osillation Current : 2uA
			status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_POWERON_CTL, 0x03);
		break; 
	}
	
	status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_DCDC_2_CTL2, 0x56); // Mark Test DC-DC2 1.9V
	
	#if (ADC_SETTINGS_FOR_EP2 == 1)
	MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003,ADX2003_POWERON_CTL,&ret);
	ret |= 0x10;
	status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_POWERON_CTL, ret); // Mark Test DC-DC2 1.9V
	#endif

	#if (BIND_SENSOR_OV2710 == 1)||(BIND_SENSOR_OV2710_MIPI == 1)
	status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_LDO_2_CTL, 0xD2);
	status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_LDO_3_CTL, 0xD8);
	#endif
	//Default turn off PMU interrupt in P_V2, if customer wish to use, please change 0 to 1
	status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_LDO_3_CTL, 0xED);
	status |= MMPF_I2cm_WriteReg( &gI2cmAttribute_adx2003, ADX2003_LDO_4_CTL, 0xED);
#if defined(ALL_FW)&&(HARDWARE_PLATFORM == SEC_PLATFORM_PV2)  
	  
	//At ALL_FW, we can get the boot interrupt status because in AitBootloader we do not clean interrupt status.
	if (glAdx2003IntStatus == 0) {
		MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_CHIP_INT_STATUS, &glAdx2003IntStatus);
		RTNA_DBG_PrintShort(0, glAdx2003IntStatus);
	}
	
	#if (CHIP == P_V2)
	//Enable the chip get the interrupt signal
	pGBL->GBL_POC_INT_EN = GBL_PMIC_INT_EN; //for P_V2
	#endif
	
	#if (ADX2003_WDT_EN == 0x1)
	//For customer's request, Watch dog is controled by UI
	/*
	status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_WDT_CTL, 0x84);
	ubTimerID = MMPF_OS_StartTimer(m_glAdx2003CleanWdtTime, MMPF_OS_TMR_OPT_PERIODIC, (MMPF_OS_TMR_CALLBACK)&ADX2003_WDT_TimerCallBack, NULL);
	if (ubTimerID >= MMPF_OS_TMRID_MAX) {
		status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_WDT_CTL, 0x04);
	}
	*/
	#endif
#endif 
	
	if (glAdx2003IntStatus == 0) 
	{
		MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_CHIP_INT_STATUS, &glAdx2003IntStatus);
		MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_CHIP_INT_STATUS, &glAdx2003IntStatus);
		MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_CHIP_INT_STATUS, &glAdx2003IntStatus);
		MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_CHIP_INT_STATUS, &glAdx2003IntStatus);
		
		RTNA_DBG_Str(0,"====================\r\n");
		RTNA_DBG_Str(0,"2013 Status : ");
		RTNA_DBG_Short(0,glAdx2003IntStatus);
		RTNA_DBG_Str(0,"\r\n");
		RTNA_DBG_Str(0,"====================\r\n");
		glAdx2003IntStatus &= ~0xEF;
		status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_CHIP_INT_STATUS,glAdx2003IntStatus);
		MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_CHIP_INT_STATUS, &glAdx2003IntStatus);
	}
		
	#if (ADX2003_ACT_DISCHARGE)
	ADX2003_ActiveDischarge_En(0);
	#endif
	
	if(status != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_PMU;
	}
	return status;
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_IsrHandler
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used to handle the interrupt of ADX2003.

This function is used to handle the interrupt of ADX2003.
@return It reports the status of the operation.
*/
void ADX2003_IsrHandler(void) 
{
	ADX2003_ALARM_ID alarm_id = ADX2003_ALARM_ID_NONE;
	AITPS_GBL pGBL = AITC_BASE_GBL;
	MMP_USHORT usAdx2003IntStatus = 0, usCleanStatus;
	MMP_UBYTE  ubRet = 0xFF;
	
	MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_CHIP_INT_STATUS, &usAdx2003IntStatus);
	usCleanStatus = usAdx2003IntStatus;
	
	if(m_bADX2013 == MMP_FALSE) {
		MMPF_OS_Sleep(8);
	}
	
	while(usAdx2003IntStatus != 0) 
	{
		if ((usAdx2003IntStatus & (1 << ADX2003_INT_EVENT_WAKE)) != 0) 
		{
			usAdx2003IntStatus &= (~(1 << ADX2003_INT_EVENT_WAKE));
			RTNA_DBG_Str(ADX2003_DEBUG_LEVEL, "ADX2003 Wake up Event !!\r\n");
		}
		else if((usAdx2003IntStatus & (1 << ADX2003_INT_EVENT_RTC)) != 0) 
		{
			ubRet = MMPF_OS_AcquireSem(m_RtcAlamSemID, ADX2003_SEM_TIMEOUT);
		
			if(m_bADX2013 == MMP_FALSE) 
			{ 	//for ADX2003
				if((m_alarmUse_A == MMP_TRUE)&&(m_alarmUse_B == MMP_TRUE)) {
					ADX2003_RTC_AlarmCheckID(&alarm_id);
				}
				else if(m_alarmUse_A == MMP_TRUE) {
					alarm_id = ADX2003_ALARM_ID_A;
				}
				else if(m_alarmUse_B == MMP_TRUE) {
					alarm_id = ADX2003_ALARM_ID_B;
				}
				else {
					//Because the Alarm continue about  1 minutes, althought we can stop it during 10 second,
					//But in this 10 s, we will continue receive the Alarm interrupt
					MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_CHIP_INT_STATUS, usCleanStatus);
					
					m_usTimerID[m_usTimerWriteIndex] = MMPF_OS_StartTimer(m_glAdx2003AccessTime, MMPF_OS_TMR_OPT_ONE_SHOT, (MMPF_OS_TMR_CALLBACK)&ADX2003_IsrTimerCallBack, NULL);
				
					if (m_usTimerID[m_usTimerWriteIndex] >= MMPF_OS_TMRID_MAX) {
						RTNA_DBG_Str(0, "Error !! : ADX2003 Get OS Timer error !\r\rn");
						MMPF_OS_Sleep(m_glAdx2003AccessTime);
						#if (CHIP == P_V2)
						pGBL->GBL_POC_INT_EN = GBL_PMIC_INT_EN;
						#endif
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
			else 
			{	//for ADX2012
				if(m_alarmUse_A == MMP_TRUE) {
					alarm_id = ADX2003_ALARM_ID_A;
				}
				else {
					//Because the Alarm continue about  1 minutes, althought we can stop it during 10 second,
					//But in this 10 s, we will continue receive the Alarm interrupt
					MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_CHIP_INT_STATUS, usCleanStatus);
					
					m_usTimerID[m_usTimerWriteIndex] = MMPF_OS_StartTimer(m_glAdx2003AccessTime, MMPF_OS_TMR_OPT_ONE_SHOT, (MMPF_OS_TMR_CALLBACK)&ADX2003_IsrTimerCallBack, NULL);
					
					if (m_usTimerID[m_usTimerWriteIndex] >= MMPF_OS_TMRID_MAX) {
						RTNA_DBG_Str(0, "Error !! : ADX2003 Get OS Timer error !\r\rn");
						MMPF_OS_Sleep(m_glAdx2003AccessTime);
						#if (CHIP == P_V2)
						pGBL->GBL_POC_INT_EN = GBL_PMIC_INT_EN;
						#endif
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
			ADX2003_RTC_SetAlarmEnable(alarm_id, MMP_FALSE, NULL, NULL);
			
			if (m_RtcCallBack[alarm_id - 1] != NULL) {
				(*m_RtcCallBack[alarm_id - 1]) ();
			}
			
			//MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_CHIP_INT_EN, 0x1E);
			//MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_CHIP_INT_STATUS, 0x2);
			
			if(m_bADX2013 == MMP_FALSE) {
				MMPF_OS_Sleep(1000);
			}
			usAdx2003IntStatus &= (~(1 << ADX2003_INT_EVENT_RTC));
		}
		else if((usAdx2003IntStatus & (1 << ADX2003_INT_EVENT_USBJIG)) != 0) 
		{
			usAdx2003IntStatus &= (~(1 << ADX2003_INT_EVENT_USBJIG));
			RTNA_DBG_Str(ADX2003_DEBUG_LEVEL, "ADX2003 USB JIG Event !!\r\n");
		}
		else if((usAdx2003IntStatus & (1 << ADX2003_INT_EVENT_VBUS)) != 0) 
		{
			usAdx2003IntStatus &= (~(1 << ADX2003_INT_EVENT_VBUS));
			RTNA_DBG_Str(ADX2003_DEBUG_LEVEL, "ADX2003 USB VBUS event !!\r\n");
		}
		else if((usAdx2003IntStatus & (1 << ADX2003_INT_EVENT_POWERK)) != 0) 
		{
			usAdx2003IntStatus &= (~(1 << ADX2003_INT_EVENT_POWERK));
			RTNA_DBG_Str(ADX2003_DEBUG_LEVEL, "ADX2003 Power Key event !!\r\n");
		}
		else if((usAdx2003IntStatus & (1 << ADX2003_INT_EVENT_RTC2)) != 0) 
		{
			ubRet = MMPF_OS_AcquireSem(m_RtcAlamSemID, ADX2003_SEM_TIMEOUT);
			
			if(m_alarmUse_B == MMP_TRUE) {
				alarm_id = ADX2003_ALARM_ID_B;
			}
			else {
				//Because the Alarm continue about  1 minutes, althought we can stop it during 10 second,
				//But in this 10 s, we will continue receive the Alarm interrupt
				MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_CHIP_INT_STATUS, usCleanStatus);
				
				m_usTimerID[m_usTimerWriteIndex] = MMPF_OS_StartTimer(m_glAdx2003AccessTime, MMPF_OS_TMR_OPT_ONE_SHOT, (MMPF_OS_TMR_CALLBACK)&ADX2003_IsrTimerCallBack, NULL);
				
				if (m_usTimerID[m_usTimerWriteIndex] >= MMPF_OS_TMRID_MAX) {
					RTNA_DBG_Str(0, "Error !! : ADX2003 Get OS Timer error !\r\rn");
					MMPF_OS_Sleep(m_glAdx2003AccessTime);
					#if (CHIP == P_V2)
					pGBL->GBL_POC_INT_EN = GBL_PMIC_INT_EN;
					#endif
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
			ADX2003_RTC_SetAlarmEnable(alarm_id, MMP_FALSE, NULL, NULL);
			
			if (m_RtcCallBack[alarm_id - 1] != NULL) {
				(*m_RtcCallBack[alarm_id - 1]) ();
			}
			
			//MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_CHIP_INT_EN, 0x1E);
			//MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_CHIP_INT_STATUS, 0x2);
			
			if(m_bADX2013 == MMP_FALSE) {
				MMPF_OS_Sleep(1000);
			}
			usAdx2003IntStatus &= (~(1 << ADX2003_INT_EVENT_RTC2));
		}
		else 
		{
			RTNA_DBG_Str(ADX2003_DEBUG_LEVEL, "ADX2003 Int processing error !\r\n");
			RTNA_DBG_PrintShort(0, usAdx2003IntStatus);
			return;
		}
	} //while(usRegValue != 0)
	
	MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_CHIP_INT_STATUS, usCleanStatus);
	
	m_usTimerID[m_usTimerWriteIndex] = MMPF_OS_StartTimer(m_glAdx2003AccessTime, MMPF_OS_TMR_OPT_ONE_SHOT, (MMPF_OS_TMR_CALLBACK)&ADX2003_IsrTimerCallBack, NULL);

	if (m_usTimerID[m_usTimerWriteIndex] >= MMPF_OS_TMRID_MAX) {
		RTNA_DBG_Str(0, "Error !! : ADX2003 Get OS Timer error !\r\rn");
		MMPF_OS_Sleep(m_glAdx2003AccessTime);
		#if (CHIP == P_V2)
		pGBL->GBL_POC_INT_EN = GBL_PMIC_INT_EN;
		#endif
	}
	else {
		m_usTimerWriteIndex = ((m_usTimerWriteIndex + 1) % MMPF_OS_TMRID_MAX);
	}
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_ADC_Measure
//  Description :
//------------------------------------------------------------------------------
/** @brief The function return the measured voltage.
The function return the measured voltage.
@param[out] level is the return value of voltage (unit: mV)
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_ADC_Measure(MMP_USHORT *level)
{
	MMP_ULONG timeout = 0;
	MMP_USHORT get_val = 0;
	MMP_USHORT get_measured_value_low = 0;
 	MMP_USHORT get_measured_value_high = 0;
 	MMP_USHORT VDD_LDO4 = 0;
 	MMP_USHORT voltage = 0; // unit : mV

#if (USE_ADC_SEMI)
    if(MMPF_OS_AcquireSem(m_ADCSemID, 0)) 
    {
		RTNA_DBG_Str(0, "m_ADCSemID OSSemPost: Fail!! \r\n");
		return 1;
	}
#endif

#if (USE_ADX2003_SEMI)
    if(MMPF_OS_AcquireSem(m_ADX2003SemID, 0)) 
    {
		RTNA_DBG_Str(0, "m_ADX2003SemID OSSemPost: Fail!! \r\n");
		return 1;
	}
#endif

 	//Enable Battery Measure
	MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_POWERON_CTL, 0x14);
	MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_7, 0x01);
	
	timeout = 0;
	
	do
	{
		MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_TP_7, &get_val);
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
		
#if (USE_ADC_SEMI)
    MMPF_OS_ReleaseSem(m_ADCSemID);
#endif		
#if (USE_ADX2003_SEMI)
    	MMPF_OS_ReleaseSem(m_ADX2003SemID);
#endif

		return MMP_SYSTEM_ERR_PMU;			
	}
	
	MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_TP_8, &get_measured_value_low);
	MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_TP_9, &get_measured_value_high);

	voltage = ((get_measured_value_high & 0x03) << 8) | get_measured_value_low;
	
	ADX2003_LDO_GetVoltage(ADX2003_LDO_ID_4, &VDD_LDO4);

	voltage = (voltage*3*VDD_LDO4)/1024;
	*level = voltage;
	
	//Disable Battery Measure
	MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_POWERON_CTL, 0x03);

#if (USE_ADC_SEMI)
    MMPF_OS_ReleaseSem(m_ADCSemID);
#endif
#if (USE_ADX2003_SEMI)
	MMPF_OS_ReleaseSem(m_ADX2003SemID);
#endif

	return MMP_ERR_NONE;
}

MMP_ERR ADX2003_ADC_EnableAVDDCheck(MMP_UBYTE enable)
{
    MMP_USHORT get_val=0;

#if (USE_ADC_SEMI)
    if(MMPF_OS_AcquireSem(m_ADCSemID, 0)) 
    {
		RTNA_DBG_Str(0, "m_ADCSemID OSSemPost: Fail!! \r\n");
		return 1;
	}
#endif

    MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_POWERON_CTL, &get_val);        
            
    if(enable){
        MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_POWERON_CTL, get_val | 0x07);
    }
    else{
        MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_POWERON_CTL, get_val & (~0x07));
    }
    
#if (USE_ADX2003_SEMI)
	MMPF_OS_ReleaseSem(m_ADX2003SemID);
#endif
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_ADC_Measure_key
//  Description :
//------------------------------------------------------------------------------
/** @brief The function return the measured voltage.
The function return the measured voltage.
@param[out] level is the return value of voltage (unit: mV)
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_ADC_Measure_Key( MMP_ULONG *level, MMP_UBYTE *source, ADX2003_ADC_TYPE type)
{
	MMP_ULONG  timeout = 0,voltage = 0,KeyVoltage = 0;
	MMP_USHORT get_val = 0;
	MMP_USHORT get_measured_value_low = 0,get_measured_value_high = 0,ADX_Value = 0;
	MMP_UBYTE  adc_sel;
	
#if defined(ALL_FW)
#if (USE_ADC_SEMI)
    if(MMPF_OS_AcquireSem(m_ADCSemID, 0)) 
    {
		RTNA_DBG_Str(0, "m_ADCSemID OSSemPost: Fail!! \r\n");
		return 1;
	}
#endif
#endif

#if (USE_ADX2003_SEMI)
    if(MMPF_OS_AcquireSem(m_ADX2003SemID, 0)) 
    {
		RTNA_DBG_Str(0, "m_ADX2003SemID OSSemPost: Fail!! \r\n");
		return 1;
	}
#endif

	switch(type)
	{
		case ADX2003_ADC_AUX1:
		adc_sel = 0x10;
		//Enable Battery Measure
	    MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_POWERON_CTL, &get_val);
	    MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_POWERON_CTL, get_val | 0x10);
		break;
		case ADX2003_ADC_AUX2:
		adc_sel = 0x20;
		break;
		case ADX2003_ADC_WIPER:
		adc_sel = 0x40;
		break;
		case ADX2003_ADC_TP_PRE:
		adc_sel = 0x08;
		break;
		case ADX2003_ADC_X_CO:
		adc_sel = 0x04;
		break;
		case ADX2003_ADC_Y_CO:
		adc_sel = 0x02;
		break;
	}
	//MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_POWERON_CTL, 0x07); //Mark test
	MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_2, 0xB1);	
	MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_5, 0x8C);  //Enable ADC	
	MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_1, adc_sel); 
	MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_6, 0x01); 
	
	//MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_DCDC_1_CTL1, &get_val);
	//MMPF_OS_Sleep(2);
	
	timeout = 0;
	do
	{
		MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_TP_6, &get_val);
		timeout ++;		
		
		get_val &= 0x03;
		
	}while(((get_val&0x02) != 0x00) && (timeout < 6));
	
	if(timeout>=6)
	{
		#if defined(ALL_FW)
		printc("TIMEOUT! read 2003 [0x%02X]=(0x%02X) type : %d\n\r", ADX2003_TP_6, get_val,type);
		#endif
	}	
		
	MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_TP_8, &get_measured_value_low);
	MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_TP_9, &get_measured_value_high);	
	*source = 0;
	*source |= (get_measured_value_high & 0x70) >> 4;
		
	//Transfer voltage to key
	voltage = ((get_measured_value_high & 0x03) << 8) | get_measured_value_low;	
	KeyVoltage =(MMP_ULONG)(((MMP_ULONG64)voltage*ADX2003_ADC_VOLTAGE)/1024); //mV
	*level = (MMP_ULONG)KeyVoltage;
	
    // Disable Keypad ADC measure   
	MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_TP_1, &ADX_Value);
	ADX_Value &= ~adc_sel;
	
	MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_1, ADX_Value);
	MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_5, 0x80);
	
	if(ADX2003_ADC_AUX1 == type)
    {
		//Disable Battery Measure
	    MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_POWERON_CTL, &get_val);
	    MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_POWERON_CTL, get_val & (~0x10));
	}
	//MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_POWERON_CTL, 0x03); //Mark test
	
#if defined(ALL_FW)
#if (USE_ADC_SEMI)
    MMPF_OS_ReleaseSem(m_ADCSemID);
#endif
#endif
   
#if (USE_ADX2003_SEMI)
    MMPF_OS_ReleaseSem(m_ADX2003SemID);
#endif

	return MMP_ERR_NONE;
}

#if 0
//------------------------------------------------------------------------------
//  Function    : ADX2003_ADC_Measure_key
//  Description :
//------------------------------------------------------------------------------
/** @brief The function return the measured voltage.
The function return the measured voltage.
@param[out] level is the return value of voltage (unit: mV)
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_ADC_Measure_WIPER( MMP_ULONG *level, MMP_UBYTE *source)
{
	MMP_ULONG  timeout = 0,voltage = 0,KeyVoltage = 0;
	MMP_USHORT get_val = 0;
	MMP_USHORT get_measured_value_low = 0,get_measured_value_high = 0,ADX_Value = 0;
	//MMP_USHORT ADC_Not_Stable = 0;
	
	//MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_POWERON_CTL, 0x14); //Battery
	MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_2, 0xB1);
	//MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_TP_2, &get_val);
	MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_5, 0x8C);  //Enable ADC
	//MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_TP_5, &get_val);
	//MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_1, 0x60);  //aux2,WIPER
	MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_1, 0x40);  //WIPER
	//MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_1, 0x20);  //aux2
	//MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_1, 0x10);  //aux1
	//MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_TP_1, &get_val);
	MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_6, 0x01); 
	//MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_DCDC_2_CTL2, &get_val);

	timeout = 0;

	do
	{
		MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_TP_6, &get_val);
		timeout ++;		
		
		get_val &= 0x03;
		
	}while(((get_val&0x02) != 0x00) && (timeout < 6));
	
	if(timeout>=6)
	{
		#if defined(ALL_FW)
		printc("TIMEOUT! read 2003 [0x%02X]=(0x%02X) ADX2003_ADC_Measure()\n\r", ADX2003_TP_6, get_val);
		#endif
	}
	
	MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_TP_8, &get_measured_value_low);
	MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_TP_9, &get_measured_value_high);

	*source = 0;
	*source |= (get_measured_value_high & 0x70) >> 4;

	//Transfer voltage to key
	voltage = ((get_measured_value_high & 0x03) << 8) | get_measured_value_low;	
	KeyVoltage =(MMP_ULONG)(((MMP_ULONG64)voltage*ADX2003_ADC_VOLTAGE)/1024); //mV
	*level = (MMP_ULONG)KeyVoltage;
	
    // Disable Keypad ADC measure   
	MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_TP_1, &ADX_Value);
	
	//ADX_Value &= 0xDF; //AUX2
	//ADX_Value &= 0x9F;   //AUX2,WIPER
	ADX_Value &= 0xBF; //WIPER
	
	MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_1, ADX_Value);
	MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_5, 0x80);
	
	return MMP_ERR_NONE;
}
#endif

//------------------------------------------------------------------------------
//  Function    : ADX2003_ADC_Measure_Lens_Temp
//  Description :
//------------------------------------------------------------------------------
/** @brief The function return the measured voltage.
The function return the measured voltage.
@param[out] level is the return value of voltage (unit: mV)
@return It reports the status of the operation.
*/
MMP_ERR ADX2003_ADC_Measure_Lens_Temp( MMP_ULONG *level, MMP_UBYTE *source)
{
	#define ADX2003_ADC_TEMP_VOLTAGE 3300
	MMP_ULONG  timeout = 0,voltage = 0,KeyVoltage = 0;
	MMP_USHORT get_val = 0;
	MMP_USHORT get_measured_value_low = 0,get_measured_value_high = 0,ADX_Value = 0;
	MMP_ERR		status = MMP_ERR_NONE;

#if (USE_ADC_SEMI)
    if(MMPF_OS_AcquireSem(m_ADCSemID, 0)) 
    {
		#if defined(ALL_FW)
		printc("m_ADCSemID OSSemPost: Fail!! \r\n");
		#endif
		return 1;
	}
#endif

	//MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_POWERON_CTL, 0x14); //Battery
	status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_2, 0xB1);
	//MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_TP_2, &get_val);
	status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_5, 0x8C);  //Enable ADC
	//MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_TP_5, &get_val);
	
	#if (ADC_SETTINGS_FOR_EP2 == 1)
	status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_1, 0x40);  // wiper
	#else
	status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_1, 0x02);  // XP-BR
	#endif
	
	//MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_1, 0x40);  //WIPER
	//MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_1, 0x20);  //aux2
	//MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_1, 0x10);  //aux1
	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_TP_1, &get_val);

	status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_6, 0x01); 
	//MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_CHIP_INT_STATUS, &get_val);

	timeout = 0;

	do
	{
		status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_TP_6, &get_val);
		timeout ++;		
		
		get_val &= 0x03;
		
	}while(((get_val&0x02) != 0x00) && (timeout < 6));
	
	if(timeout>=6)
	{
		#if defined(ALL_FW)
		printc("TIMEOUT! read 2003 [0x%02X]=(0x%02X) ADX2003_ADC_Measure()\n\r", ADX2003_TP_6, get_val);
		#endif
		#if (USE_ADC_SEMI)
    	MMPF_OS_ReleaseSem(m_ADCSemID);
		#endif
		
		return MMP_SYSTEM_ERR_CMDTIMEOUT;
	}
	
	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_TP_8, &get_measured_value_low);
	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_TP_9, &get_measured_value_high);

	*source = 0;
	*source |= (get_measured_value_high & 0x70) >> 4;
	
	//Transfer digital value to voltage  
	voltage = ((get_measured_value_high & 0x03) << 8) | get_measured_value_low;	
	KeyVoltage =(MMP_ULONG)(((MMP_ULONG64)voltage*ADX2003_ADC_TEMP_VOLTAGE)/1024); //mV
	*level = (MMP_ULONG)KeyVoltage;
	
    // Disable ADC measure   
	status |= MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_TP_1, &ADX_Value);
	//ADX_Value &= 0xDF; //AUX2
	//ADX_Value &= 0x9F;   //AUX2,WIPER
	
	#if (ADC_SETTINGS_FOR_EP2 == 1)
	ADX_Value &= ~0x40;
	#else
	ADX_Value &= 0xFD; //
	#endif
		
	//ADX_Value &= 0xBF; //WIPER
	status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_1, ADX_Value);
	status |= MMPF_I2cm_WriteReg(&gI2cmAttribute_adx2003, ADX2003_TP_5, 0x80);

#if (USE_ADC_SEMI)
    MMPF_OS_ReleaseSem(m_ADCSemID);
#endif

	return status;
}



MMP_USHORT ADX2003_Measure_Power_Key(void)
{
	MMP_USHORT get_val;
	MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_CHIP_INT_STATUS, &get_val);
	return get_val;
}

//------------------------------------------------------------------------------
//  Function    : ADX2003_GetIntPinState
//  Description : Get the status of the 2003 inttrupt from the cpu side.
//------------------------------------------------------------------------------
/** @brief The function return the measured voltage.
Get the status of the 2003 inttrupt from the cpu side.

@param[in] bGetNewData - 
        TRUE  - Getting the new data from the register.
        FALSE - Using the previous data keeped in memory.
@return The value of the status.
*/
MMP_UBYTE ADX2003_GetIntPinState(MMP_BOOL bGetNewData )
{
    #define REG_POC_INT_STATUS_FOR_CPU    (0x80008F85)
    
	static MMP_UBYTE ubIntState = 0x0;
    MMP_UBYTE * pIntData;

    if( bGetNewData )
    {
        pIntData = (MMP_UBYTE*)REG_POC_INT_STATUS_FOR_CPU;
        ubIntState = *pIntData;        
    }

    return ubIntState;
}
void ADX2003_BootUpCheck(void) 
{
	MMP_ERR status = MMP_ERR_NONE;
	MMP_USHORT ReadBack;
	MMP_UBYTE i = 0;
	status |= MMPF_I2cm_Initialize(&gI2cmAttribute_adx2003);
	
	for(i = 0; i< 5; i++)
	MMPF_I2cm_ReadReg(&gI2cmAttribute_adx2003, ADX2003_CHIP_INT_STATUS, &ReadBack);
	
	RTNA_DBG_Str(0,"====================\r\n");
	RTNA_DBG_Str(0,"2013 Status : ");
	RTNA_DBG_Short(0,ReadBack);
	RTNA_DBG_Str(0,"\r\n");
	RTNA_DBG_Str(0,"====================\r\n");
	if(ReadBack == 0x00)
	{
		RTNA_DBG_Str(0, "Abnormal Power on. Turn off Power\r\n");
		status |= ADX2003_PowerOff();
	}
	
}
#endif //#if (ADX2003_EN)

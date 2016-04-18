//******************************************************************************
//
//  File        : PMUCtrl.c
//  Description : Power management unit() Control 
//  Author      : .
//  Revision    : 1.0
//
//******************************************************************************

#include "Customer_config.h"
#include "config_fw.h"
#include "PMUCtrl.h"
#include "mmpf_i2cm.h"
#include "mmpf_pio.h"
#include "mmpf_saradc.h"
#include "ahc_saradc.h"

#if (ADX2003_EN)
#include "ControlIC_ADX2003.h"
#elif (ADX2015_EN)
#include "ControlIC_ADX2015.h"
#endif

#include "lib_retina.h"
#include "AHC_General_CarDV.h"
//******************************************************************************
//
//                              FUNCTION PROTOTYPES
//
//******************************************************************************

#if !defined(MINIBOOT_FW)
extern MMP_ERR MMPF_SARADC_SetMeasMode(MMPF_SARADC_DELAY_CLOCK DEL, MMPF_SARADC_MEA_RATE CR, MMP_BOOL TPWaitenable);

//******************************************************************************
//
//                              Function API
//
//******************************************************************************
void PMUCtrl_ADC_Measure_Key( MMP_ULONG *level, MMP_UBYTE *source, PMU_ADC_TYPE type)
{
#if (ADX2003_EN)
	ADX2003_ADC_Measure_Key( level, source, (ADX2003_ADC_TYPE)type);
#elif (ADX2015_EN)
    // NOP
#else
    MMP_UBYTE maxTimes = 10;

    while (maxTimes--)
    {
        MMP_USHORT saradc_data;

        if (AHC_TRUE == AHC_SARADC_ReadData(type, &saradc_data)) {
            *level = saradc_data;
            return;
        }
        
        MMPF_OS_Sleep_MS(20);
    }
#endif
}

void PMUCtrl_Power_Gpio_En(MMP_BOOL bGPIOEnable) 
{
	if (POWER_EN_GPIO != MMP_GPIO_MAX) {
        MMPF_PIO_PadConfig(POWER_EN_GPIO, PAD_OUT_DRIVING(0), MMP_TRUE);
		MMPF_PIO_EnableOutputMode(POWER_EN_GPIO, MMP_TRUE, MMP_TRUE);

		#ifdef POWER_EN_GPIO_LEVEL
		if(bGPIOEnable) {
			MMPF_PIO_SetData(POWER_EN_GPIO, POWER_EN_GPIO_LEVEL, MMP_TRUE);
		}
		else {
			MMPF_PIO_SetData(POWER_EN_GPIO, !POWER_EN_GPIO_LEVEL, MMP_TRUE);
		}
		#else
		MMPF_PIO_SetData(POWER_EN_GPIO, bGPIOEnable, MMP_TRUE);
		#endif
	}
}	

void PMUCtrl_Power_Off(void)
{
	RTNA_DBG_Str(0, "PMUCtrl_Power_Off ...\r\n");

	#ifdef USB_OUT_PWR_SW_PIN
	if (USB_OUT_PWR_SW_PIN != MMP_GPIO_MAX){
        MMPF_PIO_PadConfig(USB_OUT_PWR_SW_PIN, PAD_OUT_DRIVING(0), MMP_TRUE);
	    MMPF_PIO_SetData(USB_OUT_PWR_SW_PIN, ((GPIO_HIGH == USB_OUT_PWR_SW_PIN_ACTIVE) ? GPIO_LOW : GPIO_HIGH), MMP_TRUE);
	    MMPF_PIO_EnableOutputMode(USB_OUT_PWR_SW_PIN, MMP_TRUE, MMP_TRUE);
	}
	#endif

#if defined(ALL_FW) && defined(CFG_CUS_POWER_OFF_CB)
    CFG_CUS_POWER_OFF_CB();
#endif

#if (SUPPORT_GSENSOR && POWER_ON_BY_GSENSOR_EN && defined(ALL_FW))
    AHC_Gsensor_SetPowerOnIntThreshold();
#endif  // POWER_ON_BY_GSENSOR_EN

#if (ADX2003_EN)
	#if (PMU_LDO1_FOR_SENSOR == 1)
	PMUCtrl_Enable_LDO(PMU_LDO_ID_1,MMP_FALSE);
	#endif
	ADX2003_PowerOff();
#elif (ADX2015_EN)
	ADX2015_PowerOff();
#else
	#ifdef PMU2206_CHARGER_STATUS 
    //PMUCtrl_Write_Reg(0x09, 0x04);
    PMUCtrl_Write_Reg(0x08, 0x12);
	#endif
	RTNA_DBG_Str(0, "Non AIT PMU device!\r\n");

#if !defined(MINIBOOT_FW)    
    PMUCtrl_Power_Gpio_En(MMP_FALSE);
#endif

    while (1) {
        #if (SUPPORT_GSENSOR && POWER_ON_BY_GSENSOR_EN && defined(ALL_FW))
        extern AHC_BOOL AHC_Gsensor_GetCurIntStatus(AHC_BOOL* bObjMove);
        AHC_BOOL bObjMove;
        AHC_Gsensor_GetCurIntStatus(&bObjMove);
		MMPF_OS_SleepMs(100);
        #endif  // POWER_ON_BY_GSENSOR_EN
    }
#endif
}

#endif

void PMUCtrl_Write_Reg(MMP_UBYTE ubReg, MMP_UBYTE ubValue)
{
#if (ADX2003_EN)
	ADX2003_Write_Reg((ADX2003_REG)ubReg,ubValue);
#elif (ADX2015_EN)
	ADX2015_write_reg((ADX2015_REG)ubReg,ubValue);
#elif(CUSTOMER_PMU_EN)
	{
		static MMP_I2CM_ATTR gI2cmAttribute_PMIC = {
			MMP_I2CM0,			//i2cmID
			0x12,				//ubSlaveAddr
			8, 					//ubRegLen
			8, 					//ubDataLen
			0x0, 				//ubDelayTime
			MMP_FALSE, 			//bDelayWaitEn
			MMP_TRUE, 			//bInputFilterEn
			MMP_FALSE, 			//b10BitModeEn
			MMP_FALSE, 			//bClkStretchEn
			0, 					//ubSlaveAddr1
			0, 					//ubDelayCycle
			2, 					//ubPadNum
			250000 /*250KHZ*/, 	//ulI2cmSpeed
			MMP_TRUE, 			//bOsProtectEn
			NULL, 				//sw_clk_pin
			NULL, 				//sw_data_pin
			MMP_FALSE,			//bRfclModeEn 
			MMP_FALSE,			//bWfclModeEn
			MMP_FALSE,			//bRepeatModeEn
			0                   //ubVifPioMdlId
		};
		
		MMPF_I2cm_WriteReg(&gI2cmAttribute_PMIC, ubReg, ubValue);
	}
#else
	RTNA_DBG_Str(0, "No PMU device!\r\n");
#endif
}
void PMUCtrl_Read_Reg(MMP_UBYTE ubReg, MMP_USHORT* ubValue)
{
#if (ADX2003_EN)
	ADX2003_Read_Reg((ADX2003_REG)ubReg,ubValue);
#elif (ADX2015_EN)
	ADX2015_read_reg((ADX2015_REG)ubReg,ubValue);
#elif(CUSTOMER_PMU_EN)
	{
		static MMP_I2CM_ATTR gI2cmAttribute_PMIC = {
			MMP_I2CM0,			//i2cmID
			0x12,				//ubSlaveAddr
			8, 					//ubRegLen
			8, 					//ubDataLen
			0x0, 				//ubDelayTime
			MMP_FALSE, 			//bDelayWaitEn
			MMP_TRUE, 			//bInputFilterEn
			MMP_FALSE, 			//b10BitModeEn
			MMP_FALSE, 			//bClkStretchEn
			0, 					//ubSlaveAddr1
			0, 					//ubDelayCycle
			2, 					//ubPadNum
			250000 /*250KHZ*/, 	//ulI2cmSpeed
			MMP_TRUE, 			//bOsProtectEn
			NULL, 				//sw_clk_pin
			NULL, 				//sw_data_pin
			MMP_FALSE,			//bRfclModeEn 
			MMP_FALSE,			//bWfclModeEn
			MMP_TRUE,           //bRepeatModeEn
		    0                   //ubVifPioMdlId
		};
		
		MMPF_I2cm_ReadReg(&gI2cmAttribute_PMIC, ubReg, ubValue);
	}
#else
	RTNA_DBG_Str(0, "No PMU device!\r\n");
#endif
}

void PMUCtrl_Enable_LDO(PMU_LDO_ID ldoID, MMP_BOOL bEnable)
{
#if (ADX2003_EN)
	ADX2003_EnableLDO((ADX2003_LDO_ID)ldoID, bEnable);
#elif (ADX2015_EN)
	ADX2015_EnableLDO((ADX2015_LDO_ID)ldoID, bEnable);
#endif
}

MMP_ERR PMUCtrl_Initialize(void)
{
	MMP_ERR returnvalue;

	RTNA_DBG_Str(0, "PMUCtrl_Initialize()\r\n");

#if (ADX2003_EN)
	ADX2003_Read_Reg((ADX2003_REG)ubReg,ubValue);
#elif (ADX2015_EN)
	ADX2015_read_reg((ADX2015_REG)ubReg,ubValue);
#elif (CUSTOMER_PMU_EN)
	{
		// temprarily applied for MiVue6xx, TBD
		#ifdef CUSTOMER_PMU_SELECTION
			#if (CUSTOMER_PMU_SELECTION == PMU_2206)
			PMUCtrl_Write_Reg(0x07, 0x4B);
			PMUCtrl_Write_Reg(0x08, 0x10);
			PMUCtrl_Write_Reg(0x0C, 0xC4);
			//PMUCtrl_Write_Reg(0x00, 0x9A);
			PMUCtrl_Write_Reg(0x00, 0x92);
			PMUCtrl_Write_Reg(0x07, 0x43);
			PMUCtrl_Write_Reg(0x03, 0xFF);
			PMUCtrl_Write_Reg(0x06, 0x0C);
			PMUCtrl_Write_Reg(0x02, 0x60);
			PMUCtrl_Write_Reg(0x0B, 0xA6);
			PMUCtrl_Write_Reg(0x0C, 0x04);
			//PMUCtrl_Write_Reg(0x04, 0xA2); //Set CH2 to 3.18V for Test.
			#elif(CUSTOMER_PMU_SELECTION == PMU_2207) 
			PMUCtrl_Write_Reg(0x00, 0x1A);//Force PWM mode for CH10.
			PMUCtrl_Write_Reg(0x01, 0x00);//Force PWM mode.
			PMUCtrl_Write_Reg(0x02, 0x00);//MODE CH6-CH9:LDO mode
			PMUCtrl_Write_Reg(0x06, 0xB5);//CH6:2.8V; CH7:1.8V
			PMUCtrl_Write_Reg(0x07, 0x28);//CH8:1.2V; CH9:2.8V
			PMUCtrl_Write_Reg(0x05, 0xFF);//CH5:3.3V, Enable CH6-9
			PMUCtrl_Write_Reg(0x01, 0x02);//Force PWM mode. Enable CH5
			//PMUCtrl_Write_Reg(0x00, 0x9A);//Enable CH10.

			#endif
		#else
			PMUCtrl_Write_Reg(0x07, 0x4B);
			PMUCtrl_Write_Reg(0x08, 0x10);
			PMUCtrl_Write_Reg(0x0C, 0xC4);
			//PMUCtrl_Write_Reg(0x00, 0x9A);
			PMUCtrl_Write_Reg(0x00, 0x92);
			PMUCtrl_Write_Reg(0x07, 0x43);
			PMUCtrl_Write_Reg(0x03, 0xFF);
			PMUCtrl_Write_Reg(0x06, 0x0C);
			PMUCtrl_Write_Reg(0x02, 0x60);
			PMUCtrl_Write_Reg(0x0B, 0xA6);
			PMUCtrl_Write_Reg(0x0C, 0x04);
		#endif
		
#ifdef PMU2206_CHARGER_STATUS 
		PMUCtrl_Write_Reg(0x08, 0x02); // temporarily applied for SP86/SP86G, TBD
        PMUCtrl_Write_Reg(0x09, 0x00);  
#endif		
	}
#else
	RTNA_DBG_Str(0, "No PMU device!\r\n");
#endif

	return returnvalue;
}

void PMUCtrl_Power_Off_For_Exception(void)
{
#if !defined(MINIBOOT_FW)

#ifdef CFG_CUS_EXCEPTION_POWER_OFF
    CFG_CUS_EXCEPTION_POWER_OFF();
#else
	#ifdef PMU2206_CHARGER_STATUS 
    //PMUCtrl_Write_Reg(0x09, 0x04);
    PMUCtrl_Write_Reg(0x08, 0x12);
	#endif
    PMUCtrl_Power_Gpio_En(MMP_FALSE);
#endif

#endif
}

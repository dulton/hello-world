//==============================================================================
//
//  File        : sensor_JXF02.c
//  Description : Firmware Sensor Control File
//  Author      : Philip Lin
//  Revision    : 1.0
//
//=============================================================================

#include "includes_fw.h"
#include "customer_config.h"

#if (SENSOR_EN)
#if (BIND_SENSOR_JXF02)

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmpf_sensor.h"
#include "mmpf_i2cm.h"
#include "Sensor_Mod_Remapping.h"
#include "isp_if.h"

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#define MAX_SENSOR_GAIN     (4)          // max sensor gain

//==============================================================================
//
//                              GLOBAL VARIABLE
//
//==============================================================================

// Resolution Table
MMPF_SENSOR_RESOLUTION m_SensorRes = 
{
	2,				/* ubSensorModeNum */
	0,				/* ubDefPreviewMode */
	0,				/* ubDefCaptureMode */
	3000,			// usPixelSize
//  Mode0   Mode1
    {1,     1},	/* usVifGrabStX */
    {1,     1},	/* usVifGrabStY */
    {1928,  1928},	/* usVifGrabW */
    {1088,  1088},	/* usVifGrabH */
    #if (CHIP == MCR_V2)
    {1,     1}, 	// usBayerInGrabX
    {1,     1},    // usBayerInGrabY
    {8,     8},  /* usBayerDummyInX */
    {8,     8},  /* usBayerDummyInY */
    {1920,  1920},	/* usBayerOutW */
    {1080,  1080},	/* usBayerOutH */
    #endif
    {1920,  1920},	/* usScalInputW */
    {1080,  1080},	/* usScalInputH */
    {300,   600},	/* usTargetFpsx10 */
    {1130,  1134},	/* usVsyncLine */  //Reg 0x22 0x23
    {1,     1},  /* ubWBinningN */
    {1,     1},  /* ubWBinningN */
    {1,     1},  /* ubWBinningN */
    {1,     1},  /* ubWBinningN */
    {0,     0},    	// ubCustIQmode
    {0,     0}     	// ubCustAEmode  
};

// OPR Table and Vif Setting
MMPF_SENSOR_OPR_TABLE       m_OprTable;
MMPF_SENSOR_VIF_SETTING     m_VifSetting;

//lv1-7, 2	3	6	11	17	30	60
#if 1//	new extent node for18//LV1,		LV2,		LV3,		LV4,		LV5,		LV6,		LV7,		LV8,		LV9,		LV10,	LV11,	LV12,	LV13,	LV14,	LV15,	LV16 	LV17  	LV18
//abby curve iq 12
ISP_UINT32 AE_Bias_tbl[54] =
/*lux*/			//			{2,			3,			6,			10,			17, 		26, 		54, 		101, 		206,		407,	826,	1638,	3277,	6675,	13554,	27329,	54961, 111285/*930000=LV17*/  //with  1202
/*lux*/						{1,			2,			3,			6,			10,			17, 		26, 		54, 		101, 		206,	407,	826,	1638,	3277,	6675,	13554,	27329,	54961/*930000=LV17*/ //with  1203
/*ENG*/						,0x2FFFFFF, 4841472*2,	3058720,	1962240,	1095560,  	616000, 	334880, 	181720,     96600,	 	52685,	27499,	14560,	8060,	4176,	2216,	1144,	600,   300
/*Tar*/			//			,55,		60,		 	65,	        90,			110, 		122,	 	180,	 	200,	    205,	    210,	215,	220,	225,	230,	235,	240,	250,   250 //with max 15x 1202V1
/*Tar*/			//			,42,		48,		 	55,	        60,			75, 		90,	 		130,	 	180,	    220,	    240,	242,	244,	246,	247,	248,	248,	249,   250 //with max 13x 1202V2
/*Tar*/			//			,30,		40,			45,		 	52,	        60,			75, 		90,	 		150,	 	200,	    220,	240,	242,	244,	246,	247,	248,	248,	249 //with max 11x 1203V1
/*Tar*/						,30,		40,			45,		 	52,	        65,			80, 	   100,	 		150,	 	200,	    220,	240,	255,	265,	268,	270,	274,	277,	280 //with max 11x 1207

 };	
#define AE_tbl_size  (18)	//32  35  44  50
#endif
// IQ Table
const ISP_UINT8 Sensor_IQ_CompressedText[] = 
{
#if(CHIP == MCR_V2)
	#include "isp_8428_iq_data_v2_JXF02_20160203_Daniel.xls.ciq.txt"	//Adjust Lux reference
#endif
};

// I2cm Attribute
static MMP_I2CM_ATTR m_I2cmAttr = {
    MMP_I2CM0, // i2cmID
    0x40,       // ubSlaveAddr
    8,          // ubRegLen
    8,          // ubDataLen
    0,          // ubDelayTime
    MMP_FALSE,  // bDelayWaitEn
    MMP_TRUE,   // bInputFilterEn
    MMP_FALSE,  // b10BitModeEn
    MMP_FALSE,  // bClkStretchEn
    0,          // ubSlaveAddr1
    0,          // ubDelayCycle
    0,          // ubPadNum
    150000, //150KHZ,,,,,,400000,     // ulI2cmSpeed 400KHZ
    MMP_TRUE,   // bOsProtectEn
    NULL,       // sw_clk_pin
    NULL,       // sw_data_pin
    MMP_FALSE,  // bRfclModeEn
    MMP_FALSE,  // bWfclModeEn
    MMP_FALSE,  // bRepeatModeEn
    0           // ubVifPioMdlId
};

// 3A Timing
MMPF_SENSOR_AWBTIMIMG   m_AwbTime    = 
{
	6,	/* ubPeriod */
	1, 	/* ubDoAWBFrmCnt */
	2	/* ubDoCaliFrmCnt */
};

MMPF_SENSOR_AETIMIMG    m_AeTime     = 
{	
	4, /* ubPeriod */
	0, 	/* ubFrmStSetShutFrmCnt */
	1	/* ubFrmStSetGainFrmCnt */
};

MMPF_SENSOR_AFTIMIMG    m_AfTime     = 
{
	1, 	/* ubPeriod */
	0	/* ubDoAFFrmCnt */
};

#if (ISP_EN)
static ISP_UINT32 s_gain;
#endif

//==============================================================================
//
//                              EXTERN VARIABLE
//
//==============================================================================

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

#if 0
void ____Sensor_OPR_Table____(){ruturn;} //dummy
#endif

MMP_USHORT SNR_JXF02_Reg_Init_Customer[] = 
{
0x12, 0x80,  // [7]: software reset, [6]: sleep mode, [5]:mirror, [4]: flip, [3]: HDR mode
};

// 1080p 30FPS
MMP_USHORT SNR_JXF02_Reg_1920x1080_Customer[] = 
{
#if (SENSOR_IF == SENSOR_IF_MIPI_1_LANE)

0x12, 0x40,
0x0D, 0xA4,
0x0E, 0x10,
0x0F, 0x09,
0x10, 0x22,
0x11, 0x80,
0x20, 0x5A,
0x21, 0x02,
0x22, 0x6A,
0x23, 0x04,
0x24, 0xE2,
0x25, 0x44,
0x26, 0x41,
0x27, 0xC7,
0x28, 0x12,
0x29, 0x00,
0x2A, 0xC1,
0x2B, 0x18,
0x2C, 0x02,
0x2D, 0x00,
0x2E, 0x14,
0x2F, 0x44,
0x30, 0x8C,
0x31, 0x06,
0x32, 0x10,
0x33, 0x0C,
0x34, 0x66,
0x35, 0xD1,
0x36, 0x0E,
0x37, 0x34,
0x38, 0x14,
0x39, 0x82,
0x3A, 0x08,
0x1D, 0x00,
0x1E, 0x00,
0x6C, 0x50,
0x70, 0xE1,
0x71, 0xAA,
0x72, 0x8A,
0x73, 0x53,
0x74, 0x52,
0x75, 0x2B,
0x76, 0x6A,
0x77, 0x09,
0x78, 0x09,
0x41, 0xD0,
0x42, 0x23,
0x67, 0x70,
0x68, 0x02,
0x69, 0x74,
0x6A, 0x4A,
0x5A, 0x04,
0x5C, 0x8f,
0x5D, 0xB0,
0x5E, 0xe6,
0x60, 0x16,
0x62, 0x30,
0x64, 0x58,
0x65, 0x00,
0x12, 0x00,
0x47, 0x80,
0x50, 0x02,
0x13, 0x85,
0x14, 0x80,
0x16, 0xC0,
0x17, 0x40,
0x18, 0x1A,
0x19, 0xA1,
0x4a, 0x01,
0x49, 0x10,
0x45, 0x19,
0x1F, 0x01,
0x48, 0x81,
//INI End

#elif (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)

0x12, 0x40,
0x0D, 0xA4,
0x0E, 0x10,
0x0F, 0x19,
0x10, 0x22,
0x11, 0x80,
0x20, 0x5A,
0x21, 0x02,
0x22, 0x6A,
0x23, 0x04,
0x24, 0xE2,
0x25, 0x44,
0x26, 0x41,
0x27, 0xC7,
0x28, 0x0E,
0x29, 0x00,
0x2A, 0xC1,
0x2B, 0x38,
0x2C, 0x02,
0x2D, 0x00,
0x2E, 0x15,
0x2F, 0x44,
0x30, 0x8C,
0x31, 0x06,
0x32, 0x10,
0x33, 0x0C,
0x34, 0x66,
0x35, 0xD1,
0x36, 0x0E,
0x37, 0x34,
0x38, 0x14,
0x39, 0x82,
0x3A, 0x08,
0x3B, 0x00,
0x3C, 0xFF,
0x3D, 0x90,
0x3E, 0xA0,
0x1D, 0x00,
0x1E, 0x00,
0x6C, 0x40,
0x70, 0x61,
0x71, 0x4A,
0x72, 0x4A,
0x73, 0x33,
0x74, 0x52,
0x75, 0x2B,
0x76, 0x6A,
0x77, 0x09,
0x78, 0x0B,
0x41, 0xD1,
0x42, 0x23,
0x67, 0x70,
0x68, 0x02,
0x69, 0x74,
0x6A, 0x4A,
0x5A, 0x04,
0x5C, 0x8F,
0x5D, 0xB0,
0x5E, 0xE6,
0x60, 0x16,
0x62, 0x30,
0x63, 0x80,
0x64, 0x58,
0x65, 0x00,
0x12, 0x00,
0x47, 0x80,
0x50, 0x02,
0x13, 0x85,
0x14, 0x80,
0x16, 0xC0,
0x17, 0x40,
0x18, 0x1A,
0x19, 0x81,
0x4A, 0x03,
0x49, 0x10,
0x45, 0x19,
0x1F, 0x01,
//0x48, 0x81, /////
//INI End
#elif (SENSOR_IF == SENSOR_IF_PARALLEL)
0x12, 0x40,
0x0D, 0xA4,
0x0E, 0x10,
0x0F, 0x09,
0x10, 0x22,
0x11, 0x80,
0x20, 0x58,
0x21, 0x02,
0x22, 0x6E,
0x23, 0x04,
0x24, 0xE2,
0x25, 0x40,
0x26, 0x41,
0x27, 0xC7,
0x28, 0x10,
0x29, 0x00,
0x2A, 0xC1,
0x2B, 0x38,
0x2C, 0x01,
0x2D, 0x00,
0x2E, 0x14,
0x2F, 0x44,
0x30, 0x8C,
0x31, 0x06,
0x32, 0x10,
0x33, 0x0C,
0x34, 0x66,
0x35, 0xD1,
0x36, 0x0E,
0x37, 0x34,
0x38, 0x14,
0x39, 0x82,
0x3A, 0x08,
0x3B, 0x00,
0x3C, 0xFF,
0x3D, 0x90,
0x3E, 0xA0,
0x1D, 0xFF,
0x1E, 0x9F,//0x1F,
0x6C, 0x90,
0x70, 0x61,
0x71, 0x4A,
0x72, 0x4A,
0x73, 0x33,
0x74, 0x52,
0x75, 0x2B,
0x76, 0x6A,
0x77, 0x09,
0x78, 0x0B,
0x41, 0xD1,
0x42, 0x23,
0x67, 0x70,
0x68, 0x02,
0x69, 0x74,
0x6A, 0x4A,
0x5A, 0x04,
0x5C, 0x8f,
0x5D, 0xB0,
0x5E, 0xe6,
0x60, 0x16,
0x62, 0x30,
0x63, 0x80,
0x64, 0x58,
0x65, 0x00,
0x12, 0x00,
0x47, 0x80,
0x50, 0x02,
0x13, 0x85,
0x14, 0x80,
0x16, 0xC0,
0x17, 0x40,
0x18, 0x1B,
0x19, 0xA1,
//0x4a, 0x03,
// BLC trigger
0x48, 0x81,  //OB change th. >1
0x4a, 0x01,	 //trigger by OB change

0x49, 0x10,
0x45, 0x19,
0x1F, 0x01
#endif

};

// 1080p 60FPS
MMP_USHORT SNR_JXF02_Reg_1920x1080_60_Customer[] = 
{
#if (SENSOR_IF == SENSOR_IF_MIPI_1_LANE || SENSOR_IF == SENSOR_IF_PARALLEL)
//Not support
0x0, 0x0,
#elif (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)
//Product Ver:VAE00 
//Output Detail:  
//MCLK:24 MHz     
//PCLK:81.6       
//Mipi Freq:816 (Mipi Clk: 816/2 = 408 MHz)  
//VCO:816         
//FrameW:2400     
//FrameH:1134     

0x12,0x40,                
0x0D,0xA4,                
0x0E,0x10,                
0x0F,0x09,                
0x10,0x22,                
0x11,0x80,                
0x20,0xB0,                
0x21,0x04,                
0x22,0x6E,                
0x23,0x04,                
0x24,0xC4,                
0x25,0x40,                
0x26,0x43,                
0x27,0x8D,                
0x28,0x10,                
0x29,0x01,                
0x2A,0x82,                
0x2B,0x39,                
0x2C,0x01,                
0x2D,0x00,                
0x2E,0x14,                
0x2F,0x44,                
0x30,0x98,                
0x31,0x0C,                
0x32,0x20,                
0x33,0x18,                
0x34,0xCC,                
0x35,0xE3,                
0x36,0x1C,                
0x37,0x68,                
0x38,0x18,                
0x39,0x82,                
0x3A,0x08,                
0x3B,0xFF,                
0x3C,0xFF,                
0x3D,0x10,                
0x3E,0xF0,                
0x1D,0x00,                
0x1E,0x00,                
0x6C,0x40,                
0x70,0x61,                
0x71,0xAA,                
0x72,0x8A,                
0x73,0x53,                
0x74,0x52,                
0x75,0x2B,                
0x76,0x6A,                
0x77,0x09,                
0x78,0x1D,                
0x41,0xD1,                
0x42,0x23,                
0x67,0x70,                
0x68,0x02,                
0x69,0x74,                
0x6A,0x4A,                
0x5A,0x04,                
0x5C,0x8f,                
0x5D,0xB0,                
0x5E,0xe6,                
0x60,0x16,                
0x62,0x30,                
0x63,0x80,                
0x64,0x58,                
0x65,0x00,                
0x12,0x00,                
0x47,0x80,                
0x50,0x02,                
0x13,0x85,                
0x14,0x80,                
0x16,0xC0,                
0x17,0x40,                
0x18,0x1B,                
0x19,0xA9,                
0x4a,0x03,                
0x49,0x10,           

//I2C Sleep 100ms(min)  
//SENSOR_DELAY_REG, 110,

0x45,0x19,                
0x1F,0x01,                
//INI End

#endif
};

#if 0
void ____Sensor_Customer_Func____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_InitConfig
//  Description :
//------------------------------------------------------------------------------
static void SNR_Cust_InitConfig(void)
{
	MMP_USHORT 	i;

	RTNA_DBG_Str(0, "SNR_Cust_IniConfig JXF02\r\n");
	
    // Init OPR Table
    SensorCustFunc.OprTable->usInitSize                   = (sizeof(SNR_JXF02_Reg_Init_Customer)/sizeof(SNR_JXF02_Reg_Init_Customer[0]))/2;
    SensorCustFunc.OprTable->uspInitTable                 = &SNR_JXF02_Reg_Init_Customer[0];    
    
    SensorCustFunc.OprTable->usSize[RES_IDX_1920x1080]    = (sizeof(SNR_JXF02_Reg_1920x1080_Customer)/sizeof(SNR_JXF02_Reg_1920x1080_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1920x1080]  = &SNR_JXF02_Reg_1920x1080_Customer[0];

    SensorCustFunc.OprTable->usSize[RES_IDX_1920x1080_60]    = (sizeof(SNR_JXF02_Reg_1920x1080_60_Customer)/sizeof(SNR_JXF02_Reg_1920x1080_60_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1920x1080_60]  = &SNR_JXF02_Reg_1920x1080_60_Customer[0];

	// Init Vif Setting : Common
    //SensorCustFunc.VifSetting->SnrType                                = MMPF_VIF_SNR_TYPE_BAYER;
    #if (SENSOR_IF == SENSOR_IF_PARALLEL)
    SensorCustFunc.VifSetting->OutInterface                           = MMPF_VIF_IF_PARALLEL;
    #elif (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)
	SensorCustFunc.VifSetting->OutInterface 					      = MMPF_VIF_IF_MIPI_DUAL_01;
    #else
	SensorCustFunc.VifSetting->OutInterface 						  = MMPF_VIF_IF_MIPI_SINGLE_0;
    #endif

    #if (PRM_SENSOR_VIF_ID == MMPF_VIF_MDL_ID1)
    SensorCustFunc.VifSetting->VifPadId							      = MMPF_VIF_MDL_ID1;
    #else
    SensorCustFunc.VifSetting->VifPadId							      = MMPF_VIF_MDL_ID0;
    #endif
    
    // Init Vif Setting : PowerOn Setting
    SensorCustFunc.VifSetting->powerOnSetting.bTurnOnExtPower         = MMP_TRUE;
    #if (CHIP == MCR_V2)
    SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPin           = MMP_GPIO_MAX;
    #endif   
    SensorCustFunc.VifSetting->powerOnSetting.bFirstEnPinHigh         = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubFirstEnPinDelay       = 10;
    SensorCustFunc.VifSetting->powerOnSetting.bNextEnPinHigh          = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOnSetting.ubNextEnPinDelay        = 10;
    SensorCustFunc.VifSetting->powerOnSetting.bTurnOnClockBeforeRst   = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.bFirstRstPinHigh        = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOnSetting.ubFirstRstPinDelay      = 10;
    SensorCustFunc.VifSetting->powerOnSetting.bNextRstPinHigh         = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubNextRstPinDelay       = 20;
    
    // Init Vif Setting : PowerOff Setting
    SensorCustFunc.VifSetting->powerOffSetting.bEnterStandByMode      = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOffSetting.usStandByModeReg       = 0x100;
    SensorCustFunc.VifSetting->powerOffSetting.usStandByModeMask      = 0x0;    
    SensorCustFunc.VifSetting->powerOffSetting.bEnPinHigh             = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOffSetting.ubEnPinDelay           = 20;
    SensorCustFunc.VifSetting->powerOffSetting.bTurnOffMClock         = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOffSetting.bTurnOffExtPower       = MMP_FALSE;    
    #if (CHIP == P_V2)
    SensorCustFunc.VifSetting->powerOffSetting.usExtPowerPin          = MMPF_PIO_REG_SGPIO0;
    #endif
    #if (CHIP == MCR_V2)
    SensorCustFunc.VifSetting->powerOffSetting.usExtPowerPin          = MMP_GPIO_MAX;
    #endif
    
    // Init Vif Setting : Sensor paralAttr Setting
    SensorCustFunc.VifSetting->clockAttr.bClkOutEn                    = MMP_TRUE; 
    SensorCustFunc.VifSetting->clockAttr.ubClkFreqDiv                 = 0;
    SensorCustFunc.VifSetting->clockAttr.ulMClkFreq                   = 24000;
    SensorCustFunc.VifSetting->clockAttr.ulDesiredFreq                = 24000;
    SensorCustFunc.VifSetting->clockAttr.ubClkPhase                   = MMPF_VIF_SNR_PHASE_DELAY_NONE;
    SensorCustFunc.VifSetting->clockAttr.ubClkPolarity                = MMPF_VIF_SNR_CLK_POLARITY_POS;
    SensorCustFunc.VifSetting->clockAttr.ubClkSrc					  = MMPF_VIF_SNR_CLK_SRC_PMCLK;
        
    // Init Vif Setting : Parallel Sensor Setting
    SensorCustFunc.VifSetting->paralAttr.ubLatchTiming                = MMPF_VIF_SNR_LATCH_POS_EDGE;
    SensorCustFunc.VifSetting->paralAttr.ubHsyncPolarity              = MMPF_VIF_SNR_CLK_POLARITY_POS;
    SensorCustFunc.VifSetting->paralAttr.ubVsyncPolarity              = MMPF_VIF_SNR_CLK_POLARITY_NEG;

	// Init Vif Setting : MIPI Sensor Setting
	#if (SENSOR_IF == SENSOR_IF_MIPI_1_LANE)
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[0]                = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[1]                = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[2]                = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[3]                = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[0]               = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[1]               = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[2]               = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[3]               = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[0]            = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[1]            = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[2]            = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[3]            = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[0]              = (SensorCustFunc.VifSetting->VifPadId == MMPF_VIF_MDL_ID0)? MMPF_VIF_MIPI_DATA_SRC_PHY_0 : MMPF_VIF_MIPI_DATA_SRC_PHY_1;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[1]              = 0;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[2]              = 0;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[3]              = 0;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[0]                = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[1]                = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[2]                = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[3]                = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[0]               = 0x0F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[1]               = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[2]               = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[3]               = 0x08;
    #elif (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[0]                = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[1]                = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[2]                = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[3]                = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[0]               = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[1]               = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[2]               = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[3]               = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[0]            = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[1]            = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[2]            = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[3]            = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[0]              = (SensorCustFunc.VifSetting->VifPadId == MMPF_VIF_MDL_ID0)? MMPF_VIF_MIPI_DATA_SRC_PHY_0 : MMPF_VIF_MIPI_DATA_SRC_PHY_1;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[1]              = (SensorCustFunc.VifSetting->VifPadId == MMPF_VIF_MDL_ID0)? MMPF_VIF_MIPI_DATA_SRC_PHY_1 : MMPF_VIF_MIPI_DATA_SRC_PHY_2;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[2]              = MMPF_VIF_MIPI_DATA_SRC_PHY_UNDEF;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[3]              = MMPF_VIF_MIPI_DATA_SRC_PHY_UNDEF;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[0]                = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[1]                = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[2]                = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[3]                = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[0]               = 0x17;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[1]               = 0x17;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[2]               = 0x17;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[3]               = 0x17;
	#endif
	
    // Init Vif Setting : Color ID Setting
    SensorCustFunc.VifSetting->colorId.VifColorId              		  = MMPF_VIF_COLORID_11; 
    
	SensorCustFunc.VifSetting->colorId.CustomColorId.bUseCustomId  	  = MMP_TRUE; //MMP_FALSE;

    for (i = 0; i < MAX_SENSOR_RES_MODE; i++)
    {
        SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[i]   = MMPF_VIF_COLORID_11;
        SensorCustFunc.VifSetting->colorId.CustomColorId.Rot90d_Id[i]  = MMPF_VIF_COLORID_UNDEF;
        SensorCustFunc.VifSetting->colorId.CustomColorId.Rot180d_Id[i] = MMPF_VIF_COLORID_00;
        SensorCustFunc.VifSetting->colorId.CustomColorId.Rot270d_Id[i] = MMPF_VIF_COLORID_UNDEF;
        SensorCustFunc.VifSetting->colorId.CustomColorId.H_Flip_Id[i]  = MMPF_VIF_COLORID_10;
        SensorCustFunc.VifSetting->colorId.CustomColorId.V_Flip_Id[i]  = MMPF_VIF_COLORID_10;
        SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[i] = MMPF_VIF_COLORID_00;
    }
}

//------------------------------------------------------------------------------
//  Function    : SNR_JXF02_DoAE_FrmSt
//  Description :
//------------------------------------------------------------------------------

MMP_ULONG 	reg2;
#define	DGAIN_BASE	0x200
ISP_UINT32	Dgain = DGAIN_BASE;
void SNR_Cust_DoAE_FrmSt(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
#if (ISP_EN)
    MMP_ULONG   ulVsync = 0;
    MMP_ULONG   ulShutter = 0;
	MMP_UBYTE   ubPeriod              = (SensorCustFunc.pAeTime)->ubPeriod;
	MMP_UBYTE   ubFrmStSetShutFrmCnt  = (SensorCustFunc.pAeTime)->ubFrmStSetShutFrmCnt;	
	MMP_UBYTE   ubFrmStSetGainFrmCnt  = (SensorCustFunc.pAeTime)->ubFrmStSetGainFrmCnt;
	//return; // MATT.
	if(ulFrameCnt % ubPeriod == ubFrmStSetShutFrmCnt) {
		ISP_IF_AE_Execute();
		gsSensorFunction->MMPF_Sensor_SetShutter(ubSnrSel, 0, 0);
    }
	else if(ulFrameCnt % ubPeriod == ubFrmStSetGainFrmCnt) {
	    gsSensorFunction->MMPF_Sensor_SetGain(ubSnrSel, 0);
	}
#endif
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_DoAE_FrmEnd
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_DoAE_FrmEnd(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
	MMP_UBYTE   ubPeriod              = (SensorCustFunc.pAeTime)->ubPeriod;
	MMP_UBYTE   ubFrmStSetGainFrmCnt  = (SensorCustFunc.pAeTime)->ubFrmStSetGainFrmCnt;
	
	if(ulFrameCnt % ubPeriod == ubFrmStSetGainFrmCnt) {
		ISP_IF_IQ_SetAEGain(Dgain, DGAIN_BASE);
	}
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_DoAWB
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_DoAWB(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
	// TBD

#if 1
    MMP_U_LONG Lux = ISP_IF_AE_GetLightCond();
    MMP_U_LONG luma_ratio;
    MMP_U_LONG Lux_max	= 100;	    
    MMP_U_LONG Lux_min	= 15;
    MMP_U_LONG Lux_Base	= 128;	//128=1x
    MMP_U_LONG LowBound = 30;	
    if (Lux > Lux_max){ 
        ISP_IF_IQ_SetOpr(0x7095, 1, Lux_Base); 
        ISP_IF_IQ_SetOpr(0x7096, 1, Lux_Base); 
        ISP_IF_IQ_SetOpr(0x7097, 1, Lux_Base);
    }
    else{
        if (Lux <= Lux_min) Lux = Lux_min;	        
        luma_ratio = (Lux - Lux_min) * (Lux_Base - LowBound) / (Lux_max - Lux_min) + LowBound;
        ISP_IF_IQ_SetOpr(0x7095, 1, luma_ratio);
        ISP_IF_IQ_SetOpr(0x7096, 1, luma_ratio);
        ISP_IF_IQ_SetOpr(0x7097, 1, luma_ratio);
    }   
#endif
	
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_DoIQ
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_DoIQ(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetGain
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetGain(MMP_UBYTE ubSnrSel, MMP_ULONG ulGain)
{
    ISP_UINT16 s_gain, gainbase;
    ISP_UINT32 H, L;
	ISP_UINT32 set_gain = 16;
	ISP_UINT8 VFPN_SW = 1, VFPN_L = 0;
	
	//return; // MATT.
	if(ulGain == 0)
		ulGain = VR_MAX(ISP_IF_AE_GetGain(), ISP_IF_AE_GetGainBase());
    gainbase = ISP_IF_AE_GetGainBase();

	if (ulGain >= (ISP_IF_AE_GetGainBase() * MAX_SENSOR_GAIN))
	{
		Dgain 	= DGAIN_BASE * ulGain / (ISP_IF_AE_GetGainBase() * MAX_SENSOR_GAIN);
		ulGain  = ISP_IF_AE_GetGainBase() * MAX_SENSOR_GAIN;
	}else
	{
		Dgain	= DGAIN_BASE;
	}

    s_gain = ISP_MIN(ISP_MAX(ulGain, gainbase), gainbase*16-1); // API input gain range : 64~511, 64=1X
  		
    if (s_gain >= gainbase * 8) {
        H = 3;
        L = s_gain * 16 / gainbase / 8 - 16;         
        set_gain = 8 * (16+L) * gainbase / 16; 
        VFPN_SW = 1; 
		VFPN_L  = 0;
    } else if (s_gain >= gainbase * 4) {
        H = 2;
        L = s_gain * 16 / gainbase / 4 - 16;         
        set_gain = 4*(16+L) * gainbase / 16;                
        VFPN_SW = 1;
		VFPN_L  = 0;        
    } else if (s_gain >= gainbase * 2) {
        H = 1;
        L = s_gain * 16 / gainbase / 2 - 16;        
        set_gain = 2*(16+L) * gainbase / 16;
        VFPN_SW = 1;
		VFPN_L  = 0;                  
    } else {
        H = 0;
        L = s_gain * 16 / gainbase / (1) - 16;
        set_gain = 1*(16+L) * gainbase / 16; 
        VFPN_SW = 0;
		VFPN_L  = 15 - ISP_MIN(L,15);                         
    }
    if (L > 15) L = 15;	   
  	Dgain = ulGain * Dgain / set_gain;
  
    // set sensor gain
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x00, (H<<4)+L); //Total gain = (2^PGA[6:4])*(1+PGA[3:0]/16) //PGA:0x00
	
	
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x0C, VFPN_SW*0x40); 	// light-VFPN function
	//gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x6A, 0x40 + (VFPN_L&0xF)); 		// 	
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetShutter
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetShutter(MMP_UBYTE ubSnrSel, MMP_ULONG shutter, MMP_ULONG vsync)
{
    ISP_UINT32 new_vsync;
    ISP_UINT32 new_shutter;
	//return; // MATT.
    if(shutter == 0 || vsync == 0)
    {
        new_vsync = gSnrLineCntPerSec[ubSnrSel] * ISP_IF_AE_GetVsync() / ISP_IF_AE_GetVsyncBase();
        new_shutter = gSnrLineCntPerSec[ubSnrSel] * ISP_IF_AE_GetShutter() / ISP_IF_AE_GetShutterBase();
    }
    else
    {
        new_vsync = gSnrLineCntPerSec[ubSnrSel] * vsync / ISP_IF_AE_GetVsyncBase();
        new_shutter = gSnrLineCntPerSec[ubSnrSel] * shutter / ISP_IF_AE_GetShutterBase();
    }

	new_vsync   = ISP_MIN(ISP_MAX(new_shutter + 5, new_vsync), 0xFFFF);
	new_shutter = ISP_MIN(ISP_MAX(new_shutter, 1), new_vsync - 5);
	
//	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x23, new_vsync >> 8);
//	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x22, new_vsync);
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x02, (ISP_UINT8)((new_shutter >> 8) & 0xff));
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x01, (ISP_UINT8)(new_shutter & 0xff));
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetFlip
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetFlip(MMP_UBYTE ubSnrSel, MMP_UBYTE ubMode)
{
	printc("SNR_Cust_SetFlip %d \r\n", ubMode);
	if (ubMode == MMPF_SENSOR_NO_FLIP)
	{
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel,0x12, 0x00);  // [7]: software reset, [6]: sleep mode, [5]:mirror, [4]: flip, [3]: HDR mode
	}else if (ubMode == MMPF_SENSOR_COLUMN_FLIP)
	{
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel,0x12, 0x20);  // [7]: software reset, [6]: sleep mode, [5]:mirror, [4]: flip, [3]: HDR mode
	
	}else if (ubMode == MMPF_SENSOR_ROW_FLIP)
	{
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel,0x12, 0x10);  // [7]: software reset, [6]: sleep mode, [5]:mirror, [4]: flip, [3]: HDR mode
	
	}else if (ubMode == MMPF_SENSOR_COLROW_FLIP)
	{
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel,0x12, 0x30);  // [7]: software reset, [6]: sleep mode, [5]:mirror, [4]: flip, [3]: HDR mode
	
	}
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetRotate
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetRotate(MMP_UBYTE ubSnrSel, MMP_UBYTE ubMode)
{
	printc("SNR_Cust_SetRotate %d \r\n", ubMode);
	if (ubMode == MMPF_SENSOR_ROTATE_NO_ROTATE)
	{
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel,0x12, 0x00);  // [7]: software reset, [6]: sleep mode, [5]:mirror, [4]: flip, [3]: HDR mode
	}else if (ubMode == MMPF_SENSOR_ROTATE_RIGHT_180)
	{
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel,0x12, 0x30);  // [7]: software reset, [6]: sleep mode, [5]:mirror, [4]: flip, [3]: HDR mode
	}else{

	}

}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_CheckVersion
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_CheckVersion(MMP_UBYTE ubSnrSel, MMP_ULONG *pulVersion)
{
}

MMPF_SENSOR_CUSTOMER  SensorCustFunc = 
{
    SNR_Cust_InitConfig,
    SNR_Cust_DoAE_FrmSt,
    SNR_Cust_DoAE_FrmEnd,
    SNR_Cust_DoAWB,
    SNR_Cust_DoIQ,
    SNR_Cust_SetGain,
    SNR_Cust_SetShutter,
    SNR_Cust_SetFlip,
    SNR_Cust_SetRotate,
    SNR_Cust_CheckVersion,
    
    &m_SensorRes,
    &m_OprTable,
    &m_VifSetting,
    &m_I2cmAttr,
    &m_AwbTime,
    &m_AeTime,
    &m_AfTime,
	MMP_SNR_PRIO_PRM
};

int SNR_Module_Init(void)
{
    MMPF_SensorDrv_Register(PRM_SENSOR, &SensorCustFunc);

    return 0;
}

#pragma arm section code = "initcall6", rodata = "initcall6", rwdata = "initcall6", zidata = "initcall6"
#pragma O0
ait_module_init(SNR_Module_Init);
#pragma
#pragma arm section rodata, rwdata, zidata

#endif  //BIND_SENSOR_JXF02
#endif	//SENSOR_EN

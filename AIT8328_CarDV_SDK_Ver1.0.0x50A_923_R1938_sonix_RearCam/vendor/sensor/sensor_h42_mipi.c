//==============================================================================
//
//  File        : sensor_H42.c
//  Description : Firmware Sensor Control File
//  Author      :
//  Revision    : 1.0
//
//==============================================================================

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "includes_fw.h"
#include "customer_config.h"

#if (SENSOR_EN)
#if (BIND_SENSOR_H42_MIPI)

#include "mmpf_sensor.h"
#include "Sensor_Mod_Remapping.h"
#include "isp_if.h"

//==============================================================================
//
//                              GLOBAL VARIABLE
//
//==============================================================================
//#define SENSOR_IF                       (SENSOR_IF_MIPI_1_LANE) // wilsonjin 20151208

// Resolution Table
MMPF_SENSOR_RESOLUTION m_SensorRes = 
{
	2, 												// ubSensorModeNum
	0, 												// ubDefPreviewMode
	1, 												// ubDefCaptureMode
	3000,                                           // usPixelSize
// Mode0 	Mode1 
	{1,  	1,      160}, 	// usVifGrabStX
	{1,	    1,      1}, 	// usVifGrabStY
	{1288,   1288,  960}, // usVifGrabW
	{724,    724,   724}, 	// usVifGrabH
	{1,      1,     1}, 	// usBayerInGrabX
	{1,      1,     1}, 	// usBayerInGrabY
	{8,      8,     8}, 	// usBayerInDummyX
	{4,      4,     4}, 	// usBayerInDummyY
	{1280,   1280,  960}, // usBayerOutW
	{720,    720,   720}, 	// usBayerOutH
	{1280,   1280,  960}, // usScalInputW
	{720,    720,   720}, 	// usScalInputH
	{300,    600,   300}, 	// usTargetFpsx10
	{0x2F0,  744,   1104}, 	// usVsyncLine // wilsonjin 20151208
	{1,      1,     1}, 	// ubWBinningN
	{1,      1,     1}, 	// ubWBinningM
	{1,      1,     1}, 	// ubHBinningN
	{1,      1,     1}, 	// ubHBinningM
    {0,      0,     0},    // ubCustIQmode
    {0,      1,     0}     // ubCustAEmode
};

// OPR Table and Vif Setting
MMPF_SENSOR_OPR_TABLE 	m_OprTable;
MMPF_SENSOR_VIF_SETTING m_VifSetting;

// IQ Table
const ISP_UINT8 Sensor_IQ_CompressedText[] = 
{
#ifdef CUS_ISP_8428_IQ_DATA
#include CUS_ISP_8428_IQ_DATA
#else
//#include "isp_8428_iq_data_v2_OV2710.xls.ciq.txt"
#include "isp_8428_iq_data_v2_H42_20160105.xls.ciq.txt"
#endif
};

// I2cm Attribute
static MMP_I2CM_ATTR m_I2cmAttr = 
{
	MMP_I2CM0,      // i2cmID
    0x60>>1,       	// ubSlaveAddr[60,  64,68,  6c]
	8, 			    // ubRegLen
	8, 				// ubDataLen
	0, 				// ubDelayTime
	MMP_FALSE, 		// bDelayWaitEn
	MMP_TRUE, 		// bInputFilterEn
	MMP_FALSE, 		// b10BitModeEn
	MMP_FALSE, 		// bClkStretchEn
	0, 				// ubSlaveAddr1
	0, 				// ubDelayCycle
	0, 				// ubPadNum
	50000, 		// ulI2cmSpeed 250KHZ
	MMP_TRUE, 		// bOsProtectEn
	NULL, 			// sw_clk_pin
	NULL, 			// sw_data_pin
	MMP_FALSE, 		// bRfclModeEn
	MMP_FALSE,      // bWfclModeEn
	MMP_FALSE,		// bRepeatModeEn
    0               // ubVifPioMdlId
};

// 3A Timing
MMPF_SENSOR_AWBTIMIMG m_AwbTime = 
{
	6, 	// ubPeriod
	1, 	// ubDoAWBFrmCnt
	3 	// ubDoCaliFrmCnt
};

MMPF_SENSOR_AETIMIMG m_AeTime = 
{
	3, 	// ubPeriod
	0, 	// ubFrmStSetShutFrmCnt
	1 	// ubFrmStSetGainFrmCnt
};

MMPF_SENSOR_AFTIMIMG m_AfTime = 
{
	1, 	// ubPeriod
	0 	// ubDoAFFrmCnt
};

#if (ISP_EN)
static ISP_UINT32 s_gain;
#endif

// Only H42 need to refrence this gain table. Other sensor may can set gain by calculate.
const ISP_UINT8 H42_GainTable[128] = 
{
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,

	0x00, //0x10
	0x01,
	0x02,
	0x03,
	0x04,
	0x05,
	0x06,
	0x07,
	0x08,
	0x09,
	0x0a,
	0x0b,
	0x0c,
	0x0d,
	0x0e,
	0x0f,

	0x11,//0x20
	0x11,
	0x12,
	0x12,
	0x13,
	0x13,
	0x14,
	0x14,
	0x15,
	0x15,
	0x16,
	0x16,
	0x17,
	0x17,
	0x18,
	0x18,

	0x19,//0x30
	0x19,
	0x1a,
	0x1a,
	0x1b,
	0x1b,
	0x1c,
	0x1c,
	0x1d,
	0x1d,
	0x1e,
	0x1e,
	0x1f,
	0x1f,
	0x31,
	0x31,

	0x31,//0x40
	0x31,
	0x31,
	0x31,
	0x32,
	0x32,
	0x32,
	0x32,
	0x33,
	0x33,
	0x33,
	0x33,
	0x34,
	0x34,
	0x34,
	0x34,

	0x35,//0x50
	0x35,
	0x35,
	0x35,
	0x36,
	0x36,
	0x36,
	0x36,
	0x37,
	0x37,
	0x37,
	0x37,
	0x38,
	0x38,
	0x38,
	0x38,

	0x39,//0x60
	0x39,
	0x39,
	0x39,
	0x3a,
	0x3a,
	0x3a,
	0x3a,
	0x3b,
	0x3b,
	0x3b,
	0x3b,
	0x3c,
	0x3c,
	0x3c,
	0x3c,

	0x3d,//0x70
	0x3d,
	0x3d,
	0x3d,
	0x3e,
	0x3e,
	0x3e,
	0x3e,
	0x3f,
	0x3f,
	0x3f,
	0x3f,
	0x71,
	0x71,
	0x71,
	0x71,
};

//CASIO 20150423
//for soso machine
//lv1-7, 2	3	6	11	17	30	60
#if 0//	new extent node for18//LV1,		LV2,		LV3,		LV4,		LV5,		LV6,		LV7,		LV8,		LV9,		LV10,	LV11,	LV12,	LV13,	LV14,	LV15,	LV16 	LV17  	LV18
//abby curve iq 12
ISP_UINT32 AE_Bias_tbl[54] =
/*lux*/						{2,			3,			6,			10,			17, 		26, 		54, 		101, 		206,		407,	826,	1638,	3277,	6675,	13554,	27329,	54961, 111285/*930000=LV17*/
/*ENG*/						,0x2FFFFFF, 4841472*2,	3058720,	1962240,	1095560,  	616000, 	334880, 	181720,     96600,	 	52685,	27499,	14560,	8060,	4176,	2216,	1144,	600,   300
/*Tar*/						,55,		55,		 	65,	        82,			89,	 		104,	 	111,	 	119,	    135,	    152,	160,	170,	182,	195,	210,	230,	250,   250
 };	
#define AE_tbl_size  (18)	//32  35  44  50
#endif

// New extent node for18
//			LV1,			LV2,		LV3,		LV4,        LV5,		LV6,	LV7,	LV8,	LV9,	LV10,	LV11,	LV12,	LV13,	LV14,	LV15,	LV16,	LV17,	LV18
//abby curve
ISP_UINT32 AE_Bias_tbl[54] =
/*lux*/		{1,				2,			3,			6,			12,			24,		50,		101,	206,	407,	826,	1638,	3277,	6675,	13554,	27329,	54961,	111285/*930000=LV17*/
/*ENG*/		,0x2FFFFFF,		4841472*2,	3058720,	1962240,	1095560,	616000,	334880,	181720,	96600,	52685,	27499,	14560,	8060,	4176,	2216,	1144,	600,	300
///*Tar*/	,110,        	110,		110,		110,		128,		137,	161,	170,	190,	202,	220,	237,	256,	256,	256,	256,	256,	256
///*Tar*/	,60,			66,		 	72,	        80,			90,	 		100, 	110,	145,	180,    220,	256,	256,	256,	256,	256,	256,	256,   256
///*Tar*/	,60,			66,		 	72,	        80,			90,	 		100, 	110,	122,    135,    143,	155,	170,	185,	195,	220,	240,	250,   256
/*Tar*/		,60,			66,		 	72,	        80,			95,	 		120, 	135,	145,    155,    165,	175,	195,	210,	220,	230,	240,	250,   256
};
#define AE_tbl_size  (18)

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

#if 0
void ____Sensor_Init_OPR_Table____(){ruturn;} //dummy
#endif

ISP_UINT16 SNR_H42_Reg_Unsupport[] = 
{
	SENSOR_DELAY_REG, 100 // delay
};

ISP_UINT16 SNR_H42_Reg_Init_Customer[] = 
{
	//0x5780, 0x00,
	// TBD
	SENSOR_DELAY_REG, 100 // delay
};

#if 0
void ____Sensor_Res_OPR_Table____(){ruturn;} //dummy
#endif


// 1288x728 30fps
MMP_USHORT SNR_H42_Reg_1280x720_30P_Customer[] = 
{
	#if 1
	0x12,0x40,
	// ;;DVP Setting
	0x0D,0x40,
	0x1F,0x04,
	// ;;PLL Setting
	0x0E,0x1d,
	0x0F,0x09,
	0x10,0x1E,
	0x11,0x80,
	//;;Frame/Window
	0x20,0x50,
	0x21,0x06, // W
	0x22,0xF0,
	0x23,0x02, // H
	0x24,0x10,
	0x25,0xDC,
	0x26,0x25,
	0x27,0x45,
	0x28,0x0D,
	0x29,0x01,
	0x2A,0x24,
	0x2B,0x29,
	0x2C,0x00,
	0x2D,0x00,
	0x2E,0xBB,
	0x2F,0x00,
	//;;Sensor Timing
	0x30,0x92,
	0x31,0x0A,
	0x32,0xAA,
	0x33,0x14,
	0x34,0x38,
	0x35,0x54,
	0x42,0x41,
	0x43,0x50,
	//;;Interface
	0x1D,0x00,
	0x1E,0x00,
	0x6C,0x50,
	0x73,0x33,
	0x70,0x68,
	0x76,0x54,
	0x77,0x06,
	0x78,0x18,
	// ;;Array/AnADC/PWC
	0x48,0x40,
	0x60,0xA4,
	0x61,0xFF,
	0x62,0x40,
	0x65,0x00,
	0x66,0x20,
	0x67,0x30,
	0x68,0x04,
	0x69,0x74,
	0x6F,0x04,
	//;;Black Sun
	0x63,0x19,
	0x6A,0x09,
	//;;AE/AG/ABLC
	0x13,0x87,
	0x14,0x80,
	0x16,0xC0,
	0x17,0x40,
	0x18,0xB9,
	0x38,0x38,
	0x39,0x92,
	0x4a,0x03,
	0x49,0x10,
	//;;INI End
	0x12,0x00	
	#else
	0x12,0x80,
    0x01,0xFF,
    0x02,0xFF,
    0x0D,0x40,
    0x1F,0x04,
    0x0E,0x1D,
    0x0F,0x09,
    0x10,0x1E,
    0x11,0x80,
    0x20,0x50,
    0x21,0x06,
    0x22,0xF0,
    0x23,0x02,
    0x24,0x10,
    0x25,0xDC,
    0x26,0x25,
    0x27,0x2C,
    0x28,0x0D,
    0x29,0x01,
    0x2A,0x24,
    0x2B,0x29,
    0x2C,0x04,
    0x2D,0x00,
    0x2E,0xB9,
    0x2F,0x00,
    0x30,0x92,
    0x31,0x0A,
    0x32,0xAA,
    0x33,0x14,
    0x34,0x38,
    0x35,0x54,
    0x42,0x41,
    0x43,0x50,
    0x1D,0x00,
    0x1E,0x00, // wilsonjin 20151208
    0x6C,0x50,
    
    0x73,0x33,
    0x70,0x68,
    0x76,0x40,
    0x77,0x06,
    0x78,0x18,
    0x48,0x60,
    0x60,0xA4,
    0x61,0xFF,
    0x62,0x40,
    0x65,0x00,
    0x66,0x20,
    0x67,0x30,
    0x68,0x04,
    0x69,0x74,
    0x6F,0x04,
    0x63,0x51,
    0x6A,0x09,
    0x13,0x87,
    0x14,0x80,
    0x16,0xC0,
    0x17,0x40,
    0x18,0x77,
    0x38,0x35,
    0x39,0x98,
    0x4a,0x03,
    0x49,0x00,
    0x0C,0x41, // Test pattern 
    0x12,0x00,
    #endif
};

// 1288x728 60fps
MMP_USHORT SNR_H42_Reg_1280x720_60P_Customer[] = 
{
    0x12,0x40,
    0x0D,0x40,
    0x1F,0x04,
    0x0E,0x1C,
    0x0F,0x09,
    0x10,0x1E,
    0x11,0x80,
    0x20,0x40,
    0x21,0x06,
    0x22,0xEE,
    0x23,0x02,
    0x24,0x08,
    0x25,0xD4,
    0x26,0x25,
    0x27,0x3B,
    0x28,0x0D,
    0x29,0x01,
    0x2A,0x24,
    0x2B,0x29,
    0x2C,0x04,
    0x2D,0x00,
    0x2E,0xB9,
    0x2F,0x00,
    0x30,0x92,
    0x31,0x0A,
    0x32,0xAA,
    0x33,0x14,
    0x34,0x38,
    0x35,0x54,
    0x42,0x41,
    0x43,0x50,
    0x1D,0x00,
    0x1E,0x00,
    0x6C,0x50,
    0x73,0x33,
    0x70,0x68,
    0x76,0x40,
    0x77,0x06,
    0x78,0x18,
    0x48,0x60,
    0x60,0xA4,
    0x61,0xFF,
    0x62,0x40,
    0x65,0x00,
    0x66,0x20,
    0x67,0x30,
    0x68,0x04,
    0x69,0x74,
    0x6F,0x04,
    0x63,0x51,
    0x6A,0x09,
    0x13,0x87,
    0x14,0x80,
    0x16,0xC0,
    0x17,0x40,
    0x18,0x77,
    0x38,0x35,
    0x39,0x98,
    0x4a,0x03,
    0x49,0x00,
    0x0C, 0x41, // Test pattern 

    0x12,0x00,
};



#if 0
void ____Sensor_Customer_Func____(){ruturn;} // dummy
#endif

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_InitConfig
//  Description :
//------------------------------------------------------------------------------
static void SNR_Cust_InitConfig(void)
{
	MMP_USHORT i;

    printc("SNR_Cust_InitConfig JX-H42,Build Time - %s  %s  \n", __DATE__,__TIME__);

	// Init OPR Table
	SensorCustFunc.OprTable->usInitSize 							= (sizeof(SNR_H42_Reg_Init_Customer)/sizeof(SNR_H42_Reg_Init_Customer[0]))/2;
	SensorCustFunc.OprTable->uspInitTable 							= &SNR_H42_Reg_Init_Customer[0];

    SensorCustFunc.OprTable->bBinTableExist                         = MMP_FALSE;
    SensorCustFunc.OprTable->bInitDoneTableExist                    = MMP_FALSE;

	for (i = 0; i < MAX_SENSOR_RES_MODE; i++)
	{
		SensorCustFunc.OprTable->usSize[i] 							= (sizeof(SNR_H42_Reg_Unsupport)/sizeof(SNR_H42_Reg_Unsupport[0]))/2;
		SensorCustFunc.OprTable->uspTable[i] 						= &SNR_H42_Reg_Unsupport[0];
	}

	SensorCustFunc.OprTable->usSize[RES_IDX_1280x720_30P] 			= (sizeof(SNR_H42_Reg_1280x720_30P_Customer)/sizeof(SNR_H42_Reg_1280x720_30P_Customer[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720_30P] 		= &SNR_H42_Reg_1280x720_30P_Customer[0];

	SensorCustFunc.OprTable->usSize[RES_IDX_1280x720_60P] 			= (sizeof(SNR_H42_Reg_1280x720_60P_Customer)/sizeof(SNR_H42_Reg_1280x720_60P_Customer[0]))/2; // TBD
	SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720_60P] 		= &SNR_H42_Reg_1280x720_60P_Customer[0];

	SensorCustFunc.OprTable->usSize[RES_IDX_960x720_30P] 			= (sizeof(SNR_H42_Reg_1280x720_30P_Customer)/sizeof(SNR_H42_Reg_1280x720_30P_Customer[0]))/2; // TBD
	SensorCustFunc.OprTable->uspTable[RES_IDX_960x720_30P] 		= &SNR_H42_Reg_1280x720_30P_Customer[0];

	// Init Vif Setting : Common
	SensorCustFunc.VifSetting->SnrType                              = MMPF_VIF_SNR_TYPE_BAYER;

#if (SUPPORT_DUAL_SNR)

#if (SENSOR_IF == SENSOR_IF_MIPI_1_LANE)
	SensorCustFunc.VifSetting->OutInterface 						= MMPF_VIF_IF_MIPI_SINGLE_1; // lei.qin
#else
#error "sensor unsupport!!!"
#endif

#else

#if (SENSOR_IF == SENSOR_IF_PARALLEL)
	SensorCustFunc.VifSetting->OutInterface 						= MMPF_VIF_IF_PARALLEL;
#elif (SENSOR_IF == SENSOR_IF_MIPI_1_LANE)
	SensorCustFunc.VifSetting->OutInterface 						= MMPF_VIF_IF_MIPI_SINGLE_0; // wilsonjin 20151208
#elif (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)
    SensorCustFunc.VifSetting->OutInterface                         = MMPF_VIF_IF_MIPI_DUAL_01;
#elif (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)
    SensorCustFunc.VifSetting->OutInterface                         = MMPF_VIF_IF_MIPI_QUAD;
#endif
#endif

    #if (PRM_SENSOR_VIF_ID == MMPF_VIF_MDL_ID1)
    SensorCustFunc.VifSetting->VifPadId							    = MMPF_VIF_MDL_ID1;
    #else
    SensorCustFunc.VifSetting->VifPadId							    = MMPF_VIF_MDL_ID0;
    #endif

	// Init Vif Setting : PowerOn Setting
	SensorCustFunc.VifSetting->powerOnSetting.bTurnOnExtPower 		= MMP_TRUE;
	SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPin 		= SENSOR_GPIO_ENABLE; // it might be defined in Config_SDK.h
	SensorCustFunc.VifSetting->powerOnSetting.bExtPowerPinHigh 		= SENSOR_GPIO_ENABLE_ACT_LEVEL;
	SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPinDelay 	= 0; // 0 ms which is suggested by Kenny Shih @ 20150327
	SensorCustFunc.VifSetting->powerOnSetting.bFirstEnPinHigh 		= MMP_TRUE;
	SensorCustFunc.VifSetting->powerOnSetting.ubFirstEnPinDelay 	= 10;
	SensorCustFunc.VifSetting->powerOnSetting.bNextEnPinHigh 		= MMP_FALSE;
	SensorCustFunc.VifSetting->powerOnSetting.ubNextEnPinDelay 		= 10;
	SensorCustFunc.VifSetting->powerOnSetting.bTurnOnClockBeforeRst = MMP_TRUE;
	SensorCustFunc.VifSetting->powerOnSetting.bFirstRstPinHigh 		= MMP_FALSE;
	SensorCustFunc.VifSetting->powerOnSetting.ubFirstRstPinDelay 	= 30;
	SensorCustFunc.VifSetting->powerOnSetting.bNextRstPinHigh 		= MMP_TRUE;
	SensorCustFunc.VifSetting->powerOnSetting.ubNextRstPinDelay 	= 10;

	// Init Vif Setting : PowerOff Setting
	SensorCustFunc.VifSetting->powerOffSetting.bEnterStandByMode 	= MMP_FALSE;
	SensorCustFunc.VifSetting->powerOffSetting.usStandByModeReg 	= 0x0;
	SensorCustFunc.VifSetting->powerOffSetting.usStandByModeMask 	= 0x0;
	SensorCustFunc.VifSetting->powerOffSetting.bEnPinHigh 			= MMP_TRUE;
	SensorCustFunc.VifSetting->powerOffSetting.ubEnPinDelay 		= 20;
	SensorCustFunc.VifSetting->powerOffSetting.bTurnOffMClock 		= MMP_TRUE;
	SensorCustFunc.VifSetting->powerOffSetting.bTurnOffExtPower 	= MMP_TRUE;
	SensorCustFunc.VifSetting->powerOffSetting.usExtPowerPin 		= SENSOR_GPIO_ENABLE; // it might be defined in Config_SDK.h

	// Init Vif Setting : Sensor MClock Setting
	SensorCustFunc.VifSetting->clockAttr.bClkOutEn 					= MMP_TRUE;
	SensorCustFunc.VifSetting->clockAttr.ubClkFreqDiv 				= 0;
	SensorCustFunc.VifSetting->clockAttr.ulMClkFreq 				= 24000;
	SensorCustFunc.VifSetting->clockAttr.ulDesiredFreq 				= 24000;
	SensorCustFunc.VifSetting->clockAttr.ubClkPhase 				= MMPF_VIF_SNR_PHASE_DELAY_NONE;
	SensorCustFunc.VifSetting->clockAttr.ubClkPolarity 				= MMPF_VIF_SNR_CLK_POLARITY_POS;
	SensorCustFunc.VifSetting->clockAttr.ubClkSrc 					= MMPF_VIF_SNR_CLK_SRC_PMCLK;

	// Init Vif Setting : Parallel Sensor Setting
	SensorCustFunc.VifSetting->paralAttr.ubLatchTiming 				= MMPF_VIF_SNR_LATCH_POS_EDGE;
	SensorCustFunc.VifSetting->paralAttr.ubHsyncPolarity 			= MMPF_VIF_SNR_CLK_POLARITY_POS;
    SensorCustFunc.VifSetting->paralAttr.ubVsyncPolarity            = MMPF_VIF_SNR_CLK_POLARITY_POS;
    SensorCustFunc.VifSetting->paralAttr.ubBusBitMode               = MMPF_VIF_SNR_PARAL_BITMODE_16;

	// Init Vif Setting : MIPI Sensor Setting
	SensorCustFunc.VifSetting->mipiAttr.bClkDelayEn 				= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.bClkLaneSwapEn 				= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.usClkDelay 					= 0;
	SensorCustFunc.VifSetting->mipiAttr.ubBClkLatchTiming 			= MMPF_VIF_SNR_LATCH_NEG_EDGE;
#if (SENSOR_IF == SENSOR_IF_MIPI_1_LANE)
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[0] 				= MMP_TRUE;
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[1] 				= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[2] 				= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[3] 				= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[0] 			= MMP_TRUE;
	SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[1] 			= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[2] 			= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[3] 			= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[0] 			= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[1] 			= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[2] 			= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[3] 			= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[0]			= (SensorCustFunc.VifSetting->VifPadId == MMPF_VIF_MDL_ID0)? MMPF_VIF_MIPI_DATA_SRC_PHY_0: MMPF_VIF_MIPI_DATA_SRC_PHY_1; 
	SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[1] 			= MMPF_VIF_MIPI_DATA_SRC_PHY_UNDEF;
	SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[2] 			= MMPF_VIF_MIPI_DATA_SRC_PHY_UNDEF;
	SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[3] 			= MMPF_VIF_MIPI_DATA_SRC_PHY_UNDEF;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[0] 				= 0x08;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[1] 				= 0x08;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[2] 				= 0x08;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[3] 				= 0x08;
	SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[0] 			= 0x0e;
	SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[1] 			= 0x1F;
	SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[2] 			= 0x1F;
	SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[3] 			= 0x1F;
#elif (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[0]              = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[1]              = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[2]              = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[3]              = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[0]             = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[1]             = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[2]             = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[3]             = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[0]          = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[1]          = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[2]          = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[3]          = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[0]            = (SensorCustFunc.VifSetting->VifPadId == MMPF_VIF_MDL_ID0)? MMPF_VIF_MIPI_DATA_SRC_PHY_0 : MMPF_VIF_MIPI_DATA_SRC_PHY_1;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[1]            = (SensorCustFunc.VifSetting->VifPadId == MMPF_VIF_MDL_ID0)? MMPF_VIF_MIPI_DATA_SRC_PHY_1 : MMPF_VIF_MIPI_DATA_SRC_PHY_2;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[2]            = MMPF_VIF_MIPI_DATA_SRC_PHY_UNDEF;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[3]            = MMPF_VIF_MIPI_DATA_SRC_PHY_UNDEF;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[0]              = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[1]              = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[2]              = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[3]              = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[0]             = 0x0F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[1]             = 0x0F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[2]             = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[3]             = 0x1F;
#endif

	// Init Vif Setting : Color ID Setting
#if (SENSOR_ROTATE_180)
	SensorCustFunc.VifSetting->colorId.VifColorId 					= MMPF_VIF_COLORID_01;
#else
	SensorCustFunc.VifSetting->colorId.VifColorId 					= MMPF_VIF_COLORID_11;
#endif
	SensorCustFunc.VifSetting->colorId.CustomColorId.bUseCustomId 	= MMP_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_DoAE_FrmSt
//  Description :
//------------------------------------------------------------------------------

#define MAX_SENSOR_GAIN		(16)
#define ISP_DGAIN_BASE		(0x200)

ISP_UINT32 dgain;

void SNR_Cust_DoAE_FrmSt(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
#if (ISP_EN)
	MMP_UBYTE ubPeriod 				= (SensorCustFunc.pAeTime)->ubPeriod;
	MMP_UBYTE ubFrmStSetShutFrmCnt 	= (SensorCustFunc.pAeTime)->ubFrmStSetShutFrmCnt;
	MMP_UBYTE ubFrmStSetGainFrmCnt 	= (SensorCustFunc.pAeTime)->ubFrmStSetGainFrmCnt;

	// For AE curve
	if ((ulFrameCnt % 1000) == 10)
	{
		ISP_IF_CMD_SendCommandtoAE(0x51, AE_Bias_tbl,AE_tbl_size, 0); // <<AE table set once at preview start
		ISP_IF_NaturalAE_Enable(2); // 0: no, 1: ENERGY, 2: Lux, 3: test mode
		ISP_IF_F_SetWDREn(1);
		//ISP_IF_AE_SetFlicker(ISP_AE_FLICKER_60HZ); // temp for test
	}
	if (ulFrameCnt % ubPeriod == ubFrmStSetShutFrmCnt)
	{
		ISP_IF_AE_Execute();
		gsSensorFunction->MMPF_Sensor_SetShutter(ubSnrSel, 0, 0);

		//RTNA_DBG_Short(0,ISP_IF_AE_GetGain());
		//RTNA_DBG_Short(0,ISP_IF_AE_GetShutter());
		//RTNA_DBG_Long(0,ISP_IF_AE_GetGain()*ISP_IF_AE_GetShutter());
		//RTNA_DBG_Short(0,ISP_IF_AE_GetShutterBase());
		//RTNA_DBG_Short(0,ISP_IF_AE_GetShutterBase()/ISP_IF_AE_GetShutter());
		//RTNA_DBG_Short(0,ISP_IF_AE_GetLightCond());
		//RTNA_DBG_Str(0,"\r\n");
	}
	else if ((ulFrameCnt % ubPeriod) == ubFrmStSetGainFrmCnt)
	{
		s_gain = VR_MAX(ISP_IF_AE_GetGain(), ISP_IF_AE_GetGainBase());

		if (s_gain >= (ISP_IF_AE_GetGainBase() * MAX_SENSOR_GAIN))
		{
			s_gain  = ISP_IF_AE_GetGainBase() * MAX_SENSOR_GAIN;
		}
	    gsSensorFunction->MMPF_Sensor_SetGain(ubSnrSel, s_gain);

	}
#endif
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_DoAE_FrmEnd
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_DoAE_FrmEnd(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_DoAWB
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_DoAWB(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_DoIQ
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_DoIQ(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
#ifdef CFG_CUS_DO_IQ_OPERATION // for SP86 only? TBD
	MMP_ULONG ulval;
  	MMP_ULONG ulratio;
  	//MMP_ULONG ultmp;
	MMP_ULONG ulRgain_Uth = 0x500;
	MMP_ULONG ulRgain_Lth = 0x430; //after adj AWB 0x400
	MMP_ULONG usccm_d[6] = { 8,137,1, 8,127,7}; //137 - 8 = 128; 8+127 = 135
	//MMP_ULONG usccm_a[6] = {17,146,1,36, 99,7}; //146 - 17 = 129; 36+99 = 135
	MMP_ULONG usccm_a[6] = {17,146,1,24, 111,7}; //146 - 17 = 129; 36+99 = 135
	MMP_ULONG usccm[6],i;
	//MMP_USHORT usLux_Uth  = 20;
	//MMP_USHORT usLux_Lth  = 2; 
	//MMP_USHORT usMaxRatio = 0x80; //max shading ratio
	//MMP_USHORT usMinRatio = 0x20; //min shading ratio
	MMP_SHORT  sSnrModeOffset;
	//static MMP_SHORT  sLSratio = 0x80;
	static MMP_SHORT  sMeterMode = 0; //average
	MMP_SHORT sPreMeterMode;

	// Update ccm
 	ulval = ISP_IF_AWB_GetGainR();

 	if (ulval >= ulRgain_Uth) {
 		for (i = 0; i < 6; i++)
 			usccm[i] = usccm_d[i];

 	}
 	else if (ulval <= ulRgain_Lth) {
 		for(i = 0; i < 6; i++)
 			usccm[i] = usccm_a[i];
 	}
 	else {
 		ulratio = 256 * (ulval - ulRgain_Lth) / (ulRgain_Uth - ulRgain_Lth);
 		
 		for (i = 0; i < 6; i++){
 			usccm[i] = (usccm_d[i] * ulratio + (256 - ulratio) * usccm_a[i])/256;
 		}
 	}
 	
 	for (i = 0; i < 6; i++) {
 		ISP_IF_IQ_SetOpr(0x705d+i, 1, usccm[i]); 
 	}

 	if (ulFrameCnt % 6 == 5) {
	 	ulval = ISP_IF_AE_GetLightCond();
	 	
	 	sPreMeterMode = ISP_IF_AE_GetMetering();
	 	
	 	if(1) //if (MenuSettingConfig()->uiMOVSize == 0) //FHD //TBD. Don't call menusetting here!
	 	{
	 		if (sPreMeterMode == 2) {
	 			ISP_IF_AE_SetMetering(0);
	 		}
	 		else if (sPreMeterMode == 3) {
	 			ISP_IF_AE_SetMetering(1);
	 		}
	 		
	 		sSnrModeOffset = 0;
	 	}
	 	else {
	 		if (sPreMeterMode == 0) {
	 			ISP_IF_AE_SetMetering(2);
	 		}
	 		else if (sPreMeterMode == 1) {
	 			ISP_IF_AE_SetMetering(3);
	 		}
	 		
	 		sSnrModeOffset = 2;
	 	}
	 	
		if (ulval < 50 && sMeterMode ==0) { //switch to center weight
	 		sMeterMode = 1;
	 		ISP_IF_AE_SetMetering(ISP_AE_METERING_CENTER + sSnrModeOffset);
	 	}
	 	else if (ulval > 150 && sMeterMode ==1) {//switch to average weight
	 		sMeterMode = 0;
	 		ISP_IF_AE_SetMetering(ISP_AE_METERING_AVERAGE + sSnrModeOffset);	 	
	 	}
    }
#endif
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetGain
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetGain(MMP_UBYTE ubSnrSel, MMP_ULONG ulGain)
{
#if (ISP_EN)
    MMP_USHORT s_gain, gainbase;
    MMP_ULONG  H, L;

	gainbase = ISP_IF_AE_GetGainBase();

  	s_gain = VR_MIN(VR_MAX(ulGain, gainbase), gainbase*16-1); // API input gain range : 64~511, 64=1X

    if (s_gain >= gainbase * 8) {
        H = 3;
		L = s_gain * 16 / gainbase / 8 - 16;         
    } else if (s_gain >= gainbase * 4) {
        H = 2;
		L = s_gain * 16 / gainbase / 4 - 16;         
    } else if (s_gain >= gainbase * 2) {
        H = 1;
		L = s_gain * 16 / gainbase / 2 - 16;        
    } else {
        H = 0;
   		L = s_gain * 16 / gainbase / (H+1) - 16;
	}
    if (L > 15) L = 15;	   
  
    // set sensor gain
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x00, (H<<4)+L);     //Total gain = (2^PGA[6:4])*(1+PGA[3:0]/16) //PGA:0x00   

#endif
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetShutter
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetShutter(MMP_UBYTE ubSnrSel, MMP_ULONG shutter, MMP_ULONG vsync)
{
#if (ISP_EN)
    ISP_UINT32 new_vsync;
    ISP_UINT32 new_shutter;

    #if 1 //if sensor_common.c have defined "g_SNR_LineCntPerSec", can open this region
	if(shutter == 0 | vsync == 0)
	{
		new_vsync = gSnrLineCntPerSec[ubSnrSel] * ISP_IF_AE_GetVsync() / ISP_IF_AE_GetVsyncBase();
   		new_shutter = gSnrLineCntPerSec[ubSnrSel] * ISP_IF_AE_GetShutter() / ISP_IF_AE_GetShutterBase();
	}else
	{
		new_vsync = gSnrLineCntPerSec[ubSnrSel] * vsync / ISP_IF_AE_GetVsyncBase();
		new_shutter = gSnrLineCntPerSec[ubSnrSel] * shutter / ISP_IF_AE_GetShutterBase();
	}
	#endif
	
    new_vsync   = VR_MAX(new_vsync, new_shutter + 5);  //+8
    new_shutter = VR_MIN(VR_MAX(new_shutter, 1), new_vsync - 5);//2);

	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x23, new_vsync >> 8); 
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x22, new_vsync); 

	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x02, new_shutter >> 8); 
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x01, new_shutter); 

#endif
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetFlip
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetFlip(MMP_UBYTE ubSnrSel, MMP_UBYTE ubMode)
{
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetRotate
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetRotate(MMP_UBYTE ubSnrSel, MMP_UBYTE ubMode)
{
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_CheckVersion
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_CheckVersion(MMP_UBYTE ubSnrSel, MMP_ULONG *pulVersion)
{
	// TBD
}

MMPF_SENSOR_CUSTOMER SensorCustFunc = 
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
    if (SensorCustFunc.sPriority == MMP_SNR_PRIO_PRM)
        MMPF_SensorDrv_Register(PRM_SENSOR, &SensorCustFunc);
    else
        MMPF_SensorDrv_Register(SCD_SENSOR, &SensorCustFunc);

    return 0;
}

#pragma arm section code = "initcall6", rodata = "initcall6", rwdata = "initcall6",  zidata = "initcall6" 
#pragma O0
ait_module_init(SNR_Module_Init);
#pragma
#pragma arm section rodata, rwdata, zidata

#endif // (BIND_SENSOR_H42_MIPI)
#endif // (SENSOR_EN)

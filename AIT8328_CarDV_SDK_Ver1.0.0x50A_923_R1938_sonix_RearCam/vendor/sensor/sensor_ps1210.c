//==============================================================================
//
//  File        : sensor_PS1210.c
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


#if (1)
#if (1)

#include "mmpf_pll.h"
#include "mmpf_sensor.h"
#include "Sensor_Mod_Remapping.h"
#include "isp_if.h"

//==============================================================================
//
//                              GLOBAL VARIABLE
//
//==============================================================================

// Resolution Table
MMPF_SENSOR_RESOLUTION m_SensorRes = 
{
	1, 																				                	// ubSensorModeNum
	0, 																				                		// ubDefPreviewMode
	0, 																					                	// ubDefCaptureMode
	3000,                                                                                                   // usPixelSize (TBD)
// Mode0
	{1}, // usVifGrabStX
	{1}, // usVifGrabStY
	{1928}, // usVifGrabW
	{1088}, // usVifGrabH
#if (CHIP == MCR_V2)
	{1}, // usBayerInGrabX
	{1}, // usBayerInGrabY
	{8}, // usBayerInDummyX
	{8}, // usBayerInDummyY
	{1920}, // usBayerOutW
	{1080}, // usBayerOutH
#endif
	{1920}, // usScalInputW
	{1080}, // usScalInputH
	{300}, // usTargetFpsx10
	{1100}, // usVsyncLine
	{1}, // ubWBinningN
	{1}, // ubWBinningM
	{1}, // ubHBinningN
	{1}, // ubHBinningM
	{0xFF}, // ubCustIQmode
	{0xFF}  // ubCustAEmode
};

// OPR Table and Vif Setting
MMPF_SENSOR_OPR_TABLE 	m_OprTable;
MMPF_SENSOR_VIF_SETTING m_VifSetting;

// IQ Table
const ISP_UINT8 Sensor_IQ_CompressedText[] = 
{
#ifdef CUS_ISP_8428_IQ_DATA     // maybe defined in project MCP or Config_SDK.h
#include CUS_ISP_8428_IQ_DATA
#else
#include "isp_8428_iq_data_v2_IMX322.xls.ciq.txt"
#endif
};

// I2cm Attribute
static MMP_I2CM_ATTR m_I2cmAttr = 
{
	MMP_I2CM0,      // i2cmID
	(0xEE >> 1), 	// ubSlaveAddr
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
	150000, 		// ulI2cmSpeed 150KHZ
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
	4, 	// ubPeriod
	0, 	// ubFrmStSetShutFrmCnt
	0 	// ubFrmStSetGainFrmCnt
};

MMPF_SENSOR_AFTIMIMG m_AfTime = 
{
	1, 	// ubPeriod
	0 	// ubDoAFFrmCnt
};
#if 0
ISP_UINT16 PS1210_GainTable[] =
{
	64	,	//	0
	66	,	//	1
	69	,	//	2
	71	,	//	3
	73	,	//	4
	76	,	//	5
	79	,	//	6
	82	,	//	7
	84	,	//	8
	87	,	//	9
	90	,	//	10
	94	,	//	11
	97	,	//	12
	100	,	//	13
	104	,	//	14
	107	,	//	15
	111	,	//	16
	115	,	//	17
	119	,	//	18
	123	,	//	19
	128	,	//	20
	132	,	//	21
	137	,	//	22
	142	,	//	23
	147	,	//	24
	152	,	//	25
	157	,	//	26
	163	,	//	27
	168	,	//	28
	174	,	//	29
	180	,	//	30
	187	,	//	31
	193	,	//	32 3X
	200	,	//	33
	207	,	//	34
	214	,	//	35
	222	,	//	36
	230	,	//	37
	238	,	//	38
	246	,	//	39
	255	,	//	40
	264	,	//	41
	273	,	//	42
	283	,	//	43
	293	,	//	44
	303	,	//	45
	313	,	//	46
	324	,	//	47
	336	,	//	48
	348	,	//	49
	360	,	//	50
	373	,	//	51
	386	,	//	52
	399	,	//	53
	413	,	//	54
	428	,	//	55
	443	,	//	56
	458	,	//	57
	474	,	//	58
	491	,	//	59
	508	,	//	60
	526	,	//	61
	545	,	//	62
	564	,	//	63
	584	,	//	64  9.12 X
	604	,	//	65
	625	,	//	66
	647	,	//	67
	670	,	//	68
	694	,	//	69
	718	,	//	70
	743	,	//	71
	769	,	//	72
	796	,	//	73
	824	,	//	74
	853	,	//	75
	883	,	//	76
	914	,	//	77
	947	,	//	78
	980	,	//	79
	1014	//	80  15.84 X
	/* reserved 
	1050	,
	1087	,
	1125	,
	1165	,
	1206	,
	1248	,
	1292	,
	1337	,
	1384	,
	1433	,
	1483	,
	1535	,
	1589	,
	1645	,
	1703	,
	1763	,
	1825	,
	1889	,
	1955	,
	2024	,
	2095	,
	2169	,
	2245	,
	2324	,
	2405	,
	2490	, //106	
	2577	  // 40 X
	*/
};
#endif
#if 1
//8427 HB TABLE,141-O
//ISP_UINT16 PS1210_GainTable[] = {
const ISP_UINT16 PS1210_GainTable[141] = {
	256,
	265,
	274,
	284,
	294,
	304,
	315,
	326,
	338,
	350,
	362,
	375,
	388,
	402,
	416,
	431,
	446,
	461,
	478,
	495,
	512,
	530,
	549,
	568,
	588,
	609,
	630,
	653,
	676,
	699,
	724,
	750,
	776,
	803,
	832,
	861,
	891,
	923,
	955,
	989,
	1024,
	1060,
	1097,
	1136,
	1176,
	1218,
	1261,
	1305,
	1351,
	1399,
	1448,
	1499,
	1552,
	1607,
	1663,
	1722,
	1783,
	1846,
	1911,
	1978,
	2048,
	2120,
	2195,
	2272,
	2353,
	2435,
	2521,
	2610,
	2702,
	2798,
	2896,
	2998,
	3104,
	3214,
	3327,
	3444,
	3566,
	3692,
	3822,
	3956,
	4096,
	4240,
	4390,
	4545,
	4705,
	4871,
	5043,
	5221,
	5405,
	5595,
	5793,
	5997,
	6208,
	6427,
	6654,
	6889,
	7132,
	7383,
	7643,
	7913,
	8192,
	8481,
	8780,
	9090,
	9410,
	9742,
	10086,
	10441,
	10809,
	11191,
	11585,
	11994,
	12417,
	12855,
	13308,
	13777,
	14263,
	14766,
	15287,
	15826,
	16384,
	16962,
	17560,
	18179,
	18820,
	19484,
	20171,
	20882,
	21619,
	22381,
	23170,
	23988,
	24834,
	25709,
	26616,
	27554,
	28526,
	29532,
	30574,
	31652,
	32768,
};
#endif

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

#if 0
void ____Sensor_Init_OPR_Table____(){ruturn;} //dummy
#endif

ISP_UINT16 SNR_PS1210_Reg_Unsupport[] =
{
    SENSOR_DELAY_REG, 100 // delay
};

ISP_UINT16 SNR_PS1210_Reg_Init_Customer[] =
{
	// TBD
	SENSOR_DELAY_REG, 100 // delay
};

#if 0
void ____Sensor_Res_OPR_Table____(){ruturn;} //dummy
#endif


ISP_UINT16 SNR_PS1210_Reg_1920x1080_30P[] =
{
    0x03, 0x00,
    0x29, 0x98,     //# output Hi-z release

    0x03, 0x00,
    0x05, 0x03,     //# mirror/flip
    0x06, 0x08,     //# framewidth_h        (08)
    0x07, 0x97,     //# framewidth_l        (97)
    0x08, 0x04,     //# framehight_h        (04)
    0x09, 0x64,     //# framehight_l        (64)

    0x14, 0x00,     //# vsyncstartrow_f0_h    (00)
    0x15, 0x0B,     //# vsyncstartrow_f0_l    (0D)

    0x25, 0x10,     //# CLK DIV1

    0x33, 0x01,     //# pixelbias
    0x34, 0x02,     //# compbias

    0x36, 0x88,     //# TX_Bias; DCDC 4.96 V, LDO 4.37 V => DCDC 4.48 V, LDO 4.37 V ( C8h => 88h )
    0x38, 0x48,     //# black_bias, range_sel 0.4 V
    0x3A, 0x22,     //# main regulator output

    0x41, 0x21,     //# pll_m_cnt (21)
    0x42, 0x04,     //# pll_r_cnt (04)

    0x40, 0x10,     //# pll_control
    SENSOR_DELAY_REG , 500,
    0x40, 0x00,     //# pll_control on

    0x03, 0x01,
    0x26, 0x03,     //# blacksun_th_h

    0x03, 0x01,
    0xC0, 0x04,     //# inttime_h
    0xC1, 0x5F,     //# inttime_m
    0xC2, 0x00,     //# inttime_l
    0xC3, 0x00,     //# globalgain
    0xC4, 0x40,     //# digitalgain


};

ISP_UINT16 SNR_PS1210_Reg_1280x720_30P[] =
{
     // TBD
     SENSOR_DELAY_REG, 100 // delay
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
	MMP_USHORT 	i;
	MMP_ULONG 	ulSensorClkSrc;
	MMP_ULONG 	ulSensorMCLK = 27000; // 27 M

    RTNA_DBG_Str(0, "SNR_Cust_InitConfig PS1210\r\n");

	// Init OPR Table
	SensorCustFunc.OprTable->usInitSize 							= (sizeof(SNR_PS1210_Reg_Init_Customer)/sizeof(SNR_PS1210_Reg_Init_Customer[0]))/2;
	SensorCustFunc.OprTable->uspInitTable 							= &SNR_PS1210_Reg_Init_Customer[0];

    SensorCustFunc.OprTable->bBinTableExist                         = MMP_FALSE;
    SensorCustFunc.OprTable->bInitDoneTableExist                    = MMP_FALSE;

	for (i = 0; i < MAX_SENSOR_RES_MODE; i++)
	{
		SensorCustFunc.OprTable->usSize[i] 							= (sizeof(SNR_PS1210_Reg_Unsupport)/sizeof(SNR_PS1210_Reg_Unsupport[0]))/2;
		SensorCustFunc.OprTable->uspTable[i] 						= &SNR_PS1210_Reg_Unsupport[0];
	}

	// 16:9
	SensorCustFunc.OprTable->usSize[RES_IDX_1920x1080_30P]			= (sizeof(SNR_PS1210_Reg_1920x1080_30P)/sizeof(SNR_PS1210_Reg_1920x1080_30P[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_1920x1080_30P] 		= &SNR_PS1210_Reg_1920x1080_30P[0];


	// Init Vif Setting : Common
    SensorCustFunc.VifSetting->SnrType                              = MMPF_VIF_SNR_TYPE_BAYER;
#if (SENSOR_IF == SENSOR_IF_PARALLEL)
	SensorCustFunc.VifSetting->OutInterface 						= MMPF_VIF_IF_PARALLEL;
#elif (SENSOR_IF == SENSOR_IF_MIPI_1_LANE)
	SensorCustFunc.VifSetting->OutInterface 						= MMPF_VIF_IF_MIPI_SINGLE_0;
#elif (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)
	SensorCustFunc.VifSetting->OutInterface 						= MMPF_VIF_IF_MIPI_DUAL_01;
#elif (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)
	SensorCustFunc.VifSetting->OutInterface							= MMPF_VIF_IF_MIPI_QUAD;
#endif
    #if (SUPPORT_DUAL_SNR)
    SensorCustFunc.VifSetting->VifPadId							    = MMPF_VIF_MDL_ID1;
    #else
	SensorCustFunc.VifSetting->VifPadId 							= MMPF_VIF_MDL_ID0;
    #endif

		// Init Vif Setting : PowerOn Setting
	SensorCustFunc.VifSetting->powerOnSetting.bTurnOnExtPower 		= MMP_TRUE;
	SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPin 		= SENSOR_GPIO_ENABLE; // it might be defined in Config_SDK.h
	SensorCustFunc.VifSetting->powerOnSetting.bExtPowerPinHigh 		= MMP_FALSE;//SENSOR_GPIO_ENABLE_ACT_LEVEL;
	SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPinDelay 	= 50;
	SensorCustFunc.VifSetting->powerOnSetting.bFirstEnPinHigh 		= MMP_TRUE;
	SensorCustFunc.VifSetting->powerOnSetting.ubFirstEnPinDelay 	= 10;
	SensorCustFunc.VifSetting->powerOnSetting.bNextEnPinHigh 		= MMP_FALSE;
	SensorCustFunc.VifSetting->powerOnSetting.ubNextEnPinDelay 		= 100;
	SensorCustFunc.VifSetting->powerOnSetting.bTurnOnClockBeforeRst = MMP_TRUE;
	SensorCustFunc.VifSetting->powerOnSetting.bFirstRstPinHigh 		= MMP_FALSE;
	SensorCustFunc.VifSetting->powerOnSetting.ubFirstRstPinDelay 	= 100;
	SensorCustFunc.VifSetting->powerOnSetting.bNextRstPinHigh 		= MMP_TRUE;
	SensorCustFunc.VifSetting->powerOnSetting.ubNextRstPinDelay 	= 100;

	// Init Vif Setting : PowerOff Setting
	SensorCustFunc.VifSetting->powerOffSetting.bEnterStandByMode 	= MMP_FALSE;
	SensorCustFunc.VifSetting->powerOffSetting.usStandByModeReg 	= 0x00;
	SensorCustFunc.VifSetting->powerOffSetting.usStandByModeMask 	= 0x00;
	SensorCustFunc.VifSetting->powerOffSetting.bEnPinHigh 			= MMP_TRUE;
	SensorCustFunc.VifSetting->powerOffSetting.ubEnPinDelay 		= 20;
	SensorCustFunc.VifSetting->powerOffSetting.bTurnOffMClock 		= MMP_TRUE;
	SensorCustFunc.VifSetting->powerOffSetting.bTurnOffExtPower 	= MMP_TRUE;
	SensorCustFunc.VifSetting->powerOffSetting.usExtPowerPin 		= SENSOR_GPIO_ENABLE; // it might be defined in Config_SDK.h

	// Init Vif Setting : Sensor MClock Setting
	MMPF_PLL_GetGroupFreq(CLK_GRP_SNR, &ulSensorClkSrc);

	SensorCustFunc.VifSetting->clockAttr.bClkOutEn 					= MMP_TRUE;
	SensorCustFunc.VifSetting->clockAttr.ubClkFreqDiv 				= 22;
	SensorCustFunc.VifSetting->clockAttr.ulMClkFreq 				= 27000;
	SensorCustFunc.VifSetting->clockAttr.ulDesiredFreq 				= 27000;
	SensorCustFunc.VifSetting->clockAttr.ubClkPhase 				= MMPF_VIF_SNR_PHASE_DELAY_NONE;
	SensorCustFunc.VifSetting->clockAttr.ubClkPolarity 				= MMPF_VIF_SNR_CLK_POLARITY_POS;
	SensorCustFunc.VifSetting->clockAttr.ubClkSrc 					= MMPF_VIF_SNR_CLK_SRC_VIFCLK;

	// Init Vif Setting : Parallel Sensor Setting
	SensorCustFunc.VifSetting->paralAttr.ubLatchTiming 				= MMPF_VIF_SNR_LATCH_POS_EDGE;
	SensorCustFunc.VifSetting->paralAttr.ubHsyncPolarity 			= MMPF_VIF_SNR_CLK_POLARITY_POS;
	SensorCustFunc.VifSetting->paralAttr.ubVsyncPolarity 			= MMPF_VIF_SNR_CLK_POLARITY_POS;
    SensorCustFunc.VifSetting->paralAttr.ubBusBitMode               = MMPF_VIF_SNR_PARAL_BITMODE_10;
    
	// Init Vif Setting : MIPI Sensor Setting
	SensorCustFunc.VifSetting->mipiAttr.bClkDelayEn 				= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.bClkLaneSwapEn 				= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.usClkDelay 					= 0;
	SensorCustFunc.VifSetting->mipiAttr.ubBClkLatchTiming 			= MMPF_VIF_SNR_LATCH_POS_EDGE;
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
	SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[0] 			= MMPF_VIF_MIPI_DATA_SRC_PHY_0;
	SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[1] 			= MMPF_VIF_MIPI_DATA_SRC_PHY_UNDEF;
	SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[2] 			= MMPF_VIF_MIPI_DATA_SRC_PHY_UNDEF;
	SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[3] 			= MMPF_VIF_MIPI_DATA_SRC_PHY_UNDEF;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[0] 				= 0x08;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[1] 				= 0x08;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[2] 				= 0x08;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[3] 				= 0x08;
	SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[0] 			= 0x1F;
	SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[1] 			= 0x1F;
	SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[2] 			= 0x1F;
	SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[3] 			= 0x1F;
#elif (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[0] 				= MMP_TRUE;
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[1] 				= MMP_TRUE;
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[2] 				= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[3] 				= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[0] 			= MMP_TRUE;
	SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[1] 			= MMP_TRUE;
	SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[2] 			= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[3] 			= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[0] 			= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[1] 			= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[2] 			= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[3] 			= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[0] 			= MMPF_VIF_MIPI_DATA_SRC_PHY_0;
	SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[1] 			= MMPF_VIF_MIPI_DATA_SRC_PHY_1;
	SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[2] 			= MMPF_VIF_MIPI_DATA_SRC_PHY_UNDEF;
	SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[3] 			= MMPF_VIF_MIPI_DATA_SRC_PHY_UNDEF;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[0] 				= 0x08;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[1] 				= 0x08;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[2] 				= 0x08;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[3] 				= 0x08;
	SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[0] 			= 0x1F;
	SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[1] 			= 0x1F;
	SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[2] 			= 0x1F;
	SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[3] 			= 0x1F;
#elif (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[0] 				= MMP_TRUE;
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[1] 				= MMP_TRUE;
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[2] 				= MMP_TRUE;
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[3] 				= MMP_TRUE;
	SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[0] 			= MMP_TRUE;
	SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[1] 			= MMP_TRUE;
	SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[2] 			= MMP_TRUE;
	SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[3] 			= MMP_TRUE;
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[0] 			= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[1] 			= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[2] 			= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[3] 			= MMP_FALSE;
	SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[0] 			= MMPF_VIF_MIPI_DATA_SRC_PHY_0;
	SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[1] 			= MMPF_VIF_MIPI_DATA_SRC_PHY_1;
	SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[2] 			= MMPF_VIF_MIPI_DATA_SRC_PHY_2;
	SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[3] 			= MMPF_VIF_MIPI_DATA_SRC_PHY_3;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[0] 				= 0x08;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[1] 				= 0x08;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[2] 				= 0x08;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[3] 				= 0x08;
	SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[0] 			= 0x1F;
	SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[1] 			= 0x1F;
	SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[2] 			= 0x1F;
	SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[3] 			= 0x1F;
#endif

	// Init Vif Setting : Color ID Setting
	SensorCustFunc.VifSetting->colorId.VifColorId 					= MMPF_VIF_COLORID_11;
	SensorCustFunc.VifSetting->colorId.CustomColorId.bUseCustomId 	= MMP_FALSE;
}

//20150526
//lv1-7, 2	3	6	11	17	30	60
#if 1//	new extent node for18//LV1,		LV2,		LV3,		LV4,		LV5,		LV6,		LV7,		LV8,		LV9,		LV10,	LV11,	LV12,	LV13,	LV14,	LV15,	LV16 	LV17  	LV18
//abby curve iq 12
ISP_UINT32 AE_Bias_tbl[54] =
/*lux*/						{2,			3,			5,			9,			16, 		31, 		63, 		122, 		252,		458,	836,	1684,	3566,	6764,	13279,	27129,	54640, 108810/*930000=LV17*/
/*ENG*/						,0x2FFFFFF, 4841472*2,	3058720,	1962240,	1095560,  	616000, 	334880, 	181720,     96600,	 	52685,	27499,	14560,	8060,	4176,	2216,	1144,	600,   300
/*Tar*/						,43,		55,		 	64,	        75,			82,	 		94,	 		110,	 	122,	    135,	    143,	155,	170,	185,	195,	220,	240,	250,   270
 };	
#define AE_tbl_size  (18)	//32  35  44  50
#endif

#define AGAIN_1X  0x100
#define DGAIN_1X  0x100
#define IMX122_MaxGain 128

static ISP_UINT32	again, dgain, s_gain, tmp_shutter;
ISP_UINT32 isp_gain;

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_DoAE_FrmSt
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_DoAE_FrmSt(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
#if 1
	static ISP_UINT16 ae_gain;
	static ISP_UINT16 ae_shutter;
	static ISP_UINT16 ae_vsync;
	
	MMP_ULONG ulVsync = 0;
	MMP_ULONG ulShutter = 0;
	
	if (ulFrameCnt < 2) {
		return ;
	}

	if (ulFrameCnt % 100 == 10) {
		ISP_IF_CMD_SendCommandtoAE(0x51,AE_Bias_tbl,AE_tbl_size,0);         // <<AE table set once at preview start
		ISP_IF_NaturalAE_Enable(2);	//0: no , 1:ENERGY 2: Lux 3: test mode
	}

	if ((ulFrameCnt % m_AeTime.ubPeriod) == 0) 
	{
		ISP_IF_AE_Execute();

		ae_gain 	= ISP_IF_AE_GetGain();
		ae_shutter 	= ISP_IF_AE_GetShutter();
		ae_vsync 	= ISP_IF_AE_GetVsync();
	}
	
	ulVsync 	= (gSnrLineCntPerSec[ubSnrSel] * ISP_IF_AE_GetVsync()) / ISP_IF_AE_GetVsyncBase();
	ulShutter 	= (gSnrLineCntPerSec[ubSnrSel] * ISP_IF_AE_GetShutter()) / ISP_IF_AE_GetShutterBase();
	
	dgain = DGAIN_1X;
	dgain = dgain * 4 * ulShutter / (4*ulShutter -1);
	s_gain= ae_gain * dgain/DGAIN_1X;
	
	if( s_gain > AGAIN_1X * IMX122_MaxGain )
	{
		s_gain = AGAIN_1X * IMX122_MaxGain;
		dgain  = DGAIN_1X * s_gain / AGAIN_1X*IMX122_MaxGain;
	}
    else
	{
	    dgain = DGAIN_1X;
	}

	if ((ulFrameCnt % m_AeTime.ubPeriod) == m_AeTime.ubFrmStSetShutFrmCnt)
		gsSensorFunction->MMPF_Sensor_SetShutter(ubSnrSel, ulShutter, ulVsync);
	
	if ((ulFrameCnt % m_AeTime.ubPeriod) == m_AeTime.ubFrmStSetGainFrmCnt)
		gsSensorFunction->MMPF_Sensor_SetGain(ubSnrSel, s_gain);
	
	//casio
	if ((ulFrameCnt % m_AeTime.ubPeriod) == 2)
		ISP_IF_IQ_SetAEGain(dgain, ISP_IF_AE_GetGainBase());
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
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetGain
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetGain(MMP_UBYTE ubSnrSel, MMP_ULONG ulGain)
{
#if 1
	ISP_UINT8 i, sensor_gain;

	for( i = 0; i< 141; i++)
	{
		if (ulGain >= PS1210_GainTable[i])
			sensor_gain = i;
		else
			break;
	}

	dgain = dgain*ulGain/PS1210_GainTable[sensor_gain];
	
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x03, 0x01);//bank GroupB
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0xC3, sensor_gain);
#endif
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetShutter
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetShutter(MMP_UBYTE ubSnrSel, MMP_ULONG shutter, MMP_ULONG vsync)
{
#if 1
	ISP_UINT32 new_vsync 	= vsync;
	ISP_UINT32 new_shutter 	= shutter;

	new_vsync 	= ISP_MIN(ISP_MAX((new_shutter + 3), new_vsync), 0xFFFF);
	new_shutter = ISP_MIN(ISP_MAX(new_shutter, 1), (new_vsync - 3));

	// shutter
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x03, 0x01);//bank GroupB
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0xC0, (new_vsync >> 8));
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0xC1, new_vsync);

	// vsync
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x1A,  (((new_vsync - new_shutter) >> 8) & 0xFF));
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x1B, (( new_vsync - new_shutter) & 0xFF));
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
    if (SensorCustFunc.sPriority == MMP_SNR_PRIO_PRM)
        MMPF_SensorDrv_Register(PRM_SENSOR, &SensorCustFunc);
    else
        MMPF_SensorDrv_Register(SCD_SENSOR, &SensorCustFunc); 
    
    return 0;
}

#pragma arm section code = "initcall6", rodata = "initcall6", rwdata = "initcall6", zidata = "initcall6"
#pragma O0
ait_module_init(SNR_Module_Init);
#pragma
#pragma arm section rodata, rwdata, zidata

#endif // (BIND_SENSOR_PS1210)
#endif // (SENSOR_EN)

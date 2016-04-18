//==============================================================================
//
//  File        : sensor_imx322.c
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
#if (BIND_SENSOR_IMX322)

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
	13, 																				                	// ubSensorModeNum
	2, 																				                		// ubDefPreviewMode
	0, 																					                	// ubDefCaptureMode
	3000,                                                                                                   // usPixelSize (TBD)
// Mode0 	Mode1 	Mode2 	Mode3 	Mode4 	Mode5 	Mode6 	Mode7 	Mode8 	Mode9 	Mode10  Mode11  Mode12
	{1+372, 1+452,  1+140,  1+140,  1+140,  1+140,  1+452,  1+189,  1+189,  1+189,  1+772,  1+140,	1+452}, // usVifGrabStX
	{1+22,   1+66,   1+22,   1+22,   1+22,   1+22,  1+186,   1+19,   1+19,   1+19,  1+306,   1+22,  1+186}, // usVifGrabStY
	{1448,   1288,   1928,   1928,   1928,   1928,   1288,   1288,   1288,   1288,    648,   1928,   1288}, // usVifGrabW
	{1096,    976,   1096,   1096,   1096,   1096,    724,    724,    724,    724,    496,   1096,    724}, // usVifGrabH
#if (CHIP == MCR_V2)
	{1,	        1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1}, // usBayerInGrabX
	{1,	        1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1}, // usBayerInGrabY
	{8,         8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8}, // usBayerInDummyX
	{16,       16,     16,     16,     16,     16,      4,      4,      4,      4,     16,     16,      4}, // usBayerInDummyY
	{1440,   1280,   1920,   1920,   1920,   1920,   1280,   1280,   1280,   1280,    640,   1920,   1280}, // usBayerOutW
	{1080,    960,   1080,   1080,   1080,   1080,    720,    720,    720,    720,    480,   1080,    720}, // usBayerOutH
#endif
	{1440,   1280,   1920,   1920,   1920,   1920,   1280,   1280,   1280,   1280,    640,   1920,   1280}, // usScalInputW
	{1080,    960,   1080,   1080,   1080,   1080,    720,    720,    720,    720,    480,   1080,    720}, // usScalInputH
	{300,     300,    300,    500,    600,    150,    300,    600,   1000,   1200,    300,    240,    240}, // usTargetFpsx10
	{1141,   1141,   1141,   1141,   1141,   1141,   1141,   1141,   1141,   1141,   1141,   1141,   1141}, // usVsyncLine
	{1,         1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1}, // ubWBinningN
	{1,	        1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1}, // ubWBinningM
	{1,	        1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1}, // ubHBinningN
	{1,	        1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1}, // ubHBinningM
	{0xFF,	 0xFF,	 0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF}, // ubCustIQmode
	{0xFF,   0xFF,	 0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF}  // ubCustAEmode
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
	(0x34 >> 1), 	// ubSlaveAddr
	16, 			// ubRegLen
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
ISP_UINT16 imx322_GainTable[] = 
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
//ISP_UINT16 imx322_GainTable[] = {
const ISP_UINT16 imx322_GainTable[141] = {
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

ISP_UINT16 SNR_IMX322_Reg_Unsupport[] = 
{
    SENSOR_DELAY_REG, 100 // delay
};

ISP_UINT16 SNR_IMX322_Reg_Init_Customer[] = 
{
	// TBD
	SENSOR_DELAY_REG, 100 // delay
};

#if 0
void ____Sensor_Res_OPR_Table____(){ruturn;} //dummy
#endif

ISP_UINT16 SNR_IMX322_Reg_1440x1080_30P[] = 
{
	// TBD
	SENSOR_DELAY_REG, 100 // delay
};

ISP_UINT16 SNR_IMX322_Reg_1280x960_30P[] = 
{
	// TBD
	SENSOR_DELAY_REG, 100 // delay
};

ISP_UINT16 SNR_IMX322_Reg_1920x1080_24P[] =
{
//        1080@24fps-12bit,Parallel CMOS SDR
//        Input 37.125MHz to INCK
//
//      address data
        0x3000,  0x31,
        0x0100,  0x00,

        0x302C,  0x01,
        0x0008,  0x00,
        0x0009,  0xF0, //Change it from 0x3C to 0xF0 because of green image
#if (SENSOR_ROTATE_180)
    	0x0101,	 0x03, 	// (IMG_ORIENTATION, address 0101h[1]), Vertical (V) scanning direction control (I2C)
					    // 0: Normal, 1: Inverted
					    // (IMG_ORIENTATION, address 0101h[2]), Horizontal (H) scanning direction control (I2C)
					    // 0: Normal, 1: Inverted
#else
        0x0101,  0x00,
#endif
        0x0104,  0x00,
        0x0112,  0x0C,
        0x0113,  0x0C,
        0x0202,  0x00,
        0x0203,  0x00,
        0x0340,  0x04,
        0x0341,  0x65,
        0x0342,  0x05,
        0x0343,  0x4C,

        0x3001,  0x00,
        0x3002,  0x0F,
        0x3003,  0x5F,
        0x3004,  0x05,
        0x3005,  0x65,
        0x3006,  0x04,
        0x3007,  0x00,
        0x3011,  0x00,
        0x3012,  0x82,
        0x3016,  0x3C,
        0x301F,  0x73,
        0x3020,  0x3C,
        0x3021,  0x00,
        0x3022,  0x00,
        0x3027,  0x20,
        0x307A,  0x00,
        0x307B,  0x00,
        0x3098,  0x26,
        0x3099,  0x02,
        0x309A,  0x26,
        0x309B,  0x02,
        0x30CE,  0x16,
        0x30CF,  0x82,
        0x30D0,  0x00,

//      address data
        0x3117,  0x0D,

        // wait 100ms
        SENSOR_DELAY_REG, 100,

//      address data
        0x302C,  0x00,

//      wait 100ms
        SENSOR_DELAY_REG, 100,

//      address data
        0x3000,  0x30,

        0x0100,  0x01,
};

ISP_UINT16 SNR_IMX322_Reg_1920x1080_30P[] = 
{
	0x0100,	0x00,
	0x0008,	0x00,
	0x0009,	0xF0,
#if (SENSOR_ROTATE_180)
	0x0101,	0x03, 	// (IMG_ORIENTATION, address 0101h[1]), Vertical (V) scanning direction control (I2C)
					// 0: Normal, 1: Inverted
					// (IMG_ORIENTATION, address 0101h[2]), Horizontal (H) scanning direction control (I2C)
					// 0: Normal, 1: Inverted
#else
	0x0101,	0x00,
#endif
	0x0104,	0x00,

#if (USE_10_BIT)
	0x0112,	0x0A,
	0x0113,	0x0A,
#else
	0x0112,	0x0C,
	0x0113,	0x0C,
#endif

	0x0202,	0x00,
	0x0203,	0x00,

#if (USE_10_BIT)
	0x0340,	0x04,
	0x0341,	0x65,
#else
	0x0340,	0x04,
	0x0341,	0x71,//0x65, // FRM_LENGTH
#endif

	0x0342,	0x04,
	0x0343,	0x3D, // LINE_LENGTH

	0x0344,	0x00,
	0x0345,	0x00,
	0x0346,	0x00,
	0x0347,	0x00,

	0x034C,	0x07,
	0x034D,	0xC0, // 1984
	0x034E,	0x04,
	0x034F,	0xC9, // 1225
	0x3000,	0x31,
	0x3001,	0x00,
	0x3002,	0x0F,
	0x300D,	0x00,
	0x300E,	0x00,
	0x300F,	0x00,
	0x3010,	0x00,
	0x3011,	0x00,

#if (USE_10_BIT)
	0x3012,	0x80,
#else
	0x3012,	0x82,
#endif

	0x3016,	0x3C,
	0x3017,	0x00,
	0x301E,	0x00,
	0x301F,	0x73,
	0x3020,	0x3C,

#if (USE_10_BIT)
	0x3021,	0xA0, // 22clk //0x00, // XHSLNG
	0x3022,	0x00,
#else
	0x3021,	0x20, // 22clk //0x00, // XHSLNG
	0x3022,	0x00,
#endif

	0x3027,	0x20,
	0x302C,	0x01,
	0x303B,	0xE1,

#if (USE_10_BIT)
	0x307A,	0x40, // Rogers: 10bit //0x00,
	0x307B,	0x02, // Rogers: 10bit //0x00,
#else
	0x307A,	0x00,
	0x307B,	0x00,
#endif

	0x3098,	0x26,
	0x3099,	0x02,

#if (USE_10_BIT)
	0x309A,	0x4C, // Rogers: 10bit //0x26,
	0x309B,	0x04, // Rogers: 10bit //0x02,
#else
	0x309A,	0x26,
	0x309B,	0x02,
#endif

	0x30CE,	0x16,
	0x30CF,	0x82,
	0x30D0,	0x00,
	0x3117,	0x0D,

	0x302C,	0x00,
	0x0100,	0x01
};

ISP_UINT16 SNR_IMX322_Reg_1280x720_30P[] = 
{
/*
 * 720@30fps-10bit,Parallel CMOS SDR
 * Input 37.125MHz to INCK
 */

	// address, data,
	0x3000, 0x31,
	0x0100, 0x00,

	0x302C, 0x01,


	0x0008, 0x00,
	0x0009, 0x3C,
#if (SENSOR_ROTATE_180)
	0x0101,	0x03, 	// (IMG_ORIENTATION, address 0101h[1]), Vertical (V) scanning direction control (I2C)
					// 0: Normal, 1: Inverted
					// (IMG_ORIENTATION, address 0101h[2]), Horizontal (H) scanning direction control (I2C)
					// 0: Normal, 1: Inverted
#else
	0x0101, 0x00,
#endif
	0x0104, 0x00,
	0x0112, 0x0A,
	0x0113, 0x0A,
	0x0202, 0x00,
	0x0203, 0x00,
	0x0340, 0x06,
	0x0341, 0x72,
	0x0342, 0x02,
	0x0343, 0xEE,

	0x3001, 0x00,
	0x3002, 0x01,
	0x3003, 0x72,
	0x3004, 0x06,
	0x3005, 0xEE,
	0x3006, 0x02,
	0x3007, 0x00,
	0x3011, 0x01,
	0x3012, 0x80,
	0x3016, 0xF0,
	0x301F, 0x73,
	0x3020, 0x3C,
	0x3021, 0x80,
	0x3022, 0xC0,
	0x3027, 0x20,
	0x307A, 0x40,
	0x307B, 0x02,
	0x3098, 0x26,
	0x3099, 0x02,
	0x309A, 0x4C,
	0x309B, 0x04,
	0x30CE, 0x00,
	0x30CF, 0x00,
	0x30D0, 0x00,

	// address, data,
	0x3117, 0x0D,

	SENSOR_DELAY_REG, 100, // wait 100ms
	// address, data,
	0x302C, 0x00,

	SENSOR_DELAY_REG, 100, // wait 100ms
	// address, data,
	0x3000, 0x30,

	0x0100, 0x01
};

ISP_UINT16 SNR_IMX322_Reg_1280x720_24P[] =
{
//720@24fps-12bit,Parallel CMOS SDR
//Input 37.125MHz to INCK

        0x3000,  0x31,
        0x0100,  0x00,

        0x302C,  0x01,

        0x0008,  0x00,
        0x0009,  0x3C,
#if (SENSOR_ROTATE_180)
        0x0101,  0x03,   // (IMG_ORIENTATION, address 0101h[1]), Vertical (V) scanning direction control (I2C)
                         // 0: Normal, 1: Inverted
                         // (IMG_ORIENTATION, address 0101h[2]), Horizontal (H) scanning direction control (I2C)
                         // 0: Normal, 1: Inverted
#else
        0x0101,  0x00,
#endif
        0x0104,  0x00,
        0x0112,  0x0A,//0x0A,//0x0C,
        0x0113,  0x0A,//0x0A,//0x0C,
        0x0202,  0x00,
        0x0203,  0x00,
        0x0340,  0x08,//0x02,
        0x0341,  0x0E,//0xEE,
        0x0342,  0x02,//0x08,
        0x0343,  0xEE,//0x0E,

        0x3001,  0x00,
        0x3002,  0x01,
        0x3003,  0x0E,
        0x3004,  0x08,
        0x3005,  0xEE,
        0x3006,  0x02,
        0x3007,  0x00,
        0x3011,  0x01,
        0x3012,  0x80,//0x80,//0x82
        0x3016,  0xF0,
        0x301F,  0x73,
        0x3020,  0x3C,//0x3C,//0xF0
        0x3021,  0x80,//0x80,//0x00
        0x3022,  0xC0,
        0x3027,  0x20,
        0x307A,  0x40,
        0x307B,  0x02,
        0x3098,  0x26,
        0x3099,  0x02,
        0x309A,  0x4C,
        0x309B,  0x04,
        0x30CE,  0x00,//0x00,//0x40
        0x30CF,  0x00,//0x00,//0x81
        0x30D0,  0x00,//0x00,//0x01

        0x3117,  0x0D,
        SENSOR_DELAY_REG, 100,
        0x302C,  0x00,
        SENSOR_DELAY_REG, 100,
        0x3000,  0x30,
        0x0100,  0x01,
};

ISP_UINT16 SNR_IMX322_Reg_1280x720_60P[] = 
{
/*
 * 720@60fps-10bit,Parallel CMOS SDR
 * Input 37.125MHz to INCK
 */

	// address, data,
	0x3000, 0x31,
	0x0100, 0x00,

	0x302C, 0x01,


	0x0008, 0x00,
	0x0009, 0x3C,
#if (SENSOR_ROTATE_180)
	0x0101,	0x03, 	// (IMG_ORIENTATION, address 0101h[1]), Vertical (V) scanning direction control (I2C)
					// 0: Normal, 1: Inverted
					// (IMG_ORIENTATION, address 0101h[2]), Horizontal (H) scanning direction control (I2C)
					// 0: Normal, 1: Inverted
#else
	0x0101, 0x00,
#endif
	0x0104, 0x00,
	0x0112, 0x0A,
	0x0113, 0x0A,
	0x0202, 0x00,
	0x0203, 0x00,
	0x0340, 0x03,
	0x0341, 0x39,
	0x0342, 0x02,
	0x0343, 0xEE,

	0x3001, 0x00,
	0x3002, 0x01,
	0x3003, 0x39,
	0x3004, 0x03,
	0x3005, 0xEE,
	0x3006, 0x02,
	0x3007, 0x00,
	0x3011, 0x00,
	0x3012, 0x80,
	0x3016, 0xF0,
	0x301F, 0x73,
	0x3020, 0x3C,
	0x3021, 0x80,
	0x3022, 0xC0,
	0x3027, 0x20,
	0x307A, 0x00,
	0x307B, 0x00,
	0x3098, 0x26,
	0x3099, 0x02,
	0x309A, 0x4C,
	0x309B, 0x04,
	0x30CE, 0x00,
	0x30CF, 0x00,
	0x30D0, 0x00,

	// address, data,
	0x3117, 0x0D,

	SENSOR_DELAY_REG, 100, // wait 100ms
	// address, data,
	0x302C, 0x00,

	SENSOR_DELAY_REG, 100, // wait 100ms
	// address, data,
	0x3000, 0x30,

	0x0100, 0x01
};

ISP_UINT16 SNR_IMX322_Reg_640x480_30P[] = 
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
	MMP_ULONG 	ulSensorMCLK = 37125; // 37.125 M

    RTNA_DBG_Str(0, "SNR_Cust_InitConfig IMX322\r\n");

	// Init OPR Table
	SensorCustFunc.OprTable->usInitSize 							= (sizeof(SNR_IMX322_Reg_Init_Customer)/sizeof(SNR_IMX322_Reg_Init_Customer[0]))/2;
	SensorCustFunc.OprTable->uspInitTable 							= &SNR_IMX322_Reg_Init_Customer[0];

    SensorCustFunc.OprTable->bBinTableExist                         = MMP_FALSE;
    SensorCustFunc.OprTable->bInitDoneTableExist                    = MMP_FALSE;

	for (i = 0; i < MAX_SENSOR_RES_MODE; i++)
	{
		SensorCustFunc.OprTable->usSize[i] 							= (sizeof(SNR_IMX322_Reg_Unsupport)/sizeof(SNR_IMX322_Reg_Unsupport[0]))/2;
		SensorCustFunc.OprTable->uspTable[i] 						= &SNR_IMX322_Reg_Unsupport[0];
	}

	// 16:9
	SensorCustFunc.OprTable->usSize[RES_IDX_1920x1080_30P]			= (sizeof(SNR_IMX322_Reg_1920x1080_30P)/sizeof(SNR_IMX322_Reg_1920x1080_30P[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_1920x1080_30P] 		= &SNR_IMX322_Reg_1920x1080_30P[0];
#if 1 // cropped from FHD30P, TBC??
	SensorCustFunc.OprTable->usSize[RES_IDX_1280x720_30P] 			= (sizeof(SNR_IMX322_Reg_1920x1080_30P)/sizeof(SNR_IMX322_Reg_1920x1080_30P[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720_30P] 		= &SNR_IMX322_Reg_1920x1080_30P[0];
#else
	SensorCustFunc.OprTable->usSize[RES_IDX_1280x720_30P] 			= (sizeof(SNR_IMX322_Reg_1280x720_30P)/sizeof(SNR_IMX322_Reg_1280x720_30P[0]))/2; // TBD
	SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720_30P] 		= &SNR_IMX322_Reg_1280x720_30P[0]; // TBD
#endif
	SensorCustFunc.OprTable->usSize[RES_IDX_1920x1080_24P]           = (sizeof(SNR_IMX322_Reg_1920x1080_24P)/sizeof(SNR_IMX322_Reg_1920x1080_24P[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_1920x1080_24P]         = &SNR_IMX322_Reg_1920x1080_24P[0];
#if 1
	SensorCustFunc.OprTable->usSize[RES_IDX_1280x720_24P]           = (sizeof(SNR_IMX322_Reg_1920x1080_24P)/sizeof(SNR_IMX322_Reg_1920x1080_24P[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720_24P]         = &SNR_IMX322_Reg_1920x1080_24P[0];
#else
	SensorCustFunc.OprTable->usSize[RES_IDX_1280x720_24P]           = (sizeof(SNR_IMX322_Reg_1280x720_24P)/sizeof(SNR_IMX322_Reg_1280x720_24P[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720_24P]         = &SNR_IMX322_Reg_1280x720_24P[0];
#endif
	SensorCustFunc.OprTable->usSize[RES_IDX_1280x720_60P] 			= (sizeof(SNR_IMX322_Reg_1280x720_60P)/sizeof(SNR_IMX322_Reg_1280x720_60P[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720_60P] 		= &SNR_IMX322_Reg_1280x720_60P[0];

	// 4:3
#if 1 // cropped from FHD30P, TBC??
	SensorCustFunc.OprTable->usSize[RES_IDX_1440x1080_30P] 			= (sizeof(SNR_IMX322_Reg_1920x1080_30P)/sizeof(SNR_IMX322_Reg_1920x1080_30P[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_1440x1080_30P] 		= &SNR_IMX322_Reg_1920x1080_30P[0];
	SensorCustFunc.OprTable->usSize[RES_IDX_1280x960_30P] 			= (sizeof(SNR_IMX322_Reg_1920x1080_30P)/sizeof(SNR_IMX322_Reg_1920x1080_30P[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_1280x960_30P] 		= &SNR_IMX322_Reg_1920x1080_30P[0];
	SensorCustFunc.OprTable->usSize[RES_IDX_640x480_30P] 			= (sizeof(SNR_IMX322_Reg_1920x1080_30P)/sizeof(SNR_IMX322_Reg_1920x1080_30P[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_640x480_30P] 			= &SNR_IMX322_Reg_1920x1080_30P[0];
#else
	SensorCustFunc.OprTable->usSize[RES_IDX_1440x1080_30P] 			= (sizeof(SNR_IMX322_Reg_1440x1080_30P)/sizeof(SNR_IMX322_Reg_1440x1080_30P[0]))/2; // TBD
	SensorCustFunc.OprTable->uspTable[RES_IDX_1440x1080_30P] 		= &SNR_IMX322_Reg_1440x1080_30P[0];
	SensorCustFunc.OprTable->usSize[RES_IDX_1280x960_30P] 			= (sizeof(SNR_IMX322_Reg_1280x960_30P)/sizeof(SNR_IMX322_Reg_1280x960_30P[0]))/2; // TBD
	SensorCustFunc.OprTable->uspTable[RES_IDX_1280x960_30P] 		= &SNR_IMX322_Reg_1280x960_30P[0]; // TBD
	SensorCustFunc.OprTable->usSize[RES_IDX_640x480_30P] 			= (sizeof(SNR_IMX322_Reg_640x480_30P)/sizeof(SNR_IMX322_Reg_640x480_30P[0]))/2; // TBD
	SensorCustFunc.OprTable->uspTable[RES_IDX_640x480_30P] 			= &SNR_IMX322_Reg_640x480_30P[0]; // TBD
#endif

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
	SensorCustFunc.VifSetting->powerOnSetting.bExtPowerPinHigh 		= SENSOR_GPIO_ENABLE_ACT_LEVEL;
	SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPinDelay 	= 50;
	SensorCustFunc.VifSetting->powerOnSetting.bFirstEnPinHigh 		= MMP_FALSE;
	SensorCustFunc.VifSetting->powerOnSetting.ubFirstEnPinDelay 	= 10;
	SensorCustFunc.VifSetting->powerOnSetting.bNextEnPinHigh 		= MMP_TRUE;
	SensorCustFunc.VifSetting->powerOnSetting.ubNextEnPinDelay 		= 100;
	SensorCustFunc.VifSetting->powerOnSetting.bTurnOnClockBeforeRst = MMP_TRUE;
	SensorCustFunc.VifSetting->powerOnSetting.bFirstRstPinHigh 		= MMP_FALSE;
	SensorCustFunc.VifSetting->powerOnSetting.ubFirstRstPinDelay 	= 100;
	SensorCustFunc.VifSetting->powerOnSetting.bNextRstPinHigh 		= MMP_TRUE;
	SensorCustFunc.VifSetting->powerOnSetting.ubNextRstPinDelay 	= 100;

	// Init Vif Setting : PowerOff Setting
	SensorCustFunc.VifSetting->powerOffSetting.bEnterStandByMode 	= MMP_FALSE;
	SensorCustFunc.VifSetting->powerOffSetting.usStandByModeReg 	= 0x0100;
	SensorCustFunc.VifSetting->powerOffSetting.usStandByModeMask 	= 0x01;
	SensorCustFunc.VifSetting->powerOffSetting.bEnPinHigh 			= MMP_TRUE;
	SensorCustFunc.VifSetting->powerOffSetting.ubEnPinDelay 		= 20;
	SensorCustFunc.VifSetting->powerOffSetting.bTurnOffMClock 		= MMP_TRUE;
	SensorCustFunc.VifSetting->powerOffSetting.bTurnOffExtPower 	= MMP_TRUE;
	SensorCustFunc.VifSetting->powerOffSetting.usExtPowerPin 		= SENSOR_GPIO_ENABLE; // it might be defined in Config_SDK.h

	// Init Vif Setting : Sensor MClock Setting
	MMPF_PLL_GetGroupFreq(CLK_GRP_SNR, &ulSensorClkSrc);

	SensorCustFunc.VifSetting->clockAttr.bClkOutEn 					= MMP_TRUE;
	SensorCustFunc.VifSetting->clockAttr.ubClkFreqDiv 				= ulSensorClkSrc / ulSensorMCLK; // (528MHz / 37.125MHz)
	SensorCustFunc.VifSetting->clockAttr.ulMClkFreq 				= 24000;
	SensorCustFunc.VifSetting->clockAttr.ulDesiredFreq 				= 24000;
	SensorCustFunc.VifSetting->clockAttr.ubClkPhase 				= MMPF_VIF_SNR_PHASE_DELAY_NONE;
	SensorCustFunc.VifSetting->clockAttr.ubClkPolarity 				= MMPF_VIF_SNR_CLK_POLARITY_NEG;
	SensorCustFunc.VifSetting->clockAttr.ubClkSrc 					= MMPF_VIF_SNR_CLK_SRC_VIFCLK;

	// Init Vif Setting : Parallel Sensor Setting
	SensorCustFunc.VifSetting->paralAttr.ubLatchTiming 				= MMPF_VIF_SNR_LATCH_POS_EDGE;
	SensorCustFunc.VifSetting->paralAttr.ubHsyncPolarity 			= MMPF_VIF_SNR_CLK_POLARITY_POS;
	SensorCustFunc.VifSetting->paralAttr.ubVsyncPolarity 			= MMPF_VIF_SNR_CLK_POLARITY_POS;
    SensorCustFunc.VifSetting->paralAttr.ubBusBitMode               = MMPF_VIF_SNR_PARAL_BITMODE_16;
    
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
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[0] 				= 0;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[1] 				= 0;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[2] 				= 0;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[3] 				= 0;
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
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[0] 				= 0;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[1] 				= 0;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[2] 				= 0;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[3] 				= 0;
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
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[0] 				= 0;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[1] 				= 0;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[2] 				= 0;
	SensorCustFunc.VifSetting->mipiAttr.usDataDelay[3] 				= 0;
	SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[0] 			= 0x1F;
	SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[1] 			= 0x1F;
	SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[2] 			= 0x1F;
	SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[3] 			= 0x1F;
#endif

	// Init Vif Setting : Color ID Setting
	SensorCustFunc.VifSetting->colorId.VifColorId 					= MMPF_VIF_COLORID_00;
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
	ISP_UINT8 i, sensor_gain;

	for( i = 0; i< 141; i++)
	{
		if (ulGain >= imx322_GainTable[i])
			sensor_gain = i;
		else
			break;
	}

	dgain = dgain*ulGain/imx322_GainTable[sensor_gain];
	
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x301E, sensor_gain);
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetShutter
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetShutter(MMP_UBYTE ubSnrSel, MMP_ULONG shutter, MMP_ULONG vsync)
{
	ISP_UINT32 new_vsync 	= vsync;
	ISP_UINT32 new_shutter 	= shutter;

	new_vsync 	= ISP_MIN(ISP_MAX((new_shutter + 3), new_vsync), 0xFFFF);
	new_shutter = ISP_MIN(ISP_MAX(new_shutter, 1), (new_vsync - 3));

	// shutter
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x0340, (new_vsync >> 8));
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x0341, new_vsync);

	// vsync
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x0202,  (((new_vsync - new_shutter) >> 8) & 0xFF));
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x0203, (( new_vsync - new_shutter) & 0xFF));
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

#endif // (BIND_SENSOR_IMX322)
#endif // (SENSOR_EN)

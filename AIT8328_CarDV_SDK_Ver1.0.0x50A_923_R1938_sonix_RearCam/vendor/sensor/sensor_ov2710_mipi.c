//==============================================================================
//
//  File        : sensor_ov2710.c
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
#if (BIND_SENSOR_OV2710_MIPI)

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
	6, 												// ubSensorModeNum
	0, 												// ubDefPreviewMode
	4, 												// ubDefCaptureMode
	3000,                                           // usPixelSize
// Mode0 	Mode1	Mode2 	Mode3 	Mode4 	Mode5
	{1,  	1,    	321,  	1,    	241,   	641}, 	// usVifGrabStX
	{1,  	1,    	181,	1,      1,     	301}, 	// usVifGrabStY
	{1928, 	1928,   1288,   1288,   1448,  	648}, 	// usVifGrabW
	{1088, 	1088, 	724,    724,   	1088,  	488}, 	// usVifGrabH
#if (CHIP == MCR_V2)
	{1,   	1,      1,      1,      1,      1}, 	// usBayerInGrabX
	{1,   	1,      1,      1,      1,      1}, 	// usBayerInGrabY
	{8,   	8,      8,      8,      8,      8}, 	// usBayerInDummyX
	{8,   	8,      4,      4,      8,      8}, 	// usBayerInDummyY
	{1920, 	1920,   1280,   1280,   1440,	640}, 	// usBayerOutW
	{1080, 	1080, 	720,    720,   	1080, 	480}, 	// usBayerOutH
#endif
	{1920, 	1920,   1280,   1280,   1440, 	640}, 	// usScalInputW
	{1080, 	1080, 	720,    720,   	1080, 	480}, 	// usScalInputH
	{300,   250,    300,    600,    300,    300}, 	// usTargetFpsx10
	{1104, 	1104,   1104,	744,   	1104,   1104}, 	// usVsyncLine
	{1,   	1,      1,      1,      1,      1}, 	// ubWBinningN
	{1,   	1,      1,      1,      1,      1}, 	// ubWBinningM
	{1,   	1,      1,      1,      1,      1}, 	// ubHBinningN
	{1,   	1,      1,      1,      1,      1}, 	// ubHBinningM
    {1,		0,		0,      0,      0,      0},    	// ubCustIQmode
    {0,		0,		0,      1,      0,      0}     	// ubCustAEmode
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
#include "isp_8428_iq_data_v2_OV2710.xls.ciq.txt"
#endif
};

// I2cm Attribute
static MMP_I2CM_ATTR m_I2cmAttr = 
{
	MMP_I2CM0,      // i2cmID
    0x36,       	// ubSlaveAddr
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
	250000, 		// ulI2cmSpeed 250KHZ
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
	1, 	// ubFrmStSetShutFrmCnt
	2 	// ubFrmStSetGainFrmCnt
};

MMPF_SENSOR_AFTIMIMG m_AfTime = 
{
	1, 	// ubPeriod
	0 	// ubDoAFFrmCnt
};

#if (ISP_EN)
static ISP_UINT32 s_gain;
#endif

// Only OV2710 need to refrence this gain table. Other sensor may can set gain by calculate.
const ISP_UINT8 OV2710_GainTable[128] = 
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

#if 0//	new extent node for18//LV1,		LV2,		LV3,		LV4,		LV5,		LV6,		LV7,		LV8,		LV9,		LV10,	LV11,	LV12,	LV13,	LV14,	LV15,	LV16 	LV17  	LV18
//abby curve
ISP_UINT32 AE_Bias_tbl[54] =
/*lux*/						{1,			2,			3,			6,			12, 		24, 		50, 		101, 		206,		407,	826,	1638,	3277,	6675,	13554,	27329,	54961, 111285/*930000=LV17*/
/*ENG*/						,0x2FFFFFF, 4841472*2,	3058720,	1962240,	1095560,  	616000, 	334880, 	181720,     96600,	 	52685,	27499,	14560,	8060,	4176,	2216,	1144,	600,   300
/*Tar*/						,86,		86,		 	86,	        110,		128,	 	137,	 	161,	 	170,	    190,	    202,	220,	237,	256,	256,	256,	256,	256,   256
 };	
#define AE_tbl_size  (18)
#endif

#if 0//	new extent node for18//LV1,		LV2,		LV3,		LV4,		LV5,		LV6,		LV7,		LV8,		LV9,		LV10,	LV11,	LV12,	LV13,	LV14,	LV15,	LV16 	LV17  	LV18
//abby curve
ISP_UINT32 AE_Bias_tbl[54] =
/*lux*/						{1,			2,			3,			6,			12, 		24, 		50, 		101, 		206,		407,	826,	1638,	3277,	6675,	13554,	27329,	54961, 111285/*930000=LV17*/
/*ENG*/						,0x2FFFFFF, 4841472*2,	3058720,	1962240,	1095560,  	616000, 	334880, 	181720,     96600,	 	52685,	27499,	14560,	8060,	4176,	2216,	1144,	600,   300
/*Tar*/						,86,		86,		 	86,	        110,		128,	 	137,	 	161,	 	170,	    190,	    202,	220,	237,	256,	256,	256,	256,	256,   256
 };	
#define AE_tbl_size  (18)
#endif

#if 0//	new extent node for18//LV1,		LV2,		LV3,		LV4,		LV5,		LV6,		LV7,		LV8,		LV9,		LV10,	LV11,	LV12,	LV13,	LV14,	LV15,	LV16 	LV17  	LV18
//abby curve 白天降15%
ISP_UINT32 AE_Bias_tbl[54] =
/*lux*/						{1,			2,			3,			6,			12, 		24, 		50, 		101, 		206,		407,	826,	1638,	3277,	6675,	13554,	27329,	54961, 111285/*930000=LV17*/
/*ENG*/						,0x2FFFFFF, 4841472*2,	3058720,	1962240,	1095560,  	616000, 	334880, 	181720,     96600,	 	52685,	27499,	14560,	8060,	4176,	2216,	1144,	600,   300
/*Tar*/						,86,		86,		 	86,	        110,		128,	 	137,	 	161,	 	170,	    190,	    202,	220,	220,	220,	220,	220,	220,	220,   220
 };	
#define AE_tbl_size  (18)	
#endif

#if 0//	new extent node for18//LV1,		LV2,		LV3,		LV4,		LV5,		LV6,		LV7,		LV8,		LV9,		LV10,	LV11,	LV12,	LV13,	LV14,	LV15,	LV16 	LV17  	LV18
//abby curve 白天降15%
ISP_UINT32 AE_Bias_tbl[54] =
/*lux*/						{1,			2,			3,			6,			12, 		24, 		50, 		101, 		206,		407,	826,	1638,	3277,	6675,	13554,	27329,	54961, 111285/*930000=LV17*/
/*ENG*/						,0x2FFFFFF, 4841472*2,	3058720,	1962240,	1095560,  	616000, 	334880, 	181720,     96600,	 	52685,	27499,	14560,	8060,	4176,	2216,	1144,	600,   300
/*Tar*/						,86,		86,		 	86,	        110,		128,	 	137,	 	138,	 	140,	    145,	    150,	155,	160,	165,	170,	190,	220,	220,   220
 };	
#define AE_tbl_size  (18)
#endif

#if 0//	new extent node for18//LV1,		LV2,		LV3,		LV4,		LV5,		LV6,		LV7,		LV8,		LV9,		LV10,	LV11,	LV12,	LV13,	LV14,	LV15,	LV16 	LV17  	LV18
//abby curve 白天降15%, 在mio調
ISP_UINT32 AE_Bias_tbl[54] =
/*lux*/						{1,			2,			3,			6,			12, 		24, 		50, 		101, 		206,		407,	826,	1638,	3277,	6675,	13554,	27329,	54961, 111285/*930000=LV17*/
/*ENG*/						,0x2FFFFFF, 4841472*2,	3058720,	1962240,	1095560,  	616000, 	334880, 	181720,     96600,	 	52685,	27499,	14560,	8060,	4176,	2216,	1144,	600,   300
/*Tar*/						,90,		90,		 	90,	        100,		110,	 	120,	 	130,	 	140,	    145,	    152,	160,	170,	182,	195,	210,	230,	250,   250
 };	
#define AE_tbl_size  (18)
#endif

#if 0//	new extent node for18//LV1,		LV2,		LV3,		LV4,		LV5,		LV6,		LV7,		LV8,		LV9,		LV10,	LV11,	LV12,	LV13,	LV14,	LV15,	LV16 	LV17  	LV18
//abby curve 白天降15% casio modify
ISP_UINT32 AE_Bias_tbl[54] =
/*lux*/						{1,			2,			3,			6,			12, 		24, 		50, 		101, 		206,		407,	826,	1638,	3277,	6675,	13554,	27329,	54961, 111285/*930000=LV17*/
/*ENG*/						,0x2FFFFFF, 4841472*2,	3058720,	1962240,	1095560,  	616000, 	334880, 	181720,     96600,	 	52685,	27499,	14560,	8060,	4176,	2216,	1144,	600,   300
/*Tar*/						,75,		80,		 	85,	        90,			95,	 		100,	 	110,	 	120,	    140,	    152,	160,	170,	182,	195,	210,	230,	250,   250
 };	
#define AE_tbl_size  (18)
#endif

#if 0//	new extent node for18//LV1,		LV2,		LV3,		LV4,		LV5,		LV6,		LV7,		LV8,		LV9,		LV10,	LV11,	LV12,	LV13,	LV14,	LV15,	LV16 	LV17  	LV18
//abby curve 白天降15%
ISP_UINT32 AE_Bias_tbl[54] =
/*lux*/						{1,			2,			3,			6,			12, 		24, 		50, 		101, 		206,		407,	826,	1638,	3277,	6675,	13554,	27329,	54961, 111285/*930000=LV17*/
/*ENG*/						,0x2FFFFFF, 4841472*2,	3058720,	1962240,	1095560,  	616000, 	334880, 	181720,     96600,	 	52685,	27499,	14560,	8060,	4176,	2216,	1144,	600,   300
/*Tar*/						,70,		70,		 	70,	        75,			93,	 		97,	 		104,	 	119,	    135,	    152,	160,	170,	182,	195,	210,	230,	250,   250
 };	
#define AE_tbl_size  (18)
#endif

#if 0//	new extent node for18//LV1,		LV2,		LV3,		LV4,		LV5,		LV6,		LV7,		LV8,		LV9,		LV10,	LV11,	LV12,	LV13,	LV14,	LV15,	LV16 	LV17  	LV18
//abby curve 白天降15% MIO WITH CS
ISP_UINT32 AE_Bias_tbl[54] =
/*lux*/						{1,			2,			3,			6,			12, 		24, 		50, 		101, 		206,		407,	826,	1638,	3277,	6675,	13554,	27329,	54961, 111285/*930000=LV17*/
/*ENG*/						,0x2FFFFFF, 4841472*2,	3058720,	1962240,	1095560,  	616000, 	334880, 	181720,     96600,	 	52685,	27499,	14560,	8060,	4176,	2216,	1144,	600,   300
/*Tar*/						,70,		70,		 	70,	        75,			93,	 		97,	 		104,	 	119,	    135,	    152,	160,	170,	182,	195,	210,	230,	250,   250
 };	
#define AE_tbl_size  (18)
#endif

#if 0//	new extent node for18//LV1,		LV2,		LV3,		LV4,		LV5,		LV6,		LV7,		LV8,		LV9,		LV10,	LV11,	LV12,	LV13,	LV14,	LV15,	LV16 	LV17  	LV18
//abby curve iq 12
ISP_UINT32 AE_Bias_tbl[54] =
/*lux*/						{1,			2,			3,			6,			12, 		24, 		50, 		101, 		206,		407,	826,	1638,	3277,	6675,	13554,	27329,	54961, 111285/*930000=LV17*/
/*ENG*/						,0x2FFFFFF, 4841472*2,	3058720,	1962240,	1095560,  	616000, 	334880, 	181720,     96600,	 	52685,	27499,	14560,	8060,	4176,	2216,	1144,	600,   300
/*Tar*/						,70,		70,		 	70,	        75,			87,	 		97,	 		104,	 	119,	    135,	    152,	160,	170,	182,	195,	210,	230,	250,   250
 };	
#define AE_tbl_size  (18)
#endif

//lv1-7, 2	3	6	11	17	30	60
#if 0//	new extent node for18//LV1,		LV2,		LV3,		LV4,		LV5,		LV6,		LV7,		LV8,		LV9,		LV10,	LV11,	LV12,	LV13,	LV14,	LV15,	LV16 	LV17  	LV18
//abby curve iq 12
ISP_UINT32 AE_Bias_tbl[54] =
/*lux*/						{2,			3,			6,			10,			17, 		26, 		54, 		101, 		206,		407,	826,	1638,	3277,	6675,	13554,	27329,	54961, 111285/*930000=LV17*/
/*ENG*/						,0x2FFFFFF, 4841472*2,	3058720,	1962240,	1095560,  	616000, 	334880, 	181720,     96600,	 	52685,	27499,	14560,	8060,	4176,	2216,	1144,	600,   300
/*Tar*/						,55,		55,		 	68,	        73,			85,	 		93,	 		111,	 	119,	    135,	    152,	160,	170,	182,	195,	210,	230,	250,   250
 };	
#define AE_tbl_size  (18)
#endif

//for soso machine
//lv1-7, 2	3	6	11	17	30	60
#if 0//	new extent node for18//LV1,		LV2,		LV3,		LV4,		LV5,		LV6,		LV7,		LV8,		LV9,		LV10,	LV11,	LV12,	LV13,	LV14,	LV15,	LV16 	LV17  	LV18
//abby curve iq 12
ISP_UINT32 AE_Bias_tbl[54] =
/*lux*/						{2,			3,			6,			10,			17, 		26, 		54, 		101, 		206,		407,	826,	1638,	3277,	6675,	13554,	27329,	54961, 111285/*930000=LV17*/
/*ENG*/						,0x2FFFFFF, 4841472*2,	3058720,	1962240,	1095560,  	616000, 	334880, 	181720,     96600,	 	52685,	27499,	14560,	8060,	4176,	2216,	1144,	600,   300
/*Tar*/						,55,		55,		 	65,	        76,			83,	 		102,	 		111,	 	119,	    135,	    152,	160,	170,	182,	195,	210,	230,	250,   250
 };	
#define AE_tbl_size  (18)
#endif

//for soso machine
//lv1-7, 2	3	6	11	17	30	60
#if 0//	new extent node for18//LV1,		LV2,		LV3,		LV4,		LV5,		LV6,		LV7,		LV8,		LV9,		LV10,	LV11,	LV12,	LV13,	LV14,	LV15,	LV16 	LV17  	LV18
//abby curve iq 12
ISP_UINT32 AE_Bias_tbl[54] =
/*lux*/						{2,			3,			6,			10,			17, 		26, 		54, 		101, 		206,		407,	826,	1638,	3277,	6675,	13554,	27329,	54961, 111285/*930000=LV17*/
/*ENG*/						,0x2FFFFFF, 4841472*2,	3058720,	1962240,	1095560,  	616000, 	334880, 	181720,     96600,	 	52685,	27499,	14560,	8060,	4176,	2216,	1144,	600,   300
/*Tar*/						,55,		55,		 	65,	        77,			82,	 		104,	 	111,	 	119,	    135,	    152,	160,	170,	182,	195,	210,	230,	250,   250
 };	
#define AE_tbl_size  (18)
#endif

//CASIO 20150423
//for soso machine
//lv1-7, 2	3	6	11	17	30	60
#if 1//	new extent node for18//LV1,		LV2,		LV3,		LV4,		LV5,		LV6,		LV7,		LV8,		LV9,		LV10,	LV11,	LV12,	LV13,	LV14,	LV15,	LV16 	LV17  	LV18
//abby curve iq 12
ISP_UINT32 AE_Bias_tbl[54] =
/*lux*/						{2,			3,			6,			10,			17, 		26, 		54, 		101, 		206,		407,	826,	1638,	3277,	6675,	13554,	27329,	54961, 111285/*930000=LV17*/
/*ENG*/						,0x2FFFFFF, 4841472*2,	3058720,	1962240,	1095560,  	616000, 	334880, 	181720,     96600,	 	52685,	27499,	14560,	8060,	4176,	2216,	1144,	600,   300
/*Tar*/						,55,		55,		 	65,	        82,			89,	 		104,	 	111,	 	119,	    135,	    152,	160,	170,	182,	195,	210,	230,	250,   250
 };	
#define AE_tbl_size  (18)	//32  35  44  50
#endif

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

#if 0
void ____Sensor_Init_OPR_Table____(){ruturn;} //dummy
#endif

ISP_UINT16 SNR_OV2710_Reg_Unsupport[] = 
{
	SENSOR_DELAY_REG, 100 // delay
};

ISP_UINT16 SNR_OV2710_Reg_Init_Customer[] = 
{
	0x5780, 0x00,
	// TBD
	SENSOR_DELAY_REG, 10 // delay
};

#if 0
void ____Sensor_Res_OPR_Table____(){ruturn;} //dummy
#endif

// 1928x1088 30fps
ISP_UINT16 SNR_OV2710_Reg_1920x1080_30P_Customer[] = 
{
#if 1
        0x3103, 0x93, // SCCB_PWUP_DIS

        0x3008, 0x82,

        0x3017, 0x7f, // PAD_OUTPUT ENABLE 1
        0x3018, 0xfc, // PAD_OUTPUT ENABLE 2
        0x3706, 0x61, // ANALOG CONTROL
        0x3712, 0x0c, // ANALOG CONTROL
        0x3630, 0x6d, //
        #if SENSOR_ROTATE_180
        0x3621, 0x14, // [7]: 1: H-sub, [4]: 0: if 0x3818[6]=1, 1: if 0x3818[6]=0
        #else
        0x3621, 0x04, // [7]: 1: H-sub, [4]: 0: if 0x3818[6]=1, 1: if 0x3818[6]=0
        #endif
        0x3604, 0x60, //
        0x3603, 0xa7, //
        0x3631, 0x26, //
        0x3600, 0x04, //
        0x3620, 0x37, //
        0x3623, 0x00,
        0x3702, 0x9e, // ANALOG CONTROL
        0x3703, 0x74, // // HREF offset
        0x3704, 0x10, // ANALOG CONTROL
        0x370d, 0x0f,
        0x3713, 0x8b, // ANALOG CONTROL
        0x3714, 0x74,
        0x3710, 0x9e, // ANALOG CONTROL
#if 0
        //0x3800, 0x01, // HREF Horizontal Start Point High Byte [3:0]
        0x3801, 0xc4,//0x7c, // HREF Horizontal Start Point Low Byte
        //0x3802, 0x00, // HREF Vertical Start Point Low Byte
        //0x3803, 0x0a, // HREF Vertical Start Point Low Byte       ==> 0x10
#else
        0x3800, 0x01,
        0x3801, 0xC4,
        0x3804, 0x07,
        0x3805, 0x88,
        0x3802, 0x00,
        0x3803, 0x0A,
        0x3806, 0x04,
        0x3807, 0x40,
        0x3808, 0x07,
        0x3809, 0x88,
        0x380a, 0x04,
        0x380b, 0x40,
        0x380c, 0x09,
        0x380d, 0x74, // 0xA4
        0x380e, 0x04,
        0x380f, 0x50,
        0x3810, 0x08,
#endif
        0x3811, 0x02,

#if (SENSOR_ROTATE_180)
        0x3621, 0x14, // [7]: 1: H-sub, [4]: 0: if 0x3818[6]=1, 1: if 0x3818[6]=0
#else
        0x3621, 0x04, // [7]: 1: H-sub, [4]: 0: if 0x3818[6]=1, 1: if 0x3818[6]=0
#endif
        0x3622, 0x08,
#if (SENSOR_ROTATE_180)
        0x3818, 0xE0, // TIMING CONTROL 18, [6]: mirror, [5]: vertical flip, [0]: vsub2, [1]: vsub4
#else
        0x3818, 0x80,
#endif

        0x3010, 0x20, // 0x20 //LeiQin
        0x302c, 0xC0,       // driving 4x //LeiQin

        0x3605, 0x05, //
        0x3606, 0x12, //
        0x302d, 0x90,
        0x370b, 0x40, // ANALOG CONTROL
        0x3716, 0x31,


        0x5181, 0x20,
        0x518f, 0x00,
        0x4301, 0xff,
        0x4303, 0x00,
        0x3a00, 0x00, //0x38, // AEC CONTROL

        0x3a1a, 0x06, // DIFF_MAX
        0x3a18, 0x00,
        0x3a19, 0x7a,
        0x3a13, 0x54,
        0x382e, 0x0f,
        0x381a, 0x1a,//0x00,//0x3c, // TIMING TC HS MIRR ADJ        ==> 0x3c
        0x5688, 0x03,
        0x5684, 0x07,
        0x5685, 0xa0,
        0x5686, 0x04,
        0x5687, 0x43,

        //#if 0//SENSOR_IF_USING_MIPI
        #if (SENSOR_IF != SENSOR_IF_PARALLEL)
        // for MIPI interface
        0x3017, 0x00,
        0x3018, 0x00,
        0x300e, 0x04, // SC_MIPI_SC_CTRL 0
        0x4801, 0x0f, // MIPI CONTROL 01
        #endif

        0x3a0f, 0x40,
        0x3a10, 0x38,
        0x3a1b, 0x48,
        0x3a1e, 0x30,
        0x3a11, 0x90,
        0x3a1f, 0x10,


        0x3500, 0x00, // long_exposure[19:16]
        0x3501, 0x10, // long_exposure[15:8]
        0x3502, 0x00, // long_exposure[7:0]

        0x3503, 0x07, // MANUAL CONTROL, [0]:AEC manual, 0: Auto, 1: manual
        //                 [1]:AGC manual, 0: Auto, 1: manual
        //                 [2]:VTS manual, 0: Auto, 1: manual

        0x3406, 0x01,  // manual AWB

        0x350c, 0x00, // VTS_DIFF[15:8], set to zero in manual mode
        0x350d, 0x00, // VTS_DIFF[7:0], set to zero in manual mode
        0x3c01, 0x80, // 5060HZ CONTROL 01

        0x4006, 0x00, // BLC target[9:8]
        0x4007, 0x00, // BLC target[7:0]
        0x4000, 0x01,//0x01, // BLC CONTROL 00, [0]: BLC enable
        0x401d, 0x22,//0x28, // BLC CONTROL 1D


        0x5000, 0x06,//0x02, // ISP CONTROL 00, [1]: wc_en, [2]: bc_en, [7]: lenc_en
        0x5001, 0x00, // ISP CONTROL 01 (AWB), [0]: awb_en
        //        0x5002, 0x00, // ISP CONTROL 02, [1]: vap_en
        //        0x503d, 0x00, // ISP CONTROL 3D,
        //        0x5900, 0x01, // VAP CONTROL 00, [0]: avg_en
        //        0x5901, 0x00, // VAP CONTROL 01, [3:2]: hsub_coef, [1:0]: vsub_coef

        //        0x5180, 0x00, // AWB CONTROL, [7]: debug mode, [6]: fast_awb
        #if SENSOR_ROTATE_180
        0x3818, 0xE0,
        #else
        0x3818, 0x80, // TIMING CONTROL 18, [6]: mirror, [5]: vertical flip, [0]: vsub2, [1]: vsub4
        #endif
        //#if 0//SENSOR_IF_USING_MIPI
        #if (SENSOR_IF != SENSOR_IF_PARALLEL)
        0x3008, 0x04, // Start streaming, [1]:
        0x300e, 0x04, // Start streaming, [1]:
        #else
        0x3008, 0x02, // Start streaming, [1]:
        0x300e, 0x02, // Start streaming, [1]:
        #endif


        0x300f, 0x8a, // PLL CONTROL, [2]: R_DIVL 0: one lane, 1: two lanes
        //0x3011, 0x0e,//0x0c, // 14*12*4/8 => PCLK = 672/8 = 84MHz
        0x3011, 0x14,//0x0c, // 14*12*4/8 => PCLK = 672/8 = 84MHz
        //        0x3011, 0x0f, // 11*15*4 = 660MHz => PCLK = 660/10 = 66MHz
        #if 0
        0x3012, 0x02, // [2:0]: R_PREDIV, 010: divide by 2
        #else
        0x3012, 0x00, // [2:0]: R_PREDIV, 000: divide by 1
        #endif

#endif
};

// 1928x1088 25fps
ISP_UINT16 SNR_OV2710_Reg_1920x1080_25P_Customer[] = 
{
	0x3103, 0x93,
	0x3008, 0x82,
	0x3017, 0x7f,
	0x3018, 0xfc,
	0x3706, 0x61,
	0x3712, 0x0c,
	0x3630, 0x6d,
	0x3801, 0xb4,
#if (SENSOR_ROTATE_180)
	0x3621, 0x14, // [7]: 1: H-sub, [4]: 0: if 0x3818[6]=1, 1: if 0x3818[6]=0
#else
	0x3621, 0x04, // [7]: 1: H-sub, [4]: 0: if 0x3818[6]=1, 1: if 0x3818[6]=0
#endif
	0x3604, 0x60,
	0x3603, 0xa7,
	0x3631, 0x26,
	0x3600, 0x04,
	0x3620, 0x37,
	0x3623, 0x00,
	0x3702, 0x9e,
	0x3703, 0x5c,
	0x3704, 0x40,
	0x370d, 0x0f,
	0x3713, 0x9f,
	0x3714, 0x4c,
	0x3710, 0x9e,
	0x3801, 0xc4,
	0x3605, 0x05,
	0x3606, 0x3f,
	0x302d, 0x90,
	0x370b, 0x40,
	0x3716, 0x31,
	0x3707, 0x52,
	0x380d, 0x74,
	0x5181, 0x20,
	0x518f, 0x00,
	0x4301, 0xff,
	0x4303, 0x00,
	0x3a00, 0x78,
	0x300f, 0x88,
	0x3011, 0x28,
	0x3a1a, 0x06,
	0x3a18, 0x00,
	0x3a19, 0x7a,
	0x3a13, 0x54,
	0x382e, 0x0f,
	0x381a, 0x1a,
	0x401d, 0x02,
	0x5688, 0x03,
	0x5684, 0x07,
	0x5685, 0xa0,
	0x5686, 0x04,
	0x5687, 0x43,
	0x3a0f, 0x40,
	0x3a10, 0x38,
	0x3a1b, 0x48,
	0x3a1e, 0x30,
	0x3a11, 0x90,
	0x3a1f, 0x10,

	0x3010, 0x00, // 0x20
	0x302c, 0xC0,       // driving 4x

	0x3820, 0x00,
	0x3821, 0x00,
	0x381c, 0x00,
	0x381d, 0x02,
	0x381e, 0x04,
	0x381f, 0x44,
	0x3800, 0x01,
	0x3801, 0xC4,
	0x3804, 0x07,
	0x3805, 0x88,
	0x3802, 0x00,
	0x3803, 0x0A,
	0x3806, 0x04,
	0x3807, 0x40,
	0x3808, 0x07,
	0x3809, 0x88,
	0x380a, 0x04,
	0x380b, 0x40,
	0x380c, 0x09,
	0x380d, 0x74, // 0xA4
	0x380e, 0x04,
	0x380f, 0x50,
	0x3810, 0x08,
	0x3811, 0x02,

#if (SENSOR_ROTATE_180)
	0x3621, 0x14, // [7]: 1: H-sub, [4]: 0: if 0x3818[6]=1, 1: if 0x3818[6]=0
#else
	0x3621, 0x04, // [7]: 1: H-sub, [4]: 0: if 0x3818[6]=1, 1: if 0x3818[6]=0
#endif
	0x3622, 0x08,
#if (SENSOR_ROTATE_180)
	0x3818, 0xE0, // TIMING CONTROL 18, [6]: mirror, [5]: vertical flip, [0]: vsub2, [1]: vsub4
#else
	0x3818, 0x80,
#endif

// AVG windows
	0x5688, 0x03,
	0x5684, 0x07,
	0x5685, 0x88,
	0x5686, 0x04,
	0x5687, 0x40,

	0x3a08, 0x14,
	0x3a09, 0xB3,
	0x3a0a, 0x11,
	0x3a0b, 0x40,
	0x3a0e, 0x03,
	0x3a0d, 0x04,

	0x401c, 0x08,

	// Off 3A
	//0x3503, 0x07, // off aec/agc
	0x3503, 0x37, // off aec/agc
	0x5001, 0x4e, // off awb
	//0x5000, 0x5f, // off lenc

// 25fps
	0x380c, 0x0b,
	0x380d, 0x50,
	0x5780, 0x00
};

// 1288x728 30fps
MMP_USHORT SNR_OV2710_Reg_1280x720_30P_Customer[] = 
{
	0x3103, 0x93,
	0x3008, 0x82,
	0x3017, 0x7f,
	0x3018, 0xfc,
	0x3706, 0x61,
	0x3712, 0x0c,
	0x3630, 0x6d,
	0x3801, 0xb4,
#if (SENSOR_ROTATE_180)
	0x3621, 0x14, // [7]: 1: H-sub, [4]: 0: if 0x3818[6]=1, 1: if 0x3818[6]=0
#else
	0x3621, 0x04, // [7]: 1: H-sub, [4]: 0: if 0x3818[6]=1, 1: if 0x3818[6]=0
#endif
	0x3604, 0x60,
	0x3603, 0xa7,
	0x3631, 0x26,
	0x3600, 0x04,
	0x3620, 0x37,
	0x3623, 0x00,
	0x3702, 0x9e,
	0x3703, 0x5c,
	0x3704, 0x40,
	0x370d, 0x0f,
	0x3713, 0x9f,
	0x3714, 0x4c,
	0x3710, 0x9e,
	0x3801, 0xc4,
	0x3605, 0x05,
	0x3606, 0x3f,
	0x302d, 0x90,
	0x370b, 0x40,
	0x3716, 0x31,
	0x3707, 0x52,
	0x380d, 0x74,
	0x5181, 0x20,
	0x518f, 0x00,
	0x4301, 0xff,
	0x4303, 0x00,
	0x3a00, 0x78,
	0x300f, 0x88,
	0x3011, 0x28,
	0x3a1a, 0x06,
	0x3a18, 0x00,
	0x3a19, 0x7a,
	0x3a13, 0x54,
	0x382e, 0x0f,
	0x381a, 0x1a,
	0x401d, 0x02,
	0x381c, 0x10,
	0x381d, 0xb8,
	0x381e, 0x02,
	0x381f, 0xdc,
	0x3820, 0x0a,
	0x3821, 0x29,
	0x3804, 0x05,
	0x3805, 0x00,
	0x3806, 0x02,
	0x3807, 0xd0,
	0x3808, 0x05,
	0x3809, 0x00,
	0x380a, 0x02,
	0x380b, 0xd0,
	0x380e, 0x02,
	0x380f, 0xe8,
	0x380c, 0x07,
	0x380d, 0x00,
	0x5688, 0x03,
	0x5684, 0x05,
	0x5685, 0x00,
	0x5686, 0x02,
	0x5687, 0xd0,
	0x3a08, 0x1b,
	0x3a09, 0xe6,
	0x3a0a, 0x17,
	0x3a0b, 0x40,
	0x3a0e, 0x01,
	0x3a0d, 0x02,
        #if (SENSOR_IF != SENSOR_IF_PARALLEL)
        // for MIPI interface
        0x3017, 0x00,
        0x3018, 0x00,
        0x300e, 0x04, // SC_MIPI_SC_CTRL 0
        0x4801, 0x0f, // MIPI CONTROL 01
        #endif
	0x3a0f, 0x40,
	0x3a10, 0x38,
	0x3a1b, 0x48,
	0x3a1e, 0x30,
	0x3a11, 0x90,
	0x3a1f, 0x10,

// 30FPS
	0x3010, 0x10,
	0x3012, 0x01, // [2:0]: R_PREDIV, 000: divide by 1

	0x3820, 0x09,
		
	0x3821, 0x29,
	0x381c, 0x10,
	0x381d, 0xBE,
	0x381e, 0x02,
	0x381f, 0xDC,
	0x3800, 0x01,
	0x3801, 0xC8,
	0x3804, 0x05,
	0x3805, 0x08,
	0x3802, 0x00,
	0x3803, 0x0A,
	0x3806, 0x02,
	0x3807, 0xD8,
	0x3808, 0x05,
	0x3809, 0x08,
	0x380a, 0x02,
	0x380b, 0xD8,
	0x380c, 0x07,
	0x380d, 0x00, // 0x04
        #if (SENSOR_IF != SENSOR_IF_PARALLEL)
        0x3008, 0x04, // Start streaming, [1]:
        0x300e, 0x04, // Start streaming, [1]:
        #else
        0x3008, 0x02, // Start streaming, [1]:
        0x300e, 0x02, // Start streaming, [1]:
        #endif
	0x380f, 0xE8,
	0x3810, 0x08,
	0x3811, 0x02,

	// Off 3A
	//0x3503, 0x07, // off aec/agc
	0x3503, 0x37, // off aec/agc
	0x5001, 0x4e, // off awb
	//0x5000, 0x5f, // off lenc
	0x5780, 0x00,

#if (SENSOR_ROTATE_180)
	0x3818, 0xE0, // TIMING CONTROL 18, [6]: mirror, [5]: vertical flip, [0]: vsub2, [1]: vsub4
#else
	0x3818, 0x80
#endif
};

// 1288x728 60fps
MMP_USHORT SNR_OV2710_Reg_1280x720_60P_Customer[] = 
{
	0x3103, 0x93,
	0x3008, 0x82,
	0x3017, 0x7f,
	0x3018, 0xfc,
	0x3706, 0x61,
	0x3712, 0x0c,
	0x3630, 0x6d,
	0x3801, 0xb4,
#if (SENSOR_ROTATE_180)
	0x3621, 0x14, // [7]: 1: H-sub, [4]: 0: if 0x3818[6]=1, 1: if 0x3818[6]=0
#else
	0x3621, 0x04, // [7]: 1: H-sub, [4]: 0: if 0x3818[6]=1, 1: if 0x3818[6]=0
#endif
	0x3604, 0x60,
	0x3603, 0xa7,
	0x3631, 0x26,
	0x3600, 0x04,
	0x3620, 0x37,
	0x3623, 0x00,
	0x3702, 0x9e,
	0x3703, 0x5c,
	0x3704, 0x40,
	0x370d, 0x0f,
	0x3713, 0x9f,
	0x3714, 0x4c,
	0x3710, 0x9e,
	0x3801, 0xc4,
	0x3605, 0x05,
	0x3606, 0x3f,
	0x302d, 0x90,
	0x370b, 0x40,
	0x3716, 0x31,
	0x3707, 0x52,
	0x380d, 0x74,
	0x5181, 0x20,
	0x518f, 0x00,
	0x4301, 0xff,
	0x4303, 0x00,
	0x3a00, 0x78,
	0x300f, 0x88,
	0x3011, 0x28,
	0x3a1a, 0x06,
	0x3a18, 0x00,
	0x3a19, 0x7a,
	0x3a13, 0x54,
	0x382e, 0x0f,
	0x381a, 0x1a,
	0x401d, 0x02,
	0x381c, 0x10,
	0x381d, 0xb8,
	0x381e, 0x02,
	0x381f, 0xdc,
	0x3820, 0x0a,
	0x3821, 0x29,
	0x3804, 0x05,
	0x3805, 0x00,
	0x3806, 0x02,
	0x3807, 0xd0,
	0x3808, 0x05,
	0x3809, 0x00,
	0x380a, 0x02,
	0x380b, 0xd0,
	0x380e, 0x02,
	0x380f, 0xe8,
	0x380c, 0x07,
	0x380d, 0x00,
	0x5688, 0x03,
	0x5684, 0x05,
	0x5685, 0x00,
	0x5686, 0x02,
	0x5687, 0xd0,
	0x3a08, 0x1b,
	0x3a09, 0xe6,
	0x3a0a, 0x17,
	0x3a0b, 0x40,
	0x3a0e, 0x01,
	0x3a0d, 0x02,
	0x3a0f, 0x40,
	0x3a10, 0x38,
	0x3a1b, 0x48,
	0x3a1e, 0x30,
	0x3a11, 0x90,
	0x3a1f, 0x10,

 // 60FPS
	0x3010, 0x00,
	0x302c, 0xC0,       // driving 4x

	0x3820, 0x09,
		
	0x3821, 0x29,
	0x381c, 0x10,
	0x381d, 0xBE,
	0x381e, 0x02,
	0x381f, 0xDC,
	0x3800, 0x01,
	0x3801, 0xC8,
	0x3804, 0x05,
	0x3805, 0x08,
	0x3802, 0x00,
	0x3803, 0x0A,
	0x3806, 0x02,
	0x3807, 0xD8,
	0x3808, 0x05,
	0x3809, 0x08,
	0x380a, 0x02,
	0x380b, 0xD8,
	0x380c, 0x07,
	0x380d, 0x00, // 0x04
	0x380e, 0x02,
	0x380f, 0xE8,
	0x3810, 0x08,
	0x3811, 0x02,
	
	// Off 3A
	//0x3503, 0x07, // off aec/agc
	0x3503, 0x37, // off aec/agc
	0x5001, 0x4e, // off awb
	//0x5000, 0x5f, // off lenc
	0x5780, 0x00,

#if (SENSOR_ROTATE_180)
	0x3818, 0xE0, // TIMING CONTROL 18, [6]: mirror, [5]: vertical flip, [0]: vsub2, [1]: vsub4
#else
	0x3818, 0x80
#endif
};

// 1448x1088 30fps
ISP_UINT16 SNR_OV2710_Reg_1440x1080_30P_Customer[] = 
{
	0x3103, 0x93,
	0x3008, 0x82,
	0x3017, 0x7f,
	0x3018, 0xfc,
	0x3706, 0x61,
	0x3712, 0x0c,
	0x3630, 0x6d,
	0x3801, 0xb4,
#if (SENSOR_ROTATE_180)
	0x3621, 0x14, // [7]: 1: H-sub, [4]: 0: if 0x3818[6]=1, 1: if 0x3818[6]=0
#else
	0x3621, 0x04, // [7]: 1: H-sub, [4]: 0: if 0x3818[6]=1, 1: if 0x3818[6]=0
#endif
	0x3604, 0x60,
	0x3603, 0xa7,
	0x3631, 0x26,
	0x3600, 0x04,
	0x3620, 0x37,
	0x3623, 0x00,
	0x3702, 0x9e,
	0x3703, 0x5c,
	0x3704, 0x40,
	0x370d, 0x0f,
	0x3713, 0x9f,
	0x3714, 0x4c,
	0x3710, 0x9e,
	0x3801, 0xc4,
	0x3605, 0x05,
	0x3606, 0x3f,
	0x302d, 0x90,
	0x370b, 0x40,
	0x3716, 0x31,
	0x3707, 0x52,
	0x380d, 0x74,
	0x5181, 0x20,
	0x518f, 0x00,
	0x4301, 0xff,
	0x4303, 0x00,
	0x3a00, 0x78,
	0x300f, 0x88,
	0x3011, 0x28,
	0x3a1a, 0x06,
	0x3a18, 0x00,
	0x3a19, 0x7a,
	0x3a13, 0x54,
	0x382e, 0x0f,
	0x381a, 0x1a,
	0x401d, 0x02,
	0x5688, 0x03,
	0x5684, 0x07,
	0x5685, 0xa0,
	0x5686, 0x04,
	0x5687, 0x43,
	0x3a0f, 0x40,
	0x3a10, 0x38,
	0x3a1b, 0x48,
	0x3a1e, 0x30,
	0x3a11, 0x90,
	0x3a1f, 0x10,

	0x3010, 0x00, // 0x20
	0x302c, 0xC0,       // driving 4x

	0x3820, 0x00,
	0x3821, 0x00,
	0x381c, 0x00,
	0x381d, 0x02,
	0x381e, 0x04,
	0x381f, 0x44,
	0x3800, 0x02, // 0x01
	0x3801, 0xb4, // 0xC4
	0x3804, 0x05, // 0x07
	0x3805, 0xa8, // 0x88
	0x3802, 0x00,
	0x3803, 0x0A,
	0x3806, 0x04,
	0x3807, 0x40,
	0x3808, 0x05, // 0x07
	0x3809, 0xa8, // 0x88
	0x380a, 0x04,
	0x380b, 0x40,
	0x380c, 0x09,
	0x380d, 0x74, // 0xA4
	0x380e, 0x04,
	0x380f, 0x50,
	0x3810, 0x08,
	0x3811, 0x02,

// AVG windows
	0x5688, 0x03,
	0x5684, 0x05, // 007
	0x5685, 0xa8, // 088
	0x5686, 0x04,
	0x5687, 0x40,

	0x3a00, 0x58,

	// Off 3A
	0x3503, 0x07, // off aec/agc
	0x5001, 0x4e, // off awb
	//0x5000, 0x5f, // off lenc
	0x5780, 0x00,

#if (SENSOR_ROTATE_180)
	0x3818, 0xE0, // TIMING CONTROL 18, [6]: mirror, [5]: vertical flip, [0]: vsub2, [1]: vsub4
#else
	0x3818, 0x80
#endif
};

// 648x488 30fps
ISP_UINT16 SNR_OV2710_Reg_640x480_30P_Customer[] = 
{
	0x3103, 0x93,
	0x3008, 0x82,
	0x3017, 0x7f,
	0x3018, 0xfc,
	0x3706, 0x61,
	0x3712, 0x0c,
	0x3630, 0x6d,
	0x3801, 0xb4,
#if (SENSOR_ROTATE_180)
	0x3621, 0x14, // [7]: 1: H-sub, [4]: 0: if 0x3818[6]=1, 1: if 0x3818[6]=0
#else
	0x3621, 0x04, // [7]: 1: H-sub, [4]: 0: if 0x3818[6]=1, 1: if 0x3818[6]=0
#endif
	0x3604, 0x60,
	0x3603, 0xa7,
	0x3631, 0x26,
	0x3600, 0x04,
	0x3620, 0x37,
	0x3623, 0x00,
	0x3702, 0x9e,
	0x3703, 0x5c,
	0x3704, 0x40,
	0x370d, 0x0f,
	0x3713, 0x9f,
	0x3714, 0x4c,
	0x3710, 0x9e,
	0x3801, 0xc4,
	0x3605, 0x05,
	0x3606, 0x3f,
	0x302d, 0x90,
	0x370b, 0x40,
	0x3716, 0x31,
	0x3707, 0x52,
	0x380d, 0x74,
	0x5181, 0x20,
	0x518f, 0x00,
	0x4301, 0xff,
	0x4303, 0x00,
	0x3a00, 0x78,
	0x300f, 0x88,
	0x3011, 0x28,
	0x3a1a, 0x06,
	0x3a18, 0x00,
	0x3a19, 0x7a,
	0x3a13, 0x54,
	0x382e, 0x0f,
	0x381a, 0x1a,
	0x401d, 0x02,
	0x3803, 0x06,
	0x381c, 0x10,
	0x381d, 0x42,
	0x381e, 0x03,
	0x381f, 0xc8,
	0x3820, 0x0a,
	0x3821, 0x29,
	0x3804, 0x02,
	0x3805, 0x80,
	0x3806, 0x01,
	0x3807, 0xe0,
	0x3808, 0x02,
	0x3809, 0x80,
	0x380a, 0x01,
	0x380b, 0xe0,
	0x3810, 0x08,
	0x3811, 0x02,
#if (SENSOR_ROTATE_180)
	0x3818, 0xE1, // TIMING CONTROL 18, [6]: mirror, [5]: vertical flip, [0]: vsub2, [1]: vsub4
#else
	0x3818, 0x81,
#endif
#if (SENSOR_ROTATE_180)
	0x3621, 0xd4, // [7]: 1: H-sub, [4]: 0: if 0x3818[6]=1, 1: if 0x3818[6]=0
#else
	0x3621, 0xc4, // [7]: 1: H-sub, [4]: 0: if 0x3818[6]=1, 1: if 0x3818[6]=0
#endif
	0x380e, 0x01,
	0x380f, 0xf0,
	0x380c, 0x05,
	0x380d, 0x40,
	0x3a08, 0x24,
	0x3a09, 0xe6,
	0x3a0a, 0x1e,
	0x3a0b, 0xc0,
	0x3a0e, 0x00,
	0x3a0d, 0x01,
	0x401c, 0x04,
	0x3a0f, 0x40,
	0x3a10, 0x38,
	0x3a1b, 0x48,
	0x3a1e, 0x30,
	0x3a11, 0x90,
	0x3a1f, 0x10,

// 30FPS
	0x3615, 0xf0,
	0x3010, 0x30, // 0x10
	0x302c, 0xC0,       // driving 4x

	// Off 3A
	0x3503, 0x07, // off aec/agc
	0x5001, 0x4e, // off awb

	0x3820, 0x09,
		
	0x3821, 0x2A,
	0x381c, 0x10,
	0x381d, 0x3E,
	0x381e, 0x03,
	0x381f, 0xD8,
	0x3800, 0x01,
	0x3801, 0xC8,
	0x3804, 0x02,
	0x3805, 0x88,
	0x3802, 0x00,
	0x3803, 0x06,
	0x3806, 0x01,
	0x3807, 0xE8,
	0x3808, 0x02,
	0x3809, 0x88,
	0x380a, 0x01,
	0x380b, 0xE8,
	0x380c, 0x05,
	0x380d, 0x38,
	0x380e, 0x01,
	0x380f, 0xF4,
	0x3810, 0x08,
	0x3811, 0x02,
	
	0x3500, 0x00,
	0x3501, 0x1e,
	0x3502, 0xd0,
	0x5780, 0x00,

#if (SENSOR_ROTATE_180)
	0x3818, 0xE0, // TIMING CONTROL 18, [6]: mirror, [5]: vertical flip, [0]: vsub2, [1]: vsub4
#else
	0x3818, 0x80,
#endif
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

    RTNA_DBG_Str(0, "!!SNR_Cust_InitConfig OV2710mipi\r\n");

    // Init OPR Table
    SensorCustFunc.OprTable->usInitSize 							= (sizeof(SNR_OV2710_Reg_Init_Customer)/sizeof(SNR_OV2710_Reg_Init_Customer[0]))/2;
    SensorCustFunc.OprTable->uspInitTable 							= &SNR_OV2710_Reg_Init_Customer[0];

    SensorCustFunc.OprTable->bBinTableExist                         = MMP_FALSE;
    SensorCustFunc.OprTable->bInitDoneTableExist                    = MMP_FALSE;

    for (i = 0; i < MAX_SENSOR_RES_MODE; i++)
    {
        SensorCustFunc.OprTable->usSize[i] 							= (sizeof(SNR_OV2710_Reg_Unsupport)/sizeof(SNR_OV2710_Reg_Unsupport[0]))/2;
        SensorCustFunc.OprTable->uspTable[i] 						= &SNR_OV2710_Reg_Unsupport[0];
    }

    // 16:9
    SensorCustFunc.OprTable->usSize[RES_IDX_1920x1080_30P] 			= (sizeof(SNR_OV2710_Reg_1920x1080_30P_Customer)/sizeof(SNR_OV2710_Reg_1920x1080_30P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1920x1080_30P] 		= &SNR_OV2710_Reg_1920x1080_30P_Customer[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_1920x1080_25P] 			= (sizeof(SNR_OV2710_Reg_1920x1080_25P_Customer)/sizeof(SNR_OV2710_Reg_1920x1080_25P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1920x1080_25P] 		= &SNR_OV2710_Reg_1920x1080_25P_Customer[0];
#if 1 // cropped from FHD30P, TBC??
    SensorCustFunc.OprTable->usSize[RES_IDX_1280x720_30P] 			= (sizeof(SNR_OV2710_Reg_1920x1080_30P_Customer)/sizeof(SNR_OV2710_Reg_1920x1080_30P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720_30P] 		= &SNR_OV2710_Reg_1920x1080_30P_Customer[0];
#else
    SensorCustFunc.OprTable->usSize[RES_IDX_1280x720_30P] 			= (sizeof(SNR_OV2710_Reg_1280x720_30P_Customer)/sizeof(SNR_OV2710_Reg_1280x720_30P_Customer[0]))/2; // TBD
    SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720_30P] 		= &SNR_OV2710_Reg_1280x720_30P_Customer[0];
#endif
    SensorCustFunc.OprTable->usSize[RES_IDX_1280x720_60P] 			= (sizeof(SNR_OV2710_Reg_1280x720_60P_Customer)/sizeof(SNR_OV2710_Reg_1280x720_60P_Customer[0]))/2; // TBD
    SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720_60P] 		= &SNR_OV2710_Reg_1280x720_60P_Customer[0];

    // 4:3
#if 1 // cropped from FHD30P, TBC??
    SensorCustFunc.OprTable->usSize[RES_IDX_1440x1080_30P] 			= (sizeof(SNR_OV2710_Reg_1920x1080_30P_Customer)/sizeof(SNR_OV2710_Reg_1920x1080_30P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1440x1080_30P] 		= &SNR_OV2710_Reg_1920x1080_30P_Customer[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_640x480_30P] 			= (sizeof(SNR_OV2710_Reg_1920x1080_30P_Customer)/sizeof(SNR_OV2710_Reg_1920x1080_30P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_640x480_30P] 			= &SNR_OV2710_Reg_1920x1080_30P_Customer[0];
#else
    SensorCustFunc.OprTable->usSize[RES_IDX_1440x1080_30P] 			= (sizeof(SNR_OV2710_Reg_1440x1080_30P_Customer)/sizeof(SNR_OV2710_Reg_1440x1080_30P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1440x1080_30P] 		= &SNR_OV2710_Reg_1440x1080_30P_Customer[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_640x480_30P] 			= (sizeof(SNR_OV2710_Reg_640x480_30P_Customer)/sizeof(SNR_OV2710_Reg_640x480_30P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_640x480_30P] 			= &SNR_OV2710_Reg_640x480_30P_Customer[0];
#endif

    // Init Vif Setting : Common
    SensorCustFunc.VifSetting->SnrType                              = MMPF_VIF_SNR_TYPE_BAYER;
#if (SENSOR_IF == SENSOR_IF_PARALLEL)
    SensorCustFunc.VifSetting->OutInterface 						= MMPF_VIF_IF_PARALLEL;
#elif (SENSOR_IF == SENSOR_IF_MIPI_1_LANE)
#if (PRM_SENSOR_VIF_ID == MMPF_VIF_MDL_ID1)
    SensorCustFunc.VifSetting->OutInterface 						= MMPF_VIF_IF_MIPI_SINGLE_1;//use for 2531project
#else 	
    SensorCustFunc.VifSetting->OutInterface 						= MMPF_VIF_IF_MIPI_SINGLE_0;//use for HSG
#endif
#elif (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)
#if (PRM_SENSOR_VIF_ID == MMPF_VIF_MDL_ID1)
    SensorCustFunc.VifSetting->OutInterface                         = MMPF_VIF_IF_MIPI_DUAL_01;
#else
    SensorCustFunc.VifSetting->OutInterface                         = MMPF_VIF_IF_MIPI_DUAL_01; //TBD
#endif
#elif (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)
    SensorCustFunc.VifSetting->OutInterface                         = MMPF_VIF_IF_MIPI_QUAD;
#endif

#if (PRM_SENSOR_VIF_ID == MMPF_VIF_MDL_ID1) //MIPI 2 lane or 1 lane
    SensorCustFunc.VifSetting->VifPadId  = MMPF_VIF_MDL_ID1; 
#else 
    SensorCustFunc.VifSetting->VifPadId = MMPF_VIF_MDL_ID0; 
#endif     
    // Init Vif Setting : PowerOn Setting
    SensorCustFunc.VifSetting->powerOnSetting.bTurnOnExtPower 		= MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPin 		= SENSOR_GPIO_ENABLE; // it might be defined in Config_SDK.h
    SensorCustFunc.VifSetting->powerOnSetting.bExtPowerPinHigh 		= SENSOR_GPIO_ENABLE_ACT_LEVEL;
    SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPinDelay 	= 0; // 0 ms which is suggested by Kenny Shih @ 20150327
    SensorCustFunc.VifSetting->powerOnSetting.bFirstEnPinHigh 		= MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubFirstEnPinDelay 	= 10;
    SensorCustFunc.VifSetting->powerOnSetting.bNextEnPinHigh 		= MMP_FALSE;
    SensorCustFunc.VifSetting->powerOnSetting.ubNextEnPinDelay 		= 5;
    SensorCustFunc.VifSetting->powerOnSetting.bTurnOnClockBeforeRst = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.bFirstRstPinHigh 		= MMP_FALSE;
    SensorCustFunc.VifSetting->powerOnSetting.ubFirstRstPinDelay 	= 10;
    SensorCustFunc.VifSetting->powerOnSetting.bNextRstPinHigh 		= MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubNextRstPinDelay 	= 10;

    // Init Vif Setting : PowerOff Setting
    SensorCustFunc.VifSetting->powerOffSetting.bEnterStandByMode 	= MMP_FALSE;
    SensorCustFunc.VifSetting->powerOffSetting.usStandByModeReg 	= 0x0;
    SensorCustFunc.VifSetting->powerOffSetting.usStandByModeMask 	= 0x0;
    SensorCustFunc.VifSetting->powerOffSetting.bEnPinHigh 			= MMP_TRUE;
    SensorCustFunc.VifSetting->powerOffSetting.ubEnPinDelay 		= 10;
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
    SensorCustFunc.VifSetting->paralAttr.ubVsyncPolarity 			= MMPF_VIF_SNR_CLK_POLARITY_NEG;
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
#if (PRM_SENSOR_VIF_ID == MMPF_VIF_MDL_ID1)
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[0] 			= MMPF_VIF_MIPI_DATA_SRC_PHY_1;//use for 2531project	
#else
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[0] 			= MMPF_VIF_MIPI_DATA_SRC_PHY_0;//use for HSG project
#endif
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[1] 			= MMPF_VIF_MIPI_DATA_SRC_PHY_UNDEF;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[2] 			= MMPF_VIF_MIPI_DATA_SRC_PHY_UNDEF;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[3] 			= MMPF_VIF_MIPI_DATA_SRC_PHY_UNDEF;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[0] 				= 0x08;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[1] 				= 0x08;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[2] 				= 0x08;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[3] 				= 0x08;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[0] 			= 0x0F;//0x0e;
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
#if (PRM_SENSOR_VIF_ID == MMPF_VIF_MDL_ID1)    
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[0]            = MMPF_VIF_MIPI_DATA_SRC_PHY_1;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[1]            = MMPF_VIF_MIPI_DATA_SRC_PHY_2;
#else
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[0]            = MMPF_VIF_MIPI_DATA_SRC_PHY_1;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[1]            = MMPF_VIF_MIPI_DATA_SRC_PHY_2;
#endif    
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
ISP_UINT32 drop_flag = 0;
ISP_UINT32 isp_gain;
ISP_UINT32 energy;

#define MAX_SHUTTER_STEP (12)//7
#define SKIP_FRAME_TH    (331)//276
#define SKIP_FRAME_CNT   (20) 

void SNR_Cust_DoAE_FrmSt(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
#if (ISP_EN)
	MMP_ULONG ulVsync = 0;
	MMP_ULONG ulShutter = 0;
	MMP_UBYTE ubPeriod 				= (SensorCustFunc.pAeTime)->ubPeriod;
	MMP_UBYTE ubFrmStSetShutFrmCnt 	= (SensorCustFunc.pAeTime)->ubFrmStSetShutFrmCnt;
	MMP_UBYTE ubFrmStSetGainFrmCnt 	= (SensorCustFunc.pAeTime)->ubFrmStSetGainFrmCnt;
	
	static MMP_USHORT usPreShutter = 0;
	static MMP_UBYTE  ubPauseAE = 0;
	MMP_USHORT usDiff;	
	MMP_USHORT avglum;
	MMP_USHORT aetarget;
	
	ISP_IF_CMD_SendCommandtoAE(0x31, &avglum, 2, 1);
	
	// For AE curve
	if ((ulFrameCnt % 100) == 10)
	{
		ISP_IF_CMD_SendCommandtoAE(0x51, AE_Bias_tbl,AE_tbl_size, 0); // <<AE table set once at preview start
		ISP_IF_NaturalAE_Enable(0); // 0: no, 1: ENERGY, 2: Lux, 3: test mode
		//ISP_IF_AE_SetFlicker(ISP_AE_FLICKER_60HZ); // temp solution, it needs to be fixed and removed, 50HZ 1011, 60HZ 1124
	}

	if (ulFrameCnt == SKIP_FRAME_CNT) {
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x4000, 0x09); // End group1
		//RTNA_DBG_Str(0,"Set Sensor BLC \r\n");
	}

	if (ulFrameCnt % ubPeriod == ubFrmStSetShutFrmCnt)
	{
		avglum   = ISP_IF_AE_GetDbgData(1);
		aetarget = ISP_IF_AE_GetDbgData(0);
		
		if (ubPauseAE==0 || ulFrameCnt < SKIP_FRAME_CNT) {
			ISP_IF_AE_Execute();
		}

		s_gain = VR_MAX(ISP_IF_AE_GetGain(), ISP_IF_AE_GetGainBase());

		if (s_gain >= (ISP_IF_AE_GetGainBase() * MAX_SENSOR_GAIN))
		{
			s_gain  = ISP_IF_AE_GetGainBase() * MAX_SENSOR_GAIN;
		}

		ulVsync 	= (gSnrLineCntPerSec[ubSnrSel] * ISP_IF_AE_GetVsync()) / ISP_IF_AE_GetVsyncBase();
		ulShutter 	= (gSnrLineCntPerSec[ubSnrSel] * ISP_IF_AE_GetShutter()) / ISP_IF_AE_GetShutterBase();
		
		//v line issue
		if (ulShutter > usPreShutter && ulShutter < SKIP_FRAME_TH && (avglum*2) > aetarget) {
			usDiff = ulShutter - usPreShutter;

			if (usDiff > MAX_SHUTTER_STEP) {
				ulShutter =  usPreShutter + MAX_SHUTTER_STEP;
				ubPauseAE = 1;
			}
			else {
				ubPauseAE = 0;
		    }
		}
		else {
			ubPauseAE = 0;
		}
		usPreShutter = ulShutter;

		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3212, 0x00); // Enable group1
		gsSensorFunction->MMPF_Sensor_SetShutter(ubSnrSel, ulShutter, ulVsync);
		gsSensorFunction->MMPF_Sensor_SetGain(ubSnrSel, s_gain);
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3212, 0x10); // End group1
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3212, 0xA0); // Launch group1
		
		if (drop_flag == 1) {
			ISP_IF_IQ_SetAEGain(isp_gain, ISP_IF_AE_GetGainBase());
		}
	}
	else if ((ulFrameCnt % ubPeriod) == ubFrmStSetGainFrmCnt)
	{
		//gsSensorFunction->MMPF_Sensor_SetGain(ubSnrSel, s_gain);
		ISP_IF_IQ_SetAEGain(isp_gain, ISP_IF_AE_GetGainBase());
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
void SNR_Cust_SetGain(MMP_UBYTE ubSnrSel, MMP_ULONG AGain)
{
#if (0)
	ISP_UINT16 s_gain;
	ISP_UINT32 sensor_DataH, sensor_DataL;

	s_gain = AGain>>8;

	if((AGain>>8)>=32)
	{
		sensor_DataH = 0x1F;
		sensor_DataL = (AGain & ((sensor_DataH<<8)+ 0xFF))>>9;
	}
	else if((AGain>>8)>=16)
	{
		sensor_DataH = 0x0F;
		sensor_DataL = (AGain & ((sensor_DataH<<8)+ 0xFF))>>8;
	}
	else if((AGain>>8)>=8)
	{
		sensor_DataH = 0x07;
		sensor_DataL = (AGain & ((sensor_DataH<<8)+ 0xFF))>>7;
	}
	else if((AGain>>8)>=4)
	{
		sensor_DataH = 0x03;
		sensor_DataL = (AGain & ((sensor_DataH<<8)+ 0xFF))>>6;
	}
	else if((AGain>>8)>=2)
	{
		sensor_DataH = 0x01;
		sensor_DataL = (AGain & ((sensor_DataH<<8)+ 0xFF))>>5;
	}
	else
	{
		sensor_DataH = 0x00;
		sensor_DataL = (AGain & ((sensor_DataH<<8)+ 0xFF))>>4;
	}
	
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3212, 0x01);

	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x350A, ((sensor_DataH >> 3) & 0x01));
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x350B, (((sensor_DataH & 0x0F)<<4) + sensor_DataL));

	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3212, 0x11);
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3212, 0xA1); 

#else
	
	ISP_UINT32 sensor_gain, ulGain;
	ISP_UINT32 sensor_DataH, sensor_DataL;//, isp_gain;

	ulGain = AGain;
	
	// convert to base 16 = 1x
	//sensor_gain 	= VR_MIN(16 * VR_MAX(ulGain, ISP_IF_AE_GetGainBase()) / ISP_IF_AE_GetGainBase() , 255);
	sensor_gain 	= VR_MIN(16 * VR_MAX(ulGain, ISP_IF_AE_GetGainBase()) / ISP_IF_AE_GetGainBase() , 496);//SP86  //x31
	
	if (sensor_gain >= 0x200) {
		sensor_DataH = 0x1F;
		sensor_DataL = ((sensor_gain - 0x200) >> 5) & 0x0F;
		sensor_gain = 32;
	}
	else if (sensor_gain >= 0x100) {
		sensor_DataH = 0x0F;
		sensor_DataL = ((sensor_gain - 0x100) >> 4) & 0x0F;
		sensor_gain = 16;
	}
	else if (sensor_gain >= 0x80) {
		sensor_DataH = 0x07;
		sensor_DataL = ((sensor_gain - 0x80) >> 3) & 0x0F;
		sensor_gain = 8;
	}
	else if (sensor_gain >= 0x40) {
		sensor_DataH = 0x03;
		sensor_DataL = ((sensor_gain - 0x40) >> 2) & 0x0F;
		sensor_gain = 4;
	}
	else if (sensor_gain >= 0x20) {
		sensor_DataH = 0x01;
		sensor_DataL = ((sensor_gain - 0x20) >> 1) & 0x0F;
		sensor_gain = 2;
	}
	else {
		sensor_DataH = 0x00;
		sensor_DataL = ((sensor_gain - 0x10) >> 0) & 0x0F;
		sensor_gain = 1;
	}
	
	// use isp digital gain
	sensor_gain = sensor_gain * (sensor_DataL + 16) * ISP_IF_AE_GetGainBase() / 16;
	isp_gain = ulGain * ISP_IF_AE_GetGainBase() / sensor_gain;	 
	
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x350A,  (sensor_DataH & 0x10) >> 4);
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x350B, ((sensor_DataH & 0x0F) << 4) + sensor_DataL);
#endif
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetShutter
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetShutter(MMP_UBYTE ubSnrSel, MMP_ULONG shutter, MMP_ULONG vsync)
{
    // 1 ~ (frame length line - 6)
    ISP_UINT32 ExtraVsyncWidth;

    static ISP_UINT32 pre_shutter = 0;

    if(pre_shutter < shutter && shutter > SKIP_FRAME_TH) //1/120s = 1928 * 30 / 30
    {
        //Do not drop frame. It causes slow card problem due to sensor input is less than 30FPS.
        //drop_flag = 1;
        RTNA_DBG_Str(0, "[Shutter ov]");
    }
    else {
        drop_flag = 0;
    }
    pre_shutter = shutter;

    if(shutter > (vsync - 6))
        shutter = vsync - 6;

    if (shutter <= (vsync - 6))
    {
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3500, (ISP_UINT8)((shutter >> 12) & 0xFF));
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3501, (ISP_UINT8)((shutter >> 4) & 0xFF));
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3502, (ISP_UINT8)((shutter << 4) & 0xFF));
    }
    else
    {
        //		ExtraVsyncWidth = (shutter + 6) - vsync;

        //		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x350C, (ISP_UINT8)((ExtraVsyncWidth >> 8) & 0xFF));
        //		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x350D, (ISP_UINT8)((ExtraVsyncWidth) & 0xFF));

        //		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3500, (ISP_UINT8)((shutter >> 12) & 0xFF));
        //		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3501, (ISP_UINT8)((shutter >> 4) & 0xFF));
        //		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3502, (ISP_UINT8)((shutter << 4) & 0xFF));
    }

    if (drop_flag == 1) {
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x4202, 0x01); //frame mask
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3000, 0x23); //timing reset
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3000, 0xa0); //back normal timing
    }
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetFlip
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetFlip(MMP_UBYTE ubSnrSel, MMP_UBYTE ubMode)
{
	// TBD  0x3818, 0xE0        0x3621, 0x14,
	if (ubMode == MMPF_SENSOR_NO_FLIP) 
	{
	    gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3621, 0x04);
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3818, 0x80);
        SensorCustFunc.VifSetting->colorId.VifColorId = MMPF_VIF_COLORID_11;
	}	
	else if (ubMode == MMPF_SENSOR_COLROW_FLIP) 
	{
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3621, 0x14);
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3818, 0xE0);
        SensorCustFunc.VifSetting->colorId.VifColorId = MMPF_VIF_COLORID_10;
	}
    SensorCustFunc.VifSetting->colorId.CustomColorId.bUseCustomId  	= MMP_FALSE;
	printc("SNR_Cust_SetFlip\n");
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

#endif // (BIND_SENSOR_OV2710_MIPI)
#endif // (SENSOR_EN)

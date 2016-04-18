//==============================================================================
//
//  File        : sensor_ov5653.c
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
#if (BIND_SENSOR_OV5653)

#include "mmpf_sensor.h"
#include "mmp_reg_vif.h"
#include "mmpf_i2cm.h"
#include "Sensor_Mod_Remapping.h"
#include "isp_if.h"

#define SNR1440_MODE_USING_FHD_CROPPING

//==============================================================================
//
//                              GLOBAL VARIABLE
//
//==============================================================================

// Resolution Table
MMPF_SENSOR_RESOLUTION m_SensorRes = 
{
	5, 										// ubSensorModeNum
	0, 										// ubDefPreviewMode
	4, 										// ubDefCaptureMode
	3000,                                   // usPixelSize
// Mode0	Mode1	Mode2 	Mode3 	Mode4
	{1,  	1,  	1,    	241,   	1}, 	// usVifGrabStX
	{1,  	1,	    1,      1,     	1}, 	// usVifGrabStY
	{1936, 	1288,   1288,   1448,  	2592}, 	// usVifGrabW
	{1104, 	736,    736,   	1088,  	1944}, 	// usVifGrabH
#if (CHIP == MCR_V2)
	{1,   	1,      1,      1,      1}, 	// usBayerInGrabX
	{1,   	1,      1,      1,      1}, 	// usBayerInGrabY
	{14,   	8,      8,      8,      30}, 	// usBayerDummyInX
	{22,   	16,     16,     8,      22}, 	// usBayerDummyInY
	{1922, 	1280,   1280,   1440,	2562}, 	// usBayerOutW
	{1082, 	720,    720,   	1080, 	1922}, 	// usBayerOutH
#endif
	{1920, 	1280,   1280,   1440, 	2560}, 	// usScalInputW
	{1080, 	720,    720,   	1080, 	1922}, 	// usScalInputH
	{300,   300,    600,    300,    140}, 	// usTargetFpsx10
	{1148,  752,    752,    1104,   1960}, 	// usVsyncLine
	{1,   	1,      1,      1,      1}, 	// ubWBinningN
	{1,   	1,      1,      1,      1}, 	// ubWBinningM
	{1,   	1,      1,      1,      1}, 	// ubHBinningN
	{1,   	1,      1,      1,      1}, 	// ubHBinningM
    {0xFF,	0xFF,	0xFF,   0xFF,   0xFF}, 	// ubCustIQmode
    {0xFF,	0xFF,	0xFF,   0xFF,   0xFF}  	// ubCustAEmode
};

// OPR Table and Vif Setting
MMPF_SENSOR_OPR_TABLE       m_OprTable;
MMPF_SENSOR_VIF_SETTING     m_VifSetting;

// IQ Table
const ISP_UINT8 Sensor_IQ_CompressedText[] = 
{
    #ifdef CUS_ISP_8428_IQ_DATA
    #include CUS_ISP_8428_IQ_DATA
    #else
    #include "isp_8428_iq_data_v2_OV5653_150707.xls.ciq.txt"
    #endif
};

// I2cm Attribute
static MMP_I2CM_ATTR m_I2cmAttr = 
{
    MMP_I2CM0,  // i2cmID
    #ifdef SENSOR_I2C_ADDR
    SENSOR_I2C_ADDR,
    #else
    0x36,       // ubSlaveAddr
    #endif
    16,         // ubRegLen
    8,          // ubDataLen
    0,          // ubDelayTime
    MMP_FALSE,  // bDelayWaitEn
    MMP_TRUE,   // bInputFilterEn
    MMP_FALSE,  // b10BitModeEn
    MMP_FALSE,  // bClkStretchEn
    0,          // ubSlaveAddr1
    0,          // ubDelayCycle
    0,          // ubPadNum
    150000,     // ulI2cmSpeed, unit is HZ
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
	6, 	// ubPeriod
	1, 	// ubDoAWBFrmCnt
	3 	// ubDoCaliFrmCnt
};

MMPF_SENSOR_AETIMIMG m_AeTime = 
{
	4, 	// ubPeriod
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

// Only OV2710 need to refrence this gain table. Other sensor may can set gain by calculate.
const ISP_UINT8 OV5653_GainTable[128] = 
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

ISP_UINT16 SNR_OV5653_Reg_Unsupport[] = 
{
	SENSOR_DELAY_REG, 100 // delay
};

ISP_UINT16 SNR_OV5653_Reg_Init_Customer[] = 
{
	0x3008, 0x82,
	0x3008, 0x42,
	//
	0x3103, 0x93,
	0x3b07, 0x0c,
	0x3017, 0xff,
	0x3018, 0xfc,
	0x3706, 0x41,
	0x3630, 0x22,
	0x3605, 0x04,
	0x3606, 0x3f,
	0x3712, 0x13,
	0x370e, 0x00,
	0x370b, 0x40,
	0x3600, 0x54,
	0x3601, 0x05,
	0x3631, 0x22,
	0x3612, 0x1a,
	0x3604, 0x40,
	0x3710, 0x28,
	0x3702, 0x3a,
	0x3704, 0x18,
	0x3a18, 0x00,
	0x3a19, 0xf8,
	0x3a00, 0x38,
	0x3830, 0x50,
	0x3a08, 0x12,
	0x3a09, 0x70,
	0x3a0a, 0x0f,
	0x3a0b, 0x60,
	0x3a0d, 0x06,
	0x3a0e, 0x06,
	0x3a13, 0x54,
	0x3815, 0x82,
	0x5059, 0x80,
	0x3615, 0x52,
	0x3a08, 0x16,
	0x3a09, 0x48,
	0x3a0a, 0x12,
	0x3a0b, 0x90,
	0x3a0d, 0x03,
	0x3a0e, 0x03,
	0x3a1a, 0x06,
	0x3623, 0x01,
	0x3633, 0x24,
	0x3c01, 0x34,
	0x3c04, 0x28,
	0x3c05, 0x98,
	0x3c07, 0x07,
	0x3c09, 0xc2,
	0x4000, 0x05,
	0x401d, 0x28,
	0x4001, 0x02,
	0x401c, 0x46,
	0x5046, 0x01,
	0x3810, 0x40,
	0x3836, 0x41,
	0x505f, 0x04,
	0x5000, 0x06,//defect enable //0x00,
	0x5001, 0x00,
	0x503d, 0x00,
	0x585a, 0x01,
	0x585b, 0x2c,
	0x585c, 0x01,
	0x585d, 0x93,
	0x585e, 0x01,
	0x585f, 0x90,
	0x5860, 0x01,
	0x5861, 0x0d,
	0x5180, 0xc0,
	0x5184, 0x00,
	0x470a, 0x00,
	0x470b, 0x00,
	0x470c, 0x00,
	0x300f, 0x8e,
	0x3603, 0xa7,
	0x3632, 0x55,
	0x3620, 0x56,
	0x3631, 0x36,
	0x3632, 0x5f,
	0x3711, 0x24,
	0x401f, 0x03,
	0x3010, 0x10,
	0x3011, 0x10,
	0x3406, 0x01,
	0x3400, 0x04,
	0x3401, 0x00,
	0x3402, 0x04,
	0x3403, 0x00,
	0x3404, 0x04,
	0x3405, 0x00,
	0x3503, 0x37,//0x17,
	0x3500, 0x00,
	0x3501, 0x18,
	0x3502, 0x00,
	0x350a, 0x00,
//	0x350b, 0x38,
	0x4704, 0x00,
	0x4708, 0x01,
	0x3008, 0x02,
	SENSOR_DELAY_REG, 100 // delay
};

#if 0
void ____Sensor_Res_OPR_Table____(){ruturn;} //dummy
#endif

MMP_USHORT SNR_OV5653_Reg_2560x1920_Customer[] =
{
	//2592 x 1944 14fps mclk = 24M pclk = 96M
	0x3800, 0x02,
	0x3801, 0x22,
	0x3803, 0x0C,
	0x3804, 0x0A,
	0x3805, 0x20,//+8
	0x3806, 0x07,
	0x3807, 0x98,//+8
	0x3808, 0x0A,
	0x3809, 0x20,//+8
	0x380A, 0x07,
	0x380B, 0x98,//+8

	0x380C, 0x0D,//08
	0x380D, 0xAA,//-66  

	0x380E, 0x07,//03
	0x380F, 0xA8,//-e8

	0x3815, 0x82,
	#ifdef SENSOR_ROTATE_180
	0x3818, 0xA0, // TIMING CONTROL 18, [6]: mirror, [5]: vertical flip, [0]: vsub2, [1]: vsub4
	#else
	0x3818, 0xC0,
	#endif
	0x381A, 0x1C,
	0x381C, 0x2f,//-30 V-start  //0x31,
	0x381D, 0xf2,	// 0x12,//-fa  //0x02,
	0x381E, 0x07,
	0x381F, 0xB8,//-d0 //0xC0,
	0x3820, 0x00,
	0x3821, 0x20,
	0x3824, 0x22,
	0x3825, 0x54,//-26 //0x2A,

	0x4007, 0x14,

	0x3613, 0x44,
	#ifdef SENSOR_ROTATE_180
	0x3621, 0x3F, // [7]: 1: H-sub, [4]: 0: if 0x3818[6]=1, 1: if 0x3818[6]=0
	#else
	0x3621, 0x2F,
	#endif
	0x3703, 0xE6,
	0x3705, 0xDA,
	0x370A, 0x80,
	0x370C, 0x00,
	0x370D, 0x04,
	0x3713, 0x22,
	0x3714, 0x27,
	0x401C, 0x46,
	0x5002, 0x00,
	#ifdef SENSOR_ROTATE_180
	0x505A, 0x00,
	0x505B, 0x12,
	#else
	0x505A, 0x0A,
	0x505B, 0x2E,
	#endif
	0x5901, 0x00,
	//0x3010, 0x00,//0x00,//0x10,
	//0x3011, 0x10,//0x10,

	0x401d, 0x08,
	0x3810, 0x40,
};

// 1928x1088 30fps
ISP_UINT16 SNR_OV5653_Reg_1920x1080_30P_Customer[] = 
{
	0x3800, 0x02,       // HSYNC Start
	0x3801, 0x22,
	0x3802, 0x00,       // VSYNC Start
	0x3803, 0x0C,
	0x3804, 0x07,       // HREF Width, 1952
	0x3805, 0xA0,//+8
	0x3806, 0x04,       // VREF Height, 1112
	0x3807, 0x58,//+8
	0x3808, 0x07,       // DVP H output size, 1952
	0x3809, 0xA0,//+8
	0x380A, 0x04,       // DVP V output size, 1112
	0x380B, 0x58,//+8

	0x380C, 0x0B,       // Total H Size, 2836
	0x380D, 0x14,

	0x380E, 0x04,//03   // Total V Size, 1128
	0x380F, 0x68,//-e8

	0x3815, 0x82,
	#ifdef SENSOR_ROTATE_180
	0x3818, 0xA0,
	#else	
	0x3818, 0xC0,
	#endif
	0x381A, 0x1C,
	0x381C, 0x31,//-30 V-start  //0x31,
	0x381D, 0xB2,//-fa  //0x02,
	0x381E, 0x04,
	0x381F, 0x78,//-d0 //0xC0,
	0x3820, 0x02,
	0x3821, 0x19,
	0x3824, 0x22,
	0x3825, 0xD4,//-26 //0x2A,

	0x4007, 0x10,

	0x3613, 0x44,
	#ifdef SENSOR_ROTATE_180
	0x3621, 0x3F, // [7]: 1: H-sub, [4]: 0: if 0x3818[6]=1, 1: if 0x3818[6]=0
	#else
	0x3621, 0x2F,
	#endif
	0x3703, 0xE6,
	0x3705, 0xDA,
	0x370A, 0x80,
	0x370C, 0x00,
	0x370D, 0x04,
	0x3713, 0x22,
	0x3714, 0x27,
	0x401C, 0x46,
	0x5002, 0x00,
	#ifdef SENSOR_ROTATE_180
	0x505A, 0x00,
	0x505B, 0x12,
	#else
	0x505A, 0x0A,
	0x505B, 0x2E,
	#endif
	0x5901, 0x00,
	//0x3010, 0x00,
	//0x3011, 0x10,

//	0x401d, 0x28,
	0x401d, 0x08,
	0x3810, 0x40
};

// 1928x1088 25fps
ISP_UINT16 SNR_OV5653_Reg_1920x1080_25P_Customer[] = 
{
	0x3800, 0x02,
	0x3801, 0x22,
	0x3803, 0x0C,
	0x3804, 0x07,
	0x3805, 0xA0,//+8
	0x3806, 0x04,
	0x3807, 0x58,//+8
	0x3808, 0x07,
	0x3809, 0xA0,//+8
	0x380A, 0x04,
	0x380B, 0x58,//+8

	0x380C, 0x0B,
	0x380D, 0x14,

	0x380E, 0x04,//03
	0x380F, 0x68,//-e8

	0x3815, 0x82,
	0x3818, 0xC0,
	0x381A, 0x1C,
	0x381C, 0x31,//-30 V-start  //0x31,
	0x381D, 0xB2,//-fa  //0x02,
	0x381E, 0x04,
	0x381F, 0x78,//-d0 //0xC0,
	0x3820, 0x02,
	0x3821, 0x19,
	0x3824, 0x22,
	0x3825, 0xD4,//-26 //0x2A,

	0x4007, 0x10,

	0x3613, 0x44,
	0x3621, 0x2F,
	0x3703, 0xE6,
	0x3705, 0xDA,
	0x370A, 0x80,
	0x370C, 0x00,
	0x370D, 0x04,
	0x3713, 0x22,
	0x3714, 0x27,
	0x401C, 0x46,
	0x5002, 0x00,
	0x505A, 0x0A,
	0x505B, 0x2E,
	0x5901, 0x00,
	//0x3010, 0x00,
	//0x3011, 0x10,

//	0x401d, 0x28,
	0x401d, 0x08,	
	0x3810, 0x40,

// 25fps
	0x380c, 0x0b,
	0x380d, 0x50
};

// 1288x728 30fps
MMP_USHORT SNR_OV5653_Reg_1280x720_30P_Customer[] = 
{
	0x3800, 0x02,
	0x3801, 0x22,
	0x3803, 0x0C,
	0x3804, 0x05,
	0x3805, 0x08,//+8
	0x3806, 0x02,
	0x3807, 0xE0,//+8
	0x3808, 0x05,
	0x3809, 0x08,//+8
	0x380A, 0x02,
	0x380B, 0xE0,//+8

	0x380C,0x10,
	0x380D,0x9F,
	/*
	0x380C, 0x0B,//uvc
	0x380D, 0x8C,//uvc
	*/
	0x380E, 0x02,//03
	0x380F, 0xF0,//-e8

	0x3815, 0x81,
	#ifdef SENSOR_ROTATE_180
	0x3818, 0xA1, // TIMING CONTROL 18, [6]: mirror, [5]: vertical flip, [0]: vsub2, [1]: vsub4
	#else
	0x3818, 0xC1,
	#endif
	0x381A, 0x00,
	0x381C, 0x30,//-30 V-start  //0x31,
	0x381D, 0xF2,//-fa  //0x02,
	0x381E, 0x05,
	0x381F, 0xE0,//-d0 //0xC0,
	0x3820, 0x00,
	0x3821, 0x20,
	0x3824, 0x23,
	0x3825, 0x26,//-26 //0x2A,

	0x4007, 0x14,

	0x3613, 0xC4,
	#ifdef SENSOR_ROTATE_180
	0x3621, 0xBF, // [7]: 1: H-sub, [4]: 0: if 0x3818[6]=1, 1: if 0x3818[6]=0
	#else
	0x3621, 0xAF,
	#endif
	0x3703, 0x9A,
	0x3705, 0xDB,
	0x370A, 0x81,
	0x370C, 0x00,
	0x370D, 0x42,
	0x3713, 0x92,
	0x3714, 0x17,
	0x401C, 0x42,
	0x5002, 0x00,
	#ifdef SENSOR_ROTATE_180
	0x505A, 0x00,
	0x505B, 0x12,
	#else
	0x505A, 0x0A,
	0x505B, 0x2E,
	#endif
	0x5901, 0x00,
	//0x3010, 0x00,
	//0x3011, 0x10,
	
	0x3810, 0x40,
};

// 1288x728 60fps
MMP_USHORT SNR_OV5653_Reg_1280x720_60P_Customer[] = 
{
	0x3800, 0x02,
	0x3801, 0x22,
	0x3803, 0x0C,
	0x3804, 0x05,
	0x3805, 0x08,//+8
	0x3806, 0x02,
	0x3807, 0xE0,//+8
	0x3808, 0x05,
	0x3809, 0x08,//+8
	0x380A, 0x02,
	0x380B, 0xE0,//+8

	0x380C,0x08,
	0x380D,0x4F,
	/*
	0x380C, 0x0B,//uvc
	0x380D, 0x8C,//uvc
	*/
	0x380E, 0x02,//03
	0x380F, 0xE0,//-e8

	0x3815, 0x81,
	#ifdef SENSOR_ROTATE_180
	0x3818, 0xA1, // TIMING CONTROL 18, [6]: mirror, [5]: vertical flip, [0]: vsub2, [1]: vsub4
	#else
	0x3818, 0xC1,
	#endif
	0x381A, 0x00,
	0x381C, 0x30,//-30 V-start  //0x31,
	0x381D, 0xF2,//-fa  //0x02,
	0x381E, 0x05,
	0x381F, 0xE0,//-d0 //0xC0,
	0x3820, 0x00,
	0x3821, 0x20,
	0x3824, 0x23,
	0x3825, 0x26,//-26 //0x2A,

	0x4007, 0x14,

	0x3613, 0xC4,
	#ifdef SENSOR_ROTATE_180
	0x3621, 0xBF, // [7]: 1: H-sub, [4]: 0: if 0x3818[6]=1, 1: if 0x3818[6]=0
	#else
	0x3621, 0xAF,
	#endif
	0x3703, 0x9A,
	0x3705, 0xDB,
	0x370A, 0x81,
	0x370C, 0x00,
	0x370D, 0x42,
	0x3713, 0x92,
	0x3714, 0x17,
	0x401C, 0x42,
	0x5002, 0x00,
	#ifdef SENSOR_ROTATE_180
	0x505A, 0x00,
	0x505B, 0x12,
	#else
	0x505A, 0x0A,
	0x505B, 0x2E,
	#endif
	0x5901, 0x00,
	//0x3010, 0x00,//0x00,//0x10,
	//0x3011, 0x10,//0x0B uvc,//0x10,

//	0x401d, 0x28,
	0x401d, 0x08,
	
	0x3810, 0x40,
};

// 1448x1088 30fps
ISP_UINT16 SNR_OV5653_Reg_1440x1080_30P_Customer[] = 
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
	0x5000, 0x5f, // off lenc

#if (SENSOR_ROTATE_180)
	0x3818, 0xE0, // TIMING CONTROL 18, [6]: mirror, [5]: vertical flip, [0]: vsub2, [1]: vsub4
#else
	0x3818, 0x80
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

	RTNA_DBG_Str(0, "SNR_Cust_InitConfig OV5653\r\n");

	// Init OPR Table
	SensorCustFunc.OprTable->usInitSize 							= (sizeof(SNR_OV5653_Reg_Init_Customer)/sizeof(SNR_OV5653_Reg_Init_Customer[0]))/2;
	SensorCustFunc.OprTable->uspInitTable 							= &SNR_OV5653_Reg_Init_Customer[0];

	for (i = 0; i < MAX_SENSOR_RES_MODE; i++)
	{
		SensorCustFunc.OprTable->usSize[i] 							= (sizeof(SNR_OV5653_Reg_Unsupport)/sizeof(SNR_OV5653_Reg_Unsupport[0]))/2;
		SensorCustFunc.OprTable->uspTable[i] 						= &SNR_OV5653_Reg_Unsupport[0];
	}

	// 16:9
	SensorCustFunc.OprTable->usSize[RES_IDX_1920x1080_30P] 			= (sizeof(SNR_OV5653_Reg_1920x1080_30P_Customer)/sizeof(SNR_OV5653_Reg_1920x1080_30P_Customer[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_1920x1080_30P] 		= &SNR_OV5653_Reg_1920x1080_30P_Customer[0];
#if 1 // cropped from FHD30P, TBC??
	SensorCustFunc.OprTable->usSize[RES_IDX_1280x720_30P] 			= (sizeof(SNR_OV5653_Reg_1920x1080_30P_Customer)/sizeof(SNR_OV5653_Reg_1920x1080_30P_Customer[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720_30P] 		= &SNR_OV5653_Reg_1920x1080_30P_Customer[0];
#else
	SensorCustFunc.OprTable->usSize[RES_IDX_1280x720_30P] 			= (sizeof(SNR_OV2710_Reg_1280x720_30P_Customer)/sizeof(SNR_OV2710_Reg_1280x720_30P_Customer[0]))/2; // TBD
	SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720_30P] 		= &SNR_OV2710_Reg_1280x720_30P_Customer[0];
#endif
	SensorCustFunc.OprTable->usSize[RES_IDX_1280x720_60P] 			= (sizeof(SNR_OV5653_Reg_1280x720_60P_Customer)/sizeof(SNR_OV5653_Reg_1280x720_60P_Customer[0]))/2; // TBD
	SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720_60P] 		= &SNR_OV5653_Reg_1280x720_60P_Customer[0];
	
	// 4:3
	SensorCustFunc.OprTable->usSize[RES_IDX_2560x1920_14P] 			= (sizeof(SNR_OV5653_Reg_2560x1920_Customer)/sizeof(SNR_OV5653_Reg_2560x1920_Customer[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_2560x1920_14P] 		= &SNR_OV5653_Reg_2560x1920_Customer[0];
#ifdef SNR1440_MODE_USING_FHD_CROPPING  // cropped from FHD30P, TBC??
	SensorCustFunc.OprTable->usSize[RES_IDX_1440x1080_30P] 			= (sizeof(SNR_OV5653_Reg_1920x1080_30P_Customer)/sizeof(SNR_OV5653_Reg_1920x1080_30P_Customer[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_1440x1080_30P] 		= &SNR_OV5653_Reg_1920x1080_30P_Customer[0];
	SensorCustFunc.OprTable->usSize[RES_IDX_640x480_30P] 			= (sizeof(SNR_OV5653_Reg_1920x1080_30P_Customer)/sizeof(SNR_OV5653_Reg_1920x1080_30P_Customer[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_640x480_30P] 			= &SNR_OV5653_Reg_1920x1080_30P_Customer[0];
#else
	SensorCustFunc.OprTable->usSize[RES_IDX_1440x1080_30P] 			= (sizeof(SNR_OV5653_Reg_1440x1080_30P_Customer)/sizeof(SNR_OV5653_Reg_1440x1080_30P_Customer[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_1440x1080_30P] 		= &SNR_OV5653_Reg_1440x1080_30P_Customer[0];
	SensorCustFunc.OprTable->usSize[RES_IDX_640x480_30P] 			= (sizeof(SNR_OV2710_Reg_640x480_30P_Customer)/sizeof(SNR_OV2710_Reg_640x480_30P_Customer[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_640x480_30P] 			= &SNR_OV2710_Reg_640x480_30P_Customer[0];
#endif

	// Init Vif Setting : Common
    SensorCustFunc.VifSetting->SnrType                              = MMPF_VIF_SNR_TYPE_BAYER;
#if (SENSOR_IF == SENSOR_IF_PARALLEL)
	SensorCustFunc.VifSetting->OutInterface 						= MMPF_VIF_IF_PARALLEL;
#elif (SENSOR_IF == SENSOR_IF_MIPI_1_LANE)
	SensorCustFunc.VifSetting->OutInterface 						= MMPF_VIF_IF_MIPI_SINGLE_0;
#elif (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)
    SensorCustFunc.VifSetting->OutInterface                         = MMPF_VIF_IF_MIPI_DUAL_01;
#elif (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)
    SensorCustFunc.VifSetting->OutInterface                         = MMPF_VIF_IF_MIPI_QUAD;
#endif
	SensorCustFunc.VifSetting->VifPadId 							= MMPF_VIF_MDL_ID0;

	// Init Vif Setting : PowerOn Setting
	SensorCustFunc.VifSetting->powerOnSetting.bTurnOnExtPower 		= MMP_FALSE;
	SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPin 		= SENSOR_GPIO_ENABLE;
	SensorCustFunc.VifSetting->powerOnSetting.bExtPowerPinHigh 		= SENSOR_GPIO_ENABLE_ACT_LEVEL;
	SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPinDelay 	= 2;

	SensorCustFunc.VifSetting->powerOnSetting.bFirstEnPinHigh 		= MMP_TRUE;
	SensorCustFunc.VifSetting->powerOnSetting.ubFirstEnPinDelay 	= 1;
	SensorCustFunc.VifSetting->powerOnSetting.bNextEnPinHigh 		= MMP_FALSE;
	SensorCustFunc.VifSetting->powerOnSetting.ubNextEnPinDelay 		= 10;
	SensorCustFunc.VifSetting->powerOnSetting.bTurnOnClockBeforeRst = MMP_TRUE;
	SensorCustFunc.VifSetting->powerOnSetting.bFirstRstPinHigh 		= MMP_FALSE;
	SensorCustFunc.VifSetting->powerOnSetting.ubFirstRstPinDelay 	= 30;
	SensorCustFunc.VifSetting->powerOnSetting.bNextRstPinHigh 		= MMP_TRUE;
	SensorCustFunc.VifSetting->powerOnSetting.ubNextRstPinDelay 	= 30;

	// Init Vif Setting : PowerOff Setting
	SensorCustFunc.VifSetting->powerOffSetting.bEnterStandByMode 	= MMP_FALSE;
	SensorCustFunc.VifSetting->powerOffSetting.usStandByModeReg 	= 0x0;
	SensorCustFunc.VifSetting->powerOffSetting.usStandByModeMask 	= 0x0;
	SensorCustFunc.VifSetting->powerOffSetting.bEnPinHigh 			= MMP_TRUE;
	SensorCustFunc.VifSetting->powerOffSetting.ubEnPinDelay 		= 20;
	SensorCustFunc.VifSetting->powerOffSetting.bTurnOffMClock 		= MMP_TRUE;
	SensorCustFunc.VifSetting->powerOffSetting.bTurnOffExtPower 	= MMP_TRUE;
	SensorCustFunc.VifSetting->powerOffSetting.usExtPowerPin 		= MMP_GPIO_MAX;

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

	// Init Vif Setting : Color ID Setting
#if (SENSOR_ROTATE_180)
	SensorCustFunc.VifSetting->colorId.VifColorId 					= MMPF_VIF_COLORID_10;
#else
	SensorCustFunc.VifSetting->colorId.VifColorId 					= MMPF_VIF_COLORID_11;
#endif
	SensorCustFunc.VifSetting->colorId.CustomColorId.bUseCustomId 	= MMP_FALSE;

    // Enable Pin
    MMPF_VIF_SetPIODir(SensorCustFunc.VifSetting->VifPadId, VIF_SIF_SEN, MMP_TRUE);
    MMPF_VIF_SetPIOOutput(SensorCustFunc.VifSetting->VifPadId, VIF_SIF_SEN, GPIO_HIGH);
    MMPF_OS_Sleep(1);

    // Reset Pin
    MMPF_VIF_SetPIODir(SensorCustFunc.VifSetting->VifPadId, VIF_SIF_RST, MMP_TRUE);
    MMPF_VIF_SetPIOOutput(SensorCustFunc.VifSetting->VifPadId, VIF_SIF_RST, GPIO_HIGH);
    MMPF_OS_Sleep(1);
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

	if (ulFrameCnt % ubPeriod == ubFrmStSetShutFrmCnt)
	{
		ISP_IF_AE_Execute();

		s_gain = VR_MAX(ISP_IF_AE_GetGain(), ISP_IF_AE_GetGainBase());

		if (s_gain >= (ISP_IF_AE_GetGainBase() * MAX_SENSOR_GAIN))
		{
			s_gain  = ISP_IF_AE_GetGainBase() * MAX_SENSOR_GAIN;
		}

		ulVsync 	= (gSnrLineCntPerSec[ubSnrSel] * ISP_IF_AE_GetVsync()) / ISP_IF_AE_GetVsyncBase();
		ulShutter 	= (gSnrLineCntPerSec[ubSnrSel] * ISP_IF_AE_GetShutter()) / ISP_IF_AE_GetShutterBase();
		gsSensorFunction->MMPF_Sensor_SetShutter(ubSnrSel, ulShutter, ulVsync);
		gsSensorFunction->MMPF_Sensor_SetGain(ubSnrSel, s_gain);
	}
	else if ((ulFrameCnt % ubPeriod) == ubFrmStSetGainFrmCnt)
	{
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
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetGain
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetGain(MMP_UBYTE ubSnrSel, MMP_ULONG AGain)
{
#if (ISP_EN)
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
#if (ISP_EN)
	ISP_UINT32 new_vsync    = gSnrLineCntPerSec[ubSnrSel] * ISP_IF_AE_GetVsync() / ISP_IF_AE_GetVsyncBase();
	ISP_UINT32 new_shutter  = gSnrLineCntPerSec[ubSnrSel] * ISP_IF_AE_GetShutter() / ISP_IF_AE_GetShutterBase();

	new_vsync   = ISP_MIN(ISP_MAX(new_shutter + 3, new_vsync), 0xFFFF);
	new_shutter = ISP_MIN(ISP_MAX(new_shutter, 1), new_vsync - 3);

//	if (shutter <= (vsync - 6))
	{
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x380E, (ISP_UINT8)((new_vsync >> 8) & 0xFF));
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x380F, (ISP_UINT8)((new_vsync)      & 0xFF));
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3500, (ISP_UINT8)((new_shutter >> 12) & 0xFF));
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3501, (ISP_UINT8)((new_shutter >> 4) & 0xFF));
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3502, (ISP_UINT8)((new_shutter << 4) & 0xFF));
	}
#endif
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetFlip
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetFlip(MMP_UBYTE ubSnrSel, MMP_UBYTE ubMode)
{
#if 0
    extern MMP_USHORT gsCurPreviewMode[];
    MMP_USHORT usValue = 0;

    switch (ubMode)
    {
    case MMPF_SENSOR_NO_FLIP:
        switch(gsCurPreviewMode[SensorCustFunc.sPriority]) {
        case RES_IDX_2560x1920_14P:
        case RES_IDX_1920x1080_30P:
#ifdef SNR1440_MODE_USING_FHD_CROPPING
        case RES_IDX_1440x1080_30P:
#endif
            gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3621, 0x2F);
            break;

        default:
        //case RES_IDX_1280x720_30P:
        //case RES_IDX_640x480_30P:
        //case RES_IDX_1280x720_60P:
            gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3621, 0xAF);
            break;
        }

        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x505A, 0x0A);
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x505B, 0x2E);

        gsSensorFunction->MMPF_Sensor_GetReg(ubSnrSel, 0x3818, &usValue);
        usValue &= ~0x60;
        usValue |= 0x40;
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3818, usValue);

        // Set Color ID
        SensorCustFunc.VifSetting->colorId.VifColorId = MMPF_VIF_COLORID_11;
        break;

    case MMPF_SENSOR_COLROW_FLIP:
        switch(gsCurPreviewMode[SensorCustFunc.sPriority]) {
        case RES_IDX_2560x1920_14P:
        case RES_IDX_1920x1080_30P:
#ifdef SNR1440_MODE_USING_FHD_CROPPING
        case RES_IDX_1440x1080_30P:
#endif
            gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3621, 0x3F);
            gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x505A, 0x00);
            gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x505B, 0x12); 
            break;

        default:
        //case RES_IDX_1280x720_30P:
        //case RES_IDX_640x480_30P:
        //case RES_IDX_1280x720_60P:
            gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3621, 0xBF);
            break;
        }

        gsSensorFunction->MMPF_Sensor_GetReg(ubSnrSel, 0x3818, &usValue);
		usValue &= ~0x60;
		usValue |= 0x20;
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3818, usValue);	

        // Set Color ID
    	SensorCustFunc.VifSetting->colorId.VifColorId = MMPF_VIF_COLORID_10;
		break;

    default:
        return;
    }

    MMPF_VIF_SetColorID(SensorCustFunc.VifSetting->VifPadId, SensorCustFunc.VifSetting->colorId.VifColorId);
#endif
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
    printc(FG_BLUE("OV5653 Module_Init \r\n"));

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

#endif // (BIND_SENSOR_OV5653)
#endif // (SENSOR_EN)

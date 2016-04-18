//==============================================================================
//
//  File        : sensor_ov4689.c
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
#if (BIND_SENSOR_OV4689_2LINE)

#include "mmpf_sensor.h"
#include "sensor_ov4689_2lane.h"
#include "isp_if.h"
#include "hdr_cfg.h"

//==============================================================================
//
//                              GLOBAL VARIABLE
//
//==============================================================================

// Resolution Table
MMPF_SENSOR_RESOLUTION m_SensorRes = 
{
    3,     // ubSensorModeNum
    2,      // ubDefPreviewMode
    2,      // ubDefCaptureMode
    2000,   // usPixelSize
// 	Mode0   Mode1   Mode2	
    {1,    	1,      1},    	// usVifGrabStX
    {1,    	1,      1},    	// usVifGrabStY
    {2692, 	1288,   1928}, 	// usVifGrabW
    {1524,  964,   	1092}, 	// usVifGrabH
    
#if (CHIP == MCR_V2)
    {1,     1,      1},    	// usBayerInGrabX
    {1,     1,      1},    	// usBayerInGrabY
    {4,   	8,      8},   	// usBayerInDummyX
    {12,    4,     	12},    	// usBayerInDummyY
    {2688, 	1280,   1920},	// usBayerOutW
    {1512,  960,   	1080}, 	// usBayerOutH
#endif

    {2688, 	1280,   1920}, 	// usScalInputW
    {1512,  960,   	1080}, 	// usScalInputH    
    {300,   300,    300}, 	// usTargetFpsx10
    {1562, 	1597,   1136},	// usVsyncLine
    {1,     1,      1},    	// ubWBinningN
    {1,     1,      1},    	// ubWBinningM
    {1,     1,      1},    	// ubHBinningN
    {1,     1,      1},     // ubHBinningM
    {0xFF,	0xFF,	0xFF},  // ubCustIQmode
    {0xFF,	0xFF,	0xFF}   // ubCustAEmode
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
#include "isp_8428_iq_data_v2_OV4689_v1_casio.xls.ciq.txt"
#endif
};

// I2cm Attribute
static MMP_I2CM_ATTR m_I2cmAttr = 
{
    MMP_I2CM0,      // i2cmID
/*
 * In the OV4689, the SCCB ID is controlled by the SID pin.
 * If SID pin is low, the sensor's SCCB address comes from register 0x3004 which has a default value of 0x6C.
 * If SID pin is high, the sensor's SCCB address comes from register 0x3012 which has a default value of 0x20.
 * The device slave address are 0x6C for write and 0x6D for read (when SID = 1, 0x20 for write and 0x21 for read).
 */
#if defined (SNR_OV4689_SID_PIN) && (SNR_OV4689_SID_PIN == 1) // defined in Config_xxx.h
    (0x20 >> 1),    // ubSlaveAddr
#else
    (0x6C >> 1),    // ubSlaveAddr
#endif
    16,             // ubRegLen
    8,              // ubDataLen
    0,              // ubDelayTime
    MMP_FALSE,      // bDelayWaitEn
    MMP_TRUE,       // bInputFilterEn
    MMP_FALSE,      // b10BitModeEn
    MMP_FALSE,      // bClkStretchEn
    0,              // ubSlaveAddr1
    0,              // ubDelayCycle
    0,              // ubPadNum
    150000,         // ulI2cmSpeed 150KHZ
    MMP_TRUE,       // bOsProtectEn
    NULL,           // sw_clk_pin
    NULL,           // sw_data_pin
    MMP_FALSE,      // bRfclModeEn
    MMP_FALSE,      // bWfclModeEn
	MMP_FALSE,		// bRepeatModeEn
	0               // ubVifPioMdlId
};

// 3A Timing
MMPF_SENSOR_AWBTIMIMG m_AwbTime = 
{
    3,  // ubPeriod
    1,  // ubDoAWBFrmCnt
    2   // ubDoCaliFrmCnt
};

MMPF_SENSOR_AETIMIMG m_AeTime = 
{
    4,  // ubPeriod
    0,  // ubFrmStSetShutFrmCnt
    0   // ubFrmStSetGainFrmCnt
};

MMPF_SENSOR_AFTIMIMG m_AfTime = 
{
    1,  // ubPeriod
    0   // ubDoAFFrmCnt
};

#if (ISP_EN) //For HDR
static ISP_UINT16 ae_gain_l;
static ISP_UINT16 ae_shutter_l;
static ISP_UINT16 ae_gain_s;
static ISP_UINT16 ae_shutter_s;

ISP_UINT16 hdr_th           = 6;        //fix
ISP_UINT16 hdr_gap          = 7;        //fix
ISP_UINT16 hdr_gain         = 6*256;    //dynamic by code
ISP_UINT16 hdr_short_gain   = 13;       //fix
ISP_UINT16 hdr_x_thd        = 1023;     //fix
ISP_UINT16 hdr_on           = 1;        //hdr on
ISP_UINT16 sensor_max_gain  = 16;       //fix
#endif

// Casio : when use this, open code in DoAEOperation_ST
#ifdef CUS_AE_BIAS_TBL		// maybe defined in project's MCP or Config_SDK.h

#include CUS_AE_BIAS_TBL

#else

//AE curve	new extent node for18//LV1,		LV2,		LV3,		LV4,		LV5,		LV6,		LV7,		LV8,		LV9,		LV10,	LV11,	LV12,	LV13,	LV14,	LV15,	LV16 	LV17  	LV18
//test,IQ 8
ISP_UINT32 AE_Bias_tbl[54] =
/*lux*/						{4,			8,			15,			30,			56, 		118, 		224, 		426, 		810,		1630,	3369,	7101,	14720,	28380,	53300,	110000,	220000, 440000 /*930000=LV17*/
/*ENG*/						,0x2FFFFFF, 4841472*2,	2420736  ,  1562060,	801396,     439704, 	241131, 	132233, 	72400,		39000,	19000,	11000,	5000,	3084,	1716,	966,	370 , 200
/*Tar*/						,110,		145,		    175,	    200,		225,		240,		253,		256,		256,		256,	256,	256,	256,	226,	226,	256,	256 , 256//400
 };	

#define AE_tbl_size  (18)

#endif

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

#if 0
void ____Sensor_Init_OPR_Table____(){ruturn;} //dummy
#endif

ISP_UINT16 SNR_OV4689_Reg_Init_Customer[] = 
{
    SENSOR_DELAY_REG, 100 // delay
};

#if 0
void ____Sensor_Res_OPR_Table____(){ruturn;} //dummy
#endif

ISP_UINT16 SNR_OV4689_Reg_Unsupport[] = 
{
    SENSOR_DELAY_REG, 100 // delay
};

ISP_UINT16 SNR_OV4689_Reg_2688x1520_30P[] = 
{
#if (SENSOR_IF == SENSOR_IF_MIPI_1_LANE)
    // TBD
    SENSOR_DELAY_REG, 100 // delay
#elif (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)
    // TBD
    SENSOR_DELAY_REG, 100 // delay
#elif (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)
	// TBD
    SENSOR_DELAY_REG, 100 // delay
#endif
};



ISP_UINT16 SNR_OV4689_Reg_1280x960_30P[] = 
{
#if (SENSOR_IF == SENSOR_IF_MIPI_1_LANE)
    // TBD
    SENSOR_DELAY_REG, 100 // delay
#elif (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)
    // TBD
    SENSOR_DELAY_REG, 100 // delay
#elif (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)
    // TBD
    SENSOR_DELAY_REG, 100 // delay
#endif
};

ISP_UINT16 SNR_OV4689_Reg_1920x1080_30P[] = 
{
#if (SENSOR_IF == SENSOR_IF_MIPI_1_LANE)
    // TBD
    SENSOR_DELAY_REG, 100 // delay
#elif (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)
		/*0x0103,0x01,
			0x3638,0x00,
			0x0300,0x00,
			0x0301,0x00,
			0x0302,0x2a,
			0x0303,0x00,
			0x0304,0x03,
			0x0305,0x01,
			0x0306,0x01,
			0x030a,0x00,
			0x030b,0x00,
			0x030c,0x00,
			0x030d,0x1e,
			0x030e,0x04,
			0x030f,0x01,
			0x0311,0x00,
			0x0312,0x01,
			0x031e,0x00,
			0x3000,0x20,
			0x3002,0x00,
			0x3018,0x32,
			0x3019,0x0c,
			0x0311,0x01,
			0x3020,0x93,
			0x3021,0x03,
			0x3022,0x01,
			0x3031,0x0a,
			0x303f,0x0c,
			0x3305,0xf1,
			0x3307,0x04,
			0x3309,0x29,
			0x3500,0x00,
			0x3501,0x46,	//60,
			0x3502,0x80,	//00,
			0x3503,0x04,
			0x3504,0x00,
			0x3505,0x00,
			0x3506,0x00,
			0x3507,0x00,
			0x3508,0x0c,
			0x3509,0x80,
			0x350a,0x00,
			0x350b,0x00,
			0x350c,0x00,
			0x350d,0x00,
			0x350e,0x00,
			0x350f,0x80,
			0x3510,0x00,
			0x3511,0x00,
			0x3512,0x00,
			0x3513,0x00,
			0x3514,0x00,
			0x3515,0x80,
			0x3516,0x00,
			0x3517,0x00,
			0x3518,0x00,
			0x3519,0x00,
			0x351a,0x00,
			0x351b,0x80,
			0x351c,0x00,
			0x351d,0x00,
			0x351e,0x00,
			0x351f,0x00,
			0x3520,0x00,
			0x3521,0x80,
			0x3522,0x08,
			0x3524,0x08,
			0x3526,0x08,
			0x3528,0x08,
			0x352a,0x08,
			0x3602,0x00,
			0x3603,0x40,
			0x3604,0x02,
			0x3605,0x00,
			0x3606,0x00,
			0x3607,0x00,
			0x3609,0x12,
			0x360a,0x40,
			0x360c,0x08,
			0x360f,0xe5,
			0x3608,0x8f,
			0x3611,0x00,
			0x3613,0xf7,
			0x3616,0x58,
			0x3619,0x99,
			0x361b,0x60,
			0x361c,0x7a,
			0x361e,0x79,
			0x361f,0x02,
			0x3632,0x00,
			0x3633,0x10,
			0x3634,0x10,
			0x3635,0x10,
			0x3636,0x15,
			0x3646,0x86,
			0x364a,0x0b,
			0x3700,0x17,
			0x3701,0x22,
			0x3703,0x10,
			0x370a,0x37,
			0x3705,0x00,
			0x3706,0x63,
			0x3709,0x3c,
			0x370b,0x01,
			0x370c,0x30,
			0x3710,0x24,
			0x3711,0x0c,
			0x3716,0x00,
			0x3720,0x28,
			0x3729,0x7b,
			0x372a,0x84,
			0x372b,0xbd,
			0x372c,0xbc,
			0x372e,0x52,
			0x373c,0x0e,
			0x373e,0x33,
			0x3743,0x10,
			0x3744,0x88,
			0x3745,0xc0,
			0x374a,0x43,
			0x374c,0x00,
			0x374e,0x23,
			0x3751,0x7b,
			0x3752,0x84,
			0x3753,0xbd,
			0x3754,0xbc,
			0x3756,0x52,
			0x375c,0x00,
			0x3760,0x00,
			0x3761,0x00,
			0x3762,0x00,
			0x3763,0x00,
			0x3764,0x00,
			0x3767,0x04,
			0x3768,0x04,
			0x3769,0x08,
			0x376a,0x08,
			0x376b,0x20,
			0x376c,0x00,
			0x376d,0x00,
			0x376e,0x00,
			0x3773,0x00,
			0x3774,0x51,
			0x3776,0xbd,
			0x3777,0xbd,
			0x3781,0x18,
			0x3783,0x25,
			0x3798,0x1b,
			0x3800,0x00,
			0x3801,0x08,
			0x3802,0x00,
			0x3803,0x04,
			0x3804,0x0a,
			0x3805,0x97,
			0x3806,0x05,
			0x3807,0xfb,
			0x3808,0x0a,
			0x3809,0x80,
			0x380a,0x05,
			0x380b,0xf0,
			0x380c,0x0b,	//0x0d,
			0x380d,0xb2,	//0xbc,
			0x380e,0x05,	//0x04,
			0x380f,0x38,	//0xb8,	//0x70,	//68
			0x3810,0x00,
			0x3811,0x08,
			0x3812,0x00,
			0x3813,0x04,
			0x3814,0x01,
			0x3815,0x01,
			0x3819,0x01,
			0x3820,0x00,
			0x3821,0x06,
			0x3829,0x00,
			0x382a,0x01,
			0x382b,0x01,
			0x382d,0x7f,
			0x3830,0x04,
			0x3836,0x01,
			0x3837,0x00,
			0x3841,0x02,
			0x3846,0x08,
			0x3847,0x07,
			0x3d85,0x36,
			0x3d8c,0x71,
			0x3d8d,0xcb,
			0x3f0a,0x00,
			0x4000,0x71,		
			0x4001,0x40,
			0x4002,0x04,
			0x4003,0x14,
			0x400e,0x00,
			0x4011,0x00,
			0x401a,0x00,
			0x401b,0x00,
			0x401c,0x00,
			0x401d,0x00,
			0x401f,0x00,
			0x4020,0x00,
			0x4021,0x10,
			0x4022,0x07,
			0x4023,0xcf,
			0x4024,0x09,
			0x4025,0x60,
			0x4026,0x09,
			0x4027,0x6f,
			0x4028,0x00,
			0x4029,0x02,
			0x402a,0x06,
			0x402b,0x04,
			0x402c,0x02,
			0x402d,0x02,
			0x402e,0x0e,
			0x402f,0x04,
			0x4302,0xff,
			0x4303,0xff,
			0x4304,0x00,
			0x4305,0x00,
			0x4306,0x00,
			0x4308,0x02,
			0x4500,0x6c,
			0x4501,0xc4,
			0x4502,0x40,
			0x4503,0x01,
			0x4600,0x00,
			0x4601,0x77,	//0x04
			0x4800,0x04,
			0x4813,0x08,
			0x481f,0x40,
			0x4829,0x78,
			0x4837,0x10,
			0x4b00,0x2a,
			0x4b0d,0x00,
			0x4d00,0x04,
			0x4d01,0x42,
			0x4d02,0xd1,
			0x4d03,0x93,
			0x4d04,0xf5,
			0x4d05,0xc1,
			0x5000,0xf3,
			0x5001,0x11,
			0x5004,0x00,
			0x500a,0x00,
			0x500b,0x00,
			0x5032,0x00,
			0x5040,0x00,
			0x5050,0x0c,
			0x5500,0x00,
			0x5501,0x10,
			0x5502,0x01,
			0x5503,0x0f,
			0x8000,0x00,
			0x8001,0x00,
			0x8002,0x00,
			0x8003,0x00,
			0x8004,0x00,
			0x8005,0x00,
			0x8006,0x00,
			0x8007,0x00,
			0x8008,0x00,
			0x3638,0x00,
			0x3500,0x00,
			0x3508,0x04,
			0x0100,0x01,*/
			0x0103, 0x01,
    0x3638, 0x00,
    0x0300, 0x00,
    0x0302, 0x1e, // 0x2a
    0x0303, 0x01, // 0x00
    0x0304, 0x03,
    0x030b, 0x00,
    0x030d, 0x1e,
    0x030e, 0x04,
    0x030f, 0x01,
    0x0312, 0x01,
    0x031e, 0x00,
    0x3000, 0x20,
    0x3002, 0x00,
 
	    0x3018,0x32,// 0x72, // 0x32/0x72 // 2lane/4lane
	    0x3019, 0x0c,//0x00, // 0x0c/0x00 // 2lane/4lane
    0x3020, 0x93,
    0x3021, 0x03,
    0x3022, 0x01,
    0x3031, 0x0a,
    0x303f, 0x0c,
    0x3305, 0xf1,
    0x3307, 0x04,
    0x3309, 0x29,
    0x3500, 0x00,
    0x3501, 0x46, // 0x61
    0x3502, 0x80, // 0x00
    0x3503, 0x04,
    0x3504, 0x00,
    0x3505, 0x00,
    0x3506, 0x00,
    0x3507, 0x00,
    0x3508, 0x07, // 0x8X
    0x3509, 0x78, // 0x80
    0x350a, 0x00,
    0x350b, 0x00,
    0x350c, 0x00,
    0x350d, 0x00,
    0x350e, 0x00,
    0x350f, 0x80,
    0x3510, 0x00,
    0x3511, 0x00,
    0x3512, 0x00,
    0x3513, 0x00,
    0x3514, 0x00,
    0x3515, 0x80,
    0x3516, 0x00,
    0x3517, 0x00,
    0x3518, 0x00,
    0x3519, 0x00,
    0x351a, 0x00,
    0x351b, 0x80,
    0x351c, 0x00,
    0x351d, 0x00,
    0x351e, 0x00,
    0x351f, 0x00,
    0x3520, 0x00,
    0x3521, 0x80,
    0x3522, 0x08,
    0x3524, 0x08,
    0x3526, 0x08,
    0x3528, 0x08,
    0x352a, 0x08,
    0x3602, 0x00,
    0x3603, 0x40,
    0x3604, 0x02,
    0x3605, 0x00,
    0x3606, 0x00,
    0x3607, 0x00,
    0x3609, 0x12,
    0x360a, 0x40,
    0x360c, 0x08,
	//0x360f, 0xe5,
  	0x360f, 0xdb,
    0x3608, 0x8f,
    0x3611, 0x00,
    0x3613, 0xf7,
    0x3616, 0x58,
    0x3619, 0x99,
    0x361b, 0x60,
    0x361c, 0x7a,
    0x361e, 0x79,
    0x361f, 0x02,
    0x3632, 0x00,
    0x3633, 0x10,
    0x3634, 0x10,
	//0x3636, 0x15,
    0x3636, 0x10,
    0x3635, 0x10,
    0x3646, 0x86,
    0x364a, 0x0b,
    0x3700, 0x17,
    0x3701, 0x22,
    0x3703, 0x10,
    0x370a, 0x37,
    0x3705, 0x00,
    0x3706, 0x63,
    0x3709, 0x3c,
    0x370b, 0x01,
    0x370c, 0x30,
    0x3710, 0x24,
    0x3711, 0x0c,
    0x3716, 0x00,
    0x3720, 0x28,
    0x3729, 0x7b,
    0x372a, 0x84,
    0x372b, 0xbd,
    0x372c, 0xbc,
    0x372e, 0x52,
    0x373c, 0x0e,
    0x373e, 0x33,
    0x3743, 0x10,
    0x3744, 0x88,
    0x3745, 0xc0,
    0x374a, 0x43,
    0x374c, 0x00,
    0x374e, 0x23,
    0x3751, 0x7b,
    0x3752, 0x84,
    0x3753, 0xbd,
    0x3754, 0xbc,
    0x3756, 0x52,
    0x375c, 0x00,
    0x3760, 0x00,
    0x3761, 0x00,
    0x3762, 0x00,
    0x3763, 0x00,
    0x3764, 0x00,
    0x3767, 0x04,
    0x3768, 0x04,
    0x3769, 0x08,
    0x376a, 0x08,
    0x376b, 0x20,
    0x376c, 0x00,
    0x376d, 0x00,
    0x376e, 0x00,
    0x3773, 0x00,
    0x3774, 0x51,
    0x3776, 0xbd,
    0x3777, 0xbd,
    0x3781, 0x18,
    0x3783, 0x25,
    0x3798, 0x1b,
    0x3800, 0x01,
    0x3801, 0x84, // 0x24
    0x3802, 0x00,
    0x3803, 0xda, // 0xa6
    0x3804, 0x09,
    0x3805, 0x1b, // 0x7b
    0x3806, 0x05,
    0x3807, 0x25, // 0x59
    0x3808, 0x07, // 0x08
    0x3809, 0x88, // 0x48
    0x380a, 0x04,
    0x380b, 0x44, // 0xac
    0x380c, 0x0d, // 0x0a // 0x14 // 0x0a // 0x03
    0x380d, 0xbc, // 0x04 // 0xf6 // 0x30 // 0x18 // 0x5c
    0x380e, 0x04, // 0x06
    0x380f, 0x70, // 0x1c // 0x20 // 0x12
    0x3810, 0x00,
    0x3811, 0x08,
    0x3812, 0x00,
    0x3813, 0x04,
    0x3814, 0x01,
    0x3815, 0x01,
    0x3819, 0x01,
#if SENSOR_ROTATE_180
    0x3820, 0x06, // Tom: 0x00: No Flip, 0x06: Row & Column Flip
    0x3821, 0x00, // Tom: 0x06: No Flip, 0x00: Row & Column Flip
#else
    0x3820, 0x00,
    0x3821, 0x06,
#endif
    0x3829, 0x00,
    0x382a, 0x01,
    0x382b, 0x01,
    0x382d, 0x7f,
    0x3830, 0x04,
    0x3836, 0x01,
    0x3837, 0x00,
    0x3841, 0x02,
    0x3846, 0x08,
    0x3847, 0x07,
    0x3d85, 0x36,
    0x3d8c, 0x71,
    0x3d8d, 0xcb,
    0x3f0a, 0x00,
    0x4000, 0x71,
    0x4001, 0x40,
    0x4002, 0x04,
    0x4003, 0x14,
    0x400e, 0x00,
    0x4011, 0x00,
    0x401a, 0x00,
    0x401b, 0x00,
    0x401c, 0x00,
    0x401d, 0x00,
    0x401f, 0x00,
    0x4020, 0x00,
    0x4021, 0x10,
    0x4022, 0x06,
    0x4023, 0x1b, // 0x3f
    0x4024, 0x07,
    0x4025, 0x48, // 0x6c
    0x4026, 0x07,
    0x4027, 0x58, // 0x7b
    0x4028, 0x00,
    0x4029, 0x02,
    0x402a, 0x06,
    0x402b, 0x04,
    0x402c, 0x02,
    0x402d, 0x02,
    0x402e, 0x0e,
    0x402f, 0x04,
    0x4302, 0xff,
    0x4303, 0xff,
    0x4304, 0x00,
    0x4305, 0x00,
    0x4306, 0x00,
    0x4308, 0x02,
    0x4500, 0x6c,
    0x4501, 0xc4,
    0x4502, 0x40,
    0x4503, 0x01,
    0x4601, 0x77,
    0x4800, 0x04,
    0x4813, 0x08,
    0x481f, 0x40,
    0x4829, 0x78,
    0x4837, 0x16,//0x2c, // 0x18
    0x4b00, 0x2a,
    0x4b0d, 0x00,
    0x4d00, 0x04,
    0x4d01, 0x42,
    0x4d02, 0xd1,
    0x4d03, 0x93,
    0x4d04, 0xf5,
    0x4d05, 0xc1,
    0x5000, 0xf3,
    0x5001, 0x11,
    0x5004, 0x00,
    0x500a, 0x00,
    0x500b, 0x00,
    0x5032, 0x00,

    0x5040, 0x00,

    0x5050, 0x0c,
    0x5500, 0x00,
    0x5501, 0x10,
    0x5502, 0x01,
    0x5503, 0x0f,
    0x8000, 0x00,
    0x8001, 0x00,
    0x8002, 0x00,
    0x8003, 0x00,
    0x8004, 0x00,
    0x8005, 0x00,
    0x8006, 0x00,
    0x8007, 0x00,
    0x8008, 0x00,
    0x3638, 0x00,
    0x0100, 0x01
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

    RTNA_DBG_Str(0, "SNR_Cust_InitConfig OV4689\r\n");
    
    // Init OPR Table
    SensorCustFunc.OprTable->usInitSize                             = (sizeof(SNR_OV4689_Reg_Init_Customer)/sizeof(SNR_OV4689_Reg_Init_Customer[0]))/2;
    SensorCustFunc.OprTable->uspInitTable                           = &SNR_OV4689_Reg_Init_Customer[0];

//    SensorCustFunc.OprTable->bBinTableExist                         = MMP_FALSE;
  //  SensorCustFunc.OprTable->bInitDoneTableExist                    = MMP_FALSE;

    for (i = 0; i < MAX_SENSOR_RES_MODE; i++)
    {
        SensorCustFunc.OprTable->usSize[i]                          = (sizeof(SNR_OV4689_Reg_Unsupport)/sizeof(SNR_OV4689_Reg_Unsupport[0]))/2;
        SensorCustFunc.OprTable->uspTable[i]                        = &SNR_OV4689_Reg_Unsupport[0];
    }
    
    // 16:9
//    SensorCustFunc.OprTable->usSize[RES_IDX_2688x1520_30P]          = (sizeof(SNR_OV4689_Reg_2688x1520_30P)/sizeof(SNR_OV4689_Reg_2688x1520_30P[0]))/2;
//    SensorCustFunc.OprTable->uspTable[RES_IDX_2688x1520_30P]        = &SNR_OV4689_Reg_2688x1520_30P[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_1920x1080_30P]          = (sizeof(SNR_OV4689_Reg_1920x1080_30P)/sizeof(SNR_OV4689_Reg_1920x1080_30P[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1920x1080_30P]        = &SNR_OV4689_Reg_1920x1080_30P[0];

    // Init Vif Setting : Common
//    SensorCustFunc.VifSetting->SnrType                              = MMPF_VIF_SNR_TYPE_BAYER;
#if (SENSOR_IF == SENSOR_IF_PARALLEL)
    SensorCustFunc.VifSetting->OutInterface                         = MMPF_VIF_IF_PARALLEL;
#elif (SENSOR_IF == SENSOR_IF_MIPI_1_LANE)
    SensorCustFunc.VifSetting->OutInterface                         = MMPF_VIF_IF_MIPI_SINGLE_0;
#elif (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)
    #if (PRM_SENSOR_VIF_ID == MMPF_VIF_MDL_ID1) //MIPI 2 lane
    SensorCustFunc.VifSetting->OutInterface                         = MMPF_VIF_IF_MIPI_DUAL_12;
	
	#else
    SensorCustFunc.VifSetting->OutInterface                         = MMPF_VIF_IF_MIPI_DUAL_01;
    #endif
    
#elif (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)
    SensorCustFunc.VifSetting->OutInterface                         = MMPF_VIF_IF_MIPI_QUAD;
#endif

#if (PRM_SENSOR_VIF_ID == MMPF_VIF_MDL_ID1) //MIPI 2 lane

SensorCustFunc.VifSetting->VifPadId                             = MMPF_VIF_MDL_ID1;
#else
SensorCustFunc.VifSetting->VifPadId                             = MMPF_VIF_MDL_ID0;
#endif
    

    // Init Vif Setting : PowerOn Setting
    SensorCustFunc.VifSetting->powerOnSetting.bTurnOnExtPower       = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPin         = SENSOR_GPIO_ENABLE;
    SensorCustFunc.VifSetting->powerOnSetting.bExtPowerPinHigh      = SENSOR_GPIO_ENABLE_ACT_LEVEL;
    SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPinDelay    = 50;
    SensorCustFunc.VifSetting->powerOnSetting.bFirstEnPinHigh       = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOnSetting.ubFirstEnPinDelay     = 10;
    SensorCustFunc.VifSetting->powerOnSetting.bNextEnPinHigh        = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubNextEnPinDelay      = 100;
    SensorCustFunc.VifSetting->powerOnSetting.bTurnOnClockBeforeRst = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.bFirstRstPinHigh      = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOnSetting.ubFirstRstPinDelay    = 100;
    SensorCustFunc.VifSetting->powerOnSetting.bNextRstPinHigh       = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubNextRstPinDelay     = 100;

    // Init Vif Setting : PowerOff Setting
    SensorCustFunc.VifSetting->powerOffSetting.bEnterStandByMode    = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOffSetting.usStandByModeReg     = 0x0100;
    SensorCustFunc.VifSetting->powerOffSetting.usStandByModeMask    = 0x01;
    SensorCustFunc.VifSetting->powerOffSetting.bEnPinHigh           = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOffSetting.ubEnPinDelay         = 20;
    SensorCustFunc.VifSetting->powerOffSetting.bTurnOffMClock       = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOffSetting.bTurnOffExtPower     = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOffSetting.usExtPowerPin        = SENSOR_GPIO_ENABLE; // it might be defined in Config_SDK.h

    // Init Vif Setting : Sensor MClock Setting
    SensorCustFunc.VifSetting->clockAttr.bClkOutEn                  = MMP_TRUE;
    SensorCustFunc.VifSetting->clockAttr.ubClkFreqDiv               = 0;
    SensorCustFunc.VifSetting->clockAttr.ulMClkFreq                 = 24000;
    SensorCustFunc.VifSetting->clockAttr.ulDesiredFreq              = 24000;
    SensorCustFunc.VifSetting->clockAttr.ubClkPhase                 = MMPF_VIF_SNR_PHASE_DELAY_NONE;
    SensorCustFunc.VifSetting->clockAttr.ubClkPolarity              = MMPF_VIF_SNR_CLK_POLARITY_POS;
    SensorCustFunc.VifSetting->clockAttr.ubClkSrc                   = MMPF_VIF_SNR_CLK_SRC_PMCLK;

    // Init Vif Setting : Parallel Sensor Setting
    SensorCustFunc.VifSetting->paralAttr.ubLatchTiming              = MMPF_VIF_SNR_LATCH_POS_EDGE;
    SensorCustFunc.VifSetting->paralAttr.ubHsyncPolarity            = MMPF_VIF_SNR_CLK_POLARITY_POS;
    SensorCustFunc.VifSetting->paralAttr.ubVsyncPolarity            = MMPF_VIF_SNR_CLK_POLARITY_POS;
    SensorCustFunc.VifSetting->paralAttr.ubBusBitMode               = MMPF_VIF_SNR_PARAL_BITMODE_16;

    // Init Vif Setting : MIPI Sensor Setting
    SensorCustFunc.VifSetting->mipiAttr.bClkDelayEn                 = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bClkLaneSwapEn              = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.usClkDelay                  = 0;
    SensorCustFunc.VifSetting->mipiAttr.ubBClkLatchTiming           = MMPF_VIF_SNR_LATCH_NEG_EDGE;
#if (SENSOR_IF == SENSOR_IF_MIPI_1_LANE)
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[0]              = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[1]              = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[2]              = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[3]              = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[0]             = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[1]             = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[2]             = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[3]             = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[0]          = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[1]          = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[2]          = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[3]          = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[0]            = MMPF_VIF_MIPI_DATA_SRC_PHY_0;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[1]            = MMPF_VIF_MIPI_DATA_SRC_PHY_UNDEF;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[2]            = MMPF_VIF_MIPI_DATA_SRC_PHY_UNDEF;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[3]            = MMPF_VIF_MIPI_DATA_SRC_PHY_UNDEF;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[0]              = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[1]              = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[2]              = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[3]              = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[0]             = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[1]             = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[2]             = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[3]             = 0x1F;
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
#if (PRM_SENSOR_VIF_ID == MMPF_VIF_MDL_ID1) //MIPI 2 lane
	
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[0]            = MMPF_VIF_MIPI_DATA_SRC_PHY_1;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[1]            = MMPF_VIF_MIPI_DATA_SRC_PHY_2;
#else
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[0]            = MMPF_VIF_MIPI_DATA_SRC_PHY_0;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[1]            = MMPF_VIF_MIPI_DATA_SRC_PHY_1;
#endif
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[2]            = MMPF_VIF_MIPI_DATA_SRC_PHY_UNDEF;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[3]            = MMPF_VIF_MIPI_DATA_SRC_PHY_UNDEF;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[0]              = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[1]              = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[2]              = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[3]              = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[0]             = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[1]             = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[2]             = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[3]             = 0x1F;
#elif (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[0]              = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[1]              = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[2]              = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[3]              = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[0]             = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[1]             = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[2]             = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[3]             = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[0]          = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[1]          = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[2]          = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[3]          = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[0]            = MMPF_VIF_MIPI_DATA_SRC_PHY_0;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[1]            = MMPF_VIF_MIPI_DATA_SRC_PHY_1;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[2]            = MMPF_VIF_MIPI_DATA_SRC_PHY_2;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[3]            = MMPF_VIF_MIPI_DATA_SRC_PHY_3;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[0]              = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[1]              = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[2]              = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[3]              = 0x08;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[0]             = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[1]             = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[2]             = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[3]             = 0x1F;
#endif

    if (gsHdrCfg.bEnable && gsHdrCfg.ubMode == HDR_MODE_STAGGER)
    {
        SensorCustFunc.VifSetting->vcAttr.bEnable                   = MMP_TRUE;
        SensorCustFunc.VifSetting->vcAttr.bAllChannel2Isp           = MMP_TRUE;
        SensorCustFunc.VifSetting->vcAttr.bVC2Isp[0]                = MMP_TRUE;
        SensorCustFunc.VifSetting->vcAttr.bVC2Isp[1]                = MMP_TRUE;
        SensorCustFunc.VifSetting->vcAttr.bVC2Isp[2]                = MMP_FALSE;
        SensorCustFunc.VifSetting->vcAttr.bVC2Isp[3]                = MMP_FALSE;
        SensorCustFunc.VifSetting->vcAttr.bVC2Raw[0]                = MMP_FALSE;
        SensorCustFunc.VifSetting->vcAttr.bVC2Raw[1]                = MMP_FALSE;
        SensorCustFunc.VifSetting->vcAttr.bVC2Raw[2]                = MMP_FALSE;
        SensorCustFunc.VifSetting->vcAttr.bVC2Raw[3]                = MMP_FALSE;
        SensorCustFunc.VifSetting->vcAttr.bSlowFsForStagger         = MMP_TRUE;
    }
    else
    {
        SensorCustFunc.VifSetting->vcAttr.bEnable                   = MMP_FALSE;
        SensorCustFunc.VifSetting->vcAttr.bAllChannel2Isp           = MMP_FALSE;
        SensorCustFunc.VifSetting->vcAttr.bVC2Isp[0]                = MMP_FALSE;
        SensorCustFunc.VifSetting->vcAttr.bVC2Isp[1]                = MMP_FALSE;
        SensorCustFunc.VifSetting->vcAttr.bVC2Isp[2]                = MMP_FALSE;
        SensorCustFunc.VifSetting->vcAttr.bVC2Isp[3]                = MMP_FALSE;
        SensorCustFunc.VifSetting->vcAttr.bVC2Raw[0]                = MMP_FALSE;
        SensorCustFunc.VifSetting->vcAttr.bVC2Raw[1]                = MMP_FALSE;
        SensorCustFunc.VifSetting->vcAttr.bVC2Raw[2]                = MMP_FALSE;
        SensorCustFunc.VifSetting->vcAttr.bVC2Raw[3]                = MMP_FALSE;
        SensorCustFunc.VifSetting->vcAttr.bSlowFsForStagger         = MMP_FALSE;
    }

    // Init Vif Setting : Color ID Setting
    #if SENSOR_ROTATE_180 
    SensorCustFunc.VifSetting->colorId.VifColorId                   = MMPF_VIF_COLORID_00;
    #else
    SensorCustFunc.VifSetting->colorId.VifColorId                   = MMPF_VIF_COLORID_11;
    #endif
    
	SensorCustFunc.VifSetting->colorId.CustomColorId.bUseCustomId   = MMP_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_DoAE_FrmSt
//  Description :
//------------------------------------------------------------------------------
#define MAX_SENSOR_GAIN		(16)
#define ISP_DGAIN_BASE		(0x200)

ISP_UINT32 dgain;
//unsigned short a[10];

void SNR_Cust_DoAE_FrmSt(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
#if (ISP_EN)
	MMP_ULONG ulVsync = 0;
	MMP_ULONG ulShutter = 0;
	
    if (gsHdrCfg.bEnable && gsHdrCfg.ubMode == HDR_MODE_STAGGER)
    {

    #if 0 //Group Hold Test
    #else
    
        if ((ulFrameCnt % m_AeTime.ubPeriod) == 0)
        {
            ISP_IF_AE_Execute();

            // Get long exposure
            ae_shutter_l = ISP_IF_AE_GetShutter();
            ae_gain_l    = ISP_IF_AE_GetGain();        

            ae_shutter_s = ISP_IF_IQ_GetHDR_ShortShutter();
            ae_gain_s    = ISP_IF_IQ_GetHDR_ShortGain();

	        if (ae_gain_l >= ISP_IF_AE_GetGainBase() * MAX_SENSOR_GAIN) {
               dgain      = ISP_DGAIN_BASE * ae_gain_l / ISP_IF_AE_GetGainBase() / MAX_SENSOR_GAIN ;
               ae_gain_l  = ISP_IF_AE_GetGainBase() * MAX_SENSOR_GAIN;
            }
            else {
            	dgain = ISP_DGAIN_BASE;
            }

            ae_gain_s = ae_gain_s * ISP_DGAIN_BASE / dgain;
            
            // Sensor limit
            if (ae_shutter_l <= ae_shutter_s)   ae_shutter_l = ae_shutter_s + 1;

            // Hdr on/off, use different iq setting
            ISP_IF_IQ_SetSysMode(0);
            ISP_IF_AE_SetSysMode(2);  //2 for hdr mode
        }
        
        // For AE curve
        if (ulFrameCnt % 100 == 10) {
    		#ifdef HB_IQ_VER    // temp, For DVR002
            if ((HB_IQ_VER == 4) || (HB_IQ_VER == 5))
                ISP_IF_CMD_SendCommandtoAE(0x52, AE_Bias_tbl, AE_tbl_size, 1);
            else
                ISP_IF_CMD_SendCommandtoAE(0x51, AE_Bias_tbl, AE_tbl_size, 0);
    		#else
            ISP_IF_CMD_SendCommandtoAE(0x51, AE_Bias_tbl, AE_tbl_size, 0); // <<AE table set once at preview start
    		#endif
            ISP_IF_NaturalAE_Enable(2);	//0: no , 1:ENERGY 2: Lux 3: test mode
        }

		//gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3208, 0x0);  //group 0 start
        
        // Set shutter
        if ((ulFrameCnt % m_AeTime.ubPeriod) == m_AeTime.ubFrmStSetShutFrmCnt) {
            gsSensorFunction->MMPF_Sensor_SetShutter(ubSnrSel, 0, 0);    
        }
        
        // Set gain
		if ((ulFrameCnt % m_AeTime.ubPeriod) == m_AeTime.ubFrmStSetGainFrmCnt) {
            gsSensorFunction->MMPF_Sensor_SetGain(ubSnrSel, 0);
       	}

       	ISP_IF_IQ_SetAEGain(dgain, ISP_DGAIN_BASE);

		//gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3208, 0x10);//group 0 end
		//gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3208, 0xE0);//group 0 quick launch
    #endif
    }
    else
    {
        static ISP_UINT16 ae_gain;
        static ISP_UINT16 ae_shutter;
        static ISP_UINT16 ae_vsync;

        #if 1// For AE curve
        if (ulFrameCnt % 100 == 10) {
	        ISP_IF_F_SetWDREn(1);
            ISP_IF_CMD_SendCommandtoAE(0x51,AE_Bias_tbl,AE_tbl_size,0);         // <<AE table set once at preview start
            ISP_IF_NaturalAE_Enable(2);	//0: no , 1:ENERGY 2: Lux 3: test mode
        }
        #endif
        
        if ((ulFrameCnt % m_AeTime.ubPeriod) == 0)
        {
            ISP_IF_AE_Execute();
			ISP_IF_IQ_SetSysMode(1);
            ISP_IF_AE_SetSysMode(0);
                
            ae_gain     = ISP_IF_AE_GetGain();
            ae_shutter  = ISP_IF_AE_GetShutter();
            ae_vsync    = ISP_IF_AE_GetVsync();
        }

		ulVsync 	= (gSnrLineCntPerSec[ubSnrSel] * ISP_IF_AE_GetVsync()) / ISP_IF_AE_GetVsyncBase();
		ulShutter 	= (gSnrLineCntPerSec[ubSnrSel] * ISP_IF_AE_GetShutter()) / ISP_IF_AE_GetShutterBase();

        if ((ulFrameCnt % m_AeTime.ubPeriod) == m_AeTime.ubFrmStSetShutFrmCnt) {
            gsSensorFunction->MMPF_Sensor_SetShutter(ubSnrSel, ulShutter, ulVsync);
        }
        
        if ((ulFrameCnt % m_AeTime.ubPeriod) == m_AeTime.ubFrmStSetGainFrmCnt)
       	{
	        if (ae_gain >= ISP_IF_AE_GetGainBase() * MAX_SENSOR_GAIN) {

               	dgain  	= ISP_DGAIN_BASE*ae_gain /ISP_IF_AE_GetGainBase() / MAX_SENSOR_GAIN ;
              	ae_gain	= ISP_IF_AE_GetGainBase() * MAX_SENSOR_GAIN;
	        }
	        else {
	    		dgain 	= ISP_DGAIN_BASE;
	        }

            gsSensorFunction->MMPF_Sensor_SetGain(ubSnrSel, ae_gain);

        	//ISP_IF_IQ_SetAEGain(dgain, ISP_DGAIN_BASE);
        }

	    if ((ulFrameCnt % m_AeTime.ubPeriod) == m_AeTime.ubFrmStSetGainFrmCnt+1)
	    {
	        ISP_IF_IQ_SetAEGain(dgain, ISP_DGAIN_BASE);
	    }
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
void SNR_Cust_SetGain(MMP_UBYTE ubSnrSel, MMP_ULONG ulGain)
{
#if (ISP_EN)

    if (gsHdrCfg.bEnable && gsHdrCfg.ubMode == HDR_MODE_STAGGER)
    {
        ISP_UINT16  sensor_H;
        ISP_UINT16  sensor_L;
        MMP_ULONG   ulTempGain;

        ulTempGain = ae_gain_l;

        /* Sensor Gain Mapping */
        if ((ulTempGain >> 8) < 2)
        {
            sensor_H = 0;
            sensor_L = (ulTempGain * 128) >> 8;                 // 1X ~ 2X
        }
        else if ((ulTempGain >> 8) < 4)
        {
            sensor_H = 1;   
            sensor_L = ((ulTempGain * 64) - (256 * 8)) >> 8;    // 2X ~ 4X
        }       
        else if ((ulTempGain >> 8) < 8)
        {
            sensor_H = 3;   
            sensor_L = ((ulTempGain * 32) - (256 * 12)) >> 8;   // 4X ~ 8X
        }   
        else
        {
            sensor_H = 7;
            sensor_L = ((ulTempGain * 16) - (256 * 8)) >> 8;    // 8X ~16X
        }

        // Long Gain
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3508, sensor_H);
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3509, sensor_L);

        ulTempGain = ae_gain_s;

        /* Sensor Gain Mapping */
        if ((ulTempGain >> 8) < 2)
        {
            sensor_H = 0;
            sensor_L = (ulTempGain * 128) >> 8;                 // 1X ~ 2X
        }
        else if ((ulTempGain >> 8) < 4)
        {
            sensor_H = 1;   
            sensor_L = ((ulTempGain * 64) - (256 * 8)) >> 8;    // 2X ~ 4X
        }       
        else if ((ulTempGain >> 8) < 8)
        {
            sensor_H = 3;   
            sensor_L = ((ulTempGain * 32) - (256 * 12)) >> 8;   // 4X ~ 8X
        }   
        else
        {
            sensor_H = 7;
            sensor_L = ((ulTempGain * 16) - (256 * 8)) >> 8;    // 8X ~16X
        }
        
        // Middle Gain
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x350E, sensor_H);
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x350F, sensor_L);
    }
    else
    {
        ISP_UINT16 sensor_H;
        ISP_UINT16 sensor_L;

        /* Sensor Gain Mapping */
        if ((ulGain>>8) < 2)
        {
            sensor_H = 0;
            sensor_L = (ulGain * 128) >> 8;                 // 1X ~ 2X
        }
        else if ((ulGain >> 8) < 4)
        {
            sensor_H = 1;   
            sensor_L = ((ulGain * 64) - (256 * 8)) >> 8;    // 2X ~ 4X
        }       
        else if ((ulGain >> 8) < 8)
        {
            sensor_H = 3;   
            sensor_L = ((ulGain * 32) - (256 * 12)) >> 8;   // 4X ~ 8X
        }   
        else
        {
            sensor_H = 7;
            sensor_L = ((ulGain * 16) - (256 * 8)) >> 8;    // 8X ~16X
        }

        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3508, sensor_H);
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3509, sensor_L);
    }
#endif
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetShutter
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetShutter(MMP_UBYTE ubSnrSel, MMP_ULONG shutter, MMP_ULONG vsync)
{
#if (ISP_EN)
    
    if (gsHdrCfg.bEnable && gsHdrCfg.ubMode == HDR_MODE_STAGGER)
    {
        //Long Expo
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3500, (ISP_UINT8)((ae_shutter_l >> 12) & 0xFF));
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3501, (ISP_UINT8)((ae_shutter_l >> 4) & 0xFF));
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3502, (ISP_UINT8)((ae_shutter_l << 4) & 0xFF));
        
        //Middle Expo
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x350A, (ISP_UINT8)((ae_shutter_s >> 12) & 0xFF));
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x350B, (ISP_UINT8)((ae_shutter_s >> 4) & 0xFF));
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x350C, (ISP_UINT8)((ae_shutter_s << 4) & 0xFF));
    }
    else
    {
        ISP_UINT32 new_vsync;
        ISP_UINT32 new_shutter;

        new_vsync   = ISP_MIN(ISP_MAX((shutter + 4), vsync), 0xFFFF);
        new_shutter = ISP_MIN(ISP_MAX(shutter, 1), (vsync - 4));

        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x380E, (new_vsync >> 8));
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x380F, new_vsync);

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
    MMP_USHORT  usRdVal1, usRdVal2;

    #define SENSOR_FLIP_MASK_BITS (0x06)
	
    gsSensorFunction->MMPF_Sensor_GetReg(ubSnrSel, 0x3820, &usRdVal1);
    gsSensorFunction->MMPF_Sensor_GetReg(ubSnrSel, 0x3821, &usRdVal2);
    
	if (ubMode == MMPF_SENSOR_NO_FLIP) 
	{
		usRdVal1 &= ~SENSOR_FLIP_MASK_BITS;
		usRdVal2 |= SENSOR_FLIP_MASK_BITS;
	}	
	else if (ubMode == MMPF_SENSOR_COLROW_FLIP)
	{
		usRdVal1 |= SENSOR_FLIP_MASK_BITS;
		usRdVal2 &= ~SENSOR_FLIP_MASK_BITS;
	}
	else {
		return;
	}

	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3820, usRdVal1);
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3821, usRdVal2);
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

#pragma arm section code = "initcall6", rodata = "initcall6", rwdata = "initcall6",  zidata = "initcall6" 
#pragma O0
ait_module_init(SNR_Module_Init);
#pragma
#pragma arm section rodata, rwdata, zidata

#endif // (BIND_SENSOR_OV4689_2LINE)
#endif // (SENSOR_EN)

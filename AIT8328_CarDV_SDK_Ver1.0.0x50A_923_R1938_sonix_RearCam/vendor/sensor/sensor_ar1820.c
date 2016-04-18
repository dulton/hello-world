//==============================================================================
//
//  File        : sensor_ar1820.c
//  Description : Firmware Sensor Control File
//  Author      : Philip Lin
//  Revision    : 1.0
//
//=============================================================================

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "includes_fw.h"
#include "customer_config.h"

#if (SENSOR_EN)
#if (BIND_SENSOR_AR1820)

#include "mmpf_sensor.h"
#include "Sensor_Mod_Remapping.h"
#include "isp_if.h"
#include "mmpf_i2cm.h"

//==============================================================================
//
//                              GLOBAL VARIABLE
//
//==============================================================================

#if (OPTIMIZE_1080_30P)
//Modiify Mode 15
MMPF_SENSOR_RESOLUTION m_SensorRes = 
{
	27,				/* ubSensorModeNum */
	0,				/* ubDefPreviewMode */
	0,				/* ubDefCaptureMode */
	3000,           /* usPixelSize (TBD) */
//  Mode0   Mode1   Mode2   Mode3   Mode4	Mode5	Mode6 	Mode7   Mode8	Mode9	Mode10 	Mode11 	Mode12	Mode13	Mode14	Mode15	Mode16	Mode17	Mode18	Mode19	Mode20	Mode21	Mode22	Mode23	Mode24	Mode25	Mode26
    {1,     1,      1,      1,      1,      1,      1,		1,		1,		1,		1,		1,      1,      1,      1,		1,		1,		1,		1,		1,      1,      1,      1,      1,      1,		1,		1	},	// usVifGrabStX
    {1,     1,      1,      1,      1,      1,      1,		1,		1,		1,		1,		1,      1,      1,      1,		1,		1,		1,		1,		1,      1,      1,      1,      1,      1,		1,		1	},	// usVifGrabStY
    {2568,  2568,   2568,   1288,   1288,   2728,	3848,	3688,	3688,	3688,	1848,	1928,   3848,   3848,   3848,	3848,	1928,	1928,	1928,	3688,   3848,  	3848,   1928,   1928,   1928,	3848,	3848},	// usVifGrabW
    {1096,  1096,   1096,   728,    728,    2010,   2168,	2776,	2088,	2776,	1384,	2056,   2168,   1096,   1096,	2168,	1088,	1088,	1096,	1384,   2168,   1096,   1096,   1096,   1096,	2168,	2168},	// usVifGrabH
#if (CHIP == MCR_V2)
    {1,     1,      1,      1,      1,      1,      1,		1,		1,		1,		1,		1,   	1,      1,      1,		1,		1,		1,		1,		1,      1,      1,      1,      1,      1,		1,		1	},	// usBayerInGrabX
    {1,     1,      1,      1,      1,      1,      1,		1,		1,		1,		1,		1,   	1,      1,      1,		1,		2,		2,		1,		1,      1,      1,      2,      2,      2,		1,		1	},	// usBayerInGrabY
    {8,     8,      8,      8,      8,      8,      8,		8,		8,		8,		8,		8,    	8,      8,      8,		8,		8,		8,		8,		8,      8,      8,      8,      8,      8,		8,		8	},	// usBayerInDummyX
    {16,    16,     16,     8,      8,      74,     8,		16,		18,		8,		4,		8,   	8,      16,     16,		8,		8,		8,		16,		4,      8,      16,     16,     16,     16,		8,		8	},	// usBayerInDummyY
    {2560,  2560,   2560,   1280,   1280,   2720,   3840,	3680,	3680,	3680,	1840,	1920, 	2720,   1920,   1920,	2720,	1280,	1280,	1280,	1840,   1920,   1920,   1280,   1280,   1280,	3840,	2720},	// usBayerOutW
    {1080,  1080,   1080,   720,    720,    1936,   2160,	2760,	2070,	2768,	1380,	2048, 	1536,   1080,  	1080,	1530,	720,	720,	720,	1380,   1080,   1080,   720,    720,    720,	2160,	1530},	// usBayerOutH
#endif 
    {1920,  2560,   2560,   1280,   1280,   2720,   3840,	3680,	3680,	3680,	1840,	1920, 	2720,   1920,  	1920,	2720,	1280,	1280,	1280,	1840,   1920,   1920,   1280,   1280,  	1280,	3840,	2720},	// usScalInputW
    {1080,  1080,   1080,   720,    720,    1936,	2160,	2760,	2070,	2768,	1380,	2048, 	1536,   1080,   1080,	1530,	720,	720,	720,	1380,   1080,   1080,   720,    720,    720,	2160,	1530},	// usScalInputH
    {300,   500,    600,    1000,   1200,   300,    150,	150,	150,	150,	300,	300,	300,	600,	500,	300,	1200,	1000,	600,	300,    300,    250,    500,    300,    250,	125,	250	},	// usTargetFpsx10
    {1672,  1401,   1168,   943,    786,    2078,   2318,	2934,	2934,	4740,	1708,	2118,  	2234,   1154,   1384,	2234,	1126,	1351,	2288,	1467,   2234,   2769,   2745,   4576,   5491,	2781, 	2680},	// usVsyncLine
    {1,     1,      1,      1,      1,      1,      1,		1,		1,		1,		1,		1,   	1,      1,      1,		1,		1,		1,		1,		1,      1,      1,      1,      1,      1,		1,		1	},	// ubWBinningN
    {1,     1,      1,      1,      1,      1,      1,		1,		1,		1,		1,		1,   	1,      1,      1,		1,		2,		2,		1,		1,      1,      1,      2,      2,      2,		1,		1	},	// ubWBinningM
    {1,     1,      1,      1,      1,      1,      1,		1,		1,		1,		1,		1, 		1,      1,      1,		1,		1,		1,		1,		1,      1,      1,      1,      1,      1,		1,		1	},	// ubHBinningN
    {1,     1,      1,      1,      1,      1,      1,		1,		1,		1,		1,		1,    	1,      2,      1,		1,		2,		2,		1,		2,      1,      2,      2,      2,      2,		1, 		1	},	// ubHBinningM
    {0xFF,	0xFF,	0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF    0xFF,   0xFF},  // ubCustIQmode
    {0xFF,	0xFF,	0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF    0xFF,   0xFF}   // ubCustAEmode
};
#else
MMPF_SENSOR_RESOLUTION m_SensorRes = 
{
	27,				/* ubSensorModeNum */
	0,				/* ubDefPreviewMode */
	0,				/* ubDefCaptureMode */
	3000,           /* usPixelSize (TBD) */
//  Mode0   Mode1   Mode2   Mode3   Mode4	Mode5	Mode6 	Mode7   Mode8	Mode9	Mode10 	Mode11 	Mode12	Mode13	Mode14	Mode15	Mode16	Mode17	Mode18	Mode19	Mode20	Mode21	Mode22	Mode23	Mode24	Mode25	Mode26
    {1,     1,      1,      1,      1,      1,      1,		1,		1,		1,		1,		1,      1,      1,      1,		1,		1,		1,		1,		1,      1,      1,      1,      1,      1,		1,		1	},	// usVifGrabStX
    {1,     1,      1,      1,      1,      1,      1,		1,		1,		1,		1,		1,      1,      1,      1,		1,		1,		1,		1,		1,      1,      1,      1,      1,      1,		1,		1	},	// usVifGrabStY
    {2568,  2568,   2568,   1288,   1288,   2728,	3848,	3688,	3688,	3688,	1848,	1928,   3848,   3848,   3848,	3848,	1928,	1928,	1928,	3688,   3848,  	3848,   1928,   1928,   1928,	3848,	3848},	// usVifGrabW
    {1096,  1096,   1096,   728,    728,    2010,   2168,	2776,	2088,	2776,	1384,	2056,   2168,   1096,   1096,	1096,	1088,	1088,	1096,	1384,   2168,   1096,   1096,   1096,   1096,	2168,	2168},	// usVifGrabH
#if (CHIP == MCR_V2)
    {1,     1,      1,      1,      1,      1,      1,		1,		1,		1,		1,		1,   	1,      1,      1,		1,		2,		2,		1,		1,      1,      1,      2,      2,      2,		1,		1	},	// usBayerInGrabX
    {1,     1,      1,      1,      1,      1,      1,		1,		1,		1,		1,		1, 		1,      1,      1,		1,		1,		1,		1,		1,      1,      1,      1,      1,      1,		1,		1	},	// usBayerInGrabY
    {8,     8,      8,      8,      8,      8,      8,		8,		8,		8,		8,		8,    	8,      8,      8,		8,		8,		8,		8,		8,      8,      8,      8,      8,      8,		8,		8	},	// usBayerInDummyX
    {16,    16,     16,     8,      8,      74,     8,		16,		18,		8,		4,		8,   	8,      16,     16,		16,		8,		8,		16,		4,      8,      16,     16,     16,     16,		8,		8	},	// usBayerInDummyY  
    {2560,  2560,   2560,   1280,   1280,   2720,   3840,	3680,	3680,	3680,	1840,	1920, 	2720,   1920,   1920,	1920,	1280,	1280,	1280,	1840,   1920,   1920,   1280,   1280,   1280,	3840,	2720},	// usBayerOutW
    {1080,  1080,   1080,   720,    720,    1936,   2160,	2760,	2070,	2768,	1380,	2048, 	1536,   1080,  	1080,	1080,	720,	720,	720,	1380,   1080,   1080,   720,    720,    720,	2160,	1530},	// usBayerOutH
#endif 
    {1920,  2560,   2560,   1280,   1280,   2720,   3840,	3680,	3680,	3680,	1840,	1920, 	2720,   1920,  	1920,	1920,	1280,	1280,	1280,	1840,   1920,   1920,   1280,   1280,  	1280,	3840,	2720},	// usScalInputW
    {1080,  1080,   1080,   720,    720,    1936,	2160,	2760,	2070,	2768,	1380,	2048, 	1536,   1080,   1080,	1080,	720,	720,	720,	1380,   1080,   1080,   720,    720,    720,	2160,	1530},	// usScalInputH
    {300,   500,    600,    1000,   1200,   300,    150,	150,	150,	150,	300,	300,	300,	600,	500,	300,	1200,	1000,	600,	300,    300,    250,    500,    300,    250,	125,	250	},	// usTargetFpsx10
    {1672,  1401,   1168,   943,    786,    2078,   2318,	2934,	2934,	4740,	1708,	2118,  	2234,   1154,   1384,	2308,	1126,	1351,	2288,	1467,   2234,   2769,   2745,   4576,   5491,	2781, 	2680},	// usVsyncLine
    {1,     1,      1,      1,      1,      1,      1,		1,		1,		1,		1,		1,   	1,      1,      1,		1,		1,		1,		1,		1,      1,      1,      1,      1,      1,		1,		1	},	// ubWBinningN
    {1,     1,      1,      1,      1,      1,      1,		1,		1,		1,		1,		1,   	1,      1,      1,		1,		2,		2,		1,		1,      1,      1,      2,      2,      2,		1,		1	},	// ubWBinningM
    {1,     1,      1,      1,      1,      1,      1,		1,		1,		1,		1,		1, 		1,      1,      1,		1,		1,		1,		1,		1,      1,      1,      1,      1,      1,		1,		1	},	// ubHBinningN
    {1,     1,      1,      1,      1,      1,      1,		1,		1,		1,		1,		1,    	1,      2,      1,		2,		2,		2,		1,		2,      1,      2,      2,      2,      2,		1, 		1	},	// ubHBinningM
    {0xFF,	0xFF,	0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF    0xFF,   0xFF},  // ubCustIQmode
    {0xFF,	0xFF,	0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF    0xFF,   0xFF}   // ubCustAEmode
};
#endif

// OPR Table and Vif Setting
MMPF_SENSOR_OPR_TABLE       m_OprTable;
MMPF_SENSOR_VIF_SETTING     m_VifSetting;

// IQ Table
const ISP_UINT8 Sensor_IQ_CompressedText[] = 
{
#include "isp_8428_iq_data_v2_AR1820_v1.xls.ciq.txt"
};

// I2cm Attribute
static MMP_I2CM_ATTR m_I2cmAttr = 
{
    MMP_I2CM0,  // i2cmID
    0x36,       // ubSlaveAddr
    16,         // ubRegLen
    16,         // ubDataLen
    0,          // ubDelayTime
    MMP_FALSE,  // bDelayWaitEn
    MMP_TRUE,   // bInputFilterEn
    MMP_FALSE,  // b10BitModeEn
    MMP_FALSE,  // bClkStretchEn
    0,          // ubSlaveAddr1
    0,          // ubDelayCycle
    0,          // ubPadNum
    250000,     // ulI2cmSpeed 250KHZ
    MMP_TRUE,   // bOsProtectEn
    NULL,       // sw_clk_pin
    NULL,       // sw_data_pin
    MMP_FALSE,  // bRfclModeEn
    MMP_FALSE,  // bWfclModeEn
    MMP_FALSE,	// bRepeatModeEn
    0           // ubVifPioMdlId
};

// 3A Timing
MMPF_SENSOR_AWBTIMIMG   m_AwbTime    = 
{
	6,	/* ubPeriod */
	1, 	/* ubDoAWBFrmCnt */
	3	/* ubDoCaliFrmCnt */
};

MMPF_SENSOR_AETIMIMG    m_AeTime     = 
{	
	4, 	/* ubPeriod */
	0, 	/* ubFrmStSetShutFrmCnt */
	0	/* ubFrmStSetGainFrmCnt */
};

MMPF_SENSOR_AFTIMIMG    m_AfTime     = 
{
	1, 	/* ubPeriod */
	0	/* ubDoAFFrmCnt */
};

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

#if 0
void ____Sensor_Init_OPR_Table____(){ruturn;} //dummy
#endif

MMP_USHORT SNR_AR1820_Reg_Init_Customer[] = 
{
	//[Initialize for Camera] 
	//XMCLK=24000000                                          
	0x301A, 0x0019,     
	//SNR_REG_DELAY, DELAY_TIME,                                 
	//0x301A, 0x0018,                                      
	SNR_REG_DELAY, 2,                                              
	//[Analog Optimization setting with sequencer_SVN8166]                       
	0x3042, 0x0004,           
	0x30C0, 0xA000,           
	0x30D4, 0x3000,           
	0x30D6, 0x6800,           
	0x30DA, 0x0060,           
	0x30DC, 0x0080,           
	0x30EE, 0x3138,           
	0x316A, 0x0000,           
	0x316C, 0x0400,           
	0x316E, 0x0400,           
	0x3170, 0x236E,           
	0x3172, 0x0286,           
	0x3174, 0x0000,           
	0x3176, 0x9000,           
	0x3178, 0x0000,           
	0x3180, 0xF800,           
	0x317A, 0x416E,           
	0x317C, 0xE104,           
	0x3ED0, 0x00FF,           
	0x3ED4, 0x50C8,           
	0x3EDE, 0x00E0,           
	0x3EE0, 0x3558,           
	0x3EE2, 0x8A3C,           
	0x3EE4, 0x2B79,           
	0x3EE6, 0xF87A,           
	0x3EEA, 0x6400,           
	0x3EEC, 0x0400,           
	0x3EEE, 0x0000,           
	0x3EF0, 0x043F,           
	0x3EFA, 0xA484,           
	0x3EFC, 0x2020,           
	0x3EFE, 0x9096,           
	0x3F20, 0x0249,           
	0x3F26, 0x4428,           
	0x3F2C, 0x111D,           
	0x3F32, 0x1104,           
	0x3F2E, 0x0023,  
	0x3F38, 0x1328,           
	0x3ECE, 0xA8A6,              

	0x3180, 0xBFFF,  
	0x3F38, 0x1328,  
	0x3EE8, 0x1100,  
	0x3ED6, 0x786B,  
	0x3F3C, 0x0001,  

	//[Dynamic Col Correction Default change]
	0x30D2, 0x000C, 
	0x31E0, 0x0741, 
	0x31E6, 0x1000, 
	0x3F04, 0x0002, 
	0x3F06, 0x0004, 
	0x3F08, 0x0008, 
	0x3F10, 0x0303, 
	0x3F12, 0x0303, 
	0x3F14, 0x0101, 
	0x3F16, 0x0204, 
	0x3F1C, 0x0080, 

	//
	//Fix Issue: some sensor has back Image. 
	#if 1
	0x31B0, 0x006A,
	0x31B2, 0x0046,
	0x31B4, 0x130E,
	0x31B6, 0x1308,
	0x31B8, 0x1C12,
	0x31BA, 0x185A,
	0x31BC, 0x0509,
	#endif

	//LOAD = Sequencer_SVN8166_2Bytes 
	0x3D00, 0x0465,
	0x3D02, 0xB8FF,
	0x3D04, 0xFFFF,
	0x3D06, 0xFFFF,
	0x3D08, 0x3804,
	0x3D0A, 0x0005,
	0x3D0C, 0x170E,
	0x3D0E, 0x2180,
	0x3D10, 0x4180,
	0x3D12, 0x5C00,
	0x3D14, 0xC081,
	0x3D16, 0x5D80,
	0x3D18, 0x5A80,
	0x3D1A, 0x5780,
	0x3D1C, 0x200B,
	0x3D1E, 0x8150,
	0x3D20, 0x3301,
	0x3D22, 0x8500,
	0x3D24, 0xC082,
	0x3D26, 0x6064,
	0x3D28, 0x4281,
	0x3D2A, 0x658A,
	0x3D2C, 0x6580,
	0x3D2E, 0x6E87,
	0x3D30, 0x6E84,
	0x3D32, 0x5F9F,
	0x3D34, 0x6897,
	0x3D36, 0x5888,
	0x3D38, 0x5994,
	0x3D3A, 0x5B80,
	0x3D3C, 0x5B89,
	0x3D3E, 0x5881,
	0x3D40, 0x5A81,
	0x3D42, 0x5980,
	0x3D44, 0x5980,
	0x3D46, 0x5AD6,
	0x3D48, 0x5980,
	0x3D4A, 0x59F0,
	0x3D4C, 0x5081,
	0x3D4E, 0x6480,
	0x3D50, 0x6480,
	0x3D52, 0x6AAE,
	0x3D54, 0x4D85,
	0x3D56, 0x7A83,
	0x3D58, 0x6A82,
	0x3D5A, 0x6B80,
	0x3D5C, 0x5845,
	0x3D5E, 0x0600,
	0x3D60, 0x439F,
	0x3D62, 0x0028,
	0x3D64, 0xA206,
	0x3D66, 0x0058,
	0x3D68, 0x7B6B,
	0x3D6A, 0x6852,
	0x3D6C, 0x67A8,
	0x3D6E, 0x6764,
	0x3D70, 0x6465,
	0x3D72, 0x65F7,
	0x3D74, 0x5283,
	0x3D76, 0x5E80,
	0x3D78, 0x6964,
	0x3D7A, 0x8064,
	0x3D7C, 0x826A,
	0x3D7E, 0x967A,
	0x3D80, 0xA26A,
	0x3D82, 0x8058,
	0x3D84, 0x806B,
	0x3D86, 0x8241,
	0x3D88, 0x8140,
	0x3D8A, 0x8140,
	0x3D8C, 0x8041,
	0x3D8E, 0x8042,
	0x3D90, 0x827B,
	0x3D92, 0x8106,
	0x3D94, 0xC817,
	0x3D96, 0x0080,
	0x3D98, 0x200C,
	0x3D9A, 0x8018,
	0x3D9C, 0x0C83,
	0x3D9E, 0x448E,
	0x3DA0, 0x6B20,
	0x3DA2, 0x3069,
	0x3DA4, 0x803B,
	0x3DA6, 0x0480,
	0x3DA8, 0x4481,
	0x3DAA, 0x06C8,
	0x3DAC, 0x8042,
	0x3DAE, 0x4182,
	0x3DB0, 0x4080,
	0x3DB2, 0x4080,
	0x3DB4, 0x4180,
	0x3DB6, 0x4281,
	0x3DB8, 0x5E82,
	0x3DBA, 0x06C8,
	0x3DBC, 0x8844,
	0x3DBE, 0x9444,
	0x3DC0, 0x8106,
	0x3DC2, 0xC880,
	0x3DC4, 0x4241,
	0x3DC6, 0x8240,
	0x3DC8, 0x7000,
	0x3DCA, 0x3804,
	0x3DCC, 0x0005,
	0x3DCE, 0x170E,
	0x3DD0, 0x2380,
	0x3DD2, 0x4180,
	0x3DD4, 0x5C00,
	0x3DD6, 0xC081,
	0x3DD8, 0x5D80,
	0x3DDA, 0x5A80,
	0x3DDC, 0x5780,
	0x3DDE, 0x200B,
	0x3DE0, 0x00C0,
	0x3DE2, 0x8050,
	0x3DE4, 0x3301,
	0x3DE6, 0x8142,
	0x3DE8, 0x8361,
	0x3DEA, 0x8160,
	0x3DEC, 0x6482,
	0x3DEE, 0x658A,
	0x3DF0, 0x6580,
	0x3DF2, 0x6E82,
	0x3DF4, 0x5F81,
	0x3DF6, 0x5580,
	0x3DF8, 0x6E86,
	0x3DFA, 0x5590,
	0x3DFC, 0x598B,
	0x3DFE, 0x6887,
	0x3E00, 0x5B80,
	0x3E02, 0x5B8C,
	0x3E04, 0x5A85,
	0x3E06, 0x5AA2,
	0x3E08, 0x5584,
	0x3E0A, 0x55B0,
	0x3E0C, 0x5581,
	0x3E0E, 0x558A,
	0x3E10, 0x5580,
	0x3E12, 0x55D9,
	0x3E14, 0x5081,
	0x3E16, 0x6480,
	0x3E18, 0x6480,
	0x3E1A, 0x6AAE,
	0x3E1C, 0x4D93,
	0x3E1E, 0x7ACA,
	0x3E20, 0x6A82,
	0x3E22, 0x6B80,
	0x3E24, 0x5845,
	0x3E26, 0x0600,
	0x3E28, 0x439F,
	0x3E2A, 0x4384,
	0x3E2C, 0x0600,
	0x3E2E, 0x587B,
	0x3E30, 0x6B68,
	0x3E32, 0x5267,
	0x3E34, 0x8345,
	0x3E36, 0xA367,
	0x3E38, 0x6464,
	0x3E3A, 0x6565,
	0x3E3C, 0xFF95,
	0x3E3E, 0x5285,
	0x3E40, 0x6964,
	0x3E42, 0x8064,
	0x3E44, 0x826A,
	0x3E46, 0x967A,
	0x3E48, 0xA26A,
	0x3E4A, 0x8058,
	0x3E4C, 0x806B,
	0x3E4E, 0x9641,
	0x3E50, 0x8240,
	0x3E52, 0x8240,
	0x3E54, 0x4C45,
	0x3E56, 0x847B,
	0x3E58, 0x8217,
	0x3E5A, 0x0080,
	0x3E5C, 0x200C,
	0x3E5E, 0x8018,
	0x3E60, 0x0C93,
	0x3E62, 0x6B20,
	0x3E64, 0x3069,
	0x3E66, 0x803B,
	0x3E68, 0x0487,
	0x3E6A, 0x4580,
	0x3E6C, 0x4C40,
	0x3E6E, 0x7000,
	0x3E70, 0x8040,
	0x3E72, 0x4CBD,
	0x3E74, 0x000E,
	0x3E76, 0xBE44,
	0x3E78, 0x8844,
	0x3E7A, 0xBC78,
	0x3E7C, 0x0900,
	0x3E7E, 0x8904,
	0x3E80, 0x8080,
	0x3E82, 0x0240,
	0x3E84, 0x8609,
	0x3E86, 0x008E,
	0x3E88, 0x0900,
	0x3E8A, 0x8002,
	0x3E8C, 0x4080,
	0x3E8E, 0x0480,
	0x3E90, 0x887D,
	0x3E92, 0x7E86,
	0x3E94, 0x0900,
	0x3E96, 0xBA43,
	0x3E98, 0xFFFF,
	0x3E9A, 0xFFFF,
	0x3E9C, 0xFFFF,
	0x3E9E, 0xFFBE,
	0x3EA0, 0x41A6,
	0x3EA2, 0x4291,
	0x3EA4, 0x794C,
	0x3EA6, 0x40BF,
	0x3EA8, 0x7000,
	0x3EAA, 0x0000,
	0x3EAC, 0x0000,
	0x3EAE, 0x0000,
	0x3EB0, 0x0000,
	0x3EB2, 0x0000,
	0x3EB4, 0x0000,
	0x3EB6, 0x0000,
	0x3EB8, 0x0000,
	0x3EBA, 0x0000,
	0x3EBC, 0x0000,
	0x3EBE, 0x0000,
	0x3EC0, 0x0000,
	0x3EC2, 0x0000,
	0x3EC4, 0x0000,
	0x3EC6, 0x0000,
};

#if 0
void ____Sensor_Res_OPR_Table____(){ruturn;} //dummy
#endif

//Mode0:
//[MIPI_4L_FOX_1080p30-2568x1096 FOV=3840x2160 op=384Mbps]
MMP_USHORT SNR_AR1820_Reg_2568x1096_30P_Customer[] = 
{
	0x301A, 0x0018, 	// RESET_REGISTER                   

	SNR_REG_DELAY, 2, 

	0x31BE, 0x2003,    	// MIPI_CONFIG                                    
	0x31AE, 0x0204,    	// SERIAL_FORMAT                                  
	0x0112, 0x0A0A,   	// CCP_DATA_FORMAT                                 
	0x0300, 0x0003,     // VT_PIX_CLK_DIV                               
	0x0302, 0x0001,     // VT_SYS_CLK_DIV                                
	0x0304, 0x0003,     // PRE_PLL_CLK_DIV                               
	0x0306, 96    ,    	// PLL_MULTIPLIER          
	0x0308, 0x000A,    	// OP_PIX_CLK_DIV                                 
	0x030A, 0x0002,    	// OP_SYS_CLK_DIV                                
	0x3016, 0x0111,     // ROW_SPEED                                     
	0x3064, 0x5800,     // SMIA_TEST                                     
	0x3F3A, 0x0027,    	// ANALOG_CONTROL8                                
	0x3172, 0x0286,     // ANALOG_CONTROL2                               
	0x3EDA, 0x1152,   	// DAC_LD_18_19                                    
	0x3EF2, 0x6457,     // DAC_LD_42_43                                  
	0x3154, 0x0500,     // GLOBAL_BOOST                                  
	0x3120, 0x0021,     // GAIN_DITHER_CONTROL                           

	0x3012, 1672  ,  	// COARSE_INTEGRATION_TIME_            
	0x300A, 1672  ,    	// FRAME_LENGTH_LINE_                  
	0x300C, 10200 ,     // LINE_LENGTH_PCK_                    
	0x3002, 754   ,     // Y_ADDR_START_                     
	0x3004, 536   ,  	// X_ADDR_START_                     
	0x30BE, 0x0000,     // X_OUTPUT_OFFSET                        
	0x3006, 2944  , 	// Y_ADDR_END_                         
	0x3008, 4391  , 	// X_ADDR_END_                         
	0x034C, 2568  ,   	// X_OUTPUT_SIZE                       
	0x034E, 1096  ,  	// Y_OUTPUT_SIZE                       
	0x303E, 0x0000,  	// READ_STYLE                            
	 
	0x3040, 0xE043,		// READ_MODE   


	0x306E, 0x9080,     //                                        
	0x0400, 0x0001,  	// SCALING_MODE                          
	0x0404, 0x0018,     // SCALE_M                               
	0x0408, 0x1010,     // SECOND_RESIDUAL                        
	0x040A, 0x016B,     // SECOND_CROP                            
	0x3F00, 0x001D,     // BM_T0                                  
	0x3F02, 0x00EC,     // BM_T1                                  

	0x3F0A, 0x0302,     // NOISE_FLOOR10           
	0x3F0C, 0x0403,     // NOISE_FLOOR32           
	0x3F1E, 0x0004,   	// NOISE_COEF             
	0x3F40, 0x0909,   	// COUPLE_K_FACTOR0       
	0x3F42, 0x0303,     // COUPLE_K_FACTOR1       
	0x3F18, 0x0304,     // CROSSFACTOR1           
	0x305E, 0x2C81,     // GLOBAL_GAIN             
	0x3ED2, 0xE49D,    	// DAC_LD_10_11             
	0x3EE0, 0x9518,     // DAC_LD_24_25           
	0x3EDC, 0x01C9,    	// DAC_LD_20_21             
	0x3EC8, 0x00A4,     // DAC_LD_0_1              
	0x3ED8, 0x60B0,     // DAC_LD_16_17            
	0x3ECC, 0x7386,     // DAC_LD_4_5              
	0x3F1A, 0x0404,     // CROSSFACTOR2            
	0x3F44, 0x0101,     // COUPLE_K_FACTOR2       
	0x301A, 0x001C,     // RESET_REGISTER          
	//STATE = Master Clock, 128000000                  
};


//Mode1: RES_IDX_2568x1096_50P  Video Record 1080(1920x1080)@50P
//[MIPI_4L_FOX_1080p60-2568x1096 FOV=3840x2160 op=528Mbps]-> 50FPS
MMP_USHORT SNR_AR1820_Reg_2568x1096_50P_Customer[] = 
{
	0x301A, 0x0018, 	// RESET_REGISTER                   

	SNR_REG_DELAY, 2, 

	0x31BE, 0x2003, 	// MIPI_CONFIG                     
	0x31AE, 0x0204, 	// SERIAL_FORMAT                   
	0x0112, 0x0A0A, 	// CCP_DATA_FORMAT                 
	0x0300, 0x0003, 	// VT_PIX_CLK_DIV                 
	0x0302, 0x0001, 	// VT_SYS_CLK_DIV                  
	0x0304, 0x0003, 	// PRE_PLL_CLK_DIV                 
	0x0306, 66,			// PLL_MULTIPLIER                 
	0x0308, 0x000A, 	// OP_PIX_CLK_DIV                  
	0x030A, 0x0001, 	// OP_SYS_CLK_DIV                 
	0x3016, 0x0111, 	// ROW_SPEED                       
	0x3064, 0x5800, 	// SMIA_TEST                       
	0x3F3A, 0x0027, 	// ANALOG_CONTROL8                 
	0x3172, 0x0286, 	// ANALOG_CONTROL2                 
	0x3EDA, 0x1152, 	// DAC_LD_18_19                    
	0x3EF2, 0x6457, 	// DAC_LD_42_43                    
	0x3154, 0x0500, 	// GLOBAL_BOOST                    
	0x3120, 0x0021, 	// GAIN_DITHER_CONTROL             

	0x3012, 1168 	,	// COARSE_INTEGRATION_TIME_     
	0x300A, 1401  , 	// FRAME_LENGTH_LINE_		//50fps //1168*(60/50) = 1401
	          
	0x300C, 5020,		// LINE_LENGTH_PCK_              
	0x3002, 754	 ,		// Y_ADDR_START_                
	0x3004, 536	 ,		// X_ADDR_START_                
	0x30BE, 0x0000, 	// X_OUTPUT_OFFSET                
	0x3006, 2944 	,	// Y_ADDR_END_                    
	0x3008, 4391 	,	// X_ADDR_END_                    
	0x034C, 2568	,	// X_OUTPUT_SIZE                  
	0x034E, 1096 	,	// Y_OUTPUT_SIZE                  
	0x303E, 0x0000, 	// READ_STYLE                     

	0x3040, 0xE043, 	// READ_MODE  

	0x306E, 0x9080, 	//                                
	0x0400, 0x0001, 	// SCALING_MODE                   
	0x0404, 0x0018, 	// SCALE_M                        
	0x0408, 0x1010,     // SECOND_RESIDUAL
	0x040A, 0x016B,     // SECOND_CROP                     
	0x3F00, 0x001D, 	// BM_T0                          
	0x3F02, 0x00EC, 	// BM_T1                          

	0x3F0A, 0x0302, 	// NOISE_FLOOR10           
	0x3F0C, 0x0403, 	// NOISE_FLOOR32           
	0x3F1E, 0x0004, 	// NOISE_COEF              
	0x3F40, 0x0909, 	// COUPLE_K_FACTOR0        
	0x3F42, 0x0303, 	// COUPLE_K_FACTOR1        
	0x3F18, 0x0304, 	// CROSSFACTOR1            
	0x305E, 0x2C81, 	// GLOBAL_GAIN             
	0x3ED2, 0xE49D, 	// DAC_LD_10_11            
	0x3EE0, 0x9518, 	// DAC_LD_24_25            
	0x3EDC, 0x01C9, 	// DAC_LD_20_21            
	0x3EC8, 0x00A4, 	// DAC_LD_0_1              
	0x3ED8, 0x60B0, 	// DAC_LD_16_17            
	0x3ECC, 0x7386, 	// DAC_LD_4_5              
	0x3F1A, 0x0404, 	// CROSSFACTOR2            
	0x3F44, 0x0101, 	// COUPLE_K_FACTOR2        
	0x301A, 0x001C, 	// RESET_REGISTER          

	//Master Clock, 88000000  
};

//Mode2: RES_IDX_2568x1096_60P  Video Record 1080(1920x1080)@60P
//[MIPI_4L_FOX_1080p60-2568x1096 FOV=3840x2160 op=528Mbps]
MMP_USHORT SNR_AR1820_Reg_2568x1096_60P_Customer[] = 
{
	0x301A, 0x0018, 	// RESET_REGISTER                   

	SNR_REG_DELAY, 2, 

	0x31BE, 0x2003, 	// MIPI_CONFIG                     
	0x31AE, 0x0204, 	// SERIAL_FORMAT                   
	0x0112, 0x0A0A, 	// CCP_DATA_FORMAT                 
	0x0300, 0x0003, 	// VT_PIX_CLK_DIV                 
	0x0302, 0x0001, 	// VT_SYS_CLK_DIV                  
	0x0304, 0x0003, 	// PRE_PLL_CLK_DIV                 
	0x0306, 66,			// PLL_MULTIPLIER                 
	0x0308, 0x000A, 	// OP_PIX_CLK_DIV                  
	0x030A, 0x0001, 	// OP_SYS_CLK_DIV                 
	0x3016, 0x0111, 	// ROW_SPEED                       
	0x3064, 0x5800, 	// SMIA_TEST                       
	0x3F3A, 0x0027, 	// ANALOG_CONTROL8                 
	0x3172, 0x0286, 	// ANALOG_CONTROL2                 
	0x3EDA, 0x1152, 	// DAC_LD_18_19                    
	0x3EF2, 0x6457, 	// DAC_LD_42_43                    
	0x3154, 0x0500, 	// GLOBAL_BOOST                    
	0x3120, 0x0021, 	// GAIN_DITHER_CONTROL             

	0x3012, 1168 	,	// COARSE_INTEGRATION_TIME_       
	0x300A, 1168 	,	// FRAME_LENGTH_LINE_             
	0x300C, 5020	,	// LINE_LENGTH_PCK_              
	0x3002, 754	 ,		// Y_ADDR_START_                
	0x3004, 536	 ,		// X_ADDR_START_                
	0x30BE, 0x0000, 	// X_OUTPUT_OFFSET                
	0x3006, 2944 	,	// Y_ADDR_END_                    
	0x3008, 4391 	,	// X_ADDR_END_                    
	0x034C, 2568	,	// X_OUTPUT_SIZE                  
	0x034E, 1096 	,	// Y_OUTPUT_SIZE                  
	0x303E, 0x0000, 	// READ_STYLE                     
	  
	0x3040, 0xE043, 	// READ_MODE  

	0x306E, 0x9080, 	//                                
	0x0400, 0x0001, 	// SCALING_MODE                   
	0x0404, 0x0018, 	// SCALE_M                        
	0x0408, 0x1010,     // SECOND_RESIDUAL                        
	0x040A, 0x016B,     // SECOND_CROP                         
	0x3F00, 0x001D, 	// BM_T0                          
	0x3F02, 0x00EC, 	// BM_T1                          

	0x3F0A, 0x0302, 	// NOISE_FLOOR10           
	0x3F0C, 0x0403, 	// NOISE_FLOOR32           
	0x3F1E, 0x0004, 	// NOISE_COEF              
	0x3F40, 0x0909, 	// COUPLE_K_FACTOR0        
	0x3F42, 0x0303, 	// COUPLE_K_FACTOR1        
	0x3F18, 0x0304, 	// CROSSFACTOR1            
	0x305E, 0x2C81, 	// GLOBAL_GAIN             
	0x3ED2, 0xE49D, 	// DAC_LD_10_11            
	0x3EE0, 0x9518, 	// DAC_LD_24_25            
	0x3EDC, 0x01C9, 	// DAC_LD_20_21            
	0x3EC8, 0x00A4, 	// DAC_LD_0_1              
	0x3ED8, 0x60B0, 	// DAC_LD_16_17            
	0x3ECC, 0x7386, 	// DAC_LD_4_5              
	0x3F1A, 0x0404, 	// CROSSFACTOR2            
	0x3F44, 0x0101, 	// COUPLE_K_FACTOR2        
	0x301A, 0x001C, 	// RESET_REGISTER          

	//Master Clock, 88000000  
};

//Mode3: RES_IDX_1288x728_100P, Video Record 720(1280x720)@100P 
//[MIPI_4L_FOX: 720p 1288x728 120fps FOV=3840x2160 op=408Mbps]-> 100FPS
MMP_USHORT SNR_AR1820_Reg_1288x728_100P_Customer[] = 
{
	0x301A, 0x0018, 	// RESET_REGISTER                   

	SNR_REG_DELAY, 2, 

	0x31BE, 0x2003, 	// MIPI_CONFIG                   
	0x31AE, 0x0204, 	// SERIAL_FORMAT                 
	0x0112, 0x0A0A, 	// CCP_DATA_FORMAT               
	0x0300, 0x0003,  	// VT_PIX_CLK_DIV                
	0x0302, 0x0001,  	// VT_SYS_CLK_DIV                
	0x0304, 0x0003,  	// PRE_PLL_CLK_DIV              
	0x0306, 102   ,  	// PLL_MULTIPLIER               
	0x0308, 0x000A,  	// OP_PIX_CLK_DIV                
	0x030A, 0x0002,  	// OP_SYS_CLK_DIV               
	0x3016, 0x0111, 	// ROW_SPEED                     
	0x3064, 0x5800, 	// SMIA_TEST                     
	0x3F3A, 0x0027, 	// ANALOG_CONTROL8               
	0x3172, 0x0286, 	// ANALOG_CONTROL2               
	0x3EDA, 0x1152, 	// DAC_LD_18_19                  
	0x3EF2, 0x6457, 	// DAC_LD_42_43                  
	0x3154, 0x0500, 	// GLOBAL_BOOST                  
	0x3120, 0x0021, 	// GAIN_DITHER_CONTROL                     
	                                                          
	0x3012, 786		,	// COARSE_INTEGRATION_TIME_      
	0x300A, 943		,	// FRAME_LENGTH_LINE_           //100fps  //786*(120/100) = 943
	0x300C, 5760	,	// LINE_LENGTH_PCK_            
	0x3002, 758	 ,		// Y_ADDR_START_                  
	0x3004, 528	 ,		// X_ADDR_START_                  
	0x30BE, 0x0000,		// X_OUTPUT_OFFSET                

	0x3006, 2939	,	// Y_ADDR_END_ 
	0x3008, 4397 	,	// X_ADDR_END_                    
	0x034C, 1288 	,	// X_OUTPUT_SIZE                  
	0x034E, 728	 ,		// Y_OUTPUT_SIZE                  
	0x303E, 0x0000,		// READ_STYLE                     

	0x3040, 0xE8C5,		// READ_MODE 

	0x0400, 0x0001,		// SCALING_MODE                   
	0x0404, 0x0018,		// SCALE_M                        
	0x0408, 0x1010,		// SECOND_RESIDUAL      
	0x040A, 0x16B,		// SECOND_CROP          
	0x3F00, 0x0006,		// BM_T0 ?                        
	0x3F02, 0x0031,		// BM_T1 ?                        

	0x3F0A, 0x0302, 	// NOISE_FLOOR10           
	0x3F0C, 0x0403, 	// NOISE_FLOOR32           
	0x3F1E, 0x0004, 	// NOISE_COEF              
	0x3F40, 0x0909, 	// COUPLE_K_FACTOR0        
	0x3F42, 0x0303, 	// COUPLE_K_FACTOR1        
	0x3F18, 0x0304, 	// CROSSFACTOR1            
	0x305E, 0x2C81, 	// GLOBAL_GAIN             
	0x3ED2, 0xE49D, 	// DAC_LD_10_11            
	0x3EE0, 0x9518, 	// DAC_LD_24_25            
	0x3EDC, 0x01C9, 	// DAC_LD_20_21            
	0x3EC8, 0x00A4, 	// DAC_LD_0_1              
	0x3ED8, 0x60B0, 	// DAC_LD_16_17            
	0x3ECC, 0x7386, 	// DAC_LD_4_5              
	0x3F1A, 0x0404, 	// CROSSFACTOR2            
	0x3F44, 0x0101, 	// COUPLE_K_FACTOR2        
	0x301A, 0x001C, 	// RESET_REGISTER          

	//STATE = Master Clock, 136000000  
};

//Mode4: RES_IDX_1288x728_120P, Video Record 720(1280x720)@120P 
//[MIPI_4L_FOX: 720p 1288x728 120fps FOV=3840x2160 op=408Mbps]
MMP_USHORT SNR_AR1820_Reg_1288x728_120P_Customer[] = 
{
	0x301A, 0x0018, 	// RESET_REGISTER                   

	SNR_REG_DELAY, 2, 

	0x31BE, 0x2003, 	// MIPI_CONFIG                   
	0x31AE, 0x0204, 	// SERIAL_FORMAT                 
	0x0112, 0x0A0A, 	// CCP_DATA_FORMAT               
	0x0300, 0x0003,  	// VT_PIX_CLK_DIV                
	0x0302, 0x0001,  	// VT_SYS_CLK_DIV                
	0x0304, 0x0003,  	// PRE_PLL_CLK_DIV              
	0x0306, 102   ,  	// PLL_MULTIPLIER               
	0x0308, 0x000A,  	// OP_PIX_CLK_DIV                
	0x030A, 0x0002,  	// OP_SYS_CLK_DIV               
	0x3016, 0x0111, 	// ROW_SPEED                     
	0x3064, 0x5800, 	// SMIA_TEST                     
	0x3F3A, 0x0027, 	// ANALOG_CONTROL8               
	0x3172, 0x0286, 	// ANALOG_CONTROL2               
	0x3EDA, 0x1152, 	// DAC_LD_18_19                  
	0x3EF2, 0x6457, 	// DAC_LD_42_43                  
	0x3154, 0x0500, 	// GLOBAL_BOOST                  
	0x3120, 0x0021, 	// GAIN_DITHER_CONTROL                     
	                                                     
	0x3012, 786		,	// COARSE_INTEGRATION_TIME_      
	0x300A, 786		,	// FRAME_LENGTH_LINE_            
	0x300C, 5760	,	// LINE_LENGTH_PCK_            
	0x3002, 758	 ,		// Y_ADDR_START_                  
	0x3004, 528	 ,		// X_ADDR_START_                  
	0x30BE, 0x0000,		// X_OUTPUT_OFFSET                

	0x3006, 2939	,	// Y_ADDR_END_ 
	0x3008, 4397 	,	// X_ADDR_END_                    
	0x034C, 1288 	,	// X_OUTPUT_SIZE                  
	0x034E, 728	 ,		// Y_OUTPUT_SIZE                  
	0x303E, 0x0000,		// READ_STYLE                     

	0x3040, 0xE8C5,		// READ_MODE 

	0x0400, 0x0001,		// SCALING_MODE                   
	0x0404, 0x0018,		// SCALE_M                        
	0x0408, 0x1010,		// SECOND_RESIDUAL      
	0x040A, 0x16B,		// SECOND_CROP          
	0x3F00, 0x0006,		// BM_T0 ?                        
	0x3F02, 0x0031,		// BM_T1 ?                        

	0x3F0A, 0x0302, 	// NOISE_FLOOR10           
	0x3F0C, 0x0403, 	// NOISE_FLOOR32           
	0x3F1E, 0x0004, 	// NOISE_COEF              
	0x3F40, 0x0909, 	// COUPLE_K_FACTOR0        
	0x3F42, 0x0303, 	// COUPLE_K_FACTOR1        
	0x3F18, 0x0304, 	// CROSSFACTOR1            
	0x305E, 0x2C81, 	// GLOBAL_GAIN             
	0x3ED2, 0xE49D, 	// DAC_LD_10_11            
	0x3EE0, 0x9518, 	// DAC_LD_24_25            
	0x3EDC, 0x01C9, 	// DAC_LD_20_21            
	0x3EC8, 0x00A4, 	// DAC_LD_0_1              
	0x3ED8, 0x60B0, 	// DAC_LD_16_17            
	0x3ECC, 0x7386, 	// DAC_LD_4_5              
	0x3F1A, 0x0404, 	// CROSSFACTOR2            
	0x3F44, 0x0101, 	// COUPLE_K_FACTOR2        
	0x301A, 0x001C, 	// RESET_REGISTER          

	//STATE = Master Clock, 136000000  
};

//Mode5: RES_IDX_2728x2010_30P, Video Record 2.7K(2704x1524)@30P (2720x1536)@30P
//[MIPI_4L_FOX_2.7K_30fps-2728x2010 X-16/21 scale Y:native FOV=3570x2002 op=500Mbps]  
MMP_USHORT SNR_AR1820_Reg_2728x2010_30P_Customer[] = 
{
	0x301A, 0x0018, 	// RESET_REGISTER                   

	SNR_REG_DELAY, 2, 

	0x31BE, 0x2003,		// MIPI_CONFIG                      
	0x31AE, 0x0204,		// SERIAL_FORMAT                    
	0x0112, 0x0A0A,		// CCP_DATA_FORMAT                  
	0x0300, 0x0004,		// VT_PIX_CLK_DIV                  
	0x0302, 0x0001,		// VT_SYS_CLK_DIV                   
	0x0304, 0x0006,		// PRE_PLL_CLK_DIV                 
	0x0306, 250		, 	// PLL_MULTIPLIER                
	0x0308, 0x000A,		// OP_PIX_CLK_DIV                   
	0x030A, 0x0002,		// OP_SYS_CLK_DIV                  
	0x3016, 0x0111,		// ROW_SPEED                        
	0x3064, 0x5800,		// SMIA_TEST                        
	0x3F3A, 0x0027,		// ANALOG_CONTROL8                  
	0x3172, 0x0286,		// ANALOG_CONTROL2                  
	0x3EDA, 0x1152,		// DAC_LD_18_19                     
	0x3EF2, 0x6457,		// DAC_LD_42_43                     
	0x3154, 0x0500,		// GLOBAL_BOOST                     
	0x3120, 0x0021,		// GAIN_DITHER_CONTROL              

	0x3012, 2078,		// COARSE_INTEGRATION_TIME_    
	0x300A, 2078	,	// FRAME_LENGTH_LINE_          
	0x300C, 8020 	,	// LINE_LENGTH_PCK_            
	0x3002, 844	 ,		// Y_ADDR_START_                
	0x3004, 672	 ,		// X_ADDR_START_                
	0x30BE, 0x0000,		// X_OUTPUT_OFFSET              
	0x3006, 2854 	,	// Y_ADDR_END_                  
	0x3008, 4255 	,	// X_ADDR_END_                  
	0x034C, 2728 	,	// X_OUTPUT_SIZE                
	0x034E, 2010 	,	// Y_OUTPUT_SIZE                
	0x303E, 0x0000,		// READ_STYLE                   

	0x3040, 0xC041,		// READ_MODE 

	0x0400, 0x0001,		// SCALING_MODE                 
	0x0404, 0x0015,		// SCALE_M                      
	0x0408, 0x0909,		// SECOND_RESIDUAL              
	0x040A, 0x018C,		// SECOND_CROP                  
	0x3F00, 0x0059,		// BM_T0                        
	0x3F02, 0x02D2,		// BM_T1                        

	0x3F0A, 0x0505, 	// NOISE_FLOOR10          
	0x3F0C, 0x0D07, 	// NOISE_FLOOR32          
	0x3F1E, 0x000E, 	// NOISE_COEF             
	0x3F40, 0x1414, 	// COUPLE_K_FACTOR0       
	0x3F42, 0x0C0C, 	// COUPLE_K_FACTOR1       
	0x3F18, 0x0604, 	// CROSSFACTOR1           

	//Gain_Native=1.00x
	0x305E, 0x2001, 	// GLOBAL_GAIN        
	0x3ED2, 0x449D, 	// DAC_LD_10_11       
	0x3EDC, 0x01C9, 	// DAC_LD_20_21       
	0x3EE0, 0x1558, 	// DAC_LD_24_25       
	0x3EC8, 0x0074, 	// DAC_LD_0_1         
	0x3ED8, 0x60A0, 	// DAC_LD_16_17       
	0x3ECC, 0x7386, 	// DAC_LD_4_5         
	0x3F1A, 0x0F04, 	// CROSSFACTOR2       
	0x3F44, 0x0C0C, 	// COUPLE_K_FACTOR2   
	0x301A, 0x001C, 	// RESET_REGISTER     
	//Master Clock, 125000000    
};

//Mode6: RES_IDX_3848x2168_15P, Video Record 4K(3840x2160)@15P 
//[MIPI_4L_FOX_4K2K_15fps-3848x2168 FOV=3840x2160 op=384Mbps] 
MMP_USHORT SNR_AR1820_Reg_3848x2168_15P_Customer[] = 
{
	0x301A, 0x0018, 	// RESET_REGISTER                   

	SNR_REG_DELAY, 60, 

	0x31BE, 0x2003,     // MIPI_CONFIG                    
	0x31AE, 0x0204,     // SERIAL_FORMAT                  
	0x0112, 0x0A0A,    	// CCP_DATA_FORMAT               
	0x0300, 0x0003,  	// VT_PIX_CLK_DIV                
	0x0302, 0x0001,     // VT_SYS_CLK_DIV                
	0x0304, 0x0003,     // PRE_PLL_CLK_DIV              
	0x0306, 96    ,     // PLL_MULTIPLIER           
	0x0308, 0x000A,     // OP_PIX_CLK_DIV                 
	0x030A, 0x0002,     // OP_SYS_CLK_DIV                
	0x3016, 0x0111,     // ROW_SPEED                     
	0x3064, 0x5800,     // SMIA_TEST                     
	0x3F3A, 0x0027,     // ANALOG_CONTROL8                
	0x3172, 0x0286,     // ANALOG_CONTROL2               
	0x3EDA, 0x1152,    	// DAC_LD_18_19                    
	0x3EF2, 0x6457,     // DAC_LD_42_43                  
	0x3154, 0x0500,     // GLOBAL_BOOST                  
	0x3120, 0x0021,     // GAIN_DITHER_CONTROL           

	0x3012, 2318  ,     // COARSE_INTEGRATION_TIME_      
	0x300A, 2318  ,     // FRAME_LENGTH_LINE_            
	0x300C, 14720 ,     // LINE_LENGTH_PCK_           
	0x3002, 0x02FE,     // Y_ADDR_START_                       
	0x3004, 0x021C,    	// X_ADDR_START_                        
	0x30BE, 0x0000,    	// X_OUTPUT_OFFSET                      
	0x3006, 0x0B76,    	// Y_ADDR_END_                          
	0x3008, 0x1123,     // X_ADDR_END_                         
	0x034C, 0x0F08,    	// X_OUTPUT_SIZE                        
	0x034E, 0x0878,     // Y_OUTPUT_SIZE                       
	0x303E, 0x0000,     // READ_STYLE                          

	0x3040, 0xC041,		// READ_MODE 

	0x0400, 0x0000,     // SCALING_MODE                        
	0x0404, 0x0010,     // SCALE_M                             
	0x0408, 0x1010,     // SECOND_RESIDUAL                     
	0x040A, 0x0210,    	// SECOND_CROP                          
	0x3F00, 0x0035,     // BM_T0                               
	0x3F02, 0x01AD,    	// BM_T1                                

	0x3F0A, 0x0505,     // NOISE_FLOOR10                 
	0x3F0C, 0x0D07,     // NOISE_FLOOR32                 
	0x3F1E, 0x000E,     // NOISE_COEF                   
	0x3F40, 0x1414,     // COUPLE_K_FACTOR0             
	0x3F42, 0x0C0C,     // COUPLE_K_FACTOR1              
	0x3F18, 0x0604,     // CROSSFACTOR1                 
	0x305E, 0x2001,     // GLOBAL_GAIN                  
	0x3ED2, 0x449D,    	// DAC_LD_10_11                   
	0x3EDC, 0x01C9,    	// DAC_LD_20_21                   
	0x3EE0, 0x1558,     // DAC_LD_24_25                 
	0x3EC8, 0x0074,     // DAC_LD_0_1                    
	0x3ED8, 0x60A0,    	// DAC_LD_16_17                   
	0x3ECC, 0x7386,     // DAC_LD_4_5                    
	0x3F1A, 0x0F04,     // CROSSFACTOR2                  
	0x3F44, 0x0C0C,     // COUPLE_K_FACTOR2              
	0x301A, 0x001C,     // RESET_REGISTER                

	//Master Clock, 128000000  
};

//----------------------------------------------------------------------
//For photo 

//Mode7: RES_IDX_3688x2776_15P, Photo 8M(3264x2448)@15P 
//[FOX:8M/6M 3688x2776 15fps  XY-native  FOV=3680x2764 op=456Mbps]
MMP_USHORT SNR_AR1820_Reg_3688x2776_15P_Customer[] = 
{
	0x301A, 0x0018, 	// RESET_REGISTER                   

	SNR_REG_DELAY, 2, 

	0x31BE, 0x2003, 	// MIPI_CONFIG                 
	0x31AE, 0x0204, 	// SERIAL_FORMAT               
	0x0112, 0x0A0A, 	// CCP_DATA_FORMAT             
	0x0300, 0x0004,     // VT_PIX_CLK_DIV         
	0x0302, 0x0001,     // VT_SYS_CLK_DIV          
	0x0304, 0x0003,     // PRE_PLL_CLK_DIV        
	0x0306,114    ,     // PLL_MULTIPLIER    
	0x0308, 0x000A,     // OP_PIX_CLK_DIV           
	0x030A, 0x0002,     // OP_SYS_CLK_DIV          
	0x3016, 0x0111, 	// ROW_SPEED                   
	0x3064, 0x5800, 	// SMIA_TEST                   
	0x3F3A, 0x0027, 	// ANALOG_CONTROL8             
	0x3172, 0x0286, 	// ANALOG_CONTROL2             
	0x3EDA, 0x1152, 	// DAC_LD_18_19                
	0x3EF2, 0x6457, 	// DAC_LD_42_43                
	0x3154, 0x0500, 	// GLOBAL_BOOST                
	0x3120, 0x0021, 	// GAIN_DITHER_CONTROL         

	0x3012, 2934	,	// COARSE_INTEGRATION_TIME_   
	0x300A, 2934	,	// FRAME_LENGTH_LINE_   
	0x300C, 10360	,	// LINE_LENGTH_PCK_           
	0x3002, 462	 	,	// Y_ADDR_START_             
	0x3004, 620	 	,	// X_ADDR_START_             
	0x30BE, 0x0000, 	// X_OUTPUT_OFFSET             
	0x3006, 3238 	,	// Y_ADDR_END_                 
	0x3008, 4307 	,	// X_ADDR_END_                 
	0x034C, 3688	,	// X_OUTPUT_SIZE               
	0x034E, 2776 	,	// Y_OUTPUT_SIZE               
	0x303E, 0x0000, 	// READ_STYLE                  

	0x3040, 0xC041,		// READ_MODE 

	0x0400, 0x0000, 	// SCALING_MODE                
	0x0404, 0x0010, 	// SCALE_M                     
	0x0408, 0x1010,		// SECOND_RESIDUAL           
	0x040A, 0x0210, 	// SECOND_CROP                 
	0x3F00, 0x0043, 	// BM_T0                       
	0x3F02, 0x021F, 	// BM_T1                                 

	0x3F0A, 0x0505, 	// NOISE_FLOOR10    
	0x3F0C, 0x0D07, 	// NOISE_FLOOR32    
	0x3F1E, 0x000E, 	// NOISE_COEF       
	0x3F40, 0x1414, 	// COUPLE_K_FACTOR0 
	0x3F42, 0x0C0C, 	// COUPLE_K_FACTOR1 
	0x3F18, 0x0604, 	// CROSSFACTOR1     

	//Gain_Native=1.00x     
	0x305E, 0x2001, 	// GLOBAL_GAIN               
	0x3ED2, 0x449D, 	// DAC_LD_10_11              
	0x3EDC, 0x01C9, 	// DAC_LD_20_21              
	0x3EE0, 0x1558, 	// DAC_LD_24_25              
	0x3EC8, 0x0074, 	// DAC_LD_0_1                
	0x3ED8, 0x60A0, 	// DAC_LD_16_17              
	0x3ECC, 0x7386, 	// DAC_LD_4_5                
	0x3F1A, 0x0F04, 	// CROSSFACTOR2              
	0x3F44, 0x0C0C, 	// COUPLE_K_FACTOR2          
	0x301A, 0x001C, 	// RESET_REGISTER            

	//STATE = Master Clock, 114000000  
};

//Mode8: RES_IDX_3688x2088_15P, Photo 6M-Wide(3264x1836)@15P 
//[FOX:6M-WIDE 3688x2088 15fps  XY-native FOV=3680x2080 op=384Mbps]
MMP_USHORT SNR_AR1820_Reg_3688x2088_15P_Customer[] = 
{
	0x301A, 0x0018, 	// RESET_REGISTER                   

	SNR_REG_DELAY, 2, 

	0x31BE, 0x2003, 	// MIPI_CONFIG                    
	0x31AE, 0x0204, 	// SERIAL_FORMAT                  
	0x0112, 0x0A0A, 	// CCP_DATA_FORMAT                
	0x0300, 0x0004,     // VT_PIX_CLK_DIV            
	0x0302, 0x0001,     // VT_SYS_CLK_DIV             
	0x0304, 0x0003,     // PRE_PLL_CLK_DIV           
	0x0306,114    ,     // PLL_MULTIPLIER       
	0x0308, 0x000A,     // OP_PIX_CLK_DIV              
	0x030A, 0x0002,     // OP_SYS_CLK_DIV             
	0x3016, 0x0111, 	// ROW_SPEED                      
	0x3064, 0x5800, 	// SMIA_TEST                      
	0x3F3A, 0x0027, 	// ANALOG_CONTROL8                
	0x3172, 0x0286, 	// ANALOG_CONTROL2                
	0x3EDA, 0x1152, 	// DAC_LD_18_19                   
	0x3EF2, 0x6457, 	// DAC_LD_42_43                   
	0x3154, 0x0500, 	// GLOBAL_BOOST                   
	0x3120, 0x0021, 	// GAIN_DITHER_CONTROL                  

	0x3012, 2934	,	// COARSE_INTEGRATION_TIME_   
	0x300A, 2934	,	// FRAME_LENGTH_LINE_         
	0x300C, 10360	,	// LINE_LENGTH_PCK_           
	0x3002, 806	 	,	// Y_ADDR_START_             
	0x3004, 620	 	,	// X_ADDR_START_             
	0x30BE, 0x0000, 	// X_OUTPUT_OFFSET             
	0x3006, 2894 	,	// Y_ADDR_END_                 
	0x3008, 4307 	,	// X_ADDR_END_                 
	0x034C, 3688	,	// X_OUTPUT_SIZE               
	0x034E, 2088 	,	// Y_OUTPUT_SIZE               
	0x303E, 0x0000, 	// READ_STYLE                  

	0x3040, 0xC041,		// READ_MODE 

	0x0400, 0x0000, 	// SCALING_MODE                
	0x0404, 0x0010, 	// SCALE_M                     
	0x0408, 0x1010,		// SECOND_RESIDUAL           
	0x040A, 0x0210, 	// SECOND_CROP                 
	0x3F00, 0x0059, 	// BM_T0                       
	0x3F02, 0x02D2, 	// BM_T1                       

	0x3F0A, 0x0505, 	// NOISE_FLOOR10             
	0x3F0C, 0x0D07, 	// NOISE_FLOOR32             
	0x3F1E, 0x000E, 	// NOISE_COEF                
	0x3F40, 0x1414, 	// COUPLE_K_FACTOR0          
	0x3F42, 0x0C0C, 	// COUPLE_K_FACTOR1          
	0x3F18, 0x0604, 	// CROSSFACTOR1  
	            
	//Gain_Native=1.00x 
	0x305E, 0x2001, 	// GLOBAL_GAIN         
	0x3ED2, 0x449D, 	// DAC_LD_10_11        
	0x3EDC, 0x01C9, 	// DAC_LD_20_21        
	0x3EE0, 0x1558, 	// DAC_LD_24_25        
	0x3EC8, 0x0074, 	// DAC_LD_0_1          
	0x3ED8, 0x60A0, 	// DAC_LD_16_17        
	0x3ECC, 0x7386, 	// DAC_LD_4_5          
	0x3F1A, 0x0F04, 	// CROSSFACTOR2        
	0x3F44, 0x0C0C, 	// COUPLE_K_FACTOR2    
	0x301A, 0x001C, 	// RESET_REGISTER      
	//Master Clock, 114000000  
};

//Mode9: RES_IDX_2824x2120_10P, Photo 6M(2816x2112)@10P 
//[FOX:6M 2824x2120 10fps vt=256 op=384Mbps FOV=3264x2448]
MMP_USHORT SNR_AR1820_Reg_2824x2120_10P_Customer[] = 
{
	#if 1
	SNR_REG_DELAY, DELAY_TIME, 
	#else
	0x301A, 0x0018, 	// RESET_REGISTER                   

	SNR_REG_DELAY, 2, 

	0x31AE, 0x0204, 	// SERIAL_FORMAT      
	0x0112, 0x0A0A, 	// CCP_DATA_FORMAT    
	0x3F3A, 0xFF03, 	// ANALOG_CONTROL8    
	0x0300, 0x0003,		// VT_PIX_CLK_DIV       
	0x0302, 0x0001, 	// VT_SYS_CLK_DIV     
	0x0304, 0x0002, 	// PRE_PLL_CLK_DIV    
	0x0306, 0x0020,		// PLL_MULTIPLIER         
	0x0308, 0x000A, 	// OP_PIX_CLK_DIV     
	0x030A, 0x0001,		// OP_SYS_CLK_DIV       
	0x3016, 0x0111, 	// ROW_SPEED          
	0x3064, 0x5800, 	// SMIA_TEST                        
	                                                     
	//LOAD = MIPI Timing-348mbps                                
	0x31B0, 0x003C, 	// FRAME_PREAMBLE  
	0x31B2, 0x002C, 	// LINE_PREAMBLE   
	0x31B4, 0x1633, 	// MIPI_TIMING_0   
	0x31B6, 0x11D4, 	// MIPI_TIMING_1   
	0x31B8, 0x2048, 	// MIPI_TIMING_2   
	0x31BA, 0x0105, 	// MIPI_TIMING_3   
	0x31BC, 0x0004, 	// MIPI_TIMING_4   

	0x3002, 8,	 		// Y_ADDR_START                
	0x3006, 2455,	 	// Y_ADDR_END                  
	0x3004, 0x0008, 	// X_ADDR_START              
	0x3008, 0x0CC7, 	// X_ADDR_END                
	0x3040, 0x4041, 	// READ_MODE                 
	0x300C, 5400,		// LINE_LENGTH_PCK //            
	0x300A, 4740,		// FRAME_LENGTH_LINES    
	0x3012, 4740,		// COARSE_INTEGRATION_TIME     
	0x0400, 0x0002,		// SCALING_MODE                
	0x0402, 0x0000, 	// SPATIAL_SAMPLING          
	0x0404, 0x0012, 	// SCALE_M                   
	0x0408, 0x0E0F, 	// SECOND_RESIDUAL           
	0x040A, 0x01CE, 	// SECOND_CROP               
	0x306E, 0x9090, 	// DATA_PATH_SELECT          
	0x034C, 2824,	 	// X_OUTPUT_SIZE               
	0x034E, 2120,		// Y_OUTPUT_SIZE               
	0x3120, 0x0031, 	// GAIN_DITHER_CONTROL       
	0x301A, 0x001C, 	// RESET_REGISTER            
	                 
	//STATE = Master Clock, 256000000  
	#endif
};

//----------------------------------------------------------------------
//For preview 

//Mode10: RES_IDX_1848x1384_30P, Preview 1280x960@30P 
//[MIPI_4L_FOX: Preview1 1848x1384 30fps FOV=3680x2752 op=300Mbps]
MMP_USHORT SNR_AR1820_Reg_1848x1384_30P_Customer[] = 
{
	0x301A, 0x0018, 	// RESET_REGISTER                   

	SNR_REG_DELAY, 2, 

	0x31BE, 0x2003, 	// MIPI_CONFIG                   
	0x31AE, 0x0204, 	// SERIAL_FORMAT                 
	0x0112, 0x0A0A, 	// CCP_DATA_FORMAT               
	0x0300, 0x0003, 	// VT_PIX_CLK_DIV                
	0x0302, 0x0001, 	// VT_SYS_CLK_DIV                
	0x0304, 0x0004, 	// PRE_PLL_CLK_DIV               
	0x0306, 100	 ,		// PLL_MULTIPLIER              
	0x0308, 0x000A, 	// OP_PIX_CLK_DIV                
	0x030A, 0x0002, 	// OP_SYS_CLK_DIV                
	0x3016, 0x0111, 	// ROW_SPEED                     
	0x3064, 0x5800, 	// SMIA_TEST                     
	0x3F3A, 0x0027, 	// ANALOG_CONTROL8               
	0x3172, 0x0286, 	// ANALOG_CONTROL2               
	0x3EDA, 0x1152, 	// DAC_LD_18_19                  
	0x3EF2, 0x6457, 	// DAC_LD_42_43                  
	0x3154, 0x0500, 	// GLOBAL_BOOST                  
	0x3120, 0x0021, 	// GAIN_DITHER_CONTROL                      

	//LOAD = MIPI Timing-312mbps                                
	0x3012, 1708 	,	// COARSE_INTEGRATION_TIME_         
	0x300A, 1708 	,	// FRAME_LENGTH_LINE_               
	0x300C, 7800 	,	// LINE_LENGTH_PCK                  
	0x3002, 466	 ,		// Y_ADDR_START_                    
	0x3004, 616	 ,		// X_ADDR_START_                    
	0x30BE, 0x0000,		// X_OUTPUT_OFFSET                  
	0x3006, 3232 	,	// Y_ADDR_END_                      
	0x3008, 4309 	,	// X_ADDR_END_                      
	0x034C, 1848 	,	// X_OUTPUT_SIZE                    
	0x034E, 1384 	,	// Y_OUTPUT_SIZE                    
	0x303E, 0x0000,		// READ_STYLE                       

	0x3040, 0xE8C3,		// READ_MODE 

	0x0400, 0x0000,		// SCALING_MODE                     
	0x0404, 0x0010,		// SCALE_M                          
	0x0408, 0x1010,		// SECOND_RESIDUAL                  
	0x040A, 0x0210,		// SECOND_CROP                      
	0x3F00, 0x001D,		// BM_T0                            
	0x3F02, 0x00EC,		// BM_T1                            

	0x3F0A, 0x0302, 	// NOISE_FLOOR10         
	0x3F0C, 0x0403, 	// NOISE_FLOOR32         
	0x3F1E, 0x0004, 	// NOISE_COEF            
	0x3F40, 0x0909, 	// COUPLE_K_FACTOR0      
	0x3F42, 0x0303, 	// COUPLE_K_FACTOR1      
	0x3F18, 0x0304, 	// CROSSFACTOR1          
	0x305E, 0x2C81, 	// GLOBAL_GAIN           
	0x3ED2, 0xE49D, 	// DAC_LD_10_11          
	0x3EE0, 0x9518, 	// DAC_LD_24_25          
	0x3EDC, 0x01C9, 	// DAC_LD_20_21          
	0x3EC8, 0x00A4, 	// DAC_LD_0_1            
	0x3ED8, 0x60B0, 	// DAC_LD_16_17          
	0x3ECC, 0x7386, 	// DAC_LD_4_5            
	0x3F1A, 0x0404, 	// CROSSFACTOR2          
	0x3F44, 0x0101, 	// COUPLE_K_FACTOR2      
	0x301A, 0x001C, 	// RESET_REGISTER        

	//Master Clock, 100000000  
};

//Mode11: RES_IDX_1848x1048_30P, Preview 1280x720@30P 
//[NEW-MIPI_4L_FOX_Preview2-1928x2056 X-16/30 scale Y:native FOV=3616x2056 op=384Mbps]
MMP_USHORT SNR_AR1820_Reg_1848x1048_30P_Customer[] = 
{
	0x301A, 0x0018, 	// RESET_REGISTER                   

	SNR_REG_DELAY, 2, 

	0x31BE, 0x2003,     // MIPI_CONFIG                                        
	0x31AE, 0x0204,     // SERIAL_FORMAT                                      
	0x0112, 0x0A0A,    	// CCP_DATA_FORMAT                                     
	0x0300, 0x0003,  	// VT_PIX_CLK_DIV                                    
	0x0302, 0x0001,  	// VT_SYS_CLK_DIV                                    
	0x0304, 0x0003,  	// PRE_PLL_CLK_DIV                                   
	0x0306, 96    ,   	// PLL_MULTIPLIER                
	0x0308, 0x000A,     // OP_PIX_CLK_DIV                                     
	0x030A, 0x0002,     // OP_SYS_CLK_DIV                                     
	0x3016, 0x0111,     // ROW_SPEED                                         
	0x3064, 0x5800,     // SMIA_TEST                                         
	0x3F3A, 0x0027,     // ANALOG_CONTROL8                                    
	0x3172, 0x0286,     // ANALOG_CONTROL2                                   
	0x3EDA, 0x1152,    	// DAC_LD_18_19                                        
	0x3EF2, 0x6457,     // DAC_LD_42_43                                      
	0x3154, 0x0500,     // GLOBAL_BOOST                                      
	0x3120, 0x0021,     // GAIN_DITHER_CONTROL 

	0x3012, 2122  ,		// COARSE_INTEGRATION_TIME_           
	0x300A, 2122  ,   	// FRAME_LENGTH_LINE_                                 
	0x300C, 8040  ,   	// LINE_LENGTH_PCK_   
	0x3002, 822   ,   	// Y_ADDR_START_                     
	0x3004, 656   ,  	// X_ADDR_START_                     
	0x30BE, 0x0000,     // X_OUTPUT_OFFSET                                       
	0x3006, 2878  ,  	// Y_ADDR_END_                                        
	0x3008, 4271  , 	// X_ADDR_END_                                        
	0x034C, 1928  ,  	// X_OUTPUT_SIZE                                      
	0x034E, 2056  ,  	// Y_OUTPUT_SIZE 
	0x303E, 0x0000,     // READ_STYLE                                           
	0x3040, 0x0041,     // READ_MODE                                            
	0x0400, 0x0001,     // SCALING_MODE                                         
	0x0404, 0x001E,     // SCALE_M                                              
	0x0408, 0x0404,     // SECOND_RESIDUAL                                      
	0x040A, 0x0129,     // SECOND_CROP                                           
	0x3F00, 0x0059,     // BM_T0                                                
	0x3F02, 0x02D2,     // BM_T1                                                 

	//LOAD = Dynamic Col Correction - 10bit-native  
	0x3F0A, 0x0505,     // NOISE_FLOOR10      
	0x3F0C, 0x0D07,     // NOISE_FLOOR32      
	0x3F1E, 0x000E,     // NOISE_COEF        
	0x3F40, 0x1414,     // COUPLE_K_FACTOR0  
	0x3F42, 0x0C0C,     // COUPLE_K_FACTOR1   
	0x3F18, 0x0604,     // CROSSFACTOR1 
	     
	//Gain_Native=1.00x  
	0x305E, 0x2001,     // GLOBAL_GAIN           
	0x3ED2, 0x449D,    	// DAC_LD_10_11            
	0x3EDC, 0x01C9,    	// DAC_LD_20_21            
	0x3EE0, 0x1558,     // DAC_LD_24_25          
	0x3EC8, 0x0074,     // DAC_LD_0_1             
	0x3ED8, 0x60A0,    	// DAC_LD_16_17            
	0x3ECC, 0x7386,     // DAC_LD_4_5             
	0x3F1A, 0x0F04,     // CROSSFACTOR2           
	0x3F44, 0x0C0C,     // COUPLE_K_FACTOR2       
	0x301A, 0x001C,     // RESET_REGISTER         

	//STATE = Master Clock, 128000000  
};

//----------------------------------------------------------------------
//For MP Video Record

//Mode12: RES_IDX_3848x2168_30P, Video Record 2.7K(2704x1524)@30P  (MP)
//[NEW-MIPI_4L_FOX_4K2K_30fps-3848x2168 FOV=3840x2160 op=720Mbps] 
MMP_USHORT SNR_AR1820_Reg_3848x2168_30P_Customer[] = 
{    
    0x301A, 0x0018,     // RESET_REGISTER 
    //Delay=300 
    SNR_REG_DELAY, 60, 
    //POLL_REG= 0x303C, 0x0002, ==0, DELAY=10, TIMEOUT=50                           // polling register R0x303C[1] till it become to 1 
    0x31BE, 0x2003,     // MIPI_CONFIG 
    0x31AE, 0x0204,     // SERIAL_FORMAT 
    0x0112, 0x0A0A,    	// CCP_DATA_FORMAT 
    0x0300, 0x0003,   	// VT_PIX_CLK_DIV 
    0x0302, 0x0001,  	// VT_SYS_CLK_DIV 
    0x0304, 0x0002,  	// PRE_PLL_CLK_DIV 
    0x0306, 60, 		// PLL_MULTIPLIER 
    0x0308, 0x000A,     // OP_PIX_CLK_DIV 
    0x030A, 0x0001,     // OP_SYS_CLK_DIV 
    0x3016, 0x0111,     // ROW_SPEED 
    0x3064, 0x5800,     // SMIA_TEST 
    0x3F3A, 0x0027,     // ANALOG_CONTROL8 
    0x3172, 0x0286,     // ANALOG_CONTROL2 
    0x3EDA, 0x1152,    	// DAC_LD_18_19 
    0x3EF2, 0x6457,     // DAC_LD_42_43 
    0x3154, 0x0500,     // GLOBAL_BOOST 
    0x3120, 0x0021,     // GAIN_DITHER_CONTROL 
 
    0x3012, 2234,      	// COARSE_INTEGRATION_TIME_ 
    0x300A, 2234,       // FRAME_LENGTH_LINE_ 
    0x300C, 7160,      	// LINE_LENGTH_PCK_ 
    0x3002, 0x02FE,     // Y_ADDR_START_ 
    0x3004, 0x021C,     // X_ADDR_START_ 
    0x30BE, 0x0000,     // X_OUTPUT_OFFSET 
    0x3006, 0x0B76,     // Y_ADDR_END_ 
    0x3008, 0x1123,     // X_ADDR_END_ 
    0x034C, 0x0F08,     // X_OUTPUT_SIZE 
    0x034E, 0x0878,     // Y_OUTPUT_SIZE 
    0x303E, 0x0000,     // READ_STYLE 

	0x3040, 0xC041, 	// READ_MODE	
	
	0x0400, 0x0000,     // SCALING_MODE 
    0x0404, 0x0010,     // SCALE_M 
    0x0408, 0x1010,     // SECOND_RESIDUAL 
    0x040A, 0x0210,     // SECOND_CROP 
    0x3F00, 0x0011,     // BM_T0 
    0x3F02, 0x0089,     // BM_T1 
 
    0x3F0A, 0x0505,     // NOISE_FLOOR10 
    0x3F0C, 0x0D07,     // NOISE_FLOOR32 
    0x3F1E, 0x000E,     // NOISE_COEF 
    0x3F40, 0x1414,     // COUPLE_K_FACTOR0 
    0x3F42, 0x0C0C,     // COUPLE_K_FACTOR1 
    0x3F18, 0x0604,     // CROSSFACTOR1 
    0x305E, 0x2001,     // GLOBAL_GAIN 
    0x3ED2, 0x449D,    	// DAC_LD_10_11 
    0x3EDC, 0x01C9,    	// DAC_LD_20_21 
    0x3EE0, 0x1558,     // DAC_LD_24_25 
    0x3EC8, 0x0074,     // DAC_LD_0_1 
    0x3ED8, 0x60A0,    	// DAC_LD_16_17 
    0x3ECC, 0x7386,     // DAC_LD_4_5 
    0x3F1A, 0x0F04,     // CROSSFACTOR2 
    0x3F44, 0x0C0C,     // COUPLE_K_FACTOR2 
    0x301A, 0x001C,     // RESET_REGISTER 
    //STATE = Master Clock, 120000000    
};

//Mode13: RES_IDX_3848x1096_60P, Video Record 1080(1920x1088)@60P  (MP)
//[NEW-MIPI_4L_FOX_1080p60-3848x1096 FOV=3840x2160 op=744Mbps]
MMP_USHORT SNR_AR1820_Reg_3848x1096_60P_Customer[] = 
{  
    0x301A, 0x0018, 	// RESET_REGISTER
    //Delay=300
    SNR_REG_DELAY, 2, 
    //POLL_REG= 0x303C, 0x0002, ==0, DELAY=10, TIMEOUT=50		// polling register R0x303C[1] till it become to 1
    0x31BE, 0x2003, 	// MIPI_CONFIG
    0x31AE, 0x0204, 	// SERIAL_FORMAT
    0x0112, 0x0A0A, 	// CCP_DATA_FORMAT
    0x0300, 0x0003, 	// VT_PIX_CLK_DIV
    0x0302, 0x0001, 	// VT_SYS_CLK_DIV
    0x0304, 0x0002, 	// PRE_PLL_CLK_DIV
    0x0306, 62,	 		// PLL_MULTIPLIER
    0x0308, 0x000A, 	// OP_PIX_CLK_DIV
    0x030A, 0x0001, 	// OP_SYS_CLK_DIV
    0x3016, 0x0111, 	// ROW_SPEED
    0x3064, 0x5800, 	// SMIA_TEST
    0x3F3A, 0x0027, 	// ANALOG_CONTROL8
    0x3172, 0x0286, 	// ANALOG_CONTROL2
    0x3EDA, 0x1152, 	// DAC_LD_18_19
    0x3EF2, 0x6457, 	// DAC_LD_42_43
    0x3154, 0x0500, 	// GLOBAL_BOOST
    0x3120, 0x0021, 	// GAIN_DITHER_CONTROL

    0x3012, 1154, 		// COARSE_INTEGRATION_TIME_
    0x300A, 1154,		// FRAME_LENGTH_LINE_
    0x300C, 7160, 		// LINE_LENGTH_PCK_
    0x3002, 0x02F2, 	// Y_ADDR_START_
    0x3004, 0x021C, 	// X_ADDR_START_
    0x30BE, 0x0000, 	// X_OUTPUT_OFFSET
    0x3006, 0x0B80, 	// Y_ADDR_END_
    0x3008, 0x1123,		// X_ADDR_END_
    0x034C, 3848,		// X_OUTPUT_SIZE
    0x034E, 0x0448, 	// Y_OUTPUT_SIZE
    0x303E, 0x0000, 	// READ_STYLE

    0x3040, 0xE043, 	// READ_MODE
    
	0x0400, 0x0000, 	// SCALING_MODE
    0x0404, 0x0010, 	// SCALE_M
    0x0408, 0x1010, 	// SECOND_RESIDUAL
    0x040A, 0x0210, 	// SECOND_CROP
    0x3F00, 0x0008, 	// BM_T0
    0x3F02, 0x0041, 	// BM_T1
    //
    0x3F0A, 0x0302, 	// NOISE_FLOOR10
    0x3F0C, 0x0403, 	// NOISE_FLOOR32
    0x3F1E, 0x0004, 	// NOISE_COEF
    0x3F40, 0x0909, 	// COUPLE_K_FACTOR0
    0x3F42, 0x0303, 	// COUPLE_K_FACTOR1
    0x3F18, 0x0304, 	// CROSSFACTOR1
    0x305E, 0x2C81, 	// GLOBAL_GAIN
    0x3ED2, 0xE49D, 	// DAC_LD_10_11
    0x3EE0, 0x9518, 	// DAC_LD_24_25
    0x3EDC, 0x01C9, 	// DAC_LD_20_21
    0x3EC8, 0x00A4, 	// DAC_LD_0_1
    0x3ED8, 0x60B0, 	// DAC_LD_16_17
    0x3ECC, 0x7386, 	// DAC_LD_4_5
    0x3F1A, 0x0404, 	// CROSSFACTOR2
    0x3F44, 0x0101, 	// COUPLE_K_FACTOR2
    0x301A, 0x001C, 	// RESET_REGISTER
    //STATE = Master Clock, 124000000    
};

//Mode14: RES_IDX_3848x1096_50P, Video Record 1080(1920x1088)@50P  (MP)  
//[NEW-MIPI_4L_FOX_1080p50-3848x1096 FOV=3840x2160 op=744Mbps] --> 50fps 
MMP_USHORT SNR_AR1820_Reg_3848x1096_50P_Customer[] = 
{  
    0x301A, 0x0018, 	// RESET_REGISTER
    //Delay=300
    SNR_REG_DELAY, 2, 
    //POLL_REG= 0x303C, 0x0002, ==0, DELAY=10, TIMEOUT=50		// polling register R0x303C[1] till it become to 1
    0x31BE, 0x2003, 	// MIPI_CONFIG
    0x31AE, 0x0204, 	// SERIAL_FORMAT
    0x0112, 0x0A0A, 	// CCP_DATA_FORMAT
    0x0300, 0x0003, 	// VT_PIX_CLK_DIV
    0x0302, 0x0001, 	// VT_SYS_CLK_DIV
    0x0304, 0x0002, 	// PRE_PLL_CLK_DIV
    0x0306, 62,	 		// PLL_MULTIPLIER
    0x0308, 0x000A, 	// OP_PIX_CLK_DIV
    0x030A, 0x0001, 	// OP_SYS_CLK_DIV
    0x3016, 0x0111, 	// ROW_SPEED
    0x3064, 0x5800, 	// SMIA_TEST
    0x3F3A, 0x0027, 	// ANALOG_CONTROL8
    0x3172, 0x0286, 	// ANALOG_CONTROL2
    0x3EDA, 0x1152, 	// DAC_LD_18_19
    0x3EF2, 0x6457, 	// DAC_LD_42_43
    0x3154, 0x0500, 	// GLOBAL_BOOST
    0x3120, 0x0021, 	// GAIN_DITHER_CONTROL
    //
    0x3012, 1154, 		// COARSE_INTEGRATION_TIME_

    0x300A, 1384  , 	// FRAME_LENGTH_LINE_		//50fps //1154*(60/50) = 1384
    
	0x300C, 7160, 		// LINE_LENGTH_PCK_
    0x3002, 0x02F2, 	// Y_ADDR_START_
    0x3004, 0x021C, 	// X_ADDR_START_
    0x30BE, 0x0000, 	// X_OUTPUT_OFFSET
    0x3006, 0x0B80, 	// Y_ADDR_END_
    0x3008, 0x1123,		// X_ADDR_END_
    0x034C, 3848,		// X_OUTPUT_SIZE
    0x034E, 0x0448, 	// Y_OUTPUT_SIZE
    0x303E, 0x0000, 	// READ_STYLE

    0x3040, 0xE043, 	// READ_MODE
    
	0x0400, 0x0000, 	// SCALING_MODE
    0x0404, 0x0010, 	// SCALE_M
    0x0408, 0x1010, 	// SECOND_RESIDUAL
    0x040A, 0x0210, 	// SECOND_CROP
    0x3F00, 0x001D, 	// BM_T0
    0x3F02, 0x00EC, 	// BM_T1

    0x3F0A, 0x0302, 	// NOISE_FLOOR10
    0x3F0C, 0x0403, 	// NOISE_FLOOR32
    0x3F1E, 0x0004, 	// NOISE_COEF
    0x3F40, 0x0909, 	// COUPLE_K_FACTOR0
    0x3F42, 0x0303, 	// COUPLE_K_FACTOR1
    0x3F18, 0x0304, 	// CROSSFACTOR1
    0x305E, 0x2C81, 	// GLOBAL_GAIN
    0x3ED2, 0xE49D, 	// DAC_LD_10_11
    0x3EE0, 0x9518, 	// DAC_LD_24_25
    0x3EDC, 0x01C9, 	// DAC_LD_20_21
    0x3EC8, 0x00A4, 	// DAC_LD_0_1
    0x3ED8, 0x60B0, 	// DAC_LD_16_17
    0x3ECC, 0x7386, 	// DAC_LD_4_5
    0x3F1A, 0x0404, 	// CROSSFACTOR2
    0x3F44, 0x0101, 	// COUPLE_K_FACTOR2
    0x301A, 0x001C, 	// RESET_REGISTER
    //STATE = Master Clock, 124000000    
};

//Mode15: RES_IDX_3848x1096_30P, Video Record 1080(1920x1088)@30P  (MP)
//[NEW-MIPI_4L_FOX_1080p30-3848x1096 FOV=3840x2160 op=372Mbps]
MMP_USHORT SNR_AR1820_Reg_3848x1096_30P_Customer[] = 
{  
    0x301A, 0x0018, 	// RESET_REGISTER
    //Delay=300
    SNR_REG_DELAY, 2, 
    //POLL_REG= 0x303C, 0x0002, ==0, DELAY=10, TIMEOUT=50		// polling register R0x303C[1] till it become to 1
    0x31BE, 0x2003, 	// MIPI_CONFIG
    0x31AE, 0x0204, 	// SERIAL_FORMAT
    0x0112, 0x0A0A, 	// CCP_DATA_FORMAT
    0x0300, 0x0003, 	// VT_PIX_CLK_DIV
    0x0302, 0x0001, 	// VT_SYS_CLK_DIV
    0x0304, 0x0002, 	// PRE_PLL_CLK_DIV
    0x0306, 62,	 		// PLL_MULTIPLIER
    0x0308, 0x000A, 	// OP_PIX_CLK_DIV
    0x030A, 0x0001, 	// OP_SYS_CLK_DIV
    0x3016, 0x0111, 	// ROW_SPEED
    0x3064, 0x5800, 	// SMIA_TEST
    0x3F3A, 0x0027, 	// ANALOG_CONTROL8
    0x3172, 0x0286, 	// ANALOG_CONTROL2
    0x3EDA, 0x1152, 	// DAC_LD_18_19
    0x3EF2, 0x6457, 	// DAC_LD_42_43
    0x3154, 0x0500, 	// GLOBAL_BOOST
    0x3120, 0x0021, 	// GAIN_DITHER_CONTROL

    0x3012, 1154, 		// COARSE_INTEGRATION_TIME_
    0x300A, 2308, 		// FRAME_LENGTH_LINE_		//30fps //1154*(60/30) = 2308
    0x300C, 7160, 		// LINE_LENGTH_PCK_
    0x3002, 0x02F2, 	// Y_ADDR_START_
    0x3004, 0x021C, 	// X_ADDR_START_
    0x30BE, 0x0000, 	// X_OUTPUT_OFFSET
    0x3006, 0x0B80, 	// Y_ADDR_END_
    0x3008, 0x1123,		// X_ADDR_END_
    0x034C, 3848,		// X_OUTPUT_SIZE
    0x034E, 0x0448, 	// Y_OUTPUT_SIZE
    0x303E, 0x0000, 	// READ_STYLE

    0x3040, 0xE043, 	// READ_MODE
    
	0x0400, 0x0000, 	// SCALING_MODE
    0x0404, 0x0010, 	// SCALE_M
    0x0408, 0x1010, 	// SECOND_RESIDUAL
    0x040A, 0x0210, 	// SECOND_CROP
    0x3F00, 0x001D, 	// BM_T0
    0x3F02, 0x00EC, 	// BM_T1

    0x3F0A, 0x0302, 	// NOISE_FLOOR10
    0x3F0C, 0x0403, 	// NOISE_FLOOR32
    0x3F1E, 0x0004, 	// NOISE_COEF
    0x3F40, 0x0909, 	// COUPLE_K_FACTOR0
    0x3F42, 0x0303, 	// COUPLE_K_FACTOR1
    0x3F18, 0x0304, 	// CROSSFACTOR1
    0x305E, 0x2C81, 	// GLOBAL_GAIN
    0x3ED2, 0xE49D, 	// DAC_LD_10_11
    0x3EE0, 0x9518, 	// DAC_LD_24_25
    0x3EDC, 0x01C9, 	// DAC_LD_20_21
    0x3EC8, 0x00A4, 	// DAC_LD_0_1
    0x3ED8, 0x60B0, 	// DAC_LD_16_17
    0x3ECC, 0x7386, 	// DAC_LD_4_5
    0x3F1A, 0x0404, 	// CROSSFACTOR2
    0x3F44, 0x0101, 	// COUPLE_K_FACTOR2
    0x301A, 0x001C, 	// RESET_REGISTER
    //STATE = Master Clock, 124000000    
};

//Mode16: RES_IDX_1928x1096_120P, Video Record 720(1280x720)@120P   (MP)
//[NEW-MIPI_4L_FOX_1080p120-1928x1088 FOV=3840x2160 op=800Mbps]
MMP_USHORT SNR_AR1820_Reg_1928x1096_120P_Customer[] = 
{
    0x301A, 0x0018, 	// RESET_REGISTER
    //Delay=300
    SNR_REG_DELAY, 2, 
    
	0x31BE, 0x2003, 	// MIPI_CONFIG					 
	0x31AE, 0x0204, 	// SERIAL_FORMAT				 
	0x0112, 0x0A0A, 	// CCP_DATA_FORMAT				 
	0x0300, 0x0003, 	// VT_PIX_CLK_DIV				 
	0x0302, 0x0001, 	// VT_SYS_CLK_DIV				 
	0x0304, 0x0003, 	// PRE_PLL_CLK_DIV 			 
	0x0306, 0x0064, 	// PLL_MULTIPLIER				 
	0x0308, 0x000A, 	// OP_PIX_CLK_DIV				 
	0x030A, 0x0001, 	// OP_SYS_CLK_DIV				 
	0x3016, 0x0111, 	// ROW_SPEED					 
	0x3064, 0x5800, 	// SMIA_TEST					 
	0x3F3A, 0x0027, 	// ANALOG_CONTROL8				 
	0x3172, 0x0286, 	// ANALOG_CONTROL2				 
	0x3EDA, 0x1152, 	// DAC_LD_18_19 				 
	0x3EF2, 0x6457, 	// DAC_LD_42_43 				 
	0x3154, 0x0500, 	// GLOBAL_BOOST 				 
	0x3120, 0x0021, 	// GAIN_DITHER_CONTROL			 
	0x3012, 0x0465, 	// COARSE_INTEGRATION_TIME_	 
	0x300A, 0x0466, 	// FRAME_LENGTH_LINE_			 
	0x300C, 0x0F6C, 	// LINE_LENGTH_PCK_			 
	0x3002, 0x02FA, 	// Y_ADDR_START_				 
	0x3004, 0x0218, 	// X_ADDR_START_				 
	0x30BE, 0x0000, 	// X_OUTPUT_OFFSET				 
	0x3006, 0x0B78, 	// Y_ADDR_END_ 				 
	0x3008, 0x1125, 	// X_ADDR_END_					 
	0x034C, 0x0788, 	// X_OUTPUT_SIZE				 
	0x034E, 0x0440, 	// Y_OUTPUT_SIZE				 
	0x303E, 0x0000, 	// READ_STYLE					 
	0x3040, 0x28C3, 	// READ_MODE					 
	0x0400, 0x0000, 	// SCALING_MODE 				 
	0x0404, 0x0010, 	// SCALE_M						 
	0x0408, 0x1010, 	// SECOND_RESIDUAL				 
	0x040A, 0x0210, 	// SECOND_CROP					 
	0x3F00, 0x0008, 	// BM_T0						 
	0x3F02, 0x0040, 	// BM_T1						 
	0x3F0A, 0x0302, 	// NOISE_FLOOR10				 
	0x3F0C, 0x0403, 	// NOISE_FLOOR32				 
	0x3F1E, 0x0004, 	// NOISE_COEF					 
	0x3F40, 0x0909, 	// COUPLE_K_FACTOR0 			 
	0x3F42, 0x0303, 	// COUPLE_K_FACTOR1 			 
	0x3F18, 0x0304, 	// CROSSFACTOR1 				 
	0x305E, 0x2C81, 	// GLOBAL_GAIN					 
	0x3ED2, 0xE49D, 	// DAC_LD_10_11 				 
	0x3EDC, 0x01C9, 	// DAC_LD_20_21 				 
	0x3EE0, 0x9518, 	// DAC_LD_24_25 				 
	0x3EC8, 0x00A4, 	// DAC_LD_0_1					 
	0x3ED8, 0x60B0, 	// DAC_LD_16_17 				 
	0x3ECC, 0x7386, 	// DAC_LD_4_5					 
	0x3F1A, 0x0404, 	// CROSSFACTOR2 				 
	0x3F44, 0x0101, 	// COUPLE_K_FACTOR2 			 
	0x301A, 0x001C, 	// RESET_REGISTER				 
    
    //STATE = Master Clock, 133333333    
};

//Mode17: RES_IDX_1928x1096_100P, Video Record 720(1280x720)@100P   (MP)
//[NEW-MIPI_4L_FOX_1080p120-1928x1088 FOV=3840x2160 op=800Mbps]
MMP_USHORT SNR_AR1820_Reg_1928x1096_100P_Customer[] = 
{
    0x301A, 0x0018, 	// RESET_REGISTER
    //Delay=300
    SNR_REG_DELAY, 2, 
    
	0x31BE, 0x2003, 	// MIPI_CONFIG					 
	0x31AE, 0x0204, 	// SERIAL_FORMAT				 
	0x0112, 0x0A0A, 	// CCP_DATA_FORMAT				 
	0x0300, 0x0003, 	// VT_PIX_CLK_DIV				 
	0x0302, 0x0001, 	// VT_SYS_CLK_DIV				 
	0x0304, 0x0003, 	// PRE_PLL_CLK_DIV 			 
	0x0306, 0x0064, 	// PLL_MULTIPLIER				 
	0x0308, 0x000A, 	// OP_PIX_CLK_DIV				 
	0x030A, 0x0001, 	// OP_SYS_CLK_DIV				 
	0x3016, 0x0111, 	// ROW_SPEED					 
	0x3064, 0x5800, 	// SMIA_TEST					 
	0x3F3A, 0x0027, 	// ANALOG_CONTROL8				 
	0x3172, 0x0286, 	// ANALOG_CONTROL2				 
	0x3EDA, 0x1152, 	// DAC_LD_18_19 				 
	0x3EF2, 0x6457, 	// DAC_LD_42_43 				 
	0x3154, 0x0500, 	// GLOBAL_BOOST 				 
	0x3120, 0x0021, 	// GAIN_DITHER_CONTROL			 
	0x3012, 0x0465, 	// COARSE_INTEGRATION_TIME_	 
	
	0x300A, 1351	,	// FRAME_LENGTH_LINE_      //100fps  //0x0466*(120/100) = 1351
	
	0x300C, 0x0F6C, 	// LINE_LENGTH_PCK_			 
	0x3002, 0x02FA, 	// Y_ADDR_START_				 
	0x3004, 0x0218, 	// X_ADDR_START_				 
	0x30BE, 0x0000, 	// X_OUTPUT_OFFSET				 
	0x3006, 0x0B78, 	// Y_ADDR_END_ 				 
	0x3008, 0x1125, 	// X_ADDR_END_					 
	0x034C, 0x0788, 	// X_OUTPUT_SIZE				 
	0x034E, 0x0440, 	// Y_OUTPUT_SIZE				 
	0x303E, 0x0000, 	// READ_STYLE					 
	0x3040, 0x28C3, 	// READ_MODE					 
	0x0400, 0x0000, 	// SCALING_MODE 				 
	0x0404, 0x0010, 	// SCALE_M						 
	0x0408, 0x1010, 	// SECOND_RESIDUAL				 
	0x040A, 0x0210, 	// SECOND_CROP					 
	0x3F00, 0x0008, 	// BM_T0						 
	0x3F02, 0x0040, 	// BM_T1						 
	0x3F0A, 0x0302, 	// NOISE_FLOOR10				 
	0x3F0C, 0x0403, 	// NOISE_FLOOR32				 
	0x3F1E, 0x0004, 	// NOISE_COEF					 
	0x3F40, 0x0909, 	// COUPLE_K_FACTOR0 			 
	0x3F42, 0x0303, 	// COUPLE_K_FACTOR1 			 
	0x3F18, 0x0304, 	// CROSSFACTOR1 				 
	0x305E, 0x2C81, 	// GLOBAL_GAIN					 
	0x3ED2, 0xE49D, 	// DAC_LD_10_11 				 
	0x3EDC, 0x01C9, 	// DAC_LD_20_21 				 
	0x3EE0, 0x9518, 	// DAC_LD_24_25 				 
	0x3EC8, 0x00A4, 	// DAC_LD_0_1					 
	0x3ED8, 0x60B0, 	// DAC_LD_16_17 				 
	0x3ECC, 0x7386, 	// DAC_LD_4_5					 
	0x3F1A, 0x0404, 	// CROSSFACTOR2 				 
	0x3F44, 0x0101, 	// COUPLE_K_FACTOR2 			 
	0x301A, 0x001C, 	// RESET_REGISTER				 
    
    //STATE = Master Clock, 133333333    
};

//Mode18: RES_IDX_1928x1096_60P, Video Record 720(1280x720)@60P   (MP)
//[NEW-MIPI_4L_FOX_1080p60-1928x1096 FOV=3840x2160 op=408Mbps]
MMP_USHORT SNR_AR1820_Reg_1928x1096_60P_Customer[] = 
{
    0x301A, 0x0018, 	// RESET_REGISTER
    //Delay=300
    SNR_REG_DELAY, 2, 
    //POLL_REG= 0x303C, 0x0002, ==0, DELAY=10, TIMEOUT=50		// polling register R0x303C[1] till it become to 1
    0x31BE, 0x2003, 	// MIPI_CONFIG
    0x31AE, 0x0204, 	// SERIAL_FORMAT
    0x0112, 0x0A0A, 	// CCP_DATA_FORMAT
    0x0300, 0x0003, 	// VT_PIX_CLK_DIV
    0x0302, 0x0001, 	// VT_SYS_CLK_DIV
    0x0304, 0x0002, 	// PRE_PLL_CLK_DIV
    0x0306, 68,	 		// PLL_MULTIPLIER
    0x0308, 0x000A, 	// OP_PIX_CLK_DIV
    0x030A, 0x0001, 	// OP_SYS_CLK_DIV
    0x3016, 0x0111, 	// ROW_SPEED
    0x3064, 0x5800, 	// SMIA_TEST
    0x3F3A, 0x0027, 	// ANALOG_CONTROL8
    0x3172, 0x0286, 	// ANALOG_CONTROL2
    0x3EDA, 0x1152, 	// DAC_LD_18_19
    0x3EF2, 0x6457, 	// DAC_LD_42_43
    0x3154, 0x0500, 	// GLOBAL_BOOST
    0x3120, 0x0021, 	// GAIN_DITHER_CONTROL

    0x3012, 1144, 		// COARSE_INTEGRATION_TIME_
    0x300A, 2288, 		// FRAME_LENGTH_LINE_    //60fps  //1144*(120/60) = 2288
    0x300C, 3960, 		// LINE_LENGTH_PCK_
    0x3002, 0x02F2, 	// Y_ADDR_START_
    0x3004, 0x0218, 	// X_ADDR_START_
    0x30BE, 0x0000, 	// X_OUTPUT_OFFSET
    0x3006, 0x0B80, 	// Y_ADDR_END_
    0x3008, 0x1125, 	// X_ADDR_END_
    0x034C, 0x0788, 	// X_OUTPUT_SIZE
    0x034E, 0x0448, 	// Y_OUTPUT_SIZE
    0x303E, 0x0000, 	// READ_STYLE

	0x3040, 0xE8C3, 	// READ_MODE

	0x0400, 0x0000, 	// SCALING_MODE
    0x0404, 0x0010, 	// SCALE_M
    0x0408, 0x1010, 	// SECOND_RESIDUAL
    0x040A, 0x0210, 	// SECOND_CROP
    0x3F00, 0x001D, 	// BM_T0
    0x3F02, 0x00EC, 	// BM_T1
    //
    0x3F0A, 0x0302, 	// NOISE_FLOOR10
    0x3F0C, 0x0403, 	// NOISE_FLOOR32
    0x3F1E, 0x0004, 	// NOISE_COEF
    0x3F40, 0x0909, 	// COUPLE_K_FACTOR0
    0x3F42, 0x0303, 	// COUPLE_K_FACTOR1
    0x3F18, 0x0304, 	// CROSSFACTOR1
    0x305E, 0x2C81, 	// GLOBAL_GAIN
    0x3ED2, 0xE49D, 	// DAC_LD_10_11
    0x3EE0, 0x9518, 	// DAC_LD_24_25
    0x3EDC, 0x01C9, 	// DAC_LD_20_21
    0x3EC8, 0x00A4, 	// DAC_LD_0_1
    0x3ED8, 0x60B0, 	// DAC_LD_16_17
    0x3ECC, 0x7386, 	// DAC_LD_4_5
    0x3F1A, 0x0404, 	// CROSSFACTOR2
    0x3F44, 0x0101, 	// COUPLE_K_FACTOR2
    0x301A, 0x001C, 	// RESET_REGISTER
    //STATE = Master Clock, 136000000    
};

//Mode19: RES_IDX_3688x1384_30P, Photo 4:3(8M or 6M) preview @30P   (MP)
//[MIPI_4L_FOX: Preview1 3688x1384 30fps FOV=3680x2752 op=456Mbps]
MMP_USHORT SNR_AR1820_Reg_3688x1384_30P_Customer[] = 
{
    0x301A, 0x0018, 	// RESET_REGISTER
    //Delay=300
    SNR_REG_DELAY, 2, 
    
    //POLL_REG= 0x303C, 0x0002, ==0, DELAY=10, TIMEOUT=50		// polling register R0x303C[1] till it become to 1
    0x31BE, 0x2003, 	// MIPI_CONFIG
    0x31AE, 0x0204, 	// SERIAL_FORMAT
    0x0112, 0x0A0A, 	// CCP_DATA_FORMAT
    0x0300, 0x0004, 	// VT_PIX_CLK_DIV
    0x0302, 0x0001, 	// VT_SYS_CLK_DIV
    0x0304, 0x0003, 	// PRE_PLL_CLK_DIV
    0x0306, 114,	 	// PLL_MULTIPLIER
    0x0308, 0x000A, 	// OP_PIX_CLK_DIV
    0x030A, 0x0002, 	// OP_SYS_CLK_DIV
    0x3016, 0x0111, 	// ROW_SPEED
    0x3064, 0x5800, 	// SMIA_TEST
    0x3F3A, 0x0027, 	// ANALOG_CONTROL8
    0x3172, 0x0286, 	// ANALOG_CONTROL2
    0x3EDA, 0x1152, 	// DAC_LD_18_19
    0x3EF2, 0x6457, 	// DAC_LD_42_43
    0x3154, 0x0500, 	// GLOBAL_BOOST
    0x3120, 0x0021, 	// GAIN_DITHER_CONTROL
    //
    0x3012,1467,		// COARSE_INTEGRATION_TIME_
    0x300A, 1467,		// FRAME_LENGTH_LINE_
    0x300C, 10360,		// LINE_LENGTH_PCK_
    0x3002, 466,	 	// Y_ADDR_START_
    0x3004, 620,	 	// X_ADDR_START_
    0x30BE, 0x0000, 	// X_OUTPUT_OFFSET
    0x3006, 3234, 		// Y_ADDR_END_
    0x3008, 4307, 		// X_ADDR_END_
    0x034C, 3688,		// X_OUTPUT_SIZE
    0x034E, 1384, 		// Y_OUTPUT_SIZE
    0x303E, 0x0000, 	// READ_STYLE

    0x3040, 0xE043,      // READ_MODE   
    
    0x0400, 0x0000, 	// SCALING_MODE
    0x0404, 0x0010, 	// SCALE_M
    0x0408, 0x1010,		// SECOND_RESIDUAL
    0x040A, 0x0210, 	// SECOND_CROP
    0x3F00, 0x000B, 	// BM_T0
    0x3F02, 0x005A, 	// BM_T1
    //
    0x3F0A, 0x0302, 	// NOISE_FLOOR10
    0x3F0C, 0x0403, 	// NOISE_FLOOR32
    0x3F1E, 0x0004, 	// NOISE_COEF
    0x3F40, 0x0909, 	// COUPLE_K_FACTOR0
    0x3F42, 0x0303, 	// COUPLE_K_FACTOR1
    0x3F18, 0x0304, 	// CROSSFACTOR1
    0x305E, 0x2C81, 	// GLOBAL_GAIN
    0x3ED2, 0xE49D, 	// DAC_LD_10_11
    0x3EE0, 0x9518, 	// DAC_LD_24_25
    0x3EDC, 0x01C9, 	// DAC_LD_20_21
    0x3EC8, 0x00A4, 	// DAC_LD_0_1
    0x3ED8, 0x60B0, 	// DAC_LD_16_17
    0x3ECC, 0x7386, 	// DAC_LD_4_5
    0x3F1A, 0x0404, 	// CROSSFACTOR2
    0x3F44, 0x0101, 	// COUPLE_K_FACTOR2
    0x301A, 0x001C, 	// RESET_REGISTER
    //STATE = Master Clock, 114000000  
};

//Mode21: RES_IDX_3848x1096_25P, Video Record 1080(1920x1080)@25P  (MP)
//[NEW-MIPI_4L_FOX_1080p50-3848x1096 FOV=3840x2160 op=744Mbps] --> 25fps 
MMP_USHORT SNR_AR1820_Reg_3848x1096_25P_Customer[] = 
{  
    0x301A, 0x0018, 	// RESET_REGISTER
    //Delay=300
    SNR_REG_DELAY, 2, 
    //POLL_REG= 0x303C, 0x0002, ==0, DELAY=10, TIMEOUT=50		// polling register R0x303C[1] till it become to 1
    0x31BE, 0x2003, 	// MIPI_CONFIG
    0x31AE, 0x0204, 	// SERIAL_FORMAT
    0x0112, 0x0A0A, 	// CCP_DATA_FORMAT
    0x0300, 0x0003, 	// VT_PIX_CLK_DIV
    0x0302, 0x0001, 	// VT_SYS_CLK_DIV
    0x0304, 0x0002, 	// PRE_PLL_CLK_DIV
    0x0306, 62,	 		// PLL_MULTIPLIER
    0x0308, 0x000A, 	// OP_PIX_CLK_DIV
    0x030A, 0x0001, 	// OP_SYS_CLK_DIV
    0x3016, 0x0111, 	// ROW_SPEED
    0x3064, 0x5800, 	// SMIA_TEST
    0x3F3A, 0x0027, 	// ANALOG_CONTROL8
    0x3172, 0x0286, 	// ANALOG_CONTROL2
    0x3EDA, 0x1152, 	// DAC_LD_18_19
    0x3EF2, 0x6457, 	// DAC_LD_42_43
    0x3154, 0x0500, 	// GLOBAL_BOOST
    0x3120, 0x0021, 	// GAIN_DITHER_CONTROL

    0x3012, 1154, 		// COARSE_INTEGRATION_TIME_

    0x300A, 2769, 		// FRAME_LENGTH_LINE_		//25fps //1154*(60/25) = 2769
	0x300C, 7160, 		// LINE_LENGTH_PCK_
    0x3002, 0x02F2, 	// Y_ADDR_START_
    0x3004, 0x021C, 	// X_ADDR_START_
    0x30BE, 0x0000, 	// X_OUTPUT_OFFSET
    0x3006, 0x0B80, 	// Y_ADDR_END_
    0x3008, 0x1123,		// X_ADDR_END_
    0x034C, 3848,		// X_OUTPUT_SIZE
    0x034E, 0x0448, 	// Y_OUTPUT_SIZE
    0x303E, 0x0000, 	// READ_STYLE

    0x3040, 0xE043, 	// READ_MODE
    
	0x0400, 0x0000, 	// SCALING_MODE
    0x0404, 0x0010, 	// SCALE_M
    0x0408, 0x1010, 	// SECOND_RESIDUAL
    0x040A, 0x0210, 	// SECOND_CROP
    0x3F00, 0x001D, 	// BM_T0
    0x3F02, 0x00EC, 	// BM_T1
    //
    0x3F0A, 0x0302, 	// NOISE_FLOOR10
    0x3F0C, 0x0403, 	// NOISE_FLOOR32
    0x3F1E, 0x0004, 	// NOISE_COEF
    0x3F40, 0x0909, 	// COUPLE_K_FACTOR0
    0x3F42, 0x0303, 	// COUPLE_K_FACTOR1
    0x3F18, 0x0304, 	// CROSSFACTOR1
    0x305E, 0x2C81, 	// GLOBAL_GAIN
    0x3ED2, 0xE49D, 	// DAC_LD_10_11
    0x3EE0, 0x9518, 	// DAC_LD_24_25
    0x3EDC, 0x01C9, 	// DAC_LD_20_21
    0x3EC8, 0x00A4, 	// DAC_LD_0_1
    0x3ED8, 0x60B0, 	// DAC_LD_16_17
    0x3ECC, 0x7386, 	// DAC_LD_4_5
    0x3F1A, 0x0404, 	// CROSSFACTOR2
    0x3F44, 0x0101, 	// COUPLE_K_FACTOR2
    0x301A, 0x001C, 	// RESET_REGISTER
    //STATE = Master Clock, 124000000    
};

//Mode22: RES_IDX_1928x1096_50P, Video Record 720(1280x720)@50P   (MP)
//[NEW-MIPI_4L_FOX_1080p100-1928x1096 FOV=3840x2160 op=816Mbps] --> 50 fps 
MMP_USHORT SNR_AR1820_Reg_1928x1096_50P_Customer[] = 
{
    0x301A, 0x0018, 	// RESET_REGISTER
    //Delay=300
    SNR_REG_DELAY, 2, 
    //POLL_REG= 0x303C, 0x0002, ==0, DELAY=10, TIMEOUT=50		// polling register R0x303C[1] till it become to 1
    0x31BE, 0x2003, 	// MIPI_CONFIG
    0x31AE, 0x0204, 	// SERIAL_FORMAT
    0x0112, 0x0A0A, 	// CCP_DATA_FORMAT
    0x0300, 0x0003, 	// VT_PIX_CLK_DIV
    0x0302, 0x0001, 	// VT_SYS_CLK_DIV
    0x0304, 0x0002, 	// PRE_PLL_CLK_DIV
    0x0306, 68,	 		// PLL_MULTIPLIER
    0x0308, 0x000A, 	// OP_PIX_CLK_DIV
    0x030A, 0x0001, 	// OP_SYS_CLK_DIV
    0x3016, 0x0111, 	// ROW_SPEED
    0x3064, 0x5800, 	// SMIA_TEST
    0x3F3A, 0x0027, 	// ANALOG_CONTROL8
    0x3172, 0x0286, 	// ANALOG_CONTROL2
    0x3EDA, 0x1152, 	// DAC_LD_18_19
    0x3EF2, 0x6457, 	// DAC_LD_42_43
    0x3154, 0x0500, 	// GLOBAL_BOOST
    0x3120, 0x0021, 	// GAIN_DITHER_CONTROL

    0x3012, 1144, 		// COARSE_INTEGRATION_TIME_

	0x300A, 2745,		// FRAME_LENGTH_LINE_      //50fps  //1144*(120/50) = 2745
	0x300C, 3960, 		// LINE_LENGTH_PCK_
    0x3002, 0x02F2, 	// Y_ADDR_START_
    0x3004, 0x0218, 	// X_ADDR_START_
    0x30BE, 0x0000, 	// X_OUTPUT_OFFSET
    0x3006, 0x0B80, 	// Y_ADDR_END_
    0x3008, 0x1125, 	// X_ADDR_END_
    0x034C, 0x0788, 	// X_OUTPUT_SIZE
    0x034E, 0x0448, 	// Y_OUTPUT_SIZE
    0x303E, 0x0000, 	// READ_STYLE

	0x3040, 0xE8C3, 	// READ_MODE

	0x0400, 0x0000, 	// SCALING_MODE
    0x0404, 0x0010, 	// SCALE_M
    0x0408, 0x1010, 	// SECOND_RESIDUAL
    0x040A, 0x0210, 	// SECOND_CROP
    0x3F00, 0x001D, 	// BM_T0
    0x3F02, 0x00EC, 	// BM_T1

    0x3F0A, 0x0302, 	// NOISE_FLOOR10
    0x3F0C, 0x0403, 	// NOISE_FLOOR32
    0x3F1E, 0x0004, 	// NOISE_COEF
    0x3F40, 0x0909, 	// COUPLE_K_FACTOR0
    0x3F42, 0x0303, 	// COUPLE_K_FACTOR1
    0x3F18, 0x0304, 	// CROSSFACTOR1
    0x305E, 0x2C81, 	// GLOBAL_GAIN
    0x3ED2, 0xE49D, 	// DAC_LD_10_11
    0x3EE0, 0x9518, 	// DAC_LD_24_25
    0x3EDC, 0x01C9, 	// DAC_LD_20_21
    0x3EC8, 0x00A4, 	// DAC_LD_0_1
    0x3ED8, 0x60B0, 	// DAC_LD_16_17
    0x3ECC, 0x7386, 	// DAC_LD_4_5
    0x3F1A, 0x0404, 	// CROSSFACTOR2
    0x3F44, 0x0101, 	// COUPLE_K_FACTOR2
    0x301A, 0x001C, 	// RESET_REGISTER
    //STATE = Master Clock, 136000000    
};

//Mode23: RES_IDX_1928x1096_30P, Video Record 720(1280x720)@30P   (MP)
//[NEW-MIPI_4L_FOX_1080p100-1928x1096 FOV=3840x2160 op=816Mbps] --> 30 fps 
MMP_USHORT SNR_AR1820_Reg_1928x1096_30P_Customer[] = 
{
    0x301A, 0x0018, 	// RESET_REGISTER
    //Delay=300
    SNR_REG_DELAY, 2, 
    //POLL_REG= 0x303C, 0x0002, ==0, DELAY=10, TIMEOUT=50		// polling register R0x303C[1] till it become to 1
    0x31BE, 0x2003, 	// MIPI_CONFIG
    0x31AE, 0x0204, 	// SERIAL_FORMAT
    0x0112, 0x0A0A, 	// CCP_DATA_FORMAT
    0x0300, 0x0003, 	// VT_PIX_CLK_DIV
    0x0302, 0x0001, 	// VT_SYS_CLK_DIV
    0x0304, 0x0002, 	// PRE_PLL_CLK_DIV
    0x0306, 68,	 		// PLL_MULTIPLIER
    0x0308, 0x000A, 	// OP_PIX_CLK_DIV
    0x030A, 0x0001, 	// OP_SYS_CLK_DIV
    0x3016, 0x0111, 	// ROW_SPEED
    0x3064, 0x5800, 	// SMIA_TEST
    0x3F3A, 0x0027, 	// ANALOG_CONTROL8
    0x3172, 0x0286, 	// ANALOG_CONTROL2
    0x3EDA, 0x1152, 	// DAC_LD_18_19
    0x3EF2, 0x6457, 	// DAC_LD_42_43
    0x3154, 0x0500, 	// GLOBAL_BOOST
    0x3120, 0x0021, 	// GAIN_DITHER_CONTROL

    0x3012, 1144, 		// COARSE_INTEGRATION_TIME_

	0x300A, 4576,		// FRAME_LENGTH_LINE_      //25fps  //1144*(120/30) = 4576

	0x300C, 3960, 		// LINE_LENGTH_PCK_
    0x3002, 0x02F2, 	// Y_ADDR_START_
    0x3004, 0x0218, 	// X_ADDR_START_
    0x30BE, 0x0000, 	// X_OUTPUT_OFFSET
    0x3006, 0x0B80, 	// Y_ADDR_END_
    0x3008, 0x1125, 	// X_ADDR_END_
    0x034C, 0x0788, 	// X_OUTPUT_SIZE
    0x034E, 0x0448, 	// Y_OUTPUT_SIZE
    0x303E, 0x0000, 	// READ_STYLE

	0x3040, 0xE8C3, 	// READ_MODE

	0x0400, 0x0000, 	// SCALING_MODE
    0x0404, 0x0010, 	// SCALE_M
    0x0408, 0x1010, 	// SECOND_RESIDUAL
    0x040A, 0x0210, 	// SECOND_CROP
    0x3F00, 0x001D, 	// BM_T0
    0x3F02, 0x00EC, 	// BM_T1
    //
    0x3F0A, 0x0302, 	// NOISE_FLOOR10
    0x3F0C, 0x0403, 	// NOISE_FLOOR32
    0x3F1E, 0x0004, 	// NOISE_COEF
    0x3F40, 0x0909, 	// COUPLE_K_FACTOR0
    0x3F42, 0x0303, 	// COUPLE_K_FACTOR1
    0x3F18, 0x0304, 	// CROSSFACTOR1
    0x305E, 0x2C81, 	// GLOBAL_GAIN
    0x3ED2, 0xE49D, 	// DAC_LD_10_11
    0x3EE0, 0x9518, 	// DAC_LD_24_25
    0x3EDC, 0x01C9, 	// DAC_LD_20_21
    0x3EC8, 0x00A4, 	// DAC_LD_0_1
    0x3ED8, 0x60B0, 	// DAC_LD_16_17
    0x3ECC, 0x7386, 	// DAC_LD_4_5
    0x3F1A, 0x0404, 	// CROSSFACTOR2
    0x3F44, 0x0101, 	// COUPLE_K_FACTOR2
    0x301A, 0x001C, 	// RESET_REGISTER
    //STATE = Master Clock, 136000000    
};

//Mode24: RES_IDX_1928x1096_25P, Video Record 720(1280x720)@25P   (MP)
//[NEW-MIPI_4L_FOX_1080p100-1928x1096 FOV=3840x2160 op=816Mbps] --> 25 fps 
MMP_USHORT SNR_AR1820_Reg_1928x1096_25P_Customer[] = 
{
    0x301A, 0x0018, 	// RESET_REGISTER
    //Delay=300
    SNR_REG_DELAY, 2, 
    //POLL_REG= 0x303C, 0x0002, ==0, DELAY=10, TIMEOUT=50		// polling register R0x303C[1] till it become to 1
    0x31BE, 0x2003, 	// MIPI_CONFIG
    0x31AE, 0x0204, 	// SERIAL_FORMAT
    0x0112, 0x0A0A, 	// CCP_DATA_FORMAT
    0x0300, 0x0003, 	// VT_PIX_CLK_DIV
    0x0302, 0x0001, 	// VT_SYS_CLK_DIV
    0x0304, 0x0002, 	// PRE_PLL_CLK_DIV
    0x0306, 68,	 		// PLL_MULTIPLIER
    0x0308, 0x000A, 	// OP_PIX_CLK_DIV
    0x030A, 0x0001, 	// OP_SYS_CLK_DIV
    0x3016, 0x0111, 	// ROW_SPEED
    0x3064, 0x5800, 	// SMIA_TEST
    0x3F3A, 0x0027, 	// ANALOG_CONTROL8
    0x3172, 0x0286, 	// ANALOG_CONTROL2
    0x3EDA, 0x1152, 	// DAC_LD_18_19
    0x3EF2, 0x6457, 	// DAC_LD_42_43
    0x3154, 0x0500, 	// GLOBAL_BOOST
    0x3120, 0x0021, 	// GAIN_DITHER_CONTROL

    0x3012, 1144, 		// COARSE_INTEGRATION_TIME_

	0x300A, 5491,		// FRAME_LENGTH_LINE_      //25fps  //1144*(120/25) = 5491

	0x300C, 3960, 		// LINE_LENGTH_PCK_
    0x3002, 0x02F2, 	// Y_ADDR_START_
    0x3004, 0x0218, 	// X_ADDR_START_
    0x30BE, 0x0000, 	// X_OUTPUT_OFFSET
    0x3006, 0x0B80, 	// Y_ADDR_END_
    0x3008, 0x1125, 	// X_ADDR_END_
    0x034C, 0x0788, 	// X_OUTPUT_SIZE
    0x034E, 0x0448, 	// Y_OUTPUT_SIZE
    0x303E, 0x0000, 	// READ_STYLE

	0x3040, 0xE8C3, 	// READ_MODE

	0x0400, 0x0000, 	// SCALING_MODE
    0x0404, 0x0010, 	// SCALE_M
    0x0408, 0x1010, 	// SECOND_RESIDUAL
    0x040A, 0x0210, 	// SECOND_CROP
    0x3F00, 0x001D, 	// BM_T0
    0x3F02, 0x00EC, 	// BM_T1
    //
    0x3F0A, 0x0302, 	// NOISE_FLOOR10
    0x3F0C, 0x0403, 	// NOISE_FLOOR32
    0x3F1E, 0x0004, 	// NOISE_COEF
    0x3F40, 0x0909, 	// COUPLE_K_FACTOR0
    0x3F42, 0x0303, 	// COUPLE_K_FACTOR1
    0x3F18, 0x0304, 	// CROSSFACTOR1
    0x305E, 0x2C81, 	// GLOBAL_GAIN
    0x3ED2, 0xE49D, 	// DAC_LD_10_11
    0x3EE0, 0x9518, 	// DAC_LD_24_25
    0x3EDC, 0x01C9, 	// DAC_LD_20_21
    0x3EC8, 0x00A4, 	// DAC_LD_0_1
    0x3ED8, 0x60B0, 	// DAC_LD_16_17
    0x3ECC, 0x7386, 	// DAC_LD_4_5
    0x3F1A, 0x0404, 	// CROSSFACTOR2
    0x3F44, 0x0101, 	// COUPLE_K_FACTOR2
    0x301A, 0x001C, 	// RESET_REGISTER
    //STATE = Master Clock, 136000000    
};

//Mode25: RES_IDX_3848x2168_12d5P, Video Record 4K(3840x2160)@12d5P 
//Reference RES_IDX_3848x2168_15P   (6)
//[MIPI_4L_FOX_4K2K_15fps-3848x2168 FOV=3840x2160 op=384Mbps] 
MMP_USHORT SNR_AR1820_Reg_3848x2168_12d5P_Customer[] = 
{
	0x301A, 0x0018, 	// RESET_REGISTER                   

	SNR_REG_DELAY, 60, 

	0x31BE, 0x2003,     // MIPI_CONFIG                    
	0x31AE, 0x0204,     // SERIAL_FORMAT                  
	0x0112, 0x0A0A,    	// CCP_DATA_FORMAT               
	0x0300, 0x0003,  	// VT_PIX_CLK_DIV                
	0x0302, 0x0001,  	// VT_SYS_CLK_DIV                
	0x0304, 0x0003,  	// PRE_PLL_CLK_DIV              
	0x0306, 96    ,     // PLL_MULTIPLIER           
	0x0308, 0x000A,     // OP_PIX_CLK_DIV                 
	0x030A, 0x0002,     // OP_SYS_CLK_DIV                
	0x3016, 0x0111,     // ROW_SPEED                     
	0x3064, 0x5800,     // SMIA_TEST                     
	0x3F3A, 0x0027,     // ANALOG_CONTROL8                
	0x3172, 0x0286,     // ANALOG_CONTROL2               
	0x3EDA, 0x1152,    	// DAC_LD_18_19                    
	0x3EF2, 0x6457,     // DAC_LD_42_43                  
	0x3154, 0x0500,     // GLOBAL_BOOST                  
	0x3120, 0x0021,     // GAIN_DITHER_CONTROL           

	0x3012, 2318  ,     // COARSE_INTEGRATION_TIME_      
	        
	0x300A, 2781  ,     // FRAME_LENGTH_LINE_  //12.5fps  //2318*(15/12.5) = 2781

	0x300C, 14720 ,     // LINE_LENGTH_PCK_           
	0x3002, 0x02FE,     // Y_ADDR_START_                       
	0x3004, 0x021C,    	// X_ADDR_START_                        
	0x30BE, 0x0000,    	// X_OUTPUT_OFFSET                      
	0x3006, 0x0B76,    	// Y_ADDR_END_                          
	0x3008, 0x1123,     // X_ADDR_END_                         
	0x034C, 0x0F08,    	// X_OUTPUT_SIZE                        
	0x034E, 0x0878,     // Y_OUTPUT_SIZE                       
	0x303E, 0x0000,     // READ_STYLE                          

	0x3040, 0xC041,		// READ_MODE 

	0x0400, 0x0000,     // SCALING_MODE                        
	0x0404, 0x0010,     // SCALE_M                             
	0x0408, 0x1010,     // SECOND_RESIDUAL                     
	0x040A, 0x0210,    	// SECOND_CROP                          
	0x3F00, 0x0035,     // BM_T0                               
	0x3F02, 0x01AD,    	// BM_T1                                

	0x3F0A, 0x0505,     // NOISE_FLOOR10                 
	0x3F0C, 0x0D07,     // NOISE_FLOOR32                 
	0x3F1E, 0x000E,   	// NOISE_COEF                   
	0x3F40, 0x1414,   	// COUPLE_K_FACTOR0             
	0x3F42, 0x0C0C,     // COUPLE_K_FACTOR1              
	0x3F18, 0x0604,    	// CROSSFACTOR1                 
	0x305E, 0x2001,   	// GLOBAL_GAIN                  
	0x3ED2, 0x449D,    	// DAC_LD_10_11                   
	0x3EDC, 0x01C9,    	// DAC_LD_20_21                   
	0x3EE0, 0x1558,   	// DAC_LD_24_25                 
	0x3EC8, 0x0074,     // DAC_LD_0_1                    
	0x3ED8, 0x60A0,    	// DAC_LD_16_17                   
	0x3ECC, 0x7386,     // DAC_LD_4_5                    
	0x3F1A, 0x0F04,     // CROSSFACTOR2                  
	0x3F44, 0x0C0C,     // COUPLE_K_FACTOR2              
	0x301A, 0x001C,     // RESET_REGISTER                

	//Master Clock, 128000000  
};

//Mode26: RES_IDX_3848x2168_25P, Video Record 2.7K(2704x1524)@25P (MP)
//Reference RES_IDX_3848x2168_30P   (12)
//[NEW-MIPI_4L_FOX_4K2K_30fps-3848x2168 FOV=3840x2160 op=720Mbps] 
MMP_USHORT SNR_AR1820_Reg_3848x2168_25P_Customer[] = 
{    
    0x301A, 0x0018,     // RESET_REGISTER 
    //Delay=300 
    SNR_REG_DELAY, 60, 
    //POLL_REG= 0x303C, 0x0002, ==0, DELAY=10, TIMEOUT=50                           // polling register R0x303C[1] till it become to 1 
    0x31BE, 0x2003,     // MIPI_CONFIG 
    0x31AE, 0x0204,     // SERIAL_FORMAT 
    0x0112, 0x0A0A,    	// CCP_DATA_FORMAT 
    0x0300, 0x0003, 	// VT_PIX_CLK_DIV 
    0x0302, 0x0001,   	// VT_SYS_CLK_DIV 
    0x0304, 0x0002,  	// PRE_PLL_CLK_DIV 
    0x0306, 60,         // PLL_MULTIPLIER 
    0x0308, 0x000A,     // OP_PIX_CLK_DIV 
    0x030A, 0x0001,     // OP_SYS_CLK_DIV 
    0x3016, 0x0111,     // ROW_SPEED 
    0x3064, 0x5800,     // SMIA_TEST 
    0x3F3A, 0x0027,     // ANALOG_CONTROL8 
    0x3172, 0x0286,     // ANALOG_CONTROL2 
    0x3EDA, 0x1152,    	// DAC_LD_18_19 
    0x3EF2, 0x6457,     // DAC_LD_42_43 
    0x3154, 0x0500,     // GLOBAL_BOOST 
    0x3120, 0x0021,     // GAIN_DITHER_CONTROL 

    0x3012, 2234,       // COARSE_INTEGRATION_TIME_ 

	0x300A, 2680,       // FRAME_LENGTH_LINE_ 	//25fps	 //2234*(30/25) = 2680

	0x300C, 7160,       // LINE_LENGTH_PCK_ 
    0x3002, 0x02FE,     // Y_ADDR_START_ 
    0x3004, 0x021C,     // X_ADDR_START_ 
    0x30BE, 0x0000,     // X_OUTPUT_OFFSET 
    0x3006, 0x0B76,     // Y_ADDR_END_ 
    0x3008, 0x1123,     // X_ADDR_END_ 
    0x034C, 0x0F08,     // X_OUTPUT_SIZE 
    0x034E, 0x0878,     // Y_OUTPUT_SIZE 
    0x303E, 0x0000,     // READ_STYLE 

	0x3040, 0xC041, 	// READ_MODE	
	
	0x0400, 0x0000,     // SCALING_MODE 
    0x0404, 0x0010,     // SCALE_M 
    0x0408, 0x1010,     // SECOND_RESIDUAL 
    0x040A, 0x0210,     // SECOND_CROP 
    0x3F00, 0x0011,     // BM_T0 
    0x3F02, 0x0089,     // BM_T1 

    0x3F0A, 0x0505,     // NOISE_FLOOR10 
    0x3F0C, 0x0D07,     // NOISE_FLOOR32 
    0x3F1E, 0x000E,     // NOISE_COEF 
    0x3F40, 0x1414,     // COUPLE_K_FACTOR0 
    0x3F42, 0x0C0C,     // COUPLE_K_FACTOR1 
    0x3F18, 0x0604,     // CROSSFACTOR1 
    0x305E, 0x2001,     // GLOBAL_GAIN 
    0x3ED2, 0x449D,    	// DAC_LD_10_11 
    0x3EDC, 0x01C9,    	// DAC_LD_20_21 
    0x3EE0, 0x1558,     // DAC_LD_24_25 
    0x3EC8, 0x0074,     // DAC_LD_0_1 
    0x3ED8, 0x60A0,    	// DAC_LD_16_17 
    0x3ECC, 0x7386,     // DAC_LD_4_5 
    0x3F1A, 0x0F04,     // CROSSFACTOR2 
    0x3F44, 0x0C0C,     // COUPLE_K_FACTOR2 
    0x301A, 0x001C,     // RESET_REGISTER 
    //STATE = Master Clock, 120000000    
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
    // Init OPR Table
    SensorCustFunc.OprTable->usInitSize                   			= (sizeof(SNR_AR1820_Reg_Init_Customer)/sizeof(SNR_AR1820_Reg_Init_Customer[0]))/2;
    SensorCustFunc.OprTable->uspInitTable                 			= &SNR_AR1820_Reg_Init_Customer[0];    

    SensorCustFunc.OprTable->bBinTableExist                         = MMP_FALSE;
    SensorCustFunc.OprTable->bInitDoneTableExist                    = MMP_FALSE;

    //Mode0
	SensorCustFunc.OprTable->usSize[RES_IDX_2568x1096_30P]       	= (sizeof(SNR_AR1820_Reg_2568x1096_30P_Customer)/sizeof(SNR_AR1820_Reg_2568x1096_30P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_2568x1096_30P]     	= &SNR_AR1820_Reg_2568x1096_30P_Customer[0];

    //Mode1
    SensorCustFunc.OprTable->usSize[RES_IDX_2568x1096_50P]		  	= (sizeof(SNR_AR1820_Reg_2568x1096_50P_Customer)/sizeof(SNR_AR1820_Reg_2568x1096_50P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_2568x1096_50P] 	  	= &SNR_AR1820_Reg_2568x1096_50P_Customer[0];    

    //Mode2
    SensorCustFunc.OprTable->usSize[RES_IDX_2568x1096_60P]		  	= (sizeof(SNR_AR1820_Reg_2568x1096_60P_Customer)/sizeof(SNR_AR1820_Reg_2568x1096_60P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_2568x1096_60P] 	  	= &SNR_AR1820_Reg_2568x1096_60P_Customer[0];

    //Mode3
    SensorCustFunc.OprTable->usSize[RES_IDX_1288x728_100P]		  	= (sizeof(SNR_AR1820_Reg_1288x728_100P_Customer)/sizeof(SNR_AR1820_Reg_1288x728_100P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1288x728_100P] 	  	= &SNR_AR1820_Reg_1288x728_100P_Customer[0];

    //Mode4
    SensorCustFunc.OprTable->usSize[RES_IDX_1288x728_120P]			= (sizeof(SNR_AR1820_Reg_1288x728_120P_Customer)/sizeof(SNR_AR1820_Reg_1288x728_120P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1288x728_120P] 	  	= &SNR_AR1820_Reg_1288x728_120P_Customer[0];

    //Mode5
	SensorCustFunc.OprTable->usSize[RES_IDX_2728x2010_30P]		  	= (sizeof(SNR_AR1820_Reg_2728x2010_30P_Customer)/sizeof(SNR_AR1820_Reg_2728x2010_30P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_2728x2010_30P] 	  	= &SNR_AR1820_Reg_2728x2010_30P_Customer[0];

    //Mode6
    SensorCustFunc.OprTable->usSize[RES_IDX_3848x2168_15P]       	= (sizeof(SNR_AR1820_Reg_3848x2168_15P_Customer)/sizeof(SNR_AR1820_Reg_3848x2168_15P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_3848x2168_15P]     	= &SNR_AR1820_Reg_3848x2168_15P_Customer[0];

    //Mode7
    SensorCustFunc.OprTable->usSize[RES_IDX_3688x2776_15P]		  	= (sizeof(SNR_AR1820_Reg_3688x2776_15P_Customer)/sizeof(SNR_AR1820_Reg_3688x2776_15P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_3688x2776_15P] 	  	= &SNR_AR1820_Reg_3688x2776_15P_Customer[0];

    //Mode8
    SensorCustFunc.OprTable->usSize[RES_IDX_3688x2088_15P]       	= (sizeof(SNR_AR1820_Reg_3688x2088_15P_Customer)/sizeof(SNR_AR1820_Reg_3688x2088_15P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_3688x2088_15P]     	= &SNR_AR1820_Reg_3688x2088_15P_Customer[0];

    //Mode9
    SensorCustFunc.OprTable->usSize[RES_IDX_2824x2120_10P]       	= (sizeof(SNR_AR1820_Reg_2824x2120_10P_Customer)/sizeof(SNR_AR1820_Reg_2824x2120_10P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_2824x2120_10P]     	= &SNR_AR1820_Reg_2824x2120_10P_Customer[0];

    //Mode10
	SensorCustFunc.OprTable->usSize[RES_IDX_1848x1384_30P]		  	= (sizeof(SNR_AR1820_Reg_1848x1384_30P_Customer)/sizeof(SNR_AR1820_Reg_1848x1384_30P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1848x1384_30P] 	  	= &SNR_AR1820_Reg_1848x1384_30P_Customer[0];

    //Mode11
    SensorCustFunc.OprTable->usSize[RES_IDX_1848x1048_30P]       	= (sizeof(SNR_AR1820_Reg_1848x1048_30P_Customer)/sizeof(SNR_AR1820_Reg_1848x1048_30P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1848x1048_30P]     	= &SNR_AR1820_Reg_1848x1048_30P_Customer[0];

    //Mode12 
    SensorCustFunc.OprTable->usSize[RES_IDX_3848x2168_30P]       	= (sizeof(SNR_AR1820_Reg_3848x2168_30P_Customer)/sizeof(SNR_AR1820_Reg_3848x2168_30P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_3848x2168_30P]     	= &SNR_AR1820_Reg_3848x2168_30P_Customer[0];

    //Mode13
    SensorCustFunc.OprTable->usSize[RES_IDX_3848x1096_60P]       	= (sizeof(SNR_AR1820_Reg_3848x1096_60P_Customer)/sizeof(SNR_AR1820_Reg_3848x1096_60P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_3848x1096_60P]     	= &SNR_AR1820_Reg_3848x1096_60P_Customer[0];
    
	//Mode14
	SensorCustFunc.OprTable->usSize[RES_IDX_3848x1096_50P]			= (sizeof(SNR_AR1820_Reg_3848x1096_50P_Customer)/sizeof(SNR_AR1820_Reg_3848x1096_50P_Customer[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_3848x1096_50P]		= &SNR_AR1820_Reg_3848x1096_50P_Customer[0];

	//Mode15
	#if (OPTIMIZE_1080_30P)
	SensorCustFunc.OprTable->usSize[RES_IDX_3848x2168_30P_1080]		= (sizeof(SNR_AR1820_Reg_3848x2168_30P_Customer)/sizeof(SNR_AR1820_Reg_3848x2168_30P_Customer[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_3848x2168_30P_1080]	= &SNR_AR1820_Reg_3848x2168_30P_Customer[0];
	#else
	SensorCustFunc.OprTable->usSize[RES_IDX_3848x1096_30P]			= (sizeof(SNR_AR1820_Reg_3848x1096_30P_Customer)/sizeof(SNR_AR1820_Reg_3848x1096_30P_Customer[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_3848x1096_30P]		= &SNR_AR1820_Reg_3848x1096_30P_Customer[0];
	#endif

	//Mode16
    SensorCustFunc.OprTable->usSize[RES_IDX_1928x1096_120P]       	= (sizeof(SNR_AR1820_Reg_1928x1096_120P_Customer)/sizeof(SNR_AR1820_Reg_1928x1096_120P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1928x1096_120P]     	= &SNR_AR1820_Reg_1928x1096_120P_Customer[0];

	//Mode17
	SensorCustFunc.OprTable->usSize[RES_IDX_1928x1096_100P] 		= (sizeof(SNR_AR1820_Reg_1928x1096_100P_Customer)/sizeof(SNR_AR1820_Reg_1928x1096_100P_Customer[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_1928x1096_100P]		= &SNR_AR1820_Reg_1928x1096_100P_Customer[0];

	//Mode18
	SensorCustFunc.OprTable->usSize[RES_IDX_1928x1096_60P] 			= (sizeof(SNR_AR1820_Reg_1928x1096_60P_Customer)/sizeof(SNR_AR1820_Reg_1928x1096_60P_Customer[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_1928x1096_60P]		= &SNR_AR1820_Reg_1928x1096_60P_Customer[0];

	//Mode19
	SensorCustFunc.OprTable->usSize[RES_IDX_3688x1384_30P] 			= (sizeof(SNR_AR1820_Reg_3688x1384_30P_Customer)/sizeof(SNR_AR1820_Reg_3688x1384_30P_Customer[0]))/2;
	SensorCustFunc.OprTable->uspTable[RES_IDX_3688x1384_30P]		= &SNR_AR1820_Reg_3688x1384_30P_Customer[0];

    //Mode20 //The same as Mode12
    SensorCustFunc.OprTable->usSize[RES_IDX_3848x2168_30P_DSC]      = (sizeof(SNR_AR1820_Reg_3848x2168_30P_Customer)/sizeof(SNR_AR1820_Reg_3848x2168_30P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_3848x2168_30P_DSC]    = &SNR_AR1820_Reg_3848x2168_30P_Customer[0];

    //Mode21
    SensorCustFunc.OprTable->usSize[RES_IDX_3848x1096_25P]       	= (sizeof(SNR_AR1820_Reg_3848x1096_25P_Customer)/sizeof(SNR_AR1820_Reg_3848x1096_25P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_3848x1096_25P]     	= &SNR_AR1820_Reg_3848x1096_25P_Customer[0];

    //Mode22
    SensorCustFunc.OprTable->usSize[RES_IDX_1928x1096_50P]       	= (sizeof(SNR_AR1820_Reg_1928x1096_50P_Customer)/sizeof(SNR_AR1820_Reg_1928x1096_50P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1928x1096_50P]     	= &SNR_AR1820_Reg_1928x1096_50P_Customer[0];

    //Mode23
    SensorCustFunc.OprTable->usSize[RES_IDX_1928x1096_30P]       	= (sizeof(SNR_AR1820_Reg_1928x1096_30P_Customer)/sizeof(SNR_AR1820_Reg_1928x1096_30P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1928x1096_30P]     	= &SNR_AR1820_Reg_1928x1096_30P_Customer[0];

    //Mode24
    SensorCustFunc.OprTable->usSize[RES_IDX_1928x1096_25P]       	= (sizeof(SNR_AR1820_Reg_1928x1096_25P_Customer)/sizeof(SNR_AR1820_Reg_1928x1096_25P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1928x1096_25P]     	= &SNR_AR1820_Reg_1928x1096_25P_Customer[0];

	//Mode25
    SensorCustFunc.OprTable->usSize[RES_IDX_3848x2168_12d5P]       	= (sizeof(SNR_AR1820_Reg_3848x2168_12d5P_Customer)/sizeof(SNR_AR1820_Reg_3848x2168_12d5P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_3848x2168_12d5P]     	= &SNR_AR1820_Reg_3848x2168_12d5P_Customer[0];

	//Mode26
    SensorCustFunc.OprTable->usSize[RES_IDX_3848x2168_25P]       	= (sizeof(SNR_AR1820_Reg_3848x2168_25P_Customer)/sizeof(SNR_AR1820_Reg_3848x2168_25P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_3848x2168_25P]     	= &SNR_AR1820_Reg_3848x2168_25P_Customer[0];
     	
    // Init Vif Setting : Common
    SensorCustFunc.VifSetting->SnrType                              = MMPF_VIF_SNR_TYPE_BAYER;
#if (SENSOR_IF == SENSOR_IF_PARALLEL)
    SensorCustFunc.VifSetting->OutInterface                         = MMPF_VIF_IF_PARALLEL;
#elif (SENSOR_IF == SENSOR_IF_MIPI_1_LANE)
    SensorCustFunc.VifSetting->OutInterface                         = MMPF_VIF_IF_MIPI_SINGLE_0;
#elif (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)
    SensorCustFunc.VifSetting->OutInterface                         = MMPF_VIF_IF_MIPI_DUAL_01;
#elif (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)
    SensorCustFunc.VifSetting->OutInterface                         = MMPF_VIF_IF_MIPI_QUAD;
#endif
    #if (SUPPORT_DUAL_SNR)
    SensorCustFunc.VifSetting->VifPadId							    = MMPF_VIF_MDL_ID1;
    #else
	SensorCustFunc.VifSetting->VifPadId								= MMPF_VIF_MDL_ID0;
    #endif

    // Init Vif Setting : PowerOn Setting
    SensorCustFunc.VifSetting->powerOnSetting.bTurnOnExtPower       = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPin         = SENSOR_GPIO_ENABLE; // it might be defined in Config_SDK.h
    SensorCustFunc.VifSetting->powerOnSetting.bExtPowerPinHigh	    = SENSOR_GPIO_ENABLE_ACT_LEVEL;
    SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPinDelay    = 50;
    SensorCustFunc.VifSetting->powerOnSetting.bFirstEnPinHigh       = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOnSetting.ubFirstEnPinDelay     = 1;
    SensorCustFunc.VifSetting->powerOnSetting.bNextEnPinHigh        = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubNextEnPinDelay      = 10;
    SensorCustFunc.VifSetting->powerOnSetting.bTurnOnClockBeforeRst = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.bFirstRstPinHigh      = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOnSetting.ubFirstRstPinDelay    = 10;
    SensorCustFunc.VifSetting->powerOnSetting.bNextRstPinHigh       = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubNextRstPinDelay     = 10;

    // Init Vif Setting : PowerOff Setting
    SensorCustFunc.VifSetting->powerOffSetting.bEnterStandByMode    = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOffSetting.usStandByModeReg     = 0x301A;
    SensorCustFunc.VifSetting->powerOffSetting.usStandByModeMask    = 0x04;
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
    SensorCustFunc.VifSetting->clockAttr.ubClkSrc					= MMPF_VIF_SNR_CLK_SRC_PMCLK; 

    // Init Vif Setting : Parallel Sensor Setting
    SensorCustFunc.VifSetting->paralAttr.ubLatchTiming              = MMPF_VIF_SNR_LATCH_POS_EDGE;
    SensorCustFunc.VifSetting->paralAttr.ubHsyncPolarity            = MMPF_VIF_SNR_CLK_POLARITY_POS;
    SensorCustFunc.VifSetting->paralAttr.ubVsyncPolarity            = MMPF_VIF_SNR_CLK_POLARITY_NEG;
    SensorCustFunc.VifSetting->paralAttr.ubBusBitMode               = MMPF_VIF_SNR_PARAL_BITMODE_16;
    
    // Init Vif Setting : MIPI Sensor Setting
    SensorCustFunc.VifSetting->mipiAttr.bClkDelayEn                 = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bClkLaneSwapEn              = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.usClkDelay                  = 0;
    SensorCustFunc.VifSetting->mipiAttr.ubBClkLatchTiming           = MMPF_VIF_SNR_LATCH_NEG_EDGE;
#if (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)
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
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[0]              = 0;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[1]              = 0;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[2]              = 0;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[3]              = 0;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[0]             = 0x0F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[1]             = 0x0F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[2]             = 0x0F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[3]             = 0x0F;
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
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[0]            = MMPF_VIF_MIPI_DATA_SRC_PHY_1;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[1]            = MMPF_VIF_MIPI_DATA_SRC_PHY_2;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[2]            = MMPF_VIF_MIPI_DATA_SRC_PHY_UNDEF;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[3]            = MMPF_VIF_MIPI_DATA_SRC_PHY_UNDEF;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[0]              = 0;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[1]              = 0;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[2]              = 0;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[3]              = 0;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[0]             = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[1]             = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[2]             = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[3]             = 0x1F;
#endif

    // Init Vif Setting : Color ID Setting
    SensorCustFunc.VifSetting->colorId.VifColorId              	    = MMPF_VIF_COLORID_00;
	SensorCustFunc.VifSetting->colorId.CustomColorId.bUseCustomId   = MMP_TRUE;

	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_2568x1096_30P] 	= MMPF_VIF_COLORID_00;
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_2568x1096_50P] 	= MMPF_VIF_COLORID_00;
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_2568x1096_60P] 	= MMPF_VIF_COLORID_00;
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_1288x728_100P] 	= MMPF_VIF_COLORID_10;
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_1288x728_120P] 	= MMPF_VIF_COLORID_10;
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_2728x2010_30P] 	= MMPF_VIF_COLORID_00;
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_3848x2168_15P] 	= MMPF_VIF_COLORID_00;
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_3688x2776_15P] 	= MMPF_VIF_COLORID_00;
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_3688x2088_15P] 	= MMPF_VIF_COLORID_00;
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_2824x2120_10P] 	= MMPF_VIF_COLORID_00;
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_1848x1384_30P] 	= MMPF_VIF_COLORID_00;
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_1848x1048_30P] 	= MMPF_VIF_COLORID_00;
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_3848x2168_30P] 	= MMPF_VIF_COLORID_00;
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_3848x1096_60P] 	= MMPF_VIF_COLORID_00;
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_3848x1096_50P] 	= MMPF_VIF_COLORID_00;
#if (OPTIMIZE_1080_30P)
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_3848x2168_30P_1080] = MMPF_VIF_COLORID_00;
#else
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_3848x1096_30P] 	= MMPF_VIF_COLORID_00;
#endif
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_1928x1096_120P] 	= MMPF_VIF_COLORID_00;
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_1928x1096_100P] 	= MMPF_VIF_COLORID_00;
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_1928x1096_60P] 	= MMPF_VIF_COLORID_00;
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_3688x1384_30P] 	= MMPF_VIF_COLORID_00;
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_3848x2168_30P_DSC] = MMPF_VIF_COLORID_00;
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_3848x1096_25P] 	= MMPF_VIF_COLORID_00;
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_1928x1096_50P] 	= MMPF_VIF_COLORID_00;
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_1928x1096_30P] 	= MMPF_VIF_COLORID_00;
	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[RES_IDX_1928x1096_25P] 	= MMPF_VIF_COLORID_00;

	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_2568x1096_30P] 	= MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_2568x1096_50P] 	= MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_2568x1096_60P] 	= MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_1288x728_100P] 	= MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_1288x728_120P] 	= MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_2728x2010_30P] 	= MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_3848x2168_15P] 	= MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_3688x2776_15P] 	= MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_3688x2088_15P] 	= MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_2824x2120_10P] 	= MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_1848x1384_30P] 	= MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_1848x1048_30P] 	= MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_3848x2168_30P] 	= MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_3848x1096_60P] 	= MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_3848x1096_50P] 	= MMPF_VIF_COLORID_01;
#if (OPTIMIZE_1080_30P)
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_3848x2168_30P_1080] = MMPF_VIF_COLORID_01;
#else	
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_3848x1096_30P] 	= MMPF_VIF_COLORID_01;
#endif
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_1928x1096_120P] = MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_1928x1096_100P] = MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_1928x1096_60P] 	= MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_3688x1384_30P] 	= MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_3848x2168_30P_DSC] = MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_3848x1096_25P] 	= MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_1928x1096_50P] 	= MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_1928x1096_30P] 	= MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_1928x1096_25P] 	= MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_3848x2168_12d5P]= MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[RES_IDX_3848x2168_25P] 	= MMPF_VIF_COLORID_01;
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_DoAE_FrmSt
//  Description :
//------------------------------------------------------------------------------
MMP_USHORT gsFixIsoMode = 0;	//0: auto, 1: iso100, 2: iso200, 3: iso400, 4: iso800, 5: iso1600, 6:iso3200 , 7:iso6400

void SNR_Cust_DoAE_FrmSt(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
#if (ISP_EN)
	static ISP_UINT16 ae_gain;
	static ISP_UINT16 ae_shutter;
	static ISP_UINT16 ae_vsync;
	ISP_UINT32 ae_energy;
	ISP_UINT32 i, j;
    MMP_USHORT  usRdVal;

    gsSensorFunction->MMPF_Sensor_GetReg(ubSnrSel, 0x3040, &usRdVal);
	
	if ((usRdVal & 0x2000) == 0)
		ISP_IF_AE_SetSysMode(0); 	
	else
		ISP_IF_AE_SetSysMode(1); 

    #if (ENABLE_CUSTOMIZED_AE == 1)
    if (ISP_IF_3A_GetSwitch( ISP_3A_ALGO_AE ) == 0)
    {
        return;
    }
    #endif
	
	if (ulFrameCnt % m_AeTime.ubPeriod == 0) {
		ISP_IF_AE_Execute();
		
		ae_gain    = ISP_IF_AE_GetGain();
		ae_shutter = ISP_IF_AE_GetShutter();
		ae_vsync   = ISP_IF_AE_GetVsync();
		
		//============= Fix ISO Test ======================
		if (gsFixIsoMode != 0) {
			j = 1;
			for (i=0; i<gsFixIsoMode-1; i++) {
				j *=2; 
			}

			ae_energy  = ae_gain * ae_shutter;
			ae_gain    = j * ISP_IF_AE_GetGainBase();
			ae_shutter = ae_energy / ae_gain;
			
			ISP_IF_AE_SetGain(ae_gain, ISP_IF_AE_GetGainBase());
			ISP_IF_AE_SetShutter(ae_shutter, ISP_IF_AE_GetShutterBase());
		}
	}
		
	if (ulFrameCnt % m_AeTime.ubPeriod == m_AeTime.ubFrmStSetShutFrmCnt)
		gsSensorFunction->MMPF_Sensor_SetShutter(ubSnrSel, ae_shutter, ae_vsync);
	
    if (ulFrameCnt % m_AeTime.ubPeriod == m_AeTime.ubFrmStSetGainFrmCnt)
    	gsSensorFunction->MMPF_Sensor_SetGain(ubSnrSel, ae_gain);

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
	
	ISP_UINT16 sensor_again_fine;
	ISP_UINT16 sensor_again_fine_db;
	ISP_UINT16 sensor_again_coarse;
	ISP_UINT16 sensor_again_coarse_db;
	ISP_UINT16 sensor_dgain;
	ISP_UINT16 ae_gain = ulGain;
	MMP_USHORT  usRdVal;
	
	static ISP_UINT16 sensor_gain_type     = 0;
	static ISP_UINT16 sensor_gain_type_pre = 255;
	static ISP_UINT16 update_sensor_gain   = 1;

    gsSensorFunction->MMPF_Sensor_GetReg(ubSnrSel, 0x3040, &usRdVal);
	
	// original size scale down
	if((usRdVal & 0x2000) == 0)
	{
		ISP_IF_AE_SetAdditionMode(0);
	
		if (ae_gain < 8 * ISP_IF_AE_GetGainBase())
			sensor_gain_type = 0;
		else if (ae_gain < 16 * ISP_IF_AE_GetGainBase())
			sensor_gain_type = 1;
		else
			sensor_gain_type = 2;
		
		if (sensor_gain_type_pre != sensor_gain_type)
			update_sensor_gain = 1;
		
		if (update_sensor_gain == 1)
		{	
			switch (sensor_gain_type) {
			case 0:
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3ED2, 0x449D);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3EDC, 0x01C9);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3EE0, 0x1558);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3EC8, 0x0074);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3ED8, 0x60A0);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3ECC, 0x7386);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3F1A, 0x0F04);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3F44, 0x0C0C);
				update_sensor_gain = 0;
				break;
			case 1:
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3ED2, 0x449D);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3EDC, 0x01C9);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3EE0, 0x1558);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3EC8, 0x0074);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3ED8, 0x60A0);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3ECC, 0x7586);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3F1A, 0x1E1E);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3F44, 0x0708);
				update_sensor_gain = 0;
				break;
			case 2:
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3ED2, 0x449D);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3EDC, 0x01C9);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3EE0, 0x1558);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3EC8, 0x0074);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3ED8, 0x60A0);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3ECC, 0x7586);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3F1A, 0x1919);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3F44, 0x0708);
				update_sensor_gain = 0;
				break;			
			}
		}	
		
		sensor_gain_type_pre = sensor_gain_type;
		
		// Sensor Gain Mapping Real Size
		if(ae_gain < 8 * ISP_IF_AE_GetGainBase()) {
		
			if(ae_gain < 2 * ISP_IF_AE_GetGainBase()) { 		// 1x ~ 2x
				sensor_again_coarse = 1;
				sensor_again_coarse_db = 1 * ISP_IF_AE_GetGainBase();
			}
			else if (ae_gain < 3 * ISP_IF_AE_GetGainBase()) {	// 2x ~ 3x
				sensor_again_coarse = 2;
				sensor_again_coarse_db = 2 * ISP_IF_AE_GetGainBase();
			}
			else if (ae_gain < 4 * ISP_IF_AE_GetGainBase()) {	// 3x ~ 4x
				sensor_again_coarse = 6;
				sensor_again_coarse_db = 3 * ISP_IF_AE_GetGainBase();
			}
			else {												// 4x ~ 8x
				sensor_again_coarse = 3;
				sensor_again_coarse_db = 4 * ISP_IF_AE_GetGainBase();
			}
			
			ae_gain = ae_gain * ISP_IF_AE_GetGainBase() / sensor_again_coarse_db;
			
			sensor_again_fine = 32 - (32 * ISP_IF_AE_GetGainBase() / ae_gain) ;
			sensor_again_fine = (sensor_again_fine == 0) ? (0) : (sensor_again_fine-1);
			sensor_again_fine_db = (32 * ISP_IF_AE_GetGainBase()) / (32 - sensor_again_fine) ;
			
			sensor_dgain = ae_gain * 64 / sensor_again_fine_db ;
			sensor_dgain = ISP_MIN(ISP_MAX(sensor_dgain, 0x40), 0x1ff);
		}
		else {													// 8x ~ 64x(again 8x, dgain 8x)
			// if crose = 8x, fine gain need to 1x, and use dgain.
			sensor_again_coarse = 4;
			sensor_again_coarse_db = 8 * ISP_IF_AE_GetGainBase();
			
			sensor_again_fine = 0;
			sensor_again_fine_db = 1 * ISP_IF_AE_GetGainBase();
			
			sensor_dgain = ae_gain * 64 * ISP_IF_AE_GetGainBase() / sensor_again_coarse_db / sensor_again_fine_db ;  
			sensor_dgain = ISP_MIN(ISP_MAX(sensor_dgain, 0x40), 0x1ff);
		}
	}
	else
	{		
		ISP_IF_AE_SetAdditionMode(1);
		
		if (ae_gain < 2 * ISP_IF_AE_GetGainBase())
			sensor_gain_type = 0;
		else if (ae_gain < 8 * ISP_IF_AE_GetGainBase())
			sensor_gain_type = 1;
		else if (ae_gain < 16 * ISP_IF_AE_GetGainBase())
			sensor_gain_type = 2;
		else
			sensor_gain_type = 3;
		
		if (sensor_gain_type_pre != sensor_gain_type)
			update_sensor_gain = 1;
		
		// set sensor opr
		if (update_sensor_gain == 1) {
			
			switch (sensor_gain_type) {
			case 0:
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3ED2, 0xE49D);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3EDC, 0x01C9);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3EE0, 0x9518);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3EC8, 0x00A4);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3ED8, 0x60B0);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3ECC, 0x7386);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3F1A, 0x0404);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3F44, 0x0101);
				update_sensor_gain = 0;
				break;
			case 1:
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3ED2, 0x449D);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3EDC, 0x01C9);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3EE0, 0x1558);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3EC8, 0x0074);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3ED8, 0x60A0);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3ECC, 0x7786);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3F1A, 0x0404);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3F44, 0x0101);
				update_sensor_gain = 0;
				break;
			case 2:
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3ED2, 0x449D);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3EDC, 0x01C8);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3EE0, 0x1558);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3EC8, 0x0074);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3ED8, 0x60A0);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3ECC, 0x7786);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3F1A, 0x0504);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3F44, 0x0101);
				update_sensor_gain = 0;
				break;
			case 3:
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3ED2, 0x449D);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3EDC, 0x01C8);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3EE0, 0x1558);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3EC8, 0x0074);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3ED8, 0x60A0);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3ECC, 0x7786);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3F1A, 0x0609);
				gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3F44, 0x0101);
				update_sensor_gain = 0;
				break;			
			}
		}	
		
		sensor_gain_type_pre = sensor_gain_type;
		
		// calu sensor gain
		if (ulGain < 2 * ISP_IF_AE_GetGainBase()) {				// 1x ~ 2x
			
			sensor_again_coarse = 1;
			sensor_again_coarse_db = 0.705 * ISP_IF_AE_GetGainBase();
					
			ae_gain = ae_gain * ISP_IF_AE_GetGainBase() / sensor_again_coarse_db;
			
			sensor_again_fine = 0;
			sensor_again_fine_db = (32 * ISP_IF_AE_GetGainBase()) / (32 - sensor_again_fine) ;
			
			sensor_dgain = ae_gain * 64 / sensor_again_fine_db ;  
			sensor_dgain = ISP_MIN(ISP_MAX(sensor_dgain, 0x5B), 0x1ff);
		}
		else if(ae_gain < 8 * ISP_IF_AE_GetGainBase()) {
		
			if (ae_gain < 4 * ISP_IF_AE_GetGainBase()) {			// 2x ~ 4x
				sensor_again_coarse = 2;
				sensor_again_coarse_db = 2 * ISP_IF_AE_GetGainBase();
			}
			else if (ae_gain < 8 * ISP_IF_AE_GetGainBase()) {	// 4x ~ 8x
				sensor_again_coarse = 3;
				sensor_again_coarse_db = 4 * ISP_IF_AE_GetGainBase();
			}		
			
			ae_gain = ae_gain * ISP_IF_AE_GetGainBase() / sensor_again_coarse_db;
			
			sensor_again_fine = 32- (32 * ISP_IF_AE_GetGainBase() / ae_gain);
			sensor_again_fine = (sensor_again_fine == 0) ? (0) : (sensor_again_fine-1);
			sensor_again_fine_db = (32 * ISP_IF_AE_GetGainBase()) / (32 - sensor_again_fine) ;
			
			sensor_dgain = ae_gain * 64 / sensor_again_fine_db;
			sensor_dgain = ISP_MIN(ISP_MAX(sensor_dgain, 0x40), 0x1ff);
		}
		else {													// 8 ~ 64x(again 8x, dgain 8x)
			// if crose = 8x, fine gain need to 1x, and use dgain.
			sensor_again_coarse = 4;
			sensor_again_coarse_db = 8 * ISP_IF_AE_GetGainBase();
			
			sensor_again_fine = 0;
			sensor_again_fine_db = 1 * ISP_IF_AE_GetGainBase();
			
			sensor_dgain = ae_gain * 64 * ISP_IF_AE_GetGainBase() / sensor_again_coarse_db / sensor_again_fine_db ;  
			sensor_dgain = ISP_MIN(ISP_MAX(sensor_dgain, 0x40), 0x1ff);
		}
	}

	sensor_again_coarse = sensor_again_coarse | (sensor_again_fine << 3) | (sensor_dgain << 7) ;
	
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x305E, sensor_again_coarse);
	
#endif
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetShutter
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetShutter(MMP_UBYTE ubSnrSel, MMP_ULONG shutter, MMP_ULONG vsync)
{
#if (ISP_EN)
	ISP_UINT32 new_vsync    = gSnrLineCntPerSec[ubSnrSel] * vsync / ISP_IF_AE_GetVsyncBase();
	ISP_UINT32 new_shutter  = gSnrLineCntPerSec[ubSnrSel] * shutter / ISP_IF_AE_GetShutterBase();
    
	new_vsync   = ISP_MIN(ISP_MAX(new_shutter + 3, new_vsync), 0xFFFF);
	new_shutter = ISP_MIN(ISP_MAX(new_shutter, 1), new_vsync - 3);

	// this sensor don't need to config sensor vsync.
	//gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x300A, new_vsync);
	
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3012, new_shutter);	
#endif	
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetFlip
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetFlip(MMP_UBYTE ubSnrSel, MMP_UBYTE ubMode)
{
    MMP_USHORT  usRdVal;
    
	if (ubMode == MMPF_SENSOR_NO_FLIP) {
	    gsSensorFunction->MMPF_Sensor_GetReg(ubSnrSel, 0x3040, &usRdVal);
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3040, usRdVal|0xC000);
	}	
	else if (ubMode == MMPF_SENSOR_COLROW_FLIP) {
	    gsSensorFunction->MMPF_Sensor_GetReg(ubSnrSel, 0x3040, &usRdVal);
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3040, usRdVal&0x3FFF);
	}
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
    MMP_USHORT usRdVal = 0;
    
    /* Initial I2cm */
    MMPF_I2cm_Initialize(&m_I2cmAttr);
    MMPF_OS_SleepMs(20);
    
    if (gsSensorFunction->MMPF_Sensor_GetReg(ubSnrSel, 0x3000, &usRdVal)) {
        //I2C Error!!
        m_I2cmAttr.ubSlaveAddr = 0x36;
    }
    else {
        if(usRdVal != 0x0050) {
            m_I2cmAttr.ubSlaveAddr = 0x36;
        }   
    }
    printc("Sensor I2C ubSlaveAddr = 0x%X\r\n", m_I2cmAttr.ubSlaveAddr);
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

#endif // (BIND_SENSOR_AR1820)
#endif // (SENSOR_EN)

//==============================================================================
//
//  File        : sensor_ar0330.c
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
#if (BIND_SENSOR_AR0330)

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
	3,				// ubSensorModeNum
	0,				// ubDefPreviewMode
	1,				// ubDefCaptureMode
	2200,           // usPixelSize
//  Mode0	Mode1   Mode2
    {1,     1,      1	},	// usVifGrabStX
    {1,     1,      1	},	// usVifGrabStY
    #if (PARALLEL_SENSOR_CROP_2M == 1)
    {1928,  1928,   1156},	// usVifGrabW
    {1092,  1092	652	},	// usVifGrabH
    #else
    {2312,  2311,   1156},	// usVifGrabW
    {1300,  1540,	652	},	// usVifGrabH
    #endif
    
    #if (CHIP == MCR_V2)
    {1,   	1, 		1	},  // usBayerInGrabX
    {1,  	1,      1	},  // usBayerInGrabY
    #if (PARALLEL_SENSOR_CROP_2M == 1)
    {8,   	263, 	4	},  // usBayerInDummyX
    {12,  	4,      4	},  // usBayerInDummyY
    {1920,  1920,   1152},	// usBayerOutW
    {1080,  1440, 	648	},	// usBayerOutH
    #else
    {8,   	263,  	4	},  // usBayerInDummyX
    {4,  	4,      4	},  // usBayerInDummyY
    {2304,  2048,   1152},	// usBayerOutW
    {1296,  1536,	648	},	// usBayerOutH
    #endif
    #endif

    #if (PARALLEL_SENSOR_CROP_2M == 1)
    {1920,  1920, 	800	},	// usScalInputW
    {1080,  1440, 	600	},	// usScalInputH
    #else
    #if (SUPPORT_EIS)
    {1920,  2048, 	800	},	// usScalInputW
    {1080,  1536, 	600	},	// usScalInputH
    #else
    {2304,  2048, 	800	},	// usScalInputW
    {1296,  1536, 	600	},	// usScalInputH
    #endif
    #endif
    
	#if (SENSOR_IF == SENSOR_IF_PARALLEL) || (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)
    {300, 	250,  	600	},	// usTargetFpsx10
	#else
    {600, 	250, 	600	},	// usTargetFpsx10
	#endif
    #if (PARALLEL_SENSOR_CROP_2M == 1)
    {1310,  1310,	672	},	// usVsyncLine
    #else
    {1329,  1540,	672	},	// usVsyncLine
    #endif
    {1,   	1, 		1	},  // ubWBinningN
    {1,   	1,      1	},  // ubWBinningN
    {1,   	1,      1	},  // ubWBinningN
    {1,   	1,      1	},  // ubWBinningN
    {0xFF,	0xFF,   0xFF},  // ubCustIQmode
    {0xFF,	0xFF,   0xFF}   // ubCustAEmode
};

// OPR Table and Vif Setting
MMPF_SENSOR_OPR_TABLE       m_OprTable;
MMPF_SENSOR_VIF_SETTING     m_VifSetting;

// IQ Table
const ISP_UINT8 Sensor_IQ_CompressedText[] = 
{
#include "isp_8590_iq_data_v2_AR0330_v1.xls.ciq.txt"
};

// I2cm Attribute
static MMP_I2CM_ATTR m_I2cmAttr = {
    MMP_I2CM0, 	// i2cmID
#if (SENSOR_IF == SENSOR_IF_PARALLEL)
    0x18,       // ubSlaveAddr
#else
    0x10,       // ubSlaveAddr
#endif
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
    150000,     // ulI2cmSpeed
    MMP_TRUE,   // bOsProtectEn
    NULL,       // sw_clk_pin
    NULL,       // sw_data_pin
    MMP_FALSE,  // bRfclModeEn
	MMP_FALSE,      // bWfclModeEn
	MMP_FALSE,		// bRepeatModeEn
    0               // ubVifPioMdlId
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
	6, 	/* ubPeriod */
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

#if (SENSOR_IF == SENSOR_IF_PARALLEL)

ISP_UINT16 SNR_AR0330_Reg_Init_Customer[] = 
{
    0x301A, 0x10DC,		// RESET_REGISTER
    0x301A, 0x10D8,		// RESET_REGISTER
	0x302A,      6,		// VT_PIX_CLK_DIV
	0x302C,      1,		// VT_SYS_CLK_DIV
	0x302E,      2,		// PRE_PLL_CLK_DIV
	0x3030,     49,		// PLL_MULTIPLIER
	0x3036,     12,		// OP_PIX_CLK_DIV
	0x3038,      1,		// OP_SYS_CLK_DIV
	0x31AC, 0x0C0C,		// DATA_FORMAT_BITS
	0x301A, 0x10DC, 	// RESET_REGISTER
	0x31AE, 0x0301,  	// SERIAL_FORMAT
	0x3002,    234,		// Y_ADDR_START
	0x3004,    198, 	// X_ADDR_START
	0x3006,   1313+16,	// Y_ADDR_END
	0x3008,   2117+8, 	// X_ADDR_END
	0x300A,   1315,		// FRAME_LENGTH_LINES
	0x300C,   1242, 	// LINE_LENGTH_PCK
	0x3012,    657,		// COARSE_INTEGRATION_TIME
	0x3014,      0,		// FINE_INTEGRATION_TIME
	0x30A2,      1,		// X_ODD_INC
	0x30A6,      1,		// Y_ODD_INC
	0x308C,      6,		// Y_ADDR_START_CB
	0x308A,      6,		// X_ADDR_START_CB
	0x3090,   1541,		// Y_ADDR_END_CB
	0x308E,   2309,		// X_ADDR_END_CB
	0x30AA,   1570,		// FRAME_LENGTH_LINES_CB
	0x303E,   1248,		// LINE_LENGTH_PCK_CB
	0x3016,   1569,		// COARSE_INTEGRATION_TIME_CB
	0x3018,      0,		// FINE_INTEGRATION_TIME_CB
	0x30AE,      1,		// X_ODD_INC_CB
	0x30A8,      1,		// Y_ODD_INC_CB
	0x3040, 0x0000,  	// READ_MODE
	0x3042,    103,		// EXTRA_DELAY
	0x30BA, 0x002C,  	// DIGITAL_CTRL
	0x3088, 0x80BA, 	// SEQ_CTRL_PORT
	0x3086, 0xE653,  	// SEQ_DATA_PORT
	0x306E, 0xFC10,  
    SENSOR_DELAY_REG,    100  
};

#elif (SENSOR_IF == SENSOR_IF_MIPI_1_LANE)

ISP_UINT16 SNR_AR0330_Reg_Init_Customer[] = 
{
	0x31AE, 0x0201,
	0x301A, 0x0058,     // Disable streaming

	SENSOR_DELAY_REG, 10,

	0x31AE, 0x0201,     // Output 1-lane MIPI
	
	//Configure for Serial Interface
	0x301A, 0x0058,     // Drive Pins,Parallel Enable,SMIA Serializer Disable
	0x3064, 0x1802, 	// Disable Embedded Data
	
	//Optimized Gain Configuration
	0x3EE0, 0x1500, 	// DAC_LD_20_21
	0x3EEA, 0x001D,
	0x31E0, 0x1E01,
	0x3F06, 0x046A,
	0x3ED2, 0x0186,	
	0x3ED4, 0x8F2C,	
	0x3ED6, 0x2244,	
	0x3ED8, 0x6442,	
	0x30BA, 0x002C,     // Dither enable
	0x3046, 0x4038,		// Enable Flash Pin
	0x3048, 0x8480,		// Flash Pulse Length
    0x3ED0, 0x0016, 	// DAC_LD_4_5
	0x3ED0, 0x0036, 	// DAC_LD_4_5
	0x3ED0, 0x0076, 	// DAC_LD_4_5
    0x3ED0, 0x00F6, 	// DAC_LD_4_5
	0x3ECE, 0x1003, 	// DAC_LD_2_3
	0x3ECE, 0x100F, 	// DAC_LD_2_3
	0x3ECE, 0x103F, 	// DAC_LD_2_3
    0x3ECE, 0x10FF, 	// DAC_LD_2_3
	0x3ED0, 0x00F6, 	// DAC_LD_4_5
	0x3ED0, 0x04F6, 	// DAC_LD_4_5
	0x3ED0, 0x24F6, 	// DAC_LD_4_5
	0x3ED0, 0xE4F6, 	// DAC_LD_4_5
	0x3EE6, 0xA480, 	// DAC_LD_26_27
	0x3EE6, 0xA080, 	// DAC_LD_26_27
	0x3EE6, 0x8080, 	// DAC_LD_26_27
	0x3EE6, 0x0080, 	// DAC_LD_26_27
	0x3EE6, 0x0080, 	// DAC_LD_26_27
	0x3EE8, 0x2024, 	// DAC_LD_28_29
	0x30FE, 128,        // Noise Pedestal of 128

	//Assuming Input Clock of 24MHz.  Output Clock will be 70Mpixel/s
	0x302A, 0x0005, 	// VT_PIX_CLK_DIV
	0x302C, 0x0004, 	// VT_SYS_CLK_DIV
	0x302E, 0x0002, 	// PRE_PLL_CLK_DIV
	0x3030, 0x0040, 	// PLL_MULTIPLIER
	0x3036, 0x000A, 	// OP_PIX_CLK_DIV
	0x3038, 0x0001, 	// OP_SYS_CLK_DIV
	0x31AC, 0x0A0A, 	// DATA_FORMAT_BITS
	
	//MIPI TIMING
	0x31B0, 40,         // FRAME PREAMBLE
	0x31B2, 14,         // LINE PREAMBLE
	0x31B4, 0x2743,     // MIPI TIMING 0
	0x31B6, 0x114E,     // MIPI TIMING 1
	0x31B8, 0x2049,     // MIPI TIMING 2
	0x31BA, 0x0186,     // MIPI TIMING 3
	0x31BC, 0x8005,     // MIPI TIMING 4
	0x31BE, 0x2003,     // MIPI CONFIG STATUS

	//Sequencer
    0x3088, 0x8000,
    0x3086, 0x4540,
    0x3086, 0x6134,
    0x3086, 0x4A31,
    0x3086, 0x4342,
    0x3086, 0x4560,
    0x3086, 0x2714,
    0x3086, 0x3DFF,
    0x3086, 0x3DFF,
    0x3086, 0x3DEA,
    0x3086, 0x2704,
    0x3086, 0x3D10,
    0x3086, 0x2705,
    0x3086, 0x3D10,
    0x3086, 0x2715,
    0x3086, 0x3527,
    0x3086, 0x053D,
    0x3086, 0x1045,
    0x3086, 0x4027,
    0x3086, 0x0427,
    0x3086, 0x143D,
    0x3086, 0xFF3D,
    0x3086, 0xFF3D,
    0x3086, 0xEA62,
    0x3086, 0x2728,
    0x3086, 0x3627,
    0x3086, 0x083D,
    0x3086, 0x6444,
    0x3086, 0x2C2C,
    0x3086, 0x2C2C,
    0x3086, 0x4B01,
    0x3086, 0x432D,
    0x3086, 0x4643,
    0x3086, 0x1647,
    0x3086, 0x435F,
    0x3086, 0x4F50,
    0x3086, 0x2604,
    0x3086, 0x2684,
    0x3086, 0x2027,
    0x3086, 0xFC53,
    0x3086, 0x0D5C,
    0x3086, 0x0D60,
    0x3086, 0x5754,
    0x3086, 0x1709,
    0x3086, 0x5556,
    0x3086, 0x4917,
    0x3086, 0x145C,
    0x3086, 0x0945,
    0x3086, 0x0045,
    0x3086, 0x8026,
    0x3086, 0xA627,
    0x3086, 0xF817,
    0x3086, 0x0227,
    0x3086, 0xFA5C,
    0x3086, 0x0B5F,
    0x3086, 0x5307,
    0x3086, 0x5302,
    0x3086, 0x4D28,
    0x3086, 0x644C,
    0x3086, 0x0928,
    0x3086, 0x2428,
    0x3086, 0x234E,
    0x3086, 0x1718,
    0x3086, 0x26A2,
    0x3086, 0x5C03,
    0x3086, 0x1744,
    0x3086, 0x27F2,
    0x3086, 0x1708,
    0x3086, 0x2803,
    0x3086, 0x2808,
    0x3086, 0x4D1A,
    0x3086, 0x27FA,
    0x3086, 0x2683,
    0x3086, 0x45A0,
    0x3086, 0x1707,
    0x3086, 0x27FB,
    0x3086, 0x1729,
    0x3086, 0x4580,
    0x3086, 0x1708,
    0x3086, 0x27FA,
    0x3086, 0x1728,
    0x3086, 0x2682,
    0x3086, 0x5D17,
    0x3086, 0x0E48,
    0x3086, 0x4D4E,
    0x3086, 0x2803,
    0x3086, 0x4C0B,
    0x3086, 0x175F,
    0x3086, 0x27F2,
    0x3086, 0x170A,
    0x3086, 0x2808,
    0x3086, 0x4D1A,
    0x3086, 0x27FA,
    0x3086, 0x2602,
    0x3086, 0x5C00,
    0x3086, 0x4540,
    0x3086, 0x2798,
    0x3086, 0x172A,
    0x3086, 0x4A0A,
    0x3086, 0x4316,
    0x3086, 0x0B43,
    0x3086, 0x279C,
    0x3086, 0x4560,
    0x3086, 0x1707,
    0x3086, 0x279D,
    0x3086, 0x1725,
    0x3086, 0x4540,
    0x3086, 0x1708,
    0x3086, 0x2798,
    0x3086, 0x5D53,
    0x3086, 0x0026,
    0x3086, 0x445C,
    0x3086, 0x014b,
    0x3086, 0x1244,
    0x3086, 0x5251,
    0x3086, 0x1702,
    0x3086, 0x6018,
    0x3086, 0x4A03,
    0x3086, 0x4316,
    0x3086, 0x0443,
    0x3086, 0x1658,
    0x3086, 0x4316,
    0x3086, 0x5943,
    0x3086, 0x165A,
    0x3086, 0x4316,
    0x3086, 0x5B43,
    0x3086, 0x4540,
    0x3086, 0x279C,
    0x3086, 0x4560,
    0x3086, 0x1707,
    0x3086, 0x279D,
    0x3086, 0x1725,
    0x3086, 0x4540,
    0x3086, 0x1710,
    0x3086, 0x2798,
    0x3086, 0x1720,
    0x3086, 0x224B,
    0x3086, 0x1244,
    0x3086, 0x2C2C,
    0x3086, 0x2C2C
};

#elif (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)

MMP_USHORT SNR_AR0330_Reg_Init_Customer[] = 
{
	0x31AE, 0x0202,
	0x301A, 0x0058,     // Disable streaming
	
	SENSOR_DELAY_REG, 10,
	
	0x31AE, 0x202,      // Output 2-lane MIPI
	
	//Configure for Serial Interface
	0x301A, 0x0058,     // Drive Pins,Parallel Enable,SMIA Serializer Disable
	0x3064, 0x1802, 	// Disable Embedded Data
	
	//Optimized Gain Configuration
    0x3EE0, 0x1500, 	// DAC_LD_20_21
	0x3EEA, 0x001D,
	0x31E0, 0x1E01,
	0x3F06, 0x046A,
	0x3ED2, 0x0186,
	0x3ED4, 0x8F2C,	
	0x3ED6, 0x2244,	
	0x3ED8, 0x6442,	
	0x30BA, 0x002C,     // Dither enable
	0x3046, 0x4038,		// Enable Flash Pin
	0x3048, 0x8480,		// Flash Pulse Length
	0x3ED0, 0x0016, 	// DAC_LD_4_5
	0x3ED0, 0x0036, 	// DAC_LD_4_5
	0x3ED0, 0x0076, 	// DAC_LD_4_5
	0x3ED0, 0x00F6, 	// DAC_LD_4_5
	0x3ECE, 0x1003, 	// DAC_LD_2_3
	0x3ECE, 0x100F, 	// DAC_LD_2_3
	0x3ECE, 0x103F, 	// DAC_LD_2_3
	0x3ECE, 0x10FF, 	// DAC_LD_2_3
	0x3ED0, 0x00F6, 	// DAC_LD_4_5
	0x3ED0, 0x04F6, 	// DAC_LD_4_5
	0x3ED0, 0x24F6, 	// DAC_LD_4_5
	0x3ED0, 0xE4F6, 	// DAC_LD_4_5
	0x3EE6, 0xA480, 	// DAC_LD_26_27
	0x3EE6, 0xA080, 	// DAC_LD_26_27
	0x3EE6, 0x8080, 	// DAC_LD_26_27
	0x3EE6, 0x0080, 	// DAC_LD_26_27
	0x3EE6, 0x0080, 	// DAC_LD_26_27
	0x3EE8, 0x2024, 	// DAC_LD_28_29
	0x30FE, 128,        // Noise Pedestal of 128

	//Assuming Input Clock of 24MHz.  Output Clock will be 70Mpixel/s
	0x302A, 0x0005, 	// VT_PIX_CLK_DIV
	0x302C, 0x0002, 	// VT_SYS_CLK_DIV
	0x302E, 0x0004, 	// PRE_PLL_CLK_DIV
	0x3030, 82, 	    // PLL_MULTIPLIER
	0x3036, 0x000A, 	// OP_PIX_CLK_DIV
	0x3038, 0x0001, 	// OP_SYS_CLK_DIV
	0x31AC, 0x0A0A, 	// DATA_FORMAT_BITS
	
	//MIPI TIMING
	0x31B0, 40,         // FRAME PREAMBLE
	0x31B2, 14,         // LINE PREAMBLE
	0x31B4, 0x2743,     // MIPI TIMING 0
	0x31B6, 0x114E,     // MIPI TIMING 1
	0x31B8, 0x2049,     // MIPI TIMING 2
	0x31BA, 0x0186,     // MIPI TIMING 3
	0x31BC, 0x8005,     // MIPI TIMING 4
	0x31BE, 0x2003,     // MIPI CONFIG STATUS

	//Sequencer
    0x3088, 0x8000,
    0x3086, 0x4540,
    0x3086, 0x6134,
    0x3086, 0x4A31,
    0x3086, 0x4342,
    0x3086, 0x4560,
    0x3086, 0x2714,
    0x3086, 0x3DFF,
    0x3086, 0x3DFF,
    0x3086, 0x3DEA,
    0x3086, 0x2704,
    0x3086, 0x3D10,
    0x3086, 0x2705,
    0x3086, 0x3D10,
    0x3086, 0x2715,
    0x3086, 0x3527,
    0x3086, 0x053D,
    0x3086, 0x1045,
    0x3086, 0x4027,
    0x3086, 0x0427,
    0x3086, 0x143D,
    0x3086, 0xFF3D,
    0x3086, 0xFF3D,
    0x3086, 0xEA62,
    0x3086, 0x2728,
    0x3086, 0x3627,
    0x3086, 0x083D,
    0x3086, 0x6444,
    0x3086, 0x2C2C,
    0x3086, 0x2C2C,
    0x3086, 0x4B01,
    0x3086, 0x432D,
    0x3086, 0x4643,
    0x3086, 0x1647,
    0x3086, 0x435F,
    0x3086, 0x4F50,
    0x3086, 0x2604,
    0x3086, 0x2684,
    0x3086, 0x2027,
    0x3086, 0xFC53,
    0x3086, 0x0D5C,
    0x3086, 0x0D60,
    0x3086, 0x5754,
    0x3086, 0x1709,
    0x3086, 0x5556,
    0x3086, 0x4917,
    0x3086, 0x145C,
    0x3086, 0x0945,
    0x3086, 0x0045,
    0x3086, 0x8026,
    0x3086, 0xA627,
    0x3086, 0xF817,
    0x3086, 0x0227,
    0x3086, 0xFA5C,
    0x3086, 0x0B5F,
    0x3086, 0x5307,
    0x3086, 0x5302,
    0x3086, 0x4D28,
    0x3086, 0x644C,
    0x3086, 0x0928,
    0x3086, 0x2428,
    0x3086, 0x234E,
    0x3086, 0x1718,
    0x3086, 0x26A2,
    0x3086, 0x5C03,
    0x3086, 0x1744,
    0x3086, 0x27F2,
    0x3086, 0x1708,
    0x3086, 0x2803,
    0x3086, 0x2808,
    0x3086, 0x4D1A,
    0x3086, 0x27FA,
    0x3086, 0x2683,
    0x3086, 0x45A0,
    0x3086, 0x1707,
    0x3086, 0x27FB,
    0x3086, 0x1729,
    0x3086, 0x4580,
    0x3086, 0x1708,
    0x3086, 0x27FA,
    0x3086, 0x1728,
    0x3086, 0x2682,
    0x3086, 0x5D17,
    0x3086, 0x0E48 ,
    0x3086, 0x4D4E,
    0x3086, 0x2803,
    0x3086, 0x4C0B,
    0x3086, 0x175F,
    0x3086, 0x27F2,
    0x3086, 0x170A,
    0x3086, 0x2808,
    0x3086, 0x4D1A,
    0x3086, 0x27FA,
    0x3086, 0x2602,
    0x3086, 0x5C00,
    0x3086, 0x4540,
    0x3086, 0x2798,
    0x3086, 0x172A,
    0x3086, 0x4A0A,
    0x3086, 0x4316,
    0x3086, 0x0B43,
    0x3086, 0x279C,
    0x3086, 0x4560,
    0x3086, 0x1707,
    0x3086, 0x279D,
    0x3086, 0x1725,
    0x3086, 0x4540,
    0x3086, 0x1708,
    0x3086, 0x2798,
    0x3086, 0x5D53,
    0x3086, 0x0026,
    0x3086, 0x445C,
    0x3086, 0x014b,
    0x3086, 0x1244,
    0x3086, 0x5251,
    0x3086, 0x1702,
    0x3086, 0x6018,
    0x3086, 0x4A03,
    0x3086, 0x4316,
    0x3086, 0x0443,
    0x3086, 0x1658,
    0x3086, 0x4316,
    0x3086, 0x5943,
    0x3086, 0x165A,
    0x3086, 0x4316,
    0x3086, 0x5B43,
    0x3086, 0x4540,
    0x3086, 0x279C,
    0x3086, 0x4560,
    0x3086, 0x1707,
    0x3086, 0x279D,
    0x3086, 0x1725,
    0x3086, 0x4540,
    0x3086, 0x1710,
    0x3086, 0x2798,
    0x3086, 0x1720,
    0x3086, 0x224B,
    0x3086, 0x1244,
    0x3086, 0x2C2C,
    0x3086, 0x2C2C
};

#elif (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)

MMP_USHORT SNR_AR0330_Reg_Init_Customer[] = 
{
	0x31AE, 0x204,		// 4-lane MIPI Output
	SENSOR_DELAY_REG, 10,
	0x301A, 0x0058,     // Disable streaming
	
	SENSOR_DELAY_REG, 10,
	
	0x31AE, 0x204,      // Output 4-lane MIPI
    SENSOR_DELAY_REG, 10,

	//Configure for Serial Interface
	0x301A, 0x0058,     // Drive Pins,Parallel Enable,SMIA Serializer Disable
	SENSOR_DELAY_REG, 10,	
	0x3064, 0x1802, 	// Disable Embedded Data
	SENSOR_DELAY_REG, 10,	

	//Optimized Gain Configuration
	0x3EE0, 0x1500, 	// DAC_LD_20_21
	0x3EEA, 0x001D,
	0x31E0, 0x1E01,
	0x3F06, 0x046A,
	0x3ED2, 0x0186,	
	0x3ED4, 0x8F2C,	
	0x3ED6, 0x2244,	
	0x3ED8, 0x6442,
	0x30BA, 0x002C,     // Dither enable
	0x3046, 0x4038,		// Enable Flash Pin
	0x3048, 0x8480,		// Flash Pulse Length
	0x3ED0, 0x0016, 	// DAC_LD_4_5
	0x3ED0, 0x0036, 	// DAC_LD_4_5
	0x3ED0, 0x0076, 	// DAC_LD_4_5
	0x3ED0, 0x00F6, 	// DAC_LD_4_5
	0x3ECE, 0x1003, 	// DAC_LD_2_3
	0x3ECE, 0x100F, 	// DAC_LD_2_3
    0x3ECE, 0x103F, 	// DAC_LD_2_3
	0x3ECE, 0x10FF, 	// DAC_LD_2_3
	0x3ED0, 0x00F6, 	// DAC_LD_4_5
	0x3ED0, 0x04F6, 	// DAC_LD_4_5
	0x3ED0, 0x24F6, 	// DAC_LD_4_5
	0x3ED0, 0xE4F6, 	// DAC_LD_4_5
	0x3EE6, 0xA480, 	// DAC_LD_26_27
	0x3EE6, 0xA080, 	// DAC_LD_26_27
	0x3EE6, 0x8080, 	// DAC_LD_26_27
	0x3EE6, 0x0080, 	// DAC_LD_26_27
	0x3EE6, 0x0080, 	// DAC_LD_26_27
	0x3EE8, 0x2024, 	// DAC_LD_28_29
	0x30FE, 128,        // Noise Pedestal of 128

	//Assuming Input Clock of 24MHz.  Output Clock will be 70Mpixel/s
	0x302A, 0x0005, 	// VT_PIX_CLK_DIV
	0x302C, 0x0001, 	// VT_SYS_CLK_DIV **MIPI4
	0x302E, 0x0004, 	// PRE_PLL_CLK_DIV
	0x3030, 82,			// PLL_MULTIPLIER
	0x3036, 0x000A, 	// OP_PIX_CLK_DIV
	0x3038, 0x0001, 	// OP_SYS_CLK_DIV
	0x31AC, 0x0A0A, 	// DATA_FORMAT_BITS
	SENSOR_DELAY_REG, 10,
	//MIPI TIMING
	0x31B0, 40,         // FRAME PREAMBLE
	0x31B2, 14,         // LINE PREAMBLE
	0x31B4, 0x2743,     // MIPI TIMING 0
	0x31B6, 0x114E,     // MIPI TIMING 1
	0x31B8, 0x2049,     // MIPI TIMING 2
	0x31BA, 0x0186,     // MIPI TIMING 3
	0x31BC, 0x8005,     // MIPI TIMING 4
	0x31BE, 0x2003,     // MIPI CONFIG STATUS
    SENSOR_DELAY_REG, 10,
    
	//Sequencer
#if 1//Rev0
	0x3088, 0x8000,
	0x3086, 0x4540,
	0x3086, 0x6134,
	0x3086, 0x4A31,
	0x3086, 0x4342,
	0x3086, 0x4560,
	0x3086, 0x2714,
	0x3086, 0x3DFF,
	0x3086, 0x3DFF,
	0x3086, 0x3DEA,
	0x3086, 0x2704,
	0x3086, 0x3D10,
	0x3086, 0x2705,
	0x3086, 0x3D10,
	0x3086, 0x2715,
	0x3086, 0x3527,
	0x3086, 0x053D,
	0x3086, 0x1045,
	0x3086, 0x4027,
	0x3086, 0x0427,
	0x3086, 0x143D,
	0x3086, 0xFF3D,
	0x3086, 0xFF3D,
	0x3086, 0xEA62,
	0x3086, 0x2728,
	0x3086, 0x3627,
	0x3086, 0x083D,
	0x3086, 0x6444,
	0x3086, 0x2C2C,
	0x3086, 0x2C2C,
	0x3086, 0x4B01,
	0x3086, 0x432D,
	0x3086, 0x4643,
	0x3086, 0x1647,
	0x3086, 0x435F,
	0x3086, 0x4F50,
	0x3086, 0x2604,
	0x3086, 0x2684,
	0x3086, 0x2027,
	0x3086, 0xFC53,
	0x3086, 0x0D5C,
	0x3086, 0x0D60,
	0x3086, 0x5754,
	0x3086, 0x1709,
	0x3086, 0x5556,
	0x3086, 0x4917,
	0x3086, 0x145C,
	0x3086, 0x0945,
	0x3086, 0x0017,
	0x3086, 0x0545,
	0x3086, 0x8026,
	0x3086, 0xA627,
	0x3086, 0xF817,
	0x3086, 0x0227,
	0x3086, 0xFA5C,
	0x3086, 0x0B5F,
	0x3086, 0x5307,
	0x3086, 0x5302,
	0x3086, 0x4D28,
	0x3086, 0x6C4C,
	0x3086, 0x0928,
	0x3086, 0x2C28,
	0x3086, 0x294E,
	0x3086, 0x1718,
	0x3086, 0x26A2,
	0x3086, 0x5C03,
	0x3086, 0x1744,
	0x3086, 0x2809,
	0x3086, 0x27F2,
	0x3086, 0x1708,
	0x3086, 0x2809,
	0x3086, 0x1628,
	0x3086, 0x084D,
	0x3086, 0x1A26,
	0x3086, 0x8327,
	0x3086, 0xFA45,
	0x3086, 0xA017,
	0x3086, 0x0727,
	0x3086, 0xFB17,
	0x3086, 0x2945,
	0x3086, 0x8017,
	0x3086, 0x0827,
	0x3086, 0xFA17,
	0x3086, 0x285D,
	0x3086, 0x5300,
	0x3086, 0x17E8,
	0x3086, 0x5302,
	0x3086, 0x1710,
	0x3086, 0x2682,
	0x3086, 0x170E,
	0x3086, 0x484D,
	0x3086, 0x4E28,
	0x3086, 0x094C,
	0x3086, 0x0B17,
	0x3086, 0x5F28,
	0x3086, 0x0927,
	0x3086, 0xF217,
	0x3086, 0x0A28,
	0x3086, 0x0928,
	0x3086, 0x084D,
	0x3086, 0x1A27,
	0x3086, 0xFA26,
	0x3086, 0x025C,
	0x3086, 0x0045,
	0x3086, 0x4027,
	0x3086, 0x9817,
	0x3086, 0x2A4A,
	0x3086, 0x0A43,
	0x3086, 0x160B,
	0x3086, 0x4327,
	0x3086, 0x9C45,
	0x3086, 0x6017,
	0x3086, 0x0727,
	0x3086, 0x9D17,
	0x3086, 0x2545,
	0x3086, 0x4017,
	0x3086, 0x0827,
	0x3086, 0x985D,
	0x3086, 0x5307,
	0x3086, 0x2644,
	0x3086, 0x5C01,
	0x3086, 0x4B12,
	0x3086, 0x4452,
	0x3086, 0x5117,
	0x3086, 0x0260,
	0x3086, 0x184A,
	0x3086, 0x0343,
	0x3086, 0x1604,
	0x3086, 0x4316,
	0x3086, 0x5843,
	0x3086, 0x1659,
	0x3086, 0x4316,
	0x3086, 0x5A43,
	0x3086, 0x165B,
	0x3086, 0x4327,
	0x3086, 0x9C45,
	0x3086, 0x6017,
	0x3086, 0x0727,
	0x3086, 0x9D17,
	0x3086, 0x2545,
	0x3086, 0x4017,
	0x3086, 0x1027,
	0x3086, 0x9817,
	0x3086, 0x2022,
	0x3086, 0x4B12,
	0x3086, 0x442C,
	0x3086, 0x2C2C,
	0x3086, 0x2C00,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
	0x3086, 0x0000,
#else //Rev1
    0x3088, 0x8000,
    0x3086, 0x4540,
    0x3086, 0x6134,
    0x3086, 0x4A31,
    0x3086, 0x4342,
    0x3086, 0x4560,
    0x3086, 0x2714,
    0x3086, 0x3DFF,
    0x3086, 0x3DFF,
    0x3086, 0x3DEA,
    0x3086, 0x2704,
    0x3086, 0x3D10,
    0x3086, 0x2705,
    0x3086, 0x3D10,
    0x3086, 0x2715,
    0x3086, 0x3527,
    0x3086, 0x053D,
    0x3086, 0x1045,
    0x3086, 0x4027,
    0x3086, 0x0427,
    0x3086, 0x143D,
    0x3086, 0xFF3D,
    0x3086, 0xFF3D,
    0x3086, 0xEA62,
    0x3086, 0x2728,
    0x3086, 0x3627,
    0x3086, 0x083D,
    0x3086, 0x6444,
    0x3086, 0x2C2C,
    0x3086, 0x2C2C,
    0x3086, 0x4B01,
    0x3086, 0x432D,
    0x3086, 0x4643,
    0x3086, 0x1647,
    0x3086, 0x435F,
    0x3086, 0x4F50,
    0x3086, 0x2604,
    0x3086, 0x2684,
    0x3086, 0x2027,
    0x3086, 0xFC53,
    0x3086, 0x0D5C,
    0x3086, 0x0D60,
    0x3086, 0x5754,
    0x3086, 0x1709,
    0x3086, 0x5556,
    0x3086, 0x4917,
    0x3086, 0x145C,
    0x3086, 0x0945,
    0x3086, 0x0045,
    0x3086, 0x8026,
    0x3086, 0xA627,
    0x3086, 0xF817,
    0x3086, 0x0227,
    0x3086, 0xFA5C,
    0x3086, 0x0B5F,
    0x3086, 0x5307,
    0x3086, 0x5302,
    0x3086, 0x4D28,
    0x3086, 0x644C,
    0x3086, 0x0928,
    0x3086, 0x2428,
    0x3086, 0x234E,
    0x3086, 0x1718,
    0x3086, 0x26A2,
    0x3086, 0x5C03,
    0x3086, 0x1744,
    0x3086, 0x27F2,
    0x3086, 0x1708,
    0x3086, 0x2803,
    0x3086, 0x2808,
    0x3086, 0x4D1A,
    0x3086, 0x27FA,
    0x3086, 0x2683,
    0x3086, 0x45A0,
    0x3086, 0x1707,
    0x3086, 0x27FB,
    0x3086, 0x1729,
    0x3086, 0x4580,
    0x3086, 0x1708,
    0x3086, 0x27FA,
    0x3086, 0x1728,
    0x3086, 0x2682,
    0x3086, 0x5D17,
    0x3086, 0x0E48 ,
    0x3086, 0x4D4E,
    0x3086, 0x2803,
    0x3086, 0x4C0B,
    0x3086, 0x175F,
    0x3086, 0x27F2,
    0x3086, 0x170A,
    0x3086, 0x2808,
    0x3086, 0x4D1A,
    0x3086, 0x27FA,
    0x3086, 0x2602,
    0x3086, 0x5C00,
    0x3086, 0x4540,
    0x3086, 0x2798,
    0x3086, 0x172A,
    0x3086, 0x4A0A,
    0x3086, 0x4316,
    0x3086, 0x0B43,
    0x3086, 0x279C,
    0x3086, 0x4560,
    0x3086, 0x1707,
    0x3086, 0x279D,
    0x3086, 0x1725,
    0x3086, 0x4540,
    0x3086, 0x1708,
    0x3086, 0x2798,
    0x3086, 0x5D53,
    0x3086, 0x0026,
    0x3086, 0x445C,
    0x3086, 0x014b,
    0x3086, 0x1244,
    0x3086, 0x5251,
    0x3086, 0x1702,
    0x3086, 0x6018,
    0x3086, 0x4A03,
    0x3086, 0x4316,
    0x3086, 0x0443,
    0x3086, 0x1658,
    0x3086, 0x4316,
    0x3086, 0x5943,
    0x3086, 0x165A,
    0x3086, 0x4316,
    0x3086, 0x5B43,
    0x3086, 0x4540,
    0x3086, 0x279C,
    0x3086, 0x4560,
    0x3086, 0x1707,
    0x3086, 0x279D,
    0x3086, 0x1725,
    0x3086, 0x4540,
    0x3086, 0x1710,
    0x3086, 0x2798,
    0x3086, 0x1720,
    0x3086, 0x224B,
    0x3086, 0x1244,
    0x3086, 0x2C2C,
    0x3086, 0x2C2C
#endif	
};
#endif

#if 0
void ____Sensor_Res_OPR_Table____(){ruturn;} //dummy
#endif

#if (SENSOR_IF == SENSOR_IF_PARALLEL)

ISP_UINT16 SNR_AR0330_Reg_2304x1296[] = 
{
#if (PARALLEL_SENSOR_CROP_2M == 1)
    0x301A, 0x10DC,  	// RESET_REGISTER
    0x301A, 0x10D8,  	// RESET_REGISTER

	0x302A,      6,		// VT_PIX_CLK_DIV
	0x302C,      1, 	// VT_SYS_CLK_DIV
	0x302E,      2,		// PRE_PLL_CLK_DIV
	0x3030,     49,		// PLL_MULTIPLIER
	0x3036,     12,		// OP_PIX_CLK_DIV
	0x3038,      1,		// OP_SYS_CLK_DIV
	0x31AC, 0x0C0C, 	// DATA_FORMAT_BITS
	0x301A, 0x10DC,  	// RESET_REGISTER

	0x31AE, 0x0301,  	// SERIAL_FORMAT

	0x3002,    220,		// Y_ADDR_START
	0x3004,    184,		// X_ADDR_START
	0x3006,   1096+220,	// Y_ADDR_END
	0x3008,   1936+184,	// X_ADDR_END
	0x300A,   1310,		// FRAME_LENGTH_LINES 
	0x300C,   1248,		// LINE_LENGTH_PCK

	0x3012,    1000,	// COARSE_INTEGRATION_TIME
	0x3014,      0, 	// FINE_INTEGRATION_TIME
	0x30A2,      1,  	// X_ODD_INC
	0x30A6,      1,		// Y_ODD_INC
	0x308C,      6, 	// Y_ADDR_START_CB
	0x308A,      6, 	// X_ADDR_START_CB
	0x3090,   1541, 	// Y_ADDR_END_CB
	0x308E,   2309, 	// X_ADDR_END_CB
	0x30AA,   1570,  	// FRAME_LENGTH_LINES_CB
	0x303E,   1248, 	// LINE_LENGTH_PCK_CB
	0x3016,   1569,		// COARSE_INTEGRATION_TIME_CB
	0x3018,      0,  	// FINE_INTEGRATION_TIME_CB
	0x30AE,      1,  	// X_ODD_INC_CB
	0x30A8,      1,  	// Y_ODD_INC_CB
	0x3040, 0x0000,  	// READ_MODE
	0x3042,    103,  	// EXTRA_DELAY

	0x30BA, 0x002C,  	// DIGITAL_CTRL
	0x3088, 0x80BA,  	// SEQ_CTRL_PORT 
	0x3086, 0xE653,  	// SEQ_DATA_PORT
	0x306E, 0xFC10, 
	 
	///AR0330 defect correction
	0x31E0, 0x0741,
    SENSOR_DELAY_REG,	100   // Delay
#else
    0x301A, 0x10DC,  	// RESET_REGISTER
    0x301A, 0x10D8, 	// RESET_REGISTER
	0xFFFF, 100,
	0x302A,      6,		// VT_PIX_CLK_DIV
	0x302C,      1,		// VT_SYS_CLK_DIV
	0x302E,      2, 	// PRE_PLL_CLK_DIV
	0x3030,     61,  	// PLL_MULTIPLIER
	0x3036,     12, 	// OP_PIX_CLK_DIV
	0x3038,      1, 	// OP_SYS_CLK_DIV
	0x31AC, 0x0C0C,  	// DATA_FORMAT_BITS
	0x301A, 0x10DC,  	// RESET_REGISTER
	0xFFFF, 100,
	0x31AE, 0x0301,  	// SERIAL_FORMAT
	0x3002,    0, 		// Y_ADDR_START
	0x3004,    0,		// X_ADDR_START
	0x3006,   1299,		// Y_ADDR_END
	0x3008,   2311,		// X_ADDR_END
	0x300A,   1329, 	// FRAME_LENGTH_LINES 
	0x300C,   1531,
	0x3012,    657,		// COARSE_INTEGRATION_TIME
	0x3014,      0,		// FINE_INTEGRATION_TIME
	0x30A2,      1,		// X_ODD_INC
	0x30A6,      1, 	// Y_ODD_INC 
	0x308C,      6, 	// Y_ADDR_START_CB 
	0x308A,      6,  	// X_ADDR_START_CB 
	0x3090,   1541, 	// Y_ADDR_END_CB 
	0x308E,   2309, 	// X_ADDR_END_CB
	0x30AA,   1570,  	// FRAME_LENGTH_LINES_CB
	0x303E,   1248, 	// LINE_LENGTH_PCK_CB
	0x3016,   1569,  	// COARSE_INTEGRATION_TIME_CB
	0x3018,      0,  	// FINE_INTEGRATION_TIME_CB
	0x30AE,      1, 	// X_ODD_INC_CB
	0x30A8,      1,  	// Y_ODD_INC_CB
	0x3040, 0x0000,  	// READ_MODE
	0x3042,    103, 	// EXTRA_DELAY
	0x30BA, 0x002C,  	// DIGITAL_CTRL
	0x3088, 0x80BA,  	// SEQ_CTRL_PORT
	0x3086, 0xE653,  	// SEQ_DATA_PORT
	0x306E, 0xFC10,  
	0x31E0, 0x0741,
    SENSOR_DELAY_REG,   100 // Delay
#endif
};

ISP_UINT16 SNR_AR0330_Reg_2304x1536[] = 
{
	SENSOR_DELAY_REG, 	100
}

ISP_UINT16 SNR_AR0330_Reg_1152x648[] = 
{
    SENSOR_DELAY_REG, 	100
};

#elif (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)

ISP_UINT16 SNR_AR0330_Reg_2304x1296[] = 
{
	0x3030, (82+1), 	// PLL_MULTIPLIER  @ 490Mbps/lane
    0xFFFF, 100,
    
	//ARRAY READOUT SETTINGS
	// +2 for X axis dead pixel.
	0x3004, 0+2,		// X_ADDR_START
	0x3008, 2311+2,	    // X_ADDR_END
	0x3002, 122,    	// Y_ADDR_START
	0x3006, 1421,	    // Y_ADDR_END

	//Sub-sampling
	0x30A2, 1,			// X_ODD_INCREMENT
	0x30A6, 1,			// Y_ODD_INCREMENT
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    
    //Frame-Timing
	0x300C, 1250,       // LINE_LENGTH_PCK
	0x300A, 1312,		// FRAME_LENGTH_LINES
	0x3014, 0,		    // FINE_INTEGRATION_TIME
	0x3012, 152,		// Coarse_Integration_Time
	0x3042, 0,			// EXTRA_DELAY
	0x30BA, 0x2C,

	0x301A, 0x000C,     // Grouped Parameter Hold = 0x0
};

ISP_UINT16 SNR_AR0330_Reg_2304x1536[] = 
{
	0x3030, 82, 	    // PLL_MULTIPLIER  @ 490Mbps/lane

	//ARRAY READOUT SETTINGS
	0x3004, 0+2,		// X_ADDR_START
	0x3008, 2311+2,	    // X_ADDR_END
	0x3002, 0 + 6,    	// Y_ADDR_START
	0x3006, 0 + 6 + (1536 + 4 - 1),	// Y_ADDR_END

	//Sub-sampling
	0x30A2, 1,			// X_ODD_INCREMENT
	0x30A6, 1,			// Y_ODD_INCREMENT
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	
	//Frame-Timing
	0x300C, 1250,       // LINE_LENGTH_PCK
	0x300A, 1536+4,     // FRAME_LENGTH_LINES
	0x3014, 0,		    // FINE_INTEGRATION_TIME
	0x3012, 1000,		// Coarse_Integration_Time
	0x3042, 0,			// EXTRA_DELAY
	0x30BA, 0x2C,

	0x301A, 0x000C,     // Grouped Parameter Hold = 0x0
};

ISP_UINT16 SNR_AR0330_Reg_1920x1080[] = 
{
	0x3030, 82, 	    // PLL_MULTIPLIER  @ 490Mbps/lane

	//ARRAY READOUT SETTINGS
	0x3004, 192,		// X_ADDR_START
	0x3008, 2127,		// X_ADDR_END
	0x3002, 232,    	// Y_ADDR_START
	0x3006, 1319,		// Y_ADDR_END

	//Sub-sampling
	0x30A2, 1,			// X_ODD_INCREMENT
	0x30A6, 1,			// Y_ODD_INCREMENT
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    
    //Frame-Timing
	0x300C, 1260,		// LINE_LENGTH_PCK
	0x300A, 1270,		// FRAME_LENGTH_LINES
	0x3014, 0,		    // FINE_INTEGRATION_TIME
	0x3012, 1000,		// Coarse_Integration_Time
	0x3042, 0,			// EXTRA_DELAY
	0x30BA, 0x2C,

	0x301A, 0x000C,     // Grouped Parameter Hold = 0x0
};

ISP_UINT16 SNR_AR0330_Reg_1152x648[] = 
{
	0x3030, 82, 	    // PLL_MULTIPLIER @ 490Mbps/lane

    //ARRAY READOUT SETTINGS
	0x3004, 2+0,		// X_ADDR_START
	0x3008, 2+ (1152 + 4 )* 2 - 1,  // X_ADDR_END
	0x3002, 122,   		// Y_ADDR_START
	0x3006, 122 + (648 + 4) * 2 - 1,// Y_ADDR_END

    //Sub-sampling
    0x30A2, 3,			// X_ODD_INCREMENT
    0x30A6, 3,			// Y_ODD_INCREMENT
    0x3040, 0x3000, 	// Row-colum Bin, [12]: Row binning, [13]: column binning

    //Frame-Timing
    0x300C, 1232,       // LINE_LENGTH_PCK
    0x300A, 672,		// FRAME_LENGTH_LINES
    0x3014, 0,			// FINE_INTEGRATION_TIME
    0x3012, 552,		// Coarse_Integration_Time
    0x3042, 0,			// EXTRA_DELAY
    0x30BA, 0x2C,	    // Digital_Ctrl_Adc_High_Speed
  
    0x301A, 0x000C,     // Grouped Parameter Hold = 0x0
};

#elif (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)	

ISP_UINT16 SNR_AR0330_Reg_2304x1296[] = 
{
	0x3030, 83,	    	// PLL_MULTIPLIER

    //ARRAY READOUT SETTINGS
    0x3004, 0,			// X_ADDR_START
	0x3008, 2311,		// X_ADDR_END
	0x3002, 0,    		// Y_ADDR_START
	0x3006, 1299,		// Y_ADDR_END

    //Sub-sampling
    0x30A2, 1,			// X_ODD_INCREMENT
    0x30A6, 1,			// Y_ODD_INCREMENT
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning 

    //Frame-Timing
	0x300C, 1250,       // LINE_LENGTH_PCK
	0x300A, 1329,		// FRAME_LENGTH_LINES
    0x3014, 0,			// FINE_INTEGRATION_TIME
	0x3012, 152,		// Coarse_Integration_Time
    0x3042, 0,			// EXTRA_DELAY
    0x30BA, 0x2C,	    // Digital_Ctrl_Adc_High_Speed
  
    0x301A, 0x000C,     // Grouped Parameter Hold = 0x0
};

ISP_UINT16 SNR_AR0330_Reg_2304x1536[] =
{
	0x3030, 82, 	    // PLL_MULTIPLIER  @ 490Mbps/lane

	//ARRAY READOUT SETTINGS
	0x3004, 0+2,		// X_ADDR_START
	0x3008, 2311+2,	    // X_ADDR_END
	0x3002, 0 + 6,    	// Y_ADDR_START
	0x3006, 0 + 6 + (1536 + 4/* - 1*/),	// Y_ADDR_END

	//Sub-sampling
	0x30A2, 1,			// X_ODD_INCREMENT
	0x30A6, 1,			// Y_ODD_INCREMENT
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	
	//Frame-Timing
	0x300C, 1250,       // LINE_LENGTH_PCK
	0x300A, 1536+4,     // FRAME_LENGTH_LINES
	0x3014, 0,		    // FINE_INTEGRATION_TIME
	0x3012, 1000,		// Coarse_Integration_Time
	0x3042, 0,			// EXTRA_DELAY
	0x30BA, 0x2C,

	0x301A, 0x000C,     // Grouped Parameter Hold = 0x0
};

ISP_UINT16 SNR_AR0330_Reg_1920x1080[] = 
{
	0x3030, 40, 	    // PLL_MULTIPLIER  @ 240Mbps/lane

    //ARRAY READOUT SETTINGS
	0x3004, 192,		// X_ADDR_START
	0x3008, 2127,		// X_ADDR_END
	0x3002, 232,    	// Y_ADDR_START
	0x3006, 1319,		// Y_ADDR_END

    //Sub-sampling
    0x30A2, 1,			// X_ODD_INCREMENT
    0x30A6, 1,			// Y_ODD_INCREMENT
    0x3040, 0, 	        // Row Bin, [12]: Row binning, [13]: column binning

    //Frame-Timing
    0x300C, 1260,		// LINE_LENGTH_PCK
    0x300A, 1270,		// FRAME_LENGTH_LINES
    0x3014, 136,		// FINE_INTEGRATION_TIME
    0x3012, 1000,		// Coarse_Integration_Time
    0x3042, 0,			// EXTRA_DELAY
    0x30BA, 0x2C,	    // Digital_Ctrl_Adc_High_Speed
  
 	0x301A, 0x000C,     // Grouped Parameter Hold = 0x0
};

ISP_UINT16 SNR_AR0330_Reg_1152x648[] = 
{
	0x3030, 42, 	    // PLL_MULTIPLIER  @ 430Mbps/lane

    //ARRAY READOUT SETTINGS
	0x3004, 0,			// X_ADDR_START
	0x3008, 2311,		// X_ADDR_END
	0x3002, 0,   		// Y_ADDR_START
	0x3006, 1303,		// Y_ADDR_END

    //Sub-sampling
    0x30A2, 3,			// X_ODD_INCREMENT
    0x30A6, 3,			// Y_ODD_INCREMENT
    0x3040, 0x3000, 	// Row-colum Bin, [12]: Row binning, [13]: column binning

    //Frame-Timing
    0x300C, 1250,		// LINE_LENGTH_PCK
    0x300A, 660+8,		// FRAME_LENGTH_LINES
    0x3014, 0,			// FINE_INTEGRATION_TIME
    0x3012, 550,		// Coarse_Integration_Time
    0x3042, 0,			// EXTRA_DELAY
    0x30BA, 0x2C,	    // Digital_Ctrl_Adc_High_Speed
  
    0x301A, 0x000C,     // Grouped Parameter Hold = 0x0
};

#endif

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
    SensorCustFunc.OprTable->usInitSize                   		= (sizeof(SNR_AR0330_Reg_Init_Customer)/sizeof(SNR_AR0330_Reg_Init_Customer[0]))/2;
    SensorCustFunc.OprTable->uspInitTable                 		= &SNR_AR0330_Reg_Init_Customer[0];    

    SensorCustFunc.OprTable->bBinTableExist                     = MMP_FALSE;
    SensorCustFunc.OprTable->bInitDoneTableExist                = MMP_FALSE;
        
    SensorCustFunc.OprTable->usSize[RES_IDX_2304x1296_30FPS]    = (sizeof(SNR_AR0330_Reg_2304x1296)/sizeof(SNR_AR0330_Reg_2304x1296[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_2304x1296_30FPS]  = &SNR_AR0330_Reg_2304x1296[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_2048x1536_25FPS]    = (sizeof(SNR_AR0330_Reg_2304x1536)/sizeof(SNR_AR0330_Reg_2304x1536[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_2048x1536_25FPS]  = &SNR_AR0330_Reg_2304x1536[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_800x600_60FPS]      = (sizeof(SNR_AR0330_Reg_1152x648)/sizeof(SNR_AR0330_Reg_1152x648[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_800x600_60FPS]    = &SNR_AR0330_Reg_1152x648[0];

    // Init Vif Setting : Common
    SensorCustFunc.VifSetting->SnrType                                = MMPF_VIF_SNR_TYPE_BAYER;
    #if (SENSOR_IF == SENSOR_IF_PARALLEL)
    SensorCustFunc.VifSetting->OutInterface                           = MMPF_VIF_IF_PARALLEL;
    #else
    SensorCustFunc.VifSetting->OutInterface                           = MMPF_VIF_IF_MIPI_QUAD;
    #endif
    #if (SUPPORT_DUAL_SNR)
    SensorCustFunc.VifSetting->VifPadId								  = MMPF_VIF_MDL_ID1;
    #else
    SensorCustFunc.VifSetting->VifPadId								  = MMPF_VIF_MDL_ID0;
    #endif
    
    // Init Vif Setting : PowerOn Setting
    SensorCustFunc.VifSetting->powerOnSetting.bTurnOnExtPower         = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPin           = MMP_GPIO_MAX;  
    SensorCustFunc.VifSetting->powerOnSetting.bFirstEnPinHigh         = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubFirstEnPinDelay       = 0;
    SensorCustFunc.VifSetting->powerOnSetting.bNextEnPinHigh          = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOnSetting.ubNextEnPinDelay        = 0;
    SensorCustFunc.VifSetting->powerOnSetting.bTurnOnClockBeforeRst   = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.bFirstRstPinHigh        = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOnSetting.ubFirstRstPinDelay      = 1;
    SensorCustFunc.VifSetting->powerOnSetting.bNextRstPinHigh         = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubNextRstPinDelay       = 0;

    // Init Vif Setting : PowerOff Setting
    SensorCustFunc.VifSetting->powerOffSetting.bEnterStandByMode      = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOffSetting.usStandByModeReg       = 0x301A;
    SensorCustFunc.VifSetting->powerOffSetting.usStandByModeMask      = 0x04;
    SensorCustFunc.VifSetting->powerOffSetting.bEnPinHigh             = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOffSetting.ubEnPinDelay           = 20;
    SensorCustFunc.VifSetting->powerOffSetting.bTurnOffMClock         = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOffSetting.bTurnOffExtPower       = MMP_TRUE;    
    SensorCustFunc.VifSetting->powerOffSetting.usExtPowerPin          = MMP_GPIO_MAX;

    // Init Vif Setting : Sensor MClock Setting
    SensorCustFunc.VifSetting->clockAttr.bClkOutEn                    = MMP_TRUE; 
    SensorCustFunc.VifSetting->clockAttr.ubClkFreqDiv                 = 0;
    SensorCustFunc.VifSetting->clockAttr.ulMClkFreq                   = 24000;
    #if (SUPPORT_EIS)
    SensorCustFunc.VifSetting->clockAttr.ulDesiredFreq                = 12000; //For EIS Test, Slow down FPS
    #else
    SensorCustFunc.VifSetting->clockAttr.ulDesiredFreq                = 24000;
    #endif
    SensorCustFunc.VifSetting->clockAttr.ubClkPhase                   = MMPF_VIF_SNR_PHASE_DELAY_NONE;
    SensorCustFunc.VifSetting->clockAttr.ubClkPolarity                = MMPF_VIF_SNR_CLK_POLARITY_POS;
    SensorCustFunc.VifSetting->clockAttr.ubClkSrc					  = MMPF_VIF_SNR_CLK_SRC_PMCLK;
    
    // Init Vif Setting : Parallel Sensor Setting
    SensorCustFunc.VifSetting->paralAttr.ubLatchTiming                = MMPF_VIF_SNR_LATCH_POS_EDGE;
    SensorCustFunc.VifSetting->paralAttr.ubHsyncPolarity              = MMPF_VIF_SNR_CLK_POLARITY_POS;
    SensorCustFunc.VifSetting->paralAttr.ubVsyncPolarity              = MMPF_VIF_SNR_CLK_POLARITY_POS;
    
    // Init Vif Setting : MIPI Sensor Setting
    SensorCustFunc.VifSetting->mipiAttr.bClkDelayEn                   = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bClkLaneSwapEn                = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.usClkDelay                    = 0;
    SensorCustFunc.VifSetting->mipiAttr.ubBClkLatchTiming             = MMPF_VIF_SNR_LATCH_NEG_EDGE;
#if (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[0]                = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[1]                = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[2]                = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneEn[3]                = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[0]               = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[1]               = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[2]               = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataDelayEn[3]               = MMP_TRUE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[0]            = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[1]            = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[2]            = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.bDataLaneSwapEn[3]            = MMP_FALSE;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[0]              = 0;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[1]              = 1;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[2]              = 2;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[3]              = 3;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[0]                = 0;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[1]                = 0;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[2]                = 0;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[3]                = 0;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[0]               = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[1]               = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[2]               = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[3]               = 0x1F;
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
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[0]              = 0;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[1]              = 1;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[2]              = 2;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[3]              = 3;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[0]                = 0;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[1]                = 0;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[2]                = 0;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[3]                = 0;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[0]               = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[1]               = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[2]               = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[3]               = 0x1F;
#endif
    
    // Init Vif Setting : Color ID Setting
    SensorCustFunc.VifSetting->colorId.VifColorId              		  = MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.bUseCustomId  	  = MMP_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_DoAE_FrmSt
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_DoAE_FrmSt(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
	switch (ulFrameCnt % 6) {
	case 0:
		#if (ISP_EN)
		ISP_IF_AE_Execute();
		gsSensorFunction->MMPF_Sensor_SetShutter(ubSnrSel, 0, 0);
	    gsSensorFunction->MMPF_Sensor_SetGain(ubSnrSel, ISP_IF_AE_GetGain());
		#endif
		break;
	}
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
    ISP_UINT16 sensor_again;
    ISP_UINT16 sensor_dgain;

    ulGain = ulGain * 0x40 / ISP_IF_AE_GetGainBase();

    // Sensor Gain Mapping
    if(ulGain < 0x80){
        sensor_dgain = ulGain << 1;   
        sensor_again = 0x0;     // 1X ~ 2X
    }
    else if (ulGain < 0x100){
        sensor_dgain = ulGain;   
        sensor_again = 0x10;    // 2X ~ 4X
    }       
    else if (ulGain < 0x200){
        sensor_dgain = ulGain >> 1;   
        sensor_again = 0x20;    // 4X ~ 8X
    }   
    else{
        sensor_dgain = ulGain >> 2;  
        sensor_again = 0x30;    // 8X ~16X
    }      

    gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x305E, sensor_dgain);
    gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3060, sensor_again);
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
	
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x300A, new_vsync);
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3012, new_shutter);
#endif
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetFlip
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetFlip(MMP_UBYTE ubSnrSel, MMP_UBYTE ubMode)
{
	//TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetRotate
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetRotate(MMP_UBYTE ubSnrSel, MMP_UBYTE ubMode)
{
	//TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_CheckVersion
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_CheckVersion(MMP_UBYTE ubSnrSel, MMP_ULONG *pulVersion)
{
    //TBD
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

#endif  //BIND_SENSOR_AR0330
#endif	//SENSOR_EN
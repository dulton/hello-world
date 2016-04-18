//==============================================================================
//
//  File        : sensor_ar0835.c
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
#if (BIND_SENSOR_AR0835)

#include "mmpf_sensor.h"
#include "Sensor_Mod_Remapping.h"
#include "isp_if.h"

//==============================================================================
//
//                              GLOBAL VARIABLE
//
//==============================================================================

MMPF_SENSOR_RESOLUTION m_SensorRes = 
{
	7,				// ubSensorModeNum
	0,				// ubDefPreviewMode
	0,				// ubDefCaptureMode
	3000,           // usPixelSize (TBD)
//  Mode0   	Mode1   	Mode2   	Mode3(WVGA) Mode4(WVGA) Mode5   	Mode6   	
//  1080@30P    720@60P    	720@30P		480@30P	    480@120P	1800@15P	1512@15P	
    {1,     	1,      	1,      	1,      	1,       	1,      	1,			},	// usVifGrabStX
    {1,     	1,      	1,      	1,      	1,       	1,      	1,			},	// usVifGrabStY
    {1928,  	1288,   	1288,   	856,   		856,    	3208,      	2696,		},	// usVifGrabW
    {1096,  	728,   		728,   		488,    	488,     	1808,      	1528,		},	// usVifGrabH
    #if (CHIP == MCR_V2)
    {1,     	1,      	1,      	1,      	1,       	1,      	1,			},	// usBayerInGrabX
    {1,     	1,      	1,      	1,      	1,       	1,      	1,			},	// usBayerInGrabY
    {8,     	8,      	8,      	8,      	8,       	8,      	8,			},	// usBayerInDummyX
    {16,     	8,      	8,      	8,      	8,       	8,      	16,			},	// usBayerInDummyY
    {1920,  	1280,   	1280,   	848,   		848,    	3200,      	2688,		},	// usBayerOutW
    {1080,  	720,   		720,   		480,    	480,     	1800,      	1512,		},	// usBayerOutH
    #endif 
    {1920,  	1280,   	1280,   	848,   		848,    	3200,      	2688,		},	// usScalInputW
    {1080,  	720,   		720,   		480,    	480,     	1800,      	1512,		},	// usScalInputH
    {300,    	600,     	300,     	300,    	1200,     	150,      	150,		},	// usTargetFpsx10
    {1906,  	985,   		1659,   	1659,    	959,     	2902,      	2902,		},	// usVsyncLine
    {1,     	1,      	1,      	1,      	1,       	1,      	1,			},	// ubWBinningN
    {1,     	1,      	1,      	1,      	1,       	1,      	1,			},	// ubWBinningM
    {1,     	1,      	1,      	1,      	1,       	1,      	1,			},	// ubHBinningN
    {1,     	1,      	1,      	1,      	1,       	1,      	1,			},	// ubHBinningM
    {0xFF,     	0xFF,      	0xFF,      	0xFF,      	0xFF,       0xFF,      	0xFF,		},	// ubCustIQmode
    {0xFF,     	0xFF,      	0xFF,      	0xFF,      	0xFF,       0xFF,      	0xFF,		},	// ubCustAEmode   
};

// OPR Table and Vif Setting
MMPF_SENSOR_OPR_TABLE       m_OprTable;
MMPF_SENSOR_VIF_SETTING     m_VifSetting;

// IQ Table
const ISP_UINT8 Sensor_IQ_CompressedText[] = 
{
#include "isp_8428_iq_data_v2_AR0835_v1.xls.ciq.txt"
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

MMP_USHORT SNR_AR0835_Reg_Init_Customer[] = 
{
	//[Initialize for Camera] 
	//XMCLK=24000000                                          
	
	0x301A, 0x0019,
	0x301A, 0x0018,
	                                      
	SENSOR_DELAY_REG, 2,                                              
	
	0x3042, 0x0000, 			
	0x30C0, 0x1810, 			
	0x30C8, 0x0018, 			
	0x30D2, 0x0000, 			
	0x30D4, 0x3030, 			
	0x30D6, 0x2200, 			
	0x30DA, 0x0080, 			
	0x30DC, 0x0080, 			
	0x30EE, 0x0340, 			
	0x316A, 0x8800, 			
	0x316C, 0x8200, 			
	0x316E, 0x8200, //1_7		
	0x3172, 0x0286, 			
	0x3174, 0x8000, 			
	0x317C, 0xE103, 			
	0x3180, 0xB080, 			
	0x31E0, 0x0741, 			
	0x31E6, 0x0000, 			
	0x3ECC, 0x0056, 			
	0x3ED0, 0xA666, //833		
	0x3ED2, 0x6664, //833		
	0x3ED4, 0x6ACC, 			
	0x3ED8, 0x7488, 			
	0x3EDA, 0x77CB, 			
	0x3EDE, 0x6664, 			
	0x3EE0, 0x26D5, 			
	0x3EE4, 0x35C8, //1_7		
	0x3EE6, 0xB10C, 			
	0x3EE8, 0x6E79, 			
	0x3EEA, 0xC8B9, //833		
	0x3EFA, 0xA6A6, //833		
	0x3EFE, 0x99CC, //833		
	0x3F00, 0x0051, 			
	0x3F02, 0x00A2, 			
	0x3F04, 0x0002, 			
	0x3F06, 0x0004, 			
	0x3F08, 0x0008, 			
	0x3F0A, 0x0702, 			
	0x3F0C, 0x0707, 			
	0x3F10, 0x0505, 			
	0x3F12, 0x0303, 			
	0x3F14, 0x0101, 			
	0x3F16, 0x0103, 			
	0x3F18, 0x0114, 			
	0x3F1A, 0x0112, 			
	0x3F1C, 0x0014, 			
	0x3F1E, 0x001E, 			
	0x3F20, 0x0209, 			
	0x3F2C, 0x2210, 			
	0x3F38, 0x44A8, 			
	0x3F40, 0x1D1D, 			
	0x3F42, 0x1D1D, 			
	0x3F44, 0x1D1D, 		
    
	//						
	//Configure Hispi 10 bit	
	0x31AE, 0x0204, 	// SERIAL_FORMAT					
	0x31BE, 0xC003, 	// MIPI_CONFIG //For VDD_SLVS=1.2v	
	0x0112, 0x0A0A, 	// CCP_DATA_FORMAT					
    //Sequencer_v14p02-2Bytes	//833  

	//[Sequencer_v14p02-2Bytes] 
	
	0x3D00, 0x0477,
	0x3D02, 0xCFFF,
	0x3D04, 0xFFFF,
	0x3D06, 0xFFFF,
	0x3D08, 0x6F40,
	0x3D0A, 0x140E,
	0x3D0C, 0x23C2,
	0x3D0E, 0x4120,
	0x3D10, 0x3054,
	0x3D12, 0x8042,
	0x3D14, 0x00C0,
	0x3D16, 0x8357,
	0x3D18, 0x8464,
	0x3D1A, 0x6455,
	0x3D1C, 0x8165,
	0x3D1E, 0x6582,
	0x3D20, 0x00C0,
	0x3D22, 0x6E80,
	0x3D24, 0x5051,
	0x3D26, 0x5881,
	0x3D28, 0x6042,
	0x3D2A, 0x8161,
	0x3D2C, 0x816E,
	0x3D2E, 0x805E,
	0x3D30, 0x8130,
	0x3D32, 0x0C84,
	0x3D34, 0x6382,
	0x3D36, 0x5B92,
	0x3D38, 0x5980,
	0x3D3A, 0x5AA9,
	0x3D3C, 0x300C,
	0x3D3E, 0x8359,
	0x3D40, 0x5982,
	0x3D42, 0x5F97,
	0x3D44, 0x5E80,
	0x3D46, 0x6C80,
	0x3D48, 0x6D98,
	0x3D4A, 0x5E89,
	0x3D4C, 0x5080,
	0x3D4E, 0x5182,
	0x3D50, 0x5880,
	0x3D52, 0x6683,
	0x3D54, 0x6464,
	0x3D56, 0x8030,
	0x3D58, 0x50DC,
	0x3D5A, 0x6A83,
	0x3D5C, 0x6BAA,
	0x3D5E, 0x3094,
	0x3D60, 0x6784,
	0x3D62, 0x6565,
	0x3D64, 0x814D,
	0x3D66, 0x686A,
	0x3D68, 0xAC06,
	0x3D6A, 0x088D,
	0x3D6C, 0x4596,
	0x3D6E, 0x4585,
	0x3D70, 0x6A83,
	0x3D72, 0x6B06,
	0x3D74, 0x08A9,
	0x3D76, 0x3090,
	0x3D78, 0x6764,
	0x3D7A, 0x6489,
	0x3D7C, 0x6565,
	0x3D7E, 0x8158,
	0x3D80, 0x8810,
	0x3D82, 0xC0B1,
	0x3D84, 0x5E96,
	0x3D86, 0x5382,
	0x3D88, 0x5E52,
	0x3D8A, 0x6680,
	0x3D8C, 0x5883,
	0x3D8E, 0x6464,
	0x3D90, 0x805B,
	0x3D92, 0x815A,
	0x3D94, 0x1D0C,
	0x3D96, 0x8055,
	0x3D98, 0x3060,
	0x3D9A, 0x4182,
	0x3D9C, 0x42B2,
	0x3D9E, 0x4280,
	0x3DA0, 0x4081,
	0x3DA2, 0x4089,
	0x3DA4, 0x06C0,
	0x3DA6, 0x4180,
	0x3DA8, 0x4285,
	0x3DAA, 0x4483,
	0x3DAC, 0x4382,
	0x3DAE, 0x6A83,
	0x3DB0, 0x6B8D,
	0x3DB2, 0x4383,
	0x3DB4, 0x4481,
	0x3DB6, 0x4185,
	0x3DB8, 0x06C0,
	0x3DBA, 0x8C30,
	0x3DBC, 0xA467,
	0x3DBE, 0x8142,
	0x3DC0, 0x8265,
	0x3DC2, 0x6581,
	0x3DC4, 0x696A,
	0x3DC6, 0x9640,
	0x3DC8, 0x8240,
	0x3DCA, 0x8906,
	0x3DCC, 0xC041,
	0x3DCE, 0x8042,
	0x3DD0, 0x8544,
	0x3DD2, 0x8343,
	0x3DD4, 0x9243,
	0x3DD6, 0x8344,
	0x3DD8, 0x8541,
	0x3DDA, 0x8106,
	0x3DDC, 0xC081,
	0x3DDE, 0x6A83,
	0x3DE0, 0x6B82,
	0x3DE2, 0x42A0,
	0x3DE4, 0x4084,
	0x3DE6, 0x38A8,
	0x3DE8, 0x3300,
	0x3DEA, 0x2830,
	0x3DEC, 0x7000,
	0x3DEE, 0x6F40,
	0x3DF0, 0x140E,
	0x3DF2, 0x23C2,
	0x3DF4, 0x4182,
	0x3DF6, 0x4200,
	0x3DF8, 0xC05D,
	0x3DFA, 0x805A,
	0x3DFC, 0x8057,
	0x3DFE, 0x8464,
	0x3E00, 0x8055,
	0x3E02, 0x8664,
	0x3E04, 0x8065,
	0x3E06, 0x8865,
	0x3E08, 0x8254,
	0x3E0A, 0x8058,
	0x3E0C, 0x8000,
	0x3E0E, 0xC086,
	0x3E10, 0x4282,
	0x3E12, 0x1030,
	0x3E14, 0x9C5C,
	0x3E16, 0x806E,
	0x3E18, 0x865B,
	0x3E1A, 0x8063,
	0x3E1C, 0x9E59,
	0x3E1E, 0x8C5E,
	0x3E20, 0x8A6C,
	0x3E22, 0x806D,
	0x3E24, 0x815F,
	0x3E26, 0x6061,
	0x3E28, 0x8810,
	0x3E2A, 0x3066,
	0x3E2C, 0x836E,
	0x3E2E, 0x8064,
	0x3E30, 0x8764,
	0x3E32, 0x3050,
	0x3E34, 0xD36A,
	0x3E36, 0x6BAD,
	0x3E38, 0x3094,
	0x3E3A, 0x6784,
	0x3E3C, 0x6582,
	0x3E3E, 0x4D83,
	0x3E40, 0x6530,
	0x3E42, 0x50A7,
	0x3E44, 0x4306,
	0x3E46, 0x008D,
	0x3E48, 0x459A,
	0x3E4A, 0x6A6B,
	0x3E4C, 0x4585,
	0x3E4E, 0x0600,
	0x3E50, 0x8143,
	0x3E52, 0x8A6F,
	0x3E54, 0x9630,
	0x3E56, 0x9067,
	0x3E58, 0x6488,
	0x3E5A, 0x6480,
	0x3E5C, 0x6582,
	0x3E5E, 0x10C0,
	0x3E60, 0x8465,
	0x3E62, 0xEF10,
	0x3E64, 0xC066,
	0x3E66, 0x8564,
	0x3E68, 0x8117,
	0x3E6A, 0x0080,
	0x3E6C, 0x200D,
	0x3E6E, 0x8018,
	0x3E70, 0x0C80,
	0x3E72, 0x6430,
	0x3E74, 0x6041,
	0x3E76, 0x8242,
	0x3E78, 0xB242,
	0x3E7A, 0x8040,
	0x3E7C, 0x8240,
	0x3E7E, 0x4C45,
	0x3E80, 0x926A,
	0x3E82, 0x6B9B,
	0x3E84, 0x4581,
	0x3E86, 0x4C40,
	0x3E88, 0x8C30,
	0x3E8A, 0xA467,
	0x3E8C, 0x8565,
	0x3E8E, 0x8765,
	0x3E90, 0x3060,
	0x3E92, 0xD36A,
	0x3E94, 0x6BAC,
	0x3E96, 0x6C32,
	0x3E98, 0xA880,
	0x3E9A, 0x2830,
	0x3E9C, 0x7000,
	0x3E9E, 0x8040,
	0x3EA0, 0x4CBD,
	0x3EA2, 0x000E,
	0x3EA4, 0xBE44,
	0x3EA6, 0x8844,
	0x3EA8, 0xBC78,
	0x3EAA, 0x0900,
	0x3EAC, 0x8904,
	0x3EAE, 0x8080,
	0x3EB0, 0x0240,
	0x3EB2, 0x8609,
	0x3EB4, 0x008E,
	0x3EB6, 0x0900,
	0x3EB8, 0x8002,
	0x3EBA, 0x4080,
	0x3EBC, 0x0480,
	0x3EBE, 0x887D,
	0x3EC0, 0xAC86,
	0x3EC2, 0x0900,
	0x3EC4, 0x877A,
	0x3EC6, 0x000E,
	0x3EC8, 0xC379,
	0x3ECA, 0x4C40,
	
	//0x3070, 2, //Color Pattern
};

#if 0
void ____Sensor_Res_OPR_Table____(){ruturn;} //dummy
#endif

//Mode 0: Video Record 1080(1920x1080)@30P
//[FOX:1080p 1928x1096 29.97fps vt=312 op=312Mbps  FOV=3264x1836]
MMP_USHORT SNR_AR0835_Reg_1928x1096_30P_Customer[] = 
{
	// 312bps 
	0x301A, 0x0018, 	// RESET_REGISTER                   

	SENSOR_DELAY_REG, 2, 
	
	0x31AE, 0x0204, 	// SERIAL_FORMAT	  
	0x0112, 0x0A0A, 	// CCP_DATA_FORMAT	  
	0x3F3A, 0xFF03, 	// ANALOG_CONTROL8	  
	0x0300, 0x0004, 	// VT_PIX_CLK_DIV	
	0x0302, 0x0001, 	// VT_SYS_CLK_DIV	  
	0x0304, 0x0002, 	// PRE_PLL_CLK_DIV	  
	0x0306, 52			,	// PLL_MULTIPLIER	  
	0x0308, 0x000A, 	// OP_PIX_CLK_DIV	  
	0x030A, 0x0002, 	// OP_SYS_CLK_DIV	
	0x3016, 0x0111, 	// ROW_SPEED		  
	0x3064, 0x5800, 	// SMIA_TEST		  
	
	//							 
	//LOAD = MIPI Timing-312mbps                       
	
	0x31B0, 0x003B, 	// FRAME_PREAMBLE //  
	0x31B2, 0x0029, 	// LINE_PREAMBLE	  
	0x31B4, 0x1533, 	// MIPI_TIMING_0		
	0x31B6, 0x11D4, 	// MIPI_TIMING_1	  
	0x31B8, 0x2048, 	// MIPI_TIMING_2		
	0x31BA, 0x0104, 	// MIPI_TIMING_3	
	0x31BC, 0x0004, 	// MIPI_TIMING_4		
	//0x31BC, 0x8004, 	// MIPI_TIMING_4  //Rogers: MIPI Clk cont
	//                                                     
	0x3002, 306  ,		// Y_ADDR_START 				
	0x3006, 2157	 ,		// Y_ADDR_END					
	0x3004, 0x0008, 	// X_ADDR_START 				  
	0x3008, 0x0CC7, 	// X_ADDR_END					  
	0x3040, 0x4041, 	// READ_MODE					  
	0x300C, 5460		,	// LINE_LENGTH_PCK //			  
	0x300A, 1906		,	// FRAME_LENGTH_LINES			  
	0x3012, 1904	,	// COARSE_INTEGRATION_TIME		  
	0x3018, 3650	,	// extra_delay					  
	0x0400, 0x0002, // SCALING_MODE 				  
	0x0402, 0x0000, 	// SPATIAL_SAMPLING 			  
	0x0404, 0x001B, 	// SCALE_M						  
	0x0408, 0x050A, 	// SECOND_RESIDUAL				  
	0x040A, 0x014A, 	// SECOND_CROP					  
	0x306E, 0x9090, 	// DATA_PATH_SELECT 			  
	0x034C, 1928	 ,	// X_OUTPUT_SIZE				  
	0x034E, 1096		,// Y_OUTPUT_SIZE					
	0x3120, 0x0031, 	// GAIN_DITHER_CONTROL			  
	0x301A, 0x001C, 	// RESET_REGISTER				  	              
	        
	//STATE = Master Clock, 300000000  
};

#if 0
//Mode 1: Video Record 720(1280x720)@60P
//[FOX: 720p 1288x728 59.94fps vt=228 op=228Mbps FOV=3264x1836] 
MMP_USHORT SNR_AR0835_Reg_1288x728_60P_Customer[] = 
{
	// 228bps
	0x301A, 0x0018, 	// RESET_REGISTER                   

	SENSOR_DELAY_REG, 2, 
	
	0x31AE, 0x0204, 	// SERIAL_FORMAT	 
	0x0112, 0x0A0A, 	// CCP_DATA_FORMAT	 
	0x3F3A, 0xFF03, 	// ANALOG_CONTROL8	 
	0x0300, 0x0004, 	// VT_PIX_CLK_DIV	 
	0x0302, 0x0001, 	// VT_SYS_CLK_DIV	 
	0x0304, 0x0002, 	// PRE_PLL_CLK_DIV	 
	0x0306, 38			,	// PLL_MULTIPLIER	 
	0x0308, 0x000A, 	// OP_PIX_CLK_DIV	 
	0x030A, 0x0002, 	// OP_SYS_CLK_DIV  
	0x3016, 0x0111, 	// ROW_SPEED		 
	0x3064, 0x5800, 	// SMIA_TEST		 
	
	//							 
	//LOAD = MIPI Timing-312mbps 
	                         
	0x31B0, 0x003B, 	// FRAME_PREAMBLE // 
	0x31B2, 0x0029, 	// LINE_PREAMBLE	 
	0x31B4, 0x1533, 	// MIPI_TIMING_0	   
	0x31B6, 0x11D4, 	// MIPI_TIMING_1	 
	0x31B8, 0x2048, 	// MIPI_TIMING_2	   
	0x31BA, 0x0104, 	// MIPI_TIMING_3	   
	0x31BC, 0x0004, 	// MIPI_TIMING_4	   	
	//0x31BC, 0x8004, 	// MIPI_TIMING_4  //Rogers: MIPI Clk cont
	//                                                     
	
	0x3002, 314  ,		// Y_ADDR_START_					   
	0x3006, 2147	,	// Y_ADDR_END_							 
	0x3004, 8			,	// X_ADDR_START_						 
	0x3008, 3269	,	// X_ADDR_END_							 
	0x3040, 0x68C3, 	// READ_MODE							 
	0x300C, 3860		,	// LINE_LENGTH_PCK_ 					 
	0x300A, 985 	,	// 1166-100fps//FRAME_LENGTH_LINE_		 
	0x3012,  984		,	// COARSE_INTEGRATION_TIME_ 			 
	0x3018, 1703	,	// extra_delay							 
	0x0400, 0x0002, 	// SCALING_MODE 						 
	0x0402, 0x0000, 	// SPATIAL_SAMPLING 					 
	0x0404, 0x0014, 	// SCALE_M								 
	0x0408, 0x1204, 	// SECOND_RESIDUAL						 
	0x040A, 0x018D, 	// SECOND_CROP						   
	0x306E, 0x90B0, 	// DATA_PATH_SELECT 					 
	0x034C, 1288	,	// X_OUTPUT_SIZE						 
	0x034E, 728  ,		// Y_OUTPUT_SIZE					   
	0x3120, 0x0031, 	// GAIN_DITHER_CONTROL					 
	0x301A, 0x001C, 	// RESET_REGISTER						 	             
	        
	//STATE = Master Clock, 228000000  
};
#endif


//Mode 1: Video Record 720(1280x720)@60P
//[FOX: 720p 1288x728 59.94fps vt=448 op=672Mbps FOV=3264x1836]
//720p60 with scaling only to compare the power consumption
MMP_USHORT SNR_AR0835_Reg_1288x728_60P_Customer[] = 
{
	// 228bps
	0x301A, 0x0018, 	// RESET_REGISTER                   

	SENSOR_DELAY_REG, 2, 
	
	0x31AE, 0x0204, 	// SERIAL_FORMAT		   
	0x0112, 0x0A0A, 	// CCP_DATA_FORMAT		   
	0x3F3A, 0xFF03, 	// ANALOG_CONTROL8		   
	0x0300, 0x0003, 	// VT_PIX_CLK_DIV		   
	0x0302, 0x0001, 	// VT_SYS_CLK_DIV		   
	0x0304, 0x0002, 	// PRE_PLL_CLK_DIV		   
	0x0306, 56			,	// PLL_MULTIPLIER		   
	0x0308, 0x000A, 	// OP_PIX_CLK_DIV		   
	0x030A, 0x0001, 	// OP_SYS_CLK_DIV		 
	0x3016, 0x0111, 	// ROW_SPEED			   
	0x3064, 0x5800, 	// SMIA_TEST			   
	
	//							 
	//LOAD = MIPI Timing-672mbps               
	
	0x31B0, 0x0058, 	// FRAME_PREAMBLE	   
	0x31B2, 0x003D, 	// LINE_PREAMBLE	   
	0x31B4, 0x1A56, 	// MIPI_TIMING_0	   
	0x31B6, 0x11D4, 	// MIPI_TIMING_1	   
	0x31B8, 0x304A, 	// MIPI_TIMING_2	   
	0x31BA, 0x0209, 	// MIPI_TIMING_3	   
	0x31BC, 0x0007, 	// MIPI_TIMING_4	   		
	//0x31BC, 0x8004, 	// MIPI_TIMING_4  //Rogers: MIPI Clk cont
	//                                                     
	
	
	0x3002, 314 	,	// Y_ADDR_START_						
	0x3006, 2147	,	// Y_ADDR_END_							
	0x3004, 8		,	// X_ADDR_START_						
	0x3008, 3271	,	// X_ADDR_END_							
	0x3040, 0x4041	,	// READ_MODE							
	0x300C, 3860	,	// LINE_LENGTH_PCK_ 					
	0x300A, 1936	,	// 1166-100fps//FRAME_LENGTH_LINE_		
	0x3012,  1834	,	// COARSE_INTEGRATION_TIME_ 			
	0x3018, 0		,	// extra_delay							
	0x0400, 0x0002 ,	// SCALING_MODE 						
	0x0402, 0x0000 ,	// SPATIAL_SAMPLING 					
	0x0404, 0x0028 ,	// SCALE_M								
	0x0408, 0x2004 ,	// SECOND_RESIDUAL						
	0x040A, 0x00A6	,	// SECOND_CROP							
	0x306E, 0x9090 ,	// DATA_PATH_SELECT 					
	0x034C, 1288	,	// X_OUTPUT_SIZE						
	0x034E, 728 	,	// Y_OUTPUT_SIZE						
	0x3120, 0x0031 ,	// GAIN_DITHER_CONTROL					
	0x301A, 0x001C ,	// RESET_REGISTER						
	          
	        
	//STATE = Master Clock, 448000000  
};

//Mode 2: Video Record 720(1280x720)@30P
//[FOX: 720p 1288x728 29.97fps vt=192 op=192Mbps FOV=3264x1836]
MMP_USHORT SNR_AR0835_Reg_1288x728_30P_Customer[] = 
{
	// 192bps
	0x301A, 0x0018, 	// RESET_REGISTER                   

	SENSOR_DELAY_REG, 2, 
	
	0x31AE, 0x0204, 	// SERIAL_FORMAT					
	0x0112, 0x0A0A, 	// CCP_DATA_FORMAT					
	0x3F3A, 0xFF03, 	// ANALOG_CONTROL8					
	0x0300, 0x0004, 	// VT_PIX_CLK_DIV					
	0x0302, 0x0001, 	// VT_SYS_CLK_DIV					
	0x0304, 0x0002, 	// PRE_PLL_CLK_DIV					
	0x0306, 32			,	// PLL_MULTIPLIER					
	0x0308, 0x000A, 	// OP_PIX_CLK_DIV					
	0x030A, 0x0002, // OP_SYS_CLK_DIV					
	0x3016, 0x0111, 	// ROW_SPEED						
	0x3064, 0x5800, 	// SMIA_TEST						
	
	//							 
	//LOAD = MIPI Timing-312mbps 
	                          
	0x31B0, 0x003B, // FRAME_PREAMBLE //		
	0x31B2, 0x0029, // LINE_PREAMBLE			
	0x31B4, 0x1533, 	// MIPI_TIMING_0			
	0x31B6, 0x11D4, // MIPI_TIMING_1			
	0x31B8, 0x2048, 	// MIPI_TIMING_2			
	0x31BA, 0x0104, 	// MIPI_TIMING_3			
	0x31BC, 0x0004, 	// MIPI_TIMING_4					
	//0x31BC, 0x8004, 	// MIPI_TIMING_4  //Rogers: MIPI Clk cont
	//                                                     
	
	0x3002, 314  ,	// Y_ADDR_START_					 
	0x3006, 2147	,	// Y_ADDR_END_						 
	0x3004, 8			,	// X_ADDR_START_					 
	0x3008, 3269	,	// X_ADDR_END_						 
	0x3040, 0x68C3, 	// READ_MODE						 
	0x300C, 3860		,	// LINE_LENGTH_PCK_ 				 
	0x300A, 1659		,	// 1166-100fps//FRAME_LENGTH_LINE_	 
	0x3012,  1657	,	// COARSE_INTEGRATION_TIME_ 		 
	0x3018,  2666	,	// extra_delay						 
	0x0400, 0x0002, 	// SCALING_MODE 					 
	0x0402, 0x0000, 	// SPATIAL_SAMPLING 				 
	0x0404, 0x0014, 	// SCALE_M							 
	0x0408, 0x1204, 	// SECOND_RESIDUAL					 
	0x040A, 0x018D, // SECOND_CROP						 
	0x306E, 0x90B0, 	// DATA_PATH_SELECT 				 
	0x034C, 1288	,	// X_OUTPUT_SIZE					 
	0x034E, 728  ,	// Y_OUTPUT_SIZE					 
	0x3120, 0x0031, 	// GAIN_DITHER_CONTROL				 
	0x301A, 0x001C, 	// RESET_REGISTER					 		 	             
	        
	//STATE = Master Clock, 192000000  
};

//Mode 3: Video Record 848x480@30P
//[FOX: WVGA  856x488 29.97fps vt=192 op=192Mbps FOV=3264x1836]
MMP_USHORT SNR_AR0835_Reg_856x488_30P_Customer[] = 
{
	// 192bps
	0x301A, 0x0018, 	// RESET_REGISTER                   

	SENSOR_DELAY_REG, 2, 
	
	0x31AE, 0x0204, 	// SERIAL_FORMAT	  
	0x0112, 0x0A0A, 	// CCP_DATA_FORMAT	  
	0x3F3A, 0xFF03, 	// ANALOG_CONTROL8	  
	0x0300, 0x0004, 	// VT_PIX_CLK_DIV	  
	0x0302, 0x0001, 	// VT_SYS_CLK_DIV	  
	0x0304, 0x0002, 	// PRE_PLL_CLK_DIV	  
	0x0306, 32	  ,		// PLL_MULTIPLIER	  
	0x0308, 0x000A, 	// OP_PIX_CLK_DIV	  
	0x030A, 0x0002, 	// OP_SYS_CLK_DIV	
	0x3016, 0x0111, 	// ROW_SPEED		  
	0x3064, 0x5800, 	// SMIA_TEST		  
	
	//							 
	//LOAD = MIPI Timing-312mbps 
	                          	
	0x31B0, 0x003B, 	// FRAME_PREAMBLE // 
	0x31B2, 0x0029, 	// LINE_PREAMBLE	 
	0x31B4, 0x1533, 	// MIPI_TIMING_0	   
	0x31B6, 0x11D4, 	// MIPI_TIMING_1	 
	0x31B8, 0x2048, 	// MIPI_TIMING_2	   
	0x31BA, 0x0104, 	// MIPI_TIMING_3	   
	0x31BC, 0x0004, 	// MIPI_TIMING_4	   		  	   	
	//0x31BC, 0x8004, 	// MIPI_TIMING_4  //Rogers: MIPI Clk cont
	//                                                     
	
	0x3002, 314  ,		// Y_ADDR_START_					   
	0x3006, 2147	,	// Y_ADDR_END_							 
	0x3004, 8		,	// X_ADDR_START_						 
	0x3008, 3269	,	// X_ADDR_END_							 
	0x3040, 0x68C3, 	// READ_MODE							 
	0x300C, 3860	,	// LINE_LENGTH_PCK_ 					 
	0x300A, 1659	,	// 1166-100fps//FRAME_LENGTH_LINE_		 
	0x3012,  1657	,	// COARSE_INTEGRATION_TIME_ 			 
	0x3018,  2666	,	// extra_delay							 
	0x0400, 0x0002, 	// SCALING_MODE 						 
	0x0402, 0x0000, 	// SPATIAL_SAMPLING 					 
	0x0404, 0x001E, 	// SCALE_M							   
	0x0408, 0x121D, 	// SECOND_RESIDUAL						 
	0x040A, 0x00E7, 	// SECOND_CROP						   
	0x306E, 0x90B0, 	// DATA_PATH_SELECT 					 
	0x034C, 856  ,		// X_OUTPUT_SIZE					   
	0x034E, 488  ,		// Y_OUTPUT_SIZE					   
	0x3120, 0x0031, 	// GAIN_DITHER_CONTROL					 
	0x301A, 0x001C, 	// RESET_REGISTER						 
	
	//STATE = Master Clock, 192000000  
};

//Mode 4: Video Record 848x480@120P
//[FOX: WVGA  856x488 119.88fps vt=444 op=444Mbps FOV=3264x1836]
MMP_USHORT SNR_AR0835_Reg_856x488_120P_Customer[] = 
{
	// 444bps
	0x301A, 0x0018, 	// RESET_REGISTER                   

	SENSOR_DELAY_REG, 2, 
	
	0x31AE, 0x0204, 	// SERIAL_FORMAT		   
	0x0112, 0x0A0A, 	// CCP_DATA_FORMAT		   
	0x3F3A, 0xFF03, 	// ANALOG_CONTROL8		   
	0x0300, 0x0004, 	// VT_PIX_CLK_DIV		   
	0x0302, 0x0001, 	// VT_SYS_CLK_DIV		   
	0x0304, 0x0002, 	// PRE_PLL_CLK_DIV		   
	0x0306, 74	  ,	  	// PLL_MULTIPLIER		   
	0x0308, 0x000A, 	// OP_PIX_CLK_DIV		   
	0x030A, 0x0002, 	// OP_SYS_CLK_DIV		 
	0x3016, 0x0111, 	// ROW_SPEED			   
	0x3064, 0x5800, 	// SMIA_TEST			   	
	
	//							 
	//LOAD = MIPI Timing-312mbps 
	                          
	0x31B0, 0x003B, 	// FRAME_PREAMBLE //			
	0x31B2, 0x0029, 	// LINE_PREAMBLE				
	0x31B4, 0x1533, 	// MIPI_TIMING_0				  
	0x31B6, 0x11D4, 	// MIPI_TIMING_1				
	0x31B8, 0x2048, 	// MIPI_TIMING_2				  
	0x31BA, 0x0104, 	// MIPI_TIMING_3				  
	0x31BC, 0x0004, 	// MIPI_TIMING_4				  	   	
	//0x31BC, 0x8004, 	// MIPI_TIMING_4  //Rogers: MIPI Clk cont
	//                                                     
	
	0x3002, 314  ,		// Y_ADDR_START_				  
	0x3006, 2147	,	// Y_ADDR_END_						
	0x3004, 8		,	// X_ADDR_START_					
	0x3008, 3269	,	// X_ADDR_END_						
	0x3040, 0x68C3, 	// READ_MODE						
	0x300C, 3860	,	// LINE_LENGTH_PCK_ 				
	0x300A, 959 	,	// 1166-100fps//FRAME_LENGTH_LINE_	
	0x3012,  958	,	// COARSE_INTEGRATION_TIME_ 		
	0x3018,  1960	,	// extra_delay						
	0x0400, 0x0002, 	// SCALING_MODE 					
	0x0402, 0x0000, 	// SPATIAL_SAMPLING 				
	0x0404, 0x001E, 	// SCALE_M						  
	0x0408, 0x121D, 	// SECOND_RESIDUAL					
	0x040A, 0x00E7, 	// SECOND_CROP					  
	0x306E, 0x90B0, 	// DATA_PATH_SELECT 				
	0x034C, 856  ,		// X_OUTPUT_SIZE				  
	0x034E, 488  ,		// Y_OUTPUT_SIZE				  
	0x3120, 0x0031, 	// GAIN_DITHER_CONTROL				
	0x301A, 0x001C, 	// RESET_REGISTER					
	
	//STATE = Master Clock, 444000000  
};

//Mode 5: Photo 3200x1800
//[FOX:STILL1 3208x1808 15fps vt=256 op=384Mbps  FOV=3200x1800]
MMP_USHORT SNR_AR0835_Reg_3208x1808_15P_Customer[] = 
{
	// 384bps
	0x301A, 0x0018, 	// RESET_REGISTER                   

	SENSOR_DELAY_REG, 2, 
	
	0x31AE, 0x0204, 	// SERIAL_FORMAT		   
	0x0112, 0x0A0A, 	// CCP_DATA_FORMAT		   
	0x3F3A, 0xFF03, 	// ANALOG_CONTROL8		   
	0x0300, 0x0003, 	// VT_PIX_CLK_DIV		 
	0x0302, 0x0001, 	// VT_SYS_CLK_DIV		   
	0x0304, 0x0002, 	// PRE_PLL_CLK_DIV		   
	0x0306, 0x0020, 	// PLL_MULTIPLIER		 
	0x0308, 0x000A, 	// OP_PIX_CLK_DIV		   
	0x030A, 0x0001, 	// OP_SYS_CLK_DIV		   
	0x3016, 0x0111, 	// ROW_SPEED			   
	0x3064, 0x5800, 	// SMIA_TEST			   	
	
	//							 
	//LOAD = MIPI Timing-312mbps 
	                          	
	0x31B0, 0x003C, 	// FRAME_PREAMBLE	
	0x31B2, 0x002C, 	// LINE_PREAMBLE	  
	0x31B4, 0x1633, 	// MIPI_TIMING_0	  
	0x31B6, 0x11D4, 	// MIPI_TIMING_1	
	0x31B8, 0x2048, 	// MIPI_TIMING_2	  
	0x31BA, 0x0105, 	// MIPI_TIMING_3	  
	0x31BC, 0x0004, 	// MIPI_TIMING_4	  	  	   	
	//0x31BC, 0x8004, 	// MIPI_TIMING_4  //Rogers: MIPI Clk cont
	//                                                     
	
	0x3002, 328 	,	// Y_ADDR_START 			   
	0x3006, 2135	,	// Y_ADDR_END				   
	0x3004, 36		,	// X_ADDR_START 			   
	0x3008, 3243	,	// X_ADDR_END				   
	0x3040, 0x4041, 	// READ_MODE				   
	0x300C, 5880	,	// LINE_LENGTH_PCK			   
	0x300A, 2902	,	// FRAME_LENGTH_LINES		   
	0x3012, 2902	,	// COARSE_INTEGRATION_TIME	   
	0x0400, 0x0000, 	// SCALING_MODE 			   
	0x0402, 0x0000, 	// SPATIAL_SAMPLING 		   
	0x0404, 0x0010, 	// SCALE_M					   
	0x0408, 0x1010, 	// SECOND_RESIDUAL			   
	0x040A, 0x0210, 	// SECOND_CROP				   
	0x306E, 0x9080, 	// DATA_PATH_SELECT 		   
	0x034C, 3208	,	// X_OUTPUT_SIZE			   
	0x034E, 1808	,	// Y_OUTPUT_SIZE			   
	0x3120, 0x0031, 	// GAIN_DITHER_CONTROL		   
	0x301A, 0x001C, 	// RESET_REGISTER			   
				 
	//STATE = Master Clock, 256000000  
};

//Mode 6: Photo 2688x1512
//[FOX:STILL2 2696x1528 15fps vt=256 op=384Mbps  FOV=3200x1800]
MMP_USHORT SNR_AR0835_Reg_2696x1528_15P_Customer[] = 
{
	// 384bps
	0x301A, 0x0018, 	// RESET_REGISTER                   

	SENSOR_DELAY_REG, 2, 
	
	0x31AE, 0x0204, 	// SERIAL_FORMAT	   
	0x0112, 0x0A0A, 	// CCP_DATA_FORMAT	   
	0x3F3A, 0xFF03, 	// ANALOG_CONTROL8	   
	0x0300, 0x0003, 	// VT_PIX_CLK_DIV	 
	0x0302, 0x0001, 	// VT_SYS_CLK_DIV	   
	0x0304, 0x0002, 	// PRE_PLL_CLK_DIV	   
	0x0306, 0x0020, 	// PLL_MULTIPLIER	 
	0x0308, 0x000A, 	// OP_PIX_CLK_DIV	   
	0x030A, 0x0001, 	// OP_SYS_CLK_DIV	   
	0x3016, 0x0111, 	// ROW_SPEED		   
	0x3064, 0x5800, 	// SMIA_TEST		   	   	
	
	//							 
	//LOAD = MIPI Timing-312mbps 
	                          	
	0x31B0, 0x003C, 	// FRAME_PREAMBLE	 
	0x31B2, 0x002C, 	// LINE_PREAMBLE	   
	0x31B4, 0x1633, 	// MIPI_TIMING_0	   
	0x31B6, 0x11D4, 	// MIPI_TIMING_1	 
	0x31B8, 0x2048, 	// MIPI_TIMING_2	   
	0x31BA, 0x0105, 	// MIPI_TIMING_3	   
	0x31BC, 0x0004, 	// MIPI_TIMING_4	   		  	   	
	//0x31BC, 0x8004, 	// MIPI_TIMING_4  //Rogers: MIPI Clk cont
	//                                                     
	
	0x3002, 324 	,	// Y_ADDR_START 				
	0x3006, 2139		,	// Y_ADDR_END					
	0x3004, 36			,	// X_ADDR_START 				
	0x3008, 3243		,	// X_ADDR_END					
	0x3040, 0x4041, 	// READ_MODE					
	0x300C, 5880		,	// LINE_LENGTH_PCK				
	0x300A, 2902		,	// FRAME_LENGTH_LINES		  
	0x3012, 2902		,	// COARSE_INTEGRATION_TIME		
	0x0400, 0x0002, 	// SCALING_MODE 				
	0x0402, 0x0000, 	// SPATIAL_SAMPLING 			
	0x0404, 0x0013, 	// SCALE_M						
	0x0408, 0x0708, 	// SECOND_RESIDUAL			  
	0x040A, 0x01CE, 	// SECOND_CROP					
	0x306E, 0x9090, 	// DATA_PATH_SELECT 			
	0x034C, 2696		,	// X_OUTPUT_SIZE				
	0x034E, 1528	,	// Y_OUTPUT_SIZE				
	0x3120, 0x0031, 	// GAIN_DITHER_CONTROL			
	0x301A, 0x001C, 	// RESET_REGISTER				
	
	//STATE = Master Clock, 256000000  
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
    SensorCustFunc.OprTable->usInitSize                   			= (sizeof(SNR_AR0835_Reg_Init_Customer)/sizeof(SNR_AR0835_Reg_Init_Customer[0]))/2;
    SensorCustFunc.OprTable->uspInitTable                 			= &SNR_AR0835_Reg_Init_Customer[0];    
    
    SensorCustFunc.OprTable->bBinTableExist                         = MMP_FALSE;
    SensorCustFunc.OprTable->bInitDoneTableExist                    = MMP_FALSE;
    
	SensorCustFunc.OprTable->usSize[RES_IDX_1928x1096_30P]       	= (sizeof(SNR_AR0835_Reg_1928x1096_30P_Customer)/sizeof(SNR_AR0835_Reg_1928x1096_30P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1928x1096_30P]     	= &SNR_AR0835_Reg_1928x1096_30P_Customer[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_1288x728_60P]		  	= (sizeof(SNR_AR0835_Reg_1288x728_60P_Customer)/sizeof(SNR_AR0835_Reg_1288x728_60P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1288x728_60P] 	  	= &SNR_AR0835_Reg_1288x728_60P_Customer[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_1288x728_30P]		  	= (sizeof(SNR_AR0835_Reg_1288x728_30P_Customer)/sizeof(SNR_AR0835_Reg_1288x728_30P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1288x728_30P] 	  	= &SNR_AR0835_Reg_1288x728_30P_Customer[0];
	SensorCustFunc.OprTable->usSize[RES_IDX_856x488_30P]		  	= (sizeof(SNR_AR0835_Reg_856x488_30P_Customer)/sizeof(SNR_AR0835_Reg_856x488_30P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_856x488_30P] 	  		= &SNR_AR0835_Reg_856x488_30P_Customer[0];
	SensorCustFunc.OprTable->usSize[RES_IDX_856x488_120P]		  	= (sizeof(SNR_AR0835_Reg_856x488_120P_Customer)/sizeof(SNR_AR0835_Reg_856x488_120P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_856x488_120P] 	  	= &SNR_AR0835_Reg_856x488_120P_Customer[0];
	SensorCustFunc.OprTable->usSize[RES_IDX_3208x1808_15P]		  	= (sizeof(SNR_AR0835_Reg_3208x1808_15P_Customer)/sizeof(SNR_AR0835_Reg_3208x1808_15P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_3208x1808_15P] 	  	= &SNR_AR0835_Reg_3208x1808_15P_Customer[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_2696x1528_15P]       	= (sizeof(SNR_AR0835_Reg_2696x1528_15P_Customer)/sizeof(SNR_AR0835_Reg_2696x1528_15P_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_2696x1528_15P]     	= &SNR_AR0835_Reg_2696x1528_15P_Customer[0];
    
    // Init Vif Setting : Common
    SensorCustFunc.VifSetting->SnrType                              = MMPF_VIF_SNR_TYPE_BAYER;
    SensorCustFunc.VifSetting->OutInterface                         = MMPF_VIF_IF_MIPI_QUAD;
    #if (SUPPORT_DUAL_SNR)
    SensorCustFunc.VifSetting->VifPadId							    = MMPF_VIF_MDL_ID1;
    #else
	SensorCustFunc.VifSetting->VifPadId							    = MMPF_VIF_MDL_ID0;
    #endif
    
    // Init Vif Setting : PowerOn Setting
    SensorCustFunc.VifSetting->powerOnSetting.bTurnOnExtPower       = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPin         = SENSOR_GPIO_ENABLE; // it might be defined in Config_SDK.h
    SensorCustFunc.VifSetting->powerOnSetting.bExtPowerPinHigh      = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPinDelay    = 50;
    SensorCustFunc.VifSetting->powerOnSetting.bFirstEnPinHigh       = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubFirstEnPinDelay     = 1;
    SensorCustFunc.VifSetting->powerOnSetting.bNextEnPinHigh        = MMP_FALSE;
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
	SensorCustFunc.VifSetting->colorId.CustomColorId.bUseCustomId   = MMP_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_DoAE_FrmSt
//  Description :
//------------------------------------------------------------------------------
MMP_USHORT gsFixIsoMode = 0;	//0: auto, 1: iso100, 2: iso200, 3: iso400, 4: iso800, 5: iso1600, 6:iso3200 , 7:iso6400
void SNR_Cust_DoAE_FrmSt(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
#if (ISP_EN)

	if (ulFrameCnt % m_AeTime.ubPeriod == 0)
		ISP_IF_AE_Execute();
		
	if (ulFrameCnt % m_AeTime.ubPeriod == m_AeTime.ubFrmStSetShutFrmCnt)
		gsSensorFunction->MMPF_Sensor_SetShutter(ubSnrSel, 0, 0);
	
    if (ulFrameCnt % m_AeTime.ubPeriod == m_AeTime.ubFrmStSetGainFrmCnt)
    	gsSensorFunction->MMPF_Sensor_SetGain(ubSnrSel, ISP_IF_AE_GetGain());

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
//  Function    : SNR_Cust_SetGain
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetGain(MMP_UBYTE ubSnrSel, MMP_ULONG ulGain)
{
#if (ISP_EN)
	ISP_UINT16 sensor_again;
	ISP_UINT16 sensor_dgain;
	MMP_USHORT usRdVal;
    MMP_BOOL   bUseYSum = MMP_FALSE;
	
	gsSensorFunction->MMPF_Sensor_GetReg(ubSnrSel, 0x3040, &usRdVal);
	
	if((usRdVal & 0x2000) == 0) 
		bUseYSum = MMP_FALSE;
	else
		bUseYSum = MMP_TRUE;
		
	// Sensor Gain Mapping
	if(ulGain < 2 * ISP_IF_AE_GetGainBase()) {
		sensor_dgain = ulGain * 0x80 / (1 * ISP_IF_AE_GetGainBase());
		sensor_again = 0x00;     // 1X ~ 2X
	}
	else if (ulGain < 3 * ISP_IF_AE_GetGainBase()) {
		sensor_dgain = ulGain * 0x80 / (2 * ISP_IF_AE_GetGainBase());
		sensor_again = 0x01;    // 2X ~ 3X
	}       
	else if (ulGain < 4 * ISP_IF_AE_GetGainBase()) {
		sensor_dgain = ulGain * 0x80 / (3 * ISP_IF_AE_GetGainBase());
		sensor_again = 0x02;    // 3X ~ 4X
	}   
	else if (ulGain < 6 * ISP_IF_AE_GetGainBase()) {
		sensor_dgain = ulGain * 0x80 / (4 * ISP_IF_AE_GetGainBase());
		sensor_again = 0x03;    // 4X ~ 6X
	}   
	else if (ulGain < 8 * ISP_IF_AE_GetGainBase()) {
		sensor_dgain = ulGain * 0x80 / (6 * ISP_IF_AE_GetGainBase());
		sensor_again = 0x06;    // 6X ~ 8X
	}   
	else {
		sensor_dgain = ISP_MIN((ulGain * 0x80 / (8 * ISP_IF_AE_GetGainBase())), 0x1ff); 	//maxi = 512/128 = 4x
		sensor_again = 0x07;    // 8X ~ 16X ~ 32x
	}

	sensor_again = sensor_again | (sensor_dgain << 5);
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x305E, sensor_again);


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

MMPF_SENSOR_CUSTOMER  SensorCustFunc = 
{
    SNR_Cust_InitConfig,
    SNR_Cust_DoAE_FrmSt,
    SNR_Cust_DoAE_FrmEnd,
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

#endif // (BIND_SENSOR_AR0835)
#endif // (SENSOR_EN)

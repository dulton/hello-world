//==============================================================================
//
//  File        : sensor_ov10822.c
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
#if (BIND_SENSOR_OV10822)

#include "mmpf_sensor.h"
#include "Sensor_Mod_Remapping.h"
#include "isp_if.h"

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#define SENSOR_IF 			(SENSOR_IF_MIPI_4_LANE)

#define RES_IDX_3840x2160   (0)
#define RES_IDX_1920x1080	(1)

#define MODE_8M     		(0)
#define MODE_10M    		(1)
#define SENSOR_IN_PIXELS    (MODE_10M)
#if (SENSOR_IN_PIXELS == MODE_8M)       
#define SENSOR_IN_W     	(3848)
#define SENSOR_IN_H     	(2168) 
#define SENSOR_BASE_SHUTTER (2248)
#endif
#if (SENSOR_IN_PIXELS == MODE_10M)     
#define SENSOR_IN_W     	(4320) 
#define SENSOR_IN_H     	(2432)  
#define SENSOR_BASE_SHUTTER (2504)
#endif

#define MAX_SENSOR_GAIN 	(16)
#define ISP_DGAIN_BASE 		(0x200)

#define SET_WORD(addr,v)  addr,   ( (v) >> 8  )&0xff, \
                          addr+1 ,( (v) & 0xff)

//==============================================================================
//
//                              GLOBAL VARIABLE
//
//==============================================================================

// Resolution Table
MMPF_SENSOR_RESOLUTION m_SensorRes = 
{
	2,				// ubSensorModeNum
	0,				// ubDefPreviewMode
	0,				// ubDefCaptureMode
	1400,           // usPixelSize
//  Mode0   Mode1
    {1,     1},		// usVifGrabStX
    {1,     1},		// usVifGrabStY
    {3848,  1928},	// usVifGrabW
    {2168,  1092},	// usVifGrabH
    #if (CHIP == MCR_V2)
	{1,   	1}, 	// usBayerInGrabX
	{1,   	1}, 	// usBayerInGrabY
    {8,     8},  	// usBayerInDummyX
    {8,     12},  	// usBayerInDummyY
    {1920,  1920},	// usBayerOutW
    {1080,  1080},	// usBayerOutH
    #endif
    {1920,  1920},	// usScalInputW
    {1080,  1080},	// usScalInputH
    {300,   300},	// usTargetFpsx10
    {2248,  1136},	// usVsyncLine
    {1,		1},  	// ubWBinningN
    {1,		1},  	// ubWBinningM
    {1,		1},  	// ubHBinningN
    {1,		1},  	// ubHBinningM
    {0xFF,	0xFF},  // ubCustIQmode
    {0xFF,  0xFF}   // ubCustAEmode
};

// OPR Table and Vif Setting
MMPF_SENSOR_OPR_TABLE       m_OprTable;
MMPF_SENSOR_VIF_SETTING     m_VifSetting;

// IQ Table
const ISP_UINT8 Sensor_IQ_CompressedText[] = 
{
#ifdef CUS_ISP_8428_IQ_DATA     // maybe defined in project MCP or Config_SDK.h
#include CUS_ISP_8428_IQ_DATA
#else
#include "isp_8453_iq_data_v2_MT9T002.xls.ciq.txt"
#endif
};

// I2cm Attribute
static MMP_I2CM_ATTR m_I2cmAttr = {
    MMP_I2CM0,  // i2cmID
    0x36,       // ubSlaveAddr
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
    250000,     // ulI2cmSpeed 250KHZ
    MMP_FALSE,  // bOsProtectEn
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
	2	/* ubDoCaliFrmCnt */
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

#if (ISP_EN)
static ISP_UINT32 dgain, s_gain;
#endif

unsigned char metering_weight_tbl[128] = 
{
	//ISP_AE_METERING_CENTER
	0xc1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x00, 0x00,
	0x00, 0x00, 0x33, 0x34, 0x43, 0x33, 0x00, 0x00,
	0x00, 0x00, 0x33, 0x34, 0x43, 0x33, 0x00, 0x00,
	0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0,	
};

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

#if 0
void ____Sensor_Init_OPR_Table____(){ruturn;} //dummy
#endif
 
MMP_USHORT SNR_OV10822_Reg_Init_Customer[] = 
{
	0x3080, 0x04,      
	0x3083, 0x00,      
	0x3084, 0x03,      
	0x308B, 0x05,      
	0x308D, 0xAE,      
	0x308E, 0x00,      
	0x308F, 0x09,      
	                   
	0x3011, 0xD0,      
	0x3012, 0x41,      
	0x3031, 0x0C,      
	0x3503, 0x07,      
	0x3603, 0x04,      
	0x3641, 0xA3,      
	0x3643, 0x02,      
	0x3645, 0x41,      
	0x3646, 0x83,      
	0x3648, 0x0D,      
	0x3649, 0x66,      
	0x3653, 0x37,      
	0x3658, 0x86,      
	0x3659, 0x57,      
	0x365C, 0x5A,      
	0x3660, 0x82,      
	0x371B, 0x30,      
	0x371E, 0x01,      
	0x371F, 0x20,      
	0x3720, 0xA0,      
	0x372A, 0x08,      
	0x371C, 0x40,      
	0x372C, 0x11,      
	0x3723, 0x78,      
	0x3821, 0x04,      
	0x382E, 0x04,      
	0x3834, 0x00,      
	0x3901, 0x80,      
	0x3902, 0xC6,      
	0x4004, 0x00,      
	0x4005, 0x20,// Fisher test
	0x400D, 0x10,      
	0x401B, 0x00,      
	0x401D, 0x00,      
	0x401F, 0x00,      
	0x4020, 0x01,      
	0x4022, 0x03,      
	0x4024, 0x0E,      
	0x4025, 0x10,      
	0x4026, 0x0F,      
	0x4027, 0x9F,      
	0x4028, 0x00,      
	0x4029, 0x04,      
	0x402D, 0x04,      
	0x402F, 0x08,      
	0x4602, 0x02,      
	0x4810, 0x00,      
	0x4811, 0x1F,      
	0x4819, 0xA0,      
	0x481B, 0x35,      
	0x481F, 0x30,      
	0x4823, 0x35,      
	0x4D00, 0x04,      
	0x4D01, 0x71,      
	0x4D03, 0xF5,      
	0x4D04, 0x0C,      
	0x4D05, 0xCC,      
	0x4D0B, 0x01,      
	0x5002, 0x08,      
	0x5A04, 0x00,      
	0x5A06, 0x00,      
	0x5B01, 0x10,      
	0x5B04, 0x02,      
	0x5B05, 0xEC,      
	0x5B09, 0x02,      
	0x5E10, 0x0C,      
	                   
	0x3082, 0x57,			
	0x3082, 0x57,      
	                   
	0x3092, 0x03,      
	                   
	0x300D, 0x23,      
	0x301B, 0xB4,      
	0x3033, 0x73,      
	0x3034, 0xB6,      
	0x3035, 0x8A,      
	0x3036, 0xCD,      
	0x3037, 0x8A,      
	0x3038, 0xCD,      
	                   
	0x3642, 0x33,      
	0x3663, 0x01,      
	                   
	0x3700, 0x44,      
	0x3701, 0x24,      
	0x3702, 0xB4,      
	0x3703, 0x78,      
	0x3704, 0x3C,      
	0x3705, 0x22,      
	0x3706, 0x1E,      
	0x3707, 0x75,      
	0x3708, 0x84,      
	0x3709, 0x50,      
	0x370A, 0x31,      
	0x370B, 0x40,      
	0x370C, 0xFF,      
	0x370D, 0x03,      
	0x370E, 0x0F,      
	0x370F, 0x45,      
	0x3710, 0x30,      
	0x3714, 0x42,      
	0x3719, 0x0C,      
	0x3721, 0xF0,      
	0x374C, 0x58,      
	                   
	0x3800, 0x00,      
	0x3801, 0xF4,
	0x3802, 0x00,      
	0x3803, 0x90, 
	0x3804, 0x10,      
	0x3805, 0x0B,
	0x3806, 0x09,      
	0x3807, 0x0F,
	0x3808, 0x0F,
	0x3809, 0x08, 
	0x380A, 0x08,
	0x380B, 0x78,
	0x380C, 0x18,
	0x380D, 0x1A, 
	0x380E, 0x08, 
	0x380F, 0xD0, 
	0x3810, 0x00,      
	0x3811, 0x08,      
	0x3812, 0x00,      
	0x3813, 0x04,      
	                   
	0x3815, 0x11,      
	0x3820, 0x00,      
	0x3834, 0x00,      
	0x4001, 0x00,      
	0x4003, 0x30,      
	0x402A, 0x0C,      
	0x402B, 0x08,      
	0x402E, 0x1C,      
	0x4837, 0x12,      
	            
	0x3500, 0x00,      
	0x3501, 0x9C,      
	0x3502, 0x00,      
	0x350B, 0x40,      
	                   
	0x0100, 0x01
};
        
#if 0
void ____Sensor_Res_OPR_Table____(){ruturn;} //dummy
#endif
            
MMP_USHORT SNR_OV10822_Reg_3840x2160_Customer[] = 
{
	0x3080, 0x04,
	0x3083, 0x00,      
	0x3084, 0x03,      
	0x308B, 0x05, 
	//sys pll
	0x308D, 0xAC,
	0x308E, 0x00,      
	0x308F, 0x09,      
	                   
	0x3011, 0xD0,      
	0x3012, 0x41,      
	0x3031, 0x0C,      
	0x3503, 0x07,      
	0x3603, 0x04,      
	0x3641, 0xA3,      
	0x3643, 0x02,      
	0x3645, 0x41,      
	0x3646, 0x83,      
	0x3648, 0x0D,      
	0x3649, 0x66,      
	0x3653, 0x37,      
	0x3658, 0x86,      
	0x3659, 0x57,      
	0x365C, 0x5A,      
	0x3660, 0x82,      
	0x371B, 0x30,      
	0x371E, 0x01,      
	0x371F, 0x20,      
	0x3720, 0xA0,      
	0x372A, 0x08,      
	0x371C, 0x40,      
	0x372C, 0x11,      
	0x3723, 0x78,      
	0x3821, 0x04,      
	0x382E, 0x04,      
	0x3834, 0x00,      
	0x3901, 0x80,      
	0x3902, 0xC6,      
	0x4004, 0x00,      
	0x4005, 0x20,// Fisher test
	0x400D, 0x10,      
	0x401B, 0x00,      
	0x401D, 0x00,      
	0x401F, 0x00,      
	0x4020, 0x01,      
	0x4022, 0x03,      
	0x4024, 0x0E,      
	0x4025, 0x10,      
	0x4026, 0x0F,      
	0x4027, 0x9F,      
	0x4028, 0x00,      
	0x4029, 0x04,      
	0x402D, 0x04,      
	0x402F, 0x08,      
	0x4602, 0x02,      
	0x4810, 0x00,      
	0x4811, 0x1F,      
	0x4819, 0xA0,      
	0x481B, 0x35,      
	0x481F, 0x30,      
	0x4823, 0x35,      
	0x4D00, 0x04,      
	0x4D01, 0x71,      
	0x4D03, 0xf5,      
	0x4D04, 0x0C,      
	0x4D05, 0xCC,      
	0x4D0B, 0x01,      
	0x5002, 0x08,      
	0x5A04, 0x00,      
	0x5A06, 0x00,      
	0x5B01, 0x10,      
	0x5B04, 0x02,      
	0x5B05, 0xEC,      
	0x5B09, 0x02,      
	0x5E10, 0x0C,
	      
	// MIPI 4-Lane 4336x2440 10-bit 30fps 864Mbps/lane                   
	0x3082, 0x6C,// mipi pll div
	0x3092, 0x03,      
	                   
	0x300D, 0x23,      
	0x301B, 0xB4,      
	0x3033, 0x73,      
	0x3034, 0xB6,      
	0x3035, 0x8A,      
	0x3036, 0xCD,      
	0x3037, 0x8A,      
	0x3038, 0xCD,      
	                   
	0x3642, 0x33,      
	0x3663, 0x01,      
	                   
	0x3700, 0x44,      
	0x3701, 0x24,      
	0x3702, 0xB4,      
	0x3703, 0x78,      
	0x3704, 0x3C,      
	0x3705, 0x22,      
	0x3706, 0x1E,      
	0x3707, 0x75,      
	0x3708, 0x84,      
	0x3709, 0x50,      
	0x370A, 0x31,      
	0x370B, 0x40,      
	0x370C, 0xFF,      
	0x370D, 0x03,      
	0x370E, 0x0F,      
	0x370F, 0x45,      
	0x3710, 0x30,      
	0x3714, 0x42,      
	0x3719, 0x0C,      
	0x3721, 0xF0,      
	0x374C, 0x58,      

	SET_WORD(0x3800,0 ),        	// x_start
	SET_WORD(0x3802,8 ),      		// y_start
	SET_WORD(0x3804,4351),      	// x_end
	SET_WORD(0x3806,2455),      	// y_end
	SET_WORD(0x3808,SENSOR_IN_W),	// x_output_size
	SET_WORD(0x380A,SENSOR_IN_H),  	// y_output_size
	SET_WORD(0x380C,5500),      	// HTS
	SET_WORD(0x380E,SENSOR_BASE_SHUTTER),      // VTS
	SET_WORD(0x3810,8),  			// isp_x_offset
	SET_WORD(0x3812,4),    			// isp_y_offset  
	                   
	0x3815, 0x11,      
	0x3820, 0x00,      
	0x3834, 0x00,      
	0x4001, 0x00,      
	0x4003, 0x30,      
	0x402A, 0x0C,      
	0x402B, 0x08,      
	0x402E, 0x1C,      
	0x4837, 0x12,      
	 
	0x3500, 0x00,      
	0x3501, 0x9C,      
	0x3502, 0x00,      
	0x350B, 0x40,      
	                   
	0x0100, 0x01
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
    SensorCustFunc.OprTable->usInitSize                   = (sizeof(SNR_OV10822_Reg_Init_Customer)/sizeof(SNR_OV10822_Reg_Init_Customer[0]))/2;
    SensorCustFunc.OprTable->uspInitTable                 = &SNR_OV10822_Reg_Init_Customer[0];    
    
    SensorCustFunc.OprTable->bBinTableExist                 = MMP_FALSE;
    SensorCustFunc.OprTable->bInitDoneTableExist            = MMP_FALSE;
    
    SensorCustFunc.OprTable->usSize[RES_IDX_3840x2160]    = (sizeof(SNR_OV10822_Reg_3840x2160_Customer)/sizeof(SNR_OV10822_Reg_3840x2160_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_3840x2160]  = &SNR_OV10822_Reg_3840x2160_Customer[0];

    SensorCustFunc.OprTable->usSize[RES_IDX_1920x1080]    = (sizeof(SNR_OV10822_Reg_3840x2160_Customer)/sizeof(SNR_OV10822_Reg_3840x2160_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1920x1080]  = &SNR_OV10822_Reg_3840x2160_Customer[0];    
    
    // Init Vif Setting : Common
    SensorCustFunc.VifSetting->SnrType                              = MMPF_VIF_SNR_TYPE_BAYER;
    SensorCustFunc.VifSetting->OutInterface                         = MMPF_VIF_IF_MIPI_QUAD;
    #if (SUPPORT_DUAL_SNR)
    SensorCustFunc.VifSetting->VifPadId							    = MMPF_VIF_MDL_ID1;
    #else
    SensorCustFunc.VifSetting->VifPadId							    = MMPF_VIF_MDL_ID0;
    #endif
    
    // Init Vif Setting : PowerOn Setting
    SensorCustFunc.VifSetting->powerOnSetting.bTurnOnExtPower       = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPin         = SENSOR_GPIO_ENABLE; // it might be defined in Config_SDK.h
    SensorCustFunc.VifSetting->powerOnSetting.bExtPowerPinHigh	    = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPinDelay    = 50;
    SensorCustFunc.VifSetting->powerOnSetting.bFirstEnPinHigh       = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubFirstEnPinDelay     = 1;
    SensorCustFunc.VifSetting->powerOnSetting.bNextEnPinHigh        = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubNextEnPinDelay      = 10;
    SensorCustFunc.VifSetting->powerOnSetting.bTurnOnClockBeforeRst = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.bFirstRstPinHigh      = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOnSetting.ubFirstRstPinDelay    = 5;
    SensorCustFunc.VifSetting->powerOnSetting.bNextRstPinHigh       = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubNextRstPinDelay     = 20;
    
    // Init Vif Setting : PowerOff Setting
    SensorCustFunc.VifSetting->powerOffSetting.bEnterStandByMode    = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOffSetting.usStandByModeReg     = 0x0;
    SensorCustFunc.VifSetting->powerOffSetting.usStandByModeMask    = 0x0;    
    SensorCustFunc.VifSetting->powerOffSetting.bEnPinHigh           = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOffSetting.ubEnPinDelay         = 1;
    SensorCustFunc.VifSetting->powerOffSetting.bTurnOffMClock       = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOffSetting.bTurnOffExtPower     = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOffSetting.usExtPowerPin        = SENSOR_GPIO_ENABLE; // it might be defined in Config_SDK.h
    
    // Init Vif Setting : Sensor MClock Setting
    SensorCustFunc.VifSetting->clockAttr.bClkOutEn                  = MMP_TRUE; 
    SensorCustFunc.VifSetting->clockAttr.ubClkFreqDiv               = 0;
    SensorCustFunc.VifSetting->clockAttr.ulMClkFreq                 = 24000;
    SensorCustFunc.VifSetting->clockAttr.ulDesiredFreq              = 24000;
    SensorCustFunc.VifSetting->clockAttr.ubClkPhase                 = MMPF_VIF_SNR_PHASE_DELAY_NONE;
    SensorCustFunc.VifSetting->clockAttr.ubClkPolarity              = MMPF_VIF_SNR_CLK_POLARITY_POS;
    SensorCustFunc.VifSetting->clockAttr.ubClkSrc				    = MMPF_VIF_SNR_CLK_SRC_PMCLK;
    
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
    SensorCustFunc.VifSetting->colorId.VifColorId                   = MMPF_VIF_COLORID_11;
	SensorCustFunc.VifSetting->colorId.CustomColorId.bUseCustomId   = MMP_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_DoAE_FrmSt
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_DoAE_FrmSt(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
#if (ISP_EN)

	MMP_UBYTE   ubPeriod              = (SensorCustFunc.pAeTime)->ubPeriod;
	MMP_UBYTE   ubFrmStSetShutFrmCnt  = (SensorCustFunc.pAeTime)->ubFrmStSetShutFrmCnt;	
	MMP_UBYTE   ubFrmStSetGainFrmCnt  = (SensorCustFunc.pAeTime)->ubFrmStSetGainFrmCnt;

	if(ulFrameCnt % ubPeriod == ubFrmStSetShutFrmCnt) {

		ISP_IF_AE_Execute();

        s_gain = VR_MAX(ISP_IF_AE_GetGain(), ISP_IF_AE_GetGainBase());

        if (s_gain >= ISP_IF_AE_GetGainBase() * MAX_SENSOR_GAIN) {
            s_gain  = ISP_IF_AE_GetGainBase() * MAX_SENSOR_GAIN;
            dgain 	= ISP_DGAIN_BASE*s_gain /ISP_IF_AE_GetGainBase() / MAX_SENSOR_GAIN ;
        }
        else{
        	dgain	= ISP_DGAIN_BASE;
        }
		gsSensorFunction->MMPF_Sensor_SetShutter(ubSnrSel, 0, 0);
    }
	if(ulFrameCnt % ubPeriod == ubFrmStSetGainFrmCnt) {
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
   	switch (ulFrameCnt % 6) {
	case 2:
		//ISP_IF_IQ_SetAEGain(dgain,ISP_DGAIN_BASE);
	break;
	}
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
	
	sensor_again = ISP_MAX(ulGain,ISP_IF_AE_GetGainBase()) * 0x10 / ISP_IF_AE_GetGainBase();
	sensor_again = ISP_MIN(sensor_again, 0x3ff);
	
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x350A, (sensor_again >> 8) & 0x03);
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x350B, (sensor_again >> 0) & 0xFF);
#endif
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetShutter
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetShutter(MMP_UBYTE ubSnrSel, MMP_ULONG shutter, MMP_ULONG vsync)
{
	// TBD
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

#endif // (BIND_SENSOR_OV10822)
#endif // (SENSOR_EN)

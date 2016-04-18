//==============================================================================
//
//  File        : sensor_imx175.c
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
#if (BIND_SENSOR_IMX175)

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
	2,				// ubSensorModeNum
	0,				// ubDefPreviewMode
	0,				// ubDefCaptureMode
	1400,           // usPixelSize
//  Mode0   Mode1
    {1,   	1	},	// usVifGrabStX
    {1,    	1	},	// usVifGrabStY
    {3272,  1156},	// usVifGrabW
    {1840,  652	},	// usVifGrabH
    #if (CHIP == MCR_V2)
	{1,   	1   }, 	// usBayerInGrabX
	{1,   	1   }, 	// usBayerInGrabY
    {8,     4	},  // usBayerInDummyX
    {4,    	4	},  // usBayerInDummyY
    {1920,  1152},	// usBayerOutW
    {1080,  648	},	// usBayerOutH
    #endif
    {1920,  800	},	// usScalInputW
    {1080,  600	},	// usScalInputH
    {300,   600	},	// usTargetFpsx10
    {1928,  672	},	// usVsyncLine
    {1,		1	},  // ubWBinningN
    {1,		1	},  // ubWBinningM
    {1,		1	},  // ubHBinningN
    {1,		1	},  // ubHBinningM
    {0xFF,	0xFF},  // ubCustIQmode
    {0xFF,	0xFF}   // ubCustAEmode
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
#include "isp_8590_iq_data_v2_IMX175_v1.xls.ciq.txt"
#endif
};

// I2cm Attribute
static MMP_I2CM_ATTR m_I2cmAttr = {
    MMP_I2CM0,  // i2cmID
    0x10,       // ubSlaveAddr
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

#if (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)

MMP_USHORT SNR_IMX175_Reg_Init_Customer[] = 
{
	SENSOR_DELAY_REG, 100
};

#elif (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)

MMP_USHORT SNR_IMX175_Reg_Init_Customer[] = 
{
	0x0100, 0x0,
	0x0103, 0x1,
	0x0202, 0x9,
	0x0203, 0xAA,

	0x0301, 0x5,
	0x0303, 0x1,
	0x0305, 0xC,
	0x0309, 0x5, 
	0x030B, 0x1,
	0x030C, 0x0,
	0x030D, 0xA0,
	0x030E, 0x1,
	0x0340, 0x9,
	0x0341, 0xAE,

	0x0342, 0xD,
	0x0343, 0x70,
	0x0344, 0x0,
	0x0345, 0x0,
	0x0346, 0x0, 
	0x0347, 0x0,
	0x0348, 0xC,
	0x0349, 0xCF,
	0x034A, 0x9,
	0x034B, 0x9F,
	0x034C, 0xC,
	0x034D, 0xD0,
	0x034E, 0x9,
	0x034F, 0xA0,
	0x0390, 0x0,

	0x0600, 0,
	0x0601, 0,
	0x0602, 0,
	0x0603, 0,
	0x0604, 0,
	0x0605, 0,
	0x0606, 0,
	0x0607, 0,
	0x0608, 0,
	0x0609, 0,
	0x060A, 0,
	0x060B, 0,
	0x060C, 0,
	0x060D, 0,
	0x060E, 0,
	0x060F, 0,
	0x0610, 0,
	0x0611, 0,

	0x3020, 0x10,
	0x302D, 0x02,
	0x302F, 0x80,
	0x3032, 0xA3,
	0x3033, 0x20,
	0x3034, 0x24,
	0x3041, 0x15,
	0x3042, 0x87,
	0x3050, 0x35,
	0x3056, 0x57,
	0x305D, 0x41,
	0x3097, 0x69,
	0x3109, 0x41,
	0x3148, 0x3F,
	0x330F, 0x07,
	0x3344, 0x5F,
	0x3345, 0x1F,
    0x3364, 0x0,   	// 0x2 For two lane, 0x0 for 4 lane
	0x3368, 0x18,
	0x3369, 0x00,
	0x3370, 0x8F,
	0x3371, 0x4F,	// Prepare
	0x3372, 0x6F,	// zero
	0x3373, 0x4F,
	0x3374, 0x4F,
	0x3375, 0x4F,
	0x3376, 0x9F,
	0x3377, 0x47,
	0x33D4, 0x0C,
	0x33D5, 0xD0,
	0x33D6, 0x9,
	0x33D7, 0xA0,
	0x4100, 0x0E,
	0x4104, 0x32,
	0x4105, 0x32,
	0x4108, 0x01,
	0x4109, 0x7C,

	0x410A, 0x00,
	0x410B, 0x00,
	0x0100, 0x1,
};
#endif

#if 0
void ____Sensor_Res_OPR_Table____(){ruturn;} //dummy
#endif

#if (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)

ISP_UINT16 SNR_AR0330_Reg_3264x1836[] = 
{
	SENSOR_DELAY_REG, 100
};

ISP_UINT16 SNR_AR0330_Reg_1152x648[] = 
{
	SENSOR_DELAY_REG, 100
};

#elif (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)		

ISP_UINT16 SNR_IMX175_Reg_3264x1836[] = 
{
    0x0100, 0x00,
	0x030C, 0x0,	//0x0, --> 8M(1080P) 30fps
	0x030D, 0xFA,
    0x0340, 0x07,
    0x0341, 0x88,
    0x0342, 0x0D,
    0x0343, 0x78,    
    0x0346, 0x01,
    0x0347, 0x34,
    0x034A, 0x08,
    0x034B, 0x6B,
    0x034C, 0x0C,
    0x034D, 0xD0,
    0x034E, 0x07,
    0x034F, 0x38,    
    0x0381, 0x01,
    0x0383, 0x01,    
    0x0385, 0x01,
    0x0387, 0x01,    
    0x0202, 0x07,  // shutter
    0x0203, 0x84,  // shutter    
    0x33D4, 0x0C,  
    0x33D5, 0xD0,  
    0x33D6, 0x07,  
    0x33D7, 0x38,  

   	0x0100, 0x01
};

ISP_UINT16 SNR_IMX175_Reg_1152x648[] = 
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
    SensorCustFunc.OprTable->usInitSize                   		= (sizeof(SNR_IMX175_Reg_Init_Customer)/sizeof(SNR_IMX175_Reg_Init_Customer[0]))/2;
    SensorCustFunc.OprTable->uspInitTable                 		= &SNR_IMX175_Reg_Init_Customer[0];

    SensorCustFunc.OprTable->bBinTableExist                     = MMP_FALSE;
    SensorCustFunc.OprTable->bInitDoneTableExist                = MMP_FALSE;

    SensorCustFunc.OprTable->usSize[RES_IDX_3264x1836_30FPS]    = (sizeof(SNR_IMX175_Reg_3264x1836)/sizeof(SNR_IMX175_Reg_3264x1836[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_3264x1836_30FPS]  = &SNR_IMX175_Reg_3264x1836[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_800x600_60P]      	= (sizeof(SNR_IMX175_Reg_1152x648)/sizeof(SNR_IMX175_Reg_1152x648[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_800x600_60P]    	= &SNR_IMX175_Reg_1152x648[0];

    // Init Vif Setting : Common
    SensorCustFunc.VifSetting->SnrType                              = MMPF_VIF_SNR_TYPE_BAYER;
    SensorCustFunc.VifSetting->OutInterface                         = MMPF_VIF_IF_MIPI_QUAD;
    #if (SUPPORT_DUAL_SNR)
    SensorCustFunc.VifSetting->VifPadId							    = MMPF_VIF_MDL_ID1;
    #else
	SensorCustFunc.VifSetting->VifPadId								= MMPF_VIF_MDL_ID0;
    #endif

    // Init Vif Setting : PowerOn Setting
    SensorCustFunc.VifSetting->powerOnSetting.bTurnOnExtPower       = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPin         = SENSOR_GPIO_ENABLE; // it might be defined in Config_SDK.h
    SensorCustFunc.VifSetting->powerOnSetting.bExtPowerPinHigh	    = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPinDelay    = 50;
    SensorCustFunc.VifSetting->powerOnSetting.bFirstEnPinHigh      	= MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubFirstEnPinDelay    	= 1;
    SensorCustFunc.VifSetting->powerOnSetting.bNextEnPinHigh        = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOnSetting.ubNextEnPinDelay    	= 10;
    SensorCustFunc.VifSetting->powerOnSetting.bTurnOnClockBeforeRst	= MMP_TRUE;
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
    SensorCustFunc.VifSetting->clockAttr.ubClkSrc                   = MMPF_VIF_SNR_CLK_SRC_PMCLK; 

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
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[0]             	= 0;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[1]              = 0;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[2]              = 0;
    SensorCustFunc.VifSetting->mipiAttr.usDataDelay[3]              = 0;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[0]             = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[1]             = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[2]             = 0x1F;
    SensorCustFunc.VifSetting->mipiAttr.ubDataSotCnt[3]             = 0x1F;
#endif

    // Init Vif Setting : Color ID Setting
    SensorCustFunc.VifSetting->colorId.VifColorId              		= MMPF_VIF_COLORID_00;
	SensorCustFunc.VifSetting->colorId.CustomColorId.bUseCustomId  	= MMP_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_DoAE_FrmSt
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_DoAE_FrmSt(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
#if (ISP_EN)
    ISP_UINT32 s_gain;

    switch (ulFrameCnt % 6) {
    case 0:
        ISP_IF_AE_Execute();
        s_gain = ISP_MAX(ISP_IF_AE_GetGain(), ISP_IF_AE_GetGainBase());

        if (s_gain >= ISP_IF_AE_GetGainBase() * MAX_SENSOR_GAIN) {
            s_gain  = ISP_IF_AE_GetGainBase() * MAX_SENSOR_GAIN;
        }	

        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x104, 1);
        gsSensorFunction->MMPF_Sensor_SetShutter(ubSnrSel, 0, 0);
        gsSensorFunction->MMPF_Sensor_SetGain(ubSnrSel, s_gain);
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x104, 0);
        break;
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
    ISP_UINT32 s_gain;

    //Change Again setting for rounding error
    s_gain = VR_MIN(256 - (256 * ISP_IF_AE_GetGainBase() + (ulGain-1)) /
                    VR_MAX(ulGain, ISP_IF_AE_GetGainBase()), 224);
                    
    gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x205, (s_gain >> 0) & 0xff);
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

    // 1 ~ (frame length line - 6)
    new_vsync = VR_MIN(VR_MAX(new_shutter + 6, new_vsync), 0xFFFF);
    new_shutter = VR_MIN(VR_MAX(new_shutter, 1), new_vsync - 6);

    gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x340, new_vsync >> 8);
    gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x341, new_vsync);
    gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x202, (new_shutter >> 8) & 0xff);
    gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x203, (new_shutter >> 0) & 0xff);
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

#endif // (BIND_SENSOR_IMX175)
#endif // (SENSOR_EN)

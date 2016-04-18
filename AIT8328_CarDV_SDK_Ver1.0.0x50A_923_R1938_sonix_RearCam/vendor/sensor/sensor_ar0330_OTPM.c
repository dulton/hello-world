//==============================================================================
//
//  File        : sensor_ar0330.c
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
#include "mmpf_vif.h"
#if (SENSOR_EN)
#if (BIND_SENSOR_AR0330_OTPM)

#include "mmpf_sensor.h"
#include "mmpf_i2cm.h"
#include "Sensor_Mod_Remapping.h"
#include "isp_if.h"

//==============================================================================
//
//                              GLOBAL VARIABLE
//
//==============================================================================

static MMP_UBYTE gOTPM_Ver = 0xFF;

// OPR Table and Vif Setting
MMPF_SENSOR_OPR_TABLE       m_OprTable;
MMPF_SENSOR_VIF_SETTING     m_VifSetting;

// IQ Table
const ISP_UINT8 Sensor_IQ_CompressedText[] = 
{
    #ifdef CUS_ISP_8428_IQ_DATA
    #include CUS_ISP_8428_IQ_DATA
    #else
    #include "isp_8428_iq_data_v2_AR0330_MP.xls.ciq.txt"
    #endif
};

// I2cm Attribute
static MMP_I2CM_ATTR m_I2cmAttr = 
{
    MMP_I2CM0,  // i2cmID
    #ifdef SENSOR_I2C_ADDR
    SENSOR_I2C_ADDR,
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
    250000,     // ulI2cmSpeed 150KHZ
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

// Casio : when use this, open code in DoAEOperation_ST
#ifdef CUS_AE_BIAS_TBL		// maybe defined in project's MCP or Config_SDK.h

#include CUS_AE_BIAS_TBL

#else

// New extent node for18//LV1,         LV2,        LV3,
//LV4,        LV5,        LV6,        LV7,        LV8,        LV9,
//LV10, LV11, LV12, LV13, LV14, LV15, LV16  LV17        LV18
//abby curve
ISP_UINT32 AE_Bias_tbl[54] =
/*lux*/                                   {1,               2,
3,                6,                12,         24,         50,
101,        206,        407,  826,  1638, 3277, 6675, 13554,      27329,
54961, 111285/*930000=LV17*/
/*ENG*/                                   ,0x2FFFFFF, 4841472*2,  3058720,
1962240,    1095560,    616000,     334880,     181720,     96600,
52685,      27499,      14560,      8060, 4176, 2216, 1144, 600,   300
/*Tar*/                                   ,86,        86,               86,
        110,            128,        137,        161,        170,      190,
    202,    220,  237,  256,  256,  256,  256,  256,   256
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

#include "AR0330_Parallel_ModeTable.c"
#include "AR0330_2Lane_ModeTable.c"
#include "AR0330_4Lane_ModeTable.c"

#if 0
void ____Sensor_Customer_Func____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_InitConfig
//  Description :
//------------------------------------------------------------------------------
static void SNR_Cust_InitConfig(void)
{
    MMP_UBYTE i;
    
#if (SENSOR_IF == SENSOR_IF_PARALLEL)
	RTNA_DBG_Str(0, "SNR_Cust_InitConfig AR0330 Parallel\r\n");
#elif (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)
	RTNA_DBG_Str(0, "SNR_Cust_InitConfig AR0330 MIPI 2-lane\r\n");
#elif (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)
	RTNA_DBG_Str(0, "SNR_Cust_InitConfig AR0330 MIPI 4-lane\r\n");
#endif

    SensorCustFunc.OprTable->bBinTableExist                         = MMP_FALSE;
    SensorCustFunc.OprTable->bInitDoneTableExist                    = MMP_FALSE;

    // Init OPR Table
#if (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)

    SensorCustFunc.OprTable->usInitSize                             = (sizeof(SNR_AR0330_MIPI_2_LANE_Reg_Init_Customer)/sizeof(SNR_AR0330_MIPI_2_LANE_Reg_Init_Customer[0]))/2;
    SensorCustFunc.OprTable->uspInitTable                           = &SNR_AR0330_MIPI_2_LANE_Reg_Init_Customer[0];

    SensorCustFunc.OprTable->usSize[RES_IDX_2304x1296_29D97FPS]     = (sizeof(SNR_AR0330_MIPI_2_LANE_Reg_2304x1296)/sizeof(SNR_AR0330_MIPI_2_LANE_Reg_2304x1296[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_2304x1296_29D97FPS]   = &SNR_AR0330_MIPI_2_LANE_Reg_2304x1296[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_2304x1296_25FPS]     = (sizeof(SNR_AR0330_MIPI_2_LANE_Reg_2304x1296_25P)/sizeof(SNR_AR0330_MIPI_2_LANE_Reg_2304x1296_25P[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_2304x1296_25FPS]   = &SNR_AR0330_MIPI_2_LANE_Reg_2304x1296_25P[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_2048x1536_30FPS]        = (sizeof(SNR_AR0330_MIPI_2_LANE_Reg_2304x1536)/sizeof(SNR_AR0330_MIPI_2_LANE_Reg_2304x1536[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_2048x1536_30FPS]      = &SNR_AR0330_MIPI_2_LANE_Reg_2304x1536[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_800x600]                = (sizeof(SNR_AR0330_MIPI_2_LANE_Reg_1152x648)/sizeof(SNR_AR0330_MIPI_2_LANE_Reg_1152x648[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_800x600]              = &SNR_AR0330_MIPI_2_LANE_Reg_1152x648[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_1280x720_60FPS]         = (sizeof(SNR_AR0330_Reg_1280x720_60fps_Customer)/sizeof(SNR_AR0330_Reg_1280x720_60fps_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720_60FPS]       = &SNR_AR0330_Reg_1280x720_60fps_Customer[0];

#elif (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)

    SensorCustFunc.OprTable->usInitSize 				            = (sizeof(SNR_AR0330_Reg_Init_OTPM_V5_Customer)/sizeof(SNR_AR0330_Reg_Init_OTPM_V5_Customer[0]))/2;
    SensorCustFunc.OprTable->uspInitTable				            = &SNR_AR0330_Reg_Init_OTPM_V5_Customer[0];

    // For Video Wide
    SensorCustFunc.OprTable->usSize[RES_IDX_1920x1080_30FPS_W]		= (sizeof(SNR_AR0330_Reg_2266x1280_30fps)/sizeof(SNR_AR0330_Reg_2266x1280_30fps[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1920x1080_30FPS_W]	= &SNR_AR0330_Reg_2266x1280_30fps[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_1280x720_60FPS_W]	  	= (sizeof(SNR_AR0330_Reg_2266x1280_60fps)/sizeof(SNR_AR0330_Reg_2266x1280_60fps[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720_60FPS_W]		= &SNR_AR0330_Reg_2266x1280_60fps[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_1280x720_30FPS_W]	  	= (sizeof(SNR_AR0330_Reg_2266x1280_30fps)/sizeof(SNR_AR0330_Reg_2266x1280_30fps[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720_30FPS_W]		= &SNR_AR0330_Reg_2266x1280_30fps[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_848x480_30FPS_W]	  	= (sizeof(SNR_AR0330_Reg_2266x1280_30fps)/sizeof(SNR_AR0330_Reg_2266x1280_30fps[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_848x480_30FPS_W]		= &SNR_AR0330_Reg_2266x1280_30fps[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_848x480_120FPS_W]	  	= (sizeof(SNR_AR0330_Reg_1132x640_120fps)/sizeof(SNR_AR0330_Reg_1132x640_120fps[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_848x480_120FPS_W]		= &SNR_AR0330_Reg_1132x640_120fps[0];

    // For Video Standard
    SensorCustFunc.OprTable->usSize[RES_IDX_1920x1080_30FPS_S]		= (sizeof(SNR_AR0330_Reg_1928x1096_30fps)/sizeof(SNR_AR0330_Reg_1928x1096_30fps[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1920x1080_30FPS_S]	= &SNR_AR0330_Reg_1928x1096_30fps[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_1920x1080_60FPS_S]	  	= (sizeof(SNR_AR0330_Reg_1928x1096_60fps)/sizeof(SNR_AR0330_Reg_1928x1096_60fps[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1920x1080_60FPS_S]	= &SNR_AR0330_Reg_1928x1096_60fps[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_1280x720_60FPS_S]	  	= (sizeof(SNR_AR0330_Reg_1928x1096_60fps)/sizeof(SNR_AR0330_Reg_1928x1096_60fps[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720_60FPS_S]		= &SNR_AR0330_Reg_1928x1096_60fps[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_1280x720_30FPS_S]	  	= (sizeof(SNR_AR0330_Reg_1928x1096_30fps)/sizeof(SNR_AR0330_Reg_1928x1096_30fps[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720_30FPS_S]		= &SNR_AR0330_Reg_1928x1096_30fps[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_848x480_30FPS_S]	  	= (sizeof(SNR_AR0330_Reg_1928x1096_30fps)/sizeof(SNR_AR0330_Reg_1928x1096_30fps[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_848x480_30FPS_S]		= &SNR_AR0330_Reg_1928x1096_30fps[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_848x480_120FPS_S]	  	= (sizeof(SNR_AR0330_Reg_964x544_120fps)/sizeof(SNR_AR0330_Reg_964x544_120fps[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_848x480_120FPS_S]		= &SNR_AR0330_Reg_964x544_120fps[0];

    SensorCustFunc.OprTable->usSize[RES_IDX_2304x1296_29D97FPS_S]	= (sizeof(SNR_AR0330_Reg_2304x1296_29_97fps)/sizeof(SNR_AR0330_Reg_2304x1296_29_97fps[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_2304x1296_29D97FPS_S] = &SNR_AR0330_Reg_2304x1296_29_97fps[0];

    // For Photo
    SensorCustFunc.OprTable->usSize[RES_IDX_2184x1232_30FPS_W]	  	= (sizeof(SNR_AR0330_Reg_2184x1232_30fps)/sizeof(SNR_AR0330_Reg_2184x1232_30fps[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_2184x1232_30FPS_W]	= &SNR_AR0330_Reg_2184x1232_30fps[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_2184x1232_15FPS_W]	  	= (sizeof(SNR_AR0330_Reg_2184x1232_15fps)/sizeof(SNR_AR0330_Reg_2184x1232_15fps[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_2184x1232_15FPS_W]	= &SNR_AR0330_Reg_2184x1232_15fps[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_1928x1088_30FPS_S]	  	= (sizeof(SNR_AR0330_Reg_1928x1088_30fps)/sizeof(SNR_AR0330_Reg_1928x1088_30fps[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1928x1088_30FPS_S]	= &SNR_AR0330_Reg_1928x1088_30fps[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_1928x1088_15FPS_S]	  	= (sizeof(SNR_AR0330_Reg_1928x1088_15fps)/sizeof(SNR_AR0330_Reg_1928x1088_15fps[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1928x1088_15FPS_S]	= &SNR_AR0330_Reg_1928x1088_15fps[0];

    SensorCustFunc.OprTable->usSize[RES_IDX_2048x1536_30FPS]	  	= (sizeof(SNR_AR0330_Reg_2048x1536_30fps)/sizeof(SNR_AR0330_Reg_2048x1536_30fps[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_2048x1536_30FPS]		= &SNR_AR0330_Reg_2048x1536_30fps[0];

#elif (SENSOR_IF == SENSOR_IF_PARALLEL)

	SensorCustFunc.OprTable->usInitSize 							= (sizeof(SNR_AR0330_PARALLEL_Reg_Init_Customer)/sizeof(SNR_AR0330_PARALLEL_Reg_Init_Customer[0]))/2;
	SensorCustFunc.OprTable->uspInitTable 							= &SNR_AR0330_PARALLEL_Reg_Init_Customer[0];

    SensorCustFunc.OprTable->usSize[RES_IDX_2304x1296_30FPS]	  	= (sizeof(SNR_AR0330CS_Reg_2304x1296_Customer)/sizeof(SNR_AR0330CS_Reg_2304x1296_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_2304x1296_30FPS]      = &SNR_AR0330CS_Reg_2304x1296_Customer[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_2048x1536_25FPS]	  	= (sizeof(SNR_AR0330CS_Reg_2048x1536_25fps_Customer)/sizeof(SNR_AR0330CS_Reg_2048x1536_25fps_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_2048x1536_25FPS]      = &SNR_AR0330CS_Reg_2048x1536_25fps_Customer[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_1920x1080_30FPS]	  	= (sizeof(SNR_AR0330CS_Reg_1920x1080_Customer)/sizeof(SNR_AR0330CS_Reg_1920x1080_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1920x1080_30FPS]      = &SNR_AR0330CS_Reg_1920x1080_Customer[0];
    SensorCustFunc.OprTable->usSize[RES_IDX_1280x720_60FPS]	  	    = (sizeof(SNR_AR0330CS_Reg_1280x720_60fps_Customer)/sizeof(SNR_AR0330CS_Reg_1280x720_60fps_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720_60FPS]       = &SNR_AR0330CS_Reg_1280x720_60fps_Customer[0];

#else

#error "SENSOR_AR0330_IF_ERROR"

#endif

    // Init Vif Setting : Common
    SensorCustFunc.VifSetting->SnrType                              = MMPF_VIF_SNR_TYPE_BAYER;
#if (SENSOR_IF == SENSOR_IF_PARALLEL)
	SensorCustFunc.VifSetting->OutInterface                         = MMPF_VIF_IF_PARALLEL;
#elif (SENSOR_IF == SENSOR_IF_MIPI_1_LANE)
	SensorCustFunc.VifSetting->OutInterface 					    = MMPF_VIF_IF_MIPI_SINGLE_0;
#elif (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)
#if (PRM_SENSOR_VIF_ID == MMPF_VIF_MDL_ID1) //MIPI 2 lane
	SensorCustFunc.VifSetting->OutInterface 					    = MMPF_VIF_IF_MIPI_DUAL_12;
#else
	SensorCustFunc.VifSetting->OutInterface 					    = MMPF_VIF_IF_MIPI_DUAL_01;
#endif
#elif (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)
	SensorCustFunc.VifSetting->OutInterface 					    = MMPF_VIF_IF_MIPI_QUAD;
#endif

#if (PRM_SENSOR_VIF_ID == MMPF_VIF_MDL_ID1)
    SensorCustFunc.VifSetting->VifPadId							    = MMPF_VIF_MDL_ID1;
#else
    SensorCustFunc.VifSetting->VifPadId							    = MMPF_VIF_MDL_ID0;
#endif
    
    // Init Vif Setting : PowerOn Setting
 	/********************************************/
	// Power On serquence
	// 1. Supply Power
	// 2. Deactive RESET
	// 3. Enable MCLK
	// 4. Active RESET (1ms)
	// 5. Deactive RESET (Wait 150000 clock of MCLK, about 8.333ms under 24MHz)
	/********************************************/

    SensorCustFunc.VifSetting->powerOnSetting.bTurnOnExtPower       = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPin         = SENSOR_GPIO_ENABLE;	
	SensorCustFunc.VifSetting->powerOnSetting.bExtPowerPinHigh	    = SENSOR_GPIO_ENABLE_ACT_LEVEL; 	
    SensorCustFunc.VifSetting->powerOnSetting.bFirstEnPinHigh       = (SENSOR_GPIO_ENABLE_ACT_LEVEL == GPIO_HIGH) ? MMP_TRUE : MMP_FALSE;
    SensorCustFunc.VifSetting->powerOnSetting.ubFirstEnPinDelay     = 5;
    SensorCustFunc.VifSetting->powerOnSetting.bNextEnPinHigh        = (SENSOR_GPIO_ENABLE_ACT_LEVEL == GPIO_HIGH) ? MMP_FALSE : MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubNextEnPinDelay      = 5;
    SensorCustFunc.VifSetting->powerOnSetting.bTurnOnClockBeforeRst = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.bFirstRstPinHigh      = (SENSOR_RESET_ACT_LEVEL == GPIO_HIGH) ? MMP_TRUE : MMP_FALSE;
    SensorCustFunc.VifSetting->powerOnSetting.ubFirstRstPinDelay    = 10;
    SensorCustFunc.VifSetting->powerOnSetting.bNextRstPinHigh       = (SENSOR_RESET_ACT_LEVEL == GPIO_HIGH) ? MMP_FALSE : MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubNextRstPinDelay     = 10;
    
    // Init Vif Setting : PowerOff Setting
    SensorCustFunc.VifSetting->powerOffSetting.bEnterStandByMode    = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOffSetting.usStandByModeReg     = 0x301A;
    SensorCustFunc.VifSetting->powerOffSetting.usStandByModeMask    = 0x04;
    SensorCustFunc.VifSetting->powerOffSetting.bEnPinHigh           = GPIO_HIGH;
    SensorCustFunc.VifSetting->powerOffSetting.ubEnPinDelay         = 20;
    SensorCustFunc.VifSetting->powerOffSetting.bTurnOffMClock       = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOffSetting.bTurnOffExtPower     = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOffSetting.usExtPowerPin        = MMP_GPIO_MAX;
    
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
    SensorCustFunc.VifSetting->paralAttr.ubVsyncPolarity            = MMPF_VIF_SNR_CLK_POLARITY_POS;
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
#if (PRM_SENSOR_VIF_ID == MMPF_VIF_MDL_ID1) //MIPI 2 lane
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[0]            = MMPF_VIF_MIPI_DATA_SRC_PHY_1;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[1]            = MMPF_VIF_MIPI_DATA_SRC_PHY_2;
#else
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[0]            = MMPF_VIF_MIPI_DATA_SRC_PHY_0;
    SensorCustFunc.VifSetting->mipiAttr.ubDataLaneSrc[1]            = MMPF_VIF_MIPI_DATA_SRC_PHY_1;
#endif
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
	SensorCustFunc.VifSetting->colorId.VifColorId				    = MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.bUseCustomId   = MMP_TRUE;

	for (i = 0; i < MAX_SENSOR_RES_MODE; i++)
	{
    	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot0d_Id[i]   = MMPF_VIF_COLORID_01;
    	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot90d_Id[i]  = MMPF_VIF_COLORID_UNDEF;
    	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot180d_Id[i] = MMPF_VIF_COLORID_UNDEF;
    	SensorCustFunc.VifSetting->colorId.CustomColorId.Rot270d_Id[i] = MMPF_VIF_COLORID_UNDEF;
    	SensorCustFunc.VifSetting->colorId.CustomColorId.H_Flip_Id[i]  = MMPF_VIF_COLORID_UNDEF;
    	SensorCustFunc.VifSetting->colorId.CustomColorId.V_Flip_Id[i]  = MMPF_VIF_COLORID_UNDEF;
    	SensorCustFunc.VifSetting->colorId.CustomColorId.HV_Flip_Id[i] = MMPF_VIF_COLORID_01;
	}
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_DoAE_FrmSt
//  Description :
//------------------------------------------------------------------------------
ISP_UINT32 isp_dgain;
ISP_UINT32 dGainBase = 0x200;

void SNR_Cust_DoAE_FrmSt(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
    // For AE curve
    if ((ulFrameCnt % 100) == 10)
    {
        ISP_IF_F_SetWDREn(1);
        ISP_IF_CMD_SendCommandtoAE(0x51, AE_Bias_tbl,AE_tbl_size, 0); // <<AE table set once at preview start 
        ISP_IF_NaturalAE_Enable(2); // 0: no, 1: ENERGY, 2: Lux, 3: test mode
    }

	switch (ulFrameCnt % m_AwbTime.ubPeriod) {
	case 0:
		#if (ISP_EN)
		ISP_IF_AE_Execute();
		gsSensorFunction->MMPF_Sensor_SetShutter(ubSnrSel, 0, 0);
	    gsSensorFunction->MMPF_Sensor_SetGain(ubSnrSel, ISP_IF_AE_GetGain());
		#endif
		break;
	case 1:
		ISP_IF_IQ_SetAEGain(isp_dgain, 0x200);
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
	isp_dgain = dGainBase;

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

   		if (sensor_dgain >= 0x100) { // >=15.99x , sensor_dgain<=1.99x
	        sensor_again = 0x30;    // 8X 
			sensor_dgain = 0xFF;	// 1.99x
			isp_dgain = ulGain * dGainBase * 0x80 / 0x08/ 0xFF/0x40; //ulGain base=0x40, again base = 0x40, ispgain base=0x200
		}
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
    MMP_USHORT  usRdVal;
    
	if (ubMode == MMPF_SENSOR_NO_FLIP) 
	{
	    gsSensorFunction->MMPF_Sensor_GetReg(ubSnrSel, 0x3040, &usRdVal);
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3040, usRdVal & 0x3FFF);
        SensorCustFunc.VifSetting->colorId.VifColorId = MMPF_VIF_COLORID_01;
	}	
	else if (ubMode == MMPF_SENSOR_COLROW_FLIP) 
	{
	    gsSensorFunction->MMPF_Sensor_GetReg(ubSnrSel, 0x3040, &usRdVal);
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3040, usRdVal | 0xC000);
        SensorCustFunc.VifSetting->colorId.VifColorId = MMPF_VIF_COLORID_10;
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
	MMP_USHORT usRdVal300E,usRdVal30F0,usRdVal3072 = 0;
    
    /* Initial I2cm */
    MMPF_I2cm_Initialize(&m_I2cmAttr);
    MMPF_OS_SleepMs(10);

	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x301A, 0x0001);
	MMPF_OS_SleepMs(20);
    
    gsSensorFunction->MMPF_Sensor_GetReg(ubSnrSel, 0x300E, &usRdVal300E);
	gsSensorFunction->MMPF_Sensor_GetReg(ubSnrSel, 0x30F0, &usRdVal30F0); 
	gsSensorFunction->MMPF_Sensor_GetReg(ubSnrSel, 0x3072, &usRdVal3072); 

    if (usRdVal3072 != 0x0) {
		if (usRdVal3072 == 0x0008)
			gOTPM_Ver = 5;
		else if (usRdVal3072 == 0x0007)
			gOTPM_Ver = 4;
		else
			gOTPM_Ver = 3;
    }
	else {
		if (usRdVal300E == 0x10)
			gOTPM_Ver = 1;
		else
			gOTPM_Ver = 2;
	}
	
	printc(FG_BLUE("@@@AR0330-Ver%d")"\r\n", gOTPM_Ver);

#if (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)

    SensorCustFunc.OprTable->usInitSize   = (sizeof(SNR_AR0330_MIPI_2_LANE_Reg_Init_Customer)/sizeof(SNR_AR0330_MIPI_2_LANE_Reg_Init_Customer[0]))/2;
    SensorCustFunc.OprTable->uspInitTable = &SNR_AR0330_MIPI_2_LANE_Reg_Init_Customer[0];

#elif (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)

	switch(gOTPM_Ver) {
	case 1:
    	SensorCustFunc.OprTable->usInitSize   = (sizeof(SNR_AR0330_Reg_Init_OTPM_V1_Customer)/sizeof(SNR_AR0330_Reg_Init_OTPM_V1_Customer[0]))/2;
    	SensorCustFunc.OprTable->uspInitTable = &SNR_AR0330_Reg_Init_OTPM_V1_Customer[0];
    	break;
	case 2:
    	SensorCustFunc.OprTable->usInitSize   = (sizeof(SNR_AR0330_Reg_Init_OTPM_V2_Customer)/sizeof(SNR_AR0330_Reg_Init_OTPM_V2_Customer[0]))/2;
    	SensorCustFunc.OprTable->uspInitTable = &SNR_AR0330_Reg_Init_OTPM_V2_Customer[0];
    	break;
    case 3:
    	SensorCustFunc.OprTable->usInitSize   = (sizeof(SNR_AR0330_Reg_Init_OTPM_V3_Customer)/sizeof(SNR_AR0330_Reg_Init_OTPM_V3_Customer[0]))/2;
    	SensorCustFunc.OprTable->uspInitTable = &SNR_AR0330_Reg_Init_OTPM_V3_Customer[0];
    	break;
	case 4:
    	SensorCustFunc.OprTable->usInitSize   = (sizeof(SNR_AR0330_Reg_Init_OTPM_V4_Customer)/sizeof(SNR_AR0330_Reg_Init_OTPM_V4_Customer[0]))/2;
    	SensorCustFunc.OprTable->uspInitTable = &SNR_AR0330_Reg_Init_OTPM_V4_Customer[0];
    	break;
	case 5:
    	SensorCustFunc.OprTable->usInitSize   = (sizeof(SNR_AR0330_Reg_Init_OTPM_V5_Customer)/sizeof(SNR_AR0330_Reg_Init_OTPM_V5_Customer[0]))/2;
    	SensorCustFunc.OprTable->uspInitTable = &SNR_AR0330_Reg_Init_OTPM_V5_Customer[0];
    	break;
	}
#else

	SensorCustFunc.OprTable->usInitSize         = (sizeof(SNR_AR0330_PARALLEL_Reg_Init_Customer)/sizeof(SNR_AR0330_PARALLEL_Reg_Init_Customer[0]))/2;
	SensorCustFunc.OprTable->uspInitTable       = &SNR_AR0330_PARALLEL_Reg_Init_Customer[0];

#endif
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

#pragma arm section code = "initcall6", rodata = "initcall6", rwdata = "initcall6", zidata = "initcall6"
#pragma O0
ait_module_init(SNR_Module_Init);
#pragma
#pragma arm section rodata, rwdata, zidata

#endif // (BIND_SENSOR_AR0330_OTPM)
#endif // (SENSOR_EN)

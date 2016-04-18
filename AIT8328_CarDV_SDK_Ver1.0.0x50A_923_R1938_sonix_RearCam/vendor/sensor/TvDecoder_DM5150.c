//==============================================================================
//
//  File        : sensor_DM5150.c
//  Description : Firmware Sensor Control File
//  Author      :
//  Revision    : 1.0
//
//=============================================================================

#include "includes_fw.h"
#include "customer_config.h"

#if (SENSOR_EN)
#if (BIND_SENSOR_DM5150)

#include "mmpf_sensor.h"
#include "Sensor_Mod_Remapping.h"
#include "isp_if.h"

//==============================================================================
//
//                              GLOBAL VARIABLE
//
//==============================================================================

//Need to check Mode 0 (PAL) is correct or not.
MMPF_SENSOR_RESOLUTION m_DM5150_SensorRes =
{
	2,				// ubSensorModeNum
	0,				// ubDefPreviewMode
	0,				// ubDefCaptureMode
	3000,           // usPixelSize (TBD)
//  Mode0   Mode1
    {3,     3},	    // usVifGrabStX
    {14,    14},    // usVifGrabStY
    {720,   720},	// usVifGrabW
    {242,   242},	// usVifGrabH
    #if (CHIP == MCR_V2)
	{1,   	1}, 	// usBayerInGrabX
	{1,   	1}, 	// usBayerInGrabY
    {2,     2},     // usBayerInDummyX
    {2,     2},     // usBayerInDummyY
    {718,   718},	// usBayerOutW
    {240,   240},	// usBayerOutH
    #endif
    {718,   718},	// usScalInputW
    {240,   240},	// usScalInputH
    {300,   300},	// usTargetFpsx10 	// Real Frame Rate 29.914
    {250,   250},	// usVsyncLine
	{1,   	1}, 	// ubWBinningN
	{1,   	1}, 	// ubWBinningN
	{1,   	1}, 	// ubWBinningN
	{1,   	1}, 	// ubWBinningN
    {0xFF,  0xFF},  // ubCustIQmode
    {0xFF,	0xFF}   // ubCustAEmode
};

// OPR Table and Vif Setting
MMPF_SENSOR_OPR_TABLE       m_DM5150_OprTable;
MMPF_SENSOR_VIF_SETTING     m_DM5150_VifSetting;

// IQ Table
#if (SENSOR_PROI == PRM_SENSOR)
const ISP_UINT8 Sensor_IQ_CompressedText[] = 
{
#ifdef CUS_ISP_8428_IQ_DATA     // maybe defined in project MCP or Config_SDK.h
#include CUS_ISP_8428_IQ_DATA
#else
#include "isp_8428_iq_data_v2_AR0331.xls.ciq.txt"
#endif
};
#endif

// I2cm Attribute
static MMP_I2CM_ATTR m_DM5150_I2cmAttr = 
{
    MMP_I2CM0,  // i2cmID
    0x5C,       // ubSlaveAddr
    8,          // ubRegLen
    8,          // ubDataLen
    0,          // ubDelayTime
    MMP_FALSE,  // bDelayWaitEn
    MMP_FALSE,  // bInputFilterEn
    MMP_FALSE,  // b10BitModeEn
    MMP_FALSE,  // bClkStretchEn
    0,          // ubSlaveAddr1
    0,          // ubDelayCycle
    0,          // ubPadNum
    400000,     // ulI2cmSpeed 400KHZ
    MMP_FALSE,  // bOsProtectEn
    NULL,       // sw_clk_pin
    NULL,       // sw_data_pin
    MMP_FALSE,  // bRfclModeEn
    MMP_FALSE,  // bWfclModeEn
    MMP_FALSE,	// bRepeatModeEn
    0           // ubVifPioMdlId
};

// 3A Timing
MMPF_SENSOR_AWBTIMIMG   m_DM5150_AwbTime    =
{
	6,	/* ubPeriod */
	1, 	/* ubDoAWBFrmCnt */
	3	/* ubDoCaliFrmCnt */
};

MMPF_SENSOR_AETIMIMG    m_DM5150_AeTime     =
{
	6, 	/* ubPeriod */
	0, 	/* ubFrmStSetShutFrmCnt */
	0	/* ubFrmStSetGainFrmCnt */
};

MMPF_SENSOR_AFTIMIMG    m_DM5150_AfTime     =
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

ISP_UINT16 SNR_DM5150_Reg_Init_Customer[] = 
{
	0x51, 0x01,
	0x32, 0x00,
	0x3b, 0x00,
	0x3c, 0x05,
	0x08, 0x30,
	0x06, 0x0b, // Default 0x0b blue pannel  0x0a black pannel
    0x29, 0x01, // No Signal, NTSC Blue panel out.
	0x14, 0x16, // Always tracking clkoffset
	0x17, 0x0a, //
	0x05, 0x20, //
	0x18, 0x23, // Reg 0x18 bit 0 auto update HSYNCTH
	0x39, 0xd2, // Increase color AGC threshold
	0x3d, 0x40, // CCIR buffer reset
	0x3a, 0x90, // CAGC enable
	0x1A, 0x15, //
	0x10, 0x00, // Auto
    0x2A, 0x3A, // Enable FID
    0x4A, 0xC7, // Set BT 601
	0x00, 0x80, // Close V/H sync
};

#if 0
void ____Sensor_Res_OPR_Table____(){ruturn;} //dummy
#endif

ISP_UINT16 SNR_DM5150_Reg_720x240_30P[] = 
{
    SENSOR_DELAY_REG, 100 // delay
};

#if 0
void ____Sensor_Customer_Func____(){ruturn;} // dummy
#endif

#if 0 //EROY TBD
/*
 * Wait ready is called before VIF enabled
 */
int MMPF_Sensor_DM5150WaitReady(void)
{
#if 0
    // We want to VIF always get a particular field (EVEN or ODD), there are two method.
    // Method 1.
	// If FID connected to GPIO, to wait FID from 1 -> 0 to start VI, it is better solution
	unsigned char	fid;
	unsigned int	tmi = 0;
	
	do {
		tmi++;
		MMPF_PIO_GetData(MMPS_PIO_REG_GPIO33, &fid);
	} while (fid == 0);
	do {
		tmi++;
		MMPF_PIO_GetData(MMPS_PIO_REG_GPIO33, &fid);
	} while (fid == 1);
	return (int)tmi;
#else
	// Method 2.
	// Without the FID, to Enable Video out is closer VI enable,
	// to make sure the time between Video out enabled and VIF enabled is fixed
	// Open V/H sync
	MMPF_I2cm_WriteReg(&m_I2cmAttribute, 0x00, 0x80);
	MMPF_OS_Sleep(200);
	printc("***** RESET DM5150 *****\r\n");
	MMPF_I2cm_WriteReg(&m_I2cmAttribute, 0x00, 0x8A);
	return 0;
#endif
}
#endif

//------------------------------------------------------------------------------
//  Function    : SNR_DM5150_Cust_InitConfig
//  Description :
//------------------------------------------------------------------------------
static void SNR_DM5150_Cust_InitConfig(void)
{
    MMPF_SENSOR_CUSTOMER *pCust = NULL;

#if (SENSOR_PROI == PRM_SENSOR)
    pCust = &SensorCustFunc;
#else
    pCust = &SubSensorCustFunc;
#endif

    RTNA_DBG_Str(0, "SNR_Cust_InitConfig DM5150\r\n");

    // Init OPR Table
    pCust->OprTable->usInitSize 						    = (sizeof(SNR_DM5150_Reg_Init_Customer)/sizeof(SNR_DM5150_Reg_Init_Customer[0]))/2;
    pCust->OprTable->uspInitTable 					        = &SNR_DM5150_Reg_Init_Customer[0];

    pCust->OprTable->bBinTableExist                         = MMP_FALSE;
    pCust->OprTable->bInitDoneTableExist                    = MMP_FALSE;

    //Share with NTSC format due to no initial setting.
    pCust->OprTable->usSize[RES_IDX_PAL_25FPS]		    = (sizeof(SNR_DM5150_Reg_720x240_30P)/sizeof(SNR_DM5150_Reg_720x240_30P[0]))/2;
    pCust->OprTable->uspTable[RES_IDX_PAL_25FPS] 	        = &SNR_DM5150_Reg_720x240_30P[0];

    pCust->OprTable->usSize[RES_IDX_NTSC_30FPS]		    = (sizeof(SNR_DM5150_Reg_720x240_30P)/sizeof(SNR_DM5150_Reg_720x240_30P[0]))/2;
    pCust->OprTable->uspTable[RES_IDX_NTSC_30FPS] 	        = &SNR_DM5150_Reg_720x240_30P[0];

    // Init Vif Setting : Common
    pCust->VifSetting->SnrType                              = MMPF_VIF_SNR_TYPE_YUV_TV_DEC;
#if (SENSOR_IF == SENSOR_IF_PARALLEL)
    pCust->VifSetting->OutInterface 					    = MMPF_VIF_IF_PARALLEL;
#elif (SENSOR_IF == SENSOR_IF_MIPI_1_LANE)
    pCust->VifSetting->OutInterface 				        = MMPF_VIF_IF_MIPI_SINGLE_0;
#elif (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)
    pCust->VifSetting->OutInterface 					    = MMPF_VIF_IF_MIPI_DUAL_01;
#elif (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)
    pCust->VifSetting->OutInterface				            = MMPF_VIF_IF_MIPI_QUAD;
#endif
#if (SENSOR_PROI == PRM_SENSOR)
    pCust->VifSetting->VifPadId 						    = MMPF_VIF_MDL_ID0;
#else
    pCust->VifSetting->VifPadId 						    = MMPF_VIF_MDL_ID0;
#endif

    // Init Vif Setting : PowerOn Setting
    pCust->VifSetting->powerOnSetting.bTurnOnExtPower       = MMP_TRUE;
    pCust->VifSetting->powerOnSetting.usExtPowerPin 	    = SENSOR_GPIO_ENABLE; // it might be defined in Config_SDK.h
    pCust->VifSetting->powerOnSetting.bExtPowerPinHigh      = SENSOR_GPIO_ENABLE_ACT_LEVEL;
    pCust->VifSetting->powerOnSetting.usExtPowerPinDelay    = 50;
    pCust->VifSetting->powerOnSetting.bFirstEnPinHigh       = MMP_FALSE;
    pCust->VifSetting->powerOnSetting.ubFirstEnPinDelay     = 10;
    pCust->VifSetting->powerOnSetting.bNextEnPinHigh        = MMP_TRUE;
    pCust->VifSetting->powerOnSetting.ubNextEnPinDelay      = 100;
    pCust->VifSetting->powerOnSetting.bTurnOnClockBeforeRst = MMP_TRUE;
    pCust->VifSetting->powerOnSetting.bFirstRstPinHigh      = MMP_FALSE;
    pCust->VifSetting->powerOnSetting.ubFirstRstPinDelay    = 100;
    pCust->VifSetting->powerOnSetting.bNextRstPinHigh       = MMP_TRUE;
    pCust->VifSetting->powerOnSetting.ubNextRstPinDelay     = 100;

    // Init Vif Setting : PowerOff Setting
    pCust->VifSetting->powerOffSetting.bEnterStandByMode    = MMP_FALSE;
    pCust->VifSetting->powerOffSetting.usStandByModeReg     = 0x0100;
    pCust->VifSetting->powerOffSetting.usStandByModeMask    = 0x01;
    pCust->VifSetting->powerOffSetting.bEnPinHigh 	        = MMP_TRUE;
    pCust->VifSetting->powerOffSetting.ubEnPinDelay 	    = 20;
    pCust->VifSetting->powerOffSetting.bTurnOffMClock       = MMP_TRUE;
    pCust->VifSetting->powerOffSetting.bTurnOffExtPower     = MMP_FALSE;
    pCust->VifSetting->powerOffSetting.usExtPowerPin        = SENSOR_GPIO_ENABLE; // it might be defined in Config_SDK.h

    // Init Vif Setting : Sensor MClock Setting
    pCust->VifSetting->clockAttr.bClkOutEn 			        = MMP_TRUE;
    pCust->VifSetting->clockAttr.ubClkFreqDiv 		        = 0;
    pCust->VifSetting->clockAttr.ulMClkFreq 			    = 24000;
    pCust->VifSetting->clockAttr.ulDesiredFreq 		        = 24000;
    pCust->VifSetting->clockAttr.ubClkPhase 		        = MMPF_VIF_SNR_PHASE_DELAY_NONE;
    pCust->VifSetting->clockAttr.ubClkPolarity 		        = MMPF_VIF_SNR_CLK_POLARITY_NEG;
    pCust->VifSetting->clockAttr.ubClkSrc 			        = MMPF_VIF_SNR_CLK_SRC_VIFCLK;

    // Init Vif Setting : Parallel Sensor Setting
    pCust->VifSetting->paralAttr.ubLatchTiming 		        = MMPF_VIF_SNR_LATCH_POS_EDGE;
    pCust->VifSetting->paralAttr.ubHsyncPolarity 	        = MMPF_VIF_SNR_CLK_POLARITY_POS;
    pCust->VifSetting->paralAttr.ubVsyncPolarity 	        = MMPF_VIF_SNR_CLK_POLARITY_POS;
    pCust->VifSetting->paralAttr.ubBusBitMode               = MMPF_VIF_SNR_PARAL_BITMODE_16;

    // Init Vif Setting : Color ID Setting
    pCust->VifSetting->colorId.VifColorId 				    = MMPF_VIF_COLORID_00;
    pCust->VifSetting->colorId.CustomColorId.bUseCustomId 	= MMP_FALSE;

    // Init Vif Setting : YUV Setting
#if (SCD_SNR_STORE_YUV420 == 0)
    pCust->VifSetting->yuvAttr.bYuv422ToYuv420              = MMP_FALSE;
    pCust->VifSetting->yuvAttr.bYuv422ToYuv422              = MMP_TRUE;
#else
    pCust->VifSetting->yuvAttr.bYuv422ToYuv420              = MMP_TRUE;
    pCust->VifSetting->yuvAttr.bYuv422ToYuv422              = MMP_FALSE;
#endif
    pCust->VifSetting->yuvAttr.bYuv422ToBayer               = MMP_FALSE;
    pCust->VifSetting->yuvAttr.ubYuv422Order                = MMPF_VIF_YUV422_VYUY;
}

//------------------------------------------------------------------------------
//  Function    : SNR_DM5150_Cust_DoAE_FrmSt
//  Description :
//------------------------------------------------------------------------------
void SNR_DM5150_Cust_DoAE_FrmSt(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
    // TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_DM5150_Cust_DoAE_FrmEnd
//  Description :
//------------------------------------------------------------------------------
void SNR_DM5150_Cust_DoAE_FrmEnd(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_DM5150_Cust_DoAWB
//  Description :
//------------------------------------------------------------------------------
void SNR_DM5150_Cust_DoAWB(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_DM5150_Cust_DoIQ
//  Description :
//------------------------------------------------------------------------------
void SNR_DM5150_Cust_DoIQ(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_DM5150_Cust_SetGain
//  Description :
//------------------------------------------------------------------------------
void SNR_DM5150_Cust_SetGain(MMP_UBYTE ubSnrSel, MMP_ULONG ulGain)
{
    // TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_DM5150_Cust_SetShutter
//  Description :
//------------------------------------------------------------------------------
void SNR_DM5150_Cust_SetShutter(MMP_UBYTE ubSnrSel, MMP_ULONG shutter, MMP_ULONG vsync)
{
    // TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_DM5150_Cust_SetFlip
//  Description :
//------------------------------------------------------------------------------
void SNR_DM5150_Cust_SetFlip(MMP_UBYTE ubSnrSel, MMP_UBYTE ubMode)
{
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_DM5150_Cust_SetRotate
//  Description :
//------------------------------------------------------------------------------
void SNR_DM5150_Cust_SetRotate(MMP_UBYTE ubSnrSel, MMP_UBYTE ubMode)
{
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_DM5150_Cust_CheckVersion
//  Description :
//------------------------------------------------------------------------------
void SNR_DM5150_Cust_CheckVersion(MMP_UBYTE ubSnrSel, MMP_ULONG *pulVersion)
{
	// TBD
}

#if (SENSOR_PROI == PRM_SENSOR)
MMPF_SENSOR_CUSTOMER SensorCustFunc = 
{
	SNR_DM5150_Cust_InitConfig,
	SNR_DM5150_Cust_DoAE_FrmSt,
	SNR_DM5150_Cust_DoAE_FrmEnd,
    SNR_DM5150_Cust_DoAWB,
    SNR_DM5150_Cust_DoIQ,
	SNR_DM5150_Cust_SetGain,
	SNR_DM5150_Cust_SetShutter,
	SNR_DM5150_Cust_SetFlip,
	SNR_DM5150_Cust_SetRotate,
	SNR_DM5150_Cust_CheckVersion,

	&m_DM5150_SensorRes,
	&m_DM5150_OprTable,
	&m_DM5150_VifSetting,
	&m_DM5150_I2cmAttr,
	&m_DM5150_AwbTime,
	&m_DM5150_AeTime,
	&m_DM5150_AfTime,
    MMP_SNR_PRIO_PRM
};

int SNR_Module_Init(void)
{
    MMPF_SensorDrv_Register(PRM_SENSOR, &SensorCustFunc);
    
    return 0;
}
#else
MMPF_SENSOR_CUSTOMER SubSensorCustFunc = 
{
	SNR_DM5150_Cust_InitConfig,
	SNR_DM5150_Cust_DoAE_FrmSt,
	SNR_DM5150_Cust_DoAE_FrmEnd,
    SNR_DM5150_Cust_DoAWB,
    SNR_DM5150_Cust_DoIQ,
	SNR_DM5150_Cust_SetGain,
	SNR_DM5150_Cust_SetShutter,
	SNR_DM5150_Cust_SetFlip,
	SNR_DM5150_Cust_SetRotate,
	SNR_DM5150_Cust_CheckVersion,

	&m_DM5150_SensorRes,
	&m_DM5150_OprTable,
	&m_DM5150_VifSetting,
	&m_DM5150_I2cmAttr,
	&m_DM5150_AwbTime,
	&m_DM5150_AeTime,
	&m_DM5150_AfTime,
    MMP_SNR_PRIO_SCD
};

int SubSNR_Module_Init(void)
{
    MMPF_SensorDrv_Register(SCD_SENSOR, &SubSensorCustFunc); 
        
    return 0;
}
#endif

#pragma arm section code = "initcall6", rodata = "initcall6", rwdata = "initcall6", zidata = "initcall6"
#pragma O0
#if (SENSOR_PROI == PRM_SENSOR)
ait_module_init(SNR_Module_Init);
#else
ait_module_init(SubSNR_Module_Init);
#endif
#pragma
#pragma arm section rodata, rwdata, zidata

#endif // (BIND_SENSOR_DM5150)
#endif // (SENSOR_EN)

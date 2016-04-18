//==============================================================================
//
//  File        : TvDecoder_BIT1603.c
//  Description : Firmware Sensor Control File
//  Author      :
//  Revision    : 1.0
//
//=============================================================================

#include "includes_fw.h"
#include "customer_config.h"

#if (defined(DUAL_SENSOR_MBOOT_USE_BIT1603) && DUAL_SENSOR_MBOOT_USE_BIT1603)
#undef BIND_SENSOR_BIT1603
#define BIND_SENSOR_BIT1603	(1)
#undef SENSOR_EN
#define SENSOR_EN	(1)
#endif

#if (SENSOR_EN)
#if (BIND_SENSOR_BIT1603)

#include "mmpf_sensor.h"
#include "Sensor_Mod_Remapping.h"
#include "isp_if.h"

//==============================================================================
//
//                              GLOBAL VARIABLE
//
//==============================================================================

MMPF_SENSOR_RESOLUTION m_BIT1603_SensorRes =
{
	2,				// ubSensorModeNum
	0,				// ubDefPreviewMode
	0,				// ubDefCaptureMode
	3000,           // usPixelSize (TBD)
//  Mode0   Mode1
#if (SCD_SNR_STORE_YUV420)
    {1,     1},     // usVifGrabStX
    {1,     1},     // usVifGrabStY
    {792,   792},	// usVifGrabW
    {242,   242},	// usVifGrabH
    #if (CHIP == MCR_V2)
    {1,   	1}, 	// usBayerInGrabX
    {1,   	1}, 	// usBayerInGrabY
    {2,     2},     // usBayerInDummyX
    {2,     2},     // usBayerInDummyY
    {790,   790},	// usBayerOutW
    {240,   240},	// usBayerOutH
    #endif
    {790,   790},	// usScalInputW
    {240,   240},	// usScalInputH
#else
#if 0 //SVN
    {154,   138},//{130,   130},//{138,   138},   // usVifGrabStX
    {18,    17},    // usVifGrabStY
    {1344,  1376},	// usVifGrabW //Grab smaller area to avoid garbage boundary
    {242,   240}, 	// usVifGrabH
    #if (CHIP == MCR_V2)
	{1,   	1}, 	// usBayerInGrabX
	{1,   	1}, 	// usBayerInGrabY
    {672,   688},   // usBayerInDummyX
    {2,     0},     // usBayerInDummyY
    {720,   720},	// usBayerOutW
    {242,   240},	// usBayerOutH
    #endif
    #if (SCD_SNR_USE_DMA_DEINTERLACE)
    {672,   688},	// usScalInputW // Note: It must be half  of usVifGrabW
    {484,   480},   // usScalInputH // Note: It must be twice of usVifGrabH
    #else
    {720,   720},	// usScalInputW
    {240,   240},	// usScalInputH
    #endif
#else
    {150,   138},//{142,   142},//{138,   138},//{130,   130},//{138,   138},   // usVifGrabStX
    {15,    15},    // usVifGrabStY
    {1440,  1440},	// usVifGrabW
    {242,   242},//{242,   242},	// usVifGrabH
    {1,   	1}, // usBayerInGrabX //No use for raw store/fetch
    {1,   	1}, 	// usBayerInGrabY //No use for raw store/fetch
    {720,   720},   // usBayerInDummyX //No use for raw store/fetch
    {2,     2},     // usBayerInDummyY //No use for raw store/fetch
    {720,   720},	// usBayerOutW //No use for raw store/fetch
    {240,   240},	// usBayerOutH //No use for raw store/fetch
    #if (SCD_SNR_USE_DMA_DEINTERLACE)
    {688, 688},//{720,   720},	// usScalInputW
    {480,   480},	// usScalInputH
    #else
    {720,   720},	// usScalInputW
    {240,   240},	// usScalInputH
    #endif
#endif    
#endif    
    {250, 300},	// usTargetFpsx10
    {250, 250},	// usVsyncLine
    {1,   	1}, 	// ubWBinningN
    {1,   	1}, 	// ubWBinningN
    {1,   	1}, 	// ubWBinningN
    {1,   	1}, 	// ubWBinningN
    {0xFF, 0xFF},  // ubCustIQmode
    {0xFF, 0xFF}   // ubCustAEmode
};

// OPR Table and Vif Setting
MMPF_SENSOR_OPR_TABLE       m_BIT1603_OprTable;
MMPF_SENSOR_VIF_SETTING     m_BIT1603_VifSetting;

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
static MMP_I2CM_ATTR m_BIT1603_I2cmAttr = 
{
    MMP_I2CM0,  // i2cmID
    0x4C >> 1,  // ubSlaveAddr
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
    MMP_TRUE,  // bOsProtectEn
    NULL,       // sw_clk_pin
    NULL,       // sw_data_pin
    MMP_FALSE,  // bRfclModeEn
    MMP_FALSE,  // bWfclModeEn
    MMP_FALSE,	// bRepeatModeEn
    0           // ubVifPioMdlId
};

// 3A Timing
MMPF_SENSOR_AWBTIMIMG   m_BIT1603_AwbTime    =
{
	6,	/* ubPeriod */
	1, 	/* ubDoAWBFrmCnt */
	3	/* ubDoCaliFrmCnt */
};

MMPF_SENSOR_AETIMIMG    m_BIT1603_AeTime     =
{
	6, 	/* ubPeriod */
	0, 	/* ubFrmStSetShutFrmCnt */
	0	/* ubFrmStSetGainFrmCnt */
};

MMPF_SENSOR_AFTIMIMG    m_BIT1603_AfTime     =
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

ISP_UINT16 SNR_BIT1603_Reg_Init_Customer[] = 
{
	#if (TEST_PATTERN_EN)
	0x5F, 0x21,
	0x5D, 0x80,
	#else
	0x5F, 0x00,
	0x10, 0x80,//Brightness
	0x11, 0x80,//contrast
	0x17, 0xc0,//bit7=together,|0x40    //saturation
	0x18, 0x40,							//saturation
	#endif
	0x6D, 0x01,
};

#if 0
void ____Sensor_Res_OPR_Table____(){ruturn;} //dummy
#endif

ISP_UINT16 SNR_BIT1603_Reg_720x240_30P[] = 
{
    SENSOR_DELAY_REG, 0 // delay
};

#if 0
void ____Sensor_Customer_Func____(){ruturn;} // dummy
#endif

//------------------------------------------------------------------------------
//  Function    : SNR_BIT1603_Cust_InitConfig
//  Description :
//------------------------------------------------------------------------------
static void SNR_BIT1603_Cust_InitConfig(void)
{
    MMPF_SENSOR_CUSTOMER *pCust = NULL;

#if (SENSOR_PROI == PRM_SENSOR)
    pCust = &SensorCustFunc;
#else
    pCust = &SubSensorCustFunc;
#endif

    RTNA_DBG_Str(0, "SNR_Cust_InitConfig BIT1603\r\n");

    // Init OPR Table
    pCust->OprTable->usInitSize 						    = (sizeof(SNR_BIT1603_Reg_Init_Customer)/sizeof(SNR_BIT1603_Reg_Init_Customer[0]))/2;
    pCust->OprTable->uspInitTable 					        = &SNR_BIT1603_Reg_Init_Customer[0];

    pCust->OprTable->bBinTableExist                         = MMP_FALSE;
    pCust->OprTable->bInitDoneTableExist                    = MMP_FALSE;

    //Share with NTSC format due to no initial setting.
    pCust->OprTable->usSize[RES_IDX_PAL_25FPS]		    = (sizeof(SNR_BIT1603_Reg_720x240_30P)/sizeof(SNR_BIT1603_Reg_720x240_30P[0]))/2;
    pCust->OprTable->uspTable[RES_IDX_PAL_25FPS] 	        = &SNR_BIT1603_Reg_720x240_30P[0];

    pCust->OprTable->usSize[RES_IDX_NTSC_30FPS]		    = (sizeof(SNR_BIT1603_Reg_720x240_30P)/sizeof(SNR_BIT1603_Reg_720x240_30P[0]))/2;
    pCust->OprTable->uspTable[RES_IDX_NTSC_30FPS] 	        = &SNR_BIT1603_Reg_720x240_30P[0];

    // Init Vif Setting : Common
    pCust->VifSetting->SnrType                      		= MMPF_VIF_SNR_TYPE_YUV_TV_DEC;
#if 1//(SENSOR_IF == SENSOR_IF_PARALLEL) //Andy Liu test
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
    pCust->VifSetting->powerOnSetting.usExtPowerPinDelay    = 1;
    pCust->VifSetting->powerOnSetting.bFirstEnPinHigh       = MMP_FALSE;
    pCust->VifSetting->powerOnSetting.ubFirstEnPinDelay     = 5;
    pCust->VifSetting->powerOnSetting.bNextEnPinHigh        = MMP_TRUE;
    pCust->VifSetting->powerOnSetting.ubNextEnPinDelay      = 5;
    pCust->VifSetting->powerOnSetting.bTurnOnClockBeforeRst = MMP_TRUE;
    pCust->VifSetting->powerOnSetting.bFirstRstPinHigh      = MMP_FALSE;
    pCust->VifSetting->powerOnSetting.ubFirstRstPinDelay    = 5;
    pCust->VifSetting->powerOnSetting.bNextRstPinHigh       = MMP_TRUE;
    pCust->VifSetting->powerOnSetting.ubNextRstPinDelay     = 5;

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
    pCust->VifSetting->paralAttr.ubLatchTiming 		        = MMPF_VIF_SNR_LATCH_NEG_EDGE;//MMPF_VIF_SNR_LATCH_POS_EDGE;
    pCust->VifSetting->paralAttr.ubHsyncPolarity 	        = MMPF_VIF_SNR_CLK_POLARITY_NEG;
    pCust->VifSetting->paralAttr.ubVsyncPolarity 	        = MMPF_VIF_SNR_CLK_POLARITY_POS;
    pCust->VifSetting->paralAttr.ubBusBitMode               = MMPF_VIF_SNR_PARAL_BITMODE_10;

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

void BOOT_Init_Config(MMPF_SENSOR_CUSTOMER *pCust)
{
	SNR_BIT1603_Cust_InitConfig();
	pCust = &SubSensorCustFunc;
}

//------------------------------------------------------------------------------
//  Function    : SNR_BIT1603_Cust_DoAE_FrmSt
//  Description :
//------------------------------------------------------------------------------
void SNR_BIT1603_Cust_DoAE_FrmSt(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
    // TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_BIT1603_Cust_DoAE_FrmEnd
//  Description :
//------------------------------------------------------------------------------
void SNR_BIT1603_Cust_DoAE_FrmEnd(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_BIT1603_Cust_DoAWB
//  Description :
//------------------------------------------------------------------------------
void SNR_BIT1603_Cust_DoAWB(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_BIT1603_Cust_DoIQ
//  Description :
//------------------------------------------------------------------------------
void SNR_BIT1603_Cust_DoIQ(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_BIT1603_Cust_SetGain
//  Description :
//------------------------------------------------------------------------------
void SNR_BIT1603_Cust_SetGain(MMP_UBYTE ubSnrSel, MMP_ULONG ulGain)
{
    // TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_BIT1603_Cust_SetShutter
//  Description :
//------------------------------------------------------------------------------
void SNR_BIT1603_Cust_SetShutter(MMP_UBYTE ubSnrSel, MMP_ULONG shutter, MMP_ULONG vsync)
{
    // TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_BIT1603_Cust_SetFlip
//  Description :
//------------------------------------------------------------------------------
void SNR_BIT1603_Cust_SetFlip(MMP_UBYTE ubSnrSel, MMP_UBYTE ubMode)
{
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_BIT1603_Cust_SetRotate
//  Description :
//------------------------------------------------------------------------------
void SNR_BIT1603_Cust_SetRotate(MMP_UBYTE ubSnrSel, MMP_UBYTE ubMode)
{
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_BIT1603_Cust_CheckVersion
//  Description :
//------------------------------------------------------------------------------
void SNR_BIT1603_Cust_CheckVersion(MMP_UBYTE ubSnrSel, MMP_ULONG *pulVersion)
{
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_BIT1603_Cust_CheckSnrTVSrcType
//  Description :
//------------------------------------------------------------------------------
void SNR_BIT1603_Cust_CheckSnrTVSrcType(MMP_UBYTE ubSnrSel, MMPF_VIF_TV_DEC_SRC_TYPE *TVSrcType)
{
#if !defined(MBOOT_FW)
	MMP_USHORT usData;
	gsSensorFunction->MMPF_Sensor_GetReg(ubSnrSel, 0x7A, &usData);
	if ((usData & 0x0E) != 0x0E)
	{
		*TVSrcType =  MMPF_VIF_SRC_NO_READY;
	}else if(usData & 0x01)
	{
		*TVSrcType =  MMPF_VIF_SRC_NTSC;
	}else
	{
		*TVSrcType =  MMPF_VIF_SRC_PAL;
	}
#endif
}

void MMPF_Cust_GetSnrID(MMP_UBYTE ubSnrSel, MMP_ULONG *SensorID)
{
	*SensorID = 0xbbbb1603;//The ID is customized.Pls make it different with other sensors.
}


#if (SENSOR_PROI == PRM_SENSOR)
MMPF_SENSOR_CUSTOMER SensorCustFunc = 
{
	SNR_BIT1603_Cust_InitConfig,
	SNR_BIT1603_Cust_DoAE_FrmSt,
	SNR_BIT1603_Cust_DoAE_FrmEnd,
    SNR_BIT1603_Cust_DoAWB,
    SNR_BIT1603_Cust_DoIQ,
	SNR_BIT1603_Cust_SetGain,
	SNR_BIT1603_Cust_SetShutter,
	SNR_BIT1603_Cust_SetFlip,
	SNR_BIT1603_Cust_SetRotate,
	SNR_BIT1603_Cust_CheckVersion,

	&m_BIT1603_SensorRes,
	&m_BIT1603_OprTable,
	&m_BIT1603_VifSetting,
	&m_BIT1603_I2cmAttr,
	&m_BIT1603_AwbTime,
	&m_BIT1603_AeTime,
	&m_BIT1603_AfTime,
    MMP_SNR_PRIO_PRM,
	SNR_BIT1603_Cust_CheckSnrTVSrcType,
	MMPF_Cust_GetSnrID
};

int SNR_Module_Init(void)
{
    MMPF_SensorDrv_Register(PRM_SENSOR, &SensorCustFunc);
    
    return 0;
}
#else
MMPF_SENSOR_CUSTOMER SubSensorCustFunc = 
{
	SNR_BIT1603_Cust_InitConfig,
	SNR_BIT1603_Cust_DoAE_FrmSt,
	SNR_BIT1603_Cust_DoAE_FrmEnd,
    SNR_BIT1603_Cust_DoAWB,
    SNR_BIT1603_Cust_DoIQ,
	SNR_BIT1603_Cust_SetGain,
	SNR_BIT1603_Cust_SetShutter,
	SNR_BIT1603_Cust_SetFlip,
	SNR_BIT1603_Cust_SetRotate,
	SNR_BIT1603_Cust_CheckVersion,

	&m_BIT1603_SensorRes,
	&m_BIT1603_OprTable,
	&m_BIT1603_VifSetting,
	&m_BIT1603_I2cmAttr,
	&m_BIT1603_AwbTime,
	&m_BIT1603_AeTime,
	&m_BIT1603_AfTime,
    MMP_SNR_PRIO_SCD,
    SNR_BIT1603_Cust_CheckSnrTVSrcType,
    MMPF_Cust_GetSnrID
};

int SubSNR_Module_Init(void)
{
#if !defined(MBOOT_FW)
    MMPF_SensorDrv_Register(SCD_SENSOR, &SubSensorCustFunc); 
#endif        
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

#endif // (BIND_SENSOR_BIT1603)
#endif // (SENSOR_EN)

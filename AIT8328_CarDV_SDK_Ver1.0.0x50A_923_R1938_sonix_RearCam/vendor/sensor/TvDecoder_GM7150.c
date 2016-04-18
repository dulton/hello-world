//==============================================================================
//
//  File        : TvDecoder_GM7150.c
//  Description : Firmware Sensor Control File
//  Author      :
//  Revision    : 1.0
//
//=============================================================================

#include "includes_fw.h"
#include "customer_config.h"

#if (defined(DUAL_SENSOR_MBOOT_USE_GM7150) && DUAL_SENSOR_MBOOT_USE_GM7150)
#undef BIND_SENSOR_GM7150
#define BIND_SENSOR_GM7150	(1)
#undef SENSOR_EN
#define SENSOR_EN	(1)
#endif

#if (SENSOR_EN)
#if (BIND_SENSOR_GM7150)

#include "mmpf_sensor.h"
#include "Sensor_Mod_Remapping.h"
#include "isp_if.h"

//==============================================================================
//
//                              GLOBAL VARIABLE
//
//==============================================================================

//Need to check Mode 0 (PAL) is correct or not.
MMPF_SENSOR_RESOLUTION m_GM7150_SensorRes =
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
    {130,   130},//{138,   138},   // usVifGrabStX
    {15,    15},    // usVifGrabStY
    {1440,  1440},	// usVifGrabW
    {242,   242},	// usVifGrabH
    #if (CHIP == MCR_V2)
	{1,   	1}, 	// usBayerInGrabX
	{1,   	1}, 	// usBayerInGrabY
    {720,   720},   // usBayerInDummyX
    {2,     2},     // usBayerInDummyY
    {720,   720},	// usBayerOutW
    {240,   240},	// usBayerOutH
    #endif
    #if (SCD_SNR_USE_DMA_DEINTERLACE)
    {720,   720},	// usScalInputW
    {480,   480},	// usScalInputH
    #else
    {720,   720},	// usScalInputW
    {240,   240},	// usScalInputH
    #endif
#endif    
    {300,   300},	// usTargetFpsx10
    {250,   250},	// usVsyncLine
	{1,   	1}, 	// ubWBinningN
	{1,   	1}, 	// ubWBinningN
	{1,   	1}, 	// ubWBinningN
	{1,   	1}, 	// ubWBinningN
    {0xFF,  0xFF},  // ubCustIQmode
    {0xFF,	0xFF}   // ubCustAEmode
};

// OPR Table and Vif Setting
MMPF_SENSOR_OPR_TABLE       m_GM7150_OprTable;
MMPF_SENSOR_VIF_SETTING     m_GM7150_VifSetting;

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
static MMP_I2CM_ATTR m_GM7150_I2cmAttr = 
{
    MMP_I2CM0,  // i2cmID
    0x5d,       // ubSlaveAddr
    8,          // ubRegLen
    8,          // ubDataLen
    0,          // ubDelayTime
    MMP_FALSE,  // bDelayWaitEn
    MMP_TRUE,  // bInputFilterEn
    MMP_FALSE,  // b10BitModeEn
    MMP_FALSE,  // bClkStretchEn
    0,          // ubSlaveAddr1
    0,          // ubDelayCycle
    0,          // ubPadNum
    300000,     // ulI2cmSpeed 300KHZ
    MMP_TRUE,  // bOsProtectEn
    NULL,       // sw_clk_pin
    NULL,       // sw_data_pin
    MMP_FALSE,  // bRfclModeEn
    MMP_FALSE,  // bWfclModeEn
    MMP_FALSE,	// bRepeatModeEn
    0           // ubVifPioMdlId
};

// 3A Timing
MMPF_SENSOR_AWBTIMIMG   m_GM7150_AwbTime    =
{
	6,	/* ubPeriod */
	1, 	/* ubDoAWBFrmCnt */
	3	/* ubDoCaliFrmCnt */
};

MMPF_SENSOR_AETIMIMG    m_GM7150_AeTime     =
{
	6, 	/* ubPeriod */
	0, 	/* ubFrmStSetShutFrmCnt */
	0	/* ubFrmStSetGainFrmCnt */
};

MMPF_SENSOR_AFTIMIMG    m_GM7150_AfTime     =
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

ISP_UINT16 SNR_GM7150_Reg_Init_Customer[] = 
{
#if 0
	0x00, 0x00,
	0x02, 0x40,
	0x03, 0xaf//0xaf,
	
	0x0d, 0x00,
	0x0f, 0x00,
	0x1a, 0x1c, // Default 0x0b blue pannel  0x0a black pannel
    0x1c, 0x1f, // No Signal, NTSC Blue panel out.
#else
    0x00, 0x00,
    0x03, 0x0d,
    0x0d, 0x40,
	0x8c, 0x80,

    0x0f, 0x00,
#endif   
	

};

#if 0
void ____Sensor_Res_OPR_Table____(){ruturn;} //dummy
#endif

ISP_UINT16 SNR_GM7150_Reg_720x240_30P[] = 
{
    SENSOR_DELAY_REG, 10 // delay
};

#if 0
void ____Sensor_Customer_Func____(){ruturn;} // dummy
#endif

//------------------------------------------------------------------------------
//  Function    : SNR_GM7150_Cust_InitConfig
//  Description :
//------------------------------------------------------------------------------
static void SNR_GM7150_Cust_InitConfig(void)
{
    MMPF_SENSOR_CUSTOMER *pCust = NULL;

#if (SENSOR_PROI == PRM_SENSOR)
    pCust = &SensorCustFunc;
#else
    pCust = &SubSensorCustFunc;
#endif

    RTNA_DBG_Str(0, "SNR_Cust_InitConfig GM7150\r\n");

    // Init OPR Table
    pCust->OprTable->usInitSize 						    = (sizeof(SNR_GM7150_Reg_Init_Customer)/sizeof(SNR_GM7150_Reg_Init_Customer[0]))/2;
    pCust->OprTable->uspInitTable 					        = &SNR_GM7150_Reg_Init_Customer[0];

    pCust->OprTable->bBinTableExist                         = MMP_FALSE;
    pCust->OprTable->bInitDoneTableExist                    = MMP_FALSE;

    //Share with NTSC format due to no initial setting.
    pCust->OprTable->usSize[RES_IDX_PAL_25FPS]		    = (sizeof(SNR_GM7150_Reg_720x240_30P)/sizeof(SNR_GM7150_Reg_720x240_30P[0]))/2;
    pCust->OprTable->uspTable[RES_IDX_PAL_25FPS] 	        = &SNR_GM7150_Reg_720x240_30P[0];

    pCust->OprTable->usSize[RES_IDX_NTSC_30FPS]		    = (sizeof(SNR_GM7150_Reg_720x240_30P)/sizeof(SNR_GM7150_Reg_720x240_30P[0]))/2;
    pCust->OprTable->uspTable[RES_IDX_NTSC_30FPS] 	        = &SNR_GM7150_Reg_720x240_30P[0];

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
    pCust->VifSetting->powerOnSetting.usExtPowerPin 	    = SENSOR_GPIO_REAR_ENABLE; // it might be defined in Config_SDK.h
    pCust->VifSetting->powerOnSetting.bExtPowerPinHigh      = SENSOR_GPIO_REAR_ENABLE_ACT_LEVEL;
    pCust->VifSetting->powerOnSetting.usExtPowerPinDelay    = 5;
    pCust->VifSetting->powerOnSetting.bFirstEnPinHigh       = MMP_FALSE;
    pCust->VifSetting->powerOnSetting.ubFirstEnPinDelay     = 5;
    pCust->VifSetting->powerOnSetting.bNextEnPinHigh        = MMP_TRUE;
    pCust->VifSetting->powerOnSetting.ubNextEnPinDelay      = 5;
    pCust->VifSetting->powerOnSetting.bTurnOnClockBeforeRst = MMP_TRUE;
    pCust->VifSetting->powerOnSetting.bFirstRstPinHigh      = MMP_FALSE;
    pCust->VifSetting->powerOnSetting.ubFirstRstPinDelay    = 10;
    pCust->VifSetting->powerOnSetting.bNextRstPinHigh       = MMP_TRUE;
    pCust->VifSetting->powerOnSetting.ubNextRstPinDelay     = 10;

    // Init Vif Setting : PowerOff Setting
    pCust->VifSetting->powerOffSetting.bEnterStandByMode    = MMP_FALSE;
    pCust->VifSetting->powerOffSetting.usStandByModeReg     = 0x0100;
    pCust->VifSetting->powerOffSetting.usStandByModeMask    = 0x01;
    pCust->VifSetting->powerOffSetting.bEnPinHigh 	        = MMP_TRUE;
    pCust->VifSetting->powerOffSetting.ubEnPinDelay 	    = 10;
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

    pCust->VifSetting->paralAttr.ubLatchTiming				= MMPF_VIF_SNR_LATCH_NEG_EDGE;//MMPF_VIF_SNR_LATCH_POS_EDGE;
    pCust->VifSetting->paralAttr.ubHsyncPolarity			= MMPF_VIF_SNR_CLK_POLARITY_NEG;
    pCust->VifSetting->paralAttr.ubVsyncPolarity			= MMPF_VIF_SNR_CLK_POLARITY_NEG;
    pCust->VifSetting->paralAttr.ubBusBitMode				= MMPF_VIF_SNR_PARAL_BITMODE_10;
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
    if(SENSOR_GPIO_ODDEVEN_STATE != MMP_GPIO_MAX)
    {
        MMPF_PIO_PadConfig(SENSOR_GPIO_ODDEVEN_STATE, PAD_OUT_DRIVING(0), MMP_TRUE);
        MMPF_PIO_EnableOutputMode(SENSOR_GPIO_ODDEVEN_STATE, MMP_FALSE, MMP_TRUE);
    }
	
}

void BOOT_Init_Config(MMPF_SENSOR_CUSTOMER *pCust)
{
	SNR_GM7150_Cust_InitConfig();
	pCust = &SubSensorCustFunc;
}

//------------------------------------------------------------------------------
//  Function    : SNR_GM7150_Cust_DoAE_FrmSt
//  Description :
//------------------------------------------------------------------------------
void SNR_GM7150_Cust_DoAE_FrmSt(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
    // TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_GM7150_Cust_DoAE_FrmEnd
//  Description :
//------------------------------------------------------------------------------
void SNR_GM7150_Cust_DoAE_FrmEnd(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_GM7150_Cust_DoAWB
//  Description :
//------------------------------------------------------------------------------
void SNR_GM7150_Cust_DoAWB(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_GM7150_Cust_DoIQ
//  Description :
//------------------------------------------------------------------------------
void SNR_GM7150_Cust_DoIQ(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_GM7150_Cust_SetGain
//  Description :
//------------------------------------------------------------------------------
void SNR_GM7150_Cust_SetGain(MMP_UBYTE ubSnrSel, MMP_ULONG ulGain)
{
    // TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_GM7150_Cust_SetShutter
//  Description :
//------------------------------------------------------------------------------
void SNR_GM7150_Cust_SetShutter(MMP_UBYTE ubSnrSel, MMP_ULONG shutter, MMP_ULONG vsync)
{
    // TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_GM7150_Cust_SetFlip
//  Description :
//------------------------------------------------------------------------------
void SNR_GM7150_Cust_SetFlip(MMP_UBYTE ubSnrSel, MMP_UBYTE ubMode)
{
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_GM7150_Cust_SetRotate
//  Description :
//------------------------------------------------------------------------------
void SNR_GM7150_Cust_SetRotate(MMP_UBYTE ubSnrSel, MMP_UBYTE ubMode)
{
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_GM7150_Cust_CheckVersion
//  Description :
//------------------------------------------------------------------------------
void SNR_GM7150_Cust_CheckVersion(MMP_UBYTE ubSnrSel, MMP_ULONG *pulVersion)
{
	// TBD
}

//------------------------------------------------------------------------------
//  Function    : SNR_GM7150_Cust_CheckSnrTVSrcType
//  Description :
//------------------------------------------------------------------------------
void SNR_GM7150_Cust_CheckSnrTVSrcType(MMP_UBYTE ubSnrSel, MMPF_VIF_TV_DEC_SRC_TYPE *TVSrcType)
{
	MMP_USHORT usData;
	gsSensorFunction->MMPF_Sensor_GetReg(ubSnrSel, 0x88, &usData);
	//printc("[0x%x]", usData);
	if (((usData & 0x10) == 0x10) && ((usData & 0x06) == 0x00))//bit1&bit2 indicate signal is valide
	{
		//printc("->No ready\r\n");
		*TVSrcType =  MMPF_VIF_SRC_NO_READY;
	}else if(usData & 0x20) //50Hz
	{
		//printc("->PAL\r\n");
		*TVSrcType =  MMPF_VIF_SRC_PAL;
	}else //60Hz
	{
		//printc("->NTSC\r\n");
		*TVSrcType =  MMPF_VIF_SRC_NTSC;
	}
}

void SNR_GM7150_Cust_GetSnrOddEvenState(MMP_UBYTE ubSnrSel, MMP_UBYTE *State)
{
	MMP_UBYTE Value =  0;
    if(SENSOR_GPIO_ODDEVEN_STATE != MMP_GPIO_MAX )
    {
        MMPF_PIO_GetData(SENSOR_GPIO_ODDEVEN_STATE, Value);
        *State = Value;
    }
    else
    {
        *State = 0xff;
    }
}

void SNR_GM7150_Cust_GetSnrID(MMP_UBYTE ubSnrSel, MMP_ULONG *SensorID)
{
	*SensorID = 0xbbbb7150;//The ID is customized.Pls make it different with other sensors.
}

#if (SENSOR_PROI == PRM_SENSOR)
MMPF_SENSOR_CUSTOMER SensorCustFunc = 
{
	SNR_GM7150_Cust_InitConfig,
	SNR_GM7150_Cust_DoAE_FrmSt,
	SNR_GM7150_Cust_DoAE_FrmEnd,
	SNR_GM7150_Cust_DoAWB,
	SNR_GM7150_Cust_DoIQ,
	SNR_GM7150_Cust_SetGain,
	SNR_GM7150_Cust_SetShutter,
	SNR_GM7150_Cust_SetFlip,
	SNR_GM7150_Cust_SetRotate,
	SNR_GM7150_Cust_CheckVersion,

	&m_GM7150_SensorRes,
	&m_GM7150_OprTable,
	&m_GM7150_VifSetting,
	&m_GM7150_I2cmAttr,
	&m_GM7150_AwbTime,
	&m_GM7150_AeTime,
	&m_GM7150_AfTime,
	MMP_SNR_PRIO_PRM,
	SNR_GM7150_Cust_CheckSnrTVSrcType,
	SNR_GM7150_Cust_GetSnrID,
	SNR_GM7150_Cust_GetSnrOddEvenState
};

int SNR_Module_Init(void)
{
    printc("%s,enter\r\n", __func__);
    MMPF_SensorDrv_Register(PRM_SENSOR, &SensorCustFunc);
    
    return 0;
}
#else
MMPF_SENSOR_CUSTOMER SubSensorCustFunc = 
{
	SNR_GM7150_Cust_InitConfig,
	SNR_GM7150_Cust_DoAE_FrmSt,
	SNR_GM7150_Cust_DoAE_FrmEnd,
	SNR_GM7150_Cust_DoAWB,
	SNR_GM7150_Cust_DoIQ,
	SNR_GM7150_Cust_SetGain,
	SNR_GM7150_Cust_SetShutter,
	SNR_GM7150_Cust_SetFlip,
	SNR_GM7150_Cust_SetRotate,
	SNR_GM7150_Cust_CheckVersion,

	&m_GM7150_SensorRes,
	&m_GM7150_OprTable,
	&m_GM7150_VifSetting,
	&m_GM7150_I2cmAttr,
	&m_GM7150_AwbTime,
	&m_GM7150_AeTime,
	&m_GM7150_AfTime,
	MMP_SNR_PRIO_SCD,
	SNR_GM7150_Cust_CheckSnrTVSrcType,
    SNR_GM7150_Cust_GetSnrID,
    SNR_GM7150_Cust_GetSnrOddEvenState
};

int SubSNR_Module_Init(void)
{
    printc("%s,enter\r\n", __func__);
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

#endif // (BIND_SENSOR_GM7150)
#endif // (SENSOR_EN)


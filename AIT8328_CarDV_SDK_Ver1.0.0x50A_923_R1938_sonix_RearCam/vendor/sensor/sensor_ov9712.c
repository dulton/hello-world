//==============================================================================
//
//  File        : sensor_ov9712.c
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
#if (BIND_SENSOR_OV9712)

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
	1,				// ubSensorModeNum
	0,				// ubDefPreviewMode
	0,				// ubDefCaptureMode
	3000,           // usPixelSize (TBD)
//  Mode0   Mode1
    {1  },			// usVifGrabStX
    {41  }, 		// usVifGrabStY
    {1288},			// usVifGrabW
    {730},			// usVifGrabH
    #if (CHIP == MCR_V2)
	{1}, 			// usBayerInGrabX
	{1}, 			// usBayerInGrabY
    {8},  			// usBayerInDummyX
    {10},  			// usBayerInDummyY
    {1280},			// usBayerOutW
    {720},			// usBayerOutH
    #endif
    {1280},			// usScalInputW
    {720},			// usScalInputH
    {300,},			// usTargetFpsx10
    {828},			// usVsyncLine
    {1},  			// ubWBinningN
    {1},  			// ubWBinningM
    {1},  			// ubHBinningN
    {1},  			// ubHBinningM 
    {0xFF},         // ubCustIQmode
    {0xFF}          // ubCustAEmode
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
#include "isp_8590_iq_data_v2_AR0330_v1.xls.ciq.txt"
#endif
};

// I2cm Attribute
static MMP_I2CM_ATTR m_I2cmAttr = {
    MMP_I2CM0,  // i2cmID
    0x30,       // ubSlaveAddr
    8,          // ubRegLen
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
	3	/* ubDoCaliFrmCnt */
};

MMPF_SENSOR_AETIMIMG    m_AeTime     = 
{	
	6, 	/* ubPeriod */
	1, 	/* ubFrmStSetShutFrmCnt */
	3	/* ubFrmStSetGainFrmCnt */
};

MMPF_SENSOR_AFTIMIMG    m_AfTime     = 
{
	1, 	/* ubPeriod */
	0	/* ubDoAFFrmCnt */
};

#if (ISP_EN)
static ISP_UINT32 s_gain;
#endif

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

#if 0
void ____Sensor_Init_OPR_Table____(){ruturn;} //dummy
#endif

MMP_USHORT SNR_OV9712_Reg_Init_Customer[] = 
{
    //------------------------------------------------------------
    //Sensor    : OV9715
    //Sensor Rev    : Rev1B
    //------------------------------------------------------------
    //Setting Rev   : R2.3
    //Resolution    : 1280x800
    //Framerate : 30fps
    //Clock  : 21MHz PCLK@24MHz MCLK
    //DVDD : external DVDD
    //AEC/AGC/AWB : manual mode
    //------------------------------------------------------------
    0x12, 0x80,   //Reset
    SENSOR_DELAY_REG, 100,
    //---------------------------------------------------------
    //Core Settings
    //---------------------------------------------------------
    0x1e, 0x07,
    0x5f, 0x18,
    0x69, 0x04,
    0x65, 0x2a,
    0x68, 0x0a,
    0x39, 0x28,
    0x4d, 0x90,
    0xc1, 0x80,
    0x0c, 0x30,
    0x6d, 0x02,  // default : 0x82, new : 0x02 to reduce fixed pattern noise

    0xC2, 0x80, // [0]: Vsync polarity, 0: low active, 1: high active

    //---------------------------------------------------------
    //DSP
    //---------------------------------------------------------
    0x96, 0xf1, // DSP options enable,
                // [0]: enable ISP
                // [2]: enable the LENC gain coef module
                // [3]: enable LENC module
                // [4]: awb enable
                // [5]: aeb gain function enable
                // [6]: enable white pixel removal function
                // [7]: enable black pixel removal function
    0xbc, 0x68, // [7]   reserved

    // [6:5] bd_sel
    // [4]   th_opt
    // [3:0] thresh_hold
    //---------------------------------------------------------
    //Resolution and Format
    //---------------------------------------------------------
    0x12, 0x00, // [6]: verital downsample, [0]:horizontal downsample
    0x3b, 0x00, // DSP Downsample,
    0x97, 0x80, // [7]   smph_mean
    // [6]   reserved
    // [5:4] smph_drop
    //---- Place generated settings here ----;;
    0x17, 0x25,  // 0x25*8+7 = 0x12F = 303
    0x18, 0xA2,  // 0xA2*8+0 = 0x510 = 1296
    0x19, 0x01,  // 0x01*4+2 = 6
    0x1a, 0xCA,  // 0xCA*4+2 = 0x32A = 810
    0x03, 0x2A,  // [7:5]: VAEC ceiling 1,2,3,7 frames
    0x32, 0x07,

    0x0e, 0x40,  // [3]: VAEC ON/OFF

    0x04, 0x08,  // [7]: mirror, [6]: flip

    0x98, 0x00,
    0x99, 0x00,
    0x9a, 0x00,
    0x57, 0x00,
    0x58, 0xCA,  // DSP output vertical size = REG58[7:0]:REG57[1:0]
    0x59, 0xA2,  // DSP output horizontal size = REG59[7:0]:REG57[4:2]
    0x4c, 0x13,  // reserved
    0x4b, 0x36,  // reserved
    0x3d, 0x3c,  // R counter End Point LSB
    0x3e, 0x03,  // R counter End Point MSB
    0xbd, 0xA0,  // yavg_winh
    0xbe, 0xc8,  // yavg_winv
    //---------------------------------------------------------
    //AWB
    //---------------------------------------------------------
    //
    //---------------------------------------------------------
    //Lens Correction
    //---------------------------------------------------------
    //---- Place lens correction settins here ----;;
    // Lens model   :
    // Module type  :
    //---------------------------------------------------------
    //YAVG
    //---------------------------------------------------------
    //---- Place generated "WIndows Weight" settings here ----;;
    0x4e, 0x55, //AVERAGE
    0x4f, 0x55, //
    0x50, 0x55, //
    0x51, 0x55, //
    0x24, 0x55, //Exposure windows
    0x25, 0x40,
    0x26, 0xa1,
    //---------------------------------------------------------
    //Clock
    //---------------------------------------------------------
	//CCIR_HD_30FPS // OFF: 15fps, ON: 30fps
    0x5c, 0x59,//0x59,  // [6:5]: PLL pre-divider, [4:0]: PLL multiplier
                 // CLK1 = XCLK/REG5C[6:5], CLK2 = CLK1*(32-REG5C[4:0])

    0x5d, 0x00,  // CLK3 = CLK2/(REG5D[3:2]+1)
    0x11, 0x00,//0x01  // [5:0] : system clock divider
                 // SYSCLK = CLK3/((REG11[5:0]+1)*2)
    0x2a, 0x9A,
    0x2b, 0x06,  // line_pixel_width MSB[15:8]

    0x2d, 0x00,  // dummy line LSB[7:0]
    0x2e, 0x00,  // dummy line MSB[15:8]

    //---------------------------------------------------------
    //General
    //---------------------------------------------------------
    //0x13, 0x85,
    0x14, 0x40, //Gain Ceiling 8X

    //0x15, 0x08,
    //---------------------------------------------------------
    //Manual Setting for ISP
    //---------------------------------------------------------
    0x13, 0x00,  //AEC[0],AGC[2] :0-manual, 1-auto/LAEC[3] :0-OFF,1-ON
    0x0d, 0x03,//0x03,  //Manual mode for tp level exposure[1] :1'b0-OFF,1'b0-ON
    #if 1
    0x16, 0x00,  //AEC (r16+r10)
    0x10, 0x01,  //
    #else
    0x16, 0x02,  //AEC (r16+r10)
    0x10, 0x60,  //
    #endif
    0x00, 0x00,  //AGC (r00)
    0x96, 0xc1,  //LCC disable[3][2], AWB[4]-AWB en,AWB[5]-AWB gain en : 0-disable,1-enable
    //0xb6, 0x07,  //AWB[2:0]-awb_fast[2],awb_bias_on[1],awb_bias_plus[0]
    0x41, 0x84,  //BLC target[3:0]
    0x37, 0x02,  //BLC always OFF [2]: 0   2011/7/26

#if 0 //ENABLE_SENSOR_AWB_FUNCTION
    0x38, 0x10,  // [4]: AWB_gain write options
    0xb6, 0x07,  // [3]: Enable manual AWB bias
    0x96, 0xf1,  // DPC black[7],DPC white[6]:0-OFF,1-ON, [5]: apply AWB gain
#else
    0x38, 0x00,  // [4]: AWB_gain write options
    0xb6, 0x08,  // [3]: Enable manual AWB bias
//          0x96, 0xe1,  // DPC black[7],DPC white[6]:0-OFF,1-ON, [5]: apply AWB gain
    0x96, 0x41,  // DPC black[7],DPC white[6]:0-OFF,1-ON, [5]: apply AWB gain
#endif

#if 0//(CUSTOMER == LIT)
    0x63, 0x00,  // [2]: 0, enable internal regulator
#else
    0x63, 0x04,  // [2]: 1, bypass internal regulator
#endif
};

#if 0
void ____Sensor_Res_OPR_Table____(){ruturn;} //dummy
#endif

// 720p 60FPS
MMP_USHORT SNR_OV9712_Reg_1280x720_Customer[] =
{
    //---------------------------------------------------------
    //Resolution and Format
    //---------------------------------------------------------
    0x12, 0x00, // [6]: verital downsample, [0]:horizontal downsample
    0x3b, 0x00, // DSP Downsample,
    0x97, 0x80, // [7]   smph_mean
    // [6]   reserved
    // [5:4] smph_drop

#if (SENSOR_ROTATE_180 == 1)

    0x17, 0x25,  // 0x25*8+7 = 0x12F = 303
    0x18, 0xA2,  // 0xA2*8+0 = 0x510 = 1296
    0x19, 0x00,  // 0x00*4+2 = 2
    0x1a, 0xCA,  // 0xCA*4+2 = 0x32A = 810
    0x03, 0x2A,  // [7:5]: VAEC ceiling 1,2,3,7 frames
    0x32, 0x07,

    0x04, 0xC8,  // [7]: mirror, [6]: flip

#elif (SENSOR_FLIP == 1)

    0x17, 0x25,  // 0x25*8+7 = 0x12F = 303
    0x18, 0xA2,  // 0xA2*8+0 = 0x510 = 1296
    0x19, 0x00,  // 0x00*4+2 = 2
    0x1a, 0xCA,  // 0xCA*4+2 = 0x32A = 810
    0x03, 0x2A,  // [7:5]: VAEC ceiling 1,2,3,7 frames
    0x32, 0x07,

    0x04, 0x48,  // [7]: mirror, [6]: flip

#else

    0x17, 0x25,  // 0x25*8+7 = 0x12F = 303
    0x18, 0xA2,  // 0xA2*8+0 = 0x510 = 1296
    0x19, 0x01,  // 0x01*4+2 = 6
    0x1a, 0xCA,  // 0xCA*4+2 = 0x32A = 810
    0x03, 0x2A,  // [7:5]: VAEC ceiling 1,2,3,7 frames
    0x32, 0x07,

    0x04, 0x08,  // [7]: mirror, [6]: flip

#endif
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
    SensorCustFunc.OprTable->usInitSize                   = (sizeof(SNR_OV9712_Reg_Init_Customer)/sizeof(SNR_OV9712_Reg_Init_Customer[0]))/2;
    SensorCustFunc.OprTable->uspInitTable                 = &SNR_OV9712_Reg_Init_Customer[0];    

    SensorCustFunc.OprTable->bBinTableExist               = MMP_FALSE;
    SensorCustFunc.OprTable->bInitDoneTableExist          = MMP_FALSE;

    SensorCustFunc.OprTable->usSize[RES_IDX_1280x720]     = (sizeof(SNR_OV9712_Reg_1280x720_Customer)/sizeof(SNR_OV9712_Reg_1280x720_Customer[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720]   = &SNR_OV9712_Reg_1280x720_Customer[0];    

    // Init Vif Setting : Common
    SensorCustFunc.VifSetting->SnrType                              = MMPF_VIF_SNR_TYPE_BAYER;
    SensorCustFunc.VifSetting->OutInterface                         = MMPF_VIF_IF_PARALLEL;
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
    SensorCustFunc.VifSetting->powerOnSetting.ubFirstEnPinDelay     = 5;
    SensorCustFunc.VifSetting->powerOnSetting.bNextEnPinHigh        = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOnSetting.ubNextEnPinDelay      = 5;
    SensorCustFunc.VifSetting->powerOnSetting.bTurnOnClockBeforeRst = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.bFirstRstPinHigh      = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOnSetting.ubFirstRstPinDelay    = 5;
    SensorCustFunc.VifSetting->powerOnSetting.bNextRstPinHigh       = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubNextRstPinDelay     = 5;

    // Init Vif Setting : PowerOff Setting
    SensorCustFunc.VifSetting->powerOffSetting.bEnterStandByMode    = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOffSetting.usStandByModeReg     = 0x0;
    SensorCustFunc.VifSetting->powerOffSetting.usStandByModeMask    = 0x0;
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
    MMP_ULONG   ulVsync = 0;
    MMP_ULONG   ulShutter = 0;
	MMP_UBYTE   ubPeriod              = (SensorCustFunc.pAeTime)->ubPeriod;
	MMP_UBYTE   ubFrmStSetShutFrmCnt  = (SensorCustFunc.pAeTime)->ubFrmStSetShutFrmCnt;	
	MMP_UBYTE   ubFrmStSetGainFrmCnt  = (SensorCustFunc.pAeTime)->ubFrmStSetGainFrmCnt;

	if(ulFrameCnt % ubPeriod == ubFrmStSetShutFrmCnt) {

		ISP_IF_AE_Execute();

        s_gain = VR_MAX(ISP_IF_AE_GetGain(), ISP_IF_AE_GetGainBase());

        if (s_gain >= ISP_IF_AE_GetGainBase() * MAX_SENSOR_GAIN) {
            s_gain  = ISP_IF_AE_GetGainBase() * MAX_SENSOR_GAIN;
        } 

		ulVsync     = gSnrLineCntPerSec * ISP_IF_AE_GetVsync() / ISP_IF_AE_GetVsyncBase();
		ulShutter   = gSnrLineCntPerSec * ISP_IF_AE_GetShutter() / ISP_IF_AE_GetShutterBase();
		gsSensorFunction->MMPF_Sensor_SetShutter(ubSnrSel, ulShutter, ulVsync);
    }
	else if(ulFrameCnt % ubPeriod == ubFrmStSetGainFrmCnt) {
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
#if 0
	ISP_UINT16 snr_gain;
	ISP_UINT32 sensor_gain;

	snr_gain    = VR_MIN(16 * VR_MAX(ulGain, ISP_IF_AE_GetGainBase()) / ISP_IF_AE_GetGainBase() , 127);		
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x350A, (sensor_gain >> 8) & 0x01);
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x350B, sensor_gain & 0xff);
#else
	MMP_ULONG  H, L;

	MMP_ULONG gain_base = ISP_IF_AE_GetGainBase();

    //s_gain = ISP_MIN(ISP_MAX(gain, 64), 511); // API input gain range : 64~511, 64=1X

    if (ulGain >= (gain_base << 3)) {
        H = 7;
    } else if (ulGain >= (gain_base << 2)) {
        H = 3;
    } else if (ulGain >= (gain_base << 1)) {
        H = 1;
    } else {
        H = 0;
    }

    L = (ulGain << 4) / gain_base / (H+1) - 16;

	if (L > 15) L = 15;

	// set sensor gain
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x00, (H<<4)+L);
#endif	
#endif
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetShutter
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetShutter(MMP_UBYTE ubSnrSel, MMP_ULONG shutter, MMP_ULONG vsync)
{
	// 1 ~ (frame length line - 6)
#if 0
	ISP_UINT32 ExtraVsyncWidth;
	
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x380E, vsync >> 8);
	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x380F, vsync);                
	
	if(shutter <= (vsync - 6)){
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x350C, 0);
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x350D, 0);

		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3500, (ISP_UINT8)((shutter >> 12) & 0xFF));
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3501, (ISP_UINT8)((shutter >> 4) & 0xFF));
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3502, (ISP_UINT8)((shutter << 4) & 0xFF));
	}
	else{
		ExtraVsyncWidth = (shutter + 6) - vsync;
		
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x350C, (ISP_UINT8)((ExtraVsyncWidth >> 8) & 0xFF));
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x350D, (ISP_UINT8)((ExtraVsyncWidth) & 0xFF));

		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3500, (ISP_UINT8)((shutter >> 12) & 0xFF));
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3501, (ISP_UINT8)((shutter >> 4) & 0xFF));
		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3502, (ISP_UINT8)((shutter << 4) & 0xFF));
	}
#else
    ISP_UINT32 new_vsync = 828 * 30 * ISP_IF_AE_GetVsync() / ISP_IF_AE_GetVsyncBase();
    ISP_UINT32 new_shutter = 828 * 30 * ISP_IF_AE_GetShutter() / ISP_IF_AE_GetShutterBase();

    new_vsync = ISP_MIN(ISP_MAX(new_shutter + 3, new_vsync), 0xFFFF);
    new_shutter = ISP_MIN(ISP_MAX(new_shutter, 1), new_vsync - 3);
	
    gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3E, new_vsync >> 8);
    gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x3D, new_vsync);

    gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x16, new_shutter >> 8);
    gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x10, new_shutter);
#endif	
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetFlip
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetFlip(MMP_UBYTE ubSnrSel, MMP_UBYTE ubMode)
{
    switch(ubMode) {
    case MMPF_SENSOR_NO_FLIP:
   		gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x19, 0x01);// VSTART-M 0x01*4+2 = 6
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x04, 0x08);// [7]: mirror, [6]: flip
    	break;
    case MMPF_SENSOR_COLUMN_FLIP:
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x19, 0x00);// 0x00*4+2 = 2
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x04, 0x48);// [7]: mirror, [6]: flip    
    	break;
    case MMPF_SENSOR_ROW_FLIP:
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x19, 0x01);// VSTART-M 0x01*4+2 = 6
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x04, 0x88);// [7]: mirror, [6]: fli
        break;    
    case MMPF_SENSOR_COLROW_FLIP:
    	break;
    default:
		break;
    }
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetRotate
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetRotate(MMP_UBYTE ubSnrSel, MMP_UBYTE ubMode)
{
    switch(ubMode) {
    case MMPF_SENSOR_ROTATE_NO_ROTATE:
    	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x19, 0x01);// VSTART-M 0x01*4+2 = 6
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x04, 0x08);// [7]: mirror, [6]: flip
    	break;
    case MMPF_SENSOR_ROTATE_RIGHT_90:
    	break;
    case MMPF_SENSOR_ROTATE_RIGHT_180:
    	gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x19, 0x00);// 0x00*4+2 = 2
        gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x04, 0xC8);// [7]: mirror, [6]: flip
        break;
    case MMPF_SENSOR_ROTATE_RIGHT_270:
    	break;
    default:
		break;
    }
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

#endif  //BIND_SENSOR_OV9712
#endif	//SENSOR_EN

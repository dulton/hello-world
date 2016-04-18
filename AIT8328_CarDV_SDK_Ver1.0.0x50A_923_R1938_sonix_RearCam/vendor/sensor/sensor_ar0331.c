//==============================================================================
//
//  File        : sensor_ar0331.c
//  Description : Firmware Sensor Control File
//  Author      : lucas
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
#if (BIND_SENSOR_AR0331)

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
	3000,           // usPixelSize (TBD)
//  Mode0   Mode1
    {1,     1},	    // usVifGrabStX
    {1,     1},	    // usVifGrabStY
    {1928,  1928},	// usVifGrabW
    {1088,  1088},	// usVifGrabH
    #if (CHIP == MCR_V2)
	{1,   	1}, 	// usBayerInGrabX
	{1,   	1}, 	// usBayerInGrabY
    {8,     8},     // usBayerInDummyX
    {8,     8},     // usBayerInDummyY
    {1920,  1920},	// usBayerOutW
    {1080,  1080},	// usBayerOutH
    #endif
    {1920,  1920},	// usScalInputW
    {1080,  1080},	// usScalInputH
    {300,   300},	// usTargetFpsx10
    {1125,  1125}	// usVsyncLine
	{1,   	1}, 	// ubWBinningN
	{1,   	1}, 	// ubWBinningM
	{1,   	1}, 	// ubHBinningN
	{1,   	1}, 	// ubHBinningM
    {0xFF,  0xFF},  // ubCustIQmode
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
#include "isp_8428_iq_data_v2_AR0331.xls.ciq.txt"
#endif
};

// I2cm Attribute
static MMP_I2CM_ATTR m_I2cmAttr = 
{
    MMP_I2CM0,  // i2cmID
    0x10,       // ubSlaveAddr
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
    150000,     // ulI2cmSpeed 150KHZ
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

MMP_USHORT SNR_AR0331_Reg_Init_Customer[] = 
{

    0x301A, 0x0001,	//Reset
    0xFFFF, 200, 	//APTINA necessary for OTP loading
    0x301A, 0x10D8,	//RESET_REGISTER

	// New linear mode from Joyce 0509
	#if (!REMOVE_SENSOR_MANUAL_OTP_SETTINGS)
	0x3088, 0x8000, //SEQ_CTRL_PORT
	0x3086, 0x4A03, //SEQ_DATA_PORT
	0x3086, 0x4316, //SEQ_DATA_PORT
	0x3086, 0x0443, //SEQ_DATA_PORT
	0x3086, 0x1645, //SEQ_DATA_PORT
	0x3086, 0x4045, //SEQ_DATA_PORT
	0x3086, 0x6017, //SEQ_DATA_PORT
	0x3086, 0x5045, //SEQ_DATA_PORT
	0x3086, 0x404B, //SEQ_DATA_PORT
	0x3086, 0x1244, //SEQ_DATA_PORT
	0x3086, 0x6134, //SEQ_DATA_PORT
	0x3086, 0x4A31, //SEQ_DATA_PORT
	0x3086, 0x4342, //SEQ_DATA_PORT
	0x3086, 0x4560, //SEQ_DATA_PORT
	0x3086, 0x2714, //SEQ_DATA_PORT
	0x3086, 0x3DFF, //SEQ_DATA_PORT
	0x3086, 0x3DFF, //SEQ_DATA_PORT
	0x3086, 0x3DEA, //SEQ_DATA_PORT
	0x3086, 0x2704, //SEQ_DATA_PORT
	0x3086, 0x3D10, //SEQ_DATA_PORT
	0x3086, 0x2705, //SEQ_DATA_PORT
	0x3086, 0x3D10, //SEQ_DATA_PORT
	0x3086, 0x2715, //SEQ_DATA_PORT
	0x3086, 0x3527, //SEQ_DATA_PORT
	0x3086, 0x053D, //SEQ_DATA_PORT
	0x3086, 0x1045, //SEQ_DATA_PORT
	0x3086, 0x4027, //SEQ_DATA_PORT
	0x3086, 0x0427, //SEQ_DATA_PORT
	0x3086, 0x143D, //SEQ_DATA_PORT
	0x3086, 0xFF3D, //SEQ_DATA_PORT
	0x3086, 0xFF3D, //SEQ_DATA_PORT
	0x3086, 0xEA62, //SEQ_DATA_PORT
	0x3086, 0x2728, //SEQ_DATA_PORT
	0x3086, 0x3627, //SEQ_DATA_PORT
	0x3086, 0x083D, //SEQ_DATA_PORT
	0x3086, 0x6444, //SEQ_DATA_PORT
	0x3086, 0x2C2C, //SEQ_DATA_PORT
	0x3086, 0x2C2C, //SEQ_DATA_PORT
	0x3086, 0x4B01, //SEQ_DATA_PORT
	0x3086, 0x432D, //SEQ_DATA_PORT
	0x3086, 0x4643, //SEQ_DATA_PORT
	0x3086, 0x1647, //SEQ_DATA_PORT
	0x3086, 0x435F, //SEQ_DATA_PORT
	0x3086, 0x4F50, //SEQ_DATA_PORT
	0x3086, 0x2604, //SEQ_DATA_PORT
	0x3086, 0x2694, //SEQ_DATA_PORT
	0x3086, 0x2027, //SEQ_DATA_PORT
	0x3086, 0xFC53, //SEQ_DATA_PORT
	0x3086, 0x0D5C, //SEQ_DATA_PORT
	0x3086, 0x0D57, //SEQ_DATA_PORT
	0x3086, 0x5417, //SEQ_DATA_PORT
	0x3086, 0x0955, //SEQ_DATA_PORT
	0x3086, 0x5649, //SEQ_DATA_PORT
	0x3086, 0x5307, //SEQ_DATA_PORT
	0x3086, 0x5303, //SEQ_DATA_PORT
	0x3086, 0x4D28, //SEQ_DATA_PORT
	0x3086, 0x6C4C, //SEQ_DATA_PORT
	0x3086, 0x0928, //SEQ_DATA_PORT
	0x3086, 0x2C28, //SEQ_DATA_PORT
	0x3086, 0x294E, //SEQ_DATA_PORT
	0x3086, 0x5C09, //SEQ_DATA_PORT
	0x3086, 0x4500, //SEQ_DATA_PORT
	0x3086, 0x4580, //SEQ_DATA_PORT
	0x3086, 0x26B6, //SEQ_DATA_PORT
	0x3086, 0x27F8, //SEQ_DATA_PORT
	0x3086, 0x1702, //SEQ_DATA_PORT
	0x3086, 0x27FA, //SEQ_DATA_PORT
	0x3086, 0x5C0B, //SEQ_DATA_PORT
	0x3086, 0x1718, //SEQ_DATA_PORT
	0x3086, 0x26B2, //SEQ_DATA_PORT
	0x3086, 0x5C03, //SEQ_DATA_PORT
	0x3086, 0x1744, //SEQ_DATA_PORT
	0x3086, 0x27F2, //SEQ_DATA_PORT
	0x3086, 0x1702, //SEQ_DATA_PORT
	0x3086, 0x2809, //SEQ_DATA_PORT
	0x3086, 0x1710, //SEQ_DATA_PORT
	0x3086, 0x1628, //SEQ_DATA_PORT
	0x3086, 0x084D, //SEQ_DATA_PORT
	0x3086, 0x1A26, //SEQ_DATA_PORT
	0x3086, 0x9316, //SEQ_DATA_PORT
	0x3086, 0x1627, //SEQ_DATA_PORT
	0x3086, 0xFA45, //SEQ_DATA_PORT
	0x3086, 0xA017, //SEQ_DATA_PORT
	0x3086, 0x0727, //SEQ_DATA_PORT
	0x3086, 0xFB17, //SEQ_DATA_PORT
	0x3086, 0x2945, //SEQ_DATA_PORT
	0x3086, 0x8017, //SEQ_DATA_PORT
	0x3086, 0x0827, //SEQ_DATA_PORT
	0x3086, 0xFA17, //SEQ_DATA_PORT
	0x3086, 0x285D, //SEQ_DATA_PORT
	0x3086, 0x170E, //SEQ_DATA_PORT
	0x3086, 0x2691, //SEQ_DATA_PORT
	0x3086, 0x5301, //SEQ_DATA_PORT
	0x3086, 0x1740, //SEQ_DATA_PORT
	0x3086, 0x5302, //SEQ_DATA_PORT
	0x3086, 0x1710, //SEQ_DATA_PORT
	0x3086, 0x2693, //SEQ_DATA_PORT
	0x3086, 0x2692, //SEQ_DATA_PORT
	0x3086, 0x484D, //SEQ_DATA_PORT
	0x3086, 0x4E28, //SEQ_DATA_PORT
	0x3086, 0x094C, //SEQ_DATA_PORT
	0x3086, 0x0B17, //SEQ_DATA_PORT
	0x3086, 0x5F27, //SEQ_DATA_PORT
	0x3086, 0xF217, //SEQ_DATA_PORT
	0x3086, 0x1428, //SEQ_DATA_PORT
	0x3086, 0x0816, //SEQ_DATA_PORT
	0x3086, 0x4D1A, //SEQ_DATA_PORT
	0x3086, 0x1616, //SEQ_DATA_PORT
	0x3086, 0x27FA, //SEQ_DATA_PORT
	0x3086, 0x2603, //SEQ_DATA_PORT
	0x3086, 0x5C01, //SEQ_DATA_PORT
	0x3086, 0x4540, //SEQ_DATA_PORT
	0x3086, 0x2798, //SEQ_DATA_PORT
	0x3086, 0x172A, //SEQ_DATA_PORT
	0x3086, 0x4A0A, //SEQ_DATA_PORT
	0x3086, 0x4316, //SEQ_DATA_PORT
	0x3086, 0x0B43, //SEQ_DATA_PORT
	0x3086, 0x279C, //SEQ_DATA_PORT
	0x3086, 0x4560, //SEQ_DATA_PORT
	0x3086, 0x1707, //SEQ_DATA_PORT
	0x3086, 0x279D, //SEQ_DATA_PORT
	0x3086, 0x1725, //SEQ_DATA_PORT
	0x3086, 0x4540, //SEQ_DATA_PORT
	0x3086, 0x1708, //SEQ_DATA_PORT
	0x3086, 0x2798, //SEQ_DATA_PORT
	0x3086, 0x5D53, //SEQ_DATA_PORT
	0x3086, 0x0D26, //SEQ_DATA_PORT
	0x3086, 0x455C, //SEQ_DATA_PORT
	0x3086, 0x014B, //SEQ_DATA_PORT
	0x3086, 0x1244, //SEQ_DATA_PORT
	0x3086, 0x5251, //SEQ_DATA_PORT
	0x3086, 0x1702, //SEQ_DATA_PORT
	0x3086, 0x6018, //SEQ_DATA_PORT
	0x3086, 0x4A03, //SEQ_DATA_PORT
	0x3086, 0x4316, //SEQ_DATA_PORT
	0x3086, 0x0443, //SEQ_DATA_PORT
	0x3086, 0x1658, //SEQ_DATA_PORT
	0x3086, 0x4316, //SEQ_DATA_PORT
	0x3086, 0x5943, //SEQ_DATA_PORT
	0x3086, 0x165A, //SEQ_DATA_PORT
	0x3086, 0x4316, //SEQ_DATA_PORT
	0x3086, 0x5B43, //SEQ_DATA_PORT
	0x3086, 0x4540, //SEQ_DATA_PORT
	0x3086, 0x279C, //SEQ_DATA_PORT
	0x3086, 0x4560, //SEQ_DATA_PORT
	0x3086, 0x1707, //SEQ_DATA_PORT
	0x3086, 0x279D, //SEQ_DATA_PORT
	0x3086, 0x1725, //SEQ_DATA_PORT
	0x3086, 0x4540, //SEQ_DATA_PORT
	0x3086, 0x1710, //SEQ_DATA_PORT
	0x3086, 0x2798, //SEQ_DATA_PORT
	0x3086, 0x1720, //SEQ_DATA_PORT
	0x3086, 0x224B, //SEQ_DATA_PORT
	0x3086, 0x1244, //SEQ_DATA_PORT
	0x3086, 0x2C2C, //SEQ_DATA_PORT
	0x3086, 0x2C2C, //SEQ_DATA_PORT
	#endif

	0x301A, 0x10D8, //RESET_REGISTER
	0x30B0, 0x0000, //DIGITAL_TEST
	0x30BA, 0x06EC, //DIGITAL_CTRL
	0x31D0, 0x0000, //COMPANDING
	0x31AC, 0x0A0A, //DATA_FORMAT_BITS
	0x31AE, 0x0301, //SERIAL_FORMAT

	0x3082, 0x0009, //OPERATION_MODE_CTRL
	0x318C, 0x0000, //HDR_MC_CTRL2
	0x3190, 0x0000, //HDR_MC_CTRL4
	0x30FE, 0x0080, //NOISE_PEDESTAL
	0x320A, 0x0080, //ADACD_PEDESTAL
	0x2400, 0x0001, //ALTM_CONTROL
	0x2450, 0x0000, //ALTM_OUT_PEDESTAL
	0x301E, 0x00A8, //DATA_PEDESTAL
	0x3200, 0x0000, //ADACD_CONTROL
	0x31D0, 0x0000, //COMPANDING
	0x31E0, 0x0200, //PIX_DEF_ID
	0x3060, 0x0006, //ANALOG_GAIN
	0x3064, 0x1802, //SMIA_TEST

	0x306E, 0x9010, //

	0x3180, 0x8089, //DELTA_DK_CONTROL
	0x30F4, 0x4000, //ADC_COMMAND3_HS
	0x3ED4, 0x8F6C, //DAC_LD_8_9
	0x3ED6, 0x66CC, //DAC_LD_10_11
	0x3ED8, 0x8C42, //DAC_LD_12_13
	0x3EDA, 0x8899, //DAC_LD_14_15
	0x3EE6, 0x00F0, //DAC_LD_26_27

	0x3ED2, 0x1F46, //DAC_LD_6_7

	//24 /8 *198 /1 /8 = 74.25MHz
	0x302A, 0x0008, //VT_PIX_CLK_DIV
	0x302C, 0x0001, //VT_SYS_CLK_DIV
	0x302E, 0x0008, //PRE_PLL_CLK_DIV
	0x3030, 0x00C6, //PLL_MULTIPLIER
	0x3036, 0x000C, //OP_PIX_CLK_DIV
	0x3038, 0x0001, //OP_SYS_CLK_DIV
	#if 0
	0x3002, 0x00E4, //Y_ADDR_START//228
	0x3004, 0x0042, //X_ADDR_START//66
	0x3006, 0x0523, //Y_ADDR_END//1315=1087+228
	0x3008, 0x07C9, //X_ADDR_END//1993=1927+66
	0x300A, 0x0465, //FRAME_LENGTH_LINES//1125
	0x300C, 0x044C, //LINE_LENGTH_PCK//1100
	0x3012, 0x0465, //COARSE_INTEGRATION_TIME
	0x3014, 0x0000, //
	0x30A2, 0x0001, //X_ODD_INC
	0x30A6, 0x0001, //Y_ODD_INC
	0x3040, 0x0000, //READ_MODE

	0x301A, 0x10DC, //RESET_REGISTER
	#endif
};

MMP_USHORT SNR_AR0331_HDR_Reg_Init_Customer[] = 
{
	// New HDR mode from Joyce 0509
	#if (!REMOVE_SENSOR_MANUAL_OTP_SETTINGS)
	0x3088, 0x8000, //SEQ_CTRL_PORT
	0x3086, 0x4540, //SEQ_DATA_PORT
	0x3086, 0x6134, //SEQ_DATA_PORT
	0x3086, 0x4A31, //SEQ_DATA_PORT
	0x3086, 0x4342, //SEQ_DATA_PORT
	0x3086, 0x4560, //SEQ_DATA_PORT
	0x3086, 0x2714, //SEQ_DATA_PORT
	0x3086, 0x3DFF, //SEQ_DATA_PORT
	0x3086, 0x3DFF, //SEQ_DATA_PORT
	0x3086, 0x3DEA, //SEQ_DATA_PORT
	0x3086, 0x2704, //SEQ_DATA_PORT
	0x3086, 0x3D10, //SEQ_DATA_PORT
	0x3086, 0x2705, //SEQ_DATA_PORT
	0x3086, 0x3D10, //SEQ_DATA_PORT
	0x3086, 0x2715, //SEQ_DATA_PORT
	0x3086, 0x3527, //SEQ_DATA_PORT
	0x3086, 0x053D, //SEQ_DATA_PORT
	0x3086, 0x1045, //SEQ_DATA_PORT
	0x3086, 0x4027, //SEQ_DATA_PORT
	0x3086, 0x0427, //SEQ_DATA_PORT
	0x3086, 0x143D, //SEQ_DATA_PORT
	0x3086, 0xFF3D, //SEQ_DATA_PORT
	0x3086, 0xFF3D, //SEQ_DATA_PORT
	0x3086, 0xEA62, //SEQ_DATA_PORT
	0x3086, 0x2728, //SEQ_DATA_PORT
	0x3086, 0x3627, //SEQ_DATA_PORT
	0x3086, 0x083D, //SEQ_DATA_PORT
	0x3086, 0x6444, //SEQ_DATA_PORT
	0x3086, 0x2C2C, //SEQ_DATA_PORT
	0x3086, 0x2C2C, //SEQ_DATA_PORT
	0x3086, 0x4B00, //SEQ_DATA_PORT
	0x3086, 0x432D, //SEQ_DATA_PORT
	0x3086, 0x6343, //SEQ_DATA_PORT
	0x3086, 0x1664, //SEQ_DATA_PORT
	0x3086, 0x435F, //SEQ_DATA_PORT
	0x3086, 0x4F50, //SEQ_DATA_PORT
	0x3086, 0x2604, //SEQ_DATA_PORT
	0x3086, 0x2694, //SEQ_DATA_PORT
	0x3086, 0x27FC, //SEQ_DATA_PORT
	0x3086, 0x530D, //SEQ_DATA_PORT
	0x3086, 0x5C0D, //SEQ_DATA_PORT
	0x3086, 0x5754, //SEQ_DATA_PORT
	0x3086, 0x1709, //SEQ_DATA_PORT
	0x3086, 0x5556, //SEQ_DATA_PORT
	0x3086, 0x4953, //SEQ_DATA_PORT
	0x3086, 0x0753, //SEQ_DATA_PORT
	0x3086, 0x034D, //SEQ_DATA_PORT
	0x3086, 0x286C, //SEQ_DATA_PORT
	0x3086, 0x4C09, //SEQ_DATA_PORT
	0x3086, 0x282C, //SEQ_DATA_PORT
	0x3086, 0x2828, //SEQ_DATA_PORT
	0x3086, 0x261C, //SEQ_DATA_PORT
	0x3086, 0x4E5C, //SEQ_DATA_PORT
	0x3086, 0x0960, //SEQ_DATA_PORT
	0x3086, 0x4500, //SEQ_DATA_PORT
	0x3086, 0x4580, //SEQ_DATA_PORT
	0x3086, 0x26BE, //SEQ_DATA_PORT
	0x3086, 0x27F8, //SEQ_DATA_PORT
	0x3086, 0x1702, //SEQ_DATA_PORT
	0x3086, 0x27FA, //SEQ_DATA_PORT
	0x3086, 0x5C0B, //SEQ_DATA_PORT
	0x3086, 0x1712, //SEQ_DATA_PORT
	0x3086, 0x26BA, //SEQ_DATA_PORT
	0x3086, 0x5C03, //SEQ_DATA_PORT
	0x3086, 0x1713, //SEQ_DATA_PORT
	0x3086, 0x27F2, //SEQ_DATA_PORT
	0x3086, 0x171C, //SEQ_DATA_PORT
	0x3086, 0x5F28, //SEQ_DATA_PORT
	0x3086, 0x0817, //SEQ_DATA_PORT
	0x3086, 0x0360, //SEQ_DATA_PORT
	0x3086, 0x173C, //SEQ_DATA_PORT
	0x3086, 0x26B2, //SEQ_DATA_PORT
	0x3086, 0x1616, //SEQ_DATA_PORT
	0x3086, 0x5F4D, //SEQ_DATA_PORT
	0x3086, 0x1926, //SEQ_DATA_PORT
	0x3086, 0x9316, //SEQ_DATA_PORT
	0x3086, 0x1627, //SEQ_DATA_PORT
	0x3086, 0xFA45, //SEQ_DATA_PORT
	0x3086, 0xA017, //SEQ_DATA_PORT
	0x3086, 0x0527, //SEQ_DATA_PORT
	0x3086, 0xFB17, //SEQ_DATA_PORT
	0x3086, 0x1F45, //SEQ_DATA_PORT
	0x3086, 0x801F, //SEQ_DATA_PORT
	0x3086, 0x1705, //SEQ_DATA_PORT
	0x3086, 0x27FA, //SEQ_DATA_PORT
	0x3086, 0x171E, //SEQ_DATA_PORT
	0x3086, 0x5D17, //SEQ_DATA_PORT
	0x3086, 0x0C26, //SEQ_DATA_PORT
	0x3086, 0x9248, //SEQ_DATA_PORT
	0x3086, 0x4D4E, //SEQ_DATA_PORT
	0x3086, 0x269A, //SEQ_DATA_PORT
	0x3086, 0x2808, //SEQ_DATA_PORT
	0x3086, 0x4C0B, //SEQ_DATA_PORT
	0x3086, 0x6017, //SEQ_DATA_PORT
	0x3086, 0x0327, //SEQ_DATA_PORT
	0x3086, 0xF217, //SEQ_DATA_PORT
	0x3086, 0x2626, //SEQ_DATA_PORT
	0x3086, 0x9216, //SEQ_DATA_PORT
	0x3086, 0x165F, //SEQ_DATA_PORT
	0x3086, 0x4D19, //SEQ_DATA_PORT
	0x3086, 0x2693, //SEQ_DATA_PORT
	0x3086, 0x1616, //SEQ_DATA_PORT
	0x3086, 0x27FA, //SEQ_DATA_PORT
	0x3086, 0x2643, //SEQ_DATA_PORT
	0x3086, 0x5C01, //SEQ_DATA_PORT
	0x3086, 0x4540, //SEQ_DATA_PORT
	0x3086, 0x2798, //SEQ_DATA_PORT
	0x3086, 0x1720, //SEQ_DATA_PORT
	0x3086, 0x4A65, //SEQ_DATA_PORT
	0x3086, 0x4316, //SEQ_DATA_PORT
	0x3086, 0x6643, //SEQ_DATA_PORT
	0x3086, 0x5A43, //SEQ_DATA_PORT
	0x3086, 0x165B, //SEQ_DATA_PORT
	0x3086, 0x4327, //SEQ_DATA_PORT
	0x3086, 0x9C45, //SEQ_DATA_PORT
	0x3086, 0x6017, //SEQ_DATA_PORT
	0x3086, 0x0627, //SEQ_DATA_PORT
	0x3086, 0x9D17, //SEQ_DATA_PORT
	0x3086, 0x1C45, //SEQ_DATA_PORT
	0x3086, 0x4023, //SEQ_DATA_PORT
	0x3086, 0x1705, //SEQ_DATA_PORT
	0x3086, 0x2798, //SEQ_DATA_PORT
	0x3086, 0x5D26, //SEQ_DATA_PORT
	0x3086, 0x4417, //SEQ_DATA_PORT
	0x3086, 0x0E28, //SEQ_DATA_PORT
	0x3086, 0x0053, //SEQ_DATA_PORT
	0x3086, 0x014B, //SEQ_DATA_PORT
	0x3086, 0x5251, //SEQ_DATA_PORT
	0x3086, 0x1244, //SEQ_DATA_PORT
	0x3086, 0x4B01, //SEQ_DATA_PORT
	0x3086, 0x432D, //SEQ_DATA_PORT
	0x3086, 0x4643, //SEQ_DATA_PORT
	0x3086, 0x1647, //SEQ_DATA_PORT
	0x3086, 0x434F, //SEQ_DATA_PORT
	0x3086, 0x5026, //SEQ_DATA_PORT
	0x3086, 0x0426, //SEQ_DATA_PORT
	0x3086, 0x8427, //SEQ_DATA_PORT
	0x3086, 0xFC53, //SEQ_DATA_PORT
	0x3086, 0x0D5C, //SEQ_DATA_PORT
	0x3086, 0x0D57, //SEQ_DATA_PORT
	0x3086, 0x5417, //SEQ_DATA_PORT
	0x3086, 0x0955, //SEQ_DATA_PORT
	0x3086, 0x5649, //SEQ_DATA_PORT
	0x3086, 0x5307, //SEQ_DATA_PORT
	0x3086, 0x5303, //SEQ_DATA_PORT
	0x3086, 0x4D28, //SEQ_DATA_PORT
	0x3086, 0x6C4C, //SEQ_DATA_PORT
	0x3086, 0x0928, //SEQ_DATA_PORT
	0x3086, 0x2C28, //SEQ_DATA_PORT
	0x3086, 0x2826, //SEQ_DATA_PORT
	0x3086, 0x0C4E, //SEQ_DATA_PORT
	0x3086, 0x5C09, //SEQ_DATA_PORT
	0x3086, 0x6045, //SEQ_DATA_PORT
	0x3086, 0x0045, //SEQ_DATA_PORT
	0x3086, 0x8026, //SEQ_DATA_PORT
	0x3086, 0xAE27, //SEQ_DATA_PORT
	0x3086, 0xF817, //SEQ_DATA_PORT
	0x3086, 0x0227, //SEQ_DATA_PORT
	0x3086, 0xFA5C, //SEQ_DATA_PORT
	0x3086, 0x0B17, //SEQ_DATA_PORT
	0x3086, 0x1226, //SEQ_DATA_PORT
	0x3086, 0xAA5C, //SEQ_DATA_PORT
	0x3086, 0x0317, //SEQ_DATA_PORT
	0x3086, 0x0B27, //SEQ_DATA_PORT
	0x3086, 0xF217, //SEQ_DATA_PORT
	0x3086, 0x265F, //SEQ_DATA_PORT
	0x3086, 0x2808, //SEQ_DATA_PORT
	0x3086, 0x1703, //SEQ_DATA_PORT
	0x3086, 0x6017, //SEQ_DATA_PORT
	0x3086, 0x0326, //SEQ_DATA_PORT
	0x3086, 0xA216, //SEQ_DATA_PORT
	0x3086, 0x165F, //SEQ_DATA_PORT
	0x3086, 0x4D1A, //SEQ_DATA_PORT
	0x3086, 0x2683, //SEQ_DATA_PORT
	0x3086, 0x1616, //SEQ_DATA_PORT
	0x3086, 0x27FA, //SEQ_DATA_PORT
	0x3086, 0x45A0, //SEQ_DATA_PORT
	0x3086, 0x1705, //SEQ_DATA_PORT
	0x3086, 0x27FB, //SEQ_DATA_PORT
	0x3086, 0x171F, //SEQ_DATA_PORT
	0x3086, 0x4580, //SEQ_DATA_PORT
	0x3086, 0x2017, //SEQ_DATA_PORT
	0x3086, 0x0527, //SEQ_DATA_PORT
	0x3086, 0xFA17, //SEQ_DATA_PORT
	0x3086, 0x1E5D, //SEQ_DATA_PORT
	0x3086, 0x170C, //SEQ_DATA_PORT
	0x3086, 0x2682, //SEQ_DATA_PORT
	0x3086, 0x484D, //SEQ_DATA_PORT
	0x3086, 0x4E26, //SEQ_DATA_PORT
	0x3086, 0x8A28, //SEQ_DATA_PORT
	0x3086, 0x084C, //SEQ_DATA_PORT
	0x3086, 0x0B60, //SEQ_DATA_PORT
	0x3086, 0x1707, //SEQ_DATA_PORT
	0x3086, 0x27F2, //SEQ_DATA_PORT
	0x3086, 0x1738, //SEQ_DATA_PORT
	0x3086, 0x2682, //SEQ_DATA_PORT
	0x3086, 0x1616, //SEQ_DATA_PORT
	0x3086, 0x5F4D, //SEQ_DATA_PORT
	0x3086, 0x1A26, //SEQ_DATA_PORT
	0x3086, 0x8316, //SEQ_DATA_PORT
	0x3086, 0x1627, //SEQ_DATA_PORT
	0x3086, 0xFA26, //SEQ_DATA_PORT
	0x3086, 0x435C, //SEQ_DATA_PORT
	0x3086, 0x0145, //SEQ_DATA_PORT
	0x3086, 0x4027, //SEQ_DATA_PORT
	0x3086, 0x9817, //SEQ_DATA_PORT
	0x3086, 0x1F4A, //SEQ_DATA_PORT
	0x3086, 0x1244, //SEQ_DATA_PORT
	0x3086, 0x0343, //SEQ_DATA_PORT
	0x3086, 0x1604, //SEQ_DATA_PORT
	0x3086, 0x4316, //SEQ_DATA_PORT
	0x3086, 0x5843, //SEQ_DATA_PORT
	0x3086, 0x1659, //SEQ_DATA_PORT
	0x3086, 0x4316, //SEQ_DATA_PORT
	0x3086, 0x279C, //SEQ_DATA_PORT
	0x3086, 0x4560, //SEQ_DATA_PORT
	0x3086, 0x1705, //SEQ_DATA_PORT
	0x3086, 0x279D, //SEQ_DATA_PORT
	0x3086, 0x171D, //SEQ_DATA_PORT
	0x3086, 0x4540, //SEQ_DATA_PORT
	0x3086, 0x2217, //SEQ_DATA_PORT
	0x3086, 0x0527, //SEQ_DATA_PORT
	0x3086, 0x985D, //SEQ_DATA_PORT
	0x3086, 0x2645, //SEQ_DATA_PORT
	0x3086, 0x170E, //SEQ_DATA_PORT
	0x3086, 0x2800, //SEQ_DATA_PORT
	0x3086, 0x5301, //SEQ_DATA_PORT
	0x3086, 0x4B52, //SEQ_DATA_PORT
	0x3086, 0x5112, //SEQ_DATA_PORT
	0x3086, 0x4460, //SEQ_DATA_PORT
	0x3086, 0x2C2C, //SEQ_DATA_PORT
	0x3086, 0x2C2C, //SEQ_DATA_PORT
	#endif
		
	0x301A, 0x10D8, //RESET_REGISTER
	0x30B0, 0x0000, //DIGITAL_TEST
	0x30BA, 0x06EC, //DIGITAL_CTRL
	0x31D0, 0x0000, //COMPANDING
	0x31AC, 0x0A0A, //DATA_FORMAT_BITS
	0x31AE, 0x0301, //SERIAL_FORMAT

	0x3082, 0x0008, //OPERATION_MODE_CTRL
	0x305E, 0x0080, //GLOBAL_GAIN
	0x3190, 0x0000, //HDR_MC_CTRL4
	0x318C, 0xC001, //HDR_MC_CTRL2
	0x3198, 0x06F0, //HDR_MC_CTRL8
	0x30FE, 0x0000, //NOISE_PEDESTAL
	0x320A, 0x0000, //ADACD_PEDESTAL
	0x2410, 0x0010, //ALTM_POWER_GAIN
	0x2412, 0x0010, //ALTM_POWER_OFFSET
	0x2442, 0x0080, //ALTM_CONTROL_KEY_K0
	0x2444, 0x0000, //ALTM_CONTROL_KEY_K01_LO
	0x2446, 0x0004, //ALTM_CONTROL_KEY_K01_HI
	0x2440, 0x0040, //ALTM_CONTROL_DAMPER
	0x2450, 0x0000, //ALTM_OUT_PEDESTAL
	0x2438, 0x0010, //ALTM_CONTROL_MIN_FACTOR
	0x243A, 0x0020, //ALTM_CONTROL_MAX_FACTOR
	0x243C, 0x0000, //ALTM_CONTROL_DARK_FLOOR
	0x243E, 0x0200, //ALTM_CONTROL_BRIGHT_FLOOR
	0x31D0, 0x0000, //COMPANDING
	0x301E, 0x0010, //DATA_PEDESTAL
	0x2400, 0x0002, //ALTM_CONTROL
	0x3202, 0x0200, //ADACD_NOISE_MODEL1
	0x3206, 0x0A06, //ADACD_NOISE_FLOOR1
	0x3208, 0x1A12, //ADACD_NOISE_FLOOR2
	0x3200, 0x0002, //ADACD_CONTROL
	0x31E0, 0x0200, //PIX_DEF_ID
	0x3060, 0x0006, //ANALOG_GAIN
	0x318A, 0x0E10, //HDR_MC_CTRL1
	0x3064, 0x1802, //SMIA_TEST
	0x2440, 0x0040, //ALTM_CONTROL_DAMPER
	0x2420, 0x0007, //REDUCE SHARPNESS

	0x306E, 0x9010, //

	0x3180, 0x8089, //DELTA_DK_CONTROL
	0x30F4, 0x4000, //ADC_COMMAND3_HS
	0x3ED4, 0x8F6C, //DAC_LD_8_9
	0x3ED6, 0x66CC, //DAC_LD_10_11
	0x3ED8, 0x8C42, //DAC_LD_12_13
	0x3EDA, 0x8899, //DAC_LD_14_15
	0x3EE6, 0x00F0, //DAC_LD_26_27

	0x3ED2, 0x1F46, //DAC_LD_6_7

	//24 /8 *198 /1 /8 = 74.25MHz
	0x302A, 0x0008, //VT_PIX_CLK_DIV
	0x302C, 0x0001, //VT_SYS_CLK_DIV
	0x302E, 0x0008, //PRE_PLL_CLK_DIV
	0x3030, 0x00C6, //PLL_MULTIPLIER
	0x3036, 0x000C, //OP_PIX_CLK_DIV
	0x3038, 0x0001, //OP_SYS_CLK_DIV
	#if 0
	0x3002, 0x00E4, //Y_ADDR_START//228
	0x3004, 0x0042, //X_ADDR_START//66
	0x3006, 0x0523, //Y_ADDR_END//1315=1087+228
	0x3008, 0x07C9, //X_ADDR_END//1993=1927+66
	0x300A, 0x0465, //FRAME_LENGTH_LINES//1125
	0x300C, 0x044C, //LINE_LENGTH_PCK//1100
	0x3012, 0x041E, //COARSE_INTEGRATION_TIME
	0x3014, 0x0000, //
	0x30A2, 0x0001, //X_ODD_INC
	0x30A6, 0x0001, //Y_ODD_INC
	0x3040, 0x0000, //READ_MODE

	0x301A, 0x10DC //RESET_REGISTER
	#endif
};

#if 0
void ____Sensor_Res_OPR_Table____(){ruturn;} //dummy
#endif

MMP_USHORT SNR_AR0331_Reg_1920x1080[] = 
{
#if 0
    0x301A, 0x10D8, //RESET_REGISTER

    0x3002, 0x00E4, //Y_ADDR_START//228
    0x3004, 0x0042, //X_ADDR_START//66
    0x3006, 0x0523, //Y_ADDR_END//1315=1087+228
    0x3008, 0x07C9, //X_ADDR_END//1993=1927+66
    0x300A, 0x0465, //FRAME_LENGTH_LINES//1125
    0x300C, 0x044C, //LINE_LENGTH_PCK//1100
    0x3012, 0x041E, //COARSE_INTEGRATION_TIME
    0x3014, 0x0000, //
    0x30A2, 0x0001, //X_ODD_INC
    0x30A6, 0x0001, //Y_ODD_INC
    0x3040, 0x0000, //READ_MODE

    0x301A, 0x10DC //RESET_REGISTER
#else

    0x301A, 0x10D8, //RESET_REGISTER

    0x3002, 0x00E4, //Y_ADDR_START//228
    0x3004, 0x0042, //X_ADDR_START//66
    0x3006, 0x0523, //Y_ADDR_END//1315=1087+228
    0x3008, 0x07C9, //X_ADDR_END//1993=1927+66
    0x300A, 0x0465, //FRAME_LENGTH_LINES//1125
    0x300C, 0x044C, //LINE_LENGTH_PCK//1100
    0x3012, 0x0465, //COARSE_INTEGRATION_TIME
    0x3014, 0x0000, //
    0x30A2, 0x0001, //X_ODD_INC
    0x30A6, 0x0001, //Y_ODD_INC
    0x3040, 0x0000, //READ_MODE

    0x301A, 0x10DC, //RESET_REGISTER
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
    SensorCustFunc.OprTable->usInitSize                   			= (sizeof(SNR_AR0331_Reg_Init_Customer)/sizeof(SNR_AR0331_Reg_Init_Customer[0]))/2;
    SensorCustFunc.OprTable->uspInitTable                 			= &SNR_AR0331_Reg_Init_Customer[0];

    SensorCustFunc.OprTable->bBinTableExist                         = MMP_FALSE;
    SensorCustFunc.OprTable->bInitDoneTableExist                    = MMP_FALSE;

    SensorCustFunc.OprTable->usSize[RES_IDX_1920x1080_30FPS]    	= (sizeof(SNR_AR0331_Reg_1920x1080)/sizeof(SNR_AR0331_Reg_1920x1080[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1920x1080_30FPS]  	= &SNR_AR0331_Reg_1920x1080[0];

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
    SensorCustFunc.VifSetting->powerOnSetting.bFirstEnPinHigh       = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubFirstEnPinDelay     = 10;
    SensorCustFunc.VifSetting->powerOnSetting.bNextEnPinHigh        = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubNextEnPinDelay      = 100;
    SensorCustFunc.VifSetting->powerOnSetting.bTurnOnClockBeforeRst = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.bFirstRstPinHigh      = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOnSetting.ubFirstRstPinDelay    = 100;
    SensorCustFunc.VifSetting->powerOnSetting.bNextRstPinHigh       = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubNextRstPinDelay     = 100;
    
    // Init Vif Setting : PowerOff Setting
    SensorCustFunc.VifSetting->powerOffSetting.bEnterStandByMode    = MMP_FALSE;
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
    SensorCustFunc.VifSetting->colorId.VifColorId              	    = MMPF_VIF_COLORID_01;
	SensorCustFunc.VifSetting->colorId.CustomColorId.bUseCustomId   = MMP_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_DoAE_FrmSt
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_DoAE_FrmSt(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
#if 0
    ISP_IF_F_SetWDR(60);
    
    switch (ulFrameCnt % 6) {
        case 0:
            #if (ISP_EN)
            ISP_IF_AE_Execute();
            gsSensorFunction->MMPF_Sensor_SetShutter(ubSnrSel, 0, 0);
            gsSensorFunction->MMPF_Sensor_SetGain(ubSnrSel, ISP_IF_AE_GetGain());
            #endif
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
#if 0
    ISP_UINT16 sensor_again;
    ISP_UINT16 sensor_dgain;

    ulGain = ulGain * 79 / ISP_IF_AE_GetGainBase();  //0x40 = 1x , for sensor min_gain limit 1.23x 79/64 = 1.235

    // Sensor Gain Mapping
    if(ulGain < 0x80){
        // 1X ~ 2X (DG*1.23AG)
        sensor_dgain = ulGain*0x80 / 79;
        sensor_again = 0x06;
    }

    else if(ulGain < 0x100){
        // 2X ~ 4X (AG=2x)
        sensor_dgain = ulGain;
        sensor_again = 0x10;
    }

    else if(ulGain < 0x200){
        // 4X ~ 8X (AG=4x)
        sensor_dgain = ulGain >> 1;
        sensor_again = 0x20;
    }

    else{
        // 8X ~16X (AG=8x)
        sensor_dgain = ulGain >> 2;
        sensor_again = 0x30;
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
#if 0
    ISP_UINT32 new_vsync    = gSnrLineCntPerSec[ubSnrSel] * ISP_IF_AE_GetVsync() / ISP_IF_AE_GetVsyncBase();
    ISP_UINT32 new_shutter  = gSnrLineCntPerSec[ubSnrSel] * ISP_IF_AE_GetShutter() / ISP_IF_AE_GetShutterBase();

    // 1 ~ (frame length line - 6)
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

#endif // (BIND_SENSOR_AR0331)
#endif // (SENSOR_EN)

//==============================================================================
//
//  File        : sensor_imx214.c
//  Description : Firmware Sensor Control File
//  Author      : Eric zhang
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
#if (BIND_SENSOR_IMX214)

#include "mmpf_sensor.h"
#include "Sensor_Mod_Remapping.h"
#include "isp_if.h"
#include "hdr_cfg.h"

//==============================================================================
//
//                              GLOBAL VARIABLE
//
//==============================================================================
MMPF_SENSOR_RESOLUTION m_SensorRes = 
{
    14,             /* ubSensorModeNum */
    11,             /* ubDefPreviewMode */
    11,             /* ubDefCaptureMode */
    6000,          // usPixelSize                                                                                            // usPixelSize (TBD)
// Mode0    Mode1   Mode2   Mode3   Mode4   Mode5   Mode6   Mode7   Mode8   Mode9   Mode10      Mode11   Mode12   Mode13
    {1,       1,      1,      1,      1,      1,      88,     1,      1,      4,      4,              1,        1,      1}, // usVifGrabStX
    {1,       1,      1,      1,      1,      1,     232,      1,      1,     16,      16,        1,        1,      1}, // usVifGrabStY
    {4216,    4208,   3848,  2056,   2112,   2112,  1928,  2104,   1288,   1288,  1288,       4216,     2048,   1928}, // usVifGrabW
    {3136,    3120,   2176,  1160,   1576,   1576,  1096,  1560,   724,    724,    724,       3136,     1536,   1096}, // usVifGrabH
#if (CHIP == MCR_V2)
    {1,         1,      1,      1,      1,      1,      1,      1,      1,      1,      1,          1,      1,      1}, // usBayerInGrabX
    {1,         1,      1,      1,      1,      1,      1,      1,      1,      1,      1,          1,      1,      1}, // usBayerInGrabY
    {8,       8,       8,      8,      8,      8,      8,     8,      8,      8,      8,           8,       8,      8}, // usBayerDummyInX
    {16,      16,      16,     8,     16,     16,     16,    16,     4,      4,     4,          8,      4,      16}, // usBayerDummyInY
    {4208,    4200,    3840,  2048,   1920,   1920,  1920,  1440,   1280,   1280,  1280,       4208,    2040,   1920}, // usBayerOutW
    {3120,    3104,    2160,  1152,   1080,   1080,  1080,  1080,    720,   720,   720,        3120,    1532,   1080}, // usBayerOutH
#endif
    {4208,    4200,    3840,   2048,   1920,   1920,   1920,   1440,   1280,   1280,    1280,   4208,   2040,   1920}, // usScalInputW
    {3120,    3104,    2160,   1152,   1080,   1080,   1080,    1080,    720,    720,    720,   3120,   1532,   1080}, // usScalInputH
    {250,     100,     300,    300,    300,    500,    300,    300,   1000,   600,    300,      100,    300,    600}, // usTargetFpsx10
    {3194,    3194,    2746,  2746,   1648,   1648,   1618,  1648,   798,    798,   798,        2746,   1720,   1648}, // usVsyncLine
    {1,         1,      1,      1,      1,      1,      1,      1,      1,      1,      1,          1,      1,      1}, // ubWBinningN
    {1,         1,      1,      1,      1,      1,      1,      1,      1,      1,      1,          1,      1,      1}, // ubWBinningM
    {1,         1,      1,      1,      1,      1,      1,      1,      1,      1,      1,          1,      1,      1}, // ubHBinningN
    {1,         1,      1,      1,      1,      1,      1,      1,      1,      1,      1,          1,      1,      1},     // ubHBinningM
    {0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,      0xFF}, // ubCustIQmode
    {0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,      0xFF}  // ubCustAEmode
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
static MMP_I2CM_ATTR m_I2cmAttr = 
{
    MMP_I2CM0,  // i2cmID
    0x1A,       // ubSlaveAddr
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
    6,  /* ubPeriod */
    1,  /* ubDoAWBFrmCnt */
    3   /* ubDoCaliFrmCnt */
};

MMPF_SENSOR_AETIMIMG    m_AeTime     = 
{   
    6,  /* ubPeriod */
    0,  /* ubFrmStSetShutFrmCnt */
    0   /* ubFrmStSetGainFrmCnt */
};

MMPF_SENSOR_AFTIMIMG    m_AfTime     = 
{
    1,  /* ubPeriod */
    0   /* ubDoAFFrmCnt */
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

MMP_USHORT SNR_IMX214_Reg_Init_Customer[] = 
{
    SENSOR_DELAY_REG, 100
};

#elif (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)

ISP_UINT16 SNR_IMX214_Reg_Unsupport[] = 
{
    SENSOR_DELAY_REG, 100 // delay
};

MMP_USHORT SNR_IMX214_Reg_Init_Customer[] = 
{
#if 1
    0x0136,0x18,
    0x0137,0x00,

    0x0101,0x00,
    0x0105,0x01,
    0x0106,0x01,
    0x4601,0x00,
    0x4642,0x05,
    0x6276,0x00,
    0x900E,0x06,
    0xA802,0x90,
    0xA803,0x11,
    0xA804,0x62,
    0xA805,0x77,
    0xA806,0xAE,
    0xA807,0x34,
    0xA808,0xAE,
    0xA809,0x35,
    0xA80A,0x62,
    0xA80B,0x83,
    0xAE33,0x00,

    0x4174,0x00,
    0x4175,0x11,
    0x4612,0x29,
    0x461B,0x12,
    0x461F,0x06,
    0x4635,0x07,
    0x4637,0x30,
    0x463F,0x18,
    0x4641,0x0D,
    0x465B,0x12,
    0x465F,0x11,
    0x4663,0x11,
    0x4667,0x0F,
    0x466F,0x0F,
    0x470E,0x09,
    0x4909,0xAB,
    0x490B,0x95,
    0x4915,0x5D,
    0x4A5F,0xFF,
    0x4A61,0xFF,
    0x4A73,0x62,
    0x4A85,0x00,
    0x4A87,0xFF,
    0x583C,0x04,
    0x620E,0x04,
    0x6EB2,0x01,
    0x6EB3,0x00,
    0x9300,0x02,

    0x3001,0x07,
    0x6D12,0x3F,
    0x6D13,0xFF,
    0x9344,0x03,
    0x9706,0x10,
    0x9707,0x03,
    0x9708,0x03,
    0x9E04,0x01,
    0x9E05,0x00,
    0x9E0C,0x01,
    0x9E0D,0x02,
    0x9E24,0x00,
    0x9E25,0x8C,
    0x9E26,0x00,
    0x9E27,0x94,
    0x9E28,0x00,
    0x9E29,0x96,
    //0x5041,0x00,//no embedded data 

    0x69DB,0x01,
    0x6957,0x01,
    0x6987,0x17,
    0x698A,0x03,
    0x698B,0x03,
    0x0B8E,0x01,
    0x0B8F,0x00,
    0x0B90,0x01,
    0x0B91,0x00,
    0x0B92,0x01,
    0x0B93,0x00,
    0x0B94,0x01,
    0x0B95,0x00,
    0x6E50,0x00,
    0x6E51,0x32,
    0x9340,0x00,
    0x9341,0x3C,
    0x9342,0x03,
    0x9343,0xFF,
#else
    //0x0100, 0x0,
    //external clock setting    
    0x0136,  0x09, ///exck_freq
    0x0137, 0x09,///exck_freq
    //global setting
    0x0105, 0x01,
    0x0106, 0x01,
    
    0x6276, 0x00,
    0xA802, 0x90,
    0xA803, 0x11,
    0xA804, 0x62,
    0xA805, 0x77,
    0xA806, 0xAE,
    0xA807, 0x34,
    0xA808, 0xAE,
    0xA809, 0x35,
    0xA80A, 0x62,
    0xA80B, 0x83,
    0xAE33, 0x00,
    
    0x4175, 0x11,
    0x4612, 0x29, 
    0x461B, 0x12,
    0x461F, 0x06,
    0x4635, 0x07,
    0x4637, 0x30,
    0x463F, 0x18,
    0x4641, 0x0D,
    0x465B, 0x12,
    0x465F, 0x11,
    0x4663, 0x11,
    0x4667, 0x0F,
    0x466F, 0x0F,
    0x470E, 0x09,
    0x4909, 0xAB,
    0x490B, 0x95,
    0x4915, 0x5D,
    0x4A5F, 0xFF,
    0x4A61, 0xFF,
    0x4A73, 0x62,
    0x4A85, 0x00,
    0x4A87, 0xFF,
    
    0x620E, 0x04,
    0x6EB2, 0x01,

    0x9300, 0x02,

    //0x0100, 0x1,
#endif
};
#endif

#if 0
void ____Sensor_Res_OPR_Table____(){ruturn;} //dummy
#endif

#if (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)


#elif (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)      

ISP_UINT16 SNR_IMX214_Reg_4208x3120_25P[] = 
{
#if 1
    0x0100, 0x00,
    ///mode setting 
    0x0114, 0x03,      //csi lane mode
    0x0220, 0x00,      //hdr mode
    0x0221, 0x11,       //[7:4]hdr reso redu h [3:0]hdr reso redu v 
    0x0222, 0x01,   //expo ratio
    0x0340, 0x0C,     //frm length lines
    0x0341, 0x7A,     //frm length lines
    0x0342, 0x13,       //line length pck
    0x0343, 0x90,       //line length pck
    0x0344, 0x00,       //x add sta
    0x0345, 0x00,       //x add sta
    0x0346, 0x00,       //y add sta
    0x0347, 0x00,       //y add sta
    0x0348, 0x10,       //x add end
    0x0349, 0x6F,       //x add end
    0x034A, 0x0C,       //y add end
    0x034B, 0x2F,       //y add end
    0x0381, 0x01,       //x evn inc
    0x0383, 0x01,       //x odd inc
    0x0385, 0x01,       //y evn inc
    0x0387, 0x01,       //y odd inc
    0x0900, 0x00,       //binning mode
    0x0901, 0x00,       //[7:4]binning type h [3:0]binning type v
    0x0902, 0x00,       //binning weighting
    0x3000, 0x35,       //hdr output ctrl
    0x3054, 0x01,       
    0x305C, 0x11,
    //output size setting
    0x0112, 0x0A,       //csi dt fmt h
    0x0113, 0x0A,       //csi st fmt l
    0x034C, 0x10,       //x out size
    0x034D, 0x70,       //x out size
    0x034E, 0x0C,       //y out size
    0x034F, 0x30,       //y out size
    0x0401, 0x00,       //scale mode
    0x0404, 0x00,       //scale m
    0x0405, 0x10,       //scale m
    0x0408, 0x00,       //dig crop x offset
    0x0409, 0x00,       //dig crop x offset
    0x040A, 0x00,       //dig crop y offset
    0x040B, 0x00,       //dig crop y offset
    0x040C, 0x10,       //dig crop image width
    0x040D, 0x70,       //dig crop image width
    0x040E, 0x0C,       //dig crop image height
    0x040F, 0x30,       //dig crop image height
    //clock setting
    0x0301, 0x05,       //vtpxck div
    0x0303, 0x02,       //vtsyck div
    0x0305, 0x03,       //prepllck vt div
    0x0306, 0x00,       //pll vt mpy
    0x0307, 0x4C,       //pll vt mpy
    0x0309, 0x0A,       //oppxck div
    0x030B, 0x01,       //opsyck div
    0x0310, 0x00,       //pll mult drv
    //data rate setting
    0x0820, 0x09,       //req link bit rate Mbps
    0x0821, 0x80,
    0x0822, 0x00,
    0x0823, 0x00,
    //water mark setting
    0x3A03, 0x08,
    0x3A04, 0xC0,
    0x3A05, 0x02,
    //enable setting
    0x0B06, 0x01,
    0x30A2, 0x00,
    //hdr setting
    0x3A02, 0xFF,
    //stats setting
    0x3013, 0x00,
    //integration time setting
    0x0202, 0x0C,
    0x0203, 0x78,
    0x0224, 0x01,
    0x0225, 0xF4,
    //analog setting
    0x4170, 0x00,
    0x4171, 0x10,
    0x4176, 0x00,
    0x4177, 0x3C,
    0xAE20, 0x04,
    0xAE21, 0x5C,

    0x0100, 0x01///streaming
#else
    0x0100, 0x00,
    ///mode setting 
    0x0114, 0x03,      //csi lane mode
    0x0220, 0x00,      //hdr mode
    0x0221, 0x11,       //[7:4]hdr reso redu h [3:0]hdr reso redu v 
    0x0222, 0x01,   //expo ratio
    0x0340, 0x0C,     //frm length lines
    0x0341, 0x7A,     //frm length lines
    0x0342, 0x13,       //line length pck
    0x0343, 0x90,       //line length pck
    0x0344, 0x00,       //x add sta
    0x0345, 0x00,       //x add sta
    0x0346, 0x00,       //y add sta
    0x0347, 0x00,       //y add sta
    0x0348, 0x10,       //x add end
    0x0349, 0x6F,       //x add end
    0x034A, 0x0C,       //y add end
    0x034B, 0x2F,       //y add end
    0x0381, 0x01,       //x evn inc
    0x0383, 0x01,       //x odd inc
    0x0385, 0x01,       //y evn inc
    0x0387, 0x01,       //y odd inc
    0x0900, 0x00,       //binning mode
    0x0901, 0x00,       //[7:4]binning type h [3:0]binning type v
    0x0902, 0x00,       //binning weighting
    0x3000, 0x35,       //hdr output ctrl
    0x3054, 0x01,       
    0x305C, 0x11,
    //output size setting
    0x0112, 0x0A,       //csi dt fmt h
    0x0113, 0x0A,       //csi st fmt l
    0x034C, 0x10,       //x out size
    0x034D, 0x70,       //x out size
    0x034E, 0x0C,       //y out size
    0x034F, 0x30,       //y out size
    0x0401, 0x00,       //scale mode
    0x0404, 0x00,       //scale m
    0x0405, 0x10,       //scale m
    0x0408, 0x00,       //dig crop x offset
    0x0409, 0x00,       //dig crop x offset
    0x040A, 0x00,       //dig crop y offset
    0x040B, 0x00,       //dig crop y offset
    0x040C, 0x10,       //dig crop image width
    0x040D, 0x70,       //dig crop image width
    0x040E, 0x0C,       //dig crop image height
    0x040F, 0x30,       //dig crop image height
    //clock setting
    0x0301, 0x05,       //vtpxck div
    0x0303, 0x02,       //vtsyck div
    0x0305, 0x04,       //prepllck vt div
    0x0306, 0x00,       //pll vt mpy
    0x0307, 0xAC,       //pll vt mpy
    0x0309, 0x0A,       //oppxck div
    0x030B, 0x01,       //opsyck div
    0x0310, 0x00,       //pll mult drv
    //data rate setting
    0x0820, 0x10,       //req link bit rate Mbps
    0x0821, 0x20,
    0x0822, 0x00,
    0x0823, 0x00,
    //water mark setting
    0x3A03, 0x08,
    0x3A04, 0xC0,
    0x3A05, 0x02,
    //enable setting
    0x0B06, 0x01,
    0x30A2, 0x00,
    //hdr setting
    0x3A02, 0xFF,
    //stats setting
    0x3013, 0x00,
    //integration time setting
    0x0202, 0x0C,
    0x0203, 0x70,
    0x0224, 0x01,
    0x0225, 0xF4,
    //analog setting
    0x4170, 0x00,
    0x4171, 0x10,
    0x4176, 0x00,
    0x4177, 0x3C,
    0xAE20, 0x04,
    0xAE21, 0x5C,

    0x0100, 0x01///streaming
 #endif     
};
///no ready
ISP_UINT16 SNR_IMX214_Reg_4208x3120_25P_HDR[] = 
{
    0x0100, 0x00,
    ///mode setting 
    0x0114, 0x03,      //csi lane mode
    0x0220, 0x01,      //hdr mode
    0x0221, 0x11,       //[7:4]hdr reso redu h [3:0]hdr reso redu v 
    0x0222, 0x10,   //expo ratio
    0x0340, 0x0C,     //frm length lines
    0x0341, 0x7A,     //frm length lines
    0x0342, 0x13,       //line length pck
    0x0343, 0x90,       //line length pck
    0x0344, 0x00,       //x add sta
    0x0345, 0x00,       //x add sta
    0x0346, 0x00,       //y add sta
    0x0347, 0x00,       //y add sta
    0x0348, 0x10,       //x add end
    0x0349, 0x6F,       //x add end
    0x034A, 0x0C,       //y add end
    0x034B, 0x2F,       //y add end
    0x0381, 0x01,       //x evn inc
    0x0383, 0x01,       //x odd inc
    0x0385, 0x01,       //y evn inc
    0x0387, 0x01,       //y odd inc
    0x0900, 0x00,       //binning mode
    0x0901, 0x00,       //[7:4]binning type h [3:0]binning type v
    0x0902, 0x00,       //binning weighting
    0x3000, 0x35,       //hdr output ctrl
    0x3054, 0x00,       
    0x305C, 0x11,
    //output size setting
    0x0112, 0x0A,       //csi dt fmt h
    0x0113, 0x0A,       //csi st fmt l
    0x034C, 0x10,       //x out size
    0x034D, 0x70,       //x out size
    0x034E, 0x0C,       //y out size
    0x034F, 0x30,       //y out size
    0x0401, 0x00,       //scale mode
    0x0404, 0x00,       //scale m
    0x0405, 0x10,       //scale m
    0x0408, 0x00,       //dig crop x offset
    0x0409, 0x00,       //dig crop x offset
    0x040A, 0x00,       //dig crop y offset
    0x040B, 0x00,       //dig crop y offset
    0x040C, 0x10,       //dig crop image width
    0x040D, 0x70,       //dig crop image width
    0x040E, 0x0C,       //dig crop image height
    0x040F, 0x30,       //dig crop image height
    //clock setting
    0x0301, 0x05,       //vtpxck div
    0x0303, 0x02,       //vtsyck div
    0x0305, 0x04,       //prepllck vt div
    0x0306, 0x00,       //pll vt mpy
    0x0307, 0xAC,       //pll vt mpy
    0x0309, 0x0A,       //oppxck div
    0x030B, 0x01,       //opsyck div
    0x0310, 0x00,       //pll mult drv
    //data rate setting
    0x0820, 0x10,       //req link bit rate Mbps
    0x0821, 0x20,
    0x0822, 0x00,
    0x0823, 0x00,
    //water mark setting
    0x3A03, 0x08,
    0x3A04, 0xC0,
    0x3A05, 0x01,
    //enable setting
    0x0B06, 0x01,
    0x30A2, 0x00,
    //hdr setting
    0x3A02, 0x06,
    //stats setting
    0x3013, 0x01,
    //integration time setting
    0x0202, 0x0C,
    0x0203, 0x70,
    0x0224, 0x01,
    0x0225, 0x8E,

    //analog setting
    0x4170, 0x00,
    0x4171, 0x10,
    0x4176, 0x00,
    0x4177, 0x3C,
    0xAE20, 0x04,
    0xAE21, 0x5C,

    0x0100, 0x01///streaming
};

ISP_UINT16 SNR_IMX214_Reg_4208x3120_10P[] = 
{
    //----------------------------------------------
    // MCLK: 24Mhz
    // Mipi : 4 lane 
    // Mipi data rate:Mbps/Lane
    // SystemCLK   :120MHz
    // FPS          :10 ( = SystemCLK / (HTS*VTS) ) 
    // HTS      :1280(R380c:R380d)                    
    // VTS      :1562(R380e:R380f) 
    // Tline    :    
    //---------------------------------------------
    0x0100, 0x00,
    ///mode setting 
    0x0114, 0x03,      //csi lane mode
#if 0   
    // jeff 屏蔽
    0x0220, 0x01,      //hdr mode
#else
    0x0220, 0x00,      //disable hdr mode
#endif
    0x0221, 0x11,       //[7:4]hdr reso redu h [3:0]hdr reso redu v  BINNING_TYPE_H BINNING_TYPE_V
    0x0222, 0x10,   //expo ratio
    0x0340, 0x0C,     //frm length lines
    0x0341, 0x7A,     //frm length lines
    0x0342, 0x0A,       //line length pck
    0x0343, 0xBA,       //line length pck
    
    0x0344, 0x00,       //x add start
    0x0345, 0x00,       //x add start
    0x0346, 0x00,       //y add start
    0x0347, 0x00,       //y add start
    0x0348, 0x10,       //x add end   4207 -  106F
    0x0349, 0x6F,       //x add end
    0x034A, 0x0C,       //y add end   3119  - C2F
    0x034B, 0x2F,       //y add end
    0x0381, 0x01,       //x evn inc
    0x0383, 0x01,       //x odd inc
    0x0385, 0x01,       //y evn inc
    0x0387, 0x01,       //y odd inc
    0x0900, 0x00,       //binning mode
    0x0901, 0x00,       //[7:4]binning type h [3:0]binning type v
    0x0902, 0x00,       //binning weighting
    0x3000, 0x35,       //hdr output ctrl
    0x3054, 0x00,       
    0x305C, 0x11,
    //output size setting
    0x0112, 0x0A,       //CSI_DT_FMT_H    CSI Data Format  RAW10 
    0x0113, 0x0A,       //CSI_DT_FMT_L
    
    0x034C, 0x10,       //x out size  4208 - 1070
    0x034D, 0x70,       //x out size
    0x034E, 0x0C,       //y out size  3120 - C30
    0x034F, 0x30,       //y out size
    0x0401, 0x00,       //scale mode
    0x0404, 0x00,       //scale m
    0x0405, 0x10,       //scale m
    0x0408, 0x00,       //dig crop x offset
    0x0409, 0x00,       //dig crop x offset
    0x040A, 0x00,       //dig crop y offset
    0x040B, 0x00,       //dig crop y offset
    0x040C, 0x10,       //dig crop image width  4208 - 1070
    0x040D, 0x70,       //dig crop image width
    0x040E, 0x0C,       //dig crop image height 3120 - C30
    0x040F, 0x30,       //dig crop image height
    //clock setting
    0x0301, 0x05,       //vtpxck div
    0x0303, 0x02,       //vtsyck div
    0x0305, 0x0F,       //prepllck vt div
#if 0   
    0x0306, 0x00,       //pll vt mpy
    0x0307, 0xAC,       //pll vt mpy
#else
    0x0306, 0x02,       //pll vt mpy
    0x0307, 0xEE,       //pll vt mpy
#endif
    0x0309, 0x0A,       //oppxck div
    0x030B, 0x01,       //opsyck div
    0x0310, 0x00,       //pll mult drv 0 (dec) : Single PLL
    0x0808, 0x01,      // jeff add 2015-5-12
    //data rate   REQ_LINK_BIT_RATE_MBPS
    0x0820, 0x00,       
    0x0821, 0xC8,
    0x0822, 0x55,
    0x0823, 0x00,
    //water mark setting
    0x3A03, 0x08,
    0x3A04, 0xC0,
    0x3A05, 0x01,
    //enable setting
    0x0B06, 0x00,
    0x30A2, 0x00,
#if 0   
    //hdr setting
    0x3A02, 0x06,
#else
    0x3A02, 0x06,
#endif
    //stats setting
    0x3013, 0x00,
    //integration time setting
    0x0202, 0x0C,
    0x0203, 0x70,
    0x0224, 0x01,
    0x0225, 0x8E,

    //analog setting
    0x4170, 0x00,
    0x4171, 0x10,
    0x4176, 0x00,
    0x4177, 0x3C,
    0xAE20, 0x04,
    0xAE21, 0x5C,

#if 0
    ///mode setting 
    0x0114, 0x03,      //csi lane mode
    0x0220, 0x00,      //hdr mode disable
    0x0221, 0x11,       //[7:4]hdr reso redu h [3:0]hdr reso redu v 
    0x0222, 0x10,   //expo ratio
    0x0340, 0x0C,     //frm length lines  3120
    0x0341, 0x30,     //frm length lines
    0x0342, 0x10,       //line length pck  4208
    0x0343, 0x70,       //line length pck
    0x0344, 0x00,       //x add start
    0x0345, 0x00,       //x add start
    0x0346, 0x00,       //y add start
    0x0347, 0x00,       //y add start
    0x0348, 0x0C,       //x add end
    0x0349, 0x30,       //x add end
    0x034A, 0x0F,       //y add end
    0x034B, 0xA0,       //y add end
    0x0381, 0x01,       //x evn inc
    0x0383, 0x01,       //x odd inc
    0x0385, 0x01,       //y evn inc
    0x0387, 0x01,       //y odd inc
    0x0900, 0x00,       //binning mode disable
    0x0901, 0x00,       //[7:4]binning type h [3:0]binning type v
    0x0902, 0x03,       //binning weighting
    0x3000, 0x35,       //hdr output ctrl ????手册未开放
    0x3054, 0x00,       //  ????
    0x305C, 0x11,       // ?????
    //output size setting
    0x0112, 0x0A,       //csi dt fmt h
    0x0113, 0x0A,       //csi st fmt l
    0x034C, 0x0F,       //x out size  4000
    0x034D, 0xA0,       //x out size
    0x034E, 0x0A,       //y out size  2800
    0x034F, 0xF0,       //y out size
    0x0401, 0x00,       //scale mode  0:no scaling
    0x0404, 0x00,       //scale mode
    0x0405, 0x10,       //scale mode
    0x0408, 0x00,       //dig crop x offset
    0x0409, 0x00,       //dig crop x offset
    0x040A, 0x00,       //dig crop y offset
    0x040B, 0x00,       //dig crop y offset
    0x040C, 0x0F,       //dig crop image width    4000
    0x040D, 0xA0,       //dig crop image width
    0x040E, 0x0A,       //dig crop image height   2800
    0x040F, 0xF0,       //dig crop image height
    //clock setting
    0x0301, 0x05,       //vtpxck div
    0x0303, 0x02,       //vtsyck div
    0x0305, 0x03,       //prepllck vt div
    0x0306, 0x00,       //pll vt mpy
    0x0307, 0x4C,       //pll vt mpy
    0x0309, 0x0A,       //oppxck div
    0x030B, 0x01,       //opsyck div
    0x0310, 0x00,       //pll mult drv
    //data rate setting
    0x0820, 0x09,       //req link bit rate Mbps
    0x0821, 0x80,
    0x0822, 0x00,
    0x0823, 0x00,
    //data rate setting
#if 0   
    0x0820, 0x10,       //req link bit rate Mbps
    0x0821, 0x20,
    0x0822, 0x00,
    0x0823, 0x00,  7BFA480
#else
    0x0820, 0x07,       //req link bit rate Mbps
    0x0821, 0xBF,
    0x0822, 0xA4,
    0x0823, 0x80,
#endif
    //water mark setting
    0x3A03, 0x08,
    0x3A04, 0xC0,
    0x3A05, 0x01,
    // correction enable setting
    0x0B06, 0x01,
    0x30A2, 0x00,
    //hdr setting   ????  未公开
    0x3A02, 0x06,
    //stats setting
    0x3013, 0x00,
    //integration time setting
    0x0202, 0x0C,
    0x0203, 0x70,
    0x0224, 0x01,    // 2800
    0x0225, 0x8E,

    // gain set
    0x0204,0x00,                    
    0x0205,0x00,                       
    0x020E,0x01,                      
    0x020F,0x00,                      
    0x0210,0x01,                      
    0x0211,0x00,                      
    0x0212,0x01,                       
    0x0213,0x00,                      
    0x0214,0x01,                       
    0x0215,0x00,                       
    0x0216,0x00,                       
    0x0217,0x00, 

    //analog setting
    0x4170, 0x00,
    0x4171, 0x10,
    0x4176, 0x00,
    0x4177, 0x3C,
    0xAE20, 0x04,
    0xAE21, 0x5C,
#else
#endif
    0x0100, 0x01///streaming
};


ISP_UINT16 SNR_IMX214_Reg_3840x2160_30P[] = 
{
    0x0100, 0x00,
    ///mode setting 
    0x0114, 0x03,      //csi lane mode
    0x0220, 0x00,      //hdr mode
    0x0221, 0x11,       //[7:4]hdr reso redu h [3:0]hdr reso redu v 
    0x0222, 0x01,   //expo ratio
    0x0340, 0x0A,     //frm length lines
    0x0341, 0xBA,     //frm length lines
    0x0342, 0x13,       //line length pck
    0x0343, 0x90,       //line length pck
    0x0344, 0x00,       //x add sta
    0x0345, 0xB8,       //x add sta
    0x0346, 0x01,       //y add sta
    0x0347, 0xE0,       //y add sta
    0x0348, 0x0F,       //x add end
    0x0349, 0xB7,       //x add end
    0x034A, 0x0A,       //y add end
    0x034B, 0x4F,       //y add end
    0x0381, 0x01,       //x evn inc
    0x0383, 0x01,       //x odd inc
    0x0385, 0x01,       //y evn inc
    0x0387, 0x01,       //y odd inc
    0x0900, 0x00,       //binning mode
    0x0901, 0x00,       //[7:4]binning type h [3:0]binning type v
    0x0902, 0x00,       //binning weighting
    0x3000, 0x35,       //hdr output ctrl
    0x3054, 0x01,       
    0x305C, 0x11,
    //output size setting
    0x0112, 0x0A,       //csi dt fmt h
    0x0113, 0x0A,       //csi st fmt l
    0x034C, 0x0F,       //x out size
    0x034D, 0x00,       //x out size
    0x034E, 0x08,       //y out size
    0x034F, 0x70,       //y out size
    0x0401, 0x00,       //scale mode
    0x0404, 0x00,       //scale m
    0x0405, 0x10,       //scale m
    0x0408, 0x00,       //dig crop x offset
    0x0409, 0x00,       //dig crop x offset
    0x040A, 0x00,       //dig crop y offset
    0x040B, 0x00,       //dig crop y offset
    0x040C, 0x0F,       //dig crop image width
    0x040D, 0x00,       //dig crop image width
    0x040E, 0x08,       //dig crop image height
    0x040F, 0x70,       //dig crop image height
    //clock setting
    0x0301, 0x05,       //vtpxck div
    0x0303, 0x02,       //vtsyck div
    0x0305, 0x03,       //prepllck vt div  03
    0x0306, 0x00,       //pll vt mpy
    0x0307, 0x63,       //pll vt mpy  720Mbps  63/5C/4C
    0x0309, 0x0A,       //oppxck div
    0x030B, 0x01,       //opsyck div
    0x0310, 0x00,       //pll mult drv
    //data rate setting
    0x0820, 0x09,       //req link bit rate Mbps
    0x0821, 0xF0,
    0x0822, 0x00,
    0x0823, 0x00,
    //water mark setting
    0x3A03, 0x08,
    0x3A04, 0xC0,
    0x3A05, 0x00,
    //enable setting
    0x0B06, 0x01,
    0x30A2, 0x00,

    0x30B4,0x00,
    //hdr setting
    0x3A02, 0xFF,
    //stats setting
    0x3013, 0x00,
    //integration time setting
    0x0202, 0x0A,
    0x0203, 0xB0,
    0x0224, 0x01,
    0x0225, 0xF4,

    ////
    0x0204,0x00,                    
    0x0205,0x00,                       
    0x020E,0x01,                      
    0x020F,0x00,                      
    0x0210,0x01,                      
    0x0211,0x00,                      
    0x0212,0x01,                       
    0x0213,0x00,                      
    0x0214,0x01,                       
    0x0215,0x00,                       
    0x0216,0x00,                       
    0x0217,0x00, 
    //analog setting
    0x4170, 0x00,
    0x4171, 0x10,
    0x4176, 0x00,
    0x4177, 0x3C,
    0xAE20, 0x04,
    0xAE21, 0x5C,
    
    0x0138, 0x01,
    0x0100, 0x01///streaming
};


ISP_UINT16 SNR_IMX214_Reg_3840x2160_30P_HDR[] = 
{
    0x0100, 0x00,
    ///mode setting 
    0x0114, 0x03,      //csi lane mode
    0x0220, 0x01,      //hdr mode
    0x0221, 0x11,       //[7:4]hdr reso redu h [3:0]hdr reso redu v 
    0x0222, 0x10,   //expo ratio
    0x0340, 0x0A,     //frm length lines
    0x0341, 0xBA,     //frm length lines
    0x0342, 0x13,       //line length pck
    0x0343, 0x90,       //line length pck
    0x0344, 0x00,       //x add sta
    0x0345, 0xB8,       //x add sta
    0x0346, 0x01,       //y add sta
    0x0347, 0xE0,       //y add sta
    0x0348, 0x0F,       //x add end
    0x0349, 0xB7,       //x add end
    0x034A, 0x0A,       //y add end
    0x034B, 0x4F,       //y add end
    0x0381, 0x01,       //x evn inc
    0x0383, 0x01,       //x odd inc
    0x0385, 0x01,       //y evn inc
    0x0387, 0x01,       //y odd inc
    0x0900, 0x00,       //binning mode
    0x0901, 0x00,       //[7:4]binning type h [3:0]binning type v
    0x0902, 0x00,       //binning weighting
    0x3000, 0x35,       //hdr output ctrl
    0x3054, 0x01,       
    0x305C, 0x11,
    //output size setting
    0x0112, 0x0A,       //csi dt fmt h
    0x0113, 0x0A,       //csi st fmt l
    0x034C, 0x0F,       //x out size
    0x034D, 0x00,       //x out size
    0x034E, 0x08,       //y out size
    0x034F, 0x70,       //y out size
    0x0401, 0x00,       //scale mode
    0x0404, 0x00,       //scale m
    0x0405, 0x10,       //scale m
    0x0408, 0x00,       //dig crop x offset
    0x0409, 0x00,       //dig crop x offset
    0x040A, 0x00,       //dig crop y offset
    0x040B, 0x00,       //dig crop y offset
    0x040C, 0x0F,       //dig crop image width
    0x040D, 0x00,       //dig crop image width
    0x040E, 0x08,       //dig crop image height
    0x040F, 0x70,       //dig crop image height
    //clock setting
    0x0301, 0x05,       //vtpxck div
    0x0303, 0x02,       //vtsyck div
    0x0305, 0x04,       //prepllck vt div
    0x0306, 0x00,       //pll vt mpy
    0x0307, 0xAC,       //pll vt mpy
    0x0309, 0x0A,       //oppxck div
    0x030B, 0x01,       //opsyck div
    0x0310, 0x00,       //pll mult drv
    //data rate setting
    0x0820, 0x10,       //req link bit rate Mbps
    0x0821, 0x20,
    0x0822, 0x00,
    0x0823, 0x00,
    //water mark setting
    0x3A03, 0x04,
    0x3A04, 0x58,
    0x3A05, 0x01,
    //enable setting
    0x0B06, 0x01,
    0x30A2, 0x00,
    //hdr setting
    0x3A02, 0x06,
    //stats setting
    0x3013, 0x01,
    //integration time setting
    0x0202, 0x0A,
    0x0203, 0xB0,
    0x0224, 0x01,
    0x0225, 0x56,
    //analog setting
    0x4170, 0x00,
    0x4171, 0x10,
    0x4176, 0x00,
    0x4177, 0x3C,
    0xAE20, 0x04,
    0xAE21, 0x5C,

    0x0100, 0x01///streaming
};

ISP_UINT16 SNR_IMX214_Reg_1920x1080_60P[] = 
{
    0x0100, 0x00,
    //image flip    
    0x0101,  0x00,
    ///mode setting     
    0x0114, 0x03,      //csi lane mode
    0x0220, 0x00,      //hdr mode
    0x0221, 0x11,       //[7:4]hdr reso redu h [3:0]hdr reso redu v 
    0x0222, 0x01,   //expo ratio
    0x0340, 0x06,     //frm length lines  1648
    0x0341, 0x70,     //frm length lines
    0x0342, 0x13,       //line length pck
    0x0343, 0x90,       //line length pck
    0x0344, 0x00,       //x add sta
    0x0345, 0x00,       //x add sta
    0x0346, 0x00,       //y add sta
    0x0347, 0x00,       //y add sta
    0x0348, 0x10,       //x add end   3856=F10
    0x0349, 0x6F,       //x add end
    0x034A, 0x0C,       //y add end   2192=890
    0x034B, 0x2F,       //y add end
    0x0381, 0x01,       //x evn inc
    0x0383, 0x01,       //x odd inc
    0x0385, 0x01,       //y evn inc
    0x0387, 0x01,       //y odd inc
    0x0900, 0x01,       //binning mode
    0x0901, 0x22,       //[7:4]binning type h [3:0]binning type v
    0x0902, 0x03,       //binning weighting
    0x3000, 0x35,       //hdr output ctrl
    0x3054, 0x01,       
    0x305C, 0x11,
    //output size setting
    0x0112, 0x0A,       //csi dt fmt h
    0x0113, 0x0A,       //csi st fmt l
    0x034C, 0x08,       //x out size   1928   
    0x034D, 0x38,       //x out size
    0x034E, 0x06,       //y out size   1096    
    0x034F, 0x18,       //y out size
    0x0401, 0x00,       //scale mode
    0x0404, 0x00,       //scale m
    0x0405, 0x10,       //scale m
#if 0   
    0x0408, 0x00,       //dig crop x offset   (4224-1928*2)/2 - 1 = 183  = 0xB7
    0x0409, 0xB7,       //dig crop x offset
    0x040A, 0x01,       //dig crop y offset (3208-1096*2)/2 - 1 = 507 = 0x1FB
    0x040B, 0xFB,       //dig crop y offset
#else
    0x0408, 0x00,       //dig crop x offset   (4224-1928*2)/2 - 1 = 183  = 0xB7
    0x0409, 0x00,       //dig crop x offset
    0x040A, 0x00,       //dig crop y offset (3208-1096*2)/2 - 1 = 507 = 0x1FB
    0x040B, 0x00,       //dig crop y offset
#endif
    0x040C, 0x08,       //dig crop image width  1928
    0x040D, 0x38,       //dig crop image width
    0x040E, 0x06,       //dig crop image height  1096
    0x040F, 0x18,       //dig crop image height
    //clock setting
#if 0   
    0x0301, 0x05,       //vtpxck div
    0x0303, 0x02,       //vtsyck div
    0x0305, 0x04,       //prepllck vt div
    0x0306, 0x00,       //pll vt mpy
    0x0307, 0xAC,       //pll vt mpy
    0x0309, 0x0A,       //oppxck div
    0x030B, 0x01,       //opsyck div
    0x0310, 0x00,       //pll mult drv
#else
    0x0301, 0x05,       //vtpxck div
    0x0303, 0x02,       //vtsyck div
    0x0305, 0x03,       //prepllck vt div
    0x0306, 0x00,       //pll vt mpy
    0x0307, 0x6F,       //pll vt mpy    608MHZ * 4
    0x0309, 0x0A,       //oppxck div
    0x030B, 0x01,       //opsyck div
    0x0310, 0x00,       //pll mult drv
#endif
    //data rate setting
    0x0820, 0x10,       //req link bit rate Mbps  integer part
    0x0821, 0x20,
    0x0822, 0x00,     // decimal part
    0x0823, 0x00,
    //water mark setting
    0x3A03, 0x08,
    0x3A04, 0xC0,
    0x3A05, 0x02,
    //enable setting
    0x0B06, 0x01,
    0x30A2, 0x00,
    //hdr setting
    0x3A02, 0xFF,
    //stats setting
    0x3013, 0x00,
    //integration time setting
    0x0202, 0x0C,
    0x0203, 0x70,
    0x0224, 0x01,
    0x0225, 0xF4,
    //analog setting
    0x4170, 0x00,
    0x4171, 0x10,
    0x4176, 0x00,
    0x4177, 0x3C,
    0xAE20, 0x04,
    0xAE21, 0x5C,

    0x0100, 0x01///streaming
};


///no ready
ISP_UINT16 SNR_IMX214_Reg_1920x1080_50P[] = 
{
    0x0100, 0x00,
    ///mode setting 
    0x0114, 0x03,      //csi lane mode
    0x0220, 0x00,      //hdr mode
    0x0221, 0x11,       //[7:4]hdr reso redu h [3:0]hdr reso redu v 
    0x0222, 0x01,   //expo ratio
    0x0340, 0x0C,     //frm length lines
    0x0341, 0x7A,     //frm length lines
    0x0342, 0x13,       //line length pck
    0x0343, 0x90,       //line length pck
    0x0344, 0x00,       //x add sta
    0x0345, 0x00,       //x add sta
    0x0346, 0x00,       //y add sta
    0x0347, 0x00,       //y add sta
    0x0348, 0x10,       //x add end
    0x0349, 0x6F,       //x add end
    0x034A, 0x0C,       //y add end
    0x034B, 0x2F,       //y add end
    0x0381, 0x01,       //x evn inc
    0x0383, 0x01,       //x odd inc
    0x0385, 0x01,       //y evn inc
    0x0387, 0x01,       //y odd inc
    0x0900, 0x00,       //binning mode
    0x0901, 0x00,       //[7:4]binning type h [3:0]binning type v
    0x0902, 0x00,       //binning weighting
    0x3000, 0x35,       //hdr output ctrl
    //0x3054, 0x00,     
    //0x305C, 0x00,
    //output size setting
    0x0112, 0x0A,       //csi dt fmt h
    0x0113, 0x0A,       //csi st fmt l
    0x034C, 0x10,       //x out size
    0x034D, 0x70,       //x out size
    0x034E, 0x0C,       //y out size
    0x034F, 0x30,       //y out size
    0x0401, 0x00,       //scale mode
    0x0404, 0x00,       //scale m
    0x0405, 0x10,       //scale m
    0x0408, 0x00,       //dig crop x offset
    0x0409, 0x00,       //dig crop x offset
    0x040A, 0x00,       //dig crop y offset
    0x040B, 0x00,       //dig crop y offset
    0x040C, 0x10,       //dig crop image width
    0x040D, 0x70,       //dig crop image width
    0x040E, 0x0C,       //dig crop image height
    0x040F, 0x30,       //dig crop image height
    //clock setting
#if 0   
    0x0301, 0x05,       //vtpxck div
    0x0303, 0x02,       //vtsyck div
    0x0305, 0x04,       //prepllck vt div
    0x0306, 0x00,       //pll vt mpy
    0x0307, 0xAC,       //pll vt mpy
    0x0309, 0x0A,       //oppxck div
    0x030B, 0x01,       //opsyck div
    0x0310, 0x00,       //pll mult drv
#else
    0x0301, 0x05,       //vtpxck div
    0x0303, 0x02,       //vtsyck div
    0x0305, 0x03,       //prepllck vt div
    0x0306, 0x00,       //pll vt mpy
    0x0307, 0x6F,       //pll vt mpy    608MHZ * 4
    0x0309, 0x0A,       //oppxck div
    0x030B, 0x01,       //opsyck div
    0x0310, 0x00,       //pll mult drv
#endif
    //data rate setting
    0x0820, 0x10,       //req link bit rate Mbps
    0x0821, 0x20,
    0x0822, 0x00,
    0x0823, 0x00,
    //water mark setting
    0x3A03, 0x08,
    0x3A04, 0xC0,
    0x3A05, 0x02,
    //enable setting
    0x0B06, 0x01,
    0x30A2, 0x00,
    //hdr setting
    0x3A02, 0xFF,
    //stats setting
    0x3013, 0x00,
    //integration time setting
    0x0202, 0x0C,
    0x0203, 0x70,
    0x0224, 0x01,
    0x0225, 0xF4,
    //analog setting
    0x4170, 0x00,
    0x4171, 0x10,
    0x4176, 0x00,
    0x4177, 0x3C,
    0xAE20, 0x04,
    0xAE21, 0x5C,

    0x0100, 0x01///streaming
};


ISP_UINT16 SNR_IMX214_Reg_1920x1080_30P_HDR[] = 
{
    0x0100, 0x00,
    ///mode setting 
    0x0114, 0x03,      //csi lane mode
    0x0220, 0x01,      //hdr mode
    0x0221, 0x22,       //[7:4]hdr reso redu h [3:0]hdr reso redu v 
    0x0222, 0x10,   //expo ratio
    0x0340, 0x06,     //frm length lines
    0x0341, 0x4C,     //frm length lines
    0x0342, 0x13,       //line length pck
    0x0343, 0x90,       //line length pck
    0x0344, 0x00,       //x add sta
    0x0345, 0x00,       //x add sta
    0x0346, 0x00,       //y add sta
    0x0347, 0x00,       //y add sta
    0x0348, 0x10,       //x add end    4207
    0x0349, 0x6F,       //x add end
    0x034A, 0x0C,       //y add end   3119
    0x034B, 0x2F,       //y add end    
    0x0381, 0x01,       //x evn inc
    0x0383, 0x01,       //x odd inc
    0x0385, 0x01,       //y evn inc
    0x0387, 0x01,       //y odd inc
    0x0900, 0x00,       //binning mode
    0x0901, 0x00,       //[7:4]binning type h [3:0]binning type v
    0x0902, 0x00,       //binning weighting
    0x3000, 0x35,       //hdr output ctrl
    0x3054, 0x01,       
    0x305C, 0x11,
    //output size setting
    0x0112, 0x0A,       //csi dt fmt h
    0x0113, 0x0A,       //csi st fmt l
    0x034C, 0x08,       //x out size   2104
    0x034D, 0x38,       //x out size
    0x034E, 0x06,       //y out size   1560
    0x034F, 0x18,       //y out size
    0x0401, 0x00,       //scale mode
    0x0404, 0x00,       //scale m
    0x0405, 0x10,       //scale m
    0x0408, 0x00,       //dig crop x offset
    0x0409, 0x00,       //dig crop x offset
    0x040A, 0x00,       //dig crop y offset
    0x040B, 0x00,       //dig crop y offset
    0x040C, 0x08,       //dig crop image width  2104
    0x040D, 0x38,       //dig crop image width
    0x040E, 0x06,       //dig crop image height  1560
    0x040F, 0x18,       //dig crop image height
    //clock setting
    0x0301, 0x05,       //vtpxck div
    0x0303, 0x02,       //vtsyck div
    0x0305, 0x04,       //prepllck vt div
    0x0306, 0x00,       //pll vt mpy
    0x0307, 0x65,       //pll vt mpy
    0x0309, 0x0A,       //oppxck div
    0x030B, 0x01,       //opsyck div
    0x0310, 0x00,       //pll mult drv
    //data rate setting
    0x0820, 0x09,       //req link bit rate Mbps
    0x0821, 0x78,
    0x0822, 0x00,
    0x0823, 0x00,
    //water mark setting
    0x3A03, 0x06,
    0x3A04, 0xE8,
    0x3A05, 0x01,
    //enable setting
    0x0B06, 0x01,
    0x30A2, 0x00,
    //hdr setting
    0x3A02, 0x06,
    //stats setting
    0x3013, 0x01,
    //integration time setting
    0x0202, 0x06,
    0x0203, 0x42,
    0x0224, 0x00,
    0x0225, 0xC8,
    //analog setting
    0x4170, 0x00,
    0x4171, 0x10,
    0x4176, 0x00,
    0x4177, 0x3C,
    0xAE20, 0x04,
    0xAE21, 0x5C,

    0x0100, 0x01///streaming
};

ISP_UINT16 SNR_IMX214_Reg_1920x1080_30P[] = 
{
#if 1
0x0100, 0x00,
    ///mode setting 
    0x0114, 0x03,      //csi lane mode 3:4lane  1:2lane
    0x0220, 0x00,      //hdr mode
    0x0221, 0x11,       //[7:4]hdr reso redu h [3:0]hdr reso redu v 
    0x0222, 0x01,   //expo ratio
    0x0340, 0x06,     //frm length lines    1618
    0x0341, 0x52,     //frm length lines
    0x0342, 0x13,       //line length pck    5008
    0x0343, 0x90,       //line length pck
    0x0344, 0x00,       //x add sta
    0x0345, 0x00,       //x add sta
    0x0346, 0x00,       //y add sta
    0x0347, 0x00,       //y add sta
    0x0348, 0x10,       //x add end
    0x0349, 0x6F,       //x add end
    0x034A, 0x0C,       //y add end
    0x034B, 0x2F,       //y add end
    0x0381, 0x01,       //x evn inc
    0x0383, 0x01,       //x odd inc
    0x0385, 0x01,       //y evn inc
    0x0387, 0x01,       //y odd inc
    0x0900, 0x01,       //binning mode
    0x0901, 0x22,       //[7:4]binning type h [3:0]binning type v
    0x0902, 0x02,       //binning weighting
    0x3000, 0x35,       //hdr output ctrl
    0x3054, 0x01,       
    0x305C, 0x11,
    //output size setting
    0x0112, 0x0A,       //csi dt fmt h
    0x0113, 0x0A,       //csi st fmt l
    0x034C, 0x08,       //x out size
    0x034D, 0x38,       //x out size
    0x034E, 0x06,       //y out size  1096
    0x034F, 0x18,       //y out size
    0x0401, 0x00,       //scale mode
    0x0404, 0x00,       //scale m
    0x0405, 0x10,       //scale m
    0x0408, 0x00,       //dig crop x offset
    0x0409, 0x00,       //dig crop x offset
    0x040A, 0x00,       //dig crop y offset
    0x040B, 0x00,       //dig crop y offset
    0x040C, 0x08,       //dig crop image width
    0x040D, 0x38,       //dig crop image width
    0x040E, 0x06,       //dig crop image height 1560
    0x040F, 0x18,       //dig crop image height

    //clock setting
    0x0301, 0x05,       //vtpxck div
    0x0303, 0x02,       //vtsyck div
    0x0305, 0x03,       //prepllck vt div
    0x0306, 0x00,       //pll vt mpy
    0x0307, 0x4C,       //pll vt mpy    608MHZ * 4
    0x0309, 0x0A,       //oppxck div
    0x030B, 0x01,       //opsyck div
    0x0310, 0x00,       //pll mult drv
    //data rate setting
    0x0820, 0x09,       //req link bit rate Mbps  159 383552
    0x0821, 0x80,
    0x0822, 0x00,
    0x0823, 0x00,
    //water mark setting
    0x3A03, 0x06,
    0x3A04, 0x68,
    0x3A05, 0x01,
    //enable setting
    0x0B06, 0x01,
    0x30A2, 0x00,
    //test setting
    0x30B4, 0x00,
    //hdr setting
    0x3A02, 0xFF,
    //stats setting
    0x3013, 0x00,
    //integration time setting
    0x0202, 0x06,
    0x0203, 0x48,
    0x0224, 0x01,
    0x0225, 0xF4,

    ////
    0x0204,0x00,   // gain                  
    0x0205,0x00,                       
    0x020E,0x01,     // green                  
    0x020F,0x00,                      
    0x0210,0x01,     // red                   
    0x0211,0x00,                      
    0x0212,0x01,                       
    0x0213,0x00,        // blue                      
    0x0214,0x01,                       
    0x0215,0x00,                       
    0x0216,0x00,                       
    0x0217,0x00, 
    
    //analog setting
    0x4170, 0x00,
    0x4171, 0x10,
    0x4176, 0x00,
    0x4177, 0x3C,
    0xAE20, 0x04,
    0xAE21, 0x5C,
    
    0x0138, 0x01,
    0x0100, 0x01///streaming
#else
    0x0100, 0x00,
    ///mode setting 
    0x0114, 0x03,      //csi lane mode
    0x0220, 0x00,      //hdr mode
    0x0221, 0x11,       //[7:4]hdr reso redu h [3:0]hdr reso redu v 
    0x0222, 0x01,   //expo ratio
    0x0340, 0x06,     //frm length lines
    0x0341, 0x52,     //frm length lines
    0x0342, 0x13,       //line length pck
    0x0343, 0x90,       //line length pck
    0x0344, 0x00,       //x add sta
    0x0345, 0x00,       //x add sta
    0x0346, 0x00,       //y add sta
    0x0347, 0x00,       //y add sta
    0x0348, 0x10,       //x add end
    0x0349, 0x6F,       //x add end
    0x034A, 0x0C,       //y add end
    0x034B, 0x2F,       //y add end
    0x0381, 0x01,       //x evn inc
    0x0383, 0x01,       //x odd inc
    0x0385, 0x01,       //y evn inc
    0x0387, 0x01,       //y odd inc
    0x0900, 0x01,       //binning mode
    0x0901, 0x22,       //[7:4]binning type h [3:0]binning type v
    0x0902, 0x02,       //binning weighting
    0x3000, 0x35,       //hdr output ctrl
    0x3054, 0x01,       
    0x305C, 0x11,
    //output size setting
    0x0112, 0x0A,       //csi dt fmt h
    0x0113, 0x0A,       //csi st fmt l
    0x034C, 0x08,       //x out size
    0x034D, 0x38,       //x out size
    0x034E, 0x06,       //y out size
    0x034F, 0x18,       //y out size
    0x0401, 0x00,       //scale mode
    0x0404, 0x00,       //scale m
    0x0405, 0x10,       //scale m
    0x0408, 0x00,       //dig crop x offset
    0x0409, 0x00,       //dig crop x offset
    0x040A, 0x00,       //dig crop y offset
    0x040B, 0x00,       //dig crop y offset
    0x040C, 0x08,       //dig crop image width
    0x040D, 0x38,       //dig crop image width
    0x040E, 0x06,       //dig crop image height
    0x040F, 0x18,       //dig crop image height
    //clock setting
    0x0301, 0x05,       //vtpxck div
    0x0303, 0x02,       //vtsyck div
    0x0305, 0x03,       //prepllck vt div
    0x0306, 0x00,       //pll vt mpy
    0x0307, 0x4C,       //pll vt mpy
    0x0309, 0x0A,       //oppxck div
    0x030B, 0x01,       //opsyck div
    0x0310, 0x00,       //pll mult drv
    //data rate setting
    0x0820, 0x09,       //req link bit rate Mbps
    0x0821, 0x80,
    0x0822, 0x00,
    0x0823, 0x00,
    //water mark setting
    0x3A03, 0x06,
    0x3A04, 0x68,
    0x3A05, 0x01,
    //enable setting
    0x0B06, 0x01,
    0x30A2, 0x00,

    0x30B4,0x00,
    //hdr setting
    0x3A02, 0xFF,
    //stats setting
    0x3013, 0x00,
    //integration time setting
    0x0202, 0x06,
    0x0203, 0x48,
    0x0224, 0x01,
    0x0225, 0xF4,

    ////
    0x0204,0x00,                    
    0x0205,0x00,                       
    0x020E,0x01,                      
    0x020F,0x00,                      
    0x0210,0x01,                      
    0x0211,0x00,                      
    0x0212,0x01,                       
    0x0213,0x00,                      
    0x0214,0x01,                       
    0x0215,0x00,                       
    0x0216,0x00,                       
    0x0217,0x00, 
    //analog setting
    0x4170, 0x00,
    0x4171, 0x10,
    0x4176, 0x00,
    0x4177, 0x3C,
    0xAE20, 0x04,
    0xAE21, 0x5C,
    
    0x0138, 0x01,
    0x0100, 0x01///streaming
   #endif
};

ISP_UINT16 SNR_IMX214_Reg_1280x720_30P[] = 
{
    0x0100, 0x00,
    ///mode setting 
    0x0114, 0x03,      //csi lane mode
    0x0220, 0x00,      //hdr mode
    0x0221, 0x11,       //[7:4]hdr reso redu h [3:0]hdr reso redu v 
    0x0222, 0x01,   //expo ratio
    0x0340, 0x03,     //frm length lines  798
    0x0341, 0x1E,     //frm length lines
    0x0342, 0x13,       //line length pck
    0x0343, 0x90,       //line length pck
    #if 0
    0x0344, 0x00,       //X_ADD_STA
    0x0345, 0x00,       //X_ADD_STA
    0x0346, 0x00,       //Y_ADD_STA
    0x0347, 0x00,       //Y_ADD_STA
    0x0348, 0x10,       //X_ADD_END
    0x0349, 0x6F,       //X_ADD_END  4207
    0x034A, 0x0C,       //Y_ADD_END
    0x034B, 0x2F,       //Y_ADD_END  3119
    #else
    0x0344, 0x03,       //X_ADD_STA   808
    0x0345, 0x28,       //X_ADD_STA
    0x0346, 0x03,       //Y_ADD_STA   808
    0x0347, 0x28,       //Y_ADD_STA
    0x0348, 0x0D,       //X_ADD_END
    0x0349, 0x48,       //X_ADD_END  3400
    0x034A, 0x09,       //Y_ADD_END
    0x034B, 0x08,       //Y_ADD_END  2312
    #endif
    0x0381, 0x01,       //x evn inc
    0x0383, 0x01,       //x odd inc
    0x0385, 0x01,       //y evn inc
    0x0387, 0x01,       //y odd inc
    0x0900, 0x01,       //binning mode
    0x0901, 0x22,       //[7:4]binning type h [3:0]binning type v
    0x0902, 0x02,       //binning weighting
    0x3000, 0x35,       //hdr output ctrl
    //0x3054, 0x00,     
    //0x305C, 0x00,
    //output size setting
    0x0112, 0x0A,       //csi dt fmt h
    0x0113, 0x0A,       //csi st fmt l

    0x034C, 0x05,       //x out size  1296
    0x034D, 0x10,       //x out size
    0x034E, 0x02,       //y out size  752
    0x034F, 0xF0,       //y out size

    0x0401, 0x00,       //scale mode
    0x0404, 0x00,       //scale m
    0x0405, 0x20,       //scale m

    0x0408, 0x00,       //dig crop x offset  
    0x0409, 0x00,       //dig crop x offset
    0x040A, 0x00,       //dig crop y offset 
    0x040B, 0x00,       //dig crop y offset

    0x040C, 0x05,       //dig crop image width  1296
    0x040D, 0x10,       //dig crop image width
    0x040E, 0x02,       //dig crop image height  752
    0x040F, 0xF0,       //dig crop image height

    //clock setting
    0x0301, 0x05,       //vtpxck div
    0x0303, 0x02,       //vtsyck div
    0x0305, 0x03,       //prepllck vt div
    0x0306, 0x00,       //pll vt mpy
    0x0307, 0x25,       //pll vt mpy    312MHZ * 4
    0x0309, 0x0A,       //oppxck div
    0x030B, 0x01,       //opsyck div
    0x0310, 0x00,       //pll mult drv

    //data rate setting
    0x0820, 0x12,       //req link bit rate Mbps
    0x0821, 0xC0,
    0x0822, 0x00,
    0x0823, 0x00,
    //water mark setting
    0x3A03, 0x06,
    0x3A04, 0x68,
    0x3A05, 0x01,
    //enable setting
    0x0B06, 0x01,
    0x30A2, 0x00,
    //hdr setting
    0x3A02, 0xFF,
    //stats setting
    0x3013, 0x00,
    //integration time setting
    0x0202, 0x04,
    0x0203, 0x56,
    0x0224, 0x01,
    0x0225, 0xF4,
    //analog setting
    0x4170, 0x00,
    0x4171, 0x10,
    0x4176, 0x00,
    0x4177, 0x3C,
    0xAE20, 0x04,
    0xAE21, 0x5C,

    0x0100, 0x01///streaming
};

ISP_UINT16 SNR_IMX214_Reg_1280x720_60P[] = 
{
    0x0100, 0x00,
    ///mode setting 
    0x0114, 0x03,      //csi lane mode
    0x0220, 0x00,      //hdr mode
    0x0221, 0x11,       //[7:4]hdr reso redu h [3:0]hdr reso redu v 
    0x0222, 0x01,   //expo ratio
    0x0340, 0x03,      //frm length lines  798
    0x0341, 0x1E,      //frm length lines
    0x0342, 0x13,       //line length pck
    0x0343, 0x90,       //line length pck
    #if 0
    0x0344, 0x00,       //x add sta
    0x0345, 0x00,       //x add sta
    0x0346, 0x00,       //y add sta
    0x0347, 0x00,       //y add sta
    0x0348, 0x10,       //x add end
    0x0349, 0x6F,       //x add end
    0x034A, 0x0C,       //y add end
    0x034B, 0x2F,       //y add end
    #else
    0x0344, 0x03,       //X_ADD_STA   808
    0x0345, 0x28,       //X_ADD_STA
    0x0346, 0x03,       //Y_ADD_STA   808
    0x0347, 0x28,       //Y_ADD_STA
    0x0348, 0x0D,       //X_ADD_END
    0x0349, 0x48,       //X_ADD_END  3400
    0x034A, 0x09,       //Y_ADD_END
    0x034B, 0x08,       //Y_ADD_END  2312
    #endif
    0x0381, 0x01,       //x evn inc
    0x0383, 0x01,       //x odd inc
    0x0385, 0x01,       //y evn inc
    0x0387, 0x01,       //y odd inc
    0x0900, 0x01,       //binning mode
    0x0901, 0x22,       //[7:4]binning type h [3:0]binning type v
    0x0902, 0x02,       //binning weighting
    0x3000, 0x35,       //hdr output ctrl
    //0x3054, 0x00,     
    //0x305C, 0x00,
    //output size setting
    0x0112, 0x0A,       //csi dt fmt h
    0x0113, 0x0A,       //csi st fmt l
    0x034C, 0x05,       //x out size  1296
    0x034D, 0x10,       //x out size
    0x034E, 0x02,       //y out size    752
    0x034F, 0xF0,       //y out size
    0x0401, 0x00,       //scale mode
    0x0404, 0x00,       //scale m
    0x0405, 0x10,       //scale m
    0x0408, 0x00,       //dig crop x offset  
    0x0409, 0x00,       //dig crop x offset
    0x040A, 0x00,       //dig crop y offset  
    0x040B, 0x00,       //dig crop y offset
    0x040C, 0x05,       //dig crop image width  1296
    0x040D, 0x10,       //dig crop image width
    0x040E, 0x02,       //dig crop image height  752
    0x040F, 0xF0,       //dig crop image height
    //clock setting
    0x0301, 0x05,       //vtpxck div
    0x0303, 0x02,       //vtsyck div
    0x0305, 0x03,       //prepllck vt div
    0x0306, 0x00,       //pll vt mpy
    0x0307, 0x25,       //pll vt mpy    624MHZ * 4
    0x0309, 0x0A,       //oppxck div
    0x030B, 0x01,       //opsyck div
    0x0310, 0x00,       //pll mult drv
    //data rate setting
    0x0820, 0x12,       //req link bit rate Mbps
    0x0821, 0xC0,
    0x0822, 0x00,
    0x0823, 0x00,
    //water mark setting
    0x3A03, 0x06,
    0x3A04, 0x68,
    0x3A05, 0x01,
    //enable setting
    0x0B06, 0x01,
    0x30A2, 0x00,
    //hdr setting
    0x3A02, 0xFF,
    //stats setting
    0x3013, 0x00,
    //integration time setting
    0x0202, 0x04,
    0x0203, 0x56,
    0x0224, 0x01,
    0x0225, 0xF4,
    //analog setting
    0x4170, 0x00,
    0x4171, 0x10,
    0x4176, 0x00,
    0x4177, 0x3C,
    0xAE20, 0x04,
    0xAE21, 0x5C,

    0x0100, 0x01///streaming
};

///no ready
ISP_UINT16 SNR_IMX214_Reg_1280x720_100P[] = 
{
    0x0100, 0x00,
    ///mode setting 
    0x0114, 0x03,      //csi lane mode
    0x0220, 0x00,      //hdr mode
    0x0221, 0x11,       //[7:4]hdr reso redu h [3:0]hdr reso redu v 
    0x0222, 0x01,   //expo ratio
    0x0340, 0x04,     //frm length lines
    0x0341, 0x60,     //frm length lines
    0x0342, 0x13,       //line length pck
    0x0343, 0x90,       //line length pck
    0x0344, 0x00,       //x add sta
    0x0345, 0xB8,       //x add sta
    0x0346, 0x01,       //y add sta
    0x0347, 0xE0,       //y add sta
    0x0348, 0x0F,       //x add end
    0x0349, 0xB7,       //x add end
    0x034A, 0x0A,       //y add end
    0x034B, 0x4F,       //y add end
    0x0381, 0x01,       //x evn inc
    0x0383, 0x01,       //x odd inc
    0x0385, 0x01,       //y evn inc
    0x0387, 0x01,       //y odd inc
    0x0900, 0x01,       //binning mode
    0x0901, 0x22,       //[7:4]binning type h [3:0]binning type v
    0x0902, 0x02,       //binning weighting
    0x3000, 0x35,       //hdr output ctrl
    //0x3054, 0x00,     
    //0x305C, 0x00,
    //output size setting
    0x0112, 0x0A,       //csi dt fmt h
    0x0113, 0x0A,       //csi st fmt l
    0x034C, 0x07,       //x out size
    0x034D, 0x80,       //x out size
    0x034E, 0x04,       //y out size
    0x034F, 0x38,       //y out size
    0x0401, 0x00,       //scale mode
    0x0404, 0x00,       //scale m
    0x0405, 0x10,       //scale m
    0x0408, 0x00,       //dig crop x offset
    0x0409, 0x00,       //dig crop x offset
    0x040A, 0x00,       //dig crop y offset
    0x040B, 0x00,       //dig crop y offset
    0x040C, 0x07,       //dig crop image width
    0x040D, 0x80,       //dig crop image width
    0x040E, 0x04,       //dig crop image height
    0x040F, 0x38,       //dig crop image height
    //clock setting
    0x0301, 0x05,       //vtpxck div
    0x0303, 0x02,       //vtsyck div
    0x0305, 0x03,       //prepllck vt div
    0x0306, 0x00,       //pll vt mpy
    0x0307, 0x96,       //pll vt mpy
    0x0309, 0x0A,       //oppxck div
    0x030B, 0x01,       //opsyck div
    0x0310, 0x00,       //pll mult drv
    //data rate setting
    0x0820, 0x12,       //req link bit rate Mbps  ED4E000
    0x0821, 0xC0,
    0x0822, 0x00,
    0x0823, 0x00,
    //water mark setting
    0x3A03, 0x06,
    0x3A04, 0x68,
    0x3A05, 0x01,
    //enable setting
    0x0B06, 0x01,
    0x30A2, 0x00,
    //hdr setting
    0x3A02, 0xFF,
    //stats setting
    0x3013, 0x00,
    //integration time setting
    0x0202, 0x04,
    0x0203, 0x56,
    0x0224, 0x01,
    0x0225, 0xF4,
    //analog setting
    0x4170, 0x00,
    0x4171, 0x10,
    0x4176, 0x00,
    0x4177, 0x3C,
    0xAE20, 0x04,
    0xAE21, 0x5C,

    0x0100, 0x01///streaming
};

ISP_UINT16 SNR_IMX214_Reg_2016x1512_30P[] = 
{
    0x0100, 0x00,
    ///mode setting 
    0x0114, 0x03,      //csi lane mode 3:4lane  1:2lane
    0x0220, 0x00,      //hdr mode
    0x0221, 0x11,       //[7:4]hdr reso redu h [3:0]hdr reso redu v 
    0x0222, 0x01,   //expo ratio
    0x0340, 0x06,     //frm length lines    1720
    0x0341, 0xB8,     //frm length lines
    0x0342, 0x13,       //line length pck    5008
    0x0343, 0x90,       //line length pck
    
    0x0344, 0x00,       //x add sta    
    0x0345, 0x00,       //x add sta       
    0x0346, 0x00,       //y add sta    
    0x0347, 0x00,       //y add sta
    0x0348, 0x10,       //x add end
    0x0349, 0x6F,       //x add end
    0x034A, 0x0C,       //y add end
    0x034B, 0x2F,       //y add end

    0x0381, 0x01,       //x evn inc
    0x0383, 0x01,       //x odd inc
    0x0385, 0x01,       //y evn inc
    0x0387, 0x01,       //y odd inc
    0x0900, 0x01,       //binning mode
    0x0901, 0x22,       //[7:4]binning type h [3:0]binning type v
    0x0902, 0x02,       //binning weighting
    0x3000, 0x35,       //hdr output ctrl
    0x3054, 0x01,       
    0x305C, 0x11,
    //output size setting
    0x0112, 0x0A,       //csi dt fmt h   RAW10 format
    0x0113, 0x0A,       //csi st fmt l

    0x034C, 0x08,       //x out size       2056
    0x034D, 0x00,       //x out size
    0x034E, 0x06,       //y out size      1540
    0x034F, 0x00,       //y out size

    0x0401, 0x00,       //scale mode
    0x0404, 0x00,       //scale m
    0x0405, 0x10,       //scale m
    
    0x0408, 0x00,       //dig crop x offset    0xB8 - 1
    0x0409, 0x00,       //dig crop x offset
    0x040A, 0x00,       //dig crop y offset     0x1E0 - 1
    0x040B, 0x00,       //dig crop y offset

    0x040C, 0x08,       //dig crop image width     2048
    0x040D, 0x00,       //dig crop image width
    0x040E, 0x06,       //dig crop image height    1536
    0x040F, 0x00,       //dig crop image height

    //clock setting
#if 0   
    0x0301, 0x05,       //vtpxck div
    0x0303, 0x02,       //vtsyck div
    0x0305, 0x03,       //prepllck vt div
    0x0306, 0x00,       //pll vt mpy
    0x0307, 0x4c,       //pll vt mpy    608MHZ * 4
    0x0309, 0x0A,       //oppxck div
    0x030B, 0x01,       //opsyck div
    0x0310, 0x00,       //pll mult drv
#else
    0x0301, 0x05,       //vtpxck div
    0x0303, 0x02,       //vtsyck div
    0x0305, 0x03,       //prepllck vt div
    0x0306, 0x00,       //pll vt mpy
    0x0307, 0x60,       //pll vt mpy    608MHZ * 4
    0x0309, 0x0A,       //oppxck div
    0x030B, 0x01,       //opsyck div
    0x0310, 0x00,       //pll mult drv
#endif
    //data rate setting
    0x0820, 0x10,       //req link bit rate Mbps  integer part
    0x0821, 0x20,
    0x0822, 0x00,     // decimal part
    0x0823, 0x00,
    //water mark setting
    0x3A03, 0x06,
    0x3A04, 0x68,
    0x3A05, 0x01,
    //enable setting
    0x0B06, 0x01,
    0x30A2, 0x00,
    //test setting
    0x30B4, 0x00,
    //hdr setting
    0x3A02, 0xFF,
    //stats setting
    0x3013, 0x00,
    //integration time setting
    //0x0202, 0x06,   // shutter
    //0x0203, 0x48,
    //0x0224, 0x01,
    //0x0225, 0xF4,

    ////
    //0x0204,0x00,   // gain                  
  //  0x0205,0x00,                       
  //  0x020E,0x01,                      
  //  0x020F,0x00,                      
  // 0x0210,0x01,                      
   //0x0211,0x00,                      
   // 0x0212,0x01,                       
   // 0x0213,0x00,                      
   // 0x0214,0x01,                       
  //  0x0215,0x00,                       
   // 0x0216,0x00,                       
   // 0x0217,0x00, 
    //analog setting
    0x4170, 0x00,
    0x4171, 0x10,
    0x4176, 0x00,
    0x4177, 0x3C,
    0xAE20, 0x04,
    0xAE21, 0x5C,
    
    0x0138, 0x01,
    0x0100, 0x01///streaming

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
    int i;
    printc("SNR_Cust_InitConfig IMX214\r\n");
    
    // Init OPR Table
    SensorCustFunc.OprTable->usInitSize                         = (sizeof(SNR_IMX214_Reg_Init_Customer)/sizeof(SNR_IMX214_Reg_Init_Customer[0]))/2;
    SensorCustFunc.OprTable->uspInitTable                       = &SNR_IMX214_Reg_Init_Customer[0];    
    
    for (i = 0; i < MAX_SENSOR_RES_MODE; i++)
    {
        SensorCustFunc.OprTable->usSize[i]                          = (sizeof(SNR_IMX214_Reg_Unsupport)/sizeof(SNR_IMX214_Reg_Unsupport[0]))/2;
        SensorCustFunc.OprTable->uspTable[i]                        = &SNR_IMX214_Reg_Unsupport[0];
    }

    SensorCustFunc.OprTable->usSize[RES_IDX_4208x3120_25P_HDR]    = (sizeof(SNR_IMX214_Reg_4208x3120_25P_HDR)/sizeof(SNR_IMX214_Reg_4208x3120_25P_HDR[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_4208x3120_25P_HDR]  = &SNR_IMX214_Reg_4208x3120_25P_HDR[0];
    
    SensorCustFunc.OprTable->usSize[RES_IDX_4208x3120_25P]          = (sizeof(SNR_IMX214_Reg_4208x3120_25P)/sizeof(SNR_IMX214_Reg_4208x3120_25P[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_4208x3120_25P]        = &SNR_IMX214_Reg_4208x3120_25P[0];
    
    SensorCustFunc.OprTable->usSize[RES_IDX_3840x2160_30P_HDR]      = (sizeof(SNR_IMX214_Reg_3840x2160_30P_HDR)/sizeof(SNR_IMX214_Reg_3840x2160_30P_HDR[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_3840x2160_30P_HDR]    = &SNR_IMX214_Reg_3840x2160_30P_HDR[0];

    SensorCustFunc.OprTable->usSize[RES_IDX_3840x2160_30P]    = (sizeof(SNR_IMX214_Reg_3840x2160_30P)/sizeof(SNR_IMX214_Reg_3840x2160_30P[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_3840x2160_30P]  = &SNR_IMX214_Reg_3840x2160_30P[0];
    
    SensorCustFunc.OprTable->usSize[RES_IDX_1920x1080_30P_HDR]          = (sizeof(SNR_IMX214_Reg_1920x1080_30P_HDR)/sizeof(SNR_IMX214_Reg_1920x1080_30P_HDR[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1920x1080_30P_HDR]        = &SNR_IMX214_Reg_1920x1080_30P_HDR[0];
    
    SensorCustFunc.OprTable->usSize[RES_IDX_1920x1080_50P]      = (sizeof(SNR_IMX214_Reg_1920x1080_50P)/sizeof(SNR_IMX214_Reg_1920x1080_50P[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1920x1080_50P]    = &SNR_IMX214_Reg_1920x1080_50P[0];

    SensorCustFunc.OprTable->usSize[RES_IDX_1920x1080_30P]    = (sizeof(SNR_IMX214_Reg_1920x1080_30P)/sizeof(SNR_IMX214_Reg_1920x1080_30P[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1920x1080_30P]  = &SNR_IMX214_Reg_1920x1080_30P[0];
    
    SensorCustFunc.OprTable->usSize[RES_IDX_1440x1080_30P]    = (sizeof(SNR_IMX214_Reg_1920x1080_30P)/sizeof(SNR_IMX214_Reg_1920x1080_30P[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1440x1080_30P]  = &SNR_IMX214_Reg_1920x1080_30P[0];
        
    SensorCustFunc.OprTable->usSize[RES_IDX_1280x720_100P]          = (sizeof(SNR_IMX214_Reg_1280x720_100P)/sizeof(SNR_IMX214_Reg_1280x720_100P[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720_100P]        = &SNR_IMX214_Reg_1280x720_100P[0];
    
    SensorCustFunc.OprTable->usSize[RES_IDX_1280x720_60P]      = (sizeof(SNR_IMX214_Reg_1280x720_60P)/sizeof(SNR_IMX214_Reg_1280x720_60P[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720_60P]    = &SNR_IMX214_Reg_1280x720_60P[0];
    
    SensorCustFunc.OprTable->usSize[RES_IDX_1280x720_30P]          = (sizeof(SNR_IMX214_Reg_1280x720_30P)/sizeof(SNR_IMX214_Reg_1280x720_30P[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1280x720_30P]        = &SNR_IMX214_Reg_1280x720_30P[0];

    SensorCustFunc.OprTable->usSize[RES_IDX_4208x3120_10P]          = (sizeof(SNR_IMX214_Reg_4208x3120_10P)/sizeof(SNR_IMX214_Reg_4208x3120_10P[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_4208x3120_10P]        = &SNR_IMX214_Reg_4208x3120_10P[0];

    SensorCustFunc.OprTable->usSize[RES_IDX_2016x1512_30P]          = (sizeof(SNR_IMX214_Reg_2016x1512_30P)/sizeof(SNR_IMX214_Reg_2016x1512_30P[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_2016x1512_30P]        = &SNR_IMX214_Reg_2016x1512_30P[0];

    SensorCustFunc.OprTable->usSize[RES_IDX_1920x1080_60P]          = (sizeof(SNR_IMX214_Reg_1920x1080_60P)/sizeof(SNR_IMX214_Reg_1920x1080_60P[0]))/2;
    SensorCustFunc.OprTable->uspTable[RES_IDX_1920x1080_60P]        = &SNR_IMX214_Reg_1920x1080_60P[0];
    
    // Init Vif Setting : Common
    SensorCustFunc.VifSetting->SnrType                              = MMPF_VIF_SNR_TYPE_BAYER;
    SensorCustFunc.VifSetting->OutInterface                         = MMPF_VIF_IF_MIPI_QUAD;
    SensorCustFunc.VifSetting->VifPadId                             = MMPF_VIF_MDL_ID0;

    // Init Vif Setting : PowerOn Setting
    SensorCustFunc.VifSetting->powerOnSetting.bTurnOnExtPower       = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPin         = SENSOR_GPIO_ENABLE; // it might be defined in Config_SDK.h
    SensorCustFunc.VifSetting->powerOnSetting.bExtPowerPinHigh      = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.usExtPowerPinDelay    = 10; ///50
    SensorCustFunc.VifSetting->powerOnSetting.bFirstEnPinHigh       = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOnSetting.ubFirstEnPinDelay     = 10;
    SensorCustFunc.VifSetting->powerOnSetting.bNextEnPinHigh        = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubNextEnPinDelay      = 10;
    SensorCustFunc.VifSetting->powerOnSetting.bTurnOnClockBeforeRst = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.bFirstRstPinHigh      = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOnSetting.ubFirstRstPinDelay    = 100;
    SensorCustFunc.VifSetting->powerOnSetting.bNextRstPinHigh       = MMP_TRUE;
    SensorCustFunc.VifSetting->powerOnSetting.ubNextRstPinDelay     = 100;
    
    // Init Vif Setting : PowerOff Setting
    SensorCustFunc.VifSetting->powerOffSetting.bEnterStandByMode    = MMP_FALSE;
    SensorCustFunc.VifSetting->powerOffSetting.usStandByModeReg     = 0x0100;
    SensorCustFunc.VifSetting->powerOffSetting.usStandByModeMask    = 0x01;
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
    SensorCustFunc.VifSetting->paralAttr.ubVsyncPolarity            = MMPF_VIF_SNR_CLK_POLARITY_POS;

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
    SensorCustFunc.VifSetting->colorId.VifColorId                   = MMPF_VIF_COLORID_00;
    SensorCustFunc.VifSetting->colorId.CustomColorId.bUseCustomId   = MMP_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_DoAE_FrmSt
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_DoAE_FrmSt(MMP_UBYTE ubSnrSel, MMP_ULONG ulFrameCnt)
{
#if 0//(ISP_EN)
    ISP_UINT32 s_gain;

    switch (ulFrameCnt % 6) {
    case 0:
        ISP_IF_AE_Execute();
        s_gain = ISP_MAX(ISP_IF_AE_GetGain(), ISP_IF_AE_GetGainBase());

#if 1
        if (s_gain >= ISP_IF_AE_GetGainBase() * MAX_SENSOR_GAIN) {
            s_gain  = ISP_IF_AE_GetGainBase() * MAX_SENSOR_GAIN;
        }
#else
        s_gain = 1 * 256;
#endif

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
#if 0//(ISP_EN)
    ISP_UINT32 s_gain;

    //Change Again setting for rounding error
    #if 0
    s_gain = VR_MIN(256 - (256 * ISP_IF_AE_GetGainBase() + (ulGain-1)) /
                    VR_MAX(ulGain, ISP_IF_AE_GetGainBase()), 224);
    #else
    s_gain = VR_MIN(512 - (512 * ISP_IF_AE_GetGainBase()) /
                    VR_MAX(ulGain, ISP_IF_AE_GetGainBase()), 224);
    #endif
              
    gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x204, (s_gain >> 8) & 0xff);
    gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x205, (s_gain >> 0) & 0xff);
#endif
}

//------------------------------------------------------------------------------
//  Function    : SNR_Cust_SetShutter
//  Description :
//------------------------------------------------------------------------------
void SNR_Cust_SetShutter(MMP_UBYTE ubSnrSel, MMP_ULONG shutter, MMP_ULONG vsync)
{
#if 0//(ISP_EN)
    ISP_UINT32 new_vsync    = gSnrLineCntPerSec * ISP_IF_AE_GetVsync() / ISP_IF_AE_GetVsyncBase();
    ISP_UINT32 new_shutter  = gSnrLineCntPerSec * ISP_IF_AE_GetShutter() / ISP_IF_AE_GetShutterBase();

    // 1 ~ (frame length line - 10)
    new_vsync = VR_MIN(VR_MAX(new_shutter + 10, new_vsync), 0xFFFF);
    new_shutter = VR_MIN(VR_MAX(new_shutter, 1), new_vsync - 10);

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
    MMP_USHORT usRdVal0A38,usRdVal0A39= 0;
    
    gsSensorFunction->MMPF_Sensor_GetReg(ubSnrSel, 0x0005, &usRdVal0A38);
    //gsSensorFunction->MMPF_Sensor_SetReg(ubSnrSel, 0x0136, 0x09);
    // MMPF_OS_SleepMs(100);
     //gsSensorFunction->MMPF_Sensor_GetReg(ubSnrSel, 0x0A38, &usRdVal0A38);
    gsSensorFunction->MMPF_Sensor_GetReg(ubSnrSel, 0x0A39, &usRdVal0A39); 
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

#endif // (BIND_SENSOR_IMX214)
#endif // (SENSOR_EN)

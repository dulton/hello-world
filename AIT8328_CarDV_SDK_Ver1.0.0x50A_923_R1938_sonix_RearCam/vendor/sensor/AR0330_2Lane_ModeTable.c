#if (SENSOR_IF == SENSOR_IF_MIPI_2_LANE)

MMPF_SENSOR_RESOLUTION m_SensorRes = 
{
	5,				// ubSensorModeNum
	0,				// ubDefPreviewMode
	1,				// ubDefCaptureMode
	2200,           // usPixelSize
//  Mode0   Mode1   Mode2   Mode3
    {1,     1,    1,      1,      1},	    // usVifGrabStX
    {1,     1,    1,      1,      1},	    // usVifGrabStY
    {2312,  2312,   2311,   1156,   1284},	// usVifGrabW
    {1300,  1300,   1540,   652,    724},	// usVifGrabH
	#if (CHIP == MCR_V2)
    {1,     1,    1,      1,      1},	    // usVifGrabStX
    {1,     1,    1,      1,      1},	    // usVifGrabStY
    {8,     8,   263,    4,      4},     // usBayerDummyInX
    {4,     4,    4,      4,      4},     // usBayerDummyInY
    {2304,  2304,  2048,   1152,   1280},	// usBayerOutW
    {1296,  1296,  1536,   648,    720},	// usBayerOutH
  	#endif
    {2304,  2304,  2048,   800,    1280},	// usScalInputW
    {1296,  1296,  1536,   600,    720},	// usScalInputH
    {300,   250,   250,    600,    600},	// usTargetFpsx10
    {1329,  1329,  1540,   672,    811},	// usVsyncLine
    {1,     1,    1,      1,      1},     // ubWBinningN
    {1,     1,    1,      1,      1},     // ubWBinningN
    {1,     1,    1,      1,      1},     // ubWBinningN
    {1,     1,    1,      1,      1},     // ubWBinningN
    {0xFF,  0xFF,  0xFF,   0xFF,   0xFF},  // ubCustIQmode
    {0xFF,  0xFF,  0xFF,   0xFF,   0xFF}   // ubCustAEmode
};

#if 0
void ____Sensor_Init_OPR_Table____(){ruturn;} //dummy
#endif

MMP_USHORT SNR_AR0330_MIPI_2_LANE_Reg_Init_Customer[] = 
{
    0x31AE, 0x0202,
    0x301A, 0x0058,     // Disable streaming

    SENSOR_DELAY_REG, 10,

    0x31AE, 0x202,      // Output 2-lane MIPI

    //Configure for Serial Interface
    0x301A, 0x0058,     // Drive Pins,Parallel Enable,SMIA Serializer Disable
    0x3064, 0x1802, 	// Disable Embedded Data

    //Optimized Gain Configuration
    0x3EE0, 0x1500, 	// DAC_LD_20_21
    0x3EEA, 0x001D,
    0x31E0, 0x0003,
    0x3F06, 0x046A,
    0x3ED2, 0x0186,
    0x3ED4, 0x8F2C,	
    0x3ED6, 0x2244,	
    0x3ED8, 0x6442,	
    0x30BA, 0x002C,     // Dither enable
    0x3046, 0x4038,		// Enable Flash Pin
    0x3048, 0x8480,		// Flash Pulse Length
    0x3ED0, 0x0016, 	// DAC_LD_4_5
    0x3ED0, 0x0036, 	// DAC_LD_4_5
    0x3ED0, 0x0076, 	// DAC_LD_4_5
    0x3ED0, 0x00F6, 	// DAC_LD_4_5
    0x3ECE, 0x1003, 	// DAC_LD_2_3
    0x3ECE, 0x100F, 	// DAC_LD_2_3
    0x3ECE, 0x103F, 	// DAC_LD_2_3
    0x3ECE, 0x10FF, 	// DAC_LD_2_3
    0x3ED0, 0x00F6, 	// DAC_LD_4_5
    0x3ED0, 0x04F6, 	// DAC_LD_4_5
    0x3ED0, 0x24F6, 	// DAC_LD_4_5
    0x3ED0, 0xE4F6, 	// DAC_LD_4_5
    0x3EE6, 0xA480, 	// DAC_LD_26_27
    0x3EE6, 0xA080, 	// DAC_LD_26_27
    0x3EE6, 0x8080, 	// DAC_LD_26_27
    0x3EE6, 0x0080, 	// DAC_LD_26_27
    0x3EE6, 0x0080, 	// DAC_LD_26_27
    0x3EE8, 0x2024, 	// DAC_LD_28_29
    0x30FE, 128,        // Noise Pedestal of 128

    //Assuming Input Clock of 24MHz.  Output Clock will be 70Mpixel/s
    0x302A, 0x0005, 	// VT_PIX_CLK_DIV
    0x302C, 0x0002, 	// VT_SYS_CLK_DIV
    0x302E, 0x0004, 	// PRE_PLL_CLK_DIV
    0x3030, 82, 	    // PLL_MULTIPLIER
    0x3036, 0x000A, 	// OP_PIX_CLK_DIV
    0x3038, 0x0001, 	// OP_SYS_CLK_DIV
    0x31AC, 0x0A0A, 	// DATA_FORMAT_BITS

    //MIPI TIMING
    0x31B0, 40,         // FRAME PREAMBLE
    0x31B2, 14,         // LINE PREAMBLE
    0x31B4, 0x2743,     // MIPI TIMING 0
    0x31B6, 0x114E,     // MIPI TIMING 1
    0x31B8, 0x2049,     // MIPI TIMING 2
    0x31BA, 0x0186,     // MIPI TIMING 3
    0x31BC, 0x8005,     // MIPI TIMING 4
    0x31BE, 0x2003,     // MIPI CONFIG STATUS

    //Sequencer
    0x3088, 0x8000,      // SEQ_CTRL_PORT - [0:00:29.002]
    0x3086, 0x4A03,     // SEQ_DATA_PORT - [0:00:29.010]
    0x3086, 0x4316,      // SEQ_DATA_PORT - [0:00:29.014]
    0x3086, 0x0443,      // SEQ_DATA_PORT - [0:00:29.019]
    0x3086, 0x1645,      // SEQ_DATA_PORT - [0:00:29.023]
    0x3086, 0x4045,     // SEQ_DATA_PORT - [0:00:29.028]
    0x3086, 0x6017,      // SEQ_DATA_PORT - [0:00:29.033]
    0x3086, 0x2045,      // SEQ_DATA_PORT - [0:00:29.037]
    0x3086, 0x404B,     // SEQ_DATA_PORT - [0:00:29.042]
    0x3086, 0x1244,      // SEQ_DATA_PORT - [0:00:29.047]
    0x3086, 0x6134,      // SEQ_DATA_PORT - [0:00:29.051]
    0x3086, 0x4A31,     // SEQ_DATA_PORT - [0:00:29.055]
    0x3086, 0x4342,      // SEQ_DATA_PORT - [0:00:29.058]
    0x3086, 0x4560,      // SEQ_DATA_PORT - [0:00:29.061]
    0x3086, 0x2714,      // SEQ_DATA_PORT - [0:00:29.064]
    0x3086, 0x3DFF,     // SEQ_DATA_PORT - [0:00:29.067]
    0x3086, 0x3DFF,     // SEQ_DATA_PORT - [0:00:29.070]
    0x3086, 0x3DEA,    // SEQ_DATA_PORT - [0:00:29.073]
    0x3086, 0x2704,      // SEQ_DATA_PORT - [0:00:29.076]
    0x3086, 0x3D10,     // SEQ_DATA_PORT - [0:00:29.079]
    0x3086, 0x2705,      // SEQ_DATA_PORT - [0:00:29.082]
    0x3086, 0x3D10,     // SEQ_DATA_PORT - [0:00:29.085]
    0x3086, 0x2715,      // SEQ_DATA_PORT - [0:00:29.088]
    0x3086, 0x3527,      // SEQ_DATA_PORT - [0:00:29.091]
    0x3086, 0x053D,     // SEQ_DATA_PORT - [0:00:29.094]
    0x3086, 0x1045,      // SEQ_DATA_PORT - [0:00:29.097]
    0x3086, 0x4027,      // SEQ_DATA_PORT - [0:00:29.100]
    0x3086, 0x0427,      // SEQ_DATA_PORT - [0:00:29.103]
    0x3086, 0x143D,     // SEQ_DATA_PORT - [0:00:29.106]
    0x3086, 0xFF3D,     // SEQ_DATA_PORT - [0:00:29.109]
    0x3086, 0xFF3D,     // SEQ_DATA_PORT - [0:00:29.114]
    0x3086, 0xEA62,    // SEQ_DATA_PORT - [0:00:29.117]
    0x3086, 0x2728,      // SEQ_DATA_PORT - [0:00:29.120]
    0x3086, 0x3627,      // SEQ_DATA_PORT - [0:00:29.123]
    0x3086, 0x083D,     // SEQ_DATA_PORT - [0:00:29.126]
    0x3086, 0x6444,      // SEQ_DATA_PORT - [0:00:29.129]
    0x3086, 0x2C2C,     // SEQ_DATA_PORT - [0:00:29.132]
    0x3086, 0x2C2C,     // SEQ_DATA_PORT - [0:00:29.135]
    0x3086, 0x4B01,     // SEQ_DATA_PORT - [0:00:29.138]
    0x3086, 0x432D,    // SEQ_DATA_PORT - [0:00:29.141]
    0x3086, 0x4643,      // SEQ_DATA_PORT - [0:00:29.145]
    0x3086, 0x1647,      // SEQ_DATA_PORT - [0:00:29.148]
    0x3086, 0x435F,      // SEQ_DATA_PORT - [0:00:29.150]
    0x3086, 0x4F50,      // SEQ_DATA_PORT - [0:00:29.153]
    0x3086, 0x2604,      // SEQ_DATA_PORT - [0:00:29.156]
    0x3086, 0x2684,      // SEQ_DATA_PORT - [0:00:29.159]
    0x3086, 0x2027,      // SEQ_DATA_PORT - [0:00:29.164]
    0x3086, 0xFC53,     // SEQ_DATA_PORT - [0:00:29.167]
    0x3086, 0x0D5C,     // SEQ_DATA_PORT - [0:00:29.170]
    0x3086, 0x0D57,     // SEQ_DATA_PORT - [0:00:29.173]
    0x3086, 0x5417,      // SEQ_DATA_PORT - [0:00:29.176]
    0x3086, 0x0955,      // SEQ_DATA_PORT - [0:00:29.179]
    0x3086, 0x5649,      // SEQ_DATA_PORT - [0:00:29.182]
    0x3086, 0x5307,      // SEQ_DATA_PORT - [0:00:29.185]
    0x3086, 0x5302,      // SEQ_DATA_PORT - [0:00:29.188]
    0x3086, 0x4D28,     // SEQ_DATA_PORT - [0:00:29.191]
    0x3086, 0x6C4C,     // SEQ_DATA_PORT - [0:00:29.195]
    0x3086, 0x0928,      // SEQ_DATA_PORT - [0:00:29.197]
    0x3086, 0x2C28,     // SEQ_DATA_PORT - [0:00:29.200]
    0x3086, 0x294E,      // SEQ_DATA_PORT - [0:00:29.203]
    0x3086, 0x5C09,     // SEQ_DATA_PORT - [0:00:29.206]
    0x3086, 0x6045,      // SEQ_DATA_PORT - [0:00:29.209]
    0x3086, 0x0045,      // SEQ_DATA_PORT - [0:00:29.211]
    0x3086, 0x8026,      // SEQ_DATA_PORT - [0:00:29.215]
    0x3086, 0xA627,     // SEQ_DATA_PORT - [0:00:29.218]
    0x3086, 0xF817,      // SEQ_DATA_PORT - [0:00:29.220]
    0x3086, 0x0227,      // SEQ_DATA_PORT - [0:00:29.223]
    0x3086, 0xFA5C,     // SEQ_DATA_PORT - [0:00:29.225]
    0x3086, 0x0B17,     // SEQ_DATA_PORT - [0:00:29.228]
    0x3086, 0x1826,      // SEQ_DATA_PORT - [0:00:29.231]
    0x3086, 0xA25C,     // SEQ_DATA_PORT - [0:00:29.233]
    0x3086, 0x0317,      // SEQ_DATA_PORT - [0:00:29.236]
    0x3086, 0x4427,      // SEQ_DATA_PORT - [0:00:29.239]
    0x3086, 0xF25F,      // SEQ_DATA_PORT - [0:00:29.241]
    0x3086, 0x2809,      // SEQ_DATA_PORT - [0:00:29.244]
    0x3086, 0x1714,      // SEQ_DATA_PORT - [0:00:29.246]
    0x3086, 0x2808,      // SEQ_DATA_PORT - [0:00:29.249]
    0x3086, 0x1616,      // SEQ_DATA_PORT - [0:00:29.252]
    0x3086, 0x4D1A,    // SEQ_DATA_PORT - [0:00:29.254]
    0x3086, 0x2683,      // SEQ_DATA_PORT - [0:00:29.257]
    0x3086, 0x1616,      // SEQ_DATA_PORT - [0:00:29.260]
    0x3086, 0x27FA,     // SEQ_DATA_PORT - [0:00:29.262]
    0x3086, 0x45A0,     // SEQ_DATA_PORT - [0:00:29.265]
    0x3086, 0x1707,      // SEQ_DATA_PORT - [0:00:29.268]
    0x3086, 0x27FB,     // SEQ_DATA_PORT - [0:00:29.270]
    0x3086, 0x1729,      // SEQ_DATA_PORT - [0:00:29.273]
    0x3086, 0x4580,      // SEQ_DATA_PORT - [0:00:29.275]
    0x3086, 0x1708,      // SEQ_DATA_PORT - [0:00:29.278]
    0x3086, 0x27FA,     // SEQ_DATA_PORT - [0:00:29.281]
    0x3086, 0x1728,      // SEQ_DATA_PORT - [0:00:29.283]
    0x3086, 0x5D17,     // SEQ_DATA_PORT - [0:00:29.286]
    0x3086, 0x0E26,      // SEQ_DATA_PORT - [0:00:29.289]
    0x3086, 0x8153,      // SEQ_DATA_PORT - [0:00:29.291]
    0x3086, 0x0117,      // SEQ_DATA_PORT - [0:00:29.294]
    0x3086, 0xE653,      // SEQ_DATA_PORT - [0:00:29.297]
    0x3086, 0x0217,      // SEQ_DATA_PORT - [0:00:29.299]
    0x3086, 0x1026,      // SEQ_DATA_PORT - [0:00:29.302]
    0x3086, 0x8326,      // SEQ_DATA_PORT - [0:00:29.304]
    0x3086, 0x8248,      // SEQ_DATA_PORT - [0:00:29.307]
    0x3086, 0x4D4E,     // SEQ_DATA_PORT - [0:00:29.310]
    0x3086, 0x2809,      // SEQ_DATA_PORT - [0:00:29.312]
    0x3086, 0x4C0B,     // SEQ_DATA_PORT - [0:00:29.315]
    0x3086, 0x6017,      // SEQ_DATA_PORT - [0:00:29.318]
    0x3086, 0x2027,      // SEQ_DATA_PORT - [0:00:29.320]
    0x3086, 0xF217,      // SEQ_DATA_PORT - [0:00:29.323]
    0x3086, 0x535F,      // SEQ_DATA_PORT - [0:00:29.325]
    0x3086, 0x2808,      // SEQ_DATA_PORT - [0:00:29.328]
    0x3086, 0x164D,     // SEQ_DATA_PORT - [0:00:29.331]
    0x3086, 0x1A16,     // SEQ_DATA_PORT - [0:00:29.334]
    0x3086, 0x1627,      // SEQ_DATA_PORT - [0:00:29.336]
    0x3086, 0xFA26,     // SEQ_DATA_PORT - [0:00:29.339]
    0x3086, 0x035C,     // SEQ_DATA_PORT - [0:00:29.341]
    0x3086, 0x0145,      // SEQ_DATA_PORT - [0:00:29.344]
    0x3086, 0x4027,      // SEQ_DATA_PORT - [0:00:29.346]
    0x3086, 0x9817,      // SEQ_DATA_PORT - [0:00:29.349]
    0x3086, 0x2A4A,    // SEQ_DATA_PORT - [0:00:29.352]
    0x3086, 0x0A43,     // SEQ_DATA_PORT - [0:00:29.354]
    0x3086, 0x160B,     // SEQ_DATA_PORT - [0:00:29.357]
    0x3086, 0x4327,      // SEQ_DATA_PORT - [0:00:29.359]
    0x3086, 0x9C45,     // SEQ_DATA_PORT - [0:00:29.364]
    0x3086, 0x6017,      // SEQ_DATA_PORT - [0:00:29.366]
    0x3086, 0x0727,      // SEQ_DATA_PORT - [0:00:29.369]
    0x3086, 0x9D17,     // SEQ_DATA_PORT - [0:00:29.371]
    0x3086, 0x2545,      // SEQ_DATA_PORT - [0:00:29.374]
    0x3086, 0x4017,     // SEQ_DATA_PORT - [0:00:29.376]
    0x3086, 0x0827,      // SEQ_DATA_PORT - [0:00:29.379]
    0x3086, 0x985D,     // SEQ_DATA_PORT - [0:00:29.381]
    0x3086, 0x2645,      // SEQ_DATA_PORT - [0:00:29.384]
    0x3086, 0x5C01,     // SEQ_DATA_PORT - [0:00:29.387]
    0x3086, 0x4B17,     // SEQ_DATA_PORT - [0:00:29.389]
    0x3086, 0x0A28,     // SEQ_DATA_PORT - [0:00:29.392]
    0x3086, 0x0853,      // SEQ_DATA_PORT - [0:00:29.395]
    0x3086, 0x0D52,     // SEQ_DATA_PORT - [0:00:29.397]
    0x3086, 0x5112,      // SEQ_DATA_PORT - [0:00:29.400]
    0x3086, 0x4460,      // SEQ_DATA_PORT - [0:00:29.402]
    0x3086, 0x184A,     // SEQ_DATA_PORT - [0:00:29.405]
    0x3086, 0x0343,      // SEQ_DATA_PORT - [0:00:29.407]
    0x3086, 0x1604,      // SEQ_DATA_PORT - [0:00:29.410]
    0x3086, 0x4316,      // SEQ_DATA_PORT - [0:00:29.412]
    0x3086, 0x5843,      // SEQ_DATA_PORT - [0:00:29.415]
    0x3086, 0x1659,      // SEQ_DATA_PORT - [0:00:29.418]
    0x3086, 0x4316,      // SEQ_DATA_PORT - [0:00:29.420]
    0x3086, 0x5A43,     // SEQ_DATA_PORT - [0:00:29.423]
    0x3086, 0x165B,     // SEQ_DATA_PORT - [0:00:29.425]
    0x3086, 0x4345,      // SEQ_DATA_PORT - [0:00:29.428]
    0x3086, 0x4027,      // SEQ_DATA_PORT - [0:00:29.431]
    0x3086, 0x9C45,     // SEQ_DATA_PORT - [0:00:29.433]
    0x3086, 0x6017,      // SEQ_DATA_PORT - [0:00:29.436]
    0x3086, 0x0727,      // SEQ_DATA_PORT - [0:00:29.438]
    0x3086, 0x9D17,     // SEQ_DATA_PORT - [0:00:29.441]
    0x3086, 0x2545,      // SEQ_DATA_PORT - [0:00:29.443]
    0x3086, 0x4017,      // SEQ_DATA_PORT - [0:00:29.446]
    0x3086, 0x1027,      // SEQ_DATA_PORT - [0:00:29.449]
    0x3086, 0x9817,      // SEQ_DATA_PORT - [0:00:29.451]
    0x3086, 0x2022,      // SEQ_DATA_PORT - [0:00:29.454]
    0x3086, 0x4B12,     // SEQ_DATA_PORT - [0:00:29.456]
    0x3086, 0x442C,     // SEQ_DATA_PORT - [0:00:29.459]
    0x3086, 0x2C2C,     // SEQ_DATA_PORT - [0:00:29.461]
    0x3086, 0x2C00,     // SEQ_DATA_PORT - [0:00:29.464]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.467]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.469]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.472]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.474]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.477]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.479]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.482]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.485]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.487]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.490]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.492]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.496]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.499]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.501]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.504]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.506]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.509]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.511]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.514]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.517]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.519]
    0x3086, 0x0000,     // SEQ_DATA_PORT - [0:00:29.522]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.524]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.527]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.529]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.532]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.535]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.537]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.540]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.542]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.546]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.549]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.551]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.554]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.556]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.559]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.561]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.564]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.567]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.569]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.572]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.574]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.577]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.579]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.582]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.585]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.588]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.590]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.593]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.596]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.598]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.601]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.603]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.606]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.608]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.611]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.614]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.616]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.619]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.621]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.624]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.626]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.629]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.632]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.634]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.637]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.640]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.643]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.645]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.648]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.651]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.653]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.656]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.659]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.662]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.665]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.668]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.670]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.673]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.676]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.680]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.682]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.685]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.688]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.690]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.693]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.696]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.699]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.701]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.704]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.706]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.709]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.712]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.714]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.717]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.719]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.722]
    0x3086, 0x0000,      // SEQ_DATA_PORT - [0:00:29.724]
};

#if 0
void ____Sensor_Res_OPR_Table____(){ruturn;} //dummy
#endif

MMP_USHORT SNR_AR0330_MIPI_2_LANE_Reg_2304x1296[] = 
{
	0x3030, (82+1), 	// PLL_MULTIPLIER  @ 490Mbps/lane
    0xFFFF, 100,
    
	//ARRAY READOUT SETTINGS
	// +2 for X axis dead pixel.
	0x3004, 0+2,		// X_ADDR_START
	0x3008, 2311+2,	    // X_ADDR_END
	0x3002, 122,    	// Y_ADDR_START
	0x3006, 1421,	    // Y_ADDR_END

	//Sub-sampling
	0x30A2, 1,			// X_ODD_INCREMENT
	0x30A6, 1,			// Y_ODD_INCREMENT
    #if SENSOR_ROTATE_180
	0x3040, 0xC000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    #else
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	#endif
    
    //Frame-Timing
	0x300C, 1250,       // LINE_LENGTH_PCK
	0x300A, 1312,		// FRAME_LENGTH_LINES
	0x3014, 0,		    // FINE_INTEGRATION_TIME
	0x3012, 152,		// Coarse_Integration_Time
	0x3042, 0,			// EXTRA_DELAY
	0x30BA, 0x2C,

	0x301A, 0x000C,     // Grouped Parameter Hold = 0x0
};

#if 0
void ____Sensor_Res_OPR_Table____(){ruturn;} //dummy
#endif

MMP_USHORT SNR_AR0330_MIPI_2_LANE_Reg_2304x1296_25P[] = 
{
	0x3030, (82+1), 	// PLL_MULTIPLIER  @ 490Mbps/lane
       0xFFFF, 100,
    
	//ARRAY READOUT SETTINGS
	// +2 for X axis dead pixel.
	0x3004, 0+2,		// X_ADDR_START
	0x3008, 2311+2,	    // X_ADDR_END
	0x3002, 122,    	// Y_ADDR_START
	0x3006, 1421,	    // Y_ADDR_END

	//Sub-sampling
	0x30A2, 1,			// X_ODD_INCREMENT
	0x30A6, 1,			// Y_ODD_INCREMENT
    #if SENSOR_ROTATE_180
	0x3040, 0xC000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    #else
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	#endif
    
    //Frame-Timing
	0x300C, 1480,       // LINE_LENGTH_PCK
	0x300A, 1620,		// FRAME_LENGTH_LINES
	0x3014, 0,		    // FINE_INTEGRATION_TIME
	0x3012, 152,		// Coarse_Integration_Time
	0x3042, 0,			// EXTRA_DELAY
	0x30BA, 0x2C,

	0x301A, 0x000C,     // Grouped Parameter Hold = 0x0
};

MMP_USHORT SNR_AR0330_MIPI_2_LANE_Reg_2304x1536[] = 
{
	0x3030, 82, 	    // PLL_MULTIPLIER  @ 490Mbps/lane

	//ARRAY READOUT SETTINGS
	0x3004, 0+2,		// X_ADDR_START
	0x3008, 2311+2,	    // X_ADDR_END
	0x3002, 0 + 6,    	// Y_ADDR_START
	0x3006, 0 + 6 + (1536 + 4 - 1),	// Y_ADDR_END

	//Sub-sampling
	0x30A2, 1,			// X_ODD_INCREMENT
	0x30A6, 1,			// Y_ODD_INCREMENT
    #if SENSOR_ROTATE_180
	0x3040, 0xC000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    #else
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	#endif
	
	//Frame-Timing
	0x300C, 1250,       // LINE_LENGTH_PCK
	0x300A, 1536+4,     // FRAME_LENGTH_LINES
	0x3014, 0,		    // FINE_INTEGRATION_TIME
	0x3012, 1000,		// Coarse_Integration_Time
	0x3042, 0,			// EXTRA_DELAY
	0x30BA, 0x2C,

	0x301A, 0x000C,     // Grouped Parameter Hold = 0x0
};

MMP_USHORT SNR_AR0330_MIPI_2_LANE_Reg_1920x1080[] = 
{
	0x3030, 82, 	    // PLL_MULTIPLIER  @ 490Mbps/lane

	//ARRAY READOUT SETTINGS
	0x3004, 192,		// X_ADDR_START
	0x3008, 2127,		// X_ADDR_END
	0x3002, 232,    	// Y_ADDR_START
	0x3006, 1319,		// Y_ADDR_END

	//Sub-sampling
	0x30A2, 1,			// X_ODD_INCREMENT
	0x30A6, 1,			// Y_ODD_INCREMENT
    #if SENSOR_ROTATE_180
	0x3040, 0xC000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    #else
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	#endif
    
    //Frame-Timing
	0x300C, 1260,		// LINE_LENGTH_PCK
	0x300A, 1270,		// FRAME_LENGTH_LINES
	0x3014, 0,		    // FINE_INTEGRATION_TIME
	0x3012, 1000,		// Coarse_Integration_Time
	0x3042, 0,			// EXTRA_DELAY
	0x30BA, 0x2C,

	0x301A, 0x000C,     // Grouped Parameter Hold = 0x0
};

MMP_USHORT SNR_AR0330_MIPI_2_LANE_Reg_1152x648[] = 
{
	0x3030, 82, 	    // PLL_MULTIPLIER @ 490Mbps/lane

    //ARRAY READOUT SETTINGS
	0x3004, 2+0,		// X_ADDR_START
	0x3008, 2+ (1152 + 4 )* 2 - 1,  // X_ADDR_END
	0x3002, 122,   		// Y_ADDR_START
	0x3006, 122 + (648 + 4) * 2 - 1,// Y_ADDR_END

    //Sub-sampling
    0x30A2, 3,			// X_ODD_INCREMENT
    0x30A6, 3,			// Y_ODD_INCREMENT
    #if SENSOR_ROTATE_180
	0x3040, 0xF000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    #else
	0x3040, 0x3000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	#endif

    //Frame-Timing
    0x300C, 1232,       // LINE_LENGTH_PCK
    0x300A, 672,		// FRAME_LENGTH_LINES
    0x3014, 0,			// FINE_INTEGRATION_TIME
    0x3012, 552,		// Coarse_Integration_Time
    0x3042, 0,			// EXTRA_DELAY
    0x30BA, 0x2C,	    // Digital_Ctrl_Adc_High_Speed
  
    0x301A, 0x000C,     // Grouped Parameter Hold = 0x0
};

MMP_USHORT SNR_AR0330_Reg_1280x720_60fps_Customer[] = 
{
	0x3004,   384,      // X_ADDR_START
	0x3008,   384+1283, // X_ADDR_END
	0x3002,   408,      // Y_ADDR_START
	0x3006,   408+723,  // Y_ADDR_END
	0x30A2,      1,     // X_ODD_INC
	0x30A6,      1,     // Y_ODD_INC
    #if SENSOR_ROTATE_180
	0x3040, 0xC000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	0x3040, 0xC000,	    // READ_MODE
	0x3040, 0xC000,	    // READ_MODE
    #else
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	0x3040, 0x0000,     // READ_MODE
	0x3040, 0x0000,     // READ_MODE
	#endif
	0x300C,   1128,     // LINE_LENGTH_PCK
	0x300A,    788,     // FRAME_LENGTH_LINES
	0x3014,      0,     // FINE_INTEGRATION_TIME
	0x3012,    785,     // COARSE_INTEGRATION_TIME
	0x3042,    968,     // EXTRA_DELAY
	0x30BA, 0x002C,     // DIGITAL_CTRL

    0x301A, 0x000C,     // Grouped Parameter Hold = 0x0

//	0x301A, 0x10D8,  // RESET_REGISTER
//	0xFFFF, 100,
//	0x3088, 0x80BA,  // SEQ_CTRL_PORT
//	0x3086, 0x0253,  // SEQ_DATA_PORT
//	0x301A, 0x10DC,  // RESET_REGISTER
//	0x301A, 0x10DC,  // RESET_REGISTER
//	0xFFFF,    100   // delay = 100
};

#endif      // SENSOR_IF

#if (SENSOR_IF == SENSOR_IF_MIPI_4_LANE)
#if (SUPPORT_EIS)
MMPF_SENSOR_RESOLUTION m_SensorRes = 
{
	17,				// ubSensorModeNum
	16,				// ubDefPreviewMode
	0,				// ubDefCaptureMode
	2200,           // usPixelSize
//  Mode0   Mode1	Mode2   Mode3   Mode4	Mode5   Mode6	Mode7   Mode8   Mode9	Mode10	Mode11	Mode12	Mode13	Mode14	Mode15	Mode16
    {1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1	},	// usVifGrabStX
    {1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1	},	// usVifGrabStY
    {2266,	2266,	2266,	2266,	1132,	1928,	1928,	1928,	1928,	964,	2184,	2184,	1928,	1928,	2048,	1928,	2304+2},// usVifGrabW
    {1280,	1280,	1280,	1280,	640,	1096,	1096,	1096,	1096,	544,	1232,	1232,	1088,	1088,	1536,	1096,	1296+2},// usVifGrabH
	#if (CHIP == MCR_V2)
    {1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1	},	// usBayerInGrabX
    {1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1	},	// usBayerInGrabY
    {10, 	10,		10,		18,		12,		8, 		8,		8,		8,		18,		8,		8,		8,		8,		8,		8,		2	},  // usBayerDummyInX
    {11,	11,		11,		8,		6,		16,		16,		16,		16,		8,		8,		8,		8,		8,		6,		16,		2	},  // usBayerDummyInY
    {1920,  1280,	1280,	1696,	1120,	1920,  	1280,	1280,	1920,	946,	2176,	2176,	1920,	1920,	2040,	1920,	2304},	// usBayerOutW
    {1080,  720,	720,	960,	634,	1080,  	720,	720,	1080,	536,	1224,	1224,	1080,	1080,	1530,	1080,	1296},	// usBayerOutH
	#endif
    {1920,  1280,	1280,	1696,	1120,	1920,  	1280,	1280,	1920,	946,	2176,	2176,	1920,	1920,	2040,	1920,	1920/*2304*/},	// usScalInputW
    {1080,  720,	720,	960,	634,	1080,  	720,	720,	1080,	536,	1224,	1224,	1080,	1080,	1530,	1080,	1080/*1296*/},	// usScalInputH
    {300,   600,	300,	300,	1200,	300,    600,	300,	300,	1200,	300,	150,	300,	150,	300,	600,	300	},	// usTargetFpsx10
    {2634,	1320,	2634,	2634,	711,	2634,	1321,	2634,	2634,	711,	1242,	1242,	1242,	1242,	1548,	1321,	2630},	// usVsyncLine
    {1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1	},  // ubWBinningN
    {1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1	},  // ubWBinningN
    {1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1	},  // ubWBinningN
    {1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1	},  // ubWBinningN
    {0xFF,	0xFF,	0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF},  // ubCustIQmode
    {0xFF,	0xFF,	0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF}   // ubCustAEmode
};
#else
MMPF_SENSOR_RESOLUTION m_SensorRes = 
{
	17,				// ubSensorModeNum
	0,				// ubDefPreviewMode
	0,				// ubDefCaptureMode
	2200,           // usPixelSize
//  Mode0   Mode1	Mode2   Mode3   Mode4	Mode5   Mode6	Mode7   Mode8   Mode9	Mode10	Mode11	Mode12	Mode13	Mode14	Mode15	Mode16
    {1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1	},	// usVifGrabStX
    {1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1	},	// usVifGrabStY
    {2266,	2266,	2266,	2266,	1132,	1928,	1928,	1928,	1928,	964,	2184,	2184,	1928,	1928,	2048,	1928,	2304+2},// usVifGrabW
    {1280,	1280,	1280,	1280,	640,	1096,	1096,	1096,	1096,	544,	1232,	1232,	1088,	1088,	1536,	1096,	1296+2},// usVifGrabH
	#if (CHIP == MCR_V2)
    {1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1	},	// usBayerInGrabX
    {1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1	},	// usBayerInGrabY
    {10, 	10,		10,		18,		12,		8, 		8,		8,		8,		18,		8,		8,		8,		8,		8,		8,		2	},  // usBayerDummyInX
    {11,	11,		11,		8,		6,		16,		16,		16,		16,		8,		8,		8,		8,		8,		6,		16,		2	},  // usBayerDummyInY
    {1920,  1280,	1280,	1696,	1120,	1920,  	1280,	1280,	1920,	946,	2176,	2176,	1920,	1920,	2040,	1920,	2304},	// usBayerOutW
    {1080,  720,	720,	960,	634,	1080,  	720,	720,	1080,	536,	1224,	1224,	1080,	1080,	1530,	1080,	1296},	// usBayerOutH
	#endif
    {1920,  1280,	1280,	1696,	1120,	1920,  	1280,	1280,	1920,	946,	2176,	2176,	1920,	1920,	2040,	1920,	2304},	// usScalInputW
    {1080,  720,	720,	960,	634,	1080,  	720,	720,	1080,	536,	1224,	1224,	1080,	1080,	1530,	1080,	1296},	// usScalInputH
    {300,   600,	300,	300,	1200,	300,    600,	300,	300,	1200,	300,	150,	300,	150,	300,	600,	300	},	// usTargetFpsx10
    {2634,	1320,	2634,	2634,	711,	2634,	1321,	2634,	2634,	711,	1242,	1242,	1242,	1242,	1548,	1321,	2630},	// usVsyncLine
    {1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1	},  // ubWBinningN
    {1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1	},  // ubWBinningN
    {1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1	},  // ubWBinningN
    {1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1	},  // ubWBinningN
    {0xFF,	0xFF,	0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF},  // ubCustIQmode
    {0xFF,	0xFF,	0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF}   // ubCustAEmode
};
#endif

#if 0
void ____Sensor_Init_OPR_Table____(){ruturn;} //dummy
#endif

MMP_USHORT SNR_AR0330_Reg_Init_OTPM_V1_Customer[] = 
{
	//Table 2. Recommend default register and Sequencer
	0x30BA, 0x2C  , 								   
	0x30FE, 0x0080, 								   
	0x31E0, 0x0003, 								   
	0x3ECE, 0xFF  , 								   
	0x3ED0, 0xE4F6, 								   
	0x3ED2, 0x0146, 								   
	0x3ED4, 0x8F6C, 								   
	0x3ED6, 0x66CC, 								   
	0x3ED8, 0x8C42, 								   
	0x3EDA, 0x8822, 								   
	0x3EDC, 0x2222, 								   
	0x3EDE, 0x22C0, 								   
	0x3EE0, 0x1500, 								   
	0x3EE6, 0x0080, 								   
	0x3EE8, 0x2027, 								   
	0x3EEA, 0x001D, 								   
	0x3F06, 0x046A, 								   
	0x305E, 0x00A0, 								   
													   
	//Sequencer A	
	0x301A, 0x0058,     // Disable streaming
	SNR_REG_DELAY, 10,
	
	0x3088, 0x8000,
	0x3086, 0x4540,
	0x3086, 0x6134,
	0x3086, 0x4A31,
	0x3086, 0x4342,
	0x3086, 0x4560,
	0x3086, 0x2714,
	0x3086, 0x3DFF,
	0x3086, 0x3DFF,
	0x3086, 0x3DEA,
	0x3086, 0x2704,
	0x3086, 0x3D10,
	0x3086, 0x2705,
	0x3086, 0x3D10,
	0x3086, 0x2715,
	0x3086, 0x3527,
	0x3086, 0x053D,
	0x3086, 0x1045,
	0x3086, 0x4027,
	0x3086, 0x0427,
	0x3086, 0x143D,
	0x3086, 0xFF3D,
	0x3086, 0xFF3D,
	0x3086, 0xEA62,
	0x3086, 0x2728,
	0x3086, 0x3627,
	0x3086, 0x083D,
	0x3086, 0x6444,
	0x3086, 0x2C2C,
	0x3086, 0x2C2C,
	0x3086, 0x4B01,
	0x3086, 0x432D,
	0x3086, 0x4643,
	0x3086, 0x1647,
	0x3086, 0x435F,
	0x3086, 0x4F50,
	0x3086, 0x2604,
	0x3086, 0x2684,
	0x3086, 0x2027,
	0x3086, 0xFC53,
	0x3086, 0x0D5C,
	0x3086, 0x0D60,
	0x3086, 0x5754,
	0x3086, 0x1709,
	0x3086, 0x5556,
	0x3086, 0x4917,
	0x3086, 0x145C,
	0x3086, 0x0945,
	0x3086, 0x0045,
	0x3086, 0x8026,
	0x3086, 0xA627,
	0x3086, 0xF817,
	0x3086, 0x0227,
	0x3086, 0xFA5C,
	0x3086, 0x0B5F,
	0x3086, 0x5307,
	0x3086, 0x5302,
	0x3086, 0x4D28,
	0x3086, 0x6C4C,
	0x3086, 0x0928,
	0x3086, 0x2C28,
	0x3086, 0x294E,
	0x3086, 0x1718,
	0x3086, 0x26A2,
	0x3086, 0x5C03,
	0x3086, 0x1744,
	0x3086, 0x2809,
	0x3086, 0x27F2,
	0x3086, 0x1714,
	0x3086, 0x2808,
	0x3086, 0x164D,
	0x3086, 0x1A26,
	0x3086, 0x8317,
	0x3086, 0x0145,
	0x3086, 0xA017,
	0x3086, 0x0727,
	0x3086, 0xF317,
	0x3086, 0x2945,
	0x3086, 0x8017,
	0x3086, 0x0827,
	0x3086, 0xF217,
	0x3086, 0x285D,
	0x3086, 0x27FA,
	0x3086, 0x170E,
	0x3086, 0x2681,
	0x3086, 0x5300,
	0x3086, 0x17E6,
	0x3086, 0x5302,
	0x3086, 0x1710,
	0x3086, 0x2683,
	0x3086, 0x2682,
	0x3086, 0x4827,
	0x3086, 0xF24D,
	0x3086, 0x4E28,
	0x3086, 0x094C,
	0x3086, 0x0B17,
	0x3086, 0x6D28,
	0x3086, 0x0817,
	0x3086, 0x014D,
	0x3086, 0x1A17,
	0x3086, 0x0126,
	0x3086, 0x035C,
	0x3086, 0x0045,
	0x3086, 0x4027,
	0x3086, 0x9017,
	0x3086, 0x2A4A,
	0x3086, 0x0A43,
	0x3086, 0x160B,
	0x3086, 0x4327,
	0x3086, 0x9445,
	0x3086, 0x6017,
	0x3086, 0x0727,
	0x3086, 0x9517,
	0x3086, 0x2545,
	0x3086, 0x4017,
	0x3086, 0x0827,
	0x3086, 0x905D,
	0x3086, 0x2808,
	0x3086, 0x530D,
	0x3086, 0x2645,
	0x3086, 0x5C01,
	0x3086, 0x2798,
	0x3086, 0x4B12,
	0x3086, 0x4452,
	0x3086, 0x5117,
	0x3086, 0x0260,
	0x3086, 0x184A,
	0x3086, 0x0343,
	0x3086, 0x1604,
	0x3086, 0x4316,
	0x3086, 0x5843,
	0x3086, 0x1659,
	0x3086, 0x4316,
	0x3086, 0x5A43,
	0x3086, 0x165B,
	0x3086, 0x4327,
	0x3086, 0x9C45,
	0x3086, 0x6017,
	0x3086, 0x0727,
	0x3086, 0x9D17,
	0x3086, 0x2545,
	0x3086, 0x4017,
	0x3086, 0x1027,
	0x3086, 0x9817,
	0x3086, 0x2022,
	0x3086, 0x4B12,
	0x3086, 0x442C,
	0x3086, 0x2C2C,
	0x3086, 0x2C00,
									   
	0x301A, 0x0004, 								   
													   
	//Initialization								   
	0x301A, 	0x0059, 								 
	SNR_REG_DELAY, 10,								 
	0x3052, 0xA114, // fix low temp OTPM wrong issue   
	0x304A, 0x0070, // fix low temp OTPM wrong issue   
	SNR_REG_DELAY, 10,								 
	0x31AE, 	0x0204, 								 
	0x301A, 	0x0058, 								 
	SNR_REG_DELAY, 35,								 
	0x3064, 	0x1802, 								 
	0x3078, 	0x0001, 								 
	0x31E0, 	0x0203, 								 
														 
	// Configure PLL 								   
	0x302A, 5	  , 									 
	0x302C, 1	  , 								   
	0x302E, 2	  , 								   
	0x3030, 41	  , 								   
	0x3036, 10	  , 								   
	0x3038, 1	  , 								   
	0x31AC, 0x0A0A, 								   
	0x31B0, 47	  , 								   
	0x31B2, 19	  , 								   
	0x31B4, 0x3C44, 								   
	0x31B6, 0x314D, 								   
	0x31B8, 0x208A, 								   
	0x31BA, 0x0207, 								   
	0x31BC, 0x8005, 								   
	0x31BE, 0x2003, 								   
};

MMP_USHORT SNR_AR0330_Reg_Init_OTPM_V2_Customer[] = 
{
	//Table 2. Recommend default register and Sequencer
	0x30BA, 0x2C  , 								   
	0x30FE, 0x0080, 								   
	0x31E0, 0x0003, 								   
	0x3ECE, 0xFF  , 								   
	0x3ED0, 0xE4F6, 								   
	0x3ED2, 0x0146, 								   
	0x3ED4, 0x8F6C, 								   
	0x3ED6, 0x66CC, 								   
	0x3ED8, 0x8C42, 								   
	0x3EDA, 0x889B, 								   
	0x3EDC, 0x8863, 								   
	0x3EDE, 0xAA04, 								   
	0x3EE0, 0x15F0, 								   
	0x3EE6, 0x008C, 								   
	0x3EE8, 0x2024, 								   
	0x3EEA, 0xFF1F, 								   
	0x3F06, 0x046A, 								   
	0x305E, 0x00A0, 								   
													   
	//Sequencer B	
	0x301A, 0x0058,     // Disable streaming
	SNR_REG_DELAY, 10,
	0x3088, 0x8000, 								   
	0x3086, 0x4A03, 								   
	0x3086, 0x4316, 								   
	0x3086, 0x0443, 								   
	0x3086, 0x1645, 								   
	0x3086, 0x4045, 								   
	0x3086, 0x6017, 								   
	0x3086, 0x2045, 								   
	0x3086, 0x404B, 								   
	0x3086, 0x1244, 								   
	0x3086, 0x6134, 								   
	0x3086, 0x4A31, 								   
	0x3086, 0x4342, 								   
	0x3086, 0x4560, 								   
	0x3086, 0x2714, 								   
	0x3086, 0x3DFF, 								   
	0x3086, 0x3DFF, 								   
	0x3086, 0x3DEA, 								   
	0x3086, 0x2704, 								   
	0x3086, 0x3D10, 								   
	0x3086, 0x2705, 								   
	0x3086, 0x3D10, 								   
	0x3086, 0x2715, 								   
	0x3086, 0x3527, 								   
	0x3086, 0x053D, 								   
	0x3086, 0x1045, 								   
	0x3086, 0x4027, 								   
	0x3086, 0x0427, 								   
	0x3086, 0x143D, 								   
	0x3086, 0xFF3D, 								   
	0x3086, 0xFF3D, 								   
	0x3086, 0xEA62, 								   
	0x3086, 0x2728, 								   
	0x3086, 0x3627, 								   
	0x3086, 0x083D, 								   
	0x3086, 0x6444, 								   
	0x3086, 0x2C2C, 								   
	0x3086, 0x2C2C, 								   
	0x3086, 0x4B01, 								   
	0x3086, 0x432D, 								   
	0x3086, 0x4643, 								   
	0x3086, 0x1647, 								   
	0x3086, 0x435F, 								   
	0x3086, 0x4F50, 								   
	0x3086, 0x2604, 								   
	0x3086, 0x2684, 								   
	0x3086, 0x2027, 								   
	0x3086, 0xFC53, 								   
	0x3086, 0x0D5C, 								   
	0x3086, 0x0D57, 								   
	0x3086, 0x5417, 								   
	0x3086, 0x0955, 								   
	0x3086, 0x5649, 								   
	0x3086, 0x5307, 								   
	0x3086, 0x5302, 								   
	0x3086, 0x4D28, 								   
	0x3086, 0x6C4C, 								   
	0x3086, 0x0928, 								   
	0x3086, 0x2C28, 								   
	0x3086, 0x294E, 								   
	0x3086, 0x5C09, 								   
	0x3086, 0x6045, 								   
	0x3086, 0x0045, 								   
	0x3086, 0x8026, 								   
	0x3086, 0xA627, 								   
	0x3086, 0xF817, 								   
	0x3086, 0x0227, 								   
	0x3086, 0xFA5C, 								   
	0x3086, 0x0B17, 								   
	0x3086, 0x1826, 								   
	0x3086, 0xA25C, 								   
	0x3086, 0x0317, 								   
	0x3086, 0x4427, 								   
	0x3086, 0xF25F, 								   
	0x3086, 0x2809, 								   
	0x3086, 0x1714, 								   
	0x3086, 0x2808, 								   
	0x3086, 0x1701, 								   
	0x3086, 0x4D1A, 								   
	0x3086, 0x2683, 								   
	0x3086, 0x1701, 								   
	0x3086, 0x27FA, 								   
	0x3086, 0x45A0, 								   
	0x3086, 0x1707, 								   
	0x3086, 0x27FB, 								   
	0x3086, 0x1729, 								   
	0x3086, 0x4580, 								   
	0x3086, 0x1708, 								   
	0x3086, 0x27FA, 								   
	0x3086, 0x1728, 								   
	0x3086, 0x5D17, 								   
	0x3086, 0x0E26, 								   
	0x3086, 0x8153, 								   
	0x3086, 0x0117, 								   
	0x3086, 0xE653, 								   
	0x3086, 0x0217, 								   
	0x3086, 0x1026, 								   
	0x3086, 0x8326, 								   
	0x3086, 0x8248, 								   
	0x3086, 0x4D4E, 								   
	0x3086, 0x2809, 								   
	0x3086, 0x4C0B, 								   
	0x3086, 0x6017, 								   
	0x3086, 0x2027, 								   
	0x3086, 0xF217, 								   
	0x3086, 0x535F, 								   
	0x3086, 0x2808, 								   
	0x3086, 0x164D, 								   
	0x3086, 0x1A17, 								   
	0x3086, 0x0127, 								   
	0x3086, 0xFA26, 								   
	0x3086, 0x035C, 								   
	0x3086, 0x0145, 								   
	0x3086, 0x4027, 								   
	0x3086, 0x9817, 								   
	0x3086, 0x2A4A, 								   
	0x3086, 0x0A43, 								   
	0x3086, 0x160B, 								   
	0x3086, 0x4327, 								   
	0x3086, 0x9C45, 								   
	0x3086, 0x6017, 								   
	0x3086, 0x0727, 								   
	0x3086, 0x9D17, 								   
	0x3086, 0x2545, 								   
	0x3086, 0x4017, 								   
	0x3086, 0x0827, 								   
	0x3086, 0x985D, 								   
	0x3086, 0x2645, 								   
	0x3086, 0x5C01, 								   
	0x3086, 0x4B17, 								   
	0x3086, 0x0A28, 								   
	0x3086, 0x0853, 								   
	0x3086, 0x0D52, 								   
	0x3086, 0x5112, 								   
	0x3086, 0x4460, 								   
	0x3086, 0x184A, 								   
	0x3086, 0x0343, 								   
	0x3086, 0x1604, 								   
	0x3086, 0x4316, 								   
	0x3086, 0x5843, 								   
	0x3086, 0x1659, 								   
	0x3086, 0x4316, 								   
	0x3086, 0x5A43, 								   
	0x3086, 0x165B, 								   
	0x3086, 0x4345, 								   
	0x3086, 0x4027, 								   
	0x3086, 0x9C45, 								   
	0x3086, 0x6017, 								   
	0x3086, 0x0727, 								   
	0x3086, 0x9D17, 								   
	0x3086, 0x2545, 								   
	0x3086, 0x4017, 								   
	0x3086, 0x1027, 								   
	0x3086, 0x9817, 								   
	0x3086, 0x2022, 								   
	0x3086, 0x4B12, 								   
	0x3086, 0x442C, 								   
	0x3086, 0x2C2C, 								   
	0x3086, 0x2C00, 								   
	0x3086, 0x0000, 								   
	0x301A, 0x0004, 								   
													   
	//Initialization								   
	0x301A, 	0x0059, 								 
	SNR_REG_DELAY, 10,								 
	0x3052, 0xA114, // fix low temp OTPM wrong issue   
	0x304A, 0x0070, // fix low temp OTPM wrong issue   
	SNR_REG_DELAY, 10,								 
	0x31AE, 	0x0204, 								 
	0x301A, 	0x0058, 								 
	SNR_REG_DELAY, 35,								 
	0x3064, 	0x1802, 								 
	0x3078, 	0x0001, 								 
	0x31E0, 	0x0203, 								 
														 
	// Configure PLL 								   
	0x302A, 5	  , 									 
	0x302C, 1	  , 								   
	0x302E, 2	  , 								   
	0x3030, 41	  , 								   
	0x3036, 10	  , 								   
	0x3038, 1	  , 								   
	0x31AC, 0x0A0A, 								   
	0x31B0, 47	  , 								   
	0x31B2, 19	  , 								   
	0x31B4, 0x3C44, 								   
	0x31B6, 0x314D, 								   
	0x31B8, 0x208A, 								   
	0x31BA, 0x0207, 								   
	0x31BC, 0x8005, 								   
	0x31BE, 0x2003, 								   
};

MMP_USHORT SNR_AR0330_Reg_Init_OTPM_V3_Customer[] = 
{
	//Table 2. Recommend default register and Sequencer
	0x31E0, 0x0003,
	0x3ED2, 0x0146,
	0x3ED4, 0x8F6C,
	0x3ED6, 0x66CC,
    0x3ED8, 0x8C42,
	0x3EDA, 0x88BC,
	0x3EDC, 0xAA63,
	0x305E, 0x00A0,
	0x3046, 0x4038,
	0x3048, 0x8480,
	
	//Sequencer Patch 1
	0x301A, 0x0058,     // Disable streaming
	SNR_REG_DELAY, 10,		
	0x3088, 0x800C,
    0x3086, 0x2045,
    0x301A, 0x0004, 
	
	//Initialization								   
	0x301A, 	0x0059, 								 
	SNR_REG_DELAY, 10,								 
	0x3052, 0xA114, // fix low temp OTPM wrong issue   
	0x304A, 0x0070, // fix low temp OTPM wrong issue   
	SNR_REG_DELAY, 10,								 
	0x31AE, 	0x0204, 								 
	0x301A, 	0x0058, 								 
	SNR_REG_DELAY, 35,								 
	0x3064, 	0x1802, 								 
	0x3078, 	0x0001, 								 
	0x31E0, 	0x0203, 
											 
	// Configure PLL 								   
	0x302A, 5	  , 									 
	0x302C, 1	  , 								   
	0x302E, 2	  , 								   
	0x3030, 41	  , 								   
	0x3036, 10	  , 								   
	0x3038, 1	  , 								   
	0x31AC, 0x0A0A, 								   
	0x31B0, 47	  , 								   
	0x31B2, 19	  , 								   
	0x31B4, 0x3C44, 								   
	0x31B6, 0x314D, 								   
	0x31B8, 0x208A, 								   
	0x31BA, 0x0207, 								   
	0x31BC, 0x8005, 								   
	0x31BE, 0x2003, 	
};

MMP_USHORT SNR_AR0330_Reg_Init_OTPM_V4_Customer[] = 
{
	//Table 2. Recommend default register and Sequencer
	0x31E0, 0x0003,
	0x3ED2, 0x0146,
	0x3ED6, 0x66CC,
    0x3ED8, 0x8C42,
	0x3EDA, 0x88BC,
	0x3EDC, 0xAA63,
	0x305E, 0x00A0,
	0x3046, 0x4038,
	0x3048, 0x8480,
	
	//Initialization								   
	0x301A, 	0x0059, 								 
	SNR_REG_DELAY, 10,								 
	0x3052, 0xA114, // fix low temp OTPM wrong issue   
	0x304A, 0x0070, // fix low temp OTPM wrong issue   
	SNR_REG_DELAY, 10,								 
	0x31AE, 	0x0204, 								 
	0x301A, 	0x0058, 								 
	SNR_REG_DELAY, 35,								 
	0x3064, 	0x1802, 								 
	0x3078, 	0x0001, 								 
	0x31E0, 	0x0203, 								 
														 
	//configure PLL 								   
	0x302A, 5	  , 									 
	0x302C, 1	  , 								   
	0x302E, 2	  , 								   
	0x3030, 41	  , 								   
	0x3036, 10	  , 								   
	0x3038, 1	  , 								   
	0x31AC, 0x0A0A, 								   
	0x31B0, 47	  , 								   
	0x31B2, 19	  , 								   
	0x31B4, 0x3C44, 								   
	0x31B6, 0x314D, 								   
	0x31B8, 0x208A, 								   
	0x31BA, 0x0207, 								   
	0x31BC, 0x8005, 								   
	0x31BE, 0x2003, 	
};

MMP_USHORT SNR_AR0330_Reg_Init_OTPM_V5_Customer[] = 
{
	//Table 2. Recommend default register and Sequencer
	0x3ED2, 0x0146,
	0x3EDA, 0x88BC,
	0x3EDC, 0xAA63,
	0x305E, 0x00A0,
	0x3046, 0x4038,
	0x3048, 0x8480,
	
	//Initialization								   
	0x301A, 	0x0059, 								 
	SNR_REG_DELAY, 10,								 
	0x3052, 0xA114, // fix low temp OTPM wrong issue   
	0x304A, 0x0070, // fix low temp OTPM wrong issue   
	SNR_REG_DELAY, 10,								 
	0x31AE, 	0x0204, 								 
	0x301A, 	0x0058, 								 
	SNR_REG_DELAY, 35,								 
	0x3064, 	0x1802, 								 
	0x3078, 	0x0001, 								 
	0x31E0, 	0x0203, 								 
														 
	//configure PLL 								   
	0x302A, 5	  , 									 
	0x302C, 1	  , 								   
	0x302E, 2	  , 								   
	0x3030, 41	  , 								   
	0x3036, 10	  , 								   
	0x3038, 1	  , 								   
	0x31AC, 0x0A0A, 								   
	0x31B0, 47	  , 								   
	0x31B2, 19	  , 								   
	0x31B4, 0x3C44, 								   
	0x31B6, 0x314D, 								   
	0x31B8, 0x208A, 								   
	0x31BA, 0x0207, 								   
	0x31BC, 0x8005, 								   
	0x31BE, 0x2003, 	
};

#if 0
void ____Sensor_Res_OPR_Table____(){ruturn;} //dummy
#endif

//Mode0
//for 1080p30(W) / 720p30(W) / WVGA30(W)
//Interface	MIPI 4Lane
//sensor output	2266x1280
//frame rate	29.97
//Sampling	no sampling
//PLL setting	PLL-01
ISP_UINT16 SNR_AR0330_Reg_2266x1280_30fps[] = 
{
	0x3004, 26	  , 		//X_ADDR_START											
	0x3008, 2291  , 		//X_ADDR_END												
	0x3002, 134   , 		//Y_ADDR_START											
	0x3006, 1413  , 		//Y_ADDR_END												
	0x30A2, 0x0001, 		//X_ODD_INCREMENT										
	0x30A6, 0x0001, 		//Y_ODD_INCREMENT										
    #if SENSOR_ROTATE_180
	0x3040, 0xC000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    #else
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	#endif
	0x300C, 1242  , 		//LINE_LENGTH_PCK										
	0x300A, 2634  , 		//FRAME_LENGTH_LINES								
	0x3014, 0	  , 		//FINE_INTEGRATION_TIME(fixed value)
	0x3012, 2640  , 		//Coarse_Integration_Time						
	0x3042, 677   , 		//EXTRA_DELAY												
	0x30BA, 0x002C, 		//Digital_Ctrl_Adc_High_Speed	

	0x3ED2,	0x0146,
	0x3ED4,	0x8F6C,
	0x3ED6,	0x66CC,
	0x3ECC,	0x0E0D,
							//Sequencer 							
	0x301A, 0,				//Disable Streaming 								
	SNR_REG_DELAY, 30,		//Delay 							  
	0x3088, 0x80BA, 																			  
	0x3086, 0xE653, 																			  
																	
	0x301A, 0x0004,			//Enable Streaming									
};

//Mode1
//for 1080p60(W) -> 720p60(W)
//Interface	MIPI 4Lane
//sensor output	2266x1280
//frame rate	59.94
//Sampling	no sampling
//PLL setting	PLL-01
ISP_UINT16 SNR_AR0330_Reg_2266x1280_60fps[] = 
{
	0x3004, 26	  , 		//X_ADDR_START											
	0x3008, 2291  , 		//X_ADDR_END												
	0x3002, 134   , 		//Y_ADDR_START											
	0x3006, 1413  , 		//Y_ADDR_END												
	0x30A2, 0x0001, 		//X_ODD_INCREMENT										
	0x30A6, 0x0001, 		//Y_ODD_INCREMENT										
    #if SENSOR_ROTATE_180
	0x3040, 0xC000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    #else
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	#endif
	0x300C, 1242  , 		//LINE_LENGTH_PCK										
	0x300A, 1320  , 		//FRAME_LENGTH_LINES								
	0x3014, 0	  , 		//FINE_INTEGRATION_TIME(fixed value)
	0x3012, 1320  , 		//Coarse_Integration_Time						
	0x3042, 959   , 		//EXTRA_DELAY												
	0x30BA, 0x002C, 		//Digital_Ctrl_Adc_High_Speed	
    
	0x3ED2,	0x0146,
	0x3ED4,	0x8F6C,
	0x3ED6,	0x66CC,
	0x3ECC,	0x0E0D,
							//Sequencer 							
	0x301A, 0,				//Disable Streaming 								
	SNR_REG_DELAY, 30,		//Delay 							  
	0x3088, 0x80BA, 																			  
	0x3086, 0xE653, 																			  
																	
	0x301A, 0x0004,			//Enable Streaming									
};

//Mode2
//for WVGA@120(W)
//Interface	MIPI 4Lane
//sensor output	1132x640
//frame rate	119.88
//Sampling	xy: 2xbin
//PLL setting	PLL-01
ISP_UINT16 SNR_AR0330_Reg_1132x640_120fps[] = 
{
	0x3004, 26	  , 		//X_ADDR_START											
	0x3008, 2289  , 		//X_ADDR_END												
	0x3002, 134   , 		//Y_ADDR_START											
	0x3006, 1413  , 		//Y_ADDR_END												
	0x30A2, 0x0003, 		//X_ODD_INCREMENT										
	0x30A6, 0x0003, 		//Y_ODD_INCREMENT										
    #if SENSOR_ROTATE_180
	0x3040, 0xF000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    #else
	0x3040, 0x3000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	#endif
	0x300C, 1154  , 		//LINE_LENGTH_PCK										
	0x300A, 711   , 		//FRAME_LENGTH_LINES								
	0x3014, 0	  , 		//FINE_INTEGRATION_TIME(fixed value)
	0x3012, 711   , 		//Coarse_Integration_Time						
	0x3042, 326   , 		//EXTRA_DELAY												
	0x30BA, 0x006C, 		//Digital_Ctrl_Adc_High_Speed
    
	0x3ED2,	0xBF46,
	0x3ED4,	0x8F3C,
	0x3ED6,	0x33CC,
	0x3ECC,	0x1D0D,
							//Sequencer 							
	0x301A, 0,				//Disable Streaming 								
	SNR_REG_DELAY, 30,		//Delay 							  
	0x3088, 0x80BA, 																			  
	0x3086, 0x0253, 																			  
																	
	0x301A, 0x0004,			//Enable Streaming									
};

//------------------------------------------------------
//Mode3
//for 1080p30(S) / 720p30(S) / WVGA30(S)
//Interface	MIPI 4Lane
//sensor output	1928x1096
//frame rate	29.97
//Sampling	no sampling
//PLL setting	PLL-01
ISP_UINT16 SNR_AR0330_Reg_1928x1096_30fps[] = 
{
	0x3004, 194	  , 		//X_ADDR_START											
	0x3008, 2121  , 		//X_ADDR_END												
	0x3002, 226   , 		//Y_ADDR_START											
	0x3006, 1321  , 		//Y_ADDR_END												
	0x30A2, 0x0001, 		//X_ODD_INCREMENT										
	0x30A6, 0x0001, 		//Y_ODD_INCREMENT										
    #if SENSOR_ROTATE_180
	0x3040, 0xC000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    #else
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	#endif
	0x300C, 1242  , 		//LINE_LENGTH_PCK										
    0x300A, 2634  , 		//FRAME_LENGTH_LINES	
	0x3014, 0	  , 		//FINE_INTEGRATION_TIME(fixed value)
	0x3012, 2640  , 		//Coarse_Integration_Time						
	0x3042, 677   , 		//EXTRA_DELAY												
	0x30BA, 0x002C, 		//Digital_Ctrl_Adc_High_Speed	

	0x3ED2,	0x0146,
	0x3ED4,	0x8F6C,
	0x3ED6,	0x66CC,
	0x3ECC,	0x0E0D,
							//Sequencer 							
	0x301A, 0,				//Disable Streaming 								
	SNR_REG_DELAY, 30,		//Delay 							  
	0x3088, 0x80BA, 																			  
	0x3086, 0x0253, 																			  
																	
	0x301A, 0x0004,			//Enable Streaming									
};

//Mode4
//for 1080p60(S) -> 720p60(S)
//Interface	MIPI 4Lane
//sensor output	1928x1096
//frame rate	59.94
//Sampling	no sampling
//PLL setting	PLL-01
ISP_UINT16 SNR_AR0330_Reg_1928x1096_60fps[] = 
{
	0x3004, 194	  , 		//X_ADDR_START											
	0x3008, 2121  , 		//X_ADDR_END												
	0x3002, 226   , 		//Y_ADDR_START											
	0x3006, 1321  , 		//Y_ADDR_END												
	0x30A2, 0x0001, 		//X_ODD_INCREMENT										
	0x30A6, 0x0001, 		//Y_ODD_INCREMENT										
    #if SENSOR_ROTATE_180
	0x3040, 0xC000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    #else
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	#endif
	0x300C, 1242  , 		//LINE_LENGTH_PCK										
	0x300A, 1321  , 		//FRAME_LENGTH_LINES								
	0x3014, 0	  , 		//FINE_INTEGRATION_TIME(fixed value)
	0x3012, 1320  , 		//Coarse_Integration_Time						
	0x3042, 959   , 		//EXTRA_DELAY												
	0x30BA, 0x002C, 		//Digital_Ctrl_Adc_High_Speed	
	
	0x3ED2,	0x0146,
	0x3ED4,	0x8F6C,
	0x3ED6,	0x66CC,
	0x3ECC,	0x0E0D,
							//Sequencer 							
	0x301A, 0,				//Disable Streaming 								
	SNR_REG_DELAY, 30,		//Delay 							  
	0x3088, 0x80BA, 																			  
	0x3086, 0x0253, 																			  
																	
	0x301A, 0x0004,			//Enable Streaming									
};

//Mode5
//for WVGA@120(S)
//Interface	MIPI 4Lane
//sensor output	964x544
//frame rate	119.88
//Sampling	xy: 2xbin
//PLL setting	PLL-01
ISP_UINT16 SNR_AR0330_Reg_964x544_120fps[] = 
{
	0x3004, 194	  , 		//X_ADDR_START											
	0x3008, 2121  , 		//X_ADDR_END												
	0x3002, 230   , 		//Y_ADDR_START											
	0x3006, 1317  , 		//Y_ADDR_END												
	0x30A2, 0x0003, 		//X_ODD_INCREMENT										
	0x30A6, 0x0003, 		//Y_ODD_INCREMENT										
    #if SENSOR_ROTATE_180
	0x3040, 0xF000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    #else
	0x3040, 0x3000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	#endif
	0x300C, 1154  , 		//LINE_LENGTH_PCK										
	0x300A, 711   , 		//FRAME_LENGTH_LINES								
	0x3014, 0	  , 		//FINE_INTEGRATION_TIME(fixed value)
	0x3012, 710   , 		//Coarse_Integration_Time						
	0x3042, 326   , 		//EXTRA_DELAY												
	0x30BA, 0x006C, 		//Digital_Ctrl_Adc_High_Speed

	0x3ED2,	0xBF46,
	0x3ED4,	0x8F3C,
	0x3ED6,	0x33CC,
	0x3ECC,	0x1D0D,
							//Sequencer 							
	0x301A, 0,				//Disable Streaming 								
	SNR_REG_DELAY, 30,		//Delay 							  
	0x3088, 0x80BA, 																			  
	0x3086, 0x0253, 																			  
																	
	0x301A, 0x0004,			//Enable Streaming									
};

//Mode6
//for 2M@30(W) preview
//Interface	MIPI 4Lane
//sensor output	2184x1232
//frame rate	30
//Sampling	no sampling
//PLL setting	PLL-01
ISP_UINT16 SNR_AR0330_Reg_2184x1232_30fps[] = 
{
	0x3004, 66	  , 		//X_ADDR_START											
	0x3008, 2249  , 		//X_ADDR_END												
	0x3002, 158   , 		//Y_ADDR_START											
	0x3006, 1389  , 		//Y_ADDR_END												
	0x30A2, 0x0001, 		//X_ODD_INCREMENT										
	0x30A6, 0x0001, 		//Y_ODD_INCREMENT										
    #if SENSOR_ROTATE_180
	0x3040, 0xC000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    #else
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	#endif
	0x300C, 2643  , 		//LINE_LENGTH_PCK										
	0x300A, 1242  , 		//FRAME_LENGTH_LINES								
	0x3014, 0	  , 		//FINE_INTEGRATION_TIME(fixed value)
	0x3012, 2640  , 		//Coarse_Integration_Time						
	0x3042, 677   , 		//EXTRA_DELAY												
	0x30BA, 0x002C, 		//Digital_Ctrl_Adc_High_Speed

	0x3ED2,	0x0146,
	0x3ED4,	0x8F6C,
	0x3ED6,	0x66CC,
	0x3ECC,	0x0E0D,
							//Sequencer 							
	0x301A, 0,				//Disable Streaming 								
	SNR_REG_DELAY, 30,		//Delay 							  
	0x3088, 0x80BA, 																			  
	0x3086, 0xE653, 																			  
																	
	0x301A, 0x0004,			//Enable Streaming									
};

//Mode7
//For 2M@15(W) Capture
//Interface	MIPI 4Lane
//Sensor output	2184x1232
//Frame rate	15
//Sampling		No sampling
//PLL setting	PLL-01
ISP_UINT16 SNR_AR0330_Reg_2184x1232_15fps[] = 
{
	0x3004, 66	  , 		//X_ADDR_START											
	0x3008, 2249  , 		//X_ADDR_END												
	0x3002, 158   , 		//Y_ADDR_START											
	0x3006, 1389  , 		//Y_ADDR_END												
	0x30A2, 0x0001, 		//X_ODD_INCREMENT										
	0x30A6, 0x0001, 		//Y_ODD_INCREMENT										
    #if SENSOR_ROTATE_180
	0x3040, 0xC000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    #else
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	#endif
	0x300C, 5281  , 		//LINE_LENGTH_PCK										
	0x300A, 1242  , 		//FRAME_LENGTH_LINES								
	0x3014, 0	  , 		//FINE_INTEGRATION_TIME(fixed value)
	0x3012, 4806  , 		//Coarse_Integration_Time						
	0x3042, 998   , 		//EXTRA_DELAY												
	0x30BA, 0x002C, 		//Digital_Ctrl_Adc_High_Speed
	
	0x3ED2,	0x0146,
	0x3ED4,	0x8F6C,
	0x3ED6,	0x66CC,
	0x3ECC,	0x0E0D,
							//Sequencer 							
	0x301A, 0,				//Disable Streaming 								
	SNR_REG_DELAY, 30,		//Delay 							  
	0x3088, 0x80BA, 																			  
	0x3086, 0xE653, 																			  
																	
	0x301A, 0x0004,			//Enable Streaming									
};

//Mode8
//For 2M@30(S) preview
//Interface	MIPI 4Lane
//Sensor output	1928x1088
//Frame rate	30
//Sampling		No sampling
//PLL setting	PLL-01
ISP_UINT16 SNR_AR0330_Reg_1928x1088_30fps[] = 
{
	0x3004, 194	  , 		//X_ADDR_START											
	0x3008, 2121  , 		//X_ADDR_END												
	0x3002, 230   , 		//Y_ADDR_START											
	0x3006, 1317  , 		//Y_ADDR_END												
	0x30A2, 0x0001, 		//X_ODD_INCREMENT										
	0x30A6, 0x0001, 		//Y_ODD_INCREMENT										
    #if SENSOR_ROTATE_180
	0x3040, 0xC000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    #else
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	#endif
	0x300C, 2643  , 		//LINE_LENGTH_PCK										
	0x300A, 1242  , 		//FRAME_LENGTH_LINES								
	0x3014, 0	  , 		//FINE_INTEGRATION_TIME(fixed value)
	0x3012, 2641  , 		//Coarse_Integration_Time						
	0x3042, 677   , 		//EXTRA_DELAY												
	0x30BA, 0x002C, 		//Digital_Ctrl_Adc_High_Speed		
	
	0x3ED2,	0x0146,
	0x3ED4,	0x8F6C,
	0x3ED6,	0x66CC,
	0x3ECC,	0x0E0D,
							//Sequencer 							
	0x301A, 0,				//Disable Streaming 								
	SNR_REG_DELAY, 30,		//Delay 							  
	0x3088, 0x80BA, 																			  
	0x3086, 0xE653, 																			  
																	
	0x301A, 0x0004,			//Enable Streaming									
};

//Mode9
//For 2M@15(S) Capture
//Interface	MIPI 4Lane
//Sensor output	1928x1088
//Frame rate	15
//Sampling		No sampling
//PLL setting	PLL-01
ISP_UINT16 SNR_AR0330_Reg_1928x1088_15fps[] = 
{
	0x3004, 194	  , 		//X_ADDR_START											
	0x3008, 2121  , 		//X_ADDR_END												
	0x3002, 230   , 		//Y_ADDR_START											
	0x3006, 1317  , 		//Y_ADDR_END												
	0x30A2, 0x0001, 		//X_ODD_INCREMENT										
	0x30A6, 0x0001, 		//Y_ODD_INCREMENT										
    #if SENSOR_ROTATE_180
	0x3040, 0xC000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    #else
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	#endif
	0x300C, 5282  , 		//LINE_LENGTH_PCK										
	0x300A, 1242  , 		//FRAME_LENGTH_LINES								
	0x3014, 0	  , 		//FINE_INTEGRATION_TIME(fixed value)
	0x3012, 5281  , 		//Coarse_Integration_Time						
	0x3042, 998   , 		//EXTRA_DELAY												
	0x30BA, 0x002C, 		//Digital_Ctrl_Adc_High_Speed

	0x3ED2,	0x0146,
	0x3ED4,	0x8F6C,
	0x3ED6,	0x66CC,
	0x3ECC,	0x0E0D,
							//Sequencer 							
	0x301A, 0,				//Disable Streaming 								
	SNR_REG_DELAY, 30,		//Delay 							  
	0x3088, 0x80BA, 																			  
	0x3086, 0xE653, 																			  
																	
	0x301A, 0x0004,			//Enable Streaming									
};

//Mode10
//For 3M@30(S) Capture
//Interface	MIPI 4Lane
//Sensor output	2048x1536
//Frame rate	30
//Sampling		No sampling
//PLL setting	PLL-01
ISP_UINT16 SNR_AR0330_Reg_2048x1536_30fps[] = 
{
	0x3004, 128	  , 		//X_ADDR_START											
	0x3008, 2175  , 		//X_ADDR_END												
	0x3002, 6     , 		//Y_ADDR_START											
	0x3006, 1541  , 		//Y_ADDR_END												
	0x30A2, 0x0001, 		//X_ODD_INCREMENT										
	0x30A6, 0x0001, 		//Y_ODD_INCREMENT										
    #if SENSOR_ROTATE_180
	0x3040, 0xC000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    #else
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	#endif
	0x300C, 2118  , 		//LINE_LENGTH_PCK										
	0x300A, 1548  , 		//FRAME_LENGTH_LINES								
	0x3014, 0	  , 		//FINE_INTEGRATION_TIME(fixed value)
	0x3012, 2118  , 		//Coarse_Integration_Time						
	0x3042, 0     , 		//EXTRA_DELAY												
	0x30BA, 0x002C, 		//Digital_Ctrl_Adc_High_Speed

	0x3ED2,	0x0146,
	0x3ED4,	0x8F6C,
	0x3ED6,	0x66CC,
	0x3ECC,	0x0E0D,
							//Sequencer 							
	0x301A, 0,				//Disable Streaming 								
	SNR_REG_DELAY, 30,		//Delay 							  
	0x3088, 0x80BA, 																			  
	0x3086, 0xE653, 																			  
																	
	0x301A, 0x0004,			//Enable Streaming									
};

//Mode16
//for 3M@30(S) Video
//Interface	MIPI 4Lane
//sensor output	2304x1296
//frame rate	29.97
//Sampling	no sampling
//PLL setting	PLL-01
ISP_UINT16 SNR_AR0330_Reg_2304x1296_29_97fps[] = 
{
    0x3004,	6,				//X_ADDR_START	
    0x3008,	2309+2,			//X_ADDR_END
    0x3002,	120, 			//Y_ADDR_START		
    0x3006,	1415+2, 		//Y_ADDR_END	
    0x30A2,	0x0001, 		//X_ODD_INCREMENT
    0x30A6,	0x0001, 		//Y_ODD_INCREMENT
    #if SENSOR_ROTATE_180
	0x3040, 0xC000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    #else
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	#endif

    0x300C,	1248,			//LINE_LENGTH_PCK	
    0x300A,	2630, 			//FRAME_LENGTH_LINES	
    0x3014,	0, 				//FINE_INTEGRATION_TIME(fixed value)
    0x3012,	2628, 			//Coarse_Integration_Time
    0x3042,	0, 				//EXTRA_DELAY	
    0x30BA,	0x002C, 		//Digital_Ctrl_Adc_High_Speed
    0x3ED2,	0x0146,
    0x3ED4,	0x8F6C,
    0x3ED6,	0x66CC,
    0x3ECC,	0x0E0D,

    0x301A,	0,				//Disable Streaming 		
    SNR_REG_DELAY, 30,		//Delay 		
    0x3088,	0x80BA,
    0x3086,	0xE653,

    0x301A,	0x0004,			//Enable Streaming	
};

#endif  // SENSOR_IF

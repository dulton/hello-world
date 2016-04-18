#if (SENSOR_IF == SENSOR_IF_PARALLEL)

MMPF_SENSOR_RESOLUTION m_SensorRes = 
{
	4,				// ubSensorModeNum
	0,				// ubDefPreviewMode
	1,				// ubDefCaptureMode
	2200,           // usPixelSize
//  Mode0   Mode1   Mode2   Mode3
    {1,     1,      1,      1},     // usVifGrabStX
    {4,     1,      4,       4},	// usVifGrabStY
    {2308,  2056,   1928,   1284},	// usVifGrabW
    {1300,  1544,   1096,   724},	// usVifGrabH
#if (CHIP == MCR_V2)   
    {1,     1,      1,      1},     // usBayerInGrabX
    {1,     1,      1,      1},     // usBayerInGrabY
    {4,     8,      8,      4},     // usBayerInDummyX
    {4,     8,      16,     4},	    // usBayerInDummyY
    {2304,  2048,   1920,   1280},	// usBayerOutW
    {1296,  1536,   1080,   720},	// usBayerOutH
#endif
    {2304,  2048,   1920,   1280},	// usScalInputW
    {1296,  1536,   1080,   720},	// usScalInputH 
    {300,   250,    300,    600},   // usTargetFpsx10
    {1329,  1578,   1314,   811},	// usVsyncLine
    {1,     1,      1,      1},     // ubWBinningN
    {1,     1,      1,      1},     // ubWBinningN
    {1,     1,      1,      1},     // ubWBinningN
    {1,     1,      1,      1},     // ubWBinningN
    {0xFF,	0xFF,	0xFF,   0xFF},  // ubCustIQmode
    {0xFF,  0xFF,	0xFF,   0xFF}   // ubCustAEmode
};

#if 0
void ____Sensor_Init_OPR_Table____(){ruturn;} //dummy
#endif

MMP_USHORT SNR_AR0330_PARALLEL_Reg_Init_Customer[] = 
{
	0x301A, 0x10DD, 	// RESET_REGISTER
	SNR_REG_DELAY,    100,  // delay = 100

	0x3052,0xA114,  	// Modified for OTPM cannot be access at low temperature
	0X304A,0x0070,  	// Modified for OTPM cannot be access at low temperature

	SNR_REG_DELAY,    50,   // delay = 50
	0x301A, 0x10D8, 	// RESET_REGISTER = 4312

	0x302A,      6,		// VT_PIX_CLK_DIV = 6
	0x302C,      1,		// VT_SYS_CLK_DIV = 1
	0x302E,      2,		// PRE_PLL_CLK_DIV = 2
	0x3030,     49,		// PLL_MULTIPLIER = 49
	0x3036,     12,		// OP_PIX_CLK_DIV = 12
	0x3038,      1, 	// OP_SYS_CLK_DIV = 1
	0x31AC,   3084, 	// DATA_FORMAT_BITS = 3084
	0x301A,   4316,		// RESET_REGISTER = 4316

	0x31AE, 0x0301,		// SERIAL_FORMAT = 769

	0x3002,    234,		// Y_ADDR_START = 234
	0x3004,    198,		// X_ADDR_START = 198
	0x3006,   1313+16,	// Y_ADDR_END = 1313
	0x3008,   2117+8,	// X_ADDR_END = 2117
	0x300A,   1315,		// FRAME_LENGTH_LINES = 1315
	0x300C,   1242,		// LINE_LENGTH_PCK = 1242
	0x3012,    657,		// COARSE_INTEGRATION_TIME = 657
	0x3014,      0,		// FINE_INTEGRATION_TIME = 0
	0x30A2,      1,		// X_ODD_INC = 1
	0x30A6,      1,		// Y_ODD_INC = 1
	0x308C,      6,		// Y_ADDR_START_CB = 6
	0x308A,      6,		// X_ADDR_START_CB = 6
	0x3090,   1541,		// Y_ADDR_END_CB = 1541
	0x308E,   2309,		// X_ADDR_END_CB = 2309
	0x30AA,   1570,		// FRAME_LENGTH_LINES_CB = 1570
	0x303E,   1248,		// LINE_LENGTH_PCK_CB = 1248
	0x3016,   1569,		// COARSE_INTEGRATION_TIME_CB = 1569
	0x3018,      0,		// FINE_INTEGRATION_TIME_CB = 0
	0x30AE,      1,		// X_ODD_INC_CB = 1
	0x30A8,      1,		// Y_ODD_INC_CB = 1
    #if SENSOR_ROTATE_180
	0x3040, 0xC000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    #else
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	#endif
	0x3042,    103,		// EXTRA_DELAY = 103

	0x30BA, 0x002C,  	// DIGITAL_CTRL = 44
	0x3088, 0x80BA,  	// SEQ_CTRL_PORT = 32954
	0x3086, 0xE653,  	// SEQ_DATA_PORT = 58963
	0x306E, 0xFC10,  

	0xFFFF,    100   	// delay = 100
};

#if 0
void ____Sensor_Res_OPR_Table____(){ruturn;} //dummy
#endif

MMP_USHORT SNR_AR0330CS_Reg_1280x720_60fps_Customer[] = 
{
	0x301A, 0x10DC,  	// RESET_REGISTER
	0xFFFF, 100,
	0x301A, 0x10D8,  	// RESET_REGISTER
	0xFFFF, 100,
	0x31AE, 0x0301,  	// SERIAL_FORMAT
	0x3004,   384,	 	// X_ADDR_START
	0x3008,   384+1283,	// X_ADDR_END
	0x3002,   408,		// Y_ADDR_START
	0x3006,   408+723,	// Y_ADDR_END
	0x30A2,      1,		// X_ODD_INC
	0x30A6,      1,		// Y_ODD_INC
    #if SENSOR_ROTATE_180
	0x3040, 0xC000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	0x3040, 0xC000,  	// READ_MODE
	0x3040, 0xC000,  	// READ_MODE
    #else
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	0x3040, 0x0000,  	// READ_MODE
	0x3040, 0x0000,  	// READ_MODE
	#endif
	0x300C,   1014,		// LINE_LENGTH_PCK
	0x300A,    788,		// FRAME_LENGTH_LINES
	0x3014,      0,		// FINE_INTEGRATION_TIME
	0x3012,    785,		// COARSE_INTEGRATION_TIME
	0x3042,    968,		// EXTRA_DELAY
	0x30BA, 0x006C,  	// DIGITAL_CTRL
	0x301A, 0x10D8,  	// RESET_REGISTER
	0xFFFF, 100,
	0x3088, 0x80BA,  	// SEQ_CTRL_PORT
	0x3086, 0x0253,  	// SEQ_DATA_PORT
	0x301A, 0x10DC,  	// RESET_REGISTER
	0x301A, 0x10DC,  	// RESET_REGISTER
	0xFFFF,    100   	// delay = 100
};

MMP_USHORT SNR_AR0330CS_Reg_2048x1536_25fps_Customer[] = 
{
	0x301A, 0x10DC, 	// RESET_REGISTER
    0xFFFF,    100,  	// delay = 100
	0x301A, 0x10D8,  	// RESET_REGISTER = 4312
	0xFFFF,    100,

	0x302A,      6,		// VT_PIX_CLK_DIV = 6
	0x302C,      1,		// VT_SYS_CLK_DIV = 1
	0x302E,      2,		// PRE_PLL_CLK_DIV = 2
	0x3030,     49,		// PLL_MULTIPLIER = 49
	0x3036,     12,		// OP_PIX_CLK_DIV = 12
	0x3038,      1,		// OP_SYS_CLK_DIV = 1
	0x31AC,   3084,		// DATA_FORMAT_BITS = 3084
	0x301A,   4316,	 	// RESET_REGISTER = 4316
	0xFFFF, 100,

	0x31AE, 0x0301,  	// SERIAL_FORMAT = 769

	0x3002,      6,		// Y_ADDR_START = 6
	0x3004,    134,		// X_ADDR_START = 134
	0x3006,   1541+8,	// Y_ADDR_END = 1541
	0x3008,   2181+8,	// X_ADDR_END = 2181
	0x300A,   1578,		// FRAME_LENGTH_LINES = 1578
	0x300C,   1242,		// LINE_LENGTH_PCK = 1242
	0x3012,    657,		// COARSE_INTEGRATION_TIME = 657
	0x3014,      0,		// FINE_INTEGRATION_TIME = 0
	0x30A2,      1,		// X_ODD_INC = 1
	0x30A6,      1,		// Y_ODD_INC = 1
	0x308C,      6,		// Y_ADDR_START_CB = 6
	0x308A,      6,		// X_ADDR_START_CB = 6
	0x3090,   1541+8,	// Y_ADDR_END_CB = 1541
	0x308E,   2309,		// X_ADDR_END_CB = 2309
	0x30AA,   1570,		// FRAME_LENGTH_LINES_CB = 1570
	0x303E,   1248,		// LINE_LENGTH_PCK_CB = 1248
	0x3016,   1569,		// COARSE_INTEGRATION_TIME_CB = 1569
	0x3018,      0,		// FINE_INTEGRATION_TIME_CB = 0
	0x30AE,      1,		// X_ODD_INC_CB = 1
	0x30A8,      1,		// Y_ODD_INC_CB = 1
    #if SENSOR_ROTATE_180
	0x3040, 0xC000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    #else
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	#endif
	0x3042,    124,		// EXTRA_DELAY = 124

	0x30BA, 0x002C,  	// DIGITAL_CTRL = 44
	0x3088, 0x80BA,  	// SEQ_CTRL_PORT = 32954
	0x3086, 0xE653, 	// SEQ_DATA_PORT = 58963

	0xFFFF,    100   	// delay = 100
};

MMP_USHORT SNR_AR0330CS_Reg_2304x1296_Customer[] = 
{
	0x301A, 0x10DC,  	// RESET_REGISTER
	0xFFFF, 100,
	0x301A, 0x10D8,  	// RESET_REGISTER = 4312
	0xFFFF, 100,

	0x302A,      6,		// VT_PIX_CLK_DIV = 6
	0x302C,      1,		// VT_SYS_CLK_DIV = 1
	0x302E,      2,		// PRE_PLL_CLK_DIV = 2
	0x3030,     51,		// PLL_MULTIPLIER = 49=>OK
	0x3036,     12,		// OP_PIX_CLK_DIV = 12
	0x3038,      1,		// OP_SYS_CLK_DIV = 1
	0x31AC,   3084,		// DATA_FORMAT_BITS = 3084
	0x301A,   4316,		// RESET_REGISTER = 4316
	0xFFFF, 100,

	0x31AE, 0x0301,  	// SERIAL_FORMAT = 769

	0x3002,    120,		// Y_ADDR_START = 234
	0x3004,    0,		// X_ADDR_START = 198
	0x3006,   1422,		// Y_ADDR_END = 1313
	0x3008,   2308,		// X_ADDR_END = 2117
	0x300A,   1330,		// FRAME_LENGTH_LINES = 1315
	0x300C,   2308-1030,
	0x3012,    657,		// COARSE_INTEGRATION_TIME = 657
	0x3014,      0,		// FINE_INTEGRATION_TIME = 0
	0x30A2,      1,		// X_ODD_INC = 1
	0x30A6,      1,		// Y_ODD_INC = 1
	0x308C,      6,		// Y_ADDR_START_CB = 6
	0x308A,      6,		// X_ADDR_START_CB = 6
	0x3090,   1541,		// Y_ADDR_END_CB = 1541
	0x308E,   2309,		// X_ADDR_END_CB = 2309
	0x30AA,   1570,		// FRAME_LENGTH_LINES_CB = 1570
	0x303E,   1248,		// LINE_LENGTH_PCK_CB = 1248
	0x3016,   1569,		// COARSE_INTEGRATION_TIME_CB = 1569
	0x3018,      0,		// FINE_INTEGRATION_TIME_CB = 0
	0x30AE,      1,		// X_ODD_INC_CB = 1
	0x30A8,      1,	 	// Y_ODD_INC_CB = 1
    #if SENSOR_ROTATE_180
	0x3040, 0xC000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    #else
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	#endif
	0x3042,      0,		// EXTRA_DELAY

	0x30BA, 0x002C,  	// DIGITAL_CTRL = 44
	0x3088, 0x80BA,  	// SEQ_CTRL_PORT = 32954
	0x3086, 0xE653,  	// SEQ_DATA_PORT = 58963
	0x306E, 0xFC10,  
	///AR0330 defect correction
	0x31E0, 0x0741,
	0xFFFF,    100   	// delay = 100
};

// 1080p 30FPS
MMP_USHORT SNR_AR0330CS_Reg_1920x1080_Customer[] = 
{
	0x301A, 0x10DC,  	// RESET_REGISTER
	0x301A, 0x10D8,  	// RESET_REGISTER = 4312

	0x302A,      6,		// VT_PIX_CLK_DIV = 6
	0x302C,      1,		// VT_SYS_CLK_DIV = 1
	0x302E,      2,		// PRE_PLL_CLK_DIV = 2
	0x3030,     49,		// PLL_MULTIPLIER = 49
	0x3036,     12,		// OP_PIX_CLK_DIV = 12
	0x3038,      1,		// OP_SYS_CLK_DIV = 1
	0x31AC,   3084,		// DATA_FORMAT_BITS = 3084
	0x301A,   4316,		// RESET_REGISTER = 4316

	0x31AE, 0x0301,  	// SERIAL_FORMAT = 769
#if 0
	0x3002,    234,		// Y_ADDR_START = 234
	0x3004,    198,		// X_ADDR_START = 198
	0x3006,   1095+234,	// Y_ADDR_END = 1313
	0x3008,   1927+198,	// X_ADDR_END = 2117
#else
	0x3002,    220,		// Y_ADDR_START = 234
	0x3004,    184,		// X_ADDR_START = 198
	0x3006,   1096+220,	// Y_ADDR_END = 1313
	0x3008,   1936+184,	// X_ADDR_END = 2117
#endif

	0x300A,   1310,		// FRAME_LENGTH_LINES = 1315
	0x300C,   1248,		// LINE_LENGTH_PCK = 1242

	0x3012,   1000,		// COARSE_INTEGRATION_TIME = 657
	0x3014,      0,		// FINE_INTEGRATION_TIME = 0
	0x30A2,      1,		// X_ODD_INC = 1
	0x30A6,      1,		// Y_ODD_INC = 1
	0x308C,      6,		// Y_ADDR_START_CB = 6
	0x308A,      6,		// X_ADDR_START_CB = 6
	0x3090,   1541,		// Y_ADDR_END_CB = 1541
	0x308E,   2309,		// X_ADDR_END_CB = 2309
	0x30AA,   1570,		// FRAME_LENGTH_LINES_CB = 1570
	0x303E,   1248,		// LINE_LENGTH_PCK_CB = 1248
	0x3016,   1569,		// COARSE_INTEGRATION_TIME_CB = 1569
	0x3018,      0,		// FINE_INTEGRATION_TIME_CB = 0
	0x30AE,      1,		// X_ODD_INC_CB = 1
	0x30A8,      1,		// Y_ODD_INC_CB = 1
    #if SENSOR_ROTATE_180
	0x3040, 0xC000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
    #else
	0x3040, 0x0000,	    // [15]: flip, [14]: mirror, [12]: Row binning, [13]: column binning
	#endif
	0x3042,    103,		// EXTRA_DELAY = 103

	0x30BA, 0x002C,  	// DIGITAL_CTRL = 44
	0x3088, 0x80BA,  	// SEQ_CTRL_PORT = 32954
	0x3086, 0xE653,  	// SEQ_DATA_PORT = 58963
	0x306E, 0xFC10,
	// AR0330 defect correction
	0x31E0, 0x0741,
	0xFFFF,    100   // delay = 100
};

#endif  // SENSOR_IF

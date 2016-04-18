//==============================================================================
//
//  File        : lcd_OTA5180A_SerialRGB.c
//  Description : 480x272 LCD Panel Control Function
//                Serial-RGB without SPI initial
//  Author      :
//  Revision    : 1.0
//
//==============================================================================

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "Customer_config.h" // CarDV
#include "lib_retina.h"
#include "lcd_common.h"
#include "mmpf_pio.h"

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#define Delayms(t) 					LCD_WAIT(t)

#define SECONDARY_DISPLAY 			(0)

#define PANEL_WIDTH                 (480)
#define PANEL_HEIGHT                (272)

static MMPF_PANEL_ATTR m_PanelAttr = 
{
	// Panel basic setting
	PANEL_WIDTH, PANEL_HEIGHT,
	LCD_TYPE_RGBLCD,
	LCD_PRM_CONTROLER,
	0,

	// CarDV
#if ((16 == LCD_MODEL_RATIO_X) && (9 == LCD_MODEL_RATIO_Y))
	LCD_RATIO_16_9,
#else
	LCD_RATIO_4_3,
#endif

	// Panel initial sequence
	NULL,

	// Index/Cmd sequence
	NULL,

	// MCU interface
	LCD_BUS_WIDTH_8,
	LCD_PHASE0,
	LCD_POLARITY0,
	LCD_MCU_80SYS,
	0,
	0,
	0,
	LCD_RGB_ORDER_RGB,

	// RGB interface
	MMP_FALSE,
	LCD_SIG_POLARITY_H,//LCD_SIG_POLARITY_L,     // DCLK Polarity
	LCD_SIG_POLARITY_L,     // H-Sync Polarity
	LCD_SIG_POLARITY_L,     // V-Sync Polarity
	RGB_D24BIT_RGB888,

	{0}
};

#define ENABLE_LCD_LOG       		(0)

#if defined(FAT_BOOT)
#define ENABLE_LCD_TEST_PATTERN 	(1)
#else
#define ENABLE_LCD_TEST_PATTERN 	(0)
#endif

#if (ENABLE_LCD_LOG)
#define LCD_DBG_LEVEL               (0)
#else
#define LCD_DBG_LEVEL               (3)
#endif

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

void RTNA_LCD_InitMainLCD(void)
{
#if (SECONDARY_DISPLAY == 0)
	RTNA_DBG_Str(LCD_DBG_LEVEL, "### RTNA_LCD_InitMainLCD - OTA5180A\r\n");

	m_PanelAttr.usDotClkRatio 	= 8;    // 266MHz / Divder

	// RGB interface
	m_PanelAttr.usHBPorch 		= (42 / 3) + 1;
	m_PanelAttr.usHSyncW 		= 0;
	m_PanelAttr.usHBlanking 	= 51 / 3;   // HBP + HFP
	m_PanelAttr.usVBPorch 		= 12;
	m_PanelAttr.usVSyncW 		= 0;
	m_PanelAttr.usVBlanking 	= 16;//14;
	m_PanelAttr.usPRT2HdotClk 	= 2;
	m_PanelAttr.bDeltaRBG 		= MMP_TRUE;
	m_PanelAttr.bDummyRBG 		= MMP_FALSE;
	m_PanelAttr.ubEvenLnOrder   = LCD_SPI_PIX_ORDER_BRG;
	m_PanelAttr.ubOddLnOrder    = LCD_SPI_PIX_ORDER_BRG;

/*
    // SPI interface - for initial
	m_PanelAttr.usSpi2MciRatio 	= 0x80;
	m_PanelAttr.usCsSetupCyc 	= 0x0F;
	m_PanelAttr.usCsHoldCyc 	= 0x0F;
	m_PanelAttr.usCsHighWidth 	= 0xFF;
	m_PanelAttr.usSPIRegBitCnt  = SPI_PANEL_8BITS;
*/

	// Window setting (For drawing test pattern)
	m_PanelAttr.ubDispWinId 	= LCD_DISP_WIN_PIP;
	m_PanelAttr.usWinStX 		= 0;
	m_PanelAttr.usWinStY 		= 0;
	m_PanelAttr.usWinW 			= PANEL_WIDTH;
	m_PanelAttr.usWinH 			= PANEL_HEIGHT;
	m_PanelAttr.usWinBPP 		= 2;
	m_PanelAttr.usWinFmt 		= LCD_WIN_FMT_16BPP;
	m_PanelAttr.ulWinAddr 		= 0x03000000;

	MMPF_LCD_InitPanel(&m_PanelAttr);
#endif
}

void RTNA_LCD_Init2ndLCD(void)
{
#if (SECONDARY_DISPLAY == 1)
    // TBD
#endif
}

void RTNA_LCD_Init(void)
{
	RTNA_DBG_Str(0, "### RTNA_LCD_Init for OTA5180A ...\r\n");

#if (SECONDARY_DISPLAY == 1)
	RTNA_LCD_Init2ndLCD();
#else
	RTNA_LCD_InitMainLCD();
#endif
}

//============================================================================//
void RTNA_LCD_RGB_Test(void)
{
#if (ENABLE_LCD_TEST_PATTERN)
	#if defined(LCD_GPIO_BACK_LIGHT)
		/* Force turn-on LCD backlight */
		if (LCD_GPIO_BACK_LIGHT != MMP_GPIO_MAX)
		{
            MMPF_PIO_PadConfig(LCD_GPIO_BACK_LIGHT, PAD_OUT_DRIVING(0), MMP_TRUE);
			MMPF_PIO_EnableOutputMode(LCD_GPIO_BACK_LIGHT, MMP_TRUE, MMP_TRUE);
			RTNA_DBG_Str(LCD_DBG_LEVEL, "dbg-set LCD_BL to output mode\r\n");

			MMPF_PIO_SetData(LCD_GPIO_BACK_LIGHT, !LCD_GPIO_BACK_LIGHT_ACT_LEVEL, MMP_TRUE);
			Delayms(20);
			RTNA_DBG_Str(LCD_DBG_LEVEL, "dbg-set LCD_BL to low\r\n");

			MMPF_PIO_SetData(LCD_GPIO_BACK_LIGHT, LCD_GPIO_BACK_LIGHT_ACT_LEVEL, MMP_TRUE);
			Delayms(20);
			RTNA_DBG_Str(LCD_DBG_LEVEL, "dbg-set LCD_BL to high\r\n");
		}
	#endif

	MMPF_LCD_DrawTestPattern(&m_PanelAttr);
    Delayms(5000);
#endif
}

void RTNA_LCD_Direction(LCD_DIR dir)
{
	// TBD
}

void RTNA_LCD_SetReg(MMP_ULONG reg, MMP_UBYTE value)
{
	// TBD
}

void RTNA_LCD_GetReg(MMP_ULONG reg, MMP_ULONG *value)
{
	// TBD
}

void RTNA_LCD_AdjustBrightness(MMP_UBYTE level)
{
	// TBD
}

void RTNA_LCD_AdjustBrightness_R(MMP_UBYTE level)
{
	// TBD
}

void RTNA_LCD_AdjustBrightness_B(MMP_UBYTE level)
{
	// TBD
}

void RTNA_LCD_AdjustContrast(MMP_UBYTE level)
{
	// TBD
}

void RTNA_LCD_AdjustContrast_R(MMP_BYTE level)
{
	// TBD
}

void RTNA_LCD_AdjustContrast_B(MMP_BYTE level)
{
	// TBD
}

/*
 * Set Panel Properties
 * Width, Height, Color Depth, Display Type
 */
MMPF_PANEL_ATTR* RTNA_LCD_GetAttr(void)
{
	return &m_PanelAttr;
}

//==============================================================================
//
//  File        : lcd_ILI8961.c
//  Description : 320x240 LCD Panel Control Function
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

#define	DELTA_RGB 					(0)
#define	STRIP_RGB 					(1) // TBD??
#define	LCD_RGB_MODE 				(DELTA_RGB)

#define SECONDARY_DISPLAY 			(0)

#define LCD_DEFAULT_WIDTH           (720)
#define LCD_DEFAULT_HEIGHT          (480)

static MMPF_PANEL_ATTR m_PanelAttr = 
{
	// Panel basic setting
	LCD_DEFAULT_WIDTH, LCD_DEFAULT_HEIGHT,
	LCD_TYPE_RGBLCD,
	LCD_PRM_CONTROLER,
	0,

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
    LCD_SIG_POLARITY_H,     // DCLK Polarity
	LCD_SIG_POLARITY_L,     // H-Sync Polarity
	LCD_SIG_POLARITY_L,     // V-Sync Polarity
	RGB_D24BIT_RGB888,

	{0}
};

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

void RTNA_LCD_InitMainLCD(void)
{
    //NOP
}

void RTNA_LCD_Init2ndLCD(void)
{
    //NOP
}

void RTNA_LCD_Init(void)
{
    RTNA_DBG_Str(0, "### RTNA_LCD_Init for dummy...\r\n");
}

//============================================================================//
void RTNA_LCD_RGB_Test(void)
{
    //NOP
}

void RTNA_LCD_Direction(LCD_DIR dir)
{
    //NOP
}

void RTNA_LCD_SetReg(MMP_ULONG reg, MMP_UBYTE value)
{
    //NOP
}

void RTNA_LCD_GetReg(MMP_ULONG reg, MMP_ULONG *value)
{
    //NOP
}

void RTNA_LCD_AdjustBrightness(MMP_UBYTE level)
{
    //NOP
}

void RTNA_LCD_AdjustBrightness_R(MMP_UBYTE level)
{
    //NOP
}

void RTNA_LCD_AdjustBrightness_B(MMP_UBYTE level)
{
    //NOP
}

void RTNA_LCD_AdjustContrast(MMP_UBYTE level)
{
    //NOP
}

void RTNA_LCD_AdjustContrast_R(MMP_BYTE level)
{
    //NOP
}

void RTNA_LCD_AdjustContrast_B(MMP_BYTE level)
{
    //NOP
}

/*
 * Set Panel Properties
 * Width, Height, Color Depth, Display Type
 */
MMPF_PANEL_ATTR* RTNA_LCD_GetAttr(void)
{
	return &m_PanelAttr;
}

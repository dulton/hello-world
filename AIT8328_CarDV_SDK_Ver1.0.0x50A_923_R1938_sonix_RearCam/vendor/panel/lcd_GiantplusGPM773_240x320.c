//==============================================================================
//
//  File        : lcd_GiantplusGPM773_240x320.h
//  Description : Giantplus GPM773 240x320 LCD Panel Control Function
//  Author      : Robin Feng
//  Revision    : 1.0
//
//==============================================================================

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "lcd_common.h"
#include "lcd_GiantplusGPM773_240x320.h"

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#define SECONDARY_DISPLAY		(0)

//==============================================================================
//
//                              GLOBAL VARIABLE
//
//==============================================================================

static MMPF_PANEL_ATTR m_PanelAttr = 
{
	240, 
	160, 
	LCD_TYPE_PLCD, 
	LCD_PRM_CONTROLER, 
	0,
	LCD_RATIO_4_3,
	
	NULL, 
	NULL,
	
	LCD_BUS_WIDTH_16, 
	LCD_PHASE1, 
	LCD_POLARITY1, 
	LCD_MCU_80SYS, 
	0, 
	1, 
	9, 
	LCD_RGB_ORDER_RGB,
	
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

static void LcdGPM733InitIndexCmd(MMP_UBYTE ubController)
{
	DSPY_DECL;
	
	if (ubController == LCD_PRM_CONTROLER)
	{
		DSPY_WR_W(DSPY_LCD_TX_0,         0x2C);
    	DSPY_WR_W(DSPY_LCD_AUTO_CFG,     LCD_TX_TYPE_IDX(0));                                    
    	DSPY_WR_W(DSPY_PLCD_IDX_CMD_NUM, 1);
	}
	else
	{
		DSPY_WR_W(DSPY_SCD_LCD_TX_0,     0x2C);
    	DSPY_WR_W(DSPY_SCD_LCD_AUTO_CFG, LCD_TX_TYPE_IDX(0));                                    
    	DSPY_WR_W(DSPY_PLCD_IDX_CMD_NUM, 1);
	}
}

void RTNA_LCD_InitMainLCD(void)
{
	MMPF_PANEL_ATTR panelAttr;

	panelAttr.usPanelW 		= 240;
	panelAttr.usPanelH 		= 160;
	panelAttr.ubDevType 	= LCD_TYPE_PLCD;
	panelAttr.ubController	= LCD_PRM_CONTROLER;
	panelAttr.ulBgColor		= 0;
	panelAttr.ubRatio		= LCD_RATIO_4_3;
	
	panelAttr.pInitSeq		= LcdGPM733InitSeq;	
	panelAttr.pIdxCmdSeq	= LcdGPM733InitIndexCmd;
	
	panelAttr.ubBusWidth	= LCD_BUS_WIDTH_16;
	panelAttr.ubPhase		= LCD_PHASE1;
	panelAttr.ubPolarity	= LCD_POLARITY1;
	panelAttr.ubMCUSystem	= LCD_MCU_80SYS;
	panelAttr.usRsLeadCsCnt	= 0;
	panelAttr.usCsLeadRwCnt	= 1;
	panelAttr.usRwCycCnt	= 9;
	panelAttr.ubRgbOrder	= LCD_RGB_ORDER_RGB;
	
	panelAttr.ubDispWinId	= LCD_DISP_WIN_PIP;
	panelAttr.usWinStX		= 0;
	panelAttr.usWinStY		= 0;
	panelAttr.usWinW		= 240;
	panelAttr.usWinH		= 160;
	panelAttr.usWinBPP		= 2;
	panelAttr.usWinFmt		= LCD_WIN_FMT_16BPP;
	panelAttr.ulWinAddr		= 0x01500000;
	
	m_PanelAttr = panelAttr;
	
	MMPF_LCD_InitPanel(&panelAttr);
}

void RTNA_LCD_Init2ndLCD(void)
{
	MMPF_PANEL_ATTR panelAttr;

	panelAttr.usPanelW 		= 240;
	panelAttr.usPanelH 		= 160;
	panelAttr.ubDevType 	= LCD_TYPE_PLCD;
	panelAttr.ubController	= LCD_SCD_CONTROLER;
	panelAttr.ulBgColor		= 0;
	
	panelAttr.pInitSeq		= LcdGPM733InitSeq;	
	panelAttr.pIdxCmdSeq	= LcdGPM733InitIndexCmd;
	
	panelAttr.ubBusWidth	= LCD_BUS_WIDTH_16;
	panelAttr.ubPhase		= LCD_PHASE1;
	panelAttr.ubPolarity	= LCD_POLARITY1;
	panelAttr.ubMCUSystem	= LCD_MCU_80SYS;
	panelAttr.usRsLeadCsCnt	= 0;
	panelAttr.usCsLeadRwCnt	= 1;
	panelAttr.usRwCycCnt	= 9;
	panelAttr.ubRgbOrder	= LCD_RGB_ORDER_RGB;
	
	panelAttr.ubDispWinId	= LCD_DISP_WIN_OSD;
	panelAttr.usWinStX		= 0;
	panelAttr.usWinStY		= 0;
	panelAttr.usWinW		= 240;
	panelAttr.usWinH		= 160;
	panelAttr.usWinBPP		= 2;
	panelAttr.usWinFmt		= LCD_WIN_FMT_16BPP;
	panelAttr.ulWinAddr		= 0x01500000;
	
	m_PanelAttr = panelAttr;
	
	MMPF_LCD_InitPanel(&panelAttr);
}

void RTNA_LCD_Init(void)
{
    #if (SECONDARY_DISPLAY == 1)  
    RTNA_LCD_Init2ndLCD();   
    #else 
    RTNA_LCD_InitMainLCD();
    #endif
}

void RTNA_LCD_RGB_Test(void)
{
	MMPF_LCD_DrawTestPattern(&m_PanelAttr);	
}

MMPF_PANEL_ATTR* RTNA_LCD_GetAttr(void)
{
	return &m_PanelAttr;
}


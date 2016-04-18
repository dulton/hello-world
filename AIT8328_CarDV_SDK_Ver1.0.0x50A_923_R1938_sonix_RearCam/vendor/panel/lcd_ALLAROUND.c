//==============================================================================
//
//  File        : lcd_ALLAROUND.c
//  Description : All_around 240x320 LCD Panel Control Function
//  Author      : Philip Lin
//  Revision    : 1.1
//
//==============================================================================



//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "lcd_common.h"

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

//[GLOBAL] LCD configuration

#undef LCD_WAIT
#define LCD_WAIT(cnt)
#define Delayms(t)				LCD_WAIT(t)


#define SECONDARY_DISPLAY		(0)

static MMPF_PANEL_ATTR m_PanelAttr = {320, 240, LCD_TYPE_RGBLCD, LCD_PRM_CONTROLER, 0,
									  NULL, NULL,
									  LCD_BUS_WIDTH_8, LCD_PHASE0, LCD_POLARITY0, LCD_MCU_80SYS, 0, 0, 0, LCD_RGB_ORDER_RGB,
									  MMP_FALSE,
                                      LCD_SIG_POLARITY_H,     // DCLK Polarity
	                                  LCD_SIG_POLARITY_L,     // H-Sync Polarity
	                                  LCD_SIG_POLARITY_L,     // V-Sync Polarity
									  RGB_D24BIT_RGB888,
									  {0}};

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

void RTNA_LCD_InitMainLCD(void)
{
	MMPF_PANEL_ATTR panelAttr;

	panelAttr.usPanelW 		= 320;
	panelAttr.usPanelH 		= 240;
	panelAttr.ubDevType 	= LCD_TYPE_RGBLCD;//LCD_TYPE_RGB2LCD
	panelAttr.ubController	= LCD_PRM_CONTROLER;
	panelAttr.ulBgColor		= 0;
	
	panelAttr.pInitSeq		= NULL;	
	panelAttr.pIdxCmdSeq	= NULL;
		
	panelAttr.bPartialDisp	= MMP_FALSE;
	panelAttr.ubHsyncPor	= LCD_SIG_POLARITY_L;
	panelAttr.ubVsyncPor	= LCD_SIG_POLARITY_L;
	panelAttr.ubRgbFmt		= RGB_D24BIT_RGB888;
	
	panelAttr.usDotClkRatio	= 7;
	panelAttr.usHBPorch		= 0x3F;
	panelAttr.usHBlanking	= 0x46;
	panelAttr.usHSyncW		= 0;
	panelAttr.usVBPorch		= 13;
	panelAttr.usVBlanking	= 22;
	panelAttr.usVSyncW		= 0;
	panelAttr.usPRT2HdotClk	= 2;
	
	panelAttr.bDeltaRBG		= MMP_TRUE;
	panelAttr.bDummyRBG		= MMP_TRUE;
	panelAttr.ubEvenLnOrder	= LCD_SPI_PIX_ORDER_RGB;
	panelAttr.ubOddLnOrder	= LCD_SPI_PIX_ORDER_RGB;
	
	panelAttr.usSpi2MciRatio = 0x80;
	panelAttr.usCsSetupCyc	= 0x0F;
	panelAttr.usCsHoldCyc	= 0x0F;
	panelAttr.usCsHighWidth	= 0xFF;
	panelAttr.usSPIRegBitCnt= SPI_PANEL_16BITS;

	panelAttr.ubDispWinId	= LCD_DISP_WIN_PIP;
	panelAttr.usWinStX		= 0;
	panelAttr.usWinStY		= 0;
	panelAttr.usWinW		= 160;
	panelAttr.usWinH		= 240;
	panelAttr.usWinBPP		= 2;
	panelAttr.usWinFmt		= LCD_WIN_FMT_16BPP;
	panelAttr.ulWinAddr		= 0x01500000;
	
	m_PanelAttr = panelAttr;
	
	MMPF_LCD_InitPanel(&panelAttr); 
}

void RTNA_LCD_Init2ndLCD(void)
{
	MMPF_PANEL_ATTR panelAttr;

	panelAttr.usPanelW 		= 320;
	panelAttr.usPanelH 		= 240;
	panelAttr.ubDevType 	= LCD_TYPE_RGBLCD;//LCD_TYPE_RGB2LCD
	panelAttr.ubController	= LCD_SCD_CONTROLER;
	
	panelAttr.pInitSeq		= NULL;	
	panelAttr.pIdxCmdSeq	= NULL;
		
	panelAttr.bPartialDisp	= MMP_FALSE;
	panelAttr.ubHsyncPor	= LCD_SIG_POLARITY_L;
	panelAttr.ubVsyncPor	= LCD_SIG_POLARITY_L;
	panelAttr.ubRgbFmt		= RGB_D24BIT_RGB888;
	
	panelAttr.usDotClkRatio	= 7;//5
	panelAttr.usHBPorch		= 0x3F;
	panelAttr.usHBlanking	= 0x46;
	panelAttr.usHSyncW		= 0;
	panelAttr.usVBPorch		= 13;
	panelAttr.usVBlanking	= 22;
	panelAttr.usVSyncW		= 0;
	panelAttr.usPRT2HdotClk	= 2;
	
	panelAttr.bDeltaRBG		= MMP_TRUE;
	panelAttr.bDummyRBG		= MMP_TRUE;
	panelAttr.ubEvenLnOrder	= LCD_SPI_PIX_ORDER_RGB;
	panelAttr.ubOddLnOrder	= LCD_SPI_PIX_ORDER_RGB;
	
	panelAttr.usSpi2MciRatio = 0x80;
	panelAttr.usCsSetupCyc	= 0x0F;
	panelAttr.usCsHoldCyc	= 0x0F;
	panelAttr.usCsHighWidth	= 0xFF;
	panelAttr.usSPIRegBitCnt= SPI_PANEL_16BITS;

	panelAttr.ubDispWinId	= LCD_DISP_WIN_OSD;
	panelAttr.usWinStX		= 0;
	panelAttr.usWinStY		= 0;
	panelAttr.usWinW		= 160;
	panelAttr.usWinH		= 240;
	panelAttr.usWinBPP		= 2;
	panelAttr.usWinFmt		= LCD_WIN_FMT_16BPP;
	panelAttr.ulWinAddr		= 0x02500000;
	
	m_PanelAttr = panelAttr;
	
	MMPF_LCD_InitPanel(&panelAttr);   
}

void RTNA_LCD_Init(void)
{
	MMPF_PANEL_ATTR panelAttr;

#if (SECONDARY_DISPLAY == 1) 
    RTNA_LCD_Init2ndLCD();   
#else
    RTNA_LCD_InitMainLCD();
#endif
	
	MMPF_LCD_Write16BitCmd(&m_PanelAttr, 0x051E);   
	Delayms(10); 
	MMPF_LCD_Write16BitCmd(&m_PanelAttr, 0x055D);
	MMPF_LCD_Write16BitCmd(&m_PanelAttr, 0x2B01); 
	Delayms(10); 
	MMPF_LCD_Write16BitCmd(&m_PanelAttr, 0x000A);   
	MMPF_LCD_Write16BitCmd(&m_PanelAttr, 0x0B81);   
	MMPF_LCD_Write16BitCmd(&m_PanelAttr, 0x0194);   
	MMPF_LCD_Write16BitCmd(&m_PanelAttr, 0x041B);   
	MMPF_LCD_Write16BitCmd(&m_PanelAttr, 0x0695);   
	MMPF_LCD_Write16BitCmd(&m_PanelAttr, 0x07F0);
	MMPF_LCD_Write16BitCmd(&m_PanelAttr, 0x1600);
	MMPF_LCD_Write16BitCmd(&m_PanelAttr, 0x1777);
    MMPF_LCD_Write16BitCmd(&m_PanelAttr, 0x1855);
	MMPF_LCD_Write16BitCmd(&m_PanelAttr, 0x1901);
	MMPF_LCD_Write16BitCmd(&m_PanelAttr, 0x1A20);
	Delayms(10); 
    // 1st cmd
        
    
 
    
    

   
}


//=====================================================================//
void RTNA_LCD_RGB_Test(void)
{	
	MMPF_LCD_DrawTestPattern(&m_PanelAttr);
}

void RTNA_LCD_Direction(LCD_DIR dir)
{
	//TBD
}

void RTNA_LCD_SetReg(MMP_ULONG reg, MMP_UBYTE value)
{
	MMP_SHORT Tempvalue;	
	Tempvalue = (((reg & 0xff)<< 8) + value );
	MMPF_LCD_Write16BitCmd(&m_PanelAttr, Tempvalue);
}

void RTNA_LCD_GetReg(MMP_ULONG reg, MMP_ULONG *value)
{
	//TBD
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

MMPF_PANEL_ATTR* RTNA_LCD_GetAttr(void)
{
	return &m_PanelAttr;
}
 

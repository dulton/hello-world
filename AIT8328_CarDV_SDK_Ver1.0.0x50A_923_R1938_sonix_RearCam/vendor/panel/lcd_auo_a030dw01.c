//==============================================================================
//
//  File        : lcd_chimei_wvga.c
//  Description : SONY 240x320 LCD Panel Control Function
//  Author      : Philip Lin
//  Revision    : 1.1
//
//==============================================================================



//==============================================================================
//
//                              MACRO for different environment
//
//==============================================================================

#ifdef BUILD_CE
#undef BUILD_FW
#endif

#ifdef BUILD_FW

#include "lib_retina.h"
#include "mmpf_reg_display.h"
#include "mmpf_reg_gbl.h"

#define  LCD_DECL               AITPS_LCD   pLCD  = AITC_BASE_LCD
#define  GBL_DECL               AITPS_GBL   pGBL  = AITC_BASE_GBL
#define  LCD_WAIT(cnt)          RTNA_WAIT_MS(cnt)
#define  LCD_CMD(idx, cmd)      RTNA_LCD_BurstCmd(idx, cmd)

#define  GBL_SET_B(reg, val)    (pGBL->##reg) = (val)
#define  GBL_GET_B(reg)         (pGBL->##reg)

#define  LCD_GET_W(reg)         (pLCD->##reg)
#define  LCD_GET_B(reg)         (pLCD->##reg)
#define  LCD_SET_B(reg, val)    (pLCD->##reg) = (val)
#define  LCD_SET_W(reg, val)    (pLCD->##reg) = (val)
#define  LCD_SET_D(reg, val)    (pLCD->##reg) = (val)
#define  MEM_SET_W(addr, val)   *(MMP_USHORT*)(addr) = (val);

#define  PIP_FB0                g_PIP_fb0   // refer to BSP\lib_retina.c

#else

#include "ait_utility.h"
#include "mmp_lib.h"

#include "mmph_hif.h"
#include "mmp_reg_display.h"
#include "mmp_reg_gbl.h"
#include "mmpd_system.h"

#define  LCD_DECL
#define  LCD_WAIT(cnt)          
#define  LCD_CMD(idx, cmd)      MMPD_LCD_SendIndexCommand(idx, cmd)

#define  GBL_SET_B(reg, val)    MMPH_HIF_RegSetB(reg, val)
#define  GBL_GET_B(reg)         MMPH_HIF_RegGetB(reg)

#define  LCD_GET_W(reg)         MMPH_HIF_RegGetW(reg)
#define  LCD_GET_B(reg)         MMPH_HIF_RegGetB(reg)
#define  LCD_SET_B(reg, val)    MMPH_HIF_RegSetB(reg, val)
#define  LCD_SET_W(reg, val)    MMPH_HIF_RegSetW(reg, val)
#define  LCD_SET_D(reg, val)    MMPH_HIF_RegSetL(reg, val)
#define  MEM_SET_W(addr, val)   MMPH_HIF_MemSetW(addr, val)

#if (CHIP == P_V2)
#define  PIP_FB0                  0x00132000
#endif

#endif

#define LCD_SDI_PAD_0 (0)
#define LCD_SDI_PAD_1 (1)

#define UPS051_MODE (0)
#define UPS052_MODE (1)

#define RESOL_320X240 (0)
#define RESOL_360X240 (1)

#define A030DW01_MODE UPS052_MODE
#define A030DW01_RESOL RESOL_360X240
#define USE_LCD_SDI_PAD LCD_SDI_PAD_1

//==============================================================================
//
//                              CONSTANTS & VARIABLES
//
//==============================================================================

//[GLOBAL] LCD configuration
#if (A030DW01_MODE == UPS051_MODE)
MMP_USHORT  g_PANL_w  = 320;
#endif

#if (A030DW01_MODE == UPS052_MODE)
#if (A030DW01_RESOL == RESOL_320X240)
MMP_USHORT  g_PANL_w  = 320;
#endif
#if (A030DW01_RESOL == RESOL_360X240)
MMP_USHORT  g_PANL_w  = 360;
#endif
#endif

#if (CHIP == P_V2)
MMP_USHORT  g_PANL_h  = 240;
#endif

MMP_USHORT  g_PIP_w   = 320;

#if (CHIP == P_V2)
MMP_USHORT  g_PIP_h   = 240;
#endif
MMP_USHORT  g_PIP_bpp = 2; // byte per pixel
//[GLOBAL]

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================
#pragma O0
void RTNA_LCD_WaitCount(unsigned int count)
{
    while(count--);
}
#pragma

void RTNA_LCD_Init(void)
{
    
#if (CHIP == P_V2)
    LCD_DECL;
    
    MMPH_HIF_RegSetB(DSPY_RGB_SYNC_MODE_V1, DSPY_RGB_SYNC_MODE_EN);
    MMPH_HIF_RegSetB(DSPY_RGB_CTL, MMPH_HIF_RegGetB(DSPY_RGB_CTL) & ~(RGB_IF_EN));
    while(MMPH_HIF_RegGetB(DSPY_RGB_CTL) & RGB_IF_EN);
    
#if (SECONDARY_DISPLAY == 1) 
    LCD_SET_W(DSPY_CTL_2,       DSPY_SCD_SEL(DSPY_TYPE_RGB_LCD) |
                                    DSPY_SCD_EN);
#else
    LCD_SET_W(DSPY_CTL_2,       PRM_DSPY_REG_READY   |
                                    DSPY_PRM_SEL(DSPY_TYPE_RGB_LCD) |
                                    DSPY_PRM_EN);
#endif

    
    #if 1
    LCD_SET_B(DSPY_RGB_CTL,     //PRT_DATA_MODE |
                                //DOT_POLAR_NEG | 
                                VSYNC_POLAR_LOW |
                                //HSYNC_POLAR_LOW
                                HSYNC_POLAR_LOW
                                /* |
                                PARTIAL_MODE_EN*/);
    #else
    LCD_SET_B(DSPY_RGB_CTL,     //PRT_DATA_MODE |
                                //DOT_POLAR_NEG | 
                                VSYNC_POLAR_HIGH |
                                HSYNC_POLAR_HIGH/* |
                                PARTIAL_MODE_EN*/);
    #endif
    
    //LCD_SET_B(DSPY_RGB_FMT,     RGB_D24BIT_RGB666);
    LCD_SET_B(DSPY_RGB_FMT,     RGB_D24BIT_RGB888);

//    LCD_SET_B(DSPY_RGB_DOT_CLK, 16);
//    LCD_SET_B(DSPY_RGB_DOT_CLK, 6);
//    LCD_SET_B(DSPY_RGB_DOT_CLK, 3);
//    LCD_SET_B(DSPY_RGB_DOT_CLK, 9);
    LCD_SET_B(DSPY_RGB_DOT_CLK, 6);
    
    #if (A030DW01_MODE == UPS051_MODE)
    LCD_SET_B(DSPY_RGB_H_BPORCH, 23);
    
    LCD_SET_B(DSPY_RGB_PORCH_HIGH_BIT_V1, 0x00);

    LCD_SET_B(DSPY_RGB_H_BLANK, 252);
    #endif
    #if (A030DW01_MODE == UPS052_MODE)
    LCD_SET_B(DSPY_RGB_H_BPORCH, 60);
    
    LCD_SET_B(DSPY_RGB_PORCH_HIGH_BIT_V1, 0x00);

    LCD_SET_B(DSPY_RGB_H_BLANK, 70);
    #endif
    LCD_SET_B(DSPY_RGB_HSYNC_W, 0x00);
    
    LCD_SET_B(DSPY_RGB_V_BPORCH, 0x15);
    LCD_SET_B(DSPY_RGB_V_BLANK, 0x16);
    LCD_SET_B(DSPY_RGB_VSYNC_W, 0);

    LCD_SET_B(DSPY_RGB_V_2_H_DOT, 1);
    LCD_SET_B(DSPY_RGB_PRT_2_H_DOT, 2);
	

#if (SECONDARY_DISPLAY == 1) 
    LCD_SET_W(DSPY_SCD_W,          g_PANL_w);
    LCD_SET_W(DSPY_SCD_H,          g_PANL_h);
    LCD_SET_D(DSPY_SCD_PIXL_CNT,   g_PANL_w * g_PANL_h);
#else
    LCD_SET_W(DSPY_W,          g_PANL_w);
    LCD_SET_W(DSPY_H,          g_PANL_h);
    LCD_SET_D(DSPY_PIXL_CNT,   g_PANL_w * g_PANL_h);
#endif    
    
    #if (A030DW01_MODE == UPS051_MODE)
    LCD_SET_B(DSPY_RGB_DELTA_MODE, RGB_DELTA_MODE_ENABLE | SPI_ODD_LINE_RGB | SPI_EVEN_LINE_GBR);
    #endif
    #if (A030DW01_MODE == UPS052_MODE)
    LCD_SET_B(DSPY_RGB_DELTA_MODE, RGB_DELTA_MODE_ENABLE | RGB_DUMMY_MODE_ENABLE);
    #endif

	LCD_SET_B(DSPY_RGB_CTL, LCD_GET_B(DSPY_RGB_CTL) | RGB_IF_EN);

    // LCD Scaling Setting
    LCD_SET_W(DSPY_SCAL_IN_THR,     16);
    LCD_SET_W(DSPY_SCAL_CTL,       DSPY_SCAL_BYPASS);
    LCD_SET_W(DSPY_SOUT_CTL,       DSPY_SOUT_RGB_EN);
    LCD_SET_W(DSPY_SEDGE_CTL,      DSPY_SEDGE_BYPASS);

#if (SECONDARY_DISPLAY == 1) 
    LCD_SET_W(DSPY_CTL_0,   SCD_DSPY_REG_READY);
#else
    LCD_SET_B(DSPY_RGB_SHARE_P_LCD_BUS,      RGB_LCD_ONLY);
     
    //0xF800
    //LCD_SET_D( DSPY_BG_COLR,    0x00FF0000 );
    LCD_SET_D( DSPY_BG_COLR,    0xFFFFFFFF );
    
   	GBL_SET_B(GBL_MIO_LCD_CTL, GBL_GET_B(GBL_MIO_LCD_CTL) | RGB_LCD_IF_EN);
   	
#endif

    #if (USE_LCD_SDI_PAD == LCD_SDI_PAD_0)
    LCD_SET_B(GBL_LCD_SPI_PAD_CTL, 0x1);
    #endif
    #if (USE_LCD_SDI_PAD == LCD_SDI_PAD_1)
    LCD_SET_B(GBL_LCD_SPI_PAD_CTL, 0x2);
    #endif
    //LCD_SET_B(0x80007003, 0x12);
    LCD_SET_B(DSPY_RGB_SPI_CTL, 0x60);
    LCD_SET_B(DSPY_RGB_RATIO_SPI_MCI, 0x0B);
    LCD_SET_B(DSPY_RGB_SPI_CS_SETUP_CYCLE, 0x0B);
    LCD_SET_B(DSPY_RGB_SPI_CS_HOLD_CYCLE, 0x0B);
    LCD_SET_B(DSPY_RGB_SPI_CS_HIGH_WIDTH, 0xff);
    LCD_SET_B(DSPY_RGB_SPI_CONTROL_REGISTER1, RGB_SPI_DATA_ONLY_MODE);
    LCD_SET_B(DSPY_PLCD_IDX_CMD_NUM, 0x01);
    // 1st cmd
    LCD_SET_W(0x80007010, 0x052C);
    LCD_SET_B(0x80007000, 0x4);
    while(LCD_GET_B(0x80007000) & 0x04);
    RTNA_LCD_WaitCount(1000000);
    // 2nd cmd
    LCD_SET_W(DSPY_LCD_TX_0, 0x056C);
    LCD_SET_B(DSPY_CTL_0, LCD_IDX_RDY);
    while(LCD_GET_B(DSPY_CTL_0) & LCD_IDX_RDY);
    RTNA_LCD_WaitCount(1000000);
    // 3rd cmd
    LCD_SET_W(DSPY_LCD_TX_0, 0x0197);
    LCD_SET_B(DSPY_CTL_0, LCD_IDX_RDY);
    while(LCD_GET_B(DSPY_CTL_0) & LCD_IDX_RDY);
    // 4th cmd
    #if (A030DW01_MODE == UPS051_MODE)
    LCD_SET_W(DSPY_LCD_TX_0, 0x040B); //320x240, UPS051
    #endif
    #if (A030DW01_MODE == UPS052_MODE)
        #if (A030DW01_RESOL == RESOL_320X240)
        LCD_SET_W(DSPY_LCD_TX_0, 0x041B); //320x240, UPS052
        #endif
        #if (A030DW01_RESOL == RESOL_360X240)
        LCD_SET_W(DSPY_LCD_TX_0, 0x042B); //360x240, UPS052
        #endif
    #endif
    LCD_SET_B(DSPY_CTL_0, LCD_IDX_RDY);
    while(LCD_GET_B(DSPY_CTL_0) & LCD_IDX_RDY);
    // 5th cmd
    LCD_SET_W(DSPY_LCD_TX_0, 0x0695);
    LCD_SET_B(DSPY_CTL_0, LCD_IDX_RDY);
    while(LCD_GET_B(DSPY_CTL_0) & LCD_IDX_RDY);
    
    #if (A030DW01_MODE == UPS051_MODE)
    LCD_SET_W(DSPY_LCD_TX_0, 0x0745);
    #endif
    #if (A030DW01_MODE == UPS052_MODE)
    LCD_SET_W(DSPY_LCD_TX_0, 0x07F0);
    #endif
    LCD_SET_B(DSPY_CTL_0, LCD_IDX_RDY);
    while(LCD_GET_B(DSPY_CTL_0) & LCD_IDX_RDY);
    // 6th cmd
    LCD_SET_W(DSPY_LCD_TX_0, 0x056d);
    LCD_SET_B(DSPY_CTL_0, LCD_IDX_RDY);
    while(LCD_GET_B(DSPY_CTL_0) & LCD_IDX_RDY);
    RTNA_LCD_WaitCount(1000000);
#endif
   
}


//=====================================================================//
void RTNA_LCD_RGB_Test(void)
{
#if 1
    int     x, y;
    MMP_USHORT  lcd_pat[] = {0xF800, 0x07E0, 0x001F, 0xFFFF};
    //MMP_USHORT  lcd_pat[] = {0x07E0, 0xF800, 0xFFFF,  0x001F};
    //MMP_USHORT  lcd_pat[] = {0x001F, 0x001F, 0x001F, 0x001F};

    LCD_DECL;

    LCD_SET_W( DSPY_WIN_PRIO,       (ICON_WIN  << WIN_1_SHFT) |
                                    (OVLY_WIN  << WIN_2_SHFT) |
                                    (PIP_WIN   << WIN_3_SHFT) |
                                    (MAIN_WIN  << WIN_4_SHFT) );


#if (SECONDARY_DISPLAY == 1) 
    LCD_SET_D( DSPY_SCD_BG_COLR,    0x00000000 );

    LCD_SET_W( DSPY_SCD_CTL,         LCD_FRAME_TX_SETADDR_EN |
                                    SCD_SRC_RGB565 | 
                                    SCD_565_2_888_STUFF_0);

    LCD_SET_D( DSPY_SCD_WIN_ADDR_ST,   PIP_FB0 );
    LCD_SET_D( DSPY_SCD_WIN_OFST_ST,     0 );
    LCD_SET_W( DSPY_SCD_WIN_W,           g_PIP_w );
    LCD_SET_W( DSPY_SCD_WIN_H,           g_PIP_h );
    LCD_SET_W( DSPY_SCD_WIN_X,           0 );
    LCD_SET_W( DSPY_SCD_WIN_Y,           0 );
    LCD_SET_D( DSPY_SCD_WIN_PIXL_CNT,    g_PIP_w * g_PIP_h);
#else
    LCD_SET_D( DSPY_BG_COLR,    0xFFFFFFFF );

    LCD_SET_W( DSPY_PIP_FMT,         WIN_16BPP);
    //LCD_SET_D( DSPY_PIP_TP_COLR,     0x00000000 );

    LCD_SET_D( DSPY_PIP_0_ADDR_ST,   PIP_FB0 );
    LCD_SET_D( DSPY_PIP_OFST_ST,     0 );
    LCD_SET_W( DSPY_PIP_OFST_PIXL,   g_PIP_bpp );                        // byte unit
    LCD_SET_W( DSPY_PIP_OFST_ROW,    g_PIP_w * g_PIP_bpp );              // byte unit
    LCD_SET_W( DSPY_PIP_W,           g_PIP_w );
    LCD_SET_W( DSPY_PIP_H,           g_PIP_h );
    LCD_SET_W( DSPY_PIP_X,           0 );
    LCD_SET_W( DSPY_PIP_Y,           0 );
    LCD_SET_D( DSPY_PIP_PIXL_CNT,    g_PIP_w * g_PIP_h);
#endif


    //[Jerry]
    // When PIP format is YUV (422,420) it's forced to be scaling path
    // Scaling bypass only controls the N/M and LPF, all regsiter should
    // be correct when using YUV format

    // LCD Scaling Setting
    LCD_SET_W( DSPY_SIN_W,           g_PIP_w );
    LCD_SET_W( DSPY_SIN_H,           g_PIP_h );
    LCD_SET_D( DSPY_SOUT_GRAB_PIXL_CNT,   g_PIP_w * g_PIP_h );
    LCD_SET_W( DSPY_SOUT_GRAB_H_ST,  1 );
    LCD_SET_W( DSPY_SOUT_GRAB_H_ED,  g_PIP_w );
    LCD_SET_W( DSPY_SOUT_GRAB_V_ST,  1 );
    LCD_SET_W( DSPY_SOUT_GRAB_V_ED,  g_PIP_h );


    // Display different pattern for HOST and FW
    #ifdef BUILD_FW
	// Draw 4 color blocks: Red/Green/Blue/White
    for (y = 0; y < 4; y++) {
        for (x = 0; x < g_PIP_w*g_PIP_h/4; x++) {
            MEM_SET_W((PIP_FB0 + g_PIP_bpp*(y*g_PIP_w*g_PIP_h/4+x)), lcd_pat[y]);
        }
    }
    // Draw first 3 even line as Yellow color
    for (y = 0; y < 15; y += 5) {
    	for (x = 0; x < g_PIP_w; x++) {
        	MEM_SET_W( (PIP_FB0 + (g_PIP_bpp*(g_PIP_w*y + x))), 0xFFE0 );
    	}
    }

    #else
/*
    for (y = 0; y < g_PIP_h; y++) {
    	// Red block
        for (x = 0; x < g_PIP_w; x++) {
            MEM_SET_W((PIP_FB0 + g_PIP_bpp*(g_PIP_w*y+x)), lcd_pat[0]);
        }
    }
*/   
/*
    for (y = 0; y < 80; y++) {
    	// Red block
        for (x = 0; x < 800; x++) {
            MEM_SET_W((PIP_FB0 + g_PIP_bpp*(g_PIP_w*y+x)), lcd_pat[0]);
        }
    } 
*/    
    
    for (y = 0; y < g_PIP_h/2; y++) {
    	// Red block
        for (x = 0; x < g_PIP_w/2; x++) {
            MEM_SET_W((PIP_FB0 + g_PIP_bpp*(g_PIP_w*y+x)), lcd_pat[0]);
        }
        // Green block
        for (x = g_PIP_w/2; x < g_PIP_w; x++) {
            MEM_SET_W((PIP_FB0 + g_PIP_bpp*(g_PIP_w*y+x)), lcd_pat[1]);
        }
    }
    for (y = g_PIP_h/2; y < g_PIP_h; y++) {
    	// Blue block
        for (x = 0; x < g_PIP_w/2; x++) {
            MEM_SET_W((PIP_FB0 + g_PIP_bpp*(g_PIP_w*y+x)), lcd_pat[2]);
        }
        // White block
        for (x = g_PIP_w/2; x < g_PIP_w; x++) {
            MEM_SET_W((PIP_FB0 + g_PIP_bpp*(g_PIP_w*y+x)), lcd_pat[3]);
        }
    }
/*
    // Draw first 3 even line as Yellow color
    for (y = 0; y < 15; y += 5) {
    	for (x = 0; x < g_PIP_w/2; x++) {
        	MEM_SET_W( (PIP_FB0 + (g_PIP_bpp*(g_PIP_w*y + x))), 0xFFE0 );
    	}
    }
*/    

	#endif

#if (SECONDARY_DISPLAY == 1) 
#else
    LCD_SET_W( DSPY_PIP_CTL,         WIN_EN );
#endif

#endif

    return;
}

void RTNA_LCD_InitMainLCD(void)
{
    return;	
}



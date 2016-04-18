//==============================================================================
//
//  File        : lcd_none.c
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

#include "lcd_common.h"

#ifdef BUILD_FW

#include "reg_retina.h"
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
//#define  LCD_WAIT(cnt)          
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
#define  PIP_FB0                  0x01320000
#endif

#endif

#include "mmps_vidplay.h"
#include "mmps_audio.h"

#define LCD_SDI_PAD_0 (0)
#define LCD_SDI_PAD_1 (1)

#define USE_LCD_SDI_PAD LCD_SDI_PAD_1

#define ENABLE_LCD_ILI9242_LOG (0)

typedef enum _LCDDIR{ //For compiler
	UPLEFT_DOWNRIGHT = 0,
	DOWNLEFT_UPRIGHT,
	UPRIGHT_DOWNLEFT,
	DOWNRIGHT_UPLEFT
}LCDDIR;

//==============================================================================
//
//                              GLOBAL VARIABLE
//
//==============================================================================

static MMPF_PANEL_ATTR m_PanelAttr = 
{
	320, 
	240, 
	LCD_TYPE_MAX,   
	LCD_PRM_CONTROLER, 
	0,
	
	NULL, 
	NULL,
	
	LCD_BUS_WIDTH_8, 
	LCD_PHASE0, 
	LCD_POLARITY0, 
	LCD_MCU_80SYS, 
	0, 
	0, 
	0, 
	LCD_RGB_ORDER_RGB,
		
	MMP_FALSE, 
	LCD_SIG_POLARITY_L, 
	LCD_SIG_POLARITY_L, 
	RGB_D24BIT_RGB888,
	{0}
};


//==============================================================================
//
//                              CONSTANTS & VARIABLES
//
//==============================================================================

//[GLOBAL] LCD configuration
MMP_USHORT  g_PANL_w  = 320;

#if (CHIP == P_V2)
MMP_USHORT  g_PANL_h  = 240;
#endif

MMP_USHORT  g_PIP_w   = 320;

#if (CHIP == P_V2)
MMP_USHORT  g_PIP_h   = 240;
#endif
MMP_USHORT  g_PIP_bpp = 2; // byte per pixel

void RTNA_LCD_InitMainLCD(void);
void RTNA_LCD_Init(void)
{
    RTNA_LCD_InitMainLCD();
}

void RTNA_LCD_RGB_Test(void){return;}
void RTNA_LCD_InitMainLCD(void)
{
    
#if (CHIP == P_V2)

    LCD_DECL;
    MMPH_HIF_RegSetB(DSPY_RGB_SYNC_MODE_V1, DSPY_RGB_SYNC_MODE_EN);
    MMPH_HIF_RegSetB(DSPY_RGB_CTL, MMPH_HIF_RegGetB(DSPY_RGB_CTL) & ~(RGB_IF_EN));
    while(MMPH_HIF_RegGetB(DSPY_RGB_CTL) & RGB_IF_EN);
    
    // Turn on RGB IF clock
    MMPH_HIF_RegSetB(0x80008c75, MMPH_HIF_RegGetB(0x80008c75) | 0x02);

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
   	LCD_SET_B(DSPY_RGB_DOT_CLK, 13);//rr 73a4
    LCD_SET_B(DSPY_RGB_H_BPORCH, 7);   
    LCD_SET_B(DSPY_RGB_PORCH_HIGH_BIT_V1, 0x00);
    LCD_SET_B(DSPY_RGB_H_BLANK, 10);   
    LCD_SET_B(DSPY_RGB_HSYNC_W, 0x02);    
    LCD_SET_B(DSPY_RGB_V_BPORCH, 1);
    LCD_SET_B(DSPY_RGB_V_BLANK, 5);
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
    
    LCD_SET_B(DSPY_RGB_DELTA_MODE, RGB_DELTA_MODE_ENABLE | SPI_ODD_LINE_RGB | SPI_EVEN_LINE_RGB);
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
    LCD_SET_B(DSPY_RGB_SPI_CTL, 0x20);
    LCD_SET_B(DSPY_RGB_RATIO_SPI_MCI, 0x0F);
    LCD_SET_B(DSPY_RGB_SPI_CS_SETUP_CYCLE, 0x0B);
    LCD_SET_B(DSPY_RGB_SPI_CS_HOLD_CYCLE, 0x0B);
    LCD_SET_B(DSPY_RGB_SPI_CS_HIGH_WIDTH, 0xff);
    LCD_SET_B(DSPY_RGB_SPI_CONTROL_REGISTER1, RGB_SPI_DATA_ONLY_MODE);
    LCD_SET_B(DSPY_PLCD_IDX_CMD_NUM, 0x01);    
#endif   
}
void RTNA_LCD_AdjustBrightness(MMP_UBYTE level){return;}
void RTNA_LCD_AdjustContrast(MMP_UBYTE level){return;}
void RTNA_LCD_AdjustContrast_R(MMP_BYTE level){return;}
void RTNA_LCD_AdjustBrightness_R(MMP_UBYTE level){return;}
void RTNA_LCD_AdjustContrast_B(MMP_BYTE level){return;}
void RTNA_LCD_AdjustBrightness_B(MMP_UBYTE level){return;}
void RTNA_LCD_Direction(LCD_DIR dir){return;}
void RTNA_LCD_SetReg(MMP_ULONG reg, MMP_UBYTE value){return;}
void RTNA_LCD_GetReg(MMP_ULONG reg, MMP_ULONG *value){return;}
void RTNA_LCD_Show_Image(MMP_USHORT *psrc_buf){return;}
void RTNA_LCD_ReduceHblank(MMP_BYTE enable){return;}


void RTNA_LCD_DispAll(MMP_UBYTE* map){return;}
void RTNA_LCD_DisplayOn(void){}
void RTNA_LCD_SetDisplayOnOff(MMP_UBYTE OnOff){return;}
void RTNA_LCD_SOFT_Reset(void){}
void RTNA_LCD_DispALL(MMP_UBYTE* map){return;}

#include "disp_drv.h"

DISP_PROP	this_p = {720, 480, 16, DISP_NTSC};

DISP_PROP* RTNA_GetDisplayProp()
{
	return &this_p;	
}

MMPF_PANEL_ATTR* RTNA_LCD_GetAttr(void)
{
	return &m_PanelAttr;
}

#if 0
void RTNA_LCD_AdjustBrightness(MMP_UBYTE level)
{
	//MMPF_LCD_Write16BitCmd(&m_PanelAttr, 0x0300 | level); 	// Brightness(R03h[7:0]): RGB brightness level control
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
	//MMPF_LCD_Write16BitCmd(&m_PanelAttr, 0x0D00 | level); 	// CONTRAST(R0Dh[7:0]): RGB contrast level setting, the gain changes (1/64) bit.
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
#endif
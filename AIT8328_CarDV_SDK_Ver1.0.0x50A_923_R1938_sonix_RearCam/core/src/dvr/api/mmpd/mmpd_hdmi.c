//==============================================================================
//
//  File        : mmpd_hdmi.c
//  Description : Ritian HDMI Control driver function
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

/**
*  @file mmpd_hdmi.c
*  @brief The HDMI control functions
*  @author Penguin Torng
*  @version 1.0
*/

#ifdef BUILD_CE
#undef BUILD_FW
#endif

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "mmpd_display.h"
#include "mmpd_system.h"
#include "mmph_hif.h"
#include "mmp_reg_gbl.h"
#include "mmp_reg_display.h"
#include "ait_utility.h"
#include "mmp_reg_hdmi.h"
#include "mmpf_display.h"

/** @addtogroup MMPD_Display
 *  @{
 */

//==============================================================================
//
//                              GLOBAL VARIABLES
//
//==============================================================================

static MMP_UBYTE m_ubHDMIClk = 0;
static MMP_UBYTE m_ubHDMIClk2 = 0;

//==============================================================================
//
//                              EXTERN VARIABLES
//
//==============================================================================

extern MMP_BOOL m_bHdmiRgbIF1;

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

#if 0
void ____HDMI_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_SetHDMIOutput
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Display_SetHDMIOutput(	MMP_DISPLAY_CONTROLLER 	controller, 
									MMP_HDMI_ATTR 	*hdmiAttr, 
									MMP_DISPLAY_RGB_IF 		rgbIf)
{
#if defined(ALL_FW)
    if (rgbIf == MMP_DISPLAY_RGB_IF1)
    {
        m_bHdmiRgbIF1 = MMP_TRUE;

        MMPD_System_EnableClock(MMPD_SYS_CLK_HDMI, MMP_TRUE);

        MMPH_HIF_RegSetB(DSPY_RGB_FMT, MMPH_HIF_RegGetB(DSPY_RGB_FMT) & ~(DSPY_RGB_SYNC_MODE_DIS));
        MMPH_HIF_RegSetB(DSPY_RGB_CTL, MMPH_HIF_RegGetB(DSPY_RGB_CTL) & ~(RGB_IF_EN));
        while(MMPH_HIF_RegGetB(DSPY_RGB_CTL) & RGB_IF_EN);

        MMPH_HIF_RegSetB(DSPY_RGB_DELTA_MODE, 0x00);
        MMPH_HIF_RegSetW(DSPY_CTL_4, MMPH_HIF_RegGetW(DSPY_CTL_4) & ~LCD_OUT_SEL_MASK);
        MMPH_HIF_RegSetB(GBL_LCD_PAD_CFG, MMPH_HIF_RegGetB(GBL_LCD_PAD_CFG) & ~(GBL_LCD_RGB_SPI_MASK));

        MMPD_HDMI_InitAnalog(hdmiAttr);
        MMPD_HDMI_Initialize(hdmiAttr,0);
        
        MMPH_HIF_RegSetB(DSPY_RGB_SHARE_P_LCD_BUS,  RGBLCD_SRC_SEL_RGB);
        if (controller == MMP_DISPLAY_PRM_CTL) {
        
    		MMPH_HIF_RegSetW(DSPY_W, 				hdmiAttr->usDisplayWidth);
    		MMPH_HIF_RegSetW(DSPY_H, 				hdmiAttr->usDisplayHeight);
    	  	MMPH_HIF_RegSetW(DSPY_RGB_PART_ST_X, 	0);
      		MMPH_HIF_RegSetW(DSPY_RGB_PART_ST_Y, 	0);
      		MMPH_HIF_RegSetW(DSPY_RGB_PART_ED_X, 	hdmiAttr->usDisplayWidth);
    		MMPH_HIF_RegSetW(DSPY_RGB_PART_ED_Y, 	hdmiAttr->usDisplayHeight);
    		MMPH_HIF_RegSetL(DSPY_PIXL_CNT, 		hdmiAttr->usDisplayWidth * hdmiAttr->usDisplayHeight);
    		MMPH_HIF_RegSetL(DSPY_BG_COLOR, 		hdmiAttr->ulDspyBgColor);
            
       	    MMPH_HIF_RegSetW(DSPY_PIP_SOUT_CTL, DSPY_SOUT_RGB_888 | DSPY_SOUT_RGB);

    		MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) & ~(DSPY_PRM_SEL_MASK));
    		MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) | DSPY_PRM_SEL(DSPY_TYPE_RGB_LCD));
    	    MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) | DSPY_PRM_EN);
        	    
    		MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) | PRM_DSPY_REG_READY);
    	}
    	else {
    		MMPH_HIF_RegSetW(DSPY_SCD_W, 			hdmiAttr->usDisplayWidth);
    		MMPH_HIF_RegSetW(DSPY_SCD_H, 			hdmiAttr->usDisplayHeight);
    	  	MMPH_HIF_RegSetW(DSPY_RGB_PART_ST_X, 	0);
      		MMPH_HIF_RegSetW(DSPY_RGB_PART_ST_Y, 	0);
      		MMPH_HIF_RegSetW(DSPY_RGB_PART_ED_X, 	hdmiAttr->usDisplayWidth);
    		MMPH_HIF_RegSetW(DSPY_RGB_PART_ED_Y, 	hdmiAttr->usDisplayHeight);
    		MMPH_HIF_RegSetL(DSPY_SCD_PIXL_CNT, 	hdmiAttr->usDisplayWidth * hdmiAttr->usDisplayHeight);
    		MMPH_HIF_RegSetL(DSPY_SCD_BG_COLOR, 	hdmiAttr->ulDspyBgColor);

    	    //MMPH_HIF_RegSetW(DSPY_SCD_CTL, SCD_SRC_RGB565 |  SCD_565_2_888_STUFF_0);

    		MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) & ~(DSPY_SCD_SEL_MASK));
    		MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) | DSPY_SCD_SEL(DSPY_TYPE_RGB_LCD) | DSPY_SCD_EN);
    	    MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) | DSPY_SCD_EN);

    		MMPH_HIF_RegSetW(DSPY_CTL_0, MMPH_HIF_RegGetW(DSPY_CTL_0) | SCD_DSPY_REG_READY);
    	}

    	if(hdmiAttr->ubOutputMode == MMP_HDMI_OUTPUT_1920X1080I) {
            MMPH_HIF_RegSetW(DSPY_RGB1_LINE_CPU_INT_1, 0);
            MMPH_HIF_RegSetW(DSPY_RGB1_LINE_CPU_INT_2, 562);
            MMPH_HIF_RegSetB(DSPY_INT_CPU_EN_2, RGB_LINE_INT2);
        }
        else {
            MMPH_HIF_RegSetB(DSPY_INT_CPU_EN_2, 0x0);
        }

    	MMPH_HIF_RegSetB(DSPY_RGB_CTL, MMPH_HIF_RegGetB(DSPY_RGB_CTL) | RGB_IF_EN);
    }
    else if (rgbIf == MMP_DISPLAY_RGB_IF2)
    {
        m_bHdmiRgbIF1 = MMP_FALSE;

        MMPD_System_EnableClock(MMPD_SYS_CLK_HDMI, MMP_TRUE);

        MMPH_HIF_RegSetB(DSPY_RGB2_FMT, MMPH_HIF_RegGetB(DSPY_RGB2_FMT) & ~(DSPY_RGB_SYNC_MODE_DIS));
        MMPH_HIF_RegSetB(DSPY_RGB2_CTL, MMPH_HIF_RegGetB(DSPY_RGB2_CTL) & ~(RGB_IF_EN));
        while(MMPH_HIF_RegGetB(DSPY_RGB2_CTL) & RGB_IF_EN);

        MMPH_HIF_RegSetB(DSPY_RGB2_DELTA_MODE, 0x00);
        MMPH_HIF_RegSetW(DSPY_CTL_4, MMPH_HIF_RegGetW(DSPY_CTL_4) & ~LCD_OUT_SEL_MASK);
        MMPH_HIF_RegSetB(GBL_LCD_PAD_CFG, MMPH_HIF_RegGetB(GBL_LCD_PAD_CFG) & ~(GBL_LCD_RGB_SPI_MASK));

        MMPD_HDMI_InitAnalog(hdmiAttr);
        MMPD_HDMI_Initialize(hdmiAttr,2);
        
        MMPH_HIF_RegSetB(DSPY_RGB_SHARE_P_LCD_BUS,  RGBLCD_SRC_SEL_RGB2);
        if (controller == MMP_DISPLAY_PRM_CTL) {
        
    		MMPH_HIF_RegSetW(DSPY_W, 				hdmiAttr->usDisplayWidth);
    		MMPH_HIF_RegSetW(DSPY_H, 				hdmiAttr->usDisplayHeight);
    	  	MMPH_HIF_RegSetW(DSPY_RGB2_PART_ST_X, 	0);
      		MMPH_HIF_RegSetW(DSPY_RGB2_PART_ST_Y, 	0);
      		MMPH_HIF_RegSetW(DSPY_RGB2_PART_ED_X, 	hdmiAttr->usDisplayWidth);
    		MMPH_HIF_RegSetW(DSPY_RGB2_PART_ED_Y, 	hdmiAttr->usDisplayHeight);
    		MMPH_HIF_RegSetL(DSPY_PIXL_CNT, 		hdmiAttr->usDisplayWidth * hdmiAttr->usDisplayHeight);
    		MMPH_HIF_RegSetL(DSPY_BG_COLOR, 		hdmiAttr->ulDspyBgColor);
            
       	    MMPH_HIF_RegSetW(DSPY_PIP_SOUT_CTL, DSPY_SOUT_RGB_888 | DSPY_SOUT_RGB);

    		MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) & ~(DSPY_PRM_SEL_MASK));
    		MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) | DSPY_PRM_SEL(DSPY_TYPE_RGB_LCD));
    	    MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) | DSPY_PRM_EN);
        	    
    		MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) | PRM_DSPY_REG_READY);
    	}
    	else {
    		MMPH_HIF_RegSetW(DSPY_SCD_W, 			hdmiAttr->usDisplayWidth);
    		MMPH_HIF_RegSetW(DSPY_SCD_H, 			hdmiAttr->usDisplayHeight);
    	  	MMPH_HIF_RegSetW(DSPY_RGB2_PART_ST_X, 	0);
      		MMPH_HIF_RegSetW(DSPY_RGB2_PART_ST_Y, 	0);
      		MMPH_HIF_RegSetW(DSPY_RGB2_PART_ED_X, 	hdmiAttr->usDisplayWidth);
    		MMPH_HIF_RegSetW(DSPY_RGB2_PART_ED_Y, 	hdmiAttr->usDisplayHeight);
    		MMPH_HIF_RegSetL(DSPY_SCD_PIXL_CNT, 	hdmiAttr->usDisplayWidth * hdmiAttr->usDisplayHeight);
    		MMPH_HIF_RegSetL(DSPY_SCD_BG_COLOR, 	hdmiAttr->ulDspyBgColor);

    	    //MMPH_HIF_RegSetW(DSPY_SCD_CTL, SCD_SRC_RGB565 |  SCD_565_2_888_STUFF_0);

    		MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) & ~(DSPY_SCD_SEL_MASK));
    		MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) | DSPY_SCD_SEL(DSPY_TYPE_RGB_LCD) | DSPY_SCD_EN);
    	    MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) | DSPY_SCD_EN);

    		MMPH_HIF_RegSetW(DSPY_CTL_0, MMPH_HIF_RegGetW(DSPY_CTL_0) | SCD_DSPY_REG_READY);
    	}

        if(hdmiAttr->ubOutputMode == MMP_HDMI_OUTPUT_1920X1080I) {
            MMPH_HIF_RegSetW(DSPY_RGB2_LINE_CPU_INT_1, 0);
            MMPH_HIF_RegSetW(DSPY_RGB2_LINE_CPU_INT_2, 562);
            MMPH_HIF_RegSetB(DSPY_INT_CPU_EN_2, RGB_LINE_INT2);
        } 
        else {
            MMPH_HIF_RegSetB(DSPY_INT_CPU_EN_2, 0x0);
        }

    	MMPH_HIF_RegSetB(DSPY_RGB2_CTL, MMPH_HIF_RegGetB(DSPY_RGB2_CTL) | RGB_IF_EN);    
    }
    
	MMPD_HDMI_InitDigital(hdmiAttr);

	MMPF_Display_SetOutputDev(controller, MMP_DISPLAY_HDMI);
#endif
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_HDMI_DisableHDMI
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_HDMI_DisableHDMI(void)
{
    // PHY Power Down
    MMPH_HIF_RegSetB(HDMI_CTL_6, HDMI_POWER_DOWN);

    MMPH_HIF_RegSetB(DSPY_INT_CPU_EN_2, 0);
    MMPH_HIF_RegSetB(HDMI_CTL_8, 0);

    MMPH_HIF_RegSetB(DSPY_RGB_FMT, DSPY_RGB_SYNC_MODE_EN);
    MMPH_HIF_RegSetB(DSPY_RGB_CTL, MMPH_HIF_RegGetB(DSPY_RGB_CTL) & ~(RGB_IF_EN));
    while(MMPH_HIF_RegGetB(DSPY_RGB_CTL) & RGB_IF_EN);
    MMPH_HIF_RegSetB(DSPY_RGB2_CTL, MMPH_HIF_RegGetB(DSPY_RGB2_CTL) & ~(RGB_IF_EN));
    while(MMPH_HIF_RegGetB(DSPY_RGB2_CTL) & RGB_IF_EN);   
    MMPH_HIF_RegSetB(DSPY_INT_CPU_SR_2, RGB_LINE_INT2 | RGB_LINE_INT1);
    
    MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL0, 0x81);
    MMPC_System_WaitMmp(30);
    MMPH_HIF_RegSetB(HDMI_PHY_PLL_CTL0, 0x82);
    MMPH_HIF_RegSetW(HDMI_PHY_BANDGAP_CTL, MMPH_HIF_RegGetW(HDMI_PHY_BANDGAP_CTL) | BS_PWR_DOWN | BG_PWR_DOWN);

    if (gbEcoVer == 0) {
        /* Reset HDMI PHY to the initial state */
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL0, 0x00);
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL4, 0x00);
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL5, 0x00);
        MMPH_HIF_RegSetW(HDMI_PHY_BANDGAP_CTL, 0x00);
    }

    MMPH_HIF_RegSetB(GBL_HDMI_CLK_SRC, m_ubHDMIClk);
    MMPH_HIF_RegSetB(GBL_HDMI_CLK_DIV, m_ubHDMIClk2);

    MMPD_System_EnableClock(MMPD_SYS_CLK_HDMI, MMP_FALSE);
    return MMP_ERR_NONE;
}

#if defined(ALL_FW)
//------------------------------------------------------------------------------
//  Function    : MMPD_HDMI_InitAnalog
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_HDMI_InitAnalog(MMP_HDMI_ATTR *hdmiattribute)
{	
    m_ubHDMIClk = MMPH_HIF_RegGetB(GBL_HDMI_CLK_SRC);
    m_ubHDMIClk2 = MMPH_HIF_RegGetB(GBL_HDMI_CLK_DIV);
    
    MMPH_HIF_RegSetB(GBL_HDMI_CLK_SRC, HDMI_CLK_SRC_PIXL);
    MMPH_HIF_RegSetB(GBL_HDMI_CLK_DIV, 0x01);
    
    if (gbEcoVer == 0) {
        /* Must power down PHY before power on */
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL0, MMPH_HIF_RegGetB(HDMI_PHY_TMDS_CTL0) | 0x81);
        MMPC_System_WaitMmp(30);
        MMPH_HIF_RegSetB(HDMI_PHY_PLL_CTL0, MMPH_HIF_RegGetB(HDMI_PHY_PLL_CTL0) | 0x80);
        MMPH_HIF_RegSetW(HDMI_PHY_BANDGAP_CTL, BS_PWR_DOWN | BG_PWR_DOWN);
    }

    MMPC_System_WaitMmp(50);
    MMPH_HIF_RegSetB(HDMI_PHY_BANDGAP_CTL, 0x00);
    
    switch(hdmiattribute->ubOutputMode){
    case MMP_HDMI_OUTPUT_640X480P:
        //TBD????
        MMPH_HIF_RegSetB(HDMI_PHY_PLL_CTL3, 0x54); // HDMI PLL M value
        MMPH_HIF_RegSetB(HDMI_PHY_PLL_CTL4, 0x50); // HDMI PLL N value
        MMPH_HIF_RegSetB(HDMI_PHY_PLL_CTL1, HDMI_PHY_PHASE2);//phase2
        // HDMI PHY Setting
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL0, 0x15); // power dn TX (sync.)
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL1, 0x10);
        MMPH_HIF_RegSetB(HDMI_PHY_PLL_CTL0, MMPH_HIF_RegGetB(HDMI_PHY_PLL_CTL0) &~ 0x80);
        MMPF_OS_Sleep(1);
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL0, 0x35); // power on TX
        MMPF_OS_Sleep(1);                        
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL4, 0x00); // Enable data termination
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL5, 0x01); // Enable CLK termination
        break;
    case MMP_HDMI_OUTPUT_720X480P:
    case MMP_HDMI_OUTPUT_720X576P:
        MMPH_HIF_RegSetB(HDMI_PHY_PLL_CTL3, 0x5A); // HDMI PLL M value
        MMPH_HIF_RegSetB(HDMI_PHY_PLL_CTL4, 0x50); // HDMI PLL N value
        MMPH_HIF_RegSetB(HDMI_PHY_PLL_CTL1, HDMI_PHY_PHASE2);//phase2
        // HDMI PHY Setting
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL0, 0x11); // power dn TX (sync.)
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL1, 0x10);
        MMPH_HIF_RegSetB(HDMI_PHY_PLL_CTL0, MMPH_HIF_RegGetB(HDMI_PHY_PLL_CTL0) &~ 0x80);
        MMPF_OS_Sleep(1);
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL0, 0x31); // power on TX
        MMPF_OS_Sleep(1);                        
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL4, 0x00); // disable data termination
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL5, 0x00); // disable CLK termination
        break;
    case MMP_HDMI_OUTPUT_1280X720P:
    case MMP_HDMI_OUTPUT_1280X720P_50FPS:
    case MMP_HDMI_OUTPUT_1920X1080P_30FPS:
    case MMP_HDMI_OUTPUT_1920X1080I:
        MMPH_HIF_RegSetB(HDMI_PHY_PLL_CTL3, 0x63); // HDMI PLL M value
        MMPH_HIF_RegSetB(HDMI_PHY_PLL_CTL4, 0x50); // HDMI PLL N value
        MMPH_HIF_RegSetB(HDMI_PHY_PLL_CTL1, HDMI_PHY_PHASE1);//phase1
       
        // HDMI PHY Setting
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL0, 0x15); // power dn TX (sync.)
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL1, 0x10);
        MMPH_HIF_RegSetB(HDMI_PHY_PLL_CTL0, MMPH_HIF_RegGetB(HDMI_PHY_PLL_CTL0) &~ 0x80);
        MMPF_OS_Sleep(1);
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL0, 0x31); // power on TX
        MMPF_OS_Sleep(1);                        
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL4, 0x00); // Enable data termination
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL5, 0x01); // Enable CLK termination
        break;
    case MMP_HDMI_OUTPUT_1280X720P_100FPS:
    case MMP_HDMI_OUTPUT_1280X720P_120FPS:
    case MMP_HDMI_OUTPUT_1920X1080P_50FPS:
    case MMP_HDMI_OUTPUT_1920X1080P:
        MMPH_HIF_RegSetB(HDMI_PHY_PLL_CTL3, 0x63); // HDMI PLL M value
        MMPH_HIF_RegSetB(HDMI_PHY_PLL_CTL4, 0x50); // HDMI PLL N value
        MMPH_HIF_RegSetB(HDMI_PHY_PLL_CTL1, HDMI_PHY_PHASE0);//phase0
        
        // HDMI PHY Setting
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL0, 0x17); 
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL1, 0x10); // Latch phase
        MMPH_HIF_RegSetB(HDMI_PHY_PLL_CTL0, MMPH_HIF_RegGetB(HDMI_PHY_PLL_CTL0) &~ 0x80);
        MMPF_OS_Sleep(1);
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL0, 0x31); // power on TX
        MMPF_OS_Sleep(1);            
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL4, 0x80); // Enable data termination
        MMPH_HIF_RegSetB(HDMI_PHY_TMDS_CTL5, 0x09); // Enable CLK termination
        MMPH_HIF_RegSetL(HDMI_PHY_BIST_CTL,
            (MMPH_HIF_RegGetL(HDMI_PHY_BIST_CTL) & ~(0xFF000000)) | 0x31000000);
        break;
    default:
        return MMP_DISPLAY_ERR_PARAMETER;
    } 

    return MMP_ERR_NONE;
}

const MMP_HDMI_OUTPUT_MODE HDMIVideoTable[128] =
{
    0,
    MMP_HDMI_OUTPUT_640X480P,
    MMP_HDMI_OUTPUT_720X480P,
    MMP_HDMI_OUTPUT_720X480P,
    MMP_HDMI_OUTPUT_1280X720P,
    MMP_HDMI_OUTPUT_1920X1080I,
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_UNSUPPORT,
    //10
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_UNSUPPORT,//MMP_HDMI_OUTPUT_1920X1080P, // 16
    MMP_HDMI_OUTPUT_720X576P,
    MMP_HDMI_OUTPUT_720X576P,
    MMP_HDMI_OUTPUT_1280X720P_50FPS, // 19
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_UNSUPPORT,
    MMP_HDMI_OUTPUT_1920X1080P_30FPS,
    0
};

//------------------------------------------------------------------------------
//  Function    : MMPD_HDMI_ParseEDID
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_HDMI_ParseEDID(MMP_UBYTE *pEDID, MMP_ULONG *pSpportFormat, MMP_ULONG *pEDIDType)
{
    MMP_USHORT i = 0, d = 0, Len = 0, j = 0;
    MMP_UBYTE revnum;
    
    *pEDIDType = pEDID[0];
    
    switch(pEDID[0]) {
    case 0x00:
        break;
    case 0x02: // Extend timing block
        revnum = pEDID[1];
        d = pEDID[2];
        i = 4;
        while(1) {
            if(i >= d)
                break;
            if((pEDID[i] & 0xE0) == 0x40) {
                Len = pEDID[i] & 0x1F;
                for(j = 1; j <= Len; j++) {
                    //VAR_L(0, pEDID[i+j]);
                    *pSpportFormat |= HDMIVideoTable[pEDID[i+j] & 0x7F];
                }
                break;
            } else {
                i += (pEDID[i] & 0x1F) + 1;
            }
        }
        break;
    default:
        break;
    }
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_HDMI_InitDigital
//  Description :
// AVI info frame
/*
AVI info frame field
===============================
         bit7                    bit0
BYTE 1   F17=0|Y1|Y0|A0|B1|B0|S1|S0              
===============================
Y1  Y0          RGB or YCbCr
0   0           RGB
0   1           YCbCr 4:2:2
1   0           YCbCr 4:4:4
1   1           Future

A0              Active Format Info Present
0               no data
===============================
         bit7                 bit0
BYTE 2   C1|C0|M1|M0|R3|R2|R1|R0       
===============================
C1  C0          Colorimetry
0   0           no data
0   1           SMPTE 170M  ITU601
1   0           ITU709
1   1           Extended Colorimetry

M1  M0          Picture Aspect Ratio
0   0           no data
0   1           4:3
1   0           16:9
1   1           Future

R3  R2  R1  R0  Active Format Aspect Ratio
1   0   0   0   same as picture aspect ratio
1   0   0   1   4:3(center)
1   0   1   0   16:9(center)
1   0   1   1   14:9(center)
===============================
         bit7                      bit0
BYTE 3   ITC|EC2|EC1|EC0|Q1|Q0|SC1|SC0
===============================
ITC             IT Content
0               no data
1               IT content

EC2 EC1 EC0     Extended Colorimetry
0   0   0       xvYCC601
0   0   1       xvYCC709

Q1  Q0          RGB Quantization Range
0   0           Default(depends on video format)
0   1           Limited Range
1   0           Full Range
1   1           Reserved        
*/
//------------------------------------------------------------------------------
MMP_ERR MMPD_HDMI_InitDigital(MMP_HDMI_ATTR *hdmiattribute)
{
    MMPH_HIF_RegSetB(HDMI_SKEW_NUM_VBLANK, 0x0B);
    
    MMPH_HIF_RegSetB(HDMI_SKEW_NUM_HBLANK, 0x0B);
    MMPH_HIF_RegSetB(HDMI_SPACING_NUM, 0x00);
    
    switch(hdmiattribute->ubOutputMode){
    
    case MMP_HDMI_OUTPUT_640X480P:
        MMPH_HIF_RegSetB(HDMI_PACKET_NUM_VBLANK, 0x00);
        MMPH_HIF_RegSetB(HDMI_PACKET_NUM_HBLANK, 0x00);
        //MMPH_HIF_RegSetB(0x80011404, 0x00); // DVI Mode
        MMPH_HIF_RegSetB(HDMI_CTL_2, 0x80); // HDMI Mode
        //MMPH_HIF_RegSetB(0x80011417, 0x00);
        MMPH_HIF_RegSetB(HDMI_CTL_5, 0xf0); // Audio format
        MMPH_HIF_RegSetB(HDMI_SYNC_POLARITY, 0x03);
        
        MMPH_HIF_RegSetB(HDMI_V_RESOLUTION_MSB, 0x01); // 480 MSB
        MMPH_HIF_RegSetB(HDMI_V_RESOLUTION_LSB, 0xE0); // 480 LSB
        MMPH_HIF_RegSetB(HDMI_H_RESOLUTION_MSB, 0x02); // 640 MSB
        MMPH_HIF_RegSetB(HDMI_H_RESOLUTION_LSB, 0x80); // 640 LSB
        
        MMPH_HIF_RegSetB(HDMI_V_BACK_PORCH_MSB, 0x00); // 33 MSB
        MMPH_HIF_RegSetB(HDMI_V_BACK_PORCH_LSB, 0x21); // 33 LSB
        MMPH_HIF_RegSetB(HDMI_H_BACK_PORCH_MSB, 0x00); // 48 MSB
        MMPH_HIF_RegSetB(HDMI_H_BACK_PORCH_LSB, 0x30); // 48 LSB
        
        // CTS
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER, 0x00);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 1, 0x6D);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 2, 0x60);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 3, 0x00);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 4, 0x18);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 5, 0x80);
        
        {
            MMP_ULONG i;
            for(i = 0x30; i < 0xB4; i++){
                MMPH_HIF_RegSetB(HDMI_CTL_1 + i, i);
            }
        }
        
        // AVI info frame
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 0, 0x56);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 1, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 2, 0x18);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 3, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 4, 0x01);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 5, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 6, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 7, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 8, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 9, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 10, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 11, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 12, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 13, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_CTL_28, 0x80); // Video Only
        MMPH_HIF_RegSetB(HDMI_CTL_29, 0x60); // Video Only
        MMPH_HIF_RegSetB(HDMI_CTL_30, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_CTL_6, 0x82); // Video Only
        break;
    case MMP_HDMI_OUTPUT_720X480P:
        MMPH_HIF_RegSetB(HDMI_PACKET_NUM_VBLANK, 0x01);
        MMPH_HIF_RegSetB(HDMI_PACKET_NUM_HBLANK, 0x11);
        //MMPH_HIF_RegSetB(0x80011404, 0x00); // DVI Mode
        MMPH_HIF_RegSetB(HDMI_CTL_2, 0x80); // HDMI Mode
        //MMPH_HIF_RegSetB(0x80011417, 0x00);
        MMPH_HIF_RegSetB(HDMI_CTL_5, 0xf0); // Audio format
        MMPH_HIF_RegSetB(HDMI_SYNC_POLARITY, 0x03);
        
        MMPH_HIF_RegSetB(HDMI_V_RESOLUTION_MSB, 0x01); // 480 MSB
        MMPH_HIF_RegSetB(HDMI_V_RESOLUTION_LSB, 0xE0); // 480 LSB
        MMPH_HIF_RegSetB(HDMI_H_RESOLUTION_MSB, 0x02); // 720 MSB
        MMPH_HIF_RegSetB(HDMI_H_RESOLUTION_LSB, 0xD0); // 720 LSB
        
        MMPH_HIF_RegSetB(HDMI_V_BACK_PORCH_MSB, 0x00); // 30 MSB
        MMPH_HIF_RegSetB(HDMI_V_BACK_PORCH_LSB, 0x1E); // 30 LSB
        MMPH_HIF_RegSetB(HDMI_H_BACK_PORCH_MSB, 0x00); // 60 MSB
        MMPH_HIF_RegSetB(HDMI_H_BACK_PORCH_LSB, 0x3C); // 60 LSB
        
        // CTS
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 0, 0x00);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 1, 0x75);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 2, 0x30);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 3, 0x00);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 4, 0x18);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 5, 0x80);
        
        {
            MMP_ULONG i;
            for(i = 0x30; i < 0xB4; i++){
                MMPH_HIF_RegSetB(HDMI_CTL_1 + i, i);
            }
        }
        
        // AVI info frame
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 0, 0x44);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 1, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 2, 0x28);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 3, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 4, 0x03);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 5, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 6, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 7, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 8, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 9, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 10, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 11, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 12, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 13, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_CTL_28, 0x80); // Video Only
        MMPH_HIF_RegSetB(HDMI_CTL_29, 0x60); // Video Only
        MMPH_HIF_RegSetB(HDMI_CTL_30, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_CTL_6, 0x82); // Video Only
        break;
    case MMP_HDMI_OUTPUT_720X576P:
        MMPH_HIF_RegSetB(HDMI_PACKET_NUM_VBLANK, 0x00);
        MMPH_HIF_RegSetB(HDMI_PACKET_NUM_HBLANK, 0x11);
        //MMPH_HIF_RegSetB(0x80011404, 0x00); // DVI Mode
        MMPH_HIF_RegSetB(HDMI_CTL_2, 0x80); // HDMI Mode
        //MMPH_HIF_RegSetB(0x80011417, 0x00);
        MMPH_HIF_RegSetB(HDMI_CTL_5, 0xf0); // Audio format
        MMPH_HIF_RegSetB(HDMI_SYNC_POLARITY, 0x03);
        
        MMPH_HIF_RegSetB(HDMI_V_RESOLUTION_MSB, 0x02); // 576 MSB
        MMPH_HIF_RegSetB(HDMI_V_RESOLUTION_LSB, 0x40); // 576 LSB
        MMPH_HIF_RegSetB(HDMI_H_RESOLUTION_MSB, 0x02); // 720 MSB
        MMPH_HIF_RegSetB(HDMI_H_RESOLUTION_LSB, 0xD0); // 720 LSB
        
        MMPH_HIF_RegSetB(HDMI_V_BACK_PORCH_MSB, 0x00); // 39 MSB
        MMPH_HIF_RegSetB(HDMI_V_BACK_PORCH_LSB, 0x27); // 39 LSB
        MMPH_HIF_RegSetB(HDMI_H_BACK_PORCH_MSB, 0x00); // 68 MSB
        MMPH_HIF_RegSetB(HDMI_H_BACK_PORCH_LSB, 0x44); // 68 LSB
        
        // CTS
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 0, 0x00);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 1, 0x75);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 2, 0x30);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 3, 0x00);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 4, 0x18);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 5, 0x80);
        
        {
            MMP_ULONG i;
            for(i = 0x30; i < 0xB4; i++){
                MMPH_HIF_RegSetB(HDMI_CTL_1 + i, i);
            }
        }
        
        // AVI info frame
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 0, 0x55);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 1, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 2, 0x08);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 3, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 4, 0x12);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 5, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 6, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 7, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 8, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 9, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 10, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 11, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 12, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 13, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_CTL_28, 0x80); // Video Only
        MMPH_HIF_RegSetB(HDMI_CTL_29, 0x60); // Video Only
        MMPH_HIF_RegSetB(HDMI_CTL_30, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_CTL_6, 0x82); // Video Only
        break;
    case MMP_HDMI_OUTPUT_1280X720P:
        MMPH_HIF_RegSetB(HDMI_PACKET_NUM_VBLANK, 0x11);
        MMPH_HIF_RegSetB(HDMI_PACKET_NUM_HBLANK, 0x10);
        //MMPH_HIF_RegSetB(0x80011404, 0x00); // DVI Mode
        MMPH_HIF_RegSetB(HDMI_CTL_2, 0x80); // HDMI Mode
        //MMPH_HIF_RegSetB(0x80011417, 0x00);
        MMPH_HIF_RegSetB(HDMI_CTL_5, 0xf0); // Audio format
        MMPH_HIF_RegSetB(HDMI_SYNC_POLARITY, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_V_RESOLUTION_MSB, 0x02); // 720 MSB
        MMPH_HIF_RegSetB(HDMI_V_RESOLUTION_LSB, 0xD0); // 720 LSB
        MMPH_HIF_RegSetB(HDMI_H_RESOLUTION_MSB, 0x05); // 1280 MSB
        MMPH_HIF_RegSetB(HDMI_H_RESOLUTION_LSB, 0x00); // 1280 LSB
        
        MMPH_HIF_RegSetB(HDMI_V_BACK_PORCH_MSB, 0x00); // 20 MSB
        MMPH_HIF_RegSetB(HDMI_V_BACK_PORCH_LSB, 20); // 20 LSB
        MMPH_HIF_RegSetB(HDMI_H_BACK_PORCH_MSB, 0x00); // 220 MSB
        MMPH_HIF_RegSetB(HDMI_H_BACK_PORCH_LSB, 0xDC); // 220 LSB
        
        // CTS
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 0, 0x01);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 1, 0x42);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 2, 0x44);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 3, 0x00);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 4, 0x18);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 5, 0x80);
        
        {
            MMP_ULONG i;
            for(i = 0x30; i < 0xB4; i++){
                MMPH_HIF_RegSetB(HDMI_CTL_1 + i, i);
            }
        }
            
        // AVI info frame
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 0, 0x43);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 1, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 2, 0x28);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 3, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 4, 0x04);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 5, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 6, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 7, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 8, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 9, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 10, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 11, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 12, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 13, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_CTL_28, 0x80); // Video Only
        MMPH_HIF_RegSetB(HDMI_CTL_29, 0x60); // Video Only
        MMPH_HIF_RegSetB(HDMI_CTL_30, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_CTL_6, 0x82); // Video Only
        break;
    case MMP_HDMI_OUTPUT_1280X720P_50FPS:
        MMPH_HIF_RegSetB(HDMI_PACKET_NUM_VBLANK, 0x13);
        MMPH_HIF_RegSetB(HDMI_PACKET_NUM_HBLANK, 0x10);
        //MMPH_HIF_RegSetB(0x80011404, 0x00); // DVI Mode
        MMPH_HIF_RegSetB(HDMI_CTL_2, 0x80); // HDMI Mode
        //MMPH_HIF_RegSetB(0x80011417, 0x00);
        MMPH_HIF_RegSetB(HDMI_CTL_5, 0xf0); // Audio format
        MMPH_HIF_RegSetB(HDMI_SYNC_POLARITY, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_V_RESOLUTION_MSB, 0x02); // 720 MSB
        MMPH_HIF_RegSetB(HDMI_V_RESOLUTION_LSB, 0xD0); // 720 LSB
        MMPH_HIF_RegSetB(HDMI_H_RESOLUTION_MSB, 0x05); // 1280 MSB
        MMPH_HIF_RegSetB(HDMI_H_RESOLUTION_LSB, 0x00); // 1280 LSB
        
        MMPH_HIF_RegSetB(HDMI_V_BACK_PORCH_MSB, 0x00); // 20 MSB
        MMPH_HIF_RegSetB(HDMI_V_BACK_PORCH_LSB, 20); // 20 LSB
        MMPH_HIF_RegSetB(HDMI_H_BACK_PORCH_MSB, 0x00); // 220 MSB
        MMPH_HIF_RegSetB(HDMI_H_BACK_PORCH_LSB, 0xDC); // 220 LSB

        // CTS
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 0, 0x01);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 1, 0x42);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 2, 0x44);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 3, 0x00);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 4, 0x18);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 5, 0x80);
        
        {
            MMP_ULONG i;
            for(i = 0x30; i < 0xB4; i++){
                MMPH_HIF_RegSetB(HDMI_CTL_1 + i, i);
            }
        }

        // AVI info frame
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 0, 0x54);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 1, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 2, 0x08);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 3, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 4, 0x13);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 5, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 6, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 7, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 8, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 9, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 10, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 11, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 12, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 13, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_CTL_28, 0x80); // Video Only
        MMPH_HIF_RegSetB(HDMI_CTL_29, 0x60); // Video Only
        MMPH_HIF_RegSetB(HDMI_CTL_30, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_CTL_6, 0x82); // Video Only
        break;
    case MMP_HDMI_OUTPUT_1280X720P_100FPS:
        MMPH_HIF_RegSetB(HDMI_PACKET_NUM_VBLANK, 0x13);
        MMPH_HIF_RegSetB(HDMI_PACKET_NUM_HBLANK, 0x10);
        //MMPH_HIF_RegSetB(0x80011404, 0x00); // DVI Mode
        MMPH_HIF_RegSetB(HDMI_CTL_2, 0x80); // HDMI Mode
        //MMPH_HIF_RegSetB(0x80011417, 0x00);
        MMPH_HIF_RegSetB(HDMI_CTL_5, 0xf0); // Audio format
        MMPH_HIF_RegSetB(HDMI_SYNC_POLARITY, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_V_RESOLUTION_MSB, 0x02); // 720 MSB
        MMPH_HIF_RegSetB(HDMI_V_RESOLUTION_LSB, 0xD0); // 720 LSB
        MMPH_HIF_RegSetB(HDMI_H_RESOLUTION_MSB, 0x05); // 1280 MSB
        MMPH_HIF_RegSetB(HDMI_H_RESOLUTION_LSB, 0x00); // 1280 LSB
        
        MMPH_HIF_RegSetB(HDMI_V_BACK_PORCH_MSB, 0x00); // 20 MSB
        MMPH_HIF_RegSetB(HDMI_V_BACK_PORCH_LSB, 20); // 20 LSB
        MMPH_HIF_RegSetB(HDMI_H_BACK_PORCH_MSB, 0x00); // 220 MSB
        MMPH_HIF_RegSetB(HDMI_H_BACK_PORCH_LSB, 0xDC); // 220 LSB

        // CTS
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 0, 0x01);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 1, 0x42);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 2, 0x44);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 3, 0x00);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 4, 0x18);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 5, 0x80);
        
        {
            MMP_ULONG i;
            for(i = 0x30; i < 0xB4; i++){
                MMPH_HIF_RegSetB(HDMI_CTL_1 + i, i);
            }
        }

        // AVI info frame
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 0, 0x3E);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 1, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 2, 0x08);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 3, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 4, 0x29); //VIC=41
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 5, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 6, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 7, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 8, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 9, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 10, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 11, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 12, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 13, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_CTL_28, 0x80); // Video Only
        MMPH_HIF_RegSetB(HDMI_CTL_29, 0x60); // Video Only
        MMPH_HIF_RegSetB(HDMI_CTL_30, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_CTL_6, 0x82); // Video Only
        break;
    case MMP_HDMI_OUTPUT_1280X720P_120FPS:
        MMPH_HIF_RegSetB(HDMI_PACKET_NUM_VBLANK, 0x13);
        MMPH_HIF_RegSetB(HDMI_PACKET_NUM_HBLANK, 0x10);
        //MMPH_HIF_RegSetB(0x80011404, 0x00); // DVI Mode
        MMPH_HIF_RegSetB(HDMI_CTL_2, 0x80); // HDMI Mode
        //MMPH_HIF_RegSetB(0x80011417, 0x00);
        MMPH_HIF_RegSetB(HDMI_CTL_5, 0xf0); // Audio format
        MMPH_HIF_RegSetB(HDMI_SYNC_POLARITY, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_V_RESOLUTION_MSB, 0x02); // 720 MSB
        MMPH_HIF_RegSetB(HDMI_V_RESOLUTION_LSB, 0xD0); // 720 LSB
        MMPH_HIF_RegSetB(HDMI_H_RESOLUTION_MSB, 0x05); // 1280 MSB
        MMPH_HIF_RegSetB(HDMI_H_RESOLUTION_LSB, 0x00); // 1280 LSB
        
        MMPH_HIF_RegSetB(HDMI_V_BACK_PORCH_MSB, 0x00); // 20 MSB
        MMPH_HIF_RegSetB(HDMI_V_BACK_PORCH_LSB, 20); // 20 LSB
        MMPH_HIF_RegSetB(HDMI_H_BACK_PORCH_MSB, 0x00); // 220 MSB
        MMPH_HIF_RegSetB(HDMI_H_BACK_PORCH_LSB, 0xDC); // 220 LSB

        // CTS
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 0, 0x01);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 1, 0x42);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 2, 0x44);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 3, 0x00);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 4, 0x18);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 5, 0x80);
        
        {
            MMP_ULONG i;
            for(i = 0x30; i < 0xB4; i++){
                MMPH_HIF_RegSetB(HDMI_CTL_1 + i, i);
            }
        }

        // AVI info frame
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 0, 0x38);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 1, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 2, 0x08);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 3, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 4, 0x2F); //VIC=47
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 5, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 6, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 7, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 8, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 9, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 10, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 11, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 12, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 13, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_CTL_28, 0x80); // Video Only
        MMPH_HIF_RegSetB(HDMI_CTL_29, 0x60); // Video Only
        MMPH_HIF_RegSetB(HDMI_CTL_30, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_CTL_6, 0x82); // Video Only
        break;
    case MMP_HDMI_OUTPUT_1920X1080P:
        MMPH_HIF_RegSetB(HDMI_PACKET_NUM_VBLANK, 0x01);
        MMPH_HIF_RegSetB(HDMI_PACKET_NUM_HBLANK, 0x10);
        //MMPH_HIF_RegSetB(0x80011404, 0x00); // DVI Mode
        MMPH_HIF_RegSetB(HDMI_CTL_2, 0x80); // HDMI Mode
        MMPH_HIF_RegSetB(HDMI_CTL_5, 0xf0); // Audio format
        MMPH_HIF_RegSetB(HDMI_SYNC_POLARITY, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_V_RESOLUTION_MSB, 0x04); // 1080 MSB
        MMPH_HIF_RegSetB(HDMI_V_RESOLUTION_LSB, 0x38); // 1080 LSB
        MMPH_HIF_RegSetB(HDMI_H_RESOLUTION_MSB, 0x07); // 1920 MSB
        MMPH_HIF_RegSetB(HDMI_H_RESOLUTION_LSB, 0x80); // 1920 LSB
        
        MMPH_HIF_RegSetB(HDMI_V_BACK_PORCH_MSB, 0x00); // 36 MSB
        MMPH_HIF_RegSetB(HDMI_V_BACK_PORCH_LSB, 0x24); // 36 LSB
        MMPH_HIF_RegSetB(HDMI_H_BACK_PORCH_MSB, 0x00); // 148 MSB
        MMPH_HIF_RegSetB(HDMI_H_BACK_PORCH_LSB, 0x94); // 148 LSB
        
        // CTS
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 0, 0x02);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 1, 0x84);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 2, 0x88);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 3, 0x00);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 4, 0x18);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 5, 0x80);
        
        {
            MMP_ULONG i;
            for(i = 0x30; i < 0xB4; i++){
                MMPH_HIF_RegSetB(HDMI_CTL_1 + i, i);
            }
        }
        
        // AVI info frame
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 0, 0x37);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 1, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 2, 0x28);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 3, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 4, 0x10);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 5, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 6, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 7, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 8, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 9, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 10, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 11, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 12, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 13, 0x00);
                
        MMPH_HIF_RegSetB(HDMI_CTL_28, 0x80); // Video Only
        MMPH_HIF_RegSetB(HDMI_CTL_29, 0x60); // Video Only
        MMPH_HIF_RegSetB(HDMI_CTL_30, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_CTL_6, 0x82); // Video Only
        break;
    case MMP_HDMI_OUTPUT_1920X1080P_50FPS:
        MMPH_HIF_RegSetB(HDMI_PACKET_NUM_VBLANK, 0x01);
        MMPH_HIF_RegSetB(HDMI_PACKET_NUM_HBLANK, 0x10);
        //MMPH_HIF_RegSetB(0x80011404, 0x00); // DVI Mode
        MMPH_HIF_RegSetB(HDMI_CTL_2, 0x80); // HDMI Mode
        MMPH_HIF_RegSetB(HDMI_CTL_5, 0xf0); // Audio format
        MMPH_HIF_RegSetB(HDMI_SYNC_POLARITY, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_V_RESOLUTION_MSB, 0x04); // 1080 MSB
        MMPH_HIF_RegSetB(HDMI_V_RESOLUTION_LSB, 0x38); // 1080 LSB
        MMPH_HIF_RegSetB(HDMI_H_RESOLUTION_MSB, 0x07); // 1920 MSB
        MMPH_HIF_RegSetB(HDMI_H_RESOLUTION_LSB, 0x80); // 1920 LSB
        
        MMPH_HIF_RegSetB(HDMI_V_BACK_PORCH_MSB, 0x00); // 36 MSB
        MMPH_HIF_RegSetB(HDMI_V_BACK_PORCH_LSB, 0x24); // 36 LSB
        MMPH_HIF_RegSetB(HDMI_H_BACK_PORCH_MSB, 0x00); // 148 MSB
        MMPH_HIF_RegSetB(HDMI_H_BACK_PORCH_LSB, 0x94); // 148 LSB
        
        // CTS
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 0, 0x02);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 1, 0x84);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 2, 0x88);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 3, 0x00);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 4, 0x18);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 5, 0x80);
        
        {
            MMP_ULONG i;
            for(i = 0x30; i < 0xB4; i++){
                MMPH_HIF_RegSetB(HDMI_CTL_1 + i, i);
            }
        }
        
        // AVI info frame
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 0, 0x28);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 1, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 2, 0x28);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 3, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 4, 0x1F); //VIC=31
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 5, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 6, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 7, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 8, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 9, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 10, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 11, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 12, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 13, 0x00);
                
        MMPH_HIF_RegSetB(HDMI_CTL_28, 0x80); // Video Only
        MMPH_HIF_RegSetB(HDMI_CTL_29, 0x60); // Video Only
        MMPH_HIF_RegSetB(HDMI_CTL_30, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_CTL_6, 0x82); // Video Only
        break;
    case MMP_HDMI_OUTPUT_1920X1080P_30FPS:
        MMPH_HIF_RegSetB(HDMI_PACKET_NUM_VBLANK, 0x13);
        MMPH_HIF_RegSetB(HDMI_PACKET_NUM_HBLANK, 0x10);
        //MMPH_HIF_RegSetB(0x80011404, 0x00); // DVI Mode
        MMPH_HIF_RegSetB(HDMI_CTL_2, 0x80); // HDMI Mode
        //MMPH_HIF_RegSetB(0x80011417, 0x00);
        MMPH_HIF_RegSetB(HDMI_CTL_5, 0xf0); // Audio format
        MMPH_HIF_RegSetB(HDMI_SYNC_POLARITY, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_V_RESOLUTION_MSB, 0x04); // 1080 MSB
        MMPH_HIF_RegSetB(HDMI_V_RESOLUTION_LSB, 0x38); // 1080 LSB
        MMPH_HIF_RegSetB(HDMI_H_RESOLUTION_MSB, 0x07); // 1920 MSB
        MMPH_HIF_RegSetB(HDMI_H_RESOLUTION_LSB, 0x80); // 1920 LSB
        
        MMPH_HIF_RegSetB(HDMI_V_BACK_PORCH_MSB, 0x00); // 36 MSB
        MMPH_HIF_RegSetB(HDMI_V_BACK_PORCH_LSB, 0x24); // 36 LSB
        MMPH_HIF_RegSetB(HDMI_H_BACK_PORCH_MSB, 0x00); // 148 MSB
        MMPH_HIF_RegSetB(HDMI_H_BACK_PORCH_LSB, 0x94); // 148 LSB
        
        // CTS
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 0, 0x01);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 1, 0x42);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 2, 0x44);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 3, 0x00);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 4, 0x18);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 5, 0x80);
        
        {
            MMP_ULONG i;
            for(i = 0x30; i < 0xB4; i++){
                MMPH_HIF_RegSetB(HDMI_CTL_1 + i, i);
            }
        }
        
        // AVI info frame
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 0, 0x25);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 1, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 2, 0x28);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 3, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 4, 0x22); // format 34
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 5, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 6, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 7, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 8, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 9, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 10, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 11, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 12, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 13, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_CTL_28, 0x80); // Video Only
        MMPH_HIF_RegSetB(HDMI_CTL_29, 0x60); // Video Only
        MMPH_HIF_RegSetB(HDMI_CTL_30, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_CTL_6, 0x82); // Video Only
        break;
    case MMP_HDMI_OUTPUT_1920X1080I:
        MMPH_HIF_RegSetB(HDMI_PACKET_NUM_VBLANK, 0x01);
        MMPH_HIF_RegSetB(HDMI_PACKET_NUM_HBLANK, 0x10);
        MMPH_HIF_RegSetB(HDMI_CTL_2, 0x80); // HDMI Mode
        //MMPH_HIF_RegSetB(0x80011417, 0x00);
        MMPH_HIF_RegSetB(HDMI_CTL_5, 0xf0); // Audio format
        MMPH_HIF_RegSetB(HDMI_SYNC_POLARITY, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_V_RESOLUTION_MSB, 0x02); // 1080 MSB
        MMPH_HIF_RegSetB(HDMI_V_RESOLUTION_LSB, 0x1c); // 1080 LSB
        MMPH_HIF_RegSetB(HDMI_H_RESOLUTION_MSB, 0x07); // 1920 MSB
        MMPH_HIF_RegSetB(HDMI_H_RESOLUTION_LSB, 0x80); // 1920 LSB
        
        MMPH_HIF_RegSetB(HDMI_V_BACK_PORCH_MSB, 0x00); // 15 MSB
        MMPH_HIF_RegSetB(HDMI_V_BACK_PORCH_LSB, 0x0F); // 15 LSB
        MMPH_HIF_RegSetB(HDMI_H_BACK_PORCH_MSB, 0x00); // 148 MSB
        MMPH_HIF_RegSetB(HDMI_H_BACK_PORCH_LSB, 0x94); // 148 LSB
                
        // CTS 44.1K
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 0, 0x01);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 1, 0x42);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 2, 0x44);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 3, 0x00);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 4, 0x18);
        MMPH_HIF_RegSetB(HDMI_ACR_PARAMETER + 5, 0x80);
        
        {
            MMP_ULONG i;
            for(i = 0x30; i < 0xB4; i++){
                MMPH_HIF_RegSetB(HDMI_CTL_1 + i, i);
            }
        }
        
        // AVI info frame
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 0, 0x42);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 1, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 2, 0x28);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 3, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 4, 0x05);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 5, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 6, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 7, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 8, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 9, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 10, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 11, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 12, 0x00);
        MMPH_HIF_RegSetB(HDMI_AVI_INFOFRAME + 13, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_CTL_28, 0x80); // Video Only
        MMPH_HIF_RegSetB(HDMI_CTL_29, 0x60); // Video Only
        MMPH_HIF_RegSetB(HDMI_CTL_30, 0x00);
        
        MMPH_HIF_RegSetB(HDMI_CTL_6, 0x82); // Video Only
        break;
    default:
        return MMP_DISPLAY_ERR_PARAMETER;
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_HDMI_Initialize
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_HDMI_Initialize(MMP_HDMI_ATTR *hdmiattribute, MMP_DISPLAY_RGB_IF rgbIf)
{
    if(rgbIf == MMP_DISPLAY_RGB_IF1)
    {
        m_bHdmiRgbIF1 = MMP_TRUE;
        MMPH_HIF_RegSetL(DSPY_LCD_TX_6, 0x0);
        MMPH_HIF_RegSetB(DSPY_RGB_FMT,     hdmiattribute->ubColorType);
        MMPH_HIF_RegSetB(DSPY_RGB_PORCH_HIGH_BIT_EXT, 0);
       
        MMPH_HIF_RegSetB(DSPY_RGB_SHARE_P_LCD_BUS,  HDMI_SRC_SEL_RGB);
        switch(hdmiattribute->ubOutputMode){
        case MMP_HDMI_OUTPUT_USERDEF:
            MMPH_HIF_RegSetB(DSPY_RGB_CTL,     VSYNC_POLAR_LOW |
                                        HSYNC_POLAR_LOW
                                        );

            MMPH_HIF_RegSetB(DSPY_RGB_DOT_CLK_RATIO, 5);
            
            MMPH_HIF_RegSetB(DSPY_RGB_H_BPORCH, 10);
            MMPH_HIF_RegSetB(DSPY_RGB_H_BLANK, 14);
            MMPH_HIF_RegSetB(DSPY_RGB_HSYNC_W, 10);
            
            MMPH_HIF_RegSetB(DSPY_RGB_V_BPORCH, 2);
            MMPH_HIF_RegSetB(DSPY_RGB_V_BLANK, 2);
            MMPH_HIF_RegSetB(DSPY_RGB_VSYNC_W, 5);

            MMPH_HIF_RegSetB(DSPY_RGB_PRT_2_H_DOT, 2);
            break;
        case MMP_HDMI_OUTPUT_640X480P:                
            MMPH_HIF_RegSetB(DSPY_RGB_CTL,     HSYNC_POLAR_LOW |
                                        HSYNC_POLAR_LOW
                                        );

            MMPH_HIF_RegSetB(DSPY_RGB_DOT_CLK_RATIO, 0);
            
            MMPH_HIF_RegSetB(DSPY_RGB_H_BPORCH, 144);
            MMPH_HIF_RegSetB(DSPY_RGB_H_BLANK, 160);
            MMPH_HIF_RegSetB(DSPY_RGB_HSYNC_W, 95);
            
            MMPH_HIF_RegSetB(DSPY_RGB_V_BPORCH, 35);
            MMPH_HIF_RegSetB(DSPY_RGB_V_BLANK, 45);
            MMPH_HIF_RegSetB(DSPY_RGB_VSYNC_W, 1);

            MMPH_HIF_RegSetB(DSPY_RGB_PRT_2_H_DOT, 2);
            break;
        case MMP_HDMI_OUTPUT_720X480P:
            MMPH_HIF_RegSetB(DSPY_RGB_CTL,     HSYNC_POLAR_LOW |
                                        HSYNC_POLAR_LOW
                                        );

            MMPH_HIF_RegSetB(DSPY_RGB_DOT_CLK_RATIO, 0);
            
            MMPH_HIF_RegSetB(DSPY_RGB_H_BPORCH, 122);
            MMPH_HIF_RegSetB(DSPY_RGB_H_BLANK, 138);
            MMPH_HIF_RegSetB(DSPY_RGB_HSYNC_W, 61);
            
            MMPH_HIF_RegSetB(DSPY_RGB_V_BPORCH, 36);
            MMPH_HIF_RegSetB(DSPY_RGB_V_BLANK, 45);
            MMPH_HIF_RegSetB(DSPY_RGB_VSYNC_W, 5);

            MMPH_HIF_RegSetB(DSPY_RGB_PRT_2_H_DOT, 2);
            break;
        case MMP_HDMI_OUTPUT_720X576P:                
            MMPH_HIF_RegSetB(DSPY_RGB_CTL,     HSYNC_POLAR_LOW |
                                        HSYNC_POLAR_LOW
                                        );

            MMPH_HIF_RegSetB(DSPY_RGB_DOT_CLK_RATIO, 0);
            
            MMPH_HIF_RegSetB(DSPY_RGB_H_BPORCH, 132);
            MMPH_HIF_RegSetB(DSPY_RGB_H_BLANK, 144);
            MMPH_HIF_RegSetB(DSPY_RGB_HSYNC_W, 63);
            
            MMPH_HIF_RegSetB(DSPY_RGB_V_BPORCH, 44);
            MMPH_HIF_RegSetB(DSPY_RGB_V_BLANK, 49);
            MMPH_HIF_RegSetB(DSPY_RGB_VSYNC_W, 4);

            MMPH_HIF_RegSetB(DSPY_RGB_PRT_2_H_DOT, 2);
            break;
        case MMP_HDMI_OUTPUT_1280X720P:
            MMPH_HIF_RegSetB(DSPY_RGB_CTL,     HSYNC_POLAR_HIGH |
                                            VSYNC_POLAR_HIGH
                                            );

            MMPH_HIF_RegSetB(DSPY_RGB_DOT_CLK_RATIO, 0);
            
            // H Blanking = 370, Back porch = 260
            MMPH_HIF_RegSetB(DSPY_RGB_PORCH_HIGH_BIT_EXT, 0x05);
            
            MMPH_HIF_RegSetB(DSPY_RGB_H_BPORCH, 0x04);
            MMPH_HIF_RegSetB(DSPY_RGB_H_BLANK, 0x72);
            MMPH_HIF_RegSetB(DSPY_RGB_HSYNC_W, 39);
            
            #if 0
            MMPH_HIF_RegSetB(DSPY_RGB_V_BPORCH, 30);
            MMPH_HIF_RegSetB(DSPY_RGB_V_BLANK, 30);
            MMPH_HIF_RegSetB(DSPY_RGB_VSYNC_W, 4);
            #else
            MMPH_HIF_RegSetB(DSPY_RGB_V_BPORCH, 25);
            MMPH_HIF_RegSetB(DSPY_RGB_V_BLANK, 30);
            MMPH_HIF_RegSetB(DSPY_RGB_VSYNC_W, 4);
            #endif

            MMPH_HIF_RegSetB(DSPY_RGB_PRT_2_H_DOT, 2);
            break;
        case MMP_HDMI_OUTPUT_1280X720P_50FPS:
            MMPH_HIF_RegSetB(DSPY_RGB_CTL,     HSYNC_POLAR_HIGH |
                                            VSYNC_POLAR_HIGH
                                            );

            MMPH_HIF_RegSetB(DSPY_RGB_DOT_CLK_RATIO, 0);
            
            // H Blanking = 700, Back porch = 260
            MMPH_HIF_RegSetB(DSPY_RGB_PORCH_HIGH_BIT_EXT, 0x06);
            
            MMPH_HIF_RegSetB(DSPY_RGB_H_BPORCH, 0x04);
            MMPH_HIF_RegSetB(DSPY_RGB_H_BLANK, 0xBC);
            MMPH_HIF_RegSetB(DSPY_RGB_HSYNC_W, 39);

            MMPH_HIF_RegSetB(DSPY_RGB_V_BPORCH, 25);
            MMPH_HIF_RegSetB(DSPY_RGB_V_BLANK, 30);
            MMPH_HIF_RegSetB(DSPY_RGB_VSYNC_W, 4);

            MMPH_HIF_RegSetB(DSPY_RGB_PRT_2_H_DOT, 2);
            break;
        case MMP_HDMI_OUTPUT_1280X720P_100FPS:
            MMPH_HIF_RegSetB(DSPY_RGB_CTL,     HSYNC_POLAR_HIGH |
                                            VSYNC_POLAR_HIGH
                                            );

            MMPH_HIF_RegSetB(DSPY_RGB_DOT_CLK_RATIO, 0);
            
            // H Blanking = 700, Back porch = 260
            MMPH_HIF_RegSetB(DSPY_RGB_PORCH_HIGH_BIT_EXT, 0x06);
            
            MMPH_HIF_RegSetB(DSPY_RGB_H_BPORCH, 0x04);
            MMPH_HIF_RegSetB(DSPY_RGB_H_BLANK, 0xBC);
            MMPH_HIF_RegSetB(DSPY_RGB_HSYNC_W, 39);

            MMPH_HIF_RegSetB(DSPY_RGB_V_BPORCH, 25);
            MMPH_HIF_RegSetB(DSPY_RGB_V_BLANK, 30);
            MMPH_HIF_RegSetB(DSPY_RGB_VSYNC_W, 4);

            MMPH_HIF_RegSetB(DSPY_RGB_PRT_2_H_DOT, 2);
            break;
        case MMP_HDMI_OUTPUT_1280X720P_120FPS:
            MMPH_HIF_RegSetB(DSPY_RGB_CTL,     HSYNC_POLAR_HIGH |
                                            VSYNC_POLAR_HIGH
                                            );

            MMPH_HIF_RegSetB(DSPY_RGB_DOT_CLK_RATIO, 0);
                    
            // H Blanking = 370, Back porch = 260
            MMPH_HIF_RegSetB(DSPY_RGB_PORCH_HIGH_BIT_EXT, 0x05);
            //MMPH_HIF_RegSetB(DSPY_RGB_PORCH_HIGH_BIT_EXT, 0x01);
            
            MMPH_HIF_RegSetB(DSPY_RGB_H_BPORCH, 0x04);
            //MMPH_HIF_RegSetB(DSPY_RGB_H_BPORCH, 0xD0);
            MMPH_HIF_RegSetB(DSPY_RGB_H_BLANK, 0x72);
            //MMPH_HIF_RegSetB(DSPY_RGB_H_BLANK, 0x80);
            MMPH_HIF_RegSetB(DSPY_RGB_HSYNC_W, 39);
                            
            #if 0
            MMPH_HIF_RegSetB(DSPY_RGB_V_BPORCH, 30);
            MMPH_HIF_RegSetB(DSPY_RGB_V_BLANK, 30);
            MMPH_HIF_RegSetB(DSPY_RGB_VSYNC_W, 4);
            #else
            MMPH_HIF_RegSetB(DSPY_RGB_V_BPORCH, 25);
            MMPH_HIF_RegSetB(DSPY_RGB_V_BLANK, 30);
            MMPH_HIF_RegSetB(DSPY_RGB_VSYNC_W, 4);           
            #endif

            MMPH_HIF_RegSetB(DSPY_RGB_PRT_2_H_DOT, 2);
            break;
	   	case MMP_HDMI_OUTPUT_1920X1080P_50FPS:
	   		
            MMPH_HIF_RegSetB(DSPY_RGB_CTL,     HSYNC_POLAR_HIGH |
                                            VSYNC_POLAR_HIGH
                                            );

            MMPH_HIF_RegSetB(DSPY_RGB_DOT_CLK_RATIO, 0);            
            MMPH_HIF_RegSetB(DSPY_RGB_PORCH_HIGH_BIT_EXT,0x02);
            
            MMPH_HIF_RegSetB(DSPY_RGB_H_BPORCH, 0xC0);
            MMPH_HIF_RegSetB(DSPY_RGB_H_BLANK, 0xD0);
            MMPH_HIF_RegSetB(DSPY_RGB_HSYNC_W, 43);
            
            MMPH_HIF_RegSetB(DSPY_RGB_V_BPORCH, 41);
            MMPH_HIF_RegSetB(DSPY_RGB_V_BLANK, 45);
            MMPH_HIF_RegSetB(DSPY_RGB_VSYNC_W, 4);

            MMPH_HIF_RegSetB(DSPY_RGB_PRT_2_H_DOT, 2);
	   		break;
        case MMP_HDMI_OUTPUT_1920X1080P:
            MMPH_HIF_RegSetB(DSPY_RGB_CTL,     HSYNC_POLAR_HIGH |
                                            VSYNC_POLAR_HIGH
                                            );

            MMPH_HIF_RegSetB(DSPY_RGB_DOT_CLK_RATIO, 0);
            
            #if 0 // 50Hz
            MMPH_HIF_RegSetB(DSPY_RGB_PORCH_HIGH_BIT_EXT,0x02);
            
            MMPH_HIF_RegSetB(DSPY_RGB_H_BPORCH, 0xC0);
            MMPH_HIF_RegSetB(DSPY_RGB_H_BLANK, 0xD0);
            MMPH_HIF_RegSetB(DSPY_RGB_HSYNC_W, 43);
            
            MMPH_HIF_RegSetB(DSPY_RGB_V_BPORCH, 41);
            MMPH_HIF_RegSetB(DSPY_RGB_V_BLANK, 45);
            MMPH_HIF_RegSetB(DSPY_RGB_VSYNC_W, 4);
            #else // 60Hz
            MMPH_HIF_RegSetB(DSPY_RGB_PORCH_HIGH_BIT_EXT,0x01);
            
            MMPH_HIF_RegSetB(DSPY_RGB_H_BPORCH, 0xC0);
            MMPH_HIF_RegSetB(DSPY_RGB_H_BLANK, 0x18);
            MMPH_HIF_RegSetB(DSPY_RGB_HSYNC_W, 43);
            
            MMPH_HIF_RegSetB(DSPY_RGB_V_BPORCH, 41);
            MMPH_HIF_RegSetB(DSPY_RGB_V_BLANK, 45);
            MMPH_HIF_RegSetB(DSPY_RGB_VSYNC_W, 4);
            #endif

            MMPH_HIF_RegSetB(DSPY_RGB_PRT_2_H_DOT, 2);
            break;
        case MMP_HDMI_OUTPUT_1920X1080P_30FPS:
            MMPH_HIF_RegSetB(DSPY_RGB_CTL,     HSYNC_POLAR_HIGH |
                                            VSYNC_POLAR_HIGH
                                            );

            MMPH_HIF_RegSetB(DSPY_RGB_DOT_CLK_RATIO, 0);
            
            MMPH_HIF_RegSetB(DSPY_RGB_PORCH_HIGH_BIT_EXT,0x01);
            
            MMPH_HIF_RegSetB(DSPY_RGB_H_BPORCH, 0xC0);
            MMPH_HIF_RegSetB(DSPY_RGB_H_BLANK, 0x18);
            MMPH_HIF_RegSetB(DSPY_RGB_HSYNC_W, 43);
            
            MMPH_HIF_RegSetB(DSPY_RGB_V_BPORCH, 41);
            MMPH_HIF_RegSetB(DSPY_RGB_V_BLANK, 45);
            MMPH_HIF_RegSetB(DSPY_RGB_VSYNC_W, 4);
            
            MMPH_HIF_RegSetB(DSPY_RGB_PRT_2_H_DOT, 2);
            break;
        case MMP_HDMI_OUTPUT_1920X1080I:
          
            MMPH_HIF_RegSetB(DSPY_RGB_CTL,  HSYNC_POLAR_HIGH |
                                            VSYNC_POLAR_HIGH
                                            );

            MMPH_HIF_RegSetB(DSPY_RGB_DOT_CLK_RATIO, 0);

            MMPH_HIF_RegSetB(DSPY_RGB_PORCH_HIGH_BIT_EXT,0x01);
            
            MMPH_HIF_RegSetB(DSPY_RGB_H_BPORCH, 0xC0);
            MMPH_HIF_RegSetB(DSPY_RGB_H_BLANK, 0x18);
            MMPH_HIF_RegSetB(DSPY_RGB_HSYNC_W, 43);
            
            MMPH_HIF_RegSetB(DSPY_RGB_V_BPORCH, 20);
            MMPH_HIF_RegSetB(DSPY_RGB_V_BLANK, 22);
            MMPH_HIF_RegSetB(DSPY_RGB_VSYNC_W, 4);
                  
            MMPH_HIF_RegSetW(DSPY_PIP_TV_EVEN_FIELD_ST, 1620);//1080 * 1.5,to be test
            // in mmpf_vpp.c MMPF_DISPLAY_ISR : pDSPY->DSPY_PIP_TV_EVEN_FIELD_ST = pDSPY->DSPY_PIP_OFST_ROW >> 1;
            break;
        }
    }
    else if (rgbIf == MMP_DISPLAY_RGB_IF2)
    {
        m_bHdmiRgbIF1 = MMP_FALSE;
        MMPH_HIF_RegSetL(DSPY_LCD_TX_6, 0x0);
        MMPH_HIF_RegSetB(DSPY_RGB2_FMT,     hdmiattribute->ubColorType);
        MMPH_HIF_RegSetB(DSPY_RGB2_PORCH_HIGH_BIT_EXT, 0);
        
        MMPH_HIF_RegSetB(DSPY_RGB_SHARE_P_LCD_BUS,  HDMI_SRC_SEL_RGB2);
        switch(hdmiattribute->ubOutputMode){
        case MMP_HDMI_OUTPUT_USERDEF:
            MMPH_HIF_RegSetB(DSPY_RGB2_CTL,     VSYNC_POLAR_LOW |
                                        HSYNC_POLAR_LOW
                                        );

            MMPH_HIF_RegSetB(DSPY_RGB2_DOT_CLK_RATIO, 5);
            
            MMPH_HIF_RegSetB(DSPY_RGB2_H_BPORCH, 10);
            MMPH_HIF_RegSetB(DSPY_RGB2_H_BLANK, 14);
            MMPH_HIF_RegSetB(DSPY_RGB2_HSYNC_W, 10);
            
            MMPH_HIF_RegSetB(DSPY_RGB2_V_BPORCH, 2);
            MMPH_HIF_RegSetB(DSPY_RGB2_V_BLANK, 2);
            MMPH_HIF_RegSetB(DSPY_RGB2_VSYNC_W, 5);

            MMPH_HIF_RegSetB(DSPY_RGB2_PRT_2_H_DOT, 2);
            break;
        case MMP_HDMI_OUTPUT_640X480P:                
            MMPH_HIF_RegSetB(DSPY_RGB2_CTL,     HSYNC_POLAR_LOW |
                                        HSYNC_POLAR_LOW
                                        );

            MMPH_HIF_RegSetB(DSPY_RGB2_DOT_CLK_RATIO, 0);
            
            MMPH_HIF_RegSetB(DSPY_RGB2_H_BPORCH, 144);
            MMPH_HIF_RegSetB(DSPY_RGB2_H_BLANK, 160);
            MMPH_HIF_RegSetB(DSPY_RGB2_HSYNC_W, 95);
            
            MMPH_HIF_RegSetB(DSPY_RGB2_V_BPORCH, 35);
            MMPH_HIF_RegSetB(DSPY_RGB2_V_BLANK, 45);
            MMPH_HIF_RegSetB(DSPY_RGB2_VSYNC_W, 1);

            MMPH_HIF_RegSetB(DSPY_RGB2_PRT_2_H_DOT, 2);
            break;
        case MMP_HDMI_OUTPUT_720X480P:
            MMPH_HIF_RegSetB(DSPY_RGB2_CTL,     HSYNC_POLAR_LOW |
                                        HSYNC_POLAR_LOW
                                        );

            MMPH_HIF_RegSetB(DSPY_RGB2_DOT_CLK_RATIO, 0);
            
            MMPH_HIF_RegSetB(DSPY_RGB2_H_BPORCH, 122);
            MMPH_HIF_RegSetB(DSPY_RGB2_H_BLANK, 138);
            MMPH_HIF_RegSetB(DSPY_RGB2_HSYNC_W, 61);
            
            MMPH_HIF_RegSetB(DSPY_RGB2_V_BPORCH, 36);
            MMPH_HIF_RegSetB(DSPY_RGB2_V_BLANK, 45);
            MMPH_HIF_RegSetB(DSPY_RGB2_VSYNC_W, 5);

            MMPH_HIF_RegSetB(DSPY_RGB2_PRT_2_H_DOT, 2);
            break;
        case MMP_HDMI_OUTPUT_720X576P:                
            MMPH_HIF_RegSetB(DSPY_RGB2_CTL,     HSYNC_POLAR_LOW |
                                        HSYNC_POLAR_LOW
                                        );

            MMPH_HIF_RegSetB(DSPY_RGB2_DOT_CLK_RATIO, 0);
            
            MMPH_HIF_RegSetB(DSPY_RGB2_H_BPORCH, 132);
            MMPH_HIF_RegSetB(DSPY_RGB2_H_BLANK, 144);
            MMPH_HIF_RegSetB(DSPY_RGB2_HSYNC_W, 63);
            
            MMPH_HIF_RegSetB(DSPY_RGB2_V_BPORCH, 44);
            MMPH_HIF_RegSetB(DSPY_RGB2_V_BLANK, 49);
            MMPH_HIF_RegSetB(DSPY_RGB2_VSYNC_W, 4);

            MMPH_HIF_RegSetB(DSPY_RGB2_PRT_2_H_DOT, 2);
            break;
        case MMP_HDMI_OUTPUT_1280X720P:
            MMPH_HIF_RegSetB(DSPY_RGB2_CTL,     HSYNC_POLAR_HIGH |
                                            VSYNC_POLAR_HIGH
                                            );

            MMPH_HIF_RegSetB(DSPY_RGB2_DOT_CLK_RATIO, 0);
            
            // H Blanking = 370, Back porch = 260
            MMPH_HIF_RegSetB(DSPY_RGB2_PORCH_HIGH_BIT_EXT, 0x05);
            
            MMPH_HIF_RegSetB(DSPY_RGB2_H_BPORCH, 0x04);
            MMPH_HIF_RegSetB(DSPY_RGB2_H_BLANK, 0x72);
            MMPH_HIF_RegSetB(DSPY_RGB2_HSYNC_W, 39);
            
            #if 0
            MMPH_HIF_RegSetB(DSPY_RGB2_V_BPORCH, 30);
            MMPH_HIF_RegSetB(DSPY_RGB2_V_BLANK, 30);
            MMPH_HIF_RegSetB(DSPY_RGB2_VSYNC_W, 4);
            #else
            MMPH_HIF_RegSetB(DSPY_RGB2_V_BPORCH, 25);
            MMPH_HIF_RegSetB(DSPY_RGB2_V_BLANK, 30);
            MMPH_HIF_RegSetB(DSPY_RGB2_VSYNC_W, 4);
            #endif

            MMPH_HIF_RegSetB(DSPY_RGB2_PRT_2_H_DOT, 2);
            break;
        case MMP_HDMI_OUTPUT_1280X720P_50FPS:
            MMPH_HIF_RegSetB(DSPY_RGB2_CTL,     HSYNC_POLAR_HIGH |
                                            VSYNC_POLAR_HIGH
                                            );

            MMPH_HIF_RegSetB(DSPY_RGB2_DOT_CLK_RATIO, 0);
            
            // H Blanking = 700, Back porch = 260
            MMPH_HIF_RegSetB(DSPY_RGB2_PORCH_HIGH_BIT_EXT, 0x06);
            
            MMPH_HIF_RegSetB(DSPY_RGB2_H_BPORCH, 0x04);
            MMPH_HIF_RegSetB(DSPY_RGB2_H_BLANK, 0xBC);
            MMPH_HIF_RegSetB(DSPY_RGB2_HSYNC_W, 39);

            MMPH_HIF_RegSetB(DSPY_RGB2_V_BPORCH, 25);
            MMPH_HIF_RegSetB(DSPY_RGB2_V_BLANK, 30);
            MMPH_HIF_RegSetB(DSPY_RGB2_VSYNC_W, 4);

            MMPH_HIF_RegSetB(DSPY_RGB2_PRT_2_H_DOT, 2);
            break;
        case MMP_HDMI_OUTPUT_1920X1080P:
            MMPH_HIF_RegSetB(DSPY_RGB2_CTL,     HSYNC_POLAR_HIGH |
                                            VSYNC_POLAR_HIGH
                                            );

            MMPH_HIF_RegSetB(DSPY_RGB2_DOT_CLK_RATIO, 0);
            
            #if 0 // 50Hz
            MMPH_HIF_RegSetB(DSPY_RGB2_PORCH_HIGH_BIT_EXT,0x02);
            
            MMPH_HIF_RegSetB(DSPY_RGB2_H_BPORCH, 0xC0);
            MMPH_HIF_RegSetB(DSPY_RGB2_H_BLANK, 0xD0);
            MMPH_HIF_RegSetB(DSPY_RGB2_HSYNC_W, 43);
            
            MMPH_HIF_RegSetB(DSPY_RGB2_V_BPORCH, 41);
            MMPH_HIF_RegSetB(DSPY_RGB2_V_BLANK, 45);
            MMPH_HIF_RegSetB(DSPY_RGB2_VSYNC_W, 4);
            #else // 60Hz
            MMPH_HIF_RegSetB(DSPY_RGB2_PORCH_HIGH_BIT_EXT,0x01);
            
            MMPH_HIF_RegSetB(DSPY_RGB2_H_BPORCH, 0xC0);
            MMPH_HIF_RegSetB(DSPY_RGB2_H_BLANK, 0x18);
            MMPH_HIF_RegSetB(DSPY_RGB2_HSYNC_W, 43);
            
            MMPH_HIF_RegSetB(DSPY_RGB2_V_BPORCH, 41);
            MMPH_HIF_RegSetB(DSPY_RGB2_V_BLANK, 45);
            MMPH_HIF_RegSetB(DSPY_RGB2_VSYNC_W, 4);
            #endif

            MMPH_HIF_RegSetB(DSPY_RGB2_PRT_2_H_DOT, 2);
            break;
        case MMP_HDMI_OUTPUT_1920X1080P_30FPS:
            MMPH_HIF_RegSetB(DSPY_RGB2_CTL,     HSYNC_POLAR_HIGH |
                                            //HSYNC_POLAR_LOW
                                            VSYNC_POLAR_HIGH
                                            );

            MMPH_HIF_RegSetB(DSPY_RGB2_DOT_CLK_RATIO, 0);
            
            MMPH_HIF_RegSetB(DSPY_RGB2_PORCH_HIGH_BIT_EXT,0x01);
            
            MMPH_HIF_RegSetB(DSPY_RGB2_H_BPORCH, 0xC0);
            MMPH_HIF_RegSetB(DSPY_RGB2_H_BLANK, 0x18);
            //MMPH_HIF_RegSetB(DSPY_RGB_HSYNC_W, 44);
            MMPH_HIF_RegSetB(DSPY_RGB2_HSYNC_W, 43);
            
            MMPH_HIF_RegSetB(DSPY_RGB2_V_BPORCH, 41);
            MMPH_HIF_RegSetB(DSPY_RGB2_V_BLANK, 45);
            //MMPH_HIF_RegSetB(DSPY_RGB_VSYNC_W, 5);
            MMPH_HIF_RegSetB(DSPY_RGB_VSYNC_W, 4);

            MMPH_HIF_RegSetB(DSPY_RGB2_PRT_2_H_DOT, 2);
            break;
        case MMP_HDMI_OUTPUT_1920X1080I:
          
            MMPH_HIF_RegSetB(DSPY_RGB2_CTL,  HSYNC_POLAR_HIGH |
                                            //HSYNC_POLAR_LOW
                                            VSYNC_POLAR_HIGH
                                            );

            MMPH_HIF_RegSetB(DSPY_RGB2_DOT_CLK_RATIO, 0);

            MMPH_HIF_RegSetB(DSPY_RGB2_PORCH_HIGH_BIT_EXT,0x01);
            
            MMPH_HIF_RegSetB(DSPY_RGB2_H_BPORCH, 0xC0);
            MMPH_HIF_RegSetB(DSPY_RGB2_H_BLANK, 0x18);
            //MMPH_HIF_RegSetB(DSPY_RGB_HSYNC_W, 44);
            MMPH_HIF_RegSetB(DSPY_RGB2_HSYNC_W, 43);
            
            MMPH_HIF_RegSetB(DSPY_RGB2_V_BPORCH, 20);
            MMPH_HIF_RegSetB(DSPY_RGB2_V_BLANK, 22);

            MMPH_HIF_RegSetB(DSPY_RGB2_VSYNC_W, 4);
                  
            MMPH_HIF_RegSetW(DSPY_PIP_TV_EVEN_FIELD_ST, 1620);//1080 * 1.5,to be test
            // in mmpf_vpp.c MMPF_DISPLAY_ISR : pDSPY->DSPY_PIP_TV_EVEN_FIELD_ST = pDSPY->DSPY_PIP_OFST_ROW >> 1;
            break;
        }
    }

    MMPH_HIF_RegSetB(DSPY_RGB_SHARE_P_LCD_BUS,     0x20 );
	//for DUAL with RGB panel
    //MMPH_HIF_RegSetB(DSPY_RGB_SHARE_P_LCD_BUS,     RGB_LCD_ONLY);
    
    return MMP_ERR_NONE;
}
#endif//(ALL_FW)

/// @}

#ifdef BUILD_CE
#define BUILD_FW
#endif

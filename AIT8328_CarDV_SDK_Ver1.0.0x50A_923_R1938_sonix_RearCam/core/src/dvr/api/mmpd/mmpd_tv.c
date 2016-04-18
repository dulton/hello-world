//==============================================================================
//
//  File        : mmpd_tv.c
//  Description : Ritian TV Control driver function
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

/**
*  @file mmpd_tv.c
*  @brief The TV control functions
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
void ____TV_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_SetTVOutput
//  Description : 
//------------------------------------------------------------------------------
/** 
@brief The function set the needed parameter for TV 
@param[in] controller  the display controller
@param[in] tvattribute struct with TV related information
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Display_SetTVOutput(MMP_DISPLAY_CONTROLLER 	controller, 
								 MMP_TV_ATTR				*tvAttr,
								 MMP_DISPLAY_RGB_IF 		rgbIf)
{
    if (rgbIf == MMP_DISPLAY_RGB_IF1)
    {
        m_bHdmiRgbIF1 = MMP_TRUE;
        
    	MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) | TV_FIELD_SYNC_EN);
        MMPH_HIF_RegSetB(DSPY_RGB_FMT, MMPH_HIF_RegGetB(DSPY_RGB_FMT) & ~(DSPY_RGB_SYNC_MODE_DIS));
        MMPH_HIF_RegSetB(DSPY_RGB_CTL, MMPH_HIF_RegGetB(DSPY_RGB_CTL) & ~(RGB_IF_EN));
        while (MMPH_HIF_RegGetB(DSPY_RGB_CTL) & RGB_IF_EN);
        MMPH_HIF_RegSetB(DSPY_RGB_SHARE_P_LCD_BUS, RGBLCD_SRC_SEL_RGB);
    }
    else if(rgbIf == MMP_DISPLAY_RGB_IF2)
    {
        m_bHdmiRgbIF1 = MMP_FALSE;

        MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) | TV_FIELD_SYNC_EN);
        MMPH_HIF_RegSetB(DSPY_RGB2_FMT, MMPH_HIF_RegGetB(DSPY_RGB2_FMT) & ~(DSPY_RGB_SYNC_MODE_DIS));
        MMPH_HIF_RegSetB(DSPY_RGB2_CTL, MMPH_HIF_RegGetB(DSPY_RGB2_CTL) & ~(RGB_IF_EN));
        while (MMPH_HIF_RegGetB(DSPY_RGB2_CTL) & RGB_IF_EN);
        MMPH_HIF_RegSetB(DSPY_RGB_SHARE_P_LCD_BUS, RGBLCD_SRC_SEL_RGB2);
    }

	MMPD_System_EnableClock(MMPD_SYS_CLK_TV, MMP_TRUE);
	MMPD_System_ResetHModule(MMPD_SYS_MDL_DSPY, MMP_FALSE);

	MMPH_HIF_RegSetW(DSPY_CTL_4, MMPH_HIF_RegGetW(DSPY_CTL_4) & ~LCD_OUT_SEL_MASK);

   	{
   	    MMP_ULONG ulFreq;

   	    MMPD_System_GetGroupFreq(CLK_GRP_GBL, &ulFreq);
   	    
   	    if(ulFreq > 108000) {
   	        MMPH_HIF_RegSetB(TVIF_CLK_DELAY_V1, DELAY_1T);
   	    }
   	    else {
   	        MMPH_HIF_RegSetB(TVIF_CLK_DELAY_V1, NO_DELAY);
   	    }
   	}
   	
	MMPD_TV_SetInterface(tvAttr);

	if (controller == MMP_DISPLAY_PRM_CTL) {

		MMPH_HIF_RegSetW(DSPY_PIP_SOUT_CTL, DSPY_SOUT_RGB_888 | DSPY_SOUT_RGB);
		MMPH_HIF_RegSetL(DSPY_BG_COLOR, 	tvAttr->ulDspyBgColor);

	    MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) & ~(DSPY_PRM_SEL_MASK));
	    MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) | DSPY_PRM_SEL(DSPY_TYPE_TV));
	    MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) | DSPY_PRM_EN);

		MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) | PRM_DSPY_REG_READY);
	}
	else {
		MMPH_HIF_RegSetL(DSPY_SCD_BG_COLOR, tvAttr->ulDspyBgColor);

	    MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) & ~(DSPY_SCD_SEL_MASK));
	    MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) | DSPY_SCD_SEL(DSPY_TYPE_TV));
	    MMPH_HIF_RegSetW(DSPY_CTL_2, MMPH_HIF_RegGetW(DSPY_CTL_2) | DSPY_SCD_EN);

		MMPH_HIF_RegSetW(DSPY_CTL_0, MMPH_HIF_RegGetW(DSPY_CTL_0) | SCD_DSPY_REG_READY);
	}
		    
	MMPD_System_ResetHModule(MMPD_SYS_MDL_DSPY, MMP_FALSE);

	MMPD_TV_Initialize(MMP_TRUE, tvAttr->tvtype);
	MMPD_TV_EnableDisplay(MMP_TRUE);
    
    MMPF_Display_SetOutputDev(controller, MMP_DISPLAY_TV);      

	return MMP_ERR_NONE;  
}

//------------------------------------------------------------------------------
//  Function    : MMPD_TV_Initialize
//  Description : Enable TV interface
//------------------------------------------------------------------------------
/**
@brief The function call Firmware to init TV encoder
@param[in] bInit 1 for enable TC, 0 for disable TV.
*/
MMP_ERR MMPD_TV_Initialize(MMP_BOOL bInit, MMP_TV_TYPE output_panel)
{
    MMP_ULONG   reg;
    MMP_ERR     mmpstatus;
	
    mmpstatus = MMPD_System_TVInitialize(bInit);
    
    if (mmpstatus)
    	return mmpstatus;	

	if (output_panel == MMP_TV_TYPE_NTSC) {
	    reg = MMPH_HIF_RegGetL(TVENC_MODE_CTL);
        reg |= TV_SETUP_751RE_EN | TV_714MV_286MV_MODE;  /*7.5IRE enable, EIA/CEA-770.2, vpp=714/-286 mV picture/sync ratio*/
	    MMPH_HIF_RegSetL(TVENC_MODE_CTL,reg);

	    MMPH_HIF_RegSetL(TVENC_Y_SCALE_CTL,0x00040060);	     
	    MMPD_TV_SetContrast(0x60);
	    MMPD_TV_SetSaturation(0x40,0x5a);
   	    MMPH_HIF_RegSetL(TVENC_GAMMA_COEF_0,0x00000000);  /*Remove bright level offset*/
    }
    else {
        reg = MMPH_HIF_RegGetL(TVENC_MODE_CTL);
        reg &= ~(TV_SETUP_751RE_EN | TV_714MV_286MV_MODE) ; /*7.5IRE disable, EIA/CEA-770.2, vpp=700/-300 mV picture/sync ratio */
	    MMPH_HIF_RegSetL(TVENC_MODE_CTL,reg);    
	    MMPD_TV_SetContrast(0x66);
	    MMPD_TV_SetSaturation(0x45,0x62);
   	    MMPH_HIF_RegSetL(TVENC_GAMMA_COEF_0,0x00000000);    /*Remove bright level offset*/
    }
   	
    return MMP_ERR_NONE;    
}

//------------------------------------------------------------------------------
//  Function    : MMPD_TV_EnableDisplay
//  Description : Enable TV interface
//------------------------------------------------------------------------------
/** 
@brief The function enable TV_IF. After enable TV_IF, the real signal send to TV

@param[in] enable  1 to enable TV IF,  0 to disable TV IF
*/
MMP_ERR MMPD_TV_EnableDisplay(MMP_UBYTE enable)
{
    if (enable) {
        MMPH_HIF_RegSetB(TVIF_IF_EN, (MMPH_HIF_RegGetB(TVIF_IF_EN) | TV_ENC_IF_EN));
    }
    else {
        MMPH_HIF_RegSetB(TVIF_IF_EN, (MMPH_HIF_RegGetB(TVIF_IF_EN) & ~TV_ENC_IF_EN));
    	MMPD_System_ResetHModule(MMPD_SYS_MDL_DSPY, MMP_FALSE);
    }
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_TV_SetInterface
//  Description : Initialize TV interface
//------------------------------------------------------------------------------
/** @brief The function set all attribute for TV

The function set TV attributes such as type(NTSC/PAL), start X, Y, Width, Height and background color.
@param[in] tvattribute Attributes for type(NTSC/PAL), start X, Y, Width, Height and background color.
*/
MMP_ERR MMPD_TV_SetInterface(MMP_TV_ATTR *tvattribute)
{
    if (tvattribute->tvtype == MMP_TV_TYPE_NTSC) {
    	if (((tvattribute->usStartX + tvattribute->usDisplayWidth) > 720) ||
    		((tvattribute->usStartY + tvattribute->usDisplayHeight) > 480)) {
    		return	MMP_DISPLAY_ERR_PARAMETER;		
		}    			
      	MMPH_HIF_RegSetB(TVIF_IF_EN, (TV_IF_DAC_CTL | TV_DISPLAY_SPECIFIED_IMAGE | TV_TYPE_NTSC));

        MMPH_HIF_RegSetW(TVIF_NTSC_ODFIELD_LINE, 20);
        MMPH_HIF_RegSetW(TVIF_NTSC_EVFIELD_LINE, 283);
        MMPH_HIF_RegSetW(TVIF_NTSC_EVLINE_SUB1, MMPH_HIF_RegGetW(TVIF_NTSC_EVFIELD_LINE) - 1);
        
        MMPH_HIF_RegSetB(TVIF_EARLY_PIXL, 30);
        MMPH_HIF_RegSetB(TVIF_1ST_PXL_RQST_TIMING, 98);
    }
    else if (tvattribute->tvtype == MMP_TV_TYPE_PAL) {  // MMP_TV_TYPE_PAL
    	if (((tvattribute->usStartX + tvattribute->usDisplayWidth) > 720) ||
    		((tvattribute->usStartY + tvattribute->usDisplayHeight) > 576)) {
        		return	MMP_DISPLAY_ERR_PARAMETER;		
        }
        MMPH_HIF_RegSetB(TVIF_IF_EN, (TV_IF_DAC_CTL | TV_DISPLAY_SPECIFIED_IMAGE | TV_TYPE_PAL));
        
        MMPH_HIF_RegSetB(TVIF_EARLY_PIXL, 24);
        MMPH_HIF_RegSetB(TVIF_1ST_PXL_RQST_TIMING, 110);
    }
    else {
		return	MMP_DISPLAY_ERR_PARAMETER;		
    }

    MMPH_HIF_RegSetB(TVIF_DAC_IF_1ST_CTL, TV_DAC_POWER_DOWN_EN | TV_BGREF_POWER_DOWN_EN);

    MMPH_HIF_RegSetW(DSPY_W, tvattribute->usDisplayWidth);
	MMPH_HIF_RegSetW(DSPY_H, tvattribute->usDisplayHeight >> 1);
	MMPH_HIF_RegSetL(DSPY_PIXL_CNT, tvattribute->usDisplayWidth * (tvattribute->usDisplayHeight >> 1));

    MMPH_HIF_RegSetW(TVIF_IMAGE_START_X, (tvattribute->usStartX + 1));
    MMPH_HIF_RegSetW(TVIF_IMAGE_START_Y, (tvattribute->usStartY + 1));

    MMPH_HIF_RegSetW(TVIF_IMAGE_WIDTH, tvattribute->usDisplayWidth);
	MMPH_HIF_RegSetW(TVIF_IMAGE_HEIGHT, tvattribute->usDisplayHeight);

	MMPH_HIF_RegSetB(TVIF_BACKGROUND_Y_COLOR, tvattribute->ubTvBgYColor);
	MMPH_HIF_RegSetB(TVIF_BACKGROUND_U_COLOR, tvattribute->ubTvBgUColor);
	MMPH_HIF_RegSetB(TVIF_BACKGROUND_V_COLOR, tvattribute->ubTvBgVColor);

	// TV interface frame start line number
    MMPH_HIF_RegSetB(TVIF_ENDLINE_OFFSET_CTL, 0x88);

    //MMPH_HIF_RegSetB(TVIF_EARLY_PIXL, 26);
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_TV_EncRegSet
//  Description : General function for setting TV encoder register
//------------------------------------------------------------------------------
/** @brief General function for setting TV encoder register
@param[in] addr Register address.
@param[in] data Register value.
@retval MMP_ERR_NONE Success.
@retval MMP_DISPLAY_ERR_NOT_SUPPORT Fail.
*/
MMP_ERR MMPD_TV_EncRegSet(MMP_ULONG addr, MMP_ULONG data)
{
    MMPH_HIF_RegSetL(addr, data);
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_TV_EncRegGet
//  Description : General function for getting TV encoder register
//------------------------------------------------------------------------------
/** @brief General function for getting TV encoder register
@param[in] addr Register address.
@param[out] data Register value.
@retval MMP_ERR_NONE Success.
@retval MMP_DISPLAY_ERR_NOT_SUPPORT Fail.
*/
MMP_ERR MMPD_TV_EncRegGet(MMP_ULONG addr, MMP_ULONG *data)
{
    *data = MMPH_HIF_RegGetL(addr);
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_TV_SetBrightLevel
//  Description : Function for setting TV Bright Level
//------------------------------------------------------------------------------
/** @brief Function for setting TV Bright Level
@param[in] ubLevel Bright level
@retval MMP_ERR_NONE Success.
@retval MMP_DISPLAY_ERR_NOT_SUPPORT Fail.
*/
MMP_ERR MMPD_TV_SetBrightLevel(MMP_UBYTE ubLevel)
{
	MMP_ULONG ulReg;
	
	MMPD_TV_EncRegGet(TVENC_GAMMA_COEF_0, &ulReg);
	ulReg = (ulReg & 0x00FFFFFF) | ((MMP_ULONG)ubLevel << 24);
	MMPD_TV_EncRegSet(TVENC_GAMMA_COEF_0, ulReg);
	
	return MMP_ERR_NONE;
}

MMP_ERR MMPD_TV_SetGamma0(MMP_USHORT ubLevel)
{
	MMP_ULONG ulReg;
	
	MMPD_TV_EncRegGet(TVENC_GAMMA_COEF_0, &ulReg);
	ulReg = (ulReg & 0xFFFF0000) | ((MMP_ULONG)ubLevel & 0x0fff);
	MMPD_TV_EncRegSet(TVENC_GAMMA_COEF_0, ulReg);
	
	return MMP_ERR_NONE;
}

MMP_ERR MMPD_TV_SetGamma1(MMP_USHORT ubLevel)
{
	MMP_ULONG ulReg;
	
	MMPD_TV_EncRegGet(TVENC_GAMMA_COEF_1_2, &ulReg);
	ulReg = (ulReg & 0xFFFF0000) | ((MMP_USHORT)ubLevel & 0x0fff);
	MMPD_TV_EncRegSet(TVENC_GAMMA_COEF_1_2, ulReg);
	
	return MMP_ERR_NONE;
}

MMP_ERR MMPD_TV_SetGamma2(MMP_USHORT ubLevel)
{
	MMP_ULONG ulReg;
	
	MMPD_TV_EncRegGet(TVENC_GAMMA_COEF_1_2, &ulReg);
	ulReg = (ulReg & 0x0000FFFF) | (((MMP_USHORT)ubLevel & 0x0fff)<<16);
	MMPD_TV_EncRegSet(TVENC_GAMMA_COEF_1_2, ulReg);
	
	return MMP_ERR_NONE;
}

MMP_ERR MMPD_TV_SetGamma3(MMP_USHORT ubLevel)
{
	MMP_ULONG ulReg;
	
	MMPD_TV_EncRegGet(TVENC_GAMMA_COEF_3_4, &ulReg);
	ulReg = (ulReg & 0xFFFF0000) | ((MMP_USHORT)ubLevel & 0x0fff);
	MMPD_TV_EncRegSet(TVENC_GAMMA_COEF_3_4, ulReg);
	
	return MMP_ERR_NONE;
}

MMP_ERR MMPD_TV_SetGamma4(MMP_USHORT ubLevel)
{
	MMP_ULONG ulReg;
	
	MMPD_TV_EncRegGet(TVENC_GAMMA_COEF_3_4, &ulReg);
	ulReg = (ulReg & 0x0000FFFF) | (((MMP_USHORT)ubLevel & 0x0fff)<<16);
	MMPD_TV_EncRegSet(TVENC_GAMMA_COEF_3_4, ulReg);
	
	return MMP_ERR_NONE;
}

MMP_ERR MMPD_TV_SetGamma5(MMP_USHORT ubLevel)
{
	MMP_ULONG ulReg;
	
	MMPD_TV_EncRegGet(TVENC_GAMMA_COEF_5_6, &ulReg);
	ulReg = (ulReg & 0xFFFF0000) | ((MMP_USHORT)ubLevel & 0x0fff);
	MMPD_TV_EncRegSet(TVENC_GAMMA_COEF_5_6, ulReg);
	
	return MMP_ERR_NONE;
}

MMP_ERR MMPD_TV_SetGamma6(MMP_USHORT ubLevel)
{
	MMP_ULONG ulReg;
	
	MMPD_TV_EncRegGet(TVENC_GAMMA_COEF_5_6, &ulReg);
	ulReg = (ulReg & 0x0000FFFF) | (((MMP_USHORT)ubLevel & 0x0fff)<<16);
	MMPD_TV_EncRegSet(TVENC_GAMMA_COEF_5_6, ulReg);
	
	return MMP_ERR_NONE;
}

MMP_ERR MMPD_TV_SetGamma7(MMP_USHORT ubLevel)
{
	MMP_ULONG ulReg;
	
	MMPD_TV_EncRegGet(TVENC_GAMMA_COEF_7_8, &ulReg);
	ulReg = (ulReg & 0xFFFF0000) | ((MMP_USHORT)ubLevel & 0x0fff);
	MMPD_TV_EncRegSet(TVENC_GAMMA_COEF_7_8, ulReg);
	
	return MMP_ERR_NONE;
}

MMP_ERR MMPD_TV_SetGamma8(MMP_USHORT ubLevel)
{
	MMP_ULONG ulReg;
	
	MMPD_TV_EncRegGet(TVENC_GAMMA_COEF_7_8, &ulReg);
	ulReg = (ulReg & 0x0000FFFF) | (((MMP_USHORT)ubLevel & 0x0fff)<<16);
	MMPD_TV_EncRegSet(TVENC_GAMMA_COEF_7_8, ulReg);
	
	return MMP_ERR_NONE;
}

MMP_ERR MMPD_TV_GetGamma(MMP_DISPLAY_TV_GAMMA *DspyGamma)
{
	MMP_ULONG ulReg;
	
	MMPD_TV_EncRegGet(TVENC_GAMMA_COEF_0, &ulReg);
	DspyGamma->usGamma0 = (ulReg & 0x00000FFF);
	MMPD_TV_EncRegGet(TVENC_GAMMA_COEF_1_2, &ulReg);
	DspyGamma->usGamma1 = (ulReg & 0x00000FFF);	
	DspyGamma->usGamma2 = (ulReg & 0x0FFF0000)>>16;	
    MMPD_TV_EncRegGet(TVENC_GAMMA_COEF_3_4, &ulReg);
	DspyGamma->usGamma3 = (ulReg & 0x00000FFF);	
	DspyGamma->usGamma4 = (ulReg & 0x0FFF0000)>>16;	
    MMPD_TV_EncRegGet(TVENC_GAMMA_COEF_5_6, &ulReg);
	DspyGamma->usGamma5 = (ulReg & 0x00000FFF);	
	DspyGamma->usGamma6 = (ulReg & 0x0FFF0000)>>16;	
    MMPD_TV_EncRegGet(TVENC_GAMMA_COEF_7_8, &ulReg);
	DspyGamma->usGamma7 = (ulReg & 0x00000FFF);	
	DspyGamma->usGamma8 = (ulReg & 0x0FFF0000)>>16;	
	return MMP_ERR_NONE;
}
//------------------------------------------------------------------------------
//  Function    : MMPD_TV_GetBrightLevel
//  Description : Function for getting TV Bright Level
//------------------------------------------------------------------------------
/** @brief Function for getting TV Bright Level
@param[in] *pubLevel Bright level
@retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_TV_GetBrightLevel(MMP_UBYTE *pubLevel)
{
	MMP_ULONG ulReg;
	
	MMPD_TV_EncRegGet(TVENC_GAMMA_COEF_0, &ulReg);
    *pubLevel = (MMP_UBYTE) (ulReg >> 24); 
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_TV_SetContrast
//  Description :
//------------------------------------------------------------------------------
/**
@brief Luma scale of contrast in main channel.
@param[in] usLevel Contrast level.
@retval MMP_ERR_NONE Success.
@retval MMP_DISPLAY_ERR_NOT_SUPPORT Fail.
*/
MMP_ERR MMPD_TV_SetContrast(MMP_USHORT usLevel)
{
	MMP_ULONG uldata;

	MMPD_TV_EncRegGet(TVENC_Y_SCALE_CTL, &uldata);
	uldata = (uldata & 0xFEFFFF00) | (usLevel & 0xFF);
	uldata = (usLevel & 0x100) ? (uldata | 0x01000000) : uldata;
	MMPD_TV_EncRegSet(TVENC_Y_SCALE_CTL, uldata);
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_TV_GetContrast
//  Description :
//------------------------------------------------------------------------------
/**
@brief Luma scale of contrast in main channel.
@param[in] *pusLevel Contrast level.
@retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_TV_GetContrast(MMP_USHORT *pusLevel)
{
	MMP_ULONG uldata;

	MMPD_TV_EncRegGet(TVENC_Y_SCALE_CTL, &uldata);
    *pusLevel = ((uldata & 0x01000000) ? 0x0100 : 0x0000) | (uldata & 0x00FF);
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_TV_SetSaturation
//  Description :
//------------------------------------------------------------------------------
/**
@brief Chroma scale of saturation in main and second channels.
@param[in] ubUlevel Saturation U scale level.
@param[in] ubVlevel Saturation V scale level.
@retval MMP_ERR_NONE Success.
@retval MMP_DISPLAY_ERR_NOT_SUPPORT Fail.
*/
MMP_ERR MMPD_TV_SetSaturation(MMP_UBYTE ubUlevel, MMP_UBYTE ubVlevel)
{
	MMP_ULONG uldata;
	
	uldata = ((MMP_ULONG)ubUlevel << 24) | ((MMP_ULONG)ubUlevel << 16) |
			 ((MMP_ULONG)ubUlevel << 8)  | ((MMP_ULONG)ubUlevel);
	MMPD_TV_EncRegSet(TVENC_U_SCALE_CTL, uldata);
	uldata = ((MMP_ULONG)ubVlevel << 24) | ((MMP_ULONG)ubVlevel << 16) |
			 ((MMP_ULONG)ubVlevel << 8)  | ((MMP_ULONG)ubVlevel);
	MMPD_TV_EncRegSet(TVENC_V_SCALE_CTL, uldata);
	uldata = ((MMP_ULONG)ubVlevel << 24) | ((MMP_ULONG)ubVlevel << 16) |
			 ((MMP_ULONG)ubUlevel << 8)  | ((MMP_ULONG)ubUlevel);
	MMPD_TV_EncRegSet(TVENC_UV_SCALE_GAIN_4_5, uldata);
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_TV_GetSaturation
//  Description :
//------------------------------------------------------------------------------
/**
@brief Chroma scale of saturation in main and second channels.
@param[in] *pubUlevel Saturation U scale level.
@param[in] *pubVlevel Saturation V scale level.
@retval MMP_ERR_NONE Success.
@retval MMP_DISPLAY_ERR_NOT_SUPPORT Fail.
*/
MMP_ERR MMPD_TV_GetSaturation(MMP_UBYTE *pubUlevel, MMP_UBYTE *pubVlevel)
{
	MMP_ULONG uldata;
	
	MMPD_TV_EncRegGet(TVENC_U_SCALE_CTL, &uldata);
    *pubUlevel = (MMP_UBYTE) (uldata & 0x000000FF);
	MMPD_TV_EncRegGet(TVENC_V_SCALE_CTL, &uldata);
    *pubVlevel = (MMP_UBYTE) (uldata & 0x000000FF);
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_TV_SetInterlaceMode
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_TV_SetInterlaceMode(MMP_BOOL bEnable)
{
    return MMPF_TV_SetInterlaceMode(bEnable);
}

/// @}

#ifdef BUILD_CE
#define BUILD_FW
#endif

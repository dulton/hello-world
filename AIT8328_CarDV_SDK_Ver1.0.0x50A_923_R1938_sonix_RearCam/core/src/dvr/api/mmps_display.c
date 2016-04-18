//==============================================================================
//
//  File        : mmps_display.c
//  Description : Ritian Display Control host function, including LCD/TV/Win
//  Author      : Alan Wu
//  Revision    : 1.0
//
//==============================================================================

/**
*  @file mmps_display.c
*  @brief The Display control functions
*  @author Alan Wu
*  @version 1.0
*/

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "mmps_display.h"
#include "mmps_system.h"
#include "mmps_dsc.h"
#include "mmps_3gprecd.h"
#include "mmps_vidplay.h"
#include "mmpd_ccir.h"
#include "mmpd_system.h"
#include "mmpd_scaler.h"
#include "mmpd_dma.h"
#include "ait_utility.h"

#if defined(WIFI_PORT) && (WIFI_PORT == 1)
#include "netapp.h"
#endif

/** @addtogroup MMPS_DISPLAY
@{
*/

//==============================================================================
//
//                              GLOBAL VARIABLES
//
//==============================================================================

/**@brief The system display mode

Use @ref MMPS_Display_SetOutputPanel to assign the field value of it.
*/
static MMPS_DISPLAY_CONFIG      m_displayConfig;

/**@brief The display type in each controller

Use @ref MMPS_Display_SetOutputPanel to set it.
And use @ref MMPS_Display_GetOutputPanel to get the current mode.
*/
static MMP_DISPLAY_OUTPUT_SEL	m_displayCurOutput[MMP_DISPLAY_CTL_MAX] = {MMP_DISPLAY_SEL_NONE, MMP_DISPLAY_SEL_NONE};

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_GetConfig
//  Description :
//------------------------------------------------------------------------------
/** @brief The function gets the Display configuration for the host application

The function gets the current Display configuration for reference by the host application. 

@return It return the pointer of the Display configuration data structure.
*/
MMPS_DISPLAY_CONFIG* MMPS_Display_GetConfig(void)
{
    return &m_displayConfig;
}

#if 0
void ____Output_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_SetOutputPanel
//  Description :
//------------------------------------------------------------------------------
/** @brief The function designates the output display device

The function designates the output device 
@param[in] controller  the display controller
@param[in] dispOutSel the display type
@return It reports the status of the operation.
*/
MMP_ERR MMPS_Display_SetOutputPanel(MMP_DISPLAY_CONTROLLER controller, MMP_DISPLAY_OUTPUT_SEL dispOutSel)
{
    MMP_DISPLAY_WIN_ID      winID;
    MMP_DISPLAY_CONTROLLER  winBindController;
    MMP_LCD_ATTR            lcdAttr;

	// Remove later
	MMPD_Display_BindWinToController(MMP_DISPLAY_WIN_MAIN,		MMP_DISPLAY_PRM_CTL);
    MMPD_Display_BindWinToController(MMP_DISPLAY_WIN_PIP,		MMP_DISPLAY_PRM_CTL);
    MMPD_Display_BindWinToController(MMP_DISPLAY_WIN_OVERLAY,	MMP_DISPLAY_PRM_CTL);
    MMPD_Display_BindWinToController(MMP_DISPLAY_WIN_OSD,	    MMP_DISPLAY_PRM_CTL);
	
	if (m_displayCurOutput[controller] == dispOutSel) {
		// To reset the LCD width/height and some important things. In bypass mode, customer maybe 
		// use the partial LCD refresh and change the LCD width/height by LCD command. So when AIT 
		// is active, we have to change the LCD width/height back.
		switch (dispOutSel) {
		case MMP_DISPLAY_SEL_MAIN_LCD:
		    if (m_displayConfig.mainlcd.reinitialfx)
                m_displayConfig.mainlcd.reinitialfx(MMP_FALSE);
			break;
		case MMP_DISPLAY_SEL_SUB_LCD:
            if (m_displayConfig.sublcd.reinitialfx)
                m_displayConfig.sublcd.reinitialfx(MMP_FALSE);
			break;
		}

		return MMP_ERR_NONE;
    }

	/* Inactive all windows */
    for (winID = MMP_DISPLAY_WIN_MAIN; winID < MMP_DISPLAY_WIN_MAX; winID++) {

    	MMPD_Display_GetWinBindController(winID, &winBindController);
    	
    	if (winBindController == controller)
            MMPD_Display_SetWinActive(winID, MMP_FALSE);
    }

	/* Disable the orignal output and config new output */
	switch (dispOutSel) {
	case MMP_DISPLAY_SEL_NONE:
    case MMP_DISPLAY_SEL_CCIR:
		if ((m_displayCurOutput[controller] == MMP_DISPLAY_SEL_NTSC_TV) ||
			(m_displayCurOutput[controller] == MMP_DISPLAY_SEL_PAL_TV)) {
		    
		    MMPD_TV_EnableDisplay(MMP_FALSE);
		    
		    if (m_displayCurOutput[controller] == MMP_DISPLAY_SEL_NTSC_TV)
			    MMPD_TV_Initialize(MMP_FALSE, MMP_TV_TYPE_NTSC);
			else
    			MMPD_TV_Initialize(MMP_FALSE, MMP_TV_TYPE_PAL);
    			
            MMPD_System_EnableClock(MMPD_SYS_CLK_TV, MMP_FALSE);
		}

        #if (HDMI_OUTPUT_EN)
        if (m_displayCurOutput[controller] == MMP_DISPLAY_SEL_HDMI) {
            MMPD_HDMI_DisableHDMI();
        }
        #endif

        if ((m_displayCurOutput[controller] == MMP_DISPLAY_SEL_MAIN_LCD && m_displayConfig.mainlcd.lcdtype == MMP_DISPLAY_RGB_LCD) ||
            (m_displayCurOutput[controller] == MMP_DISPLAY_SEL_SUB_LCD && m_displayConfig.sublcd.lcdtype == MMP_DISPLAY_RGB_LCD)) {

            MMPD_Display_DisableRGBOutput(controller, MMP_DISPLAY_RGB_IF1);
        }

        if (dispOutSel == MMP_DISPLAY_SEL_NONE)
            MMPD_Display_SetNoneOutput(controller);
        #if defined(ALL_FW)
        else
            MMPD_Display_SetCCIROutput(controller, m_displayConfig.ccir);
        #endif
		break;

	case MMP_DISPLAY_SEL_MAIN_LCD:

		if ((m_displayCurOutput[controller] == MMP_DISPLAY_SEL_NTSC_TV) ||
			(m_displayCurOutput[controller] == MMP_DISPLAY_SEL_PAL_TV)) {
			
		    MMPD_TV_EnableDisplay(MMP_FALSE);
		    
			if (m_displayCurOutput[controller] == MMP_DISPLAY_SEL_NTSC_TV)
			    MMPD_TV_Initialize(MMP_FALSE, MMP_TV_TYPE_NTSC);
			else
    			MMPD_TV_Initialize(MMP_FALSE, MMP_TV_TYPE_PAL);
    			
            MMPD_System_EnableClock(MMPD_SYS_CLK_TV, MMP_FALSE);
		}

        #if (HDMI_OUTPUT_EN)
        if (m_displayCurOutput[controller] == MMP_DISPLAY_SEL_HDMI) {
            MMPD_HDMI_DisableHDMI();
        }
        #endif

		if ((m_displayConfig.mainlcd.lcdtype == MMP_DISPLAY_P_LCD) ||
			(m_displayConfig.mainlcd.lcdtype == MMP_DISPLAY_RGB_LCD) ||
			(m_displayConfig.mainlcd.lcdtype == MMP_DISPLAY_P_LCD_FLM)) {

			if (m_displayConfig.mainlcd.lcdtype == MMP_DISPLAY_RGB_LCD) {
                MMPD_System_EnableClock(MMPD_SYS_CLK_HDMI, MMP_TRUE);
			}

            if (m_displayConfig.mainlcd.initialfx) {
			    m_displayConfig.mainlcd.initialfx(MMP_TRUE);
			}

            lcdAttr.usWidth     = m_displayConfig.mainlcd.usWidth;
            lcdAttr.usHeight    = m_displayConfig.mainlcd.usHeight;
			lcdAttr.colordepth  = m_displayConfig.mainlcd.colordepth;
            lcdAttr.ulBgColor   = m_displayConfig.mainlcd.ulBgColor;
			lcdAttr.bFLMType    = m_displayConfig.mainlcd.bFLMType;
			
			if ((m_displayConfig.mainlcd.lcdtype == MMP_DISPLAY_P_LCD) || 
			    (m_displayConfig.mainlcd.lcdtype == MMP_DISPLAY_P_LCD_FLM)) {

				MMPD_Display_SetPLCDOutput(controller, m_displayConfig.mainlcd.cssel, &lcdAttr);
			}
			else if (m_displayConfig.mainlcd.lcdtype == MMP_DISPLAY_RGB_LCD) {

                MMPD_Display_SetRGBLCDOutput(controller, &lcdAttr, MMP_DISPLAY_RGB_IF1);
            }
		}
        else if (m_displayConfig.mainlcd.lcdtype == MMP_DISPLAY_NONE) {
        
		    if (m_displayConfig.mainlcd.initialfx) {
			    m_displayConfig.mainlcd.initialfx(MMP_FALSE);
			}
            MMPD_Display_SetNoneOutput(controller);
        }
		break;

	case MMP_DISPLAY_SEL_SUB_LCD:

		if  ((m_displayCurOutput[controller] == MMP_DISPLAY_SEL_NTSC_TV) ||
			 (m_displayCurOutput[controller] == MMP_DISPLAY_SEL_PAL_TV)) {
			
		    MMPD_TV_EnableDisplay(MMP_FALSE);
		    
			if (m_displayCurOutput[controller] == MMP_DISPLAY_SEL_NTSC_TV)
			    MMPD_TV_Initialize(MMP_FALSE, MMP_TV_TYPE_NTSC);
			else
    			MMPD_TV_Initialize(MMP_FALSE, MMP_TV_TYPE_PAL);

            MMPD_System_EnableClock(MMPD_SYS_CLK_TV, MMP_FALSE);
        }

        #if (HDMI_OUTPUT_EN)
        if (m_displayCurOutput[controller] == MMP_DISPLAY_SEL_HDMI) {
            MMPD_HDMI_DisableHDMI();
        }
        #endif

		if ((m_displayConfig.sublcd.lcdtype == MMP_DISPLAY_P_LCD) ||
			(m_displayConfig.sublcd.lcdtype == MMP_DISPLAY_RGB_LCD) ||
			(m_displayConfig.sublcd.lcdtype == MMP_DISPLAY_P_LCD_FLM)) {

			if(m_displayConfig.sublcd.lcdtype == MMP_DISPLAY_RGB_LCD) {
                MMPD_System_EnableClock(MMPD_SYS_CLK_HDMI, MMP_TRUE);
			}

            if (m_displayConfig.sublcd.initialfx) {
			    m_displayConfig.sublcd.initialfx(MMP_TRUE);
			}

			lcdAttr.usWidth     = m_displayConfig.sublcd.usWidth;
			lcdAttr.usHeight    = m_displayConfig.sublcd.usHeight;
            lcdAttr.colordepth  = m_displayConfig.sublcd.colordepth;
            lcdAttr.ulBgColor   = m_displayConfig.sublcd.ulBgColor;
			lcdAttr.bFLMType    = m_displayConfig.sublcd.bFLMType;
			
			if ((m_displayConfig.sublcd.lcdtype == MMP_DISPLAY_P_LCD) || 
			    (m_displayConfig.sublcd.lcdtype == MMP_DISPLAY_P_LCD_FLM)) {

				MMPD_Display_SetPLCDOutput(controller, m_displayConfig.sublcd.cssel, &lcdAttr);					
			}
			else if (m_displayConfig.sublcd.lcdtype == MMP_DISPLAY_RGB_LCD) {
			
                MMPD_Display_SetRGBLCDOutput(controller, &lcdAttr, MMP_DISPLAY_RGB_IF1);
			}
		}
        else if (m_displayConfig.mainlcd.lcdtype == MMP_DISPLAY_NONE) {

            MMPD_Display_SetNoneOutput(controller);
        }
		break;

	case MMP_DISPLAY_SEL_NTSC_TV:
	case MMP_DISPLAY_SEL_PAL_TV:
	
		#if (HDMI_OUTPUT_EN)
        if (m_displayCurOutput[controller] == MMP_DISPLAY_SEL_HDMI) {
            MMPD_HDMI_DisableHDMI();
        }
        #endif

        if ((m_displayCurOutput[controller] == MMP_DISPLAY_SEL_MAIN_LCD && m_displayConfig.mainlcd.lcdtype == MMP_DISPLAY_RGB_LCD) ||
            (m_displayCurOutput[controller] == MMP_DISPLAY_SEL_SUB_LCD && m_displayConfig.sublcd.lcdtype == MMP_DISPLAY_RGB_LCD)) {

            MMPD_Display_DisableRGBOutput(controller, MMP_DISPLAY_RGB_IF1);
        }
		
		if (dispOutSel == MMP_DISPLAY_SEL_NTSC_TV)
			MMPD_Display_SetTVOutput(controller, &m_displayConfig.ntsctv, MMP_DISPLAY_RGB_IF1);
		else
			MMPD_Display_SetTVOutput(controller, &m_displayConfig.paltv, MMP_DISPLAY_RGB_IF1);

		break;

    case MMP_DISPLAY_SEL_HDMI:

		if ((m_displayCurOutput[controller] == MMP_DISPLAY_SEL_NTSC_TV) ||
			(m_displayCurOutput[controller] == MMP_DISPLAY_SEL_PAL_TV)) {
		    
		    MMPD_TV_EnableDisplay(MMP_FALSE);
		    
			if (m_displayCurOutput[controller] == MMP_DISPLAY_SEL_NTSC_TV)
			    MMPD_TV_Initialize(MMP_FALSE, MMP_TV_TYPE_NTSC);
			else
    			MMPD_TV_Initialize(MMP_FALSE, MMP_TV_TYPE_PAL);
    			
            MMPD_System_EnableClock(MMPD_SYS_CLK_TV, MMP_FALSE);
        }

        if ((m_displayCurOutput[controller] == MMP_DISPLAY_SEL_MAIN_LCD && m_displayConfig.mainlcd.lcdtype == MMP_DISPLAY_RGB_LCD) ||
            (m_displayCurOutput[controller] == MMP_DISPLAY_SEL_SUB_LCD && m_displayConfig.sublcd.lcdtype == MMP_DISPLAY_RGB_LCD)) {

            MMPD_Display_DisableRGBOutput(controller, MMP_DISPLAY_RGB_IF1);
        }

		MMPD_Display_SetHDMIOutput(controller, &m_displayConfig.hdmi, MMP_DISPLAY_RGB_IF1);
        break;
	}

	m_displayCurOutput[controller] = dispOutSel;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_SetOutputPanelMediaError
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_Display_SetOutputPanelMediaError(MMP_DISPLAY_CONTROLLER controller, MMP_DISPLAY_OUTPUT_SEL dispOutSel)
{
	MMP_LCD_ATTR 	lcdAttr;

	if (m_displayCurOutput[controller] == dispOutSel) {
		// To reset the LCD width/height and some important things. In bypass mode, customer maybe
		// use the partial LCD refresh and change the LCD width/height by LCD command. So when AIT
		// is active, we have to change the LCD width/height back.
		switch (dispOutSel) {
		case MMP_DISPLAY_SEL_MAIN_LCD:
		    if (m_displayConfig.mainlcd.reinitialfx)
                m_displayConfig.mainlcd.reinitialfx(MMP_FALSE);
			break;
		case MMP_DISPLAY_SEL_SUB_LCD:
            if (m_displayConfig.sublcd.reinitialfx)
                m_displayConfig.sublcd.reinitialfx(MMP_FALSE);
			break;
		}

		return	MMP_ERR_NONE;
	}

	/* Disable the orignal output and config new output */
	switch (dispOutSel) {
	case MMP_DISPLAY_SEL_NONE:
    case MMP_DISPLAY_SEL_CCIR:

		if ((m_displayCurOutput[controller] == MMP_DISPLAY_SEL_NTSC_TV) ||
			(m_displayCurOutput[controller] == MMP_DISPLAY_SEL_PAL_TV)) {

		    MMPD_TV_EnableDisplay(MMP_FALSE);

		    if (m_displayCurOutput[controller] == MMP_DISPLAY_SEL_NTSC_TV)
			    MMPD_TV_Initialize(MMP_FALSE, MMP_TV_TYPE_NTSC);
			else
    			MMPD_TV_Initialize(MMP_FALSE, MMP_TV_TYPE_PAL);

            MMPD_System_EnableClock(MMPD_SYS_CLK_TV, MMP_FALSE);
		}

        #if (HDMI_OUTPUT_EN)
        if (m_displayCurOutput[controller] == MMP_DISPLAY_SEL_HDMI) {
            MMPD_HDMI_DisableHDMI();
        }
        #endif

        if ((m_displayCurOutput[controller] == MMP_DISPLAY_SEL_MAIN_LCD && m_displayConfig.mainlcd.lcdtype == MMP_DISPLAY_RGB_LCD) ||
            (m_displayCurOutput[controller] == MMP_DISPLAY_SEL_SUB_LCD && m_displayConfig.sublcd.lcdtype == MMP_DISPLAY_RGB_LCD)) {

            MMPD_Display_DisableRGBOutput(controller, MMP_DISPLAY_RGB_IF1);
        }
        
	    if (dispOutSel == MMP_DISPLAY_SEL_NONE)
            MMPD_Display_SetNoneOutput(controller);
        #if defined(ALL_FW)
        else
            MMPD_Display_SetCCIROutput(controller, m_displayConfig.ccir);
        #endif
	    break;

	case MMP_DISPLAY_SEL_MAIN_LCD:

		if ((m_displayConfig.mainlcd.lcdtype == MMP_DISPLAY_P_LCD) ||
			(m_displayConfig.mainlcd.lcdtype == MMP_DISPLAY_RGB_LCD) ||
			(m_displayConfig.mainlcd.lcdtype == MMP_DISPLAY_P_LCD_FLM)) {

            if (m_displayConfig.mainlcd.initialfx) {
			    m_displayConfig.mainlcd.initialfx(MMP_TRUE);
			}

			lcdAttr.usWidth 	= m_displayConfig.mainlcd.usWidth;
			lcdAttr.usHeight 	= m_displayConfig.mainlcd.usHeight;
			lcdAttr.colordepth 	= m_displayConfig.mainlcd.colordepth;
			lcdAttr.ulBgColor 	= m_displayConfig.mainlcd.ulBgColor;
			lcdAttr.bFLMType    = m_displayConfig.mainlcd.bFLMType;
			
			if ((m_displayConfig.mainlcd.lcdtype == MMP_DISPLAY_P_LCD) || 
				(m_displayConfig.mainlcd.lcdtype == MMP_DISPLAY_P_LCD_FLM)) {

				MMPD_Display_SetPLCDOutput(controller, m_displayConfig.mainlcd.cssel, &lcdAttr);
			}
			else if (m_displayConfig.mainlcd.lcdtype == MMP_DISPLAY_RGB_LCD) {

                MMPD_Display_SetRGBLCDOutput(controller, &lcdAttr, MMP_DISPLAY_RGB_IF1);
			}
		}
		else if (m_displayConfig.mainlcd.lcdtype == MMP_DISPLAY_NONE) {

		    if (m_displayConfig.mainlcd.initialfx) {
			    m_displayConfig.mainlcd.initialfx(MMP_FALSE);
			}
            MMPD_Display_SetNoneOutput(controller);
        }
		break;

    case MMP_DISPLAY_SEL_HDMI:

		if  ((m_displayCurOutput[controller] == MMP_DISPLAY_SEL_NTSC_TV) ||
			 (m_displayCurOutput[controller] == MMP_DISPLAY_SEL_PAL_TV)) {
		    
		    MMPD_TV_EnableDisplay(MMP_FALSE);

			if (m_displayCurOutput[controller] == MMP_DISPLAY_SEL_NTSC_TV)
			    MMPD_TV_Initialize(MMP_FALSE, MMP_TV_TYPE_NTSC);
			else
    			MMPD_TV_Initialize(MMP_FALSE, MMP_TV_TYPE_PAL);
		}

        if ((m_displayCurOutput[controller] == MMP_DISPLAY_SEL_MAIN_LCD && m_displayConfig.mainlcd.lcdtype == MMP_DISPLAY_RGB_LCD) ||
            (m_displayCurOutput[controller] == MMP_DISPLAY_SEL_SUB_LCD && m_displayConfig.sublcd.lcdtype == MMP_DISPLAY_RGB_LCD)) {
            
            MMPD_Display_DisableRGBOutput(controller, MMP_DISPLAY_RGB_IF1);
        }

        MMPD_Display_SetHDMIOutput(controller, &m_displayConfig.hdmi, MMP_DISPLAY_RGB_IF1);
        break;
	}

	m_displayCurOutput[controller] = dispOutSel;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_GetOutputPanel
//  Description :
//------------------------------------------------------------------------------
/** @brief The function retrives the current output display device

The function retrives the current output display device 
@param[in] controller  	The display controller
@param[out] dispOutSel  The display type
@return It reports the status of the operation.
*/
MMP_ERR MMPS_Display_GetOutputPanel(MMP_DISPLAY_CONTROLLER controller, MMP_DISPLAY_OUTPUT_SEL *pDispOutSel)
{
	*pDispOutSel = m_displayCurOutput[controller];

    return MMP_ERR_NONE;
}


//------------------------------------------------------------------------------
//  Function    : MMPS_Display_GetRGBDotClk
//  Description :
//------------------------------------------------------------------------------
/** @brief: Get the RGB panel Dot clock
*/
MMP_ERR MMPS_Display_GetRGBDotClk(MMP_ULONG *ulRGBDotClk)
{
	return MMPD_Display_GetRGBDotClk(ulRGBDotClk);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_WaitDisplayRefresh
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_Display_WaitDisplayRefresh(MMP_DISPLAY_CONTROLLER controller)
{
    return MMPD_Display_WaitDisplayRefresh(controller);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_SetDisplayRefresh
//  Description :
//------------------------------------------------------------------------------
/** @brief Refresh display data from the frame buffer to LCD

@return It reports the total effect number
*/
MMP_ERR MMPS_Display_SetDisplayRefresh(MMP_DISPLAY_CONTROLLER controller)
{
    #if !(defined(MBOOT_FW)||defined(UPDATER_FW)||defined(MBOOT_EX_FW))
    MMP_M_STATE     state;
    MMP_BOOL        bAvail = MMP_FALSE;
    MMP_BOOL        bInStreaming = MMP_FALSE;   // CarDV
    
    #if defined(WIFI_PORT) && (WIFI_PORT == 1)
	if (get_NetAppStatus() & NETAPP_STREAM_PLAY) {
	    bInStreaming = MMP_TRUE;
    }
    #endif

    MMPS_VIDPLAY_GetState(&state);
    
    if (state == MMP_M_STATE_EXECUTING) {
        MMPS_VIDPLAY_GetVideoAvailable(&bAvail);
        if (bAvail) {
            return MMP_ERR_NONE;
        }
    }
    
    MMPS_DSC_GetPreviewStatus(&bAvail);
    if (bAvail && (MMP_FALSE == bInStreaming)) {    // CarDV
    	return MMP_ERR_NONE;
    }
    
    MMPS_3GPRECD_GetPreviewPipeStatus(0/*PRM_SENSOR*/, &bAvail);
    if (bAvail && (MMP_FALSE == bInStreaming)) {    // CarDV
    	return MMP_ERR_NONE;
    }
    #endif

    return MMPD_Display_SetDisplayRefresh(controller);
}

#if 0
void ____Window_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_ReadWinBuffer
//  Description :
//------------------------------------------------------------------------------
/** @brief The function reads the display data of the specified Window

@param[in] winID the Wiondow ID
@param[in] usMemPtr the address of host buffer
@param[in] usWidth the width of clip
@param[in] usHeight the height of clip
@param[in] usStartx the X offset of window
@param[in] usStarty the Y offset of window
@return It reports the status of the operation.
*/
MMP_ERR MMPS_Display_ReadWinBuffer(MMP_DISPLAY_WIN_ID winID, MMP_USHORT *usMemPtr)
{
    MMP_DISPLAY_WIN_ATTR 	winAttr;
    MMP_USHORT				usSize = 0;

    MMPD_Display_GetWinAttributes(winID, &winAttr);
    
    if (winAttr.colordepth == MMP_DISPLAY_WIN_COLORDEPTH_8) {
    	usSize = winAttr.usWidth * winAttr.usHeight * 1;
    }    
    else if (winAttr.colordepth == MMP_DISPLAY_WIN_COLORDEPTH_16) {
        usSize = winAttr.usWidth * winAttr.usHeight * 2;
    }    
    else if (winAttr.colordepth == MMP_DISPLAY_WIN_COLORDEPTH_24) {
		usSize = winAttr.usWidth * winAttr.usHeight * 3;
    }
    else if (winAttr.colordepth == MMP_DISPLAY_WIN_COLORDEPTH_32) {
		usSize = winAttr.usWidth * winAttr.usHeight * 4;
    }
    else if (winAttr.colordepth == MMP_DISPLAY_WIN_COLORDEPTH_YUV422) {
        usSize = winAttr.usWidth * winAttr.usHeight * 2;
	}
    else if (winAttr.colordepth == MMP_DISPLAY_WIN_COLORDEPTH_YUV420 ||
    		 winAttr.colordepth == MMP_DISPLAY_WIN_COLORDEPTH_YUV420_INTERLEAVE) {
        usSize = winAttr.usWidth * winAttr.usHeight * 3 / 2;
	}
	
	MMPD_DMA_MoveData(winAttr.ulBaseAddr, (MMP_ULONG)usMemPtr, usSize, NULL, NULL);
	
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_WriteWinBuffer
//  Description :
//------------------------------------------------------------------------------
/** @brief The function writes the data into the specified Window

@param[in] winID the Wiondow ID
@param[in] usMemPtr the address of host buffer
@param[in] usWidth the width of clip
@param[in] usHeight the height of clip
@param[in] usStartx the X offset of window
@param[in] usStarty the Y offset of window
@return It reports the status of the operation.
*/
MMP_ERR MMPS_Display_WriteWinBuffer(MMP_DISPLAY_WIN_ID winID, MMP_USHORT *usMemPtr)
{
    MMP_DISPLAY_WIN_ATTR 	winAttr;
    MMP_USHORT              usSize = 0;
    
    MMPD_Display_GetWinAttributes(winID, &winAttr);
    
    if (winAttr.colordepth == MMP_DISPLAY_WIN_COLORDEPTH_8) {
    	usSize = winAttr.usWidth * winAttr.usHeight * 1;
    }    
    else if (winAttr.colordepth == MMP_DISPLAY_WIN_COLORDEPTH_16) {
        usSize = winAttr.usWidth * winAttr.usHeight * 2;
    }    
    else if (winAttr.colordepth == MMP_DISPLAY_WIN_COLORDEPTH_24) {
		usSize = winAttr.usWidth * winAttr.usHeight * 3;
    }
    else if (winAttr.colordepth == MMP_DISPLAY_WIN_COLORDEPTH_32) {
		usSize = winAttr.usWidth * winAttr.usHeight * 4;
    }
    else if (winAttr.colordepth == MMP_DISPLAY_WIN_COLORDEPTH_YUV422) {
        usSize = winAttr.usWidth * winAttr.usHeight * 2;
	}
    else if (winAttr.colordepth == MMP_DISPLAY_WIN_COLORDEPTH_YUV420 ||
    		 winAttr.colordepth == MMP_DISPLAY_WIN_COLORDEPTH_YUV420_INTERLEAVE) {
        usSize = winAttr.usWidth * winAttr.usHeight * 3 / 2;
	}
    
    MMPD_DMA_MoveData((MMP_ULONG)usMemPtr, winAttr.ulBaseAddr, usSize, NULL, NULL);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_SetWinPriority
//  Description :
//------------------------------------------------------------------------------
/** @brief The function sets window display priorities by window ID

The function sets window display priorities by window IDs, the input parameters. The first parameter
gives the highest priority to that window and the fourth parameter gives the lowest priority to it. The
function can be used for dual panels.

@param[in] prio1 window ID for the first priority
@param[in] prio2 window ID for the second priority
@param[in] prio3 window ID for the third priority
@param[in] prio4 window ID for the fourth priority

@return It reports the status of the operation.
*/
MMP_ERR MMPS_Display_SetWinPriority(MMP_DISPLAY_WIN_ID prio1, MMP_DISPLAY_WIN_ID prio2, 
                                    MMP_DISPLAY_WIN_ID prio3, MMP_DISPLAY_WIN_ID prio4)
{
    return MMPD_Display_SetWinPriority(prio1, prio2, prio3, prio4);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_ClearWinBuffer
//  Description :
//------------------------------------------------------------------------------
/** @brief The function writes the data into the specified Window

@param[in] winID the Wiondow ID
@param[in] ulClearColor the clear color. If window color attribute is YUV422. then the low-word (16-bits LSB)
		ulClearColor will be translated as the same YCb and YCr.If window color attribute is YUV420, then
		ulClearColor represent 0(31-24), Cr(23-16) Cb(15-8) and Y(7-0).
@param[in] usWidth the width of clip
@param[in] usHeight the height of clip
@param[in] usStartx the X offset of window
@param[in] usStarty the Y offset of window
@return It reports the status of the operation.
*/
MMP_ERR MMPS_Display_ClearWinBuffer(MMP_DISPLAY_WIN_ID winID, 
									MMP_ULONG   ulClearColor,
                                    MMP_USHORT  usWidth,    MMP_USHORT usHeight, 
                                    MMP_USHORT  usStartX,   MMP_SHORT usStartY)
{
    MMP_DISPLAY_WIN_ATTR 	winAttr;
    MMP_GRAPHICS_RECT		rect;

    MMPD_Display_GetWinAttributes(winID, &winAttr);

	rect.usLeft 	= usStartX;
	rect.usTop 		= usStartY;
	rect.usWidth 	= usWidth;
	rect.usHeight 	= usHeight;

    return MMPD_Display_ClearWindowBuf(&winAttr, &rect, ulClearColor);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_SetWinSemiTransparent
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_Display_SetWinSemiTransparent(MMP_DISPLAY_WIN_ID 		winID, 
										   MMP_BOOL 				bSemiTpActive, 
										   MMP_DISPLAY_SEMITP_FUNC 	semifunc, 
										   MMP_USHORT 				usSemiWeight)
{
    if (usSemiWeight >= 256) {
        return MMP_DISPLAY_ERR_PARAMETER;
    }

    return MMPD_Display_SetWinSemiTransparent(winID, bSemiTpActive, semifunc, usSemiWeight);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_GetWinSemiTransparent
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_Display_GetWinSemiTransparent(MMP_DISPLAY_WIN_ID winID, MMP_USHORT* usSemiWeight)
{
    return MMPD_Display_GetWinSemiTransparent(winID, usSemiWeight);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_SetWinActive
//  Description :
//------------------------------------------------------------------------------
/** @brief The function activates or deactivates the window

The function activates or deactivates the window from the input parameter, winID. The function can be
used for dual panels.

@param[in] winID the window ID
@param[in] bActive activate or de-activate the window
@return It reports the status of the operation.
*/
MMP_ERR MMPS_Display_SetWinActive(MMP_DISPLAY_WIN_ID winID, MMP_BOOL bActive)
{
    return MMPD_Display_SetWinActive(winID, bActive);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_SetWinDuplicate
//  Description :
//------------------------------------------------------------------------------
/** @brief The function do duplication of a window

The function do duplication of a window and it only support RGB format, before do
this function, the MMPD_Display_SetWinAttributes and MMPD_Display_SetWinToDisplay
must be done. the flow is MMPD_Display_SetWinAttributes->MMPD_Display_SetWinToDisplay
->MMPS_Display_SetWinDuplicate

@param[in] winID 	The window ID
@param[in] h_ratio	Scaling factor for width. 
@param[in] v_ratio 	Scaling factor for height.
@return It reports the status of the operation.
*/
MMP_ERR MMPS_Display_SetWinDuplicate(MMP_DISPLAY_WIN_ID 	winID, 
                					 MMP_DISPLAY_DUPLICATE 	h_ratio, 
                					 MMP_DISPLAY_DUPLICATE 	v_ratio)
{
    return MMPD_Display_SetWinDuplicate(winID, h_ratio, v_ratio);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_SetWinTransparent
//  Description :
//------------------------------------------------------------------------------
MMP_ULONG MMPS_Display_SetWinTransparent(MMP_DISPLAY_WIN_ID winID,
                                         MMP_BOOL 			bTranspActive, 
                                         MMP_ULONG 			ulTranspColor)
{
    return MMPD_Display_SetWinTransparent(winID, bTranspActive, ulTranspColor);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_SetAlphaBlending
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_Display_SetAlphaBlending(MMP_DISPLAY_WIN_ID      	winID, 
                                      MMP_BYTE                  bAlphaEn, 
									  MMP_DISPLAY_ALPHA_FMT  	alphaformat)
{
    return MMPD_Display_SetAlphaBlending(winID, bAlphaEn, alphaformat);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_SetWindowAttrToDisp
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_Display_SetWindowAttrToDisp(MMP_DISPLAY_WIN_ID  	winID, 
                                    	 MMP_DISPLAY_WIN_ATTR 	winAttr,
                                    	 MMP_DISPLAY_DISP_ATTR	dispAttr)
{
    MMPD_Display_SetWinAttributes(winID, &winAttr);
    MMPD_Display_SetWinToDisplay(winID, &dispAttr);
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_LoadWinPalette
//  Description : 
//------------------------------------------------------------------------------
/** @brief The function activates transparent operation by its transparent color with weight

The function activates transparent operation by its transparent color with weight from the input parameter

@param[in] winID the window ID
@param[in] ubPalette palette data
@param[in] usEntry define the number of palette
@return It reports the status of the operation.
*/
MMP_ERR MMPS_Display_LoadWinPalette(MMP_DISPLAY_WIN_ID winID, MMP_UBYTE *ubPalette, MMP_USHORT usEntry)
{
    return MMPD_Display_LoadWinPalette(winID, ubPalette, usEntry);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_UpdateWinAddr
//  Description : Set window address
//------------------------------------------------------------------------------
/** 
@brief     Update the base address of a window 
@param[in]  winID       The window that address will be update
@param[in]  ulBaseAddr  New address (address of Y if YUV420)
@param[in]  ulBaseUAddr New address of U if YUV420
@param[in]  ulBaseVAddr new address of V if YUV420
@return It reports the status of the operation.
*/
MMP_ERR MMPS_Display_UpdateWinAddr(MMP_DISPLAY_WIN_ID 	winID, 
                            	   MMP_ULONG 			ulBaseAddr, 
                            	   MMP_ULONG 			ulBaseUAddr, 
                            	   MMP_ULONG 			ulBaseVAddr)
{  
    return MMPD_Display_UpdateWinAddr(winID, ulBaseAddr, ulBaseUAddr, ulBaseVAddr);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_SetWinScaleAndOffset
//  Description : Set window scaling and offset
//------------------------------------------------------------------------------
MMP_ERR MMPS_Display_SetWinScaleAndOffset(MMP_DISPLAY_WIN_ID 	winID,
                                          MMP_UBYTE		        sFitMode,
                                          MMP_ULONG             ulInputW,
                                          MMP_ULONG             ulInputH,
                                          MMP_ULONG             ulOutputW,
                                          MMP_ULONG             ulOutputH,
                                          MMP_USHORT            usWinOffsetX,
                                          MMP_USHORT            usWinOffsetY)
{
#if defined(ALL_FW)
    MMP_SCAL_FIT_RANGE  fitrange;
    MMP_SCAL_GRAB_CTRL  DispGrabctl;
    
    fitrange.fitmode        = sFitMode;
    fitrange.scalerType 	= MMP_SCAL_TYPE_WINSCALER;
    fitrange.ulInWidth      = ulInputW;
    fitrange.ulInHeight     = ulInputH;
    fitrange.ulOutWidth     = ulOutputW;
    fitrange.ulOutHeight    = ulOutputH;

	fitrange.ulInGrabX 		= 1;
    fitrange.ulInGrabY 		= 1;
    fitrange.ulInGrabW 		= fitrange.ulInWidth;
    fitrange.ulInGrabH 		= fitrange.ulInHeight;
    
    MMPD_Scaler_GetGCDBestFitScale(&fitrange, &DispGrabctl);

    MMPD_Display_SetWinScaleAndOffset(winID, &fitrange, &DispGrabctl, usWinOffsetX, usWinOffsetY);
#endif
    return MMP_ERR_NONE;
}


MMP_ERR MMPS_Display_AdjustWinColorMatrix(  MMP_DISPLAY_WIN_ID 	winID, 
                                            MMP_DISPLAY_WIN_COLRMTX *DspyColrMtx, 
                                            MMP_BOOL bApplyColMtx) 
{
   return MMPD_Display_AdjustWinColorMatrix(winID, DspyColrMtx, bApplyColMtx);
}
#if 0
void ____Ratio_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_GetPixelXYRatio
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPS_Display_GetPixelXYRatio(MMP_DISPLAY_OUTPUT_SEL PanelType, 
                                     MMP_ULONG      *ulXRatio,  MMP_ULONG *ulYRatio, 
                                     MMP_ULONG      *ulWidth,   MMP_ULONG *ulHeight, 
                                     MMP_ULONG64    *ulPixelWidth,
                                     MMP_BOOL       *bRotateVerticalLCD)
{
    switch (PanelType) {
    case MMP_DISPLAY_SEL_MAIN_LCD:
        if (m_displayConfig.mainlcd.bRotateVerticalLCD) {
            *ulXRatio   = m_displayConfig.mainlcd.ulPanelYratio;
            *ulYRatio   = m_displayConfig.mainlcd.ulPanelXratio;
            *ulWidth    = m_displayConfig.mainlcd.usHeight;
            *ulHeight   = m_displayConfig.mainlcd.usWidth;
        }
        else {
            *ulXRatio   = m_displayConfig.mainlcd.ulPanelXratio;
            *ulYRatio   = m_displayConfig.mainlcd.ulPanelYratio;
            *ulWidth    = m_displayConfig.mainlcd.usWidth;
            *ulHeight   = m_displayConfig.mainlcd.usHeight;
        }
        *ulPixelWidth 	    = ((*ulXRatio * (*ulHeight)) << 16) / (*ulYRatio * (*ulWidth));
        *bRotateVerticalLCD = m_displayConfig.mainlcd.bRotateVerticalLCD;
        break;
    case MMP_DISPLAY_SEL_SUB_LCD:
        if (m_displayConfig.sublcd.bRotateVerticalLCD) {
            *ulXRatio   = m_displayConfig.sublcd.ulPanelYratio;
            *ulYRatio   = m_displayConfig.sublcd.ulPanelXratio;
            *ulWidth    = m_displayConfig.sublcd.usHeight;
            *ulHeight   = m_displayConfig.sublcd.usWidth;
        }
        else {
            *ulXRatio   = m_displayConfig.sublcd.ulPanelXratio;
            *ulYRatio   = m_displayConfig.sublcd.ulPanelYratio;
            *ulWidth    = m_displayConfig.sublcd.usWidth;
            *ulHeight   = m_displayConfig.sublcd.usHeight;
        }
        *ulPixelWidth 	    = ((*ulXRatio * (*ulHeight)) << 16) / (*ulYRatio * (*ulWidth));
        *bRotateVerticalLCD = m_displayConfig.sublcd.bRotateVerticalLCD;
        break;
    case MMP_DISPLAY_SEL_NTSC_TV:
        *ulXRatio 		    = 3;
        *ulYRatio 		    = 2;
        *ulWidth  		    = m_displayConfig.ntsctv.usDisplayWidth;
        *ulHeight 		    = m_displayConfig.ntsctv.usDisplayHeight;
        *ulPixelWidth 	    = 1 << 16;
        *bRotateVerticalLCD = MMP_FALSE;
        break;
    case MMP_DISPLAY_SEL_PAL_TV:
        *ulXRatio 		    = 5;
        *ulYRatio 		    = 4;
        *ulWidth  		    = m_displayConfig.paltv.usDisplayWidth;
        *ulHeight 		    = m_displayConfig.paltv.usDisplayHeight;
        *ulPixelWidth 	    = 1 << 16;
        *bRotateVerticalLCD = MMP_FALSE;
        break;
    case MMP_DISPLAY_SEL_HDMI:
        *ulXRatio 		    = 16;
        *ulYRatio 		    = 9;
        *ulWidth  		    = m_displayConfig.hdmi.usDisplayWidth;
        *ulHeight 		    = m_displayConfig.hdmi.usDisplayHeight;
        *ulPixelWidth 	    = 1 << 16;
        *bRotateVerticalLCD = MMP_FALSE;
        break;
    case MMP_DISPLAY_SEL_CCIR:
        *ulXRatio 		    = 3;
        *ulYRatio 		    = 2;
        *ulWidth  		    = m_displayConfig.ccir.usDisplayWidth;
        *ulHeight 		    = m_displayConfig.ccir.usDisplayHeight;
        *ulPixelWidth 	    = 1 << 16;
        *bRotateVerticalLCD = MMP_FALSE;
        break;
    default:
        return MMP_DISPLAY_ERR_NOT_SUPPORT;
    }
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_CheckPixelRatio
//  Description : Check if panel pixel is square or not. And get the pixel width-height ratio.
//------------------------------------------------------------------------------
/**
@brief Check if panel pixel is square or not

@param[in]  controller    the display controller
@param[out] bSquarePixel  Square pixel or not
@param[out] ulPixelWidth  XY ratio 16.16
*/
MMP_ERR MMPS_Display_CheckPixelRatio(MMP_DISPLAY_CONTROLLER controller, MMP_BOOL *bSquarePixel, MMP_ULONG *ulPixelWidth)
{
    MMP_ULONG                   ulPanelXratio       = m_displayConfig.mainlcd.ulPanelXratio;
    MMP_ULONG                   ulPanelYratio       = m_displayConfig.mainlcd.ulPanelYratio;
    MMP_ULONG                   panel_width         = m_displayConfig.mainlcd.usWidth;
    MMP_ULONG                   panel_height        = m_displayConfig.mainlcd.usHeight;
    MMP_ULONG64                 pixel_width         = ((ulPanelXratio * panel_height) << 16) / (ulPanelYratio * panel_width);
    MMP_BOOL                    bRotateVerticalLCD  = m_displayConfig.mainlcd.bRotateVerticalLCD;
    MMP_DISPLAY_OUTPUT_SEL      displaypanel        = m_displayCurOutput[controller];

    MMPS_Display_GetPixelXYRatio(displaypanel, &ulPanelXratio, &ulPanelYratio, &panel_width, &panel_height, &pixel_width, &bRotateVerticalLCD);

    if (pixel_width == (1 << 16)) {
        *bSquarePixel = MMP_TRUE;
    }
    else {
        *bSquarePixel = MMP_FALSE;
    }
    *ulPixelWidth = (MMP_ULONG)pixel_width;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_AdjustScaleInSize
//  Description : According to the given input buffer width/height and the requested display width/height,
//                calculate the suggest width/height for the display buffer
//------------------------------------------------------------------------------
/** 
@brief     According to the given input buffer width/height and the requested display width/height,
           calculate the suggest width/height for the display buffer

@param[in]  controller      the display controller
@param[in]  InputWidth      Input buffer width
@param[in]  InputHeight     Input buffer height
@param[in]  RequiredWidth   Required display width
@param[in]  RequiredHeight  Required display height
@param[out] SuggestWidth    Suggest buffer width
@param[out] SuggestHeight   Suggest buffer height
*/
MMP_ERR MMPS_Display_AdjustScaleInSize( MMP_DISPLAY_CONTROLLER 	controller,
                                        MMP_ULONG               InputWidth,
                                        MMP_ULONG               InputHeight,
                                        MMP_ULONG               RequiredWidth,
                                        MMP_ULONG               RequiredHeight,
                                        MMP_ULONG               *SuggestWidth,
                                        MMP_ULONG               *SuggestHeight)
{
    MMP_ULONG                   ulPanelXratio       = m_displayConfig.mainlcd.ulPanelXratio;
    MMP_ULONG                   ulPanelYratio       = m_displayConfig.mainlcd.ulPanelYratio;
    MMP_ULONG                   panel_width         = m_displayConfig.mainlcd.usWidth;
    MMP_ULONG                   panel_height        = m_displayConfig.mainlcd.usHeight;    
    MMP_ULONG64                 pixel_width         = ((ulPanelXratio * panel_height) << 16) / (ulPanelYratio * panel_width);
    MMP_BOOL                    bRotateVerticalLCD  = m_displayConfig.mainlcd.bRotateVerticalLCD;
    MMP_DISPLAY_OUTPUT_SEL    	displaypanel        = m_displayCurOutput[controller];
    
    MMPS_Display_GetPixelXYRatio(displaypanel, &ulPanelXratio, &ulPanelYratio, &panel_width, &panel_height, &pixel_width, &bRotateVerticalLCD);
    switch(displaypanel)
    {
        case MMP_DISPLAY_SEL_MAIN_LCD:
        case MMP_DISPLAY_SEL_SUB_LCD:
 	*SuggestWidth  = ((RequiredWidth * InputWidth * ulPanelYratio) / (InputHeight * ulPanelXratio)) & 0xFFFFFFFE;
    *SuggestHeight = RequiredHeight & 0xFFFFFFFE;
    
    if (*SuggestWidth > RequiredWidth) {
        *SuggestWidth  = RequiredWidth & 0xFFFFFFFE;
        *SuggestHeight = ((RequiredHeight * InputHeight * ulPanelXratio) / (InputWidth * ulPanelYratio)) & 0xFFFFFFFE;
    }
        break;
        case MMP_DISPLAY_SEL_NTSC_TV:
        case MMP_DISPLAY_SEL_PAL_TV:
        case MMP_DISPLAY_SEL_HDMI:
        case MMP_DISPLAY_SEL_CCIR:
            *SuggestWidth  = RequiredWidth & 0xFFFFFFFE;
            *SuggestHeight = RequiredHeight & 0xFFFFFFFE;
        break;
        default:
        return MMP_DISPLAY_ERR_NOT_SUPPORT;
    }
	#if defined(ALL_FW) && defined(SCAL_FUNC_DBG)
    {
    extern void printc( char* szFormat, ... );

    printc("%s,IW:%d,IH:%d,RW:%d,RH:%d,SW:%d,SH:%d\r\n",
    		__func__, InputWidth, InputHeight, RequiredWidth, RequiredHeight, *SuggestWidth, *SuggestHeight);
    }
	#endif

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Display_PureAdjustScaleInSize
//  Description : According to the given input buffer width/height and the requested display width/height,
//                calculate the suggest width/height for the buffer independ display device
//------------------------------------------------------------------------------
/**
@brief     According to the given input buffer width/height and the requested display width/height,
           calculate the suggest width/height for the buffer independ display device

@param[in]  bScaleUpAvail   Can use the scaler or not
@param[in]  InputWidth      Input buffer width
@param[in]  InputHeight     Input buffer height
@param[in]  RequiredWidth   Required display width
@param[in]  RequiredHeight  Required display height
@param[out] SuggestWidth    Suggest buffer width
@param[out] SuggestHeight   Suggest buffer height
*/
MMP_ERR MMPS_Display_PureAdjustScaleInSize( MMP_BOOL    bScaleUpAvail,
                                            MMP_ULONG   InputWidth,
                                            MMP_ULONG   InputHeight,
                                            MMP_ULONG   RequiredWidth,
                                            MMP_ULONG   RequiredHeight,
                                            MMP_ULONG   *SuggestWidth,
                                            MMP_ULONG   *SuggestHeight)
{
	if (InputHeight == 1088) {
		InputHeight = 1080; // For keep 1920 X 1080 16 : 9 ratio
	}

	if ( (InputWidth * RequiredHeight) >= (InputHeight * RequiredWidth) ) {
		// 16:9 => 4:3 or same ratio => fit width
		if (InputWidth >= RequiredWidth) {
			*SuggestWidth = ALIGN16(RequiredWidth & ~(1));
			*SuggestHeight = (*SuggestWidth * InputHeight / InputWidth) & ~(1);
		}
		else {
			if (bScaleUpAvail) {
				*SuggestWidth = ALIGN16(RequiredWidth & ~(1));
				*SuggestHeight = (*SuggestWidth * InputHeight / InputWidth) & ~(1);
			}
			else {
				*SuggestWidth = ALIGN16(InputWidth & ~(1));
				*SuggestHeight = InputHeight & ~(1);
			}
		}
	}
	else
	{
		// 4:3 => 16:9 => fit height
		if (InputHeight >= RequiredHeight) {
			*SuggestHeight = RequiredHeight & ~(1);
			*SuggestWidth = ALIGN16((*SuggestHeight * InputWidth / InputHeight) & ~(1));
		}
		else {
			if (bScaleUpAvail) {
				*SuggestHeight = RequiredHeight & ~(1);
				*SuggestWidth = ALIGN16((*SuggestHeight * InputWidth / InputHeight) & ~(1));
			}
			else {
				*SuggestWidth = ALIGN16(InputWidth & ~(1));
				*SuggestHeight = InputHeight & ~(1);
			}
		}
	}

	return MMP_ERR_NONE;
}

#if 0
void ____TV_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_TV_SetBrightLevel
//  Description :
//------------------------------------------------------------------------------
/**
@brief Brightness adjustment on Luma in CVBS/SVideo.
@param[in] ubLevel Brightness level.
@retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_TV_SetBrightLevel(MMP_UBYTE ubLevel)
{
    return MMPD_TV_SetBrightLevel(ubLevel);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_TV_SetContrast
//  Description :
//------------------------------------------------------------------------------
/**
@brief Luma scale of contrast in main channel.
@param[in] usLevel Contrast level.
@retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_TV_SetContrast(MMP_USHORT usLevel)
{
    return MMPD_TV_SetContrast(usLevel);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_TV_SetSaturation
//  Description :
//------------------------------------------------------------------------------
/**
@brief Chroma scale of saturation in main and second channels.
@param[in] ubUlevel Saturation U scale level.
@param[in] ubVlevel Saturation V scale level.
@retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPS_TV_SetSaturation(MMP_UBYTE ubUlevel, MMP_UBYTE ubVlevel)
{
    return MMPD_TV_SetSaturation(ubUlevel, ubVlevel);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_TV_SetInterlaceMode
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_TV_SetInterlaceMode(MMP_BOOL bEnable)
{
    return MMPD_TV_SetInterlaceMode(bEnable);
}

//CarDV...
#if 0
void ____CCIR_Function____(){ruturn;} //dummy
#endif

#if defined(ALL_FW)
//------------------------------------------------------------------------------
//  Function    : MMPS_CCIR_SetDisplayBuf
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPS_CCIR_SetDisplayBuf(MMP_ULONG ulBaseAddr)
{
    return MMPD_CCIR_SetDisplayBufAddr(ulBaseAddr);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_CCIR_SetDisplayRefresh
//  Description : 
//------------------------------------------------------------------------------
/** @brief Enable display refresh

The function check if CCIR output is enabled. If not, CCIR output will be
enabled. Otherwise, do nothing.

@return It reports the status of the operation.
*/
MMP_ERR MMPS_CCIR_SetDisplayRefresh(void)
{
    return MMPD_CCIR_SetDisplayRefresh();
}

//------------------------------------------------------------------------------
//  Function    : MMPS_CCIR_DisableDisplay
//  Description : 
//------------------------------------------------------------------------------
/** @brief Disable CCIR display refresh

The function disables the CCIR display.

@return It reports the status of the operation.
*/
MMP_ERR MMPS_CCIR_DisableDisplay(void)
{
    return MMPD_CCIR_DisableDisplay();
}
#endif
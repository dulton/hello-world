//==============================================================================
//
//  File        : mmpd_display.c
//  Description : Ritian Display Control driver function, including LCD/TV/Win
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

/**
*  @file mmpd_display.c
*  @brief The Display control functions
*  @author Penguin Torng
*  @version 1.0
*/

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "mmpd_display.h"
#include "mmpf_display.h"
#include "mmpf_ccir.h"

/** @addtogroup MMPD_Display
 *  @{
 */

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

#if 0
void ____Output_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_SetNoneOutput
//  Description : 
//------------------------------------------------------------------------------
/** 
@brief The function set the needed parameter for No LCD case
@param[in] controller  the display controller
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Display_SetNoneOutput(MMP_DISPLAY_CONTROLLER controller)
{
	return MMPF_Display_SetNoneOutput(controller);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_SetPLCDOutput
//  Description : 
//------------------------------------------------------------------------------
/** 
@brief The function set the needed parameter for Parallel LCD 
@param[in] controller  the display controller
@param[in] cssel       panel use which chip select pin (CS1 or CS2)
@param[in] lcdattribute struct with panel related information
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Display_SetPLCDOutput(	MMP_DISPLAY_CONTROLLER 	controller, 
									MMP_LCD_CS_SEL 			cssel, 
									MMP_LCD_ATTR 			*pLcdAttr)
{
	return MMPF_Display_SetPLCDOutput(controller, cssel, pLcdAttr);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_SetRGBLCDOutput
//  Description : 
//------------------------------------------------------------------------------
/** 
@brief The function set the needed parameter for RGB-LCD (LCD without RAM) 
@param[in] controller  the display controller
@param[in] lcdattribute struct with panel related information
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Display_SetRGBLCDOutput(MMP_DISPLAY_CONTROLLER controller, 
									 MMP_LCD_ATTR 			*pLcdAttr, 
									 MMP_DISPLAY_RGB_IF 	ubRgbIf)
{
	return MMPF_Display_SetRGBLCDOutput(controller, pLcdAttr, ubRgbIf);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_DisableRGBOutput
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Display_DisableRGBOutput(MMP_DISPLAY_CONTROLLER controller, MMP_DISPLAY_RGB_IF ubRgbIf)
{
    return MMPF_Display_DisableRGBOutput(controller, ubRgbIf);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_GetOutputDev
//  Description : 
//------------------------------------------------------------------------------
/** 
@brief     Return which kind of panel connected to the give controller 
@param[in] controller   The controller you want to query
@param[out] output      Hardware type of the controller is using
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Display_GetOutputDev(MMP_DISPLAY_CONTROLLER controller, MMP_DISPLAY_DEV_TYPE *pOutput)
{
	return MMPF_Display_GetOutputDev(controller, pOutput);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_GetWidthHeight
//  Description : 
//------------------------------------------------------------------------------
/** 
@brief     Return which the panel dimension 
@param[in] controller   The controller you want to query
@param[out] usWidth     Width of this panel
@param[out] usHeight    Height of this panel
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Display_GetWidthHeight(MMP_DISPLAY_CONTROLLER controller, MMP_USHORT *usWidth, MMP_USHORT *usHeight)
{
	return MMPF_Display_GetWidthHeight(controller, usWidth, usHeight);  
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_WaitDisplayRefresh
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Display_WaitDisplayRefresh(MMP_DISPLAY_CONTROLLER controller)
{
	return MMPF_Display_WaitDisplayRefresh(controller);
}

MMP_ERR MMPD_Display_WaitRotateDMA(MMP_USHORT usIBC_PIPEID)
{
    return MMPF_Display_WaitRotateDMA(usIBC_PIPEID);
}
//------------------------------------------------------------------------------
//  Function    : MMPD_Display_SetDisplayRefresh
//  Description : 
//------------------------------------------------------------------------------
/** @brief Refresh the display device

The function refreshes LCD or TV for the frame data output by programming LCD controller register. The
refresh is activated only when necessary. The function can be used for dual panels. It returns the status
about the refresh.
@param[in] controller  the display controller
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Display_SetDisplayRefresh(MMP_DISPLAY_CONTROLLER controller)
{
	return MMPF_Display_SetDisplayRefresh(controller);
}


MMP_ERR MMPD_Display_ResetRotateDMAIdx(void)
{
    return MMPF_Display_ResetRotateDMAIdx();
}
#if 0
void ____IndexCommand_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPD_LCD_SendIndex
//  Description :
//------------------------------------------------------------------------------
/** @brief  Send specified index to LCD panel

The function sends the specified index to one LCD panel by programming LCD controller register. The
function can be used for dual panels.

@param[in] usIndex the index for the LCD to be sent
@return It reports the status of the operation.
*/
MMP_ERR MMPD_LCD_SendIndex(MMP_ULONG ulIndex)
{
	return MMPF_LCD_SendIndex(ulIndex);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_LCD_SendCommand
//  Description :
//------------------------------------------------------------------------------
/** @brief Send the specified command to LCD panel

The function sends the specified command to one LCD panel by programming LCD controller register.
The function can be used for dual panels.

@param[in] ulCommand the command for the LCD to be sent
@return It reports the status of the operation.
*/
MMP_ERR MMPD_LCD_SendCommand(MMP_ULONG ulCommand)
{
	return MMPF_LCD_SendCommand(ulCommand);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_LCD_SendIndexCommand
//  Description :
//------------------------------------------------------------------------------
/** @brief The function sends a index and a command to LCD panel

The function sends a index and a command to LCD panel by programming LCD controller registers.
The function can be used for the auto transfer of LCD index/command sent before outputting frame
data to LCD panel. The function can be used for dual panels.

@param[in] ulIndex
@param[in] ulCommand
@return It reports the status of the operation.
*/
MMP_ERR MMPD_LCD_SendIndexCommand(MMP_ULONG ulIndex, MMP_ULONG ulCommand)
{
	return MMPF_LCD_SendIndexCommand(ulIndex, ulCommand);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_LCD_GetIndexCommand
//  Description :
//------------------------------------------------------------------------------
/** @brief The function get the command back from LCD panel according to the index sent

The function return the command back from the LCD panel by programming LCD controller registers.
@param[in] ulIndex
@param[out] ulCommand
@param[in] ulReadCount
@return It reports the status of the operation.
*/
MMP_ERR MMPD_LCD_GetIndexCommand(MMP_ULONG ulIndex, MMP_ULONG *ulCommand, MMP_ULONG ulReadCount)
{
	return MMPF_LCD_GetIndexCommand(ulIndex, ulCommand, ulReadCount);
}

#if 0
void ____RGB_Panel_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPD_RGBLCD_WaitFrame
//  Description : 
//------------------------------------------------------------------------------
/** @brief The function get the command back from LCD panel according to the index sent

The function return the command back from the LCD panel by programming LCD controller registers.
@param[in] ubFrameCount number of frame waiting.
@return It reports the status of the operation.
*/
MMP_ERR MMPD_RGBLCD_WaitFrame(MMP_UBYTE ubFrameCount, MMP_DISPLAY_RGB_IF ubRgbIf)
{
	return MMPF_RGBLCD_WaitFrame(ubFrameCount, ubRgbIf);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_RGBLCD_SetPartialPosition
//  Description : 
//------------------------------------------------------------------------------
/** @brief The function get the command back from LCD panel according to the index sent

The function return the command back from the LCD panel by programming LCD controller registers.
@param[in] rect rectangle information such as left, top, width, height.
@return It reports the status of the operation.
*/
MMP_ERR MMPD_RGBLCD_SetPartialPosition(MMP_DISPLAY_RECT *pRect, MMP_DISPLAY_RGB_IF ubRgbIf)
{
	return MMPF_RGBLCD_SetPartialPosition(pRect, ubRgbIf);
}

#if 0
void ____Window_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_GetWinAttributes
//  Description :
//------------------------------------------------------------------------------
/** @brief The function gets current window attributes

The function gets current window attributes from the specified window. The function can be used
for dual panels.

@param[in] winID the window ID
@param[out] winattribute the attribute buffer
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Display_GetWinAttributes(MMP_DISPLAY_WIN_ID 	winID,
                                      MMP_DISPLAY_WIN_ATTR 	*pWinAttr)
{
	return MMPF_Display_GetWinAttributes(winID, pWinAttr);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_SetWinAttributes
//  Description :
//------------------------------------------------------------------------------
/** @brief Set window attributes to the specified window

The function sets window attributes for the specified window. The attribute information is given in the
structure of MMPD_LCD_WinAttribute. The attributes for the window are saved into an internal buffer.
All of the attributes can be used to set the LCD control registers. The function can be used for dual
panels.

@param[in] winID the window ID
@param[in] winattribute the attribute buffer
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Display_SetWinAttributes(MMP_DISPLAY_WIN_ID 	winID,
                                      MMP_DISPLAY_WIN_ATTR 	*pWinAttr)
{
	return MMPF_Display_SetWinAttributes(winID, pWinAttr);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_ClearWindowBuf
//  Description :
//------------------------------------------------------------------------------
/** @brief The function clear a given window with specific color

The function clears the buffer with the input color. The window attribute is given by the internal
configuration structure. This information includes the buffer starting address and total buffer length
calculated from the start and end location. The function can be used for dual panels.

@param[in] winattribute the attribute buffer
@param[in] rect rectangle information such as left, top, width, height.
@param[in] ulClearColor the clear color. If window color attribute is YUV422. then the low-word (16-bits LSB)
		ulClearColor will be translated as the same YCb and YCr.If window color attribute is YUV422, then
		ulClearColor represent 0(31-24), Cr(23-16) Cb(15-8) and Y(7-0).
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Display_ClearWindowBuf(MMP_DISPLAY_WIN_ATTR 	*pWinAttr,
                                    MMP_GRAPHICS_RECT 		*pRect, 
                                    MMP_ULONG 				ulClearColor)
{
    return MMPF_Display_ClearWindowBuf(pWinAttr, pRect, ulClearColor);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_GetWinBindController
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Display_GetWinBindController(MMP_DISPLAY_WIN_ID 		winID,
                                          MMP_DISPLAY_CONTROLLER	*pController)
{
	return MMPF_Display_GetWinBindController(winID, pController);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_BindWinToController
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_Display_BindWinToController(MMP_DISPLAY_WIN_ID 	winID,
                                         MMP_DISPLAY_CONTROLLER	controller)
{
	return MMPF_Display_BindWinToController(winID, controller);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_BindBufToWin
//  Description :
//------------------------------------------------------------------------------
/** @brief The function set attribute of a window

The function validates and binds buffer attributes such as buffer width, height and color depth to the
specified window by setting them into LCD controller registers. The successful binding means that the
buffer attributes can be applied to the specified window. The system maintains internal window
structure. The function can be used for dual panels.

@param[in] pBufAttr the struct of buffer attribute which contains W, H, line offeset, color depth, and buffer address.
@param[in] winID for the window ID
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Display_BindBufToWin(MMP_GRAPHICS_BUF_ATTR *pBufAttr, MMP_DISPLAY_WIN_ID winID)
{
	return MMPF_Display_BindBufToWin(pBufAttr, winID);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_UpdateWinAddr
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
MMP_ERR MMPD_Display_UpdateWinAddr( MMP_DISPLAY_WIN_ID  winID, 
                                    MMP_ULONG           ulBaseAddr, 
                                    MMP_ULONG           ulBaseUAddr, 
                                    MMP_ULONG           ulBaseVAddr)
{
	return MMPF_Display_UpdateWinAddr(winID, ulBaseAddr, ulBaseUAddr, ulBaseVAddr);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_SetWinToDisplay
//  Description :
//------------------------------------------------------------------------------
/** @brief The function set how a window will be show on the panel

The function validates and binds the window display attributes for display device . For example, the
display dimension, display location and display rotation type can be set from the specified display
attributes. The function can be used for dual panels.

@param[in] winID the window ID
@param[in] dispAttr the display attributes
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Display_SetWinToDisplay(MMP_DISPLAY_WIN_ID 	winID,
                                     MMP_DISPLAY_DISP_ATTR 	*pDispAttr)
{
	return MMPF_Display_SetWinToDisplay(winID, pDispAttr);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_SetWinDuplicate
//  Description : 
//------------------------------------------------------------------------------
/** @brief The function do duplication of a window

The function only scale the display buffer with the same aspect ratio
@param[in] winID the window ID for specific display buffer
@param[in] width scaling factor for width. Available input: 0:MMP_DISPLAY_DUPLICATE_1X, 1:MMP_DISPLAY_DUPLICATE_2X, MMP_DISPLAY_DUPLICATE_4X.
@param[in] height scaling factor for width. Available input: 0:MMP_DISPLAY_DUPLICATE_1X, 1:MMP_DISPLAY_DUPLICATE_2X, MMP_DISPLAY_DUPLICATE_4X.
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Display_SetWinDuplicate(MMP_DISPLAY_WIN_ID 	winID, 
                					 MMP_DISPLAY_DUPLICATE 	h_ratio, 
                					 MMP_DISPLAY_DUPLICATE 	v_ratio)
{
	return MMPF_Display_SetWinDuplicate(winID, h_ratio, v_ratio);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_SetWinScaling
//  Description :
//------------------------------------------------------------------------------
/** @brief The function configures the scaling and the grab operations

The function configures the scaling and the grab operations by programming LCD
controller registers.

@param[in] winID The window ID
@param[in] fitrange The fit range parameters.
@param[out] grabctl The grab control parameters. Assign NULL to ignore the output.
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Display_SetWinScaling( MMP_DISPLAY_WIN_ID 	winID,
                                    MMP_SCAL_FIT_RANGE 	*fitrange,
                                    MMP_SCAL_GRAB_CTRL 	*grabctl)
{
	return MMPF_Display_SetWinScaling(winID, fitrange, grabctl);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_SetWinScaleAndOffset
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_Display_SetWinScaleAndOffset(  MMP_DISPLAY_WIN_ID 	winID,
                                            MMP_SCAL_FIT_RANGE 	*fitrange,
                                            MMP_SCAL_GRAB_CTRL 	*grabctl,
                                            MMP_USHORT          usWinOffsetX,
                                            MMP_USHORT          usWinOffsetY)
{
	return MMPF_Display_SetWinScaleAndOffset(winID, fitrange, grabctl, usWinOffsetX, usWinOffsetY);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_SetWinPriority
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
MMP_ERR MMPD_Display_SetWinPriority(MMP_DISPLAY_WIN_ID prio1,
                                    MMP_DISPLAY_WIN_ID prio2,
                                    MMP_DISPLAY_WIN_ID prio3,
                                    MMP_DISPLAY_WIN_ID prio4)
{
	return MMPF_Display_SetWinPriority(prio1, prio2, prio3, prio4);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_SetWinActive
//  Description :
//------------------------------------------------------------------------------
/** @brief The function activates or deactivates the window

The function activates or deactivates the window from the input parameter, winID. The function can be
used for dual panels.

@param[in] winID the window ID
@param[in] bActive activate or de-activate the window
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Display_SetWinActive(MMP_DISPLAY_WIN_ID winID, MMP_BOOL bActive)
{
    MMPF_Display_SetWinActive(winID, bActive);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_GetWinActive
//  Description : 
//------------------------------------------------------------------------------
/** @brief The function get the window active state

The function return the activa or non-active status of the the window from the input parameter, winID. 
The function can be used for dual panels.

@param[in] winID the window ID
@param[out] bActive active status of specific window ID
@return It reports the active status of the winID
*/
MMP_ERR MMPD_Display_GetWinActive(MMP_DISPLAY_WIN_ID winID, MMP_BOOL *bActive)
{
    return MMPF_Display_GetWinActive(winID, bActive);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_SetWinTransparent
//  Description :
//------------------------------------------------------------------------------
/** @brief The function activates transparent operation of window

The function activates transparent operation with its transparent color from the input parameter or
deactivates ¡§transparency¡¨ operation which sets transparent color to be 0. The previous transparent
color value is returned

@param[in] winID the window ID
@param[in] bTranspActive activate or deactivate transparent operation
@param[in] ulTranspColor transparent color to set
@return It reports previous transparency color setting
*/
MMP_ULONG MMPD_Display_SetWinTransparent(MMP_DISPLAY_WIN_ID winID,
                                         MMP_BOOL 			bTranspActive, 
                                         MMP_ULONG 			ulTranspColor)
{
	return MMPF_Display_SetWinTransparent(winID, bTranspActive, ulTranspColor);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_SetWinSemiTransparent
//  Description :
//------------------------------------------------------------------------------
/** @brief The function activates transparent operation by its transparent color with weight

The function activates transparent operation by its transparent color with weight from the input parameter

@param[in] winID the window ID
@param[in] bSemiTranspActive activate or deactivate semi-transparent operation
@param[in] semifunc the operation of AVERAGE, AND, OR, INVERSE
@param[in] usSemiWeight the weight
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Display_SetWinSemiTransparent(	MMP_DISPLAY_WIN_ID 		winID, 
											MMP_BOOL 				bSemiTpActive, 
											MMP_DISPLAY_SEMITP_FUNC semifunc, 
											MMP_USHORT 				usSemiWeight)
{
	return MMPF_Display_SetWinSemiTransparent(winID, bSemiTpActive, semifunc, usSemiWeight);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_GetWinSemiTransparent
//  Description :
//------------------------------------------------------------------------------
/** @brief: Get the weighting of semi-transparent

@param[in] winID the window ID
@param[in] bSemiTranspActive activate or deactivate semi-transparent operation
@param[in] usSemiWeight the weight
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Display_GetWinSemiTransparent(MMP_DISPLAY_WIN_ID winID, MMP_USHORT* usSemiWeight)
{
	return MMPF_Display_GetWinSemiTransparent(winID, usSemiWeight);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_SetWinAlphaBlending
//  Description : Function to set alpha blending parameters
//------------------------------------------------------------------------------
/** @brief Function to set alpha blending registers
@param[in] winID            the window ID.
@param[in] bAlphaEn         enable or disable the alpha blending
@param[in] alphaformat RGBA or ARGB format
@param[in] icon_alpha_depth alpha bit-depth for ICON window 
@retval MMPD_DISPLAY_SUCCESS Success.
*/
MMP_ERR MMPD_Display_SetAlphaBlending(	MMP_DISPLAY_WIN_ID      winID, 
                                        MMP_BYTE                bAlphaEn, 
										MMP_DISPLAY_ALPHA_FMT  	alphaformat)
{
	return MMPF_Display_SetAlphaBlending(winID, bAlphaEn, alphaformat);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_SetAlphaWeight
//  Description : Function to set alpha blending LUT
//------------------------------------------------------------------------------
/** @brief Function to set alpha blending LUT
@param[in] *ubAlphaWeight alpha blending LUT 
@retval MMPD_DISPLAY_SUCCESS Success.
*/
MMP_ERR MMPD_Display_SetAlphaWeight(MMP_UBYTE* ubAlphaWeight)
{
	return MMPF_Display_SetAlphaWeight(ubAlphaWeight);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_LoadWinPalette
//  Description : 
//------------------------------------------------------------------------------
/** @brief The function activates transparent operation by its transparent color with weight

The function activates transparent operation by its transparent color with weight from the input parameter

@param[in] winID the window ID
@param[in] ubPalette palette data
@param[in] usEntry define the number of palette
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Display_LoadWinPalette(MMP_DISPLAY_WIN_ID winID, MMP_UBYTE *pubPalette, MMP_USHORT usEntry)
{
    return MMPF_Display_LoadWinPalette(winID, pubPalette, usEntry);
}


MMP_ERR MMPD_Display_AdjustWinColorMatrix(MMP_DISPLAY_WIN_ID 	winID, 
                                        MMP_DISPLAY_WIN_COLRMTX *DspyColrMtx, MMP_BOOL bApplyColMtx) 
{
   return MMPF_Display_AdjustWinColorMatrix(winID, DspyColrMtx, bApplyColMtx);
}

#if 1
//------------------------------------------------------------------------------
//  Function    : MMPD_Display_GetRGBDotClk
//  Description :
//------------------------------------------------------------------------------
/** @brief: Get the RGB panel Dot clock
*/
MMP_ERR MMPD_Display_GetRGBDotClk(MMP_ULONG *ulRGBDotClk)
{
	return MMPF_Display_GetRGBDotClk(ulRGBDotClk);
}
#endif
/// @}

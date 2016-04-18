//==============================================================================
//
//  File        : mmpd_display.h
//  Description : INCLUDE File for the Host Display Control driver function
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPD_DISPLAY_H_
#define _MMPD_DISPLAY_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmpd_graphics.h"
#include "mmpd_ccir.h"
#include "mmp_scal_inc.h"
#include "mmp_display_inc.h"

/** @addtogroup MMPD_Display
 *  @{
 */

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

// Output Function
MMP_ERR MMPD_Display_SetNoneOutput(MMP_DISPLAY_CONTROLLER controller);
MMP_ERR MMPD_Display_SetPLCDOutput(	MMP_DISPLAY_CONTROLLER 	controller, 
									MMP_LCD_CS_SEL 			cssel, 
									MMP_LCD_ATTR 			*pLcdAttr);
MMP_ERR MMPD_Display_SetRGBLCDOutput(MMP_DISPLAY_CONTROLLER controller, 
									 MMP_LCD_ATTR 			*pLcdAttr, 
									 MMP_DISPLAY_RGB_IF 	ubRgbIf);
MMP_ERR MMPD_Display_DisableRGBOutput(MMP_DISPLAY_CONTROLLER controller, MMP_DISPLAY_RGB_IF ubRgbIf);
MMP_ERR MMPD_Display_GetOutputDev(MMP_DISPLAY_CONTROLLER controller, MMP_DISPLAY_DEV_TYPE *pOutput);
MMP_ERR MMPD_Display_GetWidthHeight(MMP_DISPLAY_CONTROLLER controller, MMP_USHORT *usWidth, MMP_USHORT *usHeight);
MMP_ERR MMPD_Display_WaitDisplayRefresh(MMP_DISPLAY_CONTROLLER controller);
MMP_ERR MMPD_Display_WaitRotateDMA(MMP_USHORT usIBC_PIPEID);
MMP_ERR MMPD_Display_SetDisplayRefresh(MMP_DISPLAY_CONTROLLER controller);
MMP_ERR MMPD_Display_ResetRotateDMAIdx(void);
MMP_ERR MMPD_Display_GetRGBDotClk(MMP_ULONG *ulRGBDotClk);
// LCD Function
MMP_ERR MMPD_LCD_SendCommand(MMP_ULONG ulCommand);
MMP_ERR MMPD_LCD_SendIndex(MMP_ULONG ulIndex);
MMP_ERR MMPD_LCD_SendIndexCommand(MMP_ULONG ulIndex, MMP_ULONG ulCommand);
MMP_ERR MMPD_LCD_GetIndexCommand(MMP_ULONG ulIndex, MMP_ULONG *ulCommand, MMP_ULONG ulReadCount);

// RGB Function
MMP_ERR MMPD_RGBLCD_WaitFrame(MMP_UBYTE ubFrameCount, MMP_DISPLAY_RGB_IF ubRgbIf);
MMP_ERR MMPD_RGBLCD_SetPartialPosition(MMP_DISPLAY_RECT *pRect, MMP_DISPLAY_RGB_IF ubRgbIf);

// Window Function
MMP_ERR MMPD_Display_GetWinAttributes(MMP_DISPLAY_WIN_ID 	winID,
                                      MMP_DISPLAY_WIN_ATTR 	*pWinAttr);
MMP_ERR MMPD_Display_SetWinAttributes(MMP_DISPLAY_WIN_ID 	winID,
                                      MMP_DISPLAY_WIN_ATTR 	*pWinAttr);
MMP_ERR MMPD_Display_ClearWindowBuf(MMP_DISPLAY_WIN_ATTR 	*pWinAttr,
                                    MMP_GRAPHICS_RECT 		*pRect, 
                                    MMP_ULONG 				ulClearColor);
MMP_ERR MMPD_Display_GetWinBindController(MMP_DISPLAY_WIN_ID 		winID,
                                          MMP_DISPLAY_CONTROLLER	*pController);
MMP_ERR MMPD_Display_BindWinToController(MMP_DISPLAY_WIN_ID 	winID,
                                         MMP_DISPLAY_CONTROLLER controller);
MMP_ERR MMPD_Display_BindBufToWin(MMP_GRAPHICS_BUF_ATTR *pBufAttr, MMP_DISPLAY_WIN_ID winID);
MMP_ERR MMPD_Display_UpdateWinAddr( MMP_DISPLAY_WIN_ID  winID, 
                                    MMP_ULONG           ulBaseAddr, 
                                    MMP_ULONG           ulBaseUAddr, 
                                    MMP_ULONG           ulBaseVAddr);
MMP_ERR MMPD_Display_SetWinToDisplay(MMP_DISPLAY_WIN_ID 	winID,
                                     MMP_DISPLAY_DISP_ATTR 	*pDispAttr);
MMP_ERR MMPD_Display_SetWinDuplicate(MMP_DISPLAY_WIN_ID 	winID, 
                					 MMP_DISPLAY_DUPLICATE 	h_ratio, 
                					 MMP_DISPLAY_DUPLICATE 	v_ratio);
MMP_ERR MMPD_Display_SetWinScaling( MMP_DISPLAY_WIN_ID 	winID, 
                                    MMP_SCAL_FIT_RANGE 	*fitrange,
                                    MMP_SCAL_GRAB_CTRL 	*grabctl);
MMP_ERR MMPD_Display_SetWinScaleAndOffset(  MMP_DISPLAY_WIN_ID 	winID,
                                            MMP_SCAL_FIT_RANGE 	*fitrange,
                                            MMP_SCAL_GRAB_CTRL 	*grabctl,
                                            MMP_USHORT          usWinOffsetX,
                                            MMP_USHORT          usWinOffsetY);
MMP_ERR MMPD_Display_SetWinPriority(MMP_DISPLAY_WIN_ID prio1,
                                    MMP_DISPLAY_WIN_ID prio2,
                                    MMP_DISPLAY_WIN_ID prio3,
                                    MMP_DISPLAY_WIN_ID prio4);
MMP_ERR MMPD_Display_SetWinActive(MMP_DISPLAY_WIN_ID winID, MMP_BOOL bActive);
MMP_ERR MMPD_Display_GetWinActive(MMP_DISPLAY_WIN_ID winID, MMP_BOOL *bActive);
MMP_ULONG MMPD_Display_SetWinTransparent(MMP_DISPLAY_WIN_ID winID,
                                         MMP_BOOL 			bTranspActive, 
                                         MMP_ULONG 			ulTranspColor);
MMP_ERR MMPD_Display_SetWinSemiTransparent(	MMP_DISPLAY_WIN_ID 		winID, 
											MMP_BOOL 				bSemiTpActive, 
											MMP_DISPLAY_SEMITP_FUNC	semifunc, 
											MMP_USHORT 				usSemiWeight);
MMP_ERR MMPD_Display_GetWinSemiTransparent(MMP_DISPLAY_WIN_ID winID, MMP_USHORT* usSemiWeight);
MMP_ERR MMPD_Display_SetAlphaBlending(	MMP_DISPLAY_WIN_ID      winID, 
                                        MMP_BYTE                bAlphaEn, 
										MMP_DISPLAY_ALPHA_FMT  	alphaformat);
MMP_ERR MMPD_Display_SetAlphaWeight(MMP_UBYTE* ubAlphaWeight);
MMP_ERR MMPD_Display_LoadWinPalette(    MMP_DISPLAY_WIN_ID winID, 
                                        MMP_UBYTE *pubPalette, 
                                        MMP_USHORT usEntry);
MMP_ERR MMPD_Display_AdjustWinColorMatrix(  MMP_DISPLAY_WIN_ID 	winID, 
                                            MMP_DISPLAY_WIN_COLRMTX *DspyColrMtx, 
                                            MMP_BOOL bApplyColMtx);

// HDMI Function
MMP_ERR MMPD_Display_SetHDMIOutput(MMP_DISPLAY_CONTROLLER controller,MMP_HDMI_ATTR *hdmiattribute,
                                   MMP_DISPLAY_RGB_IF rgbIf);
MMP_ERR MMPD_HDMI_DisableHDMI(void);
MMP_ERR MMPD_HDMI_InitDigital(MMP_HDMI_ATTR *hdmiAttr);
MMP_ERR MMPD_HDMI_InitAnalog(MMP_HDMI_ATTR *hdmiAttr);
MMP_ERR MMPD_HDMI_Initialize(MMP_HDMI_ATTR *hdmiAttr, MMP_DISPLAY_RGB_IF rgbIf);
MMP_ERR MMPD_HDMI_ParseEDID(MMP_UBYTE *pEDID, MMP_ULONG *pSpportFormat, MMP_ULONG *pEDIDType);

// TV Function
MMP_ERR MMPD_Display_SetTVOutput(MMP_DISPLAY_CONTROLLER controller, MMP_TV_ATTR *tvattribute,
                                 MMP_DISPLAY_RGB_IF rgbIf);
MMP_ERR MMPD_TV_Initialize(MMP_BOOL bInit,MMP_TV_TYPE output_panel);
MMP_ERR MMPD_TV_EnableDisplay(MMP_UBYTE enable);
MMP_ERR MMPD_TV_SetInterface(MMP_TV_ATTR *tvAttr);
MMP_ERR MMPD_TV_EncRegSet(MMP_ULONG addr, MMP_ULONG data);
MMP_ERR MMPD_TV_EncRegGet(MMP_ULONG addr, MMP_ULONG *data);
MMP_ERR MMPD_TV_SetBrightLevel(MMP_UBYTE ubLevel);
MMP_ERR MMPD_TV_SetContrast(MMP_USHORT usLevel);
MMP_ERR MMPD_TV_SetSaturation(MMP_UBYTE ubUlevel, MMP_UBYTE ubVlevel);
MMP_ERR MMPD_TV_SetInterlaceMode(MMP_BOOL bEnable);
MMP_ERR MMPD_TV_GetBrightLevel(MMP_UBYTE *pubLevel);
MMP_ERR MMPD_TV_GetContrast(MMP_USHORT *pusLevel);
MMP_ERR MMPD_TV_GetSaturation(MMP_UBYTE *pubUlevel, MMP_UBYTE *pubVlevel);
MMP_ERR MMPD_TV_SetGamma0(MMP_USHORT ubLevel);
MMP_ERR MMPD_TV_SetGamma1(MMP_USHORT ubLevel);
MMP_ERR MMPD_TV_SetGamma2(MMP_USHORT ubLevel);
MMP_ERR MMPD_TV_SetGamma3(MMP_USHORT ubLevel);
MMP_ERR MMPD_TV_SetGamma4(MMP_USHORT ubLevel);
MMP_ERR MMPD_TV_SetGamma5(MMP_USHORT ubLevel);
MMP_ERR MMPD_TV_SetGamma6(MMP_USHORT ubLevel);
MMP_ERR MMPD_TV_SetGamma7(MMP_USHORT ubLevel);
MMP_ERR MMPD_TV_SetGamma8(MMP_USHORT ubLevel);
MMP_ERR MMPD_TV_GetGamma(MMP_DISPLAY_TV_GAMMA *DspyGamma);
/// @}
#endif // _MMPD_DISPLAY_H_

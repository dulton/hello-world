//==============================================================================
//
//  File        : mmps_display.h
//  Description : INCLUDE File for the Host Display Control driver function
//  Author      : Alan Wu
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPS_DISPLAY_H_
#define _MMPS_DISPLAY_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "mmp_display_inc.h"

//==============================================================================
//
//                              STRUCTURE
//
//==============================================================================

typedef struct _MMPS_DISPLAY_CONFIG {
	MMP_LCD_ATTR 		        mainlcd;		///< Main LCD attribute
	MMP_LCD_ATTR  		        sublcd;			///< Sub LCD attribute
	MMP_TV_ATTR   				ntsctv;			///< NTSC TV attribute	
	MMP_TV_ATTR   				paltv;			///< NTSC PAL attribute	
	MMP_HDMI_ATTR 				hdmi;       	///< HDMI attribute
	MMP_CCIR_ATTR 				ccir;       	///< CCIR attribute	
	MMP_DISPLAY_OUTPUT_SEL   	OutputPanel;
} MMPS_DISPLAY_CONFIG;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

// Output Function
MMPS_DISPLAY_CONFIG* MMPS_Display_GetConfig(void);
MMP_ERR MMPS_Display_SetOutputPanel(MMP_DISPLAY_CONTROLLER controller, MMP_DISPLAY_OUTPUT_SEL dispOutSel);
MMP_ERR MMPS_Display_SetOutputPanelMediaError(MMP_DISPLAY_CONTROLLER controller, MMP_DISPLAY_OUTPUT_SEL dispOutSel);
MMP_ERR MMPS_Display_GetOutputPanel(MMP_DISPLAY_CONTROLLER controller, MMP_DISPLAY_OUTPUT_SEL *pDispOutSel);
MMP_ERR MMPS_Display_WaitDisplayRefresh(MMP_DISPLAY_CONTROLLER controller);
MMP_ERR MMPS_Display_SetDisplayRefresh(MMP_DISPLAY_CONTROLLER controller);
MMP_ERR MMPS_Display_GetRGBDotClk(MMP_ULONG *ulRGBDotClk);
// Window function
MMP_ERR MMPS_Display_ReadWinBuffer(MMP_DISPLAY_WIN_ID winID, MMP_USHORT *usMemPtr);								 			   
MMP_ERR MMPS_Display_WriteWinBuffer(MMP_DISPLAY_WIN_ID winID, MMP_USHORT *usMemPtr);
MMP_ERR MMPS_Display_SetWinPriority(MMP_DISPLAY_WIN_ID prio1, MMP_DISPLAY_WIN_ID prio2, 
                                    MMP_DISPLAY_WIN_ID prio3, MMP_DISPLAY_WIN_ID prio4);
MMP_ERR MMPS_Display_ClearWinBuffer(MMP_DISPLAY_WIN_ID winID, 
									MMP_ULONG 	ulClearColor,
									MMP_USHORT 	usWidth, 	MMP_USHORT usHeight, 
									MMP_USHORT 	usStartX, 	MMP_SHORT usStartY);
MMP_ERR MMPS_Display_SetWinSemiTransparent(MMP_DISPLAY_WIN_ID 		winID, 
										   MMP_BOOL 				bSemiTpActive, 
										   MMP_DISPLAY_SEMITP_FUNC 	semifunc, 
										   MMP_USHORT 				usSemiWeight);
MMP_ERR MMPS_Display_GetWinSemiTransparent(MMP_DISPLAY_WIN_ID winID, MMP_USHORT* usSemiWeight);
MMP_ERR MMPS_Display_SetWinActive(MMP_DISPLAY_WIN_ID winID, MMP_BOOL bActive);
MMP_ERR MMPS_Display_SetWinDuplicate(MMP_DISPLAY_WIN_ID 	winID, 
                					 MMP_DISPLAY_DUPLICATE 	h_ratio, 
                					 MMP_DISPLAY_DUPLICATE 	v_ratio);
MMP_ULONG MMPS_Display_SetWinTransparent(MMP_DISPLAY_WIN_ID winID,
                                         MMP_BOOL 			bTranspActive, 
                                         MMP_ULONG 			ulTranspColor);
MMP_ERR MMPS_Display_SetAlphaBlending(MMP_DISPLAY_WIN_ID      	winID, 
                                      MMP_BYTE                	bAlphaEn, 
									  MMP_DISPLAY_ALPHA_FMT  	alphaformat);
MMP_ERR MMPS_Display_SetWindowAttrToDisp(MMP_DISPLAY_WIN_ID  	winID, 
                                    	 MMP_DISPLAY_WIN_ATTR 	winAttr,
                                    	 MMP_DISPLAY_DISP_ATTR	dispAttr);
MMP_ERR MMPS_Display_LoadWinPalette(MMP_DISPLAY_WIN_ID winID, MMP_UBYTE *ubPalette, MMP_USHORT usEntry);
MMP_ERR MMPS_Display_UpdateWinAddr(MMP_DISPLAY_WIN_ID 	winID, 
                            	   MMP_ULONG 			ulBaseAddr, 
                            	   MMP_ULONG 			ulBaseUAddr, 
                            	   MMP_ULONG 			ulBaseVAddr);
MMP_ERR MMPS_Display_SetWinScaleAndOffset(MMP_DISPLAY_WIN_ID 	winID,
                                          MMP_UBYTE		        sFitMode,
                                          MMP_ULONG             ulInputW,
                                          MMP_ULONG             ulInputH,
                                          MMP_ULONG             ulOutputW,
                                          MMP_ULONG             ulOutputH,
                                          MMP_USHORT            usWinOffsetX,
                                          MMP_USHORT            usWinOffsetY);
MMP_ERR  MMPS_Display_AdjustWinColorMatrix( MMP_DISPLAY_WIN_ID 	winID, 
                                            MMP_DISPLAY_WIN_COLRMTX *DspyColrMtx, 
                                            MMP_BOOL bApplyColMtx);

// Ratio Function
MMP_ERR MMPS_Display_GetPixelXYRatio(MMP_DISPLAY_OUTPUT_SEL PanelType, 
                                     MMP_ULONG      *ulXRatio,  MMP_ULONG *ulYRatio, 
                                     MMP_ULONG      *ulWidth,   MMP_ULONG *ulHeight, 
                                     MMP_ULONG64    *ulPixelWidth,
                                     MMP_BOOL       *bRotateVerticalLCD);
MMP_ERR MMPS_Display_CheckPixelRatio(MMP_DISPLAY_CONTROLLER controller, MMP_BOOL *bSquarePixel, MMP_ULONG *ulPixelWidth);

MMP_ERR MMPS_Display_AdjustScaleInSize( MMP_DISPLAY_CONTROLLER      controller,
                                        MMP_ULONG                   InputWidth,
                                        MMP_ULONG                   InputHeight,
                                        MMP_ULONG                   RequiredWidth,
                                        MMP_ULONG                   RequiredHeight,
                                        MMP_ULONG                   *SuggestWidth,
                                        MMP_ULONG                   *SuggestHeight); 

MMP_ERR MMPS_Display_PureAdjustScaleInSize(	MMP_BOOL                bScaleUpAvail,
                                        	MMP_ULONG               InputWidth,
                                        	MMP_ULONG               InputHeight,
                                        	MMP_ULONG               RequiredWidth,
                                        	MMP_ULONG               RequiredHeight,
                                        	MMP_ULONG               *SuggestWidth,
                                        	MMP_ULONG               *SuggestHeight);

// TV Function
MMP_ERR MMPS_TV_SetBrightLevel(MMP_UBYTE ubLevel);
MMP_ERR MMPS_TV_SetContrast(MMP_USHORT usLevel);
MMP_ERR MMPS_TV_SetSaturation(MMP_UBYTE ubUlevel, MMP_UBYTE ubVlevel);
MMP_ERR MMPS_TV_SetInterlaceMode(MMP_BOOL bEnable);

// CCIR Function
MMP_ERR MMPS_CCIR_SetDisplayBuf(MMP_ULONG ulBaseAddr);
MMP_ERR MMPS_CCIR_SetDisplayRefresh(void);
MMP_ERR MMPS_CCIR_DisableDisplay(void);

#endif //_MMPS_DISPLAY_H_

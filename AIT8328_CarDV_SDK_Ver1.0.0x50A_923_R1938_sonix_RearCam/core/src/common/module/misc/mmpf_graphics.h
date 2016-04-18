//==============================================================================
//
//  File        : mmpf_graphics.h
//  Description : INCLUDE File for the Firmware Graphic Driver.
//  Author      : Ben Lu
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPF_GRAPHICS_H_
#define _MMPF_GRAPHICS_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_graphics_inc.h"

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

MMP_ERR MMPF_Graphics_Initialize(void);
void    MMPF_Graphics_ISR(void);

MMP_ERR MMPF_Graphics_SetDelayType(MMP_GRAPHICS_DELAY_TYPE ubType);
MMP_ERR MMPF_Graphics_SetPixDelay(MMP_UBYTE ubPixDelayN, MMP_UBYTE ubPixDelayM);
MMP_ERR MMPF_Graphics_SetLineDelay(MMP_USHORT usLineDelay);
MMP_ERR MMPF_Graphics_SetMCIByteCnt(MMP_USHORT usByteCnt);

MMP_BOOL MMPF_Graphics_IsScaleIdle(void);
MMP_ERR MMPF_Graphics_SetScaleAttr(MMP_GRAPHICS_BUF_ATTR  	*bufAttr,
								   MMP_GRAPHICS_RECT      	*rect, 
								   MMP_USHORT              	usUpscale);
MMP_ERR MMPF_Graphics_ScaleStart(MMP_GRAPHICS_BUF_ATTR 		*src,
                                 MMP_GRAPHICS_BUF_ATTR 		*dst,
                                 GraphicScaleCallBackFunc 	*GraCallBack,
                                 GraphicScaleCallBackFunc 	*GraSelfTriggerCB);
                                
MMP_ERR MMPF_Graphics_TriggerAndWaitDone(MMP_GRAPHICS_COLORDEPTH colordepth);

MMP_ERR MMPF_Graphics_CopyImageBuftoBuf(MMP_GRAPHICS_BUF_ATTR 	*srcBufAttr, 
                        				MMP_GRAPHICS_RECT 		*srcrect, 
                        				MMP_GRAPHICS_BUF_ATTR	*dstBufAttr, 
										MMP_USHORT 				usDstStartx, 
										MMP_USHORT 				usDstStarty, 
										MMP_GRAPHICS_ROP 		ropcode, 
										MMP_UBYTE 				ubTranspActive, 
										MMP_ULONG 				ulKeyColor);

MMP_ERR MMPF_Graphics_RotateImageBuftoBuf(MMP_GRAPHICS_BUF_ATTR 	*srcBufAttr,
						                  MMP_GRAPHICS_RECT         *srcrect, 
						                  MMP_GRAPHICS_BUF_ATTR 	*dstBufAttr, 
						                  MMP_USHORT                usDstStartx, 
						                  MMP_USHORT                usDstStarty, 
						                  MMP_GRAPHICS_ROP         	ropcode,
						                  MMP_GRAPHICS_ROTATE_TYPE 	rotate, 
						                  MMP_UBYTE 				ubTranspActive, 
						                  MMP_ULONG 				ulKeyColor);

MMP_ERR MMPF_Graphics_DrawRectToBuf(MMP_GRAPHICS_DRAWRECT_ATTR 	*drawAttr, 
                                    MMP_GRAPHICS_RECT          	*rect,
                                    MMP_ULONG                   *pOldColor);

MMP_ULONG MMPF_Graphics_SetKeyColor(MMP_GRAPHICS_KEYCOLOR keyColorSel, MMP_ULONG ulColor);

#endif // _MMPF_GRAPHICS_H_

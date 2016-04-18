//==============================================================================
//
//  File        : mmpd_graphics.h
//  Description : INCLUDE File for the Host Graphics Driver.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

/**
 *  @file mmpd_graphics.h
 *  @brief The header File for the Host Graphic control functions
 *  @author Penguin Torng
 *  @version 1.0
 */

#ifndef _MMPD_GRAPHICS_H_
#define _MMPD_GRAPHICS_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_graphics_inc.h"

/** @addtogroup MMPD_Graphics
 *  @{
 */

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

MMP_ERR MMPD_Graphics_SetDelayType(MMP_GRAPHICS_DELAY_TYPE ubType);
MMP_ERR MMPD_Graphics_SetPixDelay(MMP_UBYTE ubPixDelayN, MMP_UBYTE ubPixDelayM);
MMP_ERR MMPD_Graphics_SetLineDelay(MMP_USHORT usLineDelay);

MMP_ERR MMPD_Graphics_SetScaleAttr(MMP_GRAPHICS_BUF_ATTR 	*bufAttr,
						           MMP_GRAPHICS_RECT     	*rect, 
						           MMP_USHORT             	usUpscale);

MMP_ERR MMPD_Graphics_CopyImageBuftoBuf(MMP_GRAPHICS_BUF_ATTR  *srcBufAttr,
                                        MMP_GRAPHICS_RECT      *srcrect, 
                                        MMP_GRAPHICS_BUF_ATTR  *dstBufAttr, 
						                MMP_USHORT              usDstStartx, 
						                MMP_USHORT              usDstStarty, 
						                MMP_GRAPHICS_ROP       	ropcode,
						                MMP_UBYTE               ubTranspActive);
						                
MMP_ERR MMPD_Graphics_RotateImageBuftoBuf(MMP_GRAPHICS_BUF_ATTR    *srcBufAttr,
						                  MMP_GRAPHICS_RECT        *srcrect, 
						                  MMP_GRAPHICS_BUF_ATTR    *dstBufAttr, 
						                  MMP_USHORT                usDstStartx, 
						                  MMP_USHORT                usDstStarty, 
						                  MMP_GRAPHICS_ROP         	ropcode,
						                  MMP_GRAPHICS_ROTATE_TYPE 	rotate,
						                  MMP_UBYTE                 ubTranspActive);

MMP_ERR MMPD_Graphics_DrawRectToBuf(MMP_GRAPHICS_DRAWRECT_ATTR 	*drawAttr,
						            MMP_GRAPHICS_RECT          	*rect,
						            MMP_ULONG               	*pOldColor);

MMP_ULONG MMPD_Graphics_SetKeyColor(MMP_GRAPHICS_KEYCOLOR keyColorSel, MMP_ULONG ulColor);

/// @}

#endif // _MMPD_GRAPHICS_H_


//==============================================================================
//
//  File        : mmpd_graphics.c
//  Description : Ritian Graphics(2D) Engine Control driver function
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "mmpd_system.h"
#include "mmpd_graphics.h"
#include "mmpf_graphics.h"

/** @addtogroup MMPD_Graphics
 *  @{
 */

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

//------------------------------------------------------------------------------
//  Function    : MMPD_Graphics_SetDelayType
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Graphics_SetDelayType(MMP_GRAPHICS_DELAY_TYPE ubType)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_TRUE);

    MMPF_Graphics_SetDelayType(ubType);

    MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Graphics_SetPixDelay
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Graphics_SetPixDelay(MMP_UBYTE ubPixDelayN, MMP_UBYTE ubPixDelayM)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_TRUE);

    MMPF_Graphics_SetPixDelay(ubPixDelayN, ubPixDelayM);

    MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Graphics_SetLineDelay
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Graphics_SetLineDelay(MMP_USHORT usLineDelay)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_TRUE);

    MMPF_Graphics_SetLineDelay(usLineDelay);

    MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Graphics_SetScaleAttr
//  Description : 
//------------------------------------------------------------------------------
/** @brief   The function copy image date from one frame buffer to another frame buffer

  The function copy image date from one buffer to another buffer
  @param[in] bufAttr the source scale buffer
  @param[in] usSrcx the start x position of source image
  @param[in] usSrcy the start y position of source image
  @param[in] usWidth the width of image in frame buffer
  @param[in] usHeight the height of image in frame buffer
  @param[in] usUpscale the up scale ratio
  @param[in] srcsel input from graphic SCALE FIFO or directly from FB
  @return It reports the status of the operation.
*/
MMP_ERR MMPD_Graphics_SetScaleAttr(MMP_GRAPHICS_BUF_ATTR 	*pBufAttr,                       
						           MMP_GRAPHICS_RECT    	*pRect, 
						           MMP_USHORT        		usUpscale)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_TRUE);

    MMPF_Graphics_SetScaleAttr(pBufAttr, pRect, usUpscale);

    MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_FALSE);

    return MMP_ERR_NONE;
}

#if 0
void ____BLT_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPD_Graphics_CopyImageBuftoBuf
//  Description :
//------------------------------------------------------------------------------
/** @brief   The function copy image data from one frame buffer to another frame buffer

  The function copy image date from one buffer to another buffer
  @param[in] srcbufattribute the source graphic buffer
  @param[in] srcrect the rect of source image
  @param[in] dstbufattribute the destination graphic buffer
  @param[in] usDstStartx the x position of destination frame buffer
  @param[in] usDstStarty the y position of destination frame buffer
  @param[in] ropcode the Rop Code for graphics operation
  @return It reports the status of the operation.
*/
MMP_ERR MMPD_Graphics_CopyImageBuftoBuf(MMP_GRAPHICS_BUF_ATTR  *pSrcBufAttr,
                                        MMP_GRAPHICS_RECT      *pSrcrect, 
                                        MMP_GRAPHICS_BUF_ATTR  *pDstBufAttr, 
						                MMP_USHORT 				usDstStartx, 
						                MMP_USHORT 				usDstStarty, 
						                MMP_GRAPHICS_ROP 		ropcode,
						                MMP_UBYTE 				ubTranspActive)

{
    MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_TRUE);

    MMPF_Graphics_CopyImageBuftoBuf(pSrcBufAttr,
                                    pSrcrect,
                                    pDstBufAttr,
                                    usDstStartx,
                                    usDstStarty,
                                    ropcode,
                                    ubTranspActive,
                                    0/*TBD*/);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Graphics_RotateImageBuftoBuf
//  Description :
//------------------------------------------------------------------------------
/** @brief The function ...

  The function copy and rotate image data from memory to memory (frame buffer)
  
  @param[in] srcbufattribute the source graphic buffer
  @param[in] srcrect the rect of source image
  @param[in] dstbufattribute the destination graphic buffer
  @param[in] usDstStartx the x position of destination frame buffer
  @param[in] usDstStarty the y position of destination frame buffer
  @param[in] ropcode the Rop Code for graphics operation
  @param[in] rotate the rotate direction
  @return It reports the status of the operation.
*/
MMP_ERR MMPD_Graphics_RotateImageBuftoBuf(MMP_GRAPHICS_BUF_ATTR    	*pSrcBufAttr,
						                  MMP_GRAPHICS_RECT       	*pSrcrect, 
						                  MMP_GRAPHICS_BUF_ATTR    	*pDstBufAttr, 
						                  MMP_USHORT                usDstStartx, 
						                  MMP_USHORT                usDstStarty, 
						                  MMP_GRAPHICS_ROP         	ropcode,
						                  MMP_GRAPHICS_ROTATE_TYPE  rotate,
						                  MMP_UBYTE                 ubTranspActive)

{
    MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_TRUE);

    MMPF_Graphics_RotateImageBuftoBuf(pSrcBufAttr,
                                      pSrcrect,
                                      pDstBufAttr,
                                      usDstStartx,
                                      usDstStarty,
                                      ropcode,
                                      rotate,
                                      ubTranspActive,
                                      0);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Graphics_DrawRectToBuf
//  Description :
//------------------------------------------------------------------------------
/** @brief The function draws a rectangle to the buffer with its buffer ID

The function draws a rectangle to the buffer with its buffer ID (usBufID). The second parameter gives
the rectangle information. The third parameter gives the type of rectangle fill. The fourth parameter
gives the ROP (Raster Operation) code for BitBlt operation. The system supports 16 raster operations.

  @param[in] bufAttr the graphic buffer
  @param[in] rect the rectangle information
  @return It reports the status of the operation.
*/
MMP_ERR MMPD_Graphics_DrawRectToBuf(MMP_GRAPHICS_DRAWRECT_ATTR 	*pDrawAttr,
						            MMP_GRAPHICS_RECT          	*pRect,
						            MMP_ULONG                   *pOldColor)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_TRUE);
    
    MMPF_Graphics_DrawRectToBuf(pDrawAttr, pRect, pOldColor);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Graphics_SetKeyColor
//  Description :
//------------------------------------------------------------------------------
/** @brief The function sets foreground or background color for Graphics

The function sets foreground or background color for Graphics drawing by programming Graphics controller
registers

  @param[in] keycolor the key color type (foreground or background)
  @param[in] ulColor the color to be set
  @return It returns the previous color
*/
MMP_ULONG MMPD_Graphics_SetKeyColor(MMP_GRAPHICS_KEYCOLOR keyColorSel, MMP_ULONG ulColor)
{
    MMP_ULONG ulOldColor;

	MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_TRUE);
    
    ulOldColor = MMPF_Graphics_SetKeyColor(keyColorSel, ulColor);
    
 	MMPD_System_EnableClock(MMPD_SYS_CLK_GRA, MMP_FALSE);

    return ulOldColor;
}

/// @}


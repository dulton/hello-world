//==============================================================================
//
//  File        : mmpd_ccir.c
//  Description : Ritian CCIR Control driver function
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

/**
*  @file mmpd_ccir.c
*  @brief The CCIR control functions
*  @author Penguin Torng
*  @version 1.0
*/

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "mmpf_display.h"
#include "mmpf_ccir.h"
#include "mmp_reg_ccir.h"		// CarDV
#include "mmp_reg_gbl.h"		// CarDV
#include "mmpd_system.h"		// CarDV
#include "AHC_Config_SDK.h"

/** @addtogroup MMPD_Display
 *  @{
 */

// CarDV +++
#if defined(CCIR656_ENABLE_TEST_PATTERN)
typedef struct _RGBTerm {
  MMP_UBYTE R;
  MMP_UBYTE G;
  MMP_UBYTE B;
} RGBTerms;

typedef struct _YUVTerm {
  MMP_UBYTE Y;
  MMP_UBYTE U;
  MMP_UBYTE V;
} YUVTerms;


typedef union {
   RGBTerms rgb;
   YUVTerms yuv;
} Pixel;

#define COLORBAR        0
#define WITH_BORDER     1
#define PATTERN         COLORBAR

#define CCIR656_720X480 0
#define CCIR656_720X576 1

#define CCIR_27MHZ      0
#define CCIR_54MHZ      1
#define CCIR_RESOL CCIR656_720X480
//#define CCIR_RESOL CCIR656_720X576
#if (CCIR_RESOL == CCIR656_720X480)
#define CCIRClOCK       CCIR_27MHZ
#define CCIR656_WIDTH   720
#define CCIR656_HEIGHT  480
#define H_ACT_PIXEL     720
#define H_TOTAL_BLK     138
#define V_ACT_LINE      480
#define F1_TOP_BLK      26
#define F1_BOT_BLK      19
#endif
#endif
// CarDV ---

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

//------------------------------------------------------------------------------
//  Function    : MMPD_Display_SetCCIROutput
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Display_SetCCIROutput( MMP_DISPLAY_CONTROLLER  controller,
                                    MMP_CCIR_ATTR           ccirAttr)
{
    #if defined(ALL_FW)
    MMPF_Display_SetOutputDev(controller, MMP_DISPLAY_CCIR);
    return MMPF_CCIR_ConfigCCIR(&ccirAttr);
    #else
    return MMP_ERR_NONE;
    #endif
}

//------------------------------------------------------------------------------
//  Function    : MMPD_CCIR_SetDisplayBuf
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_CCIR_SetDisplayBufAddr(MMP_ULONG ulBaseAddr)
{
    return MMPF_CCIR_SetPedBufferAddr(ulBaseAddr);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_CCIR_SetDisplayRefresh
//  Description : 
//------------------------------------------------------------------------------
/** @brief Enable display refresh

The function check if CCIR output is enabled. If not, CCIR output will be
enabled. Otherwise, do nothing.

@return It reports the status of the operation.
*/
MMP_ERR MMPD_CCIR_SetDisplayRefresh(void)
{
    #if defined(ALL_FW)
	return MMPF_CCIR_SetRefresh();
    #else
    return MMP_ERR_NONE;
    #endif
}

//------------------------------------------------------------------------------
//  Function    : MMPD_CCIR_DisableDisplay
//  Description : 
//------------------------------------------------------------------------------
/** @brief Disable CCIR display refresh

The function disables the CCIR display.

@return It reports the status of the operation.
*/
MMP_ERR MMPD_CCIR_DisableDisplay(void)
{
    #if defined(ALL_FW)
	return MMPF_CCIR_StopRefresh();
    #else
    return MMP_ERR_NONE;
    #endif
}

// CarDV +++
#if defined(CCIR656_ENABLE_TEST_PATTERN)
MMP_ULONG DisplayDrawHorizontalSegment1(MMP_ULONG pDrawAddr, MMP_DISPLAY_COLORMODE DrawFormat, Pixel PixelVal, MMP_USHORT PixelCnt)
{
    MMP_ULONG i;

    RTNA_DBG_Str(0, FG_BLUE("### DisplayDrawHorizontalSegment1 -\r\n"));
    
    switch(DrawFormat) {
    case MMP_DISPLAY_COLOR_RGB888:
        for(i=0;i<PixelCnt;i++) {
            //B
            *((MMP_UBYTE*)(pDrawAddr)) = PixelVal.rgb.B;
            pDrawAddr+=1;
            //G
            *((MMP_UBYTE*)(pDrawAddr)) = PixelVal.rgb.G;
            pDrawAddr+=1;
            //R
            *((MMP_UBYTE*)(pDrawAddr)) = PixelVal.rgb.R;
            pDrawAddr+=1;
        }
        break;
    case MMP_DISPLAY_COLOR_YUV422:
        for(i=0;i<PixelCnt/2;i++) {
            //U
            *((MMP_UBYTE*)(pDrawAddr)) = PixelVal.yuv.U;
            pDrawAddr+=1;
            //Y
            *((MMP_UBYTE*)(pDrawAddr)) = PixelVal.yuv.Y;
            pDrawAddr+=1;
            //V
            *((MMP_UBYTE*)(pDrawAddr)) = PixelVal.yuv.V;
            pDrawAddr+=1;
            //Y
            *((MMP_UBYTE*)(pDrawAddr)) = PixelVal.yuv.Y;
            pDrawAddr+=1;
        }
        break;
      }
    return pDrawAddr;
}

MMP_ULONG DisplayDrawVerticalSegment1(MMP_ULONG pDrawAddr, MMP_DISPLAY_COLORMODE DrawFormat, Pixel PixelVal, 
                                      MMP_USHORT drawheight, MMP_USHORT drawidth, MMP_USHORT lineoffset)
{
    MMP_ULONG i,j;

    RTNA_DBG_Str(0, FG_BLUE("### DisplayDrawVerticalSegment1 -\r\n"));

    switch(DrawFormat) {
    case MMP_DISPLAY_COLOR_YUV422:
        for(i=0;i<drawheight;i++) {
            for(j=0;j<drawidth;j++) {
            //U
            *((MMP_UBYTE*)(pDrawAddr)) = PixelVal.yuv.U;
            pDrawAddr+=1;
            //Y
            *((MMP_UBYTE*)(pDrawAddr)) = PixelVal.yuv.Y;
            pDrawAddr+=1;
            //V
            *((MMP_UBYTE*)(pDrawAddr)) = PixelVal.yuv.V;
            pDrawAddr+=1;
            //Y
            *((MMP_UBYTE*)(pDrawAddr)) = PixelVal.yuv.Y;
             pDrawAddr+=1;           
            }
            pDrawAddr += (lineoffset-4*drawidth);
        }
        break;
      }
    return pDrawAddr;
}

MMP_ULONG GetDrawAddrOffset(MMP_ULONG pixel, MMP_ULONG line, MMP_ULONG width, MMP_ULONG height,MMP_DISPLAY_COLORMODE DrawFormat)
{
    if(DrawFormat == MMP_DISPLAY_COLOR_YUV422)
    {
        return  (pixel/2)*4 + width*line*2;
    }
    else if (DrawFormat == MMP_DISPLAY_COLOR_RGB888)
    {
        return width*height*3;
    }    
    return 0;
}

void DisplayDrawPic1(MMP_ULONG DrawAddr, MMP_ULONG width, MMP_ULONG height, MMP_ULONG DrawType)
{
    //MMP_ULONG              y;
    Pixel                  pixelval;
    MMP_ULONG              lineoffset; 
    MMP_ULONG              curDrawAddr;
    //MMP_ULONG              lastLineDrawAddr;
    MMP_ULONG              offset;

    RTNA_DBG_Str(0, FG_BLUE("### DisplayDrawPic1 -\r\n"));

    lineoffset = width*2;
    #if (PATTERN == WITH_BORDER)
    /*draw pattern for test*/
    curDrawAddr =   DrawAddr;
    pixelval.yuv.Y=29;     
    pixelval.yuv.U=235;
    pixelval.yuv.V=107;    
    DisplayDrawHorizontalSegment1(curDrawAddr, MMP_DISPLAY_COLOR_YUV422,pixelval,width*7);

    offset=GetDrawAddrOffset(0,7,width,height,MMP_DISPLAY_COLOR_YUV422);
    pixelval.yuv.Y=29; 
    pixelval.yuv.U=235;
    pixelval.yuv.V=107;
    DisplayDrawVerticalSegment1(curDrawAddr+offset,MMP_DISPLAY_COLOR_YUV422,pixelval,height-8,5,lineoffset);

    offset=GetDrawAddrOffset(width-10,8,width,height,MMP_DISPLAY_COLOR_YUV422);
    pixelval.yuv.Y=235; 
    pixelval.yuv.U=128;
    pixelval.yuv.V=128;
    DisplayDrawVerticalSegment1(curDrawAddr+offset,MMP_DISPLAY_COLOR_YUV422,pixelval,height-8,5,lineoffset);

    pixelval.yuv.Y=235; 
    pixelval.yuv.U=128;
    pixelval.yuv.V=128;
    offset=GetDrawAddrOffset(0,height-7,width,height,MMP_DISPLAY_COLOR_YUV422);
    DisplayDrawHorizontalSegment1(DrawAddr+offset,MMP_DISPLAY_COLOR_YUV422,pixelval,width*7);               

    RTNA_DBG_Str0("offset=\r\n");
    RTNA_DBG_Long0(offset);
    #endif
    
    #if (PATTERN == COLORBAR)
    pixelval.yuv.Y=235;     
    pixelval.yuv.U=0;
    pixelval.yuv.V=0;    
    offset=GetDrawAddrOffset(0,0,width,height,MMP_DISPLAY_COLOR_YUV422);
    DisplayDrawVerticalSegment1(DrawAddr+offset,MMP_DISPLAY_COLOR_YUV422,pixelval,height,width/16,lineoffset);
    pixelval.yuv.Y=210; 
    pixelval.yuv.U=16;
    pixelval.yuv.V=146;
    offset=GetDrawAddrOffset(width/8*1,0,width,height,MMP_DISPLAY_COLOR_YUV422);
    DisplayDrawVerticalSegment1(DrawAddr+offset,MMP_DISPLAY_COLOR_YUV422,pixelval,height,width/16,lineoffset);
    
    pixelval.yuv.Y=170;     
    pixelval.yuv.U=166;
    pixelval.yuv.V=16;    
    offset=GetDrawAddrOffset((width/8)*2,0,width,height,MMP_DISPLAY_COLOR_YUV422);
    DisplayDrawVerticalSegment1(DrawAddr+offset,MMP_DISPLAY_COLOR_YUV422,pixelval,height,width/16,lineoffset);
    pixelval.yuv.Y=145; 
    pixelval.yuv.U=54;
    pixelval.yuv.V=34;
    offset=GetDrawAddrOffset((width/8)*3,0,width,height,MMP_DISPLAY_COLOR_YUV422);
    DisplayDrawVerticalSegment1(DrawAddr+offset,MMP_DISPLAY_COLOR_YUV422,pixelval,height,width/16,lineoffset);
    
    pixelval.yuv.Y=107;     
    pixelval.yuv.U=202;
    pixelval.yuv.V=221;    
    offset=GetDrawAddrOffset((width/8)*4,0,width,height,MMP_DISPLAY_COLOR_YUV422);
    DisplayDrawVerticalSegment1(DrawAddr+offset,MMP_DISPLAY_COLOR_YUV422,pixelval,height,width/16,lineoffset);
    pixelval.yuv.Y=82; 
    pixelval.yuv.U=90;
    pixelval.yuv.V=240;
    offset=GetDrawAddrOffset((width/8)*5,0,width,height,MMP_DISPLAY_COLOR_YUV422);
    DisplayDrawVerticalSegment1(DrawAddr+offset,MMP_DISPLAY_COLOR_YUV422,pixelval,height,width/16,lineoffset);
    
    pixelval.yuv.Y=41;     
    pixelval.yuv.U=240;
    pixelval.yuv.V=110;    
    offset=GetDrawAddrOffset((width/8)*6,0,width,height,MMP_DISPLAY_COLOR_YUV422);
    DisplayDrawVerticalSegment1(DrawAddr+offset,MMP_DISPLAY_COLOR_YUV422,pixelval,height,width/16,lineoffset);
    pixelval.yuv.Y=0; 
    pixelval.yuv.U=128;
    pixelval.yuv.V=128;
    offset=GetDrawAddrOffset((width/8)*7,0,width,height,MMP_DISPLAY_COLOR_YUV422);
    DisplayDrawVerticalSegment1(DrawAddr+offset,MMP_DISPLAY_COLOR_YUV422,pixelval,height,width/16,lineoffset);
    
    #endif
}

void DrawPattern(void) 
{
#if 0
    MMP_ULONG addr=AHC_CCIR656_PREVIEW_ADDR;//0x4000000;

    RTNA_DBG_Str(0, FG_BLUE("### DrawPattern -\r\n"));

    DisplayDrawPic1(addr,CCIR656_WIDTH,CCIR656_HEIGHT,1);
#endif    
}


MMP_ERR MMPD_CCIR_SetCCIR656Output(void)
{

    AITPS_CCIR  pCCIR   = AITC_BASE_CCIR;
    AITPS_GBL   pGBL    = AITC_BASE_GBL;
    MMP_ULONG   ulgrpSrc ;
    //AITPS_AIC   pAIC = AITC_BASE_AIC;

    RTNA_DBG_Str(0, FG_BLUE("### MMPD_CCIR_SetCCIR656Output\r\n"));
    
    #if 0
    RTNA_AIC_Open(pAIC, AIC_SRC_CCIR,   ccir_isr_a,
                    AIC_INT_TO_IRQ | AIC_SRCTYPE_HIGH_LEVEL_SENSITIVE    | 3);   
    #endif 
    #if 0
    pCCIR->CCIR_PED_MODE_SELECT                         =  CCIR_PED_OUT_BT656 ; 
    pCCIR->CCIR_PED_CPU_INT_EN                          =  CCIR_PED_V_FRONT_PORCH_END_EN ; 
    pCCIR->CCIR_PED_CPU_INT_EN                          |= CCIR_PED_FIELD1_BOT_BLANK_END_EN ; 
    pCCIR->CCIR_PED_CPU_INT_EN                          |= CCIR_PED_LINE_CNT_REACH_EN ; 
    pCCIR->CCIR_PED_CPU_INT_EN                          |= CCIR_PED_LINE_CNT1_REACH_EN ; 
    #endif
    
    //LCD_intial(0,0x0F,0xEF); //polarity
    //MEMSET((void *)AHC_CCIR656_PREVIEW_ADDR, 0x0, 0xA8C00);
    //DrawPattern();
    pGBL->GBL_MISC_IO_CFG |= GBL_CCIR_PAD_EN;
    MMPD_System_EnableClock(MMPD_SYS_CLK_CCIR, MMP_TRUE);    
    MMPD_System_GetGroupFreq(CLK_GRP_GBL, &ulgrpSrc);    
    pCCIR->CCIR_PED_MODE_SELECT                         =  CCIR_PED_OUT_BT656 ; 
    #if (CCIRClOCK == CCIR_27MHZ)
    pCCIR->CCIR_PED_PIXEL_CLK_DIV                         = ulgrpSrc/27000-1; //TBD
    #endif    
    #if (CCIRClOCK == CCIR_54MHZ)
    pCCIR->CCIR_PED_PIXEL_CLK_DIV                         = ulgrpSrc/54000-1; //TBD
    #endif
    
    pCCIR->CCIR_PED_DATA_FMT                            = CCIR_OUTPUT_FMT_YUV422_8BIT;
    pCCIR->CCIR_PED_DATA_ORDER                          = CCIR_YUV422_8BIT_UYVY;
    
    pCCIR->CCIR_PED_IBC_BUF_ST_ADDR                     = 0x4000000;//AHC_CCIR656_PREVIEW_ADDR;//0x4000000;
    pCCIR->CCIR_PED_IBC_BUF_ED_ADDR                     = /*AHC_CCIR656_PREVIEW_ADDR+CCIR656_WIDTH*/0x4000000*CCIR656_HEIGHT*2-128;//0x4000000+CCIR656_WIDTH*CCIR656_HEIGHT*2-128; 
   
    #if 1
    pCCIR->CCIR_PED_H_ACT_PIXL_CNT                      = H_ACT_PIXEL;
    pCCIR->CCIR_PED_H_TOTAL_BLANK                       = H_TOTAL_BLK;  
    pCCIR->CCIR_PED_V_ACT_LINE_CNT                      = V_ACT_LINE;
    pCCIR->CCIR_PED_BT656_1120_FILD1_TOP_V_BLANK        = F1_TOP_BLK;
    pCCIR->CCIR_PED_BT656_1120_FILD1_BOT_V_BLANK        = F1_BOT_BLK;
    #endif
    //pCCIR->CCIR_PED_USER_DEF_EAV_SAV_EN                 = 1;
    
    #define CCIR656_PATTERN 0
    #if CCIR656_PATTERN
    pCCIR->CCIR_PED_H_ACT_PIXL_CNT                      = 128;
    pCCIR->CCIR_PED_H_TOTAL_BLANK                       = 16;  //pixel 
    //pCCIR->CCIR_PED_H_BACK_PORCH                        = 2;   //not used for BT656 
    //pCCIR->CCIR_PED_H_FRONT_PORCH                       = 2;   //not used for BT656
    pCCIR->CCIR_PED_V_ACT_LINE_CNT                      = 96;
    pCCIR->CCIR_PED_BT565_1120_FILD1_TOP_V_BLANK        = 8;
    pCCIR->CCIR_PED_BT565_1120_FILD1_BOT_V_BLANK        = 12;
    
    pCCIR->CCIR_PED_BT656_1120_FILD1_TOP_EAV        = 0xB6;
    pCCIR->CCIR_PED_BT656_1120_FILD1_TOP_SAV        = 0xAB;
    pCCIR->CCIR_PED_BT656_1120_FILD1_ACT_EAV        = 0x9D;
    pCCIR->CCIR_PED_BT656_1120_FILD1_ACT_SAV        = 0x80;
    pCCIR->CCIR_PED_BT656_1120_FILD1_BOT_EAV        = 0xB6;
    pCCIR->CCIR_PED_BT656_1120_FILD1_BOT_SAV        = 0xAB;
    #endif
    #if 1
    pCCIR->CCIR_PED_MODE_SELECT                         |=  CCIR_PED_GEN_TIME_SIG_START | CCIR_SW_MODE_TRIG ; 
    #else
    pCCIR->CCIR_PED_MODE_SELECT                         |=  CCIR_PED_GEN_TIME_SIG_START | CCIR_HW_MODE_TRIG | CCIR_FRM_SYNC_ENC ; 
    #endif
    return MMP_ERR_NONE;
}
#endif
// CarDV ---

/// @}

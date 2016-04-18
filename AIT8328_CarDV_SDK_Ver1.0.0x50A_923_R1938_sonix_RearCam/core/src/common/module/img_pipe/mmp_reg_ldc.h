//==============================================================================
//
//  File        : mmp_register_ldc.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Eroy Yang
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_REG_LDC_H_
#define _MMP_REG_LDC_H_

#include "mmp_register.h"

#if (CHIP == MCR_V2)
//------------------------------
// LDC Structure (0x8000 4000)
//------------------------------
typedef struct _AITS_LDC 
{
    AIT_REG_B   LDC_CTL;                                                // 0x00 
        /*-DEFINE-----------------------------------------------------*/
        #define LDC_BYPASS  	            0x01
        #define LDC_SHARE_FB_EN             0x02
        #define LDC_DRAM_MODE_EN            0x04
        #define LDC_SRAM_MODE_EN            0x08
        #define LDC_ENALBE                  0x80
        /*------------------------------------------------------------*/ 
    AIT_REG_B   LDC_INPUT_PATH;                                         // 0x01
        /*-DEFINE-----------------------------------------------------*/
        #define LDC_GRA_INPUT  	            0x01
        #define LDC_ISP_INPUT               0x00
        /*------------------------------------------------------------*/    
    AIT_REG_B   LDC_INPUT_FIFO_RST;                                     // 0x02
        /*-DEFINE-----------------------------------------------------*/
        #define LDC_INPUT_FIFO_RESET  	    0x01
        /*------------------------------------------------------------*/
    AIT_REG_B   LDC_BUSY_CHK;                                           // 0x03
        /*-DEFINE-----------------------------------------------------*/
        #define LDC_BUSY_CHK_EN  	        0x01
        /*------------------------------------------------------------*/
    AIT_REG_B   LDC_INT_CPU_EN;                                         // 0x04
    AIT_REG_B   LDC_INT_HOST_EN;                                        // 0x05
    AIT_REG_B   LDC_INT_CPU_SR;                                         // 0x06
    AIT_REG_B   LDC_INT_HOST_SR;                                        // 0x07
        /*-DEFINE-----------------------------------------------------*/
        #define LDC_INT_INPUT_FRM_END  	    0x01
        #define LDC_INT_INPUT_FRM_ST 	    0x02
        #define LDC_INT_OUTPUT_FRM_END  	0x04
        #define LDC_INT_TBL_UPDATE_RDY      0x08
        #define LDC_INT_INPUT_BUF_FULL 	    0x10
        #define LDC_INT_INPUT_BUF_OVF  	    0x20
        /*------------------------------------------------------------*/
    AIT_REG_W   LDC_IN_INIT_X_OFST;                                     // 0x08
    AIT_REG_W   LDC_IN_INIT_Y_OFST;                                     // 0x0A
        /*-DEFINE-----------------------------------------------------*/
        #define LDC_IN_INIT_OFST_MASK  	    0x1FFF
        /*------------------------------------------------------------*/
    AIT_REG_W   LDC_SHARE_FB_DEEP;                                      // 0x0C
    AIT_REG_B   LDC_SHARE_FB_MODE;                                      // 0x0E
        /*-DEFINE-----------------------------------------------------*/
        #define LDC_SHARE_FB_720P_MODE      0x01
        #define LDC_EOL_RST_LOAD_FSM		0x02
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x0F;
    
    AIT_REG_W   LDC_IN_FRM_W;                                           // 0x10
    AIT_REG_W   LDC_IN_FRM_H;                                           // 0x12
    AIT_REG_B   LDC_OUT_CROP_EN;                                        // 0x14
        /*-DEFINE-----------------------------------------------------*/
        #define LDC_OUT_CROP_ENABLE         0x01
        /*------------------------------------------------------------*/     
    AIT_REG_B   LDC_OUT_LINE_BLANKING;                                  // 0x15
    AIT_REG_B                           _x16[0x2];
    AIT_REG_W   LDC_OUT_CROP_X_ST;                                      // 0x18
    AIT_REG_W   LDC_OUT_CROP_Y_ST;                                      // 0x1A
    AIT_REG_W   LDC_OUT_CROP_X_LEN;                                     // 0x1C
    AIT_REG_W   LDC_OUT_CROP_Y_LEN;                                     // 0x1E
    
    AIT_REG_B   LDC_IN_X_RATIO;                                         // 0x20
    AIT_REG_B   LDC_IN_Y_RATIO;                                         // 0x21
    AIT_REG_B                           _x22[0x2];
    AIT_REG_W   LDC_IN_X_ST;                                            // 0x24
    AIT_REG_W   LDC_IN_Y_ST;                                            // 0x26
    AIT_REG_B                           _x28[0x8];
    
    AIT_REG_B   LDC_X_DELTA_BASE;                                       // 0x30
    AIT_REG_B   LDC_Y_DELTA_BASE;                                       // 0x31
    AIT_REG_B   LDC_BICUBIC_INT_EN;                                     // 0x32
        /*-DEFINE-----------------------------------------------------*/
        #define LDC_BICUBIC_INT_ENABLE      0x01
        #define LDC_UTINITY_TBL_EN          0x02
        /*------------------------------------------------------------*/    
    AIT_REG_B   LDC_BOUND_EXT;                                          // 0x33
        /*-DEFINE-----------------------------------------------------*/
        #define LDC_BOUND_EXT_EN            0x01
        #define LDC_UV_OVF_VAL              0x02
        /*------------------------------------------------------------*/    
    AIT_REG_B                           _x34[0xC]; 
    
    AIT_REG_W   LDC_LUT_X[0x2A];                                        // 0x40~0x93
    AIT_REG_B                           _x94[0xC]; 

    AIT_REG_W   LDC_LUT_Y[0x20];                                        // 0xA0~0xDF
    
    AIT_REG_B   LDC_Y_SRC_OFST;                                         // 0xE0
    AIT_REG_B   LDC_Y_TAR_OFST;                                         // 0xE1
    AIT_REG_B   LDC_Y_CLIP_HIGH;                                        // 0xE2
    AIT_REG_B   LDC_Y_CLIP_LOW;                                         // 0xE3
    AIT_REG_B   LDC_U_SRC_OFST;                                         // 0xE4
    AIT_REG_B   LDC_U_TAR_OFST;                                         // 0xE5
    AIT_REG_B   LDC_U_CLIP_HIGH;                                        // 0xE6
    AIT_REG_B   LDC_U_CLIP_LOW;                                         // 0xE7
    AIT_REG_B   LDC_V_SRC_OFST;                                         // 0xE8
    AIT_REG_B   LDC_V_TAR_OFST;                                         // 0xE9
    AIT_REG_B   LDC_V_CLIP_HIGH;                                        // 0xEA
    AIT_REG_B   LDC_V_CLIP_LOW;                                         // 0xEB    
    AIT_REG_B                           _xEC[0x4];
    
    AIT_REG_B   LDC_TBL_WR_SEL;                                         // 0xF0  
        /*-DEFINE-----------------------------------------------------*/
        #define LDC_RW_000_127_DATA_MEMA    0x00
        #define LDC_RW_128_255_DATA_MEMA    0x01
        #define LDC_RW_256_335_DATA_MEMA    0x02
        #define LDC_RW_000_127_DATA_MEMB    0x04
        #define LDC_RW_128_255_DATA_MEMB    0x05
        #define LDC_RW_256_335_DATA_MEMB    0x06
        #define LDC_RW_000_127_DATA_MEMC    0x08
        #define LDC_RW_128_255_DATA_MEMC    0x09
        #define LDC_RW_256_335_DATA_MEMC    0x0A
        #define LDC_RW_000_127_DATA_MEMD    0x0C
        #define LDC_RW_128_255_DATA_MEMD    0x0D
        #define LDC_RW_256_335_DATA_MEMD    0x0E                
        /*------------------------------------------------------------*/ 
    AIT_REG_B   LDC_LUT_SRAM_RME;                                       // 0xF1
    AIT_REG_B   LDC_LUT_DMA_EN;                                         // 0xF2
        /*-DEFINE-----------------------------------------------------*/
        #define LDC_LUT_DMA_FRM_END         0x01
        #define LDC_LUT_DMA_NOW             0x02
        /*------------------------------------------------------------*/
    AIT_REG_B                           _xF3;    
    AIT_REG_D   LDC_LUT_DMA_ADDR;                                       // 0xF4        
    AIT_REG_W   LDC_PROG_FRM_X_ST;                                      // 0xF8
    AIT_REG_W   LDC_PROG_FRM_Y_ST;                                      // 0xFA
    AIT_REG_B                           _xFC[0x4];
                   
} AITS_LDC, *AITPS_LDC;

typedef struct _AITS_LDC_LUT 
{
    AIT_REG_D  DELTA_TBL[128]; // LDC_DELTA_X[31:16] LDC_DELTA_Y[15:0] 
} AITS_LDC_LUT, *AITPS_LDC_LUT;

#endif

#endif // _MMP_REG_LDC_H_
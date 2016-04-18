//==============================================================================
//
//  File        : mmp_register_ibc.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_REG_IBC_H_
#define _MMP_REG_IBC_H_

#include "mmp_register.h"

/** @addtogroup MMPH_reg
@{
*/

#if (CHIP == MCR_V2)

//------------------------------
// IBC Structure (0x8000 5600)
//------------------------------
typedef struct _AITS_IBCP 
{
    //For IBC Pipe0, Pipe1, Pipe2, Pipe3
	AIT_REG_D   IBC_ADDR_Y_ST;                                          // 0x20
    AIT_REG_D   IBC_ADDR_U_ST;                                          // 0x24
    AIT_REG_D   IBC_ADDR_V_ST;                                          // 0x28
    AIT_REG_B                           _x2C[0x4];

    AIT_REG_D   IBC_ADDR_Y_END;                                         // 0x30
    AIT_REG_D   IBC_ADDR_U_END;                                         // 0x34
    AIT_REG_D   IBC_ADDR_V_END;                                         // 0x38
    AIT_REG_B   IBC_422_OUTPUT_SEQ;                                     // 0x3C
        /*-DEFINE-----------------------------------------------------*/
        #define IBC_422_SEQ_UYVY        	0x00
        #define IBC_422_SEQ_VYUY        	0x01
        #define IBC_422_SEQ_YUYV        	0x02
        #define IBC_422_SEQ_YVYU        	0x03
        #define IBC_422_SEQ_MASK        	0x03
       /*------------------------------------------------------------*/
   	AIT_REG_B                           _x3D[0x3];
 
   	AIT_REG_B   IBC_SRC_SEL;                                            // 0x40
        /*-DEFINE-----------------------------------------------------*/
        #define IBC_SRC_RGB565           	0x00
        #define IBC_SRC_YUV422           	0x00
        #define IBC_SRC_YUV444_2_YUV422  	0x01 
        #define IBC_SRC_RGB888           	0x02 
        #define IBC_SRC_YUV444           	0x02 
        #define IBC_SRC_YUV444_2_YUV420     0x03
        #define IBC_SRC_FMT_MASK         	0xA3
        #define IBC_SRC_FMT_MASK1           0x03
        
        #define IBC_SRC_SEL_MASK        	0x0C
        #define IBC_SRC_SEL_ICO(_a)      	((_a << 2) & IBC_SRC_SEL_MASK)
        
        #define IBC_MIRROR_EN            	0x10
        #define IBC_420_STORE_CBR        	0x20
        #define IBC_LINEOFFSET_EN        	0x40
        #define IBC_INTERLACE_MODE_EN    	0x80
        /*------------------------------------------------------------*/
   	AIT_REG_B                           _x41[0x3];
    AIT_REG_B   IBC_BUF_CFG;										    // 0x44
        /*-DEFINE-----------------------------------------------------*/
        #define IBC_STORE_SING_FRAME    	0x01
        #define IBC_STORE_PIX_CONT      	0x02
        #define IBC_STORE_PIX_NCONT         0x00
        #define IBC_ICON_PAUSE_EN       	0x04
        #define IBC_STORE_EN            	0x08
        #define IBC_INT_STOP_STORE_EN   	0x10
        #define IBC_NV12_EN             	0x00
        #define IBC_NV21_EN             	0x20
        #define IBC_M420_EN             	0x40
        #define IBC_FMT_MASK            	0x60
        #define IBC_RING_BUF_EN         	0x80
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x45[0x3];        
    AIT_REG_W   IBC_INT_LINE_CNT0;										// 0x48
    AIT_REG_W   IBC_INT_LINE_CNT1;										// 0x4A
    AIT_REG_W   IBC_FRM_WIDTH;                                          // 0x4C   
   	AIT_REG_B                  _x4E[0x2];  
      
    AIT_REG_D   IBC_LINE_OFST;                                          // 0x50
    AIT_REG_D   IBC_CBR_LINE_OFST;                                      // 0x54
    AIT_REG_D   IBC_WR_POINTER;											// 0x58
    AIT_REG_B                           _x5C[0x4];
} AITS_IBCP, *AITPS_IBCP;

typedef struct _AITS_IBC {
    AIT_REG_B   IBC_IMG_PIPE_CTL;                                       // 0x00
        /*-DEFINE-----------------------------------------------------*/
        #define IBC_CCIR_SRC_SEL_MASK   	0x03
        #define IBC_CCIR_SRC_SEL(_a)    	(_a & IBC_CCIR_SRC_SEL_MASK)
        #define IBC_PREV_BUF_EMPTY_CHK      0x04
        #define IBC_JPEG_ENC_THUMB_EN   	0x08
        #define IBC_H264_RT_EN          	0x80
        #define IBC_H264_FB_EN          	0x00
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x01[0x3];  
    AIT_REG_B   IBC_JPEG_PIPE_SEL;                                      // 0x04
        /*-DEFINE-----------------------------------------------------*/
        #define IBC_JPEG_SRC_SEL_MASK       0x03
        #define IBC_JPEG_SRC_SEL(_a)        (_a & IBC_JPEG_SRC_SEL_MASK)
        #define IBC_JPEG_THUMB_SRC_SEL_MASK 0x30
        #define IBC_JPEG_THUMB_SRC_SEL(_a)  ((_a << 4) & IBC_JPEG_THUMB_SRC_SEL_MASK)
        #define IBC_H264_SRC_SEL_MASK       0xC0
        #define IBC_H264_SRC_SEL(_a)        ((_a << 6) & IBC_H264_SRC_SEL_MASK)
        /*------------------------------------------------------------*/  
    AIT_REG_B                           _x05[0x3];
    AIT_REG_B   IBC_MCI_CFG;                                            // 0x08
        /*-DEFINE-----------------------------------------------------*/
        #define IBC_P0_MCI_256BYTE_CNT_EN  	0x08
        #define IBC_P1_MCI_256BYTE_CNT_EN  	0x10
        #define IBC_P2_MCI_256BYTE_CNT_EN  	0x20
        #define IBC_P3_MCI_256BYTE_CNT_EN  	0x40
        #define IBC_P4_MCI_256BYTE_CNT_EN  	0x80
        /*------------------------------------------------------------*/  
    AIT_REG_B                           _x09[0x3];
    AIT_REG_B   IBC_SRAM_CFG_CTL;                                       // 0x0C
    AIT_REG_B                           _x0D[0x3]; 
    
    AIT_REG_B   IBC_P0_INT_CPU_EN;                                      // 0x10
    AIT_REG_B   IBC_P1_INT_CPU_EN;                                      // 0x11
    AIT_REG_B   IBC_P2_INT_CPU_EN;                                      // 0x12
    AIT_REG_B   IBC_P3_INT_CPU_EN;                                      // 0x13
    AIT_REG_B   IBC_P0_INT_HOST_EN;                                     // 0x14
    AIT_REG_B   IBC_P1_INT_HOST_EN;                                     // 0x15
    AIT_REG_B   IBC_P2_INT_HOST_EN;                                     // 0x16
    AIT_REG_B   IBC_P3_INT_HOST_EN;                                     // 0x17
    AIT_REG_B   IBC_P0_INT_CPU_SR;                                      // 0x18
    AIT_REG_B   IBC_P1_INT_CPU_SR;                                      // 0x19
    AIT_REG_B   IBC_P2_INT_CPU_SR;                                      // 0x1A
    AIT_REG_B   IBC_P3_INT_CPU_SR;                                      // 0x1B
    AIT_REG_B   IBC_P0_INT_HOST_SR;                                     // 0x1C
    AIT_REG_B   IBC_P1_INT_HOST_SR;                                     // 0x1D
    AIT_REG_B   IBC_P2_INT_HOST_SR;                                     // 0x1E
    AIT_REG_B   IBC_P3_INT_HOST_SR;                                     // 0x1F
        /*-DEFINE-----------------------------------------------------*/
        #define IBC_INT_FRM_RDY         	0x01
        #define IBC_INT_FRM_ST          	0x02
        #define IBC_INT_FRM_END         	0x04
        #define IBC_INT_PRE_FRM_RDY     	0x08
        #define IBC_INT_PRE_FRM_RDY_1   	0x08
        #define IBC_INT_PRE_FRM_RDY_2   	0x10
        #define IBC_INT_ALL             	0x1F
        #define IBC_INT_H264_RT_BUF_OVF   	0x40
        /*------------------------------------------------------------*/

    AITS_IBCP   IBCP_0;                                                 // 0x20~0x5F
    AIT_REG_B                           _x60[0x40];
    AITS_IBCP   IBCP_3;                                                 // 0xA0~0xDF
    AIT_REG_B                           _xE0[0x40];
    AITS_IBCP   IBCP_1;                                                 // 0x120~0x15F
    AIT_REG_B                           _x160[0x20];

    AIT_REG_B   IBC_P4_INT_CPU_EN;                                      // 0x180
    AIT_REG_B                           _x181[0x3];
    AIT_REG_B   IBC_P4_INT_HOST_EN;                                     // 0x184
    AIT_REG_B                           _x185[0x3];
    AIT_REG_B   IBC_P4_INT_CPU_SR;                                      // 0x188
    AIT_REG_B                           _x189[0x3];
    AIT_REG_B   IBC_P4_INT_HOST_SR;                                     // 0x18C
    AIT_REG_B                           _x18D[0x13];
    
    AIT_REG_D   IBC_P4_ADDR_Y_ST;                               		// 0x1A0
    AIT_REG_D   IBC_P4_ADDR_Y_END;                               		// 0x1A4
    AIT_REG_W   IBC_P4_FRM_WIDTH;                                       // 0x1A8
    AIT_REG_B                           _x1AA[0x6];

    AIT_REG_W   IBC_P4_INT_LINE_CNT0;									// 0x1B0
    AIT_REG_W   IBC_P4_INT_LINE_CNT1;									// 0x1B2
	AIT_REG_B                           _x1B4[0xC];
	AIT_REG_B   IBC_P4_BUF_CFG;										    // 0x1C0
        /*-DEFINE-----------------------------------------------------*/
        #define IBC_P4_STORE_SING_FRAME    	0x01
        #define IBC_P4_STORE_PIX_CONT      	0x04
        #define IBC_P4_STORE_PIX_NCONT      0x00
        #define IBC_P4_ICON_PAUSE_EN       	0x08
        #define IBC_P4_STORE_EN   			0x10
		#define IBC_P4_MIRROR_EN            0x40
        #define IBC_P4_RING_BUF_EN         	0x80
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x1C1[0x7];
    AIT_REG_D   IBC_P4_WR_POINTER;										// 0x1C8
    AIT_REG_B                           _x1CC[0x4];

    AIT_REG_B                           _x1D0[0x50];
    AITS_IBCP   IBCP_2;                                                 // 0x220~0x260
    AIT_REG_B                           _x260[0x20];

} AITS_IBC, *AITPS_IBC;
#endif

/// @}
#endif // _MMP_REG_IBC_H_

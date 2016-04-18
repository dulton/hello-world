//==============================================================================
//
//  File        : mmp_reg_jpeg.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_REG_JPEG_H_
#define _MMP_REG_JPEG_H_

#include "mmp_register.h"

/** @addtogroup MMPH_reg
@{
*/

#if (CHIP == MCR_V2)
typedef struct _AITS_JPG 
{
    AIT_REG_W   JPG_ENC_W;                                              // 0x00
    AIT_REG_W   JPG_ENC_H;												// 0x02
    AIT_REG_W   JPG_CTL;												// 0x04
        /*-DEFINE-----------------------------------------------------*/
        #define JPG_ENC_FMT_YUV422          0x0000
        #define JPG_ENC_FMT_MASK            0x0001
        #define JPG_DEC_EN                  0x0002
        #define JPG_DEC_EN_WITH_PAUSE       0x0000
        #define JPG_DEC_EN_NO_PAUSE         0x0004
        #define JPG_ENC_EN                  0x0008
		#define JPG_ENC_MJPG_EN             0x0010
        #define JPG_ENC_MARKER_EN           0x0020
		#define JPG_ENC_MJPG_HEADER_EN      0x0040
		#define JPG_ENC_SIZE_CTL_EN         0x0080	
        #define JPG_ENC_TO_FB_DIS           0x0100
        #define JPG_ENC_QTABLE_FROM_FILE    0x0200
        #define JPG_ENC_DC_QT_DIR_1024      0x0400
        #define JPG_ENC_DC_QT_DIR_0         0x0000
        /*------------------------------------------------------------*/
    AIT_REG_W   JPG_QLTY_CTL_FACTOR1;                                   // 0x06
    AIT_REG_W   JPG_QLTY_CTL_FACTOR2;                                   // 0x08
    AIT_REG_W   JPG_QLTY_CTL_FACTOR1_SHADOW;                            // 0x0A [RO]
    AIT_REG_W   JPG_QLTY_CTL_FACTOR2_SHADOW;                            // 0x0C [RO]
    AIT_REG_B   JPG_DEC_CTL;											// 0x0E
        /*-DEFINE-----------------------------------------------------*/
        #define JPG_DEC_DB_BUF_EN           0x01
        #define JPG_DEC_H_DNSAMP_2_EN       0x02
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x0F;
    
    AIT_REG_D   JPG_LINE_BUF_ST;                                        // 0x10 
    AIT_REG_D   JPG_ENC_CMP_BUF_ST;                                     // 0x14
    AIT_REG_D   JPG_ENC_CMP_BUF_ED;                                     // 0x18
    AIT_REG_D   JPG_ENC_FRAME_SIZE;                                     // 0x1C
   
    AIT_REG_B                           _x20[0x10];
    
    AIT_REG_B   JPG_CHROMA_CMP_CTL;                                     // 0x30
        /*-DEFINE-----------------------------------------------------*/
        #define JPG_CHROMA_CMP_DC_EN        0x01
        #define JPG_CHROMA_CMP_AC_EN        0x02
        /*------------------------------------------------------------*/    
    AIT_REG_B   JPG_CHROMA_CMP_Y_TH;                                    // 0x31
    AIT_REG_B   JPG_CHROMA_CMP_CB_TH;                                   // 0x32
    AIT_REG_B   JPG_CHROMA_CMP_CR_TH;                                   // 0x33
    AIT_REG_B                           _x34[0x4];
    AIT_REG_B   JPG_URGENT_EN;                                          // 0x38
    AIT_REG_B   JPG_URGENT_TH;                                          // 0x39
        /*-DEFINE-----------------------------------------------------*/
        #define JPG_URGENT_TH_MASK          0x07
        #define JPG_URGENT_THD(_a)          ((_a >> 8) - 1)
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x3A[0x6];
    
    AIT_REG_W   JPG_INT_HOST_EN;                                        // 0x40
    AIT_REG_W   JPG_INT_HOST_SR;									 	// 0x42
    AIT_REG_W   JPG_INT_CPU_EN;											// 0x44
    AIT_REG_W   JPG_INT_CPU_SR;											// 0x46
        /*-DEFINE-----------------------------------------------------*/
        #define JPG_INT_ENC_DONE            0x0001
        #define JPG_INT_CMP_BUF_OVERFLOW    0x0002
        #define JPG_INT_LINE_BUF_OVERLAP    0x0004
        #define JPG_INT_DEC_DONE            0x0100
        #define JPG_INT_DEC_MARKER_DONE     0x0200
        #define JPG_INT_DEC_MARKER_ERR      0x0400
        #define JPG_INT_DEC_HUFFMAN_ERR     0x0800
        #define JPG_INT_DEC_NONBASELINE     0x1000
        #define JPG_INT_ALL_MASK            0xFFFF
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x48[0x8];

    AIT_REG_D   JPG_ENC_CNT;                                            // 0x50
    AIT_REG_D   JPG_ENC_CNT_SR8;										// 0x54
    AIT_REG_B                           _x58[0x8];

    AIT_REG_B   JPG_ENC_LINEBUF_CTL;								    // 0x60
    	/*-DEFINE-----------------------------------------------------*/
    	#define JPG_LINE_BUF_CTL_MASK		0x03
    	#define JPG_SINGLE_LINE_BUF			0x02
    	#define JPG_DOUBLE_LINE_BUF_2ADDR	0x01
    	#define JPG_DOUBLE_LINE_BUF_1ADDR	0x00
    	/*------------------------------------------------------------*/
	AIT_REG_B                           _x61[0x3];
    AIT_REG_D   JPG_2ND_LINE_BUF_ST;	                                // 0x64
    AIT_REG_W   JPG_ENC_INSERT_RST;								        // 0x68, unit: MCU
    AIT_REG_B                           _x6A[0x6];

    AIT_REG_B   JPG_SCAL_CTL;                                           // 0x70, decode output FIFO control(scaling FIFO)
        /*-DEFINE-----------------------------------------------------*/
        #define JPG_DEC_CHK_SCALE_BUSY      0x01
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x71[0x3];
    AIT_REG_B   JPG_SCA_OUT_PIX_DLY;                                    // 0x74
    AIT_REG_B   JPG_DEC_Y_HV_FMT;                                       // 0x75 [RO]
    AIT_REG_B   JPG_DEC_CB_HV_FMT;                                      // 0x76 [RO]
    AIT_REG_B   JPG_DEC_CR_HV_FMT;                                      // 0x77 [RO]
    AIT_REG_W   JPG_SCA_OUT_LINE_DLY;                                   // 0x78
    AIT_REG_B                           _x7A[0x2];
    AIT_REG_W   JPG_DEC_W;												// 0x7C
    AIT_REG_W   JPG_DEC_H;												// 0x7E

    AIT_REG_B                           _x80[0x30];
    
    AIT_REG_D   JPG_DEC_BUF_CMP_ST;									    // 0xB0
    AIT_REG_D   JPG_DEC_BUF_CMP_ED;									    // 0xB4
    AIT_REG_W	JPG_ENC_MAX_BUF_USAGE;                                  // 0xB8
    AIT_REG_B                           _xBA[0x6];

    AIT_REG_D   JPG_EST_FRM_SIZE_2Q;									// 0xC0
    AIT_REG_D   JPG_EST_FRM_SIZE_4Q;									// 0xC4
    AIT_REG_B                           _xC8[0x8];
    
    AIT_REG_B                           _xD0[0x28];
    
    AIT_REG_B   JPG_SRAM_CTL_QTABLE;									// 0xF8
    AIT_REG_B   JPG_SRAM_CTL_HUFF_TABLE;								// 0xF9
    AIT_REG_B   JPG_SRAM_CTL_DCT_BUF;									// 0xFA
    AIT_REG_B   JPG_SRAM_CTL_IBC_BUF;									// 0xFB
    AIT_REG_B   JPG_SRAM_CTL_BS_BUF;									// 0xFC
    AIT_REG_B                           _xFD[0x2];
    AIT_REG_B   JPG_PROBE_SEL_CTL;									    // 0xFF
        
} AITS_JPG, *AITPS_JPG;
#endif

/// @}

#endif // _MMP_REG_JPEG_H_

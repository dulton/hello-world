//==============================================================================
//
//  File        : mmp_reg_scaler.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_REG_SCALER_H_
#define _MMP_REG_SCALER_H_

#include "mmp_register.h"

/** @addtogroup MMPH_reg
@{
*/

#if (CHIP == MCR_V2)
typedef struct _AITS_SCAL_IN_CT {
    AIT_REG_B   IN_COLOR_CTL;                               // 0x00
        /*-DEFINE-----------------------------------------------------*/
        #define IN_COLOR_UV_MINUS128  				0x01
        #define IN_COLOR_TRANS_BYPASS  				0x02
        /*------------------------------------------------------------*/    
    AIT_REG_B   IN_Y_GAIN_MINUS1;                           // 0x01
    AIT_REG_B   IN_U_GAIN_MINUS1;                           // 0x02
    AIT_REG_B   IN_V_GAIN_MINUS1;                           // 0x03
    AIT_REG_B   IN_Y_OFST;                                  // 0x04
    AIT_REG_B   IN_U_OFST;                                  // 0x05
    AIT_REG_B   IN_V_OFST;                                  // 0x06
    AIT_REG_B                           _x7;
} AITS_SCAL_IN_CT, *AITPS_SCAL_IN_CT;

typedef struct _AITS_SCAL_EE_CLIP {
    AIT_REG_B   OUT_Y_MIN;                                  // 0x00
    AIT_REG_B   OUT_Y_MAX;                                  // 0x01
    AIT_REG_B   OUT_U_MIN;                                  // 0x02
    AIT_REG_B   OUT_U_MAX;                                  // 0x03
    AIT_REG_B   OUT_V_MIN;                                  // 0x04
    AIT_REG_B   OUT_V_MAX;                                  // 0x05
    AIT_REG_B                           _x6[0x2];
} AITS_SCAL_EE_CLIP, *AITPS_SCAL_EE_CLIP;

typedef struct _AITS_SCAL_COLR_CLIP {
    AIT_REG_B   OUT_Y_MIN;                                  // 0x00
    AIT_REG_B   OUT_Y_MAX;                                  // 0x01
    AIT_REG_B   OUT_U_MIN;                                  // 0x02
    AIT_REG_B   OUT_U_MAX;                                  // 0x03
    AIT_REG_B   OUT_V_MIN;                                  // 0x04
    AIT_REG_B   OUT_V_MAX;                                  // 0x05
    AIT_REG_W   OUT_YUV_MSB;                                // 0x06
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_CLIP_MSB(_a, _c)               (((_a >> 8) & 0x03) << (_c << 1))
        /*------------------------------------------------------------*/
} AITS_SCAL_COLR_CLIP, *AITPS_SCAL_COLR_CLIP;

typedef struct _AITS_SCAL_MTX {
    AIT_REG_B   COLRMTX_CTL;                                // 0x00
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_COLRMTX_EN                     0x01
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x1[0x3];

    AIT_REG_B   MTX_COEFF_ROW1[3];                          // 0x04~0x06
    AIT_REG_B   MTX_COEFF_ROW1_MSB;                         // 0x07
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_MTX_COL_COEFF_MSB_MASK         0x3F
        #define SCAL_MTX_COL_COEFF_MSB(_a, _c)      (((_a >> 8) & 0x03) << (_c << 1))
        #define SIGN_BIT_CONVERT(_a, _nb)           ((_a < 0)? (((MMP_LONG)-_a) | (1 << (_nb-1))): _a)
        #define SCAL_MTX_MASK                       0x03FF // 10Bits
        #define SCAL_MTX_2S_COMPLEMENT(_a)          (((MMP_SHORT)_a < 0) ? ((~(-_a) + 1) & SCAL_MTX_MASK) : _a)
        /*------------------------------------------------------------*/
    AIT_REG_B   MTX_COEFF_ROW2[3];                          // 0x08~0x0A
    AIT_REG_B   MTX_COEFF_ROW2_MSB;                         // 0x0B
    AIT_REG_B   MTX_COEFF_ROW3[3];                          // 0x0C~0x0E
    AIT_REG_B   MTX_COEFF_ROW3_MSB;                         // 0x0F
} AITS_SCAL_MTX, *AITPS_SCAL_MTX;

//--------------------------------------------
// Scaler structure (0x80004500 / 0x80006400 / 0x80006F00)
//--------------------------------------------
typedef struct _AITS_SCAL 
{
	/* Base 0x4500 */
    AIT_REG_W   SCAL_GRAB_LPF_4_H_ST;                           	// 0x00
    AIT_REG_W   SCAL_GRAB_LPF_4_H_ED;								// 0x02
    AIT_REG_W   SCAL_GRAB_LPF_4_V_ST;								// 0x04
    AIT_REG_W   SCAL_GRAB_LPF_4_V_ED;								// 0x06
    AIT_REG_B                           _x4508[0x8];

    AIT_REG_W   SCAL_GRAB_SCAL_4_H_ST;                              // 0x10
    AIT_REG_W   SCAL_GRAB_SCAL_4_H_ED;								// 0x12
    AIT_REG_W   SCAL_GRAB_SCAL_4_V_ST;								// 0x14
    AIT_REG_W   SCAL_GRAB_SCAL_4_V_ED;								// 0x16
    AIT_REG_B                           _x4518[0x8];

    AIT_REG_W   SCAL_GRAB_OUT_4_H_ST;                               // 0x20
    AIT_REG_W   SCAL_GRAB_OUT_4_H_ED;								// 0x22
    AIT_REG_W   SCAL_GRAB_OUT_4_V_ST;								// 0x24
    AIT_REG_W   SCAL_GRAB_OUT_4_V_ED;								// 0x26
    AIT_REG_B                           _x4528[0x8];
    
    AIT_REG_B   SCAL_DNSAMP_LPF_4_H;                               	// 0x30
    AIT_REG_B   SCAL_DNSAMP_LPF_4_V;								// 0x31
    AIT_REG_B   SCAL_DNSAMP_SCAL_4_H;								// 0x32
    AIT_REG_B   SCAL_DNSAMP_SCAL_4_V;								// 0x33
    AIT_REG_B   SCAL_DNSAMP_OUT_4_H;								// 0x34
    AIT_REG_B   SCAL_DNSAMP_OUT_4_V;								// 0x35
    AIT_REG_B                           _x4536[0xA];
    
    AIT_REG_B   SCAL_LPF4_DN_CTL;                                   // 0x40
        /*-DEFINE-----------------------------------------------------*/
        // ref to SCAL_LPF1_DN_CTL
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x4541;
    AIT_REG_B   SCAL_LPF4_AUTO_CTL;                                 // 0x42
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_LPF4_EN                0x10
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x4543[0xD];
    
    AIT_REG_B   SCAL_SCAL_4_CTL;                                    // 0x50
    AIT_REG_B                           _x4551;
    AIT_REG_W   SCAL_SCAL_4_H_N;                                    // 0x52
    AIT_REG_W   SCAL_SCAL_4_H_M;                                    // 0x54
    AIT_REG_W   SCAL_SCAL_4_H_WT;                                   // 0x56
    AIT_REG_W   SCAL_SCAL_4_V_N;                                    // 0x58
    AIT_REG_W   SCAL_SCAL_4_V_M;                                    // 0x5A
    AIT_REG_W   SCAL_SCAL_4_V_WT;                                   // 0x5C
    AIT_REG_B                           _x455E[0x2];

    AIT_REG_B   SCAL_EDGE_4_CTL;                                    // 0x60
    AIT_REG_B                           _x4561;
    AIT_REG_B   SCAL_EDGE_4_GAIN;                                   // 0x62
    AIT_REG_B   SCAL_EDGE_4_CORE;                                   // 0x63
    AIT_REG_B                           _x4564[0x4];
    AIT_REG_B   SCAL_OUT_BLKNG_4_EN;                                // 0x68
    AIT_REG_B                           _x4569;
    AIT_REG_W   SCAL_OUT_BLKNG_4_TH;                                // 0x6A
    AIT_REG_B                           _x456C[0x4];
    
    AIT_REG_B                           _x4570[0x2];
    AIT_REG_B   SCAL_PATH_4_CTL;                                    // 0x72
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_P4_IN_COLOR_YUV_EN		0x80
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x4573;
	AIT_REG_B   SCAL_P4_PIXL_DELAY_N;                       		// 0x74
	AIT_REG_B   SCAL_P4_PIXL_DELAY_M;                      			// 0x75
	AIT_REG_B                           _x4576[0x2];
    AIT_REG_B   SCAL_P4_BUSY_MODE_CTL;                          	// 0x78
    AIT_REG_B                           _x4579;
    AIT_REG_B   SCAL_P4_H264_STOP_EN;                               // 0x7A
    AIT_REG_B                           _x457B[0x3];
	AIT_REG_B   SCAL_P4_LINE_DELAY;                         		// 0x7E
	AIT_REG_B                           _x457F;
	
	AIT_REG_B   						_x4580[0x10];
    
    AIT_REG_W   SCAL_P4_Y_OFST;                                     // 0x90
    AIT_REG_W   SCAL_P4_Y_GAIN;                                     // 0x92
    AIT_REG_B   						_x4594[0xC];

    AIT_REG_B                           _x45A0[0x4];
    AIT_REG_B   SCAL_P4_CPU_INT_EN;                                 // 0xA4
    AIT_REG_B   SCAL_P4_HOST_INT_EN;                                // 0xA5
    AIT_REG_B   SCAL_P4_CPU_INT_SR;                                 // 0xA6
    AIT_REG_B   SCAL_P4_HOST_INT_SR;                                // 0xA7
    AIT_REG_B                           _x45A8[0x6];  
	AIT_REG_B	SCAL_P4_IN_FRM_SYNC_CTL; 							// 0xAE
		    /*-DEFINE-----------------------------------------------------*/
			#define SCAL_P4_FRM_ST_MODE_EN				0x01
			#define SCAL_P4_IN_MUX_FRM_SYNC_EN  		0x02
			#define SCAL_P4_IN_MUX_SYNC_BY_ISP_FRM_ST   0x04
			#define SCAL_P4_IN_MUX_SYNC_BY_GRA_FRM_ST   0x08
			#define SCAL_P4_IN_MUX_SYNC_BY_JPG_FRM_ST  	0x10
			#define SCAL_P4_IN_MUX_SYNC_BY_LDC_FRM_ST   0x20
			#define SCAL_P4_IN_MUX_SYNC_BY_YUV_FRM_ST	0x40
		    #define SCAL_P4_IN_MUX_SYNC_CHK_MASK    	0x7C
		    /*------------------------------------------------------------*/
	AIT_REG_B                           _x45AF;
   
    AIT_REG_B   SCAL_P4_IN_COLOR_CTL;                               // 0xB0
    AIT_REG_B   SCAL_P4_IN_Y_GAIN_MINUS1;                           // 0xB1
    AIT_REG_B   SCAL_P4_IN_Y_OFST_MINUS1;                           // 0xB2
    AIT_REG_B                           _x45B3[0xD];

  	AIT_REG_B   SCAL_P4_OUT_Y_MIN;                                  // 0xC0
    AIT_REG_B   SCAL_P4_OUT_Y_MAX;                                  // 0xC1
    AIT_REG_B                           _x45C2[0xE];
    
    AIT_REG_B 	SCAL_P4_COLRMTX_CTL;								// 0xD0
    AIT_REG_B                           _x45D1[0x7];
    AIT_REG_B 	SCAL_RS_BANK_SEL;									// 0xD8
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_RS_BANK_SEL0		0x00
        #define SCAL_RS_BANK_SEL1		0x04
        #define SCAL_RS_ADDR_SEL_MASK	0x03
        #define SCAL_RS_ADDR_SEL(_a)	(_a & SCAL_RS_ADDR_SEL_MASK)
        /*------------------------------------------------------------*/
    AIT_REG_B 	SCAL_RS_PIPE_SEL;									// 0xD9 
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_RS_PIPE_SEL_MASK	0x03
        #define SCAL_RS_PIPE_SEL(_a)	(_a & SCAL_RS_PIPE_SEL_MASK)
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x45DA[0x2];
    AIT_REG_B 	SCAL_RS_HW_SRAM_SEL;								// 0xDC
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_RS_HW_ACCESS_SRAM0	0x01
        #define SCAL_RS_HW_ACCESS_SRAM1	0x02
        #define SCAL_RS_CLR_SRAM_ADDR	0x10
        /*------------------------------------------------------------*/
    AIT_REG_B 	SCAL_RS_COMP_EN;									// 0xDD
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_RS_COMP_ENABLE		0x01
        /*------------------------------------------------------------*/ 
    AIT_REG_B                           _x45DE[0x2];

    AIT_REG_W   SCAL_GRAB_SUB_SCALUP_H_ST;                       	// 0xE0
	AIT_REG_W   SCAL_GRAB_SUB_SCALUP_H_ED;                       	// 0xE2
  	AIT_REG_W   SCAL_GRAB_SUB_SCALDN_H_ST;                       	// 0xE4
  	AIT_REG_W   SCAL_GRAB_SUB_SCALDN_H_ED;                       	// 0xE6
  	AIT_REG_W   SCAL_GRAB_SUB_V_ST;                       			// 0xE8
  	AIT_REG_W   SCAL_GRAB_SUB_V_ED;                       			// 0xEA
  	AIT_REG_B                           _x45EC[0x4];
  	
  	AIT_REG_B   SCAL_P4_CT_OUT_Y_MIN;                          		// 0xF0
    AIT_REG_B   SCAL_P4_CT_OUT_Y_MAX;                               // 0xF1
  	AIT_REG_B                           _x45F2[0xE];
  	       
    AIT_REG_B                           _x4600[0x1E00];
    
	/* Base 0x6400 */
    AIT_REG_W   SCAL_GRAB_LPF_2_H_ST;                            	// 0x00
    AIT_REG_W   SCAL_GRAB_LPF_2_H_ED;								// 0x02
    AIT_REG_W   SCAL_GRAB_LPF_2_V_ST;								// 0x04
    AIT_REG_W   SCAL_GRAB_LPF_2_V_ED;								// 0x06
    AIT_REG_W   SCAL_GRAB_LPF_3_H_ST;                               // 0x08
    AIT_REG_W   SCAL_GRAB_LPF_3_H_ED;								// 0x0A
    AIT_REG_W   SCAL_GRAB_LPF_3_V_ST;								// 0x0C
    AIT_REG_W   SCAL_GRAB_LPF_3_V_ED;								// 0x0E

    AIT_REG_W   SCAL_GRAB_SCAL_2_H_ST;                              // 0x10
    AIT_REG_W   SCAL_GRAB_SCAL_2_H_ED;								// 0x12
    AIT_REG_W   SCAL_GRAB_SCAL_2_V_ST;								// 0x14
    AIT_REG_W   SCAL_GRAB_SCAL_2_V_ED;								// 0x16
    AIT_REG_W   SCAL_GRAB_SCAL_3_H_ST;                              // 0x18
    AIT_REG_W   SCAL_GRAB_SCAL_3_H_ED;								// 0x1A
    AIT_REG_W   SCAL_GRAB_SCAL_3_V_ST;								// 0x1C
    AIT_REG_W   SCAL_GRAB_SCAL_3_V_ED;								// 0x1E

    AIT_REG_W   SCAL_GRAB_OUT_2_H_ST;                               // 0x20
    AIT_REG_W   SCAL_GRAB_OUT_2_H_ED;								// 0x22
    AIT_REG_W   SCAL_GRAB_OUT_2_V_ST;								// 0x24
    AIT_REG_W   SCAL_GRAB_OUT_2_V_ED;								// 0x26
    AIT_REG_W   SCAL_GRAB_OUT_3_H_ST;                               // 0x28
    AIT_REG_W   SCAL_GRAB_OUT_3_H_ED;								// 0x2A
    AIT_REG_W   SCAL_GRAB_OUT_3_V_ST;								// 0x2C
    AIT_REG_W   SCAL_GRAB_OUT_3_V_ED;								// 0x2E
    
    AIT_REG_B   SCAL_DNSAMP_LPF_2_H;                                // 0x30
    AIT_REG_B   SCAL_DNSAMP_LPF_2_V;								// 0x31
    AIT_REG_B   SCAL_DNSAMP_SCAL_2_H;								// 0x32
    AIT_REG_B   SCAL_DNSAMP_SCAL_2_V;								// 0x33
    AIT_REG_B   SCAL_DNSAMP_OUT_2_H;								// 0x34
    AIT_REG_B   SCAL_DNSAMP_OUT_2_V;								// 0x35
    AIT_REG_B   SCAL_DNSAMP_LPF_3_H;                                // 0x36
    AIT_REG_B   SCAL_DNSAMP_LPF_3_V;								// 0x37
    AIT_REG_B   SCAL_DNSAMP_SCAL_3_H;								// 0x38
    AIT_REG_B   SCAL_DNSAMP_SCAL_3_V;								// 0x39
    AIT_REG_B   SCAL_DNSAMP_OUT_3_H;								// 0x3A
    AIT_REG_B   SCAL_DNSAMP_OUT_3_V;								// 0x3B
    AIT_REG_B                           _x03C[0x4];
    
    AIT_REG_B   SCAL_LPF2_DN_CTL;                                   // 0x40
        /*-DEFINE-----------------------------------------------------*/
        // ref to SCAL_LPF1_DN_CTL
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x041;
    AIT_REG_B   SCAL_LPF2_AUTO_CTL;                                 // 0x42
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_LPF2_EN                0x10
        #define SCAL_LPF3_EN                0x20
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x043;
    AIT_REG_B   SCAL_LPF3_DN_CTL;                                   // 0x44
    AIT_REG_B                           _x045[0xB];
    
    AIT_REG_B   SCAL_SCAL_2_CTL;                                    // 0x50
    AIT_REG_B   SCAL_SCAL_3_CTL;                                    // 0x50
        /*-DEFINE-----------------------------------------------------*/
        // ref to SCAL_SCAL_0_CTL
        /*------------------------------------------------------------*/
    AIT_REG_W   SCAL_SCAL_2_H_N;                                    // 0x52
    AIT_REG_W   SCAL_SCAL_2_H_M;                                    // 0x54
    AIT_REG_W   SCAL_SCAL_2_H_WT;                                   // 0x56
    AIT_REG_W   SCAL_SCAL_2_V_N;                                    // 0x58
    AIT_REG_W   SCAL_SCAL_2_V_M;                                    // 0x5A
    AIT_REG_W   SCAL_SCAL_2_V_WT;                                   // 0x5C
    AIT_REG_B   SCAL_SCAL_2_RS_V_N;                                 // 0x5E
	AIT_REG_B   SCAL_SCAL_2_RS_V_M;                                 // 0x5F

    AIT_REG_B   SCAL_EDGE_2_CTL;                                    // 0x60
        /*-DEFINE-----------------------------------------------------*/
        // ref to SCAL_EDGE_0_CTL
        /*------------------------------------------------------------*/
    AIT_REG_B   SCAL_EDGE_3_CTL;                                    // 0x61
    AIT_REG_B   SCAL_EDGE_2_GAIN;                                   // 0x62
    AIT_REG_B   SCAL_EDGE_2_CORE;                                   // 0x63
    AIT_REG_B   SCAL_EDGE_3_GAIN;                                   // 0x64
    AIT_REG_B   SCAL_EDGE_3_CORE;                                   // 0x65
    AIT_REG_B                           _x066[0x2];
    AIT_REG_B   SCAL_OUT_BLKNG_2_EN;                                // 0x68
    AIT_REG_B   SCAL_OUT_BLKNG_3_EN;                                // 0x69
    AIT_REG_W   SCAL_OUT_BLKNG_2_TH;                                // 0x6A
    AIT_REG_W   SCAL_OUT_BLKNG_3_TH;                                // 0x6C
    AIT_REG_B                           _x06E[0x2];
    
    AIT_REG_B   SCAL_OUT_2_CTL;                                     // 0x70
        /*-DEFINE-----------------------------------------------------*/
        // ref to SCAL_OUT_0_CTL
        /*------------------------------------------------------------*/
    AIT_REG_B   SCAL_OUT_3_CTL;                                     // 0x71
    AIT_REG_B   SCAL_PATH_2_CTL;                                    // 0x72
        /*-DEFINE-----------------------------------------------------*/
        // ref to SCAL_PATH_0_CTL
        /*------------------------------------------------------------*/
    AIT_REG_B   SCAL_PATH_3_CTL;                                    // 0x73
	AIT_REG_B   SCAL_P2_PIXL_DELAY_N;                       		// 0x74
	AIT_REG_B   SCAL_P2_PIXL_DELAY_M;                      			// 0x75
	AIT_REG_B   SCAL_P3_PIXL_DELAY_N;                       		// 0x76
	AIT_REG_B   SCAL_P3_PIXL_DELAY_M;                      			// 0x77
    AIT_REG_B   SCAL_P2_BUSY_MODE_CTL;                          	// 0x78
    AIT_REG_B   SCAL_P3_BUSY_MODE_CTL;                          	// 0x79
    AIT_REG_B   SCAL_P2_H264_STOP_EN;                               // 0x7A
    AIT_REG_B   SCAL_P3_H264_STOP_EN;                               // 0x7B
    AIT_REG_B   SCAL_P2_BAYER_OUT_CTL;                              // 0x7C
	AIT_REG_B   SCAL_P3_BAYER_OUT_CTL;                              // 0x7D
	AIT_REG_B   SCAL_P2_LINE_DELAY;                         		// 0x7E
	AIT_REG_B   SCAL_P3_LINE_DELAY;                         		// 0x7F
	
	AIT_REG_B   						_x080[0x2];
    AIT_REG_W   SCAL_SCAL_3_H_N;                                    // 0x82
    AIT_REG_W   SCAL_SCAL_3_H_M;                                    // 0x84
    AIT_REG_W   SCAL_SCAL_3_H_WT;                                   // 0x86
    AIT_REG_W   SCAL_SCAL_3_V_N;                                    // 0x88
    AIT_REG_W   SCAL_SCAL_3_V_M;                                    // 0x8A
    AIT_REG_W   SCAL_SCAL_3_V_WT;                                   // 0x8C
    AIT_REG_B   SCAL_SCAL_3_RS_V_N;                                 // 0x8E
	AIT_REG_B   SCAL_SCAL_3_RS_V_M;                                 // 0x8F
    
    AIT_REG_W   SCAL_P2_Y_OFST;                                     // 0x90
    AIT_REG_W   SCAL_P2_U_OFST;                                     // 0x92
    AIT_REG_W   SCAL_P2_V_OFST;                                     // 0x94
    AIT_REG_B                           _x096[0x2];
    AIT_REG_W   SCAL_P3_Y_OFST;                                     // 0x98
    AIT_REG_W   SCAL_P3_U_OFST;                                     // 0x9A
    AIT_REG_W   SCAL_P3_V_OFST;                                     // 0x9C
	AIT_REG_B   SCAL_P2_IN_MUX_CTL;                     			// 0x9E
	AIT_REG_B   SCAL_P3_IN_MUX_CTL;                     			// 0x9F

    AIT_REG_B                           _x0A0[0x4];
    AIT_REG_B   SCAL_P2_CPU_INT_EN;                                 // 0xA4
    AIT_REG_B   SCAL_P2_HOST_INT_EN;                                // 0xA5
    AIT_REG_B   SCAL_P2_CPU_INT_SR;                                 // 0xA6
    AIT_REG_B   SCAL_P2_HOST_INT_SR;                                // 0xA7
        /*-DEFINE-----------------------------------------------------*/
		// ref to SCAL_P0_CPU_INT_EN
        /*------------------------------------------------------------*/
    AIT_REG_B   SCAL_P3_CPU_INT_EN;                                 // 0xA8
    AIT_REG_B   SCAL_P3_HOST_INT_EN;                                // 0xA9
    AIT_REG_B   SCAL_P3_CPU_INT_SR;                                 // 0xAA
    AIT_REG_B   SCAL_P3_HOST_INT_SR;                                // 0xAB
    AIT_REG_B                           _x0AC[0x2];   
	AIT_REG_B   SCAL_P2_IN_FRM_SYNC_CTL;           					// 0xAE
	AIT_REG_B   SCAL_P3_IN_FRM_SYNC_CTL;               				// 0xAF

    AITS_SCAL_IN_CT SCAL_P2_IN_COLR_TRANS;                     		// 0xB0~0xB7
    AITS_SCAL_IN_CT SCAL_P3_IN_COLR_TRANS;                     		// 0xB8~0xBF
    
    AITS_SCAL_EE_CLIP  SCAL_P2_EE_CLIP;                        	    // 0xC0~0xC7
    AITS_SCAL_EE_CLIP  SCAL_P3_EE_CLIP;                        	    // 0xC8~0xCF
    
    AITS_SCAL_MTX SCAL_P2_CT_MTX;                         			// 0xD0~0xDF
    AITS_SCAL_MTX SCAL_P3_CT_MTX;                         			// 0xE0~0xEF
    
    AITS_SCAL_COLR_CLIP SCAL_P2_MTX_CLIP;                       	// 0xF0~0xF7
    AITS_SCAL_COLR_CLIP SCAL_P3_MTX_CLIP;                       	// 0xF8~0xFF

    AIT_REG_B                           _x100[0xA00];

	/* Base 0x6F00 */
    AIT_REG_W   SCAL_GRAB_LPF_0_H_ST;                             	// 0x00
    AIT_REG_W   SCAL_GRAB_LPF_0_H_ED;								// 0x02
    AIT_REG_W   SCAL_GRAB_LPF_0_V_ST;								// 0x04
    AIT_REG_W   SCAL_GRAB_LPF_0_V_ED;								// 0x06
    AIT_REG_W   SCAL_GRAB_LPF_1_H_ST;                              	// 0x08
    AIT_REG_W   SCAL_GRAB_LPF_1_H_ED;								// 0x0A
    AIT_REG_W   SCAL_GRAB_LPF_1_V_ST;								// 0x0C
    AIT_REG_W   SCAL_GRAB_LPF_1_V_ED;								// 0x0E

    AIT_REG_W   SCAL_GRAB_SCAL_0_H_ST;                              // 0x10
    AIT_REG_W   SCAL_GRAB_SCAL_0_H_ED;								// 0x12
    AIT_REG_W   SCAL_GRAB_SCAL_0_V_ST;								// 0x14
    AIT_REG_W   SCAL_GRAB_SCAL_0_V_ED;								// 0x16
    AIT_REG_W   SCAL_GRAB_SCAL_1_H_ST;								// 0x18
    AIT_REG_W   SCAL_GRAB_SCAL_1_H_ED;								// 0x1A
    AIT_REG_W   SCAL_GRAB_SCAL_1_V_ST;								// 0x1C
    AIT_REG_W   SCAL_GRAB_SCAL_1_V_ED;								// 0x1E

    AIT_REG_W   SCAL_GRAB_OUT_0_H_ST;                              	// 0x20
    AIT_REG_W   SCAL_GRAB_OUT_0_H_ED;								// 0x22
    AIT_REG_W   SCAL_GRAB_OUT_0_V_ST;								// 0x24
    AIT_REG_W   SCAL_GRAB_OUT_0_V_ED;								// 0x26
    AIT_REG_W   SCAL_GRAB_OUT_1_H_ST;								// 0x28
    AIT_REG_W   SCAL_GRAB_OUT_1_H_ED;								// 0x2A
    AIT_REG_W   SCAL_GRAB_OUT_1_V_ST;								// 0x2C
    AIT_REG_W   SCAL_GRAB_OUT_1_V_ED;								// 0x2E

    AIT_REG_B   SCAL_DNSAMP_LPF_0_H;                             	// 0x30
    AIT_REG_B   SCAL_DNSAMP_LPF_0_V;								// 0x31
    AIT_REG_B   SCAL_DNSAMP_SCAL_0_H;								// 0x32
    AIT_REG_B   SCAL_DNSAMP_SCAL_0_V;								// 0x33
    AIT_REG_B   SCAL_DNSAMP_OUT_0_H;								// 0x34
    AIT_REG_B   SCAL_DNSAMP_OUT_0_V;								// 0x35
    AIT_REG_B   SCAL_DNSAMP_LPF_1_H;                              	// 0x36
    AIT_REG_B   SCAL_DNSAMP_LPF_1_V;								// 0x37
    AIT_REG_B   SCAL_DNSAMP_SCAL_1_H;								// 0x38
    AIT_REG_B   SCAL_DNSAMP_SCAL_1_V;								// 0x39
    AIT_REG_B   SCAL_DNSAMP_OUT_1_H;								// 0x3A
    AIT_REG_B   SCAL_DNSAMP_OUT_1_V;								// 0x3B
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_DNSAMP_NONE            0x00
        #define SCAL_DNSAMP_1_2             0x01
        #define SCAL_DNSAMP_1_3             0x02
        #define SCAL_DNSAMP_1_4             0x03
        #define SCAL_DNSAMP_MASK            0x03
        #define SCAL_DNSAMP_KEEP_1ST_PIXL	0x00
        #define SCAL_DNSAMP_KEEP_2ND_PIXL	0x04
        #define SCAL_DNSAMP_KEEP_3RD_PIXL	0x08
        #define SCAL_DNSAMP_KEEP_4TH_PIXL	0x0C
        #define SCAL_DNSAMP_KEEP_MASK       0x0C
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x3C[0x4];

    AIT_REG_B   SCAL_LPF0_DN_CTL;                                   // 0x40
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_LPF_DNSAMP_NONE        0x00
        #define SCAL_LPF_DNSAMP_1_2         0x01
        #define SCAL_LPF_DNSAMP_1_4         0x02
        #define SCAL_LPF_DNSAMP_1_8         0x03
        #define SCAL_LPF_DNSAMP_MASK        0x03
        #define SCAL_LPF_Y_L1_EN            0x04
        #define SCAL_LPF_Y_L2_EN            0x08
        #define SCAL_LPF_Y_L3_EN            0x10
        #define SCAL_LPF_UV_L1_EN           0x20
        #define SCAL_LPF_UV_L2_EN           0x40
        /*------------------------------------------------------------*/
    AIT_REG_B   SCAL_LPF0_EQ_CTL;                                   // 0x41
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_LPF_BYPASS             0x00
        #define SCAL_LPF_4X4                0x02
        #define SCAL_LPF_7X4                0x03
        #define SCAL_LPF_EQ_MASK			0x03
        /*------------------------------------------------------------*/
    AIT_REG_B   SCAL_LPF_AUTO_CTL;                                  // 0x42
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_LPF0_EN                0x10
        #define SCAL_LPF1_EN                0x20
        /*------------------------------------------------------------*/
    AIT_REG_B   SCAL_EARLY_START_ADDR;								// 0x43
    AIT_REG_B   SCAL_LPF1_DN_CTL;                                   // 0x44
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_LPF123_DNSAMP_NONE     0x00
        #define SCAL_LPF123_DNSAMP_1_2      0x02
        #define SCAL_LPF123_DNSAMP_1_4      0x03
        #define SCAL_LPF123_Y_L1_EN         0x10
        #define SCAL_LPF123_Y_L2_EN         0x20
        #define SCAL_LPF123_UV_L1_EN        0x40
        /*------------------------------------------------------------*/ 
    AIT_REG_B                           _x45[0xB];

    AIT_REG_B   SCAL_SCAL_0_CTL;                                    // 0x50
    AIT_REG_B   SCAL_SCAL_1_CTL;									// 0x51
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_SCAL_EN                0x00
        #define SCAL_SCAL_BYPASS            0x01
        #define SCAL_SCAL_DBL_FIFO          0x02
        #define SCAL_UP_EARLY_ST            0x04
        #define SCAL_SCAL_PATH_EN           0x80
        /*------------------------------------------------------------*/
    AIT_REG_W   SCAL_SCAL_0_H_N;                                    // 0x52
    AIT_REG_W   SCAL_SCAL_0_H_M;									// 0x54
    AIT_REG_W   SCAL_SCAL_0_H_WT;									// 0x56
    AIT_REG_W   SCAL_SCAL_0_V_N;                                    // 0x58
    AIT_REG_W   SCAL_SCAL_0_V_M;									// 0x5A
    AIT_REG_W   SCAL_SCAL_0_V_WT;									// 0x5C
    AIT_REG_B   SCAL_SCAL_0_RS_V_N;                                 // 0x5E
	AIT_REG_B   SCAL_SCAL_0_RS_V_M;                                 // 0x5F
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_NM_MAX                 0x0FFF
        #define SCAL_SCAL_WT_AVG            0x01	//Check
        #define SCAL_SCAL_WT_MASK           0x0FFF
        /*------------------------------------------------------------*/
    
    AIT_REG_B   SCAL_EDGE_0_CTL;                                    // 0x60
    AIT_REG_B   SCAL_EDGE_1_CTL;									// 0x61
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_EDGE_EN                0x00
        #define SCAL_EDGE_BYPASS            0x01
        #define SCAL_EDGE_YUV_AVG_EN        0x02
        /*------------------------------------------------------------*/
    AIT_REG_B   SCAL_EDGE_0_GAIN;                                   // 0x62
    AIT_REG_B   SCAL_EDGE_0_CORE;									// 0x63
    AIT_REG_B   SCAL_EDGE_1_GAIN;									// 0x64
    AIT_REG_B   SCAL_EDGE_1_CORE;									// 0x65
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_EDGE_GAIN_MASK         0x3F
        #define SCAL_EDGE_CORE_MASK         0x3F
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x66[0x2];
    AIT_REG_B   SCAL_OUT_BLKNG_0_EN;								// 0x68                                    
    AIT_REG_B   SCAL_OUT_BLKNG_1_EN;								// 0x69   
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_OUT_BLKNG_FOR_EQ_DN    0x01
        /*------------------------------------------------------------*/   
    AIT_REG_W   SCAL_OUT_BLKNG_0_TH;                                // 0x6A
    AIT_REG_W   SCAL_OUT_BLKNG_1_TH; 								// 0x6C
    AIT_REG_B                           _x6E[0x2];

    AIT_REG_B   SCAL_OUT_0_CTL;                                     // 0x70
    AIT_REG_B   SCAL_OUT_1_CTL;										// 0x71
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_OUT_YUV_EN             0x01
        #define SCAL_OUT_RGB_EN             0x02
        #define SCAL_YUV422_LPF_EN          0x04
        #define SCAL_OUT_FMT_YUV422         0x08
        #define SCAL_OUT_FMT_YUV444         0x00
        #define SCAL_OUT_FMT_RGB888         0x00
        #define SCAL_OUT_FMT_RGB565         0x10
        #define SCAL_OUT_FMT_RGB666         0x20
        #define SCAL_OUT_DITHER_RGB565		0x40
        #define SCAL_OUT_RGB_ORDER          0x00
        #define SCAL_OUT_BGR_ORDER          0x80
        #define SCAL_OUT_MASK               0xFB
        /*------------------------------------------------------------*/
    AIT_REG_B   SCAL_PATH_0_CTL;      						        // 0x72
    AIT_REG_B   SCAL_PATH_1_CTL;      						        // 0x73
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_ISP_2_SCAL             0x00
        #define SCAL_GRA_2_SCAL             0x01
        #define SCAL_JPG_2_SCAL             0x02
        #define SCAL_LDC_2_SCAL            	0x04
        #define SCAL_P1_2_P0                0x08
        #define SCAL_P2_2_P0                0x10
        #define SCAL_P3_2_P0                0x20
        #define SCAL_P0_2_P1                0x08
        #define SCAL_P2_2_P1                0x10
        #define SCAL_P3_2_P1                0x20
        #define SCAL_P0_2_P2                0x08
        #define SCAL_P1_2_P2                0x10
        #define SCAL_P3_2_P2                0x20
        #define SCAL_P0_2_P3                0x08
        #define SCAL_P1_2_P3                0x10
        #define SCAL_P2_2_P3                0x20
        #define SCAL_SRC_MASK               0x2F
        #define SCAL_IN_COLOR_YUV_EN		0x40
        /*------------------------------------------------------------*/
 	AIT_REG_B   SCAL_P0_PIXL_DELAY_N;                       		// 0x74
	AIT_REG_B   SCAL_P0_PIXL_DELAY_M;                      			// 0x75
	AIT_REG_B   SCAL_P1_PIXL_DELAY_N;                       		// 0x76
	AIT_REG_B   SCAL_P1_PIXL_DELAY_M;                      			// 0x77
    AIT_REG_B   SCAL_P0_BUSY_MODE_CTL;                              // 0x78
    AIT_REG_B   SCAL_P1_BUSY_MODE_CTL;                              // 0x79
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_BUSY_MODE_EN           0x01
        #define SCAL_STOP_BY_JPEG_EN		0x02
        /*------------------------------------------------------------*/
    AIT_REG_B   SCAL_P0_H264_STOP_EN;                               // 0x7A
    AIT_REG_B   SCAL_P1_H264_STOP_EN;                               // 0x7B
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_STOP_BY_H264_EN        0x01
        /*------------------------------------------------------------*/ 
    AIT_REG_B   SCAL_P0_BAYER_OUT_CTL;                              // 0x7C
    AIT_REG_B   SCAL_P1_BAYER_OUT_CTL;                              // 0x7D
        /*-DEFINE-----------------------------------------------------*/
        #define SCAL_RAW_OUT_EN           	0x01
        #define SCAL_RAW_OUT_10BIT          0x02
        #define SCAL_RAW_OUT_8BIT           0x00
        #define SCAL_RAW_OUT_ORDER_MASK		0x0C
        /*------------------------------------------------------------*/
	AIT_REG_B   SCAL_P0_LINE_DELAY;                         		// 0x7E
	AIT_REG_B   SCAL_P1_LINE_DELAY;                         		// 0x7F
	
	AIT_REG_B   						_x80[0x2];
    AIT_REG_W   SCAL_SCAL_1_H_N;                                    // 0x82
    AIT_REG_W   SCAL_SCAL_1_H_M;									// 0x84
    AIT_REG_W   SCAL_SCAL_1_H_WT;									// 0x86
    AIT_REG_W   SCAL_SCAL_1_V_N;                                    // 0x88
    AIT_REG_W   SCAL_SCAL_1_V_M;									// 0x8A
    AIT_REG_W   SCAL_SCAL_1_V_WT;									// 0x8C
    AIT_REG_B                           _x8E[0x2];
    
    AIT_REG_W   SCAL_P0_Y_OFST;                                     // 0x90
    AIT_REG_W   SCAL_P0_U_OFST;                                     // 0x92
    AIT_REG_W   SCAL_P0_V_OFST;                                     // 0x94
    AIT_REG_B                           _x96[0x2];
    AIT_REG_W   SCAL_P1_Y_OFST;                                     // 0x98
    AIT_REG_W   SCAL_P1_U_OFST;                                     // 0x9A
    AIT_REG_W   SCAL_P1_V_OFST;                                     // 0x9C
        /*-DEFINE-----------------------------------------------------*/
		#define SCAL_YUV_OFST_MASK          0x07FF // 11Bits
        #define SCAL_YUV_OFST_2S_COMPLEMENT(_a)     (((MMP_SHORT)_a < 0) ? ((~(-_a) + 1) & SCAL_YUV_OFST_MASK) : _a)
        /*------------------------------------------------------------*/

    AIT_REG_B   SCAL_P0_IN_MUX_CTL;                     			// 0x9E
    AIT_REG_B   SCAL_P1_IN_MUX_CTL;                     			// 0x9F
	    /*-DEFINE-----------------------------------------------------*/
		#define SCAL_IN_MUX_SYNC_BY_ISP_FRM_ST      0x01
		#define SCAL_IN_MUX_SYNC_BY_GRA_FRM_ST      0x02
		#define SCAL_IN_MUX_SYNC_BY_JPG_FRM_ST      0x04
		#define SCAL_IN_MUX_SYNC_BY_LDC_FRM_ST      0x08
		#define SCAL_IN_MUX_SYNC_BY_YUV_FRM_ST	    0x80
		#define SCAL_IN_0_MUX_SYNC_BY_P1_FRM_ST     0x10
	    #define SCAL_IN_0_MUX_SYNC_BY_P2_FRM_ST     0x20
	    #define SCAL_IN_0_MUX_SYNC_BY_P3_FRM_ST     0x40
		#define SCAL_IN_1_MUX_SYNC_BY_P0_FRM_ST 	0x10
	    #define SCAL_IN_1_MUX_SYNC_BY_P2_FRM_ST 	0x20
		#define SCAL_IN_1_MUX_SYNC_BY_P3_FRM_ST 	0x40
		#define SCAL_IN_2_MUX_SYNC_BY_P0_FRM_ST     0x10
	    #define SCAL_IN_2_MUX_SYNC_BY_P1_FRM_ST	    0x20
	    #define SCAL_IN_2_MUX_SYNC_BY_P3_FRM_ST	    0x40
	    #define SCAL_IN_3_MUX_SYNC_BY_P0_FRM_ST	    0x10
	    #define SCAL_IN_3_MUX_SYNC_BY_P1_FRM_ST	    0x20
	    #define SCAL_IN_3_MUX_SYNC_BY_P2_FRM_ST	    0x40
	    #define SCAL_IN_MUX_SYNC_CHK_MASK    	    0xFF
	    /*------------------------------------------------------------*/
    
    AIT_REG_B                           _xA0[0x4];
    AIT_REG_B   SCAL_P0_CPU_INT_EN;                                 // 0xA4
    AIT_REG_B   SCAL_P0_HOST_INT_EN;                                // 0xA5
    AIT_REG_B   SCAL_P0_CPU_INT_SR;                                 // 0xA6
    AIT_REG_B   SCAL_P0_HOST_INT_SR;                                // 0xA7
    AIT_REG_B   SCAL_P1_CPU_INT_EN;                                 // 0xA8
    AIT_REG_B   SCAL_P1_HOST_INT_EN;                                // 0xA9
    AIT_REG_B   SCAL_P1_CPU_INT_SR;                                 // 0xAA
    AIT_REG_B   SCAL_P1_HOST_INT_SR;                                // 0xAB
        /*-DEFINE-----------------------------------------------------*/
		#define SCAL_INT_FRM_END       		0x01
		#define SCAL_INT_INPUT_END     		0x02
		#define SCAL_INT_UP_DLINE_OVWR  	0x04
		#define SCAL_INT_DBL_FRM_ST    		0x08
        #define SCAL_INT_FRM_ST        		0x10
        /*------------------------------------------------------------*/
	AIT_REG_B                           _xAC[0x2];
    AIT_REG_B   SCAL_P0_IN_FRM_SYNC_CTL;           					// 0xAE
    AIT_REG_B   SCAL_P1_IN_FRM_SYNC_CTL;               				// 0xAF
	    /*-DEFINE-----------------------------------------------------*/
		#define SCAL_FRM_ST_MODE_EN	        0x01
		#define SCAL_IN_MUX_FRM_SYNC_EN     0x02
	    /*------------------------------------------------------------*/

    AITS_SCAL_IN_CT         SCAL_P0_IN_COLR_TRANS;               	// 0xB0~0xB7
    AITS_SCAL_IN_CT         SCAL_P1_IN_COLR_TRANS;               	// 0xB8~0xBF
    
    AITS_SCAL_EE_CLIP       SCAL_P0_EE_CLIP;                        // 0xC0~0xC7
    AITS_SCAL_EE_CLIP       SCAL_P1_EE_CLIP;                        // 0xC8~0xCF
    
    AITS_SCAL_MTX           SCAL_P0_CT_MTX;                         // 0xD0~0xDF
    AITS_SCAL_MTX           SCAL_P1_CT_MTX;                         // 0xE0~0xEF
    
    AITS_SCAL_COLR_CLIP     SCAL_P0_MTX_CLIP;                       // 0xF0~0xF7
    AITS_SCAL_COLR_CLIP     SCAL_P1_MTX_CLIP;                       // 0xF8~0xFF    
    
} AITS_SCAL, *AITPS_SCAL;
#endif

/// @}

#endif // _MMP_REG_SCALER_H_

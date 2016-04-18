//==============================================================================
//
//  File        : mmp_reg_rawproc.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Eroy Yang
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_REG_RAWPROC_H_
#define _MMP_REG_RAWPROC_H_

#include "mmp_register.h"

#if (CHIP == MCR_V2)

typedef struct _AITS_RAWS_GRAB {
    AIT_REG_W   PIXL_ST;
    AIT_REG_W   PIXL_ED;
    AIT_REG_W   LINE_ST;
    AIT_REG_W   LINE_ED;
} AITS_RAWS_GRAB, *AITPS_RAWS_GRAB;

#define RAW_STORE_MAX_PLANE (4)

//--------------------------------
// RAWPROC structure (0x8000 6B00)
//--------------------------------
typedef struct _AITS_RAWPROC 
{
    AIT_REG_W   RAWPROC_F_WIDTH;                                        // 0x00
    AIT_REG_W   RAWPROC_F_HEIGHT;                                       // 0x02
    AIT_REG_D   RAWPROC_F_ADDR;                                         // 0x04
    AIT_REG_D   RAWPROC_F_ST_OFST;                                      // 0x08
    AIT_REG_W   RAWPROC_F_PIX_OFST;                                     // 0x0C
    AIT_REG_W   RAWPROC_F_LINE_OFST;                                    // 0x0E
    	/*-DEFINE-----------------------------------------------------*/    
    	#define RAWPROC_F_PIX_OFST_MASK			0x7FFF
    	#define RAWPROC_F_LINE_OFST_MASK		0x7FFF
    	#define RAW_OFST_2S_COMPLEMENT(_a)		(((MMP_SHORT)_a < 0)? ((~(-_a) + 1) & RAWPROC_F_PIX_OFST_MASK): _a)
    	/*------------------------------------------------------------*/

    AIT_REG_B   RAWPROC_MODE_SEL;                                       // 0x10
    	/*-DEFINE-----------------------------------------------------*/
        #define RAWPROC_FETCH_EN   				0x01
        #define RAWPROC_S_PLANE0_EN   			0x02
        #define RAWPROC_S_PLANE0_1PIXEL2BYTE 	0x04
        #define RAWPROC_F_BURST_EN  			0x08
        #define RAWPROC_F_1LINE_MODE_EN			0x10
        #define RAWPROC_F_1LINE_START			0x20

        #define RAWPROC_S_8BIT_MODE 	 		0x00
        #define RAWPROC_S_10BIT_MODE  			0x01
        #define RAWPROC_S_12BIT_MODE  			0x02
        #define RAWPROC_S_14BIT_MODE  			0x03
        #define RAWPROC_S_PLANE0_BIT_MODE_MASK 	0xC0
        #define RAWPROC_S_PLANE0_BIT_MODE(_m)	((_m << 6) & RAWPROC_S_PLANE0_BIT_MODE_MASK)
        /*------------------------------------------------------------*/
    AIT_REG_B   RAWPROC_F_PIXL_DELAY;                              		// 0x11
    	/*-DEFINE-----------------------------------------------------*/
        #define RAWPROC_F_MAX_PIXL_DELAY  	    0x07
        /*------------------------------------------------------------*/
    AIT_REG_B	RAWPROC_S_EN_CTL_PLANE123;								// 0x12
    	/*-DEFINE-----------------------------------------------------*/
        #define RAWPROC_S_PLANE1_EN   			0x02
        #define RAWPROC_S_PLANE2_EN   			0x04
        #define RAWPROC_S_PLANE3_EN  			0x08
        #define RAWPROC_S_PLANE1_1PIXL2BYTE 	0x20
        #define RAWPROC_S_PLANE2_1PIXL2BYTE		0x40
        #define RAWPROC_S_PLANE3_1PIXL2BYTE		0x80
        /*------------------------------------------------------------*/    
    AIT_REG_B	RAWPROC_S_BIT_MODE_PLANE123;							// 0x13
    	/*-DEFINE-----------------------------------------------------*/
        #define RAWPROC_S_PLANE1_BIT_MODE_MASK	0x0C
        #define RAWPROC_S_PLANE2_BIT_MODE_MASK	0x30
        #define RAWPROC_S_PLANE3_BIT_MODE_MASK	0xC0
        #define RAWPROC_S_PLANE1_BIT_MODE(_m)   (_m << 2)
        #define RAWPROC_S_PLANE2_BIT_MODE(_m)   (_m << 4)
        #define RAWPROC_S_PLANE3_BIT_MODE(_m)   (_m << 6)
        /*------------------------------------------------------------*/       
    AIT_REG_D   RAWPROC_S_ADDR_PLANE0;                                	// 0x14
    AIT_REG_B   RAWPROC_S_DNSAMP_H_PLANE0;                         		// 0x18
    	/*-DEFINE-----------------------------------------------------*/
        #define RAWPROC_S_DNSAMP_RATIO_MASK   	0xFF
        #define RAWPROC_S_DNSAMP_RATIO(_m)   	((_m - 1) & RAWPROC_S_DNSAMP_RATIO_MASK)
        /*------------------------------------------------------------*/
    AIT_REG_B   RAWPROC_S_SRC_SEL;                                      // 0x19
    	/*-DEFINE-----------------------------------------------------*/
    	#define RAWPROC_SRC_VIF0         		0x00
    	#define RAWPROC_SRC_VIF1         		0x01
    	#define RAWPROC_SRC_ISP_BEFORE_BS  		0x04
    	#define RAWPROC_SRC_ISP_AFTER_BS   		0x05
    	#define RAWPROC_SRC_ISP_Y_DATA			0x06
    	#define RAWPROC_SRC_ISP_AFTER_HDR		0x07
    	#define RAWPROC_SRC_MASK				0x07
        /*------------------------------------------------------------*/
    AIT_REG_W   RAWPROC_F_LINE_DELAY;            				   		// 0x1A
    	/*-DEFINE-----------------------------------------------------*/
        #define RAWPROC_F_MAX_LINE_DELAY  	    0x0FFF
        /*------------------------------------------------------------*/
    AIT_REG_D   RAWPROC_S_END_ADDR_PLANE0;                      		// 0x1C

	AIT_REG_B   RAWPROC_HOST_INT_EN_PLANE0;                  			// 0x20
	AIT_REG_B   RAWPROC_HOST_INT_SR_PLANE0;                  			// 0x21
	AIT_REG_B   RAWPROC_CPU_INT_EN_PLANE0;                  			// 0x22
	AIT_REG_B   RAWPROC_CPU_INT_SR_PLANE0;                  			// 0x23
    	/*-DEFINE-----------------------------------------------------*/
    	#define BAYER_STORE_DONE_PLANE0			0x01
        #define BAYER_STORE_FIFO_FULL_PLANE0  	0x04
        #define BAYER_FETCH_DONE				0x10
        /*------------------------------------------------------------*/
	AIT_REG_B   RAWPROC_HOST_INT_EN_PLANE123;                  			// 0x24
	AIT_REG_B   RAWPROC_HOST_INT_SR_PLANE123;                  			// 0x25
	AIT_REG_B   RAWPROC_CPU_INT_EN_PLANE123;                  			// 0x26
	AIT_REG_B   RAWPROC_CPU_INT_SR_PLANE123;                  			// 0x27
    	/*-DEFINE-----------------------------------------------------*/
    	#define BAYER_STORE_DONE_PLANE1			0x01
    	#define BAYER_STORE_DONE_PLANE2			0x02
    	#define BAYER_STORE_DONE_PLANE3			0x04    	
        #define BAYER_STORE_FIFO_FULL_PLANE1	0x10
        #define BAYER_STORE_FIFO_FULL_PLANE2	0x20
        #define BAYER_STORE_FIFO_FULL_PLANE3	0x40          
        /*------------------------------------------------------------*/
    AIT_REG_B   RAWPROC_S_DNSAMP_V_PLANE0;                       		// 0x28
	AIT_REG_B	RAWPROC_S_DNSAMP_MODE;									// 0x29
    	/*-DEFINE-----------------------------------------------------*/
    	#define RAWPROC_S_BAYER_DNSAMP			0x00
    	#define RAWPROC_S_SING_PIXL_DNSAMP		0x01
    	#define RAWPROC_S_422_DNSAMP			0x02    	
        #define RAWPROC_S_LUMA_DNSAMP			0x03         
        #define RAWPROC_S_PLANE0_DNSAMP(_m)     (_m << 0)
        #define RAWPROC_S_PLANE1_DNSAMP(_m)     (_m << 2)
        #define RAWPROC_S_PLANE2_DNSAMP(_m)     (_m << 4)
        #define RAWPROC_S_PLANE3_DNSAMP(_m)     (_m << 6)                
        /*------------------------------------------------------------*/
    AIT_REG_B   RAWPROC_S_DNSAMP_H_PLANE1; 								// 0x2A
    AIT_REG_B   RAWPROC_S_DNSAMP_H_PLANE2; 								// 0x2B
    AIT_REG_B   RAWPROC_S_DNSAMP_H_PLANE3; 								// 0x2C
    AIT_REG_B   RAWPROC_S_DNSAMP_V_PLANE1; 								// 0x2D
    AIT_REG_B   RAWPROC_S_DNSAMP_V_PLANE2; 								// 0x2E
    AIT_REG_B   RAWPROC_S_DNSAMP_V_PLANE3; 								// 0x2F
    
    AITS_RAWS_GRAB  RAWS0_GRAB[RAW_STORE_MAX_PLANE];                    // 0x30~0x4F

    AIT_REG_W   RAWPROC_F_EXTRA_LINE_TIME;            				    // 0x50
    AIT_REG_B                                       _x52[0x02];
    AIT_REG_W   RAWPROC_F_FRONT_LINE_TIME;                              // 0x54
    AIT_REG_B   RAWPROC_F_FRONT_LINE_NUM;                               // 0x56
    AIT_REG_B                                       _x57;
	AIT_REG_B	RAWPROC_F_BUSY_MODE_CTL;								// 0x58
    	/*-DEFINE-----------------------------------------------------*/
        #define RAWPROC_SCA_BUSY_MODE_EN    	0x01
        /*------------------------------------------------------------*/
	AIT_REG_B										_x59[0x7];
	
    AIT_REG_B   RAWPROC_S_FUNC_CTL0;                                    // 0x60
    	/*-DEFINE-----------------------------------------------------*/
    	#define RAWPROC_S_256BYTECNT(_plane)    (1 << (_plane))	
        #define RAWPROC_S_GRAB_EN(_plane)       (1 << (4 + _plane))
        /*------------------------------------------------------------*/    
    AIT_REG_B   RAWPROC_S_FUNC_CTL1;                                    // 0x61
    	/*-DEFINE-----------------------------------------------------*/
    	#define RAWPROC_S_RINGBUF_EN(_plane)    (1 << (_plane))
    	#define RAWPROC_S_YUV420_EN(_plane)     (1 << (4 + _plane))
        /*------------------------------------------------------------*/   
    AIT_REG_B   RAWPROC_S_FUNC_CTL2;                                    // 0x62
    	/*-DEFINE-----------------------------------------------------*/
    	#define RAWPROC_S_LINEOFST_EN(_plane)   (1 << (_plane))
    	#define RAWPROC_S_VC_EN(_plane)         (1 << (4 + _plane))
        /*------------------------------------------------------------*/  
	AIT_REG_B	RAWPROC_S_VC_SELECT;									// 0x63
    	/*-DEFINE-----------------------------------------------------*/
        #define RAWPROC_S_SRC_VC0				0x00
        #define RAWPROC_S_SRC_VC1				0x01
        #define RAWPROC_S_SRC_VC2				0x02
        #define RAWPROC_S_SRC_VC3				0x03
        #define RAWPROC_S_PLANE0_SRC_VC_MASK	0x03
        #define RAWPROC_S_PLANE1_SRC_VC_MASK	0x0C
        #define RAWPROC_S_PLANE2_SRC_VC_MASK	0x30
        #define RAWPROC_S_PLANE3_SRC_VC_MASK	0xC0
        #define RAWPROC_S_PLANE0_SRC_VC(_m)     (_m)
        #define RAWPROC_S_PLANE1_SRC_VC(_m)     (_m << 2)
        #define RAWPROC_S_PLANE2_SRC_VC(_m)     (_m << 4)
        #define RAWPROC_S_PLANE3_SRC_VC(_m)     (_m << 6) 
        /*------------------------------------------------------------*/ 
 	AIT_REG_B	RAWPROC_S_FUNC_CTL3;									// 0x64
    	/*-DEFINE-----------------------------------------------------*/
    	#define RAWPROC_S_TW4CH_EN(_plane)      (1 << (_plane))
        /*------------------------------------------------------------*/
    AIT_REG_B                                       _x65[0x2];	
	AIT_REG_B	RAWPROC_S_OPR_SYNC_EN;									// 0x67
    	/*-DEFINE-----------------------------------------------------*/
        #define RAWPROC_S_OPR_FRMSYNC_EN		0x01              
        /*------------------------------------------------------------*/  
   	AIT_REG_B	RAWPROC_F_MODE_CTL;										// 0x68
    	/*-DEFINE-----------------------------------------------------*/
        #define RAWPROC_F_8BIT_MODE				0x00             
        #define RAWPROC_F_10BIT_MODE			0x01 
        #define RAWPROC_F_12BIT_MODE			0x02 
        #define RAWPROC_F_14BIT_MODE			0x03
        #define RAWPROC_F_BIT_MODE_MASK         0x03
        #define RAWPROC_F_BIT_MODE(_m)			(_m & RAWPROC_F_BIT_MODE_MASK)
        #define RAWPROC_F_256BYTECNT_EN			0x04              
        #define RAWPROC_F_128BYTECNT_EN			0x00 
        #define RAWPROC_F_DPCM_MODE_EN			0x08
        /*------------------------------------------------------------*/
   	AIT_REG_B	RAWPROC_F_PIPELINE_MODE;								// 0x69
    	/*-DEFINE-----------------------------------------------------*/
        #define RAWPROC_F_PIPELINE_MODE_EN		0x01              
        /*------------------------------------------------------------*/    
   	AIT_REG_B	RAWPROC_F_PIPELINE_PERIOD;								// 0x6A
   	AIT_REG_B	RAWPROC_F_URGENT_CTL;									// 0x6B
    	/*-DEFINE-----------------------------------------------------*/
        #define RAWPROC_F_URGENT_MODE_EN		0x01              
        #define RAWPROC_F_URGENT_THD_MASK		0xFE
        #define RAWPROC_F_URGENT_THD(_a)		((_a << 1) & RAWPROC_F_URGENT_THD_MASK)
        /*------------------------------------------------------------*/ 
   	AIT_REG_B	RAWPROC_F_AUTO_CTL;										// 0x6C
    	/*-DEFINE-----------------------------------------------------*/
        #define RAWPROC_F_AUTO_MODE_EN			0x01              
        #define RAWPROC_F_AUTO_MODE_TRIGGER		0x02 
        /*------------------------------------------------------------*/ 
   	AIT_REG_B	RAWPROC_F_AUTO_PERIOD_L;								// 0x6D
   	AIT_REG_B	RAWPROC_F_AUTO_PERIOD_M;								// 0x6E
   	AIT_REG_B	RAWPROC_F_AUTO_BUSY_SR;									// 0x6F [RO]
    	/*-DEFINE-----------------------------------------------------*/
        #define RAWPROC_F_AUTO_IS_BUSY		    0x01
        /*------------------------------------------------------------*/

    AIT_REG_B                                       _x70[0x8];
   	AIT_REG_B	RAWPROC_S_SRAM_PARA_PLANE0;								// 0x78
   	AIT_REG_B	RAWPROC_S_SRAM_PARA_PLANE123;							// 0x79
   	AIT_REG_B	RAWPROC_F_SRAM_PARA;									// 0x7A
    AIT_REG_B                                       _x7B;      
    AIT_REG_W   RAWPROC_F_PIXL_CNT;                                     // 0x7C [RO]
    AIT_REG_W   RAWPROC_F_LINE_CNT;                                     // 0x7E [RO]

   	AIT_REG_B	RAWPROC_S_LUMA_DNSAMP_EN;								// 0x80
    	/*-DEFINE-----------------------------------------------------*/
        #define RAWPROC_S_LUMA_DNSAMP_ENABLE	0x01
        #define RAWPROC_S_LUMA_RING_ENABLE      0x02
        /*------------------------------------------------------------*/   	
   	AIT_REG_B	RAWPROC_S_LUMA_DNSAMP_CTL;								// 0x81
    	/*-DEFINE-----------------------------------------------------*/
        #define RAWPROC_S_LUMA_DNSAMP_0PIXEL	0x00
        #define RAWPROC_S_LUMA_DNSAMP_1PIXEL	0x01
        #define RAWPROC_S_LUMA_DNSAMP_0LINE	    0x00
        #define RAWPROC_S_LUMA_DNSAMP_1LINE	    0x02
        #define RAWPROC_S_LUMA_10BIT_MODE       0x04
        /*------------------------------------------------------------*/
   	AIT_REG_B	RAWPROC_S_LUMA_DNSAMP_H_DIV;							// 0x82
   	AIT_REG_B	RAWPROC_S_LUMA_DNSAMP_V_DIV;							// 0x83
    	/*-DEFINE-----------------------------------------------------*/
        #define RAWPROC_S_LUMA_DNSAMP_DIV_MASK	0x0F
        /*------------------------------------------------------------*/
    AIT_REG_B                                       _x84[0xC];
    
   	AIT_REG_B	RAWPROC_GAMMA_BLOOM_SIGN;								// 0x90
   	AIT_REG_B	RAWPROC_GAMMA_BLOOM_VAL;								// 0x91
   	AIT_REG_B	RAWPROC_GAMMA_BLOOM_GAIN;								// 0x92
    AIT_REG_B                                       _x93; 
   	AIT_REG_B	RAWPROC_GAMMA_BLOOM_CTL;								// 0x94
    	/*-DEFINE-----------------------------------------------------*/
        #define RAWPROC_GAMMA_CORRECT_EN        0x01
        #define RAWPROC_GAMMA_OUT_VAL_INV_EN    0x02
        /*------------------------------------------------------------*/   	
   	AIT_REG_B	RAWPROC_GAMMA_BLOOM_COOR_B[11];							// 0x95~0x9F

    AIT_REG_D   RAWPROC_S_ADDR_PLANE1;                                	// 0xA0
    AIT_REG_D   RAWPROC_S_ADDR_PLANE2;                                	// 0xA4
    AIT_REG_D   RAWPROC_S_ADDR_PLANE3;                                	// 0xA8
    AIT_REG_B                                       _xAC[0x4]; 

    AIT_REG_D   RAWPROC_S_END_ADDR_PLANE1;                            	// 0xB0
    AIT_REG_D   RAWPROC_S_END_ADDR_PLANE2;                            	// 0xB4
    AIT_REG_D   RAWPROC_S_END_ADDR_PLANE3;                            	// 0xB8 
    AIT_REG_B                                       _xBC[0x4]; 

    AIT_REG_D   RAWPROC_S_LINEOFST[RAW_STORE_MAX_PLANE];                // 0xC0~0xCF

    AIT_REG_B   RAWPROC_S_URGENT_CTL[RAW_STORE_MAX_PLANE];              // 0xD0~0xD3
    	/*-DEFINE-----------------------------------------------------*/
        #define RAWPROC_S_URGENT_MODE_EN		0x01              
        #define RAWPROC_S_URGENT_THD_MASK		0xFE
        #define RAWPROC_S_URGENT_THD(_a)		((_a << 1) & RAWPROC_S_URGENT_THD_MASK)
        /*------------------------------------------------------------*/ 
    AIT_REG_B                                       _xD4[0x1C];
     
    AIT_REG_B   RAWPROC_F_ROLLING_OFST_CTL;                             // 0xF0
    	/*-DEFINE-----------------------------------------------------*/
        #define RAWPROC_F_ROLLING_OFST_EN		0x01              
        #define RAWPROC_F_ROLLING_OFST_TBL_AUTO_INIT0   0x02 
        /*------------------------------------------------------------*/ 
    AIT_REG_B   RAWPROC_F_ROLLING_OFST_TBL_BANK_SEL;                    // 0xF1
    	/*-DEFINE-----------------------------------------------------*/
        #define RAWPROC_F_ROLLING_OFST_TBL_BANK_MASK    0x0F
        /*------------------------------------------------------------*/     
    AIT_REG_W   RAWPROC_F_ROLLING_OFST_START_LINE;                      // 0xF2
    AIT_REG_B                                       _xF4[0xA];    
    AIT_REG_W   RAWPROC_ISP2RAW_H_LEN;                       			// 0xFE
	
	AIT_REG_B                                       _xC00[0x1200];

	// RAWPROC1 (2nd) structure (0x8000 7E00)
    AIT_REG_B                                       _xE00[0x10];
    
	AIT_REG_B   RAWPROC1_MODE_SEL;                                      // 0x10
    	/*-DEFINE-----------------------------------------------------*/
        // Ref: RAWPROC_MODE_SEL
        /*------------------------------------------------------------*/
    AIT_REG_B                                       _xE11; 
    AIT_REG_B	RAWPROC1_S_EN_CTL_PLANE123;								// 0x12
    	/*-DEFINE-----------------------------------------------------*/
		// Ref: RAWPROC_S_EN_CTL_PLANE123
        /*------------------------------------------------------------*/ 
	AIT_REG_B	RAWPROC1_S_BIT_MODE_PLANE123;							// 0x13
    	/*-DEFINE-----------------------------------------------------*/
		// Ref: RAWPROC_S_BIT_MODE_PLANE123
        /*------------------------------------------------------------*/ 
    AIT_REG_D   RAWPROC1_S_ADDR_PLANE0;                          	 	// 0x14
    AIT_REG_B   RAWPROC1_S_DNSAMP_H_PLANE0;                     		// 0x18
    AIT_REG_B   RAWPROC1_S_SRC_SEL;                             		// 0x19
    	/*-DEFINE-----------------------------------------------------*/
		// Ref: RAWPROC_S_SRC_SEL
        /*------------------------------------------------------------*/ 
    AIT_REG_B                                       _xE1A[0x2];
    AIT_REG_D   RAWPROC1_S_END_ADDR_PLANE0;                          	// 0x1C
    
	AIT_REG_B   RAWPROC1_HOST_INT_EN_PLANE0;                  			// 0x20
	AIT_REG_B   RAWPROC1_HOST_INT_SR_PLANE0;                  			// 0x21
	AIT_REG_B   RAWPROC1_CPU_INT_EN_PLANE0;                  			// 0x22
	AIT_REG_B   RAWPROC1_CPU_INT_SR_PLANE0;                  			// 0x23
    	/*-DEFINE-----------------------------------------------------*/
		// Ref: RAWPROC_HOST_INT_EN_PLANE0
        /*------------------------------------------------------------*/
	AIT_REG_B   RAWPROC1_HOST_INT_EN_PLANE123;                  		// 0x24
	AIT_REG_B   RAWPROC1_HOST_INT_SR_PLANE123;                  		// 0x25
	AIT_REG_B   RAWPROC1_CPU_INT_EN_PLANE123;                  			// 0x26
	AIT_REG_B   RAWPROC1_CPU_INT_SR_PLANE123;                  			// 0x27
    	/*-DEFINE-----------------------------------------------------*/
		// Ref: RAWPROC_HOST_INT_EN_PLANE123
        /*------------------------------------------------------------*/         
    AIT_REG_B   RAWPROC1_S_DNSAMP_V_PLANE0;                          	// 0x28
	AIT_REG_B	RAWPROC1_S_DNSAMP_MODE;									// 0x29
    	/*-DEFINE-----------------------------------------------------*/
		// Ref: RAWPROC_S_DNSAMP_MODE
        /*------------------------------------------------------------*/  
    AIT_REG_B   RAWPROC1_S_DNSAMP_H_PLANE1; 							// 0x2A
    AIT_REG_B   RAWPROC1_S_DNSAMP_H_PLANE2; 							// 0x2B
    AIT_REG_B   RAWPROC1_S_DNSAMP_H_PLANE3; 							// 0x2C
    AIT_REG_B   RAWPROC1_S_DNSAMP_V_PLANE1; 							// 0x2D
    AIT_REG_B   RAWPROC1_S_DNSAMP_V_PLANE2; 							// 0x2E
    AIT_REG_B   RAWPROC1_S_DNSAMP_V_PLANE3; 							// 0x2F
    
    AITS_RAWS_GRAB  RAWS1_GRAB[RAW_STORE_MAX_PLANE];                    // 0x30~0x4F 
	
	/* For Merge Frame */
    AIT_REG_B   RAWPROC_FRAME_MERGE_EN;            						// 0x50
    	/*-DEFINE-----------------------------------------------------*/
    	#define RAWPROC_MERGE_LEFT_RIGHT		0x01
    	#define RAWPROC_MERGE_TOP_DOWN			0x02
        /*------------------------------------------------------------*/
    AIT_REG_B                                       _xE51; 
    AIT_REG_W   RAWPROC_SUBFETCH_H_LEN;            						// 0x52
    AIT_REG_W   RAWPROC_SUBFETCH_V_LEN;            						// 0x54
    AIT_REG_W   RAWPROC_2ND_FETCH_ADDR_L;                             	// 0x56
    AIT_REG_W   RAWPROC_2ND_FETCH_ADDR_H;                             	// 0x58
    AIT_REG_W   RAWPROC_2ND_FETCH_ST_OFST_L;                       		// 0x5A
    AIT_REG_W   RAWPROC_2ND_FETCH_ST_OFST_H;                           	// 0x5C
    AIT_REG_W   RAWPROC_2ND_FETCH_LINE_OFST;                        	// 0x5E
	
    AIT_REG_B   RAWPROC1_S_FUNC_CTL0;                                   // 0x60
    	/*-DEFINE-----------------------------------------------------*/
		// Ref: RAWPROC_S_FUNC_CTL0
        /*------------------------------------------------------------*/ 
    AIT_REG_B   RAWPROC1_S_FUNC_CTL1;                                   // 0x61
    	/*-DEFINE-----------------------------------------------------*/
		// Ref: RAWPROC_S_FUNC_CTL1
        /*------------------------------------------------------------*/ 
    AIT_REG_B   RAWPROC1_S_FUNC_CTL2;                                   // 0x62
    	/*-DEFINE-----------------------------------------------------*/
		// Ref: RAWPROC_S_FUNC_CTL2
        /*------------------------------------------------------------*/ 
	AIT_REG_B	RAWPROC1_S_VC_SELECT;									// 0x63
    	/*-DEFINE-----------------------------------------------------*/
		// Ref: RAWPROC_S_VC_SELECT
        /*------------------------------------------------------------*/ 
 	AIT_REG_B	RAWPROC1_S_FUNC_CTL3;									// 0x64
    	/*-DEFINE-----------------------------------------------------*/
		// Ref: RAWPROC_S_FUNC_CTL3
        /*------------------------------------------------------------*/ 
    AIT_REG_B                                       _xE65[0x2];
	AIT_REG_B	RAWPROC1_S_OPR_SYNC_EN;									// 0x67    
    	/*-DEFINE-----------------------------------------------------*/
        // Ref: RAWPROC_S_OPR_SYNC_EN              
        /*------------------------------------------------------------*/
    AIT_REG_B                                       _xE68[0x10];

   	AIT_REG_B	RAWPROC1_S_SRAM_PARA0;									// 0x78
   	AIT_REG_B	RAWPROC1_S_SRAM_PARA123;								// 0x79
    AIT_REG_B                                       _xE7A[0x6];

   	AIT_REG_B	RAWPROC1_S_LUMA_DNSAMP_EN;								// 0x80
   	AIT_REG_B	RAWPROC1_S_LUMA_DNSAMP_CTL;								// 0x81
   	AIT_REG_B	RAWPROC1_S_LUMA_DNSAMP_H_DIV;							// 0x82
   	AIT_REG_B	RAWPROC1_S_LUMA_DNSAMP_V_DIV;							// 0x83   	   	
    AIT_REG_B                                       _xE84[0xC];

   	AIT_REG_B	RAWPROC1_GAMMA_BLOOM_SIGN;								// 0x90    
   	AIT_REG_B	RAWPROC1_GAMMA_BLOOM_VAL;								// 0x91
   	AIT_REG_B	RAWPROC1_GAMMA_BLOOM_GAIN;								// 0x92
    AIT_REG_B                                       _xE93; 
   	AIT_REG_B	RAWPROC1_GAMMA_BLOOM_CTL;								// 0x94
   	AIT_REG_B	RAWPROC1_GAMMA_BLOOM_COOR_B[11];						// 0x95~0x9F

    AIT_REG_D   RAWPROC1_S_ADDR_PLANE1;                               	// 0xA0
    AIT_REG_D   RAWPROC1_S_ADDR_PLANE2;                               	// 0xA4
    AIT_REG_D   RAWPROC1_S_ADDR_PLANE3;                               	// 0xA8    
    AIT_REG_B                                       _xEAC[0x4]; 

    AIT_REG_D   RAWPROC1_S_END_ADDR_PLANE1;                           	// 0xB0
    AIT_REG_D   RAWPROC1_S_END_ADDR_PLANE2;                           	// 0xB4
    AIT_REG_D   RAWPROC1_S_END_ADDR_PLANE3;                           	// 0xB8 
    AIT_REG_B                                       _xEBC[0x4];

    AIT_REG_D   RAWPROC1_S_LINEOFST[RAW_STORE_MAX_PLANE];               // 0xC0~0xCF 

    AIT_REG_B   RAWPROC1_S_URGENT_CTL[RAW_STORE_MAX_PLANE];             // 0xD0~0xD4  
    	/*-DEFINE-----------------------------------------------------*/
		// Ref: RAWPROC_S_URGENT_CTL
        /*------------------------------------------------------------*/
    AIT_REG_B                                       _xED4[0x2A]; 

    AIT_REG_W   RAWPROC1_ISP2RAW_H_LEN;                       			// 0xFE  
	
} AITS_RAWPROC, *AITPS_RAWPROC;
#endif

#endif //_MMP_REG_RAWPROC_H_


//==============================================================================
//
//  File        : mmp_reg_mp3dec.h
//  Description : INCLUDE File for the AIT555 register map.
//  Author      : Allen Chen
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_REG_MP3DEC_H_
#define _MMP_REG_MP3DEC_H_

#include "mmp_register.h"

/** @addtogroup MMPH_reg
@{
*/

#if	(CHIP == P_V2)
// ********************************
//  MP3 Decoder (0x8001 1000)
// ********************************
typedef struct _AITS_MP3DEC {     //MP3 HW 0.13
    AIT_REG_B   MP3D_EN;                                                 // 0x00
        /*-DEFINE-----------------------------------------------------*/
		#define MP3D_SINGLE_FUNC_EN 	   	(0x08)
		#define MP3D_MP3DECODER_EN		   	(0x04)
		#define MP3D_IMDCT_EN			  	(0x02)
		#define MP3D_SUBBAND_EN		   	   	(0x01)
		/*------------------------------------------------------------*/
    AIT_REG_B                           _x01;   						 // 0x01                
    AIT_REG_B   MP3D_SINGLE_ACTIVE;   						             // 0x02
    AIT_REG_B                           _x03;   						 // 0x03
	AIT_REG_B   MP3D_CPU_INTR_EN;                                        // 0x04
	    /*-DEFINE-----------------------------------------------------*/
		#define INTR_SINGLE_FUNC_DONE		(0x08)
		#define INTR_MP3DECODER_DONE		(0x04)
		#define INTR_IMDCT_DONE 			(0x02)
		#define INTR_SUB_DONE		   		(0x01)
		/*------------------------------------------------------------*/    
    AIT_REG_B   MP3D_HOST_INTR_EN;                                    	 // 0x05
    AIT_REG_B   MP3D_CPU_INTR_STATUS;                                    // 0x06
        /*-DEFINE-----------------------------------------------------*/
		// same as MP3D_CPU_INTR_EN
		/*------------------------------------------------------------*/    
    AIT_REG_B   MP3D_HOST_INTR_STATUS;                                   // 0x07
	AIT_REG_B	MP3D_INFO;									 			 // 0x08
		/*-DEFINE-----------------------------------------------------*/
		#define MP3D_INFO_STEREO_EN			(0x01)
		#define MP3D_INFO_MONO_EN			(0x00)
		/*------------------------------------------------------------*/    
    AIT_REG_B   MP3D_CLK_CFG;                         	                 // 0x09
		/*-DEFINE-----------------------------------------------------*/
        #define MP3D_CLK_CFG_EQU_MCI    	(0x01)
        #define MP3D_CLK_CFG_NEQU_MCI  		(0x00)
		/*------------------------------------------------------------*/    
    AIT_REG_B   MP3D_PPS_CLIP_SHIFT;                                     // 0x0A
    AIT_REG_B   						_x0B;                         	 // 0x0B    
    AIT_REG_D   MP3D_PPS_VBUF_ADDR_ST;                                   // 0x0C
    
    AIT_REG_D   MP3D_PPS_PCM_ADDR_ST;                                    // 0x10
    AIT_REG_B   MP3D_FDCT32_CTL;                                         // 0x14
    	/*-DEFINE-----------------------------------------------------*/
        #define FDCT32_ODDBLK_PARAM      	(0x02)
        #define FDCT32_2ND_CALL         	(0x01)
        #define FDCT32_1ST_CALL           	(0x00)
		/*------------------------------------------------------------*/  
    AIT_REG_B                           _0x15;                           // 0x15
    AIT_REG_W   MP3D_FDCT32_OFFSET;                                      // 0x16
    AIT_REG_B   MP3D_FDCT32_GB0;                                         // 0x18
    AIT_REG_B   MP3D_FDCT32_GB1;                                         // 0x19
    AIT_REG_W   MP3D_FDCT32_VBUF_LEN;                                    // 0x1A
    AIT_REG_D   MP3D_FDCT32_BUF0_ADDR_ST;                                // 0x1C

    AIT_REG_D   MP3D_FDCT32_BUF1_ADDR_ST;                                // 0x20
    AIT_REG_D   MP3D_FDCT32_DST_ADDR_ST;                                 // 0x24
    AIT_REG_D                           _0x28;                           // 0x28
    AIT_REG_B   MP3D_IMDCT_SETTING;                                      // 0x2C
    AIT_REG_B   MP3D_IMDCT_GB;                                           // 0x2D
    AIT_REG_W                           _0x2E;                           // 0x2E

    AIT_REG_D   MP3D_IMDCT_CUR_BUF_ADDR_0_ST;                            // 0x30
    AIT_REG_D   MP3D_IMDCT_PRV_BUF_ADDR_0_ST;                            // 0x34
    AIT_REG_D   MP3D_IMDCT_YOUT_BUF_ADDR_0_ST;                           // 0x38
    AIT_REG_D   MP3D_IMDCT_MOUT_BUF_ADDR_0_ST;                           // 0x3C

    AIT_REG_B   MP3D_IMDCT_NB_FLY_0;                                     // 0x40
    	/*-DEFINE-----------------------------------------------------*/        
        #define IMDCT_NBFLY_MASK                (0x3F)  //bit 0-5
    	/*------------------------------------------------------------*/  
    AIT_REG_B   MP3D_IMDCT_NBLK_LNG_0;                                   // 0x41
    	/*-DEFINE-----------------------------------------------------*/        
        #define IMDCT_NBLK_LNG_MASK             (0x3F)  //bit 0-5
    	/*------------------------------------------------------------*/  
    AIT_REG_B   MP3D_IMDCT_NBLK_TOTAL_0;                                 // 0x42
    	/*-DEFINE-----------------------------------------------------*/        
        #define IMDCT_NBLK_TOTAL_MASK           (0x3F)  //bit 0-5
    	/*------------------------------------------------------------*/  
    AIT_REG_B   MP3D_IMDCT_NBLK_PRV_0;                                   // 0x43
    	/*-DEFINE-----------------------------------------------------*/        
        #define IMDCT_NBLK_PREV_MASK            (0x3F)  //bit 0-5
    	/*------------------------------------------------------------*/  
    AIT_REG_B   MP3D_IMDCT_CUR_WIN_SW_0;                                 // 0x44
    	/*-DEFINE-----------------------------------------------------*/        
        #define IMDCT_CUR_WIN_SW_MASK           (0x3F)  //bit 0-5
    	/*------------------------------------------------------------*/  
    AIT_REG_B   MP3D_IMDCT_PRV_WIN_SW_0;                                 // 0x45
    	/*-DEFINE-----------------------------------------------------*/        
        #define IMDCT_PRV_WIN_SW_MASK           (0x3F)  //bit 0-5
    	/*------------------------------------------------------------*/  
    AIT_REG_B   MP3D_IMDCT_BLK_TYPE_0;                                   // 0x46
    	/*-DEFINE-----------------------------------------------------*/        
        #define IMDCT_MX_BLK_SHIFT              (0x4)  //<<4 for bit-4
        #define IMDCT_PRV_TYPE_SHIFT            (0x2)  //<<2 for bit-2~3
        #define IMDCT_BLK_TYPE_SHIFT            (0x0)  //<<0 for bit-0~1
    	/*------------------------------------------------------------*/
    AIT_REG_B   MP3D_IMDCT_GB_IN_0;                                      // 0x47
    AIT_REG_B   MP3D_IMDCT_NBLK_OUT_0;                                   // 0x48
    	/*-DEFINE-----------------------------------------------------*/        
        #define IMDCT_NBLKOUT_MASK              (0x3F)  //bit 0-5
    	/*------------------------------------------------------------*/  
	AIT_REG_B							_x49[7];						 // 0x49

	AIT_REG_D	MP3D_IMDCT_CUR_BUF_ADDR_1_ST; 	 					 	 // 0x50
	AIT_REG_D	MP3D_IMDCT_PRV_BUF_ADDR_1_ST; 						 	 // 0x54
	AIT_REG_D	MP3D_IMDCT_YOUT_BUF_ADDR_1_ST;						 	 // 0x58
	AIT_REG_D	MP3D_IMDCT_MOUT_BUF_ADDR_1_ST;						 	 // 0x5C

	AIT_REG_B	MP3D_IMDCT_NB_FLY_1;									 // 0x60
	AIT_REG_B	MP3D_IMDCT_NBLK_LNG_1;									 // 0x61
	AIT_REG_B	MP3D_IMDCT_NBLK_TOTAL_1;								 // 0x62
	AIT_REG_B	MP3D_IMDCT_NBLK_PRV_1;									 // 0x63
	AIT_REG_B	MP3D_IMDCT_CUR_WIN_SW_1;								 // 0x64
	AIT_REG_B	MP3D_IMDCT_PRV_WIN_SW_1;								 // 0x65
	AIT_REG_B	MP3D_IMDCT_BLK_TYPE_1;									 // 0x66
	AIT_REG_B	MP3D_IMDCT_GB_IN_1;										 // 0x67
	AIT_REG_B	MP3D_IMDCT_NBLK_OUT_1;									 // 0x68

} AITS_MP3DEC, *AITPS_MP3DEC;
#endif //(CHIP == P_V2)

/// @}

#endif // _MMP_REG_MP3DEC_H_

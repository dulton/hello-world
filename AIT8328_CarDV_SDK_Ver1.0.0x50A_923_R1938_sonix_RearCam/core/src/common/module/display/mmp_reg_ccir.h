//==============================================================================
//
//  File        : mmp_reg_ccir.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Will Chen
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_REG_CCIR_H_
#define _MMP_REG_CCIR_H_

#include "mmp_register.h"

/** @addtogroup MMPH_reg
@{
*/

#if (CHIP == MCR_V2)
//------------------------------
// CCIR Structure (0x8000 3200) Non-Periodic CCIR Controller
// CCIR Structure (0x8000 3300) Periodic CCIR Controller
//------------------------------
typedef struct _AITS_CCIR {
	AIT_REG_B	CCIR_MODE_SELECT;			            //0x00
    	/*-DEFINE-------------------------------------------------------*/
    	#define	CCIR_DEV_ENABLE				    0x01
    	#define	CCIR_DEV_DISABLE			    0x00
    	#define CCIR_HSYNC_LOW_ACTIVE		    0x04
    	#define CCIR_HSYNC_HIGH_ACTIVE		    0x00
    	#define CCIR_VSYNC_LOW_ACTIVE		    0x08
    	#define CCIR_VSYNC_HIGH_ACTIVE		    0x00
    	#define CCIR_PREVIEW_MODE_EN		    0x40
    	#define CCIR_CAPTURE_MODE_EN		    0x80
    	#define	CCIR_MODE_MASK				    0xC0
    	/*--------------------------------------------------------------*/
	AIT_REG_B	CCIR_CTL_0;				                //0x01
    	/*-DEFINE-------------------------------------------------------*/
    	#define CCIR_PCLK_DIV_MASK              0x0F
    	#define CCIR_PCLK_DIV(_a)               ((_a - 1) & CCIR_PCLK_DIV_MASK)
    	#define CCIR_PCLK_KEEP_ACT_AT_BLANKING  0x10
    	#define CCIR_PCLK_HOLD_LOW_AT_BLANKING  0x00
    	#define CCIR_BUS_IDLE_VALUE_BY_SYNC	    0x20
    	#define CCIR_BUS_IDLE_VALUE_ZERO        0x00
    	#define CCIR_OUTPUT_TRI_STATE		    0x40 // default
    	#define CCIR_OUTPUT_ENABLE		        0x00
    	#define CCIR_PCLK_INVERT_EN			    0x80
    	/*--------------------------------------------------------------*/
	AIT_REG_B	CCIR_CTL_1;				                //0x02
    	/*-DEFINE-------------------------------------------------------*/
    	#define CCIR_CAPTURE_FIX_H_PIXELS	    0x00
    	#define CCIR_CAPTURE_CONSTANT_RATE	    0x01
    	#define CCIR_CAPTURE_NO_FIX_H_PIEXELS   0x02
    	#define CCIR_CAPT_MODE_JPEG_ONLY        0x00
    	#define CCIR_CAPT_MODE_JPEG_WITH_THUMB  0x04
    	#define CCIR_CAPT_MODE_NORMAL_JPEG      0x00
        #define CCIR_CAPT_MODE_MOTION_JPEG      0x08
    	#define CCIR_SW_TRIGGER				    0x10
    	#define CCIR_HW_TRIGGER				    0x00
    	#define CCIR_SYNC_PULSE_OVERLAP_DIS	    0x20
    	#define	CCIR_SYNC_PULSE_OVERLAP_EN 	    0x00
    	#define CCIR_LCD_SEND_PORCH_EN		    0x40
    	#define CCIR_LCD_SEND_PORCH_DIS		    0x00
    	#define CCIR_SW_TRIG_MULTI_FRAME        0x80
    	#define CCIR_SW_TRIG_SINGLE_FRAME       0x00
    	/*--------------------------------------------------------------*/
	AIT_REG_B	CCIR_CTL_2;				                //0x03
    	/*-DEFINE-------------------------------------------------------*/
    	#define	CCIR_FRAME_SYNC_EN			    0x00
    	#define	CCIR_FRAME_SYNC_DIS			    0x01
    	#define CCIR_DMA_MODE_GRAB_EN           0x04
    	/*--------------------------------------------------------------*/
	AIT_REG_B	CCIR_CTL_3;				                //0x04
		/*-DEFINE-------------------------------------------------------*/
        #define CCIR_THUMB_FROM_IBC_YUV_FORMAT  0x00
        #define CCIR_THUMB_FROM_JPEG_ENCODER    0x01
        #define CCIR_FETCH_ENG1_SEL_MAIN_JPEG   0x00
        #define CCIR_FETCH_ENG1_SEL_2ND_JPEG    0x02
    	#define CCIR_SHARE_MIPI_TX_SRAM_EN      0x04
    	#define	CCIR_1ST_MCI_128BYTE_SEL	    0x00
    	#define	CCIR_1ST_MCI_256BYTE_SEL	    0x08
    	#define CCIR_2ND_MCI_128BYTE_SEL	    0x00
    	#define	CCIR_2ND_MCI_256BYTE_SEL	    0x10
    	/*--------------------------------------------------------------*/
	AIT_REG_B	CCIR_CTL_4;				                //0x05
		/*-DEFINE-------------------------------------------------------*/
    	#define	CCIR_1ST_SRC_LINE_THD_DIS       0x01
        #define	CCIR_2ND_SRC_LINE_THD_DIS       0x02
    	/*--------------------------------------------------------------*/
	AIT_REG_B	                        _x06[2];
	AIT_REG_W	CCIR_INT_HOST_EN;			            //0x08
	AIT_REG_W	CCIR_INT_HOST_SR;			            //0x0A
	AIT_REG_W	CCIR_INT_CPU_EN;			            //0x0C
	AIT_REG_W	CCIR_INT_CPU_SR;			            //0x0E
    	/*-DEFINE-------------------------------------------------------*/
    	#define CCIR_INT_1ST_BUF_OVERFLOW               0x0001
    	#define CCIR_INT_2ND_BUF_OVERFLOW               0x0002
    	#define CCIR_INT_NOT_CONST_RATE                 0x0004
    	#define CCIR_INT_FRAME_DONE                     0x0008
    	#define CCIR_INT_CAPT_MODE1_NOT_COMPLETE        0x0010
    	#define CCIR_INT_FRAME_START                    0x0020
    	
    	#define CCIR_INT_1ST_BUF_UNDERFLOW              0x0100
    	#define CCIR_INT_1ST_FIFO_UNDERFLOW             0x0200
    	#define CCIR_INT_1ST_BUF_HIT_LOW_THD            0x0400
    	#define CCIR_INT_1ST_BUF_HIT_HIGH_THD           0x0800
    	#define CCIR_INT_2ND_BUF_UNDERFLOW              0x1000
    	#define CCIR_INT_2ND_FIFO_UNDERFLOW             0x2000
    	#define CCIR_INT_2ND_BUF_HIT_LOW_THD            0x4000
    	#define CCIR_INT_2ND_BUF_HIT_HIGH_THD           0x8000
    	/*--------------------------------------------------------------*/
    	
	AIT_REG_W	CCIR_VSYNC_ST_CNT;			            //0x10
	AIT_REG_W	CCIR_HSYNC_ST_CNT;			            //0x12
    	/*-DEFINE-------------------------------------------------------*/
    	#define CCIR_VSYNC_ST_CNT_MASK	        0x3FFF
    	#define CCIR_HSYNC_ST_CNT_MASK	        0x3FFF
    	/*--------------------------------------------------------------*/	
	AIT_REG_W	CCIR_HSYNC_BLANK_CNT;			        //0x14
	AIT_REG_W	CCIR_BLANK_DATA_OUTPUT;			        //0x16
	AIT_REG_D	CCIR_PREVIEW_BUF_ST_ADDR;		        //0x18
	AIT_REG_D	CCIR_PREVIEW_BUF_ED_ADDR;		        //0x1C
	
	AIT_REG_D	CCIR_CAPTURE_BUF_ST_ADDR;		        //0x20
	AIT_REG_D	CCIR_CAPTURE_BUF_ED_ADDR;		        //0x24
    AIT_REG_B	                        _x28[2];
   	AIT_REG_W	CCIR_VSYNC_BLANK_CNT;			        //0x2A
	AIT_REG_B	CCIR_PREVIEW_FORMAT;			        //0x2C
    	/*-DEFINE-------------------------------------------------------*/
    	#define CCIR_OUTPUT_YUV422_FORMAT	    0x00
    	#define CCIR_OUTPUT_YUV444_FORMAT	    0x01
    	#define CCIR_OUTPUT_RGB888_FORMAT	    0x02
    	#define CCIR_OUTPUT_RGB555_FORMAT	    0x03
    	#define CCIR_OUTPUT_RGB565_FORMAT	    0x04
    	#define CCIR_OUTPUT_RGB444_FORMAT	    0x05
    	#define CCIR_OUTPUT_BAYER_FORMAT	    0x06
    	/*--------------------------------------------------------------*/
	AIT_REG_B	CCIR_PREVIEW_YUV_SEQ;			        //0x2D
    	/*-DEFINE-------------------------------------------------------*/
    	#define CCIR_422_YUYV				    0x00
    	#define CCIR_422_YVYU				    0x01
    	#define CCIR_422_UYVY				    0x02
    	#define CCIR_422_VYUY				    0x03
    	#define CCIR_444_YUV				    0x00
    	#define CCIR_444_YVU				    0x04
    	#define CCIR_444_UVY				    0x08
    	#define CCIR_444_VUY				    0x0C
    	/*--------------------------------------------------------------*/
	AIT_REG_B	CCIR_PREVIEW_RGB_SEQ;			        //0x2E
    	/*-DEFINE-------------------------------------------------------*/
    	#define CCIR_RGB555_RGB_RALIGN	        0x00
    	#define CCIR_RGB555_BGR_RALIGN		    0x01
    	#define CCIR_RGB555_RGB_LALIGN		    0x02
    	#define CCIR_RGB555_BGR_LALIGN		    0x03
    	#define CCIR_RGB444_RGB_RALIGN		    0x00
    	#define CCIR_RGB444_BGR_RALIGN		    0x04
    	#define CCIR_RGB444_RGB_LALIGN		    0x08
    	#define CCIR_RGB444_BGR_LALIGN		    0x0C
    	#define CCIR_RGB565_RGB_FORMAT		    0x00
    	#define CCIR_RGB565_BGR_FORMAT		    0x10
    	#define CCIR_RGB888_RGB_FORMAT		    0x00
    	#define CCIR_RGB888_BGR_FORMAT			0x20
    	/*--------------------------------------------------------------*/
	AIT_REG_B	CCIR_PREVIEW_BAYER_SEQ;			        //0x2F
    	/*-DEFINE-------------------------------------------------------*/
    	#define CCIR_BAYER_RG_GB			    0x00
    	#define CCIR_BAYER_GB_RG			    0x01
    	#define CCIR_BAYER_GR_BG		        0x02
    	#define CCIR_BAYER_BG_GR			    0x03
    	/*--------------------------------------------------------------*/
    	
	AIT_REG_W	CCIR_LINE_PIXL_NUM;			            //0x30
	AIT_REG_W	CCIR_LINE_NUMBER;			            //0x32
	AIT_REG_W	CCIR_CAPTURE_HSYNC_PIXL_NUM;	        //0x34
	AIT_REG_W	CCIR_CAPTURE_LINE_NUM;			        //0x36
	AIT_REG_W	CCIR_FILLER;				            //0x38
	AIT_REG_W	CCIR_PREVIEW_BUF_LINE_OFFSET;	        //0x3A
	AIT_REG_B   CCIR_THUMBNAIL_START_MARK;              //0x3C
	AIT_REG_B   CCIR_THUMBNAIL_END_MARK;                //0x3D
	AIT_REG_B   CCIR_THUMBNAIL_START_LINE;              //0x3E
	AIT_REG_B                           _x3F;
	
	AIT_REG_B	CCIR_CAPTURE_FRAME_EN;			        //0x40
    	/*-DEFINE-------------------------------------------------------*/
    	#define CCIR_CAPTURE_FRAME_ST		    0x01
    	#define CCIR_PREVIEW_FRAME_ST		    0x02
    	/*--------------------------------------------------------------*/
	AIT_REG_B	                        _x41;
	AIT_REG_B	CCIR_BUF_RESET;				            //0x42
    	/*-DEFINE-------------------------------------------------------*/
    	#define CCIR_BUF_SW_RESET_CLAER		    0x00
    	#define CCIR_BUF_SW_RESET			    0x01
    	/*--------------------------------------------------------------*/
	AIT_REG_B	CCIR_CLK_OUT_DELAY_SEL;		            //0x43
	AIT_REG_W	CCIR_VSYNC_ED_CNT;			            //0x44
	AIT_REG_B	CCIR_HSYNC_WAIT_CNT;		            //0x46
    	/*-DEFINE-------------------------------------------------------*/
    	#define CCIR_HSYNC_ST_WT_CNT(VAL)       (VAL)
    	#define CCIR_HSYNC_ED_WT_CNT(VAL)       (VAL << 4)
    	/*--------------------------------------------------------------*/
	AIT_REG_B	CCIR_CAPTURE_SRAM_CFG;	    	        //0x47
	AIT_REG_D	CCIR_1ST_BUF_LO_THR;			        //0x48
	AIT_REG_D	CCIR_1ST_BUF_HI_THR;    	            //0x4C
	
	AIT_REG_B	                        _x50[0x10];
	
	AIT_REG_B	CCIR_SYNC_PULSE_WIDTH_CNT;		        //0x60
	AIT_REG_B	                        _x61[0x17];
	AIT_REG_D	CCIR_2ND_BUF_LO_THR;			        //0x78
	AIT_REG_D	CCIR_2ND_BUF_HI_THR;			        //0x7C
	
	AIT_REG_B	                        _x80[0x20];
	
	AIT_REG_B	CCIR_HDCCTV_CTL;			            //0xA0
	AIT_REG_B	                        _xA1;
	AIT_REG_B	CCIR_HSYNC_LINE_CNT;		            //0xA2
	AIT_REG_B	                        _xA3;
	AIT_REG_W	CCIR_HSYNC_BLANK_PED_BYTE;              //0xA4
	AIT_REG_W	CCIR_HSYNC_ACT_PED_BYTE;                //0xA6
	AIT_REG_B	CCIR_RAW_TO_CCIR_PT_SEL;                //0xA8
	AIT_REG_B	                        _xA9[7]; 

	AIT_REG_W	CCIR_LINE_PXL_NUM_SR;                   //0xB0
	AIT_REG_W	CCIR_LINE_NUM_SR;                       //0xB2
	AIT_REG_B	                        _xB4[0x4C]; 

	AIT_REG_B	CCIR_PED_MODE_SELECT;			        //0x100
    	/*-DEFINE-------------------------------------------------------*/
    	#define	CCIR_PED_GEN_TIME_SIG_START	    0x01
    	#define	CCIR_PED_OUT_BT656			    0x02
    	#define	CCIR_PED_OUT_BT1120			    0x04
    	#define CCIR_SW_MODE_TRIG   		    0x10
    	#define CCIR_HW_MODE_TRIG   		    0x00
    	#define CCIR_FRM_SYNC_DIS   		    0x20
    	#define CCIR_PED_SHARE_MIPI_TX_SRAM_EN  0x40
    	#define CCIR_PED_MCI_BYTE_CNT_256	    0x80
    	#define CCIR_PED_MCI_BYTE_CNT_128	    0x00
    	/*--------------------------------------------------------------*/
	AIT_REG_B	CCIR_PED_PIXEL_CLK_DIV;		            //0x101
    	/* PCLK = (pixel_clk_divisor+1)*system clk */
    AIT_REG_B	CCIR_PED_CTL_1;                         //0x102
    	/*-DEFINE-------------------------------------------------------*/
    	#define CCIR_DDR_MOD_CLK_INV            0x20
    	#define CCIR_DDR_MOD_EN                 0x10
    	#define CCIR_PED_VSYNC_CLK_INV          0x08
    	#define CCIR_PED_HSYNC_CLK_INV          0x04
    	#define CCIR_PED_DATA_EN_INV            0x02
    	#define CCIR_PED_PIXL_CLK_INV           0x01
    	/*--------------------------------------------------------------*/
    AIT_REG_B	CCIR_PED_DATA_FMT;                      //0x103
    	/*-DEFINE-------------------------------------------------------*/
    	#define CCIR_OUTPUT_FMT_YUV422_8BIT	    0x01
    	#define CCIR_OUTPUT_FMT_YUV422_16BIT    0x02
    	#define CCIR_OUTPUT_FMT_RGB565_16BIT    0x04
    	/*--------------------------------------------------------------*/
    AIT_REG_B	CCIR_PED_DATA_ORDER;	                //0x104
    	/*-DEFINE-------------------------------------------------------*/
    	#define CCIR_RGB565_RGB			        0x00
    	#define CCIR_RGB565_BGR			        0x04
    	#define CCIR_YUV422_16BIT_UYVY		    0x00
    	#define CCIR_YUV422_16BIT_VYUY		    0x01
    	#define CCIR_YUV422_16BIT_YUYV		    0x02
    	#define CCIR_YUV422_16BIT_YVYU		    0x03
    	#define CCIR_YUV422_8BIT_UYVY		    0x00
    	#define CCIR_YUV422_8BIT_VYUY		    0x01
    	#define CCIR_YUV422_8BIT_YUYV		    0x02
    	#define CCIR_YUV422_8BIT_YVYU		    0x03
    	/*--------------------------------------------------------------*/
	AIT_REG_B                           _x105[3];
	AIT_REG_B	CCIR_PED_SRM_CFG;		                //0x108	
	AIT_REG_B                           _x109[3];
	AIT_REG_W	CCIR_PED_DELAY_CNT;			            //0x10C
	AIT_REG_B                           _x10E[2];
	
	AIT_REG_B   CCIR_PED_CPU_INT_EN;                    //0x110
    	/*-DEFINE-------------------------------------------------------*/
    	#define CCIR_PED_V_FRONT_PORCH_END_EN       0x01
    	#define CCIR_PED_FIELD1_BOT_BLANK_END_EN    0x02
    	#define CCIR_PED_FIELD2_BOT_BLANK_END_EN    0x04
    	#define CCIR_PED_V_SYNC_START_EN            0x08
    	#define CCIR_PED_H_SYNC_START_EN            0x10
    	#define CCIR_PED_LINE_CNT_REACH_EN          0x20
    	#define CCIR_PED_LINE_CNT1_REACH_EN         0x40
    	/*--------------------------------------------------------------*/
	AIT_REG_B   CCIR_PED_CPU_INT_EN1;                   //0x111
    	/*-DEFINE-------------------------------------------------------*/
    	#define CCIR_PED_IBC_PRE_BUF_OVF_EN     0x01
    	#define CCIR_PED_IBC_PRE_BUF_UDF_EN     0x02
    	#define CCIR_PED_IBC_FRAME_ST_EN        0x04
    	#define CCIR_PED_IBC_FRAME_ED_EN        0x08
    	#define CCIR_PED_FIFO_UDF               0x10
    	/*--------------------------------------------------------------*/
	AIT_REG_B                           _x112[0x02];
	AIT_REG_B   CCIR_PED_CPU_INT_SR;                    //0x114
	AIT_REG_B   CCIR_PED_CPU_INT_SR1;                   //0x115
	AIT_REG_B                           _x116[0x0A];
	
	AIT_REG_W	CCIR_PED_H_ACT_PIXL_CNT;		        //0x120
	AIT_REG_B	CCIR_PED_H_WIDTH_CNT;    		        //0x122
	AIT_REG_B                           _x123;
	AIT_REG_W	CCIR_PED_H_BACK_PORCH;    		        //0x124
	AIT_REG_W	CCIR_PED_H_FRONT_PORCH;    		        //0x126
	AIT_REG_W	CCIR_PED_H_TOTAL_BLANK;    		        //0x128
	AIT_REG_B                           _x12A[0x6];
	
	AIT_REG_W	CCIR_PED_V_ACT_LINE_CNT;		        //0x130
	AIT_REG_B	CCIR_PED_V_WIDTH_CNT;    		        //0x132
	AIT_REG_B                           _x133;
	AIT_REG_W	CCIR_PED_V_BACK_PORCH;    		        //0x134
	AIT_REG_W	CCIR_PED_V_FRONT_PORCH;    		        //0x136
	AIT_REG_B                           _x138[0x8];
	
	AIT_REG_D	CCIR_PED_IBC_BUF_ST_ADDR;    	        //0x140
	AIT_REG_D	CCIR_PED_IBC_BUF_ED_ADDR;    	        //0x144
	AIT_REG_W	CCIR_PED_LINE_INT_CNT0;    	            //0x148
	AIT_REG_W	CCIR_PED_LINE_INT_CNT1;    	            //0x14A
	AIT_REG_B                           _x14C[0x4];
	
	AIT_REG_B	CCIR_PED_BT656_1120_FILD1_TOP_V_BLANK;  //0x150
	AIT_REG_B	CCIR_PED_BT656_1120_FILD1_BOT_V_BLANK;  //0x151
	AIT_REG_B	CCIR_PED_BT656_1120_FILD2_TOP_V_BLANK;  //0x152
	AIT_REG_B	CCIR_PED_BT656_1120_FILD2_BOT_V_BLANK;  //0x153
	AIT_REG_B	CCIR_PED_BT656_1120_FILD1_USER_TOP_EAV; //0x154
	AIT_REG_B	CCIR_PED_BT656_1120_FILD1_USER_TOP_SAV; //0x155
	AIT_REG_B	CCIR_PED_BT656_1120_FILD1_USER_ACT_EAV; //0x156
	AIT_REG_B	CCIR_PED_BT656_1120_FILD1_USER_ACT_SAV; //0x157
	AIT_REG_B	CCIR_PED_BT656_1120_FILD1_USER_BOT_EAV; //0x158
	AIT_REG_B	CCIR_PED_BT656_1120_FILD1_USER_BOT_SAV; //0x159
	AIT_REG_B	CCIR_PED_USER_DEF_EAV_SAV_EN;           //0x15A
    	/*-DEFINE-------------------------------------------------------*/
    	#define CCIR_PED_USER_DEF_EAV_SAV_ENABLE    0x01
    	/*--------------------------------------------------------------*/	
	AIT_REG_B                           _x15B[0x5];
	
	AIT_REG_W	CCIR_PED_V_CUR_LINE_CNT;                //0x160 [RO]
	AIT_REG_W	CCIR_PED_H_CUR_PIXL_CNT;                //0x162 [RO]
	AIT_REG_W	CCIR_PED_1ST_FIFO_SR;                   //0x164 [RO]
    	/*-DEFINE-------------------------------------------------------*/
    	#define CCIR_1ST_FIFO_FULL              0x02
    	#define CCIR_1ST_FIFO_EMPTY             0x00
    	/*--------------------------------------------------------------*/
    AIT_REG_B                           _x166[0xA];
    	
} AITS_CCIR, *AITPS_CCIR;
#endif

/// @}
#endif // _MMP_REG_CCIR_H_
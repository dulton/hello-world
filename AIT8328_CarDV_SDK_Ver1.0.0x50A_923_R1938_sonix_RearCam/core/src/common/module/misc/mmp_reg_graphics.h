//==============================================================================
//
//  File        : mmp_reg_graphics.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_REG_GRAPHICS_H_
#define _MMP_REG_GRAPHICS_H_

#include "mmp_register.h"

/** @addtogroup MMPH_reg
@{
*/

#if (CHIP == MCR_V2)
//--------------------------------
// GRA BLT Structure (0x8000 5000)
// GRA SCA Structure (0x8000 7900)
//--------------------------------
typedef struct _AITS_GRA 
{
    AIT_REG_B   GRA_BLT_EN;										// 0x00
        /*-DEFINE-----------------------------------------------------*/
        #define GRA_BLT_ST                  0x01
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x01;
    AIT_REG_B   GRA_BLT_FIFO_RST;								// 0x02
        /*-DEFINE-----------------------------------------------------*/
        #define GRA_BLT_FIFO_RESET          0x01
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x03;
    AIT_REG_B   GRA_PAL_SWAP;									// 0x04
        /*-DEFINE-----------------------------------------------------*/
        #define GRA_PAL_SWAP_EN          	0x01
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x05[0x3];
    AIT_REG_W   GRA_BLT_INT_CPU_EN;								// 0x08
    AIT_REG_W   GRA_BLT_INT_CPU_SR;								// 0x0A
    AIT_REG_W   GRA_BLT_INT_HOST_EN;							// 0x0C
    AIT_REG_W   GRA_BLT_INT_HOST_SR;							// 0x0E
		/*-DEFINE-----------------------------------------------------*/
        #define GRA_FIFO_EMPTY              0x0001
		#define GRA_FIFO_FULL               0x0002
		#define GRA_FIFO_WR_TH              0x0004
		#define GRA_FIFO_RD_TH              0x0008
		#define GRA_BLT_DONE                0x0010
		/*------------------------------------------------------------*/

    AIT_REG_D   GRA_BLT_SRC_ADDR;                      			// 0x10
    AIT_REG_D   GRA_BLT_SRC_PITCH;								// 0x14
    AIT_REG_D   GRA_BLT_DST_ADDR;								// 0x18
    AIT_REG_D   GRA_BLT_DST_PITCH;								// 0x1C

    AIT_REG_D   GRA_BLT_PAT_ADDR;                               // 0x20
    AIT_REG_D   GRA_BLT_PAT_PITCH;								// 0x24
    AIT_REG_B                           _x28[0x8];

    AIT_REG_W   GRA_BLT_FIFO_DATA;                              // 0x30
    AIT_REG_B                           _x32[0x2];
    AIT_REG_W   GRA_BLT_FIFO_THD;                              	// 0x34
	AIT_REG_B                           _x36[0x2];
    AIT_REG_B   GRA_BLT_FIFO_SR;								// 0x38
        /*-DEFINE-----------------------------------------------------*/
        #define GRA_FIFO_EPY                0x01
        #define GRA_FIFO_FUL                0x02
        #define GRA_FIFO_WR_U_TH            0x04
        #define GRA_FIFO_RD_O_TH            0x08
        /*------------------------------------------------------------*/
	AIT_REG_B   	                    _x39;
    AIT_REG_B   GRA_BLT_FIFO_WR_SPC;							// 0x3A
		/*-DEFINE-----------------------------------------------------*/
		#define GRA_FIFO_MASK               0x1F
		/*------------------------------------------------------------*/
    AIT_REG_B   GRA_BLT_FIFO_RD_SPC;							// 0x3B
    AIT_REG_B                           _x3C[0x4];

    AIT_REG_W   GRA_BLT_W;                                    	// 0x40
    AIT_REG_W   GRA_BLT_H;										// 0x42
    AIT_REG_D   GRA_BLT_FG;										// 0x44
    AIT_REG_D   GRA_BLT_BG;										// 0x48
    AIT_REG_W   GRA_BLT_PAT_W;									// 0x4C
    AIT_REG_W   GRA_BLT_PAT_H;									// 0x4E

    AIT_REG_B   GRA_BLT_FMT;                                   	// 0x50
        /*-DEFINE-----------------------------------------------------*/
        #define	GRA_BIT_DAT_CON			0x80
        #define	GRA_BIT_DAT_NOT_CON		0x00
        #define	GRA_BIT_DAT_M_2_L		0x40
        #define	GRA_BIT_DAT_L_2_M		0x00
        #define GRA_CLR_FMT_8BPP		0x00
        #define GRA_CLR_FMT_16BPP		0x01
        #define GRA_CLR_FMT_24BPP		0x02
        #define GRA_CLR_FMT_MASK        0x03
        #define GRA_PAT_CLR_FMT(f)		( (f & GRA_CLR_FMT_MASK) << 4)
        #define GRA_DES_CLR_FMT(f)		( (f & GRA_CLR_FMT_MASK) << 2)
        #define GRA_SRC_CLR_FMT(f)		( (f & GRA_CLR_FMT_MASK) << 0)
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x51;
    AIT_REG_B   GRA_BLT_ROP;									// 0x52
        /*-DEFINE-----------------------------------------------------*/
        #define	GRA_ROP_BLACKNESS		0x00
        #define	GRA_ROP_NOTSRCERASE		0x01	// ~(S+D)
        #define GRA_ROP_NOTSRCCOPY		0x03	// ~S
        #define	GRA_ROP_SRCERASE		0x04	// S.~D
        #define	GRA_ROP_DSTINVERT		0x05	// ~D
        #define GRA_ROP_SRCINVERT		0x06	// S^D
        #define GRA_ROP_SRCAND			0x08	// S.D
        #define GRA_ROP_MERGEPAINT		0x0B	// ~S+D
        #define	GRA_ROP_SRCCOPY			0x0C	// S
        #define	GRA_ROP_SRCPAINT		0x0E	// S+D
        #define	GRA_ROP_WHITENESS		0x0F
        #define	GRA_ROP_MASK			0x0F
        /*------------------------------------------------------------*/
    AIT_REG_B   GRA_BLT_ROP_CTL;								// 0x53
        /*-DEFINE-----------------------------------------------------*/
        #define	GRA_DO_ROP				0x00
        #define	GRA_DO_ROP_WITH_TP		0x01
        #define	GRA_DO_CLR_EXPEND		0x02
        #define GRA_DO_CLR_EXPEND_WITH_TP 0x03
        #define	GRA_MEM_2_HOST			0x00
        #define	GRA_HOST_2_MEM			0x04
        #define	GRA_MEM_2_MEM			0x08
        #define	GRA_PAT_2_DISPLAY		0x0C
        #define	GRA_SOLID_FILL			0x10
        #define	GRA_DRAW_PIXL			0x14
        #define	GRA_LEFT_TOP_RIGHT		0x00
        #define	GRA_RIGHT_TOP_LEFT		0x20
        #define	GRA_LEFT_BOT_RIGHT		0x40
        #define	GRA_RIGHT_BOT_LEFT		0x60
        /*------------------------------------------------------------*/
    AIT_REG_B   GRA_BLT_BLD_CTL;								// 0x54
        /*-DEFINE-----------------------------------------------------*/
        #define GRA_BLT_BLD_EN          0x80
        #define GRA_BLT_BLD_DST_WT_MASK 0x0F
        /*------------------------------------------------------------*/
    AIT_REG_B   GRA_BLT_BLD_SRC_WT;								// 0x55
        /*-DEFINE-----------------------------------------------------*/
        #define GRA_BLT_BLD_SRC_WT_MASK 0x1F
        /*------------------------------------------------------------*/
    AIT_REG_W   GRA_BLT_PAT_ST;									// 0x56
    AIT_REG_B   GRA_LINE_DRAW_CTL;								// 0x58
    AIT_REG_B                           _x59[0x3];
    AIT_REG_B   GRA_ROTE_CTL;									// 0x5C
        /*-DEFINE-----------------------------------------------------*/
        #define GRA_ROTE_MODE	            0x01
        #define GRA_LINEOST_POS             0x02
        #define GRA_LINEOST_NEG             0x00
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x5D;
    AIT_REG_W   GRA_ROTE_PIXL_OFFSET;							// 0x5E
        /*-DEFINE-----------------------------------------------------*/
        #define GRA_PIXOFST_POS             0x8000
        #define GRA_PIXOFST_NEG             0x0000
        /*------------------------------------------------------------*/
    
    AIT_REG_W   GRA_PROBE_SEL;									// 0x60
    AIT_REG_W   GRA_FIFO_PTR;									// 0x62
    AIT_REG_W   GRA_LEFT_PIXL_CNT;								// 0x64
    AIT_REG_W   GRA_LINE_CNT;									// 0x66
	AIT_REG_B                           _x68[0x8];
	
	AIT_REG_B							_x70[0x2890];
	
	//GRA SCA structure (0x8000 7900)
    AIT_REG_B   GRA_SCAL_EN;                                    // 0x00
        /*-DEFINE-----------------------------------------------------*/
        #define GRA_SCAL_ST                 0x01
        #define GRA_YUV420_SCAL_ST          0x02
        #define GRA_YUV420_INTERLEAVE       0x04
        #define GRA_YUV420_NV21             0x08
        #define GRA_YUV420_NV12             0x00
        /*------------------------------------------------------------*/
    AIT_REG_B   GRA_SCAL_FLOW_CTL;                           	// 0x01
        /*-DEFINE-----------------------------------------------------*/
        #define GRA_SCAL_DLY_CHK_LN_END     0x00
        #define GRA_SCAL_DLY_CHK_SCA_BUSY   0x01
        #define GRA_SCAL_DLY_CTL_MASK       0x01
        #define GRA_SCAL_Y_ONLY             0x02
        #define GRA_SCAL_FRM_BASE_EN        0x04
        #define GRA_SCAL_URGENT_REQ_EN      0x08
        #define GRA_SCAL_URGENT_THD_MASK	0xF0
        /*------------------------------------------------------------*/
    AIT_REG_B   GRA_SCAL_FMT;                            		// 0x02
        /*-DEFINE-----------------------------------------------------*/
        #define GRA_SCAL_MEM_RGB565         0x00
        #define GRA_SCAL_MEM_RGB888         0x01
        #define GRA_SCAL_FIFO_RGB565        0x02
        #define GRA_SCAL_FIFO_YUV422        0x03
        #define GRA_SCAL_MEM_YUV422         0x40
        #define GRA_SCAL_SRC_FMT_MASK       0x43
        #define GRA_SCAL_FIFO_MODE          0x02
        #define GRA_SCAL_Y_LOW              0x04
        #define GRA_SCAL_CR_FIRST           0x08
        #define GRA_SCAL_SRC_YUV422_UYVY    0x00
        #define GRA_SCAL_SRC_YUV422_VYUY    0x10
        #define GRA_SCAL_SRC_YUV422_YUYV    0x20
        #define GRA_SCAL_SRC_YUV422_YVYU    0x30
        #define GRA_SCAL_RGB_SWAP           0x80
        /*------------------------------------------------------------*/
    AIT_REG_B   GRA_SCAL_UP_FACT;                          		// 0x03
        /*-DEFINE-----------------------------------------------------*/
        #define GRA_SCALUP_MASK             0x1F
        #define GRA_SCALUP(_a)              (_a & GRA_SCALUP_MASK)
       	#define GRA_SCAL_MCI_256_BYTE     	0x00
        #define GRA_SCAL_MCI_128_BYTE     	0x80
        /*------------------------------------------------------------*/
    AIT_REG_W   GRA_SCAL_FIFO_RST;                             	// 0x04
        /*-DEFINE-----------------------------------------------------*/
        #define GRA_SCAL_FIFO_RESET         0x0001
        #define GRA_SCAL_FIFO_ASYNC_RST     0x0100
        /*------------------------------------------------------------*/
    AIT_REG_B   GRA_SCAL_SRAM_Y_DRIVING;						// 0x06
    AIT_REG_B   GRA_SCAL_SRAM_UV_DRIVING;						// 0x07
    AIT_REG_W   GRA_SCAL_INT_CPU_EN;                       		// 0x08
    AIT_REG_W   GRA_SCAL_INT_CPU_SR;							// 0x0A
    AIT_REG_W   GRA_SCAL_INT_HOST_EN;							// 0x0C
    AIT_REG_W   GRA_SCAL_INT_HOST_SR;							// 0x0E
		/*-DEFINE-----------------------------------------------------*/
        #define GRA_FIFO_EMPTY              0x0001
		#define GRA_FIFO_FULL               0x0002
		#define GRA_FIFO_WR_TH              0x0004
		#define GRA_FIFO_RD_TH              0x0008
		#define GRA_YUV420_SCAL_DONE        0x0010		
		#define GRA_SCAL_DONE               0x0020
		#define GRA2SCAL_FRM_ST             0x0040	
		#define GRA2SCAL_FRM_END            0x0080
		/*------------------------------------------------------------*/

    AIT_REG_D   GRA_SCAL_ADDR_Y_ST;                        		// 0x10
    AIT_REG_D   GRA_SCAL_ADDR_U_ST;                      		// 0x14
    AIT_REG_D   GRA_SCAL_ADDR_V_ST;                      		// 0x18
    AIT_REG_W   GRA_SCAL_W;                                 	// 0x1C
    AIT_REG_W   GRA_SCAL_H;                                		// 0x1E
    
    AIT_REG_B   GRA_SCAL_PIXL_DLY;                         		// 0x20
        /*-DEFINE-----------------------------------------------------*/
        #define GRA_SCAL_PIXL_DLY_MASK      0x1F
        #define GRA_SCAL_SET_PIX_DELAY(_a)	(_a - 1)
        /*------------------------------------------------------------*/    
    AIT_REG_B   GRA_SCAL_PIXL_DLY_N;							// 0x21
    AIT_REG_W   GRA_SCAL_LINE_DLY;                          	// 0x22
    AIT_REG_W   GRA_SCAL_FIFO_DATA;                      		// 0x24
    AIT_REG_B                           _x126[0x2];
    AIT_REG_B   GRA_SCAL_FIFO_WR_TH;                           	// 0x28
    AIT_REG_B   GRA_SCAL_FIFO_RD_TH;                        	// 0x29
    AIT_REG_B                           _x12A[0x2];
    AIT_REG_B   GRA_SCAL_FIFO_SR;                              	// 0x2C
	AIT_REG_B   	                    _x12D; 
    AIT_REG_B   GRA_SCAL_FIFO_WR_SPC;                         	// 0x2E
    AIT_REG_B   GRA_SCAL_FIFO_RD_SPC;							// 0x2F
    
    AIT_REG_D   GRA_SCAL_ADDR_Y_OFST;                           // 0x30
    AIT_REG_D   GRA_SCAL_ADDR_U_OFST;                           // 0x34
    AIT_REG_D   GRA_SCAL_ADDR_V_OFST;                           // 0x38
	AIT_REG_B                           _x13C[0x4];

    AIT_REG_B   GRA_SCAL_FIFO_PTR;                              // 0x40
	AIT_REG_B                           _x141[0xF];
} AITS_GRA, *AITPS_GRA;
#endif

#endif // _MMP_REG_GRAPHICS_H_

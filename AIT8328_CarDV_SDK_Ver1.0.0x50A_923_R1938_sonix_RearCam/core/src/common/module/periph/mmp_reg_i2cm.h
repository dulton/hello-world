//==============================================================================
//
//  File        : mmp_reg_i2cm.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_REG_I2CM_H_
#define _MMP_REG_I2CM_H_

#include "mmp_register.h"

#if (CHIP == MCR_V2)
//--------------------------------
// I2CM structure (0x8000 7A00)
//--------------------------------
typedef struct _AITS_I2CMS {
    AIT_REG_B   I2CM_CTL;                                               // 0x00
        /*-DEFINE-----------------------------------------------------*/
        #define I2CM_MASTER_EN              0x01
        #define I2CM_REG_16BIT_MODE         0x02
        #define I2CM_REG_8BIT_MODE          0x00
        #define I2CM_SDA_OH_MODE            0x04
        #define I2CM_SDA_OD_MODE            0x00
        #define I2CM_SCK_OH_MODE            0x00
        #define I2CM_SCK_OD_MODE            0x08
        #define I2CM_STOP_IF_NOACK          0x00
        #define I2CM_CONTI_IF_NOACK         0x10
        #define I2CM_SLAVE_ACK_MODE_EN		0x20
        #define I2CM_RFCL_MODE_EN		    0x40
        #define I2CM_REPEAT_MODE_EN		    0x80
         /*------------------------------------------------------------*/
    AIT_REG_B	I2CM_SLAVE_ADDR;									    // 0x01
    	/*-DEFINE-----------------------------------------------------*/
        #define I2CM_WRITE_MODE             0x80
        #define I2CM_READ_MODE              0x00
        #define I2CM_RW_MODE_MASK           0x80
        /*------------------------------------------------------------*/
    AIT_REG_B	I2CM_DMA_MODE_CTRL;								        // 0x02
    	/*-DEFINE-----------------------------------------------------*/
        #define I2CM_DMA_TX_DESCP_MODE	    0x00
        #define I2CM_DMA_TX_BURST_MODE	    0x10
        #define I2CM_DMA_RX_STOP_IF_OVF	    0x08
        #define I2CM_DMA_TX_STOP_IF_UDF	    0x04
        #define I2CM_DMA_RX_EN			    0x02
        #define I2CM_DMA_TX_EN			    0x01
        /*------------------------------------------------------------*/
    AIT_REG_B	I2CM_SLAVE_ADDR1;										// 0x03
    AIT_REG_W   I2CM_SCK_DIVIDER;                                       // 0x04
    AIT_REG_B   I2CM_DATA_HOLD_CNT;                                     // 0x06
    AIT_REG_B   I2CM_CMDSET_WAIT_CNT;                                   // 0x07
    	/*-DEFINE-----------------------------------------------------*/
    	#define I2CM_SET_WAIT_CNT_MASK      0x0F
    	#define I2CM_SET_WAIT_CNT(_a)       ((_a - 1) & I2CM_SET_WAIT_CNT_MASK)
    	#define I2CM_DELAY_WAIT_EN			0x20
    	#define I2CM_INPUT_FILTERN_DIS		0x40
    	#define I2CM_10BIT_SLAVEADDR_EN	    0x80
    	#define I2CM_7BIT_SLAVEADDR_EN      0x00
    	/*------------------------------------------------------------*/
    AIT_REG_W   I2CM_INT_HOST_EN;                                       // 0x08
    AIT_REG_W   I2CM_INT_HOST_SR;                                       // 0x0A
    AIT_REG_W   I2CM_INT_CPU_EN;                                        // 0x0C
    AIT_REG_W   I2CM_INT_CPU_SR;                                        // 0x0E
        /*-DEFINE-----------------------------------------------------*/
        #define I2CM_TX_DONE                0x0001
        #define I2CM_ONE_SET_TX_DONE        0x0002
        #define I2CM_SLAVE_NO_ACK           0x0004
    	#define I2CM_DMA_TX2FIFO_DONE		0x0008
    	#define I2CM_DMA_RX2MEM_DONE		0x0010
    	#define I2CM_DMA_TXFIFO_UDF 		0x0020
        #define I2CM_DMA_RXFIFO_OVF	    	0x0040
        #define I2CM_TXFIFO_EMPTY           0x0100
        #define I2CM_TXFIFO_LE_THD          0x0200
        #define I2CM_TXFIFO_FULL            0x0400
        #define I2CM_RXFIFO_EMPTY           0x0800
        #define I2CM_RXFIFO_GE_THD          0x1000
        #define I2CM_RXFIFO_FULL            0x2000
        #define I2CM_TRXFIFO_UDFOVF         0x4000
        /*------------------------------------------------------------*/
        
    union {                                                             // 0x10
    AIT_REG_B   B[4];
    AIT_REG_W   W[2];
    AIT_REG_D   D[1];
    }           I2CM_TXFIFO_DATA;
    union {                                                             // 0x14
    AIT_REG_B   B[4];
    AIT_REG_W   W[2];
    AIT_REG_D   D[1];
    }           I2CM_RXFIFO_DATA;
    
    AIT_REG_B   I2CM_TXFIFO_THD;										// 0x18
    AIT_REG_B   I2CM_TXFIFO_SPC;										// 0x19
    AIT_REG_B   I2CM_RXFIFO_THD;	    								// 0x1A
    AIT_REG_B   I2CM_RXFIFO_SPC;    									// 0x1B
        /*-DEFINE-----------------------------------------------------*/
        #define I2CM_FIFO_THD_MASK          0x1F
        /*------------------------------------------------------------*/
    AIT_REG_B   I2CM_RST_FIFO_SW;                                       // 0x1C
        /*-DEFINE-----------------------------------------------------*/
        #define I2CM_FIFO_RST               0x01
        /*------------------------------------------------------------*/
    AIT_REG_B   I2CM_DELAY_WAIT_CYC;									// 0x1D									
    AIT_REG_B   I2CM_CLK_STRETCH_EN;									// 0x1E
    	/*-DEFINE-----------------------------------------------------*/
        #define I2CM_STRETCH_ENABLE         0x01
        /*------------------------------------------------------------*/
    AIT_REG_B	                        _x1F;
    
    AIT_REG_W   I2CM_SCL_DUTY_CNT;                                      // 0x20
    AIT_REG_W   I2CM_SET_CNT;                                           // 0x22
    AIT_REG_B	I2CM_EDDC_SLAVE_ADDR;									// 0x24
        /*-DEFINE-----------------------------------------------------*/
        #define I2CM_EDDC_ENABLE            0x80
        /*------------------------------------------------------------*/
    AIT_REG_B	                        _x25[0xB];
    
    AIT_REG_W	I2CM_DMA_MODE_REG_ADDR;							        // 0x30
    AIT_REG_B	I2CM_DMA_EDDC_MODE_ADDR;								// 0x32
    AIT_REG_B	                        _x33[0xD];
    
    AIT_REG_D	I2CM_DMA_TX_ST_ADDR;									// 0x40
    AIT_REG_W	I2CM_DMA_TX_BYTE_CNT;									// 0x44
    AIT_REG_B	                        _x46[2];
    AIT_REG_D	I2CM_DMA_RX_ST_ADDR;									// 0x48
    AIT_REG_W	I2CM_DMA_RX_BYTE_CNT;									// 0x4C
    AIT_REG_B	                        _x4E[2];
    
    AIT_REG_B	I2CM_GBL_CTRL;											// 0x50
        /*-DEFINE-----------------------------------------------------*/
        #define I2CM_GBL_MODE_EN            0x01
        /*------------------------------------------------------------*/
    AIT_REG_B	                        _x51[0x2F];
    
} AITS_I2CMS, *AITPS_I2CMS;

typedef struct _AITS_I2CM {
	AITS_I2CMS    I2CMS[4];
} AITS_I2CM, *AITPS_I2CM;

#endif

#endif // _MMP_REG_I2CM_H_
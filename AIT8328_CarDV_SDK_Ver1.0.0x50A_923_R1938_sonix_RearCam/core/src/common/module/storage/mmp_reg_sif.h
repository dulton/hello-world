//==============================================================================
//
//  File        : mmp_reg_sif.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_REG_SIF_H_
#define _MMP_REG_SIF_H_

#include "mmp_register.h"

/** @addtogroup MMPH_reg
@{
*/

// *****************************************************************************
//   SIF structure
//   MCR_V2:    (0x8000 6700)
// *****************************************************************************
typedef struct _AITS_SIF {
    AIT_REG_B   SIF_INT_CPU_EN;                 //0x00
    AIT_REG_B   _x01[3];
        /*-DEFINE-----------------------------------------------------*/
        // Refer to Offset 0x0C
        /*------------------------------------------------------------*/
    AIT_REG_B   SIF_INT_HOST_EN;                //0x04
    AIT_REG_B   _x05[3];
        /*-DEFINE-----------------------------------------------------*/
        // Refer to Offset 0x0C
        /*------------------------------------------------------------*/
    AIT_REG_B   SIF_INT_CPU_SR;                 //0x08
    AIT_REG_B   _x09[3];
        /*-DEFINE-----------------------------------------------------*/
        // Refer to Offset 0x0C
        /*------------------------------------------------------------*/
    AIT_REG_B   SIF_INT_HOST_SR;                //0x0C
    AIT_REG_B   _x0D[3];
        /*-DEFINE-----------------------------------------------------*/
        #define SIF_CMD_DONE                0x01
        #define SIF_AAI_CMD_DONE            0x02
        #define SIF_CLR_CMD_STATUS          0x01
        #define SIF_CLR_AAI_CMD_STATUS      0x02
        /*------------------------------------------------------------*/
    AIT_REG_B   SIF_CTL;                        //0x10
        /*-DEFINE-----------------------------------------------------*/
        #define SIF_START                   0x80
        #define SIF_FAST_READ               0x40
        #define SIF_R                       0x20
        #define SIF_W                       0x00
        #define SIF_DMA_EN                  0x10
        #define SIF_DATA_EN                 0x08
        #define SIF_ADDR_EN                 0x04
        #define SIF_ADDR_LEN_3        		0x02
        #define SIF_ADDR_LEN_2              0x01
        #define SIF_ADDR_LEN_1              0x00 
        /*------------------------------------------------------------*/
    AIT_REG_B   SIF_CTL2;                   //0x11
        /*-DEFINE-----------------------------------------------------*/
        #define SIF_DUAL_MODE               0x10 // change from SIF_QUAL_MODE to SIF_DUAL_MODE
        #define SIF_QUAD_MODE               0x08 // change from 0x80 to 0x08
        #define SIF_AUTO_LOAD_MASK_DIS      0x00
        #define SIF_AUTO_LOAD_MASK_EN       0x02
        #define SIF_AAI_MODE_DIS            0x00
        #define SIF_AAI_MODE_EN             0x01
        /*------------------------------------------------------------*/
    AIT_REG_B   SIF_CLK_DIV;                    //0x12
        // Target clock rate = source clock rate / 2(SIF_CLK_DIV + 1)
        /*-DEFINE-----------------------------------------------------*/
        #define SIF_CLK_DIV_4     			0x01
        #define SIF_CLK_DIV(_a)             (((_a) >> 1) - 1)    // get the original divider index
        /*------------------------------------------------------------*/
    AIT_REG_B   SIF_PD_CPU;                     //0x13
    AIT_REG_B   SIF_CMD;                        //0x14
    AIT_REG_B                       _x15[3];
    AIT_REG_D   SIF_FLASH_ADDR;                 //0x18
    AIT_REG_B   SIF_BIST_EN;                    //0x1C
        /*-DEFINE-----------------------------------------------------*/
        #define SIF_BIST_ENABLE             0x01
        #define SIF_CRC_AUTO_CLEAN_EN       0x02
        /*------------------------------------------------------------*/
    AIT_REG_B   SIF_DATA_IN_LATCH;              //0x1D
    AIT_REG_W   SIF_CRC_OUT;                    //0x1E

    AIT_REG_W   SIF_DATA_WR;                    //0x21
    AIT_REG_B                       _x22[14];
    AIT_REG_B   SIF_DATA_RD;                    //0x30
    AIT_REG_B                       _x31[15];
    AIT_REG_D   SIF_DMA_ST;                     //0x40
    AIT_REG_D   SIF_DMA_CNT;                    //0x44
    AIT_REG_B   SIF_AAI_INTER_CMD_DELAY;        //0x48
    AIT_REG_B                       _x49;
    AIT_REG_B   SIF_PAD_DELAY;                  //0x4A
} AITS_SIF, *AITPS_SIF;

/// @}
#endif // _MMPH_REG_SIF_H_

//==============================================================================
//
//  File        : mmp_register_irda.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Eroy Yang
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_REG_IRDA_H_
#define _MMP_REG_IRDA_H_

#include "mmp_register.h"

/** @addtogroup MMPH_reg
@{
*/

#if (CHIP == MCR_V2)
typedef struct _AITS_IRDA_CMD
{
    AIT_REG_B   IRDA_CMD_FMT_SET;                       // 0x20
        /*-DEFINE-----------------------------------------------------*/
        #define IRDA_CMD_HAVE_1ST_START_BIT     0x01
        #define IRDA_CMD_HAVE_2ND_START_BIT     0x02
        #define IRDA_CMD_HAVE_3RD_START_BIT     0x04
        #define IRDA_CMD_HAVE_4TH_START_BIT     0x08
        #define IRDA_CMD_HAVE_5TH_START_BIT     0x10
        #define IRDA_CMD_HAVE_6TH_START_BIT     0x20
        #define IRDA_CMD_HAVE_7TH_START_BIT     0x40
        #define IRDA_CMD_HAVE_DATA_BIT          0x80
        /*------------------------------------------------------------*/
    AIT_REG_B   IRDA_CMD_BIT_FMT_SET;                   // 0x21
        /*-DEFINE-----------------------------------------------------*/
        #define IRDA_CMD_1ST_START_BIT_MODE1    0x01
        #define IRDA_CMD_1ST_START_BIT_MODE0    0x00
        #define IRDA_CMD_2ND_START_BIT_MODE1    0x02
        #define IRDA_CMD_2ND_START_BIT_MODE0    0x00
        #define IRDA_CMD_3RD_START_BIT_MODE1    0x04
        #define IRDA_CMD_3RD_START_BIT_MODE0    0x00
        #define IRDA_CMD_4TH_START_BIT_MODE1    0x08
        #define IRDA_CMD_4TH_START_BIT_MODE0    0x00
        #define IRDA_CMD_5TH_START_BIT_MODE1    0x10
        #define IRDA_CMD_5TH_START_BIT_MODE0    0x00
        #define IRDA_CMD_6TH_START_BIT_MODE1    0x20
        #define IRDA_CMD_6TH_START_BIT_MODE0    0x00
        #define IRDA_CMD_7TH_START_BIT_MODE1    0x40
        #define IRDA_CMD_7TH_START_BIT_MODE0    0x00
        #define IRDA_CMD_START_BIT_MODE_MASK    0x70
        #define IRDA_CMD_DATA_BIT_MODE1         0x80
        #define IRDA_CMD_DATA_BIT_MODE0         0x00        
        /*------------------------------------------------------------*/
    AIT_REG_W   IRDA_MODE1_START_BIT_LEN;               // 0x22
    AIT_REG_W   IRDA_MODE1_LOG1_LEVEL_LEN;              // 0x24
    AIT_REG_B   IRDA_DATA_BIT_WIDTH;                    // 0x26
        /*-DEFINE-----------------------------------------------------*/
        #define IRDA_DATA_BIT_WIDTH_MASK        0x40
        /*------------------------------------------------------------*/
    AIT_REG_B   IRDA_MODE1_DATA_RANGE;                  // 0x27
        /*-DEFINE-----------------------------------------------------*/
        #define IRDA_DATA_BIT_DROP_NUM_MASK     0x07
        #define IRDA_DATA_BIT_DROP_NUM(_a)      ((_a - 3) & IRDA_DATA_BIT_DROP_NUM_MASK)
        #define IRDA_MODE1_DIFF_LEVEL_H         0x10
        #define IRDA_MODE1_DIFF_LEVEL_L         0x00
        /*------------------------------------------------------------*/
    AIT_REG_W   IRDA_MODE0_DATA_PERIOD;                 // 0x28
    AIT_REG_W   IRDA_MODE0_DATA_LATCH_FAC;              // 0x2A
    AIT_REG_B                   _0x2C[0x34];
                        
} AITS_IRDA_CMD, *AITPS_IRDA_CMD;

//------------------------------
// IRDA Structure (0x8000 5B00)
//------------------------------
typedef struct _AITS_IRDA 
{
    AIT_REG_B   IRDA_EN;                                // 0x00
        /*-DEFINE-----------------------------------------------------*/
        #define IRDA_ENABLE                 0x01
        /*------------------------------------------------------------*/
    AIT_REG_B   IRDA_INPUT_FMT_CTL;                     // 0x01
        /*-DEFINE-----------------------------------------------------*/
        #define IRDA_IN_DATA_INV_EN         0x01
        #define IRDA_START_BIT_HIGH_2_LOW   0x02
        #define IRDA_START_BIT_LOW_2_HIGH   0x00
        #define IRDA_IN_DATA_MSB_FIRST      0x04
        #define IRDA_IN_DATA_LSB_FIRST      0x00
        #define IRDA_DATA2FIFO_INV_EN       0x08
        /*------------------------------------------------------------*/
    AIT_REG_B   IRDA_CLK_DIV;                           // 0x02
        /*-DEFINE-----------------------------------------------------*/
        #define IRDA_CLK_DIV_RATIO(_a)      (_a - 1)
        /*------------------------------------------------------------*/    
    AIT_REG_B                   _0x03;   
    AIT_REG_B   IRDA_INT_CPU_EN;                        // 0x04    
    AIT_REG_B   IRDA_INT_CPU_SR;                        // 0x05 
    AIT_REG_B   IRDA_INT_HOST_EN;                       // 0x06 
    AIT_REG_B   IRDA_INT_HOST_SR;                       // 0x07         
        /*-DEFINE-----------------------------------------------------*/
        #define IRDA_INT_CMD0               0x01
        #define IRDA_INT_CMD1               0x02
        #define IRDA_INT_DATA_PSHOT         0x04
        #define IRDA_INT_DATA_NSHOT         0x08
        #define IRDA_INT_ERR_CMD            0x10                
        /*------------------------------------------------------------*/
    AIT_REG_B                   _0x08[0x18];
    
    AITS_IRDA_CMD   IRDA_CMD[2];                        // 0x20~0x5F, 0x60~0x9F
    
    AIT_REG_D   IRDA_RECEIVED_DATA0;                    // 0xA0
    AIT_REG_D   IRDA_RECEIVED_DATA1;                    // 0xA4
    AIT_REG_B                   _0xA8[0x8];
            
} AITS_IRDA, *AITPS_IRDA;

#endif

#endif // _MMPH_REG_IRDA_H_
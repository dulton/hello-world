//==============================================================================
//
//  File        : mmp_reg_timer.h
//  Description : INCLUDE File for the Timer module register map.
//  Author      : Alterman
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_REG_TIMER_H_
#define _MMP_REG_TIMER_H_

#include "mmp_register.h"

/** @addtogroup MMPH_reg
@{
*/

//------------------------------------------------------
// TC Structure (0xFFFE 0000)
//------------------------------------------------------
typedef struct _AITS_TC {
    AIT_REG_B   TC_CTL;                                         // 0x00
        /*-DEFINE-----------------------------------------------------*/
        #define TC_CLK_EN           0x01
        #define TC_CLK_DIS          0x02
        #define TC_SW_TRIG          0x04
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x01[0x3];
    AIT_REG_W   TC_MODE;                                        // 0x04
        /*-DEFINE-----------------------------------------------------*/
        #define TC_CLK_MCK_D2       0x0000
        #define TC_CLK_MCK_D8       0x0001
        #define TC_CLK_MCK_D32      0x0002
        #define TC_CLK_MCK_D128     0x0003
        #define TC_CLK_MCK_D1024    0x0004
        #define TC_CLK_MCK_D4       0x0005
        #define TC_CLK_MCK_D16      0x0006
        #define TC_CLK_MCK          0x0007
        #define TC_CLK_MCK_MASK		0x0007

        #define TC_COMP_TRIG        0x4000 ///< Auto Reset Counter
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x06[0xA];

    AIT_REG_D   TC_COUNT_VAL;                                   // 0x10
    AIT_REG_B                           _x14[0x8];
    AIT_REG_D   TC_COMP_VAL;                                    // 0x1C

    AIT_REG_D   TC_SR;                                          // 0x20
        /*-DEFINE-----------------------------------------------------*/
        #define TC_CNT_OVF          0x00000001
        #define TC_COMP_VAL_HIT     0x00000010
        #define TC_IS_COUNTING   	0x00010000
        /*------------------------------------------------------------*/
    AIT_REG_D   TC_INT_EN;                                      // 0x24
    AIT_REG_D   TC_INT_DIS;                                     // 0x28
    AIT_REG_D   TC_INT_MASK;                                    // 0x2C

    AIT_REG_B                           _x30[0x10];
} AITS_TC, *AITPS_TC;

typedef struct _AITS_TCB {
    AITS_TC     TC0_2[3];                                       // 0x00~0xBF // TC 0,1,2
    AIT_REG_B   TC_SYNC_CTL;                                    // 0xC0
        /*-DEFINE-----------------------------------------------------*/
        #define TC_SYNC_EN          0x01
        /*------------------------------------------------------------*/
    AIT_REG_B                           _xC1[0x7];
    AIT_REG_B   TC_DBG_MODE;                                    // 0xC8
        /*-DEFINE-----------------------------------------------------*/
        #define TC_DBG_EN           0x01
        /*------------------------------------------------------------*/
    AIT_REG_B                           _xC9[0x37];
    AITS_TC     TC3_5[3];                                       // 0x100~0x1BF // TC 3,4,5
} AITS_TCB, *AITPS_TCB;

/// @}

#endif // _MMP_REG_TIMER_H_
//==============================================================================
//
//  File        : mmp_reg_rtc.h
//  Description : INCLUDE File for the RTC module register map.
//  Author      : Alterman
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_REG_RTC_H_
#define _MMP_REG_RTC_H_

#include "mmp_register.h"

/** @addtogroup MMPH_reg
@{
*/

#if (CHIP == MCR_V2)
//--------------------------------
// RTC structure (0x80003A90)
//--------------------------------
typedef struct _AITS_RTC {
    AIT_REG_W   DIG2RTC_CTL;                                // 0x3A90
        /*-DEFINE-----------------------------------------------------*/
        #define RTC_SET_CNT_VALID           0x0001
        #define RTC_BASE_WR                 0x0002
        #define RTC_BASE_RD                 0x0004
        #define RTC_CNT_RST                 0x0008
        #define RTC_ALARM_WR                0x0010

        #define RTC_CNT_RD                  0x0100
        #define RTC_ALARM_RD                0x0200
        #define RTC_GAIN_CTL_EN             0x0400  // For analog debug
        #define RTC_ALARM_EN                0x0800
        #define RTC_ALARM_INT_CLR           0x1000
        #define RTC_SET_VALID               0x2000
        #define RTC_CLK_OUT_EN              0x4000  // For analog debug
        #define RTC_TMD                     0x8000  // For analog debug
         /*------------------------------------------------------------*/
    AIT_REG_W   RTC_SEQ;                                    // 0x3A92
        /*-DEFINE-----------------------------------------------------*/
        // 1->2->3->4->5->2->1
        #define RTC_EN_SEQ_1                0x0000
        #define RTC_EN_SEQ_2                0x0100
        #define RTC_EN_SEQ_3                0x0300
        #define RTC_EN_SEQ_4                0x0700
        #define RTC_EN_SEQ_5                0x0500
        /*------------------------------------------------------------*/
    AIT_REG_D   RTC_WR_DATA;                                // 0x3A94
    AIT_REG_D   RTC2DIG_CTL;                                // 0x3A98
        /*-DEFINE-----------------------------------------------------*/
        #define RTC_ALARM_EN_STATUS      	0x01
        #define RTC_ALARM_INT_STATUS       	0x02
        #define RTC_CNT_VALID_STATUS    	0x04
        #define RTC_VALID_STATUS      		0x08
        #define RTC_ISO_CTL_ACK				0x10
        /*------------------------------------------------------------*/
    AIT_REG_D   RTC_RD_DATA;                                // 0x3A9C
        /*-DEFINE-----------------------------------------------------*/
        #define RTC_RD_DATA_MASK            0x7FFFFFFF
        #define RTC_CNT_BUSY                0x80000000
        /*------------------------------------------------------------*/
} AITS_RTC, *AITPS_RTC;

#endif

/// @}

#endif // _MMP_REG_RTC_H_
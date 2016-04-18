//==============================================================================
//
//  File        : mmph_reg_wd.h
//  Description : INCLUDE File for the WatchDog module register map.
//  Author      : Alterman
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPH_REG_WD_H_
#define _MMPH_REG_WD_H_

#include "mmp_register.h"

/** @addtogroup MMPH_reg
@{
*/

//--------------------------------
// WD structure (0xFFFE 8000)
//--------------------------------
typedef struct _AITS_WD {
    AIT_REG_D   WD_MODE_CTL0;                               // 0x00
        /*-DEFINE-----------------------------------------------------*/
        #define WD_CTL_ACCESS_KEY           0x2340
        #define WD_INT_EN                   0x0004
        #define WD_RST_EN                   0x0002
        #define WD_EN                       0x0001
        /*------------------------------------------------------------*/
    AIT_REG_D   WD_MODE_CTL1;                               // 0x04
        /*-DEFINE-----------------------------------------------------*/
        #define WD_CLK_CTL_ACCESS_KEY       0x3700
        /*------------------------------------------------------------*/
    AIT_REG_D   WD_RE_ST;                                   // 0x08
        /*-DEFINE-----------------------------------------------------*/
        #define WD_RESTART                  0xC071
        /*------------------------------------------------------------*/
    AIT_REG_D   WD_SR;                                      // 0x0C
        /*-DEFINE-----------------------------------------------------*/
        #define WD_RESET_SR                 0x0002
        #define WD_OVF_SR                   0x0001
        /*------------------------------------------------------------*/
} AITS_WD, *AITPS_WD;

/// @}

#endif // _MMPH_REG_WD_H_
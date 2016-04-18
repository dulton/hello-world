//------------------------------------------------------------------------------
//
//  File        : clk_cfg.h
//  Description : Header file of clock frequency configuration
//  Author      : Alterman
//  Revision    : 1.0
//
//------------------------------------------------------------------------------
#ifndef _CLK_CFG_H_
#define _CLK_CFG_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "includes_fw.h"
#include "mmp_clk_inc.h"
#if (CPU_ID == CPU_A)
#include "Customer_config.h"
#endif

#define CUSTOMIZED_DPLL1_CLOCK (0)

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef struct _GRP_CLK_CFG {
    GRP_CLK_SRC src;    ///< clock source
    MMP_UBYTE   div;    ///< clock divider
} GRP_CLK_CFG;

//==============================================================================
//
//                              EXTERN VARIABLES
//
//==============================================================================

extern const GRP_CLK_CFG gGrpClkCfg[CLK_GRP_NUM];

#endif // _CLK_CFG_H_


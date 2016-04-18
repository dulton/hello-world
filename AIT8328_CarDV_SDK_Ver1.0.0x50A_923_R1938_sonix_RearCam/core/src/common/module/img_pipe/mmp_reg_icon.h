//==============================================================================
//
//  File        : mmp_reg_icon.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_REG_ICON_H_
#define _MMP_REG_ICON_H_

#include "mmp_register.h"

/** @addtogroup MMPH_reg
@{
*/

#if (CHIP == MCR_V2)
typedef struct _AITS_ICOJ {
    AIT_REG_D   ICO_ADDR_ST;                        //0x00
    AIT_REG_W   ICO_X_ST;                      		//0x04
    AIT_REG_W   ICO_Y_ST;                      		//0x06
    AIT_REG_W   ICO_X_ED;                      		//0x08
    AIT_REG_W   ICO_Y_ED;                      		//0x0A
    AIT_REG_W   ICO_TP_COLR;                        //0x0C
    AIT_REG_B   ICO_SEMI_WT_ICON;                   //0x0E
    AIT_REG_B   ICO_SEMI_WT_SRC;                    //0x0F

    AIT_REG_B   ICO_CTL;                            //0x10
        /*-DEFINE-----------------------------------------------------*/
        #define ICO_TP_EN                   0x01
        #define ICO_SEMITP_EN               0x02
        #define ICO_FMT_RGB565              0x00
        #define ICO_FMT_INDEX8              0x04
        #define ICO_FMT_INDEX1              0x08
        #define ICO_FMT_INDEX2              0x0C
        #define ICO_FMT_MASK                0x04
		#define ICO_STICKER_EN				0x10
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x11[0x3];
    AIT_REG_W	ICO_IDX_COLOR[4];					//0x14
    AIT_REG_B                           _x1C[0x4];
} AITS_ICOJ, *AITPS_ICOJ;

//------------------------------
// ICON Structure (0x8000 6C00)
//------------------------------
typedef struct _AITS_ICOB {
    AIT_REG_B   ICO_DLINE_CFG[4];                    //0x00
        /*-DEFINE-----------------------------------------------------*/
        #define ICO_DLINE_SRC_SEL_MASK      0x06
        #define ICO_DLINE_SRC_SEL(_a)       ((_a << 1) & ICO_DLINE_SRC_SEL_MASK)
        #define ICO_USE_DLINE               0x00
        #define ICO_DIS_DLINE               0x08
        #define ICO_RGB565_2_888_EXT_EN     0x10
        /*------------------------------------------------------------*/										
    AIT_REG_W   ICO_DLINE_MAX_USE[4];               //0x04
    AIT_REG_B   ICO_P4_DLINE_CFG;                   //0x0C
    AIT_REG_B							_x0D;
    AIT_REG_W   ICO_P4_DLINE_MAX_USE;				//0x0E

    AIT_REG_W   ICO_DLINE_FRM_WIDTH[5];				//0x10
    AIT_REG_B							_x1C[0x4];

	AITS_ICOJ	ICOJ[4];							//0x20
	
} AITS_ICOB, *AITPS_ICOB;

typedef struct _AITS_ICON_LUT 
{
    AIT_REG_W  INDEX_TBL[128]; // For index8 color format
} AITS_ICON_LUT, *AITPS_ICON_LUT;

#endif

/// @}

#endif // _MMP_REG_ICON_H_

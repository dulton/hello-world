//------------------------------------------------------------------------------
//
//  File        : hdr_cfg.c
//  Description : Source file of HDR configuration
//  Author      : Eroy
//  Revision    : 0.1
//
//------------------------------------------------------------------------------

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "customer_config.h"
#include "AHC_Config_SDK.h"
#include "hdr_cfg.h"
#include "mmps_3gprecd.h"

//==============================================================================
//
//                              VARIABLES
//
//==============================================================================

/*
 * Configure of HDR
 */
HDR_CFG gsHdrCfg = {
    MMP_FALSE,          	// bEnable
    HDR_MODE_STAGGER,   	// ubMode
    MMP_TRUE,     			// bRawGrabEnable
    HDR_BITMODE_8BIT,   	// ubRawStoreBitMode
    HDR_VC_STORE_2PLANE,	// ubVcStoreMethod
	2						// ubBktFrameNum
};

void MMP_EnalbeHDR(MMP_BOOL bEnable)
{
#if (SNR_CLK_POWER_SAVING_SETTING || (AHC_DRAM_SIZE == COMMON_DRAM_SIZE_32MB))
    RTNA_DBG_Str(0, FG_BLUE("Force turn-off HDR\r\n"));
	gsHdrCfg.bEnable = MMP_FALSE;
	//MMPS_3GPRECD_GetConfig()->bRawPreviewEnable[0] = MMP_FALSE;
	/*raw preview enable or disable should be decided by PreviewStart*/
#else
	gsHdrCfg.bEnable = bEnable;
	//MMPS_3GPRECD_GetConfig()->bRawPreviewEnable[0] = bEnable;
	/*raw preview enable or disable should be decided by PreviewStart*/
#endif
}

MMP_BOOL MMP_IsHDREnable(void)
{
    return gsHdrCfg.bEnable;
}


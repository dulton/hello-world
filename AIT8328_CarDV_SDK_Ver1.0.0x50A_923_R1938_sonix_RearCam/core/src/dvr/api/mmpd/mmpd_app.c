/// @ait_only

//==============================================================================
//
//  File        : mmpd_app.c
//  Description : Customer Application Control Driver Function
//  Author      : Alterman
//  Revision    : 1.0
//
//==============================================================================
/**
 *  @file mmpd_app.c
 *  @brief The customer application device driver functions
 *  @author Alterman
 *  @version 1.0
 */

#include "ait_utility.h"
#include "mmph_hif.h"
#include "mmpd_app.h"

/** @addtogroup MMPD_APP
 *  @{
 */

//==============================================================================
//
//                              VARIABLES
//
//==============================================================================

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
#if (ARCSOFT_SDTC_EN)
//------------------------------------------------------------------------------
//  Function    : MMPD_APP_AllocationSmileDetectBuf
//  Description :
//------------------------------------------------------------------------------
/**
    @brief Set working buffer addreass and size for smile detection
    @param[in] ulWorkBufAddr working buffer address.
    @param[in] ulWOrkBufSize working buffer size.
    @param[in] ulInputBufAddr Input buffer address.
    @param[in] ulInputBufSize Input buffer size.
    @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_APP_AllocationSmileDetectBuf(MMP_ULONG ulWorkBufAddr, MMP_ULONG ulWorkBufSize, 
                                          MMP_ULONG ulInputBufAddr, MMP_ULONG ulInputBufSize)
{
    MMPH_HIF_CmdSetParameterL(0, ulWorkBufAddr);
    MMPH_HIF_CmdSetParameterL(4, ulWorkBufSize);
    MMPH_HIF_CmdSetParameterL(8, ulInputBufAddr);
    MMPH_HIF_CmdSetParameterL(12, ulInputBufSize);

    MMPH_HIF_CmdSend(SET_SDTC_BUF | HIF_CMD_APP);
    return MMPC_System_CheckMMPStatus(APP_CMD_IN_EXEC);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_APP_SetSmileDetectEnable
//  Description :
//------------------------------------------------------------------------------
/**
    @brief Enable or disable smile detection
    @param[in] bEnable Enable or disable smile detection.
    @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_APP_SetSmileDetectEnable(MMP_BOOL bEnable)
{
    MMPH_HIF_CmdSetParameterW(0, bEnable);

    MMPH_HIF_CmdSend(ENABLE_SDTC | HIF_CMD_APP);
    return MMPC_System_CheckMMPStatus(APP_CMD_IN_EXEC);
}

#endif

/// @}
/// @end_ait_only


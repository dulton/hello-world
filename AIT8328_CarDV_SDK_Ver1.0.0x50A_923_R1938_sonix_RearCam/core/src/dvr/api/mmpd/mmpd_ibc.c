/// @ait_only
//==============================================================================
//
//  File        : mmpd_ibc.c
//  Description : Ritina IBC Module Control driver function
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "mmpd_system.h"
#include "mmpd_ibc.h"
#include "mmpf_ibc.h"

/** @addtogroup MMPD_IBC
 *  @{
 */

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

//------------------------------------------------------------------------------
//  Function    : MMPD_IBC_SetH264RT_Enable
//  Description : This function set IBC to H264 realtime mode enable.
//------------------------------------------------------------------------------
/** 
 * @brief This function set IBC to H264 realtime mode enable.
 * 
 *  This function set IBC to H264 realtime mode enable.
 * @param[in] ubEn : stands for IBC realtime mode enable.
 * @return It return the function status.  
 */
MMP_ERR MMPD_IBC_SetH264RT_Enable(MMP_BOOL ubEn)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_TRUE);
    
    MMPF_IBC_SetH264RT_Enable(ubEn);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_FALSE);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_IBC_SetRtPingPongAddr
//  Description : This function set IBC realtime to H264 ping-pong buffer.
//------------------------------------------------------------------------------
/** 
 * @brief This function set IBC realtime to H264 ping-pong buffer.
 * 
 *  This function set IBC realtime to H264 ping-pong buffer. 
 * @param[in] pipeID   : stands for IBC pipe ID.
 * @param[in] ulYAddr  : stands for IBC ping-pong Y line buffer. 
 * @param[in] ulUAddr  : stands for IBC ping-pong Cb line buffer. 
 * @param[in] ulVAddr  : stands for IBC ping-pong Cr line buffer. 
 * @param[in] ulY1Addr : stands for IBC ping-pong Y1 line buffer. 
 * @param[in] ulU1Addr : stands for IBC ping-pong Cb1 line buffer.         
 * @param[in] ulV1Addr : stands for IBC ping-pong Cr1 line buffer.
 * @param[in] usFrmW   : stands for IBC frame width for encode.
 * @return It return the function status.  
 */
MMP_ERR MMPD_IBC_SetRtPingPongAddr(MMP_IBC_PIPEID pipeID, 
                                   MMP_ULONG ulYAddr,  MMP_ULONG ulUAddr, MMP_ULONG ulVAddr,
                                   MMP_ULONG ulY1Addr, MMP_ULONG ulU1Addr, MMP_ULONG ulV1Addr, 
                                   MMP_USHORT usFrmW)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_TRUE);

    MMPF_IBC_SetRtPingPongAddr(pipeID,
                               ulYAddr,  ulUAddr,  ulVAddr,
                               ulY1Addr, ulU1Addr, ulV1Addr,
                               usFrmW);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_IBC_SetMCI_ByteCount
//  Description : This function set MCI byte count for IBC.
//------------------------------------------------------------------------------
/** 
 * @brief This function set MCI byte count for IBC.
 * 
 *  This function set MCI byte count for IBC.
 * @param[in] pipeID       : stands for IBC pipe ID.
 * @param[in] ubByteCntSel : stands for byte count selection.
 * @return It return the function status.  
 */
MMP_ERR MMPD_IBC_SetMCI_ByteCount(MMP_IBC_PIPEID pipeID, MMP_USHORT ubByteCntSel)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_TRUE);
 
    MMPF_IBC_SetMCI_ByteCount(pipeID, ubByteCntSel);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_FALSE);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_IBC_ResetModule
//  Description : This function reset IBC module.
//------------------------------------------------------------------------------
MMP_ERR MMPD_IBC_ResetModule(MMP_IBC_PIPEID pipeID)
{
	return MMPF_IBC_ResetModule(pipeID);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_IBC_SetColorFmt
//  Description : This function set IBC input/output color format.
//------------------------------------------------------------------------------
/** 
 * @brief This function set IBC input/output color format.
 * 
 *  This function set IBC input/output color format.
 * @param[in] pipeID : stands for IBC pipe ID.
 * @param[in] fmt    : stands for color format index. 
 * @return It return the function status.  
 */
MMP_ERR MMPD_IBC_SetColorFmt(MMP_IBC_PIPEID pipeID, MMP_IBC_COLOR fmt)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_TRUE);
    
    MMPF_IBC_SetColorFmt(pipeID, fmt);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_FALSE);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_IBC_SetAttributes
//  Description : 
//------------------------------------------------------------------------------
/** @brief The function sets the attributes to the specified ibc with its ibc ID

  @param[in] usPipeID the IBC ID
  @param[in] pipeAttr the IBC attribute buffer
  @return It reports the status of the operation.
*/
MMP_ERR MMPD_IBC_SetAttributes(MMP_IBC_PIPEID pipeID, MMP_IBC_PIPE_ATTR *pipeAttr)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_TRUE);

    MMPF_IBC_SetAttributes(pipeID, pipeAttr);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_FALSE);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_IBC_GetAttributes
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_IBC_GetAttributes(MMP_IBC_PIPEID pipeID, MMP_IBC_PIPE_ATTR *pipeAttr)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_TRUE);
    
    MMPF_IBC_GetAttributes(pipeID, pipeAttr);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_FALSE);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_IBC_SetStoreEnable
//  Description : 
//------------------------------------------------------------------------------
/** @brief The function enables or disables ibc store a frame to FB.

  @param[in] usPipeID the IBC ID
  @param[in] bEnable enable or disable IBC
  @return It reports the status of the operation.
*/
MMP_ERR MMPD_IBC_SetStoreEnable(MMP_IBC_PIPEID pipeID, MMP_BOOL bEnable)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_TRUE);
    
    MMPF_IBC_SetStoreEnable(pipeID, bEnable);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_FALSE);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_IBC_SetSingleStoreEnable
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_IBC_SetSingleStoreEnable(MMP_IBC_PIPEID pipeID, MMP_BOOL bEnable)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_TRUE);
    
    MMPF_IBC_SetSingleFrmEnable(pipeID, bEnable);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_FALSE);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_IBC_ClearFrameEnd
//  Description : 
//------------------------------------------------------------------------------
/** @brief The function clear the frame end status. 

  @param[in] usPipeID the IBC ID
  @return It reports the status of the operation.
*/
MMP_ERR MMPD_IBC_ClearFrameEnd(MMP_IBC_PIPEID pipeID)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_TRUE);
    
    MMPF_IBC_ClearFrameEnd(pipeID);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_FALSE);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_IBC_ClearFrameReady
//  Description : 
//------------------------------------------------------------------------------
/** @brief The function clear the frame ready status. 

  @param[in] usPipeID the IBC ID
  @return It reports the status of the operation.
*/
MMP_ERR MMPD_IBC_ClearFrameReady(MMP_IBC_PIPEID pipeID)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_TRUE);
    
    MMPF_IBC_ClearFrameReady(pipeID);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_FALSE);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_IBC_CheckFrameEnd
//  Description : 
//------------------------------------------------------------------------------
/** @brief The function wait the frame end status. 

  @param[in] usPipeID the IBC ID
  @return It reports the status of the operation.
*/
MMP_ERR MMPD_IBC_CheckFrameEnd(MMP_IBC_PIPEID pipeID)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_TRUE);
    
    MMPF_IBC_CheckFrameEnd(pipeID);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_FALSE);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_IBC_CheckFrameReady
//  Description : 
//------------------------------------------------------------------------------
/** @brief The function wait the frame ready status. 

  @param[in] usPipeID the IBC ID
  @return It reports the status of the operation.
*/
MMP_ERR MMPD_IBC_CheckFrameReady(MMP_IBC_PIPEID pipeID)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_TRUE);
    
    MMPF_IBC_CheckFrameReady(pipeID);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_FALSE);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_IBC_SetMirrorEnable
//  Description : This function enable mirror function.
//------------------------------------------------------------------------------
/** 
 * @brief This function enable mirror function.
 * 
 *  This function enable mirror function.
 * @param[in] pipeID  : stands for IBC pipe ID.
 * @param[in] bEnable : stands for mirror enable.
 * @param[in] usWidth : stands for frame width.  
 * @return It return the function status.  
 */
MMP_ERR MMPD_IBC_SetMirrorEnable(MMP_IBC_PIPEID pipeID, MMP_BOOL bEnable, MMP_USHORT usWidth)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_TRUE);

    MMPF_IBC_SetMirrorEnable(pipeID,
                             bEnable,
                             usWidth);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_IBC_SetInterruptEnable
//  Description : This function enable interrupt event.
//------------------------------------------------------------------------------
/** 
 * @brief This function enable interrupt event.
 * 
 *  This function enable interrupt event.
 * @param[in] pipeID  : stands for IBC pipe ID.
 * @param[in] event   : stands for interrupt event.
 * @param[in] bEnable : stands for interrupt enable. 
 * @return It return the function status.  
 */
MMP_ERR MMPD_IBC_SetInterruptEnable(MMP_IBC_PIPEID pipeID, MMP_IBC_EVENT event, MMP_BOOL bEnable)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_TRUE);

    MMPF_IBC_SetInterruptEnable(pipeID,
                                event,
                                bEnable);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_IBC_RegisterIntrCallBack
//  Description : This function register interrupt callback function.
//------------------------------------------------------------------------------
/** 
 * @brief This function register interrupt callback function.
 * 
 *  This function register interrupt callback function.
 * @param[in] pipeID    : stands for IBC pipe ID.
 * @param[in] event     : stands for interrupt event.
 * @param[in] pCallBack : stands for interrupt callback function. 
 * @return It return the function status.  
 */
MMP_ERR MMPD_IBC_RegisterIntrCallBack(MMP_IBC_PIPEID pipeID, MMP_IBC_EVENT event, void* pCallBack, void* pCallBackArgu)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_TRUE);

    MMPF_IBC_RegisterIntrCallBack(pipeID,
                                  event,
                                  (IbcCallBackFunc *)pCallBack,
                                  (void *)pCallBackArgu);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_IBC, MMP_FALSE);

    return MMP_ERR_NONE;
}

/// @}
/// @end_ait_only

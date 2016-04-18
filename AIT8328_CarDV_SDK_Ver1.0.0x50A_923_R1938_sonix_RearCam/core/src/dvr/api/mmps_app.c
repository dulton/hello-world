//==============================================================================
//
//  File        : mmps_app.c
//  Description : Customer user Application Interface
//  Author      : Alterman
//  Revision    : 1.0
//
//==============================================================================
/**
 *  @file mmps_app.c
 *  @brief The customer user application functions
 *  @author Alterman
 *  @version 1.0
 */

#include "mmph_hif.h"
#include "mmps_sensor.h"
#include "mmps_app.h"
#include "mmpd_app.h"
 /** @addtogroup MMPS_APP
@{
*/

//==============================================================================
//
//                              VARIABLES
//
//==============================================================================
#if (ARCSOFT_SDTC_EN)
/**@brief Current smile detection mode.

Use @ref MMPS_APP_GetSmileDetectEnable to get it,
         and use MMPS_APP_SetSmileDetectEnable to set it.
*/
static MMP_BOOL m_CurSdtcMode = MMP_FALSE;
#endif

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
#if (ARCSOFT_SDTC_EN)
//------------------------------------------------------------------------------
//  Function    : MMPS_APP_AllocateSmileDetectBuffer
//  Description :
//------------------------------------------------------------------------------
/**
    @brief  The function will allocate buffers for face detection.
            The function should be called in MMPS_DSC_SetPreviewMode().
    @param[in] ulStartAddr The start address to allocate buffers.
    @return It reports the status of buffer allocation.
*/
MMP_ERR MMPS_APP_AllocateSmileDetectBuffer(MMP_ULONG *ulStartAddr, MMP_USHORT usInputW, MMP_USHORT usInputH)
{
    MMP_ERR   err;
    MMP_ULONG ulCurBuf = *ulStartAddr;
    MMP_ULONG ulWorkingBuf, ulInputBuf;
    MMP_ULONG ulWorkingBufSize, ulInputBufSize;

    //4K alignment for dynamic adjustment cache mechanism
    ulCurBuf = ((ulCurBuf + 0xFFF) >> 12) << 12;
    ulWorkingBuf = ulCurBuf;
    //By default, 600KB size is required in this version of the library.
    ulWorkingBufSize = 0x96000;
    ulCurBuf += ulWorkingBufSize;

    ulInputBuf = ulCurBuf;
    ulInputBufSize = (usInputW * usInputH * 3) >> 1; //need cacheable
    ulCurBuf = ((ulCurBuf + ulInputBufSize + 0xFFF) >> 12) << 12;

    err = MMPD_APP_AllocationSmileDetectBuf(ulWorkingBuf, ulWorkingBufSize, ulInputBuf, ulInputBufSize);
    *ulStartAddr = ulCurBuf;

    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_APP_SetSmileDetectEnable
//  Description :
//------------------------------------------------------------------------------
/**
    @brief  The function will enable or disable smile detection.
    @param[in] bEnable Enable or disable smile detection.
    @return It reports the status of smile detection control.
*/
MMP_ERR MMPS_APP_SetSmileDetectEnable(MMP_BOOL bEnable)
{
    m_CurSdtcMode = bEnable;
    return MMPD_APP_SetSmileDetectEnable(bEnable);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_APP_GetSmileDetectEnable
//  Description :
//------------------------------------------------------------------------------
/**
    @brief  The function gets the current status of smile detection.
    @return It reports the status of smile detection.
*/
MMP_BOOL MMPS_APP_GetSmileDetectEnable(void)
{
    return m_CurSdtcMode;
}
#endif


/// @}

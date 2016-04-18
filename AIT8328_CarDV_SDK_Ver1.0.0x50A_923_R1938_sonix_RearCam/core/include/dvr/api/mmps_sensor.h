//==============================================================================
//
//  File        : mmps_sensor.h
//  Description : INCLUDE File for the Host Application for Sensor.
//  Author      : Alan Wu
//  Revision    : 1.0
//
//==============================================================================

/**
 *  @file mmps_sensor.h
 *  @brief The header File for the Host Sensor control functions
 *  @author Alan Wu
 *  @version 1.0
 */

#ifndef _MMPS_SENSOR_H_
#define _MMPS_SENSOR_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "mmpd_sensor.h"

/** @addtogroup MMPS_SENSOR
@{
*/

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

MMP_ERR  MMPS_Sensor_Initialize(MMP_UBYTE ubSnrSel, MMP_UBYTE ubPreviewMode, MMP_SNR_MODE sSnrMode);
MMP_BOOL MMPS_Sensor_IsInitialized(MMP_UBYTE ubSnrSel);
MMP_ERR  MMPS_Sensor_ChangePreviewMode(MMP_UBYTE ubSnrSel, MMP_UBYTE ubPreviewMode, MMP_UBYTE ubWaitCount);
MMP_ERR  MMPS_Sensor_SetPreviewMode(MMP_UBYTE ubSnrSel, MMP_UBYTE ubPreviewMode);
MMP_ERR  MMPS_Sensor_SetCaptureMode(MMP_UBYTE ubSnrSel, MMP_UBYTE ubCaptureMode);
MMP_ERR  MMPS_Sensor_GetCurPrevScalInputRes(MMP_UBYTE ubSnrSel, MMP_ULONG *ulWidth, MMP_ULONG *ulHeight);
MMP_ERR  MMPS_Sensor_GetCurCaptScalInputRes(MMP_UBYTE ubSnrSel, MMP_ULONG *ulWidth, MMP_ULONG *ulHeight);
MMP_ERR  MMPS_Sensor_GetScalInputRes(MMP_UBYTE ubSnrSel, MMP_UBYTE ubMode, MMP_ULONG *ulWidth, MMP_ULONG *ulHeight);
MMP_UBYTE MMPS_Sensor_GetPreviewResNum(MMP_UBYTE ubSnrSel);
MMP_UBYTE MMPS_Sensor_GetDefPreviewMode(MMP_UBYTE ubSnrSel);
MMP_ERR  MMPS_Sensor_GetCurFpsx10(MMP_UBYTE ubSnrSel, MMP_ULONG *ulFpsx10);
MMP_ERR  MMPS_Sensor_PowerDown(MMP_UBYTE ubSnrSel);
MMP_ERR  MMPS_Sensor_SetSensorReg(MMP_UBYTE ubSnrSel, MMP_USHORT usAddr, MMP_USHORT usData);
MMP_ERR  MMPS_Sensor_GetSensorReg(MMP_UBYTE ubSnrSel, MMP_USHORT usAddr, MMP_USHORT *usData);
MMP_ERR  MMPS_Sensor_SetFlip(MMP_UBYTE ubSnrSel, MMP_UBYTE ubFlipType);
MMP_ERR  MMPS_Sensor_GetMemEnd(MMP_ULONG *ulMemEnd);
MMP_ERR  MMPS_Sensor_SetGain(MMP_UBYTE ubSnrSel, MMP_ULONG ulGain, MMP_UBYTE ubType);
MMP_ERR  MMPS_Sensor_SetShutter(MMP_UBYTE ubSnrSel, MMP_UBYTE ubIndex, MMP_UBYTE ubType);
MMP_ERR  MMPS_Sensor_SetExposureLimit(MMP_ULONG ulBufaddr, MMP_ULONG ulDatatypeByByte, MMP_ULONG ulSize, MMP_UBYTE ubType);
MMP_ERR  MMPS_Sensor_Set3AFunction(MMP_ISP_3A_FUNC sFunc, int pParam);
MMP_ERR  MMPS_Sensor_SetIQFunction(MMP_ISP_IQ_FUNC sFunc, int pParam);
MMP_ERR  MMPS_Sensor_SetPreviewDelayCount(MMP_UBYTE ubSnrSel, MMP_UBYTE ubDelayCount);

/// @}
#endif // _MMPS_SENSOR_H_

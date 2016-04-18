/// @ait_only
//==============================================================================
//
//  File        : mmpd_sensor.h
//  Description : INCLUDE File for the Host DSC Driver.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPD_SENSOR_H_
#define _MMPD_SENSOR_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "mmp_snr_inc.h"

/** @addtogroup MMPD_Sensor
 *  @{
 */

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

typedef enum _MMPD_SENSOR_EVENT {
    MMPD_SENSOR_EVENT_NONE = 0,
    MMPD_SENSOR_EVENT_TV_SRC_TYPE
} MMPD_SENSOR_EVENT;


//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

MMP_ERR	MMPD_Sensor_Initialize(MMP_UBYTE ubSnrSel, MMP_UBYTE ubPreviewMode);
MMP_ERR MMPD_Sensor_AllocateBuffer(MMP_ULONG ulStartAddr, MMP_ULONG *ulSize);
MMP_ERR MMPD_Sensor_GetHWBufferSize(MMP_ULONG *ulSize);
MMP_ERR MMPD_Sensor_SetMode(MMP_UBYTE ubSnrSel, MMP_SNR_MODE sSnrMode);
MMP_ERR MMPD_Sensor_ChangePreviewMode(MMP_UBYTE ubSnrSel, MMP_UBYTE ubPreviewMode, MMP_UBYTE ubWaitCount);
MMP_ERR MMPD_Sensor_SetPreviewMode(MMP_UBYTE ubSnrSel, MMP_UBYTE ubPreviewMode);
MMP_ERR	MMPD_Sensor_PowerDown(MMP_UBYTE ubSnrSel);
MMP_ERR	MMPD_Sensor_SetSensorReg(MMP_UBYTE ubSnrSel, MMP_USHORT usAddr, MMP_USHORT usData);
MMP_ERR	MMPD_Sensor_GetSensorReg(MMP_UBYTE ubSnrSel, MMP_USHORT usAddr, MMP_USHORT *usData);
MMP_ERR MMPD_Sensor_SetFlip(MMP_UBYTE ubSnrSel, MMP_UBYTE ubFlipType);
MMP_ERR MMPD_Sensor_SetGain(MMP_UBYTE ubSnrSel, MMP_ULONG ulGain, MMP_UBYTE ubType);
MMP_ERR MMPD_Sensor_SetShutter(MMP_UBYTE ubSnrSel, MMP_UBYTE ubIndex, MMP_UBYTE ubType);
MMP_ERR	MMPD_Sensor_SetExposureLimit(MMP_ULONG ulBufaddr, MMP_ULONG ulDataTypeByByte, MMP_ULONG ulSize, MMP_UBYTE ubType);
MMP_ERR MMPD_Sensor_SetCaptureMode(MMP_UBYTE ubSnrSel, MMP_UBYTE ubCaptureMode);
MMP_ERR MMPD_Sensor_GetDriverParam(MMP_UBYTE ubSnrSel, MMP_SNR_DRV_PARAM type,
                                   MMP_UBYTE ubInputSize, MMP_ULONG *pInput,
                                   MMP_UBYTE ubOutputSize, MMP_ULONG *pParam);
MMP_UBYTE MMPD_Sensor_GetVIFPad(MMP_UBYTE ubSnrSel);
MMP_ERR MMPD_Sensor_Set3AFunction(MMP_ISP_3A_FUNC sFunc, int pParam);
MMP_ERR MMPD_Sensor_SetIQFunction(MMP_ISP_IQ_FUNC sFunc, int pParam);
MMP_ERR MMPD_Sensor_RegisterCallback(MMPD_SENSOR_EVENT event, void *Callback);

#endif // _MMPD_SENSOR_H_


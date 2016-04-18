//==============================================================================
//
//  File        : mmps_iva.h
//  Description : INCLUDE File for the Intelligent video analysis function.
//  Author      :
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPS_IVA_H_
#define _MMPS_IVA_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "ait_config.h"
#include "mmpd_fctl.h"

/** @addtogroup MMPS_IVA
@{
*/

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

typedef enum _MMPS_IVA_FDTC_PATH {
    MMPS_IVA_FDTC_LOOPBACK = 0x00,	
    MMPS_IVA_FDTC_SUBPIPE,
    MMPS_IVA_FDTC_YUV420
} MMPS_IVA_FDTC_PATH;

typedef enum _MMPS_IVA_EVENT {
    MMPS_IVA_EVENT_MDTC = 0x0,
    MMPS_IVA_EVENT_TV_SRC_TYPE,
    MMPS_IVA_EVENT_MAX
} MMPS_IVA_EVENT;

typedef enum _MMPS_IVA_ADAS_FEATURE {
    MMPS_ADAS_LDWS = 0x0,
    MMPS_ADAS_FCWS,
    MMPS_ADAS_SAG,
    MMPS_ADAS_FEATURE_NUM
} MMPS_IVA_ADAS_FEATURE;

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef struct _MMPS_FDTC_CONFIG {
    // ++ Face Detection
    MMP_BOOL                bFaceDetectEnable;                          ///< Need to support face detection function or not.
    MMP_BOOL                bSmileDetectEnable;                         ///< Need to support smile detection function or not.
    MMP_USHORT              usFaceDetectGrabScaleM;                     ///< The grab M factor for face detection scale path.
    /**	@brief	the maximum size of feature buffer for face detection. 
    			the buffer for Y data is used to detect face feature. */
    MMP_UBYTE               ubDetectFaceNum;                            ///< Maximum number of face to detect.
    MMP_UBYTE               ubFaceDetectInputBufCnt;                    ///< The number of FD frame buffers.
    MMPS_IVA_FDTC_PATH      faceDetectInputPath;                        ///< Face detection input path selection
    // -- Face Detection
} MMPS_FDTC_CONFIG;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

#if (SUPPORT_FDTC)
MMPS_FDTC_CONFIG* MMPS_Fdtc_GetConfig(void);
MMP_ERR  MMPS_Sensor_AllocateFDBuffer(	MMP_ULONG 	*ulStartAddr, 
										MMP_USHORT 	usInputWidth, 
										MMP_USHORT 	usInputHeight,
										MMP_BOOL 	bAllocate);
MMP_ERR  MMPS_Sensor_FDPathInterface(MMPS_SENSOR_FDTC_PATH pathselect, MMPD_FCTL_LINK *fctlLinkFDPreview, MMPD_FCTL_LINK *fctlLinkFDLoopback);
#endif

MMP_ERR  MMPS_Sensor_SetFDEnable(MMP_BOOL bEnable);
MMP_BOOL MMPS_Sensor_GetFDEnable(void);

#if (SUPPORT_MDTC)
void     MMPS_Sensor_SetVMDPipe(MMP_IBC_PIPEID pipe);
MMP_ERR  MMPS_Sensor_AllocateVMDBuffer(MMP_ULONG *ulStartAddr, MMP_BOOL bAllocate);
MMP_ERR  MMPS_Sensor_GetVMDResolution(MMP_ULONG *ulWidth, MMP_ULONG *ulHeight);
MMP_ERR  MMPS_Sensor_RegisterCallback(MMPS_IVA_EVENT event, void *callback);
MMP_ERR  MMPS_Sensor_EnableVMD(MMP_BOOL bEnable);
MMP_BOOL MMPS_Sensor_IsVMDEnable(void);
#endif

#if (SUPPORT_ADAS)
MMP_ERR  MMPS_Sensor_AllocateADASBuffer(MMP_ULONG *ulStartAddr, MMP_BOOL bAllocate, MMP_ULONG ulDMABufSize);
MMP_ERR  MMPS_Sensor_GetADASResolution(MMP_ULONG *ulWidth, MMP_ULONG *ulHeight);
MMP_ERR  MMPS_Sensor_SetADASFeatureEn(MMPS_IVA_ADAS_FEATURE feature, MMP_BOOL bEnable);
MMP_BOOL MMPS_Sensor_GetADASFeatureEn(MMPS_IVA_ADAS_FEATURE feature);
MMP_ERR  MMPS_Sensor_EnableADAS(MMP_BOOL bEnable);
MMP_BOOL MMPS_Sensor_IsADASEnable(void);
#endif

/// @}
#endif // _MMPS_IVA_H_

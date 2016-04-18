//==============================================================================
//
//  File        : StateVideoFunc.h
//  Description : INCLUDE File for the StateCameraFunc function porting.
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================

#ifndef _STATEVIDEOFUNC_H_
#define _STATEVIDEOFUNC_H_

/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "AHC_General.h"
#include "MenuCommon.h"

//typedef enum {
//    ALERT_NON,
//    ALERT_DRIVER_FATIGUE,
//    ALERT_REMIND_HEADLIGHT,
//    ALERT_LDWS,
//    ALERT_FCWS,
//    ALERT_SPEED_CAM,
//    ALERT_SPEED_LIMIT_CAM,
//    ALERT_NUM
//} ALERT_TYPE;

typedef enum {
    F_LARGE_R_SMALL = 0,
    F_SMALL_R_LARGE,
    ONLY_FRONT,
    ONLY_REAR,
    F_TOP_R_BOTTOM,
    F_LEFT_R_RIGHT,
    PIP_SWAP_TYPE_NUM
} PIP_SWAP_TYPE;

/*===========================================================================
 * Function prototype
 *===========================================================================*/ 

//ALERT_TYPE 	CheckAlertState(void);
//AHC_BOOL 	CheckAlertNonState(void);
void 		StateVideoRecMode(UINT32 ulEvent);
UINT32 		VideoFunc_GetRecordTimeOffset(void);
void 		VideoRecMode_PreviewUpdate(void);
AHC_BOOL 	VideoRecMode_Start(void);
AHC_BOOL 	VideoFunc_RecordStatus(void);
void 		VideoFunc_GetFreeSpace(UINT64 *pFreeBytes);
AHC_BOOL    VideoFunc_ExitVMDMode(void);
AHC_BOOL 	VideoFunc_ZoomOperation(AHC_ZOOM_DIRECTION bZoomDir);
void 		VideoFunc_PresetSensorMode(MOVIESIZE_SETTING ubResolution);
void 		VideoFunc_PresetFrameRate(MOVIESIZE_SETTING ubResolution);
AHC_BOOL 	VideoFunc_RecordStatus(void);
AHC_BOOL 	VideoFunc_RecordRestart(void);
AHC_BOOL 	VideoFunc_RecordStop(void);
AHC_BOOL 	VideoTimer_Start(UINT32 ulTime);
AHC_BOOL 	VideoTimer_Stop(void);
AHC_BOOL 	VideoFunc_Preview(void);
AHC_BOOL 	VideoFunc_PreRecordStatus(void); 
AHC_BOOL 	VideoFunc_SetFileLock(void);
AHC_BOOL 	VideoFunc_Shutter(void);
AHC_BOOL 	VideoFunc_ShutterFail(void);
AHC_BOOL    VideoFunc_LockFileEnabled(void);
void        StateVideoRecMode_StartRecordingProc(UINT32 ulJobEvent);
void        StateVideoRecMode_StopRecordingProc(UINT32 ulJobEvent);
AHC_BOOL	VideoRec_TriggeredByVMD(void);

// for ADAS_LDWS, TBD
AHC_BOOL VideoFunc_Init(void* pData);
AHC_BOOL VideoFunc_ShutDown(void* pData);
#endif //_STATEVIDEOFUNC_H_


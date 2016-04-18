/// @ait_only
//==============================================================================
//
//  File        : mmpd_iva.h
//  Description : Intelligent video analysis function
//  Author      :
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPD_IVA_H_
#define _MMPD_IVA_H_

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

#if (SUPPORT_FDTC)
typedef enum _MMPD_SENSOR_FD_BUF_TYPE {
	MMPD_SENSOR_FD_WORK_BUF = 0,
	MMPD_SENSOR_FD_INFO_BUF,
	MMPD_SENSOR_SD_WORK_BUF
} MMPD_SENSOR_FD_BUF_TYPE;

typedef enum _MMPD_SENSOR_FDTC_PATH {
    MMPD_SENSOR_FDTC_LOOPBACK = 0x00,
    MMPD_SENSOR_FDTC_SUBPIPE,
    MMPD_SENSOR_FDTC_YUV420
} MMPD_SENSOR_FDTC_PATH;
#endif

//==============================================================================
//
//                              STRUCTURE
//
//==============================================================================

#if (SUPPORT_FDTC)
typedef struct _MMPD_FDTC_ATTR {

	MMP_USHORT  usInputW;                   ///< The width of input frame to do face detection
	MMP_USHORT  usInputH;                   ///< The height of input frame to do face detection

    MMP_ULONG   ulInputBufAddr[2];          ///< The address of slots to keep whole frames
    MMP_ULONG   ulFDWorkBufAddr;            ///< The address of face detection working buffer
    MMP_ULONG   ulSDWorkBufAddr;            ///< The address of smile detection working buffer
    MMP_ULONG   ulFeatureBufAddr;           ///< The address of face detection feature buffer
    MMP_ULONG   ulInfoBufAddr;              ///< The address of buffer for keeping face detection results
    
    MMP_USHORT  usGrabScaleM;               ///< The grab M factor of face detection scale path
    MMP_UBYTE   ubInputBufCnt;              ///< The total count of slots to keep whole frames
    MMP_UBYTE   ubDetectFaceNum;            ///< The maximum number of face for detection
    MMP_ULONG   winID;                      ///< The window to display face rectangles
    MMP_UBYTE   ubSmileDetectEn;            ///< Enable smile detection
} MMPD_FDTC_ATTR;
#endif


//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

#if (SUPPORT_FDTC)
MMP_ERR MMPD_Sensor_GetFaceDetectSpec(MMP_USHORT *usMaxH, MMP_USHORT *usMaxV, MMP_UBYTE *ubMaxFaceNum);
MMP_ERR MMPD_Sensor_InitFaceDetectConfig(MMPD_FDTC_ATTR *fdtcattribute);
MMP_ERR MMPD_Sensor_SetFaceDetectBuf(MMPD_FDTC_ATTR *fdtcattribute);
MMP_ERR MMPD_Sensor_SetFaceDetectDisplayInfo(MMP_IBC_PIPEID pipeID, 
                                             MMP_USHORT usPreviewW, MMP_USHORT usPreviewH);
MMP_ERR MMPD_Sensor_ActivateFDBuffer(MMP_BOOL bEnable, MMPD_SENSOR_FDTC_PATH fdtcPath);
MMP_ERR MMPD_Sensor_GetFDBufSize(	MMPD_SENSOR_FD_BUF_TYPE fdBufType,
									MMP_ULONG ulWidth, 
									MMP_ULONG ulHeight, 
									MMP_ULONG ulFaceNum,
									MMP_ULONG *ulSize);
#endif

#if (SUPPORT_MDTC)
MMP_ERR MMPD_Sensor_InitializeVMD(void);
MMP_ERR MMPD_Sensor_SetVMDBuf(MMP_ULONG buf_addr, MMP_ULONG *buf_size);
MMP_ERR MMPD_Sensor_GetVMDResolution(MMP_ULONG *width, MMP_ULONG *height);
MMP_ERR MMPD_Sensor_RegisterVMDCallback(void *Callback);
MMP_ERR MMPD_Sensor_EnableVMD(MMP_BOOL bEnable);
#endif

#if (SUPPORT_ADAS)
MMP_ERR MMPD_Sensor_InitializeADAS(void);
MMP_ERR MMPD_Sensor_SetADASBuf(MMP_ULONG buf_addr, MMP_ULONG *buf_size, MMP_ULONG dma_buf_addr);
MMP_ERR MMPD_Sensor_GetADASResolution(MMP_ULONG *width, MMP_ULONG *height);
MMP_ERR MMPD_Sensor_SetADASFeature(MMP_BOOL ldws_on, MMP_BOOL fcws_on, MMP_BOOL sag_on);
MMP_ERR MMPD_Sensor_EnableADAS(MMP_BOOL bEnable);
#endif

#endif //_MMPD_IVA_H_
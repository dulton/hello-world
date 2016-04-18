//==============================================================================
//
//  File        : mmpf_fdtc.h
//  Description : INCLUDE File for the face detection Driver Function
//  Author      : Rogers Chen
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPF_FDTC_H_
#define _MMPF_FDTC_H_

#include "includes_fw.h"
#if ((DSC_R_EN)||(VIDEO_R_EN))&&(SUPPORT_MDTC)
#include "mmpf_vmd.h"
#endif
#if (VIDEO_R_EN)&&(SUPPORT_ADAS)
#include "mmpf_adas.h"
#endif

/** @addtogroup MMPF_FDTC
@{
*/
#if (DSC_R_EN)||(VIDEO_R_EN)

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================
/* flags for FDTC */
#define FLAG_FDTC_DMACOPY       (0x00000001)
#define FLAG_FDTC_OPER          (0x00000002)
#define SYS_FLAG_FDTC           (0x00000004)
/* flags for VMD */
#define SYS_FLAG_VMD            (0x00000008)
#define FLAG_VMD_OPER           (0x00000010)
/* flags for ADAS */
#define SYS_FLAG_ADAS           (0x00000100)
#define FLAG_ADAS_DMACOPY       (0x00000200)
#define FLAG_ADAS_OPER          (0x00000400)

// the color for the rectangle of FDCT
#define FACE_RECT_COLOR_RGB888  (0x00FF00)
#define FACE_RECT_COLOR_RGB565  (0x07E0)
#define FACE_RECT_COLOR_YUV422  (0x9620)
#define FACE_RECT_COLOR_YUV420  (0x962115)

#define FACE_RECT_THICKNESS     (1)

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

#if (SUPPORT_FDTC)
typedef enum _MMPF_FDTC_PATH {
	/**	@brief	draw rectangle and framedonetrigger @ the interrupt of graphic done */
    MMPF_FDTC_PATH_LOOPBACK = 0,
	/**	@brief	draw rectangle and framedonetrigger @ the interrupt of ibc frame ready */
    MMPF_FDTC_PATH_SUBPIPE,
	/**	@brief	draw rectangle and framedonetrigger @ the interrupt of ibc frame ready or DMA done*/
    MMPF_FDTC_PATH_YUV420
} MMPF_FDTC_PATH;

typedef enum _MMPF_FDTC_STATE {
    MMPF_FDTC_IDLE_STATE = 0,
    MMPF_FDTC_ACTIVE_STATE,
    MMPF_FDTC_DMA_TRIGGER_STATE,
    MMPF_FDTC_DMA_MOVING_STATE,
    MMPF_FDTC_OPERATING_STATE
} MMPF_FDTC_STATE;

typedef enum _MMPF_FDTC_DRAW_STATE {
    MMPF_FDTC_DRAW_IDLE_STATE = 0,
    MMPF_FDTC_DRAW_TRIGGER_STATE,
    MMPF_FDTC_DRAWING_STATE,
    MMPF_FDTC_DRAWING_DONE_STATE,
    MMPF_FDTC_FRAME_TRIGGER_STATE
} MMPF_FDTC_DRAW_STATE;
#endif

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

#if (SUPPORT_FDTC)
typedef struct _MMPF_FDTC_ATTR {
	MMP_USHORT  usInputW;			///< the width of input frame to do face detection
	MMP_USHORT  usInputH;			///< the height of input frame to do face detection
    MMP_USHORT  usPreviewBufW;
    MMP_USHORT  usPreviewBufH;
    MMP_USHORT  usFdtcIBCPipe;
    MMP_USHORT  usPreviewIBCPipe;
    MMP_ULONG   ulFDWorkBufAddr;	///< the address of smile detection working buffer
    MMP_ULONG   ulSDWorkBufAddr;	///< the address of face detection working buffer
    MMP_ULONG   ulFeatureBufAddr;	///< the address of face detection feature buffer 
    MMP_ULONG   ulInfoBufAddr;		///< the address of buffer for keeping face detection results
    
    MMP_UBYTE   ubFrameSlotCnt;		///< the total count of slots to keep whole frames
    MMP_UBYTE   ubDetectFaceNum;	///< the maximum number of face for detection
	MMP_UBYTE   ubSmileDetectEn;
} MMPF_FDTC_ATTR;

typedef struct _MMPF_FDTC_RESULT {
    MMP_USHORT   usFacePosX;
    MMP_USHORT   usFacePosY;
    MMP_USHORT   usFaceWidthHeight;
    MMP_UBYTE    ubFaceYmean;
	MMP_UBYTE    ubIsSmile;
} MMPF_FDTC_RESULT;
#endif

//==============================================================================
//
//                              VARIABLES
//
//==============================================================================

extern MMPF_OS_FLAGID   FDTC_Flag;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

#if (SUPPORT_FDTC)
MMPF_FDTC_STATE MMPF_FDTC_GetFaceDeteState(void);
void MMPF_FDTC_SetFaceDetectInputSrc(MMP_ULONG ulMovDmaSrcAddr);
void MMPF_FDTC_SetCurPreviewBufAddr(MMP_ULONG ulPreviewBufAddr, 
                                    MMP_ULONG ulPreviewUBufAddr, MMP_ULONG ulPreviewVBufAddr);
void MMPF_FDTC_DrawRectangleTrigger(void);
void MMPF_FDTC_DrawFaceRectangles(void);
void MMPF_FDTC_DrawFrameDoneTrigger(void);
MMP_ERR MMPF_FDTC_GetFaceDetectResult(MMP_UBYTE *ubFaceNum, MMPF_FDTC_RESULT **FdtcResult, MMP_USHORT* usFdInW, MMP_USHORT* usFdInH);
MMPF_FDTC_PATH MMPF_FDTC_GetFDPath(void);
MMP_BOOL MMPF_Sensor_GetFDEnable(void);
MMP_ERR MMPF_FDTC_GetPreviewScalerPath(MMP_USHORT *path);
#endif

#endif
#endif // _MMPF_FDTC_H_
/// @}

/**
 @file mmpf_vpp.h
 @brief Header function of Video Post-Processor
 @author Truman Yang
 @version 1.0 Original Version
*/

#include "includes_fw.h"

#ifndef _MMPF_M_VPP_H_
#define _MMPF_M_VPP_H_

#include "mmpf_media_core.h"
#include "mmpf_graphics.h"
#include "mmpf_display.h"

//==============================================================================
//                                IPC SECTION
//==============================================================================
/** @addtogroup MMPF_VPP
@{
*/
//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================
/** The deblock type
*/
typedef enum MMPF_3GPPLAY_DEBLOCK_TYPE {
	MMPF_3GPPLAY_DEBLOCK_NONE,   ///< No deblocking
	MMPF_3GPPLAY_DEBLOCK_EN,     ///< Enable the deblock, with Y and Cb,Cr
	MMPF_3GPPLAY_DEBLOCK_Y_ONLY, ///< Enable the Y-only deblocking
	MMPF_3GPPLAY_DEBLOCK_MAX
} MMPF_3GPPLAY_DEBLOCK_TYPE;

typedef enum MMPF_3GPPLAY_OUTPUTPANEL {
    MMPF_3GPPLAY_NONE = 0,
    MMPF_3GPPLAY_P_LCD,        ///< Output device is P-LCD
    MMPF_3GPPLAY_S_LCD,        ///< Output device is S-LCD
    MMPF_3GPPLAY_P_LCD_FLM,    ///< Output device is P-LCD and FLM enable
    MMPF_3GPPLAY_RGB_LCD,      ///< Output device is RGBLCD
    MMPF_3GPPLAY_TV,           ///< Output device is TV
    MMPF_3GPPLAY_HDMI,         ///< Output device is HDMI
    MMPF_3GPPLAY_CCIR
} MMPF_3GPPLAY_OUTPUTPANEL;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
// ===== video post processor =====

void        MMPF_VPP_SetVideoRendererOp(MMPF_3GPPLAY_OUTPUTPANEL PanelType,MMP_BOOL bPause);
MMP_ERR     MMPF_VPP_ConfigDMA( const MMP_UBYTE                 DecodeBuf,
                                const MMP_UBYTE                 nBufferUsed,
                                const MMP_GRAPHICS_ROTATE_TYPE 	rotationtype, 
                                const MMP_ULONG                 ulStartAddr, 
                                const MMP_USHORT                sWidth, 
                                const MMP_USHORT                sHeight,
                                const MMP_USHORT                usBufType);
                            
MMP_ERR     MMPF_VPP_GetFrame(  MMP_HANDLE                      *h, 
                                MMPF_M_VIDEO_RAW_BUFFER_INFO    **output_frame);
                            
MMP_ERR     MMPF_VPP_SetDeblockType(const MMPF_3GPPLAY_DEBLOCK_TYPE deblock_type);

MMP_ERR     MMPF_VPP_Process(MMP_ULONG CodecType);
MMP_ERR     MMPF_VPP_Reset(void);
MMP_ERR     MMPF_VPP_GetDeblockType(MMPF_3GPPLAY_DEBLOCK_TYPE *deblock_type);
MMP_ERR     MMPF_VPP_Load(void);

MMP_ERR     MMPF_VPP_RefreshDone(MMPF_M_VIDEO_RAW_BUFFER_INFO *pBuf);
MMP_ERR     MMPF_VPP_SetRefresh(MMP_DISPLAY_WIN_ID WinID, MMPF_M_VIDEO_RAW_BUFFER_INFO *pBuf);
MMP_ERR     MMPF_VPP_RefreshFrame(void);
MMP_ERR     MMPF_VPP_RedrawFrame2(MMP_DISPLAY_WIN_ID WinID);
MMP_ERR     MMPF_VPP_GetLastRefreshFrame(MMP_HANDLE *h, MMPF_M_VIDEO_RAW_BUFFER_INFO **output_frame);
MMP_ERR     MMPF_VPP_GetLastRefreshFrame2(MMP_HANDLE *h, MMPF_M_VIDEO_RAW_BUFFER_INFO **output_frame);
MMP_BOOL    MMPF_VPP_IsRefreshingBuffer(MMP_ULONG addr);
MMP_ERR     MMPF_VPP_RedrawFrame(MMP_DISPLAY_WIN_ID WinID);
MMP_ERR     MMPF_VPP_RedrawLastRefreshFrame(MMP_DISPLAY_WIN_ID WinID);

MMP_ERR     MMPF_VPP_SetDecodeBufStatus(MMPF_3GP_BUFFERSTATUS status);
MMP_ERR     MMPF_VPP_UpdateDecodeBuf(MMP_ULONG          time,
                                     MMP_ULONG          flag,
                                     MMP_ULONG          frame_num,
                                     MMP_MEDIA_FRAME    frame);

MMP_ERR     MMPF_VPP_IsFreeFrameAvail(MMP_BOOL *avail);
MMP_ERR     MMPF_VPP_GetFreeFrame(MMP_MEDIA_FRAME *frame);

MMP_ERR     MMPF_VPP_GetRefreshAddr(MMP_ULONG *ulAddr);

MMP_MEDIA_FRAME     MMPF_VPP_GetDecFrame(void);
MMP_ERR             MMPF_VPP_CheckProcessFinish(MMP_BOOL *bFinish);
MMP_ERR             MMPF_VPP_DropFrame(MMPF_M_VIDEO_RAW_BUFFER_INFO *frame);
MMP_ERR             MMPF_VPP_WaitRefreshDone(void);
MMP_ERR             MMPF_VPP_WaitProcessDone(void);
MMP_ERR             MMPF_VPP_ResetDMAFlag(void);
MMP_ERR             MMPF_VPP_SetHDMIInterface(MMP_BOOL bInterlace, MMP_ULONG ulFreq);
MMP_ERR             MMPF_VPP_GetHDMIClock(MMP_ULONG *ulClock);

MMP_ERR MMPF_VPP_SetCropping(MMP_ULONG ulTop, MMP_ULONG ulBottom, MMP_ULONG ulLeft, MMP_ULONG ulRight);
MMP_ERR MMPF_VPP_ConfigScaler(const MMP_UBYTE DecodeBuf,
                           const MMP_UBYTE nBufferUsed,
                           const MMP_BOOL bUseScaleEXBuf, 
                           const MMP_ULONG ulStartAddr, const MMP_USHORT sWidth, const MMP_USHORT sHeight, const MMP_USHORT usBufType);
MMP_ERR MMPF_VPP_WaitScalerDone(void);
MMP_ERR MMPF_VPP_Scaler(MMP_ULONG CodecType);
MMP_ERR MMPF_VPP_SetResolution(MMP_USHORT usWidth, MMP_USHORT usHeight, MMP_ULONG ulVideoWidth, MMP_ULONG ulVideoHeight);

/** @} */ // end of group MMPF_M
#endif
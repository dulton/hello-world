#ifndef _MMPF_MFD_DEC_H_
#define  _MMPF_MFD_DEC_H_

#include "mmp_err.h"
#include "mmpf_typedef.h"
#include "h264decapi.h"
#include "mp4decapi.h"
#include "On2Rvdecapi.h"
#include "VC1decapi.h"
#include "ppapi.h"
#include "mmpf_scaler.h"
#include "mmpf_graphics.h"

MMP_ERR     MMPF_MFD_InitMemAddr    (MMP_ULONG ulCurrAddr,MMP_ULONG ulMemEnd);
MMP_ERR     MMPF_MFD_ClearBuffer    (MMP_ULONG ulAddr);
MMP_ERR     MMPF_MFD_UnlockBuffer(void);
MMP_ERR     MMPF_MFD_GetCurrMemAddr (MMP_ULONG *ulCurrAddr,MMP_ULONG *ulMemEnd);
MMP_ERR     MMPF_MFD_SetCurrMemAddr (MMP_ULONG ulCurrAddr);
MMP_ERR     MMPF_MFD_InitCodec( MMP_VID_VIDEOTYPE   VideoFormat, 
                                MMP_ULONG           ulBSAddr, 
                                MMP_ULONG           ulBSLength, 
                                MMP_BOOL            *bNeedFirstFrame,
                                MMP_BOOL            bDisableTimeReorder,
                                MMP_ULONG			ulMaxFrameBufSize,
                                MMP_USHORT          usWidth,
                                MMP_USHORT          usHeight);
MMP_ERR     MMPF_MFD_GetInfo        (MMP_VIDEO_INFO *info);
MMP_ERR     MMPF_MFD_GetState       (MMP_M_STATE *state);
MMP_ERR     MMPF_MFD_SetState       (MMP_M_STATE state);
MMP_ERR     MMPF_MFD_GetFreeFrame   (MMP_MEDIA_FRAME *frame,MMP_BOOL bEOSFrame);
MMP_ULONG   MMPF_MFD_DecodeData     (MMP_ULONG          ulFrameAddr,
                                     MMP_ULONG          ulFrameSize,
                                     MMP_ULONG          ulFrameID,
                                     MMP_ULONG          ulTime,
                                     MMP_ULONG          ulFlag,
                                     MMP_ULONG          *ulDataLeft,
                                     MMP_BOOL           bLastFrame,
                                     MMP_ULONG          ulNumSegments,
                                     MMP_ULONG          ulRVSegAddr,
                                     MMP_ULONG          *ulRetVal);
MMP_ULONG   MMPF_MFD_GetFrame       (MMP_MEDIA_FRAME *frame,MMP_ULONG *flag,MMP_BOOL bEOS);
MMP_ERR     MMPF_MFD_ReleaseCodec   (void);
MMP_ERR     MMPF_MFD_InitPP         (void);
                                     
MMP_ERR     MMPF_MFD_ConfigDeinterlaceMemory(MMP_ULONG ulInputWidth, MMP_ULONG ulInputHeight);
MMP_ERR     MMPF_MFD_ConfigScaleMemory(MMP_ULONG ulOutputWidth, MMP_ULONG ulOutputHeight,MMPF_GRAPHICS_COLORDEPTH    OutputColorFormat);

MMP_ERR     MMPF_MFD_DeinterlaceFrame(MMP_ULONG ulInputFrameAddr,
                                      MMP_ULONG *ulOutputFrameAddr,
                                      MMP_ULONG ulFrameWidth,
                                      MMP_ULONG ulFrameHeight);
                                      
MMP_ERR     MMPF_MFD_ScaleFrame      (MMP_ULONG                 ulInputFrameAddr,
                                      MMP_ULONG                 *ulOutputFrameAddr,
                                      MMPF_SCALER_FIT_RANGE     *fitrange, 
                                      MMPF_SCALER_GRABCONTROL   *grabctl,
                                      MMPF_GRAPHICS_COLORDEPTH  ColorFormat);
                                      
MMP_ERR     MMPF_MFD_ReleasePP      (void);

MMP_ERR     MMPF_MFD_SetBackward(MMP_BOOL bBackward);

MMP_ERR     MMPF_MFD_LockBuffer(MMP_ULONG ulAddr);
#endif
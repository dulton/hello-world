//==============================================================================
//
//  File        : mmpf_fpsctl.h
//  Description : Header function of record frame rate control
//  Author      : Alterman
//  Revision    : 1.0
//
//==============================================================================
#ifndef _MMPF_FPSCTL_H_
#define _MMPF_FPSCTL_H_

#include "includes_fw.h"

#if (VIDEO_R_EN)&&(FPS_CTL_EN)

#include "mmp_mux_inc.h"
#include "mmpf_h264enc.h"

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

#define FPS_CTL_MAX_STREAM      (4)

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef struct _MMPF_FPS_CTL {
    MMP_VID_FPS SnrFps;         /* sensor input frame rate                    */
    MMP_VID_FPS EncFps;         /* encode output frame rate                   */
    MMP_VID_FPS DecFps;         /* (container specified) decode frame rate    */
    MMP_VID_FPS UpdEncFps;      /* (to be updated) encode output frame rate   */

    MMP_ULONG   ulSnrInAcc;     /* for frame rate control calculation         */
    MMP_ULONG   ulEncOutAcc;    /* for frame rate control calculation         */

    MMP_BOOL    bFpsCtlFlag;    /* flag to indicate fps control is necessary  *
                                 * because sensor/encode fps is different     */
    MMP_BOOL    bFpsUpdFlag;    /* flag to indicate encode output fps are     *
                                 * requested to update by application         */
    MMP_BOOL    bFpsMixedFlag;  /* flag to indicate CURRENTLY encode/decode   *
                                 * fps are different.
                                 * this flag is used for merger to confirm
                                 * whether current recording is in
                                 * normal speed mode or not. In normal speed
                                 * mode, merger mux audio frames normally; but
                                 * in fast/slow speed mode, merger will
                                 * replace audio frames by silence.           */
    MMP_BOOL    bFpsFixedFlag;  /* flag to indicate sensor/encode/decode fps  *
                                 * are always indentical during recording     */
} MMPF_FPS_CTL;

//==============================================================================
//
//                              VARIABLES
//
//==============================================================================

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

void MMPF_FpsCtl_SetSnrFrameRate(MMP_UBYTE id, MMP_ULONG inc, MMP_ULONG resol);
void MMPF_FpsCtl_SetEncFrameRate(MMP_UBYTE id, MMP_ULONG inc, MMP_ULONG resol);
void MMPF_FpsCtl_SetDecFrameRate(MMP_UBYTE id, MMP_ULONG inc, MMP_ULONG resol);
void MMPF_FpsCtl_UpdateEncFrameRate(MMP_UBYTE id, MMP_ULONG inc, MMP_ULONG resol);

void MMPF_FpsCtl_Init(MMP_UBYTE id);
void MMPF_FpsCtl_Operation(MMP_UBYTE id, MMP_BOOL *bDropFrm, MMP_ULONG *ulDupCnt);

MMP_BOOL MMPF_FpsCtl_IsEnabled(MMP_UBYTE id);
MMP_BOOL MMPF_FpsCtl_IsMixedFps(MMP_UBYTE id);
MMP_BOOL MMPF_FpsCtl_IsFixedFps(MMP_UBYTE id);

#endif //(VIDEO_R_EN)&&(FPS_CTL_EN)
#endif //_MMPF_FPSCTL_H_

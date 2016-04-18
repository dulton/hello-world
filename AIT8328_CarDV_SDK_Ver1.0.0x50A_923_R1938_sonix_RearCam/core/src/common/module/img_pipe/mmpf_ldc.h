#ifndef _MMPF_LDC_H_
#define _MMPF_LDC_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_ldc_inc.h"

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#define LDC_X_POS_ARRAY_SIZE    (42)
#define LDC_Y_POS_ARRAY_SIZE    (32)
#define LDC_DELTA_ARRAY_SIZE    (128)

#define LDC_MAX_LB_CNT_FOR_FHD	(2)
#define LDC_MAX_LB_CNT_FOR_HD	(4)
#define LDC_MAX_LB_CNT_FOR_WVGA	(4)

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

typedef enum _MMPF_LDC_INPUT_PATH {
  	MMPF_LDC_INPUT_FROM_ISP = 0,
  	MMPF_LDC_INPUT_FROM_GRA,
 	MMPF_LDC_INPUT_PATH_NUM
} MMPF_LDC_INPUT_PATH;

typedef enum _MMPF_LDC_PIPE_MODE {
  	MMPF_LDC_PIPE_MODE_LOOPBACK = 0,
  	MMPF_LDC_PIPE_MODE_ORIGINAL,
 	MMPF_LDC_PIPE_MODE_NUM
} MMPF_LDC_PIPE_MODE;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

void MMPF_LDC_ISR(void);
MMP_ERR MMPF_LDC_Initialize(void);
MMP_ERR MMPF_LDC_UnInitialize(void);
MMP_ERR MMPF_LDC_SetResMode(MMP_UBYTE ubResMode);
MMP_UBYTE MMPF_LDC_GetResMode(void);
MMP_ERR MMPF_LDC_SetFpsMode(MMP_UBYTE ubFpsMode);
MMP_UBYTE MMPF_LDC_GetFpsMode(void);
MMP_ERR MMPF_LDC_SetRunMode(MMP_UBYTE ubRunMode);
MMP_UBYTE MMPF_LDC_GetRunMode(void);
MMP_ERR MMPF_LDC_SetFrameRes(MMP_ULONG ulSrcW, MMP_ULONG ulSrcH,
							 MMP_ULONG ulOutW, MMP_ULONG ulOutH);
MMP_ERR MMPF_LDC_SetLinkPipe(MMP_UBYTE ubSrcPipe, MMP_UBYTE ubPrvPipe, MMP_UBYTE ubEncPipe, MMP_UBYTE ubSwiPipe);
MMP_ERR MMPF_LDC_GetLinkPipe(MMP_LDC_LINK *pLink);
MMP_ERR MMPF_LDC_SetAttribute(MMP_LDC_ATTR* pAttr);
MMP_ERR MMPF_LDC_UpdateLUT(MMP_USHORT* pPosTbl[], MMP_ULONG* pDeltaTbl[]);

/* Multi-Run */
void MMPF_LDC_SwitchPipeMode(MMP_UBYTE ubMode);
void MMPF_LDC_MultiRunTriggerLoopBack(MMP_UBYTE ubIBCPipe);
MMP_ULONG MMPF_LDC_MultiRunGetMaxLoopBackCount(void);
void MMPF_LDC_MultiRunSetLoopBackCount(MMP_UBYTE ubCnt);
void MMPF_LDC_MultiRunNormalGraCallback(void);
void MMPF_LDC_MultiRunRetriggerGraCallback(void);

/* Multi-Slice */
void MMPF_LDC_MultiSliceNormalGraCallback(void);
void MMPF_LDC_MultiSliceRetriggerGraCallback(void);
MMP_ERR MMPF_LDC_MultiSliceInitAttr(void);
MMP_ERR MMPF_LDC_MultiSliceInitOutStoreBuf(MMP_ULONG ulYAddr, MMP_ULONG ulUAddr, MMP_ULONG ulVAddr);
MMP_ERR MMPF_LDC_MultiSliceUpdateSrcBufAddr(MMP_ULONG ulYAddr, MMP_ULONG ulUAddr, MMP_ULONG ulVAddr);
MMP_ERR MMPF_LDC_MultiSliceUpdatePipeAttr(MMP_UBYTE ubSlice);
MMP_ERR MMPF_LDC_MultiSliceRestoreDispPipe(void);
MMP_USHORT MMPF_LDC_GetMultiSliceNum(void);

#endif //_MMPF_LDC_H_
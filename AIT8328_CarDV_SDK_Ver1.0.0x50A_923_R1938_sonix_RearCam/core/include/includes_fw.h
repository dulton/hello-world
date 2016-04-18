//==============================================================================
//
//  File        : includes_fw.h
//  Description : Top level global definition and configuration.
//  Author      : Jerry Tsao
//  Revision    : 1.0
//
//==============================================================================

#ifndef _INCLUDES_FW_H_
#define _INCLUDES_FW_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "ait_init.h"
#include "ucos_ii.h"
#include "mmp_err.h"
#include "config_fw.h"
#include "mmpf_task_cfg.h"
#include "mmpf_typedef.h"
#include "os_wrap.h"
#include "bsp.h"
#include "mmpf_hif.h"

//==============================================================================
//
//                              VARIABLES
//
//==============================================================================

extern MMP_UBYTE    gbSystemCoreID;
extern MMP_UBYTE    gbEcoVer;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

/** @brief Memory set functions exactly like memset in standard c library.

The uFS uses FS_MEMSET marco to memset too. So we just use it here.
And this file has included string.h already.
There is a deprecated version function in sys.c.
@note Please try to use this function for faster memset and reduce the code size.
*/
#include <stdlib.h>
#ifndef	MEMSET
#define MEMSET(s,c,n)       memset(s,c,n)
#endif

/// Set this variable to 0
#ifndef	MEMSET0
#define MEMSET0(s)      memset(s,0,sizeof(*s))
#endif
#ifndef	MEMCPY
#define MEMCPY(d, s, c) memcpy (d, s, c)
#endif

void 				MMPF_HIGH_TaskHandler(void *p_arg);
void 				MMPF_HIGH_WorkTaskHandler(void *p_arg);
void                MMPF_DSC_TaskHandler(void *p_arg);
void                MMPF_DSC_Stream_TaskHandler(void *p_arg);
void                MMPF_JPEG_Ctl_TaskHandler(void *p_arg);

void                MMPF_MP4VENC_TaskHandler(void *p_arg);
void                MMPF_3GPMERGR_TaskHandler(void *p_arg);
void                MMPF_VIDPLAY_TaskHandler(void *p_arg);
void                MMPF_3GPPARSR_TaskHandler(void *p_arg);
void                MMPF_VIDDEC_TaskHandler(void *p_arg);

#if (VIDEO_BGPARSER_EN == 1)
void                MMPF_VIDPSR_BGTaskHandler(void *p_arg);
#if (VIDEO_HIGHPRIORITY_FS == 1)
void                MMPF_VIDPSR_BGFSTaskHandler(void *p_arg);
#endif
#endif

void                MMPF_SENSOR_TaskHandler(void *p_arg);
void                MMPF_FS_TaskHandler(void *p_arg);
void                MMPF_AUDIOPLAY_TaskHandler(void *p_arg);
void                MMPF_AUDIOSTREAMING_TaskHandler(void *p_arg);
void                MMPF_USB_TaskHandler(void *p_arg);
#if (MSDC_WR_FLOW_TEST_EN)
void				MMPF_USB_WriteTaskHandler(void *p_arg);
#endif
#if (SUPPORT_USB_HOST_FUNC)
void                MMPF_USB_UVCH_PostProcess_TaskHandler(void *p_arg);
void                MMPF_USB_UVCH_BulkIn_TaskHandler(void *p_arg);
#endif
#if ((DSC_R_EN)||(VIDEO_R_EN))&&((SUPPORT_FDTC)||(SUPPORT_MDTC))
void                MMPF_FDTC_TaskHandler(void *p_arg);
#endif
void                MMPF_AUDIORECORD_TaskHandler(void *p_arg);
#if !defined(MBOOT_FW)&&(HANDLE_EVENT_BY_TASK == 1)
void                MMPF_EVENT_TaskHandler(void *p_arg);
#endif

//==============================================================================
//
//                              MACRO FUNCTIONS
//
//==============================================================================

#define MMPF_OS_Sleep_MS(_ms)   MMPF_OS_Sleep((MMP_USHORT)(OS_TICKS_PER_SEC * ((MMP_ULONG)_ms + 500L / OS_TICKS_PER_SEC) / 1000L));

#endif // _INCLUDES_FW_H_


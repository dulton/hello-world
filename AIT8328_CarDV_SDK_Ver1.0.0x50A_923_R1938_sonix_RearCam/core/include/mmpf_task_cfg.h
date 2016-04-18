//==============================================================================
//
//  File        : mmpf_task_cfg.h
//  Description : Task configuration file for A-I-T MMPF source code
//  Author      : Philip Lin
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPF_TASK_CFG_H_
#define _MMPF_TASK_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

#define TASK_SYS_STK_SIZE           (1024)
#define TASK_DSC_STK_SIZE           (1024)
#define TASK_DSC_STREAM_STK_SIZE    (1024)
#define TASK_JPEG_CTL_STK_SIZE      (1024)
#define TASK_SENSOR_STK_SIZE        (1024)
#define TASK_FDTC_STK_SIZE          (768)
#define PLAYER_STK_SIZE             (1024)  // Stack size in DW (1024)
#define DECODER_STK_SIZE            (4096)  // Stack size in DW (4096)
#define DEMUXER_STK_SIZE            (4096)  // Stack size in DW (4096)
#if (VIDEO_BGPARSER_EN == 1)
#define TASK_BG_PARSER_STK_SIZE     (768)
#if (VIDEO_HIGHPRIORITY_FS == 1)
#define HIGH_PRIO_FS_STK_SIZE       (768)
#endif
#endif

#define MP4VENC_STK_SIZE            (384)   // 384 x4 Bytes
#define MERGR3GP_STK_SIZE           (1024)  // 512 x4 Bytes
#if (MP3HD_P_EN == 1)
#define AUDIO_PLAY_STK_SIZE         (5120)
#else
#define AUDIO_PLAY_STK_SIZE         (4096)
#endif
#define AUDIO_WRITEFILE_STK_SIZE    (4096)  // For MP3HD file parsing, original (1024)
#define AUDIO_RECORD_STK_SIZE    	(4096)
#define USB_STK_SIZE                (4096)  // 4096 x4 Bytes
#define USB_UVCH_POSTPROC_STK_SIZE  (4096)  // 4096 x4 Bytes
#define USB_UVCH_BULKIN_STK_SIZE    (4096)  // 4096 x4 Bytes
#if (FS_LIB == FS_UFS)
#define FS_STK_SIZE                 (1024)
#endif
#define LTASK_STK_SIZE              (128)
#if (HANDLE_EVENT_BY_TASK == 1)
#define EVENT_STK_SIZE              (256)
#endif

#define	TASK_SYS_PRIO				(1)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MMPF_TASK_CFG_H_ */

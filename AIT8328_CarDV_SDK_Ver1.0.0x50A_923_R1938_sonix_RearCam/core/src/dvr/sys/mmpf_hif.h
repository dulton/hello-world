//==============================================================================
//
//  File        : mmpf_hif.h
//  Description : INCLUDE File for the Firmware Host Interface Function
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPF_HIF_H_
#define _MMPF_HIF_H_

/** @addtogroup MMPF_HIF
@{
*/
//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_hif_cmd.h"

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

/* Flag for HIF_CMD (SYS_Flag_Hif) */
#define SYS_FLAG_SYS                        		(0x00000001)
#define SYS_FLAG_FS                         		(0x00000002)
#define	SYS_FLAG_USBOP								(0x00000004)
#define SYS_FLAG_MEMDEV_ACK                 		(0x00000008)
#define SYS_FLAG_LTASK	                    		(0x00000010)
#define SYS_FLAG_AGC	                    		(0x00000020)

/* Flag for Sensor/ISP (SENSOR_Flag) */
#define SENSOR_FLAG_VIF_GRAB_END         			(0x00000001)
#define SENSOR_FLAG_VIF_FRM_END          			(0x00000002)
#define SENSOR_FLAG_VIF_FRM_ST           			(0x00000004)
#define SENSOR_FLAG_VIF_LINE_NO          			(0x00000008)

#define SENSOR_FLAG_ISP_FRM_ST              		(0x00000010)
#define SENSOR_FLAG_ISP_FRM_END             		(0x00000020)

#define SENSOR_FLAG_SENSOR_CMD                  	(0x00000200)
#define SENSOR_FLAG_ROTDMA                  		(0x00000400)
#define SENSOR_FLAG_FDTC_DRAWRECT          	 		(0x00000800)
#define SENSOR_FLAG_PCAMOP							(0x00001000)
#define SENSOR_FLAG_CHANGE_SENSOR_MODE	    		(0x00002000)
#define SENSOR_FLAG_TRIGGER_DEINTERLACE	    		(0x00004000)
#define SENSOR_FLAG_TRIGGER_GRA_TO_PIPE	    		(0x00008000)
#define SENSOR_FLAG_ROTDMAREAR                 		(0x00010000)//For use as "Rear" Camera only
#define SENSOR_FLAG_ROTDMAPIPE3                     (0x00020000)
//add task(SENSOR_FLAG_ROTDMAREAR),for case of doing the RotateDMA of Front&Rear Image independently
#define SENSOR_FLAG_REINIT          	    		(0x00040000)

/* Flag for DSC (DSC_Flag) */
#define DSC_FLAG_DSC_CMD							(0x00000001)

/* Flag for DSC Stream (DSC_STREAM_Flag) */
#define DSC_FLAG_WRITE_FILE                    		(0x00000001)
#define DSC_FLAG_WIFI_STREAM						(0x00000002)

/* Flag for JPEG Control (JPEG_Ctl_Flag) */
#define JPG_FLAG_EXE_ENCODE                   		(0x00000001)
#define JPG_FLAG_EXE_DECODE                   		(0x00000002)
#define JPG_FLAG_FLUSH                       		(0x00000004)

/* Flag for Video Record (VID_REC_Flag) */
#define CMD_FLAG_MGR3GP                             (0x00000001)
#define CMD_FLAG_VIDRECD							(0x00000002)
#define SYS_FLAG_VIDENC                             (0x00000004)
#define SYS_FLAG_MGR3GP                             (0x00020000)
#define SYS_FLAG_DVS_RUN                            (0x00040000)
#define SYS_FLAG_VIDRECD_CB                         (0x00080000)

/* Flag for Wifi Streaming (Streaming_Flag) */
#define STREAM_FLAG_JPEG1_STREAM                  	(0x00000001)
#define STREAM_FLAG_JPEG2_STREAM            	    (0x00000002)
#define STREAM_FLAG_PCM_STREAM                  	(0x00000004)
#define STREAM_FLAG_MP3_STREAM                  	(0x00000008)
#define STREAM_FLAG_AAC_STREAM                    	(0x00000010)
#define STREAM_FLAG_PUMP_STREAM                  	(0x00000020)
#define STREAM_FLAG_MSG_STREAM                   	(0x00000040)
#define STREAM_FLAG_RESET_ALL                   	(0x00003000)
#define STREAM_FLAG_RESET_AUDIO                   	(0x00001000)
#define STREAM_FLAG_RESET_VIDEO1                   	(0x00002000)
#define STREAM_FLAG_RESET_VIDEO2                   	(0x00004000)

/* Flags for Network */
#define	SYS_FLAG_MSG_NETWORK_INIT					(0x00000001)
#define	SYS_FLAG_MSG_NETWORK_CMD					(0x00000002)
#define	SYS_FLAG_MSG_NETWORK_UNIN					(0x00000004)
#define	SYS_FLAG_MSG_NETWORK_RESET					(0x00000008)
#define	SYS_FLAG_MSG_NETWORK_UINOTIFY				(0x00000010)
#define	SYS_FLAG_MSG_NETWORK_JOB					(0x00000020)
#define	SYS_FLAG_MSG_NETWORK_WIFILINK_IND			(0x00000040)
#define	SYS_FLAG_MSG_NETWORK_WIFILINK_DOWN_IND		(0x00000080)

/* Flag for Audio Play (AUD_PLAY_Flag) */
#define AUD_FLAG_AUDIOPLAY                          (0x00000001)
#define AUD_FLAG_AUDIO_PREVIEW                      (0x00000002)
#define AUD_FLAG_AUDIO_BYPASS                       (0x00000004)

/* Flag for Audio Record (AUD_REC_Flag) */
#define AUD_FLAG_AUDIOREC                           (0x00000001)
#define AUD_FLAG_GAPLESS_CB                         (0x00000002)

/* Flag for Audio Streaming (AUD_STREAM_Flag) */
#define AUD_FLAG_AUDIOWRITE_FILE                    (0x00000001)
#define AUD_FLAG_AUDIOREAD_FILE                     (0x00000002)
#define AUD_FLAG_MP3_GET_INFO                       (0x00000004)
#define AUD_FLAG_AUI_READ_FILE                      (0x00000008)

/* Flag for Video Play (VID_PLAY_Flag) */
#define SYS_FLAG_VIDPLAY                            (0x00000001)
#define SYS_FLAG_PSR3GP                             (0x00000002)
#define SYS_FLAG_VID_DEC                            (0x00000004)
#define SYS_FLAG_DEMUX                              (0x00000008)
#define	SYS_FLAG_DEBUG								(0x00000010)
#define SYS_FLAG_FW_VIDPLAY                         (0x00000020)
#define SYS_FLAG_VID_CMD                            (0x00000040)

/* Flag for USB operation (USB_OP_Flag) */
#define USB_FLAG_GENOP								(0x00000001)
#define USB_FLAG_CLSOP								(0x00000002)
#define USB_FLAG_XU_CMD_OP                          (0x00000004)
#define USB_FLAG_UVC								(0x00000008)
#define USB_FLAG_UVC_START_PREVIEW					(0x00000010)
#define USB_FLAG_UVC_STOP_PREVIEW					(0x00000020)
#define USB_FLAG_USBH_GET_PKT_REQ				    (0x00000040)
#define USB_FLAG_USBH_PROC_PKT_REQ				    (0x00000080)
#define USB_FLAG_USBH_STOP_UVC_REQ				    (0x00000100)
#define USB_FLAG_USBH_INPUT_FRAME_REQ			    (0x00000200)
#define USB_FLAG_USBH_SYNC_UVC_TIME_REQ			    (0x00000400)
#define USB_FLAG_USBH_COPY_FRM_REQ				    (0x00000800)
#define USB_FLAG_UAC_START                          (0x00001000)
#define USB_FLAG_UAC_STOP                           (0x00002000)
#define USB_FLAG_UAC_SAMPLE_RATE                    (0x00004000)
#define USB_FLAG_UAC_MUTE                           (0x00008000)
#define USB_FLAG_UAC_VOLUME                         (0x00010000)
#define USB_FLAG_USBH_DMA_RX_DONE_REQ			    (0x00020000)
#define USB_FLAG_USBH_ROTE_FRM_REQ				    (0x00040000)

//==============================================================================
//
//                              EXTERN VARIABLE
//
//==============================================================================

extern MMP_ULONG    m_ulHifCmd[GRP_IDX_NUM];
extern MMP_ULONG    m_ulHifParam[GRP_IDX_NUM][MAX_HIF_ARRAY_SIZE];

typedef MMP_ULONG   MMPF_OS_FLAGID; //Ref:os_wrap.h

extern MMPF_OS_FLAGID	SYS_Flag_Hif;
extern MMPF_OS_FLAGID	AUD_PLAY_Flag;
extern MMPF_OS_FLAGID	SENSOR_Flag;
extern MMPF_OS_FLAGID	DSC_Flag;
extern MMPF_OS_FLAGID	DSC_STREAM_Flag;
extern MMPF_OS_FLAGID 	AUD_REC_Flag;
extern MMPF_OS_FLAGID	AUD_STREAM_Flag;
extern MMPF_OS_FLAGID	VID_REC_Flag;
extern MMPF_OS_FLAGID	VID_PLAY_Flag;
extern MMPF_OS_FLAGID	USB_OP_Flag;
extern MMPF_OS_FLAGID	DSC_UI_Flag;
extern MMPF_OS_FLAGID	Streaming_Flag;
extern MMPF_OS_FLAGID	Network_Flag;
extern MMPF_OS_FLAGID   JPEG_Ctl_Flag;

#if ((DSC_R_EN)||(VIDEO_R_EN))&&((SUPPORT_FDTC)||(SUPPORT_MDTC))
extern MMPF_OS_FLAGID   FDTC_Flag;
#endif

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

typedef enum _MMPF_HIF_INT_FLAG
{
    MMPF_HIF_INT_VIDEOR_END     = 0x00000001,
    MMPF_HIF_INT_VIDEOP_END     = 0x00000002,
    MMPF_HIF_INT_AUDIOR_END     = 0x00000004,
    MMPF_HIF_INT_AUDIOP_END     = 0x00000008,
    MMPF_HIF_INT_AUDIOR_GETDATA = 0x00000010,
    MMPF_HIF_INT_AUDIOP_FILLBUF = 0x00000020,
    MMPF_HIF_INT_VIDEOR_MOV     = 0x00000040,
    MMPF_HIF_INT_VIDEOP_MOV     = 0x00000080,
    MMPF_HIF_INT_FDTC_END       = 0x00000100,
    MMPF_HIF_INT_SDCARD_REMOVE  = 0x00000200,
    MMPF_HIF_INT_MDTV_ESG_DONE  = 0x00000400,
    MMPF_HIF_INT_MDTV_BUF_OK    = 0x00000800,
    MMPF_HIF_INT_SBC_EMPTY_BUF  = 0x00001000,
    MMPF_HIF_INT_WAV_EMPTY_BUF  = 0x00002000,
    MMPF_HIF_INT_USB_SUSPEND    = 0x00004000,
    MMPF_HIF_INT_USB_HOST_DEV   = 0x00008000,
    MMPF_HIF_INT_SMILE_SHOT     = 0x00010000
} MMPF_HIF_INT_FLAG;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

void MMPF_HIF_ResetCmdStatus(void);
void MMPF_HIF_SetCmdStatus(MMP_ULONG status);
void MMPF_HIF_ClearCmdStatus(MMP_ULONG status);
MMP_ULONG MMPF_HIF_GetCmdStatus(MMP_ULONG status);

void MMPF_HIF_SetCpu2HostInt(MMPF_HIF_INT_FLAG status);

void MMPF_HIF_FeedbackParamL(MMP_UBYTE ubGroup, MMP_UBYTE ubParamnum, MMP_ULONG ulParamdata);
void MMPF_HIF_FeedbackParamW(MMP_UBYTE ubGroup, MMP_UBYTE ubParamnum, MMP_USHORT usParamdata);
void MMPF_HIF_FeedbackParamB(MMP_UBYTE ubGroup, MMP_UBYTE ubParamnum, MMP_UBYTE ubParamdata);

#endif // _MMPF_HIF_H_
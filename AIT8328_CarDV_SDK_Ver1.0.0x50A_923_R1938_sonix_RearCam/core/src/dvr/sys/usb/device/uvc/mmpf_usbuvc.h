#ifndef USBUVC_H
#define USBUVC_H

#include "mmpf_typedef.h"
#include "mmpf_usbpccam.h"
#include "mmpf_usb_h264.h"
#include "mmpf_dma.h"
#include "mmp_usb_inc.h"

// total numbers of video frames
#if (ENABLED_PCAM_2M_RES)
#define NUM_YUY2_VIDEO (7)
#define NUM_MJPG_VIDEO (7)
#else
#define NUM_YUY2_VIDEO (6)
#define NUM_MJPG_VIDEO (6)
#endif
#define NUM_H264_VIDEO (5)

// total numbers of still frames
#define NUM_YUY2_STILL 3
#define NUM_MJPG_STILL 3
/*
//still capture
#define STILL_IMAGE_WAITING 0
#define STILL_IMAGE_TRIGGER 1
#define STILL_IMAGE_PREVIEW_STOP 2
#define STILL_IMAGE_SENDING 3
#define STILL_IMAGE_FINISH 4
*/
//still capture
typedef enum _MMPF_USBUVC_CAPTURE_EVENT {

	STILL_IMAGE_WAITING 		= 0,
	STILL_IMAGE_TRIGGER 		= 1,
	STILL_IMAGE_PREVIEW_STOP 	= 2,
	STILL_IMAGE_SENDING 		= 3,
	STILL_IMAGE_FINISH 			= 4

} MMPF_USBUVC_CAPTURE_EVENT;

typedef struct _DataExchangeParam
{
	MMP_USHORT	id;			//command ID
	MMP_ULONG	data_len;	//transfer lens
	MMP_ULONG	cur_offset;	//current read/write offset
	MMP_BYTE	*ptr;		//data buffer
}DataExchangeParam;

typedef void (*USB_Cmd2TaskCB)(void);
void *USB_GetCommandToTask(void);
void USB_SetCommandToTask(void *pfUSBCmd2Task);
void usb_vc_preview_enable(void);
void usb_vc_preview_disable(void);

#define UVC_SEND_IMG_RET_CONTINUE 0x00
#define UVC_SEND_IMG_RET_END_FRAME 0x01
MMP_UBYTE usb_vc_send_image(MMP_UBYTE);
void usb_vc_send_packet(MMP_ULONG cur_pt,MMP_USHORT xsize) ;
MMP_ULONG usb_vc_move_packet(STREAM_DMA_BLK *dma_blk,MMP_UBYTE endbit,MMP_UBYTE sti_mode,MMP_UBYTE *pad_app3_header,MMP_ULONG *sent_size);
MMP_ULONG usb_vc_empty_packet(STREAM_DMA_BLK *dma_blk,MMP_UBYTE endbit,MMP_UBYTE sti_mode);

MMP_ERR usb_vc_wait_dma_done(MMP_ULONG timeout);
void usb_vc_set_still_commit(STILL_PROBE* pSCIN);
void usb_vc_get_still_probe(MMP_UBYTE FormatIndex,MMP_UBYTE FrameIndex ,STILL_PROBE* pSP);
void usb_vc_set_still_probe(MMP_UBYTE FormatIndex,MMP_UBYTE FrameIndex);
void usb_vc_set_still_resolution(MMP_UBYTE FormatIndex,MMP_UBYTE FrameIndex);
MMP_UBYTE SetH264Mode(MMP_UBYTE Mode);
void usb_uvc_set_h264_mode(MMP_UBYTE FormatIndex,MMP_UBYTE FrameIndex);
void usb_uvc_start_preview(void);

MMP_BOOL usb_uvc_config_stream(void);
void usb_uvc_init_stream(MMP_BOOL mjpeg_stream);
void usb_uvc_stop_stream(void);
void usb_uvc_start_stream(MMP_BOOL mjpeg_stream,STREAM_CFG *stream_cfg,MMP_ULONG res);
void usb_uvc_fill_payload_header(MMP_UBYTE *frame_ptr,MMP_ULONG framelength,MMP_ULONG frameseq,MMP_ULONG flag,MMP_ULONG timestamp,MMP_USHORT w,MMP_USHORT h,MMP_USHORT type,MMP_USHORT framerate);
void usb_skype_fill_payload_header(MMP_UBYTE *frame_ptr,MMP_ULONG framelength,MMP_ULONG frameseq,MMP_ULONG flag,MMP_ULONG timestamp,MMP_USHORT w,MMP_USHORT h,MMP_USHORT type,MMP_USHORT framerate);
void usb_frameh264_fill_payload_header(MMP_UBYTE *frame_ptr,MMP_ULONG framelength,MMP_ULONG frameseq,MMP_ULONG flag,MMP_ULONG timestamp,MMP_USHORT w,MMP_USHORT h,MMP_USHORT pw,MMP_USHORT ph,MMP_USHORT type,MMP_USHORT framerate);
MMP_UBYTE *usb_uvc_search_app3_insert_addr(MMP_UBYTE *jpeg,MMP_ULONG len,MMP_USHORT tag);

void uvc_init(void);
void usb_vc_req_proc(MMP_UBYTE req);
void usb_vc_update_automode(void);
void usb_vc_out_data(void);
void usb_vc_capture_trigger(MMP_UBYTE pressed);
void usb_uvc_stop_preview(void);
void usb_vc_update_async_mode(MMP_UBYTE bOriginator,MMP_UBYTE bSelector,MMP_UBYTE bAttribute,void *bValue,MMP_UBYTE bValUnit);
MMP_ULONG usb_vc_take_raw_picture(MMP_UBYTE ubSnrSel, MMP_UBYTE raw_mode, MMP_ULONG srcAddr, MMP_ULONG dstAddr);
volatile MMP_UBYTE *usb_vc_fill_header(volatile MMP_UBYTE *ptr,MMP_UBYTE bfh,MMP_ULONG pts,MMP_ULONG stc,MMP_USHORT sofnum);
H264_ENCODER_VFC_CFG *usb_get_uvc_h264_cfg(void);
void usb_vc_cmd_cfg(MMP_UBYTE req,VC_CMD_CFG *cfg,MMP_ULONG cur_val);

void usb_uvc_ibc_frmRdy_callback(MMP_UBYTE usIBCPipe);
void usb_uvc_mjpeg_tx(void);

#endif

//==============================================================================
//
//  File        : mmpf_usbh_ctl.h
//  Description : Header file for the USB HOST controller function
//  Author      : Bruce_Chien
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPF_USBH_CTL_H_
#define _MMPF_USBH_CTL_H_

#include "includes_fw.h"
#include "mmpf_typedef.h"

#if (USB_EN)&&(SUPPORT_USB_HOST_FUNC)
#include "mmpf_usbuvc.h"
#include "mmpf_usbvend.h"
#include "mmpf_usbh_cfg.h"
#include "mmpf_usbh_core.h"
#include "mmpf_usbh_uvc.h"
#include "aitu_ringbuf.h"
#include "mmp_graphics_inc.h"
#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
#include "mmp_dsc_inc.h"
#endif

#define DMA3_RX_LAST_PKT_PATCH          (1)
#define EN_USBH_CHECK_RINGBUF_STS       (1)
#define EN_EP0_SEM                      (1)
#define EN_REMOVE_TRAN_HW_TIMER         (1)     /* remove CheckTranTimerAttr, merge transaction timer check into CheckFrmRxTimerAttr */
#define EN_USBH_ALWAYS_INPUT_FRM        (1)     //1: always input frame to Mgr, 0: input frame depend on ubUVCRecdEn,
#define EN_DBGLOG_TRIGUSBDMA            (0)
#if (SUPPORT_SONIX_UVC_ISO_MODE==1)
#define EN_USB_EP_ISOLATION             (0)     //0: disable, ISO mode do not support, endpoint x (USB Rx DMA trigger) always at ISR
#else
#define EN_USB_EP_ISOLATION             (0)     //0: disable, 1: enable semaphore to isolate endpoint 0 and endpoint 1, prevent work together 
#endif

//==============================================================================
//
//                              DATA TYPES
//
//==============================================================================

typedef MMP_ULONG u32;

//==============================================================================
//
//                              CONSTANT
//
//==============================================================================

#define SEM_RES_0                           (0)
#define SEM_RES_1                           (1)

/* YUV420 color pattern */
#define YUV420_Y_BLACK_COLOR                (0x00)
#define YUV420_UV_BLACK_COLOR               (0x80)

/* USB Connect Speed */
#define USB_XFER_SPEED                      (USB_HIGH_SPEED)

/* USB Max Packet Size */
#define EP0_MAX_PACKET_SIZE                 (64)
#if (USB_XFER_SPEED == USB_FULL_SPEED)
#define BULK_MAX_PACKET_SIZE                (64)
#define BULK_PACKET_CNT(n)                  ((n+63) >> 6)
#elif (USB_XFER_SPEED == USB_HIGH_SPEED)
#define BULK_MAX_PACKET_SIZE                (512)
#define BULK_PACKET_CNT(n)                  ((n+511) >> 9)
#endif

#define ISO_MAX_PACKET_SIZE                 (1024)

/* bRequest (AIT specified) */
#define ROOT_DIR                            (0x00)
    #define ROOT_DIR_DATA_SZ                (0x04)
#define GET_FILE_LIST                       (0x01)
    #define FILE_ATTR_READ_ONLY             (0x01)
    #define FILE_ATTR_HIDDEN                (0x02)
    #define FILE_ATTR_DIR                   (0x10)
#define SELECT_ENTRY                        (0x02)
    #define SELECT_ENTRY_DATA_SZ            (0x08)
#define START_XFER                          (0x03)
    #define START_XFER_DATA_SZ              (0x04)
#define ABORT_XFER                          (0x04)
    #define ABORT_XFER_DATA_SZ              (0x04)

/* descriptor size */
#define DEVICE_DESCR_SIZE                   (0x12)
#define CONFIG_DESCR_SIZE                   (0x09)
#define STRING_DESCR_SIZE                   (0x04)
#define INTERFACE_DESCR_SIZE                (0x09)
#define EP_DESCR_SIZE                       (0x07)
#define DEV_QUALIFIER_DESCR_SIZE            (0x0A)
#define OTHER_SPEED_DESCR_SIZE              (0x09)

#define MSDC_GET_MAX_LUN                    (0xFE)

/* Mass Storage Related */
#define CBW_PACKET_SIZE                     (0x1F)
#define CSW_PACKET_SIZE                     (0x0D)
#define REQ_SENSE_DATA_SIZE                 (0x12)
#define INQUIRY_DATA_SIZE                   (0x24)
#define RTL_BULKOUT_SIZE                    (118)

/* Mass Storage Device Command */
#define CMD_AIT_SCSI_OPCODE                 (0xFA)  /* AIT-Specific command */

/* OTG Example Related */
#define BULK_IN_EP                          (1)
#define BULK_OUT_EP                         (2)

/* Standard Feature Selectors @ USB 2.0 Table 9-6 */
#define DEV_REMOTE_WAKEUP                   (0x01)
#define EP_HALT                             (0x00)
#define DEV_TEST_MODE                       (0x02)

/* error message info relatives */
#define USBH_ERR_MSG_QUEUE_SIZE             (64)    //queue size of error mesage
#define USBH_ERR_STR_LEN                    (32)    
#define USBH_ERR_NONE                       (0)     
#define USBH_ERR_ROTE_DMA_FAIL              (1)     //rotate DMA NG
#define USBH_ERR_MOVE_DMA_FAIL              (2)     //Move DMA NG
#define USBH_ERR_BAD_UVC_HDR_YUV            (3)     //no UVC header in transfer of YUV frame 
#define USBH_ERR_BAD_UVC_HDR_H264           (4)     //no UVC header in transfer of H264 frame 
#define USBH_ERR_BAD_JPG_HDR                (5)     //first packet of MJPEG frame not 0xFF 0xD8
#define USBH_ERR_BAD_PACKET_SIZE            (6)     //packet size greater than max packet size
#define USBH_ERR_BAD_FRM_SIZE               (7)     //video frame size greater than max video frame size
#define USBH_ERR_BAD_EOI                    (8)     //can not find EOI (w/EOF) over MMPF_USBH_LOOKUP_MAX_CNT packet
#define USBH_ERR_WRONG_WR_PTR               (9)     //WR pointer wrong, check update case
#define USBH_ERR_DEC_BUF_FULL               (10)    //decode JPG buffer full
#define USBH_ERR_RX_BUF_FULL                (11)    //USB Rx buffer full, not enough to receive one frame
#define USBH_ERR_RECD_INFO_Q_FULL           (12)    //Record frame info queue full(for size, timestamp, start offset)
#define USBH_ERR_RING_BUF_ST                (13)    //ring buffer overflow/underflow status, status, size, rd, wr
#define USBH_ERR_RING_BUF_ST2               (14)    //ring buffer overflow/underflow status2, rd_wrap. wr_wrap
#define USBH_ERR_PREVW_BUF_FULL             (15)    //preview buffer full
#define USBH_ERR_PREVW_ROTE_BUF_FULL        (16)    //preview rotate buffer full

#if (SUPPORT_SONIX_UVC_ISO_MODE==1)
/* Backup & restore memory, use to reduce MoveDMA operation */
#define USBH_BACKUP_DATA_NUM                (3)
#define USBH_BACKUP_DATA_FULL_SIZE          (12)
#endif

/* Endpoint 0 memory */
#define EP0_SETUP_PACKET_SIZE               (8)
#define EP0_DATA_PACKET_SIZE                (4096)

/* Receive packet memory */
#define EP_RX_TMP_BUF_SIZE                  (1024+32)  //use when out of receive packet buffer
#if (SUPPORT_SONIX_UVC_ISO_MODE==1)
#define EP_RX_BUF_SIZE                      (8192)
#define EP_RX_BUF_CNT                       (192)   //from RECD_FRM_BUF_SZ*RECD_FRM_BUF_CNT/EP_RX_BUF_SIZE
#else
#define EP_RX_BUF_SIZE                      (8192)
#define EP_RX_BUF_CNT                       (8)
#endif
#define EP_RX_FULL_BUF_SIZE                 (EP_RX_BUF_CNT*EP_RX_BUF_SIZE)

/* Record frame memory */
#if (SUPPORT_SONIX_UVC_ISO_MODE==1)
#define RECD_FRM_BUF_SZ                     (ISO_MJPEG_MAX_VIDEO_FRM_SIZE)
#define RECD_FRM_BUF_CNT                    (3)
#define RECD_FRM_Q_SZ                       (48)    //# queue to keep record frame info, e.g., size, timestamp, type, start offset
#define RECD_FRM_CPLT_Q_SZ                  (40)    //# queue to keep(TRUE/FALSE) record frame if complete(one frame) by this moveDMA operation (cut UVC header)
#else
#define RECD_FRM_BUF_SZ                     (0x10000)
#define RECD_FRM_BUF_CNT                    (15)//(50 to dump 10 frames, (4) is original
#define RECD_BUF_NUM_PER_FRM                (5)//estimate worst case: 10x (avg frame size 34953B/frame <= 8Mbps/30fps)
#define RECD_BUF_FRM_SLOT_CNT               (RECD_FRM_BUF_CNT/RECD_BUF_NUM_PER_FRM)
#define RECD_FRM_MAX_SZ                     (RECD_BUF_NUM_PER_FRM*RECD_FRM_BUF_SZ)
#define RECD_FRM_Q_SZ                       (32)
#define RECD_FRM_CPLT_Q_SZ                  (40)//about (RECD_FRM_BUF_SZ/EP_RX_BUF_SIZE)
#endif

/* Preview frame memory */
#define PREVW_FRM_BUF_CNT                   (3)
#define PREVW_FRM_ROTE_BUF_CNT              (3)
#if (DEVICE_YUV_TYPE==ST_YUY2)
#define PREVW_FRM_CPLT_Q_SZ                 (75)//about (PREVW_BUF_SIZE/EP_RX_BUF_SIZE)
#else
#define PREVW_FRM_CPLT_Q_SZ                 (57)//about (PREVW_BUF_SIZE/EP_RX_BUF_SIZE), # queue to keep(TRUE/FALSE) preview frame if complete(one frame) by this moveDMA operation (cut UVC header)
#endif

/* Related constant */
#define AIT_H264_FRM_START_CODE             (0x00000001)
#define UVC_PH_LEN                          sizeof(UVC_VIDEO_PAYLOAD_HEADER)
#define PKT_64BYTE_LEN                      (64) //(UVC_PH_LEN + FRAME_PAYLOAD_HEADER_SZ)

/* Error handle limitation */
#define FRM_TRAN_TIMEOUT_MIN                (1) // min timeout to check transaction transfer in millisecond
#define FRM_RX_TIMEOUT_HDL_0                (0) // handle of check frame Rx lost TH
#define FRM_RX_TIMEOUT_HDL_1                (1) // handle of check frame Rx lost TH2
#define FRM_RX_TIMEOUT_MAX_HDL              (2) // max handles to check frame Rx lost
#define FRM_RX_LOST_MIN_TH                  (4) // min threshold to check frame Rx lost count
#define FRM_SEQ_NO_LOST_MIN_TH              (1) // min threshold to check frame seq. no lost count

#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
/* gbDecJpgErrSts */
#define DEC_JPG_MARKER_ERR                  (0x04) /* SW status refer to JPG_INT_DEC_MARKER_ERR */
#define DEC_JPG_HUFFMAN_ERR                 (0x08) /* SW status refer to  JPG_INT_DEC_HUFFMAN_ERR */
#define DEC_JPG_NONBASELINE                 (0x10) /* SW status refer to  JPG_INT_DEC_NONBASELINE */
#define DEC_JPG_CLR_ERR_STS                 (0x00) /* Clear SW status */
#endif

//==============================================================================
//
//                              MACRO FUNC
//
//==============================================================================

#define UGETB(w) (w)
#define USETB(w,v) (w = v)
#define UGETW(w) ((w)[0] | ((w)[1] << 8))
#define USETW(w,v) ((w)[0] = (uByte)(v), (w)[1] = (uByte)((v) >> 8))
#define UGETDW(w) ((w)[0] | ((w)[1] << 8) | ((w)[2] << 16) | ((w)[3] << 24))
#define USETDW(w,v) ((w)[0] = (uByte)(v), \
             (w)[1] = (uByte)((v) >> 8), \
             (w)[2] = (uByte)((v) >> 16), \
             (w)[3] = (uByte)((v) >> 24))

//bTerminalID of Video Control Input Terminal ==> UVC_IT_ID 
//bUnitID of Video Control Processing Unit ==> UVC_PU_ID
//bInterfaceNumber of Video Control Interface SubClass ==> UVC_VC_VCI_EP
//bInterfaceNumber of Video Streaming Interface ==> UVC_VC_VSI_EP
//bEndpointAddress of VS Video Input Header Descriptor ==> attribute->ep_num of RX
//bUnitID of Video Control Extension Unit ==> UVC_EU3_ID,UVC_EU2_ID,UVC_EU1_ID,UVC_EU0_ID
#define MMPF_USBH_SetVCCmd(ubReq,usVal,usLen,pubDataBuf)                MMPF_USBH_SetClassIfCmd(ubReq, (usVal<<8), (gUsbh_UvcDevInfo.bIfNoVC), usLen, (MMP_UBYTE *)pubDataBuf)
#define MMPF_USBH_GetVCCmd(ubReq,usVal,usLen,pulDataLen,pubDataBuf)     MMPF_USBH_GetClassIfCmd(ubReq, (usVal<<8), (gUsbh_UvcDevInfo.bIfNoVC), usLen, (MMP_ULONG *)pulDataLen, (MMP_UBYTE *)pubDataBuf)
#define MMPF_USBH_SetVSCmd(ubReq,usVal,usLen,pubDataBuf)                MMPF_USBH_SetClassIfCmd(ubReq, (usVal<<8), (gUsbh_UvcDevInfo.bIfNoVS), usLen, (MMP_UBYTE *)pubDataBuf)
#define MMPF_USBH_GetVSCmd(ubReq,usVal,usLen,pulDataLen,pubDataBuf)     MMPF_USBH_GetClassIfCmd(ubReq, (usVal<<8), (gUsbh_UvcDevInfo.bIfNoVS), usLen, (MMP_ULONG *)pulDataLen, (MMP_UBYTE *)pubDataBuf)
#define MMPF_USBH_SetCTCmd(ubReq,usVal,usLen,pubDataBuf)                MMPF_USBH_SetClassIfCmd(ubReq, (usVal<<8), ((gUsbh_UvcDevInfo.bIdIT<<8)|gUsbh_UvcDevInfo.bIfNoVC), usLen, (MMP_UBYTE *)pubDataBuf)
#define MMPF_USBH_GetCTCmd(ubReq,usVal,usLen,pulDataLen,pubDataBuf)     MMPF_USBH_GetClassIfCmd(ubReq, (usVal<<8), ((gUsbh_UvcDevInfo.bIdIT<<8)|gUsbh_UvcDevInfo.bIfNoVC), usLen, (MMP_ULONG *)pulDataLen, (MMP_UBYTE *)pubDataBuf)
#define MMPF_USBH_SetPUCmd(ubReq,usVal,usLen,pubDataBuf)                MMPF_USBH_SetClassIfCmd(ubReq, (usVal<<8), ((gUsbh_UvcDevInfo.bIdPU<<8)|gUsbh_UvcDevInfo.bIfNoVC), usLen, (MMP_UBYTE *)pubDataBuf)
#define MMPF_USBH_GetPUCmd(ubReq,usVal,usLen,pulDataLen,pubDataBuf)     MMPF_USBH_GetClassIfCmd(ubReq, (usVal<<8), ((gUsbh_UvcDevInfo.bIdPU<<8)|gUsbh_UvcDevInfo.bIfNoVC), usLen, (MMP_ULONG *)pulDataLen, (MMP_UBYTE *)pubDataBuf)

#define MMPF_USBH_SetEU0Cmd(ubReq,usVal,usLen,pubDataBuf)               MMPF_USBH_SetClassIfCmd(ubReq, (usVal<<8), ((gUsbh_UvcDevInfo.bIdEU<<8)|gUsbh_UvcDevInfo.bIfNoVC), usLen, (MMP_UBYTE *)pubDataBuf)
#define MMPF_USBH_GetEU0Cmd(ubReq,usVal,usLen,pulDataLen,pubDataBuf)    MMPF_USBH_GetClassIfCmd(ubReq, (usVal<<8), ((gUsbh_UvcDevInfo.bIdEU<<8)|gUsbh_UvcDevInfo.bIfNoVC), usLen, (MMP_ULONG *)pulDataLen, (MMP_UBYTE *)pubDataBuf)
#define MMPF_USBH_SetEU1Cmd(ubReq,usVal,usLen,pubDataBuf)               MMPF_USBH_SetClassIfCmd(ubReq, (usVal<<8), ((gUsbh_UvcDevInfo.bIdEU<<8)|gUsbh_UvcDevInfo.bIfNoVC), usLen, (MMP_UBYTE *)pubDataBuf)
#define MMPF_USBH_GetEU1Cmd(ubReq,usVal,usLen,pulDataLen,pubDataBuf)    MMPF_USBH_GetClassIfCmd(ubReq, (usVal<<8), ((gUsbh_UvcDevInfo.bIdEU<<8)|gUsbh_UvcDevInfo.bIfNoVC), usLen, (MMP_ULONG *)pulDataLen, (MMP_UBYTE *)pubDataBuf)
#define MMPF_USBH_SetEU2Cmd(ubReq,usVal,usLen,pubDataBuf)               MMPF_USBH_SetClassIfCmd(ubReq, (usVal<<8), ((gUsbh_UvcDevInfo.bIdEU<<8)|gUsbh_UvcDevInfo.bIfNoVC), usLen, (MMP_UBYTE *)pubDataBuf)
#define MMPF_USBH_GetEU2Cmd(ubReq,usVal,usLen,pulDataLen,pubDataBuf)    MMPF_USBH_GetClassIfCmd(ubReq, (usVal<<8), ((gUsbh_UvcDevInfo.bIdEU<<8)|gUsbh_UvcDevInfo.bIfNoVC), usLen, (MMP_ULONG *)pulDataLen, (MMP_UBYTE *)pubDataBuf)

#define MMPF_USBH_SetFUCmd(ubReq,usVal,usLen,pubDataBuf)                MMPF_USBH_SetClassIfCmd(ubReq, (usVal<<8), ((UAC_FU_ID<<8)|UAC_AC_AS_EP), usLen, (MMP_UBYTE *)pubDataBuf)
#define MMPF_USBH_GetFUCmd(ubReq,usVal,usLen,pulDataLen,pubDataBuf)     MMPF_USBH_GetClassIfCmd(ubReq, (usVal<<8), ((UAC_FU_ID<<8)|UAC_AC_AS_EP), usLen, (MMP_ULONG *)pulDataLen, (MMP_UBYTE *)pubDataBuf)

//------------------------------------------------------------------------------
//  Function    : _MMPF_USBH_ASSERT
//  Description : assert function
//------------------------------------------------------------------------------
#define _MMPF_USBH_ASSERT(expr, file, line)  {  do {                    \
                                                RTNA_DBG_Str(0, expr);  \
                                                RTNA_DBG_Str(0, file);  \
                                                RTNA_DBG_Long(0, line); \
                                                } while(1); }

//------------------------------------------------------------------------------
//  Function    : MMPF_USBH_InitRBufHandle
//  Description : Initialize the ring buffer handler
//------------------------------------------------------------------------------
#define MMPF_USBH_InitRBufHandle(psRHdl,pvBuf,pulSize)      AUTL_RingBuf_Init((AUTL_RINGBUF *)psRHdl, (void *)pvBuf, pulSize)

//------------------------------------------------------------------------------
//  Function    : MMPF_USBH_StrictRBufReadPtr
//  Description : Advance the read pointer of ring buffer
//------------------------------------------------------------------------------
#define MMPF_USBH_StrictAdvRBufRdPtr(psRHdl, pulSize)       AUTL_RingBuf_StrictCommitRead((AUTL_RINGBUF *)psRHdl, pulSize)

//------------------------------------------------------------------------------
//  Function    : MMPF_USBH_StrictRBufWritePtr
//  Description : Advance the write pointer of ring buffer
//------------------------------------------------------------------------------
#define MMPF_USBH_StrictAdvRBufWrPtr(psRHdl, pulSize)       AUTL_RingBuf_StrictCommitWrite((AUTL_RINGBUF *)psRHdl, pulSize)

//------------------------------------------------------------------------------
//  Function    : MMPF_USBH_AdvRBufRdPtr
//  Description : Advance the read pointer of ring buffer
//------------------------------------------------------------------------------
#define MMPF_USBH_AdvRBufRdPtr(psRHdl, pulSize)       AUTL_RingBuf_CommitRead((AUTL_RINGBUF *)psRHdl, pulSize)

//------------------------------------------------------------------------------
//  Function    : MMPF_USBH_AdvRBufWrPtr
//  Description : Advance the write pointer of ring buffer
//------------------------------------------------------------------------------
#define MMPF_USBH_AdvRBufWrPtr(psRHdl, pulSize)      AUTL_RingBuf_CommitWrite((AUTL_RINGBUF *)psRHdl, pulSize)

//------------------------------------------------------------------------------
//  Function    : MMPF_USBH_IsRBufFull
//  Description : Check if ring buffer full
//------------------------------------------------------------------------------
#define MMPF_USBH_IsRBufFull(psRHdl)                        AUTL_RingBuf_Full((AUTL_RINGBUF *)psRHdl)

//------------------------------------------------------------------------------
//  Function    : MMPF_USBH_GetRBufFreeSpace
//  Description : Get the available free space in ring buffer
//------------------------------------------------------------------------------
#define MMPF_USBH_GetRBufFreeSpace(psRHdl, pulSpace)        AUTL_RingBuf_SpaceAvailable((AUTL_RINGBUF *)psRHdl, (MMP_ULONG *)pulSpace)

//------------------------------------------------------------------------------
//  Function    : MMPF_USBH_CheckRBufSpaceSts
//  Description : Check the space status in ring buffer
//------------------------------------------------------------------------------
#if (EN_USBH_CHECK_RINGBUF_STS)
#define MMPF_USBH_CheckRBufSpaceSts(psRHdl) {   MMP_LONG    lRet;   \
                                                MMP_ULONG   ulFreeSize; \
                                                lRet = MMPF_USBH_GetRBufFreeSpace(psRHdl, &ulFreeSize);   \
                                                if(lRet < RINGBUF_SUCCESS)  \
                                                {   \
                                                MMPF_USBH_SendErrMsg(USBH_ERR_RING_BUF_ST, (MMP_ULONG)lRet, __LINE__,0,0);    \
                                                MMPF_USBH_SendErrMsg(USBH_ERR_RING_BUF_ST, ulFreeSize, __LINE__,0,0);    \
                                                MMPF_USBH_SendErrMsg(USBH_ERR_RING_BUF_ST, (MMP_ULONG)(psRHdl.ptr.rd), (MMP_ULONG)(psRHdl.ptr.wr),0,0);    \
                                                MMPF_USBH_SendErrMsg(USBH_ERR_RING_BUF_ST2, (MMP_ULONG)(psRHdl.ptr.rd_wrap), (MMP_ULONG)(psRHdl.ptr.wr_wrap),0,0);    }}
#define MMPF_USBH_CheckRBufOvSts(plRet)     {   if(plRet < RINGBUF_SUCCESS)  \
                                                {   DBG_S(0, "[ERR]: Ring Buffer Ov:");   \
                                                    DBG_L(0, plRet); \
                                                    DBG_S(0, "! ");   \
                                                    DBG_S(0, __func__);   \
                                                    DBG_S(0, ",");   \
                                                    DBG_L(0, __LINE__);   \
                                                    DBG_S(0, ".\r\n");   }}
#define MMPF_USBH_CheckRBufUnSts(plRet)     {   if(plRet < RINGBUF_SUCCESS)  \
                                                {   DBG_S(0, "[ERR]: Ring Buffer Un:");   \
                                                    DBG_L(0, plRet); \
                                                    DBG_S(0, "! ");   \
                                                    DBG_S(0, __func__);   \
                                                    DBG_S(0, ",");   \
                                                    DBG_L(0, __LINE__);   \
                                                    DBG_S(0, ".\r\n");   }}
#else
#define MMPF_USBH_CheckRBufSpaceSts(psRHdl) {}
#define MMPF_USBH_CheckRBufOvSts(psRHdl)    {}
#define MMPF_USBH_CheckRBufUnSts(psRHdl)    {}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPF_USBH_GetRBufDatCnt
//  Description : Get how many data available in the ring buffer.
//------------------------------------------------------------------------------
#define MMPF_USBH_GetRBufDatCnt(psRHdl, pulCnt)        AUTL_RingBuf_DataAvailable((AUTL_RINGBUF *)psRHdl, (MMP_ULONG *)pulCnt)

//------------------------------------------------------------------------------
//  Function    : CurRBufRdPtr
//  Description : Get current read pointer of ring buffer
//------------------------------------------------------------------------------
#define CurRBufRdPtr(psRHdl)                              (psRHdl.ptr.rd)

//------------------------------------------------------------------------------
//  Function    : CurRBufWrPtr
//  Description : Get current write pointer of ring buffer
//------------------------------------------------------------------------------
#define CurRBufWrPtr(psRHdl)                             (psRHdl.ptr.wr)

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

/* EP0 Control Transfer State Machine */
typedef enum _USB_EP0_STATE {
    EP0_IDLE,
    EP0_IN_DATA_PHASE,
    EP0_OUT_DATA_PHASE,
    EP0_IN_STATUS_PHASE,
    EP0_OUT_STATUS_PHASE,
    EP0_STALL
} USB_EP0_STATE;

/* UVC Host Stream Receive State Machine */
typedef enum _UVC_RX_STATE {
    RX_INACTIVE = 0x00,
    RX_UNKNOWN_PKT,
    RX_VALID_PKT,
    RX_1ST_DATA_PKT,
    RX_RES_DATA_PKT
} UVC_RX_STATE;

//==============================================================================
//
//                              STRUCTURE
//
//==============================================================================
/* Command Block Wrapper (CBW), exactly 31 bytes */
typedef struct _CMD_BLOCK_WRAPPER {
    MMP_ULONG   dCBWSignature;
        #define CBW_SIGANTURE       0x43425355
    MMP_ULONG   dCBWTag;
    MMP_ULONG   dCBWDataTxLength;
    MMP_UBYTE   bmCBWFlags;
        #define DATA_DIR_IN         0x80
        #define DATA_DIR_OUT        0x00
    MMP_UBYTE   bCBWLUN;
    MMP_UBYTE   bCBWCBLength;
#if 1
    MMP_UBYTE   bCBWCB[16];
#else
    // Inquiry
    MMP_UBYTE   bOPCode;
    MMP_UBYTE   bReserved;
    MMP_UBYTE   bPageCode;
    MMP_USHORT  bAllocLen;
    MMP_UBYTE   bControl;
    MMP_UBYTE   bPad[10];
#endif
} __packed CMD_BLOCK_WRAPPER;

/* Command Status Wrapper (CSW), exactly 13 bytes */
typedef struct _CMD_STATUS_WRAPPER {
    MMP_ULONG   dCSWSignature;
        #define CSW_SIGANTURE       0x53425355
    MMP_ULONG   dCSWTag;
    MMP_ULONG   dCSWDataResidue;
    MMP_UBYTE   bCSWStatus;
        #define CMD_PASSED          0x00
        #define CMD_FAILED          0x01
        #define PHASE_ERR           0x02
} CMD_STATUS_WRAPPER;

/* USB OTG Handler for Example */
typedef __packed struct _USB_OTG_HANDLE {
    USB_EP0_STATE   ep0_state;
    MMP_UBYTE       ep0_setup_buf[EP0_SETUP_PACKET_SIZE];
    MMP_UBYTE       ep0_data_buf[EP0_DATA_PACKET_SIZE + EP0_MAX_PACKET_SIZE];
#if (1)
    MMP_ULONG       ep_data_tmp_addr;
    MMP_ULONG       ep_data_addr[EP_RX_BUF_CNT];
    MMP_ULONG       recd_data_addr[RECD_FRM_BUF_CNT];
    MMP_ULONG       prevw_data_addr[PREVW_FRM_BUF_CNT];
    MMP_ULONG       prevw_rote_data_addr[PREVW_FRM_ROTE_BUF_CNT];
#else
    //MMP_ULONG       ep0_data_size;
    MMPF_OS_SEMID   rx_sem;
    MMPF_OS_SEMID   ep1_sem;
    MMPF_OS_SEMID   ep2_sem;
    MMPF_OS_SEMID   dma_sem;
    MMP_ULONG       ep_rx_buf;
    MMP_ULONG       ep_rx_buf_sz;
    MMP_ULONG       ep_tx_buf;
    MMP_ULONG       ep_tx_buf_sz;
    MMP_ULONG       tx_list_buf;
    MMP_ULONG       tx_list_buf_sz;
    MMP_UBYTE       dev_addr;
#endif
} USB_OTG_HANDLE;

typedef __packed struct _USB_EP0_DATA {
    MMP_UBYTE   bData[8];
} EP0_DATA;

/* EP Configuration Attribute */
typedef __packed struct _USB_EP_ATR {
    MMP_UBYTE   ep_dir;
    MMP_UBYTE   ep_speed;
    MMP_UBYTE   ep_xfer_type;
    MMP_UBYTE   ep_num;
    MMP_UBYTE   ep_addr;
    MMP_UBYTE   ep_interval;
    MMP_ULONG   ep_maxP;
} USB_EP_ATR;

/* Slot buffer manager */
typedef __packed struct _SLOT_BUF_CTL {
    MMP_ULONG   ulTmpBufAddr;
    MMP_ULONG   ulBufAddr[EP_RX_BUF_CNT];
    MMP_ULONG   ulBufSize;
    MMP_ULONG   ulWritePtr;
    MMP_ULONG   ulContentSize[EP_RX_BUF_CNT];
    MMP_UBYTE   ulWriteBufIdx;
} SLOT_BUF_CTL;

/* Packet Transfer Control */
typedef __packed struct _UVCH_XFER_CTL {
    MMP_ULONG   ulFrmSize;
    MMP_ULONG   ulXferSize;
    MMP_ULONG   ulUsbRxByte;
    MMP_ULONG   ulUsbPktCnt;
    MMP_ULONG   ulUsbLastPktByte;
    MMP_ULONG   ulFrmCnt;
    MMP_ULONG   ulFTarget;
    MMP_UBYTE   ubFrmToggle;
    MMP_UBYTE   ubUsbRxSkipEn;
    SLOT_BUF_CTL buf_ctl;
    MMP_UBYTE   ubUVCRxState;
    MMP_UBYTE   ubUVCStreamEn;
    MMP_UBYTE   ubUVCPrevwEn;
    MMP_UBYTE   ubUVCRecdEn;
#if (EN_USBH_ALWAYS_INPUT_FRM)
    MMP_UBYTE   ubRecdStopReqEx;
    MMP_UBYTE   ubRecdStopDone;
#endif
    MMP_UBYTE   ubRecdStopReleaseSemDone;
} UVCH_XFER_CTL;

/* Ring buffer manager */
typedef __packed struct _RECD_BUF_CTL {
    MMP_ULONG   ulRecdBufAddr[RECD_FRM_BUF_CNT];
    MMP_ULONG   ulRecdBufSize;
    MMP_ULONG   ulInputPtr;
    MMP_ULONG   ulStOfst[RECD_FRM_Q_SZ];          //frame start offset
    MMP_ULONG   ulTargetFrmSize[RECD_FRM_Q_SZ];   //xfer/received target frame size (w/ALIGN32 from device)
    MMP_ULONG   ulFrmSize[RECD_FRM_Q_SZ];         //exact frame size (w/o ALIGN32)
    MMP_ULONG   ulFrmTimeStamp[RECD_FRM_Q_SZ];    //frame timestamp
    MMP_UBYTE   ubFrmType[RECD_FRM_Q_SZ];         //frame type
    MMP_UBYTE   ubInFrm[RECD_FRM_CPLT_Q_SZ];
    MMP_UBYTE   ubInFrmCnt;                       //received frame # that wait to be wr
//    MMP_UBYTE   ubOutBufIdx;
//    MMP_UBYTE   ubWaitingCnt;
} RECD_BUF_CTL;

/* Ring buffer manager */
typedef __packed struct _PREVW_BUF_CTL {
    MMP_ULONG                   ulPrevwBufAddr[PREVW_FRM_BUF_CNT];
    MMP_ULONG                   ulPrevwRoteBufAddr[PREVW_FRM_ROTE_BUF_CNT];
    MMP_ULONG                   ulPrevwBufSize;
    MMP_ULONG                   ulInputPtr;
    MMP_UBYTE                   ubWinID;
    MMP_ULONG                   ulFrmSize;
    #if (DEVICE_YUV_TYPE==ST_NV12)
    MMP_ULONG                   ulUAddrOfst;
    MMP_ULONG                   ulVAddrOfst;
    MMP_ULONG                   ulUAddrOfstWALIGN16;
    MMP_ULONG                   ulVAddrOfstWALIGN16;
    #endif
    MMP_USHORT                  usPrevwRoteDstBufLineOfst;
    MMP_USHORT                  usPrevwRoteSrcRectTop;
    MMP_USHORT                  usPrevwRoteSrcRectHeight;
    MMP_UBYTE                   ubPrevwRoteBufFreeCnt;
    MMP_UBYTE                   ubPrevwRoteBufIdx;
    MMP_GRAPHICS_ROTATE_TYPE    ubPrevwRoteType;
    MMP_UBYTE                   ubUpdFrm[PREVW_FRM_CPLT_Q_SZ];
    MMP_UBYTE                   ubSkipCopyPrevwBufEn;
} PREVW_BUF_CTL;

typedef __packed struct _UVCH_Rx_Param {
    MMP_ULONG   ulDmaByte;
    MMP_ULONG   ulFrmSize;
    MMP_ULONG   ulRealOfst;
    MMP_ULONG   ulTimeStamp;
    MMP_USHORT  usFrmType;
}UVCH_Rx_Param;

typedef __packed struct _MMPF_USBH_POLLING_CFG 
{
    MMPF_OS_TMR_CALLBACK        TriggerReadUSBIDCallback;
    MMPF_USBH_Callback          *OtgSessionTimeoutCallback;
    MMPF_USBH_Callback          *DevConnCallback;
    MMPF_USBH_Callback          *DevDisconnCallback;
} MMPF_USBH_POLLING_CFG ;

typedef __packed struct _MMPF_USBH_MD_CFG 
{
    MMP_UBYTE                   ubDevMDEn;      //handle MD frame or not
    MMPF_USBH_Callback          *DevTriggerMDCallback;
} MMPF_USBH_MD_CFG ;

/* USB UVC error handle manager (software) */
typedef __packed struct _MMPF_USBH_ERROR_HANDLE_CFG 
{
    MMP_UBYTE                   ubFirstFrmReceived;                                 //TRUE,FALSE
    MMP_UBYTE                   ubFrmRxTimerStart;                                  //TRUE,FALSE
    MMP_UBYTE                   ubTranTimerStart;                                   //TRUE,FALSE
    MMP_UBYTE                   ubCheckDevAliveTimerStart;                          //TRUE,FALSE
    MMP_UBYTE                   ubFrmRxTimeoutTHValid;                              //TRUE,FALSE
    MMP_UBYTE                   ubUpdFrmRxTimeoutParm;                              //TRUE,FALSE
    MMP_UBYTE                   ubFrmRxTimeoutCallbackDone[FRM_RX_TIMEOUT_MAX_HDL]; //TRUE,FALSE
    MMP_UBYTE                   ubTranTimeoutCallbackDone;                          //TRUE,FALSE
    MMP_UBYTE                   ubDevNoResponseTriggered;                             //TRUE,FALSE
    MMP_UBYTE                   ubFrmRxTimeoutTriggered[FRM_RX_TIMEOUT_MAX_HDL];      //TRUE,FALSE
    MMP_ULONG                   ulFrmRxLostTH[FRM_RX_TIMEOUT_MAX_HDL];              //#
    MMP_ULONG                   ulFrmSeqNoLostTH;                                   //#
    MMP_ULONG                   ulFrmRxFirstTimeoutInterval[FRM_RX_TIMEOUT_MAX_HDL];//ms
    MMP_ULONG                   ulFrmRxNextTimeoutInterval[FRM_RX_TIMEOUT_MAX_HDL]; //ms
    MMP_ULONG                   ulTranTimeoutInterval;                              //ms, exclude 1st transaction of frame
    MMP_ULONG                   ulCheckDevAliveInterval;                            //ms
    MMP_ULONG                   ulFrmRxCheckTimerId;
    #if (!EN_REMOVE_TRAN_HW_TIMER)
    MMP_ULONG                   ulTranTimerId;
    #endif
    MMP_ULONG                   ulCheckDevAliveTimerId;
    MMP_ULONG                   ulCurWaitRxDoneMsCnt;                               //ms
    #if (EN_REMOVE_TRAN_HW_TIMER)
    MMP_ULONG                   ulCurTranTimerStartMs;                              //ms
    #endif
    MMP_ULONG                   ulCurTranCnt;
    MMP_ULONG                   ulCurYUVFrmSeqNo;
    MMP_ULONG                   ulCurH264FrmSeqNo;
    MMP_ULONG                   ulLastYUVFrmSeqNo;
    MMP_ULONG                   ulLastH264FrmSeqNo;
    MMP_ULONG                   ulDropRxFrmCurCnt; 
    MMP_ULONG                   ulDropRxFrmCurContinCnt;  
    MMP_ULONG                   ulDropRxFrmTotalCnt; //How many frames are dropped in total
    MMP_ULONG                   ulDropRxFrmContinCnt;  //How many frames are dropped sequentially
    
    MMPF_USBH_Callback          *DevTranTimeoutCallback;
    MMPF_USBH_Callback          *DevFrmRxTimeoutCallback[FRM_RX_TIMEOUT_MAX_HDL];
    MMPF_USBH_Callback          *DevFrmSeqNoLostCallback;
    MMPF_USBH_Callback          *DevNoResponseCallback;
    MMPF_USBH_Callback          *DevDropRxFrmCallback;    
} MMPF_USBH_ERROR_HANDLE_CFG ;

/* Error message info structure */
typedef __packed struct _MMPF_USBH_ERR_MSG_CFG 
{
    MMP_UBYTE ubErrCase;
    MMP_ULONG ulParm0;
    MMP_ULONG ulParm1;
    //MMP_ULONG ulParm2;
    //MMP_ULONG ulParm3;
    //MMP_UBYTE ubErrStr[USBH_ERR_STR_LEN];
} MMPF_USBH_ERR_MSG_CFG ;

/* Still rear cam structure */
typedef __packed struct _MMPF_USBH_STILL_CFG 
{
    MMP_ULONG   ulJpgDstBufAddr;    //destination address to store stilled JPG
    MMP_ULONG   ulJpgDstBufSize;    //destination reserved buffer size for store JPG
    MMP_BOOL    bJpgStart;          //enable still JPG
    MMP_UBYTE   ubJpgStartDelayCnt; //delay frame count before still
    MMP_ULONG   ulCurJpgSize;       //stilled JPG size
    MMP_BOOL    bJpgDone;           //still complete or not
    MMP_ERR     mStillSR;           //still operation status
} MMPF_USBH_STILL_CFG ;

//==============================================================================
//
//                              VARIABLES
//
//==============================================================================

#if (HANDLE_JPEG_EVENT_BY_QUEUE)
extern MMP_UBYTE                gbEncodeMjpegStream;
#endif
#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
extern MMP_DSC_DECODE_BUF       gsMjpegToPreviewDecodeBuf[];
extern MMP_UBYTE                gbDecJpgErrSts;
#endif
extern MMPF_OS_SEMID            m_CtlXferSemID;

extern AUTL_RINGBUF             mRecdFrmXferFsQHdl; //record frame xfer frame size queue
extern AUTL_RINGBUF             mRecdFrmFsQHdl;     //record frame valid frame size queue
extern AUTL_RINGBUF             mRecdFrmTstpQHdl;   //record frame time stamp queue
extern AUTL_RINGBUF             mRecdFrmTypQHdl;    //record frame type queue
extern AUTL_RINGBUF             mRecdFrmStOfstQHdl; //record frame start offset queue
extern AUTL_RINGBUF             mRecdFrmCpltQHdl;   //record frame if complete(one frame) by this moveDMA operation, before move into Mgr
extern AUTL_RINGBUF             mRecdFrmBufHdl;     //record frame data ring buffer
extern AUTL_RINGBUF             mPrevwFrmCpltQHdl;  //preview frame if complete(one frame)by this moveDMA operation, before refresh
extern AUTL_RINGBUF             mPrevwFrmBufHdl;    //preview frame data ring buffer
#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
extern AUTL_RINGBUF             mDecJpgRdyQHdl;     //JPG, input ready queue, before trigger decode
#endif

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

MMP_ERR MMPF_USBH_WaitEp0TxDone(void);
MMP_ERR MMPF_USBH_WaitEp0RxDone(void);
MMP_ERR MMPF_USBH_WaitEpTxDone(MMP_UBYTE ep);
MMP_ERR MMPF_USBH_WaitEpRxDone(MMP_UBYTE ep);
MMP_ERR MMPF_USBH_WriteEpFifo(MMP_UBYTE ep, MMP_UBYTE *data, MMP_ULONG size);
MMP_ERR MMPF_USBH_ReadEpFifo(   MMP_UBYTE   ep,
                                MMP_UBYTE   *buf,
                                MMP_USHORT  *psByteinFifo);

MMP_ERR MMPF_USBH_ConfigEp0(USBH_DEVICE *dev);
MMP_ERR MMPF_USBH_ConfigEp(MMP_UBYTE ep, USB_EP_ATR *attribute);
void    MMPF_USBH_EnableEpInterrupt(MMP_UBYTE           ep,
                                    MMP_UBYTE           dir,
                                    MMP_USB_EpCallback  *cb);
void    MMPF_USBH_DisableEpInterrupt(MMP_UBYTE ep, MMP_UBYTE dir);

MMP_ERR MMPF_USBH_InitCtlXferSem(void);
MMP_ERR MMPF_USBH_SendEp0Setup(EP0_REQUEST *setup_pkt);
MMP_ERR MMPF_USBH_ReceiveEp0Data(MMP_UBYTE *data_buf, MMP_ULONG *plDataBytes);
MMP_ERR MMPF_USBH_SendEp0Data(MMP_UBYTE *data_buf, MMP_ULONG size);
MMP_ERR MMPF_USBH_HandleEp0Status(MMP_UBYTE dir);
MMP_ERR MMPF_USBH_ControlTransfer(  USBH_DEVICE *dev,
                                    EP0_REQUEST *req,
                                    void        *buf,
                                    MMP_ULONG   *xfer_size);
void    MMPF_USBH_InterruptInTransfer(  USBH_DEVICE         *dev,
                                        MMP_UBYTE           ep,
                                        MMP_USB_EpCallback  *cb);


MMP_ERR MMPF_USBH_TriggerDmaRx64Byte(void);
MMP_ERR MMPF_USBH_TriggerDmaRx448Byte(void);
MMP_ERR MMPF_USBH_TriggerDmaRxOnePkt(void);
MMP_ERR MMPF_USBH_WaitDmaDone(MMP_UBYTE ch);
MMP_ERR MMPF_USBH_WaitDmaRxDone(MMP_UBYTE ep, MMP_UBYTE ch);
MMP_ERR MMPF_USBH_WaitDmaTxDone(MMP_UBYTE ep, MMP_UBYTE ch);
MMP_ERR MMPF_USBH_TriggerDmaRx(void);
MMP_ERR MMPF_USBH_ConfigDma3Tx( MMP_UBYTE   endpoint,
                                MMP_ULONG   fb_addr,
                                MMP_USHORT  pkt_byte,
                                MMP_USHORT  last_pkt_byte,
                                MMP_ULONG   pkt_sum,
                                MMP_ULONG   zero_end);
MMP_ERR MMPF_USBH_StopFrmRxClrFIFO(void);
#if (DMA3_RX_LAST_PKT_PATCH)
MMP_ERR MMPF_USBH_ConfigDma3RxLP(MMP_UBYTE endpoint,MMP_ULONG fb_addr,MMP_USHORT pkt_byte,MMP_USHORT last_pkt_byte,MMP_ULONG pkt_sum,MMP_BOOL bPktReq);
#else
MMP_ERR MMPF_USBH_ConfigDma3Rx(MMP_UBYTE endpoint,MMP_ULONG fb_addr,MMP_USHORT pkt_byte,MMP_USHORT last_pkt_byte,MMP_ULONG pkt_sum);
#endif
MMP_ERR MMPF_USBH_SetBulkDma3Tx(MMP_UBYTE ep, MMP_ULONG fb_addr,MMP_ULONG size);
MMP_ERR MMPF_USBH_SetBulkDma3Rx(MMP_UBYTE ep, MMP_ULONG fb_addr,MMP_ULONG rx_byte,MMP_BOOL bPktReq);
MMP_ERR MMPF_USBH_initUVCPrevwRingBuf(void);
MMP_ERR MMPF_USBH_initUVCRecdRingBuf(void);
MMP_ERR MMPF_USBH_initFBMemory(void);
MMP_ERR MMPF_USBH_IsSetFBMemory(MMP_UBYTE *ubUVCFBMemSet);
MMP_ERR MMPF_USBH_ResetFBMemory(void);
MMP_ERR MMPF_USBH_SetFBMemory(MMP_ULONG plStartAddr, MMP_ULONG *plAllocSize);
MMP_ERR MMPF_USBH_SetUVCPrevwRote(MMP_GRAPHICS_ROTATE_TYPE ubRoteType);
MMP_ERR MMPF_USBH_GetTimerCount(MMP_ULONG *pulTimeUs);
MMP_ERR MMPF_USBH_StopOTGSession(void);
MMP_ERR MMPF_USBH_TriggerOTGSession(MMP_ULONG *pulTimerId);
MMP_ERR MMPF_USBH_ResetPort(void);
MMP_UBYTE MMPF_USBH_GetBusSpeed(void);
MMP_ERR MMPF_USBH_InitOTG(void);
MMP_ERR MMPF_USBH_SetTxBuf(MMP_ULONG fb_addr,MMP_USHORT size);
MMP_ERR MMPF_USBH_TestBulkOut(MMP_ULONG sec_num, MMP_UBYTE dma_ch);
MMP_ERR MMPF_USBH_GetClassIfCmd(MMP_UBYTE bReq, MMP_USHORT wVal, MMP_USHORT wInd, MMP_USHORT wLen, MMP_ULONG *UVCDataLength, MMP_UBYTE *UVCDataBuf);
MMP_ERR MMPF_USBH_SetClassIfCmd(MMP_UBYTE bReq, MMP_USHORT wVal, MMP_USHORT wInd, MMP_USHORT wLen, MMP_UBYTE *UVCDataBuf);
MMP_ERR MMPF_USBH_SetStdIfCmd(MMP_UBYTE bReq, MMP_USHORT wVal, MMP_USHORT wInd, MMP_USHORT wLen, MMP_UBYTE *UVCDataBuf);
MMP_ERR MMPF_USBH_SetStdEpCmd(MMP_UBYTE bReq, MMP_USHORT wVal, MMP_USHORT wInd, MMP_USHORT wLen, MMP_UBYTE *UVCDataBuf);
MMP_ERR MMPF_USBH_GetStdDevCmd(MMP_UBYTE bReq, MMP_USHORT wVal, MMP_USHORT wInd, MMP_USHORT wLen, MMP_ULONG *UVCDataLength, MMP_UBYTE *UVCDataBuf);
MMP_ERR MMPF_USBH_GetDeviceConnectedSts(MMP_BOOL *pbEnable);
MMP_ERR MMPF_USBH_SetDeviceConnectedSts(MMP_BOOL pbEnable);
MMP_ERR MMPF_USBH_OpenUVCDevice(USBH_DEVICE *dev);
MMP_ERR MMPF_USBH_GetStreamReq(MMP_USHORT wMin);
MMP_UBYTE USB_UVCH_GetRemainFrmCnt(void);
MMPF_USBH_POLLING_CFG* MMPF_USBH_GetPollingCFG(void);
MMP_ERR MMPF_USBH_RegPollingCallBack(void *pReadUSBIDTimeoutCallback, void *pOtgSessionTimeoutCallback, void *pDevConnCallback, void *pDevDisconnCallback);
MMP_ERR MMPF_USBH_TriggerReadUSBID(MMP_ULONG *pulTimerId);
MMP_ERR MMPF_USBH_ReadUSBID(MMP_UBYTE *pubDevType);
MMPF_USBH_MD_CFG* MMPF_USBH_GetUVCMDCFG(void);
MMP_ERR MMPF_USBH_RegUVCMDCallBack(void *pDevTriggerMDCallback);
MMP_ERR MMPF_USBH_EnableUVCMD(void);
MMP_ERR MMPF_USBH_DisableUVCMD(void);
MMPF_USBH_ERROR_HANDLE_CFG* MMPF_USBH_GetErrorHandleCFG(void);
MMP_ERR MMPF_USBH_StopCheckDevAliveTimer(void);
MMP_ERR MMPF_USBH_StartCheckDevAliveTimer(void);
MMP_ERR MMPF_USBH_SetCheckDevAliveCFG(MMP_ULONG pulCheckDevAliveInterval, void *pDevNoResponseCallback);
MMP_ERR MMPF_USBH_StopTranTimer(void);
MMP_ERR MMPF_USBH_RestartTranTimer(void);
MMP_ERR MMPF_USBH_SetTranTimeoutCFG(MMP_ULONG pulTranTimeoutInterval, void *pDevTranTimeoutCallback);
MMP_ERR MMPF_USBH_StopFrmRxTimer(void);
MMP_ERR MMPF_USBH_RestartFrmRxTimer(void);
MMP_ERR MMPF_USBH_SetFrmRxTimeoutCFG(MMP_ULONG pulFrmRxLostTH, MMP_ULONG pulFrmRxFirstAddGap, void *pDevFrmRxLostTHCallback);
MMP_ERR MMPF_USBH_SetFrmRxTimeout2CFG(MMP_ULONG pulFrmRxLostTH2, void *pDevFrmRxLostTH2Callback);
MMP_ERR MMPF_USBH_SetFrmSeqNoLostCFG(MMP_ULONG pulSeqNoLostTH, void *pDevSeqNoLostTHCallback);
MMP_ERR MMPF_USB_SetDropRxFrmCFG(MMP_ULONG ulTotalCnt, MMP_ULONG ulContinCnt, void *pDropRxFrmCallback);

#if (HANDLE_JPEG_EVENT_BY_QUEUE)
MMP_ERR MMPF_USBH_SetDualStreamingEnable(MMP_BOOL bFrontCamEn, MMP_BOOL bRearCamEn);
#endif

#if (SUPPORT_DEC_MJPEG_TO_PREVIEW)
MMP_UBYTE MMPF_USBH_GetEncPipeForDualStreaming(void);
MMP_ULONG MMPF_USBH_GetMaxRsvdBufSizeForDualStreaming(MMP_BOOL bFrontCam);
MMP_ERR MMPF_USBH_GetRearCamBufForDualStreaming(MMP_ULONG *pulCompAddr, MMP_ULONG *pulCompSize, MMP_ULONG *pulLineBuf);

MMP_UBYTE MMPF_USBH_GetDecMjpegToPreviewPipe(void);
MMP_UBYTE MMPF_USBH_GetDecMjpegToEncodePipe(void);
MMP_ERR MMPF_USBH_SetDecMjpegToPreviewPipe(MMP_UBYTE ubPipe);
MMP_ERR MMPF_USBH_SetDecMjpegToEncodePipe(MMP_UBYTE ubPipe);
MMP_ERR MMPF_USBH_SetDecMjpegToPreviewSrcAttr(MMP_USHORT usSrcW, MMP_USHORT usSrcH);
MMP_ERR MMPF_USBH_InitDecMjpegToPreviewRingBuf(void);
MMP_ERR MMPF_USBH_InitDecMjpegToPreviewJpegBuf(void);
MMP_ERR MMPF_USBH_IsSetDecMjpegToPreviewJpegBuf(MMP_UBYTE *ubMjpegToPreviewJpegSet);
MMP_ERR MMPF_USBH_ResetDecMjpegToPreviewJpegBuf(void);
MMP_ERR MMPF_USBH_SetDecMjpegToPreviewJpegBuf(MMP_ULONG ulAddr, MMP_ULONG* pulSize); 
void MMPF_USBH_SetDecMjpegToPreviewIbcInterrput(MMP_BOOL bEnable);
void MMPF_USBH_SetDecMjpegToPreviewJpegInterrput(MMP_BOOL bEnable);
void MMPF_USBH_TriggerDecMjpegToPreview(void);
void MMPF_USBH_InitDecMjpegToPreview(void);
void MMPF_USBH_StopDecMjpegToPreview(void);
MMPF_USBH_STILL_CFG* MMPF_USBH_GetStillCFG(void);
MMP_ERR MMPF_USBH_StartJpegCpature (MMP_ULONG ulJpegAddr, MMP_ULONG ulBufSize, MMP_UBYTE ubDelayCnt);
MMP_ERR MMPF_USBH_QueryJpeg (MMP_ULONG *ulJpegSize);
#endif

#endif //end of #if (USB_EN)&&(SUPPORT_USB_HOST_FUNC)

#endif // _MMPF_USBH_CTL_H_

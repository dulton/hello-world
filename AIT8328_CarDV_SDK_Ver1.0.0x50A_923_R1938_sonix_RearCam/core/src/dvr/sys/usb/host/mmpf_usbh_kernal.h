//==============================================================================
//
//  File        : mmpf_usbh_kernal.h
//  Description : Header file for the USB HOST kernal function
//  Author      : Bruce_Chien
//  Revision    : 1.0
//
//==============================================================================
#ifndef _MMPF_USBH_KERNAL_H_
#define _MMPF_USBH_KERNAL_H_

#include "includes_fw.h"
#include "mmp_reg_usb.h"

//==============================================================================
//
//                              DATA TYPES
//
//==============================================================================


//==============================================================================
//
//                              MACRO FUNC
//
//==============================================================================
#define SEPARATE_HEADER_RX          (0)//separate 64B-448B-LEFT_B, USB_MCI least 32B 
#define EN_CALI_USE_SW_TMR          (1)
#define EN_DBGLOG_PFMC              (0)
#define EN_DBGLOG_RX_PFMC           (0)
#define EN_CHECK_AIT_TEST_PATTERN   (0)
#define EN_DUMP_H264_BS             (0)
#define EN_DUMP_YUV_FRM             (0) //change EP_RX_BUF_CNT to dump whole frame, e.g. (58)
#define EN_DUMP_MJPEG_FRM           (0)
#define EN_BENCH_LOG                (0)

//==============================================================================
//
//                              CONSTANT
//
//==============================================================================
#define USBH_H264_NALU_SIZE         (80)
#if (EN_DBGLOG_PFMC)
#define MAX_USBH_PFMC_PHASE         (51)
#define MAX_USBH_PFMC_ROUND         (10)
#endif
#if (EN_DBGLOG_RX_PFMC)
#define MAX_USBH_RX_PFMC_ROUND      (1890)
#endif
#if (EN_BENCH_LOG)
#define USBH_BCH_TIME_QUEUE_SIZE    (6)
#define USBH_BCH_DATA_QUEUE_SIZE    (6)
#endif

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================


//==============================================================================
//
//                              STRUCTURE
//
//==============================================================================
/* Mgr/USBH handshake config */
typedef __packed struct _MMPF_USBH_MGR_HS_CFG 
{
    /* Mgr notify USBH to close file by seamless record close file */
    MMP_UBYTE   ubVidMgrNotifyUVCForceI;        /* HS OP1: set flag */
    MMP_UBYTE   ubReqUVCForceIDone;             /* HS OP2: req force I done */
    MMP_UBYTE   ubGetUVCIDRFrm;                 /* HS OP3: get IDR frame */
    MMP_ULONG   ulFrmCntBeforeCloseCurFile;     /* # of frame RX after OP2 till OP3  */

    /* USBH notify Mgr to close file by UVC buffer overflow */
    MMP_UBYTE   ubNotifyVidMgr2StopRecordDone;  /* HS OP: notify Mgr done */

    /* Mgr notify USBH to stop UVC record by Mgr slow card */
    MMP_UBYTE   ubVidMgrNotifySlowCard;         /* HS OP: set flag */

    /* USBH maintain parameters of UVC record only */
    MMP_BOOL    bVidMgrUVCRecdSt;               /* UVC record start: MMP_TRUE/MMP_FALSE, trigger by frist I */
    MMP_BOOL    bGetRecdHdrDone;                /* get H264 SpsPpsHdr info from first I */
    MMP_UBYTE   ubH264Nalu[USBH_H264_NALU_SIZE];
    MMP_UBYTE   ubProfileIDC;
    MMP_UBYTE   ubLevelIDC;
} MMPF_USBH_MGR_HS_CFG ;

#if (EN_BENCH_LOG)
typedef struct _MMPF_USBH_BENCH_CFG 
{
    MMP_UBYTE ubEnShowLog;
    MMP_ULONG ulTimeIdx;
    MMP_ULONG ulDataIdx;
    MMP_ULONG ulSizeIdx;
    MMP_ULONG ulTime[USBH_BCH_TIME_QUEUE_SIZE];
    MMP_ULONG ulAddr[USBH_BCH_DATA_QUEUE_SIZE];
    MMP_ULONG ulSAddr[USBH_BCH_DATA_QUEUE_SIZE];
    MMP_ULONG ulDAddr[USBH_BCH_DATA_QUEUE_SIZE];
    MMP_ULONG ulData[USBH_BCH_DATA_QUEUE_SIZE];
    MMP_ULONG ulSize[USBH_BCH_DATA_QUEUE_SIZE];
    MMP_ULONG ulSize2[USBH_BCH_DATA_QUEUE_SIZE];
    MMP_ULONG ulCntIdx;
    MMP_UBYTE ubStr[32];
} MMPF_USBH_BENCH_CFG ;
#endif


//==============================================================================
//
//                              INLINE FUNCTION
//
//==============================================================================
__inline void SetEp0Csr(MMP_USHORT csr)
{
    AITPS_USB_CTL pUSB_CTL = AITC_BASE_USBCTL;
    pUSB_CTL->USB_EP[0x0].USB_EP_TX_CSR = csr;
}

__inline MMP_USHORT GetEp0Csr(void)
{
    AITPS_USB_CTL pUSB_CTL = AITC_BASE_USBCTL;
    return pUSB_CTL->USB_EP[0x0].USB_EP_TX_CSR;
}

__inline void SetEpTxCsr(MMP_UBYTE ep, MMP_USHORT csr)
{
    AITPS_USB_CTL pUSB_CTL = AITC_BASE_USBCTL;
    pUSB_CTL->USB_EP[ep].USB_EP_TX_CSR = csr;
}

__inline MMP_USHORT GetEpTxCsr(MMP_UBYTE ep)
{
    AITPS_USB_CTL pUSB_CTL = AITC_BASE_USBCTL;
    return pUSB_CTL->USB_EP[ep].USB_EP_TX_CSR;
}

__inline void SetEpRxCsr(MMP_UBYTE ep, MMP_USHORT csr)
{
    AITPS_USB_CTL pUSB_CTL = AITC_BASE_USBCTL;
    pUSB_CTL->USB_EP[ep].USB_EP_RX_CSR = csr;
}

__inline MMP_USHORT GetEpRxCsr(MMP_UBYTE ep)
{
    AITPS_USB_CTL pUSB_CTL = AITC_BASE_USBCTL;
    return pUSB_CTL->USB_EP[ep].USB_EP_RX_CSR;
}

__inline MMP_USHORT GetEpRxCount(MMP_UBYTE ep)
{
    AITPS_USB_CTL pUSB_CTL = AITC_BASE_USBCTL;

    if (ep > 7)
        return 0;
    return pUSB_CTL->USB_EP[ep].USB_EP_COUNT;
}

__inline void WaitEpTxInterrupt(MMP_UBYTE ep)
{
    AITPS_USB_CTL pUSB_CTL = AITC_BASE_USBCTL;
    MMP_UBYTE ep_int_bit = 1 << ep;

    while(!(pUSB_CTL->USB_TX_INT_SR & ep_int_bit));
}

__inline void WaitEpRxInterrupt(MMP_UBYTE ep)
{
    AITPS_USB_CTL pUSB_CTL = AITC_BASE_USBCTL;
    MMP_UBYTE ep_int_bit = 1 << ep;

    if (ep)
        while(!(pUSB_CTL->USB_RX_INT_SR & ep_int_bit));
    else
        while(!(pUSB_CTL->USB_TX_INT_SR & ep_int_bit));
}


//==============================================================================
//
//                              VARIABLES
//
//==============================================================================
#if (EN_DBGLOG_PFMC)
extern MMP_ULONG    glUSBHPtx[MAX_USBH_PFMC_ROUND][MAX_USBH_PFMC_PHASE];
extern MMP_USHORT   gsUSBHPPIdx;
extern MMP_USHORT   gsUSBHPRIdx;
extern MMP_UBYTE    gbEnPFMC;
#endif
#if (EN_DBGLOG_RX_PFMC)
extern MMP_ULONG    gltmpStTime;
extern MMP_ULONG    gltmpEndTime;
extern MMP_ULONG    gltmpSeqNo;
extern MMP_ULONG    glDmaRxSt[MAX_USBH_RX_PFMC_ROUND];
extern MMP_ULONG    glDmaRxEnd[MAX_USBH_RX_PFMC_ROUND];
extern MMP_BYTE     gbDmaSType[MAX_USBH_RX_PFMC_ROUND];
extern MMP_ULONG    glDmaSeqNo[MAX_USBH_RX_PFMC_ROUND];
extern MMP_USHORT   gsDmaRxStIdx;
extern MMP_USHORT   gsDmaRxEndIdx;
#endif

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
#if (SUPPORT_SYNC_UVC_TIME)
MMP_ERR MMPF_USBH_StopCalibrationTimer(void);
MMP_ERR MMPF_USBH_StopSyncUVCTimer(void);
MMP_ERR MMPF_USBH_StartSyncUVCTimer(void);
#endif
MMP_ERR MMPF_USBH_RefreshLcd(void);
MMP_ERR MMPF_USBH_UpdRoteFrmParm(void);
MMP_ERR MMPF_USBH_RstMgrHsState(void);
MMP_ERR MMPF_USBH_RstUVCRecdState(void);
MMP_ERR MMPF_USBH_SendErrMsg(MMP_UBYTE ubErrCase, MMP_ULONG ulParm0, MMP_ULONG ulParm1, MMP_ULONG ulParm2, MMP_ULONG ulParm3);
#if (EN_BENCH_LOG)
MMPF_USBH_BENCH_CFG* MMPF_USBH_GetBchCFG(void);
MMP_ERR MMPF_USBH_SwitchShowLog(void);
#endif
void MMPF_USBH_ProcPacketHandler(MMP_UBYTE ubRxState);


#endif // _MMPF_USBH_KERNAL_H_

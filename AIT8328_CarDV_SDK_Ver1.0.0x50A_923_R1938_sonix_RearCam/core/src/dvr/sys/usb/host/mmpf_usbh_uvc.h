//==============================================================================
//
//  File        : mmpf_usbh_uvc.h
//  Description : Header file for the USB HOST UVC function
//  Author      : Bruce_Chien
//  Revision    : 1.0
//
//==============================================================================
#ifndef _MMPF_USBH_UVC_H_
#define _MMPF_USBH_UVC_H_

#include "includes_fw.h"

#if (USB_EN)&&(SUPPORT_USB_HOST_FUNC)
#include "mmpf_usbh_core.h"

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================
#define MMPF_USBH_UVC_CHIP_STR_LEN  (8)
#define MMPF_USBH_DEV_CFG_MAX_NUM   (5)

/* Default UVC Parameters */
#define DEFAULT_PREVW_W             (320)
#define DEFAULT_PREVW_H             (180)
#define DEFAULT_PREVW_TIMERESO      (200)
#define DEFAULT_PREVW_TIMEINCR      (6000)
#define DEFAULT_RECD_RES            (VIDRECD_RESOL_1920x1088)//(MMPF_USBH_UVC_RESOL_1080P)
#define DEFAULT_RECD_W              (1920)
#define DEFAULT_RECD_H              (1080)

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================
typedef enum _MMPF_USBH_UPD_UVC_CFG_OP {
    MMPF_USBH_UPD_OP_NONE           = 0,
    MMPF_USBH_UPD_OPEN_UVC_CB       = 1,
    MMPF_USBH_UPD_START_UVC_CB      = 2,
    MMPF_USBH_UPD_NALU_TBL          = 3,
    MMPF_USBH_UPD_OP_MAX            = 4
} MMPF_USBH_UPD_UVC_CFG_OP;

typedef enum _MMPF_USBH_REG_UVC_ADDI_CFG_TYP {
    MMPF_USBH_REG_TYP_NONE          = 0,
    MMPF_USBH_REG_TYP_GUID_EU       = 1,
    MMPF_USBH_REG_TYP_MAX           = 2
} MMPF_USBH_REG_UVC_ADDI_CFG_TYP;

//==============================================================================
//
//                              STRUCTURES AND TYPEDEF
//
//==============================================================================
typedef __packed struct _MMPF_USBH_UVC_H264_NALU_CFG 
{
    MMP_UBYTE   ubSpsOfst;          //from *UVC_HDR to SPS_TYPE_HDR(0x67)
    MMP_UBYTE   ubPpsOfst;          //from *UVC_HDR to PPS_TYPE_HDR(0x68)
    MMP_UBYTE   ubSpsSize;          //size of SPS_TYPE w/o startcode
    MMP_UBYTE   ubPpsSize;          //size of PPS_TYPE w/o startcode
    MMP_UBYTE   ubIFAddNaluSize;    //size of additional nalu w/o startcode in IDR frame
    MMP_UBYTE   ubPFAddNaluSize;    //size of additional nalu w/o startcode in NIDR frame
    MMP_UBYTE   ubIFOfst;           //from *PAYLOAD_DATA(1st startcode) to IDR_TYPE_HDR(0x65)
    MMP_UBYTE   ubPFOfst;           //from *PAYLOAD_DATA(1st startcode) to NIDR_TYPE_HDR(0x41)
} MMPF_USBH_UVC_H264_NALU_CFG ;

typedef __packed struct _MMPF_USBH_UVC_RECD_CFG 
{
    MMP_UBYTE                   usResol;
    MMP_USHORT                  usWidth;
    MMP_USHORT                  usHeight;
    MMP_ULONG                   ulBitrate;
    MMP_USHORT                  usTimeIncrement;
    MMP_USHORT                  usTimeIncrResol;
    MMP_USHORT                  usPFrame;
} MMPF_USBH_UVC_RECD_CFG ;

typedef __packed struct _MMPF_USBH_UVC_PREVW_CFG 
{
    MMP_USHORT                  usWidth;
    MMP_USHORT                  usHeight;
    MMP_UBYTE                   ubFps;
    MMP_BOOL                    ubSet;
} MMPF_USBH_UVC_PREVW_CFG ;

typedef __packed struct _MMPF_USBH_UVC_STREAM_CFG 
{
    MMPF_USBH_UVC_RECD_CFG      mRecd;
    MMPF_USBH_UVC_PREVW_CFG     mPrevw;
} MMPF_USBH_UVC_STREAM_CFG ;

typedef void        (MMPF_USBH_Callback) (void);

typedef __packed struct _MMPF_USBH_DEV_CFG 
{
    MMP_UBYTE                   ubDevStrInfo[MMPF_USBH_UVC_CHIP_STR_LEN];
    MMPF_USBH_Callback          *OpenUVCCallback;
    MMPF_USBH_Callback          *StartUVCCallback;
    MMPF_USBH_UVC_H264_NALU_CFG mNalu;
    MMP_ULONG                   ulGuidEu[CS_INTERFACE_DESCR_GUID_SZ/(sizeof(MMP_ULONG))];
} MMPF_USBH_DEV_CFG ;

typedef __packed struct _MMPF_USBH_DEV_GRP_CFG 
{
    MMP_UBYTE                   ubFreeCount;
    MMP_UBYTE                   ubDevIdx; //current index that match chip info
    MMPF_USBH_DEV_CFG           mDev[(MMPF_USBH_DEV_CFG_MAX_NUM+1)]; //reserve last one NULL if no match dev
} MMPF_USBH_DEV_GRP_CFG ;

//==============================================================================
//
//                              EXTERN VARIABLES
//
//==============================================================================

//==============================================================================
//
//                              VARIABLES
//
//==============================================================================

//==============================================================================
//
//                              EXTERN FUNCTIONS
//
//==============================================================================

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
MMP_ERR MMPF_USBH_StartOTGSession(MMP_ULONG *pulTimerId);
MMP_ERR MMPF_USBH_OpenUVC(USBH_DEVICE *dev);
MMP_ERR MMPF_USBH_StartUVCPrevw(void);
MMP_ERR MMPF_USBH_StopUVCPrevw(void);
MMP_ERR MMPF_USBH_SetUVCStreamOn(void);
MMP_ERR MMPF_USBH_SetUVCStreamOff(void);
MMP_ERR MMPF_USBH_GetUVCStreamSts(MMP_BOOL *pbEnable);
MMP_ERR MMPF_USBH_SetUVCPrevwOn(void);
MMP_ERR MMPF_USBH_SetUVCPrevwOff(void);
MMP_ERR MMPF_USBH_GetUVCPrevwSts(MMP_BOOL *pbEnable);
MMP_ERR MMPF_USBH_SetStartFrameofUVCRecd(void);
MMP_ERR MMPF_USBH_SetEndFrameofUVCRecd(void);
MMP_ERR MMPF_USBH_GetUVCRecdSts(MMP_BOOL *pbEnable);
MMP_ERR MMPF_USBH_SetUVCPrevwWinID(MMP_UBYTE pbWinID);
MMP_ERR MMPF_USBH_SetUVCIDR(void);
MMPF_USBH_UVC_STREAM_CFG* MMPF_USBH_GetUVCCFG(void);
MMP_ERR MMPF_USBH_FindDevInfo(MMP_UBYTE *pubStr);
MMP_ERR MMPF_USBH_SetUVCRecdResol(MMP_UBYTE ubResol, MMP_USHORT usWidth, MMP_USHORT usHeight);
MMP_ERR MMPF_USBH_SetUVCRecdBitrate(MMP_ULONG ulBitrate);
MMP_ERR MMPF_USBH_SetUVCRecdFrameRate(MMP_USHORT usTimeIncrement, MMP_USHORT usTimeIncrResol);
MMP_ERR MMPF_USBH_SetUVCRecdFrameCount(MMP_USHORT usPFrameCount);
MMP_ERR MMPF_USBH_SetUVCPrevwResol(MMP_USHORT usWidth, MMP_USHORT usHeight);
MMP_ERR MMPF_USBH_SetUVCPrevwFrameRate(MMP_UBYTE ubFps);
MMPF_USBH_DEV_GRP_CFG* MMPF_USBH_GetDevCFG(void);
MMP_ERR MMPF_USBH_ClrDevCFG(void);
MMP_ERR MMPF_USBH_AddDevCFG(MMP_UBYTE *pubStr, void *pOpenDevCallback, void *pStartDevCallback, void *pNaluInfo);
MMP_ERR MMPF_USBH_UpdDevCFG(MMP_ULONG ulEvent, MMP_UBYTE *pubStr, void *pParm);
MMP_ERR MMPF_USBH_RegDevAddiCFG(MMP_ULONG ulRegTyp, MMP_UBYTE *pubStr, MMP_ULONG ulParm0, MMP_ULONG ulParm1, MMP_ULONG ulParm2, MMP_ULONG ulParm3);
MMP_ERR MMPF_USBH_ExeCB(void *pCBFunc);

#endif

#endif //_MMPF_USBH_UVC_H_

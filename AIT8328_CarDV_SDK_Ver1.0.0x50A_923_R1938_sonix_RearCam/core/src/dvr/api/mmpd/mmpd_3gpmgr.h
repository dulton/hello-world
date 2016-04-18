/// @ait_only
/**
 @file mmpd_3gpmgr.h
 @brief Header File for the Host 3GP MERGER Driver.
 @author Will Tseng
 @version 1.0
*/

#ifndef _MMPD_3GPMGR_H_
#define _MMPD_3GPMGR_H_

#include "includes_fw.h"
#include "ait_config.h"
#include "mmp_lib.h"
#include "mmp_mux_inc.h"
#include "mmp_graphics_inc.h"

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

#define LARGE_FRAME_EN      	(1)

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

/// AV operation mode. This is for recorder only, used in Initialization
typedef enum _MMPD_3GPMGR_AUDIO_FORMAT {
    MMPD_3GPMGR_AUDIO_FORMAT_AAC = 0x00,    ///< Video encode with AAC audio
    MMPD_3GPMGR_AUDIO_FORMAT_AMR,           ///< Video encode with AMR audio
    MMPD_3GPMGR_AUDIO_FORMAT_ADPCM,         ///< Video encode with ADPCM audio
    MMPD_3GPMGR_AUDIO_FORMAT_MP3,           ///< Video encode with MP3 audio
    MMPD_3GPMGR_AUDIO_FORMAT_PCM            ///< Video encode with raw PCM audio
} MMPD_3GPMGR_AUDIO_FORMAT;

/// 3gp data format
typedef enum _MMPD_3GPMGR_FORMAT {
    MMPD_3GPMGR_FORMAT_OTHERS = 0x00,
    MMPD_3GPMGR_FORMAT_H263,
    MMPD_3GPMGR_FORMAT_MP4V,
    MMPD_3GPMGR_FORMAT_H264
} MMPD_3GPMGR_FORMAT;

/// Buffer info
typedef struct _MMPD_3GPMGR_REPACKBUF {
    MMP_ULONG ulAvRepackStartAddr;   			///< AV repack buffer start address for 3gp encoder (can be calculated)
    MMP_ULONG ulAvRepackSize;     				///< AV repack buffer size for 3gp encoder (can be calculated)
    MMP_ULONG ulVideoEncSyncAddr;   	      	///< Parameter sync buffer for 3gp encoder (can be calculated)
    MMP_ULONG ulVideoSizeTableAddr;		   		///< Video encode frame table buffer start address (can be calculated)
    MMP_ULONG ulVideoSizeTableSize;     		///< Video encode frame table buffer size (can be calculated)
    MMP_ULONG ulVideoTimeTableAddr;   			///< Video encode time table buffer start address (can be calculated)
    MMP_ULONG ulVideoTimeTableSize;    			///< Video encode time table buffer size (can be calculated)
	MMP_ULONG ulVideoRegStoreAddr;				///< Buffer to store register for frame base encoder
} MMPD_3GPMGR_REPACKBUF;

/// Aux table index
typedef enum _MMPD_3GPMGR_AUX_TABLE {
    MMPD_3GPMGR_AUX_FRAME_TABLE = 0,			///< Video encode aux frame table
    MMPD_3GPMGR_AUX_TIME_TABLE = 1				///< Video encode aux time table
} MMPD_3GPMGR_AUX_TABLE;

typedef struct _MMPD_3GPMGR_AVCOMPRESSEDBUFFER {
	MMP_ULONG ulVideoCompBufStart;
	MMP_ULONG ulVideoCompBufEnd;
	MMP_ULONG ulAudioCompBufStart;
	MMP_ULONG ulAudioCompBufEnd;
} MMPD_3GPMGR_AVCOMPRESSEDBUFFER;

/// Video speed mode
typedef enum _MMPD_3GPMGR_SPEED_MODE {
    MMPD_3GPMGR_SPEED_NORMAL = 0,                      ///< Video speed, normal
    MMPD_3GPMGR_SPEED_SLOW,                            ///< Video speed, slow
    MMPD_3GPMGR_SPEED_FAST                             ///< Video speed, fast
} MMPD_3GPMGR_SPEED_MODE;

/// Video speed mode
typedef enum _MMPD_3GPMGR_SPEED_RATIO {
    MMPD_3GPMGR_SPEED_1X = 0,                          ///< Video speed ratio
    MMPD_3GPMGR_SPEED_2X,
    MMPD_3GPMGR_SPEED_3X,
    MMPD_3GPMGR_SPEED_4X,
    MMPD_3GPMGR_SPEED_5X,
    MMPD_3GPMGR_SPEED_MAX
} MMPD_3GPMGR_SPEED_RATIO;

/// Video event
typedef enum _MMPD_3GPMGR_EVENT {
    MMPD_3GPMGR_EVENT_NONE = 0,
    MMPD_3GPMGR_EVENT_MEDIA_FULL,
    MMPD_3GPMGR_EVENT_FILE_FULL,
    MMPD_3GPMGR_EVENT_LONG_TIME_FILE_FULL,
    MMPD_3GPMGR_EVENT_MEDIA_SLOW,
    MMPD_3GPMGR_EVENT_SEAMLESS,
    MMPD_3GPMGR_EVENT_MEDIA_ERROR,
    MMPD_3GPMGR_EVENT_ENCODE_START,
    MMPD_3GPMGR_EVENT_ENCODE_STOP,
	#if (DUALENC_SUPPORT == 1)
    MMPD_3GPMGR_EVENT_DUALENCODE_START,
    MMPD_3GPMGR_EVENT_DUALENCODE_STOP,
	#endif
    MMPD_3GPMGR_EVENT_POSTPROCESS,
    MMPD_3GPMGR_EVENT_BITSTREAM_DISCARD,
    MMPD_3GPRECD_EVENT_MEDIA_WRITE,
    MMPD_3GPRECD_EVENT_STREAMCB,
    MMPD_3GPRECD_EVENT_EMERGFILE_FULL,
    MMPD_3GPRECD_EVENT_RECDSTOP_CARDSLOW,
    MMPD_3GPRECD_EVENT_APSTOPVIDRECD,
    MMPD_3GPRECD_EVENT_PREGETTIME_CARDSLOW,
    MMPD_3GPRECD_EVENT_UVCFILE_FULL,
    MMPD_3GPRECD_EVENT_COMPBUF_FREE_SPACE,
    MMPD_3GPRECD_EVENT_APNEED_STOP_RECD,
    MMPD_3GPRECD_EVENT_DUALENC_FILE_FULL
} MMPD_3GPMGR_EVENT;

/// Video frame type
typedef enum _MMPD_3GPMGR_FRAME_TYPE {
    MMPD_3GPMGR_FRAME_TYPE_I = 0,
    MMPD_3GPMGR_FRAME_TYPE_P,
    MMPD_3GPMGR_FRAME_TYPE_B,
    MMPD_3GPMGR_FRAME_TYPE_MAX
} MMPD_3GPMGR_FRAME_TYPE;

typedef enum _MMPD_3GPMGR_CONTAINER {
    MMPD_3GPMGR_CONTAINER_3GP = 0,
    MMPD_3GPMGR_CONTAINER_AVI,
    MMPD_3GPMGR_CONTAINER_NONE,
    MMPD_3GPRECD_CONTAINER_UNKNOWN
} MMPD_3GPMGR_CONTAINER;

//sync MMPF_USBH_UPD_UVC_CFG_OP
typedef enum _MMPD_3GPMGR_UPD_UVC_CFG_OP {
    MMPD_3GPMGR_UPD_OP_NONE         = 0,
    MMPD_3GPMGR_UPD_OPEN_UVC_CB     = 1,
    MMPD_3GPMGR_UPD_START_UVC_CB    = 2,
    MMPD_3GPMGR_UPD_NALU_TBL        = 3,
    MMPD_3GPMGR_UPD_OP_MAX          = 4
} MMPD_3GPMGR_UPD_UVC_CFG_OP;

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
/** @addtogroup MMPD_3GPMGR
 *  @{
 */

typedef MMP_USHORT (*MMPD_3GPMGR_FileWrite)(void *FileHandle, MMP_UBYTE *ubSrcAddr, MMP_ULONG ulByte, MMP_ULONG *ulCount, MMP_BOOL bAsync);
typedef MMP_USHORT (*MMPD_3GPMGR_FileRead)(void *FileHandle, MMP_UBYTE *uData, MMP_ULONG ulNumBytes, MMP_ULONG *ulReadCount);
typedef MMP_USHORT (*MMPD_3GPMGR_FileSeek)(void *FileHandle, MMP_LONG64 llOffset, MMP_USHORT ulOrigin);

MMP_ERR MMPD_3GPMGR_PreCapture(MMP_ULONG ulEncId, MMP_ULONG ulPreCaptureMs);
MMP_ERR MMPD_3GPMGR_StartCapture(MMP_ULONG ubEncId, MMP_USHORT filetype);
MMP_ERR MMPD_3GPMGR_StopCapture(MMP_ULONG ubEncId, MMP_USHORT filetype);
MMP_ERR MMPD_3GPMGR_PauseCapture(void);
MMP_ERR MMPD_3GPMGR_ResumeCapture(void);
MMP_ERR MMPD_3GPMGR_SetStoragePath(MMP_UBYTE ubEnable);
MMP_ERR MMPD_3GPMGR_SetFileName(MMP_BYTE bFileName[], MMP_USHORT usLength, MMP_USHORT usFileType);
MMP_ERR MMPD_3GPMGR_SetUserDataAtom(MMP_BYTE AtomName[], MMP_BYTE UserDataBuf[], MMP_USHORT UserDataLength, MMP_USHORT usFileType);
MMP_ERR MMPD_3GPMGR_EnableAVSyncEncode(MMP_UBYTE ubEnable);
MMP_BOOL MMPD_3GPMGR_GetAVSyncEncode(void);
MMP_ERR MMPD_3GPMGR_SetEncodeCompBuf(MMPD_3GPMGR_AVCOMPRESSEDBUFFER *BufInfo, MMP_3GPRECD_FILETYPE filetype);
MMP_ERR MMPD_3GPMGR_SetTempBuf2FixedTailInfo(MMP_ULONG tempaddr, MMP_ULONG tempsize, MMP_ULONG AVaddr, MMP_ULONG AVsize, MMP_ULONG Reseraddr, MMP_ULONG Resersize);
MMP_ERR MMPD_3GPMGR_Set3GPCreateModifyTimeInfo(MMP_3GPRECD_FILETYPE filetype, MMP_ULONG CreateTime, MMP_ULONG ModifyTime);
MMP_ERR MMPD_3GPMGR_ModifyAVIListAtom(MMP_BOOL bEnable, MMP_BYTE *pStr);
MMP_ERR MMPD_3GPMGR_Get3gpFileCurSize(MMP_ULONG *ulCurSize);
MMP_ERR MMPD_3GPMGR_Get3gpSaveStatus(MMP_UBYTE *status);
MMP_ERR MMPD_3GPMGR_Get3gpFileSize(MMP_ULONG *filesize);
MMP_ERR MMPD_3GPMGR_SetFileLimit(MMP_ULONG ulFileMax, MMP_ULONG ulReserved, MMP_ULONG *ulSpace);

MMP_ERR MMPD_3GPMGR_SetAudioParam(  MMP_ULONG                   param,
                                    MMPD_3GPMGR_AUDIO_FORMAT    AudioMode);

MMP_ERR MMPD_3GPMGR_GetRecordingTime(MMP_ULONG *ulTime, MMP_ULONG ulFileType);
MMP_ERR MMPD_3GPMGR_GetRecordingDuration(MMP_ULONG *ulTime, MMP_ULONG ulFileType);
MMP_ERR MMPD_3GPMGR_GetRecordingOffset(MMP_ULONG *ulTime, MMP_ULONG ulFileType);
MMP_ERR MMPD_3GPMGR_SetTimeLimit(MMP_ULONG ulTimeMax);
MMP_ERR MMPD_3GPMGR_SetTimeDynamicLimit(MMP_ULONG ulTimeMax);
MMP_ERR MMPD_3GPMGR_GetStatus(MMP_ERR *status, MMP_ULONG *tx_status);
MMP_UBYTE MMPD_3GPMGR_GetStoragePath(void);
MMPD_3GPMGR_AUDIO_FORMAT MMPD_3GPMGR_GetAudioFormat(void);

MMP_ERR MMPD_3GPMGR_SetGOP(MMP_USHORT usPFrame, MMP_USHORT usBFrame, MMP_USHORT usfiletype);

MMP_ERR MMPD_3GPMGR_SetRepackMiscBuf(MMPD_3GPMGR_REPACKBUF *repackbuf, MMP_3GPRECD_FILETYPE filetype);
MMP_ERR MMPD_3GPMGR_SetSeamless(MMP_BOOL enable);
MMP_ERR MMPD_3GPMGR_SetRecordSpeed(MMPD_3GPMGR_SPEED_MODE SpeedMode, MMPD_3GPMGR_SPEED_RATIO SpeedRatio);
MMP_ERR MMPD_3GPMGR_SetRecordTailSpeed(MMP_BOOL ubHighSpeedEn, MMP_ULONG ulTailInfoAddress, MMP_ULONG ulTailInfoSize, MMP_3GPRECD_FILETYPE filetype);

MMP_ERR MMPD_3GPMGR_SetContainerType(MMPD_3GPMGR_CONTAINER type);
MMP_ERR MMPD_3GPMGR_RegisterCallback(MMPD_3GPMGR_EVENT event, void *CallBack);
MMP_ERR MMPD_3GPMGR_GetRegisteredCallback(MMPD_3GPMGR_EVENT event, void **CallBack);
MMP_ERR MMPD_3GPMGR_SetSkipCntThreshold(MMP_USHORT threshold);
MMP_ERR MMPD_3GPMGR_MakeRoom(MMP_ULONG ulEncId, MMP_ULONG ulRequiredSize);

MMP_ERR MMPD_3GPMGR_GetEmergentRecStatus(MMP_BOOL *bEnable, MMP_BOOL *bRecording);
MMP_ERR MMPD_3GPMGR_EnableEmergentRecd(MMP_BOOL bEnabled);
MMP_ERR MMPD_3GPMGR_EnableUVCEmergentRecd(MMP_BOOL bEnabled);
MMP_ERR MMPD_3GPMGR_StartEmergentRecd(MMP_BOOL bStopVidRecd);
MMP_ERR MMPD_3GPMGR_StopEmergentRecd(void);
MMP_ERR MMPD_3GPMGR_SetEmergentTimeLimit(MMP_ULONG ulTimeMax);
MMP_ERR MMPD_3GPMGR_SetEmergPreEncTimeLimit(MMP_ULONG ulTimeMax);
MMP_ERR MMPD_3GPMGR_SetTempFileNameAddr(MMP_ULONG addr, MMP_ULONG size);
MMP_ERR MMPD_3GPMGR_SetVidRecdSkipModeParas(MMP_ULONG ulTotalCount, MMP_ULONG ulContinCount);
MMP_ERR MMPD_3GPMGR_SetDualH264Enable(MMP_BOOL bEnable, MMP_ULONG type);

MMP_ERR MMPD_3GPMGR_SetSEIShutterMode(MMP_ULONG ulMode);

MMP_ERR MMPD_3GPMGR_StartUVCRecd(MMP_UBYTE type);
MMP_ERR MMPD_3GPMGR_EnableUVCRecd(void);
MMP_ERR MMPD_3GPMGR_StopUVCRecd(void);
MMP_ERR MMPD_3GPMGR_SetUVCRepackMiscBuf(MMP_ULONG ulFileType, MMPD_3GPMGR_REPACKBUF *repackbuf);
MMP_ERR MMPD_3GPMGR_UVCRecdInputFrame(MMP_ULONG bufaddr, MMP_ULONG size, MMP_ULONG timestamp,
									  MMP_USHORT frametype, MMP_USHORT vidaudtype);
MMP_ERR MMPD_3GPMGR_SetUVCFBMemory(MMP_LONG plStartAddr, MMP_ULONG *plSize);
MMP_ERR MMPD_3GPMGR_StartUVCPrevw(void);
MMP_ERR MMPD_3GPMGR_StopUVCPrevw(void);
MMP_ERR MMPD_3GPMGR_SetUVCPrevwWinID(MMP_UBYTE ubWinID);
MMP_ERR MMPD_3GPMGR_SetUVCPrevwRote(MMP_GRAPHICS_ROTATE_TYPE ubRoteType);
MMP_ERR MMPD_3GPMGR_SetUVCRecdResol(MMP_UBYTE ubResol, MMP_USHORT usWidth, MMP_USHORT usHeight);
MMP_ERR MMPD_3GPMGR_SetUVCRecdBitrate(MMP_ULONG ulBitrate);
MMP_ERR MMPD_3GPMGR_SetUVCRecdFrameRate(MMP_USHORT usTimeIncrement, MMP_USHORT usTimeIncrResol);
MMP_ERR MMPD_3GPMGR_SetUVCRecdPFrameCount(MMP_USHORT usFrameCount);
MMP_ERR MMPD_3GPMGR_SetUVCPrevwResol(MMP_USHORT usWidth, MMP_USHORT usHeight);
MMP_ERR MMPD_3GPMGR_IsUVCPrevwResolSet(MMP_BOOL *pbIsSet);
MMP_ERR MMPD_3GPMGR_SetUVCPrevwFrameRate(MMP_UBYTE ubFps);
MMP_ERR MMPD_3GPMGR_AddDevCFG(MMP_UBYTE *pubStr, void *pOpenDevCallback, void *pStartDevCallback, void *pNaluInfo);
MMP_ERR MMPD_3GPMGR_UpdDevCFG(MMPD_3GPMGR_UPD_UVC_CFG_OP Event, MMP_UBYTE *pubStr, void *pParm);
MMP_ERR MMPD_3GPMGR_RegDevAddiCFG(MMP_ULONG ulRegTyp, MMP_UBYTE *pubStr, MMP_ULONG ulParm0, MMP_ULONG ulParm1, MMP_ULONG ulParm2, MMP_ULONG ulParm3);
MMP_ERR MMPD_3GPMGR_SetClassIfCmd(MMP_UBYTE bReq, MMP_USHORT wVal, MMP_USHORT wInd, MMP_USHORT wLen, MMP_UBYTE *UVCDataBuf);
MMP_ERR MMPD_3GPMGR_GetClassIfCmd(MMP_UBYTE bReq, MMP_USHORT wVal, MMP_USHORT wInd, MMP_USHORT wLen, MMP_ULONG *UVCDataLength, MMP_UBYTE *UVCDataBuf);
MMP_ERR MMPD_3GPMGR_SetStdIfCmd(MMP_UBYTE bReq, MMP_USHORT wVal, MMP_USHORT wInd, MMP_USHORT wLen, MMP_UBYTE *UVCDataBuf);
MMP_ERR MMPD_3GPMGR_GetStdDevCmd(MMP_UBYTE bReq, MMP_USHORT wVal, MMP_USHORT wInd, MMP_USHORT wLen, MMP_ULONG *UVCDataLength, MMP_UBYTE *UVCDataBuf);
MMP_ERR MMPD_3GPMGR_SetEncodeFormat(MMP_USHORT usFileType, MMP_USHORT usFormat);
MMP_ERR MMPD_3GPMGR_SetEncodeResolution(MMP_USHORT usFileType, MMP_USHORT ResolW, MMP_USHORT ResolH);
MMP_ERR MMPD_3GPMGR_SetFrameRate(MMP_USHORT usFileType, MMP_ULONG timeresol, MMP_ULONG timeincrement);
MMP_ERR MMPD_3GPMGR_SetEncodeGOP(MMP_USHORT usFileType, MMP_USHORT usPFrame, MMP_USHORT usBFrame);	
MMP_ERR MMPD_3GPMGR_SetEncodeSPSPPSHdr(MMP_USHORT usFileType, MMP_ULONG ulSPSAddr, MMP_USHORT ulSPSSize, MMP_ULONG ulPPSAddr, 
									   MMP_USHORT ulPPSSize, MMP_USHORT ulProfileIDC, MMP_USHORT ulLevelIDC);
MMP_ERR MMPD_3GPMGR_SetTime2FlushFSCache(MMP_ULONG time);
MMP_ERR MMPD_3GPMGR_CheckFile2Refix(void);
MMP_ERR MMPD_3GPMGR_SetThumbnailInfo(MMP_ULONG uladdr, MMP_ULONG ulsize);
MMP_ERR MMPD_3GPMGR_SetThumbnailBuf(MMP_ULONG uladdr, MMP_ULONG ulsize);

#if (DUALENC_SUPPORT)
MMP_ERR MMPD_3GPMGR_StartAllCapture(MMP_UBYTE ubTotalEncCnt, MMP_ULONG *pEncID);
#endif

#endif // _INCLUDES_H_
/// @end_ait_only

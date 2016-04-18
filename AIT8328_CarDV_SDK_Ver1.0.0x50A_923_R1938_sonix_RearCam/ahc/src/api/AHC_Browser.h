/**
 @file AHC_Browser.h
 @brief Header File for the AHC browser API.
 @author 
 @version 1.0
*/

#ifndef _AHC_BROWSER_H_
#define _AHC_BROWSER_H_

/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "AHC_Common.h"
#include "AHC_Video.h"
#include "AHC_Audio.h"
#include "AHC_General.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/ 

#define TIME_SEC(TotalTime)        (((TotalTime + 500)/1000)%60) //round it up if totaltime is over then 500
#define TIME_MIN(TotalTime)        (((TotalTime + 500)/1000/60)%60)
#define TIME_HOUR(TotalTime)       ((TotalTime + 500)/1000/(60*60))

/*===========================================================================
 * Structure define
 *===========================================================================*/ 

typedef enum _AHC_VIDEO_CODEC_FORMAT 
{
    AHC_VIDEO_CODEC_NONE        = 0,
    AHC_VIDEO_CODEC_H263        = 0x1,
    AHC_VIDEO_CODEC_MP4V_SP     = 0x2,
    AHC_VIDEO_CODEC_MP4V        = 0x2,
    AHC_VIDEO_CODEC_MP4V_ASP    = 0x4,
    AHC_VIDEO_CODEC_H264        = 0x8,
    AHC_VIDEO_CODEC_RV          = 0x10,
    AHC_VIDEO_CODEC_WMV         = 0x20,
    AHC_VIDEO_CODEC_UNSUPPORTED = 0x80,
    AHC_VIDEO_CODEC_MAX
} AHC_VIDEO_CODEC_FORMAT;

typedef struct _AHC_THUMB_CFG {
	UINT16 uwTHMPerLine;
	UINT16 uwTHMPerColume;
	UINT16 uwHGap;
	UINT16 uwVGap;	
	UINT32 byBkColor;
	UINT32 byFocusColor;
	
	UINT32 StartX;
	UINT32 StartY;
	UINT32 DisplayWidth;
	UINT32 DisplayHeight;
	
} AHC_THUMB_CFG;

typedef struct _AHC_VIDEO_ATTR{
    AHC_VIDEO_CODEC_FORMAT  VideoFormat[2];
    AHC_AUDIO_CODEC_FORMAT  AudioFormat;
    UINT32                  VideoAvailable[2];
    UINT32                  AudioAvailable;
    UINT32                  Seekable[2];
    UINT32                  Width[2];
    UINT32                  Height[2];
    UINT32                  SampleRate;
    UINT32                  Channels;
    UINT32                  TotalTime;
} AHC_VIDEO_ATTR;

/*===========================================================================
 * Enum define
 *===========================================================================*/ 

typedef enum _AHC_PLAYBACK_CMDS {
	AHC_PB_MOVIE_PAUSE = 0,
	AHC_PB_MOVIE_RESUME,
	AHC_PB_MOVIE_FAST_FORWARD,
	AHC_PB_MOVIE_FAST_FORWARD_RATE,
	AHC_PB_MOVIE_BACKWARD,
	AHC_PB_MOVIE_BACKWARD_RATE,
	AHC_PB_MOVIE_AUDIO_MUTE,
	//AHC_PB_MOVIE_SEEK_BY_FRAME,
	AHC_PB_MOVIE_SEEK_BY_TIME,
	AHC_PB_MOVIE_ROTATE,
	AHC_PB_MOVIE_NORMAL_PLAY_SPEED,
	AHC_PB_MOVIE_SEEK_TO_SOS
	/*
	AHC_PB_PAUSE_AVI = 0,
	AHC_PB_SLOW_MOTION_AVI,
	AHC_PB_PAUSE_WAVMP3,
	AHC_PB_RESUME_WAVMP3,
	AHC_PB_SEEK_TIME_WAV,
	AHC_PB_STOP_WAVMPE,
	AHC_PB_MUTE_WAVMPE,
	AHC_PB_MPEG_FAST_FORWARD,
	AHC_PB_MPEG_FAST_FORWARD_RATE
	*/
} AHC_PLAYBACK_CMDS;

typedef enum _AHC_PLAYBACKSEL{
    AHC_PLAYBACK_PREVIOUS = 0,
    AHC_PLAYBACK_CURRENT  ,
    AHC_PLAYBACK_NEXT     ,
    AHC_PLAYFILESEL_MAX   
} AHC_PLAYBACKSEL;

typedef enum _AHC_FILETYPE {
    AHC_FILETYPE_IMAGE = 0,
    AHC_FILETYPE_VIDEO ,
    AHC_FILETYPE_AUDIO ,
    AHC_FILETYPE_UNSUPPORTED
} AHC_FILETYPE;

/*===========================================================================
 * Function prototype
 *===========================================================================*/ 

/** @addtogroup AHC_BROWSER
@{
*/
AHC_BOOL AHC_Thumb_Config(AHC_THUMB_CFG *pConfig, AHC_BOOL bKeephumbIndex);
AHC_BOOL AHC_Thumb_GetConfig(AHC_THUMB_CFG *pConfig);
AHC_BOOL AHC_Thumb_IsFileProtect(UINT32 wDcfObjIdx);
AHC_BOOL AHC_Thumb_IsFileLocked(UINT32 wDcfObjIdx);
AHC_BOOL AHC_Thumb_ProtectKey(void);
AHC_BOOL AHC_Thumb_Rewind(UINT8 byNum);
AHC_BOOL AHC_Thumb_Forward(UINT8 byNum);
AHC_BOOL AHC_Thumb_GetFirstIdxCurPage(UINT16 *pwIndex);
AHC_BOOL AHC_PlaybackClipCmd(AHC_PLAYBACK_CMDS bCommand, UINT32 iOp);
AHC_BOOL AHC_GetVideoFileAttr(AHC_VIDEO_ATTR *pVideoAttr);
AHC_BOOL AHC_GetVideoCurrentPlayTime(UINT32 *pulTime);
AHC_BOOL AHC_Thumb_RewindPage(void);
AHC_BOOL AHC_Thumb_ForwardPage(void);
AHC_BOOL AHC_Thumb_GetFirstIdx(UINT32 *puiIdx);
AHC_BOOL AHC_Thumb_GetPrevIdx(UINT32 *puiIdx);
AHC_BOOL AHC_Thumb_GetNextIdx(UINT32 *puiIdx);
AHC_BOOL AHC_Thumb_GetCurIdx(UINT32 *puiIdx);
AHC_BOOL AHC_Thumb_GetSorting(void);
AHC_BOOL AHC_Thumb_GetCurPos(UINT32 *puiPos);
AHC_BOOL AHC_Thumb_GetCurPage(UINT16 *puwPage);
AHC_BOOL AHC_Thumb_GetTotalPages(UINT16 *puwPages);
AHC_BOOL AHC_Thumb_GetMaxNumPerPage(UINT32  *ObjNumPerPage);
AHC_BOOL AHC_Thumb_GetCountCurPage(UINT16 *ulObjCount);
AHC_BOOL AHC_Thumb_GetCurPosCurPage(UINT16 *ulObjIndex);
AHC_BOOL AHC_DrawThumbNailEmpty(void);
AHC_BOOL AHC_Thumb_DrawPage(AHC_BOOL bForce);
AHC_BOOL AHC_DrawThumbNail(void);
void	 AHC_SetPlaySpeed(AHC_PLAYBACK_CMDS cmd, UINT32 speed);
int      AHC_Thumb_GetJPEGFromVidFF(UINT8 *pSrcBufAddr, UINT32 dwBufSize, char *pszFullPath, UINT16 wWdith, UINT16 wHeigh);
GUI_BITMAP* AHC_ICON_GetProtectKey(AHC_PROTECT_TYPE Type);
void AHC_ICON_LoadProtectKey(AHC_PROTECT_TYPE Type, GUI_BITMAP* psKeyIcon);
AHC_BOOL AHC_Thumb_SetCurrentIdx(UINT16 uiIndex);
void AHC_ICON_LoadFileDamage(GUI_BITMAP* psFileDamageIcon);
AHC_BOOL AHC_Thumb_SetCurIdx(UINT32 uiIdx);
AHC_BOOL AHC_Thumb_Quick_DrawLockOrUnlock(void);
void AHC_Thumb_SetEmrBMPIcon(GUI_BITMAP * psEmrIcon);
/// @}

void AHC_BlinkFocusRec(RECT* psRECT,  AHC_BOOL bRestore);
#define AHC_IsFileProtect			AHC_Thumb_IsFileProtect
#define AHC_GetDCFObjNumPerPage		AHC_Thumb_GetMaxNumPerPage

#endif //_AHC_BROWSER_H_
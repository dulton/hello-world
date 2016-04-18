//==============================================================================
//
//  File        : AHC_Capture.h
//  Description : INCLUDE File for the AHC capture function porting.
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================

#ifndef _AHC_VIDEO_H_
#define _AHC_VIDEO_H_

/*===========================================================================
 * Include files
 *===========================================================================*/

#include "Customer_config.h"
#include "AHC_Common.h"
#include "AHC_Config_SDK.h"
#include "Mmp_mux_inc.h"

/*===========================================================================
 * Enum define
 *===========================================================================*/

// Note that these enumerates must be group in video, audio and movie order.
// leave the naming such as AHC_CLIP for better comparison with CarDV code base.
typedef enum _AHC_MOVIE_CONFIG 
{
	/* Video Config */
    AHC_MAX_PFRAME_NUM = 0,
    AHC_MAX_BFRAME_NUM,
    AHC_MAX_BRC_QSCALE,
    AHC_VIDEO_RESOLUTION,
    AHC_FRAME_RATE,
    AHC_FRAME_RATEx10,
    AHC_VIDEO_BITRATE,
    AHC_VIDEO_CODEC_TYPE,
    AHC_VIDEO_CODEC_TYPE_SETTING, 	// MMPS_3GPRECD_PROFILE
    AHC_VIDEO_ENCODE_MODE, 			// MMPS_3GPRECD_CURBUF_MODE
    AHC_VIDEO_COMPRESSION_RATIO,
    AHC_VIDEO_RESERVED_SIZE,
    AHC_VIDEO_STREAMING, 			// AHC_STREAMING_MODE
    AHC_VIDEO_CONFIG_MAX,
    /* Audio Config */
    AHC_AUD_MUTE_END = 100,
    AHC_AUD_SAMPLE_RATE,
    AHC_AUD_CHANNEL_CONFIG, 		// AHC_AUDIO_CHANNEL_CONFIG, not number of channels
    AHC_AUD_BITRATE,
    AHC_AUD_CODEC_TYPE,
    AHC_AUD_PRERECORD_DAC,
    AHC_AUD_STREAMING,
    AHC_AUD_CONFIG_MAX,
    /* Movie Config */
    AHC_CLIP_CONTAINER_TYPE = 200,
    AHC_VIDEO_PRERECORD_LENGTH,
    AHC_VIDEO_PRERECORD_STATUS,
    AHC_MOVIE_CONFIG_MAX
} AHC_MOVIE_CONFIG;

typedef enum _AHC_MOVIE_CAPTURE_CMD 
{
	AHC_CAPTURE_CLIP_PAUSE = 0,
	AHC_CAPTURE_CLIP_RESUME,
	AHC_CAPTURE_SNAPSHOT
} AHC_MOVIE_CAPTURE_CMD;

typedef enum _AHC_MOVIE_CONFIG_PARAM 
{
    AHC_MOVIE_CONTAINER_3GP = 0,
    AHC_MOVIE_CONTAINER_AVI,
    
    AHC_MOVIE_VIDEO_CODEC_NONE,
    AHC_MOVIE_VIDEO_CODEC_MPEG4,
    AHC_MOVIE_VIDEO_CODEC_H264,
    AHC_MOVIE_VIDEO_CODEC_MJPEG,
    AHC_MOVIE_VIDEO_CODEC_YUV422,
    
    AHC_MOVIE_AUDIO_CODEC_AAC,		///< Video encode with AAC audio
    AHC_MOVIE_AUDIO_CODEC_ADPCM,	///< Video encode with ADPCM audio
    AHC_MOVIE_AUDIO_CODEC_MP3,		///< Video encode with MP3 audio
    AHC_MOVIE_AUDIO_CODEC_G711,		///< Video encode with G.711 audio
    AHC_MOVIE_AUDIO_MAX
} AHC_MOVIE_CONFIG_PARAM;

typedef enum _AHC_VIDRECD_CMD
{
    AHC_VIDRECD_INIT,				///< Init the sensor and possible relevant previewing image flow pipe
    AHC_VIDRECD_START,
    AHC_VIDRECD_STOP,
    AHC_VIDRECD_RESTART_PREVIEW, 	///< It's used to avoid instant flicker when entering Preview mode.
    AHC_VIDRECD_MAX
} AHC_VIDRECD_CMD;

typedef enum _AHC_VIDRECD_RESOL 
{
    AHC_VIDRECD_RESOL_128x96 = 0,
    AHC_VIDRECD_RESOL_176x144   ,
    AHC_VIDRECD_RESOL_320x240   ,
    AHC_VIDRECD_RESOL_352x288   ,
    AHC_VIDRECD_RESOL_640x360   ,
    AHC_VIDRECD_RESOL_640x480   , //0x05:05
    AHC_VIDRECD_RESOL_720x480   ,
    AHC_VIDRECD_RESOL_128x128   ,
    AHC_VIDRECD_RESOL_1280x720  ,
    AHC_VIDRECD_RESOL_1280x720_60p,
    AHC_VIDRECD_RESOL_1440x1088 , //0x0A:10
    AHC_VIDRECD_RESOL_160x120   ,
    AHC_VIDRECD_RESOL_1920x1088 ,
    AHC_VIDRECD_RESOL_384x216   ,
    AHC_VIDRECD_RESOL_2304x1296 ,
    AHC_VIDRECD_RESOL_2560x1440 , //0x0F:15
    AHC_VIDRECD_RESOL_AUTO,
    AHC_VIDRECD_RESOL_MAX
} AHC_VIDRECD_RESOL;

#define AHC_VIDRECD_TIME_INCREMENT_BASE (1001)
#define AHC_VIDRECD_TIME_INCREMENT_BASE_NTSC (1000)
#define AHC_VIDRECD_TIME_INCREMENT_BASE_PAL (1200)

/*===========================================================================
 * Structure define
 *===========================================================================*/

typedef UINT16 AHC_VIDEO_FORMAT; 			// Maps to MMPS_3GPRECD_VIDEO_FORMAT
typedef UINT16 AHC_AUDIO_FORMAT; 			// Maps to MMPS_3GPRECD_AUDIO_FORMAT
typedef UINT16 AHC_CONTAINER;    			// Maps to MMPS_3GPRECD_CONTAINER
typedef UINT16 AHC_AUDIO_CHANNEL_CONFIG; 	// Maps to AHC_AUDIO_CHANNEL_CONFI
typedef UINT16 AHC_VIDEO_FORMAT_SETTING; 	// Maps to MMPS_3GPRECD_PROFILE or other format setting.
typedef UINT8  AHC_VIDEO_CURBUF_ENCODE_MODE;// Maps to MMPS_3GPRECD_CURBUF_MODE.

/*===========================================================================
 * Function prototype
 *===========================================================================*/

AHC_BOOL 	AIHC_GetMovieConfig(UINT32 wCommand, UINT32 *wOp);

AHC_BOOL 	AHC_SetMovieConfig(UINT16 i, AHC_MOVIE_CONFIG wCommand, UINT32 wOp);
AHC_BOOL 	AHC_ConfigMovie(UINT32 wCommand, UINT32 wOp);
AHC_BOOL 	AHC_CaptureClipCmd(UINT16 wCommand, UINT16 wOp);
AHC_BOOL 	AHC_GetCurrentRecordingTime(UINT32 *ulTime);
AHC_BOOL 	AHC_GetPreRecordStatus(UINT32 *ulStatus);
AHC_BOOL 	AHC_GetPreRecordDACStatus(UINT32 *ulStatus);
UINT32	 	AHC_GetVideoBitrate(int type);
UINT32	 	AHC_GetVideoRealFrameRate(UINT32 Frate);
AHC_BOOL 	AHC_WaitVideoWriteFileFinish(void);

AHC_BOOL 	AHC_SetFaceDetection_Video(UINT8 bMode);
AHC_BOOL 	AHC_GetVideoCurZoomStep(UINT16 *usZoomStepNum);
AHC_BOOL 	AHC_GetVideoMaxZoomStep(UINT16 *usMaxZoomStep);
AHC_BOOL 	AHC_GetVideoMaxZoomRatio(UINT16 *usMaxZoomRatio);
AHC_BOOL 	AHC_GetVideoDigitalZoomRatio(UINT32 *usZoomRatio);

UINT32	 	AHC_GetVideoRecTimeLimit(void);
AHC_BOOL 	AHC_Delete_DcfMinKeyFile(AHC_BOOL bFirstTime, const char *ext);
void        AHC_GetVideoRecStorageSpeceNeed(UINT32 ulVidBitRate, UINT32 ulAudBitRate, UINT32 ulTimelimit, UINT64 *pulSpaceNeeded);

AHC_BOOL 	AHC_GetVideoResolution(UINT16 stream, UINT16 *w, UINT16 *h);
void		AHC_AvailableRecordTime(UBYTE* Hour, UBYTE* Min, UBYTE* Sec);

void       	AHC_VideoSetSlowMotionFPS(AHC_BOOL bEnable, UINT32 usTimeIncrement, UINT32 usTimeIncrResol);
void       	AHC_VideoGetSlowMotionFPS(AHC_BOOL *pbEnable, UINT32 *pusTimeIncrement, UINT32 *pusTimeIncrResol);
#if (defined(VIDEO_REC_TIMELAPSE_EN) && VIDEO_REC_TIMELAPSE_EN == 1)
void       	AHC_VideoSetTimeLapseFPS(AHC_BOOL bEnable, UINT32 usTimeIncrement, UINT32 usTimeIncrResol);
void       	AHC_VideoGetTimeLapseFPS(AHC_BOOL *pbEnable, UINT32 *pusTimeIncrement, UINT32 *pusTimeIncrResol);
void 	   	VideoFunc_GetTimeLapseBitrate(UINT32 ulFrameRate, UINT32 ulTimeLapseSetting, UINT32 *pulVidBitRate, UINT32 *pulAudBitRate);
#endif
MMP_ERR    	AHC_SetVideoFileName( MMP_BYTE byFilename[], UINT16 usLength, MMP_3GPRECD_FILETYPE eFileType );

#endif //_AHC_VIDEO_H_

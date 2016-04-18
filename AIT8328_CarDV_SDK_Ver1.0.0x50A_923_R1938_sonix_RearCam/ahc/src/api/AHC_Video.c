//==============================================================================
//
//  File        : AHC_video.c
//  Description : AHC video function
//  Author      :
//  Revision    : 1.0
//
//==============================================================================

/*===========================================================================
 * Include files
 *===========================================================================*/

#include "AHC_Video.h"
#include "AHC_Audio.h"
#include "AHC_Capture.h"
#include "AHC_USB.h"
#include "AHC_USBHost.h"
//#include "AHC_Capture.h"
#include "AHC_Config_SDK.h"
#include "AHC_UF.h"
#include "AHC_DCF.h"
#include "AIHC_DCF.h"
#include "AHC_General.h"
#include "AHC_Menu.h"
#include "AHC_Utility.h"
#include "MenuSetting.h"
#include "dsc_key.h"
#include "ptz_cfg.h"
#include "mmps_iva.h"
#include "mmps_3gprecd.h"
#include "mmps_audio.h"
#include "mmps_sensor.h"
#include "mmps_vidplay.h"
#include "mmpf_usbh_uvc.h"
#include "mmps_fs.h"

/*===========================================================================
 * Macro Define
 *===========================================================================*/

// Max number of videos used in this code base
#define AHC_MAX_VIDEO	(2)
#define AHC_MAX_AUDIO	(1)

typedef UINT16 AHC_MEDIA_USAGE;

// Definition of AHC_MEDIA_USAGE. bit 0 is for switching the media on or off.
// bit 1 ~ 7 is for bit mapped configuration
// @Warning Be careful about the condition check, 0x40 still means this media is disabled.
#define AHC_MEDIA_DISABLE    ((AHC_MEDIA_USAGE)0x00)  // General disabling
#define AHC_MEDIA_ENABLE     ((AHC_MEDIA_USAGE)0x01)  // General enabling
#define AHC_MEDIA_STREAMING  ((AHC_MEDIA_USAGE)0x10)  // Output to network

/*===========================================================================
 * Structure
 *===========================================================================*/

// Properties of a single video
typedef struct _AHC_VIDEO_PROPERTY {
    AHC_MEDIA_USAGE 				Usage;
    AHC_VIDEO_FORMAT 				CodecType; 			// Map to MMPS_3GPRECD_VIDEO_FORMAT
    AHC_VIDEO_FORMAT_SETTING 		sCodecTypeSetting;  // Map to MMPS_3GPRECD_PROFILE or other format setting.
    AHC_VIDEO_CURBUF_ENCODE_MODE	sVidEncMode; 		// Map to MMPS_3GPRECD_CURBUF_MODE.
    UINT32 							SampleRate; 		// FPS, e.g. VIDRECD_FRAMERATE_30FPS
    UINT32 							FPSx10; 			// FPSx10, 75 means 7.5FPS
    UINT32 							Bitrate; 			// Video[0] always uses 3GP config as bitrate. Only other videos use this field.
    UINT32 							nMaxP; 				// Max number of P frames
    UINT32 							nMaxB; 				// Max number of B frames
    UINT32 							nMaxQP; 			// Max QP 51
    UINT32					 		CompressRatio; 		// Compression ratio, e.g. VIDRECD_QUALITY_HIGH
    UINT32 							ReservedSize;		// Reserved size
    UINT32 							Resolution; 		// Encode resolution, e.g. VIDRECD_RESOL_1920x1088
    AHC_BOOL	 					bStreaming; 		// Back up the status only. for quicker status reference
} AHC_VIDEO_PROPERTY;

// Properties of a single audio
typedef struct _AHC_AUDIO_PROPERTY {
    AHC_MEDIA_USAGE 				Usage;
    AHC_AUDIO_FORMAT 				CodecType; 			// Map to MMPS_3GPRECD_AUDIO_FORMAT;
    UINT32 							SampleRate;
    UINT32 							Bitrate;
    UINT32 							ChannelType;
    AHC_BOOL 						bPreRecordDAC;
    AHC_BOOL 						bMute;
    AHC_BOOL 						bStreaming; 		// Back up the status only. for quicker status reference
} AHC_AUDIO_PROPERTY;

// Property sets of the video used in this. This video includes audio information.
typedef struct _AHC_MOVIE {
    UINT16 							nVideo;				// Number of video
    UINT16 							nAudio;				// Number of audio
    AHC_CONTAINER 					ContainerType;		// Map to MMPS_3GPRECD_CONTAINER.
    AHC_BOOL 						PreRecordStatus;
    UINT32 							PreRecordLength; 	// PreEncode length, Unit ms
    AHC_VIDEO_PROPERTY 				v[AHC_MAX_VIDEO];
    AHC_AUDIO_PROPERTY 				a[AHC_MAX_AUDIO];
} AHC_MOVIE;

/*===========================================================================
 * Global variable
 *===========================================================================*/

static AHC_MOVIE m_sMovieConfig = 
{
	0, 							// nVideo
	0, 							// nAudio
	MMPS_3GPRECD_CONTAINER_3GP, // ContainerType
	AHC_FALSE, 					// PreRecordStatus
    10000, 						// PreRecordLength
    {
        {   AHC_MEDIA_DISABLE,				// Usage
            MMPS_3GPRECD_VIDEO_FORMAT_H264,	// CodecType
            MMPS_H264ENC_BASELINE_PROFILE,	// sCodecTypeSetting
            MMPS_3GPRECD_CURBUF_RT,			// sVidEncMode
            VIDRECD_FRAMERATE_30FPS,		// SampleRate
            300,							// FPSx10
            0, 								// Bitrate
            14,								// nMaxP 
            0,								// nMaxB
            51,								// nMaxQP
            VIDRECD_QUALITY_HIGH,			// CompressRatio
            2*1024*1024,					// ReservedSize
            VIDRECD_RESOL_1920x1088, 		// Resolution
            AHC_FALSE						// bStreaming
        },
        #if (AHC_MAX_VIDEO >= 2)
        {   AHC_MEDIA_DISABLE,				// Usage
            MMPS_3GPRECD_VIDEO_FORMAT_MJPEG,// CodecType
            MMPS_H264ENC_BASELINE_PROFILE,	// sCodecTypeSetting, No used
            MMPS_3GPRECD_CURBUF_RT,         // sVidEncMode, No used
            VIDRECD_FRAMERATE_30FPS,		// SampleRate
            300,							// FPSx10
            0,								// Bitrate
            0,								// nMaxP  
            0,								// nMaxB
            51,								// nMaxQP
            VIDRECD_QUALITY_HIGH,			// CompressRatio
            1*1024*1024,					// ReservedSize
            VIDRECD_RESOL_320x240,			// Resolution
            AHC_FALSE 						// bStreaming
        },
        #endif
    },
    {   AHC_MEDIA_DISABLE,					// Usage
        MMPS_3GPRECD_AUDIO_FORMAT_AAC, 		// CodecType
        16000,								// SampleRate
        32000,								// Bitrate
        1,									// ChannelType
        AHC_TRUE,							// bPreRecordDAC
        AHC_FALSE,							// bMute
    	AHC_FALSE 							// bStreaming
    }
};

MMPS_3GPRECD_FRAMERATE gsSlowMotionContainerFps = {1000, 30000};
AHC_BOOL gbSlowMotionEnable = MMP_FALSE;

#if (defined(VIDEO_REC_TIMELAPSE_EN) && VIDEO_REC_TIMELAPSE_EN)
MMPS_3GPRECD_FRAMERATE gsTimeLapseFps = {1000, 30000};
AHC_BOOL gbTimeLapseEnable = MMP_FALSE;
#endif

/*===========================================================================
 * Extern varible
 *===========================================================================*/ 

extern MMP_USHORT               gsAhcCurrentSensor;
extern AHC_BOOL                 gbAhcDbgBrk;
/*===========================================================================
 * Main body
 *===========================================================================*/

#if 0
void _____Internal_Functions_____(){}
#endif

#if (defined(VIDEO_REC_TIMELAPSE_EN) && VIDEO_REC_TIMELAPSE_EN)
void VideoFunc_GetTimeLapseBitrate(UINT32 ulFrameRate, UINT32 ulTimeLapseSetting, UINT32 *pulVidBitRate, UINT32 *pulAudBitRate)
{
    UINT32 ulTimeLapseRatio = 0;

    switch(ulTimeLapseSetting){
        case COMMON_VR_TIMELAPSE_5SEC:
            ulTimeLapseRatio = 5;
            break;
        case COMMON_VR_TIMELAPSE_10SEC:
            ulTimeLapseRatio = 10;
            break;
        case COMMON_VR_TIMELAPSE_30SEC:
            ulTimeLapseRatio = 30;
            break;
        case COMMON_VR_TIMELAPSE_60SEC:
            ulTimeLapseRatio = 60;
            break;
        default:
        case COMMON_VR_TIMELAPSE_1SEC:
            ulTimeLapseRatio = 1;
            break;
    }
    
    ulFrameRate = ulFrameRate * ulTimeLapseRatio;
    *pulVidBitRate = (*pulVidBitRate + ulFrameRate - 1) / ulFrameRate;
    *pulAudBitRate = 0;   // Mute
}
#endif

//------------------------------------------------------------------------------
//  Function    : AIHC_MapFromRawVRes
//  Description : Map Integer Video resolutions to AHC resolution
//------------------------------------------------------------------------------ 
AHC_BOOL AIHC_MapFromRawVRes(UINT16 w, UINT16 h, UINT16 fps, AHC_VIDRECD_RESOL *AhcRes)
{
    if (w >= 2560) {
        *AhcRes = AHC_VIDRECD_RESOL_2560x1440;
    }
    else if (w >= 2304) {
        *AhcRes = AHC_VIDRECD_RESOL_2304x1296;
    }
    else if (w >= 1920) {
        *AhcRes = AHC_VIDRECD_RESOL_1920x1088;
    }
    else if (w >= 1280) {
        if (fps == 60)
            *AhcRes = AHC_VIDRECD_RESOL_1280x720_60p;
        else
            *AhcRes = AHC_VIDRECD_RESOL_1280x720;
    } 
    else if (w >= 640) {
        if (h == 360)
            *AhcRes = AHC_VIDRECD_RESOL_640x360;
        else
            *AhcRes = AHC_VIDRECD_RESOL_640x480;
    }
    else if (w >= 384) {
        *AhcRes = AHC_VIDRECD_RESOL_384x216;
    }
    else if (w >= 320) {
        *AhcRes = AHC_VIDRECD_RESOL_320x240;
    }
    else if (w >= 176) {
        *AhcRes = AHC_VIDRECD_RESOL_176x144;
    }
    else if (w >= 160) {
        *AhcRes = AHC_VIDRECD_RESOL_160x120;
    }
    else if (w >= 128) {
        if (h == 128)
            *AhcRes = AHC_VIDRECD_RESOL_128x128;
        else
            *AhcRes = AHC_VIDRECD_RESOL_128x96;
    }
    else {
        *AhcRes = AHC_VIDRECD_RESOL_MAX;
        return AHC_FALSE;
    }

    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AIHC_MapToRawVRes
//  Description : Map AHC resolution to Integer Video resolutions
//------------------------------------------------------------------------------
AHC_BOOL AIHC_MapToRawVRes(AHC_VIDRECD_RESOL res, UINT16 *w, UINT16 *h)
{
    switch(res) {

    case AHC_VIDRECD_RESOL_320x240:
        *w = 320;
        *h = 240;
        break;
    case AHC_VIDRECD_RESOL_640x360:
        *w = 640;
        *h = 360;
        break;
    case AHC_VIDRECD_RESOL_640x480:
        *w = 640;
        *h = 480;
        break;
    case AHC_VIDRECD_RESOL_1280x720:
        *w = 1280;
        *h = 720;
        break;
    case AHC_VIDRECD_RESOL_1920x1088:
        *w = 1920;
        *h = 1080;
        break;
    case AHC_VIDRECD_RESOL_2304x1296:
        *w = 2304;
        *h = 1296;
        break;
    case AHC_VIDRECD_RESOL_2560x1440:
        *w = 2560;
        *h = 1440;
        break;		
    default:
        *h = 0; 
        *w = 0;
        return MMP_FALSE;
        break;
    }
    return MMP_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AIHC_GetMovieConfigEx
//  Description :
//------------------------------------------------------------------------------
/**
 * Get the config parameter of the movie
 * @param i[in] index of the media track if required.
 * @param wCommand[in] parameter to be get
 * @param wOp[out] The pointer of the parameter. Typically UINT32 type.
 * @return AHC_TRUE if supported
 */
AHC_BOOL AIHC_GetMovieConfigEx(UINT16 i, AHC_MOVIE_CONFIG wCommand, UINT32 *wOp)
{
    AHC_BOOL 	bCheckAudio = AHC_FALSE;
    AHC_BOOL 	bCheckVideo = AHC_FALSE;
    UINT32 		value = 0;

    switch(wCommand){
    case AHC_MAX_PFRAME_NUM:
        bCheckVideo = AHC_TRUE;
        value = m_sMovieConfig.v[i].nMaxP;
        break;
    case AHC_MAX_BFRAME_NUM:
        bCheckVideo = AHC_TRUE;
        value = m_sMovieConfig.v[i].nMaxB;
        break;
    case AHC_MAX_BRC_QSCALE:
        bCheckVideo = AHC_TRUE;
        value = m_sMovieConfig.v[i].nMaxQP;
        break;
	case AHC_VIDEO_RESOLUTION:
        bCheckVideo = AHC_TRUE;
        value = m_sMovieConfig.v[i].Resolution;
        break;
 	case AHC_FRAME_RATE:
        bCheckVideo = AHC_TRUE;
        value = m_sMovieConfig.v[i].SampleRate;
        break;
    case AHC_FRAME_RATEx10:
        bCheckVideo = AHC_TRUE;
        value = m_sMovieConfig.v[i].FPSx10;
        break;
    case AHC_VIDEO_BITRATE:
    	if (i == 1) {	// i is 1 for network video streaming
    		value = m_sMovieConfig.v[i].Bitrate;
    		break;
    	}
    	
        bCheckVideo = AHC_TRUE;
        
        {
            UINT32 ulBitrate = 0;

		#if (MENU_MOVIE_FRAME_RATE_EN)
			switch(MenuSettingConfig()->uiMOVFrameRate)
		    {
			#if (MENU_MOVIE_FRAME_RATE_30_EN)
			    case MOVIE_FRAME_RATE_30FPS:
					ulBitrate = MMPS_3GPRECD_GetConfig()->ulFps30BitrateMap[m_sMovieConfig.v[i].Resolution][m_sMovieConfig.v[i].CompressRatio];
		    	break;
		    #endif
		    #if (MENU_MOVIE_FRAME_RATE_20_EN)
			    case MOVIE_FRAME_RATE_20FPS:
					ulBitrate = MMPS_3GPRECD_GetConfig()->ulFps30BitrateMap[m_sMovieConfig.v[i].Resolution][m_sMovieConfig.v[i].CompressRatio]*2/3;
			    break;
			#endif
			#if (MENU_MOVIE_FRAME_RATE_15_EN)
				case MOVIE_FRAME_RATE_15FPS:
					ulBitrate = MMPS_3GPRECD_GetConfig()->ulFps30BitrateMap[m_sMovieConfig.v[i].Resolution][m_sMovieConfig.v[i].CompressRatio]/2;
				break;
			#endif
			#if (MENU_MOVIE_FRAME_RATE_10_EN)
			    case MOVIE_FRAME_RATE_10FPS:
					ulBitrate = MMPS_3GPRECD_GetConfig()->ulFps30BitrateMap[m_sMovieConfig.v[i].Resolution][m_sMovieConfig.v[i].CompressRatio]/3;
			    break;
			#endif
		    }
		#else
			if (VIDRECD_FRAMERATE_30FPS == m_sMovieConfig.v[i].SampleRate) {
	            ulBitrate = MMPS_3GPRECD_GetConfig()->ulFps30BitrateMap[m_sMovieConfig.v[i].Resolution][m_sMovieConfig.v[i].CompressRatio];
	        }
	        else {
	            ulBitrate = MMPS_3GPRECD_GetConfig()->ulFps60BitrateMap[m_sMovieConfig.v[i].Resolution][m_sMovieConfig.v[i].CompressRatio];
	        }
		#endif

            #if (C005_PARKING_CLIP_Length_10PRE_10REC) && (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
		    if (uiGetParkingModeEnable())
   		    #else
			if (MenuSettingConfig()->uiMOVPreRecord == MOVIE_PRE_RECORD_ON)
            #endif
			{
				if (ulBitrate > AHC_VIDEO_MAXBITRATE_PRERECORD)
				{
					ulBitrate = AHC_VIDEO_MAXBITRATE_PRERECORD;
				}
				
				#if (C005_PARKING_LOW_FRAME_RATE_15FPS)
	            if (MenuSettingConfig()->uiParkingMode_LowFrameRate == PARKINGMODE_LOW_FRAME_RATE_ON)
	            {
	            	ulBitrate = ulBitrate/2;
	            }
	            #endif
			}

        	value = ulBitrate;
        }
        break;
    case AHC_VIDEO_CODEC_TYPE:
        bCheckVideo = AHC_TRUE;
        value = m_sMovieConfig.v[i].CodecType;
        break;
    case AHC_VIDEO_CODEC_TYPE_SETTING:
        bCheckVideo = AHC_TRUE;

        switch (m_sMovieConfig.v[i].CodecType) {
            case MMPS_3GPRECD_VIDEO_FORMAT_H264:
                value = (UINT32)m_sMovieConfig.v[i].sCodecTypeSetting;
            case MMPS_3GPRECD_VIDEO_FORMAT_MJPEG:
                break;
            case MMPS_3GPRECD_VIDEO_FORMAT_YUV422:
                break;
            case MMPS_3GPRECD_VIDEO_FORMAT_YUV420:
                break;
            default:
                break;
        }
        break;
    case AHC_VIDEO_ENCODE_MODE:
        bCheckVideo = AHC_TRUE;
        value = (UINT32)m_sMovieConfig.v[i].sVidEncMode;
        break;
    case AHC_VIDEO_COMPRESSION_RATIO:
        bCheckVideo = AHC_TRUE;
        value= m_sMovieConfig.v[i].CompressRatio;
        break;
    case AHC_VIDEO_RESERVED_SIZE:
        bCheckVideo = AHC_TRUE;
        value= m_sMovieConfig.v[i].ReservedSize;
        break;
    case AHC_VIDEO_STREAMING:
        bCheckVideo = AHC_TRUE;
        value = m_sMovieConfig.v[i].bStreaming;
        break;
    /* Audio Config */
    case AHC_AUD_MUTE_END:
        bCheckAudio = AHC_TRUE;
        value = m_sMovieConfig.a[i].bMute;
        break;
    case AHC_AUD_SAMPLE_RATE:
        bCheckAudio = AHC_TRUE;
        value = m_sMovieConfig.a[i].SampleRate;
        break;
    case AHC_AUD_CHANNEL_CONFIG:
        bCheckAudio = AHC_TRUE;
        value = m_sMovieConfig.a[i].ChannelType;
        break;
    case AHC_AUD_BITRATE:
        bCheckAudio = AHC_TRUE;
        value = m_sMovieConfig.a[i].Bitrate;
        break;
    case AHC_AUD_CODEC_TYPE:
        bCheckAudio = AHC_TRUE;
        value = m_sMovieConfig.a[i].CodecType;
        break;
    case AHC_AUD_PRERECORD_DAC:
        bCheckAudio = AHC_TRUE;
        value = m_sMovieConfig.a[i].bPreRecordDAC;
        break;       
    case AHC_AUD_STREAMING:
    	bCheckAudio = AHC_TRUE;
        value = m_sMovieConfig.a[i].bStreaming;
        break;
   	/* Movie Config */
    case AHC_CLIP_CONTAINER_TYPE:
        value = m_sMovieConfig.ContainerType;
        break;
 	case AHC_VIDEO_PRERECORD_LENGTH:
        value = m_sMovieConfig.PreRecordLength;
        break;
    case AHC_VIDEO_PRERECORD_STATUS:
        value = m_sMovieConfig.PreRecordStatus;
        break;
    default:
        return AHC_FALSE;
        break;
    }

    if (bCheckAudio) {
        if (i >= AHC_MAX_AUDIO) {
            return AHC_FALSE;
        }
    }
    if (bCheckVideo) {
        if (i >= AHC_MAX_VIDEO) {
            return AHC_FALSE;
        }
    }
    
    *wOp = value;

    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetMovieConfig
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AIHC_GetMovieConfig(UINT32 wCommand, UINT32 *wOp)
{
    return AIHC_GetMovieConfigEx(0, wCommand, wOp);
}

#if 0
void _____Movie_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : AHC_SetMovieConfig
//  Description :
//------------------------------------------------------------------------------
/**
 * Set the config parameter of the movie

 This API configures both record and playback movie files

 Parameters:

 @param[in] i index of the media track if required.
 @param[in] wCommand Config type.
 @param[in] wOp      config value.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_SetMovieConfig(UINT16 i, AHC_MOVIE_CONFIG wCommand, UINT32 wOp)
{
    char *szErr;

    if (wCommand < AHC_VIDEO_CONFIG_MAX) {
        if (i >= AHC_MAX_VIDEO) {
            szErr = "video stream";
            goto config_error;
        }
    } 
    else if (wCommand < AHC_AUD_CONFIG_MAX){
        if (i >= AHC_MAX_AUDIO) {
            szErr = "audio stream";
            goto config_error;
        }
    }

    switch(wCommand){
    case AHC_MAX_PFRAME_NUM:
        m_sMovieConfig.v[i].nMaxP = wOp;
        break;
    case AHC_MAX_BFRAME_NUM:
        if (wOp != 0)
            m_sMovieConfig.v[i].nMaxB = 2;
        else
            m_sMovieConfig.v[i].nMaxB = 0;
        break;
    case AHC_MAX_BRC_QSCALE:
        m_sMovieConfig.v[i].nMaxQP = wOp;
        break;
    case AHC_VIDEO_RESOLUTION:
        m_sMovieConfig.v[i].Resolution = wOp;
        break;
    case AHC_FRAME_RATE:
    case AHC_FRAME_RATEx10:
        if (wCommand == AHC_FRAME_RATEx10) {
            wOp /= 10;
        }
        m_sMovieConfig.v[i].SampleRate = wOp;
        m_sMovieConfig.v[i].FPSx10 = wOp * 10;

        switch (wOp) {
		case 7:
			m_sMovieConfig.v[i].SampleRate = VIDRECD_FRAMERATE_7d5FPS;
            m_sMovieConfig.v[i].FPSx10 = 75;
		break;
		case 10:
			m_sMovieConfig.v[i].SampleRate = VIDRECD_FRAMERATE_10FPS;
		break;
		case 12:
			m_sMovieConfig.v[i].SampleRate = VIDRECD_FRAMERATE_12FPS;
		break;
		case 15:
			m_sMovieConfig.v[i].SampleRate = VIDRECD_FRAMERATE_15FPS;
		break;
		case 20:
			m_sMovieConfig.v[i].SampleRate = VIDRECD_FRAMERATE_20FPS;
		break;
		case 24:
			m_sMovieConfig.v[i].SampleRate = VIDRECD_FRAMERATE_24FPS;
		break;
		case 25:
			m_sMovieConfig.v[i].SampleRate = VIDRECD_FRAMERATE_25FPS;
		break;
		case 30:
			m_sMovieConfig.v[i].SampleRate = VIDRECD_FRAMERATE_30FPS;
		break;
		case 50:
			m_sMovieConfig.v[i].SampleRate = VIDRECD_FRAMERATE_50FPS;
		break;
		case 60:
			m_sMovieConfig.v[i].SampleRate = VIDRECD_FRAMERATE_60FPS;
		break;
		case 100:
			m_sMovieConfig.v[i].SampleRate = VIDRECD_FRAMERATE_100FPS;
		break;
		case 120:
			m_sMovieConfig.v[i].SampleRate = VIDRECD_FRAMERATE_120FPS;
		break;
        }
        break;
	case AHC_VIDEO_BITRATE:
        m_sMovieConfig.v[i].Bitrate = wOp;
        break;
	case AHC_VIDEO_CODEC_TYPE:
        switch (wOp) {
        case AHC_MOVIE_VIDEO_CODEC_H264:
            wOp = MMPS_3GPRECD_VIDEO_FORMAT_H264;
            break;
        case AHC_MOVIE_VIDEO_CODEC_MJPEG:
            wOp = MMPS_3GPRECD_VIDEO_FORMAT_MJPEG;
            break;
        case AHC_MOVIE_VIDEO_CODEC_YUV422:
            wOp = MMPS_3GPRECD_VIDEO_FORMAT_YUV422;
            break;
		case AHC_MOVIE_VIDEO_CODEC_NONE:
			wOp = MMPS_3GPRECD_VIDEO_FORMAT_OTHERS;
			break;
        default:
            return AHC_FALSE;
            break;
        }
        
        m_sMovieConfig.v[i].CodecType = wOp;
        break;
    case AHC_VIDEO_CODEC_TYPE_SETTING:
        switch (m_sMovieConfig.v[i].CodecType) {
            case MMPS_3GPRECD_VIDEO_FORMAT_H264:
                wOp = (MMPS_3GPRECD_PROFILE)wOp;
            case MMPS_3GPRECD_VIDEO_FORMAT_MJPEG:
                break;
            case MMPS_3GPRECD_VIDEO_FORMAT_YUV422:
                break;
            case MMPS_3GPRECD_VIDEO_FORMAT_YUV420:
                break;
            default:
                break;
        }

        m_sMovieConfig.v[i].sCodecTypeSetting = wOp;
        break;
    case AHC_VIDEO_ENCODE_MODE:
        m_sMovieConfig.v[i].sVidEncMode = (MMPS_3GPRECD_CURBUF_MODE)wOp;
        break;
    case AHC_VIDEO_COMPRESSION_RATIO:
        m_sMovieConfig.v[i].CompressRatio = wOp;
        break;
    case AHC_VIDEO_RESERVED_SIZE:
        m_sMovieConfig.v[i].ReservedSize = wOp;
        break; 
    case AHC_VIDEO_STREAMING:
        m_sMovieConfig.v[i].bStreaming = wOp;
        break;
    /* Audio Config */
    case AHC_AUD_MUTE_END:
        // Always ignore this value
        break;
    case AHC_AUD_SAMPLE_RATE:
        if ((wOp == 22050) || (wOp == 32000) || (wOp == 44100) || (wOp == 48000)) {
        	m_sMovieConfig.a[i].SampleRate = wOp;
        }	
        break;
    case AHC_AUD_CHANNEL_CONFIG:
        if (AHC_AUDIO_CHANNEL_STEREO == wOp) {
            m_sMovieConfig.a[i].ChannelType = MMPS_AUDIO_LINEIN_DUAL;
        }
        else if (AHC_AUDIO_CHANNEL_MONO_R == wOp) {
            m_sMovieConfig.a[i].ChannelType = MMPS_AUDIO_LINEIN_R;
        }
        else if (AHC_AUDIO_CHANNEL_MONO_L == wOp) {
            m_sMovieConfig.a[i].ChannelType = MMPS_AUDIO_LINEIN_L;
        }
        else {
            return AHC_FALSE;
        }
        break;
    case AHC_AUD_BITRATE:
        m_sMovieConfig.a[i].Bitrate = wOp;
        break;
    case AHC_AUD_CODEC_TYPE:
        if (AHC_MOVIE_AUDIO_CODEC_AAC == wOp) {
            m_sMovieConfig.a[i].CodecType = MMPS_3GPRECD_AUDIO_FORMAT_AAC;
        }
        else if (AHC_MOVIE_AUDIO_CODEC_ADPCM == wOp) {
            m_sMovieConfig.a[i].CodecType = MMPS_3GPRECD_AUDIO_FORMAT_ADPCM;
        }
        else if (AHC_MOVIE_AUDIO_CODEC_MP3 == wOp){
            m_sMovieConfig.a[i].CodecType = MMPS_3GPRECD_AUDIO_FORMAT_MP3;
        }
        else {
            return AHC_FALSE;
        }
        break;
    case AHC_AUD_PRERECORD_DAC:
        m_sMovieConfig.a[i].bPreRecordDAC = wOp;
        break;
    case AHC_AUD_STREAMING:
        m_sMovieConfig.a[i].bStreaming = wOp;
        break;
	/* Movie Config */
    case AHC_CLIP_CONTAINER_TYPE:
        if (AHC_MOVIE_CONTAINER_AVI == wOp) {
            m_sMovieConfig.ContainerType = MMPS_3GPRECD_CONTAINER_AVI;
        }
        else if (AHC_MOVIE_CONTAINER_3GP == wOp) {
            m_sMovieConfig.ContainerType = MMPS_3GPRECD_CONTAINER_3GP;
        }
        else {
            return AHC_FALSE;
        }
        break;
    case AHC_VIDEO_PRERECORD_LENGTH:
        m_sMovieConfig.PreRecordLength = wOp;
        break;
    case AHC_VIDEO_PRERECORD_STATUS:
        m_sMovieConfig.PreRecordStatus = wOp;
        break;
    default:
        szErr = "command";

config_error:
        printc(FG_RED("Invalid config %s.") "stream:%d command:%d\r\n", szErr, i, wCommand);
        return MMP_FALSE;
        break;
    }

    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_ConfigMovie
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief This API configures both record and playback movie files

 This API configures both record and playback movie files

 Parameters:

 @param[in] wCommand Config type.
 @param[in] wOp      config value.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_ConfigMovie(UINT32 wCommand, UINT32 wOp)
{
    return AHC_SetMovieConfig(0, wCommand, wOp);
}

//------------------------------------------------------------------------------
//  Function    : AHC_CaptureClipCmd
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief This API configures both record and playback movie files

 This API configures both record and playback movie files

 Parameters:

 @param[in] wCommand capture command.
 @param[in] wOp      command value.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_CaptureClipCmd(UINT16 wCommand, UINT16 wOp)
{
    AHC_BOOL ahc_ret = AHC_TRUE;
        
    switch(wCommand){

    case AHC_CAPTURE_CLIP_PAUSE:
        MMPS_3GPRECD_PauseRecord();
        break;
    case AHC_CAPTURE_CLIP_RESUME:
        MMPS_3GPRECD_ResumeRecord();
        break;
    case AHC_CAPTURE_SNAPSHOT:
    {
        MMP_ERR 	err;
        MMP_BYTE 	bJpgFileName[MAX_FILE_NAME_SIZE];
        MMP_BYTE 	DirName[16];
    	UINT16    	CaptureDirKey;
        UINT8     	CaptureFileName[32];
        UINT8    	bCreateNewDir;
        UINT32 		ThumbWidth, ThumbHeight;
        MMP_BOOL 	bFDEnable;
        AHC_BOOL 	bRet;
        UINT32     	movieSize;
        AHC_BOOL    bAddRearCamtoDB = AHC_FALSE;

        MMPS_3GPRECD_STILL_CAPTURE_INFO VideoCaptureInfo = {
            NULL,
            0,
    	    1920,       // Jpeg width
    	    1080,       // Jpeg height
    	    160,  		// Thumbnail width
    	    120,  		// Thumbnail height
    	    #if (HANDLE_JPEG_EVENT_BY_QUEUE)
    	    MMP_FALSE,  // Thumbnail Enable
    	    #else
    	    MMP_TRUE,   // Thumbnail Enable
    	    #endif
    	    MMP_TRUE,   // EXIF Enable
    	    MMP_FALSE,  // bTargetCtl
    	    MMP_FALSE,  // bLimitCtl
    	    200,		// bTargetSize
    	    220,		// bLimitSize
			1,			// bMaxTrialCnt
    	    MMP_DSC_CAPTURE_NORMAL_QUALITY,
    	    MMP_3GPRECD_CAPTURE_SRC_FRONT_CAM,
    	    NULL,
    	    0
    	};

        AHC_Menu_SettingGetCB((char *) COMMON_KEY_MOVIE_SIZE, &movieSize);

        switch(movieSize) {
		case MOVIE_SIZE_4K_15P:
        	VideoCaptureInfo.usWidth  = 3840;
            VideoCaptureInfo.usHeight = 2160;
    	break;
		case MOVIE_SIZE_2d7K_30P:
        	VideoCaptureInfo.usWidth  = 2704;
            VideoCaptureInfo.usHeight = 1524; //Not 8x
    	break;
        case MOVIE_SIZE_SHD_30P:
            VideoCaptureInfo.usWidth  = 2304;
            VideoCaptureInfo.usHeight = 1296;
        break;
        case MOVIE_SIZE_1080_30P:
		case MOVIE_SIZE_1080_50P:
        case MOVIE_SIZE_1080_60P:
        	VideoCaptureInfo.usWidth  = 1920;
            VideoCaptureInfo.usHeight = 1080;
            #if (HANDLE_JPEG_EVENT_BY_QUEUE) && (!USE_H264_CUR_BUF_AS_CAPT_BUF)//TBD
    	    VideoCaptureInfo.usWidth  = 1280;
    	    VideoCaptureInfo.usHeight = 720;
    	    #endif
    	break;
        case MOVIE_SIZE_720_30P:
        case MOVIE_SIZE_720_60P:
        case MOVIE_SIZE_720_100P:
        case MOVIE_SIZE_720_120P:
        	VideoCaptureInfo.usWidth  = 1280;
            VideoCaptureInfo.usHeight = 720;
    	break;
    	case MOVIE_SIZE_VGA_30P:
    	case MOVIE_SIZE_VGA_120P:
        	VideoCaptureInfo.usWidth  = 640;
            VideoCaptureInfo.usHeight = 480;
        break;
        default:
            printc(FG_RED("%s,%d - Wrong Movie Size\r\n"), __func__, __LINE__);
        case MOVIE_SIZE_360_30P:
            VideoCaptureInfo.usWidth  = 640;
            VideoCaptureInfo.usHeight = 360;
        break;
        }

		#if defined(CAR_DV) || defined(C005_PHOTO_EXIF_GPS_COORDINATE)

        if (MMP_TRUE == VideoCaptureInfo.bExifEn) {
            
            AHC_ConfigEXIF_RTC();
            AHC_ConfigEXIF_SystemInfo();
            AHC_ConfigEXIF_MENU();
        }
		#endif

        bFDEnable = MMPS_Sensor_GetFDEnable();

        if (bFDEnable) {
        	MMPS_Sensor_SetFDEnable(MMP_FALSE);
		}

        AHC_GetCaptureConfig(ACC_CUS_THUMB_W, &ThumbWidth);
        AHC_GetCaptureConfig(ACC_CUS_THUMB_H, &ThumbHeight);
        
        VideoCaptureInfo.usThumbWidth  = ThumbWidth;
        VideoCaptureInfo.usThumbHeight = ThumbHeight;

    	MEMSET(bJpgFileName    , 0, sizeof(bJpgFileName));
        MEMSET(DirName         , 0, sizeof(DirName));
        MEMSET(CaptureFileName , 0, sizeof(CaptureFileName));

		#ifdef CAR_DV
        AHC_UF_SetFreeChar(0, DCF_SET_FREECHAR, (UINT8*)SNAP_DEFAULT_FILE_FREECHAR);
		#else
        AHC_UF_SetFreeChar(0, DCF_SET_FREECHAR, (UINT8*)DSC_DEFAULT_FLIE_FREECHAR);
		#endif

	#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_NORMAL)
        bRet = AHC_UF_GetName(&CaptureDirKey, (INT8*)DirName, (INT8*)CaptureFileName, &bCreateNewDir);
    	
    	if (bRet == AHC_TRUE) {
            printc("CaptureDirKey=%d  CaptureFileName=%s\r\n", CaptureDirKey, CaptureFileName);

    	    STRCPY(bJpgFileName, (MMP_BYTE*)AHC_UF_GetRootName());
            STRCAT(bJpgFileName, "\\");
            STRCAT(bJpgFileName, DirName);

            if (bCreateNewDir) {
                MMPS_FS_DirCreate((INT8*)bJpgFileName, STRLEN(bJpgFileName));
                AHC_UF_AddDir(DirName);
            }

            STRCAT(bJpgFileName, "\\");
            STRCAT(bJpgFileName, (INT8*)CaptureFileName);

            STRCAT(bJpgFileName, EXT_DOT);
            STRCAT(bJpgFileName, PHOTO_JPG_EXT);
            
            VideoCaptureInfo.bFileName = bJpgFileName;
            VideoCaptureInfo.ulFileNameLen = STRLEN(bJpgFileName);
            
            err = MMPS_3GPRECD_StillCapture(&VideoCaptureInfo);

            if (MMP_ERR_NONE == err)
            {
        		STRCAT((INT8*)CaptureFileName, EXT_DOT);
                STRCAT((INT8*)CaptureFileName, PHOTO_JPG_EXT);

            	AHC_UF_PreAddFile(CaptureDirKey,(INT8*)CaptureFileName);
            	AHC_UF_PostAddFile(CaptureDirKey,(INT8*)CaptureFileName);
        	}
            else {
                printc("-E- MMPS_3GPRECD_StillCapture return 0x%X\r\n", __func__);
            }
        }

	#elif (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
        {
            AHC_RTC_TIME 	RtcTime;
            USB_DETECT_PHASE USBCurrentStates = 0;
            MMP_BYTE 	        bRearCamJpgFileName[MAX_FILE_NAME_SIZE];
            UINT8               bRearCamCaptureFileName[32];
            AHC_RTC_GetTime(&RtcTime);
            

            bRet = AHC_UF_GetName2(&RtcTime, (INT8*)bJpgFileName, (INT8*)CaptureFileName, &bCreateNewDir);
            if(bRet != AHC_TRUE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,bRet); return AHC_FALSE;}             



                STRCAT(bJpgFileName, EXT_DOT);
                STRCAT(bJpgFileName, PHOTO_JPG_EXT);
                STRCAT((INT8*)CaptureFileName, EXT_DOT);
                STRCAT((INT8*)CaptureFileName, PHOTO_JPG_EXT);
            printc("%s,%d,",__func__,__LINE__);
            printc("bJpgFileName: %s\r\n",bJpgFileName);
            
            VideoCaptureInfo.sCaptSrc = 0;
            VideoCaptureInfo.bFileName = bJpgFileName;
            VideoCaptureInfo.ulFileNameLen = STRLEN(bJpgFileName);
            VideoCaptureInfo.sCaptSrc |= MMP_3GPRECD_CAPTURE_SRC_FRONT_CAM;
            
            
#if (REAR_CAM_TYPE == REAR_CAM_TYPE_SONIX_MJPEG2H264) || (REAR_CAM_TYPE == REAR_CAM_TYPE_SONIX_MJPEG) || (REAR_CAM_TYPE == REAR_CAM_TYPE_TV_DECODER)
            //AHC_USBGetStates(&USBCurrentStates,AHC_USB_GET_PHASE_CURRENT);
#if (REAR_CAM_TYPE == REAR_CAM_TYPE_SONIX_MJPEG2H264) || (REAR_CAM_TYPE == REAR_CAM_TYPE_SONIX_MJPEG)
			AHC_USBGetStates(&USBCurrentStates,AHC_USB_GET_PHASE_CURRENT);
            if ((USBCurrentStates == USB_DETECT_PHASE_REAR_CAM) && 
            (AHC_TRUE == AHC_HostUVCVideoIsEnabled())){
                VideoCaptureInfo.sCaptSrc |= MMP_3GPRECD_CAPTURE_SRC_REAR_CAM;
                bAddRearCamtoDB = AHC_TRUE;
            }
            else{
                bAddRearCamtoDB = AHC_FALSE;
            }
#elif (REAR_CAM_TYPE == REAR_CAM_TYPE_TV_DECODER)
			if(AHC_TRUE == AHC_GetSecondSensorCnntStatus())//bit1603 must be connect
			{
            	bAddRearCamtoDB = AHC_TRUE;
            	VideoCaptureInfo.sCaptSrc |= MMP_3GPRECD_CAPTURE_SRC_REAR_CAM;  
			}
			else
			{
				bAddRearCamtoDB = AHC_FALSE;
			}
#endif
            if(bAddRearCamtoDB)
            {
            AHC_UF_SetRearCamPathFlag(AHC_TRUE);
            ahc_ret = AHC_UF_GetName2(&RtcTime, (INT8*)bRearCamJpgFileName, (INT8*)bRearCamCaptureFileName, &bCreateNewDir);            
            if(ahc_ret != AHC_TRUE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,ahc_ret); /*return AHC_FALSE;*/} 
            AHC_UF_SetRearCamPathFlag(AHC_FALSE);

            STRCAT(bRearCamJpgFileName, EXT_DOT);
            STRCAT(bRearCamJpgFileName, PHOTO_JPG_EXT);
            STRCAT((INT8*)bRearCamCaptureFileName, EXT_DOT);
            STRCAT((INT8*)bRearCamCaptureFileName, PHOTO_JPG_EXT);
            //printc("%s,%d,bRearCamJpgFileName: %s ,\r\n",__func__,__LINE__,bRearCamJpgFileName);
            printc("%s,%d,",__func__,__LINE__);
            printc("bRearCamJpgFileName: %s\r\n",bRearCamJpgFileName);

            VideoCaptureInfo.bRearFileName      = bRearCamJpgFileName;
            VideoCaptureInfo.ulRearFileNameLen  = STRLEN(bRearCamJpgFileName);

            }
#endif
            if(bAddRearCamtoDB == AHC_TRUE)       
            {
                AHC_UF_SetRearCamPathFlag(AHC_TRUE);
                    
            }
            ahc_ret = AHC_UF_PreAddFile(CaptureDirKey, (INT8*)CaptureFileName); // flow for delete the same file name.
                if(ahc_ret != AHC_TRUE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,ahc_ret); /*return AHC_FALSE;*/} 
            AHC_UF_SetRearCamPathFlag(AHC_FALSE);

            #if (AHC_ENABLE_VIDEO_STICKER)
            {
                UINT32  uiTimeStampOp;
        
                AHC_GetCaptureConfig(ACC_DATE_STAMP, &uiTimeStampOp);
        
                if (uiTimeStampOp & AHC_ACC_TIMESTAMP_ENABLE_MASK)
                {
                    AHC_RTC_TIME sRtcTime;
                    
                    AHC_RTC_GetTime(&sRtcTime);
                    AIHC_ConfigVRTimeStamp(uiTimeStampOp, &sRtcTime, MMP_TRUE);
                }
            }
            #endif
            err = MMPS_3GPRECD_StillCapture(&VideoCaptureInfo);

            if (MMP_ERR_NONE == err)
            {
                ahc_ret = AHC_UF_PostAddFile(CaptureDirKey,(INT8*)CaptureFileName);
                if(ahc_ret != AHC_TRUE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,ahc_ret); /*return AHC_FALSE;*/} 
                printc("CaptureFileName=%s\n", CaptureFileName);
                
            }
            else {

                printc("-E- MMPS_3GPRECD_StillCapture %s line %d return 0x%X\r\n", __func__, __LINE__, err);
            }
        }
	#endif

        AHC_UF_SetFreeChar(0, DCF_SET_FREECHAR, (UINT8*)VIDEO_DEFAULT_FLIE_FREECHAR);

        if(bFDEnable) {
	       	MMPS_Sensor_SetFDEnable(MMP_TRUE);
    	}
    }
        break;
    default:
        break;
    }

	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetCurrentRecordingTime
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_GetCurrentRecordingTime(UINT32 *ulTime)
{
    if (!MMPS_3GPRECD_GetRecordTime(ulTime))
	    return AHC_FALSE;
	else
	    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetPreRecordStatus
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_GetPreRecordStatus(UINT32 *ulStatus)
{
	AIHC_GetMovieConfig(AHC_VIDEO_PRERECORD_STATUS, ulStatus);
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetPreRecordDACStatus
//  Description : 
//------------------------------------------------------------------------------
AHC_BOOL AHC_GetPreRecordDACStatus(UINT32 *ulStatus)
{
	AIHC_GetMovieConfig(AHC_AUD_PRERECORD_DAC, ulStatus);
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetVideoBitrate
//  Description : 
//------------------------------------------------------------------------------
UINT32 AHC_GetVideoBitrate(int type)
{
	UINT32 ulResIdx = 0;
	UINT32 ulFps = 30;

	switch (type) {
	#if(MENU_MOVIE_SIZE_1440_30P_EN)
    case MOVIE_SIZE_1440_30P:
    	ulResIdx	= VIDRECD_RESOL_2560x1440;
    	ulFps		= 30;
    	break;
	#endif
    #if(MENU_MOVIE_SIZE_SHD_30P_EN)
    case MOVIE_SIZE_SHD_30P:
    	ulResIdx	= VIDRECD_RESOL_2304x1296;
    	ulFps		= 30;
    	break;
	#endif
	#if(MENU_MOVIE_SIZE_SHD_25P_EN)
    case MOVIE_SIZE_SHD_25P:
    	ulResIdx	= VIDRECD_RESOL_2304x1296;
    	ulFps		= 25;
    	break;
	#endif
    #if(MENU_MOVIE_SIZE_SHD_24P_EN)
    case MOVIE_SIZE_SHD_24P:
    	ulResIdx	= VIDRECD_RESOL_2304x1296;
    	ulFps		= 24;
    	break;
	#endif

    #if (MENU_MOVIE_SIZE_1080_60P_EN)
    case MOVIE_SIZE_1080_60P:
    	ulResIdx 	= VIDRECD_RESOL_1920x1088;
    	ulFps 		= 60;
    	break;
	#endif
    #if (MENU_MOVIE_SIZE_1080_24P_EN)
    case MOVIE_SIZE_1080_24P:
        ulResIdx    = VIDRECD_RESOL_1920x1088;
        ulFps       = 24;
        break;
    #endif
	#if (MENU_MOVIE_SIZE_1080P_EN)
    case MOVIE_SIZE_1080P:
    	ulResIdx 	= VIDRECD_RESOL_1920x1088;
    	ulFps 		= 30;
    	break;
    #endif
    #if (MENU_MOVIE_SIZE_720_60P_EN)
    case MOVIE_SIZE_720_60P:
		ulResIdx 	= VIDRECD_RESOL_1280x720;
    	ulFps 		= 60;
		break;
	#endif
    #if (MENU_MOVIE_SIZE_720_24P_EN)
    case MOVIE_SIZE_720_24P:
        ulResIdx    = VIDRECD_RESOL_1280x720;
        ulFps       = 24;
        break;
    #endif
	#if(MENU_MOVIE_SIZE_720P_EN)
	case MOVIE_SIZE_720P:
		ulResIdx 	= VIDRECD_RESOL_1280x720;
    	ulFps 		= 30;
		break;
	#endif
	#if(MENU_MOVIE_SIZE_VGA30P_EN)
	case MOVIE_SIZE_VGA_30P:
		ulResIdx 	= VIDRECD_RESOL_640x480;
    	ulFps 		= 30;
		break;
	#endif
    case MOVIE_SIZE_360_30P:
        ulResIdx 	= VIDRECD_RESOL_640x368;
     	ulFps 		= 30;
        break;
	default:
    	ulResIdx 	= VIDRECD_RESOL_1920x1088;
     	ulFps 		= 30;
		printc("ERROR: movie size\r\n");
		break;
	}
	
	if (ulFps == 60) {
		return MMPS_3GPRECD_GetConfig()->ulFps60BitrateMap[ulResIdx][MenuSettingConfig()->uiMOVQuality];
	}
	else {
		return MMPS_3GPRECD_GetConfig()->ulFps30BitrateMap[ulResIdx][MenuSettingConfig()->uiMOVQuality];
	}
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetVideoRealFrameRate
//  Description : 
//------------------------------------------------------------------------------
UINT32 AHC_GetVideoRealFrameRate(UINT32 FrateIdx)
{
    UINT32 RealFrameRate;

    switch(FrateIdx)
    {
    	case VIDRECD_FRAMERATE_7d5FPS:
            RealFrameRate = 7500;
            break;
        case VIDRECD_FRAMERATE_10FPS:
            RealFrameRate = 10000;
            break;
		case VIDRECD_FRAMERATE_12FPS:
            RealFrameRate = 12000;
            break;
        case VIDRECD_FRAMERATE_15FPS:
            RealFrameRate = 15000;
            break;
		case VIDRECD_FRAMERATE_20FPS:
            RealFrameRate = 20000;
            break;
		case VIDRECD_FRAMERATE_24FPS:
            RealFrameRate = 24000;
            break;
	case VIDRECD_FRAMERATE_25FPS:
            RealFrameRate = 25000;
            break;
		case VIDRECD_FRAMERATE_30FPS:
            RealFrameRate = 30000;
            break;
    	case VIDRECD_FRAMERATE_50FPS:
            RealFrameRate = 50000;
            break;
        case VIDRECD_FRAMERATE_60FPS:
            RealFrameRate = 60000;
            break;
        case VIDRECD_FRAMERATE_100FPS:
            RealFrameRate = 100000;
            break;
		case VIDRECD_FRAMERATE_120FPS:
            RealFrameRate = 120000;
            break;
        default:
            RealFrameRate = 30000;//TBD
            break;
    }

    return RealFrameRate;
}

//------------------------------------------------------------------------------
//  Function    : AHC_WaitVideoWriteFileFinish
//  Description : 
//------------------------------------------------------------------------------
AHC_BOOL AHC_WaitVideoWriteFileFinish(void)
{
	UINT32 				ulTimeOut = 20;//450;		// unit is 100ms
	MMPD_MP4VENC_FW_OP 	status_vid;

	do {
		if (0 == ulTimeOut) {
			printc(FG_RED("--E-- Waiting for video file write to SD - Timeout !!!\r\n"));
			break;
		}

		ulTimeOut--;

		MMPD_VIDENC_GetStatus(0/*ulEncId*/, &status_vid);
		AHC_OS_SleepMs(100);

	} while ((status_vid != MMPD_MP4VENC_FW_OP_NONE) && (status_vid != MMPD_MP4VENC_FW_OP_STOP) && (status_vid != MMPD_MP4VENC_FW_OP_PREENCODE));

	AHC_OS_SleepMs(100);

	return (ulTimeOut ? AHC_TRUE : AHC_FALSE);
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetVideoFileName
//  Description : This function is used to set file name before video recording
//------------------------------------------------------------------------------
MMP_ERR AHC_SetVideoFileName( MMP_BYTE byFilename[], UINT16 usLength, MMP_3GPRECD_FILETYPE eFileType )
{
    #if ( FS_FORMAT_FREE_ENABLE )
    char byOldFilename[MAX_FILE_NAME_SIZE] = {0};
    
    MMPS_FORMATFREE_EnableWrite( 1 );
    
    AHC_UF_SearchAvailableFileSlot( byOldFilename );
    printc("--Old File-- %s\r\n", byOldFilename);
    
    MMPS_FS_FileDirRename( (MMP_BYTE*)byOldFilename, strlen(byOldFilename), byFilename, strlen(byFilename) );

    // Set Non-hidden attribute to let recorded file can be seen.
    {
        AHC_FS_ATTRIBUTE sAttrib;
        AHC_FS_FileDirGetAttribute( byFilename, strlen(byFilename), &sAttrib );
        if( sAttrib & AHC_FS_ATTR_HIDDEN )
        {
            sAttrib &= ~AHC_FS_ATTR_HIDDEN;
            AHC_FS_FileDirSetAttribute( byFilename, strlen(byFilename), sAttrib );
        }
    }
    
    #endif

    if( eFileType == MMP_3GPRECD_FILETYPE_EMERGENCY )
    {
        MMPS_3GPRECD_SetEmergFileName( byFilename, usLength );
    }
    else
    {
        MMPS_3GPRECD_SetFileName( byFilename, usLength, eFileType );
    }

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetFaceDetection_Video
//  Description : 
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetFaceDetection_Video(UINT8 bMode)
{
#if (SUPPORT_FDTC)
	if (bMode == 0) {
        MMPS_3GPRECD_StartFDTC(AHC_TRUE);
    }
    else if (bMode == 2){
        MMPS_3GPRECD_StartFDTC(AHC_FALSE);
    }
#endif

	return AHC_SUCCESS;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetVideoCurZoomStep
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_GetVideoCurZoomStep(UINT16 *usZoomStepNum)
{
    MMP_IBC_PIPEID ubPipe;

    MMPS_3GPRECD_GetPreviewPipe(gsAhcCurrentSensor, &ubPipe);

    if (!MMPS_3GPRECD_GetCurZoomStep(ubPipe, usZoomStepNum))
	    return AHC_FALSE;
	else
	    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetVideoMaxZoomStep
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_GetVideoMaxZoomStep(UINT16 *usMaxZoomStep)
{
    *usMaxZoomStep = gsVidPtzCfg.usMaxZoomSteps;

	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetVideoMaxZoomRatio
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_GetVideoMaxZoomRatio(UINT16 *usMaxZoomRatio)
{
     *usMaxZoomRatio = gsVidPtzCfg.usMaxZoomRatio;

     return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetVideoCurZoomStatus
//  Description :
//------------------------------------------------------------------------------
MMP_UBYTE AHC_GetVideoCurZoomStatus(void)
{
	return MMPS_3GPRECD_GetCurZoomStatus();
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetVideoDigitalZoomRatio
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_GetVideoDigitalZoomRatio(UINT32 *usZoomRatio)
{
    UINT16 usMaxZoomRatio, usCurrentStep, usMaxZoomStep;

    AHC_GetVideoMaxZoomRatio(&usMaxZoomRatio);

    AHC_GetVideoCurZoomStep(&usCurrentStep);
    AHC_GetVideoMaxZoomStep(&usMaxZoomStep);

    *usZoomRatio = ((usMaxZoomRatio > 1) && (usMaxZoomStep > 0)) ? (100 + (usCurrentStep * (usMaxZoomRatio - 1) * 100) / usMaxZoomStep) : 100;
    return AHC_TRUE;
}

#if defined(CAR_DV)
//------------------------------------------------------------------------------
//  Function    : AHC_GetVideoRecTimeLimit
//  Description :
//------------------------------------------------------------------------------
UINT32 AHC_GetVideoRecTimeLimit(void)
{
    #define SEC_PER_MIN  (60)

    UINT32	m_VRTimeLimit = 0;  // Unit:seconds
    UINT32 clipTime;

    if (AHC_Menu_SettingGetCB((char*)COMMON_KEY_VR_CLIP_TIME, &clipTime) == AHC_FALSE) {
        RTNA_DBG_Str(0, FG_RED("Get COMMON_KEY_VR_CLIP_TIME error - using default\r\n"));
        clipTime = MOVIE_CLIP_TIME_OFF;
    }
	printc("clipTime %d\r\n",clipTime);
    switch (clipTime)
    {
    case COMMON_MOVIE_CLIP_TIME_6SEC:
        m_VRTimeLimit = 6;
        break;
    case COMMON_MOVIE_CLIP_TIME_1MIN:
        m_VRTimeLimit = 1*SEC_PER_MIN;
        break;
    case COMMON_MOVIE_CLIP_TIME_2MIN:
        m_VRTimeLimit = 2*SEC_PER_MIN;
        break;
    case COMMON_MOVIE_CLIP_TIME_3MIN:
        m_VRTimeLimit = 3*SEC_PER_MIN;
        break;
    case COMMON_MOVIE_CLIP_TIME_5MIN:
        m_VRTimeLimit = 5*SEC_PER_MIN;
        break;
    case COMMON_MOVIE_CLIP_TIME_10MIN:
        m_VRTimeLimit = 10*SEC_PER_MIN;
        break;
    case COMMON_MOVIE_CLIP_TIME_25MIN:
        m_VRTimeLimit = 25*SEC_PER_MIN;
        break;
    case COMMON_MOVIE_CLIP_TIME_30MIN:
        m_VRTimeLimit = 30*SEC_PER_MIN;
        break;
    case COMMON_MOVIE_CLIP_TIME_OFF:
    default:
        m_VRTimeLimit = NON_CYCLING_TIME_LIMIT;
        break;
    }

#if (MOTION_DETECTION_EN)
    #if (SET_MOTION_DETECTION_MOVIE_CLIP_TIME)
    {
        extern AHC_BOOL m_ubMotionDtcEn;

		if (m_ubMotionDtcEn)
        {
			m_VRTimeLimit = MOTION_DETECTION_MOVIE_CLIP_TIME;
		}
	}
	#endif
#endif

#ifdef VIDEO_REC_TIMELAPSE_EN
    if (VIDEO_REC_TIMELAPSE_EN)
    {
        UINT32 ulTimeLapseSetting;
        UINT32 Frate;
        UINT64 ullFramerate_1000Times;
        
        AIHC_GetMovieConfig(AHC_FRAME_RATE, &Frate);
        ullFramerate_1000Times = (AHC_GetVideoRealFrameRate(Frate)*1000) / AHC_VIDRECD_TIME_INCREMENT_BASE;
        
        if ((AHC_Menu_SettingGetCB(COMMON_KEY_VR_TIMELAPSE, &ulTimeLapseSetting) == AHC_TRUE) && (ulTimeLapseSetting != COMMON_VR_TIMELAPSE_OFF)
				&&(m_VRTimeLimit != NON_CYCLING_TIME_LIMIT)) {
            switch(ulTimeLapseSetting){
                case COMMON_VR_TIMELAPSE_5SEC:
                    m_VRTimeLimit = (UINT32)(((UINT64)m_VRTimeLimit * ullFramerate_1000Times * 5)/1000);
                    break;
                case COMMON_VR_TIMELAPSE_10SEC:
                    m_VRTimeLimit = (UINT32)(((UINT64)m_VRTimeLimit * ullFramerate_1000Times * 10)/1000);
                    break;
                case COMMON_VR_TIMELAPSE_30SEC:
                    m_VRTimeLimit = (UINT32)(((UINT64)m_VRTimeLimit * ullFramerate_1000Times * 30)/1000);                    
                    break;
                case COMMON_VR_TIMELAPSE_60SEC:
                    m_VRTimeLimit = (UINT32)(((UINT64)m_VRTimeLimit * ullFramerate_1000Times * 60)/1000);                    
                    break;
                case COMMON_VR_TIMELAPSE_1SEC:
                default:
                    m_VRTimeLimit = (UINT32)(((UINT64)m_VRTimeLimit * ullFramerate_1000Times * 1)/1000);                    
                    break;
            }
        }
    }
#endif

    return m_VRTimeLimit;
}

//------------------------------------------------------------------------------
//  Function    : AHC_Delete_DcfMinKeyFile
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_Delete_DcfMinKeyFile(AHC_BOOL bFirstTime, const char *ext)
{
	/*
	 * FIXME: DON'T set CHECK_CROSS_LINK be 1, there is a BUG!!
	 *		  It is wrong in check free space before and after file deleted when
	 *		  that file is last one (index is 9999) in folder. The folder will be
	 *		  delete too, free space will be release more then only file deleted
	 *		  It makes fault of cross link in checking!!
	 * For CarDV never stop recording even something wrong, but popup a warning
	 * message box until user checked.
	 */
	#define CHECK_CROSS_LINK	(0)

 	AHC_BOOL			ahc_ret 		= AHC_TRUE;
   	UINT32				MaxObjIdx		= 0;
    UINT32      		CurObjIdx		= 0;
    UINT32              uiFileSize;
    AHC_BOOL            bReadOnly;
    AHC_BOOL            bCharLock;
    UINT64      		ulFreeSpace 	= 0;
	UINT32 				ulAudBitRate 	= 128000;//128K
    UINT32      		ulVidBitRate	= 0;
	UINT32				ulTimelimit		= 0;
	UINT64      		ulSpaceNeeded 	= 0;
	UINT32				InitObjIdx 		= 0;
#if (CHECK_CROSS_LINK)
	UINT64				ulFreeSpace1	= 0;
	UINT32				ulCluseterSize	= 0;
	AHC_FS_DISK_INFO 	volInfo;

	AHC_FS_GetVolumeInfo("SD:\\", strlen("SD:\\"), &volInfo);

	ulCluseterSize = (volInfo.usBytesPerSector) * (volInfo.usSecPerCluster);
#endif

	AHC_UF_SetFreeChar(0, DCF_SET_ALLOWED, (UINT8 *) ext);

	AIHC_GetMovieConfig(AHC_VIDEO_BITRATE, &ulVidBitRate);

	ulTimelimit = AHC_GetVideoRecTimeLimit();

	if (ulTimelimit == NON_CYCLING_TIME_LIMIT)
	{
		ulSpaceNeeded = 0x24000000;/* 576MB */
	}
	else
	{

#if (defined(VIDEO_REC_TIMELAPSE_EN) && VIDEO_REC_TIMELAPSE_EN)
        UINT32 slVRTimelapse = 0;		// Off

        if ((AHC_Menu_SettingGetCB(COMMON_KEY_VR_TIMELAPSE, &slVRTimelapse) == AHC_TRUE) && (slVRTimelapse != COMMON_VR_TIMELAPSE_OFF)){
            UINT32      Framerate, Frate;

            AIHC_GetMovieConfig(AHC_FRAME_RATE, &Frate);
            Framerate = AHC_GetVideoRealFrameRate(Frate) / AHC_VIDRECD_TIME_INCREMENT_BASE;
            VideoFunc_GetTimeLapseBitrate(Framerate, slVRTimelapse, &ulVidBitRate, &ulAudBitRate);
        }
        else
#endif    
        {
            if (MenuSettingConfig()->uiMOVSoundRecord == MOVIE_SOUND_RECORD_OFF) {
                ulAudBitRate = 0;
            }
        }

        AHC_GetVideoRecStorageSpeceNeed(ulVidBitRate, ulAudBitRate, ulTimelimit, &ulSpaceNeeded);

		if (bFirstTime) {
			ulSpaceNeeded *= 2;
		}
	}

	#if (!CHECK_CROSS_LINK)
	{
        extern void VideoFunc_GetFreeSpace(UINT64 *pFreeBytes);
        VideoFunc_GetFreeSpace(&ulFreeSpace);
    }
	#endif

	do {
		#if (CHECK_CROSS_LINK)
		VideoFunc_GetFreeSpace(&ulFreeSpace);
		#endif

		if (ulFreeSpace >= ulSpaceNeeded)
		{
			printc(FG_BLUE("FreeSpace is Enough [SpaceNeeded %dKB]\r\n"),(ulSpaceNeeded>>10));
			return AHC_TRUE;
		}

	   	AHC_UF_GetTotalFileCount(&MaxObjIdx);

		if (MaxObjIdx == 0)
		{
			printc("No More DCF File for Delete!\r\n");
			return AHC_FALSE;
		}

		if (InitObjIdx >= MaxObjIdx)
		{
			printc("All %d File Can't Delete!!\r\n",MaxObjIdx);
			return AHC_FALSE;
		}

		AHC_UF_SetCurrentIndex(InitObjIdx);
		AHC_UF_GetCurrentIndex(&CurObjIdx);

        if (AHC_UF_GetFileSizebyIndex(CurObjIdx, &uiFileSize) == AHC_FALSE)
        {
		    printc("AHC_UF_GetFileSizebyIndex Error\r\n");
            return AHC_FALSE;
        }
        
        AHC_UF_IsReadOnlybyIndex(CurObjIdx, &bReadOnly);
        AHC_UF_IsCharLockbyIndex(CurObjIdx, &bCharLock);

		if (bReadOnly
		#if	(PROTECT_FILE_TYPE == PROTECT_FILE_RENAME)
			|| bCharLock   //Protect file
		#endif
			  )// Read Only File
		{
			InitObjIdx++;
			continue;
		}

        ahc_ret = AHC_UF_FileOperation_ByIdx(CurObjIdx, DCF_FILE_DELETE, NULL, NULL);
        
	    if (ahc_ret == AHC_FALSE)
	    {
	    	printc(FG_RED("AHC_DCF_FileOperation Delete Error\r\n"));
	    	return AHC_FALSE;
	    }
	    else
	    {
	    	#if (CHECK_CROSS_LINK)
			VideoFunc_GetFreeSpace(&ulFreeSpace1);

			if (((ulFreeSpace + uiFileSize - ulCluseterSize) < ulFreeSpace1) &&
				((ulFreeSpace + uiFileSize + ulCluseterSize) > ulFreeSpace1))
			{
				//Check Pass
			}
			else
			{
				printc("FS Cross Link!!!!!\r\n");
				AHC_WMSG_Draw(AHC_TRUE, WMSG_FORMAT_SD_CARD, 3);
				return AHC_FALSE;
			}
			#else
	    	ulFreeSpace += uiFileSize;
	    	#endif
	    }
    } while(1);

	return ahc_ret;
}
#else
//------------------------------------------------------------------------------
//  Function    : AHC_GetVideoRecTimeLimit
//  Description :
//------------------------------------------------------------------------------
UINT32 AHC_GetVideoRecTimeLimit(void)
{
    #define SEC_PER_MIN   (60)

    UINT32	m_VRTimeLimit;    //Unit:seconds

	switch(MenuSettingConfig()->uiMOVClipTime)
    {
    	case MOVIE_CLIP_TIME_OFF:
	        m_VRTimeLimit = NON_CYCLING_TIME_LIMIT;
	        break;
	    case MOVIE_CLIP_TIME_6SEC:
	        m_VRTimeLimit = 6;
	        break;    
	    case MOVIE_CLIP_TIME_1MIN:
	        m_VRTimeLimit = 1*SEC_PER_MIN;
	        break;    
	    case MOVIE_CLIP_TIME_3MIN:
	        m_VRTimeLimit = 3*SEC_PER_MIN;
	        break;
	    case MOVIE_CLIP_TIME_5MIN:
	        m_VRTimeLimit = 5*SEC_PER_MIN;
	        break;
	    case MOVIE_CLIP_TIME_10MIN:
	        m_VRTimeLimit = 10*SEC_PER_MIN;
	        break;
	    case MOVIE_CLIP_TIME_25MIN:
	        m_VRTimeLimit = 25*SEC_PER_MIN;
	        break;
	    case MOVIE_CLIP_TIME_30MIN:
	        m_VRTimeLimit = 30*SEC_PER_MIN;
	        break;
	    default:
	    	// Out of index range, by value to do
			m_VRTimeLimit = 100;
	        break;    
    }

    return m_VRTimeLimit;
}
#endif

//------------------------------------------------------------------------------
//  Function    : AHC_GetVideoRecStorageSpeceNeed
//  Description :
//------------------------------------------------------------------------------
void AHC_GetVideoRecStorageSpeceNeed(UINT32 ulVidBitRate, UINT32 ulAudBitRate, UINT32 ulTimelimit, UINT64 *pulSpaceNeeded) 
{
    UINT64 ulSpaceNeeded = 0;

	ulSpaceNeeded = ((((UINT64)ulVidBitRate + (UINT64)ulAudBitRate) >> 3) * (UINT64)ulTimelimit >> 3) * 9; // More 1/8 bitstream size

    // Modify TV in case media full issue. Need check free space with TV in record bitstream.
	// Suggest to check bitrate of video if need more correct size. (ex: TV in bitrate is 3Mb)
    #if (UVC_HOST_VIDEO_ENABLE) || (REAR_CAM_TYPE == REAR_CAM_TYPE_TV_DECODER)
    {
        MMP_BOOL bStatus = MMP_TRUE;
		#if (REAR_CAM_TYPE != REAR_CAM_TYPE_TV_DECODER)
        MMPF_USBH_GetUVCPrevwSts(&bStatus);
		#endif
		// For Rear Cam
        if (MMP_TRUE == bStatus) {
            if (ulSpaceNeeded <= (SIGNLE_FILE_SIZE_LIMIT_3_75G / 2))
                ulSpaceNeeded <<= 1;
            else
                ulSpaceNeeded = SIGNLE_FILE_SIZE_LIMIT_3_75G;
        }
    }
	#endif
	
    if (ulSpaceNeeded > SIGNLE_FILE_SIZE_LIMIT_3_75G) {
        ulSpaceNeeded = SIGNLE_FILE_SIZE_LIMIT_3_75G;
	}

    if (pulSpaceNeeded) {
        *pulSpaceNeeded = ulSpaceNeeded;
	}

    printc("Free space %dMB is need\r\n", ulSpaceNeeded >> 20);
}

//------------------------------------------------------------------------------
//  Function    : AHC_VideoSetSlowMotionFPS
//  Description :
//------------------------------------------------------------------------------
void AHC_VideoSetSlowMotionFPS(AHC_BOOL bEnable, UINT32 usTimeIncrement, UINT32 usTimeIncrResol)
{
    gbSlowMotionEnable = bEnable;

    gsSlowMotionContainerFps.usVopTimeIncrement = usTimeIncrement;
    gsSlowMotionContainerFps.usVopTimeIncrResol = usTimeIncrResol;
}

//------------------------------------------------------------------------------
//  Function    : AHC_VideoGetSlowMotionFPS
//  Description :
//------------------------------------------------------------------------------
void AHC_VideoGetSlowMotionFPS(AHC_BOOL *pbEnable, UINT32 *pusTimeIncrement, UINT32 *pusTimeIncrResol)
{
    *pbEnable = gbSlowMotionEnable;

    *pusTimeIncrement = gsSlowMotionContainerFps.usVopTimeIncrement;
    *pusTimeIncrResol = gsSlowMotionContainerFps.usVopTimeIncrResol;
}

#if (defined(VIDEO_REC_TIMELAPSE_EN) && VIDEO_REC_TIMELAPSE_EN == 1)
//------------------------------------------------------------------------------
//  Function    : AHC_VideoSetTimeLapseFPS
//  Description :
//------------------------------------------------------------------------------
void AHC_VideoSetTimeLapseFPS(AHC_BOOL bEnable, UINT32 usTimeIncrement, UINT32 usTimeIncrResol)
{
    gbTimeLapseEnable = bEnable;

    gsTimeLapseFps.usVopTimeIncrement = usTimeIncrement;
    gsTimeLapseFps.usVopTimeIncrResol = usTimeIncrResol;
}

//------------------------------------------------------------------------------
//  Function    : AHC_VideoGetTimeLapseFPS
//  Description :
//------------------------------------------------------------------------------
void AHC_VideoGetTimeLapseFPS(AHC_BOOL *pbEnable, UINT32 *pusTimeIncrement, UINT32 *pusTimeIncrResol)
{
    *pbEnable = gbTimeLapseEnable;

    *pusTimeIncrement = gsTimeLapseFps.usVopTimeIncrement;
    *pusTimeIncrResol = gsTimeLapseFps.usVopTimeIncrResol;
}
#endif


#if (SUPPORT_DUAL_SNR == 1 && DUALENC_SUPPORT == 1) //For Bitek1603
void AHC_VideoDualEncodeSetting(void)
{
    MMP_USHORT usResoIdx = VIDRECD_RESOL_720x480;
    MMPS_3GPRECD_PROFILE        m_2ndProfile = MMPS_H264ENC_BASELINE_PROFILE;//MMPS_H264ENC_HIGH_PROFILE;
    MMPS_3GPRECD_CURBUF_MODE    m_2ndVideoCurBufMode = MMPS_3GPRECD_CURBUF_FRAME;   
    MMPS_3GPRECD_FRAMERATE      m_2ndSensorInFps  = {AHC_VIDRECD_TIME_INCREMENT_BASE_NTSC, 30000};
    MMPS_3GPRECD_FRAMERATE      m_2ndContainerFps = {AHC_VIDRECD_TIME_INCREMENT_BASE_NTSC, 30000};
    MMP_USHORT                  m_usQuality       = VIDRECD_QUALITY_LOW;
    MMP_USHORT usPFrameCount=14, usBFrameCount=0;
    MMPF_VIF_TV_DEC_SRC_TYPE sTVDecSrc = MMPF_VIF_SRC_NO_READY;
    MMP_ULONG ubVidTimeIncBase = AHC_VIDRECD_TIME_INCREMENT_BASE_NTSC;
    MMP_ERR sRet;
        
    /* Set 2nd Video Record Attribute */
    sRet = MMPS_3GPRECD_SetDualEncResIdx(usResoIdx);

    sRet |= MMPS_3GPRECD_SetDualProfile(m_2ndProfile);
    sRet |= MMPS_3GPRECD_SetDualCurBufMode(m_2ndVideoCurBufMode);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return;} 
    
    MMPF_Sensor_GetTV_Dec_Src_Type(&sTVDecSrc);
    switch(sTVDecSrc){
        case MMPF_VIF_SRC_PAL:
            ubVidTimeIncBase = AHC_VIDRECD_TIME_INCREMENT_BASE_PAL;
            break;
        case MMPF_VIF_SRC_NTSC:
            ubVidTimeIncBase = AHC_VIDRECD_TIME_INCREMENT_BASE_NTSC;
            break;
        case MMPF_VIF_SRC_NO_READY:
        default:
            AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; 
            break;
    }
        
    m_2ndSensorInFps.usVopTimeIncrement = ubVidTimeIncBase;
    m_2ndSensorInFps.usVopTimeIncrResol = 30000;
    m_2ndContainerFps.usVopTimeIncrement = m_2ndSensorInFps.usVopTimeIncrement;
    m_2ndContainerFps.usVopTimeIncrResol = m_2ndSensorInFps.usVopTimeIncrResol;

    {
        sRet = MMPS_3GPRECD_SetDualFrameRatePara(&m_2ndSensorInFps, &m_2ndSensorInFps, &m_2ndContainerFps);
#if (SUPPORT_VR_FHD_60FPS)
        sRet |= MMPS_3GPRECD_SetDualH264Bitrate(MMPS_3GPRECD_GetConfig()->ulFps60BitrateMap[usResoIdx][m_usQuality]);
#else
        sRet |= MMPS_3GPRECD_SetDualH264Bitrate(MMPS_3GPRECD_GetConfig()->ulFps30BitrateMap[usResoIdx][m_usQuality]);
#endif
    }

    sRet |= MMPS_3GPRECD_SetDualPFrameCount(usPFrameCount);
    sRet |= MMPS_3GPRECD_SetDualBFrameCount(usBFrameCount);

    sRet |= MMPS_3GPRECD_CustomedEncResol(1/*ubEncIdx*/, 
                                          MMP_SCAL_FITMODE_OPTIMAL, 
                                          MMPS_3GPRECD_GetConfig()->usEncWidth[usResoIdx], 
                                          MMPS_3GPRECD_GetConfig()->usEncHeight[usResoIdx]); //Fix as 720x480 in NTSC and PAL format.    
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return;}                                           
}

AHC_BOOL AHC_VideoDualRecordStart(void)
{
    MMP_ULONG       ulDualEncMode = MMP_VIDRECD_USEMODE_RECD; // MMP_VIDRECD_USEMODE_CB2AP
    MMP_ERR sRet;
    
    sRet = MMPS_3GPRECD_SetDualH264Enable(MMP_TRUE, ulDualEncMode);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
    
    AHC_VideoDualEncodeSetting();

    sRet = MMPS_3GPRECD_StartDualH264();
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

    return AHC_TRUE;
}
#endif



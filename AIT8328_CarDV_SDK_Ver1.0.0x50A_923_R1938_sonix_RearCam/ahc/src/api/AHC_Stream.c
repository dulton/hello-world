/*
 * AHC_Stream.c
 */
#include "AHC_General.h"
#include "AHC_Stream.h"
#include "AHC_Audio.h"
#include "AIHC_Audio.h"
#include "AHC_Video.h"
#include "AIHC_Video.h"
#include "AIHC_Capture.h"
#include "AHC_Sensor.h"
#include "AHC_Gui.h"
#include "AHC_Display.h"
#include "AHC_V4L.h"
#include "menusetting.h"
#include "mmpf_ringbuf.h"
#include "mmpf_streaming.h"
#include "mmp_lib.h"
#include "mmp_media_def.h"
#include "mmps_iva.h"
#include "mmps_audio.h"
#include "mmps_sensor.h"
#include "mmps_vidplay.h"
#include "mmps_3gprecd.h"
#include "stateVideofunc.h"
#include "netapp.h"

#if (defined(WIFI_PORT) && WIFI_PORT == 1)

// external
INT32 AIHC_CloseTranscode(void);
void VRSeamlessCB(void);
void VRFileFullCB(void);
void VRMediaFullCB(void);
void VRMediaErrorCB(AHC_ERR VideoStatus, AHC_ERR VideoFileErr);
void VRMotionDetectCB(void);
AHC_BOOL AIHC_StopVideoRecordModeEx(AHC_BOOL bStopRecord,AHC_BOOL bShutdownSensor, AHC_BOOL bStopPreview);
extern AHC_BOOL		m_bSeamlessRecord;
extern MMP_USHORT	gsAhcCurrentSensor;
extern MMP_USHORT	gsAhcSecondSensor;
extern AHC_BOOL		m_ubMotionDtcEn;
extern MMP_DISPLAY_OUTPUT_SEL   gsAhcCurrentDisplay;
extern AHC_BOOL                 gbAhcDbgBrk;
MMP_MJPEG_OBJ_PTR mjpeg;
static AHC_STREAMING_MODE           m_AhcStreamingMode      = AHC_STREAM_NONE;
static AHC_STREAMING_MODE           m_AhcStreamingResume    = AHC_STREAM_NONE;

static MMP_UBYTE VidCnxt[] = "Transcode";
static void VideoStopCb(void *Context, MMP_ULONG flag1, MMP_ULONG flag2)
{
    printc("%s Video stopped with flag %X, %X\r\n", Context, flag1, flag2);
}

AHC_BOOL VideoRecordStreamCallback(MMP_M_STREAMCB_ACTION cb_action, MMP_ULONG framesize, MMP_M_STREAM_INFO* moveinfo)
{
    return vidioc_notify(cb_action, framesize, moveinfo);
}

AHC_STREAMING_MODE AHC_GetStreamingMode(void)
{
    return m_AhcStreamingMode;
}

AHC_BOOL AIHC_Restore2ndVideo(void)
{
    MMP_ERR err;

    // Means it has been initialized.
    err = MMPS_MJPEG_StopMjpeg(mjpeg);

    if (err == MMP_ERR_NONE) {
        err = MMPS_MJPEG_Return2Display(mjpeg);
    }

    if (err == MMP_ERR_NONE) {
        return AHC_TRUE;
    }
    return AHC_FALSE;
}
// for LCD display
AHC_BOOL AIHC_SetVideoPreviewMode(UINT16 EncWidth, UINT16 EncHeight)
{
    MMP_USHORT      usVideoPreviewMode = VIDRECD_NORMAL_PREVIEW_MODE;
    MMP_ERR sRet = MMP_ERR_NONE;
    
    printc("### %s -\r\n", __func__);

    AHC_OSDSetActive(THUMBNAIL_OSD_FRONT_ID, AHC_FALSE);

    #ifdef FLM_GPIO_NUM
    AHC_OSDSetActive(0, AHC_FALSE);
    #endif

    sRet = MMPS_3GPRECD_PreviewStop(gsAhcCurrentSensor);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

#if (SUPPORT_DUAL_SNR)
    MMPF_Sensor_EnableTV_Dec_Src_TypeDetection(MMP_FALSE);
    sRet = MMPS_3GPRECD_2ndSnrPreviewStop(gsAhcSecondSensor);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); /*return AHC_FALSE;*/} //TBD.  If rear sensor is not connected, skip second stream record.       
#endif     

    AHC_SetVRWithWNR(AHC_FALSE);
    
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

    #if (TVOUT_PREVIEW_EN)
    if (AHC_IsTVConnectEx())
    {
        UINT8 ctv_sys = 0;

        if ((AHC_Menu_SettingGetCB((char*)COMMON_KEY_TV_SYSTEM, &ctv_sys) == AHC_FALSE)) {
            ctv_sys = COMMON_TV_SYSTEM_NTSC; //default value
        }

        if (ctv_sys == COMMON_TV_SYSTEM_NTSC) {
            usVideoPreviewMode = VIDRECD_NTSC_PREVIEW_MODE;
        }
        else {
            usVideoPreviewMode = VIDRECD_PAL_PREVIEW_MODE;
        }
    }
    else
    #endif
    #if HDMI_PREVIEW/*in config_fw.h*/ //ported from #if (HDMI_PREVIEW_EN) //in config_ooo.h
    if(AHC_IsHdmiConnect())
    {
        usVideoPreviewMode = VIDRECD_HDMI_PREVIEW_MODE;
    }
    else
    #endif
    {
        usVideoPreviewMode = VIDRECD_NORMAL_PREVIEW_MODE;
    }

    sRet = MMPS_Sensor_Initialize(gsAhcCurrentSensor, AHC_GetPresetSensorMode(), MMP_SNR_VID_PRW_MODE);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
    
    sRet = MMPS_Display_SetOutputPanel(MMP_DISPLAY_PRM_CTL, gsAhcCurrentDisplay);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

    sRet = MMPS_3GPRECD_SetPreviewMode(usVideoPreviewMode);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

#if WIFI_SEAL <= 1
    Menu_RenewMenuSetting(AHC_FALSE);
#endif
    sRet = MMPS_3GPRECD_PreviewStart(gsAhcCurrentSensor, MMP_FALSE);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
    
    if(MenuSettingConfig()->uiFaceTouch == FACE_DETECT_ON)
        MMPS_Sensor_SetFDEnable(MMP_TRUE);

#if (MOTION_DETECTION_EN)
    #if (VMD_ACTION & VMD_RECORD_VIDEO)
    if( m_ubMotionDtcEn && MenuSettingConfig()->uiMotionDtcSensitivity != MOTION_DTC_SENSITIVITY_OFF)
    {
        UINT8 ubMvTh, ubCntTh;

        AHC_GetMotionDtcSensitivity(&ubMvTh, &ubCntTh);

        printc("AIHC_SetVideoPreviewMode: Motion ubMvTh %d, ubCntTh %d\r\n",ubMvTh, ubCntTh);

        if(ubMvTh==0xFF || ubCntTh==0xFF)
        {
            AHC_EnableMotionDetection(AHC_FALSE);
        }
        else
        {
            AHC_SetMotionDtcSensitivity( ubMvTh, ubCntTh );
            sRet = MMPS_Sensor_RegisterCallback(MMPS_IVA_EVENT_MDTC, (void *)VRMotionDetectCB);
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
            
            AHC_EnableMotionDetection(AHC_TRUE);
        }
    }
    else
    #endif
    {
        AHC_EnableMotionDetection(AHC_FALSE);
    }
#endif

    return AHC_TRUE;
}

AHC_BOOL AIHC_ApplyVideoTranscoding(UINT16 streamID, AHC_BOOL bPlay)
{
    #if (VIDEO_DEC_TO_MJPEG)
    MMP_ERR err = MMP_ERR_NONE;
    UINT16 w, h;
    AHC_MODE_ID ahcMode = AHC_GetAhcSysMode();
    #endif
    UINT32 fpsx10, bitRate, fps;
    MMPF_FRAME_RING_BUF_HANDLE pVideoRing = MMPF_StreamRing_GetFrameRing(VIDEO1_RING_STREAM); 

    // Set audio volume
    #if (AUDIO_SET_DB == 0x01)
    MMPS_AUDIO_SetPlayVolumeDb(95);
    #else
    MMPS_AUDIO_SetPlayVolume(12, MMP_FALSE);
    #endif

    #if (VIDEO_DEC_TO_MJPEG)
    // Initial Wifi MJPEG resolution
    if (AHC_FALSE == AHC_GetVideoResolution(streamID, &w, &h)) {
        AIHC_CloseTranscode();
        return AHC_FALSE;
    }
    MMPS_DSC_SetMJpegResol(w, h);

    if (AHC_MODE_NET_PLAYBACK	!= ahcMode &&
    	AHC_MODE_IDLE			!= ahcMode &&
    	AHC_MODE_THUMB_BROWSER	!= ahcMode) {
        printc(FG_RED("Error. Cannot transcode during AHC MODE %X\r\n"), ahcMode);
        AIHC_CloseTranscode();
        return AHC_FALSE;
    }

    //set bit rate & FPS
    {
        AIHC_GetMovieConfigEx(1, AHC_VIDEO_BITRATE, &bitRate);
        AIHC_GetMovieConfigEx(1, AHC_FRAME_RATEx10, &fpsx10);
        MMPS_VIDPLAY_GetCurFileFps(&fps);
        MMPS_DSC_SetTranscodeRate(fps * 10, fpsx10, bitRate);
    }

    {
        MMP_USHORT fileW, fileH;
        MMPS_VIDPLAY_GetCurFileResolution(&fileW, &fileH);
        err = MMPS_DSC_ConfigVideo2Mjpeg(fileW, fileH);
        MMPF_FrameRing_SetFrc(pVideoRing, fps * 1000, fpsx10 * 100);
    }

    if(!bPlay)
        return AHC_TRUE;

    if (err == MMP_ERR_NONE) {
        err = MMPS_VIDPLAY_Play((void *)VideoStopCb, (void *)VidCnxt);
    }

    if (err != MMP_ERR_NONE) {
        printc("Unable to play the file (%X).\r\n", err);

        AIHC_CloseTranscode();
        return AHC_FALSE;
    }

    /// Get the file duration for SDP
    {
        NETAPP_VALUE value = {VALUE_TYPE_INT, NULL};
        MMPS_VIDPLAY_GetCurFileDuration((MMP_ULONG*)&value.v.i32);
        if (!netapp_set_DeviceSetting(NADS_FILE_DURATION, &value)) {
            printc(FG_RED("[ERR] set file duration %d\r\n"), value.v.i32);
            return AHC_FALSE;
        }
    }

    //reset the player into regular playing mode
    MMPS_VIDPLAY_SetDramMallocAddr(NULL);
    return err;
    #endif

    return AHC_TRUE;
}

AHC_BOOL AIHC_ApplyVideoConfig(UINT16 streamID, UINT16 camid)
{
    MMP_MJPEG_ENC_INFO StillCapture;
    MMP_ULONG fpsx10;
    MMP_MJPEG_RATE_CTL *pRC = NULL;
    MMP_ULONG bitRate;
    MMP_ULONG val;

    if (streamID != 1) {
        return AHC_FALSE;
    }

    AIHC_GetMovieConfigEx(streamID, AHC_VIDEO_CODEC_TYPE, &val);

    if (MMPS_3GPRECD_VIDEO_FORMAT_YUV422 == val) {
        if (AIHC_Restore2ndVideo()) {
            MMPS_MJPEG_CloseMjpeg(&mjpeg);
        }
        else {
            return AHC_FALSE;
        }
    } 
    else if (MMPS_3GPRECD_VIDEO_FORMAT_MJPEG == val) {
        AHC_STREAMING_MODE ahcStreamingMode;
        UINT32 ulValue;

        AIHC_GetMovieConfigEx(1, AHC_VIDEO_STREAMING, &ulValue);
        ahcStreamingMode = ulValue;

        if (AHC_STREAMING_TRANSCODE == ahcStreamingMode) {
            return AIHC_ApplyVideoTranscoding(streamID, AHC_TRUE);
        }

        StillCapture.bTargetCtl     = MMP_FALSE;
        StillCapture.bLimitCtl      = MMP_FALSE;
        StillCapture.bTargetSize    = 200;
        StillCapture.bLimitSize     = 220;
        StillCapture.bMaxTrialCnt   = 1;
        StillCapture.Quality        = MMP_DSC_CAPTURE_HIGH_QUALITY;

        if (AHC_FALSE == AHC_GetVideoResolution(streamID, &StillCapture.usEncWidth, &StillCapture.usEncHeight)) {
            return AHC_FALSE;
        }

        AIHC_AdjustJpegFromSensor(StillCapture.usEncWidth, StillCapture.usEncHeight,
                                  &StillCapture.usEncWidth, &StillCapture.usEncHeight);

        AIHC_GetMovieConfigEx(streamID, AHC_VIDEO_BITRATE, &bitRate);
        AIHC_GetMovieConfigEx(streamID, AHC_FRAME_RATEx10, &fpsx10);

        StillCapture.bTargetCtl = MMP_TRUE;
        StillCapture.bTargetSize = ((bitRate * 10 / fpsx10) >> (3 + 10)) + 1;

        printc("Stream[%d] TargetSize %d KB\r\n",streamID, StillCapture.bTargetSize);

		if (camid == AHC_STREAM_FRONT)
            (void)MMPS_MJPEG_OpenMjpeg(MMP_WIFI_STREAM_FRONTCAM_VIDEO, MMP_WIFI_MODE_VR, &mjpeg);
		else if (camid == AHC_STREAM_REAR)
			(void)MMPS_MJPEG_OpenMjpeg(MMP_WIFI_STREAM_REARCAM_VIDEO, MMP_WIFI_MODE_VR, &mjpeg);
        
        if (NULL != mjpeg) {
            extern MMP_ERR MMPF_Clean_PIP(int w, int h);
            UINT32 sensorFPS, outFPS;
            
            sensorFPS = AHC_GetCurSensorFPS();
            
            if (0 == sensorFPS) {
                sensorFPS = 30;
            }
            
            AIHC_GetMovieConfigEx(streamID, AHC_FRAME_RATEx10, &outFPS);
            
            if (outFPS > 0) {
                if (0 != netapp_set_FRC(sensorFPS * 1000, outFPS * 100, "VIDEO.0/EncMJPEG")) {
                    return AHC_FALSE;
                }
            }

    		if (camid == AHC_STREAM_FRONT)
                MMPS_MJPEG_StartFrontCamMjpeg(gsAhcCurrentSensor, 0, mjpeg, &StillCapture, pRC);
    		else if (camid == AHC_STREAM_REAR)
    			MMPS_MJPEG_StartRearCamMjpeg(0, mjpeg, &StillCapture, pRC);
            
            #if !defined(NO_PANEL)
            if (camid == AHC_STREAM_FRONT) {
                // Disable Preview Window (PIP)
                MMPF_Clean_PIP( MMPS_Display_GetConfig()->mainlcd.usWidth,
                                MMPS_Display_GetConfig()->mainlcd.usHeight);
                MMPS_Display_SetWinActive(FRONT_CAM_WINDOW_ID, 0);
            }
            else if (camid == AHC_STREAM_REAR) {
                MMPS_Display_SetWinActive(REAR_CAM_WINDOW_ID, 0);
            }
            #endif
        }
    }
    else if (MMPS_3GPRECD_VIDEO_FORMAT_OTHERS == val) {
        MMP_M_STATE     state;
        MMP_ULONG       KeyFrameNum;
        MMPS_VIDPLAY_GetState(&state);

        if (state != MMP_M_STATE_IDLE) {
            MMPS_VIDPLAY_Stop(&KeyFrameNum);
        }
        // Stop Streaming Timer after transcode to reset time stamp
        MMPD_Streaming_Enable(VIDEO1_RING_STREAM, MMP_FALSE);
        AIHC_CloseTranscode();
    } 
    else {
        printc("Unknown v format %d\r\n", val);
        MMP_FALSE;
    }
    return AHC_TRUE;
}

AHC_BOOL AIHC_ApplyAudioBypass(UINT16 streamID)
{
    MMP_ULONG addr, size;

    MMPD_Streaming_Init(AUDIO_RING_STREAM); //new a ring buffer in the name of JPEG functions
    MMPS_VIDPLAY_GetCurAudioBuffer(&addr, &size);
    MMPD_Streaming_SetCompBuf(AUDIO_RING_STREAM, addr, size);//new a ring buffer in the name of JPEG functions
    MMPD_Streaming_Enable(AUDIO_RING_STREAM, MMP_TRUE);
    return AHC_TRUE;
}

static __inline AHC_BOOL _Is_LiveAudio_Recording(void)
{
    MMP_USHORT  status;

    MMPD_AUDIO_GetLiveRecordStatus(&status);

    if (status == ENCODE_OP_START || status == ENCODE_OP_PREENCODE) {
    	return AHC_TRUE;
    }
    return AHC_FALSE;
}

AHC_BOOL AIHC_ApplyAudioConfig(UINT16 streamID, AHC_STREAMING_MODE mode)
{
    UINT32 val;
    UINT32 mute;
    AHC_BOOL bValidStream;
    MMP_ULONG bitRate, samplingRate;
    MMPS_3GPRECD_AUDIO_FORMAT format;

    bValidStream = AIHC_GetMovieConfigEx(streamID, AHC_AUD_MUTE_END, &mute);
    if (!bValidStream) {
        printc("invalid audio stream %d\r\n", streamID);
        return AHC_FALSE;
    }
    AIHC_GetMovieConfigEx(streamID, AHC_AUD_CODEC_TYPE, &val);
    printc(">>>> AUDIO TYPE %d\n", val);
    if (mute == MMP_TRUE) {
        //TODO do something to turn off the streaming audio track
        //reset the buffer to 0. Now they are used to know as ring buffer initialization indicator.
        //MMPD_MJPEG_SetCompBuf(1, 0, 0);
        //probably need to return here.
    }
    if (mode == AHC_STREAM_ADPCM) {
    	// This is PCM format, we are not support ADPCM
        MMPS_FW_OPERATION  status_vid;
        MMPS_3GPRECD_RecordStatus(&status_vid);
        if ((status_vid != MMPS_FW_OPERATION_START) && (status_vid != MMPS_FW_OPERATION_PREENCODE)) {
            MMP_ERR     err;
            MMP_UBYTE   ubWMR;
            AIHC_InitAudioExtDACIn();
            MMPS_3GPRECD_SetAudioRecMode(MMPS_3GPRECD_REC_AUDIO_DATA);
            
            AHC_Menu_SettingGetCB((char *)COMMON_KEY_WNR_EN, &ubWMR);
            if( ubWMR == WNR_ON ) {
	    		AHC_SetVRWithWNR(AHC_TRUE);
			}
			else {
				AHC_SetVRWithWNR(AHC_FALSE);
			}
			
            AIHC_GetMovieConfigEx(streamID, AHC_AUD_CHANNEL_CONFIG, &val);
            MMPS_AUDIO_SetLineInChannel(val);
            AHC_Audio_ParseOptions(AHC_ADPCM_AUDIO_OPTION, &format, &samplingRate, &bitRate);
            err = MMPS_3GPRECD_SetAudioFormat(format, AHC_ADPCM_AUDIO_OPTION);
            if (MMP_ERR_NONE != err) {
                //this should be an assertion! the code should not go here. The current code could not handle this!
                printc("[ERR] Unable to set audio format\r\n");
            }

            //These 2 keeps the value of the setting for quicker reference only. The real settings
            // automatically applied in MMPS
            AHC_SetMovieConfig(streamID, AHC_AUD_BITRATE, bitRate);
            AHC_SetMovieConfig(streamID, AHC_AUD_SAMPLE_RATE, samplingRate);

            if(MenuSettingConfig()->uiMOVSoundRecord == MOVIE_SOUND_RECORD_ON)
                AHC_SetVideoRecordVolume(AHC_TRUE);
            else
                AHC_SetVideoRecordVolume(AHC_FALSE);

            if (MMP_ERR_NONE == MMPS_3GPRECD_PreAllocateMemory()) {
                UINT32 addr, size;
                MMPD_Streaming_Init(AUDIO_RING_STREAM); //new a ring buffer in the name of JPEG functions
                MMPD_AUDIO_GetEncodeBuf(&addr, &size);
                MMPD_Streaming_SetCompBuf(AUDIO_RING_STREAM, addr, size);//new a ring buffer in the name of JPEG functions
                AHC_SetVRVolume(AHC_TRUE);
                MMPD_Streaming_Enable(AUDIO_RING_STREAM, MMP_TRUE);
				MMPS_AUDIO_InitLiveRecord(MMPD_AUDIO_PCM_ENCODE, 0, AHC_GetAudioSamplingRate(VR_AUDIO_TYPE_ADPCM), NULL);
                if(m_AhcStreamingMode != AHC_STREAM_PAUSE)
					MMPS_LiveAudio_StartRecord();
				return AHC_TRUE;
            }
            return AHC_FALSE;
        }

        if ((status_vid == MMPS_FW_OPERATION_START) || (status_vid == MMPS_FW_OPERATION_PREENCODE)) {
            UINT32 addr, size;
            MMPD_Streaming_Init(AUDIO_RING_STREAM); //new a ring buffer in the name of JPEG functions
            MMPD_AUDIO_GetEncodeBuf(&addr, &size);
            MMPD_Streaming_SetCompBuf(AUDIO_RING_STREAM, addr, size);//new a ring buffer in the name of JPEG functions
            MMPD_Streaming_Enable(AUDIO_RING_STREAM, MMP_TRUE);
            MMPD_AUDIO_StartAudioRecordEx(MMP_FALSE, MMPD_AUDIO_PCM_ENCODE, samplingRate);
           // MMPD_AUDIO_DoEncodeOp(ENCODE_OP_START);
        }
        else {
            printc(ESC_ATR_COLOR_BRIGHTER FG_RED("not recording, this path is not implemented\r\n"));
            return AHC_FALSE;
        }
    } else 
    if (AHC_STREAM_AAC == mode) {
        if (!_Is_LiveAudio_Recording()) {
			MMPS_AUDIO_ENCODE_FMT mmps_fmt;
			mmps_fmt = MMPS_AUDIO_AAC_ENCODE;
            //review MMPS_AUDIO_LINEIN_DUAL & 48000
            //MMPS_AUDIO_SetLineInChannel(MMPS_AUDIO_LINEIN_DUAL);
			MMPD_Streaming_Init(AUDIO_RING_STREAM); //new a ring buffer in the name of JPEG functions
			//AHC_SetVRVolume(AHC_TRUE);
            AIHC_GetMovieConfigEx(streamID, AHC_AUD_CHANNEL_CONFIG, &val);
            if(m_AhcStreamingMode != AHC_STREAM_PAUSE) {
            	MMP_ERR err;
				//AIHC_GetMovieConfigEx(streamID, AHC_AUD_SAMPLE_RATE, &samplingRate);
            	MMPS_AUDIO_InitLiveRecord(mmps_fmt, MMPS_AUDIO_AAC_RECORD_32K_128K, 32000, NULL);
            	(void)MMPS_LiveAudio_StopRecord();
            	err = MMPS_LiveAudio_StartRecord();
            	if (err != MMP_ERR_NONE) {
            		printc(FG_RED("Unable to start live audio. %X\r\n"), err);
            	}
            }
			MMPD_Streaming_Enable(AUDIO_RING_STREAM, MMP_TRUE);
        }
        
    }
    else if (AHC_MOVIE_AUDIO_CODEC_G711 == val) {
        //mixed use with MMPS_AUDIO since there is no MMPS_3GPRECD_AUDIO_FORMAT_G711 now.
        //Need to be revised later with RD team.
        MMPS_FW_OPERATION  status_vid;
        //TODO here it assumes it is recording, otherwise there is an error about this
        MMPS_3GPRECD_RecordStatus(&status_vid);
        if ((status_vid != MMPS_FW_OPERATION_START) && (status_vid != MMPS_FW_OPERATION_PREENCODE)) {
            //review MMPS_AUDIO_LINEIN_DUAL & 48000
            //MMPS_AUDIO_SetLineInChannel(MMPS_AUDIO_LINEIN_DUAL);
            //MMPS_AUDIO_InitLiveRecord(MMPS_AUDIO_G711_ENCODE,48000);
            AIHC_GetMovieConfigEx(streamID, AHC_AUD_CHANNEL_CONFIG, &val);
            MMPS_AUDIO_SetLineInChannel(val);
            (void)AHC_Audio_ParseOptions(AHC_AAC_AUDIO_OPTION, &format, &samplingRate, &bitRate);
            MMPS_AUDIO_InitLiveRecord(MMPS_AUDIO_G711_ENCODE, 0, samplingRate, NULL);
            MMPS_LiveAudio_StartRecord();
        }
    } else {
        printc("Unsupported audio format %d\r\n", val);
    }
    return AHC_TRUE;
}

//Centralize all state transition which would cause problems.
//The goal is to remove the constrain as much as possible.
//assume all streaming state could transit to AHC_STREAM_OFF
static AHC_BOOL AIHC_IsStateStreamable(AHC_STREAMING_MODE streamingMode)
{
	AHC_MODE_ID	mode = AHC_GetAhcSysMode();

	if ((streamingMode & AHC_STREAM_TRANSCODE) || (m_AhcStreamingMode & AHC_STREAM_TRANSCODE)) {
		return AHC_TRUE;
	}
    if ((AHC_MODE_VIDEO_RECORD == mode) || (AHC_MODE_RECORD_VONLY == mode)) {
        //feasible but not support it for the moment
        //because now in this mode, the card is really recording now.
        //so H.264 is not available.
        if (AHC_STREAM_H264 == streamingMode)
            return AHC_FALSE;
    } else if (AHC_MODE_RECORD_PREVIEW != mode) {
        return AHC_FALSE;
    }

    //validate the setting mode
    if (((m_AhcStreamingMode & AHC_STREAM_V_MASK) <= AHC_STREAM_V_MAX) &&
        ((m_AhcStreamingMode & AHC_STREAM_A_MASK) <= AHC_STREAM_A_MAX))
        return AHC_TRUE;
    return AHC_FALSE;
}

AHC_BOOL AHC_IsRecorderControllable(AHC_MODE_ID ahcMode)
{
    if (AHC_STREAM_OFF != m_AhcStreamingMode) {
    //    return AHC_FALSE;
    }
    if (AHC_STREAM_H264 == m_AhcStreamingMode) {
        return AHC_FALSE;
    }
    if ((ahcMode == AHC_MODE_VIDEO_RECORD) ||
        (ahcMode == AHC_MODE_RECORD_VONLY) ||
        (ahcMode == AHC_MODE_RECORD_PREVIEW)) {
        return AHC_TRUE;
    }
    return AHC_FALSE;
}
/** @brief Map 2 dimensions resolution to UVC or resolution index
 *
 *  @note The crop only deal with right and bottom
 *  @param[in] w Width
 *  @param[in] h Height
 *  @param[out] ResIdx The index of the resolution. It might be UVC index or CAR DV index
 *  @param[out] cropBottom Crop
 *  @param[out] cropRight The index of the resolution. It might be UVC index or CAR DV index
 */
AHC_BOOL AIHC_Video_MapResToIdx(UINT16 w, UINT16 h, UINT16 *ResIdx, UINT16 *cropBottom, UINT16 *cropRight)
{
#if defined(SUPPORT_UVC_FUNC)
    MMP_USHORT                  VideoRes;

    if((1920 == w) && (1088 == h)){
        VideoRes = VIDRECD_RESOL_1920x1088;
    }
    else if((1280 == w) && (720 == h)){
        VideoRes = VIDRECD_RESOL_1280x720;
    }
    else if((640 == w) && (480 == h)){
        VideoRes = VIDRECD_RESOL_640x480;
    }
    else if((640 == w) && (368 == h)){
        VideoRes = VIDRECD_RESOL_640x368;
    }
    else if((2304 == w) && (1296 == h)){
        VideoRes = VIDRECD_RESOL_2304x1296;
    }
    else if((2560 == w) && (1440 == h)){
        VideoRes = VIDRECD_RESOL_2560x1440;
    }	
    else{
        printc(FG_RED("--E-- Video resolution error. %dx%d\r\n"), w, h);
        return AHC_FALSE;
    }

    *ResIdx = VideoRes;
#else
    MMP_USHORT VideoRes[VIDRECD_RESOL_MAX_NUM] =
    {
        //VIDRECD_RESOL_128x96, //Andy Liu TBD
        VIDRECD_RESOL_176x144,
        VIDRECD_RESOL_320x240,
        VIDRECD_RESOL_352x288,
        VIDRECD_RESOL_640x480,
        VIDRECD_RESOL_720x480,
        //VIDRECD_RESOL_128x128, //Andy Liu TBD
        VIDRECD_RESOL_1280x720,
        VIDRECD_RESOL_1920x1088
    };
    MMP_USHORT                  usResolIdx;
    UINT16                      i;
    MMPS_3GPRECD_CONFIGS        *VideoConfig    = MMPS_3GPRECD_GetConfig();

    for (i=0;i < VIDRECD_RESOL_MAX_NUM;i++) {
        if ( (w * h) <= (VideoConfig->usEncWidth[i] * VideoConfig->usEncHeight[i]) ) {
            usResolIdx = i;
            break;
        }
    }

    if (VIDRECD_RESOL_MAX_NUM == i) {
        return AHC_FALSE;
    }

    *ResIdx = VideoRes[usResolIdx];
#endif

    *cropBottom = 0;
    *cropRight = 0;
    if (*ResIdx == VIDRECD_RESOL_1920x1088 || *ResIdx == VIDRECD_RESOL_640x368) {
        *cropBottom = 8;
    }

    return AHC_TRUE;
}
//
static AHC_BOOL AIHC_SetVideoPreRecordMode(AHC_BOOL bAudioOn, AHC_BOOL bSetPreview, MMPS_3GPRECD_SRCMODE srcMode)
{

    MMPS_3GPRECD_VIDEO_FORMAT   VideoFmt;
    MMPS_3GPRECD_AUDIO_FORMAT   AudioFmt;
    MMPS_3GPRECD_CONTAINER      ContainerType;
    MMP_ULONG                   ulSamplerate;
    MMPS_AUDIO_LINEIN_CHANNEL   AudioLineIn;
    UINT32                      Param;
    INT32                       FrameRate, PFrameNum, BFrameNum;
    UINT32                      EncWidth, EncHeight;
    UINT16                      ResIdx, cropBottom, cropRight;
    UINT32                      TimeStampOp;
    UINT32                      TimeLimit       = 0;
    MMP_USHORT                  RealFrameRate   = 0;
    MMP_ULONG                   ulCurBitrate    = 0;
    MMPS_3GPRECD_FRAMERATE      sFps;
    MMPS_3GPRECD_FRAMERATE      sContainerFps;
    MMP_ERR sRet = MMP_ERR_NONE;
    
    AHC_GetImageSize(VIDEO_CAPTURE_MODE, &EncWidth, &EncHeight);

    if (AHC_FALSE == AIHC_Video_MapResToIdx(EncWidth, EncHeight, &ResIdx, &cropBottom, &cropRight)) {
        return AHC_FALSE;
    }

    AHC_ConfigMovie(AHC_VIDEO_RESOLUTION, ResIdx);
    MMPS_3GPRECD_SetEncResIdx(ResIdx);

    AHC_ConfigMovie(AHC_VIDEO_COMPRESSION_RATIO, MenuSettingConfig()->uiMOVQuality);

    // CarDV
    // Call netapp_get_** needs wait Network task start up!!
    ulCurBitrate = netapp_get_H264Bitrate(); // For Streaming
    MMPS_3GPRECD_SetBitrate(ulCurBitrate);

    if (bSetPreview)
    {
        AIHC_SetVideoPreviewMode(EncWidth, EncHeight);
    }

    AHC_GetCaptureConfig(ACC_DATE_STAMP, &TimeStampOp);

    if (TimeStampOp & AHC_ACC_TIMESTAMP_ENABLE_MASK)
    {
        UINT32 movieSize = 0;

        AHC_Menu_SettingGetCB((char *) COMMON_KEY_MOVIE_SIZE, &movieSize);

        {
            AHC_RTC_TIME sRtcTime;

            AHC_RTC_GetTime(&sRtcTime);
            AIHC_ConfigVRTimeStamp(TimeStampOp, &sRtcTime, MMP_TRUE);
        }
    }

    MMPS_3GPRECD_SetCropping(0/*ulEncId*/, 0, cropBottom, 0, cropRight);

    AIHC_GetMovieConfig(AHC_VIDEO_CODEC_TYPE, &Param);
    VideoFmt        = Param;

    AIHC_GetMovieConfig(AHC_AUD_CODEC_TYPE, &Param);
    AudioFmt        = Param;

    AIHC_GetMovieConfig(AHC_CLIP_CONTAINER_TYPE, &Param);
    ContainerType   = Param;

    AIHC_GetMovieConfig(AHC_MAX_PFRAME_NUM, &Param);
    PFrameNum       = Param;

    AIHC_GetMovieConfig(AHC_MAX_BFRAME_NUM, &Param);
    BFrameNum       = Param;

    AIHC_GetMovieConfig(AHC_AUD_SAMPLE_RATE, &Param);
    ulSamplerate    = Param;

    AIHC_GetMovieConfig(AHC_AUD_CHANNEL_CONFIG, &Param);
    AudioLineIn     = Param;

    AIHC_GetMovieConfig(AHC_FRAME_RATE, &Param);
    FrameRate       = Param;

    if (bAudioOn) {
        AIHC_InitAudioExtDACIn();
    }

    MMPS_3GPRECD_SetContainerType(ContainerType);
    MMPS_3GPRECD_SetVideoFormat  (VideoFmt);
#if (VR_VIDEO_CURBUF_ENCODE_MODE == COMMON_VR_VIDEO_CURBUF_RT)
    MMPS_3GPRECD_SetCurBufMode(MMPS_3GPRECD_CURBUF_RT); //Force it as RT mode to prevent memory overflow.
#else
    MMPS_3GPRECD_SetCurBufMode(MMPS_3GPRECD_CURBUF_FRAME);
#endif

    RealFrameRate = AHC_GetVideoRealFrameRate(FrameRate);

    sFps.usVopTimeIncrement = AHC_VIDRECD_TIME_INCREMENT_BASE;
    sFps.usVopTimeIncrResol = RealFrameRate;

    sContainerFps.usVopTimeIncrement = sFps.usVopTimeIncrement;
    sContainerFps.usVopTimeIncrResol = sFps.usVopTimeIncrResol;

    MMPS_3GPRECD_SetFrameRatePara(&sFps, &sFps, &sContainerFps);

    MMPS_3GPRECD_SetBFrameCount  (BFrameNum);
    MMPS_3GPRECD_SetPFrameCount  (PFrameNum);
    
    if (bAudioOn)
    	MMPS_3GPRECD_SetAudioRecMode(MMPS_3GPRECD_REC_AUDIO_DATA);
    else
    	MMPS_3GPRECD_SetAudioRecMode(MMPS_3GPRECD_NO_AUDIO_DATA);

    MMPS_AUDIO_SetLineInChannel(AudioLineIn);

    if(AudioFmt == MMPS_3GPRECD_AUDIO_FORMAT_AMR) {
        MMPS_3GPRECD_SetAudioFormat(AudioFmt, MMPS_3GPRECD_AUDIO_AMR_12d2K);
    }
    else if (AudioFmt == MMPS_3GPRECD_AUDIO_FORMAT_AAC) {
        MMPS_3GPRECD_SetAudioFormat(AudioFmt, AHC_AAC_AUDIO_OPTION);
    }
    else if (AudioFmt == MMPS_3GPRECD_AUDIO_FORMAT_ADPCM) {
        MMPS_3GPRECD_SetAudioFormat(AudioFmt, MMPS_3GPRECD_AUDIO_ADPCM_16K_22K);
    }
    else if (AudioFmt == MMPS_3GPRECD_AUDIO_FORMAT_MP3){
        MMPS_3GPRECD_SetAudioFormat(AudioFmt, AHC_MP3_AUDIO_OPTION);
    }

    if(bAudioOn) {
        MMP_UBYTE   ubWMR;
        AHC_Menu_SettingGetCB((char *)COMMON_KEY_WNR_EN, &ubWMR);
		if( ubWMR == WNR_ON ) {
    		AHC_SetVRWithWNR(AHC_TRUE);
		}
		else {
			AHC_SetVRWithWNR(AHC_FALSE);
		}
	}
	else {
		AHC_SetVRWithWNR(AHC_FALSE);
	}
	
    MMPS_3GPRECD_SetStoragePath( srcMode );

    if (srcMode != MMPS_3GPRECD_SRCMODE_STREAM)
    {
        MMPS_3GPRECD_RegisterCallback(MMPS_3GPRECD_EVENT_MEDIA_FULL,   (void*)VRMediaFullCB/*VideoRecordMediaFullCallback*/);
        MMPS_3GPRECD_RegisterCallback(MMPS_3GPRECD_EVENT_FILE_FULL,     (void*)VRFileFullCB/*VideoRecordFileFullCallback*/);
        MMPS_3GPRECD_RegisterCallback(MMPS_3GPRECD_EVENT_MEDIA_ERROR,  (void*)VRMediaErrorCB/*VideoRecordMediaErrorCallback*/);
        MMPS_3GPRECD_RegisterCallback(MMPS_3GPRECD_EVENT_SEAMLESS ,    (void*)VRSeamlessCB/*VideoRecordSeamlessCallback*/);
#if (DUALENC_SUPPORT == 1)
        sRet = MMPS_3GPRECD_RegisterCallback(MMPS_3GPRECD_EVENT_DUALENC_FILE_FULL, (void *)DualEncRecordCbFileFullCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
#endif        

        MMPS_3GPRECD_SetFileSizeLimit(SIGNLE_FILE_SIZE_LIMIT_3_75G);

        TimeLimit = AHC_GetVRTimeLimit();

        if(TimeLimit==NON_CYCLING_TIME_LIMIT)
        {
            UINT64  ulStorageFreeSpace  = 0;

            VideoFunc_GetFreeSpace(&ulStorageFreeSpace);

            if(ulStorageFreeSpace < SIGNLE_FILE_SIZE_LIMIT_4G)
            {
                printc("Free Space Not Enough to Enable Seamless-2\r\n");
                MMPS_3GPRECD_StartSeamless(MMP_FALSE);
                m_bSeamlessRecord = AHC_FALSE;
            }
            else
            {
                MMPS_3GPRECD_StartSeamless(MMP_TRUE);
                m_bSeamlessRecord = AHC_TRUE;
            }
        }
        else
        {
            MMPS_3GPRECD_StartSeamless(MMP_TRUE);
            m_bSeamlessRecord = AHC_TRUE;
        }

        if(MenuSettingConfig()->uiMOVSoundRecord == MOVIE_SOUND_RECORD_ON)
            AHC_SetVideoRecordVolume(AHC_TRUE);
        else
            AHC_SetVideoRecordVolume(AHC_FALSE);

        sRet = MMPS_3GPRECD_PreRecord(AHC_GetPreRecordTime());
        if (sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

        printc("Starting PreRecord!!!\r\n");
    }
    else {
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_MEDIA_FULL,   (void*)VRMediaFullCB/*VideoRecordMediaFullCallback*/);
        //MMPS_3GPRECD_RegisterCallback(MMPS_3GPRECD_EVENT_FILE_FULL,   (void*)VRFileFullCB/*VideoRecordFileFullCallback*/);//TBD
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_MEDIA_ERROR,  (void*)VRMediaErrorCB/*VideoRecordMediaErrorCallback*/);
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_SEAMLESS ,    (void*)VRSeamlessCB/*VideoRecordSeamlessCallback*/);
#if (DUALENC_SUPPORT == 1)
        sRet = MMPS_3GPRECD_RegisterCallback(MMPS_3GPRECD_EVENT_DUALENC_FILE_FULL, (void *)DualEncRecordCbFileFullCB);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
#endif        
        
#if 0
        MMPS_3GPRECD_SetFileSizeLimit(SIGNLE_FILE_SIZE_LIMIT_3_75G);
        MMPS_3GPRECD_StartSeamless(MMP_TRUE);
        m_bSeamlessRecord = AHC_TRUE;
        AHC_SetVideoRecordVolume(AHC_FALSE);
#endif

        sRet = MMPS_3GPRECD_PreRecord(AHC_GetPreRecordTime());
        if (sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

        printc("Starting Video Streaming!!!\r\n");
    }
    return AHC_TRUE;
}

static AHC_BOOL AIHC_ConfigStreamingH264(AHC_BOOL on)
{
    NETAPP_VALUE value;

    if (!netapp_get_DeviceSetting(NADS_AUDIO_PREVIEW, &value)) {
        return AHC_FALSE;
    }

    if (on) {
        return AIHC_SetVideoPreRecordMode( (AHC_BOOL)value.v.i32, AHC_TRUE, MMPS_3GPRECD_SRCMODE_STREAM );
    }
    else {
        AIHC_StopVideoRecordModeEx(AHC_TRUE, AHC_FALSE, AHC_FALSE);
    }
    return AHC_TRUE;
}

MMP_ULONG gAhcRes = AHC_VIDRECD_RESOL_320x240;
#ifdef REARVIEW_STREAMING_TEST
/* add for rear MJPEG test start*/
static MMP_ULONG AHC_TEST_CalThresholdx2(MMP_ULONG ulFrameSize)
{
    return ulFrameSize * 2;
}


void AHC_TEST(void *aa, void *bb)
{
	AITPS_JPG 	pJPG = AITC_BASE_JPG;
	MMP_ULONG   time;
	unsigned int fsize;

	MMP_ULONG FAddr;
	MMP_ULONG SSize;
	
	MMPF_FRAME_RING_BUF_HANDLE pRing = MMPF_StreamRing_GetFrameRing(VIDEO2_RING_STREAM);
	AHC_OS_GetTime(&time);
	MMPF_FrameRing_GetNextReadAddr(pRing,&FAddr,&SSize);
	fsize=netapp_avi_filegetframe(FAddr);
	if (fsize==0)
		netapp_avi_stopstream();
//	printc("fsize=%d, ptr=%x\n",fsize,FAddr);
	if (MMPF_FrameRing_AddFrame(pRing, fsize/*pJPG->JPG_ENC_FRAME_SIZE*/, MMP_STREAM_JPEG2, time) == MMP_TRUE) {
       	MMPF_OS_SetFlags(Streaming_Flag, STREAM_FLAG_JPEG_STREAM2, MMPF_OS_FLAG_SET);
    	}
}
/* add for rear MJPEG test end*/
#endif

/** Set the streaming mode
 *
 * The streaming logic is put here.
 * @param streamingMode The streaming mode
 * @param bWait. In some case like as MoveFrame call Push to get a V4L buffer to move frame.
 *               It should not wait, because the MoveFrame is critical in timing of audio and
 *               video, and the _AHC_SetStreamingMode may block by AHC_SetMode a while.
 *               In this case, the bWait must be FALSE, and causes MoveFrame to drop a frame.
 *               It will keep Ring healthy working.
 * @return AHC_FALSE fail to config streaming mode.
 */
AHC_BOOL _AHC_SetStreamingMode(AHC_STREAMING_MODE streamingMode, AHC_BOOL bWait, char * str, int line)
{
#ifdef REARVIEW_STREAMING_TEST
/* add for rear MJPEG */
	static MMPF_OS_TMRID    TEST_MODE = 0xFF;
	MMP_ULONG ahcbuf;
/* add for rear MJPEG */	
#endif

    AHC_BOOL audioEn = AHC_FALSE; //get the audio config from MMI later
    //currently because audio streaming is not supported, so it is always off.
    //MenuSettingConfig()->uiMOVSoundRecord == MOVIE_SOUND_RECORD_ON
    AHC_STREAMING_MODE vMode, aMode,vMode2;
    MMP_BOOL    dscv, vidv;

    if (!bWait) {
        UINT16  cnt;
        AHC_OS_QuerySem(ahc_semi, &cnt);
        if (cnt == 0) return AHC_FALSE;
        if (LOCK_AHC_MODE_NOWAIT() != 0) return AHC_FALSE;
    } else {
        LOCK_AHC_MODE();
    }
    /*
     * NOTE:
     * the expression ((streamingMode | m_AhcStreamingMode) & AHC_STREAM_TRANSCODE == 0)
     * it seems to do like as (streamingMode | m_AhcStreamingMode) & (AHC_STREAM_TRANSCODE == 0)
     * not as our thought ((streamingMode | m_AhcStreamingMode) & AHC_STREAM_TRANSCODE) == 0
     */
    MMPS_DSC_GetPreviewStatus(&dscv);
    MMPS_3GPRECD_GetPreviewPipeStatus(gsAhcCurrentSensor, &vidv);
    printc("@@AHC_SetStreamingMode: mode %x -> %x AHCMODE %x %d/%d @ %s line %d\r\n",
            m_AhcStreamingMode, streamingMode, AHC_GetAhcSysMode(), dscv, vidv, str, line);
    if (((m_AhcStreamingMode & AHC_STREAM_TRANSCODE) && streamingMode == AHC_STREAM_PAUSE) ||
        ((m_AhcStreamingResume & AHC_STREAM_TRANSCODE) && streamingMode == AHC_STREAM_RESUME)) {

        if (streamingMode == AHC_STREAM_PAUSE) {
            printc(BG_PURPLE("PAUSE\n"));
            MMPS_VIDPLAY_Pause();
            m_AhcStreamingResume = m_AhcStreamingMode;
        }
        else {
            MMPS_VIDPLAY_Resume();
            printc(BG_BLUE("RESUME\n"));
            m_AhcStreamingResume = AHC_STREAM_NONE;
        }
        UNLOCK_AHC_MODE();
        return AHC_TRUE;
    }
    if (!(streamingMode & AHC_STREAM_TRANSCODE) && !(m_AhcStreamingResume & AHC_STREAM_TRANSCODE)) {

        if (AHC_GetAhcSysMode() == AHC_MODE_IDLE && (((streamingMode | m_AhcStreamingMode) & AHC_STREAM_TRANSCODE) == 0)) {
            printc(FG_YELLOW("VIDEO PREVIEW WAS OFF!!\n"));
            UNLOCK_AHC_MODE();
            return AHC_FALSE;
        }
    }

    if (m_AhcStreamingMode == streamingMode) { // no change, just return
        UNLOCK_AHC_MODE();
        return AHC_TRUE;
    }
    if ((streamingMode == AHC_STREAM_PAUSE  && m_AhcStreamingMode == AHC_STREAM_NONE) ||
        (streamingMode == AHC_STREAM_RESUME && m_AhcStreamingMode != AHC_STREAM_PAUSE)) {
        UNLOCK_AHC_MODE();
        return AHC_FALSE;
    }
    if (streamingMode == AHC_STREAM_RESUME) {
        streamingMode = m_AhcStreamingResume;
        printc(BG_BLUE("RESUME\n"));
        aMode = AHC_STREAM_RESUME;
    }
    else if (streamingMode == AHC_STREAM_PAUSE) {
        if ((m_AhcStreamingMode & (AHC_STREAM_V_MASK | AHC_STREAM_A_MASK)) == AHC_STREAM_NONE) {
            UNLOCK_AHC_MODE();
            return AHC_FALSE;
        }
        m_AhcStreamingResume = m_AhcStreamingMode;
        aMode = AHC_STREAM_PAUSE;
        printc(BG_YELLOW("PAUSE\n"));
    }
    else {
        aMode = streamingMode & AHC_STREAM_A_MASK;
    }

    vMode = streamingMode & AHC_STREAM_V_MASK;
	vMode2 = streamingMode & AHC_STREAM_V_MASK2;// add for rear MJPEG

    if (AIHC_IsStateStreamable(streamingMode) == AHC_FALSE) {
        if (AHC_GetAhcSysMode() == AHC_MODE_IDLE && vMode != AHC_STREAM_NONE && vMode2 != AHC_STREAM_NONE) {
            /* In some case, there is no SD and LCD. It won't be previewing
             * or other viewing state and is in idle mode instead.
             * */
            UNLOCK_AHC_MODE();

            if (AHC_SetMode(AHC_MODE_RECORD_PREVIEW) == AHC_FALSE) {
                printc("Unable to set RECORD_PREVIEW mode\r\n");
                return AHC_FALSE;
            }
            LOCK_AHC_MODE();
        }
        else {
            printc("Unable to set streaming mode %d due to current AHC_Mode and streaming Mode\r\n",
                    streamingMode);
            UNLOCK_AHC_MODE();
            return AHC_FALSE;
        }
    }

    if ((AHC_STREAM_NONE == vMode)&& (AHC_STREAM_NONE == vMode2)) {
        // Turn off H264 Streaming
        MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_STREAMCB,     (void*)NULL);
    } else {
        //set to MMPF_VIDMGR_StreamBufCallback
        if (vMode == AHC_STREAM_H264) {
            MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_STREAMCB,     (void*)VideoRecordStreamCallback);
        }
    }

	if ((AHC_STREAM_NONE == vMode) && (AHC_STREAM_NONE == vMode2)&&(AHC_STREAM_NONE == aMode)){
		MMPF_FrameRing_SetMoveCallback(NULL);
    }
    else {
        MMPF_FrameRing_SetMoveCallback(VideoRecordStreamCallback);
    }
#ifdef REARVIEW_STREAMING_TEST
	if (vMode2==AHC_STREAM_NONE) {
		if (TEST_MODE != 0xFF)
		{
			MMPF_OS_StopTimer(TEST_MODE,NULL);
			netapp_test_mfree();
			netapp_avi_fileclose();
			TEST_MODE = 0xFF;
			if (vMode != AHC_STREAM_NONE) {
				m_AhcStreamingMode = streamingMode;
    				UNLOCK_AHC_MODE();
   				return AHC_TRUE;
			}
        }
	}
#endif	
    //MMPS_3GPRECD_RegisterCallback (MMPS_3GPRECD_EVENT_MEDIA_SLOW,   (void*)VideoRecordMediaSlowCallback);
    if (aMode != AHC_STREAM_NONE && aMode != AHC_STREAM_PAUSE && aMode != AHC_STREAM_RESUME) {
        audioEn = AHC_TRUE;

        if (AHC_STREAM_AAC == aMode || AHC_STREAM_ADPCM == aMode) {
            // AHC_ConfigAudio(AHC_AUDIO_STREAM_ID, AHC_MOVIE_AUDIO_CODEC_AAC, AHC_AUDIO_CHANNEL_MONO_R);
            if (streamingMode & AHC_STREAM_TRANSCODE) {
                ///@warning assume it's video transcoding and audio bypass
                //TODO check if the file contains AAC, since here it should be parsed
                if (AHC_TRUE != AIHC_ApplyAudioBypass(AHC_AUDIO_STREAM_ID)) {

                }
            }
            else {
                if (AHC_TRUE != AIHC_ApplyAudioConfig(AHC_AUDIO_STREAM_ID, aMode)) {
                    printc("error applying audio track %d\r\n", AHC_AUDIO_STREAM_ID);
                    goto restore_video;
                }
            }
            // for live streaming (liveRTSP) to set PCM or ACC.
            // for file streaming (txRTSP) it should set it
            //   when liveRTSP_describe call netapp_parsefile, but set again in here.
            if (AHC_STREAM_ADPCM == aMode) { /* for StreamPCM */
                printc(FG_GREEN("SET audio stream IS PCM\n"));
            } else {    /* for StreamAAC */
                printc(FG_GREEN("SET audio stream IS AAC\n"));
            }
        } else if (AHC_STREAM_G711 == aMode && (VR_AUDIO_TYPE == VR_AUDIO_TYPE_AAC)) {
        #if 0   // TODO: Port G711
            AHC_ConfigAudio(AHC_AUDIO_STREAM_ID, AHC_MOVIE_AUDIO_CODEC_AAC, AHC_AUDIO_CHANNEL_MONO_R);
            AHC_ConfigAudio(AHC_AUDIO_STREAM_2_ID, AHC_MOVIE_AUDIO_CODEC_G711, AHC_AUDIO_CHANNEL_MONO_R);
            AHC_SetMovieConfig(AHC_AUDIO_STREAM_ID, AHC_AUD_MUTE_END, AHC_TRUE);
            AHC_SetMovieConfig(AHC_AUDIO_STREAM_2_ID, AHC_AUD_MUTE_END, AHC_FALSE);
            if (streamingMode & AHC_STREAM_TRANSCODE) {
                printc("Unsupported g.711 while transcoding format %X\r\n", aMode);
                goto restore_video;
            } else {
                UINT16 i;
                for (i = 0; i <= AHC_AUDIO_STREAM_2_ID; ++i) {
                    if (AHC_TRUE != AIHC_ApplyAudioConfig(i)) {
                        printc("error applying audio track %d\r\n", i);
                        goto restore_video;
                    }
                }
            }
        #endif
        } else {
            printc("Unsupported audio format %X\r\n", aMode);
            goto restore_video;
        }
    } else if (aMode == AHC_STREAM_NONE) {
        /* NOTE:
         * 1. not to stop audio for timer when in pause state
         *    because if stop timer the timestamp will restart at 0
         *    it will cause player waiting until time of streaming to
         *    paused time.
         * 2. If pause both of video and audio, it will cause player to stop or restart RTSP.
         * 3. Before stopping audio engine, it will check Audio ring and video recording status
         *    - MMPF_IsReadyToOp()
         *    Consequently, it needs stop Audio streaming first,
         *    then stop Audio Engine - MMPD_AUDIO_DoEncodeOp(ENCODE_OP_STOP).
         */
        MMPD_Streaming_Enable(AUDIO_RING_STREAM, MMP_FALSE);
        // TODO: should add PAUSE for ONLY VIDEO or AUDIO or BOTH
        // Not to STOP AUDIO in Video Recording.
        if (MMP_ERR_NONE != MMPD_AUDIO_DoLiveEncodeOp(ENCODE_OP_STOP)) {
            printc(FG_RED("[ERR] Unable to stop audio encode\r\n"));
        }
    }
    AHC_SetMovieConfig(AHC_AUDIO_STREAM_ID, AHC_AUD_STREAMING, audioEn);

    if (AHC_STREAM_MJPEG == vMode) {
        AHC_VIDEO_2ND_CONFIG cfg;
        int iVal;
        AHC_VIDRECD_RESOL AhcRes;
        int w, h;

        if ((m_AhcStreamingMode & AHC_STREAM_V_MASK) == AHC_STREAM_H264)
            AIHC_ConfigStreamingH264(AHC_FALSE); // turn OFF H264

        //support one streaming only
        AHC_SetMovieConfig(0, AHC_VIDEO_STREAMING, AHC_STREAMING_OFF);
        if (streamingMode & AHC_STREAM_TRANSCODE) {
            AHC_SetMovieConfig(1, AHC_VIDEO_STREAMING, AHC_STREAMING_TRANSCODE);
        } else {
            AHC_SetMovieConfig(1, AHC_VIDEO_STREAMING, AHC_STREAMING_ON);
        }

        if (streamingMode & AHC_STREAM_TRANSCODE) {
            MMP_ULONG fps;
            MMPS_VIDPLAY_GetCurFileFps(&fps);
            cfg.FPS = fps; //it is only known after parsing
        } else {
            //get it from current sensor setting
            cfg.FPS = AHC_GetCurSensorFPS();
            if (0 == cfg.FPS) {
                cfg.FPS = 30;
            }
        }

        cfg.vFormat = AHC_MOVIE_VIDEO_CODEC_MJPEG;

        netapp_cfg_get("Camera.Preview.MJPEG.bitrate", &cfg.param.mj.Bitrate);
        netapp_cfg_get("Camera.Preview.MJPEG.fps", &iVal);
        if (iVal != 0) {
            cfg.FpsOutx1000 = iVal * 1000;
        }

        netapp_cfg_get("Camera.Preview.MJPEG.w", &w);
        netapp_cfg_get("Camera.Preview.MJPEG.h", &h);
        if (AHC_TRUE == AIHC_MapFromRawVRes((UINT16) w, (UINT16) h, cfg.FPS, &AhcRes)) {
            cfg.param.mj.ahcRes = AhcRes;
        } else {
            cfg.param.mj.ahcRes = gAhcRes; //A resolution by UART for testing
        }

        AHC_Config2ndVideo(&cfg);
        AIHC_ApplyVideoConfig(1,netapp_getCameraSrc((unsigned int)AHC_STREAM_MJPEG));

    }
    else if (AHC_STREAM_H264 == vMode) {
        int w, h;
        
        if ((m_AhcStreamingMode & AHC_STREAM_V_MASK) == AHC_STREAM_MJPEG) {
            // AHC_SetStreamingMode(AHC_STREAM_OFF); // turn OFF MJPEG, this would turn off audio
            AHC_VIDEO_2ND_CONFIG cfg;
            cfg.FPS = 30;
            cfg.vFormat = AHC_MOVIE_VIDEO_CODEC_YUV422;
            cfg.param.mj.Bitrate = 0;//don't care
            cfg.param.mj.ahcRes = AHC_VIDRECD_RESOL_AUTO;//by LCD

            //when doing transcoding, change to no streaming, not back to YUV preview.
            if (m_AhcStreamingMode & AHC_STREAM_TRANSCODE) {
                cfg.vFormat = AHC_MOVIE_VIDEO_CODEC_NONE;
            }
            AHC_Config2ndVideo(&cfg);
            AIHC_ApplyVideoConfig(1,0);
            AHC_SetMovieConfig(0, AHC_VIDEO_STREAMING, AHC_STREAMING_OFF);
            AHC_SetMovieConfig(1, AHC_VIDEO_STREAMING, AHC_STREAMING_OFF);
        }
        //support one streaming only
        AHC_SetMovieConfig(0, AHC_VIDEO_STREAMING, AHC_STREAMING_OFF);
        AHC_SetMovieConfig(1, AHC_VIDEO_STREAMING, AHC_STREAMING_ON);

        AHC_SetMovieConfig(1, AHC_VIDEO_CODEC_TYPE, AHC_MOVIE_VIDEO_CODEC_H264);
        AHC_SetMovieConfig(1, AHC_FRAME_RATE, 30);
        netapp_cfg_get("Camera.Preview.H264.w", &w);
        netapp_cfg_get("Camera.Preview.H264.h", &h);

        w = ALIGN16(w);
        h = ALIGN16(h);
        if (AHC_TRUE != AHC_SetImageSize(VIDEO_CAPTURE_MODE, w, h)) {
            printc("Unable to set H.264 image size %dx%d\r\n", w, h);
            UNLOCK_AHC_MODE();
            return AHC_FALSE;
        }

        AIHC_ConfigStreamingH264(AHC_TRUE); // turn ON H264
    } else if (AHC_STREAM_NONE == vMode) {

        if ((m_AhcStreamingMode & AHC_STREAM_V_MASK) == AHC_STREAM_H264) {
            AIHC_ConfigStreamingH264(AHC_FALSE); // turn OFF H264
        }
        else {
            
            AHC_VIDEO_2ND_CONFIG cfg;
            
            cfg.FpsOutx1000 = 0;
            cfg.FPS = 0;
            cfg.vFormat = AHC_MOVIE_VIDEO_CODEC_YUV422;
            cfg.param.mj.Bitrate = 0;//don't care
            cfg.param.mj.ahcRes = AHC_VIDRECD_RESOL_AUTO;//by LCD
            //when doing transcoding, change to no streaming, not back to YUV preview.
            if (m_AhcStreamingMode & AHC_STREAM_TRANSCODE) {
                cfg.vFormat = AHC_MOVIE_VIDEO_CODEC_NONE;
            }
            
            AHC_Config2ndVideo(&cfg);
            AIHC_ApplyVideoConfig(1,0);
            
            AHC_SetMovieConfig(0, AHC_VIDEO_STREAMING, AHC_STREAMING_OFF);
            AHC_SetMovieConfig(1, AHC_VIDEO_STREAMING, AHC_STREAMING_OFF);
        }
    }
    else {
        UNLOCK_AHC_MODE();
        return AHC_FALSE;
    }

    m_AhcStreamingMode = streamingMode;
    UNLOCK_AHC_MODE();
    return AHC_TRUE;

restore_video:
    if (AHC_STREAM_MJPEG == vMode) {
        if (AIHC_Restore2ndVideo()) {
            MMPS_MJPEG_CloseMjpeg(&mjpeg);
        }
    }
    UNLOCK_AHC_MODE();
    return AHC_FALSE;
}

#if 0
void _____Streaming_Functions_____(){}
#endif

/*
 * For Streaming
 */
#include "AHC_Message.h"
#include "AHC_General.h"
#include "KeyParser.h"
int	sps_pps_update = 0;
#define	sps_pps_renew()	(sps_pps_update != 0)
#define	sps_pps_clean()	sps_pps_update = 0

int wait_sps_pps_renew(int tc)
{
	while (!sps_pps_renew() && tc != 0) {
		tc--;
		AHC_OS_SleepMs(100);
	}
	if (tc)
		return 1;
	return 0;
}

int AHC_OpenVideoStream(void *handle)
{
	sps_pps_clean();
	AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, VIRTUAL_EVENT, EVENT_OPEN_H264_STREAM);
	/* wait sps/pps in 2 sec */
	return wait_sps_pps_renew(20);
}

int AHC_CloseVideoStream(void *handle)
{
	AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, VIRTUAL_EVENT, EVENT_CLOSE_H264_STREAM);
	return 0;
}

#if (WIFI_PORT == 1)
//a global transcoding mode. This is a temp variable which is a bad coding style.
//It will be removed later for better modulization.
int transcoding;

#define UNDEFINED_SEM_ID (0xFF)
#define MAX_TRANSCODING_SESSION (1)
static struct AHC_TRANSCODE_CONTEXT
{
	AHC_OS_SEMID sessSem; //used for block open right after close case
	AHC_OS_SEMID opSem;   //used for block close right after open case
	                      //play and stop are not protected
	UINT32 semTimeOut;
} m_transcodeContext = {UNDEFINED_SEM_ID, UNDEFINED_SEM_ID, 5000};

INT32 AIHC_OpenTranscode(void)
{
#if (VIDEO_DEC_TO_MJPEG)
	if (m_transcodeContext.sessSem == UNDEFINED_SEM_ID) {
		///@warning assume it would not have re-entry issue here!!
		m_transcodeContext.sessSem = AHC_OS_CreateSem(MAX_TRANSCODING_SESSION);
		m_transcodeContext.opSem = AHC_OS_CreateSem(1);
	}

	if (0 == AHC_OS_AcquireSem(m_transcodeContext.sessSem, m_transcodeContext.semTimeOut)) {
		AHC_MODE_ID ahcMode = AHC_GetAhcSysMode();
		UINT16 sem_count;
		AHC_OS_QuerySem(m_transcodeContext.sessSem, &sem_count);
		if ((ahcMode != AHC_MODE_IDLE &&
			 ahcMode != AHC_MODE_NET_PLAYBACK &&
			 ahcMode != AHC_MODE_THUMB_BROWSER) ||
			sem_count >= MAX_TRANSCODING_SESSION) {
			printc(FG_RED("ERR: Transcoding could only be done in AHC IDLE mode cnt=%d!\r\n"), sem_count);
			AHC_OS_ReleaseSem(m_transcodeContext.sessSem);
			return -2;
		}
	#if MAX_TRANSCODING_SESSION > 1
		if (sem_count < (MAX_TRANSCODING_SESSION - 1)) {
			return 1; //OK
		}
	#endif

		if (0 == AHC_OS_AcquireSem(m_transcodeContext.opSem, m_transcodeContext.semTimeOut)) {
			transcoding = 1;

			MMPS_DSC_SetVideo2MJpegEnable(MMP_TRUE);
			MMPS_VIDPLAY_EnableGraphicsScaler(MMP_FALSE);
			MMPS_VIDPLAY_SetMemoryMode(MMP_FALSE);

			AHC_OS_ReleaseSem(m_transcodeContext.opSem);
		} else {
			printc(FG_RED("ERR: Cannot acquire opSem!\r\n"));
			return -3;
		}
	} else {
		//1: time out, 2: ISR
		printc(FG_RED("ERR: Transcoding timeout!\r\n"));
		return -1;
	}

#endif

	return 0;
}

///@retval -1: err.\n
/// 1: closed one session but not stop the transcoding.
/// 0: stop the transcoding.
INT32 AIHC_CloseTranscode(void)
{
#if (VIDEO_DEC_TO_MJPEG)
	UINT16 sem_count;
	AHC_OS_QuerySem(m_transcodeContext.sessSem, &sem_count);
	if (m_transcodeContext.sessSem == UNDEFINED_SEM_ID || sem_count == MAX_TRANSCODING_SESSION) {
		//check calling flow for this error.
		printc(FG_RED("ERR: Transcoding on closing! sem_cnt:%d\r\n"), sem_count);
		return -1;
	}
	#if MAX_TRANSCODING_SESSION > 1
	if (sem_count < (MAX_TRANSCODING_SESSION - 1)) {
		AHC_OS_ReleaseSem(m_transcodeContext.sessSem);
		return 1; //OK
	}
	#endif

	//close the transcoding
	AHC_OS_AcquireSem(m_transcodeContext.opSem, m_transcodeContext.semTimeOut);
	{
		MMPS_VIDPLAY_Close();
		MMPS_DSC_SetVideo2MJpegEnable(MMP_FALSE);
		MMPS_VIDPLAY_EnableGraphicsScaler(MMP_TRUE);
		transcoding = 0;
	}
	AHC_OS_ReleaseSem(m_transcodeContext.opSem);

	AHC_OS_ReleaseSem(m_transcodeContext.sessSem);
	// TODO:
	// AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, TRANSCODE_EVENT, 0);
#endif
	return 0;
}
#endif

void AHC_StreamingTimer_Start(void)
{
#if (WIFI_PORT == 1)
	MMPF_StreamingTimer_Start();
#endif
}

void AHC_StreamingTimer_Stop(void)
{
#if (WIFI_PORT == 1)
	MMPF_StreamingTimer_Stop();
#endif
}

void AHC_StreamingTimer_SetMs(MMP_ULONG ulMs)
{
#if (WIFI_PORT == 1)
	MMPF_StreamingTimer_SetMs(ulMs);
#endif
}

MMP_ULONG AHC_StreamingTimer_GetMs(void)
{
#if (WIFI_PORT == 1)
	return MMPF_StreamingTimer_GetMs();
#else
    return 0;
#endif
}

#else

AHC_BOOL VideoRecordStreamCallback(MMP_M_STREAMCB_ACTION cb_action, MMP_ULONG framesize, MMP_M_STREAM_INFO* moveinfo)
{
    return AHC_FALSE;
}

#endif  // WIFI_PORT

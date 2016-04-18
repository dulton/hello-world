/*
 * AHC_Streaming.h
 */
#if (defined(WIFI_PORT) && WIFI_PORT == 1)
extern AHC_OS_SEMID ahc_semi;
#define LOCK_AHC_MODE_NOWAIT()  AHC_OS_AcquireSem(ahc_semi, 1)
#define LOCK_AHC_MODE()         AHC_OS_AcquireSem(ahc_semi, 0)
#define UNLOCK_AHC_MODE()       AHC_OS_ReleaseSem(ahc_semi)
#define AHC_SetStreamingMode(StreamingMode) \
						_AHC_SetStreamingMode(StreamingMode, AHC_TRUE,  __MODULE__, __LINE__)
#define AHC_SetStreamingModeNoWait(StreamingMode) \
						_AHC_SetStreamingMode(StreamingMode, AHC_FALSE, __MODULE__, __LINE__)

AHC_BOOL _AHC_SetStreamingMode(AHC_STREAMING_MODE streamingMode, AHC_BOOL bWait, char * str, int line);
AHC_BOOL AHC_IsRecorderControllable(AHC_MODE_ID ahcMode);
AHC_STREAMING_MODE AHC_GetStreamingMode(void);

typedef struct _AHC_VIDEO_2ND_CONFIG 
{
    AHC_VIDEO_FORMAT 	vFormat;
    
    UINT16 				FPS; 	//As 2nd video, The FPS here now is for rate control only. The main FPS still depends on VIF.
                				//Typically usage. The sensor set to 1080P@30 or 720P@60.
                				//This field have to set to 30 or 60 accordingly. See Bitrate for further info.
    
    UINT32 				FpsOutx1000;
    
    union {
        struct {
            AHC_VIDRECD_RESOL ahcRes; //Resolution is not supported in YUV now
            
            UINT32 Bitrate;//Note that it depends on FPS field to make this working right.
                           //If the streamer skipping is used, this field must set according to the ratio of skipping.
                           //For example, now the sensor is @30FPS.
                           //             the remote end wants to get 2Mbps,30FPS MJPEG bitstream.
                           //
                           //The settings should be:
                           //Set FPS field here and MJPEG source node input FPS must set to 30.
                           //Set 10 FPS as output of MJPEG source node.
                           //Set bitrate here to 6Mbps.
                           //
                           //Then, the image flow would be like this as intention:
                           //Sensor@30FPS -> (MJPEG_HW @ 6Mbps,30FPS) -> (Streamer source @ 2Mbps, 10FPS)
        } mj;
        struct {
            UINT16 dummy;  //No further info could be set
        } yuv;
    } param;

} AHC_VIDEO_2ND_CONFIG;

typedef enum _AHC_MOVIE_STREAMING_MODE 
{
	AHC_STREAMING_OFF,
	AHC_STREAMING_ON,
	AHC_STREAMING_TRANSCODE,
	AHC_STREAMING_MAX
} AHC_MOVIE_STREAMING_MODE;

typedef enum _AHC_STREAM_ID 
{
    AHC_STREAM_FRONT = 0,
    AHC_STREAM_REAR,
    AHC_STREAM_MAX
} AHC_STREAM_ID;

AHC_BOOL 	AHC_Config2ndVideo(AHC_VIDEO_2ND_CONFIG *cfg);

void        AHC_StreamingTimer_Start(void);
void        AHC_StreamingTimer_Stop(void);
void        AHC_StreamingTimer_SetMs(MMP_ULONG ulMs);
MMP_ULONG   AHC_StreamingTimer_GetMs(void);

#else   // WIFI_PORT

#define	AHC_GetStreamingMode()	(AHC_STREAM_OFF)
#define	AHC_SetStreamingMode(m) (AHC_FALSE)
#define LOCK_AHC_MODE()
#define UNLOCK_AHC_MODE()

#endif	// WIFI_PORT

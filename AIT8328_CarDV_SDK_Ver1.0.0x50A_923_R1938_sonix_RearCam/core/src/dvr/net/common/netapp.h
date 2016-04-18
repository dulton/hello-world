/*
 * netapp.h
 */
#ifndef	__NETAPP_H__
#define	__NETAPP_H__

#include "os_wrap.h"
//Because network module includes this file.
//Try not to use AHC files or even MMPS/MMPD files for better portability
//#include "AHC_WarningMSG.h"

#define	NETAPP_WIFI_NONE		0xf0000000
#define	NETAPP_WIFI_FAIL		0xe0000000
#define	NETAPP_WIFI_INIT		0
#define	NETAPP_WIFI_IDLE		1
#define	NETAPP_WIFI_READY		2
#define	NETAPP_WIFI_NOIP		3
#define	NETAPP_WIFI_HTTP		4
#define	NETAPP_WIFI_DOWN		5
#define	NETAPP_STREAM_MASK		0x000000f0
#define	NETAPP_STREAM_FAIL		0x80000000
#define	NETAPP_STREAM_TEARDOWN	0x00000020
#define	NETAPP_STREAM_SETUP		0x00000030
#define	NETAPP_STREAM_PLAY		0x00000040
#define	NETAPP_STREAM_STATUS(s)			(s & NETAPP_STREAM_MASK)

#define IS_WIFI_FAIL(s)			((s & NETAPP_WIFI_NONE) == NETAPP_STREAM_FAIL)

#define	NETAPP_DONE	(-1)	// finish job, not to do any more after return.
#define	NETAPP_FAIL	(-2)
#define	NETAPP_CONT	(0)

#define	NETAPP_WIFI_AP_MODE			1
#define	NETAPP_WIFI_STATION_MODE	2
#ifdef __AMN_OS_LINUX__
#define set_NetAppStatus(s)
#else
#undef	_NETAPP_DEBUG
#ifndef	_NETAPP_DEBUG
int	set_NetAppStatus(int status);
int get_NetAppStatus(void);
#else
int	set_NetAppStatusDB(int status, char *file, int line);
#define	set_NetAppStatus(s)	set_NetAppStatusDB(s, __FILE__, __LINE__)
#endif
#endif

extern MMPF_OS_FLAGID	Network_Flag;
#define	SEND_NETWORK_EVENT(e)	MMPF_OS_SetFlags(Network_Flag, e, MMPF_OS_FLAG_SET)
#define	IS_CAMERA_IN_HDMI()	(strcmp(netapp_get_TVStatus(), "NONE") != 0)

//==== declaration for lwip_main.c ====
typedef void *(*job_routine)(void *arg);
void* add_NetworkJob(job_routine routine, void *arg, /*MMP_BOOL*/unsigned char bneedlock);
void kick_NetworkJob(void);
int  network_init_done(void);
//==== end of lwip_main.c ====


int Reset_Network(void);
int TurnOnWiFiModule(void);
int TurnOffWiFiModule(void);
/* Wireless driver handle. */
typedef void* WLAN_DRV_HDL;
void netapp_setHwHandle(WLAN_DRV_HDL h);
WLAN_DRV_HDL* netapp_getHwHandle(void);
int Netapp_wifi_link_report(int updown);
int NetApp_SetStreaming(short enable);
int NetApp_GetRecordStatus(void);
int NetApp_GetCaptureStatus(void);
int Netapp_get_cur_wifi_mode(void);
int Netapp_get_target_wifi_mode(void);
int Netapp_set_wifi_mode(int mode);
int Netapp_get_status(void);
int Netapp_set_status(int status);
//int netapp_stabroadcast_receive_start(void);
int netapp_warning_msg_state(void);
int netapp_get_warning_msg_info(void);
//m_WMSG_Info mapped to type WMSG_INFO
int set_netapp_WMSGInfo(int m_WMSG_Info);
int netapp_get_TimeStamp_year(void);
int netapp_get_TimeStamp_month(void);
int netapp_get_TimeStamp_day(void);
int netapp_get_TimeStamp_hour(void);
int netapp_get_TimeStamp_minute(void);
int netapp_get_TimeStamp_second(void);
typedef	int  (*PFN_WEBWAIT)(void*, void*);
typedef void (*PFN_CGI_OP_CALLBACK)(void*, int);
int netapp_CGIOpCallback(PFN_CGI_OP_CALLBACK pfn);
int NetApp_SetKeyevent(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s);
#undef	CGIOP_DBG
#ifdef	CGIOP_DBG
int netapp_CGIOpFeedbackDB(void *handle, int result, char *, int);
#define netapp_CGIOpFeedback(h, r) netapp_CGIOpFeedbackDB(h, r, __func__, __LINE__)
#else
int netapp_CGIOpFeedback(void *handle, int result);
#endif

//
const char* netapp_get_Video_Resolution(void);
const char* netapp_get_VideoClipTime(void);
const char* netapp_get_Image_Resolution(void);
const char* netapp_get_WB(void);
const char* netapp_get_Flicker_Hz(void);
const char* netapp_get_MTD_Level(void);
const char* netapp_get_EV_Level(void);
const char* netapp_get_PowerOffDelay(void);
const char* netapp_get_TVSystem(void);
const char* netapp_get_TVStatus(void);
const char* netapp_get_SD0Status(void);
#define	FORMAT_SD_NONE	0
#define	FORMAT_SD_DOING	1
#define	FORMAT_SD_DONE	2
#define	FORMAT_SD_ERROR	3
int netapp_set_SD0Status(int status);

const char* netapp_get_FW_Version(void);
int netapp_get_MjpegBitrate(void);
int netapp_get_H264Bitrate(void);
int netapp_Enableh264Stream(int bEnabled);
int netapp_cfg_get(char* key, void* val);
int netapp_set_FRC(int inx1000, int outx1000, char* name);
int netapp_set_framerate(int fps, char* name);
int netapp_get_MjpegResolution(unsigned short *w, unsigned short *h, char *name);
int netapp_InitNetwork(void);
int netapp_StopNetwork(void);
int netapp_ResetNetwork(void);
int netapp_CmdNetwork(void);
int netapp_DeleteFile(char *path);
const char* netapp_GetUIState(void);
const char* netapp_GetUIStateEx(int uId);
const char* netapp_GetDefMode(void);
void netapp_SendCameraState(void);
//
#define	NOTIFY_CAPTURE			1
#define	NOTIFY_TIMELAPSE		2
#define	NOTIFY_TIMELAPSE_STOP	3
#define	NOTIFY_FWUPDATED		4

void* netapp_UINotifyRoll(int reason, void *data);
int netapp_UINotifyChanged(void*);
//
int netapp_BroadcastSSID(int enable);
int netapp_flashFW(char *filename, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s);

int netapp_SaveNetworkConf(void);
int netapp_stopTranscoding(void);
int netapp_closeTranscoding(void);
//
const char* netapp_get_PhotoBurst(void);
const char* netapp_get_Timelapse(void);
const char* netapp_get_Upsidedown(void);
const char* netapp_get_HDR(void);
const char* netapp_get_LoopingVideo(void);
const char* netapp_get_SpotMeter(void);
const char* netapp_get_Q_SHOT(void);
const char* netapp_get_StatusLights(void);
const char* netapp_get_SoundIndicator(void);
const char* netapp_get_PowerSaving(void);
int netapp_get_RecordTimeTotal(void);
int netapp_get_RecordTimeStart(void);
int netapp_get_RecordTimeRemaining(void);
int netapp_get_RecordTimeAvailable(void);
int netapp_get_CaptureRemaining(void);
unsigned long netapp_getStreaming_Timestamp(void);
int netapp_get_PowerLevel(void);
void netapp_set_LdwsResult(int val);
char* netapp_get_LdwsResult(void);
int netapp_query_Streamable(int streamtype);
int netapp_query_Flashable(void);
int netapp_uploadprogress(unsigned int cursize, unsigned int total);
int netapp_getAudioSampleRate(void);
int netapp_switchCamera(void);
int netapp_getCameraSrc(unsigned int vMode);
int netapp_getRearcamReady(void);
void netapp_setRearcamReady(int state);

void netapp_getWiFiMacAddr(MMP_UBYTE* addr);
//
#define	netapp_getStreamingMode()	AHC_GetStreamingMode()

/*
 * Get which SET for WiFi AP used.
 * return 0 or 1 for AP configuration set 0 or 1
 */
int netapp_GetWiFi_AP_set(void);

/*
 * return 1: AP mode
 *        2: STA mode
 */
int netapp_GetWiFiMode(void);
/*
 * return unsigned int (little endian) for ipv4
 *        ip: xxx.xxx.xxx.xxx = byte0.byte1.byte2.byte3
 */
unsigned int netapp_GetIPAddress(void);
void netapp_Pump(void);
void netapp_DateTime_Parser(int* datetime, char* timestr);
int netapp_update_AudioSamplingRate(int transcode);
unsigned int netapp_get_streaming_timer(void);
int netapp_parsefile(char *file_name, int video_format, int audio_format);
const char* netapp_SensorStatus(void);
void start_station_service(void);
void stop_station_service(void);

enum VALUE_TYPE {
    VALUE_TYPE_INT, // 4 bytes integer
    VALUE_TYPE_STR, // c string
    VALUE_TYPE_PTR, // pointer
    VALUE_TYPE_2_INT, // 2 2-bytes integers. example use: resolution
    VALUE_TYPE_4_BYTES, // 4 1-byte integers. example use: RGBA
    VALUE_TYPE_UNDEFINED,
    VALUE_TYPE_MAX
};

typedef struct {
    enum VALUE_TYPE type;
    union {
        int i32;
        void* ptr;
        char* str;
        short i16[2];
        char i8[4];
    } v;
} NETAPP_VALUE;

enum NETAPP_DEVICE_SETTING {
    NADS_AUDIO_SAMPLING_RATE, //VALUE_TYPE_INT
    NADS_AUDIO_PREVIEW,//VALUE_TYPE_INT, audio on or off in preview
    NADS_TRANSCODE_FILE_NAME,//VALUE_TYPE_STR
    NADS_FILE_DURATION, //VALUE_TYPE_INT, file duration in milli-seconds, (-1L) means undefined
		#define NADS_FILE_DURATION_UNDEFINED (-1L)
    NADS_MAX
};

int netapp_get_DeviceSetting(enum NETAPP_DEVICE_SETTING setting, NETAPP_VALUE* v);
int netapp_set_DeviceSetting(enum NETAPP_DEVICE_SETTING setting, NETAPP_VALUE* v);
// To protect between GET-VIDEO-TUMBNAIL and CAPTURE/BURST/TIMELAPSE to avoid system crashing.
#define	LOCK_CAPTURE			1
#define	UNLOCK_CAPTURE			0
#define	LOCK_CAPTURE_TIMEOUT	8000
#undef	LOCK_DEBUG
#ifdef	LOCK_DEBUG
#define	netapp_CaptureLock(e, t)	netapp_CaptureLockDBG(e, t, __func__, __LINE__)
int netapp_CaptureLockDBG(int enLock, unsigned int timeout, char *f, int l);
#else
int netapp_CaptureLock(int enLock, unsigned int timeout);
#endif

//
// mi Media API
//
void* miOpenThumbnail(char *filename);
void miCloseThumbnail(void *handle);
int miGetThumbnail(void *handle, unsigned char **jpegbuf);
int miGetJPEGThumbnail(char *filename, unsigned char *pbuf, int bufsize, int *width, int *height);
int miGetVideoThumbnail(char *filename, unsigned char *pbuf, int bufsize, int *width, int *height);
int miCreateJPEGThumbnail(char *filename, unsigned char *pbuf, int bufsize, int *width, int *height);

typedef struct {
	int		type;
	int		width;
	int		height;
	int		samplecount;
	int		timescale;
	int		duration;
	int		fps;
} MI_INFO;
int miGetMediaInfo(char *filename, MI_INFO *miInfo);
//
// FILE Enum
#include "fs_api.h"
typedef struct {
	unsigned int	tot;
	unsigned int	idx;
	unsigned int	flen;
	int				fmt;	// AVI: 2, JPEG: 0
	int				rw;		// rd only:1, rw: 0
	void			*data;
	FS_FILETIME		ft;
} FFINST, *PFFINST;
/*
 * There is same declarations in AHC_DCF.h too.
 */
typedef int (*FFN)(void *hinst, char *inout, void* attr, void* arg); 
typedef struct {
	FFN		ffn;
	void	*arg;
} FFFILTER_FN;
//
typedef struct {
	unsigned short	grpid;	// Group ID
	unsigned short	grpno;	// Group File No
	unsigned short	grpto;	// Group File total
	unsigned short	dkey;
	unsigned short	fkey;
	unsigned char	grpmo;	// Group File mode as created
} FILE_GRPINFO;
#define	GROUP_FLAG		(0x80)
#define	ALL_TYPE		(0x7f)
void* FindFirstFile(char *inout, int bufsize, int *retsize, FFFILTER_FN* pfilter);
int FindNextFile(void *hinst, char *out, int bufsize, int *retsize, FFFILTER_FN* pfilter);
int FindFileGroup(void *hinst, FILE_GRPINFO *pinf);
int GetFrontFilename(char* front);


int wildstrcmp(char *s1, const char* ws2, int ws2len);
int Report2UIAndWaitFeedBack(MMP_ULONG msg, MMP_ULONG param1, PFN_WEBWAIT pfn_wait, void *arg);
int NetApp3_SetKeyevent(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s);

/*
 * functions in AHC
 */
__weak void AHC_DrawNetFwDownloadProgress( int uiDownloadPercentage );

#endif	// __NETAPP_H__

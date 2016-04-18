//
// netapp2.c
//
#include "AHC_MENU.h"
#include "AHC_General.h"
#include "AHC_Stream.h"
#include "AHC_Display.h"	// CarDV - Aaron
#include "LedControl.h"
#include "upnp.h"
#include "upnpapi.h"
#include "inet_pton.h"
#include "sockets.h"
#include "sock.h"
#include "netapp.h"
#include "MenuSetting.h"
#include "amn_osal.h"
#include "ucos_osl.h"
#include "ucos_osl_ext.h"
#include "netapp.h"
#include "mmpd_scaler.h"
#include "mmpf_dsc.h"
#include "amn_event.h"

#define	PREOID_TO_BROADCAST	(2 * 1000)	// 2 seconds
#define	TIMES_UI_BROADCAST	3
#define	PACK_VERSION		100
#define	BROADCAST_TIME_OUT	(TIMES_UI_BROADCAST * PREOID_TO_BROADCAST)
static unsigned int	bc_uiticket = 0;
typedef	struct {
	int				ticket;
	int				times;
	int				version;
	time_t			unixtime;
	osl_ext_sem_t	sem;
	MMPF_OS_TMRID	tid;
	void	*data;
} BROADCAST_PACK;
static BROADCAST_PACK	bcpack = {0};
void Broadcast_UIChanged(void *p1, void* p2);

typedef struct {
	int		reason;
	int		shots;
	int		datalen;
	char	data[32];
} notify_reason_t;
static notify_reason_t _nrson = { 0 };
MMP_ULONG MMPF_StreamingTimer_GetMs(void);
void* netapp_UINotifyRoll(int reason, void *data)
{
	if (_nrson.reason != 0) {
		printc(BG_RED("%s %d: There is a notification (%d) not to send yet!!!\n"),
					__func__, __LINE__, _nrson.reason);
		return NULL;
	}
	switch (reason) {
	case NOTIFY_CAPTURE:
	case NOTIFY_TIMELAPSE:
		_nrson.reason = reason;
		_nrson.shots  = 1;
		sprintf(_nrson.data, "%u", (unsigned int)MMPF_StreamingTimer_GetMs());
		break;
	case NOTIFY_TIMELAPSE_STOP:
		_nrson.reason = reason;
		_nrson.shots  = 1;
		sprintf(_nrson.data, "%s", "STOP");
		break;
	case NOTIFY_FWUPDATED:
		_nrson.reason = reason;
		_nrson.shots  = 1;
		sprintf(_nrson.data, "%s", ((int)data == 0)? "done" : "failed");
		break;
	}
	return &_nrson;
}
/*
 * netapp_UINotifyChanged
 * Call by UI when something changed, like as settings,
 * start to video record/continuous snapshot by key
 */
int netapp_UINotifyChanged(void* somethingchanged)
{
	unsigned int		netstatus;
	notify_reason_t*	reas;
	
	netstatus = get_NetAppStatus();
	if (netstatus == NETAPP_WIFI_NONE ||
		netstatus == NETAPP_WIFI_INIT ||
		netstatus == NETAPP_WIFI_DOWN) {
		// network isn't ready, not to do anything!!
		return 0;
	}
	if (bcpack.sem == 0) {
		if (osl_ext_sem_create(NULL, 1, &bcpack.sem) != OSL_EXT_SUCCESS) {
			printc("%s %d: Create Sem ERROR\n", __func__, __LINE__);
		}
	}
	// Check broadcast running and stop it
	// Avoid netapp_SendCameraState called druing setup
	if (bcpack.sem != 0 &&
		osl_ext_sem_take(&bcpack.sem, BROADCAST_TIME_OUT) != OSL_EXT_SUCCESS) {
		printc(BG_RED("%s %d: Sem Timeout or ERROR\n"), __func__, __LINE__);
	}
	if (bcpack.tid != 0) {
		MMPF_OS_StopTimer(bcpack.tid, OS_TMR_OPT_NONE);
	}
	reas = (notify_reason_t*)somethingchanged;
	bcpack.tid	   = 0;
	bcpack.ticket  = ++bc_uiticket;
	bcpack.unixtime= time(NULL);
	bcpack.version = PACK_VERSION;
	bcpack.times   = (reas == NULL)? TIMES_UI_BROADCAST - 1 : reas->shots - 1;
	bcpack.data    = (void*)reas;

	Broadcast_UIChanged(NULL, &bcpack);
	if (bcpack.sem != 0)
		osl_ext_sem_give(&bcpack.sem);
	return 0;
}
/*
 * 
 */
void Broadcast_UIChanged(void *p1, void* p2)
{
	SEND_NETWORK_EVENT(SYS_FLAG_MSG_NETWORK_UINOTIFY);
}

static unsigned int	checksum(char* buf)
{
	unsigned int	i, sum;
	
	for (i = 0, sum = 0; *(buf + i) != 0; i++)
		sum += *(buf + i);
	return sum;
}
/*
 *
 */
#define	BROADCAST_BUFFER_MAXLEN	1024
static int collectChanged(BROADCAST_PACK *ppack, char **buf)
{
	char			*list;
	int				len, maxlen;
	unsigned int	ip;
	#define	N(p, m) (*(((unsigned char*)(&p)) + m))

	ip = netapp_GetIPAddress();
	
	if (buf == NULL) return 0;
	maxlen = BROADCAST_BUFFER_MAXLEN;
	*buf = NULL;
	list = osal_malloc(maxlen);
	if (list == NULL) return 0;
	len  = 0;
	len += snprintf(list + len, maxlen - len, "IP=%u.%u.%u.%u\n", N(ip, 0), N(ip, 1), N(ip,2), N(ip, 3));
	if (ppack->data != NULL) {
		notify_reason_t*	reas = (notify_reason_t*)ppack->data;
		switch(reas->reason) {
		case NOTIFY_CAPTURE:
			len += snprintf(list + len, maxlen - len, "Capturing=%s\n", reas->data);
			break;
		case NOTIFY_TIMELAPSE:
		case NOTIFY_TIMELAPSE_STOP:
			len += snprintf(list + len, maxlen - len, "Timelapse=%s\n",	reas->data);
			break;
		case NOTIFY_FWUPDATED:
			len += snprintf(list + len, maxlen - len, "fwflash=%s\n", reas->data);
			break;
		}
		reas->reason = 0;	// Clean, allow next to send
	} else {
		len += snprintf(list + len, maxlen - len,
						"ver=%d\n",    	ppack->version);
		len += snprintf(list + len, maxlen - len,
						"ticket=%u\n",	ppack->ticket);
		len += snprintf(list + len, maxlen - len,
						"time=%u\n",	ppack->unixtime);
		len += snprintf(list + len, maxlen - len,
						"Videores=%s\n", netapp_get_Video_Resolution());
		len += snprintf(list + len, maxlen - len,
						"Imageres=%s\n", netapp_get_Image_Resolution());
		len += snprintf(list + len, maxlen - len,
						"UIMode=%s\n", netapp_GetUIState());
		len += snprintf(list + len, maxlen - len,
						"AWB=%s\n",		netapp_get_WB());
		len += snprintf(list + len, maxlen - len,
						"Flicker=%s\n",	netapp_get_Flicker_Hz());
		len += snprintf(list + len, maxlen - len,
						"Timelapse=%s\n",	netapp_get_Timelapse());
		len += snprintf(list + len, maxlen - len,
						"PhotoBurst=%s\n",	netapp_get_PhotoBurst());
		len += snprintf(list + len, maxlen - len,
						"Upsidedown=%s\n",	netapp_get_Upsidedown());
		len += snprintf(list + len, maxlen - len,
						"HDR=%s\n",	netapp_get_HDR());
		len += snprintf(list + len, maxlen - len,
						"LoopingVideo=%s\n",netapp_get_LoopingVideo());
		len += snprintf(list + len, maxlen - len,
						"SpotMeter=%s\n",	netapp_get_SpotMeter());
		len += snprintf(list + len, maxlen - len,
						"Q-SHOT=%s\n",	netapp_get_Q_SHOT());
		len += snprintf(list + len, maxlen - len,
						"StatusLights=%s\n",netapp_get_StatusLights());
		len += snprintf(list + len, maxlen - len,
						"SoundIndicator=%s\n",	netapp_get_SoundIndicator());
		len += snprintf(list + len, maxlen - len,
						"PowerSaving=%s\n",	netapp_get_PowerSaving());
		len += snprintf(list + len, maxlen - len,
						"TVSystem=%s\n",	netapp_get_TVSystem());
		len += snprintf(list + len, maxlen - len,
						"EV=%s\n",		netapp_get_EV_Level());
		len += snprintf(list + len, maxlen - len, "Recording=%s\n",
						NetApp_GetRecordStatus()? "YES" : "NO");
		len += snprintf(list + len, maxlen - len, "Streaming=%s\n",
						(netapp_getStreamingMode())? "YES" : "NO");
		len += snprintf(list + len, maxlen - len,
						"TV=%s\n", netapp_get_TVStatus());
		len += snprintf(list + len, maxlen - len,
						"SD0=%s\n", netapp_get_SD0Status());
		len += snprintf(list + len, maxlen - len,
						"Power=%d\n", netapp_get_PowerLevel());
		len += snprintf(list + len, maxlen - len,
						"LDWS=%s\n", netapp_get_LdwsResult());
	}	
	// end of data, add new status before this line
	len += snprintf(list + len, maxlen - len, "CHKSUM=%u", checksum(list));
	if (len > maxlen) {
		printc(BG_RED("FATAL ERROR! %s %d: need len %d but MAXLEN %d\n"),
						__func__, __LINE__, len, maxlen);
	}
	*buf = list;
	return len;
}
/*
 * netapp_SendCameraState()
 */
#define BROADCAST_IP   "255.255.255.255"
#define BROADCAST_PORT 49142
void netapp_SendCameraState()
{
	SOCKET brsock;
	struct sockaddr_storage __ss;
	struct sockaddr_in *DestAddr4 = (struct sockaddr_in *)&__ss;
	socklen_t socklen = sizeof(struct sockaddr_storage);
	unsigned long replyAddr = inet_addr(gIF_IPV4);
	int ttl = 4;
	ssize_t rc;
	char			*data;
	unsigned int	len;
	BROADCAST_PACK	*ppack = &bcpack;
	
	// Avoid netapp_UINotifyChanged called druing broadcasting
	if (ppack->sem != 0 &&
		osl_ext_sem_take(&ppack->sem, BROADCAST_TIME_OUT) != OSL_EXT_SUCCESS) {
		printc("%s %d: Sem Timeout or ERROR\n", __func__, __LINE__);
	}
	len = collectChanged(ppack, &data);
	if (len == 0) {
		printc("ERROR: %s %d: Lenght %d\n", __func__, __LINE__, len);
		goto end_SendCameraState;
	}
	memset(&__ss, 0, sizeof(__ss));
	DestAddr4->sin_family = (sa_family_t)AF_INET;
	inet_pton(AF_INET, BROADCAST_IP, &DestAddr4->sin_addr);
	DestAddr4->sin_port = htons(BROADCAST_PORT);

	brsock = net_socket((int)DestAddr4->sin_family, SOCK_DGRAM, 0);
	if (brsock == INVALID_SOCKET) {
		goto end_SendCameraState;
	}
	net_setsockopt(brsock, IPPROTO_IP, IP_MULTICAST_IF,
		   (char *)&replyAddr, sizeof(replyAddr));
	net_setsockopt(brsock, IPPROTO_IP, IP_MULTICAST_TTL,
		   (char *)&ttl, sizeof(int));
	socklen = sizeof(struct sockaddr_in);
	rc = net_sendto(brsock, (const void*)data,
		    len, 0, (const struct sockaddr *)DestAddr4, socklen);
	if (rc == -1) {
		goto end_NewRequestHandler;
	}

 end_NewRequestHandler:
	if (net_shutdown(brsock, SD_BOTH) == -1) {
	}
	net_close(brsock);
	//
	// Trigger next
	if (ppack->tid != 0) {
		MMPF_OS_StopTimer(ppack->tid, OS_TMR_OPT_NONE);
        ppack->tid = 0;
	}
	// Trigger for next broadcasting
	if (ppack->times != 0) {
		ppack->times--;
		//printc("%s %d: %d\n", __func__, __LINE__, ppack->times);
		ppack->tid = MMPF_OS_StartTimer(PREOID_TO_BROADCAST,
										OS_TMR_OPT_ONE_SHOT,
										Broadcast_UIChanged,
										ppack);
		//printc("%s %d: NEXT timer id %u\n", __func__, __LINE__, ppack->tid);
		if ((ppack->tid & 0xFE) == 0xFE) {
			printc("FATAL ERROR:%s %d Timer failed!\n", __func__, __LINE__);
		}
	}
end_SendCameraState:
	if (data) osal_free(data);
	if (bcpack.sem != 0)
		osl_ext_sem_give(&bcpack.sem);
	//
	return;
}

extern void acquire_cgi_Callback(void);
extern void release_cgi_Callback(void);

static PFN_CGI_OP_CALLBACK pfn_CGIop_callback = NULL;
int netapp_CGIOpCallback(PFN_CGI_OP_CALLBACK pfn)
{
	acquire_cgi_Callback();
	pfn_CGIop_callback = pfn;
	return 0;
}

/*
 * Call by UI/AHC, after finished the CGI request to call this function
 * with the result.
 * handle is a value of enum WIRELESS_CONNECT_MESSAGE
 * result: < 0 - failed, others - success
 */
#ifdef	CGIOP_DBG
int netapp_CGIOpFeedbackDB(void *handle, int result, char *callfile, int line)
#else
int netapp_CGIOpFeedback(void *handle, int result)
#endif
{
#ifdef	CGIOP_DBG
	printc(FG_YELLOW("%s: CALL from %s %d\n"), __func__, callfile, line);
#endif
	if (pfn_CGIop_callback) {
		printc("%s %d:Handle %d RESULT %d\n", __func__, __LINE__, (int)handle, result);
		pfn_CGIop_callback(handle, result);
		pfn_CGIop_callback = NULL;
		release_cgi_Callback();
		return 1;
	}
	return 0;
}


/*******************************************************************
 *
 * Miscellaneous for NETWORK
 *
 *******************************************************************/
#include "mmpf_exif.h"
#include "mmp_dsc_inc.h"
#include "mmpd_dsc.h"
#include "mmpf_fs_api.h"
#include "amn_osal.h"
#include "amn_sysobjs.h"
//
extern MMP_ULONG m_ulMediaFileNameAddr;
extern MMP_ULONG m_ulDSCFileID;
extern MMP_ULONG m_ulDscFileSize;

extern MMP_ERR MMPF_DSC_OpenJpegDecFile(void);
extern MMP_ERR MMPF_DSC_CloseJpegDecFile(void);
extern MMP_ERR MMPF_DSC_ReadJpegFile(MMP_UBYTE *ubTarget, MMP_ULONG ulSeekpos, MMP_ULONG ulReadsize);
extern MMP_ERR MMPF_EXIF_GetThumbInfo(MMP_DSC_JPEG_INFO 	*pJpegInfo,  
									  JpegFileReadCBFunc	*CallBackFunc);
MMP_ERR	MMPF_DSC_ScanJpegMarker(MMP_ULONG 			JpegStartOffset, 
								MMP_ULONG 			JpegEndOffset,
								MMP_USHORT 			*usWidth, 
								MMP_USHORT 			*usHeight, 
                    			MMP_DSC_JPEG_FMT 	*pFormat);
MMP_ERR MMPD_DSC_GetBufferSize(	MMPD_DSC_BUF_TYPE 	ubBufType, 
							    MMP_DSC_JPEG_FMT 	jpegformat,
								MMP_USHORT 			usWidth, 
								MMP_USHORT 			usHeight, 
								MMP_ULONG			*ulSize);
MMP_UBYTE* MMPF_DSC_MallocBuf(MMP_USHORT usExifNodeId, MMP_ULONG ulSize, MMP_BOOL bForUpdate);
MMP_ERR	MMPF_DSC_SetDecodeInputBuf(MMP_ULONG ulDataTmpAddr, MMP_ULONG ulDataTmpSize,
                                   MMP_ULONG ulBSBufAddr, MMP_ULONG ulBSBufSize);
#define	MI_FILE_JPEG		0
#define	MI_FILE_VIDEO		1
typedef struct {
	char			file[64];
	int				type;
	int				width;
	int				height;
	int				bufsize;
	int				jsize;
	unsigned char	*jbuf;
} MI_THUMBNAIL;
#define	MAX_JPEG_SIZE		(32*1024)
int miGetJPEGThumbnail(char *filename, unsigned char *pbuf, int bufsize, int *width, int *height);
int miGetVideoThumbnail(char *filename, unsigned char *pbuf, int bufsize, int *width, int *height);
/*
 * Give a filename and buf/size, return thumbnail of jpeg, width,height and size.
 * Caller has to free *pbuf by osal_free if *pbuf is NULL and bufize is 0.
 */
void* miOpenThumbnail(char *filename)
{
	MI_THUMBNAIL	*pmi;
	char *p;
	
	pmi = NULL;
	// Check File Type
	p = strrchr(filename, '.');
	if (p == NULL) {
		return NULL;
	}
	pmi = (MI_THUMBNAIL*)osal_malloc(MAX_JPEG_SIZE);
	if (pmi == NULL) {
		return NULL;
	}
	pmi->bufsize = MAX_JPEG_SIZE - sizeof(MI_THUMBNAIL);
	pmi->jsize   = 0;
	pmi->jbuf    = (unsigned char*)pmi + sizeof(MI_THUMBNAIL);
	pmi->type    = (stricmp(p + 1, "JPG") == 0)? MI_FILE_JPEG : MI_FILE_VIDEO;
	strncpy(pmi->file, filename, sizeof(pmi->file));
	if (pmi->type == MI_FILE_JPEG) {
		pmi->jsize = miGetJPEGThumbnail(pmi->file, pmi->jbuf, pmi->bufsize,
										&pmi->width, &pmi->height);
		if (pmi->jsize > 0) {
			goto END_miOpenThumbnail;
		}
		// there is no thumbnail in file, generate one
		pmi->width  = 160;
		pmi->height = 120;
		if (netapp_CaptureLock(LOCK_CAPTURE, LOCK_CAPTURE_TIMEOUT) != 0) {
			goto END_miOpenThumbnail;
		}
		pmi->jsize = miCreateJPEGThumbnail(pmi->file, pmi->jbuf, pmi->bufsize,
										   &pmi->width, &pmi->height);
		netapp_CaptureLock(UNLOCK_CAPTURE, 0);
	} else {
		if (netapp_CaptureLock(LOCK_CAPTURE, LOCK_CAPTURE_TIMEOUT) != 0) {
			goto END_miOpenThumbnail;
		}
		pmi->jsize = miGetVideoThumbnail(pmi->file, pmi->jbuf, pmi->bufsize,
										&pmi->width, &pmi->height);
		netapp_CaptureLock(UNLOCK_CAPTURE, 0);
	}
END_miOpenThumbnail:
	if (pmi->jsize <= 0) {
		osal_free(pmi);
		pmi = NULL;
	}
	return (void*)pmi;
}

void miCloseThumbnail(void *handle)
{
	if (handle == NULL)
		return;
	osal_free(handle);
}

/*
 * Give a filename and output a buffer of thumbnail
 */
int miCreateJPEGThumbnail(char *filename, unsigned char *pbuf, int bufsize, int *width, int *height)
{
	MMP_DSC_JPEG_INFO       jpeginfo;
    MMP_GRAPHICS_BUF_ATTR   BufAttr;
    MMP_ERR				    err;
    MI_INFO				    miinfo;
    unsigned short		    w, h;
	MMP_ULONG			    ulJpegSize;
	AHC_MODE_ID			    save;
	int					    ret;
    
 	if (miGetMediaInfo(filename, &miinfo) == 0) {
 		return 0;
 	}
	ret = 0;
	AHC_SetStreamingMode(AHC_STREAM_PAUSE);

	save = AHC_GetAhcSysMode();
	AHC_SetMode(AHC_MODE_IDLE);
	//
 	w = (*width  + 15) & (~0xf);
 	h = (miinfo.height * (*width) / miinfo.width + 15) & (~0xf);
 	memset(&jpeginfo, 0, sizeof(MMP_DSC_JPEG_INFO));
    strcpy(jpeginfo.bJpegFileName, filename);
    jpeginfo.usJpegFileNameLength   = strlen(filename); //+1;
    jpeginfo.ulJpegBufAddr          = 0;
    jpeginfo.ulJpegBufSize          = 0;
	jpeginfo.bDecodeThumbnail       = MMP_FALSE;
	#if (DSC_SUPPORT_BASELINE_MP_FILE)
	jpeginfo.bDecodeLargeThumb		= MMP_TRUE;
	#endif
    jpeginfo.bValid                 = MMP_FALSE;

    err = MMPS_DSC_DecodeThumbFirst(&jpeginfo, 
                                    w,
                                    h,
                                    MMP_DISPLAY_COLOR_YUV422);

	if (err != MMP_ERR_NONE) {
	 	printc(BG_RED("%s %d\n"), __func__, __LINE__);
		goto End_miCreateJPEGThumbnail;
	}

    MMPS_DSC_GetJpegDispBufAttr(&BufAttr);

	/* for debug -- Write raw to file
	printc(FG_YELLOW("Write to 001.raw %dx%d\n"), BufAttr.usWidth, BufAttr.usHeight);
	MMPD_DSC_SetFileName("SD:\\001.raw", strlen("SD:\\001.raw"));
	MMPD_DSC_JpegDram2Card(BufAttr.ulBaseAddr,
						   BufAttr.usWidth * BufAttr.usHeight,
						   MMP_TRUE, MMP_TRUE);
	*/

	{    
    MMP_SCAL_FIT_RANGE  fitrange;
    MMP_SCAL_GRAB_CTRL  grabctl;
	MMP_DSC_CAPTURE_BUF capturebuf;
	MMP_GRAPHICS_RECT   rect;	
	MMP_ULONG           ulSramCurPos = 0x104000;
	MMPD_FCTL_ATTR 		fctlAttr;
	MMPD_FCTL_LINK m_fctlLinkCapture = {
			MMP_SCAL_PIPE_0,
			MMP_ICO_PIPE_0,
			MMP_IBC_PIPE_0
	};
	extern MMP_ERR MMPD_System_GetSramEndAddr(MMP_ULONG *ulAddress);

	MMPD_System_GetSramEndAddr(&ulSramCurPos);
    capturebuf.ulCompressStart  = BufAttr.ulBaseAddr + ALIGN32(BufAttr.usWidth * 2) * BufAttr.usHeight;
    capturebuf.ulCompressEnd    = ((capturebuf.ulCompressStart + jpeginfo.usPrimaryWidth * 2 * jpeginfo.usPrimaryHeight)>>4)<<4;
	capturebuf.ulLineStart      = ulSramCurPos;
    MMPD_DSC_SetCaptureBuffers(&capturebuf);

    rect.usLeft     		= 0;
    rect.usTop      		= 0;
    rect.usWidth    		= BufAttr.usWidth;
    rect.usHeight   		= BufAttr.usHeight;

    fitrange.fitmode     	= MMP_SCAL_FITMODE_OUT;
    fitrange.scalerType		= MMP_SCAL_TYPE_SCALER;
    fitrange.ulInWidth   	= BufAttr.usWidth;
    fitrange.ulInHeight  	= BufAttr.usHeight;
    fitrange.ulOutWidth  	= BufAttr.usWidth;
    fitrange.ulOutHeight 	= BufAttr.usHeight;

    fitrange.ulInGrabX		= 1;
    fitrange.ulInGrabY		= 1;
    fitrange.ulInGrabW		= fitrange.ulInWidth;
    fitrange.ulInGrabH		= fitrange.ulInHeight;

    MMPD_Scaler_GetGCDBestFitScale(&fitrange, &grabctl);
    
    fctlAttr.fctllink       = m_fctlLinkCapture;
    fctlAttr.fitrange       = fitrange;
    fctlAttr.grabctl        = grabctl;
    fctlAttr.scalsrc		= MMP_SCAL_SOURCE_GRA;
    fctlAttr.bSetScalerSrc	= MMP_TRUE;

    MMPD_DSC_RawBuf2Jpeg(&BufAttr, &rect, 1, &fctlAttr);
	MMPD_DSC_GetJpegSize(&ulJpegSize);
	
	if (ulJpegSize > bufsize) {
		printc(BG_RED("%s %d: ERROR JPEG size %d but buffer size %d\n"), __func__, __LINE__,
							ulJpegSize, bufsize);
		goto End_miCreateJPEGThumbnail;
	}
	/* for debug
	MMPD_DSC_SetFileName("SD:\\001.jpg", strlen("SD:\\001.jpg"));
	MMPD_DSC_JpegDram2Card(capturebuf.ulCompressStart, ulJpegSize, MMP_TRUE, MMP_TRUE);
	*/
	memcpy(pbuf, (void*)capturebuf.ulCompressStart, ulJpegSize);
    }
    *width  = w;
    *height = h;
 	ret = (int)ulJpegSize;
End_miCreateJPEGThumbnail:
	AHC_SetMode(save);
	AHC_SetStreamingMode(AHC_STREAM_RESUME);
	return ret;
}

int miGetThumbnail(void *handle, unsigned char **jpegbuf)
{
	MI_THUMBNAIL	*pmi;
	
	if (!handle) return 0;
	pmi = (MI_THUMBNAIL*)handle;
	if (jpegbuf != NULL)
		*jpegbuf = pmi->jbuf;
	return pmi->jsize;
}

int miGetThumbnailSize(void *handle)
{
	MI_THUMBNAIL	*pmi;
	
	if (!handle) return 0;
	pmi = (MI_THUMBNAIL*)handle;
	return pmi->jsize;
}

int miGetThumbnailDimension(void *handle, int *pwidth, int *pheight)
{
	MI_THUMBNAIL	*pmi;
	
	if (!handle) return 0;
	pmi = (MI_THUMBNAIL*)handle;
	*pwidth  = pmi->width;
	*pheight = pmi->height;
	return 1;
}

int miGetJPEGThumbnail(char *filename, unsigned char *pbuf, int bufsize, int *width, int *height)
{
	MMP_ULONG			offset, size;
	unsigned int		ret;
	unsigned short		usJpgWidth, usJpgHeight;
	MMP_ULONG			exifBufSize;
	MMP_UBYTE			*exifBuf = NULL;
	MMP_ULONG			cache, rsiz;
	MMP_DSC_JPEG_FMT	jpgFormat;
	MMP_DSC_JPEG_INFO 	sJpegInfo;
	
	m_ulMediaFileNameAddr = (MMP_ULONG)filename;
	if (MMPF_DSC_OpenJpegDecFile() != MMP_ERR_NONE) {
		printc(BG_RED("ERROR: %s %d Open - %s\n"), __func__, __LINE__, filename);
		return -1;
	}
	if (m_ulDscFileSize == 0) {
		printc(BG_RED("ERROR: %s %d %s\n"), __func__, __LINE__, filename);
		ret = -1;
		goto END_miGetJPEGThumbnail;
	}
	
	MMPF_DSC_ResetPreSeekPos();
	
	MMPF_DSC_SetJpgDecOffset(0, m_ulDscFileSize);
	//
    MMPD_DSC_GetBufferSize(MMPD_DSC_EXIF_WORK_BUFFER, 0, 0 ,0, &exifBufSize);
    exifBuf = (MMP_UBYTE*)osal_malloc(exifBufSize);
    if (exifBuf == NULL) {
		printc(BG_RED("ERROR: %s %d Memory not eough %d\n"), __func__, __LINE__, exifBufSize);
		ret = -1;
		goto END_miGetJPEGThumbnail;
    }
    // Give working buffer, but its size is less then MMPS flow,
    // and the buffer is from Network memory POOL (cachable).
    // So, it must flush/invalidate before/after writing data into buffer 
	MMPD_DSC_SetExifWorkingBuffer(EXIF_NODE_ID_PRIMARY, (MMP_UBYTE *)exifBuf, exifBufSize / 2, MMP_FALSE);
    MMPD_DSC_SetExifWorkingBuffer(EXIF_NODE_ID_PRIMARY, (MMP_UBYTE* )exifBuf + (exifBufSize / 2),
    								exifBufSize / 2, MMP_TRUE);
	cache = (MMP_ULONG)MMPF_DSC_MallocBuf(EXIF_NODE_ID_PRIMARY, 1024, MMP_TRUE);
    MMPF_DSC_SetDecodeInputBuf(cache, 1024, 0, 0);

	if (MMPF_EXIF_GetThumbInfo(&sJpegInfo, MMPF_DSC_ReadJpegFile) != MMP_ERR_NONE) {
		printc("%s %d: %s NO Thumbnail\n", __func__, __LINE__, filename);
		ret = 0;
		goto END_miGetJPEGThumbnail;
	}

	offset = sJpegInfo.ulThumbOffset;
	size = sJpegInfo.ulThumbSize;
	
	MMPF_DSC_SetJpgDecOffset(offset, offset + size);
	
	if (MMPF_DSC_ScanJpegMarker(offset, offset + size,
								&usJpgWidth, &usJpgHeight, &jpgFormat) != MMP_ERR_NONE) {
		printc("%s %d: %s Thumbnail ERROR!\n", __func__, __LINE__, filename);
		ret = -1;
		goto END_miGetJPEGThumbnail;
	}
	if (width != NULL && height != NULL) {
		*width  = (int)usJpgWidth;
		*height = (int)usJpgHeight;
	}
	// free exif buffer
	osal_free(exifBuf);
	exifBuf = NULL;
	//
	if (pbuf) {
		// alloc buffer of thumbnail for caller when caller not specify address.
		if (bufsize < size) {
			printc("%s %d:Buffer Error!\n", __func__, __LINE__);
			ret = -1;
			goto END_miGetJPEGThumbnail;
		}
		// Seek to thumbnail start position
		MMPF_FS_FSeek(m_ulDSCFileID, offset, MMP_FS_FILE_BEGIN);
		// flush all data in cache into physical memory
		osal_flush_dcache();
		if (MMPF_FS_FRead(m_ulDSCFileID, pbuf, size, &rsiz) != MMP_ERR_NONE ||
			rsiz != size) {
			printc("%s %d: %s Thumbnail Read ERROR!\n", __func__, __LINE__, filename);
			ret = -1;
			goto END_miGetJPEGThumbnail;
		}
		// Make D-cache is invalid to
		// make sure CPU(Program) can read correct data that in phyical memory.
		osal_invalidate_dcache(((unsigned int)(pbuf) & ~(MEM_ALIGNMENT_HEAP - 1)) , size);
	}
	// return thumbnail size
	ret = size;
END_miGetJPEGThumbnail:
	MMPF_DSC_CloseJpegDecFile();
	if (exifBuf)
		osal_free(exifBuf);
	return ret;
}

/*
 *
 */
int AHC_Thumb_GetJPEGFromVidFF(	UINT8	*pSrcBufAddr ,
								UINT32	dwBufSize ,
								char 	*pszFullPath ,
								UINT16 	wWdith ,
								UINT16	wHeigh);

#define	THUMBNAIL_WIDTH		160
#define	THUMBNAIL_HEIGHT	 96
/*
 * When pbuf is NULL return the thumbnail size and width/height
 */
#include "AHC_General.h"
int miGetVideoThumbnail(char *filename, unsigned char *pbuf, int bufsize, int *width, int *height)
{
	int			ret, size;
	MMP_UBYTE	*jpegBuf;
	AHC_MODE_ID	save;

	MMP_UBYTE	ubLCDBKPowerOff = 0;			// CarDV - Aaron
	AHC_DISPLAY_OUTPUTPANEL		OutputDevice;	// CarDV - Aaron

	if (pbuf == NULL) {
		jpegBuf = (MMP_UBYTE*)osal_malloc(MAX_JPEG_SIZE);
		if (jpegBuf == NULL) {
			printc(BG_RED("%s %d: ERROR at Memory Alloc\n"), __func__, __LINE__);
			ret = -1;
			goto END_miGetVideoThumbnail;
		}
		size = MAX_JPEG_SIZE;
	} else {
		jpegBuf = pbuf;
		size = bufsize;
	}
	
	save = AHC_MODE_MAX;
	if (uiGetCurrentState() != UI_NET_PLAYBACK_STATE) {

		/// CarDV - Aaron +++
		// Read media files, LCD will be huaping.
		AHC_GetDisplayOutputDev(&OutputDevice);
		if(OutputDevice == MMP_DISPLAY_SEL_MAIN_LCD)
		{
			ubLCDBKPowerOff = AHC_TRUE;
			LedCtrl_LcdBackLight(AHC_FALSE);
		}
		// CarDV - Aaron ---

		AHC_SetStreamingMode(AHC_STREAM_PAUSE);
		save = AHC_GetAhcSysMode();
		AHC_SetMode(AHC_MODE_IDLE);
	}

	// flush all data in cache into physical memory
	osal_flush_dcache();
	ret = AHC_Thumb_GetJPEGFromVidFF((UINT8*)jpegBuf, size, filename, THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT);	
	
	if (save != AHC_MODE_MAX) {
		AHC_SetMode(save);
		AHC_SetStreamingMode(AHC_STREAM_RESUME);
	}
	
	if (ret < 0 || ret > size) {
		printc(BG_RED("%s %d: ERROR File - %s\n"), __func__, __LINE__, filename);
		ret = -1;
		goto END_miGetVideoThumbnail;
	}
	osal_invalidate_dcache(((unsigned int)(jpegBuf) & ~(MEM_ALIGNMENT_HEAP - 1)) , size);
	*width  = THUMBNAIL_WIDTH;
	*height = THUMBNAIL_HEIGHT;
	if (pbuf == NULL)
		if (jpegBuf) osal_free(jpegBuf);
END_miGetVideoThumbnail:
		// CarDV - Aaron +++
		AHC_GetDisplayOutputDev(&OutputDevice);

		if(OutputDevice == MMP_DISPLAY_SEL_MAIN_LCD)
		{
			if(ubLCDBKPowerOff)
			{
				if(!LedCtrl_GetBacklightStatus())
				{
					LedCtrl_LcdBackLight(AHC_TRUE);
				}
			}
		}
		// CarDV - Aaron ---

	return ret;
}

//
typedef	unsigned short JPEGTAG;
typedef struct {
	JPEGTAG			tag;
	unsigned short	size;
} JTAG_HEADER;
//
#define	TAG_JPEG	0xffd8
#define	TAG_APP1	0xffe1
#define	TAG_SOF		0xffc0
#define	TAG_QT		0xffdb
#define	MAKE_JTAG_HEADER(jh, src)	jh.tag  = (*(src)     << 8) + (*(src + 1));\
									jh.size = (*(src + 2) << 8) + (*(src + 3));
//
#define	ATOM(a, b, c, d)	((a) + (b << 8) + (c << 16) + (d << 24))
#define	ATOM_ftyp	ATOM('f','t','y','p')
#define	ATOM_moov	ATOM('m','o','o','v')
#define	ATOM_mvhd	ATOM('m','v','h','d')
#define	ATOM_trak	ATOM('t','r','a','k')
#define	ATOM_tkhd	ATOM('t','k','h','d')
#define	ATOM_mdia	ATOM('m','d','i','a')
#define	ATOM_minf	ATOM('m','i','n','f')
#define	ATOM_stbl	ATOM('s','t','b','l')
#define	ATOM_stsz	ATOM('s','t','s','z')
//
typedef	unsigned int	MP4ATOM;
typedef	struct {
	unsigned int	size;
	MP4ATOM			atom;
} ATOM_HEADER;
//
typedef struct {
	OSAL_FILE*		fhdl;
	unsigned int	fpos;	// file position of read
	unsigned int	bpos;
	unsigned int	rpos;	// file position of data in buf
	char*			buf;
	unsigned int	bufsize;
	ATOM_HEADER		ah;
} ACONTEXT;
//
#define	MAKE_ATOM_HEADER(ah, src)	ah.size =  (*(src)     << 24) +	\
											   (*(src + 1) << 16) + \
											   (*(src + 2) <<  8) + \
											   (*(src + 3));  \
									ah.atom =  (*(src + 4)) + \
											   (*(src + 5) <<  8) + \
											   (*(src + 6) << 16) + \
											   (*(src + 7) << 24);
											  
#define	MAKE_INT16(p)				(int)(*(p) << 8) + (int)(*(p + 1))
#define	MAKE_INT32(p)				(int)(*(p) << 24)    + (int)(*(p + 1) << 16) + \
									(int)(*(p + 2) << 8) + (int)(*(p + 3))

ACONTEXT* InitAtomContext(ACONTEXT *atxt, OSAL_FILE *hdl);
void UninitAtomContext(ACONTEXT *atxt);
ATOM_HEADER* Reach_Sibling_Atom(ACONTEXT *atxt, MP4ATOM theAtom);
ATOM_HEADER *Find_AtomString(ACONTEXT *atxt, MP4ATOM *astr);

//
JTAG_HEADER *Find_JPEG_Tag(ACONTEXT *jtxt, JPEGTAG theTag)
{
	while (1) {
		JTAG_HEADER	jh;

		MAKE_JTAG_HEADER(jh, jtxt->buf + (jtxt->bpos - jtxt->rpos));
		if (jh.size == 0) {
			printc(BG_RED("%s %d UNRECOGNIZE FORMAT\n"), __func__, __LINE__);
			return NULL;	// Unrecognize  Format
		}
		if (jh.tag == theTag) {
			memcpy(&jtxt->ah, &jh, sizeof(JTAG_HEADER));
			return (JTAG_HEADER*)&jtxt->ah;
		}
		// next sibling
		jtxt->bpos += (jh.size + sizeof(JPEGTAG));
		if (jtxt->bpos > jtxt->fpos - sizeof(JTAG_HEADER)) {
			unsigned int	rs;
		
			jtxt->rpos = jtxt->bpos & (~(512 - 1));
			if (osal_fseek(jtxt->fhdl, jtxt->rpos, MMP_FS_FILE_BEGIN) != 0) {
				break;
			}
			osal_flush_dcache();
			if ((rs = osal_fread(jtxt->buf, 1, jtxt->bufsize, jtxt->fhdl)) == 0) {
				break;
			}
			osal_invalidate_dcache(((unsigned int)(jtxt->buf) & ~(MEM_ALIGNMENT_HEAP - 1)) , rs);
			jtxt->fpos = jtxt->rpos + rs;
		} else if (jtxt->bpos < jtxt->rpos) {
			printc(BG_RED("%s %d UNRECOGNIZE FORMAT\n"), __func__, __LINE__);
			return NULL;
		}
	}
	return NULL;
}
//
int miGetJPGInfo(OSAL_FILE *hdl, MI_INFO *miInfo)
{
	ACONTEXT	context;
	int		  	ret;

	ret = 0;	
	if (InitAtomContext(&context, hdl) == NULL)
		goto ERR_miGetJPGInfo;
	context.bpos += sizeof(JPEGTAG);	// skip TAG_JPEG
	if (Find_JPEG_Tag(&context, TAG_SOF) == NULL)
		goto ERR_miGetJPGInfo;
	miInfo->width  = MAKE_INT16(context.buf + (context.bpos - context.rpos) + 0x7);
	miInfo->height = MAKE_INT16(context.buf + (context.bpos - context.rpos) + 0x5);
	miInfo->type   = 2; // Still Image
	ret = sizeof(MI_INFO);
ERR_miGetJPGInfo:
	UninitAtomContext(&context);
	return ret;
}

// frame rate = moov.mdia.stbl.stsz.samplecount /
//				(moov.trak.tkhd.duration / moov.mvhd.timescale)
int miGetMP4Info(OSAL_FILE *hdl, MI_INFO *miInfo)
{
	#define	BUF_MP4SIZE		1024
	#define	READ_MP4SIZE	1024
	//char			*buf;
	//size_t			rs;
	int				ret;
	//ATOM_HEADER		ah;
	ACONTEXT		context;
	MP4ATOM	mvhdstr[] = {ATOM_moov, ATOM_mvhd, 0};
	MP4ATOM tkhdstr[] = {			ATOM_trak, ATOM_tkhd, 0};
	MP4ATOM stszstr[] = {                      ATOM_mdia, ATOM_minf, ATOM_stbl, ATOM_stsz, 0};
	ret = 0;
	// |<-------------------------->|
	// |<----->r<---b---->f<------->|
	//
	// init context
	if (InitAtomContext(&context, hdl) == NULL)
		goto ERR_miGetMP4Info;
	// moov.mvhd, get timescale, duration
	if (Find_AtomString(&context, mvhdstr) == NULL)
		goto ERR_miGetMP4Info;
	miInfo->timescale = MAKE_INT32(context.buf + (context.bpos - context.rpos) + 0x14);
	miInfo->duration  = MAKE_INT32(context.buf + (context.bpos - context.rpos) + 0x18);
	// moov.trak.tkhd, get width, height
	// *NOTE*: the first trak MUST BE vide
	if (Find_AtomString(&context, tkhdstr) == NULL)
		goto ERR_miGetMP4Info;
	miInfo->width  = MAKE_INT16(context.buf + (context.bpos - context.rpos) + 0x54);
	miInfo->height = MAKE_INT16(context.buf + (context.bpos - context.rpos) + 0x58);
	// moov.trak.mdia.minf.stbl.stsz, get sample counts
	if (Find_AtomString(&context, stszstr) == NULL)
		goto ERR_miGetMP4Info;
	miInfo->samplecount = MAKE_INT32(context.buf + (context.bpos - context.rpos) + 0x10);
	// It's MP4.
	miInfo->type = 1;	//MP4
	ret = sizeof(MI_INFO);

ERR_miGetMP4Info:
	UninitAtomContext(&context);
	return ret;
}
/*
 * Init ATON Context
 */
ACONTEXT* InitAtomContext(ACONTEXT *atxt, OSAL_FILE *hdl)
{
	#define	BUF_MP4SIZE		1024
	#define	READ_MP4SIZE	1024
	size_t			rs;
	
	// init context
	atxt->rpos = atxt->bpos = atxt->fpos = 0;
	atxt->fhdl = hdl;
	atxt->buf  = osal_malloc(BUF_MP4SIZE);
	atxt->bufsize = BUF_MP4SIZE;
	if (atxt->buf == NULL) {
		return NULL;
	}
	osal_fseek(atxt->fhdl, atxt->rpos, MMP_FS_FILE_BEGIN);
	osal_flush_dcache();
	if ((rs = osal_fread(atxt->buf, 1, atxt->bufsize, atxt->fhdl)) == 0) {
		osal_free(atxt->buf);
		return NULL;
	}
	osal_invalidate_dcache(((unsigned int)(atxt->buf) & ~(MEM_ALIGNMENT_HEAP - 1)) , rs);
	atxt->fpos += rs;
	return atxt;
}
/*
 *
 */
void UninitAtomContext(ACONTEXT *atxt)
{
	if (atxt->buf)
		osal_free(atxt->buf);
}
/*
 *
 */
ATOM_HEADER* Reach_Sibling_Atom(ACONTEXT *atxt, MP4ATOM theAtom)
{
	while (1) {
		ATOM_HEADER	ah;

		MAKE_ATOM_HEADER(ah, atxt->buf + (atxt->bpos - atxt->rpos));
		if (ah.size == 0) {
			printc(BG_RED("%s %d UNRECOGNIZE FORMAT\n"), __func__, __LINE__);
			return NULL;	// Unrecognize  Format
		}
		if (ah.atom == theAtom) {
			memcpy(&atxt->ah, &ah, sizeof(ATOM_HEADER));
			return &atxt->ah;
		}
		// next sibling
		atxt->bpos += ah.size;
		if (atxt->bpos > atxt->fpos - sizeof(ATOM_HEADER)) {
			unsigned int	rs;
		
			atxt->rpos = atxt->bpos & (~(512 - 1));
			if (osal_fseek(atxt->fhdl, atxt->rpos, MMP_FS_FILE_BEGIN) != 0) {
				break;
			}
			osal_flush_dcache();
			if ((rs = osal_fread(atxt->buf, 1, atxt->bufsize, atxt->fhdl)) == 0) {
				break;
			}
			osal_invalidate_dcache(((unsigned int)(atxt->buf) & ~(MEM_ALIGNMENT_HEAP - 1)) , rs);
			atxt->fpos = atxt->rpos + rs;
		} else if (atxt->bpos < atxt->rpos) {
			printc(BG_RED("%s %d UNRECOGNIZE FORMAT\n"), __func__, __LINE__);
			return NULL;
		}
	}
	return NULL;
}
/*
 *
 */
ATOM_HEADER *Find_AtomString(ACONTEXT *atxt, MP4ATOM *astr)
{
	MP4ATOM *pato;
	
	if (astr == NULL) return NULL;
	pato = astr;
	while (Reach_Sibling_Atom(atxt, *pato) != NULL) {
		pato++;
		if (*pato == NULL)
			return &atxt->ah;
		// to offspring
		atxt->bpos += sizeof(ATOM_HEADER);
	}
	return NULL;
}
/*
 * handle is file handle
 * Get Media File Info
 */
int miGetMediaInfo(char *filename, MI_INFO *miInfo)
{
	OSAL_FILE *hdl;
	char	  *p;
	int		  ret;
	
	ret = 0;
	p = strrchr(filename, '.');
	if (p == NULL)
		return ret;
	hdl = osal_fopen(filename, "rb");
	if (hdl == NULL)
		return ret;
	if (stricmp(p + 1, "JPG") == 0) {
		ret = miGetJPGInfo(hdl, miInfo);
	} else if (stricmp(p + 1, "MP4") == 0 ||
			   stricmp(p + 1, "MOV") == 0) {
		ret = miGetMP4Info(hdl, miInfo);
	}
	osal_fclose(hdl);
	return ret;
}
//
// Enum DCF File
#include "AHC_General.h"
#include "AHC_UF.h"
#include "fs_api.h"
static void copy_ffattr(PFFINST pffi, DCF_INFO *pinfo);
static FFINST	_ffi;

int FindNextFile(void *hinst, char *out, int bufsize, int *retsize, FFFILTER_FN* pfilter);

/*
 * check the input file is writting by recorder
 */
static int fileIsRecording(char* filename)
{
	if (AHC_GetAhcSysMode() == AHC_MODE_VIDEO_RECORD) {
		char	*fullname;
		//char	*r1, *r2;
		//int		i;
		
		fullname = (char*)AHC_GetCurrentVRFileName(AHC_TRUE);
		printc("Current Recording file %s\r\n", fullname);
		return 0;
	}
	return 0;
}
/*
 * Currently Support DCF only, \DCIM\*
 */
void* FindFirstFile(char *inout, int bufsize, int *retsize, FFFILTER_FN* pfilter)
{
	if (!AHC_UF_GetTotalFileCount(&_ffi.tot)) {
		return NULL;
	}
	_ffi.idx = -1;
	if (FindNextFile(&_ffi, inout, bufsize, retsize, pfilter) == 0)
		return NULL;
	return &_ffi;
}

int FindNextFile(void *hinst, char *out, int bufsize, int *retsize, FFFILTER_FN* pfilter)
{
	unsigned char	type;
	int				len;
	PFFINST			pffi;
	int				idx;
	DCF_INFO		info;
 	
	len = *retsize = 0;
	if (!hinst) {
		return 0;
	}
	pffi = (PFFINST)(hinst);
	idx  = pffi->idx;
	pffi->idx++;

__fnnext:
#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_NORMAL)
	if (!AHC_DCF_GetInfobyIndex(pffi->idx, &info)) {
		pffi->idx = idx;
		return 0;
	}
#else
	{
 	AHC_RTC_TIME	ftime;
	if (AHC_UF_GetFilePathNamebyIndex(pffi->idx, info.FileName) == AHC_FALSE) {
		pffi->idx = idx;
		return 0;
	}
	AHC_UF_GetFileSizebyIndex(pffi->idx, &info.uiFileSize);
	AHC_UF_GetFileTypebyIndex(pffi->idx, &info.FileType);
	AHC_UF_IsReadOnlybyIndex(pffi->idx, &info.bReadOnly);
	AHC_UF_GetFileTimebyIndex(pffi->idx, &ftime);
	info.uwYear = ftime.uwYear;
	info.uwMonth= ftime.uwMonth;
	info.uwDay  = ftime.uwDay;
	info.uwHour = ftime.uwHour;
	info.uwMinute=ftime.uwMinute;
	info.uwSecond=ftime.uwSecond;
	}
#endif
	strncpy(out, info.FileName, bufsize);
	pffi->data = (void*)&info;
	type = info.FileType;
	if (pfilter) {
		if ((len = (pfilter->ffn)(hinst, out, (void*)type, pfilter->arg)) == 0) {
			pffi->idx++;
			goto __fnnext;
		}
	}
	len = strlen(out);
	// don't send writing file to remote to be deleted or downloaded
	if (fileIsRecording(out)) {
		pffi->idx++;
		goto __fnnext;
	}
	copy_ffattr(pffi, &info);
	*retsize = (len > bufsize - 1)? bufsize : len;
	return *retsize;
}

int FindFileAttr(void *hinst, char *in, unsigned int* size, char** fmt, int *rd, FS_FILETIME *ft)
{
	PFFINST			pffi;

	if (!hinst)
		return 0;
	pffi  = (PFFINST)(hinst);
	*size = pffi->flen;
	switch (pffi->fmt) {
	case 0:
		*fmt = "jpeg";
		break;
	case 2:
		*fmt = "AVI";
		break;
	case 3:
		*fmt = "MOV";
		break;
	case 4:
		*fmt = "MP4";
		break;
	default:
		*fmt = "BAD";
	}
	*rd   = pffi->rw;
	*ft   = pffi->ft;
	return 1;
}

int FindFileGroup(void *hinst, FILE_GRPINFO *pinf)
{
	PFFINST			pffi;
	DCF_INFO		*pif;

	if (!hinst) {
		return 0;
	}
	pffi = (PFFINST)(hinst);
	/*
	if (AHC_UF_GetDirFileKeybyIndex(pffi->idx, &pinf->dkey, &pinf->fkey) != AHC_TRUE) {
		printc("ERROR:%s %d\n", __func__, __LINE__);
		return 0;
	}
	*/
	pif = (DCF_INFO*)pffi->data;
	pinf->dkey = pif->uwDirKey;
	pinf->fkey = pif->uwFileKey;
	#if (defined(DCF_GROUP_FUN) && DCF_GROUP_FUN != 0)
	if (AHC_UF_GetCurGrp(pinf->dkey, pinf->fkey, &pinf->grpid, &pinf->grpmo) != AHC_TRUE) {
		printc("ERROR:%s %d\n", __func__, __LINE__);
		return 0;
	}
	if (AHC_UF_GetGrpCurFileNum(pinf->dkey, pinf->fkey, pinf->grpid, &pinf->grpno) != AHC_TRUE) {
		printc("ERROR:%s %d\n", __func__, __LINE__);
		return 0;
	}
	if (AHC_UF_GetGrpTotalFileNum(pinf->dkey, pinf->grpid, &pinf->grpto) != AHC_TRUE) {
		printc("ERROR:%s %d\n", __func__, __LINE__);
		return 0;
	}
	#else
	//TBD
	#endif
	return 1;
}

int FindFileGetPosition(void *hinst)
{
	PFFINST			pffi;

	if (!hinst)
		return 0;
	pffi = (PFFINST)(hinst);
	return pffi->idx;
}

void *FindFileSetPosition(void *hinst, int pos, FFFILTER_FN* pFilter)
{
	PFFINST			pffi;

	if (!hinst)
		return 0;
	pffi = (PFFINST)(hinst);
	pffi->idx = pos;
	return hinst;
}

static void copy_ffattr(PFFINST pffi, DCF_INFO *pinfo)
{
	pffi->flen = pinfo->uiFileSize;
	pffi->rw = (int)(pinfo->bReadOnly == AHC_TRUE)? 1 : 0;	// ??
	pffi->fmt  = pinfo->FileType;
	pffi->ft.Year     =  pinfo->uwYear;
	pffi->ft.Month    =  pinfo->uwMonth;
	pffi->ft.Day      =  pinfo->uwDay;
	pffi->ft.Hour     =  pinfo->uwHour;
	pffi->ft.Minute   =  pinfo->uwMinute;
	pffi->ft.Second   =  pinfo->uwSecond;
}

int GetFrontFilename(char* front)
{
	char *p,*n;
	p = strrchr(front, '.');
	p--;
	if (!strncmp(p,  "R", 1)) {
		n = strrchr(front, 'l');
		n+=2;
		if (!strncmp(n,  "R", 1)) {
			memcpy(n,"F",1);
			memcpy(p,"F",1);
			return 1;
		}
	}
	return 0;
}

#define STATION_SERVICE_PORT 49132//43
#define STATION_SERVICE_VERSION	0x0f0a //2015/10

extern struct netif main_netdev;
static int 	gSTAser_fd = -1;
static struct ammo_evmsg * gpSTAev = NULL;

//#define DBG_STA_SERV
static int station_handle_request(char * src,int revlen,char *pack)
{
	
	short reqtype; 
	char *prestype,*presdata;
	int reslen;
	
	if(revlen < 4) 
		return 0;
	
	if(*src != (STATION_SERVICE_VERSION >> 8) || *(src+1) != (STATION_SERVICE_VERSION & 0xff))
		return 0;
	
	#ifdef DBG_STA_SERV	
	{
		int i ;
		for(i=0 ; i < revlen ; i++){
			printc(FG_BLUE("0x%x "),*(src+i));
		}
		printc(FG_BLUE("\n"));
	}
	#endif
	
	reslen 		= 0;
	reqtype 	= ntohs(*(uint16*)(src + 2));
	prestype 	= pack + 2;
	presdata 	= pack + 4;
	
	printc("%s : reqtype=%x\n",__FUNCTION__,reqtype);
	
	switch(reqtype)
	{
		case 0x0001:
		{//request ip
			*(uint16*)prestype = htons(0x0101);
			*(uint32*)presdata = htonl(main_netdev.ip_addr.addr);
			printc("ip = %x \n",main_netdev.ip_addr.addr);
			reslen = 8;
		}
		break;
		
		default:
		return 0;
	}
	
	*pack = STATION_SERVICE_VERSION >> 8;
	*(pack+1) = STATION_SERVICE_VERSION & 0xff;
	
	#ifdef DBG_STA_SERV	
	{
		int i ;
		for(i=0 ; i < reslen ; i++){
			printc(FG_RED("0x%x "),*(pack+i));
		}
		printc(FG_RED("\n"));
	}
	#endif
	
	return reslen;
	
}

#define DISCOVER_STR "DISCOVER.CAMERA.IP"
static void station_service_requset(struct ammo_evmsg *m, void *d)
{
	socklen_t fromlen;
	int rev_len;
	struct sockaddr_in sockaddr;
	char rec_buf[128];
	
	fromlen = sizeof(struct sockaddr_in);
	if((rev_len = lwip_recvfrom(gSTAser_fd, rec_buf, sizeof(rec_buf), 0 , (struct sockaddr *)&sockaddr, &fromlen)) > 0)
	{
		int sendlen;
		//printc("UDP receive data =%s,len=%d\n",rec_buf,rev_len);

		if (!strncasecmp(rec_buf,DISCOVER_STR,rev_len)) {
				netapp_UINotifyChanged(NULL);
		}

		if((sendlen = station_handle_request(rec_buf,rev_len,rec_buf)) != 0){
			int client_fd;	
			struct sockaddr_in bind_sockaddr;		
			int cnt;
			
			if((client_fd = lwip_socket( PF_INET, SOCK_DGRAM, 0 )) < 0)
				return;

			bind_sockaddr.sin_family      = AF_INET;
			bind_sockaddr.sin_addr.s_addr = 0;
			bind_sockaddr.sin_port        = 0;

			if( lwip_bind( client_fd, (struct sockaddr *)&bind_sockaddr, sizeof( sockaddr ) ) < 0 ) {
				osal_dprint(ERR, "fail to bind socket: %s",__FUNCTION__);
				lwip_close( client_fd );
			}
			
			#ifdef DBG_STA_SERV	
			printc("rec ip = %x port = %d\n",sockaddr.sin_addr.s_addr,sockaddr.sin_port);
			#endif

			cnt = lwip_sendto(client_fd, rec_buf, sendlen, 0, (struct sockaddr*)&sockaddr, fromlen);
			lwip_close( client_fd );
		}
	}
}

/*
* Only for station Mode, listen to UDP port AIT_SERVICE_PORT
* to service request ,such as ip request ...
*/
void start_station_service(void)
{
	struct sockaddr_in sockaddr;
	
	if(gSTAser_fd != -1){
		printc("%s : socket has exsited\n",__FUNCTION__);
		return; 
	}
	
	if((gSTAser_fd = lwip_socket( PF_INET, SOCK_DGRAM, 0 )) < 0){ 
	    return; 
	}	
	
	sockaddr.sin_family      = AF_INET;
	sockaddr.sin_addr.s_addr = IPADDR_ANY;//INADDR_BROADCAST;
	sockaddr.sin_port        = htons(STATION_SERVICE_PORT);

	if( lwip_bind( gSTAser_fd, (struct sockaddr *)&sockaddr, sizeof( sockaddr ) ) < 0 ) {
		osal_dprint(ERR, "fail to bind socket: %s", osal_strerror() );
		lwip_close( gSTAser_fd );
		return ;
	}
	
	//lwip_fcntl( gSTAser_fd, F_SETFL, O_NONBLOCK );
	
	gpSTAev = evmsg_new_FD( gSTAser_fd, 0, 0, station_service_requset, 0 );
}

void stop_station_service(void)
{
	
	if(gpSTAev){
		evmsg_remove(gpSTAev);
		gpSTAev = 0;
	}
	
	if(gSTAser_fd != -1){
		lwip_close(gSTAser_fd);
		gSTAser_fd = -1;
	}
	
}


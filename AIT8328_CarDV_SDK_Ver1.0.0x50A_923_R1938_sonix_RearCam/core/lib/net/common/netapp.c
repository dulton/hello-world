/**
@file netapp.c
  Created on: 2013/10/17
      Author: Truman

      This module centralized as mediator, manager.
      MMI, ASS, WLAN, LWIP, Net Daemon could be the "client module".
      The client module can set and get status with this module.
      The client module can post event here and the message would be dispatched to other modules.

      Because so many client modules belongs to different layers or packages, there is no appropriate existing prefix for this.
      since NetApp exists, it was used.

      Listed client modules:

      MMI: AHC_Main(33)
      ASS:
      WLAN: start->Network_Task(54)->tcp_init->tcp_thread(40) before the real TCP thread.
      LWIP: netif
      Net Daemon:
      Web Server:(UPNP) (41 or 42)->CONNECT(43~53)->handle request
      DHCP Server:(in LWIP package)
*/
//#include "customer_config.h"
#include "netapp.h"
#include "wlan.h"
#include "netconf.h"
#include "dhcp_server.h"
#include "mmp_lib.h" //for printc
//#include "AHC_OS.h"
#include "AHC_General.h" //for GPIO & AHC_SendAHLMessage
#include "AHC_Version.h" //for AHC_GetMinorVersion()
#include "AHC_Stream.h"
#include "AHC_DCF.h"
#include "AHC_Media.h"
#include "AIHC_Video.h"
#include "CarDV_dsc_key.h" //for virtual event
#include "amn_sysobjs.h"
#include "amnss_mmobjs.h"
#include "amnss.h"
#include "amn_osal.h"
//#include "ucos_ii.h"
#include "ahc_general_CarDV.h"
#include "AHC_WarningMSG.h"
#include "ahc_message.h"
#include "keyparser.h"

//#include "MenuCommon.h"
#include "MenuSetting.h"
#include "AHC_Menu.h"
#include "StateCameraFunc.h"
#include "StateVideoFunc.h"
#include "mmpf_display.h"
#include "mmps_vidplay.h"
#include "AHC_Parameter.h"
#include "StateTVFunc.h"    // CarDV, temp
#include "StateHDMIFunc.h"  // CarDV, temp
#include "MenuConfig_SDK.h" //CarDV...
#include "ldws.h" 

typedef struct {
	char *cgi_val;
	int	 menu_id;
} CGI_MENU_MAP;

AHC_BOOL MenuItemDeleteAll(PSMENUITEM pItem, AHC_BOOL bIsHover);
INT32 AIHC_OpenTranscode(void);
// Functions for convertion of BootUpMode index and ID
//UINT8 CameraSettingsFunc_GetBootUpUIState( UINT8 uiBootUpMode ); //Andy Liu TBD
//UINT8 CameraSettingsFunc_GetBootUpMode( UINT8 uiUIState ); //Andy Liu TBD
static int 		g_WifiTargetMode = NETAPP_WIFI_AP_MODE;
static int		_format_sd = FORMAT_SD_NONE;
WLAN_DRV_HDL    g_hdl = NULL;

void netapp_setHwHandle(WLAN_DRV_HDL h)
{
    g_hdl = h;
}

WLAN_DRV_HDL* netapp_getHwHandle()
{
    return g_hdl;
}

#define ERR_DISP_CNT            8
#define SET_WIFI_FAIL(s)        (s) |= NETAPP_STREAM_FAIL
#define CLR_WIFI_FAIL(s)        (s) &= ~NETAPP_STREAM_FAIL
static int  _netappstatus = NETAPP_WIFI_NONE;
static int  _dispcnt;
#ifndef _NETAPP_DEBUG
int set_NetAppStatus(int status)
{
	if(_netappstatus == NETAPP_WIFI_DOWN && 
		status != NETAPP_WIFI_IDLE){
		return NETAPP_WIFI_DOWN;
	}
		
    if (IS_WIFI_FAIL(status)) {
        SET_WIFI_FAIL(_netappstatus);
        //if (_dispcnt == 0) {
        //	PlayWiFiErrorSound();
        //	AHC_PlaySoundEffect(AHC_SOUNDEFFECT_WIFIFAIL);
		//}
        _dispcnt = ERR_DISP_CNT;
    } else {
        _netappstatus = status;
    }
    return _netappstatus;
}
#else
int set_NetAppStatusDB(int status, char *file, int line)
{
    printc("set_NetAppStatusDB: %s %d 0x%08x\r\n", file, line, status);
    
    if(_netappstatus == NETAPP_WIFI_DOWN && 
		status != NETAPP_WIFI_NONE){
		return NETAPP_WIFI_DOWN;
	}
	
    if (NETAPP_STREAM_STATUS(status) || IS_WIFI_FAIL(status)) {
        if (IS_WIFI_FAIL(status)) {
            SET_WIFI_FAIL(_netappstatus);
            PlayWiFiErrorSound();
            _dispcnt = ERR_DISP_CNT;
        } else {
            _netappstatus = status;
        }
    } else {
        _netappstatus = status;
    }
    return _netappstatus;
}
#endif

int get_NetAppStatus(void)
{
    if (IS_WIFI_FAIL(_netappstatus)) {
        if (_dispcnt == 0) {
            CLR_WIFI_FAIL(_netappstatus);
            _dispcnt = ERR_DISP_CNT;
        } else {
            _dispcnt--;
        }
    }
    return _netappstatus;
}
/*
 * return Camera Status, bit0 - 1 for video recording.
 * TODO:
 */
int get_CamAppStatus()
{
	int		status;
	
	status  = 0;
	status |= NetApp_GetRecordStatus()? 1 : 0;
	return status;
}

struct net_context_t *load_netconfig(int *isap);
int load_wificonfig(int apset, char **pssid, int *pcrypto, char **pckey, int *hidden, int *pchan);
extern struct netif main_netdev;//in lwip_main.c
extern int gbWifiConnect;//in wlan

/***********************************************************************************************/
//      function: TurnOffWiFiModule()
//   description: To turn off the WiFi module..
//      input   : none.
/***********************************************************************************************/
//currently called in AHC_Main UI
int TurnOffWiFiModule(void)
{
	return netapp_StopNetwork();
/*	
    netif_set_down(&main_netdev);
    WLAN_SYS_Stop(g_hdl);
    set_NetAppStatus(NETAPP_IDLE);
    gbWifiConnect = 0;
    return 0;
*/
}

/***********************************************************************************************/
//      function: TurnOnWiFiModule()
//   description: To turn on the WiFi module..
//      input   : none.
/***********************************************************************************************/
//currently called in AHC_Main UI
int  TurnOnWiFiModule(void)
{
    struct net_context_t *thenet;
    int     isap;
    char    *ssid, *ckey;
    int     hidden, crypto, chan;
    int     err;

//function in lwip_main.c, return type in Firmware/lwip/port/include/netconf.h(Firmware/NET/Common/netconfig.c)
    if ((thenet = load_netconfig(&isap)) == NULL)
        return -1;

//function in netif.c, Firmware/lwip/port/include/netif.h
//main_detdev (handle) in lwip_main.c
	if (isap) {
		int		apset;
	    netif_set_addr(&main_netdev, &thenet->local_ip,
	                                 &thenet->net_mask,
	                                 &thenet->gateway_ip);
	    netif_set_up(&main_netdev);
	    apset = netapp_GetWiFi_AP_set();
	    load_wificonfig(apset, &ssid, &crypto, &ckey, &hidden, &chan);
	    DhcpServerSetToDefault();
	    DhcpServerStart();

	// handle in ahc_netmain.c
	    g_hdl = WLAN_SYS_Start();
	    printc("wlan_start g_hdl = 0x%x\r\n", g_hdl);
	    printc("Create SW AP : ");
        err = WLAN_AP_Create((int8*)ssid,
                             WLAN_WPA2_AUTH_PSK, WLAN_ENCRYPT_AES,
                             (int8*)ckey,
                             chan, hidden);
	}
	else{
		//TODO
		//if IP is STATIC type ,set netif here
		/*netif_set_addr(&main_netdev, 
					&thenet->local_ip,
	              	&thenet->net_mask,
	               	&thenet->gateway_ip);
		netif_set_up(&main_netdev);*/
		
		g_hdl = WLAN_SYS_Start();
	    printc("wlan_start g_hdl = 0x%x\r\n", g_hdl);
	}
//move to WLAN_SYS_Start(), it's GB9616 code
#if 0
    //FIXME GB9616 dependent or Wifi spec dependent?
    AHC_OS_SleepMs(200);
    //FIXME GB9616 dependent?
    AHC_SetGPIO(NCFG_RESET_PIN, AHC_TRUE);
#endif
    return 0;
}

/*
 * return 1: AP mode (NETAPP_WIFI_AP_MODE)
 *        2: STA mode (NETAPP_WIFI_STATION_MODE)
 */
int Netapp_get_cur_wifi_mode(void)
{
	return net_context.mode;
}
/*
 * return 1: AP mode(NETAPP_WIFI_AP_MODE)
 *        2: STA mode(NETAPP_WIFI_STATION_MODE)
 */
int Netapp_get_target_wifi_mode(void)
{
	return g_WifiTargetMode;
}
/*
 * mode 	1: AP mode (NETAPP_WIFI_AP_MODE)
 *        	2: STA mode (NETAPP_WIFI_STATION_MODE)
 */
int Netapp_set_wifi_mode(int mode)
{
	g_WifiTargetMode = mode;
	return 0;
}



/**
 *
 * It uses virtual key so that all UI job and GUI drawing would be the the same as Human UI controlling.
 * Later it would need to use other task to turn on the streaming.
 *
 * @param enable 1 to turn on the streaming. 0 to turn off the streaming
 * @retval 1 The request is processed.
 * @retval 0 The request is ignored because it does not require changing.
 * @retval <-1 Other error.
 */
int AHC_IsInVideoMode(void);
extern int transcoding;
int NetApp_SetStreaming(short enable)
{
	bool	bon;
    char	*wl_cmd;
	
	if (AHC_GetStreamingMode() == AHC_STREAM_OFF) {
		if (!AHC_IsInVideoMode() && !transcoding) return -1;
	}
	if (!aitstreamer_is_ready())
		return -2;
	
	bon = get_NetAppStatus() & NETAPP_STREAM_PLAY;
	if(!bon && enable) {
		wl_cmd = "VIDEO.0/EncMJPEG";
	} else if (!enable) {
		wl_cmd = "none";
	} else {
		return 0;
	}
	aitstreamer_run(wl_cmd);
	return 1;
}

/**
 *
 * It accept web command and set as keyevent to do corresponding actions.
 *
 * @input: web event
 */
extern AHC_BOOL VideoTimer_Stop(void);
extern AHC_BOOL CaptureTimer_Stop(void);
void SetKeyPadEvent(MMP_ULONG keyevent);

int Report2UIAndWaitFeedBack(MMP_ULONG msg, MMP_ULONG param1, PFN_WEBWAIT pfn_wait, void *arg)
{
	int		ret;

	// Speed up WiFi event!
	// Send the WiFi message into High Priority Queue,
	// If send to normal queue by AHC_SendAHLMessage,
	// it will wait for normal messages finished like as
	// AHLM_UI_NOTIFICATION/BUTTON_UPDATE_MESSAGE, when system is busy!!
	AHC_SendAHLMessage_HP(AHLM_WIRELESS_CONNECT, msg, param1);
	
    if (pfn_wait) {
    	// Wait
    	ret = pfn_wait((void*)msg, arg);
		printc("--> COMMAND FINISH %d\n", ret);
    	if (ret >= 0)
    		return NETAPP_DONE;
    	else
    		return NETAPP_FAIL;
    }
    return NETAPP_DONE;
}

// UI State
static CGI_MENU_MAP ui_state_cgi_menu[] = {
	{"VIDEO",	  UI_VIDEO_STATE},
	{"CAMERA",	  UI_CAMERA_STATE},
	{"BURST",	  UI_CAMERA_BURST_STATE},
	{"TIMELAPSE", UI_TIME_LAPSE_STATE},
	{"SETTING",	  UI_CAMERA_SETTINGS_STATE},
	{"PLAYBACK",  UI_NET_PLAYBACK_STATE},
	{"UPDATEFW",  UI_NET_FWUPDATE_STATE},
	{NULL, 0}
};
// Video Resolution
static CGI_MENU_MAP videores_cgi_menu[] = {
	{"4K15",		MOVIE_SIZE_4K_15P},
    {"4K12.5",  	MOVIE_SIZE_4K_12d5P},
	{"2.7K30", 		MOVIE_SIZE_2d7K_30P},
    {"2.7K25",  	MOVIE_SIZE_2d7K_25P},	
	{"1080P60", 	MOVIE_SIZE_1080_60P},
	{"1080P50", 	MOVIE_SIZE_1080_50P},
	{"1080P30", 	MOVIE_SIZE_1080_30P},
	{"1080P30fps",	MOVIE_SIZE_1080_30P},	// compatible with 8427 CarDV CGI
	{"1080P25", 	MOVIE_SIZE_1080_25P},
	{"720P120", 	MOVIE_SIZE_720_120P},
	{"720P100", 	MOVIE_SIZE_720_100P},
	{"720P60",  	MOVIE_SIZE_720_60P},
	{"720P60fps",	MOVIE_SIZE_720_60P},	// compatible with 8427 CarDV CGI
	{"720P50",  	MOVIE_SIZE_720_50P},
	{"720P30",  	MOVIE_SIZE_720_30P},
	{"720P30fps",	MOVIE_SIZE_720_30P},	// compatible with 8427 CarDV CGI
	{"VGA120P",		MOVIE_SIZE_VGA_120P},
	{"VGA",			MOVIE_SIZE_VGA_30P},	// compatible with 8427 CarDV CGI
	{NULL, 0}
};
// Image Resolution
static CGI_MENU_MAP imageres_cgi_menu[] = {
	{"8MP",  IMAGE_SIZE_8M},
	{"6MPW", IMAGE_SIZE_6M_WIDE},
	{"6MP",	 IMAGE_SIZE_6M},
	{"5MP",  IMAGE_SIZE_5M},
	{"3MP",  IMAGE_SIZE_3M},
	{"2MP",	 IMAGE_SIZE_2M},
	{"1.2MP",IMAGE_SIZE_1_2M},
	{"VGA",  IMAGE_SIZE_VGA},
	{NULL, 0}
};
// Video Clip Time//CarDV
static CGI_MENU_MAP videoclip_cgi_menu[] = {
#if defined(MENU_MOVIE_CLIP_TIME_OFF_EN) && (MENU_MOVIE_CLIP_TIME_OFF_EN == 1)	
	{"OFF",   MOVIE_CLIP_TIME_OFF},
#endif	
#if defined(MENU_MOVIE_CLIP_TIME_6SEC_EN) && (MENU_MOVIE_CLIP_TIME_6SEC_EN == 1)	
	{"6SEC",  MOVIE_CLIP_TIME_6SEC},
#endif		
#if defined(MENU_MOVIE_CLIP_TIME_1MIN_EN) && (MENU_MOVIE_CLIP_TIME_1MIN_EN == 1)	
	{"1MIN",  MOVIE_CLIP_TIME_1MIN},
#endif
#if defined(MENU_MOVIE_CLIP_TIME_2MIN_EN) && (MENU_MOVIE_CLIP_TIME_2MIN_EN == 1)		
	{"2MIN",  MOVIE_CLIP_TIME_2MIN},
#endif
#if defined(MENU_MOVIE_CLIP_TIME_3MIN_EN) && (MENU_MOVIE_CLIP_TIME_3MIN_EN == 1)		
	{"3MIN",  MOVIE_CLIP_TIME_3MIN},
#endif
#if defined(MENU_MOVIE_CLIP_TIME_5MIN_EN) && (MENU_MOVIE_CLIP_TIME_5MIN_EN == 1)		
	{"5MIN",  MOVIE_CLIP_TIME_5MIN},
#endif
#if defined(MENU_MOVIE_CLIP_TIME_10MIN_EN) && (MENU_MOVIE_CLIP_TIME_10MIN_EN == 1)	
	{"10MIN", MOVIE_CLIP_TIME_10MIN},
#endif
#if defined(MENU_MOVIE_CLIP_TIME_25MIN_EN) && (MENU_MOVIE_CLIP_TIME_25MIN_EN == 1)		
	{"25MIN", MOVIE_CLIP_TIME_25MIN},
#endif	
#if defined(MENU_MOVIE_CLIP_TIME_30MIN_EN) && (MENU_MOVIE_CLIP_TIME_30MIN_EN == 1)		
	{"30MIN", MOVIE_CLIP_TIME_30MIN},
#endif	
	{NULL, 0}
};
// White Balance
static CGI_MENU_MAP wb_cgi_menu[] = {
	{"AUTO",		 WB_AUTO},
	{"DAYLIGHT",	 WB_DAYLIGHT},
	{"CLOUDY",		 WB_CLOUDY},
	{"FLUORESCENT1", WB_FLUORESCENT1},
	{"FLUORESCENT2", WB_FLUORESCENT2},
	{"FLUORESCENT3", WB_FLUORESCENT3},
	{"INCANDESCENT", WB_INCANDESCENT},
	{NULL, 0}
};
// Exposure Time
static CGI_MENU_MAP expo_cgi_menu[] = {
	{"EVN200", EVVALUE_N20},
	{"EVN167", EVVALUE_N17},
    {"EVN150", EVVALUE_N15},
	{"EVN133", EVVALUE_N13},
	{"EVN100", EVVALUE_N10},
	{"EVN067", EVVALUE_N07},
    {"EVN050", EVVALUE_N05},
	{"EVN033", EVVALUE_N03},
	{"EV0",    EVVALUE_00},
	{"EVP033", EVVALUE_P03},
    {"EVP050", EVVALUE_P05},
	{"EVP067", EVVALUE_P07},
	{"EVP100", EVVALUE_P10},
	{"EVP133", EVVALUE_P13},
    {"EVP150", EVVALUE_P15},
	{"EVP167", EVVALUE_P17},
	{"EVP200", EVVALUE_P20},
	{NULL, 0}
};
// Flicker
static CGI_MENU_MAP flicker_cgi_menu[] = {
	{"50Hz", FLICKER_50HZ},
	{"60Hz", FLICKER_60HZ},
	{NULL, 0}
};
// Motion Detection (MTD)
static CGI_MENU_MAP mtd_cgi_menu[] = {
	{"OFF",    MOTION_DTC_SENSITIVITY_OFF},
	{"LOW",    MOTION_DTC_SENSITIVITY_LOW},
	{"MIDDLE", MOTION_DTC_SENSITIVITY_MID},
	{"HIGH",   MOTION_DTC_SENSITIVITY_HIGH},
	{NULL, 0}
};
// PowerOff delay
static CGI_MENU_MAP off_delay_cgi_menu[] = {
	{"OFF",   MOVIE_POWEROFF_TIME_0MIN},
	{"5SEC",  MOVIE_POWEROFF_TIME_5SEC},
	{"10SEC", MOVIE_POWEROFF_TIME_10SEC},
	{"15SEC", MOVIE_POWEROFF_TIME_15SEC},
	{"30SEC", MOVIE_POWEROFF_TIME_30SEC},
	{"60SEC", MOVIE_POWEROFF_TIME_60SEC},
	{"2MIN",  MOVIE_POWEROFF_TIME_2MIN},
	{"3MIN",  MOVIE_POWEROFF_TIME_3MIN},
	{"5MIN",  MOVIE_POWEROFF_TIME_5MIN},
	{"15MIN", MOVIE_POWEROFF_TIME_15MIN},
	{"30MIN", MOVIE_POWEROFF_TIME_30MIN},
	{"60MIN", MOVIE_POWEROFF_TIME_60MIN},
	{NULL, 0}
};
// Video/Image Quality
static CGI_MENU_MAP quality_cgi_menu[] = {
	{"FINE",		QUALITY_FINE},
	{"SUPERFINE",	QUALITY_SUPER_FINE},
	{NULL, 0}
};
// Time format
static CGI_MENU_MAP time_fmt_cgi_menu[] = {
	{"OFF",	DATETIME_SETUP_NONE},
	{"YMD",	DATETIME_SETUP_YMD},
	{"MDY",	DATETIME_SETUP_MDY},
	{"DMY", DATETIME_SETUP_DMY},
	{NULL, 0},
};
// TV System
static CGI_MENU_MAP tv_system_cgi_menu[] = {
	{"NTSC", TV_SYSTEM_NTSC},
	{"PAL",  TV_SYSTEM_PAL},
	{NULL, 0}
};
// LCD Power save
static CGI_MENU_MAP lcd_save_cgi_menu[] = {
	{"OFF",   LCD_POWER_SAVE_OFF},
	{"10SEC", LCD_POWER_SAVE_10SEC},
	{"30SEC", LCD_POWER_SAVE_30SEC},
	{"1MIN",  LCD_POWER_SAVE_1MIN},
	{"3MIN",  LCD_POWER_SAVE_3MIN},
	{NULL, 0}
};
// Photo burst
static CGI_MENU_MAP photo_burst_cgi_menu[] = {
	{"3_1SEC",  BURST_SHOT_LO},
	{"5_1SEC",  BURST_SHOT_MID},
	{"10_1SEC", BURST_SHOT_HI},
	{NULL, 0}
};
// Timelapse
static CGI_MENU_MAP timelapse_cgi_menu[] = {
	//{"3SEC",   TIMELAPSE_INTERVAL_3_SEC},
	{"5SEC",   TIMELAPSE_INTERVAL_5_SEC},
	{"10SEC",  TIMELAPSE_INTERVAL_10_SEC},
	{"30SEC",  TIMELAPSE_INTERVAL_30_SEC},
	{"60SEC",  TIMELAPSE_INTERVAL_60_SEC},
	{NULL, 0}
};
// Upsidedown
static CGI_MENU_MAP upsidedown_cgi_menu[] = {
	{"Normal",       IMAGE_UPSIDE_DOWN_NORMAL},
	{"Upsidedown",   IMAGE_UPSIDE_DOWN_UPSIDE_DOWN},
	{NULL, 0}
};
// Spot Meter
static CGI_MENU_MAP spot_meter_cgi_menu[] = {
	{"OFF",  METERING_CENTER},
	{"ON",   METERING_CENTER_SPOT},
	{NULL, 0}
};
// Looping video//CarDV
static CGI_MENU_MAP looping_video_cgi_menu[] = {
#if defined(MENU_MOVIE_CLIP_TIME_OFF_EN) && (MENU_MOVIE_CLIP_TIME_OFF_EN == 1)	
	{"OFF",   MOVIE_CLIP_TIME_OFF},
#endif
#if defined(MENU_MOVIE_CLIP_TIME_6SEC_EN) && (MENU_MOVIE_CLIP_TIME_6SEC_EN == 1)	
	{"6SEC",  MOVIE_CLIP_TIME_6SEC},
#endif
#if defined(MENU_MOVIE_CLIP_TIME_1MIN_EN) && (MENU_MOVIE_CLIP_TIME_1MIN_EN == 1)	
	{"1MIN",  MOVIE_CLIP_TIME_1MIN},
#endif
#if defined(MENU_MOVIE_CLIP_TIME_2MIN_EN) && (MENU_MOVIE_CLIP_TIME_2MIN_EN == 1)		
	{"2MIN",  MOVIE_CLIP_TIME_2MIN},
#endif
#if defined(MENU_MOVIE_CLIP_TIME_3MIN_EN) && (MENU_MOVIE_CLIP_TIME_3MIN_EN == 1)		
	{"3MIN",  MOVIE_CLIP_TIME_3MIN},
#endif
#if defined(MENU_MOVIE_CLIP_TIME_5MIN_EN) && (MENU_MOVIE_CLIP_TIME_5MIN_EN == 1)		
	{"5MIN",  MOVIE_CLIP_TIME_5MIN},
#endif
#if defined(MENU_MOVIE_CLIP_TIME_10MIN_EN) && (MENU_MOVIE_CLIP_TIME_10MIN_EN == 1)		
	{"10MIN", MOVIE_CLIP_TIME_10MIN},
#endif
#if defined(MENU_MOVIE_CLIP_TIME_25MIN_EN) && (MENU_MOVIE_CLIP_TIME_25MIN_EN == 1)		
	{"25MIN", MOVIE_CLIP_TIME_25MIN},
#endif
#if defined(MENU_MOVIE_CLIP_TIME_30MIN_EN) && (MENU_MOVIE_CLIP_TIME_30MIN_EN == 1)		
	{"30MIN", MOVIE_CLIP_TIME_30MIN},
#endif	
	{NULL, 0}
};
// HDR
static CGI_MENU_MAP hdr_cgi_menu[] = {
	{"OFF",  WDR_OFF},
	{"ON",   WDR_ON},
	{NULL, 0}
};
// Q-SHOT
static CGI_MENU_MAP q_shot_cgi_menu[] = {
	{"OFF",  AUTO_REC_OFF},
	{"ON",   AUTO_REC_ON},
	{NULL, 0}
};
// Status lights
static CGI_MENU_MAP status_lights_cgi_menu[] = {
	{"OFF",  STATUS_LIGHT_OFF},
	{"ON",   STATUS_LIGHT_ON},
	{NULL, 0}
};
// Sound Indicator
static CGI_MENU_MAP sound_indicator_cgi_menu[] = {
	{"OFF",  BEEP_OFF},
	{"ON",   BEEP_ON},
	{NULL, 0}
};
// Power saving
static CGI_MENU_MAP power_saving_cgi_menu[] = {
	{"OFF",  AUTO_POWER_OFF_NEVER},
	{"1MIN", AUTO_POWER_OFF_1MIN},
	{"2MIN", AUTO_POWER_OFF_2MINS},
	{"5MIN", AUTO_POWER_OFF_5MINS},
	{NULL, 0}
};
// SD0 Card
static CGI_MENU_MAP sd0_cgi_menu[] = {
	{"format", 1},
	{NULL, 0}
};

#define	INVALID_ID	(-1)
/*
 * return MENU ID or INVALID_ID if NOT FOUND
 */
static int GetMenuIDbyCGIval(char *cgival, CGI_MENU_MAP *map)
{
	int		i;
	
	for (i = 0; map[i].cgi_val != NULL; i++) {
		if (stricmp(cgival, map[i].cgi_val) == 0)
			return map[i].menu_id;
	}
	return INVALID_ID;
}
/*
 * return CGI value string or NULL if NOT FOUND
 */
const char* GetCGIvalbyMenuId(int menuid, CGI_MENU_MAP *map)
{
	int		i;
	
	for (i = 0; map[i].cgi_val != NULL; i++) {
		if (menuid == map[i].menu_id)
			return (const char*)map[i].cgi_val;
	}
	return "UNKNOWN";
}


typedef int (*PFN_CGI_HANDLER)(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s);
typedef struct {
	const char*		cmdKey;
	const int		cmdKeyLen;
	PFN_CGI_HANDLER	cgiHandler;
} CGI_HANDLER;
// property=UIMode&value=[VIDEO|CAMERA|BURST|TIMELAPSE|SETTING]
static int Handle_UIMode(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret, id;

	ret = NETAPP_DONE;	

#ifdef __CONFIG_SDK_LETV_CDV01__ //set tmp mode for demo
	printc("SET MODE to %s \n",val);
	if (strcmp(val, "PLAYBACK") == 0) {
		AHC_SetMode(AHC_MODE_IDLE);
	}else {
		AHC_SetMode(AHC_MODE_RECORD_PREVIEW);
	} 	
	amn_config_set(CONFIG_SPACE_TYPE_CURRENT,"Camera.Menu.UIMode", val);
#else
	if ((id = GetMenuIDbyCGIval(val, ui_state_cgi_menu)) != INVALID_ID) {
		// REVIEW: Call UI from NETWORK task directly!?
		netapp_CGIOpCallback(pfn_s);
		ret = Report2UIAndWaitFeedBack(WIRELESS_SET_UIMODE, id, pfn_w, (void*)6000);
	} else {
		ret = NETAPP_FAIL;
	}

#endif
	return ret;
}
// property=DefMode&value=[VIDEO|CAMERA|BURST|TIMELAPSE|SETTING]
static int Handle_DefMode(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret, id;

	ret = NETAPP_DONE;
	id = GetMenuIDbyCGIval(val, ui_state_cgi_menu);
	
	if (id != INVALID_ID) {
		//Report2UIAndWaitFeedBack(WIRELESS_SET_BOOTUPMODE,
		//						CameraSettingsFunc_GetBootUpMode(id), pfn_w, (void*)0);
		printc(">>>>> %s,%d Wifi TBD. <<<<<\r\n",__func__,__LINE__); //Andy Liu TBD
	} else {
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=Video&value=[record|capture]
static int Handle_VideoOperation(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret;

	if (stricmp(val, "record") == 0) {
		netapp_CGIOpCallback(pfn_s);
		ret = Report2UIAndWaitFeedBack(WIRELESS_RECORD, 0, pfn_w, (void*)6000);
	} else if (stricmp(val, "capture") == 0) {
		netapp_CGIOpCallback(pfn_s);
		ret = Report2UIAndWaitFeedBack(WIRELESS_CAPTURE, 0, pfn_w, (void*)6000);
	} else {
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=Videores&value=...
static int Handle_VideoResolution(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret, id;

	ret = NETAPP_DONE;	
	if (!NetApp_GetRecordStatus()) {
		id = GetMenuIDbyCGIval(val, videores_cgi_menu);
		if (id != INVALID_ID) {
			//Report2UIAndWaitFeedBack(WIRELESS_SET_MOVIE_SIZE, id, NULL, (void*)0);
			MenuSettingConfig()->uiMOVSize = id;
			if( uiGetCurrentState() == UI_VIDEO_STATE )
			{
				VideoTimer_Stop();
				VideoRecMode_PreviewUpdate();    
			}
		} else {
			ret = NETAPP_FAIL;
		}
	} else {
		ret = NETAPP_FAIL;	// the command failed!!
	}
	return ret;
}
// property=Imageres&value=...
static int Handle_ImageResolution(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret, id;

	ret = NETAPP_DONE;	
	id = GetMenuIDbyCGIval(val, imageres_cgi_menu);
	if (id != INVALID_ID) {
		//Report2UIAndWaitFeedBack(WIRELESS_SET_IMAGE_SIZE, id, NULL, (void*)0);
		MenuSettingConfig()->uiIMGSize = id;
		if( uiGetCurrentState() == UI_CAMERA_STATE ) {
			CaptureTimer_Stop();
			CaptureMode_PreviewUpdate();
		}
	} else {
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=Flicker&value=...
static int Handle_Flicker(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret, id;
	
	ret = NETAPP_DONE;
	id = GetMenuIDbyCGIval(val, flicker_cgi_menu);
	if (id != INVALID_ID) {
		//Report2UIAndWaitFeedBack(WIRELESS_SET_FLICKER, id, pfn_w, (void*)0);
		MenuSettingConfig()->uiFlickerHz = id;
		Menu_SetFlickerHz(MenuSettingConfig()->uiFlickerHz);
	} else {
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=AWB&value=...
static int Handle_WB(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret, id;
	
	ret = NETAPP_DONE;
	id = GetMenuIDbyCGIval(val, wb_cgi_menu);
	if (id != INVALID_ID) {
		//Report2UIAndWaitFeedBack(WIRELESS_SET_AWB, id, pfn_w, (void*)0);
		MenuSettingConfig()->uiWB = id;
		Menu_SetAWB(MenuSettingConfig()->uiWB);
	} else {
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=TimeSettings&value=...
static int Handle_TimeSettings(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret;
	int		mDatetime[6] = {2011,01,01,12,0,0};
	
	ret = NETAPP_DONE;
	netapp_DateTime_Parser(mDatetime, val);
#if WIFI_SEAL <= 1
	if(AHC_Validate_ClockSetting(mDatetime, CHECK_ALL, AHC_TRUE)==CHECK_PASS)
#endif
	{
		AHC_SetParam(PARAM_ID_USE_RTC,AHC_TRUE);
		AHC_SetClock(mDatetime[0],  mDatetime[1],  mDatetime[2], 0,
				     mDatetime[3],  mDatetime[4],  mDatetime[5], 0, 0);
	}
	return ret;
}
// property=Net&value=...
static int Handle_Net(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret;
	
	ret = NETAPP_DONE;
	if (stricmp(val, "reset") == 0) {
		netapp_ResetNetwork();
	} else if (stricmp(val, "findme")==0) {
		AHC_PlaySoundEffect(AHC_SOUNDEFFECT_FINDME);
		AHC_BuzzerAlert(BUZZER_SOUND_HZ, 1, 1000);
	}
	return ret;
}
// property=MJPEG&value=...
#define MJPEG_CFG_TAG "Camera.Preview.MJPEG."
static int Handle_MJPEG(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret;
	
	ret = NETAPP_DONE;
    key += sizeof(MJPEG_CFG_TAG) - 1;
    if (strcmp(key, "bitrate") == 0) {
		// return 0, the AMN Config will store the value
        ret = NETAPP_CONT;
    }
    else if (strcmp(key, "w") == 0) {//Warning! State machine! "h" must be set after this.
        //DO NOTHING
		// return 0, the AMN Config will store the value
        ret = NETAPP_CONT;
    }
    else if (strcmp(key, "h") == 0) {//warning! State machine! This based on "w" & "fps"
        //DO NOTHING
		// return 0, the AMN Config will store the value
        ret = NETAPP_CONT;
    }
    else if (strcmp(key, "fps") == 0) {
        //DO NOTHING
		// return 0, the AMN Config will store the value
        ret = NETAPP_CONT;
    }
    else {
        ret = NETAPP_FAIL;
    }
    return ret;
}
// property=EV&value=...
static int Handle_EV(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret, id;
	
	ret = NETAPP_DONE;
	id = GetMenuIDbyCGIval(val, expo_cgi_menu);
	if (id != INVALID_ID) {
		//Report2UIAndWaitFeedBack(WIRELESS_SET_EV, id, pfn_w, (void*)0);
		MenuSettingConfig()->uiEV = id;
		Menu_SetEV(MenuSettingConfig()->uiEV);
	} else {
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=MTD&value=...
static int Handle_MTD(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret, id;
	
	ret = NETAPP_DONE;
	id = GetMenuIDbyCGIval(val, mtd_cgi_menu);
	if (id != INVALID_ID) {
		extern void AHC_SwitchMotionDetectionMode(void);
		MenuSettingConfig()->uiMotionDtcSensitivity = id;
		Menu_SetMotionDtcSensitivity(MenuSettingConfig()->uiMotionDtcSensitivity);
		// Switch MVD mode when sensitivity is changed
		AHC_SwitchMotionDetectionMode();
	} else {
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=PhotoBurst&value=...
static int Handle_PhotoBurst(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret, id;
	
	ret = NETAPP_DONE;
	id = GetMenuIDbyCGIval(val, photo_burst_cgi_menu);
	if (id != INVALID_ID) {
		Report2UIAndWaitFeedBack(WIRELESS_SET_PHOTOBURST, id, pfn_w, (void*)0);
	} else {
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=Timelapse&value=...
static int Handle_Timelapse(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret, id;
	
	ret = NETAPP_DONE;
	id = GetMenuIDbyCGIval(val, timelapse_cgi_menu);
	if (id != INVALID_ID) {
		Report2UIAndWaitFeedBack(WIRELESS_SET_TIMELAPSE, id, pfn_w, (void*)0);
	} else {
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=UpsideDown&value=...
static int Handle_UpsideDown(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret, id;
	
	ret = NETAPP_DONE;
	id = GetMenuIDbyCGIval(val, upsidedown_cgi_menu);
	if (id != INVALID_ID) {
		Report2UIAndWaitFeedBack(WIRELESS_SET_UPSIDEDOWN, id, pfn_w, (void*)0);
	} else {
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=SpotMeter&value=...
static int Handle_SpotMeter(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret, id;
	
	ret = NETAPP_DONE;
	id = GetMenuIDbyCGIval(val, spot_meter_cgi_menu);
	if (id != INVALID_ID) {
		Report2UIAndWaitFeedBack(WIRELESS_SET_SPOTMETER, id, pfn_w, (void*)0);
	} else {
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=LoopingVideo&value=...
static int Handle_LoopingVideo(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret, id;
	
	if (NetApp_GetRecordStatus())
		return NETAPP_FAIL;
	ret = NETAPP_DONE;
	id = GetMenuIDbyCGIval(val, looping_video_cgi_menu);
	if (id != INVALID_ID) {
		//Report2UIAndWaitFeedBack(WIRELESS_SET_LOOPINGVIDEO, id, pfn_w, (void*)0);
		MenuSettingConfig()->uiMOVClipTime = id;
	} else {
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=VideoQuality&value=...
static int Handle_VideoQuality(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret, id;
	
	if (NetApp_GetRecordStatus())
		return NETAPP_FAIL;
	ret = NETAPP_DONE;
	id = GetMenuIDbyCGIval(val, quality_cgi_menu);
	if (id != INVALID_ID) {
		Report2UIAndWaitFeedBack(WIRELESS_SET_VQUALITY, id, pfn_w, (void*)0);
	} else {
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=HDR&value=...
static int Handle_HDR(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret, id;
	
	if (NetApp_GetRecordStatus())
		return NETAPP_FAIL;
	ret = NETAPP_DONE;
	id = GetMenuIDbyCGIval(val, hdr_cgi_menu);
	if (id != INVALID_ID) {
		Report2UIAndWaitFeedBack(WIRELESS_SET_HDR, id, pfn_w, (void*)0);
	} else {
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=Q-SHOT&value=...
static int Handle_Q_SHOT(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret, id;
	
	if (NetApp_GetRecordStatus())
		return NETAPP_FAIL;
	ret = NETAPP_DONE;
	id = GetMenuIDbyCGIval(val, q_shot_cgi_menu);
	if (id != INVALID_ID) {
		Report2UIAndWaitFeedBack(WIRELESS_SET_Q_SHOT, id, pfn_w, (void*)0);
	} else {
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=StatusLights&value=...
static int Handle_StatusLights(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret, id;
	
	ret = NETAPP_DONE;
	id = GetMenuIDbyCGIval(val, status_lights_cgi_menu);
	if (id != INVALID_ID) {
		Report2UIAndWaitFeedBack(WIRELESS_SET_STATUSLIGHTS, id, pfn_w, (void*)0);
	} else {
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=SoundIndicator&value=...
static int Handle_SoundIndicator(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret, id;
	
	ret = NETAPP_DONE;
	id = GetMenuIDbyCGIval(val, sound_indicator_cgi_menu);
	if (id != INVALID_ID) {
		// TODO: Sound Indicator ON/OFF
		//Report2UIAndWaitFeedBack(WIRELESS_SET_SOUNDINDICATOR, id, pfn_w, (void*)0);
		MenuSettingConfig()->uiBeep = id;
	} else {
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=PowerSaving&value=...
static int Handle_PowerSaving(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret, id;
	
	ret = NETAPP_DONE;
	id = GetMenuIDbyCGIval(val, power_saving_cgi_menu);
	if (id != INVALID_ID) {
		Report2UIAndWaitFeedBack(WIRELESS_SET_POWERSAVING, id, pfn_w, (void*)0);
	} else {
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=TVSystem&value=...
static int Handle_TVSystem(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	// TV Output (NTSC/PAL)
	int		ret, id;
	
	ret = NETAPP_DONE;
	id = GetMenuIDbyCGIval(val, tv_system_cgi_menu);
	if (id != INVALID_ID) {
		Report2UIAndWaitFeedBack(WIRELESS_SET_TV_SYS, id, pfn_w, (void*)0);
	} else {
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=TimeFormat&value=...
static int Handle_TimeFormat(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	// Time format (YY/MM/DD, DD/MM/YY...)
	int		ret, id;
	
	if (NetApp_GetRecordStatus())
		return NETAPP_FAIL;
	ret = NETAPP_DONE;
	id = GetMenuIDbyCGIval(val, time_fmt_cgi_menu);
	if (id != INVALID_ID) {
		Report2UIAndWaitFeedBack(WIRELESS_SET_TIME_FMT, id, pfn_w, (void*)0);
	} else {
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=LCDPowerSave&value=...
static int Handle_LCDPowerSave(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	// LCDPowerSave
	int		ret, id;
	
	ret = NETAPP_DONE;
	id = GetMenuIDbyCGIval(val, lcd_save_cgi_menu);
	if (id != INVALID_ID) {
		Report2UIAndWaitFeedBack(WIRELESS_SET_LCD_PWRS, id, pfn_w, (void*)0);
	} else {
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=PowerOffDelay&value=...
static int Handle_PowerOffDelay(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	// LCDPowerSave
	int		ret, id;
	
	ret = NETAPP_DONE;
	id = GetMenuIDbyCGIval(val, off_delay_cgi_menu);
	if (id != INVALID_ID) {
		//Report2UIAndWaitFeedBack(WIRELESS_SET_POWROFF_DELAY, id, pfn_w, (void*)0);
		MenuSettingConfig()->uiMOVPowerOffTime = id;
	} else {
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=SD0&value=format
static int Handle_SD0(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret, id;
	
	if (NetApp_GetRecordStatus())
		return NETAPP_FAIL;
	ret = NETAPP_DONE;
	id = GetMenuIDbyCGIval(val, sd0_cgi_menu);
	if (id != INVALID_ID) {
		// TODO: Format SD0
		//Report2UIAndWaitFeedBack(WIRELESS_FORMAT_SD0, id, pfn_w, (void*)0);
		//printc(">>>>> %s,%d Wifi TBD. <<<<<\r\n",__func__,__LINE__); //Andy Liu TBD
		_format_sd = FORMAT_SD_DOING;
		if (AHC_FormatStorageMedia(AHC_MEDIA_MMC) &&
        	AHC_MountStorageMedia(AHC_MEDIA_MMC)  &&
        	AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect)) {
        	_format_sd = FORMAT_SD_DONE;
        } else {
        	_format_sd = FORMAT_SD_ERROR;
        }
	} else {
		_format_sd = FORMAT_SD_ERROR;
		ret = NETAPP_FAIL;
	}
	return ret;
}
// property=FactoryReset&value=Camera
static int Handle_FactoryReset(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret, id = 0;
	
	if (NetApp_GetRecordStatus())
		return NETAPP_FAIL;
	ret = NETAPP_DONE;
	if (id != INVALID_ID) {
		printc("Factory Reset!! - %s\n", key);
		if (stricmp(key, "Camera") == 0) {
			// TODO: Do factory reset
			//void RestoreFromDefaultSetting(void);
			//RestoreFromDefaultSetting();
			AHC_RestoreFromDefaultSetting();//CarDV
		}
	} else {
		//_format_sd = FORMAT_SD_ERROR;
		ret = NETAPP_FAIL;
	}
	return ret;
}

#ifdef NET_SYNC_PLAYBACK_MODE
// property=Playback&value=[enter|exit]
static int Handle_Playback(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret;
	
	ret = NETAPP_DONE;
			
	if (stricmp(val, "enter") == 0){
		netapp_CGIOpCallback(pfn_s);
		ret = Report2UIAndWaitFeedBack(WIRELESS_ENTER_PLAYBACK, 0, pfn_w, (void*)6000);
	}
	else{
		netapp_CGIOpCallback(pfn_s);
		ret = Report2UIAndWaitFeedBack(WIRELESS_EXIT_PLAYBACK, 0, pfn_w, (void*)6000);
	}
	return ret;
}
#endif

#define	CGI_CMD(k)	(k), sizeof(k) - 1
/* Note: the key compared with cmdKeyLen, be care for cmdKey being Substring of key */
CGI_HANDLER cgi_handler[] = {
	{CGI_CMD("UIMode"),			Handle_UIMode},
	{CGI_CMD("DefMode"),		Handle_DefMode},
	{CGI_CMD("Video"),			Handle_VideoOperation},
	{CGI_CMD("Videores"),		Handle_VideoResolution},
	{CGI_CMD("VideoClipTime"),	Handle_LoopingVideo},
	{CGI_CMD("VideoQuality"),	Handle_VideoQuality},
	{CGI_CMD("Imageres"),		Handle_ImageResolution},
	{CGI_CMD("Exposure"),		Handle_EV},
	{CGI_CMD("EV"),				Handle_EV},
	{CGI_CMD("MTD"),			Handle_MTD},
	{CGI_CMD("Flicker"),		Handle_Flicker},
	{CGI_CMD("Net"),			Handle_Net},
	{CGI_CMD(MJPEG_CFG_TAG),	Handle_MJPEG},
	{CGI_CMD("TimeSettings"),	Handle_TimeSettings},
	{CGI_CMD("AWB"),			Handle_WB},
	{CGI_CMD("PhotoBurst"),		Handle_PhotoBurst},
	{CGI_CMD("Timelapse"),		Handle_Timelapse},
	{CGI_CMD("UpsideDown"),		Handle_UpsideDown},
	{CGI_CMD("SpotMeter"),		Handle_SpotMeter},
	{CGI_CMD("LoopingVideo"),	Handle_LoopingVideo},
	{CGI_CMD("HDR"),			Handle_HDR},
	{CGI_CMD("Q-SHOT"),			Handle_Q_SHOT},
	{CGI_CMD("StatusLights"),	Handle_StatusLights},
	{CGI_CMD("Camera.Menu.SoundIndicator"), Handle_SoundIndicator},
	{CGI_CMD("SoundIndicator"),	Handle_SoundIndicator},
	{CGI_CMD("PowerSaving"),	Handle_PowerSaving},
	{CGI_CMD("TVSystem"),		Handle_TVSystem},
	{CGI_CMD("Beep"),			Handle_SoundIndicator},
	{CGI_CMD("AutoPowerOff"),	Handle_PowerSaving},
	{CGI_CMD("TimeFormat"),		Handle_TimeFormat},
	{CGI_CMD("LCDPowerSave"),	Handle_LCDPowerSave},
	{CGI_CMD("PowerOffDelay"),	Handle_PowerOffDelay},
	{CGI_CMD("SD0"),			Handle_SD0},
	{CGI_CMD("FactoryReset"),	Handle_FactoryReset},
#ifdef NET_SYNC_PLAYBACK_MODE
	{CGI_CMD("Playback"),		Handle_Playback},
#endif
	// End of Map
	{NULL, NULL}
};
int wildstrcmp(char *s1, const char* ws2, int ws2len)
{
	if (*(ws2 + ws2len - 1) == '.') {
		return strncmp(s1, ws2, ws2len);
	}
	return strcmp(s1, ws2);
}
/*
 * the command has not processed yet -> return  0
 * the command processes failed      -> return -2
 * the command processed             -> return -1
 */
int NetApp_SetKeyevent(char *key, char *val, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int	ret;
 	int	i;
 
 	ret = NETAPP_DONE;	// finish job, not to do any more after return.
	for (i = 0; cgi_handler[i].cmdKey != NULL; i++) {
		if (wildstrcmp(key, cgi_handler[i].cmdKey, cgi_handler[i].cmdKeyLen) == 0 &&
			cgi_handler[i].cgiHandler != NULL) {
			ret = cgi_handler[i].cgiHandler(key, val, pfn_w, pfn_s);
			if( ret == NETAPP_DONE)
				Menu_WriteSetting();
			
			return ret;
		}
	} 	
	//SetKey_Unprocessed:
    printc("unprocessed key:%s value:%s\r\n", key, val);
    ret = 0;	// not finish yet, do something after exit this function.
	return ret;
}

//get current record mode status.
int NetApp_GetRecordStatus()
{
	return VideoFunc_RecordStatus();	
}

// Get timestamp of still capture
int NetApp_GetCaptureStatus()
{
	return 0;
}

static WMSG_INFO m_web_WMSG_Info;

//get if it is on warning state.
int netapp_warning_msg_state()
{
	if(m_web_WMSG_Info == WMSG_NONE) {
        return AHC_FALSE;
    }
    else {
        return AHC_TRUE;
    }
}

//get warning message information.It still have many to implement.
int netapp_get_warning_msg_info()
{
	switch(m_web_WMSG_Info)
	{
		case WMSG_NO_CARD:
			set_netapp_WMSGInfo(WMSG_NONE);		
			return 1;
			
		default:
			break;
	
	}
	return 0;
}

//set warning message information.
int set_netapp_WMSGInfo(int m_WMSG_Info)
{
	m_web_WMSG_Info = (WMSG_INFO)m_WMSG_Info;
	return 0;
}

//get value of year from RTC time. 
int netapp_get_TimeStamp_year()
{
    AHC_RTC_TIME sRtcTime;
    AHC_RTC_GetTime(&sRtcTime);

    return sRtcTime.uwYear;		
}

//get value of month from RTC time.
int netapp_get_TimeStamp_month()
{
    AHC_RTC_TIME sRtcTime;
    AHC_RTC_GetTime(&sRtcTime);

    return sRtcTime.uwMonth;		
}

//get value of day from RTC time.
int netapp_get_TimeStamp_day()
{
    AHC_RTC_TIME sRtcTime;
    AHC_RTC_GetTime(&sRtcTime);

    return sRtcTime.uwDay;		
}

//get value of hour from RTC time.
int netapp_get_TimeStamp_hour()
{
    AHC_RTC_TIME sRtcTime;
    AHC_RTC_GetTime(&sRtcTime);

    return sRtcTime.uwHour;		
}

//get value of minute from RTC time.
int netapp_get_TimeStamp_minute()
{
    AHC_RTC_TIME sRtcTime;
    AHC_RTC_GetTime(&sRtcTime);

    return sRtcTime.uwMinute;		
}

//get value of second from RTC time.
int netapp_get_TimeStamp_second()
{
    AHC_RTC_TIME sRtcTime;
    AHC_RTC_GetTime(&sRtcTime);

    return sRtcTime.uwSecond;		
}

//get Video Resolution.
const char* netapp_get_Video_Resolution()
{	
	return GetCGIvalbyMenuId(MenuSettingConfig()->uiMOVSize, videores_cgi_menu);
}

//get video clip time
const char* netapp_get_VideoClipTime()
{
	return GetCGIvalbyMenuId(MenuSettingConfig()->uiMOVClipTime,
							 videoclip_cgi_menu);
}

//get Image Resolution.
const char* netapp_get_Image_Resolution()
{	
	return GetCGIvalbyMenuId(MenuSettingConfig()->uiIMGSize,
							 imageres_cgi_menu);
}
// get White balance
const char* netapp_get_WB()
{	
	return GetCGIvalbyMenuId(MenuSettingConfig()->uiWB,
							 wb_cgi_menu);
}
// get flicker (50HZ, 60HZ)
const char* netapp_get_Flicker_Hz()
{	
	return GetCGIvalbyMenuId(MenuSettingConfig()->uiFlickerHz,
							 flicker_cgi_menu);
}
// get Motion Detection Level
const char* netapp_get_MTD_Level()
{
	return GetCGIvalbyMenuId(MenuSettingConfig()->uiMotionDtcSensitivity,
							 mtd_cgi_menu);
}
// get Exposure
const char* netapp_get_EV_Level()
{
	return GetCGIvalbyMenuId(MenuSettingConfig()->uiEV,
							 expo_cgi_menu);
}
// get Power off delay
const char* netapp_get_PowerOffDelay()
{
	return GetCGIvalbyMenuId(MenuSettingConfig()->uiMOVPowerOffTime,
							 off_delay_cgi_menu);
}
// get version
__weak char* AHC_CB_GetFwVersion(void);
const char* netapp_get_FW_Version()
{
	#define	MAX_VERSION_LEN	20
	static char version[MAX_VERSION_LEN] = {0};
	char	*ver;
	int		l;
	
	if (version[0] != 0) return version;
	l = 0;
	if (AHC_CB_GetFwVersion) {
		ver = AHC_CB_GetFwVersion();
		for (; *ver != 0; ver++) {
			if (isdigit(*ver)) version[l++] = *ver;
		}
	}
	snprintf(version + l, MAX_VERSION_LEN - 1 - l, "%04d", AHC_GetMinorVersion());
	return version;
}

int netapp_get_MjpegBitrate(void)
{
    UINT32 bitrate;
    UINT32 codecType;
    AIHC_GetMovieConfigEx(1, AHC_VIDEO_CODEC_TYPE, &codecType);
    AIHC_GetMovieConfigEx(1, AHC_VIDEO_BITRATE, &bitrate);
    if ((codecType == AHC_MOVIE_VIDEO_CODEC_MJPEG) || (codecType == AHC_MOVIE_VIDEO_CODEC_YUV422)) {
        return bitrate;
    }
    else {
        //default bitrate, make it weird so that we could tell it's default value
        printc("\r\n codec Type %d\r\n", codecType);
        return 4000001;
    }
}

int netapp_get_MjpegResolution(unsigned short *w, unsigned short *h, char *name)
{
    AHC_BOOL ret = 0;
    int configWidth, configHeight;

    netapp_cfg_get("Camera.Preview.MJPEG.w", &configWidth);
    netapp_cfg_get("Camera.Preview.MJPEG.h", &configHeight);
    AIHC_AdjustJpegFromSensor(configWidth, configHeight, w, h);
    printc(">>>> CGI Get %s %d %d\r\n", name, *w, *h);
    return ret;
}

int netapp_get_H264Bitrate(void)
{
    UINT32 bitrate;
    
    netapp_cfg_get("Camera.Preview.H264.bitrate", &bitrate);
    return bitrate;
}

static NETAPP_VALUE m_values[NADS_MAX] = {
    {VALUE_TYPE_INT, NADS_AUDIO_SAMPLING_RATE},
    {VALUE_TYPE_INT, NADS_AUDIO_PREVIEW},
    {VALUE_TYPE_STR, NADS_TRANSCODE_FILE_NAME},
    {VALUE_TYPE_INT, NADS_FILE_DURATION}
};

int netapp_get_DeviceSetting(enum NETAPP_DEVICE_SETTING setting, NETAPP_VALUE* v)
{
    if (NULL == v) {
        return 0;
    }
    if (setting >= NADS_MAX) {
        printc("Invalid net app get setting:%d\r\n", setting);
        v->type = VALUE_TYPE_UNDEFINED;
        v->v.ptr = NULL;
        return 0;
    }

    v->type = m_values[setting].type;
    switch(setting) {
    case NADS_AUDIO_SAMPLING_RATE:
        //m_values[setting].v.i32 = AHC_GetDefaultAudioSamplingRate();
        break;
    default:
        break;
    }

    v->v = m_values[setting].v;
    return 1;
}

int netapp_set_DeviceSetting(enum NETAPP_DEVICE_SETTING setting, NETAPP_VALUE* v)
{
    if (NULL == v) {
        return 0;
    }
    if (setting >= NADS_MAX) {
        printc("Invalid net app set setting:%d\r\n", setting);
        return 0;
    }
    if (v->type != m_values[setting].type) {
        printc("unmatched net app setting:%d type %d, expected type %d\r\n", setting, v->type, m_values[setting].type);
        return 0;
    }

    m_values[setting].v = v->v;


    //add special post setting code here
    switch(setting) {
    //case NADS_AUDIO_PREVIEW:
    //    break;
    default:
        break;
    }
    return 1;
}

int netapp_cfg_get(char* key, void* val)
{
    struct amn_cfgspace_value_t *cfg_val;
    char* cfg_name = key;
    int i = amn_config_locate(key);
    if (i >= 0) {
        cfg_val  = amn_config_get( CONFIG_SPACE_TYPE_CURRENT, cfg_name );
        if (cfg_val->type == CFGSPACE_VALTYP__STR)
        	val = cfg_val->v.strVal;
        else if (cfg_val->type == CFGSPACE_VALTYP__INT)
        	*(int*)val = cfg_val->v.intVal;
        else
        	return -1;	// ERROR!!
    }
    return i;
}

// Broadcast frame rate of video source (v4L) to each AMN NODE
int netapp_set_framerate(int fps, char* name)
{
    struct ammo_node* node = NULL;

    node = node_find_name( AMMO_TYPE__NODE_SOURCE, name, -1 );
    if (NULL == node) {
        printc("net ass framerate <NODE_NAME> in out\r\n <NODE_NAME> supports MJPEG only\r\n");
        return ASS_ERR_PARAM;
    }
   	return pipeline_set_node_attr_all(node, PLCFG__SET_FRAMERATE, (void*)fps);
}

//set frame rate conversion
int netapp_set_FRC(int inx1000, int outx1000, char* name)
{
    struct plcfg_frc_t frc;
    struct ammo_node* node = NULL;
    ass_err err;

    node = node_find_name( AMMO_TYPE__NODE_SOURCE, name, -1 );
    if (NULL == node) {
        printc("net ass frc <NODE_NAME> in out\r\n <NODE_NAME> supports MJPEG only\r\n");
        return ASS_ERR_PARAM;
    } else {
        frc.inx1000 = inx1000;
        frc.outx1000 = outx1000;
        err = node_set_attr(node, PLCFG__SET_FRC, &frc);
        if (ASS_ERR_NONE != err) {
            printc(BG_RED("Error(%d): %s %d setting node %s\r\n"), err, __func__, __LINE__, name);
            return err;
        }
    }
    return ASS_ERR_NONE;
}

int Netapp_set_status(int status)
{
	if(_netappstatus == NETAPP_WIFI_DOWN && 
		status != NETAPP_WIFI_IDLE){
		return NETAPP_WIFI_DOWN;
	}
		
    if (IS_WIFI_FAIL(status)) {
        SET_WIFI_FAIL(_netappstatus);
        //if (_dispcnt == 0) {
        //	PlayWiFiErrorSound();
        //	AHC_PlaySoundEffect(AHC_SOUNDEFFECT_WIFIFAIL);
		//}
        _dispcnt = ERR_DISP_CNT;
    } else {
        _netappstatus = status;
    }
    return _netappstatus;
}
int Netapp_get_status(void)
{
    if (IS_WIFI_FAIL(_netappstatus)) {
        if (_dispcnt == 0) {
            CLR_WIFI_FAIL(_netappstatus);
            _dispcnt = ERR_DISP_CNT;
        } else {
            _dispcnt--;
        }
    }
    return _netappstatus;
}

int Netapp_wifi_link_report(int updown)
{
	if(updown)
		return (int)SEND_NETWORK_EVENT(SYS_FLAG_MSG_NETWORK_WIFILINK_IND);
	else
		return (int)SEND_NETWORK_EVENT(SYS_FLAG_MSG_NETWORK_WIFILINK_DOWN_IND);
}

int netapp_InitNetwork()
{
	return (int)SEND_NETWORK_EVENT(SYS_FLAG_MSG_NETWORK_INIT);
}

int netapp_StopNetwork()
{
	return (int)SEND_NETWORK_EVENT(SYS_FLAG_MSG_NETWORK_UNIN);
}

int netapp_ResetNetwork()
{
	return (int)SEND_NETWORK_EVENT(SYS_FLAG_MSG_NETWORK_RESET);
}

int netapp_CmdNetwork()
{
	return (int)SEND_NETWORK_EVENT(SYS_FLAG_MSG_NETWORK_CMD);
}

void netapp_Pump(void)
{
    extern MMPF_OS_FLAGID   Streaming_Flag;
    MMP_UBYTE MMPF_OS_SetFlags(MMPF_OS_FLAGID ulFlagID, MMPF_OS_FLAGS flags, MMPF_OS_FLAG_OPT opt);
    MMPF_OS_SetFlags(Streaming_Flag, STREAM_FLAG_PUMP_STREAM, MMPF_OS_FLAG_SET);
}

#include "AHC_UF.h"
int netapp_DeleteFile(char *path)
{
	char	*ptr;
	
	ptr = strrchr(path, '\\');
	if (!ptr) return -1;
	*ptr = 0;
	ptr++;
	return (AHC_UF_FileOperation((unsigned char*)path,
					    		  (unsigned char*)ptr,
						    	  DCF_FILE_DELETE,
							      NULL, NULL) != 0)? 0 : -1;
}

//Turn a time string into an integer
void netapp_DateTime_Parser(int* datetime, char* timestr)
{
	int		i;
	char	*tok;

	//Year/Month/Day/Hour/Minute/Second
	tok = strtok(timestr, "$");	
	for (i = 0; i < 6 && tok != NULL; i++) {
		datetime[i] = atoi(tok);
		tok = strtok(NULL,"$"); 
	}
	for (; i < 6; i++) datetime[i] = 0;
	//printc("netapp_DateTime_Parser : %d %d %d %d %d %d\r\n",datetime[0],datetime[1],datetime[2],datetime[3],datetime[4],datetime[5]);
}

//identify the sensor is working or not
// TODO: review it, Canlet
const char* netapp_SensorStatus()
{
	AHC_MODE_ID	mode;
	
	mode = AHC_GetAhcSysMode();
	switch (mode) {
	case AHC_MODE_VIDEO_RECORD:
	case AHC_MODE_RECORD_VONLY:
	case AHC_MODE_RECORD_PREVIEW:
		return "Videomode";	// for compitable with old version
	case AHC_MODE_CAPTURE_PREVIEW:
		return netapp_GetUIState();
	}
	if (netapp_getStreamingMode()) {
		// for transcoding
		return "Streaming";
	}
	return "Idlemode";
}
/*
 * for H264 streaming to get SPS/PPS at RTSP starting.
 */
int AHC_OpenVideoStream(void *handle);
int AHC_CloseVideoStream(void *handle);
int netapp_Enableh264Stream(int bEnabled)
{
    int w, h;
    MOVIESIZE_SETTING res;

	if (bEnabled) {
        AHC_SetMovieConfig(1, AHC_VIDEO_CODEC_TYPE, AHC_MOVIE_VIDEO_CODEC_H264);
        AHC_SetMovieConfig(1, AHC_VIDEO_STREAMING, AHC_TRUE);
        netapp_cfg_get("Camera.Preview.H264.w", &w);
        netapp_cfg_get("Camera.Preview.H264.h", &h);
        if (w == 1920 && h == 1080) {
            res = MOVIE_SIZE_1080_30P;
        } else if (w == 1280 && h == 720) {
            res = MOVIE_SIZE_720_30P;
        } else if (w == 640 && h == 360) {
            res = MOVIE_SIZE_360_30P;
        } else {
            bEnabled = 0;
        }
        if (bEnabled) {
            VideoFunc_SetResolution(res);
            AHC_OpenVideoStream(NULL);
        }
	} else {
		AHC_CloseVideoStream(NULL);
	}
	return bEnabled; 
}

/*
 * return 1: AP mode
 *        2: STA mode
 */
unsigned int netapp_GetIPAddress()
{
	return main_netdev.ip_addr.addr;
}
/*
 * return unsigned int (little endian) for ipv4
 *        ip: xxx.xxx.xxx.xxx = byte0.byte1.byte2.byte3
 */
int netapp_GetWiFiMode()
{
	return net_context.mode;
}
/*
 * Return UI State CGI value string
 */
const char* netapp_GetUIState()
{
	int	state;
	state = (int)uiGetCurrentState();
	return GetCGIvalbyMenuId(state, ui_state_cgi_menu);
}
/*
 * Return Default UIMode after bootup
 */
const char* netapp_GetDefMode()
{
	int state;
	// Convert menu indxe to setting id	
#ifdef CAR_DV
	printc(">>>>> %s,%d Wifi TBD. Force at UI_VIDEO_STATE <<<<<\r\n",__func__,__LINE__); //Andy Liu TBD
	state = UI_VIDEO_STATE;
	//state = CameraSettingsFunc_GetBootUpUIState(MenuSettingConfig()->uiBootUpMode);
#else
	state = CameraSettingsFunc_GetBootUpUIState(MenuSettingConfig()->uiBootUpMode);
#endif

	return GetCGIvalbyMenuId(state, ui_state_cgi_menu);
}
/*
 * Return UI State CGI value string by ID
 */
const char* netapp_GetUIStateEx(int uId)
{
	const char	*p;
	
	p = GetCGIvalbyMenuId(uId, ui_state_cgi_menu);
	return (p)? p : "UNKNOWN";
}
/*
 * Broadcast SSID or not
 * return 1 : OK
 *        0 : FAILED
 */
int netapp_BroadcastSSID(int enable)
{
	#define	SSID_BROADCAST	0
	#define	SSID_HIDDEN		1
	extern int wlmClosedSet(int val);
	return wlmClosedSet((enable == 0)? SSID_HIDDEN : SSID_BROADCAST);
}
/*
 * Get which SET for WiFi AP used.
 * return 0 or 1 for AP configuration set 0 or 1
 */
int netapp_GetWiFi_AP_set()
{
    printc(">>>>> %s,%d Wifi TBD. <<<<<\r\n",__func__,__LINE__); //Andy Liu TBD
    return 0;
	//return (MenuSettingConfig()->uiWirelessStatus == WIRELESS_STATUS_REMOTE)? 1 : 0;
}
/*
 * Get camera status
 */
const char* netapp_get_PhotoBurst(void)
{
	return GetCGIvalbyMenuId(MenuSettingConfig()->uiBurstShot, photo_burst_cgi_menu);
}
const char* netapp_get_Timelapse(void)
{
	return GetCGIvalbyMenuId(MenuSettingConfig()->uiTimeLapseTime, timelapse_cgi_menu);
}
const char* netapp_get_Upsidedown(void)
{
	return GetCGIvalbyMenuId(MenuSettingConfig()->uiImageUpsideDown, upsidedown_cgi_menu);
}
const char* netapp_get_HDR(void)
{
	return (char*)GetCGIvalbyMenuId(MenuSettingConfig()->uiWDR, hdr_cgi_menu);
}
const char* netapp_get_LoopingVideo(void)
{
    UINT32 clipTime;

    if (AHC_Menu_SettingGetCB(COMMON_KEY_VR_CLIP_TIME, &clipTime) == AHC_FALSE)
        clipTime = MOVIE_CLIP_TIME_OFF;     

	return GetCGIvalbyMenuId(clipTime, looping_video_cgi_menu);
}
const char* netapp_get_SpotMeter(void)
{
	return GetCGIvalbyMenuId(MenuSettingConfig()->uiMetering, spot_meter_cgi_menu);
}
const char* netapp_get_Q_SHOT(void)
{
	return GetCGIvalbyMenuId(MenuSettingConfig()->uiAutoRec, q_shot_cgi_menu);
}
const char* netapp_get_StatusLights(void)
{
	return GetCGIvalbyMenuId(MenuSettingConfig()->uiStatusLight, status_lights_cgi_menu);
}
const char* netapp_get_SoundIndicator(void)
{
	return GetCGIvalbyMenuId(MenuSettingConfig()->uiBeep, sound_indicator_cgi_menu);
}
const char* netapp_get_PowerSaving(void)
{
	return GetCGIvalbyMenuId(MenuSettingConfig()->uiAutoPowerOff, power_saving_cgi_menu);
}
const char* netapp_get_TVSystem(void)
{
	return GetCGIvalbyMenuId(MenuSettingConfig()->uiTVSystem, tv_system_cgi_menu);
}
// Get Camera with HTMI status
const char* netapp_get_TVStatus(void)
{
#if (HDMI_PREVIEW_EN)
	extern AHC_BOOL AHC_IsHdmiConnect(void);
	if (AHC_IsHdmiConnect()){
        return (const char *) "HDMI";
    }
#endif

    return (const char *) "NONE";
}
// Get SD0 Status [NONE|READY|FORMATING|ERROR]
AHC_BOOL MenuFunc_CheckAvailableSpace( UINT8 uiCameraState );
const char* netapp_get_SD0Status()
{
	if (MenuSettingConfig()->uiMediaSelect == MEDIA_SETTING_SD_CARD) {
		switch (_format_sd) {
		case FORMAT_SD_NONE:
		case FORMAT_SD_DONE: {
			#if 0
			if (MenuFunc_CheckAvailableSpace( uiGetCurrentState() ) == AHC_TRUE)
				return "READY";
			else {
				printc(">>>>> %s,%d Wifi TBD. <<<<<\r\n",__func__,__LINE__); //Andy Liu TBD
				return "CARDFULL";
			}
			#else
			return "READY";
			#endif
		}
		
		case FORMAT_SD_DOING:
			return "FORMATING";
		default:
			return "ERROR";
		}
	}
	return "NONE";
}
// Format SD0 status
int netapp_set_SD0Status(int status)
{
	if (_format_sd != status) {
		_format_sd = status;
		netapp_UINotifyChanged(NULL);
	}
	return _format_sd;
}
// Save network configures in storage
int netapp_SaveNetworkConf()
{
	export_net_config_file(CONFIG_SPACE_TYPE_CURRENT, NCFG_USERSET_FILE_NAME);
	return 0;
}
/*
 * the function called must be in streaming.
 * Get Recording Start time in streaming time line
 */
MMP_ULONG MMPF_StreamingTimer_GetMs(void);
//
int netapp_get_RecordTimeStart()
{
	int		total, curr;

	AHC_GetCurrentRecordingTime((unsigned int*)&total); 
    total += (unsigned int)VideoFunc_GetRecordTimeOffset();
	curr  = MMPF_StreamingTimer_GetMs();
	printc(BG_RED("Total %d at %d start AT %d\n"), total, curr, curr - total);
	// if the start time is negtive, it means Recording start before streaming.
	return curr - total;
}
//
int netapp_get_RecordTimeTotal()
{
	//Total elapsed time of recording ,it will be reset to 0 when stopping record
	unsigned int ultime;

	AHC_GetCurrentRecordingTime(&ultime); 
    ultime += VideoFunc_GetRecordTimeOffset();
//    printc("TOTAL %d at %d\n", (int)ultime, MMPF_Streaming_GetTime());
	return (int)ultime;
}
int netapp_get_RecordTimeRemaining()
{
	//Total remaining time for record in SD Card, include current elapsed time of recording
	int vRemainingInSec;

#if 0
	//vRemainingInSec = 	CameraSettingConfig()->byRemainingHour * 3600 + //Andy Liu TBD
	//					CameraSettingConfig()->byRemainingMinute * 60 +
	//					CameraSettingConfig()->byRemainingSecond;
#else
    unsigned char bHour, bMin, bSec;

    AHC_AvailableRecordTime(&bHour, &bMin, &bSec);
    vRemainingInSec =   bHour * 3600 +
                        bMin * 60 +
                        bSec;
#endif
	
	return vRemainingInSec;
}
int netapp_get_RecordTimeAvailable()
{
	//Total available time for record in SD Card : 
	//Total remaining time for record in SD Card - current elapsed time of recording
	int vAvailableInSec;
	int vRecrdTimeInSec;
	int vRemainingInSec;

#if 0
	//vRemainingInSec =	CameraSettingConfig()->byRemainingHour * 3600 + //Andy Liu TBD
    //					CameraSettingConfig()->byRemainingMinute * 60 +
    //					CameraSettingConfig()->byRemainingMinute;
    					
   	//vRecrdTimeInSec = 	CameraSettingConfig()->byRecordedHour * 3600 + //Andy Liu TBD
    //					CameraSettingConfig()->byRecordedMinute * 60 +
    //					CameraSettingConfig()->byRecordedSecond;
#else
    vRemainingInSec =   netapp_get_RecordTimeRemaining();
    vRecrdTimeInSec =   (unsigned int)VideoFunc_GetRecordTimeOffset();
#endif

   	vAvailableInSec = 	vRemainingInSec - vRecrdTimeInSec;

	return  vAvailableInSec;
}
int netapp_get_CaptureRemaining()
{
#if 0
	//it will be updated after each capture done or swtiching to capture mode
	return 0;
	//return CameraSettingConfig()->uiRemainingShots; //Andy Liu TBD
#else
    unsigned long long  freeBytes;
    unsigned int        remainCaptureNum;
    
    CaptureFunc_CheckMemSizeAvailable(&freeBytes, &remainCaptureNum);
    
    return remainCaptureNum; 
#endif
}
int netapp_get_PowerLevel()
{
	int vBatPercen;
	
	switch(MenuSettingConfig()->uiBatteryVoltage)
	{
		case BATTERY_VOLTAGE_EMPTY:
		case BATTERY_VOLTAGE_LOW:
			vBatPercen = 0;
		break;
		case BATTERY_VOLTAGE_LEVEL_0:
			vBatPercen = 25;
		break;
		case BATTERY_VOLTAGE_LEVEL_1:
			vBatPercen = 50;
		break;
		case BATTERY_VOLTAGE_LEVEL_2:
			vBatPercen = 75;
		break;
		case BATTERY_VOLTAGE_LEVEL_3:
		case BATTERY_VOLTAGE_FULL:
			vBatPercen = 100;
		break;
	}
	return vBatPercen;
}

static int g_ldws_val;
char* netapp_get_LdwsResult()
{
	if (g_ldws_val == LDWS_STATE_DEPARTURE_LEFT) {
		return "LEFT";
	}
	else if (g_ldws_val == LDWS_STATE_DEPARTURE_RIGHT) {
		return "RIGHT";
	}
	return "NORMAL"; //vBatPercen;
}

void netapp_set_LdwsResult(int val)
{
	static int cnt = 0;
	if (((val==LDWS_STATE_DEPARTURE_LEFT)||(val==LDWS_STATE_DEPARTURE_RIGHT))&&
		(g_ldws_val == val)) {
		cnt++;
	}else
		cnt = 0;
	
	if (((val==LDWS_STATE_DEPARTURE_LEFT)||(val==LDWS_STATE_DEPARTURE_RIGHT))/*||
		(g_ldws_val != val)*/) {
		g_ldws_val = val;
		if ((cnt ==0)||((cnt%5)==0))//5 continue times of same value send one UINotify
			netapp_UINotifyChanged(NULL);
	}else {
		g_ldws_val = val;
	}
}

/*
 * parameter:
 *     video_format supports FORMAT_MJPEG
 *	   audio_format supports FORMAT_PCM, FORMAT_AAC
 */
int netapp_parsefile(char *file_name, int video_format, int audio_format)
{
	int err;
    AHC_MODE_ID ahcMode = AHC_GetAhcSysMode();
    MMP_BYTE                    bFilePath[200];
    MMP_ULONG ulFileNameLen;
    NETAPP_VALUE value;
    unsigned short i;


    if (file_name != NULL) {
    	printc(FG_RED("Use file %s\r\n"), file_name);
    } else {
    	printc(FG_YELLOW("Use live streaming\r\n"));
    	return -1;
    }

	if (AHC_MODE_RECORD_PREVIEW == ahcMode) {
		//MMPS_VIDPLAY_SetDramMallocAddr(MMPS_3GPRECD_GetPreviewEndAddr());
	}

    #if (FS_INPUT_ENCODE == UCS2)
    uniStrcpy(bFilePath, vidFileName);
    ulFileNameLen = uniStrlen((short *)bFileName);
    #elif (FS_INPUT_ENCODE == UTF8)
    STRCPY(bFilePath, "SD:0:\\"/*FS_1ST_DEV*/);
    STRCAT(bFilePath, file_name);
    ulFileNameLen = STRLEN(bFilePath);
    #endif
    for (i = 0; i < ulFileNameLen; ++i) {
		if (bFilePath[i] == '/') {
			bFilePath[i] = '\\';
		}
	}

	err = AIHC_OpenTranscode();
	if (err != 0) {
		return -4;
	}
	
    err = MMPS_VIDPLAY_Open(bFilePath, ulFileNameLen, 0, MMP_TRUE, MMP_TRUE, NULL);

    if (err != MMP_ERR_NONE) {

        printc("File Name : %s\r\n"
               "Unable to open the file (%X).\r\n", bFilePath, err);

        if(err != MMP_3GPPLAY_ERR_INCORRECT_STATE_OPERATION){
            printc("Close Video\r\n");
            AIHC_CloseTranscode();
        }
        return -2;
    }

    value.type = VALUE_TYPE_INT;
    MMPS_VIDPLAY_GetCurFileDuration((MMP_ULONG*)&value.v.i32);
    if (!netapp_set_DeviceSetting(NADS_FILE_DURATION, &value)) {
    	return -3;
    }
	return 0;
}

// To protect between GET-VIDEO-TUMBNAIL and CAPTURE/BURST/TIMELAPSE
// to avoid system crashing.
// return 0 - successful, go ahead
//        1 - failed, encode JPEG is busy
#ifdef LOCK_DEBUG
int netapp_CaptureLockDBG(int enLock, unsigned int timeout, char *f, int l)
#else
int netapp_CaptureLock(int enLock, unsigned int timeout)
#endif
{
	static MMPF_OS_SEMID	_capsemi = -1;
	unsigned char			ret;
	unsigned short			cnt;
	
#ifdef LOCK_DEBUG
	printc(FG_RED("%s %d (%d): %s %d\n"), __func__, enLock, timeout, f, l);
#endif
	if (_capsemi == -1) {
		osal_mutex_init(&_capsemi);
		if (_capsemi >= OSAL_MUTEX_CODE) {
			// ERROR! System resource is not enough!!
			_capsemi = -1;
			printc(BG_RED("%s %d: Create semi Error!\n"), __func__, __LINE__);
			return 0;	// go ahead!!
		}
	}
	if (osal_mutex_queryEx(&_capsemi, &cnt) >= OSAL_MUTEX_CODE) {
		// ERROR!, for debug
		printc(BG_RED("%s %d: QUERY semi Error!\n"), __func__, __LINE__);
		return 0;	// go ahead!!
	}
	if (enLock) {
		if (cnt == 0 && timeout == 0)
			return 1;
		if (cnt > 1) {
			// ERROR!, for debug
			printc(BG_RED("%s %d: LOCK semi Error!\n"), __func__, __LINE__);
			return 0;	// go ahead!!
		}
		ret = osal_mutex_lockwait(&_capsemi, timeout);
		if (ret != 0)
			printc(BG_RED("Lock return %d\n"), ret);
	} else {
		if (cnt != 0) {
			// ERROR!, for debug
			printc(BG_RED("%s %d: UNLOCK semi Error!\n"), __func__, __LINE__);
			return 0;	// go ahead!!
		}
		ret = osal_mutex_unlock(&_capsemi);
	}
	if (ret == OS_NO_ERR) {
		return 0;
	}
	return 1;
}
// 0: liveRTSP, 1: txRTSP
// TODO: txRTSP should run under PLAYBACK mode only
int netapp_query_Streamable(int streamtype)
{
	int		state;
	
	state = (int)uiGetCurrentState();
#ifdef __CONFIG_SDK_LETV_CDV01__	
	return 1;
#endif
	if (streamtype == 0/*liveRTSP*/){
        // For CarDV TVOUT / HDMI state
        if (state == UI_TVOUT_STATE) {
            if (TVFunc_Status() == AHC_TV_VIDEO_PREVIEW_STATUS ||
                TVFunc_Status() == AHC_TV_DSC_PREVIEW_STATUS)
                return 1;
            else
                return 0;
        }
        else if (state == UI_HDMI_STATE) {
            if (HDMIFunc_GetStatus() == AHC_HDMI_VIDEO_PREVIEW_STATUS ||
            	HDMIFunc_GetStatus() == AHC_HDMI_DSC_PREVIEW_STATUS)
                return 1;
            else
                return 0;
        }   
        else
        {
            return (state == UI_VIDEO_STATE  ||
                    state == UI_CAMERA_BURST_STATE ||
                    state == UI_TIME_LAPSE_STATE)
                    ?  1 : 0;
        }
	}
	else{	/*txRTSP work under PLAYBACK or SETTING UI state */
		return (state == UI_NET_PLAYBACK_STATE ||
				state == UI_BROWSER_STATE      ||
				state == UI_CAMERA_SETTINGS_STATE)? 1 : 0;
	}
}
// 1: can flash FW
// 0: can NOT flash FW
int netapp_query_Flashable()
{
	int		state;
	
	state = (int)uiGetCurrentState();
    printc(">>>>> %s,%d Wifi TBD. <<<<<\r\n",__func__,__LINE__);
    return 0;
	//return (state == UI_NET_FWUPDATE_STATE)? 1 : 0;
}
//
int netapp_update_AudioSamplingRate(int transcode)
{
	NETAPP_VALUE value = {VALUE_TYPE_INT, NADS_FILE_DURATION_UNDEFINED};
	const char *str = NULL;

	if (transcode) {
		str = "transcoding bit rate";
		MMPD_VIDPSR_GetParameter(MMPD_VIDPLAY_PARAMETER_AUDIO_SAMPLING_RATE, &value.v.i32);
	} else {
		str = "live streaming";
		value.v.i32 = AHC_GetDefaultAudioSamplingRate();
	}
	printc("%s %d\r\n", str, value.v.i32);

	if (!netapp_set_DeviceSetting(NADS_AUDIO_SAMPLING_RATE, &value)) {
		osal_dprint(ERR, "set %s", value.v.str);
		return -1;
	}
	return 0;
}

unsigned int netapp_get_streaming_timer(void)
{
	return AHC_StreamingTimer_GetMs();
}

void netapp_getWiFiMacAddr(MMP_UBYTE* addr)
{
	WLAN_DRV_HDL hdl;
	
	if(WLAN_SYS_IsStart())
	{
		WLAN_SYS_GetMacAddr(addr);
	}
	else
	{
		WLAN_SYS_InitHw();
		
		if(!mem_is_ready())
			mem_init();
			
		hdl = WLAN_SYS_Start();
		
		if(hdl){
			WLAN_SYS_GetMacAddr(addr);
			WLAN_SYS_Stop(hdl);
		}
	}
}


/** Stop transcoding streaming. Typically RTSP
 *
 * @return 0 if successful. < 1 if failed.
 */
int netapp_stopTranscoding(void)
{
	struct amn_session_t *s;
	struct ammo_node* node = NULL;
	char src_name[] = "VIDEO.0/EncMJPEG"; //double-check this with conf file.

	printc("%s %d\r\n", __func__, __LINE__);

	//static void __aitstreamer_set_running( char *src_name, int running )
	node = node_find_name( AMMO_TYPE__NODE_SOURCE, src_name, -1 );

	if (node) {
		struct list_head *l;
		struct ammo_pipe *p;
		/* see if anybody else is listening */
		list_for_each( l, &(node->pl->pipe_TO_list) ) {
			p = list_entry( l, struct ammo_pipe, link );
			printc("name:%s type:%d\r\n", p->to_node->name, p->to_node->node_type);
			if (p->to_node->node_type == AMMO_TYPE__NODE_SINK) {
				int i;
				for (i=0; ;i++) {
					s = session_search_owner(p->to_node, i);
					if (NULL != s) {
						printc("found session of %s\r\n", p->to_node->name);
						session_destroy(s, "stop transcoding");
					} else {
						break;
					}
				}
			}
		}
	}
	else {
		printc("set_running for Source Node '%s' : invalid \r\n", src_name);
		return -1;
	}
	return 0;
}

//
int netapp_flashFW(char *filename, PFN_WEBWAIT pfn_w, PFN_CGI_OP_CALLBACK pfn_s)
{
	int		ret;

	printc("Flash FW in %s\n", (filename == NULL)? "CALL DEFAULT" : filename);
	netapp_CGIOpCallback(pfn_s);
	// ret = Report2UIAndWaitFeedBack(WIRELESS_SET_UIMODE, id, pfn_w, (void*)6000);
	
	printc(">>>>> %s,%d Wifi TBD. <<<<<\r\n",__func__,__LINE__);
	//AHC_SendAHLMessage_HP(AHLM_UI_NOTIFICATION, EVENT_NET_FWUPDATE_FROM_SD, 6000);
	
    if (pfn_w) {
    	// Wait
        printc(">>>>> %s,%d Wifi TBD. <<<<<\r\n",__func__,__LINE__);
    	//ret = pfn_w((void*)EVENT_NET_FWUPDATE_FROM_SD, (void*)6000);
    	
		printc("--> COMMAND FINISH %d\n", ret);
    	if (ret >= 0)
    		return NETAPP_DONE;
    	else
    		return NETAPP_FAIL;
    }
    return NETAPP_DONE;
}
//
int netapp_uploadprogress(unsigned int cursize, unsigned int total)
{
	// TODO: Call to UI to show progress
	static char prm[] = "\\|/-";
	static int  idx = 0;
    static int uiPercentage = -1;
	printc("%c %u/%u\r", prm[(idx++) & 0x03], cursize, total);

    if( AHC_DrawNetFwDownloadProgress != NULL )
    {
        if( uiPercentage != ((cursize*100)/total) )
        {
            uiPercentage = (cursize*100)/total;
            AHC_DrawNetFwDownloadProgress( uiPercentage );
        }
    }
	return 0;
}
//  close transcodig only without handling network sessions. This is for net code only.
int netapp_closeTranscoding(void)
{
	return AIHC_CloseTranscode();
}
// get audio sample rate of source
// return should be 8000, 16000, 24000, 32000, 48000
int netapp_getAudioSampleRate(void)
{
	return AHC_GetAudioSamplingRate(VR_AUDIO_TYPE_AAC);
}

int netapp_getCameraSrc(unsigned int vMode)
{
	struct amn_cfgspace_value_t *cfg_val;

	if ((AHC_STREAMING_MODE)vMode == AHC_STREAM_MJPEG) {

		cfg_val = amn_config_get(CONFIG_SPACE_TYPE_CURRENT, "Camera.Preview.Source.1.Camid");

		if (strcmp(cfg_val->v.strVal,"front")&&strcmp(cfg_val->v.strVal,"rear")) {
			amn_config_set(CONFIG_SPACE_TYPE_CURRENT,"Camera.Preview.Source.1.Camid", "front");
			return AHC_STREAM_FRONT;
		}	
		else if (!strcmp(cfg_val->v.strVal,"front")) {
			return AHC_STREAM_FRONT;
		}
		else {
			return AHC_STREAM_REAR;
	    }
	}
	return AHC_STREAM_FRONT;
}

int netapp_switchCamera()
{
	AHC_STREAMING_MODE sMode,vMode,aMode;
	
	sMode=AHC_GetStreamingMode();
	vMode = sMode&AHC_STREAM_V_MASK;
	aMode = sMode&AHC_STREAM_A_MASK;
	if (vMode == AHC_STREAM_OFF)
		return 0;
	AHC_SetStreamingMode(aMode);
	AHC_SetStreamingMode(vMode|aMode);
	return 0;
}

static int rcamstate=0;
int netapp_getRearcamReady()
{
	return rcamstate;
}

void netapp_setRearcamReady(int state)
{
	rcamstate = state;
}

//broascast ip to AP 
#if 0
////////receive hotspot udp broadcast
#define BROADCAST_RECV_PORT 49132
#define BUFSIZE 2500
#define DISCOVER_STR "DISCOVER.CAMERA.IP"
static int fd_recv = 0;
void netapp_hotspotbroadcast_task(void *fd) {
	fd_set rdSet;
	int ret = 0;
	int stopSock = 0;
	int byte_count;
	socklen_t fromlen;
	struct sockaddr addr;
	char buf[512];

	//struct sockaddr *from, socklen_t *fromlen

	int recv_fd;
	recv_fd = *((int*)fd);

	while (1) {
		FD_ZERO(&rdSet);
		//FD_ZERO(&expSet);
		/* FD_SET()'s */
		FD_SET(recv_fd, &rdSet);
		memset(buf,0,sizeof(buf));
		ret = net_select(recv_fd+1, &rdSet, NULL, NULL, NULL);

		fromlen = sizeof addr;
		byte_count = net_recvfrom(recv_fd, buf, sizeof(buf), 0, &addr, &fromlen);
		if (byte_count > 0) {
			//printc("UDP receive data =%s\n",buf);
			if (!strncasecmp(buf,DISCOVER_STR,byte_count)) {
				netapp_UINotifyChanged(NULL);
			}
		}	
	}
	net_close(recv_fd);
}

int netapp_stabroadcast_receive_start()
{
	struct sockaddr_in myaddr;      
//    int fd;
	int task;
    if ((fd_recv = net_socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    	printc("cannot create socket\n");
        return 0;
    }
	
    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(BROADCAST_RECV_PORT);

    if (net_bind(fd_recv, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
        printc("bind failed");
        return 0;
    }
	task = osal_thread_new(netapp_hotspotbroadcast_task, (void*)&fd_recv);
	if (task <= 0)
		return 0;
	return task;
    /* never exits */

}

void netapp_hotspotbroadcast_routine_stop()
{
	int ret;
	if (fd_recv > 0) {
		ret = net_close(fd_recv);
		//printc("####net_close = %d\n",ret);
		fd_recv = 0;
	}
	
}
#endif

// CarDV +++
#ifdef REARVIEW_STREAMING_TEST
/* add for rear MJPEG test start*/
char* buf_data;
char* buf_tdata;
char* netapp_test_malloc()
{
	
	buf_data=osal_malloc(256*1024);
	buf_tdata=osal_malloc(256*1024);
	//ait_printf("DDDDATA ADDR=%p \n",buf_data);
	
	return buf_data;
}


void netapp_test_mfree()
{
	osal_free(buf_data);
	osal_free(buf_tdata);
}

OSAL_FILE *hdl=NULL;
static unsigned int g_offset;
static unsigned int f_cnt;
static unsigned int t_cnt;
void netapp_avi_fileopen(char* filename)
{
	unsigned int frames;
	g_offset = 0;
	f_cnt =0;
	frames = 0;
	if (hdl == NULL) {
		hdl = osal_fopen(filename, "rb");
		osal_fseek(hdl,0,SEEK_SET);
		osal_fread(buf_tdata,sizeof(char),512,hdl);
		frames = buf_tdata[51]*256*256*256+buf_tdata[50]*256*256+buf_tdata[49]*256+buf_tdata[48];
		printc("######### Total frames=%d \n",frames);
		t_cnt = frames;

	}
	if (hdl != NULL)
		ait_printf("FILE %s OPEN OK \n",filename);
	else
		ait_printf(" FILE %s OPEN FAILED \n",filename);
}

unsigned int netapp_avi_filegetframe(MMP_ULONG baddr)
{
#if 1
	unsigned int rd,offset_start,offset_end,f_size,i;
	if ((hdl == NULL)||(buf_tdata==NULL))
		return 0;
	if (f_cnt >= t_cnt) 
	{
		f_cnt=g_offset = 0;
		//return 0;
	}
	osal_fseek(hdl,g_offset/*256*/,SEEK_SET);
	osal_invalidate_dcache(((unsigned int)(buf_tdata) & ~(MEM_ALIGNMENT_HEAP - 1)) , 64*1024);
	rd = osal_fread(buf_tdata,sizeof(char),64*1024,hdl);
	for (i=0;i<rd;i++)
	{
		if ((buf_tdata[i]==0xff)&&(buf_tdata[i+1]==0xd8))
			break;
	}
	offset_start = i;
	
	for (i=offset_start;i<rd;i++)
	{
		if ((buf_tdata[i]==0xff)&&(buf_tdata[i+1]==0xd9))
			break;
	}
	offset_end = i+1;
	
	g_offset += (offset_end);
	
	f_size = offset_end-offset_start+1;
	memcpy((void*)baddr/*buf_data*/,buf_tdata+offset_start,f_size);
	f_cnt ++;
	g_offset = (g_offset/512)*512;
//	ait_printf("\n f_size = %d addr=%p\n",f_size,baddr);
	return f_size;
#endif	
	
}

void netapp_avi_fileclose(OSAL_FILE *hdl)
{
	osal_fclose(hdl);
	hdl = NULL;
}

int netapp_avi_stopstream(void)
{
	struct amn_session_t *s;
	struct ammo_node* node = NULL;
	char src_name[] = "VIDEO.1/EncRMJPEG"; //double-check this with conf file.

	printc("%s %d\r\n", __func__, __LINE__);

	//static void __aitstreamer_set_running( char *src_name, int running )
	node = node_find_name( AMMO_TYPE__NODE_SOURCE, src_name, -1 );

	if (node) {
		struct list_head *l;
		struct ammo_pipe *p;
		/* see if anybody else is listening */
		list_for_each( l, &(node->pl->pipe_TO_list) ) {
			p = list_entry( l, struct ammo_pipe, link );
			printc("name:%s type:%d\r\n", p->to_node->name, p->to_node->node_type);
			if (p->to_node->node_type == AMMO_TYPE__NODE_SINK) {
				int i;
				for (i=0; ;i++) {
					s = session_search_owner(p->to_node, i);
					if (NULL != s) {
						printc("found session of %s\r\n", p->to_node->name);
						session_destroy(s, "stop transcoding");
					} else {
						break;
					}
				}
			}
		}
	}
	else {
		printc("set_running for Source Node '%s' : invalid \r\n", src_name);
		return -1;
	}
	return 0;
}
/* add for rear MJPEG test end*/
#endif

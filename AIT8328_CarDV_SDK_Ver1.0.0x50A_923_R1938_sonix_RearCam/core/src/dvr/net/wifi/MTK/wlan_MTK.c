//==============================================================================
//
//  Description :
//  Author      :
//  Revision    :
//
//==============================================================================
#include "config_fw.h" //for chip define and possibly PIN config
#include "AHC_FS.h"
#include "AHC_General.h" //for GPIO
#include "AHC_Message.h"


/* lwIP and AMAPAK*/
//#include "gb9662if.h"
#include "gb9619.h"
//#include "lwip/ip_addr.h"
#include "wlan_MTK.h"
#include "mem.h"
#include "netapp.h"
#include "amn_config.h"
#include <string.h>
#ifndef LINUX
#include "lib_retina.h"//for printc
#endif
#include "netif.h"



/*----------------------------------------------------------------------------*/
/*                      VARIABLES                                             */
/*----------------------------------------------------------------------------*/
extern struct netif main_netdev;

static AHC_BOOL m_bWPSStart       = AHC_FALSE;
static AHC_BOOL m_bWiFiStatus     = AHC_FALSE;
static UINT32 m_uiScanFileHandler = NULL;
static void* m_pScanBufPtr = NULL;
static WLAN_TARGET_AP_INFO m_sTargetAPInfo;

static UINT32 m_uiTxBufCnt = NCFG_TXBUF_COUNT;
static UINT32 m_uiRxBufCnt = NCFG_RXBUF_COUNT;
static UINT32 m_uiMaxScanBufSize = 0;
int gbWifiConnect = 0; 


void WLAN_SYS_StatusCallback(WLAN_EVET_MSQ sEventMsg)
{
    printc("WLAN CB : %d : ", sEventMsg);

    switch(sEventMsg){

	case WLAN_E_SET_SSID://0
		// Station Mode
	    printc("CONNECTED \n");
	    gbWifiConnect++;
	    if (main_netdev.flags & NETIF_FLAG_LINK_UP)
	    	Netapp_set_status(NETAPP_WIFI_READY);
	    break;
	    
	case WLAN_E_DISASSOC_IND://12
    	if (gbWifiConnect > 0) {
    		gbWifiConnect--;
    	}
    	AHC_SendAHLMessage( AHLM_WIRELESS_CONNECT, WIRELESS_CONNECT_FAIL, 0 );
    	printc("DISCONNECTED %d\n", gbWifiConnect);
    	if (gbWifiConnect == 0) {
	    	Netapp_set_status(NETAPP_WIFI_IDLE);
#ifdef	ALWAYS_STREAM_ON
			aitstreamer_run("none");
#endif
		}
    	break;

    case WLAN_E_ASSOC_IND://8
    	// AP Mode
	    gbWifiConnect++;
	    AHC_SendAHLMessage( AHLM_WIRELESS_CONNECT, WIRELESS_CONNECT_SUCCESS, 0 );
	    printc(BG_GREEN("Client Connect SoftAP %d\n"), gbWifiConnect);

		// TODO: 
	    if (gbWifiConnect == 1 &&
	    	Netapp_get_status() != NETAPP_STREAM_PLAY) {
	    	Netapp_set_status(NETAPP_WIFI_READY);
#ifdef	ALWAYS_STREAM_ON
			aitstreamer_run("MJPEG.0");
#endif
	    }
	    break;
	    
    case WLAN_E_SCAN_COMPLETE://26
	    printc("Scan Done \n");	    
	    break;
	
	case WLAN_E_TXFAIL:
		// TODO:
		printc("!!! TX FAILED !!!\r\n");
	    Netapp_set_status(NETAPP_STREAM_FAIL);
		break;
    default:
    	printc("NO Mapping");
	    break;
	}
	
	printc("\n");

}

int8* WLAN_SYS_GetVersion(void)
{
    return (int8*)wlan_get_version();
}

static void WLAN_SYS_WaitForLoading9619(void)
{

}

/****************************************************************************
* Function:   	wlan_start
*
* Purpose:   	This routine is used to initialize GB9619 module.
*
* Parameters: callback function to receive driver events.
*			 tx_buf_cnt		count of buf in tx queue, default is 32 if 0 is set
*			 rx_buf_cnt		count of buf in rx queue, default is 16 if 0 is set
*			scan_buf_size		scan buf size, default is 32KB if 0 is set
*
* Returns:	Returns driver handle, otherwise a NULL pointer.
*****************************************************************************
*/
WLAN_DRV_HDL WLAN_SYS_Start(void)
{
    wl_drv_hdl h;

    h = wlan_start(WLAN_SYS_StatusCallback,0,0,0);

    m_bWiFiStatus = AHC_TRUE;
    return h;
}

void WLAN_SYS_Stop(WLAN_DRV_HDL hdl)
{
    wlan_stop(hdl);
    m_bWiFiStatus = AHC_FALSE;
    gbWifiConnect = 0;
}

bool WLAN_SYS_IsStart(void)
{
	return m_bWiFiStatus;
}

/****************************************************************************
* Function:   	wlan_get_mode
*
* Purpose:   	This routine is used to current mode of Wi-Fi
*
* Parameters: None
*
* Returns:	Returns 0 if in STA mode, 1 in SW AP mode and 2 when WPS is in progress.
*****************************************************************************
*/
INT32 WLAN_SYS_GetMode(void)
{
    if(m_bWiFiStatus == AHC_TRUE)
        return wlan_get_mode();
    else
        return -1;
}

/****************************************************************************
* Function:   	wlan_get_mac_addr
*
* Purpose:   	This routine is used to get wifi MAC addressi
*
* Parameters: buffer to store MAC address
*
* Returns:	None.
*****************************************************************************
*/
void WLAN_SYS_GetMacAddr(uint8* addr)
{
    if(m_bWiFiStatus == AHC_FALSE)
        return;
    wlan_get_mac_addr(addr);
}

void WLAN_SYS_GetMacAddrStr(int8* addrStr)
{
    
    uint8 byMacAddr[6];
    uint32 i;
    uint8 byTempChar;
    
    if(m_bWiFiStatus == AHC_FALSE)
        return;
    
    wlan_get_mac_addr(byMacAddr);
    
    for(i = 0; i < 6; i++){
        
        byTempChar = (byMacAddr[i] & 0xF0) >> 4;
        
        if(byTempChar < 10){
        
            byTempChar += '0';
        }else{
            byTempChar += ('A' - 10);
        }
        
        *addrStr = byTempChar;
        addrStr++;
        
        byTempChar = (byMacAddr[i] & 0xF);
        
        if(byTempChar < 10){
        
            byTempChar += '0';
        }else{
            byTempChar += ('A' - 10);
        }
        
        *addrStr = byTempChar;
        addrStr++;
    
    }
}

void WLAN_SYS_TxPkt(struct pbuf *p)
{
    if(m_bWiFiStatus == AHC_FALSE)
        return;
    wlan_tx_pkt(p);
}

void wlan_sys_rx_pkt(struct pbuf *p)
{
	host_network_process_ethernet_data(p, &main_netdev);
}

INT32 WLAN_SYS_GetTXQueueAvailCount(void)
{
    if(m_bWiFiStatus == AHC_FALSE)
        return -1;
    else
        return kalGetTxAvaliableFrameCount();//return 5;
}

void WLAN_SYS_GetStatsInfo(WLAN_STATISTICS_INFO *stats, char *sdio_stat_buf, int sdio_stat_size)
{

}

static UINT32 WLAN_SYS_Init9619(void)
{

    return 0;
}

static UINT32 WLAN_SYS_Reset9619(void)
{

    return 0;
}


void WLAN_SYS_InitHw(void)
{
}

void WLAN_SYS_ResetHw(void)
{
}



INT32 WLAN_STA_AllocateScanBuf(void)
{
    //void* pScanMemPtr;
    
    if(m_pScanBufPtr != NULL)
        return 0;
    
    m_pScanBufPtr = mem_malloc(WLAN_SCAN_BUF_SIZE);

    if(m_pScanBufPtr == NULL){
        printc("alloc scan buffer fail\n");
        return -1;
    }
    
    return 0;
}

void WLAN_STA_FreeScanBuf(void)
{

	mem_free(m_pScanBufPtr);
}

int8* WLAN_STA_ScanBufAddr(void)
{
    return m_pScanBufPtr;
}

int8* WLAN_STA_ScanAPbyModule(uint32* uiCount)
{
	WLAN_SCAN_RESULT *list;
	
    if(m_pScanBufPtr == NULL)
        return NULL;
        
    if(m_bWiFiStatus == AHC_FALSE)
        return NULL;
        
    //wlan_scan_network(m_pScanBufPtr, WLAN_SCAN_BUF_SIZE);
    
    list = (WLAN_SCAN_RESULT*)m_pScanBufPtr;
    *uiCount = list->count;
    
    return (int8*)m_pScanBufPtr;
}
/*
start from 0.
*/
int8* WLAN_STA_GetScanResultByIndex(uint32 uiIndex, int8* pchBuf)
{
	WLAN_SCAN_RESULT *list;
	WLAN_BSS_INFO *bi;
	
    if(m_bWiFiStatus == AHC_FALSE)
        return NULL;
    return NULL;
}


/**
scan_buf must be 32 bit alignment.
*/
int32 WLAN_STA_ScanAP(int8* scan_buf, uint32 buf_size)
{
    if(m_bWiFiStatus == AHC_FALSE)
        return -1;
	 return -1;
}

int32 WLAN_STA_PutScanInfo(int8* scan_buf, uint32 buf_size)
{
    uint32 uiWriteCount;
    
    
    return 0;
}

INT32 WLAN_STA_OpenScanInfo(UINT32* puiFileHandler)
{
    
    return 0;
}

void WLAN_STA_CloseScanInfo(void)
{
        
    m_uiScanFileHandler = NULL;
}

int32 WLAN_STA_ScanAPbySSID(int8* pchSSID, WLAN_BSS_INFO *psInfo)
{
   
	return -1;
}


void WLAN_STA_ScanChannel(UINT32* puiChannelList)
{
   return;
}

void WLAN_STA_CalculateChannel(UINT8* pubChannelList)
{
    return;
}

void WLAN_STA_AutoSelectChannel(UINT32* puiChannel)
{
    return;
}

void WLAN_STA_AutoSelectChannel_PanaRule(uint32* puiChannel)
{
	return;
}

int32 WLAN_STA_SetTargetAP(int8 *ssid, WLAN_AUTH_MODE authMode, WLAN_ENCRYPTION encryption, const int8 *key)
{
   
    return 0;
}

INT32 WLAN_STA_ConnectTargetAP(void)
{
 
	return -1;
}
/****************************************************************************
* Function:   	wlan_join_network
*
* Purpose:   	This routine is used to ask driver to join a netwrok.
*
* Parameters: ssid			SSID of the AP used to join a network
*			 authMode	authentication mode used to join a network
*			 encryption	encryption mode used to join a network
*			 key			passphrase used to join a network
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
*****************************************************************************
*/
int32 WLAN_STA_JoinAP(int8 *ssid, WLAN_AUTH_MODE authMode, WLAN_ENCRYPTION encryption, const int8 *key )
{
	return -1;
}

/****************************************************************************
* Function:   	wlan_get_connection_info
*
* Purpose:   	This routine is used to get the current connection information at STA mode
*
* Parameters: wlan connection information
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
*****************************************************************************
*/
/*
support multi-task.
*/
INT32 WLAN_STA_GetConnectedINFO(WLAN_CONNECTION_INFO* info)
{
	return -1;
}

/****************************************************************************
* Function:   	wlan_create_sw_ap
*
* Purpose:   	This routine is used to create a SW AP network
*
* Parameters: ssid			SSID of the SW AP to be created
*			 authMode	Authentication mode used for the SW AP
*			 encryption	Encryption mode used for the SW AP
*			 key			Passphrase used for the SW AP
*			 channel		Channle used for the SW AP
*			 ssidBcst		0: to broadcast SSID, 1: to hide SSID
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
*****************************************************************************
*/
int32 WLAN_AP_Create(int8* ssid, 
                     WLAN_AUTH_MODE authMode, 
                     WLAN_ENCRYPTION encryption, 
                     const int8 *key, 
                     WLAN_CHANNEL channel,
                     bool bHideSSID)
{
    if(m_bWiFiStatus == AHC_FALSE)
        return -1;

    if(bHideSSID == AHC_TRUE){
        printc("Hide SSID \n");
    	return wlan_create_sw_ap((char*)ssid, authMode, encryption, (char*)key, channel, 1);
	}else{
        printc("broadcast SSID \n");
	    return wlan_create_sw_ap((char*)ssid, authMode, encryption, (char*)key, channel, 0);
	}        
}

/****************************************************************************
* Function:   	wlan_get_assoc_list
*
* Purpose:   	This routine is used to the associated client list in SW AP mode
*
* Parameters: buf			The buffer to store the associated client list
*			 buf_size		size of the buffer
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
*****************************************************************************
*/
int32 WLAN_AP_GetAssocList(int8* pchBuf, uint32 uiBufSize)
{
    if(m_bWiFiStatus == AHC_FALSE)
        return -1;
    return wlan_get_assoc_list((char*)pchBuf, uiBufSize);    
}

void WLAN_WPS_CallBack(WLAN_WPS_STATE event)
{
    printc("WPS CB : %d : ", event);

#if WIFI_SEAL == 0
#ifndef CAR_DV
    DrawSubItem_WifiWPSState(event);
#endif
#endif

    switch(event){
    
    case WLAN_WPS_STATE_OFF:
        printc("OFF");
    break;
    case WLAN_WPS_STATE_INIT:
        printc("INIT");
    break;
	
	case WLAN_WPS_STATE_START:
        printc("START");
    break;
	
	case WLAN_WPS_STATE_CONN_FAIL:
        printc("CONN FAIL");
    break;
    
	case WLAN_WPS_STATE_FAIL:
        printc("FAIL");
    break;
    
	case WLAN_WPS_STATE_LINK_UP:
        printc("LINK UP");
    break;
	
	case WLAN_WPS_STATE_WAIT_START:
        printc("WAIT START");
    break;
	
	case WLAN_WPS_STATE_WAIT_M2:
        printc("WAIT M2");
    break;
	
	case WLAN_WPS_STATE_RX_M2D:
        printc("RX M2D");
    break;
    
	case WLAN_WPS_STATE_WAIT_M4:

        printc("WAIT M4");
    break;
	
	case WLAN_WPS_STATE_WAIT_M6:

        printc("WAIT M6");
    break;
	
	case WLAN_WPS_STATE_WAIT_M8:
        printc("WAIT M8");
    break;
	
	case WLAN_WPS_STATE_WAIT_EAPFAIL:

        printc("WAIT EAP FAIL");
    break;
	
	case WLAN_WPS_STATE_WAIT_DISCONN:

        printc("WAIT DISCONN");
    break;
	
	case WLAN_WPS_STATE_CONFIGURED:
        printc("CONFIGURED");
    break;
    
    }
    
    printc(": status : %d \n", WLAN_WPS_GetStatus());

}

/****************************************************************************
* Function:   	wlan_wps_start
*
* Purpose:   	This routine is used to perform WPS
*
* Parameters: wps configuration
*			cb	wps event callback
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
*****************************************************************************
*/

INT32 WLAN_WPS_Start(WLAN_WPS_CONFIG* wpsConfigInfo, void* pFunc)
{
	return -1;
}
/****************************************************************************
* Function:   	wlan_stop_wps
*
* Purpose:   	This routine is used to stop WPS
*
* Parameters: None
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
*****************************************************************************
*/
void WLAN_WPS_Stop(void)
{
    if(m_bWiFiStatus == AHC_FALSE)
        return;
        
}

/****************************************************************************
* Function:   	wlan_wps_gen_pin
*
* Purpose:   	This routine is used to generate WPS PIN
*
* Parameters: pin	generated PIN
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
*****************************************************************************
*/
INT32 WLAN_WPS_GenPin(UINT32* uiPin)
{
    if(m_bWiFiStatus == AHC_FALSE)
        return -1;
    return -1;    
}

bool WLAN_WPS_Enable(void)
{
    return m_bWPSStart;

}

/****************************************************************************
* Function:   	wlan_wps_get_state
*
* Purpose:   	This routine is used to get state of WPS process
*
* Parameters: None
*
* Returns:	State of WPS process
*****************************************************************************
*/

WLAN_WPS_STATE WLAN_WPS_GetState(void)
{
    WPS_STATE state;
    
    if(m_bWiFiStatus == AHC_FALSE)
        return WLAN_WPS_STATE_OFF;
    
    return state;
}

/****************************************************************************
* Function:   	wlan_wps_get_status
*
* Purpose:   	This routine is used to get status of WPS process
*
* Parameters: None
*
* Returns:	Status of WPS process
*****************************************************************************
*/

UINT32 WLAN_WPS_GetStatus(void)
{
    if(m_bWiFiStatus == AHC_FALSE)
        return -1;
    return -1;
}

/****************************************************************************
* Function:   	wlan_wps_get_credentials
*
* Purpose:   	This routine is used to get credentials after WPS succeed
*
* Parameters: Credentials of the AP
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
*****************************************************************************
*/

INT32 WLAN_WPS_GetCredential(WLAN_CONFIG_INFO *wlan_config_info)
{
    if(m_bWiFiStatus == AHC_FALSE)
        return -1;
        
    return wlan_wps_get_credentials((wl_config_info_t*)wlan_config_info);
}

#if 0
void __MFG__(){}
#endif
/*
1. 11b TX test
	wlan_mfg_tx_bg(ch, WLM_RATE_11M, txpwr)
	- ch: channel number
	- txpwr: -1 means default power or value in mili dBm (15000 for 15dBm)
	
2. 11g TX test
	wlan_mfg_tx_bg(ch, WLM_RATE_54M, txpwr)
	- ch: channel number
	- txpwr: -1 means default power or value in mili dBm (15000 for 15dBm)
	
3. 11n TX test
	wlan_mfg_tx_n(ch, WLM_MCS_RATE_7, txpwr)
	- ch: channel number
	txpwr: -1 means default power or value in mili dBm (15000 for 15dBm)
	
4. RX test
	wlan_mfg_rx(ch)
	- ch: channel number
	
5. Get received packets
	wlan_mfg_rx_getpkt(&count)
	- count: received packet count
	
6. Non-modulation TX test
	wlan_mfg_carrier_tone(ch)
	- ch; channel number

7. Stop MFG test
	wlan_mfg_stop()

*/
/****************************************************************************
* Function:   	wlan_mfg_tx_bg
*
* Purpose:   	This routine is used to perform manufacturing 11b/g continuous TX test
*
* Parameters: channel		Channel used for TX
*			 rate			11b/g rate used for TX
*			 powerValue	Output power in units of milli-dBm, -1 means default power
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
*****************************************************************************
*/
INT32 WLAN_MFG_TxBG(WLAN_CHANNEL channel, WLAN_RATE rate, INT32 powerValue)
{
	return -1;
}

/****************************************************************************
* Function:   	wlan_mfg_tx_n
*
* Purpose:   	This routine is used to perform manufacturing 11n continuous TX test
*
* Parameters: channel		Channel used for TX
*			 rate			11n rate used for TX
*			 powerValue	Output power in units of milli-dBm, -1 means default power
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
*****************************************************************************
*/

INT32 WLAN_MFG_TxN(WLAN_CHANNEL channel, WLAN_MCS_RATE rate, INT32 powerValue)
{
	return -1;
}

/****************************************************************************
* Function:   	wlan_mfg_rx
*
* Purpose:   	This routine is used to perform manufacturing RX test
*
* Parameters: channel		Channel used for RX
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
*****************************************************************************
*/
INT32 WLAN_MFG_RX(WLAN_CHANNEL channel)
{
	return -1;
}

/****************************************************************************
* Function:   	wlan_mfg_rx_getpkt
*
* Purpose:   	This routine is used to get received packet count
*
* Parameters: count		Received packet count
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
*****************************************************************************
*/
INT32 WLAN_MFG_RxGetPkt(UINT32* uiCount)
{
	return -1;
}

/****************************************************************************
* Function:   	wlan_mfg_carrier_tone
*
* Purpose:   	This routine is used to perform manufacturing non-modulation TX test
*
* Parameters: channel		Channel used for test
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
*****************************************************************************
*/
INT32 WLAN_MFG_CarrierTone(WLAN_CHANNEL channel)
{
	return -1;
}

/****************************************************************************
* Function:   	wlan_mfg_stop
*
* Purpose:   	This routine is used to stop manufacturing test
*
* Parameters: None
*
* Returns:	Returns 0 if succeed, otherwise a negative error code.
*****************************************************************************
*/
INT32 WLAN_MFG_Stop(void)
{
	return -1;
}


int AP_StartWPS(wps_config_info_t *pwps_config_info, wlan_wps_event_callback cb)
{
	return -1;
}


const char *
capmode2str(uint16 capability)
{
	capability &= (DOT11_CAP_ESS | DOT11_CAP_IBSS);

	if (capability == DOT11_CAP_ESS)
		return "Managed";
	else if (capability == DOT11_CAP_IBSS)
		return "Ad Hoc";
	else
		return "<unknown>";
}


#define	ETHER_ADDR_LEN		6

char *
wl_ether_etoa(const struct ether_addr_packed *n)
{
	static char etoa_buf[ETHER_ADDR_LEN * 3];
	char *c = etoa_buf;
	int i;

	for (i = 0; i < ETHER_ADDR_LEN; i++) {
		if (i)
			*c++ = ':';
		c += sprintf(c, "%02X", n->mac[i] & 0xff);
	}
	return etoa_buf;
}


int
wlu_main_args(int argc, char **argv)
{
	return -1;
}

int wlmClosedSet(int val)
{
	return -1;
}

int wlan_wps_get_credentials(wl_config_info_t *wlan_config_info)
{
	return -1;
}

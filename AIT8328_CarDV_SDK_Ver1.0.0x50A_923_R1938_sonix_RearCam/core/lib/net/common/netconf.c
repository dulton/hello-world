//==============================================================================
//
//  Description : Network Configuration
//  Author      : Bernard Shyu
//  Revision    : 1.0
//
//==============================================================================
//#include "opt.h"

//#include "lwip/init.h"

//#include "lwip/mem.h"
//#include "lwip/memp.h"
//#include "lwip/sys.h"

//#include "lwip/stats.h"

//#include "lwip/tcp_impl.h"
//#include "lwip/inet_chksum.h"

//#include "lwip/tcpip.h"
//#include "lwip/sockets.h"

//#include "ethernetif.h"
#include "netconf.h"
#include "os_wrap.h"

//#include "gb9662if.h"
//#include "gb9619.h"
#include "wlan.h"
#include "typedefs.h"
#include "mmp_lib.h"
#ifndef LINUX
#include <stdio.h>
#endif
/*----------------------------------------------------------------------------*/
/*                      DEFINITIONS                                           */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*                      VARIABLES                                             */
/*----------------------------------------------------------------------------*/

struct net_context_t net_context;
extern struct netif main_netdev;
extern bool gbWifiConnect; 

struct wifi_info_t {
	char ssid[WIFI_SSID_LEN];
	char BSSID[6];
	int  chan;
} wifi_status;

//static AHC_BOOL m_bWPSStart = AHC_FALSE;
/*----------------------------------------------------------------------------*/
/*                      PROTOTYPES                                            */
/*----------------------------------------------------------------------------*/
const char * capmode2str(uint16 capability);	// wlu.c

bool   CheckNetmask(ip_addr_t * mask)
{
	u32_t t_mask;
	int i;
	
	if(mask == NULL)
		return false;
	t_mask = 0xFFFFFFFF;
	if(!mask->addr)
		return true;

	for(i=0; i<25; ++i){
		if(t_mask == htonl(mask->addr))
			return true;
		t_mask <<=1;
	}
	return false;
}

ip_addr_t * GetLocalhost(void)
{
	if(net_context.local_ip.addr == 0)
		return NULL;
	return &net_context.local_ip;
}

void   SetLocalhost(ip_addr_t * ip)
{
	ip_addr_set(&net_context.local_ip,ip);
}

ip_addr_t * GetPrimaryDNS(void)
{
	if(net_context.dns[0].addr == 0)
		return NULL;	
	return &net_context.dns[0];
}

void SetDNS(u8_t n,ip_addr_t *  ip)
{
	ip_addr_set(&net_context.dns[n],ip);
}

ip_addr_t * GetNetMask(void)
{
	if(net_context.net_mask.addr == 0)
		return NULL;	
	if(!CheckNetmask(&net_context.net_mask))
		net_context.net_mask.addr = htonl(0xFFFFFF00);
	return &net_context.net_mask;
}

void SetNetMask(ip_addr_t *  mask)
{
	ip_addr_set(&net_context.net_mask,mask);
}

ip_addr_t *  GetGateway(void)
{
	if(net_context.gateway_ip.addr == 0)
		return NULL;		
	return &net_context.gateway_ip;
}

void SetGateway(ip_addr_t *  ip)
{
	ip_addr_set(&net_context.gateway_ip,ip);
}

ip_addr_t * GetSecondaryDNS(void)
{
	if(net_context.dns[1].addr == 0)
		return NULL;		
	return &net_context.dns[1];
}

/* ********************************************************************* */
void ShowWlanFavAP(void)
{
	int i;

	for (i=0; i<MAX_FAV_AP_LIST; i++)
		if (*net_context.FavAPs[i].ssid) {
			LWIP_DEBUGF(OS_PORT_DEBUG, ("AP[%d]: SSID=%s crypto=%d key=%s\n", i,
					net_context.FavAPs[i].ssid,
					net_context.FavAPs[i].crypto, 
					net_context.FavAPs[i].key));
		}
}

int AddWlanFavAP(int idx, char *ssid, int crypto, char *key)
{
	int i;

	// when idx < 0, we will find an empty entry and fill it in
	if (idx >= 0) goto L_found;

	for (i=0; i<MAX_FAV_AP_LIST; i++) {
		if (*net_context.FavAPs[i].ssid == 0) {
			// found an empty entry, add here
L_found:
			idx = i;
			strncpy( net_context.FavAPs[idx].ssid, ssid, WIFI_SSID_LEN );
			strncpy( net_context.FavAPs[idx].key,  key,  WIFI_KEY_LEN );
			net_context.FavAPs[idx].crypto = crypto;
			return 0;
		}
	}
	return -1;
}

void DelWlanFavAP_idx(int idx)
{
	memset( &net_context.FavAPs[idx], 0, sizeof(struct WIFI_APConfig_t) );
}

int DelWlanFavAP_ssid(char *ssid)
{
	int ret = -1;
	int i;

	for (i=0; i<MAX_FAV_AP_LIST; i++) {
		// search all for the specified SSID, remove them (may have duplicate SSID entries)
		if (!strcmp( ssid, net_context.FavAPs[i].ssid )) {
			DelWlanFavAP_idx(i);
			ret = 0;
		}
	}
	return ret;
}

struct WIFI_APConfig_t *FindWlanFavAP(char *ssid, int *idx)
{
	int i;
	for (i=*idx; i < MAX_FAV_AP_LIST; i++) {
		if (!net_context.FavAPs[i].ssid) continue;
		if (!strcmp( ssid, net_context.FavAPs[i].ssid)) {
			*idx = i;
			return &net_context.FavAPs[i];
		}
	}
	return NULL;
}


/* ********************************************************************* */
void LwIP_show_netconfig(void)
{
	LWIP_DEBUGF(OS_PORT_DEBUG, ("main_netdev:  "));
	LWIP_DEBUGF(OS_PORT_DEBUG, ("IP=%s ",    inet_ntoa(main_netdev.ip_addr)));
	LWIP_DEBUGF(OS_PORT_DEBUG, ("GW=%s ",    inet_ntoa(main_netdev.gw)));
	LWIP_DEBUGF(OS_PORT_DEBUG, ("MASK=%s\n", inet_ntoa(main_netdev.netmask)));
	LWIP_DEBUGF(OS_PORT_DEBUG, ("net_context:  "));
	LWIP_DEBUGF(OS_PORT_DEBUG, ("IP=%s ",    inet_ntoa(net_context.local_ip)));
	LWIP_DEBUGF(OS_PORT_DEBUG, ("GW=%s ",    inet_ntoa(net_context.gateway_ip)));
	LWIP_DEBUGF(OS_PORT_DEBUG, ("MASK=%s\n", inet_ntoa(net_context.net_mask)));
	LWIP_DEBUGF(OS_PORT_DEBUG, ("wifi_status:  "));
	LWIP_DEBUGF(OS_PORT_DEBUG, ("VER:%s ", WLAN_SYS_GetVersion()));
	if (gbWifiConnect) {
		LWIP_DEBUGF(OS_PORT_DEBUG, ("SSID=%s CH=%d ", wifi_status.ssid, wifi_status.chan));
		LWIP_DEBUGF(OS_PORT_DEBUG, ("BSSID=%02x:%02x:%02x:%02x:%02x:%02x\n",
					wifi_status.BSSID[0], wifi_status.BSSID[1], wifi_status.BSSID[2],
					wifi_status.BSSID[3], wifi_status.BSSID[4], wifi_status.BSSID[5]));
	}
	else
		LWIP_DEBUGF(OS_PORT_DEBUG, ("not connected\n"));
}

void LwIP_convert_WLAN_crypto(int crypto, int *auth, int *enc)
{
	switch(crypto) {
	case WPA_CRYPTO__NONE:
		*auth = WLAN_WPA_AUTH_NONE;
		*enc  = WLAN_ENCRYPT_NONE;
		break;
	case WPA_CRYPTO__AES:
		*auth = WLAN_WPA2_AUTH_PSK;
		*enc  = WLAN_ENCRYPT_AES;
		break;
	case WPA_CRYPTO__TKIP:
		*auth = WLAN_WPA_AUTH_PSK;
		*enc  = WLAN_ENCRYPT_TKIP;
		break;
	case WPA_CRYPTO__WEP:
		*auth = WLAN_WPA_AUTH_DISABLED;
		*enc  = WLAN_ENCRYPT_WEP;
		break;
	default:
		LWIP_DEBUGF(OS_PORT_DEBUG, ("unknown crytp type %d\n", crypto));
		break;
	}
}

int LwIP_join_WLAN_AP(char *ssid, int crypto, char *key, int wait)
{
	int auth, enc, i;

	LwIP_convert_WLAN_crypto( crypto, &auth, &enc );
	*wifi_status.ssid  = 0;
	*wifi_status.BSSID = 0;

	WLAN_STA_JoinAP( (int8*)ssid, auth, enc, (int8*)key );
	for (i=0; i<wait; i++) {
		if (gbWifiConnect) return 0;  // successfully JOIN
		MMPF_OS_Sleep(1000);
	}

	return 1; // not joining
}

int LwIP_join_WLAN_auto(int retry)
{
	char *wlan_scan_buf;
	WLAN_SCAN_RESULT *list;
	WLAN_BSS_INFO *bi;
	int i, j, k;
	int showres;

	wlan_scan_buf = mem_malloc(SCAN_BUF_SIZE);
	if (wlan_scan_buf == NULL)
		LWIP_DEBUGF(OS_PORT_DEBUG, ("LwIP_join_WLAN_auto: malloc scanbuf fails \r\n"));
	list = (WLAN_SCAN_RESULT*)wlan_scan_buf;

	k = retry;
	
L_retry:
	showres = 1;
	list->count = 0;
	WLAN_STA_ScanAP((int8*)wlan_scan_buf, SCAN_BUF_SIZE);
	LWIP_DEBUGF(OS_PORT_DEBUG, ("LwIP_join_WLAN_auto: AP count = %d\r\n", list->count));
	
	if (list->count > 0) {
		for (j=0; j<MAX_FAV_AP_LIST; j++) {
			if (!net_context.FavAPs[j].ssid || net_context.FavAPs[j].ssid[0] == '\0') continue;
			bi = list->bss_info;
			for (i = 0; i < list->count; i++, bi = (WLAN_BSS_INFO*)((int8*)bi + bi->length)) {
				/* SSID */
				if (bi->SSID_len == 0) continue;
				if(showres){
					LWIP_DEBUGF(OS_PORT_DEBUG, ("Scanned AP: SSID=%s CH=%d RSSI=%ddBm BSSID=%02x:%02x:%02x:%02x:%02x:%02x Mode=%s Sec=%s\n",
						    	bi->SSID, WLAN_CHSPEC_CHANNEL(bi->chanspec), (int16)(bi->RSSI), 
						    	bi->BSSID[0], bi->BSSID[1], bi->BSSID[2], bi->BSSID[3], bi->BSSID[4], bi->BSSID[5],
						    	capmode2str(bi->capability), (bi->capability & WLAN_DOT11_CAP_PRIVACY)? "Encrypted":"Open"));
				}
				*((char*)bi->SSID + bi->SSID_len) = 0;
				if (!strcmp((const char*)bi->SSID, net_context.FavAPs[j].ssid)) {
						LWIP_DEBUGF(OS_PORT_DEBUG, ("Joining to AP:%s CRYPTO %d\t",
																net_context.FavAPs[j].ssid,
																net_context.FavAPs[j].crypto));
					if (LwIP_join_WLAN_AP((char*)net_context.FavAPs[j].ssid,
											  net_context.FavAPs[j].crypto,
											  net_context.FavAPs[j].key, 30 ) == 0) {
						LWIP_DEBUGF(OS_PORT_DEBUG, ("joined sucessful\n"));
						strncpy( wifi_status.ssid, (const char*)bi->SSID, WIFI_SSID_LEN );
						strncpy( wifi_status.BSSID, (const char*)bi->BSSID, 6 );
						wifi_status.chan = WLAN_CHSPEC_CHANNEL(bi->chanspec);
						mem_free( wlan_scan_buf );
						return 0;
					}
					else
						LWIP_DEBUGF(OS_PORT_DEBUG, ("... failed\n"));
				}
			}
			showres = 0;
		}
	}
	if (--k > 0){
		LWIP_DEBUGF(OS_PORT_DEBUG, ("LwIP_join_WLAN_auto: failed ,retry = %d\r\n",k));
		MMPF_OS_Sleep(500);   // sleeping for 0.5 sec
		goto L_retry;
	}

	LWIP_DEBUGF(OS_PORT_DEBUG, ("LwIP_join_WLAN_auto: failed to join to any AP \r\n"));
	
	mem_free( wlan_scan_buf );

	return -1;
}

int LwIP_start_DHCP(void)
{
	int wait;
	ip_addr_t ip, mask, gw;

	// dhcp_stop(&main_netdev);
	netif_set_addr(&main_netdev, 0,0,0);
	dhcp_start(&main_netdev);

	for (wait=0; wait<60; wait++) {
 	 	MMPF_OS_Sleep(500);
		if (!ip_addr_isany(&main_netdev.ip_addr)) {
			netif_set_up(&main_netdev);
			LWIP_DEBUGF(OS_PORT_DEBUG, ("DHCP %d sec : \n", wait/2));
			LwIP_show_netconfig();
			return 0;
		}
	}

	IP4_ADDR( &ip,  169, 254, 1, 123 );
	IP4_ADDR( &gw,  169, 254, 1,  1 );
	IP4_ADDR( &mask,255, 255, 0,  0 );
	netif_set_addr(&main_netdev, &ip, &mask, &gw);
	netif_set_up(&main_netdev);

	LWIP_DEBUGF(OS_PORT_DEBUG, ("DHCP: timeout !! Fall back to autoip: \n"));
	LwIP_show_netconfig();
	return -1;
}

/**
 * Start up the network interface. Both the system config <net_context>
 * and the net device <main_netdev> will be updated if applicable.
 *
 * @param ip  the local IP address of the interface
 *            NULL or empty string ==> will use system config in net_context
 *            "dhcp" ==> will start DHCP to get the IP address
 * @param gw  the gateway address
 * @return none
 */
int LwIP_start_netif(char *ip, char *gw)
{
	ip_addr_t ipAddr, maskAddr, gwAddr;

	netif_set_hostname(&main_netdev, net_context.host_name);
	if (ip && gw && *ip && *gw) {
		// STAIC-IP case, store the IP config into net_context
		ipAddr.addr = inet_addr(ip);
		gwAddr.addr = inet_addr(gw);
		if      (IP_CLASSA(ipAddr.addr))   IP4_ADDR( &maskAddr, 255, 0,   0,   0 );
		else if (IP_CLASSB(ipAddr.addr))   IP4_ADDR( &maskAddr, 255, 255, 0,   0 );
		else if (IP_CLASSC(ipAddr.addr))   IP4_ADDR( &maskAddr, 255, 255, 255, 0 );
		else if (IP_CLASSD(ipAddr.addr))   IP4_ADDR( &maskAddr, 0xf0, 0,  0,   0 );

		ip_addr_copy( net_context.local_ip,   ipAddr );
		ip_addr_copy( net_context.gateway_ip, gwAddr );
		ip_addr_copy( net_context.net_mask,   maskAddr );
	}

	if (ip_addr_isany(&net_context.local_ip) || (ip && !strcmp(ip,"dhcp"))) {
		///////////////////////////////////////////////////////////////////
		// Creates a new DHCP client for this interface on the first call.
		// Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
		// the predefined regular intervals after starting the client.
		// You can peek in the netif->dhcp struct for the actual DHCP status.
		return LwIP_start_DHCP();
	}
	else {
		netif_set_addr(&main_netdev, &net_context.local_ip, &net_context.net_mask, &net_context.gateway_ip);

		/*	When the netif is fully configured this function must be called.*/
		netif_set_up(&main_netdev);
		LWIP_DEBUGF(OS_PORT_DEBUG, ("STATIC-IP: \n"));
		LwIP_show_netconfig();
	}
	return 0;
}

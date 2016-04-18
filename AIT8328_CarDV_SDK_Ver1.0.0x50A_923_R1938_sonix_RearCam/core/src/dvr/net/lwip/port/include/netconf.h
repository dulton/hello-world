//==============================================================================
//
//  Description : Network Configuration
//  Author      : Bernard Shyu
//  Revision    : 1.0
//
//==============================================================================
#ifndef _NETCONF_H
#define _NETCONF_H

#include "lwipopts.h"
#include "lwip/ip_addr.h"
#include "lwip/memp.h"
#include "lwip/tcp.h"
#include "lwip/tcpip.h"
#include "lwip/udp.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/igmp.h"
#include "ethernetif.h"
#include "lwip.h"
//#include "AHC_Common.h"
#define MAX_NAME_LENGTH     64
#define MAX_FAV_AP_LIST     20

#define WIFI_SSID_LEN       32
#define WIFI_KEY_LEN        68 

#define	WIFI_AP_MODE		1
#define	WIFI_STATION_MODE	2

#define WPA_CRYPTO__NONE    0
#define WPA_CRYPTO__AES     1
#define WPA_CRYPTO__WEP     2
#define	WPA_CRYPTO__TKIP	3
#define WPA_CRYPTO__END     3

#define SCAN_BUF_SIZE  		4096

struct WIFI_APConfig_t {
	char ssid[WIFI_SSID_LEN];
	char key[WIFI_KEY_LEN];
	int  crypto;
};

struct net_context_t {
    char      host_name[MAX_NAME_LENGTH];
    ip_addr_t local_ip;
    ip_addr_t net_mask;
    ip_addr_t gateway_ip;
    ip_addr_t dns[2];
    int       mode; // 1:AP mode, 2: STATION mode
    struct WIFI_APConfig_t FavAPs[MAX_FAV_AP_LIST];
};



void ShowWlanFavAP(void);
int AddWlanFavAP(int idx, char *ssid, int crypto, char *key);
void DelWlanFavAP_idx(int idx);
int DelWlanFavAP_ssid(char *ssid);
struct WIFI_APConfig_t *FindWlanFavAP(char *ssid, int *idx);

void LwIP_show_netconfig(void);
int  LwIP_join_WLAN_AP(char *ssid, int crypto, char *key, int wait);
int  LwIP_join_WLAN_auto(int retry);
int  LwIP_start_DHCP(void);
int  LwIP_start_netif(char *ip, char *gw);
void LwIP_convert_WLAN_crypto(int crypto, int *auth, int *enc);

//void wifi_ctrl_init(void);
//void wifi_ctrl_reset(void);

extern struct net_context_t net_context;
#endif

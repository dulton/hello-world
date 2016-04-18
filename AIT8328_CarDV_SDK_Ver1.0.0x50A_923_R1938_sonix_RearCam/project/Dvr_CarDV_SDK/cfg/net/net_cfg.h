//------------------------------------------------------------------------------
//
//  File        : net_cfg.h
//  Description : 
//  Author      : Titan
//  Revision    : 1.0
//
//------------------------------------------------------------------------------

#ifndef _NET_CFG_H_
#define _NET_CFG_H_

#define WIFI_DRIVER_BCM			1
#define WIFI_DRIVER_MTK			2

#define WIFI_DRIVER_TYPE		WIFI_DRIVER_BCM

#if(WIFI_DRIVER_TYPE == WIFI_DRIVER_MTK)
#define LWIP_TX_DMA 
#define	WIFI_MAC_ON_HOST		1
#else
#define	WIFI_MAC_ON_HOST		0
#endif

#if(WIFI_MAC_ON_HOST)
#define SD_MAC_UPDATER_BURNIN	1
#define SD_MAC_POWERON_BURNIN	1
#define SD_MAC_REMOVE_SOURCE	1
#else
#define SD_MAC_UPDATER_BURNIN	0
#define SD_MAC_POWERON_BURNIN	0
#define SD_MAC_REMOVE_SOURCE	0
#endif

#define SD_MAC_FILENAME			"SD:\\HostBurnInMAC.bin"
#define SF_MAC_FILENAME			"SF:1:\\MAC.bin"


#endif // _NET_CFG_H_


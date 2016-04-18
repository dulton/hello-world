//==============================================================================
//
//  Description : 
//  Author      : 
//  Revision    : 
//    Truman@131224 For better portibility to other chips, switch the type from
//    AHC_Common.h to typedefs.h
//
//==============================================================================
#ifndef _WLAN_H
#define _WLAN_H

#include "net_cfg.h"

#if(WIFI_DRIVER_TYPE == WIFI_DRIVER_BCM)
#include "wlan_BCM.h"
#elif(WIFI_DRIVER_TYPE == WIFI_DRIVER_MTK)
#include "wlan_MTK.h"
#endif

#endif//_WLAN_H

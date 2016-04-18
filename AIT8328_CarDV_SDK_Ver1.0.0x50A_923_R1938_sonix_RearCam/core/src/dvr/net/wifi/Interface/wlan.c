//==============================================================================
//
//  Description :
//  Author      :
//  Revision    :
//
//==============================================================================
#include "wlan.h"

#if(WIFI_DRIVER_TYPE == WIFI_DRIVER_BCM)
#include "gb9662if.c"
#include "wlan_BCM.c"
#elif(WIFI_DRIVER_TYPE == WIFI_DRIVER_MTK)
#include "gb9662if.c"
#include "wlan_MTK.c"
#endif


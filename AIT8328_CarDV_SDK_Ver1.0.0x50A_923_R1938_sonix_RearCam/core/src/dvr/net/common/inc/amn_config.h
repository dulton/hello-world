/*
*********************************************************************************************************
*                   AIT Multimedia Network -- AIT Configuration Settings
*                     (c) Copyright 2012, Alpha Imaging Technology Corp.
*               All rights reserved. Protected by international copyright laws.
*********************************************************************************************************
*
* Description: definitions of various compiling flags of Configuration Settings for Network layer
*
*/

#ifndef __AMN_CONFIGURATION_SETTINGS_DEFINITIONS__
#define __AMN_CONFIGURATION_SETTINGS_DEFINITIONS__


//*******************************************************************************************************
#ifndef NCFG_V4L_MAX_FRAMES
#define NCFG_V4L_MAX_FRAMES                     32
#endif

#ifndef NCFG_V4L_MAX_FILE_NODES
#define NCFG_V4L_MAX_FILE_NODES                 3// 2+1 add rear MPJEG
#endif

#ifndef NCFG_V4L_H264_CONG_CONTROL
#define NCFG_V4L_H264_CONG_CONTROL             15   // 2/3 seconds
#endif
#ifndef NCFG_V4L_MJPEG_CONG_CONTROL
#define NCFG_V4L_MJPEG_CONG_CONTROL            10
#endif
#ifndef NCFG_V4L_MJPEG_CONG_RATE                    // every N'th frame will be non-droppable
#define NCFG_V4L_MJPEG_CONG_RATE               15   // i.e, equivalent to "guaranteed framerate"
#endif

// Threshold of remaining packet size to be sent in units of network MTUs, when SnkNode
// will inform the SrcNode to replenish the pipeline with the newest Video frame
#ifndef NCFG_V4L_FLOWCTL_SNK_UPDATE_MTU
#define NCFG_V4L_FLOWCTL_SNK_UPDATE_MTU         8
#endif

#ifndef NCFG_V4L_DEBUG_LEVEL
#define NCFG_V4L_DEBUG_LEVEL                    0
#endif


// CONFIG for WLAN Device Driver's Packet Buffer
//--------------------------------------------------
#ifndef NCFG_TXBUF_COUNT
#define NCFG_TXBUF_COUNT                        128
#endif
#ifndef NCFG_RXBUF_COUNT
#define NCFG_RXBUF_COUNT                        0    // means default 32
#endif


// CONFIG for Streaming
//--------------------------------------------------
#define NCFG_STREAMING_EN                       1
#define NCFG_STREAM_DISABLE_RECORDING

#ifndef NCFG_MAX_STREAMING_SESSIONS
#define NCFG_MAX_STREAMING_SESSIONS             1
#endif

// Enable special WIFI MFG mode: not for normal WLAN operation, but for EMI/MFG.
// The gb9662.a in Firmware/NET/wifi/, needs to be replaced with gb9662_MFG.a
//--------------------------------------------------
#ifndef NCFG_WIFI_MFG_EN
#define NCFG_WIFI_MFG_EN                        0
#endif


// CONFIG for User / Default Config files
//--------------------------------------------------
#ifndef NCFG_USERSET_FILE_NAME
#define NCFG_USERSET_FILE_NAME	"SF:1:\\usernetconf.txt"
#endif
#ifndef NCFG_DEFSET_FILE_NAME
#define NCFG_DEFSET_FILE_NAME	"SF:1:\\defnetconf.txt"
#endif

#define CONFIG_SPACE_NUMBER          3
#define CONFIG_SPACE_TYPE_CURRENT    0
#define CONFIG_SPACE_TYPE_FACTORY    1
#define CONFIG_SPACE_TYPE_STARTUP    2

unsigned int export_net_config_file(int type, char* path);


//*******************************************************************************************************

#endif

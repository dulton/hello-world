//==============================================================================
//
//  File        : sensor_Mod_Remapping.h
//  Description : Firmware Sensor Control File
//  Author      : Andy Liu
//  Revision    : 1.0
//
//==============================================================================

#ifndef	_SENSOR_MOD_REMAPPING_
#define	_SENSOR_MOD_REMAPPING_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "includes_fw.h"
#include "customer_config.h"

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#ifndef SENSOR_DRIVER_MODE_NOT_SUUPORT
#define SENSOR_DRIVER_MODE_NOT_SUUPORT  (0xFFFF)
#endif

#if (BIND_SENSOR_AR0330)
#include "sensor_ar0330.h"
#endif // BIND_SENSOR_AR0330

#if (BIND_SENSOR_AR0330_OTPM)
#include "sensor_ar0330_OTPM.h"
#endif // BIND_SENSOR_AR0330_OTPM

#if (BIND_SENSOR_AR0331)
#include "sensor_ar0331.h"
#endif // (BIND_SENSOR_AR0331)

#if (BIND_SENSOR_AR0835)
#include "sensor_ar0835.h"
#endif // (BIND_SENSOR_AR0835)

#if (BIND_SENSOR_AR1820)
#include "sensor_ar1820.h"
#endif // (BIND_SENSOR_AR1820)

#if (BIND_SENSOR_IMX175)
#include "sensor_imx175.h"
#endif // (BIND_SENSOR_IMX175)

#if (BIND_SENSOR_IMX214)
#include "sensor_imx214.h"
#endif // (BIND_SENSOR_IMX214)

#if (BIND_SENSOR_IMX322)
#include "sensor_imx322.h"
#endif // (BIND_SENSOR_IMX322)

#if (BIND_SENSOR_OV2710)
#include "sensor_ov2710.h"
#endif // (BIND_SENSOR_OV2710)

#if (BIND_SENSOR_OV2710_MIPI)
#include "sensor_ov2710_mipi.h"
#endif // (BIND_SENSOR_OV2710_MIPI)

#if (BIND_SENSOR_H42_MIPI)
#include "sensor_H42_mipi.h"
#endif // (BIND_SENSOR_H42_MIPI)

#if (BIND_SENSOR_OV4689)
#include "sensor_ov4689.h"
#endif // (BIND_SENSOR_OV4689)

#if (BIND_SENSOR_OV4689_2LINE)
#include "sensor_ov4689_2lane.h"
#endif // (BIND_SENSOR_OV4689_2LINE)

#if (BIND_SENSOR_OV5653)
#include "sensor_ov5653.h"
#endif // (BIND_SENSOR_OV5653)

#if (BIND_SENSOR_OV9712)
#include "sensor_ov9712.h"
#endif // (BIND_SENSOR_OV9712)

#if (BIND_SENSOR_OV9732_MIPI)
#include "sensor_ov9732_mipi.h"
#endif // (BIND_SENSOR_OV9732_MIPI)

#if (BIND_SENSOR_OV10822)
#include "sensor_ov10822.h"
#endif // (BIND_SENSOR_OV10822)

#if (BIND_SENSOR_DM5150)
#include "TvDecoder_dm5150.h"
#endif // (BIND_SENSOR_DM5150)

#if (BIND_SENSOR_BIT1603)
#include "TvDecoder_BIT1603.h"
#endif // (BIND_SENSOR_BIT1603)

#if (BIND_SENSOR_CJC5150)
#include "TvDecoder_CJC5150.h"
#endif // (BIND_SENSOR_CJC5150)

#if (BIND_SENSOR_GM7150)
#include "TvDecoder_GM7150.h"
#endif // (BIND_SENSOR_GM7150)

#if (BIND_SENSOR_CP8210)
#include "sensor_cp8210.h"
#endif // (BIND_SENSOR_CP8210)

#if (BIND_SENSOR_PS1210)
#include "sensor_ps1210.h"
#endif // (BIND_SENSOR_PS1210)

#if (BIND_SENSOR_JXF02)
#include "sensor_jxf02.h"
#endif // (BIND_SENSOR_JXF02)

/////////////////////////////////////////////////////////////////////////////////////////////////
// Keep this at last line
#ifndef SENSOR_DRIVER_MODE_PCCAM_DEFAULT_RESOLUTION
#define SENSOR_DRIVER_MODE_PCCAM_DEFAULT_RESOLUTION     (SENSOR_DRIVER_MODE_FULL_HD_30P_RESOLUTION)
#endif

#endif	// _SENSOR_MOD_REMAPPING_

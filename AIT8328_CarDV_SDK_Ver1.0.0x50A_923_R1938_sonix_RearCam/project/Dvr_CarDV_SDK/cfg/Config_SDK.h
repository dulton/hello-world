/*
 * PCBA config file
 */
#ifndef __CONFIG_SDK__
#define __CONFIG_SDK__

#include "dsc_key.h"
#include "Mmps_3gprecd.h"
#include "all_fw.h"
#include "mmps_pio.h"
#include "mmpf_sensor.h"
#include "AHC_common.h"
#if !defined(MINIBOOT_FW) && !defined(UPDATER_FW)
#include "GUI.h"
#endif

/*===========================================================================
 *  DBG Config
 *===========================================================================*/

#ifndef CFG_DBG_AIT8428_HDK_BD      // defined in MCP target
#define CFG_DBG_AIT8428_HDK_BD 		(4) // 0: AIT8428 EVB (old blue),   1: AIT8428 HDK BD V01 (new),

//Do not put customer project config file here!!!

// 0: AIT8428 EVB (old blue),   1: AIT8428 HDK BD V01 (new),
// 2: AIT8428 EVB (old Green),  3: AIT8428 EVB VersionC (green)
// 4: AIT8428 EVB VersionC (green) for TV decoder Bitek1603 rear cam
// 6: AIT8428 customer(ALONG)


#endif

#if (CFG_DBG_AIT8428_HDK_BD == 0)
#include "Config_SDK_EVB_A.h"
#elif (CFG_DBG_AIT8428_HDK_BD == 1)
#include "Config_SDK_BD_V01.h"
#elif (CFG_DBG_AIT8428_HDK_BD == 2)
#include "Config_SDK_EVB_B.h"
#elif (CFG_DBG_AIT8428_HDK_BD == 3)
#include "Config_SDK_EVB_C.h"
#elif (CFG_DBG_AIT8428_HDK_BD == 4)
//#include "Config_SDK_EVB_JWD_TV_Decoder.h"
#include "Config_SDK_EVB_C_TV_Decoder.h"
#elif (CFG_DBG_AIT8428_HDK_BD == 6)
#include "Config_ALong.h"
#elif (CFG_DBG_AIT8428_HDK_BD == 7)
#include "Config_HSG601.h"
#elif (CFG_DBG_AIT8428_HDK_BD == 8)
#include "Config_SDK_5608_TV_Decoder.h"
#else
#error Wrong Board setting - CFG_DBG_AIT8428_HDK_BD
#endif

#define CUS_MIC_SENSITIVITY

#endif // __CONFIG_SDK__

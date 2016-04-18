/// @ait_only
//==============================================================================
//
//  File        : mmpd_system.h
//  Description : INCLUDE File for the Host System Control Driver.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

/**
 *  @file mmpd_system.h
 *  @brief The header File for the Host System Control Driver
 *  @author Penguin Torng
 *  @version 1.0
 */

#ifndef _MMPD_SYSTEM_H_
#define _MMPD_SYSTEM_H_

/** @addtogroup MMPD_System
 *  @{
 */

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "ait_config.h"
#include "config_fw.h"
#include "mmpf_timer.h"
#include "mmp_clk_inc.h"
#include "mmp_gpio_inc.h"
#include "mmp_display_inc.h"

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

// These declaration should be sync with fw mmpf_system.h
#define COMPILER_DATE_LEN (12) /// "mmm dd yyyy"
#define COMPILER_TIME_LEN (10) /// "hh:mm:ss" 9 bytes but word alignment

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

typedef enum _MMPD_SYS_MDL
{
    MMPD_SYS_MDL_VIF0       = 0,
    MMPD_SYS_MDL_VIF1       = 1,
    MMPD_SYS_MDL_VIF2       = 2,
    MMPD_SYS_MDL_RAW_S0     = 3,
    MMPD_SYS_MDL_RAW_S1     = 4,
    MMPD_SYS_MDL_RAW_S2     = 5,
    MMPD_SYS_MDL_RAW_F      = 6,
    MMPD_SYS_MDL_ISP        = 7,

    MMPD_SYS_MDL_SCAL0      = 8,
    MMPD_SYS_MDL_SCAL1      = 9,
    MMPD_SYS_MDL_SCAL2      = 10,
    MMPD_SYS_MDL_SCAL3      = 11,
    MMPD_SYS_MDL_ICON0      = 12,
    MMPD_SYS_MDL_ICON1      = 13,
    MMPD_SYS_MDL_ICON2      = 14,
    MMPD_SYS_MDL_ICON3      = 15,

    MMPD_SYS_MDL_IBC0       = 16,
    MMPD_SYS_MDL_IBC1       = 17,
    MMPD_SYS_MDL_IBC2       = 18,
    MMPD_SYS_MDL_IBC3       = 19,
    MMPD_SYS_MDL_CCIR       = 20,
    MMPD_SYS_MDL_HDMI       = 21,
    MMPD_SYS_MDL_DSPY       = 22,
    MMPD_SYS_MDL_TV         = 23,

    MMPD_SYS_MDL_MCI        = 24,
    MMPD_SYS_MDL_DRAM       = 25,
    MMPD_SYS_MDL_H264       = 26,
    MMPD_SYS_MDL_JPG        = 27,
    MMPD_SYS_MDL_SD0        = 28,
    MMPD_SYS_MDL_SD1        = 29,
    MMPD_SYS_MDL_SD2        = 30,
    MMPD_SYS_MDL_SD3        = 31,

    MMPD_SYS_MDL_CPU_PHL    = 32,
    MMPD_SYS_MDL_PHL        = 33,
    MMPD_SYS_MDL_AUD        = 34,
    MMPD_SYS_MDL_GRA        = 35,
    MMPD_SYS_MDL_DMA_M0     = 36,
    MMPD_SYS_MDL_DMA_M1     = 37,
    MMPD_SYS_MDL_DMA_R0     = 38,
    MMPD_SYS_MDL_DMA_R1     = 39,

    MMPD_SYS_MDL_GPIO       = 40,
    MMPD_SYS_MDL_PWM        = 41,
    MMPD_SYS_MDL_PSPI       = 42,
    MMPD_SYS_MDL_DBIST      = 43,
    MMPD_SYS_MDL_IRDA       = 44,
    MMPD_SYS_MDL_RTC        = 45,
    MMPD_SYS_MDL_USB        = 46,
    MMPD_SYS_MDL_USBPHY     = 47,

    MMPD_SYS_MDL_I2CS       = 48,
    MMPD_SYS_MDL_SM         = 49,
    MMPD_SYS_MDL_LDC        = 50,
    MMPD_SYS_MDL_PLL_JITTER = 51,
    MMPD_SYS_MDL_SCAL4      = 52,
    MMPD_SYS_MDL_ICON4      = 53,
    MMPD_SYS_MDL_IBC4       = 54,
    MMPD_SYS_MDL_SCAL_RS    = 55,

    MMPD_SYS_MDL_NUM        = 56
} MMPD_SYS_MDL;

typedef enum _MMPD_SYS_CLK
{
    MMPD_SYS_CLK_CPU_A      = 0,
    MMPD_SYS_CLK_CPU_A_PHL  = 1,
    MMPD_SYS_CLK_CPU_B      = 2,
    MMPD_SYS_CLK_CPU_B_PHL  = 3,
    MMPD_SYS_CLK_MCI        = 4,
    MMPD_SYS_CLK_DRAM       = 5,
    MMPD_SYS_CLK_VIF        = 6,
    MMPD_SYS_CLK_RAW_F      = 7,

    MMPD_SYS_CLK_RAW_S0     = 8,
    MMPD_SYS_CLK_RAW_S1     = 9,
    MMPD_SYS_CLK_RAW_S2     = 10,
    MMPD_SYS_CLK_ISP        = 11,
    MMPD_SYS_CLK_COLOR_MCI  = 12,
    MMPD_SYS_CLK_GNR        = 13,
    MMPD_SYS_CLK_SCALE      = 14,
    MMPD_SYS_CLK_ICON       = 15,

    MMPD_SYS_CLK_IBC        = 16,
    MMPD_SYS_CLK_CCIR       = 17,
    MMPD_SYS_CLK_DSPY       = 18,
    MMPD_SYS_CLK_HDMI       = 19,
    MMPD_SYS_CLK_TV         = 20,
    MMPD_SYS_CLK_JPG        = 21,
    MMPD_SYS_CLK_H264       = 22,
    MMPD_SYS_CLK_GRA        = 23,

    MMPD_SYS_CLK_DMA        = 24,
    MMPD_SYS_CLK_PWM        = 25,
    MMPD_SYS_CLK_PSPI       = 26,
    MMPD_SYS_CLK_SM         = 27,
    MMPD_SYS_CLK_SD0        = 28,
    MMPD_SYS_CLK_SD1        = 29,
    MMPD_SYS_CLK_SD2        = 30,
    MMPD_SYS_CLK_SD3        = 31,
    MMPD_SYS_CLK_GRP0_NUM   = 32,
    
    MMPD_SYS_CLK_USB        = 32,
    MMPD_SYS_CLK_I2CM       = 33,
    MMPD_SYS_CLK_BS_SPI     = 34,
    MMPD_SYS_CLK_GPIO       = 35,
    MMPD_SYS_CLK_AUD        = 36,
    MMPD_SYS_CLK_ADC        = 37,
    MMPD_SYS_CLK_DAC        = 38,
    MMPD_SYS_CLK_IRDA       = 39,

    MMPD_SYS_CLK_LDC        = 40,
    MMPD_SYS_CLK_BAYER      = 41,
    MMPD_SYS_CLK_MDL_NUM    = 42
} MMPD_SYS_CLK;

typedef enum _MMPD_SYSTEM_BYPASS_MODE
{
    MMPD_SYSTEM_ENTER_BYPASS = 0,
    MMPD_SYSTEM_EXIT_BYPASS
} MMPD_SYSTEM_BYPASS_MODE;

typedef enum _MMPD_SYSTEM_PS_MODE
{
    MMPD_SYSTEM_ENTER_PS = 0,
    MMPD_SYSTEM_EXIT_PS
} MMPD_SYSTEM_PS_MODE;

typedef enum _MMPD_SYSTEM_WAKEUP_EVNET
{
	MMPD_SYSTEM_WAKEUP_NONE = 0x0,
	MMPD_SYSTEM_WAKEUP_GPIO,
	MMPD_SYSTEM_WAKEUP_TVPLUG_IN,
	MMPD_SYSTEM_WAKEUP_USB_RESUME,
	MMPD_SYSTEM_WAKEUP_HDMI_HPD,
	MMPD_SYSTEM_WAKEUP_RTC_CNT_VALID,
	MMPD_SYSTEM_WAKEUP_RTC_VALID,
	MMPD_SYSTEM_WAKEUP_RTC_INT,
	MMPD_SYSTEM_WAKEUP_MAX
} MMPD_SYSTEM_WAKEUP_EVNET;

typedef enum _MMPD_SYSTEM_MEM_DIR
{
    MMPD_SYSTEM_MEM_TOP = 0,           //#define MEM_ALLOC_FROM_TOP 
    MMPD_SYSTEM_MEM_BOTTOM             //#define MEM_ALLOC_FROM_BOTTOM 
} MMPD_SYSTEM_MEM_DIR;

//==============================================================================
//
//                              STRUCTURE
//
//==============================================================================

/** @brief Compiler build version

This version is build by compiler.
The human modified version might not correct. Use this version to compare
the version.
*/
typedef struct MMPD_SYSTEM_BUILD_VERSION {
    MMP_UBYTE szDate[COMPILER_DATE_LEN]; /// "mmm dd yyyy"
    MMP_UBYTE szTime[COMPILER_TIME_LEN]; /// "hh:mm:ss" 9 bytes
} MMPD_SYSTEM_BUILD_VERSION;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

MMP_ERR MMPD_System_ResetVariables(void);
MMP_ERR MMPD_System_EnableClock(MMPD_SYS_CLK clocktype, MMP_BOOL bEnableclock);
MMP_ERR MMPD_System_ResetHModule(MMPD_SYS_MDL moduletype, MMP_BOOL bResetRegister);
MMP_ERR MMPD_System_TVInitialize(MMP_BOOL bInit);
MMP_ERR MMPD_System_TVColorBar(MMP_TV_TYPE tvType, MMP_BOOL turnOn, MMP_UBYTE colorBarType);

MMP_ERR MMPD_System_ResetCPU(const MMP_BOOL bLock);
MMP_ERR MMPD_System_DownloadFW(MMP_ULONG ulDevaddr,MMP_USHORT *usFwaddr, MMP_ULONG ulLength);

MMP_ERR MMPD_System_SetBypassMode(MMPD_SYSTEM_BYPASS_MODE bypassmode);
MMP_ERR MMPD_System_SetPSMode(MMPD_SYSTEM_PS_MODE psmode);

MMP_ERR MMPD_System_GetFWEndAddr(MMP_ULONG *ulAddress);
MMP_ERR MMPD_System_GetAudioFWStartAddr(MMP_ULONG ulRegion, MMP_ULONG *ulAddress);
MMP_ERR MMPD_System_GetSramEndAddr(MMP_ULONG *ulAddress);
MMP_ERR MMPD_System_ResetSystem(void);
MMP_ERR MMPD_System_SetWakeUpEvent(MMP_BOOL bEnable, MMPD_SYSTEM_WAKEUP_EVNET event, MMP_GPIO_PIN piopin, MMP_UBYTE polarity);
MMP_ERR MMPD_System_CheckVersion(void);
MMP_ERR MMPD_System_SendEchoCommand(void);
MMP_UBYTE MMPD_System_ReadCoreID(void);
MMP_ERR MMPD_System_GetGroupSrcAndDiv(CLK_GRP ubGroup, MMP_ULONG *ulGroupSrc, MMP_UBYTE *ubPllDiv);
MMP_ERR MMPD_System_GetGroupFreq(CLK_GRP ubGroup, MMP_ULONG *ulGroupFreq);
MMP_ERR MMPD_System_SetAudioGroupFract(MMP_LONG ulFract);
MMP_ERR MMPD_System_SetSelfSleepMode(void);

/// @}

#endif // _MMPD_SYSTEM_H_

/// @end_ait_only


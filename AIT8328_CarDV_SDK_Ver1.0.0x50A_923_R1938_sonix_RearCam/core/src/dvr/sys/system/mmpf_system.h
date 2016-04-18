/**
 @file mmpf_system.h
 @brief Header File for the mmpf system.
 @author Truman Yang
 @version 1.0
*/

/** @addtogroup MMPF_SYS
@{
*/

#ifndef _MMPF_SYSTEM_H_
#define _MMPF_SYSTEM_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//============================================================================== 

#include "includes_fw.h"
#include "mmpf_pll.h"
#include "mmpf_timer.h"
#include "mmp_gpio_inc.h"

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#define COMPILER_DATE_LEN   (12) /// "mmm dd yyyy"
#define COMPILER_TIME_LEN   (10) /// "hh:mm:ss" 9 bytes but word alignment

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

typedef enum _MMPF_SYS_MDL
{
    MMPF_SYS_MDL_VIF0       = 0,
    MMPF_SYS_MDL_VIF1       = 1,
    MMPF_SYS_MDL_VIF2       = 2,
    MMPF_SYS_MDL_RAW_S0     = 3,
    MMPF_SYS_MDL_RAW_S1     = 4,
    MMPF_SYS_MDL_RAW_S2     = 5,
    MMPF_SYS_MDL_RAW_F      = 6,
    MMPF_SYS_MDL_ISP        = 7,

    MMPF_SYS_MDL_SCAL0      = 8,
    MMPF_SYS_MDL_SCAL1      = 9,
    MMPF_SYS_MDL_SCAL2      = 10,
    MMPF_SYS_MDL_SCAL3      = 11,
    MMPF_SYS_MDL_ICON0      = 12,
    MMPF_SYS_MDL_ICON1      = 13,
    MMPF_SYS_MDL_ICON2      = 14,
    MMPF_SYS_MDL_ICON3      = 15,

    MMPF_SYS_MDL_IBC0       = 16,
    MMPF_SYS_MDL_IBC1       = 17,
    MMPF_SYS_MDL_IBC2       = 18,
    MMPF_SYS_MDL_IBC3       = 19,
    MMPF_SYS_MDL_CCIR       = 20,
    MMPF_SYS_MDL_HDMI       = 21,
    MMPF_SYS_MDL_DSPY       = 22,
    MMPF_SYS_MDL_TV         = 23,

    MMPF_SYS_MDL_MCI        = 24,
    MMPF_SYS_MDL_DRAM       = 25,
    MMPF_SYS_MDL_H264       = 26,
    MMPF_SYS_MDL_JPG        = 27,
    MMPF_SYS_MDL_SD0        = 28,
    MMPF_SYS_MDL_SD1        = 29,
    MMPF_SYS_MDL_SD2        = 30,
    MMPF_SYS_MDL_SD3        = 31,

    MMPF_SYS_MDL_CPU_PHL    = 32,
    MMPF_SYS_MDL_PHL        = 33,
    MMPF_SYS_MDL_AUD        = 34,
    MMPF_SYS_MDL_GRA        = 35,
    MMPF_SYS_MDL_DMA_M0     = 36,
    MMPF_SYS_MDL_DMA_M1     = 37,
    MMPF_SYS_MDL_DMA_R0     = 38,
    MMPF_SYS_MDL_DMA_R1     = 39,

    MMPF_SYS_MDL_GPIO       = 40,
    MMPF_SYS_MDL_PWM        = 41,
    MMPF_SYS_MDL_PSPI       = 42,
    MMPF_SYS_MDL_DBIST      = 43,
    MMPF_SYS_MDL_IRDA       = 44,
    MMPF_SYS_MDL_RTC        = 45,
    MMPF_SYS_MDL_USB        = 46,
    MMPF_SYS_MDL_USBPHY     = 47,

    MMPF_SYS_MDL_I2CS       = 48,
    MMPF_SYS_MDL_SM         = 49,
    MMPF_SYS_MDL_LDC        = 50,
    MMPF_SYS_MDL_PLL_JITTER = 51,
    MMPF_SYS_MDL_SCAL4      = 52,
    MMPF_SYS_MDL_ICON4      = 53,
    MMPF_SYS_MDL_IBC4       = 54,
    MMPF_SYS_MDL_SCAL_RS    = 55,
    MMPF_SYS_MDL_NUM        = 56
} MMPF_SYS_MDL;

typedef enum _MMPF_SYS_CLK
{
    MMPF_SYS_CLK_CPU_A          = 0,
    MMPF_SYS_CLK_CPU_A_PHL      = 1,
    MMPF_SYS_CLK_CPU_B          = 2,
    MMPF_SYS_CLK_CPU_B_PHL      = 3,
    MMPF_SYS_CLK_MCI            = 4,
    MMPF_SYS_CLK_DRAM           = 5,
    MMPF_SYS_CLK_VIF            = 6,
    MMPF_SYS_CLK_RAW_F          = 7,

    MMPF_SYS_CLK_RAW_S0         = 8,
    MMPF_SYS_CLK_RAW_S1         = 9,
    MMPF_SYS_CLK_RAW_S2         = 10,
    MMPF_SYS_CLK_ISP            = 11,
    MMPF_SYS_CLK_COLOR_MCI      = 12,
    MMPF_SYS_CLK_GNR            = 13,
    MMPF_SYS_CLK_SCALE          = 14,
    MMPF_SYS_CLK_ICON           = 15,

    MMPF_SYS_CLK_IBC            = 16,
    MMPF_SYS_CLK_CCIR           = 17,
    MMPF_SYS_CLK_DSPY           = 18,
    MMPF_SYS_CLK_HDMI           = 19,
    MMPF_SYS_CLK_TV             = 20,
    MMPF_SYS_CLK_JPG            = 21,
    MMPF_SYS_CLK_H264           = 22,
    MMPF_SYS_CLK_GRA            = 23,

    MMPF_SYS_CLK_DMA            = 24,
    MMPF_SYS_CLK_PWM            = 25,
    MMPF_SYS_CLK_PSPI           = 26,
    MMPF_SYS_CLK_SM             = 27,
    MMPF_SYS_CLK_SD0            = 28,
    MMPF_SYS_CLK_SD1            = 29,
    MMPF_SYS_CLK_SD2            = 30,
    MMPF_SYS_CLK_SD3            = 31,
    #define MMPF_SYS_CLK_SD(x)  (x + MMPF_SYS_CLK_SD0)
    MMPF_SYS_CLK_GRP0_NUM       = 32,
    
    MMPF_SYS_CLK_USB            = 32,
    MMPF_SYS_CLK_I2CM           = 33,
    MMPF_SYS_CLK_BS_SPI         = 34,
    MMPF_SYS_CLK_GPIO           = 35,
    MMPF_SYS_CLK_AUD            = 36,
    MMPF_SYS_CLK_ADC            = 37,
    MMPF_SYS_CLK_DAC            = 38,
    MMPF_SYS_CLK_IRDA           = 39,

    MMPF_SYS_CLK_LDC            = 40,
    MMPF_SYS_CLK_BAYER          = 41,
    MMPF_SYS_CLK_MDL_NUM        = 42
} MMPF_SYS_CLK;

typedef enum _MMPF_WAKEUP_EVNET
{
	MMPF_WAKEUP_NONE = 0x0,
	MMPF_WAKEUP_GPIO,
	MMPF_WAKEUP_TVPLUG_IN,
	MMPF_WAKEUP_USB_RESUME,
	MMPF_WAKEUP_HDMI_HPD,
	MMPF_WAKEUP_RTC_CNT_VALID,
	MMPF_WAKEUP_RTC_VALID,
	MMPF_WAKEUP_RTC_INT,
	MMPF_WAKEUP_MAX
} MMPF_WAKEUP_EVNET;

#if (DBG_ISR_EXE_TIME == 1) //&& defined(ALL_FW)
typedef enum __MMPF_SYS_ISR_TYPE {
  MMP_SYS_VIF_ISR = 0,
  MMP_SYS_DMA_ISR,
  MMP_SYS_IBC_ISR,
  MMP_SYS_AUD_ISR,
  MMP_SYS_USB_ISR,
  MMP_SYS_JPG_ISR,
  MMP_SYS_H264_ISR,
  MMP_SYS_TIMER_ISR,
  MMP_SYS_DSPY_ISR,
  MMP_SYS_ISP_ISR,
  MMP_SYS_SD_ISR,
  MMP_SYS_GRA_ISR,
  MMP_SYS_ISR_MAX
} MMPF_SYS_ISR_TYPE;
#endif

#if (TASK_MONITOR_ENABLE)
#define TASK_MONITOR_MAXTASKNAMELEN (32)
#define TASK_MONITOR_MAXTASKNUM     (128)
#define TASK_MONITOR_MAXPARASIZE    (128)
#define TASK_MONITOR_TIMER_INTERVAL (800) //ms
#define TASK_MONITOR_TIMEOUT        (10000)//(3000) //ms
typedef void (TASK_MONITOR_TimeoutCallback) (void);
#endif

#if (SYSTEM_FLOW_MONITOR_ENABLE)
#define SYSTEM_FLOW_MONITOR_TIMER_INTERVAL (300) //ms

#define SYSTEM_FLOW_MONITOR_MAX_NAME_LEN (12)
#define SYSTEM_FLOW_MONITOR_MAX_TRACKING_NUM_GLOBAL     (64)
#define SYSTEM_FLOW_MONITOR_MAX_TRACKING_NUM_PIPELINE     (64)
#define SYSTEM_FLOW_MONITOR_MAX_TRACKING_NUM_VIDEO_RECORD     (64)
#define SYSTEM_FLOW_MONITOR_MAX_TRACKING_NUM_VIDEO_PLAYBACK     (64)
#define SYSTEM_FLOW_MONITOR_MAX_TRACKING_NUM_DSC_CAPTURE     (64)
#define SYSTEM_FLOW_MONITOR_MAX_TRACKING_NUM_DSC_PLAYBACK     (64)
#define SYSTEM_FLOW_MONITOR_MAX_TRACKING_NUM_AUDIO_RECORD     (64)
#define SYSTEM_FLOW_MONITOR_MAX_TRACKING_NUM_AUDIO_PLAYBACK     (64)
#define SYSTEM_FLOW_MONITOR_MAX_TRACKING_NUM_PERIPHERAL     (64)
#define SYSTEM_FLOW_MONITOR_MAX_TRACKING_NUM_WIFI     (64)
#define SYSTEM_FLOW_MONITOR_MAX_TRACKING_NUM_UI     (64)

typedef enum _MMPF_SYSTEM_FLOW_MONITOR_GROUP
{
    MMPF_SYSTEM_FLOW_MONITOR_GLOBAL = 0x0,

    MMPF_SYSTEM_FLOW_MONITOR_PIPELINE,

    MMPF_SYSTEM_FLOW_MONITOR_VIDEO_RECORD,
    MMPF_SYSTEM_FLOW_MONITOR_VIDEO_PLAYBACK,

    MMPF_SYSTEM_FLOW_MONITOR_DSC_CAPTURE,
    MMPF_SYSTEM_FLOW_MONITOR_DSC_PLAYBACK,

    MMPF_SYSTEM_FLOW_MONITOR_AUDIO_RECORD,
    MMPF_SYSTEM_FLOW_MONITOR_AUDIO_PLAYBACK,

    MMPF_SYSTEM_FLOW_MONITOR_PERIPHERAL,

    MMPF_SYSTEM_FLOW_MONITOR_WIFI,

    MMPF_SYSTEM_FLOW_MONITOR_UI,

    MMPF_SYSTEM_FLOW_MONITOR_MAX
} MMPF_SYSTEM_FLOW_MONITOR_GROUP;

typedef MMP_BOOL (SYSTEM_FLOW_MONITOR_CheckFunction) (void);
typedef void (SYSTEM_FLOW_MONITOR_FailureCallBack) (void);
#endif

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef struct MMP_RELEASE_VERSION {
    MMP_UBYTE   major;
    MMP_UBYTE   minor;
    MMP_USHORT  build;
} MMP_RELEASE_VERSION;

typedef struct MMP_RELEASE_DATE {
    MMP_UBYTE   year;
    MMP_UBYTE   month;
    MMP_UBYTE   day;
} MMP_RELEASE_DATE;

typedef struct MMP_SYS_BUILD_TIME {
    MMP_UBYTE 	szDate[COMPILER_DATE_LEN]; /// "mmm dd yyyy"
    MMP_UBYTE 	szTime[COMPILER_TIME_LEN]; /// "hh:mm:ss" 9 bytes
} MMP_SYSTEM_BUILD_TIME;

/*
 * Booting parameters
 */
typedef struct MMP_SYS_BOOT_PARAM {
    MMP_UBYTE   ubEcoVer;
    MMP_UBYTE   ubReserve[31];
} MMP_SYSTEM_BOOT_PARAM;

#if (TASK_MONITOR_ENABLE)
typedef enum _MMPF_TASK_MONITOR_STATES
{
    MMPF_TASK_MONITOR_STATES_WAITING = 0,
    MMPF_TASK_MONITOR_STATES_RUNNING    
} MMPF_TASK_MONITOR_STATES;

typedef struct _MMPF_TASK_MONITOR {
    MMP_UBYTE TaskName[TASK_MONITOR_MAXTASKNAMELEN];
    MMPF_TASK_MONITOR_STATES sTaskMonitorStates;
    MMP_ULONG ulExecTime;
    MMP_UBYTE ParaArray[TASK_MONITOR_MAXTASKNUM];
    MMP_ULONG ulParaLength;
    TASK_MONITOR_TimeoutCallback *pTimeoutCB;
} MMPF_TASK_MONITOR;
#endif

#if (SYSTEM_FLOW_MONITOR_ENABLE)
typedef enum _MMPF_SYSTEM_FLOW_MONITOR_STATES
{
    MMPF_SYSTEM_FLOW_MONITOR_STATES_WAITING = 0,
    MMPF_SYSTEM_FLOW_MONITOR_STATES_RUNNING    
} MMPF_SYSTEM_FLOW_MONITOR_STATES;

typedef struct _MMPF_SYSTEM_FLOW_MONITOR { //sizeof(MMPF_SYSTEM_FLOW_MONITOR)=32;
    MMP_UBYTE SystemFlowName[SYSTEM_FLOW_MONITOR_MAX_NAME_LEN];
    MMPF_SYSTEM_FLOW_MONITOR_STATES sSystemMonitorStates;
    MMP_ULONG ulExecTime;
    MMP_ULONG ulTimeout; //If ulTimeout is 0, system flow checking routine will call pCheckFunc directly.
    SYSTEM_FLOW_MONITOR_CheckFunction *pCheckFunc;
    SYSTEM_FLOW_MONITOR_FailureCallBack *pFailureCB;
} MMPF_SYSTEM_FLOW_MONITOR; 
#endif

//==============================================================================
//
//                              VARIABLES
//
//==============================================================================

typedef MMP_BOOL PollingCallBackFunc(void);

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

MMP_ERR	MMPF_SYS_ResetHModule(MMPF_SYS_MDL moduletype, MMP_BOOL bResetRegister);
MMP_ERR MMPF_SYS_ResetVariables(void);
MMP_UBYTE MMPF_SYS_ReadCoreID(void);
MMP_ERR MMPF_SYS_EnterPSMode(MMP_BOOL bEnterPSMode);
MMP_ERR MMPF_SYS_EnterBypassMode(MMP_BOOL bEnterBypassMode);
MMP_ERR MMPF_SYS_EnterSelfSleepMode(void);
MMP_ERR MMPF_SYS_GetFWEndAddr(MMP_ULONG *ulEndAddr);
MMP_ERR MMPF_SYS_GetFWFBEndAddr(MMP_ULONG *ulEndAddr);
MMP_ERR MMPF_SYS_SetWakeUpEvent(MMP_BOOL bEnable, MMPF_WAKEUP_EVNET event, MMP_GPIO_PIN piopin, MMP_UBYTE polarity);
MMP_ERR MMPF_SYS_EnableClock(MMPF_SYS_CLK clockTypeIdx, MMP_BOOL bEnableclock);
MMP_BOOL MMPF_SYS_CheckClockEnable(MMPF_SYS_CLK clockTypeIdx);

MMP_ERR MMPF_SYS_StartTimerCount(void);
MMP_ERR MMPF_SYS_WaitUs(MMP_ULONG ulus, MMP_BOOL *bPollingDone, PollingCallBackFunc *Func);
MMP_ERR MMPF_SYS_GetTimerCount(MMP_ULONG *ulTick, MMP_ULONG *ulTickPerUs);
#if (DBG_ISR_EXE_TIME == 1)
MMP_ERR MMPF_SYS_GetISRExeTime(MMPF_SYS_ISR_TYPE ISRType, MMP_BOOL bStartExeTime, MMP_ULONG ulExeTimeLimit);
#endif
#if (TASK_MONITOR_ENABLE)
MMP_ERR MMPF_TaskMonitor_RegisterTask(MMPF_TASK_MONITOR *pTaskMonitor);
MMP_ERR MMPF_TaskMonitor_UnRegisterTask(MMPF_TASK_MONITOR *pTaskMonitor);
MMP_ERR MMPF_TaskMonitor_RegisterGblCB(void *pTaskMonitorGblCB);
void MMPF_TaskMonitor_PrintAllTaskInfo(void);
void MMPF_TaskMonitor_TimerCB(void);
#endif

#endif	//_MMPF_SYSTEM_H_


//#define AHC_PRINT_RET_ERROR(ret_val) printc(FG_RED("%s,%d error:0x%x!\r\n"),__func__,__LINE__, ret_val)

/** @}*/ //end of MMPF_SYS

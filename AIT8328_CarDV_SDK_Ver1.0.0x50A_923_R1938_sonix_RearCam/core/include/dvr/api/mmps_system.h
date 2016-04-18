/**
 *  @file mmps_system.h
 *  @brief Header file the host system API
 *  @author Jerry Tsao, Truman Yang
 *  @version 1.1
 */
#ifndef _MMPS_SYSTEM_H_
#define _MMPS_SYSTEM_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "ait_bsp.h"
#include "config_fw.h"
#include "mmp_gpio_inc.h"
#include "mmpd_system.h"

/** @addtogroup MMPS_System
@{
*/

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

typedef enum _MMPS_SYSTEM_OPMODE
{
    MMPS_SYSTEM_OPMODE_BYPASS = 0,		/**< System Bypass mode, LCD bus is bypassed and controlled by B/B, 
    										Firmware is active. */
    MMPS_SYSTEM_OPMODE_ACTIVE,			/**< System active mode, LCD bus is controlled by AIT chip, Firmware is active */
    MMPS_SYSTEM_OPMODE_SUSPEND,			/**< System suspend mode, LCD bus is bypassed and controlled by B/B,
    										Firmware is sleeping. */
    MMPS_SYSTEM_OPMODE_PWRDN,			/**< System power down. LCD bus is bypassed and controlled by B/B,
    										Firmware is released. */
    MMPS_SYSTEM_OPMODE_SELFSLEEP,       /**< Standalone System enter self sleep mode*/
    MMPS_SYSTEM_OPMODE_MAX
} MMPS_SYSTEM_OPMODE;

typedef enum _MMPS_SYSTEM_APMODE
{
    MMPS_SYSTEM_APMODE_NULL = 0, 			/**<Null parameter. It's used to reset the FirmwareHandler state
                                    		 	machine to download the firmware again.*/
    MMPS_SYSTEM_APMODE_DSC = 1,	 			///< DSC
    MMPS_SYSTEM_APMODE_MP3_PLAY, 			///< MP3 player
    MMPS_SYSTEM_APMODE_VIDEO_AMR_RECORD,	///< AMR video recorder
    MMPS_SYSTEM_APMODE_VIDEO_AAC_RECORD, 	///< AAC video recorder
    MMPS_SYSTEM_APMODE_VIDEO_AMR_PLAY, 		///< Hardware video with AMR video player
    MMPS_SYSTEM_APMODE_VIDEO_AAC_PLAY, 		///< Hardware video with AAC video player
    MMPS_SYSTEM_APMODE_H264_AMR_PLAY, 		///< Software H.264 with AMR video player
    MMPS_SYSTEM_APMODE_H264_AAC_PLAY, 		///< Software H.264 with AAC video player
    MMPS_SYSTEM_APMODE_VIDEO_AACPLUS_PLAY, 	///< Not supported yet
    MMPS_SYSTEM_APMODE_VIDEO_BSAC_PLAY, 	///< Not supported yet
    MMPS_SYSTEM_APMODE_AMR, 				///< AMR player and recoder without video
    MMPS_SYSTEM_APMODE_AAC_RECORD, 			///< AAC player and recoder without video
    MMPS_SYSTEM_APMODE_MP3_RECORD, 			///< MP3 recoder without video
    MMPS_SYSTEM_APMODE_MIDI_PLAY, 			///< Midi player
    MMPS_SYSTEM_APMODE_AACPLUS_PLAY,		///< AAC+ player
    MMPS_SYSTEM_APMODE_WMA_PLAY, 			///< WMA player
    MMPS_SYSTEM_APMODE_OGG_PLAY, 			///< OGG Vorbis player
    MMPS_SYSTEM_APMODE_RA_PLAY, 			///< RA Vorbis player
    MMPS_SYSTEM_APMODE_WAV_PLAY, 			///< WAV player
    MMPS_SYSTEM_APMODE_USB, 				///< USB storage
    MMPS_SYSTEM_APMODE_MDTV, 				///< Mobile Digital TV
    MMPS_SYSTEM_APMODE_TEST, 				///< TEST firmware
    MMPS_SYSTEM_APMODE_ERASE,				///< Nand Flash Initialization
    MMPS_SYSTEM_APMODE_PLL, 				///< For change PLL
    MMPS_SYSTEM_APMODE_AVI_MP3_PLAY,		///< Hardware video with MP3 avi video player
    MMPS_SYSTEM_APMODE_MAX 					/**< It could be used to debug different versions of system mode definition.*/
} MMPS_SYSTEM_APMODE;

typedef enum _MMPS_DRAM_TYPE
{
    MMPS_DRAM_TYPE_NONE = 0,	// no stack memory
    MMPS_DRAM_TYPE_1,			// first used
    MMPS_DRAM_TYPE_2,			// second used
    MMPS_DRAM_TYPE_3,			// third used
    MMPS_DRAM_TYPE_EXT,
    MMPS_DRAM_TYPE_AUTO
} MMPS_DRAM_TYPE;

typedef enum _MMPS_DRAM_MODE
{
    MMPS_DRAM_MODE_SDRAM = 0,	// SD RAM
    MMPS_DRAM_MODE_DDR,			// DDR RAM
    MMPS_DRAM_MODE_DDR2,
    MMPS_DRAM_MODE_DDR3,
    MMPS_DRAM_MODE_NUM
} MMPS_DRAM_MODE;

typedef enum _MMPS_SYSTEM_MEM_DIR
{
    MMPS_SYSTEM_MEM_TOP = 0,           //#define MEM_ALLOC_FROM_TOP
    MMPS_SYSTEM_MEM_BOTTOM             //#define MEM_ALLOC_FROM_BOTTOM 
} MMPS_SYSTEM_MEM_DIR;

typedef enum
{
    MMPS_SYSTEM_WAKEUP_NONE = 0x0,
    MMPS_SYSTEM_WAKEUP_GPIO,
    MMPS_SYSTEM_WAKEUP_TVPLUG_IN,
    MMPS_SYSTEM_WAKEUP_USB_RESUME,
    MMPS_SYSTEM_WAKEUP_HDMI_HPD,
    MMPS_SYSTEM_WAKEUP_RTC_CNT_VALID,
    MMPS_SYSTEM_WAKEUP_RTC_VALID,
    MMPS_SYSTEM_WAKEUP_RTC_INT,
    MMPS_SYSTEM_WAKEUP_MAX
}MMPS_SYSTEM_WAKEUP_EVNET;

//==============================================================================
//
//                              STRUCTURE
//
//==============================================================================

/** @brief Type of by pass pin controller call back function.

The responsibility of this function should set the bypass ping according to the input
@param[in] value Value 1 for high, value 0 for low.
@return If the initalize successful
*/
typedef struct _MMPS_SYSTEM_CONFIG {
    MMPS_SYSTEM_APMODE  apMode;					///< System application mode
    MMPS_DRAM_TYPE		stackMemoryType;		///< Stack memory type inside AIT chip
    MMPS_DRAM_MODE      stackMemoryMode;    	///< DDR or SDRAM
	MMP_ULONG           ulStackMemoryStart;		///< Stack memory start address
	MMP_ULONG           ulStackMemorySize;		///< Stack memory size
    MMP_ULONG           ulMaxReserveBufferSize; ///< Reserve Buffer Size for Audio Operation
    MMP_ULONG           ulMaxReserveChannel; 	///< Reserve Buffer Size for Audio Operation
} MMPS_SYSTEM_CONFIG;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

MMPS_SYSTEM_CONFIG* MMPS_System_GetConfig(void);
MMP_UBYTE MMPS_System_ReadCoreID(void);
MMP_ERR MMPS_System_ResetSystem(void);
MMP_ERR MMPS_System_SetOperationMode(MMPS_SYSTEM_OPMODE opMode);

MMP_ERR MMPS_System_GetGroupSrcAndDiv(CLK_GRP ubGroup, MMP_ULONG* ulGroupSrc, MMP_UBYTE* ubPllDiv);
MMP_ERR MMPS_System_GetGroupFreq(CLK_GRP ubGroup, MMP_ULONG *ulGroupFreq);
MMP_ERR MMPS_System_SetAudioGroupFract(MMP_LONG ulFract);

MMP_ERR MMPS_System_AllocateFSBuffer(const MMPS_SYSTEM_APMODE apMode);
MMP_ERR MMPS_System_SendEchoCommand(void);
MMP_ERR MMPS_System_SetAudioFrameStart(MMP_ULONG *ulFBEndAddr);
MMP_ERR MMPS_System_GetAudioFrameStart(MMP_ULONG audio_id, MMP_ULONG *ulFBEndAddr);
MMP_ERR MMPS_System_GetPreviewFrameStart(MMP_ULONG *ulFBEndAddr);
MMP_ERR MMPS_System_SetWakeUpEvent(MMP_BOOL bEnable, MMPS_SYSTEM_WAKEUP_EVNET event, MMP_GPIO_PIN piopin, MMP_UBYTE polarity);

#if (ENABLE_AUTO_TEST == 1)
MMP_ERR MMPS_System_GetHostCmdStart(MMP_ULONG **cmd_start, MMP_ULONG **cmd_param_start);
#endif

/* Memory Pool Function */
void MMPS_System_SetMemHeapEnd(MMP_ULONG ulHeapEnd);
MMP_ULONG MMPS_System_GetMemHeapEnd(void);

MMP_ERR MMPS_System_CreatePool(MMP_BYTE *bPoolName, MMP_ULONG ulSize, MMP_ULONG ulAddr, void *pHandler) ;
MMP_ERR MMPS_System_AllocateMemory(void *pHandler, MMP_ULONG *ulAllocBuf, MMP_BOOL bCacheAble, MMP_ULONG ulSize, MMPS_SYSTEM_MEM_DIR memDir, MMP_BOOL bAllocate, MMP_BYTE* szFilename, MMP_ULONG ulLineNumber);
MMP_ERR MMPS_System_GetPoolHandler(void **pHandler);
MMP_ERR MMPS_System_FlushAndInvalidDCache(void);

/// @}

#endif // _MMPS_SYSTEM_H_

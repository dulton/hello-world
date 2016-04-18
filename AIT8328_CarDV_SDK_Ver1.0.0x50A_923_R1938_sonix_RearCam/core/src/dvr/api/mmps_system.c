//==============================================================================
//
//  File        : mmps_system.c
//  Description : Ritian System Control function
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

/**
@file mmps_system.c
@brief The System Control functions
@author Penguin Torng
@version 1.0
*/

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "mmps_system.h"
#include "mmph_hif.h"
#include "mmp_reg_gbl.h"
#include "mmpd_audio.h"
#include "ait_utility.h"
#include "mmps_pio.h"

/** @addtogroup MMPS_System
@{
*/

//==============================================================================
//
//                              GLOBAL VARIABLES
//
//==============================================================================

/**@brief The system configuration

Use @ref MMPS_System_GetConfig to assign the field value of it.
You should read this functions for more information.
*/
static MMPS_SYSTEM_CONFIG	m_systemConfig;

/**@brief The system operation mode.
         
Use @ref MMPS_System_SetOperationMode to set it.
And use @ref MMPS_System_GetOperationMode to get the current mode.
You should read these functions for more information.
*/
static MMPS_SYSTEM_OPMODE	m_systemOpMode = MMPS_SYSTEM_OPMODE_ACTIVE;

/**@brief The system application mode.
         
Use @ref MMPS_System_SetApplicationMode to set it.
And use @ref MMPS_System_GetApplicationMode to get the current mode.
You should read these functions for more information.
*/
static MMPS_SYSTEM_APMODE	m_systemApMode = MMPS_SYSTEM_APMODE_NULL;

/**@brief Keep the host command start address, and the host command parameters address
         
Use @ref MMPS_System_SetHostCmdStart to set it, and use 
@ref MMPS_System_GetHostCmdStart to retrieve it
*/
#if (ENABLE_AUTO_TEST == 1)
static MMP_ULONG    m_ulHostCmdStart;
static MMP_ULONG    m_ulHostCmdParamStart;
#endif

/**@brief Keep the audio frame start address, and the preview frame start address
         
Use @ref MMPS_System_SetAudioFrameStart to set it, and use 
@ref MMPS_System_GetAudioFrameStart to retrieve it
*/
static MMP_ULONG	m_ulAudioFrameStart;
static MMP_ULONG	m_ulPreviewFrameStart;

/**@brief Firmware bootup status, set to TRUE when lower priority tasks is ready.
*/
MMP_BOOL            gbSysBootComplete = MMP_FALSE;

/**@brief Firmware memory heap end address
*/
static MMP_ULONG    m_ulSysHeapEnd = 0xFFFFFFFF;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

//------------------------------------------------------------------------------
//  Function    : MMPS_System_GetConfig
//  Description :
//------------------------------------------------------------------------------
/** @brief The function gets the current system configuration for the host application

The function gets the current system configuration for reference by the host application. The current
configuration can be accessed from output parameter pointer. The function calls
MMPD_System_GetConfiguration to get the current settings from Host Device Driver Interface.

@return It return the pointer of the system configuration data structure.
*/
MMPS_SYSTEM_CONFIG *MMPS_System_GetConfig(void)
{
    return &m_systemConfig;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_System_ReadCoreID
//  Description :
//------------------------------------------------------------------------------
/** @brief Check the chip code ID

This function Get the code ID of the chip
@return It reports the status of the operation.
*/
MMP_UBYTE MMPS_System_ReadCoreID(void)
{
	return MMPD_System_ReadCoreID();
}

//------------------------------------------------------------------------------
//  Function    : MMPS_System_SetOperationMode
//  Description :
//------------------------------------------------------------------------------
/** @brief The function sets possible system operation mode

The function sets possible system operation mode defined in MMPS_System_OpMode by calling
MMPD_System_SetOperationMode. Three operation modes can be supported Bypass, Active,
Power Down. (Bypass <--> Active <--> Power Down)

@param[in] systemopmode the system operation mode
@return It reports the status of the operation.
*/
MMP_ERR MMPS_System_SetOperationMode(MMPS_SYSTEM_OPMODE opMode)
{
	if (m_systemOpMode == MMPS_SYSTEM_OPMODE_ACTIVE) {
		 if (opMode == MMPS_SYSTEM_OPMODE_SELFSLEEP) {
			 MMPD_System_SetPSMode(MMPD_SYSTEM_ENTER_PS);
			 MMPD_System_SetSelfSleepMode();
			 MMPD_System_SetPSMode(MMPD_SYSTEM_EXIT_PS);
		 }
	}
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_System_SetWakeUpEvent
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to set wake-up event for self sleep mode.

The function is used to set wake-up event for self sleep mode.

@param[bEnable] bEnable : enable/disable the wake up event
@param[bEnable] event : wake up event that AIT suppot
@param[bEnable] piopin : used in GPIO wake up event, to select GPIO
@param[bEnable] polarity : used in GPIO wake up event, to set high level/low level wake up. 0 is for high level
@return It reports the status of the operation.
*/
MMP_ERR MMPS_System_SetWakeUpEvent(MMP_BOOL bEnable, MMPS_SYSTEM_WAKEUP_EVNET event, MMP_GPIO_PIN piopin, MMP_UBYTE polarity)
{
	return MMPD_System_SetWakeUpEvent(bEnable, event, piopin, polarity);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_System_ResetSystem
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used for system reset

The function is used for system reset
@return It reports the status of the operation.
*/
MMP_ERR MMPS_System_ResetSystem(void)
{
	return MMPD_System_ResetSystem();
}

//------------------------------------------------------------------------------
//  Function    : MMPS_System_SetHostCmdStart
//  Description :
//------------------------------------------------------------------------------
/** 
@brief  Keep host command and command parameters address
@return MMP_ERR_NONE
*/
#if (ENABLE_AUTO_TEST == 1)
MMP_ERR MMPS_System_SetHostCmdStart(MMP_ULONG cmdStart, MMP_ULONG cmdParamStart)
{
    m_ulHostCmdStart 	  = cmdStart;
    m_ulHostCmdParamStart = cmdParamStart;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_System_GetHostCmdStart
//  Description :
//------------------------------------------------------------------------------
/** 
@brief  Get host command and command parameters address
@return MMP_ERR_NONE
*/
MMP_ERR MMPS_System_GetHostCmdStart(MMP_ULONG **cmdStart, MMP_ULONG **cmdParamStart)
{
    *cmdStart 		= (MMP_ULONG *)m_ulHostCmdStart;
    *cmdParamStart 	= (MMP_ULONG *)m_ulHostCmdParamStart;

    return MMP_ERR_NONE;
}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_System_AllocateFSBuffer
//  Description :
//------------------------------------------------------------------------------
/** 
@brief  Application-mode Memory Allocation
@param[in] apMode : application mode
@return MMPS_SYSTEM_SUCCESS
*/
MMP_ERR MMPS_System_AllocateFSBuffer(const MMPS_SYSTEM_APMODE apMode)
{
#if ((!defined(MBOOT_FW))||(defined(FAT_BOOT)))

	#if (FS_INPUT_ENCODE == UCS2)
	MMP_BYTE	volume[32];
	#endif

    MMP_ULONG	buf_start, buf_size, cur_buf;
    MMP_ULONG   nameBufSize, paramBufSize, nandBufRequire, sfBufRequire, sfnandBufRequire;
	MMP_ULONG	maxFileCount, maxDirCount;

    buf_start = MMPS_System_GetConfig()->ulStackMemoryStart + 
                MMPS_System_GetConfig()->ulStackMemorySize;
    buf_size  = MMPS_System_GetConfig()->ulStackMemorySize;


	#if (ENABLE_AUTO_TEST == 1)
    // Buffer for host command and parameters
    cur_buf = buf_start;

    if (cur_buf - (((cur_buf - 0x100) >> 5) << 5) > 40)
        cur_buf = ((cur_buf - 0x100) >> 5) << 5;
    else
        cur_buf = ((cur_buf - 0x200) >> 5) << 5;

    MMPS_System_SetHostCmdStart((cur_buf >> 4) << 4, ((cur_buf >> 4) << 4) + 4);
    buf_start = cur_buf;
	#endif

    #if (FS_INPUT_ENCODE == UCS2)
    nameBufSize = 0x300;
    #elif (FS_INPUT_ENCODE == UTF8)
    nameBufSize = 0x200;
    #endif

    paramBufSize = 8*1024;

    cur_buf = buf_start;

	/* Allocate for Filename buffer */
   	cur_buf -= nameBufSize;  
   	cur_buf -= paramBufSize;
    MMPD_FS_SetFileNameDmaAddr(cur_buf, nameBufSize, cur_buf + nameBufSize, paramBufSize);

	/* Allocate for SD working buffer */
    cur_buf -= 512;
    MMPD_FS_SetSDTmpMemory(cur_buf, 512);

	#if (OMA_DRM_EN == 1)
	/* Allocate for DRM working buffer */
    cur_buf -= 16*12;
    MMPD_FS_SetDRMIVMemory(cur_buf, 16*12);
	#endif
	
	/* Allocate for SM working buffer */
	MMPD_FS_GetSMMEMAlloc(&nandBufRequire);
   	cur_buf -= nandBufRequire;
	MMPD_FS_SetSMMemory(cur_buf, nandBufRequire);
	
    /* Allocate for SN<SPI Nand Flash> working buffer */
	MMPD_FS_GetSNMEMAlloc(&sfnandBufRequire);
   	cur_buf -= sfnandBufRequire;
	MMPD_FS_SetSNMemory(cur_buf, sfnandBufRequire);	

	/* Allocate for SF working buffer */
	cur_buf -= 32; // reserve 32-byte for read device ID
	MMPD_FS_GetSFMEMAlloc(cur_buf, &sfBufRequire);
	
	if(sfBufRequire != 0){
		cur_buf -= sfBufRequire;
		MMPD_FS_SetSFMemory(cur_buf, sfBufRequire);
	}
	
	/* Allocate for File System/Audio buffer */
	MMPD_FS_GetMaxFileOpenSupport(&maxFileCount);
	MMPD_FS_GetMaxDirOpenSupport(&maxDirCount);	
	MMPD_FS_SetFATGlobalBufAddr(cur_buf - 8*1024 - (maxFileCount + maxDirCount)*1024, 8*1024);
    #if defined(ALL_FW)
    printc("file system memory alocate start: 0x%08x, End : 0x%08x\r\n", buf_start, (cur_buf - 
                                                   8*1024 - (maxFileCount + maxDirCount)*1024));
    #endif
	MMPS_FS_Initialize();

    cur_buf = cur_buf - 8*1024 - (maxFileCount + maxDirCount)*1024;
   	MMPS_System_SetAudioFrameStart(&cur_buf);
    MMPS_System_SetMemHeapEnd(cur_buf);

	gbSysBootComplete = MMP_TRUE;

	#if (FS_INPUT_ENCODE == UCS2)
	uniStrcpy(volume, (const char *)L"SD:\\");
	MMPS_FS_MountVolume(volume, uniStrlen((const short *)volume));
	uniStrcpy(volume, (const char *)L"SM:\\");
	MMPS_FS_MountVolume(volume, uniStrlen((const short *)volume));
	#elif (FS_INPUT_ENCODE == UTF8)
	#if USING_SD_IF
	MMPS_FS_MountVolume("SD:\\", STRLEN("SD:\\"));
	#endif
	#if USING_SD1_IF 
	MMPS_FS_MountVolume("SD1:\\", STRLEN("SD1:\\"));
	#endif
	#if USING_SF_IF
	MMPS_FS_MountVolume("SF:\\", STRLEN("SF:\\"));
	#endif
	#if USING_SM_IF
	MMPS_FS_MountVolume("SM:\\", STRLEN("SM:\\"));
	#endif
	#endif
#endif
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_System_SendEchoCommand
//  Description :
//------------------------------------------------------------------------------
/** @brief The function will check current host interface workable

@return The result of the current host command interface is work or not.
*/
MMP_ERR MMPS_System_SendEchoCommand(void)
{
	if (m_systemApMode != MMPS_SYSTEM_APMODE_NULL) {
		return MMPD_System_SendEchoCommand();
	}
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_System_GetPreviewFrameStart
//  Description :
//------------------------------------------------------------------------------
/** 
@brief  Return the frame end address
@return MMPS_SYSTEM_SUCCESS
*/
MMP_ERR MMPS_System_GetPreviewFrameStart(MMP_ULONG *ulFBAddr)
{
	*ulFBAddr = m_ulPreviewFrameStart;
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_System_SetAudioFrameStart
//  Description :
//------------------------------------------------------------------------------
/** 
@brief  Keep the frame end address
@return MMPS_SYSTEM_SUCCESS
*/
MMP_ERR	MMPS_System_SetAudioFrameStart(MMP_ULONG *ulFBEndAddr)
{
	m_ulAudioFrameStart = *ulFBEndAddr;
    *ulFBEndAddr        = *ulFBEndAddr -
                            (MMPS_System_GetConfig()->ulMaxReserveBufferSize *
                            MMPS_System_GetConfig()->ulMaxReserveChannel);
	m_ulPreviewFrameStart = *ulFBEndAddr;

    #if defined(ALL_FW)
    printc("Audio memory alocate start: 0x%X, End: 0x%X\r\n",
            *ulFBEndAddr, m_ulAudioFrameStart);
    #endif

	return	MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_System_GetAudioFrameStart
//  Description :
//------------------------------------------------------------------------------
/** 
@brief  Return the frame end address
@return MMPS_SYSTEM_SUCCESS
*/
MMP_ERR MMPS_System_GetAudioFrameStart(MMP_ULONG audio_id, MMP_ULONG *ulFBEndAddr)
{
	 *ulFBEndAddr = m_ulAudioFrameStart - (audio_id * MMPS_System_GetConfig()->ulMaxReserveBufferSize);
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_System_GetGroupSrcAndDiv
//  Description :
//------------------------------------------------------------------------------
/** @brief Get the group PLL Source.
@param[in]  ubGroup Select the group.
@param[out] ulGroupFreq The current group PLL Source.
@return Always return success.
*/
MMP_ERR MMPS_System_GetGroupSrcAndDiv(CLK_GRP ubGroup, MMP_ULONG* ulGroupSrc, MMP_UBYTE* ubPllDiv)
{
    return MMPD_System_GetGroupSrcAndDiv(ubGroup, ulGroupSrc, ubPllDiv);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_System_GetGroupFreq
//  Description :
//------------------------------------------------------------------------------
/** @brief Get the group freq.
@param[in]  ubGroup Select the group.
@param[out] ulGroupFreq The current group freq.
@return Always return success.
*/
MMP_ERR MMPS_System_GetGroupFreq(CLK_GRP ubGroup, MMP_ULONG *ulGroupFreq)
{
	return MMPD_System_GetGroupFreq(ubGroup, ulGroupFreq);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_System_SetAudioGroupFract
//  Description :
//------------------------------------------------------------------------------
/** @brief Set the fraction part of audio PLL. Each project and each
           sensor mode may have different frame rate inaccuracy.
           To achieve a native A/V sync, we allow application to specify the
           fraction part of audio PLL by this function.
@param[in]  ulFract Fraction part of audio PLL. Max: 0x3FFFF for MCR_V2
@return Always return success.
*/
MMP_ERR MMPS_System_SetAudioGroupFract(MMP_LONG ulFract)
{
	return MMPD_System_SetAudioGroupFract(ulFract);
}

#if 0
void ____MemPool_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPS_System_SetMemHeapEnd
//  Description :
//------------------------------------------------------------------------------
/** @brief Set the end address of memory heap, kernel feature should not
           allocate memory with address over than this end address.
@param[in]  ulHeapEnd The end address of heap.
@return It reports the status of the operation.
*/
void MMPS_System_SetMemHeapEnd(MMP_ULONG ulHeapEnd)
{
    m_ulSysHeapEnd = ulHeapEnd;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_System_GetMemHeapEnd
//  Description :
//------------------------------------------------------------------------------
/** @brief Get the end address of memory heap, kernel feature must check if the
           address of allocated memory is larger than this end address.
@return The end address of memory heap.
*/
MMP_ULONG MMPS_System_GetMemHeapEnd(void)
{
    return m_ulSysHeapEnd;
}

/// @}
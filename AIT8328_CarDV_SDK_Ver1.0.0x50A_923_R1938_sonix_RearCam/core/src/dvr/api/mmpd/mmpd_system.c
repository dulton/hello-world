/// @ait_only
//==============================================================================
//
//  File        : mmpd_system.c
//  Description : Ritian System Control Device Driver Function
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

/**
 *  @file mmpd_system.c
 *  @brief The header File for the Host System Control Device Driver Function
 *  @author Penguin Torng
 *  @version 1.0
 */

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "mmpd_system.h"
#include "mmph_hif.h"
#include "mmp_reg_gbl.h"
#include "mmpf_pll.h"
#include "ait_utility.h"
#include "mmpf_system.h"

/** @addtogroup MMPD_System
 *  @{
 */

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

//------------------------------------------------------------------------------
//  Function    : MMPD_System_ResetHModule
//  Description :
//------------------------------------------------------------------------------
/** @brief The function resets hardware module

The function resets hardware module
@param[in] moduletype the module
@param[in] bResetRegister MMP_TRUE: reset the registers and state of the module; MMP_FALSE: only reset the state of the module.
@return It reports the status of the operation.
*/
MMP_ERR MMPD_System_ResetHModule(MMPD_SYS_MDL moduletype, MMP_BOOL bResetRegister)
{
	return MMPF_SYS_ResetHModule((MMPF_SYS_MDL)moduletype, bResetRegister);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_System_ResetVariables
//  Description :
//------------------------------------------------------------------------------
/** @brief The function reset all file-scope variables to default value

The function reset all file-scope variables to default value
@return It reports the status of the operation.
*/
MMP_ERR MMPD_System_ResetVariables(void)
{
    return MMPF_SYS_ResetVariables();
}

//------------------------------------------------------------------------------
//  Function    : MMPD_System_ResetSystem
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used for system reset

The function is used for system reset
@return It reports the status of the operation.
*/
MMP_ERR MMPD_System_ResetSystem(void)
{
    MMPH_HIF_WaitSem(GRP_IDX_SYS, 0);
    MMPH_HIF_SendCmd(GRP_IDX_SYS, HIF_SYS_CMD_SYSTEM_RESET | SYSTEM_SOFTWARE_RESET);
    MMPH_HIF_ReleaseSem(GRP_IDX_SYS);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_System_SetWakeUpEvent
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
MMP_ERR MMPD_System_SetWakeUpEvent(MMP_BOOL bEnable, MMPD_SYSTEM_WAKEUP_EVNET event, MMP_GPIO_PIN piopin, MMP_UBYTE polarity)
{
	MMP_ERR	mmpstatus;

	MMPH_HIF_WaitSem(GRP_IDX_SYS, 0);
	
	MMPH_HIF_SetParameterL(GRP_IDX_SYS, 0, bEnable);
    MMPH_HIF_SetParameterL(GRP_IDX_SYS, 4, event);
    MMPH_HIF_SetParameterL(GRP_IDX_SYS, 8, piopin);
    MMPH_HIF_SetParameterL(GRP_IDX_SYS, 12, polarity);
    MMPH_HIF_SendCmd(GRP_IDX_SYS, HIF_SYS_CMD_SELF_SLEEP | SET_WAKE_EVENT);
    
    mmpstatus = MMPH_HIF_GetParameterL(GRP_IDX_SYS, 0);
    
	MMPH_HIF_ReleaseSem(GRP_IDX_SYS);

	return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_System_SetSelfSleepMode
//  Description :
//------------------------------------------------------------------------------
/** @brief The function sets standalone platform to enter self-sleep mode

The function sets standalone platform to enter self-sleep mode
@return It reports the status of the operation.
*/
MMP_ERR MMPD_System_SetSelfSleepMode(void)
{
    MMPH_HIF_WaitSem(GRP_IDX_SYS, 0);
	MMPH_HIF_SendCmd(GRP_IDX_SYS, HIF_SYS_CMD_SELF_SLEEP | SYSTEM_ENTER_SLEEP);
	MMPH_HIF_ReleaseSem(GRP_IDX_SYS);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_System_SetPSMode
//  Description :
//------------------------------------------------------------------------------
/** @brief The function sets the power saving mode

The function sets the power saving mode
@param[in] psmode the power saving mode
@return It reports the status of the operation.
*/
MMP_ERR MMPD_System_SetPSMode(MMPD_SYSTEM_PS_MODE psmode)
{
    if (psmode == MMPD_SYSTEM_ENTER_PS) {
    
       	MMPD_System_EnableClock(MMPD_SYS_CLK_AUD, MMP_TRUE);
       	MMPD_System_EnableClock(MMPD_SYS_CLK_ADC, MMP_TRUE);
       	MMPD_System_EnableClock(MMPD_SYS_CLK_DAC, MMP_TRUE);
        
        MMPH_HIF_WaitSem(GRP_IDX_SYS, 0);
        MMPH_HIF_SendCmd(GRP_IDX_SYS, HIF_SYS_CMD_SET_PS_MODE | ENTER_PS_MODE);
        MMPH_HIF_ReleaseSem(GRP_IDX_SYS);
    }
    else {

        MMPH_HIF_WaitSem(GRP_IDX_SYS, 0);
        MMPH_HIF_SendCmd(GRP_IDX_SYS, HIF_SYS_CMD_SET_PS_MODE | EXIT_PS_MODE);
        MMPH_HIF_ReleaseSem(GRP_IDX_SYS);
    }
    
    MMPC_System_WaitMs(100);
     
    if (psmode == MMPD_SYSTEM_ENTER_PS) {
    
       	MMPD_System_EnableClock(MMPD_SYS_CLK_ADC, MMP_FALSE);
       	MMPD_System_EnableClock(MMPD_SYS_CLK_DAC, MMP_FALSE);
    	MMPD_System_EnableClock(MMPD_SYS_CLK_AUD, MMP_FALSE);
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_System_SetBypassMode
//  Description :
//------------------------------------------------------------------------------
/** @brief The function enables or disables the bypass mode

The function enables or disables the bypass mode by sending host command request
@param[in] bypassmode enable or disable bypass mode
@return It reports the status of the operation.
*/
MMP_ERR MMPD_System_SetBypassMode(MMPD_SYSTEM_BYPASS_MODE byPassMode)
{
    MMPH_HIF_WaitSem(GRP_IDX_SYS, 0);
    
    if (byPassMode == MMPD_SYSTEM_ENTER_BYPASS) {
        MMPH_HIF_SendCmd(GRP_IDX_SYS, HIF_SYS_CMD_SET_BYPASS_MODE | ENTER_BYPASS_MODE);
    }
    else {
        MMPH_HIF_SendCmd(GRP_IDX_SYS, HIF_SYS_CMD_SET_BYPASS_MODE | EXIT_BYPASS_MODE);
    }
    
    MMPH_HIF_ReleaseSem(GRP_IDX_SYS);
    
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_System_GetFWEndAddr
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get the end address of download firmware.
 
 This API gets the end address of current downloaded firmware. It can optimize the
 memory usage without ldsfile setting.
 @param[out] ulAddress Start address of firmware available buffer.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_System_GetFWEndAddr(MMP_ULONG *ulAddress)
{
	MMP_ERR	mmpstatus = MMP_ERR_NONE;
    
	MMPH_HIF_WaitSem(GRP_IDX_SYS, 0);
	
	MMPH_HIF_SendCmd(GRP_IDX_SYS, HIF_SYS_CMD_GET_FW_ADDR | FW_END);
	
	*ulAddress = MMPH_HIF_GetParameterL(GRP_IDX_SYS, 0);
	
	MMPH_HIF_ReleaseSem(GRP_IDX_SYS);
    
	return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_System_GetSramEndAddr
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get the end address of the FW in SRAM region
 
 This API gets the end address of FW codes in SRAM region.
 @param[out] ulAddress End address of FW in SRAM region.
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_System_GetSramEndAddr(MMP_ULONG *ulAddress)
{
	MMPH_HIF_WaitSem(GRP_IDX_SYS, 0);
	
	MMPH_HIF_SendCmd(GRP_IDX_SYS, HIF_SYS_CMD_GET_FW_ADDR | FW_SRAM_END);
	
	*ulAddress = MMPH_HIF_GetParameterL(GRP_IDX_SYS, 0);
	
	MMPH_HIF_ReleaseSem(GRP_IDX_SYS);
    
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_System_GetAudioFWStartAddr
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Get the start address of the audio code
 
 This API gets the start address of audio codes.
 @param[in] ulRegion Region 1 or 2
 @param[out] ulAddress Start address of audio .
 @retval MMP_ERR_NONE Success.
*/
MMP_ERR MMPD_System_GetAudioFWStartAddr(MMP_ULONG ulRegion, MMP_ULONG *ulAddress)
{
	MMPH_HIF_WaitSem(GRP_IDX_SYS, 0);
	
	MMPH_HIF_SetParameterL(GRP_IDX_SYS, 0, ulRegion);
    MMPH_HIF_SendCmd(GRP_IDX_SYS, HIF_SYS_CMD_GET_FW_ADDR |  AUDIO_START);
    
    *ulAddress = MMPH_HIF_GetParameterL(GRP_IDX_SYS, 0);
    
    MMPH_HIF_ReleaseSem(GRP_IDX_SYS);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_System_ReadCoreID
//  Description :
//------------------------------------------------------------------------------
/** @brief Check the chip code ID

This function Get the code ID of the chip
@return It reports the status of the operation.
*/
MMP_UBYTE MMPD_System_ReadCoreID(void)
{
	return MMPF_SYS_ReadCoreID();
}

//------------------------------------------------------------------------------
//  Function    : MMPD_System_EnableClock
//  Description :
//------------------------------------------------------------------------------
/** @brief The function enables or disables the specified clock

The function enables or disables the specified clock from the clock type input by programming the
Global controller registers.

@param[in] ulClockType the clock type to be selected
@param[in] bEnableclock enable or disable the clock
@return It reports the status of the operation.
*/
MMP_ERR MMPD_System_EnableClock(MMPD_SYS_CLK clocktype, MMP_BOOL bEnable)
{
    return MMPF_SYS_EnableClock(clocktype, bEnable);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_System_TVInitialize
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_System_TVInitialize(MMP_BOOL bInit)
{
    MMP_ERR mmpstatus = MMP_ERR_NONE;
    
    MMPH_HIF_WaitSem(GRP_IDX_SYS, 0);
    
    if (bInit) {
	    MMPH_HIF_SendCmd(GRP_IDX_SYS, HIF_SYS_CMD_CONFIG_TV | TVENC_INIT);
    }
    else {
	    MMPH_HIF_SendCmd(GRP_IDX_SYS, HIF_SYS_CMD_CONFIG_TV | TVENC_UNINIT);
    }
    
    MMPH_HIF_ReleaseSem(GRP_IDX_SYS);

    return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_System_TVColorBar
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_System_TVColorBar(MMP_TV_TYPE tvType, MMP_BOOL turnOn, MMP_UBYTE colorBarType)
{
    MMP_ERR mmpstatus = MMP_ERR_NONE;
    
    MMPH_HIF_WaitSem(GRP_IDX_SYS, 0);
    
    MMPH_HIF_SetParameterL(GRP_IDX_SYS, 0, (MMP_ULONG) tvType);
    MMPH_HIF_SetParameterL(GRP_IDX_SYS, 4, (MMP_ULONG) turnOn);
    MMPH_HIF_SetParameterL(GRP_IDX_SYS, 8, (MMP_ULONG) colorBarType);
    MMPH_HIF_SendCmd(GRP_IDX_SYS, HIF_SYS_CMD_CONFIG_TV | TV_COLORBAR);
    
    MMPH_HIF_ReleaseSem(GRP_IDX_SYS);

    return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_System_ResetCPU
//  Description : 
//------------------------------------------------------------------------------
/** @brief Reset the CPU or let CPU free run.

The reason why seperate CPU reset from MMPD_System_ResetHModule is that the CPU
might not free run right away. Now it's only used in firmware downloading.
@param[in] bLock Lock CPU or not.
@return The status of the the function call.
*/
MMP_ERR MMPD_System_ResetCPU(MMP_BOOL bLock)
{
    #if (CHIP == P_V2)
    MMP_ULONG i;
    
    /* Do the software reset */
    if (bLock) {
        // Clear CPU boot status
        //MMPH_HIF_RegSetL(HOSTCMD_STS, (MMPH_HIF_CmdGetStatusL() & (~CPU_BOOT_FINISH)));
    
        // Reset the CPU and wait
        MMPH_HIF_RegSetB(GBL_RST_SW_EN, GBL_MODULE_CPU);
        
        // Patch for during download RGB LCD is working
		MMPD_System_ResetHModule(MMPD_SYS_MDL_MCI, MMP_FALSE);
		MMPC_System_WaitMmp(100);

		return MMP_ERR_NONE;
    } 
    else {
        // CPU free run
		MMPH_HIF_RegSetB(GBL_ARM_PWR_DOWN, GBL_CPU_WFI_CLKOFF_EN); 
        MMPH_HIF_RegSetB(GBL_RST_SW_DIS, GBL_MODULE_CPU);

        // Add CPU time out
        for (i = 0; i < 0x10000; i++) {
            if ((MMPH_HIF_CmdGetStatusL() & CPU_BOOT_FINISH)) {
                PRINTF("OK\r\n");
               	return MMP_ERR_NONE;
            }
        }
        PRINTF("Failed\r\n");
    }
    #endif
    return MMP_SYSTEM_ERR_CPUBOOT;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_System_CheckVersion
//  Description :
//------------------------------------------------------------------------------
/** @brief Check the version of the firmware and the MMPD library.

This function Get the compile-made version from the firmware and this function.
Then it displayed on the screen if possible.
@pre This function should be called after PLL setting so that the firmware could output the FW name correctly.
@return It reports the status of the operation. Now it always return success.
*/
MMP_ERR MMPD_System_CheckVersion(void)
{
    MMP_USHORT i;
    MMPD_SYSTEM_BUILD_VERSION buildVersion;

	MMPH_HIF_WaitSem(GRP_IDX_SYS, 0);
	
	MMPH_HIF_SendCmd(GRP_IDX_SYS, HIF_SYS_CMD_VERSION_CONTROL | FW_RELEASE_TIME);
	
	for (i = 0; i <= COMPILER_TIME_LEN; i++) {
        buildVersion.szTime[i] = MMPH_HIF_GetParameterB(GRP_IDX_SYS, i);
    }
    
    MMPH_HIF_SendCmd(GRP_IDX_SYS, HIF_SYS_CMD_VERSION_CONTROL | FW_RELEASE_DATE);
    
    for (i = 0; i <= COMPILER_DATE_LEN; i++) {
        buildVersion.szDate[i] = MMPH_HIF_GetParameterB(GRP_IDX_SYS, i);
    }
    
	MMPH_HIF_ReleaseSem(GRP_IDX_SYS);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_System_SendEchoCommand
//  Description :
//------------------------------------------------------------------------------
/** @brief Check the command interrupt still workable.

This function Get the command data with host parameter0 and can use to check HOST command interface.
@pre This function should be called after download firmware success.
@return It reports the status of the operation.
*/
MMP_ERR MMPD_System_SendEchoCommand(void)
{
	MMPH_HIF_WaitSem(GRP_IDX_SYS, 0);
	MMPH_HIF_SendCmd(GRP_IDX_SYS, HIF_SYS_CMD_ECHO | GET_INTERNAL_STATUS);
	
	if (MMPH_HIF_GetParameterW(GRP_IDX_SYS, 0) != (HIF_SYS_CMD_ECHO | GET_INTERNAL_STATUS)) {
		PRINTF("ECHO fail\r\n");
		MMPH_HIF_ReleaseSem(GRP_IDX_SYS);
		return MMP_SYSTEM_ERR_HW;	
	}
	MMPH_HIF_ReleaseSem(GRP_IDX_SYS);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_System_GetGroupSrcAndDiv
//  Description :
//------------------------------------------------------------------------------
/** @brief Get the group PLL Source.
@param[in]  ubGroup Select the group.
@param[out] ulGroupFreq The current group PLL Source.
@return Always return success.
*/
MMP_ERR MMPD_System_GetGroupSrcAndDiv(CLK_GRP ubGroup, MMP_ULONG *ulGroupSrc, MMP_UBYTE *ubPllDiv)
{
	MMP_ERR	mmpstatus = MMP_ERR_NONE;
    
    MMPH_HIF_WaitSem(GRP_IDX_SYS, 0);
    
    MMPH_HIF_SetParameterL(GRP_IDX_SYS, 0, ubGroup);
	MMPH_HIF_SendCmd(GRP_IDX_SYS, HIF_SYS_CMD_GET_GROUP_SRC_AND_DIV);
	
	*ulGroupSrc = MMPH_HIF_GetParameterL(GRP_IDX_SYS, 4);
	*ubPllDiv = MMPH_HIF_GetParameterL(GRP_IDX_SYS, 8);
	
    MMPH_HIF_ReleaseSem(GRP_IDX_SYS);

    return mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_System_GetGroupFreq
//  Description :
//------------------------------------------------------------------------------
/** @brief Get the group freq.
@param[in]  ubGroup Select the group.
@param[out] ulGroupFreq The current group freq.
@return Always return success.
*/
MMP_ERR MMPD_System_GetGroupFreq(CLK_GRP ubGroup, MMP_ULONG *ulGroupFreq)
{
	MMP_ERR	mmpstatus = MMP_ERR_NONE;
    
    MMPH_HIF_WaitSem(GRP_IDX_SYS, 0);
    
    MMPH_HIF_SetParameterL(GRP_IDX_SYS, 0, ubGroup);
	MMPH_HIF_SendCmd(GRP_IDX_SYS, HIF_SYS_CMD_GET_GROUP_FREQ);
	
	*ulGroupFreq = MMPH_HIF_GetParameterL(GRP_IDX_SYS, 4);
	
    MMPH_HIF_ReleaseSem(GRP_IDX_SYS);

    return  mmpstatus;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_System_SetAudioGroupFract
//  Description :
//------------------------------------------------------------------------------
/** @brief Set the fraction part of audio PLL. Each project and each
           sensor mode may have different frame rate inaccuracy.
           To achieve a native A/V sync, we allow application to specify the
           fraction part of audio PLL by this function.
@param[in]  ulFract Fraction part of audio PLL. Max: 0x3FFFF for MCR_V2
@return Always return success.
*/
MMP_ERR MMPD_System_SetAudioGroupFract(MMP_LONG ulFract)
{
    MMP_ERR	mmpstatus = MMP_ERR_NONE;

    MMPH_HIF_WaitSem(GRP_IDX_SYS, 0);

    MMPH_HIF_SetParameterL(GRP_IDX_SYS, 0, ulFract);
    mmpstatus = MMPH_HIF_SendCmd(GRP_IDX_SYS, HIF_SYS_CMD_SET_AUD_FRACT);

    MMPH_HIF_ReleaseSem(GRP_IDX_SYS);

    return mmpstatus;
}

/// @}
/// @end_ait_only


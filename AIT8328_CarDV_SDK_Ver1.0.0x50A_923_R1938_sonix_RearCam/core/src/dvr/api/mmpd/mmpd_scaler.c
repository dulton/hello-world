/// @ait_only
//==============================================================================
//
//  File        : mmpd_scaler.c
//  Description : Ritian Scaler Module Control driver function
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "mmpd_scaler.h"
#include "mmpd_system.h"
#include "mmpf_scaler.h"

/** @addtogroup MMPD_Scaler
 *  @{
 */

//==============================================================================
//
//                              GLOBAL VARIABLES
//
//==============================================================================

static MMP_SCAL_SETTING m_scalersetting[MMP_SCAL_PIPE_NUM];

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

#if 0
void ____Scaler_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_ResetModule
//  Description : This function reset scaler module.
//------------------------------------------------------------------------------
MMP_ERR MMPD_Scaler_ResetModule(MMP_SCAL_PIPEID pipeID)
{
	return MMPF_Scaler_ResetModule(pipeID);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_SetStopEnable
//  Description :
//------------------------------------------------------------------------------
/** @brief The function configures the parameters of stop attribute.

The function configures the parameters of stop attribute.
@param[in] pipeID 		the scaling path in Scaler
@param[in] ubStopSrc 	the stop source module
@param[in] bEn 			the enable or not
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Scaler_SetStopEnable(MMP_SCAL_PIPEID pipeID, MMP_UBYTE ubStopSrc, MMP_BOOL bEnable)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);

    MMPF_Scaler_SetStopEnable(pipeID, ubStopSrc, bEnable);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_SetBusyMode
//  Description :
//------------------------------------------------------------------------------
/** @brief The function configures the parameters of busy mode attribute.

The function configures the parameters of busy mode attribute.
@param[in] pipeID 		the scaling path in Scaler
@param[in] bEn 			the enable or not
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Scaler_SetBusyMode(MMP_SCAL_PIPEID pipeID, MMP_BOOL bEnable)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);

    MMPF_Scaler_SetBusyMode(pipeID, bEnable);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);
    
    return MMP_ERR_NONE;
}
//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_GetMaxDelayLineWidth
//  Description :
//------------------------------------------------------------------------------
/** 
 * @brief This function get max delay line width.
 * 
 *  This function get max delay line width.
 * @param[in] pipeID   : stands for scaler path index. 
 * @return It return the function status.  
 */
MMP_ULONG MMPD_Scaler_GetMaxDelayLineWidth(MMP_SCAL_PIPEID pipeID)
{
    return MMPF_Scaler_GetMaxDelayLineWidth(pipeID);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_BypassScaler
//  Description :
//------------------------------------------------------------------------------
/** @brief The function configures the parameters used by Scaler Engine

The function configures the parameters used by Scaler Engine.
@param[in] bUserdefine the flag that user deine the fit range or calaulated by system
@param[in] pipeID the scaling path in Scaler
@param[in] fitrange the fit range parameters
@param[in] grabctl the grab control parameters
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Scaler_BypassScaler(MMP_SCAL_PIPEID pipeID, MMP_BOOL bBypass)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);

    MMPF_Scaler_BypassScaler(pipeID, bBypass);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_SetEngine
//  Description :
//------------------------------------------------------------------------------
/** @brief The function configures the parameters used by Scaler Engine

The function configures the parameters used by Scaler Engine.
@param[in] bUserdefine the flag that user deine the fit range or calaulated by system
@param[in] pipeID the scaling path in Scaler
@param[in] fitrange the fit range parameters
@param[in] grabctl the grab control parameters
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Scaler_SetEngine(	MMP_SCAL_USER_DEF_TYPE	bUserdefine,
								MMP_SCAL_PIPEID			pipeID,
								MMP_SCAL_FIT_RANGE		*fitrange, 
								MMP_SCAL_GRAB_CTRL		*grabctl)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);

    MMPF_Scaler_SetEngine(bUserdefine,
                          pipeID,
                          fitrange,
                          grabctl);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_SetLPF
//  Description : 
//------------------------------------------------------------------------------
/** @brief The function set the LPF coefficient according to the grab control
		
The function calculates the best grab parameter..
@param[in] pipeID the scaling path in Scaler
@param[in] fitrange the fit range parameters
@param[in] grabctl the grab control parameters
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Scaler_SetLPF( MMP_SCAL_PIPEID		pipeID,
                            MMP_SCAL_FIT_RANGE *fitrange,
                            MMP_SCAL_GRAB_CTRL *grabctl)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);

    MMPF_Scaler_SetLPF(pipeID, fitrange, grabctl);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_SetOutColor
//  Description :
//------------------------------------------------------------------------------
/** @brief The function set scaler output color format
The function set scaler output color format

@param[in] path : path 0 or path 1
@param[in] outcolor : out color format
@return MMP_ERR_NONE
*/
MMP_ERR MMPD_Scaler_SetOutColor(MMP_SCAL_PIPEID pipeID, MMP_SCAL_COLORMODE outcolor)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);

    MMPF_Scaler_SetOutputFormat(pipeID, outcolor);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_SetOutColorTransform
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_Scaler_SetOutColorTransform(MMP_SCAL_PIPEID pipeID, MMP_BOOL bEnable, MMP_SCAL_COLRMTX_MODE MatrixMode)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);

    MMPF_Scaler_SetOutColorTransform(pipeID, bEnable, MatrixMode);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_SetEnable
//  Description : 
//------------------------------------------------------------------------------
/** @brief The function enables or disables an icon on the display device

The function enables or disables icon with the specified icon ID on the display device. It is implemented
by programming Icon Controller registers.

  @param[in] usPipeID the IBC ID
  @param[in] bEnable enable or disable IBC
  @return It reports the status of the operation.
*/
MMP_ERR MMPD_Scaler_SetEnable(MMP_SCAL_PIPEID pipeID, MMP_BOOL bEnable)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);
 
    MMPF_Scaler_SetEnable(pipeID, bEnable); 
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_GetEnable
//  Description : 
//------------------------------------------------------------------------------
/** @brief The function gets the specified scaler pipe is enabled or disabled

The function check with the specified scaler ID status. It is implemented
by programming scaler engine registers.

  @param[in] usPipeID the scaler ID
  @param[out] bEnable get scaler enable or disable
  @return It reports the status of the operation.
*/
MMP_ERR MMPD_Scaler_GetEnable(MMP_SCAL_PIPEID pipeID, MMP_BOOL *bEnable)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);
    
    MMPF_Scaler_GetEnable(pipeID, bEnable); 
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

	return MMP_ERR_NONE;	
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_SetPixelDelay
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Scaler_SetPixelDelay(MMP_SCAL_PIPEID pipeID, MMP_UBYTE ubPixelDelayN, MMP_UBYTE ubPixelDelayM)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);

    MMPF_Scaler_SetPixelDelay(pipeID,
                              ubPixelDelayN,
                              ubPixelDelayM);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_SetLineDelay
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Scaler_SetLineDelay(MMP_SCAL_PIPEID pipeID, MMP_UBYTE ubLineDelay)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);

    MMPF_Scaler_SetLineDelay(pipeID, ubLineDelay);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_GetPath
//  Description : 
//------------------------------------------------------------------------------
/** @brief The function gets the scaler source path
		
The function gets the source path of the specified scaler pipe
@param[in] pipeID the scaling path in Scaler
@return The source of scaler pipe
*/
MMP_ERR MMPD_Scaler_GetPath(MMP_SCAL_PIPEID		pipeID, 
                            MMP_SCAL_SOURCE		*source)
{
    *source = MMP_SCAL_SOURCE_MAX;

    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);
    
    if (source) {
        MMPF_Scaler_GetInputSrc(pipeID, source);
    }

    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_SetPath
//  Description : 
//------------------------------------------------------------------------------
/** @brief The function set the LPF coefficient according to the grab control
		
The function calculates the best grab parameter..
@param[in] pipeID the scaling path in Scaler
@param[in] grabctl the grab control parameters
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Scaler_SetPath(MMP_SCAL_PIPEID		pipeID, 
                            MMP_SCAL_SOURCE		source, 
							MMP_BOOL			bFrameSync)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);
    
    if (bFrameSync == MMP_FALSE) {
        MMPF_Scaler_SetInputSrc(pipeID,
                                source,
                                MMP_TRUE);
    }
    else {
        MMPF_Scaler_SetPath(pipeID, source);
    }
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_SetSerialLinkPipe
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Scaler_SetSerialLinkPipe(MMP_SCAL_PIPEID srcPath, MMP_SCAL_PIPEID dstPath)
{
    MMP_ERR err;

    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);

    err = MMPF_Scaler_SetSerialLinkPipe(srcPath, dstPath);

    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_SetGrabPosition
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Scaler_SetGrabPosition(MMP_SCAL_PIPEID 		pipeID,
									MMP_SCAL_GRAB_STAGE 	grabstage,
									MMP_USHORT 				usHStartPos, 
									MMP_USHORT 				usHEndPos, 
									MMP_USHORT 				usVStartPos, 
									MMP_USHORT 				usVEndPos)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);

    MMPF_Scaler_SetGrabPosition(pipeID,
                                grabstage,
                                usHStartPos,
                                usHEndPos,
                                usVStartPos,
                                usVEndPos);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_GetGrabPosition
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Scaler_GetGrabPosition(MMP_SCAL_PIPEID pipeID, MMP_SCAL_GRAB_STAGE 	grabstage,
									MMP_USHORT 		*pusHStartPos, MMP_USHORT 		*pusHEndPos, 
									MMP_USHORT 		*pusVStartPos, MMP_USHORT 		*pusVEndPos)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);

    MMPF_Scaler_GetGrabPosition(pipeID,
                                grabstage,
                                pusHStartPos, pusHEndPos,
                                pusVStartPos, pusVEndPos);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_GetGrabRange
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Scaler_GetGrabRange(MMP_SCAL_PIPEID 		pipeID,
								 MMP_SCAL_GRAB_STAGE 	grabstage,
								 MMP_USHORT 			*pusHrange, 
								 MMP_USHORT 			*pusVrange)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);

    MMPF_Scaler_GetGrabRange(pipeID,
                             grabstage,
                             pusHrange, pusVrange);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_CheckLPFAbility
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Scaler_CheckLPFAbility(MMP_SCAL_PIPEID pipeID, MMP_SCAL_LPF_ABILITY *pAbility)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);

    MMPF_Scaler_CheckLPFAbility(pipeID, pAbility);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_GetLPFEnable
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Scaler_GetLPFEnable(MMP_SCAL_PIPEID pipeID, MMP_BOOL *pbEnable)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);
    
    MMPF_Scaler_GetLPFEnable(pipeID, pbEnable);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_GetLPFDownsample
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Scaler_GetLPFDownsample(MMP_SCAL_PIPEID pipeID, MMP_SCAL_LPF_DNSAMP *pDownsample)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);
    
    MMPF_Scaler_GetLPFDownSample(pipeID, pDownsample);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_GetScalingRatio
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Scaler_GetScalingRatio(MMP_SCAL_PIPEID 	pipeID, 
									MMP_USHORT 			*pusHN,
									MMP_USHORT 			*pusHM,
									MMP_USHORT 			*pusVN,
									MMP_USHORT 			*pusVM)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);

    MMPF_Scaler_GetScalingRatio(pipeID,
                                pusHN, pusHM, pusVN, pusVM);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_BackupAttributes
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Scaler_BackupAttributes(MMP_SCAL_PIPEID pipeID)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);
    
    MMPF_Scaler_GetAttributes(pipeID, &m_scalersetting[pipeID]);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_RestoreAttributes
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Scaler_RestoreAttributes(MMP_SCAL_PIPEID pipeID)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);
    
    MMPF_Scaler_SetAttributes(pipeID, &m_scalersetting[pipeID]);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);
	
	return MMP_ERR_NONE;
}

#if 0
void ____Flow_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_GetScaleUpHBlanking
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_Scaler_GetScaleUpHBlanking(MMP_SCAL_PIPEID pipeID, MMP_ULONG64 *pHBlanking)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);

    MMPF_Scaler_GetScaleUpHBlanking(pipeID, pHBlanking);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_GetGCDBestFitScale
//  Description :
//------------------------------------------------------------------------------
/** @brief The function calculates the best grab parameter.

The function calculates the best grab parameter..
@param[in] fitrange the fit range parameters
@param[out] grabctl the grab control parameters
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Scaler_GetGCDBestFitScale(MMP_SCAL_FIT_RANGE *fitrange, MMP_SCAL_GRAB_CTRL *grabctl)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);

    MMPF_Scaler_GetGCDBestFitScale(fitrange, grabctl);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_GetBestFitScale
//  Description :
//------------------------------------------------------------------------------
/** @brief The function calculates the best grab parameter.

The function calculates the best grab parameter..
@param[in] fitrange the fit range parameters
@param[out] grabctl the grab control parameters
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Scaler_GetBestFitScale(MMP_SCAL_FIT_RANGE *fitrange, MMP_SCAL_GRAB_CTRL *grabctl)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);

    MMPF_Scaler_GetBestOutGrabRange(fitrange, grabctl);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Scaler_TransferCoordinate
//  Description :
//------------------------------------------------------------------------------
/** @brief The function transfers the coordination from scaler output to scaler input

The function transfers the coordination in scaler ouput to a corresponding
coordination in scaler input.

@param[in] pipeID specify the scaler pipe path
@param[in] us3rdGrabXposition the X pos in scaler ouput frame
@param[in] us3rdGrabYposition the Y pos in scaler ouput frame
@param[out] us1stGrabXposition the transferred X pos in scaler input frame
@param[out] us1stGrabYposition the transferred Y pos in scaler input frame
@return It reports the status of the operation.
*/
MMP_ERR MMPD_Scaler_TransferCoordinate(MMP_SCAL_PIPEID 	pipeID,
                                       MMP_USHORT       usOutGrabXpos, 
                                       MMP_USHORT       usOutGrabYpos, 
                                       MMP_USHORT       *pus1stGrabXpos, 
                                       MMP_USHORT       *pus1stGrabYpos)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);

    MMPF_Scaler_TransferCoordinate(pipeID, 
                                   usOutGrabXpos,
                                   usOutGrabYpos,
                                   pus1stGrabXpos, 
                                   pus1stGrabYpos);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

    return MMP_ERR_NONE;
}

/// @}
/// @end_ait_only

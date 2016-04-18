//==============================================================================
//
//  File        : mmps_sensor.c
//  Description : Ritian Sensor Control driver function
//  Author      : Alan Wu
//  Revision    : 1.0
//
//==============================================================================

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "ait_utility.h"
#include "mmps_sensor.h"
#include "mmpd_system.h"

/** @addtogroup MMPS_SENSOR
@{
*/

//==============================================================================
//
//                              GLOBAL VARIABLES
//
//==============================================================================

/**@brief The current sensor mode.

Use @ref MMPS_Sensor_Initialize to set it.
*/
static  MMP_SNR_MODE    m_CurSensorMode[SENSOR_MAX_NUM];

/**@brief The current preview mode.

Use @ref MMPS_Sensor_Initialize to set it.
*/
static MMP_UBYTE        m_ubCurPreviewMode[SENSOR_MAX_NUM] = {0, 0};
static MMP_UBYTE        m_ubCurCaptureMode[SENSOR_MAX_NUM] = {0, 0};

/**@brief Sensor initialized or not.

Use @ref MMPS_Sensor_Initialize to set it.
*/
static MMP_BOOL         m_bSensorInit[SENSOR_MAX_NUM] = {MMP_FALSE, MMP_FALSE};

static MMP_ULONG        m_ulSensorMemEnd = 0;
extern MMP_USHORT       gsAhcCurrentSensor;
extern MMP_UBYTE 	    m_ubPreviewDelayCount[];
//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_InitPresetMode
//  Description :
//------------------------------------------------------------------------------
/** @brief Initialize Sensor Mode

The function initializes Sensor Mode.

@param[in] ubSnrSel the Sensor ID (0, 1, .....)
@param[in] ubPreviewMode Sensor preview mode, choose the input resolution of the sensor
@return It reports the status of the initialization result.
*/
static MMP_ERR MMPS_Sensor_InitPresetMode(MMP_UBYTE ubSnrSel, MMP_UBYTE ubPreviewMode)
{
    m_ubCurPreviewMode[ubSnrSel]    = ubPreviewMode;
    m_bSensorInit[ubSnrSel]	        = MMP_TRUE;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_SetMode
//  Description :
//------------------------------------------------------------------------------
static MMP_ERR MMPS_Sensor_SetMode(MMP_UBYTE ubSnrSel, MMP_SNR_MODE sSnrMode)
{
	MMP_ERR     error = MMP_ERR_NONE;
    MMP_ULONG   ulSize;
    MMP_ULONG   stackaddr;

	m_CurSensorMode[ubSnrSel] = sSnrMode;

    if (MMP_SNR_VID_PRW_MODE == sSnrMode) 
    {
        MMPD_System_GetFWEndAddr(&stackaddr);
	    MMPD_Sensor_AllocateBuffer(stackaddr, &ulSize);

	    m_ulSensorMemEnd = ALIGN32(stackaddr + ulSize);
	}

	error = MMPD_Sensor_SetMode(ubSnrSel, sSnrMode);

	return error;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_Initialize
//  Description :
//------------------------------------------------------------------------------
/** @brief Initialize Sensor

The function initializes Sensor.

@param[in] ubSnrSel the Sensor ID (0, 1, .....)
@param[in] ubPreviewMode Sensor preview mode, choose the input resolution of the sensor
@param[in] sensormode Sensor preview mode, choose the camera or video preview mode

@return It reports the status of the initialization result.
*/
MMP_ERR MMPS_Sensor_Initialize(MMP_UBYTE ubSnrSel, MMP_UBYTE ubPreviewMode, MMP_SNR_MODE sSnrMode)
{
    MMP_ERR error;

    error = MMPS_Sensor_InitPresetMode(ubSnrSel, ubPreviewMode);
    error = MMPS_Sensor_SetMode(ubSnrSel, sSnrMode);
    
    if (error != MMP_ERR_NONE) {
        PRINTF("Sensor Initial Set Mode Failed!\r\n");
        return error;
    }

    if (ubPreviewMode >= MMPS_Sensor_GetPreviewResNum(ubSnrSel)) {
        error =	MMPD_Sensor_Initialize(ubSnrSel, 
                                       MMPS_Sensor_GetDefPreviewMode(ubSnrSel));
    }
    else {
        error =	MMPD_Sensor_Initialize(ubSnrSel,
                                       ubPreviewMode);
    }

    return error;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_IsInitialized
//  Description :
//------------------------------------------------------------------------------
/** @brief Check if sensor initialized

The function check sensor initialize status.

@param[in] ubSnrSel the Sensor ID (0, 1, .....)

@return It reports the status of the sensor initialized or not.
*/
MMP_BOOL MMPS_Sensor_IsInitialized(MMP_UBYTE ubSnrSel)
{
    return m_bSensorInit[ubSnrSel];
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_ChangePreviewMode
//  Description :
//------------------------------------------------------------------------------
/** @brief Change the sensor preview mode

The function can be called after sensor is initialized to change the sensor preview mode.

@param[in] ubPreviewMode Sensor preview mode, choose the input resolution of the sensor
@param[in] ubWaitCount Sensor phase count

@return It reports the status of the change mode status
*/
MMP_ERR MMPS_Sensor_ChangePreviewMode(MMP_UBYTE ubSnrSel, MMP_UBYTE ubPreviewMode, MMP_UBYTE ubWaitCount)
{
    m_ubCurPreviewMode[ubSnrSel] = ubPreviewMode;

    return MMPD_Sensor_ChangePreviewMode(ubSnrSel, ubPreviewMode, ubWaitCount);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_SetPreviewMode
//  Description :
//------------------------------------------------------------------------------
/** @brief Change the sensor preview mode

The function can be called after sensor is initialized to change the sensor preview mode.

@param[in] ubPreviewMode Sensor preview mode, choose the input resolution of the sensor
@param[in] ubPhaseCount Sensor phase count

@return It reports the status of the change mode status
*/
MMP_ERR MMPS_Sensor_SetPreviewMode(MMP_UBYTE ubSnrSel, MMP_UBYTE ubPreviewMode)
{
    m_ubCurPreviewMode[ubSnrSel] = ubPreviewMode;

    return MMPD_Sensor_SetPreviewMode(ubSnrSel, ubPreviewMode);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_SetCaptureMode
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_Sensor_SetCaptureMode(MMP_UBYTE ubSnrSel, MMP_UBYTE ubCaptureMode)
{
    m_ubCurCaptureMode[ubSnrSel] = ubCaptureMode;

    return MMPD_Sensor_SetCaptureMode(ubSnrSel, ubCaptureMode);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_GetCurPrevScalInputRes
//  Description :
//------------------------------------------------------------------------------
/** @brief The function retrives current sensor output resolution

The function retrives current sensor output resolution
@param[out] usWidth  the width
@param[out] usHeight  the height
@retval MMP_ERR_NONE success
*/
MMP_ERR MMPS_Sensor_GetCurPrevScalInputRes(MMP_UBYTE ubSnrSel, MMP_ULONG *ulWidth, MMP_ULONG *ulHeight)
{
    MMP_ERR   err;
    MMP_ULONG ulMode, ulParam[2];

    ulMode = m_ubCurPreviewMode[ubSnrSel];

    err = MMPD_Sensor_GetDriverParam(ubSnrSel, MMP_SNR_DRV_PARAM_SCAL_IN_RES,
                                     1, &ulMode, 2, ulParam);

    *ulWidth  = ulParam[0];
    *ulHeight = ulParam[1];

    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_GetCurCaptScalInputRes
//  Description :
//------------------------------------------------------------------------------
/** @brief The function retrives capture mode sensor output resolution

The function retrives capture mode sensor output resolution
@param[out] usWidth  the width
@param[out] usHeight  the height
@retval MMP_ERR_NONE success
*/
MMP_ERR MMPS_Sensor_GetCurCaptScalInputRes(MMP_UBYTE ubSnrSel, MMP_ULONG *ulWidth, MMP_ULONG *ulHeight)
{
    MMP_ERR   err;
    MMP_ULONG ulMode, ulParam[2];

    ulMode = m_ubCurCaptureMode[ubSnrSel];

    err = MMPD_Sensor_GetDriverParam(ubSnrSel, MMP_SNR_DRV_PARAM_SCAL_IN_RES,
                                     1, &ulMode, 2, ulParam);

    *ulWidth  = ulParam[0];
    *ulHeight = ulParam[1];

    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_GetScalInputRes
//  Description :
//------------------------------------------------------------------------------
/** @brief The function retrives sensor output resolution of the specified mode

The function retrives sensor output resolution of the specified mode
@param[in]  ubSnrSel : the sensor id (PRM_SENSOR, SCD_SENSOR)
@param[in]  ubMode   : the sensor mode
@param[out] usWidth  : the width
@param[out] usHeight : the height
@retval MMP_ERR_NONE success
*/
MMP_ERR MMPS_Sensor_GetScalInputRes(MMP_UBYTE ubSnrId, MMP_UBYTE ubMode,
                                    MMP_ULONG *ulWidth, MMP_ULONG *ulHeight)
{
    MMP_ERR   err;
    MMP_ULONG ulMode, ulParam[2];

    ulMode = ubMode;

    err = MMPD_Sensor_GetDriverParam(ubSnrId, MMP_SNR_DRV_PARAM_SCAL_IN_RES,
                                     1, &ulMode, 2, ulParam);

    *ulWidth  = ulParam[0];
    *ulHeight = ulParam[1];

    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_GetPreviewResNum
//  Description :
//------------------------------------------------------------------------------
/** @brief The function retrives the total number of sensor output resolutions

The function retrives the total number of sensor output resolutions
@param[in]  ubSnrSel : the sensor id (PRM_SENSOR, SCD_SENSOR)
@retval the total number of sensor output resolutions
*/
MMP_UBYTE MMPS_Sensor_GetPreviewResNum(MMP_UBYTE ubSnrSel)
{
    MMP_ULONG ulParam = 0;

    MMPD_Sensor_GetDriverParam(ubSnrSel, MMP_SNR_DRV_PARAM_PREVIEW_RES_NUM,
                               0, NULL, 1, &ulParam);

    return (MMP_UBYTE)ulParam;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_GetDefPreviewMode
//  Description :
//------------------------------------------------------------------------------
/** @brief The function retrives the default preview mode

The function retrives the default preview mode
@param[in]  ubSnrSel : the sensor id (PRM_SENSOR, SCD_SENSOR)
@retval the default preview mode
*/
MMP_UBYTE MMPS_Sensor_GetDefPreviewMode(MMP_UBYTE ubSnrSel)
{
	MMP_ULONG ulParam = 0;

    MMPD_Sensor_GetDriverParam(ubSnrSel, MMP_SNR_DRV_PARAM_DEF_PREVIEW_RES,
                               0, NULL, 1, &ulParam);

    return (MMP_UBYTE)ulParam;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_GetCurFpsx10
//  Description :
//------------------------------------------------------------------------------
/** @brief The function retrives sensor output frame rate x 10 of current mode

The function retrives current sensor output frame rate x 10 of current mode
@param[out] ulFpsx10  sensor output frame rate x 10
@retval MMP_ERR_NONE success
*/
MMP_ERR MMPS_Sensor_GetCurFpsx10(MMP_UBYTE ubSnrSel, MMP_ULONG *ulFpsx10)
{
    MMP_ULONG ulMode = m_ubCurPreviewMode[ubSnrSel];

    return MMPD_Sensor_GetDriverParam(ubSnrSel, MMP_SNR_DRV_PARAM_FPSx10,
                                      1, &ulMode, 1, ulFpsx10);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_PowerDown
//  Description :
//------------------------------------------------------------------------------
/** @brief Power Down Sensor

The function power down Sensor. Before the function is called, the exact firmware code needs
to be downloaded into the MMP memory

@param[in] usSensor the Sensor Module
@retval MMP_ERR_NONE success
*/
MMP_ERR MMPS_Sensor_PowerDown(MMP_UBYTE ubSnrSel)
{
    MMP_ERR error = MMP_ERR_NONE;
    
    m_bSensorInit[ubSnrSel] = MMP_FALSE;
    
    error =	MMPD_Sensor_PowerDown(ubSnrSel);
    
    return error;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_SetGain
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_Sensor_SetGain(MMP_UBYTE ubSnrSel, MMP_ULONG ulGain, MMP_UBYTE ubType)
{
	return MMPD_Sensor_SetGain(ubSnrSel, ulGain, ubType);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_SetShutter
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_Sensor_SetShutter(MMP_UBYTE ubSnrSel, MMP_UBYTE ubIndex, MMP_UBYTE ubType)
{
	return MMPD_Sensor_SetShutter(ubSnrSel, ubIndex, ubType);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_SetExposureLimit
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPS_Sensor_SetExposureLimit(MMP_ULONG ulBufaddr, MMP_ULONG ulDataTypeByByte, MMP_ULONG ulSize, MMP_UBYTE ubType)
{
	return MMPD_Sensor_SetExposureLimit(ulBufaddr, ulDataTypeByByte, ulSize, ubType);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_SetSensorReg
//  Description :
//------------------------------------------------------------------------------
/** @brief Write the sensor register

@param[in] usAddr the register
@param[in] usData the data
@retval MMP_ERR_NONE success
@return It reports the status of the operation.
*/
MMP_ERR MMPS_Sensor_SetSensorReg(MMP_UBYTE ubSnrSel, MMP_USHORT usAddr, MMP_USHORT usData)
{
    if (MMP_TRUE == m_bSensorInit[ubSnrSel]) {
        return MMPD_Sensor_SetSensorReg(ubSnrSel, usAddr, usData);
    }

	return MMP_SENSOR_ERR_INITIALIZE_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_GetSensorReg
//  Description :
//------------------------------------------------------------------------------
/** @brief Read the sensor register

@param[in] usAddr the register
@param[in] usData the data
@retval MMP_ERR_NONE success
@return It reports the status of the operation.
*/
MMP_ERR	MMPS_Sensor_GetSensorReg(MMP_UBYTE ubSnrSel, MMP_USHORT usAddr, MMP_USHORT *usData)
{
    if (MMP_TRUE == m_bSensorInit[ubSnrSel]) {
        return MMPD_Sensor_GetSensorReg(ubSnrSel, usAddr, usData);
    }
    else {
        return MMP_SENSOR_ERR_INITIALIZE_NONE;
    }
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_SetFlip
//  Description :
//------------------------------------------------------------------------------
/** @brief Set the sensor flip mode

The function configures sensor flip type in the preview mode
@param[in] ubFlipType the sensor flip type
@retval MMP_ERR_NONE success
@return It reports the status of the operation.
*/
MMP_ERR MMPS_Sensor_SetFlip(MMP_UBYTE ubSnrSel, MMP_UBYTE ubFlipType)
{
    return MMPD_Sensor_SetFlip(ubSnrSel, ubFlipType);
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_GetMemEnd
//  Description :
//------------------------------------------------------------------------------
/** @brief Get the mem end address after sensor init

The function get the mem end address after sensor init
@param[out] ulMemEnd the mem end address
@retval MMP_ERR_NONE success
*/
MMP_ERR MMPS_Sensor_GetMemEnd(MMP_ULONG *ulMemEnd)
{
    *ulMemEnd = m_ulSensorMemEnd;
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_Set3AFunction
//  Description :
//------------------------------------------------------------------------------
/** @brief Set the 3A function

The function Set the 3A function
@param[in] sFunc the index of the 3A function
@param[in] pParam the parameter of the 3A function
@retval MMP_ERR_NONE success
*/
MMP_ERR MMPS_Sensor_Set3AFunction(MMP_ISP_3A_FUNC sFunc, int pParam)
{
    if(MMPS_Sensor_IsInitialized(gsAhcCurrentSensor))
        return MMPD_Sensor_Set3AFunction(sFunc, pParam);
    else
        return MMP_SENSOR_ERR_INITIALIZE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_SetIQFunction
//  Description :
//------------------------------------------------------------------------------
/** @brief Set the IQ function

The function Set the IQ function
@param[in] sFunc the index of the IQ function
@param[in] pParam the parameter of the IQ function
@retval MMP_ERR_NONE success
*/
MMP_ERR MMPS_Sensor_SetIQFunction(MMP_ISP_IQ_FUNC sFunc, int pParam)
{
    if(MMPS_Sensor_IsInitialized(gsAhcCurrentSensor))
        return MMPD_Sensor_SetIQFunction(sFunc, pParam);
    else
        return MMP_SENSOR_ERR_INITIALIZE;
}

//------------------------------------------------------------------------------
//  Function    : MMPS_Sensor_SetPreviewDelayCount
//  Description :
//------------------------------------------------------------------------------
/** @brief Set Preview Delay Count

The function Set Preview Delay Count to delay image show-up on display.
The purpose is to avoid unstable image after sensor init.
@param[in] ubSnrSel the index of sensor
@param[in] ubDelayCount the parameter of delay time (unit: frame)
                        Set it as DISP_DISABLE_PREVIEW_DELAY_COUNT to hide the preview.
@retval MMP_ERR_NONE success
*/
MMP_ERR MMPS_Sensor_SetPreviewDelayCount(MMP_UBYTE ubSnrSel, MMP_UBYTE ubDelayCount)
{
    m_ubPreviewDelayCount[ubSnrSel%SENSOR_MAX_NUM] = ubDelayCount;

    return MMP_ERR_NONE;
}




/// @}

//==============================================================================
//
//  File        : AHC_Sensor.c
//  Description : AHC Sensor control function
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================

/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "ait_config.h"
#include "mmps_dsc.h"
#include "mmps_3gprecd.h"
#include "mmps_display.h"
#include "mmps_sensor.h"
#include "mmpf_sensor.h"
#include "mmpf_pll.h"
#include "AHC_general.h"
#include "AHC_Sensor.h"
#include "isp_if.h"
#include "hdr_cfg.h"
#include "MenuSetting.h"
#include "Sensor_Mod_Remapping.h"

/*===========================================================================
 * Extern varible
 *===========================================================================*/ 

extern MMP_USHORT               gsAhcCurrentSensor;
extern MMP_USHORT				m_ubFlipType;
/*===========================================================================
 * Local varible
 *===========================================================================*/ 

static UINT8                    m_byPreviewExposure;
static UINT8                    m_byCaptureExposure;
static UINT8					m_byPreviewAGC;
static UINT8					m_byCaptureAGC;
static INT32                    m_wSensorMode 			= AHC_SENSOR_MODE_AUTO;
static INT32                    m_wCaptureSensorMode 	= AHC_SENSOR_MODE_AUTO;
static INT32                    m_wTVDecSrcTypeMode = AHC_SENSOR_MODE_AUTO;

/*===========================================================================
 * Main body
 *===========================================================================*/

//------------------------------------------------------------------------------
//  Function    : AHC_SetPreviewExposure
//  Description : 
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetPreviewExposure(UINT8 byIndex)
{
    MMP_ERR error = MMP_ERR_NONE;
    
    error = MMPS_Sensor_SetShutter(gsAhcCurrentSensor, byIndex, MMP_SNR_PREV_MODE);
    
    if (error == MMP_ERR_NONE) {
        m_byPreviewExposure = byIndex;
	}
	return (error == MMP_ERR_NONE) ? AHC_TRUE : AHC_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetPreviewExposure
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_GetPreviewExposure(UINT16 *pwValue)
{
    *pwValue = m_byPreviewExposure;
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetPreviewAgc
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetPreviewAgc(UINT8 byIndex)
{
	MMP_ERR error = MMP_ERR_NONE;
	
	error = MMPS_Sensor_SetGain(gsAhcCurrentSensor, byIndex, MMP_SNR_PREV_MODE);
	
	if (error == MMP_ERR_NONE) {
		m_byPreviewAGC = byIndex;
	}
	return (error == MMP_ERR_NONE) ? AHC_TRUE : AHC_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetPreviewAgc
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_GetPreviewAgc(UINT16 *pwValue)
{
	*pwValue = m_byPreviewAGC;
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetCaptureExposure
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetCaptureExposure(UINT8 byIndex)
{
    MMP_ERR error = MMP_ERR_NONE;

    error = MMPS_Sensor_SetShutter(gsAhcCurrentSensor, byIndex, MMP_SNR_CAPT_MODE);

    if (error == MMP_ERR_NONE) {
        m_byCaptureExposure = byIndex;
	}
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetCaptureExposure
//  Description : 
//------------------------------------------------------------------------------
AHC_BOOL AHC_GetCaptureExposure(UINT16 *pwValue)
{
    *pwValue = m_byCaptureExposure;
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetCaptureAgc
//  Description : 
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetCaptureAgc(UINT8 byIndex)
{
	MMP_ERR error = MMP_ERR_NONE;

	error = MMPS_Sensor_SetGain(gsAhcCurrentSensor, byIndex, MMP_SNR_CAPT_MODE);

	if (error == MMP_ERR_NONE) {
		m_byCaptureAGC = byIndex;
	}
	return (error == MMP_ERR_NONE) ? AHC_TRUE : AHC_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetCaptureAgc
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_GetCaptureAgc(UINT16 *pwValue)
{
	*pwValue = m_byCaptureAGC;
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetPreviewExposureLimit
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetPreviewExposureLimit(UINT32 buf_ptr, UINT32 DatatypeByByte, UINT32 length)
{
	MMP_ERR error;

	error = MMPS_Sensor_SetExposureLimit(buf_ptr, DatatypeByByte, length, MMP_SNR_PREV_MODE);

	return (error == MMP_ERR_NONE) ? AHC_TRUE : AHC_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetCaptureExposureLimit
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetCaptureExposureLimit(UINT32 buf_ptr, UINT32 DatatypeByByte, UINT32 length)
{	
	MMP_ERR error;

	error = MMPS_Sensor_SetExposureLimit(buf_ptr, DatatypeByByte, length, MMP_SNR_CAPT_MODE);

	return (error == MMP_ERR_NONE) ? AHC_TRUE : AHC_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_PresetSensorMode
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief This API preset sensor mode for AHC usage.
 
 This function preset sensor mode for AHC usage. This APS is 
 for select sensor mode before enter video preview or still 
 capture preview.
 
 Parameters:
 
 @param[in] wMode          The sensor mode id. This is the 
                           definition in sensor driver or in
                           ait_config.c .
  
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_PresetSensorMode(INT16 wMode)
{
	UINT32      ulHdrEn = 0;
    MMP_ULONG   ulGroupSrc;
    MMP_UBYTE   ubPllDiv;

    switch(wMode){
        case AHC_SENSOR_MODE_VGA_30P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_VGA_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_VGA_50P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_VGA_50P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_VGA_60P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_VGA_60P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_VGA_100P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_VGA_100P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_VGA_120P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_VGA_120P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_HD_24P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_HD_24P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_HD_30P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_HD_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_HD_50P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_HD_50P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_HD_60P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_HD_60P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_HD_100P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_HD_100P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_HD_120P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_HD_120P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_FULL_HD_15P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_FULL_HD_15P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_FULL_HD_24P_RESOLUTION:
             m_wSensorMode = SENSOR_DRIVER_MODE_FULL_HD_24P_RESOLUTION;
             break;

        case AHC_SENSOR_MODE_FULL_HD_25P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_FULL_HD_25P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_FULL_HD_30P_RESOLUTION:

            if (AHC_Menu_SettingGetCB((char *)COMMON_KEY_HDR_EN, &ulHdrEn) == AHC_TRUE) {
                if (COMMON_HDR_EN_ON == ulHdrEn) {
                    m_wSensorMode = SENSOR_DRIVER_MODE_FULL_HD_30P_RESOLUTION_HDR;
                    if (m_wSensorMode == SENSOR_DRIVER_MODE_NOT_SUUPORT)
                    {
                    	pf_HDR_EnSet(COMMON_HDR_EN_OFF);
                    	printc("Sensor not support HDR, set sensor mode back to FHD\r\n");
                    	m_wSensorMode = SENSOR_DRIVER_MODE_FULL_HD_30P_RESOLUTION;
                    }
                }
                else {
                    m_wSensorMode = SENSOR_DRIVER_MODE_FULL_HD_30P_RESOLUTION;
                }
            }
            else {
                m_wSensorMode = SENSOR_DRIVER_MODE_FULL_HD_30P_RESOLUTION;
            }
            break;

        case AHC_SENSOR_MODE_FULL_HD_50P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_FULL_HD_50P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_FULL_HD_60P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_FULL_HD_60P_RESOLUTION;
            break;
		
		case AHC_SENSOR_MODE_1440_30P_RESOLUTION:
			m_wSensorMode = SENSOR_DRIVER_MODE_1440_30P_RESOLUTION;
			break;

        case AHC_SENSOR_MODE_SUPER_HD_30P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_SUPER_HD_30P_RESOLUTION;
            break;
	 case AHC_SENSOR_MODE_SUPER_HD_25P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_SUPER_HD_25P_RESOLUTION;
            break;		
	 case AHC_SENSOR_MODE_SUPER_HD_24P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_SUPER_HD_24P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_2D7K_15P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_2D7K_15P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_2D7K_30P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_2D7K_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4K2K_15P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_4K2K_15P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4K2K_30P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_4K2K_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_VGA_30P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_4TO3_VGA_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_1D2M_30P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_4TO3_1D2M_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_1D5M_30P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_4TO3_1D5M_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_3M_15P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_4TO3_3M_15P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_3M_30P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_4TO3_3M_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_5M_15P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_4TO3_5M_15P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_5M_30P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_4TO3_5M_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_8M_15P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_4TO3_8M_15P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_8M_30P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_4TO3_8M_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_10M_15P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_4TO3_10M_15P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_10M_30P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_4TO3_10M_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_12M_15P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_4TO3_12M_15P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_12M_30P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_4TO3_12M_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_14M_15P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_4TO3_14M_15P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_14M_30P_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_4TO3_14M_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_BEST_CAMERA_PREVIEW_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_BEST_CAMERA_PREVIEW_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_DEFAULT_PCCAM_RESOLUTION:
            m_wSensorMode = SENSOR_DRIVER_MODE_PCCAM_DEFAULT_RESOLUTION;
            break;
        
        default:
            m_wSensorMode = SENSOR_DRIVER_MODE_NOT_SUUPORT;
            break;
    }

	if (m_wSensorMode == SENSOR_DRIVER_MODE_FULL_HD_30P_RESOLUTION_HDR &&
		m_wSensorMode != SENSOR_DRIVER_MODE_NOT_SUUPORT) {
		MMP_EnalbeHDR(MMP_TRUE);

		/* Change Bayer/ISP clock to faster clock */
		MMPF_PLL_ChangeVIFClock(DPLL5, 1);
		MMPF_PLL_WaitCount(50);
		MMPF_PLL_ChangeBayerClock(DPLL0, 1); // Need to ensure PLL0 is 600MHz
		MMPF_PLL_WaitCount(50);
		MMPS_System_GetGroupSrcAndDiv(CLK_GRP_COLOR, &ulGroupSrc, &ubPllDiv);
		MMPF_PLL_ChangeISPClock(ulGroupSrc, ubPllDiv);
	    MMPF_PLL_WaitCount(50);
	}
	else {
		MMP_EnalbeHDR(MMP_FALSE);
		
		/* Restore the Bayer/ISP clock */
		MMPS_System_GetGroupSrcAndDiv(CLK_GRP_BAYER, &ulGroupSrc, &ubPllDiv);
		MMPF_PLL_ChangeBayerClock(ulGroupSrc, ubPllDiv);
	    MMPF_PLL_WaitCount(50);
		MMPS_System_GetGroupSrcAndDiv(CLK_GRP_COLOR, &ulGroupSrc, &ubPllDiv);
		MMPF_PLL_ChangeISPClock(ulGroupSrc, ubPllDiv);
	    MMPF_PLL_WaitCount(50);
	}

    if (m_wSensorMode == SENSOR_DRIVER_MODE_NOT_SUUPORT) {
        printc(">>>>> AHC_PresetSensorMode Error!!!:%d\r\n",wMode);
        while(1);
    }
    return AHC_TRUE;
}
#if (SUPPORT_DUAL_SNR)
AHC_BOOL AHC_PresetTVDecSrcType(INT32 wTVDecSrcType)
{
    switch(wTVDecSrcType){
        case AHC_SENSOR_MODE_PAL_25FPS:
            m_wTVDecSrcTypeMode = SENSOR_DRIVER_MODE_PAL_25P_RESOLUTION;
            break;
        case AHC_SENSOR_MODE_NTSC_30FPS:
        default:
            m_wTVDecSrcTypeMode = SENSOR_DRIVER_MODE_NTSC_30P_RESOLUTION;
            break;            
    }
    
    return AHC_TRUE;
}

INT32 AHC_GetPresetTVDecSrcType(void)
{
    return m_wTVDecSrcTypeMode;
}
#endif
//------------------------------------------------------------------------------
//  Function    : AHC_GetPresetSensorMode
//  Description :
//------------------------------------------------------------------------------
INT16 AHC_GetPresetSensorMode(void)
{
    return m_wSensorMode;
}

//------------------------------------------------------------------------------
//  Function    : AHC_PresetCaptureSensorMode
//  Description : 
//------------------------------------------------------------------------------
AHC_BOOL AHC_PresetCaptureSensorMode(INT16 wMode)
{
    _AHC_PRINT_FUNC_ENTRY_();

    switch(wMode){
        case AHC_SENSOR_MODE_VGA_30P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_VGA_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_VGA_50P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_VGA_50P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_VGA_60P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_VGA_60P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_VGA_100P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_VGA_100P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_VGA_120P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_VGA_120P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_HD_30P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_HD_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_HD_50P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_HD_50P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_HD_60P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_HD_60P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_HD_100P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_HD_100P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_HD_120P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_HD_120P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_FULL_HD_15P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_FULL_HD_15P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_FULL_HD_25P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_FULL_HD_25P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_FULL_HD_30P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_FULL_HD_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_FULL_HD_50P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_FULL_HD_50P_RESOLUTION;
            break;

		case AHC_SENSOR_MODE_1440_30P_RESOLUTION:
			m_wCaptureSensorMode = SENSOR_DRIVER_MODE_1440_30P_RESOLUTION;
			break;

        case AHC_SENSOR_MODE_SUPER_HD_30P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_SUPER_HD_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_FULL_HD_60P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_FULL_HD_60P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_2D7K_15P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_2D7K_15P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_2D7K_30P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_2D7K_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4K2K_15P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_4K2K_15P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4K2K_30P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_4K2K_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_VGA_30P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_4TO3_VGA_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_1D2M_30P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_4TO3_1D2M_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_1D5M_30P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_4TO3_1D5M_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_3M_15P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_4TO3_3M_15P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_3M_30P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_4TO3_3M_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_5M_15P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_4TO3_5M_15P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_5M_30P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_4TO3_5M_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_8M_15P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_4TO3_8M_15P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_8M_30P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_4TO3_8M_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_10M_15P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_4TO3_10M_15P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_10M_30P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_4TO3_10M_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_12M_15P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_4TO3_12M_15P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_12M_30P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_4TO3_12M_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_14M_15P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_4TO3_14M_15P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_4TO3_14M_30P_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_4TO3_14M_30P_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_BEST_CAMERA_CAPTURE_4TO3_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_BEST_CAMERA_CAPTURE_4TO3_RESOLUTION;
            break;

        case AHC_SENSOR_MODE_BEST_CAMERA_CAPTURE_16TO9_RESOLUTION:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_BEST_CAMERA_CAPTURE_16TO9_RESOLUTION;
            break;

        default:
            m_wCaptureSensorMode = SENSOR_DRIVER_MODE_NOT_SUUPORT;
            break;
    }

    if (m_wCaptureSensorMode == SENSOR_DRIVER_MODE_NOT_SUUPORT) {
        printc(">>>>> AHC_PresetCaptureSensorMode Error!!!:%d\r\n",wMode);
        while(1);
    }

    MMPS_Sensor_SetCaptureMode(gsAhcCurrentSensor, m_wCaptureSensorMode);

    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetPresetCaptureSensorMode
//  Description :
//------------------------------------------------------------------------------
INT16 AHC_GetPresetCaptureSensorMode(void)
{
    return m_wCaptureSensorMode;
}

AHC_BOOL AHC_Sensor_IsInitialized(void)
{
	return MMPS_Sensor_IsInitialized(gsAhcCurrentSensor);
}

//------------------------------------------------------------------------------
//  Function    : AHC_Set_SensorFlip
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_Set_SensorFlip(AHC_SENSOR_FLIP_TYPE ubFlipType)
{
	MMPS_Sensor_SetFlip(gsAhcCurrentSensor, ubFlipType);
	m_ubFlipType = (MMP_UBYTE)ubFlipType;
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_Set_SensorLightFreq
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_Set_SensorLightFreq(AHC_SENSOR_DEBAND_MODE smode)
{
	MMPS_Sensor_Set3AFunction(MMP_ISP_3A_FUNC_SET_AE_FLICKER_MODE, (MMP_SNR_DEBAND_MODE)smode);
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetScalInputWH
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_GetScalInputWH(UINT16 wMode, UINT16* puwWidth, UINT16* puwHeight)
{
    MMP_ULONG ulWidth, ulHeight;

    if (wMode != SENSOR_DRIVER_MODE_NOT_SUUPORT) {
        return AHC_FALSE;
    }

    MMPS_Sensor_GetScalInputRes(gsAhcCurrentSensor, wMode, &ulWidth, &ulHeight);
    
    *puwWidth = ulWidth;
    *puwHeight = ulHeight;

    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetCurSensorFPS
//  Description :
//------------------------------------------------------------------------------
UINT32 AHC_GetCurSensorFPS(void)
{
	extern MMPF_SENSOR_FUNCTION *gsSensorFunction;

    if (gsSensorFunction->MMPF_Sensor_GetRealFPS) {
        return (UINT32) gsSensorFunction->MMPF_Sensor_GetRealFPS(gsAhcCurrentSensor);
    }
    return 0;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SensorPowerCtrl
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SensorPowerCtrl(AHC_BOOL bOn)
{
#ifdef SENSOR_EN_GPIO
	if (SENSOR_EN_GPIO != AHC_PIO_REG_UNKNOWN)
	{
        AHC_ConfigGPIOPad(SENSOR_EN_GPIO, PAD_OUT_DRIVING(0));
		AHC_ConfigGPIO(SENSOR_EN_GPIO, AHC_TRUE);
		AHC_SetGPIO(SENSOR_EN_GPIO, (bOn ? SENSOR_EN_GPIO_ACT_MODE : !SENSOR_EN_GPIO_ACT_MODE));
	}
#endif

	return AHC_TRUE;
}

#if (SUPPORT_DUAL_SNR == 1 || DUALENC_SUPPORT == 1) //For Bitek1603
//------------------------------------------------------------------------------
//  Function    : AHC_GetSecondSensorCnntStatus
//  Description : 
//------------------------------------------------------------------------------
AHC_BOOL AHC_GetSecondSensorCnntStatus(void)
{
    MMPF_VIF_TV_DEC_SRC_TYPE sSnrTVSrc;
    MMPF_Sensor_GetTV_Dec_Src_Type(&sSnrTVSrc);

    if(sSnrTVSrc == MMPF_VIF_SRC_NO_READY){
        return AHC_FALSE;
    }
    
    return AHC_TRUE;

}

AHC_BOOL AHC_GetSecondSensorCurrentCnntStatus(void)
{
#if 0 //It should move to sensor driver.
	MMP_USHORT usData = 0;
	MMP_USHORT usReg = 0;
	MMP_ERR ret;

#if defined(BIND_SENSOR_BIT1603)

	MMPS_Sensor_GetSensorReg(SCD_SENSOR, 0x2C, &usReg);//bit7=1,source detection mode
	MMPS_Sensor_SetSensorReg(SCD_SENSOR, 0x2C, 0x80);
	AHC_OS_SleepMs(50);
	ret = MMPS_Sensor_GetSensorReg(SCD_SENSOR, 0x7b, &usData);//BIT0&BIT2=1,signal toggle
	MMPS_Sensor_SetSensorReg(SCD_SENSOR, 0x2C, usReg);

	if(ret == MMP_ERR_NONE){
		if(usData & 0x05)
			m_bSecondSensorCnnt = AHC_TRUE;
		else
			m_bSecondSensorCnnt = AHC_FALSE;
	}
#endif

 #if defined(BIND_SENSOR_CJC5150)||defined(BIND_SENSOR_GM7150)
       ret = MMPS_Sensor_GetSensorReg(SCD_SENSOR, 0x88, &usData);//BIT0&BIT2=1,signal toggle
       //printc("[0x%x]", usData);
	   if ((usData & 0x10) == 0x10)
           m_bSecondSensorCnnt = AHC_FALSE;
       else
           m_bSecondSensorCnnt = AHC_TRUE;
#endif
	return ret;//m_bSecondSensorCnnt;
#endif
}
#endif


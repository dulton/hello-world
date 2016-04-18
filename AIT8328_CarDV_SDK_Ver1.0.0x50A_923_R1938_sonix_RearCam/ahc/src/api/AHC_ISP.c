//==============================================================================
//
//  File        : AHC_ISP.c
//  Description : AHC ISP control function
//  Author      :
//  Revision    : 1.0
//
//==============================================================================

/*===========================================================================
 * Include files
 *===========================================================================*/

#include "AHC_ISP.h"
#include "isp_if.h"
#include "MenuSetting.h"
#include "mmps_sensor.h"
#include "ISP_cfg.h"
#include "AHC_Config_Dummy.h"

/*===========================================================================
 * Macro Define
 *===========================================================================*/

#define ISP_DEBUG_OFF 	(0)
#define ISP_DEBUG_ON 	(1)
#define ISP_DEBUG 		ISP_DEBUG_OFF

/*===========================================================================
 * Global variable
 *===========================================================================*/

MMP_SHORT gsSharpnessCenter = SHARPNESS_CENTER;
MMP_SHORT gsGammaCenter    	= GAMMA_CENTER;
MMP_SHORT gsContrastCenter 	= CONTRAST_CENTER;
MMP_SHORT gsSaturateCenter 	= SATURATE_CENTER;

/*===========================================================================
 * Main body
 *===========================================================================*/

//------------------------------------------------------------------------------
//  Function    : AHC_TransAWBMenu2ISPSetting
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_TransAWBMenu2ISPSetting(UINT8 ubMenuItem, UINT8 *ISPItem)
{
    #if (ISP_DEBUG == ISP_DEBUG_ON)
	printc("ISP:AHC_TransAWBMenu2ISPSetting ubMenuItem=%d\r\n", ubMenuItem);
    #endif

	switch (ubMenuItem)
	{
	#if (MENU_MANUAL_WB_AUTO_EN)
		case WB_AUTO: 			*ISPItem = AHC_AWB_MODE_AUTO; 					break;
	#endif
	#if (MENU_MANUAL_WB_DAYLIGHT_EN)
		case WB_DAYLIGHT: 		*ISPItem = AHC_AWB_MODE_DAYLIGHT; 				break;
	#endif
	#if (MENU_MANUAL_WB_CLOUDY_EN)
		case WB_CLOUDY: 		*ISPItem = AHC_AWB_MODE_CLOUDY; 				break;
	#endif
	#if (MENU_MANUAL_WB_FLUORESCENT1_EN)
		case WB_FLUORESCENT1: 	*ISPItem = AHC_AWB_MODE_FLUORESCENT_WHITE; 		break;
	#endif
	#if (MENU_MANUAL_WB_FLUORESCENT2_EN)
		case WB_FLUORESCENT2: 	*ISPItem = AHC_AWB_MODE_FLUORESCENT_NATURAL; 	break;
	#endif
	#if (MENU_MANUAL_WB_FLUORESCENT3_EN)
		case WB_FLUORESCENT3: 	*ISPItem = AHC_AWB_MODE_FLUORESCENT_DAYLIGHT; 	break;
	#endif
	#if (MENU_MANUAL_WB_INCANDESCENT_EN)
		case WB_INCANDESCENT: 	*ISPItem = AHC_AWB_MODE_INCANDESCENT; 			break;
	#endif
	#if (MENU_MANUAL_WB_FLASH_EN)
		case WB_FLASH: 			*ISPItem = AHC_AWB_MODE_FLASH; 					break;
	#endif
	#if (MENU_MANUAL_WB_ONEPUSH_EN)
		case WB_ONEPUSH: 		*ISPItem = AHC_AWB_MODE_ONEPUSH; 				break;
	#endif
	#if (MENU_MANUAL_WB_ONE_PUSH_SET_EN)
		case WB_ONE_PUSH_SET: 	*ISPItem = AHC_AWB_MODE_ONEPUSH; 				break;
	#endif
		default: 				*ISPItem = AHC_AWB_MODE_AUTO; 					break;
	}

	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetAwbFastMode
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetAwbFastMode(UINT8 byEnable)
{
    #if (ISP_DEBUG == ISP_DEBUG_ON)
	printc("ISP:AHC_SetAwbFastMode byEnable=%d\r\n", byEnable);
    #endif

	MMPS_Sensor_Set3AFunction(MMP_ISP_3A_FUNC_SET_AWB_FAST_MODE, byEnable);
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetAeFastMode
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetAeFastMode(UINT8 byEnable)
{
    #if (ISP_DEBUG == ISP_DEBUG_ON)
	printc("ISP:AHC_SetAeFastMode byEnable=%d\r\n", byEnable);
    #endif

	MMPS_Sensor_Set3AFunction(MMP_ISP_3A_FUNC_SET_AE_FAST_MODE, byEnable);
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetAeIsoMode
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetAeIsoMode(AHC_AE_ISO byMode)
{
	UINT32 ulIso = 0;

    #if (ISP_DEBUG == ISP_DEBUG_ON)
	printc("ISP:AHC_SetAeIsoMode byMode=%d\r\n", byMode);
	#endif

	switch(byMode) {
		case AHC_AE_ISO_AUTO: 	ulIso = 0; 		break;
		case AHC_AE_ISO_100: 	ulIso = 100; 	break;
		case AHC_AE_ISO_200: 	ulIso = 200; 	break;
		case AHC_AE_ISO_400: 	ulIso = 400; 	break;
		case AHC_AE_ISO_800: 	ulIso = 800; 	break;
		case AHC_AE_ISO_1600: 	ulIso = 1600; 	break;
		case AHC_AE_ISO_3200: 	ulIso = 3200; 	break;
		default:				ulIso = 0; 		break;
	}

	MMPS_Sensor_Set3AFunction(MMP_ISP_3A_FUNC_SET_ISO, ulIso);
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetAeFlickerMode
//  Description : 
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetAeFlickerMode(AHC_AE_FLICKER byMode)
{
    #if (ISP_DEBUG == ISP_DEBUG_ON)
	printc("ISP:AHC_SetAeFlickerMode byMode=%d\r\n", byMode);
    #endif
    
	MMPS_Sensor_Set3AFunction(MMP_ISP_3A_FUNC_SET_AE_FLICKER_MODE, byMode);
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetAeMeteringMode
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetAeMeteringMode(AHC_AE_METERING byMode)
{
    #if (ISP_DEBUG == ISP_DEBUG_ON)
	printc("ISP:AHC_SetAeMeteringMode byMode=%d\r\n", byMode);
    #endif

    MMPS_Sensor_Set3AFunction(MMP_ISP_3A_FUNC_SET_AE_METER_MODE, byMode);
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetAeEvBiasMode
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetAeEvBiasMode(AHC_AE_EV_BIAS byMode)
{
    int lEv = 0;

    #if (ISP_DEBUG == ISP_DEBUG_ON)
	printc("ISP:AHC_SetAeEvBiasMode byMode=%d\r\n", byMode);
    #endif

	switch (byMode)
	{
		case AHC_AE_EV_BIAS_M200:
			lEv = -200;
			break;
		case AHC_AE_EV_BIAS_M166:
			lEv = -166;
			break;
		case AHC_AE_EV_BIAS_M133:
			lEv = -133;
			break;
		case AHC_AE_EV_BIAS_M100:
			lEv = -100;
			break; 
		case AHC_AE_EV_BIAS_M066:
			lEv = -66;
			break;
		case AHC_AE_EV_BIAS_M033:
			lEv = -33;
			break;
		case AHC_AE_EV_BIAS_0000:
			lEv = 0;
			break;
		case AHC_AE_EV_BIAS_P033:
			lEv = 33;
			break;
		case AHC_AE_EV_BIAS_P066:
			lEv = 66;
			break;
		case AHC_AE_EV_BIAS_P100:
			lEv = 100;
			break;
		case AHC_AE_EV_BIAS_P133:
			lEv = 133;
			break;
		case AHC_AE_EV_BIAS_P166:
			lEv = 166;
			break;
		case AHC_AE_EV_BIAS_P200:
			lEv = 200;
			break;
	}
	
	MMPS_Sensor_Set3AFunction(MMP_ISP_3A_FUNC_SET_EV, lEv);
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetAeSceneMode
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetAeSceneMode(AHC_SCENE byMode)
{
    #if (ISP_DEBUG == ISP_DEBUG_ON)
	printc("ISP:AHC_SetAeSceneMode byMode=%d\r\n", byMode);
	#endif

#if 0
	ISP_IF_F_SetScene(byMode);
#endif
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetAwbMode
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetAwbMode(AHC_AWB_MODE byMode)
{
    MMP_USHORT usMode       = 0;
    MMP_USHORT usColorTemp  = 5500;

    #if (ISP_DEBUG == ISP_DEBUG_ON)
	printc("ISP:AHC_SetAwbMode byMode=%d\r\n", byMode);
	#endif

	switch (byMode)
	{
	case AHC_AWB_MODE_BYPASS:
	    usMode = ISP_AWB_MODE_BYPASS;
	    break;
	case AHC_AWB_MODE_AUTO:
	    usMode = ISP_AWB_MODE_AUTO;
	    break;
	case AHC_AWB_MODE_MANUAL:
	    usMode = ISP_AWB_MODE_MANUAL;
	    usColorTemp = 5500;
	    break;
    case AHC_AWB_MODE_DAYLIGHT:
        usMode = ISP_AWB_MODE_MANUAL;
        usColorTemp = 6500;
        break;
	case AHC_AWB_MODE_CLOUDY:
	    usMode = ISP_AWB_MODE_MANUAL;
	    usColorTemp = 7500;
	    break;
	case AHC_AWB_MODE_FLUORESCENT_WHITE:
	    usMode = ISP_AWB_MODE_MANUAL;
	    usColorTemp = 4100;
	    break;
	case AHC_AWB_MODE_FLUORESCENT_DAYLIGHT:
	    usMode = ISP_AWB_MODE_MANUAL;
	    usColorTemp = 6000;
	    break;
	case AHC_AWB_MODE_INCANDESCENT:
	    usMode = ISP_AWB_MODE_MANUAL;
	    usColorTemp = 3000;
	    break;
	default :
	    usMode = ISP_AWB_MODE_AUTO;
	}

    MMPS_Sensor_Set3AFunction(MMP_ISP_3A_FUNC_SET_AWB_MODE, usMode);

    if (AHC_AWB_MODE_AUTO != usMode) {
        // Run MMP_ISP_3A_FUNC_SET_AWB_COLOR_TEMP, AWB will be change to manual mode by ISP LIB
        MMPS_Sensor_Set3AFunction(MMP_ISP_3A_FUNC_SET_AWB_COLOR_TEMP, usColorTemp);
    }

	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetImageEffect
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetImageEffect(AHC_IMAGE_EFFECT byMode)
{
    #if (ISP_DEBUG == ISP_DEBUG_ON)
	printc("ISP:AHC_SetImageEffect byMode=%d\r\n", byMode);
    #endif

	MMPS_Sensor_SetIQFunction(MMP_ISP_IQ_FUNC_SET_EFFECT, byMode);
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetColorSaturateLevel
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetColorSaturateLevel(INT16 byLev)
{
    #if (ISP_DEBUG == ISP_DEBUG_ON)
	printc("ISP:AHC_SetColorSaturateLevel gsSaturateCenter %d byLev=%d\r\n",gsSaturateCenter,  byLev);
    #endif
    
    if (gsSaturateCenter + byLev > 128)
        MMPS_Sensor_SetIQFunction(MMP_ISP_IQ_FUNC_SET_SATURATION, 128);
    else if (gsSaturateCenter + byLev < -128)
        MMPS_Sensor_SetIQFunction(MMP_ISP_IQ_FUNC_SET_SATURATION, -128);
    else
        MMPS_Sensor_SetIQFunction(MMP_ISP_IQ_FUNC_SET_SATURATION, gsSaturateCenter + byLev);

    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetColorSaturateCenter
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetColorSaturateCenter(INT16 byLev)
{
    gsSaturateCenter = byLev;
    #if (ISP_DEBUG == ISP_DEBUG_ON)
    printc("ISP:AHC_SetColorSaturateCenter gsSaturateCenter new %d ", gsSaturateCenter);
    #endif

    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetColorSharpnessLevel
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetColorSharpnessLevel(INT16 byLev)
{
    #if (ISP_DEBUG == ISP_DEBUG_ON)
    printc("ISP:AHC_SetColorSharpnessLevel gsSharpnessCenter %d  byLev=%d\r\n", gsSharpnessCenter, byLev);
    #endif
    
    if (gsSharpnessCenter + byLev > 127)
        MMPS_Sensor_SetIQFunction(MMP_ISP_IQ_FUNC_SET_SHARPNESS, 127);
    else if (gsSharpnessCenter + byLev < -128)
        MMPS_Sensor_SetIQFunction(MMP_ISP_IQ_FUNC_SET_SHARPNESS, -128);
    else
        MMPS_Sensor_SetIQFunction(MMP_ISP_IQ_FUNC_SET_SHARPNESS, gsSharpnessCenter + byLev);

    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetColorSharpnessCenter
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetColorSharpnessCenter(INT16 byLev)
{
    gsSharpnessCenter = byLev;
    #if (ISP_DEBUG == ISP_DEBUG_ON)
    printc("ISP:AHC_SetColorSharpnessCenter gsSharpnessCenter new %d ", gsSharpnessCenter);
    #endif
    
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetColorGammaLevel
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetColorGammaLevel(INT16 byLev)
{
    #if (ISP_DEBUG == ISP_DEBUG_ON)
    printc("ISP:AHC_SetColorGammaLevel gsGammaCenter %d  byLev=%d\r\n", gsGammaCenter, byLev);
    #endif
    
    if (gsGammaCenter + byLev > 127)
        MMPS_Sensor_SetIQFunction(MMP_ISP_IQ_FUNC_SET_GAMMA, 127);
    else if (gsGammaCenter + byLev < -128)
        MMPS_Sensor_SetIQFunction(MMP_ISP_IQ_FUNC_SET_GAMMA, -128);
    else
        MMPS_Sensor_SetIQFunction(MMP_ISP_IQ_FUNC_SET_GAMMA, gsGammaCenter + byLev);

    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetColorGammaCenter
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetColorGammaCenter(INT16 byLev)
{
    gsGammaCenter = byLev;
    #if (ISP_DEBUG == ISP_DEBUG_ON)
    printc("ISP:AHC_SetColorGammaCenter gsGammaCenter new %d ", gsGammaCenter);
    #endif

    return AHC_TRUE;
}
//------------------------------------------------------------------------------
//  Function    : AHC_SetColorContrastLevel
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetColorContrastLevel(INT16 byLev)
{
    #if (ISP_DEBUG == ISP_DEBUG_ON)
    printc("ISP:AHC_SetColorContrastLevel gusContrastCenter %d  byLev=%d\r\n", gsContrastCenter, byLev);
    #endif

    if (gsContrastCenter + byLev > 128)
            MMPS_Sensor_SetIQFunction(MMP_ISP_IQ_FUNC_SET_CONTRAST, 128);
        else if (gsContrastCenter + byLev < -128)
            MMPS_Sensor_SetIQFunction(MMP_ISP_IQ_FUNC_SET_CONTRAST, -128);
        else
            MMPS_Sensor_SetIQFunction(MMP_ISP_IQ_FUNC_SET_CONTRAST, gsContrastCenter + byLev);

        return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetColorContrastCenter
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetColorContrastCenter(INT16 byLev)
{
    gsContrastCenter = byLev;
    #if (ISP_DEBUG == ISP_DEBUG_ON)
    printc("ISP:AHC_SetColorContrastCenter gsContrastCenter new %d ", gsContrastCenter);
    #endif

    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetAeWDRMode
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetAeWDRMode(AHC_AE_WDR byMode)
{
    #if (ISP_DEBUG == ISP_DEBUG_ON)
	printc("ISP:AHC_SetAeWDRMode byMode=%d\r\n", byMode);
    #endif
    
    MMPS_Sensor_SetIQFunction(MMP_ISP_IQ_FUNC_SET_WDR, byMode);
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : MMP_SensorDrv_PostInitISP
//  Description : This function will be called in MMPS_Sensor_Initialize()
//                which will reset ISP settings earlier.
//                It's used to reset ISP settings after sensor init.
//------------------------------------------------------------------------------
void MMP_SensorDrv_PostInitISP(void)
{
	// Re-set ISP settings in AHC_PostInitISP()
	if (AHC_PostInitISP != NULL)
	{
		AHC_PostInitISP();
	}
}

//------------------------------------------------------------------------------
//  Function    : 
//  Description : 
//------------------------------------------------------------------------------
//ISP_INT8 MultiShading;
//ISP_INT16 CCM_PRT[9];

void VR_PrintStringOnPreview(void)
{
#if (ISP_EN)
    extern ISP_UINT8 gDrawTextEn;

	if(gDrawTextEn){
		
	#if 0
		_sprintf(gDrawTextBuf, (ISP_INT8*)"AE  : Shutter=%x (%d)FPS, Gain=%x, ShutterBase = %x", (ISP_UINT32)ISP_IF_AE_GetShutter(), (ISP_UINT32)ISP_IF_AE_GetRealFPS(), (ISP_UINT32)ISP_IF_AE_GetGain(), ISP_IF_AE_GetShutterBase());
		VR_PrintString(gDrawTextBuf,  10, 0, 0x0000, 0x0000);
		_sprintf(gDrawTextBuf, (ISP_INT8*)"EV:%x, AE  : AvgLum=%x %x %x",(ISP_UINT32)ISP_IF_AE_GetDbgData(0), (ISP_UINT32)ISP_IF_AE_GetDbgData(1), (ISP_UINT32)ISP_IF_AE_GetDbgData(2), (ISP_UINT32)ISP_IF_AE_GetDbgData(3));
		VR_PrintString(gDrawTextBuf,  20, 0, 0x0000, 0x0000);
		_sprintf(gDrawTextBuf, (ISP_INT8*)"DBG8:%x, %x, %x, %x, %x",(ISP_UINT32)ISP_IF_IQ_GetID(ISP_IQ_CHECK_CLASS_COLORTEMP),  ISP_IF_IQ_GetID(ISP_IQ_CHECK_CLASS_GAIN),  ISP_IF_IQ_GetID(ISP_IQ_CHECK_CLASS_ENERGY),ISP_IF_AE_GetMetering(),(ISP_UINT32)ISP_IF_AE_GetLightCond());
		VR_PrintString(gDrawTextBuf,  80, 0, 0x0000, 0x0000);
		_sprintf(gDrawTextBuf, (ISP_INT8*)"Avglum:%x, EVTarget%x",(ISP_UINT32)ISP_IF_AE_GetCurrentLum(),  ISP_IF_AE_GetTargetLum());
		VR_PrintString(gDrawTextBuf,  90, 0, 0x0000, 0x0000);
	#endif

	#if 0
		_sprintf(gDrawTextBuf, (ISP_INT8*)"AWB : Mode=%x, GainR=%x, GainGr=%x, GainB=%x, CT = %x,%x", (ISP_UINT32)ISP_IF_AWB_GetMode(), (ISP_UINT32)ISP_IF_AWB_GetGainR(), (ISP_UINT32)ISP_IF_AWB_GetGainG(), (ISP_UINT32)ISP_IF_AWB_GetGainB(),(ISP_UINT32)ISP_IF_AWB_GetColorTemp(),(ISP_UINT32)ISP_IF_AWB_GetMode());
		VR_PrintString(gDrawTextBuf,  30, 0, 0x0000, 0x0000);
	#endif

	#if 0
		{
			ISP_UINT32 *dbgPtr = (ISP_UINT32 *)ISP_IF_AF_GetDbgDataPtr();
			_sprintf(gDrawTextBuf, (ISP_INT8*)"AF  : AFPos=%x, dbg=%x %x %x", (ISP_UINT32)ISP_IF_AF_GetPos(10), (ISP_UINT32)ISP_IF_AF_GetPos(10), dbgPtr[1], dbgPtr[2]);
			VR_PrintString(gDrawTextBuf,  40, 0, 0xFC00, 0x0000);
		}
	#endif
	 }
#endif
}


//==============================================================================
//
//  File        : AHC_Callback.c
//  Description : Callback functions from MMP core
//  Author      : Sunny Chang
//  Revision    : 1.0
//
//==============================================================================

/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "AHC_Config_SDK.h"
#include "AHC_General.h"
#include "MenuSetting.h" 
#include "GPS_ctl.h"
#include "GSensor_ctrl.h" 
#include "Mmps_fs.h" 
#include "AHC_DCFDT.h"

/*===========================================================================
 * Main body
 *===========================================================================*/ 

//==============================================================================
//  Function    : AHC_CheckCallbackExisted
//  Description : This function will be called in AHC_Initialize() to make sure 
//                functions in AHC_Callback.c can work with __weak declare.
//                (It seems a compilation link bug of CodeWarrior.)
//==============================================================================
void AHC_CheckCallbackExisted( void )
{
	//TBD
}

//==============================================================================
//  Function    : AHC_PostInitISP
//  Description : This function will be called in MMP_SensorDrv_PostInitISP() 
//                which will be run after reset ISP in MMPS_Sensor_Initialize().
//                It's used to re-set IPS settings after sensor init.
//==============================================================================
void AHC_PostInitISP( void )
{
    // Note: Below code is just for reference, please DO NOT enable it!!!
        // They will be run in MMPS_Sensor_Initialize()
        #if 0
        // initialize AE mode, min FPS, AWB mode, matering mode, EV bias, face detection status,
        //            AF mode, contrast, sharpness, saturation, image effect, filcker mode, WDR
        ISP_IF_3A_Control(ISP_3A_DISABLE);
        ISP_IF_IQ_SetRunMode(0);
        ISP_IF_AE_SetMinFPS(8);
        ISP_IF_AE_SetISO(0);
        ISP_IF_AE_SetMetering(ISP_AE_METERING_CENTER);
        ISP_IF_AE_SetEVBias(0, 250);
        ISP_IF_AWB_SetMode(ISP_AWB_MODE_AUTO);
        ISP_IF_F_SetSaturation(0);
        ISP_IF_F_SetSharpness(0);
        ISP_IF_F_SetContrast(0);
        VR_AF_SetMode(ISP_AF_MODE_AUTO, 0);
        ISP_IF_F_SetImageEffect(ISP_IMAGE_EFFECT_NORMAL);
        ISP_IF_AE_SetFlicker(ISP_AE_FLICKER_50HZ);
        ISP_IF_AE_SetFPS(0);
        ISP_IF_AE_SetEV(0);
        ISP_IF_F_SetHue(0);
        ISP_IF_F_SetGamma(0);
        ISP_IF_F_SetBrightness(0);
        ISP_IF_F_SetPosterization(255);
        ISP_IF_F_SetZoomRatio(100);
        ISP_IF_F_AdjustColorTemp(0);
        ISP_IF_F_SetColorEffect(ISP_COLOR_EFFECT_NORMAL, 0, 0, 0, 0, 0, 0, 0);
        ISP_IF_F_SetTiltShift(0, 0, 0, 0, 0);
        ISP_IF_F_SetBeautyShot(0, 0, 0, 0, 0);
        ISP_IF_F_SetDOFEffect(0, 0, 0, 0, 0);
        ISP_IF_F_SetWDREn(0);
        ISP_IF_F_SetDark(0);
        VR_Apical_SetWDRCtl(0);
        VR_Apical_SetCCMCtl(0);
        ISP_IF_F_SetAL(ISP_AL_SET, 0x00, 0xFF, 0x20, 0xE0, 1, 1, 10, 15);
        ISP_IF_F_SetAL(ISP_AL_DISABLE, 0, 0, 0, 0, 0, 0, 0, 0);
        #endif

    INT32 s32Setting = 0;
    UINT8  u8Setting = 0;

    if (MenuSettingConfig()->uiEffect != EFFECT_NORMAL)
    {
        Menu_SetEffect(MenuSettingConfig()->uiEffect);
    }
    else
    {
        if (AHC_Menu_SettingGetCB( (char *) COMMON_KEY_CONTRAST, &s32Setting ) == AHC_TRUE) {
            AHC_SetColorContrastLevel(s32Setting);
        }

        if (MenuSettingConfig()->uiColor != COLOR_NATURAL)
        {
            Menu_SetColor(MenuSettingConfig()->uiColor);
        }
        else
        {
            if (AHC_Menu_SettingGetCB( (char *) COMMON_KEY_SATURATION, &s32Setting ) == AHC_TRUE) {
                AHC_SetColorSaturateLevel(s32Setting);
            }
        }

        if (AHC_Menu_SettingGetCB( (char *) COMMON_KEY_SHARPNESS, &s32Setting ) == AHC_TRUE) {
            AHC_SetColorSharpnessLevel(s32Setting);
        }

        if (AHC_Menu_SettingGetCB( (char *) COMMON_KEY_GAMMA, &s32Setting ) == AHC_TRUE) {
            AHC_SetColorGammaLevel(s32Setting);
        }

        if (AHC_Menu_SettingGetCB( (char *) COMMON_KEY_WB, &u8Setting ) == AHC_TRUE) {
            Menu_SetAWB(u8Setting);
        }
    }
    if (AHC_Menu_SettingGetCB( (char *) COMMON_KEY_EV, &u8Setting ) == AHC_TRUE) {
        Menu_SetEV(u8Setting);
    }

    if (AHC_Menu_SettingGetCB( (char *) COMMON_KEY_ISO, &u8Setting ) == AHC_TRUE) {
        Menu_SetISO(u8Setting);
    }
    //MMPS_Sensor_SetIQFunction(MMP_ISP_IQ_FUNC_SET_WDR,MenuSettingConfig()->uiWDR);
    //MMPS_Sensor_SetIQFunction(MMP_ISP_IQ_FUNC_SET_HUE,MenuSettingConfig()->);
}

__weak void AHC_PostEmergencyDone(void)
{
	
	if(AHC_GetVidRecdCardSlowStop())
    {
        AHC_RestartVRMode(AHC_TRUE);
        AHC_SetVidRecdCardSlowStop(AHC_FALSE);
    }
    else if(AHC_GetAPStopVideoRecord())
    {
        #if (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE && GPS_RAW_FILE_ENABLE == 1)
        //if(GPSCtrl_GPSRawWriteFirst_Normal())
        GPSCtrl_GPSRawResetBuffer();
        #endif
        AHC_RestartVRMode(AHC_TRUE);
        AHC_SetAPStopVideoRecord(AHC_FALSE);
    }
    else
    {
		// Add below code to sync GPS / GSensor data with video of next Normal record
        #if (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE && GPS_RAW_FILE_ENABLE == 0 && GSENSOR_RAW_FILE_ENABLE == 0)
        UINT32 ulPreRecordedTime = 0;
        AHC_VideoRecordStartWriteInfo();
        AHC_GetCurrentRecordingTime(&ulPreRecordedTime); //Note: AHC_GetCurrentRecordingTime() can NOT be run in VR Callback functions
		#if (GPS_CONNECT_ENABLE)
        AHC_GPS_TriggerRestorePreRecordInfo( AHC_TRUE, ulPreRecordedTime, AHC_TRUE );
		#endif
		#if (GSENSOR_CONNECT_ENABLE)
        AHC_Gsensor_TriggerRestorePreRecordInfo( AHC_TRUE, ulPreRecordedTime, AHC_TRUE );
		#endif
        #endif

        if( SystemSettingConfig()->byStartNormalRecordAfterEmergencyRecord != 0 )
        {
            AHC_RestartVRMode(AHC_TRUE);
        }
    }
	
}

#if (FS_FORMAT_FREE_ENABLE)
//==============================================================================
//  Function    : AHC_FormatFreeConfigInfo
//  Description : This function will be called before check / format media with 
//                AIT Format Free
//                It will design the folder / file layout of media.
//==============================================================================
void AHC_FormatFreeConfigInfo( void )
{
    FormatFreeFolderConfig   TempFolderConfig;
    
    TempFolderConfig.FileSizeMB = 150; 
    TempFolderConfig.SpacePercentage = 40;
    
    MMPS_FORMATFREE_ConfigInfo( &TempFolderConfig, 0, DCFDT_DB0_FOLDER, "FILE100101-000000.AVI");      
    
    TempFolderConfig.FileSizeMB = 100;
    TempFolderConfig.SpacePercentage = 30;
    
    MMPS_FORMATFREE_ConfigInfo( &TempFolderConfig, 1, DCFDT_DB1_FOLDER, "PARK100101-000000.AVI");  
    
    TempFolderConfig.FileSizeMB = 100;
    TempFolderConfig.SpacePercentage = 20;
    
    MMPS_FORMATFREE_ConfigInfo( &TempFolderConfig, 2, DCFDT_DB2_FOLDER, "EMER100101-000000.AVI");  

    TempFolderConfig.FileSizeMB = 20;
    TempFolderConfig.SpacePercentage = 5;
    
    MMPS_FORMATFREE_ConfigInfo( &TempFolderConfig, 3, DCFDT_DB3_FOLDER, "IMG100101-000000.JPG");  
}

//==============================================================================
//  Function    : AHC_FormatFreeFileNameCB
//  Description : This function will be called when format media with AIT Format Free.
//                It allows to modify default filename to meet DCF file name rule.
//==============================================================================
void AHC_FormatFreeFileNameCB( MMP_ULONG ulFolderIndex, MMP_ULONG ulFileIndex, MMP_BYTE byFileName[] )
{
    #if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
	
    UINT uiStrlen = 0;
    char sSequence[5]; //Add one more byte for '\0'.

    // Change ulFileIndex to format of minute+second
    // Ex: "FILE110101-000079.AVI" --> "FILE110101-000119.AVI"
    
    ulFileIndex = (ulFileIndex/60)*100 + (ulFileIndex%60); 
    sprintf(sSequence, "%04d", ulFileIndex );   
    
    uiStrlen = strlen( byFileName );
    strncpy( byFileName+uiStrlen-8, sSequence, 4); 

    //Because strncpy() will automatically write '\0' in the end, so the '.' will be overwrite. Manually write it back.
    *(byFileName+uiStrlen-4) = 0x2E; //ASCII '.'        
    
    #endif
    //printc("[%d][%d]: %s\r\n", ulFolderIndex, ulFileIndex, byFileName);
}
#endif


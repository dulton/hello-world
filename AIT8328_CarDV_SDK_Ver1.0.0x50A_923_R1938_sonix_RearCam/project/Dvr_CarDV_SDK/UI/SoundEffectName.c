//==============================================================================
//
//  File        : SoundEffectName.c
//  Description : 
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================

/*===========================================================================
 * Include files
 *===========================================================================*/
#include "SoundEffectName.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/


/*===========================================================================
 * Global varible
 *===========================================================================*/


/*===========================================================================
 * Extern function
 *===========================================================================*/


/*===========================================================================
 * Main body
 *===========================================================================*/ 
char* AHC_GetSoundEffectFileName(AHC_SOUNDEFFECT_TYPE soundtype)
{	
	static char path[256];

	#if(STORAGE_FOR_FACTORY_INFO == INFO_NAND_FLASH)
    STRCPY(path, "SM:1:");
    #elif (STORAGE_FOR_FACTORY_INFO == INFO_SERIAL_FLASH)
    STRCPY(path,"SF:0:\\");
    #endif

    switch(soundtype)
    {
    case AHC_SOUNDEFFECT_SHUTTER:
        #ifdef CFG_CUS_SOUNDEFFECT_SHUTTER
        STRCAT(path,CFG_CUS_SOUNDEFFECT_SHUTTER);
        #else
        STRCAT(path,"Shutter.wav");
        #endif
        break;
    case AHC_SOUNDEFFECT_START:
        #ifdef CFG_CUS_SOUNDEFFECT_START
        STRCAT(path,CUS_SOUNDEFFECT_START);
        #else
        STRCAT(path,"start.wav");
        #endif
        break;
    case AHC_SOUNDEFFECT_BUTTON:
        #ifdef CFG_CUS_SOUNDEFFECT_BUTTON
        STRCAT(path,CFG_CUS_SOUNDEFFECT_BUTTON);
        #else
        STRCAT(path,"sound_operation.wav");
        #endif
        break;
    case AHC_SOUNDEFFECT_VR_START:
        #ifdef CFG_CUS_SOUNDEFFECT_VR_START
        STRCAT(path,CFG_CUS_SOUNDEFFECT_VR_START);
        #else
        STRCAT(path,"Record_start.wav");
        #endif
        break;
    case AHC_SOUNDEFFECT_VR_STOP:
        STRCAT(path,"Record_stop.wav");
        break;
    case AHC_SOUNDEFFECT_VR_SAVE:
    case AHC_SOUNDEFFECT_VR_EMER:
        STRCAT(path,"save_record.wav");
        break;
    case AHC_SOUNDEFFECT_FOCUS:
        STRCAT(path,"sound_focus.wav");
        break;
    case AHC_SOUNDEFFECT_ERROR:
        STRCAT(path,"sound_error.wav");
        break;
    case AHC_SOUNDEFFECT_CONFIRM:
        #if CFG_SOUND_EFFECT_SPECIAL_ENABLE
        STRCAT(path,"Record_start.wav");
        #else
        STRCAT(path,"sound_operation.wav");
        #endif
        break;
    case AHC_SOUNDEFFECT_SELFTIMER:
        STRCAT(path,"SelfTimer.wav");
        break;
    case AHC_SOUNDEFFECT_POWERON:
        #ifdef CFG_CUS_SOUNDEFFECT_POWERON
        STRCAT(path,CFG_CUS_SOUNDEFFECT_POWERON);
        #else
        STRCAT(path,"Power_On.wav");
        #endif
        break;
    case AHC_SOUNDEFFECT_POWEROFF:
        #ifdef CFG_CUS_SOUNDEFFECT_POWEROFF
        STRCAT(path,CFG_CUS_SOUNDEFFECT_POWEROFF);
        #else
        STRCAT(path,"Power_Off.wav");
        #endif
        break;
    case AHC_SOUNDEFFECT_SPCAMWARN:
        STRCAT(path,"speedcam_warning.wav");
        break;
    case AHC_SOUNDEFFECT_SPCAMVOICE_CHT:
        #if(C005_TWN_ALERT_VOICE_UPDATE)
        STRCAT(path,"Speedcam_voice_CHT2.wav");
        #else
        STRCAT(path,"Speedcam_voice_CHT.wav");
        #endif
        break;
    case AHC_SOUNDEFFECT_SPCAMVOICE_CHS:
        #if(C005_TWN_ALERT_VOICE_UPDATE)
        STRCAT(path,"Speedcam_voice_CHS.wav");
        #else
        STRCAT(path,"Speedcam_voice_CHS.wav");
        #endif
        break;
    case AHC_SOUNDEFFECT_SPCAMVOICE_ENG:
        #if(C005_TWN_ALERT_VOICE_UPDATE)
        STRCAT(path,"Speedcam_voice_ENG.wav");
        #else
        STRCAT(path,"Speedcam_voice_ENG.wav");
        #endif
        break;
    case AHC_SOUNDEFFECT_SPCAMVOICE_RUS:
        #if(C005_TWN_ALERT_VOICE_UPDATE)
        STRCAT(path,"Speedcam_voice_RUS.wav");
        #else
        STRCAT(path,"Speedcam_voice_RUS.wav");
        #endif
        break;
    case AHC_SOUNDEFFECT_SPCAMRELEASE:
        STRCAT(path,"speedcam_release.wav");
        break;
    case AHC_SOUNDEFFECT_SPCAMOVERSPEED:
        STRCAT(path,"Overspeed.wav");
        break;
    case AHC_SOUNDEFFECT_SPCAMVOICE_15KM_CHT:
        STRCAT(path,"15.wav");
        break;
    case AHC_SOUNDEFFECT_SPCAMVOICE_20KM_CHT:
        STRCAT(path,"20.wav");
        break;
    case AHC_SOUNDEFFECT_SPCAMVOICE_25KM_CHT:
        STRCAT(path,"25.wav");
        break;
    case AHC_SOUNDEFFECT_SPCAMVOICE_30KM_CHT:
        STRCAT(path,"30.wav");
        break;
    case AHC_SOUNDEFFECT_SPCAMVOICE_35KM_CHT:
        STRCAT(path,"35.wav");
        break;
    case AHC_SOUNDEFFECT_SPCAMVOICE_40KM_CHT:
        STRCAT(path,"40.wav");
        break;
    case AHC_SOUNDEFFECT_SPCAMVOICE_50KM_CHT:
        STRCAT(path,"50.wav");
        break;
    case AHC_SOUNDEFFECT_SPCAMVOICE_60KM_CHT:
        STRCAT(path,"60.wav");
        break;
    case AHC_SOUNDEFFECT_SPCAMVOICE_70KM_CHT:
        STRCAT(path,"70.wav");
        break;
    case AHC_SOUNDEFFECT_SPCAMVOICE_80KM_CHT:
        STRCAT(path,"80.wav");
        break;
    case AHC_SOUNDEFFECT_SPCAMVOICE_90KM_CHT:
        STRCAT(path,"90.wav");
        break;
    case AHC_SOUNDEFFECT_SPCAMVOICE_100KM_CHT:
        STRCAT(path,"100.wav");
        break;
    case AHC_SOUNDEFFECT_SPCAMVOICE_110KM_CHT:
        STRCAT(path,"110.wav");
        break;
    case AHC_SOUNDEFFECT_SPCAMVOICE_120KM_CHT:
        STRCAT(path,"120.wav");
        break;
    case AHC_SOUNDEFFECT_NO_SD_CARD:
        STRCAT(path,"no_sd_card_1.wav");
        break;
    case AHC_SOUNDEFFECT_FINDME:
        // for Net findme
        STRCAT(path,"findme.wav");
        break;
    case AHC_SOUNDEFFECT_WIFIFAIL:
        STRCAT(path,"WIFI_Fail.wav");
        break;
    case AHC_SOUNDEFFECT_LDWS_WARNING:
        STRCAT(path,"LDWS_Warning.wav");
        break;
    case AHC_SOUNDEFFECT_FCWS_WARNING:
        STRCAT(path,"FCWS_Warning.wav");
        break;
    case AHC_SOUNDEFFECT_HEADLIGHT_WARNING:
    	STRCAT(path,"HeadLight_Alert_CHT.wav");
    	break;
    case AHC_SOUNDEFFECT_FATIGUE_ALERT:
    	STRCAT(path,"FatigueAlert_CHT.wav");
    	break;
    
    default:
        return NULL;
        break;
    }
    return path;
}

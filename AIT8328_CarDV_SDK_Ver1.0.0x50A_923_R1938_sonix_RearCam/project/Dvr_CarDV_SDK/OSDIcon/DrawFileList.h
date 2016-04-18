//==============================================================================
//
//  File        : DrawFileList.h
//  Description : INCLUDE File for the Draw File.
//                Remove unused icon files could reduce the file hundreds of KB
//                and could reduce link memory requirement and link time.
//                Include your icon only when you need it.
//  Author      :
//  Revision    : 1.0
//
//==============================================================================

#ifndef _DRAW_FILE_LIST_H_
#define _DRAW_FILE_LIST_H_

#include "Customer_Config.h"

#if (MENU_MOVIE_SIZE_EN==1 || OSD_SHOW_MOVIE_SIZE==1)
	#if defined(FONT_LARGE)

	#if MENU_MOVIE_SIZE_SHD_30P_EN
	#include "Icon_MovieSize_SHD_30PL.c"
	#endif
	#if MENU_MOVIE_SIZE_SHD_25P_EN
	#include "Icon_MovieSize_SHD_25P.c"
	#endif
	#if MENU_MOVIE_SIZE_SHD_24P_EN
	#include "Icon_MovieSize_SHD_24P.c"
	#endif
	#if MENU_MOVIE_SIZE_1080_60P_EN
	#include "Icon_MovieSize_1080P_60Large.c"
	#endif
    #if MENU_MOVIE_SIZE_1080_24P_EN
    #include "Icon_MovieSize_1080P_24Large.c"
    #endif
	#if MENU_MOVIE_SIZE_1080P_EN
	#include "Icon_MovieSize_1080PLarge.c"
	#endif
	#if MENU_MOVIE_SIZE_720P_EN
	#include "Icon_MovieSize_720PLarge.c"
	#endif
    #if MENU_MOVIE_SIZE_720_24P_EN
    #include "Icon_MovieSize_720_24PLarge.c"
    #endif
    #if MENU_MOVIE_SIZE_720_60P_EN
	#include "Icon_MovieSize_720_60PLarge.c"
	#endif
	#if MENU_MOVIE_SIZE_VGA30P_EN
	#include "Icon_MovieSize_VGA_30PLarge.c"
	#endif
	#if MENU_MOVIE_SIZE_VGA120P_EN
	#include "Icon_MovieSize_VGA_120P.c"//TBD
	#endif

	#else

	#if MENU_MOVIE_SIZE_SHD_30P_EN
	#include "Icon_MovieSize_SHD_30P.c"
	#endif
	#if MENU_MOVIE_SIZE_SHD_25P_EN
	#include "Icon_MovieSize_SHD_25P.c"
	#endif
	#if MENU_MOVIE_SIZE_SHD_24P_EN
	#include "Icon_MovieSize_SHD_24P.c"
	#endif
	#if MENU_MOVIE_SIZE_1080_60P_EN
	#include "Icon_MovieSize_1080P_60.c"
	#endif
    #if MENU_MOVIE_SIZE_1080_24P_EN
    #include "Icon_MovieSize_1080_24P.c"
    #endif
	#if MENU_MOVIE_SIZE_1080P_EN
	#include "Icon_MovieSize_1080P.c"
	#endif
	#if MENU_MOVIE_SIZE_720P_EN
	#include "Icon_MovieSize_720P.c"
	#endif
    #if MENU_MOVIE_SIZE_720_24P_EN
    #include "Icon_MovieSize_720_24P.c"
    #endif
	#if MENU_MOVIE_SIZE_720_60P_EN
	#include "Icon_MovieSize_720_60P.c"
	#endif
	#if MENU_MOVIE_SIZE_VGA30P_EN
	#include "Icon_MovieSize_VGA_30P.c"
	#endif
	#if MENU_MOVIE_SIZE_VGA120P_EN
	#include "Icon_MovieSize_VGA_120P.c"
	#endif

	#endif

#endif

#if (MENU_STILL_SIZE_EN==1 || OSD_SHOW_IMAGE_SIZE==1)

	#if defined(FONT_LARGE)
	#if MENU_STILL_SIZE_14M_EN
	#include "Icon_ImageSize_14MLarge.c"
	#endif
	#if MENU_STILL_SIZE_12M_EN
	#include "Icon_ImageSize_12MLarge.c"
    #endif
    #if MENU_STILL_SIZE_8M_EN
	#include "Icon_ImageSize_8MLarge.c"
    #endif
    #if MENU_STILL_SIZE_5M_EN
	#include "Icon_ImageSize_5MLarge.c"
    #endif
    #if MENU_STILL_SIZE_3M_EN
	#include "Icon_ImageSize_3MLarge.c"
    #endif
    #if MENU_STILL_SIZE_2M_WIDE_EN
	#include "Icon_ImageSize_2MLarge.c"
    #endif
    #if MENU_STILL_SIZE_1d2M_EN
	#include "Icon_ImageSize_1_2MLarge.c"
    #endif
    #if MENU_STILL_SIZE_VGA_EN
	#include "Icon_ImageSize_VGALarge.c"
	#endif
	#else
    #if MENU_STILL_SIZE_14M_EN
	#include "Icon_ImageSize_14M.c"
    #endif
    #if MENU_STILL_SIZE_12M_EN
	#include "Icon_ImageSize_12M.c"
    #endif
    #if MENU_STILL_SIZE_8M_EN
	#include "Icon_ImageSize_8M.c"
    #endif
    #if MENU_STILL_SIZE_5M_EN
	#include "Icon_ImageSize_5M.c"
    #endif
    #if MENU_STILL_SIZE_3M_EN
	#include "Icon_ImageSize_3M.c"
    #endif
    #if MENU_STILL_SIZE_2M_WIDE_EN
	#include "Icon_ImageSize_2M.c"
    #endif
    #if MENU_STILL_SIZE_1d2M_EN
	#include "Icon_ImageSize_1_2M.c"
    #endif
    #if MENU_STILL_SIZE_VGA_EN
	#include "Icon_ImageSize_VGA.c"
	#endif
	#endif

#endif

#if (MENU_STILL_FLASH_EN==1 || OSD_SHOW_FLASH_STATUS==1)
#if MENU_STILL_FLASH_ON_EN
#include "Icon_Flash_ON.c"
#endif
#if MENU_STILL_FLASH_OFF_EN
#include "Icon_Flash_OFF.c"
#endif
#if MENU_STILL_FLASH_AUTO_EN || OSD_SHOW_FLASH_STATUS
#include "Icon_Flash_Auto.c"
#endif
#endif

#if (MENU_STILL_SELF_TIMER_EN==1 || OSD_SHOW_SELF_TIMER==1)
#include "Icon_Selftimer_2S.c"
#include "Icon_Selftimer_10S.c"
//#include "Icon_Selftimer.c"
#endif

#include "Icon_Num_0.c"
#include "Icon_Num_1.c"
#include "Icon_Num_2.c"
#include "Icon_Num_3.c"
#include "Icon_Num_4.c"
#include "Icon_Num_5.c"
#include "Icon_Num_6.c"
#include "Icon_Num_7.c"
#include "Icon_Num_8.c"
#include "Icon_Num_9.c"

#if (MENU_STILL_BURST_SHOT_EN==1 || OSD_SHOW_BURST_SHOT==1)
#if MENU_STILL_BURST_SHOT_HI_EN
#include "Icon_Burst_High.c"
#endif
#if MENU_STILL_BURST_SHOT_LO_EN
#include "Icon_Burst_Low.c"
#endif
#if MENU_STILL_BURST_SHOT_MID_EN
#include "Icon_Burst_Middle.c"
#endif
#endif

#if (MENU_STILL_STABILIZE_EN==1 || OSD_SHOW_IMAGE_STABLIZATION==1)
#if MENU_STILL_STABILIZE_OFF_EN
#include "Icon_ImageStable_OFF.c"
#endif
#if MENU_STILL_STABILIZE_ON_EN
#include "Icon_ImageStable_ON.c"
#endif
#endif

#if (MENU_MANUAL_SMILE_DETECT_EN==1 || OSD_SHOW_SMILE_DETECTION==1)
#if MENU_MANUAL_SMILE_DETECT_1PEOPLE_EN
#include "Icon_Smile_Detection_1People.c"
#endif
#if MENU_MANUAL_SMILE_DETECT_2PEOPLE_EN
#include "Icon_Smile_Detection_2People.c"
#endif
#if MENU_MANUAL_SMILE_SEN_EN
#include "Icon_Smile_Detection.c"
#endif
#if MENU_MANUAL_SMILE_DETECT_OFF_EN
#include "Icon_Smile_Detection_Off.c"
#endif
#endif

#if (MENU_MANUAL_FACE_DETECT_EN==1 || OSD_SHOW_FACE_DETECTION==1)
#if MENU_MANUAL_FACE_DETECT_TOUCHSPOT_EN
#include "Icon_FaceTouch_TouchSpot.c"
#endif
#if MENU_MANUAL_FACE_DETECT_OFF_EN
#include "Icon_FD_OFF.c"
#endif
#if MENU_MANUAL_FACE_DETECT_ON_EN
#include "Icon_FD_ON.c"
#endif
#endif

#if (MENU_MANUAL_FOCUS_RANGE_EN==1 || OSD_SHOW_FOCAL_RANGE==1)
#include "Icon_Focus_Infinity.c"
#include "Icon_Focus_Marco.c"
#include "Icon_Focus_Window.c"
#endif

#if (MENU_MANUAL_FOCUS_EN==1)
#include "Icon_FocusWindow.c"
#include "Icon_FocusWindow_CentrSpot.c"
#include "Icon_FocusWindow_Multi.c"
#endif

#if (MENU_MANUAL_METERING_EN==1)
#include "Icon_Metering_CenterSpot.c"
#include "Icon_Metering_Multi.c"
#include "Icon_Metering_Weighted.c"
#endif

#if (MENU_MANUAL_SCENE_EN==1 || OSD_SHOW_SCENE_MODE==1)
#if MENU_MANUAL_SCENE_AUTO_EN
#include "Icon_Scene_Auto.c"
#endif
#if MENU_MANUAL_SCENE_BEACH_EN
#include "Icon_Scene_Beach.c"
#endif
#if MENU_MANUAL_SCENE_FIREWORKS_EN
#include "Icon_Scene_Fireworks.c"
#endif
#if MENU_MANUAL_SCENE_LANDSCAPE_EN
#include "Icon_Scene_Landscape.c"
#endif
#if MENU_MANUAL_SCENE_PORTRAIT_EN
#include "Icon_Scene_Portrait.c"
#endif
#if MENU_MANUAL_SCENE_SNOW_EN
#include "Icon_Scene_Snow.c"
#endif
#if MENU_MANUAL_SCENE_SPORT_EN
#include "Icon_Scene_Sport.c"
#endif
#ifdef CFG_MENU_ICON_DEV_TWLIGHT //may be defined in config_xxx.h. It is recommanded to put right after MENU_MANUAL_SCENE_TWILIGHT_EN
#if MENU_MANUAL_SCENE_TWILIGHT_EN
#include "Icon_Scene_Twilight_DEV.c"
#endif
#else
#if MENU_MANUAL_SCENE_TWILIGHT_EN
#include "Icon_Scene_Twilight.c"
#endif
#endif
#if MENU_MANUAL_SCENE_TWILIGHT_PORTRAIT_EN
#include "Icon_Scene_Twilight_Portrait.c"
#endif
#endif

#if (MENU_MANUAL_WB_EN==1 || OSD_SHOW_WB_MODE==1)
#if MENU_MANUAL_WB_CLOUDY_EN
#include "Icon_WB_Cloudy.c"
#endif
#if MENU_MANUAL_WB_DAYLIGHT_EN
#include "Icon_WB_Daylight.c"
#endif
#if MENU_MANUAL_WB_FLASH_EN
#include "Icon_WB_Flash.c"
#endif
#if MENU_MANUAL_WB_FLUORESCENT1_EN
#include "Icon_WB_Fluorescent1.c"
#endif
#if MENU_MANUAL_WB_FLUORESCENT2_EN
#include "Icon_WB_Fluorescent2.c"
#endif
#if MENU_MANUAL_WB_FLUORESCENT3_EN
#include "Icon_WB_Fluorescent3.c"
#endif
#if MENU_MANUAL_WB_INCANDESCENT_EN
#include "Icon_WB_Incandescent.c"
#endif
#if MENU_MANUAL_WB_ONEPUSH_EN
#include "Icon_WB_One_Push.c"
#endif
#if MENU_MANUAL_WB_ONE_PUSH_SET_EN
#include "Icon_WB_One_Push_Set.c"
#endif
#endif

#if (MENU_PLAYBACK_ROTATE_EN==1)
#include "Icon_Rotate_Left.c"
#include "Icon_Rotate_Right.c"
#endif

#if (OSD_SHOW_BATTERY_STATUS==1)
	#if defined(FONT_LARGE)
	#include "Icon_Battery_ExhaustLarge.c"
	#include "Icon_Battery_EmptyLarge.c"
	#include "Icon_Battery_FullLarge.c"
	#include "Icon_Battery_MidLarge.c"
	#include "Icon_Battery_NormalLarge.c"
	//#include "Icon_Battery_Low.c"
	#else
	#include "Icon_Battery_Exhaust.c"
	#include "Icon_Battery_Empty.c"
	#include "Icon_Battery_Full.c"
	#include "Icon_Battery_Mid.c"
	#include "Icon_Battery_Normal.c"
	//#include "Icon_Battery_Low.c"
	#endif

#endif

#if (OSD_SHOW_MEDIA_TYPE==1)

	#if defined(FONT_LARGE)
	#include "Icon_SD_CardLarge.c"
	#include "Icon_SD_Card_NoLarge.c"
	#include "Icon_SD_CardSlowLarge.c"
	#include "Icon_SDCard_ErrorLarge.c"
	#else
	#include "Icon_SD_Card.c"
	#include "Icon_SD_Card_No.c"
	#include "Icon_SD_CardSlow.c"
	#include "Icon_SDCard_Error.c"
	#endif

#endif

/////////////////////////////////////////////////////////////////
/* Menu Catalog Icon */
//#include "Icon_Menu_Edit.c"
//#include "Icon_Menu_Edit_HL.c"
#include "Icon_Menu_General_Settings.c"
#include "Icon_Menu_General_SettingsHL.c"
//#include "Icon_Menu_Manul_Settings.c"
//#include "Icon_Menu_Manul_Settings_HL.c"
#include "Icon_Menu_Media.c"
#include "Icon_Menu_Media_HL.c"
#include "Icon_Menu_Movie.c"
#include "Icon_Menu_Movie_HL.c"
#include "Icon_Menu_Playback.c"
#include "Icon_Menu_Playback_HL.c"
#if DSC_MODE_ENABLE
#include "Icon_Menu_Still.c"
#include "Icon_Menu_Still_HL.c"
#endif

/* Menu Bar Icon */
#if defined(FONT_LARGE)
#include "Icon_MenuBar_WhiteXLarge.c"
#include "Icon_MenuBar_YellowXLarge.c"
#include "Icon_MenuBar_WhiteLarge.c"
#include "Icon_MenuBar_YellowLarge.c"
#include "Icon_MenuBar_White_DefaultLarge.c"
#include "Icon_MenuBar_Yellow_DefaultLarge.c"
#include "Icon_MenuBar_WhiteSmall.c"
#include "Icon_MenuBar_YellowSmall.c"
#else
#include "Icon_MenuBar_White.c"
#include "Icon_MenuBar_Yellow.c"
#include "Icon_MenuBar_White_Default.c"
#include "Icon_MenuBar_Yellow_Default.c"
#include "Icon_SubMenuBar_White.c"
#include "Icon_SubMenuBar_White_Default.c"
#include "Icon_SubMenuBar_Yellow.c"
#include "Icon_SubMenuBar_Yellow_Default.c"
#endif

#if defined(FONT_LARGE)
#include "Icon_MenuBar_White_Big.c"
#include "Icon_MenuBar_Yellow_Big.c"
#include "Icon_MenuBar_White_Default_Big.c"
#include "Icon_MenuBar_Yellow_Default_Big.c"
#endif

#if (UI_MODE_SELECT_EN)||(BROWSER_MODE_SELECT_EN)
/* Menu Mode Selection Icon */
#include "Icon_VideoBroswer.c"
#include "Icon_PhotoBrowser.c"
#endif
#if (UI_MODE_SELECT_EN) || (USB_MODE_SELECT_EN)
#include "Icon_MSDC.c"
#include "Icon_PCCAM.c"
#endif

/* Menu Misc Icon */
#include "Icon_OK.c"
#if (POWER_ON_MENU_SET_EN) || (EXIT_MENU_PAGE_EN)
#include "Icon_Exit.c"
#endif

#include "Icon_D_Down.c"
#include "Icon_D_Left.c"
#include "Icon_D_Right.c"
#include "Icon_D_Up.c"

/* HDMI/TVout Icon */
//#include "Icon_HdmiStillPlay.c"
#include "Icon_HdmiVideoPlay.c"
#include "Icon_HdmiVideoPlayPause.c"
#include "Icon_HdmiVideoPlayStop.c"
#include "Icon_HdmiVideoPlay_FF.c"
#include "Icon_HdmiVideoPlay_FB.c"
#include "Icon_HdmiVideoPlay_PFF.c"
#include "Icon_HdmiVideoPlay_PFB.c"
#include "Icon_HdmiVideoPlay_Play.c"
#include "Icon_HdmiVideoPlay_Pause.c"
#include "Icon_HdmiVideoPlay_Stop.c"
//#include "Icon_HdmiSlideShow.c"

/* Warning/Wait Message Icon */
#include "Icon_WMSG.c"
#include "Icon_Wait0.c"
#include "Icon_Wait1.c"
#include "Icon_Wait2.c"
#include "Icon_Wait3.c"
#include "Icon_Wait4.c"
//#include "Icon_Info.c"

/* Video Record Icon */
#include "Icon_VRPause.c"

#include "Icon_ProtectKey.c"
#include "Icon_ProtectKey_90.c"

#ifdef CFG_MENU_ICON_RED_PROTECT //may be defined in config_xxx.h.
#include "Icon_ProtectKeyRed.c"
#endif

//#include "Icon_mutemic.c"
#include "Icon_mutemicW.c"
#if (MOTION_DETECTION_EN)
#include "Icon_Motion_Detection.c"
#endif

/* UI Mode Icon */
#if defined(FONT_LARGE)
#include "Icon_Button_Movie.c"
#include "Icon_Button_Still.c"
#include "Icon_MovieLarge.c"
#include "Icon_Movie_Playback.c"
#include "Icon_StillLarge.c"
#include "Icon_Still_Playback.c"
#else
#include "Icon_Button_Movie.c"
#include "Icon_Button_Still.c"
#include "Icon_Button_Movie_Playback.c"
//#include "Icon_Button_Still_Playback.c"
#include "Icon_Movie.c"
#include "Icon_Movie_Playback.c"
#include "Icon_Still.c"
#include "Icon_Still_Playback.c"
#endif

/* Other Icon */
#include "Icon_Volume_Decrease.c"
#include "Icon_Volume_Increase.c"

#include "Icon_USB_Connect.c"
#ifdef CFG_MENU_ICON_USB_CONNECT_2 //may be defined in config_xxx.h.
#include "Icon_USB_Connect_2.c"
#endif
#if (SUPPORT_GPS)
#include "Icon_GPSSigs.c"
#endif
#include "Icon_Music.c"
#ifdef CFG_MENU_ICON_REC //may be defined in config_xxx.h.
#include "Icon_REC.c"
#endif

#if (ENABLE_ADAS_LDWS)
#include "Icon_LDWS_Right.c"
#include "Icon_LDWS_Left.c"
#endif

#if (SUPPORT_TOUCH_PANEL)//Un-Used
#include "Icon_Button_Zoom_In.c"
//#include "Icon_Button_Zoom_In_Touch.c"
//#include "Icon_Button_Zoom_Out.c"
//#include "Icon_Button_Zoom_Out_Touch.c"
//#include "Icon_Zoom_In.c"
//#include "Icon_Zoom_Out.c"
//#include "Icon_ZoomBar.c"

#if (MENU_MEDIA_SELECT_MDEIA_EN)
#include "Icon_InternalMem.c"
#endif
//#include "Icon_InternalMem_Error.c"

#if !defined(CAR_DV)
#include "Icon_ProtectKeyG.c"
#include "Icon_ProtectKeyM.c"
#endif
//#include "Icon_ProtectKeyD.c"


//#include "Icon_VR_Volume_On.c"
//#include "Icon_VR_Volume_Off.c"

//#include "Icon_Button_Record.c"
//#include "Icon_Button_Record_Touch.c"
//#include "Icon_Button_Capture.c"
//#include "Icon_Button_Capture_Touch.c"

//#include "Icon_Slide_Show.c"
//#include "Icon_SlideShow_Next.c"
//#include "Icon_Slideshow_Previous.c"

//#include "Icon_Right.c"
#include "Icon_Menu.c"

#include "Icon_Previous.c"
#include "Icon_Next.c"
//#include "Icon_Pause.c"
//#include "Icon_Play.c"
#include "Icon_Play_Pause.c"
#include "Icon_Play_Play.c"
#include "Icon_Return.c"

//#include "Icon_Protect.c"
//#include "Icon_Delete.c"
//#include "Icon_Cut.c"
//#include "Icon_Button_Cut.c"

#include "Icon_Movie_Fast_Forward.c"
#include "Icon_Movie_Fast_Reverse.c"

#endif

#include "Icon_Dummy.c"

#if defined(WIFI_PORT) && (WIFI_PORT == 1)
// WiFi Icons
#include "Icon_WiFi.c"		// AP Connected
//#include "Icon_WiFi00.c"	// AP Disconnected
#include "Icon_WiFi01.c"	// rtsp running
#include "Icon_WiFi02.c"	// rtsp running
#include "Icon_WiFi03.c"	// rtsp running
#include "Icon_WiFi_ERR.c"	// rtsp ERROR
#include "Icon_WiFi_FAIL.c"	// init WIFI failed

#include "Icon_WiFi_On.c"		// AP Connected
#include "Icon_WiFi_IDLE.c"		// AP Disconnected
//#include "Icon_WiFi_Setting.c"	// rtsp running
#include "Icon_WiFi_Streaming_On.c"	// rtsp running
#include "Icon_WiFi_Streaming_Off.c"	// rtsp running
#include "Icon_WiFi_Disconnect.c"
#include "Icon_WiFiNoIP.c"	// No IP or Crypto type(key) error
#endif

#if (defined(VIDEO_REC_TIMELAPSE_EN) && VIDEO_REC_TIMELAPSE_EN)
#include "TV_TimeLapseB.c"
#endif

#if (MENU_MOVIE_HDR_MODE_EN)
#include "Icon_HDR.c"
//#include "Icon_HDR_OFF.c"
#endif

#include "DamageVideoFile.c"
#include "reverse_line.c"

#endif //_DRAW_FILE_LIST_H_

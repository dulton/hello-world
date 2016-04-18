//==============================================================================
//
//  File        : AHC_Message.h
//  Description : INCLUDE File for the AHC message definition and data types.
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================

#ifndef _AHC_MESSAGE_H_
#define _AHC_MESSAGE_H_

/*===========================================================================
 * Macro define
 *===========================================================================*/

#define BUTTON_RELEASE_OFFSET                    	100
#define BUTTON_MOVE_OFFSET                       	200
#define TOUCH_PRESS_ID_INDEX                      	0x100
#define TOUCH_MOVE_ID_INDEX                      	(TOUCH_PRESS_ID_INDEX + BUTTON_RELEASE_OFFSET)

#define BUTTON_PRESS_ID(x)                       	(x)
#define BUTTON_RELEASE_ID(x)                     	(x + BUTTON_RELEASE_OFFSET)
#define TOUCH_PRESS_ID(x)                        	(x + TOUCH_PRESS_ID_INDEX)
#define TOUCH_MOVE_ID(x)                         	(x + TOUCH_MOVE_ID_INDEX)

#define INDEX_TOUCH_OFFSET       					0x100

/*===========================================================================
 * Enum define
 *===========================================================================*/

typedef enum _AHC_MESSAGE {
	AHLM_USER = 0x0,		
	AHLM_AE_CONVERGED,
	AHLM_CREATE,
	AHLM_NEW_FRAME_PROCESSED,
	AHLM_FLASH_CARD_DETECT,
	AHLM_NEW_MODE,
	AHLM_OPERATION_COMPLETE,
	AHLM_AWB_CONVERGED,
	AHLM_PIP_EFFECT_COMPLETE,
	AHLM_USB_SUSPEND,
	AHLM_USB_RESUME,
	AHLM_AUDIO_EXISTS,
	AHLM_NOT_ENOUGH_FLASH_MEMORY,
	AHLM_BKG_STORE_COMPLETE,
	AHLM_PTP_POWER_DOWN,
	AHLM_END_OF_FILE,
	AHLM_DATA_STAMP,
	AHLM_VALID_IMAGE,
	AHLM_MOUNT_COMPLETED,
	AHLM_MOUNT_DUPLICATED_FOUND,
	AHLM_USB_MODE_CHANGE,
	AHLM_DIGITAL_ZOOM_COMPLETE,
	AHLM_FO_COMPLETED,
	AHLM_GET_MEDIA_STATUS_COMPLETED,
	AHLM_AVI_AUDIO_EXIST,
	AHLM_MASS_STORAGE,
	AHLM_WINDOW_AF_READY,
	AHLM_SUBWINDOW_AF_READY,
	AHLM_TIMER_INDEX,
	AHLM_ADCX_NOTIFICATION,
	AHLM_GPIO_BUTTON_NOTIFICATION,
	AHLM_LGPIO_BUTTON_NOTIFICATION,
	AHLM_SGPIO_BUTTON_NOTIFICATION,
	AHLM_USB_CONFIGURED,
	AHLM_DISPLAY_WINDOW_COMPLETE,
	AHLM_CAPTURE_RAW_READY,
	AHLM_LOW_BATTERY,
	AHLM_PWRC_BUTTON_NOTIFICATION,
	AHLM_MOVIE_PLAY_OPERATION_COMPLETE,
	AHLM_WAV_OPERATION_COMPLETE,
	AHLM_MOTOR_SET_COMPLETE,
	AHLM_VIDEOMODE_TRANSITION_COMPLETE,
	AHLM_MASS_STORAGE_UFI_COMMAND,
	AHLM_UART_LIB,
	AHLM_MASS_STORAGE_MEDIA_EJECTED,
	AHLM_DPS_MSG,
	AHLM_STORE_FINISHED,
	AHLM_MPEG_CAPTURE_COMPLETE,
	AHLM_MPEG_PLAY_COMPLETE,
	AHLM_CAPTURE_NIGHT_MODE_START,
	AHLM_SLOW_MEDIA_ERROR,
	AHLM_MPEG_PLAY_FAILURE,
	AHLM_EDIT_MOVIE_COMPLETE,
	AHLM_MASS_STORAGE_UFI_COMMAND_ERROR,
	AHLM_MPEG_SNAPSHOT_COMPLETE,
	AHLM_PTP_REQUEST,
	AHLM_PTP_STATUS,
	AHLM_MJPEG_MEDIA_WRITE_ERROR,
	AHLM_MPEG_CUMULATED_SAMPLE_BYTES,
	AHLM_MEDIA_READY,
	AHLM_PTPDB_EXISTS,
	AHLM_CLIP_MOVIE_TIMEOUT,
	AHLM_MJPEG_RECORD_CURR_CHUNK,
	AHLM_DPS_PRINTER_DISCOVERED,
	AHLM_PTP_CMD_IN_PROCESS,
	AHLM_FLASH_CARD_STATUS,
	AHLM_CEC_DISCOVERY,
	AHLM_CEC_CMD_RECEIVED,
	AHLM_CEC_CMD_SENT,
	AHLM_RAW_BUFFER_ALLOC_FAILED,
	AHLM_PLAYBACK_CURR_AUDIO_CHUNK,
	AHLM_STILL_CAPTURE_SEQUENCE_COMPLETE,
	AHLM_MOVIE_FRAME_SNAPSHOT_DECODE_COMPLETE,
	AHLM_MPEG_SNAPSHOT_ALLOCATION_FAIL,
	AHLM_DPS_DBG_DEVICE_STATUS,
	AHLM_MPEG_FILE_CLOSED,
	AHLM_AAC_PLAYBACK_INIT_DONE,
	AHLM_PLAYBACK_CURR_WAV_CHUNK, 
	AHLM_PLAYBACK_SECOND_WAV_CHUNK,
	AHLM_THUMB_SCALING_DONE,
	AHLM_FLASH_LIGHT_REQUIRED,
	AHLM_STORE_BEGIN,
	AHLM_VALID_AUDIO,
	AHLM_SEND_CONFIRM_STORE,
	AHLM_PCC_FINISHED,
	AHLM_PLAYBACK_CURR_CHUNK,
	AHLM_NO_MORE_DCF_OBJECTS,
	AHLM_DPS_DBG_SERVICE_STATUS,
	AHLM_DPS_DBG_ERROR_STATUS,
	AHLM_DPS_DBG_ERROR_REASON,
	AHLM_DPS_DBG_JOB_END_REASON,
	AHLM_DPS_DBG_XML_REQUEST_LENGTH,
	AHLM_DPS_DBG_DISCONNECT_ENABLE,
	AHLM_DPS_DBG_CAPABILITY_CHANGED,
	AHLM_DPS_DBG_NEW_JOB_OK,
	AHLM_PTP_RECV_FILE,
	AHLM_MOV_NUM_OF_FRAME_DROPPED,
	AHLM_PIP_COPY_DONE,
	AHLM_FLASH_IO_ERROR,
	AHLM_XML_BUFFER_EXCEEDED,
	AHLM_MASS_STORAGE_ATTACHED,
	AHLM_MPEG_PLAYBACK_ERROR,
	AHLM_RCC_FINISHED,
	AHLM_PIP_LAYER_HIDDEN,
	AHLM_RECORD_CURR_CHUNK,
	AHLM_PTP_DATA_RECEIVED,
	AHLM_BOOT_COMPLETE,
	AHLM_INIT_COMPLETE,
	AHLM_DCF_LAST_FILE_FOUND,
	AHLM_DCF_CREATE_COMPLETED,
	AHLM_DCF_WINDOW_CREATED,
	AHLM_REACHED_MAX_FILESIZE,
	AHLM_UI_NOTIFICATION,
	AHLM_BURNIN_TEST,
	AHLM_WIRELESS_CONNECT,
	AHLM_SYSTEM_INITIALIZATION,
	AHLM_MAX
}AHC_MESSAGE; 

typedef enum{
    UART_CMD_SUCCESS = 0,
    UART_CMD_FAIL,
    UART_CMD_START,
    UART_CMD_END
}UART_CMD_MESSAGE;

typedef enum{
    WIRELESS_CONNECT_SUCCESS = 0,
    WIRELESS_CONNECT_FAIL,
    WIRELESS_CONNECT_TIMEOUT,
    WIRELESS_CAPTURE,
	WIRELESS_RECORD,
	WIRELESS_SET_FLICKER,
	WIRELESS_SET_AWB,
	WIRELESS_SET_EV,
	WIRELESS_SET_MOVIE_SIZE,
	WIRELESS_SET_CLIP_TIME,
	WIRELESS_SET_VQUALITY,
	WIRELESS_SET_IMAGE_SIZE,
	WIRELESS_SET_POWROFF_DELAY,
	WIRELESS_SET_AUTO_PWROFF_TIME,
	WIRELESS_SET_MTD,
	WIRELESS_CHANGE_UISTATE_REQ,
	WIRELESS_SET_TIME_FMT,
	WIRELESS_SET_TV_SYS,
	WIRELESS_SET_LCD_PWRS,
	WIRELESS_SET_PHOTOBURST,
	WIRELESS_SET_TIMELAPSE,
	WIRELESS_SET_UPSIDEDOWN,
	WIRELESS_SET_SPOTMETER,
	WIRELESS_SET_LOOPINGVIDEO,
	WIRELESS_SET_HDR,
	WIRELESS_SET_Q_SHOT,
	WIRELESS_SET_STATUSLIGHTS,
	WIRELESS_SET_SOUNDINDICATOR,
	WIRELESS_SET_POWERSAVING,
	WIRELESS_SET_UIMODE,
	WIRELESS_FORMAT_SD0,		// For Wi-Fi
	WIRELESS_NETWORK_READY,
	WIRELESS_NETWORK_IP_READY,
	WIRELESS_NETWORK_WPS_FAIL,
	WIRELESS_SET_STREAMING_MODE,
	WIRELESS_ENTER_PLAYBACK,
	WIRELESS_EXIT_PLAYBACK,
	WIRELESS_SET_END
}WIRELESS_CONNECT_MESSAGE;

typedef enum _KeyEventIndex{
    EVENT_INDEX_NONE             = 0x0000  ,
    EVENT_INDEX_DSC              = 0x1000  ,
    EVENT_INDEX_DV               = 0x2000  ,
    EVENT_INDEX_THUMBNAIL        = 0x3000  ,
    EVENT_INDEX_VIDEO_PB         = 0x4000  ,
    EVENT_INDEX_PHOTO_PB         = 0x4300  ,
    EVENT_INDEX_SLIDESHOW        = 0x4700  ,
    EVENT_INDEX_MSDC             = 0x5000  ,
    EVENT_INDEX_HDMI             = 0x5100  ,
    EVENT_INDEX_TV               = 0x5200  ,
// File Access Message
    EVENT_INDEX_FS               = 0x6000  ,
// Other System State Message
    EVENT_INDEX_SYS              = 0x7000  ,

    EVENT_INDEX_MAX              = 0xFFFF
}KeyEventIndex;

typedef enum _KeyEventID {
    //Index 0
    EVENT_NONE              = EVENT_INDEX_NONE ,
    EVENT_NONE2              	               ,

    /* Key Event.*/                                                                                      
    EVENT_KEY_START                            ,    // Keep at the first of key event group
    EVENT_KEY_MENU = EVENT_KEY_START           ,
    EVENT_KEY_SET                              ,
    EVENT_KEY_MODE                             ,
    //Index 5
    EVENT_KEY_PLAYBACK_MODE                    ,
    EVENT_KEY_REC                              ,

    EVENT_KEY_UP                               ,
    EVENT_KEY_DOWN                             ,

    //Index 10
    EVENT_KEY_LEFT                             ,
    EVENT_KEY_LLEFT                            ,
    EVENT_KEY_LEFT_REL                         ,
    EVENT_KEY_LEFT1                            ,

    EVENT_KEY_RIGHT                            ,
    //Index 15
    EVENT_KEY_LRIGHT                           ,
    EVENT_KEY_RIGHT_REL                        ,
    EVENT_KEY_RIGHT1                           ,

    EVENT_KEY_CIRCLE                           ,
    EVENT_KEY_CIRCLE_RELEASE                   ,

    //Index 20
    EVENT_KEY_TELE_PRESS                       ,
    EVENT_KEY_WIDE_PRESS                       ,
    EVENT_KEY_TELE_STOP                        ,
    EVENT_KEY_WIDE_STOP                        ,

    EVENT_KEY_ZOOM_STATUS_CLEAR                ,
    //Index 25
    EVENT_KEY_DISP_PRESS                       ,
    EVENT_KEY_END                              ,      // Keep at the last of key event group

    /* Gpio Event.*/                              
    EVENT_POWER_ON = EVENT_KEY_END             ,
    EVENT_POWER_OFF                            ,
    EVENT_CLOSE_BACKLIGHT                      ,
    EVENT_SD_DETECT                            ,
    EVENT_SD_REMOVED                           ,
    EVENT_SD2_DETECT                           ,
    EVENT_SD2_REMOVED                          ,
    //Index 32
    EVENT_USB_DETECT                           ,
    EVENT_USB_REMOVED                          ,
    EVENT_HDMI_DETECT                          ,
    EVENT_HDMI_REMOVED                         ,
    EVENT_TV_DETECT                            ,
    //Index 37
    EVENT_TV_REMOVED                           ,
    EVENT_LCD_COVER_OPEN                       ,
    EVENT_LCD_COVER_CLOSE                      ,
    EVENT_LCD_COVER_NORMAL                     ,
    EVENT_LCD_COVER_ROTATE                     ,
    //Index 42    
    EVENT_DC_CABLE_IN						   ,
    EVENT_DC_CABLE_OUT						   ,
    EVENT_FOCUS_RANGE                          ,

    EVENT_VIRTUAL_POWER_ON                     ,
    EVENT_VIRTUAL_POWER_OFF                    ,

    EVENT_USB_B_DEVICE_DETECT                  ,
    EVENT_USB_B_DEVICE_REMOVED                 ,
    EVENT_USB_B_DEVICE_RESET_DONE              ,
    EVNET_USB_B_DEVICE_RESET_START             ,
    //  Capture Mode 

    /* Key Event.*/
    EVENT_DSC_PREVIEW_MODE = EVENT_INDEX_DSC   ,
    EVENT_DSC_PREVIEW_INIT                     ,
    EVENT_DSC_KEY_FOCUS                        ,
    EVENT_DSC_KEY_FOCUS_STATUS_CLEAR           ,
    EVENT_DSC_KEY_CAPTURE                      ,
    EVENT_DSC_KEY_CAPTURE_STATUS_CLEAR         ,
    EVENT_DSC_KEY_FOCUS_CAPTURE                ,
    EVENT_DSC_KEY_UPDATE_MESSAGE               ,
    EVENT_DSC_SET_EV                           ,
    EVENT_DSC_KEY_CHANGE_SHOT_MODE             ,



    /* Touch Event.*/
    EVENT_DSC_TOUCH_MENU  = EVENT_INDEX_DSC+INDEX_TOUCH_OFFSET+1,
    EVENT_DSC_TOUCH_MODE                       ,
    EVENT_DSC_TOUCH_FOCUS                      ,
    EVENT_DSC_TOUCH_PLAYBACK_MODE              ,

    EVENT_DSC_TOUCH_WIDE_PRESS                 ,
    EVENT_DSC_TOUCH_WIDE_MOVE                  ,
    EVENT_DSC_TOUCH_WIDE_RELEASE               , 

    EVENT_DSC_TOUCH_TELE_PRESS                 ,
    EVENT_DSC_TOUCH_TELE_MOVE                  ,
    EVENT_DSC_TOUCH_TELE_RELEASE               ,

    EVENT_DSC_TOUCH_ZOOM_STOP                  ,
    EVENT_DSC_TOUCH_ZOOM_STATUS_CLEAR          , 

    EVENT_DSC_TOUCH_CAPTURE_PRESS              ,
    EVENT_DSC_TOUCH_CAPTURE_CLEAR              ,
    EVENT_DSC_TOUCH_CAPTURE_RELEASE            ,

    EVENT_DSC_TOUCH_SWITCH_MAINPAGE            ,
    EVENT_DSC_TOUCH_SWITCH_CONTROLPAGE         ,

    //  Video Mode

    /* Key Event.*/  
    EVENT_VIDEO_PREVIEW_INIT =  EVENT_INDEX_DV ,
    EVENT_VIDEO_PREVIEW_MODE                   ,
    EVENT_VIDEO_KEY_FOCUS                      ,
    EVENT_VIDEO_KEY_FOCUS_STATUS_CLEAR         ,
    EVENT_VIDEO_KEY_CAPTURE                    ,
    EVENT_VIDEO_KEY_CAPTURE_STATUS_CLEAR       ,
    EVENT_VIDEO_KEY_FOCUS_CAPTURE              ,
    EVENT_VIDEO_KEY_RECORD                     ,
    EVENT_VIDEO_KEY_RECORD_START			   ,
    EVENT_VIDEO_KEY_RECORD_STOP                ,
    EVENT_VIDEO_KEY_RECORD_MUTE                ,
    EVENT_VIDEO_KEY_SWAP_PIP			   	   ,

    EVENT_VRCB_RECDSTOP_CARDSLOW               ,
    EVENT_VRCB_AP_STOP_VIDEO_RECD              ,
    EVENT_VRCB_MEDIA_FULL                      ,
    EVENT_VRCB_FILE_FULL                       ,
    EVENT_VRCB_LONG_TIME_FILE_FULL             ,
    EVENT_VRCB_MEDIA_SLOW                      ,
    EVENT_VRCB_MEDIA_ERROR                     ,    
    EVENT_VRCB_SEAM_LESS					   ,
    EVENT_VRCB_MOTION_DTC					   ,
    EVENT_VRCB_EMER_DONE					   ,
    EVENT_VR_START							   ,
    EVENT_VR_STOP							   ,
    EVENT_VR_WRITEINFO						   ,
    EVENT_VR_EMERGENT                          , 
    EVENT_VR_CONFIRM_EMERGENT                  , 
    EVENT_VR_UPDATE_EMERGENT                   , 
    EVENT_VR_CANCEL_EMERGENT                   , 
    EVENT_VR_STOP_EMERGENT                     , 
    EVENT_VR_DRIVER_FATIGUE_ALERT              ,
    EVENT_VR_REMIND_HEADLIGHT_ALERT            ,
    EVENT_VR_VIDEO_RECORD_INIT    		       ,

    //TBD
    EVENT_RECORD_BLINK_UI       		       ,

    EVENT_RECORD_MUTE						   ,
    EVENT_FORMAT_MEDIA						   ,	
    EVENT_CHANGE_NIGHT_MODE					   ,
    EVENT_LCD_POWER_SAVE					   ,
    EVENT_CHANGE_LED_MODE					   ,
    EVENT_VIDEO_PREVIEW						   ,
    EVENT_VIDEO_BROWSER						   ,
    EVENT_CAMERA_PREVIEW					   ,
    EVENT_CAMERA_BROWSER					   ,
    EVENT_ALL_BROWSER						   ,
    EVENT_LOCK_VR_FILE						   ,
    EVENT_SHOW_FW_VERSION					   ,
    EVENT_FORMAT_RESET_ALL					   ,
    EVENT_SHOW_GPS_INFO						   ,
    EVENT_UPDATE_CHARGE_ICON				   ,
    EVENT_AUDIO_RECORD						   ,
    EVENT_MENU_RETURN						   ,
    EVENT_SWITCH_VMD_MODE					   ,
    EVENT_SWITCH_TIME_LAPSE_EN				   ,
    EVENT_SWITCH_PANEL_TVOUT				   ,
    EVENT_SWITCH_TOUCH_PAGE					   ,
    EVENT_EV_INCREASE						   ,
    EVENT_EV_DECREASE						   ,
    EVENT_VR_OSD_SHOW                          ,
    EVENT_VR_OSD_HIDE                          ,
    EVENT_BROWSER_SELECT_MENU				   ,
    EVENT_VIDEO_STILLCAPTURE_VIDEO             ,

    /* Touch Event.*/
    EVENT_VIDEO_TOUCH_MENU  = EVENT_INDEX_DV+INDEX_TOUCH_OFFSET+1,
    EVENT_VIDEO_TOUCH_MODE                     ,
    EVENT_VIDEO_TOUCH_FOCUS                    ,
    EVENT_VIDEO_TOUCH_PLAYBACK_MODE            ,

    EVENT_VIDEO_TOUCH_WIDE_PRESS               ,
    EVENT_VIDEO_TOUCH_WIDE_MOVE                ,
    EVENT_VIDEO_TOUCH_WIDE_RELEASE             ,

    EVENT_VIDEO_TOUCH_TELE_PRESS               ,
    EVENT_VIDEO_TOUCH_TELE_MOVE                ,
    EVENT_VIDEO_TOUCH_TELE_RELEASE             ,

    EVENT_VIDEO_TOUCH_ZOOM_STOP                ,
    EVENT_VIDEO_TOUCH_ZOOM_STATUS_CLEAR        ,

    EVENT_VIDEO_TOUCH_RECORD_PRESS             ,
    EVENT_VIDEO_TOUCH_RECORD_STOP              ,
    EVENT_VIDEO_TOUCH_RECORD_RELEASE           ,

    EVENT_VIDEO_TOUCH_SWITCH_MAINPAGE          ,
    EVENT_VIDEO_TOUCH_SWITCH_CONTROLPAGE       ,

    EVENT_VIDEO_TOUCH_PARK                     ,
    EVENT_VIDEO_TOUCH_CAMERA                   ,
    EVENT_VIDEO_TOUCH_EMERGENCY                ,
    EVENT_VIDEO_TOUCH_STOP_EMERGENT            , 
    EVENT_VIDEO_TOUCH_YES                      ,	
    EVENT_VIDEO_TOUCH_NO                       ,
    EVENT_VIDEO_TOUCH_NONE                     ,

    EVENT_VIDREC_UPDATE_MESSAGE                ,

    //  Browser Mode

    /* Key Event.*/ 
    EVENT_THUMBNAIL_MODE_INIT = EVENT_INDEX_THUMBNAIL ,
    EVENT_THUMBNAIL_MODE                       ,
    EVENT_THUMBNAIL_KEY_PAGE_UP                ,
    EVENT_THUMBNAIL_KEY_PAGE_DOWN              ,
    EVENT_THUMBNAIL_KEY_PAGE_UPDATE            ,
    EVENT_THUMBNAIL_KEY_PLAYBACK               ,

    /* Touch Event.*/
    EVENT_THUMBNAIL_TOUCH_MENU = EVENT_INDEX_THUMBNAIL+INDEX_TOUCH_OFFSET+1,
    EVENT_THUMBNAIL_TOUCH_MODE                 ,
    EVENT_THUMBNAIL_TOUCH_PLAYBACK_MODE        ,
    EVENT_THUMBNAIL_TOUCH_PAGE_UP              ,
    EVENT_THUMBNAIL_TOUCH_PAGE_UP_RELEASE      ,
    EVENT_THUMBNAIL_TOUCH_PAGE_DOWN            ,
    EVENT_THUMBNAIL_TOUCH_PAGE_DOWN_RELEASE    ,
    EVENT_THUMBNAIL_TOUCH_PAGE_UPDATE          ,
    EVENT_THUMBNAIL_TOUCH_PS_FILE01            ,
    EVENT_THUMBNAIL_TOUCH_PS_FILE02            ,
    EVENT_THUMBNAIL_TOUCH_PS_FILE03            ,
    EVENT_THUMBNAIL_TOUCH_PS_FILE04            ,
    EVENT_THUMBNAIL_TOUCH_PS_FILE05            ,
    EVENT_THUMBNAIL_TOUCH_PS_FILE06            ,
    EVENT_THUMBNAIL_TOUCH_MV_FILE01            ,
    EVENT_THUMBNAIL_TOUCH_MV_FILE02            ,
    EVENT_THUMBNAIL_TOUCH_MV_FILE03            ,
    EVENT_THUMBNAIL_TOUCH_MV_FILE04            ,
    EVENT_THUMBNAIL_TOUCH_MV_FILE05            ,
    EVENT_THUMBNAIL_TOUCH_MV_FILE06            ,
    EVENT_THUMBNAIL_HDMI_INFO                  ,
    EVENT_THUMBNAIL_UPDATE_MESSAGE			   ,

    //  Video Playback Mode

    /* Key Event.*/   
    EVENT_MOVPB_MODE_INIT = EVENT_INDEX_VIDEO_PB ,
    EVENT_MOVPB_MODE                           ,
    EVENT_MOVPB_VOLUMN_UP                      ,
    EVENT_MOVPB_VOLUMN_DOWN                    ,
    EVENT_MOVPB_OSD_SHOW                       ,
    EVENT_MOVPB_OSD_HIDE                       ,
    EVENT_MOVPB_MEDIA_ERROR                    ,

    /* Touch Event.*/
    EVENT_MOVPB_TOUCH_RETURN = EVENT_INDEX_VIDEO_PB+INDEX_TOUCH_OFFSET+1,
    EVENT_MOVPB_TOUCH_PREV_PRESS               ,
    EVENT_MOVPB_TOUCH_PREV_MOVE                ,
    EVENT_MOVPB_TOUCH_PREV_RELEASE             ,

    EVENT_MOVPB_TOUCH_NEXT_PRESS               ,
    EVENT_MOVPB_TOUCH_NEXT_MOVE                ,
    EVENT_MOVPB_TOUCH_NEXT_RELEASE             ,

    EVENT_MOVPB_TOUCH_PLAY_PRESS               ,
    EVENT_MOVPB_TOUCH_PLAY_MOVE                ,
    EVENT_MOVPB_TOUCH_PLAY_RELEASE             ,

    EVENT_MOVPB_TOUCH_PAUSE_PRESS              ,
    EVENT_MOVPB_TOUCH_PAUSE_PRESS_2            ,
    EVENT_MOVPB_TOUCH_PAUSE_MOVE               ,
    EVENT_MOVPB_TOUCH_PAUSE_RELEASE            ,

    EVENT_MOVPB_TOUCH_BKWD_PRESS               ,
    EVENT_MOVPB_TOUCH_BKWD_MOVE                ,
    EVENT_MOVPB_TOUCH_BKWD_RELEASE             ,

    EVENT_MOVPB_TOUCH_FRWD_PRESS               ,
    EVENT_MOVPB_TOUCH_FRWD_MOVE                ,
    EVENT_MOVPB_TOUCH_FRWD_RELEASE             ,

    EVENT_MOVPB_TOUCH_ROTATE_PRESS             ,
    EVENT_MOVPB_TOUCH_ROTATE_MOVE              ,
    EVENT_MOVPB_TOUCH_ROTATE_RELEASE           ,

    EVENT_MOVPB_TOUCH_DEL_PRESS                ,
    EVENT_MOVPB_TOUCH_DEL_MOVE                 ,
    EVENT_MOVPB_TOUCH_DEL_RELEASE              ,

    EVENT_MOVPB_TOUCH_DEL_YES_PRESS                ,
    EVENT_MOVPB_TOUCH_DEL_YES_MOVE                 ,
    EVENT_MOVPB_TOUCH_DEL_YES_RELEASE              ,

    EVENT_MOVPB_TOUCH_DEL_NO_PRESS                ,
    EVENT_MOVPB_TOUCH_DEL_NO_MOVE                 ,
    EVENT_MOVPB_TOUCH_DEL_NO_RELEASE              ,

    EVENT_MOVPB_TOUCH_FULL_PAGE_PRESS                ,
    EVENT_MOVPB_TOUCH_FULL_PAGE_MOVE                 ,
    EVENT_MOVPB_TOUCH_FULL_PAGE_RELEASE              ,

    EVENT_MOVPB_TOUCH_COME2EMER_PRESS                ,
    EVENT_MOVPB_TOUCH_COME2EMER_MOVE                 ,
    EVENT_MOVPB_TOUCH_COME2EMER_RELEASE              ,

    EVENT_MOVPB_TOUCH_OPTION                   ,
    EVENT_MOVPB_UPDATE_MESSAGE                 ,
	EVENT_MOVPB_PLAY_STOP_MESSAGE				   ,

    //  Photo Playback Mode

    /* Key Event.*/ 
    EVENT_JPGPB_MODE_INIT = EVENT_INDEX_PHOTO_PB ,
    EVENT_JPGPB_MODE                           ,
    EVENT_JPGPB_KEY_ZOOMIN                     ,
    EVENT_JPGPB_KEY_ZOOMOUT                    ,
    EVENT_JPGPB_OSD_SHOW                       ,
    EVENT_JPGPB_OSD_HIDE                       ,

    /* Touch Event.*/
    EVENT_JPGPB_TOUCH_RETURN = EVENT_INDEX_PHOTO_PB+INDEX_TOUCH_OFFSET+1,
    EVENT_JPGPB_TOUCH_PREV_PRESS               ,
    EVENT_JPGPB_TOUCH_PREV_MOVE                ,
    EVENT_JPGPB_TOUCH_PREV_RELEASE             ,

    EVENT_JPGPB_TOUCH_NEXT_PRESS               ,
    EVENT_JPGPB_TOUCH_NEXT_MOVE                ,
    EVENT_JPGPB_TOUCH_NEXT_RELEASE             ,

    EVENT_JPGPB_TOUCH_ROTATE_PRESS             ,
    EVENT_JPGPB_TOUCH_ROTATE_MOVE              ,
    EVENT_JPGPB_TOUCH_ROTATE_RELEASE           ,

    EVENT_JPGPB_TOUCH_HDMI_OUTPUT              ,

    EVENT_JPGPB_TOUCH_ZOOMIN_PRESS             ,
    EVENT_JPGPB_TOUCH_ZOOMIN_MOVE              ,
    EVENT_JPGPB_TOUCH_ZOOMIN_RELEASE           ,

    EVENT_JPGPB_TOUCH_ZOOMOUT_PRESS            ,
    EVENT_JPGPB_TOUCH_ZOOMOUT_MOVE             ,
    EVENT_JPGPB_TOUCH_ZOOMOUT_RELEASE          ,

    EVENT_JPGPB_TOUCH_DEL_PRESS            ,
    EVENT_JPGPB_TOUCH_DEL_MOVE             ,
    EVENT_JPGPB_TOUCH_DEL_RELEASE          ,

    EVENT_JPGPB_TOUCH_DEL_YES_PRESS                ,
    EVENT_JPGPB_TOUCH_DEL_YES_MOVE                 ,
    EVENT_JPGPB_TOUCH_DEL_YES_RELEASE              ,

    EVENT_JPGPB_TOUCH_DEL_NO_PRESS                ,
    EVENT_JPGPB_TOUCH_DEL_NO_MOVE                 ,
    EVENT_JPGPB_TOUCH_DEL_NO_RELEASE              ,

    EVENT_JPGPB_TOUCH_OPTION                   ,
    EVENT_JPGPB_UPDATE_MESSAGE                 ,

    //  Slide Show Mode

    /* Key Event.*/ 
    EVENT_SLIDESHOW_MODE_INIT = EVENT_INDEX_SLIDESHOW ,
    EVENT_SLIDESHOW_MODE                           ,
    EVENT_SLIDESHOW_KEY_ZOOMIN                     ,
    EVENT_SLIDESHOW_KEY_ZOOMOUT                    ,
    EVENT_SLIDESHOW_OSD_SHOW                       ,
    EVENT_SLIDESHOW_OSD_HIDE                       ,

    /* Touch Event.*/
    EVENT_SLIDESHOW_TOUCH_RETURN = EVENT_INDEX_SLIDESHOW+INDEX_TOUCH_OFFSET+1,

    EVENT_SLIDESHOW_TOUCH_PREV_PRESS               ,
    EVENT_SLIDESHOW_TOUCH_PREV_MOVE                ,
    EVENT_SLIDESHOW_TOUCH_PREV_RELEASE             ,

    EVENT_SLIDESHOW_TOUCH_NEXT_PRESS               ,
    EVENT_SLIDESHOW_TOUCH_NEXT_MOVE                ,
    EVENT_SLIDESHOW_TOUCH_NEXT_RELEASE             ,

    EVENT_SLIDESHOW_TOUCH_ROTATE_PRESS             ,
    EVENT_SLIDESHOW_TOUCH_ROTATE_MOVE              ,
    EVENT_SLIDESHOW_TOUCH_ROTATE_RELEASE           ,

    EVENT_SLIDESHOW_TOUCH_ZOOMIN_PRESS             ,
    EVENT_SLIDESHOW_TOUCH_ZOOMIN_MOVE              ,
    EVENT_SLIDESHOW_TOUCH_ZOOMIN_RELEASE           ,

    EVENT_SLIDESHOW_TOUCH_ZOOMOUT_PRESS            ,
    EVENT_SLIDESHOW_TOUCH_ZOOMOUT_MOVE             ,
    EVENT_SLIDESHOW_TOUCH_ZOOMOUT_RELEASE          ,

    EVENT_SLIDESHOW_TOUCH_PLAY_PRESS               ,
    EVENT_SLIDESHOW_TOUCH_PLAY_MOVE                ,
    EVENT_SLIDESHOW_TOUCH_PLAY_RELEASE             ,

    EVENT_SLIDESHOW_TOUCH_PAUSE_PRESS              ,
    EVENT_SLIDESHOW_TOUCH_PAUSE_MOVE               ,
    EVENT_SLIDESHOW_TOUCH_PAUSE_RELEASE            ,

    EVENT_SLIDESHOW_TOUCH_BKWD_PRESS               ,
    EVENT_SLIDESHOW_TOUCH_BKWD_MOVE                ,
    EVENT_SLIDESHOW_TOUCH_BKWD_RELEASE             ,

    EVENT_SLIDESHOW_TOUCH_FRWD_PRESS               ,
    EVENT_SLIDESHOW_TOUCH_FRWD_MOVE                ,
    EVENT_SLIDESHOW_TOUCH_FRWD_RELEASE             ,

    EVENT_SLIDESHOW_TOUCH_OPTION                   ,
    EVENT_SLIDESHOW_UPDATE_FILE                    ,
    EVENT_SLIDESHOW_UPDATE_MESSAGE                 ,

    //  HDMI Mode
    EVENT_HDMI_KEY_LCD_OUTPUT = EVENT_INDEX_HDMI,
    EVENT_HDMI_CHANGE_MODE_INIT                 ,
    EVENT_HDMI_BROWSER_KEY_PAGE_UP              ,
    EVENT_HDMI_BROWSER_KEY_PAGE_DOWN            ,
    EVENT_HDMI_BROWSER_KEY_PLAYBACK             ,
    EVENT_HDMI_BROWSER_UPDATE_MESSAGE           ,
    EVENT_HDMI_PHOTOPB_UPDATE_MESSAGE			,
    EVENT_HDMI_DEL_FILE                         ,
    EVENT_HDMI_PLAY_STOP                        ,
    EVENT_HDMI_CLEAR_INFO                       ,

    //  TV-Out Mode
    EVENT_TV_KEY_LCD_OUTPUT = EVENT_INDEX_TV    ,
    EVENT_TV_CHANGE_MODE_INIT                   ,
    EVENT_TV_BROWSER_KEY_PAGE_UP                ,
    EVENT_TV_BROWSER_KEY_PAGE_DOWN              ,
    EVENT_TV_BROWSER_KEY_PLAYBACK               ,
    EVENT_TV_DEL_FILE                           ,
    EVENT_TV_PLAY_STOP                          ,
    EVENT_TV_FORCE_MODE_VIDEO                   ,
    EVENT_TV_FORCE_MODE_CAP                     ,
    EVENT_TV_FORCE_MODE_PB_MOV                  ,
    EVENT_TV_FORCE_MODE_PB_JEPG                 ,
    EVENT_TV_FORMAT_MEDIA                       ,
    EVENT_TV_RESET_SETTING                      ,

    //  File Access Event
    EVENT_FILE_SAVING       = EVENT_INDEX_FS    ,
    EVENT_FILE_DELETING                         ,
    EVENT_LOCK_FILE_G                           ,
    EVENT_LOCK_FILE_M                           ,
    EVENT_NO_FIT_STORAGE_ALLOCATE_CLEAR         ,
    EVENT_NO_FIT_STORAGE_ALLOCATE               ,

    //	Misc Event    
    EVENT_MODE_UNSUPPORTED  = EVENT_INDEX_SYS  ,
    EVENT_MODE_LOCKED  ,  
    EVENT_LENS_WMSG    ,
    EVENT_CLEAR_WMSG   ,
    EVENT_FOCUS_PASS   ,

    //  MSDC Mode
    EVENT_MSDC_UPDATE_MESSAGE,

    //  GPS, Gsensor
    EVENT_GPSGSENSOR_UPDATE,

    //  Parking mode
    EVENT_PARKING_KEY, 

    //  LDWS mode
    EVENT_LDWS_KEY, 

    // 	Sys Calibration
    EVENT_SYS_CALIB_MODE_INIT     	,
    EVENT_SYS_CALIB_UPDATE_MESSAGE	,

    //Change Battery State
    EVENT_CHANGE_BATTERY_STATE		,

    EVENT_ADD_POI                   ,
    EVENT_DEL_POI                   ,

    // Switch Mode
    EVENT_KEY_NEXT_MODE,
    EVENT_KEY_PREV_MODE,
    EVENT_KEY_RETURN,

    // Switch WiFi Preview <-> Streaming Mode
    EVENT_SWITCH_WIFI_STREAMING_MODE,

    // Switch WiFi On/Off	
    EVENT_WIFI_SWITCH_TOGGLE,
    EVENT_WIFI_SWITCH_DISABLE,
    EVENT_WIFI_SWITCH_ENABLE,
    
    // Streaming
    EVENT_OPEN_H264_STREAM,
    EVENT_CLOSE_H264_STREAM,
    //
    EVENT_LASER_LED_ONOFF,

    EVENT_SPEEDCAM_POI_IGNORE,

    EVENT_LCD_STANDBY_CANCEL_HUD,

    EVENT_LCD_STANDBY_START_HUD,

    EVENT_FATIGUEALERT_START,
    EVENT_FATIGUEALERT_STOP,

    EVENT_REMIND_HEADLIGHT_START,
    EVENT_REMIND_HEADLIGHT_STOP,

    EVENT_LDWS_START,
    EVENT_LDWS_STOP,

    EVENT_FCWS_START,
    EVENT_FCWS_STOP,

    EVENT_CUS_SW1_ON,
    EVENT_CUS_SW1_OFF,

    EVENT_CUS_CARD_FULL,

    EVENT_LDWS_CALIBRATION_FIRST_TIME_END,

    EVENT_PARKING_REC_START,
    EVENT_PARKING_REC_STOP,

    EVENT_CANCEL_CRUISE_SPEED_ALERT,

    EVENT_KEY_WIFI_SWITCH,
    EVENT_KEY_PHOTO_MODE,
    EVENT_KEY_REC_MODE,
    EVENT_KEY_6S_MODE,

    EVENT_KEY_FRCAM_SWITCH,

    EVENT_TV_DECODER_SRC_TYPE,
    EVENT_NET_ENTER_PLAYBACK,
	EVENT_NET_EXIT_PLAYBACK,
	
    EVENT_ID_MAX = EVENT_INDEX_MAX

}KeyEventID;

#endif

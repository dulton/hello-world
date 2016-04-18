/*===========================================================================
 * Include file 
 *===========================================================================*/ 
 
#include "Customer_Config.h"
#include "config_fw.h"
#include "AHC_Common.h"
#include "AHC_Parameter.h"
#include "AHC_General.h"
#include "AHC_Message.h"
#include "AHC_Utility.h"
#include "AHC_Display.h"
#include "AHC_Capture.h"
#include "AHC_Fs.h"
#include "AHC_Browser.h"
#include "AHC_Video.h"
#include "AHC_Audio.h"
#include "AHC_Menu.h"
#include "AHC_Os.h"
#include "AHC_Media.h"
#include "AHC_USB.h"
#include "AHC_General_CarDV.h"
#include "AHC_Warningmsg.h"
#include "AHC_UF.h"
#include "PMUCtrl.h"
#include "KeyParser.h"
#include "StateCameraFunc.h"
#include "StateVideoFunc.h"
#include "StateBrowserFunc.h"
#include "StateMoviePBFunc.h"
#include "StatePhotoPBFunc.h"
#include "StateSlideShowFunc.h"
#include "StateMSDCFunc.h"
#include "StateHDMIFunc.h"
#include "StateTVFunc.h"
#include "StateNetPlaybackFunc.h"
#include "MenuStateCameraMenu.h"
#include "MenuStateMovieMenu.h"
#include "MenuStatePlaybackMenu.h"
#include "MenuCommon.h"
#include "MenuTouchButton.h"
#include "MenuSetting.h"
#include "LedControl.h"
#include "dsc_charger.h"
#include "ShowOSDFunc.h"
#include "MenuDrawingFunc.h"
#include "DrawStateBrowserFunc.h"
#include "MediaPlaybackCtrl.h"
#if (SD_UPDATE_FW_EN)
#include "ait_utility.h"
#include "SD_Updater.h"
#endif

#include "mmpf_pll.h"
#include "ldws_cfg.h"
#include "hdr_cfg.h"

// for NETWORK
#if defined(WIFI_PORT) && (WIFI_PORT == 1)
#include "wlan.h"
#include "netapp.h"
#endif

#if defined(TV_ONLY) && !defined(CFG_BOOT_BYPASS_CLOCK_CHECK)
    // force define CFG_BOOT_BYPASS_CLOCK_CHECK when TV_ONLY is defined
    #define CFG_BOOT_BYPASS_CLOCK_CHECK
#endif
#include "IconDefine.h"

#if (GSENSOR_CONNECT_ENABLE)
#include "GSensor_ctrl.h"
#endif

#ifndef CUS_AUTO_VR_DELAT_TIMES
#define CUS_AUTO_VR_DELAT_TIMES         (15)
#endif

/*===========================================================================
 * Global variable
 *===========================================================================*/ 
 
static AHC_BOOL         AhcMenuInit    = AHC_FALSE;
static AHC_BOOL         AhcMenuClockInit    = AHC_FALSE;
static UI_STATE_INFO    uiSysState;
static AHC_BOOL         uBlkEvent   = AHC_FALSE;
static UI_STATE_OP      m_StateOP[UI_STATE_NUM] = {NULL};

MMP_BOOL BurninMode             = MMP_FALSE;
MMP_BOOL CalibrationMode        = MMP_FALSE;

AHC_BOOL PowerOff_InProcess     = AHC_FALSE;
AHC_BOOL PowerOff_Option        = CONFIRM_NOT_YET;
AHC_BOOL bRefreshBrowser        = AHC_FALSE;
AHC_BOOL bForce_PowerOff        = AHC_FALSE;
AHC_BOOL bShowPowerOffJpg       = AHC_FALSE;
UINT8    ubBootupSelect         = BOOTUP_MANUAL;

/*===========================================================================
 * Extern varible
 *===========================================================================*/
extern AHC_BOOL Deleting_InBrowser;
extern AHC_BOOL Protecting_InBrowser;
extern AHC_BOOL bForceSwitchBrowser;
extern UINT8    ssFileType;
extern AHC_BOOL m_ubPlaybackRearCam;

#if (SUPPORT_GSENSOR && POWER_ON_BY_GSENSOR_EN)
extern AHC_BOOL m_ubGsnrIsObjMove;
extern AHC_BOOL ubGsnrPwrOnActStart;
#endif
extern AHC_BOOL gbAhcDbgBrk;
/*===========================================================================
 * Extern function
 *===========================================================================*/

extern void     DefaultMenuSettingInit(void);
extern void     MenuStateEngineerTestMode(UINT32 ulEvent,UINT32 ulPosition);
extern void     MenuStateSDUpdateMode(UINT32 ulEvent, UINT32 ulPosition);
extern void     MenuStateUIModeSelectMode(UINT32 ulEvent, UINT32 ulPosition);
extern void     MenuStateUSBModeSelectMode(UINT32 ulEvent, UINT32 ulPosition);
extern void     MenuStatePowerOnSettingMode(UINT32 ulEvent, UINT32 ulPosition);
extern void     MenuStateBrowserModeSelectMode(UINT32 ulEvent, UINT32 ulPosition);
extern void     MenuItemDateTimeFormatToAhcTimeStampSetting(void);
extern AHC_BOOL CheckMenuSetting(MenuInfo* CurMenu);
extern void     MMPC_HDMI_PC_Enable(MMP_BOOL bEnable);
extern AHC_BOOL uiStateProcessGlobalEvent(UINT32 ulMsgId, UINT32* pulEvent , UINT32* pulParam);
extern void     uiSDUpdateFW(void);
extern AHC_BOOL AHC_GetChargerStatus(void);
extern void     RTNA_LCD_FillColor(MMP_ULONG color);
extern AHC_BOOL PCCAMMode_Start(void);

/*===========================================================================
 * Main body
 *===========================================================================*/

void StateLedControl( UINT32 uiMode )
{
    switch(uiMode)
    {
        case UI_VIDEO_STATE:
            LedCtrl_VideoModeLed(AHC_FALSE);
            LedCtrl_CameraModeLed(AHC_TRUE);
            LedCtrl_PlaybackModeLed(AHC_TRUE);
        break;
            
        case UI_CAMERA_STATE:
            LedCtrl_VideoModeLed(AHC_TRUE);
            LedCtrl_CameraModeLed(AHC_FALSE);
            LedCtrl_PlaybackModeLed(AHC_TRUE);
        break;
            
        case UI_BROWSER_STATE:
        case UI_PLAYBACK_STATE:
        case UI_SLIDESHOW_STATE:
        case UI_HDMI_STATE:
        case UI_TVOUT_STATE:
            LedCtrl_VideoModeLed(AHC_FALSE);
            LedCtrl_CameraModeLed(AHC_FALSE);
            LedCtrl_PlaybackModeLed(AHC_FALSE);
        break;
            
        case UI_MSDC_STATE:
            LedCtrl_VideoModeLed(AHC_TRUE);
            LedCtrl_CameraModeLed(AHC_TRUE);
            LedCtrl_PlaybackModeLed(AHC_TRUE);
        break;
            
        case UI_PCCAM_STATE:
            LedCtrl_VideoModeLed(AHC_TRUE);
            LedCtrl_CameraModeLed(AHC_TRUE);
            LedCtrl_PlaybackModeLed(AHC_TRUE);
        break;
            
        default:
            LedCtrl_VideoModeLed(AHC_TRUE);
            LedCtrl_CameraModeLed(AHC_TRUE);
            LedCtrl_PlaybackModeLed(AHC_TRUE);
        break;
    }
}

void StateLCDCheckStatus(UINT8 mode)
{
    UINT8 ubLCDstatus, ubSNRstatus;

    AHC_LCD_GetStatus(&ubLCDstatus);
    
    ubSNRstatus = AHC_GetSensorStatus();

    if(mode==UI_PCCAM_STATE)//For PCam
        AHC_SetKitDirection(ubLCDstatus, AHC_TRUE, ubSNRstatus, AHC_TRUE);
    else
        AHC_SetKitDirection(ubLCDstatus, AHC_TRUE, ubSNRstatus, AHC_FALSE);
}
AHC_BOOL StateModeResetAllOP(void)
{
    UINT32 i;
    for (i = 0; i <= UI_STATE_UNSUPPORTED; i++) {
        m_StateOP[i].pfInitFunc = NULL;
        m_StateOP[i].pfShutDownFunc = NULL;
    }
    return AHC_TRUE;
}
AHC_BOOL StateModeOperation(UI_STATE_ID mState, void* pfInit, void* pfShutDown)
{
    m_StateOP[mState].pfInitFunc = (pfFunc)pfInit;
    m_StateOP[mState].pfShutDownFunc = (pfFunc)pfShutDown;
    return AHC_TRUE;
}
AHC_BOOL StateSwitchMode(UI_STATE_ID mState)
{
    AHC_BOOL    ahc_ret = AHC_TRUE;

    if(AHC_TRUE == uiSysState.bModeLocked)
    {
        printc("--E-- Switch Mode Locked\n");
        return AHC_FALSE;
    }
    if (uiSysState.CurrentState != mState) {
        if (m_StateOP[uiSysState.CurrentState].pfShutDownFunc != NULL) {
            m_StateOP[uiSysState.CurrentState].pfShutDownFunc(NULL);
        }
    }
    if (mState >= UI_STATE_NUM)
        return AHC_FALSE;
    if (m_StateOP[mState].pfInitFunc != NULL)
        m_StateOP[mState].pfInitFunc(NULL);

    CHARGE_ICON_ENABLE(AHC_FALSE);
    
#ifdef SLIDE_STRING
    StopSlideString();
#endif

    switch( mState )
    {
#if (DSC_MODE_ENABLE)
        case UI_CAMERA_STATE:

            MMPC_HDMI_PC_Enable(MMP_TRUE);
            StateLedControl(mState);
            StateLCDCheckStatus(mState);

            if( uiSysState.CurrentState != mState )
            {
                uiSysState.LastState = uiSysState.CurrentState;
                #ifdef CFG_MENU_CAMERA_ALWAYS_ENTER_CAPTURE_MODE //may be defined in config_xxx.h
                // NOP
                #else
                if( UI_CAMERA_MENU_STATE   <= uiSysState.CurrentState &&
                    UI_PLAYBACK_MENU_STATE >= uiSysState.CurrentState )
                {
                    CaptureMode_PreviewUpdate();
                }
                else
                #endif
                {
                    AHC_SetMode(AHC_MODE_IDLE);
                    SetCurrentOpMode(CAPTURE_MODE);
                    ahc_ret = CaptureMode_Start();
                }
            }
        break;
#endif

        case UI_VIDEO_STATE:

            MMPC_HDMI_PC_Enable(MMP_TRUE);
            StateLedControl(mState);
            StateLCDCheckStatus(mState);

            if( uiSysState.CurrentState != mState )
            {
                uiSysState.LastState = uiSysState.CurrentState;
                
                #ifndef CFG_MENU_CAMERA_ALWAYS_ENTER_CAPTURE_MODE //may be defined in config_xxx.h
                if( UI_CAMERA_MENU_STATE   <= uiSysState.CurrentState &&
                    UI_PLAYBACK_MENU_STATE >= uiSysState.CurrentState )
                {
                    uiSysState.CurrentState = mState; //When Hdmi remove , Video ICON no display, need check
                    VideoRecMode_PreviewUpdate();
                }
                else
                #endif
                {
                    AHC_SetMode(AHC_MODE_IDLE);
                    SetCurrentOpMode(VIDEOREC_MODE);
                    uiSysState.CurrentState = mState;   //When Hdmi remove ,Video ICON no display, need check
                    ahc_ret = VideoRecMode_Start();
                } 
            }
        break;
        
        case UI_BROWSER_STATE:

            MMPC_HDMI_PC_Enable(MMP_TRUE);
            StateLedControl(mState);
            StateLCDCheckStatus(mState);

            uiSysState.LastState = uiSysState.CurrentState;

            if( (UI_CAMERA_MENU_STATE   <= uiSysState.CurrentState  &&
                 UI_PLAYBACK_MENU_STATE >= uiSysState.CurrentState) && !bForceSwitchBrowser)
            {   
                if(AHC_TRUE == AHC_SDMMC_GetMountState()) 
                {
                    if(bRefreshBrowser)
                    {   
                        DrawBrowserStateSDMMC_In();
                        AHC_Thumb_DrawPage(AHC_TRUE);
                        
                        #if (SUPPORT_TOUCH_PANEL)
                        KeyParser_ResetTouchVariable();
                        KeyParser_TouchItemRegister(&Browser_TouchButton[0], ITEMNUM(Browser_TouchButton));  
                        #endif
                         
                        bRefreshBrowser = AHC_FALSE; 
                    }
                    else
                    {
                        BrowserMode_Update();
                    }
                }
                else
                {
                    DrawBrowserStateSDMMC_Out();
                    AHC_DrawThumbNailEmpty();
                    //FIXME To reviewer: If you think this fix is OK, please remove this line. It worked but I'm not sure it's the right way.
                    BrowserMode_Update(); //Truman@130514 Fix the issue entering Global Setting Menu in browser mode, OSD hangs.
                }   
            }
            else
            {
                uiSysState.CurrentState = mState;//refresh the UI state first.
             
                AHC_SetMode(AHC_MODE_IDLE);
                
                switch(GetCurrentOpMode())
                {
                    case VIDEOREC_MODE  :
                    case MOVPB_MODE     :
                        MenuStatePlaybackSelectDB( MOVPB_MODE );
                        SetCurrentOpMode(MOVPB_MODE);
                    break;
                    case CAPTURE_MODE   :
                    case JPGPB_MODE     :
                        MenuStatePlaybackSelectDB( JPGPB_MODE );
                        SetCurrentOpMode(JPGPB_MODE);
                    break;
                    case JPGPB_MOVPB_MODE     :
                        MenuStatePlaybackSelectDB( JPGPB_MOVPB_MODE );
                        SetCurrentOpMode(JPGPB_MOVPB_MODE);
                    break;
                    case AUDPB_MODE     :
                        SetCurrentOpMode(MOVPB_MODE);
                    break;
                    case SLIDESHOW_MODE :
                    break;
                    default             :
                        SetCurrentOpMode(MOVPB_MODE);
                    break;
                }
                bForceSwitchBrowser = AHC_FALSE;
                ahc_ret = BrowserMode_Start();
            }

            if (GetCurrentOpMode() == MOVPB_MODE)
                DrawBrowserStatePlaybackRearCam( m_ubPlaybackRearCam );
            else
                DrawBrowserStatePlaybackRearCam( AHC_FALSE );
        break;

        case UI_PLAYBACK_STATE:

            MMPC_HDMI_PC_Enable(MMP_TRUE);
            
            if( uiSysState.CurrentState != mState )
            {
                uiSysState.LastState = uiSysState.CurrentState;

                if( UI_CAMERA_MENU_STATE   <= uiSysState.CurrentState &&
                    UI_PLAYBACK_MENU_STATE >= uiSysState.CurrentState )
                {
                    if( MOVPB_MODE == GetCurrentOpMode())
                    {
                        MoviePBMode_Update();
                    }
                    else if( JPGPB_MODE == GetCurrentOpMode())
                    {
                        PhotoPBMode_Update();                     
                    }
                    else if( AUDPB_MODE == GetCurrentOpMode())
                    {
                        AudioPBMode_Update();
                    }
                    else if( JPGPB_MOVPB_MODE == GetCurrentOpMode())
                    {
                        UINT32 uiCurrentIdx;
                        UINT8  ubFileType;
                        AHC_UF_GetCurrentIndex(&uiCurrentIdx);
                        AHC_UF_GetFileTypebyIndex(uiCurrentIdx, &ubFileType);
                        if ( ubFileType == DCF_OBG_JPG )
                            PhotoPBMode_Update(); 
                        else
                            MoviePBMode_Update();
                    }
                }
                else
                {
                    AHC_SetMode(AHC_MODE_IDLE);
                    
                    if( MOVPB_MODE == GetCurrentOpMode())
                    {
                        ahc_ret = MoviePBMode_Start();
                    }
                    else if( JPGPB_MODE == GetCurrentOpMode())
                    {
                        ahc_ret = PhotoPBMode_Start();
                    }
                    else if( AUDPB_MODE == GetCurrentOpMode())
                    {
                        ahc_ret = AudioPBMode_Start();     
                    }
                    else if( JPGPB_MOVPB_MODE == GetCurrentOpMode())
                    {
                        UINT32 uiCurrentIdx;
                        UINT8  ubFileType;
                        AHC_UF_GetCurrentIndex(&uiCurrentIdx);
                        AHC_UF_GetFileTypebyIndex(uiCurrentIdx, &ubFileType);
                        if ( ubFileType == DCF_OBG_JPG )
                            PhotoPBMode_Start(); 
                        else
                            MoviePBMode_Start();
                    }
                }
            }            
        break;
        
#if (SLIDESHOW_EN)
        case UI_SLIDESHOW_STATE:
            
            MMPC_HDMI_PC_Enable(MMP_TRUE);
            
            if( uiSysState.CurrentState != mState )
            {
                uiSysState.LastState = uiSysState.CurrentState;
                
                if( UI_CAMERA_MENU_STATE   <= uiSysState.CurrentState &&
                    UI_PLAYBACK_MENU_STATE >= uiSysState.CurrentState )
                {
                    SlideShowMode_Update();
                }
                else
                {
                    AHC_SetMode(AHC_MODE_IDLE);
                    ahc_ret = SlideShowFunc_Start();
                }
            }
        break;
#endif

        case UI_MSDC_STATE:

            MMPC_HDMI_PC_Enable(MMP_TRUE); 
                       
            if(UI_MSDC_STATE != uiSysState.CurrentState)
            {
                uiSysState.LastState = uiSysState.CurrentState;
                StateLedControl(UI_MSDC_STATE);

                /* The StateLCDCheckStatus will set Sensor Direction,
                 * and it may cause system crash when USB plugin in Brower mode for RY04
                 * It needs check, Canlet
                 */
                #ifndef CFG_MENU_IGNORE_MSDC_LCD_CHECK //may be defined in config_xxx.h
                if ((AHC_FALSE == AHC_IsTVConnectEx()) && (AHC_FALSE == AHC_IsHdmiConnect()))
                    StateLCDCheckStatus(mState);
                #endif
                
                AHC_SetMode(AHC_MODE_IDLE);
                ahc_ret = MSDCMode_Start();
            }
        break;

        case UI_PCCAM_STATE:

            MMPC_HDMI_PC_Enable(MMP_TRUE); 
                       
            if(UI_PCCAM_STATE != uiSysState.CurrentState)
            {
                uiSysState.LastState = uiSysState.CurrentState;
                StateLedControl(UI_PCCAM_STATE);

                /* The StateLCDCheckStatus will set Sensor Direction,
                 * and it may cause system crash when USB plugin in Brower mode for RY04
                 * It needs check, Canlet
                 */
                #ifndef CFG_MENU_IGNORE_MSDC_LCD_CHECK //may be defined in config_xxx.h
                if ((AHC_FALSE == AHC_IsTVConnectEx()) && (AHC_FALSE == AHC_IsHdmiConnect()))
                    StateLCDCheckStatus(mState);
                #endif
                
                AHC_SetMode(AHC_MODE_IDLE);
                ahc_ret = PCCAMMode_Start();
            }
        break;

#if (HDMI_ENABLE)
        case UI_HDMI_STATE:
            uiSysState.LastState = uiSysState.CurrentState;
            AHC_SetMode(AHC_MODE_IDLE);
            ahc_ret = HDMIMode_Start();
        break;
#endif

#if (TVOUT_ENABLE)
        case UI_TVOUT_STATE:
            uiSysState.LastState = uiSysState.CurrentState;
            AHC_SetMode(AHC_MODE_IDLE);
            ahc_ret = TVMode_Start();
        break;
#endif

        case UI_CAMERA_MENU_STATE:

            if( uiSysState.CurrentState!=UI_CAMERA_MENU_STATE &&
                uiSysState.CurrentState!=UI_VIDEO_MENU_STATE  &&
                uiSysState.CurrentState!=UI_PLAYBACK_MENU_STATE)
            {   
                uiSysState.LastState = uiSysState.CurrentState;
            }

            MenuStateCameraMode(BUTTON_VIRTUAL_RESET, 0);
        break;

        case UI_VIDEO_MENU_STATE:
       
            if( uiSysState.CurrentState!=UI_CAMERA_MENU_STATE &&
                uiSysState.CurrentState!=UI_VIDEO_MENU_STATE  &&
                uiSysState.CurrentState!=UI_PLAYBACK_MENU_STATE)
            {   
                uiSysState.LastState = uiSysState.CurrentState;
            }

            MenuStateVideoMode(BUTTON_VIRTUAL_RESET, 0);
        break;

        case UI_PLAYBACK_MENU_STATE:
        
            if( uiSysState.CurrentState!=UI_CAMERA_MENU_STATE &&
                uiSysState.CurrentState!=UI_VIDEO_MENU_STATE  &&
                uiSysState.CurrentState!=UI_PLAYBACK_MENU_STATE)
            {   
                uiSysState.LastState = uiSysState.CurrentState;
            }
             
            MenuStatePlaybackMode(BUTTON_VIRTUAL_RESET, 0);
        break;
		case UI_NET_PLAYBACK_STATE:
		
			MMPC_HDMI_PC_Enable(MMP_TRUE);
			uiSysState.LastState = uiSysState.CurrentState;
			uiSysState.CurrentState = mState;
			AHC_SetMode(AHC_MODE_IDLE);
		break;
        case UI_CLOCK_SETTING_STATE:
        
			uiSysState.LastState = uiSysState.CurrentState;
            uiSysState.CurrentState = mState;
             
            StateLCDCheckStatus(mState);
                    
            MenuStateClockSettingsMode(BUTTON_VIRTUAL_RESET, 0);
        break;
        
#if (SD_UPDATE_FW_EN)
        case UI_SD_UPDATE_STATE:

            uiSysState.CurrentState = mState;

            /* At this monunt, The StateLCDCheckStatus will set LCD Direction,
             * and it will cause system crash when booting from TV
            */
            if ((AHC_FALSE == AHC_IsTVConnectEx()) && (AHC_FALSE == AHC_IsHdmiConnect()))
                StateLCDCheckStatus(mState);
                    
            MenuStateSDUpdateMode(BUTTON_VIRTUAL_RESET, 0);
        break;
#endif
    
#if (USB_MODE_SELECT_EN)
        case UI_USBSELECT_MENU_STATE:
        
            if( uiSysState.CurrentState!=UI_USBSELECT_MENU_STATE &&
                uiSysState.CurrentState!=UI_STATE_UNSUPPORTED)
            {   
                uiSysState.LastState = uiSysState.CurrentState;
            }
#if (HDMI_PREVIEW)
			if(AHC_IsHdmiConnect())
			{      
				HDMIFunc_InitPreviewOSD();
                SetHDMIState(AHC_HDMI_VIDEO_PREVIEW_STATUS);
			}                    
#endif
#if (TVOUT_PREVIEW)
			if(AHC_IsTVConnectEx())
			{      
				TVFunc_InitPreviewOSD();
                SetTVState(AHC_TV_VIDEO_PREVIEW_STATUS);
			}
#endif
            uiSysState.CurrentState = mState;
             
            StateLCDCheckStatus(mState);
        
            MenuSettingConfig()->uiUSBFunction = 0xFF;
            
            MenuStateUSBModeSelectMode(BUTTON_VIRTUAL_RESET, 0);
            LedCtrl_LcdBackLight(AHC_TRUE);
        break;
#endif

#if (UI_MODE_SELECT_EN)
        case UI_MODESELECT_MENU_STATE:
        
            if( uiSysState.CurrentState!=UI_MODESELECT_MENU_STATE &&
                uiSysState.CurrentState!=UI_STATE_UNSUPPORTED)
            {   
                uiSysState.LastState = uiSysState.CurrentState;
            }
        
            uiSysState.CurrentState = mState;
             
            StateLCDCheckStatus(mState);        
            
            MenuStateUIModeSelectMode(BUTTON_VIRTUAL_RESET, 0);
            
            LedCtrl_LcdBackLight(AHC_TRUE);
        break;
#endif

#if (BROWSER_MODE_SELECT_EN)
        case UI_BROWSELECT_MENU_STATE:
        
            if( uiSysState.CurrentState!=UI_BROWSELECT_MENU_STATE &&
                uiSysState.CurrentState!=UI_STATE_UNSUPPORTED)
            {   
                uiSysState.LastState = uiSysState.CurrentState;
            }
        
            uiSysState.CurrentState = mState;
             
            StateLCDCheckStatus(mState);        
            
            MenuStateBrowserModeSelectMode(BUTTON_VIRTUAL_RESET, 0);
        break;
#endif

#if (POWER_ON_MENU_SET_EN)
        case UI_POWERON_SET:
            uiSysState.CurrentState = mState;
             
            StateLCDCheckStatus(mState);
                    
            MenuStatePowerOnSettingMode(BUTTON_VIRTUAL_RESET, 0);
            LedCtrl_LcdBackLight(AHC_TRUE);
        break;
#endif

        default:
            AHC_SetMode(AHC_MODE_IDLE);
        break;
    }
    
    CHARGE_ICON_ENABLE(AHC_TRUE);

    uiSysState.CurrentState = mState;
    
    return ahc_ret;
}

void uiStateSystemStart(void)
{
    AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, 0, 0);
}

void uiCheckDefaultMenuExist(void)
{
    MenuInfo *useMenu;
    MenuInfo *defMenu;
    AHC_BOOL ret;

    printc("### %s -\r\n", __func__);

    useMenu = MenuSettingConfig();
    defMenu = DefaultMenuSettingConfig();

#ifdef CFG_MENU_SETTING_USING_PROGRAM_DEFAULT
    DefaultMenuSettingInit();       // Set setting as program default
    CheckMenuSetting(defMenu);
    memcpy(useMenu, defMenu, sizeof(MenuInfo));
    useMenu->uiBatteryVoltage = BATTERY_VOLTAGE_FULL;
#else
    ret = AHC_PARAM_Menu_Read((UINT8*)defMenu, AHC_DEFAULT_FACTORY_FAT);

    if (!ret) {
        // No default setting file
        printc("###############################\r\n"
               "--E-- No Default Settings\r\n"
               "###############################\r\n");

        DefaultMenuSettingInit();       // Set setting as program default
        CheckMenuSetting(defMenu);
        RestoreFromDefaultSetting();    // Set setting as current
        AHC_PARAM_Menu_Write(AHC_DEFAULT_USER_FAT); // Write to file
        Menu_WriteLcdSnrTVHdmiStauts2SF();
    }

    ret = AHC_PARAM_Menu_Read((UINT8*)useMenu, AHC_DEFAULT_USER_FAT);

    if (!ret) {
        printc("###############################\r\n"
                "--E-- No User Settings\r\n"
                "###############################\r\n");

        memcpy(useMenu, defMenu, sizeof(MenuInfo));
        // Make the battery is not 0, otherwise it will show battery warning at
        // the first time power on after FW updated
        useMenu->uiBatteryVoltage = BATTERY_VOLTAGE_FULL;

        CheckMenuSetting(useMenu);
        // Write to User setting file in SF
        AHC_PARAM_Menu_Write(AHC_DEFAULT_USER_FAT);
        Menu_WriteLcdSnrTVHdmiStauts2SF();
    }
#endif

    ApplyUserTextSetting("SD:0:\\usercfg.txt", useMenu);
    CheckMenuSetting(useMenu);
    Menu_Setting_BackupMenuAtoms();
    #if defined(TXT_MENU_WRITE_TO_SD)
    ExportMenuAtom((char*)"SD:0:\\usercfg.txt", TXT_COMMENT | TXT_VERSION | TXT_MENU_ACT);
    #endif

    // Initial ACC_DATE_STAMP
    MenuItemDateTimeFormatToAhcTimeStampSetting();
#if(SUPPORT_ADAS)
	#ifdef CUS_ADAS_PARA
	MMPS_ADAS_CustomerInitPara(LDWS_ALARM_DELAY_TIME,FCWS_ALARM_RESET_TIME,FCWS_ALARM_SPEED_THD
									FCWS_ALARM_TTC_THD_1,FCWS_ALARM_TTC_THD_2
									FCWS_ALARM_DIST_THD_1,FCWS_ALARM_DIST_THD_2);
	MMPS_ADAS_CustomerSpecifyPara(ADAS_FOCAL_LENGTH,ADAS_PIXEL_SIZE,
									ADAS_DZOOM_N,ADAS_DZOOM_M);
	#endif

    LDWS_CalibrationParam_UserUpdate();
    LDWS_CalibrationParam_Read();
    #ifdef BACKUP_LDWS_CALIBRATION_FILE
    LDWS_CalibrationParam_Backup();
    #endif
#endif
}

void VideoSettingInit( void )
{
    AHC_AUDIO_FORMAT aformat;

    //Video
    AHC_ConfigMovie(AHC_VIDEO_CODEC_TYPE        , AHC_MOVIE_VIDEO_CODEC_H264);
    AHC_ConfigMovie(AHC_VIDEO_COMPRESSION_RATIO , MenuSettingConfig()->uiMOVQuality);
    AHC_ConfigMovie(AHC_MAX_PFRAME_NUM          , 14);

    //Audio
#if (VR_AUDIO_TYPE==VR_AUDIO_TYPE_AAC)
    aformat = AHC_MOVIE_AUDIO_CODEC_AAC;
#elif (VR_AUDIO_TYPE==VR_AUDIO_TYPE_MP3)
    aformat = AHC_MOVIE_AUDIO_CODEC_MP3;
#elif (VR_AUDIO_TYPE==VR_AUDIO_TYPE_ADPCM)
    aformat = AHC_MOVIE_AUDIO_CODEC_ADPCM;
#endif
        
    AHC_ConfigAudio(AHC_AUDIO_STREAM_ID, aformat, AHC_AUDIO_CHANNEL_MONO_R);

    #if (GSENSOR_CONNECT_ENABLE)
    // Since the default setting of ubStoreFreq = 4, and it makes problems with below formula in StateVideoRecMode()::case EVENT_GPSGSENSOR_UPDATE:
    // if( 0 == (ulCounterForGpsGsnrUpdate % ((1000/VIDEO_TIMER_UNIT)/(AHC_Gsensor_GetAttributeAddr()->ubStoreFreq))))
    //   AHC_Gsensor_SetCurInfo();
    // So, change the setting to 5.
    AHC_Gsensor_GetAttributeAddr()->ubStoreFreq = 5;  //Store 5 times GSensor data per second
    #endif

    //Set ADAS parameter
    {
        UINT32 bLDWS_En = LDWS_EN_OFF;
        UINT32 bFCWS_En = FCWS_EN_OFF;
        UINT32 bSAG_En  = SAG_EN_OFF;
        UINT32 uiAdasFlag = 0;
        
        if ( AHC_Menu_SettingGetCB( (char *)COMMON_KEY_LDWS_EN, &bLDWS_En ) == AHC_TRUE ) {
            if (bLDWS_En == LDWS_EN_ON ) {
                uiAdasFlag |= AHC_ADAS_ENABLED_LDWS;
            }
        }

        if ( AHC_Menu_SettingGetCB( (char *)COMMON_KEY_FCWS_EN, &bFCWS_En ) == AHC_TRUE ) {
            if (bFCWS_En == FCWS_EN_ON ) {
                uiAdasFlag |= AHC_ADAS_ENABLED_FCWS;
            }
        }

        if ( AHC_Menu_SettingGetCB( (char *)COMMON_KEY_SAG_EN, &bSAG_En ) == AHC_TRUE ) {
            if (bSAG_En == SAG_EN_ON ) {
                uiAdasFlag |= AHC_ADAS_ENABLED_SAG;
            }
        }
        
        AHC_SetParam( PARAM_ID_ADAS, uiAdasFlag );
    }

}

static void CheckRtcAndAutoSetRtc(void)
{
	UINT16		uwYear = 0, uwMonth = 0, uwDay,uwDayInWeek,uwHour,uwMinute,uwSecond;
	UINT8		 ubAmOrPm, b_12FormatEn;

	AHC_SetParam(PARAM_ID_USE_RTC, 1);
	AHC_GetClock( &uwYear, &uwMonth, &uwDay, &uwDayInWeek, &uwHour, &uwMinute, &uwSecond, &ubAmOrPm, &b_12FormatEn); 

	if (!AHC_Validate_Year(uwYear))
	{
		AHC_RestoreDefaultTime(&uwYear, &uwMonth, &uwDay, &uwDayInWeek, &uwHour, &uwMinute, &uwSecond, &ubAmOrPm, &b_12FormatEn);
		AHC_SetClock(uwYear, uwMonth, uwDay, uwDayInWeek, uwHour, uwMinute, uwSecond, ubAmOrPm, b_12FormatEn);
	}
}

static void uiStateInitialize(void)
{
    AHC_BOOL    runOp  = AHC_FALSE;
#if WIFI_PORT
    AHC_BOOL    wifiOn;
    extern UINT32 ulLastWiFiChagneTime;
#endif
    
    SystemSettingInit();
    VideoSettingInit();

#if WIFI_PORT
    wifiOn = (Getpf_WiFi() == 0) ? AHC_TRUE : AHC_FALSE;
#endif

    #ifdef BMICOM_FILE_DAMAGE
    AHC_ICON_LoadFileDamage((GUI_BITMAP*)&BMICOM_FILE_DAMAGE);
    #endif
    #ifdef BMICOM_FILE_G_PROTECTKEY
    AHC_ICON_LoadProtectKey(AHC_PROTECT_G,(GUI_BITMAP*)&BMICOM_FILE_G_PROTECTKEY);
    #endif
    #ifdef BMICOM_FILE_M_PROTECTKEY
    AHC_ICON_LoadProtectKey(AHC_PROTECT_MENU,(GUI_BITMAP*)&BMICOM_FILE_M_PROTECTKEY);
    #endif

    uiSysState.CurrentState = UI_STATE_UNSUPPORTED;

    StateModeResetAllOP();
    StateModeOperation(UI_VIDEO_STATE,              (void*)VideoFunc_Init,              (void*)VideoFunc_ShutDown           );
    StateModeOperation(UI_NET_PLAYBACK_STATE,       (void*)NetPlaybackFunc_Init,        (void*)NetPlaybackFunc_ShutDown     );
    {
        ShowOSD_SetLanguage(0xff/* here is dummy */);
        AHC_SetRecordByChargerIn(0);

        #ifdef CFG_BOOT_POWER_ON_BY_USB_SOS //may be defined in config_xxx.h
        if (KEY_GPIO_SOS != MMP_GPIO_MAX)
        {   /* Power on by USB with SOS key to enter PCCAM */
            MMP_UBYTE tempValue = 0;

            MMPF_PIO_EnableOutputMode(KEY_GPIO_SOS, MMP_FALSE, MMP_TRUE);
            MMPF_PIO_GetData(KEY_GPIO_SOS, &tempValue);

            if(tempValue == KEY_PRESSLEVEL_SOS)
                AHC_SetButtonStatus(KEY_LPRESS_REC);
        }
        #endif
        
        runOp = AHC_PowerOnOption();

        if (AHC_IsDCInBooting()) {
            ubBootupSelect = BOOTUP_CHARGER;
        }
        else if (AHC_IsUsbBooting()) {
            AHC_USB_WaitUntilFirstRoundDone(1200);

            if (IsAdapter_Connect()) {
                ubBootupSelect = BOOTUP_CHARGER;
            }
            else {
                ubBootupSelect = BOOTUP_USB;
            }
        }

#if (SD_UPDATE_FW_EN)
        #ifdef SD_UPDATE_FW_FORCE_EN
        if (1) 
        #else
        if (AHC_IsDcCableConnect() || AHC_IsUsbConnect()) 
        #endif
        {
            // Go to update firmware when SD_CarDV.bin exist and Charging
            if (SDUpdateIsFWExisted() == AHC_TRUE)
            {
				CheckRtcAndAutoSetRtc();
                StateSwitchMode(UI_SD_UPDATE_STATE);
            }
        }
#endif
        
        if (ubBootupSelect==BOOTUP_USB)
        {
            printc(FG_GREEN("@@@ Boot from USB\r\n"));

            #ifdef CFG_BOOT_FAST_LPRESS_REC_WHEN_USB_BOOT //may be defined in config_xxx.h
            if (KEY_GPIO_REC != MMP_GPIO_MAX)
            {
                UINT8 tempValue = 0;

                AHC_ConfigGPIOPad(KEY_GPIO_REC, PAD_NORMAL_TRIG);
                AHC_ConfigGPIO(KEY_GPIO_REC, MMP_FALSE);
                AHC_GetGPIO(KEY_GPIO_REC, &tempValue);

                if(tempValue == KEY_PRESSLEVEL_REC)
                    AHC_SetButtonStatus(KEY_LPRESS_REC);
            }
            #endif
			
			#if WIFI_PORT
            wifiOn = AHC_FALSE;
			#endif

            if (AHC_GetQuickKey(QUICK_PCCAM) || (MenuSettingConfig()->uiUSBFunction == USB_PCAM))
            {
            	#if USB_MODE_SELECT_EN
				USBSelectModePCam(NULL,0);
				#endif
                StateSwitchMode(UI_PCCAM_STATE);
            }
            else
            {
                #if (USB_MODE_SELECT_EN)
                if (MMPS_USB_NORMAL == MMPS_USB_GetStatus()) {
					CheckRtcAndAutoSetRtc();//	If Date Time invalid, Record will fail.
                    StateSwitchMode(UI_USBSELECT_MENU_STATE);
                }
                else
                {
                    #ifdef CFG_POWER_ON_ENTER_CAMERA_STATE
                    StateSwitchMode(UI_CAMERA_STATE);
                    #else
                    AHC_SetRecordByChargerIn(CUS_AUTO_VR_DELAT_TIMES);
                    if (MOTION_DTC_SENSITIVITY_OFF != MenuSettingConfig()->uiMotionDtcSensitivity) {
                        Menu_SetMotionDtcSensitivity(MenuSettingConfig()->uiMotionDtcSensitivity);
                    }

                    StateSwitchMode(UI_VIDEO_STATE);
                    #endif
                }
                #else
                if(MenuSettingConfig()->uiUSBFunction == USB_MSDC)
                {
                	#if USB_MODE_SELECT_EN
                	USBSelectModeMSDC(NULL,0);
					#endif
                    StateSwitchMode(UI_MSDC_STATE);
                }
                else
                {
					#if USB_MODE_SELECT_EN
					USBSelectModePCam(NULL,0);
					#endif
                    StateSwitchMode(UI_PCCAM_STATE);
                }
                #endif
            }
        }
        else if(ubBootupSelect==BOOTUP_CHARGER || ubBootupSelect==BOOTUP_MANUAL)
        {
            UINT16      uwYear,uwMonth,uwDay,uwDayInWeek,uwHour,uwMinute,uwSecond;
            UINT8       ubAmOrPm, b_12FormatEn;
            AHC_BOOL    RTC_ret = AHC_FALSE;

            printc(FG_GREEN("@@@ Boot from Charger or Manual\r\n"));
            
            #ifdef CFG_BOOT_DC_IN_POWER_OFF
            if ((AHC_GetBootingSrc() & PWR_ON_BY_KEY) != PWR_ON_BY_KEY)
            {
                printc("Power on by src %d\r\n",AHC_GetBootingSrc());
                AHC_NormalPowerOffPathEx(AHC_TRUE, AHC_FALSE, AHC_FALSE);
            }
            #endif

            AHC_SetParam(PARAM_ID_USE_RTC, 1);
            
            RTC_ret = AHC_GetClock( &uwYear, &uwMonth, &uwDay, &uwDayInWeek, &uwHour, &uwMinute, &uwSecond, &ubAmOrPm, &b_12FormatEn); 

            #ifdef CFG_POWER_ON_ALWAYS_REC
            AHC_SetRecordByChargerIn(CUS_AUTO_VR_DELAT_TIMES);
            #endif

            if ( (RTC_ret && AHC_Validate_Year(uwYear) && !Getfp_ResetTime()) ||
                 runOp == AHC_TRUE /* To Run PowerOn Option, not to enter clock setting */)
            {
                if(!AHC_Validate_Year(uwYear))
                {
                    AHC_RestoreDefaultTime(&uwYear, &uwMonth, &uwDay, &uwDayInWeek, &uwHour, &uwMinute, &uwSecond, &ubAmOrPm, &b_12FormatEn) ;
                    AHC_SetClock(uwYear, uwMonth, uwDay, uwDayInWeek, uwHour, uwMinute, uwSecond, ubAmOrPm, b_12FormatEn);
                }
                
#if (TVOUT_ENABLE && TVOUT_PREVIEW_EN)
                if(AHC_IsTVConnectEx())
                {
                    if(AHC_GetChargerStatus() && !runOp)
                    {
                        AHC_SetRecordByChargerIn(CUS_AUTO_VR_DELAT_TIMES);
                    }
                    #if (SUPPORT_GSENSOR && POWER_ON_BY_GSENSOR_EN)
                    else
                    {   
                        if ((AHC_GetBootingSrc() & PWR_ON_BY_GSENSOR) == PWR_ON_BY_GSENSOR)
                        {
                            if(GSNR_PWRON_ACT == GSNR_PWRON_REC_AUTO)
                                AHC_SetRecordByChargerIn(CUS_AUTO_VR_DELAT_TIMES);
                        }
                    }
                    #endif

                    #ifdef CFG_POWER_ON_ENTER_CAMERA_STATE
                    SetTVState(AHC_TV_DSC_PREVIEW_STATUS);
                    #else
                    SetTVState(AHC_TV_VIDEO_PREVIEW_STATUS);
                    #endif
                    
                    StateSwitchMode(UI_TVOUT_STATE);
                }
                else
#endif
#if (HDMI_ENABLE && HDMI_PREVIEW_EN)
                if(AHC_IsHdmiConnect())
                {
                    if(AHC_GetChargerStatus() && !runOp)
                        AHC_SetRecordByChargerIn(CUS_AUTO_VR_DELAT_TIMES);
                
                    #ifdef CFG_POWER_ON_ENTER_CAMERA_STATE
                    SetHDMIState(AHC_HDMI_DSC_PREVIEW_STATUS);
                    #else
                    SetHDMIState(AHC_HDMI_VIDEO_PREVIEW_STATUS);
                    #endif

                    StateSwitchMode(UI_HDMI_STATE);
                }
                else
#endif
                {
#if (MENU_GENERAL_BOOT_UP_MODE_EN)
                    
                    AHC_SetBootUpUIMode();
                    
#else
                    
                #ifdef CFG_BOOT_LCD_ALWAYS_REC_BY_CHARGER_IN //may be defined in config_xxx.h
                    AHC_SetRecordByChargerIn(CUS_AUTO_VR_DELAT_TIMES);
                #else
                    if(AHC_GetChargerStatus() && !runOp)
                    {
                        AHC_SetRecordByChargerIn(CUS_AUTO_VR_DELAT_TIMES);
                    }
                    #if (SUPPORT_GSENSOR && POWER_ON_BY_GSENSOR_EN)
                    else
                    {   
                        if ((AHC_GetBootingSrc() & PWR_ON_BY_GSENSOR) == PWR_ON_BY_GSENSOR)
                        {
                            if(GSNR_PWRON_ACT == GSNR_PWRON_REC_AUTO)
                                AHC_SetRecordByChargerIn(CUS_AUTO_VR_DELAT_TIMES);
                        }
                    }
                    #endif
                #endif

                    if (AHC_IsUsbConnect() && !IsAdapter_Connect()) {
                        #if (USB_MODE_SELECT_EN)
                        if (MMPS_USB_NORMAL == MMPS_USB_GetStatus()) {
                            StateSwitchMode(UI_USBSELECT_MENU_STATE);
                        }
                        else
                        {
                            #ifdef CFG_POWER_ON_ENTER_CAMERA_STATE
                            StateSwitchMode(UI_CAMERA_STATE);
                            #else
                            if (MOTION_DTC_SENSITIVITY_OFF != MenuSettingConfig()->uiMotionDtcSensitivity) {
                                Menu_SetMotionDtcSensitivity(MenuSettingConfig()->uiMotionDtcSensitivity);
                            }

                            StateSwitchMode(UI_VIDEO_STATE);
                            #endif
                        }
                        #else
                        if(MenuSettingConfig()->uiUSBFunction == USB_MSDC)
                        {
                            StateSwitchMode(UI_MSDC_STATE);
                        }
                        else
                        {
                            StateSwitchMode(UI_PCCAM_STATE);
                        }
                        #endif
                    }
                    else
                    {
                        #ifdef CFG_POWER_ON_ENTER_CAMERA_STATE
                        StateSwitchMode(UI_CAMERA_STATE);
                        #else
                        if (MOTION_DTC_SENSITIVITY_OFF != MenuSettingConfig()->uiMotionDtcSensitivity) {
                            Menu_SetMotionDtcSensitivity(MenuSettingConfig()->uiMotionDtcSensitivity);
                        }

                        StateSwitchMode(UI_VIDEO_STATE);
                        #endif
                    }
#endif
                }
            }
            else 
            {
                AHC_RestoreDefaultTime(&uwYear, &uwMonth, &uwDay, &uwDayInWeek, &uwHour, &uwMinute, &uwSecond, &ubAmOrPm, &b_12FormatEn);
                AHC_SetClock(uwYear, uwMonth, uwDay, uwDayInWeek, uwHour, uwMinute, uwSecond, ubAmOrPm, b_12FormatEn);

            //may be defined in config_xxx.h
            #if defined (CFG_BOOT_AUTO_REC_BY_CHARGER_IN)
                
                if(AHC_GetChargerStatus())
                    AHC_SetRecordByChargerIn(CUS_AUTO_VR_DELAT_TIMES);
                
                #if (TVOUT_ENABLE && TVOUT_PREVIEW_EN)
                if(AHC_IsTVConnectEx())
                {
                    #ifdef CFG_POWER_ON_ENTER_CAMERA_STATE
                    StateSwitchMode(AHC_TV_DSC_PREVIEW_STATUS);
                    #else
                    SetTVState(AHC_TV_VIDEO_PREVIEW_STATUS);
                    #endif
                    
                    StateSwitchMode(UI_TVOUT_STATE);
                }
                else
                #endif
                #if (HDMI_ENABLE && HDMI_PREVIEW_EN)
                if(AHC_IsHdmiConnect())
                {
                    #ifdef CFG_POWER_ON_ENTER_CAMERA_STATE
                    SetHDMIState(AHC_HDMI_DSC_PREVIEW_STATUS);
                    #else
                    SetHDMIState(AHC_HDMI_VIDEO_PREVIEW_STATUS);
                    #endif
                    
                    StateSwitchMode(UI_HDMI_STATE);
                }
                else
                #endif
                {
                    #ifdef CFG_POWER_ON_ENTER_CAMERA_STATE
                    StateSwitchMode(UI_CAMERA_STATE);
                    #else
                    if (MOTION_DTC_SENSITIVITY_OFF != MenuSettingConfig()->uiMotionDtcSensitivity) {
                        Menu_SetMotionDtcSensitivity(MenuSettingConfig()->uiMotionDtcSensitivity);
                    }

                    StateSwitchMode(UI_VIDEO_STATE);
                    #endif
                }
                
            #else

                #if (TVOUT_ENABLE && TVOUT_PREVIEW_EN)
                if(AHC_IsTVConnectEx()) 
                {
                    #ifdef CFG_POWER_ON_ENTER_CAMERA_STATE
                    StateSwitchMode(AHC_TV_DSC_PREVIEW_STATUS);
                    #else
                    SetTVState(AHC_TV_VIDEO_PREVIEW_STATUS);
                    #endif
                    
                    StateSwitchMode(UI_TVOUT_STATE);
                }
                else
                #endif
                #if (HDMI_ENABLE && HDMI_PREVIEW_EN)
                if(AHC_IsHdmiConnect())
                {
                    #ifdef CFG_POWER_ON_ENTER_CAMERA_STATE
                    SetHDMIState(AHC_HDMI_DSC_PREVIEW_STATUS);
                    #else
                    SetHDMIState(AHC_HDMI_VIDEO_PREVIEW_STATUS);
                    #endif
                    
                    StateSwitchMode(UI_HDMI_STATE);
                }
                else
                #endif
                {
                #if defined(TV_ONLY) || \
                    defined(CFG_BOOT_BYPASS_CLOCK_CHECK)  //may be defined in config_xxx.h
                    #ifdef CFG_POWER_ON_ENTER_CAMERA_STATE
                    StateSwitchMode(UI_CAMERA_STATE);
                    #elif defined(CFG_POWER_ON_ENTER_BROSWER_STATE)
                    StateSwitchMode(UI_BROWSER_STATE);
                    #else
                    if (MOTION_DTC_SENSITIVITY_OFF != MenuSettingConfig()->uiMotionDtcSensitivity) {
                        Menu_SetMotionDtcSensitivity(MenuSettingConfig()->uiMotionDtcSensitivity);
                    }

                    StateSwitchMode(UI_VIDEO_STATE);
                    #endif
                #else
                    printc("Enter Clock Setting Page ...\r\n");
					if(AIHC_GetOSDInitStatus())
					{
                    	StateSwitchMode(UI_CLOCK_SETTING_STATE);
					}
					else
					{
						AhcMenuClockInit = AHC_TRUE;
					}
                #endif
                }
            #endif
            }

            #ifdef CFG_BOOT_INIT_LAST_STATE_AS_CUR //may be defined in config_xxx.h
            if(uiGetLastState() == UI_STATE_UNSUPPORTED)
                uiSysState.LastState = uiGetCurrentState(); 
            #endif
        }
    }

    // StartUp NetWork
#if WIFI_PORT
    if (wifiOn) {
        netapp_InitNetwork();
        MMPF_OS_GetTime(&ulLastWiFiChagneTime);
    }
#endif
}

void uiStateSetLocked(AHC_BOOL bLock)
{
    uiSysState.bModeLocked = bLock;
}

UINT8 uiGetLastState(void)
{
    return uiSysState.LastState;
}

UINT8 uiGetCurrentState(void)
{
    return uiSysState.CurrentState;
}

AHC_BOOL uiStateProcessGlobalEvent2(UINT32 ulMsgId, UINT32* pulEvent, UINT32* pulParam)
{
#if (NO_CARD_POWER_OFF_EN)  
    extern AHC_BOOL gbNoCardPowerOff;
#endif

    switch(*pulEvent)
    {
        case BUTTON_SD_DETECTED:
            #if (NO_CARD_POWER_OFF_EN)
            gbNoCardPowerOff = AHC_FALSE;
            #endif

            #if (POWER_ON_BUTTON_ACTION)
            if(AHC_WMSG_GetInfo()==WMSG_SHOW_FW_VERSION)
                break;
            #endif 
			AHC_ShowErrorDialog(WMSG_NONE);	// Dismiss Error Dialog
            AHC_WMSG_Draw(AHC_TRUE, WMSG_WAIT_INITIAL_DONE, 3);

            #if UVC_HOST_VIDEO_ENABLE
            if (uiGetCurrentState() == UI_BROWSER_STATE) {
                AHC_SetDisplayMode(DISPLAY_MODE_PIP_DISABLE);
                AHC_OS_SleepMs(1000);
            }
            #endif
            break;

        case BUTTON_SD_REMOVED:
            #if (NO_CARD_POWER_OFF_EN)
            gbNoCardPowerOff = AHC_TRUE;
            #endif

            #if (POWER_ON_BUTTON_ACTION)
            if(AHC_WMSG_GetInfo()==WMSG_SHOW_FW_VERSION)
                break;
            #endif 

            AHC_WMSG_Draw(AHC_TRUE, WMSG_PLUG_OUT_SD_CARD, 2);

            #if UVC_HOST_VIDEO_ENABLE
            if (uiGetCurrentState() == UI_BROWSER_STATE) {
                AHC_SetDisplayMode(DISPLAY_MODE_PIP_DISABLE);
                AHC_OS_SleepMs(1000);
            }
            #endif
            break;
            
        case BUTTON_CLEAR_WMSG:
            AHC_WMSG_Draw(AHC_FALSE, WMSG_NONE, 0);

            #if UVC_HOST_VIDEO_ENABLE
            if (uiGetCurrentState() == UI_BROWSER_STATE) {
                AHC_SetDisplayMode(DISPLAY_MODE_PIP_ENABLE);
            }
            #endif
        break;

        case BUTTON_UPDATE_CHARGE_ICON:
#if OSD_SHOW_BATTERY_STATUS
            if(AHC_TRUE == AHC_GetChargerStatus())
            {
                AHC_DrawChargingIcon();
            }
#endif
        break;
            
        case BUTTON_DEVICES_READY:
#ifdef CFG_POWER_ON_CLEAN_SCREEN     // MCU Panel, need clean Panel GRAM
            #if (TVOUT_PREVIEW_EN)
            if(!AHC_IsTVConnectEx())
            #endif
            #if (HDMI_PREVIEW_EN)
            if(!AHC_IsHdmiConnect())
            #endif
            {
                RTNA_LCD_FillColor(0x00000000);
                AHC_OS_SleepMs(100);
            }
#endif

            // All of devices initialized
            // Turn ON LCD backlight
            LedCtrl_LcdBackLightLock(AHC_FALSE);
            #if (TVOUT_PREVIEW_EN)
            if(!AHC_IsTVConnectEx())
            #endif
            #if (HDMI_PREVIEW_EN)
            if(!AHC_IsHdmiConnect())
            #endif
            {
        #if (SUPPORT_GSENSOR && POWER_ON_BY_GSENSOR_EN)
        #endif
                LedCtrl_LcdBackLight(AHC_TRUE);
            }
            
        break;

        case BUTTON_DC_CABLE_IN:
        case BUTTON_USB_DETECTED:
            AHC_SetShutdownByChargerOut(AHC_FALSE);
            break;
        case BUTTON_USB_B_DEVICE_DETECTED:
        case BUTTON_USB_B_DEVICE_REMOVED:
            if(uiSysState.CurrentState == UI_STATE_UNSUPPORTED ||
               uiSysState.CurrentState == UI_CLOCK_SETTING_STATE){
            AHC_SetCurKeyEventID(EVENT_NONE);
            }
            break;
        case BUTTON_UPDATE_MESSAGE:
#if defined(WIFI_PORT) && (WIFI_PORT == 1)
            WiFiStreamingLED_Timer_Start(100);
#endif
        break;

        default:
        break;  
    }
    return AHC_TRUE;
}

AHC_BOOL ubUIInitDone(void)
{
    return AhcMenuInit;
}

void uiStateMachine( UINT32 ulMsgId, UINT32 ulEvent, UINT32 ulParam)
{
#if defined(CFG_ENABLE_HDMI_HPD) || defined(CFG_ENABLE_TV_HPD)
    // ++ HDMI HPD / TV
    UINT32 ulButtonEvent = ulEvent;
    // -- HDMI HPD / TV
#endif

	if(AhcMenuClockInit && AIHC_GetOSDInitStatus())
	{
		StateSwitchMode(UI_CLOCK_SETTING_STATE);
		AhcMenuClockInit = AHC_FALSE;
	}

    if(AHC_FALSE == AhcMenuInit)
    {
        uiStateInitialize();      
        AhcMenuInit = AHC_TRUE;
    }
        
#if (FLIP_CTRL_METHOD & CTRL_BY_KEY) && (KEY_ROTATE_OSD_SNR != BUTTON_ID_UNKNOWN)
    if (ulEvent == KEY_ROTATE_OSD_SNR)
    {
        AHC_SetRotateSrc(SRC_KEY);
        printc("Rotate OSD By Key\r\n");
    }
#endif

    if (uBlkEvent ||
        uiStateProcessGlobalEvent( ulMsgId, &ulEvent, &ulParam ) == AHC_FALSE)
    {
#ifdef CFG_ENABLE_HDMI_HPD
        // ++ HDMI HPD
        if ((BUTTON_HDMI_REMOVED == ulButtonEvent) || (BUTTON_HDMI_DETECTED == ulButtonEvent))
        {
            // Unlock HDMI Connection Status
            AHC_LockHdmiConnectionStatus(AHC_FALSE, AHC_FALSE);
        }
        // -- HDMI HPD
#endif
#ifdef CFG_ENABLE_TV_HPD
        // ++ TV HPD
        if ((BUTTON_TV_REMOVED == ulButtonEvent) || (BUTTON_TV_DETECTED == ulButtonEvent))
        {
            // Unlock TV Connection Status
            AHC_LockTVConnectionStatus(AHC_FALSE, AHC_FALSE);
        }
        // -- TV HPD
#endif

        return;
    }

    if(uiStateProcessGlobalEvent2( ulMsgId, &ulEvent, &ulParam ) == AHC_FALSE)
    {
#ifdef CFG_ENABLE_HDMI_HPD
        // ++ HDMI HPD
        if ((BUTTON_HDMI_REMOVED == ulButtonEvent) || (BUTTON_HDMI_DETECTED == ulButtonEvent))
        {
            // Unlock HDMI Connection Status
            AHC_LockHdmiConnectionStatus(AHC_FALSE, AHC_FALSE);
        }
        // -- HDMI HPD
#endif
#ifdef CFG_ENABLE_TV_HPD
        // ++ TV HPD
        if ((BUTTON_TV_REMOVED == ulButtonEvent) || (BUTTON_TV_DETECTED == ulButtonEvent))
        {
            // Unlock TV Connection Status
            AHC_LockTVConnectionStatus(AHC_FALSE, AHC_FALSE);
        }
        // -- TV HPD
#endif

        return;
    }

    switch( uiSysState.CurrentState )
    {
#if (DSC_MODE_ENABLE)
        case UI_CAMERA_STATE    :
            ulEvent = KeyParser_CaptureEvent(ulMsgId, ulEvent, ulParam);
            StateCaptureMode(ulEvent);
        break;
#endif
             
        case UI_VIDEO_STATE     :
            if(CalibrationMode==0)
            {
                extern void OSD_Draw_TestMessage(UINT32 Left, UINT32 Top, char result, char*);
            
                if((FW_RUN_CALI_MODE_EN) || (AHC_IsCalibrationMode()))  // special mode
                {           
                    BurninMode      = 0;
                    CalibrationMode = 1;
                    OSD_Draw_TestMessage(10,10,4,"CalibrationMode v06");
                }
            }

            if (VIRTUAL_EVENT == ulEvent) {
                ulEvent = ulParam;
            } else {
                ulEvent = KeyParser_VideoRecEvent(ulMsgId, ulEvent, ulParam);
            }

            StateVideoRecMode(ulEvent);
        break;
             
        case UI_BROWSER_STATE   :
            ulEvent = KeyParser_ThumbnailEvent(ulMsgId, ulEvent, ulParam);
            StateBrowserMode(ulEvent);
        break;

        case UI_PLAYBACK_STATE  :
            if( JPGPB_MODE == GetCurrentOpMode())
            {
                ulEvent = KeyParser_JpgPlaybackEvent(ulMsgId, ulEvent, ulParam);
                StatePhotoPlaybackMode(ulEvent);
            }
            else if( MOVPB_MODE == GetCurrentOpMode() )
            {
                ulEvent = KeyParser_MovPlaybackEvent(ulMsgId, ulEvent, ulParam);
                StateMoviePlaybackMode(ulEvent);
            }
            else if( AUDPB_MODE == GetCurrentOpMode() )
            {
                ulEvent = KeyParser_AudPlaybackEvent(ulMsgId, ulEvent, ulParam);
                StateAudioPlaybackMode(ulEvent);
            }
            else if( JPGPB_MOVPB_MODE == GetCurrentOpMode() )
            {
                UINT32 uiCurrentIdx;
                UINT8  ubFileType;
                AHC_UF_GetCurrentIndex(&uiCurrentIdx);
                AHC_UF_GetFileTypebyIndex(uiCurrentIdx, &ubFileType);
                if ( ubFileType == DCF_OBG_JPG ) {
                    ulEvent = KeyParser_JpgPlaybackEvent(ulMsgId, ulEvent, ulParam);
                    StatePhotoPlaybackMode(ulEvent);
                }
                else {
                    ulEvent = KeyParser_MovPlaybackEvent(ulMsgId, ulEvent, ulParam);
                    StateMoviePlaybackMode(ulEvent);
                }   
            }
        break;
        
#if (SLIDESHOW_EN)
        case UI_SLIDESHOW_STATE:
            ulEvent = KeyParser_SlideShowEvent(ulMsgId, ulEvent, ulParam);
            StateSlideShowMode(ulEvent);
        break;
#endif        
        
        case UI_MSDC_STATE       :
        case UI_PCCAM_STATE      :
            ulEvent = KeyParser_MsdcEvent(ulMsgId, ulEvent, ulParam);
            StateMSDCMode(ulEvent);
        break;

#if (HDMI_ENABLE)
        case UI_HDMI_STATE       :
            ulEvent = KeyParser_HdmiEvent(ulMsgId, ulEvent, ulParam);
            StateHDMIMode(ulEvent);
        break;
#endif

#if (TVOUT_ENABLE)        
        case UI_TVOUT_STATE       :
            ulEvent = KeyParser_TVEvent(ulMsgId, ulEvent, ulParam);
            StateTVMode(ulEvent);
        break;
#endif        
        
        case UI_CAMERA_MENU_STATE:
            MenuStateCameraMode(ulEvent, ulParam);
        break;

        case UI_VIDEO_MENU_STATE:
            MenuStateVideoMode(ulEvent, ulParam);
	        #if (HDMI_ENABLE)
		    if(ulEvent == BUTTON_HDMI_DETECTED)
		    {
		        ulEvent = EVENT_HDMI_DETECT;
	            StateVideoRecMode(ulEvent);
		    }
            #endif
        break;

        case UI_PLAYBACK_MENU_STATE:
            MenuStatePlaybackMode(ulEvent, ulParam);
        break;
        
        case UI_CLOCK_SETTING_STATE:
            MenuStateClockSettingsMode(ulEvent, ulParam);
        break;

#if (SD_UPDATE_FW_EN)   
        case UI_SD_UPDATE_STATE:
            printc("UI_SD_UPDATE_STATE \n");
            AHC_SDUpdateMode();//lucas
        break;
#endif

#if (USB_MODE_SELECT_EN)
        case UI_USBSELECT_MENU_STATE:
            MenuStateUSBModeSelectMode(ulEvent, ulParam);
        break;
#endif

#if (UI_MODE_SELECT_EN)
        case UI_MODESELECT_MENU_STATE:
            MenuStateUIModeSelectMode(ulEvent, ulParam);
        break;
#endif

#if (BROWSER_MODE_SELECT_EN)
        case UI_BROWSELECT_MENU_STATE:
            MenuStateBrowserModeSelectMode(ulEvent, ulParam);
        break;
#endif

#if (POWER_ON_MENU_SET_EN)
        case UI_POWERON_SET:
            MenuStatePowerOnSettingMode(ulEvent, ulParam);
        break;
#endif
		case UI_NET_PLAYBACK_STATE       :
            ulEvent = KeyParser_NetPlaybackEvent(ulMsgId, ulEvent, ulParam);
            StateNetPlaybackMode(ulEvent);
        break;
        default                       : 
        break;
    }

#ifdef CFG_ENABLE_HDMI_HPD
    // ++ HDMI HPD
    if ((BUTTON_HDMI_REMOVED == ulButtonEvent) || (BUTTON_HDMI_DETECTED == ulButtonEvent))
    {
        // Unlock HDMI Connection Status
        AHC_LockHdmiConnectionStatus(AHC_FALSE, AHC_FALSE);
    }
    // ++ HDMI HPD
#endif
#ifdef CFG_ENABLE_TV_HPD
    // ++ TV HPD
    if ((BUTTON_TV_REMOVED == ulButtonEvent) || (BUTTON_TV_DETECTED == ulButtonEvent))
    {
        // Unlock TV Connection Status
        AHC_LockTVConnectionStatus(AHC_FALSE, AHC_FALSE);
    }
    // ++ TV HPD
#endif
}

void PowerOffCB(void)
{
    printc("PowerOffCB: The second time Power Off !!!\r\n");
    PMUCtrl_Power_Off();
}

extern void OSDDraw_EnterDrawing(UINT8* pbID0, UINT8* pbID1);
extern void OSDDraw_ExitDrawing(UINT8* pbID0, UINT8* pbID1);

void PowerOffClearBuff(void)
{
	AHC_BOOL bIsTvOut;
	UINT8 bID0,bID1;

	bIsTvOut = TVFunc_IsInTVMode();

	{
	OSDDraw_EnterDrawing(&bID0, &bID1);
	AHC_OSDClearBuffer(bID0);
	OSDDraw_ExitDrawing(&bID0, &bID1);
	}

	BEGIN_LAYER(0);
	AHC_OSDClearBuffer(0);
	END_LAYER(0);

	if(UI_TVOUT_STATE == uiGetCurrentState() || UI_BROWSER_STATE == uiGetCurrentState())
	{
		AIHC_DrawReservedOSD(TRUE);
		BEGIN_LAYER(32);
		AHC_OSDClearBuffer(32);
		END_LAYER(32);
		
		BEGIN_LAYER(33);
		AHC_OSDClearBuffer(33);
		END_LAYER(33);
		AIHC_DrawReservedOSD(FALSE);
	}

}

void uiPowerOffPicture(void)
{
    AHC_OS_TMRID timerID = 0xFF;
    UINT16  WaitSecondTimePowerOff = 5000;  //unit: ms

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }
    
    printc("The second time Power Off: %d ms\r\n", WaitSecondTimePowerOff);
    timerID = AHC_OS_StartTimer(MS_TO_TICK(WaitSecondTimePowerOff), OS_TMR_OPT_ONE_SHOT, (OS_TMR_CALLBACK)&PowerOffCB, NULL);
    
    if((timerID != 0xFF) && (timerID != 0xFE))
    {
        AHC_SetMode(AHC_MODE_IDLE);
        //AHC_OS_StopTimer(timerID, OS_TMR_OPT_NONE);
    }

	PowerOffClearBuff();


#if defined(WIFI_PORT) && (WIFI_PORT == 1)
{	
		AHC_WiFi_Switch(AHC_FALSE);
	  WiFiStreamingLED_Timer_Stop();
}
#endif

    bShowPowerOffJpg = AHC_TRUE;

#ifdef CFG_NOSD_POWEROFF
    if(AHC_FALSE ==AHC_IsSDInserted())
    {
        #ifdef CFG_BOOT_POWER_OFF_VIBRATION
            AHC_Vibration_Enable(400);
        #endif

        LedCtrl_FlickerLedEx(AHC_GetVideoModeLedGPIOpin(), 5, 100);

        #ifdef CFG_BOOT_POWER_OFF_VIBRATION
        AHC_Vibration_Enable(400);
        #endif

        if(pf_GetLaserLedStatus())
            LedCtrl_FlickerLedEx(LED_GPIO_LASER, 5, 80);
    }
   else
    {
        #ifdef CFG_BOOT_POWER_OFF_VIBRATION
            AHC_Vibration_Enable(400);
        #endif
             AHC_OS_SleepMs(500);
        #ifdef CFG_BOOT_POWER_OFF_VIBRATION
            AHC_Vibration_Enable(400);
        #endif
             AHC_OS_SleepMs(600);
        if(pf_GetLaserLedStatus())
            LedCtrl_FlickerLedEx(LED_GPIO_LASER,10,200);
    }
#else
    #ifdef CFG_BOOT_POWER_OFF_VIBRATION
        AHC_Vibration_Enable(300);
    #endif
#endif

#ifdef CFG_BOOT_IGNORE_POWER_OFF_PIC //may be defined in config_xxx.h
    // NOP
#else
    AHC_PowerOffPicture("SF:0:\\goodbye.jpg");
#endif

#ifdef CFG_BOOT_IGNORE_POWER_OFF_PIC_ON_LCD     // No Panel, TV/HDMI-Out only
    // NOP
#else
    // If Backlight is off then turn on,
    // It can avoid LCD flash preview screen between Power off submenu and Goodbye!!!
    if (!HDMIFunc_IsInHdmiMode() && !TVFunc_IsInTVMode())
    {
        if (!LedCtrl_GetBacklightStatus()) 
            LedCtrl_LcdBackLight(AHC_TRUE);
    }

    AHC_OS_Sleep(Sleep_MS(1000));
#endif

#if defined(CFG_BOOT_POWER_OFF_BY_POWER_PIN) && defined(DEVICE_GPIO_CUS_POWER_PIN_EN) && (DEVICE_GPIO_CUS_POWER_PIN_EN != MMP_GPIO_MAX)
    AHC_OS_Sleep(Sleep_MS(1000));
    MMPF_PIO_EnableOutputMode(DEVICE_GPIO_CUS_POWER_PIN_EN, MMP_TRUE, MMP_TRUE);
    MMPF_PIO_SetData(DEVICE_GPIO_CUS_POWER_PIN_EN, DEVICE_GPIO_CUS_POWER_OFF);
#endif
}

/*
 * Block Events when system in busy, like as
 * 1. Battery Low and PowerOff detected
 * 2. <Please add on here>
 */
void uiBlockEvent(int arg)
{
    uBlkEvent = arg;
}

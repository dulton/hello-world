/*===========================================================================
 * Include files
 *===========================================================================*/

#include "customer_config.h"
#include "AHC_Config_SDK.h"
#include "AHC_Parameter.h"
#include "AHC_Warningmsg.h"
#include "AHC_Message.h"
#include "AHC_Menu.h"
#include "AHC_Os.h"
#include "AHC_General.h"
#include "AHC_Display.h"
#include "AHC_Utility.h"
#include "AHC_General.h"
#include "AIHC_Display.h"
#include "ColorDefine.h"
#include "mmp_lib.h"
#include "MenuCommon.h"
#include "MenuDrawCommon.h"
#include "OsdStrings.h"
#include "IconDefine.h"
#include "mmps_display.h"
#include "mmps_3gprecd.h"
#include "MenuSetting.h"
#include "ait_utility.h"
#include "ShowOSDFunc.h"
#include "StateHDMIFunc.h"
#include "lib_retina.h"
#include "lcd_common.h"
#if (ENABLE_ADAS_LDWS||ENABLE_ADAS_FCWS)
#include "ldws.h"
#include "ldws_cfg.h"
#include "GPS_Ctl.h"

// Maybe defined in config_sdk.h
#ifndef LDWS_L_X_OFFSET
#define LDWS_L_X_OFFSET         (0)
#endif

#ifndef LDWS_L_Y_OFFSET
#define LDWS_L_Y_OFFSET         (0)
#endif

#ifndef LDWS_R_X_OFFSET
#define LDWS_R_X_OFFSET         (0)
#endif

#ifndef LDWS_R_Y_OFFSET
#define LDWS_R_Y_OFFSET         (0)
#endif

#ifndef LDWS_Y_INC_LENGTH
#define LDWS_Y_INC_LENGTH       (35)
#endif

#endif

#if (ENABLE_ADAS_FCWS)
#include "fcws.h"
#endif

/*===========================================================================
 * Macro define
 *===========================================================================*/

#define USE_WMSG_SEMI (1)

#if defined(WIFI_PORT) && (WIFI_PORT == 1)
    #include "netapp.h"
#endif

/*===========================================================================
 * Global variable
 *===========================================================================*/

AHC_WMSG_CFG    m_WMSG;
UINT8           m_WMSGShowTime  = 10;
UINT32          m_LensError     = 0;

static WMSG_INFO     m_WMSGInfo = WMSG_NONE;
#if (USE_WMSG_SEMI)
AHC_OS_SEMID    m_WMSGSemID = 0xFF;
#endif

#if (ENABLE_ADAS_LDWS)
static AHC_BOOL m_bDrawLDWS = AHC_FALSE;
#endif

#if (DRIVING_SAFETY_REMIND_HEADLIGHT)
static AHC_BOOL m_bDrawRemindHeadlight = AHC_FALSE;
#endif

UINT32 m_uiWMSGStringID[END_OF_WMSG]={0};

#ifndef BM_ICON_WMSG
#define BM_ICON_WMSG        bmIcon_WMSG
#endif

/*===========================================================================
 * Extern variable
 *===========================================================================*/

extern MMP_BOOL CalibrationMode;
extern AHC_OSD_INSTANCE *m_OSD[];

#if defined(CAR_DV)
extern AHC_BOOL Deleting_InBrowser;
extern AHC_BOOL Protecting_InBrowser;
extern AHC_BOOL PowerOff_InProcess;
#endif

extern MMP_USHORT   gsAhcCurrentSensor;
extern AHC_BOOL     gbAhcDbgBrk;
/*===========================================================================
 * Extern function
 *===========================================================================*/

extern AHC_BOOL AHC_SendAHLMessage(UINT32 ulMsgID, UINT32 ulParam1, UINT32 ulParam2);
extern int      sprintf(char * /*s*/, const char * /*format*/, ...);
extern int      AHC_GetMinorVersion(void);

#if (ENABLE_ADAS_LDWS)
extern MMP_UBYTE ADAS_CTL_GetLDWSAttr(ldws_params_t *ldws_attr, MMP_LONG *alert);
#endif
#if (ENABLE_ADAS_FCWS)
extern MMP_UBYTE ADAS_CTL_GetFCWSAttr(fcws_info_t *pfcws_info);
#endif

static int      find_newline(char *line1, char *line2);
static unsigned char find_newline2(char *line1, unsigned char len);

/*===========================================================================
 * Main body
 *===========================================================================*/

#ifdef CFG_CUS_DRAW_ICON_WSG
AHC_BOOL AHC_WMSG_Config(void)
{
    m_WMSG.DisplayID        = 0;                    // Fixed at 0
    m_WMSG.StartX           = AHC_WMSG_START_X;     // default: 0, it might be defined in AHC_Config_SDK.h
    m_WMSG.StartY           = AHC_WMSG_START_Y;     // default: 60, it might be defined in AHC_Config_SDK.h
    m_WMSG.DisplayWidth     = AHC_WMSG_WIDTH;       // default: 320, it might be defined in AHC_Config_SDK.h
    m_WMSG.DisplayHeight    = AHC_WMSG_HEIGHT;      // default: 120, it might be defined in AHC_Config_SDK.h
    m_WMSG.byBkColor        = AHC_WMSG_BK_COLOR;    // default: 0xFF00D0D0, it might be defined in AHC_Config_SDK.h
    m_WMSG.byBoundColor     = AHC_WMSG_BOUND_COLOR; // default: 0xAA009090, it might be defined in AHC_Config_SDK.h

#ifdef STRETCH_X
    m_WMSG.StartY           = STRETCH_Y(m_WMSG.StartY);
    m_WMSG.DisplayWidth     = STRETCH_X(m_WMSG.DisplayWidth);
    m_WMSG.DisplayHeight    = STRETCH_Y(m_WMSG.DisplayHeight);
#endif

#if (USE_WMSG_SEMI)
    if (m_WMSGSemID >= 0xFE)
        m_WMSGSemID         = AHC_OS_CreateSem(1);
#endif

    return AHC_TRUE;
}
#else
AHC_BOOL AHC_WMSG_Config(void)
{
#ifdef CAR_DV
    m_WMSG.DisplayID        = 0;    // Fixed at 0
    m_WMSG.StartX           = 0;
    m_WMSG.StartY           = 60;
    m_WMSG.DisplayWidth     = 320;
    m_WMSG.DisplayHeight    = 120;
    #ifdef CUS_WMSG_BK_COLOR
    m_WMSG.byBkColor        = CUS_WMSG_BK_COLOR;
    #else
    m_WMSG.byBkColor        = 0xFF00D0D0;
    #endif
    m_WMSG.byBoundColor     = 0xAA009090;

    #ifdef STRETCH_X
    m_WMSG.StartY           = STRETCH_Y(m_WMSG.StartY);
    m_WMSG.DisplayWidth     = STRETCH_X(m_WMSG.DisplayWidth);
    m_WMSG.DisplayHeight    = STRETCH_Y(m_WMSG.DisplayHeight);
    #endif
#else
    m_WMSG.DisplayID        = 0;
    m_WMSG.StartX           = 20; //50; //71;
    m_WMSG.StartY           = 91;
    m_WMSG.DisplayWidth     = 240; //220; //178;
    m_WMSG.DisplayHeight    = 90;
    m_WMSG.byBkColor        = 0xAA3A3A3A;//0xAA808080;
    m_WMSG.byBoundColor     = 0xAA3A3A3A;//0xAA808080;
#endif

#if (USE_WMSG_SEMI)
    if (m_WMSGSemID >= 0xFE)
        m_WMSGSemID         = AHC_OS_CreateSem(1);
#endif

    return AHC_TRUE;
}
#endif

void AHC_WMSG_Init(void)
{
#if defined(CCIR656_OUTPUT_ENABLE) && (CCIR656_OUTPUT_ENABLE)
    return;
#endif

    AHC_WMSG_Config();
}

WMSG_INFO AHC_WMSG_GetInfo(void)
{
    return m_WMSGInfo;
}

AHC_BOOL AHC_WMSG_States(void)
{
#if defined(CCIR656_OUTPUT_ENABLE) && (CCIR656_OUTPUT_ENABLE)
    return AHC_TRUE;
#endif

    if(m_WMSGInfo == WMSG_NONE) {
        return AHC_FALSE;
    }
    else {
        return AHC_TRUE;
    }
}

AHC_BOOL AHC_WMSG_IsTimeOut(UINT32 msTimer)
{
#if defined(CCIR656_OUTPUT_ENABLE) && (CCIR656_OUTPUT_ENABLE)
    return AHC_TRUE;
#endif

    if(m_WMSGShowTime == 0)
        return AHC_TRUE;

    if(msTimer > (UINT32)m_WMSGShowTime*1000) {

#if (POWER_ON_BUTTON_ACTION)
        if(m_WMSGInfo==WMSG_SHOW_FW_VERSION)
        {
            AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, KEY_POWER_OFF, 0);
        }
#endif

        AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_CLEAR_WMSG, 0);
        return AHC_TRUE;
    }
    else {
        return AHC_FALSE;
    }
}

AHC_BOOL AHC_WMSG_SetLensError(UINT32 Number)
{
    m_LensError = Number;

    return AHC_TRUE;
}

AHC_BOOL AHC_WMSG_GetLensError(UINT32 *Number)
{
    *Number = m_LensError;

    return AHC_TRUE;
}

void AHC_WMSG_SetStringID(WMSG_INFO sWMSGInfoIndex, UINT32 uiStringID)
{
    if(sWMSGInfoIndex >= END_OF_WMSG){
        printc(FG_RED("%s,%d,%d error!\r\n"), __func__, __LINE__, sWMSGInfoIndex);
        return;
    }

    m_uiWMSGStringID[sWMSGInfoIndex] = uiStringID;
}

AHC_BOOL AHC_WMSG_Draw(AHC_BOOL Enable, WMSG_INFO WMSGInfo, UINT8 WMSGShowTime)
{
#if !defined(NO_PANEL) || defined(TV_ONLY)

    AHC_DISPLAY_OUTPUTPANEL  OutputDevice;
    RECT        StrRECT;
    UINT32      iStrID;
    UINT8       bLine = 1;
    // Using static to save the stack size
    static char TempStr[200];
    static char Temp1Str[100];
    static char Temp2Str[100];
    UINT16      Width, Height;
    UINT32      iVal[2];
    UINT8       bLine1MaxLeng = 18;

#ifdef CAR_DV

    /* The WARN MESSAGE should be OPENED and CLOSED in Browser mode */
    if(PowerOff_InProcess)
    {
        return AHC_TRUE;
    }
#else   // CAR_DV
    AHC_BOOL    AlphaLevel = 1;
    static AHC_BOOL m_bDrawCardError = AHC_FALSE;
    UINT32  StartX,StartY;
    extern  AHC_BOOL m_bSkipFirstBootSDCheck;

    #if (SUPPORT_HDMI)
    if(HDMIFunc_GetStatus()) { //TODO
        return AHC_TRUE;
    }
    #endif

    #if (SUPPORT_TV)
    if(TVFunc_Status()) { //TODO
        return AHC_TRUE;
    }
    #endif
#endif  // CAR_DV

#if defined(CCIR656_OUTPUT_ENABLE) && (CCIR656_OUTPUT_ENABLE)
    return AHC_TRUE;
#endif

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return AHC_TRUE; }

#if (SUPPORT_SYS_CALIBRATION)
    if(uiGetCurrentState() == UI_SYS_CALIBRATION_STATE) {
        return AHC_TRUE;
    }
#endif

#if (ENABLE_PIR_MODE)
    #if (PIR_POWEROFF_LCD_BACKLIGHT)
    if(AHC_IsPIRStart()) {
        DrawVideo_PIRRelatedWarning(Enable, WMSGInfo, WMSGShowTime);
        return AHC_TRUE;
    }
    #endif
#endif

    if((FW_RUN_CALI_MODE_EN) || (CalibrationMode))  // special mode
    {
        Enable = 0;
    }

    #if (USE_WMSG_SEMI)
    // give a timeout to allow failure to avoid dead-lock
    if(AHC_OS_AcquireSem(m_WMSGSemID, 200) != OS_NO_ERR)
    {
        printc("--E-- m_WMSGSemID OSSemPost: Fail!! \r\n");
        return AHC_TRUE;
    }
    #endif

    #if defined(ENABLE_GUI_SUPPORT_MULTITASK)
    BEGIN_LAYER(m_WMSG.DisplayID);
    #elif defined(SLIDE_STRING)
    //AHC_LockGUI(); instead by BEGIN_GUI_DRAW(m_WMSG.DisplayID)
    #endif

    // Drop this time when WMSGInfo is same as show or WMSG_NONE to save task loading
    if (Enable && ((WMSG_NONE == WMSGInfo) || (WMSGInfo == m_WMSGInfo))) {
        m_WMSGShowTime = WMSGShowTime;      // update show time only
        goto Label_AHC_WMSG_END;
    }

    // Update m_WMSG again.
    // Wrong configure when Panel size is not 320x240 under TV/HDMI out.
    AHC_WMSG_Config();

    AHC_GetDisplayOutputDev(&OutputDevice);

    if (OutputDevice == MMP_DISPLAY_SEL_MAIN_LCD) {
#ifdef CAR_DV
        if (OSD_DISPLAY_SCALE_LCD) {
            iVal[0] = OSD_DISPLAY_SCALE_LCD - 1;
            iVal[1] = OSD_DISPLAY_SCALE_LCD - 1;
            AHC_OSDSetDisplayAttr(m_WMSG.DisplayID, AHC_OSD_ATTR_DISPLAY_SCALE, iVal);
        }
        else {
            iVal[0] = MMP_DISPLAY_DUPLICATE_1X; // No Scale for Message Box in LCD
            iVal[1] = MMP_DISPLAY_DUPLICATE_1X;
            AHC_OSDSetDisplayAttr(m_WMSG.DisplayID, AHC_OSD_ATTR_DISPLAY_SCALE, iVal);
        }
        m_WMSG.StartX = (AHC_GET_ATTR_OSD_W(m_WMSG.DisplayID) - m_WMSG.DisplayWidth) / 2;
        m_WMSG.StartY = (AHC_GET_ATTR_OSD_H(m_WMSG.DisplayID) - m_WMSG.DisplayHeight) / 2;
        iVal[0] = (LCD_ATTR_DISP_W - AHC_GET_ATTR_OSD_W(m_WMSG.DisplayID)*OSD_DISPLAY_SCALE_LCD) / 2;
        iVal[1] = (LCD_ATTR_DISP_H - AHC_GET_ATTR_OSD_H(m_WMSG.DisplayID)*OSD_DISPLAY_SCALE_LCD) / 2;
		
        AHC_OSDSetDisplayAttr(m_WMSG.DisplayID, AHC_OSD_ATTR_DISPLAY_OFFSET, iVal);
#else
        StartX = m_WMSG.StartX;
        StartY = m_WMSG.StartY;
#endif
    }
    else {
#ifdef CAR_DV
        m_WMSG.StartX = 0;
        m_WMSG.StartY = 1;
        iVal[0] = MMP_DISPLAY_DUPLICATE_2X;
        iVal[1] = MMP_DISPLAY_DUPLICATE_2X;
        AHC_OSDSetDisplayAttr(m_WMSG.DisplayID, AHC_OSD_ATTR_DISPLAY_SCALE, iVal);

        switch(OutputDevice) {
        case MMP_DISPLAY_SEL_HDMI:
            AHC_GetHdmiDisplayWidthHeight(&Width, &Height);
            iVal[0] = (Width - m_WMSG.DisplayWidth  * (iVal[0] + 1)) / 2;
        break;

        case MMP_DISPLAY_SEL_NTSC_TV:
            AHC_GetNtscTvDisplayWidthHeight(&Width, &Height);
            iVal[0] = (Width - m_WMSG.DisplayWidth  * (iVal[0] + 1)) / 2;
        break;

        case MMP_DISPLAY_SEL_PAL_TV:
            AHC_GetPalTvDisplayWidthHeight(&Width, &Height);
            iVal[0] = (Width - m_WMSG.DisplayWidth  * (iVal[0] + 1)) / 2+10;
        break;
        }

        iVal[1] = (Height - m_WMSG.DisplayHeight * (iVal[1] + 1)) / 2;
        AHC_OSDSetDisplayAttr(m_WMSG.DisplayID, AHC_OSD_ATTR_DISPLAY_OFFSET, iVal);

#else

        switch(OutputDevice) {
        case MMP_DISPLAY_SEL_HDMI:
            AHC_GetHdmiDisplayWidthHeight(&Width, &Height);
        break;

        case MMP_DISPLAY_SEL_NTSC_TV:
            AHC_GetNtscTvDisplayWidthHeight(&Width, &Height);
        break;

        case MMP_DISPLAY_SEL_PAL_TV:
            AHC_GetPalTvDisplayWidthHeight(&Width, &Height);
        break;
        }

        StartX = 1;
        StartY = 1;
        iVal[0] = (Width - m_WMSG.DisplayWidth)/2;
        iVal[1] = (Height - m_WMSG.DisplayHeight)/2;
        AHC_OSDSetDisplayAttr(m_WMSG.DisplayID, AHC_OSD_ATTR_DISPLAY_OFFSET, iVal);

#endif

        iVal[0] = 1;
        iVal[1] = OSD_COLOR_TRANSPARENT;
        AHC_OSDSetDisplayAttr(m_WMSG.DisplayID, AHC_OSD_ATTR_TRANSPARENT_ENABLE, iVal );
    }

    if (Enable) {
        MEMSET(TempStr,  0, sizeof(TempStr));
        MEMSET(Temp1Str, 0, sizeof(Temp1Str));
        MEMSET(Temp2Str, 0, sizeof(Temp2Str));

        StrRECT.uiLeft      = m_WMSG.StartX;
        StrRECT.uiTop       = m_WMSG.StartY;
        StrRECT.uiWidth     = m_WMSG.DisplayWidth;
        StrRECT.uiHeight    = m_WMSG.DisplayHeight;

        #ifdef CAR_DV
        // NOP
        #else
        if(WMSGInfo == WMSG_HDMI_TV) {
            goto Label_AHC_WMSG_END;
        }
        #endif

        ////////////////////////////////////////////////////////////////
        switch(WMSGInfo) {
#ifndef CAR_DV
        case WMSG_NO_CARD2_2:

            iStrID = IDS_DS_Insert_SD2_Msg;
        break;

        case WMSG_CARD2_FULL:
            iStrID = IDS_DS_Backup_to_SD2_Warn;
            AlphaLevel = 0;
        break;

        case WMSG_BACKUP_FINISH:
            iStrID = IDS_DS_SD_Backup_Complete_Msg;
            AlphaLevel = 0;
        break;
#endif
        case WMSG_CARD_ERROR:
#ifdef CAR_DV
            iStrID = IDS_DS_MSG_CARD_ERROR;

            switch(MenuSettingConfig()->uiLanguage) {
            #if (MENU_GENERAL_LANGUAGE_SPANISH_EN)
            case LANGUAGE_SPANISH:
                bLine = 2;
                bLine1MaxLeng = 36;
            break;
            #endif
            #if (MENU_GENERAL_LANGUAGE_PORTUGUESE_EN)
            case LANGUAGE_PORTUGUESE:
                bLine = 2;
                bLine1MaxLeng = 37;
            break;
            #endif
            #if (MENU_GENERAL_LANGUAGE_RUSSIAN_EN)
            case LANGUAGE_RUSSIAN:
                //bLine = 2;
                //bLine1MaxLeng = 25;
            break;
            #endif
            }
#else   // CAR_DV
            iStrID = IDS_DS_No;
            AHC_OSDSetActive(m_WMSG.DisplayID, 0);
            OSDDraw_SetAlphaBlending(m_WMSG.DisplayID, 1);

            //AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
            AHC_OSDSetColor(0x880000AA);
            AHC_OSDDrawFillRoundedRect(m_WMSG.DisplayID, 65, 72, 65+188-1, 72+95-1, 2);
            AHC_OSDDrawBitmap(m_WMSG.DisplayID, &bmIcon_SD_Error, 65, 72);

            m_bDrawCardError = AHC_TRUE;
            AHC_OSDSetActive(m_WMSG.DisplayID, 1);
            m_WMSGShowTime = WMSGShowTime;
            m_WMSGInfo     = WMSGInfo;
            goto Label_AHC_WMSG_END;

#endif  // CAR_DV
        break;

        case WMSG_STORAGE_FULL:
            #ifdef CAR_DV
            iStrID = IDS_DS_MSG_STORAGE_FULL;
            #else
            iStrID = IDS_DS_No;
            #endif
        break;

        case WMSG_PARKING_OPERATING_DESCRIPTION:
            #ifdef CFG_CUS_DRAW_MULTI_LINE // TBD
            iStrID = IDS_DS_MSG_PARKING_OPERATING_DESCRIPTION;
            #else
            iStrID = m_uiWMSGStringID[WMSG_PARKING_OPERATING_DESCRIPTION];
            #endif

            switch (MenuSettingConfig()->uiLanguage)
            {
                #if (MENU_GENERAL_LANGUAGE_ENGLISH_EN)
                case LANGUAGE_ENGLISH:
                    bLine = 2;
                    bLine1MaxLeng = 20;
                break;
                #endif
                #if (MENU_GENERAL_LANGUAGE_TCHINESE_EN)
                case LANGUAGE_TCHINESE:

                break;
                #endif
                #if (MENU_GENERAL_LANGUAGE_JAPANESE_EN)
                case LANGUAGE_JAPANESE:

                break;
                #endif
                #if (MENU_GENERAL_LANGUAGE_KOREAN_EN)
                case LANGUAGE_KOREAN:
                    bLine = 2;
                    bLine1MaxLeng = 10;
                break;
                #endif
            }
        break;

        case WMSG_MOTION_OPERATING_DESCRIPTION:
            #ifdef CFG_CUS_DRAW_MULTI_LINE // TBD
            iStrID = IDS_DS_MSG_MOTION_OPERATING_DESCRIPTION;
            #else
            iStrID = m_uiWMSGStringID[WMSG_MOTION_OPERATING_DESCRIPTION];
            #endif

            switch (MenuSettingConfig()->uiLanguage)
            {
                #if (MENU_GENERAL_LANGUAGE_ENGLISH_EN)
                case LANGUAGE_ENGLISH:
                    bLine = 2;
                    bLine1MaxLeng = 23;
                break;
                #endif
                #if (MENU_GENERAL_LANGUAGE_TCHINESE_EN)
                case LANGUAGE_TCHINESE:

                break;
                #endif
                #if (MENU_GENERAL_LANGUAGE_JAPANESE_EN)
                case LANGUAGE_JAPANESE:

                break;
                #endif
                #if (MENU_GENERAL_LANGUAGE_KOREAN_EN)
                case LANGUAGE_KOREAN:
                    bLine = 2;
                    bLine1MaxLeng = 10;
                break;
                #endif
            }
        break;

        case WMSG_MSG_EMERGENCY_FILE_FULL:
            //call AHC_WMSG_SetStringID(WMSG_MSG_EMERGENCY_FILE_FULL, StringID) before this.
            iStrID = m_uiWMSGStringID[WMSG_MSG_EMERGENCY_FILE_FULL];
            //iStrID = IDS_DS_MSG_EMERGENCY_FILE_FULL;
        break;

        case WMSG_NO_CARD:
            #ifdef CAR_DV
            iStrID = IDS_DS_MSG_NO_CARD;
            #else
            m_bSkipFirstBootSDCheck = TRUE;
            AHC_PlaySoundEffect(AHC_SOUNDEFFECT_NO_SD_CARD);
            iStrID = IDS_DS_No_SD_Card;
            #endif
        break;

        case WMSG_LOW_BATTERY:
            #ifdef CAR_DV
            iStrID = IDS_DS_MSG_LOW_BATTERY;
            #else
            iStrID = IDS_DS_No;
            #endif
        break;

        case WMSG_FILE_ERROR:
            #ifdef CAR_DV
            iStrID = IDS_DS_MSG_FILE_ERROR;
            #else
            iStrID = IDS_DS_No;
            #endif
        break;

        case WMSG_CARD_LOCK:
            #ifdef CAR_DV
            iStrID = IDS_DS_MSG_CARD_LOCK;
            #else
            iStrID = IDS_DS_No;
            #endif
        break;

        #ifndef CAR_DV
        case WMSG_FORMAT_SD_PROCESSING:
            iStrID = IDS_DS_No;
        break;
        #endif

        case WMSG_CARD_SLOW:
            #ifdef CAR_DV
            iStrID = IDS_DS_MSG_CARD_SLOW;
            bLine = 2;
            #else
            iStrID = IDS_DS_No;
            #endif
        break;

        case WMSG_CANNOT_DELETE:
            #ifdef CAR_DV
            iStrID = IDS_DS_MSG_CANNOT_DELETE;
            #else
            iStrID = IDS_DS_No;
            #endif
            #ifdef CFG_CUS_DRAW_MULTI_LINE // TBD
            switch (MenuSettingConfig()->uiLanguage)
            {
                #if (MENU_GENERAL_LANGUAGE_ENGLISH_EN)
                case LANGUAGE_ENGLISH:
                break;
                #endif
                #if (MENU_GENERAL_LANGUAGE_TCHINESE_EN)
                case LANGUAGE_TCHINESE:

                break;
                #endif
                #if (MENU_GENERAL_LANGUAGE_JAPANESE_EN)
                case LANGUAGE_JAPANESE:
                    bLine = 2;
                    bLine1MaxLeng = 22;
                break;
                #endif
                #if (MENU_GENERAL_LANGUAGE_KOREAN_EN)
                case LANGUAGE_KOREAN:
                    bLine = 2;
                    bLine1MaxLeng = 22;
                break;
                #endif
            }
            #endif
        break;

        case WMSG_PLUG_OUT_SD_CARD:
            #ifdef CAR_DV
            iStrID = IDS_DS_MSG_PLUG_OUT_SD;
            #else
            iStrID = IDS_DS_No;
            #endif
        break;

        case WMSG_WAIT_INITIAL_DONE:
            #ifdef CAR_DV
            iStrID = IDS_DS_MSG_WAIT_INITIAL_DONE;
            #else
            iStrID = IDS_DS_No;
            #endif
        break;

        case WMSG_INSERT_SD_AGAIN:
            #ifdef CAR_DV
            iStrID = IDS_DS_MSG_INSERT_SD_AGAIN;
            #else
            iStrID = IDS_DS_No;
            #endif
        break;

        case WMSG_FORMAT_SD_CARD_OK:
            #ifdef CAR_DV
            iStrID = IDS_DS_MSG_FORMAT_SD_CARD_OK;
            #else
            iStrID = IDS_DS_No;
            #endif
        break;

        case WMSG_FORMAT_SD_CARD_FAIL:
            #ifdef CAR_DV
            iStrID = IDS_DS_MSG_FORMAT_SD_CARD_FAIL;
            #else
            iStrID = IDS_DS_No;
            #endif
        break;

#ifdef CAR_DV
        case WMSG_ADAPTER_ERROR:
            iStrID = IDS_DS_MSG_ADAPTER_ERROR;

            switch(MenuSettingConfig()->uiLanguage) {
            #if (MENU_GENERAL_LANGUAGE_SPANISH_EN)
            case LANGUAGE_SPANISH:
            break;
            #endif
            #if (MENU_GENERAL_LANGUAGE_PORTUGUESE_EN)
            case LANGUAGE_PORTUGUESE:
                bLine = 2;
                bLine1MaxLeng = 29;
            break;
            #endif
            #if (MENU_GENERAL_LANGUAGE_RUSSIAN_EN)
            case LANGUAGE_RUSSIAN:
                //bLine = 2;
                //bLine1MaxLeng = 25;
            break;
            #endif
            }
        break;

        case WMSG_INVALID_OPERATION:
            iStrID = IDS_DS_MSG_INVALID_OPERATION;
        break;

        case WMSG_BATTERY_FULL:
            iStrID = IDS_DS_MSG_BATTERY_FULL;
        break;

        case WMSG_LENS_ERROR:
            iStrID = IDS_DS_MSG_LENS_ERROR;
            bLine = 2;
        break;

        case WMSG_HDMI_TV:
            iStrID = IDS_DS_MSG_HDMI_TV;
        break;

        case WMSG_FHD_VR:
            iStrID = IDS_DS_MSG_FHD_VR;

            switch(MenuSettingConfig()->uiLanguage) {
            #if (MENU_GENERAL_LANGUAGE_ENGLISH_EN)
            case LANGUAGE_ENGLISH:
                bLine = 2;
                bLine1MaxLeng = 37;
            break;
            #endif
            #if (MENU_GENERAL_LANGUAGE_SPANISH_EN)
            case LANGUAGE_SPANISH:
                bLine = 2;
                bLine1MaxLeng = 38;
            break;
            #endif
            #if (MENU_GENERAL_LANGUAGE_PORTUGUESE_EN)
            case LANGUAGE_PORTUGUESE:
                bLine = 2;
                bLine1MaxLeng = 38;
            break;
            #endif
            #if (MENU_GENERAL_LANGUAGE_RUSSIAN_EN)
            case LANGUAGE_RUSSIAN:
                bLine = 2;
                bLine1MaxLeng = 63;
            break;
            #endif
            }
        break;

        case WMSG_FORMAT_SD_PROCESSING:
            iStrID = IDS_DS_MSG_FORMAT_SD_PROCESSING;
        break;

        case WMSG_SEAMLESS_ERROR:
            iStrID = IDS_DS_MSG_CYCLE_RECORD_CLEAN_SPACE;
        break;

        case WMSG_FORMAT_SD_CARD:
            iStrID = IDS_DS_MSG_FORMAT_SD_CARD;
        break;

        case WMSG_LOCK_CURRENT_FILE:
            iStrID = IDS_DS_MSG_LOCK_CURRENT_FILE;
        break;

        case WMSG_UNLOCK_CUR_FILE:
            iStrID = IDS_DS_MSG_UNLOCK_FILE;
        break;

        case WMSG_DELETE_FILE_OK:
            iStrID = IDS_DS_MSG_DELETE_FILE_OK;
        break;

        case WMSG_PROTECT_FILE_OK:
            iStrID = IDS_DS_MSG_PROTECT_FILE_OK;
        break;

        case WMSG_GOTO_POWER_OFF:
            iStrID = IDS_DS_MSG_GOTO_POWER_OFF;
        break;

        case WMSG_NO_FILE_IN_BROWSER:
            iStrID = IDS_DS_MSG_NO_FILE;
        break;

        case WMSG_CAPTURE_CUR_FRAME:
            iStrID = IDS_DS_MSG_CAPTURE_SCREEN;
        break;

        #ifdef WMSG_SHOW_WAIT
        case WMSG_OPENFILE_WAIT:
            iStrID = IDS_DS_MSG_OPENFILE_WAIT;
        break;
        #endif

        case WMSG_SHOW_FW_VERSION:
            iStrID = IDS_DS_FW_VERSION_INFO;
            bLine = 2;
        break;
#else   // CAR_DV
        case WMSG_COME2EMER:
            iStrID = IDS_DS_L2_Move_to_Emergency;
        break;

        case WMSG_START_RECORD:
            iStrID = IDS_DS_Start_Recording;
        break;

        case WMSG_START_NORMAL_RECORD:
            iStrID = IDS_DS_Start_Normal_Recording;
        break;

        case WMSG_ADD_SPEEDCAM:

            iStrID = IDS_DS_Add_SpeedCam;

        break;

        case WMSG_DEL_SPEEDCAM:
            iStrID = IDS_DS_SpeedCam_Remove;
        break;

        case WMSG_SPEEDCAM_ADDED:
            iStrID = IDS_DS_SpeedCam_Added;
        break;

        case WMSG_NO_POI_SPACE:

#ifndef CAR_DV
            iStrID = IDS_DS_No_Space_SafetyCam;
#else
            iStrID = IDS_DS_No_POI_Space;
#endif

        break;

        case WMSG_WAIT_GPS_FIX:
            iStrID = IDS_DS_GPS_Not_Fixed;
        break;

        case WMSG_PARKING_MODE_DISABLE:
            iStrID = IDS_DS_Parking_Mode_Disable;
        break;

        case WMSG_NO_CARD1:
            iStrID = IDS_DS_No_SD1_Warn;
        break;

        case WMSG_NO_CARD2:
            iStrID = IDS_DS_SD2_Ejected_Msg;
        break;

#endif  // CAR_DV

#if (ENABLE_ADAS_LDWS)
        case WMSG_LDWS_RightShift:
            if(m_bDrawLDWS)
            {
                m_WMSGShowTime = WMSGShowTime;
                m_WMSGInfo     = WMSGInfo;
                goto Label_AHC_WMSG_END;
            }
            else
            {
                #ifndef BMICON_LDWS_WARNING_RIGHT
                #define BMICON_LDWS_WARNING_RIGHT       bmIcon_Dummy
                #endif

				UINT16	tempw,temph,OSDColor;
				UINT32	OSDAddr;

                printc("m_WMSG.Display R= %dx%d\r\n", m_WMSG.DisplayWidth, m_WMSG.DisplayHeight);

                Width = ((m_WMSG.DisplayWidth - BMICON_LDWS_WARNING_RIGHT.XSize) + 1) / 2;
                Height = ((m_WMSG.DisplayHeight - BMICON_LDWS_WARNING_RIGHT.YSize) + 1) / 2;

                printc("Pos = %dx%d\r\n", Width, Height);

                AHC_OSDSetActive(m_WMSG.DisplayID, 0);
                OSDDraw_SetAlphaBlending(m_WMSG.DisplayID, 0);

                // Set background color.
                #ifdef ENABLE_GUI_SUPPORT_MULTITASK
                AHC_OSDSetColor(m_WMSG.DisplayID, OSD_COLOR_TRANSPARENT);
                #else
                AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
                #endif
				AHC_OSDGetBufferAttr(m_WMSG.DisplayID, &tempw, &temph, &OSDColor, &OSDAddr);
				#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0)
				AHC_OSDDrawFillRect(m_WMSG.DisplayID, 0, 0, tempw, temph);
				#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
				AHC_OSDDrawFillRect(m_WMSG.DisplayID, 0, 0, temph, tempw);
				#endif

                // Draw icon.
                AHC_OSDDrawBitmap(m_WMSG.DisplayID, &BMICON_LDWS_WARNING_RIGHT, Width, Height);
                AHC_OSDSetActive(m_WMSG.DisplayID, 1);

                m_WMSGShowTime = WMSGShowTime;
                m_WMSGInfo     = WMSGInfo;
                m_bDrawLDWS = AHC_TRUE;

                goto Label_AHC_WMSG_END;
            }
        break;

        case WMSG_LDWS_LeftShift:
            if(m_bDrawLDWS)
            {
                m_WMSGShowTime = WMSGShowTime;
                m_WMSGInfo     = WMSGInfo;
                goto Label_AHC_WMSG_END;
            }
            else
            {
                #ifndef BMICON_LDWS_WARNING_LEFT
                #define BMICON_LDWS_WARNING_LEFT        bmIcon_Dummy
                #endif

				UINT16	tempw,temph,OSDColor;
				UINT32	OSDAddr;

				printc("m_WMSG.Display L= %dx%d\r\n", m_WMSG.DisplayWidth, m_WMSG.DisplayHeight);
                Width = ((m_WMSG.DisplayWidth - BMICON_LDWS_WARNING_LEFT.XSize) + 1) / 2;
                Height = ((m_WMSG.DisplayHeight - BMICON_LDWS_WARNING_LEFT.YSize) + 1) / 2;

                AHC_OSDSetActive(m_WMSG.DisplayID, 0);
                OSDDraw_SetAlphaBlending(m_WMSG.DisplayID, 0);

                // Set background color.
                #ifdef ENABLE_GUI_SUPPORT_MULTITASK
                AHC_OSDSetColor(m_WMSG.DisplayID, OSD_COLOR_TRANSPARENT);
                #else
                AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
                #endif
				AHC_OSDGetBufferAttr(m_WMSG.DisplayID, &tempw, &temph, &OSDColor, &OSDAddr);
				#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0)
				AHC_OSDDrawFillRect(m_WMSG.DisplayID, 0, 0, tempw, temph);
				#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
				AHC_OSDDrawFillRect(m_WMSG.DisplayID, 0, 0, temph, tempw);
				#endif

                // Draw icon.
                AHC_OSDDrawBitmap(m_WMSG.DisplayID, &BMICON_LDWS_WARNING_LEFT, Width, Height);
                AHC_OSDSetActive(m_WMSG.DisplayID, 1);

                m_WMSGShowTime = WMSGShowTime;
                m_WMSGInfo     = WMSGInfo;
                m_bDrawLDWS = AHC_TRUE;

                goto Label_AHC_WMSG_END;
            }
        break;
#endif      // LDWS_EN

#if(ENABLE_ADAS_FCWS)
        case WMSG_FCWS:
            iStrID = IDS_DS_STOP; // Temp String ID
        break;
#endif

        #if (DRIVING_SAFETY_REMIND_HEADLIGHT)
        case WMSG_REMIND_HEADLIGHT:
            if (m_bDrawRemindHeadlight) {
                m_WMSGShowTime  = WMSGShowTime;
                m_WMSGInfo      = WMSGInfo;
                goto Label_AHC_WMSG_END;
            }
            else {
                Width = (m_WMSG.DisplayWidth - BMICON_POPUP_REMINDLIGHT.XSize) / 2;
                Height = (m_WMSG.DisplayWidth - BMICON_POPUP_REMINDLIGHT.YSize) / 2;

                AHC_OSDSetActive(m_WMSG.DisplayID, 0);
                OSDDraw_SetAlphaBlending(m_WMSG.DisplayID, 0);
                AHC_OSDDrawBitmap(m_WMSG.DisplayID, &BMICON_POPUP_REMINDLIGHT, Width, Height);
                AHC_OSDSetActive(m_WMSG.DisplayID, 1);
                m_WMSGShowTime          = WMSGShowTime;
                m_WMSGInfo              = WMSGInfo;
                m_bDrawRemindHeadlight  = AHC_TRUE;

                goto Label_AHC_WMSG_END;
            }
        break;
        #endif

        case WMSG_FATIGUEALERT:
             #ifdef CAR_DV
                iStrID = IDS_DS_NO;
                m_WMSGShowTime = WMSGShowTime;
                m_WMSGInfo     = WMSGInfo;
             #else
                iStrID = IDS_DS_No;
                AHC_OSDSetActive(m_WMSG.DisplayID, 0);
                OSDDraw_SetAlphaBlending(m_WMSG.DisplayID, 0);
                AHC_OSDDrawBitmap(m_WMSG.DisplayID, &bmIcon_Popup_Fatigue, 1, 1);
                AHC_OSDSetActive(m_WMSG.DisplayID, 1);
                m_WMSGShowTime = WMSGShowTime;
                m_WMSGInfo     = WMSGInfo;

             #endif
        break;
        default:
            printc(FG_RED("Check: WMSGInfo = %d\r\n"), WMSGInfo);
            goto Label_AHC_WMSG_END;
        break;
        }

#ifdef FLM_GPIO_NUM
        AHC_OS_SleepMs(100);
#endif

#ifdef CAR_DV
    #ifdef CUS_WSG_DRAW
        CUS_WSG_DRAW
    #else
        #ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetFont(m_WMSG.DisplayID, &GUI_Font24_1);
        #else
        AHC_OSDSetFont(&GUI_Font24_1);
        #endif

    #ifdef AHC_WMSG_WITHOUT_LINE_SETTING
    bLine = 1;
    #endif

        if(bLine == 1) {
            MEMCPY(Temp1Str, (const char*)OSD_GetStringViaID(iStrID), sizeof(Temp1Str));
            if (find_newline(Temp1Str, Temp2Str) == 1)
                OSD_ShowStringPool(m_WMSG.DisplayID, iStrID, StrRECT, AHC_WMSG_FORE_COLOR/*0xAA0000FF*/, m_WMSG.byBkColor, GUI_TA_CENTER|GUI_TA_VCENTER);
            else
                OSD_ShowStringPool2(m_WMSG.DisplayID, Temp1Str, Temp2Str, StrRECT, AHC_WMSG_FORE_COLOR/*0xAA0000FF*/, m_WMSG.byBkColor, GUI_TA_CENTER|GUI_TA_VCENTER);
        }
        else {
            UINT8 bLine2Leng = 0;

            switch(WMSGInfo) {
            case WMSG_CARD_SLOW:
                MEMCPY(Temp1Str, (const char*)OSD_GetStringViaID(IDS_DS_MSG_CARD_SLOW), sizeof(Temp1Str));
                MEMCPY(Temp2Str, (const char*)OSD_GetStringViaID(IDS_DS_MSG_USE_RECOMMENDED), sizeof(Temp2Str));
            break;
            case WMSG_LENS_ERROR:
                MEMCPY(Temp1Str, (const char*)OSD_GetStringViaID(IDS_DS_MSG_LENS_ERROR), sizeof(Temp1Str));
                sprintf(Temp2Str, "%d", m_LensError);
                m_LensError = 0;
            break;
            case WMSG_SHOW_FW_VERSION:
                MEMCPY(Temp1Str, (const char*)OSD_GetStringViaID(IDS_DS_FW_VERSION_INFO), sizeof(Temp1Str));
                sprintf(Temp2Str,"%d",AHC_GetMinorVersion());
            break;
            default:
                MEMCPY(TempStr, (const char*)OSD_GetStringViaID(iStrID), sizeof(TempStr));
                MEMCPY(Temp1Str, (const char*)TempStr, bLine1MaxLeng);
                Temp1Str[sizeof(Temp1Str) - 1] = 0;

                if (strlen(TempStr) > bLine1MaxLeng) {
                    bLine2Leng = find_newline2(TempStr, bLine1MaxLeng);
                    Temp1Str[bLine2Leng] = 0;
                    MEMCPY(Temp2Str, TempStr+bLine2Leng, sizeof(Temp2Str));
                }
                else
                {
                    Temp2Str[0] = 0;
                }
            break;
            }

            #ifdef CFG_CUS_DRAW_MULTI_LINE // TBD
            if (bLine == 2)
            {
                OSD_ShowStringPoolByCase(m_WMSG.DisplayID, Temp1Str, Temp2Str, StrRECT, AHC_WMSG_FORE_COLOR/*0xAA0000FF*/, m_WMSG.byBkColor, GUI_TA_CENTER|GUI_TA_VCENTER, -10);
            }
            else
            #endif
            {
                OSD_ShowStringPool2(m_WMSG.DisplayID, Temp1Str, Temp2Str, StrRECT, AHC_WMSG_FORE_COLOR/*0xAA0000FF*/, m_WMSG.byBkColor, GUI_TA_CENTER|GUI_TA_VCENTER);
            }
        }

        AHC_OSDSetColor(m_WMSG.DisplayID, m_WMSG.byBoundColor);
        AHC_OSDSetPenSize(m_WMSG.DisplayID, 3);
        AHC_OSDDrawLine(m_WMSG.DisplayID, m_WMSG.StartX+1, m_WMSG.StartX+m_WMSG.DisplayWidth-1, m_WMSG.StartY+1, m_WMSG.StartY+1);
        AHC_OSDDrawLine(m_WMSG.DisplayID, m_WMSG.StartX+1, m_WMSG.StartX+m_WMSG.DisplayWidth-1, m_WMSG.StartY+m_WMSG.DisplayHeight-1, m_WMSG.StartY+m_WMSG.DisplayHeight-1);
        AHC_OSDDrawLine(m_WMSG.DisplayID, m_WMSG.StartX+1, m_WMSG.StartX+1, m_WMSG.StartY+1, m_WMSG.StartY+m_WMSG.DisplayHeight-1);
        AHC_OSDDrawLine(m_WMSG.DisplayID, m_WMSG.StartX+m_WMSG.DisplayWidth-1, m_WMSG.StartX+m_WMSG.DisplayWidth-1, m_WMSG.StartY+1, m_WMSG.StartY+m_WMSG.DisplayHeight-1);

        #ifndef AHC_WMSG_WITHOUT_WMSG_Icon
        AHC_OSDDrawBitmap(m_WMSG.DisplayID, &BM_ICON_WMSG, m_WMSG.StartX+5, m_WMSG.StartY+5);
        #endif

        OSDDraw_SetSemiTransparent(m_WMSG.DisplayID, 1, AHC_OSD_SEMITP_AVG, 240);
    #endif
#else   // CAR_DV
        bLine = 1;

        AHC_OSDSetActive(m_WMSG.DisplayID, 0);
        OSDDraw_SetAlphaBlending(m_WMSG.DisplayID, AlphaLevel);

        if(m_bDrawCardError) {
            AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
            AHC_OSDDrawFillRect(m_WMSG.DisplayID, 65, 72, 65+188, 72+95);
            m_bDrawCardError = AHC_FALSE;
        }

        AHC_OSDSetColor(m_WMSG.byBkColor);

        AHC_OSDDrawRoundedRect(m_WMSG.DisplayID, m_WMSG.StartX, m_WMSG.StartY, m_WMSG.StartX + m_WMSG.DisplayWidth, m_WMSG.StartY + m_WMSG.DisplayHeight, 5);

        OSD_ShowStringPool(m_WMSG.DisplayID, iStrID, StrRECT, OSD_COLOR_WHITE, m_WMSG.byBkColor, GUI_TA_CENTER|GUI_TA_VCENTER);

        //OSDDraw_SetSemiTransparent(m_WMSG.DisplayID, 1, AHC_OSD_SEMITP_AVG, 240);
#endif  // CAR_DV

        AHC_OSDSetCurrentDisplay(m_WMSG.DisplayID);

        AHC_OSDSetActive(m_WMSG.DisplayID, 1);

        m_WMSGShowTime = WMSGShowTime;
        m_WMSGInfo     = WMSGInfo;
        #if defined(WIFI_PORT) && (WIFI_PORT == 1)
        set_netapp_WMSGInfo(m_WMSGInfo);
        #endif
    }
    else {
        #ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(m_WMSG.DisplayID, OSD_COLOR_TRANSPARENT);
        #else
        AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
        #endif

        #ifdef CAR_DV
        // NOP
        #else
        if(m_bDrawCardError) {
            AHC_OSDDrawFillRect(m_WMSG.DisplayID, 65, 72, 65+188, 72+95);
            m_bDrawCardError = AHC_FALSE;
        }
        #endif

        /* Disable WMSG window to save hardware badndwidth */
        if (WMSG_LAYER_WINDOW_ID != OSD_LAYER_WINDOW_ID)
            AHC_OSDSetActive(m_WMSG.DisplayID, 0);

        #ifdef FLM_GPIO_NUM
        AHC_OS_SleepMs(100);
        #endif

        #if (ENABLE_ADAS_LDWS)
        if(m_bDrawLDWS)
        {
            m_bDrawLDWS = AHC_FALSE;
            AHC_OSDDrawFillRect(m_WMSG.DisplayID, 0, 0, (UINT16) m_OSD[m_WMSG.DisplayID]->width, (UINT16) m_OSD[m_WMSG.DisplayID]->height);
        }
        else
        #endif
        {
            AHC_OSDDrawFillRect(m_WMSG.DisplayID, m_WMSG.StartX, m_WMSG.StartY-1, m_WMSG.StartX+m_WMSG.DisplayWidth, m_WMSG.StartY+m_WMSG.DisplayHeight+1);
        }

        #if (DRIVING_SAFETY_REMIND_HEADLIGHT)
        if (m_bDrawRemindHeadlight)
        {
            m_bDrawRemindHeadlight = AHC_FALSE;
            AHC_OSDDrawFillRect(m_WMSG.DisplayID, 0, 0, (UINT16)m_OSD[m_WMSG.DisplayID]->width, (UINT16)m_OSD[m_WMSG.DisplayID]->height);
        }
        else
        {
            AHC_OSDDrawFillRect(m_WMSG.DisplayID, m_WMSG.StartX, m_WMSG.StartY-1, m_WMSG.StartX+m_WMSG.DisplayWidth, m_WMSG.StartY+m_WMSG.DisplayHeight+1);
        }
        #endif

        #ifdef CAR_DV
        // NOP
        #else
        AHC_OSDDrawFillRoundedRect(m_WMSG.DisplayID, StartX, StartY-1, StartX+m_WMSG.DisplayWidth, StartY+m_WMSG.DisplayHeight+1, 5);
        #endif

        m_WMSGInfo = WMSG_NONE;
    }

Label_AHC_WMSG_END:

    END_LAYER(m_WMSG.DisplayID);
    AHC_OSDRefresh();

    #if defined(ENABLE_GUI_SUPPORT_MULTITASK)
    // NOP
    #elif defined(SLIDE_STRING)
    //AHC_UnlockGUI(); instead by END_GUI_DRAW()
    #endif

    #ifdef CAR_DV
    // NOP
    #else
    AHC_OSDSetColor(OSD_COLOR_BLACK);
    #endif

    #if (USE_WMSG_SEMI)
    AHC_OS_ReleaseSem(m_WMSGSemID);
    #endif

#endif//NO_PANEL

    return AHC_TRUE;
}

static int find_newline(char *line1, char *line2)
{
    static char *newline = "\\\\";
    char    *p;

    p = strstr(line1, newline);

    if (p == NULL) {
        return 1;
    }
    *p = 0;
    STRCPY(line2, p + 2);
    return 2;
}

static unsigned char find_newline2(char *line1, unsigned char len)
{
    char *p = line1;
    unsigned char pos = 0;

    while (*line1) {
        if ((*line1 > 0xA0) && (*line1 <= 0xAF)) {
            p += 2;

            if (((unsigned int) p - (unsigned int) line1) >= len)
                break;

            pos += 2;
        }
        else {
            // ASCII
            p++;

            if (((unsigned int) p - (unsigned int) line1) >= len)
                break;

            pos += 1;
        }
    }

    return pos;
}

#if (ENABLE_ADAS_LDWS)
void LDWS_DrawLaneLine(UINT16 ubID, AHC_BOOL bClear, ldws_params_t* preLDWS_attribute, ldws_params_t* curLDWS_attribute, GUI_COLOR left_lane_Color, GUI_COLOR right_lane_Color )
{
    #ifdef CUS_DRAW_LDWS_LANE_LINE  // maybe defined in Config_SDK.h

    CUS_DRAW_LDWS_LANE_LINE(bClear, preLDWS_attribute, curLDWS_attribute, left_lane_Color, right_lane_Color);

    #else

    UINT16 usDispalyWidth, usDispalyHeight;
    UINT32 ulPrevieweWidth, ulPrevieweHeight;
    UINT32 uiLeftStartX = 0;
    UINT32 uiLeftStartY = 0;
    UINT32 uiLeftEndX   = 0;
    UINT32 uiLeftEndY   = 0;
    UINT32 uiRightStartX = 0;
    UINT32 uiRightStartY = 0;
    UINT32 uiRightEndX   = 0;
    UINT32 uiRightEndY   = 0;
    UINT32 ulOffsetX, ulOffsetY;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);

    MMPS_3GPRECD_GetPreviewSize(gsAhcCurrentSensor, &ulPrevieweWidth, &ulPrevieweHeight);
    OSDDraw_GetDisplaySize( ubID, &usDispalyWidth, &usDispalyHeight );

    ulOffsetX = abs(usDispalyWidth*OSD_DISPLAY_SCALE_LCD - ulPrevieweWidth) / 2;
    ulOffsetY = abs(usDispalyHeight*OSD_DISPLAY_SCALE_LCD - ulPrevieweHeight) / 2;

    //printc("--%s-- DisplaySize: %d x %d\r\n", __func__, usDispalyWidth, usDispalyHeight);
    //printc("                      PreviewSize: %d x %d\r\n", ulPrevieweWidth, ulPrevieweHeight);

    if ((bClear == AHC_TRUE) && preLDWS_attribute)
    {
        #ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetPenSize(ubID, 3);
        AHC_OSDSetColor(ubID, OSD_COLOR_TRANSPARENT);
        #else
        AHC_OSDSetPenSize(3);
        AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
        #endif

        if ( preLDWS_attribute->left_lane[0].x < gstLdwsResol.width    &&
             preLDWS_attribute->left_lane[0].y < gstLdwsResol.height   &&
             preLDWS_attribute->left_lane[1].x < gstLdwsResol.width    &&
             preLDWS_attribute->left_lane[1].y < gstLdwsResol.height )
        {
            uiLeftStartX = ( preLDWS_attribute->left_lane[0].x * usDispalyWidth) / gstLdwsResol.width + ulOffsetX;
            uiLeftStartY = ( preLDWS_attribute->left_lane[0].y * usDispalyHeight) / gstLdwsResol.height + ulOffsetY;
            uiLeftEndX   = ( preLDWS_attribute->left_lane[1].x * usDispalyWidth) / gstLdwsResol.width + ulOffsetX;
            uiLeftEndY   = ( preLDWS_attribute->left_lane[1].y * usDispalyHeight) / gstLdwsResol.height + ulOffsetY;

            if( uiLeftStartX < usDispalyWidth  ||
                uiLeftEndX   < usDispalyWidth  ||
                uiLeftStartY < usDispalyHeight ||
                uiLeftEndY   < usDispalyHeight )
            {
                AHC_OSDDrawLine( ubID, uiLeftStartX, uiLeftEndX, uiLeftStartY, uiLeftEndY);
            }
        }

        if( preLDWS_attribute->right_lane[0].x < gstLdwsResol.width    &&
            preLDWS_attribute->right_lane[0].y < gstLdwsResol.height   &&
            preLDWS_attribute->right_lane[1].x < gstLdwsResol.width    &&
            preLDWS_attribute->right_lane[1].y < gstLdwsResol.height )
        {
            uiRightStartX = ( preLDWS_attribute->right_lane[0].x * usDispalyWidth  ) / gstLdwsResol.width + ulOffsetX;
            uiRightStartY = ( preLDWS_attribute->right_lane[0].y * usDispalyHeight) / gstLdwsResol.height + ulOffsetY;
            uiRightEndX   = ( preLDWS_attribute->right_lane[1].x * usDispalyWidth  ) / gstLdwsResol.width + ulOffsetX;
            uiRightEndY   = ( preLDWS_attribute->right_lane[1].y * usDispalyHeight) / gstLdwsResol.height + ulOffsetY;

            if( uiRightStartX < usDispalyWidth  ||
                uiRightEndX   < usDispalyWidth  ||
                uiRightStartY < usDispalyHeight ||
                uiRightEndY   < usDispalyHeight )
            {
                AHC_OSDDrawLine( ubID, uiRightStartX, uiRightEndX, uiRightStartY, uiRightEndY);
            }
        }
    }

    if (curLDWS_attribute)
    {
        if ( curLDWS_attribute->left_lane[0].x < gstLdwsResol.width    &&
             curLDWS_attribute->left_lane[0].y < gstLdwsResol.height   &&
             curLDWS_attribute->left_lane[1].x < gstLdwsResol.width    &&
             curLDWS_attribute->left_lane[1].y < gstLdwsResol.height )
        {
            uiLeftStartX = ( curLDWS_attribute->left_lane[0].x * usDispalyWidth) / gstLdwsResol.width + ulOffsetX;
            uiLeftStartY = ( curLDWS_attribute->left_lane[0].y * usDispalyHeight) / gstLdwsResol.height + ulOffsetY;
            uiLeftEndX   = ( curLDWS_attribute->left_lane[1].x * usDispalyWidth) / gstLdwsResol.width + ulOffsetX;
            uiLeftEndY   = ( curLDWS_attribute->left_lane[1].y * usDispalyHeight) / gstLdwsResol.height + ulOffsetY;

            if( uiLeftStartX < usDispalyWidth  &&
                uiLeftStartY < usDispalyHeight &&
                uiLeftEndX   < usDispalyWidth  &&
                uiLeftEndY   < usDispalyHeight )
            {
    			//printc("L:%d,%d - %d,%d\r\n", uiLeftStartX, uiLeftStartY, uiLeftEndX, uiLeftEndY);

                #ifdef ENABLE_GUI_SUPPORT_MULTITASK
                AHC_OSDSetPenSize(ubID, 3);
                AHC_OSDSetColor(ubID, left_lane_Color);
                #else
                AHC_OSDSetPenSize(3);
                AHC_OSDSetColor(left_lane_Color);
                #endif

                AHC_OSDDrawLine( ubID, uiLeftStartX, uiLeftEndX, uiLeftStartY, uiLeftEndY);
            }
        }

        if( curLDWS_attribute->right_lane[0].x < gstLdwsResol.width    &&
            curLDWS_attribute->right_lane[0].y < gstLdwsResol.height   &&
            curLDWS_attribute->right_lane[1].x < gstLdwsResol.width    &&
            curLDWS_attribute->right_lane[1].y < gstLdwsResol.height )
        {
            uiRightStartX = ( curLDWS_attribute->right_lane[0].x * usDispalyWidth) / gstLdwsResol.width + ulOffsetX;
            uiRightStartY = ( curLDWS_attribute->right_lane[0].y * usDispalyHeight) / gstLdwsResol.height + ulOffsetY;
            uiRightEndX   = ( curLDWS_attribute->right_lane[1].x * usDispalyWidth) / gstLdwsResol.width + ulOffsetX;
            uiRightEndY   = ( curLDWS_attribute->right_lane[1].y * usDispalyHeight) / gstLdwsResol.height + ulOffsetY;

            if( uiRightStartX < usDispalyWidth  &&
                uiRightEndX   < usDispalyWidth  &&
                uiRightStartY < usDispalyHeight &&
                uiRightEndY   < usDispalyHeight  )
            {
    			//printc("R:%d,%d - %d,%d\r\n", uiRightStartX, uiRightStartY, uiRightEndX, uiRightEndY);

                #ifdef ENABLE_GUI_SUPPORT_MULTITASK
                AHC_OSDSetPenSize(ubID, 3);
                AHC_OSDSetColor(ubID, right_lane_Color);
                #else
                AHC_OSDSetPenSize(3);
                AHC_OSDSetColor(right_lane_Color);
                #endif

                AHC_OSDDrawLine( ubID, uiRightStartX, uiRightEndX, uiRightStartY, uiRightEndY);
            }
        }
    }

    END_LAYER(ubID);



    #endif
}

void AHC_WMSG_LDWS_Line(UINT16 ubID, AHC_BOOL needClean)
{
#ifdef CUS_WMSG_LDWS_LINE   // maybe defined in Config_SDK.h

#if defined(CCIR656_OUTPUT_ENABLE) && (CCIR656_OUTPUT_ENABLE)
    return;
#endif

    CUS_WMSG_LDWS_LINE(needClean);

#else

    static AHC_BOOL printed_low_speed_msg = AHC_FALSE;
    MMP_LONG  ulAlert;
    ldws_params_t cur_ldws_attribute = {0};

#if defined(CCIR656_OUTPUT_ENABLE) && (CCIR656_OUTPUT_ENABLE)
    return;
#endif

    if (((m_WMSGInfo != WMSG_NONE) && (m_WMSGInfo != WMSG_LDWS_RightShift) && (m_WMSGInfo != WMSG_LDWS_LeftShift)) || IN_MENU()) {
        return;
    }

    // For LDWS
    {
        UINT32 uiAdasFlag = 0;
        AHC_GetParam( PARAM_ID_ADAS, &uiAdasFlag );

        if ((uiAdasFlag & AHC_ADAS_ENABLED_LDWS) == 0) {
            return;
        }
    }

#if (GPS_CONNECT_ENABLE && LDWS_WARN_MIN_SPEED)
#define LDWS_TEST_FORCE_SPEED   (70)
#ifdef LDWS_TEST_FORCE_SPEED
    if(LDWS_TEST_FORCE_SPEED < LDWS_WARN_MIN_SPEED)
#else
    if(GPSCtrl_GetSpeed(0) < LDWS_WARN_MIN_SPEED)
#endif
    {
        if (printed_low_speed_msg == AHC_FALSE) {
            printc("--W-- Speed is small than %dKM/H\r\n", LDWS_WARN_MIN_SPEED);
            printed_low_speed_msg = AHC_TRUE;
        }

        return;
    }
#endif

    ADAS_CTL_GetLDWSAttr(&cur_ldws_attribute, &ulAlert);
    printed_low_speed_msg = AHC_FALSE;

#if defined(WIFI_PORT) && (WIFI_PORT == 1)
    if (AHC_Get_WiFi_Streaming_Status() == AHC_FALSE)
#endif
    {
        #if (OSD_SHOW_LDWS_LANE_LINE)
        static ldws_params_t last_ldws_attribute = {0};

        if (cur_ldws_attribute.state != LDWS_STATE_NODETECT)
        {
            if( last_ldws_attribute.left_lane[0].x != cur_ldws_attribute.left_lane[0].x  ||
                last_ldws_attribute.left_lane[0].y != cur_ldws_attribute.left_lane[0].y  ||
                last_ldws_attribute.left_lane[1].x != cur_ldws_attribute.left_lane[1].x  ||
                last_ldws_attribute.left_lane[1].y != cur_ldws_attribute.left_lane[1].y  ||
                last_ldws_attribute.right_lane[0].x != cur_ldws_attribute.right_lane[0].x  ||
                last_ldws_attribute.right_lane[0].y != cur_ldws_attribute.right_lane[0].y  ||
                last_ldws_attribute.right_lane[1].x != cur_ldws_attribute.right_lane[1].x  ||
                last_ldws_attribute.right_lane[1].y != cur_ldws_attribute.right_lane[1].y  )
            {
                if (ulAlert == LDWS_STATE_DEPARTURE_LEFT)
                {   // Left Shift
                    LDWS_DrawLaneLine( ubID, AHC_TRUE, &last_ldws_attribute, &cur_ldws_attribute, OSD_COLOR_RED, OSD_COLOR_BLUE );
                }
                else if(ulAlert == LDWS_STATE_DEPARTURE_RIGHT)
                {   // Right Shift
                    LDWS_DrawLaneLine( ubID, AHC_TRUE, &last_ldws_attribute, &cur_ldws_attribute, OSD_COLOR_BLUE, OSD_COLOR_RED );
                }
                else
                {
                    LDWS_DrawLaneLine( ubID, AHC_TRUE, &last_ldws_attribute, &cur_ldws_attribute, OSD_COLOR_BLUE, OSD_COLOR_BLUE);
                }

                last_ldws_attribute.left_lane[0].x = cur_ldws_attribute.left_lane[0].x;
                last_ldws_attribute.left_lane[0].y = cur_ldws_attribute.left_lane[0].y;
                last_ldws_attribute.left_lane[1].x = cur_ldws_attribute.left_lane[1].x;
                last_ldws_attribute.left_lane[1].y = cur_ldws_attribute.left_lane[1].y;
                last_ldws_attribute.right_lane[0].x = cur_ldws_attribute.right_lane[0].x;
                last_ldws_attribute.right_lane[0].y = cur_ldws_attribute.right_lane[0].y;
                last_ldws_attribute.right_lane[1].x = cur_ldws_attribute.right_lane[1].x;
                last_ldws_attribute.right_lane[1].y = cur_ldws_attribute.right_lane[1].y;
            }
        }

        // Draw Calibration Line
        #ifdef OSD_SHOW_LDWS_CALI_LANE_LINE
        if (OSD_SHOW_LDWS_CALI_LANE_LINE)
        {
            printc("Draw CALI Lane\r\n");
            cur_ldws_attribute.left_lane[0].x = ldwsTuningParms.laneCalibrationUpPointX;
            cur_ldws_attribute.left_lane[0].y = ldwsTuningParms.laneCalibrationUpPointY;
            cur_ldws_attribute.left_lane[1].x = ldwsTuningParms.laneCalibrationLeftPointX;
            cur_ldws_attribute.left_lane[1].y = ldwsTuningParms.laneCalibrationLeftPointY;
            cur_ldws_attribute.right_lane[0].x = ldwsTuningParms.laneCalibrationUpPointX;
            cur_ldws_attribute.right_lane[0].y = ldwsTuningParms.laneCalibrationUpPointY;
            cur_ldws_attribute.right_lane[1].x = ldwsTuningParms.laneCalibrationRightPointX;
            cur_ldws_attribute.right_lane[1].y = ldwsTuningParms.laneCalibrationRightPointY;

            LDWS_DrawLaneLine( ubID, AHC_FALSE, NULL, &cur_ldws_attribute, OSD_COLOR_WHITE, OSD_COLOR_WHITE);
        }
        #endif

        #endif
    }



#endif  // CUS_WMSG_LDWS_LINE
}
#endif

#if (ENABLE_ADAS_FCWS)
void AHC_WMSG_FCWS_Info(UINT16 ubID, AHC_BOOL needClean)
{
#ifdef CUS_WMSG_FCWS_INFO   // maybe defined in Config_SDK.h
#if defined(CCIR656_OUTPUT_ENABLE) && (CCIR656_OUTPUT_ENABLE)
    return;
#endif

    CUS_WMSG_FCWS_INFO(needClean);

#else

#if defined(CCIR656_OUTPUT_ENABLE) && (CCIR656_OUTPUT_ENABLE)
    return;
#endif

    if (((m_WMSGInfo != WMSG_NONE) && (m_WMSGInfo != WMSG_LDWS_RightShift) && (m_WMSGInfo != WMSG_LDWS_LeftShift)) || IN_MENU()) {
        return;
    }

    {
        UINT32 uiAdasFlag = 0;
        AHC_GetParam( PARAM_ID_ADAS, &uiAdasFlag );

        if ( uiAdasFlag & AHC_ADAS_ENABLED_FCWS ) {
            fcws_info_t cur_fcws_info = {0};

            ADAS_CTL_GetFCWSAttr(&cur_fcws_info);

#if (OSD_SHOW_FCWS_RESULT == 1) // Show FCWS result on display
            #if defined(WIFI_PORT) && (WIFI_PORT == 1)
            if (AHC_Get_WiFi_Streaming_Status() == AHC_FALSE)
            #endif
            {
                static fcws_info_t last_fcws_info = {0};


                BEGIN_LAYER(ubID);

                if (needClean) {
                    if (last_fcws_info.car_x || last_fcws_info.car_y || last_fcws_info.car_width || last_fcws_info.car_height) {
                        RECT     stRect;

                        #ifdef ENABLE_GUI_SUPPORT_MULTITASK
                        AHC_OSDSetColor(ubID, AHC_OSDGetBkColor(ubID));
                        AHC_OSDSetPenSize(ubID, 4);
                        #else
                        AHC_OSDSetColor(AHC_OSDGetBkColor());
                        AHC_OSDSetPenSize(4);
                        #endif

                        stRect.uiLeft = last_fcws_info.car_x;
                        stRect.uiTop = last_fcws_info.car_y;
                        stRect.uiWidth = last_fcws_info.car_width;
                        stRect.uiHeight = last_fcws_info.car_height;
                        MEMSET(&last_fcws_info, 0, sizeof(fcws_info_t));

                        AHC_OSDDrawFillRect2(ubID, &stRect);
                    }
                }

                if( cur_fcws_info.state == FCWS_STATE_FIND )
                {
                    GUI_COLOR displayColor = OSD_COLOR_GREEN;
                    GUI_RECT stGuiRect;
                    UINT8   sz[6] = {0};
                    UINT16 uiDispalyWidth;
                    UINT16 uiDispalyHeight;

                    if( cur_fcws_info.distance < FCWS_WARN_DISTANCE )
                        displayColor = OSD_COLOR_RED;

                    OSDDraw_GetDisplaySize( ubID, &uiDispalyWidth, &uiDispalyHeight );

                    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
                    AHC_OSDSetFont(ubID, &GUI_Font16B_1);
                    AHC_OSDSetColor(ubID, displayColor);
                    AHC_OSDSetPenSize(ubID, 4);
                    #else
                    AHC_OSDSetFont(&GUI_Font16B_1);
                    AHC_OSDSetColor(displayColor);
                    AHC_OSDSetPenSize(4);
                    #endif

                    cur_fcws_info.car_x = ( cur_fcws_info.car_x * uiDispalyWidth  ) / gstLdwsResol.width;
                    cur_fcws_info.car_y = ( cur_fcws_info.car_y * uiDispalyHeight ) / gstLdwsResol.height;
                    cur_fcws_info.car_width =  ( cur_fcws_info.car_width * uiDispalyWidth  ) / gstLdwsResol.width;
                    cur_fcws_info.car_height = ( cur_fcws_info.car_height * uiDispalyHeight ) / gstLdwsResol.height;

                    // Make sure the text window is big enough
                    if( cur_fcws_info.car_width < 30)
                        cur_fcws_info.car_width = 30;

                    if( cur_fcws_info.car_height < 20)
                        cur_fcws_info.car_height = 20;

#ifdef FCWS_INFO_DISPLAY_REGION_LIMIT
                    {
                        UINT16 left_bound = 0, right_bound = 0xFFFF, top_bound = 0, bottom_bound = 0xFFFF;

                        #if (HDMI_ENABLE && defined(FCWS_INFO_DISPLAY_REGION_LIMIT_HDMI))
                        if (HDMIFunc_IsInHdmiMode()) {
                            left_bound = FCWS_INFO_DISPLAY_REGION_LIMIT_HDMI_LB;
                            right_bound = FCWS_INFO_DISPLAY_REGION_LIMIT_HDMI_RB;
                            top_bound = FCWS_INFO_DISPLAY_REGION_LIMIT_HDMI_TB;
                            bottom_bound = FCWS_INFO_DISPLAY_REGION_LIMIT_HDMI_BB;
                        }
                        else
                        #endif
                        #if (TVOUT_ENABLE && defined(FCWS_INFO_DISPLAY_REGION_LIMIT_TV))
                        if(TVFunc_IsInTVMode())
                        {
                            left_bound = FCWS_INFO_DISPLAY_REGION_LIMIT_TV_LB;
                            right_bound = FCWS_INFO_DISPLAY_REGION_LIMIT_TV_RB;
                            top_bound = FCWS_INFO_DISPLAY_REGION_LIMIT_TV_TB;
                            bottom_bound = FCWS_INFO_DISPLAY_REGION_LIMIT_TV_BB;
                        }
                        else
                        #endif
                        #if defined(FCWS_INFO_DISPLAY_REGION_LIMIT_LCD)
                        {
                            left_bound = FCWS_INFO_DISPLAY_REGION_LIMIT_LCD_LB;
                            right_bound = FCWS_INFO_DISPLAY_REGION_LIMIT_LCD_RB;
                            top_bound = FCWS_INFO_DISPLAY_REGION_LIMIT_LCD_TB;
                            bottom_bound = FCWS_INFO_DISPLAY_REGION_LIMIT_LCD_BB;
                        }
                        #endif

                        if (cur_fcws_info.car_x < left_bound)
                            cur_fcws_info.car_x = left_bound;

                        if ((cur_fcws_info.car_x + cur_fcws_info.car_width) > right_bound) {
                            if (cur_fcws_info.car_width > (right_bound - left_bound)) {
                                cur_fcws_info.car_width = right_bound - left_bound;
                                cur_fcws_info.car_x = left_bound;
                            }
                            else {
                                cur_fcws_info.car_x = right_bound - cur_fcws_info.car_width;
                            }
                        }

                        if (cur_fcws_info.car_y < top_bound)
                            cur_fcws_info.car_y = top_bound;

                        if ((cur_fcws_info.car_y + cur_fcws_info.car_height) > bottom_bound) {
                            if (cur_fcws_info.car_height > (bottom_bound - top_bound)) {
                                cur_fcws_info.car_height = bottom_bound - top_bound;
                                cur_fcws_info.car_y = top_bound;
                            }
                            else {
                                cur_fcws_info.car_y = bottom_bound - cur_fcws_info.car_height;
                            }
                        }
                    }
#endif

                    cur_fcws_info.car_x &= 0xFFFFFFFE;
                    cur_fcws_info.car_y &= 0xFFFFFFFE;
                    cur_fcws_info.car_width &= 0xFFFFFFFE;
                    cur_fcws_info.car_height &= 0xFFFFFFFE;
                    MEMCPY(&last_fcws_info, &cur_fcws_info, sizeof(fcws_info_t));

                    stGuiRect.x0 = cur_fcws_info.car_x;
                    stGuiRect.y0 = cur_fcws_info.car_y;
                    stGuiRect.x1 = cur_fcws_info.car_x + cur_fcws_info.car_width;
                    stGuiRect.y1 = cur_fcws_info.car_y + cur_fcws_info.car_height;

                    AHC_OSDDrawRect(ubID, stGuiRect.x0, stGuiRect.y0, stGuiRect.x1, stGuiRect.y1);

                    if (cur_fcws_info.distance > 100)
                        cur_fcws_info.distance = 99;

                    sprintf((char *) &sz, "%02dm", cur_fcws_info.distance);
                    AHC_OSDDrawTextInRect(ubID, (UINT8 *) &sz, &stGuiRect, GUI_TA_HCENTER | GUI_TA_VCENTER);
                }
                else if (!needClean && (last_fcws_info.car_x || last_fcws_info.car_y || last_fcws_info.car_width || last_fcws_info.car_height)) {
                    RECT     stRect;

                    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
                    AHC_OSDSetColor(ubID, AHC_OSDGetBkColor(ubID));
                    AHC_OSDSetPenSize(ubID, 4);
                    #else
                    AHC_OSDSetColor(AHC_OSDGetBkColor());
                    AHC_OSDSetPenSize(4);
                    #endif

                    stRect.uiLeft = last_fcws_info.car_x;
                    stRect.uiTop = last_fcws_info.car_y;
                    stRect.uiWidth = last_fcws_info.car_width;
                    stRect.uiHeight = last_fcws_info.car_height;
                    MEMSET(&last_fcws_info, 0, sizeof(fcws_info_t));

                    AHC_OSDDrawFillRect2(ubID, &stRect);
                }


                END_LAYER(ubID);
            }
#endif

            if ( cur_fcws_info.state == FCWS_STATE_FIND )
            {
                if ( cur_fcws_info.distance < FCWS_WARN_DISTANCE )
                {
                   // AHC_WMSG_Draw(AHC_TRUE, WMSG_FCWS, 1);
                   //AHC_PlaySoundEffect(AHC_SOUNDEFFECT_FCWS_WARNING); //cancel playsound by polling
                }
            }
        }
    }
#endif  // CUS_WMSG_FCWS_INFO
}
#endif

void AHC_WMSG_SetWMSG(AHC_WMSG_CFG* pSrcWMSG)
{
#if defined(CCIR656_OUTPUT_ENABLE) && (CCIR656_OUTPUT_ENABLE)
    return;
#endif

    MEMCPY(&m_WMSG, pSrcWMSG, sizeof(AHC_WMSG_CFG));
}

void AHC_WMSG_GetWMSG(AHC_WMSG_CFG* pDestWMSG)
{
#if defined(CCIR656_OUTPUT_ENABLE) && (CCIR656_OUTPUT_ENABLE)
    return;
#endif

    MEMCPY(pDestWMSG, &m_WMSG, sizeof(AHC_WMSG_CFG));
}


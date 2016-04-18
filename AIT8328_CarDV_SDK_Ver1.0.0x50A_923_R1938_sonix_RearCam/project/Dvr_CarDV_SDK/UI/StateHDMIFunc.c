/*===========================================================================
 * Include file
 *===========================================================================*/

#include "customer_config.h"
#include "AHC_Common.h"
#include "AHC_General.h"
#include "AHC_Display.h"
#include "AHC_Menu.h"
#include "AHC_Dcf.h"
#include "AHC_UF.h"
#include "AHC_Browser.h"
#include "AHC_USB.h"
#include "AHC_Parameter.h"
#include "AHC_Warningmsg.h"
#include "AHC_Media.h"
#include "AHC_OS.h"
#include "AHC_ISP.h"
#include "AHC_General_CarDV.h"
#include "Menusetting.h"
#include "MenuDrawCommon.h"
#include "MediaPlaybackCtrl.h"
#include "DrawStateHDMIFunc.h"
#include "DrawStateTVFunc.h"
#include "DrawStateVideoFunc.h"
#include "DrawStateCameraFunc.h"
#include "DrawStateSlideShowFunc.h"
#include "DrawStateMoviePBFunc.h"
#include "StateHDMIFunc.h"
#include "StateTVFunc.h"
#include "StateVideoFunc.h"
#include "StateCameraFunc.h"
#include "StateBrowserFunc.h"
#include "StatePhotoPBFunc.h"
#include "StateSlideShowFunc.h"
#include "StateMoviePBFunc.h"
#include "KeyParser.h"
#include "LedControl.h"
#include "dsc_charger.h"
#include "mmps_display.h"
#include "ColorDefine.h"
#include "IconPosition.h"
#include "IconDefine.h"
#include "ZoomControl.h"
#include "UI_DrawGeneral.h"
#include "MenuStateMenu.h"
/*===========================================================================
 * Macro define
 *===========================================================================*/
#define VIDEO_TIMER_UNIT			(100)

#if (HDMI_PREVIEW_EN) && (HDMI_MENU_EN)
#define USE_NEW_HDMI_EVENT_HANDLER	(1)
#else
#define USE_NEW_HDMI_EVENT_HANDLER	(0)
#endif

/*===========================================================================
 * Structure define
 *===========================================================================*/

typedef struct _AHC_HDMI_SMALL_OSD{
	UINT16 uwDisplayID;
	UINT16 uwWidth;
	UINT16 uwHeight;
	UINT16 uwDisplayOffsetX;
	UINT16 uwDisplayOffsetY;
} AHC_HDMI_SMALL_OSD;

/*===========================================================================
 * Global varible
 *===========================================================================*/

#if (HDMI_ENABLE)
static AHC_THUMB_CFG   		m_ThumbConfig;
static AHC_HDMI_SMALL_OSD 	HDMISmallOSD[2];
#endif
static AHC_HDMI_STATUS 		m_HdmiStatus 	 = AHC_HDMI_NONE_STATUS;

UINT16						m_uwHdmiPrevOsdId;
AHC_BOOL 					bPreviewModeHDMI = AHC_FALSE;
AHC_BOOL					bHdmiMenuMode	 = AHC_FALSE;
UINT8 						bHdmiMovieState  = MOVIE_STATE_STOP;
UINT8 						m_DelFile 		 = DEL_FILE_NONE;

/*===========================================================================
 * Extern varible
 *===========================================================================*/

extern UINT32   	ObjSelect;
extern AHC_BOOL 	m_bMovieOSDForceShowUp;
extern UINT32 		m_VideoCurPlayTime;
extern AHC_BOOL 	bEnterMenuPause;
extern UINT32		m_MovPlayFFSpeed;
extern UINT32		m_MovPlayREWSpeed;
extern AHC_BOOL		bMovPbExitFromHdmi;
extern AHC_BOOL		bMovPbEnterToHdmi;
extern UINT8 		bMovieState;

extern UINT32   	SlideShowCounter;
extern AHC_BOOL 	bSlideShow;
extern AHC_BOOL 	bSlideShowNext;
extern AHC_BOOL 	bSlideShowFirstFile;
extern AHC_BOOL 	bFileFail;

extern AHC_BOOL		bForce_PowerOff;
extern AHC_BOOL 	PowerOff_InProcess;
extern AHC_BOOL 	Protecting_InBrowser;
extern AHC_BOOL 	Deleting_InBrowser;

extern UINT32 		RecordTimeOffset;
extern UINT32 		RemainCaptureNum;
extern MMP_USHORT	gsStillZoomIndex;
extern UINT8 		m_ubDSCMode;
extern AHC_BOOL		m_ubTimeLapseStart;

#if (VR_PREENCODE_EN)
extern AHC_BOOL		bDisableVideoPreRecord;
#endif

extern MOVIESIZE_SETTING 	VideoRecSize;
extern IMAGESIZE_SETTING	CaptureResolution;
extern SMENUSTRUCT 			sSubDelete;
extern AHC_BOOL m_ubPlaybackRearCam;
extern AHC_BOOL		bForceSwitchBrowser;

/*===========================================================================
 * Extern function
 *===========================================================================*/

extern void 	InitOSD(void);
extern void 	RTNA_LCD_Backlight(MMP_BOOL bEnable);
extern AHC_BOOL AIHC_DrawReservedOSD(AHC_BOOL bDrawReserved);
extern void 	MenuDrawSubPage_PowerOff(void);
extern int 	 	VideoFunc_Record(void);

extern UINT32 MainMenuEventHandler(PSMENUSTRUCT pMenu, UINT32 ulEvent, UINT32 ulParam);
extern UINT32 SubMenuEventHandler(PSMENUSTRUCT pMenu, UINT32 ulEvent, UINT32 ulParam);

/*===========================================================================
 * Local function
 *===========================================================================*/

AHC_BOOL	HDMIFunc_ThumbnailPlayback(void);
AHC_BOOL	HDMIFunc_ThumbnailOption(UINT8 ubOp);
AHC_BOOL	HDMIFunc_SlideShow(void);
void		StateHDMI_BrowserMode_Handler(UINT32 ulJobEvent);
void		StateHDMI_EventMenu_Handler(UINT32 ulJobEvent);

/*===========================================================================
 * Main body
 *===========================================================================*/

AHC_HDMI_STATUS HDMIFunc_Status(void)
{
    return m_HdmiStatus;
}

void HDMIFunc_SetStatus(AHC_HDMI_STATUS status)
{
    m_HdmiStatus = status;
}

AHC_BOOL HDMIFunc_IsInMenu(void)
{
    return bHdmiMenuMode;
}

AHC_BOOL HDMIFunc_IsPreview(void)
{
	if(!AHC_IsHdmiConnect())
		return AHC_FALSE;

	if(m_HdmiStatus == AHC_HDMI_VIDEO_PREVIEW_STATUS ||
	   m_HdmiStatus == AHC_HDMI_DSC_PREVIEW_STATUS )
    	return AHC_TRUE;
    else
    	return AHC_FALSE;
}

AHC_BOOL HDMIFunc_IsPlayback(void)
{
	if(!AHC_IsHdmiConnect())
		return AHC_FALSE;

	if(m_HdmiStatus == AHC_HDMI_MOVIE_PB_STATUS 	||
	   m_HdmiStatus == AHC_HDMI_PHOTO_PB_STATUS ||
	   m_HdmiStatus == AHC_HDMI_SLIDE_SHOW_STATUS)
    	return AHC_TRUE;
    else
    	return AHC_FALSE;
}

AHC_BOOL HDMIFunc_IsInHdmiMode(void)
{
	if(AHC_IsHdmiConnect() && (m_HdmiStatus!=AHC_HDMI_NONE_STATUS))
		return AHC_TRUE;
	else
		return AHC_FALSE;
}

AHC_BOOL HDMIFunc_CheckHdmiStatus(UINT8 ubCheckStatus)
{
	if(AHC_IsHdmiConnect() && (m_HdmiStatus==ubCheckStatus))
		return AHC_TRUE;
	else
		return AHC_FALSE;
}
AHC_BOOL HDMIFunc_CheckHDMISystemChanged(void)
{
    AHC_DISPLAY_HDMIOUTPUTMODE 	HdmiMode;
	
    AHC_GetHDMIOutputMode(&HdmiMode);
    if((MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_1080P) && (HdmiMode != AHC_DISPLAY_HDMIOUTPUT_1920X1080P))
        return AHC_TRUE;
    else if((MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_720P) && (HdmiMode != AHC_DISPLAY_HDMIOUTPUT_1280X720P))
        return AHC_TRUE;
        
    return AHC_FALSE;    
}

void HDMIFunc_SetHDMISystem(void)
{
        switch(MenuSettingConfig()->uiHDMIOutput) 
        {
        case HDMI_OUTPUT_1080P:
            AHC_SetHDMIOutputMode(AHC_DISPLAY_HDMIOUTPUT_1920X1080P);
            break;
        case HDMI_OUTPUT_720P:
            AHC_SetHDMIOutputMode(AHC_DISPLAY_HDMIOUTPUT_1280X720P);
            break;
        }
        AHC_SetDisplayOutputDev(DISP_OUT_HDMI, AHC_DISPLAY_DUPLICATE_1X);
        AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_HDMI);
}

#if 0
void ____HDMI_OSD_Init_Function_____(){ruturn;} //dummy
#endif

UINT16 HDMIFunc_GetUImodeOsdID(void)
{
	return HDMI_UI_OSD_ID;
}

UINT16 HDMIFunc_GetMenuOsdID(void)
{
	return HDMI_MENU_OSD_ID;
}

#if (HDMI_ENABLE)
#if (HDMI_MENU_EN) && (HDMI_PREVIEW_EN)
static void Scale_HDMIMenuOSD_Window(int xc)
{
	UINT32	pos[2];

	pos[0] = xc;
	pos[1] = xc;
	AHC_OSDSetDisplayAttr(HDMI_MENU_OSD_ID, AHC_OSD_ATTR_DISPLAY_SCALE, pos);
	AHC_OSDSetDisplayAttr(HDMI_MENU_OSD2_ID, AHC_OSD_ATTR_DISPLAY_SCALE, pos);
}
#endif

UINT16 HDMIFunc_InitPreviewOSD(void)
{
    UINT16		uwHDMIWidth, uwHDMIHeight;
    UINT16		uwHDMIOffX, uwHDMIOffY;
	UINT32		pos[2];
    AHC_DISPLAY_OUTPUTPANEL 	device;

    AHC_OSDUninit();
    
	AHC_SetMode(AHC_MODE_IDLE);
    if(HDMIFunc_CheckHDMISystemChanged())                                              
        HDMIFunc_SetHDMISystem();    
          
	AHC_GetDisplayOutputDev(&device);

    if(device != AHC_DISPLAY_HDMI)
    	AHC_SetDisplayOutputDev(DISP_OUT_HDMI, AHC_DISPLAY_DUPLICATE_1X);
	
	AHC_GetHdmiDisplayWidthHeight(&uwHDMIWidth, &uwHDMIHeight);

    AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_HDMI);

    AIHC_SetGUIMemStartAddr(AHC_GUI_TEMP_BASE_ADDR_HDMI);

    switch(MenuSettingConfig()->uiHDMIOutput)
    {
    case HDMI_OUTPUT_1080I:
        uwHDMIOffX = 0;
        uwHDMIOffY = 0;
        break;
    case HDMI_OUTPUT_720P:
        uwHDMIOffX = 0;
        uwHDMIOffY = 0;
        break;
    case HDMI_OUTPUT_480P:
        uwHDMIOffX = 0;
        uwHDMIOffY = 0;
        break;
    default:
    case HDMI_OUTPUT_1080P:
        uwHDMIOffX = 0;
        uwHDMIOffY = 0;
        break;
    }

	pos[0] = uwHDMIOffX;
	pos[1] = uwHDMIOffY;

    AHC_OSDInit(    0,	0,
    				0,	0,
    				PRIMARY_DATESTAMP_WIDTH,    PRIMARY_DATESTAMP_HEIGHT,
    				THUMB_DATESTAMP_WIDTH,      THUMB_DATESTAMP_HEIGHT,
    				WMSG_LAYER_WIN_COLOR_FMT,   AHC_FALSE);

	#if (HDMI_MENU_EN)
{
	UINT32		uwDispScale = 0;
	UINT16		uwHDMITempWidth, uwHDMITempHeight;	

	if((MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_1080I)||(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_1080P))
	{
		uwDispScale = OSD_DISPLAY_SCALE_HDMI_FHD;
	}
	else if(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_720P)
	{
		uwDispScale = OSD_DISPLAY_SCALE_HDMI_HD;
	}
	else if(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_480P)
	{
		uwDispScale = OSD_DISPLAY_SCALE_TV;
	}

	uwHDMITempWidth = uwHDMIWidth / uwDispScale;
	uwHDMITempHeight = uwHDMIHeight / uwDispScale;

	//** Set HDMI OSD#1 by uwDisplayID = 18,19
	AHC_OSDCreateBuffer(HDMI_MENU_OSD_ID, uwHDMITempWidth, uwHDMITempHeight, OSD_COLOR_RGB565);
	pos[0] = uwHDMIOffX;
	pos[1] = uwHDMIOffY;
	AHC_OSDSetDisplayAttr(HDMI_MENU_OSD_ID, AHC_OSD_ATTR_DISPLAY_OFFSET, pos);
	pos[0] = uwDispScale - 1;
	pos[1] = uwDispScale - 1;
	AHC_OSDSetDisplayAttr(HDMI_MENU_OSD_ID, AHC_OSD_ATTR_DISPLAY_SCALE, pos);
    pos[0] = OSD_ROTATE_DRAW_NONE;
   	AHC_OSDSetDisplayAttr(HDMI_MENU_OSD_ID,  AHC_OSD_ATTR_ROTATE_BY_GUI, pos);
	AHC_OSDClearBuffer(HDMI_MENU_OSD_ID);

	AHC_OSDCreateBuffer(HDMI_MENU_OSD2_ID, uwHDMITempWidth, uwHDMITempHeight, OSD_COLOR_RGB565);
	pos[0] = uwHDMIOffX;
	pos[1] = uwHDMIOffY;
	AHC_OSDSetDisplayAttr(HDMI_MENU_OSD2_ID, AHC_OSD_ATTR_DISPLAY_OFFSET, pos);
	pos[0] = uwDispScale - 1;
	pos[1] = uwDispScale - 1;
	AHC_OSDSetDisplayAttr(HDMI_MENU_OSD2_ID, AHC_OSD_ATTR_DISPLAY_SCALE, pos);
    pos[0] = OSD_ROTATE_DRAW_NONE;
   	AHC_OSDSetDisplayAttr(HDMI_MENU_OSD2_ID,  AHC_OSD_ATTR_ROTATE_BY_GUI, pos);
	AHC_OSDClearBuffer(HDMI_MENU_OSD2_ID);
}
   	#endif
	
#if OSD_PREVIEW_SCALE_HDMI
{
	UINT32		uwDispScale = 0;
	UINT16		uwHDMITempWidth, uwHDMITempHeight;	

	if((MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_1080I)||(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_1080P))
	{
		uwDispScale = OSD_DISPLAY_SCALE_HDMI_FHD;
	}
	else if(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_720P)
	{
		uwDispScale = OSD_DISPLAY_SCALE_HDMI_HD;
	}
	else if(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_480P)
	{
		uwDispScale = OSD_DISPLAY_SCALE_TV;
	}

	uwHDMITempWidth = uwHDMIWidth / uwDispScale;
	uwHDMITempHeight = uwHDMIHeight / uwDispScale;

	//** Set HDMI OSD#2 by uwDisplayID = 17
	AHC_OSDCreateBuffer(HDMI_UI_OSD_ID ,uwHDMITempWidth, uwHDMITempHeight, OSD_COLOR_RGB565);
	pos[0] = uwHDMIOffX;
	pos[1] = uwHDMIOffY;
	AHC_OSDSetDisplayAttr(HDMI_UI_OSD_ID, AHC_OSD_ATTR_DISPLAY_OFFSET, pos);
	pos[0] = uwDispScale - 1;
	pos[1] = uwDispScale - 1;
	AHC_OSDSetDisplayAttr(HDMI_UI_OSD_ID, AHC_OSD_ATTR_DISPLAY_SCALE, pos);	
	pos[0] = 1;
	pos[1] = OSD_COLOR_TRANSPARENT;
	AHC_OSDSetDisplayAttr(HDMI_UI_OSD_ID, AHC_OSD_ATTR_TRANSPARENT_ENABLE, pos);

	pos[0] = OSD_ROTATE_DRAW_NONE;
	AHC_OSDSetDisplayAttr(HDMI_UI_OSD_ID, AHC_OSD_ATTR_ROTATE_BY_GUI, pos);

	pos[0] = OSD_FLIP_DRAW_NONE_ENABLE;
    AHC_OSDSetDisplayAttr(HDMI_UI_OSD_ID, AHC_OSD_ATTR_FLIP_BY_GUI, pos);

	//** Set HDMI OSD by uwDisplayID = 17
	AHC_OSDCreateBuffer(HDMI_UI_OSD2_ID ,uwHDMITempWidth, uwHDMITempHeight, OSD_COLOR_RGB565);
	pos[0] = uwHDMIOffX;
	pos[1] = uwHDMIOffY;
	AHC_OSDSetDisplayAttr(HDMI_UI_OSD2_ID, AHC_OSD_ATTR_DISPLAY_OFFSET, pos);
	pos[0] = uwDispScale - 1;
	pos[1] = uwDispScale - 1;
	AHC_OSDSetDisplayAttr(HDMI_UI_OSD2_ID, AHC_OSD_ATTR_DISPLAY_SCALE, pos);	
	pos[0] = 1;
	pos[1] = OSD_COLOR_TRANSPARENT;
	AHC_OSDSetDisplayAttr(HDMI_UI_OSD2_ID, AHC_OSD_ATTR_TRANSPARENT_ENABLE, pos);

	pos[0] = OSD_ROTATE_DRAW_NONE;
	AHC_OSDSetDisplayAttr(HDMI_UI_OSD2_ID, AHC_OSD_ATTR_ROTATE_BY_GUI, pos);

	pos[0] = OSD_FLIP_DRAW_NONE_ENABLE;
    AHC_OSDSetDisplayAttr(HDMI_UI_OSD2_ID, AHC_OSD_ATTR_FLIP_BY_GUI, pos);
	//--------------------------------------------------------
}
#else
    //** Set HDMI OSD by uwDisplayID = 16
    AHC_OSDCreateBuffer(HDMI_UI_OSD_ID ,uwHDMIWidth, uwHDMIHeight, OSD_COLOR_RGB565);
   	pos[0] = uwHDMIOffX;
	pos[1] = uwHDMIOffY;
	AHC_OSDSetDisplayAttr(HDMI_UI_OSD_ID, AHC_OSD_ATTR_DISPLAY_OFFSET, pos);
    pos[0] = 1;
    pos[1] = OSD_COLOR_TRANSPARENT;
    AHC_OSDSetDisplayAttr(HDMI_UI_OSD_ID, AHC_OSD_ATTR_TRANSPARENT_ENABLE, pos);

	//** Set HDMI OSD by uwDisplayID = 17
	AHC_OSDCreateBuffer(HDMI_UI_OSD2_ID ,uwHDMIWidth, uwHDMIHeight, OSD_COLOR_RGB565);
   	pos[0] = uwHDMIOffX;
	pos[1] = uwHDMIOffY;
	AHC_OSDSetDisplayAttr(HDMI_UI_OSD2_ID, AHC_OSD_ATTR_DISPLAY_OFFSET, pos);
    pos[0] = 1;
    pos[1] = OSD_COLOR_TRANSPARENT;
    AHC_OSDSetDisplayAttr(HDMI_UI_OSD2_ID, AHC_OSD_ATTR_TRANSPARENT_ENABLE, pos);
#endif

	BEGIN_LAYER(HDMI_UI_OSD_ID);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	AHC_OSDSetColor(HDMI_UI_OSD_ID, OSD_COLOR_TRANSPARENT);
	#else
	AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
	#endif

	AHC_OSDDrawFillRect(HDMI_UI_OSD_ID, 0, 0, uwHDMIWidth, uwHDMIHeight);

	AHC_OSDSetActive(HDMI_UI_OSD_ID, 1);
	AHC_OSDSetActive(HDMI_UI_OSD2_ID, 1);

    AHC_OSDSetCurrentDisplay(HDMI_UI_OSD_ID);

	END_LAYER(HDMI_UI_OSD_ID);

	AHC_PreviewOnDisplayEx(AHC_TRUE);

    return HDMI_UI_OSD_ID;
}

void HDMIFunc_InitBrowserSmallOSD(void)
{
	RECT  OsdRect;
	UINT8 i;

    switch(MenuSettingConfig()->uiHDMIOutput)
    {
    case HDMI_OUTPUT_1080I:
        OsdRect.uiLeft 	= POS_HDMI_1080I_BROWSER_OSD_X0;
        OsdRect.uiTop 	= POS_HDMI_1080I_BROWSER_OSD_Y0;
        OsdRect.uiWidth = POS_HDMI_1080I_BROWSER_OSD_W;
        OsdRect.uiHeight= POS_HDMI_1080I_BROWSER_OSD_H;
        break;
    case HDMI_OUTPUT_720P:
        OsdRect.uiLeft 	= POS_HDMI_720P_BROWSER_OSD_X0;
        OsdRect.uiTop 	= POS_HDMI_720P_BROWSER_OSD_Y0;
        OsdRect.uiWidth = POS_HDMI_720P_BROWSER_OSD_W;
        OsdRect.uiHeight= POS_HDMI_720P_BROWSER_OSD_H;
        break;
    case HDMI_OUTPUT_480P:  // TBD
        OsdRect.uiLeft 	= POS_HDMI_480P_BROWSER_OSD_X0;
        OsdRect.uiTop 	= POS_HDMI_480P_BROWSER_OSD_Y0;
        OsdRect.uiWidth = POS_HDMI_480P_BROWSER_OSD_W;
        OsdRect.uiHeight= POS_HDMI_480P_BROWSER_OSD_H;
        break;
    default:
    case HDMI_OUTPUT_1080P:
        OsdRect.uiLeft 	= POS_HDMI_1080P_BROWSER_OSD_X0;
        OsdRect.uiTop 	= POS_HDMI_1080P_BROWSER_OSD_Y0;
        OsdRect.uiWidth = POS_HDMI_1080P_BROWSER_OSD_W;
        OsdRect.uiHeight= POS_HDMI_1080P_BROWSER_OSD_H;
        break;
    }

    HDMISmallOSD[0].uwDisplayID = 16;
    HDMISmallOSD[1].uwDisplayID	= 17;

    for (i=0; i<2; i++)
    {
	    HDMISmallOSD[i].uwWidth  			= OsdRect.uiWidth;
	    HDMISmallOSD[i].uwHeight 			= OsdRect.uiHeight;
	    HDMISmallOSD[i].uwDisplayOffsetX 	= 0;

        switch(MenuSettingConfig()->uiHDMIOutput)
        {
        case HDMI_OUTPUT_1080I:
            HDMISmallOSD[i].uwDisplayOffsetY = 15;
            break;
        case HDMI_OUTPUT_720P:
            HDMISmallOSD[i].uwDisplayOffsetY = 20;
            break;
        case HDMI_OUTPUT_480P:      // TBD
            HDMISmallOSD[i].uwDisplayOffsetY = 20;
            break;
        default:
        case HDMI_OUTPUT_1080P:
            HDMISmallOSD[i].uwDisplayOffsetY = 15;
            break;
        }
    }
}

void HDMIFunc_InitPlaySmallOSD(void)
{
    UINT16 	uwWidth, uwHeight;
	UINT8 	i;
    RECT  	OsdRect  = POS_HDMI_PLAY_OSD;

    AHC_GetHdmiDisplayWidthHeight(&uwWidth, &uwHeight);

    HDMISmallOSD[0].uwDisplayID 	= 16;
    HDMISmallOSD[1].uwDisplayID 	= 17;

	for (i=0; i<2; i++)
	{
    	HDMISmallOSD[i].uwWidth 	= OsdRect.uiWidth;
    	HDMISmallOSD[i].uwHeight 	= OsdRect.uiHeight;

        switch(MenuSettingConfig()->uiHDMIOutput)
        {
        case HDMI_OUTPUT_1080I:
            HDMISmallOSD[i].uwDisplayOffsetX = uwWidth- HDMISmallOSD[i].uwWidth - 50;
            HDMISmallOSD[i].uwDisplayOffsetY = 25;
            break;
        case HDMI_OUTPUT_720P:
            HDMISmallOSD[i].uwDisplayOffsetX = uwWidth- HDMISmallOSD[i].uwWidth - 40;
            HDMISmallOSD[i].uwDisplayOffsetY = 25;
            break;
        case HDMI_OUTPUT_480P:  // TBD
            HDMISmallOSD[i].uwDisplayOffsetX = uwWidth- HDMISmallOSD[i].uwWidth - 40;
            HDMISmallOSD[i].uwDisplayOffsetY = 25;
            break;
        default:
        case HDMI_OUTPUT_1080P:
            HDMISmallOSD[i].uwDisplayOffsetX = uwWidth- HDMISmallOSD[i].uwWidth - 50;
            HDMISmallOSD[i].uwDisplayOffsetY = 25;
            break;
        }
    }
}

AHC_BOOL HDMIFunc_SetSmallOSDDisplayOffset(UINT8 ubSmaillOSDID)
{
    UINT32 iVal[2];

    iVal[0] = HDMISmallOSD[ubSmaillOSDID].uwDisplayOffsetX;
    iVal[1] = HDMISmallOSD[ubSmaillOSDID].uwDisplayOffsetY;
    return AHC_OSDSetDisplayAttr(HDMISmallOSD[ubSmaillOSDID].uwDisplayID, AHC_OSD_ATTR_DISPLAY_OFFSET, iVal);
}

void HDMIFunc_ChangeOSDStatus(AHC_HDMI_STATUS HdmiStatus)
{
    U32 		ulWidth, 	 ulHeight;
    UINT16 		uwHDMIWidth, uwHDMIHeight;
    UINT16		uwHDMIOffX,  uwHDMIOffY;
    AHC_DISPLAY_HDMIOUTPUTMODE 	HdmiMode;
    AHC_DISPLAY_OUTPUTPANEL 	device;

    AHC_OSDUninit();
     
    AHC_SetMode(AHC_MODE_IDLE);
    if(HDMIFunc_CheckHDMISystemChanged())                                              
       HDMIFunc_SetHDMISystem();    
	AHC_GetDisplayOutputDev(&device);

    if(device != AHC_DISPLAY_HDMI)
    	AHC_SetDisplayOutputDev(DISP_OUT_HDMI, AHC_DISPLAY_DUPLICATE_1X);

    AHC_GetHdmiDisplayWidthHeight(&uwHDMIWidth, &uwHDMIHeight);
    AHC_GetHDMIOutputMode(&HdmiMode);

    switch(HdmiMode)
    {
        case AHC_DISPLAY_HDMIOUTPUT_1920X1080P:
            ulWidth 	= uwHDMIWidth;
            ulHeight 	= uwHDMIHeight-POS_HDMI_1080I_BROWSER_OSD_H;// - POS_HDMI_1080I_BROWSER_OSD_H;
            uwHDMIOffX  = 0;
            uwHDMIOffY  = 0;
            break;
        case AHC_DISPLAY_HDMIOUTPUT_1920X1080I:
            ulWidth 	= uwHDMIWidth;
            ulHeight 	= (uwHDMIHeight<<1)-POS_HDMI_1080I_BROWSER_OSD_H;
            uwHDMIOffX  = 0;
            uwHDMIOffY  = 0;
            break;
        case AHC_DISPLAY_HDMIOUTPUT_1280X720P:
            ulWidth 	= uwHDMIWidth;
            ulHeight 	= uwHDMIHeight-POS_HDMI_720P_BROWSER_OSD_H;
            uwHDMIOffX	= 0;
            uwHDMIOffY  = 0;
            break;
        case AHC_DISPLAY_HDMIOUTPUT_640X480P:  //TBD
        case AHC_DISPLAY_HDMIOUTPUT_720X480P:
        case AHC_DISPLAY_HDMIOUTPUT_720X576P:
        case AHC_DISPLAY_HDMIOUTPUT_1280X720P_50FPS:
        case AHC_DISPLAY_HDMIOUTPUT_1920X1080P_30FPS:
        default:
            printc("HDMIFunc_ChangeOSDStatus Error!\r\n");
            break;
    }

    if((HdmiStatus == AHC_HDMI_PHOTO_PB_STATUS) || (HdmiStatus == AHC_HDMI_SLIDE_SHOW_STATUS)) {
        AIHC_SetGUIMemStartAddr(AHC_GUI_TEMP_BASE_ADDR_HDMI_PHOTO_PB);
    }
    else {
        AIHC_SetGUIMemStartAddr(AHC_GUI_TEMP_BASE_ADDR_HDMI);
    }

    switch(HdmiStatus)
    {
	    case AHC_HDMI_BROWSER_STATUS:
	    	HDMIFunc_InitBrowserSmallOSD();

			AHC_OSDInit(   320, 240,/*ulWidth,    ulHeight,*/
							ulWidth,    ulHeight,
							PRIMARY_DATESTAMP_WIDTH,    PRIMARY_DATESTAMP_HEIGHT,
							THUMB_DATESTAMP_WIDTH,      THUMB_DATESTAMP_HEIGHT,
							WMSG_LAYER_WIN_COLOR_FMT,
							AHC_FALSE);
	    break;

	    case AHC_HDMI_MOVIE_PB_STATUS:
	    case AHC_HDMI_PHOTO_PB_STATUS:
	    case AHC_HDMI_SLIDE_SHOW_STATUS:
	        HDMIFunc_InitPlaySmallOSD();
            AHC_OSDInit( 320, 240,
    		              0, 0,
                          0, 0,
                          0, 0,
                          WMSG_LAYER_WIN_COLOR_FMT,
                          AHC_TRUE);
	    break;
    }

	#if (HDMI_MENU_EN)
	{
		UINT32		pos[2];
		UINT32		uwDispScale = 0;
		UINT16		uwHDMITempWidth, uwHDMITempHeight;	
	
		if((MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_1080I)||(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_1080P))
		{
			uwDispScale = OSD_DISPLAY_SCALE_HDMI_FHD;
		}
		else if(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_720P)
		{
			uwDispScale = OSD_DISPLAY_SCALE_HDMI_HD;
		}
		else if(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_480P)
		{
			uwDispScale = OSD_DISPLAY_SCALE_TV;
		}
	
		uwHDMITempWidth = uwHDMIWidth / uwDispScale;
		uwHDMITempHeight = uwHDMIHeight / uwDispScale;
	
		AHC_OSDCreateBuffer(HDMI_MENU_OSD_ID, uwHDMITempWidth, uwHDMITempHeight, OSD_COLOR_RGB565);
		pos[0] = uwHDMIOffX;
		pos[1] = uwHDMIOffY;
		AHC_OSDSetDisplayAttr(HDMI_MENU_OSD_ID, AHC_OSD_ATTR_DISPLAY_OFFSET, pos);
        pos[0] = uwDispScale;
        pos[1] = uwDispScale;
        AHC_OSDSetDisplayAttr(HDMI_MENU_OSD_ID, AHC_OSD_ATTR_DISPLAY_SCALE, pos);
		AHC_OSDClearBuffer(HDMI_MENU_OSD_ID);
	
		AHC_OSDCreateBuffer(HDMI_MENU_OSD2_ID, uwHDMITempWidth, uwHDMITempHeight, OSD_COLOR_RGB565);
		pos[0] = uwHDMIOffX;
		pos[1] = uwHDMIOffY;
		AHC_OSDSetDisplayAttr(HDMI_MENU_OSD2_ID, AHC_OSD_ATTR_DISPLAY_OFFSET, pos);
        pos[0] = uwDispScale;
        pos[1] = uwDispScale;
        AHC_OSDSetDisplayAttr(HDMI_MENU_OSD2_ID, AHC_OSD_ATTR_DISPLAY_SCALE, pos);
		AHC_OSDClearBuffer(HDMI_MENU_OSD2_ID);
	}
    #endif
    //** Set HDMI OSD#1 by uwDisplayID = 16
    AHC_OSDCreateBuffer(HDMISmallOSD[0].uwDisplayID, HDMISmallOSD[0].uwWidth, HDMISmallOSD[0].uwHeight,OSD_COLOR_RGB565);
    HDMIFunc_SetSmallOSDDisplayOffset(0);
    {
        UINT32 pos[2];
        pos[0] = OSD_FLIP_DRAW_NONE_ENABLE;                
        AHC_OSDSetDisplayAttr(HDMISmallOSD[0].uwDisplayID,  AHC_OSD_ATTR_FLIP_BY_GUI, pos);
        pos[0] = OSD_ROTATE_DRAW_NONE;
        AHC_OSDSetDisplayAttr(HDMISmallOSD[0].uwDisplayID,  AHC_OSD_ATTR_ROTATE_BY_GUI, pos);
    }
    AHC_OSDSetCurrentDisplay(HDMISmallOSD[0].uwDisplayID);
    

    //** Set HDMI OSD#2 by uwDisplayID = 17
    AHC_OSDCreateBuffer(HDMISmallOSD[1].uwDisplayID, HDMISmallOSD[1].uwWidth, HDMISmallOSD[1].uwHeight,OSD_COLOR_RGB565);
    HDMIFunc_SetSmallOSDDisplayOffset(1);
    {
        UINT32 pos[2];
        pos[0] = OSD_FLIP_DRAW_NONE_ENABLE;                
        AHC_OSDSetDisplayAttr(HDMISmallOSD[1].uwDisplayID,  AHC_OSD_ATTR_FLIP_BY_GUI, pos);
        pos[0] = OSD_ROTATE_DRAW_NONE;
        AHC_OSDSetDisplayAttr(HDMISmallOSD[1].uwDisplayID,  AHC_OSD_ATTR_ROTATE_BY_GUI, pos);
    }
    AHC_OSDSetCurrentDisplay(HDMISmallOSD[1].uwDisplayID);

    m_HdmiStatus = HdmiStatus;
}

#if 0
void ____HDMI_Preview_Function_____(){ruturn;} //dummy
#endif

AHC_BOOL HDMIFunc_PreviewVideo(void)
{
#if (HDMI_PREVIEW_EN)

    m_uwHdmiPrevOsdId = HDMIFunc_InitPreviewOSD();

   	bPreviewModeHDMI = AHC_TRUE;
    VideoTimer_Start(VIDEO_TIMER_UNIT);

	if (VideoFunc_Preview())
	{
		DrawStateHdmiPreviewVideoInit(m_uwHdmiPrevOsdId);
		m_HdmiStatus = AHC_HDMI_VIDEO_PREVIEW_STATUS;
	}
#endif
	return AHC_TRUE;
}

AHC_BOOL HDMIFunc_PreviewCamera(void)
{
#if (HDMI_PREVIEW_EN)

    m_uwHdmiPrevOsdId = HDMIFunc_InitPreviewOSD();

   	bPreviewModeHDMI = AHC_TRUE;
    CaptureTimer_Start(100);

	if(CaptureFunc_Preview())
	{
		DrawStateHdmiPreviewCameraInit(m_uwHdmiPrevOsdId);
		m_HdmiStatus = AHC_HDMI_DSC_PREVIEW_STATUS;
	}
#endif
	return AHC_TRUE;
}

#if 0
void ____HDMI_Browser_Function_____(){ruturn;} //dummy
#endif

UINT32 HDMIFunc_BrowserSetting(void)
{
    UINT32 MaxDcfObj;
    UINT16 uwWidth, uwHeight;
    AHC_DISPLAY_HDMIOUTPUTMODE HdmiMode;

    AHC_GetHdmiDisplayWidthHeight(&uwWidth, &uwHeight);
    AHC_GetHDMIOutputMode(&HdmiMode);

    m_ThumbConfig.uwTHMPerLine    = 4;
    m_ThumbConfig.uwTHMPerColume  = 3;
    m_ThumbConfig.uwHGap          = 12;
    m_ThumbConfig.uwVGap          = 12;
    m_ThumbConfig.byBkColor       = 0x101010;
    m_ThumbConfig.byFocusColor    = 0x0000FF;

    switch(MenuSettingConfig()->uiHDMIOutput)
    {
    case HDMI_OUTPUT_1080I:
        m_ThumbConfig.StartX          = 50;
        m_ThumbConfig.StartY          = 30;
        break;
    case HDMI_OUTPUT_720P:
        m_ThumbConfig.StartX          = 30;
        m_ThumbConfig.StartY          = 20;
        break;
    case HDMI_OUTPUT_480P:
        m_ThumbConfig.StartX          = 30;
        m_ThumbConfig.StartY          = 20;
        break;
    default:
    case HDMI_OUTPUT_1080P:
        m_ThumbConfig.StartX          = 50;
        m_ThumbConfig.StartY          = 30;
        break;
    }

    switch(HdmiMode)
    {
        case AHC_DISPLAY_HDMIOUTPUT_1920X1080P:
            m_ThumbConfig.DisplayWidth    = uwWidth - m_ThumbConfig.StartX - 40;
            m_ThumbConfig.DisplayHeight   = uwHeight - POS_HDMI_1080P_BROWSER_OSD_H - 50;
            break;
        case AHC_DISPLAY_HDMIOUTPUT_1920X1080I:
            m_ThumbConfig.DisplayWidth    = uwWidth - m_ThumbConfig.StartX - 40;
            m_ThumbConfig.DisplayHeight   = (uwHeight<<1) - POS_HDMI_1080I_BROWSER_OSD_H - 50;
            break;
        case AHC_DISPLAY_HDMIOUTPUT_1280X720P:
            m_ThumbConfig.DisplayWidth    = uwWidth  - m_ThumbConfig.StartX - 30;
            m_ThumbConfig.DisplayHeight   = uwHeight - POS_HDMI_720P_BROWSER_OSD_H - 30;
            break;
        case AHC_DISPLAY_HDMIOUTPUT_720X480P:   // TBD
            m_ThumbConfig.DisplayWidth    = uwWidth  - m_ThumbConfig.StartX - 30;
            m_ThumbConfig.DisplayHeight   = uwHeight - POS_HDMI_480P_BROWSER_OSD_H - 20;
            break;
        case AHC_DISPLAY_HDMIOUTPUT_640X480P:  //TBD
        case AHC_DISPLAY_HDMIOUTPUT_720X576P:
        case AHC_DISPLAY_HDMIOUTPUT_1280X720P_50FPS:
        case AHC_DISPLAY_HDMIOUTPUT_1920X1080P_30FPS:
        default:
            printc("HDMIFunc_BrowserSetting Error!!\r\n");
            break;
    }

    AHC_Thumb_Config(&m_ThumbConfig, AHC_FALSE);

    if(GetCurrentOpMode()==MOVPB_MODE)
    	MaxDcfObj = AHC_GetMappingFileNum(FILE_MOVIE);
    else if(GetCurrentOpMode()==JPGPB_MODE)
    	MaxDcfObj = AHC_GetMappingFileNum(FILE_PHOTO);
	else if(GetCurrentOpMode()==JPGPB_MOVPB_MODE)
			MaxDcfObj = AHC_GetMappingFileNum(FILE_PHOTO_MOVIE);

	if (0 == MaxDcfObj) {
		AHC_WMSG_Draw(AHC_TRUE, WMSG_NO_FILE_IN_BROWSER, 2);
		ObjSelect = 0;
	}
	else {
	    if( ObjSelect > MaxDcfObj - 1)
	    	ObjSelect = MaxDcfObj - 1;
	}

    return MaxDcfObj;
}

AHC_BOOL HDMIFunc_Enable(AHC_BOOL bEnable)
{
    AHC_BOOL 	ahc_ret = AHC_TRUE;
    UINT32 		iVal[2];

	m_DelFile = DEL_FILE_NONE;

    if (bEnable)
    {
        if(AHC_IsHdmiConnect()) {
            if(m_HdmiStatus != AHC_HDMI_BROWSER_STATUS)
            {
                RTNA_LCD_Backlight(MMP_FALSE);
                #ifdef CFG_ENBLE_PANEL_SLEEP
                RTNA_LCD_Enter_Sleep();
                #endif

                switch (MenuSettingConfig()->uiHDMIOutput)
                {
                case HDMI_OUTPUT_1080I:
                    printc("@@@ HDMI_OUTPUT_1080I\r\n");
                    AHC_SetHDMIOutputMode(AHC_DISPLAY_HDMIOUTPUT_1920X1080I);
                    break;
                case HDMI_OUTPUT_720P:
                    printc("@@@ HDMI_OUTPUT_720P\r\n");
                    AHC_SetHDMIOutputMode(AHC_DISPLAY_HDMIOUTPUT_1280X720P);
                    break;
                case HDMI_OUTPUT_480P:
                    printc("@@@ HDMI_OUTPUT_480P\r\n");
                    AHC_SetHDMIOutputMode(AHC_DISPLAY_HDMIOUTPUT_720X480P);
                    break;
                default:
                case HDMI_OUTPUT_1080P:
                    printc("@@@ HDMI_OUTPUT_1080P\r\n");
                    AHC_SetHDMIOutputMode(AHC_DISPLAY_HDMIOUTPUT_1920X1080P);
                    break;
                }

				switch(m_HdmiStatus)
				{
                case AHC_HDMI_NONE_STATUS:

	                HDMIFunc_ChangeOSDStatus(AHC_HDMI_BROWSER_STATUS);
					
					AHC_SetDisplayMode(DISPLAY_MODE_DISABLE);
					
					MenuStatePlaybackSelectDB( GetCurrentOpMode() );
					
					SetCurrentOpMode(GetCurrentOpMode());

					AHC_SetDisplayMode(DISPLAY_MODE_OSD_DISABLE);
					
					BrowserFunc_ThumbnailEditCheckFileObj();

	                HDMIFunc_BrowserSetting();

	                AIHC_DrawReservedOSD(AHC_TRUE);
	                iVal[0] = 0;

	                switch(MenuSettingConfig()->uiHDMIOutput)
	                {
	                case HDMI_OUTPUT_1080I:
	                    iVal[1] = POS_HDMI_1080I_BROWSER_OSD_H>>1;
					break;
	                case HDMI_OUTPUT_720P:
	                    iVal[1] = POS_HDMI_720P_BROWSER_OSD_H;
	                break;
	                case HDMI_OUTPUT_480P:  // TBD
	                    iVal[1] = POS_HDMI_480P_BROWSER_OSD_H;
	                break;
	                default:
	                case HDMI_OUTPUT_1080P:
	                    iVal[1] = POS_HDMI_1080P_BROWSER_OSD_H;
					break;
	                }

	                AHC_OSDSetDisplayAttr(THUMBNAIL_OSD_BACK_ID, AHC_OSD_ATTR_DISPLAY_OFFSET, iVal);
	                AHC_OSDSetCurrentDisplay(THUMBNAIL_OSD_BACK_ID);

	                AHC_OSDSetDisplayAttr(THUMBNAIL_OSD_FRONT_ID, AHC_OSD_ATTR_DISPLAY_OFFSET, iVal);
	                AHC_OSDSetCurrentDisplay(THUMBNAIL_OSD_FRONT_ID);

	                AIHC_DrawReservedOSD(AHC_FALSE);

	                if(AHC_FALSE == AHC_SDMMC_GetMountState())
	                	AHC_DrawThumbNailEmpty();
	                else
	                	AHC_Thumb_DrawPage(AHC_TRUE);

	                DrawStateHdmiBrowserUpdate(EVENT_HDMI_CHANGE_MODE_INIT);

	                #ifdef CFG_HDMI_AS_SLIDESHOW //may be defined in config_xxx.h
	                StateHDMI_BrowserMode_Handler(EVENT_SLIDESHOW_MODE);
	                m_HdmiStatus = AHC_HDMI_SLIDE_SHOW_STATUS;
	                #else
	                m_HdmiStatus = AHC_HDMI_BROWSER_STATUS;
	                #endif
	            break;

                case AHC_HDMI_MOVIE_PB_STATUS:
                case AHC_HDMI_PHOTO_PB_STATUS:
                    HDMIFunc_ThumbnailOption(BROWSER_PLAY);
                break;

                case AHC_HDMI_SLIDE_SHOW_STATUS:
                    HDMIFunc_SlideShow();
                break;

                #if (HDMI_PREVIEW_EN)
                case AHC_HDMI_VIDEO_PREVIEW_STATUS:
					HDMIFunc_PreviewVideo();
                break;

                case AHC_HDMI_DSC_PREVIEW_STATUS:
                	HDMIFunc_PreviewCamera();
                break;
                #endif
            #ifdef NET_SYNC_PLAYBACK_MODE
    			case AHC_HDMI_NET_PLAYBACK_STATUS :
  					m_uwHdmiPrevOsdId = HDMIFunc_InitPreviewOSD();
        			DrawStateHdmiPreviewVideoInit(m_uwHdmiPrevOsdId);
        			BEGIN_LAYER(m_uwHdmiPrevOsdId);
        			AHC_OSDSetActive(m_uwHdmiPrevOsdId, 1);
        			END_LAYER(m_uwHdmiPrevOsdId);
            		AHC_SetMode(AHC_MODE_IDLE); 
            		DrawStateHDMINetPlaybackInit();
        		break;
    		#endif
                }

        		Menu_SetVolume(MenuSettingConfig()->uiVolume);//TBD
            }
        }
        else {
            printc("--E-- HDMI cable out\r\n");
            return AHC_FALSE;
        }
    }
    else
    {
        if( m_HdmiStatus )
        {
            AHC_OSDUninit();
            
            AHC_SetMode(AHC_MODE_IDLE);

            AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
            InitOSD();
            AHC_UF_GetCurrentIndex(&ObjSelect);
            StateSwitchMode(UI_VIDEO_STATE);

            RTNA_LCD_Backlight(MMP_TRUE);
        }
    }

    return ahc_ret;
}

#endif

AHC_BOOL HDMIFunc_ThumbnailPlayback(void)
{
#if (HDMI_ENABLE)

    AHC_BOOL 	ahc_ret = AHC_TRUE;

    if(MOVPB_MODE == GetCurrentOpMode())
    {
        if(AHC_GetMappingFileNum(FILE_MOVIE) == 0)
            return AHC_FALSE;

        HDMIFunc_ChangeOSDStatus(AHC_HDMI_MOVIE_PB_STATUS);

        m_bMovieOSDForceShowUp = AHC_FALSE;
        MovPlaybackParamReset();
        MovPBTimer_Start(300);

        MediaPlaybackConfig(PLAYBACK_CURRENT);

        if(bMovPbEnterToHdmi)
        {
        	switch(bMovieState)
        	{
        		case MOVIE_STATE_STOP:
        			//TBD
        		break;

        		case MOVIE_STATE_PLAY:
        			ahc_ret = MovPlayback_Play();
        			MovPBFunc_ResetPlaySpeed();
        		break;

        		case MOVIE_STATE_PAUSE:
        			ahc_ret = MovPlayback_Play();
        			MovPBFunc_ResetPlaySpeed();
        			AHC_OS_SleepMs(100);
        			ahc_ret = MovPlayback_Pause();
        		break;

        		case MOVIE_STATE_FF:
        			ahc_ret = MovPlayback_Play();
        			AHC_OS_SleepMs(100);

					AHC_SetPlaySpeed(AHC_PB_MOVIE_FAST_FORWARD_RATE, m_MovPlayFFSpeed+1);
	        		ahc_ret = MovPlayback_Forward();
        		break;

        		case MOVIE_STATE_REW:
        			ahc_ret = MovPlayback_Play();
        			AHC_OS_SleepMs(100);

					AHC_SetPlaySpeed(AHC_PB_MOVIE_BACKWARD_RATE, m_MovPlayREWSpeed+1);
	       	 		MovPlayback_Backward();
        		break;

        		default:
        		break;
        	}
        }
        else
        {
	        ahc_ret = MovPlayback_Play();
	        MovPBFunc_ResetPlaySpeed();
        }

        if(ahc_ret)
        {
        	Menu_SetVolume(MenuSettingConfig()->uiVolume);

        	if(bMovPbEnterToHdmi)
        	{
        		UpdateHdmiMovPBStatus(bMovieState);
        		bMovPbEnterToHdmi = AHC_FALSE;
        	}
        	else
        	{
        		UpdateHdmiMovPBStatus(MOVIE_STATE_PLAY);
        	}

        	DrawStateHdmiMoviePBUpdate(EVENT_HDMI_CHANGE_MODE_INIT);
        	MovPBFunc_SetOSDShowStatus(AHC_TRUE);
        }
        else
        {
            MovPBTimer_Stop();
            m_HdmiStatus = AHC_HDMI_NONE_STATUS;
            HDMIFunc_Enable(AHC_TRUE);
            AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);
            return ahc_ret;
        }
    }
    else if( JPGPB_MODE == GetCurrentOpMode())
    {
        if(AHC_GetMappingFileNum(FILE_PHOTO) == 0)
            return AHC_FALSE;

        HDMIFunc_ChangeOSDStatus(AHC_HDMI_PHOTO_PB_STATUS);

        AHC_SetMode(AHC_MODE_IDLE);

        MediaPlaybackConfig(PLAYBACK_CURRENT);
        PhotoPBTimer_Start(300);
        ahc_ret = JpgPlayback_Play();

        if(ahc_ret)
        {
            DrawStateHdmiPhotoPBUpdate(EVENT_HDMI_CHANGE_MODE_INIT);
        	PhotoPBFunc_SetOSDShowStatus(AHC_TRUE);
        }
        else
        {
            PhotoPBTimer_Stop();
            m_HdmiStatus = AHC_HDMI_NONE_STATUS;
            HDMIFunc_Enable(AHC_TRUE);
            AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);
        }
    }
	else if( JPGPB_MOVPB_MODE == GetCurrentOpMode())
	{
	    UINT32 uiCurrentIdx;
        UINT8  ubFileType;
	    AHC_UF_GetCurrentIndex(&uiCurrentIdx);
        AHC_UF_GetFileTypebyIndex(uiCurrentIdx, &ubFileType);

		if(AHC_GetMappingFileNum(FILE_PHOTO_MOVIE) == 0)
			return AHC_FALSE;

        #if (SLIDESHOW_EN)
			HDMIFunc_ChangeOSDStatus(AHC_HDMI_SLIDE_SHOW_STATUS);
        #else
		if ( ubFileType == DCF_OBG_JPG )
			HDMIFunc_ChangeOSDStatus(AHC_HDMI_PHOTO_PB_STATUS);
		else
			HDMIFunc_ChangeOSDStatus(AHC_HDMI_MOVIE_PB_STATUS);
		#endif
		
		AHC_SetMode(AHC_MODE_IDLE);

		MediaPlaybackConfig(PLAYBACK_CURRENT);

        #if (SLIDESHOW_EN)
		    //SlideOSDShowUp 		= AHC_TRUE;
		    bSlideShow 			= AHC_TRUE;
			SlideShowTimer_Start(300);
			if ( ubFileType == DCF_OBG_JPG ) 
				ahc_ret = JpgPlayback_Play();
			else
				ahc_ret = MovPlayback_Play();
		#else
		if ( ubFileType == DCF_OBG_JPG ) {
			PhotoPBTimer_Start(300);
		    ahc_ret = JpgPlayback_Play();
		}	
		else {
			MovPBTimer_Start(300);
		    ahc_ret = MovPlayback_Play();
		}	
		#endif
		
		if(ahc_ret)
		{
		    if ( ubFileType == DCF_OBG_JPG ) {
		 		DrawStateHdmiPhotoPBUpdate(EVENT_HDMI_CHANGE_MODE_INIT);
				PhotoPBFunc_SetOSDShowStatus(AHC_TRUE);
		    }	
            else {  
				DrawStateHdmiMoviePBUpdate(EVENT_HDMI_CHANGE_MODE_INIT);
        		MovPBFunc_SetOSDShowStatus(AHC_TRUE);
            }	
		}
		else
		{
		    #if (SLIDESHOW_EN)
				SlideShowTimer_Stop();
			#else
		    if ( ubFileType == DCF_OBG_JPG )
				PhotoPBTimer_Stop();
			else
				MovPBTimer_Stop();   
			#endif
			
			m_HdmiStatus = AHC_HDMI_NONE_STATUS;
			HDMIFunc_Enable(AHC_TRUE);
			AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);
		}
	}

    return ahc_ret;
#else
	return AHC_TRUE;
#endif
}

AHC_BOOL HDMIFunc_ThumbnailOption(UINT8 ubOp)
{
#if (HDMI_ENABLE)

	AHC_BOOL ahc_ret = AHC_TRUE;

	switch(ubOp)
	{
		case BROWSER_UP:
			ahc_ret = AHC_Thumb_Rewind(m_ThumbConfig.uwTHMPerLine);
		break;

		case BROWSER_DOWN:
			ahc_ret = AHC_Thumb_Forward(m_ThumbConfig.uwTHMPerLine);
		break;

		case BROWSER_LEFT:
			ahc_ret = AHC_Thumb_Rewind(1);
		break;

		case BROWSER_RIGHT:
			ahc_ret = AHC_Thumb_Forward(1);
		break;

		case BROWSER_PLAY:
			ahc_ret = HDMIFunc_ThumbnailPlayback();
		break;

		case BROWSER_PAGEUP:
			ahc_ret = AHC_Thumb_RewindPage();
		break;

		case BROWSER_PAGEDOWN:
			ahc_ret = AHC_Thumb_ForwardPage();
		break;

		default:

		break;
	}
	return ahc_ret;
#else
	return AHC_TRUE;
#endif
}

AHC_BOOL HDMIFunc_ThumbnailModeSwitch(void)
{
#if (HDMI_ENABLE)

    AHC_SetMode(AHC_MODE_IDLE);

    if(JPGPB_MODE == GetCurrentOpMode())
    {
		MenuStatePlaybackSelectDB( MOVPB_MODE );
        SetCurrentOpMode(MOVPB_MODE);
    }
    else if(MOVPB_MODE == GetCurrentOpMode())
    {
		MenuStatePlaybackSelectDB( JPGPB_MODE );
        SetCurrentOpMode(JPGPB_MODE);
	}

    HDMIFunc_BrowserSetting();
    AHC_Thumb_DrawPage(AHC_TRUE);
#endif
    return AHC_TRUE;
}

AHC_BOOL HDMIFunc_Thumbnail2Preview(void)
{
#if (HDMI_ENABLE)

    AHC_BOOL ahc_ret = AHC_TRUE;

    AHC_OSDUninit();
   
    AHC_SetMode(AHC_MODE_IDLE);

    AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
    InitOSD();

    if(MOVPB_MODE == GetCurrentOpMode())
        ahc_ret = StateSwitchMode(UI_VIDEO_STATE);
    else if(JPGPB_MODE == GetCurrentOpMode())
        ahc_ret = StateSwitchMode(UI_CAMERA_STATE);
	else if(JPGPB_MOVPB_MODE == GetCurrentOpMode())
        ahc_ret = StateSwitchMode(UI_VIDEO_STATE);

    RTNA_LCD_Backlight(MMP_TRUE);

    m_HdmiStatus = AHC_HDMI_NONE_STATUS;

    return ahc_ret;
#else
	return AHC_TRUE;
#endif
}

AHC_BOOL HDMIFunc_ThumbnailEditOneFile(void)
{
#if (HDMI_ENABLE)

	if(	m_HdmiStatus!=AHC_HDMI_MOVIE_PB_STATUS 	&&
		m_HdmiStatus!=AHC_HDMI_PHOTO_PB_STATUS	&&
		m_HdmiStatus!=AHC_HDMI_SLIDE_SHOW_STATUS )
	{
		bPreviewModeHDMI = AHC_FALSE;
		m_HdmiStatus = AHC_HDMI_NONE_STATUS;
		if(GetCurrentOpMode()==JPGPB_MODE)
			SetCurrentOpMode(JPGPB_MODE);
		else if(GetCurrentOpMode()==MOVPB_MODE)
			SetCurrentOpMode(MOVPB_MODE);
		else if(GetCurrentOpMode()==JPGPB_MOVPB_MODE)
			SetCurrentOpMode(JPGPB_MOVPB_MODE);
		HDMIFunc_Enable(AHC_TRUE);
		bHdmiMenuMode = AHC_FALSE;
	}
#endif
	return AHC_TRUE;
}

AHC_BOOL HDMIFunc_ThumbnailQuickFileDelete(void)
{
#if (HDMI_ENABLE)

	UINT8	 ret;

   	ret = QuickFileOperation(DCF_FILE_DELETE);

	if(ret!=EDIT_FILE_SUCCESS)
	{
		printc("Edit File ERROR!!!\r\n");
		return AHC_FALSE;
	}
	else
	{
		printc("Edit File Complete!\r\n");

		AHC_Thumb_DrawPage(AHC_TRUE);

		//Renew Focused Filename
		DrawStateHdmiBrowserPageUpdate();
	}
#endif
	return AHC_TRUE;
}

AHC_BOOL HDMIFunc_PlaybackEditOneFile(UINT8 Op)
{
#if (HDMI_ENABLE)

	if(	m_HdmiStatus==AHC_HDMI_MOVIE_PB_STATUS 	||
		m_HdmiStatus==AHC_HDMI_PHOTO_PB_STATUS	||
		m_HdmiStatus==AHC_HDMI_SLIDE_SHOW_STATUS )
	{
		if(	m_HdmiStatus==AHC_HDMI_MOVIE_PB_STATUS ||
			m_HdmiStatus==AHC_HDMI_SLIDE_SHOW_STATUS )
			SetCurrentOpMode(MOVPB_MODE);
		else if( m_HdmiStatus==AHC_HDMI_PHOTO_PB_STATUS ) 
			SetCurrentOpMode(JPGPB_MODE);

		GetCurrentMenu()->pParentMenu = NULL;
		BrowserFunc_ThumbnailResetAllEditOp();
		ResetCurrentMenu();
		Menu_WriteSetting();
		bHdmiMenuMode = AHC_FALSE;

		m_HdmiStatus = AHC_HDMI_NONE_STATUS;
		HDMIFunc_Enable(AHC_TRUE);
		bHdmiMenuMode = AHC_FALSE;
	}
#endif
	return AHC_TRUE;
}

#if 0
void ____HDMI_MoviePB_Function_____(){ruturn;} //dummy
#endif

#if (HDMI_ENABLE)

AHC_BOOL HDMIFunc_MoviePBCheckOSDShowUp(UINT32 ulEvent)
{
    UINT8  		bID0 = 0;
    AHC_BOOL 	state;

    if(m_HdmiStatus != AHC_HDMI_MOVIE_PB_STATUS)
    {
        return AHC_TRUE;
    }

	if(	MOV_STATE_PLAY 	!= GetMovConfig()->iState &&
		MOV_STATE_FF 	!= GetMovConfig()->iState &&
		MOV_STATE_PAUSE != GetMovConfig()->iState)
    {
       return AHC_TRUE;
    }

    if(ulEvent == EVENT_HDMI_REMOVED)
    {
        return AHC_TRUE;
    }

    if(ulEvent == EVENT_MOVPB_UPDATE_MESSAGE)
    {
        if( MOV_STATE_STOP == GetMovConfig()->iState || MOV_STATE_PAUSE == GetMovConfig()->iState)
        {
            MovPBTimer_ResetCounter();
            OSDDraw_GetLastOvlDrawBuffer(&bID0);
            AHC_OSDSetActive(bID0, 1);
            return AHC_TRUE;
        }
        else if(MOV_STATE_PLAY == GetMovConfig()->iState || MOV_STATE_FF == GetMovConfig()->iState)
        {
            state = MovPBFunc_GetOSDShowStatus();

            if(state == AHC_FALSE) {
                OSDDraw_GetLastOvlDrawBuffer( &bID0 );
                AHC_OSDSetActive(bID0, 0);
                return AHC_FALSE;
            }
        }
    }

    state = MovPBFunc_GetOSDShowStatus();

    if(state)
    {
        return AHC_TRUE;
    }
    else
    {
    	MovPBTimer_ResetCounter();
        OSDDraw_GetLastOvlDrawBuffer( &bID0 );
        AHC_OSDSetActive(bID0, 1);
        MovPBFunc_SetOSDShowStatus(AHC_TRUE);
        return AHC_FALSE;
    }
}

AHC_BOOL HDMIFunc_MoviePBPrevious(void)
{
    AHC_BOOL ahc_ret = AHC_TRUE;

	MediaPlaybackConfig(PLAYBACK_PREVIOUS);
	ahc_ret = MovPlayback_Play();

    if(!ahc_ret)
    {
        MovPBTimer_Stop();
        m_HdmiStatus = AHC_HDMI_NONE_STATUS;
        HDMIFunc_Enable(AHC_TRUE);
        AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);
    }

    return ahc_ret;
}

AHC_BOOL HDMIFunc_MoviePBNext(void)
{
    AHC_BOOL ahc_ret = AHC_TRUE;

	MediaPlaybackConfig(PLAYBACK_NEXT);
	ahc_ret = MovPlayback_Play();

    if(!ahc_ret)
    {
        MovPBTimer_Stop();
        m_HdmiStatus = AHC_HDMI_NONE_STATUS;
        HDMIFunc_Enable(AHC_TRUE);
        AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);
    }

    return ahc_ret;
}

AHC_BOOL HDMIFunc_MoviePBPlayPress(void)
{
    AHC_BOOL ahc_ret = AHC_TRUE;
    UINT32   uState;

    AHC_GetSystemStatus(&uState);

    switch(uState)
    {
	    case STATE_MOV_IDLE :
	        MovPBTimer_Start(300);
	        m_bMovieOSDForceShowUp = AHC_FALSE;
	        MediaPlaybackConfig(PLAYBACK_CURRENT);

	        if(!MovPlayback_Play())
	        {
	            MovPBTimer_Stop();
	            m_HdmiStatus = AHC_HDMI_NONE_STATUS;
	            HDMIFunc_Enable(AHC_TRUE);
	            AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);
	            return AHC_FALSE;
        	}
	        DrawStateHdmiMoviePBUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);
	    break;

	    case STATE_MOV_EXECUTE :
	        if(MOV_STATE_FF == GetMovConfig()->iState)
	        {
				MovPlayback_NormalSpeed();
				DrawStateHdmiMoviePBUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);
	        }
	        else
	        {
				MovPlayback_Pause();
				DrawStateHdmiMoviePBUpdate(EVENT_MOVPB_TOUCH_PAUSE_PRESS);
	        }
	    break;

	    case STATE_MOV_PAUSE   :
	    	AHC_PlaybackClipCmd(AHC_PB_MOVIE_NORMAL_PLAY_SPEED, 0);
	        MovPlayback_Resume();
	        DrawStateHdmiMoviePBUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);
	    break;

	    default                :
	    break;
    }

    return ahc_ret;
}

AHC_BOOL HDMIFunc_MoviePBSeekSOS(void)
{
	AHC_SetVideoPlayStartTime(0);

	#if (MOVIE_PB_FF_EOS_ACTION==EOS_ACT_PAUSE_AT_FIRST_FRAME)
	AHC_SetParam(PARAM_ID_MOVIE_AUTO_PLAY, 1);
	#endif

	MovPlaybackParamReset();
    MediaPlaybackConfig(PLAYBACK_CURRENT);
    MovPlayback_Play();
    MovPBFunc_ResetPlaySpeed();

	#if (MOVIE_PB_FF_EOS_ACTION==EOS_ACT_PAUSE_AT_FIRST_FRAME)
	AHC_SetParam(PARAM_ID_MOVIE_AUTO_PLAY, 0);
	DrawStateHdmiMoviePBUpdate(EVENT_MOVPB_TOUCH_PAUSE_PRESS);
	#else
	DrawStateHdmiMoviePBUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);
	#endif

    return AHC_TRUE;
}

AHC_BOOL HDMIFunc_MoviePBForward(void)
{
    UINT32 		ulCurTime;
    UINT32 		uState;

    AHC_GetSystemStatus(&uState);

    switch(uState)
    {
	    case STATE_MOV_IDLE :
	    break;

	    case STATE_MOV_EXECUTE :
			if(bHdmiMovieState!=MOVIE_STATE_REW || m_MovPlayFFSpeed==1)
			{
	        	m_MovPlayFFSpeed = ( (m_MovPlayFFSpeed<<1) > MOVPB_MAX_SPEED )?(MOVPB_MIN_SPEED):(m_MovPlayFFSpeed<<1);
			}
			AHC_SetPlaySpeed(AHC_PB_MOVIE_FAST_FORWARD_RATE, m_MovPlayFFSpeed+1);

	        MovPlayback_Forward();
	        bHdmiMovieState = MOVIE_STATE_FF;
	    break;

	    case STATE_MOV_PAUSE   :
	        AHC_GetVideoCurrentPlayTime(&ulCurTime);
	        AHC_PlaybackClipCmd(AHC_PB_MOVIE_NORMAL_PLAY_SPEED, 0);
	        AHC_PlaybackClipCmd(AHC_PB_MOVIE_SEEK_BY_TIME, ulCurTime+500);
	        DrawStateHdmiMoviePBUpdate(EVENT_MOVPB_UPDATE_MESSAGE);
	    break;

	    default                :
	    break;
    }

    return AHC_TRUE;
}

AHC_BOOL HDMIFunc_MoviePBBackward(void)
{
    UINT32 		ulCurTime;
    UINT32 		uState;

    AHC_GetSystemStatus(&uState);

    switch(uState)
    {
	    case STATE_MOV_IDLE :
	    break;

	    case STATE_MOV_EXECUTE :
	        if(bHdmiMovieState!=MOVIE_STATE_FF || m_MovPlayREWSpeed==1)
        	{
         		m_MovPlayREWSpeed = ( (m_MovPlayREWSpeed<<1) > MOVPB_MAX_SPEED )?(MOVPB_MIN_SPEED):(m_MovPlayREWSpeed<<1);
        	}
			AHC_SetPlaySpeed(AHC_PB_MOVIE_BACKWARD_RATE, m_MovPlayREWSpeed+1);

	        MovPlayback_Backward();
	        bHdmiMovieState = MOVIE_STATE_REW;
	    break;

	    case STATE_MOV_PAUSE   :
	        AHC_GetVideoCurrentPlayTime(&ulCurTime);
	        AHC_PlaybackClipCmd(AHC_PB_MOVIE_NORMAL_PLAY_SPEED, 0);
	        AHC_PlaybackClipCmd(AHC_PB_MOVIE_SEEK_BY_TIME, ulCurTime-900);
	        DrawStateHdmiMoviePBUpdate(EVENT_MOVPB_UPDATE_MESSAGE);
	    break;

	    default                :
	    break;
    }

    return AHC_TRUE;
}

AHC_BOOL HDMIFunc_MoviePBDelFile(void)
{
    UINT32          CurObjIdx;
    UINT32          MaxDcfObj;

    AHC_UF_GetTotalFileCount(&MaxDcfObj);

    if(MaxDcfObj == 0)
        return AHC_FALSE;

    AHC_UF_GetCurrentIndex(&CurObjIdx);
    AHC_UF_FileOperation_ByIdx(CurObjIdx, DCF_FILE_DELETE, NULL, NULL);
    AHC_UF_GetCurrentIndex(&CurObjIdx);
    AHC_UF_SetCurrentIndex(CurObjIdx);

    MovPBTimer_Start(300);
    MediaPlaybackConfig(PLAYBACK_CURRENT);

    if(!MovPlayback_Play()) {
        AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);
        return AHC_FALSE;
    }

    return AHC_TRUE;
}

AHC_BOOL HDMIFunc_MoviePBModeSwitch(void)
{
#if 0
    AHC_BOOL ahc_ret = AHC_TRUE;
    UINT32  MaxDcfObj = 0;

    SetCurrentOpMode(JPGPB_MODE);

	MaxDcfObj = AHC_GetMappingFileNum(FILE_PHOTO);

    if(MaxDcfObj == 0)
        return AHC_FALSE;

    AHC_UF_SetCurrentIndex(MaxDcfObj-1);
    ahc_ret = HDMIFunc_ThumbnailOption(BROWSER_PLAY);

    return ahc_ret;
#else
	return AHC_FALSE;
#endif
}

AHC_BOOL HDMIFunc_MoviePB2Preview(void)
{
    AHC_BOOL ahc_ret = AHC_TRUE;

    AHC_OSDUninit();
    
    AHC_SetMode(AHC_MODE_IDLE);

    AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
    InitOSD();

    ahc_ret = StateSwitchMode(UI_VIDEO_STATE);

    RTNA_LCD_Backlight(MMP_TRUE);

    m_HdmiStatus = AHC_HDMI_NONE_STATUS;

    return ahc_ret;
}

#if 0
void ____HDMI_PhotoPB_Function_____(){ruturn;} //dummy
#endif

AHC_BOOL HDMIFunc_PhotoPBCheckOSDShowUp(UINT32 ulEvent)
{
    AHC_BOOL state;

	if(m_HdmiStatus != AHC_HDMI_PHOTO_PB_STATUS)
    {
    	return AHC_TRUE;
    }

	if(ulEvent == EVENT_HDMI_REMOVED)
	{
		return AHC_TRUE;
	}

	if(ulEvent == EVENT_MOVPB_UPDATE_MESSAGE)
	{
		#if !defined(CAR_DV)//Enforce Show OSD
		AHC_OSDSetActive(OVL_BUFFER_INDEX, 0);
		#endif
		return AHC_FALSE;
	}

	state = PhotoPBFunc_GetOSDShowStatus();

	if(state) {
		return AHC_TRUE;
	}
	else {
		PhotoPBFunc_SetOSDShowStatus(AHC_TRUE);
		AHC_OSDSetActive(OVL_BUFFER_INDEX, 1);
		return AHC_FALSE;
	}

    return AHC_TRUE;
}

AHC_BOOL HDMIFunc_PhotoPBPrevious(void)
{
    AHC_BOOL ahc_ret = AHC_TRUE;

	MediaPlaybackConfig(PLAYBACK_PREVIOUS);
	ahc_ret = JpgPlayback_Play();

	if(!ahc_ret)
	{
        PhotoPBTimer_Stop();
        m_HdmiStatus = AHC_HDMI_NONE_STATUS;
        HDMIFunc_Enable(AHC_TRUE);
        AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);
    }

    return ahc_ret;
}

AHC_BOOL HDMIFunc_PhotoPBNext(void)
{
    AHC_BOOL ahc_ret = AHC_TRUE;

	MediaPlaybackConfig(PLAYBACK_NEXT);
	ahc_ret = JpgPlayback_Play();

    if(!ahc_ret)
    {
        PhotoPBTimer_Stop();
        m_HdmiStatus = AHC_HDMI_NONE_STATUS;
        HDMIFunc_Enable(AHC_TRUE);
        AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);
    }

    return ahc_ret;
}

AHC_BOOL HDMIFunc_PhotoPBDelFile(void)
{
    UINT32          CurObjIdx;
    UINT32          MaxDcfObj;

    AHC_UF_GetTotalFileCount(&MaxDcfObj);

    if(MaxDcfObj == 0)
        return AHC_FALSE;

    AHC_UF_GetCurrentIndex(&CurObjIdx);
    AHC_UF_FileOperation_ByIdx(CurObjIdx, DCF_FILE_DELETE, NULL, NULL);
    AHC_UF_GetCurrentIndex(&CurObjIdx);
    AHC_UF_SetCurrentIndex(CurObjIdx);

    MediaPlaybackConfig(PLAYBACK_CURRENT);

    if(!JpgPlayback_Play()) {
        AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);
        return AHC_FALSE;
    }

    return AHC_TRUE;
}

AHC_BOOL HDMIFunc_PhotoPBModeSwitch(void)
{
#if 0
    AHC_BOOL 	ahc_ret = AHC_TRUE;
    UINT32  	MaxDcfObj = 0;

    SetCurrentOpMode(MOVPB_MODE);

    MaxDcfObj = AHC_GetMappingFileNum(FILE_MOVIE);

    if(MaxDcfObj == 0)
        return AHC_FALSE;

    AHC_UF_SetCurrentIndex(MaxDcfObj-1);
    ahc_ret = HDMIFunc_ThumbnailOption(BROWSER_PLAY);

    return ahc_ret;
#else
	return AHC_FALSE;
#endif
}

AHC_BOOL HDMIFunc_PhotoPB2Preview(void)
{
    AHC_BOOL ahc_ret = AHC_TRUE;

    AHC_OSDUninit();

    AHC_SetMode(AHC_MODE_IDLE);

    AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
    InitOSD();

    ahc_ret = StateSwitchMode(UI_CAMERA_STATE);

    RTNA_LCD_Backlight(MMP_TRUE);

    m_HdmiStatus = AHC_HDMI_NONE_STATUS;

    return ahc_ret;
}

#if 0
void ____HDMI_Slideshow_Function_____(){ruturn;} //dummy
#endif

AHC_BOOL HDMIFunc_SlideShowCheckOSDShowUp(UINT32 ulEvent)
{
    AHC_BOOL state;

    if(m_HdmiStatus != AHC_HDMI_SLIDE_SHOW_STATUS)
    {
    	return AHC_TRUE;
    }

	if(ulEvent == EVENT_HDMI_REMOVED || ulEvent == EVENT_SLIDESHOW_UPDATE_FILE)
	{
		return AHC_TRUE;
	}

	if(ulEvent == EVENT_SLIDESHOW_OSD_HIDE)
	{
		#if !defined(CAR_DV)
		AHC_OSDSetActive(OVL_BUFFER_INDEX, 0);
		AHC_OSDSetActive(OVL_BUFFER_INDEX1, 0);
		#endif
		return AHC_FALSE;
	}

	state = SlideShowFunc_GetOSDShowStatus();

	if(state) {
		return AHC_TRUE;
	}
	else
	{
		if(ulEvent == EVENT_MOVPB_UPDATE_MESSAGE)
			return AHC_FALSE;

		SlideShowFunc_SetOSDShowStatus(AHC_TRUE);
		AHC_OSDSetActive(OVL_BUFFER_INDEX, 1);
		AHC_OSDSetActive(OVL_BUFFER_INDEX1, 1);
		DrawStateHdmiSlideShowUpdate(EVENT_SLIDESHOW_UPDATE_FILE);
		return AHC_FALSE;
	}

    return AHC_TRUE;
}

AHC_BOOL HDMIFunc_SlideShow(void)
{
    AHC_BOOL ahc_ret = AHC_TRUE;
    UINT32   MaxDcfObj;

    switch(MenuSettingConfig()->uiSlideShowFile)
    {
        case SLIDESHOW_FILE_ALL  :
        	MaxDcfObj = AHC_GetMappingFileNum(FILE_PHOTO_MOVIE);
        break;
        case SLIDESHOW_FILE_STILL:
			MaxDcfObj = AHC_GetMappingFileNum(FILE_PHOTO);
        break;
        case SLIDESHOW_FILE_MOVIE:
			MaxDcfObj = AHC_GetMappingFileNum(FILE_MOVIE);
        break;
        default                  :
        break;
    }

    if(MaxDcfObj == 0)
        return AHC_FALSE;

    HDMIFunc_ChangeOSDStatus(AHC_HDMI_SLIDE_SHOW_STATUS);

    SlideShowFunc_SetOSDShowStatus(AHC_FALSE);
    SlideShowFunc_Stop();
    SlideShowTimer_Start(100);

    MediaPlaybackConfig(PLAYBACK_CURRENT);
    ahc_ret = SlideShowFunc_Play();

    if(ahc_ret)
    {
		Menu_SetVolume(MenuSettingConfig()->uiVolume);
        DrawStateHdmiSlideShowUpdate(EVENT_HDMI_CHANGE_MODE_INIT);
    }

    SlideShowFunc_SetOSDShowStatus(AHC_TRUE);

    return ahc_ret;
}

#endif

#if 0
void ____HDMI_Main_Function_____(){ruturn;} //dummy
#endif

AHC_BOOL HDMIMode_Start(void)
{
#if (HDMI_ENABLE)

    AHC_BOOL ahc_ret = AHC_TRUE;

	if(m_HdmiStatus==AHC_HDMI_MOVIE_PB_STATUS)
	{
		AHC_PauseKeyUI();
	}

	if(Deleting_InBrowser || Protecting_InBrowser)
	{
		BrowserFunc_ThumbnailResetAllEditOp();
	}

    if(AHC_WMSG_States())
    {
       AHC_WMSG_Draw(AHC_FALSE, WMSG_NONE, 0);
    }

#if	OSD_SHOW_BATTERY_STATUS == 1
    // Pause Charger Icon update
	if(AHC_TRUE == AHC_GetChargerStatus())
	{
		AHC_EnableChargingIcon(AHC_FALSE);
		AHC_ClearAllChargingIcon();
	}
#endif

    ahc_ret = HDMIFunc_Enable(AHC_TRUE);

	m_DelFile = DEL_FILE_NONE;

	CHARGE_ICON_ENABLE(AHC_TRUE);

	/* Patch for HDMI need send ACR packet first */
	if(m_HdmiStatus!=AHC_HDMI_MOVIE_PB_STATUS)
	{
		if(	MenuSettingConfig()->uiVolume != 0 		&&
			MenuSettingConfig()->uiBeep   != BEEP_OFF)
		{
		    UINT8 	ubSoundStatus;

			Menu_SetVolume(0);
			AHC_PlaySoundEffect(AHC_SOUNDEFFECT_BUTTON);

	        do{
	        	AHC_GetSoundEffectStatus(&ubSoundStatus);
	            AHC_OS_Sleep(100);

			}while((AHC_SOUND_EFFECT_STATUS_STOP != ubSoundStatus) && (AHC_SOUND_EFFECT_STATUS_INVALID != ubSoundStatus));

			Menu_SetVolume(MenuSettingConfig()->uiVolume);
		}
	}
	else
	{
		AHC_OS_Sleep(1000);
		AHC_ResumeKeyUI();
	}

    return ahc_ret;
#else
	return AHC_TRUE;
#endif
}

void SetHDMIState(AHC_HDMI_STATUS HdmiState)
{
    m_HdmiStatus = HdmiState;
}

UINT32 SetHdmiMenuHandler(UINT32 ulJobEvent)
{
#if (HDMI_MENU_EN)
	UINT32 				ulMenuEvent,ret;
	pfMenuEventHandler	pf;
	PSMENUSTRUCT 		pMenu = GetCurrentMenu();

	if(!bHdmiMenuMode)
		return;

	ret = 0;
	switch(ulJobEvent)
	{
		case EVENT_NET_ENTER_PLAYBACK:
			ResetCurrentMenu();
			return -1;
		case EVENT_KEY_UP:
			ulMenuEvent = MENU_UP;
		break;
		case EVENT_KEY_DOWN:
			ulMenuEvent = MENU_DOWN;
		break;
		case EVENT_KEY_LEFT:
			ulMenuEvent = MENU_LEFT;
		break;
		case EVENT_KEY_MODE:
		case EVENT_KEY_RIGHT:
			ulMenuEvent = MENU_RIGHT;
		break;
		case EVENT_HDMI_BROWSER_KEY_PLAYBACK:
		case EVENT_KEY_SET:
			ulMenuEvent = MENU_OK;
		break;
		case EVENT_KEY_MENU:
			ulMenuEvent = MENU_MENU;
			StateHDMI_EventMenu_Handler(ulJobEvent);
			return;
		break;
		case EVENT_POWER_OFF:
			ulMenuEvent = MENU_POWER_OFF;
		break;
		default:
			ulMenuEvent = MENU_INIT;
			return 0;
		break;
	}

	if(IsCurrentMainMenu(pMenu))
		pf = MainMenuEventHandler;
	#if (TOP_MENU_PAGE_EN)
	else if(IsCurrentTopMenu(pMenu))
		pf = TopMenuEventHandler;
	#endif
	else
		pf = SubMenuEventHandler;

	pf(pMenu, ulMenuEvent, 0);
	
	return ret;
#else
	return 0;
#endif
	
}

#if 0
void ____Event_Event_Function_____(){ruturn;} //dummy
#endif

#if (HDMI_ENABLE)

#if (!USE_NEW_HDMI_EVENT_HANDLER)

void StateHDMI_EventUp_Handler(UINT32 ulJobEvent)
{
	if(ulJobEvent==EVENT_KEY_TELE_STOP)
	{
		if(bHdmiMenuMode)
     		return;
     	else if(m_HdmiStatus!=AHC_HDMI_VIDEO_PREVIEW_STATUS &&
     			m_HdmiStatus!=AHC_HDMI_DSC_PREVIEW_STATUS)
     		return;
    }

    if(bHdmiMenuMode)
    {
		SetHdmiMenuHandler(ulJobEvent);
    	return;
    }

	switch(m_HdmiStatus)
	{
#if (HDMI_PREVIEW_EN)
		case AHC_HDMI_VIDEO_PREVIEW_STATUS:
		case AHC_HDMI_DSC_PREVIEW_STATUS:
			if(!bHdmiMenuMode)
			{
				//Zoom Operation
				if(ulJobEvent==EVENT_KEY_UP)
					StateCaptureMode(EVENT_KEY_TELE_PRESS);
				else if(ulJobEvent==EVENT_KEY_TELE_STOP)
					StateCaptureMode(EVENT_KEY_TELE_STOP);
			}
		break;
#endif
		case AHC_HDMI_BROWSER_STATUS:
			if(!bHdmiMenuMode)
			{
				CHARGE_ICON_ENABLE(AHC_FALSE);
				HDMIFunc_ThumbnailOption(BROWSER_UP);
				DrawStateHdmiBrowserUpdate(ulJobEvent);
				CHARGE_ICON_ENABLE(AHC_TRUE);
			}
		break;

		case AHC_HDMI_MOVIE_PB_STATUS:
		#if 0	
			if(!bHdmiMenuMode)
			{
				m_DelFile = DEL_FILE_NONE;
				MovPBTimer_Stop();
				m_bMovieOSDForceShowUp = AHC_FALSE;
				MovPlaybackParamReset();
				m_HdmiStatus = AHC_HDMI_NONE_STATUS;
				HDMIFunc_Enable(AHC_TRUE);
			}
		#else
			if(!bHdmiMenuMode)
			{
				if(m_DelFile==DEL_FILE_SELECT_NO) {
					m_DelFile = DEL_FILE_SELECT_YES;
					DrawStateHdmiMoviePBUpdate(EVENT_KEY_DOWN);
				}
				else if(m_DelFile == DEL_FILE_NONE){
					HDMIFunc_MoviePBBackward();
                    DrawStateHdmiMoviePBUpdate(ulJobEvent);
				}
			}
		#endif
		break;

		case AHC_HDMI_PHOTO_PB_STATUS:
			if(!bHdmiMenuMode)
			{
				m_DelFile = DEL_FILE_NONE;
				PhotoPBTimer_Stop();
				JpgPlaybackParamReset();
				m_HdmiStatus = AHC_HDMI_NONE_STATUS;
				HDMIFunc_Enable(AHC_TRUE);
			}
		break;

		case AHC_HDMI_SLIDE_SHOW_STATUS:
			if(!bHdmiMenuMode)
			{
				SlideShowTimer_Stop();

				if(PLAYBACK_IMAGE_TYPE == GetPlayBackFileType())
				{
					JpgPlaybackParamReset();
					SetCurrentOpMode(JPGPB_MODE);
					AHC_UF_SetFreeChar(0, DCF_SET_ALLOWED, (UINT8 *) SEARCH_PHOTO);  
				}
				else
				{
					MovPlaybackParamReset();
	                SetCurrentOpMode(MOVPB_MODE);
	                AHC_UF_SetFreeChar(0, DCF_SET_ALLOWED, (UINT8 *) SEARCH_MOVIE);  
				}

				m_HdmiStatus = AHC_HDMI_NONE_STATUS;
				HDMIFunc_Enable(AHC_TRUE);
			}
		break;
	}
}

void StateHDMI_EventDown_Handler(UINT32 ulJobEvent)
{
	UINT32 			uState;
	UINT32 			CurObjIdx;

	if(ulJobEvent==EVENT_KEY_WIDE_STOP)
	{
		if(bHdmiMenuMode)
     		return;
     	else if(m_HdmiStatus!=AHC_HDMI_VIDEO_PREVIEW_STATUS &&
     			m_HdmiStatus!=AHC_HDMI_DSC_PREVIEW_STATUS)
     		return;
    }

	if(bHdmiMenuMode)
	{
		SetHdmiMenuHandler(ulJobEvent);
    	return;
	}

	switch(m_HdmiStatus)
	{
#if (HDMI_PREVIEW_EN)
		case AHC_HDMI_VIDEO_PREVIEW_STATUS:
		case AHC_HDMI_DSC_PREVIEW_STATUS:
			if(!bHdmiMenuMode)
			{
				//Zoom Operation
				if(ulJobEvent==EVENT_KEY_DOWN)
					StateCaptureMode(EVENT_KEY_WIDE_PRESS);
				else if(ulJobEvent==EVENT_KEY_TELE_STOP)
					StateCaptureMode(EVENT_KEY_WIDE_STOP);
			}
		break;
#endif
		case AHC_HDMI_BROWSER_STATUS:
	        if(!bHdmiMenuMode)
			{
				CHARGE_ICON_ENABLE(AHC_FALSE);
				HDMIFunc_ThumbnailOption(BROWSER_DOWN);
				DrawStateHdmiBrowserUpdate(ulJobEvent);
				CHARGE_ICON_ENABLE(AHC_TRUE);
			}
		break;

		case AHC_HDMI_MOVIE_PB_STATUS:
#if 0
	        if(!bHdmiMenuMode)
	        {
				AHC_GetSystemStatus(&uState);

				if(	uState == STATE_MOV_PAUSE ||
					uState == STATE_MOV_IDLE  ||
					uState == STATE_MOV_LOADED)
				{
					if(m_DelFile == DEL_FILE_NONE)
					{
						if(!AHC_SDMMC_BasicCheck())
                     		return;

						AHC_UF_GetCurrentIndex(&CurObjIdx);

                         if(AHC_IsFileProtect(CurObjIdx))
                         {
                            AHC_WMSG_Draw(AHC_TRUE, WMSG_CANNOT_DELETE, 3);
                         }
                         else
                         {
							m_DelFile = DEL_FILE_SELECT_NO;
                            DrawStateHdmiMoviePBUpdate(EVENT_KEY_DOWN);
                         }
					}
				}
			}
#else
			if(!bHdmiMenuMode)
			{
				if(m_DelFile==DEL_FILE_SELECT_YES) {
					m_DelFile = DEL_FILE_SELECT_NO;
					DrawStateHdmiMoviePBUpdate(EVENT_KEY_DOWN);
				}
				else if(m_DelFile == DEL_FILE_NONE){
					HDMIFunc_MoviePBForward();
                    DrawStateHdmiMoviePBUpdate(ulJobEvent);
				}
			}
#endif
		break;

		case AHC_HDMI_PHOTO_PB_STATUS:

			if(!bHdmiMenuMode)
			{
				if(m_DelFile == DEL_FILE_NONE)
				{
					if(!AHC_SDMMC_BasicCheck())
						return;

					AHC_UF_GetCurrentIndex(&CurObjIdx);

					if(AHC_IsFileProtect(CurObjIdx))
					{
						AHC_WMSG_Draw(AHC_TRUE, WMSG_CANNOT_DELETE, 3);
					}
					else
					{
						m_DelFile = DEL_FILE_SELECT_NO;
						DrawStateHdmiPhotoPBUpdate(EVENT_KEY_DOWN);
					}
				}
			}
		break;
	}
}

void StateHDMI_EventLeft_Handler(UINT32 ulJobEvent)
{
	if(bHdmiMenuMode)
	{
		if(SetHdmiMenuHandler(ulJobEvent) == 0)
    		return;
	}

	switch(m_HdmiStatus)
	{
#if (HDMI_PREVIEW_EN)
		case AHC_HDMI_VIDEO_PREVIEW_STATUS:
		case AHC_HDMI_DSC_PREVIEW_STATUS:
			if(!bHdmiMenuMode)
				AHC_ChangeEV(AHC_TRUE, NULL);
		break;
#endif
		case AHC_HDMI_BROWSER_STATUS:
			if(!bHdmiMenuMode)
			{
				if(Deleting_InBrowser || Protecting_InBrowser)
				{
					BrowserFunc_ThumbnailEditFile_Option(ulJobEvent);
				}
				else
				{
					CHARGE_ICON_ENABLE(AHC_FALSE);
					HDMIFunc_ThumbnailOption(BROWSER_LEFT);
					DrawStateHdmiBrowserUpdate(ulJobEvent);
					CHARGE_ICON_ENABLE(AHC_TRUE);
				}
			}
		break;

		case AHC_HDMI_MOVIE_PB_STATUS:
			if(!bHdmiMenuMode)
			{
				if(m_DelFile==DEL_FILE_SELECT_NO) {
					m_DelFile = DEL_FILE_SELECT_YES;
					DrawStateHdmiMoviePBUpdate(EVENT_KEY_DOWN);
				}
				else if(m_DelFile == DEL_FILE_NONE){
					HDMIFunc_MoviePBBackward();
                    DrawStateHdmiMoviePBUpdate(ulJobEvent);
				}
			}
		break;

		case AHC_HDMI_PHOTO_PB_STATUS:
			if(!bHdmiMenuMode)
			{
				if(m_DelFile==DEL_FILE_SELECT_NO) {
					m_DelFile = DEL_FILE_SELECT_YES;
					DrawStateHdmiPhotoPBUpdate(EVENT_KEY_DOWN);
				}
				else if(m_DelFile == DEL_FILE_NONE){
					if(HDMIFunc_PhotoPBPrevious())
						DrawStateHdmiPhotoPBUpdate(ulJobEvent);
				}
			}
		break;

		case AHC_HDMI_SLIDE_SHOW_STATUS:
			if(!bHdmiMenuMode)
			{
				if(	PLAYBACK_VIDEO_TYPE == GetPlayBackFileType() ||
					PLAYBACK_AUDIO_TYPE == GetPlayBackFileType())
				{
					MovPlayback_Pause();
				}
				MediaPlaybackConfig(PLAYBACK_PREVIOUS);
				SlideShowFunc_Play();
				DrawStateHdmiSlideShowUpdate(EVENT_SLIDESHOW_UPDATE_FILE);
			}
		break;
	}
}

void StateHDMI_EventRight_Handler(UINT32 ulJobEvent)
{
	if(bHdmiMenuMode)
	{
		if(SetHdmiMenuHandler(ulJobEvent) == 0)
    		return;
	}

	switch(m_HdmiStatus)
	{
#if (HDMI_PREVIEW_EN)
		case AHC_HDMI_VIDEO_PREVIEW_STATUS:
		case AHC_HDMI_DSC_PREVIEW_STATUS:
			if(!bHdmiMenuMode)
				AHC_ToggleFlashLED(LED_MODE_AUTO_ON_OFF);
		break;
#endif
		case AHC_HDMI_BROWSER_STATUS:
			if(!bHdmiMenuMode)
			{
				if(Deleting_InBrowser || Protecting_InBrowser)
				{
					BrowserFunc_ThumbnailEditFile_Option(ulJobEvent);
				}
				else
				{
					CHARGE_ICON_ENABLE(AHC_FALSE);
					HDMIFunc_ThumbnailOption(BROWSER_RIGHT);
					DrawStateHdmiBrowserUpdate(ulJobEvent);
					CHARGE_ICON_ENABLE(AHC_TRUE);
				}
			}
		break;

		case AHC_HDMI_MOVIE_PB_STATUS:
			if(!bHdmiMenuMode)
			{
				if(m_DelFile==DEL_FILE_SELECT_YES) {
					m_DelFile = DEL_FILE_SELECT_NO;
					DrawStateHdmiMoviePBUpdate(EVENT_KEY_DOWN);
				}
				else if(m_DelFile == DEL_FILE_NONE){
					HDMIFunc_MoviePBForward();
                    DrawStateHdmiMoviePBUpdate(ulJobEvent);
				}
			}
		break;

		case AHC_HDMI_PHOTO_PB_STATUS:
			if(!bHdmiMenuMode)
			{
				if(m_DelFile==DEL_FILE_SELECT_YES) {
					m_DelFile = DEL_FILE_SELECT_NO;
					DrawStateHdmiPhotoPBUpdate(EVENT_KEY_DOWN);
				}
				else if(m_DelFile == DEL_FILE_NONE){
					if(HDMIFunc_PhotoPBNext())
						DrawStateHdmiPhotoPBUpdate(ulJobEvent);
				}
			}
		break;

		case AHC_HDMI_SLIDE_SHOW_STATUS:
			if(!bHdmiMenuMode)
			{
				if(	PLAYBACK_VIDEO_TYPE == GetPlayBackFileType() ||
					PLAYBACK_AUDIO_TYPE == GetPlayBackFileType())
				{
					MovPlayback_Pause();
				}
				MediaPlaybackConfig(PLAYBACK_NEXT);
                SlideShowFunc_Play();
                DrawStateHdmiSlideShowUpdate(EVENT_SLIDESHOW_UPDATE_FILE);
			}
		break;
	}
}

void StateHDMI_EventExit_Handler(UINT32 ulJobEvent)
{
	if(bHdmiMenuMode)
	{
		SetHdmiMenuHandler(ulJobEvent);
		return;
	}
	switch(m_HdmiStatus)
	{
#if (HDMI_PREVIEW_EN)
		case AHC_HDMI_VIDEO_PREVIEW_STATUS:
		case AHC_HDMI_DSC_PREVIEW_STATUS:
			//TBD
		break;
#endif

		case AHC_HDMI_MOVIE_PB_STATUS:
		{
			m_DelFile = DEL_FILE_NONE;
			MovPBTimer_Stop();
			m_bMovieOSDForceShowUp = AHC_FALSE;
			MovPlaybackParamReset();
			m_HdmiStatus = AHC_HDMI_NONE_STATUS;
			HDMIFunc_Enable(AHC_TRUE);
		}
		break;
	}

}

void StateHDMI_EventOK_Handler(UINT32 ulJobEvent)
{
	UINT32 			uState;

	if(bHdmiMenuMode)
	{
		SetHdmiMenuHandler(ulJobEvent);
		return;
	}

	switch(m_HdmiStatus)
	{
#if (HDMI_PREVIEW_EN)
		case AHC_HDMI_VIDEO_PREVIEW_STATUS:
		case AHC_HDMI_DSC_PREVIEW_STATUS:
			//TBD
		break;
#endif
		case AHC_HDMI_BROWSER_STATUS:
			if(!bHdmiMenuMode)
			{
				if(Deleting_InBrowser || Protecting_InBrowser)
				{
					BrowserFunc_ThumbnailEditFile_Option(ulJobEvent);
				}
				else
				{
					HDMIFunc_ThumbnailOption(BROWSER_PLAY);
				}
			}
		break;

		case AHC_HDMI_MOVIE_PB_STATUS:

			if(!bHdmiMenuMode)
			{
				if(m_DelFile!=DEL_FILE_NONE)
				{
					DrawStateHdmiMoviePBUpdate(EVENT_HDMI_BROWSER_KEY_PLAYBACK);

					if(m_DelFile == DEL_FILE_SELECT_YES)
					{
						if(!HDMIFunc_MoviePBDelFile())
						{
							MovPBTimer_Stop();
	                        m_HdmiStatus = AHC_HDMI_NONE_STATUS;
	                        HDMIFunc_Enable(AHC_TRUE);
	                        break;
						}

						m_DelFile = DEL_FILE_NONE;
	                    DrawStateHdmiMoviePBUpdate(EVENT_KEY_DOWN);
	                    DrawStateHdmiMoviePBUpdate(EVENT_HDMI_DEL_FILE);
					}
	                else if(m_DelFile == DEL_FILE_SELECT_NO)
	                {
	                 	m_DelFile = DEL_FILE_NONE;
	                    DrawStateHdmiMoviePBUpdate(EVENT_KEY_DOWN);
	                    DrawStateHdmiMoviePBUpdate(EVENT_MOVPB_TOUCH_PAUSE_PRESS);
	                }
				}
				else
				{
					HDMIFunc_MoviePBPlayPress();
				}
			}
		break;

		case AHC_HDMI_PHOTO_PB_STATUS:

			if(!bHdmiMenuMode)
			{
				if(m_DelFile!=DEL_FILE_NONE)
				{
					DrawStateHdmiPhotoPBUpdate(EVENT_HDMI_BROWSER_KEY_PLAYBACK);

					if(m_DelFile == DEL_FILE_SELECT_YES)
					{
						if(!HDMIFunc_PhotoPBDelFile())
						{
							PhotoPBTimer_Stop();
	                        m_HdmiStatus = AHC_HDMI_NONE_STATUS;
	                        HDMIFunc_Enable(AHC_TRUE);
	                        break;
						}
					}

					m_DelFile = DEL_FILE_NONE;
					DrawStateHdmiPhotoPBUpdate(EVENT_KEY_DOWN);
					DrawStateHdmiPhotoPBUpdate(EVENT_HDMI_DEL_FILE);
				}
				else
				{
					PhotoPBTimer_Stop();
					m_HdmiStatus = AHC_HDMI_NONE_STATUS;
					HDMIFunc_Enable(AHC_TRUE);
				}
			}
		break;

		case AHC_HDMI_SLIDE_SHOW_STATUS:

			if(!bHdmiMenuMode)
			{
				if(PLAYBACK_IMAGE_TYPE == GetPlayBackFileType())
				{
					bSlideShow = (bSlideShow)?(AHC_FALSE):(AHC_TRUE);
				}
				else if((PLAYBACK_VIDEO_TYPE == GetPlayBackFileType()) || (PLAYBACK_AUDIO_TYPE == GetPlayBackFileType()))
				{
					AHC_GetSystemStatus(&uState);

					switch(uState)
					{
						case STATE_MOV_IDLE    :
							MediaPlaybackConfig(PLAYBACK_CURRENT);

							if(!MovPlayback_Play())
							{
								MovPBTimer_Stop();
                                m_HdmiStatus = AHC_HDMI_NONE_STATUS;
                                HDMIFunc_Enable(AHC_TRUE);
                                AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);
                                return;
                             }
                             DrawStateHdmiMoviePBUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);
						break;

						case STATE_MOV_EXECUTE :
							if(MOV_STATE_FF == GetMovConfig()->iState)
							{
								MovPlayback_NormalSpeed();
								DrawStateHdmiMoviePBUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);
							}
							else
							{
								MovPlayback_Pause();
								DrawStateHdmiMoviePBUpdate( EVENT_MOVPB_TOUCH_PAUSE_PRESS );
							}
						break;

						case STATE_MOV_PAUSE   :
							AHC_PlaybackClipCmd(AHC_PB_MOVIE_NORMAL_PLAY_SPEED, 0);
							MovPlayback_Resume();
							DrawStateHdmiMoviePBUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);
						break;

						default                :
						break;
					}
				}
			}
		break;
	}
}

void StateHDMI_EventMode_Handler(UINT32 ulJobEvent)
{
	if(bHdmiMenuMode)
		return;
	else if(VideoFunc_RecordStatus())
		return;

	switch(m_HdmiStatus)
	{
#if (HDMI_PREVIEW_EN)
		case AHC_HDMI_VIDEO_PREVIEW_STATUS:
			#if (DSC_MODE_ENABLE==0)
				VideoTimer_Stop();
				bPreviewModeHDMI = AHC_FALSE;
				m_HdmiStatus = AHC_HDMI_NONE_STATUS;
				#if (ALL_TYPE_FILE_BROWSER_PLAYBACK )
	            SetCurrentOpMode(JPGPB_MOVPB_MODE);
				#else
				SetCurrentOpMode(MOVPB_MODE);
				#endif
				HDMIFunc_Enable(AHC_TRUE);
			#else
				VideoTimer_Stop();
				HDMIFunc_PreviewCamera();
			#endif
		break;

		case AHC_HDMI_DSC_PREVIEW_STATUS:
			CaptureTimer_Stop();
			bPreviewModeHDMI = AHC_FALSE;
			m_HdmiStatus = AHC_HDMI_NONE_STATUS;
			#if (ALL_TYPE_FILE_BROWSER_PLAYBACK )
            SetCurrentOpMode(JPGPB_MOVPB_MODE);
			#else
			SetCurrentOpMode(MOVPB_MODE);
			#endif
			HDMIFunc_Enable(AHC_TRUE);
		break;

		case AHC_HDMI_BROWSER_STATUS:

			if(AHC_FALSE == AHC_SDMMC_GetMountState())
			{
				HDMIFunc_PreviewVideo();
			}
			else
			{
				#if (DSC_MODE_ENABLE==0)
				HDMIFunc_PreviewVideo();
				#else //(DSC_MODE_ENABLE==0)
				if(MOVPB_MODE == GetCurrentOpMode())
				{
					bPreviewModeHDMI = AHC_FALSE;
					HDMIFunc_ThumbnailModeSwitch();
					DrawStateHdmiBrowserUpdate(ulJobEvent);
				}
				else if(JPGPB_MODE == GetCurrentOpMode())
				{
					HDMIFunc_PreviewVideo();
				}
				else if(JPGPB_MOVPB_MODE == GetCurrentOpMode())
				{
					HDMIFunc_PreviewVideo();
				}
				#endif	// (DSC_MODE_ENABLE==0)
			}
		break;
#else
		case AHC_HDMI_BROWSER_STATUS:

			if(AHC_FALSE == AHC_SDMMC_GetMountState())
				break;

			HDMIFunc_ThumbnailModeSwitch();
	 		DrawStateHdmiBrowserUpdate(ulJobEvent);
		break;
#endif
		case AHC_HDMI_MOVIE_PB_STATUS:

			if(AHC_FALSE == AHC_SDMMC_GetMountState())
				break;

			m_DelFile = DEL_FILE_NONE;
			MovPBTimer_Stop();
			m_bMovieOSDForceShowUp = AHC_FALSE;
			MovPlaybackParamReset();

			#if (DIR_KEY_TYPE==KEY_TYPE_4KEY)
			if(!HDMIFunc_MoviePBModeSwitch())
			#endif
			{
				m_HdmiStatus = AHC_HDMI_NONE_STATUS;
				HDMIFunc_Enable(AHC_TRUE);
			}
		break;

		case AHC_HDMI_PHOTO_PB_STATUS:

			if(AHC_FALSE == AHC_SDMMC_GetMountState())
				break;

			m_DelFile = DEL_FILE_NONE;
			PhotoPBTimer_Stop();

			#if (DIR_KEY_TYPE==KEY_TYPE_4KEY)
			if(!HDMIFunc_PhotoPBModeSwitch())
			#endif
			{
				m_HdmiStatus = AHC_HDMI_NONE_STATUS;
				HDMIFunc_Enable(AHC_TRUE);
			}
		break;
		
		case AHC_HDMI_SLIDE_SHOW_STATUS:

			if(AHC_FALSE == AHC_SDMMC_GetMountState())
				break;

			m_DelFile = DEL_FILE_NONE;
			SlideShowTimer_Stop();

			
			m_HdmiStatus = AHC_HDMI_NONE_STATUS;
			HDMIFunc_Enable(AHC_TRUE);
			
		break;
	}
}

void StateHDMI_EventMovPBMode_Handler(UINT32 ulJobEvent)
{
	if(bHdmiMenuMode)
		return;
	else if(VideoFunc_RecordStatus())
		return;

	switch(m_HdmiStatus)
	{

		case AHC_HDMI_BROWSER_STATUS:

			if(AHC_FALSE == AHC_SDMMC_GetMountState())
				break;
            /*
			HDMIFunc_ThumbnailModeSwitch();
	 		DrawStateHdmiBrowserUpdate(ulJobEvent);
	 		*/
	 		if( GetCurrentOpMode() == MOVPB_MODE )
            {
                m_ubPlaybackRearCam = (m_ubPlaybackRearCam == AHC_TRUE)? AHC_FALSE : AHC_TRUE;
                DrawBrowserStatePlaybackRearCam( m_ubPlaybackRearCam );

				#if VIDRECD_MULTI_TRACK
				if( m_ubPlaybackRearCam == AHC_TRUE )
				{
					MMPS_VIDPLAY_SetCurrentTrack(1);  //playback rear cam.
				}
				else
				{
					MMPS_VIDPLAY_SetCurrentTrack(0);  //playback front cam.
				}

				AHC_Thumb_DrawPage(AHC_TRUE);
				#endif
            }
		break;

		case AHC_HDMI_MOVIE_PB_STATUS:

			if(AHC_FALSE == AHC_SDMMC_GetMountState())
				break;
            /*
			m_DelFile = DEL_FILE_NONE;
			MovPBTimer_Stop();
			m_bMovieOSDForceShowUp = AHC_FALSE;
			MovPlaybackParamReset();

			#if (DIR_KEY_TYPE==KEY_TYPE_4KEY)
			if(!HDMIFunc_MoviePBModeSwitch())
			#endif
			{
				m_HdmiStatus = AHC_HDMI_NONE_STATUS;
				HDMIFunc_Enable(AHC_TRUE);
			}
			*/
			#if VIDRECD_MULTI_TRACK
			m_ubPlaybackRearCam = (m_ubPlaybackRearCam == AHC_TRUE)? AHC_FALSE : AHC_TRUE;
			AHC_GetVideoCurrentPlayTime(&m_VideoCurPlayTime);
			MovPlaybackParamReset();
			AHC_SetMode(AHC_MODE_IDLE);
			if( m_ubPlaybackRearCam == AHC_TRUE )
	        {
	            MMPS_VIDPLAY_SetCurrentTrack(1);  //playback rear cam.
	        }
	        else
	        {
	            MMPS_VIDPLAY_SetCurrentTrack(0);  //playback front cam.
	        }	
			AHC_SetVideoPlayStartTime(m_VideoCurPlayTime);
			MovPlayback_Play();
			#endif
		break;

		case AHC_HDMI_PHOTO_PB_STATUS:

		break;
		
		case AHC_HDMI_SLIDE_SHOW_STATUS:

			if(AHC_FALSE == AHC_SDMMC_GetMountState())
				break;
            /*
			m_DelFile = DEL_FILE_NONE;
			SlideShowTimer_Stop();

			
			m_HdmiStatus = AHC_HDMI_NONE_STATUS;
			HDMIFunc_Enable(AHC_TRUE);
			*/
		break;
	}
}


void StateHDMI_EventRec_Handler(UINT32 ulJobEvent)
{
#if (HDMI_PREVIEW_EN)
	UINT8 ubID = HDMIFunc_GetUImodeOsdID();
#endif

	if(bHdmiMenuMode)
		return;

	switch(m_HdmiStatus)
	{
#if (HDMI_PREVIEW_EN)
		case AHC_HDMI_VIDEO_PREVIEW_STATUS:

			if(VideoFunc_RecordStatus())
			{
			    StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
				UIDrawHdmi_VideoFuncEx(ubID, HDMI_GUI_VIDEO_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
			}
			else
			{
				printc("Not Supported Record!!!!\r\n");
				break;

				if(AHC_SDMMC_BasicCheck()==AHC_FALSE)
					break;

				RecordTimeOffset = 0;

				if(VideoFunc_Record()==VIDEO_REC_START)
				{
					UpdateVideoRecordStatus(VIDEO_REC_START);
					UIDrawHdmi_VideoFuncEx(ubID, HDMI_GUI_VIDEO_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
				}
			}
		break;

		case AHC_HDMI_DSC_PREVIEW_STATUS:
		{
			UINT64   ulAvailbleSize = 0;

			if(AHC_SDMMC_BasicCheck()==AHC_FALSE)
				break;

			if(!CaptureFunc_CheckMemSizeAvailable(&ulAvailbleSize, &RemainCaptureNum))
				return;

			if(CaptureFunc_CheckSelfTimer())
			    return;

			CaptureFunc_Shutter();

			CaptureFunc_CheckMemSizeAvailable(&ulAvailbleSize, &RemainCaptureNum);
		}
		break;
#endif

#if (SLIDESHOW_EN)
		case AHC_HDMI_BROWSER_STATUS:
			if(HDMIFunc_SlideShow())
			{
				bSlideShow 	= AHC_TRUE;
        		UpdateSlideMovPBStatus(MOVIE_STATE_PLAY);
			}
		break;

		case AHC_HDMI_SLIDE_SHOW_STATUS:

			SlideShowTimer_Stop();
			AHC_UF_GetCurrentIndex(&ObjSelect);

			if(PLAYBACK_IMAGE_TYPE == GetPlayBackFileType())
			{
				JpgPlaybackParamReset();
	            SetCurrentOpMode(JPGPB_MODE);
	            AHC_UF_SetFreeChar(0, DCF_SET_ALLOWED, (UINT8*)SEARCH_PHOTO);  
			}
			else
			{
	        	MovPlaybackParamReset();
	            SetCurrentOpMode(MOVPB_MODE);
	            AHC_UF_SetFreeChar(0, DCF_SET_ALLOWED, (UINT8*)SEARCH_MOVIE);  
			}
			m_HdmiStatus = AHC_HDMI_NONE_STATUS;
			HDMIFunc_Enable(AHC_TRUE);
		break;
#endif
		default:
		break;
	}
}
#endif

void StateHDMI_EventMenu_Handler(UINT32 ulJobEvent)
{
#if (HDMI_MENU_EN)

	PSMENUSTRUCT 	pMenu = GetCurrentMenu();
#if (HDMI_PREVIEW_EN)
	AHC_BOOL 		ubExitMenu = AHC_FALSE;
#endif

	switch(m_HdmiStatus)
	{
#if (HDMI_PREVIEW_EN)
		case AHC_HDMI_VIDEO_PREVIEW_STATUS:

			if(VideoFunc_RecordStatus())
				break;

			if(bHdmiMenuMode)
			{
				#if (TOP_MENU_PAGE_EN)
				if(IsCurrentTopMenu(pMenu))
					ubExitMenu = AHC_TRUE;
				#else
				if(IsCurrentMainMenu(pMenu))
					ubExitMenu = AHC_TRUE;
				#endif

				if(ubExitMenu)
				{
				        UINT8 bID0,bID1;
					
				        OSDDraw_EnterDrawing(&bID0, &bID1 );
				        printc(FG_GREEN("Exit Menu - %d\r\n"), bID0);
                	    ImportMenuInfo(MenuSettingConfig());    // update menu item from data structure to atom
                	    //if HDMI_SYSTEM change, redraw OSD and init
                        if(HDMIFunc_CheckHDMISystemChanged()) {
                           HDMIFunc_PreviewVideo();
                        }                   

					if(VideoRecSize != MenuSettingConfig()->uiMOVSize)
					{
						AHC_SetMode(AHC_MODE_IDLE);
						VideoFunc_Preview();
					}

		            ResetCurrentMenu();
					Menu_WriteSetting();

					DrawStateHdmiPreviewVideoInit(bID0);
					AHC_OSDSetActive(HDMIFunc_GetMenuOsdID(), 0);
					Scale_HDMIMenuOSD_Window(MMP_DISPLAY_DUPLICATE_1X);
					AHC_OSDSetActive(bID0, 1);
					AHC_OSDSetCurrentDisplay(bID0);
                    OSDDraw_ExitDrawing(&bID0, &bID1 );
					AHC_OSDRefresh();
					bHdmiMenuMode = AHC_FALSE;
				}
				else
				{
					if(IsCurrentMainMenu(pMenu))
						MainMenuEventHandler(pMenu, MENU_MENU, 0);
					else
						SubMenuEventHandler(pMenu, MENU_MENU, 0);
				}
			}
			else
			{
				#if (TOP_MENU_PAGE_EN)
				SetCurrentMenu(&sTopMenu);
				if((MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_1080I)||(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_1080P))
				{
					Scale_HDMIMenuOSD_Window(OSD_DISPLAY_SCALE_HDMI_FHD - 1);
				}
				else if(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_720P)
				{
					Scale_HDMIMenuOSD_Window(OSD_DISPLAY_SCALE_HDMI_HD - 1);
				}
				else if(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_480P)
				{
					Scale_HDMIMenuOSD_Window(OSD_DISPLAY_SCALE_TV - 1);
				}					
				TopMenuEventHandler(&sTopMenu, MENU_INIT, 0);
				#else
				SetCurrentMenu(&sMainMenuVideo);
				if((MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_1080I)||(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_1080P))
				{
					Scale_HDMIMenuOSD_Window(OSD_DISPLAY_SCALE_HDMI_FHD - 1);
				}
				else if(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_720P)
				{
					Scale_HDMIMenuOSD_Window(OSD_DISPLAY_SCALE_HDMI_HD - 1);
				}
				else if(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_480P)
				{
					Scale_HDMIMenuOSD_Window(OSD_DISPLAY_SCALE_TV - 1);
				}				
				MainMenuEventHandler(&sMainMenuVideo, MENU_INIT, 0);
				#endif

				AHC_OSDSetActive(HDMI_UI_OSD_ID, 0);
				AHC_OSDSetActive(HDMI_MENU_OSD_ID, 1);
				bHdmiMenuMode = AHC_TRUE;
			}
		break;

		case AHC_HDMI_DSC_PREVIEW_STATUS:

			if(bHdmiMenuMode)
			{
				#if (TOP_MENU_PAGE_EN)
				if(IsCurrentTopMenu(pMenu))
					ubExitMenu = AHC_TRUE;
				#else
				if(IsCurrentMainMenu(pMenu))
					ubExitMenu = AHC_TRUE;
				#endif

				if(ubExitMenu)
				{
					UINT8 bID0,bID1;
		
					OSDDraw_EnterDrawing(&bID0, &bID1 );
					printc(FG_GREEN("Exit Menu - %d\r\n"), bID0);
					ImportMenuInfo(MenuSettingConfig());    // update menu item from data structure to atom
					//if HDMI_SYSTEM change, redraw OSD and init
					if(HDMIFunc_CheckHDMISystemChanged()) {
						HDMIFunc_PreviewCamera();
					}  
					if(CaptureResolution != MenuSettingConfig()->uiIMGSize)
					{
       	 				AHC_SetMode(AHC_MODE_IDLE);
        				CaptureFunc_Preview();
					}

		         	ResetCurrentMenu();
					Menu_WriteSetting();

					DrawStateHdmiPreviewCameraInit(bID0);
					AHC_OSDSetActive(HDMIFunc_GetMenuOsdID(), 0);
					Scale_HDMIMenuOSD_Window(MMP_DISPLAY_DUPLICATE_1X);
					AHC_OSDSetActive(bID0, 1);
					AHC_OSDSetCurrentDisplay(bID0);
					OSDDraw_ExitDrawing(&bID0, &bID1 );
					AHC_OSDRefresh();
					bHdmiMenuMode = AHC_FALSE;
				}
				else
				{
					if(IsCurrentMainMenu(pMenu))
						MainMenuEventHandler(pMenu, MENU_MENU, 0);
					else
						SubMenuEventHandler(pMenu, MENU_MENU, 0);
				}
			}
			else
			{
				#if (TOP_MENU_PAGE_EN)
				SetCurrentMenu(&sTopMenu);
				if((MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_1080I)||(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_1080P))
				{
					Scale_HDMIMenuOSD_Window(OSD_DISPLAY_SCALE_HDMI_FHD - 1);
				}
				else if(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_720P)
				{
					Scale_HDMIMenuOSD_Window(OSD_DISPLAY_SCALE_HDMI_HD - 1);
				}
				else if(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_480P)
				{
					Scale_HDMIMenuOSD_Window(OSD_DISPLAY_SCALE_TV - 1);
				}					
				TopMenuEventHandler(&sTopMenu, MENU_INIT, 0);
				#else
				SetCurrentMenu(&sMainMenuStill);
				if((MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_1080I)||(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_1080P))
				{
					Scale_HDMIMenuOSD_Window(OSD_DISPLAY_SCALE_HDMI_FHD - 1);
				}
				else if(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_720P)
				{
					Scale_HDMIMenuOSD_Window(OSD_DISPLAY_SCALE_HDMI_HD - 1);
				}
				else if(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_480P)
				{
					Scale_HDMIMenuOSD_Window(OSD_DISPLAY_SCALE_TV - 1);
				}					
				MainMenuEventHandler(&sMainMenuStill, MENU_INIT, 0);
				#endif

				AHC_OSDSetActive(HDMI_UI_OSD_ID, 0);
				AHC_OSDSetActive(HDMI_MENU_OSD_ID, 1);
				bHdmiMenuMode = AHC_TRUE;
			}
		break;
#endif
		case AHC_HDMI_BROWSER_STATUS:
			if(bHdmiMenuMode)
			{
				if(IsCurrentMainMenu(pMenu))
				{
					ResetCurrentMenu();
					Menu_WriteSetting();

					DrawStateHdmiBrowserInit();
					AHC_OSDSetActive(HDMI_MENU_OSD_ID, 0);
					Scale_HDMIMenuOSD_Window(MMP_DISPLAY_DUPLICATE_1X);
					AHC_OSDSetActive(HDMI_UI_OSD_ID, 1);
					AHC_OSDSetCurrentDisplay(HDMI_UI_OSD_ID);
					AHC_OSDRefresh();
					bHdmiMenuMode = AHC_FALSE;
					
					if(bForceSwitchBrowser) //redraw browser 
					{
						m_HdmiStatus = AHC_HDMI_NONE_STATUS;
						HDMIFunc_Enable(AHC_TRUE);
						bForceSwitchBrowser = AHC_FALSE;
					}
				}
				else
				{
					SubMenuEventHandler(pMenu, MENU_MENU, 0);
				}
			}
			else
			{
				AHC_OSDClearBuffer(HDMI_MENU_OSD_ID);

				SetCurrentMenu(&sMainMenuPlayback);

				if((MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_1080I)||(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_1080P))
				{
					Scale_HDMIMenuOSD_Window(OSD_DISPLAY_SCALE_HDMI_FHD - 1);
				}
				else if(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_720P)
				{
					Scale_HDMIMenuOSD_Window(OSD_DISPLAY_SCALE_HDMI_HD - 1);
				}
				else if(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_480P)
				{
					Scale_HDMIMenuOSD_Window(OSD_DISPLAY_SCALE_TV - 1);
				}

				MainMenuEventHandler(&sMainMenuPlayback, MENU_INIT, 0);

				AHC_OSDSetActive(HDMI_UI_OSD_ID, 0);
				AHC_OSDSetActive(HDMI_MENU_OSD_ID, 1);
				bHdmiMenuMode = AHC_TRUE;
			}
		break;

		case AHC_HDMI_MOVIE_PB_STATUS:
			if(bHdmiMenuMode)
			{
				if(IsCurrentMainMenu(pMenu))
				{
				    UINT8 bID0,bID1;
					if(GetMovConfig()->iState == MOV_STATE_PAUSE && bEnterMenuPause==AHC_TRUE)
				    {
				    	MovPlayback_Resume();
				    	bEnterMenuPause = AHC_FALSE;
				    }

					ResetCurrentMenu();
					Menu_WriteSetting();

                    OSDDraw_EnterDrawing(&bID0, &bID1);
					DrawStateHdmiMoviePBInit(bID0, bID1);
                    OSDDraw_ExitDrawing(&bID0, &bID1);
                                                  
					AHC_OSDSetActive(HDMI_MENU_OSD_ID, 0);
					Scale_HDMIMenuOSD_Window(MMP_DISPLAY_DUPLICATE_1X);
					AHC_OSDSetActive(HDMI_UI_OSD_ID, 1);
					AHC_OSDSetCurrentDisplay(HDMI_UI_OSD_ID);
					AHC_OSDRefresh();
					bHdmiMenuMode = AHC_FALSE;
				}
				else
				{
					SubMenuEventHandler(pMenu, MENU_MENU, 0);
				}
			}
			else
			{
				if(GetMovConfig()->iState == MOV_STATE_PLAY || GetMovConfig()->iState == MOV_STATE_FF)
	        	{
	        		MovPlayback_Pause();
	        		bEnterMenuPause = AHC_TRUE;
	        	}
	        	else if(GetMovConfig()->iState == MOV_STATE_PAUSE)
	        	{
	        		bEnterMenuPause = AHC_FALSE;
	        	}

				AHC_OSDClearBuffer(HDMI_MENU_OSD_ID);

				SetCurrentMenu(&sMainMenuPlayback);

				if((MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_1080I)||(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_1080P))
				{
					Scale_HDMIMenuOSD_Window(OSD_DISPLAY_SCALE_HDMI_FHD - 1);
				}
				else if(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_720P)
				{
					Scale_HDMIMenuOSD_Window(OSD_DISPLAY_SCALE_HDMI_HD - 1);
				}
				else if(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_480P)
				{
					Scale_HDMIMenuOSD_Window(OSD_DISPLAY_SCALE_TV - 1);
				}

				MainMenuEventHandler(&sMainMenuPlayback, MENU_INIT, 0);

				AHC_OSDSetActive(HDMI_UI_OSD_ID, 0);
				AHC_OSDSetActive(HDMI_MENU_OSD_ID, 1);
				bHdmiMenuMode = AHC_TRUE;
			}
		break;

		case AHC_HDMI_PHOTO_PB_STATUS:
			if(bHdmiMenuMode)
			{
				if(IsCurrentMainMenu(pMenu))
				{
				    UINT8 bID0,bID1;
					ResetCurrentMenu();
					Menu_WriteSetting();

					OSDDraw_EnterDrawing(&bID0, &bID1);
					DrawStateHdmiPhotoPBInit(bID0, bID1);
                    OSDDraw_ExitDrawing(&bID0, &bID1);
                                                  
					AHC_OSDSetActive(HDMI_MENU_OSD_ID, 0);
					Scale_HDMIMenuOSD_Window(MMP_DISPLAY_DUPLICATE_1X);
					AHC_OSDSetActive(HDMI_UI_OSD_ID, 1);
					AHC_OSDSetCurrentDisplay(HDMI_UI_OSD_ID);
					AHC_OSDRefresh();
					bHdmiMenuMode = AHC_FALSE;
				}
				else
				{
					SubMenuEventHandler(pMenu, MENU_MENU, 0);
				}
			}
			else
			{
				AHC_OSDClearBuffer(HDMI_MENU_OSD_ID);

				SetCurrentMenu(&sMainMenuPlayback);

				if((MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_1080I)||(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_1080P))
				{
					Scale_HDMIMenuOSD_Window(OSD_DISPLAY_SCALE_HDMI_FHD - 1);
				}
				else if(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_720P)
				{
					Scale_HDMIMenuOSD_Window(OSD_DISPLAY_SCALE_HDMI_HD - 1);
				}
				else if(MenuSettingConfig()->uiHDMIOutput == HDMI_OUTPUT_480P)
				{
					Scale_HDMIMenuOSD_Window(OSD_DISPLAY_SCALE_TV - 1);
				}

				MainMenuEventHandler(&sMainMenuPlayback, MENU_INIT, 0);

				AHC_OSDSetActive(HDMI_UI_OSD_ID, 0);
				AHC_OSDSetActive(HDMI_MENU_OSD_ID, 1);
				bHdmiMenuMode = AHC_TRUE;
			}
		break;

		case AHC_HDMI_SLIDE_SHOW_STATUS:
			if(bHdmiMenuMode)
			{
				if(IsCurrentMainMenu(pMenu))
				{
					ResetCurrentMenu();
					Menu_WriteSetting();

					DrawStateHdmiSlideShowInit();
					AHC_OSDSetActive(HDMI_MENU_OSD_ID, 0);
					AHC_OSDSetActive(HDMI_UI_OSD_ID, 1);
					AHC_OSDSetCurrentDisplay(HDMI_UI_OSD_ID);
					AHC_OSDRefresh();
					bHdmiMenuMode = AHC_FALSE;
				}
				else
				{
					SubMenuEventHandler(pMenu, MENU_MENU, 0);
				}
			}
			else
			{
				AHC_OSDClearBuffer(HDMI_MENU_OSD_ID);

				SetCurrentMenu(&sMainMenuPlayback);
				MainMenuEventHandler(&sMainMenuPlayback, MENU_INIT, 0);

				AHC_OSDSetActive(HDMI_UI_OSD_ID, 0);
				AHC_OSDSetActive(HDMI_MENU_OSD_ID, 1);
				bHdmiMenuMode = AHC_TRUE;
			}
		break;

		default:
		break;
	}
#endif
}

#if (USE_NEW_HDMI_EVENT_HANDLER)

#if 0
void ____HDMI_Mode_Event_Function_____(){ruturn;} //dummy
#endif

void StateHDMI_VideoMode_Handler(UINT32 ulJobEvent)
{
#if (HDMI_PREVIEW_EN)

	UINT8 ubID = HDMIFunc_GetUImodeOsdID();

	if(m_HdmiStatus!=AHC_HDMI_VIDEO_PREVIEW_STATUS)
		return;

    if(bHdmiMenuMode)
    {
		SetHdmiMenuHandler(ulJobEvent);
    	return;
    }

	switch(ulJobEvent)
	{
        case EVENT_NONE:
        break;

		case EVENT_KEY_TELE_PRESS:
		case EVENT_KEY_WIDE_PRESS:
		case EVENT_KEY_WIDE_STOP:
		case EVENT_KEY_TELE_STOP:
			StateVideoRecMode(ulJobEvent);
		break;

		case EVENT_EV_INCREASE:
		case EVENT_EV_DECREASE:
		case EVENT_KEY_LEFT:
		case EVENT_KEY_RIGHT:
		case EVENT_KEY_SET:
			StateVideoRecMode(ulJobEvent);
		break;

		case EVENT_KEY_MENU:
			StateHDMI_EventMenu_Handler(ulJobEvent);
		break;
	#ifdef NET_SYNC_PLAYBACK_MODE
		case EVENT_NET_ENTER_PLAYBACK :
			if(VideoFunc_RecordStatus())
				StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
			VideoTimer_Stop();
			#if (VR_PREENCODE_EN)
			bDisableVideoPreRecord = AHC_TRUE;
			#endif
			m_HdmiStatus = AHC_HDMI_NET_PLAYBACK_STATUS;
			HDMIFunc_Enable(AHC_TRUE);
            CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
		break;
	#endif
		case EVENT_KEY_MODE:
			if(!VideoFunc_RecordStatus())
			{
				VideoTimer_Stop();

				#if (VR_PREENCODE_EN)
				bDisableVideoPreRecord = AHC_TRUE;
				#endif


				#if (DSC_MODE_ENABLE)
             	HDMIFunc_PreviewCamera();
				#else
				bPreviewModeHDMI = AHC_FALSE;
				m_HdmiStatus = AHC_HDMI_NONE_STATUS;
				SetCurrentOpMode(MOVPB_MODE);
				HDMIFunc_Enable(AHC_TRUE);
				#endif

				#if (LOCK_NEXTFILE_AT_VR_RESTART)
				m_ubLockNextFile = AHC_FALSE;
				#endif
			}
		break;

		case EVENT_VIDEO_KEY_RECORD:
		case EVENT_HDMI_BROWSER_KEY_PLAYBACK:
			#if defined(WIFI_PORT) && (WIFI_PORT == 1)
			break;
			#endif
			if(VideoFunc_RecordStatus())
			{
			    StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
				UIDrawHdmi_VideoFuncEx(ubID, HDMI_GUI_VIDEO_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
			}
			else
			{
				if(AHC_SDMMC_BasicCheck()==AHC_FALSE)
					break;

				RecordTimeOffset = 0;

				if(VideoFunc_Record()==VIDEO_REC_START)
				{
					UpdateVideoRecordStatus(VIDEO_REC_START);
					UIDrawHdmi_VideoFuncEx(ubID, HDMI_GUI_VIDEO_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
				}
			}
		break;

		case EVENT_KEY_PLAYBACK_MODE:
			if(!VideoFunc_RecordStatus())
            {
             	VideoTimer_Stop();

				#if (VR_PREENCODE_EN)
				bDisableVideoPreRecord = AHC_TRUE;
				#endif

				bPreviewModeHDMI = AHC_FALSE;
				m_HdmiStatus = AHC_HDMI_NONE_STATUS;
				SetCurrentOpMode(MOVPB_MODE);
				HDMIFunc_Enable(AHC_TRUE);
			}
		break;

		case EVENT_POWER_OFF:
			if( VideoFunc_RecordStatus() )
			{
				VideoFunc_RecordStop();

				UpdateVideoRecordStatus(VIDEO_REC_STOP);
				UIDrawHdmi_VideoFuncEx(ubID, HDMI_GUI_VIDEO_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);

				AHC_OS_Sleep(100);
			}

			#if (VR_PREENCODE_EN)
			bDisableVideoPreRecord = AHC_TRUE;
			AHC_SetMode(AHC_MODE_IDLE);
			#endif

            AHC_NormalPowerOffPath();
		break;

		//Specail Event
		case EVENT_CHANGE_LED_MODE:
		case EVENT_FORMAT_MEDIA:
		case EVENT_FORMAT_RESET_ALL:
		case EVENT_VIDEO_KEY_CAPTURE:
		case EVENT_LOCK_VR_FILE:
		case EVENT_RECORD_MUTE:
		case EVENT_CHANGE_NIGHT_MODE:
			StateVideoRecMode(ulJobEvent);
		break;

		case EVENT_CAMERA_BROWSER:
		case EVENT_VIDEO_BROWSER:
		case EVENT_ALL_BROWSER:
			if(!VideoFunc_RecordStatus())
			{
             	VideoTimer_Stop();

				#if (VR_PREENCODE_EN)
				bDisableVideoPreRecord = AHC_TRUE;
				#endif

				bPreviewModeHDMI = AHC_FALSE;
				m_HdmiStatus = AHC_HDMI_NONE_STATUS;

				if(ulJobEvent==EVENT_CAMERA_BROWSER)
					SetCurrentOpMode(JPGPB_MODE);
				else if(ulJobEvent==EVENT_VIDEO_BROWSER)
					SetCurrentOpMode(MOVPB_MODE);
				else if(ulJobEvent==EVENT_ALL_BROWSER)
					SetCurrentOpMode(JPGPB_MOVPB_MODE);

				HDMIFunc_Enable(AHC_TRUE);
			}
		break;

		case EVENT_CAMERA_PREVIEW:
			if(!VideoFunc_RecordStatus())
			{
				VideoTimer_Stop();

				#if (VR_PREENCODE_EN)
				bDisableVideoPreRecord = AHC_TRUE;
				#endif


             	HDMIFunc_PreviewCamera();

				#if (LOCK_NEXTFILE_AT_VR_RESTART)
				m_ubLockNextFile = AHC_FALSE;
				#endif
			}
		break;

		case EVENT_AUDIO_RECORD:
		case EVENT_SWITCH_VMD_MODE:
		case EVENT_SWITCH_PANEL_TVOUT:
			StateVideoRecMode(ulJobEvent);
		break;
#if (UVC_HOST_VIDEO_ENABLE)
		case EVENT_USB_B_DEVICE_DETECT:
		case EVENT_USB_B_DEVICE_REMOVED:
			StateVideoRecMode(ulJobEvent);			  //TBD
			break;
#endif
		//Record Callback
        case EVENT_VRCB_MEDIA_FULL	:
        case EVENT_VRCB_FILE_FULL	:
        case EVENT_VRCB_MEDIA_SLOW	:
        case EVENT_VRCB_SEAM_LESS	:
       	case EVENT_VRCB_MOTION_DTC	:
        case EVENT_VRCB_MEDIA_ERROR	:
        case EVENT_VR_START			:
        case EVENT_VR_STOP			:
       	case EVENT_VR_WRITEINFO		:
       		StateVideoRecMode(ulJobEvent);
       	break;

		//Update Message
		case EVENT_VIDREC_UPDATE_MESSAGE:
		case EVENT_SHOW_FW_VERSION:
		case EVENT_SHOW_GPS_INFO:	//TBD
		case EVENT_GPSGSENSOR_UPDATE:
			StateVideoRecMode(ulJobEvent);
		break;

		case EVENT_MOVPB_UPDATE_MESSAGE:
			StateVideoRecMode(EVENT_VIDREC_UPDATE_MESSAGE);
		break;	

		default:
		break;
	}
#endif
}

void StateHDMI_CameraMode_Handler(UINT32 ulJobEvent)
{
#if (HDMI_PREVIEW_EN) && (DSC_MODE_ENABLE==1)

	if(m_HdmiStatus!=AHC_HDMI_DSC_PREVIEW_STATUS)
		return;

    if(bHdmiMenuMode)
    {
		SetHdmiMenuHandler(ulJobEvent);
    	return;
    }

    switch(ulJobEvent)
    {
		case EVENT_NONE:
        break;

        case EVENT_KEY_TELE_PRESS:
        case EVENT_KEY_WIDE_PRESS:
        case EVENT_KEY_TELE_STOP:
        case EVENT_KEY_WIDE_STOP:
        	StateCaptureMode(ulJobEvent);
        break;

        case EVENT_KEY_LEFT:
        case EVENT_KEY_RIGHT:
		case EVENT_KEY_UP:
		case EVENT_KEY_DOWN:
        	StateCaptureMode(ulJobEvent);
        break;

        case EVENT_KEY_MENU:
        	StateHDMI_EventMenu_Handler(ulJobEvent);
        break;
	#ifdef NET_SYNC_PLAYBACK_MODE
		case EVENT_NET_ENTER_PLAYBACK :
			bPreviewModeHDMI = AHC_FALSE;
			CaptureTimer_Stop();
			m_HdmiStatus = AHC_HDMI_NET_PLAYBACK_STATUS;
			HDMIFunc_Enable(AHC_TRUE);
            CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
		break;
	#endif
        case EVENT_KEY_MODE:

#if (DSC_MULTI_MODE==DSC_MODE_ALL)
			if(m_ubDSCMode == DSC_ONE_SHOT)
				m_ubDSCMode	= DSC_BURST_SHOT;
			else if(m_ubDSCMode	== DSC_BURST_SHOT)
				m_ubDSCMode	= DSC_TIME_LAPSE;
			else
			{
				m_ubDSCMode = DSC_ONE_SHOT;

				m_ubTimeLapseStart = AHC_FALSE;

			}

			printc("Current m_ubDSCMode [%d]\r\n",m_ubDSCMode);
#else
			CaptureTimer_Stop();
			gsStillZoomIndex = ZoomCtrl_GetStillDigitalZoomLevel();

	#if (CYCLIC_MODE_CHANGE)
			bPreviewModeHDMI = AHC_FALSE;
			m_HdmiStatus = AHC_HDMI_NONE_STATUS;

			#if (ALL_TYPE_FILE_BROWSER_PLAYBACK )
            SetCurrentOpMode(JPGPB_MOVPB_MODE);
			#else
			SetCurrentOpMode(MOVPB_MODE);
			#endif
			
			#if (FOCUS_ON_LATEST_FILE)
    		AHC_SetPlayFileOrder(PLAY_LAST_FILE);
    		#endif
			HDMIFunc_Enable(AHC_TRUE);
	#else
			HDMIFunc_PreviewVideo();
	#endif

#endif
        break;

		case EVENT_DSC_KEY_CAPTURE:
		case EVENT_HDMI_BROWSER_KEY_PLAYBACK:
			StateCaptureMode(EVENT_DSC_KEY_CAPTURE);
		break;

        case EVENT_KEY_PLAYBACK_MODE:
        	CaptureTimer_Stop();
			gsStillZoomIndex = ZoomCtrl_GetStillDigitalZoomLevel();

			bPreviewModeHDMI = AHC_FALSE;
			m_HdmiStatus = AHC_HDMI_NONE_STATUS;

            #if ALL_TYPE_FILE_BROWSER_PLAYBACK
			SetCurrentOpMode(JPGPB_MOVPB_MODE);
			#else
			SetCurrentOpMode(JPGPB_MODE);
			#endif
			
			#if (FOCUS_ON_LATEST_FILE)
    		AHC_SetPlayFileOrder(PLAY_LAST_FILE);
    		#endif
			HDMIFunc_Enable(AHC_TRUE);
        break;

        case EVENT_POWER_OFF:
        	StateCaptureMode(ulJobEvent);
        break;

        //Special Event
        case EVENT_CHANGE_LED_MODE:
        case EVENT_CHANGE_NIGHT_MODE:
        	StateCaptureMode(ulJobEvent);
        break;

		case EVENT_CAMERA_BROWSER:
		case EVENT_VIDEO_BROWSER:
		case EVENT_ALL_BROWSER:	
         	CaptureTimer_Stop();
			gsStillZoomIndex = ZoomCtrl_GetStillDigitalZoomLevel();

			bPreviewModeHDMI = AHC_FALSE;
			m_HdmiStatus = AHC_HDMI_NONE_STATUS;

			if(ulJobEvent==EVENT_CAMERA_BROWSER)
				SetCurrentOpMode(JPGPB_MODE);
			else if(ulJobEvent==EVENT_VIDEO_BROWSER)
				SetCurrentOpMode(MOVPB_MODE);
			else if(ulJobEvent==EVENT_ALL_BROWSER)
				SetCurrentOpMode(JPGPB_MOVPB_MODE);

			#if (FOCUS_ON_LATEST_FILE)
    		AHC_SetPlayFileOrder(PLAY_LAST_FILE);
    		#endif
			HDMIFunc_Enable(AHC_TRUE);
        break;

		case EVENT_VIDEO_PREVIEW:
         	CaptureTimer_Stop();
			gsStillZoomIndex = ZoomCtrl_GetStillDigitalZoomLevel();
			HDMIFunc_PreviewVideo();
        break;

        case EVENT_SWITCH_VMD_MODE:
        case EVENT_SWITCH_TIME_LAPSE_EN:
        case EVENT_SWITCH_PANEL_TVOUT:
        	StateCaptureMode(ulJobEvent);
        break;

        //CallBack
        case EVENT_VRCB_MOTION_DTC:
        	StateCaptureMode(ulJobEvent);
        break;

        //Update Message
        case EVENT_DSC_KEY_UPDATE_MESSAGE:
        	StateCaptureMode(ulJobEvent);
        break;
		
		case EVENT_MOVPB_UPDATE_MESSAGE:
			StateCaptureMode(EVENT_DSC_KEY_UPDATE_MESSAGE);
		break;	

     	default:
     	break;
	}
#endif
}

void StateHDMI_BrowserMode_Handler(UINT32 ulJobEvent)
{
	if(m_HdmiStatus!=AHC_HDMI_BROWSER_STATUS)
		return;

    if(bHdmiMenuMode)
    {
		if(SetHdmiMenuHandler(ulJobEvent) == 0)
    		return;
    }

	switch(ulJobEvent)
	{
		case EVENT_NONE                           :
        break;

		case EVENT_KEY_UP:
			CHARGE_ICON_ENABLE(AHC_FALSE);
			HDMIFunc_ThumbnailOption(BROWSER_UP);
			DrawStateHdmiBrowserUpdate(ulJobEvent);
			CHARGE_ICON_ENABLE(AHC_TRUE);
		break;

		case EVENT_KEY_DOWN:
			CHARGE_ICON_ENABLE(AHC_FALSE);
			HDMIFunc_ThumbnailOption(BROWSER_DOWN);
			DrawStateHdmiBrowserUpdate(ulJobEvent);
			CHARGE_ICON_ENABLE(AHC_TRUE);
		break;

		case EVENT_KEY_LEFT:
			if(Deleting_InBrowser || Protecting_InBrowser)
			{
				BrowserFunc_ThumbnailEditFile_Option(ulJobEvent);
			}
			else
			{
				CHARGE_ICON_ENABLE(AHC_FALSE);
				HDMIFunc_ThumbnailOption(BROWSER_LEFT);
				DrawStateHdmiBrowserUpdate(ulJobEvent);
				CHARGE_ICON_ENABLE(AHC_TRUE);
			}
		break;

		case EVENT_KEY_RIGHT:
			if(Deleting_InBrowser || Protecting_InBrowser)
			{
				BrowserFunc_ThumbnailEditFile_Option(ulJobEvent);
			}
			else
			{
				CHARGE_ICON_ENABLE(AHC_FALSE);
				HDMIFunc_ThumbnailOption(BROWSER_RIGHT);
				DrawStateHdmiBrowserUpdate(ulJobEvent);
				CHARGE_ICON_ENABLE(AHC_TRUE);
			}
		break;

		case EVENT_THUMBNAIL_KEY_PLAYBACK:
		case EVENT_HDMI_BROWSER_KEY_PLAYBACK:	
			if(Deleting_InBrowser || Protecting_InBrowser)
			{
				BrowserFunc_ThumbnailEditFile_Option(ulJobEvent);
			}
			else
			{
				HDMIFunc_ThumbnailOption(BROWSER_PLAY);
			}
		break;

		case EVENT_KEY_MENU:
			StateHDMI_EventMenu_Handler(ulJobEvent);
		break;
	#ifdef NET_SYNC_PLAYBACK_MODE
		case EVENT_NET_ENTER_PLAYBACK :
			BrowserFunc_ThumbnailResetAllEditOp();
			m_HdmiStatus = AHC_HDMI_NET_PLAYBACK_STATUS;
			HDMIFunc_Enable(AHC_TRUE);
            CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
		break;
	#endif
		case EVENT_KEY_MODE:
			if(AHC_FALSE == AHC_SDMMC_GetMountState())
			{
				#if (HDMI_PREVIEW_EN)
				HDMIFunc_PreviewVideo();
				#endif
			}
			else
			{
			#if (HDMI_PREVIEW_EN)

				#if (CYCLIC_MODE_CHANGE)
				if(MOVPB_MODE == GetCurrentOpMode())
				{
					bPreviewModeHDMI = AHC_FALSE;
					HDMIFunc_ThumbnailModeSwitch();
					DrawStateHdmiBrowserUpdate(ulJobEvent);
				}
				else if (JPGPB_MODE == GetCurrentOpMode())
				{
					BrowserFunc_ThumbnailResetAllEditOp();
					HDMIFunc_PreviewVideo();
				}
				#else
				bPreviewModeHDMI = AHC_FALSE;
				HDMIFunc_ThumbnailModeSwitch();
				DrawStateHdmiBrowserUpdate(ulJobEvent);
				#endif

			#else
				#if (DSC_MODE_ENABLE)
				HDMIFunc_ThumbnailModeSwitch();
				DrawStateHdmiBrowserUpdate(ulJobEvent);
				#endif
			#endif
			}
		break;

		case EVENT_KEY_PLAYBACK_MODE:
			#if (HDMI_PREVIEW_EN)
			if(MOVPB_MODE == GetCurrentOpMode())
			{
				BrowserFunc_ThumbnailResetAllEditOp();
				HDMIFunc_PreviewVideo();
			}
			else if(JPGPB_MODE == GetCurrentOpMode())
			{
				BrowserFunc_ThumbnailResetAllEditOp();
                HDMIFunc_PreviewCamera();
			}
			else if(JPGPB_MOVPB_MODE == GetCurrentOpMode())
			{
				BrowserFunc_ThumbnailResetAllEditOp();
				HDMIFunc_PreviewVideo();
			}
			#endif
		break;

		case EVENT_SLIDESHOW_MODE:
			if(Deleting_InBrowser || Protecting_InBrowser)
				break;

			BrowserFunc_ThumbnailResetAllEditOp();

			if(HDMIFunc_SlideShow())
			{
				bSlideShow = AHC_TRUE;
        		UpdateSlideMovPBStatus(MOVIE_STATE_PLAY);
			}
		break;

		case EVENT_POWER_OFF:
			AHC_NormalPowerOffPath();
		break;

		//Special Event
		case EVENT_FILE_DELETING:
		case EVENT_LOCK_FILE_M:
		 	StateBrowserMode(ulJobEvent);
		break;

		#if (HDMI_PREVIEW_EN)
		case EVENT_CAMERA_PREVIEW:
			BrowserFunc_ThumbnailResetAllEditOp();
			HDMIFunc_PreviewCamera();
		break;

		case EVENT_VIDEO_PREVIEW:
			BrowserFunc_ThumbnailResetAllEditOp();
			HDMIFunc_PreviewVideo();
		break;
		#endif

		case EVENT_SWITCH_PANEL_TVOUT:
			StateBrowserMode(ulJobEvent);
		break;

		//Update Message
		case EVENT_THUMBNAIL_UPDATE_MESSAGE		 :
			StateBrowserMode(ulJobEvent);
        break;

        default:
        break;
	}
}

void StateHDMI_MoviePBMode_Handler(UINT32 ulJobEvent)
{
	UINT8 	ret;
	UINT32	uState;

	if(m_HdmiStatus!=AHC_HDMI_MOVIE_PB_STATUS)
		return;

    if(bHdmiMenuMode)
    {
		SetHdmiMenuHandler(ulJobEvent);
    	return;
    }

	switch(ulJobEvent)
	{
		case EVENT_NONE                           :
        break;

        case EVENT_KEY_UP:
			if(!bHdmiMenuMode)
			{
			    if(m_DelFile==DEL_FILE_SELECT_NO) {
					m_DelFile = DEL_FILE_SELECT_YES;
					DrawStateHdmiMoviePBUpdate(EVENT_KEY_DOWN);
				}
				else if(m_DelFile == DEL_FILE_NONE){
					HDMIFunc_MoviePBBackward();
                    DrawStateHdmiMoviePBUpdate(ulJobEvent);
				}
			}
        break;
        case EVENT_KEY_DOWN:
			if(!bHdmiMenuMode)
			{ 
			    if(m_DelFile==DEL_FILE_SELECT_YES) {
					m_DelFile = DEL_FILE_SELECT_NO;
					DrawStateHdmiMoviePBUpdate(EVENT_KEY_DOWN);
				}
				else if(m_DelFile == DEL_FILE_NONE){
                    HDMIFunc_MoviePBForward();
                    DrawStateHdmiMoviePBUpdate(ulJobEvent);
				}
			}
        break;
        case EVENT_KEY_WIDE_STOP:
        case EVENT_KEY_TELE_STOP:
        	StateMoviePlaybackMode(ulJobEvent);
        break;

        case EVENT_MOVPB_TOUCH_BKWD_PRESS:
			if(m_DelFile==DEL_FILE_SELECT_NO) {
				m_DelFile = DEL_FILE_SELECT_YES;
				DrawStateHdmiMoviePBUpdate(EVENT_KEY_DOWN);
			}
			else if(m_DelFile==DEL_FILE_NONE){
				HDMIFunc_MoviePBBackward();
	            DrawStateHdmiMoviePBUpdate(EVENT_KEY_LEFT);
			}
        break;

		case EVENT_MOVPB_TOUCH_FRWD_PRESS:
			if(m_DelFile==DEL_FILE_SELECT_YES) {
				m_DelFile = DEL_FILE_SELECT_NO;
				DrawStateHdmiMoviePBUpdate(EVENT_KEY_DOWN);
			}
			else if(m_DelFile==DEL_FILE_NONE){
	        	HDMIFunc_MoviePBForward();
	            DrawStateHdmiMoviePBUpdate(EVENT_KEY_RIGHT);
			}
		break;

		case EVENT_MOVPB_TOUCH_PLAY_PRESS:
		case EVENT_HDMI_BROWSER_KEY_PLAYBACK:
			if(m_DelFile!=DEL_FILE_NONE)
			{
				DrawStateHdmiMoviePBUpdate(EVENT_HDMI_BROWSER_KEY_PLAYBACK);

				if(m_DelFile == DEL_FILE_SELECT_YES)
				{
					if(!HDMIFunc_MoviePBDelFile())
					{
						MovPBTimer_Stop();
		                m_HdmiStatus = AHC_HDMI_NONE_STATUS;
		                HDMIFunc_Enable(AHC_TRUE);
		                break;
					}

					m_DelFile = DEL_FILE_NONE;
		            DrawStateHdmiMoviePBUpdate(EVENT_KEY_DOWN);
		            DrawStateHdmiMoviePBUpdate(EVENT_HDMI_DEL_FILE);
				}
				else if(m_DelFile == DEL_FILE_SELECT_NO)
				{
		           	m_DelFile = DEL_FILE_NONE;
		            DrawStateHdmiMoviePBUpdate(EVENT_KEY_DOWN);
		            DrawStateHdmiMoviePBUpdate(EVENT_MOVPB_TOUCH_PAUSE_PRESS);
				}
			}
			else
			{
				HDMIFunc_MoviePBPlayPress();
			}
		break;
	#ifdef NET_SYNC_PLAYBACK_MODE
		case EVENT_NET_ENTER_PLAYBACK :
			m_DelFile = DEL_FILE_NONE;
			MovPBTimer_Stop();
			m_bMovieOSDForceShowUp = AHC_FALSE;
			MovPlaybackParamReset();
			m_HdmiStatus = AHC_HDMI_NET_PLAYBACK_STATUS;
			{
			    UINT32 		uState;
				
	   			 AHC_GetSystemStatus(&uState);
				 if(STATE_MOV_EXECUTE == uState)
				 	MovPlayback_Pause();
			}
			HDMIFunc_Enable(AHC_TRUE);
            CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
		break;
	#endif
		case EVENT_KEY_MODE:
		case EVENT_MOVPB_TOUCH_RETURN:
			m_DelFile = DEL_FILE_NONE;
			MovPBTimer_Stop();
			m_bMovieOSDForceShowUp = AHC_FALSE;
			MovPlaybackParamReset();
			m_HdmiStatus = AHC_HDMI_NONE_STATUS;

			{
			    UINT32 		uState;
				
	   			 AHC_GetSystemStatus(&uState);
				 if(STATE_MOV_EXECUTE == uState)
				 	MovPlayback_Pause();
			}

			HDMIFunc_Enable(AHC_TRUE);
		break;

		case EVENT_KEY_MENU:
			StateHDMI_EventMenu_Handler(ulJobEvent);
		break;

		case EVENT_POWER_OFF:
			StateMoviePlaybackMode(ulJobEvent);
		break;

		//Special Event
		case EVENT_MOVPB_TOUCH_PREV_PRESS:
			HDMIFunc_MoviePBPrevious();
		break;

		case EVENT_MOVPB_TOUCH_NEXT_PRESS:
			HDMIFunc_MoviePBNext();
		break;

		#if (HDMI_PREVIEW_EN)
		case EVENT_VIDEO_PREVIEW:
			MovPBTimer_Stop();
			MovPlaybackParamReset();
			HDMIFunc_PreviewVideo();
		break;

		case EVENT_CAMERA_PREVIEW:
			MovPBTimer_Stop();
			MovPlaybackParamReset();
			HDMIFunc_PreviewCamera();
		break;
		#endif

		case EVENT_FILE_DELETING:
			ret = QuickFileOperation(DCF_FILE_DELETE);

			if(ret!=EDIT_FILE_SUCCESS)
			{
				DrawFileOperationResult(ret);
			}
			else
			{
				m_DelFile = DEL_FILE_NONE;
				MovPBTimer_Stop();
				m_bMovieOSDForceShowUp = AHC_FALSE;
				MovPlaybackParamReset();
				m_HdmiStatus = AHC_HDMI_NONE_STATUS;
				HDMIFunc_Enable(AHC_TRUE);
			}
		break;

		case EVENT_LOCK_FILE_M:
		case EVENT_SWITCH_PANEL_TVOUT:
			StateMoviePlaybackMode(ulJobEvent);
		break;

		//Update Message
        case EVENT_MOVPB_UPDATE_MESSAGE:

			if( MOV_STATE_STOP == GetMovConfig()->iState || MOV_STATE_PAUSE == GetMovConfig()->iState)
			{
				AHC_GetVideoPlayStopStatus(&uState);

			    if ((AHC_VIDEO_PLAY_EOF == uState) || (AHC_VIDEO_PLAY_ERROR_STOP == uState))
				{
					MovPBTimer_Stop();
					m_bMovieOSDForceShowUp = AHC_FALSE;
					MovPlaybackParamReset();

					DrawStateHdmiMoviePBUpdate(EVENT_HDMI_PLAY_STOP);
                    #if 1 
					// needs return to browser mode
					m_HdmiStatus = AHC_HDMI_NONE_STATUS;
					HDMIFunc_Enable(AHC_TRUE);
					#endif 
					
				}
				else if(uState == AHC_VIDEO_PLAY_FF_EOS || uState == AHC_VIDEO_PLAY_BW_BOS)
				{
					#if ( MOVIE_PB_FF_EOS_ACTION==EOS_ACT_RESTART || \
						  MOVIE_PB_FF_EOS_ACTION==EOS_ACT_PAUSE_AT_FIRST_FRAME )
					HDMIFunc_MoviePBSeekSOS();
					#endif
				}
			}
			else if(MOV_STATE_PLAY == GetMovConfig()->iState || MOV_STATE_FF == GetMovConfig()->iState)
			{
				DrawStateHdmiMoviePBUpdate(ulJobEvent);
			}
        break;

        default:
        break;
	}
}

void StateHDMI_PhotoPBMode_Handler(UINT32 ulJobEvent)
{
	UINT8 ret;

	if(m_HdmiStatus!=AHC_HDMI_PHOTO_PB_STATUS)
		return;

    if(bHdmiMenuMode)
    {
		SetHdmiMenuHandler(ulJobEvent);
    	return;
    }

	switch(ulJobEvent)
	{
		case EVENT_NONE                           :
        break;

        case EVENT_KEY_UP:
        case EVENT_KEY_DOWN:
        	//TBD  StatePhotoPlaybackMode(ulJobEvent);
        break;

        case EVENT_KEY_LEFT:
			if(m_DelFile==DEL_FILE_SELECT_NO) {
				m_DelFile = DEL_FILE_SELECT_YES;
				DrawStateHdmiPhotoPBUpdate(EVENT_KEY_DOWN);
			}
			else if(m_DelFile==DEL_FILE_NONE){
				if(HDMIFunc_PhotoPBPrevious())
					DrawStateHdmiPhotoPBUpdate(EVENT_KEY_LEFT);
			}
        break;

        case EVENT_KEY_RIGHT:
			if(m_DelFile==DEL_FILE_SELECT_YES) {
				m_DelFile = DEL_FILE_SELECT_NO;
				DrawStateHdmiPhotoPBUpdate(EVENT_KEY_DOWN);
			}
			else if(m_DelFile==DEL_FILE_NONE){
				if(HDMIFunc_PhotoPBNext())
					DrawStateHdmiPhotoPBUpdate(EVENT_KEY_RIGHT);
			}
        break;

        case EVENT_KEY_SET:
        	StatePhotoPlaybackMode(EVENT_HDMI_BROWSER_KEY_PLAYBACK);
        break;
	#ifdef NET_SYNC_PLAYBACK_MODE
		case EVENT_NET_ENTER_PLAYBACK :
			m_DelFile = DEL_FILE_NONE;
			PhotoPBTimer_Stop();
			JpgPlaybackParamReset();
			m_HdmiStatus = AHC_HDMI_NET_PLAYBACK_STATUS;
			HDMIFunc_Enable(AHC_TRUE);
            CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
		break;
	#endif
		case EVENT_KEY_MODE:
        case EVENT_JPGPB_TOUCH_RETURN:
			m_DelFile = DEL_FILE_NONE;
			PhotoPBTimer_Stop();
			JpgPlaybackParamReset();
			m_HdmiStatus = AHC_HDMI_NONE_STATUS;
			HDMIFunc_Enable(AHC_TRUE);
        break;

        case EVENT_KEY_MENU:
        	StateHDMI_EventMenu_Handler(ulJobEvent);
        break;

        case EVENT_POWER_OFF:
        	AHC_NormalPowerOffPath();
        break;

		//Special Event
        case EVENT_JPGPB_TOUCH_PREV_PRESS:
			if(HDMIFunc_PhotoPBPrevious())
				DrawStateHdmiPhotoPBUpdate(EVENT_KEY_LEFT);
        break;

        case EVENT_JPGPB_TOUCH_NEXT_PRESS:
 			if(HDMIFunc_PhotoPBNext())
				DrawStateHdmiPhotoPBUpdate(EVENT_KEY_RIGHT);
        break;

   	    #if (HDMI_PREVIEW_EN)
		case EVENT_VIDEO_PREVIEW:
			PhotoPBTimer_Stop();
			JpgPlaybackParamReset();
			HDMIFunc_PreviewVideo();
		break;

		case EVENT_CAMERA_PREVIEW:
			PhotoPBTimer_Stop();
			JpgPlaybackParamReset();
			HDMIFunc_PreviewCamera();
		break;
		#endif

		case EVENT_FILE_DELETING:
			ret = QuickFileOperation(DCF_FILE_DELETE);

			if(ret!=EDIT_FILE_SUCCESS)
			{
				DrawFileOperationResult(ret);
			}
			else
			{
				m_DelFile = DEL_FILE_NONE;
				PhotoPBTimer_Stop();
				JpgPlaybackParamReset();
				m_HdmiStatus = AHC_HDMI_NONE_STATUS;
				HDMIFunc_Enable(AHC_TRUE);
			}
        break;

        case EVENT_LOCK_FILE_M:
        case EVENT_SWITCH_PANEL_TVOUT:
        	StatePhotoPlaybackMode(ulJobEvent);
        break;

	 	//Update Message
        case EVENT_JPGPB_UPDATE_MESSAGE:
			//TBD DrawStateJpgPlaybackUpdate(ulJobEvent);
		break;

		default:
		break;
	}
}

void StateHDMI_SlideShowMode_Handler(UINT32 ulJobEvent)
{
#if (SLIDESHOW_EN)

	UINT8 	ret;
	UINT32 	uState;

	if(m_HdmiStatus!=AHC_HDMI_SLIDE_SHOW_STATUS)
		return;

    if(bHdmiMenuMode)
    {
		SetHdmiMenuHandler(ulJobEvent);
    	return;
    }

	switch(ulJobEvent)
	{
		case EVENT_NONE                           :
        break;

        case EVENT_SLIDESHOW_TOUCH_PREV_PRESS:
			if(	PLAYBACK_VIDEO_TYPE == GetPlayBackFileType() )
			{
				MovPlayback_Pause();
			}
			else if( PLAYBACK_AUDIO_TYPE == GetPlayBackFileType())
			{
				AudPlayback_Pause();
			}

			SlideShowCounter = 0;
			MediaPlaybackConfig(PLAYBACK_PREVIOUS);
			SlideShowFunc_Play();
			DrawStateHdmiSlideShowUpdate(EVENT_SLIDESHOW_UPDATE_FILE);
        break;

        case EVENT_SLIDESHOW_TOUCH_NEXT_PRESS:
			if(	PLAYBACK_VIDEO_TYPE == GetPlayBackFileType() )
			{
				MovPlayback_Pause();
			}
			else if( PLAYBACK_AUDIO_TYPE == GetPlayBackFileType())
			{
				AudPlayback_Pause();
			}

			SlideShowCounter = 0;
			MediaPlaybackConfig(PLAYBACK_NEXT);
			SlideShowFunc_Play();
			DrawStateHdmiSlideShowUpdate(EVENT_SLIDESHOW_UPDATE_FILE);
        break;

        case EVENT_SLIDESHOW_TOUCH_PLAY_PRESS:

			if(PLAYBACK_IMAGE_TYPE == GetPlayBackFileType())
			{
				bSlideShow = (bSlideShow)?(AHC_FALSE):(AHC_TRUE);
	        }
			else if((PLAYBACK_VIDEO_TYPE == GetPlayBackFileType()) ||
					(PLAYBACK_AUDIO_TYPE == GetPlayBackFileType()) )
			{
				AHC_GetSystemStatus(&uState);

	            switch(uState)
	            {
					case STATE_MOV_IDLE    :
						MediaPlaybackConfig(PLAYBACK_CURRENT);

						if(!MovPlayback_Play())
						{
							MovPBTimer_Stop();
							m_HdmiStatus = AHC_HDMI_NONE_STATUS;
							HDMIFunc_Enable(AHC_TRUE);
	                        AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);
	                        return;
	                    }
	                    DrawStateHdmiMoviePBUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);
					break;

					case STATE_MOV_EXECUTE :
						if(MOV_STATE_FF == GetMovConfig()->iState)
						{
							MovPlayback_NormalSpeed();
							DrawStateHdmiMoviePBUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);
						}
						else
						{
							MovPlayback_Pause();
							DrawStateHdmiMoviePBUpdate(EVENT_MOVPB_TOUCH_PAUSE_PRESS);
						}
					break;

					case STATE_MOV_PAUSE   :
						AHC_PlaybackClipCmd(AHC_PB_MOVIE_NORMAL_PLAY_SPEED, 0);
						MovPlayback_Resume();
						DrawStateHdmiMoviePBUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);
	                break;

	                default                :
	                break;
				}
			}
        break;

		case EVENT_SLIDESHOW_TOUCH_RETURN:
			SlideShowTimer_Stop();

			if(PLAYBACK_IMAGE_TYPE == GetPlayBackFileType())
			{
				JpgPlaybackParamReset();
		        SetCurrentOpMode(JPGPB_MODE);
		        AHC_UF_SetFreeChar(0, DCF_SET_ALLOWED, SEARCH_PHOTO);
			}
			else
			{
				MovPlaybackParamReset();
				SetCurrentOpMode(MOVPB_MODE);
				AHC_UF_SetFreeChar(0, DCF_SET_ALLOWED, SEARCH_MOVIE);
			}
			m_HdmiStatus = AHC_HDMI_NONE_STATUS;
			HDMIFunc_Enable(AHC_TRUE);
		break;
	#ifdef NET_SYNC_PLAYBACK_MODE
		case EVENT_NET_ENTER_PLAYBACK :
			m_HdmiStatus = AHC_HDMI_NET_PLAYBACK_STATUS;
			HDMIFunc_Enable(AHC_TRUE);
            CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
		break;
	#endif
		case EVENT_KEY_MODE:
			//TBD
		break;

		case EVENT_KEY_MENU:
			StateHDMI_EventMenu_Handler(ulJobEvent);
		break;

		case EVENT_POWER_OFF:
			StateSlideShowMode(ulJobEvent);
		break;

		//Special Event
        case EVENT_SLIDESHOW_TOUCH_BKWD_PRESS:
        case EVENT_SLIDESHOW_TOUCH_FRWD_PRESS:
        	StateSlideShowMode(ulJobEvent);
        break;

        case EVENT_SWITCH_PANEL_TVOUT:
        	StateSlideShowMode(ulJobEvent);
        break;

 		//Update Message
        case EVENT_SLIDESHOW_MODE_INIT:
		case EVENT_SLIDESHOW_MODE:
			//TBD
		break;

		case EVENT_SLIDESHOW_UPDATE_FILE:

			if(AHC_TRUE == bSlideShowNext)
            {
				bSlideShow = AHC_FALSE;
				SlideShowCounter = 0;

				MediaPlaybackConfig(PLAYBACK_NEXT);
				ret = SlideShowFunc_Play();

				if(ret == AHC_FALSE) {
					bFileFail = AHC_TRUE;
					bSlideShow = AHC_TRUE;
					return;
				}
				else {
					bFileFail = AHC_FALSE;
				}

				if(SlideShowFunc_GetOSDShowStatus() == AHC_TRUE)
				{
					DrawStateHdmiSlideShowUpdate(EVENT_SLIDESHOW_UPDATE_FILE);
				}

				bSlideShowNext 	= AHC_FALSE;
				bSlideShow 		= AHC_TRUE;
			}
		break;

		case EVENT_SLIDESHOW_UPDATE_MESSAGE:
			DrawStateHdmiSlideShowUpdate(EVENT_MOVPB_UPDATE_MESSAGE);
		break;
	}
#endif
}

void StateHDMI_NetPlayback_Handler(UINT32 ulJobEvent)
{
    switch(ulJobEvent)
    {
    	case EVENT_NET_EXIT_PLAYBACK :

			MenuStatePlaybackSelectDB( MOVPB_MODE );    
            SetCurrentOpMode(MOVPB_MODE);
			BrowserFunc_ThumbnailResetAllEditOp(); 
			HDMIFunc_PreviewVideo();
    		CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
    		
    	break;
    }	
}	
#endif

#endif

//******************************************************************************
//
//                              AHC State HDMI Mode
//
//******************************************************************************

#if (!USE_NEW_HDMI_EVENT_HANDLER)

void StateHDMIMode(UINT32 ulEvent)
{
#if (HDMI_ENABLE)

    UINT32 		ulJobEvent  = ulEvent;
	AHC_BOOL 	ret 		= 0;
	UINT32 		uState;

	if(ulEvent == EVENT_NONE)
	    return;

    if(!HDMIFunc_MoviePBCheckOSDShowUp(ulEvent))
        return;

    if(!HDMIFunc_PhotoPBCheckOSDShowUp(ulEvent))
        return;

    if(!HDMIFunc_SlideShowCheckOSDShowUp(ulEvent))
        return;

    switch( ulEvent )
    {
        case EVENT_NONE                           :
        break;

        case EVENT_KEY_UP                         :
        case EVENT_KEY_TELE_STOP				  :
			if(m_DelFile!=DEL_FILE_NONE)
				break;

			StateHDMI_EventUp_Handler(ulJobEvent);
        break;

        case EVENT_KEY_DOWN                       :
        case EVENT_KEY_WIDE_STOP				  :
			StateHDMI_EventDown_Handler(ulJobEvent);
        break;

        case EVENT_KEY_LEFT                       :
			StateHDMI_EventLeft_Handler(ulJobEvent);
		break;

        case EVENT_KEY_RIGHT                      :
			StateHDMI_EventRight_Handler(ulJobEvent);
        break;

        case EVENT_HDMI_BROWSER_KEY_PLAYBACK              :
#ifndef CFG_HDMI_SKIP_OK_IN_JPGPB //may be defined in config_xxx.h
            /* Don't do OK event (REC press) for this project in JPEG playback mode */
        	if (m_HdmiStatus != AHC_HDMI_PHOTO_PB_STATUS)
#endif
				StateHDMI_EventOK_Handler(ulJobEvent);
		break;

        case EVENT_KEY_MODE                       :
			StateHDMI_EventMode_Handler(ulJobEvent);
		break;

		case EVENT_MOVPB_MODE:
			StateHDMI_EventMovPBMode_Handler(ulJobEvent);
		break;

		case EVENT_KEY_MENU:
			StateHDMI_EventMenu_Handler(ulJobEvent);
		break;

        case EVENT_HDMI_PLAY_STOP              :
			StateHDMI_EventExit_Handler(ulJobEvent);
		break;

        case EVENT_KEY_PLAYBACK_MODE              :
		break;

        case EVENT_VIDEO_KEY_RECORD               :
			StateHDMI_EventRec_Handler(ulJobEvent);
		break;

        case EVENT_POWER_OFF                      :
			AHC_NormalPowerOffPath();
        break;

		//Special Event
#if (HDMI_PREVIEW_EN)

		case EVENT_FORMAT_MEDIA				:
		case EVENT_FORMAT_RESET_ALL			:
		case EVENT_VIDEO_KEY_CAPTURE		:
		case EVENT_LOCK_VR_FILE				:
		case EVENT_RECORD_MUTE 				:
		case EVENT_AUDIO_RECORD				:
			if(HDMIFunc_Status()==AHC_HDMI_VIDEO_PREVIEW_STATUS)
				StateVideoRecMode(ulJobEvent);
		break;

      	case EVENT_CHANGE_LED_MODE			:
		case EVENT_CHANGE_NIGHT_MODE 		:
			StateVideoRecMode(ulJobEvent);
		break;

		case EVENT_SWITCH_VMD_MODE			:
			if(HDMIFunc_Status()==AHC_HDMI_VIDEO_PREVIEW_STATUS)
				StateVideoRecMode(ulJobEvent);
			else if(HDMIFunc_Status()==AHC_HDMI_DSC_PREVIEW_STATUS)
				StateCaptureMode(ulJobEvent);
		break;
#endif

		case EVENT_FILE_DELETING 				  :
            #ifdef CFG_HDMI_QUICK_DELETE_FILE //may be defined in config_xxx.h
			//Quick Delete File
			if(	m_HdmiStatus == AHC_HDMI_MOVIE_PB_STATUS ||
				m_HdmiStatus == AHC_HDMI_PHOTO_PB_STATUS)
			{
				m_DelFile = DEL_FILE_SELECT_YES;
				StateHDMI_EventOK_Handler(EVENT_HDMI_BROWSER_KEY_PLAYBACK);
			}
			else if(m_HdmiStatus == AHC_HDMI_BROWSER_STATUS)
			{
				HDMIFunc_ThumbnailQuickFileDelete();
			}
			#else//Manual Selection
			if(	m_HdmiStatus == AHC_HDMI_MOVIE_PB_STATUS ||
				m_HdmiStatus == AHC_HDMI_PHOTO_PB_STATUS)
			{
				if(m_DelFile!=DEL_FILE_NONE)
					StateHDMI_EventOK_Handler(EVENT_HDMI_BROWSER_KEY_PLAYBACK);
				else
					StateHDMI_EventDown_Handler(EVENT_KEY_DOWN);

			}
			else if(m_HdmiStatus == AHC_HDMI_BROWSER_STATUS)
			{
				if(Deleting_InBrowser || bHdmiMenuMode)
				{
					//TBD
				}
				else
				{
					SetCurrentMenu(&sSubDelete);
					SubMenuEventHandler(&sSubDelete, MENU_INIT, 0);
					AHC_OSDSetActive(HDMI_UI_OSD_ID, 0);
					AHC_OSDSetActive(HDMI_MENU_OSD_ID, 1);
					bHdmiMenuMode = AHC_TRUE;
				}
			}
			#endif
		break;

   		//Device
		case EVENT_DC_CABLE_IN:
			if(m_HdmiStatus==AHC_HDMI_VIDEO_PREVIEW_STATUS)
			{
				#if (CHARGER_IN_ACT_VIDEO_MODE==ACT_RECORD_VIDEO)
				if(VideoFunc_RecordStatus()==AHC_FALSE)
					AHC_SetRecordByChargerIn(3);
				#elif (CHARGER_IN_ACT_VIDEO_MODE == ACT_FORCE_POWER_OFF)
				SetKeyPadEvent(KEY_POWER_OFF);
				#endif
			}
		break;

		case EVENT_DC_CABLE_OUT:
			AHC_SetChargerEnable(AHC_FALSE);

			if(m_HdmiStatus==AHC_HDMI_VIDEO_PREVIEW_STATUS)
			{
		    	if( VideoFunc_RecordStatus() )
				{
					#if (CHARGER_OUT_ACT_VIDEO_REC==ACT_FORCE_POWER_OFF || CHARGER_OUT_ACT_VIDEO_REC==ACT_DELAY_POWER_OFF)
					AHC_SetShutdownByChargerOut(AHC_TRUE);
					#endif
				}
				else
				{
					#if (CHARGER_OUT_ACT_OTHER_MODE==ACT_FORCE_POWER_OFF || CHARGER_OUT_ACT_OTHER_MODE==ACT_DELAY_POWER_OFF)
		 			AHC_SetShutdownByChargerOut(AHC_TRUE);
					#endif
				}
			}
			else
			{
		     	#if (CHARGER_OUT_ACT_OTHER_MODE==ACT_FORCE_POWER_OFF || CHARGER_OUT_ACT_OTHER_MODE==ACT_DELAY_POWER_OFF)
				AHC_SetShutdownByChargerOut(AHC_TRUE);
				#endif
			}
		break;

		case EVENT_USB_DETECT                     :
			if(IsAdapter_Connect())
			{
				if(m_HdmiStatus==AHC_HDMI_VIDEO_PREVIEW_STATUS)
				{
					#if (CHARGER_IN_ACT_VIDEO_MODE==ACT_RECORD_VIDEO)
					if(VideoFunc_RecordStatus()==AHC_FALSE)
						AHC_SetRecordByChargerIn(3);
					#elif (CHARGER_IN_ACT_VIDEO_MODE == ACT_FORCE_POWER_OFF)
					SetKeyPadEvent(KEY_POWER_OFF);
					#endif
				}
			}
			else
			{
				// Lock HDMI state at non-plugged state when USB plug-in
				// Will be unlock when USB plug-out
				AHC_LockHdmiConnectionStatus(AHC_TRUE, AHC_FALSE);

				AHC_OSDUninit();
	            AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
    	        InitOSD();

				// Exit HDMI mode
				HDMIFunc_Enable(AHC_FALSE);
				m_HdmiStatus = AHC_HDMI_NONE_STATUS;

    	        #if (USB_MODE_SELECT_EN)
				if (MMPS_USB_NORMAL == MMPS_USB_GetStatus()) {
				    StateSwitchMode(UI_USBSELECT_MENU_STATE);
				}
				else
				{
					#ifdef CFG_POWER_ON_ENTER_CAMERA_STATE
					StateSwitchMode(UI_CAMERA_STATE);
					#else
					StateSwitchMode(UI_VIDEO_STATE);
					#endif
				}
				#else
				if(MenuSettingConfig()->uiUSBFunction == USB_MSDC)
				{
                	ret = StateSwitchMode(UI_MSDC_STATE);
				}
				else
				{
					ret = StateSwitchMode(UI_PCCAM_STATE);
				}
				#endif
			}
        break;

        case EVENT_USB_REMOVED                    :

			if(AHC_USB_GetLastStatus() == AHC_USB_NORMAL) // MSDC mode
			{
				AHC_SetMode(AHC_MODE_IDLE);
				bForce_PowerOff = AHC_TRUE;
				AHC_NormalPowerOffPath();
			}
			else
			{
                if (AHC_IsDcCableConnect() == AHC_TRUE)
                    break;

		     	AHC_SetChargerEnable(AHC_FALSE);

				if(m_HdmiStatus==AHC_HDMI_VIDEO_PREVIEW_STATUS)
				{
			    	if( VideoFunc_RecordStatus() )
					{
						#if (CHARGER_OUT_ACT_VIDEO_REC==ACT_FORCE_POWER_OFF || CHARGER_OUT_ACT_VIDEO_REC==ACT_DELAY_POWER_OFF)
						AHC_SetShutdownByChargerOut(AHC_TRUE);
						#endif
					}
					else
					{
						#if (CHARGER_OUT_ACT_OTHER_MODE==ACT_FORCE_POWER_OFF || CHARGER_OUT_ACT_OTHER_MODE==ACT_DELAY_POWER_OFF)
			 			AHC_SetShutdownByChargerOut(AHC_TRUE);
						#endif
					}
				}
				else
				{
			     	#if (CHARGER_OUT_ACT_OTHER_MODE==ACT_FORCE_POWER_OFF || CHARGER_OUT_ACT_OTHER_MODE==ACT_DELAY_POWER_OFF)
					AHC_SetShutdownByChargerOut(AHC_TRUE);
					#endif
				}
			}
        break;

        case EVENT_SD_DETECT                      :
            AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect);
            AHC_OS_Sleep(100);

            if(!HDMIFunc_IsPreview())
            {
            	m_HdmiStatus = AHC_HDMI_NONE_STATUS;
            	HDMIFunc_Enable(AHC_TRUE);
        	}
        break;

        case EVENT_SD_REMOVED                     :
			switch(m_HdmiStatus)
			{

#if (HDMI_PREVIEW_EN)
				case AHC_HDMI_VIDEO_PREVIEW_STATUS:
					if(VideoFunc_RecordStatus())
           		 	{
                		StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
            		}
				break;
#endif
				case AHC_HDMI_MOVIE_PB_STATUS:

					if(MOV_STATE_FF == GetMovConfig()->iState)
					{
						MovPlayback_NormalSpeed();
             			AHC_OS_Sleep(100);
					}

					AHC_SetMode(AHC_MODE_IDLE);

					MovPBTimer_Stop();
					m_bMovieOSDForceShowUp = AHC_FALSE;
					MovPlaybackParamReset();
				break;

				case AHC_HDMI_PHOTO_PB_STATUS:
					PhotoPBTimer_Stop();
					JpgPlaybackParamReset();
				break;
			}

			m_DelFile = DEL_FILE_NONE;
			ObjSelect = 0;

			if(AHC_TRUE == AHC_SDMMC_GetMountState())
			{
				AHC_DisMountStorageMedia(AHC_MEDIA_MMC);
				AHC_OS_Sleep(100);
				Enable_SD_Power(0 /* Power Off */);
			}

			if(!bPreviewModeHDMI)
			{
				m_HdmiStatus = AHC_HDMI_NONE_STATUS;
				HDMIFunc_Enable(AHC_TRUE);
			}
		break;

		case EVENT_HDMI_DETECT                    :
		break;

		case EVENT_HDMI_REMOVED                   :
        case EVENT_HDMI_KEY_LCD_OUTPUT            :
        	bHdmiMenuMode = AHC_FALSE;

#if defined(DOT_MATRIX_PANEL)

        	if(VideoFunc_RecordStatus())
			{
				StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
			}

			VideoTimer_Stop();

            AHC_OSDUninit();
            
			AHC_SetMode(AHC_MODE_IDLE);

			AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
			InitOSD();
			m_HdmiStatus = AHC_HDMI_NONE_STATUS;

			AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_MAIN_LCD);
			bPreviewModeHDMI = AHC_FALSE;

			StateSwitchMode(UI_VIDEO_STATE);
			RTNA_LCD_Backlight(MMP_TRUE);

#else	//DOT_MATRIX_PANEL

			switch(m_HdmiStatus)
			{
				case AHC_HDMI_BROWSER_STATUS:
					if(AHC_IsTVConnect())
					{ //HDMI 2 TV
						SetTVState(AHC_TV_NONE_STATUS);
	                    StateSwitchMode(UI_TVOUT_STATE);
	                }
	                else
	                {
						#ifdef CFG_HDMI_PLUGOUT_RETURN_VIDEO_PREVIEW

                        AHC_OSDUninit();
                        
						AHC_SetMode(AHC_MODE_IDLE);

						AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
						InitOSD();
						m_HdmiStatus = AHC_HDMI_NONE_STATUS;

						AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_MAIN_LCD);
						bPreviewModeHDMI = AHC_FALSE;

						StateSwitchMode(UI_VIDEO_STATE);
						RTNA_LCD_Backlight(MMP_TRUE);

						#else

	                	AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_MAIN_LCD);
	                    HDMIFunc_Enable(AHC_FALSE);

						#endif
	                }

	                m_HdmiStatus = AHC_HDMI_NONE_STATUS;
	            break;

	            case AHC_HDMI_MOVIE_PB_STATUS:
					if(AHC_IsTVConnect())
					{ //HDMI 2 TV
						AHC_GetVideoCurrentPlayTime(&m_VideoCurPlayTime);
	                    MovPBTimer_Stop();
	                    MovPlaybackParamReset();
	                    AHC_SetVideoPlayStartTime(m_VideoCurPlayTime);
	                    SetTVState(AHC_TV_MOVIE_PB_STATUS);
	                    StateSwitchMode(UI_TVOUT_STATE);
	                }
	                else
	                { //HDMI to LCD
	                	AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_MAIN_LCD);
	                    AHC_GetVideoCurrentPlayTime(&m_VideoCurPlayTime);
	                    AHC_OSDUninit();
	                    AHC_SetMode(AHC_MODE_IDLE);
	                    AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
	                    InitOSD();
	                    AHC_UF_GetCurrentIndex(&ObjSelect);
	                    AHC_SetVideoPlayStartTime(m_VideoCurPlayTime);

	                    m_HdmiStatus = AHC_HDMI_NONE_STATUS;
	                    bMovPbExitFromHdmi = AHC_TRUE;
	                    UpdateMovPBStatus(bHdmiMovieState);

						#ifdef CFG_HDMI_PLUGOUT_RETURN_VIDEO_PREVIEW
						m_VideoCurPlayTime = 0;
	                    AHC_SetVideoPlayStartTime(m_VideoCurPlayTime);
						StateSwitchMode(UI_VIDEO_STATE);
						#else
	                    StateSwitchMode(UI_PLAYBACK_STATE);
						#endif

	                    RTNA_LCD_Backlight(MMP_TRUE);
	            	}
	            break;

	            case AHC_HDMI_PHOTO_PB_STATUS:
	            	if(AHC_IsTVConnect())
	            	{ //HDMI 2 TV
	                	PhotoPBTimer_Stop();
	                    JpgPlaybackParamReset();
	                    SetTVState(AHC_TV_PHOTO_PB_STATUS);
	                    StateSwitchMode(UI_TVOUT_STATE);
	                }
	                else
	                { //HDMI to LCD
	                	AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_MAIN_LCD);
	                	AHC_OSDUninit();
	                    AHC_SetMode(AHC_MODE_IDLE);

	                    AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
	                    InitOSD();
	                    AHC_UF_GetCurrentIndex(&ObjSelect);
	                    m_HdmiStatus = AHC_HDMI_NONE_STATUS;
						#ifdef CFG_HDMI_PLUGOUT_RETURN_VIDEO_PREVIEW
						StateSwitchMode(UI_VIDEO_STATE);
						#else
	                    StateSwitchMode(UI_PLAYBACK_STATE);
						#endif
	                    RTNA_LCD_Backlight(MMP_TRUE);
	                }
				break;

				case AHC_HDMI_SLIDE_SHOW_STATUS:
	                if(AHC_IsTVConnect())
	                { //HDMI 2 TV
						AHC_GetVideoCurrentPlayTime(&m_VideoCurPlayTime);
						SlideShowTimer_Stop();
						SlideShowFunc_Stop();
						AHC_SetVideoPlayStartTime(m_VideoCurPlayTime);
						SetTVState(AHC_TV_SLIDE_SHOW_STATUS);
						StateSwitchMode(UI_TVOUT_STATE);
	                }
	                else
	                { //HDMI to LCD
						AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_MAIN_LCD);
						AHC_GetVideoCurrentPlayTime(&m_VideoCurPlayTime);
						AHC_OSDUninit();
						AHC_SetMode(AHC_MODE_IDLE);
						AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
						InitOSD();
						AHC_UF_GetCurrentIndex(&ObjSelect);
						AHC_SetVideoPlayStartTime(m_VideoCurPlayTime);
						bSlideShowFirstFile = AHC_FALSE;
						m_HdmiStatus = AHC_HDMI_NONE_STATUS;
						StateSwitchMode(UI_SLIDESHOW_STATE);
						RTNA_LCD_Backlight(MMP_TRUE);
					}
				break;

				#if (HDMI_PREVIEW_EN)
				case AHC_HDMI_VIDEO_PREVIEW_STATUS:

					if(VideoFunc_RecordStatus())
					{
						StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
					}

					VideoTimer_Stop();

					AHC_SetMode(AHC_MODE_IDLE);
					AHC_OSDUninit();

					AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
					InitOSD();
					m_HdmiStatus = AHC_HDMI_NONE_STATUS;

					AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_MAIN_LCD);
					bPreviewModeHDMI = AHC_FALSE;

					StateSwitchMode(UI_VIDEO_STATE);
					RTNA_LCD_Backlight(MMP_TRUE);
				break;

				case AHC_HDMI_DSC_PREVIEW_STATUS:
					CaptureTimer_Stop();
                    AHC_OSDUninit();
					AHC_SetMode(AHC_MODE_IDLE);
					AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
					InitOSD();
					m_HdmiStatus = AHC_HDMI_NONE_STATUS;

					AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_MAIN_LCD);
					bPreviewModeHDMI = AHC_FALSE;

					StateSwitchMode(UI_CAMERA_STATE);
					RTNA_LCD_Backlight(MMP_TRUE);
				break;
				#endif
			}
#endif//DOT_MATRIX_PANEL

			#if (POWER_OFF_CONFIRM_PAGE_EN)
			if(PowerOff_InProcess)
			{
				MenuDrawSubPage_PowerOff();
			}
			#endif
        break;

		//Update Message
		case EVENT_SLIDESHOW_UPDATE_FILE          :

			if(m_HdmiStatus == AHC_HDMI_SLIDE_SHOW_STATUS)
			{
				if(AHC_TRUE == bSlideShowNext)
                {
					bSlideShow = AHC_FALSE;
	                SlideShowCounter = 0;

	                MediaPlaybackConfig(PLAYBACK_NEXT);
	                ret = SlideShowFunc_Play();

	                if(ret == AHC_FALSE) {
	                   bFileFail = AHC_TRUE;
	                   bSlideShow = AHC_TRUE;
	                   return;
	                }
	                else {
	                   bFileFail = AHC_FALSE;
	                }

	                if(SlideShowFunc_GetOSDShowStatus() == AHC_TRUE)
	                {
	                    DrawStateHdmiSlideShowUpdate( EVENT_SLIDESHOW_UPDATE_FILE);
	                }

	                bSlideShowNext 	= AHC_FALSE;
	                bSlideShow 		= AHC_TRUE;
                }
        	}
        break;

        case EVENT_MOVPB_UPDATE_MESSAGE           :

			StateLedControl(UI_PLAYBACK_STATE);

            switch(m_HdmiStatus)
            {
				case AHC_HDMI_MOVIE_PB_STATUS:

					if( MOV_STATE_STOP == GetMovConfig()->iState || MOV_STATE_PAUSE == GetMovConfig()->iState)
					{
						AHC_GetVideoPlayStopStatus(&uState);

			            if ((AHC_VIDEO_PLAY_EOF == uState) || (AHC_VIDEO_PLAY_ERROR_STOP == uState))
						{
							MovPBTimer_Stop();
							m_bMovieOSDForceShowUp = AHC_FALSE;
							MovPlaybackParamReset();

							DrawStateHdmiMoviePBUpdate(EVENT_HDMI_PLAY_STOP);

                            #ifdef CFG_HDMI_BACK_TO_BROWSER_AFTER_EOF //may be defined in config_xxx.h
                            //needs return to browser mode
							StateHDMIMode(EVENT_KEY_MODE);
							#endif
						}
                    	else if(uState == AHC_VIDEO_PLAY_FF_EOS || uState == AHC_VIDEO_PLAY_BW_BOS)
                    	{
							#if ( MOVIE_PB_FF_EOS_ACTION==EOS_ACT_RESTART || \
								  MOVIE_PB_FF_EOS_ACTION==EOS_ACT_PAUSE_AT_FIRST_FRAME )
							HDMIFunc_MoviePBSeekSOS();
							#endif
                    	}
					}
					else if(MOV_STATE_PLAY == GetMovConfig()->iState || MOV_STATE_FF == GetMovConfig()->iState)
					{
						DrawStateHdmiMoviePBUpdate(ulJobEvent);
					}
				break;

				case AHC_HDMI_PHOTO_PB_STATUS:
					DrawStateHdmiPhotoPBUpdate(EVENT_HDMI_PHOTOPB_UPDATE_MESSAGE);
				break;

				case AHC_HDMI_SLIDE_SHOW_STATUS:
					DrawStateHdmiSlideShowUpdate(EVENT_MOVPB_UPDATE_MESSAGE);
				break;

#if (HDMI_PREVIEW_EN)
				case AHC_HDMI_VIDEO_PREVIEW_STATUS:
	            	StateVideoRecMode(EVENT_VIDREC_UPDATE_MESSAGE);
	            break;

	            case AHC_HDMI_DSC_PREVIEW_STATUS:
	            	StateCaptureMode(EVENT_DSC_KEY_UPDATE_MESSAGE);
	            break;
#endif
			}
		break;

#if (HDMI_PREVIEW_EN)
        //Record Callback for Video Record
        case EVENT_VRCB_MEDIA_FULL	:
        case EVENT_VRCB_FILE_FULL	:
        case EVENT_VRCB_MEDIA_SLOW	:
        case EVENT_VRCB_SEAM_LESS	:
        case EVENT_VRCB_MEDIA_ERROR	:
       	case EVENT_VR_WRITEINFO		:
       		if(HDMIFunc_Status()==AHC_HDMI_VIDEO_PREVIEW_STATUS)
       			StateVideoRecMode(ulJobEvent);
       	break;

       	case EVENT_VRCB_MOTION_DTC	:
       		if(HDMIFunc_Status()==AHC_HDMI_VIDEO_PREVIEW_STATUS)
       			StateVideoRecMode(ulJobEvent);
       		else if(HDMIFunc_Status()==AHC_HDMI_DSC_PREVIEW_STATUS)
       			StateCaptureMode(ulJobEvent);
       	break;

       	//Update Message
       	case EVENT_SHOW_FW_VERSION:
        case EVENT_GPSGSENSOR_UPDATE:
        	if(HDMIFunc_Status()==AHC_HDMI_VIDEO_PREVIEW_STATUS)
       			StateVideoRecMode(ulJobEvent);
       	break;
#endif

        default:
        break;
    }
#endif
}

#else

void StateHDMIMode(UINT32 ulEvent)
{
#if (HDMI_ENABLE)

	AHC_BOOL 		ret = 0;

	if(ulEvent == EVENT_NONE)
	    return;

    if(!HDMIFunc_MoviePBCheckOSDShowUp(ulEvent))
        return;

    if(!HDMIFunc_PhotoPBCheckOSDShowUp(ulEvent))
        return;

    if(!HDMIFunc_SlideShowCheckOSDShowUp(ulEvent))
        return;

    switch(ulEvent)
    {
        case EVENT_NONE                           :
        break;

   		//Device
		case EVENT_DC_CABLE_IN:
			if(m_HdmiStatus==AHC_HDMI_VIDEO_PREVIEW_STATUS)
			{
				#if (CHARGER_IN_ACT_VIDEO_MODE==ACT_RECORD_VIDEO)
				if(VideoFunc_RecordStatus()==AHC_FALSE)
					AHC_SetRecordByChargerIn(3);
				#elif (CHARGER_IN_ACT_VIDEO_MODE == ACT_FORCE_POWER_OFF)
				SetKeyPadEvent(KEY_POWER_OFF);
				#endif
			}
		break;

		case EVENT_DC_CABLE_OUT:
			AHC_SetChargerEnable(AHC_FALSE);

			if(m_HdmiStatus==AHC_HDMI_VIDEO_PREVIEW_STATUS)
			{
		    	if( VideoFunc_RecordStatus() )
				{
					#if (CHARGER_OUT_ACT_VIDEO_REC==ACT_FORCE_POWER_OFF || CHARGER_OUT_ACT_VIDEO_REC==ACT_DELAY_POWER_OFF)
					AHC_SetShutdownByChargerOut(AHC_TRUE);
					#endif
				}
				else
				{
					#if (CHARGER_OUT_ACT_OTHER_MODE==ACT_FORCE_POWER_OFF || CHARGER_OUT_ACT_OTHER_MODE==ACT_DELAY_POWER_OFF)
		 			AHC_SetShutdownByChargerOut(AHC_TRUE);
					#endif
				}
			}
			else
			{
			    #if (CHARGER_OUT_ACT_OTHER_MODE==ACT_FORCE_POWER_OFF || CHARGER_OUT_ACT_OTHER_MODE==ACT_DELAY_POWER_OFF)
				AHC_SetShutdownByChargerOut(AHC_TRUE);
				#endif
			}
		break;

		case EVENT_USB_DETECT                     :
			if(IsAdapter_Connect())
			{
				if(m_HdmiStatus==AHC_HDMI_VIDEO_PREVIEW_STATUS)
				{
					#if (CHARGER_IN_ACT_VIDEO_MODE==ACT_RECORD_VIDEO)
					if(VideoFunc_RecordStatus()==AHC_FALSE)
						AHC_SetRecordByChargerIn(3);
					#elif (CHARGER_IN_ACT_VIDEO_MODE == ACT_FORCE_POWER_OFF)
					SetKeyPadEvent(KEY_POWER_OFF);
					#endif
				}
			}
			else
			{
				#ifdef	USB_PLUGIN_SW_DISP_LCD
				AHC_OSDUninit();
	            AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);

				// Lock HDMI state at non-plugged state when USB plug-in
				// Will be unlock when USB plug-out
				AHC_LockHdmiConnectionStatus(AHC_TRUE, AHC_FALSE);

    	        InitOSD();

				// Exit HDMI mode
				HDMIFunc_Enable(AHC_FALSE);
				m_HdmiStatus = AHC_HDMI_NONE_STATUS;
				#endif
    	        #if (USB_MODE_SELECT_EN)
				if (MMPS_USB_NORMAL == MMPS_USB_GetStatus()) {
				    StateSwitchMode(UI_USBSELECT_MENU_STATE);
				}
				else
				{
					#ifdef CFG_POWER_ON_ENTER_CAMERA_STATE
					StateSwitchMode(UI_CAMERA_STATE);
					#else
					StateSwitchMode(UI_VIDEO_STATE);
					#endif
				}
				#else
				if(MenuSettingConfig()->uiUSBFunction == USB_MSDC)
				{
                	ret = StateSwitchMode(UI_MSDC_STATE);
				}
				else
				{
					ret = StateSwitchMode(UI_PCCAM_STATE);
				}
				#endif
			}
        break;

        case EVENT_USB_REMOVED                    :

			if(AHC_USB_GetLastStatus() == AHC_USB_NORMAL) // MSDC mode
			{
				AHC_SetMode(AHC_MODE_IDLE);
				bForce_PowerOff = AHC_TRUE;
				AHC_NormalPowerOffPath();
			}
			else
			{
                if (AHC_IsDcCableConnect() == AHC_TRUE)
                    break;

		     	AHC_SetChargerEnable(AHC_FALSE);

				if(m_HdmiStatus==AHC_HDMI_VIDEO_PREVIEW_STATUS)
				{
			    	if( VideoFunc_RecordStatus() )
					{
						#if (CHARGER_OUT_ACT_VIDEO_REC==ACT_FORCE_POWER_OFF || CHARGER_OUT_ACT_VIDEO_REC==ACT_DELAY_POWER_OFF)
						AHC_SetShutdownByChargerOut(AHC_TRUE);
						#endif
					}
					else
					{
						#if (CHARGER_OUT_ACT_OTHER_MODE==ACT_FORCE_POWER_OFF || CHARGER_OUT_ACT_OTHER_MODE==ACT_DELAY_POWER_OFF)
			 			AHC_SetShutdownByChargerOut(AHC_TRUE);
						#endif
					}
				}
				else
				{
			     	#if (CHARGER_OUT_ACT_OTHER_MODE==ACT_FORCE_POWER_OFF || CHARGER_OUT_ACT_OTHER_MODE==ACT_DELAY_POWER_OFF)
					AHC_SetShutdownByChargerOut(AHC_TRUE);
					#endif
				}
			}
        break;

        case EVENT_SD_DETECT                      :
            AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect);
            AHC_OS_Sleep(100);

			#if 1 // TBD
            if(!HDMIFunc_IsPreview())
            {
            	m_HdmiStatus = AHC_HDMI_NONE_STATUS;
            	HDMIFunc_Enable(AHC_TRUE);
        	}
			#endif
        break;

        case EVENT_SD_REMOVED                     :
			switch(m_HdmiStatus)
			{

#if (HDMI_PREVIEW_EN)
				case AHC_HDMI_VIDEO_PREVIEW_STATUS:
					if(VideoFunc_RecordStatus())
           		 	{
                		StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
            		}
				break;
#endif
				case AHC_HDMI_MOVIE_PB_STATUS:

					if(MOV_STATE_FF == GetMovConfig()->iState)
					{
						MovPlayback_NormalSpeed();
             			AHC_OS_Sleep(100);
					}

             		AHC_ResetVideoPlayParam();

					AHC_SetMode(AHC_MODE_IDLE);

					MovPBTimer_Stop();
					m_bMovieOSDForceShowUp = AHC_FALSE;
					MovPlaybackParamReset();
				break;

				case AHC_HDMI_PHOTO_PB_STATUS:
					PhotoPBTimer_Stop();
					JpgPlaybackParamReset();
				break;
			}

			m_DelFile = DEL_FILE_NONE;
			ObjSelect = 0;

			if(AHC_TRUE == AHC_SDMMC_GetMountState())
			{
				AHC_DisMountStorageMedia(AHC_MEDIA_MMC);
				AHC_OS_Sleep(100);
				Enable_SD_Power(0 /* Power Off */);
			}

			if(!bPreviewModeHDMI)
			{
				m_HdmiStatus = AHC_HDMI_NONE_STATUS;
				HDMIFunc_Enable(AHC_TRUE);
			}
		break;

		case EVENT_HDMI_DETECT                    :
		break;

		case EVENT_HDMI_REMOVED                   :
        case EVENT_HDMI_KEY_LCD_OUTPUT            :

        	bHdmiMenuMode = AHC_FALSE;

			switch(m_HdmiStatus)
			{
				case AHC_HDMI_BROWSER_STATUS:
					if(AHC_IsTVConnect())
					{ //HDMI 2 TV
						SetTVState(AHC_TV_NONE_STATUS);
	                    StateSwitchMode(UI_TVOUT_STATE);
	                }
	                else
	                {
	                	AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_MAIN_LCD);
	                    HDMIFunc_Enable(AHC_FALSE);
	                }
	                m_HdmiStatus = AHC_HDMI_NONE_STATUS;
	            break;

	            case AHC_HDMI_MOVIE_PB_STATUS:
					if(AHC_IsTVConnect())
					{ //HDMI 2 TV
						AHC_GetVideoCurrentPlayTime(&m_VideoCurPlayTime);
	                    MovPBTimer_Stop();
	                    MovPlaybackParamReset();
	                    AHC_SetVideoPlayStartTime(m_VideoCurPlayTime);
	                    SetTVState(AHC_TV_MOVIE_PB_STATUS);
	                    StateSwitchMode(UI_TVOUT_STATE);
	                }
	                else
	                { //HDMI to LCD
	                	AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_MAIN_LCD);
	                    AHC_GetVideoCurrentPlayTime(&m_VideoCurPlayTime);
	                    AHC_OSDUninit();
	                    AHC_SetMode(AHC_MODE_IDLE);
	                   
	                    AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
	                    InitOSD();
	                    AHC_UF_GetCurrentIndex(&ObjSelect);
	                    AHC_SetVideoPlayStartTime(m_VideoCurPlayTime);

	                    m_HdmiStatus = AHC_HDMI_NONE_STATUS;
	                    bMovPbExitFromHdmi = AHC_TRUE;
	                    UpdateMovPBStatus(bHdmiMovieState);
						#ifdef CFG_HDMI_PLUGOUT_RETURN_VIDEO_PREVIEW
						StateSwitchMode(UI_VIDEO_STATE);
						#else
	                    StateSwitchMode(UI_PLAYBACK_STATE);
						#endif
	                    RTNA_LCD_Backlight(MMP_TRUE);
	            	}
	            break;

	            case AHC_HDMI_PHOTO_PB_STATUS:
	            	if(AHC_IsTVConnect())
	            	{ //HDMI 2 TV
	                	PhotoPBTimer_Stop();
	                    JpgPlaybackParamReset();
	                    SetTVState(AHC_TV_PHOTO_PB_STATUS);
	                    StateSwitchMode(UI_TVOUT_STATE);
	                }
	                else
	                { //HDMI to LCD
	                	AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_MAIN_LCD);
	                	AHC_OSDUninit();
	                    AHC_SetMode(AHC_MODE_IDLE);

	                    AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
	                    InitOSD();
	                    AHC_UF_GetCurrentIndex(&ObjSelect);
	                    m_HdmiStatus = AHC_HDMI_NONE_STATUS;
						#ifdef CFG_HDMI_PLUGOUT_RETURN_VIDEO_PREVIEW
						StateSwitchMode(UI_VIDEO_STATE);
						#else
	                    StateSwitchMode(UI_PLAYBACK_STATE);
						#endif
	                    RTNA_LCD_Backlight(MMP_TRUE);
	                }
				break;

				case AHC_HDMI_SLIDE_SHOW_STATUS:
	                if(AHC_IsTVConnect())
	                { //HDMI 2 TV
						AHC_GetVideoCurrentPlayTime(&m_VideoCurPlayTime);
						SlideShowTimer_Stop();
						SlideShowFunc_Stop();
						AHC_SetVideoPlayStartTime(m_VideoCurPlayTime);
						SetTVState(AHC_TV_SLIDE_SHOW_STATUS);
						StateSwitchMode(UI_TVOUT_STATE);
	                }
	                else
	                { //HDMI to LCD
						AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_MAIN_LCD);
						AHC_GetVideoCurrentPlayTime(&m_VideoCurPlayTime);
						AHC_OSDUninit();
						AHC_SetMode(AHC_MODE_IDLE);

						AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
						InitOSD();
						AHC_UF_GetCurrentIndex(&ObjSelect);
						AHC_SetVideoPlayStartTime(m_VideoCurPlayTime);
						bSlideShowFirstFile = AHC_FALSE;
						m_HdmiStatus = AHC_HDMI_NONE_STATUS;
						StateSwitchMode(UI_SLIDESHOW_STATE);
						RTNA_LCD_Backlight(MMP_TRUE);
					}
				break;

				#if (HDMI_PREVIEW_EN)
				case AHC_HDMI_VIDEO_PREVIEW_STATUS:

					if(VideoFunc_RecordStatus())
					{
						StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
					}

					VideoTimer_Stop();
					
                    AHC_OSDUninit(); 
					AHC_SetMode(AHC_MODE_IDLE);

					AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
					InitOSD();
					m_HdmiStatus = AHC_HDMI_NONE_STATUS;

					AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_MAIN_LCD);
					bPreviewModeHDMI = AHC_FALSE;

					StateSwitchMode(UI_VIDEO_STATE);
					RTNA_LCD_Backlight(MMP_TRUE);
				break;

				case AHC_HDMI_DSC_PREVIEW_STATUS:
					CaptureTimer_Stop();
                    
                    AHC_OSDUninit();
					AHC_SetMode(AHC_MODE_IDLE);

					AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
					InitOSD();
					m_HdmiStatus = AHC_HDMI_NONE_STATUS;

					AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_MAIN_LCD);
					bPreviewModeHDMI = AHC_FALSE;

					StateSwitchMode(UI_CAMERA_STATE);
					RTNA_LCD_Backlight(MMP_TRUE);
				break;
				case AHC_HDMI_NET_PLAYBACK_STATUS:
					AHC_OSDUninit();
					AHC_SetMode(AHC_MODE_IDLE); 

					AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
					InitOSD();
					m_HdmiStatus = AHC_HDMI_NONE_STATUS;
					
					AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_MAIN_LCD);
					bPreviewModeHDMI = AHC_FALSE;
					
					StateSwitchMode(UI_NET_PLAYBACK_STATE);
					RTNA_LCD_Backlight(MMP_TRUE);
				break;
				#endif
			}

			#if (POWER_OFF_CONFIRM_PAGE_EN)
			if(PowerOff_InProcess)
			{
				MenuDrawSubPage_PowerOff();
			}
			#endif
        break;

        default:

	 		switch(m_HdmiStatus)
			{
				case AHC_HDMI_VIDEO_PREVIEW_STATUS:
					StateHDMI_VideoMode_Handler(ulEvent);
				break;
				case AHC_HDMI_DSC_PREVIEW_STATUS:
					StateHDMI_CameraMode_Handler(ulEvent);
				break;
				case AHC_HDMI_BROWSER_STATUS:
					StateHDMI_BrowserMode_Handler(ulEvent);
				break;
				case AHC_HDMI_MOVIE_PB_STATUS:
					StateHDMI_MoviePBMode_Handler(ulEvent);
				break;
				case AHC_HDMI_PHOTO_PB_STATUS:
					StateHDMI_PhotoPBMode_Handler(ulEvent);
				break;
				case AHC_HDMI_SLIDE_SHOW_STATUS:
					StateHDMI_SlideShowMode_Handler(ulEvent);
				break;
				case AHC_HDMI_NET_PLAYBACK_STATUS:
					StateHDMI_NetPlayback_Handler(ulEvent);
				default:
				break;
			}
        break;
    }
#endif
}

#endif

/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "customer_config.h"
#include "AHC_Common.h"
#include "AHC_General.h"
#include "AHC_Display.h"
#include "AHC_Menu.h"
#include "AHC_DCF.h"
#include "AHC_UF.h"
#include "AHC_Browser.h"
#include "AHC_Parameter.h"
#include "AHC_Warningmsg.h"
#include "AHC_USB.h"
#include "AHC_Media.h"
#include "AHC_OS.h"
#include "AHC_ISP.h"
#include "AHC_General_CarDV.h"
#include "KeyParser.h"
#include "DrawStateTVFunc.h"
#include "DrawStateHDMIFunc.h"
#include "DrawStateVideoFunc.h"
#include "DrawStateCameraFunc.h"
#include "DrawStateSlideShowFunc.h"
#include "DrawStateMoviePBFunc.h"
#include "Menusetting.h"
#include "MenuCommon.h"
#include "MenuDrawCommon.h"
#include "MenuStateMenu.h"
#include "MediaPlaybackCtrl.h"
#include "StateTVFunc.h"
#include "StateHDMIFunc.h"
#include "StatePhotoPBFunc.h"
#include "StateMoviePBFunc.h"
#include "StateSlideShowFunc.h"
#include "StateVideoFunc.h"
#include "StateCameraFunc.h"
#include "StateBrowserFunc.h"
#include "dsc_charger.h"
#include "LedControl.h"
#include "mmps_display.h"
#include "mmps_sensor.h"
#include "mmps_iva.h"
#include "ColorDefine.h"
#include "IconPosition.h"
#include "IconDefine.h"
#include "ZoomControl.h"
#include "UI_DrawGeneral.h"
#include "lib_retina.h"
#include "hdr_cfg.h"
#include "ldws_cfg.h"
#include "MenuStatePlaybackMenu.h"
#ifdef	_OEM_UI_
#include "Oem_UI.h"
#endif

#if defined(WIFI_PORT) && (WIFI_PORT == 1)
#include "amnss_mmobjs.h"
#include "wlan.h"
#include "netapp.h"
#endif

/*===========================================================================
 * Macro define
 *===========================================================================*/
#define VIDEO_TIMER_UNIT                    (100)//unit :ms

#define TV_OSD_COLOR_FMT                    (OSD_COLOR_ARGB32)//(OSD_COLOR_RGB565)
#define TV_MENU_COLOR_FMT                   (OSD_COLOR_ARGB32)//(OSD_COLOR_RGB565)

/*===========================================================================
 * Structure define
 *===========================================================================*/ 

typedef struct _AHC_TV_SMALL_OSD{
	UINT16 uwDisplayID;
	UINT16 uwWidth;
	UINT16 uwHeight;
	UINT16 uwDisplayOffsetX;
	UINT16 uwDisplayOffsetY;
} AHC_TV_SMALL_OSD;

/*===========================================================================
 * Global varible
 *===========================================================================*/ 

#if (TVOUT_ENABLE)
static AHC_THUMB_CFG   	m_ThumbConfig;
static AHC_TV_SMALL_OSD TVSmallOSD[2];
#endif
AHC_TV_STATUS 	        m_TVStatus 			= AHC_TV_NONE_STATUS;

UINT16					m_uwTVPrevOsdId;
AHC_BOOL 				bPreviewModeTVout 	= AHC_FALSE;
UINT8 					bTVMovieState 	  	= MOVIE_STATE_STOP;

/*===========================================================================
 * Extern varible
 *===========================================================================*/ 

extern UINT32   	ObjSelect;
extern AHC_BOOL 	m_bMovieOSDForceShowUp; 
extern UINT8 		m_DelFile;
extern UINT32 		m_VideoCurPlayTime;
extern AHC_BOOL 	bEnterMenuPause;
extern UINT32		m_MovPlayFFSpeed;
extern UINT32		m_MovPlayREWSpeed;
extern AHC_BOOL		bMovPbExitFromTV;
extern AHC_BOOL		bMovPbEnterToTV;
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
extern AHC_BOOL 	m_ubInTVMode;
extern MMP_USHORT	gsStillZoomIndex;
extern UINT8 		m_ubDSCMode;
extern AHC_BOOL		m_ubTimeLapseStart;
extern AHC_BOOL		bMuteRecord; 

#if (VR_PREENCODE_EN)	
extern AHC_BOOL		bDisableVideoPreRecord;
#endif

#if (LOCK_NEXTFILE_AT_VR_RESTART)
extern AHC_BOOL		m_ubLockNextFile;
#endif

extern MOVIESIZE_SETTING 	VideoRecSize;
extern IMAGESIZE_SETTING	CaptureResolution;	
extern SMENUSTRUCT 			sSubDelete;

#if (MOTION_DETECTION_EN)
extern AHC_BOOL 	m_ubMotionDtcEn;
extern UINT32		m_ulVMDRemindTime;
extern AHC_BOOL		m_ubInRemindTime;
extern AHC_BOOL		m_ubVMDStart;
extern AHC_BOOL		m_ulVMDCancel;
extern UINT32       m_ulVMDStopCnt;
#endif

#if (VR_PREENCODE_EN)
extern UINT8        bVideoPreRecordStatus;
extern AHC_BOOL     m_ubPreRecording;
extern AHC_BOOL     m_ubPreEncodeEn;
#endif

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
extern AHC_BOOL VideoFunc_TriggerEmerRecord(void);

/*===========================================================================
 * Local function
 *===========================================================================*/ 

AHC_BOOL 	TVFunc_ThumbnailPlayback(void);
AHC_BOOL 	TVFunc_ThumbnailOption(UINT8 ubOp);
AHC_BOOL 	TVFunc_SlideShow(void);
void 		StateTV_EventMenu_Handler(UINT32 ulJobEvent);

/*===========================================================================
 * Main body
 *===========================================================================*/ 

AHC_TV_STATUS TVFunc_Status(void)
{
    return m_TVStatus;
}

AHC_BOOL TVFunc_IsInMenu(void)
{
    return IN_MENU();
}

AHC_BOOL TVFunc_IsPreview(void)
{
	if(!AHC_IsTVConnectEx())
		return AHC_FALSE;

	if(m_TVStatus == AHC_TV_VIDEO_PREVIEW_STATUS ||
	   m_TVStatus == AHC_TV_DSC_PREVIEW_STATUS )
    	return AHC_TRUE;
    else
    	return AHC_FALSE;
}

AHC_BOOL TVFunc_IsPlayback(void)
{
	if(!AHC_IsTVConnectEx())
		return AHC_FALSE;

	if(m_TVStatus == AHC_TV_MOVIE_PB_STATUS ||
	   m_TVStatus == AHC_TV_PHOTO_PB_STATUS ||
	   m_TVStatus == AHC_TV_SLIDE_SHOW_STATUS)
    	return AHC_TRUE;
    else
    	return AHC_FALSE;
}

AHC_BOOL TVFunc_IsInTVMode(void)
{
	if(AHC_IsTVConnectEx() && (m_TVStatus!=AHC_TV_NONE_STATUS))
		return AHC_TRUE;
	else
		return AHC_FALSE;
}

AHC_BOOL TVFunc_CheckTVStatus(UINT8 ubCheckStatus)
{
	if(AHC_IsTVConnectEx() && (m_TVStatus==ubCheckStatus))
		return AHC_TRUE;
	else
		return AHC_FALSE;
}

AHC_BOOL TVFunc_CheckTVSystemChanged(void)
{
	AHC_DISPLAY_OUTPUTPANEL pCurDevice;
	
    AHC_GetDisplayOutputDev(&pCurDevice);    
    if((MenuSettingConfig()->uiTVSystem == TV_SYSTEM_PAL) && (pCurDevice != AHC_DISPLAY_PAL_TV))
        return AHC_TRUE;
    else if((MenuSettingConfig()->uiTVSystem == TV_SYSTEM_NTSC) && (pCurDevice != AHC_DISPLAY_NTSC_TV))
        return AHC_TRUE;
        
    return AHC_FALSE;    
}

void TVFunc_SetTVSystem(void)
{
	switch(MenuSettingConfig()->uiTVSystem) 
    {
	    case TV_SYSTEM_PAL:
	        AHC_SetDisplayOutputDev(DISP_OUT_TV_PAL, AHC_DISPLAY_DUPLICATE_1X);
			AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_PAL_TV);
	    break;	    	    
	    case TV_SYSTEM_NTSC:
	    default:
	        AHC_SetDisplayOutputDev(DISP_OUT_TV_NTSC, AHC_DISPLAY_DUPLICATE_1X);
			AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_NTSC_TV);
		break;
    }    
}
/*
 * Scale up the Window of MENU OSD, for TV used
 */
#if (TV_MENU_EN) && (TVOUT_PREVIEW_EN)
void Scale_MenuOSD_Window(int xc)
{
	UINT32	pos[2];

	pos[0] = xc;
	pos[1] = xc;
	AHC_OSDSetDisplayAttr(TV_MENU_OSD_ID, AHC_OSD_ATTR_DISPLAY_SCALE, pos);
	AHC_OSDSetDisplayAttr(TV_MENU_OSD2_ID, AHC_OSD_ATTR_DISPLAY_SCALE, pos);
}
#endif

#if 0
void ____TV_OSD_Init_Function_____(){ruturn;} //dummy
#endif

UINT16 TVFunc_GetUImodeOsdID(void)
{
	return TV_UI_OSD_ID;
}

UINT16 TVFunc_GetMenuOsdID(void)
{
	return TV_MENU_OSD_ID;
}

#if (TVOUT_ENABLE)

UINT16 TVFunc_InitPreviewOSD(void)
{
    UINT16		uwTVWidth, uwTVHeight;
    UINT16		uwTVOffX, uwTVOffY;
	UINT32		pos[2];

    AHC_OSDUninit();
	AHC_SetMode(AHC_MODE_IDLE); 
	    	
	AHC_InitTV(AHC_FALSE);//TBD  
    
    if(TVFunc_CheckTVSystemChanged())
        TVFunc_SetTVSystem(); 

    AHC_Display_GetWidthHdight(&uwTVWidth, &uwTVHeight);       
    switch(MenuSettingConfig()->uiTVSystem) 
    {
	    case TV_SYSTEM_PAL:
			uwTVOffX = 0;
			uwTVOffY = 0;//48;
	    break;	    	    
	    case TV_SYSTEM_NTSC:
	    default:
			uwTVOffX = 0;
			uwTVOffY = 0;
		break;
    }
   	AHC_InitTV(AHC_TRUE);  
	pos[0] = uwTVOffX; 
	pos[1] = uwTVOffY;

	AIHC_SetGUIMemStartAddr(AHC_GUI_TEMP_BASE_ADDR);
	
    AHC_OSDInit(0, 0, 0, 0,
                 PRIMARY_DATESTAMP_WIDTH, PRIMARY_DATESTAMP_HEIGHT,
                 THUMB_DATESTAMP_WIDTH, THUMB_DATESTAMP_HEIGHT,
                 WMSG_LAYER_WIN_COLOR_FMT, AHC_FALSE);

    //** Set TV OSD#1 by uwDisplayID = 18,19
    {
	UINT32		uwDispScale = 0;
	UINT16		uwTVTempWidth, uwTVTempHeight;	

	uwDispScale = OSD_DISPLAY_SCALE_TV;

	uwTVTempWidth = uwTVWidth / OSD_DISPLAY_SCALE_TV;
	uwTVTempHeight = uwTVHeight / OSD_DISPLAY_SCALE_TV;

	//** Set TV OSD#1 by uwDisplayID = 18,19
	AHC_OSDCreateBuffer(TV_MENU_OSD_ID, uwTVTempWidth, uwTVTempHeight, OSD_COLOR_RGB565);
	AHC_OSDCreateBuffer(TV_MENU_OSD2_ID, uwTVTempWidth, uwTVTempHeight, OSD_COLOR_RGB565);
	pos[0] = uwTVOffX;
	pos[1] = uwTVOffY;
	AHC_OSDSetDisplayAttr(TV_MENU_OSD_ID, AHC_OSD_ATTR_DISPLAY_OFFSET, pos);
	AHC_OSDSetDisplayAttr(TV_MENU_OSD2_ID, AHC_OSD_ATTR_DISPLAY_OFFSET, pos);
	pos[0] = uwDispScale - 1;
	pos[1] = uwDispScale - 1;
	AHC_OSDSetDisplayAttr(TV_MENU_OSD_ID, AHC_OSD_ATTR_DISPLAY_SCALE, pos);
	AHC_OSDSetDisplayAttr(TV_MENU_OSD2_ID, AHC_OSD_ATTR_DISPLAY_SCALE, pos);
   	pos[0] = OSD_FLIP_DRAW_NONE_ENABLE;                
   	AHC_OSDSetDisplayAttr(TV_MENU_OSD_ID,  AHC_OSD_ATTR_FLIP_BY_GUI, pos);
   	AHC_OSDSetDisplayAttr(TV_MENU_OSD2_ID,  AHC_OSD_ATTR_FLIP_BY_GUI, pos);
   	pos[0] = OSD_ROTATE_DRAW_NONE;
   	AHC_OSDSetDisplayAttr(TV_MENU_OSD_ID,  AHC_OSD_ATTR_ROTATE_BY_GUI, pos);
   	AHC_OSDSetDisplayAttr(TV_MENU_OSD2_ID,  AHC_OSD_ATTR_ROTATE_BY_GUI, pos);
	AHC_OSDClearBuffer(TV_MENU_OSD_ID);;
	AHC_OSDClearBuffer(TV_MENU_OSD2_ID);
    }
	
    //** Set TV OSD#2 by uwDisplayID = 17
    AHC_OSDCreateBuffer(TV_UI_OSD_ID, uwTVWidth, uwTVHeight, TV_OSD_COLOR_FMT);
   	pos[0] = uwTVOffX; 
	pos[1] = uwTVOffY;
	AHC_OSDSetDisplayAttr(TV_UI_OSD_ID, AHC_OSD_ATTR_DISPLAY_OFFSET, pos);
    pos[0] = AHC_TRUE;
    pos[1] = OSD_COLOR_TRANSPARENT;
    AHC_OSDSetDisplayAttr(TV_UI_OSD_ID, AHC_OSD_ATTR_TRANSPARENT_ENABLE, pos);
    pos[0] = OSD_FLIP_DRAW_NONE_ENABLE;                
    AHC_OSDSetDisplayAttr(TV_UI_OSD_ID,  AHC_OSD_ATTR_FLIP_BY_GUI, pos);
	pos[0] = OSD_ROTATE_DRAW_NONE;
	AHC_OSDSetDisplayAttr(TV_UI_OSD_ID,  AHC_OSD_ATTR_ROTATE_BY_GUI, pos);

    BEGIN_LAYER(TV_UI_OSD_ID);
	
	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	AHC_OSDSetColor(TV_UI_OSD_ID, OSD_COLOR_TRANSPARENT);
	#else
	AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
	#endif
	
	AHC_OSDDrawFillRect(TV_UI_OSD_ID, 0, 0, uwTVWidth, uwTVHeight);
	
    AHC_OSDSetCurrentDisplay(TV_UI_OSD_ID);

    END_LAYER(TV_UI_OSD_ID);
    
	AHC_PreviewOnDisplayEx(AHC_TRUE);

    return TV_UI_OSD_ID;
}

void TVFunc_InitBrowserSmallOSD(void)
{
	RECT  OsdRect;
	UINT8 i;
	
	switch(MenuSettingConfig()->uiTVSystem) 
	{
	    case TV_SYSTEM_NTSC:
	        OsdRect.uiLeft 	 = POS_TV_NTSC_BROWSER_OSD_X0; 
	        OsdRect.uiTop 	 = POS_TV_NTSC_BROWSER_OSD_Y0;
	        OsdRect.uiWidth  = POS_TV_NTSC_BROWSER_OSD_W;
	        OsdRect.uiHeight = POS_TV_NTSC_BROWSER_OSD_H;
	    break;
	    
	    case TV_SYSTEM_PAL:
	        OsdRect.uiLeft 	 = POS_TV_PAL_BROWSER_OSD_X0; 
	        OsdRect.uiTop 	 = POS_TV_PAL_BROWSER_OSD_Y0;
	        OsdRect.uiWidth  = POS_TV_PAL_BROWSER_OSD_W;
	        OsdRect.uiHeight = POS_TV_PAL_BROWSER_OSD_H;  
	    break;
	    
	    default:
	        printc("TVFunc_InitBrowserSmallOSD Error!!\r\n");
	    break;
    }
	   
    TVSmallOSD[0].uwDisplayID 	= 16;
    TVSmallOSD[1].uwDisplayID	= 17;
    
    for (i=0; i<2; i++)
    {
	    TVSmallOSD[i].uwWidth  			= OsdRect.uiWidth;
	    TVSmallOSD[i].uwHeight 			= OsdRect.uiHeight;   
	    TVSmallOSD[i].uwDisplayOffsetX 	= 0;    
    
	    switch(MenuSettingConfig()->uiTVSystem) 
	    {
		    case TV_SYSTEM_NTSC:
		        TVSmallOSD[i].uwDisplayOffsetY = 20;
		    break;
		    case TV_SYSTEM_PAL:
		        TVSmallOSD[i].uwDisplayOffsetY = 68;
		    break;
	    }
    }
}

void TVFunc_InitPlaySmallOSD(void)
{
    UINT16 	uwWidth, uwHeight;
	UINT8 	i;
    RECT  	OsdRect  = POS_TV_PLAY_OSD;

    switch(MenuSettingConfig()->uiTVSystem) 
    {
	    case TV_SYSTEM_NTSC:
	        AHC_GetNtscTvDisplayWidthHeight(&uwWidth, &uwHeight);
	    break;
	    case TV_SYSTEM_PAL:
	        AHC_GetPalTvDisplayWidthHeight(&uwWidth, &uwHeight);
	    break;
	    default:
	        printc("TVFunc_InitPlaySmallOSD Error\r\n");
	    break;
    }

    TVSmallOSD[0].uwDisplayID 	= 16;
    TVSmallOSD[1].uwDisplayID 	= 17;

	for (i=0; i<2; i++)
	{
    	TVSmallOSD[i].uwWidth 	= OsdRect.uiWidth;
    	TVSmallOSD[i].uwHeight 	= OsdRect.uiHeight;
    
	    switch(MenuSettingConfig()->uiTVSystem) 
	    {
		    case TV_SYSTEM_NTSC:
		        TVSmallOSD[i].uwDisplayOffsetX = uwWidth- TVSmallOSD[i].uwWidth-20;
		        TVSmallOSD[i].uwDisplayOffsetY = 20;
		    break;
		    case TV_SYSTEM_PAL:
		        TVSmallOSD[i].uwDisplayOffsetX = uwWidth- TVSmallOSD[i].uwWidth-40;
		        TVSmallOSD[i].uwDisplayOffsetY = 68;
		    break;
	    }
	}
}

AHC_BOOL TVFunc_SetSmallOSDDisplayOffset(UINT8 ubSmaillOSDID)
{
    UINT32 iVal[2];
    
    iVal[0] = TVSmallOSD[ubSmaillOSDID].uwDisplayOffsetX;
    iVal[1] = TVSmallOSD[ubSmaillOSDID].uwDisplayOffsetY;
    return AHC_OSDSetDisplayAttr(TVSmallOSD[ubSmaillOSDID].uwDisplayID, AHC_OSD_ATTR_DISPLAY_OFFSET, iVal);
}   

void TVFunc_ChangeOSDStatus(AHC_TV_STATUS TVStatus)
{
    U32 		ulWidth,   ulHeight;
    UINT16 		uwTVWidth, uwTVHeight;
    UINT16		uwTVOffX,  uwTVOffY;
    
    AHC_OSDUninit();
    AHC_SetMode(AHC_MODE_IDLE); 
       
    if(TVFunc_CheckTVSystemChanged())
        TVFunc_SetTVSystem();
                    
    AHC_Display_GetWidthHdight(&uwTVWidth, &uwTVHeight);       
    switch(MenuSettingConfig()->uiTVSystem) 
    {
	    case TV_SYSTEM_NTSC:
	        ulWidth 	= uwTVWidth;
	        ulHeight 	= uwTVHeight-POS_TV_NTSC_BROWSER_OSD_H;
			uwTVOffX 	= 0;
			uwTVOffY 	= 0;	    
	    break;
	    
	    case TV_SYSTEM_PAL:
	        ulWidth 	= uwTVWidth;
	        ulHeight	= uwTVHeight-POS_TV_PAL_BROWSER_OSD_H;
	        uwTVOffX 	= 0;
			uwTVOffY 	= 48; 
	    break;    	    
	    default:
	        printc("TVFunc_ChangeOSDStatus Error!\r\n");
	    break;
    }

    if((TVStatus == AHC_TV_PHOTO_PB_STATUS) || (TVStatus == AHC_TV_SLIDE_SHOW_STATUS)) {
        AIHC_SetGUIMemStartAddr(AHC_GUI_TEMP_BASE_ADDR); 
    }
    else {
        AIHC_SetGUIMemStartAddr(AHC_GUI_TEMP_BASE_ADDR); 
    }

    switch(TVStatus) 
    {
	    case AHC_TV_BROWSER_STATUS:       
	        TVFunc_InitBrowserSmallOSD();
        	AHC_OSDInit(	ulWidth, 
        					ulHeight, 
        					ulWidth,
        					ulHeight,
        					PRIMARY_DATESTAMP_WIDTH, 
        					PRIMARY_DATESTAMP_HEIGHT, 
        					0, 
        					0, 
        					WMSG_LAYER_WIN_COLOR_FMT, AHC_FALSE);
	    break;
	    case AHC_TV_MOVIE_PB_STATUS: 
	    case AHC_TV_PHOTO_PB_STATUS:
		case AHC_TV_ALL_PB_STATUS:
	    case AHC_TV_SLIDE_SHOW_STATUS:
	    case AHC_TV_SLEEP_STATUS:
	        TVFunc_InitPlaySmallOSD();
	        AHC_OSDInit(0, 0, 0, 0, 0, 0, 0, 0, WMSG_LAYER_WIN_COLOR_FMT, AHC_FALSE);
	    break;
    }

	#if (TV_MENU_EN)
	{
		UINT32	pos[2];
	
		printc("MENU %d %d\r\n", ulWidth, ulHeight);
		AHC_OSDCreateBuffer(TV_MENU_OSD_ID, TV_MENU_WIDTH, TV_MENU_HEIGHT, OSD_COLOR_RGB565); 
		AHC_OSDCreateBuffer(TV_MENU_OSD2_ID, TV_MENU_WIDTH, TV_MENU_HEIGHT, OSD_COLOR_RGB565); 
		pos[0] = uwTVOffX + (ulWidth - TV_MENU_WIDTH * 2) / 2; 
		pos[1] = uwTVOffY + (480/*ulHeight*/ - TV_MENU_HEIGHT * 2) / 2;
		AHC_OSDSetDisplayAttr(TV_MENU_OSD_ID, AHC_OSD_ATTR_DISPLAY_OFFSET, pos);
		AHC_OSDSetDisplayAttr(TV_MENU_OSD2_ID, AHC_OSD_ATTR_DISPLAY_OFFSET, pos);
        pos[0] = MMP_DISPLAY_DUPLICATE_2X;
        pos[1] = MMP_DISPLAY_DUPLICATE_2X;
        AHC_OSDSetDisplayAttr(TV_MENU_OSD_ID, AHC_OSD_ATTR_DISPLAY_SCALE, pos);
        AHC_OSDSetDisplayAttr(TV_MENU_OSD2_ID, AHC_OSD_ATTR_DISPLAY_SCALE, pos);
       	pos[0] = OSD_FLIP_DRAW_NONE_ENABLE;                
       	AHC_OSDSetDisplayAttr(TV_MENU_OSD_ID,  AHC_OSD_ATTR_FLIP_BY_GUI, pos);
       	AHC_OSDSetDisplayAttr(TV_MENU_OSD2_ID,  AHC_OSD_ATTR_FLIP_BY_GUI, pos);
		AHC_OSDClearBuffer(TV_MENU_OSD_ID);	

		AHC_OSDClearBuffer(TV_MENU_OSD2_ID);	
	}
	#else
    //** Set TV OSD#1 by uwDisplayID = 16
    AHC_OSDCreateBuffer(TVSmallOSD[0].uwDisplayID, TVSmallOSD[0].uwWidth, TVSmallOSD[0].uwHeight, OSD_COLOR_RGB565); 
    TVFunc_SetSmallOSDDisplayOffset(0);
    AHC_OSDSetCurrentDisplay(TVSmallOSD[0].uwDisplayID);
    #endif
    
    //** Set TV OSD#2 by uwDisplayID = 17
    AHC_OSDCreateBuffer(TVSmallOSD[1].uwDisplayID, TVSmallOSD[1].uwWidth, TVSmallOSD[1].uwHeight, OSD_COLOR_RGB565); 
    TVFunc_SetSmallOSDDisplayOffset(1);
    {
        UINT32 pos[2];
        pos[0] = OSD_FLIP_DRAW_NONE_ENABLE;                
        AHC_OSDSetDisplayAttr(TVSmallOSD[1].uwDisplayID,  AHC_OSD_ATTR_FLIP_BY_GUI, pos);
        pos[0] = OSD_ROTATE_DRAW_NONE;
        AHC_OSDSetDisplayAttr(TVSmallOSD[1].uwDisplayID,  AHC_OSD_ATTR_ROTATE_BY_GUI, pos);
    }
    AHC_OSDClearBuffer(TVSmallOSD[1].uwDisplayID);
    AHC_OSDSetCurrentDisplay(TVSmallOSD[1].uwDisplayID);
    
    m_TVStatus = TVStatus;       
}

#if 0
void ____TV_Preview_Function_____(){ruturn;} //dummy
#endif

#if (TVOUT_PREVIEW_EN)	

AHC_BOOL TVFunc_PreviewVideo(void)
{
    m_uwTVPrevOsdId = TVFunc_InitPreviewOSD();

    bPreviewModeTVout = AHC_TRUE;
    VideoTimer_Start(VIDEO_TIMER_UNIT);

    if(VideoFunc_Preview())
    {
        DrawStateTVPreviewVideoInit(m_uwTVPrevOsdId);
        BEGIN_LAYER(m_uwTVPrevOsdId);
        AHC_OSDSetActive(m_uwTVPrevOsdId, 1);
        END_LAYER(m_uwTVPrevOsdId);
        m_TVStatus = AHC_TV_VIDEO_PREVIEW_STATUS;
    }

    return AHC_TRUE;
}

AHC_BOOL TVFunc_PreviewCamera(void)
{   
    m_uwTVPrevOsdId = TVFunc_InitPreviewOSD();

    bPreviewModeTVout = AHC_TRUE;
    CaptureTimer_Start(100); 

    if(CaptureFunc_Preview())
    {
        DrawStateTVPreviewCameraInit(m_uwTVPrevOsdId);
        BEGIN_LAYER(m_uwTVPrevOsdId);
        AHC_OSDSetActive(m_uwTVPrevOsdId, 1);
        END_LAYER(m_uwTVPrevOsdId);
        m_TVStatus = AHC_TV_DSC_PREVIEW_STATUS;
    }

    return AHC_TRUE;
}

#endif

#if 0
void ____TV_Browser_Function_____(){ruturn;} //dummy
#endif
/*
 * return number of thumbnail
 */
int TVFunc_BrowserSetting(void)
{
    UINT32 MaxDcfObj; 
    UINT16 uwWidth, uwHeight;
	
    m_ThumbConfig.uwTHMPerLine    = 4;
    m_ThumbConfig.uwTHMPerColume  = 3;
    m_ThumbConfig.uwHGap          = 15;
    m_ThumbConfig.uwVGap          = 15;
    m_ThumbConfig.byBkColor       = 0x101010;
    m_ThumbConfig.byFocusColor    = 0x404090;

    switch(MenuSettingConfig()->uiTVSystem) 
    {
	    case TV_SYSTEM_NTSC:
	        m_ThumbConfig.StartX          = 20;
	        m_ThumbConfig.StartY          = 20;	

	        AHC_GetNtscTvDisplayWidthHeight(&uwWidth, &uwHeight);
	        m_ThumbConfig.DisplayWidth    = uwWidth  - m_ThumbConfig.StartX - 30;
	        m_ThumbConfig.DisplayHeight   = uwHeight - POS_TV_NTSC_BROWSER_OSD_H - 45; 
	    break;
	    case TV_SYSTEM_PAL:
	        m_ThumbConfig.StartX          = 48;
	        m_ThumbConfig.StartY          = 65;	
	        
	        AHC_GetPalTvDisplayWidthHeight(&uwWidth, &uwHeight);
	        m_ThumbConfig.DisplayWidth    = uwWidth  - m_ThumbConfig.StartX - 34;
	        m_ThumbConfig.DisplayHeight   = uwHeight - POS_TV_PAL_BROWSER_OSD_H - 58;
	    break;
    }  
    
    AHC_Thumb_Config(&m_ThumbConfig, AHC_FALSE);
    
    if(GetCurrentOpMode()==MOVPB_MODE)
    	MaxDcfObj = AHC_GetMappingFileNum(FILE_MOVIE);
    else if(GetCurrentOpMode()==JPGPB_MODE)	
    	MaxDcfObj = AHC_GetMappingFileNum(FILE_PHOTO);
	else if(GetCurrentOpMode()==JPGPB_MOVPB_MODE) 
			MaxDcfObj = AHC_GetMappingFileNum(FILE_PHOTO_MOVIE);

    if(MaxDcfObj==0) {
    	AHC_WMSG_Draw(AHC_TRUE, WMSG_NO_FILE_IN_BROWSER, 2);
    } else {
	    if( ObjSelect > MaxDcfObj - 1)  
	    	ObjSelect = MaxDcfObj - 1;
	}
	return (int)MaxDcfObj;
}

AHC_BOOL TVFunc_Enable(AHC_BOOL bEnable)
{
    AHC_BOOL 	ahc_ret = AHC_TRUE;
    UINT32 		iVal[2];
	
	m_DelFile = DEL_FILE_NONE;
	
    if(bEnable)
    {
        if(m_TVStatus != AHC_TV_BROWSER_STATUS)
        {
            RTNA_LCD_Backlight(MMP_FALSE);
            #ifdef CFG_ENBLE_PANEL_SLEEP
            RTNA_LCD_Enter_Sleep();
            #endif
            
			switch(m_TVStatus) 
			{
                case AHC_TV_NONE_STATUS:
                
                	AHC_SetDisplayMode(DISPLAY_MODE_DISABLE);
                    TVFunc_ChangeOSDStatus(AHC_TV_BROWSER_STATUS);
                    
					MenuStatePlaybackSelectDB( GetCurrentOpMode() );
					
					SetCurrentOpMode(GetCurrentOpMode()); 
					AHC_SetDisplayMode(DISPLAY_MODE_OSD_DISABLE);
					BrowserFunc_ThumbnailEditCheckFileObj();
                    
                    TVFunc_BrowserSetting();

                    AIHC_DrawReservedOSD(AHC_TRUE);
                    iVal[0] = 0;
                    
                    switch(MenuSettingConfig()->uiTVSystem) 
                    {
	                    case TV_SYSTEM_NTSC:
	                        iVal[1] = POS_TV_NTSC_BROWSER_OSD_H;
	        			break;		        			
	                    case TV_SYSTEM_PAL:
	                        iVal[1] = POS_TV_PAL_BROWSER_OSD_H; 
	                    break;		                    
	                    default:
	                        printc("TVFunc_Enable Error!!\r\n");
	                    break;
                    }
                    
                    AHC_OSDSetDisplayAttr(THUMBNAIL_OSD_BACK_ID, AHC_OSD_ATTR_DISPLAY_OFFSET, iVal);   
                    AHC_OSDSetCurrentDisplay(THUMBNAIL_OSD_BACK_ID);

                    AHC_OSDSetDisplayAttr(THUMBNAIL_OSD_FRONT_ID, AHC_OSD_ATTR_DISPLAY_OFFSET, iVal);
                    AHC_OSDSetCurrentDisplay(THUMBNAIL_OSD_FRONT_ID);
                    
                    AIHC_DrawReservedOSD(AHC_FALSE);
                    // TODO: should check number of thumbnail to draw!!
                    //       

                	if(AHC_FALSE == AHC_SDMMC_GetMountState())
                		AHC_DrawThumbNailEmpty();
                	else
                    	AHC_Thumb_DrawPage(AHC_TRUE);

                    DrawStateTVBrowserUpdate(EVENT_TV_CHANGE_MODE_INIT);

                    m_TVStatus = AHC_TV_BROWSER_STATUS;
                break; 
                   
                case AHC_TV_MOVIE_PB_STATUS:
                case AHC_TV_PHOTO_PB_STATUS:
                    TVFunc_ThumbnailOption(BROWSER_PLAY);
                break;    
                
                case AHC_TV_SLIDE_SHOW_STATUS: 
                    TVFunc_SlideShow();
                break;  
            
            	#if (TVOUT_PREVIEW_EN)
            	case AHC_TV_VIDEO_PREVIEW_STATUS:
            		TVFunc_PreviewVideo();
            	break;
            	
            	case AHC_TV_DSC_PREVIEW_STATUS:
            		TVFunc_PreviewCamera();
            	break;
            	#endif
            	case AHC_TV_SLEEP_STATUS:
					// Enter TV SLEEP MODE, and draw blank on TV!
            		TVFunc_ChangeOSDStatus(AHC_TV_SLEEP_STATUS);
            		DrawStateTVSleepUpdate(EVENT_TV_CHANGE_MODE_INIT);
            	break;
            #ifdef NET_SYNC_PLAYBACK_MODE
    			case AHC_TV_NET_PLAYBACK_STATUS :
  					m_uwTVPrevOsdId = TVFunc_InitPreviewOSD();
        			DrawStateTVPreviewVideoInit(m_uwTVPrevOsdId);
        			BEGIN_LAYER(m_uwTVPrevOsdId);
        			AHC_OSDSetActive(m_uwTVPrevOsdId, 1);
        			END_LAYER(m_uwTVPrevOsdId);
            		AHC_SetMode(AHC_MODE_IDLE); 
            		DrawStateTVNetPlaybackInit();
        		break;
    		#endif
    		
            }

    		Menu_SetVolume(MenuSettingConfig()->uiVolume);//TBD
        } 
    }
    else
    {
        if( m_TVStatus )
        {
            AHC_OSDUninit();
            AHC_SetMode(AHC_MODE_IDLE); 
            
            AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
            InitOSD();
            AHC_UF_GetCurrentIndex(&ObjSelect);

#ifndef TV_ONLY
            #ifdef CFG_TV_PLUGOUT_RETURN_VIDEO_PREVIEW
            StateSwitchMode(UI_VIDEO_STATE);
            #else
            StateSwitchMode(UI_BROWSER_STATE);
            #endif
#endif

            RTNA_LCD_Backlight(MMP_TRUE);
        }
    }

    return ahc_ret;
}

#endif

AHC_BOOL TVFunc_ThumbnailPlayback(void)
{
#if (TVOUT_ENABLE)

    AHC_BOOL 	ahc_ret = AHC_TRUE;
   
    if(MOVPB_MODE == GetCurrentOpMode())
    {		
        if(AHC_GetMappingFileNum(FILE_MOVIE) == 0)
            return AHC_FALSE;
            
        TVFunc_ChangeOSDStatus(AHC_TV_MOVIE_PB_STATUS);
        
        m_bMovieOSDForceShowUp = AHC_FALSE;
        MovPlaybackParamReset();
        MovPBTimer_Start(300); 
         
        MediaPlaybackConfig(PLAYBACK_CURRENT);
        
        if(bMovPbEnterToTV)
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

        	if(bMovPbEnterToTV)
        	{
        		UpdateTVMovPBStatus(bMovieState);
        		bMovPbEnterToTV = AHC_FALSE;
        	}
        	else
        	{
        		UpdateTVMovPBStatus(MOVIE_STATE_PLAY);
        	}

        	DrawStateTVMoviePBUpdate(EVENT_TV_CHANGE_MODE_INIT);
        	MovPBFunc_SetOSDShowStatus(AHC_TRUE); 
		}
        else 
        {
            MovPBTimer_Stop();
            m_TVStatus = AHC_TV_NONE_STATUS;
            TVFunc_Enable(AHC_TRUE);
            AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);    
            return ahc_ret;
        }
    }
    else if(JPGPB_MODE == GetCurrentOpMode())
    {    
        if(AHC_GetMappingFileNum(FILE_PHOTO) == 0)
            return AHC_FALSE;
            
        TVFunc_ChangeOSDStatus(AHC_TV_PHOTO_PB_STATUS);
        
        AHC_SetMode(AHC_MODE_IDLE);
        
        MediaPlaybackConfig(PLAYBACK_CURRENT);
        PhotoPBTimer_Start(300);
        ahc_ret = JpgPlayback_Play();
        
		if(ahc_ret) 
		{
            DrawStateTVPhotoPBUpdate(EVENT_TV_CHANGE_MODE_INIT);
            PhotoPBFunc_SetOSDShowStatus(AHC_TRUE);  
        }
        else 
        {
            PhotoPBTimer_Stop();
            m_TVStatus = AHC_TV_NONE_STATUS;
            TVFunc_Enable(AHC_TRUE);
            AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);    
        }    
    }
    else if(JPGPB_MOVPB_MODE == GetCurrentOpMode())
    {    
        if(AHC_GetMappingFileNum(FILE_PHOTO_MOVIE) == 0)
            return AHC_FALSE;
            
        TVFunc_ChangeOSDStatus(AHC_TV_ALL_PB_STATUS);
        
        AHC_SetMode(AHC_MODE_IDLE);
        
        MediaPlaybackConfig(PLAYBACK_CURRENT);
        if(PLAYBACK_IMAGE_TYPE == GetPlayBackFileType())
			PhotoPBTimer_Start(300);
		else
			MovPBTimer_Start(300);   
        ahc_ret = JpgPlayback_Play();
        
		if(ahc_ret) 
		{
            DrawStateTVPhotoPBUpdate(EVENT_TV_CHANGE_MODE_INIT);
            PhotoPBFunc_SetOSDShowStatus(AHC_TRUE);  
        }
        else 
        {
            if(PLAYBACK_IMAGE_TYPE == GetPlayBackFileType())
				PhotoPBTimer_Stop();
			else
				MovPBTimer_Stop();   
            m_TVStatus = AHC_TV_NONE_STATUS;
            TVFunc_Enable(AHC_TRUE);
            AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);    
        }    
    }
    
    return ahc_ret;
#else
	return AHC_TRUE;
#endif    
}

AHC_BOOL TVFunc_ThumbnailOption(UINT8 ubOp)
{
#if (TVOUT_ENABLE)

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
			ahc_ret = TVFunc_ThumbnailPlayback();
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

AHC_BOOL TVFunc_ThumbnailModeSwitch(void)
{
#if (TVOUT_ENABLE)

    AHC_SetMode(AHC_MODE_IDLE);
	
    if(JPGPB_MODE == GetCurrentOpMode())
    {
		MenuStatePlaybackSelectDB( MOVPB_MODE );    
        SetCurrentOpMode(MOVPB_MODE);
    }
    else
    {
    	MenuStatePlaybackSelectDB( JPGPB_MODE );
        SetCurrentOpMode(JPGPB_MODE);
	}

    TVFunc_BrowserSetting();
    AHC_Thumb_DrawPage(AHC_TRUE);
#endif
    return AHC_TRUE;
}

AHC_BOOL TVFunc_Thumbnail2Preview(void)
{
#if (TVOUT_ENABLE)

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
                 
    m_TVStatus = AHC_TV_NONE_STATUS;
    
    return ahc_ret;
#else
	return AHC_TRUE;
#endif
}

AHC_BOOL TVFunc_ThumbnailEditOneFile(void)
{
#if (TVOUT_ENABLE)

	if(	m_TVStatus!=AHC_TV_MOVIE_PB_STATUS 	&&
		m_TVStatus!=AHC_TV_PHOTO_PB_STATUS	&&
        m_TVStatus!=AHC_TV_ALL_PB_STATUS	&&
		m_TVStatus!=AHC_TV_SLIDE_SHOW_STATUS )
	{
		bPreviewModeTVout = AHC_FALSE;				
		m_TVStatus = AHC_TV_NONE_STATUS;
		if(GetCurrentOpMode()==JPGPB_MODE)
			SetCurrentOpMode(JPGPB_MODE);
		else if(GetCurrentOpMode()==MOVPB_MODE)
			SetCurrentOpMode(MOVPB_MODE);
		else if(GetCurrentOpMode()==JPGPB_MOVPB_MODE)
			SetCurrentOpMode(JPGPB_MOVPB_MODE);
		TVFunc_Enable(AHC_TRUE);
	}
#endif
	return AHC_TRUE;
}

AHC_BOOL TVFunc_ThumbnailQuickFileDelete(void)
{
#if (TVOUT_ENABLE)

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
		DrawStateTVBrowserPageUpdate();
	}
#endif
	return AHC_TRUE;
}

AHC_BOOL TVFunc_PlaybackEditOneFile(UINT8 Op)
{
#if (TVOUT_ENABLE)

	if(	m_TVStatus==AHC_TV_MOVIE_PB_STATUS 	||
		m_TVStatus==AHC_TV_PHOTO_PB_STATUS	||
		m_TVStatus==AHC_TV_ALL_PB_STATUS	||
		m_TVStatus==AHC_TV_SLIDE_SHOW_STATUS )
	{	
		if(	m_TVStatus==AHC_TV_MOVIE_PB_STATUS ||
			m_TVStatus==AHC_TV_SLIDE_SHOW_STATUS )
			SetCurrentOpMode(MOVPB_MODE);
		else if( m_TVStatus==AHC_TV_PHOTO_PB_STATUS )	
			SetCurrentOpMode(JPGPB_MODE);
		else if( m_TVStatus==AHC_TV_ALL_PB_STATUS )		
			SetCurrentOpMode(JPGPB_MODE);
		GetCurrentMenu()->pParentMenu = NULL;
		BrowserFunc_ThumbnailResetAllEditOp();
		ResetCurrentMenu();
		Menu_WriteSetting();   	             		
		
		m_TVStatus = AHC_TV_NONE_STATUS;
		TVFunc_Enable(AHC_TRUE);
	}
#endif
	return AHC_TRUE;
}

#if 0
void ____TV_MoviePB_Function_____(){ruturn;} //dummy
#endif

#if (TVOUT_ENABLE)

AHC_BOOL TVFunc_MoviePBCheckOSDShowUp(UINT32 ulEvent)
{ 
    UINT8  		bID0 = 0;
    AHC_BOOL 	state;
    
	if(m_TVStatus != AHC_TV_MOVIE_PB_STATUS)
	{
        return AHC_TRUE;
    }
    
	if(	MOV_STATE_PLAY 	!= GetMovConfig()->iState && 
		MOV_STATE_FF 	!= GetMovConfig()->iState && 
		MOV_STATE_PAUSE != GetMovConfig()->iState)
    {
       return AHC_TRUE;
    }

	if(ulEvent == EVENT_TV_REMOVED || ulEvent == EVENT_HDMI_DETECT)
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
                OSDDraw_GetLastOvlDrawBuffer(&bID0);
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
        OSDDraw_GetLastOvlDrawBuffer(&bID0);
        AHC_OSDSetActive(bID0, 1);  
        MovPBFunc_SetOSDShowStatus(AHC_TRUE); 
        return AHC_FALSE;
    }    
}

AHC_BOOL TVFunc_MoviePBPrevious(void)
{
    AHC_BOOL ahc_ret = AHC_TRUE;

	MediaPlaybackConfig(PLAYBACK_PREVIOUS);
	ahc_ret = MovPlayback_Play();

    if(!ahc_ret) 
    {
        MovPBTimer_Stop();
        m_TVStatus = AHC_TV_NONE_STATUS;
        TVFunc_Enable(AHC_TRUE);
        AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);    
    }
	
    return ahc_ret;
}

AHC_BOOL TVFunc_MoviePBNext(void)
{
    AHC_BOOL ahc_ret = AHC_TRUE;

	MediaPlaybackConfig(PLAYBACK_NEXT);
	ahc_ret = MovPlayback_Play();

    if(!ahc_ret) 
    {
        MovPBTimer_Stop();
        m_TVStatus = AHC_TV_NONE_STATUS;
        TVFunc_Enable(AHC_TRUE);
        AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);    
    }
    
    return ahc_ret;
}

AHC_BOOL TVFunc_MoviePBPlayPress(void)
{
    AHC_BOOL 	ahc_ret = AHC_TRUE; 
    UINT32 		uState;
     
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
	            m_TVStatus = AHC_TV_NONE_STATUS;
	            TVFunc_Enable(AHC_TRUE);
	            AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);  
	            return AHC_FALSE;
        	}
	        DrawStateTVMoviePBUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);     
	    break;
	    
	    case STATE_MOV_EXECUTE :
	        if(MOV_STATE_FF == GetMovConfig()->iState)
	        {	
				MovPlayback_NormalSpeed();
	          	DrawStateTVMoviePBUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);
	        }
	        else
	        {	
	          	MovPlayback_Pause();
	          	DrawStateTVMoviePBUpdate(EVENT_MOVPB_TOUCH_PAUSE_PRESS);                      
	        }
	    break;
	    
	    case STATE_MOV_PAUSE   :
	    	AHC_PlaybackClipCmd(AHC_PB_MOVIE_NORMAL_PLAY_SPEED, 0);
	        MovPlayback_Resume();
	        DrawStateTVMoviePBUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);
	    break;
	    
	    default                :
	    break;
    }    
   
    return ahc_ret;
}

AHC_BOOL TVFunc_MoviePBSeekSOS(void)
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
	DrawStateTVMoviePBUpdate(EVENT_MOVPB_TOUCH_PAUSE_PRESS);
	#else
	DrawStateTVMoviePBUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);
	#endif
	
    return AHC_TRUE;
}

AHC_BOOL TVFunc_MoviePBForward(void)
{
    UINT32 		ulCurTime;
    UINT32 		uState;
    
    AHC_GetSystemStatus(&uState);
 
    switch(uState)
    {
	    case STATE_MOV_IDLE :	                         
	    break;
	    
	    case STATE_MOV_EXECUTE :
			if(bTVMovieState!=MOVIE_STATE_REW || m_MovPlayFFSpeed==1)
			{
	        	m_MovPlayFFSpeed = ( (m_MovPlayFFSpeed<<1) > MOVPB_MAX_SPEED )?(MOVPB_MIN_SPEED):(m_MovPlayFFSpeed<<1);
			}
			AHC_SetPlaySpeed(AHC_PB_MOVIE_FAST_FORWARD_RATE, m_MovPlayFFSpeed+1);

	        MovPlayback_Forward();
	    break;
	    
	    case STATE_MOV_PAUSE   :
	        AHC_GetVideoCurrentPlayTime(&ulCurTime);
	        AHC_PlaybackClipCmd(AHC_PB_MOVIE_NORMAL_PLAY_SPEED, 0);
	        AHC_PlaybackClipCmd(AHC_PB_MOVIE_SEEK_BY_TIME, ulCurTime+500);
	        DrawStateTVMoviePBUpdate(EVENT_MOVPB_UPDATE_MESSAGE);
	    break;
	    
	    default                :
	    break;
    }    
   
    return AHC_TRUE;
}

AHC_BOOL TVFunc_MoviePBBackward(void)
{ 
    UINT32 		ulCurTime;
    UINT32 		uState;
    
    AHC_GetSystemStatus(&uState);
    
    switch(uState)
    {
	    case STATE_MOV_IDLE :
	    break;
	    
	    case STATE_MOV_EXECUTE :
	        if(bTVMovieState!=MOVIE_STATE_FF || m_MovPlayREWSpeed==1)
        	{	
         		m_MovPlayREWSpeed = ( (m_MovPlayREWSpeed<<1) > MOVPB_MAX_SPEED )?(MOVPB_MIN_SPEED):(m_MovPlayREWSpeed<<1); 	
        	}
			AHC_SetPlaySpeed(AHC_PB_MOVIE_BACKWARD_RATE, m_MovPlayREWSpeed+1); 
	    
	        MovPlayback_Backward();
	    break;
	    
	    case STATE_MOV_PAUSE   :
	        AHC_GetVideoCurrentPlayTime(&ulCurTime);
	        AHC_PlaybackClipCmd(AHC_PB_MOVIE_NORMAL_PLAY_SPEED, 0);
	        AHC_PlaybackClipCmd(AHC_PB_MOVIE_SEEK_BY_TIME, ulCurTime-900);
	        DrawStateTVMoviePBUpdate(EVENT_MOVPB_UPDATE_MESSAGE);
	    break;
	    
	    default                :
	    break;
    }    
   
    return AHC_TRUE;
}    

AHC_BOOL TVFunc_MoviePBDelFile(void)
{
    UINT32          CurObjIdx;
    UINT32          MaxDcfObj;
 
    AHC_UF_GetTotalFileCount(&MaxDcfObj);
    
    if(MaxDcfObj == 0)
        return AHC_FALSE;
 
    AHC_UF_GetCurrentIndex(&CurObjIdx);
    AHC_UF_FileOperation_ByIdx(CurObjIdx,DCF_FILE_DELETE, NULL, NULL);
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

AHC_BOOL TVFunc_MoviePBModeSwitch(void)
{
#if 0
    AHC_BOOL 	ahc_ret = AHC_TRUE; 
    UINT32  	MaxDcfObj = 0;
    
    SetCurrentOpMode(JPGPB_MODE);

    MaxDcfObj = AHC_GetMappingFileNum(FILE_PHOTO);
    
    if(MaxDcfObj == 0) 
        return AHC_FALSE;
    
    AHC_UF_SetCurrentIndex(MaxDcfObj-1);
    ahc_ret = TVFunc_ThumbnailOption(BROWSER_PLAY);

    return ahc_ret;
#else
	return AHC_FALSE;
#endif
}

AHC_BOOL TVFunc_MoviePB2Preview(void)
{
    AHC_BOOL ahc_ret = AHC_TRUE; 

    AHC_OSDUninit();
    AHC_SetMode(AHC_MODE_IDLE);
             
    AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
    InitOSD();
           
    ahc_ret = StateSwitchMode(UI_VIDEO_STATE);

    RTNA_LCD_Backlight(MMP_TRUE);

    m_TVStatus = AHC_TV_NONE_STATUS;
    
    return ahc_ret;
}

#if 0
void ____TV_PhotoPB_Function_____(){ruturn;} //dummy
#endif

AHC_BOOL TVFunc_PhotoPBCheckOSDShowUp(UINT32 ulEvent)
{ 
    AHC_BOOL state;
    
    if(m_TVStatus != AHC_TV_PHOTO_PB_STATUS)
    {
    	return AHC_TRUE;
    }
    
    if(ulEvent == EVENT_TV_REMOVED || ulEvent == EVENT_HDMI_DETECT)
    {
		return AHC_TRUE;
	} 

	if(ulEvent == EVENT_MOVPB_UPDATE_MESSAGE) {
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
}

AHC_BOOL TVFunc_PhotoPBPrevious(void)
{
    AHC_BOOL ahc_ret = AHC_TRUE;

	MediaPlaybackConfig(PLAYBACK_PREVIOUS);
	ahc_ret = JpgPlayback_Play();

    if(!ahc_ret) 
    {
        PhotoPBTimer_Stop();
        m_TVStatus = AHC_TV_NONE_STATUS;
        TVFunc_Enable(AHC_TRUE);
        AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);    
    }
	
    return ahc_ret;
}

AHC_BOOL TVFunc_PhotoPBNext(void)
{
    AHC_BOOL ahc_ret = AHC_TRUE;

	MediaPlaybackConfig(PLAYBACK_NEXT);
	ahc_ret = JpgPlayback_Play();

    if(!ahc_ret)
    {
        PhotoPBTimer_Stop();
        m_TVStatus = AHC_TV_NONE_STATUS;
        TVFunc_Enable(AHC_TRUE);
        AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);
    }
    
    return ahc_ret;
}

AHC_BOOL TVFunc_PhotoPBDelFile(void)
{
    UINT32          CurObjIdx;
    UINT32          MaxDcfObj; 

    AHC_UF_GetTotalFileCount(&MaxDcfObj);
    
    if(MaxDcfObj == 0)
        return AHC_FALSE;
 
    AHC_UF_GetCurrentIndex(&CurObjIdx);
    AHC_UF_FileOperation_ByIdx(CurObjIdx,DCF_FILE_DELETE, NULL, NULL);    
    AHC_UF_GetCurrentIndex(&CurObjIdx);
    AHC_UF_SetCurrentIndex(CurObjIdx);
    
    MediaPlaybackConfig(PLAYBACK_CURRENT);
    
    if(!JpgPlayback_Play()) {
        AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);
        return AHC_FALSE;
    }
   
    return AHC_TRUE;
}

AHC_BOOL TVFunc_PhotoPBModeSwitch(void)
{
#if 0
    AHC_BOOL 	ahc_ret = AHC_TRUE; 
    UINT32  	MaxDcfObj = 0;
    
    SetCurrentOpMode(MOVPB_MODE);
    
    MaxDcfObj = AHC_GetMappingFileNum(FILE_MOVIE);
    
    if(MaxDcfObj == 0)
        return AHC_FALSE;
    
    AHC_UF_SetCurrentIndex(MaxDcfObj-1);
    ahc_ret = TVFunc_ThumbnailOption(BROWSER_PLAY);

    return ahc_ret;
#else
	return AHC_FALSE;
#endif
}

AHC_BOOL TVFunc_PhotoPB2Preview(void)
{
    AHC_BOOL ahc_ret = AHC_TRUE; 

    AHC_OSDUninit();
    AHC_SetMode(AHC_MODE_IDLE); 
            
    AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
    InitOSD();
           
    ahc_ret = StateSwitchMode(UI_CAMERA_STATE);

    RTNA_LCD_Backlight(MMP_TRUE);
    
    m_TVStatus = AHC_TV_NONE_STATUS;
    
    return ahc_ret;
}

#if 0
void ____TV_Slideshow_Function_____(){ruturn;} //dummy
#endif

AHC_BOOL TVFunc_SlideShowCheckOSDShowUp(UINT32 ulEvent)
{ 
    AHC_BOOL state;
    
    if(m_TVStatus != AHC_TV_SLIDE_SHOW_STATUS) 
    {
    	return AHC_TRUE;
    }
    
	if(ulEvent == EVENT_TV_REMOVED || ulEvent == EVENT_HDMI_DETECT || ulEvent == EVENT_SLIDESHOW_UPDATE_FILE)
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
		DrawStateTVSlideShowUpdate(EVENT_SLIDESHOW_UPDATE_FILE);
		return AHC_FALSE;
	}
}

AHC_BOOL TVFunc_SlideShow(void)
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

    TVFunc_ChangeOSDStatus(AHC_TV_SLIDE_SHOW_STATUS);    

    SlideShowFunc_SetOSDShowStatus(AHC_FALSE); 
    SlideShowFunc_Stop();
    SlideShowTimer_Start(100);

    MediaPlaybackConfig(PLAYBACK_CURRENT);
    ahc_ret = SlideShowFunc_Play();
    
    if(ahc_ret) 
    {
		Menu_SetVolume(MenuSettingConfig()->uiVolume);
        DrawStateTVSlideShowUpdate(EVENT_TV_CHANGE_MODE_INIT);
    }

    SlideShowFunc_SetOSDShowStatus(AHC_TRUE); 
    
    return ahc_ret;
}

#endif

#if 0
void ____TV_Main_Function_____(){ruturn;} //dummy
#endif

AHC_BOOL TVMode_Start(void)
{
#if (TVOUT_ENABLE)

    AHC_BOOL ahc_ret = AHC_TRUE;

	if(Deleting_InBrowser || Protecting_InBrowser)
	{
		BrowserFunc_ThumbnailResetAllEditOp();
	}

    if(AHC_WMSG_States()) 
    {
       AHC_WMSG_Draw(AHC_FALSE, WMSG_NONE, 0);
    }
    
#if	OSD_SHOW_BATTERY_STATUS == 1
	if(AHC_TRUE == AHC_GetChargerStatus())
	{
		AHC_EnableChargingIcon(AHC_FALSE);
		AHC_ClearAllChargingIcon();
	}
#endif
    ahc_ret = TVFunc_Enable(AHC_TRUE);

	m_DelFile = DEL_FILE_NONE;

	CHARGE_ICON_ENABLE(AHC_TRUE);

#if (TVOUT_PREVIEW_EN)
	AHC_SetTVBooting(AHC_FALSE);
#endif

	m_ubInTVMode = AHC_TRUE;

    return ahc_ret;
#else
	return AHC_TRUE;
#endif    
}

void SetTVState(AHC_TV_STATUS TVState) 
{
    m_TVStatus = TVState;
}

UINT32 SetTVMenuHandler(UINT32 ulJobEvent)
{
#if (TV_MENU_EN)
	UINT32 				ulMenuEvent,ret;
	pfMenuEventHandler	pf;
	PSMENUSTRUCT 		pMenu = GetCurrentMenu();

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
		case EVENT_KEY_RIGHT:
		case EVENT_KEY_MODE:
			ulMenuEvent = MENU_RIGHT;
		break;	
		case EVENT_TV_BROWSER_KEY_PLAYBACK:
		case EVENT_KEY_SET:
		case EVENT_VIDEO_KEY_RECORD:
			ulMenuEvent = MENU_OK;
		break;	
		case EVENT_KEY_MENU:
			ulMenuEvent = MENU_MENU;
			StateTV_EventMenu_Handler(ulJobEvent);
			return ret;
		break;
		case EVENT_POWER_OFF:
			ulMenuEvent = MENU_POWER_OFF;
		break;
		default:
			ulMenuEvent = MENU_INIT;
			return ret;
		break;
	}

	if(IsCurrentMainMenu(pMenu)) {
		pf = MainMenuEventHandler;
	}
	#if (TOP_MENU_PAGE_EN)
	else if(IsCurrentTopMenu(pMenu)) {
		pf = TopMenuEventHandler;
	}
	#endif	
	else {
		pf = SubMenuEventHandler;
	}

	if (pf(pMenu, ulMenuEvent, 0) == MENU_ERR_EXIT) {
		ResetCurrentMenu();
	}
#endif
	return ret;
}

#if 0
void ____TV_Event_Function_____(){ruturn;} //dummy
#endif

#if (TVOUT_ENABLE)
static void TV_VideoRecMode_PreviewUpdate(void) 
{
    AHC_BOOL    ubUpdatePreview   = AHC_FALSE;

    _AHC_PRINT_FUNC_ENTRY_();

    if(VideoRecSize != MenuSettingConfig()->uiMOVSize) {
        printc("Video size changed\r\n");

        ubUpdatePreview = AHC_TRUE;
    }

    #if (ENABLE_ADAS_LDWS)
    {
        UINT32 bLDWS_En;

        if (LDWS_SettingChanged() == MMP_TRUE){
            printc("LDWS setting changed\r\n");
            ubUpdatePreview = AHC_TRUE;
        }

        if ( AHC_Menu_SettingGetCB( (char *)COMMON_KEY_LDWS_EN, &bLDWS_En ) == AHC_TRUE ) {
            if ((LDWS_EN_OFF == bLDWS_En) && (MMPS_Sensor_IsADASEnable() == MMP_TRUE) ) {
                printc("LDWS Off to Off\r\n");
                ubUpdatePreview = AHC_TRUE;
            }
            else if ((LDWS_EN_ON == bLDWS_En) && (MMPS_Sensor_IsADASEnable() == MMP_FALSE) ) { 
                printc("LDWS On to Off\r\n");
                ubUpdatePreview = AHC_TRUE;
            }
        }
    }
    #endif

    {
        UINT32 hdr_En;

        if ( AHC_Menu_SettingGetCB( (char *)COMMON_KEY_HDR_EN, &hdr_En ) == AHC_TRUE ) {
            if ((MMP_IsHDREnable() == MMP_TRUE) && (hdr_En == COMMON_HDR_EN_OFF) ) {
                ubUpdatePreview = AHC_TRUE;
            }
            else if ((MMP_IsHDREnable() == MMP_FALSE) && (hdr_En == COMMON_HDR_EN_ON) ) {
                ubUpdatePreview = AHC_TRUE;
            }
        }
    }

    #if (MOTION_DETECTION_EN)
    if (m_ubVMDStart && m_ulVMDRemindTime) {
        printc("Motion setting changed\r\n");
        ubUpdatePreview = AHC_TRUE;
    }
    #endif

    #if (VR_PREENCODE_EN)
    {
        UINT32  ulPreRecordStatus = 0;

        AHC_GetPreRecordStatus(&ulPreRecordStatus);

        if(bVideoPreRecordStatus != ulPreRecordStatus)
        {
            printc("Video preRecord changed\r\n");
            bVideoPreRecordStatus = ulPreRecordStatus;
            ubUpdatePreview       = AHC_TRUE;
        }
    }
    #endif

	if (Menu_Setting_CheckMenuAtoms() == AHC_FALSE)  
		ubUpdatePreview = AHC_TRUE;    //fixed sensor rotate

    if(ubUpdatePreview == AHC_TRUE)
    {
        #if (VR_PREENCODE_EN)
        if(!m_ubPreEncodeEn)
            bDisableVideoPreRecord = AHC_TRUE;
        #endif
        AHC_SetMode(AHC_MODE_IDLE);
        VideoFunc_Preview();
    }
}

void StateTV_EventMenu_Handler(UINT32 ulJobEvent)
{
#if (TV_MENU_EN)	

	PSMENUSTRUCT 	pMenu = GetCurrentMenu();
	AHC_BOOL 		ubExitMenu = AHC_FALSE;
	switch(m_TVStatus) 
	{		
#if (TVOUT_PREVIEW_EN)
		case AHC_TV_VIDEO_PREVIEW_STATUS:
		
			if(VideoFunc_RecordStatus()) 
				break;
			if(IN_MENU())
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
                	//if TV_SYSTEM change, redraw OSD and init
                    if(TVFunc_CheckTVSystemChanged()) {                    
                        TVFunc_InitPreviewOSD();
                        AHC_SetMode(AHC_MODE_IDLE);
                        VideoFunc_Preview();
                    }                   
                    else //else update preview
                    {
                    	TV_VideoRecMode_PreviewUpdate();
                    }                   
		            ResetCurrentMenu();
					Menu_WriteSetting();
					DrawStateTVPreviewVideoInit(bID0);
					AHC_OSDSetActive(TVFunc_GetMenuOsdID(), 0);
					Scale_MenuOSD_Window(MMP_DISPLAY_DUPLICATE_1X);
					AHC_OSDSetActive(bID0, 1);
					AHC_OSDSetCurrentDisplay(bID0);
					OSDDraw_ExitDrawing(&bID0, &bID1 );
					AHC_OSDRefresh();
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
				TopMenuEventHandler(&sTopMenu, MENU_INIT, 0);
				#else
				SetCurrentMenu(&sMainMenuVideo);
				Scale_MenuOSD_Window(MMP_DISPLAY_DUPLICATE_2X);
				MainMenuEventHandler(&sMainMenuVideo, MENU_INIT, 0);
				#endif	
					
				AHC_OSDSetActive(TV_UI_OSD_ID, 0);
				AHC_OSDSetActive(TV_MENU_OSD_ID, 1);
			}
		break;
				
		case AHC_TV_DSC_PREVIEW_STATUS:
			             	
			if(IN_MENU())
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
                	ImportMenuInfo(MenuSettingConfig());    // update menu item from data structure to atom

					if( CaptureResolution != MenuSettingConfig()->uiIMGSize || 
					    (Menu_Setting_CheckMenuAtoms() == AHC_FALSE))
					{
					    if(TVFunc_CheckTVSystemChanged())                                              
                            TVFunc_InitPreviewOSD();                        
       	 				AHC_SetMode(AHC_MODE_IDLE);
        				CaptureFunc_Preview();
					}                    		            
		         	ResetCurrentMenu();
					Menu_WriteSetting();
					
					DrawStateTVPreviewCameraInit(TV_UI_OSD_ID);
					AHC_OSDSetActive(TV_MENU_OSD_ID, 0);
					Scale_MenuOSD_Window(MMP_DISPLAY_DUPLICATE_1X);
					AHC_OSDSetActive(TV_UI_OSD_ID, 1);
					AHC_OSDSetCurrentDisplay(TV_UI_OSD_ID);
					AHC_OSDRefresh();
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
				TopMenuEventHandler(&sTopMenu, MENU_INIT, 0);
				#else
				SetCurrentMenu(&sMainMenuStill);
				Scale_MenuOSD_Window(MMP_DISPLAY_DUPLICATE_2X);
				MainMenuEventHandler(&sMainMenuStill, MENU_INIT, 0);
				#endif
				
				AHC_OSDSetActive(TV_UI_OSD_ID, 0);
				AHC_OSDSetActive(TV_MENU_OSD_ID, 1);
			}
		break;
#endif	
		case AHC_TV_BROWSER_STATUS:
			if(IN_MENU())
			{
				if(IsCurrentMainMenu(pMenu))
				{	  
					ResetCurrentMenu();
					Menu_WriteSetting();
				
					DrawStateTVBrowserInit();
					AHC_OSDSetActive(TV_MENU_OSD_ID, 0);
					AHC_OSDSetActive(TV_UI_OSD_ID, 1);
					AHC_OSDSetCurrentDisplay(TV_UI_OSD_ID);
					AHC_OSDRefresh();
					if(bForceSwitchBrowser) //redraw browser 
					{
						m_TVStatus = AHC_TV_NONE_STATUS;
						TVFunc_Enable(AHC_TRUE); 
						bForceSwitchBrowser = AHC_FALSE;
					}
					// No need to scale 1:1, if do, it will see a small menu page when exit. Strange!!
					//Scale_MenuOSD_Window(MMP_DISPLAY_DUPLICATE_1X);
				}
				else
				{
					SubMenuEventHandler(pMenu, MENU_MENU, 0);
				}
			}
			else
			{
				AHC_OSDClearBuffer(TV_MENU_OSD_ID);
				    
				SetCurrentMenu(&sMainMenuPlayback);
				Scale_MenuOSD_Window(MMP_DISPLAY_DUPLICATE_2X);
				MainMenuEventHandler(&sMainMenuPlayback, MENU_INIT, 0);
				
				AHC_OSDSetActive(TV_UI_OSD_ID, 0);
				AHC_OSDSetActive(TV_MENU_OSD_ID, 1);
			}
		break;
		
		case AHC_TV_MOVIE_PB_STATUS:
			if(IN_MENU())
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
					DrawStateTVMoviePBInit(bID0, bID1);
                    OSDDraw_ExitDrawing(&bID0, &bID1);
					AHC_OSDSetActive(TV_MENU_OSD_ID, 0);
					AHC_OSDSetActive(TV_UI_OSD_ID, 1);
					AHC_OSDSetCurrentDisplay(TV_UI_OSD_ID);
					AHC_OSDRefresh();
					Scale_MenuOSD_Window(MMP_DISPLAY_DUPLICATE_1X);
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
			
				AHC_OSDClearBuffer(TV_MENU_OSD_ID);
				    
				SetCurrentMenu(&sMainMenuPlayback);
				Scale_MenuOSD_Window(MMP_DISPLAY_DUPLICATE_2X);
				MainMenuEventHandler(&sMainMenuPlayback, MENU_INIT, 0);
				
				AHC_OSDSetActive(TV_UI_OSD_ID, 0);
				AHC_OSDSetActive(TV_MENU_OSD_ID, 1);
			}		
		break;

		case AHC_TV_PHOTO_PB_STATUS:
			if(IN_MENU())
			{
				if(IsCurrentMainMenu(pMenu))
				{	
				    UINT8 bID0,bID1;
					ResetCurrentMenu();
					Menu_WriteSetting();
				    OSDDraw_EnterDrawing(&bID0, &bID1);
					DrawStateTVPhotoPBInit(bID0, bID1);
                    OSDDraw_ExitDrawing(&bID0, &bID1);
					AHC_OSDSetActive(TV_MENU_OSD_ID, 0);
					Scale_MenuOSD_Window(MMP_DISPLAY_DUPLICATE_1X);
					AHC_OSDSetActive(TV_UI_OSD_ID, 1);
					AHC_OSDSetCurrentDisplay(TV_UI_OSD_ID);
					AHC_OSDRefresh();
				}
				else
				{
					SubMenuEventHandler(pMenu, MENU_MENU, 0);
				}
			}
			else
			{
				AHC_OSDClearBuffer(TV_MENU_OSD_ID); 
				   
				SetCurrentMenu(&sMainMenuPlayback);
				Scale_MenuOSD_Window(MMP_DISPLAY_DUPLICATE_2X);
				MainMenuEventHandler(&sMainMenuPlayback, MENU_INIT, 0);
				
				AHC_OSDSetActive(TV_UI_OSD_ID, 0);
				AHC_OSDSetActive(TV_MENU_OSD_ID, 1);
			}
		break;
		
		case AHC_TV_SLIDE_SHOW_STATUS:
			if(IN_MENU())
			{
				if(IsCurrentMainMenu(pMenu))
				{	
					ResetCurrentMenu();
					Menu_WriteSetting();
				
					DrawStateTVSlideShowInit();
					AHC_OSDSetActive(TV_MENU_OSD_ID, 0);
					Scale_MenuOSD_Window(MMP_DISPLAY_DUPLICATE_1X);
					AHC_OSDSetActive(TV_UI_OSD_ID, 1);
					AHC_OSDSetCurrentDisplay(TV_UI_OSD_ID);
					AHC_OSDRefresh();
				}
				else
				{
					SubMenuEventHandler(pMenu, MENU_MENU, 0);
				}
			}
			else
			{
				AHC_OSDClearBuffer(TV_MENU_OSD_ID);
				   
				SetCurrentMenu(&sMainMenuPlayback);
				Scale_MenuOSD_Window(MMP_DISPLAY_DUPLICATE_2X);
				MainMenuEventHandler(&sMainMenuPlayback, MENU_INIT, 0);
				
				AHC_OSDSetActive(TV_UI_OSD_ID, 0);
				AHC_OSDSetActive(TV_MENU_OSD_ID, 1);
			}
		break;
		
		default:
		break;
	}
#endif
}

#if 0
void ____TV_Mode_Event_Function_____(){ruturn;} //dummy
#endif

void StateTV_VideoMode_Handler(UINT32 ulJobEvent)
{
#if (TVOUT_PREVIEW_EN)

	UINT8 ubID = TVFunc_GetUImodeOsdID();

	if(m_TVStatus!=AHC_TV_VIDEO_PREVIEW_STATUS)
		return;
	
    if(IN_MENU())
    {
 		if(SetTVMenuHandler(ulJobEvent) == 0)
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
			StateTV_EventMenu_Handler(ulJobEvent);
		break;
		case EVENT_PARKING_KEY:
        	#if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
            if( VideoFunc_RecordStatus() == AHC_TRUE )
            {
            	break;
            }
            printc("TV_PARKING!!!\n");
            if( uiGetParkingModeEnable() == AHC_FALSE )
            {
                uiSetParkingModeEnable( AHC_TRUE );

                #if (MOTION_DETECTION_EN)
                if( MenuSettingConfig()->uiMotionDtcSensitivity == MOTION_DTC_SENSITIVITY_OFF )
                {
                	MenuSettingConfig()->uiMotionDtcSensitivity = MOTION_DTC_SENSITIVITY_LOW;
                }
                    Menu_SetMotionDtcSensitivity( MenuSettingConfig()->uiMotionDtcSensitivity );
               	#endif

                AIHC_SetVideoRecordMode( AHC_TRUE, AHC_VIDRECD_INIT ); // Use AHC_VIDRECD_RESTART_PREVIEW if Parking setting is the same as Normal mode
             }
             else
             {
                    AIHC_StopVideoRecordMode( AHC_TRUE, AHC_FALSE ); // Stop video Pre-record before uiSetParkingModeEnable( AHC_FALSE ) to avoid un-expected AHC_UF_PostAddFile()
                    uiSetParkingModeEnable( AHC_FALSE );

                    #if (MOTION_DETECTION_EN)
         			if( MenuSettingConfig()->uiMotionDtcSensitivity != MOTION_DTC_SENSITIVITY_OFF )
                    {
                        MenuSettingConfig()->uiMotionDtcSensitivity = MOTION_DTC_SENSITIVITY_OFF;
                    }
                    Menu_SetMotionDtcSensitivity( MenuSettingConfig()->uiMotionDtcSensitivity );				
                    #endif

                    AIHC_SetVideoRecordMode( AHC_FALSE, AHC_VIDRECD_INIT );
             }
             DrawVideoParkingMode( uiGetParkingModeEnable() );
             #endif
       	break;
		case EVENT_KEY_MODE:
		case EVENT_KEY_NEXT_MODE:
			if(!VideoFunc_RecordStatus())
			{
				VideoTimer_Stop();
			
				#if (VR_PREENCODE_EN)
				bDisableVideoPreRecord = AHC_TRUE;
				#endif

				#if (DSC_MODE_ENABLE)
                MenuStatePlaybackSelectDB( CAPTURE_MODE );
                SetCurrentOpMode(CAPTURE_MODE);
             	TVFunc_PreviewCamera();
				#else
				bPreviewModeTVout = AHC_FALSE;
				m_TVStatus = AHC_TV_NONE_STATUS;
				SetCurrentOpMode(MOVPB_MODE);
				TVFunc_Enable(AHC_TRUE);
				#endif
				
				#if (LOCK_NEXTFILE_AT_VR_RESTART)
				m_ubLockNextFile = AHC_FALSE;
				#endif
			}	
		break;
		
		case EVENT_KEY_PREV_MODE:
			if(!VideoFunc_RecordStatus()) {
				VideoTimer_Stop();
				bPreviewModeTVout = AHC_FALSE;
				m_TVStatus = AHC_TV_NONE_STATUS;
		
				SetCurrentOpMode(JPGPB_MODE);
				#if (FOCUS_ON_LATEST_FILE)
	    		AHC_SetPlayFileOrder(PLAY_LAST_FILE);
	    		#endif	
				TVFunc_Enable(AHC_TRUE);
				#ifdef	_OEM_UI_	// For MDRS
				oemUI_SetFocus(AIR_MODE);	// MDRS
				#endif
			}
		break;
	#ifdef NET_SYNC_PLAYBACK_MODE
		case EVENT_NET_ENTER_PLAYBACK : 
			
			if (VideoFunc_RecordStatus())
			{
                //VideoFunc_RecordStop();
				
                #ifdef CFG_REC_FORCE_UNMUTE //may be defined in config_xxx.h
				// unmute recoding everytime when recording starts
				if(bMuteRecord)
					AHC_ToggleMute();
				#endif
                
				//TODO: Truman Add callbacks here
				#ifdef CFG_ENABLE_VIDEO_REC_LED
				RecLED_Timer_Stop();
				#endif

				//UpdateVideoRecordStatus(VIDEO_REC_STOP);
				StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
				
				UIDrawTV_VideoFuncEx(ubID, TV_GUI_VIDEO_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
				//CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
            }
			
			VideoTimer_Stop();
			
			#if (VR_PREENCODE_EN)
			bDisableVideoPreRecord = AHC_TRUE;
			#endif
			
			#if (LOCK_NEXTFILE_AT_VR_RESTART)
			m_ubLockNextFile = AHC_FALSE;
			#endif
			m_TVStatus = AHC_TV_NET_PLAYBACK_STATUS;	
            TVFunc_Enable(AHC_TRUE);
            CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
        break;
    #endif//NET_SYNC_PLAYBACK_MODE
		case EVENT_VIDEO_KEY_RECORD:
			if(VideoFunc_RecordStatus())
			{
				//VideoFunc_RecordStop();
				
                #ifdef CFG_REC_FORCE_UNMUTE //may be defined in config_xxx.h
				// unmute recoding everytime when recording starts
				if(bMuteRecord)
					AHC_ToggleMute();
				#endif
                
				//TODO: Truman Add callbacks here
				#ifdef CFG_ENABLE_VIDEO_REC_LED
				RecLED_Timer_Stop();
				#endif

				//UpdateVideoRecordStatus(VIDEO_REC_STOP);
				StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
				
				UIDrawTV_VideoFuncEx(ubID, TV_GUI_VIDEO_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
				CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
			}
			else
			{
				VideoRecordStatus retVal;

				if(AHC_SDMMC_BasicCheck()==AHC_FALSE) {
					printc("--E-- AHC_SDMMC_BasicCheck fail !!!\r\n");
					CGI_FEEDBACK(ulJobEvent, -1 /* NO SD */);
					break;
				}

                #if (DCF_DB_COUNT >= 2)
                if(uiGetParkingModeEnable() == AHC_TRUE)
                {
                	AHC_UF_SelectDB(DCF_DB_TYPE_2ND_DB);
                }
                else
                {
                    AHC_UF_SelectDB(DCF_DB_TYPE_1ST_DB);
                }
                #endif
                                           
				RecordTimeOffset = 0;
				retVal = VideoFunc_Record();

				switch (retVal) {
				case VIDEO_REC_START:
					#ifdef CFG_ENABLE_VIDEO_REC_VIBRATION
					AHC_Vibration_Enable(CFG_VIDEO_REC_VIBRATION_TIME);
					AHC_OS_SleepMs(CFG_VIDEO_REC_VIBRATION_TIME);
					#endif
					#ifdef CFG_ENABLE_VIDEO_REC_LED
					RecLED_Timer_Start(100);
					#endif

					UpdateVideoRecordStatus(VIDEO_REC_START);
					UIDrawTV_VideoFuncEx(ubID, TV_GUI_VIDEO_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
					break;

                case VIDEO_REC_NO_SD_CARD:
                case VIDEO_REC_CARD_FULL:
                    #ifdef CFG_VIDEO_RECORD_FLICK_LED_BY_CARD_ERROR
                    LedCtrl_FlickerLedByCustomer(LED_GPIO_SDMMC_ERROR, LED_GPIO_SDMMC_FLICK_PERIOD, LED_GPIO_SDMMC_FLICK_TIMES);
                    #endif
                    break;
                }

                #if 0//(BUTTON_BIND_LED)
                if(VIDEO_REC_START != retVal)
                    LedCtrl_ButtonLed(AHC_GetVideoModeLedGPIOpin(), AHC_FALSE);
                #endif

                if (VIDEO_REC_NO_SD_CARD == retVal) {
					CGI_FEEDBACK(ulJobEvent, -1 /* NO SD */);
                    AHC_WMSG_Draw(AHC_TRUE, WMSG_INSERT_SD_AGAIN, 3);
                    break;
                }
                else if (VIDEO_REC_CARD_FULL == retVal) {
					CGI_FEEDBACK(ulJobEvent, -4 /* SD FULL */);
                    AHC_WMSG_Draw(AHC_TRUE, WMSG_STORAGE_FULL, 3);
                    break;
                }
                else if(VIDEO_REC_SEAMLESS_ERROR == retVal){
					CGI_FEEDBACK(ulJobEvent, -5 /* INTERNAL ERROR */);
                    AHC_WMSG_Draw(AHC_TRUE, WMSG_SEAMLESS_ERROR, 3);
                    break;
                }
                else if(VIDEO_REC_SD_CARD_ERROR == retVal){
					CGI_FEEDBACK(ulJobEvent, -3 /* SD ERROR */);
                    AHC_WMSG_Draw(AHC_TRUE, WMSG_FORMAT_SD_CARD, 3);
                    break;
                }
                else if(VIDEO_REC_START == retVal){
					CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
				}
                else {
					CGI_FEEDBACK(ulJobEvent, -3 /* SD ERROR */);
				}
            }

            DrawStateVideoRecUpdate(ulJobEvent);
		break;
		
		case EVENT_KEY_PLAYBACK_MODE:
			if (VideoFunc_RecordStatus()) {
				StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
			}
		
         	VideoTimer_Stop();
         
			#if (VR_PREENCODE_EN)	
			bDisableVideoPreRecord = AHC_TRUE;
			#endif             
        
			bPreviewModeTVout = AHC_FALSE;
			m_TVStatus = AHC_TV_MOVIE_PB_STATUS;
			SetCurrentOpMode(MOVPB_MODE);
			#if (FOCUS_ON_LATEST_FILE)
			AHC_SetPlayFileOrder(PLAY_LAST_FILE);
			#endif

			TVFunc_Enable(AHC_TRUE);
		break;
		
		case EVENT_POWER_OFF:
			if( VideoFunc_RecordStatus() )
			{
				StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
				UIDrawTV_VideoFuncEx(ubID, TV_GUI_VIDEO_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);

				AHC_OS_Sleep(100);
			}
 
			#if (VR_PREENCODE_EN)
			bDisableVideoPreRecord = AHC_TRUE;
			AHC_SetMode(AHC_MODE_IDLE);
			#endif 
 
            AHC_NormalPowerOffPath();
		break;

		//Specail Event
#if (UVC_HOST_VIDEO_ENABLE)
        case EVENT_USB_B_DEVICE_DETECT:
        case EVENT_USB_B_DEVICE_REMOVED:
            StateVideoRecMode(ulJobEvent);            //TBD
            break;
#endif
#if (SUPPORT_DUAL_SNR == 1 && DUALENC_SUPPORT == 1) //For Bitek1603
        case EVENT_TV_DECODER_SRC_TYPE:
			StateVideoRecMode(ulJobEvent);            //TBD
            break;
#endif
		case EVENT_CHANGE_LED_MODE: 
		case EVENT_FORMAT_MEDIA:
		case EVENT_FORMAT_RESET_ALL:
		case EVENT_VIDEO_KEY_CAPTURE:
		case EVENT_LOCK_VR_FILE:
		case EVENT_CHANGE_NIGHT_MODE:
			StateVideoRecMode(ulJobEvent);
		break;
        case EVENT_RECORD_MUTE              :
            AHC_ToggleMute();
        break;
		case EVENT_CAMERA_BROWSER:
		case EVENT_VIDEO_BROWSER:
		case EVENT_ALL_BROWSER:	
			if (VideoFunc_RecordStatus()) {
				StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
			}

         	VideoTimer_Stop();
         
			#if (VR_PREENCODE_EN)
			bDisableVideoPreRecord = AHC_TRUE;
			#endif             
        
			bPreviewModeTVout = AHC_FALSE;
			m_TVStatus = AHC_TV_NONE_STATUS;
			
			if(ulJobEvent==EVENT_CAMERA_BROWSER)
				SetCurrentOpMode(JPGPB_MODE);
			else if(ulJobEvent==EVENT_VIDEO_BROWSER)
				SetCurrentOpMode(MOVPB_MODE);
            else if(ulJobEvent==EVENT_ALL_BROWSER)
				SetCurrentOpMode(JPGPB_MOVPB_MODE);
			
			TVFunc_Enable(AHC_TRUE);
		break;
		
		case EVENT_CAMERA_PREVIEW:
			if(!VideoFunc_RecordStatus())
			{
				VideoTimer_Stop();
			
				#if (VR_PREENCODE_EN)
				bDisableVideoPreRecord = AHC_TRUE;
				#endif
				
				
             	TVFunc_PreviewCamera();
				
				#if (LOCK_NEXTFILE_AT_VR_RESTART)
				m_ubLockNextFile = AHC_FALSE;
				#endif
			}
		break;
		
		case EVENT_AUDIO_RECORD:
		case EVENT_SWITCH_VMD_MODE:
		case EVENT_SWITCH_PANEL_TVOUT:
		case EVENT_LOCK_FILE_G:
			StateVideoRecMode(ulJobEvent);
		break;

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
        case EVENT_VRCB_RECDSTOP_CARDSLOW:
       		StateVideoRecMode(ulJobEvent);
       	break;
       	
		//Update Message
		case EVENT_VIDREC_UPDATE_MESSAGE:
		case EVENT_SHOW_FW_VERSION:
		case EVENT_SHOW_GPS_INFO:	//TBD
		case EVENT_GPSGSENSOR_UPDATE:
			StateVideoRecMode(ulJobEvent);
		break;
				
		default:
		break;			
	}
#endif	
}

void StateTV_CameraMode_Handler(UINT32 ulJobEvent)
{
#if (TVOUT_PREVIEW_EN) && (DSC_MODE_ENABLE==1)

	if(m_TVStatus!=AHC_TV_DSC_PREVIEW_STATUS)
		return;

    if(IN_MENU())
    {
 		if(SetTVMenuHandler(ulJobEvent) == 0)
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
        	StateCaptureMode(ulJobEvent);
        break; 
        
        case EVENT_KEY_MENU:
        	StateTV_EventMenu_Handler(ulJobEvent);
        break;

		case EVENT_DSC_KEY_CHANGE_SHOT_MODE       :
#if (DSC_MULTI_MODE != DSC_MULTI_DIS)
			if (DSC_ONE_SHOT == m_ubDSCMode) {
				#if (DSC_MULTI_MODE & DSC_BURST_SHOT)
				m_ubDSCMode	= DSC_BURST_SHOT;
				RTNA_DBG_Str(0, " ===> DSC_BURST_SHOT\r\n");
				#elif (DSC_MULTI_MODE & DSC_TIME_LAPSE)
				m_ubDSCMode	= DSC_TIME_LAPSE;
				RTNA_DBG_Str(0, " ===> DSC_TIME_LAPSE\r\n");
				#endif
			}
			else if (DSC_BURST_SHOT == m_ubDSCMode) {
				#if (DSC_MULTI_MODE & DSC_TIME_LAPSE)
				m_ubDSCMode	= DSC_TIME_LAPSE;
				RTNA_DBG_Str(0, " ===> DSC_TIME_LAPSE\r\n");
				#elif (DSC_MULTI_MODE & DSC_ONE_SHOT)
				m_ubDSCMode	= DSC_ONE_SHOT;
				RTNA_DBG_Str(0, " ===> DSC_ONE_SHOT\r\n");
				#endif
			}
			else if (DSC_TIME_LAPSE == m_ubDSCMode) {
				if (AHC_FALSE == m_ubTimeLapseStart) {
					#if (DSC_MULTI_MODE & DSC_ONE_SHOT)
					m_ubDSCMode	= DSC_ONE_SHOT;
					RTNA_DBG_Str(0, " ===> DSC_ONE_SHOT\r\n");
					#elif (DSC_MULTI_MODE & DSC_BURST_SHOT)
					m_ubDSCMode	= DSC_BURST_SHOT;
					RTNA_DBG_Str(0, " ===> DSC_BURST_SHOT\r\n");
					#endif
				}
			}
#endif
		break;
	#ifdef NET_SYNC_PLAYBACK_MODE
		case EVENT_NET_ENTER_PLAYBACK :
			
			CaptureTimer_Stop();
			gsStillZoomIndex = ZoomCtrl_GetStillDigitalZoomLevel();
			
			bPreviewModeTVout = AHC_FALSE;
			m_TVStatus = AHC_TV_NET_PLAYBACK_STATUS;
			MenuStatePlaybackSelectDB( MOVPB_MODE );
			
			#if (FOCUS_ON_LATEST_FILE)
    		AHC_SetPlayFileOrder(PLAY_LAST_FILE);
    		#endif	
			TVFunc_Enable(AHC_TRUE);
			
            CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
		break;
	#endif
        case EVENT_KEY_MODE:
       	case EVENT_KEY_NEXT_MODE:
			CaptureTimer_Stop();
			gsStillZoomIndex = ZoomCtrl_GetStillDigitalZoomLevel();
			
	#if (CYCLIC_MODE_CHANGE)
			bPreviewModeTVout = AHC_FALSE;
			m_TVStatus = AHC_TV_NONE_STATUS;
			MenuStatePlaybackSelectDB( MOVPB_MODE );
			SetCurrentOpMode(MOVPB_MODE);
			#if (FOCUS_ON_LATEST_FILE)
    		AHC_SetPlayFileOrder(PLAY_LAST_FILE);
    		#endif	
			TVFunc_Enable(AHC_TRUE);
	#else
			TVFunc_PreviewVideo();
	#endif
	
        break; 
        
        case EVENT_KEY_PREV_MODE:
			CaptureTimer_Stop();
			gsStillZoomIndex = ZoomCtrl_GetStillDigitalZoomLevel();
			TVFunc_PreviewVideo();
        break;

		case EVENT_DSC_KEY_CAPTURE:
			StateCaptureMode(ulJobEvent);
		break;
        
        case EVENT_KEY_PLAYBACK_MODE:
        	CaptureTimer_Stop();
			gsStillZoomIndex = ZoomCtrl_GetStillDigitalZoomLevel();

			bPreviewModeTVout = AHC_FALSE;
			m_TVStatus = AHC_TV_NONE_STATUS;
	
			SetCurrentOpMode(JPGPB_MODE);
			#if (FOCUS_ON_LATEST_FILE)
    		AHC_SetPlayFileOrder(PLAY_LAST_FILE);
    		#endif	
			TVFunc_Enable(AHC_TRUE);
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
         	CaptureTimer_Stop();
			gsStillZoomIndex = ZoomCtrl_GetStillDigitalZoomLevel();

			bPreviewModeTVout = AHC_FALSE;
			m_TVStatus = AHC_TV_NONE_STATUS;
	
			if(ulJobEvent==EVENT_CAMERA_BROWSER)
				SetCurrentOpMode(JPGPB_MODE);
			else if(ulJobEvent==EVENT_VIDEO_BROWSER)
				SetCurrentOpMode(MOVPB_MODE);

			#if (FOCUS_ON_LATEST_FILE)
    		AHC_SetPlayFileOrder(PLAY_LAST_FILE);
    		#endif	
			TVFunc_Enable(AHC_TRUE);
        break;
        
		case EVENT_VIDEO_PREVIEW:
         	CaptureTimer_Stop();
			gsStillZoomIndex = ZoomCtrl_GetStillDigitalZoomLevel();
			TVFunc_PreviewVideo();
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
        
     	default:
     	break;   
	}        
#endif
}

void StateTV_BrowserMode_Handler(UINT32 ulJobEvent)
{
	if(m_TVStatus!=AHC_TV_BROWSER_STATUS)
		return;

    if(IN_MENU())
    {
 		if(SetTVMenuHandler(ulJobEvent) == 0)
    		return;
    }

	switch(ulJobEvent)
	{
		case EVENT_NONE                           :
        break;	

		case EVENT_KEY_UP:
			CHARGE_ICON_ENABLE(AHC_FALSE);
			TVFunc_ThumbnailOption(BROWSER_UP);
			DrawStateTVBrowserUpdate(ulJobEvent);
			CHARGE_ICON_ENABLE(AHC_TRUE);
		break;
		
		case EVENT_KEY_DOWN:
			CHARGE_ICON_ENABLE(AHC_FALSE);
			TVFunc_ThumbnailOption(BROWSER_DOWN);
			DrawStateTVBrowserUpdate(ulJobEvent);
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
				TVFunc_ThumbnailOption(BROWSER_LEFT);
				DrawStateTVBrowserUpdate(ulJobEvent);
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
				TVFunc_ThumbnailOption(BROWSER_RIGHT);
				DrawStateTVBrowserUpdate(ulJobEvent);
				CHARGE_ICON_ENABLE(AHC_TRUE);
			}
		break;

		case EVENT_VIDEO_KEY_RECORD:
		case EVENT_THUMBNAIL_KEY_PLAYBACK:
			if(Deleting_InBrowser || Protecting_InBrowser)
			{
				BrowserFunc_ThumbnailEditFile_Option(ulJobEvent);
			}
			else
			{
				TVFunc_ThumbnailOption(BROWSER_PLAY);
			}
		break;
		
		case EVENT_KEY_MENU:
			StateTV_EventMenu_Handler(ulJobEvent);
		break;
		
		case EVENT_KEY_RETURN:
			#ifdef	_OEM_UI_	// For MDRS
			oemUI_SetFocus(ANCHOR_MODE);	// MRDS
			#endif
			break;
	#ifdef NET_SYNC_PLAYBACK_MODE
		case EVENT_NET_ENTER_PLAYBACK :
			bPreviewModeTVout = AHC_TRUE;
			MenuStatePlaybackSelectDB( MOVPB_MODE );    
            SetCurrentOpMode(MOVPB_MODE);
            TVFunc_BrowserSetting();
			BrowserFunc_ThumbnailResetAllEditOp();
			m_TVStatus = AHC_TV_NET_PLAYBACK_STATUS;
			TVFunc_Enable(AHC_TRUE);
            CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
		break;
	#endif
		case EVENT_KEY_MODE:
		case EVENT_KEY_NEXT_MODE:
			if(AHC_FALSE == AHC_SDMMC_GetMountState())
			{
				#if (TVOUT_PREVIEW_EN)
				TVFunc_PreviewVideo();
				#endif
			}
			else
			{
			#if (TVOUT_PREVIEW_EN)
				
				#if (CYCLIC_MODE_CHANGE)
				if(MOVPB_MODE == GetCurrentOpMode())
				{
				#if (DSC_MODE_ENABLE)
					bPreviewModeTVout = AHC_FALSE;
					TVFunc_ThumbnailModeSwitch();
					DrawStateTVBrowserUpdate(ulJobEvent);
				#else
					BrowserFunc_ThumbnailResetAllEditOp(); 
					TVFunc_PreviewVideo();
				#endif
				}
				else if (JPGPB_MODE == GetCurrentOpMode())
				{
			        bPreviewModeTVout = AHC_TRUE;
                    AHC_SetMode(AHC_MODE_IDLE);
			        MenuStatePlaybackSelectDB( MOVPB_MODE );    
                    SetCurrentOpMode(MOVPB_MODE);
                    TVFunc_BrowserSetting();
					BrowserFunc_ThumbnailResetAllEditOp(); 
					TVFunc_PreviewVideo();
				}
				#else
				bPreviewModeTVout = AHC_FALSE;
				TVFunc_ThumbnailModeSwitch();
				DrawStateTVBrowserUpdate(ulJobEvent);
				#endif
				
			#else
				#if (DSC_MODE_ENABLE)
				TVFunc_ThumbnailModeSwitch();
				DrawStateTVBrowserUpdate(ulJobEvent);
				#endif
			#endif
			}		
		break;
		
		case EVENT_KEY_PREV_MODE:
			#if (TVOUT_PREVIEW_EN)
				
				#if (CYCLIC_MODE_CHANGE)
				if(MOVPB_MODE == GetCurrentOpMode())
				{	// Back To Camera Preview
	             	TVFunc_PreviewCamera();
				}
				else if (JPGPB_MODE == GetCurrentOpMode())
				{
					bPreviewModeTVout = AHC_FALSE;
					m_TVStatus = AHC_TV_NONE_STATUS;
			
					SetCurrentOpMode(MOVPB_MODE);
					#if (FOCUS_ON_LATEST_FILE)
		    		AHC_SetPlayFileOrder(PLAY_LAST_FILE);
		    		#endif	
					TVFunc_Enable(AHC_TRUE);
				}
				#else
				bPreviewModeTVout = AHC_FALSE;
				TVFunc_ThumbnailModeSwitch();
				DrawStateTVBrowserUpdate(ulJobEvent);
				#endif
				
			#else
				#if (DSC_MODE_ENABLE)	
				TVFunc_ThumbnailModeSwitch();
				DrawStateTVBrowserUpdate(ulJobEvent);
				#endif
			#endif
		break;
		case EVENT_KEY_PLAYBACK_MODE:
			#if (TVOUT_PREVIEW_EN)
			if(MOVPB_MODE == GetCurrentOpMode())
			{	
				BrowserFunc_ThumbnailResetAllEditOp();
				TVFunc_PreviewVideo();
			}
			else if(JPGPB_MODE == GetCurrentOpMode())
			{
				BrowserFunc_ThumbnailResetAllEditOp();
                TVFunc_PreviewCamera();
			}
			else if(JPGPB_MOVPB_MODE == GetCurrentOpMode())
			{	
				BrowserFunc_ThumbnailResetAllEditOp();
				TVFunc_PreviewVideo();
			}
			#endif
		break;
		
		case EVENT_SLIDESHOW_MODE:
			if(Deleting_InBrowser || Protecting_InBrowser)
				break;
		
			BrowserFunc_ThumbnailResetAllEditOp();
			
			if(TVFunc_SlideShow())
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
		
		#if (TVOUT_PREVIEW_EN)
		case EVENT_CAMERA_PREVIEW:
			BrowserFunc_ThumbnailResetAllEditOp();
			TVFunc_PreviewCamera();
		break;
		
		case EVENT_VIDEO_PREVIEW:
			BrowserFunc_ThumbnailResetAllEditOp();
			TVFunc_PreviewVideo();
		break;		
		#endif
	
		case EVENT_SWITCH_PANEL_TVOUT:
			StateBrowserMode(ulJobEvent);
		break;
		
		//Update Message
		case EVENT_THUMBNAIL_UPDATE_MESSAGE		 :
			StateBrowserMode(ulJobEvent);
        break;
        case EVENT_PARKING_KEY:
			if( GetCurrentOpMode() == MOVPB_MODE || GetCurrentOpMode() == JPGPB_MODE )
            {
				bPreviewModeTVout = AHC_TRUE;
                m_ubPlaybackRearCam = (m_ubPlaybackRearCam == AHC_TRUE)? AHC_FALSE : AHC_TRUE;
        		TVFunc_BrowserSetting();
                #if VIDRECD_MULTI_TRACK
            	if( m_ubPlaybackRearCam == AHC_TRUE )
            	{
            		MMPS_VIDPLAY_SetCurrentTrack(1);  //playback rear cam.
            	}
            	else
            	{
            		MMPS_VIDPLAY_SetCurrentTrack(0);  //playback front cam.
            	}
            	#endif
                AHC_Thumb_DrawPage(AHC_TRUE);
        		DrawStateTVBrowserUpdate(ulJobEvent);
            }
        break;
        
        default:
        break;
	}
}

void StateTV_MoviePBMode_Handler(UINT32 ulJobEvent)
{
	UINT8 	ret;
	UINT32	uState;

	if(m_TVStatus!=AHC_TV_MOVIE_PB_STATUS)
		return;

    if(IN_MENU())
    {
 		if(SetTVMenuHandler(ulJobEvent) == 0)
    		return;
    }

	switch(ulJobEvent)
	{
		case EVENT_NONE                           :
        break;
        
        case EVENT_KEY_UP:
        case EVENT_KEY_DOWN:
        case EVENT_KEY_WIDE_STOP:
        case EVENT_KEY_TELE_STOP:
        	//StateMoviePlaybackMode(ulJobEvent);
        break;	
    #ifdef NET_SYNC_PLAYBACK_MODE
		case EVENT_NET_ENTER_PLAYBACK :
			MovPBTimer_Stop();
			MovPlaybackParamReset();
			m_TVStatus = AHC_TV_NET_PLAYBACK_STATUS;
			TVFunc_Enable(AHC_TRUE);
            CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
		break;
	#endif
        case EVENT_MOVPB_TOUCH_BKWD_PRESS:
			if(m_DelFile==DEL_FILE_SELECT_NO) {
				m_DelFile = DEL_FILE_SELECT_YES; 
				DrawStateTVMoviePBUpdate(EVENT_KEY_DOWN);
			}
			else if(m_DelFile==DEL_FILE_NONE){
				TVFunc_MoviePBBackward();
	            DrawStateTVMoviePBUpdate(EVENT_KEY_LEFT);
			}        
        break;
	
		case EVENT_MOVPB_TOUCH_FRWD_PRESS: 
			if(m_DelFile==DEL_FILE_SELECT_YES) {
				m_DelFile = DEL_FILE_SELECT_NO; 
				DrawStateTVMoviePBUpdate(EVENT_KEY_DOWN);
			}
			else if(m_DelFile==DEL_FILE_NONE){
	        	TVFunc_MoviePBForward();
	            DrawStateTVMoviePBUpdate(EVENT_KEY_RIGHT);
			}		
		break;
	
		case EVENT_MOVPB_TOUCH_PLAY_PRESS:
			if(m_DelFile!=DEL_FILE_NONE) 
			{     
				DrawStateTVMoviePBUpdate(EVENT_TV_BROWSER_KEY_PLAYBACK);
		                     
				if(m_DelFile == DEL_FILE_SELECT_YES) 
				{
					if(!TVFunc_MoviePBDelFile()) 
					{
						MovPBTimer_Stop();
		                m_TVStatus = AHC_TV_NONE_STATUS;
		                TVFunc_Enable(AHC_TRUE);
		                break;
					}   
		                     
					m_DelFile = DEL_FILE_NONE;
		            DrawStateTVMoviePBUpdate(EVENT_KEY_DOWN);
		            DrawStateTVMoviePBUpdate(EVENT_TV_DEL_FILE);
				}
				else if(m_DelFile == DEL_FILE_SELECT_NO)
				{
		           	m_DelFile = DEL_FILE_NONE;
		            DrawStateTVMoviePBUpdate(EVENT_KEY_DOWN);
		            DrawStateTVMoviePBUpdate(EVENT_MOVPB_TOUCH_PAUSE_PRESS);
				}  
			}
			else 
			{
				TVFunc_MoviePBPlayPress();
			} 		
		break;
		
		case EVENT_MOVPB_TOUCH_RETURN:
			m_DelFile = DEL_FILE_NONE;
			MovPBTimer_Stop();
			m_bMovieOSDForceShowUp = AHC_FALSE;
			MovPlaybackParamReset();
			m_TVStatus = AHC_TV_NONE_STATUS;

			{
			    UINT32 		uState;
				
   			 AHC_GetSystemStatus(&uState);
			 if(STATE_MOV_EXECUTE == uState)
			 	MovPlayback_Pause();
			}

			TVFunc_Enable(AHC_TRUE);
		break;
		
		case EVENT_KEY_MENU:
			StateTV_EventMenu_Handler(ulJobEvent);
		break;
		
		case EVENT_POWER_OFF:
			StateMoviePlaybackMode(ulJobEvent);
		break;
		
		//Special Event
		case EVENT_MOVPB_TOUCH_PREV_PRESS:
			TVFunc_MoviePBPrevious();
			DrawStateTVMoviePBUpdate(ulJobEvent);
		break;

		case EVENT_MOVPB_TOUCH_NEXT_PRESS:
			TVFunc_MoviePBNext();
			DrawStateTVMoviePBUpdate(ulJobEvent);
		break;
		 
		#if (TVOUT_PREVIEW_EN)
		case EVENT_VIDEO_PREVIEW:
			MovPBTimer_Stop();
			MovPlaybackParamReset();
			TVFunc_PreviewVideo();
		break;
	
		case EVENT_CAMERA_PREVIEW:
			MovPBTimer_Stop();
			MovPlaybackParamReset();
			TVFunc_PreviewCamera();
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
				m_TVStatus = AHC_TV_NONE_STATUS;
				TVFunc_Enable(AHC_TRUE); 
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
                        	
					DrawStateTVMoviePBUpdate(EVENT_TV_PLAY_STOP); 
					
                    #ifdef CFG_TV_MOVPB_BACK_TO_BROWSER //may be defined in config_xxx.h
					// needs return to browser mode
					m_TVStatus = AHC_TV_NONE_STATUS;
					TVFunc_Enable(AHC_TRUE);
					#endif 
				}
				else if(uState == AHC_VIDEO_PLAY_FF_EOS || uState == AHC_VIDEO_PLAY_BW_BOS)
				{
					#if ( MOVIE_PB_FF_EOS_ACTION==EOS_ACT_RESTART || \
						  MOVIE_PB_FF_EOS_ACTION==EOS_ACT_PAUSE_AT_FIRST_FRAME )
					TVFunc_MoviePBSeekSOS();
					#endif
				}
			}
			else if(MOV_STATE_PLAY == GetMovConfig()->iState || MOV_STATE_FF == GetMovConfig()->iState)
			{
				DrawStateTVMoviePBUpdate(ulJobEvent); 
			}        
        break;
        
        default:
        break;
	}
}        

void StateTV_PhotoPBMode_Handler(UINT32 ulJobEvent)
{
	UINT8 ret;

	if(m_TVStatus!=AHC_TV_PHOTO_PB_STATUS)
		return;

    if(IN_MENU())
    {
 		if(SetTVMenuHandler(ulJobEvent) == 0)
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
				DrawStateTVPhotoPBUpdate(EVENT_KEY_DOWN);
			}
			else if(m_DelFile==DEL_FILE_NONE){
				if(TVFunc_PhotoPBPrevious())
					DrawStateTVPhotoPBUpdate(EVENT_KEY_LEFT); 
			}        
        break;
        
        case EVENT_KEY_RIGHT:
			if(m_DelFile==DEL_FILE_SELECT_YES) {
				m_DelFile = DEL_FILE_SELECT_NO; 
				DrawStateTVPhotoPBUpdate(EVENT_KEY_DOWN);
			}
			else if(m_DelFile==DEL_FILE_NONE){
				if(TVFunc_PhotoPBNext())
					DrawStateTVPhotoPBUpdate(EVENT_KEY_RIGHT);
			}        
        break;
        case EVENT_TV_BROWSER_KEY_PLAYBACK:
        case EVENT_KEY_SET:
        	StatePhotoPlaybackMode(EVENT_TV_BROWSER_KEY_PLAYBACK);
        break;
        
        case EVENT_JPGPB_TOUCH_RETURN:
			m_DelFile = DEL_FILE_NONE;
			PhotoPBTimer_Stop();
			JpgPlaybackParamReset();
			m_TVStatus = AHC_TV_NONE_STATUS;
			TVFunc_Enable(AHC_TRUE);         
        break;
        
        case EVENT_KEY_MENU:
        	StateTV_EventMenu_Handler(ulJobEvent);
        break;
        
        case EVENT_POWER_OFF:
        	AHC_NormalPowerOffPath();
        break;

		//Special Event
        case EVENT_JPGPB_TOUCH_PREV_PRESS:
			if(TVFunc_PhotoPBPrevious())
				DrawStateTVPhotoPBUpdate(EVENT_KEY_LEFT); 
        break;
        
        case EVENT_JPGPB_TOUCH_NEXT_PRESS:
 			if(TVFunc_PhotoPBNext())
				DrawStateTVPhotoPBUpdate(EVENT_KEY_RIGHT);       
        break;
    #ifdef NET_SYNC_PLAYBACK_MODE
		case EVENT_NET_ENTER_PLAYBACK :
			PhotoPBTimer_Stop();
			JpgPlaybackParamReset();
			m_TVStatus = AHC_TV_NET_PLAYBACK_STATUS;
			TVFunc_Enable(AHC_TRUE);
            CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
		break;
	#endif
        #if (TVOUT_PREVIEW_EN)
		case EVENT_VIDEO_PREVIEW:
			PhotoPBTimer_Stop();
			JpgPlaybackParamReset();
			TVFunc_PreviewVideo();
		break;
	
		case EVENT_CAMERA_PREVIEW:
			PhotoPBTimer_Stop();
			JpgPlaybackParamReset();
			TVFunc_PreviewCamera();		
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
				m_TVStatus = AHC_TV_NONE_STATUS;
				TVFunc_Enable(AHC_TRUE); 
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

void StateTV_SlideShowMode_Handler(UINT32 ulJobEvent)
{
#if (SLIDESHOW_EN)

	UINT8 	ret;
	UINT32 	uState;
	
	if(m_TVStatus!=AHC_TV_SLIDE_SHOW_STATUS)
		return;

    if(IN_MENU())
    {
 		if(SetTVMenuHandler(ulJobEvent) == 0)
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
			DrawStateTVSlideShowUpdate(EVENT_SLIDESHOW_UPDATE_FILE);       
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
			DrawStateTVSlideShowUpdate(EVENT_SLIDESHOW_UPDATE_FILE);         
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
							m_TVStatus = AHC_TV_NONE_STATUS;
							TVFunc_Enable(AHC_TRUE);
	                        AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);    
	                        return;
	                    }
	                    DrawStateTVMoviePBUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);
					break;
	                         	
					case STATE_MOV_EXECUTE :
						if(MOV_STATE_FF == GetMovConfig()->iState)
						{
							MovPlayback_NormalSpeed();
							DrawStateTVMoviePBUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);
						}
						else
						{
							MovPlayback_Pause();
							DrawStateTVMoviePBUpdate(EVENT_MOVPB_TOUCH_PAUSE_PRESS); 
						}
					break;

					case STATE_MOV_PAUSE   :
						AHC_PlaybackClipCmd(AHC_PB_MOVIE_NORMAL_PLAY_SPEED, 0);
						MovPlayback_Resume();
						DrawStateTVMoviePBUpdate(EVENT_MOVPB_TOUCH_PLAY_PRESS);
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
		        AHC_UF_SetFreeChar(0, DCF_SET_ALLOWED, (UINT8*)SEARCH_PHOTO);  
			}
			else 
			{
				MovPlaybackParamReset();    
				SetCurrentOpMode(MOVPB_MODE);
				AHC_UF_SetFreeChar(0, DCF_SET_ALLOWED, (UINT8*)SEARCH_MOVIE);  
			}
			m_TVStatus = AHC_TV_NONE_STATUS;
			TVFunc_Enable(AHC_TRUE); 	
		break;
	#ifdef NET_SYNC_PLAYBACK_MODE
		case EVENT_NET_ENTER_PLAYBACK :
			SlideShowTimer_Stop();
			m_TVStatus = AHC_TV_NET_PLAYBACK_STATUS;
			TVFunc_Enable(AHC_TRUE);
            CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
		break;
	#endif
		case EVENT_KEY_MODE:
			//TBD
		break;
		
		case EVENT_KEY_MENU:
			StateTV_EventMenu_Handler(ulJobEvent);
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
					DrawStateTVSlideShowUpdate(EVENT_SLIDESHOW_UPDATE_FILE);
				}

				bSlideShowNext 	= AHC_FALSE;
				bSlideShow 		= AHC_TRUE;
			}
		break;
		
		case EVENT_SLIDESHOW_UPDATE_MESSAGE:
			DrawStateTVSlideShowUpdate(EVENT_MOVPB_UPDATE_MESSAGE);
		break;
	}
#endif	
}

void StateTV_Sleep_Handler(UINT32 ulJobEvent)
{
}

void StateTV_NetPlayback_Handler(UINT32 ulJobEvent)
{
    switch(ulJobEvent)
    {
    	case EVENT_NET_EXIT_PLAYBACK :

    		bPreviewModeTVout = AHC_TRUE;
            AHC_SetMode(AHC_MODE_IDLE);
			MenuStatePlaybackSelectDB( MOVPB_MODE );    
            SetCurrentOpMode(MOVPB_MODE);
            TVFunc_BrowserSetting();
			BrowserFunc_ThumbnailResetAllEditOp(); 
			TVFunc_PreviewVideo();
    		CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
    		
    	break;
    }	
}	
#endif

//******************************************************************************
//
//                              AHC State TV Mode
//
//******************************************************************************
/*
 * New TV Mode for TV Preview enabled
 */
void StateTVMode(UINT32 ulEvent)
{
#if (TVOUT_ENABLE)
	
	if(ulEvent == EVENT_NONE)
	    return;
	
    if(!TVFunc_MoviePBCheckOSDShowUp(ulEvent))
        return;

    if(!TVFunc_PhotoPBCheckOSDShowUp(ulEvent))
        return;

    if(!TVFunc_SlideShowCheckOSDShowUp(ulEvent))
        return;  
              
    switch(ulEvent)
    {
        case EVENT_NONE                           :  
        break;
        	
		//Device
		case EVENT_DC_CABLE_IN:
			if(m_TVStatus==AHC_TV_VIDEO_PREVIEW_STATUS)
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

			if(m_TVStatus==AHC_TV_VIDEO_PREVIEW_STATUS)
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
				if(m_TVStatus==AHC_TV_VIDEO_PREVIEW_STATUS)
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
				#if 1 // Lock TV state at non-plugged state when USB plug-in
				    // Will be unlock when USB plug-out
				 AHC_LockTVConnectionStatus(AHC_TRUE, AHC_FALSE);   
				 StateTVMode(EVENT_TV_REMOVED) ;
				
				#else
					#ifdef CFG_REC_IGNORE_USB //may be defined in config_xxx.h
					if (VideoFunc_RecordStatus()) {
						// Video Recording to ignore USB
						break;
					}	
					else {
						VideoTimer_Stop();
					}
					#else
					if( VideoFunc_RecordStatus() )
					{
						StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
						AHC_WaitVideoWriteFileFinish();
					}

					VideoTimer_Stop();
					#endif

					#if (VR_PREENCODE_EN)
					bDisableVideoPreRecord = AHC_TRUE;
					#endif

					AHC_OSDUninit();
					AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);

					// Lock TV state at non-plugged state when USB plug-in
					// Will be unlock when USB plug-out
					AHC_LockTVConnectionStatus(AHC_TRUE, AHC_FALSE);

	    	        InitOSD();

	                // Exit TV mode
					TVFunc_Enable(AHC_FALSE);
					m_TVStatus = AHC_TV_NONE_STATUS;
				#endif
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
#ifdef CFG_TV_FORCE_POWER_OFF_WHEN_REMOVING_USB //may be defined in config_xxx.h
			AHC_SetMode(AHC_MODE_IDLE);
    	 	bForce_PowerOff = AHC_TRUE;
    	 	AHC_NormalPowerOffPath();
#else	         
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
		     	
				if(m_TVStatus==AHC_TV_VIDEO_PREVIEW_STATUS)
				{
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
				}
				else
				{
					#if (CHARGER_OUT_ACT_OTHER_MODE==ACT_FORCE_POWER_OFF || CHARGER_OUT_ACT_OTHER_MODE==ACT_DELAY_POWER_OFF)
					AHC_SetShutdownByChargerOut(AHC_TRUE);
					#endif
				}
			}
#endif
        break;

        case EVENT_SD_DETECT                      :
            AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect);
            AHC_OS_Sleep(100);

			if (IN_MENU()) {
				// in menu, do nothing
		    	return;
			}
			if(!TVFunc_IsPreview())
			{
            	m_TVStatus = AHC_TV_NONE_STATUS;
            	TVFunc_Enable(AHC_TRUE);
            } 
        break;
        
        case EVENT_SD_REMOVED                     :
			if (IN_MENU()) {
				// in menu, do nothing
		    	return;
			}
			switch(m_TVStatus) 
			{
#if (TVOUT_PREVIEW_EN)
				case AHC_TV_VIDEO_PREVIEW_STATUS:
					if(VideoFunc_RecordStatus())
           		 	{
                		StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
            		}
				break;
				case AHC_TV_DSC_PREVIEW_STATUS:
					// TODO: Do something??
				break;
#endif	
				case AHC_TV_BROWSER_STATUS:
					// TODO: Do something??
				break;
				case AHC_TV_MOVIE_PB_STATUS:

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

				case AHC_TV_PHOTO_PB_STATUS:
					PhotoPBTimer_Stop();
					JpgPlaybackParamReset();
				break;
				
				case AHC_TV_SLIDE_SHOW_STATUS:
    				SlideShowFunc_Stop();
    				SlideShowTimer_Stop();
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
			
			if(!bPreviewModeTVout)
			{
				m_TVStatus = AHC_TV_NONE_STATUS;
				TVFunc_Enable(AHC_TRUE);
        	}
        break;
        
        case EVENT_TV_REMOVED:
        case EVENT_TV_KEY_LCD_OUTPUT:
        
        	m_ubInTVMode = AHC_FALSE;
        	
			switch(m_TVStatus) 
			{
				case AHC_TV_BROWSER_STATUS:
					AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_MAIN_LCD);
					TVFunc_Enable(AHC_FALSE);
					m_TVStatus = AHC_TV_NONE_STATUS;
				break;
	             
				case AHC_TV_MOVIE_PB_STATUS:
					AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_MAIN_LCD);
					AHC_GetVideoCurrentPlayTime(&m_VideoCurPlayTime);
					AHC_OSDUninit();
					AHC_SetMode(AHC_MODE_IDLE); 
	                 
					AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
					InitOSD();
					AHC_UF_GetCurrentIndex(&ObjSelect);
					AHC_SetVideoPlayStartTime(m_VideoCurPlayTime);
					
					m_TVStatus = AHC_TV_NONE_STATUS;
					bMovPbExitFromTV = AHC_TRUE;
					UpdateMovPBStatus(bTVMovieState);

		            #ifdef CFG_TV_PLUGOUT_RETURN_VIDEO_PREVIEW
					// RESET play progress
		            m_VideoCurPlayTime = 0;
					AHC_SetVideoPlayStartTime(m_VideoCurPlayTime);
					StateSwitchMode(UI_VIDEO_STATE);
					#else
					StateSwitchMode(UI_PLAYBACK_STATE);
					#endif
					RTNA_LCD_Backlight(MMP_TRUE);
				break;
	             
				case AHC_TV_PHOTO_PB_STATUS:
					AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_MAIN_LCD);
					AHC_OSDUninit();
					AHC_SetMode(AHC_MODE_IDLE); 
	                 
					AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
					InitOSD();
					AHC_UF_GetCurrentIndex(&ObjSelect);
					m_TVStatus = AHC_TV_NONE_STATUS;
		            #ifdef CFG_TV_PLUGOUT_RETURN_VIDEO_PREVIEW
					StateSwitchMode(UI_VIDEO_STATE);
					#else
					StateSwitchMode(UI_PLAYBACK_STATE);
					#endif
					RTNA_LCD_Backlight(MMP_TRUE);
				break;

				case AHC_TV_SLIDE_SHOW_STATUS:
					AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_MAIN_LCD);
					AHC_GetVideoCurrentPlayTime(&m_VideoCurPlayTime);
					AHC_OSDUninit();
					AHC_SetMode(AHC_MODE_IDLE); 

					AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
					InitOSD();
					AHC_UF_GetCurrentIndex(&ObjSelect);
					AHC_SetVideoPlayStartTime(m_VideoCurPlayTime);
					bSlideShowFirstFile = AHC_FALSE;
					m_TVStatus = AHC_TV_NONE_STATUS;
		            #ifdef CFG_TV_PLUGOUT_RETURN_VIDEO_PREVIEW
					StateSwitchMode(UI_VIDEO_STATE);
					#else
					StateSwitchMode(UI_SLIDESHOW_STATE);
					#endif
					RTNA_LCD_Backlight(MMP_TRUE);
				break; 
				
				#if (TVOUT_PREVIEW_EN)
				case AHC_TV_VIDEO_PREVIEW_STATUS:
				
					if(VideoFunc_RecordStatus())
					{
						StateVideoRecMode_StopRecordingProc(EVENT_VIDEO_KEY_RECORD);
					}

					VideoTimer_Stop();

					#if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
		            if( uiGetParkingModeEnable() == AHC_TRUE )
		             {
		                    AIHC_StopVideoRecordMode( AHC_TRUE, AHC_FALSE ); // Stop video Pre-record before uiSetParkingModeEnable( AHC_FALSE ) to avoid un-expected AHC_UF_PostAddFile()
		                    uiSetParkingModeEnable( AHC_FALSE );

		                    #if (MOTION_DETECTION_EN)
		         			if( MenuSettingConfig()->uiMotionDtcSensitivity != MOTION_DTC_SENSITIVITY_OFF )
		                    {
		                        MenuSettingConfig()->uiMotionDtcSensitivity = MOTION_DTC_SENSITIVITY_OFF;
		                    }
		                    Menu_SetMotionDtcSensitivity( MenuSettingConfig()->uiMotionDtcSensitivity );				
		                    #endif
		             }
		             #endif
                    AHC_OSDUninit();
					AHC_SetMode(AHC_MODE_IDLE); 

					AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
					InitOSD();
					m_TVStatus = AHC_TV_NONE_STATUS;
					
					AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_MAIN_LCD);
					bPreviewModeTVout = AHC_FALSE;
					
					StateSwitchMode(UI_VIDEO_STATE);
					RTNA_LCD_Backlight(MMP_TRUE);
				break;
				
				case AHC_TV_DSC_PREVIEW_STATUS:
					CaptureTimer_Stop();
					
					AHC_OSDUninit();
					AHC_SetMode(AHC_MODE_IDLE); 

					AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
					InitOSD();
					m_TVStatus = AHC_TV_NONE_STATUS;
					
					AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_MAIN_LCD);
					bPreviewModeTVout = AHC_FALSE;
					
					StateSwitchMode(UI_CAMERA_STATE);
					RTNA_LCD_Backlight(MMP_TRUE);
				break;
				#endif
				case AHC_TV_NET_PLAYBACK_STATUS:
					AHC_OSDUninit();
					AHC_SetMode(AHC_MODE_IDLE); 

					AHC_SetDisplayOutputDev(DISP_OUT_LCD, AHC_DISPLAY_DUPLICATE_1X);
					InitOSD();
					m_TVStatus = AHC_TV_NONE_STATUS;
					
					AHC_SetCurrentDisplayEx(MMP_DISPLAY_SEL_MAIN_LCD);
					bPreviewModeTVout = AHC_FALSE;
					
					StateSwitchMode(UI_NET_PLAYBACK_STATE);
					RTNA_LCD_Backlight(MMP_TRUE);
				break;
			}

			#if (POWER_OFF_CONFIRM_PAGE_EN)
			if(PowerOff_InProcess)
			{
				MenuDrawSubPage_PowerOff();
			}
			#endif
        break;
        
        case EVENT_HDMI_DETECT                  :
			switch(m_TVStatus)
			{
				case AHC_TV_BROWSER_STATUS:
					SetHDMIState(AHC_HDMI_NONE_STATUS);
					StateSwitchMode(UI_HDMI_STATE);
				break;
	             
				case AHC_TV_MOVIE_PB_STATUS:
					AHC_GetVideoCurrentPlayTime(&m_VideoCurPlayTime);
					MovPBTimer_Stop();
					MovPlaybackParamReset();
					AHC_SetVideoPlayStartTime(m_VideoCurPlayTime);
					SetHDMIState(AHC_HDMI_MOVIE_PB_STATUS);
					StateSwitchMode(UI_HDMI_STATE);
				break;
	             
				case AHC_TV_PHOTO_PB_STATUS:
					PhotoPBTimer_Stop();
					JpgPlaybackParamReset();
					SetHDMIState(AHC_HDMI_PHOTO_PB_STATUS);
					StateSwitchMode(UI_HDMI_STATE);
				break;
	             
				case AHC_TV_SLIDE_SHOW_STATUS:
					AHC_GetVideoCurrentPlayTime(&m_VideoCurPlayTime);
                 	SlideShowTimer_Stop();
                 	SlideShowFunc_Stop();
                 	AHC_SetVideoPlayStartTime(m_VideoCurPlayTime);
                 	SetHDMIState(AHC_HDMI_SLIDE_SHOW_STATUS);
                 	StateSwitchMode(UI_HDMI_STATE);
				break;
			}    
        break;   
        
        case EVENT_HDMI_REMOVED                   :
        break; 

        #ifdef CFG_TV_IGNORE_POWER_OFF //may be defined in config_xxx.h
    	case EVENT_POWER_OFF:
    		
    		// Power Key not to power off,
    		LedCtrl_LcdBackLight(!LedCtrl_GetBacklightStatus());
    	break;
    	#endif

        default:
	 		switch(m_TVStatus)
			{
				case AHC_TV_VIDEO_PREVIEW_STATUS:
					StateTV_VideoMode_Handler(ulEvent);
				break;	
				case AHC_TV_DSC_PREVIEW_STATUS:
					StateTV_CameraMode_Handler(ulEvent);
				break;	
				case AHC_TV_BROWSER_STATUS:
					StateTV_BrowserMode_Handler(ulEvent);
				break;
				case AHC_TV_MOVIE_PB_STATUS:
					StateTV_MoviePBMode_Handler(ulEvent);
				break;
				case AHC_TV_PHOTO_PB_STATUS:
					StateTV_PhotoPBMode_Handler(ulEvent);
				break;
				case AHC_TV_SLIDE_SHOW_STATUS:
					StateTV_SlideShowMode_Handler(ulEvent);
				break;
				case AHC_TV_SLEEP_STATUS:
					StateTV_Sleep_Handler(ulEvent);
				break;
				case AHC_TV_NET_PLAYBACK_STATUS:
					StateTV_NetPlayback_Handler(ulEvent);
				break;
				default:
				break;
			}       
        break;

        case EVENT_LCD_COVER_NORMAL               :
        case EVENT_LCD_COVER_ROTATE               :
        	AHC_DrawRotateEvent(ulEvent);
		break;

        case EVENT_VR_EMERGENT:
            
            if( MenuSettingConfig()->uiGsensorSensitivity != GSENSOR_SENSITIVITY_OFF && VideoFunc_RecordStatus() )
            {
                #if (GSENSOR_CONNECT_ENABLE)
                extern AHC_BOOL m_ubGsnrIsObjMove;

                m_ubGsnrIsObjMove = AHC_FALSE;//Reset
                #endif
        
                RTNA_DBG_Str(0, FG_RED("CarDV is Shaking !!!!!\r\n"));
        
                //AHC_PlaySoundEffect(AHC_SOUNDEFFECT_VR_EMER);
                
                if( (AHC_IsEmerRecordStarted() == AHC_FALSE) &&
                    (AHC_IsEmerPostDone() == AHC_TRUE))
                {
                    #if (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_MOVE_FILE)
                    if(AHC_Deletion_RemoveEx(DCF_DB_TYPE_3RD_DB, AHC_GetVRTimeLimit()) == AHC_TRUE)
                    #else
                    if(AHC_Deletion_RemoveEx(DCF_DB_TYPE_3RD_DB, EMER_RECORD_DUAL_WRTIE_MAX_TIME) == AHC_TRUE)
                    #endif
                    {
                        printc("Emergency Record: Go !!!! \n");
                    
                        #if (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_MOVE_FILE)
                        if(1)
                        #else
                        if(VideoFunc_TriggerEmerRecord()) 
                        #endif
                        {
                            //UINT32 ulTickCount = OSTimeGet();
                            //DrawVideo_UpdatebyEvent(EVENT_VR_EMERGENT);
                            AHC_SetEmerRecordStarted(AHC_TRUE);
                            AHC_WMSG_Draw(AHC_TRUE, WMSG_LOCK_CURRENT_FILE, 2);
                            //printc("AHC_WMSG_Draw() Time = %d ms \n\r", ((OSTimeGet() - ulTickCount)*1000)/ OS_TICKS_PER_SEC);
                        }   
                    }
                }
                else 
                {
                    printc("Emergency Record: Keep going !!!! \n");
        
                    if(VideoFunc_TriggerEmerRecord()) 
                    {
                        //DrawVideo_UpdatebyEvent(EVENT_VR_EMERGENT);
                    }       
                }
            }
        break;  

        case EVENT_VRCB_EMER_DONE:
            printc("EVENT_VRCB_EMER_DONE \n");

            #if((EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_DUAL_FILE)||(EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE))
            if(AHC_GetKernalEmergencyStopStep() == AHC_KERNAL_EMERGENCY_STOP)
                AHC_SetKernalEmergencyStopStep(AHC_KERNAL_EMERGENCY_AHC_DONE);  
            else
                break;
            #endif

            SystemSettingConfig()->byStartNormalRecordAfterEmergencyRecord = (UINT8)VideoFunc_RecordStatus();
            AHC_EmerRecordPostProcess();
            //DrawVideo_UpdatebyEvent(EVENT_VRCB_EMER_DONE);  

            AHC_SetEmerRecordStarted(AHC_FALSE);

        break;
    }
#endif    
}

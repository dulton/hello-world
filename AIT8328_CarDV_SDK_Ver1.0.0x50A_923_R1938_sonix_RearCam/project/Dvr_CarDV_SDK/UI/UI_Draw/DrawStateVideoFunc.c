/*===========================================================================
 * Include file 
 *===========================================================================*/ 

#include "customer_config.h"
#include "AHC_GUI.h"
#include "AHC_OS.h"
#include "AHC_Browser.h"
#include "AHC_Capture.h"
#include "AHC_General.h"
#include "AHC_Menu.h"
#include "AHC_Parameter.h"
#include "AIHC_Display.h"
#include "AHC_Display.h"
#include "AHC_General_CarDV.h"
#include "MenuCommon.h"
#include "MenuDrawCommon.h"
#include "MenuTouchButton.h"
#include "MenuDrawingFunc.h"
#include "KeyParser.h"
#include "MenuSetting.h"
#include "DrawStateMenuSetting.h"
#include "DrawStateVideoFunc.h"
#include "DrawStateCameraFunc.h"
#include "ColorDefine.h"
#include "IconPosition.h"
#include "IconDefine.h"
#include "ZoomControl.h"
#include "StateVideoFunc.h"
#include "StateTVFunc.h"
#include "StateHDMIFunc.h"
#include "dsc_Charger.h"
#include "UI_DrawGeneral.h"
#include "GSensor_ctrl.h"
#include "disp_drv.h"
#include "ait_utility.h"
#include "ShowOSDFunc.h"

#if defined(WIFI_PORT) && (WIFI_PORT == 1)
#include "netapp.h"
#include "wlan.h"
#endif
#if SUPPORT_GSENSOR
#include "GSensor_ctrl.h"
#include "AHC_Gsensor.h"
#endif
#if GPS_CONNECT_ENABLE
#include "GPS_ctl.h"
#endif

#if (ENABLE_ADAS_LDWS)
#include "ldws.h"
extern void AHC_WMSG_LDWS_Line(UINT16 ubID, AHC_BOOL needClean);
#endif

#if (ENABLE_ADAS_FCWS)
#include "fcws.h"
extern void AHC_WMSG_FCWS_Info(UINT16 ubID, AHC_BOOL needClean);
#endif

/*===========================================================================
 * Macro define
 *===========================================================================*/

#define VR_OSD_HIDE_TIME	(120)
#if (AHC_ENABLE_VIDEO_STICKER)
#define VIDEO_STICKER_HEIGHT 			(64)  //(32)
#define VIDEO_STICKER_BORDER_OFFSET 	(16)
#endif

/*===========================================================================
 * Global varible
 *===========================================================================*/

UINT8		m_ubBatteryStatus 	= 0;
UINT8 		ubVRStatus			= VIDEO_REC_STOP;
#if (AUTO_HIDE_OSD_EN)
AHC_BOOL	m_ubHideOSD			= AHC_FALSE;
UINT32		m_ulOSDCounter		= 0;
#endif

#if (defined(WIFI_PORT) && (WIFI_PORT == 1) && defined(CFG_DRAW_WIFI_STREAMING_MONO_PATTERN))
static UINT8 monoPatternDrawed = AHC_FALSE;
#endif

/*===========================================================================
 * Extern varible
 *===========================================================================*/
extern AHC_BOOL	bUpdateFlashLedIcon;
extern AHC_BOOL	bMuteRecord;
extern AHC_BOOL bGPS_PageExist;
extern AHC_BOOL bPreviewModeTVout;
extern AHC_BOOL bPreviewModeHDMI;
extern AHC_BOOL bAudioRecording;
#if (CHARGE_FULL_NOTIFY)
extern AHC_BOOL bChargeFull;
#endif
#if (VR_PREENCODE_EN)
extern AHC_BOOL m_ubPreRecording;
extern AHC_BOOL m_ubPreEncodeEn;
#endif

#if (MOTION_DETECTION_EN)
extern AHC_BOOL m_ubMotionDtcEn;
extern UINT32	m_ulVMDRemindTime;
extern AHC_BOOL	m_ubInRemindTime;
extern AHC_BOOL	m_ubVMDStart;	
extern AHC_BOOL	m_ulVMDCancel;
#endif

#if (LIMIT_MAX_LOCK_FILE_NUM)
extern UINT32	m_ulLockFileNum;
extern UINT32   m_ulLockEventNum;
#endif

#if (DAY_NIGHT_MODE_SWITCH_EN)
extern AHC_BOOL bNightMode;
#endif
extern MOVIESIZE_SETTING			VideoRecSize;
extern AHC_OSD_INSTANCE             *m_OSD[];

#if (ENABLE_ADAS_LDWS)
extern UINT32                       m_ulLDWSWarn;
#endif
extern AHC_BOOL                     gbAhcDbgBrk;
/*===========================================================================
 * Extern function
 *===========================================================================*/

extern void 	DrawCaptureStateEV(void);
void			Dump_Gsensor_XYZ(UINT8 did);

/*===========================================================================
 * Main body
 *===========================================================================*/ 

void Dump_Gsensor_XYZ(UINT8 did)
{
#if (SUPPORT_GSENSOR)
	AHC_GSENSOR_DATA 	sdata;
	char			sz[32];

	AHC_Gsensor_IOControl(GSNR_CMD_READ_ACCEL_WITH_COMPENSATION, (MMP_UBYTE*) &sdata);

	sprintf(sz, "%d / %d / %d         \r\n", (signed char)sdata.acc_val[0], (signed char)sdata.acc_val[1], (signed char)sdata.acc_val[2]);	

	AHC_OSDSetColor(did, OSD_COLOR_YELLOW);
	AHC_OSDSetBkColor(did, OSD_COLOR_TRANSPARENT);
	AHC_OSDDrawText(did, (UINT8*)sz, 240, 50, strlen(sz));
#endif
}

void UpdateMotionRemindTime(int rTime)
{
#if (MOTION_DETECTION_EN)

    static int STime 	= -1;
    static const GUI_BITMAP* digicn[] = {	&bmIcon_Num_0,
		    								&bmIcon_Num_1,
		    								&bmIcon_Num_2,
		    								&bmIcon_Num_3,
		    								&bmIcon_Num_4,
		    								&bmIcon_Num_5,
		    								&bmIcon_Num_6,
		    								&bmIcon_Num_7,
		    								&bmIcon_Num_8,
		    								&bmIcon_Num_9};
    
    UINT8   bMainWID 	= MAIN_DISPLAY_BUFFER;
    UINT16  x, y;
	UINT16 	Width, Height;
	AHC_DISPLAY_OUTPUTPANEL  OutputDevice;
		    
    if(STime == rTime)
        return;
    else
       STime = rTime; 
       
	AHC_GetDisplayOutputDev(&OutputDevice);

	x = AHC_GET_ATTR_OSD_W(bMainWID)>>1;
	y = AHC_GET_ATTR_OSD_H(bMainWID)>>1;

	if(OutputDevice != MMP_DISPLAY_SEL_MAIN_LCD) 
	{   
		switch(OutputDevice) 
		{
			case MMP_DISPLAY_SEL_HDMI:
				bMainWID = HDMIFunc_GetUImodeOsdID();
				AHC_GetHdmiDisplayWidthHeight(&Width, &Height);
			break;    
			case MMP_DISPLAY_SEL_NTSC_TV:
				bMainWID = TVFunc_GetUImodeOsdID();
				AHC_GetNtscTvDisplayWidthHeight(&Width, &Height);
			break;    
			case MMP_DISPLAY_SEL_PAL_TV:
				bMainWID = TVFunc_GetUImodeOsdID();
				AHC_GetPalTvDisplayWidthHeight(&Width, &Height);
			break;   
		}
	        
		x = Width>>1;
		y = Height>>1;
	}
        
    if(STime > 0) 
    {
        if(AHC_WMSG_States()) 
           AHC_WMSG_Draw(AHC_FALSE, WMSG_NONE, 0);
	
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(bMainWID, OSD_COLOR_TRANSPARENT);
		#else
        AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
	    #endif
        AHC_OSDDrawFillRect(bMainWID, x-40, y-30, x+30, y+35);
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(bMainWID, 0xAA000000);
		#else
        AHC_OSDSetColor(0xAA000000);
	    #endif
	    AHC_OSDSetActive(bMainWID, 1);
	    if (STime > 9)
	    	STime = 9;
		AHC_OSDDrawBitmap(bMainWID, digicn[STime], x-25, y-30);
    }    
    else 
    {
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(bMainWID, OSD_COLOR_TRANSPARENT);
		#else
        AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
	    #endif
        AHC_OSDDrawFillRect(bMainWID, x-40, y-30, x+30, y+35); 
		if(OutputDevice != MMP_DISPLAY_SEL_MAIN_LCD) 
	    	AHC_OSDSetActive(bMainWID, 1);
	    
    }
#endif        
}

void UpdateVideoCurrentTimeStamp(void)
{
	UINT32	TimeStampOp;

	AHC_GetCaptureConfig(ACC_DATE_STAMP, &TimeStampOp);	

    if ((TimeStampOp & AHC_ACC_TIMESTAMP_ENABLE_MASK) && (VideoFunc_RecordStatus() || uiGetParkingModeEnable() == AHC_TRUE))
	{  
		AHC_RTC_TIME sRtcTime ;
		
		AHC_RTC_GetTime(&sRtcTime);
		AIHC_UpdateVRTimeStamp(&sRtcTime);
	}
}

void UpdateVideoRecordStatus(UINT8 ubStatus)
{
	ubVRStatus = ubStatus;
}

void DrawVideoRecInfo(AHC_BOOL bClear)
{
    UINT8    bOvlWID;
    UINT8    bMainWID = MAIN_DISPLAY_BUFFER;

#ifndef STR_STANDBY_STATUS_WIDTH
#define STR_STANDBY_STATUS_WIDTH		32
#endif

#ifndef STR_STANDBY_STATUS_HEIGHT
#define STR_STANDBY_STATUS_HEIGHT		22
#endif

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    OSDDraw_GetLastOvlDrawBuffer(&bOvlWID);    

    AHC_OSDSetActive(bOvlWID, 1);
    BEGIN_LAYER(bMainWID);
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bMainWID, OSD_COLOR_TRANSPARENT);
    AHC_OSDSetBkColor(bMainWID, OSD_COLOR_TRANSPARENT);
#else
    AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
    AHC_OSDSetBkColor(OSD_COLOR_TRANSPARENT);
#endif
    AHC_OSDDrawFillRect(bMainWID, STR_STANDBY_STATUS_X0, STR_STANDBY_STATUS_Y0, STR_STANDBY_STATUS_X0+STR_STANDBY_STATUS_WIDTH, STR_STANDBY_STATUS_Y0+STR_STANDBY_STATUS_HEIGHT); //Clear REC or PAUSE icon
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bMainWID, 0xAA000000);
#else
    AHC_OSDSetColor(0xAA000000);
#endif
    END_LAYER(bMainWID);
}

void DrawVideoRecStatus(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    const UINT8 empty_rec_state[] =	{"        "};
    const UINT8 stby_rec_state[] =	{"   STBY "};
    const UINT8 rec_rec_state[] =	{"    REC "};
    const UINT8 cap_rec_state[] =	{"   CAP  "};

#define CFG_DRAW_REC_RECORD_STOP			0xEE30DC30

#define CFG_DRAW_REC_RECORD_COLOR			(OSD_COLOR_RED2)

#ifndef CFG_DRAW_REC_CAPTURE_COLOR //may be defined in config_xxx.h
#define CFG_DRAW_REC_CAPTURE_COLOR		    CFG_DRAW_REC_RECORD_COLOR
#endif

    GUI_COLOR	IconColor = OSD_COLOR_TRANSPARENT;	

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);

#if (defined(WIFI_PORT) && (WIFI_PORT == 1) && defined(CFG_DRAW_WIFI_STREAMING_MONO_PATTERN))
    if (AHC_Get_WiFi_Streaming_Status()) {
        bkColor = CFG_DRAW_WIFI_STREAMING_MONO_PATTERN;
        IconColor = CFG_DRAW_WIFI_STREAMING_MONO_PATTERN;
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, IconColor);
#else
        AHC_OSDSetColor(IconColor);
#endif
    }
#endif

    if(bLarge==LARGE_SIZE) {
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetFont(ubID, &GUI_Font32B_1);
#else
        AHC_OSDSetFont(&GUI_Font32B_1);
#endif
    }
    else {
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetFont(ubID, &GUI_Font16B_1);
#else
        AHC_OSDSetFont(&GUI_Font16B_1);
#endif
    }

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(ubID, IconColor);
    AHC_OSDSetBkColor(ubID, bkColor);
#else
    AHC_OSDSetColor(IconColor);
    AHC_OSDSetBkColor(bkColor);
#endif

    AHC_OSDDrawFillRect(ubID, x-2, y-8, x+48, y+32);  

    switch(ubVRStatus) 
    {
        case VIDEO_REC_STOP:
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
            AHC_OSDSetColor(ubID, CFG_DRAW_REC_RECORD_STOP);
#else
            AHC_OSDSetColor(CFG_DRAW_REC_RECORD_STOP);
#endif
            AHC_OSDDrawText(ubID, (UINT8*) stby_rec_state, x, y, sizeof(stby_rec_state));
            break;

        case VIDEO_REC_START:
#ifdef CFG_DRAW_REC_START_USE_BMP //may be defined in config_xxx.h
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
            AHC_OSDSetColor(ubID, CFG_DRAW_REC_RECORD_COLOR);
#else
            AHC_OSDSetColor(CFG_DRAW_REC_RECORD_COLOR);
#endif
            AHC_OSDDrawBitmap(ubID, &bmIcon_REC, x, 0);
#elif defined (CFG_DRAW_REC_START_BY_PARAM) //may be defined in config_xxx.h
            if(Para1 == 0)
            {
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
                AHC_OSDSetColor(ubID, CFG_DRAW_REC_RECORD_COLOR);
#else
                AHC_OSDSetColor(CFG_DRAW_REC_RECORD_COLOR);
#endif
                AHC_OSDDrawText(ubID, (UINT8*)rec_rec_state, x, y, strlen(rec_rec_state));
            }
#else
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
            AHC_OSDSetColor(ubID, CFG_DRAW_REC_RECORD_COLOR);
#else
            AHC_OSDSetColor(CFG_DRAW_REC_RECORD_COLOR);
#endif
            AHC_OSDDrawText(ubID, (UINT8*) rec_rec_state, x, y, sizeof(rec_rec_state));
#endif
            break;

        case VIDEO_REC_PAUSE:
            if(bLarge==LARGE_SIZE)
            {
                AHC_OSDDrawBitmapMag(ubID, &bmIcon_VRPause, x+5, y+5, 2, 2);
            }
            else
            {	
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
                AHC_OSDSetColor(ubID, CFG_DRAW_REC_RECORD_COLOR);
#else
                AHC_OSDSetColor(CFG_DRAW_REC_RECORD_COLOR);
#endif
                AHC_OSDDrawText(ubID, (UINT8*) empty_rec_state, x-2, y, sizeof(empty_rec_state));
                AHC_OSDDrawBitmap(ubID, &bmIcon_VRPause, x, y);
            }
            break;

        case VIDEO_REC_CAPTURE:
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
            AHC_OSDSetColor(ubID, CFG_DRAW_REC_CAPTURE_COLOR);
#else
            AHC_OSDSetColor(CFG_DRAW_REC_CAPTURE_COLOR);
#endif
            AHC_OSDDrawText(ubID, (UINT8*) cap_rec_state, x, y, sizeof(cap_rec_state));
            break;
    }

    END_LAYER(ubID);
}

void DrawVideoAvailableRecordTime(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    UBYTE	Hour, Min, Sec;
    UINT16	leftpos		= x;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);

#if (defined(WIFI_PORT) && (WIFI_PORT == 1) && defined(CFG_DRAW_WIFI_STREAMING_MONO_PATTERN))
    if (AHC_Get_WiFi_Streaming_Status()) {
        bkColor = CFG_DRAW_WIFI_STREAMING_MONO_PATTERN;
    }
#endif

    if(AHC_CheckCurSysMode(AHC_MODE_RECORD_PREVIEW)) 
    {
        // 0 to get time for current setting of bitrate
        AHC_AvailableRecordTime_Ex(0, &Hour, &Min, &Sec); 

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, OSD_COLOR_WHITE);
        AHC_OSDSetBkColor(ubID, bkColor);
#else
        AHC_OSDSetColor(OSD_COLOR_WHITE);
        AHC_OSDSetBkColor(bkColor);
#endif

        if(bLarge==LARGE_SIZE)
        {
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
            AHC_OSDSetFont(ubID, &GUI_Font32B_1);
#else
            AHC_OSDSetFont(&GUI_Font32B_1);
#endif
        }
        else
        {
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
            AHC_OSDSetFont(ubID, &GUI_Font20B_1);
#else
            AHC_OSDSetFont(&GUI_Font20B_1);
#endif
        }

        {
            char	sztime[16];
            int		l;

            /* space in format will clean bitmap when the pixles of width becomes shorter then before*/
            // too many space padding at end, might overwrite lock icon
            l = sprintf(sztime, "%d:%02d:%02d  ", Hour, Min, Sec);
            AHC_OSDDrawText(ubID, (UINT8*)sztime, leftpos ,y, l);
        }
    }
    END_LAYER(ubID);
}

void DrawVideoCurrentRecordTime(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    UINT32  ulTime;
    UINT16  leftpos	= x;
    UINT32  ulSysStatus;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

#if (defined(WIFI_PORT) && (WIFI_PORT == 1) && defined(CFG_DRAW_WIFI_STREAMING_MONO_PATTERN))
    if (AHC_Get_WiFi_Streaming_Status()) {
        bkColor = CFG_DRAW_WIFI_STREAMING_MONO_PATTERN;
    }
#endif

    AHC_GetSystemStatus(&ulSysStatus);
    ulSysStatus >>= 16;

    if( (ulSysStatus == AHC_MODE_VIDEO_RECORD) ||
    (ulSysStatus == AHC_MODE_RECORD_VONLY)	)
    {
        AHC_GetCurrentRecordingTime(&ulTime); 
        ulTime += VideoFunc_GetRecordTimeOffset();

        BEGIN_LAYER(ubID);

#ifdef CFG_DRAW_REC_CUR_TIME_COLOR //may be defined in config_xxx.h
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, CFG_DRAW_REC_CUR_TIME_COLOR);
#else
        AHC_OSDSetColor(CFG_DRAW_REC_CUR_TIME_COLOR);
#endif
#else
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, OSD_COLOR_RED);
#else
        AHC_OSDSetColor(OSD_COLOR_RED);
#endif
#endif        

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetBkColor(ubID, bkColor);
#else
        AHC_OSDSetBkColor(bkColor);
#endif

        if(bLarge==LARGE_SIZE)
        {
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
            AHC_OSDSetFont(ubID, &GUI_Font32B_1);
#else
            AHC_OSDSetFont(&GUI_Font32B_1);
#endif
        }
        else
        {
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
            AHC_OSDSetFont(ubID, &GUI_Font20B_1);
#else
            AHC_OSDSetFont(&GUI_Font20B_1);
#endif
        }

        {
            char	sztime[16];
            int		l;
            /* space in format will clean bitmap when the pixles of width becomes shorter then before*/
            l = sprintf(sztime, "%d:%02d:%02d  ", TIME_HOUR(ulTime), TIME_MIN(ulTime), TIME_SEC(ulTime));
            AHC_OSDDrawText(ubID, (UINT8*)sztime, leftpos ,y, l);
        }

        END_LAYER(ubID);
    }
}

void DrawAudioCurRecordTime(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
#if (AUDIO_REC_PLAY_EN)
    UINT16  leftpos 	= x;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    if(bAudioRecording)
    {
        BEGIN_LAYER(ubID);

#if (defined(WIFI_PORT) && (WIFI_PORT == 1) && defined(CFG_DRAW_WIFI_STREAMING_MONO_PATTERN))
        if (AHC_Get_WiFi_Streaming_Status()) {
            bkColor = CFG_DRAW_WIFI_STREAMING_MONO_PATTERN;
        }
#endif

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, OSD_COLOR_WHITE);       
        AHC_OSDSetBkColor(ubID, bkColor);
#else
        AHC_OSDSetColor(OSD_COLOR_WHITE);       
        AHC_OSDSetBkColor(bkColor);
#endif

        if(bLarge==LARGE_SIZE)
        {
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
            AHC_OSDSetFont(ubID, &GUI_Font32B_1);        
#else
            AHC_OSDSetFont(&GUI_Font32B_1);        
#endif
        }
        else
        {
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
            AHC_OSDSetFont(ubID, &GUI_Font20B_1);
#else
            AHC_OSDSetFont(&GUI_Font20B_1);
#endif
        }

        {
            UINT32  ulTime;
            char	sztime[16];
            int		l;

            AHC_GetAudioCurRecTime(&ulTime);

            /* space in format will clean bitmap when the pixles of width becomes shorter then before*/
            l = sprintf(sztime, "%d:%02d:%02d  ", TIME_HOUR(ulTime), TIME_MIN(ulTime), TIME_SEC(ulTime));
            AHC_OSDDrawText(ubID, (UINT8*)sztime, leftpos ,y, l);
        }

        END_LAYER(ubID);
    }
#endif    
}

void DrawVideoFileName(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
#if (AIT_INT_USE==1)
    UINT32  ulSysStatus;
    UINT8	VideoRFileName[16];

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }
    
    AHC_GetSystemStatus(&ulSysStatus);
    ulSysStatus >>= 16;
   
    if( (ulSysStatus == AHC_MODE_VIDEO_RECORD) ||
    	(ulSysStatus == AHC_MODE_RECORD_VONLY)	)
    {
		memcpy(VideoRFileName, (UINT8*)AHC_GetCurrentVRFileName(AHC_FALSE), 16);
		BEGIN_LAYER(ubID);
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, OSD_COLOR_WHITE);
        AHC_OSDSetBkColor(ubID, bkColor);
		#else
        AHC_OSDSetColor(OSD_COLOR_WHITE);
        AHC_OSDSetBkColor(bkColor);
	    #endif
        
        if(bLarge==LARGE_SIZE)
        {
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        	AHC_OSDSetFont(ubID, &GUI_Font32B_1);        
			#else
        	AHC_OSDSetFont(&GUI_Font32B_1);        
		    #endif
        }
        else
        {
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        	AHC_OSDSetFont(ubID, &GUI_Font16B_1);
			#else
        	AHC_OSDSetFont(&GUI_Font16B_1);
		    #endif
		}

        AHC_OSDDrawText(ubID,(UINT8*)VideoRFileName, x ,y, strlen((INT8*)VideoRFileName));
        END_LAYER(ubID);
    }
#endif    
}

void DrawVideoMuteRecord(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
#ifndef CFG_DRAW_REC_MUTE_AT_TOP //may be defined in config_xxx.h
    UINT16  	leftpos = x;
    UINT16  	toppos 	= y;
#else
    UINT16  	leftpos = 10;
    UINT16  	toppos 	= 0;
#endif    
    GUI_BITMAP 	IconID 	= bmIcon_mutemicW;
#ifdef CFG_DRAW_REC_MIC //may be defined in config_xxx.h
    GUI_BITMAP 	NorMuteIconID 	= bmIcon_micW;
#endif
    GUI_COLOR	IconColor = 0xAA000000;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);

#if (defined(WIFI_PORT) && (WIFI_PORT == 1) && defined(CFG_DRAW_WIFI_STREAMING_MONO_PATTERN))
    if (AHC_Get_WiFi_Streaming_Status()) {
        bkColor = CFG_DRAW_WIFI_STREAMING_MONO_PATTERN;
        IconColor = CFG_DRAW_WIFI_STREAMING_MONO_PATTERN;
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, IconColor);
#else
        AHC_OSDSetColor(IconColor);
#endif
    }
#endif

    if(bFlag == AHC_FALSE)
    {
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, bkColor);
#else
        AHC_OSDSetColor(bkColor);
#endif

        if(bLarge==LARGE_SIZE)
            AHC_OSDDrawFillRect(ubID, leftpos, toppos, leftpos+IconID.XSize*ubMag, toppos+IconID.YSize*ubMag);
        else
            AHC_OSDDrawFillRect(ubID, leftpos, toppos, leftpos+IconID.XSize, toppos+IconID.YSize);

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, IconColor);
#else
        AHC_OSDSetColor(IconColor);
#endif
    }
    else
    {	
        if(bMuteRecord)
        {	
            if(bLarge==LARGE_SIZE)
                AHC_OSDDrawBitmapMag(ubID,  &IconID, leftpos, toppos, ubMag, ubMag);
            else
                AHC_OSDDrawBitmap(ubID,  &IconID, leftpos, toppos);
        }
#ifdef CFG_DRAW_REC_MIC
        else
        {
            if(bLarge==LARGE_SIZE)
                AHC_OSDDrawBitmapMag(ubID,  &NorMuteIconID, leftpos, toppos, ubMag, ubMag);
            else
                AHC_OSDDrawBitmap(ubID,  &NorMuteIconID, leftpos, toppos);
        }
#endif
    }
    END_LAYER(ubID);
}

void DrawVideoMotionDtc(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
#if (MOTION_DETECTION_EN)

    GUI_BITMAP 	IconID 	= BMICON_MOTION_DETECTION;
    GUI_COLOR	IconColor = 0xAA000000;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);

#if (defined(WIFI_PORT) && (WIFI_PORT == 1) && defined(CFG_DRAW_WIFI_STREAMING_MONO_PATTERN))
    if (AHC_Get_WiFi_Streaming_Status()) {
        bkColor = CFG_DRAW_WIFI_STREAMING_MONO_PATTERN;
        IconColor = CFG_DRAW_WIFI_STREAMING_MONO_PATTERN;
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, IconColor);
#else
        AHC_OSDSetColor(IconColor);
#endif
    }
#endif

    if(bFlag == AHC_FALSE)
    {
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, bkColor);
#else
        AHC_OSDSetColor(bkColor);
#endif

        if(bLarge==LARGE_SIZE)
            AHC_OSDDrawFillRect(ubID, x, y, x+IconID.XSize*ubMag, y+IconID.YSize*ubMag);
        else
            AHC_OSDDrawFillRect(ubID, x, y, x+IconID.XSize, y+IconID.YSize);

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, IconColor);
#else
        AHC_OSDSetColor(IconColor);
#endif
    }
    else
    {
        if(m_ubMotionDtcEn)
        {
            if(bLarge==LARGE_SIZE)
                AHC_OSDDrawBitmapMag(ubID, &IconID, x, y, ubMag, ubMag);
            else
                AHC_OSDDrawBitmap(ubID, &IconID, x, y);
        }
    }

    END_LAYER(ubID);
#endif	
}

#if (ENABLE_ADAS_LDWS)
void DrawVideoLDWS_Line(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    AHC_WMSG_LDWS_Line(ubID, AHC_TRUE);
}
#endif

#if (ENABLE_ADAS_FCWS)
void DrawVideoFCWS_Info(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    AHC_WMSG_FCWS_Info(ubID, AHC_TRUE);
}
#endif

void DrawVideoTimeLapse(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
#if (defined(VIDEO_REC_TIMELAPSE_EN) && VIDEO_REC_TIMELAPSE_EN)
    GUI_BITMAP 	IconID 	= bmTV_TimeLapseB;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);

    if(bFlag == AHC_FALSE) {
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, bkColor);
#else
        AHC_OSDSetColor(bkColor);
#endif

        if(bLarge==LARGE_SIZE)
            AHC_OSDDrawFillRect(ubID, x, y, x+IconID.XSize*ubMag, y+IconID.YSize*ubMag);
        else
            AHC_OSDDrawFillRect(ubID, x, y, x+IconID.XSize, y+IconID.YSize);

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, 0xAA000000);
#else
        AHC_OSDSetColor(0xAA000000);
#endif
    }
    else {
        UINT32 vrTimelapseEn = 1;		// Off

        if (AHC_Menu_SettingGetCB(COMMON_KEY_VR_TIMELAPSE, &vrTimelapseEn) == AHC_TRUE)
        {
            if (vrTimelapseEn != PRJ_VR_TIMELAPSE_OFF) {
                if (bLarge == LARGE_SIZE)
                    AHC_OSDDrawBitmapMag(ubID, &IconID, x, y, ubMag, ubMag);
                else
                    AHC_OSDDrawBitmap(ubID, &IconID, x, y);
            }
        }
    }

    END_LAYER(ubID);
#endif
}

void DrawVideoLockFileStatus(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
#ifdef CFG_DRAW_REC_CUS_LOCK_FILE //may be defined in config_xxx.h
    GUI_BITMAP 	IconID 	= CFG_DRAW_REC_CUS_LOCK_FILE;
#else
    GUI_BITMAP 	IconID 	= bmIcon_ProtectKey;
#endif
    GUI_COLOR	IconColor = 0xAA000000;	

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);

#if (defined(WIFI_PORT) && (WIFI_PORT == 1) && defined(CFG_DRAW_WIFI_STREAMING_MONO_PATTERN))
    if (AHC_Get_WiFi_Streaming_Status()) {
        bkColor = CFG_DRAW_WIFI_STREAMING_MONO_PATTERN;
        IconColor = CFG_DRAW_WIFI_STREAMING_MONO_PATTERN;
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, IconColor);
#else
        AHC_OSDSetColor(IconColor);
#endif
    }
#endif

    if(bFlag == AHC_FALSE)
    {
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, bkColor);
#else
        AHC_OSDSetColor(bkColor);
#endif

        if(bLarge==LARGE_SIZE)
            AHC_OSDDrawFillRect(ubID, x-5, y-5, x+IconID.XSize*ubMag, y+IconID.YSize*ubMag);
        else
            AHC_OSDDrawFillRect(ubID, x,   y,   x+IconID.XSize,       y+IconID.YSize);

#if defined (CFG_DRAW_REC_START_BY_PARAM) //may be defined in config_xxx.h
        if(Para1)
#endif
        {
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
            AHC_OSDSetColor(ubID, IconColor);
#else
            AHC_OSDSetColor(IconColor);
#endif
        }
    }
    else
    {	
        if(VideoFunc_LockFileEnabled())
        {
            if(bLarge==LARGE_SIZE)
                AHC_OSDDrawBitmapMag(ubID, &IconID, x, y, ubMag, ubMag);
            else
                AHC_OSDDrawBitmap(ubID, &IconID, x, y);
        }
    }

    END_LAYER(ubID);
}

void DrawVideoLockFileNum(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
 
}

void DrawVideoRecording(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    UINT32 	ulSysStatus;
    INT32 	r = Para1;
    static UINT8 prog = 0;
    GUI_COLOR	IconColor = OSD_COLOR_TRANSPARENT;	
    static  MMP_ULONG preTick = 0;
    MMP_ULONG curTick;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    MMPF_OS_GetTime(&curTick);

    if (curTick >= preTick) {
        if ((curTick - preTick) < MS_TO_TICK(500)) {
            return;
        }
    }
    else {
        if ((curTick + ((MMP_ULONG) -1) - preTick) < MS_TO_TICK(500)) {
            return;
        }
    }

    preTick = curTick;

    AHC_GetSystemStatus(&ulSysStatus);

    if( ulSysStatus == ((AHC_MODE_VIDEO_RECORD<<16)|AHC_SYS_VIDRECD_PAUSE) ||
        ulSysStatus == ((AHC_MODE_RECORD_VONLY<<16)|AHC_SYS_VIDRECD_PAUSE) )
    {
        return;
    }

    // Draw static icon at HCarDV_DrawStateVideoFunc.c
#ifdef CFG_DRAW_REC_STATIC_ICON //may be defined in config_xxx.h
    return;
#endif

    BEGIN_LAYER(ubID);

#if (defined(WIFI_PORT) && (WIFI_PORT == 1) && defined(CFG_DRAW_WIFI_STREAMING_MONO_PATTERN))
    if (AHC_Get_WiFi_Streaming_Status()) {
        bkColor = CFG_DRAW_WIFI_STREAMING_MONO_PATTERN;
        IconColor = CFG_DRAW_WIFI_STREAMING_MONO_PATTERN;
    }
#endif

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(ubID, IconColor);
    AHC_OSDSetBkColor(ubID, bkColor);
#else
    AHC_OSDSetColor(IconColor);
    AHC_OSDSetBkColor(bkColor);
#endif
    AHC_OSDDrawFillCircle(ubID, x, y, r >> ((prog++) & 0x01)); 

    ulSysStatus >>= 16;

    if(ulSysStatus == AHC_MODE_VIDEO_RECORD ||
        ulSysStatus == AHC_MODE_RECORD_VONLY) 
    {
#ifdef CFG_DRAW_REC_RECORD_COLOR2 //may be defined in config_xxx.h
        if(prog%2 == 0)
        {
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
            AHC_OSDSetColor(ubID, CFG_DRAW_REC_RECORD_COLOR);
#else
            AHC_OSDSetColor(CFG_DRAW_REC_RECORD_COLOR);
#endif
        }
        else
        {
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
            AHC_OSDSetColor(ubID, CFG_DRAW_REC_RECORD_COLOR2);//when the REC point is small
#else
            AHC_OSDSetColor(CFG_DRAW_REC_RECORD_COLOR2);//when the REC point is small
#endif
        }
#else
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, CFG_DRAW_REC_RECORD_COLOR);
#else
        AHC_OSDSetColor(CFG_DRAW_REC_RECORD_COLOR);
#endif
#endif
        AHC_OSDDrawFillCircle(ubID, x, y, r >> (prog & 0x1));
    }
    END_LAYER(ubID);
}

void DrawAudioRecording(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
#if (AUDIO_REC_PLAY_EN)

    INT32 r = Para1;
    static UINT8 prog = 0;
    GUI_COLOR	IconColor = OSD_COLOR_TRANSPARENT;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    if(!bAudioRecording) 
        return;
    
    BEGIN_LAYER(ubID);

#if (defined(WIFI_PORT) && (WIFI_PORT == 1) && defined(CFG_DRAW_WIFI_STREAMING_MONO_PATTERN))
    if (AHC_Get_WiFi_Streaming_Status()) {
        bkColor = CFG_DRAW_WIFI_STREAMING_MONO_PATTERN;
        IconColor = CFG_DRAW_WIFI_STREAMING_MONO_PATTERN;
    }
#endif

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(ubID, IconColor);
    AHC_OSDSetBkColor(ubID, bkColor);
#else
    AHC_OSDSetColor(IconColor);
    AHC_OSDSetBkColor(bkColor);
#endif
    AHC_OSDDrawFillCircle(ubID, x, y, r >> ((prog++) & 0x01)); 

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(ubID, CFG_DRAW_REC_RECORD_COLOR);
#else
    AHC_OSDSetColor(CFG_DRAW_REC_RECORD_COLOR);
#endif
    AHC_OSDDrawFillCircle(ubID, x, y, r >> (prog & 0x1));
    END_LAYER(ubID);
#endif
}

void DrawVideoRecFlashMode(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(ubID, 0xAA000000);
#else
    AHC_OSDSetColor(0xAA000000);
#endif
    END_LAYER(ubID);
    OSDDrawSetting_FlashMode(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
}

void DrawVideoZoomBar(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
#if (VIDEO_DIGIT_ZOOM_EN)
    AHC_BOOL bShow = (AHC_BOOL) Para1;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    if(bShow) {
        GUI_COLOR	IconColor = 0xAA000000;	

        BEGIN_LAYER(ubID);

#if (defined(WIFI_PORT) && (WIFI_PORT == 1) && defined(CFG_DRAW_WIFI_STREAMING_MONO_PATTERN))
        if (AHC_Get_WiFi_Streaming_Status()) {
            bkColor = CFG_DRAW_WIFI_STREAMING_MONO_PATTERN;
            IconColor = CFG_DRAW_WIFI_STREAMING_MONO_PATTERN;
        }
#endif

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, IconColor); 
        AHC_OSDSetBkColor(ubID, bkColor); 
#else
        AHC_OSDSetColor(IconColor); 
        AHC_OSDSetBkColor(bkColor); 
#endif
        END_LAYER(ubID);
    }

    OSDDraw_DigitZoomBar(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1, UI_VIDEO_STATE);
#endif
}

void DrawStateVideoRecInit(void)
{
    UINT8  	bID0 = 0;
    UINT8  	bID1 = 0;
    UINT8	idx	 = 0;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    CHARGE_ICON_ENABLE(AHC_FALSE);

    OSDDraw_EnterDrawing(&bID0, &bID1);

    OSDDraw_ClearOvlDrawBuffer(bID0);

    BEGIN_LAYER(bID0);
    BEGIN_LAYER(bID1);
    OSDDraw_EnableSemiTransparent(bID0, AHC_TRUE);
    OSDDraw_EnableSemiTransparent(bID1, AHC_TRUE);

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, 0xAA000000);
#else
    AHC_OSDSetColor(0xAA000000);
#endif
    END_LAYER(bID0);
    END_LAYER(bID1);

    for (idx = 0; idx < VIDEO_GUI_MAX; idx++)
    {
        if (idx == VIDEO_GUI_REC_STATE)
            UpdateVideoRecordStatus(VIDEO_REC_STOP);

        if ((idx == VIDEO_GUI_REC_POINT) || (idx == VIDEO_GUI_GPS_STATE))
            continue;

        UIDrawVideoFuncEx(bID0, idx, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    }

    OSDDraw_MainTouchButton(bID0, UI_VIDEO_STATE);

    OSDDraw_ExitDrawing(&bID0, &bID1);

    CHARGE_ICON_ENABLE(AHC_TRUE);
}

void DrawVideoRecStatePageSwitch(UINT8 ubPage)
{
#if (SUPPORT_TOUCH_PANEL)//disable temporally, wait touch UI
    UINT8  bID0 = 0;
    UINT8  bID1 = 0;

    OSDDraw_EnterDrawing(&bID0, &bID1);

    OSDDraw_ClearTouchButtons(bID0, VideoCtrlPage_TouchButton, ITEMNUM(VideoCtrlPage_TouchButton));

    if( TOUCH_MAIN_PAGE == ubPage )
        OSDDraw_MainTouchButton(bID0, UI_VIDEO_STATE);
    else
        OSDDraw_CtrlTouchButton(bID0, UI_VIDEO_STATE);

    OSDDraw_ExitDrawing(&bID0, &bID1);
#endif
}

void DrawVideoRecStateRecord(VideoRecordStatus Status)
{
    UINT8  bID0 = 0;
    UINT8  bID1 = 0;
    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    if(uiGetCurrentState() == UI_HDMI_STATE)
    {
        printc("--E-- %s wrong enter for HDMI \r\n", __func__);
        return;
    }

    OSDDraw_EnterDrawing(&bID0, &bID1);

    UpdateVideoRecordStatus(Status);

    UIDrawVideoFuncEx(bID0, VIDEO_GUI_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
#ifdef	_OEM_TOUCH_
    OSDDraw_MainTouchButton(bID0, (Status == VIDEO_REC_START)?
    UI_VIDEO_RECORD_STATE : UI_VIDEO_STATE);
#endif
#if (AIT_INT_USE==1)
    if(Status==VIDEO_REC_START)
    {
        DrawVideoFileName(bID0, 160, 30, NORMAL_SIZE, MAG_1X, AHC_TRUE, OSD_COLOR_TRANSPARENT, 0);
    }
    else if(Status==VIDEO_REC_STOP)
    {
        BEGIN_LAYER(bID0);

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(bID0, OSD_COLOR_TRANSPARENT);
#else
        AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
#endif
        AHC_OSDDrawFillRect(bID0, 160, 30, 260, 80);
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(bID0, 0xAA000000);	
#else
        AHC_OSDSetColor(0xAA000000);	
#endif
        END_LAYER(bID0);
    }
#endif

    OSDDraw_ExitDrawing(&bID0, &bID1);
}

void DrawVideoRecStateZoom(void)
{
#if (VIDEO_DIGIT_ZOOM_EN)
    UINT8  bID0 = 0;
    UINT8  bID1 = 0;

#if (HDMI_PREVIEW_EN || TVOUT_PREVIEW_EN)
    if(TVFunc_CheckTVStatus(AHC_TV_VIDEO_PREVIEW_STATUS))
    {
        bID0 = TVFunc_GetUImodeOsdID();

        UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_ZOOM_BAR, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, AHC_TRUE);	
    }
    else if(HDMIFunc_CheckHdmiStatus(AHC_HDMI_VIDEO_PREVIEW_STATUS))
    {
        bID0 = HDMIFunc_GetUImodeOsdID();

        UIDrawHdmi_VideoFuncEx(bID0, TV_GUI_VIDEO_ZOOM_BAR, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, AHC_TRUE);	
    }
    else
#endif
    {
        OSDDraw_EnterDrawing(&bID0, &bID1);	    
        UIDrawVideoFuncEx(bID0, VIDEO_GUI_ZOOM_BAR, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, AHC_TRUE);
        OSDDraw_ExitDrawing(&bID0, &bID1);
    }
#endif
}

void DrawVideoRecOsdDuringHide(UINT16 ubID)
{
#if (AUTO_HIDE_OSD_EN)
#endif
}

void DrawVideo_TimeStamp(UINT16 uiOSDid, UINT32 TimeStampOp, AHC_RTC_TIME* sRtcTime, UINT32* uiSrcAddr, UINT16* uwSrcW, UINT16* uwSrcH, UINT16* uwTempColorFormat,
						 UINT32* StickerX, UINT32* StickerY)
{
#if (AHC_ENABLE_VIDEO_STICKER)
    UINT32					CaptureWidth;
    UINT32					CaptureHeight;
    UINT16                  DateConfig;
    UINT16                  PositionConfig;
    UINT16                  FormatConfig;
    const GUI_FONT*         OldFont;
	#if (MENU_MOVIE_SIZE_VGA30P_EN)
	static AHC_BOOL bUIOSDDimension[2]={0};
	static UINT16  PreMovSize = MOVIE_SIZE_1080P;
	#endif

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(uiOSDid);

    if (!AHC_IsTVConnectEx()) {
        //TV_OUT: OSD_ID use 17,18 and 19
        BEGIN_LAYER(OVL_BUFFER_INDEX);
    }
    
    BEGIN_LAYER(OVL_BUFFER_INDEX1);
    BEGIN_LAYER(OVL_BUFFER_INDEX2);
    BEGIN_LAYER(OVL_BUFFER_INDEX3);
	#if (MENU_MOVIE_SIZE_VGA30P_EN)
	if(PreMovSize!= MenuSettingConfig()->uiMOVSize)
	{
		PreMovSize = MenuSettingConfig()->uiMOVSize;
		bUIOSDDimension[0] = 0;
		bUIOSDDimension[1] = 0;
	}
	if(MenuSettingConfig()->uiMOVSize == MOVIE_SIZE_VGA_30P)
	{
	    if((bUIOSDDimension[0] == 0) && (uiOSDid == DATESTAMP_PRIMARY_OSD_ID)){
	        AHC_OSDChangeDimetion(uiOSDid, PRIMARY_DATESTAMP_WIDTH-400, PRIMARY_DATESTAMP_HEIGHT);
	        bUIOSDDimension[0] = 1;
	    }
	    else if((bUIOSDDimension[1] == 0) && (uiOSDid == DATESTAMP_THUMB_OSD_ID)){
	        AHC_OSDChangeDimetion(uiOSDid, THUMB_DATESTAMP_WIDTH-400, THUMB_DATESTAMP_HEIGHT);
	        bUIOSDDimension[1] = 1;        
	    }
	}
	else
	{
	    if((bUIOSDDimension[0] == 0) && (uiOSDid == DATESTAMP_PRIMARY_OSD_ID)){
	        AHC_OSDChangeDimetion(uiOSDid, PRIMARY_DATESTAMP_WIDTH, PRIMARY_DATESTAMP_HEIGHT);
	        bUIOSDDimension[0] = 1;
	    }
	    else if((bUIOSDDimension[1] == 0) && (uiOSDid == DATESTAMP_THUMB_OSD_ID)){
	        AHC_OSDChangeDimetion(uiOSDid, THUMB_DATESTAMP_WIDTH, THUMB_DATESTAMP_HEIGHT);
	        bUIOSDDimension[1] = 1;        
	    }
	}		
	#endif

    AHC_OSDGetBufferAttr(uiOSDid, uwSrcW, uwSrcH, uwTempColorFormat, uiSrcAddr);
	
	AHC_GetImageSize(VIDEO_CAPTURE_MODE, &CaptureWidth, &CaptureHeight);
	
    DateConfig     = TimeStampOp & (AHC_ACC_TIMESTAMP_DATE_MASK   |
    								AHC_ACC_TIMESTAMP_LOGOEX_MASK |
    								AHC_ACC_TIMESTAMP_DATEEX_MASK);
    PositionConfig = TimeStampOp & AHC_ACC_TIMESTAMP_POSITION_MASK;
    FormatConfig   = TimeStampOp & AHC_ACC_TIMESTAMP_FORMAT_MASK;

#ifdef _OEM_VR_STICKER_
	{
		RECT	rc;
		POINT	p;

		rc.uiLeft  = 0;
		rc.uiTop   = 0;
		rc.uiWidth = uwSrcW;
		rc.uiHeight= uwSrcH;
		
		p = Oem_VideoStamp(uiOSDid, &rc, sRtcTime, DateConfig, FormatConfig, PositionConfig);
		
		StickerX = p.PointX;
		StickerY = p.PointY;
	}
#else	// _OEM_VR_STICKER_
	{

		INT8  byDate[64];
		UINT32 uiPosX = 21*16;//0;
		UINT32 uiPosY = 0;//18;
		#if (UUID_XML_FEATURE == 1)
		char  szProductName[PRODUCT_NAME_MAX_SIZE]={0};
		#endif
		MEMSET(byDate, 0, sizeof(byDate));
		
    	AHC_TransferDateToString(sRtcTime, byDate, DateConfig, FormatConfig);

		if  ( PositionConfig == AHC_ACC_TIMESTAMP_BOTTOM_RIGHT ){
		    *StickerX    = CaptureWidth  - *uwSrcW - VIDEO_STICKER_BORDER_OFFSET;
	        *StickerY    = CaptureHeight - *uwSrcH - VIDEO_STICKER_BORDER_OFFSET - AHC_VIDEO_STICKER_Y_DELTA;
	    }
		else if  ( PositionConfig == AHC_ACC_TIMESTAMP_BOTTOM_LEFT ){
		    *StickerX    = AHC_VIDEO_STICKER_X_DEFAULT_POS;
		    *StickerY    = CaptureHeight - *uwSrcH - VIDEO_STICKER_BORDER_OFFSET - AHC_VIDEO_STICKER_Y_DELTA;
		}
		else if  ( PositionConfig == AHC_ACC_TIMESTAMP_TOP_RIGHT ){
		    *StickerX    = CaptureWidth  - *uwSrcW - VIDEO_STICKER_BORDER_OFFSET;
		    *StickerY    = AHC_VIDEO_STICKER_Y_DEFAULT_POS;
		}
		else if  ( PositionConfig == AHC_ACC_TIMESTAMP_TOP_LEFT ){
		    *StickerX    = AHC_VIDEO_STICKER_X_DEFAULT_POS;
		    *StickerY    = AHC_VIDEO_STICKER_Y_DEFAULT_POS;
		}

		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetBkColor(uiOSDid, GUI_TRANSPARENT);
		AHC_OSDSetColor(uiOSDid, GUI_BLACK);
		#else
        AHC_OSDSetBkColor(GUI_TRANSPARENT);
		AHC_OSDSetColor(GUI_BLACK);
		#endif
		AHC_OSDDrawFillRect(uiOSDid, 0, 0, *uwSrcW, *uwSrcH);
		
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
		AHC_OSDSetColor(uiOSDid, GUI_WHITE);
		OldFont = AHC_OSDGetFont(uiOSDid);
		#else
		AHC_OSDSetColor(GUI_WHITE);
		OldFont = AHC_OSDGetFont();
		#endif
		
        #ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetFont(uiOSDid, &STICKER_FRONT_1);
        AHC_OSDSetColor(uiOSDid, GUI_WHITE);
        #else
        AHC_OSDSetFont(&STICKER_FRONT_1);
        AHC_OSDSetColor(GUI_WHITE);
        #endif
		
        #if (STICKER_PATTERN == 1)
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(uiOSDid, GUI_ORANGE);
		#else
        AHC_OSDSetColor(GUI_ORANGE);
        #endif // #ifdef ENABLE_GUI_SUPPORT_MULTITASK
        //DrawText of LOGO
        if (TimeStampOp & AHC_ACC_TIMESTAMP_LOGOEX_MASK)
        {
		    #if (UUID_XML_FEATURE == 1)
		    UI_GetProductName( szProductName, PRODUCT_NAME_MAX_SIZE );
		    AHC_OSDDrawText(uiOSDid, (UINT8*)szProductName, uiPos, 0, STRLEN(szProductName));
		    #else
            {
                UINT32 modeNameAddr;

                if (AHC_Menu_SettingGetCB((char*)COMMON_KEY_STICKER_LOGO_TXT, &modeNameAddr) == AHC_TRUE)
                {
    				AHC_OSDDrawText(uiOSDid, (UINT8 *) modeNameAddr, uiPosX, 0, STRLEN((char *) modeNameAddr));
                }
		    }
		    #endif  // #if (UUID_XML_FEATURE == 1)
        }
		#endif  // #if (STICKER_PATTERN == 1)
        //DrawText of Date TimeStamp
        if (TimeStampOp & AHC_ACC_TIMESTAMP_DATEEX_MASK){
            AHC_OSDDrawText(uiOSDid, (UINT8*)byDate, 0, uiPosY, STRLEN(byDate));
        }
		#if (GPS_CONNECT_ENABLE == 1)
        if (0/*MenuSettingConfig()->uiDistanceUnit == DISTANCE_UNIT_METRIC*/)
        {
        
            UINT32 uiSpeed = GPSCtrl_GetSpeed(0);
            UINT32 uiDigit;

            uiDigit = NumOfDigit(uiSpeed);
            
            AHC_OSDDrawDec(uiOSDid, uiSpeed, 210, uiPosY, uiDigit);
            AHC_OSDDrawText(uiOSDid, (UINT8*)"Kmh", 220+10*uiDigit, uiPosY, STRLEN("Kmh"));
            
        }
        else if (0/*MenuSettingConfig()->uiDistanceUnit ==DISTANCE_UNIT_IMPERIAL*/)
        {
            UINT32 uiSpeed = GPSCtrl_GetSpeed(1);
            UINT32 uiDigit;

            uiDigit = NumOfDigit(uiSpeed);
                
            AHC_OSDDrawDec(uiOSDid, uiSpeed, 210, uiPosY, uiDigit);

            AHC_OSDDrawText(uiOSDid, (UINT8*)"Mph", 220+10*uiDigit, uiPosY, STRLEN("Mph"));
        }
		#endif
	}
#endif  // _OEM_VR_STICKER_

	#if (AHC_GPS_STICKER_ENABLE && SUPPORT_GPS)
	{
	    if(AHC_GPS_Module_Attached())
	    { 
			if(MenuSettingConfig()->uiGPSStamp == GPS_STAMP_ON) {
			    
			    UINT16 		uiDegree = 0, uiMinute = 0;
				UINT16 		tempX;
				UINT16		tempY;
			    double 		uiSec = 0;
			    
				#if(GPS_MODULE == GPS_MODULE_NMEA0183)
				NMEAINFO   *pInfo;
			    pInfo =(NMEAINFO *)GPS_Information();
			    #elif(GPS_MODULE == GPS_MODULE_GMC1030)
			    GPSRADARINFO   *pInfo;
			    pInfo =(GPSRADARINFO *)GPSRadar_Information();
			    #endif
			    
			    tempX = 0;
			    tempY = 40;
			    
				#ifdef ENABLE_GUI_SUPPORT_MULTITASK
			    AHC_OSDSetFont(uiOSDid, &STICKER_FRONT_2);
			    #else
			    AHC_OSDSetFont(&STICKER_FRONT_2);
			    #endif
			    
			    if(GPSCtrl_GPS_IsValidValue())
				{
					UINT8 ubInterval = 12;
					UINT8 ubDigits	 = 1;
					
					if(pInfo->dwLat>0)
						AHC_OSDDrawText(uiOSDid,(UINT8 *)"N ", 0, tempY, sizeof("N "));
					else
						AHC_OSDDrawText(uiOSDid,(UINT8 *)"S ", 0, tempY, sizeof("S "));
					
					GPSCtrl_GPSFormatTrans(pInfo->dwLat, &uiDegree, &uiMinute, &uiSec);
					
					if(uiDegree < 10)
					{
						ubDigits = 1; 			
					}else
					{
						ubDigits = 2;
					}
					ubInterval *= (2 - ubDigits);
					
			        tempX += 15;
			        AHC_OSDDrawText(uiOSDid,(UINT8 *)" ", tempX + ubInterval, tempY, sizeof(" "));
					AHC_OSDDrawFloat(uiOSDid, uiDegree, ubDigits );

					tempX += 30;
					AHC_UC_SetEncodeUTF8();
			    	AHC_OSDDrawText(uiOSDid,(UINT8 *)"\xB0", tempX, tempY, sizeof("\xB0"));
			    	AHC_UC_SetEncodeNone();
			    	AHC_OSDDrawFloat(uiOSDid, uiMinute, 2 );

			    	tempX += 30;
			    	AHC_OSDDrawText(uiOSDid,(UINT8 *)"'", tempX, tempY, sizeof("'"));
			    	AHC_OSDDrawFloat(uiOSDid, uiSec, 4 );

			    	tempX += 50;
			    	AHC_OSDDrawText(uiOSDid,(UINT8 *)"''", tempX, tempY, sizeof("''"));

			    	if(pInfo->dwLon>0)
						AHC_OSDDrawText(uiOSDid,(UINT8 *)"E ", 130+20, tempY, sizeof("E "));
					else
						AHC_OSDDrawText(uiOSDid,(UINT8 *)"W ", 130+20, tempY, sizeof("W "));
					
					GPSCtrl_GPSFormatTrans(pInfo->dwLon, &uiDegree, &uiMinute, &uiSec);
			        
					
					ubInterval = 12;
			        
			        if(uiDegree < 10)
					{
						ubDigits = 1; 			
					}else if(uiDegree < 100 )
					{
						ubDigits = 2;
					}else
					{
						ubDigits = 3;
					}
					ubInterval *= (3 - ubDigits);
					tempX = 150+20;
					AHC_OSDDrawText(uiOSDid,(UINT8 *)" ", tempX + ubInterval, tempY, sizeof(" "));
					AHC_OSDDrawFloat(uiOSDid, uiDegree, ubDigits );

					tempX += 45;
					AHC_UC_SetEncodeUTF8();	    	
			    	AHC_OSDDrawText(uiOSDid,(UINT8 *)"\xB0", tempX, tempY, sizeof("\xB0"));
			    	AHC_UC_SetEncodeNone();
			    	AHC_OSDDrawFloat(uiOSDid, uiMinute, 2 );

			    	tempX += 30;
			    	AHC_OSDDrawText(uiOSDid,(UINT8 *)"'", tempX, tempY, sizeof("'"));
			    	AHC_OSDDrawFloat(uiOSDid, uiSec, 4 );

			    	tempX += 50;
			    	AHC_OSDDrawText(uiOSDid,(UINT8 *)"''",tempX,tempY,sizeof("''")); 	
				}
				else 
				{
			        AHC_UC_SetEncodeUTF8();

			    	AHC_OSDDrawText(uiOSDid, 
			    	                (UINT8*)"-  - -\xB0  - -' - - - -''   -  - -\xB0  - -' - - - -'' ", 
			    	                tempX, tempY, 
			    	                STRLEN("-  - -\xB0  - -' - - - -''   -  - -\xB0  - -' - - - -'' "));

			    	AHC_UC_SetEncodeNone();
			       
				}
			}
		}
	}
	#endif

    #if (STICKER_DRAW_EDGE)
    AIHC_DrawTextEdge(uiSrcAddr, uwSrcW, uwSrcH, 0xFF, 0x00, 0x01);   
    #endif

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetFont(uiOSDid, OldFont);
	#else
    AHC_OSDSetFont(OldFont);
	#endif
	
    AIHC_DrawReservedOSD(AHC_FALSE);
	END_LAYER(uiOSDid);
    if (!AHC_IsTVConnectEx()) {
		//TV_OUT: OSD_ID use 17,18 and 19
        END_LAYER(OVL_BUFFER_INDEX);
    }
    END_LAYER(OVL_BUFFER_INDEX1);
    END_LAYER(OVL_BUFFER_INDEX2);
    END_LAYER(OVL_BUFFER_INDEX3);
#endif
}

void DrawVideoRecStateUpdate(void)
{
    UINT8    bID0 = 0;
    UINT8    bID1 = 0;
	static UINT8 m_VidDrawCounter 	= 0;
    static UINT8 ubMediaStatus 	= AHC_FALSE;
    static UINT8 ubLockStatus 	= AHC_FALSE;
    static UINT8 ubMuteStatus 	= AHC_FALSE;
#if (DAY_NIGHT_MODE_SWITCH_EN)
    static UINT8 ubNightStatus 	= AHC_FALSE;
#endif
#if (LIMIT_MAX_LOCK_FILE_NUM)
    static UINT8 ubLockFileNum	= 0;
	static UINT8 ubLockEventNum	= 0;
#endif
#if (MOTION_DETECTION_EN)
	static UINT8 ubVMDStatus    = AHC_FALSE;
#endif
#if defined (CFG_DRAW_REC_SD_ABSENT) && CHARGE_FULL_NOTIFY//may be defined in config_xxx.h
	static UINT8 ubAdapterStatus = 0;
	static UINT8 m_PowerOnInitial = 1;
#endif

#if (TVOUT_PREVIEW_EN)

	if ( TVFunc_CheckTVStatus(AHC_TV_VIDEO_PREVIEW_STATUS) )
	{
		if (TVFunc_IsInMenu())
			return;
	
		bID0 = TVFunc_GetUImodeOsdID();
			
		if (m_ubBatteryStatus != MenuSettingConfig()->uiBatteryVoltage)
		{
			UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_BATTERY, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
			m_ubBatteryStatus = MenuSettingConfig()->uiBatteryVoltage;
		}

		if(bUpdateFlashLedIcon)
		{
			UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_FLASH, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
			bUpdateFlashLedIcon = AHC_FALSE;
		}

	    if(ubMediaStatus != AHC_SDMMC_GetMountState()) 
	    {
	        UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_SD_STATE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
	        ubMediaStatus = AHC_SDMMC_GetMountState();
	    }

		if(ubLockStatus != VideoFunc_LockFileEnabled())
		{
			UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_LOCK_FILE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
			ubLockStatus = !ubLockStatus;
		}

		if(ubMuteStatus != bMuteRecord)
		{
			UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_MUTE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
			ubMuteStatus = bMuteRecord;
		}

		#if (LIMIT_MAX_LOCK_FILE_NUM)
		if(ubLockFileNum != m_ulLockFileNum)
		{
			UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_LOCK_NUM, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
			ubLockFileNum = m_ulLockFileNum;
		}
		
		if(ubLockEventNum != m_ulLockEventNum)
		{
			UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_LOCK_NUM, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
			ubLockEventNum = m_ulLockEventNum;
		}
		#endif
		
		#if (DAY_NIGHT_MODE_SWITCH_EN)
		if(ubNightStatus != bNightMode)
		{
			UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_SCENE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
			ubNightStatus = bNightMode;
		}
		#endif

		#if (MOTION_DETECTION_EN)
		if(ubVMDStatus != m_ubMotionDtcEn)
		{
			#ifdef CFG_DRAW_REC_MOTION_SEN_OFF //may be defined in config_xxx.h
			if( MenuSettingConfig()->uiMotionDtcSensitivity == MOTION_DTC_SENSITIVITY_OFF)
			{
				UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_VMD_STATE, AHC_TRUE, AHC_FALSE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
				ubVMDStatus = AHC_FALSE;
			}
			else
			#endif
			{
				UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_VMD_STATE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
				ubVMDStatus = m_ubMotionDtcEn;
			}
			
			#ifdef CFG_DRAW_REC_MOTION_SEN_OFF_POST //may be defined in config_xxx.h
			if(m_ubMotionDtcEn == AHC_TRUE)
			{
				if( MenuSettingConfig()->uiMotionDtcSensitivity == MOTION_DTC_SENSITIVITY_OFF)
					UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_VMD_STATE, AHC_TRUE, AHC_FALSE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
			}
			#endif
		}
		#endif
		
		#ifdef CFG_DRAW_REC_UPDATE_LOCK_FILE //may be defined in config_xxx.h
		if(VideoFunc_RecordStatus() && VideoFunc_LockFileEnabled())
			UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_LOCK_FILE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, (m_VidDrawCounter & 0x01));
		
		//To detect Gsensor OK or not
		if (Gsensor_ReadDeviceID() != AHC_ERR_NONE) 
		{
			UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, (m_VidDrawCounter & 0x01));
		}
		else
		{
			UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		}
		#endif
		
		UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_REC_POINT,     AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, 8);
		UIDrawTV_VideoFuncEx(bID0, TV_GUI_AUDIO_REC_POINT,     AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, 8);
		UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_RTC_TIME,      AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_REC_TIME,      AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_REMAIN_TIME,   AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		
		if(m_VidDrawCounter == 0)
		{
			#if (MOTION_DETECTION_EN)
    		if(m_ulVMDRemindTime > 0)
    		{
				#if defined(WIFI_PORT) && (WIFI_PORT == 1)
	        	if (WLAN_SYS_GetMode() != -1) {
	        		if(AHC_Get_WiFi_Streaming_Status())
	        			m_ulVMDCancel = AHC_TRUE;
	        	}
				#endif

    			if (m_ulVMDCancel) {
    				m_ulVMDRemindTime = 0;
    				// Clean Screen
					UpdateMotionRemindTime(-1);
    			} else {
					m_ulVMDRemindTime--;
					if(m_ubInRemindTime)
						UpdateMotionRemindTime(m_ulVMDRemindTime);
				}
    		}
    		else
    		{
    			if (m_ubMotionDtcEn)
    				m_ubVMDStart = AHC_TRUE;
    		}
			#endif
		}

        #if	OSD_SHOW_BATTERY_STATUS == 1
		AHC_DrawChargingIcon_UsingTimer(bID0);
        #endif

		#if (SUPPORT_GPS)
		if(bGPS_PageExist)
		{
			CHARGE_ICON_ENABLE(AHC_FALSE);
			QuickDrawGPSInfoPage(bID0, AHC_FALSE);
			QuickDrawGPSInfoPage(bID0, AHC_TRUE);
			CHARGE_ICON_ENABLE(AHC_TRUE);
		}
		else
		{
		    #if GPS_MODULE_SHOW_OSD
			if (!GPSCtrl_ModuleConnected())
				UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_GPS_STATE,    AHC_TRUE,  AHC_FALSE,   OSD_COLOR_TRANSPARENT, NULL_PARAM);
			else
				UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_GPS_STATE,    AHC_FALSE,  AHC_TRUE,    OSD_COLOR_TRANSPARENT, NULL_PARAM);
		    #endif
		}
		#endif

    	#if defined(WIFI_PORT) && (WIFI_PORT == 1)
		// WiFi
		#if (SUPPORT_GPS)
		if(!bGPS_PageExist)
		#endif
		{
			int		status = get_NetAppStatus();
			/* be careful the enum VIDEO_GUI_BLOCK, the ORDER and NUMBER must be same in enum and UI_DARW_SET */
			UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_WIFI, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, status);
			UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_WIFI_SWITCH, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, status);
		}
    	#endif

		#if (ENABLE_ADAS_LDWS)
		// Draw LDWS line
		UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_GUI_LDWS_LINE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		#endif

        #if (ENABLE_ADAS_FCWS)
		// Draw FCWS Distance
		UIDrawTV_VideoFuncEx(bID0, TV_GUI_VIDEO_GUI_FCWS_INFO, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
        #endif
	}
	else
#endif      // #if (TVOUT_PREVIEW_EN)
#if (HDMI_PREVIEW_EN)
	if (HDMIFunc_CheckHdmiStatus(AHC_HDMI_VIDEO_PREVIEW_STATUS))
	{
		if (HDMIFunc_IsInMenu())
			return;

        #if (HDMI_SINGLE_OSD_BUF)
		bID0 = HDMIFunc_GetUImodeOsdID();
		#else
		OSDDraw_EnterDrawing(&bID0, &bID1);
		#endif
			
		if(m_ubBatteryStatus != MenuSettingConfig()->uiBatteryVoltage)
		{
			UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_BATTERY, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);	
			m_ubBatteryStatus = MenuSettingConfig()->uiBatteryVoltage;
		}

		if(bUpdateFlashLedIcon)
		{
			UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_FLASH, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);			
			bUpdateFlashLedIcon = AHC_FALSE;
		}

	    if(ubMediaStatus != AHC_SDMMC_GetMountState()) 
	    {
	        UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_SD_STATE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
	        ubMediaStatus = AHC_SDMMC_GetMountState();
	    }

		if(ubLockStatus != VideoFunc_LockFileEnabled())
		{
			UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_LOCK_FILE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
			ubLockStatus = !ubLockStatus;
		}

		if(ubMuteStatus != bMuteRecord)
		{
			UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_MUTE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
			ubMuteStatus = bMuteRecord;
		}

		#if (LIMIT_MAX_LOCK_FILE_NUM)
		if(ubLockFileNum != m_ulLockFileNum)
		{
			UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_LOCK_NUM, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
			ubLockFileNum = m_ulLockFileNum;
		}
		
		if(ubLockEventNum != m_ulLockEventNum)
		{
			UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_LOCK_NUM, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
			ubLockEventNum = m_ulLockEventNum;
		}
		#endif
		
		#if (DAY_NIGHT_MODE_SWITCH_EN)
		if(ubNightStatus != bNightMode)
		{
			UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_SCENE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
			ubNightStatus = bNightMode;
		}
		#endif

		#if (MOTION_DETECTION_EN)
		if(ubVMDStatus != m_ubMotionDtcEn)
		{
			#ifdef CFG_DRAW_REC_MOTION_SEN_OFF //may be defined in config_xxx.h
			if( MenuSettingConfig()->uiMotionDtcSensitivity == MOTION_DTC_SENSITIVITY_OFF)
			{
				UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_VMD_STATE, AHC_TRUE, AHC_FALSE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
				ubVMDStatus = AHC_FALSE;
			}
			else
			#endif
			{
				UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_VMD_STATE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
				ubVMDStatus = m_ubMotionDtcEn;
			}
			
			#ifdef CFG_DRAW_REC_MOTION_SEN_OFF_POST //may be defined in config_xxx.h
			if(m_ubMotionDtcEn == AHC_TRUE)
			{
				if( MenuSettingConfig()->uiMotionDtcSensitivity == MOTION_DTC_SENSITIVITY_OFF)
					UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_VMD_STATE, AHC_TRUE, AHC_FALSE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
			}
			#endif
		}
		#endif
		
		#ifdef CFG_DRAW_REC_UPDATE_LOCK_FILE //may be defined in config_xxx.h
		if(VideoFunc_RecordStatus() && VideoFunc_LockFileEnabled())
			UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_LOCK_FILE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, (m_VidDrawCounter & 0x01));
		
		//To detect Gsensor OK or not
		if (Gsensor_ReadDeviceID() != AHC_ERR_NONE) 
		{
			UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, (m_VidDrawCounter & 0x01));
		}
		else
		{
			UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		}
		#endif
		
		UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_REC_POINT,     AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, 8);
		UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_AUDIO_REC_POINT,     AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, 8);
		UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_RTC_TIME,      AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_REC_TIME,      AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_REMAIN_TIME,   AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		
		if(m_VidDrawCounter == 0)
		{
			#if (MOTION_DETECTION_EN)
    		if(m_ulVMDRemindTime > 0)
    		{
				#if defined(WIFI_PORT) && (WIFI_PORT == 1)
	        	if (WLAN_SYS_GetMode() != -1) {
	        		if(AHC_Get_WiFi_Streaming_Status())
	        			m_ulVMDCancel = AHC_TRUE;
	        	}
				#endif

    			if (m_ulVMDCancel) {
    				m_ulVMDRemindTime = 0;
    				// Clean Screen
					UpdateMotionRemindTime(-1);
    			} else {
					m_ulVMDRemindTime--;
					if(m_ubInRemindTime)
						UpdateMotionRemindTime(m_ulVMDRemindTime);
				}
    		}
    		else
    		{
    			if (m_ubMotionDtcEn)
    				m_ubVMDStart = AHC_TRUE;
    		}
			#endif
		}

        #if	OSD_SHOW_BATTERY_STATUS == 1
		AHC_DrawChargingIcon_UsingTimer(bID0);
        #endif

		#if (SUPPORT_GPS)
		if(bGPS_PageExist)
		{
			CHARGE_ICON_ENABLE(AHC_FALSE);
			QuickDrawGPSInfoPage(bID0, AHC_FALSE);
			QuickDrawGPSInfoPage(bID0, AHC_TRUE);
			CHARGE_ICON_ENABLE(AHC_TRUE);
		}
		else
		{
		    #if GPS_MODULE_SHOW_OSD
			if (!GPSCtrl_ModuleConnected()) {
				UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_GPS_STATE,    AHC_TRUE,  AHC_FALSE,   OSD_COLOR_TRANSPARENT, NULL_PARAM);
			}	
			else {
				//UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_GPS_STATE,    AHC_TRUE,  AHC_TRUE,    OSD_COLOR_TRANSPARENT, NULL_PARAM);
				UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_GPS_STATE,    AHC_FALSE, AHC_TRUE,    OSD_COLOR_TRANSPARENT, NULL_PARAM);
			}	
		    #endif
		}
		#endif

    	#if defined(WIFI_PORT) && (WIFI_PORT == 1)
		// WiFi
		#if (SUPPORT_GPS)
		if(!bGPS_PageExist)
		#endif
		{
			int		status = get_NetAppStatus();
			/* be careful the enum VIDEO_GUI_BLOCK, the ORDER and NUMBER must be same in enum and UI_DARW_SET */
			UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_WIFI, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, status);
			UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_WIFI_SWITCH, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, status);
		}
    	#endif

		#if (ENABLE_ADAS_LDWS)
		// Draw LDWS line
		UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_GUI_LDWS_LINE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		#endif

        #if (ENABLE_ADAS_FCWS)
		// Draw FCWS Distance
		UIDrawHdmi_VideoFuncEx(bID0, HDMI_GUI_VIDEO_GUI_FCWS_INFO, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
        #endif

		#if (!HDMI_SINGLE_OSD_BUF)
		OSDDraw_ExitDrawing(&bID0, &bID1);
		#endif
		
	}
	else
#endif      // #if (HDMI_PREVIEW_EN)
	{
	    OSDDraw_EnterDrawing(&bID0, &bID1);

		#if (defined(WIFI_PORT) && (WIFI_PORT == 1) && defined(CFG_DRAW_WIFI_STREAMING_MONO_PATTERN))
		if (AHC_Get_WiFi_Streaming_Status()) {
			if (AHC_FALSE == monoPatternDrawed) {
            	UINT16 idx;

                printc(FG_BLUE("Draw Mono Patttern ...\r\n"));
            	AHC_OSDSetBkColor(bID0, CFG_DRAW_WIFI_STREAMING_MONO_PATTERN);
				OSDDraw_ClearPanel(bID0, CFG_DRAW_WIFI_STREAMING_MONO_PATTERN);

				#ifdef CFG_WIFI_STREAMING_LOGO_IN_CENETR
				// NOP - Do not draw all icons
				#else
            	for (idx = 0; idx < VIDEO_GUI_MAX; idx++)
            	{
            		if (idx == VIDEO_GUI_REC_STATE)
            			UpdateVideoRecordStatus(VIDEO_REC_STOP);

            		if ((idx == VIDEO_GUI_REC_POINT) || (idx == VIDEO_GUI_GPS_STATE))
            			continue;

            		UIDrawVideoFuncEx(bID0, idx, AHC_TRUE, AHC_TRUE, CFG_DRAW_WIFI_STREAMING_MONO_PATTERN, NULL_PARAM);
            	}
				#endif

				monoPatternDrawed = AHC_TRUE;
			}

			#ifdef CFG_WIFI_STREAMING_LOGO_IN_CENETR
			// WiFi
			{
				int		status = get_NetAppStatus();
				/* be careful the enum VIDEO_GUI_BLOCK, the ORDER and NUMBER must be same in enum and UI_DARW_SET */
				UIDrawVideoFuncEx(bID0, VIDEO_GUI_WIFI, AHC_TRUE, AHC_TRUE, CFG_DRAW_WIFI_STREAMING_MONO_PATTERN, status);
				UIDrawVideoFuncEx(bID0, VIDEO_GUI_WIFI_SWITCH, AHC_FALSE, AHC_TRUE, CFG_DRAW_WIFI_STREAMING_MONO_PATTERN, status);
			}
		
			goto L_DrawExit;
			#endif
		}
		else if (AHC_TRUE == monoPatternDrawed) {
			DrawStateVideoRecInit();
			monoPatternDrawed = AHC_FALSE;
		}
		#endif
		
		#if (AUTO_HIDE_OSD_EN)
		if(VideoFunc_RecordStatus())
		{
			m_ulOSDCounter++;
						
			if(m_ulOSDCounter==VR_OSD_HIDE_TIME) //60sec
			{
				m_ubHideOSD = AHC_TRUE;
				#ifdef CFG_DRAW_WIFI_STREAMING_MONO_PATTERN
				if (AHC_Get_WiFi_Streaming_Status())
					OSDDraw_ClearPanel(bID0, CFG_DRAW_WIFI_STREAMING_MONO_PATTERN);
				else
				#endif
					OSDDraw_ClearPanel(bID0, OSD_COLOR_TRANSPARENT);
			
				goto L_DrawExit;
			}
			else if(m_ulOSDCounter > VR_OSD_HIDE_TIME)
			{
				DrawVideoRecOsdDuringHide(bID0);
				
				goto L_DrawExit;
			}
		}
		else
		{
			m_ulOSDCounter = 0;
		}
		
		#endif      // #if (AUTO_HIDE_OSD_EN)
		
		#if (VIDEO_DIGIT_ZOOM_EN)
		UIDrawVideoFuncEx(bID0, VIDEO_GUI_ZOOM_BAR, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, AHC_FALSE);
		#endif

        #if defined (CFG_DRAW_REC_SD_ABSENT)
		if(!AHC_IsSDInserted())
		{
        #endif
			if(m_ubBatteryStatus != MenuSettingConfig()->uiBatteryVoltage)
			{
				UIDrawVideoFuncEx(bID0, VIDEO_GUI_BATTERY, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
				m_ubBatteryStatus = MenuSettingConfig()->uiBatteryVoltage;
			}
   		#if defined (CFG_DRAW_REC_SD_ABSENT)
		}
		else
		{
			static UINT8 m_ubSDMMCStatus = 0;

			if(m_ubSDMMCStatus!=SDMMCSpaceCalculation())
			{
	    		m_ubSDMMCStatus=SDMMCSpaceCalculation();
	    		if(m_PowerOnInitial==0)
	    		LedCtrl_FlickerLedBySDMMC(KEY_LED_FUNC4,SDMMCSpaceCalculation());
	    		else
				m_PowerOnInitial=0;
	    		printc(">>>>>>>>>>Video Memory\r\n");	
			}
		}
		
		if(AdapterIsConnect==AHC_TRUE)
		{
			LedCtrl_ButtonLed(KEY_LED_FUNC4, AHC_FALSE);

	        #if (CHARGE_FULL_NOTIFY)
			if(bChargeFull==AHC_FALSE)
			{
				LedCtrl_ButtonLed(KEY_LED_FUNC3, ubAdapterStatus & 0x08);
				ubAdapterStatus++;
			}
			else
			{
				LedCtrl_ButtonLed(KEY_LED_FUNC3, AHC_TRUE);
				ubAdapterStatus = 0x02;
			}
	        #endif
		}
		else
		{
			ubAdapterStatus = 0x02;
		}
		#endif      // CFG_DRAW_REC_SD_ABSENT
		
		if(bUpdateFlashLedIcon)
		{
			UIDrawVideoFuncEx(bID0, VIDEO_GUI_FLASH, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);			
			bUpdateFlashLedIcon = AHC_FALSE;
		}
		
	    if(ubMediaStatus != AHC_SDMMC_GetMountState()) 
	    {
			UIDrawVideoFuncEx(bID0, VIDEO_GUI_SD_STATE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
	        ubMediaStatus = AHC_SDMMC_GetMountState();
	    }
		
		if(ubLockStatus != VideoFunc_LockFileEnabled())
		{
			UIDrawVideoFuncEx(bID0, VIDEO_GUI_LOCK_FILE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
			ubLockStatus = !ubLockStatus;
		}
		
		if(ubMuteStatus != bMuteRecord)
		{
			UIDrawVideoFuncEx(bID0, VIDEO_GUI_MUTE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
			ubMuteStatus = bMuteRecord;
		}

		#if (LIMIT_MAX_LOCK_FILE_NUM)
		if(ubLockFileNum != m_ulLockFileNum)
		{
			UIDrawVideoFuncEx(bID0, VIDEO_GUI_LOCK_NUM, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
			ubLockFileNum = m_ulLockFileNum;
		}
		
		if(ubLockEventNum != m_ulLockEventNum)
		{
			UIDrawVideoFuncEx(bID0, VIDEO_GUI_LOCK_NUM, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
			ubLockEventNum = m_ulLockEventNum;
		}
		#endif
		
		#if (DAY_NIGHT_MODE_SWITCH_EN)	
		if(ubNightStatus != bNightMode)
		{
			UIDrawVideoFuncEx(bID0, VIDEO_GUI_SCENE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
			ubNightStatus = bNightMode;
		}
		#endif
#if WIFI_PORT == 1
		if (!AHC_Get_WiFi_Streaming_Status())
#endif
		if(VideoRecSize != MenuSettingConfig()->uiMOVSize)
		{
			printc(">>>>>>>>[Video mode]MenuSettingConfig()->uiMOVSize = %d\r\n",MenuSettingConfig()->uiMOVSize);
			UIDrawVideoFuncEx(bID0, VIDEO_GUI_MOVIE_RES, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		}
		
	#if (MOTION_DETECTION_EN)
		if(ubVMDStatus != m_ubMotionDtcEn)
		{
			#ifdef CFG_DRAW_REC_MOTION_SEN_OFF //may be defined in config_xxx.h
			if( MenuSettingConfig()->uiMotionDtcSensitivity == MOTION_DTC_SENSITIVITY_OFF)
			{
				UIDrawVideoFuncEx(bID0, VIDEO_GUI_VMD_STATE, AHC_TRUE, AHC_FALSE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
				ubVMDStatus = AHC_FALSE;
			}
			else
			#endif
			{
				UIDrawVideoFuncEx(bID0, VIDEO_GUI_VMD_STATE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
				ubVMDStatus = m_ubMotionDtcEn;
			}
		}
		
		#ifdef CFG_DRAW_REC_MOTION_SEN_OFF_POST //may be defined in config_xxx.h
		if(m_ubMotionDtcEn == AHC_TRUE)
		{
			if( MenuSettingConfig()->uiMotionDtcSensitivity == MOTION_DTC_SENSITIVITY_OFF)
				UIDrawVideoFuncEx(bID0, VIDEO_GUI_VMD_STATE, AHC_TRUE, AHC_FALSE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		}
		#endif
	#endif      // #if (MOTION_DETECTION_EN)
		
		#ifdef CFG_DRAW_REC_UPDATE_LOCK_FILE //may be defined in config_xxx.h
		if(VideoFunc_RecordStatus() && VideoFunc_LockFileEnabled())
			UIDrawVideoFuncEx(bID0, VIDEO_GUI_LOCK_FILE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, m_VidDrawCounter);
		
		if (Gsensor_ReadDeviceID() != AHC_ERR_NONE) 
		{
			UIDrawVideoFuncEx(bID0, VIDEO_GUI_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, (m_VidDrawCounter & 0x01));
		}
		else
		{
			UIDrawVideoFuncEx(bID0, VIDEO_GUI_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, 0);
		}
		#endif
		
		UIDrawVideoFuncEx(bID0, VIDEO_GUI_REC_POINT,    AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, 6);
		UIDrawVideoFuncEx(bID0, AUDIO_GUI_REC_POINT,    AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, 6);
		UIDrawVideoFuncEx(bID0, VIDEO_GUI_RTC_TIME, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		UIDrawVideoFuncEx(bID0, VIDEO_GUI_REC_TIME, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		UIDrawVideoFuncEx(bID0, VIDEO_GUI_REMAIN_TIME, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);					
		
#if (SUPPORT_GSENSOR)
		if (AHC_GetQuickKey(QUICK_UP))
			Dump_Gsensor_XYZ(bID0);
#endif

		#if (SW_STICKER_EN == 1)
		// NOP
		#else
		if (m_VidDrawCounter == 0)
		#endif
	   	{ 
			#if (MOTION_DETECTION_EN)
    		if(m_ulVMDRemindTime > 0)
    		{
	 			#if defined(WIFI_PORT) && (WIFI_PORT == 1)
	        	if (WLAN_SYS_GetMode() != -1) {
	        		if(AHC_Get_WiFi_Streaming_Status())
	        			m_ulVMDCancel = AHC_TRUE;
	        	}
				#endif
 
    			if (m_ulVMDCancel) {
    				m_ulVMDRemindTime = 0;
    				// Clean Screen
					UpdateMotionRemindTime(-1);
    			} else {
					m_ulVMDRemindTime--;
					if(m_ubInRemindTime)
						UpdateMotionRemindTime(m_ulVMDRemindTime);
				}
    		}
    		else
    		{
    			if (m_ubMotionDtcEn)
    				m_ubVMDStart = AHC_TRUE;
    		}
			#endif
    		#if	OSD_SHOW_BATTERY_STATUS == 1
    		AHC_DrawChargingIcon_UsingTimer(bID0);
    		#endif
		}

		#if (SUPPORT_GPS)
		if(bGPS_PageExist)
		{
			CHARGE_ICON_ENABLE(AHC_FALSE);
			QuickDrawGPSInfoPage(bID0, AHC_FALSE);
			QuickDrawGPSInfoPage(bID0, AHC_TRUE);
			CHARGE_ICON_ENABLE(AHC_TRUE);
		}
		else
		{
		    #if GPS_MODULE_SHOW_OSD
			if (!GPSCtrl_ModuleConnected())
				UIDrawVideoFuncEx(bID0, VIDEO_GUI_GPS_STATE,    AHC_TRUE,  AHC_FALSE,   OSD_COLOR_TRANSPARENT, NULL_PARAM);
			else
				UIDrawVideoFuncEx(bID0, VIDEO_GUI_GPS_STATE,    AHC_TRUE,  AHC_TRUE,    OSD_COLOR_TRANSPARENT, NULL_PARAM);
		    #endif
		}
		#endif

    	#if defined(WIFI_PORT) && (WIFI_PORT == 1)
		// WiFi
		#if (SUPPORT_GPS)
		if(!bGPS_PageExist)
		#endif
		{
			int		status = get_NetAppStatus();
			/* be careful the enum VIDEO_GUI_BLOCK, the ORDER and NUMBER must be same in enum and UI_DARW_SET */
			UIDrawVideoFuncEx(bID0, VIDEO_GUI_WIFI, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, status);
			UIDrawVideoFuncEx(bID0, VIDEO_GUI_WIFI_SWITCH, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, status);
		}
    	#endif
		
		goto L_DrawExit;//avoid warning C2866W label is not used
L_DrawExit:
		#if (ENABLE_ADAS_LDWS)
		// Draw LDWS line
		UIDrawVideoFuncEx(bID0, VIDEO_GUI_LDWS_LINE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		#endif

        #if (ENABLE_ADAS_FCWS)
		// Draw FCWS Distance
		UIDrawVideoFuncEx(bID0, VIDEO_GUI_FCWS_INFO, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
        #endif
        AHC_OSDUpdateAddrOnly(AHC_TRUE);
	    OSDDraw_ExitDrawing(&bID0, &bID1);
        AHC_OSDUpdateAddrOnly(AHC_FALSE);
	}

	m_VidDrawCounter ^= 1;

	#ifdef CFG_SDMMC_ERROR_TURNON_REC_LED
	if (!VideoFunc_RecordStatus())
	{
		if(AHC_FALSE == AHC_SDMMC_GetMountState())
		{
			LedCtrl_ButtonLed(AHC_GetVideoModeLedGPIOpin(), AHC_TRUE);
		}
		else
		{
			LedCtrl_ButtonLed(AHC_GetVideoModeLedGPIOpin(), AHC_FALSE);
		}
	}
	#endif

	#ifdef FLM_GPIO_NUM
	if (AHC_Get_WiFi_Streaming_Status())
	AHC_OSDRefresh_PLCD();
	#endif
}

#ifdef CFG_BOOT_CLEAR_ICON_WHEN_OFF //may be defined in config_xxx.h
			//If LCD gets poor quality, and icons is sticky
			//Need to clear icons
void DrawStateVideoRecClearIcon(void)
{
    UINT8  	bID0 = 0;
    UINT8  	bID1 = 0;
	UINT8	idx	 = 0;

	if(uiGetCurrentState() == UI_HDMI_STATE)
	{
	    printc("--E-- %s wrong enter for HDMI \r\n", __func__);
		return AHC_TRUE;
	}

    OSDDraw_EnterDrawing(&bID0, &bID1);

    OSDDraw_ClearOvlDrawBuffer(bID0);

    OSDDraw_EnableSemiTransparent(bID0, AHC_TRUE);
    OSDDraw_EnableSemiTransparent(bID1, AHC_TRUE);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, 0xAA000000);
	#else
    AHC_OSDSetColor(0xAA000000);
    #endif
	
	for (idx = 0; idx < VIDEO_GUI_MAX; idx++)
	{
		if (idx == VIDEO_GUI_REC_STATE)
			UpdateVideoRecordStatus(VIDEO_REC_STOP);	
			
		if ((idx == VIDEO_GUI_REC_POINT) || (idx == VIDEO_GUI_GPS_STATE))
			continue;

		UIDrawVideoFuncEx(bID0, idx, AHC_TRUE, AHC_FALSE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
	}

    OSDDraw_ExitDrawing(&bID0, &bID1);
}
#endif
#if (ENABLE_ADAS_LDWS)
void DrawVideo_LDWSWarn(int ldws_state, AHC_BOOL bDraw)
{
    static UINT8 RecordPlaySound = 0x01;
    UINT8 bID0, bID1;
    //GUI_COLOR Org_BkColor, OrgColor;

//    printc("m_ulLDWSWarn = %d\r\n", m_ulLDWSWarn);
    if (!bDraw) { // Draw exit DrawVideo_LDWSWarn.
        printc("DrawVideo_LDWSWarn....exit\r\n");
        DrawStateVideoRecInit();
        RecordPlaySound = 0x01;
        return;
    }
//    printc("[15]");
    if (RecordPlaySound) {
        AHC_PlaySoundEffect(AHC_SOUNDEFFECT_ATTENTION);
        AHC_PlaySoundEffect(AHC_SOUNDEFFECT_LDWS_WARNING);
        RecordPlaySound = 0x00;
    }

    OSDDraw_EnterDrawing(&bID0, &bID1);

    // Draw LDWS line
    UIDrawVideoFuncEx(bID0, VIDEO_GUI_LDWS_LINE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);

//    //OSDDraw_EnterPopupDrawing(&bID0, &bID1);
//
//    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
//    OrgColor = AHC_OSDGetColor(bID0);
//    Org_BkColor = AHC_OSDGetBkColor(bID0);
//    AHC_OSDSetColor(bID0, OSD_COLOR_BLACK);
//    AHC_OSDSetBkColor(bID0, OSD_COLOR_BLACK);
//    #else
//    OrgColor = AHC_OSDGetColor();
//    Org_BkColor = AHC_OSDGetBkColor();
//    AHC_OSDSetColor(OSD_COLOR_BLACK);
//    AHC_OSDSetBkColor(OSD_COLOR_BLACK);
//    #endif
//
//    AHC_OSDDrawBitmap(bID0, &BMICON_LDWS_WARNING_LEFT, 80, 80);
//
//    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
//    AHC_OSDSetColor(bID0, OrgColor);
//    AHC_OSDSetBkColor(bID0, Org_BkColor);
//    #else
//    AHC_OSDSetColor(OrgColor);
//    AHC_OSDSetBkColor(Org_BkColor);
//    #endif

    OSDDraw_ExitDrawing(&bID0, &bID1);
    //OSDDraw_ExitPopupDrawing(&bID0, &bID1);
}
#endif

#if (ENABLE_ADAS_FCWS)
void DrawVideo_FCWSWarn(int ldws_state, AHC_BOOL bDraw)
{
    //static UINT8 RecordPlaySound = 0x01;
    //GUI_COLOR Org_BkColor, OrgColor;
    UINT8 bID0, bID1;

    //printc("m_ulFCWSWarn = %d\r\n", m_ulFCWSWarn);
    if (!bDraw) { // Draw exit DrawVideo_FCWSWarn.
        printc("DrawVideo_FCWSWarn....exit\r\n");
        DrawStateVideoRecInit();
        //RecordPlaySound = 0x01;
        return;
    }

    //if (RecordPlaySound) {
        //AHC_PlaySoundEffect(AHC_SOUNDEFFECT_FCWS_WARNING); 
        //RecordPlaySound = 0x00;
    //}

    OSDDraw_EnterDrawing(&bID0, &bID1);

    UIDrawVideoFuncEx(bID0, VIDEO_GUI_FCWS_INFO, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);

//    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
//    OrgColor = AHC_OSDGetColor(bID0);
//    Org_BkColor = AHC_OSDGetBkColor(bID0);
//    AHC_OSDSetColor(bID0, OSD_COLOR_BLACK);
//    AHC_OSDSetBkColor(bID0, OSD_COLOR_BLACK);
//    #else
//    OrgColor = AHC_OSDGetColor();
//    Org_BkColor = AHC_OSDGetBkColor();
//    AHC_OSDSetColor(OSD_COLOR_BLACK);
//    AHC_OSDSetBkColor(OSD_COLOR_BLACK);
//    #endif
//
//    AHC_OSDDrawBitmap(bID0, &BMICON_LDWS_WARNING_LEFT, 0, 0);
//
//    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
//    AHC_OSDSetColor(bID0, OrgColor);
//    AHC_OSDSetBkColor(bID0, Org_BkColor);
//    #else
//    AHC_OSDSetColor(OrgColor);
//    AHC_OSDSetBkColor(Org_BkColor);
//    #endif

    OSDDraw_ExitDrawing(&bID0, &bID1);
}
#endif

#if defined(WIFI_PORT) && (WIFI_PORT == 1)
void DrawVideoWiFi(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    static int			_cnt = 0;
    const GUI_BITMAP*	aicn[] = {&BMICON_WIFI_CONNECTED,		// Remote has connected
    							  &BMICON_WIFI_STREAMING_1,
    							  &BMICON_WIFI_STREAMING_2,
    							  &BMICON_WIFI_STREAMING_3,	// in air
    							};
    const GUI_BITMAP*	pbmp;
    GUI_COLOR			IconColor = 0xAA000000;	

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetBkColor(ubID, bkColor);
    AHC_OSDSetColor(ubID, IconColor);
#else
    AHC_OSDSetBkColor(bkColor);
    AHC_OSDSetColor(IconColor);
#endif

#ifdef CFG_DRAW_WIFI_STREAMING_MONO_PATTERN
	if (AHC_Get_WiFi_Streaming_Status()) {
		bkColor = CFG_DRAW_WIFI_STREAMING_MONO_PATTERN;
		IconColor = CFG_DRAW_WIFI_STREAMING_MONO_PATTERN;
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
		AHC_OSDSetColor(ubID, IconColor);
		AHC_OSDSetBkColor(ubID, bkColor);
		#else
		AHC_OSDSetColor(IconColor);
		AHC_OSDSetBkColor(bkColor);
	    #endif
	}
#endif

    // TODO: bLarge can be replaced by ubMag!!

	if (bLarge != LARGE_SIZE)
		ubMag = 1;

#ifdef CFG_WIFI_STREAMING_LOGO_IN_CENETR
	if (NETAPP_STREAM_PLAY == Para1) {
		x = (m_OSD[ubID]->width >> 1) - (aicn[0]->XSize * ubMag >> 1);
		y = (m_OSD[ubID]->height >> 1) - (aicn[0]->YSize * ubMag >> 1);
	}
#endif

   	if (Para1 == NULL_PARAM  ||
   		bFlag == AHC_FALSE) {
   		// Clean Icon
		UINT16  	x1;
		UINT16  	y1;

		x1 = x + aicn[0]->XSize * ubMag;
		y1 = y + aicn[0]->YSize * ubMag;

		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
		AHC_OSDSetColor(ubID, bkColor);
		AHC_OSDSetBkColor(ubID, bkColor);
		#else
		AHC_OSDSetColor(bkColor);
		AHC_OSDSetBkColor(bkColor);
	    #endif
		AHC_OSDDrawFillRect(ubID, x, y, x1, y1);
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
		AHC_OSDSetColor(ubID, IconColor);
#else
		AHC_OSDSetColor(IconColor);
#endif
	}
	else
	{
		if ((WLAN_SYS_GetMode() == -1) && (NETAPP_WIFI_INIT != Para1))
		{
			pbmp = &BMICON_WIFI_DISCONNECT;
		}
		else
		{
			if (Para1 == NETAPP_STREAM_TEARDOWN || Para1 == NETAPP_WIFI_READY)
			{
				// remote connected
				pbmp = &BMICON_WIFI_ON;
			}
			else if (Para1 == NETAPP_STREAM_PLAY)
			{
				// rtsp in play state
				_cnt = (_cnt + 1) & 0x3;
				pbmp = aicn[_cnt];
			}
			else if (IS_WIFI_FAIL(Para1))
			{
				pbmp = &BMICON_WIFI_ERROR;
			}
			else if (Para1 == NETAPP_WIFI_IDLE)
			{
				// remote disconnect!!
				pbmp = &BMICON_WIFI_IDLE;
			}
			else if (Para1 == NETAPP_WIFI_NOIP || (NETAPP_WIFI_INIT == Para1))
			{
				// Cannot get IP for DHCP server, or Crypto TYPE/KEY not match!!
				pbmp = &BMICON_WIFI_NO_IP;
			}
			else
			{
				pbmp = &BMICON_WIFI_FAIL;
			}
		}

		if(bLarge==LARGE_SIZE)
			AHC_OSDDrawBitmapMag(ubID, pbmp, x, y, ubMag, ubMag);
		else
			AHC_OSDDrawBitmap(ubID, pbmp, x, y);
	}
	END_LAYER(ubID);
}

void DrawVideoWiFiStreaming(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    const GUI_BITMAP*	aicn[] = {&BMICON_WIFI_STREAMING_OFF, &BMICON_WIFI_STREAMING_ON};
    const GUI_BITMAP*	pbmp;
    GUI_COLOR			IconColor = 0xAA000000;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);

#if (defined(WIFI_PORT) && (WIFI_PORT == 1) && defined(CFG_DRAW_WIFI_STREAMING_MONO_PATTERN))
	if (AHC_Get_WiFi_Streaming_Status()) {
		bkColor = CFG_DRAW_WIFI_STREAMING_MONO_PATTERN;
		IconColor = CFG_DRAW_WIFI_STREAMING_MONO_PATTERN;
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
		AHC_OSDSetColor(ubID, IconColor);
		AHC_OSDSetBkColor(ubID, bkColor);
		#else
		AHC_OSDSetColor(IconColor);
		AHC_OSDSetBkColor(bkColor);
	    #endif
	}
#endif
	
    // TODO: bLarge can be replaced by ubMag!!
	if (bLarge != LARGE_SIZE)
		ubMag = 1;

   	if ((Para1 <= 0  ||
		 Para1 == NETAPP_WIFI_NOIP ||
		 bFlag == AHC_FALSE) || (WLAN_SYS_GetMode()==-1)){
   		// Clean Icon
		UINT16  	x1;
		UINT16  	y1;

		x1 = x + aicn[0]->XSize * ubMag;
		y1 = y + aicn[0]->YSize * ubMag;
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
		AHC_OSDSetColor(ubID, bkColor);
		AHC_OSDSetBkColor(ubID, bkColor);
		#else
		AHC_OSDSetColor(bkColor);
		AHC_OSDSetBkColor(bkColor);
	    #endif
		AHC_OSDDrawFillRect(ubID, x, y, x1, y1);
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	    AHC_OSDSetColor(ubID, IconColor);
		#else
	    AHC_OSDSetColor(IconColor);
	    #endif
	} 
	else
	{
	    if(get_NetAppStatus()!=0)
	    {
	        if(AHC_Get_WiFi_Streaming_Status())
	            pbmp = &BMICON_WIFI_STREAMING_ON;
	        else
	            pbmp = &BMICON_WIFI_STREAMING_OFF;
		  
	        if(bLarge==LARGE_SIZE)
	            AHC_OSDDrawBitmapMag(ubID, pbmp, x, y, ubMag, ubMag);
	        else
	            AHC_OSDDrawBitmap(ubID, pbmp, x, y);

			
	       OSDDraw_MainTouchButton(ubID, (VideoFunc_RecordStatus())?
									UI_VIDEO_RECORD_STATE : UI_VIDEO_STATE);

	    }
    	if (netapp_GetWiFiMode() == 2 /*STA MODE*/) {
    		#define	N(p, m) (*(((unsigned char*)(&p)) + m))
	    	unsigned int	ip;
    		UINT8	szip[16];
    		GUI_RECT	rc;
			
	    	ip = netapp_GetIPAddress();
    		snprintf((char*)szip, 16, "%u.%u.%u.%u", N(ip, 0), N(ip, 1), N(ip,2), N(ip, 3));
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
			AHC_OSDSetColor(ubID, OSD_COLOR_GREEN);
			AHC_OSDSetBkColor(ubID, bkColor);
			AHC_OSDSetPenSize(ubID, 1);
			AHC_OSDSetFont(ubID, &GUI_Font8_1);
			#else
			AHC_OSDSetColor(OSD_COLOR_GREEN);
			AHC_OSDSetBkColor(bkColor);
			AHC_OSDSetPenSize(1);
			AHC_OSDSetFont(&GUI_Font8_1);
		    #endif
		    rc.x0 = 0;
		    rc.x1 = RTNA_LCD_GetAttr()->usPanelW - 4;
		    rc.y0 = y - 10;
		    rc.y1 = y;
		    AHC_OSDDrawTextInRect(ubID, (UINT8*)szip, &rc, GUI_TA_RIGHT|GUI_TA_VCENTER);
    	}
	}

	END_LAYER(ubID);
}

#endif


void DrawRearCamReverseLine(AHC_BOOL bFlag)
{
    UINT8 bID0 = 0, bID1 = 0;
    const GUI_BITMAP*	pbmp;

     if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    OSDDraw_EnterDrawing(&bID0, &bID1);
    //OSDDraw_ClearOvlDrawBuffer(bID0);
    
    BEGIN_LAYER(bID0);
    AHC_OSDSetColor(bID0, 0xAAFF80FF);
    AHC_OSDSetBkColor(bID0, 0xAAFF80FF);
    if(bFlag == AHC_FALSE)
    {
	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	AHC_OSDSetColor(bID0, OSD_COLOR_TRANSPARENT);
	AHC_OSDSetBkColor(bID0, OSD_COLOR_TRANSPARENT);
	#else
	AHC_OSDSetColor(OSD_COLOR_BACKGROUND);
	AHC_OSDSetBkColor(OSD_COLOR_BACKGROUND);
        #endif
	AHC_OSDDrawFillRect(bID0, 120, 80, m_OSD[bID0]->width, m_OSD[bID0]->height-128);
	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
            AHC_OSDSetColor(bID0, 0xAA000000); 
	#else
            AHC_OSDSetColor(0xAA000000);
        #endif
    }
    else
    {
        pbmp = &BMPICON_Reverse_LINE;
    	AHC_OSDDrawBitmap(bID0, pbmp, 120, 80);
    }

    END_LAYER(bID0);
    OSDDraw_ExitDrawing(&bID0, &bID1);
}

#if 0
void ____Event_Handler____(){ruturn;} //dummy
#endif

void DrawStateVideoRecUpdate(UINT32 ubEvent)
{
    switch(ubEvent)
    {
        case EVENT_NONE                           :  
        break;
        
        case EVENT_KEY_MODE                       :  
        case EVENT_KEY_MENU                       : 
        case EVENT_KEY_PLAYBACK_MODE              :
            DrawVideoRecInfo(AHC_TRUE);
        break;
        
        case EVENT_KEY_TELE_PRESS                 : 
        case EVENT_KEY_WIDE_PRESS                 : 
        case EVENT_KEY_WIDE_STOP                  : 
        case EVENT_KEY_TELE_STOP                  : 
        case EVENT_KEY_ZOOM_STATUS_CLEAR          : 
            DrawVideoRecStateZoom();
        break;
                
        case EVENT_KEY_SET						  :
            #if (HDMI_PREVIEW_EN || TVOUT_PREVIEW_EN)
        	if(TVFunc_CheckTVStatus(AHC_TV_VIDEO_PREVIEW_STATUS))
        	{
         		UpdateVideoRecordStatus(VIDEO_REC_PAUSE);
				UIDrawTV_VideoFuncEx(TVFunc_GetUImodeOsdID(), TV_GUI_VIDEO_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
         	}
         	else if(HDMIFunc_CheckHdmiStatus(AHC_HDMI_VIDEO_PREVIEW_STATUS))
        	{
         		UpdateVideoRecordStatus(VIDEO_REC_PAUSE);
				UIDrawHdmi_VideoFuncEx(HDMIFunc_GetUImodeOsdID(), TV_GUI_VIDEO_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
         	}
			else
			#endif
			{
				DrawVideoRecStateRecord(VIDEO_REC_PAUSE);
        	}
        break;
               
        case EVENT_LCD_COVER_NORMAL               :
        case EVENT_LCD_COVER_ROTATE               :        
			AHC_OSDSetActive(OVL_DISPLAY_BUFFER, 0);
        	 
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
			AHC_OSDSetFlipDrawMode(OVL_DISPLAY_BUFFER, OSD_FLIP_DRAW_NONE_ENABLE);
			#else
			AHC_OSDSetFlipDrawMode(OSD_FLIP_DRAW_NONE_ENABLE);
			#endif
			DrawStateVideoRecInit();

			if(VideoFunc_RecordStatus()) 
			{
				UINT32 	CurSysMode;
			
				AHC_GetSystemStatus(&CurSysMode);
			
				if( (CurSysMode == ((AHC_MODE_VIDEO_RECORD<<16)|AHC_SYS_VIDRECD_PAUSE)) ||
				    (CurSysMode == ((AHC_MODE_RECORD_VONLY<<16)|AHC_SYS_VIDRECD_PAUSE)) )
        	 		DrawVideoRecStateRecord(VIDEO_REC_PAUSE);
        	 	else
        	 		DrawVideoRecStateRecord(VIDEO_REC_START);
			}

	       	AHC_OSDSetActive(OVL_DISPLAY_BUFFER, 1);
	       	DrawVideoRecInfo(AHC_FALSE);
        break;
                        
        case EVENT_VIDEO_KEY_RECORD               :
        	#if (HDMI_PREVIEW_EN || TVOUT_PREVIEW_EN) 
        	if(TVFunc_CheckTVStatus(AHC_TV_VIDEO_PREVIEW_STATUS))
        	{
        	        if(VideoFunc_RecordStatus())
         				UpdateVideoRecordStatus(VIDEO_REC_START);
                    else
                        UpdateVideoRecordStatus(VIDEO_REC_STOP);
				UIDrawTV_VideoFuncEx(TVFunc_GetUImodeOsdID(), TV_GUI_VIDEO_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
         	}
         	else if(HDMIFunc_CheckHdmiStatus(AHC_HDMI_VIDEO_PREVIEW_STATUS))
        	{
        	        if(VideoFunc_RecordStatus())
         				UpdateVideoRecordStatus(VIDEO_REC_START);
                    else
                        UpdateVideoRecordStatus(VIDEO_REC_STOP);
				UIDrawHdmi_VideoFuncEx(HDMIFunc_GetUImodeOsdID(), TV_GUI_VIDEO_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
         	}
			else
			#endif
			{        
				DrawVideoRecStateRecord(VIDEO_REC_START);
        	}
        break;
        
        case EVENT_VIDEO_KEY_RECORD_STOP          :
        	#if (TVOUT_PREVIEW_EN)
        	if(TVFunc_CheckTVStatus(AHC_TV_VIDEO_PREVIEW_STATUS))
        	{
         		UpdateVideoRecordStatus(VIDEO_REC_STOP);
				UIDrawTV_VideoFuncEx(TVFunc_GetUImodeOsdID(), TV_GUI_VIDEO_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
         	}
         	else
         	#endif
         	#if (HDMI_PREVIEW_EN)
         	if(HDMIFunc_CheckHdmiStatus(AHC_HDMI_VIDEO_PREVIEW_STATUS))
        	{
         		UpdateVideoRecordStatus(VIDEO_REC_STOP);
				UIDrawHdmi_VideoFuncEx(HDMIFunc_GetUImodeOsdID(), HDMI_GUI_VIDEO_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
         	}
			else
			#endif
			{
				DrawVideoRecStateRecord(VIDEO_REC_STOP);
        	}
        break;
                
        case EVENT_DSC_SET_EV   : 
			DrawCaptureStateEV();
        break;
        
        case EVENT_VIDEO_KEY_CAPTURE:
            #if (HDMI_PREVIEW_EN || TVOUT_PREVIEW_EN) 
        	if(TVFunc_CheckTVStatus(AHC_TV_VIDEO_PREVIEW_STATUS))
        	{
         		UpdateVideoRecordStatus(VIDEO_REC_CAPTURE);
		UIDrawTV_VideoFuncEx(TVFunc_GetUImodeOsdID(), TV_GUI_VIDEO_REC_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
         	}
          else
            #endif
                {
			DrawVideoRecStateRecord(VIDEO_REC_CAPTURE);
                }
        break;
        
        case EVENT_VIDREC_UPDATE_MESSAGE          : 
			DrawVideoRecStateUpdate();
        break;
        
        default:    
        break;
	}
}

void DrawVideoParkingMode( AHC_BOOL bEnable )
{
    UINT8  bID0 = 0;
    UINT8  bID1 = 0;
    RECT   rtIconPosition = { 10, 4, 60, 20 };
    RECT   tvIconPosition = { 30 ,10, 150, 28 };

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    OSDDraw_EnterDrawing(&bID0, &bID1);

    BEGIN_LAYER(bID0);

    if( bEnable == AHC_TRUE )
    {
        if(TVFunc_IsInTVMode())
		{
            #ifdef ENABLE_GUI_SUPPORT_MULTITASK
            AHC_OSDSetFont(bID0, &GUI_Font24B_ASCII);
            #else
            AHC_OSDSetFont(&GUI_Font24B_ASCII);
            #endif
            OSD_ShowString(bID0, (char*)"Parking", NULL, tvIconPosition, OSD_COLOR_WHITE, OSD_COLOR_TRANSPARENT, GUI_TA_LEFT|GUI_TA_VCENTER);
		}
        else
        {
            #ifdef ENABLE_GUI_SUPPORT_MULTITASK
            AHC_OSDSetFont(bID0, &GUI_Font16B_1);
            #else
            AHC_OSDSetFont(&GUI_Font16B_1);
            #endif
            OSD_ShowString(bID0, (char*)"Parking", NULL, rtIconPosition, OSD_COLOR_WHITE, OSD_COLOR_TRANSPARENT, GUI_TA_LEFT|GUI_TA_VCENTER);
        }
    }
    else
    {
        if(TVFunc_IsInTVMode())
		{
            #ifdef ENABLE_GUI_SUPPORT_MULTITASK
            AHC_OSDSetColor( bID0, OSD_COLOR_TRANSPARENT );
        	AHC_OSDSetBkColor( bID0, OSD_COLOR_TRANSPARENT );
            #else
            AHC_OSDSetColor( OSD_COLOR_TRANSPARENT );
        	AHC_OSDSetBkColor( OSD_COLOR_TRANSPARENT );
            #endif
            AHC_OSDDrawFillRect(bID0, tvIconPosition.uiLeft, tvIconPosition.uiTop, tvIconPosition.uiLeft+tvIconPosition.uiWidth, tvIconPosition.uiTop+tvIconPosition.uiHeight);	
      	}
        else
        {
            #ifdef ENABLE_GUI_SUPPORT_MULTITASK
            AHC_OSDSetColor( bID0, OSD_COLOR_TRANSPARENT );
        	AHC_OSDSetBkColor( bID0, OSD_COLOR_TRANSPARENT );
            #else
            AHC_OSDSetColor( OSD_COLOR_TRANSPARENT );
        	AHC_OSDSetBkColor( OSD_COLOR_TRANSPARENT );
            #endif
            AHC_OSDDrawFillRect(bID0, rtIconPosition.uiLeft, rtIconPosition.uiTop, rtIconPosition.uiLeft+rtIconPosition.uiWidth, rtIconPosition.uiTop+rtIconPosition.uiHeight);
        }
    }
    
    END_LAYER(bID0);
    
    OSDDraw_ExitDrawing(&bID0, &bID1);
}


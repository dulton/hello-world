/*===========================================================================
 * Include file
 *===========================================================================*/

#include "AHC_GUI.h"
#include "AHC_OS.h"
#include "AHC_DCF.h"
#include "AHC_UF.h"
#include "AHC_Browser.h"
#include "AHC_Menu.h"
#include "AHC_Utility.h"
#include "AHC_Display.h"
#include "AHC_General_CarDV.h"
#include "MenuCommon.h"
#include "MenuDrawCommon.h"
#include "MenuTouchButton.h"
#include "KeyParser.h"
#include "MenuSetting.h"
#include "DrawStateMenuSetting.h"
#include "DrawStateMoviePBFunc.h"
#include "ColorDefine.h"
#include "IconPosition.h"
#include "IconDefine.h"
#include "OsdStrings.h"
#include "dsc_Charger.h"
#include "MediaPlaybackCtrl.h"
#include "UI_DrawGeneral.h"

/*===========================================================================
 * Global varible
 *===========================================================================*/

static UINT32 	tCurrentPlayTime 	= 0;

/*===========================================================================
 * Extern varible
 *===========================================================================*/

extern UINT8 	m_ubBatteryStatus;
extern AHC_BOOL	bLockCurMovFile;
extern UINT32	m_MovPlayFFSpeed;
extern UINT32	m_MovPlayREWSpeed;
extern UINT8 	bMovieState;
extern UINT8 	bSlideMovieState;
extern UINT8 	bTVMovieState;
extern UINT8 	bHdmiMovieState;
extern UINT32 	m_VideoCurPlayTime;
extern AHC_BOOL	bMovPbEnterToHdmi;
extern AHC_BOOL	bMovPbEnterToTV;
extern AHC_BOOL m_ubPlaybackRearCam;
extern AHC_BOOL gbAhcDbgBrk;
/*===========================================================================
 * Main body
 *===========================================================================*/

void UpdateMovPBStatus(UINT8 ubStatus)
{
	bMovieState = ubStatus;
}

void DrawMovPb_Status(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    GUI_BITMAP 	IconDir			= bmIcon_HdmiVideoPlayStop;
    GUI_BITMAP 	IconState 		= bmIcon_HdmiVideoPlay_Pause;
    AHC_BOOL	bClearTrashCan 	= AHC_FALSE;
    UINT8		CurState 		= MOVIE_STATE_PLAY;
    UINT32		OffX, OffY;
    AHC_BOOL	IsHdmiTvConnect	= AHC_FALSE;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);

 	if(bFlag==AHC_FALSE)
 	{
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    	AHC_OSDSetColor(ubID, bkColor);
    	AHC_OSDSetBkColor(ubID, bkColor);
		#else
    	AHC_OSDSetColor(bkColor);
    	AHC_OSDSetBkColor(bkColor);
	    #endif

    	if(bLarge==LARGE_SIZE)
    		AHC_OSDDrawFillRect(ubID, x, y, x+80, y+35);
    	else
    		AHC_OSDDrawFillRect(ubID, x, y, x+50, y+35);
    }
    else
    {
#if HDMI_ENABLE
    	if(AHC_IsHdmiConnect())
    	{
    		CurState = bHdmiMovieState;
			IsHdmiTvConnect = AHC_TRUE;
    	}
    	else
#endif
#if TVOUT_ENABLE
    	if(AHC_IsTVConnectEx())
    	{
    		CurState = bTVMovieState;
			IsHdmiTvConnect = AHC_TRUE;
    	}
    	else
#endif
    	{
			IsHdmiTvConnect = AHC_FALSE;

    		if(uiGetCurrentState()==UI_SLIDESHOW_STATE)
    			CurState = bSlideMovieState;
    		else
				CurState = bMovieState;
		}

    	if(bLarge==LARGE_SIZE)
    	{
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    		AHC_OSDSetFont(ubID, &GUI_Font24B_ASCII);
			#else
    		AHC_OSDSetFont(&GUI_Font24B_ASCII);
		    #endif
    		OffX = 25;
    		OffY = 8;
    	}
    	else
    	{
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    		AHC_OSDSetFont(ubID, &GUI_Font16B_ASCII);
			#else
    		AHC_OSDSetFont(&GUI_Font16B_ASCII);
		    #endif
    		OffX = 7;
    		OffY = 2;
    	}

	    switch(CurState)
	    {
			case MOVIE_STATE_STOP :
				IconDir   = bmIcon_HdmiVideoPlayStop;
				IconState = bmIcon_HdmiVideoPlay_Stop;
				bClearTrashCan = AHC_TRUE;
			break;

	    	case MOVIE_STATE_PLAY:
				IconDir   = bmIcon_HdmiVideoPlay;
				IconState = bmIcon_HdmiVideoPlay_Play;
				bClearTrashCan = AHC_FALSE;
	    	break;

	    	case MOVIE_STATE_PAUSE:
				IconDir   = bmIcon_HdmiVideoPlayPause;
				IconState = bmIcon_HdmiVideoPlay_Pause;
				bClearTrashCan = AHC_TRUE;
	    	break;

	    	case MOVIE_STATE_FF:
				IconDir   = bmIcon_HdmiVideoPlay;
				IconState = bmIcon_HdmiVideoPlay_FF;
				bClearTrashCan = AHC_FALSE;

				#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	        	AHC_OSDSetColor(ubID, OSD_COLOR_WHITE);
				#else
	        	AHC_OSDSetColor(OSD_COLOR_WHITE);
			    #endif
	    		AHC_OSDDrawDec(ubID, m_MovPlayFFSpeed, x+IconState.XSize+OffX, y+OffY, 1);
	    		AHC_OSDDrawText(ubID, (UINT8*)"X", x+IconState.XSize+OffX+10, y+OffY, strlen("X"));
	    	break;

	    	case MOVIE_STATE_REW:
				IconDir   = bmIcon_HdmiVideoPlay;
				IconState = bmIcon_HdmiVideoPlay_FB;
				bClearTrashCan = AHC_FALSE;

				#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	        	AHC_OSDSetColor(ubID, OSD_COLOR_WHITE);
				#else
	        	AHC_OSDSetColor(OSD_COLOR_WHITE);
			    #endif
	    		AHC_OSDDrawDec(ubID, m_MovPlayREWSpeed, x+IconState.XSize+OffX, y+OffY, 1);
	    		AHC_OSDDrawText(ubID, (UINT8*)"X", x+IconState.XSize+OffX+10, y+OffY, strlen("X"));
	    	break;

			case MOVIE_STATE_PFF   :
				IconDir   = bmIcon_HdmiVideoPlayPause;
				IconState = bmIcon_HdmiVideoPlay_PFF;
				bClearTrashCan = AHC_TRUE;
			break;

			case MOVIE_STATE_PFB   :
				IconDir   = bmIcon_HdmiVideoPlayPause;
				IconState = bmIcon_HdmiVideoPlay_PFB;
				bClearTrashCan = AHC_TRUE;
			break;

	   		default:
	   		break;
	    }

	    if(bLarge==LARGE_SIZE)
	    {
			#if 0//(SHOW_TV_DIR_ICON)
			AHC_OSDDrawBitmap(ubID, &IconDir, x+60, y);

			if(bClearTrashCan)
				DrawClearTrashCan(ubID);
			#endif

			AHC_OSDDrawBitmapMag(ubID, &IconState, x, y, ubMag, ubMag);
	    }
	    else
	    {
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
			AHC_OSDSetColor(ubID, 0xAA000000);
			#else
			AHC_OSDSetColor(0xAA000000);
		    #endif
			AHC_OSDDrawBitmap(ubID, &IconState, x, y);
	    }
    }

	END_LAYER(ubID);
}

void DrawMovPb_FileIndex(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    UINT32  CurObjIdx 	= 0;
    UINT32  MaxObjIdx 	= 0;
    UINT8   nDigitM	    = 0, nDigitC = 0;
    UINT16  leftpos 	= x;
    UINT16  WordWidth 	= 7;
    UINT16  CharWidth 	= 5;
    
    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    AHC_UF_GetTotalFileCount(&MaxObjIdx);
    AHC_UF_GetCurrentIndex(&CurObjIdx);

    nDigitM = NumOfDigit(MaxObjIdx);
    nDigitC = NumOfDigit(CurObjIdx+1);

    BEGIN_LAYER(ubID);

    if(bLarge==LARGE_SIZE)
    {
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetFont(ubID, &GUI_Font24B_ASCII);
		#else
        AHC_OSDSetFont(&GUI_Font24B_ASCII);
	    #endif
        WordWidth = 14;
        CharWidth = 9;
    }
    else
    {
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetFont(ubID, &GUI_Font16B_ASCII);
		#else
        AHC_OSDSetFont(&GUI_Font16B_ASCII);
	    #endif
        WordWidth = 7;
        CharWidth = 5;
	}

	if(bFlag==AHC_FALSE)
	{
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, bkColor);
		#else
        AHC_OSDSetColor(bkColor);
	    #endif
		AHC_OSDDrawFillRect(ubID, x, y, x+WordWidth*(nDigitM+nDigitC+2), y+WordWidth+8);
    }
    else
    {
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	    AHC_OSDSetColor(ubID, OSD_COLOR_WHITE);
        AHC_OSDSetBkColor(ubID, bkColor);
        AHC_OSDSetPenSize(ubID, 3);
		#else
	    AHC_OSDSetColor(OSD_COLOR_WHITE);
        AHC_OSDSetBkColor(bkColor);
        AHC_OSDSetPenSize(3);
	    #endif

		leftpos = x;
        AHC_OSDDrawDec(ubID, CurObjIdx+1, leftpos, y, nDigitC);

        leftpos += nDigitC*WordWidth;
        AHC_OSDDrawText(ubID,(UINT8*)"/", leftpos , y, strlen("/"));

        leftpos += CharWidth;
        AHC_OSDDrawDec(ubID, MaxObjIdx,   leftpos, y, nDigitM);
    }

	END_LAYER(ubID);
}

void DrawMovPb_FileInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    UINT32   	CurObjIdx;
    INT8         FileName[MAX_FILE_NAME_SIZE];

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }
    
    BEGIN_LAYER(ubID);

    if(bFlag==AHC_TRUE)
    {
        AHC_UF_GetCurrentIndex(&CurObjIdx);
        if( m_ubPlaybackRearCam == AHC_TRUE && VIDRECD_MULTI_TRACK == 0 ){
            AHC_UF_SetRearCamPathFlag(AHC_TRUE);
        }
		
        AHC_UF_GetFileNamebyIndex(CurObjIdx,FileName);
        if( m_ubPlaybackRearCam == AHC_TRUE && VIDRECD_MULTI_TRACK == 0 ){
            AHC_UF_SetRearCamPathFlag(AHC_FALSE);
        }
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, OSD_COLOR_WHITE);
        AHC_OSDSetBkColor(ubID, bkColor);
		#else
        AHC_OSDSetColor(OSD_COLOR_WHITE);
        AHC_OSDSetBkColor(bkColor);
	    #endif

        if(bLarge==LARGE_SIZE)
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        	AHC_OSDSetFont(ubID, &GUI_Font24B_ASCII);
			#else
        	AHC_OSDSetFont(&GUI_Font24B_ASCII);
		    #endif
        else {
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        	AHC_OSDSetFont(ubID, &GUI_Font16B_ASCII);
			#else
        	AHC_OSDSetFont(&GUI_Font16B_ASCII);
		    #endif
       	}

        AHC_OSDDrawText(ubID, (UINT8*)FileName, x, y, strlen(FileName));
    }

	END_LAYER(ubID);
}

void DrawMovPb_DateInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    UINT32      CurObjIdx;
    UINT16      leftpos 	= x;
    UINT16      WordWidth	= 7;
    UINT16      CharWidth	= 5;
    AHC_RTC_TIME RtcTime;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }
    
    BEGIN_LAYER(ubID);

  	if(bFlag==AHC_TRUE)
  	{
	    AHC_UF_GetCurrentIndex(&CurObjIdx);
        AHC_UF_GetFileTimebyIndex(CurObjIdx, &RtcTime);
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	    AHC_OSDSetColor(ubID, OSD_COLOR_WHITE);
	    AHC_OSDSetBkColor(ubID, bkColor);
	    AHC_OSDSetPenSize(ubID, 3);
		#else
	    AHC_OSDSetColor(OSD_COLOR_WHITE);
	    AHC_OSDSetBkColor(bkColor);
	    AHC_OSDSetPenSize(3);
	    #endif

	    if(bLarge==LARGE_SIZE)
	    {
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	        AHC_OSDSetFont(ubID, &GUI_Font24B_ASCII);
			#else
	        AHC_OSDSetFont(&GUI_Font24B_ASCII);
		    #endif
	        WordWidth = 14;
	        CharWidth = 9;
	    }
	    else
	    {
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	        AHC_OSDSetFont(ubID, &GUI_Font16B_ASCII);
			#else
	        AHC_OSDSetFont(&GUI_Font16B_ASCII);
		    #endif
	        WordWidth = 7;
	        CharWidth = 5;
		}

	    AHC_OSDDrawDec(ubID, RtcTime.uwYear, leftpos, y, 4);

	    leftpos += WordWidth*4;
	    AHC_OSDDrawText(ubID,(UINT8*)"/", leftpos, y, strlen("/"));

	    leftpos += CharWidth;
	    AHC_OSDDrawDec(ubID, RtcTime.uwMonth, leftpos, y, 2);

	    leftpos += WordWidth*2;
	    AHC_OSDDrawText(ubID,(UINT8*)"/", leftpos, y, strlen("/"));

	    leftpos += CharWidth;
	    AHC_OSDDrawDec(ubID, RtcTime.uwDay, leftpos, y, 2);

	    leftpos += WordWidth*2;
	    AHC_OSDDrawText(ubID,(UINT8*)" ", leftpos, y, strlen(" "));

	    leftpos += CharWidth;
	    AHC_OSDDrawDec(ubID, RtcTime.uwHour, leftpos, y, 2);

	    leftpos += WordWidth*2;
	    AHC_OSDDrawText(ubID,(UINT8*)":", leftpos, y, strlen(":"));

	    leftpos += CharWidth;
	    AHC_OSDDrawDec(ubID, RtcTime.uwMinute, leftpos, y, 2);
    }

	END_LAYER(ubID);
}

void DrawMovPb_PlayInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    UINT32 	ulCurPlayTime;
    UINT16  leftpos 	= x;
    UINT8   nDigit 		= 2;
    UINT16 	WordWidth 	= 7;
    UINT8  	CharWidth 	= 5;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);

    if(bFlag==AHC_TRUE)
    {
		if(bLarge==LARGE_SIZE)
		{
			WordWidth = 14;
			CharWidth = 9;
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
			AHC_OSDSetFont(ubID, &GUI_Font24B_ASCII);
			#else
			AHC_OSDSetFont(&GUI_Font24B_ASCII);
		    #endif
		}
		else
		{
			WordWidth = 7;
			CharWidth = 5;
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
			AHC_OSDSetFont(ubID, &GUI_Font16B_ASCII);
			#else
			AHC_OSDSetFont(&GUI_Font16B_ASCII);
		    #endif
		}

        AHC_GetVideoCurrentPlayTime(&ulCurPlayTime);

		if(tCurrentPlayTime != ulCurPlayTime)
        {
            tCurrentPlayTime = ulCurPlayTime;
        }
        else if(GetMovConfig()->iState == MOV_STATE_PAUSE)
        {
        	if(bMovPbEnterToHdmi || bMovPbEnterToTV)
        		tCurrentPlayTime = m_VideoCurPlayTime;
        }
        else {
			END_LAYER(ubID);
			return;
		}

		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, OSD_COLOR_WHITE);
        AHC_OSDSetBkColor(ubID, bkColor);
        AHC_OSDSetPenSize(ubID, 3);
		#else
        AHC_OSDSetColor(OSD_COLOR_WHITE);
        AHC_OSDSetBkColor(bkColor);
        AHC_OSDSetPenSize(3);
	    #endif

        AHC_OSDDrawDec(ubID, TIME_HOUR(tCurrentPlayTime), leftpos, y, nDigit);
        leftpos += nDigit*WordWidth;
        AHC_OSDDrawText(ubID,(UINT8*)":", leftpos ,y, strlen(":"));
        leftpos += CharWidth;
        AHC_OSDDrawDec(ubID, TIME_MIN(tCurrentPlayTime),  leftpos, y, nDigit);
        leftpos += nDigit*WordWidth;
        AHC_OSDDrawText(ubID,(UINT8*)":", leftpos ,y, strlen(":"));
        leftpos += CharWidth;
        AHC_OSDDrawDec(ubID, TIME_SEC(tCurrentPlayTime),  leftpos, y, nDigit);
    }

	END_LAYER(ubID);
}

void DrawMovPb_TotalTime(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    AHC_VIDEO_ATTR  Info;
    UINT16  leftpos 	= x;
    UINT8   nDigit 		= 2;
    UINT16 	WordWidth 	= 7;
    UINT8  	CharWidth 	= 5;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);

    if(bFlag==AHC_TRUE)
    {
		if(bLarge==LARGE_SIZE)
		{
			WordWidth = 14;
			CharWidth = 9;
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
			AHC_OSDSetFont(ubID, &GUI_Font24B_ASCII);
			#else
			AHC_OSDSetFont(&GUI_Font24B_ASCII);
		    #endif
		}
		else
		{
			WordWidth = 7;
			CharWidth = 5;
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
			AHC_OSDSetFont(ubID, &GUI_Font16B_ASCII);
			#else
			AHC_OSDSetFont(&GUI_Font16B_ASCII);
		    #endif
		}

        AHC_GetVideoFileAttr(&Info);

		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, OSD_COLOR_WHITE);
        AHC_OSDSetBkColor(ubID, bkColor);
        AHC_OSDSetPenSize(ubID, 3);
		#else
        AHC_OSDSetColor(OSD_COLOR_WHITE);
        AHC_OSDSetBkColor(bkColor);
        AHC_OSDSetPenSize(3);
	    #endif

		AHC_OSDDrawText(ubID,(UINT8*)"/", leftpos ,y, strlen("/"));
        leftpos += CharWidth;
        AHC_OSDDrawDec(ubID, TIME_HOUR(Info.TotalTime), leftpos, y, nDigit);
        leftpos += nDigit*WordWidth;
        AHC_OSDDrawText(ubID,(UINT8*)":", leftpos ,y, strlen(":"));
        leftpos += CharWidth;
        AHC_OSDDrawDec(ubID, TIME_MIN(Info.TotalTime),  leftpos, y, nDigit);
        leftpos += nDigit*WordWidth;
        AHC_OSDDrawText(ubID,(UINT8*)":", leftpos ,y, strlen(":"));
        leftpos += CharWidth;
        AHC_OSDDrawDec(ubID, TIME_SEC(Info.TotalTime),  leftpos, y, nDigit);
    }

	END_LAYER(ubID);
}

void DrawMovPB_VolumeBar(AHC_BOOL bClear)
{
    #define OffsetH (26)

    static UINT16 uwVolIdx = 0;
    UINT8 		i;
    RECT 		rc = {89 , 160+OffsetH , 10, 12};
    UINT16 		Left_temp;
    GUI_COLOR 	FillColor;
    UINT8 		ubID = MAIN_DISPLAY_BUFFER;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);
    AHC_OSDSetActive(ubID, 1);

    if(bClear)
    {
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, OSD_COLOR_TRANSPARENT);
		#else
        AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
	    #endif
        AHC_OSDDrawFillRect(ubID, 65, 155+OffsetH, 245, 190+OffsetH);
        AHC_OSDSetActive(ubID, 0);
		END_LAYER(ubID);

        return;
    }

    uwVolIdx = MenuSettingConfig()->uiVolume;

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(ubID, OSD_COLOR_WHITE);
    AHC_OSDSetBkColor(ubID, OSD_COLOR_TRANSPARENT);
    AHC_OSDSetFont(ubID, &GUI_Font16B_1);
	#else
    AHC_OSDSetColor(OSD_COLOR_WHITE);
    AHC_OSDSetBkColor(OSD_COLOR_TRANSPARENT);
    AHC_OSDSetFont(&GUI_Font16B_1);
    #endif

    AHC_OSDDrawText(ubID, (UINT8*)"_", 76,  150+OffsetH, 1);
    AHC_OSDDrawText(ubID, (UINT8*)"+", 232, 156+OffsetH, 1);

    for(i=0; i<10; i++)
    {
        Left_temp = rc.uiLeft + ( rc.uiWidth + 4 ) * i;

        if(i < uwVolIdx)
            FillColor = OSD_COLOR_GREEN;
        else
            FillColor = 0xFF111111;

		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, OSD_COLOR_BLACK);
		#else
        AHC_OSDSetColor(OSD_COLOR_BLACK);
	    #endif
        AHC_OSDDrawRect(ubID, Left_temp, rc.uiTop, Left_temp+rc.uiWidth, rc.uiTop+rc.uiHeight);

		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, FillColor);
		#else
        AHC_OSDSetColor(FillColor);
	    #endif
        AHC_OSDDrawFillRect(ubID, Left_temp+1, rc.uiTop+1, Left_temp+rc.uiWidth-1, rc.uiTop+rc.uiHeight-1);
    }

	END_LAYER(ubID);
}

void DrawMovPb_LockCurFile(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
#if (QUICK_FILE_EDIT_EN)
    GUI_BITMAP 	IconID 	= BMICOM_FILE_PROTECTKEY;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);

   	if(bFlag==AHC_FALSE)
   	{
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
		AHC_OSDSetColor(ubID, bkColor);
		#else
		AHC_OSDSetColor(bkColor);
	    #endif
    	AHC_OSDDrawFillRect(ubID, x, y, x+IconID.XSize*ubMag, y+IconID.YSize*ubMag);
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    	AHC_OSDSetColor(ubID, 0xAA000000);
		#else
    	AHC_OSDSetColor(0xAA000000);
	    #endif
   	}
   	else
   	{
   		if(bLockCurMovFile)
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

void DrawStateMovPbState(void)
{
    UINT8  bID0 = 0;

    OSDDraw_GetLastOvlDrawBuffer(&bID0);

    UIDrawMovPBFuncEx(bID0, MOVPB_GUI_STATUS, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
}

void DrawStateMovPbStateWithTouchButton(void)
{
#if (SUPPORT_TOUCH_PANEL)//disable temporally, wait touch UI

    UINT8  bID0 = 0;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    OSDDraw_GetLastOvlDrawBuffer(&bID0);

    UIDrawMovPBFuncEx(bID0, MOVPB_GUI_STATUS, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);

    BEGIN_LAYER(bID0);

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, OSD_COLOR_TRANSPARENT);
#else
    AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
#endif
    AHC_OSDDrawFillRect(bID0, 90, 50, AHC_GET_ATTR_OSD_W(bID0), 190);

    OSDDraw_CtrlTouchButton(bID0, UI_PLAYBACK_STATE);

#ifdef FLM_GPIO_NUM
    AHC_OSDRefresh_PLCD();
#endif

    END_LAYER(bID0);

#endif
}

void DrawStateMovPbPageSwitch(UINT8 ubPage)
{
#if (SUPPORT_TOUCH_PANEL)//disable temporally, wait touch UI
    UINT8  bID0 = 0;

	OSDDraw_GetLastOvlDrawBuffer(&bID0);

    OSDDraw_ClearTouchButtons(bID0, MovPBCtrlPage_TouchButton, ITEMNUM(MovPBCtrlPage_TouchButton));

    if( TOUCH_MAIN_PAGE == ubPage )
        OSDDraw_MainTouchButton(bID0, UI_PLAYBACK_STATE);
	else
        OSDDraw_CtrlTouchButton(bID0, UI_PLAYBACK_STATE);
#endif
}

void DrawStateMovPbInit(void)
{
    UINT8  bID0 = 0;
    UINT8  bID1 = 0;
    UINT8  idx 	= 0;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    CHARGE_ICON_ENABLE(AHC_FALSE);

    OSDDraw_GetLastOvlDrawBuffer(&bID0);

#ifdef FLM_GPIO_NUM
    AHC_OSDSetActive(bID0, AHC_FALSE);
#endif

    OSDDraw_ClearOvlDrawBuffer(bID0);

    BEGIN_LAYER(bID0);

    OSDDraw_EnableSemiTransparent( 1, 	AHC_TRUE );
    OSDDraw_EnableSemiTransparent( bID0, AHC_TRUE );
    OSDDraw_EnableSemiTransparent( bID1, AHC_TRUE );

    AHC_OSDSetCurrentDisplay(bID0);

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, 0xFF000000);
#else
    AHC_OSDSetColor(0xFF000000);
#endif

    END_LAYER(bID0);

    for (idx = 0; idx < MOVPB_GUI_MAX; idx++)
    {
        if(idx==MOVPB_GUI_CUR_TIME)
        {
            if(bMovieState != MOVIE_STATE_PLAY)
                UIDrawMovPBFuncEx(bID0, idx, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
        }
        else
            UIDrawMovPBFuncEx(bID0, idx, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    }

    OSDDraw_MainTouchButton(bID0, UI_PLAYBACK_STATE);

#ifdef FLM_GPIO_NUM
    AHC_OSDSetActive(bID0, AHC_TRUE);
    AHC_OSDRefresh_PLCD();
#endif

    CHARGE_ICON_ENABLE(AHC_TRUE);
}

void DrawStateMovPbChangeFile(void)
{
    UINT8  bID0 = 0;

    OSDDraw_GetLastOvlDrawBuffer(&bID0);

	UIDrawMovPBFuncEx(bID0, MOVPB_GUI_FILENAME,   AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
	UIDrawMovPBFuncEx(bID0, MOVPB_GUI_FILE_INDEX, AHC_TRUE,  AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    UIDrawMovPBFuncEx(bID0, MOVPB_GUI_TOTAL_TIME, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);

    #ifdef FLM_GPIO_NUM
    AHC_OSDRefresh_PLCD();
    #endif
}

void DrawStateMovPbUpdate(void)
{
    UINT8    bID0 = 0;
	static UINT8 	SD_status = AHC_FALSE;
#if (QUICK_FILE_EDIT_EN)
    static UINT8 	Lock_status = AHC_FALSE;
#endif

    OSDDraw_GetLastOvlDrawBuffer(&bID0);

    if(m_ubBatteryStatus != MenuSettingConfig()->uiBatteryVoltage)
	{
   		UIDrawMovPBFuncEx(bID0, MOVPB_GUI_BATTERY, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    	m_ubBatteryStatus = MenuSettingConfig()->uiBatteryVoltage;
    }

    if(SD_status != AHC_SDMMC_GetMountState())
    {
    	UIDrawMovPBFuncEx(bID0, MOVPB_GUI_SD_STATE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
        SD_status = AHC_SDMMC_GetMountState();
    }

#if (QUICK_FILE_EDIT_EN)
 	if(Lock_status != bLockCurMovFile)
	{
		UIDrawMovPBFuncEx(bID0, MOVPB_GUI_LOCK_FILE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		Lock_status = bLockCurMovFile;
	}
#endif

#if	OSD_SHOW_BATTERY_STATUS == 1
	AHC_DrawChargingIcon_UsingTimer(bID0);
#endif

    UIDrawMovPBFuncEx(bID0, MOVPB_GUI_CUR_TIME, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);

    #ifdef FLM_GPIO_NUM
    AHC_OSDRefresh_PLCD();
    #endif
}

void DrawStateMovPbMode_Update(void)
{
	DrawStateMovPbInit();
}

#if 0
void ____Event_Handler____(){ruturn;} //dummy
#endif

void DrawStateMovPlaybackUpdate(UINT32 ubEvent)
{
    switch(ubEvent)
    {
        case EVENT_NONE                           :
        break;

        case EVENT_KEY_TELE_PRESS                 :
        case EVENT_KEY_WIDE_PRESS                 :
            DrawMovPB_VolumeBar(AHC_FALSE);
        break;

        case EVENT_KEY_WIDE_STOP                  :
        case EVENT_KEY_TELE_STOP                  :
        	DrawMovPB_VolumeBar(AHC_TRUE);
        break;

        case EVENT_LCD_COVER_NORMAL               :
        case EVENT_LCD_COVER_ROTATE               :
			AHC_OSDSetActive(OVL_DISPLAY_BUFFER, 0);
			DrawStateMovPbInit();
			AHC_OSDSetActive(OVL_DISPLAY_BUFFER, 1);
        break;

        case EVENT_MOVPB_MODE_INIT                :
			AHC_OSDSetActive(OVL_DISPLAY_BUFFER, 0);
			DrawStateMovPbInit();
			AHC_OSDSetActive(OVL_DISPLAY_BUFFER, 1);
        break;

        case EVENT_MOVPB_TOUCH_PREV_PRESS         :
        case EVENT_MOVPB_TOUCH_NEXT_PRESS         :
			AHC_OSDSetActive(OVL_DISPLAY_BUFFER, 0);
			DrawStateMovPbChangeFile();
			AHC_OSDSetActive(OVL_DISPLAY_BUFFER, 1);
        break;

        case EVENT_MOVPB_TOUCH_PLAY_PRESS         :
		case EVENT_MOVPB_TOUCH_BKWD_PRESS 		  :
		case EVENT_MOVPB_TOUCH_FRWD_PRESS         :
        case EVENT_MOVPB_TOUCH_PAUSE_PRESS 		  :
        #if (SUPPORT_TOUCH_PANEL)//disable temporally, wait touch UI
        	DrawStateMovPbStateWithTouchButton();
        #else
        	DrawStateMovPbState();
        #endif
        break;

        case EVENT_MOVPB_UPDATE_MESSAGE           :
			DrawStateMovPbUpdate();
        break;

        default:
        break;
    }
}

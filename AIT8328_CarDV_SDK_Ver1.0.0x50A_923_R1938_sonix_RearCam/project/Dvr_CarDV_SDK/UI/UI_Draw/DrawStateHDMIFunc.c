/*===========================================================================
 * Include files
 *===========================================================================*/

#include "AHC_GUI.h"
#include "AHC_OS.h"
#include "AHC_Browser.h"
#include "AHC_Dcf.h"
#include "AHC_UF.h"
#include "AHC_Menu.h"
#include "AHC_Media.h"
#include "AHC_Display.h"
#include "AHC_General.h"
#include "AHC_General_CarDV.h"
#include "MenuCommon.h"
#include "MenuDrawCommon.h"
#include "MenuDrawingFunc.h"
#include "KeyParser.h"
#include "MenuSetting.h"
#include "DrawStateMenuSetting.h"
#include "DrawStateBrowserFunc.h"
#include "DrawStateHDMIFunc.h"
#include "DrawStateTVFunc.h"
#include "DrawStateVideoFunc.h"
#include "DrawStateCameraFunc.h"
#include "DrawStateMoviePBFunc.h"
#include "DrawStatePhotoPBFunc.h"
#include "ColorDefine.h"
#include "IconPosition.h"
#include "IconDefine.h"
#include "MediaPlaybackCtrl.h"
#include "OsdStrings.h"
#include "ShowOSDFunc.h"
#include "StateHDMIFunc.h"
#include "StateTVFunc.h"
#include "UI_DrawGeneral.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/

#if (DIR_KEY_TYPE!=KEY_TYPE_4KEY)
#define SHOW_HDMI_DIR_ICON		(0)
#else
#define SHOW_HDMI_DIR_ICON		(0)
#endif

/*===========================================================================
 * Extern varible
 *===========================================================================*/

extern UINT8 		m_ubBatteryStatus;
extern UINT8 		m_DelFile;
extern AHC_BOOL 	PowerOff_InProcess;
extern AHC_BOOL		PowerOff_Option;
extern UINT8 		bHdmiMovieState;
extern AHC_BOOL 	Protecting_InBrowser;
extern AHC_BOOL 	Deleting_InBrowser;

/*===========================================================================
 * Main body
 *===========================================================================*/

#if (HDMI_ENABLE)

void UpdateHdmiMovPBStatus(UINT8 ubStatus)
{
	bHdmiMovieState = ubStatus;
}

void DrawHdmiBrowser_PageInfo(UINT8 ubID)
{
    UINT16  CurrPage 	= 0;
    UINT16  MaxPage 	= 0;
    UINT8   digit 		= 0;
    UINT16  leftpos 	= 0;
    UINT16  tempOffset 	= 55;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(ubID, OSD_COLOR_TITLE);
#else
    AHC_OSDSetColor(OSD_COLOR_TITLE);
#endif

    switch(MenuSettingConfig()->uiHDMIOutput)
    {
        case HDMI_OUTPUT_1080I:
            tempOffset = 55;
            AHC_OSDDrawFillRect(ubID, 110+tempOffset, 0, 300+tempOffset, 150);
            break;
        case HDMI_OUTPUT_720P:
            tempOffset = 40;
            AHC_OSDDrawFillRect(ubID, 50+tempOffset, 0, 250+tempOffset, 100);
            break;
        case HDMI_OUTPUT_480P:      // TBD
            tempOffset = 40;
            AHC_OSDDrawFillRect(ubID, 50+tempOffset, 0, 250+tempOffset, 100);
            break;
        default:
        case HDMI_OUTPUT_1080P:     // TBD
            tempOffset = 55;
            AHC_OSDDrawFillRect(ubID, 110+tempOffset, 0, 300+tempOffset, 150);
            break;
    }

    DrawBrowser_GetPageInfo(&CurrPage, &MaxPage);

    if(0 != MaxPage)
    {
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, OSD_COLOR_WHITE);
        AHC_OSDSetBkColor(ubID, OSD_COLOR_TITLE);
        AHC_OSDSetFont(ubID, &GUI_Font32B_ASCII);
        AHC_OSDSetPenSize(ubID, 3);
		#else
        AHC_OSDSetColor(OSD_COLOR_WHITE);
        AHC_OSDSetBkColor(OSD_COLOR_TITLE);
        AHC_OSDSetFont(&GUI_Font32B_ASCII);
        AHC_OSDSetPenSize(3);
		#endif

        digit = NumOfDigit(MaxPage);

        switch(MenuSettingConfig()->uiHDMIOutput)
        {
            case HDMI_OUTPUT_1080I:
                leftpos = POS_PAGEINFO(digit, 120+tempOffset);
                AHC_OSDDrawDec(ubID, CurrPage, leftpos, 40, digit);

                leftpos = POS_PAGEINFO(digit, 160+tempOffset);
                AHC_OSDDrawDec(ubID, MaxPage, leftpos,  100, digit);

                AHC_OSDDrawLine(ubID, 200+tempOffset, 120+tempOffset, 75, 85);
                break;
            case HDMI_OUTPUT_720P:
                leftpos = POS_PAGEINFO(digit, 60+tempOffset);
                AHC_OSDDrawDec(ubID, CurrPage, leftpos, 20, digit);

                leftpos = POS_PAGEINFO(digit, 90+tempOffset);
                AHC_OSDDrawDec(ubID, MaxPage, leftpos, 60, digit);

                AHC_OSDDrawLine(ubID, 130+tempOffset, 60+tempOffset, 50, 58);
                break;
            case HDMI_OUTPUT_480P:      // TBD
                leftpos = POS_PAGEINFO(digit, 60+tempOffset);
                AHC_OSDDrawDec(ubID, CurrPage, leftpos, 20, digit);

                leftpos = POS_PAGEINFO(digit, 90+tempOffset);
                AHC_OSDDrawDec(ubID, MaxPage, leftpos, 60, digit);

                AHC_OSDDrawLine(ubID, 130+tempOffset, 60+tempOffset, 50, 58);
                break;
            default:
            case HDMI_OUTPUT_1080P:     // TBD
                leftpos = POS_PAGEINFO(digit, 120+tempOffset);
                AHC_OSDDrawDec(ubID, CurrPage, leftpos, 40, digit);

                leftpos = POS_PAGEINFO(digit, 160+tempOffset);
                AHC_OSDDrawDec(ubID, MaxPage, leftpos,  100, digit);

                AHC_OSDDrawLine(ubID, 200+tempOffset, 120+tempOffset, 75, 85);
                break;
        }
    }

	END_LAYER(ubID);
}

void DrawHdmiBrowser_SwitchMode(UINT8 ulID, OP_MODE_SETTING OpMode)
{
    UINT16  ulIconX, ulIconY;
    UINT32 	ulTimeX, ulTimeY;
    UINT8	ulMagSize;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ulID);

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(ulID, OSD_COLOR_TITLE);
#else
    AHC_OSDSetColor(OSD_COLOR_TITLE);
#endif

    switch(MenuSettingConfig()->uiHDMIOutput)
    {
    case HDMI_OUTPUT_1080I:
        ulIconX 	= 1590-50;
        ulIconY 	= 5;
        ulTimeX		= POS_HDMI_1080I_BRO_TOTALTIME_X;
        ulTimeY		= POS_HDMI_1080I_BRO_TOTALTIME_Y;
        ulMagSize	= 4;
        AHC_OSDDrawFillRect(ulID, ulIconX, ulIconY, ulIconX+95, ulIconY+110);
        break;
    case HDMI_OUTPUT_720P:
        ulIconX 	= 1010-20;
        ulIconY 	= 1;
        ulTimeX		= POS_HDMI_720P_BRO_TOTALTIME_X;
        ulTimeY		= POS_HDMI_720P_BRO_TOTALTIME_Y;
        ulMagSize	= 3;
        AHC_OSDDrawFillRect(ulID, ulIconX, ulIconY, ulIconX+95, ulIconY+90);
        break;
    case HDMI_OUTPUT_480P:
        ulIconX 	= 720-20;
        ulIconY 	= 1;
        ulTimeX		= POS_HDMI_480P_BRO_TOTALTIME_X;
        ulTimeY		= POS_HDMI_480P_BRO_TOTALTIME_Y;
        ulMagSize	= 3;
        AHC_OSDDrawFillRect(ulID, ulIconX, ulIconY, ulIconX+95, ulIconY+90);
        break;
    default:
    case HDMI_OUTPUT_1080P:
        ulIconX 	= 1590-50;
        ulIconY 	= 5;
        ulTimeX		= POS_HDMI_1080P_BRO_TOTALTIME_X;
        ulTimeY		= POS_HDMI_1080P_BRO_TOTALTIME_Y;
        ulMagSize	= 4;
        AHC_OSDDrawFillRect(ulID, ulIconX, ulIconY, ulIconX+95, ulIconY+110);
        break;
    }

    if(MOVPB_MODE == OpMode)
    {
    	AHC_OSDDrawBitmapMag(ulID, &bmIcon_Movie_Playback, ulIconX, ulIconY, ulMagSize, ulMagSize);
    }
    else if(JPGPB_MODE == OpMode)
    {
    	AHC_OSDDrawBitmapMag(ulID, &bmIcon_Still_Playback, ulIconX, ulIconY, ulMagSize, ulMagSize);

        DrawHdmiBrowser_TotalTime(ulID, ulTimeX, ulTimeY, LARGE_SIZE, MAG_2X, AHC_FALSE,  OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);
    }

	END_LAYER(ulID);
}

void DrawHdmiBrowser_MediaType(UINT8 ubID, AHC_BOOL OnOff)
{
    UINT32 		ulMediaX, ulMediaY;
    UINT8		ubMagSize;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    switch(MenuSettingConfig()->uiHDMIOutput)
    {
    case HDMI_OUTPUT_1080I:
        ulMediaX 	= POS_HDMI_1080I_BRO_MEDIA_X;
        ulMediaY 	= POS_HDMI_1080I_BRO_MEDIA_Y;
        ubMagSize 	= MAG_4X;
        break;
    case HDMI_OUTPUT_720P:
        ulMediaX 	= POS_HDMI_720P_BRO_MEDIA_X;
        ulMediaY 	= POS_HDMI_720P_BRO_MEDIA_Y;
        ubMagSize 	= MAG_3X;
        break;
    case HDMI_OUTPUT_480P:      // TBD
        ulMediaX 	= POS_HDMI_720P_BRO_MEDIA_X;
        ulMediaY 	= POS_HDMI_720P_BRO_MEDIA_Y;
        ubMagSize 	= MAG_2X;
        break;
    default:
    case HDMI_OUTPUT_1080P:     // TBD
        ulMediaX 	= POS_HDMI_1080P_BRO_MEDIA_X;
        ulMediaY 	= POS_HDMI_1080P_BRO_MEDIA_Y;
        ubMagSize 	= MAG_4X;
        break;
    }

    if(OnOff == AHC_FALSE)
    {
    	BEGIN_LAYER(ubID);

		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, OSD_COLOR_TITLE);
		#else
	    AHC_OSDSetColor(OSD_COLOR_TITLE);
		#endif
        AHC_OSDDrawFillRect(ubID, ulMediaX, ulMediaY, ulMediaX+90, ulMediaY+90);

    	END_LAYER(ubID);
    }
    else
    {
    	OSDDrawSetting_MediaType(ubID, ulMediaX, ulMediaY, LARGE_SIZE, ubMagSize, OnOff, OSD_COLOR_TITLE, NULL_PARAM);
    }
}

void DrawHdmiBrowser_Battery(UINT8 ubID, AHC_BOOL OnOff)
{
    UINT32 		ulBattX, ulBattY;
    UINT8		ubMagSize;

    switch(MenuSettingConfig()->uiHDMIOutput)
    {
    case HDMI_OUTPUT_1080I:
        ulBattX		= POS_HDMI_1080I_BRO_BATT_X;
        ulBattY		= POS_HDMI_1080I_BRO_BATT_Y;
        ubMagSize 	= MAG_4X;
        break;
    case HDMI_OUTPUT_720P:
        ulBattX		= POS_HDMI_720P_BRO_BATT_X;
        ulBattY		= POS_HDMI_720P_BRO_BATT_Y;
        ubMagSize 	= MAG_3X;
        break;
    case HDMI_OUTPUT_480P:      // TBD
        ulBattX		= POS_HDMI_720P_BRO_BATT_X;
        ulBattY		= POS_HDMI_720P_BRO_BATT_Y;
        ubMagSize 	= MAG_2X;
        break;
    default:
    case HDMI_OUTPUT_1080P:     // TBD
        ulBattX		= POS_HDMI_1080P_BRO_BATT_X;
        ulBattY		= POS_HDMI_1080P_BRO_BATT_Y;
        ubMagSize 	= MAG_4X;
        break;
    }

	OSDDrawSetting_BatteryStatus(ubID, ulBattX, ulBattY, LARGE_SIZE, ubMagSize, OnOff, OSD_COLOR_TITLE, NULL_PARAM);
}

void DrawHdmiBrowser_NameDateInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    //char 		 *filename;
    UINT16 		 leftpos		= x;
    UINT16  	 WordWidth 	= 16;
    UINT32       CurrentObjIdx;
    INT8         FileName[MAX_FILE_NAME_SIZE];
    UINT16       uwDirKey = 0;
    AHC_RTC_TIME RtcTime;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);

    if(bFlag == AHC_FALSE)
    {
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, bkColor);
#else
        AHC_OSDSetColor(bkColor);
#endif
        AHC_OSDDrawFillRect(ubID, leftpos-90, y, leftpos+WordWidth*40, y+30);
    }
    else
    {
    	if(Deleting_InBrowser || Protecting_InBrowser)
        {
			// It is for Delete/Protect/Unprotect, Show description
			char	*sz;

			ShowOSD_SetLanguage(ubID);
			sz = (char*)OSD_GetStringViaID(IDS_DS_SELECT_A_FILE);

			AHC_OSDDrawText( ubID, (UINT8*)sz, leftpos-90, y, strlen(sz));;
		}
		else
		{
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
			AHC_OSDSetColor(ubID, bkColor);
			#else
			AHC_OSDSetColor(bkColor);
			#endif
        	AHC_OSDDrawFillRect(ubID, leftpos-90, y, leftpos+WordWidth*40, y+30);
		}

		AHC_UF_GetCurrentIndex(&CurrentObjIdx);

        AHC_UF_GetFileNamebyIndex(CurrentObjIdx, FileName); 
        AHC_UF_GetDirKeybyIndex(CurrentObjIdx, &uwDirKey);
        AHC_UF_GetFileTimebyIndex(CurrentObjIdx, &RtcTime);

		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, bkColor);
		#else
        AHC_OSDSetColor(bkColor);
		#endif
        AHC_OSDDrawFillRect(ubID, leftpos, y, leftpos+WordWidth*40, y+30);
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, OSD_COLOR_WHITE);
        AHC_OSDSetBkColor(ubID, bkColor);
        AHC_OSDSetFont(ubID, &GUI_Font32B_ASCII);
		#else
        AHC_OSDSetColor(OSD_COLOR_WHITE);
        AHC_OSDSetBkColor(bkColor);
        AHC_OSDSetFont(&GUI_Font32B_ASCII);
		#endif

        //Draw Dir Index
        AHC_OSDDrawDec(ubID, uwDirKey, leftpos,  y, 3);
        leftpos += WordWidth*4;
        AHC_OSDDrawText( ubID,(UINT8*)"-", leftpos, y, strlen("-"));

        //Draw File Name
        leftpos += WordWidth*2;
        AHC_OSDDrawText( ubID, (UINT8*)FileName, leftpos, y, strlen(FileName));

        //Draw File Date
        leftpos += WordWidth*24;
        OSDDraw_DateTime(ubID, leftpos, y, WordWidth, (void*)&(RtcTime.uwYear));
    }

	END_LAYER(ubID);
}

void DrawHdmiBrowser_TotalTime(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    UINT16  WordWidth 	= 16;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    if(bFlag == AHC_FALSE)
    {
        BEGIN_LAYER(ubID);
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, bkColor);
#else
        AHC_OSDSetColor(bkColor);
#endif
        AHC_OSDDrawFillRect(ubID, x, y, x+WordWidth*12, y+32);
        END_LAYER(ubID);
    }
    else
    {
        DrawBrowser_TotalTime(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
    }
}

void DrawHdmiBrowser_FileInfo(UINT8 ubID)
{
    UINT32      MaxDcfObj;
	UINT32		ulNameX, ulNameY;
	UINT32		ulTimeX, ulTimeY;

    switch(MenuSettingConfig()->uiHDMIOutput)
    {
        case HDMI_OUTPUT_1080I:
            ulNameX = POS_HDMI_1080I_BRO_NAMEDATE_X;
            ulNameY = POS_HDMI_1080I_BRO_NAMEDATE_Y;
            ulTimeX = POS_HDMI_1080I_BRO_TOTALTIME_X;
            ulTimeY = POS_HDMI_1080I_BRO_TOTALTIME_Y;
            break;
        case HDMI_OUTPUT_720P:
            ulNameX = POS_HDMI_720P_BRO_NAMEDATE_X;
            ulNameY = POS_HDMI_720P_BRO_NAMEDATE_Y;
            ulTimeX = POS_HDMI_720P_BRO_TOTALTIME_X;
            ulTimeY = POS_HDMI_720P_BRO_TOTALTIME_Y;
            break;
        case HDMI_OUTPUT_480P:
            ulNameX = POS_HDMI_480P_BRO_NAMEDATE_X;
            ulNameY = POS_HDMI_480P_BRO_NAMEDATE_Y;
            ulTimeX = POS_HDMI_480P_BRO_TOTALTIME_X;
            ulTimeY = POS_HDMI_480P_BRO_TOTALTIME_Y;
            break;
        default:
        case HDMI_OUTPUT_1080P:
            ulNameX = POS_HDMI_1080P_BRO_NAMEDATE_X;
            ulNameY = POS_HDMI_1080P_BRO_NAMEDATE_Y;
            ulTimeX = POS_HDMI_1080P_BRO_TOTALTIME_X;
            ulTimeY = POS_HDMI_1080P_BRO_TOTALTIME_Y;
            break;
    }

    AHC_UF_GetTotalFileCount(&MaxDcfObj);

    if(MaxDcfObj == 0)
    {
        DrawHdmiBrowser_NameDateInfo(ubID, ulNameX, ulNameY, LARGE_SIZE, MAG_2X, AHC_FALSE,  OSD_COLOR_TITLE, NULL_PARAM);
    }
    else
    {
        if(MOVPB_MODE == GetCurrentOpMode())
        {
            DrawHdmiBrowser_NameDateInfo(ubID, ulNameX, ulNameY, LARGE_SIZE, MAG_2X, AHC_TRUE,  OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);
            DrawHdmiBrowser_TotalTime(ubID,    ulTimeX, ulTimeY, LARGE_SIZE, MAG_2X, AHC_TRUE,  OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);
        }
        else if(JPGPB_MODE == GetCurrentOpMode())
        {
            DrawHdmiBrowser_NameDateInfo(ubID,  ulNameX, ulNameY, LARGE_SIZE, MAG_2X, AHC_TRUE,  OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);
        }
		else if(JPGPB_MOVPB_MODE == GetCurrentOpMode())
        {
            DrawHdmiBrowser_NameDateInfo(ubID,  ulNameX, ulNameY, LARGE_SIZE, MAG_2X, AHC_TRUE,  OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);
        }
    }
}

void DrawHdmiMoviePB_FileIndex(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    DrawMovPb_FileIndex(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
}

void DrawHdmiMoviePB_NameInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    DrawMovPb_FileInfo(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
}

void DrawHdmiMoviePB_PlayInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
	DrawMovPb_PlayInfo(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
}

void DrawHdmiMoviePB_TotalTime(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    DrawMovPb_TotalTime(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
}

void DrawHdmiMoviePB_Status(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
	DrawMovPb_Status(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
}

void DrawHdmiMoviePB_VolumeBar(UINT8 ubID, AHC_BOOL bForce)
{
#if !defined(CAR_DV)
    static UINT16 uwVolIdx = 0;
    UINT16 		uwVolIdx_temp;
    UINT8 		i;
    RECT 		rc = {20 , 210 , 15, 18};
    UINT16 		Left_temp;
    GUI_COLOR 	FillColor;
    RECT 		StrRECT = {20, 190, 195, 20};

    uwVolIdx_temp = MenuSettingConfig()->uiVolume;

    if(uwVolIdx == uwVolIdx_temp && !bForce)
        return;

    uwVolIdx = uwVolIdx_temp;

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(ubID, OSD_COLOR_WHITE);
    AHC_OSDSetBkColor(ubID, OSD_COLOR_TRANSPARENT);
	#else
    AHC_OSDSetColor(OSD_COLOR_WHITE);
    AHC_OSDSetBkColor(OSD_COLOR_TRANSPARENT);
	#endif

    OSD_ShowStringPool(ubID, IDS_DS_VOLUME, StrRECT, OSD_COLOR_WHITE, OSD_COLOR_TRANSPARENT, GUI_TA_CENTER|GUI_TA_VCENTER);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetFont(ubID, &GUI_Font24B_ASCII);
	#else
    AHC_OSDSetFont(&GUI_Font24B_ASCII);
	#endif
    AHC_OSDDrawText(ubID, (UINT8*)"_", 0,   180, 1);
    AHC_OSDDrawText(ubID, (UINT8*)"+", 222, 190, 1);

    for(i=0; i<10; i++)
    {
        Left_temp = rc.uiLeft + ( rc.uiWidth + 5 ) * i;

        if(i < (uwVolIdx))
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
        AHC_OSDDrawFillRect(ubID, Left_temp+1, rc.uiTop+1, Left_temp+rc.uiWidth-1, rc.uiTop+rc.uiHeight-1 );
    }
#endif
}

void DrawHdmiPhotoPB_FileIndex(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    DrawJpgPb_FileIndex(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
}

void DrawHdmiPhotoPB_NameInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    DrawJpgPb_FileInfo(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
}

void DrawHdmiPhotoPB_DateInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    DrawJpgPb_DateInfo(ubID, x, y, bLarge, ubMag, bFlag, bkColor, Para1);
}

#if 0
void ____HDMI_Preview_Function_____(){ruturn;} //dummy
#endif

void DrawStateHdmiPreviewClear(UINT16 bID0)
{
    UINT16		uwHDMIWidth, uwHDMIHeight;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    switch(MenuSettingConfig()->uiHDMIOutput)
    {
        case HDMI_OUTPUT_1080I:
            AHC_GetHdmiDisplayWidthHeight(&uwHDMIWidth, &uwHDMIHeight);
            break;
        case HDMI_OUTPUT_720P:
            AHC_GetHdmiDisplayWidthHeight(&uwHDMIWidth, &uwHDMIHeight);
            break;
        case HDMI_OUTPUT_480P:
            AHC_GetHdmiDisplayWidthHeight(&uwHDMIWidth, &uwHDMIHeight);
            break;
        default:
            case HDMI_OUTPUT_1080P:
            AHC_GetHdmiDisplayWidthHeight(&uwHDMIWidth, &uwHDMIHeight);
        break;
    }

    BEGIN_LAYER(bID0);
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, OSD_COLOR_TRANSPARENT);
#else
    AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
#endif
    AHC_OSDDrawFillRect(bID0, 0, 0, uwHDMIWidth, uwHDMIHeight);
    END_LAYER(bID0);
}

void DrawStateHdmiPreviewVideoInit(UINT8 ubID)
{
#if (HDMI_PREVIEW_EN)
    UINT8	idx	 = 0;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    DrawStateHdmiPreviewClear(ubID);

    BEGIN_LAYER(ubID);

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(ubID, 0xAA000000);
#else
    AHC_OSDSetColor(0xAA000000);
#endif

    END_LAYER(ubID);

    SetCurrentOpMode(VIDEOREC_MODE);

    for (idx = 0; idx < HDMI_GUI_VIDEO_MAX; idx++)
    {
        if(idx==HDMI_GUI_VIDEO_REC_STATE)
            UpdateVideoRecordStatus(VIDEO_REC_STOP);

        if(idx==HDMI_GUI_VIDEO_REC_POINT)
            continue;

        UIDrawHdmi_VideoFuncEx(ubID, idx, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    }
#endif
}

void DrawStateHdmiPreviewCameraInit(UINT8 ubID)
{
#if (HDMI_PREVIEW_EN)
    UINT8   idx  = 0;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    DrawStateHdmiPreviewClear(ubID);

    BEGIN_LAYER(ubID);

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(ubID, 0xAA000000);
#else
    AHC_OSDSetColor(0xAA000000);
#endif

    END_LAYER(ubID);

    SetCurrentOpMode(CAPTURE_MODE);

    for(idx = 0; idx<HDMI_GUI_DSC_MAX; idx++)
    {
        UIDrawHdmi_DSCFuncEx(ubID, idx, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    }
#endif
}

#if 0
void ____HDMI_Browser_Function_____(){ruturn;} //dummy
#endif

void DrawStateHdmiBrowserInit(void)
{
    UINT8  	bID0 		= 0;
    UINT8  	bID1 		= 0;
    UINT16 	tempOffset 	= 55;
    RECT  	OsdRect;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

#if (POWER_OFF_CONFIRM_PAGE_EN)
    if(PowerOff_InProcess)
    {
        OSDDraw_EnterDrawing(&bID0, &bID1);
        OSDDraw_ClearOvlDrawOvlBufferAll();
        MenuDrawSubPage_PowerOff_HDMI_Browser(bID0);
        PowerOff_Option  = CONFIRM_OPT_YES;
        OSDDraw_ExitDrawing(&bID0, &bID1);
        return;
    }
#endif

    switch(MenuSettingConfig()->uiHDMIOutput)
    {
    case HDMI_OUTPUT_1080I:
        OsdRect.uiLeft 		= POS_HDMI_1080I_BROWSER_OSD_X0;
        OsdRect.uiTop 		= POS_HDMI_1080I_BROWSER_OSD_Y0;
        OsdRect.uiWidth 	= POS_HDMI_1080I_BROWSER_OSD_W;
        OsdRect.uiHeight 	= POS_HDMI_1080I_BROWSER_OSD_H;
        tempOffset 			= 55;
        break;
    case HDMI_OUTPUT_720P:
        OsdRect.uiLeft 		= POS_HDMI_720P_BROWSER_OSD_X0;
        OsdRect.uiTop 		= POS_HDMI_720P_BROWSER_OSD_Y0;
        OsdRect.uiWidth 	= POS_HDMI_720P_BROWSER_OSD_W;
        OsdRect.uiHeight 	= POS_HDMI_720P_BROWSER_OSD_H;
        tempOffset 			= 40;
        break;
    case HDMI_OUTPUT_480P:
        OsdRect.uiLeft 		= POS_HDMI_480P_BROWSER_OSD_X0;
        OsdRect.uiTop 		= POS_HDMI_480P_BROWSER_OSD_Y0;
        OsdRect.uiWidth 	= POS_HDMI_480P_BROWSER_OSD_W;
        OsdRect.uiHeight 	= POS_HDMI_480P_BROWSER_OSD_H;
        tempOffset 			= 40;
        break;
    default:
    case HDMI_OUTPUT_1080P:
        OsdRect.uiLeft 		= POS_HDMI_1080P_BROWSER_OSD_X0;
        OsdRect.uiTop 		= POS_HDMI_1080P_BROWSER_OSD_Y0;
        OsdRect.uiWidth 	= POS_HDMI_1080P_BROWSER_OSD_W;
        OsdRect.uiHeight 	= POS_HDMI_1080P_BROWSER_OSD_H;
        tempOffset 			= 55;
        break;
    }

    OSDDraw_EnterDrawing(&bID0, &bID1);

	BEGIN_LAYER(bID0);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, OSD_COLOR_TITLE);
	#else
    AHC_OSDSetColor(OSD_COLOR_TITLE);
    #endif

    AHC_OSDDrawFillRect(bID0, 0, 0, OsdRect.uiLeft+OsdRect.uiWidth, OsdRect.uiTop+OsdRect.uiHeight);

    switch(MenuSettingConfig()->uiHDMIOutput)
    {
    case HDMI_OUTPUT_1080I:
        AHC_OSDDrawBitmapMag(bID0, &bmIcon_D_Up, 	tempOffset, 0, 2, 2);
        AHC_OSDDrawBitmapMag(bID0, &bmIcon_D_Down, 	tempOffset, POS_HDMI_1080I_BROWSER_OSD_H>>1, 2, 2);
        break;
    case HDMI_OUTPUT_720P:
        AHC_OSDDrawBitmapMag(bID0, &bmIcon_D_Up, 	tempOffset, 0, 1, 1);
        AHC_OSDDrawBitmapMag(bID0, &bmIcon_D_Down, 	tempOffset, POS_HDMI_720P_BROWSER_OSD_H>>1,  1, 1);
        break;
    case HDMI_OUTPUT_480P:
        AHC_OSDDrawBitmapMag(bID0, &bmIcon_D_Up, 	tempOffset, 0, 1, 1);
        AHC_OSDDrawBitmapMag(bID0, &bmIcon_D_Down, 	tempOffset, POS_HDMI_480P_BROWSER_OSD_H>>1,  1, 1);
        break;
    default:
    case HDMI_OUTPUT_1080P:
        AHC_OSDDrawBitmapMag(bID0, &bmIcon_D_Up, 	tempOffset, 0, 2, 2);
        AHC_OSDDrawBitmapMag(bID0, &bmIcon_D_Down, 	tempOffset, POS_HDMI_1080P_BROWSER_OSD_H>>1, 2, 2);
        break;
    }

	END_LAYER(bID0);

    DrawHdmiBrowser_SwitchMode(bID0, GetCurrentOpMode());

    DrawHdmiBrowser_PageInfo(bID0);

    DrawHdmiBrowser_FileInfo(bID0);

    DrawHdmiBrowser_MediaType(bID0, AHC_TRUE);

    DrawHdmiBrowser_Battery(bID0, AHC_TRUE);

	BEGIN_LAYER(bID0);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, OSD_COLOR_WHITE);
    AHC_OSDSetBkColor(bID0, OSD_COLOR_TRANSPARENT);
    AHC_OSDSetPenSize(bID0, 3);
	#else
    AHC_OSDSetColor(OSD_COLOR_WHITE);
    AHC_OSDSetBkColor(OSD_COLOR_TRANSPARENT);
    AHC_OSDSetPenSize(3);
    #endif

    AHC_OSDDrawLine(bID0, 0, OsdRect.uiWidth-3, OsdRect.uiHeight-3, OsdRect.uiHeight-3);

	END_LAYER(bID0);

    OSDDraw_ExitDrawing(&bID0, &bID1);
}

void DrawStateHdmiBrowserSwitch(void)
{
    UINT8  bID0 = 0;
    UINT8  bID1 = 0;

    OSDDraw_EnterDrawing(&bID0, &bID1);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, 0xAA000000);
	#else
    AHC_OSDSetColor(0xAA000000);
    #endif

    DrawHdmiBrowser_SwitchMode(bID0, GetCurrentOpMode());
    DrawHdmiBrowser_PageInfo(bID0);
    DrawHdmiBrowser_FileInfo(bID0);

    OSDDraw_ExitDrawing(&bID0, &bID1);
}

void DrawStateHdmiBrowserPageUpdate(void)
{
    UINT8  bID0 = 0;
    UINT8  bID1 = 0;

    OSDDraw_EnterDrawing(&bID0, &bID1);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, 0xAA000000);
	#else
    AHC_OSDSetColor(0xAA000000);
    #endif

    DrawHdmiBrowser_PageInfo(bID0);
    DrawHdmiBrowser_FileInfo(bID0);

    OSDDraw_ExitDrawing(&bID0, &bID1);
}

#if 0
void ____HDMI_MoviePB_Function_____(){ruturn;} //dummy
#endif

void DrawClearTrashCan(UINT8 ubID)
{
#if 0
    UINT32 CurObjIdx;

    AHC_UF_GetCurrentIndex(&CurObjIdx);

    if(AHC_IsFileProtect(CurObjIdx) == AHC_TRUE)
    {
        AHC_OSDSetColor(ubID, OSD_COLOR_FONT);
        AHC_OSDDrawLine(ubID, 155, 155+20, 154, 154+19);
        AHC_OSDDrawLine(ubID, 155+20, 155, 154, 154+19);
    }
#endif
}

void DrawStateHdmiMoviePBInit(UINT8 bID0, UINT8 bID1)
{
    UINT8   idx;
    UINT32 	iVal[2];
    RECT  	OsdRect  = POS_HDMI_PLAY_OSD;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    OSDDraw_EnterDrawing(&bID0, &bID1);

    BEGIN_LAYER(bID0);
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, OSD_COLOR_TRANSPARENT);
    AHC_OSDSetBkColor(bID0, OSD_COLOR_TRANSPARENT);
#else
    AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
    AHC_OSDSetBkColor(OSD_COLOR_TRANSPARENT);
#endif
    AHC_OSDDrawFillRect2(bID0, &OsdRect);
    END_LAYER(bID0);

    for (idx = 0; idx < HDMI_GUI_MOVPB_MAX; idx++)
    {
        if(idx==HDMI_GUI_MOVPB_CUR_TIME)
        {
            if(bHdmiMovieState != MOVIE_STATE_PLAY)
                UIDrawHdmi_MovPBFuncEx(bID0, idx, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
        }
        else
            UIDrawHdmi_MovPBFuncEx(bID0, idx, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    }

#if (POWER_OFF_CONFIRM_PAGE_EN)
    if(PowerOff_InProcess)
    {
        MenuDrawSubPage_PowerOff_HDMI_Play(bID0);
    }
#endif

    iVal[0] = 1;
    iVal[1] = OSD_COLOR_TRANSPARENT;
    AHC_OSDSetDisplayAttr(bID0, AHC_OSD_ATTR_TRANSPARENT_ENABLE, iVal);
    AHC_OSDSetDisplayAttr(bID1, AHC_OSD_ATTR_TRANSPARENT_ENABLE, iVal);

    OSDDraw_ExitDrawing(&bID0, &bID1);
}

void DrawStateHdmiMoviePBPlayInfo(void)
{
    UINT8    bID0 = 0;

    OSDDraw_GetLastOvlDrawBuffer(&bID0);

    UIDrawHdmi_MovPBFuncEx(bID0, HDMI_GUI_MOVPB_CUR_TIME, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
}

void DrawStateHdmiMoviePBDel(UINT8 state)
{
    UINT8  	bID0 		= 0;
    RECT  	OsdRect  	= POS_HDMI_PLAY_OSD;
    RECT 	StrDelRECT 	= {30,  70,  200, 20};
    RECT 	StrYesRECT 	= {15,  100, 102, 36};
    RECT 	StrNoRECT 	= {140, 100, 102, 36};
    RECT 	tmpRECT;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    OSDDraw_GetLastOvlDrawBuffer(&bID0);

    BEGIN_LAYER(bID0);
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, OSD_COLOR_TRANSPARENT);
#else
    AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
#endif
    AHC_OSDDrawFillRect(bID0, OsdRect.uiLeft, OsdRect.uiTop+40, OsdRect.uiLeft+OsdRect.uiWidth, OsdRect.uiTop+40+135);

    if(state == DEL_FILE_NONE) {
        END_LAYER(bID0);
        return;
    }

    switch(state) {
	    #if 0
	    case DEL_FILE_NONE: //Clear Buf
	        AHC_OSDSetColor(ubID, OSD_COLOR_TRANSPARENT);
	        AHC_OSDDrawFillRect(bID0, OsdRect.uiLeft, OsdRect.uiTop+40, OsdRect.uiLeft+OsdRect.uiWidth, OsdRect.uiTop+40+135);
	    break;
	    #endif

	    case DEL_FILE_SELECT_NO:
	        AHC_OSDDrawBitmap(bID0, &BMICON_SUBBAR_WHITE, StrYesRECT.uiLeft, StrYesRECT.uiTop);
	        tmpRECT.uiLeft = StrYesRECT.uiLeft+2; tmpRECT.uiTop = StrYesRECT.uiTop+4; tmpRECT.uiWidth = StrYesRECT.uiWidth-5; tmpRECT.uiHeight = StrYesRECT.uiHeight-9;
	        OSD_ShowStringPool(bID0,IDS_DS_YES, tmpRECT, OSD_COLOR_FONT, OSD_COLOR_TV_BACKGROUND, GUI_TA_HCENTER|GUI_TA_VCENTER);

	        AHC_OSDDrawBitmap(bID0, &BMICON_SUBBAR_YELLOW, StrNoRECT.uiLeft, StrNoRECT.uiTop);
	        tmpRECT.uiLeft = StrNoRECT.uiLeft+2; tmpRECT.uiTop = StrNoRECT.uiTop+4; tmpRECT.uiWidth = StrNoRECT.uiWidth-5; tmpRECT.uiHeight = StrNoRECT.uiHeight-9;
	        OSD_ShowStringPool(bID0,IDS_DS_NO, tmpRECT, OSD_COLOR_FONT, OSD_COLOR_TV_BACKGROUND, GUI_TA_HCENTER|GUI_TA_VCENTER);

	        OSD_ShowStringPool(bID0, IDS_DS_DELETE, StrDelRECT, OSD_COLOR_WHITE, OSD_COLOR_TRANSPARENT, GUI_TA_CENTER|GUI_TA_VCENTER);

			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	        AHC_OSDSetColor(bID0, OSD_COLOR_WHITE);
        	AHC_OSDSetBkColor(bID0, OSD_COLOR_TRANSPARENT);
        	AHC_OSDSetFont(bID0, &GUI_Font16B_ASCII);
			#else
	        AHC_OSDSetColor(OSD_COLOR_WHITE);
        	AHC_OSDSetBkColor(OSD_COLOR_TRANSPARENT);
        	AHC_OSDSetFont(&GUI_Font16B_ASCII);
			#endif
        	AHC_OSDDrawText(bID0, (UINT8*)"?", 150, 72, 1);
	    break;

	    case DEL_FILE_SELECT_YES:
	        AHC_OSDDrawBitmap(bID0, &BMICON_SUBBAR_YELLOW, StrYesRECT.uiLeft, StrYesRECT.uiTop);
	        tmpRECT.uiLeft = StrYesRECT.uiLeft+2; tmpRECT.uiTop = StrYesRECT.uiTop+4; tmpRECT.uiWidth = StrYesRECT.uiWidth-5; tmpRECT.uiHeight = StrYesRECT.uiHeight-9;
	        OSD_ShowStringPool(bID0,IDS_DS_YES, tmpRECT, OSD_COLOR_FONT, OSD_COLOR_TV_BACKGROUND, GUI_TA_HCENTER|GUI_TA_VCENTER);

	        AHC_OSDDrawBitmap(bID0, &BMICON_SUBBAR_WHITE, StrNoRECT.uiLeft, StrNoRECT.uiTop);
	        tmpRECT.uiLeft = StrNoRECT.uiLeft+2; tmpRECT.uiTop = StrNoRECT.uiTop+4; tmpRECT.uiWidth = StrNoRECT.uiWidth-5; tmpRECT.uiHeight = StrNoRECT.uiHeight-9;
	        OSD_ShowStringPool(bID0,IDS_DS_NO, tmpRECT, OSD_COLOR_FONT, OSD_COLOR_TV_BACKGROUND, GUI_TA_HCENTER|GUI_TA_VCENTER);

	        OSD_ShowStringPool(bID0, IDS_DS_DELETE, StrDelRECT, OSD_COLOR_WHITE, OSD_COLOR_TRANSPARENT, GUI_TA_CENTER|GUI_TA_VCENTER);

			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	        AHC_OSDSetColor(bID0, OSD_COLOR_WHITE);
        	AHC_OSDSetBkColor(bID0, OSD_COLOR_TRANSPARENT);
        	AHC_OSDSetFont(bID0, &GUI_Font16B_ASCII);
			#else
	        AHC_OSDSetColor(OSD_COLOR_WHITE);
        	AHC_OSDSetBkColor(OSD_COLOR_TRANSPARENT);
        	AHC_OSDSetFont(&GUI_Font16B_ASCII);
			#endif
        	AHC_OSDDrawText(bID0, (UINT8*)"?", 150, 72, 1);
	    break;

	    case DEL_FILE_DECIDE_NO:
	        AHC_OSDDrawBitmap(bID0, &BMICON_SUBBAR_WHITE, StrYesRECT.uiLeft, StrYesRECT.uiTop);
	        tmpRECT.uiLeft = StrYesRECT.uiLeft+2; tmpRECT.uiTop = StrYesRECT.uiTop+4; tmpRECT.uiWidth = StrYesRECT.uiWidth-5; tmpRECT.uiHeight = StrYesRECT.uiHeight-9;
	        OSD_ShowStringPool(bID0,IDS_DS_YES, tmpRECT, OSD_COLOR_FONT, OSD_COLOR_TV_BACKGROUND, GUI_TA_HCENTER|GUI_TA_VCENTER);

	        AHC_OSDDrawBitmap(bID0, &BMICON_SUBBAR_YELLOW, StrNoRECT.uiLeft, StrNoRECT.uiTop);
	        tmpRECT.uiLeft = StrNoRECT.uiLeft+2; tmpRECT.uiTop = StrNoRECT.uiTop+4; tmpRECT.uiWidth = StrNoRECT.uiWidth-5; tmpRECT.uiHeight = StrNoRECT.uiHeight-9;
	        OSD_ShowStringPool(bID0,IDS_DS_NO, tmpRECT, OSD_COLOR_FONT, OSD_COLOR_SELECT, GUI_TA_HCENTER|GUI_TA_VCENTER);

	        OSD_ShowStringPool(bID0, IDS_DS_DELETE, StrDelRECT, OSD_COLOR_WHITE, OSD_COLOR_TRANSPARENT, GUI_TA_CENTER|GUI_TA_VCENTER);

			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	        AHC_OSDSetColor(bID0, OSD_COLOR_WHITE);
        	AHC_OSDSetBkColor(bID0, OSD_COLOR_TRANSPARENT);
        	AHC_OSDSetFont(bID0, &GUI_Font16B_ASCII);
			#else
	        AHC_OSDSetColor(OSD_COLOR_WHITE);
        	AHC_OSDSetBkColor(OSD_COLOR_TRANSPARENT);
        	AHC_OSDSetFont(&GUI_Font16B_ASCII);
			#endif
        	AHC_OSDDrawText(bID0, (UINT8*)"?", 150, 72, 1);
	    break;

	    case DEL_FILE_DECIDE_YES:
	        AHC_OSDDrawBitmap(bID0, &BMICON_SUBBAR_YELLOW, StrYesRECT.uiLeft, StrYesRECT.uiTop);
	        tmpRECT.uiLeft = StrYesRECT.uiLeft+2; tmpRECT.uiTop = StrYesRECT.uiTop+4; tmpRECT.uiWidth = StrYesRECT.uiWidth-5; tmpRECT.uiHeight = StrYesRECT.uiHeight-9;
	        OSD_ShowStringPool(bID0,IDS_DS_YES, tmpRECT, OSD_COLOR_FONT, OSD_COLOR_SELECT, GUI_TA_HCENTER|GUI_TA_VCENTER);

	        AHC_OSDDrawBitmap(bID0, &BMICON_SUBBAR_WHITE, StrNoRECT.uiLeft, StrNoRECT.uiTop);
	        tmpRECT.uiLeft = StrNoRECT.uiLeft+2; tmpRECT.uiTop = StrNoRECT.uiTop+4; tmpRECT.uiWidth = StrNoRECT.uiWidth-5; tmpRECT.uiHeight = StrNoRECT.uiHeight-9;
	        OSD_ShowStringPool(bID0,IDS_DS_NO, tmpRECT, OSD_COLOR_FONT, OSD_COLOR_TV_BACKGROUND, GUI_TA_HCENTER|GUI_TA_VCENTER);

	        OSD_ShowStringPool(bID0, IDS_DS_DELETE, StrDelRECT, OSD_COLOR_WHITE, OSD_COLOR_TRANSPARENT, GUI_TA_CENTER|GUI_TA_VCENTER);

			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	        AHC_OSDSetColor(bID0, OSD_COLOR_WHITE);
        	AHC_OSDSetBkColor(bID0, OSD_COLOR_TRANSPARENT);
        	AHC_OSDSetFont(bID0, &GUI_Font16B_ASCII);
			#else
	        AHC_OSDSetColor(OSD_COLOR_WHITE);
        	AHC_OSDSetBkColor(OSD_COLOR_TRANSPARENT);
        	AHC_OSDSetFont(&GUI_Font16B_ASCII);
			#endif
        	AHC_OSDDrawText(bID0, (UINT8*)"?", 150, 72, 1);
	    break;
    }

	END_LAYER(bID0);
}

void DrawStateHdmiMoviePBStatus(void)
{
    UINT8  bID0 = 0;

    OSDDraw_GetLastOvlDrawBuffer(&bID0);

    UIDrawHdmi_MovPBFuncEx(bID0, HDMI_GUI_MOVPB_STATUS, AHC_TRUE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
}

void DrawStateHdmiMoviePBChangeFile(void)
{
    UINT8  bID0 = 0;

    OSDDraw_GetLastOvlDrawBuffer( &bID0 );

	UpdateHdmiMovPBStatus(MOVIE_STATE_PLAY);

    UIDrawHdmi_MovPBFuncEx(bID0, HDMI_GUI_MOVPB_FILENAME, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    UIDrawHdmi_MovPBFuncEx(bID0, HDMI_GUI_MOVPB_TOTAL_TIME, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    UIDrawHdmi_MovPBFuncEx(bID0, HDMI_GUI_MOVPB_FILE_INDEX, AHC_TRUE,  AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    UIDrawHdmi_MovPBFuncEx(bID0, HDMI_GUI_MOVPB_STATUS, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
}

#if 0
void ____HDMI_PhotoPB_Function_____(){ruturn;} //dummy
#endif

void DrawStateHdmiPhotoPBInit(UINT8 bID0, UINT8 bID1)
{
    UINT8   idx;
    UINT32 	iVal[2];
    RECT  	OsdRect = POS_HDMI_PLAY_OSD;

    OSDDraw_EnterDrawing(&bID0, &bID1);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, OSD_COLOR_TRANSPARENT);
	#else
    AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
	#endif
    AHC_OSDDrawFillRect2(bID0, &OsdRect);

 	for (idx = 0; idx < HDMI_GUI_JPGPB_MAX; idx++)
	{
		UIDrawHdmi_JpgPBFuncEx(bID0, idx, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
	}

#if (SHOW_HDMI_DIR_ICON)
    AHC_OSDDrawBitmap(bID0, &bmIcon_HdmiStillPlay, 100, 45);
#endif

#if (POWER_OFF_CONFIRM_PAGE_EN)
	if(PowerOff_InProcess)
	{
		MenuDrawSubPage_PowerOff_HDMI_Play(bID0);
	}
#endif

    iVal[0] = 1;
    iVal[1] = OSD_COLOR_TRANSPARENT;
    AHC_OSDSetDisplayAttr(bID0, AHC_OSD_ATTR_TRANSPARENT_ENABLE, iVal);
    AHC_OSDSetDisplayAttr(bID1, AHC_OSD_ATTR_TRANSPARENT_ENABLE, iVal);

    OSDDraw_ExitDrawing(&bID0, &bID1);
}

void DrawStateHdmiPhotoPBChangeFile(void)
{
    UINT8  bID0 = 0;

	OSDDraw_GetLastOvlDrawBuffer(&bID0);

	UIDrawHdmi_JpgPBFuncEx(bID0, HDMI_GUI_JPGPB_FILENAME, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
	UIDrawHdmi_JpgPBFuncEx(bID0, HDMI_GUI_JPGPB_FILEDATE, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
	UIDrawHdmi_JpgPBFuncEx(bID0, HDMI_GUI_JPGPB_FILE_INDEX, AHC_TRUE,  AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);

#if (SHOW_HDMI_DIR_ICON)
    AHC_OSDDrawBitmap(bID0, &bmIcon_HdmiStillPlay, 100, 45);
#endif
}

#if 0
void ____HDMI_Slideshow_Function_____(){ruturn;} //dummy
#endif

void DrawStateHdmiSlideShowInit(void)
{
    UINT8  	bID0 	= 0;
    UINT8  	bID1 	= 0;
    UINT32	iVal[2];
    RECT  	OsdRect = POS_TV_PLAY_OSD;

    OSDDraw_EnterDrawing(&bID0, &bID1);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, OSD_COLOR_TRANSPARENT);
    AHC_OSDSetBkColor(bID0, OSD_COLOR_TRANSPARENT);
	#else
    AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
    AHC_OSDSetBkColor(OSD_COLOR_TRANSPARENT);
	#endif
    AHC_OSDDrawFillRect2(bID0, &OsdRect);

    if(GetPlayBackFileType() == PLAYBACK_IMAGE_TYPE)
    {
		UIDrawHdmi_JpgPBFuncEx(bID0, HDMI_GUI_JPGPB_FILENAME, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		UIDrawHdmi_JpgPBFuncEx(bID0, HDMI_GUI_JPGPB_FILEDATE, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		UIDrawHdmi_JpgPBFuncEx(bID0, HDMI_GUI_JPGPB_FILE_INDEX, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
	}
    else
    {
		UpdateHdmiMovPBStatus(MOVIE_STATE_PLAY);

   		UIDrawHdmi_MovPBFuncEx(bID0, HDMI_GUI_MOVPB_FILENAME, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    	UIDrawHdmi_MovPBFuncEx(bID0, HDMI_GUI_MOVPB_TOTAL_TIME, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    	UIDrawHdmi_MovPBFuncEx(bID0, HDMI_GUI_MOVPB_STATUS, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    	UIDrawHdmi_MovPBFuncEx(bID0, HDMI_GUI_MOVPB_FILE_INDEX, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    }

#if (SHOW_HDMI_DIR_ICON)
    AHC_OSDDrawBitmap(bID0, &bmIcon_HdmiSlideShow, 100, 45);
#endif

    iVal[0] = 1;
    iVal[1] = OSD_COLOR_TRANSPARENT;
    AHC_OSDSetDisplayAttr(bID0, AHC_OSD_ATTR_TRANSPARENT_ENABLE, iVal);
    AHC_OSDSetDisplayAttr(bID1, AHC_OSD_ATTR_TRANSPARENT_ENABLE, iVal);

    OSDDraw_ExitDrawing(&bID0, &bID1);
}

void DrawStateHdmiSlideShowChangeFile(void)
{
    UINT8  bID0 = 0;

    OSDDraw_GetLastOvlDrawBuffer(&bID0);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, OSD_COLOR_TRANSPARENT);
    AHC_OSDSetBkColor(bID0, OSD_COLOR_TRANSPARENT);
	#else
    AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
    AHC_OSDSetBkColor(OSD_COLOR_TRANSPARENT);
	#endif
    AHC_OSDDrawFillRect(bID0, 0, 20, 240, 150);

    if(GetPlayBackFileType() == PLAYBACK_IMAGE_TYPE)
    {
        AHC_OSDDrawFillRect(bID0, 0, 170, 240, 240);

		UIDrawHdmi_JpgPBFuncEx(bID0, HDMI_GUI_JPGPB_FILENAME, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		UIDrawHdmi_JpgPBFuncEx(bID0, HDMI_GUI_JPGPB_FILEDATE, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
		UIDrawHdmi_JpgPBFuncEx(bID0, HDMI_GUI_JPGPB_FILE_INDEX, AHC_TRUE,  AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    }
    else
    {
		UpdateHdmiMovPBStatus(MOVIE_STATE_PLAY);

   		UIDrawHdmi_MovPBFuncEx(bID0, HDMI_GUI_MOVPB_FILENAME, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    	UIDrawHdmi_MovPBFuncEx(bID0, HDMI_GUI_MOVPB_TOTAL_TIME, AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    	UIDrawHdmi_MovPBFuncEx(bID0, HDMI_GUI_MOVPB_STATUS, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    	UIDrawHdmi_MovPBFuncEx(bID0, HDMI_GUI_MOVPB_FILE_INDEX, AHC_TRUE,  AHC_TRUE, OSD_COLOR_TRANSPARENT, NULL_PARAM);
    }

#if (SHOW_HDMI_DIR_ICON)
    AHC_OSDDrawBitmap(bID0, &bmIcon_HdmiSlideShow, 100, 45);
#endif
}

#endif

#if 0
void ____HDMI_Draw_Function_____(){ruturn;} //dummy
#endif

void DrawStateHdmiBrowserUpdate(UINT32 ulEvent)
{
#if (HDMI_ENABLE)
    UINT8  bID0 = 0;
    UINT8  bID1 = 0;


    switch( ulEvent )
    {
        case EVENT_NONE                           :
        break;

        case EVENT_KEY_MODE                       :
            DrawStateHdmiBrowserSwitch();
            AHC_OSDSetActive(OVL_BUFFER_INDEX, 1);
            AHC_OSDSetActive(OVL_BUFFER_INDEX1,1);
        break;

        case EVENT_KEY_UP                         :
        case EVENT_KEY_DOWN                       :
        case EVENT_KEY_LEFT                       :
        case EVENT_KEY_RIGHT                      :
        case EVENT_HDMI_BROWSER_KEY_PAGE_UP       :
        case EVENT_HDMI_BROWSER_KEY_PAGE_DOWN     :
            DrawStateHdmiBrowserPageUpdate();
            AHC_OSDSetActive(bID0, 1);
            AHC_OSDSetActive(bID1, 1);
        break;

        case EVENT_HDMI_CHANGE_MODE_INIT          :
			DrawStateHdmiBrowserInit();
			AHC_OSDSetActive(HDMI_MENU_OSD_ID, 0);
			{
				UINT32    pos[2];
				UINT32    uwDispScale = 0;
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
				pos[0] = uwDispScale-1;
				pos[1] = uwDispScale-1;
				AHC_OSDSetDisplayAttr(HDMI_MENU_OSD_ID, AHC_OSD_ATTR_DISPLAY_SCALE, pos);
				AHC_OSDSetDisplayAttr(HDMI_MENU_OSD2_ID, AHC_OSD_ATTR_DISPLAY_SCALE, pos);
			}
			AHC_OSDSetActive(HDMI_UI_OSD_ID, 1);
			AHC_OSDSetCurrentDisplay(HDMI_UI_OSD_ID);
			AHC_OSDRefresh();
        break;

        default:
        break;
    }
	
	
#endif
}

void DrawStateHdmiMoviePBUpdate(UINT32 ulEvent)
{
#if (HDMI_ENABLE)

    UINT32 uState;
    UINT8  bID0 = 0;
    UINT8  bID1 = 0;

    OSDDraw_EnterDrawing(&bID0, &bID1);

    switch( ulEvent )
    {
        case EVENT_NONE                           :
        break;

		case EVENT_KEY_UP                       :
            AHC_GetSystemStatus(&uState);

            switch(uState)
            {
	            case STATE_MOV_IDLE :
	            break;

	            case STATE_MOV_EXECUTE :
	            	UpdateHdmiMovPBStatus(MOVIE_STATE_REW);
					DrawStateHdmiMoviePBStatus();
	            break;

	            case STATE_MOV_PAUSE   :
	            	UpdateHdmiMovPBStatus(MOVIE_STATE_PFB);
					DrawStateHdmiMoviePBStatus();
	            break;

	            default                :
	            break;
			}
        break;
			
        case EVENT_KEY_DOWN                       :
            //DrawStateHdmiMoviePBDel(m_DelFile);
            AHC_GetSystemStatus(&uState);

            switch(uState)
            {
	            case STATE_MOV_IDLE :
	            break;

	            case STATE_MOV_EXECUTE :
	            	UpdateHdmiMovPBStatus(MOVIE_STATE_FF);
					DrawStateHdmiMoviePBStatus();
	            break;

	            case STATE_MOV_PAUSE   :
	            	UpdateHdmiMovPBStatus(MOVIE_STATE_PFF);
					DrawStateHdmiMoviePBStatus();
	            break;

	            default                :
	            break;
            }
        break;

        case EVENT_KEY_LEFT                       :
            AHC_GetSystemStatus(&uState);

            switch(uState)
            {
	            case STATE_MOV_IDLE :
	            break;

	            case STATE_MOV_EXECUTE :
	            	UpdateHdmiMovPBStatus(MOVIE_STATE_REW);
					DrawStateHdmiMoviePBStatus();
	            break;

	            case STATE_MOV_PAUSE   :
	            	UpdateHdmiMovPBStatus(MOVIE_STATE_PFB);
					DrawStateHdmiMoviePBStatus();
	            break;

	            default                :
	            break;
			}
        break;

        case EVENT_KEY_RIGHT                      :
        case EVENT_KEY_CIRCLE                     :
            AHC_GetSystemStatus(&uState);

            switch(uState)
            {
	            case STATE_MOV_IDLE :
	            break;

	            case STATE_MOV_EXECUTE :
	            	UpdateHdmiMovPBStatus(MOVIE_STATE_FF);
					DrawStateHdmiMoviePBStatus();
	            break;

	            case STATE_MOV_PAUSE   :
	            	UpdateHdmiMovPBStatus(MOVIE_STATE_PFF);
					DrawStateHdmiMoviePBStatus();
	            break;

	            default                :
	            break;
            }
        break;

        case EVENT_HDMI_DEL_FILE                  :
            DrawStateHdmiMoviePBChangeFile();
        break;

        case EVENT_HDMI_BROWSER_KEY_PLAYBACK      :
            if(m_DelFile) {
                DrawStateHdmiMoviePBDel(m_DelFile+2);
                break;
            }
        break;

        case EVENT_MOVPB_TOUCH_PLAY_PRESS         :
        	UpdateHdmiMovPBStatus(MOVIE_STATE_PLAY);
			DrawStateHdmiMoviePBStatus();
        break;

        case EVENT_MOVPB_TOUCH_PAUSE_PRESS        :

			AHC_GetVideoPlayStopStatus(&uState);

			if ((AHC_VIDEO_PLAY_EOF == uState) || (AHC_VIDEO_PLAY_ERROR_STOP == uState))
				UpdateHdmiMovPBStatus(MOVIE_STATE_STOP);
			else
				UpdateHdmiMovPBStatus(MOVIE_STATE_PAUSE);

			DrawStateHdmiMoviePBStatus();
        break;

        case EVENT_HDMI_PLAY_STOP                 :
        	UpdateHdmiMovPBStatus(MOVIE_STATE_STOP);
			DrawStateHdmiMoviePBStatus();
        break;

        case EVENT_MOVPB_UPDATE_MESSAGE           :
			DrawStateHdmiMoviePBPlayInfo();
        break;

        case EVENT_HDMI_CHANGE_MODE_INIT          :
			AHC_OSDSetActive(bID0, 0);
			AHC_OSDSetActive(bID1, 0);
			DrawStateHdmiMoviePBInit(bID0, bID1);
			AHC_OSDSetActive(bID0, 1);
			AHC_OSDSetActive(bID1, 1);
        break;

        default:
        break;
    }

    OSDDraw_ExitDrawing(&bID0, &bID1);
	
#endif
}

void DrawStateHdmiPhotoPBUpdate(UINT32 ulEvent)
{
#if (HDMI_ENABLE)
    UINT8  bID0 = 0;
    UINT8  bID1 = 0;

    OSDDraw_EnterDrawing(&bID0, &bID1);

    switch( ulEvent )
    {
        case EVENT_NONE                           :
        break;

        case EVENT_KEY_DOWN                       :
			DrawStateHdmiMoviePBDel(m_DelFile);
        break;

        case EVENT_KEY_LEFT                       :
        case EVENT_KEY_RIGHT                      :
            DrawStateHdmiPhotoPBChangeFile();
        break;

        case EVENT_HDMI_DEL_FILE                  :
            DrawStateHdmiPhotoPBChangeFile();
        break;

        case EVENT_HDMI_BROWSER_KEY_PLAYBACK      :
            if(m_DelFile) {
                DrawStateHdmiMoviePBDel(m_DelFile+2);
                break;
            }
        break;

        case EVENT_HDMI_CHANGE_MODE_INIT          :
			AHC_OSDSetActive(bID0, 0);
			AHC_OSDSetActive(bID1 ,0);
			DrawStateHdmiPhotoPBInit(bID0, bID1);
			AHC_OSDSetActive(bID0, 1);
			AHC_OSDSetActive(bID1 ,1);
        break;

        default:
        break;
    }

    OSDDraw_ExitDrawing(&bID0, &bID1);
	
#endif
}

void DrawStateHdmiSlideShowUpdate(UINT32 ulEvent)
{
#if (HDMI_ENABLE)
    UINT8  bID0 = 0;
    UINT8  bID1 = 0;

    OSDDraw_EnterDrawing(&bID0, &bID1);

    switch( ulEvent )
    {
        case EVENT_NONE                           :
        break;

        case EVENT_KEY_LEFT                       :
        case EVENT_KEY_RIGHT                      :
        case EVENT_SLIDESHOW_UPDATE_FILE          :
            AHC_OSDSetActive(bID0, 0);
            AHC_OSDSetActive(bID1, 0);
            DrawStateHdmiSlideShowChangeFile();
            AHC_OSDSetActive(bID0, 1);
            AHC_OSDSetActive(bID1, 1);
        break;

        case EVENT_HDMI_CHANGE_MODE_INIT          :
			AHC_OSDSetActive(bID0, 0);
			AHC_OSDSetActive(bID1, 0);
			DrawStateHdmiSlideShowInit();
			AHC_OSDSetActive(bID0, 1);
			AHC_OSDSetActive(bID1, 1);
        break;

        case EVENT_MOVPB_UPDATE_MESSAGE			  :
			if(GetPlayBackFileType() == PLAYBACK_VIDEO_TYPE) {
				DrawStateHdmiMoviePBPlayInfo();
			}
        break;

        default:
        break;
    }

    OSDDraw_ExitDrawing(&bID0, &bID1);
	
#endif
}

void DrawStateHdmiVideoUpdate(UINT32 ulEvent)
{
#if (HDMI_ENABLE)
	DrawStateVideoRecUpdate(ulEvent);
#endif
}

void DrawStateHdmiCameraUpdate(UINT32 ulEvent)
{
#if (HDMI_ENABLE)
	DrawStateCaptureUpdate(ulEvent);
#endif
}

void DrawStateHDMINetPlaybackInit(void)
{
#if (HDMI_ENABLE)
	UINT8			bID0;           
    UINT8			bID1;        
    UINT16      	OSDWidth,OSDHeight,OSDColor;
    UINT32      	OSDAddr;
    UINT16			MaxBarLine = 135;                  
    UINT16			BarLineStartX = 90;       
    UINT16			BarLineStartY = 110;              
    const GUI_FONT  *pGuiFont = &GUI_Font20B_ASCII;
	AHC_DISPLAY_OUTPUTPANEL  OutputDevice;   


    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    OSDDraw_EnterDrawing(&bID0, &bID1);
    BEGIN_LAYER(bID0);

	AHC_OSDGetBufferAttr(bID0,&OSDWidth,&OSDHeight,&OSDColor,&OSDAddr);
    AHC_GetDisplayOutputDev(&OutputDevice);

	BarLineStartX = (AHC_GET_ATTR_OSD_W(bID0) - MaxBarLine) >> 1;
    BarLineStartY = (AHC_GET_ATTR_OSD_H(bID0) - 20) >> 1;
    pGuiFont = &GUI_Font20B_ASCII;
	
	AHC_OSDSetColor(bID0, OSD_COLOR_BLACK);
	AHC_OSDDrawFillRect(bID0, 0, 0, OSDWidth, OSDHeight);
    //Draw Text
    AHC_OSDSetColor(bID0, OSD_COLOR_WHITE);
	AHC_OSDSetFont(bID0, pGuiFont);   
    AHC_OSDDrawText(bID0, (UINT8*)"Network playback", BarLineStartX, BarLineStartY, 1);

    OSDDraw_ExitDrawing(&bID0, &bID1);

	END_LAYER(bID0);  
#endif 
}
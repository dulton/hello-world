/*===========================================================================
 * Include file 
 *===========================================================================*/ 

#include "AHC_Utility.h"
#include "AHC_MACRO.h"
#include "AHC_Menu.h"
#include "AHC_Message.h"
#include "AHC_GUI.h"
#include "AHC_General.h"
#include "AHC_Parameter.h"
#include "AHC_Display.h"
#include "AHC_Browser.h"
#include "AHC_Warningmsg.h"
#include "AHC_General_CarDV.h"
#include "MenuCommon.h"
#include "MenuTouchButton.h"
#include "MenuStateMenu.h"
#include "MenuDrawingFunc.h"
#include "MenuDrawCommon.h"
#include "MenuStateMenu.h"
#include "MenuSetting.h"
#include "IconPosition.h"
#include "ColorDefine.h"
#include "OsdStrings.h"
#include "IconDefine.h"
#include "StateBrowserFunc.h"
#ifdef _OEM_MENU_
#include "Oem_Menu.h"
#endif
#include "StateTVFunc.h"
/*===========================================================================
 * Global variable
 *===========================================================================*/ 

PSMENUSTRUCT MenuPageList[] = 
{
#if (MENU_MOVIE_PAGE_EN)
	&sMainMenuVideo,
#endif
#if (MENU_STILL_PAGE_EN)	
	&sMainMenuStill,
#endif
#if (MENU_PLAYBACK_PAGE_EN)	
	&sMainMenuPlayback,
#endif
#if (MENU_MEDIA_PAGE_EN)		
	&sMainMenuMedia,
#endif
#if (MENU_GENERAL_PAGE_EN)	
	&sMainMenuGeneral,
#endif
#if (MENU_WIFI_PAGE_EN)
	&sMainMenuWifi,
#endif
#if (EXIT_MENU_PAGE_EN)	
	&sMainMenuExit,
#endif	
};

AHC_BOOL	m_ubAtMenuTab = AHC_FALSE;

/*===========================================================================
 * Extern variable
 *===========================================================================*/ 
 
extern AHC_BOOL 	Deleting_InBrowser;
extern AHC_BOOL 	Protecting_InBrowser;
extern AHC_BOOL   	bShowHdmiWMSG;
extern AHC_BOOL   	bShowTvWMSG;
extern AHC_BOOL 	bPreviewModeTVout;
extern AHC_BOOL 	bForceSwitchBrowser;

#if (VIRTUAL_KEY_BOARD)
extern UINT32	SubMenuEventHandler_Keyboard(PSMENUSTRUCT pMenu, UINT32 ulEvent, UINT32 ulParam );
#endif

/*===========================================================================
 * Main body
 *===========================================================================*/

#if 0
void ________Touch_Function_______(){ruturn;} //dummy
#endif

#if 0
void ________MainSubMenu_Function_______(){ruturn;} //dummy
#endif

#define MENU_PAGE_NUM 	sizeof(MenuPageList)/sizeof(PSMENUSTRUCT)

PSMENUSTRUCT GetPrevCatagoryMenu(PSMENUSTRUCT pMenu)
{
	int i, Prev;
	
	for(i=0; i<MENU_PAGE_NUM; i++)
	{
		if(pMenu->iMenuId==MenuPageList[i]->iMenuId)
		{
			Prev = (i==0)?(MENU_PAGE_NUM-1):(i-1);
			return (MenuPageList[Prev]);
		}
	}

	return &sMainMenuVideo;
}

PSMENUSTRUCT GetNextCatagoryMenu(PSMENUSTRUCT pMenu)
{
	int i, Next;
	
	for(i=0; i<MENU_PAGE_NUM; i++)
	{
		if(pMenu->iMenuId==MenuPageList[i]->iMenuId)
		{
			Next = ((i+1)==MENU_PAGE_NUM)?(0):(i+1);
			return (MenuPageList[Next]);
		}
	}

	return &sMainMenuVideo;
}

int GetCatagoryMenuID(PSMENUSTRUCT pMenu)
{
    if     ( pMenu == &sMainMenuManual   ) { return 0;  }
    else if( pMenu == &sMainMenuVideo    ) { return 1;  }
    else if( pMenu == &sMainMenuStill    ) { return 2;  }
    else if( pMenu == &sMainMenuPlayback ) { return 3;  }
    else if( pMenu == &sMainMenuEdit     ) { return 4;  }
    else if( pMenu == &sMainMenuMedia    ) { return 5;  }
    else if( pMenu == &sMainMenuGeneral  ) { return 6;  }
    else if( pMenu == &sMainMenuExit	 ) { return 7;  }
#if (MENU_WIFI_PAGE_EN)
	else if( pMenu == &sMainMenuWifi	 ) { return 8;	}
#endif
    else                                   { return 0;  }
}

AHC_BOOL CommonMenuOK(PSMENUSTRUCT pMenu, AHC_BOOL bHover)
{
    INT32		 	i;
    PSMENUITEM 		pCurItem;
    PSMENUSTRUCT 	pSubMenu;

    if( pMenu == NULL )
    {
    	return AHC_FALSE;
    }

    if( bHover == AHC_FALSE )
	    i = pMenu->iSelected;
    else
	    i = pMenu->iCurrentPos;

    pCurItem = pMenu->pItemsList[i];
    pSubMenu = pCurItem->pSubMenu;

    if( pSubMenu != NULL )
    {
        pSubMenu->pParentMenu = pMenu;
        SetCurrentMenu(pSubMenu);

        pSubMenu->pfEventHandler( pSubMenu, MENU_INIT, 0 );
  
        return AHC_FALSE;
    }
    else if( pCurItem->pfItemSelect != NULL )
    {
        pCurItem->pfItemSelect( pCurItem, bHover );
        
        return AHC_TRUE;
    }
    else
    {
        return AHC_FALSE;
    }
}

UINT32 MenuGetDefault(PSMENUSTRUCT pMenu)
{
	return 0;
}

AHC_BOOL MenuModePreCheck(UINT32 ulEvent)
{
#if (TVOUT_PREVIEW_EN==0 && HDMI_PREVIEW_EN==0)

    if(BUTTON_HDMI_DETECTED == ulEvent)
    {
        if(AHC_IsHdmiConnect() && bShowHdmiWMSG)
        {
            bShowHdmiWMSG = AHC_FALSE;
            AHC_WMSG_Draw(AHC_TRUE, WMSG_HDMI_TV, 3);
        }         
    }
    
    if( BUTTON_TV_DETECTED == ulEvent)
    {
        if(AHC_IsTVConnectEx() && bShowTvWMSG) 
        {
            bShowTvWMSG = AHC_FALSE;
            AHC_WMSG_Draw(AHC_TRUE, WMSG_HDMI_TV, 3);
        }    
    }

    if(BUTTON_HDMI_REMOVED == ulEvent)
    {
        bShowHdmiWMSG = AHC_TRUE;
    }
    
    if(BUTTON_TV_REMOVED == ulEvent)
    {
        bShowTvWMSG = AHC_TRUE;
    }
#endif

    if(BUTTON_CLEAR_WMSG == ulEvent)
    {
        AHC_WMSG_Draw(AHC_FALSE, WMSG_NONE, 0);
    }
    
    return AHC_TRUE;
}

UINT32 MainMenuEventHandler(PSMENUSTRUCT pMenu, UINT32 ulEvent, UINT32 ulParam)
{
    INT32 	iPrevPos;
    POINT	TouchPoint;

    if( MENU_TOUCH == ulEvent || MENU_TOUCH_MOVE == ulEvent )
    {
        TouchPoint.PointX = ulParam & 0xFFFF;
        TouchPoint.PointY = (ulParam >> 16) & 0xFFFF;
    }
    
	switch(ulEvent)
    {
    	case MENU_LCD_NORMAL:
    	case MENU_LCD_ROTATE:
	   		AHC_DrawMenuRotateEvent(ulEvent, pMenu);
    	break;
    	
    	case MENU_SDMMC_IN:
    	case MENU_SDMMC_OUT:
			MenuDraw_SD_Status();
		break;
		
    	case MENU_BATTERY_DETECTION:
    		MenuDrawMainPage( pMenu );
    	break;
    	
		case MENU_UPDATE_MESSAGE:
	    	#if (GPS_CONNECT_ENABLE)
	    	MenuDrawSubPage_GPSInfo( pMenu ); 
	    	#endif
    	break;
    }

	switch(ulEvent)
	{
	case MENU_EXIT:
		return MENU_ERR_EXIT;
	break;
	
	case MENU_INIT:
        pMenu->uiStatus = MENU_STATUS_NONE;
		pMenu->bSetOne  = 0;
        pMenu->iCurrentPos = pMenu->iSelected;

		#if (SUPPORT_TOUCH_PANEL)
		KeyParser_TouchItemRegister(&MainMenu_TouchButton[0], ITEMNUM(MainMenu_TouchButton));
		#endif

		#if (EXIT_MENU_PAGE_EN)  
		if(pMenu == (&sMainMenuExit))
			MenuDrawExitMainPage( pMenu );
		else
		#endif
	    	MenuDrawMainPage( pMenu );
	break;
		
	case MENU_UP            :
	
        iPrevPos = pMenu->iCurrentPos;
        pMenu->iCurrentPos = OffsetItemNumber( pMenu->iCurrentPos, 0, pMenu->iNumOfItems-1, -1, AHC_TRUE );

		#if (EXIT_MENU_PAGE_EN)
        if(pMenu == (&sMainMenuExit))
        	MenuDrawChangeExitItem( pMenu, pMenu->iCurrentPos, iPrevPos);
        else
		#endif
        	MenuDrawChangeItem( pMenu, pMenu->iCurrentPos, iPrevPos, pMenu->iSelected );
	break;

	case MENU_DOWN          :

        iPrevPos = pMenu->iCurrentPos;
		pMenu->iCurrentPos = OffsetItemNumber( pMenu->iCurrentPos, 0, pMenu->iNumOfItems-1, 1, AHC_TRUE);

		#if (EXIT_MENU_PAGE_EN)
        if(pMenu == (&sMainMenuExit)) 
        	MenuDrawChangeExitItem( pMenu, pMenu->iCurrentPos, iPrevPos);
        else
		#endif
        	MenuDrawChangeItem( pMenu, pMenu->iCurrentPos, iPrevPos, pMenu->iSelected ); 
	break;

	case MENU_LEFT          :
	{
	#ifdef CFG_MENU_TOP_PAGE_SD //may be defined in config_xxx.h
		break;
	#else
	    PSMENUSTRUCT pNewMenu;

		pNewMenu = GetPrevCatagoryMenu( pMenu );
		SetCurrentMenu( pNewMenu );
		pNewMenu->pfEventHandler ( pNewMenu, MENU_INIT, 0 );
	#endif
	}
	break;
	
	case MENU_RIGHT         :
	{	
	#ifdef CFG_MENU_TOP_PAGE_SD //may be defined in config_xxx.h
		break;
	#else
	    PSMENUSTRUCT pNewMenu;

		pNewMenu = GetNextCatagoryMenu( pMenu );


		SetCurrentMenu( pNewMenu );
		pNewMenu->pfEventHandler ( pNewMenu, MENU_INIT, 0 );
	#endif
	}
	break;

	case MENU_OK            :
	case MENU_SET_ONE		:

		if(ulEvent==MENU_SET_ONE)
		{	
	        pMenu->iCurrentPos = ulParam;
	        pMenu->iSelected   = ulParam;
			pMenu->bSetOne   = 1;
	        pMenu->uiStatus |= MENU_STATUS_PRESSED;
	        pMenu->uiStatus |= MENU_STATUS_ITEM_TOUCHED;
		}
		else
		{
			pMenu->bSetOne   = 1;
			pMenu->iSelected = pMenu->iCurrentPos;
			pMenu->uiStatus |= MENU_STATUS_PRESSED;
		}

		if( CommonMenuOK( pMenu, AHC_TRUE ) == AHC_TRUE )
		{
			#if (TOP_MENU_PAGE_EN)
			
	        PSMENUSTRUCT pCurrMenu = NULL;

	        SetCurrentMenu(&sTopMenu);

	        pCurrMenu = GetCurrentMenu();

	    	if( pCurrMenu == NULL )
	    	{
	    		return MENU_ERR_EXIT;
	    	}
			
	    	pCurrMenu->pfEventHandler(pCurrMenu, MENU_INIT, 0);
	    	
			#endif
			
			return MENU_ERR_OK;
		}
	break;

	case MENU_MENU          :
    {
        PSMENUSTRUCT pParentMenu;
		
		pMenu->bSetOne     = 0;
        pParentMenu        = pMenu->pParentMenu;
        pMenu->pParentMenu = NULL;

        if( pParentMenu == NULL )
        {
	        return MENU_ERR_EXIT;
        }

        SetCurrentMenu(pParentMenu);
        pParentMenu->pfEventHandler(pParentMenu, MENU_INIT, 0);
    }
	break;

	case MENU_POWER_OFF     :
        AHC_NormalPowerOffPath();
	break; 

	case MENU_TV_DETECT:
		ResetCurrentMenu();
		
        switch(GetCurrentOpMode())
        {
            case VIDEOREC_MODE  :
                SetTVState(AHC_TV_VIDEO_PREVIEW_STATUS);
            break;
            case CAPTURE_MODE   :
                SetTVState(AHC_TV_DSC_PREVIEW_STATUS);
            break;		
            case MOVPB_MODE     :
            	SetTVState(AHC_TV_MOVIE_PB_STATUS);
            break;
            case JPGPB_MODE     :
            	SetTVState(AHC_TV_PHOTO_PB_STATUS);
            break;
            case SLIDESHOW_MODE :
            	SetTVState(AHC_TV_SLIDE_SHOW_STATUS);
            break;
            default             :
                SetTVState(AHC_TV_BROWSER_STATUS);
            break;
        }        
        
        StateSwitchMode(UI_TVOUT_STATE);
		break;

	default:
	break;
	}

    return MENU_ERR_OK;
}
#if (SUPPORT_TOUCH_PANEL)
UINT32 MainMenuItem_Touch(UINT16 pt_x,UINT16 pt_y)
{
	UINT32  uiNextEvent = MENU_NONE;
	UINT32  TouchEvent;
	UINT16  i = 0;
	PSMENUSTRUCT pMenu;
	int iPrevPos;
	
	pMenu=GetCurrentMenu();

  	i = (pt_y - OSD_MENU_ITEM_Y )/( OSD_MENU_ITEM_H + OSD_MENU_ITEM_INTERVAL );
    i = (pMenu->iCurrentPos/MAIN_MENU_PAGE_ITEM)*MAIN_MENU_PAGE_ITEM + i;
	printc("MainMenuItem_Touch %d,ofItem %d\r\n",i,pMenu->iNumOfItems);
	if(i < pMenu->iNumOfItems)
	{
		if(i ==  pMenu->iCurrentPos)
			MainMenuEventHandler(pMenu,MENU_SET_ONE,i);
		else
		{
			iPrevPos = pMenu->iCurrentPos;
			pMenu->iCurrentPos = i;
			MenuDrawChangeItem( pMenu, pMenu->iCurrentPos, iPrevPos, pMenu->iSelected );
		}
	}
	return MENU_ERR_OK;
}
#endif
UINT32 SubMenuEventHandler(PSMENUSTRUCT pMenu, UINT32 ulEvent, UINT32 ulParam)
{
    INT32 	iPrevPos;
    POINT	TouchPoint;

	switch(ulEvent)
    {
		case MENU_TV_DETECT:
			ResetCurrentMenu();
			
	        switch(GetCurrentOpMode())
	        {
	            case VIDEOREC_MODE  :
	                SetTVState(AHC_TV_VIDEO_PREVIEW_STATUS);
	            break;
	            case CAPTURE_MODE   :
	                SetTVState(AHC_TV_DSC_PREVIEW_STATUS);
	            break;
	            case MOVPB_MODE     :
	            	SetTVState(AHC_TV_MOVIE_PB_STATUS);
	            break;
	            case JPGPB_MODE     :
	            	SetTVState(AHC_TV_PHOTO_PB_STATUS);
	            break;
	            case SLIDESHOW_MODE :
	            	SetTVState(AHC_TV_SLIDE_SHOW_STATUS);
	            break;
	            default             :
	                SetTVState(AHC_TV_BROWSER_STATUS);
	            break;
	        }        
	        
	        StateSwitchMode(UI_TVOUT_STATE);
			break;
			
		#ifdef NET_SYNC_PLAYBACK_MODE
		case EVENT_NET_ENTER_PLAYBACK : 
			//ResetCurrentMenu();
            StateSwitchMode(UI_NET_PLAYBACK_STATE);
            CGI_FEEDBACK(ulEvent, 0 /* SUCCESSFULLY */);
        	break;
    	#endif//NET_SYNC_PLAYBACK_MODE
	}
	
	switch(pMenu->iMenuId)
	{	
		#if (MENU_EDIT_DELETE_ALL_EN || MENU_EDIT_PROTECT_ALL_EN || MENU_EDIT_UNPROTECT_ALL_EN)
		case MENUID_SUB_MENU_DELETE_ALL_VIDEO:
		case MENUID_SUB_MENU_DELETE_ALL_IMAGE:
		case MENUID_SUB_MENU_PROTECT_ALL_VIDEO:
		case MENUID_SUB_MENU_PROTECT_ALL_IMAGE:
		case MENUID_SUB_MENU_UNPROTECT_ALL_VIDEO:
		case MENUID_SUB_MENU_UNPROTECT_ALL_IMAGE:
			return SubMenuEventHandler_EditAllFile(pMenu, ulEvent, ulParam);
		break;	
		#endif
		
		#if (MENU_GENERAL_CLOCK_SETTING_EN)	
		case MENUID_SUB_MENU_CLOCK_SETTINGS:
			return SubMenuEventHandler_ClockSetting(pMenu, ulEvent, ulParam);
		break;
		#endif
		
		#if (MENU_GENERAL_DRIVER_ID_SETTING_EN)
		case MENUID_SUB_MENU_DRIVER_ID_SETTINGS:
			#if VIRTUAL_KEY_BOARD_FOR_CAR_ID
			return SubMenuEventHandler_Keyboard(pMenu, ulEvent, ulParam);
			#else
			return SubMenuEventHandler_DriverIdSetting(pMenu, ulEvent, ulParam);
			#endif
			break;
		#endif
		
		#if (MENU_GENERAL_RESET_SETUP_EN)
		case MENUID_SUB_MENU_RESET_SETUP:
			return SubMenuEventHandler_ResetSetup(pMenu, ulEvent, ulParam);
		break;
		#endif
		
		#if (MENU_GENERAL_FW_VERSION_EN)	
		case MENUID_SUB_MENU_FW_VERSION_INFO:
			return SubMenuEventHandler_FwVersionInfo(pMenu, ulEvent, ulParam);
		break;
		#endif
			
		#if (MENU_GENERAL_GPSINFO_EN)
		case MENUID_SUB_MENU_GPS_INFO:
			return SubMenuEventHandler_GPSInfoChart(pMenu, ulEvent, ulParam);
		break;
		#endif
		
		#if (MENU_MANUAL_EV_EN || MENU_STILL_EV_EN || MENU_MOVIE_EV_EN)
		case MENUID_SUB_MENU_EV:
			return SubMenuEventHandler_EV(pMenu, ulEvent, ulParam);
		break;
		#endif
		
		#if (MENU_MEDIA_FORMAT_SD_EN)	
		case MENUID_SUB_MENU_FORMAT_SD_CARD:
			return SubMenuEventHandler_FormatSDCard(pMenu, ulEvent, ulParam);
		break;								
		#endif
		
		#if (MENU_MEDIA_SHOW_SD_INFO)
		case MENUID_SUB_MENU_SD_CARD_INFO:
			return SubMenuEventHandler_StorageInfo(pMenu, ulEvent, ulParam);
		break;
		#endif		
		
		#if (MENU_PLAYBACK_VOLUME_EN)	
		case MENUID_SUB_MENU_VOLUME:
			return SubMenuEventHandler_Volume(pMenu, ulEvent, ulParam);
		break;
		#endif		

        #if (MENU_GENERAL_LDWS_CALIBRATION_EN)
		case MENUID_SUB_MENU_LDWS_CALIBRATION:
			return SubMenuEventHandler_LdwsCalibration(pMenu, ulEvent, ulParam);
		break;

		case MENUID_SUB_MENU_USER_LDWS_CALIBRATION:
			return SubMenuEventHandler_UserLdwsCalibration(pMenu, ulEvent, ulParam);
		break;
		#endif

		#if (VIRTUAL_KEY_BOARD)
		case MENUID_SUB_MENU_VURTUAL_KEYBOARD:
			return SubMenuEventHandler_Keyboard(pMenu, ulEvent, ulParam); 
		break;
		#endif

		#if VIRTUAL_KEY_BOARD_FOR_WIFI
		case MENUID_SUB_MENU_WIFI_AP_SSID:
		case MENUID_SUB_MENU_WIFI_AP_PSWD:
		case MENUID_SUB_MENU_WIFI_STATION1_SSID: 
		case MENUID_SUB_MENU_WIFI_STATION1_PSWD: 
		case MENUID_SUB_MENU_WIFI_STATION2_SSID:
		case MENUID_SUB_MENU_WIFI_STATION2_PSWD:
		case MENUID_SUB_MENU_WIFI_STATION3_SSID:
		case MENUID_SUB_MENU_WIFI_STATION3_PSWD:
		case MENUID_SUB_MENU_WIFI_STATION4_SSID:
		case MENUID_SUB_MENU_WIFI_STATION4_PSWD:
		case MENUID_SUB_MENU_WIFI_STATION5_SSID:
		case MENUID_SUB_MENU_WIFI_STATION5_PSWD:
		case MENUID_SUB_MENU_WIFI_STATION6_SSID:
		case MENUID_SUB_MENU_WIFI_STATION6_PSWD:
			return SubMenuEventHandler_Keyboard(pMenu, ulEvent, ulParam); 
		break;
		#endif
		
		#if (MENU_MANUAL_CONTRAST_EN || MENU_STILL_CONTRAST_EN || MENU_MOVIE_CONTRAST_EN)
		case MENUID_SUB_MENU_CONTRAST:
			return SubMenuEventHandler_Contrast(pMenu, ulEvent, ulParam);
		break;
		#endif
		
		#if (MENU_MANUAL_SATURATION_EN || MENU_STILL_SATURATION_EN || MENU_MOVIE_SATURATION_EN)
		case MENUID_SUB_MENU_SATURATION:
			return SubMenuEventHandler_Saturation(pMenu, ulEvent, ulParam);
		break;
		#endif
		
		#if (MENU_MANUAL_SHARPNESS_EN || MENU_STILL_SHARPNESS_EN || MENU_MOVIE_SHARPNESS_EN)
		case MENUID_SUB_MENU_SHARPNESS:
			return SubMenuEventHandler_Sharpness(pMenu, ulEvent, ulParam);
		break;
		#endif
		
		#if (MENU_MANUAL_GAMMA_EN || MENU_STILL_GAMMA_EN || MENU_MOVIE_GAMMA_EN)
		case MENUID_SUB_MENU_GAMMA:
			return SubMenuEventHandler_Gamma(pMenu, ulEvent, ulParam);
		break;
		#endif

		default:
		break;	
	}

    if( MENU_TOUCH == ulEvent || MENU_TOUCH_MOVE == ulEvent )
    {
        TouchPoint.PointX = ulParam & 0xFFFF;
        TouchPoint.PointY = (ulParam >> 16) & 0xFFFF;
    }

	switch(ulEvent)
    {
    	case MENU_LCD_NORMAL:
    	case MENU_LCD_ROTATE:
	   		AHC_DrawMenuRotateEvent(ulEvent, pMenu);
    	break; 
    	   
    	case MENU_SDMMC_IN:
    	case MENU_SDMMC_OUT:
			MenuDraw_SD_Status();
		break;
    	
    	case MENU_BATTERY_DETECTION:
    		MenuDrawSubPage(pMenu);
    	break;
		
		case MENU_UPDATE_MESSAGE:
	    	#if (GPS_CONNECT_ENABLE)
	    	MenuDrawSubPage_GPSInfo(pMenu); 
	    	#endif
    	break;
    }

	switch(ulEvent)
	{
	case MENU_EXIT:
		return MENU_ERR_EXIT;
	break;	
	
	case MENU_INIT          :
        pMenu->uiStatus = MENU_STATUS_NONE;
	    
	    if( pMenu->pfMenuGetDefaultVal )
	    {
		    pMenu->iSelected = pMenu->pfMenuGetDefaultVal(pMenu);
	    }
	    else
	    {
			pMenu->iSelected = 0 ;
	    }
        pMenu->iCurrentPos = pMenu->iSelected;
		pMenu->bSetOne     = 0;


		#if (SUPPORT_TOUCH_PANEL)
        if( pMenu->iNumOfItems <= 2 )
			KeyParser_TouchItemRegister(&SubMenu2_TouchButton[0], ITEMNUM(SubMenu2_TouchButton));
        else if( pMenu->iNumOfItems <= 4 )
         	KeyParser_TouchItemRegister(&SubMenu4_TouchButton[0], ITEMNUM(SubMenu4_TouchButton));
        else
          	KeyParser_TouchItemRegister(&SubMenu6_TouchButton[0], ITEMNUM(SubMenu6_TouchButton));
		#endif			
			
	    MenuDrawSubPage(pMenu);
	break;

	case MENU_UP            :

		#ifdef SLIDE_MENU
		if(IsSlidingMenu())
			break;
		#endif
		
        iPrevPos = pMenu->iCurrentPos;

	#if (DIR_KEY_TYPE==KEY_TYPE_2KEY)
		pMenu->iCurrentPos = OffsetItemNumber(pMenu->iCurrentPos, 0, pMenu->iNumOfItems-1, -1, AHC_TRUE);
	#elif defined(FONT_LARGE)
		pMenu->iCurrentPos = OffsetItemNumber(pMenu->iCurrentPos, 0, pMenu->iNumOfItems-1, -1, AHC_FALSE);
	#else
		pMenu->iCurrentPos = OffsetItemNumber(pMenu->iCurrentPos, 0, pMenu->iNumOfItems-1, -2, AHC_FALSE);
	#endif
        MenuDrawChangeSubItem(pMenu, pMenu->iCurrentPos, iPrevPos, pMenu->iSelected);
	break;

	case MENU_DOWN          :

		#ifdef SLIDE_MENU
		if(IsSlidingMenu())
			break;
		#endif

        iPrevPos = pMenu->iCurrentPos;

	#if (DIR_KEY_TYPE==KEY_TYPE_2KEY)
		pMenu->iCurrentPos = OffsetItemNumber(pMenu->iCurrentPos, 0, pMenu->iNumOfItems-1, 1, AHC_TRUE);
	#elif defined(FONT_LARGE)
		pMenu->iCurrentPos = OffsetItemNumber(pMenu->iCurrentPos, 0, pMenu->iNumOfItems-1, 1, AHC_FALSE);
	#else
		pMenu->iCurrentPos = OffsetItemNumber(pMenu->iCurrentPos, 0, pMenu->iNumOfItems-1, 2, AHC_FALSE);
	#endif
		
        MenuDrawChangeSubItem(pMenu, pMenu->iCurrentPos, iPrevPos, pMenu->iSelected);
	break;
	
#if (DIR_KEY_TYPE==KEY_TYPE_4KEY)
	  
	case MENU_LEFT          :

		#ifdef SLIDE_MENU
		if(IsSlidingMenu())
			break;
		#endif

        iPrevPos = pMenu->iCurrentPos;
		pMenu->iCurrentPos = OffsetItemNumber(pMenu->iCurrentPos, 0, pMenu->iNumOfItems-1, -1, AHC_FALSE);
		
        MenuDrawChangeSubItem(pMenu, pMenu->iCurrentPos, iPrevPos, pMenu->iSelected);
	break;

	case MENU_RIGHT          :

		#ifdef SLIDE_MENU
		if(IsSlidingMenu())
			break;
		#endif

        iPrevPos = pMenu->iCurrentPos;
		pMenu->iCurrentPos = OffsetItemNumber(pMenu->iCurrentPos, 0, pMenu->iNumOfItems-1, 1, AHC_FALSE);
		
        MenuDrawChangeSubItem(pMenu, pMenu->iCurrentPos, iPrevPos, pMenu->iSelected);
	break;
#endif
		
	case MENU_OK            :
	case MENU_SET_ONE		:
	
		pMenu->bSetOne   = 1;
        iPrevPos      	 = pMenu->iCurrentPos;
        
        if(ulEvent==MENU_SET_ONE)
        {         
	        pMenu->iCurrentPos = ulParam;
	        pMenu->iSelected   = ulParam;
	        pMenu->uiStatus |= MENU_STATUS_PRESSED;
	        pMenu->uiStatus |= MENU_STATUS_ITEM_TOUCHED;     
        }
        else
        {
	        pMenu->iSelected = pMenu->iCurrentPos;
	        pMenu->uiStatus |= MENU_STATUS_PRESSED;
        }
        
        if( CommonMenuOK( pMenu, AHC_TRUE ) == AHC_TRUE )
        {			
			if(Deleting_InBrowser || Protecting_InBrowser)
			{
				printc("Delete/Protect/UnProtect Change to Browser Mode2\r\n");
				// To check is there file in card, it no any file
				// to reset Delete/(Un)Protect file in broswer flags.
				// Otherwise those flag will make key/device events to
				// be NULL!! UI will be stuck
				BrowserFunc_ThumbnailEditCheckFileObj();

				if(TVFunc_IsInTVMode()/*HDMIFunc_IsInHdmiMode()*/)
				{
					Deleting_InBrowser = 0;
					Protecting_InBrowser = 0;
				}
				
				return MENU_ERR_EXIT;
			}
			else
			{

				PSMENUSTRUCT pParentMenu;
				
        		pParentMenu        = pMenu->pParentMenu;
        		pMenu->pParentMenu = NULL;

        		if( pParentMenu == NULL )
        		{
	        		return MENU_ERR_EXIT;
        		}
        		SetCurrentMenu(pParentMenu);
        		pParentMenu->pfEventHandler(pParentMenu, MENU_INIT, 0);	
        		
			}

            return MENU_ERR_OK;
        }
	break;

	case MENU_MENU          :
    {     
		{
	        PSMENUSTRUCT pParentMenu;
			
			pMenu->bSetOne     = 0;
	        pParentMenu        = pMenu->pParentMenu;
	        pMenu->pParentMenu = NULL;

	        if( pParentMenu == NULL )
	        {
		        return MENU_ERR_EXIT;
	        }
	        
	        SetCurrentMenu(pParentMenu);
	        pParentMenu->pfEventHandler(pParentMenu, MENU_INIT, 0);
        }
    }
	break;

	case MENU_POWER_OFF     :
        AHC_NormalPowerOffPath();
	break; 

	default:
	break;
	}

    return MENU_ERR_OK;
}
#if (SUPPORT_TOUCH_PANEL)
UINT32 SubMenuItem_Touch(UINT16 pt_x,UINT16 pt_y)
{
	UINT16  i = 0;
	PSMENUSTRUCT pMenu;
	int iPrevPos;
	UINT32 ret = MENU_ERR_OK;
	
	pMenu=GetCurrentMenu();

    if( pMenu->iNumOfItems <= 2 )
    {
		i =  ( pt_x- OSD_SUBMENU20_X )/( OSD_SUBMENU20_W + OSD_MENU_SUBITEM2X_INTERVAL );
    }
    else if( pMenu->iNumOfItems <= 4 )
    {
		i =  (( pt_x- OSD_SUBMENU40_X )/( OSD_SUBMENU40_W + OSD_MENU_SUBITEM4X_INTERVAL )
			+(( pt_y - OSD_SUBMENU40_Y )/( OSD_SUBMENU40_H+ OSD_MENU_SUBITEM4Y_INTERVAL ))*2 );
    }
    else
    {
		i =  (( pt_x- OSD_SUBMENU60_X )/( OSD_SUBMENU60_W + OSD_MENU_SUBITEM6X_INTERVAL )
            +(( pt_y - OSD_SUBMENU60_Y )/( OSD_SUBMENU60_H + OSD_MENU_ITEM_INTERVAL ))*2 );

		i = (pMenu->iCurrentPos/SUB_MENU_PAGE_ITEM)*SUB_MENU_PAGE_ITEM + i;
    }
	printc("SubMenuItem_Touch %d,ofItem %d\r\n",i,pMenu->iNumOfItems);

	if(i < pMenu->iNumOfItems)
	{
		if(i ==  pMenu->iCurrentPos)
			ret = SubMenuEventHandler(pMenu,MENU_SET_ONE,i);
		else
		{
			iPrevPos = pMenu->iCurrentPos;
			pMenu->iCurrentPos = i;
			MenuDrawChangeSubItem( pMenu, pMenu->iCurrentPos, iPrevPos, pMenu->iSelected );
		}
	}
	printc(FG_RED("SubMenuItem_Touch ret %d\r\n"),ret);
	if(ret == MENU_ERR_EXIT)
	{
		#ifdef SLIDE_MENU
		if(IsSlidingMenu())
			StopSlideMenu();
		#endif    
    
        ResetCurrentMenu();

		Menu_WriteSetting();
        
        if(Deleting_InBrowser || Protecting_InBrowser)
        {
        	bForceSwitchBrowser = AHC_TRUE;
        	StateSwitchMode(UI_BROWSER_STATE);
        }
       
    }	
	return MENU_ERR_OK;	
}

UINT32  SubMenuComfirm_Touch(UINT16 pt_x,UINT16 pt_y)
{
	UINT16  i = 0;
	UINT32 uParam;
	UINT32 ret = MENU_ERR_OK;
	PSMENUSTRUCT pMenu;
	RECT rc = RECT_TOUCH_BUTTON_MENU_YES;
	#define STR_GAP		(30)
	
	pMenu = GetCurrentMenu();
	i = (pt_x -rc.uiLeft)/(rc.uiWidth+ STR_GAP);
	if(i==0)
		uParam = 0;
	else
		uParam = 1;
	ret = pMenu->pfEventHandler(pMenu,MENU_SET_ONE,uParam);
	
	return MENU_ERR_OK;
}

#endif

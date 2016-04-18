/*===========================================================================
 * Include file 
 *===========================================================================*/ 
 
#include "AHC_Display.h"
#include "AHC_MACRO.h"
#include "AHC_Menu.h"
#include "AHC_Common.h"
#include "AHC_GUI.h"
#include "AHC_Utility.h"
#include "AHC_Video.h"
#include "AHC_WarningMSG.h"
#include "AHC_Parameter.h"
#include "AHC_DCF.h"
#include "AHC_UF.h"
#include "AHC_OS.h"
#include "AHC_Message.h"
#include "AHC_General_CarDV.h"
#include "MenuCommon.h"
#include "MenuStateModeSelectMenu.h"
#include "MenuStateGeneralSettingsMenu.h"
#include "MenuSetting.h"
#include "MenuStateMenu.h"
#include "MenuDrawCommon.h"
#include "MenuDrawingFunc.h"
#include "IconPosition.h"
#include "ColorDefine.h"
#include "OsdStrings.h"
#include "IconDefine.h"
#include "LEDControl.h"
#include "dsc_Charger.h"

/*===========================================================================
 * Local function
 *===========================================================================*/ 

#if (USB_MODE_SELECT_EN)
AHC_BOOL USBSelectModeMSDC(PSMENUITEM pItem, AHC_BOOL bIsHover);
AHC_BOOL USBSelectModePCam(PSMENUITEM pItem, AHC_BOOL bIsHover);
AHC_BOOL USBSelectModeDSC(PSMENUITEM pItem, AHC_BOOL bIsHover);
AHC_BOOL USBSelectModeDV(PSMENUITEM pItem, AHC_BOOL bIsHover);
#endif

#if (UI_MODE_SELECT_EN)
AHC_BOOL UISelectModeMenu(PSMENUITEM pItem, AHC_BOOL bIsHover);
AHC_BOOL UISelectModeDSC(PSMENUITEM pItem, AHC_BOOL bIsHover);
AHC_BOOL UISelectModeDV(PSMENUITEM pItem, AHC_BOOL bIsHover);
AHC_BOOL UISelectModePhotoBrowser (PSMENUITEM pItem, AHC_BOOL bIsHover);
AHC_BOOL UISelectModePhotoPlayback(PSMENUITEM pItem, AHC_BOOL bIsHover);
AHC_BOOL UISelectModeVideoBrowser (PSMENUITEM pItem, AHC_BOOL bIsHover);
AHC_BOOL UISelectModeVideoPlayback(PSMENUITEM pItem, AHC_BOOL bIsHover);
AHC_BOOL UISelectModeBrowserSelect(PSMENUITEM pItem, AHC_BOOL bIsHover);
#endif

#if (BROWSER_MODE_SELECT_EN)
AHC_BOOL BrowserSelectModeEventBrowser(PSMENUITEM pItem, AHC_BOOL bIsHover);
AHC_BOOL BrowserSelectModeVideoBrowser(PSMENUITEM pItem, AHC_BOOL bIsHover);
AHC_BOOL BrowserSelectModePhotoBrowser(PSMENUITEM pItem, AHC_BOOL bIsHover);
#endif

UINT32 ModeMenuEventHandler(PSMENUSTRUCT pMenu, UINT32 ulEvent, UINT32 ulParam);

/*===========================================================================
 * Global variable : Item Structure
 *===========================================================================*/ 

#if (USB_MODE_SELECT_EN)
SMENUITEM sItemUSB_MSDC_Mode     = { ITEMID_USB_MSDC_MODE, 	&bmIcon_MSDC,  		  IDS_MSDC_MODE,	NULL, USBSelectModeMSDC, 	0, AHC_TRUE, NULL };
SMENUITEM sItemUSB_PCam_Mode     = { ITEMID_USB_PCAM_MODE,  &bmIcon_PCCAM, 		  IDS_PCAM_MODE,  	NULL, USBSelectModePCam, 	0, AHC_TRUE, NULL };
SMENUITEM sItemUSB_DSC_Mode      = { ITEMID_USB_DSC_MODE,  	&bmIcon_Button_Still, IDS_DSC_MODE,  	NULL, USBSelectModeDSC, 	0, AHC_TRUE, NULL };
SMENUITEM sItemUSB_DV_Mode       = { ITEMID_USB_DV_MODE,  	&bmIcon_Button_Movie, IDS_DV_MODE,  	NULL, USBSelectModeDV, 		0, AHC_TRUE, NULL };
#endif

#if (UI_MODE_SELECT_EN)
SMENUITEM sItemUI_Menu_Mode     = { ITEMID_UI_MENU_MODE, 	&BMICON_MODE_SEL_MENU, 			IDS_DS_MENU,				NULL, UISelectModeMenu, 			0, AHC_TRUE, NULL };
SMENUITEM sItemUI_DV_Mode       = { ITEMID_UI_DV_MODE,  	&bmIcon_Button_Movie, 			IDS_DS_RECORD,  			NULL, UISelectModeDV, 				0, AHC_TRUE, NULL };
SMENUITEM sItemUI_DSC_Mode      = { ITEMID_UI_DSC_MODE,  	&bmIcon_Button_Still, 			IDS_DS_MODE_TAKE_PHOTO,  	NULL, UISelectModeDSC, 				0, AHC_TRUE, NULL };
SMENUITEM sItemUI_StillBMode    = { ITEMID_UI_STILLB_MODE, 	&bmIcon_PhotoBrowser, 			IDS_DS_MODE_BROWSER_PHOTO,	NULL, UISelectModePhotoBrowser , 	0, AHC_TRUE, NULL };
SMENUITEM sItemUI_StillPMode    = { ITEMID_UI_STILLP_MODE, 	&bmIcon_Dummy, 					IDS_DS_MODE_PLAYBACK_PHOTO,	NULL, UISelectModePhotoPlayback, 	0, AHC_TRUE, NULL };
SMENUITEM sItemUI_VideoBMode    = { ITEMID_UI_VIDEOB_MODE, 	&bmIcon_VideoBroswer,		 	IDS_DS_MODE_BROWSER_VIDEO,	NULL, UISelectModeVideoBrowser , 	0, AHC_TRUE, NULL };
SMENUITEM sItemUI_VideoPMode    = { ITEMID_UI_VIDEOP_MODE, 	&bmIcon_Dummy, 					IDS_DS_MODE_PLAYBACK_VIDEO,	NULL, UISelectModeVideoPlayback, 	0, AHC_TRUE, NULL };
SMENUITEM sItemUI_BrowserSelect	= { ITEMID_UI_BROWSER_MODE, &BMICON_MODE_SEL_PLAYBACK, 		IDS_DS_PLAYBACK,			NULL, UISelectModeBrowserSelect,    0, AHC_TRUE, NULL };
#endif

#if (BROWSER_MODE_SELECT_EN)
SMENUITEM sItemBrowser_Event    = { ITEMID_BROWSER_EVENT, 	&bmIcon_PhotoBrowser, 			IDS_DS_MODE_BROWSER_EVENT,	NULL, BrowserSelectModeEventBrowser , 	0, AHC_TRUE, NULL };
SMENUITEM sItemBrowser_Video    = { ITEMID_BROWSER_VIDEO, 	&bmIcon_VideoBroswer, 			IDS_DS_MODE_BROWSER_VIDEO,	NULL, BrowserSelectModeVideoBrowser, 	0, AHC_TRUE, NULL };
SMENUITEM sItemBrowser_Photo    = { ITEMID_BROWSER_PHOTO, 	&bmIcon_PhotoBrowser,		 	IDS_DS_MODE_BROWSER_PHOTO,	NULL, BrowserSelectModePhotoBrowser , 	0, AHC_TRUE, NULL };
#endif

/*===========================================================================
 * Global variable : Item List
 *===========================================================================*/ 

#if (USB_MODE_SELECT_EN)
PSMENUITEM   sMenuListUSBModeSelect[] =
{
    &sItemUSB_MSDC_Mode,
    &sItemUSB_PCam_Mode,  
    //&sItemUSB_DSC_Mode,
    &sItemUSB_DV_Mode  
};
#endif

#if (UI_MODE_SELECT_EN)
PSMENUITEM   sMenuListUIModeSelect[] =
{
    &sItemUI_Menu_Mode,   
    &sItemUI_DV_Mode,  
	&sItemUI_DSC_Mode, 
    &sItemUI_StillBMode,
    &sItemUI_StillPMode,
    &sItemUI_VideoBMode,
    &sItemUI_VideoPMode
};
#endif

#if (BROWSER_MODE_SELECT_EN)
PSMENUITEM   sMenuListBrowserModeSelect[] =
{  
    &sItemBrowser_Event,  
	&sItemBrowser_Video, 
    &sItemBrowser_Photo
};
#endif

/*===========================================================================
 * Global varible : Menu Structure
 *===========================================================================*/

#if (USB_MODE_SELECT_EN)
SMENUSTRUCT   sUSBModeSelectMenu =
{
    MENUID_USB_MODE_SELECT,                                 
    NULL,                                               
    IDS_USB_FUNCTION,	                                
    NULL,                                               
    sizeof( sMenuListUSBModeSelect )/sizeof(PSMENUITEM),
    sMenuListUSBModeSelect,                            
    ModeMenuEventHandler,                          		
    NULL,                                     			
    0,                                                 
    IDS_USB_FUNCTION,                                   
#if (USB_MODE_SELECT_STYLE==MENU_2ND_STYLE)    
    AHC_FALSE,                                          
#else
    2, /* display on center of screen */				
#endif
    0		                                            
};
#endif

#if (UI_MODE_SELECT_EN)
SMENUSTRUCT   sUIModeSelectMenu =
{
    MENUID_MODE_SELECT,                                
    NULL,                                               
    IDS_DS_MODE,	                                	
    NULL,                                               
    sizeof( sMenuListUIModeSelect )/sizeof(PSMENUITEM),  
    sMenuListUIModeSelect,                               
    ModeMenuEventHandler,	                           
    NULL,                                     		
    0,                                                 
    0,                                   			
    0,                                          		
    0		                                            
};
#endif

#if (BROWSER_MODE_SELECT_EN)
SMENUSTRUCT   sBrowserModeSelectMenu =
{
    MENUID_MAIN_MENU_BROWSER,                                
    NULL,                                               
    IDS_DS_MODE,	                                	
    NULL,                                               
    sizeof( sMenuListBrowserModeSelect )/sizeof(PSMENUITEM),  
    sMenuListBrowserModeSelect,                               
    ModeMenuEventHandler,	                           
    NULL,                                     		
    0,                                                 
    0,                                   			
    0,                                          		
    0		                                            
};
#endif

/*===========================================================================
 * Global varible
 *===========================================================================*/ 

#if (USB_MODE_SELECT_EN)
UINT8	 ubUSBSelectedMode	= USB_MAX_MODE;
#endif
#if (UI_MODE_SELECT_EN)
UINT8	 ubUISelectedMode	= UI_MAX_MODE;
#endif
#if (BROWSER_MODE_SELECT_EN)
UINT8	 ubBrowserSelected	= BROWSER_MAX_MODE;
#endif

UINT8 	 ubModeDrawStyle    = 0;

/*===========================================================================
 * Main body
 *===========================================================================*/ 

#if 0
void ____USB_Function_____(){ruturn;} //dummy
#endif

#if (USB_MODE_SELECT_EN)
AHC_BOOL USBSelectModeMSDC(PSMENUITEM pItem, AHC_BOOL bIsHover)
{
	MenuSettingConfig()->uiUSBFunction = USB_MSDC;
    ubUSBSelectedMode = USB_MSDC_MODE;
    return AHC_TRUE;
}

AHC_BOOL USBSelectModePCam(PSMENUITEM pItem, AHC_BOOL bIsHover)
{
	MenuSettingConfig()->uiUSBFunction = USB_PCAM;
    ubUSBSelectedMode = USB_PCAM_MODE;
    return AHC_TRUE;
}

AHC_BOOL USBSelectModeDSC(PSMENUITEM pItem, AHC_BOOL bIsHover)
{
	ubUSBSelectedMode = USB_DSC_MODE;     
    return AHC_TRUE;
}

AHC_BOOL USBSelectModeDV(PSMENUITEM pItem, AHC_BOOL bIsHover)
{
	ubUSBSelectedMode = USB_DV_MODE;  
    return AHC_TRUE;
}

AHC_BOOL USBFunctionHandler(UINT8 selection)
{
    switch (selection)
    {
        case USB_MSDC_MODE:
            StateSwitchMode(UI_MSDC_STATE);
            break;
        case USB_PCAM_MODE:
            StateSwitchMode(UI_PCCAM_STATE);
            break;	
        case USB_DSC_MODE:
            StateSwitchMode(UI_CAMERA_STATE);
            break;
        case USB_DV_MODE:
            StateSwitchMode(UI_VIDEO_STATE);
            break;
        default:
            StateSwitchMode(UI_MSDC_STATE);
            break;
    }
    return AHC_TRUE;
    }
#endif

#if 0
void ____UI_Function_____(){ruturn;} //dummy
#endif

#if (UI_MODE_SELECT_EN)
AHC_BOOL UISelectModeMenu(PSMENUITEM pItem, AHC_BOOL bIsHover)
{
	ubUISelectedMode = UI_MENU_MODE;
	return AHC_TRUE;
}

AHC_BOOL UISelectModeDSC(PSMENUITEM pItem, AHC_BOOL bIsHover)
{
	ubUISelectedMode = UI_DSC_MODE;
	return AHC_TRUE;
}

AHC_BOOL UISelectModeDV(PSMENUITEM pItem, AHC_BOOL bIsHover)
{
	ubUISelectedMode = UI_DV_MODE;
	return AHC_TRUE; 
}

AHC_BOOL UISelectModePhotoBrowser(PSMENUITEM pItem, AHC_BOOL bIsHover)
{
	ubUISelectedMode = UI_STILLB_MODE;
    return AHC_TRUE;
}

AHC_BOOL UISelectModePhotoPlayback(PSMENUITEM pItem, AHC_BOOL bIsHover)
{
	ubUISelectedMode = UI_STILLP_MODE;
    return AHC_TRUE;
}

AHC_BOOL UISelectModeVideoBrowser(PSMENUITEM pItem, AHC_BOOL bIsHover)
{
	ubUISelectedMode = UI_VIDEOB_MODE; 
    return AHC_TRUE;
}

AHC_BOOL UISelectModeVideoPlayback(PSMENUITEM pItem, AHC_BOOL bIsHover)
{
	ubUISelectedMode = UI_VIDEOP_MODE;
    return AHC_TRUE;
}

AHC_BOOL UISelectModeBrowserSelect(PSMENUITEM pItem, AHC_BOOL bIsHover)
{
	ubUISelectedMode = UI_BROW_SELECT_MODE;
    return AHC_TRUE;
}

AHC_BOOL UIFunctionHandler(UINT8 selection)
{
	switch (selection)
	{
		case UI_MENU_MODE:
			StateSwitchMode(UI_VIDEO_MENU_STATE);
		break;
	
		case UI_DSC_MODE:
			StateSwitchMode(UI_CAMERA_STATE);
		break;
	
		case UI_DV_MODE:
			StateSwitchMode(UI_VIDEO_STATE);
		break;
		
		case UI_STILLB_MODE:
   	 		AHC_SetMode( AHC_MODE_IDLE );
    		SetCurrentOpMode(JPGPB_MODE);
    		AHC_UF_SetFreeChar(0,DCF_SET_ALLOWED, SEARCH_PHOTO);
    		StateSwitchMode(UI_BROWSER_STATE);	
		break;

		case UI_STILLP_MODE:
			AHC_PlayMediaFiles(JPGPB_MODE);
		break;

		case UI_VIDEOB_MODE:
    		AHC_SetMode( AHC_MODE_IDLE );  
    		SetCurrentOpMode(MOVPB_MODE);
    		AHC_UF_SetFreeChar(0,DCF_SET_ALLOWED, SEARCH_MOVIE);
    		StateSwitchMode(UI_BROWSER_STATE);  
		break;

		case UI_VIDEOP_MODE:
			AHC_PlayMediaFiles(MOVPB_MODE);
		break;						

		case UI_ALLB_MODE:
    		AHC_SetMode( AHC_MODE_IDLE );  
    		SetCurrentOpMode(JPGPB_MOVPB_MODE);
    		AHC_UF_SetFreeChar(0,DCF_SET_ALLOWED, SEARCH_PHOTO_MOVIE);
    		StateSwitchMode(UI_BROWSER_STATE);  
		break;

		case UI_ALLP_MODE:
			AHC_PlayMediaFiles(JPGPB_MOVPB_MODE);
		break;					

		case UI_BROW_SELECT_MODE:
			StateSwitchMode(UI_BROWSELECT_MENU_STATE);
		break;
		
		default:
			StateSwitchMode(UI_VIDEO_STATE);
		break;
	}
	return AHC_TRUE;
}
#endif

#if 0
void ____Browser_Function_____(){ruturn;} //dummy
#endif

#if (BROWSER_MODE_SELECT_EN)
AHC_BOOL BrowserSelectModeEventBrowser(PSMENUITEM pItem, AHC_BOOL bIsHover)
{
	ubBrowserSelected = BROWSER_EVENT_MODE;
    return AHC_TRUE;
}

AHC_BOOL BrowserSelectModeVideoBrowser(PSMENUITEM pItem, AHC_BOOL bIsHover)
{
	ubBrowserSelected = BROWSER_VIDEO_MODE;
    return AHC_TRUE;
}

AHC_BOOL BrowserSelectModePhotoBrowser(PSMENUITEM pItem, AHC_BOOL bIsHover)
{
	ubBrowserSelected = BROWSER_PHOTO_MODE;
    return AHC_TRUE;
}

AHC_BOOL BrowserFunctionHandler(UINT8 selection)
{
	extern AHC_BOOL bForceSwitchBrowser;

	switch (selection)
	{
		case BROWSER_EVENT_MODE:
			AHC_SetFindLockFileOnlyFlag(AHC_TRUE);
    		bForceSwitchBrowser  = AHC_TRUE;  
    		SetCurrentOpMode(MOVPB_MODE);
    		AHC_UF_SetFreeChar(0,DCF_SET_ALLOWED, (UINT8*)SEARCH_MOVIE);
    		StateSwitchMode(UI_BROWSER_STATE); 
		break;
	
		case BROWSER_VIDEO_MODE:
    		bForceSwitchBrowser = AHC_TRUE;  
    		SetCurrentOpMode(MOVPB_MODE);
    		AHC_UF_SetFreeChar(0,DCF_SET_ALLOWED, (UINT8*)SEARCH_MOVIE);
    		StateSwitchMode(UI_BROWSER_STATE);  
		break;					
		
		case BROWSER_PHOTO_MODE: 
		   	bForceSwitchBrowser = AHC_TRUE;  
    		SetCurrentOpMode(JPGPB_MODE);
    		AHC_UF_SetFreeChar(0,DCF_SET_ALLOWED, (UINT8*)SEARCH_PHOTO);
    		StateSwitchMode(UI_BROWSER_STATE);	
		break;

		case BROWSER_ALL_MODE: 
		   	bForceSwitchBrowser = AHC_TRUE;  
    		SetCurrentOpMode(JPGPB_MOVPB_MODE);
    		AHC_UF_SetFreeChar(0,DCF_SET_ALLOWED, (UINT8*)SEARCH_PHOTO_MOVIE);
    		StateSwitchMode(UI_BROWSER_STATE);	
		break;
		
		default:
			StateSwitchMode(UI_BROWSER_STATE);
		break;
	}

	return AHC_TRUE;
}
#endif

#if 0
void ____Event_Function_____(){ruturn;} //dummy
#endif

#if (USB_MODE_SELECT_EN || UI_MODE_SELECT_EN || BROWSER_MODE_SELECT_EN)
UINT32 ModeMenuEventHandler(PSMENUSTRUCT pMenu, UINT32 ulEvent, UINT32 ulParam)
{
    INT32 iPrevPos;

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
    		MenuDrawMainPage_ModeSelect( pMenu );
    	break;
		
		case MENU_UPDATE_MESSAGE:
	    	#if (GPS_CONNECT_ENABLE)
	    	MenuDrawSubPage_GPSInfo( pMenu ); 
	    	#endif
    	break;
    }

	switch(ulEvent)
	{
	case MENU_INIT          :
        pMenu->uiStatus = MENU_STATUS_NONE;
	    
	    if( pMenu->pfMenuGetDefaultVal )
	    {
		    pMenu->iSelected = pMenu->pfMenuGetDefaultVal( pMenu );
	    }
	    else
	    {
			pMenu->iSelected = 0 ;
	    }
        pMenu->iCurrentPos = pMenu->iSelected;
		pMenu->bSetOne     = 0;
		
	    MenuDrawMainPage_ModeSelect( pMenu );
	break;

	case MENU_UP            :
        iPrevPos = pMenu->iCurrentPos;
		pMenu->iCurrentPos = OffsetItemNumber( pMenu->iCurrentPos, 0, pMenu->iNumOfItems-1, -1, AHC_TRUE );		
		
        MenuDrawChangeItem_ModeSelect( pMenu, pMenu->iCurrentPos, iPrevPos, pMenu->iSelected );
	break;

	case MENU_DOWN          :
        iPrevPos = pMenu->iCurrentPos;
		pMenu->iCurrentPos = OffsetItemNumber( pMenu->iCurrentPos, 0, pMenu->iNumOfItems-1, 1, AHC_TRUE);
				
        MenuDrawChangeItem_ModeSelect( pMenu, pMenu->iCurrentPos, iPrevPos, pMenu->iSelected );
	break;
	
#if(DIR_KEY_TYPE==KEY_TYPE_4KEY)	
	case MENU_LEFT          :

        iPrevPos = pMenu->iCurrentPos;
		pMenu->iCurrentPos = OffsetItemNumber( pMenu->iCurrentPos, 0, pMenu->iNumOfItems-1, -1, AHC_TRUE );	
        MenuDrawChangeItem_ModeSelect( pMenu, pMenu->iCurrentPos, iPrevPos, pMenu->iSelected );
	break;

	case MENU_RIGHT          :

        iPrevPos = pMenu->iCurrentPos;
		pMenu->iCurrentPos = OffsetItemNumber( pMenu->iCurrentPos, 0, pMenu->iNumOfItems-1, 1, AHC_TRUE);		
        MenuDrawChangeItem_ModeSelect( pMenu, pMenu->iCurrentPos, iPrevPos, pMenu->iSelected );
	break;
#endif
		
	case MENU_OK            :
    {
        UINT32 uiPrevSelected = pMenu->iSelected;

		pMenu->bSetOne   = 1;
        iPrevPos      	 = pMenu->iCurrentPos;
        pMenu->iSelected = pMenu->iCurrentPos;
        pMenu->uiStatus |= MENU_STATUS_PRESSED;
        
        MenuDrawChangeItem_ModeSelect( pMenu, pMenu->iCurrentPos, iPrevPos, uiPrevSelected );

        if( CommonMenuOK( pMenu, AHC_TRUE ) == AHC_TRUE )
        {
            return MENU_ERR_EXIT;           
        }
    }
	break;

	case MENU_MENU          :
    case MENU_MODE_PRESS	:
    {
        PSMENUSTRUCT pParentMenu;
		
		pMenu->bSetOne     = 0;
        pParentMenu        = pMenu->pParentMenu;
        pMenu->pParentMenu = NULL;

        if( pParentMenu == NULL )
        {
        	USBSelectModeDV(NULL,TRUE);//return to video preview.
	        return MENU_ERR_EXIT;
        }
        SetCurrentMenu(pParentMenu);
        pParentMenu->pfEventHandler( pParentMenu, MENU_INIT, 0 );
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
#endif

#if (USB_MODE_SELECT_EN)
void MenuStateUSBModeSelectMode( UINT32 ulEvent, UINT32 ulPosition )
{
    UINT32       ulOperation;
    UINT32       ulResult;
    PSMENUSTRUCT pCurrMenu = NULL;

    if( BUTTON_VIRTUAL_RESET == ulEvent )
    {
    	ubUSBSelectedMode = USB_MAX_MODE;
    	ubModeDrawStyle   = 0;   
        SetCurrentMenu(&sUSBModeSelectMenu);        
    }

	MenuModePreCheck(ulEvent);
	
	if(BUTTON_USB_REMOVED == ulEvent)
	{
		AHC_SetMode(AHC_MODE_IDLE);
		AHC_NormalPowerOffPath();
	}
	
    ulOperation = MenuButtonToMenuOp( ulEvent,ulPosition );
 
    if(ulOperation == MENU_NONE) 
        return;
        
#ifdef NET_SYNC_PLAYBACK_MODE
	if(EVENT_NET_ENTER_PLAYBACK == ulOperation)
	{
		ResetCurrentMenu();
		Menu_WriteSetting();
		StateSwitchMode(UI_NET_PLAYBACK_STATE);
        CGI_FEEDBACK(ulOperation, 0 /* SUCCESSFULLY */);
        return;
	}
#endif

    pCurrMenu = GetCurrentMenu();

    if( pCurrMenu == NULL )
    	return;
    	
    ulResult = pCurrMenu->pfEventHandler( pCurrMenu, ulOperation, ulPosition );

    if( ulResult == MENU_ERR_EXIT )
    {
        ResetCurrentMenu();
        
        Menu_WriteSetting();
        
		USBFunctionHandler(ubUSBSelectedMode);
    }
}
#endif

#if (UI_MODE_SELECT_EN)
void MenuStateUIModeSelectMode( UINT32 ulEvent, UINT32 ulPosition )
{
    UINT32       ulOperation;
    UINT32       ulResult;
    PSMENUSTRUCT pCurrMenu = NULL;

    if( BUTTON_VIRTUAL_RESET == ulEvent )
    {
    	ubUISelectedMode = UI_MAX_MODE;
    	ubModeDrawStyle  = 1;   
        SetCurrentMenu(&sUIModeSelectMenu);        
    }

	MenuModePreCheck(ulEvent);

    ulOperation = MenuButtonToMenuOp( ulEvent,ulPosition );
 
    if(ulOperation == MENU_NONE) 
        return;
    
    pCurrMenu = GetCurrentMenu();

    if( pCurrMenu == NULL )
    	return;

    ulResult = pCurrMenu->pfEventHandler( pCurrMenu, ulOperation, ulPosition );
	
    if( ulResult == MENU_ERR_EXIT )
    {
        ResetCurrentMenu();
        
        Menu_WriteSetting();
        
		if(ubUISelectedMode==UI_MAX_MODE)
		{
    		StateSwitchMode(UI_VIDEO_STATE/*uiGetLastState()*/);
    	}
    	else
    	{
    		UIFunctionHandler(ubUISelectedMode);
    		ubUISelectedMode = UI_MAX_MODE;	
    	}
    }
}
#endif

#if (BROWSER_MODE_SELECT_EN)
void MenuStateBrowserModeSelectMode( UINT32 ulEvent, UINT32 ulPosition )
{
    UINT32       ulOperation;
    UINT32       ulResult;
    PSMENUSTRUCT pCurrMenu = NULL;

    if( BUTTON_VIRTUAL_RESET == ulEvent )
    {
    	ubBrowserSelected = BROWSER_MAX_MODE;
    	ubModeDrawStyle   = 0;  
        SetCurrentMenu(&sBrowserModeSelectMenu);        
    }

	MenuModePreCheck(ulEvent);

    ulOperation = MenuButtonToMenuOp( ulEvent ,ulPosition);
 
    if(ulOperation == MENU_NONE) 
        return;
    
    pCurrMenu = GetCurrentMenu();

    if( pCurrMenu == NULL )
    	return;

    ulResult = pCurrMenu->pfEventHandler( pCurrMenu, ulOperation, ulPosition );
	
    if( ulResult == MENU_ERR_EXIT )
    {
        ResetCurrentMenu();
        
        Menu_WriteSetting();
        
        if(ubBrowserSelected==BROWSER_MAX_MODE)
        {
    		StateSwitchMode(UI_MODESELECT_MENU_STATE);
    	}
    	else
    	{
			BrowserFunctionHandler(ubBrowserSelected);	
    		ubBrowserSelected = BROWSER_MAX_MODE;
    	}
    }
}
#endif

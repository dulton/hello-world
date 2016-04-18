/*===========================================================================
 * Include file 
 *===========================================================================*/ 
 
#include "customer_config.h"
#include "AHC_Common.h"
#include "AHC_General.h"
#include "AHC_Utility.h"
#include "AHC_Display.h"
#include "AHC_Dcf.h"
#include "AHC_Uf.h"
#include "AHC_Browser.h"
#include "AHC_Menu.h"
#include "AHC_Media.h"
#include "AHC_Fs.h"
#include "AHC_Warningmsg.h"
#include "AHC_USB.h"
#include "AHC_General_CarDV.h"
#include "KeyParser.h"
#include "StateBrowserFunc.h"
#include "StatePhotoPBFunc.h"
#include "StateMoviePBFunc.h"
#include "StateTVFunc.h"
#include "StateHdmiFunc.h"
#include "DrawStateBrowserFunc.h"
#include "DrawStateTVFunc.h"
#include "DrawStateHdmiFunc.h"
#include "MenuCommon.h"
#include "MenuSetting.h"
#include "MenuDrawingFunc.h"
#include "MenuTouchButton.h"
#include "MenuDrawCommon.h"
#include "LedControl.h"
#include "IconPosition.h"
#include "ColorDefine.h"
#include "dsc_charger.h"
#include "MediaPlaybackCtrl.h"
#include "MenuStatePlaybackMenu.h"
#if defined(WIFI_PORT) && (WIFI_PORT == 1)
#include "netapp.h"
#endif
/*===========================================================================
 * Macro define
 *===========================================================================*/

/*===========================================================================
 * Global varible
 *===========================================================================*/ 


UINT32		ObjSelect 			= 0;
AHC_BOOL 	bForceSwitchBrowser = AHC_FALSE;

/*===========================================================================
 * Extern varible
 *===========================================================================*/ 

extern AHC_BOOL bForce_PowerOff;
extern AHC_BOOL Delete_File_Confirm;
extern AHC_BOOL Deleting_InBrowser;
extern AHC_BOOL Delete_Option;
extern AHC_BOOL Protect_File_Confirm;
extern AHC_BOOL Protecting_InBrowser;
extern AHC_BOOL Protect_Option;
extern AHC_BOOL UnProtect_File_Confirm;
extern AHC_BOOL UnProtect_Option;
extern AHC_BOOL m_ubPlaybackRearCam;
extern AHC_BOOL gbAhcDbgBrk;
/*===========================================================================
 * Main body
 *===========================================================================*/ 

void BrowserFunc_CheckLCDstatus(void)
{
	UINT8 			status;
	AHC_THUMB_CFG   ThumbConfig;
	
	AHC_LCD_GetStatus(&status);
		
	AHC_SetKitDirection(status, AHC_TRUE, AHC_SNR_NOFLIP, AHC_FALSE);
	
	ThumbConfig.uwTHMPerLine    = THUMB_PER_LINE;
	ThumbConfig.uwTHMPerColume  = THUMB_PER_COLUMN;
	ThumbConfig.uwHGap          = THUMB_HORIZ_GAP;
	ThumbConfig.uwVGap          = THUMB_VERTIC_GAP;
	ThumbConfig.byBkColor       = 0x101010;
	ThumbConfig.byFocusColor    = 0x0000FF;
	ThumbConfig.StartX     		= POS_THUMB_AREA_X0;
	ThumbConfig.StartY          = POS_THUMB_AREA_Y0;
	ThumbConfig.DisplayWidth    = POS_THUMB_AREA_W;
	ThumbConfig.DisplayHeight   = POS_THUMB_AREA_H;
	
	AHC_Thumb_Config(&ThumbConfig, AHC_TRUE);
}

void BrowserFunc_Setting(void)
{
    UINT32 MaxDcfObj; 

    if( JPGPB_MODE == GetCurrentOpMode())
    {
        MaxDcfObj = AHC_GetMappingFileNum(FILE_PHOTO);
    }
    else if ( MOVPB_MODE == GetCurrentOpMode())
    {
        MaxDcfObj = AHC_GetMappingFileNum(FILE_MOVIE);
    }
	else if( JPGPB_MOVPB_MODE == GetCurrentOpMode())
    {
        MaxDcfObj = AHC_GetMappingFileNum(FILE_PHOTO_MOVIE);
    }
    
    if(MaxDcfObj==0)
    {
    	AHC_WMSG_Draw(AHC_TRUE, WMSG_NO_FILE_IN_BROWSER, 2);

        #if UVC_HOST_VIDEO_ENABLE
        AHC_OS_SleepMs(1000);
        #endif
    } else {
        AHC_WMSG_Draw(AHC_FALSE, WMSG_NONE, 0);
    }
    
    if( ObjSelect > MaxDcfObj - 1)
    	ObjSelect = MaxDcfObj - 1;
    
    #ifdef CFG_BROWSER_ALWAYS_SELECT_LAST //may be defined in config_xxx.h
    ObjSelect = MaxDcfObj - 1;
    #endif
    
    AHC_UF_SetCurrentIndex(ObjSelect);
}

AHC_BOOL BrowserFunc_ModeSwitch(void)
{
    AHC_BOOL  ahc_ret;
    
    AHC_SetMode(AHC_MODE_IDLE);
	
    if(MOVPB_MODE == GetCurrentOpMode())
    {
        MenuStatePlaybackSelectDB( JPGPB_MODE );
        SetCurrentOpMode(JPGPB_MODE);
    }
    else if(JPGPB_MODE == GetCurrentOpMode())
    {
        MenuStatePlaybackSelectDB( MOVPB_MODE );
        SetCurrentOpMode(MOVPB_MODE);
    }
	else if(JPGPB_MOVPB_MODE == GetCurrentOpMode())
    {
        MenuStatePlaybackSelectDB( JPGPB_MOVPB_MODE );
        SetCurrentOpMode(JPGPB_MOVPB_MODE);
    }
    else
    {
        SetCurrentOpMode(JPGPB_MODE);
    }

	#if (FOCUS_ON_LATEST_FILE)
	if(!Deleting_InBrowser && !Protecting_InBrowser)
		AHC_SetPlayFileOrder(PLAY_LAST_FILE);
	#endif
    
    BrowserFunc_Setting();
    BrowserFunc_CheckLCDstatus();
    
    ahc_ret = AHC_SetMode(AHC_MODE_THUMB_BROWSER);

    return ahc_ret;
}

AHC_BOOL BrowserFunc_ThumbnailOption(UINT8 Op)
{
	AHC_BOOL ahc_ret = AHC_TRUE;

	if(!AHC_CheckCurSysMode(AHC_MODE_THUMB_BROWSER))
		return AHC_FALSE;

	switch(Op)
	{
		case BROWSER_UP:
			ahc_ret = AHC_Thumb_Rewind(THUMB_PER_LINE);
		break;

		case BROWSER_DOWN:
			ahc_ret = AHC_Thumb_Forward(THUMB_PER_LINE);
		break;

		case BROWSER_LEFT:
			ahc_ret = AHC_Thumb_Rewind(1);
		break;
		
		case BROWSER_RIGHT:
			ahc_ret = AHC_Thumb_Forward(1);
		break;
		
		case BROWSER_PLAY:
			{
			    UINT32              CurDcfIdx;
			    UINT8               FileType;
			    AHC_UF_GetCurrentIndex(&CurDcfIdx);
			    AHC_UF_GetFileTypebyIndex(CurDcfIdx, &FileType);
                
				if ( ( FileType == DCF_OBG_MP3 ) ||
              		 ( FileType == DCF_OBG_WAV ) ||
              		 ( FileType == DCF_OBG_OGG ) ||
              		 ( FileType == DCF_OBG_WMA ) )
			
				SetCurrentOpMode(AUDPB_MODE);
			}

		#if (SLIDESHOW_EN)
			ahc_ret = StateSwitchMode(UI_SLIDESHOW_STATE);	
		#else
			ahc_ret = StateSwitchMode(UI_PLAYBACK_STATE);
		#endif
		break;

		case BROWSER_PAGEUP:
			ahc_ret = AHC_Thumb_RewindPage();
		break;
		
		case BROWSER_PAGEDOWN:
			ahc_ret = AHC_Thumb_ForwardPage();
		break;

		case BROWSER_HDMI:
			ahc_ret = StateSwitchMode(UI_HDMI_STATE);
		break;													
	
		case BROWSER_TVOUT:
			ahc_ret = StateSwitchMode(UI_TVOUT_STATE);
		break;	
	
		default:
		break;
	}
	return ahc_ret;
}

AHC_BOOL BrowserFunc_ThumbnailResetVarible(UINT8 EditOp)
{
	switch(EditOp)
	{
		case EDIT_OP_DELETE:
			Delete_File_Confirm   	= 0;
			Deleting_InBrowser   	= 0;
			Delete_Option 			= CONFIRM_NOT_YET;		
		break;
		case EDIT_OP_PROTECT:
			Protect_File_Confirm   	= 0;
			Protecting_InBrowser 	= 0;
			Protect_Option 			= CONFIRM_NOT_YET;		
		break;
		case EDIT_OP_UNPROTECT:
			UnProtect_File_Confirm  = 0;
			Protecting_InBrowser 	= 0;
			UnProtect_Option 		= CONFIRM_NOT_YET;		
		break;			
		default:
		break;
	}
	return AHC_TRUE;
}

AHC_BOOL BrowserFunc_ThumbnailResetAllEditOp(void)
{
	BrowserFunc_ThumbnailResetVarible(EDIT_OP_DELETE);
	BrowserFunc_ThumbnailResetVarible(EDIT_OP_PROTECT);
	BrowserFunc_ThumbnailResetVarible(EDIT_OP_UNPROTECT);
	return AHC_TRUE;
}

AHC_BOOL BrowserFunc_ThumbnailEditFilelUpdatePage(void)
{
	if(TVFunc_IsInTVMode())
	{
		#if (TVOUT_ENABLE)
		DrawStateTVBrowserPageUpdate();
		#endif
	}
	else if(HDMIFunc_IsInHdmiMode())
	{
		#if (HDMI_ENABLE)
		DrawStateHdmiBrowserPageUpdate();	
		#endif			
	}
	else
	{	
		DrawBrowserStatePageUpdate();
	}
	return AHC_TRUE;
}

AHC_BOOL BrowserFunc_ThumbnailEditFilelComplete(void)
{
    UINT16 osdW;
    UINT16 osdH;
        
	BrowserFunc_ThumbnailResetAllEditOp();
	
	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	AHC_OSDSetColor(MAIN_DISPLAY_BUFFER, OSD_COLOR_TRANSPARENT);
	#else
	AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
	#endif
    osdW = AHC_GET_ATTR_OSD_W(MAIN_DISPLAY_BUFFER);
    osdH = AHC_GET_ATTR_OSD_H(MAIN_DISPLAY_BUFFER);
	AHC_OSDDrawFillRect(MAIN_DISPLAY_BUFFER, 0, 40, osdW, osdH - 40); 

#if (QUICK_EDIT_BROWSER_FILE==QUICK_EDIT_WITH_CONFIRM)
	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	AHC_OSDSetColor(OVL_BUFFER_INDEX2, OSD_COLOR_TRANSPARENT);
	#else
	AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
	#endif
    osdW = AHC_GET_ATTR_OSD_W(OVL_BUFFER_INDEX2);
    osdH = AHC_GET_ATTR_OSD_H(OVL_BUFFER_INDEX2);
	AHC_OSDDrawFillRect(OVL_BUFFER_INDEX2, 	 0, 40, osdW, osdH - 40);
	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	AHC_OSDSetColor(OVL_BUFFER_INDEX3, OSD_COLOR_TRANSPARENT);
	#else
	AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
	#endif
          osdW = AHC_GET_ATTR_OSD_W(OVL_BUFFER_INDEX3);
          osdH = AHC_GET_ATTR_OSD_H(OVL_BUFFER_INDEX3);
	AHC_OSDDrawFillRect(OVL_BUFFER_INDEX3,   0, 40, osdW, osdH - 40);  
#endif		
		
    #ifdef FLM_GPIO_NUM
    AHC_OSDRefresh_PLCD();
    #endif

#if (SUPPORT_TOUCH_PANEL)
    KeyParser_ResetTouchVariable();
    KeyParser_TouchItemRegister(&Browser_TouchButton[0], ITEMNUM(Browser_TouchButton));  
#endif

	return AHC_TRUE;
}

AHC_BOOL BrowserFunc_ThumbnailEditFile(UINT8 Op)
{
   	UINT8 ret;
   
   	ret = QuickFileOperation(Op);
            
	BrowserFunc_ThumbnailEditFilelComplete();
            
	if(ret!=EDIT_FILE_SUCCESS) 
	{	
		printc("Edit File ERROR!!!\r\n");           
		DrawFileOperationResult(ret);
		            
		BrowserFunc_ThumbnailEditFilelUpdatePage();
		return AHC_FALSE;
	}
	else
	{
		printc("Edit File Complete!\r\n");
		AHC_Thumb_DrawPage(AHC_TRUE);
	}
	return AHC_TRUE;
}

AHC_BOOL BrowserFunc_ThumbnailEditCheckFileObj(void)
{
    UINT32 MaxDcfObj; 

	if(MOVPB_MODE == GetCurrentOpMode())
	    MaxDcfObj = AHC_GetMappingFileNum(FILE_MOVIE);
	else if(JPGPB_MODE == GetCurrentOpMode())
	    MaxDcfObj = AHC_GetMappingFileNum(FILE_PHOTO);
	else if(JPGPB_MOVPB_MODE == GetCurrentOpMode())
	    MaxDcfObj = AHC_GetMappingFileNum(FILE_PHOTO_MOVIE);

	if(!AHC_IsSDInserted() && MenuSettingConfig()->uiMediaSelect == MEDIA_SETTING_SD_CARD)
	{
		BrowserFunc_ThumbnailResetAllEditOp();
	}
	else if(MaxDcfObj == 0)
	{
		MaxDcfObj = AHC_GetMappingFileNum(FILE_PHOTO_MOVIE);
		
		if(MaxDcfObj==0) // No File In Movie/Photo Browser.
		{
			BrowserFunc_ThumbnailResetAllEditOp();
			bForceSwitchBrowser = AHC_TRUE;
		}
		
	    if(MOVPB_MODE == GetCurrentOpMode())
	        AHC_UF_SetFreeChar(0, DCF_SET_ALLOWED, (UINT8 *) SEARCH_MOVIE);
	    else if(JPGPB_MODE == GetCurrentOpMode())
	        AHC_UF_SetFreeChar(0, DCF_SET_ALLOWED, (UINT8 *) SEARCH_PHOTO);
		else if(JPGPB_MOVPB_MODE == GetCurrentOpMode())
	        AHC_UF_SetFreeChar(0, DCF_SET_ALLOWED, (UINT8 *) SEARCH_PHOTO_MOVIE);
	    else
	        AHC_UF_SetFreeChar(0, DCF_SET_ALLOWED, (UINT8 *) SEARCH_MOVIE);
	}

	return AHC_TRUE;
}

AHC_BOOL BrowserFunc_ThumbnailEditFile_Option(UINT32 ulEvent)
{
    UINT32 MaxDcfObj; 

    AHC_UF_GetTotalFileCount(&MaxDcfObj);

	if(MaxDcfObj == 0)
	{
		//BrowserFunc_ThumbnailResetAllEditOp();
		printc("No Files in Browser\r\n");
		return AHC_TRUE;
	}
	
	if(ulEvent==EVENT_KEY_LEFT)
	{
		if(Deleting_InBrowser)
		{
			if(Delete_File_Confirm)
			{
				Delete_Option = CONFIRM_OPT_YES;
				MenuDrawChangeSubItem_EditOneFile();
				printc("Cur Delete_Option YES\r\n");
        	}
        	else
        	{
        		if(TVFunc_IsInTVMode())
        		{
					TVFunc_ThumbnailOption(BROWSER_LEFT);
					DrawStateTVBrowserUpdate(ulEvent);        	 	
        	 	}
        	 	else if(HDMIFunc_IsInHdmiMode())
        	 	{
					HDMIFunc_ThumbnailOption(BROWSER_LEFT);
					DrawStateHdmiBrowserUpdate(ulEvent);         	 	
        	 	}
        	 	else
        	 	{
	        	 	BrowserFunc_ThumbnailOption(BROWSER_LEFT);
	             	DrawStateBrowserUpdate(ulEvent);	
        		}
        	}	
        }	
		else if(Protecting_InBrowser)
		{
			if(Protect_File_Confirm && Protecting_InBrowser==1)
			{
				Protect_Option = CONFIRM_OPT_YES;
				MenuDrawChangeSubItem_EditOneFile();
				printc("Cur Protect_Option YES\r\n");
        	}
        	else if(UnProtect_File_Confirm && Protecting_InBrowser==2)
			{
				UnProtect_Option = CONFIRM_OPT_YES;
				MenuDrawChangeSubItem_EditOneFile();
				printc("Cur UnProtect_Option YES\r\n");
        	}
        	else
        	{
        		if(TVFunc_IsInTVMode())
        		{
					TVFunc_ThumbnailOption(BROWSER_LEFT);
					DrawStateTVBrowserUpdate(ulEvent);        	 	
        	 	}
        	 	else if(HDMIFunc_IsInHdmiMode())
        	 	{
					HDMIFunc_ThumbnailOption(BROWSER_LEFT);
					DrawStateHdmiBrowserUpdate(ulEvent);         	 	
        	 	}
        	 	else
        	 	{
	        	 	BrowserFunc_ThumbnailOption(BROWSER_LEFT);
	             	DrawStateBrowserUpdate(ulEvent);	
        		}	
        	}
		}
	}
	else if(ulEvent==EVENT_KEY_RIGHT)
	{
		if(Deleting_InBrowser)
		{
			if(Delete_File_Confirm)
			{
				Delete_Option = CONFIRM_OPT_NO;
				MenuDrawChangeSubItem_EditOneFile();
				printc("Cur Delete_Option NO\r\n");
			}
			else
			{
			    if(TVFunc_IsInTVMode())
        		{
					TVFunc_ThumbnailOption(BROWSER_RIGHT);
					DrawStateTVBrowserUpdate(ulEvent);        	 	
        	 	}
			    else if(HDMIFunc_IsInHdmiMode())
        		{
					HDMIFunc_ThumbnailOption(BROWSER_RIGHT);
					DrawStateHdmiBrowserUpdate(ulEvent);        	 	
        	 	}        	 	
				else
				{
        	 		BrowserFunc_ThumbnailOption(BROWSER_RIGHT);
             		DrawStateBrowserUpdate(ulEvent);	
				}
			}	
        }	
		else if(Protecting_InBrowser)
		{
			if(Protect_File_Confirm && Protecting_InBrowser==1)
			{
				Protect_Option = CONFIRM_OPT_NO;
				MenuDrawChangeSubItem_EditOneFile();
				printc("Cur Protect_Option NO\r\n");
        	}
        	else if(UnProtect_File_Confirm && Protecting_InBrowser==2)
			{
				UnProtect_Option = CONFIRM_OPT_NO;
				MenuDrawChangeSubItem_EditOneFile();
				printc("Cur UnProtect_Option NO\r\n");
        	}
        	else
        	{
			    if(TVFunc_IsInTVMode())
        		{
					TVFunc_ThumbnailOption(BROWSER_RIGHT);
					DrawStateTVBrowserUpdate(ulEvent);        	 	
        	 	}
			    else if(HDMIFunc_IsInHdmiMode())
        		{
					HDMIFunc_ThumbnailOption(BROWSER_RIGHT);
					DrawStateHdmiBrowserUpdate(ulEvent);        	 	
        	 	}
				else
				{
        	 		BrowserFunc_ThumbnailOption(BROWSER_RIGHT);
             		DrawStateBrowserUpdate(ulEvent);	
				}
        	}
		}	
	}
	else if(ulEvent==EVENT_THUMBNAIL_KEY_PLAYBACK 	||
		    ulEvent==EVENT_TV_BROWSER_KEY_PLAYBACK  ||
		    ulEvent==EVENT_HDMI_BROWSER_KEY_PLAYBACK)
	{
		
		if(Deleting_InBrowser)
		{	 
			printc("OK:Cur Delete_Option[%d]\r\n",Delete_Option);
			 	
			if(Delete_Option == CONFIRM_NOT_YET)
			{
				MenuDrawSubPage_EditOneFile(NULL);
			}
			else 
			{
				if(Delete_Option == CONFIRM_OPT_YES)
				{
					BrowserFunc_ThumbnailEditFile(DCF_FILE_DELETE);
				}
				else if(Delete_Option == CONFIRM_OPT_NO) 
				{
					BrowserFunc_ThumbnailEditFilelComplete();
				}
				#if (MENU_EDIT_CONTINUOUSLY)
				// delete continuously
				AHC_UF_GetCurrentIndex(&ObjSelect);
				Delete_Option 	 	 = CONFIRM_NOT_YET; 
				Deleting_InBrowser   = 1;
				Delete_File_Confirm  = 0;
				#endif
				BrowserFunc_ThumbnailEditFilelUpdatePage();
			}
		}	
		else if(Protecting_InBrowser)
		{
			if(Protecting_InBrowser==1)
			{
				printc("OK:Cur Protect_Option[%d]\r\n",Protect_Option);
				 	
				if(Protect_Option == CONFIRM_NOT_YET)
				{
					MenuDrawSubPage_EditOneFile(NULL);
				}
				else 
				{
					if(Protect_Option == CONFIRM_OPT_YES) 
					{
						BrowserFunc_ThumbnailEditFile(DCF_FILE_READ_ONLY);
					} 
					else if(Protect_Option == CONFIRM_OPT_NO)
					{
						BrowserFunc_ThumbnailEditFilelComplete();
					}
					#if (MENU_EDIT_CONTINUOUSLY)
					// protect continuously
					AHC_UF_GetCurrentIndex(&ObjSelect);
					Protect_Option 	 	  = CONFIRM_NOT_YET; 
					Protecting_InBrowser  = 1;
					Protect_File_Confirm  = 0;
					#endif
					BrowserFunc_ThumbnailEditFilelUpdatePage();
				}
			}
			else if(Protecting_InBrowser==2)
			{
				printc("OK:Cur UnProtect_Option[%d]\r\n",UnProtect_Option);
			
				if(UnProtect_Option == CONFIRM_NOT_YET)
				{
					MenuDrawSubPage_EditOneFile(NULL);
				} 
				else 
				{
					if(UnProtect_Option == CONFIRM_OPT_YES)
					{
						if (AHC_IsCurFileCharLock())
							BrowserFunc_ThumbnailEditFile(DCF_FILE_CHAR_UNLOCK);
						else
							BrowserFunc_ThumbnailEditFile(DCF_FILE_NON_READ_ONLY);
					}
					else if(UnProtect_Option == CONFIRM_OPT_NO)
					{
						BrowserFunc_ThumbnailEditFilelComplete();
					}
					#if (MENU_EDIT_CONTINUOUSLY)
					// unprotect continuously
					AHC_UF_GetCurrentIndex(&ObjSelect);
					Protect_Option 	 	   = CONFIRM_NOT_YET;
					Protecting_InBrowser   = 2;
					UnProtect_File_Confirm = 0;
					#endif
					BrowserFunc_ThumbnailEditFilelUpdatePage();
				}
			}
		}
	}
	
    #ifdef FLM_GPIO_NUM
    AHC_OSDRefresh_PLCD();
    #endif
	
	return AHC_TRUE;
}

AHC_BOOL BrowserFunc_TouchValid(UINT8 ubIndex)
{
#if(SUPPORT_TOUCH_PANEL)
    UINT16  ThumbCount = 0;

    AHC_Thumb_GetCountCurPage(&ThumbCount);

	if(ubIndex < ThumbCount)
        return AHC_TRUE;
    else
        return AHC_FALSE;
#else
	return AHC_TRUE;
#endif
}

AHC_BOOL BrowserFunc_Thumbnail_Focus(UINT8 CurIdx, UINT8 ubIndex)
{
#if (SUPPORT_TOUCH_PANEL)
    if( ubIndex > CurIdx )
        return AHC_Thumb_Forward(ubIndex - CurIdx);
    else
        return AHC_Thumb_Rewind(CurIdx - ubIndex);
#else
	return AHC_TRUE;
#endif
}

AHC_BOOL BrowserMode_Start(void)
{
    AHC_BOOL 	ahc_ret = AHC_TRUE;

	BrowserFunc_ThumbnailEditCheckFileObj();
    
    BrowserFunc_Setting();
    
    AHC_SetDisplayMode(DISPLAY_MODE_DISABLE);
    BrowserFunc_CheckLCDstatus();
    DrawStateBrowserUpdate(EVENT_THUMBNAIL_MODE_INIT);
    AHC_SetDisplayMode(DISPLAY_MODE_OSD_ENABLE);
    
    ahc_ret = AHC_SetMode(AHC_MODE_THUMB_BROWSER);
  	
  	if(AHC_TRUE == ahc_ret)
  	{
#if (SUPPORT_TOUCH_PANEL)
        KeyParser_ResetTouchVariable();
        KeyParser_TouchItemRegister(&Browser_TouchButton[0], ITEMNUM(Browser_TouchButton));  
#endif
	}
  	    
    return ahc_ret;
}

void BrowserMode_Update(void)
{
	BrowserFunc_ThumbnailEditCheckFileObj();

#if (SUPPORT_TOUCH_PANEL)
    KeyParser_ResetTouchVariable();
    KeyParser_TouchItemRegister(&Browser_TouchButton[0], ITEMNUM(Browser_TouchButton));  
#endif

#if (QUICK_EDIT_BROWSER_FILE==QUICK_EDIT_WITH_CONFIRM)	
  	if( (Deleting_InBrowser   && Delete_File_Confirm)  || 
  		(Protecting_InBrowser && Protect_File_Confirm) ||
  		(Protecting_InBrowser && UnProtect_File_Confirm) )
  	{
  		MenuDrawSubPage_EditOneFile(NULL);
  	}
	else
#endif	
	{
    	BrowserFunc_CheckLCDstatus();
    	DrawStateBrowserUpdate(EVENT_THUMBNAIL_MODE_INIT); 
	}
}

//******************************************************************************
//
//                              AHC State Browser Mode
//
//******************************************************************************

void StateBrowserMode( UINT32 ulEvent )
{
    UINT32 		ulJobEvent 	= ulEvent;
    AHC_BOOL 	ret 		= AHC_TRUE;
    UINT32 		MaxObjIdx 	= 0;
		
    switch(ulJobEvent)
    {
		case EVENT_NONE                           :  
        break;

        case EVENT_SLIDESHOW_UPDATE_FILE://lucas
		{
            MovPlaybackParamReset();
            MovPlayback_Play();
        }
        break;

        case EVENT_KEY_UP                         :
        	if(Delete_File_Confirm || Protect_File_Confirm || UnProtect_File_Confirm)
        		break;
         
			BrowserFunc_ThumbnailOption (BROWSER_UP);
			DrawStateBrowserUpdate(ulJobEvent);
        break;
        
        case EVENT_KEY_DOWN                       : 
        	if(Delete_File_Confirm || Protect_File_Confirm || UnProtect_File_Confirm)
        		break;
        
			BrowserFunc_ThumbnailOption(BROWSER_DOWN);
			DrawStateBrowserUpdate(ulJobEvent);		
        break;

        case EVENT_THUMBNAIL_TOUCH_PAGE_UP        :
		case EVENT_KEY_LEFT                       :
			if(Deleting_InBrowser || Protecting_InBrowser)
			{
				BrowserFunc_ThumbnailEditFile_Option(ulJobEvent);	
			}	
			else
			{ 
				BrowserFunc_ThumbnailOption(BROWSER_LEFT);
				DrawStateBrowserUpdate(ulJobEvent);
			}		    
		break;

        case EVENT_THUMBNAIL_TOUCH_PAGE_DOWN       :
		case EVENT_KEY_RIGHT                       :
			if(Deleting_InBrowser || Protecting_InBrowser)
			{
				BrowserFunc_ThumbnailEditFile_Option(ulJobEvent);		
			}
			else
			{  
				BrowserFunc_ThumbnailOption(BROWSER_RIGHT);
				DrawStateBrowserUpdate(ulJobEvent);
			}
		break;		
		
		case EVENT_THUMBNAIL_KEY_PLAYBACK         :
			if(Deleting_InBrowser || Protecting_InBrowser)
			{
				BrowserFunc_ThumbnailEditFile_Option(ulJobEvent);
			}
			else
			{
				AHC_UF_GetTotalFileCount(&MaxObjIdx);
             	
				if(MaxObjIdx == 0)
					break;
        	 		
				AHC_UF_GetCurrentIndex(&ObjSelect);   
             	   
				if(ObjSelect == -1)
					break;
             	
				if(!BrowserFunc_ThumbnailOption(BROWSER_PLAY)) 
				{
					ResetCurrentMenu();
					StateSwitchMode(UI_BROWSER_STATE);
					AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);        
				}
			}
        break;

        case EVENT_THUMBNAIL_TOUCH_MENU           :
        case EVENT_KEY_MENU                       :
        	if(Deleting_InBrowser || Protecting_InBrowser)
        	{
        		if(Delete_File_Confirm || Protect_File_Confirm || UnProtect_File_Confirm)
        		{
        			break;
        		}
        		else
        		{
					BrowserFunc_ThumbnailResetAllEditOp();
        		}
        	}

			ret = StateSwitchMode(UI_PLAYBACK_MENU_STATE);
        break;
	#ifdef NET_SYNC_PLAYBACK_MODE
		case EVENT_NET_ENTER_PLAYBACK :
			BrowserFunc_ThumbnailResetAllEditOp();
			StateSwitchMode(UI_NET_PLAYBACK_STATE);
            CGI_FEEDBACK(ulJobEvent, 0 /* SUCCESSFULLY */);
		break;
	#endif
        case EVENT_THUMBNAIL_TOUCH_MODE           :
        case EVENT_KEY_MODE                       :
        			
#if (CYCLIC_MODE_CHANGE)
			{			
				UINT8 ubNextMode = UI_VIDEO_STATE;
				
				switch(GetCurrentOpMode())
				{
					case CAPTURE_MODE   :
					case JPGPB_MODE     :
						#if (DSC_MODE_ENABLE) && defined(CFG_BROWSER_JPGPB_TO_VIDEO_MENU) //may be defined in config_xxx.h
							/* Clean Thumbnail page, customer requested */
							AHC_DrawThumbNailEmpty();
							ubNextMode = UI_VIDEO_MENU_STATE;
							/* Make Menu be in Video mode to switch for this UI flow */
							SetCurrentOpMode(VIDEOREC_MODE);
						#else
							ubNextMode = UI_VIDEO_STATE;
						#endif
					break;
					
					case VIDEOREC_MODE  :
					case MOVPB_MODE     :
                        #if (DSC_MODE_ENABLE) || defined(CFG_BROWSER_MOVPB_TO_BROWSER_MENU) //may be defined in config_xxx.h
							ubNextMode = UI_BROWSER_STATE;
						#else
							ubNextMode = UI_VIDEO_STATE;
						#endif
					break;
				}

                if (GetCurrentOpMode() == MOVPB_MODE || GetCurrentOpMode() == JPGPB_MODE || GetCurrentOpMode() == JPGPB_MOVPB_MODE)
                {
                    DrawBrowserStatePlaybackRearCam( m_ubPlaybackRearCam );
                }
			
				if(ubNextMode==UI_VIDEO_MENU_STATE)
				{
					BrowserFunc_ThumbnailResetAllEditOp();
	                ret = StateSwitchMode(UI_VIDEO_MENU_STATE);
				}
				else if(ubNextMode==UI_VIDEO_STATE)
				{
					BrowserFunc_ThumbnailResetAllEditOp();
            		ret = StateSwitchMode(UI_VIDEO_STATE);
				}
				else //if(ubNextMode==UI_BROWSER_STATE)//Browser Switch
				{
					if(Delete_File_Confirm || Protect_File_Confirm || UnProtect_File_Confirm)
	        			break; 
						
					ret = BrowserFunc_ModeSwitch();
					// after Browser Switch, reinit bForceSwitchBrowser
					if(ret && bForceSwitchBrowser)
						bForceSwitchBrowser = AHC_FALSE; 
					DrawStateBrowserUpdate(EVENT_THUMBNAIL_MODE);
				}
			}
#elif(UI_MODE_SELECT_EN)
			BrowserFunc_ThumbnailResetAllEditOp();
			ret = StateSwitchMode(UI_MODESELECT_MENU_STATE);
#else
			#if (DSC_MODE_ENABLE)
			if(Delete_File_Confirm || Protect_File_Confirm || UnProtect_File_Confirm)
        		break; 

			ret = BrowserFunc_ModeSwitch();
			DrawStateBrowserUpdate(EVENT_THUMBNAIL_MODE);
			#endif
#endif	
        break;
        
        case EVENT_KEY_PLAYBACK_MODE              :
        	 
			if(MOVPB_MODE == GetCurrentOpMode())
			{	
				BrowserFunc_ThumbnailResetAllEditOp();
				ret = StateSwitchMode(UI_VIDEO_STATE);
			}
			else if(JPGPB_MODE == GetCurrentOpMode())
			{
				BrowserFunc_ThumbnailResetAllEditOp();
                ret = StateSwitchMode(UI_CAMERA_STATE);
			}
			else if(JPGPB_MOVPB_MODE == GetCurrentOpMode())
			{
				BrowserFunc_ThumbnailResetAllEditOp();
                ret = StateSwitchMode(UI_VIDEO_STATE);
			}
        break;
        
#if (SLIDESHOW_EN==1)
        case EVENT_SLIDESHOW_MODE:
  
			if(Deleting_InBrowser || Protecting_InBrowser)
				break;
			
			if(AHC_FALSE == AHC_SDMMC_GetMountState())
			{
				AHC_WMSG_Draw(AHC_TRUE, WMSG_NO_CARD, 2);
				break;
			}
			
			AHC_UF_GetTotalFileCount(&MaxObjIdx);
             	
			if(MaxObjIdx == 0)
				break;
			
			BrowserFunc_ThumbnailResetAllEditOp(); 
			ret = StateSwitchMode(UI_SLIDESHOW_STATE);
        break;
#endif     
        
        case EVENT_POWER_OFF                      :
			AHC_NormalPowerOffPath();
        break;

		//Special Event
        case EVENT_BROWSER_SELECT_MENU            :
#if (BROWSER_MODE_SELECT_EN)
        	if(Deleting_InBrowser || Protecting_InBrowser)
        	{
        		if(Delete_File_Confirm || Protect_File_Confirm || UnProtect_File_Confirm)
        		{
        			break;
        		}
        		else
        		{
					BrowserFunc_ThumbnailResetAllEditOp();
        		}
        	}

        	AHC_SetFindLockFileOnlyFlag(AHC_FALSE);
        	
			ret = StateSwitchMode(UI_BROWSELECT_MENU_STATE);
#endif
        break;

        case EVENT_FILE_DELETING : 
			Deleting_InBrowser = 1;
			BrowserFunc_ThumbnailEditFile_Option(EVENT_THUMBNAIL_KEY_PLAYBACK);
        break;

        case EVENT_LOCK_FILE_M   :
        	if (AHC_CheckCurFileLockStatus() || AHC_IsCurFileCharLock())
				Protecting_InBrowser = 2;//Unprotect
			else
				Protecting_InBrowser = 1;//Protect

        	BrowserFunc_ThumbnailEditFile_Option(EVENT_THUMBNAIL_KEY_PLAYBACK);
        break;

		case EVENT_CAMERA_PREVIEW:
			BrowserFunc_ThumbnailResetAllEditOp();
            ret = StateSwitchMode(UI_CAMERA_STATE);
		break;

		case EVENT_VIDEO_PREVIEW:
			BrowserFunc_ThumbnailResetAllEditOp();
			ret = StateSwitchMode(UI_VIDEO_STATE);
		break;

		case EVENT_LCD_POWER_SAVE			:
			AHC_SwitchLCDBackLight();
		break;

		case EVENT_SWITCH_PANEL_TVOUT:
			AHC_SwitchLCDandTVOUT();
		break;

        //Device
		case EVENT_DC_CABLE_IN:
			#if (CHARGER_IN_ACT_OTHER_MODE==ACT_RECORD_VIDEO)
			BrowserFunc_ThumbnailResetAllEditOp();
			ret = StateSwitchMode(UI_VIDEO_STATE);
			AHC_SetRecordByChargerIn(3);
			#elif (CHARGER_IN_ACT_OTHER_MODE == ACT_FORCE_POWER_OFF)
			SetKeyPadEvent(KEY_POWER_OFF);
			#endif
		break;
		
		case EVENT_DC_CABLE_OUT:
			AHC_SetChargerEnable(AHC_FALSE);
			#if (CHARGER_OUT_ACT_OTHER_MODE==ACT_FORCE_POWER_OFF || CHARGER_OUT_ACT_OTHER_MODE==ACT_DELAY_POWER_OFF)
			AHC_SetShutdownByChargerOut(AHC_TRUE);
			#endif
		break;

        case EVENT_USB_DETECT                     :
			if(IsAdapter_Connect())
			{
		     	#if (CHARGER_IN_ACT_OTHER_MODE==ACT_RECORD_VIDEO)
				BrowserFunc_ThumbnailResetAllEditOp();
				ret = StateSwitchMode(UI_VIDEO_STATE);
				AHC_SetRecordByChargerIn(3);
				#elif (CHARGER_IN_ACT_OTHER_MODE == ACT_FORCE_POWER_OFF)
				SetKeyPadEvent(KEY_POWER_OFF);
				#endif
			}
			else
			{   
				BrowserFunc_ThumbnailResetAllEditOp();
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
		     	#if (CHARGER_OUT_ACT_OTHER_MODE==ACT_FORCE_POWER_OFF || CHARGER_OUT_ACT_OTHER_MODE==ACT_DELAY_POWER_OFF)
		     	AHC_SetShutdownByChargerOut(AHC_TRUE);
		     	#endif
			}
        break;
        
        case EVENT_SD_DETECT                      :
			AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect);
			DrawBrowserStateSDMMC_In();
			AHC_SetMode(AHC_MODE_THUMB_BROWSER);
        break;
		
		case EVENT_SD_REMOVED                     :
		
			if(Deleting_InBrowser || Protecting_InBrowser)
			{
				BrowserFunc_ThumbnailEditFilelComplete();
			}
			if(AHC_TRUE == AHC_SDMMC_GetMountState())
			{
				AHC_DisMountStorageMedia(AHC_MEDIA_MMC);
				Enable_SD_Power(0 /* Power Off */);
			}
			DrawBrowserStateSDMMC_Out();
			AHC_DrawThumbNailEmpty();
        break;

        case EVENT_TV_DETECT                      :
			if(AHC_IsTVConnectEx()) { 
				BrowserFunc_ThumbnailOption(BROWSER_TVOUT);
			}
        break;
         
        case EVENT_TV_REMOVED                     :
        break;
		
        case EVENT_HDMI_DETECT                    :
			BrowserFunc_ThumbnailOption(BROWSER_HDMI);
        break;  
        		
		case EVENT_HDMI_REMOVED                   :
        break; 

        case EVENT_LCD_COVER_OPEN                  :
			LedCtrl_LcdBackLight(AHC_TRUE); 
        break;
            
        case EVENT_LCD_COVER_CLOSE                :
			LedCtrl_LcdBackLight(AHC_FALSE);
        break;    
            
        case EVENT_LCD_COVER_NORMAL               :
        case EVENT_LCD_COVER_ROTATE               :
            if( (Deleting_InBrowser || Protecting_InBrowser) && 
                (Delete_File_Confirm || Protect_File_Confirm || UnProtect_File_Confirm) )
            	;//NOP	
            else
        		AHC_DrawRotateEvent(ulJobEvent);
        break;

		//Update Message
		case EVENT_THUMBNAIL_UPDATE_MESSAGE		 :
			DrawStateBrowserUpdate(ulJobEvent);
        break;

		case EVENT_MOVPB_MODE           		 :
            if( GetCurrentOpMode() == MOVPB_MODE || GetCurrentOpMode() == JPGPB_MODE || GetCurrentOpMode() == JPGPB_MOVPB_MODE)
            {
                m_ubPlaybackRearCam = (m_ubPlaybackRearCam == AHC_TRUE)? AHC_FALSE : AHC_TRUE;
                DrawBrowserStatePlaybackRearCam( m_ubPlaybackRearCam );

				BrowserFunc_Setting();
		
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
				DrawStateBrowserUpdate(EVENT_THUMBNAIL_MODE);
            }
        break;

#if (UVC_HOST_VIDEO_ENABLE)
		case EVENT_USB_B_DEVICE_DETECT:
		case EVENT_USB_B_DEVICE_REMOVED:
			AHC_USB_PauseDetection(0);
			if((AHC_GetCurKeyEventID() == BUTTON_USB_B_DEVICE_DETECTED) ||
				(AHC_GetCurKeyEventID() == BUTTON_USB_B_DEVICE_REMOVED)){
				AHC_SetCurKeyEventID(EVENT_NONE);
			}
			else{
				AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0);
				printc("KeyEventID: BUTTON_USB_B_DEVICE_DETECTED,REMOVED is interrupted.\r\n");
			}  
		break;
#endif		
       
        default:
        break;
	}
}
#if (SUPPORT_TOUCH_PANEL)
UINT32 BrowserFunc_Touch(UINT16 pt_x,UINT16 pt_y)
{
	UINT32 		CurrIdx   = 0;
	UINT32 		SelObjIdx = 0;
	UINT32 		PageFileNum = 0;
	UINT32 		uiMaxIdx;
	UINT16		uiCurPage = 0;
	UINT16		uiCurNumPage = 0;

	SelObjIdx = (pt_x - POS_THUMB_AREA_X0)/(POS_THUMB_AREA_W/THUMB_PER_LINE);
	SelObjIdx += ((pt_y - POS_THUMB_AREA_Y0)/(POS_THUMB_AREA_H/THUMB_PER_COLUMN)) * THUMB_PER_LINE;	
	
	AHC_Thumb_GetMaxNumPerPage(&PageFileNum);
    AHC_Thumb_GetCurIdx(&CurrIdx); 
	AHC_UF_GetTotalFileCount(&uiMaxIdx);
	AHC_Thumb_GetCurPage(&uiCurPage);
	AHC_Thumb_GetCountCurPage(&uiCurNumPage);
//	printc("SelObjIdx %d CurrIdx %d uiMaxIdx %d \r\n",SelObjIdx,CurrIdx,uiMaxIdx);
//	printc(" uiCurNumPage %d uiCurPage %d \r\n",uiCurNumPage,uiCurPage);
	if(SelObjIdx>(uiCurNumPage-1)) return EVENT_NONE;
	if( SelObjIdx != (CurrIdx %PageFileNum) )
	{
		BrowserFunc_Thumbnail_Focus(CurrIdx%PageFileNum, SelObjIdx);
		DrawStateTVBrowserUpdate(EVENT_THUMBNAIL_TOUCH_PAGE_UPDATE);
	}
	else
	{
		if(Deleting_InBrowser || Protecting_InBrowser)
		{
			BrowserFunc_ThumbnailEditFile_Option(EVENT_THUMBNAIL_KEY_PLAYBACK);
		}
		else
		{				
			if(!BrowserFunc_ThumbnailOption(BROWSER_PLAY)) 
			{
				ResetCurrentMenu();
				StateSwitchMode(UI_BROWSER_STATE);
				AHC_WMSG_Draw(AHC_TRUE, WMSG_FILE_ERROR, 3);        
			}
		}
	}
	return EVENT_NONE;
}

UINT32 Browser_FileEdit_Touch(UINT16 pt_x,UINT16 pt_y)
{
	UINT16  i = 0;
	UINT32 ulEvent;
	UINT32 ret = MENU_ERR_OK;
	RECT rc = RECT_MENU_CONFIRM_YES;
	#define STR_GAP		(30)
	
	i = (pt_x -rc.uiLeft)/(rc.uiWidth+ STR_GAP);
	
	 if(Delete_Option == CONFIRM_OPT_NO || Protect_Option == CONFIRM_OPT_NO || UnProtect_Option == CONFIRM_OPT_NO)
	 {
		if(i==0)
			ulEvent = EVENT_KEY_LEFT;
		else
			ulEvent = EVENT_THUMBNAIL_KEY_PLAYBACK;
	 }
	 else 	 if(Delete_Option == CONFIRM_OPT_YES || Protect_Option == CONFIRM_OPT_YES || UnProtect_Option == CONFIRM_OPT_YES)
	 {
		if(i==1)
			ulEvent = EVENT_KEY_RIGHT;
		else
			ulEvent = EVENT_THUMBNAIL_KEY_PLAYBACK;
	 }
	BrowserFunc_ThumbnailEditFile_Option(ulEvent);
	return EVENT_NONE;

}
#endif

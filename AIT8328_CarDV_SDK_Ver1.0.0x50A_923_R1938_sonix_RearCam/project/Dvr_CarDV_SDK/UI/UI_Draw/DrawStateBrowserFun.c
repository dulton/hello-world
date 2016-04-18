/*===========================================================================
 * Include file 
 *===========================================================================*/ 

#include "customer_config.h" 
#include "AHC_GUI.h"
#include "AHC_OS.h"
#include "AHC_Browser.h"
#include "AHC_Dcf.h"
#include "AHC_UF.h"
#include "AHC_GUI.h"
#include "AHC_Menu.h"
#include "AHC_Display.h"
#include "AHC_Fs.h"
#include "AHC_Media.h"
#include "AHC_Utility.h"
#include "AHC_WarningMsg.h"
#include "AHC_General_CarDV.h"
#include "MenuCommon.h"
#include "MenuDrawCommon.h"
#include "KeyParser.h"
#include "MenuSetting.h"
#include "StateBrowserFunc.h"
#include "StateTVFunc.h"
#include "StateHDMIFunc.h"
#include "DrawStateMenuSetting.h"
#include "DrawStateBrowserFunc.h"
#include "ColorDefine.h"
#include "IconPosition.h"
#include "IconDefine.h"
#include "UI_DrawGeneral.h"
#include "OSDStrings.h"
#include "ShowOSDFunc.h"
#include "MenuStatePlaybackMenu.h"
/*===========================================================================
 * Extern varible
 *===========================================================================*/ 

extern UINT8 	m_ubBatteryStatus;
extern AHC_BOOL Deleting_InBrowser;
extern AHC_BOOL Protecting_InBrowser;
extern AHC_OSD_INSTANCE *m_OSD[];
extern AHC_BOOL m_ubPlaybackRearCam;
extern AHC_BOOL gbAhcDbgBrk;
/*===========================================================================
 * Main body
 *===========================================================================*/ 

void DrawBrowser_GetPageInfo(UINT16 *CurrPage, UINT16 *TotalPage)
{
    UINT32  CurrObjIdx 	= 0;
    UINT32  MaxObjIdx 	= 0;
    UINT32  ObjNumPerPage;
    UINT8   remains 	= 0;
    
    AHC_UF_GetTotalFileCount(&MaxObjIdx);
    AHC_UF_GetCurrentIndex(&CurrObjIdx);
    AHC_GetDCFObjNumPerPage(&ObjNumPerPage);
    
    remains = (MaxObjIdx%ObjNumPerPage)?(1):(0);

    *CurrPage  = (CurrObjIdx/ObjNumPerPage) + 1;
    *TotalPage = (MaxObjIdx/ObjNumPerPage) + remains;
}

void DrawBrowser_NameInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
#if (BROWSER_STYLE==BROWSER_2ND_STYLE)

    UINT32   	CurObjIdx;
    char        FilePathName[MAX_FILE_NAME_SIZE];
	UINT16  	leftpos 	= x;
    UINT16  	WordWidth 	= 8;
    char 		*filename;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);
      
    if(bFlag == AHC_FALSE)
    {
        AHC_OSDSetColor(ubID, 0x101010); 
        AHC_OSDDrawFillRect(ubID, 195, 50, 320, 195);   
    }
    else
    {
    	if(bLarge==LARGE_SIZE)
    	{
    		if(TVFunc_IsInTVMode())
    		{
    			WordWidth = 14;
    			AHC_OSDSetFont(ubID, &GUI_Font24B_ASCII);
    		}
    		else if(HDMIFunc_IsInHdmiMode())
    		{
    			WordWidth = 16;
    			AHC_OSDSetFont(ubID, &GUI_Font32B_ASCII);
    		}
    	}
    	else
    	{
    		WordWidth = 8;
    		AHC_OSDSetFont(ubID, &GUI_Font16B_ASCII);
    	}
	        
        AHC_OSDSetColor(ubID, OSD_COLOR_WHITE); 
        AHC_OSDSetBkColor(ubID, bkColor);
		
		{   
			AHC_THUMB_CFG 	ThumbConfig;
		 	UINT32        	CurPage, ThumbPerPage, FirstObjIdx, MaxObjIdx;
		 	UINT32			i, y1 = 50, cnt;

   			AHC_UF_GetTotalFileCount(&MaxObjIdx);

	        AHC_UF_GetCurrentIndex(&CurObjIdx);
   
   			AHC_GetThumbnailConfig(&ThumbConfig);
   
			if(CurObjIdx >= MaxObjIdx-1)
				CurObjIdx = MaxObjIdx-1;
   
    		ThumbPerPage = ThumbConfig.uwTHMPerLine * ThumbConfig.uwTHMPerColume;
    
    		CurPage = CurObjIdx / ThumbPerPage;
     
     		FirstObjIdx = CurPage * ThumbPerPage;
     		     		
     		for (i = FirstObjIdx, cnt = 0; i<(CurPage+1)*ThumbPerPage && cnt<ThumbPerPage; i++, cnt++)
     		{
     			if(i==MaxObjIdx)
     			{
     				i = 0;
     				break;
     			}
     		
     			AHC_UF_SetCurrentIndex(i);
     			AHC_UF_GetCurrentIndex(&i);
	            if( m_ubPlaybackRearCam == AHC_TRUE && VIDRECD_MULTI_TRACK == 0 ){ 
	                AHC_UF_SetRearCamPathFlag(AHC_TRUE);
	            }				
     		    AHC_UF_GetFilePathNamebyIndex(i,FilePathName);
		        filename = (char*)GetFileNameAddr(FilePathName);				
	            if( m_ubPlaybackRearCam == AHC_TRUE && VIDRECD_MULTI_TRACK == 0 ){
	                AHC_UF_SetRearCamPathFlag(AHC_FALSE);
	            }
		        		        
		        if(i==CurObjIdx)
		        	AHC_OSDSetColor(ubID, OSD_COLOR_RED);
		        else	
		        	AHC_OSDSetColor(ubID, OSD_COLOR_WHITE);
		        	
				AHC_OSDDrawText( ubID, (UINT8*)filename, 195, y1, strlen(filename));
				
				y1+= 25;
			}
			
			AHC_UF_SetCurrentIndex(CurObjIdx);
		}
		
		if (Para1 != 0)
		{
			// It is for Delete/Protect/Unprotect, Show desctiption
			char	*sz;
			
			ShowOSD_SetLanguage(ubID);
			AHC_OSDSetColor(ubID, OSD_COLOR_WHITE);
			
			sz = (char*)OSD_GetStringViaID(IDS_DS_SELECT_A_FILE);
			
			if(TVFunc_IsInTVMode()) {
				AHC_OSDDrawTextEx( ubID, (UINT8*)sz, leftpos-90, y, strlen(sz), 2);
			} else {
				AHC_OSDDrawText( ubID, (UINT8*)sz, leftpos,    y, strlen(sz));
			}
		}
    }

	END_LAYER(ubID);

#else

    UINT32   	CurObjIdx;
    char        FilePathName[MAX_FILE_NAME_SIZE];
    char        DirPath[MAX_FILE_NAME_SIZE];
    char  		*DirName;
    UINT16  	leftpos 	= x;
    UINT16  	WordWidth 	= 8;
    char 		*filename;
    UINT32      uiFileNameLength;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);
     
    if(bFlag == AHC_FALSE)
    {
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, bkColor);
        #else
        AHC_OSDSetColor(bkColor);
        #endif
        AHC_OSDDrawFillRect(ubID, x, y, x+180, y+25);
    }
    else
    {
    	if(bLarge==LARGE_SIZE)
    	{
    		if(TVFunc_IsInTVMode())
    		{
    			WordWidth = 14;
				#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    			AHC_OSDSetFont(ubID, &GUI_Font24B_ASCII);
    			#else
    			AHC_OSDSetFont(&GUI_Font24B_ASCII);
    			#endif
    		}
    		else if(HDMIFunc_IsInHdmiMode())
    		{
    			WordWidth = 16;
				#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    			AHC_OSDSetFont(ubID, &GUI_Font32B_ASCII);
    			#else
    			AHC_OSDSetFont(&GUI_Font32B_ASCII);
    			#endif
    		}
    	}
    	else
    	{
    		WordWidth = 8;
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    		AHC_OSDSetFont(ubID, &GUI_Font16B_ASCII);
			#else
    		AHC_OSDSetFont(&GUI_Font16B_ASCII);
			#endif
    	}
	        
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, OSD_COLOR_WHITE); 
        AHC_OSDSetBkColor(ubID, bkColor);
		#else
        AHC_OSDSetColor(OSD_COLOR_WHITE); 
        AHC_OSDSetBkColor(bkColor);
		#endif
		
		if (Para1 == 0) 
		{
			// For Normal Browsing page
	        AHC_UF_GetCurrentIndex(&CurObjIdx);
            if( m_ubPlaybackRearCam == AHC_TRUE && VIDRECD_MULTI_TRACK == 0 ){ 
                AHC_UF_SetRearCamPathFlag(AHC_TRUE);
            }			
	        AHC_UF_GetFilePathNamebyIndex(CurObjIdx,FilePathName);
            if( m_ubPlaybackRearCam == AHC_TRUE && VIDRECD_MULTI_TRACK == 0 ){
                AHC_UF_SetRearCamPathFlag(AHC_FALSE);
            }			
	        filename = (char*)GetFileNameAddr(FilePathName);    //seperate filename from path 
            uiFileNameLength = (UINT32)(filename) - (UINT32)(&FilePathName); //get the filename length

            memset(DirPath,0,sizeof(DirPath));
            memcpy(DirPath,FilePathName,uiFileNameLength-1);  //generate dirpath
            DirPath[uiFileNameLength] = '\0';
            DirName = (char*)GetFileNameAddr(DirPath);        //get dirname by dirpath

            if(JPGPB_MODE == GetCurrentOpMode())
            {
                leftpos -= 15;
            }
            
	        AHC_OSDDrawText( ubID, (UINT8*)DirName, leftpos, y, strlen(DirName));
	        leftpos += (WordWidth*strlen(DirName));
	        AHC_OSDDrawText( ubID,(UINT8*)"/", leftpos, y, strlen("/"));
			leftpos += (WordWidth*1);
			AHC_OSDDrawText( ubID, (UINT8*)filename, leftpos, y, strlen(filename));
		} 
		else 
		{
			// It is for Delete/Protect/Unprotect, Show description of select a file
			char	*sz;
			
			ShowOSD_SetLanguage(ubID);
			sz = (char*)OSD_GetStringViaID(IDS_DS_SELECT_A_FILE);

			if(TVFunc_IsInTVMode()) {
				AHC_OSDDrawTextEx( ubID, (UINT8*)sz, leftpos-90, y + 4, strlen(sz), 2);
			} else {
				AHC_OSDDrawText( ubID, (UINT8*)sz, leftpos,    y, strlen(sz));
			}
		}
    }

	END_LAYER(ubID);

#endif    
}

void DrawBrowser_DateInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    UINT32   	CurObjIdx;
    char        FilePathName[MAX_FILE_NAME_SIZE];
    AHC_RTC_TIME RtcTime;
    UINT16  	WordWidth 	= 7;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);

    if(bFlag == AHC_FALSE)
    {
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, bkColor);
		#else
        AHC_OSDSetColor(bkColor);
		#endif

        #if (SUPPORT_TOUCH_PANEL)
        AHC_OSDDrawFillRect(ubID, x, y, m_OSD[ubID]->width, y+20);
        #else
        #ifdef CFG_DRAW_DATE_INFO_Y_OFFSET //may be defined in config_xxx.h
        y += CFG_DRAW_DATE_INFO_Y_OFFSET;
        #endif
        AHC_OSDDrawFillRect(ubID, 0, y, m_OSD[ubID]->width, y+20); 
    	#endif
    }
    else
    {
        AHC_UF_GetCurrentIndex(&CurObjIdx);
    	AHC_UF_GetFilePathNamebyIndex(CurObjIdx,FilePathName);
        AHC_UF_GetFileTimebyIndex(CurObjIdx,&RtcTime);
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, OSD_COLOR_WHITE); 
        AHC_OSDSetBkColor(ubID, bkColor);
		#else
        AHC_OSDSetColor(OSD_COLOR_WHITE); 
        AHC_OSDSetBkColor(bkColor);
		#endif
		
#ifdef CFG_DRAW_DATE_FONT_SIZE_20 //may be defined in config_xxx.h
    	WordWidth = 10;
    	AHC_OSDSetFont(ubID, &GUI_Font20_ASCII);
#elif defined (CFG_DRAW_SCREEN_128) //may be defined in config_xxx.h
    	WordWidth = 6;
    	AHC_OSDSetFont(ubID, &GUI_Font8_ASCII);
#else        
        if(bLarge==LARGE_SIZE)
        {
        	WordWidth = 14;
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        	AHC_OSDSetFont(ubID, &GUI_Font24B_ASCII);
			#else
        	AHC_OSDSetFont(&GUI_Font24B_ASCII);
			#endif
        }
        else
        {	
        	WordWidth = 7;
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        	AHC_OSDSetFont(ubID, &GUI_Font16B_ASCII);
			#else
        	AHC_OSDSetFont(&GUI_Font16B_ASCII);
			#endif
		}
#endif	    
	    #if (BROWSER_STYLE!=BROWSER_2ND_STYLE)   
    	if (Para1 != 0)
    	{
    		// For delete/protect/unprotect and show file name 
		    char 		*filename;
	    
	        filename = (char*)GetFileNameAddr(FilePathName);
	        
			if(TVFunc_IsInTVMode())
			{
				AHC_OSDDrawText(ubID, (UINT8*)filename, x-90, y + 16, strlen(filename));
                //cancel Draw_DateTime
                //x += 70;
				//OSDDraw_DateTime(ubID, x, y + 16, WordWidth, (void*)&(RtcTime.uwYear));
			}
			else
			{
			#if (SUPPORT_TOUCH_PANEL)
				AHC_OSDDrawText(ubID, (UINT8*)filename, x+10, y, strlen(filename));	        
	        #else
				AHC_OSDDrawText(ubID, (UINT8*)filename, x, y, strlen(filename));
			#endif
			
				x += 150;
				OSDDraw_DateTime(ubID, x, y, WordWidth, (void*)&(RtcTime.uwYear));
			}		
		} 
		else 
		{
			#ifdef CFG_DRAW_DATE_INFO_X //may be defined in config_xxx.h
			// x position of date/time for normal browser mode
			if (!TVFunc_IsInTVMode())
				x = 6;
			#endif
			OSDDraw_DateTime(ubID, x, y, WordWidth, (void*)&(RtcTime.uwYear));
		}
		#endif
		
    }

	END_LAYER(ubID);
}

#if defined(POS_BROW_RESOLUTION_SHOW)&& POS_BROW_RESOLUTION_SHOW
void DrawBrowser_ResolutionInfo(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    UINT8   		nDigit 		= 2;
    UINT16  		leftpos 	= x;
    UINT16  		WordWidth 	= 7;
    UINT32   		CurObjIdx;
    UINT8           FileType;
    AHC_VIDEO_ATTR  VidInfo;
    INT32           Res_H,Res_W;
    IMG_ATTRIB      ImgInfo;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

#ifdef CFG_DRAW_BROWSER_IGNORE_FILE_RESOLUTION_INFO //may be defined in config_xxx.h
	/* For not to show video time */
	return;
#endif

#if (BROWSER_STYLE!=BROWSER_2ND_STYLE)
	if (Para1) 
	{
		/* not to show this info in delete/protect/unprotect, due to no space */
		return;
	}
#endif	

	BEGIN_LAYER(ubID);
	
    if(bFlag == AHC_FALSE) 
    {
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    	AHC_OSDSetColor(ubID, bkColor);
    	#else
    	AHC_OSDSetColor(bkColor);
    	#endif
		AHC_OSDDrawFillRect(ubID, x, y, m_OSD[ubID]->width, y+20);
	}
    else
    {
        AHC_UF_GetCurrentIndex(&CurObjIdx);
    	AHC_UF_GetFileTypebyIndex(CurObjIdx, &FileType);
    	
	    if ( ( FileType == DCF_OBG_MOV ) ||
	         ( FileType == DCF_OBG_MP4 ) ||
	         ( FileType == DCF_OBG_AVI ) ||
	         ( FileType == DCF_OBG_3GP ) ||
	         ( FileType == DCF_OBG_WMV ) ) 
	    {
        	AHC_GetVideoFileAttr(&VidInfo);

            Res_W = VidInfo.Width[0];
            Res_H = VidInfo.Height[0];
        }
		else if ( ( FileType == DCF_OBG_JPG ) ||
                  ( FileType == DCF_OBG_TIF ) )
        {
            AHC_GetImageAttributes(&ImgInfo);
            Res_H = ImgInfo.ImageHSize;
            Res_W = ImgInfo.ImageVSize;
        }

		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, OSD_COLOR_WHITE);
        AHC_OSDSetBkColor(ubID, bkColor);
		#else
        AHC_OSDSetColor(OSD_COLOR_WHITE);
        AHC_OSDSetBkColor(bkColor);
		#endif
		
#ifdef CFG_DRAW_DATE_FONT_SIZE_20 //may be defined in config_xxx.h
    	WordWidth = 10;
    	AHC_OSDSetFont(ubID, &GUI_Font20_ASCII);
#else        
        if(bLarge==LARGE_SIZE)
        {
        	if(TVFunc_IsInTVMode())
        	{
	        	WordWidth = 14;
				#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	        	AHC_OSDSetFont(ubID, &GUI_Font24B_ASCII);
	        	#else
	        	AHC_OSDSetFont(&GUI_Font24B_ASCII);
	        	#endif
        	}
        	else if(HDMIFunc_IsInHdmiMode())
        	{
 	        	WordWidth = 16;
				#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	        	AHC_OSDSetFont(ubID, &GUI_Font32B_ASCII);
	        	#else
	        	AHC_OSDSetFont(&GUI_Font32B_ASCII);
	        	#endif
        	}
        }
        else
        {
        	WordWidth = 7;
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        	AHC_OSDSetFont(ubID, &GUI_Font16B_ASCII);
        	#else
        	AHC_OSDSetFont(&GUI_Font16B_ASCII);
        	#endif
        }
#endif   
       
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetPenSize(ubID, 3);
		#else
        AHC_OSDSetPenSize(3);
		#endif
		        
        AHC_OSDDrawText(ubID,(UINT8*)"[", leftpos ,y-2, strlen("["));

        leftpos += WordWidth*1;
        AHC_OSDDrawDec(ubID, Res_W, leftpos, y, 4);

		leftpos += 4*WordWidth;
        AHC_OSDDrawText(ubID,(UINT8*)"X", leftpos ,y, strlen("X"));

        leftpos += WordWidth+2;

        AHC_OSDDrawDec(ubID, Res_H, leftpos, y, ((Res_H >= 1000)? 4:3));

        leftpos += ((Res_H >= 1000)? 4:3)*WordWidth;
        AHC_OSDDrawText(ubID,(UINT8*)"]", leftpos ,y-2, strlen("]"));
    }

	END_LAYER(ubID);
}
#endif

void DrawBrowser_TotalTime(UINT16 ubID, UINT32 x, UINT32 y, AHC_BOOL bLarge, UINT8 ubMag, AHC_BOOL bFlag, GUI_COLOR bkColor, INT32 Para1)
{
    UINT8   		nDigit 		= 2;
    UINT16  		leftpos 	= x;
    UINT16  		WordWidth 	= 7;
    UINT32   		CurObjIdx;
    UINT8           FileType;
    AHC_VIDEO_ATTR  VidInfo;
    AUDIO_ATTR		AudInfo;
    INT32			hour, mins, sec;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

#ifdef CFG_DRAW_BROWSER_IGNORE_VIDEO_TIME //may be defined in config_xxx.h
	/* For not to show video time */
	return;
#endif

#if (BROWSER_STYLE!=BROWSER_2ND_STYLE)
	if (Para1) 
	{
		/* not to show this info in delete/protect/unprotect, due to no space */
		return;
	}
#endif	

	BEGIN_LAYER(ubID);
	
    if(bFlag == AHC_FALSE) 
    {
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    	AHC_OSDSetColor(ubID, bkColor);
    	#else
    	AHC_OSDSetColor(bkColor);
    	#endif
		AHC_OSDDrawFillRect(ubID, x, y, AHC_GET_ATTR_OSD_W(ubID), y+20);
	}
    else
    {
        AHC_UF_GetCurrentIndex(&CurObjIdx);
    	AHC_UF_GetFileTypebyIndex(CurObjIdx, &FileType);
    	
	    if ( ( FileType == DCF_OBG_MOV ) ||
	         ( FileType == DCF_OBG_MP4 ) ||
	         ( FileType == DCF_OBG_AVI ) ||
	         ( FileType == DCF_OBG_3GP ) ||
	         ( FileType == DCF_OBG_WMV ) ) 
	    {
        	AHC_GetVideoFileAttr(&VidInfo);
        	
        	hour = TIME_HOUR(VidInfo.TotalTime);
        	mins = TIME_MIN(VidInfo.TotalTime);
        	sec  = TIME_SEC(VidInfo.TotalTime);
        }
		else if ( ( FileType == DCF_OBG_MP3 ) ||
                  ( FileType == DCF_OBG_WAV ) ||
                  ( FileType == DCF_OBG_OGG ) ||
                  ( FileType == DCF_OBG_WMA ) )
        {
        	AHC_GetAudioFileAttr(&AudInfo);
        	
        	hour = TIME_HOUR(AudInfo.total_time);
        	mins = TIME_MIN(AudInfo.total_time);
        	sec  = TIME_SEC(AudInfo.total_time);
        }

		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(ubID, OSD_COLOR_WHITE);
        AHC_OSDSetBkColor(ubID, bkColor);
		#else
        AHC_OSDSetColor(OSD_COLOR_WHITE);
        AHC_OSDSetBkColor(bkColor);
		#endif
		
#ifdef CFG_DRAW_DATE_FONT_SIZE_20 //may be defined in config_xxx.h
    	WordWidth = 10;
    	AHC_OSDSetFont(ubID, &GUI_Font20_ASCII);
#else        
        if(bLarge==LARGE_SIZE)
        {
        	if(TVFunc_IsInTVMode())
        	{
	        	WordWidth = 14;
				#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	        	AHC_OSDSetFont(ubID, &GUI_Font24B_ASCII);
	        	#else
	        	AHC_OSDSetFont(&GUI_Font24B_ASCII);
	        	#endif
        	}
        	else if(HDMIFunc_IsInHdmiMode())
        	{
 	        	WordWidth = 16;
				#ifdef ENABLE_GUI_SUPPORT_MULTITASK
	        	AHC_OSDSetFont(ubID, &GUI_Font32B_ASCII);
	        	#else
	        	AHC_OSDSetFont(&GUI_Font32B_ASCII);
	        	#endif
        	}
        }
        else
        {
        	WordWidth = 7;
			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        	AHC_OSDSetFont(ubID, &GUI_Font16B_ASCII);
        	#else
        	AHC_OSDSetFont(&GUI_Font16B_ASCII);
        	#endif
        }
#endif   
       
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetPenSize(ubID, 3);
		#else
        AHC_OSDSetPenSize(3);
		#endif
		        
        AHC_OSDDrawText(ubID,(UINT8*)"[", leftpos ,y-2, strlen("["));

        leftpos += WordWidth*1;
        AHC_OSDDrawDec(ubID, hour, leftpos, y, nDigit);

		leftpos += nDigit*WordWidth;
        AHC_OSDDrawText(ubID,(UINT8*)":", leftpos ,y-2, strlen(":"));

        leftpos += WordWidth;

        AHC_OSDDrawDec(ubID, mins, leftpos, y, nDigit);

        leftpos += nDigit*WordWidth;
        AHC_OSDDrawText(ubID,(UINT8*)":", leftpos ,y-2, strlen(":"));

        leftpos += WordWidth;
        AHC_OSDDrawDec(ubID, sec, leftpos, y, nDigit); 

        leftpos += nDigit*WordWidth;
        AHC_OSDDrawText(ubID,(UINT8*)"]", leftpos ,y-2, strlen("]"));
    }

	END_LAYER(ubID);
}

void DrawBrowser_FileInfo(UINT8 ubID)
{
    UINT32      MaxDcfObj; 
    	
    AHC_UF_GetTotalFileCount(&MaxDcfObj);

    if(MaxDcfObj == 0)
    {
        UIDrawBrowserFuncEx(ubID, BROWSER_GUI_FILENAME,  		AHC_TRUE, AHC_FALSE, OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);	
        UIDrawBrowserFuncEx(ubID, BROWSER_GUI_FILE_DATE, 		AHC_TRUE, AHC_FALSE, OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);
        UIDrawBrowserFuncEx(ubID, BROWSER_GUI_MOV_TOTAL_TIME, 	AHC_TRUE, AHC_FALSE, OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);	
    }
    else
    {    
		if(MOVPB_MODE == GetCurrentOpMode())
        {
        	UIDrawBrowserFuncEx(ubID, BROWSER_GUI_FILENAME,  		AHC_TRUE, AHC_TRUE, OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);	
        	UIDrawBrowserFuncEx(ubID, BROWSER_GUI_FILE_DATE, 		AHC_TRUE, AHC_TRUE, OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);
        	UIDrawBrowserFuncEx(ubID, BROWSER_GUI_MOV_TOTAL_TIME, 	AHC_TRUE, AHC_TRUE, OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);
#if defined(POS_BROW_RESOLUTION_SHOW)&& POS_BROW_RESOLUTION_SHOW
            UIDrawBrowserFuncEx(ubID, BROWSER_GUI_RESOLUTION, 	    AHC_TRUE, AHC_TRUE, OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);
#endif
        }
        else if(JPGPB_MODE == GetCurrentOpMode())
        {
        	UIDrawBrowserFuncEx(ubID, BROWSER_GUI_FILENAME,  		AHC_TRUE, AHC_TRUE, OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);	
        	UIDrawBrowserFuncEx(ubID, BROWSER_GUI_FILE_DATE, 		AHC_TRUE, AHC_TRUE, OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);
#if defined(POS_BROW_RESOLUTION_SHOW)&& POS_BROW_RESOLUTION_SHOW
            UIDrawBrowserFuncEx(ubID, BROWSER_GUI_RESOLUTION,       AHC_TRUE, AHC_TRUE, OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);
#endif
        }
		else if(JPGPB_MOVPB_MODE == GetCurrentOpMode())
        {
        	UIDrawBrowserFuncEx(ubID, BROWSER_GUI_FILENAME,  		AHC_TRUE, AHC_TRUE, OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);	
        	UIDrawBrowserFuncEx(ubID, BROWSER_GUI_FILE_DATE, 		AHC_TRUE, AHC_TRUE, OSD_COLOR_TITLE, Deleting_InBrowser || Protecting_InBrowser);
        }
    }
}

void DrawBrowser_PageInfo(UINT8 ubID)
{
#if (BROWSER_STYLE==BROWSER_2ND_STYLE)

    UINT32  CurObjIdx, MaxObjIdx;
    char 	sz[20] ={0};

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);

    AHC_UF_GetTotalFileCount(&MaxObjIdx);
	AHC_UF_GetCurrentIndex(&CurObjIdx);

    AHC_OSDSetFont(ubID, &GUI_Font16B_1);
    AHC_OSDSetColor(ubID, OSD_COLOR_WHITE);
    AHC_OSDSetBkColor(ubID, OSD_THUMB_BACKGROUND);
	AHC_OSDSetPenSize(ubID, 1);
    
    if(MaxObjIdx==0)
    	sprintf(sz, "%d/%d", CurObjIdx, MaxObjIdx);
    else
    	sprintf(sz, "%d/%d", CurObjIdx+1, MaxObjIdx);
     
    AHC_OSDDrawText(ubID, (UINT8*)sz, 195, 7, sizeof(sz));

#else

    RECT   	obRECT  	= POS_THUMB_OBLIQUE;
    UINT16  CurrPage 	= 0;
    UINT16  MaxPage 	= 0;
    UINT8   digit 		= 0;
    UINT16  leftpos 	= 0;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ubID);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(ubID, OSD_COLOR_TRANSPARENT);
	#else
    AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
	#endif
    AHC_OSDDrawFillRect( ubID, (POS_CURPAGE_X-POS_PAGE_OFFSET), POS_CURPAGE_Y,
                               POS_MAXPAGE_X+20, 				POS_MAXPAGE_Y+16 ); 
    DrawBrowser_GetPageInfo(&CurrPage, &MaxPage);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetFont(ubID, &GUI_Font16B_1);
    AHC_OSDSetColor(ubID, OSD_COLOR_WHITE);
    AHC_OSDSetBkColor(ubID, OSD_COLOR_TRANSPARENT);
	AHC_OSDSetPenSize(ubID, 1);
	#else
    AHC_OSDSetFont(&GUI_Font16B_1);
    AHC_OSDSetColor(OSD_COLOR_WHITE);
    AHC_OSDSetBkColor(OSD_COLOR_TRANSPARENT);
	AHC_OSDSetPenSize(1);
	#endif
	
    if(0 != MaxPage)
    {
        digit = NumOfDigit(MaxPage);
        leftpos = POS_PAGEINFO(digit, POS_CURPAGE_X);
        AHC_OSDDrawDec( ubID, CurrPage, leftpos,  POS_CURPAGE_Y, digit);
    
        leftpos = POS_PAGEINFO(digit, POS_MAXPAGE_X);
        AHC_OSDDrawDec( ubID, MaxPage, leftpos,  POS_MAXPAGE_Y, digit );
    
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetPenSize(ubID, 3);
        #else
        AHC_OSDSetPenSize(3);
        #endif
        
        AHC_OSDDrawLine( ubID, obRECT.uiLeft, obRECT.uiLeft+obRECT.uiWidth, obRECT.uiTop, obRECT.uiTop-obRECT.uiHeight);
    }
	else
	{	// one page  
        leftpos = POS_PAGEINFO(digit, POS_CURPAGE_X);
        AHC_OSDDrawDec( ubID, 0, leftpos,  POS_CURPAGE_Y, 1);
    
        leftpos = POS_PAGEINFO(digit, POS_MAXPAGE_X);
        AHC_OSDDrawDec( ubID, 0, leftpos,  POS_MAXPAGE_Y, 1);
    
		#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetPenSize(ubID, 3);
        #else
        AHC_OSDSetPenSize(3);
		#endif
		
        AHC_OSDDrawLine( ubID, obRECT.uiLeft, obRECT.uiLeft+obRECT.uiWidth, obRECT.uiTop, obRECT.uiTop-obRECT.uiHeight);
	}
#endif	

    END_LAYER(ubID);
}

void DrawBrowser_BackCurtain(UINT8 ulID)
{
    UINT16 osdW;
    UINT16 osdH;
    osdW = AHC_GET_ATTR_OSD_W(ulID);
    osdH = AHC_GET_ATTR_OSD_H(ulID);

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }
    
    BEGIN_LAYER(ulID);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(ulID, OSD_COLOR_TITLE);
	#else
    AHC_OSDSetColor(OSD_COLOR_TITLE);
	#endif
    AHC_OSDDrawFillRect( ulID, 0, 0, 						  BG_CURTAIN_W, BG_CURTAIN_H);
    AHC_OSDDrawFillRect( ulID, 0, (DISP_HEIGHT-BG_CURTAIN_H), BG_CURTAIN_W, DISP_HEIGHT );

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(ulID, OSD_COLOR_TRANSPARENT);
	#else
    AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
	#endif
    AHC_OSDDrawFillRect( ulID, 0, BG_CURTAIN_H+BG_CURTAIN_EDGE, BG_CURTAIN_W, DISP_HEIGHT-BG_CURTAIN_H-BG_CURTAIN_EDGE  );

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetPenSize(ulID, 5);
    AHC_OSDSetColor(ulID, OSD_COLOR_WHITE);
	#else
    AHC_OSDSetPenSize(5);
    AHC_OSDSetColor(OSD_COLOR_WHITE);
    #endif
    AHC_OSDDrawFillRect( ulID, 0, BG_CURTAIN_H, 							  BG_CURTAIN_W, BG_CURTAIN_H+BG_CURTAIN_EDGE);
    AHC_OSDDrawFillRect(ulID, 0, osdH-BG_CURTAIN_H-BG_CURTAIN_EDGE, osdW, osdH-BG_CURTAIN_H);
    END_LAYER(ulID);
}

void DrawBrowser_SwitchMode(UINT8 ulID, OP_MODE_SETTING OpMode)
{
    RECT  mRECT  = POS_BUTTON_MENU_2;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    BEGIN_LAYER(ulID);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(ulID, OSD_COLOR_TITLE);
    #else
    AHC_OSDSetColor(OSD_COLOR_TITLE);
    #endif
    
    AHC_OSDDrawFillRect2(ulID, &mRECT);

    if(MOVPB_MODE == OpMode)
    {
        AHC_OSDDrawBitmap(ulID, &BMICON_BUTTON_MOVIE, mRECT.uiLeft, mRECT.uiTop);
        END_LAYER(ulID);
    }
    else if(JPGPB_MODE == OpMode)
    {
        AHC_OSDDrawBitmap(ulID, &BMICON_BUTTON_STILL, mRECT.uiLeft, mRECT.uiTop);
        END_LAYER(ulID);
        UIDrawBrowserFuncEx(ulID, BROWSER_GUI_MOV_TOTAL_TIME, AHC_TRUE, AHC_FALSE, OSD_COLOR_TITLE, NULL);
    }
}

void DrawStateBrowserInit(void)
{
    UINT8  bID0 	= 0;
    UINT8  bID1 	= 0;
#if (BROWSER_STYLE!=BROWSER_2ND_STYLE)
    RECT   uRECT  	= POS_THUMB_PGUP;
    RECT   dRECT  	= POS_THUMB_PGDOWN;
#endif
#if (SUPPORT_TOUCH_PANEL)    
    RECT   mRECT  	= RECT_TOUCH_BUTTON_BRO_MENU;
#endif

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    CHARGE_ICON_ENABLE(AHC_FALSE);

    OSDDraw_EnterDrawing(&bID0, &bID1);

    OSDDraw_ClearOvlDrawBuffer(bID0);

    BEGIN_LAYER(bID0);
    BEGIN_LAYER(bID1);
    OSDDraw_EnableSemiTransparent(bID0, AHC_TRUE);
    OSDDraw_EnableSemiTransparent(bID1, AHC_TRUE);
    END_LAYER(bID0);
    END_LAYER(bID1);

    DrawBrowser_BackCurtain(bID0);

#if (BROWSER_STYLE!=BROWSER_2ND_STYLE)
    BEGIN_LAYER(bID0);
    AHC_OSDDrawBitmap(bID0, &bmIcon_D_Up,   uRECT.uiLeft, uRECT.uiTop);
    AHC_OSDDrawBitmap(bID0, &bmIcon_D_Down, dRECT.uiLeft, dRECT.uiTop);
    END_LAYER(bID0);
#endif

#if (SUPPORT_TOUCH_PANEL)
    BEGIN_LAYER(bID0);
    AHC_OSDDrawBitmap(bID0, &bmIcon_Menu,   mRECT.uiLeft, mRECT.uiTop); 	
    OSD_ShowStringPool3(bID0, IDS_DS_MENU, mRECT, OSD_COLOR_WHITE, OSD_COLOR_TRANSPARENT, GUI_TA_HCENTER|GUI_TA_VCENTER);
    END_LAYER(bID0);
#endif

    DrawBrowser_SwitchMode(bID0, GetCurrentOpMode());

    DrawBrowser_PageInfo(bID0);

    DrawBrowser_FileInfo(bID0);

    UIDrawBrowserFuncEx(bID0, BROWSER_GUI_UI_MODE, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TITLE, NULL_PARAM);	

    UIDrawBrowserFuncEx(bID0, BROWSER_GUI_SD_STATE, AHC_FALSE, AHC_TRUE, OSD_COLOR_TITLE, NULL_PARAM);	

    UIDrawBrowserFuncEx(bID0, BROWSER_GUI_BATTERY, 	AHC_FALSE, AHC_TRUE, OSD_COLOR_TITLE, NULL_PARAM);	

    OSDDraw_ExitDrawing(&bID0, &bID1);

    CHARGE_ICON_ENABLE(AHC_TRUE); 
}

void DrawStateBrowserSwitch(void)
{
    UINT8  bID0 = 0;
    UINT8  bID1 = 0;

    OSDDraw_EnterDrawing(&bID0, &bID1);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, 0xAA000000); 
	#else
    AHC_OSDSetColor(0xAA000000);
    #endif
	
    DrawBrowser_SwitchMode(bID0, GetCurrentOpMode());
    
    UIDrawBrowserFuncEx(bID0, BROWSER_GUI_UI_MODE, AHC_TRUE, AHC_TRUE, OSD_COLOR_TITLE, NULL_PARAM);
        
    DrawBrowser_PageInfo(bID0);
        
    DrawBrowser_FileInfo(bID0);
    
    OSDDraw_ExitDrawing(&bID0, &bID1);
}

void DrawBrowserStatePageUpdate(void)
{
    UINT8  bID0 = 0;
    UINT8  bID1 = 0;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    OSDDraw_EnterDrawing(&bID0, &bID1);

    BEGIN_LAYER(bID0);

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(bID0, 0xAA000000); 
#else
    AHC_OSDSetColor(0xAA000000);
#endif

    END_LAYER(bID0);

    DrawBrowser_PageInfo(bID0);

    DrawBrowser_FileInfo(bID0);

    OSDDraw_ExitDrawing(&bID0, &bID1);
}

void DrawBrowserStateSDMMC_In(void)
{   
    MenuStatePlaybackSelectDB(GetCurrentOpMode());
	BrowserFunc_Setting();
	BrowserFunc_CheckLCDstatus();
    DrawStateBrowserInit();   
}

void DrawBrowserStateSDMMC_Out(void)
{
	DrawStateBrowserInit();  
    printc("Clear Browser when SDMMC OUT\r\n");
}

void DrawBrowserStatePlaybackRearCam( AHC_BOOL bEnable )
{
    UINT8  bID0 = 0;
    UINT8  bID1 = 0;
    RECT   rtIconPosition = { STRETCH_W(66), STRETCH_H(176), 60, 20 };

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

    OSDDraw_EnterDrawing(&bID0, &bID1);

    BEGIN_LAYER(bID0);

    if( bEnable == AHC_TRUE ){
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetFont(bID0, &GUI_Font16B_1);
#else
        AHC_OSDSetFont(&GUI_Font16B_1);
#endif

        OSD_ShowString(bID0, (char*)"RearCam", NULL, rtIconPosition, OSD_COLOR_WHITE, OSD_COLOR_TRANSPARENT, GUI_TA_LEFT|GUI_TA_VCENTER);
        //AHC_OSDDrawBitmap(bID0, &bmIcon_Scene_Auto, rtIconPosition.uiLeft, rtIconPosition.uiTop);
    }
    else{
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor( bID0, OSD_COLOR_TRANSPARENT );
        AHC_OSDSetBkColor( bID0, OSD_COLOR_TRANSPARENT );
#else
        AHC_OSDSetColor( OSD_COLOR_TRANSPARENT );
        AHC_OSDSetBkColor( OSD_COLOR_TRANSPARENT );
#endif

        AHC_OSDDrawFillRect(bID0, rtIconPosition.uiLeft, rtIconPosition.uiTop, rtIconPosition.uiLeft+rtIconPosition.uiWidth, rtIconPosition.uiTop+rtIconPosition.uiHeight);
    }
    
    END_LAYER(bID0);

    OSDDraw_ExitDrawing(&bID0, &bID1);
}

#if 0
void ____Event_Handler____(){ruturn;} //dummy
#endif

void DrawStateBrowserUpdate(UINT32 ubEvent)
{
    switch(ubEvent)
    {
        case EVENT_NONE                           :  
        break;
                                
        case EVENT_KEY_UP                         : 
        case EVENT_KEY_DOWN                       : 
        case EVENT_KEY_LEFT                       :
        case EVENT_KEY_RIGHT                      :
        case EVENT_THUMBNAIL_TOUCH_PAGE_UP        :
        case EVENT_THUMBNAIL_TOUCH_PAGE_DOWN      :
        case EVENT_THUMBNAIL_TOUCH_PAGE_UPDATE    :
			DrawBrowserStatePageUpdate();
        break;
        
        case EVENT_LCD_COVER_NORMAL               :
        case EVENT_LCD_COVER_ROTATE               :
			DrawStateBrowserInit();
			AHC_Thumb_DrawPage(AHC_TRUE);

			if (GetCurrentOpMode() == MOVPB_MODE)
                DrawBrowserStatePlaybackRearCam( m_ubPlaybackRearCam );
        break;
        
        case EVENT_THUMBNAIL_MODE_INIT            :
			DrawStateBrowserInit();

			if (GetCurrentOpMode() == MOVPB_MODE|| GetCurrentOpMode() == JPGPB_MODE)
                DrawBrowserStatePlaybackRearCam( m_ubPlaybackRearCam );
            else
                DrawBrowserStatePlaybackRearCam( AHC_FALSE );
        break;
        
        case EVENT_THUMBNAIL_MODE                 :
			DrawStateBrowserSwitch();
        break;
      
        case EVENT_THUMBNAIL_UPDATE_MESSAGE       :
        break;
        
        default:
        break;
    }
}

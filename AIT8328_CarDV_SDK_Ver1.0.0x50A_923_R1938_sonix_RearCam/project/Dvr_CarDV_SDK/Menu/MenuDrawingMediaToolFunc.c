/*===========================================================================
 * Include file 
 *===========================================================================*/ 

#include "AHC_Common.h"
#include "AHC_Capture.h"
#include "AHC_MACRO.h"
#include "AHC_Utility.h"
#include "AHC_Media.h"
#include "AHC_Video.h"
#include "AHC_FS.h"
#include "AHC_Display.h"
#include "AHC_General.h"
#include "AHC_General_CarDV.h"
#include "IconDefine.h"
#include "IconPosition.h"
#include "ColorDefine.h"
#include "OsdStrings.h"
#include "DrawStateMenuSetting.h"
#include "MenuCommon.h"
#include "MenuDrawCommon.h"
#include "MenuDrawingFunc.h"
#include "MenuSetting.h"
#include "ShowOSDFunc.h"
#include "StateCameraFunc.h"
#include "StateVideoFunc.h"
#include "StateTVFunc.h"
#include "StateHDMIFunc.h"
#include "dsc_charger.h"

/*===========================================================================
 * Structure define 
 *===========================================================================*/ 

typedef struct {
	const GUI_BITMAP* 	m_pBmp;
	UINT32 				m_Res;
	UINT32 				m_FrmRate;
} MOVIE_RES_INFO;

typedef struct {
	const GUI_BITMAP* 	m_pBmp;
	UINT32 				m_Res;
} IMG_RES_INFO;

/*===========================================================================
 * Global varible
 *===========================================================================*/ 

MOVIE_RES_INFO movie_info[] =
{
	#if(MENU_MOVIE_SIZE_1440_30P_EN)
	{(const GUI_BITMAP*)&BMICON_MOVIESIZE_1296P,	AHC_VIDRECD_RESOL_2560x1440,30},
	#endif
	#if (MENU_MOVIE_SIZE_SHD_30P_EN)
	{(const GUI_BITMAP*)&BMICON_MOVIESIZE_1296P,	AHC_VIDRECD_RESOL_2304x1296,30},
	#endif
	#if (MENU_MOVIE_SIZE_SHD_25P_EN)
	{(const GUI_BITMAP*)&BMICON_MOVIESIZE_1296_25P,	AHC_VIDRECD_RESOL_2304x1296,25},
	#endif
	#if (MENU_MOVIE_SIZE_SHD_24P_EN)
	{(const GUI_BITMAP*)&BMICON_MOVIESIZE_1296_24P,	AHC_VIDRECD_RESOL_2304x1296,24},
	#endif
	#if (MENU_MOVIE_SIZE_1080_60P_EN)
	{(const GUI_BITMAP*)&BMICON_MOVIESIZE_1080_60P,	AHC_VIDRECD_RESOL_1920x1088,60},
	#endif
	#if (MENU_MOVIE_SIZE_1080P_EN)
	{(const GUI_BITMAP*)&BMICON_MOVIESIZE_1080P, 	AHC_VIDRECD_RESOL_1920x1088,30},
	#endif
	#if (MENU_MOVIE_SIZE_720P_EN)
	{(const GUI_BITMAP*)&BMICON_MOVIESIZE_720P,  	AHC_VIDRECD_RESOL_1280x720, 30},
	#endif
	#if (MENU_MOVIE_SIZE_720_60P_EN) 
	{(const GUI_BITMAP*)&BMICON_MOVIESIZE_720_60P, 	AHC_VIDRECD_RESOL_1280x720, 60},
	#endif
    #if (MENU_MOVIE_SIZE_1080_24P_EN)
    {(const GUI_BITMAP*)&BMICON_MOVIESIZE_1080_24P, AHC_VIDRECD_RESOL_1920x1088,24},
    #endif
    #if (MENU_MOVIE_SIZE_720_24P_EN)
    {(const GUI_BITMAP*)&BMICON_MOVIESIZE_720_24P,  AHC_VIDRECD_RESOL_1280x720, 24},
    #endif
	#if (MENU_MOVIE_SIZE_VGA30P_EN)
	{(const GUI_BITMAP*)&BMICON_MOVIESIZE_VGA_30P,  AHC_VIDRECD_RESOL_640x480,  30},
	#endif
};

IMG_RES_INFO image_info[] = 
{
	#if (MENU_STILL_SIZE_14M_EN)
	{(const GUI_BITMAP*)&BMICON_IMAGESIZE_14M, 	IMAGE_SIZE_14M	},
	#endif
	#if (MENU_STILL_SIZE_12M_EN)
	{(const GUI_BITMAP*)&BMICON_IMAGESIZE_12M, 	IMAGE_SIZE_12M	},
	#endif
	#if (MENU_STILL_SIZE_8M_EN)
	{(const GUI_BITMAP*)&BMICON_IMAGESIZE_8M, 	IMAGE_SIZE_8M	},
	#endif
	#if (MENU_STILL_SIZE_5M_EN)
	{(const GUI_BITMAP*)&BMICON_IMAGESIZE_5M, 	IMAGE_SIZE_5M	},
	#endif
	#if (MENU_STILL_SIZE_3M_EN)
	{(const GUI_BITMAP*)&BMICON_IMAGESIZE_3M, 	IMAGE_SIZE_3M	},
	#endif
	#if (MENU_STILL_SIZE_2M_WIDE_EN)  
	{(const GUI_BITMAP*)&BMICON_IMAGESIZE_2M, 	IMAGE_SIZE_2M	},
	#endif
	#if (MENU_STILL_SIZE_1d2M_EN)
	{(const GUI_BITMAP*)&BMICON_IMAGESIZE_1_2M, IMAGE_SIZE_1_2M	},
	#endif
	#if (MENU_STILL_SIZE_VGA_EN)
	{(const GUI_BITMAP*)&BMICON_IMAGESIZE_VGA, 	IMAGE_SIZE_VGA	},
	#endif								
};

//UINT8 gNumOfVideoInfo = sizeof(movie_info) / sizeof(MOVIE_RES_INFO); 
#define	NUM_VIDEO_TYPE	sizeof(movie_info) / sizeof(MOVIE_RES_INFO)
UINT8 gNumOfImageInfo = sizeof(image_info) / sizeof(IMG_RES_INFO); 

extern AHC_OSD_INSTANCE *m_OSD[];

/*===========================================================================
 * Main body 
 *===========================================================================*/ 

void DrawSubItem_StorageInfo(int iMenuId, UINT16 uwDispID, int iItem, UINT32 iStrID, GUI_COLOR colorFont, GUI_COLOR clrBack)
{
    RECT 		tmpRECT= {30, 46, 100, 32};
    RECT		tmpRECT1;
    UBYTE 		Hour, Min, Sec;
    UINT16  	x, y, i;
    UINT16  	WordWidth = 8;
    UINT64 		FreeBytes;
    UINT32 		ulRemainImageNum;
    UINT8  		digit = 0;
	GUI_COLOR 	TextBkClr;
	
	if(TVFunc_IsInTVMode() || HDMIFunc_IsInHdmiMode())
	{
		tmpRECT.uiLeft += 50;
		TextBkClr = OSD_COLOR_TRANSPARENT;//TV_MENU_BK_COLOR;
	}	
	else
	{
		TextBkClr = OSD_COLOR_BLACK;
	}	

	tmpRECT.uiLeft   += ((tmpRECT.uiWidth+32) * iItem) - 25;
	
	tmpRECT1.uiLeft   = tmpRECT.uiLeft;
	tmpRECT1.uiTop    = tmpRECT.uiTop ;
	tmpRECT1.uiWidth  = tmpRECT.uiWidth;
	tmpRECT1.uiHeight = tmpRECT.uiHeight;
	
	if(iItem==1)
		tmpRECT1.uiWidth = AHC_GET_ATTR_OSD_W(uwDispID) - tmpRECT1.uiLeft;
	
	OSD_ShowStringPool(uwDispID, iStrID, tmpRECT1, colorFont, TextBkClr, GUI_TA_LEFT|GUI_TA_VCENTER);

	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(uwDispID, MENU_TEXT_COLOR);
    AHC_OSDSetBkColor(uwDispID, TextBkClr);
    AHC_OSDSetFont(uwDispID, &GUI_Font16B_1); 
	#else
    AHC_OSDSetColor(MENU_TEXT_COLOR);
    AHC_OSDSetBkColor(TextBkClr);
    AHC_OSDSetFont(&GUI_Font16B_1); 
	#endif

	//Draw Time left
	if(iItem == 0) 
	{  
		UINT32	br[NUM_VIDEO_TYPE];
		i = 0;
		
		#if(MENU_MOVIE_SIZE_1440_30P_EN)
		br[i++] = AHC_GetVideoBitrate(MOVIE_SIZE_1440_30P);
		#endif
		#if (MENU_MOVIE_SIZE_SHD_30P_EN)
		br[i++] = AHC_GetVideoBitrate(MOVIE_SIZE_SHD_30P);
		#endif
		#if (MENU_MOVIE_SIZE_SHD_25P_EN)
		br[i++] = AHC_GetVideoBitrate(MOVIE_SIZE_SHD_25P);
		#endif
		#if (MENU_MOVIE_SIZE_SHD_24P_EN)
		br[i++] = AHC_GetVideoBitrate(MOVIE_SIZE_SHD_24P);
		#endif
		#if (MENU_MOVIE_SIZE_1080_60P_EN)
		br[i++] = AHC_GetVideoBitrate(MOVIE_SIZE_1080_60P);
		#endif
		#if (MENU_MOVIE_SIZE_1080P_EN)
		br[i++] = AHC_GetVideoBitrate(MOVIE_SIZE_1080P);
		#endif
		#if (MENU_MOVIE_SIZE_720P_EN)
		br[i++] = AHC_GetVideoBitrate(MOVIE_SIZE_720P);
		#endif
		#if (MENU_MOVIE_SIZE_720_60P_EN)
		br[i++] = AHC_GetVideoBitrate(MOVIE_SIZE_720_60P);
		#endif
        #if (MENU_MOVIE_SIZE_1080_24P_EN)
        br[i++] = AHC_GetVideoBitrate(MOVIE_SIZE_1080_24P);
        #endif
        #if (MENU_MOVIE_SIZE_720_24P_EN)
        br[i++] = AHC_GetVideoBitrate(MOVIE_SIZE_720_24P);
        #endif
		#if (MENU_MOVIE_SIZE_VGA30P_EN)
		br[i++] = AHC_GetVideoBitrate(MOVIE_SIZE_VGA_30P);
		#endif
		

	    for(i=0; i<NUM_VIDEO_TYPE; i++)
	    {
			tmpRECT1.uiTop += 30;
			OSD_Draw_Icon(*(movie_info[i].m_pBmp), tmpRECT1, uwDispID);
				
		    AHC_AvailableRecordTime_Ex(br[i], &Hour, &Min, &Sec); 

		    x = tmpRECT1.uiLeft + 55;
		    y = tmpRECT1.uiTop  + 10;
				
			AHC_OSDDrawDec(uwDispID, Hour, x, y, 2);
			x += 2*WordWidth;
			AHC_OSDDrawText(uwDispID,(UINT8*)":", x ,y, strlen(":"));
			x += WordWidth;
			AHC_OSDDrawDec(uwDispID, Min, x, y, 2);
			x += 2*WordWidth;
			AHC_OSDDrawText(uwDispID,(UINT8*)":", x ,y, strlen(":"));
			x += WordWidth;
			AHC_OSDDrawDec(uwDispID, Sec, x, y, 2);
	    }
		
	    //Restore 
		Menu_SetMovieMode(MenuSettingConfig()->uiMOVSize);
        
		AHC_ConfigMovie(AHC_VIDEO_COMPRESSION_RATIO, MenuSettingConfig()->uiMOVQuality);
	} 
	else 
	{ //Draw Photo left

		for (i=0; i<gNumOfImageInfo; i++)
		{
			if(i==gNumOfImageInfo/2)
			{
				tmpRECT1.uiTop    = tmpRECT.uiTop + 30;
				tmpRECT1.uiLeft  += 90;	
			}
			else
			{
				tmpRECT1.uiTop   += 30;
			}
			
			OSD_Draw_Icon(*(image_info[i].m_pBmp), tmpRECT1, uwDispID);

        	CaptureFunc_SetResolution(image_info[i].m_Res, AHC_TRUE);
        
        	switch(MenuSettingConfig()->uiIMGQuality)
        	{
	        	case QUALITY_SUPER_FINE:
	            	AHC_SetCompressionRatio(350); 
	        	break;    
	        	case QUALITY_FINE:
	            	AHC_SetCompressionRatio(280);
	        	break;  
        	}
        
			CaptureFunc_CheckMemSizeAvailable(&FreeBytes, &ulRemainImageNum);

			#ifdef ENABLE_GUI_SUPPORT_MULTITASK
        	AHC_OSDSetColor(uwDispID, MENU_TEXT_COLOR); 
        	#else
        	AHC_OSDSetColor(MENU_TEXT_COLOR); 
        	#endif
			AHC_OSDDrawDec(uwDispID, ulRemainImageNum, tmpRECT1.uiLeft + 30, tmpRECT1.uiTop + 10, 6);
		
			digit = NumOfDigit(ulRemainImageNum);
		
			if(digit<6) 
			{
				#ifdef ENABLE_GUI_SUPPORT_MULTITASK
				AHC_OSDSetColor(uwDispID, TextBkClr); 
				#else
				AHC_OSDSetColor(TextBkClr); 
				#endif
				AHC_OSDDrawDec(uwDispID, 0, tmpRECT1.uiLeft + 30, tmpRECT1.uiTop + 10, 6-digit);
			}
		}
	
		//Restore
		CaptureFunc_SetResolution(MenuSettingConfig()->uiIMGSize, AHC_TRUE);
        
        switch(MenuSettingConfig()->uiIMGQuality)
        {
	        case QUALITY_SUPER_FINE:
	            AHC_SetCompressionRatio(350); 
	        break;
	        case QUALITY_FINE:
	            AHC_SetCompressionRatio(280);
	        break;  
        }
        CaptureFunc_CheckMemSizeAvailable(&FreeBytes, &ulRemainImageNum);
	}
}

void MenuDrawSubPage_SDCardInfo(PSMENUSTRUCT pMenu)
{
    UINT8  		bID0 = 0, bID1 = 0;
    UINT32 		i, iBegin, iEnd;
	UINT32 		PageItemNum;
	GUI_COLOR 	bkColor;
	GUI_COLOR	TextClr;

	CHARGE_ICON_ENABLE(AHC_FALSE);

#ifdef SLIDE_STRING
	StopSlideString();
#endif

    OSDDraw_EnterMenuDrawing(&bID0, &bID1);

    OSDDraw_ClearOvlDrawBuffer(bID0);

	PageItemNum = SUB_MENU_PAGE_ITEM;
	bkColor		= OSD_COLOR_BLACK;
	TextClr		= MENU_TEXT_COLOR;

	MenuDrawSubBackCurtain(bID0, bkColor);

	if( pMenu->uiStringId != -1 )
	{
		MenuDrawSubTitle(bID0, pMenu->uiStringId);
	}

    iBegin = ALIGN_DOWN(pMenu->iCurrentPos, PageItemNum);
    iEnd   = MIN(iBegin+PageItemNum, pMenu->iNumOfItems);

    for( i=iBegin; i<iEnd; i++ )
    { 
        DrawSubItem_StorageInfo(pMenu->iMenuId, bID0, i, pMenu->pItemsList[i]->uiStringId, TextClr, bkColor);
    }
	
    MenuDraw_BatteryStatus(bID0);
    
	MenuDraw_MediaType(bID0);
	
	OSDDraw_ExitMenuDrawing(&bID0, &bID1);

	CHARGE_ICON_ENABLE(AHC_TRUE);
}

void MenuDrawSubPage_StorageInfo(PSMENUSTRUCT pMenu)
{
	MenuDrawSubPage_SDCardInfo(pMenu);
}
